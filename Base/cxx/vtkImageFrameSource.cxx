/*=auto=========================================================================
Copyright (c) 1999 Surgical Planning Lab, Brigham and Women's Hospital
 
Direct all questions on this copyright to slicer@ai.mit.edu.
The following terms apply to all files associated with the software unless
explicitly disclaimed in individual files.   

The authors hereby grant permission to use, copy, and distribute this
software and its documentation for any NON-COMMERCIAL purpose, provided
that existing copyright notices are retained verbatim in all copies.
The authors grant permission to modify this software and its documentation 
for any NON-COMMERCIAL purpose, provided that such modifications are not 
distributed without the explicit consent of the authors and that existing
copyright notices are retained in all copies. Some of the algorithms
implemented by this software are patented, observe all applicable patent law.

IN NO EVENT SHALL THE AUTHORS OR DISTRIBUTORS BE LIABLE TO ANY PARTY FOR
DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
OF THE USE OF THIS SOFTWARE, ITS DOCUMENTATION, OR ANY DERIVATIVES THEREOF,
EVEN IF THE AUTHORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

THE AUTHORS AND DISTRIBUTORS SPECIFICALLY DISCLAIM ANY WARRANTIES, INCLUDING,
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
PARTICULAR PURPOSE, AND NON-INFRINGEMENT.  THIS SOFTWARE IS PROVIDED ON AN
'AS IS' BASIS, AND THE AUTHORS AND DISTRIBUTORS HAVE NO OBLIGATION TO PROVIDE
MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
=========================================================================auto=*/
#include <stdlib.h>
#include "vtkImageFrameSource.h"
#include "vtkObjectFactory.h"

//----------------------------------------------------------------------------
vtkImageFrameSource* vtkImageFrameSource::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkImageFrameSource");
  if(ret)
    {
    return (vtkImageFrameSource*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkImageFrameSource;
}

//----------------------------------------------------------------------------
vtkImageFrameSource::vtkImageFrameSource()
{
	this->WholeExtent[0] = this->WholeExtent[1] = this->WholeExtent[2] = 0;
	this->WholeExtent[3] = this->WholeExtent[4] = this->WholeExtent[5] = 0;
	this->SetExtent(0, 255, 0, 255);
	this->RenderWindow = NULL;
}

//----------------------------------------------------------------------------
vtkImageFrameSource::~vtkImageFrameSource()
{
  // We must UnRegister any object that has a vtkSetObjectMacro
  if (this->RenderWindow != NULL) 
  {
    this->RenderWindow->UnRegister(this);
  }
}

void vtkImageFrameSource::PrintSelf(ostream& os, vtkIndent indent)
{
	vtkImageSource::PrintSelf(os,indent);
  
	os << indent << "WholeExtent: "
		<< this->WholeExtent[1]-this->WholeExtent[0]+1 << "x"
		<< this->WholeExtent[3]-this->WholeExtent[2]+1 << "x"
		<< this->WholeExtent[5]-this->WholeExtent[4]+1 << "\n";

  // vtkSetObjectMacro
  os << indent << "RenderWindow: " << this->RenderWindow << "\n";
  if (this->RenderWindow)
  {
    this->RenderWindow->PrintSelf(os,indent.GetNextIndent());
  }
}

//----------------------------------------------------------------------------
void vtkImageFrameSource::SetExtent(int xMin, int xMax, int yMin, int yMax)
{
	if (this->WholeExtent[0] != xMin || this->WholeExtent[1] != xMax ||
		this->WholeExtent[2] != yMin || this->WholeExtent[2] != yMax)
	{
		this->WholeExtent[0] = xMin;
		this->WholeExtent[1] = xMax;
		this->WholeExtent[2] = yMin;
		this->WholeExtent[3] = yMax;
		this->WholeExtent[4] = 0;
		this->WholeExtent[5] = 0;
		this->Modified();
	}
}

//----------------------------------------------------------------------------
void vtkImageFrameSource::UpdateInformation()
{
  vtkImageData *output = this->GetOutput();
  unsigned long mem;
 
  output->SetWholeExtent(this->WholeExtent);
  output->SetScalarType(VTK_UNSIGNED_CHAR);
  output->SetNumberOfScalarComponents(3);
 
  // What if we are trying to process a VERY large 2D image?
  mem = output->GetScalarSize();
  mem = mem * (this->WholeExtent[1] - this->WholeExtent[0] + 1);
  mem = mem * (this->WholeExtent[3] - this->WholeExtent[2] + 1);
  mem = mem / 1000;
  mem = mem * (this->WholeExtent[5] - this->WholeExtent[4] + 1);
  if (mem < 1)
    {
    mem = 1;
    }
 
  output->SetEstimatedWholeMemorySize(mem);
}

//----------------------------------------------------------------------------
void vtkImageFrameSource::Execute(vtkImageData *data)
{
	int x1, y1, x2, y2, i, idxX, idxY, idxZ, w, h;
	int wOut, hOut, wIn, hIn, nc, nb;
	int outIncX, outIncY, outIncZ, rowLength;
	int *size;
	int *outExt;
	unsigned char *frame, *inPtr, *outPtr;
  
	if (this->RenderWindow == NULL) {
		vtkErrorMacro("Execute: This source requires a vtkRenderWindow.");
		return;
	}
	if (data->GetScalarType() != VTK_UNSIGNED_CHAR) {
		vtkErrorMacro("Execute: This source outputs unsigned char data not #"
			<< data->GetScalarType());
		return;
	}
  
	// find the region to loop over
	outExt = data->GetExtent();
	hOut = outExt[3] - outExt[2] + 1;
	wOut = outExt[1] - outExt[0] + 1;
	nc = data->GetNumberOfScalarComponents();
  
	// Get increments to march through data 
	data->GetContinuousIncrements(outExt, outIncX, outIncY, outIncZ);
	outPtr = (unsigned char*) data->GetScalarPointer();

	// Get Frame buffer from center of render window
	size = this->RenderWindow->GetSize();
	wIn = size[0];
	hIn = size[1];

	// If input is smaller than output, then the output will need to
	// be zero padded.
	// If input is larger than output, then take the output from the
	// center of the input.

	// Input smaller
	if (wIn < wOut) {
		x1 = 0;
		x2 = wIn-1;
	} 
	// Input larger
	else {
		x1 = wIn/2 - wOut/2;
		x2 = x1 + wOut - 1; 
	}

	// Input smaller
	if (hIn < hOut) {
		y1 = 0;
		y2 = hIn-1;
	}
	// Input larger
	else {
		y1 = hIn/2 - hOut/2;
		y2 = y1 + hOut - 1; 
	}

	w = x2 - x1 + 1;
	h = y2 - y1 + 1;

	// Allocate and return frame data
	frame = this->RenderWindow->GetPixelData(x1, y1, x2, y2, 1);
	inPtr = frame;
	nb = sizeof(unsigned char)*nc;

	// March thru output pixels.
	// Wherever the output is outside the input, place 0's.
	// Else, place input.
	for (idxY = 0; idxY < hOut; idxY++)
	{
		for (idxX = 0; idxX < wOut; idxX++)
		{
			if (idxX < w && idxY < h)
			{
				memcpy(outPtr, inPtr, nb);
				outPtr += nc;
				inPtr += nc;
			} 
			else {
				memset(outPtr, 0, nb);
				outPtr += nc;
			}
		}
	}

	// I am responsible for deleting the buffer created by the RenderWindow
	delete [] frame;
	frame = NULL;
	inPtr = NULL;
}


/*=auto=========================================================================

(c) Copyright 2001 Massachusetts Institute of Technology

Permission is hereby granted, without payment, to copy, modify, display 
and distribute this software and its documentation, if any, for any purpose, 
provided that the above copyright notice and the following three paragraphs 
appear on all copies of this software.  Use of this software constitutes 
acceptance of these terms and conditions.

IN NO EVENT SHALL MIT BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, 
INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE 
AND ITS DOCUMENTATION, EVEN IF MIT HAS BEEN ADVISED OF THE POSSIBILITY OF 
SUCH DAMAGE.

MIT SPECIFICALLY DISCLAIMS ANY EXPRESS OR IMPLIED WARRANTIES INCLUDING, 
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR 
A PARTICULAR PURPOSE, AND NON-INFRINGEMENT.

THE SOFTWARE IS PROVIDED "AS IS."  MIT HAS NO OBLIGATION TO PROVIDE 
MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

=========================================================================auto=*/
#include "vtkImageLiveWireTester.h"
#include "vtkObjectFactory.h"
#include "vtkImageLiveWireEdgeWeights.h"

//------------------------------------------------------------------------------
vtkImageLiveWireTester* vtkImageLiveWireTester::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkImageLiveWireTester");
  if(ret)
    {
    return (vtkImageLiveWireTester*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkImageLiveWireTester;
}

//----------------------------------------------------------------------------
// Description:
// Constructor sets default values.
vtkImageLiveWireTester::vtkImageLiveWireTester()
{
  // must be set by user
  this->LiveWire = NULL;
  
  this->NumberOfEdgeFilters = 4;
  
  this->EdgeFilters = new vtkImageLiveWireEdgeWeights*[this->NumberOfEdgeFilters];

  for (int i = 0; i < this->NumberOfEdgeFilters; i++)
    {
      this->EdgeFilters[i] = vtkImageLiveWireEdgeWeights::New();
      this->EdgeFilters[i]->SetEdgeDirection(i);
    }

}

//----------------------------------------------------------------------------
vtkImageLiveWireTester::~vtkImageLiveWireTester()
{
  if (this->LiveWire)
    {
      this->LiveWire->Delete();
    }
  
  if (this->EdgeFilters)
    {
      for (int i = 0; i < this->NumberOfEdgeFilters; i++)
	{
	  if (this->EdgeFilters[i])
	    {
	      this->EdgeFilters[i]->Delete();
	    }
	}
    }
  
  delete [] this->EdgeFilters;

  // We must UnRegister any object that has a vtkSetObjectMacro
  if (this->LiveWire != NULL) 
  {
    this->LiveWire->UnRegister(this);
  }

}


//----------------------------------------------------------------------------
// Description:
// Set multiplier used to weight importance of pixel difference in edge
// weight computation.
void vtkImageLiveWireTester::SetDifference(float val)
{
  for (int i = 0; i < this->NumberOfEdgeFilters; i++)
    {
      this->EdgeFilters[i]->SetDifference(val);
    }
}

//----------------------------------------------------------------------------
// Description:
// Set multiplier used to weight importance of pixel difference in edge
// weight computation.
void vtkImageLiveWireTester::SetInsidePixel(float val)
{

  for (int i = 0; i < this->NumberOfEdgeFilters; i++)
    {
      this->EdgeFilters[i]->SetInsidePixel(val);
    }
}

//----------------------------------------------------------------------------
// Description:
// Set multiplier used to weight importance of pixel difference in edge
// weight computation.
void vtkImageLiveWireTester::SetOutsidePixel(float val)
{

  for (int i = 0; i < this->NumberOfEdgeFilters; i++)
    {
      this->EdgeFilters[i]->SetOutsidePixel(val);
    }
}


//----------------------------------------------------------------------------
// Description:
// return output of edge filter
vtkImageData *vtkImageLiveWireTester::GetEdgeImage(int filter)
{
  if (filter < this->NumberOfEdgeFilters)
    {
      return this->EdgeFilters[filter]->GetOutput();
    }
  else
    {
      vtkErrorMacro(<<"Requested filter " << filter << " greater than number of filters!");
    }
}

//----------------------------------------------------------------------------
// Description:
// Pseudo-multiple-output filter.  Feeds 4 edge images to LiveWire, which
// should be next in pipeline and get this filter's output image too.
// So LiveWire will get: 
// input[0] : output of this filter (just matches filter's input)
// input[1] - input[4]: edges
//
static void vtkImageLiveWireTesterExecute(vtkImageLiveWireTester *self,
				     vtkImageData *inData, short *inPtr,
				     vtkImageData *outData, short *outPtr, 
				     int outExt[6])
{
  if (!self->GetLiveWire())
    {
      cout << "ERROR in vtkImageLiveWire??: vtkImageLiveWire member not set."<< endl;
      return;
    }

  vtkImageLiveWire *liveWire = self->GetLiveWire();
  int numEdges = self->GetNumberOfEdgeFilters();
  vtkImageLiveWireEdgeWeights ** edgeFilters = self->GetEdgeFilters();

  // Lauren max edge cost of edge filters and live wire need to match

  // set up pipeline for live wire.
  // Pipeline stuff should only happen once!
  // make edge weight images
  for (int i = 0; i < numEdges; i++)
    {
      edgeFilters[i]->SetOriginalImage(inData);
      // Lauren this sets precision.
      //edgeFilters[i]->SetMaxEdgeWeight();
      edgeFilters[i]->Update();
    }

  liveWire->SetUpEdges(edgeFilters[0]->GetOutput());
  liveWire->SetDownEdges(edgeFilters[1]->GetOutput());
  liveWire->SetLeftEdges(edgeFilters[2]->GetOutput());
  liveWire->SetRightEdges(edgeFilters[3]->GetOutput());

  // Output is the same as input
  outData->CopyAndCastFrom(inData, inData->GetExtent());  

  return;
}


//----------------------------------------------------------------------------
// Description:
// This method is passed a input and output data, and executes the filter
// algorithm to fill the output from the input.
// It just executes a switch statement to call the correct function for
// the datas data types.
void vtkImageLiveWireTester::Execute(vtkImageData *inData, vtkImageData *outData)
{
  int outExt[6], id=0, s;
  outData->GetWholeExtent(outExt);
  void *inPtr = inData->GetScalarPointerForExtent(outExt);
  void *outPtr = outData->GetScalarPointerForExtent(outExt);

  int x1;

  x1 = GetInput()->GetNumberOfScalarComponents();
  if (x1 != 1) 
  {
    vtkErrorMacro(<<"Input has "<<x1<<" instead of 1 scalar component.");
    return;
  }

  /* Need short data */
  s = inData->GetScalarType();
  if (s != VTK_SHORT) 
  {
    vtkErrorMacro("Input scalars are type "<<s 
      << " instead of "<<VTK_SHORT);
    return;
  }

  vtkImageLiveWireTesterExecute(this, inData, (short *)inPtr, 
          outData, (short *)(outPtr), outExt);
}

void vtkImageLiveWireTester::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkImageToImageFilter::PrintSelf(os,indent);
  

}

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
#include "vtkImageLabelOutline.h"
#include "vtkObjectFactory.h"

//-----------------------------------------------------------------------------

vtkImageLabelOutline* vtkImageLabelOutline::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkImageLabelOutline");
  if(ret)
    {
    return (vtkImageLabelOutline*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkImageLabelOutline;
}


//----------------------------------------------------------------------------


// Description:
// Constructor sets default values
vtkImageLabelOutline::vtkImageLabelOutline()
{
  
	this->mask = NULL;
	this->SetOutline(1);
	this->Background = 0;
	this->HandleBoundaries = 1;
	this->SetKernelSize(3,3,3);
	this->SetNeighborTo8();
	
}


//----------------------------------------------------------------------------
vtkImageLabelOutline::~vtkImageLabelOutline()
{

  if (this->mask != NULL)
    {
      delete [] this->mask;
    }

}
//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
// This method sets the size of the neighborhood and the default middle of the
// neighborhood.  Also sets the size of (allocates) the matching mask.
void vtkImageLabelOutline::SetKernelSize(int size0, int size1, int size2)
{
  int modified = 0;
  
  if (this->KernelSize[0] != size0)
    {
    modified = 1;
    this->KernelSize[0] = size0;
    this->KernelMiddle[0] = size0 / 2;
    }
  if (this->KernelSize[1] != size1)
    {
    modified = 1;
    this->KernelSize[1] = size1;
    this->KernelMiddle[1] = size1 / 2;
    }
  if (this->KernelSize[2] != size2)
    {
    modified = 1;
    this->KernelSize[2] = size2;
    this->KernelMiddle[2] = size2 / 2;
    }


  if (modified)
    {
      if (this->mask != NULL)
	{
	  delete [] this->mask;
	}
      this->mask = new unsigned char[this->KernelSize[0]*
				    this->KernelSize[1]*this->KernelSize[2]];

      this->Modified();
    }
}



//----------------------------------------------------------------------------
void vtkImageLabelOutline::SetNeighborTo4()
{
	int x, y, z;
	
	this->Neighbor = 4;

	// clear
	memset(this->mask, 0, this->KernelSize[0]*this->KernelSize[1]*
	       this->KernelSize[2]);

	// set
	z = 0;
	for (y=-1; y <= 1; y++)
		for (x=-1; x <= 1; x++)
			if (x*y == 0)
				mask[(1+z)*9+(1+y)*3+(1+x)] = 1;

	mask[1*9+1*3+1] = 0;
	mask[0*9+1*3+1] = 1;
	mask[2*9+1*3+1] = 1;

	this->Modified();
}
//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
void vtkImageLabelOutline::SetNeighborTo8()
{
	this->Neighbor = 8;

	// set
	memset(this->mask, 1, this->KernelSize[0]*this->KernelSize[1]*
	       this->KernelSize[2]);

	mask[1*9+1*3+1] = 0;

	this->Modified();
}

// Description:
// This templated function executes the filter for any type of data.
template <class T>
static void vtkImageLabelOutlineExecute(vtkImageLabelOutline *self,
				     vtkImageData *inData, T *inPtr,
				     vtkImageData *outData,
				     int outExt[6], int id)
{
	int *kernelMiddle, *kernelSize;
	// For looping though output (and input) pixels.
	int outMin0, outMax0, outMin1, outMax1, outMin2, outMax2;
	int outIdx0, outIdx1, outIdx2;
	int inInc0, inInc1, inInc2;
	int outInc0, outInc1, outInc2;
	T *inPtr0, *inPtr1, *inPtr2;
	T *outPtr0, *outPtr1, *outPtr2;
	// For looping through hood pixels
	int hoodMin0, hoodMax0, hoodMin1, hoodMax1, hoodMin2, hoodMax2;
	int hoodIdx0, hoodIdx1, hoodIdx2;
	T *hoodPtr0, *hoodPtr1, *hoodPtr2;
	// For looping through the mask.
	unsigned char *maskPtr, *maskPtr0, *maskPtr1, *maskPtr2;
	int maskInc0, maskInc1, maskInc2;
	// The extent of the whole input image
	int inImageMin0, inImageMin1, inImageMin2;
	int inImageMax0, inImageMax1, inImageMax2;
	// Other
	T backgnd = (T)(self->GetBackground());
	T pix;
	T *outPtr = (T*)outData->GetScalarPointerForExtent(outExt);
	unsigned long count = 0;
	unsigned long target;


	// Get information to march through data
	inData->GetIncrements(inInc0, inInc1, inInc2); 
	self->GetInput()->GetWholeExtent(inImageMin0, inImageMax0, inImageMin1,
		inImageMax1, inImageMin2, inImageMax2);
	outData->GetIncrements(outInc0, outInc1, outInc2); 
	outMin0 = outExt[0];   outMax0 = outExt[1];
	outMin1 = outExt[2];   outMax1 = outExt[3];
	outMin2 = outExt[4];   outMax2 = outExt[5];

	// Neighborhood around current pixel (kernel has radius 1)
	kernelSize = self->GetKernelSize();
	kernelMiddle = self->GetKernelMiddle();
	hoodMin0 = - kernelMiddle[0];
	hoodMin1 = - kernelMiddle[1];
	hoodMin2 = - kernelMiddle[2];
	hoodMax0 = hoodMin0 + kernelSize[0] - 1;
	hoodMax1 = hoodMin1 + kernelSize[1] - 1;
	hoodMax2 = hoodMin2 + kernelSize[2] - 1;

	// Setup mask info
	maskPtr = (unsigned char *)(self->GetMaskPointer());
	maskInc0 = kernelSize[0];
	maskInc1 = kernelSize[1];
	maskInc2 = kernelSize[2];

	// in and out should be marching through corresponding pixels.
	inPtr = (T *)(inData->GetScalarPointer(outMin0, outMin1, outMin2));
 
	target = (unsigned long)((outMax2-outMin2+1)*(outMax1-outMin1+1)/50.0);
	target++;

	// loop through pixels of output
 	outPtr2 = outPtr;
	inPtr2 = inPtr;
	for (outIdx2 = outMin2; outIdx2 <= outMax2; outIdx2++)
	{
	outPtr1 = outPtr2;
	inPtr1 = inPtr2;
	for (outIdx1 = outMin1; 
	     !self->AbortExecute && outIdx1 <= outMax1; outIdx1++)
	{
	if (!id) 
	  {
	  if (!(count%target))
	    {
	    self->UpdateProgress(count/(50.0*target));
	    }
	  count++;
	  }
	outPtr0 = outPtr1;
	inPtr0 = inPtr1;
	for (outIdx0 = outMin0; outIdx0 <= outMax0; outIdx0++)
	{
		pix = *inPtr0;
		// Default output equal to backgnd
		*outPtr0 = backgnd;

		if (pix != backgnd)
		{
			// Loop through neighborhood pixels (kernel radius=1)
			// Note: input pointer marches out of bounds.
			hoodPtr2 = inPtr0 - inInc0 - inInc1 - inInc2;
			maskPtr2 = maskPtr;
			for (hoodIdx2 = hoodMin2; hoodIdx2 <= hoodMax2; ++hoodIdx2)
			{
			hoodPtr1 = hoodPtr2;
			maskPtr1 = maskPtr2;
			for (hoodIdx1 = hoodMin1; hoodIdx1 <= hoodMax1;	++hoodIdx1)
			{
			hoodPtr0 = hoodPtr1;
			maskPtr0 = maskPtr1;
			for (hoodIdx0 = hoodMin0; hoodIdx0 <= hoodMax0; ++hoodIdx0)
			{
				if (*maskPtr0)
        {
          // handle boundaries
				  if (outIdx0 + hoodIdx0 >= inImageMin0 &&
					  outIdx0 + hoodIdx0 <= inImageMax0 &&
					  outIdx1 + hoodIdx1 >= inImageMin1 &&
					  outIdx1 + hoodIdx1 <= inImageMax1 &&
					  outIdx2 + hoodIdx2 >= inImageMin2 &&
					  outIdx2 + hoodIdx2 <= inImageMax2)
				  {
					  // If the neighbor not identical, use this pixel
					  // set the output to backgnd
					  if (*hoodPtr0 != pix)
					  	*outPtr0 = pix;
          }
				}
				hoodPtr0 += inInc0;
				maskPtr0 += maskInc0;
			}//for0
			hoodPtr1 += inInc1;
			maskPtr1 += maskInc1;
			}//for1
			hoodPtr2 += inInc2;
			maskPtr2 += maskInc2;
			}//for2
		}//if
	inPtr0 += inInc0;
	outPtr0 += outInc0;
	}//for0
	inPtr1 += inInc1;
	outPtr1 += outInc1;
	}//for1
	inPtr2 += inInc2;
	outPtr2 += outInc2;
	}//for2
}

//----------------------------------------------------------------------------
// Description:
// This method is passed a input and output data, and executes the filter
// algorithm to fill the output from the input.
// It just executes a switch statement to call the correct function for
// the datas data types.
void vtkImageLabelOutline::ThreadedExecute(vtkImageData *inData, 
					vtkImageData *outData,
					int outExt[6], int id)
{	int x1;

	// Single component input is required
	x1 = this->GetInput()->GetNumberOfScalarComponents();
	if (x1 != 1) {
	vtkErrorMacro(<<"Input has "<<x1<<" instead of 1 scalar component.");
		return;
	}


	void *inPtr = inData->GetScalarPointerForExtent(outExt);
  
	switch (inData->GetScalarType())
	{
	case VTK_DOUBLE:
		vtkImageLabelOutlineExecute(this, inData, (double *)(inPtr), 
			outData, outExt, id);
		break;
	case VTK_FLOAT:
		vtkImageLabelOutlineExecute(this, inData, (float *)(inPtr), 
			outData, outExt, id);
		break;
	case VTK_LONG:
		vtkImageLabelOutlineExecute(this, inData, (long *)(inPtr), 
			outData, outExt, id);
		break;
	case VTK_UNSIGNED_LONG:
		vtkImageLabelOutlineExecute(this, inData, (unsigned long *)(inPtr), 
			outData, outExt, id);
		break;
	case VTK_INT:
		vtkImageLabelOutlineExecute(this, inData, (int *)(inPtr), 
			outData, outExt, id);
		break;
	case VTK_UNSIGNED_INT:
		vtkImageLabelOutlineExecute(this, inData, (unsigned int *)(inPtr), 
			outData, outExt, id);
		break;
	case VTK_SHORT:
		vtkImageLabelOutlineExecute(this, inData, (short *)(inPtr), 
			outData, outExt, id);
		break;
	case VTK_UNSIGNED_SHORT:
		vtkImageLabelOutlineExecute(this, inData, (unsigned short *)(inPtr), 
			outData, outExt, id);
		break;
	case VTK_CHAR:
		vtkImageLabelOutlineExecute(this, inData, (char *)(inPtr), 
			outData, outExt, id);
		break;
	case VTK_UNSIGNED_CHAR:
		vtkImageLabelOutlineExecute(this, inData, (unsigned char *)(inPtr), 
			outData, outExt, id);
		break;
	default:
		vtkErrorMacro(<< "Execute: Unknown input ScalarType");
		return;
	}
}

void vtkImageLabelOutline::PrintSelf(ostream& os, vtkIndent indent)
{
	vtkImageSpatialFilter::PrintSelf(os,indent);
  
	os << indent << "Outline: " << this->Outline << "\n";
}


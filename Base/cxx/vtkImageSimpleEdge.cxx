/*=auto=========================================================================
Copyright (c) 2000 Surgical Planning Lab, Brigham and Women's Hospital
 
Direct all questions on this copyright to slicer@ai.mit.edu.
The following terms apply to all files associated with the software unless
explicitly disclaimed in individual files.   

The authors hereby grant permission to use, copy (but NOT distribute) this
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
#include "vtkImageSimpleEdge.h"
#include "vtkObjectFactory.h"

//----------------------------------------------------------------------------
vtkImageSimpleEdge* vtkImageSimpleEdge::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkImageSimpleEdge");
  if(ret)
    {
      return (vtkImageSimpleEdge*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkImageSimpleEdge;
}


//----------------------------------------------------------------------------
// Description:
// Constructor sets default values
vtkImageSimpleEdge::vtkImageSimpleEdge()
{
  
  this->SetKernelToHorizontal();
	
}


//----------------------------------------------------------------------------
vtkImageSimpleEdge::~vtkImageSimpleEdge()
{

}

//----------------------------------------------------------------------------
void vtkImageSimpleEdge::SetKernelToHorizontal()
{
  if (this->KernelSize[0] != 2)
    {
      this->Modified();
    }

  this->KernelSize[0] = 2;
  this->KernelSize[1] = 1;
  this->KernelSize[2] = 1;
  this->KernelMiddle[0] = 0;
  this->KernelMiddle[1] = 0;
  this->KernelMiddle[2] = 0;
  this->HandleBoundaries = 1;
  
}

//----------------------------------------------------------------------------
void vtkImageSimpleEdge::SetKernelToVertical()
{
  if (this->KernelSize[1] != 2)
    {
      this->Modified();
    }

  this->KernelSize[0] = 1;
  this->KernelSize[1] = 2;
  this->KernelSize[2] = 1;
  this->KernelMiddle[0] = 0;
  this->KernelMiddle[1] = 0;
  this->KernelMiddle[2] = 0;
  this->HandleBoundaries = 1;
}


//----------------------------------------------------------------------------
// Description:
// This templated function executes the filter for any type of data.
template <class T>
static void vtkImageSimpleEdgeExecute(vtkImageSimpleEdge *self,
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
  // The extent of the whole input image
  int inImageMin0, inImageMin1, inImageMin2;
  int inImageMax0, inImageMax1, inImageMax2;
  // Other
  T *outPtr = (T*)outData->GetScalarPointerForExtent(outExt);
  unsigned long count = 0;
  unsigned long target;

  T pix;      // current pixel intensity
  T sum = 0;  // sum of pix intensities in neighborhood
  int outOfRange = 0;  // if hood over edge of image

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

  // debug
  //  cout << "in incr " << inInc0 << " " << inInc1 << " " << inInc2 << endl;
  //  cout << "out incr " << outInc0 << " " << outInc1 << " " << outInc2 << endl;
  //  cout << "output: " << outMin0 << " " << outMax0 << " " << outMin1 << " " << outMax1  << " " << outMin2 << " " << outMax2 << endl;
  //  cout << "hood: " << hoodMin0 << " " << hoodMax0  << " " << hoodMin1 << " " << hoodMax1  << " " << hoodMin2 << " " << hoodMax2 << endl;

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
	      
	      // Loop through neighborhood pixels (kernel radius=1)
	      // Note: input pointer marches out of bounds.

	      // want hood to start at the pixel itself.
	      hoodPtr2 = inPtr0;
	      sum = 0;
	      outOfRange = 0;

	      for (hoodIdx2 = hoodMin2; hoodIdx2 <= hoodMax2; ++hoodIdx2)
		{
		  hoodPtr1 = hoodPtr2;

		  for (hoodIdx1 = hoodMin1; hoodIdx1 <= hoodMax1; ++hoodIdx1)
		    {
		      hoodPtr0 = hoodPtr1;

		      for (hoodIdx0 = hoodMin0; hoodIdx0 <= hoodMax0; ++hoodIdx0)
			{			  
			  // handle boundaries
			  // test 256 boundary pixels fail this test.
			  // need to set their edge weights to something 
			  // large to stop segmentation at img. edge
			  if (outIdx0 + hoodIdx0 >= inImageMin0 &&
			      outIdx0 + hoodIdx0 <= inImageMax0 &&
			      outIdx1 + hoodIdx1 >= inImageMin1 &&
			      outIdx1 + hoodIdx1 <= inImageMax1 &&
			      outIdx2 + hoodIdx2 >= inImageMin2 &&
			      outIdx2 + hoodIdx2 <= inImageMax2)
			    {
			      sum += *hoodPtr0;
			    }
			  else
			    {
			      outOfRange = 1;
			    }

			  hoodPtr0 += inInc0;

			}//for0
		      hoodPtr1 += inInc1;
		      
		    }//for1
		  hoodPtr2 += inInc2;
		  
		}//for2  (end hood loop)
	      

	      // set output to difference between pixels
	      if (!outOfRange)
		{
		  // Lauren this should be fabs if double/float...
		  // decide and set output of this filter to one type.
		  // or should be directional w/ out the abs...
		  *outPtr0 = abs((int)(sum - 2*pix));
		  // Lauren don't hard code this
		  if (*outPtr0 > 255) *outPtr0 = 255;
		}
	      else 
		{
		  // set to MAX so it's a heavy edge
		  *outPtr0 = 10;
		}

	      
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
void vtkImageSimpleEdge::ThreadedExecute(vtkImageData *inData, 
					vtkImageData *outData,
					int outExt[6], int id)
{
  int x1;

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
      vtkImageSimpleEdgeExecute(this, inData, (double *)(inPtr), 
				outData, outExt, id);
      break;
    case VTK_FLOAT:
      vtkImageSimpleEdgeExecute(this, inData, (float *)(inPtr), 
				outData, outExt, id);
      break;
    case VTK_LONG:
      vtkImageSimpleEdgeExecute(this, inData, (long *)(inPtr), 
				outData, outExt, id);
      break;
    case VTK_UNSIGNED_LONG:
      vtkImageSimpleEdgeExecute(this, inData, (unsigned long *)(inPtr), 
				outData, outExt, id);
      break;
    case VTK_INT:
      vtkImageSimpleEdgeExecute(this, inData, (int *)(inPtr), 
				outData, outExt, id);
      break;
    case VTK_UNSIGNED_INT:
      vtkImageSimpleEdgeExecute(this, inData, (unsigned int *)(inPtr), 
				outData, outExt, id);
      break;
    case VTK_SHORT:
      vtkImageSimpleEdgeExecute(this, inData, (short *)(inPtr), 
				outData, outExt, id);
      break;
    case VTK_UNSIGNED_SHORT:
      vtkImageSimpleEdgeExecute(this, inData, (unsigned short *)(inPtr), 
				outData, outExt, id);
      break;
    case VTK_CHAR:
      vtkImageSimpleEdgeExecute(this, inData, (char *)(inPtr), 
				outData, outExt, id);
      break;
    case VTK_UNSIGNED_CHAR:
      vtkImageSimpleEdgeExecute(this, inData, (unsigned char *)(inPtr), 
				outData, outExt, id);
      break;
    default:
      vtkErrorMacro(<< "Execute: Unknown input ScalarType");
      return;
    }
}

void vtkImageSimpleEdge::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkImageSpatialFilter::PrintSelf(os,indent);
  
  os << indent << "Outline: " << this->Outline << "\n";
}


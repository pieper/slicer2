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
#include "vtkImageAccumulateDiscrete.h"
#include <math.h>
#include <stdlib.h>
#include "vtkObjectFactory.h"

//------------------------------------------------------------------------------
vtkImageAccumulateDiscrete* vtkImageAccumulateDiscrete::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkImageAccumulateDiscrete");
  if(ret)
    {
    return (vtkImageAccumulateDiscrete*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkImageAccumulateDiscrete;
}

//----------------------------------------------------------------------------
// Constructor sets default values
vtkImageAccumulateDiscrete::vtkImageAccumulateDiscrete()
{
}

//----------------------------------------------------------------------------
void vtkImageAccumulateDiscrete::ExecuteInformation(vtkImageData *vtkNotUsed(input), 
					    vtkImageData *output)
{
  int ext[6];
  memset(ext, 0, 6*sizeof(int));
  ext[1] = 65535;

  float origin[3], spacing[1];
  spacing[0] = spacing[1] = spacing[2] = 1;
  origin[0] = -32768;
  origin[1] = origin[2] = 0;

  output->SetWholeExtent(ext);
  output->SetOrigin(origin);
  output->SetSpacing(spacing);
  output->SetNumberOfScalarComponents(1);
  output->SetScalarType(VTK_INT);
}

//----------------------------------------------------------------------------
// Get ALL of the input.
void vtkImageAccumulateDiscrete::ComputeInputUpdateExtent(int inExt[6], 
							  int outExt[6])
{
  int *wholeExtent;

  outExt = outExt;
  wholeExtent = this->GetInput()->GetWholeExtent();
  memcpy(inExt, wholeExtent, 6*sizeof(int));
}

//----------------------------------------------------------------------------
void vtkImageAccumulateDiscrete::EnlargeOutputUpdateExtents(vtkDataObject *vtkNotUsed(data) )
{
  int wholeExtent[8];
  
  this->GetOutput()->GetWholeExtent(wholeExtent);
  this->GetOutput()->SetUpdateExtent(wholeExtent);
}

//----------------------------------------------------------------------------
// This templated function executes the filter for any type of data.
template <class T>
static void vtkImageAccumulateDiscreteExecute(vtkImageAccumulateDiscrete *self,
				      vtkImageData *inData, T *inPtr,
				      vtkImageData *outData, int *outPtr)
{
  int min0, max0, min1, max1, min2, max2;
  int idx0, idx1, idx2;
  int inInc0, inInc1, inInc2;
  T *inPtr0, *inPtr1, *inPtr2;
  float *origin, *spacing;
  int numC, offset;
  int outExt[6];
  unsigned long count = 0;
  unsigned long target;

  // Zero count in every bin
  outData->GetExtent(min0, max0, min1, max1, min2, max2);
  memset((void *)outPtr, 0, 
	 (max0-min0+1)*(max1-min1+1)*(max2-min2+1)*sizeof(int));
    
  // Get information to march through data 
  numC = inData->GetNumberOfScalarComponents();
  inData->GetExtent(min0, max0, min1, max1, min2, max2);
  inData->GetIncrements(inInc0, inInc1, inInc2);
  outData->GetExtent(outExt);
  origin = outData->GetOrigin();
  spacing = outData->GetSpacing();
  offset = (int)(-origin[0]);

  // Ignore all components other than first one.
  // NOTE: GetIncrements takes numC into account

  target = (unsigned long)((max2 - min2 + 1)*(max1 - min1 +1)/50.0);
  target++;

  inPtr2 = inPtr;
  for (idx2 = min2; idx2 <= max2; ++idx2)
  {
    inPtr1 = inPtr2;
    for (idx1 = min1; !self->AbortExecute && idx1 <= max1; ++idx1)
    {
      if (!(count%target))
      {
        self->UpdateProgress(count/(50.0*target));
      }
      count++;
      inPtr0  = inPtr1;
      for (idx0 = min0; idx0 <= max0; ++idx0)
      {
	      outPtr[(int)(*inPtr0) + offset]++;
	      inPtr0 += inInc0;
	    }
      inPtr1 += inInc1;
    }
    inPtr2 += inInc2;
  }
}

	

//----------------------------------------------------------------------------
// This method is passed a input and output Data, and executes the filter
// algorithm to fill the output from the input.
// It just executes a switch statement to call the correct function for
// the Datas data types.
void vtkImageAccumulateDiscrete::Execute(vtkImageData *inData, 
				 vtkImageData *outData)
{
  void *inPtr;
  int *outPtr;
  
  inPtr  = inData->GetScalarPointer();
  outPtr = (int *)outData->GetScalarPointer();
  
  // this filter expects that output is type int.
  if (outData->GetScalarType() != VTK_INT)
  {
    vtkErrorMacro(<< "Execute: out ScalarType " << outData->GetScalarType()
		  << " must be int\n");
    return;
  }
  
  switch (inData->GetScalarType())
  {
    case VTK_CHAR:
      vtkImageAccumulateDiscreteExecute(this, 
			  inData, (char *)(inPtr), outData, outPtr);
      break;
    case VTK_UNSIGNED_CHAR:
      vtkImageAccumulateDiscreteExecute(this, 
			  inData, (unsigned char *)(inPtr), outData, outPtr);
      break;
    case VTK_SHORT:
      vtkImageAccumulateDiscreteExecute(this, 
			  inData, (short *)(inPtr), outData, outPtr);
      break;
    case VTK_UNSIGNED_SHORT:
      vtkImageAccumulateDiscreteExecute(this, 
			  inData, (unsigned short *)(inPtr), outData, outPtr);
      break;
    case VTK_INT:
      vtkImageAccumulateDiscreteExecute(this, 
			  inData, (int *)(inPtr), outData, outPtr);
      break;
    case VTK_UNSIGNED_INT:
      vtkImageAccumulateDiscreteExecute(this, 
			  inData, (unsigned int *)(inPtr), outData, outPtr);
      break;
    case VTK_LONG:
      vtkImageAccumulateDiscreteExecute(this, 
			  inData, (long *)(inPtr), outData, outPtr);
      break;
    case VTK_UNSIGNED_LONG:
      vtkImageAccumulateDiscreteExecute(this, 
			  inData, (unsigned long *)(inPtr), outData, outPtr);
      break;
    case VTK_FLOAT:
      vtkImageAccumulateDiscreteExecute(this, 
			  inData, (float *)(inPtr), outData, outPtr);
      break;
    case VTK_DOUBLE:
      vtkImageAccumulateDiscreteExecute(this, 
			  inData, (double *)(inPtr), outData, outPtr);
      break;
    default:
      vtkErrorMacro(<< "Execute: Unsupported ScalarType");
      return;
    }
}


void vtkImageAccumulateDiscrete::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkImageToImageFilter::PrintSelf(os,indent);

}


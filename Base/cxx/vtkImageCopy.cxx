/*=auto=========================================================================
Copyright (c) 2001 Surgical Planning Lab, Brigham and Women's Hospital
 
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
#include "vtkImageCopy.h"
#include "vtkObjectFactory.h"


//----------------------------------------------------------------------------
vtkImageCopy* vtkImageCopy::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkImageCopy");
  if(ret)
    {
    return (vtkImageCopy*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkImageCopy;
}

//----------------------------------------------------------------------------

vtkImageCopy::vtkImageCopy()
{
  this->Clear = 0;
}

//----------------------------------------------------------------------------

template <class T>
static void vtkImageCopyExecute(vtkImageCopy *self,
				     vtkImageData *inData, T *inPtr,
				     vtkImageData *outData, T *outPtr, 
				     int outExt[6], int id)
{
  int rowLength, rowSize, size;
  int inIncX, inIncY, inIncZ;
  int outIncX, outIncY, outIncZ;
  int idxY, idxZ, maxY, maxZ;
  
  rowLength = (outExt[1] - outExt[0]+1)*inData->GetNumberOfScalarComponents();
  size = inData->GetScalarSize();
  rowSize = rowLength * size;
  maxY = outExt[3] - outExt[2]; 
  maxZ = outExt[5] - outExt[4];
  
  // Get increments to march through data 
  inData->GetContinuousIncrements(outExt, inIncX, inIncY, inIncZ);
  outData->GetContinuousIncrements(outExt, outIncX, outIncY, outIncZ);
  
  // adjust increments for this loop
  inIncY = inIncY + rowLength;
  outIncY = outIncY + rowLength;
  inIncZ *= size;
  outIncZ *= size;
  
  // Loop through ouput pixels
  if (self->GetClear())
  {
    for (idxZ = 0; idxZ <= maxZ; idxZ++)
    {
      for (idxY = 0; idxY <= maxY; idxY++)
      {
        memset(outPtr, 0, rowSize);
        outPtr += outIncY;
      }
      outPtr += outIncZ;
    }
  }
  else
  {
    for (idxZ = 0; idxZ <= maxZ; idxZ++)
    {
      for (idxY = 0; idxY <= maxY; idxY++)
      {
        memcpy(outPtr, inPtr, rowSize);
        outPtr += outIncY;
        inPtr += inIncY;
      }
      outPtr += outIncZ;
      inPtr += inIncZ;
    }
  }
}
//----------------------------------------------------------------------------
// This method is passed a input and output data, and executes the filter
// algorithm to fill the output from the input.
// It just executes a switch statement to call the correct function for
// the datas data types.
void vtkImageCopy::ThreadedExecute(vtkImageData *inData, 
					vtkImageData *outData,
					int outExt[6], int id)
{
  void *inPtr = inData->GetScalarPointerForExtent(outExt);
  void *outPtr = outData->GetScalarPointerForExtent(outExt);
 
  switch (inData->GetScalarType())
    {
    case VTK_FLOAT:
      vtkImageCopyExecute(this, inData, (float *)(inPtr), 
			       outData, (float *)(outPtr), outExt, id);
      break;
    case VTK_DOUBLE:
      vtkImageCopyExecute(this, inData, (double *)(inPtr), 
			       outData, (double *)(outPtr), outExt, id);
      break;
    case VTK_INT:
      vtkImageCopyExecute(this, inData, (int *)(inPtr), 
			       outData, (int *)(outPtr), outExt, id);
      break;
    case VTK_SHORT:
      vtkImageCopyExecute(this, inData, (short *)(inPtr), 
			       outData, (short *)(outPtr), outExt, id);
      break;
    case VTK_UNSIGNED_SHORT:
      vtkImageCopyExecute(this, inData, (unsigned short *)(inPtr), 
			       outData, (unsigned short *)(outPtr), outExt, id);
      break;
    case VTK_UNSIGNED_CHAR:
      vtkImageCopyExecute(this, inData, (unsigned char *)(inPtr), 
			       outData, (unsigned char *)(outPtr), outExt, id);
      break;
    case VTK_CHAR:
      vtkImageCopyExecute(this, inData, (char *)(inPtr), 
			       outData, (char *)(outPtr), outExt, id);
      break;
    case VTK_LONG:
      vtkImageCopyExecute(this, inData, (long *)(inPtr), 
			       outData, (long *)(outPtr), outExt, id);
      break;
    case VTK_UNSIGNED_LONG:
      vtkImageCopyExecute(this, inData, (unsigned long *)(inPtr), 
			       outData, (unsigned long *)(outPtr), outExt, id);
      break;
    case VTK_UNSIGNED_INT:
      vtkImageCopyExecute(this, inData, (unsigned int *)(inPtr), 
			       outData, (unsigned int *)(outPtr), outExt, id);
      break;

    default:
      vtkErrorMacro(<< "Execute: Unknown input ScalarType");
      return;
    }
}

void vtkImageCopy::PrintSelf(ostream& os, vtkIndent indent)
{

	vtkImageToImageFilter::PrintSelf(os,indent);
	os << indent << "Clear: " << this->Clear;
}

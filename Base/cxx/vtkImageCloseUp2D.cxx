/*=auto=========================================================================
Copyright (c) 1999 Surgical Planning Lab, Brigham and Women's Hospital
 
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
/*=============================================================================
Copyright (c) 1999 Surgical Planning Lab, Brigham and Women's Hospital
 
Direct all questions on this copyright to gering@ai.mit.edu.
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
=============================================================================*/
#include "vtkImageCloseUp2D.h"
#include "vtkObjectFactory.h"

//------------------------------------------------------------------------------

vtkImageCloseUp2D* vtkImageCloseUp2D::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkImageCloseUp2D");
  if(ret)
    {
    return (vtkImageCloseUp2D*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkImageCloseUp2D;
}


//----------------------------------------------------------------------------
// Description:
// Constructor sets default values
vtkImageCloseUp2D::vtkImageCloseUp2D()
{
	this->X = 0;
	this->Y = 0;
	this->Radius = 0;
	this->Magnification = 1;
}

//----------------------------------------------------------------------------

void vtkImageCloseUp2D::ExecuteInformation(vtkImageData *inData, 
					   vtkImageData *outData)
{
	int ext[6];
	int x1;

	inData->GetWholeExtent(ext);

	// Set output to 2D, size specified by user
	ext[1] = ext[3] = this->Magnification * (this->Radius * 2 + 1) - 1;
	ext[0] = ext[2] = ext[5] = ext[4] = 0;
	outData->SetWholeExtent(ext);
}

//----------------------------------------------------------------------------

void vtkImageCloseUp2D::ComputeRequiredInputUpdateExtent(int inExt[6], int outExt[6])

{
	// Use full input extent
	this->GetInput()->GetWholeExtent(inExt);
}

//----------------------------------------------------------------------------
// Description:
// This templated function executes the filter for any type of data.
template <class T>
static void vtkImageCloseUp2DExecute(vtkImageCloseUp2D *self,
				     vtkImageData *inData, T *inPtr,
				     vtkImageData *outData, T* outPtr, 
				     int outExt[6], int id)
{
	int idxX, maxX, inRowLength, idxY, maxY, idxC;
	int outIncX, outIncY, outIncZ;
	int scalarSize, numComps, inX, inY, inMaxX, inMaxY;
	T *ptr;
	int *inExt = inData->GetExtent();
	int mag = self->Magnification;
	int offsetX = self->X - self->Radius;
	int offsetY = self->Y - self->Radius;

	// find the region to loop over
	numComps = inData->GetNumberOfScalarComponents();
	inMaxX = inExt[1] - inExt[0]; 
	inMaxY = inExt[3] - inExt[2]; 
	inRowLength = (inMaxX+1)*numComps;
	maxX = outExt[1]; 
	maxY = outExt[3];
	scalarSize = numComps*sizeof(T);

	// Get increments to march through data 
	outData->GetContinuousIncrements(outExt, outIncX, outIncY, outIncZ);

	// Loop through output pixels
	for (idxY = outExt[2]; idxY <= maxY; idxY++)
	{
		for (idxX = outExt[0]; idxX <= maxX; idxX++)
		{
			inX = idxX / mag + offsetX;
			inY = idxY / mag + offsetY;

			if (inX >= 0 && inX <= inMaxX && inY >= 0 && inY <= inMaxY)
			{
				ptr = &inPtr[inY*inRowLength + inX*numComps];
				memcpy(outPtr, ptr, scalarSize);
			}
			else {
				memset(outPtr, 0, scalarSize);
			}
			outPtr += numComps;
		}
		outPtr += outIncY;
	}
}


//----------------------------------------------------------------------------
// Description:
// This method is passed a input and output data, and executes the filter
// algorithm to fill the output from the input.
// It just executes a switch statement to call the correct function for
// the datas data types.

void vtkImageCloseUp2D::ThreadedExecute(vtkImageData *inData, 
					vtkImageData *outData,
					int outExt[6], int id)
{
	int *inExt = inData->GetExtent();
	void *inPtr = inData->GetScalarPointerForExtent(inExt);
	void *outPtr = outData->GetScalarPointerForExtent(outExt);
        int ext[6];
  
	// Ensure intput is 2D
	inData->GetWholeExtent(ext);
	if (ext[5] != ext[4]) {
		vtkErrorMacro("ExecuteImageInformation: Input must be 2D.");
		return;
	}

	switch (inData->GetScalarType())
    {
    case VTK_FLOAT:
      vtkImageCloseUp2DExecute(this, inData, (float *)(inPtr), 
  			            outData, (float *) outPtr, outExt, id);
      break;
    case VTK_INT:
      vtkImageCloseUp2DExecute(this, inData, (int *)(inPtr), 
			            outData, (int *) outPtr, outExt, id);
      break;
    case VTK_SHORT:
      vtkImageCloseUp2DExecute(this, inData, (short *)(inPtr), 
			            outData, (short *) outPtr, outExt, id);
      break;
    case VTK_UNSIGNED_SHORT:
      vtkImageCloseUp2DExecute(this, inData, (unsigned short *)(inPtr), 
			            outData, (unsigned short *) outPtr, outExt, id);
      break;
    case VTK_UNSIGNED_CHAR:
      vtkImageCloseUp2DExecute(this, inData, (unsigned char *)(inPtr), 
			            outData, (unsigned char *) outPtr, outExt, id);
      break;
    case VTK_CHAR:
      vtkImageCloseUp2DExecute(this, inData,  ( char *)(inPtr), 
			            outData, ( char *) outPtr, outExt, id);
      break;
    case VTK_LONG:
      vtkImageCloseUp2DExecute(this, inData, (long *)(inPtr), 
			            outData, (long *) outPtr, outExt, id);
      break;
    case VTK_DOUBLE:
      vtkImageCloseUp2DExecute(this, inData, (double *)(inPtr), 
			            outData, (double *) outPtr, outExt, id);
      break;
    case VTK_UNSIGNED_LONG:
      vtkImageCloseUp2DExecute(this, inData, (unsigned long *)(inPtr), 
   			            outData, (unsigned long *) outPtr, outExt, id);
      break;
    case VTK_UNSIGNED_INT:
      vtkImageCloseUp2DExecute(this, inData, (unsigned int *)(inPtr), 
			            outData, (unsigned int *) outPtr, outExt, id);
      break;

    default:
      vtkErrorMacro(<< "Execute: Unknown input ScalarType");
      return;
    }
}

void vtkImageCloseUp2D::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkImageToImageFilter::PrintSelf(os,indent);
  
	os << indent << "X:              " << this->X << "\n";
	os << indent << "Y:              " << this->Y << "\n";
	os << indent << "Radius:         " << this->Radius << "\n";
	os << indent << "Magnification:  " << this->Magnification << "\n";
}


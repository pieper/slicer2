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
#include "vtkImageLabelChange.h"
#include "vtkObjectFactory.h"

//------------------------------------------------------------------------------
vtkImageLabelChange* vtkImageLabelChange::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkImageLabelChange");
  if(ret)
    {
    return (vtkImageLabelChange*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkImageLabelChange;
}


//----------------------------------------------------------------------------
// Description:
// Constructor sets default values
vtkImageLabelChange::vtkImageLabelChange()
{
	this->InputLabel = 0;
	this->OutputLabel = 0;
}

//----------------------------------------------------------------------------
// Description:
// This templated function executes the filter for any type of data.
template <class T>
static void vtkImageLabelChangeExecute(vtkImageLabelChange *self,
				     vtkImageData *inData, T *inPtr,
				     vtkImageData *outData, 
				     int outExt[6], int id)
{
	T *outPtr = (T *)outData->GetScalarPointerForExtent(outExt);
	// looping
	int outIncX, outIncY, outIncZ;
	int idxX, idxY, idxZ, maxX, maxY, maxZ;
	// Other
	T inLabel = (T) self->GetInputLabel();
	T outLabel = (T) self->GetOutputLabel();

	// Get increments to march through data 
	outData->GetContinuousIncrements(outExt, outIncX, outIncY, outIncZ);
	maxX = outExt[1] - outExt[0];
	maxY = outExt[3] - outExt[2]; 
	maxZ = outExt[5] - outExt[4];
    
	// Loop through ouput pixels
	for (idxZ = 0; idxZ <= maxZ; idxZ++) {
		for (idxY = 0; !self->AbortExecute && idxY <= maxY; idxY++) {
			for (idxX = 0; idxX <= maxX; idxX++) {
				if (*inPtr == inLabel)
					*outPtr = outLabel;
				else
					*outPtr = *inPtr;
				outPtr++;
				inPtr++;
			}
			outPtr += outIncY;
			inPtr += outIncY;
		}
		outPtr += outIncZ;
		inPtr += outIncZ;
	}
}


//----------------------------------------------------------------------------
// Description:
// This method is passed a input and output data, and executes the filter
// algorithm to fill the output from the input.
// It just executes a switch statement to call the correct function for
// the datas data types.
void vtkImageLabelChange::ThreadedExecute(vtkImageData *inData, 
					vtkImageData *outData,
					int outExt[6], int id)
{
	// Check Single component
	int x1;
	x1 = inData->GetNumberOfScalarComponents();
	if (x1 != 1) {
	vtkErrorMacro(<<"Input has "<<x1<<" instead of 1 scalar component.");
		return;
	}

	void *inPtr = inData->GetScalarPointerForExtent(outExt);
  
	switch (inData->GetScalarType())
	{
        case VTK_DOUBLE:
		vtkImageLabelChangeExecute(this, inData, (double *)(inPtr), 
			outData, outExt, id);
		break;
	case VTK_FLOAT:
		vtkImageLabelChangeExecute(this, inData, (float *)(inPtr), 
			outData, outExt, id);
		break;		
	case VTK_LONG:
		vtkImageLabelChangeExecute(this, inData, (long *)(inPtr), 
			outData, outExt, id);
		break;
	case VTK_UNSIGNED_LONG:
		vtkImageLabelChangeExecute(this, inData, (unsigned long *)(inPtr), 
			outData, outExt, id);
		break;
	case VTK_INT:
		vtkImageLabelChangeExecute(this, inData, (int *)(inPtr), 
			outData, outExt, id);
		break;
	case VTK_UNSIGNED_INT:
		vtkImageLabelChangeExecute(this, inData, (unsigned int *)(inPtr), 
			outData, outExt, id);
		break;
	case VTK_SHORT:
		vtkImageLabelChangeExecute(this, inData, (short *)(inPtr), 
			outData, outExt, id);
		break;
	case VTK_UNSIGNED_SHORT:
		vtkImageLabelChangeExecute(this, inData, (unsigned short *)(inPtr), 
			outData, outExt, id);
		break;
	case VTK_CHAR:
		vtkImageLabelChangeExecute(this, inData, (char *)(inPtr), 
			outData, outExt, id);
		break;
	case VTK_UNSIGNED_CHAR:
		vtkImageLabelChangeExecute(this, inData, (unsigned char *)(inPtr), 
			outData, outExt, id);
		break;
	default:
		vtkErrorMacro(<< "Execute: Unknown input ScalarType");
		return;
	}
}

void vtkImageLabelChange::PrintSelf(ostream& os, vtkIndent indent)
{
	vtkImageFilter::PrintSelf(os,indent);
        
	os << indent << "InputLabel: " << this->InputLabel << "\n";
	os << indent << "OutputLabel: " << this->OutputLabel << "\n";
}


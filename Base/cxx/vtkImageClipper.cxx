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
#include "vtkImageClipper.h"
#include "vtkObjectFactory.h"


//------------------------------------------------------------------------------
vtkImageClipper* vtkImageClipper::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkImageClipper");
  if(ret)
    {
    return (vtkImageClipper*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkImageClipper;
}

//----------------------------------------------------------------------------
// Description:
// Constructor sets default values
vtkImageClipper::vtkImageClipper()
{
	this->ReplaceIn = 1;
	this->InValue = 1.0;
	this->ReplaceOut = 1;
	this->OutValue = 0.0;
}

//----------------------------------------------------------------------------
void vtkImageClipper::SetOutputWholeExtent(int extent[6])
{
  int idx;
  
  for (idx = 0; idx < 6; ++idx)
  {
    if (this->OutputWholeExtent[idx] != extent[idx])
    {
      this->OutputWholeExtent[idx] = extent[idx];
      this->Modified();
    }
  }
}

//----------------------------------------------------------------------------
void vtkImageClipper::SetOutputWholeExtent(int minX, int maxX, 
					     int minY, int maxY,
					     int minZ, int maxZ)
{
  int extent[6];
  
  extent[0] = minX;  extent[1] = maxX;
  extent[2] = minY;  extent[3] = maxY;
  extent[4] = minZ;  extent[5] = maxZ;
  this->SetOutputWholeExtent(extent);
}

//----------------------------------------------------------------------------
void vtkImageClipper::ExecuteInformation(vtkImageData *inData, vtkImageData *outData)
{
}

//----------------------------------------------------------------------------
// Description:
// This templated function executes the filter for any type of data.
template <class T>
static void vtkImageClipperExecute(vtkImageClipper *self,
				     vtkImageData *inData, T *inPtr,
				     vtkImageData *outData, T *outPtr, 
				     int outExt[6], int id)
{
	int idxX, idxY, idxZ;
	int cminX, cminY, cminZ, cmaxX, cmaxY, cmaxZ, clip[6];
	int minX, minY, minZ, maxX, maxY, maxZ;
	int inIncX, inIncY, inIncZ;
	int outIncX, outIncY, outIncZ;
	int replaceIn = self->GetReplaceIn();
	T  inValue = (T)(self->GetInValue());
	int replaceOut = self->GetReplaceOut();
	T  outValue = (T)(self->GetOutValue());
  unsigned long count = 0;
  unsigned long target;

  self->GetOutputWholeExtent(clip);
	cmaxX = clip[1];
  cminX = clip[0];
	cmaxY = clip[3];
  cminY = clip[2]; 
	cmaxZ = clip[5];
  cminZ = clip[4];

  // find the region to loop over
	maxX = outExt[1];
  minX = outExt[0];
	maxY = outExt[3];
  minY = outExt[2]; 
	maxZ = outExt[5];
  minZ = outExt[4];
  target = (unsigned long)((maxZ-minZ+1)*(maxY-minY+1)/50.0);
  target++;
  
  // Get increments to march through data 
  inData->GetContinuousIncrements(outExt, inIncX, inIncY, inIncZ);
  outData->GetContinuousIncrements(outExt, outIncX, outIncY, outIncZ);

	for (idxZ = minZ; idxZ <= maxZ; idxZ++) {
	  for (idxY = minY; !self->AbortExecute && idxY <= maxY; idxY++) {
		  if (!id) {
			  if (!(count%target))
				  self->UpdateProgress(count/(50.0*target));
			  count++;
		  }
		  for (idxX = minX; idxX <= maxX; idxX++) {
			  if (idxX >= cminX && idxX <= cmaxX &&
            idxY >= cminY && idxY <= cmaxY &&
            idxZ >= cminZ && idxZ <= cmaxZ)		
				  *outPtr = *inPtr;
			  else
				  *outPtr = outValue;
			  outPtr++;			
			  inPtr++;
		  }
		  outPtr += outIncY;
		  inPtr += inIncY;
		}
		outPtr += outIncZ;
		inPtr += inIncZ;
	}
}

//----------------------------------------------------------------------------
// Description:
// This method is passed a input and output data, and executes the filter
// algorithm to fill the output from the input.
// It just executes a switch statement to call the correct function for
// the datas data types.
void vtkImageClipper::ThreadedExecute(vtkImageData *inData, 
    vtkImageData *outData, int outExt[6], int id)
{
	void *inPtr = inData->GetScalarPointerForExtent(outExt);
	void *outPtr = outData->GetScalarPointerForExtent(outExt);
  
	switch (inData->GetScalarType())
	{
	case VTK_DOUBLE:
		vtkImageClipperExecute(this, inData, (double *)(inPtr), 
			outData, (double *)(outPtr), outExt, id);
		break;
	case VTK_FLOAT:
		vtkImageClipperExecute(this, inData, (float *)(inPtr), 
			outData, (float *)(outPtr), outExt, id);
		break;
	case VTK_LONG:
		vtkImageClipperExecute(this, inData, (long *)(inPtr), 
			outData, (long *)(outPtr), outExt, id);
		break;
	case VTK_UNSIGNED_LONG:
		vtkImageClipperExecute(this, inData, (unsigned long *)(inPtr), 
			outData, (unsigned long *)(outPtr), outExt, id);
		break;
	case VTK_INT:
		vtkImageClipperExecute(this, inData, (int *)(inPtr), 
			outData, (int *)(outPtr), outExt, id);
	case VTK_UNSIGNED_INT:
		vtkImageClipperExecute(this, inData, (unsigned int *)(inPtr), 
			outData, (unsigned int *)(outPtr), outExt, id);
		break;
		break;
	case VTK_SHORT:
		vtkImageClipperExecute(this, inData, (short *)(inPtr), 
			outData, (short *)(outPtr), outExt, id);
		break;
	case VTK_UNSIGNED_SHORT:
		vtkImageClipperExecute(this, inData, (unsigned short *)(inPtr), 
			outData, (unsigned short *)(outPtr), outExt, id);
		break;
	case VTK_CHAR:
		vtkImageClipperExecute(this, inData, (char *)(inPtr), 
			outData, (char *)(outPtr), outExt, id);
		break;
	case VTK_UNSIGNED_CHAR:
		vtkImageClipperExecute(this, inData, (unsigned char *)(inPtr), 
			outData, (unsigned char *)(outPtr), outExt, id);
		break;
	default:
		vtkErrorMacro(<< "Execute: Unknown input ScalarType");
		return;
	}
}

void vtkImageClipper::PrintSelf(ostream& os, vtkIndent indent)
{
	vtkImageFilter::PrintSelf(os,indent);

	os << indent << "ReplaceIn: " << this->ReplaceIn << "\n";
	os << indent << "InValue: " << this->InValue << "\n";
	os << indent << "ReplaceOut: " << this->ReplaceOut << "\n";
	os << indent << "OutValue: " << this->OutValue << "\n";

	int idx;
	os << indent << "OutputWholeExtent: (" << this->OutputWholeExtent[0]<< "," << this->OutputWholeExtent[1];

	for (idx = 1; idx < 3; ++idx)
	  {
	    os << indent << ", " << this->OutputWholeExtent[idx * 2]<< "," << this->OutputWholeExtent[idx*2 + 1];
	  }
	os << ")\n";
 
}





/*=auto=========================================================================

(c) Copyright 2003 Massachusetts Institute of Technology (MIT) All Rights Reserved.

This software ("3D Slicer") is provided by The Brigham and Women's 
Hospital, Inc. on behalf of the copyright holders and contributors.
Permission is hereby granted, without payment, to copy, modify, display 
and distribute this software and its documentation, if any, for internal 
research purposes only, provided that (1) the above copyright notice and 
the following four paragraphs appear on all copies of this software, and 
(2) that source code to any modifications to this software be made 
publicly available under terms no more restrictive than those in this 
License Agreement. Use of this software constitutes acceptance of these 
terms and conditions.

3D Slicer Software has not been reviewed or approved by the Food and 
Drug Administration, and is for non-clinical, IRB-approved Research Use 
Only.  In no event shall data or images generated through the use of 3D 
Slicer Software be used in the provision of patient care.

IN NO EVENT SHALL THE COPYRIGHT HOLDERS AND CONTRIBUTORS BE LIABLE TO 
ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL 
DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, 
EVEN IF THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE BEEN ADVISED OF THE 
POSSIBILITY OF SUCH DAMAGE.

THE COPYRIGHT HOLDERS AND CONTRIBUTORS SPECIFICALLY DISCLAIM ANY EXPRESS 
OR IMPLIED WARRANTIES INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND 
NON-INFRINGEMENT.

THE SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS 
IS." THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE NO OBLIGATION TO 
PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.


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
    vtkImageToImageFilter::PrintSelf(os,indent);
        
    os << indent << "InputLabel: " << this->InputLabel << "\n";
    os << indent << "OutputLabel: " << this->OutputLabel << "\n";
}


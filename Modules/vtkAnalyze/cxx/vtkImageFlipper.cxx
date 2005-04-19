/*=auto=========================================================================

(c) Copyright 2005 Massachusetts Institute of Technology (MIT) All Rights Reserved.

This software ("3D Slicer") is provided by The Brigham and Women's 
Hospital, Inc. on behalf of the copyright holders and contributors.
Permission is hereby granted, without payment, to copy, modify, display 
and distribute this software and its documentation, if any, for  
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


#include "vtkImageFlipper.h"

#include "vtkObjectFactory.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkDataArray.h"
#include "vtkInternalFlip.h"

#include <stdio.h>


vtkStandardNewMacro(vtkImageFlipper);


vtkImageFlipper::vtkImageFlipper()
{
    this->FlippingSequence = NULL; 
}


vtkImageFlipper::~vtkImageFlipper()
{
    if (this->FlippingSequence)
    {
        delete [] this->FlippingSequence;
        this->FlippingSequence = NULL;
    }
}


void vtkImageFlipper::SetFlippingSequence(const char *seq)
{
    if (this->FlippingSequence && seq && 
        (!strcmp(this->FlippingSequence, seq))) // this->FlippingSequence == seq 
    {
        return;
    }
    if (!seq && !this->FlippingSequence)
    {
        return;
    }
    if (this->FlippingSequence)
    {
        delete [] this->FlippingSequence;
    }

    if (seq)
    {
        this->FlippingSequence = new char[strlen(seq)+1];
        strcpy(this->FlippingSequence, seq);
    }
    else
    {
        this->FlippingSequence = NULL;
    }
}

// The code of this function is basically from vtkCISGAnalyzeReader.cxx: 
/*
  Library   : vtk/objects/library/CISGImage
  Module    : $RCSfile: vtkImageFlipper.cxx,v $
  Authors   : Thomas Hartkens <thomas@hartkens.de>
              Daniel Rueckert <d.rueckert@doc.ic.ac.uk>
          Julia Schnabel  <julia.schnabel@kcl.ac.uk>
          under the supervision of Dave Hawkes and Derek Hill
  Web       : http://www.image-registration.com
  Copyright : King's College London
              Div. of Radiological Sciences, 
              Computational Imaging Science Group, 1997 - 2000
          http://www-ipg.umds.ac.uk/cisg
  Date      : $Date: 2005/04/19 14:46:10 $
  Version   : $Revision: 1.2 $
*/
void vtkImageFlipper::SimpleExecute(vtkImageData* input, vtkImageData* output)
{
    if (this->GetInput() == NULL)
    {
        vtkErrorMacro( << "No input image data in this filter.");
        return;
    }

    output->DeepCopy(this->GetInput());
    int imgDim[3];  
    output->GetDimensions(imgDim);

    switch(output->GetScalarType())
    {
        case VTK_CHAR: 
        {
            char* data = (char *)output->GetScalarPointer();

            // apply sequence of flip operations specified by string
            while (this->FlippingSequence && *this->FlippingSequence) 
            {        
                InternalFlip(*this->FlippingSequence++ - '0', data, imgDim);
            }

        }
        break;
        case VTK_UNSIGNED_CHAR:
        {
            unsigned char* data = (unsigned char *)output->GetScalarPointer();
            while (this->FlippingSequence && *this->FlippingSequence) 
            {        
                InternalFlip(*this->FlippingSequence++ - '0', data, imgDim);
            }

        }
        break;
        case VTK_SHORT:
        {
            short *data = (short *)output->GetScalarPointer();
            while (this->FlippingSequence && *this->FlippingSequence) 
            {        
                InternalFlip(*this->FlippingSequence++ - '0', data, imgDim);
            }
        }
        break;
        case VTK_UNSIGNED_SHORT:
        {
            unsigned short *data = (unsigned short *)output->GetScalarPointer();
            while (this->FlippingSequence && *this->FlippingSequence) 
            {        
                InternalFlip(*this->FlippingSequence++ - '0', data, imgDim);
            }
        }
        break;
        case VTK_INT:
        {
            int *data = (int *)output->GetScalarPointer();
            while (this->FlippingSequence && *this->FlippingSequence) 
            {        
                InternalFlip(*this->FlippingSequence++ - '0', data, imgDim);
            }
        }
        break;
        case VTK_UNSIGNED_INT:
        {
            unsigned int *data = (unsigned int *)output->GetScalarPointer();
            while (this->FlippingSequence && *this->FlippingSequence) 
            {        
                InternalFlip(*this->FlippingSequence++ - '0', data, imgDim);
            }
        }
        break;
        case VTK_FLOAT:
        {
            float *data = (float *)output->GetScalarPointer();
            while (this->FlippingSequence && *this->FlippingSequence) 
            {        
                InternalFlip(*this->FlippingSequence++ - '0', data, imgDim);
            }
        }
        break;
        default:
            vtkErrorMacro(<< "vtkImageFlipper: Unknown voxel type");
            exit(1);
    }
}

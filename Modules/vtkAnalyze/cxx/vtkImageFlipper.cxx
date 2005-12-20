/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkImageFlipper.cxx,v $
  Date:      $Date: 2005/12/20 22:54:57 $
  Version:   $Revision: 1.2.8.1 $

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
  Date      : $Date: 2005/12/20 22:54:57 $
  Version   : $Revision: 1.2.8.1 $
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

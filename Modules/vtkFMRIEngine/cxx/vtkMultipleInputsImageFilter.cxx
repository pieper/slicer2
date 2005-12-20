/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMultipleInputsImageFilter.cxx,v $
  Date:      $Date: 2005/12/20 22:55:30 $
  Version:   $Revision: 1.2.2.1 $

=========================================================================auto=*/

#include "vtkMultipleInputsImageFilter.h"
#include "vtkSource.h"
#include "vtkImageData.h"
#include "vtkPointData.h"


void vtkMultipleInputsImageFilter::AddInput(vtkImageData *image)
{
    this->vtkProcessObject::AddInput(image);
}


vtkImageData *vtkMultipleInputsImageFilter::GetInput(int index)
{
    if (this->NumberOfInputs <= index)
    {
        return NULL;
    }

    return (vtkImageData*)(this->Inputs[index]);
}


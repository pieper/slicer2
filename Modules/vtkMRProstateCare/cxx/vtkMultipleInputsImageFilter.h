/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMultipleInputsImageFilter.h,v $
  Date:      $Date: 2006/07/14 15:13:28 $
  Version:   $Revision: 1.1.2.1 $

=========================================================================auto=*/

#ifndef __vtkMultipleInputsImageFilter_h
#define __vtkMultipleInputsImageFilter_h


#include <vtkMRProstateCareConfigure.h>
#include "vtkSimpleImageToImageFilter.h"

class  VTK_MRPROSTATECARE_EXPORT vtkMultipleInputsImageFilter : public vtkSimpleImageToImageFilter
{
public:
    vtkTypeMacro(vtkMultipleInputsImageFilter, vtkSimpleImageToImageFilter);

    // Description:
    // Adds an input to the input list. Expands the list memory if necessary.
    void AddInput(vtkImageData *input);

    // Description:
    // Get one input whose index is "indx" on the input list.
    vtkImageData *GetInput(int indx);
};


#endif

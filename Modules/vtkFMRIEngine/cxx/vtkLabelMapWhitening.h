/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkLabelMapWhitening.h,v $
  Date:      $Date: 2005/12/20 22:55:30 $
  Version:   $Revision: 1.2.2.1 $

=========================================================================auto=*/

#ifndef __vtkLabelMapWhitening_h
#define __vtkLabelMapWhitening_h


#include <vtkFMRIEngineConfigure.h>
#include "vtkFloatArray.h"
#include "vtkDataObject.h"
#include "vtkSimpleImageToImageFilter.h"

class  VTK_FMRIENGINE_EXPORT vtkLabelMapWhitening : public vtkSimpleImageToImageFilter
{
public:
    static vtkLabelMapWhitening *New();
    vtkTypeMacro(vtkLabelMapWhitening, vtkSimpleImageToImageFilter);

protected:
    vtkLabelMapWhitening();
    ~vtkLabelMapWhitening();

    void SimpleExecute(vtkImageData *input,vtkImageData *output);
};

#endif

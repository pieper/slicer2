/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkImageFlipper.h,v $
  Date:      $Date: 2005/12/20 22:54:57 $
  Version:   $Revision: 1.2.8.1 $

=========================================================================auto=*/


#ifndef __vtkImageFlipper_h
#define __vtkImageFlipper_h


#include <vtkAnalyzeConfigure.h>
#include "vtkSimpleImageToImageFilter.h"

class  VTK_ANALYZE_EXPORT vtkImageFlipper : public vtkSimpleImageToImageFilter
{
public:
    static vtkImageFlipper *New();
    vtkTypeMacro(vtkImageFlipper,vtkSimpleImageToImageFilter);

    vtkImageFlipper();
    ~vtkImageFlipper();

    // Description:
    // Specifies a flipping sequence. 
    void SetFlippingSequence(const char *);
    vtkGetStringMacro(FlippingSequence);

protected:

    void SimpleExecute(vtkImageData* input,vtkImageData* output);
    char *FlippingSequence;
};


#endif

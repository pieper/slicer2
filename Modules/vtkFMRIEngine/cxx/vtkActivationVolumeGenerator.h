/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkActivationVolumeGenerator.h,v $
  Date:      $Date: 2005/12/20 22:55:28 $
  Version:   $Revision: 1.10.2.1 $

=========================================================================auto=*/

#ifndef __vtkActivationVolumeGenerator_h
#define __vtkActivationVolumeGenerator_h


#include <vtkFMRIEngineConfigure.h>
#include "vtkSimpleImageToImageFilter.h"


class  VTK_FMRIENGINE_EXPORT vtkActivationVolumeGenerator : public vtkSimpleImageToImageFilter
{
public:
    vtkTypeMacro(vtkActivationVolumeGenerator, vtkSimpleImageToImageFilter);

    // Description:
    // Gets the low range. 
    float GetLowRange() {return LowRange;}
 
    // Description:
    // Gets the high range. 
    float GetHighRange() {return HighRange;}

protected:
    vtkActivationVolumeGenerator();
    ~vtkActivationVolumeGenerator();

    float LowRange;
    float HighRange;
};


#endif

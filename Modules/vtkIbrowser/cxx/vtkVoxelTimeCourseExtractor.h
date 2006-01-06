/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkVoxelTimeCourseExtractor.h,v $
  Date:      $Date: 2006/01/06 17:57:51 $
  Version:   $Revision: 1.2 $

=========================================================================auto=*/
#ifndef __vtkVoxelTimeCourseExtractor_h
#define __vtkVoxelTimeCourseExtractor_h

#include "vtkIbrowserConfigure.h"
#include "vtkFloatArray.h"
#include "vtkSimpleImageToImageFilter.h"

class  VTK_IBROWSER_EXPORT vtkVoxelTimeCourseExtractor : public vtkSimpleImageToImageFilter
{
 public:
    static vtkVoxelTimeCourseExtractor *New();
    vtkTypeMacro(vtkVoxelTimeCourseExtractor, vtkSimpleImageToImageFilter);

    // Description:
    // Returns the time course of a specified voxel (i, j, k).
    vtkFloatArray *GetTimeCourse(int i, int j, int k);
    vtkFloatArray *GetFloatTimeCourse(int i, int j, int k);
    vtkImageData *GetInput (int volNum);
    void AddInput(vtkImageData *input);
    
    float outputMax;
    float outputMin;
    float outputRange;
    float numInputs;

 protected:
    void SimpleExecute (vtkImageData* input, vtkImageData* output);
    vtkVoxelTimeCourseExtractor();
    ~vtkVoxelTimeCourseExtractor();

 private:
};


#endif

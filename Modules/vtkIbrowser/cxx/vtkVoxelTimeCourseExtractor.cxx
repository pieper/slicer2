/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkVoxelTimeCourseExtractor.cxx,v $
  Date:      $Date: 2006/01/06 17:57:51 $
  Version:   $Revision: 1.2 $

=========================================================================auto=*/
#include "vtkVoxelTimeCourseExtractor.h"
#include "vtkObjectFactory.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkCommand.h"

#include <stdio.h>

vtkStandardNewMacro(vtkVoxelTimeCourseExtractor);


vtkVoxelTimeCourseExtractor::vtkVoxelTimeCourseExtractor()
{
    float outputMax;
    float outputMin;
    float outputRange;
    this->outputMax = 0.0;
    this->outputMin = 10000000.0;
    this->outputRange = this->outputMin - this->outputMax;
    this->numInputs = 0;
}



vtkVoxelTimeCourseExtractor::~vtkVoxelTimeCourseExtractor()
{
}



vtkImageData *vtkVoxelTimeCourseExtractor::GetInput (int volNum)
{
    if (this->numInputs <= volNum)
        {
            return NULL;
        }
    return (vtkImageData*)(this->Inputs[volNum]);
}



vtkFloatArray *vtkVoxelTimeCourseExtractor::GetTimeCourse(int i, int j, int k)
{

    if (this->NumberOfInputs == 0 || this->GetInput(0) == NULL)
    {
        vtkErrorMacro( <<"No input image data; no timecourse can be extracted.");
        return NULL;
    }

    vtkFloatArray *timeCourse = vtkFloatArray::New();
    timeCourse->SetNumberOfTuples(this->NumberOfInputs);
    timeCourse->SetNumberOfComponents(1);
    for (int ii = 0; ii < this->NumberOfInputs; ii++)
    {
        short *val = (short *)this->GetInput(ii)->GetScalarPointer(i, j, k); 
        timeCourse->SetComponent(ii, 0, *val); 
    }

    return timeCourse;
}



vtkFloatArray *vtkVoxelTimeCourseExtractor::GetFloatTimeCourse(int i, int j, int k)
{
    if (this->NumberOfInputs == 0 || this->GetInput(0) == NULL)
    {
        vtkErrorMacro( <<"No input image data; no timecourse can be extracted.");
        return NULL;
    }

    vtkFloatArray *timeCourse = vtkFloatArray::New();
    timeCourse->SetNumberOfTuples(this->NumberOfInputs);
    timeCourse->SetNumberOfComponents(1);
    for (int ii = 0; ii < this->NumberOfInputs; ii++)
    {
        float *val = (float *)this->GetInput(ii)->GetScalarPointer(i, j, k); 
        timeCourse->SetComponent(ii, 0, *val); 
    }

    return timeCourse;
}




void vtkVoxelTimeCourseExtractor::AddInput(vtkImageData *input)
{
    this->vtkProcessObject::AddInput(input);
    this->numInputs = this->numInputs + 1;
}




void vtkVoxelTimeCourseExtractor::SimpleExecute ( vtkImageData* input, vtkImageData* output)
{
    // this doesn't need to do anything.
    return;
}

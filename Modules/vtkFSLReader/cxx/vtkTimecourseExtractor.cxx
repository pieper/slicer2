/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkTimecourseExtractor.cxx,v $
  Date:      $Date: 2005/12/20 22:55:33 $
  Version:   $Revision: 1.1.2.1 $

=========================================================================auto=*/
/*==============================================================================
(c) Copyright 2004 Massachusetts Institute of Technology (MIT) All Rights Reserved.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
==============================================================================*/


#include "vtkTimecourseExtractor.h"
#include "vtkObjectFactory.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkCommand.h"

#include <stdio.h>

vtkStandardNewMacro(vtkTimecourseExtractor);


vtkTimecourseExtractor::vtkTimecourseExtractor()
{
}


vtkTimecourseExtractor::~vtkTimecourseExtractor()
{
}


void vtkTimecourseExtractor::AddInput(vtkImageData *input)
{
    this->vtkProcessObject::AddInput(input);
}


vtkImageData *vtkTimecourseExtractor::GetInput(int idx)
{
    if (this->NumberOfInputs <= idx)
    {
        return NULL;
    }

    return (vtkImageData*)(this->Inputs[idx]);
}


vtkFloatArray *vtkTimecourseExtractor::GetTimeCourse(int i, int j, int k)
{
    // Checks the input list
    if (this->NumberOfInputs == 0 || this->GetInput(0) == NULL)
    {
        vtkErrorMacro( <<"No input image data in this filter.");
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


void vtkTimecourseExtractor::SimpleExecute(vtkImageData *inputs, vtkImageData* output)
{
}

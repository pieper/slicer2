/*=auto=========================================================================

(c) Copyright 2004 Massachusetts Institute of Technology (MIT) All Rights Reserved.

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


#include "vtkActivationVolumeGenerator.h"
#include "vtkObjectFactory.h"
#include "vtkImageData.h"
#include "vtkPointData.h"

#include <stdio.h>


vtkStandardNewMacro(vtkActivationVolumeGenerator);


vtkActivationVolumeGenerator::vtkActivationVolumeGenerator()
{
    this->Detector = NULL; 
    this->lowerThreshold = 0;
}


vtkActivationVolumeGenerator::~vtkActivationVolumeGenerator()
{
}


vtkFloatArray *vtkActivationVolumeGenerator::GetTimeCourse(int i, int j, int k)
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


void vtkActivationVolumeGenerator::SetDetector(vtkActivationDetector *detector)
{
    this->Detector = detector;
}


void vtkActivationVolumeGenerator::SimpleExecute(vtkImageData *inputs, vtkImageData* output)
{
    if (this->NumberOfInputs == 0 || this->GetInput(0) == NULL)
    {
        vtkErrorMacro( << "No input image data in this filter.");
        return;
    }
 
    // Sets up properties for output vtkImageData
    int imgDim[3];  
    this->GetInput(0)->GetDimensions(imgDim);
    output->SetScalarType(VTK_FLOAT);
    output->SetOrigin(this->GetInput(0)->GetOrigin());
    output->SetSpacing(this->GetInput(0)->GetSpacing());
    output->SetNumberOfScalarComponents(1);
    output->SetDimensions(imgDim[0], imgDim[1], imgDim[2]);
    output->AllocateScalars();
   
    // Array holding time course of a voxel
    vtkFloatArray *tc = vtkFloatArray::New();
    tc->SetNumberOfTuples(this->NumberOfInputs);
    tc->SetNumberOfComponents(1);

    float t = 0.0;
    int indx = 0;
    vtkDataArray *scalarsInOutput = output->GetPointData()->GetScalars();
    // Voxel iteration through the entire image volume
    for (int kk = 0; kk < imgDim[2]; kk++)
    {
        for (int jj = 0; jj < imgDim[1]; jj++)
        {
            for (int ii = 0; ii < imgDim[0]; ii++)
            {
                // Gets time course for this voxel
                float total = 0.0;
                for (int i = 0; i < this->NumberOfInputs; i++)
                {
                    short *value 
                        = (short *)this->GetInput(i)->GetScalarPointer(ii, jj, kk);
                    tc->SetComponent(i, 0, *value);
                    total += *value;
                }   

                if ((total/this->NumberOfInputs) < lowerThreshold)
                {
                    t = 0.0;
                }
                else
                {
                    t = this->Detector->Detect(tc);
                    // all negative t values are changed to positive 
                    t = (t < 0.0 ? (-t) : t);
                }

                scalarsInOutput->SetComponent(indx++, 0, t);
            }
        } 
    }

    tc->Delete();

    // Scales the scalar values in the activation volume between 0 - 100
    float range[2];
    float value;
    float newValue;
    output->GetScalarRange(range);
    lowRange = range[0];
    highRange = range[1];
    for (int i = 0; i < indx; i++)
    {
        value = scalarsInOutput->GetComponent(i, 0); 
        newValue = 100 * (value - range[0]) / (range[1] - range[0]);
        scalarsInOutput->SetComponent(i, 0, newValue);
    }
}

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


#include "GeneralLinearModel.h"
#include "vtkActivationEstimator.h"
#include "vtkObjectFactory.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkCommand.h"

#include <stdio.h>


vtkStandardNewMacro(vtkActivationEstimator);


vtkActivationEstimator::vtkActivationEstimator()
{
    this->Detector = NULL; 
    this->lowerThreshold = 0;
}


vtkActivationEstimator::~vtkActivationEstimator()
{
}


vtkFloatArray *vtkActivationEstimator::GetTimeCourse(int i, int j, int k)
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


void vtkActivationEstimator::SetDetector(vtkActivationDetector *detector)
{
    this->Detector = detector;
}


void vtkActivationEstimator::SimpleExecute(vtkImageData *inputs, vtkImageData* output)
{
    if (this->NumberOfInputs == 0 || this->GetInput(0) == NULL)
    {
        vtkErrorMacro( << "No input image data in this filter.");
        return;
    }

    // for progress update (bar)
    unsigned long count = 0;
    unsigned long target;

    // Sets up properties for output vtkImageData
    int noOfRegressors = this->Detector->GetNoOfRegressors();
    int imgDim[3];  
    this->GetInput(0)->GetDimensions(imgDim);
    output->SetScalarType(VTK_FLOAT);
    output->SetOrigin(this->GetInput(0)->GetOrigin());
    output->SetSpacing(this->GetInput(0)->GetSpacing());
    output->SetNumberOfScalarComponents(noOfRegressors * 2);
    output->SetDimensions(imgDim[0], imgDim[1], imgDim[2]);
    output->AllocateScalars();
   
    // Array holding time course of a voxel
    vtkFloatArray *tc = vtkFloatArray::New();
    tc->SetNumberOfTuples(this->NumberOfInputs);
    tc->SetNumberOfComponents(1);

    target = (unsigned long)(imgDim[0]*imgDim[1]*imgDim[2] / 50.0);
    target++;

    // Use memory allocation for MS Windows VC++ compiler.
    // beta[noOfRegressors] is not allowed by MS Windows VC++ compiler.
    float *beta = new float [noOfRegressors];
    float *cov = new float [noOfRegressors];
    if (beta == NULL || cov == NULL)
    {
        vtkErrorMacro( << "Memory allocation failed.");
        return;
    }

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

                if ((total/this->NumberOfInputs) > lowerThreshold)
                {
                    this->Detector->Detect(tc, beta, cov);
                }
                else
                {
                    for (int dd = 0; dd < noOfRegressors; dd++)
                    {
                        beta[dd] = 0.0;
                        cov[dd] = 0.0;
                    }
                }
       
                int yy = 0;
                for (int dd = 0; dd < noOfRegressors; dd++)
                {
                    scalarsInOutput->SetComponent(indx, yy++, beta[dd]);
                    scalarsInOutput->SetComponent(indx, yy++, cov[dd]);
                }
                indx++;

                if (!(count%target))
                {
                    UpdateProgress(count / 2 / (50.0*target));
                }
                count++;
            }
        } 
    }

    delete [] beta;
    delete [] cov;
 
    GeneralLinearModel::Free();
    tc->Delete();
}

/*=auto=========================================================================

(c) Copyright 2005 Massachusetts Institute of Technology (MIT) All Rights Reserved.

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
#include "vtkImageFFT.h"
#include "vtkImageRFFT.h"
#include "vtkImageIdealHighPass.h"
#include "vtkImageAppend.h"
#include "vtkImageExtractComponents.h"
#include "vtkExtractVOI.h"
#include "vtkImageViewer.h"

#include <stdio.h>

vtkStandardNewMacro(vtkActivationEstimator);


vtkActivationEstimator::vtkActivationEstimator()
{
    this->Cutoff = 0.0;
    this->LowerThreshold = 0.0;
    this->HighPassFiltering = 0;

    this->Detector = NULL; 
    this->TimeCourse = NULL; 
    this->RegionTimeCourse = NULL;
    this->RegionVoxels = NULL;
}


vtkActivationEstimator::~vtkActivationEstimator()
{
    if (this->TimeCourse != NULL)
    {
        this->TimeCourse->Delete();
    }
    if (this->RegionTimeCourse != NULL)
    {
        this->RegionTimeCourse->Delete();
    }
    if (this->RegionVoxels != NULL)
    {
        this->RegionVoxels->Delete();
    }
}


vtkFloatArray *vtkActivationEstimator::GetRegionTimeCourse()
{
    // Checks the input list
    if (this->NumberOfInputs == 0 || this->GetInput(0) == NULL)
    {
        vtkErrorMacro( <<"No input image data in this filter.");
        return NULL;
    }

    if (this->RegionVoxels == NULL)
    {
        vtkErrorMacro( <<"Indices of all voxels in the ROI is required.");
        return NULL;
    }

    if (this->RegionTimeCourse != NULL) 
    {
        this->RegionTimeCourse->Delete();
    }
    this->RegionTimeCourse = vtkFloatArray::New();
    this->RegionTimeCourse->SetNumberOfTuples(this->NumberOfInputs);
    this->RegionTimeCourse->SetNumberOfComponents(1);

    short *val;
    int size = this->RegionTimeCourse->GetNumberOfTuples();
    for (int ii = 0; ii < this->NumberOfInputs; ii++)
    {
        int total = 0;
        for (int jj = 0; jj < size; jj++)
        {
            int x = (int)this->TimeCourse->GetComponent(jj, 0);
            int y = (int)this->TimeCourse->GetComponent(jj, 1);
            int z = (int)this->TimeCourse->GetComponent(jj, 2);
            val = (short *)this->GetInput(ii)->GetScalarPointer(x, y, z); 
            total += *val;
        }

        this->TimeCourse->SetComponent(ii, 0, (short)(total/size)); 
    }

    return this->RegionTimeCourse;
}


vtkFloatArray *vtkActivationEstimator::GetTimeCourse(int i, int j, int k)
{
    // Checks the input list
    if (this->NumberOfInputs == 0 || this->GetInput(0) == NULL)
    {
        vtkErrorMacro( <<"No input image data in this filter.");
        return NULL;
    }

    if (this->TimeCourse != NULL) 
    {
        this->TimeCourse->Delete();
    }
    this->TimeCourse = vtkFloatArray::New();
    this->TimeCourse->SetNumberOfTuples(this->NumberOfInputs);
    this->TimeCourse->SetNumberOfComponents(1);

    short *val;
    for (int ii = 0; ii < this->NumberOfInputs; ii++)
    {
        val = (short *)this->GetInput(ii)->GetScalarPointer(i, j, k); 
        this->TimeCourse->SetComponent(ii, 0, *val); 
    }

    // Execute high-pass filter on the timecourse
    if (this->HighPassFiltering) 
    {
        this->PerformHighPassFiltering();
    }

    return this->TimeCourse;
}


void vtkActivationEstimator::SetDetector(vtkActivationDetector *detector)
{
    this->Detector = detector;
}


void vtkActivationEstimator::PerformHighPassFiltering()
{
    // We are going to perform high pass filtering on the time course 
    // of a specific voxel. First, we convert the time course from
    // a vtkFloatArray to vtkImageData.
    vtkImageData *img = vtkImageData::New();
    img->GetPointData()->SetScalars(this->TimeCourse);
    img->SetDimensions(this->NumberOfInputs, 1, 1);
    img->SetScalarType(VTK_FLOAT);
    img->SetSpacing(1.0, 1.0, 1.0);
    img->SetOrigin(0.0, 0.0, 0.0);

    // FFT on the vtkImageData
    vtkImageFFT *fft = vtkImageFFT::New();
    fft->SetInput(img); 

    // Cut frequency on the vtkImageData on frequence domain
    vtkImageIdealHighPass *highPass = vtkImageIdealHighPass::New();
    highPass->SetInput(fft->GetOutput());
    highPass->SetXCutOff(this->Cutoff);         
    highPass->SetYCutOff(this->Cutoff); 
    highPass->ReleaseDataFlagOff();

    // RFFT on the vtkImageData following frequency cutoff
    vtkImageRFFT *rfft = vtkImageRFFT::New();
    rfft->SetInput(highPass->GetOutput());

    // The vtkImageData now holds two components: real and imaginary.
    // The real component is the image (time course) we want to plot
    vtkImageExtractComponents *real = vtkImageExtractComponents::New();
    real->SetInput(rfft->GetOutput());
    real->SetComponents(0);
    real->Update();

    // Update the vtkFloatArray of the time course
    vtkDataArray *arr = real->GetOutput()->GetPointData()->GetScalars();
    for (int i = 0; i < this->NumberOfInputs; i++) 
    {
        float x = (float) arr->GetComponent(i, 0);
        this->TimeCourse->SetComponent(i, 0, x);
    }

    // Clean up
    highPass->Delete();
    real->Delete();
    rfft->Delete();
    fft->Delete();
    img->Delete();
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
    unsigned long target = 0;
    
    // Sets up properties for output vtkImageData
    int noOfRegressors = this->Detector->GetDesignMatrix()->GetNumberOfComponents();
    int imgDim[3];  
    this->GetInput(0)->GetDimensions(imgDim);
    output->SetScalarType(VTK_FLOAT);
    output->SetOrigin(this->GetInput(0)->GetOrigin());
    output->SetSpacing(this->GetInput(0)->GetSpacing());
    // The scalar components hold the following:
    // for each regressor: beta value
    // plus chisq (the sum of squares of the residuals from the best-fit)
    output->SetNumberOfScalarComponents(noOfRegressors+1);
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
    if (beta == NULL)
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

                float chisq;
                if ((total/this->NumberOfInputs) > this->LowerThreshold)
                {
                    this->Detector->Detect(tc, beta, &chisq);
                }
                else
                {
                    for (int dd = 0; dd < noOfRegressors; dd++)
                    {
                        beta[dd] = 0.0;
                    }
                }
       
                int yy = 0;
                for (int dd = 0; dd < noOfRegressors; dd++)
                {
                    scalarsInOutput->SetComponent(indx, yy++, beta[dd]);
                }
                scalarsInOutput->SetComponent(indx, yy++, chisq);
                indx++;

                if (!(count%target))
                {
                    UpdateProgress(count / (50.0*target));
                }
                count++;
            }
        } 
    }

    delete [] beta;
 
    GeneralLinearModel::Free();
    tc->Delete();
}

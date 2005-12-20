/*=auto=========================================================================

(c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

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

#include "vtkActivationRegionStats.h"
#include "vtkObjectFactory.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkCommand.h"


vtkStandardNewMacro(vtkActivationRegionStats);


vtkActivationRegionStats::vtkActivationRegionStats()
{
    this->RegionVoxels = NULL; 
    this->SignalChanges = NULL;

    this->Label = 0;
    this->Count = 0;
}


vtkActivationRegionStats::~vtkActivationRegionStats()
{
    if (this->RegionVoxels != NULL)
    {
        this->RegionVoxels->Delete();
    }

    if (this->SignalChanges != NULL)
    {
        this->SignalChanges->Delete();
    }
}


void vtkActivationRegionStats::SimpleExecute(vtkImageData *inputs, vtkImageData* output)
{
    if (this->NumberOfInputs != 3)
    {
        vtkErrorMacro( << "This filter can only accept three input images.");
        return;
    }
    // this->NumberOfInputs == 3 
    else
    {
        int dim[3];  
        this->GetInput(0)->GetDimensions(dim);
        int size = dim[0]*dim[1]*dim[2];

        // Array holding the intensities of all voxels in
        // the defined ROI.
        float *t = new float[size];

        // Arrays holding the coordinates of all voxels in
        // the defined ROI.
        int *x = new int[size];
        int *y = new int[size];
        int *z = new int[size];

        int len = (this->GetInput(2)->GetNumberOfScalarComponents() - 2) / 2;
        double *signalChanges = new double [len];
        for (int d = 0; d < len; d++) {
            signalChanges[d] = 0.0; // initialization 
        }

        // Number of inputs == 3 means we are going to compute stats 
        // for t volume: 
        // the first volume - the label map volume 
        // the second volume - the t volume
        // the third volume - the beta volume
        int indx = 0;
        int index2 = 0;
        vtkDataArray *betas = this->GetInput(2)->GetPointData()->GetScalars();

        // Voxel iteration through the entire image volume
        for (int kk = 0; kk < dim[2]; kk++)
        {
            for (int jj = 0; jj < dim[1]; jj++)
            {
                for (int ii = 0; ii < dim[0]; ii++)
                {
                    short *l = (short *)this->GetInput(0)->GetScalarPointer(ii, jj, kk);
                    if (*l == this->Label)
                    {
                        x[indx] = ii;
                        y[indx] = jj;
                        z[indx] = kk;

                        float *tv = (float *)this->GetInput(1)->GetScalarPointer(ii, jj, kk);
                        t[indx++] = *tv;

                        // get % signal changes
                        int yy = len + 2;
                        for (int d = 0; d < len; d++) {
                            signalChanges[d] += betas->GetComponent(index2, yy++);
                        }
                    }
                     
                    index2++;
                }
            } 
        }

        this->Count = indx;

        // Array holding all voxels in the defined ROI.
        if (this->RegionVoxels != NULL)
        {
            this->RegionVoxels->Delete();
            this->RegionVoxels = NULL;
        }

        if (indx > 0) 
        {
            this->RegionVoxels = vtkFloatArray::New();
            this->RegionVoxels->SetNumberOfTuples(indx);
            this->RegionVoxels->SetNumberOfComponents(4);

            // create the output image
            output->SetWholeExtent(0, this->Count-1, 0, 0, 0, 0);
            output->SetExtent(0, this->Count-1, 0, 0, 0, 0);
            output->SetScalarType(VTK_FLOAT);
            output->SetOrigin(this->GetInput(0)->GetOrigin());
            output->SetSpacing(this->GetInput(0)->GetSpacing());
            output->SetNumberOfScalarComponents(1);
            output->AllocateScalars();

            float *ptr = (float *) output->GetScalarPointer();
            for (int i = 0; i < indx; i++) 
            {
                *ptr++ = t[i];

                // always use InsertTuple4, instead of SetTuple4
                // since the former handles memory allocation if needed.
                this->RegionVoxels->InsertTuple4(i, x[i], y[i], z[i], t[i]);
            }

            // get average % signal changes
            this->SignalChanges = vtkFloatArray::New();
            this->SignalChanges->SetNumberOfTuples(len);
            this->SignalChanges->SetNumberOfComponents(1);
            for (int d = 0; d < len; d++) {
                signalChanges[d] /= indx; 
                this->SignalChanges->SetComponent(d, 0, signalChanges[d]);
            }
        }

        delete [] t;
        delete [] x;
        delete [] y;
        delete [] z;
        delete [] signalChanges;
    }
} 


// If the output image of a filter has different properties from the input image
// we need to explicitly define the ExecuteInformation() method
void vtkActivationRegionStats::ExecuteInformation(vtkImageData *input, vtkImageData *output)
{
    this->vtkSimpleImageToImageFilter::ExecuteInformation();

    if (this->NumberOfInputs == 3 && this->Count > 0)
    {
        int dim[3];  
        dim[0] = this->Count;
        dim[1] = 1;
        dim[2] = 1;
        output->SetDimensions(dim);
        output->SetWholeExtent(0, this->Count-1, 0, 0, 0, 0);
        output->SetExtent(0, this->Count-1, 0, 0, 0, 0);
        output->SetScalarType(VTK_FLOAT);
        output->SetOrigin(this->GetInput(0)->GetOrigin());
        output->SetSpacing(this->GetInput(0)->GetSpacing());
        output->SetNumberOfScalarComponents(1);
    }
}


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


#include "vtkActivationRegionStats.h"
#include "vtkObjectFactory.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkCommand.h"


vtkStandardNewMacro(vtkActivationRegionStats);


vtkActivationRegionStats::vtkActivationRegionStats()
{
    this->RegionVoxels = NULL; 
    this->Label = 0;
    this->Count = 0;
}


vtkActivationRegionStats::~vtkActivationRegionStats()
{
    if (this->RegionVoxels != NULL)
    {
        this->RegionVoxels->Delete();
    }
}


vtkFloatArray *vtkActivationRegionStats::GetRegionVoxels()
{
    return this->RegionVoxels;
}


void vtkActivationRegionStats::SimpleExecute(vtkImageData *inputs, vtkImageData* output)
{
    if (this->NumberOfInputs != 2)
    {
        vtkErrorMacro( << "This filter can only accept two input images.");
        return;
    }
    // this->NumberOfInputs == 2
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

        // Number of inputs == 2 means we are going to compute stats 
        // for t volume: the first volume is the label map volume and 
        // and the second is the t volume.
        int indx = 0;
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
                    }
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
        }

        delete [] t;
        delete [] x;
        delete [] y;
        delete [] z;
    }
} 


// If the output image of a filter has different properties from the input image
// we need to explicitly define the ExecuteInformation() method
void vtkActivationRegionStats::ExecuteInformation(vtkImageData *input, vtkImageData *output)
{
    this->vtkSimpleImageToImageFilter::ExecuteInformation();

    if (this->NumberOfInputs == 2 && this->Count > 0)
    {
        output->SetWholeExtent(0, this->Count-1, 0, 0, 0, 0);
        output->SetScalarType(VTK_FLOAT);
        output->SetOrigin(this->GetInput(0)->GetOrigin());
        output->SetSpacing(this->GetInput(0)->GetSpacing());
        output->SetNumberOfScalarComponents(1);
    }
}


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
    this->Count = 0;
    this->Max = 0.0; 
    this->Min = 0.0;
    this->Mean = 0.0;
    this->Indices = NULL; 
    this->Intensities = NULL;
}


vtkActivationRegionStats::~vtkActivationRegionStats()
{
    if (this->Indices != NULL)
    {
        this->Indices->Delete();
    }
    if (this->Intensities != NULL)
    {
        this->Intensities->Delete();
    }
}


vtkShortArray *vtkActivationRegionStats::GetRegionVoxels()
{
    return this->Indices;
}


void vtkActivationRegionStats::SimpleExecute(vtkImageData *inputs, vtkImageData* output)
{
    // we are not going to use the output 
    output = NULL;

    if (this->NumberOfInputs < 1)
    {
        vtkErrorMacro( << "This filter has no input of image data.");
        return;
    }
    else if (this->NumberOfInputs > 2)
    {
        vtkErrorMacro( << "This filter has too many input of image data.");
        return;
    }
    else if (this->NumberOfInputs == 1)
    {
        int dim[3];  
        this->GetInput(0)->GetDimensions(dim);

        // Array holding the indices of all voxels in
        // the defined ROI.
        vtkShortArray *tmpArray = vtkShortArray::New();
        tmpArray->SetNumberOfTuples(dim[0]*dim[1]*dim[2]);
        tmpArray->SetNumberOfComponents(3);
        
        int indx = 0;
        // Voxel iteration through the entire image volume
        for (int kk = 0; kk < dim[2]; kk++)
        {
            for (int jj = 0; jj < dim[1]; jj++)
            {
                for (int ii = 0; ii < dim[0]; ii++)
                {
                    short *l  
                        = (short *)this->GetInput(0)->GetScalarPointer(ii, jj, kk);
                    if (*l == this->Label)
                    {
                        tmpArray->SetComponent(indx, 0, (short)ii);
                        tmpArray->SetComponent(indx, 1, (short)jj);
                        tmpArray->SetComponent(indx, 2, (short)kk);
                        indx++;
                    }
                }
            } 
        }

        if (indx > 0) 
        {
            // Array holding the indices of all voxels in
            // the defined ROI.
            if (this->Indices != NULL)
            {
                this->Indices->Delete();
            }
            this->Indices = vtkShortArray::New();
            this->Indices->SetNumberOfTuples(indx);
            this->Indices->SetNumberOfComponents(3);

            for (int ii = 0; ii < indx; ii++) {
                short c1 = (short)tmpArray->GetComponent(ii, 0);
                short c2 = (short)tmpArray->GetComponent(ii, 1);
                short c3 = (short)tmpArray->GetComponent(ii, 2);

                // always use InsertTuple3, instead of SetTuple3
                // since the former handles memory allocation if needed.
                this->Indices->InsertTuple3(ii, c1, c2, c3);
            }
            tmpArray->Delete();
        }
        else
        {
            this->Indices->Delete();
            this->Indices = NULL;
        }
    }
    // this->NumberOfInputs == 2
    else
    {
        int dim[3];  
        this->GetInput(0)->GetDimensions(dim);

        // Array holding the intensities of all voxels
        // in the defined ROI.
        if (this->Intensities == NULL)
        {
            this->Intensities = vtkFloatArray::New();
            this->Intensities->SetNumberOfTuples(dim[0]*dim[1]*dim[2]);
            this->Intensities->SetNumberOfComponents(1);
        }

        // Number of inputs == 2 means we are going to compute stats 
        // for t volume: the first volume is the label map volume and 
        // and the second is the t volume.
        int indx = 0;
        float total = 0.0;
        // Voxel iteration through the entire image volume
        for (int kk = 0; kk < dim[2]; kk++)
        {
            for (int jj = 0; jj < dim[1]; jj++)
            {
                for (int ii = 0; ii < dim[0]; ii++)
                {
                    short *l  
                        = (short *)this->GetInput(0)->GetScalarPointer(ii, jj, kk);
                    if (*l == this->Label)
                    {
                        float *t  
                            = (float *)this->GetInput(1)->GetScalarPointer(ii, jj, kk);
                        this->Intensities->SetComponent(indx++, 0, *t);
                        total += *t;
                    }
                }
            } 
        }

        this->Intensities->Resize(indx);

        // ROI stats
        double range[2];
        this->Intensities->GetRange(range, 0);
        this->Min = (float) range[0];
        this->Max = (float) range[1];
        this->Mean = total / indx;
        this->Count = indx;
    }
}


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


#include "vtkActivationVolumeGenerator.h"
#include "vtkObjectFactory.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkCommand.h"

#include <stdio.h>


vtkStandardNewMacro(vtkActivationVolumeGenerator);


vtkActivationVolumeGenerator::vtkActivationVolumeGenerator()
{
    this->NumberOfVolumes = 0;
    this->ContrastVector = NULL;
}


vtkActivationVolumeGenerator::~vtkActivationVolumeGenerator()
{
}


void vtkActivationVolumeGenerator::SetContrastVector(vtkIntArray *vec)
{
    this->ContrastVector = vec;
}


void vtkActivationVolumeGenerator::SetNumberOfVolumes(int vols)
{
    this->NumberOfVolumes = vols;
}


void vtkActivationVolumeGenerator::SimpleExecute(vtkImageData *input, vtkImageData* output)
{
    if (this->GetInput() == NULL)
    {
        vtkErrorMacro( << "No input image data in this filter.");
        return;
    }

    // for progress update (bar)
    unsigned long count = 0;
    unsigned long target;

    // Sets up properties for output vtkImageData
    int imgDim[3];  
    this->GetInput()->GetDimensions(imgDim);
    output->SetScalarType(VTK_FLOAT);
    output->SetOrigin(this->GetInput()->GetOrigin());
    output->SetSpacing(this->GetInput()->GetSpacing());
    output->SetNumberOfScalarComponents(1);
    output->SetDimensions(imgDim[0], imgDim[1], imgDim[2]);
    output->AllocateScalars();
  
    target = (unsigned long)(imgDim[0]*imgDim[1]*imgDim[2] / 50.0);
    target++;

    int indx = 0;
    vtkDataArray *scalarsOutput = output->GetPointData()->GetScalars();

    vtkDataArray *scalarsInput = this->GetInput()->GetPointData()->GetScalars();
    int numOfEVs = (scalarsInput->GetNumberOfTuples()) / 2;

    // set up the contrast vector
    int lenOfContrastVec = this->ContrastVector->GetNumberOfTuples();
    if (lenOfContrastVec > numOfEVs) 
    {
        vtkErrorMacro( << "The length of the contrast vector is greater than the number of total EVs.");
        return;
    }

    int *contrastVec = new int [lenOfContrastVec];
    if (contrastVec == NULL)
    {
        vtkErrorMacro( << "Memory allocation failed.");
        return;
    }

    for (int i = 0; i < lenOfContrastVec; i++)
    {
        contrastVec[i] = (int)this->ContrastVector->GetComponent(i, 0);
    }


    float *beta = new float [lenOfContrastVec]; 
    float *cov  = new float [lenOfContrastVec];
    if (beta == NULL || cov == NULL)
    {
        vtkErrorMacro( << "Memory allocation failed.");
        return;
    }

    // Voxel iteration through the entire image volume
    for (int kk = 0; kk < imgDim[2]; kk++)
    {
        for (int jj = 0; jj < imgDim[1]; jj++)
        {
            for (int ii = 0; ii < imgDim[0]; ii++)
            {
                // computes ...
                float newBeta = 0.0;
                float newcov = 0.0;
                int yy = 0;
                for (int d = 0; d < lenOfContrastVec; d++) {
                    beta[d] = scalarsInput->GetComponent(indx, yy++);
                    cov[d] = scalarsInput->GetComponent(indx, yy++);

                    beta[d] = beta[d] * contrastVec[d];
                    newBeta = newBeta + beta[d];

                    cov[d] = cov[d] * abs(contrastVec[d]);
                    newcov = newcov + cov[d];
                }

                float t = 0.0; 
                if (newcov != 0.0)
                {
                    t = newBeta / sqrt(newcov / this->NumberOfVolumes); 
                    if (t < 0.0)
                    {
                        t = t * (-1);
                    }
                }

                scalarsOutput->SetComponent(indx++, 0, t);

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
    delete [] contrastVec;

    // Scales the scalar values in the activation volume between 0 - 100
    vtkFloatingPointType range[2];
    float value;
    float newValue;
    output->GetScalarRange(range);
    this->LowRange = range[0];
    this->HighRange = range[1];
    for (int i = 0; i < indx; i++)
    {
        value = scalarsOutput->GetComponent(i, 0); 
        newValue = 100 * (value - range[0]) / (range[1] - range[0]);
        scalarsOutput->SetComponent(i, 0, newValue);

        if (!(count%target))
        {
            UpdateProgress(count / 2 / (50.0*target));
        }
        count++;
    }
}

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
(c) Copyright 2005 Massachusetts Institute of Technology (MIT) All Rights Reserved.

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

#include "vtkObjectFactory.h"
#include "vtkActivationVolumeCaster.h"
#include "vtkSource.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkDataArray.h"
#include <fstream.h>


vtkStandardNewMacro(vtkActivationVolumeCaster);

vtkActivationVolumeCaster::vtkActivationVolumeCaster()
{
}

void vtkActivationVolumeCaster::SimpleExecute(vtkImageData *input, vtkImageData* output)
{
    if (this->GetInput() == NULL)
    {
        vtkErrorMacro( << "No input image data in this filter.");
        return;
    }

    // Sets up properties for output vtkImageData
    int imgDim[3];  
    input->GetDimensions(imgDim);
    output->SetScalarType(VTK_SHORT);
    output->SetOrigin(input->GetOrigin());
    output->SetSpacing(input->GetSpacing());
    output->SetNumberOfScalarComponents(1);
    output->SetDimensions(imgDim[0], imgDim[1], imgDim[2]);
    output->AllocateScalars();
 
    int indx = 0;
    vtkDataArray *scalarsOutput = output->GetPointData()->GetScalars();
    vtkDataArray *scalarsInput = input->GetPointData()->GetScalars();

    float low = fabs(this->LowerThreshold);
    float high = fabs(this->UpperThreshold);

    // Apply threshold if desired
    if (low <= high) 
    {
        // Voxel iteration through the entire image volume
        for (int kk = 0; kk < imgDim[2]; kk++)
        {
            for (int jj = 0; jj < imgDim[1]; jj++)
            {
                for (int ii = 0; ii < imgDim[0]; ii++)
                {
                    short val = 0;
                    float v = (float) scalarsInput->GetComponent(indx, 0);

                    // Zero out values according to the FMRI mapping:
                    //
                    // |-------|-------|------|------|--------|-------|
                    // Min    -h      -l      0      +l      +h       Max
                    // show values between -h and -l, and
                    //      values between +l and +h
                    // zero out values less than -h, and
                    //      values between -l and +l, and
                    //      values above +h
                    if ((v < low && v > (-low))  ||
                            (v > high)               ||
                            (v < (-high)))           
                    {
                        val = 0;
                    }
                    else
                    {
                        // All remaining values are kept positive shorts.
                        // A volume may appear differently in slicer if we 
                        // turn on/off the interpolation.
                        val = (short) (ceil(fabs(v)));
                    }
                    scalarsOutput->SetComponent(indx++, 0, val);
                }
            }
        }
    }    
    // Zero out everything
    else
    {
        short *ptr = (short *) output->GetScalarPointer();
        memset(ptr, 0, imgDim[0]*imgDim[1]*imgDim[2]*sizeof(short));
    }

    double range[2];
    output->GetScalarRange(range);
    this->LowRange = (short)range[0];
    this->HighRange = (short)range[1];
}


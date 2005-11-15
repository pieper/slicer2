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


#include "vtkLabelMapWhitening.h"
#include "vtkObjectFactory.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkCommand.h"


vtkStandardNewMacro(vtkLabelMapWhitening);


vtkLabelMapWhitening::vtkLabelMapWhitening()
{
}


vtkLabelMapWhitening::~vtkLabelMapWhitening()
{
}


void vtkLabelMapWhitening::SimpleExecute(vtkImageData *input, vtkImageData* output)
{
    if (this->NumberOfInputs == 0)
    {
        vtkErrorMacro( << "This filter needs one input of image data.");
        return;
    }

    // Sets up properties for output vtkImageData
    int imgDim[3];  
    input->GetDimensions(imgDim);
    output->SetScalarType(input->GetScalarType());
    output->SetOrigin(input->GetOrigin());
    output->SetSpacing(input->GetSpacing());
    output->SetNumberOfScalarComponents(1);
    output->SetDimensions(imgDim[0], imgDim[1], imgDim[2]);
    output->AllocateScalars();
 
    int indx = 0;
    vtkDataArray *scalarsOutput = output->GetPointData()->GetScalars();
    vtkDataArray *scalarsInput = input->GetPointData()->GetScalars();

    // Voxel iteration through the entire image volume
    for (int kk = 0; kk < imgDim[2]; kk++)
    {
        for (int jj = 0; jj < imgDim[1]; jj++)
        {
            for (int ii = 0; ii < imgDim[0]; ii++)
            {
                short l = (short) scalarsInput->GetComponent(indx, 0);
                short val = (l > 0 ? (500+l) : 0); 
                scalarsOutput->SetComponent(indx++, 0, val);
            }
        } 
    }
}


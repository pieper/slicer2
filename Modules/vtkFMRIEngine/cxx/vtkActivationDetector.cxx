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

#include "vtkActivationDetector.h"
#include "vtkObjectFactory.h"
#include "GeneralLinearModel.h"
#include "FMRIEngineConstants.h"


vtkStandardNewMacro(vtkActivationDetector);


void vtkActivationDetector::PrintSelf(ostream& os, vtkIndent indent)
{
    vtkObject::PrintSelf(os, indent);

    os << indent << "Activation Detection method: ";
    if (this->DetectionMethod == ACTIVATION_DETECTION_METHOD_GLM)
    {
        os << "GLM.\n";
    }
    else
    {
        os << "MI.\n";
    }
}


vtkActivationDetector::vtkActivationDetector()
{
    this->Dimensions = NULL;
    this->DesignMatrix = NULL;
    this->NoOfRegressors = 0;
}


vtkActivationDetector::~vtkActivationDetector()
{
    if (this->DesignMatrix != NULL)
    {
        for (int i = 0; i < this->Dimensions[0]; i++)
        {
            delete [] this->DesignMatrix[i];
        } 
        delete [] this->DesignMatrix;
    }
    if (this->Dimensions != NULL)
    {
        delete [] this->Dimensions;
    }
}


void vtkActivationDetector::SetRegressors(vtkFloatArray *regressors)
{
    this->Regressors = regressors;
    this->NoOfRegressors = this->Regressors->GetNumberOfComponents();

    if (this->Dimensions == NULL)
    {
        this->Dimensions = new int[2];
    }  

    // Number of volumes
    this->Dimensions[0] = this->Regressors->GetNumberOfTuples();
    // Number of evs (predictors)
    this->Dimensions[1] = this->NoOfRegressors;

    if (this->DesignMatrix == NULL)
    {
        this->DesignMatrix = new float *[this->Dimensions[0]];
        for (int i = 0; i < this->Dimensions[0]; i++)
        {
            this->DesignMatrix[i] = new float[this->Dimensions[1]];
            for (int j = 0; j < this->Dimensions[1]; j++)
            {
                this->DesignMatrix[i][j] = this->Regressors->GetComponent(i,j);
            }
        } 
    }
}

 
void vtkActivationDetector::Detect(vtkFloatArray *timeCourse, float *beta, float *cov)
{
    if (this->DetectionMethod == ACTIVATION_DETECTION_METHOD_GLM)
    {
        float *tcArray = timeCourse->GetPointer(0);
        GeneralLinearModel::FitModel(this->DesignMatrix, 
                                     this->Dimensions, 
                                     tcArray,
                                     beta,
                                     cov); 
    }
}


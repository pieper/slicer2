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

// .NAME vtkGLMDetector - Computes voxel activation   
// .SECTION Description
// vtkGLMDetector is used to compute voxel activation based on
// paradigm and detection method (GLM or MI).


#ifndef __vtkGLMDetector_h
#define __vtkGLMDetector_h


#include <vtkFMRIEngineConfigure.h>
#include "vtkActivationDetector.h"
#include "vtkFloatArray.h"
#include <iostream>
#include <fstream>
using namespace std;

class VTK_FMRIENGINE_EXPORT vtkGLMDetector : public vtkActivationDetector 
{
    public:
    static vtkGLMDetector *New();
    vtkTypeMacro(vtkGLMDetector, vtkActivationDetector);

    vtkGLMDetector();
    ~vtkGLMDetector();

    // Description:
    // Gets the design matrix 
    vtkFloatArray *GetDesignMatrix();

    // Description:
    // Sets the design matrix 
    void SetDesignMatrix(vtkFloatArray *designMat);
    void SetAR1DesignMatrix ( );
    vtkFloatArray *GetAR1DesignMatrix ( );
    vtkFloatArray *GetResiduals ( );
    
    // Description:
    // Fits linear model (voxel by voxel) 
    void FitModel(vtkFloatArray *timeCourse, float *beta, float *chisq ); 

    // This uses the first estimated beta to subtract
    // the model from the data to compute errors like so:
    // Y = XB + e --> e = Y-XB_hat
    void ComputeResiduals ( vtkFloatArray *timeCourse, float *beta );
    // Description::
    // This uses the residuals to compute the correlation coefficient
    // at lag 1 used in pre-whitening for data and residuals.
    float ComputeCorrelationCoefficient ( );
    // Description:
    // This whitens the DesignMatrix and timeCourse.
    // Saves the whitened design matrix in AR1DesignMatrix,
    // and replaces the extracted timecourse by new values.
    void PreWhitenDataAndResiduals (vtkFloatArray *timeCourse, float corrCoeff);
    // Description:
    // Sets the AR1DesignMatrix
    // and turns on the whitening flag.
    void EnableAR1Modeling ( );
    // Description:
    // This sets the AR1DesignMatrix to NULL,
    // sets the WhiteningMatrix to NULL,
    // and turns off the whitening flag.
    void DisableAR1Modeling ( );


    private:

    ofstream logfile;
    int NoOfRegressors;
    vtkFloatArray *DesignMatrix;
    // pre-whitened design matrix
    vtkFloatArray *AR1DesignMatrix;
    vtkFloatArray *residuals;
    
};


#endif

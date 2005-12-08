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

#include "vtkGLMDetector.h"
#include "vtkObjectFactory.h"
#include "GeneralLinearModel.h"
#include "FMRIEngineConstants.h"

vtkStandardNewMacro(vtkGLMDetector);


vtkGLMDetector::vtkGLMDetector()
{
    this->DesignMatrix = NULL;
    this->AR1DesignMatrix = NULL;
    this->residuals = NULL;

    //    this->logfile.open ( "dumper.txt" );
}


vtkGLMDetector::~vtkGLMDetector()
{

    //--- Haiying didn't free this; maybe it
    //--- needs to stay around....
    //if ( this->DesignMatrix != NULL) {
        // this->DesignMatrix->Delete ( );
    //}
    if (this->AR1DesignMatrix != NULL) {
        this->AR1DesignMatrix->Delete ( );
    }
    if (this->residuals != NULL) {
        this->residuals->Delete ( );
    }

    //    this->logfile.close ();
}



vtkFloatArray *vtkGLMDetector::GetDesignMatrix()
{
    return this->DesignMatrix;
}




void vtkGLMDetector::SetDesignMatrix(vtkFloatArray *designMat)
{
    this->DesignMatrix = designMat;
    GeneralLinearModel::SetDesignMatrix(designMat);
}





 
void vtkGLMDetector::FitModel(vtkFloatArray *timeCourse, float *beta, float *chisq )
{
    if (this->DetectionMethod == ACTIVATION_DETECTION_METHOD_GLM)
    {
        float *tcArray = timeCourse->GetPointer(0);
        GeneralLinearModel::FitModel(tcArray, beta, chisq); 
    }
}





// (wjp------------------------------------------------
void vtkGLMDetector::SetAR1DesignMatrix ( )
{
    GeneralLinearModel::SetAR1DesignMatrix( this->AR1DesignMatrix );
}




vtkFloatArray *vtkGLMDetector::GetResiduals ( )
{
    return this->residuals;
}





vtkFloatArray *vtkGLMDetector::GetAR1DesignMatrix ( )
{
    return this->AR1DesignMatrix;
}







void vtkGLMDetector::ComputeResiduals ( vtkFloatArray *timeCourse, float *beta )
{
    // This method computes the error vector e when the parameter estimate
    // beta is used to fit observed data Y to the linear model Y = Xbeta + e
    // The residuals are found by subtracting the model from the data:
    // e = Y-Xbeta. The observed data Y is given by vtkFloatArray timeCourse.
    int numSamples, numRegressors;
    int i, j;
    double zz;
    float e, X, Y;

    // dim = number of components in the timeCourse.    
    numSamples = timeCourse->GetNumberOfTuples ( );
    numRegressors = this->DesignMatrix->GetNumberOfComponents ( );
    
    // create new array for residuals if needed.
    if ( this->residuals == NULL ) {
        this->residuals = vtkFloatArray::New ();
        this->residuals->SetNumberOfComponents (1);
        this->residuals->SetNumberOfTuples (numSamples);
    }

    // compute residuals: e = Y-X*beta.
    for ( i=0; i<numSamples; i++ ) {
        // first compute X*beta:
        zz=0.0;
        for (j=0; j<numRegressors; j++ ) {
            X = this->DesignMatrix->GetComponent ( i,j );
            zz = zz+ (double)(X * beta[j]);
        }
        // now compute Y-X*beta:
        Y = timeCourse->GetComponent ( i,0 );
        e = Y - (float)zz;
        // and set the residual 
        residuals->SetComponent ( i,0,e );
    }
}





float vtkGLMDetector::ComputeCorrelationCoefficient ( )
{
    // The pre-whitening of data and residuals
    // uses the correlation coefficient at lag 1 
    float p, norm, e1, e0;
    int dim, i;

    // This correlation coefficient p(1) computed as recommended in:
    // Worsley, K.J., Liao, C., Aston, J., Petre, V., Duncan, G.H., Morales, F., Evans, A.C.
    // (2002). A general statistical analysis for fMRI data. NeuroImage, 15:1:15.
    
    dim = this->residuals->GetNumberOfTuples ( );    
    e1 = this->residuals->GetComponent(0,0);
    // since there's no sample prior to this one,
    // not sure which to choose!
    // p = e1*e1
    p = 0.0;
    norm = (e1*e1);
    for ( i=1; i<dim; i++ ) {
        e1 = this->residuals->GetComponent ( i,0 );
        e0 = this->residuals->GetComponent ( i-1,0 );
        p = p+ (e1*e0);
        norm = norm+ (e1*e1);
    }

    // avoid possible div by 0
    if ( norm != 0.0) {
        p = p/norm;
    }
    if ( p == 1.0 ) {
        p = 0.99999999;
    }
    return p;
}



void vtkGLMDetector::PreWhitenDataAndResiduals (vtkFloatArray *timeCourse, float corrCoeff)
{
    int rows, cols, i, j;
    float v0, v1, v, nrm;
    
    // This autoregressive modeling of temporal autocorrelation structure AR(1) modeling is as recommended in:
    // Worsley, K.J., Liao, C., Aston, J., Petre, V., Duncan, G.H., Morales, F., Evans, A.C.
    // (2002). A general statistical analysis for fMRI data. NeuroImage, 15:1:15.


    // Allocate, get dimensions and set up;
    cols = this->DesignMatrix->GetNumberOfComponents();
    rows = this->DesignMatrix->GetNumberOfTuples ( );
    if ( this->AR1DesignMatrix == NULL ) {
        this->AR1DesignMatrix = vtkFloatArray::New ();
        this->AR1DesignMatrix->SetNumberOfComponents (cols);
        this->AR1DesignMatrix->SetNumberOfTuples (rows);
    }

    // Compute AR1DesignMatrix (pre-whiten the residuals)
    nrm = (float) (sqrt ( (double) (1.0- (corrCoeff*corrCoeff))));
    for (j=0; j<cols; j++) {
        v = this->DesignMatrix->GetComponent (0,j);
        AR1DesignMatrix->SetComponent ( 0,j,v );
        for (i=1; i<rows; i++) {
            v1 = this->DesignMatrix->GetComponent (i,j);
            v0 = this->DesignMatrix->GetComponent(i-1,j);
            v =  (v1 - corrCoeff*v0) / nrm;
            AR1DesignMatrix->SetComponent ( i,j,v );
        }
    }
    // pre-whiten timeCourse 
    for (i=1; i<rows; i++) {
        v1 = timeCourse->GetComponent (i,0);
        v0 = timeCourse->GetComponent (i-1,0);
        v =  (v1 - corrCoeff*v0) / nrm;
        timeCourse->SetComponent (i,0,v);
    }


    // for debugging

    char str[256];
    for (j=0; j<cols; j++) {
        for (i=0; i<rows; i++) {
            v0 = this->DesignMatrix->GetComponent (i,j);
            v1 = this->AR1DesignMatrix->GetComponent (i,j);
            sprintf( str, "%d,%d: [ %f  %f ]\n", i,j,v0,v1);
            this->logfile << str;
        }
    }

}






void vtkGLMDetector::EnableAR1Modeling ( )
{
    // Set whitening and switch it on for the detector.
    this->SetAR1DesignMatrix ( );
    GeneralLinearModel::SetWhitening (1); 

}






void vtkGLMDetector::DisableAR1Modeling ( )
{
    GeneralLinearModel::SetWhitening (0);
}





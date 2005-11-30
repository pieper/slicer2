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

#include "vtkGLMVolumeGenerator.h"
#include "vtkObjectFactory.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkCommand.h"

#include "vnl/vnl_matrix.h" 
#include "vnl/algo/vnl_matrix_inverse.h" 


vtkStandardNewMacro(vtkGLMVolumeGenerator);


vtkGLMVolumeGenerator::vtkGLMVolumeGenerator()
{
    this->StandardError = 0.0;
    this->SizeOfContrastVector = 0;
    this->beta = NULL; 
    this->ContrastVector = NULL;
    this->DesignMatrix = NULL;

    this->X = NULL;
    this->C = NULL;
}


vtkGLMVolumeGenerator::~vtkGLMVolumeGenerator()
{
    if (this->beta != NULL)
    {
        delete [] this->beta;
    }

    if (this->X != NULL)
    {
        delete ((vnl_matrix<float> *)this->X);
    }

    if (this->C != NULL)
    {
        delete ((vnl_matrix<float> *)this->C);
    }
}


void vtkGLMVolumeGenerator::SetContrastVector(vtkIntArray *vec)
{
    this->ContrastVector = vec;
    this->SizeOfContrastVector = this->ContrastVector->GetNumberOfTuples();
    this->beta = new float [this->SizeOfContrastVector]; 
    if (this->beta == NULL)
    {
        vtkErrorMacro( << "Memory allocation failed.");
        return;
    }

    // instantiate C
    if (this->C == NULL)
    {
        this->C = new vnl_matrix<float>;
    }
    ((vnl_matrix<float> *)this->C)->set_size(1, this->SizeOfContrastVector);
    for (int i = 0; i < this->SizeOfContrastVector; i++)
    {
        ((vnl_matrix<float> *)this->C)->put(0, i, vec->GetComponent(i,0));
    } 
}


void vtkGLMVolumeGenerator::SetDesignMatrix(vtkFloatArray *designMat)
{
    this->DesignMatrix = designMat;
    int rows = this->DesignMatrix->GetNumberOfTuples();
    int cols = this->DesignMatrix->GetNumberOfComponents();

    // instantiate X 
    if (this->X == NULL)
    {
        this->X = new vnl_matrix<float>;
    }
    ((vnl_matrix<float> *)this->X)->set_size(rows, cols);
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            ((vnl_matrix<float> *)this->X)->put(i, j, designMat->GetComponent(i,j));
        }
    } 
}

void vtkGLMVolumeGenerator::ComputeStandardError(float rss)
{
    // for each voxel, after a linear modeling (best fit)
    // we'll have a list of beta (one for each regressor) and
    // a chisq (or rss) - the sum of squares of the residuals from the best-fit
    // the standard error se = sqrt(mrss*(C*pinv(X'*X)*C'));
    // where C - contrast vector
    //       X - design matrix
    //       pinv - Moore-Penrose pseudoinverse 

    // calculate mrss 
    // ------------------------------------------------------
    int rows = this->DesignMatrix->GetNumberOfTuples();
    int cols = this->DesignMatrix->GetNumberOfComponents();
    int df = rows - cols;
    float mrss  = rss / df;

    // calculate pinv(X'*X) 
    // ------------------------------------------------------
    vnl_matrix<float> A = *((vnl_matrix<float> *)this->X);
    vnl_matrix<float> B;
    vnl_matrix<float> Binv;

    B = A.transpose() * A;
    vnl_matrix_inverse<float> Pinv(B);
    Binv = Pinv.pinverse(cols);

    // calculate C*pinv(X'*X)*C' 
    // ------------------------------------------------------
    vnl_matrix<float> D = *((vnl_matrix<float> *)this->C);
    vnl_matrix<float> E = D * Binv * D.transpose();
    float v = E.get(0, 0);

    // standard error: se = sqrt(mrss*(C*pinv(X'*X)*C'))
    // ------------------------------------------------------
    this->StandardError = (float)sqrt(fabs(mrss * v));
}


void vtkGLMVolumeGenerator::SimpleExecute(vtkImageData *input, vtkImageData* output)
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

    // Voxel iteration through the entire image volume
    for (int kk = 0; kk < imgDim[2]; kk++)
    {
        for (int jj = 0; jj < imgDim[1]; jj++)
        {
            for (int ii = 0; ii < imgDim[0]; ii++)
            {
                // computes ...
                float newBeta = 0.0;
                float rss = 0.0; // = chisq
                int yy = 0;
                for (int d = 0; d < this->SizeOfContrastVector; d++) {
                    this->beta[d] = scalarsInput->GetComponent(indx, yy++);
                    this->beta[d] = this->beta[d] * ((int)this->ContrastVector->GetComponent(d, 0));
                    newBeta = newBeta + this->beta[d];
                }
                rss = scalarsInput->GetComponent(indx, yy);

                ComputeStandardError(rss);

                // t = C*B/SE;
                // where B - beta matrix afer linear modeling
                //       C - contrast vector 
                //       SE   - standard error
                //       t statistic 
                float t = 0.0; 
                if (this->StandardError != 0.0)
                {
                    t = newBeta / this->StandardError; 
                }

                scalarsOutput->SetComponent(indx++, 0, t);

                if (!(count%target))
                {
                    UpdateProgress(count / (50.0*target));
                }
                count++;
            }
        } 
    }

    delete [] this->beta;

    vtkFloatingPointType range[2];
    output->GetScalarRange(range);
    this->LowRange = range[0];
    this->HighRange = range[1];
}



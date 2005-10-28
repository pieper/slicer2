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


#include "vtkActivationVolumeGenerator.h"
#include "vtkObjectFactory.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkCommand.h"

#include <stdio.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_linalg.h>


vtkStandardNewMacro(vtkActivationVolumeGenerator);



vtkActivationVolumeGenerator::vtkActivationVolumeGenerator()
{
    this->StandardError = 0.0;
    this->SizeOfContrastVector = 0;
    this->beta = NULL; 
    this->ContrastVector = NULL;
    this->DesignMatrix = NULL;

    this->c = NULL;
    this->X = NULL;
    this->A = NULL;
    this->V = NULL; 
    this->S = NULL;
    this->Z = NULL;
    this->Sv = NULL;
    this->work = NULL;
    this->c2 = NULL; 
    this->result = NULL;
 
}


vtkActivationVolumeGenerator::~vtkActivationVolumeGenerator()
{
    if (this->beta != NULL)
    {
        delete [] this->beta;
    }

    if (this->X != NULL)
    {
        gsl_matrix_free(X);
    }
    if (this->A != NULL)
    {
        gsl_matrix_free(A);
    }
    if (this->V != NULL)
    {
        gsl_matrix_free(V);
    }
    if (this->S != NULL)
    {
        gsl_matrix_free(S);
    }
    if (this->Z != NULL)
    {
        gsl_matrix_free(Z);
    }
    if (this->c != NULL)
    {
        gsl_matrix_free(c);
    }
    if (this->c2 != NULL)
    {
        gsl_matrix_free(c2);
    }
    if (this->result != NULL)
    {
        gsl_matrix_free(result);
    }

    if (this->Sv != NULL)
    {
        gsl_vector_free(Sv);
    }
    if (this->work != NULL)
    {
        gsl_vector_free(work);
    }
}


void vtkActivationVolumeGenerator::SetContrastVector(vtkIntArray *vec)
{
    this->ContrastVector = vec;
    this->SizeOfContrastVector = this->ContrastVector->GetNumberOfTuples();
    this->beta = new float [this->SizeOfContrastVector]; 
    if (this->beta == NULL)
    {
        vtkErrorMacro( << "Memory allocation failed.");
    }
    if (this->c == NULL)
    {
        this->c = gsl_matrix_alloc(1,this->SizeOfContrastVector);
    }
    for (int i = 0; i < this->SizeOfContrastVector; i++)
    {
        gsl_matrix_set(c, 0, i, vec->GetComponent(i,0));
    } 

    if (this->c2 == NULL)
    {
        this->c2 = gsl_matrix_alloc(1,this->SizeOfContrastVector);
    }
}


void vtkActivationVolumeGenerator::SetDesignMatrix(vtkFloatArray *designMat)
{
    this->DesignMatrix = designMat;

    int rows = this->DesignMatrix->GetNumberOfTuples();
    int cols = this->DesignMatrix->GetNumberOfComponents();

    if (this->X == NULL)
    {
        this->X = gsl_matrix_alloc(rows, cols);
    }
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            gsl_matrix_set(X, i, j, designMat->GetComponent(i,j));
        }
    } 

    if (this->A == NULL)
    {
        this->A = gsl_matrix_alloc(cols, cols);
    }
    if (this->V == NULL)
    {
        this->V = gsl_matrix_alloc(cols, cols);
    }
    if (this->S == NULL)
    {
        this->S = gsl_matrix_alloc(cols, cols);
    }
    if (this->Z == NULL)
    {
        this->Z = gsl_matrix_alloc(cols, cols);
    }
    if (this->result == NULL)
    {
        this->result = gsl_matrix_alloc(1, 1);
    }

    if (this->Sv == NULL)
    {
        this->Sv = gsl_vector_alloc(cols);
    }
    if (this->work == NULL)
    {
        this->work = gsl_vector_alloc(cols);
    }
}


void vtkActivationVolumeGenerator::ComputeStandardError(float rss)
{
    // ------------------------------------------------------
    // Step One: Calculates MRSS
    // ------------------------------------------------------
    int rows = this->DesignMatrix->GetNumberOfTuples();
    int cols = this->DesignMatrix->GetNumberOfComponents();
    int df = rows-cols;
    float mrss  = rss / df;

    // ------------------------------------------------------
    // Step Two: Computes A = X'*X; X - design matrx 
    // ------------------------------------------------------
    gsl_blas_dgemm (CblasTrans, CblasNoTrans, 1.0, this->X, this->X, 0.0, this->A);

    // ------------------------------------------------------
    // Step Three: Singular value decompostion of A 
    // ------------------------------------------------------
    // after decomposition, A is replaced by U
    // Sv - a vector holding all diagonal values
    gsl_linalg_SV_decomp (this->A, this->V, this->Sv, this->work);

    // ------------------------------------------------------
    // Step Four: Computes Moore-Penrose pseudoinverse of A
    //            i.e. X'*X
    //            pinv(A) = U*pinv(S)*V'
    // ------------------------------------------------------

    // Inverses each no-zero element of Sv to get pinv(Sv)
    for (int i = 0; i < cols; i++)
    {
        float v = gsl_vector_get(Sv,i);
        if (v != 0) {
            v = 1/v;
        }
        gsl_vector_set(Sv, i, v);
    }

    // Fills each element to get pinv(S) 
    // Sv holds all diagonal values of pinv(S)
    for (int i = 0; i < cols; i++)
    {
        float v = gsl_vector_get(Sv,i);
        for (int j = 0; j < cols; j++)
        {
            if (i == j)
            {
                gsl_matrix_set(this->S, i, j, v);
            }
            else 
            {
                gsl_matrix_set(this->S, i, j, 0);
            }
        }
    }

    // U*pinv(S)
    gsl_blas_dgemm (CblasNoTrans, CblasNoTrans, 1.0, this->A, this->S, 0.0, this->Z);
    // U*pinv(S)*V'
    gsl_blas_dgemm (CblasNoTrans, CblasTrans, 1.0, this->Z, this->V, 0.0, this->A);
    // C*pinv(X'*X)
    gsl_blas_dgemm (CblasNoTrans, CblasNoTrans, 1.0, this->c, this->A, 0.0, this->c2);
    // C*pinv(X'*X)*C'
    gsl_blas_dgemm (CblasNoTrans, CblasTrans, 1.0, this->c2, this->c, 0.0, this->result);

    // ------------------------------------------------------
    // Step Five: Computes SE for the voxel on the basis of 
    //            the given contrast
    // ------------------------------------------------------
    float r = (float)gsl_matrix_get(this->result, 0, 0); 
    this->StandardError = (float)sqrt(fabs(mrss*r));
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

    // For each voxel, we have the following steps to compute t value: 
    // RSS   = sum((Y - X*B).^2);
    // MRSS  = RSS / df;
    // SE    = sqrt(MRSS*(C*pinv(X'*X)*C'));
    // t     = C*B./SE;
    // where B    - beta matrix afer linear modeling
    //       X    - the design matrix
    //       Y    - observations
    //       RSS  - the sum of squares of the residuals from the best-fit
    //       pinv - Moore-Penrose pseudoinverse 
    //       SE   - standard error
    //       t    - t statistic
    // 
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
                    beta[d] = scalarsInput->GetComponent(indx, yy++);
                    beta[d] = beta[d] * ((int)this->ContrastVector->GetComponent(d, 0));
                    newBeta = newBeta + beta[d];
                }
                rss = scalarsInput->GetComponent(indx, yy);

                ComputeStandardError(rss);

                // t statistic 
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

    delete [] beta;

    // Scales the scalar values in the activation volume between 0 - 100
    vtkFloatingPointType range[2];
    output->GetScalarRange(range);
    this->LowRange = range[0];
    this->HighRange = range[1];
}



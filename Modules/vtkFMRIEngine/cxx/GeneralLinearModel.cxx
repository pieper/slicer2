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


#include "GeneralLinearModel.h"
#include <stdio.h>
#include <math.h>
#include <gsl_multifit.h>


float GeneralLinearModel::ComputeVoxelActivation(float **designMatrix, int *dims, float *timeCourse)
{
    int i, j;
    double xi, yi, chisq, t;
    gsl_matrix *X, *cov;
    gsl_vector *y, *c;

    X = gsl_matrix_alloc (dims[0], dims[1]);
    y = gsl_vector_alloc (dims[0]);
    c = gsl_vector_alloc (dims[1]);
    cov = gsl_matrix_alloc (dims[1], dims[1]);

    for (i = 0; i < dims[0]; i++)
    {
        gsl_vector_set(y, i, timeCourse[i]);

        for(j = 0; j < dims[1]; j++)
        {
            gsl_matrix_set(X, i, j, designMatrix[i][j]);
        }
    }

    gsl_multifit_linear_workspace * work 
        = gsl_multifit_linear_alloc (dims[0], dims[1]);
    gsl_multifit_linear (X, y, c, cov, &chisq, work);
    gsl_multifit_linear_free (work);

    t = gsl_vector_get(c, 1) / sqrt(gsl_matrix_get(cov, 1, 1));

    return (float)t;
}

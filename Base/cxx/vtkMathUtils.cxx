/*=auto=========================================================================

(c) Copyright 2003 Massachusetts Institute of Technology (MIT) All Rights Reserved.

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
#include <math.h>
#include "vtkObject.h"
#include "vtkMath.h"
#include "vtkTransform.h"
#include "vtkMathUtils.h"

// Description:
// The center of mass is computed from the Points and (optionally) Weights.
// The covariance matrix is then computed and solved for principal axes
// and moments.
// The fourth vector in "Vectors" is the center of mass.

int vtkMathUtils::PrincipalMomentsAndAxes( vtkPoints *Points,
                                           vtkDataArray *Weights,
                                           vtkDataArray *Values,
                                           vtkDataArray *Vectors)
  {
  int numPts, id, ii, jj, status;
  float *covar[3], cov0[3], cov1[3], cov2[3], tmp,
        mean[3], *p, pw[3], weight, totalWeight;
  float *eigenVecs[3], eV0[3], eV1[3], eV2[3], eigenVals[3];

  mean[0] = mean[1] = mean[2] = 0.0;
  weight = 1.0;
  totalWeight = 0.0;
  covar[0] = cov0; covar[1] = cov1; covar[2] = cov2;

  // First - compute weighted means
  numPts = Points->GetNumberOfPoints();
  for ( id=0; id<numPts; id++ )
    {
    p = Points->GetPoint(id);
    if ( Weights != NULL )
      {
      weight = Weights->GetTuple1(id);
      }
    mean[0] += p[0]*weight;
    mean[1] += p[1]*weight;
    mean[2] += p[2]*weight;
    totalWeight += weight;
    }

  for ( jj=0; jj<3; jj++ )
    {
    mean[jj] /= totalWeight;
    cov0[jj] = cov1[jj] = cov2[jj] = 0.0;
    }

  // Second - compute covariance matrix
  for ( id=0; id<numPts; id++ )
    {
    p = Points->GetPoint(id);
    if ( Weights != NULL )
      {
      weight = Weights->GetTuple1(id);
      }
    pw[0] = p[0]*weight - mean[0];
    pw[1] = p[1]*weight - mean[1];
    pw[2] = p[2]*weight - mean[2];
    for ( jj=0; jj<3; jj++ )
      {
      cov0[jj] += pw[0] * pw[jj];
      cov1[jj] += pw[1] * pw[jj];
      cov2[jj] += pw[2] * pw[jj];
      }
    }
  for ( jj=0; jj<3; jj++ )
    {
    cov0[jj] /= totalWeight;
    cov1[jj] /= totalWeight;
    cov2[jj] /= totalWeight;
    }

  // Third - solve eigenvectors and eigenvalues from covariance matrix
  eigenVecs[0] = eV0; eigenVecs[1] = eV1; eigenVecs[2] = eV2;
  status = vtkMath::JacobiN( covar, 3, eigenVals, eigenVecs );

  // Fourth - copy result to output
  Values->SetNumberOfTuples( 3 );
  Vectors->SetNumberOfTuples( 4 );
  for ( jj=0; jj<3; jj++ )
    {
    Values->SetTuple1( jj, eigenVals[jj] );
    for ( ii=jj+1; ii<3; ii++ )
      {
      tmp = eigenVecs[jj][ii];
      eigenVecs[jj][ii] = eigenVecs[ii][jj];
      eigenVecs[ii][jj] = tmp;
      }
    Vectors->SetTuple( jj, eigenVecs[jj] );
    }
  Vectors->SetTuple( 3, mean );
  return status;
  }

//
// Find the least-squares solution to p' = Rp + T
// where R is a rotation matrix and T is a translation matrix
// using singular value decomposition.
//
int vtkMathUtils::AlignPoints( vtkPoints *Data, vtkPoints *Ref,
                           vtkMatrix4x4 *Xform )
  {
  int nPts, ii, jj, status;
  float *p, *p1;
  float cmData[3] = { 0.0, 0.0, 0.0 },
         cmRef[3] = { 0.0, 0.0, 0.0 },
         H[3][3] = { 0.0, 0.0, 0.0,
                     0.0, 0.0, 0.0,
                     0.0, 0.0, 0.0 };
  float (*q1)[3], (*q)[3];
  float sing[3], U[3][3], V[3][3], translate[3];
  vtkTransform *tmpXform = vtkTransform::New();

  nPts = Data->GetNumberOfPoints();
  if ( Ref->GetNumberOfPoints() != nPts )
    {
    vtkGenericWarningMacro(<< "Point numbers don't match.");
    return( -1 );
    }

  q = new float [nPts][3];
  q1 = new float [nPts][3];
  for ( ii=0; ii<nPts; ii++ )
    {
    p = Data->GetPoint( ii );
    p1 = Ref->GetPoint( ii );
    for ( jj=0; jj<3; jj++ )
      {
      cmData[jj] += p[jj];
      cmRef[jj] += p1[jj];
      q[ii][jj] = p[jj];
      q1[ii][jj] = p1[jj];
      }
    }

  for ( jj=0; jj<3; jj++ )
    {
    cmData[jj] /= nPts;
    cmRef[jj] /= nPts;
    }

  for ( ii=0; ii<nPts; ii++ )
    {
    for ( jj=0; jj<3; jj++ )
      {
      q[ii][jj] -= cmData[jj];
      q1[ii][jj] -= cmRef[jj];
      }
    for ( jj=0; jj<3; jj++ )
      {
      H[jj][0] += q[ii][jj] * q1[ii][0];
      H[jj][1] += q[ii][jj] * q1[ii][1];
      H[jj][2] += q[ii][jj] * q1[ii][2];
      }
    }

  vtkMathUtils::SVD3x3( H, U, sing, V );

  for ( ii=0; ii<3; ii++ )
    {
    for ( jj=0; jj<3; jj++ )
      {
      tmpXform->GetMatrix()->SetElement( ii, jj, U[ii][jj] );
      Xform->SetElement( ii, jj, V[jj][ii] ); // V transpose
      }
    }
  tmpXform->Concatenate( Xform );
  tmpXform->MultiplyPoint( cmRef, translate );
  Xform->DeepCopy( tmpXform->GetMatrix() );
  for ( ii=0; ii<3; ii++ )
    {
    Xform->SetElement( ii, 3, cmData[ii] - translate[ii] );
    }

  delete [] q;
  delete [] q1;
  tmpXform->Delete();

  return( 0 );
  }

#ifdef VTK_LINK_TO_EXTERNAL_SVD
extern "C" { // SVD from Numerical Recipes in C
void svdcmp( float **a, int m, int n, float w[], float **v );
}
#endif

// 
// Singular value decomposition: A = U*diag(W)*Vt
//
void vtkMathUtils::SVD3x3( float A[][3], float U[][3], float W[], float V[][3] )
  {
  int ii, jj;
  float *U1[3], *V1[3], *W1;

  // Convert the input into format compatible with foolish 1-based arrays
  // used in Numerical Recipes in C. (sneaky pointer arithmetic)
  //
  for ( ii=0; ii<3; ii++ )
    {
    for ( jj=0; jj<3; jj++ )
      {
      U[ii][jj] = A[ii][jj];
      }
    U1[ii] = &U[ii][0] - 1;
    V1[ii] = &V[ii][0] - 1;
    }
  W1 = &W[0] - 1;

#ifdef VTK_LINK_TO_EXTERNAL_SVD
  svdcmp( &U1[0]-1, 3, 3, W1, &V1[0]-1 );
#else
  cout << "look for and define VTK_LINK_TO_EXTERNAL_SVD in vtkMathUtils"
       << endl;
#endif
  }

void vtkMathUtils::Outer3(float x[3], float y[3], float A[3][3])
{
  for (int i=0; i < 3; i++)
    {
      for (int j=0; j < 3; j++)
    {
      A[i][j] = x[i]*y[j];
    }
    }
}

void vtkMathUtils::Outer2(float x[2], float y[2], float A[2][2])
{
  for (int i=0; i < 2; i++)
    {
      for (int j=0; j < 2; j++)
    {
      A[i][j] = x[i]*y[j];
    }
    }
}

/*static*/ void vtkMathUtils::MatrixMultiply(double **A, double **B, double **C, int rowA, 
               int colA, int rowB, int colB)
{
  // we need colA == rowB 
  if (colA != rowB)
    {
      vtkGenericWarningMacro("Number of columns of A must match number of rows of B, you know.");
    }
  
  // output matrix is rowA*colB

  // output row 
  for (int i=0; i < rowA; i++)
    {
      // output col
      for (int j=0; j < colB; j++)
    {
      C[i][j] = 0;
      //cout << C[i][j] << " ";
      // sum for this point
      for (int k=0; k < colA; k++)
        {
          C[i][j] += A[i][k]*B[k][j];
          //cout << A[i][k]*B[k][j] << " ";
        }
      //cout << "=" << C[i][j] << " ";
      //cout << endl;      
    }
    }
}

/*static*/ void vtkMathUtils::PrintMatrix(double **A, int rowA, int colA)
{
  int j,k;

  for (j = 0; j < rowA; j++)
    {
      for (k = 0; k < colA; k++)
    {
      cout << A[j][k] << " ";
    }
      cout << endl;
    }

}



void vtkMathUtils::PrintMatrix(double **A, int rowA, 
                               int colA, ostream& os, 
                               vtkIndent indent)
{
  int j,k;

  for (j = 0; j < rowA; j++)
    {
      os << indent;
      for (k = 0; k < colA; k++)
        {
          os << A[j][k] << " ";
        }
      os << "\n";
    }
}


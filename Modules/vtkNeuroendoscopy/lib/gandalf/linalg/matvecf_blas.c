/**
 * File:          $RCSfile: matvecf_blas.c,v $
 * Module:        C BLAS wrapper functions (single precision)
 * Part of:       Gandalf Library
 *
 * Revision:      $Revision: 1.1.2.1 $
 * Last edited:   $Date: 2007/10/14 03:16:23 $
 * Author:        $Author: ruetz $
 * Copyright:     (c) 2000 Imagineer Software Limited
 */

/* This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <gandalf/linalg/matvecf_blas.h>
#include <gandalf/linalg/cblas.h>
#include <gandalf/common/misc_error.h>

/**
 * \addtogroup LinearAlgebra
 * \{
 */

/**
 * \addtogroup GeneralSizeMatVec
 * \{
 */

/**
 * \addtogroup GeneralSizeMatVecBLAS
 * \{
 */

/**
 * \brief Function to apply the BLAS operation \c axpy.
 * \return Result vector \a y.
 *
 * Function to apply the BLAS operation \c axpy:
 * \f[
 *    y \leftarrow y + a x
 * \f]
 * for vectors \a x, \a y and scalar \a a.
 */
Gan_Vector_f *
 gan_blasf_axpy ( Gan_Vector_f *y, float a, const Gan_Vector_f *x )
{
#ifdef HAVE_LAPACK
   long onei = 1;
#endif

   /* check that the dimensions of vectors x and y are compatible */
   gan_err_test_ptr ( gan_vecf_same_size(x,y), "gan_blasf_axpy",
                      GAN_ERROR_INCOMPATIBLE, "vectors" );

   /* can't do in-place axpy, use gan_blasf_scal instead */
   gan_err_test_ptr ( y != x, "gan_blasf_axpy", GAN_ERROR_FAILURE,
                      "can't do in-place axpy" );

   /* compute y += a*x */
#ifdef HAVE_LAPACK
   saxpy_ ( (long *)&x->rows, &a, x->data, &onei, y->data, &onei );
#else
   gan_saxpy ( x->rows, a, x->data, 1, y->data, 1 );
#endif /* #ifdef HAVE_LAPACK */

   /* return result vector */
   return y;
}

/**
 * \brief Function to apply the BLAS operation \c gemv.
 * \return Result vector \a y.
 *
 * Function to apply the BLAS operation \c gemv:
 * \f[
 *   y \leftarrow \alpha a^{(\top)} x + \beta y
 * \f]
 * for vectors \a x and \a y, matrix \a a (optionally transposed) and
 * scalars \a alpha and \a beta.
 */
Gan_Vector_f *
 gan_blasf_gemv ( Gan_Vector_f *y, float alpha,
                  const Gan_Matrix_f *A, Gan_TposeFlag A_tr,
                  const Gan_Vector_f *x, float beta )
{
#ifdef HAVE_LAPACK
   long onei = 1;
#endif

   /* check that the dimensions of vectors x and y are compatible */
   gan_err_test_ptr ( (A_tr ? A->rows : A->cols) == x->rows &&
                      (A_tr ? A->cols : A->rows) == y->rows, "gan_blasf_gemv",
                      GAN_ERROR_INCOMPATIBLE, "matrix/vector" );

   /* can't gemv if x & y are the same vector */
   gan_err_test_ptr ( y != x, "gan_blasf_gemv", GAN_ERROR_FAILURE,
                      "can't do gemv with x == y" );

   /* compute y = alpha*A*x + beta*y or y = alpha*A^T*x + beta*y */
#ifdef HAVE_LAPACK
   sgemv_ ( A_tr ? "t" : "n", (long *)&A->rows, (long *)&A->cols, &alpha,
            A->data, (A->rows == 0) ? &onei : (long *)&A->rows,
            x->data, &onei, &beta, y->data, &onei );
#else
   gan_sgemv ( A_tr, A->rows, A->cols, alpha, A->data,
               (A->rows == 0) ? 1 : A->rows, x->data, 1, beta, y->data, 1 );
#endif /* #ifdef HAVE_LAPACK */

   /* return result vector */
   return y;
}

/**
 * \brief Function to apply the BLAS operation \c ger.
 * \return Updated matrix \a A.
 *
 * Function to apply the BLAS operation \c ger:
 * \f[
 *   A \leftarrow A +  a x y^{\top}
 * \f]
 * for generic matrix \a A, scalar \a a and vectors \a x and \a y.
 */
Gan_Matrix_f *
 gan_blasf_ger ( Gan_Matrix_f *A, const Gan_Vector_f *x, const Gan_Vector_f *y, float a )
{
#ifdef HAVE_LAPACK
   long onei = 1;
#endif

   /* consistency check */
   gan_err_test_ptr ( x->rows == A->rows && y->rows == A->cols,
                      "gan_blasf_ger", GAN_ERROR_INCOMPATIBLE, "" );

   /* compute A += a*x*y^T */
#ifdef HAVE_LAPACK
   sger_ ( (long *)&A->rows, (long *)&A->cols, &a, x->data, &onei,
           y->data, &onei, A->data, (long *)&A->rows );
#else
   gan_sger ( A->rows, A->cols, a, x->data, 1, y->data, 1,
              A->data, A->rows );
#endif /* #ifdef HAVE_LAPACK */

   /* return result */
   return A;
}

/**
 * \brief Function to apply the BLAS operation \c spr.
 * \return Updated symmetric matrix \a A.
 *
 * Function to apply the BLAS operation \c spr:
 * \f[
 *   A \leftarrow A + a x x^{\top}
 * \f]
 * for symmetric matrix \a A, scalar \a a and vector \a x.
 */
Gan_SquMatrix_f *
 gan_blasf_spr ( Gan_SquMatrix_f *A, const Gan_Vector_f *x, float a )
{
#ifdef HAVE_LAPACK
   long onei = 1;
#endif

   /* consistency check */
   gan_err_test_ptr ( A->type == GAN_SYMMETRIC_MATRIX && x->rows == A->size,
                      "gan_blasf_spr", GAN_ERROR_INCOMPATIBLE, "");

   /* compute A += a*x*x^T */
#ifdef HAVE_LAPACK
   sspr_ ( "U", (long *)&A->size, &a, x->data, &onei, A->data );
#else
   gan_sspr ( GAN_MATRIXPART_UPPER, A->size, a, x->data, 1, A->data );
#endif /* #ifdef HAVE_LAPACK */

   /* return result */
   return A;
}

/**
 * \brief Function to apply the BLAS operation \c scal.
 * \return Updated vector \a x.
 *
 * Function to apply the BLAS operation \c scal:
 * \f[
 *   x \leftarrow a x
 * \f]
 * for vector \a x and scalar \a a.
 */
Gan_Vector_f *
 gan_blasf_scal ( Gan_Vector_f *x, float a )
{
#ifdef HAVE_LAPACK
   long onei = 1;
#endif

   /* compute x *= a */
#ifdef HAVE_LAPACK
   sscal_ ( (long *)&x->rows, &a, x->data, &onei );
#else
   gan_sscal ( x->rows, a, x->data, 1 );
#endif /* #ifdef HAVE_LAPACK */

   /* return result vector */
   return x;
}

/**
 * \brief Function to apply the BLAS operation \c gemm.
 * \return Matrix \a C.
 *
 * Function to apply the BLAS operation \c gemm:
 * \f[
 *      C \leftarrow \alpha A^{(\top)} B^{(\top)} + \beta C
 * \f]
 * for matrices \a A, \a B (either of which is optionally transposed) and \a C,
 * and scalars \a alpha and \a beta.
 */
Gan_Matrix_f *
 gan_blasf_gemm ( Gan_Matrix_f *C, float alpha,
                  const Gan_Matrix_f *A, Gan_TposeFlag A_tr,
                  const Gan_Matrix_f *B, Gan_TposeFlag B_tr, float beta )
{
#ifdef HAVE_LAPACK
   long onei = 1;
#endif

   /* check that the dimensions of matrices A,B,C are compatible */
   gan_err_test_ptr ( C->rows == A->rows && C->cols == B->cols &&
                      A->cols == B->rows, "gan_blasf_gemm",
                      GAN_ERROR_INCOMPATIBLE, "matrices" );

   /* can't gemm if A & C or B & C are the same matrix */
   gan_err_test_ptr ( A != C && B != C, "gan_blasf_gemm", GAN_ERROR_FAILURE,
                      "can't do gemm with A == C or B == C" );

   /* compute C = alpha*A(^T)*B(^T) + beta*C */
#ifdef HAVE_LAPACK
   sgemm_ ( A_tr ? "t" : "n", B_tr ? "t" : "n",
            (long *)&C->rows, (long *)&C->cols,
            A_tr ? (long *)&A->rows : (long *)&A->cols, &alpha,
            A->data, (A->rows == 0) ? &onei : (long *)&A->rows,
            B->data, (B->rows == 0) ? &onei : (long *)&B->rows, &beta,
            C->data, (C->rows == 0) ? &onei : (long *)&C->rows );
#else
   gan_sgemm ( A_tr, B_tr, C->rows, C->cols,
               A_tr ? A->rows : A->cols, alpha,
               A->data, (A->rows == 0) ? 1 : A->rows,
               B->data, (B->rows == 0) ? 1 : B->rows, beta,
               C->data, (C->rows == 0) ? 1 : C->rows );
#endif /* #ifdef HAVE_LAPACK */

   /* return result vector */
   return C;
}

/**
 * \}
 */

/**
 * \}
 */

/**
 * \}
 */

/**
 * File:          $RCSfile: matvec_blas.c,v $
 * Module:        C BLAS wrapper functions (double precision)
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

#include <gandalf/linalg/matvec_blas.h>
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
 * \defgroup GeneralSizeMatVecBLAS General Size Matrix/Vector BLAS Operations
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
Gan_Vector *
 gan_blas_axpy ( Gan_Vector *y, double a, const Gan_Vector *x )
{
#ifdef HAVE_LAPACK
   long onei = 1;
#endif

   /* check that the dimensions of vectors x and y are compatible */
   gan_err_test_ptr ( gan_vec_same_size(x,y), "gan_blas_axpy",
                      GAN_ERROR_INCOMPATIBLE, "" );

   /* can't do in-place axpy, use gan_blas_scal instead */
   gan_err_test_ptr ( y != x, "gan_blas_axpy", GAN_ERROR_FAILURE,
                      "can't do in-place axpy" );

   /* compute y += a*x */
#ifdef HAVE_LAPACK
   daxpy_ ( (long *)&x->rows, &a, x->data, &onei, y->data, &onei );
#else
   gan_daxpy ( x->rows, a, x->data, 1, y->data, 1 );
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
Gan_Vector *
 gan_blas_gemv ( Gan_Vector *y, double alpha,
                 const Gan_Matrix *A, Gan_TposeFlag A_tr,
                 const Gan_Vector *x, double beta )
{
#ifdef HAVE_LAPACK
   long onei = 1;
#endif

   /* check that the dimensions of vectors x and y are compatible */
   gan_err_test_ptr ( (A_tr ? A->rows : A->cols) == x->rows &&
                      (A_tr ? A->cols : A->rows) == y->rows, "gan_blas_gemv",
                      GAN_ERROR_INCOMPATIBLE, "" );

   /* can't gemv if x & y are the same vector */
   gan_err_test_ptr ( y != x, "gan_blas_gemv", GAN_ERROR_FAILURE,
                      "can't do gemv with x == y" );

   /* compute y = alpha*A*x + beta*y or y = alpha*A^T*x + beta*y */
#ifdef HAVE_LAPACK
   dgemv_ ( A_tr ? "t" : "n", (long *)&A->rows, (long *)&A->cols, &alpha,
            A->data, (A->rows == 0) ? &onei : (long *)&A->rows,
            x->data, &onei, &beta, y->data, &onei );
#else
   gan_dgemv ( A_tr, A->rows, A->cols, alpha, A->data,
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
Gan_Matrix *
 gan_blas_ger ( Gan_Matrix *A, const Gan_Vector *x, const Gan_Vector *y, double a )
{
#ifdef HAVE_LAPACK
   long onei = 1;
#endif

   /* consistency check */
   gan_err_test_ptr ( x->rows == A->rows && y->rows == A->cols,
                      "gan_blas_ger", GAN_ERROR_INCOMPATIBLE, "");

   /* compute A += a*x*y^T */
#ifdef HAVE_LAPACK
   dger_ ( (long *)&A->rows, (long *)&A->cols, &a, x->data, &onei,
           y->data, &onei, A->data, (long *)&A->rows );
#else
   gan_dger ( A->rows, A->cols, a, x->data, 1, y->data, 1,
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
Gan_SquMatrix *
 gan_blas_spr ( Gan_SquMatrix *A, const Gan_Vector *x, double a )
{
#ifdef HAVE_LAPACK
   long onei = 1;
#endif

   /* consistency check */
   gan_err_test_ptr ( A->type == GAN_SYMMETRIC_MATRIX && x->rows == A->size,
                      "gan_blas_spr", GAN_ERROR_INCOMPATIBLE, "");

   /* compute A += a*x*x^T */
#ifdef HAVE_LAPACK
   dspr_ ( "U", (long *)&A->size, &a, x->data, &onei, A->data );
#else
   gan_dspr ( GAN_MATRIXPART_UPPER, A->size, a, x->data, 1, A->data );
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
Gan_Vector *
 gan_blas_scal ( Gan_Vector *x, double a )
{
#ifdef HAVE_LAPACK
   long onei = 1;
#endif

   /* compute x *= a */
#ifdef HAVE_LAPACK
   dscal_ ( (long *)&x->rows, &a, x->data, &onei );
#else
   gan_dscal ( x->rows, a, x->data, 1 );
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
Gan_Matrix *
 gan_blas_gemm ( Gan_Matrix *C, double alpha,
                 const Gan_Matrix *A, Gan_TposeFlag A_tr,
                 const Gan_Matrix *B, Gan_TposeFlag B_tr, double beta )
{
#ifdef HAVE_LAPACK
   long onei = 1;
#endif

   /* check that the dimensions of matrices A,B,C are compatible */
   gan_err_test_ptr ( C->rows == A->rows && C->cols == B->cols &&
                      A->cols == B->rows, "gan_blas_gemm",
                      GAN_ERROR_INCOMPATIBLE, "matrices" );

   /* can't gemm if A & C or B & C are the same matrix */
   gan_err_test_ptr ( A != C && B != C, "gan_blas_gemm", GAN_ERROR_FAILURE,
                      "can't do gemm with A == C or B == C" );

   /* compute C = alpha*A(^T)*B(^T) + beta*C */
#ifdef HAVE_LAPACK
   dgemm_ ( A_tr ? "t" : "n", B_tr ? "t" : "n",
            (long *)&C->rows, (long *)&C->cols,
            A_tr ? (long *)&A->rows : (long *)&A->cols, &alpha,
            A->data, (A->rows == 0) ? &onei : (long *)&A->rows,
            B->data, (B->rows == 0) ? &onei : (long *)&B->rows, &beta,
            C->data, (C->rows == 0) ? &onei : (long *)&C->rows );
#else
   gan_dgemm ( A_tr, B_tr, C->rows, C->cols,
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

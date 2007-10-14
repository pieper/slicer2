/**
 * File:          $RCSfile: matvec_clapack.c,v $
 * Module:        CLAPACK wrapper functions (double precision)
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

#include <gandalf/linalg/matvec_clapack.h>
#include <gandalf/linalg/clapack.h>
#include <gandalf/linalg/mat_diagonal.h>
#include <gandalf/linalg/mat_triangular.h>
#include <gandalf/linalg/vec_gen.h>
#include <gandalf/common/compare.h>
#include <gandalf/common/allocate.h>
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
 * \defgroup GeneralSizeMatVecCLAPACK General Size Matrix/Vector CLAPACK Operations
 * \{
 */

/**
 * \brief Function to apply the Cholesky LAPACK operation \c pptrf.
 * \param A Pointer to a symmetric matrix.
 * \param error_code A pointer to an error code value or \c NULL
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Function to apply the Cholesky factorisation LAPACK operation \c pptrf,
 * computing \f$ L \f$ from symmetric matrix \a A defined by
 * \f[
 *    A = L L^{\top}
 * \f]
 * for lower triangular matrix \f$ L \f$. The result \f$ L \f$ is overwritten
 * on the input matrix \a A.
 *
 * If an error occurs, such as \a A not being positive definite, #GAN_FALSE is
 * returned and either the error is returned in \a error_code (if \a error_code
 * is not \c NULL) or the error handling module is invoked (if \a error_code is
 * \c NULL).
 */
Gan_Bool
 gan_clapack_pptrf ( Gan_SquMatrix *A, int *error_code )
{
   long info;

   /* consistency check */
   assert ( A->type == GAN_SYMMETRIC_MATRIX );

#ifdef HAVE_LAPACK
   /* compute Cholesky factorisation */
   dpptrf_ ( "U", (long *)&A->size, A->data, &info );

   if ( info != 0 )
   {
      if ( info < 0 )
      {
         if ( error_code == NULL )
         {
            gan_err_flush_trace();
            gan_err_register ( "gan_clapack_pptrf",
                               GAN_ERROR_CLAPACK_ILLEGAL_ARG, "to dpptrf_()" );
         }
         else
            *error_code = GAN_ERROR_CLAPACK_ILLEGAL_ARG;
      }
      else /* info > 0 */
      {
         if ( error_code == NULL )
         {
            gan_err_flush_trace();
            gan_err_register ( "gan_clapack_pptrf",
                               GAN_ERROR_NOT_POSITIVE_DEFINITE,"in dpptrf_()");
         }
         else
            *error_code = GAN_ERROR_NOT_POSITIVE_DEFINITE;
      }

      return GAN_FALSE;
   }
#else
   /* compute Cholesky factorisation */
   if ( !gan_dpptrf ( GAN_MATRIXPART_UPPER, A->size, A->data, &info ) )
   {
      if ( info < 0 )
      {
         if ( error_code == NULL )
         {
            gan_err_flush_trace();
            gan_err_register ( "gan_clapack_pptrf",
                               GAN_ERROR_CLAPACK_ILLEGAL_ARG, "to dpptrf_()" );
         }
         else
            *error_code = GAN_ERROR_CLAPACK_ILLEGAL_ARG;
      }
      else /* info > 0 */
      {
         if ( error_code == NULL )
         {
            gan_err_flush_trace();
            gan_err_register ( "gan_clapack_pptrf",
                               GAN_ERROR_NOT_POSITIVE_DEFINITE,"in dpptrf_()");
         }
         else
            *error_code = GAN_ERROR_NOT_POSITIVE_DEFINITE;
      }

      return GAN_FALSE;
   }
#endif /* #ifdef HAVE_LAPACK */   

   /* convert matrix to lower triangular */
   if ( gan_ltmat_set_size ( A, A->size ) == NULL )
   {
      if ( error_code == NULL )
      {
         gan_err_register ( "gan_clapack_pptrf", GAN_ERROR_FAILURE, "" );
      }
      else
         *error_code = GAN_ERROR_FAILURE;

      return GAN_FALSE;
   }

   /* return with success */
   return GAN_TRUE;
}

/**
 * \brief Computes a symmetric matrix from its Cholesky factorisation.
 * \param A Pointer to a lower triangular matrix.
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Function to compute a symmetric matrix from its Cholesky factorisation
 * using the LAPACK operation \c pptri. The result is overwritten on the input
 * matrix \a A.
 */
Gan_Bool
 gan_clapack_pptri ( Gan_SquMatrix *A )
{
   long info;

   /* consistency check */
   assert ( A->type == GAN_LOWER_TRI_MATRIX );

#ifdef HAVE_LAPACK
   /* compute inverse */
   dpptri_ ( "U", (long *)&A->size, A->data, &info );

   if ( info != 0 )
   {
      gan_err_flush_trace();
      if ( info < 0 )
      {
         gan_err_register ( "gan_clapack_pptri", GAN_ERROR_CLAPACK_ILLEGAL_ARG,
                            "in dpptri_()" );
      }
      else /* info > 0 */
         gan_err_register ( "gan_clapack_pptri", GAN_ERROR_DIVISION_BY_ZERO,
                            "in dpptri_()" );

      return GAN_FALSE;
   }
#else
   /* compute inverse */
   if ( !gan_dpptri ( GAN_MATRIXPART_UPPER, A->size, A->data, &info ) )
   {
      gan_err_flush_trace();
      if ( info < 0 )
      {
         gan_err_register ( "gan_clapack_pptri", GAN_ERROR_CLAPACK_ILLEGAL_ARG,
                            "in dpptri_()" );
      }
      else /* info > 0 */
         gan_err_register ( "gan_clapack_pptri", GAN_ERROR_DIVISION_BY_ZERO,
                            "in dpptri_()" );

      return GAN_FALSE;
   }
#endif /* #ifdef HAVE_LAPACK */   

   /* return with success */
   return GAN_TRUE;
}

/**
 * \brief Function to apply the LU decomposition operation \c getf2.
 * \param A Pointer to a general square matrix.
 * \param ipiv Array of integer pivot indices
 * \param error_code A pointer to an error code value or \c NULL
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Function to apply the LAPACK LU decomposition operation \c getf2.
 * The factors are overwritten in the matrix \a A.
 *
 * If an error occurs, such as \a A being singular, #GAN_FALSE is returned and
 * either the error is returned in \a error_code (if \a error_code is not
 * \c NULL) or the error handling module is invoked (if \a error_code
 * is \c NULL).
 */
Gan_Bool
 gan_clapack_getf2 ( Gan_Matrix *A, long *ipiv, int *error_code )
{
   long info;
#ifdef HAVE_LAPACK
   long onei = 1;
#endif

   /* consistency check */
   gan_err_test_bool ( A->rows == A->cols, "gan_clapack_getf2",
                       GAN_ERROR_ILLEGAL_ARGUMENT, "" );

#ifdef HAVE_LAPACK
   /* compute LU decomposition */
   dgetf2_ ( (long *)&A->rows, (long *)&A->cols,
             A->data, (A->rows == 0) ? &onei : (long *)&A->rows, ipiv, &info );

   if ( info != 0 )
   {
      if ( info < 0 )
      {
         if ( error_code == NULL )
         {
            gan_err_flush_trace();
            gan_err_register ( "gan_clapack_getf2",
                               GAN_ERROR_CLAPACK_ILLEGAL_ARG, "to dgetf2_()" );
         }
         else
            *error_code = GAN_ERROR_CLAPACK_ILLEGAL_ARG;
      }
      else /* info > 0 */
      {
         if ( error_code == NULL )
         {
            gan_err_flush_trace();
            gan_err_register ( "gan_clapack_getf2",
                               GAN_ERROR_SINGULAR_MATRIX, "in dgetf2_()");
         }
         else
            *error_code = GAN_ERROR_SINGULAR_MATRIX;
      }

      return GAN_FALSE;
   }
#else
   /* compute LU decomposition */
   if ( !gan_dgetf2 ( A->rows, A->cols,
                      A->data, (A->rows == 0) ? 1 : A->rows, ipiv, &info ) )
   {
      if ( info < 0 )
      {
         if ( error_code == NULL )
         {
            gan_err_flush_trace();
            gan_err_register ( "gan_clapack_getf2",
                               GAN_ERROR_CLAPACK_ILLEGAL_ARG, "to dgetf2_()" );
         }
         else
            *error_code = GAN_ERROR_CLAPACK_ILLEGAL_ARG;
      }
      else /* info > 0 */
      {
         if ( error_code == NULL )
         {
            gan_err_flush_trace();
            gan_err_register ( "gan_clapack_getf2",
                               GAN_ERROR_SINGULAR_MATRIX, "in dgetf2_()");
         }
         else
            *error_code = GAN_ERROR_SINGULAR_MATRIX;
      }

      return GAN_FALSE;
   }
#endif /* #ifdef HAVE_LAPACK */   

   /* return with success */
   return GAN_TRUE;
}

/**
 * \brief Function to apply the LU decomposition operation \c getrf.
 * \param A Pointer to a general square matrix.
 * \param ipiv Array of integer pivot indices
 * \param error_code A pointer to an error code value or \c NULL
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Function to apply the LAPACK LU decomposition operation \c getrf.
 * The factors are overwritten in the matrix \a A.
 *
 * If an error occurs, such as \a A being singular, #GAN_FALSE is returned and
 * either the error is returned in \a error_code (if \a error_code is
 * not \c NULL) or the error handling module is invoked (if \a error_code is
 * \c NULL).
 */
Gan_Bool
 gan_clapack_getrf ( Gan_Matrix *A, long *ipiv, int *error_code )
{
   long info;
#ifdef HAVE_LAPACK
   long onei = 1;
#endif

   /* consistency check */
   gan_err_test_bool ( A->rows == A->cols, "gan_clapack_getrf",
                       GAN_ERROR_ILLEGAL_ARGUMENT, "" );

#ifdef HAVE_LAPACK
   /* compute LU decomposition */
   dgetrf_ ( (long *)&A->rows, (long *)&A->cols,
             A->data, (A->rows == 0) ? &onei : (long *)&A->rows, ipiv, &info );

   if ( info != 0 )
   {
      if ( info < 0 )
      {
         if ( error_code == NULL )
         {
            gan_err_flush_trace();
            gan_err_register ( "gan_clapack_getrf",
                               GAN_ERROR_CLAPACK_ILLEGAL_ARG, "to dgetrf_()" );
         }
         else
            *error_code = GAN_ERROR_CLAPACK_ILLEGAL_ARG;
      }
      else /* info > 0 */
      {
         if ( error_code == NULL )
         {
            gan_err_flush_trace();
            gan_err_register ( "gan_clapack_getrf",
                               GAN_ERROR_SINGULAR_MATRIX, "in dgetrf_()");
         }
         else
            *error_code = GAN_ERROR_SINGULAR_MATRIX;
      }

      return GAN_FALSE;
   }
#else
   /* compute LU decomposition */
   if ( !gan_dgetrf ( A->rows, A->cols,
                      A->data, (A->rows == 0) ? 1 : A->rows, ipiv, &info ) )
   {
      if ( info < 0 )
      {
         if ( error_code == NULL )
         {
            gan_err_flush_trace();
            gan_err_register ( "gan_clapack_getrf",
                               GAN_ERROR_CLAPACK_ILLEGAL_ARG, "to dgetrf_()" );
         }
         else
            *error_code = GAN_ERROR_CLAPACK_ILLEGAL_ARG;
      }
      else /* info > 0 */
      {
         if ( error_code == NULL )
         {
            gan_err_flush_trace();
            gan_err_register ( "gan_clapack_getrf",
                               GAN_ERROR_SINGULAR_MATRIX, "in dgetrf_()");
         }
         else
            *error_code = GAN_ERROR_SINGULAR_MATRIX;
      }

      return GAN_FALSE;
   }
#endif /* #ifdef HAVE_LAPACK */   

   /* return with success */
   return GAN_TRUE;
}

/**
 * \brief Computes a symmetric matrix from its Cholesky factorisation.
 * \param A Pointer to a lower triangular matrix
 * \param ipiv Array of integer pivot indices
 * \param work Workspace array
 * \param work_size size of the work array
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Function to compute a symmetric matrix from its Cholesky factorisation using
 * the LAPACK operation \c getri. The result is overwritten on the input
 * matrix \a A.
 */
Gan_Bool
 gan_clapack_getri ( Gan_Matrix *A, long *ipiv,
                     double *work, unsigned long work_size )
{
   long info;
#ifdef HAVE_LAPACK
   long onei = 1;
#endif

   /* consistency check */
   gan_err_test_bool ( A->rows == A->cols, "gan_clapack_getri",
                       GAN_ERROR_ILLEGAL_ARGUMENT, "" );

#ifdef HAVE_LAPACK
   /* compute inverse */
   dgetri_ ( (long *)&A->rows, A->data,
             (A->rows == 0) ? &onei : (long *)&A->rows,
             ipiv, work, (long *)&work_size, &info );

   if ( info != 0 )
   {
      gan_err_flush_trace();
      if ( info < 0 )
      {
         gan_err_register ( "gan_clapack_getri", GAN_ERROR_CLAPACK_ILLEGAL_ARG,
                            "in dgetri_()" );
      }
      else /* info > 0 */
         gan_err_register ( "gan_clapack_getri", GAN_ERROR_SINGULAR_MATRIX,
                            "in dgetri_()" );

      return GAN_FALSE;
   }
#else
   /* compute inverse */
   if ( !gan_dgetri ( A->rows, A->data, (A->rows == 0) ? 1 : A->rows,
                      ipiv, work, work_size, &info))
   {
      gan_err_flush_trace();
      if ( info < 0 )
      {
         gan_err_register ( "gan_clapack_getri", GAN_ERROR_CLAPACK_ILLEGAL_ARG,
                            "in dgetri_()" );
      }
      else /* info > 0 */
         gan_err_register ( "gan_clapack_getri", GAN_ERROR_SINGULAR_MATRIX,
                            "in dgetri_()" );

      return GAN_FALSE;
   }
#endif /* #ifdef HAVE_LAPACK */   

   /* return with success */
   return GAN_TRUE;
}

/**
 * \brief Apply the QR decomposition LAPACK operation \c geqrf.
 * \param A Pointer to a rectangular matrix
 * \param tau A vector of scalar factors of elementary reflectors
 * \param work Workspace array
 * \param work_size size of the work array
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Applies the QR decomposition LAPACK operation \c geqrf:
 * \f[
 *   A = Q R
 * \f]
 * for matrix \a A, orthogonal matrix \f$ Q \f$ and upper triangular
 * matrix \f$ R \f$. The result is overwritten on the input  matrix \a A.
 * \a tau on output contains the scalar factors of elementary reflectors
 * (see the CLAPACK documentation). \a work is a workspace array, or \c NULL
 * if the workspace is to be allocated and freed internally.
 *
 * \sa gan_clapack_orgqr().
 */
Gan_Bool
 gan_clapack_geqrf ( Gan_Matrix *A, Gan_Vector *tau,
                     double *work, unsigned long work_size )
{
   long info;
#ifdef HAVE_LAPACK
   long onei = 1;
#endif

   if ( gan_vec_set_size ( tau, gan_min2 ( A->rows, A->cols ) ) == NULL )
   {
      gan_err_register ( "gan_clapack_geqrf", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

#ifdef HAVE_LAPACK
   /* compute QR decomposition */
   dgeqrf_ ( (long *)&A->rows, (long *)&A->cols, A->data,
             (A->rows == 0) ? &onei : (long *)&A->rows,
             tau->data, work, (long *)&work_size, &info );

   if ( info != 0 )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_clapack_geqrf", GAN_ERROR_CLAPACK_ILLEGAL_ARG,
                         "in dgeqrf_()" );
      return GAN_FALSE;
   }
#else
   /* compute QR decomposition */
   if ( !gan_dgeqrf ( A->rows, A->cols, A->data, (A->rows == 0) ? 1 : A->rows,
                      tau->data, work, work_size, &info ) )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_clapack_gelqf", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }
#endif /* #ifdef HAVE_LAPACK */   

   /* return with success */
   return GAN_TRUE;
}

/**
 * \brief Apply the LQ decomposition LAPACK operation \c gelqf.
 * \param A Pointer to a rectangular matrix
 * \param tau A vector of scalar factors of elementary reflectors
 * \param work Workspace array
 * \param work_size size of the work array
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Applies the LQ decomposition LAPACK operation \c gelqf:
 * \f[
 *   A = L Q
 * \f]
 * for matrix \a A, lower triangular matrix \f$ R \f$ and orthogonal
 * matrix \f$ Q \f$. The result is overwritten on the input matrix \a A.
 * \a tau on output contains the scalar factors of elementary reflectors
 * (see the CLAPACK documentation). \a work is a workspace array, or \c NULL
 * if the workspace is to be allocated and freed internally.
 * \sa gan_clapack_orglq().
 */
Gan_Bool
 gan_clapack_gelqf ( Gan_Matrix *A, Gan_Vector *tau,
                     double *work, unsigned long work_size )
{
   long info;
#ifdef HAVE_LAPACK
   long onei = 1;
#endif

   if ( gan_vec_set_size ( tau, gan_min2 ( A->rows, A->cols ) ) == NULL )
   {
      gan_err_register ( "gan_clapack_gelqf", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

#ifdef HAVE_LAPACK
   /* compute LQ decomposition */
   dgelqf_ ( (long *)&A->rows, (long *)&A->cols, A->data,
             (A->rows == 0) ? &onei : (long *)&A->rows,
             tau->data, work, (long *)&work_size, &info );

   if ( info != 0 )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_clapack_gelqf", GAN_ERROR_CLAPACK_ILLEGAL_ARG,
                         "in dgelqf_()" );
      return GAN_FALSE;
   }
#else
   /* compute LQ decomposition */
   if ( !gan_dgelqf ( A->rows, A->cols, A->data, (A->rows == 0) ? 1 : A->rows,
                      tau->data, work, work_size, &info ) )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_clapack_gelqf", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }
#endif /* #ifdef HAVE_LAPACK */   

   /* return with success */
   return GAN_TRUE;
}

/**
 * \brief Construct the orthogonal factor \f$ Q \f$  given a QR decomposition.
 * \param A Pointer to a QR matrix computed by gan_clapack_geqrf()
 * \param tau A vector of scalar factors of elementary reflectors
 * \param work Workspace array
 * \param work_size size of the work array
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Construct the orthogonal factor \f$ Q \f$  given a QR decomposition.
 * \sa gan_clapack_geqrf().
 */
Gan_Bool
 gan_clapack_orgqr ( Gan_Matrix *A, Gan_Vector *tau,
                     double *work, unsigned long work_size )
{
   long info;
   unsigned long Acols = A->cols;
#ifdef HAVE_LAPACK
   long onei = 1;
#endif

   /* reset column size of A, leaving right-hand part of A blank */
   if ( gan_mat_set_dims ( A, A->rows, A->rows ) == NULL )
   {
      gan_err_register ( "gan_clapack_orgqr", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

#ifdef HAVE_LAPACK
   /* construct Q matrix */
   dorgqr_ ( (long *)&A->rows, (long *)&A->rows, (long *)&Acols,
             A->data, (A->rows == 0) ? &onei : (long *)&A->rows, tau->data,
             work, (long *)&work_size, &info );

   if ( info != 0 )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_clapack_orgqr", GAN_ERROR_CLAPACK_ILLEGAL_ARG,
                         "in dorgqr_()" );
      return GAN_FALSE;
   }
#else
   /* construct Q matrix */
   if ( !gan_dorgqr ( A->rows, A->rows, Acols, A->data,
                      (A->rows == 0) ? 1 : A->rows, tau->data,
                      work, work_size, &info ) )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_clapack_orgqr", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }
#endif /* #ifdef HAVE_LAPACK */   

   /* return with success */
   return GAN_TRUE;
}

/**
 * \brief Construct the orthogonal factor \f$ Q \f$ given an LQ decomposition.
 * \param A Pointer to a LQ matrix computed by gan_clapack_gelqf()
 * \param tau A vector of scalar factors of elementary reflectors
 * \param work Workspace array
 * \param work_size size of the work array
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Construct the orthogonal factor Q given an LQ decomposition.
 * \sa gan_clapack_gelqf().
 */
Gan_Bool
 gan_clapack_orglq ( Gan_Matrix *A, Gan_Vector *tau,
                     double *work, unsigned long work_size )
{
   long info;
   unsigned long Arows = A->rows;
   Gan_Matrix *Ap;
#ifdef HAVE_LAPACK
   long onei = 1;
#endif

   /* make copy of A matrix padded with extra rows to make it square */
   Ap = gan_mat_alloc ( A->cols, A->cols );
   if ( Ap == NULL )
   {
      gan_err_register ( "gan_clapack_orglq", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   gan_mat_insert ( Ap, 0, 0, A, 0, 0, A->rows, A->cols );

   /* copy Ap back into A */
   A = gan_mat_copy_q ( Ap, A );
   if ( A == NULL )
   {
      gan_err_register ( "gan_clapack_orglq", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   gan_mat_free(Ap);

#ifdef HAVE_LAPACK
   /* construct Q matrix */
   dorglq_ ( (long *)&A->cols, (long *)&A->cols, (long *)&Arows, A->data,
             (A->cols == 0) ? &onei : (long *)&A->cols, tau->data,
             work, (long *)&work_size, &info );

   if ( info != 0 )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_clapack_orglq", GAN_ERROR_CLAPACK_ILLEGAL_ARG,
                         "in dorglq_()" );
      return GAN_FALSE;
   }
#else
   /* construct Q matrix */
   if ( !gan_dorglq ( A->cols, A->cols, Arows, A->data,
                      (A->cols == 0) ? 1 : A->cols, tau->data,
                      work, work_size, &info ) )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_clapack_orglq", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }
#endif /* #ifdef HAVE_LAPACK */   

   /* return with success */
   return GAN_TRUE;
}

/**
 * \brief Solve matrix equation
 * \param A Input matrix A
 * \param B Input matrix B, overwritten with output matrix X
 * \param ipiv Array of integer pivot indices
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * \brief Solve matrix equation A*X=B for square matrix A
 */
Gan_Bool
 gan_clapack_gesv ( Gan_Matrix *A, Gan_Matrix *B, long *ipiv )
{
   long info;

   gan_err_test_bool ( A->rows == A->cols && B->rows == A->rows,
                       "gan_clapack_gesv", GAN_ERROR_CLAPACK_ILLEGAL_ARG, "" );
#ifdef HAVE_LAPACK
   dgesv_ ( &A->rows, &B->cols, A->data, &A->rows, ipiv, B->data, &B->rows,
            &info );
   if ( info != 0 )
   {
      gan_err_flush_trace();
      if ( info < 0 )
      {
         gan_err_register ( "gan_clapack_gesv", GAN_ERROR_CLAPACK_ILLEGAL_ARG,
                            "" );
      }
      else /* info > 0 */
         gan_err_register ( "gan_clapack_gesv", GAN_ERROR_FAILURE, "" );

      return GAN_FALSE;
   }
#else
   /* solve equations */
   if ( !gan_dgesv ( A->rows, B->cols, A->data, A->rows, ipiv,
                     B->data, B->rows, &info ) )
   {
      gan_err_flush_trace();
      if ( info < 0 )
      {
         gan_err_register ( "gan_clapack_gesv", GAN_ERROR_CLAPACK_ILLEGAL_ARG,
                            "" );
      }
      else /* info > 0 */
         gan_err_register ( "gan_clapack_gesv", GAN_ERROR_DIVISION_BY_ZERO,
                            "" );

      return GAN_FALSE;
   }
#endif /* #ifdef HAVE_LAPACK */   

   /* return with success */
   return GAN_TRUE;
}

#ifdef HAVE_LAPACK
/* these functions are not locally defined in Gandalf if CLAPACK is not
 * available
 */

/**
 * \brief Computes eigenvalues and (optionally) eigenvectors of a matrix.
 * \param A Pointer to the input square matrix
 * \param WR Diagonal matrix of real parts of eigenvalues
 * \param WI Diagonal matrix of imaginary parts of eigenvalues
 * \param VL Matrix of left eigenvectors or \c NULL
 * \param VR Matrix of right eigenvectors or \c NULL
 * \param work Workspace array
 * \param work_size size of the work array
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Computes the eigenvalues and optionally the left and/or right eigenvectors
 * of square non-symmetric matrix \a A. If either of the eigenvector matrices
 * \a VL or \a VR are passed as \c NULL, the corresponding eigenvectors are not
 * computed. The routine invokes the CLAPACK function \c dgeev().
 *
 * \a work is a workspace array of size \a work_size, which should be at least
 * \f$ \mbox{max}(1,3 N) \f$ if \a VL and \a VR are \c NULL, at least
 * \f$ 4 N \f$ if either are not \c NULL, where \f$ N \f$ is the size of \a A.
 * If \a work is passed as \c NULL, the workspace is allocated and freed
 * internally.
 */
Gan_Bool
 gan_clapack_geev ( Gan_Matrix *A,
                    Gan_SquMatrix *WR, Gan_SquMatrix *WI,
                    Gan_Matrix *VL, Gan_Matrix *VR,
                    double *work, unsigned long work_size )
{
   Gan_Bool work_alloc = GAN_FALSE;
   long info;
#ifdef HAVE_LAPACK
   long onei = 1;
#endif

   /* make sure the matrix is square */
   gan_err_test_bool ( A->rows == A->cols, "gan_clapack_geev",
                       GAN_ERROR_MATRIX_NOT_SQUARE, "" );

   /* set sizes of output vectors/matrices */
   if ( !gan_diagmat_set_size ( WR, A->rows ) ||
        !gan_diagmat_set_size ( WI, A->rows ) ||
        !(VL == NULL || gan_mat_set_dims ( VL, A->rows, A->rows )) ||
        !(VR == NULL || gan_mat_set_dims ( VR, A->rows, A->rows )) )
   {
      gan_err_register ( "gan_clapack_geev", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   /* allocate workspace if necessary */
   if ( work == NULL )
   {
      work_size = 10*A->rows;
      work = gan_malloc_array ( double, work_size );
      if ( work == NULL )
      {
         gan_err_flush_trace();
         gan_err_register_with_number ( "gan_clapack_geev", GAN_ERROR_MALLOC_FAILED, "", work_size*sizeof(double) );
         return GAN_FALSE;
      }

      work_alloc = GAN_TRUE;
   }

#ifdef HAVE_LAPACK
   /* call dgeev to get eigenvalues/eigenvectors */
   dgeev_ ( (VL == NULL) ? "N" : "V", (VR == NULL) ? "N" : "V",
            (long *)&A->rows, A->data,
            (A->rows == 0) ? &onei : (long *)&A->rows, WR->data, WI->data,
            (VL == NULL) ? NULL : VL->data,
            (VL == NULL || A->rows == 0) ? &onei : (long *)&A->rows,
            (VR == NULL) ? NULL : VR->data,
            (VR == NULL || A->rows == 0) ? &onei : (long *)&A->rows,
            work, (long *)&work_size, &info );
            
   if ( info != 0 )
   {
      gan_err_flush_trace();
      if ( info < 0 )
      {
         gan_err_register ( "gan_clapack_geev", GAN_ERROR_CLAPACK_ILLEGAL_ARG,
                            "in dgeev_()" );
      }
      else /* info > 0 */
         gan_err_register ( "gan_clapack_geev", GAN_ERROR_NO_CONVERGENCE,
                            "in dgeev_()" );

      return GAN_FALSE;
   }
#else
   gan_assert ( 0, "not implemented" );
#endif /* #ifdef HAVE_LAPACK */   

   /* free workspace if it was allocated */
   if ( work_alloc ) free ( work );

   /* success */
   return GAN_TRUE;
}

/**
 * \brief Computes Schur factorisation of a matrix
 * \param A Pointer to the input square matrix, overwritten with the Schur
 *        quasi-triangular factor on output
 * \param Z orthogonal factor or \c NULL
 * \param work Workspace array
 * \param work_size size of the work array
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Computes the Schur factorisation A = Z*T*Z^T of matrix A.
 *
 * \a work is a workspace array of size \a work_size, which should be at least
 * \f$ \mbox{max}(1,3 N) \f$, where \f$ N \f$ is the size of \a A.
 * If \a work is passed as \c NULL, the workspace is allocated and freed
 * internally.
 */
Gan_Bool
 gan_clapack_gees ( Gan_Matrix *A, Gan_Matrix *Z,
                    double *work, unsigned long work_size )
{
   Gan_Bool work_alloc = GAN_FALSE;
   long info;
#ifdef HAVE_LAPACK
   long zeroi = 0;
#endif
   double *WR, *WI;

   /* make sure the matrix is square */
   gan_err_test_bool ( A->rows == A->cols, "gan_clapack_gees",
                       GAN_ERROR_MATRIX_NOT_SQUARE, "" );

   /* set sizes of output vectors/matrices */
   if ( Z != NULL && gan_mat_set_dims ( Z, A->rows, A->rows ) == NULL )
   {
      gan_err_register ( "gan_clapack_gees", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   /* allocate workspace if necessary */
   if ( work == NULL )
   {
      work_size = 10*A->rows;
      work = gan_malloc_array ( double, work_size );
      if ( work == NULL )
      {
         gan_err_flush_trace();
         gan_err_register_with_number ( "gan_clapack_gees", GAN_ERROR_MALLOC_FAILED, "", work_size*sizeof(double) );
         return GAN_FALSE;
      }

      work_alloc = GAN_TRUE;
   }

   WR = gan_malloc_array(double,A->rows);
   WI = gan_malloc_array(double,A->rows);
   if ( WR == NULL || WI == NULL )
   {
      gan_err_flush_trace();
      gan_err_register_with_number ( "gan_clapack_gees", GAN_ERROR_MALLOC_FAILED, "", A->rows*sizeof(double) );
      return GAN_FALSE;
   }

#ifdef HAVE_LAPACK
   dgees_ ( (Z==NULL) ? "N" : "V", "N", NULL, &A->rows, A->data, &A->rows,
            &zeroi, WR, WI,
            (Z==NULL) ? NULL : Z->data, (Z==NULL) ? NULL : &Z->rows,
            work, &work_size, NULL, &info );
   if ( info != 0 )
   {
      gan_err_flush_trace();
      if ( info < 0 )
      {
         gan_err_register ( "gan_clapack_gees", GAN_ERROR_CLAPACK_ILLEGAL_ARG,
                            "in dgees_()" );
      }
      else /* info > 0 */
         gan_err_register ( "gan_clapack_gees", GAN_ERROR_NO_CONVERGENCE,
                            "in dgees_()" );

      return GAN_FALSE;
   }
#else
   gan_assert ( 0, "not implemented" );
#endif /* #ifdef HAVE_LAPACK */   

   gan_free_va ( WI, WR, NULL );

   /* free workspace if it was allocated */
   if ( work_alloc ) free ( work );

   /* success */
   return GAN_TRUE;
}

#endif /* #ifdef HAVE_LAPACK */

/**
 * \}
 */

/**
 * \}
 */

/**
 * \}
 */

/**
 * File:          $RCSfile: matvecf_clapack.c,v $
 * Module:        CLAPACK wrapper functions (single precision)
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

#include <gandalf/linalg/matvecf_clapack.h>
#include <gandalf/linalg/clapack.h>
#include <gandalf/linalg/matf_diagonal.h>
#include <gandalf/linalg/matf_triangular.h>
#include <gandalf/linalg/vecf_gen.h>
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
 * \addtogroup GeneralSizeMatVecCLAPACK
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
 gan_clapackf_pptrf ( Gan_SquMatrix_f *A, int *error_code )
{
   long info;

   /* consistency check */
   assert ( A->type == GAN_SYMMETRIC_MATRIX );

#ifdef HAVE_LAPACK
   /* compute Cholesky factorisation */
   spptrf_ ( "U", (long *)&A->size, A->data, &info );

   if ( info != 0 )
   {
      if ( info < 0 )
      {
         if ( error_code == NULL )
         {
            gan_err_flush_trace();
            gan_err_register ( "gan_clapackf_pptrf",
                               GAN_ERROR_CLAPACK_ILLEGAL_ARG, "to spptrf_()" );
         }
         else
            *error_code = GAN_ERROR_CLAPACK_ILLEGAL_ARG;
      }
      else /* info > 0 */
      {
         if ( error_code == NULL )
         {
            gan_err_flush_trace();
            gan_err_register ( "gan_clapackf_pptrf",
                               GAN_ERROR_NOT_POSITIVE_DEFINITE,"in spptrf_()");
         }
         else
            *error_code = GAN_ERROR_NOT_POSITIVE_DEFINITE;
      }

      return GAN_FALSE;
   }
#else
   /* compute Cholesky factorisation */
   if ( !gan_spptrf ( GAN_MATRIXPART_UPPER, A->size, A->data, &info ) )
   {
      if ( info < 0 )
      {
         if ( error_code == NULL )
         {
            gan_err_flush_trace();
            gan_err_register ( "gan_clapackf_pptrf",
                               GAN_ERROR_CLAPACK_ILLEGAL_ARG, "to spptrf_()" );
         }
         else
            *error_code = GAN_ERROR_CLAPACK_ILLEGAL_ARG;
      }
      else /* info > 0 */
      {
         if ( error_code == NULL )
         {
            gan_err_flush_trace();
            gan_err_register ( "gan_clapackf_pptrf",
                               GAN_ERROR_NOT_POSITIVE_DEFINITE,"in spptrf_()");
         }
         else
            *error_code = GAN_ERROR_NOT_POSITIVE_DEFINITE;
      }

      return GAN_FALSE;
   }
#endif /* #ifdef HAVE_LAPACK */   

   /* convert matrix to lower triangular */
   if ( gan_ltmatf_set_size ( A, A->size ) == NULL )
   {
      if ( error_code == NULL )
      {
         gan_err_register ( "gan_clapackf_pptrf", GAN_ERROR_FAILURE, "" );
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
 gan_clapackf_pptri ( Gan_SquMatrix_f *A )
{
   long info;

   /* consistency check */
   assert ( A->type == GAN_LOWER_TRI_MATRIX );

#ifdef HAVE_LAPACK
   /* compute inverse */
   spptri_ ( "U", (long *)&A->size, A->data, &info );

   if ( info != 0 )
   {
      gan_err_flush_trace();
      if ( info < 0 )
      {
         gan_err_register ( "gan_clapackf_pptri",
                            GAN_ERROR_CLAPACK_ILLEGAL_ARG,
                            "in spptri_()" );
      }
      else /* info > 0 */
         gan_err_register ( "gan_clapackf_pptri", GAN_ERROR_DIVISION_BY_ZERO,
                            "in spptri_()" );

      return GAN_FALSE;
   }
#else
   /* compute inverse */
   if ( !gan_spptri ( GAN_MATRIXPART_UPPER, A->size, A->data, &info ) )
   {
      gan_err_flush_trace();
      if ( info < 0 )
      {
         gan_err_register ( "gan_clapackf_pptri",
                            GAN_ERROR_CLAPACK_ILLEGAL_ARG,
                            "in spptri_()" );
      }
      else /* info > 0 */
         gan_err_register ( "gan_clapackf_pptri", GAN_ERROR_DIVISION_BY_ZERO,
                            "in spptri_()" );

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
 gan_clapackf_getf2 ( Gan_Matrix_f *A, long *ipiv, int *error_code )
{
   long info;
#ifdef HAVE_LAPACK
   long onei = 1;
#endif

   /* consistency check */
   gan_err_test_bool ( A->rows == A->cols, "gan_clapackf_getf2",
                       GAN_ERROR_ILLEGAL_ARGUMENT, "" );

#ifdef HAVE_LAPACK
   /* compute LU decomposition */
   sgetf2_ ( (long *)&A->rows, (long *)&A->cols,
             A->data, (A->rows == 0) ? &onei : (long *)&A->rows, ipiv, &info );

   if ( info != 0 )
   {
      if ( info < 0 )
      {
         if ( error_code == NULL )
         {
            gan_err_flush_trace();
            gan_err_register ( "gan_clapackf_getf2",
                               GAN_ERROR_CLAPACK_ILLEGAL_ARG, "to sgetf2_()" );
         }
         else
            *error_code = GAN_ERROR_CLAPACK_ILLEGAL_ARG;
      }
      else /* info > 0 */
      {
         if ( error_code == NULL )
         {
            gan_err_flush_trace();
            gan_err_register ( "gan_clapackf_getf2",
                               GAN_ERROR_SINGULAR_MATRIX, "in sgetf2_()");
         }
         else
            *error_code = GAN_ERROR_SINGULAR_MATRIX;
      }

      return GAN_FALSE;
   }
#else
   /* compute LU decomposition */
   if ( !gan_sgetf2 ( A->rows, A->cols,
                      A->data, (A->rows == 0) ? 1 : A->rows, ipiv, &info ) )
   {
      if ( info < 0 )
      {
         if ( error_code == NULL )
         {
            gan_err_flush_trace();
            gan_err_register ( "gan_clapackf_getf2",
                               GAN_ERROR_CLAPACK_ILLEGAL_ARG, "to sgetf2_()" );
         }
         else
            *error_code = GAN_ERROR_CLAPACK_ILLEGAL_ARG;
      }
      else /* info > 0 */
      {
         if ( error_code == NULL )
         {
            gan_err_flush_trace();
            gan_err_register ( "gan_clapackf_getf2",
                               GAN_ERROR_SINGULAR_MATRIX, "in sgetf2_()");
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
 gan_clapackf_getrf ( Gan_Matrix_f *A, long *ipiv, int *error_code )
{
   long info;
#ifdef HAVE_LAPACK
   long onei = 1;
#endif

   /* consistency check */
   gan_err_test_bool ( A->rows == A->cols, "gan_clapackf_getrf",
                       GAN_ERROR_ILLEGAL_ARGUMENT, "" );

#ifdef HAVE_LAPACK
   /* compute LU decomposition */
   sgetrf_ ( (long *)&A->rows, (long *)&A->cols,
             A->data, (A->rows == 0) ? &onei : (long *)&A->rows, ipiv, &info );

   if ( info != 0 )
   {
      if ( info < 0 )
      {
         if ( error_code == NULL )
         {
            gan_err_flush_trace();
            gan_err_register ( "gan_clapackf_getrf",
                               GAN_ERROR_CLAPACK_ILLEGAL_ARG, "to sgetrf_()" );
         }
         else
            *error_code = GAN_ERROR_CLAPACK_ILLEGAL_ARG;
      }
      else /* info > 0 */
      {
         if ( error_code == NULL )
         {
            gan_err_flush_trace();
            gan_err_register ( "gan_clapackf_getrf",
                               GAN_ERROR_SINGULAR_MATRIX, "in sgetrf_()");
         }
         else
            *error_code = GAN_ERROR_SINGULAR_MATRIX;
      }

      return GAN_FALSE;
   }
#else
   /* compute LU decomposition */
   if ( !gan_sgetrf ( A->rows, A->cols,
                      A->data, (A->rows == 0) ? 1 : A->rows, ipiv, &info ) )
   {
      if ( info < 0 )
      {
         if ( error_code == NULL )
         {
            gan_err_flush_trace();
            gan_err_register ( "gan_clapackf_getrf",
                               GAN_ERROR_CLAPACK_ILLEGAL_ARG, "to sgetrf_()" );
         }
         else
            *error_code = GAN_ERROR_CLAPACK_ILLEGAL_ARG;
      }
      else /* info > 0 */
      {
         if ( error_code == NULL )
         {
            gan_err_flush_trace();
            gan_err_register ( "gan_clapackf_getrf",
                               GAN_ERROR_SINGULAR_MATRIX, "in sgetrf_()");
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
 gan_clapackf_getri ( Gan_Matrix_f *A, long *ipiv,
                      float *work, unsigned long work_size )
{
   long info;
#ifdef HAVE_LAPACK
   long onei = 1;
#endif

   /* consistency check */
   gan_err_test_bool ( A->rows == A->cols, "gan_clapackf_getri",
                       GAN_ERROR_ILLEGAL_ARGUMENT, "" );

#ifdef HAVE_LAPACK
   /* compute inverse */
   sgetri_ ( (long *)&A->rows, A->data,
             (A->rows == 0) ? &onei : (long *)&A->rows,
             ipiv, work, (long *)&work_size, &info );

   if ( info != 0 )
   {
      gan_err_flush_trace();
      if ( info < 0 )
      {
         gan_err_register ( "gan_clapackf_getri",
                            GAN_ERROR_CLAPACK_ILLEGAL_ARG,
                            "in sgetri_()" );
      }
      else /* info > 0 */
         gan_err_register ( "gan_clapackf_getri", GAN_ERROR_SINGULAR_MATRIX,
                            "in sgetri_()" );

      return GAN_FALSE;
   }
#else
   /* compute inverse */
   if ( !gan_sgetri ( A->rows, A->data, (A->rows == 0) ? 1 : A->rows,
                      ipiv, work, work_size, &info))
   {
      gan_err_flush_trace();
      if ( info < 0 )
      {
         gan_err_register ( "gan_clapackf_getri",
                            GAN_ERROR_CLAPACK_ILLEGAL_ARG,
                            "in sgetri_()" );
      }
      else /* info > 0 */
         gan_err_register ( "gan_clapackf_getri", GAN_ERROR_SINGULAR_MATRIX,
                            "in sgetri_()" );

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
 gan_clapackf_geqrf ( Gan_Matrix_f *A, Gan_Vector_f *tau,
                      float *work, unsigned long work_size )
{
   long info;
#ifdef HAVE_LAPACK
   long onei = 1;
#endif

   if ( gan_vecf_set_size ( tau, gan_min2 ( A->rows, A->cols ) ) == NULL )
   {
      gan_err_register ( "gan_clapackf_geqrf", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

#ifdef HAVE_LAPACK
   /* compute QR decomposition */
   sgeqrf_ ( (long *)&A->rows, (long *)&A->cols, A->data,
             (A->rows == 0) ? &onei : (long *)&A->rows,
             tau->data, work, (long *)&work_size, &info );

   if ( info != 0 )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_clapackf_geqrf", GAN_ERROR_CLAPACK_ILLEGAL_ARG,
                         "in sgeqrf_()" );
      return GAN_FALSE;
   }
#else
   /* compute QR decomposition */
   if ( !gan_sgeqrf ( A->rows, A->cols, A->data, (A->rows == 0) ? 1 : A->rows,
                      tau->data, work, work_size, &info ) )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_clapackf_gelqf", GAN_ERROR_FAILURE, "" );
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
 gan_clapackf_gelqf ( Gan_Matrix_f *A, Gan_Vector_f *tau,
                      float *work, unsigned long work_size )
{
   long info;
#ifdef HAVE_LAPACK
   long onei = 1;
#endif

   if ( gan_vecf_set_size ( tau, gan_min2 ( A->rows, A->cols ) ) == NULL )
   {
      gan_err_register ( "gan_clapackf_gelqf", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

#ifdef HAVE_LAPACK
   /* compute LQ decomposition */
   sgelqf_ ( (long *)&A->rows, (long *)&A->cols, A->data,
             (A->rows == 0) ? &onei : (long *)&A->rows,
             tau->data, work, (long *)&work_size, &info );

   if ( info != 0 )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_clapackf_gelqf", GAN_ERROR_CLAPACK_ILLEGAL_ARG,
                         "in sgelqf_()" );
      return GAN_FALSE;
   }
#else
   /* compute LQ decomposition */
   if ( !gan_sgelqf ( A->rows, A->cols, A->data, (A->rows == 0) ? 1 : A->rows,
                      tau->data, work, work_size, &info ) )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_clapackf_gelqf", GAN_ERROR_FAILURE, "" );
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
 gan_clapackf_orgqr ( Gan_Matrix_f *A, Gan_Vector_f *tau,
                      float *work, unsigned long work_size )
{
   long info;
   unsigned long Acols = A->cols;
#ifdef HAVE_LAPACK
   long onei = 1;
#endif

   /* reset column size of A, leaving right-hand part of A blank */
   if ( gan_matf_set_dims ( A, A->rows, A->rows ) == NULL )
   {
      gan_err_register ( "gan_clapackf_orgqr", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

#ifdef HAVE_LAPACK
   /* construct Q matrix */
   sorgqr_ ( (long *)&A->rows, (long *)&A->rows, (long *)&Acols,
             A->data, (A->rows == 0) ? &onei : (long *)&A->rows, tau->data,
             work, (long *)&work_size, &info );

   if ( info != 0 )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_clapackf_orgqr", GAN_ERROR_CLAPACK_ILLEGAL_ARG,
                         "in sorgqr_()" );
      return GAN_FALSE;
   }
#else
   /* construct Q matrix */
   if ( !gan_sorgqr ( A->rows, A->rows, Acols, A->data,
                      (A->rows == 0) ? 1 : A->rows, tau->data,
                      work, work_size, &info ) )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_clapackf_orgqr", GAN_ERROR_FAILURE, "" );
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
 gan_clapackf_orglq ( Gan_Matrix_f *A, Gan_Vector_f *tau,
                      float *work, unsigned long work_size )
{
   long info;
   unsigned long Arows = A->rows;
   Gan_Matrix_f *Ap;
#ifdef HAVE_LAPACK
   long onei = 1;
#endif

   /* make copy of A matrix padded with extra rows to make it square */
   Ap = gan_matf_alloc ( A->cols, A->cols );
   if ( Ap == NULL )
   {
      gan_err_register ( "gan_clapackf_orglq", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   gan_matf_insert ( Ap, 0, 0, A, 0, 0, A->rows, A->cols );

   /* copy Ap back into A */
   A = gan_matf_copy_q ( Ap, A );
   if ( A == NULL )
   {
      gan_err_register ( "gan_clapackf_orglq", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   gan_matf_free(Ap);

#ifdef HAVE_LAPACK
   /* construct Q matrix */
   sorglq_ ( (long *)&A->cols, (long *)&A->cols, (long *)&Arows, A->data,
             (A->cols == 0) ? &onei : (long *)&A->cols, tau->data,
             work, (long *)&work_size, &info );

   if ( info != 0 )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_clapackf_orglq", GAN_ERROR_CLAPACK_ILLEGAL_ARG,
                         "in sorglq_()" );
      return GAN_FALSE;
   }
#else
   /* construct Q matrix */
   if ( !gan_sorglq ( A->cols, A->cols, Arows, A->data,
                      (A->cols == 0) ? 1 : A->cols, tau->data,
                      work, work_size, &info ) )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_clapackf_orglq", GAN_ERROR_FAILURE, "" );
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
 gan_clapackf_gesv ( Gan_Matrix_f *A, Gan_Matrix_f *B, long *ipiv )
{
   long info;

   gan_err_test_bool ( A->rows == A->cols && B->rows == A->rows,
                       "gan_clapackf_gesv", GAN_ERROR_CLAPACK_ILLEGAL_ARG,
                       "" );
#ifdef HAVE_LAPACK
   sgesv_ ( &A->rows, &B->cols, A->data, &A->rows, ipiv, B->data, &B->rows,
            &info );
   if ( info != 0 )
   {
      gan_err_flush_trace();
      if ( info < 0 )
      {
         gan_err_register ( "gan_clapackf_gesv", GAN_ERROR_CLAPACK_ILLEGAL_ARG,
                            "" );
      }
      else /* info > 0 */
         gan_err_register ( "gan_clapackf_gesv", GAN_ERROR_FAILURE, "" );

      return GAN_FALSE;
   }
#else
   /* solve equations */
   if ( !gan_sgesv ( A->rows, B->cols, A->data, A->rows, ipiv,
                     B->data, B->rows, &info ) )
   {
      gan_err_flush_trace();
      if ( info < 0 )
      {
         gan_err_register ( "gan_clapackf_gesv", GAN_ERROR_CLAPACK_ILLEGAL_ARG,
                            "" );
      }
      else /* info > 0 */
         gan_err_register ( "gan_clapackf_gesv", GAN_ERROR_DIVISION_BY_ZERO,
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
 * computed. The routine invokes the CLAPACK function \c sgeev().
 *
 * \a work is a workspace array of size \a work_size, which should be at least
 * \f$ \mbox{max}(1,3 N) \f$ if \a VL and \a VR are \c NULL, at least
 * \f$ 4 N \f$ if either are not \c NULL, where \f$ N \f$ is the size of \a A.
 * If \a work is passed as \c NULL, the workspace is allocated and freed
 * internally.
 */
Gan_Bool
 gan_clapackf_geev ( Gan_Matrix_f *A,
                     Gan_SquMatrix_f *WR, Gan_SquMatrix_f *WI,
                     Gan_Matrix_f *VL, Gan_Matrix_f *VR,
                     float *work, unsigned long work_size )
{
   Gan_Bool work_alloc = GAN_FALSE;
   long info;
#ifdef HAVE_LAPACK
   long onei = 1;
#endif

   /* make sure the matrix is square */
   gan_err_test_bool ( A->rows == A->cols, "gan_clapackf_geev",
                       GAN_ERROR_MATRIX_NOT_SQUARE, "" );

   /* set sizes of output vectors/matrices */
   if ( !gan_diagmatf_set_size ( WR, A->rows ) ||
        !gan_diagmatf_set_size ( WI, A->rows ) ||
        !(VL == NULL || gan_matf_set_dims ( VL, A->rows, A->rows )) ||
        !(VR == NULL || gan_matf_set_dims ( VR, A->rows, A->rows )) )
   {
      gan_err_register ( "gan_clapackf_geev", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   /* allocate workspace if necessary */
   if ( work == NULL )
   {
      work_size = 10*A->rows;
      work = gan_malloc_array ( float, work_size );
      work_alloc = GAN_TRUE;
   }

#ifdef HAVE_LAPACK
   /* call sgeev to get eigenvalues/eigenvectors */
   sgeev_ ( (VL == NULL) ? "N" : "V", (VR == NULL) ? "N" : "V",
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
         gan_err_register ( "gan_clapackf_geev", GAN_ERROR_CLAPACK_ILLEGAL_ARG,
                            "in sgeev_()" );
      }
      else /* info > 0 */
         gan_err_register ( "gan_clapackf_geev", GAN_ERROR_NO_CONVERGENCE,
                            "in sgeev_()" );

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

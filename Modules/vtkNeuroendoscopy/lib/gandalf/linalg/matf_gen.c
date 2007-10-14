/**
 * File:          $RCSfile: matf_gen.c,v $
 * Module:        Generic rectangular matrices (single precision)
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

#include <stdarg.h>
#include <gandalf/linalg/matf_gen.h>
#include <gandalf/linalg/vecf_gen.h>
#include <gandalf/linalg/matf_symmetric.h>
#include <gandalf/linalg/matvecf_blas.h>
#include <gandalf/linalg/matvecf_clapack.h>
#include <gandalf/linalg/clapack.h>
#include <gandalf/linalg/cblas.h>
#include <gandalf/common/allocate.h>
#include <gandalf/common/misc_error.h>
#include <gandalf/common/numerics.h>
#include <gandalf/common/array.h>
#include <gandalf/common/endian_io.h>

/**
 * \addtogroup LinearAlgebra
 * \{
 */

/**
 * \addtogroup GeneralSizeMatVec
 * \{
 */

/**
 * \addtogroup GeneralSizeMatrix
 * \{
 */

/**
 * \addtogroup GeneralSizeMatrixAllocate
 * \{
 */

/* function to re-allocate matrix data */
Gan_Bool
 gan_matf_realloc_gen ( Gan_Matrix_f *A,
                        unsigned long rows, unsigned long cols )
{
   /* re-allocate matrix data */
   gan_err_test_bool ( A->data_alloc, "gan_matf_realloc_gen",
                       GAN_ERROR_CANT_REALLOC, "" );

   A->data_size = rows*cols;
   A->data = gan_realloc_array ( float, A->data, A->data_size );
   if ( A->data == NULL )
   {
      gan_err_flush_trace();
      gan_err_register_with_number ( "gan_matf_realloc_gen", GAN_ERROR_MALLOC_FAILED, "", A->data_size*sizeof(float) );
      return GAN_FALSE;
   }
     
   /* set matrix dimensions */
   A->rows = rows;
   A->cols = cols;

   /* return with success */
   return GAN_TRUE;
}

/**
 * \brief Free a matrix.
 *
 * Free a generic rectangular matrix \a A.
 */
void
 gan_matf_free ( Gan_Matrix_f *A )
{
   if ( A->data_alloc && A->data != NULL ) free(A->data);
   if ( A->struct_alloc ) free(A);
}

/**
 * \}
 */

/**
 * \addtogroup GeneralSizeMatrixSet
 * \{
 */

/**
 * \brief Set dimensions of generic matrix.
 *
 * Set dimensions of generic matrix \a A to \a rows by \a cols.
 */
Gan_Matrix_f *
 gan_matf_set_dims ( Gan_Matrix_f *A, unsigned long rows, unsigned long cols )
{
   /* check whether matrix A is big enough; if it isn't, reallocate it */
   if ( A->data_size < rows*cols )
   {
      /* re-allocate matrix data */
      if ( !gan_matf_realloc ( A, rows, cols ) )
      {
         gan_err_register ( "gan_matf_set_dims", GAN_ERROR_FAILURE, "" );
         return NULL;
      }
   }
   else
   {
      /* set matrix dimensions */
      A->rows = rows;
      A->cols = cols;
   }

   /* return modified matrix */
   return A;
}

/**
 * \}
 */

/**
 * \addtogroup GeneralSizeMatrixTest
 * \{
 */

/**
 * \brief Checks whether a matrix is zero
 * \return Returns #GAN_TRUE if all the elements of \a A are zero, #GAN_FALSE otherwise
 */
Gan_Bool gan_matf_is_zero ( Gan_Matrix_f *A )
{
   int i;

   for ( i = (int)(A->rows*A->cols)-1; i >= 0; i-- )
      if(A->data[i] != 0.0F)
         return GAN_FALSE;

   /* all zero */
   return GAN_TRUE;
}

/**
 * \}
 */

/**
 * \addtogroup GeneralSizeMatrixNorms
 * \{
 */

/**
 * \brief Returns squared Frobenius norm of generic rectangular matrix.
 * \param A The input matrix
 * \return The value of the squared Frobenius norm of \a A.
 *
 * Returns squared Frobenius norm of generic rectangular matrix \a A
 * (sum of squares of matrix elements).
 */
float
 gan_matf_sumsqr ( const Gan_Matrix_f *A )
{
   long i;
   float total = 0.0F;

   for ( i = (long)(A->rows*A->cols)-1; i >= 0; i-- )
      total += gan_sqr(A->data[i]);

   return total;
}

/**
 * \brief Returns Frobenius norm of generic rectangular matrix.
 * \param A The input matrix
 * \return The value of the Frobenius norm of \a A.
 *
 * Returns Frobenius norm of generic rectangular matrix
 * (square-root of sum of squares of matrix elements).
 */
float
 gan_matf_Fnorm ( const Gan_Matrix_f *A )
{
   long i;
   float total = 0.0F;

   for ( i = (long)(A->rows*A->cols)-1; i >= 0; i-- )
      total += gan_sqr(A->data[i]);

   return (float)sqrt(total);
}

/**
 * \}
 */

/**
 * \addtogroup GeneralSizeMatrixInvert
 * \{
 */

/**
 * \brief Inverts general square matrix.
 * \param A Input Matrix
 * \param B Output inverse matrix
 * \return Non-\c NULL Pointer to the output inverse matrix, or \c NULL
 *         on failure.
 * Inverts general square matrix \a A into matrix \a B. This is a nasty
 * operation and should be avoided at all costs.
 * \sa gan_matf_invert_s().
 */
Gan_Matrix_f *
 gan_matf_invert_q ( const Gan_Matrix_f *A, Gan_Matrix_f *B )
{
   long *ipiv;
   float *work;

   gan_err_test_ptr ( A->rows == A->cols, "gan_matf_invert_q",
                      GAN_ERROR_ILLEGAL_ARGUMENT,
                      "cannot invert non-square matrix" );

   /* allocate/set output matrix */
   if ( B == NULL )
      B = gan_matf_alloc ( A->rows, A->cols );
   else
      B = gan_matf_set_dims ( B, A->rows, A->cols );

   if ( B == NULL )
   {
      gan_err_register ( "gan_matf_invert_q", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* handle zero size matrix */
   if ( A->rows == 0 ) return B;

   /* copy matrix A into B */
   if ( gan_matf_copy_q ( A, B ) == NULL )
   {
      gan_err_register ( "gan_matf_invert_q", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* allocate pivot indices and workspace */
   ipiv = gan_malloc_array ( long, A->rows );
   work = gan_malloc_array ( float, A->rows );
   if ( ipiv == NULL || work == NULL )
   {
      gan_err_flush_trace();
      gan_err_register_with_number ( "gan_matf_invert_q", GAN_ERROR_MALLOC_FAILED, "", A->rows*sizeof(float) );
      return NULL;
   }

   /* invert using LU decomposition */
   if ( !gan_clapackf_getrf ( B, ipiv, NULL ) )
   {
      gan_free_va ( work, ipiv, NULL );
      gan_err_register ( "gan_matf_invert_q", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   if ( !gan_clapackf_getri ( B, ipiv, work, A->rows ) )
   {
      gan_free_va ( work, ipiv, NULL );
      gan_err_register ( "gan_matf_invert_q", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   gan_free_va ( work, ipiv, NULL );

   /* success */
   return B;
}

/**
 * \}
 */

/**
 * \addtogroup GeneralSizeMatrixFill
 * \{
 */

/**
 * \brief Fill matrix from variable argument list.
 * \param A Matrix to fill or \c NULL
 * \param rows Number of rows in matrix
 * \param cols Number of columns in matrix
 *
 * Fill matrix \a A with values from variable argument list, and
 * set the dimensions of \a A to \a rows by \a cols.
 *
 * \return Pointer to the filled matrix.
 */
Gan_Matrix_f *
 gan_matf_fill_va ( Gan_Matrix_f *A, unsigned long rows, unsigned long cols,
                    ... )
{
   unsigned int i, j;
   va_list ap;

   /* allocate matrix A if necessary */
   if ( A == NULL )
      A = gan_matf_alloc ( rows, cols );
   else
      A = gan_matf_set_dims ( A, rows, cols );

   if ( A == NULL )
   {
      gan_err_register ( "gan_matf_fill_va", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* read variable argument list of matrix elements */
   va_start ( ap, cols );
   for ( i = 0; i < rows; i++ )
      for ( j = 0; j < cols; j++ )
         A->data[j*rows+i] = (float)va_arg ( ap, double );

   va_end ( ap );
   return A;
}

/**
 * \brief Fill matrix from variable argument list.
 * \param A Matrix to fill or \c NULL
 * \param rows Number of rows in matrix
 * \param cols Number of columns in matrix
 * \param aptr Variable argument list of elements
 *
 * Fill matrix \a A with values from variable argument list \a aptr, and
 * set the dimensions of \a A to \a rows by \a cols.
 *
 * \return Pointer to the filled matrix.
 */
Gan_Matrix_f *
 gan_matf_fill_vap ( Gan_Matrix_f *A, unsigned long rows, unsigned long cols,
                     va_list *aptr)
{
   unsigned int i, j;

   /* allocate matrix A if necessary */
   if ( A == NULL )
      A = gan_matf_alloc ( rows, cols );
   else
      A = gan_matf_set_dims ( A, rows, cols );

   if ( A == NULL )
   {
      gan_err_register ( "gan_matf_fill_vap", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* read variable argument list of matrix elements */
   for ( i = 0; i < rows; i++ )
      for ( j = 0; j < cols; j++ )
         A->data[j*rows+i] = (float)va_arg ( *aptr, double );

   return A;
}

/**
 * \brief Fill all elements of a matrix with the same value.
 * \param A The matrix to fill or \c NULL
 * \param rows The new row size of the matrix
 * \param cols The new column size of the matrix
 * \param value The value to set each element of the matrix to
 * \return non-\c NULL on successfully filling the matrix, or \c NULL on
 *         failure.
 *
 * Set the dimensions of matrix \a A to \a rows by \a cols, and fill each
 * element with the provided value.
 *
 * \sa gan_matf_fill_const_s().
 */
Gan_Matrix_f *
 gan_matf_fill_const_q ( Gan_Matrix_f *A,
                         unsigned long rows, unsigned long cols,
                         float value )
{
   int i;

   /* allocate matrix A if necessary */
   if ( A == NULL )
      A = gan_matf_alloc ( rows, cols );
   else
      A = gan_matf_set_dims ( A, rows, cols );

   if ( A == NULL )
   {
      gan_err_register ( "gan_matf_fill_const_q", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* fill matrix with constant */
   for ( i = rows*cols-1; i >= 0; i-- )
      A->data[i] = value;

   return A;
}

/**
 * \}
 */

/**
 * \addtogroup GeneralSizeMatrixRead
 * \{
 */

/**
 * \brief Read elements of matrix from variable argument list.
 */
Gan_Bool
 gan_matf_read_va ( const Gan_Matrix_f *A, unsigned long rows, unsigned long cols, ... )
{
   unsigned int i, j;
   va_list ap;

   /* check that dimensions match */
   gan_err_test_bool ( rows == A->rows && cols == A->cols, "gan_matf_read_va",
                       GAN_ERROR_REF_OUTSIDE_MATRIX, "" );

   /* fill variable argument list of matrix element pointers */
   va_start ( ap, cols );
   for ( i = 0; i < rows; i++ )
      for ( j = 0; j < cols; j++ )
         *(va_arg ( ap, float * )) = A->data[j*A->rows+i];

   va_end ( ap );

   /* return with success */
   return GAN_TRUE;
}

/**
 * \}
 */

/**
 * \addtogroup GeneralSizeMatrixCopy
 * \{
 */

/**
 * \brief Copy matrix.
 *
 * Copy matrix \a A to matrix \a B, and return \a B.
 */
Gan_Matrix_f *
 gan_matf_copy_q ( const Gan_Matrix_f *A, Gan_Matrix_f *B )
{
   /* allocate matrix B if necessary */
   if ( B == NULL )
      B = gan_matf_alloc ( A->rows, A->cols );
   else
      B = gan_matf_set_dims ( B, A->rows, A->cols );

   if ( B == NULL )
   {
      gan_err_register ( "gan_matf_copy_q", GAN_ERROR_FAILURE, "" );
      return NULL;
   }


   /* copy matrix */
#ifdef HAVE_LAPACK
   {
      long nel = A->rows*A->cols, onei = 1;

      scopy_ ( &nel, A->data, &onei, B->data, &onei );
   }
#else   
   if ( !gan_scopy ( A->rows*A->cols, A->data, 1, B->data, 1 ) )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_matf_copy_q", GAN_ERROR_CBLAS_FAILED, "" );
      return NULL;
   }
#endif /* #ifdef HAVE_LAPACK */

   return B;
}

/**
 * \}
 */

/**
 * \addtogroup GeneralSizeMatrixScale
 * \{
 */

/**
 * \brief Multiply matrix by scalar.
 *
 * Rescale matrix \a A by scalar \a a into matrix \a B, and return \a B.
 */
Gan_Matrix_f *
 gan_matf_scale_q ( Gan_Matrix_f *A, float a, Gan_Matrix_f *B )
{
   long nel = A->rows*A->cols;

   /* allocate matrix B if necessary */
   if ( B == NULL )
      B = gan_matf_alloc ( A->rows, A->cols );
   else
      B = gan_matf_set_dims ( B, A->rows, A->cols );

   if ( B == NULL )
   {
      gan_err_register ( "gan_matf_scale_q", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* copy and scale matrix */
#ifdef HAVE_LAPACK
   {
      long onei = 1;
      
      scopy_ ( &nel, A->data, &onei, B->data, &onei );
      sscal_ ( &nel, &a, B->data, &onei );
   }
#else   
   if ( !gan_scopy ( nel, A->data, 1, B->data, 1 ) ||
        !gan_sscal ( nel, a, B->data, 1 ) )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_matf_scale_q", GAN_ERROR_CBLAS_FAILED, "" );
      return NULL;
   }
#endif /* #ifdef HAVE_LAPACK */
   
   return B;
}

/**
 * \brief Divide matrix by scalar.
 *
 * Divide matrix \a A by scalar \a a into matrix \a B, and return \a B.
 */
Gan_Matrix_f *
 gan_matf_divide_q ( Gan_Matrix_f *A, float a, Gan_Matrix_f *B )
{
   long i;

   if ( a == 0.0F )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_matf_divide_q", GAN_ERROR_DIVISION_BY_ZERO, "" );
      return NULL;
   }
   
   /* allocate matrix B if necessary */
   if ( B == NULL )
      B = gan_matf_alloc ( A->rows, A->cols );
   else
      B = gan_matf_set_dims ( B, A->rows, A->cols );

   if ( B == NULL )
   {
      gan_err_register ( "gan_matf_scale_q", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   if ( A == B )
      for ( i = (long) (A->rows*A->cols-1); i >= 0; i-- )
         B->data[i] /= a;
   else
      for ( i = (long) (A->rows*A->cols-1); i >= 0; i-- )
         B->data[i] = A->data[i]/a;
   
   return B;
}

/**
 * \}
 */

/**
 * \addtogroup GeneralSizeMatrixTranspose
 * \{
 */

/**
 * \brief Transpose matrix.
 *
 * Function to transpose generic matrix A into another matrix B.
 */
Gan_Matrix_f *
 gan_matf_tpose_q ( Gan_Matrix_f *A, Gan_Matrix_f *B )
{
   long i;

   /* allocate matrix B if necessary */
   if ( B == NULL )
      B = gan_matf_alloc ( A->cols, A->rows );
   else
   {
      if ( A == B )
      {
         /* transpose in-place */
         float swapf;
         unsigned long swapi;
         long j;

         gan_err_test_ptr ( A->rows == A->cols, "gan_matf_tpose_q",
                            GAN_ERROR_FAILURE,
                            "can only transpose square matrices in-place" );

         for ( i = (long) A->rows-1; i >= 0; i-- )
            for ( j = 0; j < i; j++ )
            {
               swapf = A->data[i*A->cols+j];
               A->data[i*A->cols+j] = B->data[j*A->rows+i];
               B->data[j*A->rows+i] = swapf;
            }

         /* swap matrix dimensions */
         swapi = A->rows;
         A->rows = A->cols;
         A->cols = swapi;
         return A;
      }
      else
         B = gan_matf_set_dims ( B, A->cols, A->rows );
   }

   if ( B == NULL )
   {
      gan_err_register ( "gan_matf_tpose_q", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* transpose matrix A != B */
#ifdef HAVE_LAPACK
   {
      long onei = 1;

      if ( A->cols <= A->rows )
         for ( i = (long)B->cols-1; i >= 0; i-- )
            scopy_ ( (long *)&B->rows, &A->data[i], (long *)&A->rows,
                     &B->data[i*B->rows], &onei );
      else
         for ( i = (long)B->rows-1; i >= 0; i-- )
            scopy_ ( (long *)&B->cols, &A->data[i*A->rows], &onei,
                     &B->data[i], (long *)&B->rows);
      
   }
#else /* !HAVE_LAPACK */
   if ( A->cols <= A->rows )
   {
      for ( i = (long)B->cols-1; i >= 0; i-- )
         if ( !gan_scopy ( B->rows, &A->data[i], A->rows,
                           &B->data[i*B->rows], 1 ) )
         {
            gan_err_register ( "gan_matf_tpose_q", GAN_ERROR_CBLAS_FAILED,
                               "" );
            return NULL;
         }
   }
   else
   {
      for ( i = (long)B->rows-1; i >= 0; i-- )
         if ( !gan_scopy ( B->cols, &A->data[i*A->rows], 1,
                           &B->data[i], B->rows ) )
         {
            gan_err_register ( "gan_matf_tpose_q", GAN_ERROR_CBLAS_FAILED,
                               "" );
            return NULL;
         }
   }
#endif /* #ifdef HAVE_LAPACK */
   
   return B;
}

/**
 * \}
 */

/**
 * \addtogroup GeneralSizeMatrixAdd
 * \{
 */

/**
 * \brief Add general size matrices
 *
 * Add two matrices \a A, \a B, each of which may be transposed, and write
 * result into \f$ C = A(^{\top}) + B(^{\top}) \f$, which is returned.
 * Transposing \a A or \a B does not affect \a A or \a B, only the result \a C.
 */
Gan_Matrix_f *
 gan_matf_add_gen ( Gan_Matrix_f *A, Gan_TposeFlag A_tr,
                   Gan_Matrix_f *B, Gan_TposeFlag B_tr, Gan_Matrix_f *C )
{
   /* result matrix dimensions */
   unsigned long Crows = (A_tr ? A->cols : A->rows),
                 Ccols = (A_tr ? A->rows : A->cols);

   /* check that the dimensions of input matrices A & B are compatible */
   gan_err_test_ptr ( B_tr ? (B->rows == Ccols && B->cols == Crows)
                           : (B->rows == Crows && B->cols == Ccols),
                      "gan_matf_add_gen", GAN_ERROR_INCOMPATIBLE, "" );

   /* allocate result matrix C if necessary */
   if ( C == NULL )
      C = gan_matf_alloc ( Crows, Ccols );
   else
      C = gan_matf_set_dims ( C, Crows, Ccols );

   if ( C == NULL )
   {
      gan_err_register ( "gan_matf_add_gen", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   if ( !A_tr && !B_tr )
#ifdef HAVE_LAPACK
   {
      long nel = Crows*Ccols; /* number of matrix elements */
      long onei = 1;
      float onef = 1.0F;

      /* add matrix data C = A + B */
      if ( C == A )
         /* in-place operation A += B */
         saxpy_ ( &nel, &onef, B->data, &onei, A->data, &onei );
      else if ( C == B )
         /* in-place operation B += A */
         saxpy_ ( &nel, &onef, A->data, &onei, B->data, &onei );
      else
      {
         /* C = A + B */
         scopy_ ( &nel, A->data, &onei, C->data, &onei );
         saxpy_ ( &nel, &onef, B->data, &onei, C->data, &onei );
      }
   }
#else
   {
      /* add matrix data C = A + B */
      if ( C == A )
      {
         if ( !gan_saxpy ( Crows*Ccols, 1.0F, B->data, 1, A->data, 1 ) )
         {
            gan_err_flush_trace();
            gan_err_register ( "gan_matf_add_gen", GAN_ERROR_CBLAS_FAILED,
                               "" );
            return NULL;
         }
      }
      else if ( C == B )
      {
         /* in-place operation B += A */
         if ( !gan_saxpy ( Crows*Ccols, 1.0F, A->data, 1, B->data, 1 ) )
         {
            gan_err_flush_trace();
            gan_err_register ( "gan_matf_add_gen", GAN_ERROR_CBLAS_FAILED,
                               "" );
            return NULL;
         }
      }
      else
      {
         /* C = A + B */
         if ( !gan_scopy ( Crows*Ccols, A->data, 1, C->data, 1 ) ||
              !gan_saxpy ( Crows*Ccols, 1.0F, B->data, 1, C->data, 1 ) )
         {
            gan_err_flush_trace();
            gan_err_register ( "gan_matf_add_gen", GAN_ERROR_CBLAS_FAILED,
                               "" );
            return NULL;
         }
      }
   }
#endif /* #ifdef HAVE_LAPACK */
   else if ( !A_tr )
   {
      unsigned long i, j;
      float *Cptr = C->data;

      /* add matrix data C = A + B^T */
      gan_err_test_ptr ( B != C, "gan_matf_add_gen",
                         GAN_ERROR_INPLACE_TRANSPOSE, "" );

      if ( A == B )
      {
         gan_err_test_ptr ( A->rows == A->cols, "gan_matf_add_gen",
                            GAN_ERROR_MATRIX_NOT_SQUARE, "" );

         for ( i = 0; i < A->rows; i++ )
            for ( j = 0; j <= i; j++ )
               *Cptr++ = A->data[i*A->cols+j] + A->data[j*A->cols+i];
      }
      else if ( A == C )
         for ( j = 0; j < A->cols; j++ )
            for ( i = 0; i < A->rows; i++ )
               *Cptr++ += B->data[i*A->cols+j];
      else
      {
         float *Aptr = A->data;

         for ( j = 0; j < A->cols; j++ )
            for ( i = 0; i < A->rows; i++ )
               *Cptr++ = B->data[i*A->cols+j] + *Aptr++;
      }
   }
   else
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_matf_add_gen", GAN_ERROR_FAILURE,
                         "illegal flag combination" );
      return NULL;
   }

   /* return result matrix */
   return C;
}

/**
 * \brief Add general size matrices producing a symmetric matrix.
 *
 * Add two matrices \a A, \a B and write result into
 * \f$ C = A + B \f$, which is returned. \a C is square and is
 * \em assumed to be symmetric. No checking of this assumption is done.
 * \a A and \a B may both be optionally implicitly transposed.
 */
Gan_SquMatrix_f *
 gan_matf_add_sym_gen (
     const Gan_Matrix_f *A, Gan_TposeFlag A_tr,
     const Gan_Matrix_f *B, Gan_TposeFlag B_tr, struct Gan_SquMatrix_f *C )
{
   /* result matrix dimensions */
   unsigned long Csize = (A_tr ? A->cols : A->rows);
   unsigned long i, j;
   float *Cptr;

   /* check that the dimensions of input matrices A,B are compatible */
   gan_err_test_ptr ( (A_tr == B_tr) ? (A->rows==B->rows && A->cols==B->cols)
                                     : (A->rows==B->cols && A->cols==B->rows),
                      "gan_matf_add_sym_gen", GAN_ERROR_INCOMPATIBLE,
                      "input matrices" );

   /* allocate result matrix C if necessary */
   if ( C == NULL )
      C = gan_symmatf_alloc ( Csize );
   else
      C = gan_symmatf_set_size ( C, Csize );

   if ( C == NULL )
   {
      gan_err_register ( "gan_matf_add_sym_gen", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* set pointer to start of result data */
   Cptr = C->data;

   if ( !A_tr && !B_tr )
      /* add matrix data C = A+B */
      for ( i = 0; i < Csize; i++ )
         for ( j = 0; j <= i; j++ )
            *Cptr++ = A->data[j*A->rows+i] + B->data[j*B->rows+i];
   else if ( !A_tr )
      /* add matrix data C = A+B^T */
      for ( i = 0; i < Csize; i++ )
         for ( j = 0; j <= i; j++ )
            *Cptr++ = A->data[j*A->rows+i] + B->data[i*B->rows+j];
   else if ( !B_tr )
      /* add matrix data C = A^T+B */
      for ( i = 0; i < Csize; i++ )
         for ( j = 0; j <= i; j++ )
            *Cptr++ = A->data[i*A->rows+j] + B->data[j*B->rows+i];
   else /* A_tr && B_tr */
      /* add matrix data C = A^T+B^T */
      for ( i = 0; i < Csize; i++ )
         for ( j = 0; j <= i; j++ )
            *Cptr++ = A->data[i*A->rows+j] + B->data[i*B->rows+j];

   /* return result matrix */
   return C;
}

/**
 * \}
 */

/**
 * \addtogroup GeneralSizeMatrixSubtract
 * \{
 */

/**
 * \brief Subtract general size matrices
 *
 * Subtract two matrices \a A, \a B, each of which may be implicitly
 * transposed, and write result into \f$ C = A - B \f$, which is returned.
 */
Gan_Matrix_f *
 gan_matf_sub_gen ( Gan_Matrix_f *A, Gan_TposeFlag A_tr,
                   Gan_Matrix_f *B, Gan_TposeFlag B_tr, Gan_Matrix_f *C )
{
   /* result matrix dimensions */
   unsigned long Crows = (A_tr ? A->cols : A->rows),
                 Ccols = (A_tr ? A->rows : A->cols);

   /* check that the dimensions of input matrices A & B are compatible */
   gan_err_test_ptr ( B_tr ? (B->rows == Ccols && B->cols == Crows)
                           : (B->rows == Crows && B->cols == Ccols),
                      "gan_matf_sub_gen", GAN_ERROR_INCOMPATIBLE, "" );

   /* allocate result matrix C if necessary */
   if ( C == NULL )
      C = gan_matf_alloc ( Crows, Ccols );
   else
      C = gan_matf_set_dims ( C, Crows, Ccols );

   if ( C == NULL )
   {
      gan_err_register ( "gan_matf_sub_gen", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   if ( !A_tr && !B_tr )
#ifdef HAVE_LAPACK
   {
      long nel = Crows*Ccols; /* number of matrix elements */
      long onei = 1;
      float minus_onef = -1.0F;

      /* subtract matrix data C = A - B */
      if ( C == A )
         /* in-place operation A -= B */
         saxpy_ ( &nel, &minus_onef, B->data, &onei, A->data, &onei );
      else if ( C == B )
      {
         /* in-place operation B = A - B */
         float onef = 1.0F;

         sscal_ ( &nel, &minus_onef, B->data, &onei );
         saxpy_ ( &nel, &onef, A->data, &onei, B->data, &onei );
      }
      else
      {
         /* C = A - B */
         scopy_ ( &nel, A->data, &onei, C->data, &onei );
         saxpy_ ( &nel, &minus_onef, B->data, &onei, C->data, &onei );
      }
   }
#else
   {
      /* subtract matrix data C = A - B */
      if ( C == A )
      {
         /* in-place operation A -= B */
         if ( !gan_saxpy ( Crows*Ccols, -1.0F, B->data, 1, A->data, 1 ) )
         {
            gan_err_flush_trace();
            gan_err_register ( "gan_matf_sub_gen", GAN_ERROR_CBLAS_FAILED,
                               "" );
            return NULL;
         }
      }
      else if ( C == B )
      {
         /* in-place operation B = A - B */
         if ( !gan_sscal ( Crows*Ccols, -1.0F, B->data, 1 ) ||
              !gan_saxpy ( Crows*Ccols, 1.0F, A->data, 1, B->data, 1 ) )
         {
            gan_err_flush_trace();
            gan_err_register ( "gan_matf_sub_gen", GAN_ERROR_CBLAS_FAILED,
                               "" );
            return NULL;
         }
      }
      else
      {
         /* C = A - B */
         if ( !gan_scopy ( Crows*Ccols, A->data, 1, C->data, 1 ) ||
              !gan_saxpy ( Crows*Ccols, -1.0F, B->data, 1, C->data, 1 ) )
         {
            gan_err_flush_trace();
            gan_err_register ( "gan_matf_sub_gen", GAN_ERROR_CBLAS_FAILED,
                               "" );
            return NULL;
         }
      }
   }
#endif /* #ifdef HAVE_LAPACK */
   else if ( !A_tr )
   {
      unsigned long i, j;
      float *Cptr = C->data;

      /* subtract matrix data C = A - B^T */
      gan_err_test_ptr ( B != C && A != B, "gan_matf_sub_gen",
                         GAN_ERROR_INPLACE_TRANSPOSE, "");

      if ( A == C )
         for ( j = 0; j < A->cols; j++ )
            for ( i = 0; i < A->rows; i++ )
               *Cptr++ -= B->data[i*A->cols+j];
      else
      {
         float *Aptr = A->data;

         for ( j = 0; j < A->cols; j++ )
            for ( i = 0; i < A->rows; i++ )
               *Cptr++ = *Aptr++ - B->data[i*A->cols+j];
      }
   }
   else
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_matf_sub_gen", GAN_ERROR_FAILURE,
                         "illegal flag combination" );
      return NULL;
   }

   /* return result matrix */
   return C;
}

/**
 * \}
 */

/**
 * \addtogroup GeneralSizeMatrixVectorProduct
 * \{
 */

/**
 * \brief Multiply matrix and a vector.
 *
 * Multiply matrix \a A and vector \a x, \a A being optionally transposed,
 * writing the result into another vector \f$ y = A(^{\top}) x \f$.
 *
 * \return Pointer to result vector \a y.
 */
Gan_Vector_f *
 gan_matf_multv_gen ( const Gan_Matrix_f *A, Gan_TposeFlag A_tr, const Gan_Vector_f *x,
                      Gan_Vector_f *y )
{
   unsigned long yrows = (A_tr ? A->cols : A->rows);

   /* check that the dimensions of input matrix A and vector x are
      compatible */
   gan_err_test_ptr ( (A_tr ? A->rows : A->cols) == x->rows,
                      "gan_matf_multv_gen", GAN_ERROR_INCOMPATIBLE, "" );

   /* allocate result vector y if necessary */
   if ( y == NULL )
      y = gan_vecf_alloc ( yrows );
   else
   {
      /* can't do in-place matrix/vector multiply with generic matrix */
      gan_err_test_ptr ( y != x,  "gan_matf_multv_gen", GAN_ERROR_FAILURE,
                         "can't do in-place multiply" );
      y = gan_vecf_set_size ( y, yrows );
   }
   
   if ( y == NULL || gan_vecf_fill_zero_q ( y, y->rows ) == NULL )
   {
      gan_err_register ( "gan_matf_multv_gen", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* compute matrix/vector product */
   if ( gan_blasf_gemv ( y, 1.0F, A, A_tr, x, 0.0F ) == NULL )
   {
      gan_err_register ( "gan_matf_multv_gen", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* return result vector */
   return y;
}

/**
 * \}
 */

/**
 * \addtogroup GeneralSizeMatrixMatrixProduct
 * \{
 */

/* Function to multipy generic matrices: C = A(^T)*B(^T), returning C.
 * Not a user function
 */
Gan_Matrix_f *
 gan_matf_rmult_gen ( const Gan_Matrix_f *A, Gan_TposeFlag A_tr,
                      const Gan_Matrix_f *B, Gan_TposeFlag B_tr, Gan_Matrix_f *C )
{
   /* result matrix dimensions */
   unsigned long Crows = (A_tr ? A->cols : A->rows),
                 Ccols = (B_tr ? B->rows : B->cols);
   unsigned long common = (A_tr ? A->rows : A->cols);

   /* check that the dimensions of input matrices A,B are compatible */
   gan_err_test_ptr ( (B_tr ? B->cols : B->rows) == common,
                      "gan_matf_rmult_gen", GAN_ERROR_INCOMPATIBLE, "" );

   /* allocate result matrix C if necessary */
   if ( C == NULL )
      C = gan_matf_alloc ( Crows, Ccols );
   else
      C = gan_matf_set_dims ( C, Crows, Ccols );

   if ( C == NULL || gan_matf_fill_zero_q ( C, C->rows, C->cols ) == NULL )
   {
      gan_err_register ( "gan_matf_rmult_gen", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* can't do in-place matrix multiply */
   if ( A == C || B == C )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_matf_rmult_gen", GAN_ERROR_FAILURE,
                         "can't do matrix multiplication in-place" );
      return NULL;
   }

   /* multiply matrix data */
#ifdef HAVE_LAPACK
   {
      float dOne = 1.0F, dZero = 0.0F;
      long lOne = 1;

      sgemm_ ( A_tr ? "T" : "N", B_tr ? "T" : "N",
               (long *)&Crows, (long *)&Ccols, (long *)&common, &dOne,
               A->data, (A->rows == 0) ? &lOne : (long *)&A->rows,
               B->data, (B->rows == 0) ? &lOne : (long *)&B->rows, &dZero,
               C->data, (C->rows == 0) ? &lOne : (long *)&C->rows );
   }
#else   
   if ( !gan_sgemm ( A_tr, B_tr, Crows, Ccols, common, 1.0F,
                     A->data, (A->rows == 0) ? 1 : A->rows,
                     B->data, (B->rows == 0) ? 1 : B->rows, 0.0F,
                     C->data, (C->rows == 0) ? 1 : C->rows ) )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_matf_rmult_gen", GAN_ERROR_CBLAS_FAILED, "" );
      return NULL;
   }
#endif /* #ifdef HAVE_LAPACK */

   /* return result matrix */
   return C;
}

/* Function to multiply two generic matrices, producing a symmetric matrix
 * Not a user function
 */
Gan_SquMatrix_f *
 gan_matf_rmult_sym_gen ( const Gan_Matrix_f *A, Gan_TposeFlag A_tr,
                          const Gan_Matrix_f *B, Gan_TposeFlag B_tr,
                          struct Gan_SquMatrix_f *C )
{
   /* result matrix dimensions */
   unsigned long Csize = (A_tr ? A->cols : A->rows);
   unsigned long common = (A_tr ? A->rows : A->cols);
   unsigned long i, j, k;
   float total, *Cptr;

   /* check that the dimensions of input matrices A,B are compatible */
   gan_err_test_ptr ( (B_tr ? B->cols : B->rows) == common &&
                      (B_tr ? B->rows : B->cols) == Csize,
                      "gan_matf_rmult_sym_gen", GAN_ERROR_INCOMPATIBLE, "" );

   /* allocate result matrix C if necessary */
   if ( C == NULL )
      C = gan_symmatf_alloc ( Csize );
   else
      C = gan_symmatf_set_size ( C, Csize );

   if ( C == NULL )
   {
      gan_err_register ( "gan_matf_rmult_sym_gen", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* set pointer to start of result matrix data */
   Cptr = C->data;

   if ( !A_tr && !B_tr )
      /* multiply matrix data C = A*B */
      for ( i = 0; i < Csize; i++ )
         for ( j = 0; j <= i; j++ )
         {
            total = 0.0F;
            for ( k = 0; k < common; k++ )
               total += A->data[k*A->rows+i]*B->data[j*B->rows+k];

         *Cptr++ = total;
      }
   else if ( !A_tr )
      /* multiply matrix data C = A*B^T */
      for ( i = 0; i < Csize; i++ )
         for ( j = 0; j <= i; j++ )
         {
            total = 0.0F;
            for ( k = 0; k < common; k++ )
               total += A->data[k*A->rows+i]*B->data[k*B->rows+j];

            *Cptr++ = total;
         }
   else if ( !B_tr )
      /* multiply matrix data C = A^T*B */
      for ( i = 0; i < Csize; i++ )
         for ( j = 0; j <= i; j++ )
         {
            total = 0.0F;
            for ( k = 0; k < common; k++ )
               total += A->data[i*A->rows+k]*B->data[j*B->rows+k];

            *Cptr++ = total;
         }
   else /* A_tr && B_tr */
      /* multiply matrix data C = A^T*B^T */
      for ( i = 0; i < Csize; i++ )
         for ( j = 0; j <= i; j++ )
         {
            total = 0.0F;
            for ( k = 0; k < common; k++ )
               total += A->data[i*A->rows+k]*B->data[k*B->rows+j];

            *Cptr++ = total;
         }

   /* return result matrix */
   return C;
}

/**
 * \}
 */

/**
 * \addtogroup GeneralSizeMatrixInsert
 * \{
 */

/**
 * \brief Insert part of generic matrix in another generic matrix.
 *
 * Fills a rectangular part of generic matrix \a A starting at row,column
 * position \a rA, \a cA and \a rows by \a cols in size with the section
 * of generic matrix \a B, starting at position \a rB, \a cB.
 */
Gan_Matrix_f *
 gan_matf_insert_gen ( Gan_Matrix_f *A, unsigned long rA, unsigned long cA,
                       const Gan_Matrix_f *B, Gan_TposeFlag B_tr,
                       unsigned long rB, unsigned long cB,
                       unsigned long rows , unsigned long cols )
{
   long i, j;

   if ( B_tr )
   {
      /* make sure dimensions are consistent */
      gan_err_test_ptr ( cB + rows <= B->cols && rB + cols <= B->rows &&
                         cA + cols <= A->cols && rA + rows <= A->rows,
                         "gan_matf_insert_gen",
                         GAN_ERROR_INCOMPATIBLE, "input matrices" );

      for ( i = rows-1; i >= 0; i-- )
         for ( j = cols-1; j >= 0; j-- )
            A->data[(cA+j)*A->rows + rA+i] = B->data[(cB+i)*B->rows + rB+j];
   }
   else
   {
      /* make sure dimensions are consistent */
      gan_err_test_ptr ( cB + cols <= B->cols && rB + rows <= B->rows &&
                         cA + cols <= A->cols && rA + rows <= A->rows,
                         "gan_matf_insert_gen",
                         GAN_ERROR_INCOMPATIBLE, "input matrices" );

      for ( i = rows-1; i >= 0; i-- )
         for ( j = cols-1; j >= 0; j-- )
            A->data[(cA+j)*A->rows + rA+i] = B->data[(cB+j)*B->rows + rB+i];
   }

   return A;
}

/**
 * \brief Insert part of column vector in a generic matrix.
 *
 * Fills a section of a column of generic matrix \a A starting at
 * row,column position \a rA,\a cA with a section \a rows elements
 * in size, taken from column vector \a x, starting at position \a rx.
 */
Gan_Matrix_f *
 gan_matf_insertv_gen ( Gan_Matrix_f *A, unsigned long rA, unsigned long cA,
                        const Gan_Vector_f *x, Gan_TposeFlag x_tr, unsigned long rx,
                        unsigned long size )
{
   long i;

   if ( x_tr )
   {
      /* make sure dimensions are consistent */
      gan_err_test_ptr ( rx + size <= x->rows &&
                         cA + size <= A->cols && rA < A->rows,
                         "gan_matf_insertv_gen",
                         GAN_ERROR_INCOMPATIBLE, "input matrices");

      for ( i = size-1; i >= 0; i-- )
         A->data[(cA+i)*A->rows + rA] = x->data[rx+i];
   }
   else
   {
      /* make sure dimensions are consistent */
      gan_err_test_ptr ( rx + size <= x->rows &&
                         cA < A->cols && rA + size <= A->rows,
                         "gan_matf_insertv_gen",
                         GAN_ERROR_INCOMPATIBLE, "input matrices");

      for ( i = size-1; i >= 0; i-- )
         A->data[cA*A->rows + rA+i] = x->data[rx+i];
   }

   return A;
}

/**
 * \brief Insert part of symmetric matrix in a generic matrix.
 *
 * Fills a rectangular part of generic matrix \a A starting at row,column
 * position \a rA,\a cA and \a rows by \a cols in size with the section
 * of symmetric matrix \a B, starting at position \a rB, \a cB.
 */
Gan_Matrix_f *
 gan_matf_insertsym ( Gan_Matrix_f    *A, unsigned long rA, unsigned long cA,
                      const Gan_SquMatrix_f *B, unsigned long rB, unsigned long cB,
                      unsigned long rows , unsigned long cols )
{
   long i, j;

   /* make sure dimensions are consistent */
   gan_err_test_ptr ( cB + cols <= B->size && rB + rows <= B->size &&
                      cA + cols <= A->cols && rA + rows <= A->rows,
                      "gan_matf_insertsym",
                      GAN_ERROR_INCOMPATIBLE, "input matrices" );

   if ( rB < cB )
   {
      /* read from region of "source" above/right of diagonal */

      /* make sure region doesn't cross source matrix diagonal */
      gan_err_test_ptr ( rB + rows <= cB, "gan_matf_insertsym",
                         GAN_ERROR_INCOMPATIBLE, "input matrices" );

      for ( i = rows-1; i >= 0; i-- )
         for ( j = cols-1; j >= 0; j-- )
            A->data[(cA+j)*A->rows + rA+i] = B->data[(cB+j)*(cB+j+1)/2 + rB+i];
   }
   else if ( rB > cB )
   {
      /* read from region of "source" below/left of diagonal */

      /* make sure region doesn't cross source matrix diagonal */
      gan_err_test_ptr ( cB + cols <= rB, "gan_matf_insertsym",
                         GAN_ERROR_INCOMPATIBLE, "input matrices" );

      for ( i = rows-1; i >= 0; i-- )
         for ( j = cols-1; j >= 0; j-- )
            A->data[(cA+j)*A->rows + rA+i] = B->data[(rB+i)*(rB+i+1)/2 + cB+j];
   }
   else gan_assert ( 0,  "illegal region in gan_matf_insertsym()" );

   return A;
}

/**
 * \}
 */

/**
 * \addtogroup GeneralSizeMatrixScale
 * \{
 */

/**
 * \brief Free a \c NULL terminated variable argument list of matrices.
 *
 * Free a \c NULL terminated variable argument list of matrices, starting
 * with matrix \a A.
 */
void
 gan_matf_free_va ( Gan_Matrix_f *A, ... )
{
   va_list ap;

   va_start ( ap, A );
   while ( A != NULL )
   {
      /* free next matrix */
      gan_matf_free ( A );

      /* get next matrix in list */
      A = va_arg ( ap, Gan_Matrix_f * );
   }

   va_end ( ap );
}


/* not a user function */
Gan_Matrix_f *
 gan_matf_form_gen ( Gan_Matrix_f *A, unsigned long rows, unsigned long cols,
                     float *data, size_t data_size )
{
   if ( A == NULL )
   {
      /* dyamically allocate matrix */
      A = gan_malloc_object(Gan_Matrix_f);
      if ( A == NULL )
      {
         gan_err_flush_trace();
         gan_err_register_with_number ( "gan_matf_form_gen", GAN_ERROR_MALLOC_FAILED, "", sizeof(*A) );
         return NULL;
      }

      /* set dynamic allocation flag */
      A->struct_alloc = GAN_TRUE;
   }
   else
      /* indicate matrix was not dynamically allocated */
      A->struct_alloc = GAN_FALSE;

   if ( data == NULL )
   {
      if ( data_size == 0 )
         /* set size of matrix data to be allocated */
         A->data_size = rows*cols;
      else
      {
         /* a non-zero data size has been requested */
         gan_err_test_ptr ( data_size >= rows*cols, "gan_matf_form_gen",
                            GAN_ERROR_ARRAY_TOO_SMALL, "" );
         A->data_size = data_size;
      }

      /* allocate matrix data */
      if ( A->data_size == 0 ) A->data = NULL;
      else
      {
         A->data = gan_malloc_array ( float, A->data_size );
         if ( A->data == NULL )
         {
            gan_err_flush_trace();
            gan_err_register_with_number ( "gan_matf_form_gen", GAN_ERROR_MALLOC_FAILED, "", A->data_size*sizeof(float) );
            return NULL;
         }
      }

      A->data_alloc = GAN_TRUE;
   }
   else
   {
      /* data for this matrix is provided */
      gan_err_test_ptr ( data_size >= rows*cols, "gan_matf_form_gen",
                         GAN_ERROR_ARRAY_TOO_SMALL, "" );

      /* set data pointer in matrix */
      A->data = data;
      A->data_size = data_size;

      /* we shouldn't free the matrix data */
      A->data_alloc = GAN_FALSE;
   }

   /* set matrix dimensions */
   A->rows = rows;
   A->cols = cols;

   return A;
}

/**
 * \}
 */

/**
 * \addtogroup GeneralSizeMatrixExtract
 * \{
 */

/**
 * \brief Extracts rectangular part of matrix.
 * \param A Input Matrix
 * \param r0 row offset of start of region to be extracted
 * \param c0 column offset of start of region to be extracted
 * \param rows Height of region to be extracted
 * \param cols Width of region to be extracted
 * \param B output matrix
 * \return Pointer to the output matrix \a B, or \c NULL on failure.
 *
 * Extracts the given rectangular part of matrix \a A, copies it into matrix
 * \a B, and returns \a B. If \a B is passed as \c NULL, the output matrix is
 * allocated inside the function, and then returned.
 * \sa gan_matf_extract_s(), gan_matf_insert().
 */
Gan_Matrix_f *
 gan_matf_extract_q ( const Gan_Matrix_f *A,
                      unsigned long r0,   unsigned long c0,
                      unsigned long rows, unsigned long cols,
                      Gan_Matrix_f *B )
{
   gan_err_test_ptr ( A->rows >= r0+rows && A->cols >= c0+cols,
                      "gan_matf_extract_q", GAN_ERROR_INCOMPATIBLE,
                      "input matrix not big enough" );

   /* allocate/set output matrix */
   if ( B == NULL )
      B = gan_matf_alloc ( rows, cols );
   else
      B = gan_matf_set_dims ( B, rows, cols );

   if ( B == NULL )
   {
      gan_err_register ( "gan_matf_extract_q", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   if ( gan_matf_insert ( B, 0, 0, A, r0, c0, rows, cols ) == NULL )
   {
      gan_err_register ( "gan_matf_extract_q", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* success */
   return B;
}

/**
 * \}
 */

/**
 * \addtogroup GeneralSizeMatrixIO
 * \{
 */

/**
 * \brief Print matrix to file pointer.
 *
 * Print matrix \a A to file pointer \a fp, with prefix string \a prefix,
 * indentation \a indent and floating-point format \a fmt.
 *
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 */
Gan_Bool
 gan_matf_fprint ( FILE *fp, const Gan_Matrix_f *A, const char *prefix,
                  unsigned indent, const char *fmt )
{
   unsigned long i, j;
   int p;

   /* print indentation on first line */
   for ( p = indent-1; p >= 0; p-- ) fprintf ( fp, " " );
   fprintf ( fp, "%s (%0ldx%0ld matrix)\n", prefix, A->rows, A->cols );

   if ( A->cols > 0 )
      for ( i = 0; i < A->rows; i++ )
      {
         for ( p = indent-1; p >= 0; p-- ) fprintf ( fp, " " );
         for ( j = 0; j < A->cols; j++ )
         {
            fprintf ( fp, " " );
            fprintf ( fp, fmt, A->data[j*A->rows+i] );
         }

         fprintf ( fp, "\n" );
      }

   /* return with success */
   return GAN_TRUE;
}

/**
 * \brief Read matrix from file.
 *
 * Read matrix \a A from file pointer \a fp. The prefix string for
 * the matrix is read from the file into the \a prefix string, up to the
 * maximum length \a prefix_len of the \a prefix string. Any remaining
 * characters after "prefix" has been filled are ignored. Pass \c NULL for
 * \a prefix and zero for \a prefix_len to ignore the prefix string.
 * \a A should either be a pre-allocated matrix pointer or \c NULL.
 *
 * \return Pointer to the read matrix on success, \c NULL on failure.
 *
 * \sa gan_matf_fscanf_s().
 */
Gan_Matrix_f *
 gan_matf_fscanf_q ( FILE *fp, Gan_Matrix_f *A, char *prefix, int prefix_len )
{
   int ch;
   unsigned long rows, cols, i, j;

   /* read indentation */
   for(;;)
      if ( (ch = getc(fp)) != ' ' || ch == EOF )
         break;

   /* end of file means corrupted file */
   if ( ch == EOF )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_matf_fscanf_q", GAN_ERROR_CORRUPTED_FILE, "" );
      return NULL;
   }

   /* rewind one character to reread the prefix string */
   fseek ( fp, -1, SEEK_CUR );

   /* read prefix string, terminated by end-of-line */
   for ( prefix_len--; prefix_len > 0; prefix_len-- )
   {
      ch = getc(fp);
      if ( ch == EOF || ch == '(' ) break;
      if ( prefix != NULL ) *prefix++ = (char)ch;
   }

   gan_err_test_ptr ( ch == '(', "gan_matf_fscanf_q",
                      GAN_ERROR_CORRUPTED_FILE, "" );

   /* terminate string */
   if ( prefix != NULL ) prefix[-1] = '\0';

   /* read rest of string if necessary */
   if ( prefix_len == 0 )
   {
      for(;;)
      {
         ch = getc(fp);
         if ( ch == EOF || ch == '(' ) break;
      }

      gan_err_test_ptr ( ch == '(', "gan_matf_fscanf_q",
                         GAN_ERROR_CORRUPTED_FILE, "" );
   }

   /* read dimensions */
   if ( fscanf ( fp, "%ldx%ld matrix)\n", &rows, &cols ) != 2 )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_matf_fscanf_q", GAN_ERROR_CORRUPTED_FILE, "" );
      return NULL;
   }

   /* create/set matrix */
   if ( A == NULL )
      A = gan_matf_alloc ( rows, cols );
   else
      A = gan_matf_set_dims ( A, rows, cols );

   if ( A == NULL )
   {
      gan_err_register ( "gan_matf_fscanf_q", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* read matrix data */
   if ( A->cols > 0 )
      for ( i = 0; i < A->rows; i++ )
      {
         for ( j = 0; j < A->cols; j++ )
            if ( fscanf ( fp, "%f", &A->data[j*A->rows+i] ) != 1 )
            {
               gan_err_flush_trace();
               gan_err_register ( "gan_matf_fscanf_q",
                                  GAN_ERROR_CORRUPTED_FILE, "" );
               return NULL;
            }

         /* read end-of-line character */
#ifdef NDEBUG
         getc(fp);
#else
         gan_err_test_ptr ( getc(fp) == '\n', "gan_matf_fscanf_q",
                            GAN_ERROR_CORRUPTED_FILE, "" );
#endif
      }

   /* success */
   return A;
}

/**
 * \brief Print matrix to binary file pointer.
 *
 * Print matrix \a A to file pointer \a fp in binary format, with 32-bit
 * magic number printed first.
 *
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 */
Gan_Bool
 gan_matf_fwrite ( FILE *fp, const Gan_Matrix_f *A, gan_uint32 magic_number )
{
   if ( !gan_fwrite_lendian_ui32 ( fp, &magic_number, 1 ) ||
        !gan_fwrite_lendian_ul ( fp, &A->rows, 1 ) ||
        !gan_fwrite_lendian_ul ( fp, &A->cols, 1 ) ||
        !gan_fwrite_lendian_f32 ( fp, A->data, A->rows*A->cols ) )
   {
      gan_err_register ( "gan_matf_fwrite", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   return GAN_TRUE;
}

/**
 * \brief Read matrix from file in binary format.
 *
 * Read matrix \a A from file pointer \a fp in binary format. The magic
 * number is read into the provided pointer.
 * \a A should either be a pre-allocated matrix pointer or \c NULL.
 *
 * \return Pointer to the read matrix on success, \c NULL on failure.
 *
 * \sa gan_vecf_fread_s()
 */
Gan_Matrix_f *
 gan_matf_fread_q ( FILE *fp, Gan_Matrix_f *A, gan_uint32 *magic_number )
{
   unsigned long rows, cols;

   /* read magic number */
   if ( !gan_fread_lendian_ui32 ( fp, magic_number, 1 ) )
   {
      gan_err_register ( "gan_vecf_fread_q", GAN_ERROR_CORRUPTED_FILE, "" );
      return NULL;
   }

   /* read size of matrix */
   if ( !gan_fread_lendian_ul ( fp, &rows, 1 ) ||
        !gan_fread_lendian_ul ( fp, &cols, 1 ) )
   {
      gan_err_register ( "gan_matf_fread_q", GAN_ERROR_CORRUPTED_FILE, "" );
      return NULL;
   }

   /* create/set matrix */
   if ( A == NULL )
      A = gan_matf_alloc ( rows, cols );
   else
      A = gan_matf_set_dims ( A, rows, cols );

   if ( A == NULL )
   {
      gan_err_register ( "gan_matf_fread_q", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* read matrix data */
   if ( !gan_fread_lendian_f32 ( fp, A->data, A->rows*A->cols ) )
   {
      gan_err_register ( "gan_matf_fread_q", GAN_ERROR_CORRUPTED_FILE, "" );
      return NULL;
   }

   /* success */
   return A;
}

/**
 * \}
 */

#ifndef NDEBUG
/* for use in a debugger: use gan_matf_print() or gan_matf_fprint in
 * your program!
 */
void gan_matf_db ( const Gan_Matrix_f *A )
{
   gan_matf_print ( A, "", 0, "%+g" );
}
#endif

/**
 * \}
 */

/**
 * \}
 */

/**
 * \}
 */

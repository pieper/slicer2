/**
 * File:          $RCSfile: mat_diagonal.c,v $
 * Module:        Diagonal matrices (double precision)
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
#include <math.h>
#include <gandalf/linalg/mat_diagonal.h>
#include <gandalf/linalg/mat_symmetric.h>
#include <gandalf/linalg/mat_gen.h>
#include <gandalf/linalg/vec_gen.h>
#include <gandalf/linalg/clapack.h>
#include <gandalf/linalg/cblas.h>
#include <gandalf/common/numerics.h>
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
 * \addtogroup GeneralSizeMatrix
 * \{
 */

/* function to set element i,j of diagonal matrix */
static Gan_Bool
 diagonal_matrix_set_el ( Gan_SquMatrix *A,
                          unsigned i, unsigned j, double value )
{
   /* consistency check */
   assert ( A->type == GAN_DIAGONAL_MATRIX );

   /* make sure element is on the diagonal */
   assert ( i == j && i < A->size && j < A->size );

   /* set i'th diagonal element to given value */
   A->data[i] = value;

   /* return with success */
   return GAN_TRUE;
}

/* function to return element i,j of diagonal matrix */
static double
 diagonal_matrix_get_el ( const Gan_SquMatrix *A, unsigned i, unsigned j )
{
   /* consistency check */
   assert ( A->type == GAN_DIAGONAL_MATRIX );

   /* make sure element is inside matrix */
   assert ( i < A->size && j < A->size );

   if ( i == j )
      /* get i'th diagonal element */
      return A->data[i];

   /* all off-diagonal elements are zero */
   return 0.0;
}

/* function to increment element i,j of diagonal matrix by given value */
static Gan_Bool
 diagonal_matrix_inc_el ( Gan_SquMatrix *A,
                          unsigned i, unsigned j, double value )
{
   /* consistency check */
   assert ( A->type == GAN_DIAGONAL_MATRIX );

   /* make sure element is on the diagonal */
   assert ( i == j && i < A->size && j < A->size );

   /* increment i'th diagonal element by given value */
   A->data[i] += value;

   /* return with success */
   return GAN_TRUE;
}

/* function to decrement element i,j of diagonal matrix by given value */
static Gan_Bool
 diagonal_matrix_dec_el ( Gan_SquMatrix *A,
                          unsigned i, unsigned j, double value )
{
   /* consistency check */
   assert ( A->type == GAN_DIAGONAL_MATRIX );

   /* make sure element is on the diagonal */
   assert ( i == j && i < A->size && j < A->size );

   /* decrement i'th diagonal element by given value */
   A->data[i] -= value;

   /* return with success */
   return GAN_TRUE;
}

/* function to print diagonal matrix to file pointer */
static Gan_Bool
 diagonal_matrix_fprint ( FILE *fp, const Gan_SquMatrix *A, const char *prefix,
                          unsigned indent, const char *fmt )
{
   unsigned long i, j;
   int p;

   /* consistency check */
   assert ( A->type == GAN_DIAGONAL_MATRIX );

   /* print indentation on first line */
   for ( p = indent-1; p >= 0; p-- ) fprintf ( fp, " " );
   fprintf ( fp, "%s (%0ldx%0ld diagonal matrix)\n", prefix,
             A->size, A->size );

   for ( i = 0; i < A->size; i++ )
   {
      for ( p = indent-1; p >= 0; p-- ) fprintf ( fp, " " );
      for ( j = 0; j < A->size; j++ )
      {
         fprintf ( fp, " " );
         if ( j == i )
            fprintf ( fp, fmt, A->data[i] );
         else
            fprintf ( fp, fmt, 0.0 );
      }

      fprintf ( fp, "\n" );
   }

   /* return with success */
   return GAN_TRUE;
}

/* function to copy diagonal matrix A into another matrix B */
static Gan_SquMatrix *
 diagonal_matrix_copy ( const Gan_SquMatrix *A, Gan_SquMatrix *B )
{
#ifdef HAVE_LAPACK
   long onei = 1;
#endif

   /* consistency check */
   assert ( A->type == GAN_DIAGONAL_MATRIX );

   /* allocate matrix B if necessary */
   if ( B == NULL )
      B = gan_diagmat_alloc ( A->size );
   else
      B = gan_diagmat_set_size ( B, A->size );

   /* copy matrix */
#ifdef HAVE_LAPACK
   dcopy_ ( (long *)&A->size, A->data, &onei, B->data, &onei );
#else /* !HAVE_LAPACK */
   gan_dcopy ( A->size, A->data, 1, B->data, 1 );
#endif /* #ifdef HAVE_LAPACK */

   return B;
}

/* function to rescale diagonal matrix A into another matrix B */
static Gan_SquMatrix *
 diagonal_matrix_scale ( Gan_SquMatrix *A, double a, Gan_SquMatrix *B )
{
#ifdef HAVE_LAPACK
   long onei = 1;
#endif

   /* consistency check */
   assert ( A->type == GAN_DIAGONAL_MATRIX );

   /* allocate matrix B if necessary */
   if ( B == NULL )
      B = gan_diagmat_alloc ( A->size );
   else
      B = gan_diagmat_set_size ( B, A->size );

#ifdef HAVE_LAPACK
   /* copy and scale matrix */
   if(A!=B) dcopy_ ( (long *)&A->size, A->data, &onei, B->data, &onei );
   dscal_ ( (long *)&A->size, &a, B->data, &onei );
#else /* !HAVE_LAPACK */
   /* copy and scale matrix */
   if(A!=B) gan_dcopy ( A->size, A->data, 1, B->data, 1 );
   gan_dscal ( A->size, a, B->data, 1 );
#endif /* #ifdef HAVE_LAPACK */

   return B;
}

/* function to add diagonal matrices together C = A+B, returning C */
static Gan_SquMatrix *
 diagonal_matrix_add ( Gan_SquMatrix *A, Gan_TposeFlag A_tr,
                       Gan_SquMatrix *B, Gan_TposeFlag B_tr, Gan_SquMatrix *C )
{
#ifdef HAVE_LAPACK
   long onei = 1;
   double onef = 1.0;
#endif

   /* consistency check */
   assert ( A->type == GAN_DIAGONAL_MATRIX );

   /* consistency check */
   assert ( B->type == GAN_DIAGONAL_MATRIX );

   /* check that the type and dimensions of input matrices A & B are
      compatible */
   gan_err_test_ptr ( gan_squmat_same_type_size(A,B),
                      "diagonal_matrix_add", GAN_ERROR_INCOMPATIBLE, "" );

   /* allocate result matrix C if necessary */
   if ( C == NULL )
      C = gan_diagmat_alloc ( A->size );
   else
      C = gan_diagmat_set_size ( C, A->size );

#ifdef HAVE_LAPACK
   /* add matrix data */
   if ( C == A )
      /* in-place operation A += B */
      daxpy_ ( (long *)&A->size, &onef, B->data, &onei, A->data, &onei );
   else if ( C == B )
      /* in-place operation B += A */
      daxpy_ ( (long *)&A->size, &onef, A->data, &onei, B->data, &onei );
   else
   {
      /* C = A + B */
      dcopy_ ( (long *)&A->size, A->data, &onei, C->data, &onei );
      daxpy_ ( (long *)&A->size, &onef, B->data, &onei, C->data, &onei );
   }
#else /* !HAVE_LAPACK */
   /* add matrix data */
   if ( C == A )
      /* in-place operation A += B */
      gan_daxpy ( A->size, 1.0, B->data, 1, A->data, 1 );
   else if ( C == B )
      /* in-place operation B += A */
      gan_daxpy ( A->size, 1.0, A->data, 1, B->data, 1 );
   else
   {
      /* C = A + B */
      gan_dcopy ( A->size, A->data, 1, C->data, 1 );
      gan_daxpy ( A->size, 1.0, B->data, 1, C->data, 1 );
   }
#endif /* #ifdef HAVE_LAPACK */

   /* return result matrix */
   return C;
}

/* function to subtract diagonal matrices from each other: C = A-B,
 * returning C
 */
static Gan_SquMatrix *
 diagonal_matrix_sub ( Gan_SquMatrix *A, Gan_TposeFlag A_tr,
                       Gan_SquMatrix *B, Gan_TposeFlag B_tr, Gan_SquMatrix *C )
{
#ifdef HAVE_LAPACK
   long onei = 1;
   double minus_onef = -1.0;
#endif

   /* consistency check */
   assert ( A->type == GAN_DIAGONAL_MATRIX );

   /* check that the type and dimensions of input matrices A & B are
      compatible */
   gan_err_test_ptr ( gan_squmat_same_size(A,B), "diagonal_matrix_sub",
                      GAN_ERROR_INCOMPATIBLE, "" );

   switch ( B->type )
   {
      case GAN_DIAGONAL_MATRIX:

      /* allocate result matrix C if necessary */
      if ( C == NULL )
         C = gan_diagmat_alloc ( A->size );
      else
         C = gan_diagmat_set_size ( C, A->size );

#ifdef HAVE_LAPACK
      /* subtract matrix data */
      if ( C == A )
         /* in-place operation A -= B */
         daxpy_ ( (long *)&A->size, &minus_onef, B->data, &onei,
                  A->data, &onei );
      else if ( C == B )
      {
         /* in-place operation B = A - B */
         double onef = 1.0;

         dscal_ ( (long *)&A->size, &minus_onef, B->data, &onei );
         daxpy_ ( (long *)&A->size, &onef, A->data, &onei, B->data, &onei );
      }
      else
      {
         /* C = A - B */
         dcopy_ ( (long *)&A->size, A->data, &onei, C->data, &onei );
         daxpy_ ( (long *)&A->size, &minus_onef, B->data, &onei,
                  C->data, &onei );
      }
#else /* !HAVE_LAPACK */
      /* subtract matrix data */
      if ( C == A )
         /* in-place operation A -= B */
         gan_daxpy ( A->size, -1.0, B->data, 1, A->data, 1 );
      else if ( C == B )
      {
         /* in-place operation B = A - B */
         gan_dscal ( A->size, -1.0, B->data, 1 );
         gan_daxpy ( A->size,  1.0, A->data, 1, B->data, 1 );
      }
      else
      {
         /* C = A - B */
         gan_dcopy ( A->size, A->data, 1, C->data, 1 );
         gan_daxpy ( A->size, -1.0, B->data, 1, C->data, 1 );
      }
#endif /* #ifdef HAVE_LAPACK */

      break;

      case GAN_SYMMETRIC_MATRIX:
      {
         long i, j;

         /* allocate result matrix C if necessary */
         if ( C == NULL )
            C = gan_symmat_alloc ( A->size );
         else
            C = gan_symmat_set_size ( C, A->size );

         /* test whether subtraction is being done in-place */
         if ( A == C )
         {
            /* convert data in A to symmetric */

            /* copy diagonal elements, subtracting B */
            for ( i = A->size-1; i >= 0; i-- )
               A->data[i*(i+1)/2+i] = A->data[i] - B->data[i*(i+1)/2+i];

            /* set off-diagonal elements to negative of elements of B  */
            for ( i = A->size-1; i >= 0; i-- )
               for ( j = i-1; j >= 0; j-- )
                  A->data[i*(i+1)/2+j] = -B->data[i*(i+1)/2+j];
         }
         else
            /* subtract matrix data */
            for ( i = A->size-1; i >= 0; i-- )
               C->data[i*(i+1)/2] = A->data[i] - B->data[i*(i+1)/2];
      }
      break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "diagonal_matrix_sub()", GAN_ERROR_ILLEGAL_TYPE,
                           "" );
        return NULL;
   }

   /* return result matrix */
   return C;
}

/* function to compute matrix/vector product A*x = y, returning y */
static Gan_Vector *
 diagonal_matrix_multv (
     const Gan_SquMatrix *A, Gan_TposeFlag A_tr, Gan_InvertFlag A_inv,
     Gan_Vector *x, Gan_Vector *y )
{
   long i;

   /* consistency check */
   assert ( A->type == GAN_DIAGONAL_MATRIX );

   /* check that the dimensions of input matrix A and vector x are
      compatible */
   gan_err_test_ptr ( A->size == x->rows, "diagonal_matrix_multv",
                      GAN_ERROR_INCOMPATIBLE, "" );

   /* allocate result vector y if necessary */
   if ( y == NULL )
      y = gan_vec_alloc ( A->size );
   else
      y = gan_vec_set_size ( y, A->size );

   if ( x == y )
      if ( A_inv )
         /* compute matrix/vector product x <-- A^-1*x */
         for ( i = A->size-1; i >= 0; i-- )
         {
            if ( A->data[i] == 0.0 )
            {
               gan_err_flush_trace();
               gan_err_register ( "diagonal_matrix_multv",
                                  GAN_ERROR_DIVISION_BY_ZERO, "" );
               return NULL;
            }

            y->data[i] /= A->data[i];
         }
      else
         /* compute matrix/vector product x <-- A*x */
         for ( i = A->size-1; i >= 0; i-- )
            y->data[i] *= A->data[i];
   else
      if ( A_inv )
         /* compute matrix/vector product y = A^-1*x */
         for ( i = A->size-1; i >= 0; i-- )
         {
            if ( A->data[i] == 0.0 )
            {
               gan_err_flush_trace();
               gan_err_register ( "diagonal_matrix_multv",
                                  GAN_ERROR_DIVISION_BY_ZERO, "" );
               return NULL;
            }

            y->data[i] = x->data[i]/A->data[i];
         }
      else
         /* compute matrix/vector product y = A*x */
         for ( i = A->size-1; i >= 0; i-- )
            y->data[i] = x->data[i]*A->data[i];

   /* return result vector */
   return y;
}

/* function to multipy diagonal matrix A and generic matrix B: C = A*B(^T),
 * returning C
 */
static Gan_Matrix *
 diagonal_matrix_rmult (
     const Gan_SquMatrix *A, Gan_TposeFlag A_tr, Gan_InvertFlag A_inv,
     Gan_Matrix          *B, Gan_TposeFlag B_tr, Gan_Matrix *C )
{
   unsigned long Ccols = B_tr ? B->rows : B->cols;
   unsigned long i, j;
   double *Cptr;

   /* consistency check */
   gan_err_test_ptr ( A->type == GAN_DIAGONAL_MATRIX, "diagonal_matrix_rmult",
                      GAN_ERROR_ILLEGAL_TYPE, "" );

   /* check that the dimensions of input matrices A,B,C are compatible */
   gan_err_test_ptr ( A->size == (B_tr ? B->cols : B->rows),
                      "diagonal_matrix_rmult", GAN_ERROR_INCOMPATIBLE, "" );

   if ( A_inv )
      /* check for division by zero */
      for ( i = 0; i < A->size; i++ )
         if ( A->data[i] == 0.0 )
         {
            gan_err_flush_trace();
            gan_err_register ( "diagonal_matrix_rmult",
                               GAN_ERROR_DIVISION_BY_ZERO, "" );
            return NULL;
         }

   /* allocate result matrix C if necessary */
   if ( C == NULL )
      C = gan_mat_alloc ( A->size, Ccols );
   else
      C = gan_mat_set_dims ( C, A->size, Ccols );

   if ( C == NULL )
   {
      gan_err_register ( "diagonal_matrix_rmult", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   if ( B_tr )
   {
      /* can't do in-place multiply when the matrix is transposed */
      assert ( B != C );
      if ( A_inv )
         /* multiply matrix data C = A^-1*B^T */
         for ( j = 0, Cptr = C->data; j < C->cols; j++ )
            for ( i = 0; i < C->rows; i++ )
               *Cptr++ = B->data[i*B->rows+j]/A->data[i];
      else
         /* multiply matrix data C = A*B^T */
         for ( j = 0, Cptr = C->data; j < C->cols; j++ )
            for ( i = 0; i < C->rows; i++ )
               *Cptr++ = B->data[i*B->rows+j]*A->data[i];
   }
   else
   {
      double *Bptr;

      if ( A_inv )
         if ( B == C )
            /* multiply matrix data B <-- A^-1*B */
            for ( j = 0, Cptr = C->data; j < C->cols; j++ )
               for ( i = 0; i < C->rows; i++ )
                  *Cptr++ /= A->data[i];
         else
            /* multiply matrix data C = A^-1*B */
            for ( j = 0, Cptr = C->data, Bptr = B->data; j < C->cols; j++ )
               for ( i = 0; i < C->rows; i++ )
                  *Cptr++ = *Bptr++ / A->data[i];
      else
         if ( B == C )
            /* multiply matrix data B <-- A*B */
            for ( j = 0, Cptr = C->data; j < C->cols; j++ )
               for ( i = 0; i < C->rows; i++ )
                  *Cptr++ *= A->data[i];
         else
            /* multiply matrix data C = A*B */
            for ( j = 0, Cptr = C->data, Bptr = B->data; j < C->cols; j++ )
               for ( i = 0; i < C->rows; i++ )
                  *Cptr++ = *Bptr++ * A->data[i];
   }

   /* return result matrix */
   return C;
}

/* function to multipy diagonal matrix A and generic matrix B: C = B(^T)*A,
 * returning C
 */
static Gan_Matrix *
 diagonal_matrix_lmult (
     const Gan_SquMatrix *A, Gan_TposeFlag A_tr, Gan_InvertFlag A_inv,
     Gan_Matrix          *B, Gan_TposeFlag B_tr, Gan_Matrix *C )
{
   unsigned long Crows = B_tr ? B->cols : B->rows;
   unsigned long i, j;
   double *Cptr;

   /* consistency check */
   gan_err_test_ptr ( A->type == GAN_DIAGONAL_MATRIX, "diagonal_matrix_lmult",
                      GAN_ERROR_ILLEGAL_TYPE, "" );

   /* check that the dimensions of input matrices A,B,C are compatible */
   gan_err_test_ptr ( A->size == (B_tr ? B->rows : B->cols),
                      "diagonal_matrix_lmult", GAN_ERROR_INCOMPATIBLE, "" );

   if ( A_inv )
      /* check for division by zero */
      for ( i = 0; i < A->size; i++ )
         if ( A->data[i] == 0.0 )
         {
            gan_err_flush_trace();
            gan_err_register ( "diagonal_matrix_lmult",
                               GAN_ERROR_DIVISION_BY_ZERO, "" );
            return NULL;
         }

   /* allocate result matrix C if necessary */
   if ( C == NULL )
      C = gan_mat_alloc ( Crows, A->size );
   else
      C = gan_mat_set_dims ( C, Crows, A->size );

   if ( C == NULL )
   {
      gan_err_register ( "diagonal_matrix_lmult", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   if ( B_tr )
   {
      /* can't do in-place multiply when the matrix is transposed */
      gan_err_test_ptr ( B != C, "diagonal_matrix_lmult",
                         GAN_ERROR_INPLACE_TRANSPOSE, "" );
      if ( A_inv )
         /* multiply matrix data C = B^T*A^-1 */
         for ( j = 0, Cptr = C->data; j < C->cols; j++ )
            for ( i = 0; i < C->rows; i++ )
               *Cptr++ = B->data[i*B->rows+j]/A->data[j];
      else
         /* multiply matrix data C = B^T*A */
         for ( j = 0, Cptr = C->data; j < C->cols; j++ )
            for ( i = 0; i < C->rows; i++ )
               *Cptr++ = B->data[i*B->rows+j]*A->data[j];
   }
   else
   {
      double *Bptr;

      if ( A_inv )
         if ( B == C )
            /* multiply matrix data B <-- B*A^-1 */
            for ( j = 0, Cptr = C->data; j < C->cols; j++ )
               for ( i = 0; i < C->rows; i++ )
                  *Cptr++ /= A->data[j];
         else
            /* multiply matrix data C = B*A^-1 */
            for ( j = 0, Cptr = C->data, Bptr = B->data; j < C->cols; j++ )
               for ( i = 0; i < C->rows; i++ )
                  *Cptr++ = *Bptr++ / A->data[j];
      else
         if ( B == C )
            /* multiply matrix data B <-- B*A */
            for ( j = 0, Cptr = C->data; j < C->cols; j++ )
               for ( i = 0; i < C->rows; i++ )
                  *Cptr++ *= A->data[j];
         else
            /* multiply matrix data C = B*A */
            for ( j = 0, Cptr = C->data, Bptr = B->data; j < C->cols; j++ )
               for ( i = 0; i < C->rows; i++ )
                  *Cptr++ = *Bptr++ * A->data[j];
   }

   /* return result matrix */
   return C;
}

/* function to multipy diagonal matrix A and square matrix B:
 * C = A*B(^-1)(^T), returning C
 */
static Gan_SquMatrix *
 diagonal_matrix_rmult_squ (
     Gan_SquMatrix *A, Gan_TposeFlag A_tr, Gan_InvertFlag A_inv,
     Gan_SquMatrix *B, Gan_TposeFlag B_tr, Gan_InvertFlag B_inv,
     Gan_SquMatrix *C )
{
   long i;

   /* consistency check */
   gan_err_test_ptr ( A->type == GAN_DIAGONAL_MATRIX,
                      "diagonal_matrix_rmult_squ", GAN_ERROR_ILLEGAL_ARGUMENT,
                      "" );

   /* set result matrix to diagonal type */
   if ( C == NULL )
      C = gan_diagmat_alloc ( A->size );
   else
      C = gan_diagmat_set_size ( C, A->size );

   if ( C == NULL )
   {
      gan_err_register ( "diagonal_matrix_rmult_squ", GAN_ERROR_FAILURE, "" );
      return NULL;
   }
   
   /* check for self-multiplication with transpose */
   if ( A == B && A_inv == B_inv )
   {
      if ( A != C )
         /* copy A into C */
         if ( gan_squmat_copy_q ( A, C ) == NULL )
         {
            gan_err_register ( "diagonal_matrix_rmult_squ", GAN_ERROR_FAILURE,
                               "" );
            return NULL;
         }

      if ( A_inv )
      {
         /* compute A^-T * A^-1 */
         for ( i = C->size-1; i >= 0; i-- )
         {
            if ( C->data[i] == 0.0 )
            {
               gan_err_flush_trace();
               gan_err_register ( "diagonal_matrix_rmult_squ",
                                  GAN_ERROR_DIVISION_BY_ZERO, "" );
               return NULL;
            }

            C->data[i] = 1.0/gan_sqr(C->data[i]);
         }
      }
      else
         /* compute A^T * A */
         for ( i = C->size-1; i >= 0; i-- )
            C->data[i] = gan_sqr(C->data[i]);

      return C;
   }

   gan_err_test_ptr ( 0, "diagonal_matrix_rmult_squ",
                      GAN_ERROR_NOT_IMPLEMENTED, "" );
   return NULL;
}

/* function to multipy diagonal matrix A on the left my generic matrix B
 * or B^T and on the right by B^T or B, filling in symmetric matrix D with the
 * result. Generic matrix C is used to store the intermediate result A*B(^T).
 * The result matrix D is returned.
 */
static Gan_SquMatrix *
 diagonal_matrix_lrmult ( const Gan_SquMatrix *A, const Gan_Matrix *B, Gan_TposeFlag B_tr,
                          Gan_Matrix *C, Gan_SquMatrix *D )
{
   unsigned long Dsize = B_tr ? B->cols : B->rows;
   Gan_Bool Calloc = GAN_FALSE;

   /* consistency check */
   assert ( A->type == GAN_DIAGONAL_MATRIX );

   /* check that the dimensions of input matrices A,B are compatible */
   gan_err_test_ptr ( A->size == (B_tr ? B->rows : B->cols),
                      "diagonal_matrix_lrmult", GAN_ERROR_INCOMPATIBLE, "" );

   /* allocate intermediate result matrix C if necessary (to be freed at end)*/
   if ( C == NULL )
   {
      C = gan_mat_alloc ( A->size, Dsize );
      Calloc = GAN_TRUE;
   }
   else
      C = gan_mat_set_dims ( C, A->size, Dsize );

   /* allocate result matrix D if necessary */
   if ( D == NULL )
      D = gan_symmat_alloc ( Dsize );
   else
   {
      /* can't do in-place matrix multiply */
      gan_err_test_ptr ( B != C, "diagonal_matrix_lrmult",
                         GAN_ERROR_INCOMPATIBLE, "" );
      D = gan_symmat_set_size ( D, Dsize );
   }

   /* perform first multiplication step A*B(^T) */
   A->funcs->rmult ( A, GAN_NOTRANSPOSE, GAN_NOINVERT,
                     (Gan_Matrix *)B, B_tr ? GAN_NOTRANSPOSE : GAN_TRANSPOSE, C );

   /* perform second multiplication step B*(A*B^T) or B^T*(A*B) */
   gan_mat_rmult_sym_gen ( B, B_tr, C, GAN_NOTRANSPOSE, D );

   /* free intermediate result matrix C if it was allocated */
   if ( Calloc )
      gan_mat_free ( C );

   /* return result matrix */
   return D;
}

/* perform inversion of diagonal matrix A into another diagonal matrix B.
 */
static Gan_SquMatrix *
 diagonal_matrix_invert ( Gan_SquMatrix *A, Gan_SquMatrix *B )
{
   long i;

   gan_err_test_ptr ( A->type == GAN_DIAGONAL_MATRIX,
                      "diagonal_matrix_invert", GAN_ERROR_ILLEGAL_TYPE, "" );

   /* allocate matrix B if necessary */
   if ( B == NULL )
      B = gan_diagmat_alloc ( A->size );
   else
      B = gan_diagmat_set_size ( B, A->size );

   /* for inversion we need the reciprocal of each element */
   for ( i = A->size-1; i >= 0; i-- )
   {
      if ( A->data[i] == 0.0 )
      {
         gan_err_flush_trace();
         gan_err_register ( "diagonal_matrix_invert",
                            GAN_ERROR_DIVISION_BY_ZERO, "" );
         return NULL;
      }

      B->data[i] = 1.0/A->data[i];
   }

   return B;
}

/* perform Cholesky factorisation of diagonal matrix A into another
 * diagonal matrix B.
 */
static Gan_SquMatrix *
 diagonal_matrix_cholesky ( Gan_SquMatrix *A, Gan_SquMatrix *B,
                            int *error_code )
{
   long i;

   gan_err_test_ptr ( A->type == GAN_DIAGONAL_MATRIX,
                      "diagonal_matrix_cholesky", GAN_ERROR_ILLEGAL_TYPE, "" );

   /* allocate matrix B if necessary */
   if ( B == NULL )
      B = gan_diagmat_alloc ( A->size );
   else
      B = gan_diagmat_set_size ( B, A->size );

   /* Cholesky factorisation is just the square-root of each element */
   for ( i = A->size-1; i >= 0; i-- )
   {
      if ( A->data[i] < 0.0 )
      {
         if ( error_code == NULL )
         {
            gan_err_flush_trace();
            gan_err_register ( "diagonal_matrix_cholesky",
                               GAN_ERROR_NOT_POSITIVE_DEFINITE, "" );
         }
         else
            *error_code = GAN_ERROR_NOT_POSITIVE_DEFINITE;

         return NULL;
      }

      B->data[i] = sqrt(A->data[i]);
   }

   return B;
}

/* Compute squared Frobenius norm of diagonal matrix A.
 */
static double
 diagonal_matrix_sumsqr ( const Gan_SquMatrix *A )
{
   long i;
   double total = 0.0;

   /* consistency check */
   assert ( A->type == GAN_DIAGONAL_MATRIX );

   for ( i = (long)A->size-1; i >= 0; i-- )
      total += gan_sqr(A->data[i]);

   return total;
}

/* Compute Frobenius norm of diagonal matrix A.
 */
static double
 diagonal_matrix_Fnorm ( const Gan_SquMatrix *A )
{
   long i;
   double total = 0.0;

   /* consistency check */
   assert ( A->type == GAN_DIAGONAL_MATRIX );

   for ( i = (long)A->size-1; i >= 0; i-- )
      total += gan_sqr(A->data[i]);

   return sqrt(total);
}

/* Convert diagonal matrix to generic matrix */
static Gan_Matrix *
 diagonal_matrix_matrix ( const Gan_SquMatrix *A, struct Gan_Matrix *B )
{
   long i;

   /* consistency check */
   assert ( A->type == GAN_DIAGONAL_MATRIX );

   /* allocate/set result matrix B */
   if ( B == NULL )
      B = gan_mat_alloc ( A->size, A->size );
   else
      B = gan_mat_set_dims ( B, A->size, A->size );

   if ( B == NULL )
   {
      gan_err_register ( "diagonal_matrix_matrix", GAN_ERROR_FAILURE, "" );
      return NULL;
   }
   
   /* fill with zeros */
   gan_mat_fill_const_q ( B, A->size, A->size, 0.0 );

   /* now fill diagonal */
   for ( i = A->size-1; i >= 0; i-- )
      B->data[i*(A->size+1)] = A->data[i];

   return B;
}

static Gan_SquMatrixFuncs funcs =
{
   diagonal_matrix_set_el,
   diagonal_matrix_get_el,
   diagonal_matrix_inc_el,
   diagonal_matrix_dec_el,
   diagonal_matrix_fprint,
   diagonal_matrix_copy,
   diagonal_matrix_scale,
   diagonal_matrix_add,
   diagonal_matrix_sub,
   diagonal_matrix_multv,
   diagonal_matrix_rmult,
   diagonal_matrix_lmult,
   diagonal_matrix_rmult_squ,
   diagonal_matrix_lrmult,
   diagonal_matrix_invert,
   diagonal_matrix_cholesky,
   diagonal_matrix_sumsqr,
   diagonal_matrix_Fnorm,
   diagonal_matrix_matrix
};

/**
 * \addtogroup GeneralSizeMatrixSet
 * \{
 */

/**
 * \brief Set square matrix to be diagonal with given \a size.
 * \return Result matrix \a A.
 */
Gan_SquMatrix *
 gan_diagmat_set_size ( Gan_SquMatrix *A, unsigned long size )
{
   /* set type of matrix */
   A->type = GAN_DIAGONAL_MATRIX;

   /* check whether matrix is big enough */
   if ( A->data_size < size )
   {
      /* re-allocate matrix data */
      gan_err_test_ptr ( A->data_alloc, "gan_diagmat_set_size",
                         GAN_ERROR_INCOMPATIBLE, "" );
      A->data_size = size;
      A->data = gan_realloc_array ( double, A->data, A->data_size );
      if ( A->data == NULL )
      {
         gan_err_flush_trace();
         gan_err_register_with_number ( "gan_diagmat_set_size", GAN_ERROR_MALLOC_FAILED, "", A->data_size*sizeof(double) );
         return NULL;
      }
   }

   /* set matrix dimensions */
   A->size = size;

   /* set pointer to structure holding type-specific functions */
   A->funcs = &funcs;

   return A;
}

/**
 * \}
 */

/**
 * \addtogroup GeneralSizeMatrixFill
 * \{
 */

/**
 * \brief Fill diagonal matrix from variable argument list.
 * \return Result matrix \a A, or \c NULL on failure.
 *
 * Fill diagonal matrix \a A with values from variable argument list,
 * and set the size of \a A to \a size.
 */
Gan_SquMatrix *
 gan_diagmat_fill_va ( Gan_SquMatrix *A, unsigned long size, ... )
{
   unsigned int i;
   va_list ap;

   if ( A == NULL )
      A = gan_diagmat_alloc ( size );
   else
      A = gan_diagmat_set_size ( A, size );

   if ( A == NULL )
   {
      gan_err_register ( "gan_diagmat_fill_va", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* read variable argument list of diagonal elements */
   va_start ( ap, size );
   for ( i = 0; i < size; i++ )
      A->data[i] = va_arg ( ap, double );

   va_end ( ap );
   return A;
}

/**
 * \brief Fill a diagonal matrix with constant value.
 * \return Filled matrix \a A.
 *
 * Set matrix \a A to be diagonal with given \a size, and fill with
 * given \a value.
 */
Gan_SquMatrix *
 gan_diagmat_fill_const_q ( Gan_SquMatrix *A, unsigned long size,
                            double value )
{
   int i;

   if ( A == NULL )
      A = gan_diagmat_alloc ( size );
   else
      A = gan_diagmat_set_size ( A, size );

   if ( A == NULL )
   {
      gan_err_register ( "diagonal_matrix_fill_const_q", GAN_ERROR_FAILURE,
                         "" );
      return NULL;
   }

   /* fill matrix with constant */
   for ( i = (int)size-1; i >= 0; i-- )
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
 * \brief Read the elements of a diagonal matrix into pointers.
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Reads all the diagonal element values of the given diagonal matrix \a A
 * into the variable argument list of pointers to \c double.
 * The \a size argument must match the size of the matrix.
 */
Gan_Bool
 gan_diagmat_read_va ( const Gan_SquMatrix *A, unsigned long size, ... )
{
   unsigned int i;
   va_list ap;

   /* check that type & dimensions match */
   gan_err_test_bool ( A->type == GAN_DIAGONAL_MATRIX && size == A->size,
                       "gan_diagmat_read_va", GAN_ERROR_INCOMPATIBLE, "" );

   /* fill variable argument list of diagonal matrix element pointers */
   va_start ( ap, size );
   for ( i = 0; i < size; i++ )
      *(va_arg ( ap, double * )) = A->data[i];

   va_end ( ap );

   /* return with success */
   return GAN_TRUE;
}

/**
 * \}
 */

/**
 * \addtogroup GeneralSizeMatrixThreshold
 * \{
 */

/**
 * \brief Threshold small diagonal elements of a diagonal matrix.
 * \param A Diagonal matrix
 * \param null_space Size of null-space in \a A to allow for
 * \param thres Threshold on elements of \a A
 * \param indexptr Pointer to index of last unaffected element of \a A
 * \param elptr Pointer to last unaffected element of \a A
 *
 * Diagonal elements below the threshold \a thres times the first are set
 * to zero. In addition any redundant diagonal elements (indicated by
 * the value of \a null_space) are also set to zero.
 * This can be used to eliminate conditioning problems when using the
 * SVD algorithm gan_mat_svd() for least-squares. The diagonal elements
 * must be in monotonically decreasing order from the top-left corner of
 * the matrix.
 *
 * The array \a x of state vectors specifies any redundant parameters
 * in each block.
 *
 * The contents of \a indexptr and \a elptr, if not \c NULL, are set
 * respectively to the index of and a pointer to the last diagonal element
 * not set to the high value.
 */
Gan_SquMatrix *
 gan_diagmat_diag_thres ( Gan_SquMatrix *A, unsigned null_space,
                          double thres, int *indexptr, double **elptr )
{
   int i, nparams;
   double *Adata;

   /* consistency check */
   assert ( A->type == GAN_DIAGONAL_MATRIX );

   if ( A->size == 0 )
   {
      if ( indexptr != NULL ) *indexptr = -1;
      if ( elptr   != NULL ) *elptr = NULL;
      return A;
   }

   nparams = A->size - null_space;
   assert ( nparams >= 0 );

   Adata = &A->data[0];
   thres *= Adata[0];

   for ( i = A->size-1; i >= nparams; i-- )
      /*if ( Adata[i] <= thres )*/
      Adata[i] = 0.0;

   for ( ; i >= 0; i-- )
   {
      if ( Adata[i] > thres ) break;

      Adata[i] = 0.0;
   }

   if ( indexptr != NULL ) *indexptr = i;
   if ( elptr    != NULL ) *elptr    = &Adata[i];
   return A;
}

/**
 * \}
 */

/**
 * \addtogroup GeneralSizeMatrixInvert
 * \{
 */

/**
 * \brief Invert the elements of diagonal matrix.
 * \return The matrix \a A, or \c NULL on failure.
 *
 * The bottom-right \a null_space elements of \a A are set to zero,
 * as well as any other elements below the \a thres threshold value.
 * All other elements of \a A are inverted.
 * The elements of diagonal matrix \a A are assumed to be positive and
 * in decreasing order from the top-left.
 */
Gan_SquMatrix *
 gan_diagmat_inv_diag_thres ( Gan_SquMatrix *A, unsigned null_space,
                                  double thres )
{
   int i, nparams;
   double *Adata;

   if ( A->size == 0 )
      return A;

   nparams = A->size - null_space;
   assert ( A->type == GAN_DIAGONAL_MATRIX && nparams >= 0 );

   Adata = &A->data[0];
   thres *= Adata[0];

   for ( i = A->size-1; i >= nparams; i-- )
      Adata[i] = 0.0;

   for ( ; i >= 0; i-- )
   {
      if ( Adata[i] > thres ) break;

      Adata[i] = 0.0;
   }

   /* invert non-zero elements of A */
   for ( ; i >= 0; i-- )
      Adata[i] = 1.0/Adata[i];
        
   return A;
}

/**
 * \}
 */

/**
 * \addtogroup GeneralSizeMatrixMatrixProduct
 * \{
 */

/**
 * \brief Left-multiply the inverse of a diagonal matrix by a generic matrix.
 * \return The result matrix \a C, or \c NULL on failure.
 *
 * Left-multiply the inverse of a diagonal matrix \a A by a generic matrix
 * \a B, ignoring division by zero. The result is written into \a C.
 * The zero elements of \a A are ignored, i.e. effectively assumed to
 * be infinite.
 *
 * \a B and \a C may be coincident, allowing the computation to be done
 * in-place.
 */
Gan_Matrix *
 gan_diagmatI_lmult_inf_q ( const Gan_SquMatrix *A, Gan_Matrix *B, Gan_Matrix *C )
{
   unsigned long i, j;
   double *Aptr = A->data, *Bptr = B->data, *Cptr = C->data;

   /* consistency check */
   assert ( A->type == GAN_DIAGONAL_MATRIX );

   if ( B == C )
      for ( j = 0; j < B->cols; j++, Aptr++ )
         if ( *Aptr == 0.0 )
            for ( i = 0; i < B->rows; i++ )
               *Cptr++ = 0.0;
         else
            for ( i = 0; i < B->rows; i++ )
               *Cptr++ /= *Aptr;
   else
      for ( j = 0; j < B->cols; j++, Aptr++ )
         if ( *Aptr == 0.0 )
            for ( i = 0; i < B->rows; i++, Bptr++ )
               *Cptr++ = 0.0;
         else
            for ( i = 0; i < B->rows; i++ )
               *Cptr++ = *Bptr++ / *Aptr;

   return C;
}

/**
 * \}
 */

/**
 * \defgroup GeneralSizeMatrixAllocate Allocate/Free a General Size Matrix
 * \{
 */

/* not a user function */
Gan_SquMatrix *
 gan_diagmat_form_gen ( Gan_SquMatrix *A, unsigned long size,
                        double *data, size_t data_size )
{
   if ( A == NULL )
   {
      /* dyamically allocate matrix */
      A = gan_malloc_object(Gan_SquMatrix);

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
         A->data_size = size;
      else
      {
         /* a non-zero data size has been requested */
         assert ( data_size >= size );
         A->data_size = data_size;
      }

      /* allocate matrix data */
      if ( A->data_size == 0 ) A->data = NULL;
      else
      {
         A->data = gan_malloc_array ( double, A->data_size );
         if ( A->data == NULL )
         {
            gan_err_flush_trace();
            gan_err_register_with_number ( "gan_diagmat_form_gen", GAN_ERROR_MALLOC_FAILED, "", A->data_size*sizeof(double) );
            return NULL;
         }
      }

      A->data_alloc = GAN_TRUE;
   }
   else
   {
      /* data for this matrix is provided */
      gan_err_test_ptr ( data_size >= size, "gan_diagmat_form_gen",
                         GAN_ERROR_INCOMPATIBLE, "" );

      /* set data pointer in matrix */
      A->data = data;
      A->data_size = data_size;

      /* we shouldn't free the matrix data */
      A->data_alloc = GAN_FALSE;
   }

   /* set type and class member functions */
   gan_diagmat_set_size ( A, size );

   return A;
}

/**
 * \}
 */

/**
 * \addtogroup GeneralSizeMatrixFill
 * \{
 */

/**
 * \brief Fill diagonal matrix with values from variable argument list.
 *
 * Fill diagonal matrix \a A with values from variable argument list
 * \a aptr, and set the size of \a A to \a size.
 */
Gan_SquMatrix *
 gan_diagmat_fill_vap ( Gan_SquMatrix *A, unsigned long size, va_list *aptr )
{
   unsigned int i;

   A = gan_diagmat_set_size ( A, size );
   if ( A == NULL )
   {
      gan_err_register ( "gan_diagmat_fill_vap", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* read variable argument list of diagonal elements */
   for ( i = 0; i < size; i++ )
      A->data[i] = va_arg ( *aptr, double );

   return A;
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

/**
 * \}
 */

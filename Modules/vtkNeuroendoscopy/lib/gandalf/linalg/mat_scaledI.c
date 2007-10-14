/**
 * File:          $RCSfile: mat_scaledI.c,v $
 * Module:        Scaled identity matrices (double precision)
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
#include <gandalf/linalg/mat_scaledI.h>
#include <gandalf/linalg/mat_symmetric.h>
#include <gandalf/linalg/mat_gen.h>
#include <gandalf/linalg/vec_gen.h>
#include <gandalf/linalg/clapack.h>
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

/* function to set element i,j of multiple of identity */
static Gan_Bool
 scaledI_matrix_set_el ( Gan_SquMatrix *A,
                         unsigned i, unsigned j, double value )
{
   gan_assert ( 0, "not defined in scaledI_matrix_set_el()" );
   return GAN_FALSE;
}

/* function to return element i,j of multiple of identity */
static double
 scaledI_matrix_get_el ( const Gan_SquMatrix *A, unsigned i, unsigned j )
{
   /* consistency check */
   assert ( A->type == GAN_SCALED_IDENT_MATRIX );

   /* make sure element is inside matrix */
   assert ( i < A->size && j < A->size );

   if ( i == j )
      /* get diagonal value */
      return A->data[0];

   /* all off-diagonal elements are zero */
   return 0.0;
}

/* function to increment element i,j of multiple of identity by given value */
static Gan_Bool
 scaledI_matrix_inc_el ( Gan_SquMatrix *A,
                         unsigned i, unsigned j, double value )
{
   gan_assert ( 0, "not defined in scaledI_matrix_inc_el()" );
   return GAN_FALSE;
}

/* function to decrement element i,j of multiple of identity by given value */
static Gan_Bool
 scaledI_matrix_dec_el ( Gan_SquMatrix *A,
                         unsigned i, unsigned j, double value )
{
   gan_assert ( 0, "not defined in scaledI_matrix_dec_el()" );
   return GAN_FALSE;
}

/* function to print multiple of identity to file pointer */
static Gan_Bool
 scaledI_matrix_fprint ( FILE *fp, const Gan_SquMatrix *A, const char *prefix,
                         unsigned indent, const char *fmt )
{
   unsigned long i, j;
   int p;

   /* consistency check */
   assert ( A->type == GAN_SCALED_IDENT_MATRIX );

   /* print indentation on first line */
   for ( p = indent-1; p >= 0; p-- ) fprintf ( fp, " " );
   fprintf ( fp, "%s (%0ldx%0ld scaled identity matrix)\n", prefix,
             A->size, A->size );

   for ( i = 0; i < A->size; i++ )
   {
      for ( p = indent-1; p >= 0; p-- ) fprintf ( fp, " " );
      for ( j = 0; j < A->size; j++ )
      {
         fprintf ( fp, " " );
         if ( j == i )
            fprintf ( fp, fmt, A->data[0] );
         else
            fprintf ( fp, fmt, 0.0 );
      }

      fprintf ( fp, "\n" );
   }

   return GAN_TRUE;
}

/* function to copy multiple of identity A into another matrix B */
static Gan_SquMatrix *
 scaledI_matrix_copy ( const Gan_SquMatrix *A, Gan_SquMatrix *B )
{
   /* consistency check */
   assert ( A->type == GAN_SCALED_IDENT_MATRIX );

   /* allocate matrix B if necessary */
   if ( B == NULL )
      B = gan_scalImat_alloc ( A->size );
   else
      B = gan_scalImat_set_size ( B, A->size );

   /* copy matrix */
   B->data[0] = A->data[0];

   return B;
}

/* function to rescale multiple of identity A into another matrix B */
static Gan_SquMatrix *
 scaledI_matrix_scale ( Gan_SquMatrix *A, double a, Gan_SquMatrix *B )
{
   /* consistency check */
   assert ( A->type == GAN_SCALED_IDENT_MATRIX );

   /* allocate matrix B if necessary */
   if ( B == NULL )
      B = gan_scalImat_alloc ( A->size );
   else
      B = gan_scalImat_set_size ( B, A->size );

   /* copy and scale matrix */
   B->data[0] = a*A->data[0];

   return B;
}

/* function to add multiple of identity matrices together C = A+B,
 * returning C */
static Gan_SquMatrix *
 scaledI_matrix_add ( Gan_SquMatrix *A, Gan_TposeFlag A_tr,
                      Gan_SquMatrix *B, Gan_TposeFlag B_tr, Gan_SquMatrix *C )
{
   /* consistency check */
   assert ( A->type == GAN_SCALED_IDENT_MATRIX );

   /* consistency check */
   assert ( B->type == GAN_SCALED_IDENT_MATRIX );

   /* check that the type and dimensions of input matrices A & B are
      compatible */
   gan_assert ( gan_squmat_same_type_size(A,B),
                "incompatible matrices in gan_scalImat_add()" );

   /* allocate result matrix C if necessary */
   if ( C == NULL )
      C = gan_scalImat_alloc ( A->size );
   else
      C = gan_scalImat_set_size ( C, A->size );

   /* add matrix data */
   C->data[0] = A->data[0] + B->data[0];

   /* return result matrix */
   return C;
}

/* function to subtract multiple of identity matrices from each other: C = A-B,
 * returning C
 */
static Gan_SquMatrix *
 scaledI_matrix_sub ( Gan_SquMatrix *A, Gan_TposeFlag A_tr,
                      Gan_SquMatrix *B, Gan_TposeFlag B_tr, Gan_SquMatrix *C )
{
   /* consistency check */
   assert ( A->type == GAN_SCALED_IDENT_MATRIX );

   /* consistency check */
   assert ( B->type == GAN_SCALED_IDENT_MATRIX );

   /* check that the type and dimensions of input matrices A & B are
      compatible */
   gan_assert ( gan_squmat_same_size(A,B),
                "incompatible matrices in scaledI_matrix_sub()" );

   /* allocate result matrix C if necessary */
   if ( C == NULL )
      C = gan_scalImat_alloc ( A->size );
   else
      C = gan_scalImat_set_size ( C, A->size );

   /* subtract matrix data */
   C->data[0] = A->data[0] - B->data[0];

   /* return result matrix */
   return C;
}

/* function to compute matrix/vector product A*x = y, returning y */
static Gan_Vector *
 scaledI_matrix_multv (
     const Gan_SquMatrix *A, Gan_TposeFlag A_tr, Gan_InvertFlag A_inv,
     Gan_Vector *x, Gan_Vector *y )
{
   long i;
   double Aval = A->data[0];

   /* consistency check */
   assert ( A->type == GAN_SCALED_IDENT_MATRIX );

   /* check that the dimensions of input matrix A and vector x are
      compatible */
   gan_assert ( A->size == x->rows,
                "incompatible matrix/vector in scaledI_matrix_multv()" );

   /* allocate result vector y if necessary */
   if ( y == NULL )
      y = gan_vec_alloc ( A->size );
   else
      y = gan_vec_set_size ( y, A->size );

   if ( x == y )
      if ( A_inv )
      {
         /* compute matrix/vector product x <-- A^-1*x */
         gan_assert ( Aval != 0.0,
                      "division by zero in scaledI_matrix_multv()" );
         for ( i = A->size-1; i >= 0; i-- )
            y->data[i] /= Aval;
      }
      else
         /* compute matrix/vector product x <-- A*x */
         for ( i = A->size-1; i >= 0; i-- )
            y->data[i] *= Aval;
   else
      if ( A_inv )
      {
         /* compute matrix/vector product y = A^-1*x */
         gan_assert ( Aval != 0.0,
                      "division by zero in scaledI_matrix_multv()" );
         for ( i = A->size-1; i >= 0; i-- )
            y->data[i] = x->data[i]/Aval;
      }
      else
         /* compute matrix/vector product y = A*x */
         for ( i = A->size-1; i >= 0; i-- )
            y->data[i] = x->data[i]*Aval;

   /* return result vector */
   return y;
}

/* function to multipy multiple of identity A and generic matrix B:
 * C = A*B(^T), returning C
 */
static Gan_Matrix *
 scaledI_matrix_rmult (
     const Gan_SquMatrix *A, Gan_TposeFlag A_tr, Gan_InvertFlag A_inv,
     Gan_Matrix    *B, Gan_TposeFlag B_tr, Gan_Matrix *C )
{
   unsigned long Ccols = B_tr ? B->rows : B->cols;
   unsigned long i, j;
   double Aval = A->data[0], *Cptr;

   /* consistency check */
   gan_err_test_ptr ( A->type == GAN_SCALED_IDENT_MATRIX,
                      "scaledI_matrix_rmult", GAN_ERROR_ILLEGAL_ARGUMENT, "" );

   /* check that the dimensions of input matrices A,B,C are compatible */
   gan_err_test_ptr ( A->size == (B_tr ? B->cols : B->rows),
                      "scaledI_matrix_rmult", GAN_ERROR_INCOMPATIBLE, "" );

   /* allocate result matrix C if necessary */
   if ( C == NULL )
      C = gan_mat_alloc ( A->size, Ccols );
   else
      C = gan_mat_set_dims ( C, A->size, Ccols );

   if ( C == NULL )
   {
      gan_err_register ( "scaledI_matrix_rmult", GAN_ERROR_FAILURE, "" );
      return NULL;
   }
   
   if ( B_tr )
   {
      /* can't do in-place multiply when the matrix is transposed */
      gan_err_test_ptr ( B != C, "scaledI_matrix_lmult",
                         GAN_ERROR_ILLEGAL_ARGUMENT, "" );
      if ( A_inv )
      {
         /* multiply matrix data C = A^-1*B */
         gan_err_test_ptr ( Aval != 0.0, "scaledI_matrix_rmult",
                            GAN_ERROR_DIVISION_BY_ZERO, "" );
         for ( j = 0, Cptr = C->data; j < C->cols; j++ )
            for ( i = 0; i < C->rows; i++ )
               *Cptr++ = B->data[i*B->rows+j]/Aval;
      }
      else
         /* multiply matrix data C = A*B */
         for ( j = 0, Cptr = C->data; j < C->cols; j++ )
            for ( i = 0; i < C->rows; i++ )
               *Cptr++ = B->data[i*B->rows+j]*Aval;
   }
   else
   {
      double *Bptr;

      if ( A_inv )
         if ( B == C )
         {
            /* multiply matrix data B <-- A*B */
            gan_err_test_ptr ( Aval != 0.0, "scaledI_matrix_rmult",
                               GAN_ERROR_DIVISION_BY_ZERO, "" );
            for ( j = 0, Cptr = C->data; j < C->cols; j++ )
               for ( i = 0; i < C->rows; i++ )
                  *Cptr++ /= Aval;
         }
         else
         {
            /* multiply matrix data C = A*B */
            gan_err_test_ptr ( Aval != 0.0, "scaledI_matrix_rmult",
                               GAN_ERROR_DIVISION_BY_ZERO, "" );
            for ( j = 0, Cptr = C->data, Bptr = B->data; j < C->cols; j++ )
               for ( i = 0; i < C->rows; i++ )
                  *Cptr++ = *Bptr++ / Aval;
         }
      else
         if ( B == C )
            /* multiply matrix data B <-- A*B */
            for ( j = 0, Cptr = C->data; j < C->cols; j++ )
               for ( i = 0; i < C->rows; i++ )
                  *Cptr++ *= Aval;
         else
            /* multiply matrix data C = A*B */
            for ( j = 0, Cptr = C->data, Bptr = B->data; j < C->cols; j++ )
               for ( i = 0; i < C->rows; i++ )
                  *Cptr++ = *Bptr++ * Aval;
   }

   /* return result matrix */
   return C;
}

/* function to multipy multiple of identity A and generic matrix B:
 * C = B(^T)*A, returning C
 */
static Gan_Matrix *
 scaledI_matrix_lmult (
     const Gan_SquMatrix *A, Gan_TposeFlag A_tr, Gan_InvertFlag A_inv,
     Gan_Matrix          *B, Gan_TposeFlag B_tr, Gan_Matrix *C )
{
   unsigned long Crows = B_tr ? B->cols : B->rows;
   unsigned long i, j;
   double Aval = A->data[0], *Cptr;

   /* consistency check */
   gan_err_test_ptr ( A->type == GAN_SCALED_IDENT_MATRIX,
                      "scaledI_matrix_lmult", GAN_ERROR_ILLEGAL_ARGUMENT, "" );

   /* check that the dimensions of input matrices A,B,C are compatible */
   gan_err_test_ptr ( A->size == (B_tr ? B->rows : B->cols),
                      "scaledI_matrix_lmult", GAN_ERROR_INCOMPATIBLE, "" );

   /* allocate result matrix C if necessary */
   if ( C == NULL )
      C = gan_mat_alloc ( Crows, A->size );
   else
      C = gan_mat_set_dims ( C, Crows, A->size );

   if ( C == NULL )
   {
      gan_err_register ( "scaledI_matrix_lmult", GAN_ERROR_FAILURE, "" );
      return NULL;
   }
   
   if ( B_tr )
   {
      /* can't do in-place multiply when the matrix is transposed */
      gan_err_test_ptr ( B != C, "scaledI_matrix_lmult",
                         GAN_ERROR_ILLEGAL_ARGUMENT, "" );
      if ( A_inv )
      {
         /* multiply matrix data C = A^-1*B */
         gan_err_test_ptr ( Aval != 0.0, "scaledI_matrix_lmult",
                            GAN_ERROR_DIVISION_BY_ZERO, "" );
         for ( j = 0, Cptr = C->data; j < C->cols; j++ )
            for ( i = 0; i < C->rows; i++ )
               *Cptr++ = B->data[i*B->rows+j]/Aval;
      }
      else
         /* multiply matrix data C = A*B */
         for ( j = 0, Cptr = C->data; j < C->cols; j++ )
            for ( i = 0; i < C->rows; i++ )
               *Cptr++ = B->data[i*B->rows+j]*Aval;
   }
   else
   {
      double *Bptr;

      if ( A_inv )
         if ( B == C )
         {
            /* multiply matrix data B <-- A*B */
            gan_err_test_ptr ( Aval != 0.0, "scaledI_matrix_lmult",
                               GAN_ERROR_DIVISION_BY_ZERO, "" );
            for ( j = 0, Cptr = C->data; j < C->cols; j++ )
               for ( i = 0; i < C->rows; i++ )
                  *Cptr++ /= Aval;
         }
         else
         {
            /* multiply matrix data C = A*B */
            gan_err_test_ptr ( Aval != 0.0, "scaledI_matrix_lmult",
                               GAN_ERROR_DIVISION_BY_ZERO, "" );
            for ( j = 0, Cptr = C->data, Bptr = B->data; j < C->cols; j++ )
               for ( i = 0; i < C->rows; i++ )
                  *Cptr++ = *Bptr++ / Aval;
         }
      else
         if ( B == C )
            /* multiply matrix data B <-- A*B */
            for ( j = 0, Cptr = C->data; j < C->cols; j++ )
               for ( i = 0; i < C->rows; i++ )
                  *Cptr++ *= Aval;
         else
            /* multiply matrix data C = A*B */
            for ( j = 0, Cptr = C->data, Bptr = B->data; j < C->cols; j++ )
               for ( i = 0; i < C->rows; i++ )
                  *Cptr++ = *Bptr++ * Aval;
   }

   /* return result matrix */
   return C;
}

/* function to multipy multiple of identity A and square matrix B:
 * C = A*B(^-1)(^T), returning C
 */
static Gan_SquMatrix *
 scaledI_matrix_rmult_squ (
     Gan_SquMatrix *A, Gan_TposeFlag A_tr, Gan_InvertFlag A_inv,
     Gan_SquMatrix *B, Gan_TposeFlag B_tr, Gan_InvertFlag B_inv,
     Gan_SquMatrix *C )
{
   /* consistency check */
   gan_err_test_ptr ( A->type == GAN_SCALED_IDENT_MATRIX,
                      "scaledI_matrix_rmult_squ", GAN_ERROR_ILLEGAL_ARGUMENT,
                      "" );

   /* set result matrix to scaled identity type */
   if ( C == NULL )
      C = gan_scalImat_alloc ( A->size );
   else
      C = gan_scalImat_set_size ( C, A->size );

   if ( C == NULL )
   {
      gan_err_register ( "scaledI_matrix_rmult_squ", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   if ( C->size == 0 ) return C; /* no need for further action */
   
   /* check for self-multiplication with transpose */
   if ( A == B && A_inv == B_inv )
   {
      if ( A != C )
         /* copy A into C */
         if ( gan_squmat_copy_q ( A, C ) == NULL )
         {
            gan_err_register ( "scaledI_matrix_rmult_squ", GAN_ERROR_FAILURE,
                               "" );
            return NULL;
         }

      if ( A_inv )
      {
         /* compute A^-T * A^-1 */
         gan_err_test_ptr ( C->data[0] != 0.0, "scaledI_matrix_rmult_squ",
                            GAN_ERROR_DIVISION_BY_ZERO, "" );
         C->data[0] = 1.0/gan_sqr(C->data[0]);
      }
      else
         /* compute A^T * A */
         C->data[0] = gan_sqr(C->data[0]);

      return C;
   }

   gan_err_test_ptr ( 0, "scaledI_matrix_rmult_squ",
                      GAN_ERROR_NOT_IMPLEMENTED, "" );
   return NULL;
}

/* function to multipy multiple of identity A on the left my generic matrix B
 * or B^T and on the right by B^T or B, filling in symmetric matrix D with the
 * result. Generic matrix C is used to store the intermediate result A*B(^T).
 * The result matrix D is returned.
 */
static Gan_SquMatrix *
 scaledI_matrix_lrmult ( const Gan_SquMatrix *A, const Gan_Matrix *B, Gan_TposeFlag B_tr,
                         Gan_Matrix *C, Gan_SquMatrix *D )
{
   unsigned long Dsize = B_tr ? B->cols : B->rows;
   Gan_Bool Calloc = GAN_FALSE;

   /* consistency check */
   assert ( A->type == GAN_SCALED_IDENT_MATRIX );

   /* check that the dimensions of input matrices A,B are compatible */
   gan_assert ( A->size == (B_tr ? B->rows : B->cols),
                "incompatible input matrices in scaledI_matrix_lrmult()" );

   /* allocate intermediate result matrix C if necessary (to be freed at end)*/
   if ( C == NULL )
   {
      C = gan_mat_alloc ( A->size, Dsize );
      Calloc = GAN_TRUE;
   }
   else
   {
      /* can't do in-place matrix multiply */
      gan_assert ( B != C,
      "can't do matrix multiplication in-place in scaledI_matrix_lrmult()" );
      C = gan_mat_set_dims ( C, A->size, Dsize );
   }

   /* allocate result matrix D if necessary */
   if ( D == NULL )
      D = gan_symmat_alloc ( Dsize );
   else
      D = gan_symmat_set_size ( D, Dsize );

   /* perform first multiplication step A*B(^T) */
   A->funcs->rmult ( A, GAN_NOTRANSPOSE, GAN_NOINVERT,
                     (Gan_Matrix *)B, (Gan_TposeFlag) !B_tr, C );

   /* perform second multiplication step B*(A*B^T) or B^T*(A*B) */
   gan_mat_rmult_sym_gen ( B, B_tr, C, GAN_NOTRANSPOSE, D );

   /* free intermediate result matrix C if it was allocated */
   if ( Calloc )
      gan_mat_free ( C );

   /* return result matrix */
   return D;
}

/* perform inversion of multiple of identity A into another multiple of
 * identity B.
 */
static Gan_SquMatrix *
 scaledI_matrix_invert ( Gan_SquMatrix *A, Gan_SquMatrix *B )
{
   assert ( A->type == GAN_SCALED_IDENT_MATRIX );

   /* allocate matrix B if necessary */
   if ( B == NULL )
      B = gan_scalImat_alloc ( A->size );
   else
      /* set type and size of result matrix */
      B = gan_scalImat_set_size ( B, A->size );

   /* for inversion we need the reciprocal of the diagonal element */
   if ( A->data[0] == 0.0 )
   {
      gan_err_flush_trace();
      gan_err_register ( "scaledI_matrix_invert", GAN_ERROR_DIVISION_BY_ZERO,
                         "" );
      return NULL;
   }

   B->data[0] = 1.0/A->data[0];

   return B;
}

/* perform Cholesky factorisation of multiple of identity A into another
 * multiple of identity B.
 */
static Gan_SquMatrix *
 scaledI_matrix_cholesky ( Gan_SquMatrix *A, Gan_SquMatrix *B,
                           int *error_code )
{
   assert ( A->type == GAN_SCALED_IDENT_MATRIX );

   /* allocate matrix B if necessary */
   if ( B == NULL )
      B = gan_scalImat_alloc ( A->size );
   else
      /* set type and size of result matrix */
      B = gan_scalImat_set_size ( B, A->size );

   /* Cholesky factorisation is just the square-root of the diagonal element */
   if ( A->data[0] < 0.0 )
   {
      gan_err_flush_trace();
      gan_err_register ( "scalidI_matrix_cholesky_q",
                         GAN_ERROR_SQRT_OF_NEG_NUMBER, "" );
      return NULL;
   }

   B->data[0] = sqrt(A->data[0]);
   return B;
}

/* Compute squared Frobenius norm of multiple of identity matrix A.
 */
static double
 scaledI_matrix_sumsqr ( const Gan_SquMatrix *A )
{
   return ((double)A->size)*gan_sqr(A->data[0]);
}

/* Compute Frobenius norm of multiple of identity matrix A.
 */
static double
 scaledI_matrix_Fnorm ( const Gan_SquMatrix *A )
{
   return sqrt(((double)A->size)*gan_sqr(A->data[0]));
}

/* Convert multiple of identity to generic matrix */
static Gan_Matrix *
 scaledI_matrix_matrix ( const Gan_SquMatrix *A, struct Gan_Matrix *B )
{
   long i;
   double Aval = A->data[0];

   /* consistency check */
   assert ( A->type == GAN_SCALED_IDENT_MATRIX );

   /* allocate/set result matrix B */
   if ( B == NULL )
      B = gan_mat_alloc ( A->size, A->size );
   else
      B = gan_mat_set_dims ( B, A->size, A->size );

   if ( B == NULL )
   {
      gan_err_register ( "scaledI_matrix_matrix", GAN_ERROR_FAILURE, "" );
      return NULL;
   }
   
   /* fill with zeros */
   gan_mat_fill_const_q ( B, A->size, A->size, 0.0 );

   /* now fill diagonal element */
   for ( i = A->size-1; i >= 0; i-- )
      B->data[i*(A->size+1)] = Aval;

   return B;
}

static Gan_SquMatrixFuncs funcs =
{
   scaledI_matrix_set_el,
   scaledI_matrix_get_el,
   scaledI_matrix_inc_el,
   scaledI_matrix_dec_el,
   scaledI_matrix_fprint,
   scaledI_matrix_copy,
   scaledI_matrix_scale,
   scaledI_matrix_add,
   scaledI_matrix_sub,
   scaledI_matrix_multv,
   scaledI_matrix_rmult,
   scaledI_matrix_lmult,
   scaledI_matrix_rmult_squ,
   scaledI_matrix_lrmult,
   scaledI_matrix_invert,
   scaledI_matrix_cholesky,
   scaledI_matrix_sumsqr,
   scaledI_matrix_Fnorm,
   scaledI_matrix_matrix
};

/**
 * \addtogroup GeneralSizeMatrixSet
 * \{
 */

/**
 * \brief Set square matrix to be multiple of identity with given \a size.
 * \return Result matrix \a A.
 */
Gan_SquMatrix *
 gan_scalImat_set_size ( Gan_SquMatrix *A, unsigned long size )
{
   /* set type of matrix */
   A->type = GAN_SCALED_IDENT_MATRIX;

   /* check whether matrix is big enough */
   if ( A->data_size < 1 )
   {
      /* re-allocate matrix data */
      gan_assert ( A->data_alloc,
                   "cannot reallocate matrix in gan_scalImat_set_size()" );
      A->data_size = 1;
      A->data = gan_realloc_array ( double, A->data, A->data_size );
      if ( A->data == NULL )
      {
         gan_err_flush_trace();
         gan_err_register_with_number ( "gan_scalImat_set_size", GAN_ERROR_MALLOC_FAILED, "", A->data_size*sizeof(double) );
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
 * \brief Fill multiple of the identity matrix from variable argument list.
 * \return Result matrix \a A, or \c NULL on failure.
 *
 * Fill multiple of the identity matrix \a A with values from variable
 * argument list \a aptr, and set the size of \a A to \a size.
 */
Gan_SquMatrix *
 gan_scalImat_fill_va ( Gan_SquMatrix *A, unsigned long size, ... )
{
   va_list ap;

   if ( A == NULL )
      A = gan_scalImat_alloc ( size );
   else
      A = gan_scalImat_set_size ( A, size );

   if ( A == NULL )
   {
      gan_err_register ( "gan_scalImat_fill_va", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* read variable argument list containing diagonal element */
   va_start ( ap, size );
   A->data[0] = va_arg ( ap, double );
   va_end ( ap );
   return A;
}

/**
 * \brief Fill a scaled identity matrix with constant value.
 * \return Filled matrix \a A.
 *
 * Set matrix \a A to be a multiple of identity with given \a size, and fill
 * with given \a value.
 */
Gan_SquMatrix *
 gan_scalImat_fill_const_q ( Gan_SquMatrix *A, unsigned long size,
                             double value )
{
   if ( A == NULL )
      A = gan_scalImat_alloc ( size );
   else
      A = gan_scalImat_set_size ( A, size );

   if ( A == NULL )
   {
      gan_err_register ( "gan_scalImat_fill_const_q", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* fill matrix with constant */
   A->data[0] = value;

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
 * \brief Read the elements of a scaled identity matrix into pointers.
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Reads the single value of the given multiple of identity matrix \a A
 * into the variable argument list of pointers to \c double. Of course in
 * this case the variable argument list should contain exactly one pointer.
 * The \a size argument must match the size of the matrix.
 */
Gan_Bool
 gan_scalImat_read_va ( const Gan_SquMatrix *A, unsigned long size, ... )
{
   va_list ap;

   /* check that type and dimensions match */
   gan_assert ( A->type == GAN_SCALED_IDENT_MATRIX && size == A->size,
                "dimensions don't match in gan_scalImat_read_va()" );

   /* fill variable argument list of scaled identity matrix element pointers */
   va_start ( ap, size );
   *(va_arg ( ap, double * )) = A->data[0];
   va_end ( ap );

   /* return with success */
   return GAN_TRUE;
}

/**
 * \}
 */

/**
 * \addtogroup GeneralSizeMatrixAllocate
 * \{
 */

/* not a user function */
Gan_SquMatrix *
 gan_scalImat_form_gen ( Gan_SquMatrix *A, unsigned long size,
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
         A->data_size = 1;
      else
      {
         /* a non-zero data size has been requested */
         assert ( data_size >= 1 );
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
            gan_err_register_with_number ( "gan_scalImat_form_gen", GAN_ERROR_MALLOC_FAILED, "", A->data_size*sizeof(double) );
            return NULL;
         }
      }

      A->data_alloc = GAN_TRUE;
   }
   else
   {
      /* data for this matrix is provided */
      gan_err_test_ptr ( data_size >= 1, "gan_scalImat_form_gen",
                         GAN_ERROR_ILLEGAL_ARGUMENT, "" );

      /* set data pointer in matrix */
      A->data = data;
      A->data_size = data_size;

      /* we shouldn't free the matrix data */
      A->data_alloc = GAN_FALSE;;
   }

   /* set type and class member functions */
   gan_scalImat_set_size ( A, size );

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
 * \brief Fill multiple of identity matrix using variable argument list.
 * \return Result matrix \a A.
 *
 * Fill multiple of identity matrix \a A with values from variable argument
 * list \a aptr, and set the size of \a A to \a size.
 */
Gan_SquMatrix *
 gan_scalImat_fill_vap ( Gan_SquMatrix *A,
                         unsigned long size, va_list *aptr )
{
   A = gan_scalImat_set_size ( A, size );
   if ( A == NULL )
   {
      gan_err_register ( "gan_scalImat_fill_vap", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* read variable argument list containing diagonal element */
   A->data[0] = va_arg ( *aptr, double );

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

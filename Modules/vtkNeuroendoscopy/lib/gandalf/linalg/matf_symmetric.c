/**
 * File:          $RCSfile: matf_symmetric.c,v $
 * Module:        Symmetric matrices (single precision)
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
#include <gandalf/linalg/matf_symmetric.h>
#include <gandalf/linalg/matf_triangular.h>
#include <gandalf/linalg/matf_diagonal.h>
#include <gandalf/linalg/matf_gen.h>
#include <gandalf/linalg/vecf_gen.h>
#include <gandalf/linalg/matvecf_clapack.h>
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

/* function to set element i,j of symmetric matrix  */
static Gan_Bool
 symmetric_matrix_set_el ( Gan_SquMatrix_f *A,
                           unsigned i, unsigned j, float value )
{
   /* consistency check */
   gan_err_test_bool ( A->type == GAN_SYMMETRIC_MATRIX,
                       "symmetric_matrix_set_el", GAN_ERROR_ILLEGAL_ARGUMENT,
                       "" );

   /* make sure element is inside matrix */
   gan_err_test_bool ( i < A->size && j < A->size, "symmetric_matrix_set_el",
                       GAN_ERROR_REF_OUTSIDE_MATRIX, "" );

   /* set i,j element to given value */
   if ( i >= j )
      A->data[i*(i+1)/2+j] = value;
   else
      A->data[j*(j+1)/2+i] = value;

   return GAN_TRUE;
}

/* function to return element i,j of symmetric matrix */
static float
 symmetric_matrix_get_el ( const Gan_SquMatrix_f *A, unsigned i, unsigned j )
{
   /* consistency check */
   gan_err_test_float ( A->type == GAN_SYMMETRIC_MATRIX,
                         "symmetric_matrix_set_el", GAN_ERROR_ILLEGAL_ARGUMENT,
                         "" );

   /* make sure element is inside matrix */
   gan_err_test_float ( i < A->size && j < A->size, "symmetric_matrix_get_el",
                        GAN_ERROR_REF_OUTSIDE_MATRIX, "" );

   /* get i,j element */
   if ( i >= j )
      return A->data[i*(i+1)/2+j];
   else
      return A->data[j*(j+1)/2+i];
}

/* function to increment element i,j of symmetric matrix by given value */
static Gan_Bool
 symmetric_matrix_inc_el ( Gan_SquMatrix_f *A,
                           unsigned i, unsigned j, float value )
{
   /* consistency check */
   gan_err_test_bool ( A->type == GAN_SYMMETRIC_MATRIX,
                       "symmetric_matrix_inc_el", GAN_ERROR_ILLEGAL_ARGUMENT,
                       "" );

   /* make sure element is inside matrix */
   gan_err_test_bool ( i < A->size && j < A->size, "symmetric_matrix_inc_el",
                       GAN_ERROR_REF_OUTSIDE_MATRIX, "" );

   /* increment i,j element by given value */
   if ( i >= j )
      A->data[i*(i+1)/2+j] += value;
   else
      A->data[j*(j+1)/2+i] += value;

   return GAN_TRUE;
}

/* function to decrement element i,j of symmetric matrix by given value */
static Gan_Bool
 symmetric_matrix_dec_el ( Gan_SquMatrix_f *A,
                           unsigned i, unsigned j, float value )
{
   /* consistency check */
   gan_err_test_bool ( A->type == GAN_SYMMETRIC_MATRIX,
                       "symmetric_matrix_dec_el", GAN_ERROR_ILLEGAL_ARGUMENT,
                       "" );

   /* make sure element is inside matrix */
   gan_err_test_bool ( i < A->size && j < A->size, "symmetric_matrix_dec_el",
                       GAN_ERROR_REF_OUTSIDE_MATRIX, "" );


   /* decrement i,j element by given value */
   if ( i >= j )
      A->data[i*(i+1)/2+j] -= value;
   else
      A->data[j*(j+1)/2+i] -= value;

   return GAN_TRUE;
}

/* function to print symmetric matrix to file pointer */
static Gan_Bool
 symmetric_matrix_fprint ( FILE *fp, const Gan_SquMatrix_f *A, const char *prefix,
                           unsigned indent, const char *fmt )
{
   unsigned long i, j;
   int p;

   /* consistency check */
   gan_err_test_bool ( A->type == GAN_SYMMETRIC_MATRIX,
                       "symmetric_matrix_fprint", GAN_ERROR_ILLEGAL_ARGUMENT,
                       "" );

   /* print indentation on first line */
   for ( p = indent-1; p >= 0; p-- ) fprintf ( fp, " " );
   fprintf ( fp, "%s (%0ldx%0ld symmetric matrix)\n", prefix,
             A->size, A->size );

   for ( i = 0; i < A->size; i++ )
   {
      for ( p = indent-1; p >= 0; p-- ) fprintf ( fp, " " );
      for ( j = 0; j < A->size; j++ )
      {
         fprintf ( fp, " " );
         if ( j <= i )
            fprintf ( fp, fmt, A->data[i*(i+1)/2+j] );
         else
            fprintf ( fp, fmt, A->data[j*(j+1)/2+i] );
      }

      fprintf ( fp, "\n" );
   }

   return GAN_TRUE;
}

/* function to copy symmetric matrix A into another matrix B */
static Gan_SquMatrix_f *
 symmetric_matrix_copy ( const Gan_SquMatrix_f *A, Gan_SquMatrix_f *B )
{
   /* consistency check */
   gan_err_test_ptr ( A->type == GAN_SYMMETRIC_MATRIX,
                      "symmetric_matrix_copy", GAN_ERROR_ILLEGAL_ARGUMENT, "");

   /* allocate matrix B if necessary */
   if ( B == NULL )
      B = gan_symmatf_alloc ( A->size );
   else
      B = gan_symmatf_set_size ( B, A->size );

   if ( B == NULL )
   {
      gan_err_register ( "symmetric_matrix_copy", GAN_ERROR_FAILURE, "" );
      return NULL;
   }
   
   /* copy matrix */
#ifdef HAVE_LAPACK
   {
      long nel = A->size*(A->size+1)/2, onei = 1;

      scopy_ ( &nel, A->data, &onei, B->data, &onei );
   }
#else
   if ( !gan_scopy ( A->size*(A->size+1)/2, A->data, 1, B->data, 1 ) )
   {
      gan_err_flush_trace();
      gan_err_register ( "symmetric_matrix_copy", GAN_ERROR_FAILURE, "" );
      return NULL;
   }
#endif /* #ifdef HAVE_LAPACK */

   return B;
}

/* function to rescale symmetric matrix A into another matrix B */
static Gan_SquMatrix_f *
 symmetric_matrix_scale ( Gan_SquMatrix_f *A, float a, Gan_SquMatrix_f *B )
{
   /* consistency check */
   gan_err_test_ptr ( A->type == GAN_SYMMETRIC_MATRIX,
                      "symmetric_matrix_scale", GAN_ERROR_ILLEGAL_ARGUMENT,"");

   /* allocate matrix B if necessary */
   if ( B == NULL )
      B = gan_symmatf_alloc ( A->size );
   else
      B = gan_symmatf_set_size ( B, A->size );

   if ( B == NULL )
   {
      gan_err_register ( "symmetric_matrix_scale", GAN_ERROR_FAILURE, "" );
      return NULL;
   }
   
   /* copy and scale matrix */
#ifdef HAVE_LAPACK
   {
      long nel = A->size*(A->size+1)/2, onei = 1;

      if(A!=B) scopy_ ( &nel, A->data, &onei, B->data, &onei );
      sscal_ ( &nel, &a, B->data, &onei );
   }
#else
   if ( (A!=B && !gan_scopy ( A->size*(A->size+1)/2, A->data, 1, B->data, 1 )) ||
        !gan_sscal ( A->size*(A->size+1)/2, a, B->data, 1 ) )
   {
      gan_err_flush_trace();
      gan_err_register ( "symmetric_matrix_scale", GAN_ERROR_FAILURE, "" );
      return NULL;
   }
#endif /* #ifdef HAVE_LAPACK */

   return B;
}

/* function to add symmetric matrices together C = A+B */
static Gan_SquMatrix_f *
 symmetric_matrix_add ( Gan_SquMatrix_f *A, Gan_TposeFlag A_tr,
                        Gan_SquMatrix_f *B, Gan_TposeFlag B_tr,
                        Gan_SquMatrix_f *C )
{
   Gan_SquMatrixType Btype = B->type;

   /* consistency check */
   gan_err_test_ptr ( A->type == GAN_SYMMETRIC_MATRIX,
                      "symmetric_matrix_add", GAN_ERROR_ILLEGAL_ARGUMENT,"");

   /* check that the type and dimensions of input matrices A & B are
      compatible */
   gan_err_test_ptr ( gan_squmatf_same_size(A,B), "symmetric_matrix_add",
                      GAN_ERROR_DIFFERENT_DIMS, "" );

   /* allocate result matrix C if necessary */
   if ( C == NULL )
      C = gan_symmatf_alloc ( A->size );
   else
      C = gan_symmatf_set_size ( C, A->size );

   if ( C == NULL )
   {
      gan_err_register ( "symmetric_matrix_add", GAN_ERROR_FAILURE, "" );
      return NULL;
   }
   
   /* add matrix data */
   switch ( Btype )
   {
      case GAN_SYMMETRIC_MATRIX:

#ifdef HAVE_LAPACK
      {
         /* number of matrix elements */
         long nel = A->size*(A->size+1)/2, onei = 1;
         float onef = 1.0F;
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
         Gan_Bool result;

         if ( C == A )
            /* in-place operation A += B */
            result = gan_saxpy ( A->size*(A->size+1)/2, 1.0F, B->data, 1,
                                 A->data, 1 );
         else if ( C == B )
            /* in-place operation B += A */
            result = gan_saxpy ( A->size*(A->size+1)/2, 1.0F, A->data, 1,
                                 B->data, 1 );
         else
         {
            /* C = A + B */
            result = gan_scopy ( A->size*(A->size+1)/2, A->data, 1,
                                 C->data, 1 );
            result = (Gan_Bool)(result & gan_saxpy(A->size*(A->size+1)/2, 1.0F,
                                                   B->data, 1, C->data, 1));
         }

         if ( !result )
         {
            gan_err_flush_trace();
            gan_err_register ( "symmetric_matrix_add", GAN_ERROR_FAILURE, "" );
            return NULL;
         }
      }
#endif /* #ifdef HAVE_LAPACK */
      break;

      case GAN_DIAGONAL_MATRIX:
      {
         long i, j;

         /* set diagonal elements */
         for ( i = A->size-1; i >= 0; i-- )
            C->data[i*(i+1)/2+i] = B->data[i] + A->data[i*(i+1)/2+i];

         /* copy off-diagonal elements from A */
         for ( i = A->size-1; i >= 0; i-- )
            for ( j = i-1; j >= 0; j-- )
               C->data[i*(i+1)/2+j] = A->data[i*(i+1)/2+j];
      }
      break;

      case GAN_SCALED_IDENT_MATRIX:
      {
         long i, j;

         /* set diagonal elements */
         for ( i = A->size-1; i >= 0; i-- )
            C->data[i*(i+1)/2+i] = B->data[0] + A->data[i*(i+1)/2+i];

         /* copy off-diagonal elements from A */
         for ( i = A->size-1; i >= 0; i-- )
            for ( j = i-1; j >= 0; j-- )
               C->data[i*(i+1)/2+j] = A->data[i*(i+1)/2+j];
      }
      break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "symmetric_matrix_add", GAN_ERROR_ILLEGAL_TYPE, "");
        return NULL;
   }

   /* return with success */
   return C;
}

/* function to subtract symmetric matrices from each other: C = A-B.
 */
static Gan_SquMatrix_f *
 symmetric_matrix_sub ( Gan_SquMatrix_f *A, Gan_TposeFlag A_tr,
                        Gan_SquMatrix_f *B, Gan_TposeFlag B_tr,
                        Gan_SquMatrix_f *C )
{
   /* consistency check */
   gan_err_test_ptr ( A->type == GAN_SYMMETRIC_MATRIX &&
                      B->type == GAN_SYMMETRIC_MATRIX,
                      "symmetric_matrix_sub", GAN_ERROR_ILLEGAL_ARGUMENT,"");

   /* check that the type and dimensions of input matrices A & B are
      compatible */
   gan_err_test_ptr ( gan_squmatf_same_type_size(A,B), "symmetric_matrix_sub",
                      GAN_ERROR_DIFFERENT_DIMS, "" );

   /* allocate result matrix C if necessary */
   if ( C == NULL )
      C = gan_symmatf_alloc ( A->size );
   else
      C = gan_symmatf_set_size ( C, A->size );

   if ( C == NULL )
   {
      gan_err_register ( "symmetric_matrix_sub", GAN_ERROR_FAILURE, "" );
      return NULL;
   }
   
#ifdef HAVE_LAPACK
   {
      /* number of matrix elements */
      long nel = A->size*(A->size+1)/2, onei = 1;
      float minus_onef = -1.0F;

      /* subtract matrix data */
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
      Gan_Bool result;

      /* subtract matrix data */
      if ( C == A )
         /* in-place operation A -= B */
         result = gan_saxpy ( A->size*(A->size+1)/2, -1.0F, B->data, 1,
                              A->data, 1 );
      else if ( C == B )
      {
         /* in-place operation B = A - B */
         result = gan_sscal ( A->size*(A->size+1)/2, -1.0F, B->data, 1 );
         result = (Gan_Bool) (result & gan_saxpy (A->size*(A->size+1)/2, 1.0F,
                                                  A->data, 1, B->data, 1));
      }
      else
      {
         /* C = A - B */
         result = gan_scopy ( A->size*(A->size+1)/2, A->data, 1, C->data, 1 );
         result = (Gan_Bool) (result & gan_saxpy (A->size*(A->size+1)/2, -1.0F,
                                                  B->data, 1, C->data, 1));
      }

      if ( !result )
      {
         gan_err_flush_trace();
         gan_err_register ( "symmetric_matrix_sub", GAN_ERROR_FAILURE, "" );
         return NULL;
      }
   }
#endif /* #ifdef HAVE_LAPACK */

   /* return with success */
   return C;
}

/* function to compute matrix/vector product A*x = y */
static Gan_Vector_f *
 symmetric_matrix_multv (
     const Gan_SquMatrix_f *A, Gan_TposeFlag A_tr, Gan_InvertFlag A_inv,
     Gan_Vector_f *x, Gan_Vector_f *y )
{
   /* consistency check */
   gan_err_test_ptr ( A->type == GAN_SYMMETRIC_MATRIX,
                      "symmetric_matrix_multv", GAN_ERROR_FAILURE, "" );

   /* can't do implicit inverse of symmetric matrix */
   gan_err_test_ptr ( !A_inv, "symmetric_matrix_multv",
                      GAN_ERROR_NO_IMPLICIT_INV, "" );

   /* check that the dimensions of input matrix A and vector x are
      compatible */
   gan_err_test_ptr ( A->size == x->rows, "symmetric_matrix_multv",
                      GAN_ERROR_INCOMPATIBLE, "matrix/vector" );

   /* allocate result vector y if necessary */
   if ( y == NULL )
      y = gan_vecf_alloc ( A->size );
   else
   {
      /* can't do in-place matrix/vector multiply with symmetric matrix */
      gan_err_test_ptr ( y != x, "symmetric_matrix_multv",
                         GAN_ERROR_FAILURE, "can't do in-place multiply" );
      y = gan_vecf_set_size ( y, A->size );
   }

   /* set values to zero initially */
   if ( y == NULL || gan_vecf_fill_zero_q ( y, y->rows ) == NULL )
   {
      gan_err_register ( "symmetric_matrix_multv", GAN_ERROR_FAILURE, "" );
      return NULL;
   }
   
#ifdef HAVE_LAPACK
   {
      float zerof = 0.0F, onef = 1.0F;
      long onei = 1;

      /* compute matrix/vector product */
      sspmv_ ( "U", (long *)&A->size, &onef, A->data, x->data, &onei, &zerof,
               y->data, &onei );
   }
#else
   /* compute matrix/vector product */
   if ( !gan_sspmv ( GAN_MATRIXPART_UPPER, A->size, 1.0F, A->data, x->data, 1, 0.0F,
                     y->data, 1 ) )
   {
      gan_err_flush_trace();
      gan_err_register ( "symmetric_matrix_multv", GAN_ERROR_FAILURE, "" );
      return NULL;
   }
#endif /* #ifdef HAVE_LAPACK */

   /* return with success */
   return y;
}

/* function to multipy symmetric matrix A and generic matrix B: C = A*B(^T),
 * returning C
 */
static Gan_Matrix_f *
 symmetric_matrix_rmult (
     const Gan_SquMatrix_f *A, Gan_TposeFlag A_tr, Gan_InvertFlag A_inv,
     Gan_Matrix_f          *B, Gan_TposeFlag B_tr, Gan_Matrix_f *C )
{
   unsigned long Ccols = B_tr ? B->rows : B->cols;
   unsigned long j;

   /* consistency check */
   gan_err_test_ptr ( A->type == GAN_SYMMETRIC_MATRIX,
                      "symmetric_matrix_rmult", GAN_ERROR_FAILURE, "" );

   /* can't do implicit inverse of symmetric matrix */
   gan_err_test_ptr ( !A_inv, "symmetric_matrix_rmult",
                      GAN_ERROR_NO_IMPLICIT_INV, "" );

   /* check that the dimensions of input matrices A,B are compatible */
   gan_err_test_ptr ( A->size == (B_tr ? B->cols : B->rows),
                      "symmetric_matrix_rmult", GAN_ERROR_INCOMPATIBLE,
                      "matrix/vector" );

   /* allocate result matrix C if necessary */
   if ( C == NULL )
      C = gan_matf_alloc ( A->size, Ccols );
   else
   {
      /* can't do in-place matrix multiply */
      gan_err_test_ptr (  B != C, "symmetric_matrix_rmult",
                          GAN_ERROR_INPLACE_MULTIPLY, "" );
      C = gan_matf_set_dims ( C, A->size, Ccols );
   }

   /* fill matrix with zeros initially */
   if ( C == NULL || gan_matf_fill_zero_q ( C, C->rows, C->cols ) == NULL )
   {
      gan_err_register ( "symmetric_matrix_rmult", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

#ifdef HAVE_LAPACK
   {
      long onei = 1;
      float onef = 1.0F, zerof = 0.0F;

      if ( B_tr )
         /* multiply matrix data C = A*B^T */
         for ( j = 0; j < Ccols; j++ )
            sspmv_ ( "U", (long *)&A->size, &onef, A->data,
                     &B->data[j], (long *)&B->rows,
                     &zerof, &C->data[j*C->rows], &onei );
      else
         /* multiply matrix data C = A*B */
         for ( j = 0; j < Ccols; j++ )
            sspmv_ ( "U", (long *)&A->size, &onef, A->data,
                     &B->data[j*B->rows], &onei,
                     &zerof, &C->data[j*C->rows], &onei );
   }
#else
   if ( B_tr )
   {
      /* multiply matrix data C = A*B^T */
      for ( j = 0; j < Ccols; j++ )
         if ( !gan_sspmv ( GAN_MATRIXPART_UPPER, A->size, 1.0F, A->data, &B->data[j],
                           B->rows, 0.0F, &C->data[j*C->rows], 1 ) )
         {
            gan_err_flush_trace();
            gan_err_register ( "symmetric_matrix_rmult",
                               GAN_ERROR_FAILURE, "" );
            return NULL;
         }
   }
   else
   {
      /* multiply matrix data C = A*B */
      for ( j = 0; j < Ccols; j++ )
         if ( !gan_sspmv ( GAN_MATRIXPART_UPPER, A->size, 1.0F, A->data,
                           &B->data[j*B->rows], 1, 0.0F,
                           &C->data[j*C->rows], 1 ) )
         {
            gan_err_flush_trace();
            gan_err_register ( "symmetric_matrix_rmult",
                               GAN_ERROR_FAILURE, "" );
            return NULL;
         }
   }
#endif /* #ifdef HAVE_LAPACK */

   /* return with success */
   return C;
}

/* function to multipy symmetric matrix A and generic matrix B: C = B(^T)*A,
 * returning C
 */
static Gan_Matrix_f *
 symmetric_matrix_lmult (
     const Gan_SquMatrix_f *A, Gan_TposeFlag A_tr, Gan_InvertFlag A_inv,
     Gan_Matrix_f          *B, Gan_TposeFlag B_tr, Gan_Matrix_f *C )
{
   unsigned long Crows = B_tr ? B->cols : B->rows;
   unsigned long i;

   /* consistency check */
   gan_err_test_ptr ( A->type == GAN_SYMMETRIC_MATRIX,
                      "symmetric_matrix_lmult", GAN_ERROR_FAILURE, "" );

   /* can't do implicit inverse of symmetric matrix */
   gan_err_test_ptr ( !A_inv, "symmetric_matrix_lmult",
                      GAN_ERROR_NO_IMPLICIT_INV, "" );

   /* check that the dimensions of input matrices A,B are compatible */
   gan_err_test_ptr ( A->size == (B_tr ? B->rows : B->cols),
                      "symmetric_matrix_lmult", GAN_ERROR_INCOMPATIBLE,
                      "matrix/vector" );

   /* allocate result matrix C if necessary */
   if ( C == NULL )
      C = gan_matf_alloc ( Crows, A->size );
   else
   {
      /* can't do in-place matrix multiply */
      gan_err_test_ptr (  B != C, "symmetric_matrix_lmult",
                          GAN_ERROR_INPLACE_MULTIPLY, "" );
      C = gan_matf_set_dims ( C, Crows, A->size );
   }

   /* fill matrix with zeros initially */
   if ( C == NULL || gan_matf_fill_zero_q ( C, C->rows, C->cols ) == NULL )
   {
      gan_err_register ( "symmetric_matrix_lmult", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

#ifdef HAVE_LAPACK
   {
      long onei = 1;
      float onef = 1.0F, zerof = 0.0F;
      
      if ( B_tr )
         /* multiply matrix data C = B^T*A */
         for ( i = 0; i < Crows; i++ )
            sspmv_ ( "U", (long *)&A->size, &onef, A->data,
                     &B->data[i*B->rows], &onei,
                     &zerof, &C->data[i], (long *)&C->rows );
      else
         /* multiply matrix data C = B*A */
         for ( i = 0; i < Crows; i++ )
            sspmv_ ( "U", (long *)&A->size, &onef, A->data,
                     &B->data[i], (long *)&B->rows,
                     &zerof, &C->data[i], (long *)&C->rows );
   }
#else
   if ( B_tr )
   {
      /* multiply matrix data C = B^T*A */
      for ( i = 0; i < Crows; i++ )
         if ( !gan_sspmv ( GAN_MATRIXPART_UPPER, A->size, 1.0F, A->data,
                           &B->data[i*B->rows], 1, 0.0F,
                           &C->data[i], C->rows ))
         {
            gan_err_flush_trace();
            gan_err_register ( "symmetric_matrix_lmult", GAN_ERROR_FAILURE,
                               "" );
            return NULL;
         }
   }
   else
   {
      /* multiply matrix data C = B*A */
      for ( i = 0; i < Crows; i++ )
         if ( !gan_sspmv ( GAN_MATRIXPART_UPPER, A->size, 1.0F, A->data, &B->data[i],
                           B->rows, 0.0F, &C->data[i], C->rows ) )
         {
            gan_err_flush_trace();
            gan_err_register ( "symmetric_matrix_lmult", GAN_ERROR_FAILURE,
                               "" );
            return NULL;
         }
   }
#endif /* #ifdef HAVE_LAPACK */

   /* return with success */
   return C;
}

/* function to multipy symmetric matrix A and square matrix B:
 * C = A*B(^-1)(^T), returning C
 */
static Gan_SquMatrix_f *
 symmetric_matrix_rmult_squ (
     Gan_SquMatrix_f *A, Gan_TposeFlag A_tr, Gan_InvertFlag A_inv,
     Gan_SquMatrix_f *B, Gan_TposeFlag B_tr, Gan_InvertFlag B_inv,
     Gan_SquMatrix_f *C )
{
   /* consistency check */
   gan_err_test_ptr ( A->type == GAN_SYMMETRIC_MATRIX &&
                      B->type == GAN_SYMMETRIC_MATRIX,
                      "symmetric_matrix_rmult_squ", GAN_ERROR_FAILURE, "" );

   gan_err_flush_trace();
   gan_err_register ( "symmetric_matrix_rmult_squ", GAN_ERROR_NOT_IMPLEMENTED,
                      "" );
   return NULL;
}

/* function to multipy symmetric matrix A on the left my generic matrix B or
 * B^T and on the right by B^T or B, filling in symmetric matrix D with the
 * result. Generic matrix C is used to store the intermediate result A*B(^T).
 * The result matrix D is returned.
 */
static Gan_SquMatrix_f *
 symmetric_matrix_lrmult ( const Gan_SquMatrix_f *A,
                           const Gan_Matrix_f *B, Gan_TposeFlag B_tr,
                           Gan_Matrix_f *C, Gan_SquMatrix_f *D )
{
   unsigned long Dsize = B_tr ? B->cols : B->rows;
   Gan_Bool Calloc = GAN_FALSE;

   /* consistency check */
   gan_err_test_ptr ( A->type == GAN_SYMMETRIC_MATRIX,
                      "symmetric_matrix_lrmult", GAN_ERROR_FAILURE, "" );

   /* check that the dimensions of input matrices A,B are compatible */
   gan_err_test_ptr ( A->size == (B_tr ? B->rows : B->cols),
                      "symmetric_matrix_lrmult", GAN_ERROR_INCOMPATIBLE,
                      "input matrices" );

   /* allocate intermediate result matrix C if necessary (to be freed at end)*/
   if ( C == NULL )
   {
      C = gan_matf_alloc ( A->size, Dsize );
      Calloc = GAN_TRUE;
   }
   else
   {
      /* can't do in-place matrix multiply */
      gan_err_test_ptr ( B != C, "symmetric_matrix_lrmult",
                         GAN_ERROR_INPLACE_MULTIPLY, "" );
      C = gan_matf_set_dims ( C, A->size, Dsize );
   }

   /* allocate result matrix D if necessary */
   if ( D == NULL )
      D = gan_symmatf_alloc ( Dsize );
   else
      D = gan_symmatf_set_size ( D, Dsize );

   /* perform first multiplication step A*B(^T) */
   if ( A->funcs->rmult ( A, GAN_NOTRANSPOSE, GAN_NOINVERT,
                          (Gan_Matrix_f *)B, B_tr ? GAN_NOTRANSPOSE : GAN_TRANSPOSE, C )
        == NULL )
   {
      gan_err_register ( "symmetric_matrix_lrmult", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* perform second multiplication step B*(A*B^T) or B^T*(A*B) */
   if ( gan_matf_rmult_sym_gen ( B, B_tr, C, GAN_NOTRANSPOSE, D ) == NULL )
   {
      gan_err_register ( "symmetric_matrix_lrmult", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* free intermediate result matrix C if it was allocated */
   if ( Calloc )
      gan_matf_free ( C );

   /* return result matrix */
   return D;
}

#if 0
/* function to multipy symmetric matrix A on the left my square matrix B,
 * which is optionally transposed and/or inverted (implicitly) prior to
 * multiplication. Symmetric matrix D is filled in with the result.
 * Generic matrix C is used to store the intermediate result A*B(^T)(^-1).
 * The result matrix D is returned.
 */
Gan_SquMatrix_f *
 symmetric_matrix_lrmult_squ (
     Gan_SquMatrix_f *A,
     Gan_SquMatrix_f *B, Gan_TposeFlag B_tr, Gan_InvertFlag B_inv,
     Gan_Matrix_f *C, Gan_SquMatrix_f *D )
{
   unsigned long nel = A->size*(A->size+1)/2; /* number of matrix elements */
   Gan_Bool Calloc = GAN_FALSE;

   /* consistency check */
   gan_err_test_ptr ( A->type == GAN_SYMMETRIC_MATRIX,
                      "symmetric_matrix_lrmult_squ", GAN_ERROR_FAILURE, "" );

   /* check that the dimensions of input matrices A,B are compatible */
   gan_err_test_ptr ( A->size == B->size,
                      "symmetric_matrix_lrmult_squ", GAN_ERROR_FAILURE, "" );

   /* allocate intermediate result matrix C if necessary (to be freed at end)*/
   if ( C == NULL )
   {
      C = gan_matf_alloc ( A->size, A->size );
      Calloc = GAN_TRUE;
   }
   else
      C = gan_matf_set_dims ( C, A->size, A->size );

   /* allocate result matrix D if necessary */
   if ( D == NULL )
      D = gan_symmatf_alloc ( A->size );
   else
      D = gan_symmatf_set_size ( D, A->size );

   /* perform first multiplication step A*B(^T)(^-1) */
   gan_squmatf_rmult_squ ( A, GAN_NOTRANSPOSE, GAN_NOINVERT,
                           B, B_tr, B_inv, C );

   /* perform second multiplication step B(^T)(^-1)*C */
   {
      Gan_Matrix_f *E;
      long i, j;

      E = B->rmult ( B, B_tr ? GAN_NOTRANSPOSE : GAN_TRANSPOSE,
                     B_inv ? GAN_NOINVERT : GAN_INVERT,
                     C, GAN_NOTRANSPOSE, NULL );
      for ( i = A->size-1; i >= 0; i-- )
         for ( j = i; j >= 0; j-- )
            D->data[i*(i+1)/2+j] = E->data[j*E->rows+i];

      gan_matf_free ( E );
   }

   /* free intermediate result matrix C if it was allocated */
   if ( Calloc )
      gan_matf_free ( C );

   /* return result matrix */
   return D;
}
#endif

/* perform matrix inversion on symmetric matrix A, producing symmetric
 * matrix B = A^-1.
 */
static Gan_SquMatrix_f *
 symmetric_matrix_invert ( Gan_SquMatrix_f *A, Gan_SquMatrix_f *B )
{
   long info;

   /* consistency check */
   gan_err_test_ptr ( A->type == GAN_SYMMETRIC_MATRIX,
                      "symmetric_matrix_invert", GAN_ERROR_ILLEGAL_ARGUMENT,
                      "" );

   /* allocate matrix B if necessary */
   if ( B == NULL )
      B = gan_symmatf_alloc ( A->size );
   else
      B = gan_symmatf_set_size ( B, A->size );

#ifdef HAVE_LAPACK
   if ( A != B )
   {
      /* copy input matrix A into B */
      long nel = A->size*(A->size+1)/2, onei = 1;

      scopy_ ( &nel, A->data, &onei, B->data, &onei );
   }

   /* compute Cholesky factorisation */
   spptrf_ ( "U", (long *)&B->size, B->data, &info );
   if ( info != 0 )
   {
      gan_err_flush_trace();
      if ( info < 0 )
      {
         gan_err_register ( "symmetric_matrix_invert",
                            GAN_ERROR_CLAPACK_ILLEGAL_ARG, "in dpptrf_()" );
      }
      else /* info > 0 */
         gan_err_register ( "symmetric_matrix_invert",
                            GAN_ERROR_NOT_POSITIVE_DEFINITE, "" );

      return NULL;
   }

   /* compute inverse from Cholesky factorisation */
   spptri_ ( "U", (long *)&B->size, B->data, &info );
   if ( info != 0 )
   {
      gan_err_flush_trace();
      if ( info < 0 )
      {
         gan_err_register ( "symmetric_matrix_invert",
                            GAN_ERROR_CLAPACK_ILLEGAL_ARG, "in dpptri_()" );
      }
      else /* info > 0 */
         gan_err_register ( "symmetric_matrix_invert",
                            GAN_ERROR_DIVISION_BY_ZERO, "" );

      return NULL;
   }
#else
   if ( A != B )
   {
      /* copy input matrix A into B */
      if ( !gan_scopy ( A->size*(A->size+1)/2, A->data, 1, B->data, 1 ) )
      {
         gan_err_flush_trace();
         gan_err_register ( "symmetric_matrix_invert",
                            GAN_ERROR_FAILURE, "" );
         return NULL;
      }
   }
   
   /* compute Cholesky factorisation */
   if ( !gan_spptrf ( GAN_MATRIXPART_UPPER, B->size, B->data, &info ) )
   {
      gan_err_flush_trace();
      if ( info < 0 )
      {
         gan_err_register ( "symmetric_matrix_invert",
                            GAN_ERROR_CLAPACK_ILLEGAL_ARG, "in dpptrf_()" );
      }
      else /* info >= 0 */
         gan_err_register ( "symmetric_matrix_invert",
                            GAN_ERROR_NOT_POSITIVE_DEFINITE, "" );

      return NULL;
   }

   /* compute inverse from Cholesky factorisation */
   if ( !gan_spptri ( GAN_MATRIXPART_UPPER, B->size, B->data, &info ) )
   {
      gan_err_flush_trace();
      if ( info < 0 )
      {
         gan_err_register ( "symmetric_matrix_invert",
                            GAN_ERROR_CLAPACK_ILLEGAL_ARG, "in spptri_()" );
      }
      else /* info > 0 */
         gan_err_register ( "symmetric_matrix_invert",
                       GAN_ERROR_DIVISION_BY_ZERO, "" );

      return NULL;
   }
#endif /* #ifdef HAVE_LAPACK */

   return B;
}

/* perform Cholesky factorisation of symmetric matrix S into lower
 * triangular matrix L.
 */
static Gan_SquMatrix_f *
 symmetric_matrix_cholesky ( Gan_SquMatrix_f *S,
                             Gan_SquMatrix_f *L, int *error_code )
{
   /* consistency check */
   gan_err_test_ptr ( S->type == GAN_SYMMETRIC_MATRIX,
                      "symmetric_matrix_cholesky", GAN_ERROR_ILLEGAL_ARGUMENT,
                      "" );

   /* allocate matrix L if necessary */
   if ( L == NULL )
      L = gan_symmatf_alloc ( S->size );
   else
      L = gan_symmatf_set_size ( L, S->size );

   if ( L == NULL )
   {
      gan_err_register ( "symmetric_matrix_cholesky", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   if ( L != S )
   {
#ifdef HAVE_LAPACK
      /* copy input matrix S into L */
      long nel = S->size*(S->size+1)/2, onei = 1;

      scopy_ ( &nel, S->data, &onei, L->data, &onei );
#else
      if ( !gan_scopy ( S->size*(S->size+1)/2, S->data, 1, L->data, 1 ) )
      {
         gan_err_flush_trace();
         gan_err_register ( "symmetric_matrix_cholesky", GAN_ERROR_FAILURE,
                            "" );
         return NULL;
      }
#endif /* #ifdef HAVE_LAPACK */
   }

   if ( L->size > 0 && gan_squmatf_sumsqr ( L ) == 0.0F )
   {
#if 1
      if ( error_code == NULL )
      {
         gan_err_flush_trace();
         gan_err_register ( "symmetric_matrix_cholesky", GAN_ERROR_FAILURE,
                            "" );
      }
      else
         *error_code = GAN_ERROR_FAILURE;

      return NULL;
#else
      /* zero matrix: simply convert matrix to lower triangular */
      if ( gan_ltmatf_set ( L, L->size ) == NULL )
      {
         gan_err_register ( "symmetric_matrix_cholesky", GAN_ERROR_FAILURE,
                            "" );
         return NULL;
      }
#endif
   }
   else
      /* compute Cholesky factorisation */
      if ( !gan_clapackf_pptrf ( L, error_code ) )
      {
         if ( error_code == NULL )
            gan_err_register ( "symmetric_matrix_cholesky", GAN_ERROR_FAILURE,
                               "" );

         return NULL;
      }

   /* return with success */
   return L;
}

/* Compute squared Frobenius norm of symmetric matrix A.
 */
static float
 symmetric_matrix_sumsqr ( const Gan_SquMatrix_f *A )
{
   long i, j;
   float total = 0.0F;

   /* consistency check */
   assert ( A->type == GAN_SYMMETRIC_MATRIX );

   for ( i = (long)A->size-1; i >= 0; i-- )
   {
      for ( j = 0; j < i; j++ )
         total += 2.0F*gan_sqr(A->data[i*(i+1)/2+j]);

      total += gan_sqr(A->data[i*(i+1)/2+j]);
   }

   return total;
}

/* Compute Frobenius norm of symmetric matrix A.
 */
static float
 symmetric_matrix_Fnorm ( const Gan_SquMatrix_f *A )
{
   long i, j;
   float total = 0.0F;

   /* consistency check */
   assert ( A->type == GAN_SYMMETRIC_MATRIX );

   for ( i = (long)A->size-1; i >= 0; i-- )
   {
      for ( j = 0; j < i; j++ )
         total += 2.0F*gan_sqr(A->data[i*(i+1)/2+j]);

      total += gan_sqr(A->data[i*(i+1)/2+j]);
   }

   return (float)sqrt(total);
}

/* Convert symmetric matrix to generic matrix */
static Gan_Matrix_f *
 symmetric_matrix_matrix ( const Gan_SquMatrix_f *A, struct Gan_Matrix_f *B )
{
   long i, j;

   /* consistency check */
   assert ( A->type == GAN_SYMMETRIC_MATRIX );

   /* set result matrix B */
   if ( B == NULL )
      B = gan_matf_alloc ( A->size, A->size );
   else
      B = gan_matf_set_dims ( B, A->size, A->size );

   if ( B == NULL )
   {
      gan_err_register ( "symmetric_matrix_matrix", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* fill in lower triangle */
   for ( i = A->size-1; i >= 0; i-- )
      for ( j = i; j >= 0; j-- )
         B->data[i*A->size+j] = A->data[i*(i+1)/2+j];

   /* now copy into upper triangle */
   for ( j = 0; (unsigned long) j < A->size; j++ )
      for ( i = j+1; (unsigned  long) i < A->size; i++ )
         B->data[j*A->size+i] = B->data[i*A->size+j];

   return B;
}

static Gan_SquMatrixFFuncs funcs =
{
   symmetric_matrix_set_el,
   symmetric_matrix_get_el,
   symmetric_matrix_inc_el,
   symmetric_matrix_dec_el,
   symmetric_matrix_fprint,
   symmetric_matrix_copy,
   symmetric_matrix_scale,
   symmetric_matrix_add,
   symmetric_matrix_sub,
   symmetric_matrix_multv,
   symmetric_matrix_rmult,
   symmetric_matrix_lmult,
   symmetric_matrix_rmult_squ,
   symmetric_matrix_lrmult,
   symmetric_matrix_invert,
   symmetric_matrix_cholesky,
   symmetric_matrix_sumsqr,
   symmetric_matrix_Fnorm,
   symmetric_matrix_matrix
};

/**
 * \addtogroup GeneralSizeMatrixSet
 * \{
 */

/**
 * \brief Set square matrix to be symmetric with given size.
 * \return Result matrix \a A.
 *
 * Set square matrix \a A to be symmetric with given \a size.
 */
Gan_SquMatrix_f *
 gan_symmatf_set_size ( Gan_SquMatrix_f *A, unsigned long size )
{
   /* set type of matrix */
   A->type = GAN_SYMMETRIC_MATRIX;

   /* check whether matrix is big enough */
   if ( A->data_size < size*(size+1)/2 )
   {
      /* re-allocate matrix data */
      gan_err_test_ptr ( A->data_alloc, "gan_symmatf_set_size",
                         GAN_ERROR_CANT_REALLOC, "matrix" );

      A->data_size = size*(size+1)/2;
      A->data = gan_realloc_array ( float, A->data, A->data_size );
      if ( A->data == NULL )
      {
         gan_err_flush_trace();
         gan_err_register_with_number ( "gan_symmatf_set_size", GAN_ERROR_MALLOC_FAILED, "", A->data_size*sizeof(float) );
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
 * \brief Fill symmetric matrix from variable argument list.
 * \return Result matrix \a A.
 *
 * Fill symmetric matrix \a A with values from variable argument list
 * \a aptr, and set the size of \a A to \a size.
 */
Gan_SquMatrix_f *
 gan_symmatf_fill_va ( Gan_SquMatrix_f *A, unsigned long size, ... )
{
   unsigned int i, j;
   va_list ap;

   if ( A == NULL )
      A = gan_symmatf_alloc ( size );
   else
      A = gan_symmatf_set_size ( A, size );

   if ( A == NULL )
   {
      gan_err_register ( "gan_symmatf_fill_va", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* read variable argument list of matrix elements, lower triangle only */
   va_start ( ap, size );
   for ( i = 0; i < size; i++ )
      for ( j = 0; j <= i; j++ )
         A->data[i*(i+1)/2+j] = (float)va_arg ( ap, double );

   va_end ( ap );
   return A;
}

/**
 * \brief Fill symmetric matrix with constant value, and set its size.
 * \return Result matrix \a A.
 *
 * Fill matrix \a A with constant \a value, and set the size of \a A
 * to \a size.
 */
Gan_SquMatrix_f *
 gan_symmatf_fill_const_q ( Gan_SquMatrix_f *A, unsigned long size,
                            float value )
{
   int i;

   if ( A == NULL )
      A = gan_symmatf_alloc ( size );
   else
      A = gan_symmatf_set_size ( A, size );

   if ( A == NULL )
   {
      gan_err_register ( "symmetric_matrix_fill_const_q", GAN_ERROR_FAILURE,
                         "" );
      return NULL;
   }

   /* fill matrix with constant */
   for ( i = size*(size+1)/2-1; i >= 0; i-- )
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
 * \brief Read the elements of a symmetric matrix into pointers.
 *
 * The \a size argument must match the size of the matrix \a A.
 */
Gan_Bool
 gan_symmatf_read_va ( const Gan_SquMatrix_f *A, unsigned long size, ... )
{
   unsigned int i, j;
   va_list ap;

   /* check that type and dimensions match */
   gan_err_test_bool ( A->type == GAN_SYMMETRIC_MATRIX && size == A->size,
                       "gan_symmatf_read_va", GAN_ERROR_DIFFERENT_DIMS, "" );

   /* fill variable argument list of symmetric matrix element pointers,
      lower triangle only */
   va_start ( ap, size );
   for ( i = 0; i < size; i++ )
      for ( j = 0; j <= i; j++ )
         *(va_arg ( ap, float * )) = A->data[i*(i+1)/2+j];

   va_end ( ap );

   return GAN_TRUE;
}

/**
 * \}
 */

/**
 * \addtogroup GeneralSizeMatrixInsert
 * \{
 */

/**
 * \brief Inserts part of generic matrix in symmetric matrix.
 */
Gan_SquMatrix_f *
 gan_symmatf_insert_gen ( Gan_SquMatrix_f *S,
                          unsigned long rS,   unsigned long cS,
                          const Gan_Matrix_f *B, Gan_TposeFlag B_tr,
                          unsigned long rB,   unsigned long cB,
                          unsigned long rows, unsigned long cols )
{
   long i, j;

   gan_err_test_ptr ( rS != cS, "gan_symmatf_insert_gen", GAN_ERROR_FAILURE,
        "can't insert generic matrix into diagonal part of symmetric matrix" );

   /* if B is transposed, reverse orientation */
   if ( B_tr )
   {
      unsigned long ulTmp;

      ulTmp = rS; rS = cS; cS = ulTmp;
      ulTmp = rB; rB = cB; cB = ulTmp;
      ulTmp = rows; rows = cols; cols = ulTmp;

      /* this last bit is unnecessary as B_tr is not accessed */
      B_tr = GAN_NOTRANSPOSE;
   }

   /* make sure dimensions are consistent */
   gan_err_test_ptr ( cB + cols <= B->cols && rB + rows <= B->rows &&
                      cS + cols <= S->size && rS + rows <= S->size,
                      "gan_symmatf_insert_gen", GAN_ERROR_ILLEGAL_ARGUMENT,"");

   if ( rS < cS )
   {
      /* insert into region of S above diagonal */

      /* make sure region doesn't overlap diagonal */
      gan_err_test_ptr ( rS+rows <= cS, "gan_symmatf_insert_gen",
                         GAN_ERROR_FAILURE, "" );

      for ( i = rows-1; i >= 0; i-- )
         for ( j = cols-1; j >= 0; j-- )
            S->data[(cS+j)*(cS+j+1)/2 + rS+i] = B->data[(cB+j)*B->rows + rB+i];
   }
   else /* rS > cS */
   {
      /* insert into region of S below diagonal */

      /* make sure region doesn't overlap diagonal */
      gan_err_test_ptr ( cS+cols <= rS, "gan_symmatf_insert_gen",
                         GAN_ERROR_FAILURE, "" );

      for ( i = rows-1; i >= 0; i-- )
         for ( j = cols-1; j >= 0; j-- )
            S->data[(rS+i)*(rS+i+1)/2 + cS+j] = B->data[(cB+j)*B->rows + rB+i];
   }

   return S;
}

/**
 * \brief Inserts part of vector in symmetric matrix.
 */
Gan_SquMatrix_f *
 gan_symmatf_insertv_gen ( Gan_SquMatrix_f *S,
                           unsigned long rS, unsigned long cS,
                           const Gan_Vector_f *x, Gan_TposeFlag x_tr,
                           unsigned long rx,
                           unsigned long size )
{
   long i;

   /* if we are filling the upper part of the matrix, transpose the vector
      and swap everything */
   if ( rS < cS )
   {
      unsigned long ulTmp;

      x_tr = x_tr ? GAN_NOTRANSPOSE : GAN_TRANSPOSE;
      ulTmp = rS; rS = cS; cS = ulTmp;
   }
       
   if ( x_tr )
   {
      /* make sure dimensions are consistent */
      gan_err_test_ptr ( rx + size <= x->rows &&
                         cS + size <= rS && rS < S->size,
                         "gan_symmatf_insertv_gen", GAN_ERROR_INCOMPATIBLE,
                         "");

      for ( i = size-1; i >= 0; i-- )
         S->data[rS*(rS+1)/2+cS+i] = x->data[rx+i];
   }
   else
   {
      /* make sure dimensions are consistent */
      gan_err_test_ptr ( rx + size <= x->rows && rS + size <= S->size,
                         "gan_symmatf_insertv_gen", GAN_ERROR_INCOMPATIBLE,
                         "" );

      for ( i = size-1; i >= 0; i-- )
         S->data[(rS+i)*(rS+i+1)/2+cS] = x->data[rx+i];
   }

   return S;
}

/**
 * \brief Insert diagonal part of symmetric matrix in symmetric matrix.
 * \return Result matrix \a A.
 *
 * Fills a square on-diagonal part of symmetric matrix \a A starting at
 * row,column position \a rA,\a rA and with given row/column \a size
 * with the section in symmetric matrix \a B, starting at position
 * \a rB, \a rB.
 */
Gan_SquMatrix_f *
 gan_symmatf_insertsym ( Gan_SquMatrix_f *A, unsigned long rA,
                         const Gan_SquMatrix_f *B, unsigned long rB,
                         unsigned long size )
{
   long i, j;

   /* make sure dimensions are consistent */
   gan_err_test_ptr ( A->type == GAN_SYMMETRIC_MATRIX &&
                      rB + size <= B->size && rA + size <= A->size,
                      "gan_symmatf_insertsym", GAN_ERROR_INCOMPATIBLE,
                      "matrix dimensions" );

   switch ( B->type )
   {
      case GAN_SYMMETRIC_MATRIX:
        for ( j = size-1; j >= 0; j-- )
           for ( i = j; i >= 0; i-- )
              A->data[(rA+j)*(rA+j+1)/2 + rA+i] =
              B->data[(rB+j)*(rB+j+1)/2 + rB+i];

        break;

      case GAN_DIAGONAL_MATRIX:
        for ( j = size-1; j >= 0; j-- )
        {
           A->data[(rA+j)*(rA+j+1)/2 + rA+j] = B->data[rB+j];
           for ( i = j-1; i >= 0; i-- )
              A->data[(rA+j)*(rA+j+1)/2 + rA+i] = 0.0;
        }

        break;

      case GAN_SCALED_IDENT_MATRIX:
        for ( j = size-1; j >= 0; j-- )
        {
           A->data[(rA+j)*(rA+j+1)/2 + rA+j] = B->data[0];
           for ( i = j-1; i >= 0; i-- )
              A->data[(rA+j)*(rA+j+1)/2 + rA+i] = 0.0;
        }

        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_symmatf_insertsym", GAN_ERROR_ILLEGAL_TYPE,
                           "" );
        return NULL;
   }

   return A;
}

/**
 * \brief Insert off-diagonal part of symmetric matrix in symmetric matrix.
 * \return Result matrix \a A.
 *
 * Fills an off-diagonal rectangular part of symmetric matrix \a A starting at
 * row, column position \a rA,\a cA and with given dimensions \a rows,
 * \a cols with the section in symmetric matrix \a B, starting at position
 * \a rB, \a cB.
 */
Gan_SquMatrix_f *
 gan_symmatf_insertsym_od ( Gan_SquMatrix_f *A,
                            unsigned long rA, unsigned long cA,
                            const Gan_SquMatrix_f *B,
                            unsigned long rB, unsigned long cB,
                            unsigned long rows,
                            unsigned long cols )
{
   long i, j;

   gan_err_test_ptr ( A->type == GAN_SYMMETRIC_MATRIX &&
                      B->type == GAN_SYMMETRIC_MATRIX &&
                      rA != cA && rB != cB, "gan_symmatf_insertsym_od",
                      GAN_ERROR_FAILURE, "" );

   /* make sure dimensions are consistent */
   gan_err_test_ptr ( cA + cols <= A->size && rA + rows <= A->size &&
                      cB + cols <= B->size && rB + rows <= B->size,
                      "gan_symmatf_insertsym_od", GAN_ERROR_ILLEGAL_ARGUMENT,
                      "" );

   if ( rA < cA )
   {
      /* insert into region of A above diagonal */

      /* make sure region doesn't overlap diagonal */
      gan_err_test_ptr ( rA+rows <= cA && rB+rows <= cB,
                         "gan_symmatf_insertsym_od", GAN_ERROR_FAILURE, "" );

      for ( i = rows-1; i >= 0; i-- )
         for ( j = cols-1; j >= 0; j-- )
            A->data[(cA+j)*(cA+j+1)/2 + rA+i] =
            B->data[(cB+j)*(cB+j+1)/2 + rB+i];
   }
   else /* rS > cS */
   {
      /* insert into region of A below diagonal */

      /* make sure region doesn't overlap diagonal */
      gan_err_test_ptr ( cA+cols <= rA && cB+cols <= rB,
                         "gan_symmatf_insertsym_od", GAN_ERROR_FAILURE, "" );

      for ( i = rows-1; i >= 0; i-- )
         for ( j = cols-1; j >= 0; j-- )
            A->data[(rA+i)*(rA+i+1)/2 + cA+j] =
            B->data[(rB+i)*(rB+i+1)/2 + cB+j];
   }

   return A;
}

/**
 * \}
 */

/**
 * \addtogroup GeneralSizeMatrixEigen
 * \{
 */

struct EigenStuff
{
   int index;
   float eval;
};

static int eval_compare ( const struct EigenStuff *aes1,
                          const struct EigenStuff *aes2 )
{
   if ( aes1->eval > aes2->eval ) return 1;
   else if ( aes1->eval < aes2->eval ) return -1;
   else return 0;
}

/* sort eigenvalues & aigenvectors so that eigenvalues are in
 * ascending order */
static Gan_Bool
 sort_eigenstuff ( Gan_SquMatrix_f *W, Gan_Matrix_f *Z )
{
   struct EigenStuff *aes;
   int i, j;
   Gan_Matrix_f *Znew=NULL;

   /* consistency check */
   gan_err_test_bool ( W->type == GAN_DIAGONAL_MATRIX, "sort_eigenstuff",
                       GAN_ERROR_INCOMPATIBLE, "" );

   /* copy eigenvalues into array */
   aes = gan_malloc_array ( struct EigenStuff, W->size );
   if ( aes == NULL )
   {
      gan_err_register_with_number ( "sort_eigenstuff", GAN_ERROR_MALLOC_FAILED, "", W->size );
      return GAN_FALSE;
   }

   for ( i = (int)W->size-1; i >= 0; i-- )
   {
      aes[i].index = i;
      aes[i].eval = gan_squmatf_get_el(W,i,i);
   }

   qsort ( aes, (size_t) W->size, sizeof(struct EigenStuff),
           (int (*)(const void *, const void *))eval_compare );

   /* build permuted eigenvalues and eigenvectors */
   if ( Z != NULL )
   {
      Znew = gan_matf_copy_s(Z);
      if ( Znew == NULL )
      {
         gan_err_register ( "sort_eigenstuff", GAN_ERROR_FAILURE, "" );
         return GAN_FALSE;
      }
   }

   for ( i = (int)W->size-1; i >= 0; i-- )
   {
      gan_squmatf_set_el ( W, i, i, aes[i].eval );
      if ( Z != NULL )
         for ( j = (int)W->size-1; j >= 0; j-- )
            gan_matf_set_el ( Z, j, i,
                              gan_matf_get_el ( Znew, j, aes[i].index ) );
   }
   
   /* success */
   if ( Znew != NULL ) gan_matf_free(Znew);
   free(aes);
   return GAN_TRUE;
}

/**
 * \brief Computes eigenvalues and eigenvectors of a symmetric matrix.
 * \param A Input matrix, overwritten on output
 * \param W Diagonal matrix of eigenvalues
 * \param Z Matrix of eigenvectors or \c NULL
 * \param sort Whether to sort eigenvalues and eigenvectors
 * \param work Workspace array
 * \param work_size size of workspace
 * \return #GAN_TRUE on success, or #GAN_FALSE on failure.
 *
 * If \a Z is non-\c NULL it is filled with the eigenvectors of symmetric
 * matrix \a A. The eigenvalues of \a A are written into \a W. If \a work is
 * not \c NULL and LAPACK is being used, the size of the \a work array
 * (\a work_size) must be at least 3 times the size of \a A.
 *
 * If \a sort is passed as #GAN_TRUE, the eigenvalues are sorted into
 * ascending order.
 */
Gan_Bool
 gan_symmatf_eigen ( Gan_SquMatrix_f *A,
                     Gan_SquMatrix_f *W, Gan_Matrix_f *Z,
                     Gan_Bool sort,
                     float *work, unsigned long work_size )
{
   gan_err_test_bool ( gan_squmatf_symmetric_type(A),
                       "gan_clapack_spev", GAN_ERROR_ILLEGAL_TYPE, "" );

   /* set sizes of eigenvalue and eigenvector matrix */
   if ( gan_diagmatf_set_size ( W, A->size ) == NULL ||
        (Z != NULL && gan_matf_set_dims(Z,A->size,A->size) == NULL) )
   {
      gan_err_register ( "gan_clapack_spev", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

#ifdef HAVE_LAPACK
   {
      long info, onei = 1;

      gan_err_test_bool ( work_size >= 3*A->size,
                          "gan_symmatf_eigen", GAN_ERROR_FAILURE,
                          "workspace too small (at least 3*size of matrix)" );

      if ( Z == NULL )
         sspev_ ( "N", "U", (long *)&A->size, A->data, W->data, NULL, &onei,
                  work, &info );
      else
         sspev_ ( "V", "U", (long *)&A->size, A->data, W->data,
                  Z->data, (long *)&Z->rows, work, &info );

      if ( info != 0 )
      {
         gan_err_flush_trace();
         if ( info < 0 )
         {
            gan_err_register ( "gan_symmatf_eigen",
                               GAN_ERROR_CLAPACK_ILLEGAL_ARG, "" );
         }
         else /* info > 0 */
            gan_err_register ( "gan_symmatf_eigen",
                               GAN_ERROR_NO_CONVERGENCE, "" );

         return GAN_FALSE;
      }
   }
#else
   /* use CCM package instead */
   if ( Z == NULL )
   {
      /* only compute eigenvalues */
      void ccmf_eigval(float *a,float *ev,int n);

      ccmf_eigval ( A->data, W->data, A->size );
   }
   else
   {
      /* compute eigenvalues and eigenvectors */
      void ccmf_eigen(float *a,float *ev,int n);

      /* copy matrix into eigenvector matrix */
      if ( gan_matf_from_squmatf_q ( A, Z ) == NULL )
      {
         gan_err_flush_trace();
         gan_err_register ( "gan_symmatf_eigen", GAN_ERROR_FAILURE, "" );
         return GAN_FALSE;
      }
   
      ccmf_eigen ( Z->data, W->data, A->size );

      /* transpose eigenvector matrix in-place */
      if ( !gan_matf_tpose_i(Z) )
      {
         gan_err_register ( "gan_symmatf_eigen", GAN_ERROR_FAILURE, "" );
         return GAN_FALSE;
      }
   }
#endif /* #ifdef HAVE_LAPACK */

   /* sort eigenvalues & aigenvectors so that eigenvalues are in
      ascending order if requested */
   if ( sort && !sort_eigenstuff ( W, Z ) )
   {
      gan_err_register ( "gan_symmatf_eigen", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   /* success */
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
Gan_SquMatrix_f *
 gan_symmatf_form_gen ( Gan_SquMatrix_f *A, unsigned long size,
                        float *data, size_t data_size )
{
   if ( A == NULL )
   {
      /* dyamically allocate matrix */
      A = gan_malloc_object(Gan_SquMatrix_f);
      if ( A == NULL )
      {
         gan_err_flush_trace();
         gan_err_register_with_number ( "gan_symmatf_form_gen", GAN_ERROR_MALLOC_FAILED, "", sizeof(*A) );
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
         A->data_size = size*(size+1)/2;
      else
      {
         /* a non-zero data size has been requested */
         assert ( data_size >= size*(size+1)/2 );
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
            gan_err_register_with_number ( "gan_symmatf_form_gen", GAN_ERROR_MALLOC_FAILED, "", A->data_size*sizeof(float) );
            return NULL;
         }
      }

      A->data_alloc = GAN_TRUE;
   }
   else
   {
      /* data for this matrix is provided */
      gan_err_test_ptr ( data_size >= size*(size+1)/2, "gan_symmatf_form_gen",
                         GAN_ERROR_INCOMPATIBLE, "" );

      /* set data pointer in matrix */
      A->data = data;
      A->data_size = data_size;

      /* we shouldn't free the matrix data */
      A->data_alloc = GAN_FALSE;
   }

   /* set type and class member functions */
   if ( gan_symmatf_set_size ( A, size ) == NULL )
   {
      gan_err_register ( "gan_symmatf_form_gen", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

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
 * \brief Fill symmetric matrix with values from variable argument list.
 * \return Result matrix \a A.
 *
 * Fill symmetric matrix \a A with values from variable argument list
 * \a aptr, and set the size of \a A to \a size.
 */
Gan_SquMatrix_f *
 gan_symmatf_fill_vap ( Gan_SquMatrix_f *A, unsigned long size,
                        va_list *aptr )
{
   unsigned int i, j;

   A = gan_symmatf_set_size ( A, size );
   if ( A == NULL )
   {
      gan_err_register ( "gan_symmatf_fill_vap", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* read variable argument list of matrix elements, lower triangle only */
   for ( i = 0; i < size; i++ )
      for ( j = 0; j <= i; j++ )
         A->data[i*(i+1)/2+j] = (float)va_arg ( *aptr, double );

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

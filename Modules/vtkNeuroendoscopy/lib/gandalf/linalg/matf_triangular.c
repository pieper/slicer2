/**
 * File:          $RCSfile: matf_triangular.c,v $
 * Module:        Lower/upper triangular matrices (single precision)
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
#include <gandalf/linalg/matf_triangular.h>
#include <gandalf/linalg/matf_symmetric.h>
#include <gandalf/linalg/matf_gen.h>
#include <gandalf/linalg/vecf_gen.h>
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

/* function to set element i,j of lower triangular matrix */
static Gan_Bool
 lower_tri_matrix_set_el ( Gan_SquMatrix_f *L,
                           unsigned i, unsigned j, float value )
{
   /* consistency check */
   assert ( L->type == GAN_LOWER_TRI_MATRIX );

   /* make sure element is inside lower triangular part of matrix */
   assert ( i < L->size && j <= i );

   /* set i,j element to given value */
   L->data[i*(i+1)/2+j] = value;

   /* return with success */
   return GAN_TRUE;
}

/* function to return element i,j of lower triangular matrix */
static float
 lower_tri_matrix_get_el ( const Gan_SquMatrix_f *L, unsigned i, unsigned j )
{
   /* consistency check */
   assert ( L->type == GAN_LOWER_TRI_MATRIX );

   /* make sure element is inside lower triangular part of matrix */
   assert ( i < L->size && j <= i );

   /* get i,j element */
   return L->data[i*(i+1)/2+j];
}

/* function to increment element i,j of lower triangular matrix by given
 * value */
static Gan_Bool
 lower_tri_matrix_inc_el ( Gan_SquMatrix_f *L,
                           unsigned i, unsigned j, float value )
{
   /* consistency check */
   assert ( L->type == GAN_LOWER_TRI_MATRIX );

   /* make sure element is inside lower triangular part of matrix */
   assert ( i < L->size && j <= i );

   /* increment i,j element by given value */
   L->data[i*(i+1)/2+j] += value;

   /* return with success */
   return GAN_TRUE;
}

/* function to decrement element i,j of lower triangular matrix by given
 * value
 */
static Gan_Bool
 lower_tri_matrix_dec_el ( Gan_SquMatrix_f *L,
                           unsigned i, unsigned j, float value )
{
   /* consistency check */
   assert ( L->type == GAN_LOWER_TRI_MATRIX );

   /* make sure element is inside lower triangular part of matrix */
   assert ( i < L->size && j <= i );

   /* decrement i,j element by given value */
   L->data[i*(i+1)/2+j] -= value;

   /* return with success */
   return GAN_TRUE;
}

/* function to set element i,j of upper triangular matrix */
static Gan_Bool
 upper_tri_matrix_set_el ( Gan_SquMatrix_f *U,
                           unsigned i, unsigned j, float value )
{
   /* consistency check */
   assert ( U->type == GAN_UPPER_TRI_MATRIX );

   /* make sure element is inside upper triangular part of matrix */
   assert ( j < U->size && i <= j );

   /* set i,j element to given value */
   U->data[j*(j+1)/2+i] = value;

   /* return with success */
   return GAN_TRUE;
}

/* function to return element i,j of upper triangular matrix */
static float
 upper_tri_matrix_get_el ( const Gan_SquMatrix_f *U, unsigned i, unsigned j )
{
   /* consistency check */
   assert ( U->type == GAN_UPPER_TRI_MATRIX );

   /* make sure element is inside upper triangular part of matrix */
   assert ( j < U->size && i <= j );

   /* get i,j element */
   return U->data[j*(j+1)/2+i];
}

/* function to increment element i,j of upper triangular matrix by given
 * value */
static Gan_Bool
 upper_tri_matrix_inc_el ( Gan_SquMatrix_f *U,
                           unsigned i, unsigned j, float value )
{
   /* consistency check */
   assert ( U->type == GAN_UPPER_TRI_MATRIX );

   /* make sure element is inside upper triangular part of matrix */
   assert ( j < U->size && i <= j );

   /* increment i,j element by given value */
   U->data[j*(j+1)/2+i] += value;

   /* return with success */
   return GAN_TRUE;
}

/* function to decrement element i,j of upper triangular matrix by given
 * value */
static Gan_Bool
 upper_tri_matrix_dec_el ( Gan_SquMatrix_f *U,
                           unsigned i, unsigned j, float value )
{
   /* consistency check */
   assert ( U->type == GAN_UPPER_TRI_MATRIX );

   /* make sure element is inside upper triangular part of matrix */
   assert ( j < U->size && i <= j );

   /* decrement i,j element by given value */
   U->data[j*(j+1)/2+i] -= value;

   /* return with success */
   return GAN_TRUE;
}

/* function to print lower triangular matrix to file pointer */
static Gan_Bool
 lower_tri_matrix_fprint ( FILE *fp, const Gan_SquMatrix_f *L, const char *prefix,
                           unsigned indent, const char *fmt )
{
   unsigned long i, j;
   int p;

   /* consistency check */
   gan_err_test_bool ( L->type == GAN_LOWER_TRI_MATRIX,
                       "lower_tri_matrix_fprint", GAN_ERROR_INCOMPATIBLE, "" );

   /* print indentation on first line */
   for ( p = indent-1; p >= 0; p-- ) fprintf ( fp, " " );
   fprintf ( fp, "%s (%0ldx%0ld lower triangular matrix)\n", prefix,
             L->size, L->size );

   for ( i = 0; i < L->size; i++ )
   {
      for ( p = indent-1; p >= 0; p-- ) fprintf ( fp, " " );
      for ( j = 0; j < L->size; j++ )
      {
         fprintf ( fp, " " );
         if ( j <= i )
            fprintf ( fp, fmt, L->data[i*(i+1)/2+j] );
         else
            fprintf ( fp, fmt, 0.0F );
      }

      fprintf ( fp, "\n" );
   }

   /* return with success */
   return GAN_TRUE;
}

/* function to print upper triangular matrix to file pointer */
static Gan_Bool
 upper_tri_matrix_fprint ( FILE *fp, const Gan_SquMatrix_f *U, const char *prefix,
                           unsigned indent, const char *fmt )
{
   unsigned long i, j;
   int p;

   /* consistency check */
   gan_err_test_bool ( U->type == GAN_UPPER_TRI_MATRIX,
                       "upper_tri_matrix_fprint", GAN_ERROR_INCOMPATIBLE, "" );

   /* print indentation on first line */
   for ( p = indent-1; p >= 0; p-- ) fprintf ( fp, " " );
   fprintf ( fp, "%s (%0ldx%0ld upper triangular matrix)\n", prefix,
             U->size, U->size );

   for ( i = 0; i < U->size; i++ )
   {
      for ( p = indent-1; p >= 0; p-- ) fprintf ( fp, " " );
      for ( j = 0; j < U->size; j++ )
      {
         fprintf ( fp, " " );
         if ( j < i )
            fprintf ( fp, fmt, 0.0F );
         else
            fprintf ( fp, fmt, U->data[j*(j+1)/2+i] );
      }

      fprintf ( fp, "\n" );
   }

   /* return with success */
   return GAN_TRUE;
}

/* function to copy lower triangular matrix A into another matrix B */
static Gan_SquMatrix_f *
 lower_tri_matrix_copy ( const Gan_SquMatrix_f *A, Gan_SquMatrix_f *B )
{
   /* consistency check */
   gan_err_test_ptr ( A->type == GAN_LOWER_TRI_MATRIX,
                      "lower_tri_matrix_copy", GAN_ERROR_INCOMPATIBLE, "" );

   /* allocate matrix B if necessary */
   if ( B == NULL )
      B = gan_ltmatf_alloc ( A->size );
   else
      B = gan_ltmatf_set_size ( B, A->size );

   if ( B == NULL )
   {
      gan_err_register ( "lower_tri_matrix_copy", GAN_ERROR_FAILURE, "" );
      return NULL;
   }
   
#ifdef HAVE_LAPACK
   {
      long nel = A->size*(A->size+1)/2, onei = 1;

      /* copy matrix */
      scopy_ ( &nel, A->data, &onei, B->data, &onei );
   }
#else /* !HAVE_LAPACK */
   /* copy matrix */
   if ( !gan_scopy ( A->size*(A->size+1)/2, A->data, 1, B->data, 1 ) )
   {
      gan_err_flush_trace();
      gan_err_register ( "lower_tri_matrix_copy", GAN_ERROR_FAILURE, "" );
      return NULL;
   }
#endif /* #ifdef HAVE_LAPACK */
   
   return B;
}

/* function to copy upper triangular matrix A into another matrix B */
static Gan_SquMatrix_f *
 upper_tri_matrix_copy ( const Gan_SquMatrix_f *A, Gan_SquMatrix_f *B )
{
   /* consistency check */
   gan_err_test_ptr ( A->type == GAN_UPPER_TRI_MATRIX,
                      "upper_tri_matrix_copy", GAN_ERROR_INCOMPATIBLE, "" );

   /* allocate matrix B if necessary */
   if ( B == NULL )
      B = gan_utmatf_alloc ( A->size );
   else
      B = gan_utmatf_set_size ( B, A->size );

   if ( B == NULL )
   {
      gan_err_register ( "upper_tri_matrix_copy", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

#ifdef HAVE_LAPACK
   {
      long nel = A->size*(A->size+1)/2, onei = 1;

      /* copy matrix */
      scopy_ ( &nel, A->data, &onei, B->data, &onei );
   }
#else /* !HAVE_LAPACK */
   /* copy matrix */
   if ( !gan_scopy ( A->size*(A->size+1)/2, A->data, 1, B->data, 1 ) )
   {
      gan_err_flush_trace();
      gan_err_register ( "upper_tri_matrix_copy", GAN_ERROR_FAILURE, "" );
      return NULL;
   }
#endif /* #ifdef HAVE_LAPACK */

   return B;
}

/* function to rescale lower triangular matrix A into another matrix B */
static Gan_SquMatrix_f *
 lower_tri_matrix_scale ( Gan_SquMatrix_f *A, float a,
                          Gan_SquMatrix_f *B )
{
   /* consistency check */
   gan_err_test_ptr ( A->type == GAN_LOWER_TRI_MATRIX,
                      "lower_tri_matrix_scale", GAN_ERROR_INCOMPATIBLE, "" );

   /* allocate matrix B if necessary */
   if ( B == NULL )
      B = gan_ltmatf_alloc ( A->size );
   else
      B = gan_ltmatf_set_size ( B, A->size );

   if ( B == NULL )
   {
      gan_err_register ( "lower_tri_matrix_scale", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

#ifdef HAVE_LAPACK
   {
      long nel = A->size*(A->size+1)/2, onei = 1;

      /* copy and scale matrix */
      if(A!=B) scopy_ ( &nel, A->data, &onei, B->data, &onei );
      sscal_ ( &nel, &a, B->data, &onei );
   }
#else /* !HAVE_LAPACK */
   /* copy and scale matrix */
   if ( (A!=B && !gan_scopy ( A->size*(A->size+1)/2, A->data, 1, B->data, 1 )) ||
        !gan_sscal ( A->size*(A->size+1)/2, a, B->data, 1 ) )
   {
      gan_err_flush_trace();
      gan_err_register ( "lower_tri_matrix_scale", GAN_ERROR_FAILURE, "" );
      return NULL;
   }
#endif /* #ifdef HAVE_LAPACK */

   return B;
}

/* function to rescale upper triangular matrix A into another matrix B */
static Gan_SquMatrix_f *
 upper_tri_matrix_scale ( Gan_SquMatrix_f *A, float a,
                          Gan_SquMatrix_f *B )
{
   /* consistency check */
   gan_err_test_ptr ( A->type == GAN_UPPER_TRI_MATRIX,
                      "upper_tri_matrix_scale", GAN_ERROR_INCOMPATIBLE, "" );

   /* allocate matrix B if necessary */
   if ( B == NULL )
      B = gan_utmatf_alloc ( A->size );
   else
      B = gan_utmatf_set_size ( B, A->size );

   if ( B == NULL )
   {
      gan_err_register ( "upper_tri_matrix_scale", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

#ifdef HAVE_LAPACK
   {
      long nel = A->size*(A->size+1)/2, onei = 1;

      /* copy and scale matrix */
      if(A!=B) scopy_ ( &nel, A->data, &onei, B->data, &onei );
      sscal_ ( &nel, &a, B->data, &onei );
   }
#else /* !HAVE_LAPACK */
   /* copy and scale matrix */
   if ( (A!=B && !gan_scopy ( A->size*(A->size+1)/2, A->data, 1, B->data, 1 )) ||
        !gan_sscal ( A->size*(A->size+1)/2, a, B->data, 1 ) )
   {
      gan_err_flush_trace();
      gan_err_register ( "upper_tri_matrix_scale", GAN_ERROR_FAILURE, "" );
      return NULL;
   }
#endif /* #ifdef HAVE_LAPACK */

   return B;
}

/* function to add lower triangular matrices together C = A+B, returning C */
static Gan_SquMatrix_f *
 lower_tri_matrix_add ( Gan_SquMatrix_f *A, Gan_TposeFlag A_tr,
                        Gan_SquMatrix_f *B, Gan_TposeFlag B_tr,
                        Gan_SquMatrix_f *C )
{
   /* consistency check */
   gan_err_test_ptr ( A->type == GAN_LOWER_TRI_MATRIX &&
                      B->type == GAN_LOWER_TRI_MATRIX,
                      "lower_tri_matrix_add", GAN_ERROR_INCOMPATIBLE, "" );

   /* check that the type and dimensions of input matrices A & B are
      compatible */
   gan_err_test_ptr ( gan_squmatf_same_size(A,B), "lower_tri_matrix_add",
                      GAN_ERROR_INCOMPATIBLE, "" );

   /* allocate result matrix C if necessary */
   if ( C == NULL )
      C = gan_ltmatf_alloc ( A->size );
   else
      C = gan_ltmatf_set_size ( C, A->size );

   if ( C == NULL )
   {
      gan_err_register ( "lower_tri_matrix_add", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

#ifdef HAVE_LAPACK
   {
      /* number of matrix elements */
      long nel = A->size*(A->size+1)/2, onei = 1;
      float onef = 1.0F;

      /* add matrix data */
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
#else /* !HAVE_LAPACK */
   {
      Gan_Bool result;

      /* add matrix data */
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
         result = gan_scopy ( A->size*(A->size+1)/2, A->data, 1, C->data, 1 );
         result = (Gan_Bool) (result & gan_saxpy ( A->size*(A->size+1)/2, 1.0F,
                                                   B->data, 1, C->data, 1 ));
      }

      if ( !result )
      {
         gan_err_flush_trace();
         gan_err_register ( "lower_tri_matrix_add", GAN_ERROR_FAILURE, "" );
         return NULL;
      }
   }
#endif /* #ifdef HAVE_LAPACK */

   /* return result matrix */
   return C;
}

/* function to add upper triangular matrices together C = A+B, returning C */
static Gan_SquMatrix_f *
 upper_tri_matrix_add ( Gan_SquMatrix_f *A, Gan_TposeFlag A_tr,
                        Gan_SquMatrix_f *B, Gan_TposeFlag B_tr,
                        Gan_SquMatrix_f *C )
{
   /* consistency check */
   gan_err_test_ptr ( A->type == GAN_UPPER_TRI_MATRIX && !A_tr &&
                      B->type == GAN_UPPER_TRI_MATRIX && !B_tr,
                      "upper_tri_matrix_add", GAN_ERROR_INCOMPATIBLE, "" );

   /* check that the type and dimensions of input matrices A & B are
      compatible */
   gan_err_test_ptr ( gan_squmatf_same_size(A,B), "upper_tri_matrix_add",
                      GAN_ERROR_INCOMPATIBLE, "" );

   /* allocate result matrix C if necessary */
   if ( C == NULL )
      C = gan_utmatf_alloc ( A->size );
   else
      C = gan_utmatf_set_size ( C, A->size );

   if ( C == NULL )
   {
      gan_err_register ( "upper_tri_matrix_add", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

#ifdef HAVE_LAPACK
   {
      /* number of matrix elements */
      long nel = A->size*(A->size+1)/2, onei = 1;
      float onef = 1.0F;

      /* add matrix data */
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
#else /* !HAVE_LAPACK */
   {
      Gan_Bool result;

      /* add matrix data */
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
         result = gan_scopy ( A->size*(A->size+1)/2, A->data, 1, C->data, 1 );
         result = (Gan_Bool) (result & gan_saxpy ( A->size*(A->size+1)/2, 1.0F,
                                                   B->data, 1, C->data, 1 ));
      }

      if ( !result )
      {
         gan_err_flush_trace();
         gan_err_register ( "upper_tri_matrix_add", GAN_ERROR_FAILURE, "" );
         return NULL;
      }
   }
#endif /* #ifdef HAVE_LAPACK */

   /* return result matrix */
   return C;
}

/* function to subtract lower triangular matrices from each other: C = A-B,
 * returning C
 */
static Gan_SquMatrix_f *
 lower_tri_matrix_sub ( Gan_SquMatrix_f *A, Gan_TposeFlag A_tr,
                        Gan_SquMatrix_f *B, Gan_TposeFlag B_tr,
                        Gan_SquMatrix_f *C )
{
   /* consistency check */
   gan_err_test_ptr ( A->type == GAN_LOWER_TRI_MATRIX && !A_tr &&
                      B->type == GAN_LOWER_TRI_MATRIX && !B_tr,
                      "lower_tri_matrix_sub", GAN_ERROR_INCOMPATIBLE, "" );

   /* check that the type and dimensions of input matrices A & B are
      compatible */
   gan_err_test_ptr ( gan_squmatf_same_size(A,B), "lower_tri_matrix_sub",
                      GAN_ERROR_INCOMPATIBLE, "" );

   /* allocate result matrix C if necessary */
   if ( C == NULL )
      C = gan_ltmatf_alloc ( A->size );
   else
      C = gan_ltmatf_set_size ( C, A->size );

   if ( C == NULL )
   {
      gan_err_register ( "lower_tri_matrix_sub", GAN_ERROR_FAILURE, "" );
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
#else /* !HAVE_LAPACK */
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
         result = (Gan_Bool) (result & gan_saxpy ( A->size*(A->size+1)/2, 1.0F,
                                                   A->data, 1, B->data, 1 ));
      }
      else
      {
         /* C = A - B */
         result = gan_scopy ( A->size*(A->size+1)/2, A->data, 1, C->data, 1 );
         result = (Gan_Bool) (result & gan_saxpy (A->size*(A->size+1)/2, -1.0F,
                                                  B->data, 1, C->data, 1 ));
      }

      if ( !result )
      {
         gan_err_flush_trace();
         gan_err_register ( "lower_tri_matrix_sub", GAN_ERROR_FAILURE, "" );
         return NULL;
      }
   }
#endif /* #ifdef HAVE_LAPACK */

   /* return result matrix */
   return C;
}

/* function to subtract upper triangular matrices from each other: C = A-B,
 * returning C
 */
static Gan_SquMatrix_f *
 upper_tri_matrix_sub ( Gan_SquMatrix_f *A, Gan_TposeFlag A_tr,
                        Gan_SquMatrix_f *B, Gan_TposeFlag B_tr,
                        Gan_SquMatrix_f *C )
{
   /* consistency check */
   gan_err_test_ptr ( A->type == GAN_UPPER_TRI_MATRIX && !A_tr &&
                      B->type == GAN_UPPER_TRI_MATRIX && !B_tr,
                      "upper_tri_matrix_sub", GAN_ERROR_INCOMPATIBLE, "" );

   /* check that the type and dimensions of input matrices A & B are
      compatible */
   gan_err_test_ptr ( gan_squmatf_same_size(A,B), "upper_tri_matrix_sub",
                      GAN_ERROR_INCOMPATIBLE, "" );

   /* allocate result matrix C if necessary */
   if ( C == NULL )
      C = gan_utmatf_alloc ( A->size );
   else
      C = gan_utmatf_set_size ( C, A->size );

   if ( C == NULL )
   {
      gan_err_register ( "upper_tri_matrix_sub", GAN_ERROR_FAILURE, "" );
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
#else /* !HAVE_LAPACK */
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
         result = (Gan_Bool) (result & gan_saxpy ( A->size*(A->size+1)/2, 1.0F,
                                                   A->data, 1, B->data, 1 ));
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
         gan_err_register ( "upper_tri_matrix_sub", GAN_ERROR_FAILURE, "" );
         return NULL;
      }
   }
#endif /* #ifdef HAVE_LAPACK */

   /* return result matrix */
   return C;
}

/* function to compute matrix/vector product L*x = y, returning y */
static Gan_Vector_f *
 lower_tri_matrix_multv (
     const Gan_SquMatrix_f *L, Gan_TposeFlag A_tr, Gan_InvertFlag A_inv,
     Gan_Vector_f *x, Gan_Vector_f *y )
{
   float total;
   long i, j;
#ifdef HAVE_LAPACK
   long onei = 1;
#endif

   /* consistency check */
   gan_err_test_ptr ( L->type == GAN_LOWER_TRI_MATRIX,
                      "lower_tri_matrix_multv", GAN_ERROR_INCOMPATIBLE, "" );

   /* check that the dimensions of input matrix A and vector x are
      compatible */
   gan_err_test_ptr ( L->size == x->rows, "lower_tri_matrix_multv",
                      GAN_ERROR_INCOMPATIBLE, "" );

   /* allocate result vector y if necessary */
   if ( y == NULL )
      y = gan_vecf_alloc ( L->size );
   else
      y = gan_vecf_set_size ( y, L->size );

   if ( y == NULL )
   {
      gan_err_register ( "lower_tri_matrix_multv", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   if ( !A_tr && !A_inv )
      /* compute matrix/vector product y = L*x */
      for ( i = (long)x->rows-1; i >= 0; i-- )
      {
         total = 0.0F;
         for ( j = 0; j <= i; j++ )
            total += L->data[i*(i+1)/2+j]*x->data[j];

         y->data[i] = total;
      }
   else if ( !A_tr )
#ifdef HAVE_LAPACK
   {
      /* compute matrix/vector product y = L^-1*x */
      if ( x != y ) scopy_ ( (long *)&x->rows, x->data, &onei, y->data, &onei);
      stpsv_ ( "U", "T", "N", (long *)&y->rows, L->data, y->data, &onei );
   }
#else /* !HAVE_LAPACK */
   {
      /* compute matrix/vector product y = L^-1*x */
      if ( x != y ) gan_scopy ( x->rows, x->data, 1, y->data, 1 );
      gan_stpsv ( GAN_MATRIXPART_UPPER, GAN_TRANSPOSE, GAN_NOUNIT, y->rows, L->data,
                  y->data, 1 );
   }
#endif /* #ifdef HAVE_LAPACK */
   else if ( !A_inv )
      /* compute matrix/vector product y = L^T*x */
      for ( i = 0; i < (long)x->rows; i++ )
      {
         total = 0.0F;
         for ( j = (long)x->rows-1; j >= i; j-- )
            total += L->data[j*(j+1)/2+i]*x->data[j];

         y->data[i] = total;
      }
   else /* A_tr && A_inv */
   {
#ifdef HAVE_LAPACK
      /* compute matrix/vector product y = L^-T*x */
      if ( x != y ) scopy_ ( (long *)&x->rows, x->data, &onei, y->data, &onei);
      stpsv_ ( "U", "N", "N", (long *)&y->rows, L->data, y->data, &onei );
#else /* !HAVE_LAPACK */
      /* compute matrix/vector product y = L^-T*x */
      if ( x != y )
         gan_scopy ( x->rows, x->data, 1, y->data, 1 );

      gan_stpsv ( GAN_MATRIXPART_UPPER, GAN_NOTRANSPOSE, GAN_NOUNIT, y->rows, L->data,
                  y->data, 1 );
#endif /* #ifdef HAVE_LAPACK */
   }

   /* return result vector */
   return y;
}

/* function to compute matrix/vector product L*x = y, returning y */
static Gan_Vector_f *
 upper_tri_matrix_multv (
     const Gan_SquMatrix_f *U, Gan_TposeFlag A_tr, Gan_InvertFlag A_inv,
     Gan_Vector_f *x, Gan_Vector_f *y )
{
   float total;
   long i, j;
#ifdef HAVE_LAPACK
   long onei = 1;
#endif

   /* consistency check */
   gan_err_test_ptr ( U->type == GAN_UPPER_TRI_MATRIX,
                      "upper_tri_matrix_multv", GAN_ERROR_INCOMPATIBLE, "" );

   /* check that the dimensions of input matrix A and vector x are
      compatible */
   gan_err_test_ptr ( U->size == x->rows, "upper_tri_matrix_multv",
                      GAN_ERROR_INCOMPATIBLE, "" );

   /* allocate result vector y if necessary */
   if ( y == NULL )
      y = gan_vecf_alloc ( U->size );
   else
      y = gan_vecf_set_size ( y, U->size );

   if ( y == NULL )
   {
      gan_err_register ( "upper_tri_matrix_multv", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   if ( !A_tr && !A_inv )
      /* compute matrix/vector product y = U*x */
      for ( i = 0; i < (long)x->rows; i++ )
      {
         total = 0.0F;
         for ( j = (long)x->rows-1; j >= i; j-- )
            total += U->data[j*(j+1)/2+i]*x->data[j];

         y->data[i] = total;
      }
   else if ( !A_tr )
   {
#ifdef HAVE_LAPACK
      /* compute matrix/vector product y = U^-1*x */
      if ( x != y ) scopy_ ( (long *)&x->rows, x->data, &onei, y->data, &onei);
      stpsv_ ( "U", "N", "N", (long *)&y->rows, U->data, y->data, &onei );
#else /* !HAVE_LAPACK */
      /* compute matrix/vector product y = U^-1*x */
      if ( x != y ) gan_scopy ( x->rows, x->data, 1, y->data, 1 );
      gan_stpsv ( GAN_MATRIXPART_UPPER, GAN_NOTRANSPOSE, GAN_NOUNIT, y->rows, U->data,
                  y->data, 1 );
#endif /* #ifdef HAVE_LAPACK */
   }
   else if ( !A_inv )
      /* compute matrix/vector product y = L^T*x */
      for ( i = (long)x->rows-1; i >= 0; i-- )
      {
         total = 0.0F;
         for ( j = 0; j <= i; j++ )
            total += U->data[i*(i+1)/2+j]*x->data[j];

         y->data[i] = total;
      }
   else /* A_tr && A_inv */
   {
#ifdef HAVE_LAPACK
      /* compute matrix/vector product y = U^-T*x */
      if ( x != y ) scopy_ ( (long *)&x->rows, x->data, &onei, y->data, &onei);
      stpsv_ ( "U", "T", "N", (long *)&y->rows, U->data, y->data, &onei );
#else /* !HAVE_LAPACK */
      /* compute matrix/vector product y = U^-T*x */
      if ( x != y ) gan_scopy ( x->rows, x->data, 1, y->data, 1 );
      gan_stpsv ( GAN_MATRIXPART_UPPER, GAN_TRANSPOSE, GAN_NOUNIT, y->rows, U->data,
                  y->data, 1 );
#endif /* #ifdef HAVE_LAPACK */
   }

   /* return result vector */
   return y;
}

/* function to multipy lower triangular matrix L and generic matrix B:
 * C = L(^-1)(^T)*B(^T), returning C.
 */
static Gan_Matrix_f *
 lower_tri_matrix_rmult (
     const Gan_SquMatrix_f *L, Gan_TposeFlag L_tr, Gan_InvertFlag L_inv,
     Gan_Matrix_f    *B, Gan_TposeFlag B_tr, Gan_Matrix_f *C )
{
   unsigned long Ccols = B_tr ? B->rows : B->cols;
   long     i, j, k;
   float total, *Cptr;
#ifdef HAVE_LAPACK
   long nel = L->size*Ccols; /* number of matrix elements */
   long onei = 1;
#endif

   /* consistency check */
   gan_err_test_ptr ( L->type == GAN_LOWER_TRI_MATRIX,
                      "lower_tri_matrix_rmult", GAN_ERROR_INCOMPATIBLE, "" );

   /* check that the dimensions of input matrices L,B are compatible */
   gan_err_test_ptr ( L->size == (B_tr ? B->cols : B->rows),
                      "lower_tri_matrix_rmult", GAN_ERROR_INCOMPATIBLE, "" );

   /* allocate result matrix C if necessary */
   if ( C == NULL )
      C = gan_matf_alloc ( L->size, Ccols );
   else
      C = gan_matf_set_dims ( C, L->size, Ccols );

   if ( C == NULL )
   {
      gan_err_register ( "lower_tri_matrix_rmult", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   if ( B_tr )
   {
      /* can't do in-place matrix multiply with transpose */
      assert ( B != C );

      if ( !L_tr && !L_inv )
         /* compute matrix/vector product C = L*B^T */
         for ( j = 0, Cptr = C->data; (unsigned long) j < Ccols; j++ )
            for ( i = 0; (unsigned long) i < L->size; i++ )
            {
               total = 0.0F;
               for ( k = 0; k <= i; k++ )
                  total += L->data[i*(i+1)/2+k]*B->data[k*B->rows+j];

               *Cptr++ = total;
            }
      else if ( !L_tr )
      {
         /* compute matrix/vector product C = L^-1*B^T */
         gan_matf_tpose_q ( B, C );
#ifdef HAVE_LAPACK
         for ( j = 0, Cptr = C->data; (unsigned long) j < C->cols;
               j++, Cptr += C->rows )
            stpsv_ ( "U", "T", "N", (long *)&C->rows, L->data, Cptr, &onei );
#else /* !HAVE_LAPACK */
         for ( j = 0, Cptr = C->data; (unsigned long) j < C->cols;
               j++, Cptr += C->rows )
            gan_stpsv ( GAN_MATRIXPART_UPPER, GAN_TRANSPOSE, GAN_NOUNIT, C->rows, L->data,
                        Cptr, 1 );
#endif /* #ifdef HAVE_LAPACK */
      }
      else if ( !L_inv )
         /* compute matrix/vector product C = L^T*B^T */
         for ( j = 0, Cptr = C->data; (unsigned long) j < Ccols; j++ )
            for ( i = 0; (unsigned long) i < L->size; i++ )
            {
               total = 0.0F;
               for ( k = i; (unsigned long) k < L->size; k++ )
                  total += L->data[k*(k+1)/2+i]*B->data[k*B->rows+j];

               *Cptr++ = total;
            }
      else /* L_tr && L_inv */
      {
         /* compute matrix/vector product y = L^-T*B^T */
         gan_matf_tpose_q ( B, C );
#ifdef HAVE_LAPACK
         for ( j = 0, Cptr = C->data; (unsigned long) j < C->cols;
               j++, Cptr += C->rows )
            stpsv_ ( "U", "N", "N", (long *)&C->rows, L->data, Cptr, &onei );
#else /* !HAVE_LAPACK */
         for ( j = 0, Cptr = C->data; (unsigned long) j < C->cols;
               j++, Cptr += C->rows )
            gan_stpsv ( GAN_MATRIXPART_UPPER, GAN_NOTRANSPOSE, GAN_NOUNIT, C->rows,
                        L->data, Cptr, 1 );
#endif /* #ifdef HAVE_LAPACK */
      }
   }
   else
      if ( !L_tr && !L_inv )
         /* compute matrix/vector product C = L*B */
         for ( j = 0; (unsigned long) j < Ccols; j++ )
            for ( i = (long)L->size-1; i >= 0; i-- )
            {
               total = 0.0F;
               for ( k = 0; k <= i; k++ )
                  total += L->data[i*(i+1)/2+k]*B->data[j*B->rows+k];

               C->data[j*C->rows+i] = total;
            }
      else if ( !L_tr )
      {
         /* compute matrix/vector product C = L^-1*B */
#ifdef HAVE_LAPACK
         if ( B != C ) scopy_ ( &nel, B->data, &onei, C->data, &onei );
         for ( j = 0, Cptr = C->data; (unsigned long) j < C->cols;
               j++, Cptr += C->rows )
            stpsv_ ( "U", "T", "N", (long *)&C->rows, L->data, Cptr, &onei );
#else /* !HAVE_LAPACK */
         if ( B != C )
            gan_scopy ( L->size*Ccols, B->data, 1, C->data, 1 );

         for ( j = 0, Cptr = C->data; (unsigned long) j < C->cols;
               j++, Cptr += C->rows )
            gan_stpsv ( GAN_MATRIXPART_UPPER, GAN_TRANSPOSE, GAN_NOUNIT, C->rows, L->data,
                        Cptr, 1 );
#endif /* #ifdef HAVE_LAPACK */
      }
      else if ( !L_inv )
         /* compute matrix/vector product C = L^T*B */
         for ( j = 0, Cptr = C->data; (unsigned long) j < Ccols; j++ )
            for ( i = 0; (unsigned long) i < L->size; i++ )
            {
               total = 0.0F;
               for ( k = i; (unsigned long) k < L->size; k++ )
                  total += L->data[k*(k+1)/2+i]*B->data[j*B->rows+k];

               *Cptr++ = total;
            }
      else /* L_tr && L_inv */
      {
         /* compute matrix/vector product C = L^-T*B */
#ifdef HAVE_LAPACK
         if ( B != C ) scopy_ ( &nel, B->data, &onei, C->data, &onei );
         for ( j = 0, Cptr = C->data; (unsigned long) j < C->cols;
               j++, Cptr += C->rows )
            stpsv_ ( "U", "N", "N", (long *)&C->rows, L->data, Cptr, &onei );
#else /* !HAVE_LAPACK */
         if ( B != C )
            gan_scopy ( L->size*Ccols, B->data, 1, C->data, 1 );

         for ( j = 0, Cptr = C->data; (unsigned long) j < C->cols;
               j++, Cptr += C->rows )
            gan_stpsv ( GAN_MATRIXPART_UPPER, GAN_NOTRANSPOSE, GAN_NOUNIT, C->rows,
                        L->data, Cptr, 1 );
#endif /* #ifdef HAVE_LAPACK */
      }

   /* return result matrix */
   return C;
}

/* function to multipy upper triangular matrix U and generic matrix B:
 * C = U(^-1)(^T)*B(^T), returning C.
 */
static Gan_Matrix_f *
 upper_tri_matrix_rmult (
     const Gan_SquMatrix_f *U, Gan_TposeFlag U_tr, Gan_InvertFlag U_inv,
     Gan_Matrix_f          *B, Gan_TposeFlag B_tr, Gan_Matrix_f *C )
{
   unsigned long Ccols = B_tr ? B->rows : B->cols;
   long     i, j, k;
   float total, *Cptr;
#ifdef HAVE_LAPACK
   long nel = U->size*Ccols; /* number of matrix elements */
   long onei = 1;
#endif

   /* consistency check */
   assert ( U->type == GAN_UPPER_TRI_MATRIX );

   /* check that the dimensions of input matrices L,B are compatible */
   gan_err_test_ptr ( U->size == (B_tr ? B->cols : B->rows),
                      "upper_tri_matrix_rmult", GAN_ERROR_INCOMPATIBLE, "" );

   /* allocate result matrix C if necessary */
   if ( C == NULL )
      C = gan_matf_alloc ( U->size, Ccols );
   else
      C = gan_matf_set_dims ( C, U->size, Ccols );

   if ( C == NULL )
   {
      gan_err_register ( "upper_tri_matrix_rmult", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   if ( B_tr )
   {
      /* can't do in-place matrix multiply with transpose */
      assert ( B != C );

      if ( !U_tr && !U_inv )
         /* compute matrix/vector product C = U*B^T */
         for ( j = 0, Cptr = C->data; (unsigned long) j < Ccols; j++ )
            for ( i = 0; (unsigned long) i < U->size; i++ )
            {
               total = 0.0F;
               for ( k = i; (unsigned long) k < U->size; k++ )
                  total += U->data[k*(k+1)/2+i]*B->data[k*B->rows+j];

               *Cptr++ = total;
            }
      else if ( !U_tr )
      {
         /* compute matrix/vector product y = U^-1*B^T */
         gan_matf_tpose_q ( B, C );
#ifdef HAVE_LAPACK
         for ( j = 0, Cptr = C->data; (unsigned long) j < C->cols;
               j++, Cptr += C->rows )
            stpsv_ ( "U", "N", "N", (long *)&C->rows, U->data, Cptr, &onei );
#else /* !HAVE_LAPACK */
         for ( j = 0, Cptr = C->data; (unsigned long) j < C->cols;
               j++, Cptr += C->rows )
            gan_stpsv ( GAN_MATRIXPART_UPPER, GAN_NOTRANSPOSE, GAN_NOUNIT, C->rows,
                        U->data, Cptr, 1 );
#endif /* #ifdef HAVE_LAPACK */
      }
      else if ( !U_inv )
         /* compute matrix/vector product C = U^T*B^T */
         for ( j = 0, Cptr = C->data; (unsigned long) j < Ccols; j++ )
            for ( i = 0; (unsigned long) i < U->size; i++ )
            {
               total = 0.0F;
               for ( k = 0; k <= i; k++ )
                  total += U->data[i*(i+1)/2+k]*B->data[k*B->rows+j];

               *Cptr++ = total;
            }
      else /* U_tr && U_inv */
      {
         /* compute matrix/vector product C = U^-T*B^T */
         gan_matf_tpose_q ( B, C );
#ifdef HAVE_LAPACK
         for ( j = 0, Cptr = C->data; (unsigned long) j < C->cols;
               j++, Cptr += C->rows )
            stpsv_ ( "U", "T", "N", (long *)&C->rows, U->data, Cptr, &onei );
#else /* !HAVE_LAPACK */
         for ( j = 0, Cptr = C->data; (unsigned long) j < C->cols;
               j++, Cptr += C->rows )
            gan_stpsv ( GAN_MATRIXPART_UPPER, GAN_TRANSPOSE, GAN_NOUNIT, C->rows,
                        U->data, Cptr, 1 );
#endif /* #ifdef HAVE_LAPACK */
      }
   }
   else
      if ( !U_tr && !U_inv )
         /* compute matrix/vector product C = U*B */
         for ( j = 0, Cptr = C->data; (unsigned long) j < Ccols; j++ )
            for ( i = 0; (unsigned long) i < U->size; i++ )
            {
               total = 0.0F;
               for ( k = i; (unsigned long) k < U->size; k++ )
                  total += U->data[k*(k+1)/2+i]*B->data[j*B->rows+k];

               *Cptr++ = total;
            }
      else if ( !U_tr )
      {
         /* compute matrix/vector product C = U^-1*B */
#ifdef HAVE_LAPACK
         if ( B != C ) scopy_ ( &nel, B->data, &onei, C->data, &onei );
         for ( j = 0, Cptr = C->data; (unsigned long) j < C->cols;
               j++, Cptr += C->rows )
            stpsv_ ( "U", "N", "N", (long *)&C->rows, U->data, Cptr, &onei );
#else /* !HAVE_LAPACK */
         if ( B != C )
            gan_scopy ( U->size*Ccols, B->data, 1, C->data, 1 );

         for ( j = 0, Cptr = C->data; (unsigned long) j < C->cols;
               j++, Cptr += C->rows )
            gan_stpsv ( GAN_MATRIXPART_UPPER, GAN_NOTRANSPOSE, GAN_NOUNIT, C->rows,
                        U->data, Cptr, 1 );
#endif /* #ifdef HAVE_LAPACK */
      }
      else if ( !U_inv )
         /* compute matrix/vector product C = U^T*B */
         for ( j = (long)Ccols-1; j >= 0; j-- )
            for ( i = (long)U->size-1; i >= 0; i-- )
            {
               total = 0.0F;
               for ( k = 0; k <= i; k++ )
                  total += U->data[i*(i+1)/2+k]*B->data[j*B->rows+k];

               C->data[j*C->rows+i] = total;
            }
      else /* U_tr && U_inv */
      {
         /* compute matrix/vector product C = U^-T*B */
#ifdef HAVE_LAPACK
         if ( B != C ) scopy_ ( &nel, B->data, &onei, C->data, &onei );
         for ( j = 0, Cptr = C->data; (unsigned long) j < C->cols;
               j++, Cptr += C->rows )
            stpsv_ ( "U", "T", "N", (long *)&C->rows, U->data, Cptr, &onei );
#else /* !HAVE_LAPACK */
         if ( B != C )
            gan_scopy ( U->size*Ccols, B->data, 1, C->data, 1 );

         for ( j = 0, Cptr = C->data; (unsigned long) j < C->cols;
               j++, Cptr += C->rows )
            gan_stpsv ( GAN_MATRIXPART_UPPER, GAN_TRANSPOSE, GAN_NOUNIT, C->rows,
                        U->data, Cptr, 1 );
#endif /* #ifdef HAVE_LAPACK */
      }

   /* return result matrix */
   return C;
}

/* function to multipy lower triangular matrix L and generic matrix B:
 * C = B(^T)*L(^-1)(^T), returning C.
 */
static Gan_Matrix_f *
 lower_tri_matrix_lmult (
     const Gan_SquMatrix_f *L, Gan_TposeFlag L_tr, Gan_InvertFlag L_inv,
     Gan_Matrix_f          *B, Gan_TposeFlag B_tr, Gan_Matrix_f *C )
{
   unsigned long Crows = B_tr ? B->cols : B->rows;
   long     i, j, k;
   float   total, *Cptr;
#ifdef HAVE_LAPACK
   long nel = Crows*L->size; /* number of matrix elements */
   long onei = 1;
#endif

   /* consistency check */
   assert ( L->type == GAN_LOWER_TRI_MATRIX );

   /* check that the dimensions of input matrices L,B are compatible */
   gan_err_test_ptr ( L->size == (B_tr ? B->rows : B->cols),
                      "lower_tri_matrix_lmult", GAN_ERROR_INCOMPATIBLE, "" );

   /* allocate result matrix C if necessary */
   if ( C == NULL )
      C = gan_matf_alloc ( Crows, L->size );
   else
      C = gan_matf_set_dims ( C, Crows, L->size );

   if ( C == NULL )
   {
      gan_err_register ( "lower_tri_matrix_lmult", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   if ( B_tr )
   {
      /* can't do in-place matrix multiply with transpose */
      assert ( B != C );

      if ( !L_tr && !L_inv )
         /* compute matrix/vector product C = B^T*L */
         for ( j = 0, Cptr = C->data; (unsigned long) j < L->size; j++ )
            for ( i = 0; (unsigned long) i < Crows; i++ )
            {
               total = 0.0F;
               for ( k = j; (unsigned long) k < L->size; k++ )
                  total += L->data[k*(k+1)/2+j]*B->data[i*B->rows+k];

               *Cptr++ = total;
            }
      else if ( !L_tr )
      {
         /* compute matrix/vector product C = B^T*L^-1 */
         gan_matf_tpose_q ( B, C );
#ifdef HAVE_LAPACK
         for ( i = 0, Cptr = C->data; (unsigned long) i < C->rows;
               i++, Cptr++ )
            stpsv_ ( "U", "N", "N", (long *)&C->cols, L->data,
                     Cptr, (long *)&C->rows );
#else /* !HAVE_LAPACK */
         for ( i = 0, Cptr = C->data; (unsigned long) i < C->rows;
               i++, Cptr++ )
            gan_stpsv ( GAN_MATRIXPART_UPPER, GAN_NOTRANSPOSE, GAN_NOUNIT, C->cols,
                        L->data, Cptr, C->rows );
#endif /* #ifdef HAVE_LAPACK */
      }
      else if ( !L_inv )
         /* compute matrix/vector product C = B^T*L^T */
         for ( j = (long)L->size-1; j >= 0; j-- )
            for ( i = (long)Crows-1; i >= 0; i-- )
            {
               total = 0.0F;
               for ( k = 0; k <= j; k++ )
                  total += L->data[j*(j+1)/2+k]*B->data[i*B->rows+k];

               C->data[j*Crows+i] = total;
            }
      else /* L_tr && L_inv */
      {
         /* compute matrix/vector product y = B^T*L^-T */
         gan_matf_tpose_q ( B, C );
#ifdef HAVE_LAPACK
         for ( i = 0, Cptr = C->data; (unsigned long) i < C->rows;
               i++, Cptr++ )
            stpsv_ ( "U", "T", "N", (long *)&C->cols, L->data,
                     Cptr, (long *)&C->rows );
#else /* !HAVE_LAPACK */
         for ( i = 0, Cptr = C->data; (unsigned long) i < C->rows;
               i++, Cptr++ )
            gan_stpsv ( GAN_MATRIXPART_UPPER, GAN_TRANSPOSE, GAN_NOUNIT, C->cols, L->data,
                        Cptr, C->rows );
#endif /* #ifdef HAVE_LAPACK */
      }
   }
   else
      if ( !L_tr && !L_inv )
         /* compute matrix/vector product C = B*L */
         for ( j = 0; j < (long)L->size; j++ )
            for ( i = 0; (unsigned long) i < Crows; i++ )
            {
               total = 0.0F;
               for ( k = j; (unsigned long) k < L->size; k++ )
                  total += L->data[k*(k+1)/2+j]*B->data[k*B->rows+i];

               C->data[j*Crows+i] = total;
            }
      else if ( !L_tr )
      {
         /* compute matrix/vector product C = B*L^-1 */
#ifdef HAVE_LAPACK
         if ( B != C ) scopy_ ( &nel, B->data, &onei, C->data, &onei );
         for ( i = 0, Cptr = C->data; (unsigned long) i < C->rows;
               i++, Cptr++ )
            stpsv_ ( "U", "N", "N", (long *)&C->cols, L->data,
                     Cptr, (long *)&C->rows );
#else /* !HAVE_LAPACK */
         if ( B != C ) gan_scopy ( Crows*L->size, B->data, 1, C->data, 1 );
         for ( i = 0, Cptr = C->data; (unsigned long) i < C->rows;
               i++, Cptr++ )
            gan_stpsv ( GAN_MATRIXPART_UPPER, GAN_NOTRANSPOSE, GAN_NOUNIT,
                        C->cols, L->data, Cptr, C->rows );
#endif /* #ifdef HAVE_LAPACK */
      }
      else if ( !L_inv )
         /* compute matrix/vector product C = B*L^T */
         for ( j = (long)L->size-1; j >= 0; j-- )
            for ( i = (long)Crows-1; i >= 0; i-- )
            {
               total = 0.0F;
               for ( k = 0; k <= j; k++ )
                  total += L->data[j*(j+1)/2+k]*B->data[k*B->rows+i];

               C->data[j*Crows+i] = total;
            }
      else /* L_tr && L_inv */
      {
         /* compute matrix/vector product C = B*L^-T */
#ifdef HAVE_LAPACK
         if ( B != C ) scopy_ ( &nel, B->data, &onei, C->data, &onei );
         for ( i = 0, Cptr = C->data; (unsigned long) i < C->rows;
               i++, Cptr++ )
            stpsv_ ( "U", "T", "N", (long *)&C->cols, L->data,
                     Cptr, (long *)&C->rows );
#else /* !HAVE_LAPACK */
         if ( B != C ) gan_scopy ( Crows*L->size, B->data, 1, C->data, 1 );
         for ( i = 0, Cptr = C->data; (unsigned long) i < C->rows;
               i++, Cptr++ )
            gan_stpsv ( GAN_MATRIXPART_UPPER, GAN_TRANSPOSE, GAN_NOUNIT, C->cols, L->data,
                        Cptr, C->rows );
#endif /* #ifdef HAVE_LAPACK */
      }

   /* return result matrix */
   return C;
}

/* function to multipy upper triangular matrix U and generic matrix B:
 * C = B(^T)*U(^-1)(^T), returning C.
 */
static Gan_Matrix_f *
 upper_tri_matrix_lmult (
     const Gan_SquMatrix_f *U, Gan_TposeFlag U_tr, Gan_InvertFlag U_inv,
     Gan_Matrix_f          *B, Gan_TposeFlag B_tr, Gan_Matrix_f *C )
{
   unsigned long Crows = B_tr ? B->cols : B->rows;
   long     i, j, k;
   float   total, *Cptr;
#ifdef HAVE_LAPACK
   long nel = Crows*U->size; /* number of matrix elements */
   long onei = 1;
#endif

   /* consistency check */
   assert ( U->type == GAN_UPPER_TRI_MATRIX );

   /* check that the dimensions of input matrices U,B are compatible */
   gan_err_test_ptr ( U->size == (B_tr ? B->rows : B->cols),
                      "upper_tri_matrix_lmult", GAN_ERROR_INCOMPATIBLE, "" );

   /* allocate result matrix C if necessary */
   if ( C == NULL )
      C = gan_matf_alloc ( Crows, U->size );
   else
      C = gan_matf_set_dims ( C, Crows, U->size );

   if ( C == NULL )
   {
      gan_err_register ( "upper_tri_matrix_lmult", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   if ( B_tr )
   {
      /* can't do in-place matrix multiply with transpose */
      assert ( B != C );

      if ( !U_tr && !U_inv )
         /* compute matrix/vector product C = B^T*U */
         for ( j = 0, Cptr = C->data; (unsigned long) j < U->size; j++ )
            for ( i = 0; (unsigned long) i < Crows; i++ )
            {
               total = 0.0F;
               for ( k = 0; k <= j; k++ )
                  total += U->data[j*(j+1)/2+k]*B->data[i*B->rows+k];

               *Cptr++ = total;
            }
      else if ( !U_tr )
      {
         /* compute matrix/vector product y = B^T*U^-1 */
         gan_matf_tpose_q ( B, C );
#ifdef HAVE_LAPACK
         for ( i = 0, Cptr = C->data; (unsigned long) i < C->rows;
               i++, Cptr++ )
            stpsv_ ( "U", "T", "N", (long *)&C->cols, U->data,
                     Cptr, (long *)&C->rows );
#else /* !HAVE_LAPACK */
         for ( i = 0, Cptr = C->data; (unsigned long) i < C->rows;
               i++, Cptr++ )
            gan_stpsv ( GAN_MATRIXPART_UPPER, GAN_TRANSPOSE, GAN_NOUNIT, C->cols, U->data,
                        Cptr, C->rows );
#endif /* #ifdef HAVE_LAPACK */
      }
      else if ( !U_inv )
         /* compute matrix/vector product C = B^T*U^T */
         for ( j = 0, Cptr = C->data; (unsigned long) j < U->size; j++ )
            for ( i = 0; (unsigned long) i < Crows; i++ )
            {
               total = 0.0F;
               for ( k = j; (unsigned long) k < U->size; k++ )
                  total += U->data[k*(k+1)/2+j]*B->data[i*B->rows+k];

               *Cptr++ = total;
            }
      else /* U_tr && U_inv */
      {
         /* compute matrix/vector product C = B^T*U^-T */
         gan_matf_tpose_q ( B, C );
#ifdef HAVE_LAPACK
         for ( i = 0, Cptr = C->data; (unsigned long) i < C->rows;
               i++, Cptr++ )
            stpsv_ ( "U", "N", "N", (long *)&C->cols, U->data,
                     Cptr, (long *)&C->rows );
#else /* !HAVE_LAPACK */
         for ( i = 0, Cptr = C->data; (unsigned long) i < C->rows;
               i++, Cptr++ )
            gan_stpsv ( GAN_MATRIXPART_UPPER, GAN_NOTRANSPOSE, GAN_NOUNIT, C->cols,
                        U->data, Cptr, C->rows );
#endif /* #ifdef HAVE_LAPACK */
      }
   }
   else
      if ( !U_tr && !U_inv )
         /* compute matrix/vector product C = B*U */
         for ( j = (long)U->size-1; j >= 0; j-- )
            for ( i = (long)Crows-1; i >= 0; i-- )
            {
               total = 0.0F;
               for ( k = 0; k <= j; k++ )
                  total += U->data[j*(j+1)/2+k]*B->data[k*B->rows+i];

               C->data[j*Crows+i] = total;
            }
      else if ( !U_tr )
      {
         /* compute matrix/vector product C = B*U^-1 */
#ifdef HAVE_LAPACK
         if ( B != C ) scopy_ ( &nel, B->data, &onei, C->data, &onei );
         for ( i = 0, Cptr = C->data; (unsigned long) i < C->rows;
               i++, Cptr++ )
            stpsv_ ( "U", "T", "N", (long *)&C->cols, U->data,
                     Cptr, (long *)&C->rows );
#else /* !HAVE_LAPACK */
         if ( B != C )
            gan_scopy ( Crows*U->size, B->data, 1, C->data, 1 );

         for ( i = 0, Cptr = C->data; (unsigned long) i < C->rows;
               i++, Cptr++ )
            gan_stpsv ( GAN_MATRIXPART_UPPER, GAN_TRANSPOSE, GAN_NOUNIT, C->cols, U->data,
                        Cptr, C->rows );
#endif /* #ifdef HAVE_LAPACK */
      }
      else if ( !U_inv )
         /* compute matrix/vector product C = B*U^T */
         for ( j = 0, Cptr = C->data; (unsigned long)j < U->size; j++ )
            for ( i = 0; (unsigned long) i < Crows; i++ )
            {
               total = 0.0F;
               for ( k = j; (unsigned long) k < U->size; k++ )
                  total += U->data[k*(k+1)/2+j]*B->data[k*B->rows+i];

               *Cptr++ = total;
            }
      else /* U_tr && U_inv */
      {
         /* compute matrix/vector product C = B*U^-T */
#ifdef HAVE_LAPACK
         if ( B != C ) scopy_ ( &nel, B->data, &onei, C->data, &onei );
         for ( i = 0, Cptr = C->data; (unsigned long) i < C->rows;
               i++, Cptr++ )
            stpsv_ ( "U", "N", "N", (long *)&C->cols, U->data,
                     Cptr, (long *)&C->rows );
#else /* !HAVE_LAPACK */
         if ( B != C ) gan_scopy ( Crows*U->size, B->data, 1, C->data, 1 );
         for ( i = 0, Cptr = C->data; (unsigned long) i < C->rows;
               i++, Cptr++ )
            gan_stpsv ( GAN_MATRIXPART_UPPER, GAN_NOTRANSPOSE, GAN_NOUNIT, C->cols,
                        U->data, Cptr, C->rows );
#endif /* #ifdef HAVE_LAPACK */
      }

   /* return result matrix */
   return C;
}

/* function to multipy lower triangular matrix L and square matrix B:
 * C = L(^-1)(^T)*B(^-1)(^T), returning C.
 */
static Gan_SquMatrix_f *
 lower_tri_matrix_rmult_squ (
     Gan_SquMatrix_f *L, Gan_TposeFlag L_tr, Gan_InvertFlag L_inv,
     Gan_SquMatrix_f *B, Gan_TposeFlag B_tr, Gan_InvertFlag B_inv,
     Gan_SquMatrix_f *C )
{
   long info;

   /* consistency check */
   gan_err_test_ptr ( L->type == GAN_LOWER_TRI_MATRIX,
                      "lower_tri_matrix_rmult_squ", GAN_ERROR_ILLEGAL_ARGUMENT,
                      "" );

   /* set result matrix */
   if ( C == NULL )
      C = gan_ltmatf_alloc ( L->size );
   else
      C = gan_ltmatf_set_size ( C, L->size );

   if ( C == NULL )
   {
      gan_err_register ( "lower_tri_matrix_rmult_squ", GAN_ERROR_FAILURE, "" );
      return NULL;
   }
   
   /* check for self-multiplication with transpose */
   if ( L == B && L_tr != B_tr && L_inv == B_inv )
   {
      long i, j, k;
      float total;

      if ( L != C )
         /* copy L into C */
         if ( gan_squmatf_copy_q ( L, C ) == NULL )
         {
            gan_err_register ( "lower_tri_matrix_rmult_squ", GAN_ERROR_FAILURE,
                               "" );
            return NULL;
         }

      if ( L_tr )
      {
         if ( L_inv )
         {
            /* compute L^-T * L^-1 using CLAPACK function pptri */
#ifdef HAVE_LAPACK
            spptri_ ( "U", (long *)&C->size, C->data, &info );
            gan_err_test_ptr ( info == 0, "lower_tri_matrix_rmult_squ",
                               GAN_ERROR_FAILURE, "" );
#else /* !HAVE_LAPACK */
            if ( !gan_spptri ( GAN_MATRIXPART_UPPER, C->size, C->data, &info ) )
            {
               gan_err_flush_trace();
               gan_err_register ( "lower_tri_matrix_rmult_squ",
                                  GAN_ERROR_FAILURE, "" );
               return NULL;
            }
#endif /* #ifdef HAVE_LAPACK */
         }
         else
         {
            /* compute L^T * L */
            for ( i = 0; i < (long)C->size; i++ )
               for ( j = 0; j <= i; j++ )
               {
                  for ( k = i, total = 0.0F; k < (long)C->size; k++ )
                     total += C->data[k*(k+1)/2+i]*C->data[k*(k+1)/2+j];

                  C->data[i*(i+1)/2+j] = total;
               }
         }
      }
      else
      {
         /* compute L^-1 * L^-T or L * L^T */
         if ( L_inv )
         {
            /* first invert L */
            if ( gan_squmatf_invert_i ( C ) == NULL )
            {
               gan_err_register ( "lower_tri_matrix_rmult_squ",
                                  GAN_ERROR_FAILURE, "" );
               return NULL;
            }
         }
         
         /* now compute product */
         for ( i = (long)C->size-1; i >= 0; i-- )
            for ( j = i; j >= 0; j-- )
            {
               for ( k = j, total = 0.0F; k >= 0; k-- )
                  total += C->data[i*(i+1)/2+k]*C->data[j*(j+1)/2+k];

               C->data[i*(i+1)/2+j] = total;
            }
      }
      
      /* convert result matrix to symmetric type */
      if ( gan_symmatf_set_size ( C, L->size ) == NULL )
      {
         gan_err_register ( "upper_tri_matrix_rmult_squ", GAN_ERROR_FAILURE,
                            "" );
         return NULL;
      }

      return C;
   }

   gan_err_flush_trace();
   gan_err_register ( "lower_tri_matrix_rmult_squ", GAN_ERROR_NOT_IMPLEMENTED,
                      "" );
   return NULL;
}

/* function to multipy upper triangular matrix U and square matrix B:
 * C = U(^-1)(^T)*B(^-1)(^T), returning C.
 */
static Gan_SquMatrix_f *
 upper_tri_matrix_rmult_squ (
     Gan_SquMatrix_f *U, Gan_TposeFlag U_tr, Gan_InvertFlag U_inv,
     Gan_SquMatrix_f *B, Gan_TposeFlag B_tr, Gan_InvertFlag B_inv,
     Gan_SquMatrix_f *C )
{
   long info;

   /* consistency check */
   gan_err_test_ptr ( U->type == GAN_UPPER_TRI_MATRIX,
                      "upper_tri_matrix_rmult_squ", GAN_ERROR_ILLEGAL_ARGUMENT,
                      "" );

   /* set result matrix */
   if ( C == NULL )
      C = gan_utmatf_alloc ( U->size );
   else
      C = gan_utmatf_set_size ( C, U->size );

   if ( C == NULL )
   {
      gan_err_register ( "upper_tri_matrix_rmult_squ", GAN_ERROR_FAILURE, "" );
      return NULL;
   }
   
   /* check for self-multiplication with transpose */
   if ( U == B && U_tr != B_tr && U_inv == B_inv )
   {
      long i, j, k;
      float total;

      if ( U != C )
         /* copy U into C */
         if ( gan_squmatf_copy_q ( U, C ) == NULL )
         {
            gan_err_register ( "upper_tri_matrix_rmult_squ", GAN_ERROR_FAILURE,
                               "" );
            return NULL;
         }

      if ( U_tr )
      {
         /* compute U^-T * U^-i or U^T * U^-T*/
         if ( U_inv )
         {
            /* first invert U */
            if ( gan_squmatf_invert_i ( C ) == NULL )
            {
               gan_err_register ( "upper_tri_matrix_rmult_squ",
                                  GAN_ERROR_FAILURE, "" );
               return NULL;
            }
         }
         
         /* now compute product */
         for ( i = (long)C->size-1; i >= 0; i-- )
            for ( j = i; j >= 0; j-- )
            {
               for ( k = j, total = 0.0F; k >= 0; k-- )
                  total += C->data[i*(i+1)/2+k]*C->data[j*(j+1)/2+k];

               C->data[i*(i+1)/2+j] = total;
            }
      }
      else
      {
         if ( U_inv )
         {
            /* compute U^-1 * U^-T using CLAPACK function pptri */
#ifdef HAVE_LAPACK
            spptri_ ( "U", (long *)&C->size, C->data, &info );
            gan_err_test_ptr ( info == 0, "upper_tri_matrix_rmult_squ",
                               GAN_ERROR_FAILURE, "" );
#else /* !HAVE_LAPACK */
            if ( !gan_spptri ( GAN_MATRIXPART_UPPER, C->size, C->data, &info ) )
            {
               gan_err_flush_trace();
               gan_err_register ( "upper_tri_matrix_rmult_squ",
                                  GAN_ERROR_FAILURE, "" );
               return NULL;
            }
#endif /* #ifdef HAVE_LAPACK */
         }
         else
         {
            /* compute U * U^T */
            for ( i = 0; i < (long)C->size; i++ )
               for ( j = 0; j <= i; j++ )
               {
                  for ( k = i, total = 0.0F; k < (long)C->size; k++ )
                     total += C->data[k*(k+1)/2+i]*C->data[k*(k+1)/2+j];

                  C->data[i*(i+1)/2+j] = total;
               }
         }
      }
      
      /* convert result matrix to symmetric type */
      if ( gan_symmatf_set_size ( C, U->size ) == NULL )
      {
         gan_err_register ( "upper_tri_matrix_rmult_squ", GAN_ERROR_FAILURE,
                            "" );
         return NULL;
      }

      return C;
   }

   gan_err_flush_trace();
   gan_err_register ( "upper_tri_matrix_rmult_squ", GAN_ERROR_NOT_IMPLEMENTED,
                      "" );
   return NULL;
}

static Gan_SquMatrix_f *
 triangular_matrix_lrmult (
     const Gan_SquMatrix_f *A, const Gan_Matrix_f *B, Gan_TposeFlag B_tr,
     Gan_Matrix_f *C, Gan_SquMatrix_f *D )
{
   gan_err_flush_trace();
   gan_err_register ( "triangular_matrix_lrmult", GAN_ERROR_NOT_IMPLEMENTED,
                      "" );
   return NULL;
}

/* inversion for triangular matrices.
 */
static Gan_SquMatrix_f *
 triangular_matrix_invert ( Gan_SquMatrix_f *A, Gan_SquMatrix_f *B )
{
   long info;

   /* consistency check */
   assert ( A->type == GAN_LOWER_TRI_MATRIX ||
            A->type == GAN_UPPER_TRI_MATRIX );

   /* allocate matrix B if necessary */
   if ( B == NULL )
   {
      if ( A->type == GAN_LOWER_TRI_MATRIX )
         B = gan_ltmatf_alloc ( A->size );
      else
         B = gan_utmatf_alloc ( A->size );
   }
   else
   {
      /* set type and size of result matrix */
      if ( A->type == GAN_LOWER_TRI_MATRIX )
         B = gan_ltmatf_set_size ( B, A->size );
      else
         B = gan_utmatf_set_size ( B, A->size );
   }

   if ( B == NULL )
   {
      gan_err_register ( "triangular_matrix_invert", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   if ( B != A )
#ifdef HAVE_LAPACK
   {
      /* copy input matrix S into L */
      long nel = A->size*(A->size+1)/2, onei = 1;

      scopy_ ( &nel, A->data, &onei, B->data, &onei );
   }

   /* perform matrix inversion */
   stptri_ ( "U", "N", (long *)&B->size, B->data, &info );
   if ( info != 0 )
   {
      gan_err_flush_trace();
      if ( info < 0 )
      {
         gan_err_register ( "triangular_matrix_invert",
                            GAN_ERROR_CLAPACK_ILLEGAL_ARG, "" );
      }
      else
         gan_err_register ( "triangular_matrix_invert", GAN_ERROR_FAILURE, "");
   }
#else /* !HAVE_LAPACK */
   /* copy input matrix S into L */
   gan_scopy ( A->size*(A->size+1)/2, A->data, 1, B->data, 1 );

   /* perform matrix inversion */
   if ( !gan_stptri ( GAN_MATRIXPART_UPPER, GAN_NOUNIT, B->size, B->data, &info ) )
   {
      gan_err_flush_trace();
      if ( info < 0 )
      {
         gan_err_register ( "triangular_matrix_invert",
                            GAN_ERROR_CLAPACK_ILLEGAL_ARG, "" );
      }
      else
         gan_err_register ( "triangular_matrix_invert", GAN_ERROR_FAILURE, "");
   }
#endif /* #ifdef HAVE_LAPACK */

   return B;
}

/* Cholesky factorisation not defined for triangular matrices.
 */
static Gan_SquMatrix_f *
 triangular_matrix_cholesky ( Gan_SquMatrix_f *A, Gan_SquMatrix_f *B,
                              int *error_code )
{
   gan_err_flush_trace();
   gan_err_register ( "triangular_matrix_cholesky", GAN_ERROR_NOT_IMPLEMENTED,
                      "" );
   return NULL;
}

/* Compute squared Frobenius norm of triangular matrix A.
 */
static float
 triangular_matrix_sumsqr ( const Gan_SquMatrix_f *A )
{
   long i;
   float total = 0.0F;

   for ( i = A->size*(A->size+1)/2-1; i >= 0; i-- )
      total += gan_sqr(A->data[i]);

   return total;
}

/* Compute Frobenius norm of triangular matrix A.
 */
static float
 triangular_matrix_Fnorm ( const Gan_SquMatrix_f *A )
{
   long i;
   float total = 0.0F;

   for ( i = A->size*(A->size+1)/2-1; i >= 0; i-- )
      total += gan_sqr(A->data[i]);

   return (float)sqrt(total);
}

/* Convert lower triangular matrix to generic matrix */
static Gan_Matrix_f *
 lower_tri_matrix_matrix ( const Gan_SquMatrix_f *L, struct Gan_Matrix_f *B )
{
   long i, j;

   /* consistency check */
   assert ( L->type == GAN_LOWER_TRI_MATRIX );

   /* allocate/set result matrix B */
   if ( B == NULL )
      B = gan_matf_alloc ( L->size, L->size );
   else
      B = gan_matf_set_dims ( B, L->size, L->size );

   if ( B == NULL )
   {
      gan_err_register ( "lower_tri_matrix_matrix", GAN_ERROR_FAILURE, "" );
      return NULL;
   }
   
   /* fill with zeros */
   gan_matf_fill_const_q ( B, L->size, L->size, 0.0F );

   /* fill in lower triangle */
   for ( i = L->size-1; i >= 0; i-- )
      for ( j = i; j >= 0; j-- )
         B->data[j*L->size+i] = L->data[i*(i+1)/2+j];

   return B;
}

/* Convert upper triangular matrix to generic matrix */
static Gan_Matrix_f *
 upper_tri_matrix_matrix ( const Gan_SquMatrix_f *U, struct Gan_Matrix_f *B )
{
   unsigned long i, j;

   /* consistency check */
   assert ( U->type == GAN_UPPER_TRI_MATRIX );

   /* allocate/set result matrix B */
   if ( B == NULL )
      B = gan_matf_alloc ( U->size, U->size );
   else
      B = gan_matf_set_dims ( B, U->size, U->size );

   if ( B == NULL )
   {
      gan_err_register ( "upper_tri_matrix_matrix", GAN_ERROR_FAILURE, "" );
      return NULL;
   }
   
   /* fill with zeros */
   gan_matf_fill_const_q ( B, U->size, U->size, 0.0F );

   /* fill in upper triangle */
   for ( j = 0; j < U->size; j++ )
      for ( i = j; i < U->size; i++ )
         B->data[i*U->size+j] = U->data[i*(i+1)/2+j];

   return B;
}

static Gan_SquMatrixFFuncs lfuncs =
{
   lower_tri_matrix_set_el,
   lower_tri_matrix_get_el,
   lower_tri_matrix_inc_el,
   lower_tri_matrix_dec_el,
   lower_tri_matrix_fprint,
   lower_tri_matrix_copy,
   lower_tri_matrix_scale,
   lower_tri_matrix_add,
   lower_tri_matrix_sub,
   lower_tri_matrix_multv,
   lower_tri_matrix_rmult,
   lower_tri_matrix_lmult,
   lower_tri_matrix_rmult_squ,
   triangular_matrix_lrmult,
   triangular_matrix_invert,
   triangular_matrix_cholesky,
   triangular_matrix_sumsqr,
   triangular_matrix_Fnorm,
   lower_tri_matrix_matrix
};

static Gan_SquMatrixFFuncs ufuncs =
{
   upper_tri_matrix_set_el,
   upper_tri_matrix_get_el,
   upper_tri_matrix_inc_el,
   upper_tri_matrix_dec_el,
   upper_tri_matrix_fprint,
   upper_tri_matrix_copy,
   upper_tri_matrix_scale,
   upper_tri_matrix_add,
   upper_tri_matrix_sub,
   upper_tri_matrix_multv,
   upper_tri_matrix_rmult,
   upper_tri_matrix_lmult,
   upper_tri_matrix_rmult_squ,
   triangular_matrix_lrmult,
   triangular_matrix_invert,
   triangular_matrix_cholesky,
   triangular_matrix_sumsqr,
   triangular_matrix_Fnorm,
   upper_tri_matrix_matrix
};

/**
 * \addtogroup GeneralSizeMatrixSet
 * \{
 */

/**
 * \brief Set square matrix to be lower triangular with given size.
 * \return The result matrix \a L.
 *
 * Set square matrix \a L to be lower triangular with given \a size.
 * \a L should already have been allocated, with arbitrary type and size.
 * The internal data of \a L will be reallocated if necessary.
 */
Gan_SquMatrix_f *
 gan_ltmatf_set_size ( Gan_SquMatrix_f *L, unsigned long size )
{
   /* set type of matrix */
   L->type = GAN_LOWER_TRI_MATRIX;

   /* check whether matrix is big enough */
   if ( L->data_size < size*(size+1)/2 )
   {
      /* re-allocate matrix data */
      gan_err_test_ptr ( L->data_alloc, "gan_ltmatf_set_size",
                         GAN_ERROR_INCOMPATIBLE, "" );
      L->data_size = size*(size+1)/2;
      L->data = gan_realloc_array ( float,  L->data, L->data_size );
      if ( L->data == NULL )
      {
         gan_err_flush_trace();
         gan_err_register_with_number ( "gan_ltmatf_set_size", GAN_ERROR_MALLOC_FAILED, "", L->data_size*sizeof(float) );
         return NULL;
      }
   }

   /* set matrix size */
   L->size = size;

   /* set pointer to structure holding type-specific functions */
   L->funcs = &lfuncs;

   return L;
}

/**
 * \brief Set square matrix to be upper triangular with given size.
 * \return The result matrix \a U.
 *
 * Set square matrix \a U to be upper triangular with given \a size.
 * \a U should already have been allocated, with arbitrary type and size.
 * The internal data of \a U will be reallocated if necessary.
 */
Gan_SquMatrix_f *
 gan_utmatf_set_size ( Gan_SquMatrix_f *U, unsigned long size )
{
   /* set type of matrix */
   U->type = GAN_UPPER_TRI_MATRIX;

   /* check whether matrix is big enough */
   if ( U->data_size < size*(size+1)/2 )
   {
      /* re-allocate matrix data */
      gan_err_test_ptr ( U->data_alloc, "gan_utmatf_set_size",
                         GAN_ERROR_INCOMPATIBLE, "" );
      U->data_size = size*(size+1)/2;
      U->data = gan_realloc_array ( float,  U->data, U->data_size );
      if ( U->data == NULL )
      {
         gan_err_flush_trace();
         gan_err_register_with_number ( "gan_utmatf_set_size", GAN_ERROR_MALLOC_FAILED, "", U->data_size*sizeof(float) );
         return NULL;
      }
   }

   /* set matrix size */
   U->size = size;

   /* set pointer to structure holding type-specific functions */
   U->funcs = &ufuncs;

   return U;
}

/**
 * \}
 */

/**
 * \addtogroup GeneralSizeMatrixFill
 * \{
 */

/**
 * \brief Fill a lower triangular matrix with values.
 * \return Result matrix \a L.
 *
 * Set lower triangular matrix \a L to have given \a size, and fill
 * with values from a variable argument list.
 */
Gan_SquMatrix_f *
 gan_ltmatf_fill_va ( Gan_SquMatrix_f *L, unsigned long size, ... )
{
   unsigned int i, j;
   va_list ap;

   /* allocate matrix A if necessary */
   if ( L == NULL )
      L = gan_ltmatf_alloc ( size );
   else
      L = gan_ltmatf_set_size ( L, size );

   /* read variable argument list of matrix elements, lower triangle only */
   va_start ( ap, size );
   for ( i = 0; i < size; i++ )
      for ( j = 0; j <= i; j++ )
         L->data[i*(i+1)/2+j] = (float)va_arg ( ap, double );

   va_end ( ap );
   return L;
}

/**
 * \brief Fill an upper triangular matrix with values.
 * \return Result matrix \a U.
 *
 * Set upper triangular matrix \a U to have given \a size, and fill
 * with values from a variable argument list.
 */
Gan_SquMatrix_f *
 gan_utmatf_fill_va ( Gan_SquMatrix_f *U, unsigned long size, ... )
{
   unsigned int i, j;
   va_list ap;

   /* allocate matrix A if necessary */
   if ( U == NULL )
      U = gan_utmatf_alloc ( size );
   else
      U = gan_utmatf_set_size ( U, size );

   /* read variable argument list of matrix elements, upper triangle only */
   va_start ( ap, size );
   for ( i = 0; i < size; i++ )
      for ( j = i; j < size; j++ )
         U->data[j*(j+1)/2+i] = (float)va_arg ( ap, double );

   va_end ( ap );

   return U;
}

/**
 * \brief Fill a lower triangular matrix with a constant value.
 * \return Result matrix \a L.
 *
 * Set a lower triangular matrix \a L to have given \a size, and fill with
 * constant \a value.
 */
Gan_SquMatrix_f *
 gan_ltmatf_fill_const_q ( Gan_SquMatrix_f *L, unsigned long size,
                           float value )
{
   int i;

   /* allocate matrix A if necessary */
   if ( L == NULL )
      L = gan_ltmatf_alloc ( size );
   else
      L = gan_ltmatf_set_size ( L, size );

   if ( L == NULL )
   {
      gan_err_register ( "gan_ltmatf_fill_const_q", GAN_ERROR_FAILURE, "" );
      return NULL;
   }
   
   /* fill matrix with constant */
   for ( i = size*(size+1)/2-1; i >= 0; i-- )
      L->data[i] = value;

   return L;
}

/**
 * \brief Fill a upper triangular matrix with a constant value.
 * \return Result matrix \a U.
 *
 * Set an upper triangular matrix \a U to have given \a size, and fill with
 * constant \a value.
 */
Gan_SquMatrix_f *
 gan_utmatf_fill_const_q ( Gan_SquMatrix_f *U, unsigned long size,
                           float value )
{
   int i;

   /* allocate matrix A if necessary */
   if ( U == NULL )
      U = gan_utmatf_alloc ( size );
   else
      U = gan_utmatf_set_size ( U, size );

   if ( U == NULL )
   {
      gan_err_register ( "gan_utmatf_fill_const_q", GAN_ERROR_FAILURE, "" );
      return NULL;
   }
   
   /* fill matrix with constant */
   for ( i = size*(size+1)/2-1; i >= 0; i-- )
      U->data[i] = value;

   return U;
}

/**
 * \}
 */

/**
 * \addtogroup GeneralSizeMatrixRead
 * \{
 */

/**
 * \brief Read the elements of a lower triangular matrix into pointers.
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Read the elements of a lower triangular matrix \a L into pointers
 * from a variable argument list. The \a size argument must match the
 * size of the matrix \a L.
 */
Gan_Bool
 gan_ltmatf_read_va ( const Gan_SquMatrix_f *L, unsigned long size, ... )
{
   unsigned int i, j;
   va_list ap;

   /* check that dimensions match */
   gan_err_test_bool ( L->type == GAN_LOWER_TRI_MATRIX && size == L->size,
                       "gan_ltmatf_read_va", GAN_ERROR_INCOMPATIBLE, "" );

   /* fill variable argument list of lower triangular matrix element pointers*/
   va_start ( ap, size );
   for ( i = 0; i < size; i++ )
      for ( j = 0; j <= i; j++ )
         *(va_arg ( ap, float * )) = L->data[i*(i+1)/2+j];

   va_end ( ap );

   /* return with success */
   return GAN_TRUE;
}

/**
 * \brief Read the elements of an upper triangular matrix into pointers.
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Read the elements of an upper triangular matrix \a U into pointers
 * from a variable argument list. The \a size argument must match the
 * size of the matrix \a U.
 */
Gan_Bool
 gan_utmatf_read_va ( const Gan_SquMatrix_f *U, unsigned long size, ... )
{
   unsigned int i, j;
   va_list ap;

   /* check that type and dimensions match */
   gan_err_test_bool ( U->type == GAN_UPPER_TRI_MATRIX && size == U->size,
                       "gan_utmatf_read_va", GAN_ERROR_INCOMPATIBLE, "" );

   /* fill variable argument list of upper triangular matrix element pointers*/
   va_start ( ap, size );
   for ( i = 0; i < size; i++ )
      for ( j = i; j < size; j++ )
         *(va_arg ( ap, float * )) = U->data[i*(i+1)/2+j];

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
Gan_SquMatrix_f *
 gan_ltmatf_form_gen ( Gan_SquMatrix_f *L, unsigned long size,
                       float *data, size_t data_size )
{
   if ( L == NULL )
   {
      /* dyamically allocate matrix */
      L = gan_malloc_object(Gan_SquMatrix_f);

      /* set dynamic allocation flag */
      L->struct_alloc = GAN_TRUE;
   }
   else
      /* indicate matrix was not dynamically allocated */
      L->struct_alloc = GAN_FALSE;

   if ( data == NULL )
   {
      if ( data_size == 0 )
         /* set size of matrix data to be allocated */
         L->data_size = size*(size+1)/2;
      else
      {
         /* a non-zero data size has been requested */
         assert ( data_size >= size*(size+1)/2 );
         L->data_size = data_size;
      }

      /* allocate matrix data */
      if ( L->data_size == 0 ) L->data = NULL;
      else
      {
         L->data = gan_malloc_array ( float, L->data_size );
         if ( L->data == NULL )
         {
            gan_err_flush_trace();
            gan_err_register_with_number ( "gan_ltmatf_form_gen", GAN_ERROR_MALLOC_FAILED, "", L->data_size*sizeof(float) );
            return NULL;
         }
      }

      L->data_alloc = GAN_TRUE;
   }
   else
   {
      /* data for this matrix is provided */
      gan_err_test_ptr ( data_size >= size*(size+1)/2, "gan_ltmatf_form_gen",
                         GAN_ERROR_INCOMPATIBLE, "" );

      /* set data pointer in matrix */
      L->data = data;
      L->data_size = data_size;

      /* we shouldn't free the matrix data */
      L->data_alloc = GAN_FALSE;
   }

   /* set type and class member functions */
   gan_ltmatf_set_size ( L, size );

   return L;
}

/* not a user function */
Gan_SquMatrix_f *
 gan_utmatf_form_gen ( Gan_SquMatrix_f *U, unsigned long size,
                      float *data, size_t data_size )
{
   if ( U == NULL )
   {
      /* dyamically allocate matrix */
      U = gan_malloc_object(Gan_SquMatrix_f);

      /* set dynamic allocation flag */
      U->struct_alloc = GAN_TRUE;
   }
   else
      /* indicate matrix was not dynamically allocated */
      U->struct_alloc = GAN_FALSE;

   if ( data == NULL )
   {
      if ( data_size == 0 )
         /* set size of matrix data to be allocated */
         U->data_size = size*(size+1)/2;
      else
      {
         /* a non-zero data size has been requested */
         assert ( data_size >= size*(size+1)/2 );
         U->data_size = data_size;
      }

      /* allocate matrix data */
      if ( U->data_size == 0 ) U->data = NULL;
      else
      {
         U->data = gan_malloc_array ( float, U->data_size );
         if ( U->data == NULL )
         {
            gan_err_flush_trace();
            gan_err_register_with_number ( "gan_utmatf_form_gen", GAN_ERROR_MALLOC_FAILED, "", U->data_size*sizeof(float) );
            return NULL;
         }
      }

      U->data_alloc = GAN_TRUE;
   }
   else
   {
      /* data for this matrix is provided */
      gan_err_test_ptr ( data_size >= size*(size+1)/2, "gan_utmatf_form_gen",
                         GAN_ERROR_INCOMPATIBLE, "" );

      /* set data pointer in matrix */
      U->data = data;
      U->data_size = data_size;

      /* we shouldn't free the matrix data */
      U->data_alloc = GAN_FALSE;
   }

   /* set type and class member functions */
   gan_utmatf_set_size ( U, size );
   return U;
}

/**
 * \}
 */

/**
 * \addtogroup GeneralSizeMatrixFill
 * \{
 */

/**
 * \brief Fills a lower triangular matrix with values.
 * \return The filled matrix \a L.
 *
 * Sets a lower triangular matrix \a L to given \a size, and fills it
 * with values from the variable argument list \a aptr.
 */
Gan_SquMatrix_f *
 gan_ltmatf_fill_vap ( Gan_SquMatrix_f *L, unsigned long size, va_list *aptr )
{
   unsigned int i, j;

   /* allocate matrix A if necessary */
   if ( L == NULL )
      L = gan_ltmatf_alloc ( size );
   else
      L = gan_ltmatf_set_size ( L, size );

   /* read variable argument list of matrix elements, lower triangle only */
   for ( i = 0; i < size; i++ )
      for ( j = 0; j <= i; j++ )
         L->data[i*(i+1)/2+j] = (float)va_arg ( *aptr, double );

   return L;
}

/**
 * \brief Fills an upper triangular matrix with values.
 * \return The filled matrix \a U.
 *
 * Sets an upper triangular matrix \a U to given \a size, and fills it
 * with values from the variable argument list \a aptr.
 */
Gan_SquMatrix_f *
 gan_utmatf_fill_vap ( Gan_SquMatrix_f *U, unsigned long size, va_list *aptr )
{
   unsigned int i, j;

   /* allocate matrix A if necessary */
   if ( U == NULL )
      U = gan_utmatf_alloc ( size );
   else
      U = gan_utmatf_set_size ( U, size );

   /* read variable argument list of matrix elements, upper triangle only */
   for ( i = 0; i < size; i++ )
      for ( j = i; j < size; j++ )
         U->data[j*(j+1)/2+i] = (float)va_arg ( *aptr, double );

   return U;
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

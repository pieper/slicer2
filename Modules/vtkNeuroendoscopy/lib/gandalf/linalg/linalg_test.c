/**
 * File:          $RCSfile: linalg_test.c,v $
 * Module:        Linear algebra package test program
 * Part of:       Gandalf Library
 *
 * Revision:      $Revision: 1.1.2.1 $
 * Last edited:   $Date: 2007/10/14 03:16:22 $
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

#include <gandalf/TestFramework/cUnit.h>
#include <gandalf/linalg/linalg_test.h>
#include <gandalf/linalg/2vector.h>
#include <gandalf/linalg/3vector.h>
#include <gandalf/linalg/4vector.h>
#include <gandalf/linalg/2x2matrix.h>
#include <gandalf/linalg/2x3matrix.h>
#include <gandalf/linalg/3x3matrix.h>
#include <gandalf/linalg/2x4matrix.h>
#include <gandalf/linalg/3x4matrix.h>
#include <gandalf/linalg/4x4matrix.h>
#include <gandalf/linalg/2vectorf.h>
#include <gandalf/linalg/3vectorf.h>
#include <gandalf/linalg/4vectorf.h>
#include <gandalf/linalg/2x2matrixf.h>
#include <gandalf/linalg/2x3matrixf.h>
#include <gandalf/linalg/2x4matrixf.h>
#include <gandalf/linalg/3x3matrixf.h>
#include <gandalf/linalg/3x4matrixf.h>
#include <gandalf/linalg/4x4matrixf.h>
#include <gandalf/linalg/mat_gen.h>
#include <gandalf/linalg/vec_gen.h>
#include <gandalf/linalg/mat_symmetric.h>
#include <gandalf/linalg/mat_diagonal.h>
#include <gandalf/linalg/mat_triangular.h>
#include <gandalf/linalg/mat_scaledI.h>
#include <gandalf/linalg/matf_gen.h>
#include <gandalf/linalg/vecf_gen.h>
#include <gandalf/linalg/matf_symmetric.h>
#include <gandalf/linalg/matf_diagonal.h>
#include <gandalf/linalg/matf_triangular.h>
#include <gandalf/linalg/matf_scaledI.h>
#include <gandalf/linalg/mat_svd.h>
#include <gandalf/linalg/mat_qr.h>
#include <gandalf/linalg/mat_lq.h>
#include <gandalf/linalg/symmat_eigen.h>
#include <gandalf/linalg/pseudo_inverse.h>
#include <gandalf/linalg/matf_svd.h>
#include <gandalf/linalg/matf_qr.h>
#include <gandalf/linalg/matf_lq.h>
#include <gandalf/linalg/pseudo_inversef.h>
#include <gandalf/linalg/matvec_clapack.h>
#include <gandalf/linalg/matvecf_clapack.h>
#include <gandalf/common/numerics.h>
#include <gandalf/common/compare.h>
#include <gandalf/common/misc_error.h>

#define gel(M,i,j)   gan_mat_get_el(M,i,j)
#define sel(M,i,j,v) gan_mat_set_el(M,i,j,v)
#define sgel(M,i,j)   gan_squmat_get_el(M,i,j)
#define ssel(M,i,j,v) gan_squmat_set_el(M,i,j,v)
#define vgel(a,i)   gan_vec_get_el(a,i)
#define vsel(a,i,v) gan_vec_set_el(a,i,v)

#define gelf(M,i,j)   gan_matf_get_el(M,i,j)
#define self(M,i,j,v) gan_matf_set_el(M,i,j,v)
#define sgelf(M,i,j)   gan_squmatf_get_el(M,i,j)
#define sself(M,i,j,v) gan_squmatf_set_el(M,i,j,v)
#define vgelf(a,i)   gan_vecf_get_el(a,i)
#define vself(a,i,v) gan_vecf_set_el(a,i,v)

/* build random vector */
static Gan_Vector *vector_rand ( Gan_Vector *a, unsigned long rows )
{
   int i;
   
   if ( a == NULL )
      a = gan_vec_alloc(rows);
   else
      a = gan_vec_set_size(a, rows);

   if ( a == NULL ) return NULL;

   for ( i = (int)rows-1; i >= 0; i-- )
      if ( !vsel(a, i, gan_random_m11()) ) return NULL;

   return a;
}

/* simple vector addition with scale factors on input vectors */
static Gan_Vector *vector_add ( Gan_Vector *a, double as,
                                Gan_Vector *b, double bs, Gan_Vector *c )
{
   int i;

   if ( b != NULL && a->rows != b->rows ) return NULL;
   if ( c == NULL )
      c = gan_vec_alloc(a->rows);
   else
      c = gan_vec_set_size(c, a->rows);

   if ( c == NULL ) return NULL;
   if ( b == NULL )
   {
      for ( i = (int)a->rows-1; i >= 0; i-- )
         if ( !vsel ( c, i, as*vgel(a,i) ) )
            return NULL;
   }
   else
   {
      for ( i = (int)a->rows-1; i >= 0; i-- )
         if ( !vsel ( c, i, as*vgel(a,i)+bs*vgel(b,i) ) )
            return NULL;
   }

   return c;
}

/* build random full-rank matrix */
static Gan_Matrix *matrix_rand ( Gan_Matrix *A, unsigned long rows,
                                                unsigned long cols )
{
   int i, j, k, ip, jp;
   double angle, c, s, tmp1, tmp2;

   if ( A == NULL )
      A = gan_mat_alloc(rows, cols);
   else
      A = gan_mat_set_dims(A, rows, cols);

   if ( A == NULL ) return NULL;

   /* build diagonal matrix */
   gan_mat_fill_zero_q ( A, rows, cols );
   for ( i = (int)gan_min2(rows,cols)-1; i >= 0; i-- )
      if ( !sel(A, i, i, 2.0+gan_random_m11()) ) return NULL;

   /* now apply some random Givens transformations, maintaining the rank */

   /* transformations between each row */
   for ( i = (int)rows-1; i >= 0; i-- )
      for ( j = i-1; j >= 0; j-- )
      {
         angle = M_PI*gan_random_m11();
         c = cos(angle);
         s = sin(angle);
         for ( k = (int)cols-1; k >= 0; k-- )
         {
            tmp1 = gel(A,i,k);
            tmp2 = gel(A,j,k);
            sel ( A, i, k, c*tmp1 - s*tmp2 );
            sel ( A, j, k, s*tmp1 + c*tmp2 );
         }

         /* swap a random pair of rows while we're at it */
         ip = gan_random() % rows;
         jp = gan_random() % rows;
         if ( ip != jp )
            for ( k = (int)cols-1; k >= 0; k-- )
            {
               tmp1 = gel(A,ip,k);
               sel ( A, ip, k, gel(A,jp,k) );
               sel ( A, jp, k, tmp1 );
            }
      }
   
   /* transformations between each column */
   for ( i = (int)cols-1; i >= 0; i-- )
      for ( j = i-1; j >= 0; j-- )
      {
         angle = M_PI*gan_random_m11();
         c = cos(angle);
         s = sin(angle);
         for ( k = (int)rows-1; k >= 0; k-- )
         {
            tmp1 = gel(A,k,i);
            tmp2 = gel(A,k,j);
            sel ( A, k, i, c*tmp1 - s*tmp2 );
            sel ( A, k, j, s*tmp1 + c*tmp2 );
         }

         /* swap a random pair of columns while we're at it */
         ip = gan_random() % cols;
         jp = gan_random() % cols;
         if ( ip != jp )
            for ( k = (int)rows-1; k >= 0; k-- )
            {
               tmp1 = gel(A,k,ip);
               sel ( A, k, ip, gel(A,k,jp) );
               sel ( A, k, jp, tmp1 );
            }
      }

   return A;
}

/* simple matrix identity */
static Gan_Matrix *matrix_ident ( Gan_Matrix *I, unsigned long size )
{
   int i, j;

   if ( I == NULL )
      I = gan_mat_alloc(size, size);
   else
      I = gan_mat_set_dims(I, size, size);

   if ( I == NULL ) return NULL;
   for ( i = (int)I->rows-1; i >= 0; i-- )
      for ( j = (int)I->cols-1; j >= 0; j-- )
         if ( i == j )
         {
            if ( !sel ( I, i, j, 1.0 ) ) return NULL;
         }
         else
         {
            if ( !sel ( I, i, j, 0.0 ) ) return NULL;
         }
   
   return I;
}

/* simple matrix transpose */
static Gan_Matrix *matrix_transpose ( Gan_Matrix *A, Gan_Matrix *B )
{
   int i, j;

   if ( B == NULL )
      B = gan_mat_alloc(A->cols, A->rows);
   else
   {
      if ( B == A ) return NULL;
      B = gan_mat_set_dims(B, A->cols, A->rows);
   }
   
   if ( B == NULL ) return NULL;
   for ( i = (int)A->rows-1; i >= 0; i-- )
      for ( j = (int)A->cols-1; j >= 0; j-- )
         if ( !sel ( B, j, i, gel(A,i,j) ) )
            return NULL;

   return B;
}

/* simple matrix addition with scales on the input matrices */
static Gan_Matrix *matrix_add ( Gan_Matrix *A, double a,
                                Gan_Matrix *B, double b, Gan_Matrix *C )
{
   int i, j;

   if ( B != NULL && (A->rows != B->rows || A->cols != B->cols) ) return NULL;
   if ( C == NULL )
      C = gan_mat_alloc(A->rows, A->cols);
   else
      C = gan_mat_set_dims(C, A->rows, A->cols);

   if ( C == NULL ) return NULL;
   if ( B == NULL )
   {
      for ( i = (int)A->rows-1; i >= 0; i-- )
         for ( j = (int)A->cols-1; j >= 0; j-- )
            if ( !sel ( C, i, j, a*gel(A,i,j) ) )
               return NULL;
   }
   else
   {
      for ( i = (int)A->rows-1; i >= 0; i-- )
         for ( j = (int)A->cols-1; j >= 0; j-- )
            if ( !sel ( C, i, j, a*gel(A,i,j)+b*gel(B,i,j) ) )
               return NULL;
   }
   
   return C;
}

/* simple matrix/matrix multiplication */
static Gan_Matrix *matrix_matrix_mult ( Gan_Matrix *A, Gan_Matrix *B,
                                        Gan_Matrix *C )
{
   int i, j, k;

   if ( A->cols != B->rows ) return NULL;
   if ( C == NULL )
      C = gan_mat_alloc(A->rows, B->cols);
   else
      C = gan_mat_set_dims(C, A->rows, B->cols);

   if ( C == NULL ) return NULL;
   for ( i = (int)C->rows-1; i >= 0; i-- )
      for ( j = (int)C->cols-1; j >= 0; j-- )
      {
         double total = 0.0;

         for ( k = (int)A->cols-1; k >= 0; k-- )
            total += gel(A,i,k)*gel(B,k,j);

         if ( !sel ( C, i, j, total ) ) return NULL;
      }

   return C;
}

/* simple matrix/square matrix multiplication */
static Gan_Matrix *matrix_squmatrix_mult ( Gan_Matrix *A, Gan_SquMatrix *B,
                                           Gan_Matrix *C )
{
   int i, j, k;

   if ( A->cols != B->size ) return NULL;
   if ( C == NULL )
      C = gan_mat_alloc(A->rows, B->size);
   else
      C = gan_mat_set_dims(C, A->rows, B->size);

   if ( C == NULL ) return NULL;
   for ( i = (int)C->rows-1; i >= 0; i-- )
      for ( j = (int)C->cols-1; j >= 0; j-- )
      {
         double total = 0.0;

         for ( k = (int)A->cols-1; k >= 0; k-- )
            total += gel(A,i,k)*sgel(B,k,j);

         if ( !sel ( C, i, j, total ) ) return NULL;
      }

   return C;
}

/* simple square matrix/matrix multiplication */
static Gan_Matrix *squmatrix_matrix_mult ( Gan_SquMatrix *A, Gan_Matrix *B,
                                           Gan_Matrix *C )
{
   int i, j, k;

   if ( A->size != B->rows ) return NULL;
   if ( C == NULL )
      C = gan_mat_alloc(A->size, B->cols);
   else
      C = gan_mat_set_dims(C, A->size, B->cols);

   if ( C == NULL ) return NULL;
   for ( i = (int)C->rows-1; i >= 0; i-- )
      for ( j = (int)C->cols-1; j >= 0; j-- )
      {
         double total = 0.0;

         for ( k = (int)A->size-1; k >= 0; k-- )
            total += sgel(A,i,k)*gel(B,k,j);

         if ( !sel ( C, i, j, total ) ) return NULL;
      }

   return C;
}

/* simple matrix/matrix multiplication */
static Gan_Vector *matrix_vector_mult ( Gan_Matrix *A, Gan_Vector *b,
                                        Gan_Vector *c )
{
   int i, j;

   if ( A->cols != b->rows ) return NULL;
   if ( c == NULL )
      c = gan_vec_alloc(A->rows);
   else
      c = gan_vec_set_size(c, A->rows);

   if ( c == NULL ) return NULL;
   for ( i = (int)A->rows-1; i >= 0; i-- )
   {
      double total = 0.0;

      for ( j = (int)A->cols-1; j >= 0; j-- )
         total += gel(A,i,j)*vgel(b,j);

      if ( !vsel ( c, i, total ) ) return NULL;
   }

   return c;
}

/* build random symmetric matrix */
static Gan_SquMatrix *symmatrix_rand ( Gan_SquMatrix *S,
                                       unsigned long rows,
                                       unsigned long cols )
{
   int i, j, k;
   Gan_Vector a;

   if ( rows != cols ) return NULL;
   if ( S == NULL )
      S = gan_symmat_alloc(rows);
   else
      S = gan_symmat_set_size(S, rows);

   if ( S == NULL ) return NULL;

   /* allocate vector */
   if ( gan_vec_form ( &a, rows ) == NULL ) return NULL;

   /* accumulate outer products to make a positive definite matrix */
   if ( gan_symmat_fill_zero_q ( S, rows ) == NULL ) return NULL;
   for ( k = (int)5*rows-1; k >= 0; k-- )
   {
      if ( vector_rand ( &a, rows ) == NULL ) return NULL;
      for ( i = (int)rows-1; i >= 0; i-- )
         for ( j = 0; j <= i; j++ )
            if ( !gan_squmat_inc_el ( S, i, j,
                                  gan_vec_get_el(&a,i)*gan_vec_get_el(&a,j) ) )
               return NULL;
   }

   gan_vec_free(&a);
   return S;
}

/* simple symmetric matrix addition with scales on the input matrices */
static Gan_SquMatrix *symmatrix_add ( Gan_SquMatrix *A, double a,
                                      Gan_SquMatrix *B, double b,
                                      Gan_SquMatrix *C )
{
   int i, j;

   if ( A->type != GAN_SYMMETRIC_MATRIX ||
        (B != NULL && B->type != GAN_SYMMETRIC_MATRIX) )
      return NULL;

   if ( B != NULL && A->size != B->size ) return NULL;
   if ( C == NULL )
      C = gan_symmat_alloc(A->size);
   else
      C = gan_symmat_set_size(C, A->size);

   if ( C == NULL ) return NULL;
   if ( B == NULL )
   {
      for ( i = (int)A->size-1; i >= 0; i-- )
         for ( j = (int)A->size-1; j >= i; j-- )
            if ( !ssel ( C, i, j, a*sgel(A,i,j) ) )
               return NULL;
   }
   else
   {
      for ( i = (int)A->size-1; i >= 0; i-- )
         for ( j = (int)A->size-1; j >= i; j-- )
            if ( !ssel ( C, i, j, a*sgel(A,i,j)+b*sgel(B,i,j) ) )
               return NULL;
   }
   
   return C;
}

/* build random diagonal matrix */
static Gan_SquMatrix *diagmatrix_rand ( Gan_SquMatrix *D,
                                        unsigned long rows,
                                        unsigned long cols )
{
   int i;

   if ( rows != cols ) return NULL;
   if ( D == NULL )
      D = gan_diagmat_alloc(rows);
   else
      D = gan_diagmat_set_size(D, rows);

   if ( D == NULL ) return NULL;

   for ( i = (int)D->size-1; i >= 0; i-- )
      if ( !gan_squmat_set_el ( D, i, i, 1.0+gan_random_01() ) )
         return NULL;

   return D;
}

/* simple diagonal matrix addition with scales on the input matrices*/
static Gan_SquMatrix *diagmatrix_add ( Gan_SquMatrix *A, double a,
                                       Gan_SquMatrix *B, double b,
                                       Gan_SquMatrix *C )
{
   int i;

   if ( A->type != GAN_DIAGONAL_MATRIX ||
        (B != NULL && B->type != GAN_DIAGONAL_MATRIX) )
      return NULL;

   if ( B != NULL && A->size != B->size ) return NULL;
   if ( C == NULL )
      C = gan_diagmat_alloc(A->size);
   else
      C = gan_diagmat_set_size(C, A->size);

   if ( C == NULL ) return NULL;
   if ( B == NULL )
   {
      for ( i = (int)A->size-1; i >= 0; i-- )
         if ( !ssel ( C, i, i, a*sgel(A,i,i) ) )
            return NULL;
   }
   else
   {
      for ( i = (int)A->size-1; i >= 0; i-- )
         if ( !ssel ( C, i, i, a*sgel(A,i,i)+b*sgel(B,i,i) ) )
            return NULL;
   }
   
   return C;
}

/* build random scaled identity matrix */
static Gan_SquMatrix *scalImatrix_rand ( Gan_SquMatrix *sI,
                                         unsigned long rows,
                                         unsigned long cols )
{
   if ( rows != cols ) return NULL;
   if ( sI == NULL )
      sI = gan_scalImat_alloc(rows);
   else
      sI = gan_scalImat_set_size(sI, rows);

   if ( sI == NULL ) return NULL;

   if ( gan_scalImat_fill_const_q ( sI, rows, 1.0 + gan_random_01() ) == NULL )
      return NULL;

   return sI;
}

/* simple scaled identity matrix addition with scales on the input matrices */
static Gan_SquMatrix *scalImatrix_add ( Gan_SquMatrix *A, double a,
                                        Gan_SquMatrix *B, double b,
                                        Gan_SquMatrix *C )
{
   if ( A->type != GAN_SCALED_IDENT_MATRIX ||
        (B != NULL && B->type != GAN_SCALED_IDENT_MATRIX) )
      return NULL;

   if ( B != NULL && A->size != B->size ) return NULL;
   if ( C == NULL )
      C = gan_scalImat_alloc(A->size);
   else
      C = gan_scalImat_set_size(C, A->size);

   if ( C == NULL ) return NULL;
   if ( A->size == 0 ) return C; /* value irrelevant for zero-size matrix */
   if ( B == NULL )
   {
      if ( gan_scalImat_fill_const_q ( C, A->size, a*sgel(A,0,0) ) == NULL )
         return NULL;
   }
   else
   {
      if ( gan_scalImat_fill_const_q ( C, A->size, a*sgel(A,0,0)+b*sgel(B,0,0))
           == NULL )
         return NULL;
   }
   
   return C;
}


/* build random lower triangular matrix */
static Gan_SquMatrix *ltmatrix_rand ( Gan_SquMatrix *L,
                                      unsigned long rows,
                                      unsigned long cols )
{
   if ( rows != cols ) return NULL;
   if ( L == NULL )
      L = gan_symmat_alloc(rows);
   else
      L = gan_symmat_set_size(L, rows);

   if ( L == NULL ) return NULL;

   L = symmatrix_rand ( L, rows, cols );
   if ( L == NULL ) return NULL;

   /* compute Cholesky factorisation */
   L = gan_squmat_cholesky_i(L);
   if ( L == NULL ) return NULL;
   
   return L;
}

/* simple lower triangular matrix addition with scales on the input matrices */
static Gan_SquMatrix *ltmatrix_add ( Gan_SquMatrix *A, double a,
                                     Gan_SquMatrix *B, double b,
                                     Gan_SquMatrix *C )
{
   int i, j;

   if ( A->type != GAN_LOWER_TRI_MATRIX ||
        (B != NULL && B->type != GAN_LOWER_TRI_MATRIX) )
      return NULL;

   if ( B != NULL && A->size != B->size ) return NULL;
   if ( C == NULL )
      C = gan_ltmat_alloc(A->size);
   else
      C = gan_ltmat_set_size(C, A->size);

   if ( C == NULL ) return NULL;
   if ( B == NULL )
   {
      for ( i = (int)A->size-1; i >= 0; i-- )
         for ( j = i; j >= 0; j-- )
            if ( !ssel ( C, i, j, a*sgel(A,i,j) ) )
               return NULL;
   }
   else
   {
      for ( i = (int)A->size-1; i >= 0; i-- )
         for ( j = i; j >= 0; j-- )
            if ( !ssel ( C, i, j, a*sgel(A,i,j)+b*sgel(B,i,j) ) )
               return NULL;
   }
   
   return C;
}

/* build random upper triangular matrix */
static Gan_SquMatrix *utmatrix_rand ( Gan_SquMatrix *U,
                                      unsigned long rows,
                                      unsigned long cols )
{
   if ( rows != cols ) return NULL;
   if ( U == NULL )
      U = gan_symmat_alloc(rows);
   else
      U = gan_symmat_set_size(U, rows);

   if ( U == NULL ) return NULL;

   U = symmatrix_rand ( U, rows, cols );
   if ( U == NULL ) return NULL;

   /* compute Cholesky factorisation */
   U = gan_squmat_cholesky_i(U);
   if ( U == NULL ) return NULL;

   U = gan_utmat_set_size(U,rows);
   if ( U == NULL ) return NULL;
   
   return U;
}

/* simple upper triangular matrix addition with scales on the input matrices*/
static Gan_SquMatrix *utmatrix_add ( Gan_SquMatrix *A, double a,
                                     Gan_SquMatrix *B, double b,
                                     Gan_SquMatrix *C )
{
   int i, j;

   if ( A->type != GAN_UPPER_TRI_MATRIX ||
        (B != NULL && B->type != GAN_UPPER_TRI_MATRIX) )
      return NULL;

   if ( B != NULL && A->size != B->size ) return NULL;
   if ( C == NULL )
      C = gan_utmat_alloc(A->size);
   else
      C = gan_utmat_set_size(C, A->size);

   if ( C == NULL ) return NULL;
   if ( B == NULL )
   {
      for ( i = (int)A->size-1; i >= 0; i-- )
         for ( j = (int)A->size-1; j >= i; j-- )
            if ( !ssel ( C, i, j, a*sgel(A,i,j) ) )
               return NULL;
   }
   else
   {
      for ( i = (int)A->size-1; i >= 0; i-- )
         for ( j = (int)A->size-1; j >= i; j-- )
            if ( !ssel ( C, i, j, a*sgel(A,i,j)+b*sgel(B,i,j) ) )
               return NULL;
   }
   
   return C;
}

static Gan_SquMatrix *squmatrix_rand ( Gan_SquMatrix *A,
                                       Gan_SquMatrixType type,
                                       unsigned long rows,
                                       unsigned long cols )
{
   switch ( type )
   {
      case GAN_SYMMETRIC_MATRIX:
        return symmatrix_rand ( A, rows, cols );
        break;

      case GAN_DIAGONAL_MATRIX:
        return diagmatrix_rand ( A, rows, cols );
        break;

      case GAN_SCALED_IDENT_MATRIX:
        return scalImatrix_rand ( A, rows, cols );
        break;

      case GAN_LOWER_TRI_MATRIX:
        return ltmatrix_rand ( A, rows, cols );
        break;

      case GAN_UPPER_TRI_MATRIX:
        return utmatrix_rand ( A, rows, cols );
        break;

      default:
        return NULL;
        break;
   }

   /* shouldn't get here */
   return NULL;
}

static Gan_SquMatrix *squmatrix_add ( Gan_SquMatrix *A, double a,
                                      Gan_SquMatrix *B, double b,
                                      Gan_SquMatrix *C )
{
   switch ( A->type )
   {
      case GAN_SYMMETRIC_MATRIX:
        return symmatrix_add ( A, a, B, b, C );
        break;

      case GAN_DIAGONAL_MATRIX:
        return diagmatrix_add ( A, a, B, b, C );
        break;

      case GAN_SCALED_IDENT_MATRIX:
        return scalImatrix_add ( A, a, B, b, C );
        break;

      case GAN_LOWER_TRI_MATRIX:
        return ltmatrix_add ( A, a, B, b, C );
        break;

      case GAN_UPPER_TRI_MATRIX:
        return utmatrix_add ( A, a, B, b, C );
        break;

      default:
        return NULL;
        break;
   }

   /* shouldn't get here */
   return NULL;
}

/* simple square matrix identity */
static Gan_SquMatrix *scalImatrix_ident ( Gan_SquMatrix *I,
                                          unsigned long size )
{
   if ( I == NULL )
      I = gan_scalImat_alloc(size);
   else
      I = gan_scalImat_set_size(I, size);

   if ( I == NULL ) return NULL;
   if ( gan_scalImat_fill_va ( I, size, 1.0 ) == NULL ) return NULL;
   
   return I;
}

/* lower triangular matrix identity */
static Gan_SquMatrix *ltmatrix_ident ( Gan_SquMatrix *I, unsigned long size )
{
   int i;

   if ( I == NULL )
      I = gan_ltmat_alloc(size);
   else
      I = gan_ltmat_set_size(I, size);

   if ( I == NULL ) return NULL;
   if ( gan_ltmat_fill_const_q ( I, size, 0.0 ) == NULL ) return NULL;
   for ( i = (int)size-1; i >= 0; i-- )
      if ( !gan_squmat_set_el(I,i,i,1.0) ) return NULL;

   return I;
}

/* single precision functions */

/* build random vector */
static Gan_Vector_f *vectorf_rand ( Gan_Vector_f *a, unsigned long rows )
{
   int i;
   
   if ( a == NULL )
      a = gan_vecf_alloc(rows);
   else
      a = gan_vecf_set_size(a, rows);

   if ( a == NULL ) return NULL;

   for ( i = (int)rows-1; i >= 0; i-- )
      if ( !vself(a, i, gan_random_m11()) ) return NULL;

   return a;
}

/* simple vector addition with scale factors on input vectors */
static Gan_Vector_f *vectorf_add ( Gan_Vector_f *a, float as,
                                   Gan_Vector_f *b, float bs, Gan_Vector_f *c )
{
   int i;

   if ( b != NULL && a->rows != b->rows ) return NULL;
   if ( c == NULL )
      c = gan_vecf_alloc(a->rows);
   else
      c = gan_vecf_set_size(c, a->rows);

   if ( c == NULL ) return NULL;
   if ( b == NULL )
   {
      for ( i = (int)a->rows-1; i >= 0; i-- )
         if ( !vself ( c, i, as*vgelf(a,i) ) )
            return NULL;
   }
   else
   {
      for ( i = (int)a->rows-1; i >= 0; i-- )
         if ( !vself ( c, i, as*vgelf(a,i)+bs*vgelf(b,i) ) )
            return NULL;
   }

   return c;
}

/* build random matrix */
static Gan_Matrix_f *matrixf_rand ( Gan_Matrix_f *A, unsigned long rows,
                                                     unsigned long cols )
{
   int i, j, k, ip, jp;
   float angle, c, s, tmp1, tmp2;

   if ( A == NULL )
      A = gan_matf_alloc(rows, cols);
   else
      A = gan_matf_set_dims(A, rows, cols);

   if ( A == NULL ) return NULL;

   /* build diagonal matrix */
   gan_matf_fill_zero_q ( A, rows, cols );
   for ( i = (int)gan_min2(rows,cols)-1; i >= 0; i-- )
      if ( !self(A, i, i, 2.0F+(float)gan_random_m11()) ) return NULL;

   /* now apply some random Givens transformations, maintaining the rank */

   /* transformations between each row */
   for ( i = (int)rows-1; i >= 0; i-- )
      for ( j = i-1; j >= 0; j-- )
      {
         angle = (float)(M_PI*gan_random_m11());
         c = (float)cos(angle);
         s = (float)sin(angle);
         for ( k = (int)cols-1; k >= 0; k-- )
         {
            tmp1 = gelf(A,i,k);
            tmp2 = gelf(A,j,k);
            self ( A, i, k, c*tmp1 - s*tmp2 );
            self ( A, j, k, s*tmp1 + c*tmp2 );
         }

         /* swap a random pair of rows while we're at it */
         ip = gan_random() % rows;
         jp = gan_random() % rows;
         if ( ip != jp )
            for ( k = (int)cols-1; k >= 0; k-- )
            {
               tmp1 = gelf(A,ip,k);
               self ( A, ip, k, gelf(A,jp,k) );
               self ( A, jp, k, tmp1 );
            }
      }

   /* transformations between each column */
   for ( i = (int)cols-1; i >= 0; i-- )
      for ( j = i-1; j >= 0; j-- )
      {
         angle = (float)(M_PI*gan_random_m11());
         c = (float)cos(angle);
         s = (float)sin(angle);
         for ( k = (int)rows-1; k >= 0; k-- )
         {
            tmp1 = gelf(A,k,i);
            tmp2 = gelf(A,k,j);
            self ( A, k, i, c*tmp1 - s*tmp2 );
            self ( A, k, j, s*tmp1 + c*tmp2 );
         }

         /* swap a random pair of columns while we're at it */
         ip = gan_random() % cols;
         jp = gan_random() % cols;
         if ( ip != jp )
            for ( k = (int)rows-1; k >= 0; k-- )
            {
               tmp1 = gelf(A,k,ip);
               self ( A, k, ip, gelf(A,k,jp) );
               self ( A, k, jp, tmp1 );
            }
      }

   return A;
}

/* simple matrix identity */
static Gan_Matrix_f *matrixf_ident ( Gan_Matrix_f *I, unsigned long size )
{
   int i, j;

   if ( I == NULL )
      I = gan_matf_alloc(size, size);
   else
      I = gan_matf_set_dims(I, size, size);

   if ( I == NULL ) return NULL;
   for ( i = (int)I->rows-1; i >= 0; i-- )
      for ( j = (int)I->cols-1; j >= 0; j-- )
         if ( i == j )
         {
            if ( !self ( I, i, j, 1.0 ) ) return NULL;
         }
         else
         {
            if ( !self ( I, i, j, 0.0 ) ) return NULL;
         }
   
   return I;
}

/* simple matrix transpose */
static Gan_Matrix_f *matrixf_transpose ( Gan_Matrix_f *A, Gan_Matrix_f *B )
{
   int i, j;

   if ( B == NULL )
      B = gan_matf_alloc(A->cols, A->rows);
   else
   {
      if ( B == A ) return NULL;
      B = gan_matf_set_dims(B, A->cols, A->rows);
   }
   
   if ( B == NULL ) return NULL;
   for ( i = (int)A->rows-1; i >= 0; i-- )
      for ( j = (int)A->cols-1; j >= 0; j-- )
         if ( !self ( B, j, i, gelf(A,i,j) ) )
            return NULL;

   return B;
}

/* simple matrix addition with scales on the input matrices */
static Gan_Matrix_f *matrixf_add ( Gan_Matrix_f *A, float a,
                                   Gan_Matrix_f *B, float b, Gan_Matrix_f *C )
{
   int i, j;

   if ( B != NULL && (A->rows != B->rows || A->cols != B->cols) ) return NULL;
   if ( C == NULL )
      C = gan_matf_alloc(A->rows, A->cols);
   else
      C = gan_matf_set_dims(C, A->rows, A->cols);

   if ( C == NULL ) return NULL;
   if ( B == NULL )
   {
      for ( i = (int)A->rows-1; i >= 0; i-- )
         for ( j = (int)A->cols-1; j >= 0; j-- )
            if ( !self ( C, i, j, a*gelf(A,i,j) ) )
               return NULL;
   }
   else
   {
      for ( i = (int)A->rows-1; i >= 0; i-- )
         for ( j = (int)A->cols-1; j >= 0; j-- )
            if ( !self ( C, i, j, a*gelf(A,i,j)+b*gelf(B,i,j) ) )
               return NULL;
   }
   
   return C;
}

/* simple matrix/matrix multiplication */
static Gan_Matrix_f *
 matrixf_matrixf_mult ( Gan_Matrix_f *A, Gan_Matrix_f *B, Gan_Matrix_f *C )
{
   int i, j, k;

   if ( A->cols != B->rows ) return NULL;
   if ( C == NULL )
      C = gan_matf_alloc(A->rows, B->cols);
   else
      C = gan_matf_set_dims(C, A->rows, B->cols);

   if ( C == NULL ) return NULL;
   for ( i = (int)C->rows-1; i >= 0; i-- )
      for ( j = (int)C->cols-1; j >= 0; j-- )
      {
         float total = 0.0;

         for ( k = (int)A->cols-1; k >= 0; k-- )
            total += gelf(A,i,k)*gelf(B,k,j);

         if ( !self ( C, i, j, total ) ) return NULL;
      }

   return C;
}

/* simple matrix/square matrix multiplication */
static Gan_Matrix_f *
 matrixf_squmatrixf_mult ( Gan_Matrix_f *A, Gan_SquMatrix_f *B,
                           Gan_Matrix_f *C )
{
   int i, j, k;

   if ( A->cols != B->size ) return NULL;
   if ( C == NULL )
      C = gan_matf_alloc(A->rows, B->size);
   else
      C = gan_matf_set_dims(C, A->rows, B->size);

   if ( C == NULL ) return NULL;
   for ( i = (int)C->rows-1; i >= 0; i-- )
      for ( j = (int)C->cols-1; j >= 0; j-- )
      {
         float total = 0.0;

         for ( k = (int)A->cols-1; k >= 0; k-- )
            total += gelf(A,i,k)*sgelf(B,k,j);

         if ( !self ( C, i, j, total ) ) return NULL;
      }

   return C;
}

/* simple square matrix/matrix multiplication */
static Gan_Matrix_f *
 squmatrixf_matrixf_mult ( Gan_SquMatrix_f *A, Gan_Matrix_f *B,
                           Gan_Matrix_f *C )
{
   int i, j, k;

   if ( A->size != B->rows ) return NULL;
   if ( C == NULL )
      C = gan_matf_alloc(A->size, B->cols);
   else
      C = gan_matf_set_dims(C, A->size, B->cols);

   if ( C == NULL ) return NULL;
   for ( i = (int)C->rows-1; i >= 0; i-- )
      for ( j = (int)C->cols-1; j >= 0; j-- )
      {
         float total = 0.0;

         for ( k = (int)A->size-1; k >= 0; k-- )
            total += sgelf(A,i,k)*gelf(B,k,j);

         if ( !self ( C, i, j, total ) ) return NULL;
      }

   return C;
}

/* simple matrix/matrix multiplication */
static Gan_Vector_f *matrixf_vectorf_mult ( Gan_Matrix_f *A, Gan_Vector_f *b,
                                            Gan_Vector_f *c )
{
   int i, j;

   if ( A->cols != b->rows ) return NULL;
   if ( c == NULL )
      c = gan_vecf_alloc(A->rows);
   else
      c = gan_vecf_set_size(c, A->rows);

   if ( c == NULL ) return NULL;
   for ( i = (int)A->rows-1; i >= 0; i-- )
   {
      float total = 0.0;

      for ( j = (int)A->cols-1; j >= 0; j-- )
         total += gelf(A,i,j)*vgelf(b,j);

      if ( !vself ( c, i, total ) ) return NULL;
   }

   return c;
}

/* build random symmetric matrix */
static Gan_SquMatrix_f *symmatrixf_rand ( Gan_SquMatrix_f *S,
                                          unsigned long rows,
                                          unsigned long cols )
{
   int i, j, k;
   Gan_Vector_f a;

   if ( rows != cols ) return NULL;
   if ( S == NULL )
      S = gan_symmatf_alloc(rows);
   else
      S = gan_symmatf_set_size(S, rows);

   if ( S == NULL ) return NULL;

   /* allocate vector */
   if ( gan_vecf_form ( &a, rows ) == NULL ) return NULL;

   /* accumulate outer products to make a positive definite matrix */
   if ( gan_symmatf_fill_zero_q ( S, rows ) == NULL ) return NULL;
   for ( k = (int)5*rows-1; k >= 0; k-- )
   {
      if ( vectorf_rand ( &a, rows ) == NULL ) return NULL;
      for ( i = (int)rows-1; i >= 0; i-- )
         for ( j = 0; j <= i; j++ )
            if ( !gan_squmatf_inc_el ( S, i, j,
                                gan_vecf_get_el(&a,i)*gan_vecf_get_el(&a,j) ) )
               return NULL;
   }

   gan_vecf_free(&a);
   return S;
}

/* simple symmetric matrix addition with scales on the input matrices */
static Gan_SquMatrix_f *symmatrixf_add ( Gan_SquMatrix_f *A, float a,
                                         Gan_SquMatrix_f *B, float b,
                                         Gan_SquMatrix_f *C )
{
   int i, j;

   if ( A->type != GAN_SYMMETRIC_MATRIX ||
        (B != NULL && B->type != GAN_SYMMETRIC_MATRIX) )
      return NULL;

   if ( B != NULL && A->size != B->size ) return NULL;
   if ( C == NULL )
      C = gan_symmatf_alloc(A->size);
   else
      C = gan_symmatf_set_size(C, A->size);

   if ( C == NULL ) return NULL;
   if ( B == NULL )
   {
      for ( i = (int)A->size-1; i >= 0; i-- )
         for ( j = (int)A->size-1; j >= i; j-- )
            if ( !sself ( C, i, j, a*sgelf(A,i,j) ) )
               return NULL;
   }
   else
   {
      for ( i = (int)A->size-1; i >= 0; i-- )
         for ( j = (int)A->size-1; j >= i; j-- )
            if ( !sself ( C, i, j, a*sgelf(A,i,j)+b*sgelf(B,i,j) ) )
               return NULL;
   }
   
   return C;
}

/* build random diagonal matrix */
static Gan_SquMatrix_f *diagmatrixf_rand ( Gan_SquMatrix_f *D,
                                           unsigned long rows,
                                           unsigned long cols )
{
   int i;

   if ( rows != cols ) return NULL;
   if ( D == NULL )
      D = gan_diagmatf_alloc(rows);
   else
      D = gan_diagmatf_set_size(D, rows);

   if ( D == NULL ) return NULL;

   for ( i = (int)D->size-1; i >= 0; i-- )
      if ( !gan_squmatf_set_el ( D, i, i, 1.0+gan_random_01() ) )
         return NULL;

   return D;
}

/* simple diagonal matrix addition with scales on the input matrices*/
static Gan_SquMatrix_f *diagmatrixf_add ( Gan_SquMatrix_f *A, float a,
                                          Gan_SquMatrix_f *B, float b,
                                          Gan_SquMatrix_f *C )
{
   int i;

   if ( A->type != GAN_DIAGONAL_MATRIX ||
        (B != NULL && B->type != GAN_DIAGONAL_MATRIX) )
      return NULL;

   if ( B != NULL && A->size != B->size ) return NULL;
   if ( C == NULL )
      C = gan_diagmatf_alloc(A->size);
   else
      C = gan_diagmatf_set_size(C, A->size);

   if ( C == NULL ) return NULL;
   if ( B == NULL )
   {
      for ( i = (int)A->size-1; i >= 0; i-- )
         if ( !sself ( C, i, i, a*sgelf(A,i,i) ) )
            return NULL;
   }
   else
   {
      for ( i = (int)A->size-1; i >= 0; i-- )
         if ( !sself ( C, i, i, a*sgelf(A,i,i)+b*sgelf(B,i,i) ) )
            return NULL;
   }
   
   return C;
}

/* build random scaled identity matrix */
static Gan_SquMatrix_f *scalImatrixf_rand ( Gan_SquMatrix_f *sI,
                                            unsigned long rows,
                                            unsigned long cols )
{
   if ( rows != cols ) return NULL;
   if ( sI == NULL )
      sI = gan_scalImatf_alloc(rows);
   else
      sI = gan_scalImatf_set_size(sI, rows);

   if ( sI == NULL ) return NULL;

   if ( gan_scalImatf_fill_const_q ( sI, rows, (float)(1.0 + gan_random_01()) )
        == NULL )
      return NULL;

   return sI;
}

/* simple scaled identity matrix addition with scales on the input matrices */
static Gan_SquMatrix_f *scalImatrixf_add ( Gan_SquMatrix_f *A, float a,
                                           Gan_SquMatrix_f *B, float b,
                                           Gan_SquMatrix_f *C )
{
   if ( A->type != GAN_SCALED_IDENT_MATRIX ||
        (B != NULL && B->type != GAN_SCALED_IDENT_MATRIX) )
      return NULL;

   if ( B != NULL && A->size != B->size ) return NULL;
   if ( C == NULL )
      C = gan_scalImatf_alloc(A->size);
   else
      C = gan_scalImatf_set_size(C, A->size);

   if ( C == NULL ) return NULL;
   if ( A->size == 0 ) return C; /* value irrelevant for zero-size matrix */
   if ( B == NULL )
   {
      if ( gan_scalImatf_fill_const_q ( C, A->size, a*sgelf(A,0,0) ) == NULL )
         return NULL;
   }
   else
   {
      if ( gan_scalImatf_fill_const_q ( C, A->size,
                                        a*sgelf(A,0,0)+b*sgelf(B,0,0))
           == NULL )
         return NULL;
   }
   
   return C;
}


/* build random lower triangular matrix */
static Gan_SquMatrix_f *ltmatrixf_rand ( Gan_SquMatrix_f *L,
                                         unsigned long rows,
                                         unsigned long cols )
{
   if ( rows != cols ) return NULL;
   if ( L == NULL )
      L = gan_symmatf_alloc(rows);
   else
      L = gan_symmatf_set_size(L, rows);

   if ( L == NULL ) return NULL;

   L = symmatrixf_rand ( L, rows, cols );
   if ( L == NULL ) return NULL;

   /* compute Cholesky factorisation */
   L = gan_squmatf_cholesky_i(L);
   if ( L == NULL ) return NULL;
   
   return L;
}

/* simple lower triangular matrix addition with scales on the input matrices */
static Gan_SquMatrix_f *ltmatrixf_add ( Gan_SquMatrix_f *A, float a,
                                        Gan_SquMatrix_f *B, float b,
                                        Gan_SquMatrix_f *C )
{
   int i, j;

   if ( A->type != GAN_LOWER_TRI_MATRIX ||
        (B != NULL && B->type != GAN_LOWER_TRI_MATRIX) )
      return NULL;

   if ( B != NULL && A->size != B->size ) return NULL;
   if ( C == NULL )
      C = gan_ltmatf_alloc(A->size);
   else
      C = gan_ltmatf_set_size(C, A->size);

   if ( C == NULL ) return NULL;
   if ( B == NULL )
   {
      for ( i = (int)A->size-1; i >= 0; i-- )
         for ( j = i; j >= 0; j-- )
            if ( !sself ( C, i, j, a*sgelf(A,i,j) ) )
               return NULL;
   }
   else
   {
      for ( i = (int)A->size-1; i >= 0; i-- )
         for ( j = i; j >= 0; j-- )
            if ( !sself ( C, i, j, a*sgelf(A,i,j)+b*sgelf(B,i,j) ) )
               return NULL;
   }
   
   return C;
}

/* build random upper triangular matrix */
static Gan_SquMatrix_f *utmatrixf_rand ( Gan_SquMatrix_f *U,
                                         unsigned long rows,
                                         unsigned long cols )
{
   if ( rows != cols ) return NULL;
   if ( U == NULL )
      U = gan_symmatf_alloc(rows);
   else
      U = gan_symmatf_set_size(U, rows);

   if ( U == NULL ) return NULL;

   U = symmatrixf_rand ( U, rows, cols );
   if ( U == NULL ) return NULL;

   /* compute Cholesky factorisation */
   U = gan_squmatf_cholesky_i(U);
   if ( U == NULL ) return NULL;

   U = gan_utmatf_set_size(U,rows);
   if ( U == NULL ) return NULL;
   
   return U;
}

/* simple upper triangular matrix addition with scales on the input matrices*/
static Gan_SquMatrix_f *utmatrixf_add ( Gan_SquMatrix_f *A, float a,
                                        Gan_SquMatrix_f *B, float b,
                                        Gan_SquMatrix_f *C )
{
   int i, j;

   if ( A->type != GAN_UPPER_TRI_MATRIX ||
        (B != NULL && B->type != GAN_UPPER_TRI_MATRIX) )
      return NULL;

   if ( B != NULL && A->size != B->size ) return NULL;
   if ( C == NULL )
      C = gan_utmatf_alloc(A->size);
   else
      C = gan_utmatf_set_size(C, A->size);

   if ( C == NULL ) return NULL;
   if ( B == NULL )
   {
      for ( i = (int)A->size-1; i >= 0; i-- )
         for ( j = (int)A->size-1; j >= i; j-- )
            if ( !sself ( C, i, j, a*sgelf(A,i,j) ) )
               return NULL;
   }
   else
   {
      for ( i = (int)A->size-1; i >= 0; i-- )
         for ( j = (int)A->size-1; j >= i; j-- )
            if ( !sself ( C, i, j, a*sgelf(A,i,j)+b*sgelf(B,i,j) ) )
               return NULL;
   }
   
   return C;
}

static Gan_SquMatrix_f *squmatrixf_rand ( Gan_SquMatrix_f *A,
                                          Gan_SquMatrixType type,
                                          unsigned long rows,
                                          unsigned long cols )
{
   switch ( type )
   {
      case GAN_SYMMETRIC_MATRIX:
        return symmatrixf_rand ( A, rows, cols );
        break;

      case GAN_DIAGONAL_MATRIX:
        return diagmatrixf_rand ( A, rows, cols );
        break;

      case GAN_SCALED_IDENT_MATRIX:
        return scalImatrixf_rand ( A, rows, cols );
        break;

      case GAN_LOWER_TRI_MATRIX:
        return ltmatrixf_rand ( A, rows, cols );
        break;

      case GAN_UPPER_TRI_MATRIX:
        return utmatrixf_rand ( A, rows, cols );
        break;

      default:
        return NULL;
        break;
   }

   /* shouldn't get here */
   return NULL;
}

static Gan_SquMatrix_f *squmatrixf_add ( Gan_SquMatrix_f *A, float a,
                                         Gan_SquMatrix_f *B, float b,
                                         Gan_SquMatrix_f *C )
{
   switch ( A->type )
   {
      case GAN_SYMMETRIC_MATRIX:
        return symmatrixf_add ( A, a, B, b, C );
        break;

      case GAN_DIAGONAL_MATRIX:
        return diagmatrixf_add ( A, a, B, b, C );
        break;

      case GAN_SCALED_IDENT_MATRIX:
        return scalImatrixf_add ( A, a, B, b, C );
        break;

      case GAN_LOWER_TRI_MATRIX:
        return ltmatrixf_add ( A, a, B, b, C );
        break;

      case GAN_UPPER_TRI_MATRIX:
        return utmatrixf_add ( A, a, B, b, C );
        break;

      default:
        return NULL;
        break;
   }

   /* shouldn't get here */
   return NULL;
}

/* simple square matrix identity */
static Gan_SquMatrix_f *scalImatrixf_ident ( Gan_SquMatrix_f *I,
                                             unsigned long size )
{
   if ( I == NULL )
      I = gan_scalImatf_alloc(size);
   else
      I = gan_scalImatf_set_size(I, size);

   if ( I == NULL ) return NULL;
   if ( gan_scalImatf_fill_va ( I, size, 1.0 ) == NULL ) return NULL;
   
   return I;
}

/* lower triangular matrix identity */
static Gan_SquMatrix_f *
 ltmatrixf_ident ( Gan_SquMatrix_f *I, unsigned long size )
{
   int i;

   if ( I == NULL )
      I = gan_ltmatf_alloc(size);
   else
      I = gan_ltmatf_set_size(I, size);

   if ( I == NULL ) return NULL;
   if ( gan_ltmatf_fill_const_q ( I, size, 0.0 ) == NULL ) return NULL;
   for ( i = (int)size-1; i >= 0; i-- )
      if ( !gan_squmatf_set_el(I,i,i,1.0) ) return NULL;

   return I;
}

#define GAN_GENMAT_TYPE Gan_Matrix
#define GAN_GENSQUMAT_TYPE Gan_SquMatrix
#define GAN_GENVEC_TYPE Gan_Vector
#define GAN_MATRIX_ALLOC gan_mat_alloc
#define GAN_SYMMATRIX_ALLOC gan_symmat_alloc
#define GAN_VECTOR_ALLOC gan_vec_alloc
#define GAN_MATRIX_FREE gan_mat_free
#define GAN_SQUMATRIX_FREE gan_squmat_free
#define GAN_VECTOR_FREE gan_vec_free
#define GAN_VECTOR_OUTER_Q gan_vec_outer_q
#define GAN_VECTOR_OUTER_SYM_Q gan_vec_outer_sym_q
#define GAN_MATRIX_SCALE_Q gan_mat_scale_q
#define GAN_MATRIX_DIVIDE_Q gan_mat_divide_q
#define GAN_MATRIX_NEGATE_Q gan_mat_negate_q
#define GAN_MATRIX_ADD_Q gan_mat_add_q
#define GAN_MATRIX_SUB_Q gan_mat_sub_q
#define GAN_MATRIX_MULTV_Q gan_mat_multv_q
#define GAN_MATRIXT_MULTV_Q gan_matT_multv_q
#define GAN_MATRIX_SUMSQR gan_mat_sumsqr
#define GAN_MATRIX_FNORM gan_mat_Fnorm
#define GAN_MATRIX_INVERT_Q gan_mat_invert_q
#define GAN_MATRIX_RMULT_GEN gan_mat_rmult_gen
#define GAN_MATRIX_RMULT_SYM_GEN gan_mat_rmult_sym_gen
#define GAN_MATRIX_ADD_SYM_GEN gan_mat_add_sym_gen
#define GAN_MATRIX_COPY_Q gan_mat_copy_q
#define GAN_MATRIX_TPOSE_Q gan_mat_tpose_q
#define GAN_SQUMATRIX_CHOLESKY_Q gan_squmat_cholesky_q

#define GAN_VECTOR_RAND vector_rand
#define GAN_MATRIX_RAND matrix_rand
#define SYMMATRIX_RAND symmatrix_rand
#define LTMATRIX_RAND ltmatrix_rand
#define GAN_MATRIX_IDENT matrix_ident
#define MATRIX_SQUMATRIX_MULT matrix_squmatrix_mult
#define SQUMATRIX_MATRIX_MULT squmatrix_matrix_mult

/* build test function for generic double precision 2x2 matrices */
#include <gandalf/linalg/2x2mat_noc.h>
#define TEST_SMALL_MATRIX test_mat22
#define MAT_ROWS 2
#define MAT_COLS 2
#define GAN_VEC_FROM_VEC1_Q gan_vec2_from_vec_q
#define GAN_VEC_FROM_VEC2_Q gan_vec2_from_vec_q
#define GAN_VEC_SUB1_Q gan_vec2_sub_q
#define GAN_VEC_SUB2_Q gan_vec2_sub_q
#define GAN_VEC_SQRLEN1_S gan_vec2_sqrlen_s
#define GAN_VEC_SQRLEN2_S gan_vec2_sqrlen_s
#define GAN_MATL_FROM_MAT_Q gan_mat22_from_mat_q
#define GAN_MATR_FROM_MAT_Q gan_mat22_from_mat_q
#define GAN_SQUMATL_FROM_SQUMAT_Q gan_squmat22_from_squmat_q
#define GAN_SQUMATR_FROM_SQUMAT_Q gan_squmat22_from_squmat_q
#define GAN_SYMMATL_SUB_Q gan_symmat22_sub_q
#define GAN_SYMMATR_SUB_Q gan_symmat22_sub_q
#define GAN_SYMMATL_FNORM_S gan_symmat22_Fnorm_s
#define GAN_SYMMATR_FNORM_S gan_symmat22_Fnorm_s
#define MAT_THRESHOLD 1.0e-6
#include <gandalf/linalg/matrix_test_noc.c>
#undef TEST_SMALL_MATRIX
#undef MAT_ROWS
#undef MAT_COLS
#undef GAN_VEC_FROM_VEC1_Q
#undef GAN_VEC_FROM_VEC2_Q
#undef GAN_VEC_SUB1_Q
#undef GAN_VEC_SUB2_Q
#undef GAN_VEC_SQRLEN1_S
#undef GAN_VEC_SQRLEN2_S
#undef GAN_MATL_FROM_MAT_Q
#undef GAN_MATR_FROM_MAT_Q
#undef GAN_SQUMATL_FROM_SQUMAT_Q
#undef GAN_SQUMATR_FROM_SQUMAT_Q
#undef GAN_SYMMATL_SUB_Q
#undef GAN_SYMMATR_SUB_Q
#undef GAN_SYMMATL_FNORM_S
#undef GAN_SYMMATR_FNORM_S
#undef MAT_THRESHOLD
#include <gandalf/linalg/squmatrixf_noc.h>
#include <gandalf/linalg/matrixf_noc.h>

/* build test function for generic 3x3 matrices */
#include <gandalf/linalg/3x3mat_noc.h>
#define TEST_SMALL_MATRIX test_mat33
#define MAT_ROWS 3
#define MAT_COLS 3
#define GAN_VEC_FROM_VEC1_Q gan_vec3_from_vec_q
#define GAN_VEC_FROM_VEC2_Q gan_vec3_from_vec_q
#define GAN_VEC_SUB1_Q gan_vec3_sub_q
#define GAN_VEC_SUB2_Q gan_vec3_sub_q
#define GAN_VEC_SQRLEN1_S gan_vec3_sqrlen_s
#define GAN_VEC_SQRLEN2_S gan_vec3_sqrlen_s
#define GAN_MATL_FROM_MAT_Q gan_mat33_from_mat_q
#define GAN_MATR_FROM_MAT_Q gan_mat33_from_mat_q
#define GAN_SQUMATL_FROM_SQUMAT_Q gan_squmat33_from_squmat_q
#define GAN_SQUMATR_FROM_SQUMAT_Q gan_squmat33_from_squmat_q
#define GAN_SYMMATL_SUB_Q gan_symmat33_sub_q
#define GAN_SYMMATR_SUB_Q gan_symmat33_sub_q
#define GAN_SYMMATL_FNORM_S gan_symmat33_Fnorm_s
#define GAN_SYMMATR_FNORM_S gan_symmat33_Fnorm_s
#define MAT_THRESHOLD 1.0e-6
#include <gandalf/linalg/matrix_test_noc.c>
#undef TEST_SMALL_MATRIX
#undef MAT_ROWS
#undef MAT_COLS
#undef GAN_VEC_FROM_VEC1_Q
#undef GAN_VEC_FROM_VEC2_Q
#undef GAN_VEC_SUB1_Q
#undef GAN_VEC_SUB2_Q
#undef GAN_VEC_SQRLEN1_S
#undef GAN_VEC_SQRLEN2_S
#undef GAN_MATL_FROM_MAT_Q
#undef GAN_MATR_FROM_MAT_Q
#undef GAN_SQUMATL_FROM_SQUMAT_Q
#undef GAN_SQUMATR_FROM_SQUMAT_Q
#undef GAN_SYMMATL_SUB_Q
#undef GAN_SYMMATR_SUB_Q
#undef GAN_SYMMATL_FNORM_S
#undef GAN_SYMMATR_FNORM_S
#undef MAT_THRESHOLD
#include <gandalf/linalg/squmatrixf_noc.h>
#include <gandalf/linalg/matrixf_noc.h>

/* build test function for generic 4x4 matrices */
#include <gandalf/linalg/4x4mat_noc.h>
#define TEST_SMALL_MATRIX test_mat44
#define MAT_ROWS 4
#define MAT_COLS 4
#define GAN_VEC_FROM_VEC1_Q gan_vec4_from_vec_q
#define GAN_VEC_FROM_VEC2_Q gan_vec4_from_vec_q
#define GAN_VEC_SUB1_Q gan_vec4_sub_q
#define GAN_VEC_SUB2_Q gan_vec4_sub_q
#define GAN_VEC_SQRLEN1_S gan_vec4_sqrlen_s
#define GAN_VEC_SQRLEN2_S gan_vec4_sqrlen_s
#define GAN_MATL_FROM_MAT_Q gan_mat44_from_mat_q
#define GAN_MATR_FROM_MAT_Q gan_mat44_from_mat_q
#define GAN_SQUMATL_FROM_SQUMAT_Q gan_squmat44_from_squmat_q
#define GAN_SQUMATR_FROM_SQUMAT_Q gan_squmat44_from_squmat_q
#define GAN_SYMMATL_SUB_Q gan_symmat44_sub_q
#define GAN_SYMMATR_SUB_Q gan_symmat44_sub_q
#define GAN_SYMMATL_FNORM_S gan_symmat44_Fnorm_s
#define GAN_SYMMATR_FNORM_S gan_symmat44_Fnorm_s
#define MAT_THRESHOLD 1.0e-6
#include <gandalf/linalg/matrix_test_noc.c>
#undef TEST_SMALL_MATRIX
#undef MAT_ROWS
#undef MAT_COLS
#undef GAN_VEC_FROM_VEC1_Q
#undef GAN_VEC_FROM_VEC2_Q
#undef GAN_VEC_SUB1_Q
#undef GAN_VEC_SUB2_Q
#undef GAN_VEC_SQRLEN1_S
#undef GAN_VEC_SQRLEN2_S
#undef GAN_MATL_FROM_MAT_Q
#undef GAN_MATR_FROM_MAT_Q
#undef GAN_SQUMATL_FROM_SQUMAT_Q
#undef GAN_SQUMATR_FROM_SQUMAT_Q
#undef GAN_SYMMATL_SUB_Q
#undef GAN_SYMMATR_SUB_Q
#undef GAN_SYMMATL_FNORM_S
#undef GAN_SYMMATR_FNORM_S
#undef MAT_THRESHOLD
#include <gandalf/linalg/squmatrixf_noc.h>
#include <gandalf/linalg/matrixf_noc.h>

/* build test function for 2x3 matrices */
#include <gandalf/linalg/2x3mat_noc.h>
#define TEST_SMALL_MATRIX test_mat23
#define MAT_ROWS 2
#define MAT_COLS 3
#define GAN_VEC_FROM_VEC1_Q gan_vec2_from_vec_q
#define GAN_VEC_FROM_VEC2_Q gan_vec3_from_vec_q
#define GAN_VEC_SUB1_Q gan_vec2_sub_q
#define GAN_VEC_SUB2_Q gan_vec3_sub_q
#define GAN_VEC_SQRLEN1_S gan_vec2_sqrlen_s
#define GAN_VEC_SQRLEN2_S gan_vec3_sqrlen_s
#define GAN_MATL_FROM_MAT_Q gan_mat22_from_mat_q
#define GAN_MATR_FROM_MAT_Q gan_mat33_from_mat_q
#define GAN_SQUMATL_FROM_SQUMAT_Q gan_squmat22_from_squmat_q
#define GAN_SQUMATR_FROM_SQUMAT_Q gan_squmat33_from_squmat_q
#define GAN_SYMMATL_SUB_Q gan_symmat22_sub_q
#define GAN_SYMMATR_SUB_Q gan_symmat33_sub_q
#define GAN_SYMMATL_FNORM_S gan_symmat22_Fnorm_s
#define GAN_SYMMATR_FNORM_S gan_symmat33_Fnorm_s
#define MAT_THRESHOLD 1.0e-6
#include <gandalf/linalg/matrix_test_noc.c>
#undef TEST_SMALL_MATRIX
#undef MAT_ROWS
#undef MAT_COLS
#undef GAN_VEC_FROM_VEC1_Q
#undef GAN_VEC_FROM_VEC2_Q
#undef GAN_VEC_SUB1_Q
#undef GAN_VEC_SUB2_Q
#undef GAN_VEC_SQRLEN1_S
#undef GAN_VEC_SQRLEN2_S
#undef GAN_MATL_FROM_MAT_Q
#undef GAN_MATR_FROM_MAT_Q
#undef GAN_SQUMATL_FROM_SQUMAT_Q
#undef GAN_SQUMATR_FROM_SQUMAT_Q
#undef GAN_SYMMATL_SUB_Q
#undef GAN_SYMMATR_SUB_Q
#undef GAN_SYMMATL_FNORM_S
#undef GAN_SYMMATR_FNORM_S
#undef MAT_THRESHOLD
#include <gandalf/linalg/matrixf_noc.h>

/* build test function for 2x4 matrices */
#include <gandalf/linalg/2x4mat_noc.h>
#define TEST_SMALL_MATRIX test_mat24
#define MAT_ROWS 2
#define MAT_COLS 4
#define GAN_VEC_FROM_VEC1_Q gan_vec2_from_vec_q
#define GAN_VEC_FROM_VEC2_Q gan_vec4_from_vec_q
#define GAN_VEC_SUB1_Q gan_vec2_sub_q
#define GAN_VEC_SUB2_Q gan_vec4_sub_q
#define GAN_VEC_SQRLEN1_S gan_vec2_sqrlen_s
#define GAN_VEC_SQRLEN2_S gan_vec4_sqrlen_s
#define GAN_MATL_FROM_MAT_Q gan_mat22_from_mat_q
#define GAN_MATR_FROM_MAT_Q gan_mat44_from_mat_q
#define GAN_SQUMATL_FROM_SQUMAT_Q gan_squmat22_from_squmat_q
#define GAN_SQUMATR_FROM_SQUMAT_Q gan_squmat44_from_squmat_q
#define GAN_SYMMATL_SUB_Q gan_symmat22_sub_q
#define GAN_SYMMATR_SUB_Q gan_symmat44_sub_q
#define GAN_SYMMATL_FNORM_S gan_symmat22_Fnorm_s
#define GAN_SYMMATR_FNORM_S gan_symmat44_Fnorm_s
#define MAT_THRESHOLD 1.0e-6
#include <gandalf/linalg/matrix_test_noc.c>
#undef TEST_SMALL_MATRIX
#undef MAT_ROWS
#undef MAT_COLS
#undef GAN_VEC_FROM_VEC1_Q
#undef GAN_VEC_FROM_VEC2_Q
#undef GAN_VEC_SUB1_Q
#undef GAN_VEC_SUB2_Q
#undef GAN_VEC_SQRLEN1_S
#undef GAN_VEC_SQRLEN2_S
#undef GAN_MATL_FROM_MAT_Q
#undef GAN_MATR_FROM_MAT_Q
#undef GAN_SQUMATL_FROM_SQUMAT_Q
#undef GAN_SQUMATR_FROM_SQUMAT_Q
#undef GAN_SYMMATL_SUB_Q
#undef GAN_SYMMATR_SUB_Q
#undef GAN_SYMMATL_FNORM_S
#undef GAN_SYMMATR_FNORM_S
#undef MAT_THRESHOLD
#include <gandalf/linalg/matrixf_noc.h>

/* build test function for 3x4 matrices */
#include <gandalf/linalg/3x4mat_noc.h>
#define TEST_SMALL_MATRIX test_mat34
#define MAT_ROWS 3
#define MAT_COLS 4
#define GAN_VEC_FROM_VEC1_Q gan_vec3_from_vec_q
#define GAN_VEC_FROM_VEC2_Q gan_vec4_from_vec_q
#define GAN_VEC_SUB1_Q gan_vec3_sub_q
#define GAN_VEC_SUB2_Q gan_vec4_sub_q
#define GAN_VEC_SQRLEN1_S gan_vec3_sqrlen_s
#define GAN_VEC_SQRLEN2_S gan_vec4_sqrlen_s
#define GAN_MATL_FROM_MAT_Q gan_mat33_from_mat_q
#define GAN_MATR_FROM_MAT_Q gan_mat44_from_mat_q
#define GAN_SQUMATL_FROM_SQUMAT_Q gan_squmat33_from_squmat_q
#define GAN_SQUMATR_FROM_SQUMAT_Q gan_squmat44_from_squmat_q
#define GAN_SYMMATL_SUB_Q gan_symmat33_sub_q
#define GAN_SYMMATR_SUB_Q gan_symmat44_sub_q
#define GAN_SYMMATL_FNORM_S gan_symmat33_Fnorm_s
#define GAN_SYMMATR_FNORM_S gan_symmat44_Fnorm_s
#define MAT_THRESHOLD 1.0e-6
#include <gandalf/linalg/matrix_test_noc.c>
#undef TEST_SMALL_MATRIX
#undef MAT_ROWS
#undef MAT_COLS
#undef GAN_VEC_FROM_VEC1_Q
#undef GAN_VEC_FROM_VEC2_Q
#undef GAN_VEC_SUB1_Q
#undef GAN_VEC_SUB2_Q
#undef GAN_VEC_SQRLEN1_S
#undef GAN_VEC_SQRLEN2_S
#undef GAN_MATL_FROM_MAT_Q
#undef GAN_MATR_FROM_MAT_Q
#undef GAN_SQUMATL_FROM_SQUMAT_Q
#undef GAN_SQUMATR_FROM_SQUMAT_Q
#undef GAN_SYMMATL_SUB_Q
#undef GAN_SYMMATR_SUB_Q
#undef GAN_SYMMATL_FNORM_S
#undef GAN_SYMMATR_FNORM_S
#undef MAT_THRESHOLD
#include <gandalf/linalg/matrixf_noc.h>

#undef GAN_VECTOR_RAND
#undef GAN_MATRIX_RAND
#undef SYMMATRIX_RAND
#undef LTMATRIX_RAND
#undef GAN_MATRIX_IDENT
#undef MATRIX_SQUMATRIX_MULT
#undef SQUMATRIX_MATRIX_MULT

#undef GAN_GENMAT_TYPE
#undef GAN_GENSQUMAT_TYPE
#undef GAN_GENVEC_TYPE
#undef GAN_MATRIX_ALLOC
#undef GAN_SYMMATRIX_ALLOC
#undef GAN_VECTOR_ALLOC
#undef GAN_MATRIX_FREE
#undef GAN_SQUMATRIX_FREE
#undef GAN_VECTOR_FREE
#undef GAN_VECTOR_OUTER_Q
#undef GAN_VECTOR_OUTER_SYM_Q
#undef GAN_MATRIX_SCALE_Q
#undef GAN_MATRIX_DIVIDE_Q
#undef GAN_MATRIX_NEGATE_Q
#undef GAN_MATRIX_ADD_Q
#undef GAN_MATRIX_SUB_Q
#undef GAN_MATRIX_MULTV_Q
#undef GAN_MATRIXT_MULTV_Q
#undef GAN_MATRIX_SUMSQR
#undef GAN_MATRIX_FNORM
#undef GAN_MATRIX_INVERT_Q
#undef GAN_MATRIX_RMULT_GEN
#undef GAN_MATRIX_RMULT_SYM_GEN
#undef GAN_MATRIX_ADD_SYM_GEN
#undef GAN_MATRIX_COPY_Q
#undef GAN_MATRIX_TPOSE_Q
#undef GAN_SQUMATRIX_CHOLESKY_Q

#define GAN_GENMAT_TYPE Gan_Matrix_f
#define GAN_GENSQUMAT_TYPE Gan_SquMatrix_f
#define GAN_GENVEC_TYPE Gan_Vector_f
#define GAN_MATRIX_ALLOC gan_matf_alloc
#define GAN_SYMMATRIX_ALLOC gan_symmatf_alloc
#define GAN_VECTOR_ALLOC gan_vecf_alloc
#define GAN_MATRIX_FREE gan_matf_free
#define GAN_SQUMATRIX_FREE gan_squmatf_free
#define GAN_VECTOR_FREE gan_vecf_free
#define GAN_VECTOR_OUTER_Q gan_vecf_outer_q
#define GAN_VECTOR_OUTER_SYM_Q gan_vecf_outer_sym_q
#define GAN_MATRIX_SCALE_Q gan_matf_scale_q
#define GAN_MATRIX_DIVIDE_Q gan_matf_divide_q
#define GAN_MATRIX_NEGATE_Q gan_matf_negate_q
#define GAN_MATRIX_ADD_Q gan_matf_add_q
#define GAN_MATRIX_SUB_Q gan_matf_sub_q
#define GAN_MATRIX_MULTV_Q gan_matf_multv_q
#define GAN_MATRIXT_MULTV_Q gan_matTf_multv_q
#define GAN_MATRIX_SUMSQR gan_matf_sumsqr
#define GAN_MATRIX_FNORM gan_matf_Fnorm
#define GAN_MATRIX_INVERT_Q gan_matf_invert_q
#define GAN_MATRIX_RMULT_GEN gan_matf_rmult_gen
#define GAN_MATRIX_RMULT_SYM_GEN gan_matf_rmult_sym_gen
#define GAN_MATRIX_ADD_SYM_GEN gan_matf_add_sym_gen
#define GAN_MATRIX_COPY_Q gan_matf_copy_q
#define GAN_MATRIX_TPOSE_Q gan_matf_tpose_q
#define GAN_SQUMATRIX_CHOLESKY_Q gan_squmatf_cholesky_q

#define GAN_VECTOR_RAND vectorf_rand
#define GAN_MATRIX_RAND matrixf_rand
#define SYMMATRIX_RAND symmatrixf_rand
#define LTMATRIX_RAND ltmatrixf_rand
#define GAN_MATRIX_IDENT matrixf_ident
#define MATRIX_SQUMATRIX_MULT matrixf_squmatrixf_mult
#define SQUMATRIX_MATRIX_MULT squmatrixf_matrixf_mult

/* build test function for generic single precision 2x2 matrices */
#include <gandalf/linalg/2x2matf_noc.h>
#define TEST_SMALL_MATRIX test_mat22f
#define MAT_ROWS 2
#define MAT_COLS 2
#define GAN_VEC_FROM_VEC1_Q gan_vec2f_from_vecf_q
#define GAN_VEC_FROM_VEC2_Q gan_vec2f_from_vecf_q
#define GAN_VEC_SUB1_Q gan_vec2f_sub_q
#define GAN_VEC_SUB2_Q gan_vec2f_sub_q
#define GAN_VEC_SQRLEN1_S gan_vec2f_sqrlen_s
#define GAN_VEC_SQRLEN2_S gan_vec2f_sqrlen_s
#define GAN_MATL_FROM_MAT_Q gan_mat22f_from_matf_q
#define GAN_MATR_FROM_MAT_Q gan_mat22f_from_matf_q
#define GAN_SQUMATL_FROM_SQUMAT_Q gan_squmat22f_from_squmatf_q
#define GAN_SQUMATR_FROM_SQUMAT_Q gan_squmat22f_from_squmatf_q
#define GAN_SYMMATL_SUB_Q gan_symmat22f_sub_q
#define GAN_SYMMATR_SUB_Q gan_symmat22f_sub_q
#define GAN_SYMMATL_FNORM_S gan_symmat22f_Fnorm_s
#define GAN_SYMMATR_FNORM_S gan_symmat22f_Fnorm_s
#define MAT_THRESHOLD 1.0e-3
#include <gandalf/linalg/matrix_test_noc.c>
#undef TEST_SMALL_MATRIX
#undef MAT_ROWS
#undef MAT_COLS
#undef GAN_VEC_FROM_VEC1_Q
#undef GAN_VEC_FROM_VEC2_Q
#undef GAN_VEC_SUB1_Q
#undef GAN_VEC_SUB2_Q
#undef GAN_VEC_SQRLEN1_S
#undef GAN_VEC_SQRLEN2_S
#undef GAN_MATL_FROM_MAT_Q
#undef GAN_MATR_FROM_MAT_Q
#undef GAN_SQUMATL_FROM_SQUMAT_Q
#undef GAN_SQUMATR_FROM_SQUMAT_Q
#undef GAN_SYMMATL_SUB_Q
#undef GAN_SYMMATR_SUB_Q
#undef GAN_SYMMATL_FNORM_S
#undef GAN_SYMMATR_FNORM_S
#undef MAT_THRESHOLD
#include <gandalf/linalg/squmatrixf_noc.h>
#include <gandalf/linalg/matrixf_noc.h>

/* build test function for generic single precision 3x3 matrices */
#include <gandalf/linalg/3x3matf_noc.h>
#define TEST_SMALL_MATRIX test_mat33f
#define MAT_ROWS 3
#define MAT_COLS 3
#define GAN_VEC_FROM_VEC1_Q gan_vec3f_from_vecf_q
#define GAN_VEC_FROM_VEC2_Q gan_vec3f_from_vecf_q
#define GAN_VEC_SUB1_Q gan_vec3f_sub_q
#define GAN_VEC_SUB2_Q gan_vec3f_sub_q
#define GAN_VEC_SQRLEN1_S gan_vec3f_sqrlen_s
#define GAN_VEC_SQRLEN2_S gan_vec3f_sqrlen_s
#define GAN_MATL_FROM_MAT_Q gan_mat33f_from_matf_q
#define GAN_MATR_FROM_MAT_Q gan_mat33f_from_matf_q
#define GAN_SQUMATL_FROM_SQUMAT_Q gan_squmat33f_from_squmatf_q
#define GAN_SQUMATR_FROM_SQUMAT_Q gan_squmat33f_from_squmatf_q
#define GAN_SYMMATL_SUB_Q gan_symmat33f_sub_q
#define GAN_SYMMATR_SUB_Q gan_symmat33f_sub_q
#define GAN_SYMMATL_FNORM_S gan_symmat33f_Fnorm_s
#define GAN_SYMMATR_FNORM_S gan_symmat33f_Fnorm_s
#define MAT_THRESHOLD 1.0e-3
#include <gandalf/linalg/matrix_test_noc.c>
#undef TEST_SMALL_MATRIX
#undef MAT_ROWS
#undef MAT_COLS
#undef GAN_VEC_FROM_VEC1_Q
#undef GAN_VEC_FROM_VEC2_Q
#undef GAN_VEC_SUB1_Q
#undef GAN_VEC_SUB2_Q
#undef GAN_VEC_SQRLEN1_S
#undef GAN_VEC_SQRLEN2_S
#undef GAN_MATL_FROM_MAT_Q
#undef GAN_MATR_FROM_MAT_Q
#undef GAN_SQUMATL_FROM_SQUMAT_Q
#undef GAN_SQUMATR_FROM_SQUMAT_Q
#undef GAN_SYMMATL_SUB_Q
#undef GAN_SYMMATR_SUB_Q
#undef GAN_SYMMATL_FNORM_S
#undef GAN_SYMMATR_FNORM_S
#undef MAT_THRESHOLD
#include <gandalf/linalg/squmatrixf_noc.h>
#include <gandalf/linalg/matrixf_noc.h>

/* build test function for generic single precision 4x4 matrices */
#include <gandalf/linalg/4x4matf_noc.h>
#define TEST_SMALL_MATRIX test_mat44f
#define MAT_ROWS 4
#define MAT_COLS 4
#define GAN_VEC_FROM_VEC1_Q gan_vec4f_from_vecf_q
#define GAN_VEC_FROM_VEC2_Q gan_vec4f_from_vecf_q
#define GAN_VEC_SUB1_Q gan_vec4f_sub_q
#define GAN_VEC_SUB2_Q gan_vec4f_sub_q
#define GAN_VEC_SQRLEN1_S gan_vec4f_sqrlen_s
#define GAN_VEC_SQRLEN2_S gan_vec4f_sqrlen_s
#define GAN_MATL_FROM_MAT_Q gan_mat44f_from_matf_q
#define GAN_MATR_FROM_MAT_Q gan_mat44f_from_matf_q
#define GAN_SQUMATL_FROM_SQUMAT_Q gan_squmat44f_from_squmatf_q
#define GAN_SQUMATR_FROM_SQUMAT_Q gan_squmat44f_from_squmatf_q
#define GAN_SYMMATL_SUB_Q gan_symmat44f_sub_q
#define GAN_SYMMATR_SUB_Q gan_symmat44f_sub_q
#define GAN_SYMMATL_FNORM_S gan_symmat44f_Fnorm_s
#define GAN_SYMMATR_FNORM_S gan_symmat44f_Fnorm_s
#define MAT_THRESHOLD 1.0e-3
#include <gandalf/linalg/matrix_test_noc.c>
#undef TEST_SMALL_MATRIX
#undef MAT_ROWS
#undef MAT_COLS
#undef GAN_VEC_FROM_VEC1_Q
#undef GAN_VEC_FROM_VEC2_Q
#undef GAN_VEC_SUB1_Q
#undef GAN_VEC_SUB2_Q
#undef GAN_VEC_SQRLEN1_S
#undef GAN_VEC_SQRLEN2_S
#undef GAN_MATL_FROM_MAT_Q
#undef GAN_MATR_FROM_MAT_Q
#undef GAN_SQUMATL_FROM_SQUMAT_Q
#undef GAN_SQUMATR_FROM_SQUMAT_Q
#undef GAN_SYMMATL_SUB_Q
#undef GAN_SYMMATR_SUB_Q
#undef GAN_SYMMATL_FNORM_S
#undef GAN_SYMMATR_FNORM_S
#undef MAT_THRESHOLD
#include <gandalf/linalg/squmatrixf_noc.h>
#include <gandalf/linalg/matrixf_noc.h>

/* build test function for single precision 2x3 matrices */
#include <gandalf/linalg/2x3matf_noc.h>
#define TEST_SMALL_MATRIX test_mat23f
#define MAT_ROWS 2
#define MAT_COLS 3
#define GAN_VEC_FROM_VEC1_Q gan_vec2f_from_vecf_q
#define GAN_VEC_FROM_VEC2_Q gan_vec3f_from_vecf_q
#define GAN_VEC_SUB1_Q gan_vec2f_sub_q
#define GAN_VEC_SUB2_Q gan_vec3f_sub_q
#define GAN_VEC_SQRLEN1_S gan_vec2f_sqrlen_s
#define GAN_VEC_SQRLEN2_S gan_vec3f_sqrlen_s
#define GAN_MATL_FROM_MAT_Q gan_mat22f_from_matf_q
#define GAN_MATR_FROM_MAT_Q gan_mat33f_from_matf_q
#define GAN_SQUMATL_FROM_SQUMAT_Q gan_squmat22f_from_squmatf_q
#define GAN_SQUMATR_FROM_SQUMAT_Q gan_squmat33f_from_squmatf_q
#define GAN_SYMMATL_SUB_Q gan_symmat22f_sub_q
#define GAN_SYMMATR_SUB_Q gan_symmat33f_sub_q
#define GAN_SYMMATL_FNORM_S gan_symmat22f_Fnorm_s
#define GAN_SYMMATR_FNORM_S gan_symmat33f_Fnorm_s
#define MAT_THRESHOLD 1.0e-4
#include <gandalf/linalg/matrix_test_noc.c>
#undef TEST_SMALL_MATRIX
#undef MAT_ROWS
#undef MAT_COLS
#undef GAN_VEC_FROM_VEC1_Q
#undef GAN_VEC_FROM_VEC2_Q
#undef GAN_VEC_SUB1_Q
#undef GAN_VEC_SUB2_Q
#undef GAN_VEC_SQRLEN1_S
#undef GAN_VEC_SQRLEN2_S
#undef GAN_MATL_FROM_MAT_Q
#undef GAN_MATR_FROM_MAT_Q
#undef GAN_SQUMATL_FROM_SQUMAT_Q
#undef GAN_SQUMATR_FROM_SQUMAT_Q
#undef GAN_SYMMATL_SUB_Q
#undef GAN_SYMMATR_SUB_Q
#undef GAN_SYMMATL_FNORM_S
#undef GAN_SYMMATR_FNORM_S
#undef MAT_THRESHOLD
#include <gandalf/linalg/matrixf_noc.h>

/* build test function for single precision 2x4 matrices */
#include <gandalf/linalg/2x4matf_noc.h>
#define TEST_SMALL_MATRIX test_mat24f
#define MAT_ROWS 2
#define MAT_COLS 4
#define GAN_VEC_FROM_VEC1_Q gan_vec2f_from_vecf_q
#define GAN_VEC_FROM_VEC2_Q gan_vec4f_from_vecf_q
#define GAN_VEC_SUB1_Q gan_vec2f_sub_q
#define GAN_VEC_SUB2_Q gan_vec4f_sub_q
#define GAN_VEC_SQRLEN1_S gan_vec2f_sqrlen_s
#define GAN_VEC_SQRLEN2_S gan_vec4f_sqrlen_s
#define GAN_MATL_FROM_MAT_Q gan_mat22f_from_matf_q
#define GAN_MATR_FROM_MAT_Q gan_mat44f_from_matf_q
#define GAN_SQUMATL_FROM_SQUMAT_Q gan_squmat22f_from_squmatf_q
#define GAN_SQUMATR_FROM_SQUMAT_Q gan_squmat44f_from_squmatf_q
#define GAN_SYMMATL_SUB_Q gan_symmat22f_sub_q
#define GAN_SYMMATR_SUB_Q gan_symmat44f_sub_q
#define GAN_SYMMATL_FNORM_S gan_symmat22f_Fnorm_s
#define GAN_SYMMATR_FNORM_S gan_symmat44f_Fnorm_s
#define MAT_THRESHOLD 1.0e-4
#include <gandalf/linalg/matrix_test_noc.c>
#undef TEST_SMALL_MATRIX
#undef MAT_ROWS
#undef MAT_COLS
#undef GAN_VEC_FROM_VEC1_Q
#undef GAN_VEC_FROM_VEC2_Q
#undef GAN_VEC_SUB1_Q
#undef GAN_VEC_SUB2_Q
#undef GAN_VEC_SQRLEN1_S
#undef GAN_VEC_SQRLEN2_S
#undef GAN_MATL_FROM_MAT_Q
#undef GAN_MATR_FROM_MAT_Q
#undef GAN_SQUMATL_FROM_SQUMAT_Q
#undef GAN_SQUMATR_FROM_SQUMAT_Q
#undef GAN_SYMMATL_SUB_Q
#undef GAN_SYMMATR_SUB_Q
#undef GAN_SYMMATL_FNORM_S
#undef GAN_SYMMATR_FNORM_S
#undef MAT_THRESHOLD
#include <gandalf/linalg/matrixf_noc.h>

/* build test function for single precision 3x4 matrices */
#include <gandalf/linalg/3x4matf_noc.h>
#define TEST_SMALL_MATRIX test_mat34f
#define MAT_ROWS 3
#define MAT_COLS 4
#define GAN_VEC_FROM_VEC1_Q gan_vec3f_from_vecf_q
#define GAN_VEC_FROM_VEC2_Q gan_vec4f_from_vecf_q
#define GAN_VEC_SUB1_Q gan_vec3f_sub_q
#define GAN_VEC_SUB2_Q gan_vec4f_sub_q
#define GAN_VEC_SQRLEN1_S gan_vec3f_sqrlen_s
#define GAN_VEC_SQRLEN2_S gan_vec4f_sqrlen_s
#define GAN_MATL_FROM_MAT_Q gan_mat33f_from_matf_q
#define GAN_MATR_FROM_MAT_Q gan_mat44f_from_matf_q
#define GAN_SQUMATL_FROM_SQUMAT_Q gan_squmat33f_from_squmatf_q
#define GAN_SQUMATR_FROM_SQUMAT_Q gan_squmat44f_from_squmatf_q
#define GAN_SYMMATL_SUB_Q gan_symmat33f_sub_q
#define GAN_SYMMATR_SUB_Q gan_symmat44f_sub_q
#define GAN_SYMMATL_FNORM_S gan_symmat33f_Fnorm_s
#define GAN_SYMMATR_FNORM_S gan_symmat44f_Fnorm_s
#define MAT_THRESHOLD 1.0e-4
#include <gandalf/linalg/matrix_test_noc.c>
#undef TEST_SMALL_MATRIX
#undef MAT_ROWS
#undef MAT_COLS
#undef GAN_VEC_FROM_VEC1_Q
#undef GAN_VEC_FROM_VEC2_Q
#undef GAN_VEC_SUB1_Q
#undef GAN_VEC_SUB2_Q
#undef GAN_VEC_SQRLEN1_S
#undef GAN_VEC_SQRLEN2_S
#undef GAN_MATL_FROM_MAT_Q
#undef GAN_MATR_FROM_MAT_Q
#undef GAN_SQUMATL_FROM_SQUMAT_Q
#undef GAN_SQUMATR_FROM_SQUMAT_Q
#undef GAN_SYMMATL_SUB_Q
#undef GAN_SYMMATR_SUB_Q
#undef GAN_SYMMATL_FNORM_S
#undef GAN_SYMMATR_FNORM_S
#undef MAT_THRESHOLD
#include <gandalf/linalg/matrixf_noc.h>

#undef GAN_VECTOR_RAND
#undef GAN_MATRIX_RAND
#undef GAN_MATRIX_IDENT
#undef SYMMATRIX_RAND
#undef LTMATRIX_RAND
#undef MATRIX_SQUMATRIX_MULT
#undef SQUMATRIX_MATRIX_MULT

#undef GAN_GENMAT_TYPE
#undef GAN_GENSQUMAT_TYPE
#undef GAN_GENVEC_TYPE
#undef GAN_MATRIX_ALLOC
#undef GAN_SYMMATRIX_ALLOC
#undef GAN_VECTOR_ALLOC
#undef GAN_MATRIX_FREE
#undef GAN_SQUMATRIX_FREE
#undef GAN_VECTOR_FREE
#undef GAN_VECTOR_OUTER_Q
#undef GAN_VECTOR_OUTER_SYM_Q
#undef GAN_MATRIX_SCALE_Q
#undef GAN_MATRIX_DIVIDE_Q
#undef GAN_MATRIX_NEGATE_Q
#undef GAN_MATRIX_ADD_Q
#undef GAN_MATRIX_SUB_Q
#undef GAN_MATRIX_MULTV_Q
#undef GAN_MATRIXT_MULTV_Q
#undef GAN_MATRIX_SUMSQR
#undef GAN_MATRIX_FNORM
#undef GAN_MATRIX_INVERT_Q
#undef GAN_MATRIX_RMULT_GEN
#undef GAN_MATRIX_RMULT_SYM_GEN
#undef GAN_MATRIX_ADD_SYM_GEN
#undef GAN_MATRIX_COPY_Q
#undef GAN_MATRIX_TPOSE_Q
#undef GAN_SQUMATRIX_CHOLESKY_Q

#define GAN_GENMAT_TYPE Gan_SquMatrix
#define GAN_GENSQUMAT_TYPE Gan_SquMatrix
#define GAN_GENVEC_TYPE Gan_Vector
#define GAN_MATRIX_ALLOC gan_squmat_alloc
#define GAN_SYMMATRIX_ALLOC gan_symmat_alloc
#define GAN_VECTOR_ALLOC gan_vec_alloc
#define GAN_MATRIX_FREE gan_squmat_free
#define GAN_SQUMATRIX_FREE gan_squmat_free
#define GAN_VECTOR_FREE gan_vec_free
#define GAN_VECTOR_OUTER_Q gan_vec_outer_q
#define GAN_VECTOR_OUTER_SYM_Q gan_vec_outer_sym_q
#define GAN_MATRIX_SCALE_Q gan_squmat_scale_q
#define GAN_MATRIX_DIVIDE_Q gan_squmat_divide_q
#define GAN_MATRIX_NEGATE_Q gan_squmat_negate_q
#define GAN_MATRIX_ADD_Q gan_squmat_add_q
#define GAN_MATRIX_SUB_Q gan_squmat_sub_q
#define GAN_MATRIX_MULTV_Q gan_squmat_multv_q
#define GAN_MATRIXT_MULTV_Q gan_squmatT_multv_q
#define GAN_MATRIX_SUMSQR gan_squmat_sumsqr
#define GAN_MATRIX_FNORM gan_squmat_Fnorm
#define GAN_MATRIX_INVERT_Q gan_squmat_invert_q
#define GAN_MATRIX_RMULT_GEN gan_mat_rmult_gen
#define GAN_MATRIX_RMULT_SYM_GEN gan_mat_rmult_sym_gen
#define GAN_MATRIX_ADD_SYM_GEN gan_mat_add_sym_gen
#define GAN_MATRIX_COPY_Q gan_mat_copy_q
#define GAN_MATRIX_TPOSE_Q gan_mat_tpose_q
#define GAN_SQUMATRIX_CHOLESKY_Q gan_squmat_cholesky_q

#define GAN_VECTOR_RAND vector_rand
#define GAN_MATRIX_RAND symmatrix_rand
#define SYMMATRIX_RAND symmatrix_rand
#define LTMATRIX_RAND ltmatrix_rand
#define GAN_MATRIX_IDENT scalImatrix_ident
#define MATRIX_SQUMATRIX_MULT matrix_squmatrix_mult
#define SQUMATRIX_MATRIX_MULT squmatrix_matrix_mult

/* build test function for symmetric 2x2 matrices */
#include <gandalf/linalg/2x2symmat_noc.h>
#define TEST_SMALL_MATRIX test_symmat22
#define MAT_ROWS 2
#define MAT_COLS 2
#define GAN_VEC_FROM_VEC1_Q gan_vec2_from_vec_q
#define GAN_VEC_FROM_VEC2_Q gan_vec2_from_vec_q
#define GAN_VEC_SUB1_Q gan_vec2_sub_q
#define GAN_VEC_SUB2_Q gan_vec2_sub_q
#define GAN_VEC_SQRLEN1_S gan_vec2_sqrlen_s
#define GAN_VEC_SQRLEN2_S gan_vec2_sqrlen_s
#define GAN_MAT_FROM_MAT_Q gan_squmat22_from_squmat_q
#define GAN_LTMAT_SRMULTT_I gan_ltmat22_srmultT_i
#define MAT_THRESHOLD 1.0e-6
#include <gandalf/linalg/matrix_test_noc.c>
#undef TEST_SMALL_MATRIX
#undef MAT_ROWS
#undef MAT_COLS
#undef GAN_VEC_FROM_VEC1_Q
#undef GAN_VEC_FROM_VEC2_Q
#undef GAN_VEC_SUB1_Q
#undef GAN_VEC_SUB2_Q
#undef GAN_VEC_SQRLEN1_S
#undef GAN_VEC_SQRLEN2_S
#undef GAN_MAT_FROM_MAT_Q
#undef GAN_LTMAT_SRMULTT_I
#undef MAT_THRESHOLD
#include <gandalf/linalg/symmatrixf_noc.h>
#include <gandalf/linalg/squmatrixf_noc.h>
#include <gandalf/linalg/matrixf_noc.h>

/* build test function for symmetric 3x3 matrices */
#include <gandalf/linalg/3x3symmat_noc.h>
#define TEST_SMALL_MATRIX test_symmat33
#define MAT_ROWS 3
#define MAT_COLS 3
#define GAN_VEC_FROM_VEC1_Q gan_vec3_from_vec_q
#define GAN_VEC_FROM_VEC2_Q gan_vec3_from_vec_q
#define GAN_VEC_SUB1_Q gan_vec3_sub_q
#define GAN_VEC_SUB2_Q gan_vec3_sub_q
#define GAN_VEC_SQRLEN1_S gan_vec3_sqrlen_s
#define GAN_VEC_SQRLEN2_S gan_vec3_sqrlen_s
#define GAN_MAT_FROM_MAT_Q gan_squmat33_from_squmat_q
#define GAN_LTMAT_SRMULTT_I gan_ltmat33_srmultT_i
#define MAT_THRESHOLD 1.0e-6
#include <gandalf/linalg/matrix_test_noc.c>
#undef TEST_SMALL_MATRIX
#undef MAT_ROWS
#undef MAT_COLS
#undef GAN_VEC_FROM_VEC1_Q
#undef GAN_VEC_FROM_VEC2_Q
#undef GAN_VEC_SUB1_Q
#undef GAN_VEC_SUB2_Q
#undef GAN_VEC_SQRLEN1_S
#undef GAN_VEC_SQRLEN2_S
#undef GAN_MAT_FROM_MAT_Q
#undef GAN_LTMAT_SRMULTT_I
#undef MAT_THRESHOLD
#include <gandalf/linalg/symmatrixf_noc.h>
#include <gandalf/linalg/squmatrixf_noc.h>
#include <gandalf/linalg/matrixf_noc.h>

/* build test function for symmetric 4x4 matrices */
#include <gandalf/linalg/4x4symmat_noc.h>
#define TEST_SMALL_MATRIX test_symmat44
#define MAT_ROWS 4
#define MAT_COLS 4
#define GAN_VEC_FROM_VEC1_Q gan_vec4_from_vec_q
#define GAN_VEC_FROM_VEC2_Q gan_vec4_from_vec_q
#define GAN_VEC_SUB1_Q gan_vec4_sub_q
#define GAN_VEC_SUB2_Q gan_vec4_sub_q
#define GAN_VEC_SQRLEN1_S gan_vec4_sqrlen_s
#define GAN_VEC_SQRLEN2_S gan_vec4_sqrlen_s
#define GAN_MAT_FROM_MAT_Q gan_squmat44_from_squmat_q
#define GAN_LTMAT_SRMULTT_I gan_ltmat44_srmultT_i
#define MAT_THRESHOLD 1.0e-6
#include <gandalf/linalg/matrix_test_noc.c>
#undef TEST_SMALL_MATRIX
#undef MAT_ROWS
#undef MAT_COLS
#undef GAN_VEC_FROM_VEC1_Q
#undef GAN_VEC_FROM_VEC2_Q
#undef GAN_VEC_SUB1_Q
#undef GAN_VEC_SUB2_Q
#undef GAN_VEC_SQRLEN1_S
#undef GAN_VEC_SQRLEN2_S
#undef GAN_MAT_FROM_MAT_Q
#undef GAN_LTMAT_SRMULTT_I
#undef MAT_THRESHOLD
#include <gandalf/linalg/symmatrixf_noc.h>
#include <gandalf/linalg/squmatrixf_noc.h>
#include <gandalf/linalg/matrixf_noc.h>

#undef GAN_VECTOR_RAND
#undef GAN_MATRIX_RAND
#undef SYMMATRIX_RAND
#undef LTMATRIX_RAND
#undef GAN_MATRIX_IDENT
#undef MATRIX_SQUMATRIX_MULT
#undef SQUMATRIX_MATRIX_MULT

#define GAN_VECTOR_RAND vector_rand
#define GAN_MATRIX_RAND ltmatrix_rand
#define GAN_MATRIX_IDENT ltmatrix_ident
#define SYMMATRIX_RAND symmatrix_rand
#define LTMATRIX_RAND ltmatrix_rand
#define MATRIX_SQUMATRIX_MULT matrix_squmatrix_mult
#define SQUMATRIX_MATRIX_MULT squmatrix_matrix_mult

/* build test function for lower triangular 2x2 matrices */
#include <gandalf/linalg/2x2ltmat_noc.h>
#define TEST_SMALL_MATRIX test_ltmat22
#define MAT_ROWS 2
#define MAT_COLS 2
#define GAN_VEC_FROM_VEC1_Q gan_vec2_from_vec_q
#define GAN_VEC_FROM_VEC2_Q gan_vec2_from_vec_q
#define GAN_VEC_SUB1_Q gan_vec2_sub_q
#define GAN_VEC_SUB2_Q gan_vec2_sub_q
#define GAN_VEC_SQRLEN1_S gan_vec2_sqrlen_s
#define GAN_VEC_SQRLEN2_S gan_vec2_sqrlen_s
#define GAN_MAT_FROM_MAT_Q gan_squmat22_from_squmat_q
#define MAT_THRESHOLD 1.0e-6
#include <gandalf/linalg/matrix_test_noc.c>
#undef TEST_SMALL_MATRIX
#undef MAT_ROWS
#undef MAT_COLS
#undef GAN_VEC_FROM_VEC1_Q
#undef GAN_VEC_FROM_VEC2_Q
#undef GAN_VEC_SUB1_Q
#undef GAN_VEC_SUB2_Q
#undef GAN_VEC_SQRLEN1_S
#undef GAN_VEC_SQRLEN2_S
#undef GAN_MAT_FROM_MAT_Q
#undef MAT_THRESHOLD
#include <gandalf/linalg/squmatrixf_noc.h>
#include <gandalf/linalg/matrixf_noc.h>

/* build test function for lower triangular 3x3 matrices */
#include <gandalf/linalg/3x3ltmat_noc.h>
#define TEST_SMALL_MATRIX test_ltmat33
#define MAT_ROWS 3
#define MAT_COLS 3
#define GAN_VEC_FROM_VEC1_Q gan_vec3_from_vec_q
#define GAN_VEC_FROM_VEC2_Q gan_vec3_from_vec_q
#define GAN_VEC_SUB1_Q gan_vec3_sub_q
#define GAN_VEC_SUB2_Q gan_vec3_sub_q
#define GAN_VEC_SQRLEN1_S gan_vec3_sqrlen_s
#define GAN_VEC_SQRLEN2_S gan_vec3_sqrlen_s
#define GAN_MAT_FROM_MAT_Q gan_squmat33_from_squmat_q
#define MAT_THRESHOLD 1.0e-6
#include <gandalf/linalg/matrix_test_noc.c>
#undef TEST_SMALL_MATRIX
#undef MAT_ROWS
#undef MAT_COLS
#undef GAN_VEC_FROM_VEC1_Q
#undef GAN_VEC_FROM_VEC2_Q
#undef GAN_VEC_SUB1_Q
#undef GAN_VEC_SUB2_Q
#undef GAN_VEC_SQRLEN1_S
#undef GAN_VEC_SQRLEN2_S
#undef GAN_MAT_FROM_MAT_Q
#undef MAT_THRESHOLD
#include <gandalf/linalg/squmatrixf_noc.h>
#include <gandalf/linalg/matrixf_noc.h>

/* build test function for lower triangular 4x4 matrices */
#include <gandalf/linalg/4x4ltmat_noc.h>
#define TEST_SMALL_MATRIX test_ltmat44
#define MAT_ROWS 4
#define MAT_COLS 4
#define GAN_VEC_FROM_VEC1_Q gan_vec4_from_vec_q
#define GAN_VEC_FROM_VEC2_Q gan_vec4_from_vec_q
#define GAN_VEC_SUB1_Q gan_vec4_sub_q
#define GAN_VEC_SUB2_Q gan_vec4_sub_q
#define GAN_VEC_SQRLEN1_S gan_vec4_sqrlen_s
#define GAN_VEC_SQRLEN2_S gan_vec4_sqrlen_s
#define GAN_MAT_FROM_MAT_Q gan_squmat44_from_squmat_q
#define MAT_THRESHOLD 1.0e-6
#include <gandalf/linalg/matrix_test_noc.c>
#undef TEST_SMALL_MATRIX
#undef MAT_ROWS
#undef MAT_COLS
#undef GAN_VEC_FROM_VEC1_Q
#undef GAN_VEC_FROM_VEC2_Q
#undef GAN_VEC_SUB1_Q
#undef GAN_VEC_SUB2_Q
#undef GAN_VEC_SQRLEN1_S
#undef GAN_VEC_SQRLEN2_S
#undef GAN_MAT_FROM_MAT_Q
#undef MAT_THRESHOLD
#include <gandalf/linalg/squmatrixf_noc.h>
#include <gandalf/linalg/matrixf_noc.h>

#undef GAN_VECTOR_RAND
#undef GAN_MATRIX_RAND
#undef SYMMATRIX_RAND
#undef LTMATRIX_RAND
#undef GAN_MATRIX_IDENT
#undef MATRIX_SQUMATRIX_MULT
#undef SQUMATRIX_MATRIX_MULT

#undef GAN_GENMAT_TYPE
#undef GAN_GENSQUMAT_TYPE
#undef GAN_GENVEC_TYPE
#undef GAN_MATRIX_ALLOC
#undef GAN_SYMMATRIX_ALLOC
#undef GAN_VECTOR_ALLOC
#undef GAN_MATRIX_FREE
#undef GAN_SQUMATRIX_FREE
#undef GAN_VECTOR_FREE
#undef GAN_VECTOR_OUTER_Q
#undef GAN_VECTOR_OUTER_SYM_Q
#undef GAN_MATRIX_SCALE_Q
#undef GAN_MATRIX_DIVIDE_Q
#undef GAN_MATRIX_NEGATE_Q
#undef GAN_MATRIX_ADD_Q
#undef GAN_MATRIX_SUB_Q
#undef GAN_MATRIX_MULTV_Q
#undef GAN_MATRIXT_MULTV_Q
#undef GAN_MATRIX_SUMSQR
#undef GAN_MATRIX_FNORM
#undef GAN_MATRIX_INVERT_Q
#undef GAN_MATRIX_RMULT_GEN
#undef GAN_MATRIX_RMULT_SYM_GEN
#undef GAN_MATRIX_ADD_SYM_GEN
#undef GAN_MATRIX_COPY_Q
#undef GAN_MATRIX_TPOSE_Q
#undef GAN_SQUMATRIX_CHOLESKY_Q

#define GAN_GENMAT_TYPE Gan_SquMatrix_f
#define GAN_GENSQUMAT_TYPE Gan_SquMatrix_f
#define GAN_GENVEC_TYPE Gan_Vector_f
#define GAN_MATRIX_ALLOC gan_squmatf_alloc
#define GAN_SYMMATRIX_ALLOC gan_symmatf_alloc
#define GAN_VECTOR_ALLOC gan_vecf_alloc
#define GAN_MATRIX_FREE gan_squmatf_free
#define GAN_SQUMATRIX_FREE gan_squmatf_free
#define GAN_VECTOR_FREE gan_vecf_free
#define GAN_VECTOR_OUTER_Q gan_vecf_outer_q
#define GAN_VECTOR_OUTER_SYM_Q gan_vecf_outer_sym_q
#define GAN_MATRIX_SCALE_Q gan_squmatf_scale_q
#define GAN_MATRIX_DIVIDE_Q gan_squmatf_divide_q
#define GAN_MATRIX_NEGATE_Q gan_squmatf_negate_q
#define GAN_MATRIX_ADD_Q gan_squmatf_add_q
#define GAN_MATRIX_SUB_Q gan_squmatf_sub_q
#define GAN_MATRIX_MULTV_Q gan_squmatf_multv_q
#define GAN_MATRIXT_MULTV_Q gan_squmatTf_multv_q
#define GAN_MATRIX_SUMSQR gan_squmatf_sumsqr
#define GAN_MATRIX_FNORM gan_squmatf_Fnorm
#define GAN_MATRIX_INVERT_Q gan_squmatf_invert_q
#define GAN_MATRIX_RMULT_GEN gan_matf_rmult_gen
#define GAN_MATRIX_RMULT_SYM_GEN gan_matf_rmult_sym_gen
#define GAN_MATRIX_ADD_SYM_GEN gan_matf_add_sym_gen
#define GAN_MATRIX_COPY_Q gan_matf_copy_q
#define GAN_MATRIX_TPOSE_Q gan_matf_tpose_q
#define GAN_SQUMATRIX_CHOLESKY_Q gan_squmatf_cholesky_q

#define GAN_VECTOR_RAND vectorf_rand
#define GAN_MATRIX_RAND symmatrixf_rand
#define SYMMATRIX_RAND symmatrixf_rand
#define LTMATRIX_RAND ltmatrixf_rand
#define GAN_MATRIX_IDENT scalImatrixf_ident
#define MATRIX_SQUMATRIX_MULT matrixf_squmatrixf_mult
#define SQUMATRIX_MATRIX_MULT squmatrixf_matrixf_mult

/* build test function for symmetric single precision 2x2 matrices */
#include <gandalf/linalg/2x2symmatf_noc.h>
#define TEST_SMALL_MATRIX test_symmat22f
#define MAT_ROWS 2
#define MAT_COLS 2
#define GAN_VEC_FROM_VEC1_Q gan_vec2f_from_vecf_q
#define GAN_VEC_FROM_VEC2_Q gan_vec2f_from_vecf_q
#define GAN_VEC_SUB1_Q gan_vec2f_sub_q
#define GAN_VEC_SUB2_Q gan_vec2f_sub_q
#define GAN_VEC_SQRLEN1_S gan_vec2f_sqrlen_s
#define GAN_VEC_SQRLEN2_S gan_vec2f_sqrlen_s
#define GAN_MAT_FROM_MAT_Q gan_squmat22f_from_squmatf_q
#define GAN_LTMAT_SRMULTT_I gan_ltmat22f_srmultT_i
#define MAT_THRESHOLD 1.0e-4
#include <gandalf/linalg/matrix_test_noc.c>
#undef TEST_SMALL_MATRIX
#undef MAT_ROWS
#undef MAT_COLS
#undef GAN_VEC_FROM_VEC1_Q
#undef GAN_VEC_FROM_VEC2_Q
#undef GAN_VEC_SUB1_Q
#undef GAN_VEC_SUB2_Q
#undef GAN_VEC_SQRLEN1_S
#undef GAN_VEC_SQRLEN2_S
#undef GAN_MAT_FROM_MAT_Q
#undef GAN_LTMAT_SRMULTT_I
#undef MAT_THRESHOLD
#include <gandalf/linalg/symmatrixf_noc.h>
#include <gandalf/linalg/squmatrixf_noc.h>
#include <gandalf/linalg/matrixf_noc.h>

/* build test function for symmetric single precision 3x3 matrices */
#include <gandalf/linalg/3x3symmatf_noc.h>
#define TEST_SMALL_MATRIX test_symmat33f
#define MAT_ROWS 3
#define MAT_COLS 3
#define GAN_VEC_FROM_VEC1_Q gan_vec3f_from_vecf_q
#define GAN_VEC_FROM_VEC2_Q gan_vec3f_from_vecf_q
#define GAN_VEC_SUB1_Q gan_vec3f_sub_q
#define GAN_VEC_SUB2_Q gan_vec3f_sub_q
#define GAN_VEC_SQRLEN1_S gan_vec3f_sqrlen_s
#define GAN_VEC_SQRLEN2_S gan_vec3f_sqrlen_s
#define GAN_MAT_FROM_MAT_Q gan_squmat33f_from_squmatf_q
#define GAN_LTMAT_SRMULTT_I gan_ltmat33f_srmultT_i
#define MAT_THRESHOLD 1.0e-4
#include <gandalf/linalg/matrix_test_noc.c>
#undef TEST_SMALL_MATRIX
#undef MAT_ROWS
#undef MAT_COLS
#undef GAN_VEC_FROM_VEC1_Q
#undef GAN_VEC_FROM_VEC2_Q
#undef GAN_VEC_SUB1_Q
#undef GAN_VEC_SUB2_Q
#undef GAN_VEC_SQRLEN1_S
#undef GAN_VEC_SQRLEN2_S
#undef GAN_MAT_FROM_MAT_Q
#undef GAN_LTMAT_SRMULTT_I
#undef MAT_THRESHOLD
#include <gandalf/linalg/symmatrixf_noc.h>
#include <gandalf/linalg/squmatrixf_noc.h>
#include <gandalf/linalg/matrixf_noc.h>

/* build test function for symmetric single precision 4x4 matrices */
#include <gandalf/linalg/4x4symmatf_noc.h>
#define TEST_SMALL_MATRIX test_symmat44f
#define MAT_ROWS 4
#define MAT_COLS 4
#define GAN_VEC_FROM_VEC1_Q gan_vec4f_from_vecf_q
#define GAN_VEC_FROM_VEC2_Q gan_vec4f_from_vecf_q
#define GAN_VEC_SUB1_Q gan_vec4f_sub_q
#define GAN_VEC_SUB2_Q gan_vec4f_sub_q
#define GAN_VEC_SQRLEN1_S gan_vec4f_sqrlen_s
#define GAN_VEC_SQRLEN2_S gan_vec4f_sqrlen_s
#define GAN_MAT_FROM_MAT_Q gan_squmat44f_from_squmatf_q
#define GAN_LTMAT_SRMULTT_I gan_ltmat44f_srmultT_i
#define MAT_THRESHOLD 1.0e-4
#include <gandalf/linalg/matrix_test_noc.c>
#undef TEST_SMALL_MATRIX
#undef MAT_ROWS
#undef MAT_COLS
#undef GAN_VEC_FROM_VEC1_Q
#undef GAN_VEC_FROM_VEC2_Q
#undef GAN_VEC_SUB1_Q
#undef GAN_VEC_SUB2_Q
#undef GAN_VEC_SQRLEN1_S
#undef GAN_VEC_SQRLEN2_S
#undef GAN_MAT_FROM_MAT_Q
#undef GAN_LTMAT_SRMULTT_I
#undef MAT_THRESHOLD
#include <gandalf/linalg/symmatrixf_noc.h>
#include <gandalf/linalg/squmatrixf_noc.h>
#include <gandalf/linalg/matrixf_noc.h>

#undef GAN_VECTOR_RAND
#undef GAN_MATRIX_RAND
#undef SYMMATRIX_RAND
#undef LTMATRIX_RAND
#undef GAN_MATRIX_IDENT
#undef MATRIX_SQUMATRIX_MULT
#undef SQUMATRIX_MATRIX_MULT

#define GAN_VECTOR_RAND vectorf_rand
#define GAN_MATRIX_RAND ltmatrixf_rand
#define GAN_MATRIX_IDENT ltmatrixf_ident
#define SYMMATRIX_RAND symmatrixf_rand
#define LTMATRIX_RAND ltmatrixf_rand
#define MATRIX_SQUMATRIX_MULT matrixf_squmatrixf_mult
#define SQUMATRIX_MATRIX_MULT squmatrixf_matrixf_mult

/* build test function for lower triangular single precision 2x2 matrices */
#include <gandalf/linalg/2x2ltmatf_noc.h>
#define TEST_SMALL_MATRIX test_ltmat22f
#define MAT_ROWS 2
#define MAT_COLS 2
#define GAN_VEC_FROM_VEC1_Q gan_vec2f_from_vecf_q
#define GAN_VEC_FROM_VEC2_Q gan_vec2f_from_vecf_q
#define GAN_VEC_SUB1_Q gan_vec2f_sub_q
#define GAN_VEC_SUB2_Q gan_vec2f_sub_q
#define GAN_VEC_SQRLEN1_S gan_vec2f_sqrlen_s
#define GAN_VEC_SQRLEN2_S gan_vec2f_sqrlen_s
#define GAN_MAT_FROM_MAT_Q gan_squmat22f_from_squmatf_q
#define MAT_THRESHOLD 1.0e-4
#include <gandalf/linalg/matrix_test_noc.c>
#undef TEST_SMALL_MATRIX
#undef MAT_ROWS
#undef MAT_COLS
#undef GAN_VEC_FROM_VEC1_Q
#undef GAN_VEC_FROM_VEC2_Q
#undef GAN_VEC_SUB1_Q
#undef GAN_VEC_SUB2_Q
#undef GAN_VEC_SQRLEN1_S
#undef GAN_VEC_SQRLEN2_S
#undef GAN_MAT_FROM_MAT_Q
#undef MAT_THRESHOLD
#include <gandalf/linalg/squmatrixf_noc.h>
#include <gandalf/linalg/matrixf_noc.h>

/* build test function for lower triangular single precision 3x3 matrices */
#include <gandalf/linalg/3x3ltmatf_noc.h>
#define TEST_SMALL_MATRIX test_ltmat33f
#define MAT_ROWS 3
#define MAT_COLS 3
#define GAN_VEC_FROM_VEC1_Q gan_vec3f_from_vecf_q
#define GAN_VEC_FROM_VEC2_Q gan_vec3f_from_vecf_q
#define GAN_VEC_SUB1_Q gan_vec3f_sub_q
#define GAN_VEC_SUB2_Q gan_vec3f_sub_q
#define GAN_VEC_SQRLEN1_S gan_vec3f_sqrlen_s
#define GAN_VEC_SQRLEN2_S gan_vec3f_sqrlen_s
#define GAN_MAT_FROM_MAT_Q gan_squmat33f_from_squmatf_q
#define MAT_THRESHOLD 1.0e-4
#include <gandalf/linalg/matrix_test_noc.c>
#undef TEST_SMALL_MATRIX
#undef MAT_ROWS
#undef MAT_COLS
#undef GAN_VEC_FROM_VEC1_Q
#undef GAN_VEC_FROM_VEC2_Q
#undef GAN_VEC_SUB1_Q
#undef GAN_VEC_SUB2_Q
#undef GAN_VEC_SQRLEN1_S
#undef GAN_VEC_SQRLEN2_S
#undef GAN_MAT_FROM_MAT_Q
#undef MAT_THRESHOLD
#include <gandalf/linalg/squmatrixf_noc.h>
#include <gandalf/linalg/matrixf_noc.h>

/* build test function for lower triangular single precision 4x4 matrices */
#include <gandalf/linalg/4x4ltmatf_noc.h>
#define TEST_SMALL_MATRIX test_ltmat44f
#define MAT_ROWS 4
#define MAT_COLS 4
#define GAN_VEC_FROM_VEC1_Q gan_vec4f_from_vecf_q
#define GAN_VEC_FROM_VEC2_Q gan_vec4f_from_vecf_q
#define GAN_VEC_SUB1_Q gan_vec4f_sub_q
#define GAN_VEC_SUB2_Q gan_vec4f_sub_q
#define GAN_VEC_SQRLEN1_S gan_vec4f_sqrlen_s
#define GAN_VEC_SQRLEN2_S gan_vec4f_sqrlen_s
#define GAN_MAT_FROM_MAT_Q gan_squmat44f_from_squmatf_q
#define MAT_THRESHOLD 1.0e-4
#include <gandalf/linalg/matrix_test_noc.c>
#undef TEST_SMALL_MATRIX
#undef MAT_ROWS
#undef MAT_COLS
#undef GAN_VEC_FROM_VEC1_Q
#undef GAN_VEC_FROM_VEC2_Q
#undef GAN_VEC_SUB1_Q
#undef GAN_VEC_SUB2_Q
#undef GAN_VEC_SQRLEN1_S
#undef GAN_VEC_SQRLEN2_S
#undef GAN_MAT_FROM_MAT_Q
#undef MAT_THRESHOLD
#include <gandalf/linalg/squmatrixf_noc.h>
#include <gandalf/linalg/matrixf_noc.h>

#undef GAN_VECTOR_RAND
#undef GAN_MATRIX_RAND
#undef SYMMATRIX_RAND
#undef LTMATRIX_RAND
#undef GAN_MATRIX_IDENT
#undef MATRIX_SQUMATRIX_MULT
#undef SQUMATRIX_MATRIX_MULT

#undef GAN_GENMAT_TYPE
#undef GAN_GENSQUMAT_TYPE
#undef GAN_GENVEC_TYPE
#undef GAN_MATRIX_ALLOC
#undef GAN_SYMMATRIX_ALLOC
#undef GAN_VECTOR_ALLOC
#undef GAN_MATRIX_FREE
#undef GAN_SQUMATRIX_FREE
#undef GAN_VECTOR_FREE
#undef GAN_VECTOR_OUTER_Q
#undef GAN_VECTOR_OUTER_SYM_Q
#undef GAN_MATRIX_SCALE_Q
#undef GAN_MATRIX_DIVIDE_Q
#undef GAN_MATRIX_NEGATE_Q
#undef GAN_MATRIX_ADD_Q
#undef GAN_MATRIX_SUB_Q
#undef GAN_MATRIX_MULTV_Q
#undef GAN_MATRIXT_MULTV_Q
#undef GAN_MATRIX_SUMSQR
#undef GAN_MATRIX_FNORM
#undef GAN_MATRIX_INVERT_Q
#undef GAN_MATRIX_RMULT_GEN
#undef GAN_MATRIX_RMULT_SYM_GEN
#undef GAN_MATRIX_ADD_SYM_GEN
#undef GAN_MATRIX_COPY_Q
#undef GAN_MATRIX_TPOSE_Q
#undef GAN_SQUMATRIX_CHOLESKY_Q

static Gan_Bool test_vec3(void)
{
   Gan_Vector3 r, s, t;

   /* test cross-product */
   (void)gan_vec3_fill_q ( &r, 1.0, 2.0, 3.0 );
   s = gan_vec3_fill_s ( 4.0, 5.0, 6.0 );
   t = gan_vec3_cross_s ( &r, &s );
   (void)gan_vec3_cross_q ( &r, &s, &t );

   /* make sure cross-product is perpendicular to the input vectors */
   cu_assert ( fabs(gan_vec3_dot_q(&r,&t)) < 1.0e-6 );
   cu_assert ( fabs(gan_vec3_dot_q(&s,&t)) < 1.0e-6 );

   return GAN_TRUE;
}

static Gan_Bool test_vec3f(void)
{
   Gan_Vector3_f r, s, t;

   /* test cross-product */
   (void)gan_vec3f_fill_q ( &r, 1.0, 2.0, 3.0 );
   s = gan_vec3f_fill_s ( 4.0, 5.0, 6.0 );
   t = gan_vec3f_cross_s ( &r, &s );
   (void)gan_vec3f_cross_q ( &r, &s, &t );

   /* make sure cross-product is perpendicular to the input vectors */
   cu_assert ( fabs(gan_vec3f_dot_q(&r,&t)) < 1.0e-4 );
   cu_assert ( fabs(gan_vec3f_dot_q(&s,&t)) < 1.0e-4 );

   return GAN_TRUE;
}

/* Tests for general size vectors. We apply an operation both using Gandalf
 * functions and simplified functions defined in this test module.
 */
static Gan_Bool test_vec_gen(void)
{
   Gan_Vector *a, *b, *bp, *c, *cp;

   /* allocate vectors used in tests */
   a  = gan_vec_alloc(0); cu_assert ( a  != NULL );
   b  = gan_vec_alloc(0); cu_assert ( b  != NULL );
   bp = gan_vec_alloc(0); cu_assert ( bp != NULL );
   c  = gan_vec_alloc(0); cu_assert ( c  != NULL );
   cp = gan_vec_alloc(0); cu_assert ( cp != NULL );

   /* test general vector copy */
   a = vector_rand ( a, 4 ); cu_assert ( a != NULL );
   bp = vector_add ( a, 1.0, NULL, 0.0, bp ); cu_assert ( bp != NULL );
   b = gan_vec_copy_q ( a, b ); cu_assert ( b != NULL );
   bp = vector_add ( bp, 1.0, b, -1.0, bp ); cu_assert ( bp != NULL );
   cu_assert ( gan_vec_sqrlen(bp) < 1.0e-6 );

   /* test general vector multiply by scalar */
   a = vector_rand ( a, 4 ); cu_assert ( a != NULL );
   bp = vector_add ( a, 4.5, NULL, 0.0, bp ); cu_assert ( bp != NULL );
   b = gan_vec_scale_q ( a, 4.5, b ); cu_assert ( b != NULL );
   bp = vector_add ( bp, 1.0, b, -1.0, bp ); cu_assert ( bp != NULL );
   cu_assert ( gan_vec_sqrlen(bp) < 1.0e-6 );

   /* test general vector multiply by scalar in-place */
   a = vector_rand ( a, 4 ); cu_assert ( a != NULL );
   bp = vector_add ( a, 4.5, NULL, 0.0, bp ); cu_assert ( bp != NULL );
   a = gan_vec_scale_i ( a, 4.5 ); cu_assert ( a != NULL );
   bp = vector_add ( bp, 1.0, a, -1.0, bp ); cu_assert ( bp != NULL );
   cu_assert ( gan_vec_sqrlen(bp) < 1.0e-6 );

   /* test general vector divide by scalar */
   a = vector_rand ( a, 4 ); cu_assert ( a != NULL );
   bp = vector_add ( a, 1.0/4.5, NULL, 0.0, bp ); cu_assert (bp != NULL);
   b = gan_vec_divide_q ( a, 4.5, b ); cu_assert ( b != NULL );
   bp = vector_add ( bp, 1.0, b, -1.0, bp ); cu_assert ( bp != NULL );
   cu_assert ( gan_vec_sqrlen(bp) < 1.0e-6 );

   /* test general vector divide by scalar in-place */
   a = vector_rand ( a, 4 ); cu_assert ( a != NULL );
   bp = vector_add ( a, 1.0/4.5, NULL, 0.0, bp ); cu_assert (bp != NULL);
   a = gan_vec_divide_i ( a, 4.5 ); cu_assert ( a != NULL );
   bp = vector_add ( bp, 1.0, a, -1.0, bp ); cu_assert ( bp != NULL );
   cu_assert ( gan_vec_sqrlen(bp) < 1.0e-6 );

   /* test general vector negation */
   a = vector_rand ( a, 4 ); cu_assert ( a != NULL );
   bp = vector_add ( a, -1.0, NULL, 0.0, bp ); cu_assert (bp != NULL);
   c = gan_vec_negate_q ( a, c ); cu_assert ( c != NULL );
   bp = vector_add ( bp, 1.0, c, -1.0, bp ); cu_assert ( bp != NULL );
   cu_assert ( gan_vec_sqrlen(bp) < 1.0e-6 );

   /* test general vector negation in-place */
   a = vector_rand ( a, 4 ); cu_assert ( a != NULL );
   bp = vector_add ( a, -1.0, NULL, 0.0, bp ); cu_assert (bp != NULL);
   a = gan_vec_negate_i(a); cu_assert ( a != NULL );
   bp = vector_add ( bp, 1.0, a, -1.0, bp ); cu_assert ( bp != NULL );
   cu_assert ( gan_vec_sqrlen(bp) < 1.0e-6 );

   /* test general vector addition */
   a = vector_rand ( a, 4 ); cu_assert ( a != NULL );
   b = vector_rand ( b, 4 ); cu_assert ( b != NULL );
   cp = vector_add ( a, 1.0, b, 1.0, cp ); cu_assert ( cp != NULL );
   c = gan_vec_add_q ( a, b, c ); cu_assert ( c != NULL );
   cp = vector_add ( cp, 1.0, c, -1.0, cp ); cu_assert ( cp != NULL );
   cu_assert ( gan_vec_sqrlen(cp) < 1.0e-6 );
   
   /* test in-place general vector addition in first argument */
   a = vector_rand ( a, 4 ); cu_assert ( a != NULL );
   b = vector_rand ( b, 4 ); cu_assert ( b != NULL );
   cp = vector_add ( a, 1.0, b, 1.0, cp ); cu_assert ( cp != NULL );
   a = gan_vec_add_i1 ( a, b ); cu_assert ( a != NULL );
   cp = vector_add ( cp, 1.0, a, -1.0, cp ); cu_assert ( cp != NULL );
   cu_assert ( gan_vec_sqrlen(cp) < 1.0e-6 );
   
   /* test in-place general vector addition in second argument */
   a = vector_rand ( a, 4 ); cu_assert ( a != NULL );
   b = vector_rand ( b, 4 ); cu_assert ( b != NULL );
   cp = vector_add ( a, 1.0, b, 1.0, cp ); cu_assert ( cp != NULL );
   b = gan_vec_add_i2 ( a, b ); cu_assert ( b != NULL );
   cp = vector_add ( cp, 1.0, b, -1.0, cp ); cu_assert ( cp != NULL );
   cu_assert ( gan_vec_sqrlen(cp) < 1.0e-6 );
   
   /* test general vector subtraction */
   a = vector_rand ( a, 4 ); cu_assert ( a != NULL );
   b = vector_rand ( b, 4 ); cu_assert ( b != NULL );
   cp = vector_add ( a, 1.0, b, -1.0, cp ); cu_assert ( cp != NULL );
   c = gan_vec_sub_q ( a, b, c ); cu_assert ( c != NULL );
   cp = vector_add ( cp, 1.0, c, -1.0, cp ); cu_assert ( cp != NULL );
   cu_assert ( gan_vec_sqrlen(cp) < 1.0e-6 );

   /* test in-place general vector subtraction in first argument */
   a = vector_rand ( a, 4 ); cu_assert ( a != NULL );
   b = vector_rand ( b, 4 ); cu_assert ( b != NULL );
   cp = vector_add ( a, 1.0, b, -1.0, cp ); cu_assert ( cp != NULL );
   a = gan_vec_sub_i1 ( a, b ); cu_assert ( a != NULL );
   cp = vector_add ( cp, 1.0, a, -1.0, cp ); cu_assert ( cp != NULL );
   cu_assert ( gan_vec_sqrlen(cp) < 1.0e-6 );
   
   /* test in-place general vector subtraction in second argument */
   a = vector_rand ( a, 4 ); cu_assert ( a != NULL );
   b = vector_rand ( b, 4 ); cu_assert ( b != NULL );
   cp = vector_add ( a, 1.0, b, -1.0, cp ); cu_assert ( cp != NULL );
   b = gan_vec_sub_i2 ( a, b ); cu_assert ( b != NULL );
   cp = vector_add ( cp, 1.0, b, -1.0, cp ); cu_assert ( cp != NULL );
   cu_assert ( gan_vec_sqrlen(cp) < 1.0e-6 );
   
   /* free vectors and return with success */
   gan_vec_free_va ( cp, c, bp, b, a, NULL );
   return GAN_TRUE;
}

/* Tests for general size single precision vectors. We apply an operation both
 * using Gandalf functions and simplified functions defined in this test
 * module.
 */
static Gan_Bool test_vecf_gen(void)
{
   Gan_Vector_f *a, *b, *bp, *c, *cp;

   /* allocate vectors used in tests */
   a  = gan_vecf_alloc(0); cu_assert ( a  != NULL );
   b  = gan_vecf_alloc(0); cu_assert ( b  != NULL );
   bp = gan_vecf_alloc(0); cu_assert ( bp != NULL );
   c  = gan_vecf_alloc(0); cu_assert ( c  != NULL );
   cp = gan_vecf_alloc(0); cu_assert ( cp != NULL );

   /* test general vector copy */
   a = vectorf_rand ( a, 4 ); cu_assert ( a != NULL );
   bp = vectorf_add ( a, 1.0F, NULL, 0.0F, bp ); cu_assert ( bp != NULL );
   b = gan_vecf_copy_q ( a, b ); cu_assert ( b != NULL );
   bp = vectorf_add ( bp, 1.0F, b, -1.0F, bp ); cu_assert ( bp != NULL );
   cu_assert ( gan_vecf_sqrlen(bp) < 1.0e-4F );

   /* test general vector multiply by scalar */
   a = vectorf_rand ( a, 4 ); cu_assert ( a != NULL );
   bp = vectorf_add ( a, 4.5F, NULL, 0.0F, bp ); cu_assert ( bp != NULL );
   b = gan_vecf_scale_q ( a, 4.5F, b ); cu_assert ( b != NULL );
   bp = vectorf_add ( bp, 1.0F, b, -1.0F, bp ); cu_assert ( bp != NULL );
   cu_assert ( gan_vecf_sqrlen(bp) < 1.0e-4F );

   /* test general vector multiply by scalar in-place */
   a = vectorf_rand ( a, 4 ); cu_assert ( a != NULL );
   bp = vectorf_add ( a, 4.5F, NULL, 0.0F, bp ); cu_assert ( bp != NULL );
   a = gan_vecf_scale_i ( a, 4.5F ); cu_assert ( a != NULL );
   bp = vectorf_add ( bp, 1.0F, a, -1.0F, bp ); cu_assert ( bp != NULL );
   cu_assert ( gan_vecf_sqrlen(bp) < 1.0e-4F );

   /* test general vector divide by scalar */
   a = vectorf_rand ( a, 4 ); cu_assert ( a != NULL );
   bp = vectorf_add ( a, 1.0F/4.5F, NULL, 0.0F, bp ); cu_assert (bp != NULL);
   b = gan_vecf_divide_q ( a, 4.5F, b ); cu_assert ( b != NULL );
   bp = vectorf_add ( bp, 1.0F, b, -1.0F, bp ); cu_assert ( bp != NULL );
   cu_assert ( gan_vecf_sqrlen(bp) < 1.0e-4F );

   /* test general vector divide by scalar in-place */
   a = vectorf_rand ( a, 4 ); cu_assert ( a != NULL );
   bp = vectorf_add ( a, 1.0F/4.5F, NULL, 0.0F, bp ); cu_assert (bp != NULL);
   a = gan_vecf_divide_i ( a, 4.5F ); cu_assert ( a != NULL );
   bp = vectorf_add ( bp, 1.0F, a, -1.0F, bp ); cu_assert ( bp != NULL );
   cu_assert ( gan_vecf_sqrlen(bp) < 1.0e-4F );

   /* test general vector negation */
   a = vectorf_rand ( a, 4 ); cu_assert ( a != NULL );
   bp = vectorf_add ( a, -1.0F, NULL, 0.0F, bp ); cu_assert (bp != NULL);
   c = gan_vecf_negate_q ( a, c ); cu_assert ( c != NULL );
   bp = vectorf_add ( bp, 1.0F, c, -1.0F, bp ); cu_assert ( bp != NULL );
   cu_assert ( gan_vecf_sqrlen(bp) < 1.0e-4F );

   /* test general vector negation in-place */
   a = vectorf_rand ( a, 4 ); cu_assert ( a != NULL );
   bp = vectorf_add ( a, -1.0F, NULL, 0.0F, bp ); cu_assert (bp != NULL);
   a = gan_vecf_negate_i(a); cu_assert ( a != NULL );
   bp = vectorf_add ( bp, 1.0F, a, -1.0F, bp ); cu_assert ( bp != NULL );
   cu_assert ( gan_vecf_sqrlen(bp) < 1.0e-4F );

   /* test general vector addition */
   a = vectorf_rand ( a, 4 ); cu_assert ( a != NULL );
   b = vectorf_rand ( b, 4 ); cu_assert ( b != NULL );
   cp = vectorf_add ( a, 1.0F, b, 1.0F, cp ); cu_assert ( cp != NULL );
   c = gan_vecf_add_q ( a, b, c ); cu_assert ( c != NULL );
   cp = vectorf_add ( cp, 1.0F, c, -1.0F, cp ); cu_assert ( cp != NULL );
   cu_assert ( gan_vecf_sqrlen(cp) < 1.0e-4F );
   
   /* test in-place general vector addition in first argument */
   a = vectorf_rand ( a, 4 ); cu_assert ( a != NULL );
   b = vectorf_rand ( b, 4 ); cu_assert ( b != NULL );
   cp = vectorf_add ( a, 1.0F, b, 1.0F, cp ); cu_assert ( cp != NULL );
   a = gan_vecf_add_i1 ( a, b ); cu_assert ( a != NULL );
   cp = vectorf_add ( cp, 1.0F, a, -1.0F, cp ); cu_assert ( cp != NULL );
   cu_assert ( gan_vecf_sqrlen(cp) < 1.0e-4F );
   
   /* test in-place general vector addition in second argument */
   a = vectorf_rand ( a, 4 ); cu_assert ( a != NULL );
   b = vectorf_rand ( b, 4 ); cu_assert ( b != NULL );
   cp = vectorf_add ( a, 1.0F, b, 1.0F, cp ); cu_assert ( cp != NULL );
   b = gan_vecf_add_i2 ( a, b ); cu_assert ( b != NULL );
   cp = vectorf_add ( cp, 1.0F, b, -1.0F, cp ); cu_assert ( cp != NULL );
   cu_assert ( gan_vecf_sqrlen(cp) < 1.0e-4F );
   
   /* test general vector subtraction */
   a = vectorf_rand ( a, 4 ); cu_assert ( a != NULL );
   b = vectorf_rand ( b, 4 ); cu_assert ( b != NULL );
   cp = vectorf_add ( a, 1.0F, b, -1.0F, cp ); cu_assert ( cp != NULL );
   c = gan_vecf_sub_q ( a, b, c ); cu_assert ( c != NULL );
   cp = vectorf_add ( cp, 1.0F, c, -1.0F, cp ); cu_assert ( cp != NULL );
   cu_assert ( gan_vecf_sqrlen(cp) < 1.0e-4F );

   /* test in-place general vector subtraction in first argument */
   a = vectorf_rand ( a, 4 ); cu_assert ( a != NULL );
   b = vectorf_rand ( b, 4 ); cu_assert ( b != NULL );
   cp = vectorf_add ( a, 1.0F, b, -1.0F, cp ); cu_assert ( cp != NULL );
   a = gan_vecf_sub_i1 ( a, b ); cu_assert ( a != NULL );
   cp = vectorf_add ( cp, 1.0F, a, -1.0F, cp ); cu_assert ( cp != NULL );
   cu_assert ( gan_vecf_sqrlen(cp) < 1.0e-4F );
   
   /* test in-place general vector subtraction in second argument */
   a = vectorf_rand ( a, 4 ); cu_assert ( a != NULL );
   b = vectorf_rand ( b, 4 ); cu_assert ( b != NULL );
   cp = vectorf_add ( a, 1.0F, b, -1.0F, cp ); cu_assert ( cp != NULL );
   b = gan_vecf_sub_i2 ( a, b ); cu_assert ( b != NULL );
   cp = vectorf_add ( cp, 1.0F, b, -1.0F, cp ); cu_assert ( cp != NULL );
   cu_assert ( gan_vecf_sqrlen(cp) < 1.0e-4F );
   
   /* free vectors and return with success */
   gan_vecf_free_va ( cp, c, bp, b, a, NULL );
   return GAN_TRUE;
}

/* Tests for general size matrices. We apply an operation both using Gandalf
 * functions and simplified functions defined in this test module.
 */
static Gan_Bool test_mat_gen(void)
{
   Gan_Matrix *A, *AT, *B, *BT, *C, *Cp;
   Gan_Vector *a, *b, *bp;
   Gan_SquMatrix *S, *T;

   /* allocate matrices and vectors used in tests */
   A  = gan_mat_alloc ( 0, 0 ); cu_assert ( A  != NULL );
   AT = gan_mat_alloc ( 0, 0 ); cu_assert ( AT != NULL );
   B  = gan_mat_alloc ( 0, 0 ); cu_assert ( B  != NULL );
   BT = gan_mat_alloc ( 0, 0 ); cu_assert ( BT != NULL );
   C  = gan_mat_alloc ( 0, 0 ); cu_assert ( C  != NULL );
   Cp = gan_mat_alloc ( 0, 0 ); cu_assert ( Cp != NULL );
   a  = gan_vec_alloc(0); cu_assert ( a  != NULL );
   b  = gan_vec_alloc(0); cu_assert ( b  != NULL );
   bp = gan_vec_alloc(0); cu_assert ( bp != NULL );
   S = gan_symmat_alloc(0); cu_assert ( S != NULL );
   T = gan_symmat_alloc(0); cu_assert ( T != NULL );

   /* test general matrix copy */
   A = matrix_rand ( A, 3, 4 ); cu_assert ( A != NULL );
   Cp = matrix_add ( A, 1.0, NULL, 0.0, Cp ); cu_assert ( Cp != NULL );
   C = gan_mat_copy_q ( A, C ); cu_assert ( C != NULL );
   Cp = matrix_add ( Cp, 1.0, C, -1.0, Cp ); cu_assert ( Cp != NULL );
   cu_assert ( gan_mat_Fnorm(Cp) < 1.0e-6 );

   /* test general matrix multiply by scalar */
   A = matrix_rand ( A, 3, 4 ); cu_assert ( A != NULL );
   Cp = matrix_add ( A, 4.5, NULL, 0.0, Cp ); cu_assert ( Cp != NULL );
   C = gan_mat_scale_q ( A, 4.5, C ); cu_assert ( C != NULL );
   Cp = matrix_add ( Cp, 1.0, C, -1.0, Cp ); cu_assert ( Cp != NULL );
   cu_assert ( gan_mat_Fnorm(Cp) < 1.0e-6 );

   /* test general matrix multiply by scalar in-place */
   A = matrix_rand ( A, 3, 4 ); cu_assert ( A != NULL );
   Cp = matrix_add ( A, 4.5, NULL, 0.0, Cp ); cu_assert ( Cp != NULL );
   A = gan_mat_scale_i ( A, 4.5 ); cu_assert ( a != NULL );
   Cp = matrix_add ( Cp, 1.0, A, -1.0, Cp ); cu_assert ( Cp != NULL );
   cu_assert ( gan_mat_Fnorm(Cp) < 1.0e-6 );

   /* test general matrix divide by scalar */
   A = matrix_rand ( A, 3, 4 ); cu_assert ( A != NULL );
   Cp = matrix_add ( A, 1.0/4.5, NULL, 0.0, Cp ); cu_assert (Cp != NULL);
   C = gan_mat_divide_q ( A, 4.5, C ); cu_assert ( C != NULL );
   Cp = matrix_add ( Cp, 1.0, C, -1.0, Cp ); cu_assert ( Cp != NULL );
   cu_assert ( gan_mat_Fnorm(Cp) < 1.0e-6 );

   /* test general matrix divide by scalar in-place */
   A = matrix_rand ( A, 3, 4 ); cu_assert ( A != NULL );
   Cp = matrix_add ( A, 1.0/4.5, NULL, 0.0, Cp ); cu_assert (Cp != NULL);
   A = gan_mat_divide_i ( A, 4.5 ); cu_assert ( A != NULL );
   Cp = matrix_add ( Cp, 1.0, A, -1.0, Cp ); cu_assert ( Cp != NULL );
   cu_assert ( gan_mat_Fnorm(Cp) < 1.0e-6 );

   /* test general matrix transpose */
   A = matrix_rand ( A, 3, 4 ); cu_assert ( A != NULL );
   Cp = matrix_transpose ( A, Cp ); cu_assert ( Cp != NULL );
   C = gan_mat_tpose_q ( A, C ); cu_assert ( C != NULL );
   Cp = matrix_add ( Cp, 1.0, C, -1.0, Cp ); cu_assert ( Cp != NULL );
   cu_assert ( gan_mat_Fnorm(Cp) < 1.0e-6 );

   /* test general matrix negation */
   A = matrix_rand ( A, 3, 4 ); cu_assert ( A != NULL );
   Cp = matrix_add ( A, -1.0, NULL, 0.0, Cp ); cu_assert (Cp != NULL);
   C = gan_mat_negate_q ( A, C ); cu_assert ( C != NULL );
   Cp = matrix_add ( Cp, 1.0, C, -1.0, Cp ); cu_assert ( Cp != NULL );
   cu_assert ( gan_mat_Fnorm(Cp) < 1.0e-6 );

   /* test general matrix negation in-place */
   A = matrix_rand ( A, 3, 4 ); cu_assert ( A != NULL );
   Cp = matrix_add ( A, -1.0, NULL, 0.0, Cp ); cu_assert (Cp != NULL);
   A = gan_mat_negate_i(A); cu_assert ( A != NULL );
   Cp = matrix_add ( Cp, 1.0, A, -1.0, Cp ); cu_assert ( Cp != NULL );
   cu_assert ( gan_mat_Fnorm(Cp) < 1.0e-6 );

   /* test general matrix addition */
   A = matrix_rand ( A, 3, 4 ); cu_assert ( A != NULL );
   B = matrix_rand ( B, 3, 4 ); cu_assert ( B != NULL );
   Cp = matrix_add ( A, 1.0, B, 1.0, Cp ); cu_assert ( Cp != NULL );
   C = gan_mat_add_q ( A, B, C ); cu_assert ( C != NULL );
   Cp = matrix_add ( Cp, 1.0, C, -1.0, Cp ); cu_assert ( Cp != NULL );
   cu_assert ( gan_mat_Fnorm(Cp) < 1.0e-6 );
   
   /* test in-place general matrix addition in first argument */
   A = matrix_rand ( A, 3, 4 ); cu_assert ( A != NULL );
   B = matrix_rand ( B, 3, 4 ); cu_assert ( B != NULL );
   Cp = matrix_add ( A, 1.0, B, 1.0, Cp ); cu_assert ( Cp != NULL );
   A = gan_mat_add_i1 ( A, B ); cu_assert ( A != NULL );
   Cp = matrix_add ( Cp, 1.0, A, -1.0, Cp ); cu_assert ( Cp != NULL );
   cu_assert ( gan_mat_Fnorm(Cp) < 1.0e-6 );
   
   /* test in-place general matrix addition in second argument */
   A = matrix_rand ( A, 3, 4 ); cu_assert ( A != NULL );
   B = matrix_rand ( B, 3, 4 ); cu_assert ( B != NULL );
   Cp = matrix_add ( A, 1.0, B, 1.0, Cp ); cu_assert ( Cp != NULL );
   B = gan_mat_add_i2 ( A, B ); cu_assert ( B != NULL );
   Cp = matrix_add ( Cp, 1.0, B, -1.0, Cp ); cu_assert ( Cp != NULL );
   cu_assert ( gan_mat_Fnorm(Cp) < 1.0e-6 );
   
   /* test general matrix addition with second argument transposed */
   A = matrix_rand ( A, 3, 4 ); cu_assert ( A != NULL );
   B = matrix_rand ( B, 4, 3 ); cu_assert ( B != NULL );
   BT = matrix_transpose ( B, BT ); cu_assert ( BT != NULL );
   Cp = matrix_add ( A, 1.0, BT, 1.0, Cp ); cu_assert ( Cp != NULL );
   C = gan_mat_addT_q ( A, B, C ); cu_assert ( C != NULL );
   Cp = matrix_add ( Cp, 1.0, C, -1.0, Cp ); cu_assert ( Cp != NULL );
   cu_assert ( gan_mat_Fnorm(Cp) < 1.0e-6 );

   /* test in-place general matrix addition with second argument transposed */
   A = matrix_rand ( A, 3, 4 ); cu_assert ( A != NULL );
   B = matrix_rand ( B, 4, 3 ); cu_assert ( B != NULL );
   BT = matrix_transpose ( B, BT ); cu_assert ( BT != NULL );
   Cp = matrix_add ( A, 1.0, BT, 1.0, Cp ); cu_assert ( Cp != NULL );
   A = gan_mat_incrementT ( A, B ); cu_assert ( A != NULL );
   Cp = matrix_add ( Cp, 1.0, A, -1.0, Cp ); cu_assert ( Cp != NULL );
   cu_assert ( gan_mat_Fnorm(Cp) < 1.0e-6 );

   /* test matrix addition with matrix producing symmetric matrix */
   A = matrix_rand ( A, 4, 4 ); cu_assert ( A != NULL );
   AT = matrix_transpose ( A, AT ); cu_assert ( AT != NULL );
   Cp = matrix_add ( A, 1.0, AT, 1.0, Cp ); cu_assert ( Cp != NULL );
   S = gan_mat_add_sym_q ( A, AT, S ); cu_assert ( S != NULL );
   C = gan_mat_from_squmat_q ( S, C ); cu_assert ( C != NULL );
   Cp = matrix_add ( Cp, 1.0, C, -1.0, Cp ); cu_assert ( Cp != NULL );
   cu_assert ( gan_mat_Fnorm(Cp) < 1.0e-6 );
   
   /* test matrix addition with first matrix transposed producing symmetric
      matrix */
   A = matrix_rand ( A, 4, 4 ); cu_assert ( A != NULL );
   AT = matrix_transpose ( A, AT ); cu_assert ( AT != NULL );
   Cp = matrix_add ( AT, 1.0, A, 1.0, Cp ); cu_assert ( Cp != NULL );
   S = gan_matT_add_sym_q ( A, A, S ); cu_assert ( S != NULL );
   C = gan_mat_from_squmat_q ( S, C ); cu_assert ( C != NULL );
   Cp = matrix_add ( Cp, 1.0, C, -1.0, Cp ); cu_assert ( Cp != NULL );
   cu_assert ( gan_mat_Fnorm(Cp) < 1.0e-6 );
   
   /* test matrix addition with second matrix transposed producing symmetric
      matrix */
   A = matrix_rand ( A, 4, 4 ); cu_assert ( A != NULL );
   AT = matrix_transpose ( A, AT ); cu_assert ( AT != NULL );
   Cp = matrix_add ( A, 1.0, AT, 1.0, Cp ); cu_assert ( Cp != NULL );
   S = gan_mat_addT_sym_q ( A, A, S ); cu_assert ( S != NULL );
   C = gan_mat_from_squmat_q ( S, C ); cu_assert ( C != NULL );
   Cp = matrix_add ( Cp, 1.0, C, -1.0, Cp ); cu_assert ( Cp != NULL );
   cu_assert ( gan_mat_Fnorm(Cp) < 1.0e-6 );
   
   /* test matrix addition with both matrices transposed producing symmetric
      matrix */
   A = matrix_rand ( A, 4, 4 ); cu_assert ( A != NULL );
   AT = matrix_transpose ( A, AT ); cu_assert ( AT != NULL );
   Cp = matrix_add ( AT, 1.0, A, 1.0, Cp ); cu_assert ( Cp != NULL );
   S = gan_matT_addT_sym_q ( A, AT, S ); cu_assert ( S != NULL );
   C = gan_mat_from_squmat_q ( S, C ); cu_assert ( C != NULL );
   Cp = matrix_add ( Cp, 1.0, C, -1.0, Cp ); cu_assert ( Cp != NULL );
   cu_assert ( gan_mat_Fnorm(Cp) < 1.0e-6 );
   
   /* test matrix addition with self-transpose producing symmetric matrix */
   A = matrix_rand ( A, 4, 4 ); cu_assert ( A != NULL );
   AT = matrix_transpose ( A, AT ); cu_assert ( AT != NULL );
   Cp = matrix_add ( A, 1.0, AT, 1.0, Cp ); cu_assert ( Cp != NULL );
   S = gan_mat_saddT_sym_q ( A, S ); cu_assert ( S != NULL );
   C = gan_mat_from_squmat_q ( S, C ); cu_assert ( C != NULL );
   Cp = matrix_add ( Cp, 1.0, C, -1.0, Cp ); cu_assert ( Cp != NULL );
   cu_assert ( gan_mat_Fnorm(Cp) < 1.0e-6 );
   
   /* test self-transpose matrix addition producing symmetric matrix */
   A = matrix_rand ( A, 4, 4 ); cu_assert ( A != NULL );
   AT = matrix_transpose ( A, AT ); cu_assert ( AT != NULL );
   Cp = matrix_add ( AT, 1.0, A, 1.0, Cp ); cu_assert ( Cp != NULL );
   S = gan_matT_sadd_sym_q ( A, S ); cu_assert ( S != NULL );
   C = gan_mat_from_squmat_q ( S, C ); cu_assert ( C != NULL );
   Cp = matrix_add ( Cp, 1.0, C, -1.0, Cp ); cu_assert ( Cp != NULL );
   cu_assert ( gan_mat_Fnorm(Cp) < 1.0e-6 );
   
   /* test general matrix subtraction */
   A = matrix_rand ( A, 3, 4 ); cu_assert ( A != NULL );
   B = matrix_rand ( B, 3, 4 ); cu_assert ( B != NULL );
   Cp = matrix_add ( A, 1.0, B, -1.0, Cp ); cu_assert ( Cp != NULL );
   C = gan_mat_sub_q ( A, B, C ); cu_assert ( C != NULL );
   Cp = matrix_add ( Cp, 1.0, C, -1.0, Cp ); cu_assert ( Cp != NULL );
   cu_assert ( gan_mat_Fnorm(Cp) < 1.0e-6 );

   /* test in-place general matrix subtraction in first argument */
   A = matrix_rand ( A, 3, 4 ); cu_assert ( A != NULL );
   B = matrix_rand ( B, 3, 4 ); cu_assert ( B != NULL );
   Cp = matrix_add ( A, 1.0, B, -1.0, Cp ); cu_assert ( Cp != NULL );
   A = gan_mat_sub_i1 ( A, B ); cu_assert ( A != NULL );
   Cp = matrix_add ( Cp, 1.0, A, -1.0, Cp ); cu_assert ( Cp != NULL );
   cu_assert ( gan_mat_Fnorm(Cp) < 1.0e-6 );
   
   /* test in-place general matrix subtraction in second argument */
   A = matrix_rand ( A, 3, 4 ); cu_assert ( A != NULL );
   B = matrix_rand ( B, 3, 4 ); cu_assert ( B != NULL );
   Cp = matrix_add ( A, 1.0, B, -1.0, Cp ); cu_assert ( Cp != NULL );
   B = gan_mat_sub_i2 ( A, B ); cu_assert ( B != NULL );
   Cp = matrix_add ( Cp, 1.0, B, -1.0, Cp ); cu_assert ( Cp != NULL );
   cu_assert ( gan_mat_Fnorm(Cp) < 1.0e-6 );
   
   /* test general matrix subtraction with second argument transposed */
   A = matrix_rand ( A, 3, 4 ); cu_assert ( A != NULL );
   B = matrix_rand ( B, 4, 3 ); cu_assert ( B != NULL );
   BT = matrix_transpose ( B, BT ); cu_assert ( BT != NULL );
   Cp = matrix_add ( A, 1.0, BT, -1.0, Cp ); cu_assert ( Cp != NULL );
   C = gan_mat_subT_q ( A, B, C ); cu_assert ( C != NULL );
   Cp = matrix_add ( Cp, 1.0, C, -1.0, Cp ); cu_assert ( Cp != NULL );
   cu_assert ( gan_mat_Fnorm(Cp) < 1.0e-6 );

   /* test in-place general matrix subtraction with second argument
      transposed */
   A = matrix_rand ( A, 3, 4 ); cu_assert ( A != NULL );
   B = matrix_rand ( B, 4, 3 ); cu_assert ( B != NULL );
   BT = matrix_transpose ( B, BT ); cu_assert ( BT != NULL );
   Cp = matrix_add ( A, 1.0, BT, -1.0, Cp ); cu_assert ( Cp != NULL );
   A = gan_mat_decrementT ( A, B ); cu_assert ( A != NULL );
   Cp = matrix_add ( Cp, 1.0, A, -1.0, Cp ); cu_assert ( Cp != NULL );
   cu_assert ( gan_mat_Fnorm(Cp) < 1.0e-6 );

   /* test matrix/vector multiplication */
   A = matrix_rand ( A, 3, 5 ); cu_assert ( A != NULL );
   a = vector_rand ( a, 5 ); cu_assert ( a != NULL );
   bp = matrix_vector_mult ( A, a, bp ); cu_assert ( bp != NULL );
   b = gan_mat_multv_q ( A, a, b ); cu_assert ( b != NULL );
   bp = vector_add ( bp, 1.0, b, -1.0, bp ); cu_assert ( bp != NULL );
   cu_assert ( gan_vec_sqrlen(bp) < 1.0e-6 );

   /* test matrix/vector multiplication with matrix transposed */
   A = matrix_rand ( A, 5, 3 ); cu_assert ( A != NULL );
   a = vector_rand ( a, 5 ); cu_assert ( a != NULL );
   AT = matrix_transpose ( A, AT ); cu_assert ( AT != NULL );
   bp = matrix_vector_mult ( AT, a, bp ); cu_assert ( bp != NULL );
   b = gan_matT_multv_q ( A, a, b ); cu_assert ( b != NULL );
   bp = vector_add ( bp, 1.0, b, -1.0, bp ); cu_assert ( bp != NULL );
   cu_assert ( gan_vec_sqrlen(bp) < 1.0e-6 );

   /* test matrix/matrix multiplication */
   A = matrix_rand ( A, 5, 3 ); cu_assert ( A != NULL );
   B = matrix_rand ( B, 3, 4 ); cu_assert ( B != NULL );
   Cp = matrix_matrix_mult ( A, B, Cp ); cu_assert ( Cp != NULL );
   C = gan_mat_rmult_q ( A, B, C ); cu_assert ( C != NULL );
   Cp = matrix_add ( Cp, 1.0, C, -1.0, Cp ); cu_assert ( Cp != NULL );
   cu_assert ( gan_mat_Fnorm(Cp) < 1.0e-6 );

   /* test matrix/matrix multiplication with first matrix transposed */
   A = matrix_rand ( A, 3, 5 ); cu_assert ( A != NULL );
   B = matrix_rand ( B, 3, 4 ); cu_assert ( B != NULL );
   AT = matrix_transpose ( A, AT ); cu_assert ( AT != NULL );
   Cp = matrix_matrix_mult ( AT, B, Cp ); cu_assert ( Cp != NULL );
   C = gan_matT_rmult_q ( A, B, C ); cu_assert ( C != NULL );
   Cp = matrix_add ( Cp, 1.0, C, -1.0, Cp ); cu_assert ( Cp != NULL );
   cu_assert ( gan_mat_Fnorm(Cp) < 1.0e-6 );

   /* test matrix/matrix multiplication with second matrix transposed */
   A = matrix_rand ( A, 5, 3 ); cu_assert ( A != NULL );
   B = matrix_rand ( B, 4, 3 ); cu_assert ( B != NULL );
   BT = matrix_transpose ( B, BT ); cu_assert ( BT != NULL );
   Cp = matrix_matrix_mult ( A, BT, Cp ); cu_assert ( Cp != NULL );
   C = gan_mat_rmultT_q ( A, B, C ); cu_assert ( C != NULL );
   Cp = matrix_add ( Cp, 1.0, C, -1.0, Cp ); cu_assert ( Cp != NULL );
   cu_assert ( gan_mat_Fnorm(Cp) < 1.0e-6 );

   /* test matrix/matrix multiplication with both matrices transposed */
   A = matrix_rand ( A, 3, 5 ); cu_assert ( A != NULL );
   B = matrix_rand ( B, 4, 3 ); cu_assert ( B != NULL );
   AT = matrix_transpose ( A, AT ); cu_assert ( AT != NULL );
   BT = matrix_transpose ( B, BT ); cu_assert ( BT != NULL );
   Cp = matrix_matrix_mult ( AT, BT, Cp ); cu_assert (Cp != NULL );
   C = gan_matT_rmultT_q ( A, B, C ); cu_assert ( C != NULL );
   Cp = matrix_add ( Cp, 1.0, C, -1.0, Cp ); cu_assert ( Cp != NULL );
   cu_assert ( gan_mat_Fnorm(Cp) < 1.0e-6 );

   /* test matrix/matrix multiplication producing a symmetric matrix */
   S = symmatrix_rand ( S, 3, 3 ); cu_assert ( S != NULL );
   A = matrix_rand ( A, 5, 3 ); cu_assert ( A != NULL );
   AT = matrix_transpose ( A, AT ); cu_assert ( AT != NULL );
   B = matrix_squmatrix_mult ( A, S, B ); cu_assert ( B != NULL );
   C = matrix_matrix_mult ( B, AT, C ); cu_assert ( C != NULL );
   T = gan_mat_rmult_sym_q ( B, AT, T ); cu_assert ( T != NULL );
   Cp = gan_mat_from_squmat_q ( T, Cp ); cu_assert ( Cp != NULL );
   Cp = matrix_add ( Cp, 1.0, C, -1.0, Cp ); cu_assert ( Cp != NULL );
   cu_assert ( gan_mat_Fnorm(Cp) < 1.0e-6 );

   /* test matrix/matrix multiplication, the second transposed, producing a
      symmetric matrix */
   S = symmatrix_rand ( S, 3, 3 ); cu_assert ( S != NULL );
   A = matrix_rand ( A, 5, 3 ); cu_assert ( A != NULL );
   AT = matrix_transpose ( A, AT ); cu_assert ( AT != NULL );
   B = matrix_squmatrix_mult ( A, S, B ); cu_assert ( B != NULL );
   C = matrix_matrix_mult ( B, AT, C ); cu_assert ( C != NULL );
   T = gan_mat_rmultT_sym_q ( B, A, T ); cu_assert ( T != NULL );
   Cp = gan_mat_from_squmat_q ( T, Cp ); cu_assert ( Cp != NULL );
   Cp = matrix_add ( Cp, 1.0, C, -1.0, Cp ); cu_assert ( Cp != NULL );
   cu_assert ( gan_mat_Fnorm(Cp) < 1.0e-6 );

   /* test matrix/matrix multiplication, the first transposed, producing a
      symmetric matrix */
   S = symmatrix_rand ( S, 5, 5 ); cu_assert ( S != NULL );
   A = matrix_rand ( A, 5, 3 ); cu_assert ( A != NULL );
   AT = matrix_transpose ( A, AT ); cu_assert ( AT != NULL );
   B = squmatrix_matrix_mult ( S, A, B ); cu_assert ( B != NULL );
   C = matrix_matrix_mult ( AT, B, C ); cu_assert ( C != NULL );
   T = gan_matT_rmult_sym_q ( A, B, T ); cu_assert ( T != NULL );
   Cp = gan_mat_from_squmat_q ( T, Cp ); cu_assert ( Cp != NULL );
   Cp = matrix_add ( Cp, 1.0, C, -1.0, Cp ); cu_assert ( Cp != NULL );
   cu_assert ( gan_mat_Fnorm(Cp) < 1.0e-6 );

   /* test matrix/matrix multiplication, the both transposed, producing a
      symmetric matrix */
   S = symmatrix_rand ( S, 5, 5 ); cu_assert ( S != NULL );
   A = matrix_rand ( A, 5, 3 ); cu_assert ( A != NULL );
   AT = matrix_transpose ( A, AT ); cu_assert ( AT != NULL );
   B = squmatrix_matrix_mult ( S, A, B ); cu_assert ( B != NULL );
   BT = matrix_transpose ( B, BT ); cu_assert ( BT != NULL );
   C = matrix_matrix_mult ( BT, A, C ); cu_assert ( C != NULL );
   T = gan_matT_rmultT_sym_q ( B, AT, T ); cu_assert ( T != NULL );
   Cp = gan_mat_from_squmat_q ( T, Cp ); cu_assert ( Cp != NULL );
   Cp = matrix_add ( Cp, 1.0, C, -1.0, Cp ); cu_assert ( Cp != NULL );
   cu_assert ( gan_mat_Fnorm(Cp) < 1.0e-6 );

   /* test right self-multiplication of a matrix, producing a symmetric
      matrix */
   A = matrix_rand ( A, 5, 3 ); cu_assert ( A != NULL );
   AT = matrix_transpose ( A, AT ); cu_assert ( AT != NULL );
   C = matrix_matrix_mult ( A, AT, C ); cu_assert ( C != NULL );
   S = gan_mat_srmultT_q ( A, S ); cu_assert ( S != NULL );
   Cp = gan_mat_from_squmat_q ( S, Cp ); cu_assert ( Cp != NULL );
   Cp = matrix_add ( Cp, 1.0, C, -1.0, Cp ); cu_assert ( Cp != NULL );
   cu_assert ( gan_mat_Fnorm(Cp) < 1.0e-6 );
   
   /* test left self-multiplication of a matrix, producing a symmetric
      matrix */
   A = matrix_rand ( A, 5, 3 ); cu_assert ( A != NULL );
   AT = matrix_transpose ( A, AT ); cu_assert ( AT != NULL );
   C = matrix_matrix_mult ( AT, A, C ); cu_assert ( C != NULL );
   S = gan_mat_slmultT_q ( A, S ); cu_assert ( S != NULL );
   Cp = gan_mat_from_squmat_q ( S, Cp ); cu_assert ( Cp != NULL );
   Cp = matrix_add ( Cp, 1.0, C, -1.0, Cp ); cu_assert ( Cp != NULL );
   cu_assert ( gan_mat_Fnorm(Cp) < 1.0e-6 );

   /* test matrix inverse */
   A = matrix_rand ( A, 5, 5 ); cu_assert ( A != NULL );
   B = gan_mat_invert_q ( A, B ); cu_assert ( B != NULL );
   C = matrix_matrix_mult ( A, B, C ); cu_assert ( C != NULL );
   Cp = matrix_ident ( Cp, C->rows ); cu_assert ( Cp != NULL );
   Cp = matrix_add ( Cp, 1.0, C, -1.0, Cp ); cu_assert ( Cp != NULL );
   cu_assert ( gan_mat_Fnorm(Cp) < 1.0e-6 );

   /* free matrices and vectors */
   gan_squmat_free_va ( T, S, NULL );
   gan_vec_free_va ( bp, b, a, NULL );
   gan_mat_free_va ( Cp, C, BT, B, AT, A, NULL );

   /* return with success */
   return GAN_TRUE;
}

/* Tests for general size single precision matrices. We apply an operation
 * both using Gandalf functions and simplified functions defined in this test
 * module.
 */
static Gan_Bool test_matf_gen(void)
{
   Gan_Matrix_f *A, *AT, *B, *BT, *C, *Cp;
   Gan_Vector_f *a, *b, *bp;
   Gan_SquMatrix_f *S, *T;

   /* allocate matrices and vectors used in tests */
   A  = gan_matf_alloc ( 0, 0 ); cu_assert ( A  != NULL );
   AT = gan_matf_alloc ( 0, 0 ); cu_assert ( AT != NULL );
   B  = gan_matf_alloc ( 0, 0 ); cu_assert ( B  != NULL );
   BT = gan_matf_alloc ( 0, 0 ); cu_assert ( BT != NULL );
   C  = gan_matf_alloc ( 0, 0 ); cu_assert ( C  != NULL );
   Cp = gan_matf_alloc ( 0, 0 ); cu_assert ( Cp != NULL );
   a  = gan_vecf_alloc(0); cu_assert ( a  != NULL );
   b  = gan_vecf_alloc(0); cu_assert ( b  != NULL );
   bp = gan_vecf_alloc(0); cu_assert ( bp != NULL );
   S = gan_symmatf_alloc(0); cu_assert ( S != NULL );
   T = gan_symmatf_alloc(0); cu_assert ( T != NULL );

   /* test general matrix copy */
   A = matrixf_rand ( A, 3, 4 ); cu_assert ( A != NULL );
   Cp = matrixf_add ( A, 1.0F, NULL, 0.0F, Cp ); cu_assert ( Cp != NULL );
   C = gan_matf_copy_q ( A, C ); cu_assert ( C != NULL );
   Cp = matrixf_add ( Cp, 1.0F, C, -1.0F, Cp ); cu_assert ( Cp != NULL );
   cu_assert ( gan_matf_Fnorm(Cp) < 1.0e-4F );

   /* test general matrix multiply by scalar */
   A = matrixf_rand ( A, 3, 4 ); cu_assert ( A != NULL );
   Cp = matrixf_add ( A, 4.5F, NULL, 0.0F, Cp ); cu_assert ( Cp != NULL );
   C = gan_matf_scale_q ( A, 4.5F, C ); cu_assert ( C != NULL );
   Cp = matrixf_add ( Cp, 1.0F, C, -1.0F, Cp ); cu_assert ( Cp != NULL );
   cu_assert ( gan_matf_Fnorm(Cp) < 1.0e-4F );

   /* test general matrix multiply by scalar in-place */
   A = matrixf_rand ( A, 3, 4 ); cu_assert ( A != NULL );
   Cp = matrixf_add ( A, 4.5F, NULL, 0.0F, Cp ); cu_assert ( Cp != NULL );
   A = gan_matf_scale_i ( A, 4.5F ); cu_assert ( a != NULL );
   Cp = matrixf_add ( Cp, 1.0F, A, -1.0F, Cp ); cu_assert ( Cp != NULL );
   cu_assert ( gan_matf_Fnorm(Cp) < 1.0e-4F );

   /* test general matrix divide by scalar */
   A = matrixf_rand ( A, 3, 4 ); cu_assert ( A != NULL );
   Cp = matrixf_add ( A, 1.0F/4.5F, NULL, 0.0F, Cp ); cu_assert (Cp != NULL);
   C = gan_matf_divide_q ( A, 4.5F, C ); cu_assert ( C != NULL );
   Cp = matrixf_add ( Cp, 1.0F, C, -1.0F, Cp ); cu_assert ( Cp != NULL );
   cu_assert ( gan_matf_Fnorm(Cp) < 1.0e-4F );

   /* test general matrix divide by scalar in-place */
   A = matrixf_rand ( A, 3, 4 ); cu_assert ( A != NULL );
   Cp = matrixf_add ( A, 1.0F/4.5F, NULL, 0.0F, Cp ); cu_assert (Cp != NULL);
   A = gan_matf_divide_i ( A, 4.5F ); cu_assert ( A != NULL );
   Cp = matrixf_add ( Cp, 1.0F, A, -1.0F, Cp ); cu_assert ( Cp != NULL );
   cu_assert ( gan_matf_Fnorm(Cp) < 1.0e-4F );

   /* test general matrix transpose */
   A = matrixf_rand ( A, 3, 4 ); cu_assert ( A != NULL );
   Cp = matrixf_transpose ( A, Cp ); cu_assert ( Cp != NULL );
   C = gan_matf_tpose_q ( A, C ); cu_assert ( C != NULL );
   Cp = matrixf_add ( Cp, 1.0F, C, -1.0F, Cp ); cu_assert ( Cp != NULL );
   cu_assert ( gan_matf_Fnorm(Cp) < 1.0e-4F );

   /* test general matrix negation */
   A = matrixf_rand ( A, 3, 4 ); cu_assert ( A != NULL );
   Cp = matrixf_add ( A, -1.0F, NULL, 0.0F, Cp ); cu_assert (Cp != NULL);
   C = gan_matf_negate_q ( A, C ); cu_assert ( C != NULL );
   Cp = matrixf_add ( Cp, 1.0F, C, -1.0F, Cp ); cu_assert ( Cp != NULL );
   cu_assert ( gan_matf_Fnorm(Cp) < 1.0e-4F );

   /* test general matrix negation in-place */
   A = matrixf_rand ( A, 3, 4 ); cu_assert ( A != NULL );
   Cp = matrixf_add ( A, -1.0F, NULL, 0.0F, Cp ); cu_assert (Cp != NULL);
   A = gan_matf_negate_i(A); cu_assert ( A != NULL );
   Cp = matrixf_add ( Cp, 1.0F, A, -1.0F, Cp ); cu_assert ( Cp != NULL );
   cu_assert ( gan_matf_Fnorm(Cp) < 1.0e-4F );

   /* test general matrix addition */
   A = matrixf_rand ( A, 3, 4 ); cu_assert ( A != NULL );
   B = matrixf_rand ( B, 3, 4 ); cu_assert ( B != NULL );
   Cp = matrixf_add ( A, 1.0F, B, 1.0F, Cp ); cu_assert ( Cp != NULL );
   C = gan_matf_add_q ( A, B, C ); cu_assert ( C != NULL );
   Cp = matrixf_add ( Cp, 1.0F, C, -1.0F, Cp ); cu_assert ( Cp != NULL );
   cu_assert ( gan_matf_Fnorm(Cp) < 1.0e-4F );
   
   /* test in-place general matrix addition in first argument */
   A = matrixf_rand ( A, 3, 4 ); cu_assert ( A != NULL );
   B = matrixf_rand ( B, 3, 4 ); cu_assert ( B != NULL );
   Cp = matrixf_add ( A, 1.0F, B, 1.0F, Cp ); cu_assert ( Cp != NULL );
   A = gan_matf_add_i1 ( A, B ); cu_assert ( A != NULL );
   Cp = matrixf_add ( Cp, 1.0F, A, -1.0F, Cp ); cu_assert ( Cp != NULL );
   cu_assert ( gan_matf_Fnorm(Cp) < 1.0e-4F );
   
   /* test in-place general matrix addition in second argument */
   A = matrixf_rand ( A, 3, 4 ); cu_assert ( A != NULL );
   B = matrixf_rand ( B, 3, 4 ); cu_assert ( B != NULL );
   Cp = matrixf_add ( A, 1.0F, B, 1.0F, Cp ); cu_assert ( Cp != NULL );
   B = gan_matf_add_i2 ( A, B ); cu_assert ( B != NULL );
   Cp = matrixf_add ( Cp, 1.0F, B, -1.0F, Cp ); cu_assert ( Cp != NULL );
   cu_assert ( gan_matf_Fnorm(Cp) < 1.0e-4F );
   
   /* test general matrix addition with second argument transposed */
   A = matrixf_rand ( A, 3, 4 ); cu_assert ( A != NULL );
   B = matrixf_rand ( B, 4, 3 ); cu_assert ( B != NULL );
   BT = matrixf_transpose ( B, BT ); cu_assert ( BT != NULL );
   Cp = matrixf_add ( A, 1.0F, BT, 1.0F, Cp ); cu_assert ( Cp != NULL );
   C = gan_matf_addT_q ( A, B, C ); cu_assert ( C != NULL );
   Cp = matrixf_add ( Cp, 1.0F, C, -1.0F, Cp ); cu_assert ( Cp != NULL );
   cu_assert ( gan_matf_Fnorm(Cp) < 1.0e-4F );

   /* test in-place general matrix addition with second argument transposed */
   A = matrixf_rand ( A, 3, 4 ); cu_assert ( A != NULL );
   B = matrixf_rand ( B, 4, 3 ); cu_assert ( B != NULL );
   BT = matrixf_transpose ( B, BT ); cu_assert ( BT != NULL );
   Cp = matrixf_add ( A, 1.0F, BT, 1.0F, Cp ); cu_assert ( Cp != NULL );
   A = gan_matf_incrementT ( A, B ); cu_assert ( A != NULL );
   Cp = matrixf_add ( Cp, 1.0F, A, -1.0F, Cp ); cu_assert ( Cp != NULL );
   cu_assert ( gan_matf_Fnorm(Cp) < 1.0e-4F );

   /* test matrix addition with matrix producing symmetric matrix */
   A = matrixf_rand ( A, 4, 4 ); cu_assert ( A != NULL );
   AT = matrixf_transpose ( A, AT ); cu_assert ( AT != NULL );
   Cp = matrixf_add ( A, 1.0F, AT, 1.0F, Cp ); cu_assert ( Cp != NULL );
   S = gan_matf_add_sym_q ( A, AT, S ); cu_assert ( S != NULL );
   C = gan_matf_from_squmatf_q ( S, C ); cu_assert ( C != NULL );
   Cp = matrixf_add ( Cp, 1.0F, C, -1.0F, Cp ); cu_assert ( Cp != NULL );
   cu_assert ( gan_matf_Fnorm(Cp) < 1.0e-4F );
   
   /* test matrix addition with first matrix transposed producing symmetric
      matrix */
   A = matrixf_rand ( A, 4, 4 ); cu_assert ( A != NULL );
   AT = matrixf_transpose ( A, AT ); cu_assert ( AT != NULL );
   Cp = matrixf_add ( AT, 1.0F, A, 1.0F, Cp ); cu_assert ( Cp != NULL );
   S = gan_matTf_add_sym_q ( A, A, S ); cu_assert ( S != NULL );
   C = gan_matf_from_squmatf_q ( S, C ); cu_assert ( C != NULL );
   Cp = matrixf_add ( Cp, 1.0F, C, -1.0F, Cp ); cu_assert ( Cp != NULL );
   cu_assert ( gan_matf_Fnorm(Cp) < 1.0e-4F );
   
   /* test matrix addition with second matrix transposed producing symmetric
      matrix */
   A = matrixf_rand ( A, 4, 4 ); cu_assert ( A != NULL );
   AT = matrixf_transpose ( A, AT ); cu_assert ( AT != NULL );
   Cp = matrixf_add ( A, 1.0F, AT, 1.0F, Cp ); cu_assert ( Cp != NULL );
   S = gan_matf_addT_sym_q ( A, A, S ); cu_assert ( S != NULL );
   C = gan_matf_from_squmatf_q ( S, C ); cu_assert ( C != NULL );
   Cp = matrixf_add ( Cp, 1.0F, C, -1.0F, Cp ); cu_assert ( Cp != NULL );
   cu_assert ( gan_matf_Fnorm(Cp) < 1.0e-4F );
   
   /* test matrix addition with both matrices transposed producing symmetric
      matrix */
   A = matrixf_rand ( A, 4, 4 ); cu_assert ( A != NULL );
   AT = matrixf_transpose ( A, AT ); cu_assert ( AT != NULL );
   Cp = matrixf_add ( AT, 1.0F, A, 1.0F, Cp ); cu_assert ( Cp != NULL );
   S = gan_matTf_addT_sym_q ( A, AT, S ); cu_assert ( S != NULL );
   C = gan_matf_from_squmatf_q ( S, C ); cu_assert ( C != NULL );
   Cp = matrixf_add ( Cp, 1.0F, C, -1.0F, Cp ); cu_assert ( Cp != NULL );
   cu_assert ( gan_matf_Fnorm(Cp) < 1.0e-4F );
   
   /* test matrix addition with self-transpose producing symmetric matrix */
   A = matrixf_rand ( A, 4, 4 ); cu_assert ( A != NULL );
   AT = matrixf_transpose ( A, AT ); cu_assert ( AT != NULL );
   Cp = matrixf_add ( A, 1.0F, AT, 1.0F, Cp ); cu_assert ( Cp != NULL );
   S = gan_matf_saddT_sym_q ( A, S ); cu_assert ( S != NULL );
   C = gan_matf_from_squmatf_q ( S, C ); cu_assert ( C != NULL );
   Cp = matrixf_add ( Cp, 1.0F, C, -1.0F, Cp ); cu_assert ( Cp != NULL );
   cu_assert ( gan_matf_Fnorm(Cp) < 1.0e-4F );
   
   /* test self-transpose matrix addition producing symmetric matrix */
   A = matrixf_rand ( A, 4, 4 ); cu_assert ( A != NULL );
   AT = matrixf_transpose ( A, AT ); cu_assert ( AT != NULL );
   Cp = matrixf_add ( AT, 1.0F, A, 1.0F, Cp ); cu_assert ( Cp != NULL );
   S = gan_matTf_sadd_sym_q ( A, S ); cu_assert ( S != NULL );
   C = gan_matf_from_squmatf_q ( S, C ); cu_assert ( C != NULL );
   Cp = matrixf_add ( Cp, 1.0F, C, -1.0F, Cp ); cu_assert ( Cp != NULL );
   cu_assert ( gan_matf_Fnorm(Cp) < 1.0e-4F );
   
   /* test general matrix subtraction */
   A = matrixf_rand ( A, 3, 4 ); cu_assert ( A != NULL );
   B = matrixf_rand ( B, 3, 4 ); cu_assert ( B != NULL );
   Cp = matrixf_add ( A, 1.0F, B, -1.0F, Cp ); cu_assert ( Cp != NULL );
   C = gan_matf_sub_q ( A, B, C ); cu_assert ( C != NULL );
   Cp = matrixf_add ( Cp, 1.0F, C, -1.0F, Cp ); cu_assert ( Cp != NULL );
   cu_assert ( gan_matf_Fnorm(Cp) < 1.0e-4F );

   /* test in-place general matrix subtraction in first argument */
   A = matrixf_rand ( A, 3, 4 ); cu_assert ( A != NULL );
   B = matrixf_rand ( B, 3, 4 ); cu_assert ( B != NULL );
   Cp = matrixf_add ( A, 1.0F, B, -1.0F, Cp ); cu_assert ( Cp != NULL );
   A = gan_matf_sub_i1 ( A, B ); cu_assert ( A != NULL );
   Cp = matrixf_add ( Cp, 1.0F, A, -1.0F, Cp ); cu_assert ( Cp != NULL );
   cu_assert ( gan_matf_Fnorm(Cp) < 1.0e-4F );
   
   /* test in-place general matrix subtraction in second argument */
   A = matrixf_rand ( A, 3, 4 ); cu_assert ( A != NULL );
   B = matrixf_rand ( B, 3, 4 ); cu_assert ( B != NULL );
   Cp = matrixf_add ( A, 1.0F, B, -1.0F, Cp ); cu_assert ( Cp != NULL );
   B = gan_matf_sub_i2 ( A, B ); cu_assert ( B != NULL );
   Cp = matrixf_add ( Cp, 1.0F, B, -1.0F, Cp ); cu_assert ( Cp != NULL );
   cu_assert ( gan_matf_Fnorm(Cp) < 1.0e-4F );
   
   /* test general matrix subtraction with second argument transposed */
   A = matrixf_rand ( A, 3, 4 ); cu_assert ( A != NULL );
   B = matrixf_rand ( B, 4, 3 ); cu_assert ( B != NULL );
   BT = matrixf_transpose ( B, BT ); cu_assert ( BT != NULL );
   Cp = matrixf_add ( A, 1.0F, BT, -1.0F, Cp ); cu_assert ( Cp != NULL );
   C = gan_matf_subT_q ( A, B, C ); cu_assert ( C != NULL );
   Cp = matrixf_add ( Cp, 1.0F, C, -1.0F, Cp ); cu_assert ( Cp != NULL );
   cu_assert ( gan_matf_Fnorm(Cp) < 1.0e-4F );

   /* test in-place general matrix subtraction with second argument
      transposed */
   A = matrixf_rand ( A, 3, 4 ); cu_assert ( A != NULL );
   B = matrixf_rand ( B, 4, 3 ); cu_assert ( B != NULL );
   BT = matrixf_transpose ( B, BT ); cu_assert ( BT != NULL );
   Cp = matrixf_add ( A, 1.0F, BT, -1.0F, Cp ); cu_assert ( Cp != NULL );
   A = gan_matf_decrementT ( A, B ); cu_assert ( A != NULL );
   Cp = matrixf_add ( Cp, 1.0F, A, -1.0F, Cp ); cu_assert ( Cp != NULL );
   cu_assert ( gan_matf_Fnorm(Cp) < 1.0e-4F );

   /* test matrix/vector multiplication */
   A = matrixf_rand ( A, 3, 5 ); cu_assert ( A != NULL );
   a = vectorf_rand ( a, 5 ); cu_assert ( a != NULL );
   bp = matrixf_vectorf_mult ( A, a, bp ); cu_assert ( bp != NULL );
   b = gan_matf_multv_q ( A, a, b ); cu_assert ( b != NULL );
   bp = vectorf_add ( bp, 1.0F, b, -1.0F, bp ); cu_assert ( bp != NULL );
   cu_assert ( gan_vecf_sqrlen(bp) < 1.0e-4F );

   /* test matrix/vector multiplication with matrix transposed */
   A = matrixf_rand ( A, 5, 3 ); cu_assert ( A != NULL );
   a = vectorf_rand ( a, 5 ); cu_assert ( a != NULL );
   AT = matrixf_transpose ( A, AT ); cu_assert ( AT != NULL );
   bp = matrixf_vectorf_mult ( AT, a, bp ); cu_assert ( bp != NULL );
   b = gan_matTf_multv_q ( A, a, b ); cu_assert ( b != NULL );
   bp = vectorf_add ( bp, 1.0F, b, -1.0F, bp ); cu_assert ( bp != NULL );
   cu_assert ( gan_vecf_sqrlen(bp) < 1.0e-4F );

   /* test matrix/matrix multiplication */
   A = matrixf_rand ( A, 5, 3 ); cu_assert ( A != NULL );
   B = matrixf_rand ( B, 3, 4 ); cu_assert ( B != NULL );
   Cp = matrixf_matrixf_mult ( A, B, Cp ); cu_assert ( Cp != NULL );
   C = gan_matf_rmult_q ( A, B, C ); cu_assert ( C != NULL );
   Cp = matrixf_add ( Cp, 1.0F, C, -1.0F, Cp ); cu_assert ( Cp != NULL );
   cu_assert ( gan_matf_Fnorm(Cp) < 1.0e-4F );

   /* test matrix/matrix multiplication with first matrix transposed */
   A = matrixf_rand ( A, 3, 5 ); cu_assert ( A != NULL );
   B = matrixf_rand ( B, 3, 4 ); cu_assert ( B != NULL );
   AT = matrixf_transpose ( A, AT ); cu_assert ( AT != NULL );
   Cp = matrixf_matrixf_mult ( AT, B, Cp ); cu_assert ( Cp != NULL );
   C = gan_matTf_rmult_q ( A, B, C ); cu_assert ( C != NULL );
   Cp = matrixf_add ( Cp, 1.0F, C, -1.0F, Cp ); cu_assert ( Cp != NULL );
   cu_assert ( gan_matf_Fnorm(Cp) < 1.0e-4F );

   /* test matrix/matrix multiplication with second matrix transposed */
   A = matrixf_rand ( A, 5, 3 ); cu_assert ( A != NULL );
   B = matrixf_rand ( B, 4, 3 ); cu_assert ( B != NULL );
   BT = matrixf_transpose ( B, BT ); cu_assert ( BT != NULL );
   Cp = matrixf_matrixf_mult ( A, BT, Cp ); cu_assert ( Cp != NULL );
   C = gan_matf_rmultT_q ( A, B, C ); cu_assert ( C != NULL );
   Cp = matrixf_add ( Cp, 1.0F, C, -1.0F, Cp ); cu_assert ( Cp != NULL );
   cu_assert ( gan_matf_Fnorm(Cp) < 1.0e-4F );

   /* test matrix/matrix multiplication with both matrices transposed */
   A = matrixf_rand ( A, 3, 5 ); cu_assert ( A != NULL );
   B = matrixf_rand ( B, 4, 3 ); cu_assert ( B != NULL );
   AT = matrixf_transpose ( A, AT ); cu_assert ( AT != NULL );
   BT = matrixf_transpose ( B, BT ); cu_assert ( BT != NULL );
   Cp = matrixf_matrixf_mult ( AT, BT, Cp ); cu_assert (Cp != NULL );
   C = gan_matTf_rmultT_q ( A, B, C ); cu_assert ( C != NULL );
   Cp = matrixf_add ( Cp, 1.0F, C, -1.0F, Cp ); cu_assert ( Cp != NULL );
   cu_assert ( gan_matf_Fnorm(Cp) < 1.0e-4F );

   /* test matrix/matrix multiplication producing a symmetric matrix */
   S = symmatrixf_rand ( S, 3, 3 ); cu_assert ( S != NULL );
   A = matrixf_rand ( A, 5, 3 ); cu_assert ( A != NULL );
   AT = matrixf_transpose ( A, AT ); cu_assert ( AT != NULL );
   B = matrixf_squmatrixf_mult ( A, S, B ); cu_assert ( B != NULL );
   C = matrixf_matrixf_mult ( B, AT, C ); cu_assert ( C != NULL );
   T = gan_matf_rmult_sym_q ( B, AT, T ); cu_assert ( T != NULL );
   Cp = gan_matf_from_squmatf_q ( T, Cp ); cu_assert ( Cp != NULL );
   Cp = matrixf_add ( Cp, 1.0F, C, -1.0F, Cp ); cu_assert ( Cp != NULL );
   cu_assert ( gan_matf_Fnorm(Cp) < 1.0e-4F );

   /* test matrix/matrix multiplication, the second transposed, producing a
      symmetric matrix */
   S = symmatrixf_rand ( S, 3, 3 ); cu_assert ( S != NULL );
   A = matrixf_rand ( A, 5, 3 ); cu_assert ( A != NULL );
   AT = matrixf_transpose ( A, AT ); cu_assert ( AT != NULL );
   B = matrixf_squmatrixf_mult ( A, S, B ); cu_assert ( B != NULL );
   C = matrixf_matrixf_mult ( B, AT, C ); cu_assert ( C != NULL );
   T = gan_matf_rmultT_sym_q ( B, A, T ); cu_assert ( T != NULL );
   Cp = gan_matf_from_squmatf_q ( T, Cp ); cu_assert ( Cp != NULL );
   Cp = matrixf_add ( Cp, 1.0F, C, -1.0F, Cp ); cu_assert ( Cp != NULL );
   cu_assert ( gan_matf_Fnorm(Cp) < 1.0e-4F );

   /* test matrix/matrix multiplication, the first transposed, producing a
      symmetric matrix */
   S = symmatrixf_rand ( S, 5, 5 ); cu_assert ( S != NULL );
   A = matrixf_rand ( A, 5, 3 ); cu_assert ( A != NULL );
   AT = matrixf_transpose ( A, AT ); cu_assert ( AT != NULL );
   B = squmatrixf_matrixf_mult ( S, A, B ); cu_assert ( B != NULL );
   C = matrixf_matrixf_mult ( AT, B, C ); cu_assert ( C != NULL );
   T = gan_matTf_rmult_sym_q ( A, B, T ); cu_assert ( T != NULL );
   Cp = gan_matf_from_squmatf_q ( T, Cp ); cu_assert ( Cp != NULL );
   Cp = matrixf_add ( Cp, 1.0F, C, -1.0F, Cp ); cu_assert ( Cp != NULL );
   cu_assert ( gan_matf_Fnorm(Cp) < 1.0e-4F );

   /* test matrix/matrix multiplication, the both transposed, producing a
      symmetric matrix */
   S = symmatrixf_rand ( S, 5, 5 ); cu_assert ( S != NULL );
   A = matrixf_rand ( A, 5, 3 ); cu_assert ( A != NULL );
   AT = matrixf_transpose ( A, AT ); cu_assert ( AT != NULL );
   B = squmatrixf_matrixf_mult ( S, A, B ); cu_assert ( B != NULL );
   BT = matrixf_transpose ( B, BT ); cu_assert ( BT != NULL );
   C = matrixf_matrixf_mult ( BT, A, C ); cu_assert ( C != NULL );
   T = gan_matTf_rmultT_sym_q ( B, AT, T ); cu_assert ( T != NULL );
   Cp = gan_matf_from_squmatf_q ( T, Cp ); cu_assert ( Cp != NULL );
   Cp = matrixf_add ( Cp, 1.0F, C, -1.0F, Cp ); cu_assert ( Cp != NULL );
   cu_assert ( gan_matf_Fnorm(Cp) < 1.0e-4F );

   /* test right self-multiplication of a matrix, producing a symmetric
      matrix */
   A = matrixf_rand ( A, 5, 3 ); cu_assert ( A != NULL );
   AT = matrixf_transpose ( A, AT ); cu_assert ( AT != NULL );
   C = matrixf_matrixf_mult ( A, AT, C ); cu_assert ( C != NULL );
   S = gan_matf_srmultT_q ( A, S ); cu_assert ( S != NULL );
   Cp = gan_matf_from_squmatf_q ( S, Cp ); cu_assert ( Cp != NULL );
   Cp = matrixf_add ( Cp, 1.0F, C, -1.0F, Cp ); cu_assert ( Cp != NULL );
   cu_assert ( gan_matf_Fnorm(Cp) < 1.0e-4F );
   
   /* test left self-multiplication of a matrix, producing a symmetric
      matrix */
   A = matrixf_rand ( A, 5, 3 ); cu_assert ( A != NULL );
   AT = matrixf_transpose ( A, AT ); cu_assert ( AT != NULL );
   C = matrixf_matrixf_mult ( AT, A, C ); cu_assert ( C != NULL );
   S = gan_matf_slmultT_q ( A, S ); cu_assert ( S != NULL );
   Cp = gan_matf_from_squmatf_q ( S, Cp ); cu_assert ( Cp != NULL );
   Cp = matrixf_add ( Cp, 1.0F, C, -1.0F, Cp ); cu_assert ( Cp != NULL );
   cu_assert ( gan_matf_Fnorm(Cp) < 1.0e-4F );

   /* test matrix inverse */
   A = matrixf_rand ( A, 5, 5 ); cu_assert ( A != NULL );
   B = gan_matf_invert_q ( A, B ); cu_assert ( B != NULL );
   C = matrixf_matrixf_mult ( A, B, C ); cu_assert ( C != NULL );
   Cp = matrixf_ident ( Cp, C->rows ); cu_assert ( Cp != NULL );
   Cp = matrixf_add ( Cp, 1.0F, C, -1.0F, Cp ); cu_assert ( Cp != NULL );
   cu_assert ( gan_matf_Fnorm(Cp) < 1.0e-4F );

   /* free matrices and vectors */
   gan_squmatf_free_va ( T, S, NULL );
   gan_vecf_free_va ( bp, b, a, NULL );
   gan_matf_free_va ( Cp, C, BT, B, AT, A, NULL );

   /* return with success */
   return GAN_TRUE;
}

/* Tests for general size square matrices. We apply an operation both using
 * Gandalf functions and simplified functions defined in this test module.
 */
static Gan_Bool test_mat_square ( Gan_SquMatrixType type )
{
   Gan_SquMatrix *A, *B, *C, *Cp;
   Gan_Matrix *Am, *AmT, *AmI, *AmIT, *Bm, *BmT, *Cm, *Cmp;
   Gan_Vector *a, *b, *bp;

   /* allocate matrices and vectors used in tests */
   A    = gan_squmat_alloc ( type, 0 ); cu_assert ( A  != NULL );
   B    = gan_squmat_alloc ( type, 0 ); cu_assert ( B  != NULL );
   C    = gan_squmat_alloc ( type, 0 ); cu_assert ( C  != NULL );
   Cp   = gan_squmat_alloc ( type, 0 ); cu_assert ( Cp != NULL );
   Am   = gan_mat_alloc(0,0);
   AmT  = gan_mat_alloc(0,0);
   AmI  = gan_mat_alloc(0,0);
   AmIT = gan_mat_alloc(0,0);
   Bm   = gan_mat_alloc(0,0);
   BmT  = gan_mat_alloc(0,0);
   Cm   = gan_mat_alloc(0,0);
   Cmp  = gan_mat_alloc(0,0);
   a    = gan_vec_alloc(0); cu_assert ( a  != NULL );
   b    = gan_vec_alloc(0); cu_assert ( b  != NULL );
   bp   = gan_vec_alloc(0); cu_assert ( bp != NULL );

   if ( type != GAN_SYMMETRIC_MATRIX )
   {
      /* test right multiplication of inverse of square matrix by the inverse
         transpose of the same matrix */
      A = squmatrix_rand ( A, type, 3, 3 ); cu_assert ( A != NULL );
      Am = gan_mat_from_squmat_q ( A, Am ); cu_assert ( Am != NULL );
      AmI = gan_mat_invert_q ( Am, AmI ); cu_assert ( AmI != NULL );
      AmIT = matrix_transpose ( AmI, AmIT ); cu_assert (AmIT != NULL);
      Cmp = matrix_matrix_mult ( AmI, AmIT, Cmp ); cu_assert ( Cmp != NULL );
      B = gan_squmatI_srmultIT_squ_q ( A, B ); cu_assert ( B != NULL );
      Bm = gan_mat_from_squmat_q ( B, Bm ); cu_assert ( Bm != NULL );
      Cmp = matrix_add ( Cmp, 1.0, Bm, -1.0, Cmp ); cu_assert ( Cmp != NULL );
      cu_assert ( gan_mat_Fnorm(Cmp) < 1.0e-6 );
   }
   
   /* test square matrix copy */
   A = squmatrix_rand ( A, type, 4, 4 ); cu_assert ( A != NULL );
   Cp = squmatrix_add ( A, 1.0, NULL, 0.0, Cp ); cu_assert (Cp != NULL);
   C = gan_squmat_copy_q ( A, C ); cu_assert ( C != NULL );
   Cp = squmatrix_add ( Cp, 1.0, C, -1.0, Cp ); cu_assert (Cp != NULL);
   cu_assert ( gan_squmat_Fnorm(Cp) < 1.0e-6 );

   /* test square matrix multiply by scalar */
   A = squmatrix_rand ( A, type, 4, 4 ); cu_assert ( A != NULL );
   Cp = squmatrix_add ( A, 4.5, NULL, 0.0, Cp ); cu_assert (Cp != NULL);
   C = gan_squmat_scale_q ( A, 4.5, C ); cu_assert ( C != NULL );
   Cp = squmatrix_add ( Cp, 1.0, C, -1.0, Cp ); cu_assert (Cp != NULL);
   cu_assert ( gan_squmat_Fnorm(Cp) < 1.0e-6 );

   /* test square matrix multiply by scalar in-place */
   A = squmatrix_rand ( A, type,4, 4 ); cu_assert ( A != NULL );
   Cp = squmatrix_add ( A, 4.5, NULL, 0.0, Cp ); cu_assert (Cp != NULL);
   A = gan_squmat_scale_i ( A, 4.5 ); cu_assert ( A != NULL );
   Cp = squmatrix_add ( Cp, 1.0, A, -1.0, Cp ); cu_assert (Cp != NULL);
   cu_assert ( gan_squmat_Fnorm(Cp) < 1.0e-6 );

   /* test square matrix divide by scalar */
   A = squmatrix_rand ( A, type, 4, 4 ); cu_assert ( A != NULL );
   Cp = squmatrix_add ( A,1.0/4.5, NULL,0.0, Cp); cu_assert (Cp != NULL);
   C = gan_squmat_divide_q ( A, 4.5, C ); cu_assert ( C != NULL );
   Cp = squmatrix_add ( Cp, 1.0, C, -1.0, Cp ); cu_assert (Cp != NULL);
   cu_assert ( gan_squmat_Fnorm(Cp) < 1.0e-6 );

   /* test square matrix divide by scalar in-place */
   A = squmatrix_rand ( A, type, 4, 4 ); cu_assert ( A != NULL );
   Cp = squmatrix_add ( A,1.0/4.5, NULL,0.0, Cp); cu_assert (Cp != NULL);
   A = gan_squmat_divide_i ( A, 4.5 ); cu_assert ( A != NULL );
   Cp = squmatrix_add ( Cp, 1.0, A, -1.0, Cp ); cu_assert (Cp != NULL);
   cu_assert ( gan_squmat_Fnorm(Cp) < 1.0e-6 );

   /* test square matrix negation */
   A = squmatrix_rand ( A, type, 4, 4 ); cu_assert ( A != NULL );
   Cp = squmatrix_add ( A, -1.0, NULL, 0.0, Cp ); cu_assert (Cp != NULL);
   C = gan_squmat_negate_q ( A, C ); cu_assert ( C != NULL );
   Cp = squmatrix_add ( Cp, 1.0, C, -1.0, Cp ); cu_assert (Cp != NULL);
   cu_assert ( gan_squmat_Fnorm(Cp) < 1.0e-6 );

   /* test square matrix negation in-place */
   A = squmatrix_rand ( A, type, 4, 4 ); cu_assert ( A != NULL );
   Cp = squmatrix_add ( A, -1.0, NULL, 0.0, Cp ); cu_assert (Cp != NULL);
   A = gan_squmat_negate_i(A); cu_assert ( A != NULL );
   Cp = squmatrix_add ( Cp, 1.0, A, -1.0, Cp ); cu_assert (Cp != NULL);
   cu_assert ( gan_squmat_Fnorm(Cp) < 1.0e-6 );

   /* test square matrix addition */
   A = squmatrix_rand ( A, type, 4, 4 ); cu_assert ( A != NULL );
   B = squmatrix_rand ( B, type, 4, 4 ); cu_assert ( B != NULL );
   Cp = squmatrix_add ( A, 1.0, B, 1.0, Cp ); cu_assert ( Cp != NULL );
   C = gan_squmat_add_q ( A, B, C ); cu_assert ( C != NULL );
   Cp = squmatrix_add ( Cp, 1.0, C, -1.0, Cp ); cu_assert (Cp != NULL);
   cu_assert ( gan_squmat_Fnorm(Cp) < 1.0e-6 );
   
   /* test in-place square matrix addition in first argument */
   A = squmatrix_rand ( A, type, 4, 4 ); cu_assert ( A != NULL );
   B = squmatrix_rand ( B, type, 4, 4 ); cu_assert ( B != NULL );
   Cp = squmatrix_add ( A, 1.0, B, 1.0, Cp ); cu_assert ( Cp != NULL );
   A = gan_squmat_add_i1 ( A, B ); cu_assert ( A != NULL );
   Cp = squmatrix_add ( Cp, 1.0, A, -1.0, Cp ); cu_assert (Cp != NULL);
   cu_assert ( gan_squmat_Fnorm(Cp) < 1.0e-6 );
   
   /* test in-place square matrix addition in second argument */
   A = squmatrix_rand ( A, type, 4, 4 ); cu_assert ( A != NULL );
   B = squmatrix_rand ( B, type, 4, 4 ); cu_assert ( B != NULL );
   Cp = squmatrix_add ( A, 1.0, B, 1.0, Cp ); cu_assert ( Cp != NULL );
   B = gan_squmat_add_i2 ( A, B ); cu_assert ( B != NULL );
   Cp = squmatrix_add ( Cp, 1.0, B, -1.0, Cp ); cu_assert (Cp != NULL);
   cu_assert ( gan_squmat_Fnorm(Cp) < 1.0e-6 );
   
   /* test square matrix subtraction */
   A = squmatrix_rand ( A, type, 4, 4 ); cu_assert ( A != NULL );
   B = squmatrix_rand ( B, type, 4, 4 ); cu_assert ( B != NULL );
   Cp = squmatrix_add ( A, 1.0, B, -1.0, Cp ); cu_assert (Cp != NULL);
   C = gan_squmat_sub_q ( A, B, C ); cu_assert ( C != NULL );
   Cp = squmatrix_add ( Cp, 1.0, C, -1.0, Cp ); cu_assert (Cp != NULL);
   cu_assert ( gan_squmat_Fnorm(Cp) < 1.0e-6 );

   /* test in-place square matrix subtraction in first argument */
   A = squmatrix_rand ( A, type, 4, 4 ); cu_assert ( A != NULL );
   B = squmatrix_rand ( B, type, 4, 4 ); cu_assert ( B != NULL );
   Cp = squmatrix_add ( A, 1.0, B, -1.0, Cp ); cu_assert (Cp != NULL);
   A = gan_squmat_sub_i1 ( A, B ); cu_assert ( A != NULL );
   Cp = squmatrix_add ( Cp, 1.0, A, -1.0, Cp ); cu_assert (Cp != NULL);
   cu_assert ( gan_squmat_Fnorm(Cp) < 1.0e-6 );
   
   /* test in-place square matrix subtraction in second argument */
   A = squmatrix_rand ( A, type, 4, 4 ); cu_assert ( A != NULL );
   B = squmatrix_rand ( B, type, 4, 4 ); cu_assert ( B != NULL );
   Cp = squmatrix_add ( A, 1.0, B, -1.0, Cp ); cu_assert (Cp != NULL);
   B = gan_squmat_sub_i2 ( A, B ); cu_assert ( B != NULL );
   Cp = squmatrix_add ( Cp, 1.0, B, -1.0, Cp ); cu_assert (Cp != NULL);
   cu_assert ( gan_squmat_Fnorm(Cp) < 1.0e-6 );

   /* test square matrix/vector multiplication */
   A = squmatrix_rand ( A, type, 4, 4 ); cu_assert ( A != NULL );
   Am = gan_mat_from_squmat_q ( A, Am ); cu_assert ( Am != NULL );
   a = vector_rand ( a, 4 ); cu_assert ( a != NULL );
   bp = matrix_vector_mult ( Am, a, bp ); cu_assert ( bp != NULL );
   b = gan_squmat_multv_q ( A, a, b ); cu_assert ( b != NULL );
   bp = vector_add ( bp, 1.0, b, -1.0, bp ); cu_assert ( bp != NULL );
   cu_assert ( gan_vec_sqrlen(bp) < 1.0e-6 );

   /* test square matrix/vector multiplication with matrix transposed */
   A = squmatrix_rand ( A, type, 4, 4 ); cu_assert ( A != NULL );
   Am = gan_mat_from_squmat_q ( A, Am ); cu_assert ( Am != NULL );
   AmT = matrix_transpose ( Am, AmT ); cu_assert ( AmT != NULL );
   a = vector_rand ( a, 4 ); cu_assert ( a != NULL );
   bp = matrix_vector_mult ( AmT, a, bp ); cu_assert ( bp != NULL );
   b = gan_squmatT_multv_q ( A, a, b ); cu_assert ( b != NULL );
   bp = vector_add ( bp, 1.0, b, -1.0, bp ); cu_assert ( bp != NULL );
   cu_assert ( gan_vec_sqrlen(bp) < 1.0e-6 );

   if ( type != GAN_SYMMETRIC_MATRIX )
   {
      /* test square matrix/vector multiplication in-place */
      A = squmatrix_rand ( A, type, 4, 4 ); cu_assert ( A != NULL );
      Am = gan_mat_from_squmat_q ( A, Am ); cu_assert ( Am != NULL );
      a = vector_rand ( a, 4 ); cu_assert ( a != NULL );
      bp = matrix_vector_mult ( Am, a, bp ); cu_assert ( bp != NULL );
      a = gan_squmat_multv_i ( A, a ); cu_assert ( a != NULL );
      bp = vector_add ( bp, 1.0, a, -1.0, bp ); cu_assert (bp != NULL);
      cu_assert ( gan_vec_sqrlen(bp) < 1.0e-6 );

      /* test square matrix/vector multiplication with matrix transposed
         in-place */
      A = squmatrix_rand ( A, type, 4, 4 ); cu_assert ( A != NULL );
      Am = gan_mat_from_squmat_q ( A, Am ); cu_assert ( Am != NULL );
      AmT = matrix_transpose ( Am, AmT ); cu_assert ( AmT != NULL );
      a = vector_rand ( a, 4 ); cu_assert ( a != NULL );
      bp = matrix_vector_mult ( AmT, a, bp ); cu_assert ( bp != NULL );
      a = gan_squmatT_multv_i ( A, a ); cu_assert ( a != NULL );
      bp = vector_add ( bp, 1.0, a, -1.0, bp ); cu_assert (bp != NULL);
      cu_assert ( gan_vec_sqrlen(bp) < 1.0e-6 );

      /* test square matrix/vector multiplication with matrix inverted */
      A = squmatrix_rand ( A, type, 4, 4 ); cu_assert ( A != NULL );
      Am = gan_mat_from_squmat_q ( A, Am ); cu_assert ( Am != NULL );
      AmI = gan_mat_invert_q ( Am, AmI ); cu_assert ( AmI != NULL );
      a = vector_rand ( a, 4 ); cu_assert ( a != NULL );
      bp = matrix_vector_mult ( AmI, a, bp ); cu_assert ( bp != NULL );
      b = gan_squmatI_multv_q ( A, a, b ); cu_assert ( b != NULL );
      bp = vector_add ( bp, 1.0, b, -1.0, bp ); cu_assert (bp != NULL);
      cu_assert ( gan_vec_sqrlen(bp) < 1.0e-6 );

      /* test square matrix/vector multiplication with matrix inverted
         in-place */
      A = squmatrix_rand ( A, type, 4, 4 ); cu_assert ( A != NULL );
      Am = gan_mat_from_squmat_q ( A, Am ); cu_assert ( Am != NULL );
      AmI = gan_mat_invert_q ( Am, AmI ); cu_assert ( AmI != NULL );
      a = vector_rand ( a, 4 ); cu_assert ( a != NULL );
      bp = matrix_vector_mult ( AmI, a, bp ); cu_assert ( bp != NULL );
      a = gan_squmatI_multv_i ( A, a ); cu_assert ( a != NULL );
      bp = vector_add ( bp, 1.0, a, -1.0, bp ); cu_assert (bp != NULL);
      cu_assert ( gan_vec_sqrlen(bp) < 1.0e-6 );

      /* test square matrix/vector multiplication with matrix inverted &
         transposed */
      A = squmatrix_rand ( A, type, 4, 4 ); cu_assert ( A != NULL );
      Am = gan_mat_from_squmat_q ( A, Am ); cu_assert ( Am != NULL );
      AmI = gan_mat_invert_q ( Am, AmI ); cu_assert ( AmI != NULL );
      AmIT = matrix_transpose ( AmI, AmIT ); cu_assert (AmIT != NULL);
      a = vector_rand ( a, 4 ); cu_assert ( a != NULL );
      bp = matrix_vector_mult ( AmIT, a, bp ); cu_assert (bp != NULL);
      b = gan_squmatIT_multv_q ( A, a, b ); cu_assert ( b != NULL );
      bp = vector_add ( bp, 1.0, b, -1.0, bp ); cu_assert (bp != NULL);
      cu_assert ( gan_vec_sqrlen(bp) < 1.0e-6 );

      /* test square matrix/vector multiplication with matrix inverted and
         transposed in-place */
      A = squmatrix_rand ( A, type, 4, 4 ); cu_assert ( A != NULL );
      Am = gan_mat_from_squmat_q ( A, Am ); cu_assert ( Am != NULL );
      AmI = gan_mat_invert_q ( Am, AmI ); cu_assert ( AmI != NULL );
      AmIT = matrix_transpose ( AmI, AmIT ); cu_assert (AmIT != NULL);
      a = vector_rand ( a, 4 ); cu_assert ( a != NULL );
      bp = matrix_vector_mult ( AmIT, a, bp ); cu_assert (bp != NULL);
      a = gan_squmatIT_multv_i ( A, a ); cu_assert ( a != NULL );
      bp = vector_add ( bp, 1.0, a, -1.0, bp ); cu_assert (bp != NULL);
      cu_assert ( gan_vec_sqrlen(bp) < 1.0e-6 );
   }

   /* test right multiplication of square matrix by generic matrix */
   A = squmatrix_rand ( A, type, 3, 3 ); cu_assert ( A != NULL );
   Am = gan_mat_from_squmat_q ( A, Am ); cu_assert ( Am != NULL );
   Bm = matrix_rand ( Bm, 3, 5 ); cu_assert ( Bm != NULL );
   Cmp = matrix_matrix_mult ( Am, Bm, Cmp ); cu_assert ( Cmp != NULL );
   Cm = gan_squmat_rmult_q ( A, Bm, Cm ); cu_assert ( Cm != NULL );
   Cmp = matrix_add ( Cmp, 1.0, Cm, -1.0, Cmp ); cu_assert (Cmp != NULL);
   cu_assert ( gan_mat_Fnorm(Cmp) < 1.0e-6 );

   /* test right multiplication of square matrix by transpose of generic
      matrix */
   A = squmatrix_rand ( A, type, 3, 3 ); cu_assert ( A != NULL );
   Am = gan_mat_from_squmat_q ( A, Am ); cu_assert ( Am != NULL );
   Bm = matrix_rand ( Bm, 5, 3 ); cu_assert ( Bm != NULL );
   BmT = matrix_transpose ( Bm, BmT ); cu_assert ( BmT != NULL );
   Cmp = matrix_matrix_mult ( Am, BmT, Cmp ); cu_assert (Cmp != NULL);
   Cm = gan_squmat_rmultT_q ( A, Bm, Cm ); cu_assert ( Cm != NULL );
   Cmp = matrix_add ( Cmp, 1.0, Cm, -1.0, Cmp ); cu_assert (Cmp != NULL);
   cu_assert ( gan_mat_Fnorm(Cmp) < 1.0e-6 );

   /* test right multiplication of transpose of square matrix by generic
      matrix */
   A = squmatrix_rand ( A, type, 3, 3 ); cu_assert ( A != NULL );
   Am = gan_mat_from_squmat_q ( A, Am ); cu_assert ( Am != NULL );
   AmT = matrix_transpose ( Am, AmT ); cu_assert ( AmT != NULL );
   Bm = matrix_rand ( Bm, 3, 5 ); cu_assert ( Bm != NULL );
   Cmp = matrix_matrix_mult ( AmT, Bm, Cmp ); cu_assert (Cmp != NULL);
   Cm = gan_squmatT_rmult_q ( A, Bm, Cm ); cu_assert ( Cm != NULL );
   Cmp = matrix_add ( Cmp, 1.0, Cm, -1.0, Cmp ); cu_assert (Cmp != NULL);
   cu_assert ( gan_mat_Fnorm(Cmp) < 1.0e-6 );

   /* test right multiplication of transpose of square matrix by transpose of
      generic matrix */
   A = squmatrix_rand ( A, type, 3, 3 ); cu_assert ( A != NULL );
   Am = gan_mat_from_squmat_q ( A, Am ); cu_assert ( Am != NULL );
   AmT = matrix_transpose ( Am, AmT ); cu_assert ( AmT != NULL );
   Bm = matrix_rand ( Bm, 5, 3 ); cu_assert ( Bm != NULL );
   BmT = matrix_transpose ( Bm, BmT ); cu_assert ( BmT != NULL );
   Cmp = matrix_matrix_mult ( AmT, BmT, Cmp ); cu_assert (Cmp != NULL);
   Cm = gan_squmatT_rmultT_q ( A, Bm, Cm ); cu_assert ( Cm != NULL );
   Cmp = matrix_add ( Cmp, 1.0, Cm, -1.0, Cmp ); cu_assert (Cmp != NULL);
   cu_assert ( gan_mat_Fnorm(Cmp) < 1.0e-6 );

   if ( type != GAN_SYMMETRIC_MATRIX )
   {
      /* test right multiplication of square matrix by generic matrix
         in-place */
      A = squmatrix_rand ( A, type, 3, 3 ); cu_assert ( A != NULL );
      Am = gan_mat_from_squmat_q ( A, Am ); cu_assert ( Am != NULL );
      Bm = matrix_rand ( Bm, 3, 5 ); cu_assert ( Bm != NULL );
      Cmp = matrix_matrix_mult ( Am, Bm, Cmp ); cu_assert (Cmp != NULL);
      Bm = gan_squmat_rmult_i ( A, Bm ); cu_assert ( Bm != NULL );
      Cmp = matrix_add ( Cmp, 1.0, Bm, -1.0, Cmp ); cu_assert ( Cmp != NULL );
      cu_assert ( gan_mat_Fnorm(Cmp) < 1.0e-6 );

      /* test right multiplication of transpose of square matrix by generic
         matrix in-place */
      A = squmatrix_rand ( A, type, 3, 3 ); cu_assert ( A != NULL );
      Am = gan_mat_from_squmat_q ( A, Am ); cu_assert ( Am != NULL );
      AmT = matrix_transpose ( Am, AmT ); cu_assert ( AmT != NULL );
      Bm = matrix_rand ( Bm, 3, 5 ); cu_assert ( Bm != NULL );
      Cmp = matrix_matrix_mult ( AmT, Bm, Cmp ); cu_assert (Cmp != NULL);
      Bm = gan_squmatT_rmult_i ( A, Bm ); cu_assert ( Bm != NULL );
      Cmp = matrix_add ( Cmp, 1.0, Bm, -1.0, Cmp ); cu_assert ( Cmp != NULL );
      cu_assert ( gan_mat_Fnorm(Cmp) < 1.0e-6 );

      /* test right multiplication of inverse of square matrix by generic
         matrix */
      A = squmatrix_rand ( A, type, 3, 3 ); cu_assert ( A != NULL );
      Am = gan_mat_from_squmat_q ( A, Am ); cu_assert ( Am != NULL );
      AmI = gan_mat_invert_q ( Am, AmI ); cu_assert ( AmI != NULL );
      Bm = matrix_rand ( Bm, 3, 5 ); cu_assert ( Bm != NULL );
      Cmp = matrix_matrix_mult ( AmI, Bm, Cmp ); cu_assert (Cmp != NULL);
      Cm = gan_squmatI_rmult_q ( A, Bm, Cm ); cu_assert ( Cm != NULL );
      Cmp = matrix_add ( Cmp, 1.0, Cm, -1.0, Cmp ); cu_assert ( Cmp != NULL );
      cu_assert ( gan_mat_Fnorm(Cmp) < 1.0e-6 );

      /* test right multiplication of inverse of square matrix by generic
         matrix in-place */
      A = squmatrix_rand ( A, type, 3, 3 ); cu_assert ( A != NULL );
      Am = gan_mat_from_squmat_q ( A, Am ); cu_assert ( Am != NULL );
      AmI = gan_mat_invert_q ( Am, AmI ); cu_assert ( AmI != NULL );
      Bm = matrix_rand ( Bm, 3, 5 ); cu_assert ( Bm != NULL );
      Cmp = matrix_matrix_mult ( AmI, Bm, Cmp ); cu_assert (Cmp != NULL);
      Bm = gan_squmatI_rmult_i ( A, Bm ); cu_assert ( Bm != NULL );
      Cmp = matrix_add ( Cmp, 1.0, Bm, -1.0, Cmp ); cu_assert ( Cmp != NULL );
      cu_assert ( gan_mat_Fnorm(Cmp) < 1.0e-6 );

      /* test right multiplication of inverse of square matrix by transpose of
         generic matrix */
      A = squmatrix_rand ( A, type, 3, 3 ); cu_assert ( A != NULL );
      Am = gan_mat_from_squmat_q ( A, Am ); cu_assert ( Am != NULL );
      AmI = gan_mat_invert_q ( Am, AmI ); cu_assert ( AmI != NULL );
      Bm = matrix_rand ( Bm, 5, 3 ); cu_assert ( Bm != NULL );
      BmT = matrix_transpose ( Bm, BmT ); cu_assert ( BmT != NULL );
      Cmp = matrix_matrix_mult ( AmI, BmT, Cmp );cu_assert (Cmp != NULL);
      Cm = gan_squmatI_rmultT_q ( A, Bm, Cm ); cu_assert (Cm != NULL);
      Cmp = matrix_add ( Cmp, 1.0, Cm, -1.0, Cmp ); cu_assert ( Cmp != NULL );
      cu_assert ( gan_mat_Fnorm(Cmp) < 1.0e-6 );

      /* test right multiplication of inverse transpose of square matrix by
         generic matrix */
      A = squmatrix_rand ( A, type, 3, 3 ); cu_assert ( A != NULL );
      Am = gan_mat_from_squmat_q ( A, Am ); cu_assert ( Am != NULL );
      AmI = gan_mat_invert_q ( Am, AmI ); cu_assert ( AmI != NULL );
      AmIT = matrix_transpose ( AmI, AmIT ); cu_assert (AmIT != NULL);
      Bm = matrix_rand ( Bm, 3, 5 ); cu_assert ( Bm != NULL );
      Cmp = matrix_matrix_mult ( AmIT, Bm, Cmp );cu_assert (Cmp != NULL);
      Cm = gan_squmatIT_rmult_q ( A, Bm, Cm ); cu_assert (Cm != NULL);
      Cmp = matrix_add ( Cmp, 1.0, Cm, -1.0, Cmp ); cu_assert ( Cmp != NULL );
      cu_assert ( gan_mat_Fnorm(Cmp) < 1.0e-6 );

      /* test right multiplication of inverse transpose of square matrix by
         generic matrix in-place */
      A = squmatrix_rand ( A, type, 3, 3 ); cu_assert ( A != NULL );
      Am = gan_mat_from_squmat_q ( A, Am ); cu_assert ( Am != NULL );
      AmI = gan_mat_invert_q ( Am, AmI ); cu_assert ( AmI != NULL );
      AmIT = matrix_transpose ( AmI, AmIT ); cu_assert (AmIT != NULL);
      Bm = matrix_rand ( Bm, 3, 5 ); cu_assert ( Bm != NULL );
      Cmp = matrix_matrix_mult ( AmIT, Bm, Cmp );cu_assert (Cmp != NULL);
      Bm = gan_squmatIT_rmult_i ( A, Bm ); cu_assert ( Bm != NULL );
      Cmp = matrix_add ( Cmp, 1.0, Bm, -1.0, Cmp ); cu_assert ( Cmp != NULL );
      cu_assert ( gan_mat_Fnorm(Cmp) < 1.0e-6 );

      /* test right multiplication of inverse transpose of square matrix by
         transpose of generic matrix */
      A = squmatrix_rand ( A, type, 3, 3 ); cu_assert ( A != NULL );
      Am = gan_mat_from_squmat_q ( A, Am ); cu_assert ( Am != NULL );
      AmI = gan_mat_invert_q ( Am, AmI ); cu_assert ( AmI != NULL );
      AmIT = matrix_transpose ( AmI, AmIT ); cu_assert (AmIT != NULL);
      Bm = matrix_rand ( Bm, 5, 3 ); cu_assert ( Bm != NULL );
      BmT = matrix_transpose ( Bm, BmT ); cu_assert ( BmT != NULL );
      Cmp = matrix_matrix_mult ( AmIT, BmT, Cmp ); cu_assert ( Cmp != NULL );
      Cm = gan_squmatIT_rmultT_q ( A, Bm, Cm ); cu_assert (Cm != NULL);
      Cmp = matrix_add ( Cmp, 1.0, Cm, -1.0, Cmp ); cu_assert ( Cmp != NULL );
      cu_assert ( gan_mat_Fnorm(Cmp) < 1.0e-6 );
   }

   /* test left multiplication of square matrix by generic matrix */
   A = squmatrix_rand ( A, type, 3, 3 ); cu_assert ( A != NULL );
   Am = gan_mat_from_squmat_q ( A, Am ); cu_assert ( Am != NULL );
   Bm = matrix_rand ( Bm, 5, 3 ); cu_assert ( Bm != NULL );
   Cmp = matrix_matrix_mult ( Bm, Am, Cmp ); cu_assert ( Cmp != NULL );
   Cm = gan_squmat_lmult_q ( A, Bm, Cm ); cu_assert ( Cm != NULL );
   Cmp = matrix_add ( Cmp, 1.0, Cm, -1.0, Cmp ); cu_assert (Cmp != NULL);
   cu_assert ( gan_mat_Fnorm(Cmp) < 1.0e-6 );

   /* test left multiplication of square matrix by transpose of generic
      matrix */
   A = squmatrix_rand ( A, type, 3, 3 ); cu_assert ( A != NULL );
   Am = gan_mat_from_squmat_q ( A, Am ); cu_assert ( Am != NULL );
   Bm = matrix_rand ( Bm, 3, 5 ); cu_assert ( Bm != NULL );
   BmT = matrix_transpose ( Bm, BmT ); cu_assert ( BmT != NULL );
   Cmp = matrix_matrix_mult ( BmT, Am, Cmp ); cu_assert (Cmp != NULL);
   Cm = gan_squmat_lmultT_q ( A, Bm, Cm ); cu_assert ( Cm != NULL );
   Cmp = matrix_add ( Cmp, 1.0, Cm, -1.0, Cmp ); cu_assert (Cmp != NULL);
   cu_assert ( gan_mat_Fnorm(Cmp) < 1.0e-6 );

   /* test left multiplication of transpose of square matrix by generic
      matrix */
   A = squmatrix_rand ( A, type, 3, 3 ); cu_assert ( A != NULL );
   Am = gan_mat_from_squmat_q ( A, Am ); cu_assert ( Am != NULL );
   AmT = matrix_transpose ( Am, AmT ); cu_assert ( AmT != NULL );
   Bm = matrix_rand ( Bm, 5, 3 ); cu_assert ( Bm != NULL );
   Cmp = matrix_matrix_mult ( Bm, AmT, Cmp ); cu_assert (Cmp != NULL);
   Cm = gan_squmatT_lmult_q ( A, Bm, Cm ); cu_assert ( Cm != NULL );
   Cmp = matrix_add ( Cmp, 1.0, Cm, -1.0, Cmp ); cu_assert (Cmp != NULL);
   cu_assert ( gan_mat_Fnorm(Cmp) < 1.0e-6 );

   /* test left multiplication of transpose of square matrix by transpose of
      generic matrix */
   A = squmatrix_rand ( A, type, 1, 1 ); cu_assert ( A != NULL );
   Am = gan_mat_from_squmat_q ( A, Am ); cu_assert ( Am != NULL );
   AmT = matrix_transpose ( Am, AmT ); cu_assert ( AmT != NULL );
   Bm = matrix_rand ( Bm, 1, 1 ); cu_assert ( Bm != NULL );
   BmT = matrix_transpose ( Bm, BmT ); cu_assert ( BmT != NULL );
   Cmp = matrix_matrix_mult ( BmT, AmT, Cmp ); cu_assert (Cmp != NULL);
   Cm = gan_squmatT_lmultT_q ( A, Bm, Cm ); cu_assert ( Cm != NULL );
   Cmp = matrix_add ( Cmp, 1.0, Cm, -1.0, Cmp ); cu_assert (Cmp != NULL);
   cu_assert ( gan_mat_Fnorm(Cmp) < 1.0e-6 );

   if ( type != GAN_SYMMETRIC_MATRIX )
   {
      /* test left multiplication of square matrix by generic matrix
         in-place */
      A = squmatrix_rand ( A, type, 3, 3 ); cu_assert ( A != NULL );
      Am = gan_mat_from_squmat_q ( A, Am ); cu_assert ( Am != NULL );
      Bm = matrix_rand ( Bm, 5, 3 ); cu_assert ( Bm != NULL );
      Cmp = matrix_matrix_mult ( Bm, Am, Cmp ); cu_assert (Cmp != NULL);
      Bm = gan_squmat_lmult_i ( A, Bm ); cu_assert ( Bm != NULL );
      Cmp = matrix_add ( Cmp, 1.0, Bm, -1.0, Cmp ); cu_assert ( Cmp != NULL );
      cu_assert ( gan_mat_Fnorm(Cmp) < 1.0e-6 );

      /* test left multiplication of transpose of square matrix by generic
         matrix in-place */
      A = squmatrix_rand ( A, type, 3, 3 ); cu_assert ( A != NULL );
      Am = gan_mat_from_squmat_q ( A, Am ); cu_assert ( Am != NULL );
      AmT = matrix_transpose ( Am, AmT ); cu_assert ( AmT != NULL );
      Bm = matrix_rand ( Bm, 5, 3 ); cu_assert ( Bm != NULL );
      Cmp = matrix_matrix_mult ( Bm, AmT, Cmp ); cu_assert (Cmp != NULL);
      Bm = gan_squmatT_lmult_i ( A, Bm ); cu_assert ( Bm != NULL );
      Cmp = matrix_add ( Cmp, 1.0, Bm, -1.0, Cmp ); cu_assert ( Cmp != NULL );
      cu_assert ( gan_mat_Fnorm(Cmp) < 1.0e-6 );

      /* test left multiplication of inverse of square matrix by generic
         matrix */
      A = squmatrix_rand ( A, type, 3, 3 ); cu_assert ( A != NULL );
      Am = gan_mat_from_squmat_q ( A, Am ); cu_assert ( Am != NULL );
      AmI = gan_mat_invert_q ( Am, AmI ); cu_assert ( AmI != NULL );
      Bm = matrix_rand ( Bm, 5, 3 ); cu_assert ( Bm != NULL );
      Cmp = matrix_matrix_mult ( Bm, AmI, Cmp ); cu_assert (Cmp != NULL);
      Cm = gan_squmatI_lmult_q ( A, Bm, Cm ); cu_assert ( Cm != NULL );
      Cmp = matrix_add ( Cmp, 1.0, Cm, -1.0, Cmp ); cu_assert ( Cmp != NULL );
      cu_assert ( gan_mat_Fnorm(Cmp) < 1.0e-6 );

      /* test left multiplication of inverse of square matrix by generic
         matrix in-place */
      A = squmatrix_rand ( A, type, 3, 3 ); cu_assert ( A != NULL );
      Am = gan_mat_from_squmat_q ( A, Am ); cu_assert ( Am != NULL );
      AmI = gan_mat_invert_q ( Am, AmI ); cu_assert ( AmI != NULL );
      Bm = matrix_rand ( Bm, 5, 3 ); cu_assert ( Bm != NULL );
      Cmp = matrix_matrix_mult ( Bm, AmI, Cmp ); cu_assert (Cmp != NULL);
      Bm = gan_squmatI_lmult_i ( A, Bm ); cu_assert ( Bm != NULL );
      Cmp = matrix_add ( Cmp, 1.0, Bm, -1.0, Cmp ); cu_assert ( Cmp != NULL );
      cu_assert ( gan_mat_Fnorm(Cmp) < 1.0e-6 );

      /* test left multiplication of inverse of square matrix by transpose of
         generic matrix */
      A = squmatrix_rand ( A, type, 3, 3 ); cu_assert ( A != NULL );
      Am = gan_mat_from_squmat_q ( A, Am ); cu_assert ( Am != NULL );
      AmI = gan_mat_invert_q ( Am, AmI ); cu_assert ( AmI != NULL );
      Bm = matrix_rand ( Bm, 3, 5 ); cu_assert ( Bm != NULL );
      BmT = matrix_transpose ( Bm, BmT ); cu_assert ( BmT != NULL );
      Cmp = matrix_matrix_mult ( BmT, AmI, Cmp );cu_assert (Cmp != NULL);
      Cm = gan_squmatI_lmultT_q ( A, Bm, Cm ); cu_assert (Cm != NULL);
      Cmp = matrix_add ( Cmp, 1.0, Cm, -1.0, Cmp ); cu_assert ( Cmp != NULL );
      cu_assert ( gan_mat_Fnorm(Cmp) < 1.0e-6 );

      /* test left multiplication of inverse transpose of square matrix by
         generic matrix */
      A = squmatrix_rand ( A, type, 3, 3 ); cu_assert ( A != NULL );
      Am = gan_mat_from_squmat_q ( A, Am ); cu_assert ( Am != NULL );
      AmI = gan_mat_invert_q ( Am, AmI ); cu_assert ( AmI != NULL );
      AmIT = matrix_transpose ( AmI, AmIT ); cu_assert (AmIT != NULL);
      Bm = matrix_rand ( Bm, 5, 3 ); cu_assert ( Bm != NULL );
      Cmp = matrix_matrix_mult ( Bm, AmIT, Cmp );cu_assert (Cmp != NULL);
      Cm = gan_squmatIT_lmult_q ( A, Bm, Cm ); cu_assert (Cm != NULL);
      Cmp = matrix_add ( Cmp, 1.0, Cm, -1.0, Cmp ); cu_assert ( Cmp != NULL );
      cu_assert ( gan_mat_Fnorm(Cmp) < 1.0e-6 );

      /* test left multiplication of inverse transpose of square matrix by
         generic matrix in-place */
      A = squmatrix_rand ( A, type, 3, 3 ); cu_assert ( A != NULL );
      Am = gan_mat_from_squmat_q ( A, Am ); cu_assert ( Am != NULL );
      AmI = gan_mat_invert_q ( Am, AmI ); cu_assert ( AmI != NULL );
      AmIT = matrix_transpose ( AmI, AmIT ); cu_assert (AmIT != NULL);
      Bm = matrix_rand ( Bm, 5, 3 ); cu_assert ( Bm != NULL );
      Cmp = matrix_matrix_mult ( Bm, AmIT, Cmp );cu_assert (Cmp != NULL);
      Bm = gan_squmatIT_lmult_i ( A, Bm ); cu_assert ( Bm != NULL );
      Cmp = matrix_add ( Cmp, 1.0, Bm, -1.0, Cmp ); cu_assert ( Cmp != NULL );
      cu_assert ( gan_mat_Fnorm(Cmp) < 1.0e-6 );

      /* test left multiplication of inverse transpose of square matrix by
         transpose of generic matrix */
      A = squmatrix_rand ( A, type, 3, 3 ); cu_assert ( A != NULL );
      Am = gan_mat_from_squmat_q ( A, Am ); cu_assert ( Am != NULL );
      AmI = gan_mat_invert_q ( Am, AmI ); cu_assert ( AmI != NULL );
      AmIT = matrix_transpose ( AmI, AmIT ); cu_assert (AmIT != NULL);
      Bm = matrix_rand ( Bm, 3, 5 ); cu_assert ( Bm != NULL );
      BmT = matrix_transpose ( Bm, BmT ); cu_assert ( BmT != NULL );
      Cmp = matrix_matrix_mult ( BmT, AmIT, Cmp ); cu_assert ( Cmp != NULL );
      Cm = gan_squmatIT_lmultT_q ( A, Bm, Cm ); cu_assert (Cm != NULL);
      Cmp = matrix_add ( Cmp, 1.0, Cm, -1.0, Cmp ); cu_assert ( Cmp != NULL );
      cu_assert ( gan_mat_Fnorm(Cmp) < 1.0e-6 );

      /* test right multiplication of square matrix by the transpose of the
         same matrix */
      A = squmatrix_rand ( A, type, 3, 3 ); cu_assert ( A != NULL );
      Am = gan_mat_from_squmat_q ( A, Am ); cu_assert ( Am != NULL );
      AmT = matrix_transpose ( Am, AmT ); cu_assert ( AmT != NULL );
      Cmp = matrix_matrix_mult ( Am, AmT, Cmp ); cu_assert ( Cmp != NULL );
      B = gan_squmat_srmultT_squ_q ( A, B ); cu_assert ( B != NULL );
      Bm = gan_mat_from_squmat_q ( B, Bm ); cu_assert ( Bm != NULL );
      Cmp = matrix_add ( Cmp, 1.0, Bm, -1.0, Cmp ); cu_assert ( Cmp != NULL );
      cu_assert ( gan_mat_Fnorm(Cmp) < 1.0e-6 );

      /* test right multiplication of square matrix by the transpose of the
         same matrix in-place */
      A = squmatrix_rand ( A, type, 3, 3 ); cu_assert ( A != NULL );
      Am = gan_mat_from_squmat_q ( A, Am ); cu_assert ( Am != NULL );
      AmT = matrix_transpose ( Am, AmT ); cu_assert ( AmT != NULL );
      Cmp = matrix_matrix_mult ( Am, AmT, Cmp ); cu_assert ( Cmp != NULL );
      A = gan_squmat_srmultT_squ_i ( A ); cu_assert ( A != NULL );
      Am = gan_mat_from_squmat_q ( A, Am ); cu_assert ( Am != NULL );
      Cmp = matrix_add ( Cmp, 1.0, Am, -1.0, Cmp ); cu_assert ( Cmp != NULL );
      cu_assert ( gan_mat_Fnorm(Cmp) < 1.0e-6 );

      /* test right multiplication of inverse of square matrix by the
         inverse transpose of the same matrix in-place */
      A = squmatrix_rand ( A, type, 3, 3 ); cu_assert ( A != NULL );
      Am = gan_mat_from_squmat_q ( A, Am ); cu_assert ( Am != NULL );
      AmI = gan_mat_invert_q ( Am, AmI ); cu_assert ( AmI != NULL );
      AmIT = matrix_transpose ( AmI, AmIT ); cu_assert (AmIT != NULL);
      Cmp = matrix_matrix_mult ( AmI, AmIT, Cmp ); cu_assert ( Cmp != NULL );
      A = gan_squmatI_srmultIT_squ_i ( A ); cu_assert ( A != NULL );
      Am = gan_mat_from_squmat_q ( A, Am ); cu_assert ( Am != NULL );
      Cmp = matrix_add ( Cmp, 1.0, Am, -1.0, Cmp ); cu_assert ( Cmp != NULL );
      cu_assert ( gan_mat_Fnorm(Cmp) < 1.0e-6 );

      /* test right multiplication of transpose of square matrix by itself */
      A = squmatrix_rand ( A, type, 3, 3 ); cu_assert ( A != NULL );
      Am = gan_mat_from_squmat_q ( A, Am ); cu_assert ( Am != NULL );
      AmT = matrix_transpose ( Am, AmT ); cu_assert ( AmT != NULL );
      Cmp = matrix_matrix_mult ( AmT, Am, Cmp ); cu_assert ( Cmp != NULL );
      B = gan_squmatT_srmult_squ_q ( A, B ); cu_assert ( B != NULL );
      Bm = gan_mat_from_squmat_q ( B, Bm ); cu_assert ( Bm != NULL );
      Cmp = matrix_add ( Cmp, 1.0, Bm, -1.0, Cmp ); cu_assert ( Cmp != NULL );
      cu_assert ( gan_mat_Fnorm(Cmp) < 1.0e-6 );

      /* test right multiplication of transpose of square matrix by itself
         in-place */
      A = squmatrix_rand ( A, type, 3, 3 ); cu_assert ( A != NULL );
      Am = gan_mat_from_squmat_q ( A, Am ); cu_assert ( Am != NULL );
      AmT = matrix_transpose ( Am, AmT ); cu_assert ( AmT != NULL );
      Cmp = matrix_matrix_mult ( AmT, Am, Cmp ); cu_assert ( Cmp != NULL );
      A = gan_squmatT_srmult_squ_i ( A ); cu_assert ( A != NULL );
      Am = gan_mat_from_squmat_q ( A, Am ); cu_assert ( Am != NULL );
      Cmp = matrix_add ( Cmp, 1.0, Am, -1.0, Cmp ); cu_assert ( Cmp != NULL );
      cu_assert ( gan_mat_Fnorm(Cmp) < 1.0e-6 );

      /* test right multiplication of inverse transpose of square matrix by
         inverse of the same matrix */
      A = squmatrix_rand ( A, type, 3, 3 ); cu_assert ( A != NULL );
      Am = gan_mat_from_squmat_q ( A, Am ); cu_assert ( Am != NULL );
      AmI = gan_mat_invert_q ( Am, AmI ); cu_assert ( AmI != NULL );
      AmIT = matrix_transpose ( AmI, AmIT ); cu_assert (AmIT != NULL);
      Cmp = matrix_matrix_mult ( AmIT, AmI, Cmp ); cu_assert ( Cmp != NULL );
      B = gan_squmatIT_srmultI_squ_q ( A, B ); cu_assert (B != NULL);
      Bm = gan_mat_from_squmat_q ( B, Bm ); cu_assert ( Bm != NULL );
      Cmp = matrix_add ( Cmp, 1.0, Bm, -1.0, Cmp ); cu_assert ( Cmp != NULL );
      cu_assert ( gan_mat_Fnorm(Cmp) < 1.0e-6 );

      /* test right multiplication of inverse transpose of square matrix by
         inverse of the same matrix in-place */
      A = squmatrix_rand ( A, type, 3, 3 ); cu_assert ( A != NULL );
      Am = gan_mat_from_squmat_q ( A, Am ); cu_assert ( Am != NULL );
      AmI = gan_mat_invert_q ( Am, AmI ); cu_assert ( AmI != NULL );
      AmIT = matrix_transpose ( AmI, AmIT ); cu_assert (AmIT != NULL);
      Cmp = matrix_matrix_mult ( AmIT, AmI, Cmp ); cu_assert ( Cmp != NULL );
      A = gan_squmatIT_srmultI_squ_i ( A ); cu_assert ( A != NULL );
      Am = gan_mat_from_squmat_q ( A, Am ); cu_assert ( Am != NULL );
      Cmp = matrix_add ( Cmp, 1.0, Am, -1.0, Cmp ); cu_assert ( Cmp != NULL );
      cu_assert ( gan_mat_Fnorm(Cmp) < 1.0e-6 );
   }

   /* check whether the currently tested type is a symmetric type */
   A = squmatrix_rand ( A, type, 3, 3 ); cu_assert ( A != NULL );
   if ( gan_squmat_symmetric(A) )
   {
      /* test multiplication of symmetric matrix on left by a generic matrix
         on left and on the right by its transpose */
      A = squmatrix_rand ( A, type, 3, 3 ); cu_assert ( A != NULL );
      Bm = matrix_rand ( Bm, 5, 3 ); cu_assert ( Bm != NULL );
      BmT = matrix_transpose ( Bm, BmT ); cu_assert ( BmT != NULL );
      Am = gan_mat_from_squmat_q ( A, Am ); cu_assert ( Am != NULL );
      AmT = matrix_matrix_mult ( Am, BmT, AmT ); cu_assert (AmT != NULL);
      Cmp = matrix_matrix_mult ( Bm, AmT, Cmp ); cu_assert ( Cmp != NULL );
      C = gan_symmat_lrmult_q ( A, Bm, BmT, C ); cu_assert (C != NULL);
      Cm = gan_mat_from_squmat_q ( C, Cm ); cu_assert ( Cm != NULL );
      Cmp = matrix_add ( Cmp, 1.0, Cm, -1.0, Cmp ); cu_assert ( Cmp != NULL );
      cu_assert ( gan_mat_Fnorm(Cmp) < 1.0e-6 );
      AmT = matrix_add ( AmT, 1.0, BmT, -1.0, AmT ); cu_assert ( AmT != NULL );
      cu_assert ( gan_mat_Fnorm(AmT) < 1.0e-6 );

      /* test multiplication of symmetric matrix on left by the transpose of
         a generic matrix on the left and on the right by the matrix */
      A = squmatrix_rand ( A, type, 3, 3 ); cu_assert ( A != NULL );
      Bm = matrix_rand ( Bm, 3, 5 ); cu_assert ( Bm != NULL );
      BmT = matrix_transpose ( Bm, BmT ); cu_assert ( BmT != NULL );
      Am = gan_mat_from_squmat_q ( A, Am ); cu_assert ( Am != NULL );
      AmT = matrix_matrix_mult ( Am, Bm, AmT ); cu_assert (AmT != NULL);
      Cmp = matrix_matrix_mult ( BmT, AmT, Cmp ); cu_assert ( Cmp != NULL );
      C = gan_symmat_lrmultT_q ( A, Bm, BmT, C ); cu_assert (C != NULL);
      Cm = gan_mat_from_squmat_q ( C, Cm ); cu_assert ( Cm != NULL );
      Cmp = matrix_add ( Cmp, 1.0, Cm, -1.0, Cmp ); cu_assert ( Cmp != NULL );
      cu_assert ( gan_mat_Fnorm(Cmp) < 1.0e-6 );
      AmT = matrix_add ( AmT, 1.0, BmT, -1.0, AmT ); cu_assert ( AmT != NULL );
      cu_assert ( gan_mat_Fnorm(AmT) < 1.0e-6 );
   }

   /* test square matrix inversion */
   A = squmatrix_rand ( A, type, 4, 4 ); cu_assert ( A != NULL );
   gan_symmat_fill_va ( A, 4,
                        1.0,
                        1.0, 5.0,
                        0.0, 0.0, 1.0,
                        0.0, 0.0, 1.0, 5.0 );
   Am = gan_mat_from_squmat_q ( A, Am ); cu_assert ( Am != NULL );
   B = gan_squmat_invert_q ( A, B ); cu_assert ( B != NULL );
   Bm = gan_mat_from_squmat_q ( B, Bm ); cu_assert ( Bm != NULL );
   Cm = matrix_matrix_mult ( Am, Bm, Cm ); cu_assert ( Cm != NULL );
   Cmp = matrix_ident ( Cmp, Cm->rows ); cu_assert ( Cmp != NULL );
   Cmp = matrix_add ( Cmp, 1.0, Cm, -1.0, Cmp ); cu_assert (Cmp != NULL);
   cu_assert ( gan_mat_Fnorm(Cmp) < 1.0e-6 );

   if ( type != GAN_SYMMETRIC_MATRIX )
   {
      /* test square matrix inversion in-place */
      A = squmatrix_rand ( A, type, 4, 4 ); cu_assert ( A != NULL );
      Am = gan_mat_from_squmat_q ( A, Am ); cu_assert ( Am != NULL );
      A = gan_squmat_invert_i ( A ); cu_assert ( A != NULL );
      Bm = gan_mat_from_squmat_q ( A, Bm ); cu_assert ( Bm != NULL );
      Cm = matrix_matrix_mult ( Am, Bm, Cm ); cu_assert ( Cm != NULL );
      Cmp = matrix_ident ( Cmp, Cm->rows ); cu_assert ( Cmp != NULL );
      Cmp = matrix_add ( Cmp, 1.0, Cm, -1.0, Cmp ); cu_assert ( Cmp != NULL );
      cu_assert ( gan_mat_Fnorm(Cmp) < 1.0e-6 );
   }
   
   /* check whether the currently tested type is a symmetric type */
   A = squmatrix_rand ( A, type, 3, 3 ); cu_assert ( A != NULL );
   if ( gan_squmat_symmetric(A) )
   {
      /* test square matrix Cholesky factorisation */
      A = squmatrix_rand ( A, type, 4, 4 ); cu_assert ( A != NULL );
      Am = gan_mat_from_squmat_q ( A, Am ); cu_assert ( Am != NULL );
      B = gan_squmat_cholesky_q ( A, B ); cu_assert ( B != NULL );
      Bm = gan_mat_from_squmat_q ( B, Bm ); cu_assert ( Bm != NULL );
      BmT = matrix_transpose ( Bm, BmT ); cu_assert ( BmT != NULL );
      Cmp = matrix_matrix_mult ( Bm, BmT, Cmp ); cu_assert (Cmp != NULL);
      Cmp = matrix_add ( Cmp, 1.0, Am, -1.0, Cmp ); cu_assert ( Cmp != NULL );
      cu_assert ( gan_mat_Fnorm(Cmp) < 1.0e-6 );

      /* test square matrix Cholesky factorisation in-place */
      A = squmatrix_rand ( A, type, 4, 4 ); cu_assert ( A != NULL );
      Am = gan_mat_from_squmat_q ( A, Am ); cu_assert ( Am != NULL );
      A = gan_squmat_cholesky_i ( A ); cu_assert ( A != NULL );
      Bm = gan_mat_from_squmat_q ( A, Bm ); cu_assert ( Bm != NULL );
      BmT = matrix_transpose ( Bm, BmT ); cu_assert ( BmT != NULL );
      Cmp = matrix_matrix_mult ( Bm, BmT, Cmp ); cu_assert (Cmp != NULL);
      Cmp = matrix_add ( Cmp, 1.0, Am, -1.0, Cmp ); cu_assert ( Cmp != NULL );
      cu_assert ( gan_mat_Fnorm(Cmp) < 1.0e-6 );
   }

   /* free matrices & vectors */
   gan_vec_free_va ( bp, b, a, NULL );
   gan_mat_free_va ( Cmp, Cm, BmT, Bm, AmIT, AmI, AmT, Am, NULL );
   gan_squmat_free_va ( Cp, C, B, A, NULL );

   /* return with success */
   return GAN_TRUE;
}

/* Tests for general size single precision square matrices. We apply an
 * operation both using Gandalf functions and simplified functions defined in
 * this test module.
 */
static Gan_Bool test_matf_square ( Gan_SquMatrixType type )
{
   Gan_SquMatrix_f *A, *B, *C, *Cp;
   Gan_Matrix_f *Am, *AmT, *AmI, *AmIT, *Bm, *BmT, *Cm, *Cmp;
   Gan_Vector_f *a, *b, *bp;

   /* allocate matrices and vectors used in tests */
   A    = gan_squmatf_alloc ( type, 0 ); cu_assert ( A  != NULL );
   B    = gan_squmatf_alloc ( type, 0 ); cu_assert ( B  != NULL );
   C    = gan_squmatf_alloc ( type, 0 ); cu_assert ( C  != NULL );
   Cp   = gan_squmatf_alloc ( type, 0 ); cu_assert ( Cp != NULL );
   Am   = gan_matf_alloc(0,0);
   AmT  = gan_matf_alloc(0,0);
   AmI  = gan_matf_alloc(0,0);
   AmIT = gan_matf_alloc(0,0);
   Bm   = gan_matf_alloc(0,0);
   BmT  = gan_matf_alloc(0,0);
   Cm   = gan_matf_alloc(0,0);
   Cmp  = gan_matf_alloc(0,0);
   a    = gan_vecf_alloc(0); cu_assert ( a  != NULL );
   b    = gan_vecf_alloc(0); cu_assert ( b  != NULL );
   bp   = gan_vecf_alloc(0); cu_assert ( bp != NULL );

   if ( type != GAN_SYMMETRIC_MATRIX )
   {
      /* test right multiplication of inverse of square matrix by the inverse
         transpose of the same matrix */
      A = squmatrixf_rand ( A, type, 3, 3 ); cu_assert ( A != NULL );
      Am = gan_matf_from_squmatf_q ( A, Am ); cu_assert ( Am != NULL );
      AmI = gan_matf_invert_q ( Am, AmI ); cu_assert ( AmI != NULL );
      AmIT = matrixf_transpose ( AmI, AmIT ); cu_assert (AmIT != NULL);
      Cmp = matrixf_matrixf_mult ( AmI, AmIT, Cmp ); cu_assert ( Cmp != NULL );
      B = gan_squmatIf_srmultIT_squ_q ( A, B ); cu_assert ( B != NULL );
      Bm = gan_matf_from_squmatf_q ( B, Bm ); cu_assert ( Bm != NULL );
      Cmp = matrixf_add ( Cmp, 1.0F, Bm, -1.0F, Cmp );
      cu_assert ( Cmp != NULL );
      cu_assert ( gan_matf_Fnorm(Cmp) < 1.0e-4F );
   }
   
   /* test square matrix copy */
   A = squmatrixf_rand ( A, type, 4, 4 ); cu_assert ( A != NULL );
   Cp = squmatrixf_add ( A, 1.0F, NULL, 0.0F, Cp ); cu_assert (Cp != NULL);
   C = gan_squmatf_copy_q ( A, C ); cu_assert ( C != NULL );
   Cp = squmatrixf_add ( Cp, 1.0F, C, -1.0F, Cp ); cu_assert (Cp != NULL);
   cu_assert ( gan_squmatf_Fnorm(Cp) < 1.0e-4F );

   /* test square matrix multiply by scalar */
   A = squmatrixf_rand ( A, type, 4, 4 ); cu_assert ( A != NULL );
   Cp = squmatrixf_add ( A, 4.5F, NULL, 0.0F, Cp ); cu_assert (Cp != NULL);
   C = gan_squmatf_scale_q ( A, 4.5F, C ); cu_assert ( C != NULL );
   Cp = squmatrixf_add ( Cp, 1.0F, C, -1.0F, Cp ); cu_assert (Cp != NULL);
   cu_assert ( gan_squmatf_Fnorm(Cp) < 1.0e-4F );

   /* test square matrix multiply by scalar in-place */
   A = squmatrixf_rand ( A, type,4, 4 ); cu_assert ( A != NULL );
   Cp = squmatrixf_add ( A, 4.5F, NULL, 0.0F, Cp ); cu_assert (Cp != NULL);
   A = gan_squmatf_scale_i ( A, 4.5F ); cu_assert ( A != NULL );
   Cp = squmatrixf_add ( Cp, 1.0F, A, -1.0F, Cp ); cu_assert (Cp != NULL);
   cu_assert ( gan_squmatf_Fnorm(Cp) < 1.0e-4F );

   /* test square matrix divide by scalar */
   A = squmatrixf_rand ( A, type, 4, 4 ); cu_assert ( A != NULL );
   Cp = squmatrixf_add ( A,1.0F/4.5F, NULL,0.0F, Cp); cu_assert (Cp != NULL);
   C = gan_squmatf_divide_q ( A, 4.5F, C ); cu_assert ( C != NULL );
   Cp = squmatrixf_add ( Cp, 1.0F, C, -1.0F, Cp ); cu_assert (Cp != NULL);
   cu_assert ( gan_squmatf_Fnorm(Cp) < 1.0e-4F );

   /* test square matrix divide by scalar in-place */
   A = squmatrixf_rand ( A, type, 4, 4 ); cu_assert ( A != NULL );
   Cp = squmatrixf_add ( A,1.0F/4.5F, NULL,0.0F, Cp); cu_assert (Cp != NULL);
   A = gan_squmatf_divide_i ( A, 4.5F ); cu_assert ( A != NULL );
   Cp = squmatrixf_add ( Cp, 1.0F, A, -1.0F, Cp ); cu_assert (Cp != NULL);
   cu_assert ( gan_squmatf_Fnorm(Cp) < 1.0e-4F );

   /* test square matrix negation */
   A = squmatrixf_rand ( A, type, 4, 4 ); cu_assert ( A != NULL );
   Cp = squmatrixf_add ( A, -1.0F, NULL, 0.0F, Cp ); cu_assert (Cp != NULL);
   C = gan_squmatf_negate_q ( A, C ); cu_assert ( C != NULL );
   Cp = squmatrixf_add ( Cp, 1.0F, C, -1.0F, Cp ); cu_assert (Cp != NULL);
   cu_assert ( gan_squmatf_Fnorm(Cp) < 1.0e-4F );

   /* test square matrix negation in-place */
   A = squmatrixf_rand ( A, type, 4, 4 ); cu_assert ( A != NULL );
   Cp = squmatrixf_add ( A, -1.0F, NULL, 0.0F, Cp ); cu_assert (Cp != NULL);
   A = gan_squmatf_negate_i(A); cu_assert ( A != NULL );
   Cp = squmatrixf_add ( Cp, 1.0F, A, -1.0F, Cp ); cu_assert (Cp != NULL);
   cu_assert ( gan_squmatf_Fnorm(Cp) < 1.0e-4F );

   /* test square matrix addition */
   A = squmatrixf_rand ( A, type, 4, 4 ); cu_assert ( A != NULL );
   B = squmatrixf_rand ( B, type, 4, 4 ); cu_assert ( B != NULL );
   Cp = squmatrixf_add ( A, 1.0F, B, 1.0F, Cp ); cu_assert ( Cp != NULL );
   C = gan_squmatf_add_q ( A, B, C ); cu_assert ( C != NULL );
   Cp = squmatrixf_add ( Cp, 1.0F, C, -1.0F, Cp ); cu_assert (Cp != NULL);
   cu_assert ( gan_squmatf_Fnorm(Cp) < 1.0e-4F );
   
   /* test in-place square matrix addition in first argument */
   A = squmatrixf_rand ( A, type, 4, 4 ); cu_assert ( A != NULL );
   B = squmatrixf_rand ( B, type, 4, 4 ); cu_assert ( B != NULL );
   Cp = squmatrixf_add ( A, 1.0F, B, 1.0F, Cp ); cu_assert ( Cp != NULL );
   A = gan_squmatf_add_i1 ( A, B ); cu_assert ( A != NULL );
   Cp = squmatrixf_add ( Cp, 1.0F, A, -1.0F, Cp ); cu_assert (Cp != NULL);
   cu_assert ( gan_squmatf_Fnorm(Cp) < 1.0e-4F );
   
   /* test in-place square matrix addition in second argument */
   A = squmatrixf_rand ( A, type, 4, 4 ); cu_assert ( A != NULL );
   B = squmatrixf_rand ( B, type, 4, 4 ); cu_assert ( B != NULL );
   Cp = squmatrixf_add ( A, 1.0F, B, 1.0F, Cp ); cu_assert ( Cp != NULL );
   B = gan_squmatf_add_i2 ( A, B ); cu_assert ( B != NULL );
   Cp = squmatrixf_add ( Cp, 1.0F, B, -1.0F, Cp ); cu_assert (Cp != NULL);
   cu_assert ( gan_squmatf_Fnorm(Cp) < 1.0e-4F );
   
   /* test square matrix subtraction */
   A = squmatrixf_rand ( A, type, 4, 4 ); cu_assert ( A != NULL );
   B = squmatrixf_rand ( B, type, 4, 4 ); cu_assert ( B != NULL );
   Cp = squmatrixf_add ( A, 1.0F, B, -1.0F, Cp ); cu_assert (Cp != NULL);
   C = gan_squmatf_sub_q ( A, B, C ); cu_assert ( C != NULL );
   Cp = squmatrixf_add ( Cp, 1.0F, C, -1.0F, Cp ); cu_assert (Cp != NULL);
   cu_assert ( gan_squmatf_Fnorm(Cp) < 1.0e-4F );

   /* test in-place square matrix subtraction in first argument */
   A = squmatrixf_rand ( A, type, 4, 4 ); cu_assert ( A != NULL );
   B = squmatrixf_rand ( B, type, 4, 4 ); cu_assert ( B != NULL );
   Cp = squmatrixf_add ( A, 1.0F, B, -1.0F, Cp ); cu_assert (Cp != NULL);
   A = gan_squmatf_sub_i1 ( A, B ); cu_assert ( A != NULL );
   Cp = squmatrixf_add ( Cp, 1.0F, A, -1.0F, Cp ); cu_assert (Cp != NULL);
   cu_assert ( gan_squmatf_Fnorm(Cp) < 1.0e-4F );
   
   /* test in-place square matrix subtraction in second argument */
   A = squmatrixf_rand ( A, type, 4, 4 ); cu_assert ( A != NULL );
   B = squmatrixf_rand ( B, type, 4, 4 ); cu_assert ( B != NULL );
   Cp = squmatrixf_add ( A, 1.0F, B, -1.0F, Cp ); cu_assert (Cp != NULL);
   B = gan_squmatf_sub_i2 ( A, B ); cu_assert ( B != NULL );
   Cp = squmatrixf_add ( Cp, 1.0F, B, -1.0F, Cp ); cu_assert (Cp != NULL);
   cu_assert ( gan_squmatf_Fnorm(Cp) < 1.0e-4F );

   /* test square matrix/vector multiplication */
   A = squmatrixf_rand ( A, type, 4, 4 ); cu_assert ( A != NULL );
   Am = gan_matf_from_squmatf_q ( A, Am ); cu_assert ( Am != NULL );
   a = vectorf_rand ( a, 4 ); cu_assert ( a != NULL );
   bp = matrixf_vectorf_mult ( Am, a, bp ); cu_assert ( bp != NULL );
   b = gan_squmatf_multv_q ( A, a, b ); cu_assert ( b != NULL );
   bp = vectorf_add ( bp, 1.0F, b, -1.0F, bp ); cu_assert ( bp != NULL );
   cu_assert ( gan_vecf_sqrlen(bp) < 1.0e-4F );

   /* test square matrix/vector multiplication with matrix transposed */
   A = squmatrixf_rand ( A, type, 4, 4 ); cu_assert ( A != NULL );
   Am = gan_matf_from_squmatf_q ( A, Am ); cu_assert ( Am != NULL );
   AmT = matrixf_transpose ( Am, AmT ); cu_assert ( AmT != NULL );
   a = vectorf_rand ( a, 4 ); cu_assert ( a != NULL );
   bp = matrixf_vectorf_mult ( AmT, a, bp ); cu_assert ( bp != NULL );
   b = gan_squmatTf_multv_q ( A, a, b ); cu_assert ( b != NULL );
   bp = vectorf_add ( bp, 1.0F, b, -1.0F, bp ); cu_assert ( bp != NULL );
   cu_assert ( gan_vecf_sqrlen(bp) < 1.0e-4F );

   if ( type != GAN_SYMMETRIC_MATRIX )
   {
      /* test square matrix/vector multiplication in-place */
      A = squmatrixf_rand ( A, type, 4, 4 ); cu_assert ( A != NULL );
      Am = gan_matf_from_squmatf_q ( A, Am ); cu_assert ( Am != NULL );
      a = vectorf_rand ( a, 4 ); cu_assert ( a != NULL );
      bp = matrixf_vectorf_mult ( Am, a, bp ); cu_assert ( bp != NULL );
      a = gan_squmatf_multv_i ( A, a ); cu_assert ( a != NULL );
      bp = vectorf_add ( bp, 1.0F, a, -1.0F, bp ); cu_assert (bp != NULL);
      cu_assert ( gan_vecf_sqrlen(bp) < 1.0e-4F );

      /* test square matrix/vector multiplication with matrix transposed
         in-place */
      A = squmatrixf_rand ( A, type, 4, 4 ); cu_assert ( A != NULL );
      Am = gan_matf_from_squmatf_q ( A, Am ); cu_assert ( Am != NULL );
      AmT = matrixf_transpose ( Am, AmT ); cu_assert ( AmT != NULL );
      a = vectorf_rand ( a, 4 ); cu_assert ( a != NULL );
      bp = matrixf_vectorf_mult ( AmT, a, bp ); cu_assert ( bp != NULL );
      a = gan_squmatTf_multv_i ( A, a ); cu_assert ( a != NULL );
      bp = vectorf_add ( bp, 1.0F, a, -1.0F, bp ); cu_assert (bp != NULL);
      cu_assert ( gan_vecf_sqrlen(bp) < 1.0e-4F );

      /* test square matrix/vector multiplication with matrix inverted */
      A = squmatrixf_rand ( A, type, 4, 4 ); cu_assert ( A != NULL );
      Am = gan_matf_from_squmatf_q ( A, Am ); cu_assert ( Am != NULL );
      AmI = gan_matf_invert_q ( Am, AmI ); cu_assert ( AmI != NULL );
      a = vectorf_rand ( a, 4 ); cu_assert ( a != NULL );
      bp = matrixf_vectorf_mult ( AmI, a, bp ); cu_assert ( bp != NULL );
      b = gan_squmatIf_multv_q ( A, a, b ); cu_assert ( b != NULL );
      bp = vectorf_add ( bp, 1.0F, b, -1.0F, bp ); cu_assert (bp != NULL);
      cu_assert ( gan_vecf_sqrlen(bp) < 1.0e-4F );

      /* test square matrix/vector multiplication with matrix inverted
         in-place */
      A = squmatrixf_rand ( A, type, 4, 4 ); cu_assert ( A != NULL );
      Am = gan_matf_from_squmatf_q ( A, Am ); cu_assert ( Am != NULL );
      AmI = gan_matf_invert_q ( Am, AmI ); cu_assert ( AmI != NULL );
      a = vectorf_rand ( a, 4 ); cu_assert ( a != NULL );
      bp = matrixf_vectorf_mult ( AmI, a, bp ); cu_assert ( bp != NULL );
      a = gan_squmatIf_multv_i ( A, a ); cu_assert ( a != NULL );
      bp = vectorf_add ( bp, 1.0F, a, -1.0F, bp ); cu_assert (bp != NULL);
      cu_assert ( gan_vecf_sqrlen(bp) < 1.0e-4F );

      /* test square matrix/vector multiplication with matrix inverted &
         transposed */
      A = squmatrixf_rand ( A, type, 4, 4 ); cu_assert ( A != NULL );
      Am = gan_matf_from_squmatf_q ( A, Am ); cu_assert ( Am != NULL );
      AmI = gan_matf_invert_q ( Am, AmI ); cu_assert ( AmI != NULL );
      AmIT = matrixf_transpose ( AmI, AmIT ); cu_assert (AmIT != NULL);
      a = vectorf_rand ( a, 4 ); cu_assert ( a != NULL );
      bp = matrixf_vectorf_mult ( AmIT, a, bp ); cu_assert (bp != NULL);
      b = gan_squmatITf_multv_q ( A, a, b ); cu_assert ( b != NULL );
      bp = vectorf_add ( bp, 1.0F, b, -1.0F, bp ); cu_assert (bp != NULL);
      cu_assert ( gan_vecf_sqrlen(bp) < 1.0e-4F );

      /* test square matrix/vector multiplication with matrix inverted and
         transposed in-place */
      A = squmatrixf_rand ( A, type, 4, 4 ); cu_assert ( A != NULL );
      Am = gan_matf_from_squmatf_q ( A, Am ); cu_assert ( Am != NULL );
      AmI = gan_matf_invert_q ( Am, AmI ); cu_assert ( AmI != NULL );
      AmIT = matrixf_transpose ( AmI, AmIT ); cu_assert (AmIT != NULL);
      a = vectorf_rand ( a, 4 ); cu_assert ( a != NULL );
      bp = matrixf_vectorf_mult ( AmIT, a, bp ); cu_assert (bp != NULL);
      a = gan_squmatITf_multv_i ( A, a ); cu_assert ( a != NULL );
      bp = vectorf_add ( bp, 1.0F, a, -1.0F, bp ); cu_assert (bp != NULL);
      cu_assert ( gan_vecf_sqrlen(bp) < 1.0e-4F );
   }

   /* test right multiplication of square matrix by generic matrix */
   A = squmatrixf_rand ( A, type, 3, 3 ); cu_assert ( A != NULL );
   Am = gan_matf_from_squmatf_q ( A, Am ); cu_assert ( Am != NULL );
   Bm = matrixf_rand ( Bm, 3, 5 ); cu_assert ( Bm != NULL );
   Cmp = matrixf_matrixf_mult ( Am, Bm, Cmp ); cu_assert ( Cmp != NULL );
   Cm = gan_squmatf_rmult_q ( A, Bm, Cm ); cu_assert ( Cm != NULL );
   Cmp = matrixf_add ( Cmp, 1.0F, Cm, -1.0F, Cmp ); cu_assert (Cmp != NULL);
   cu_assert ( gan_matf_Fnorm(Cmp) < 1.0e-4F );

   /* test right multiplication of square matrix by transpose of generic
      matrix */
   A = squmatrixf_rand ( A, type, 3, 3 ); cu_assert ( A != NULL );
   Am = gan_matf_from_squmatf_q ( A, Am ); cu_assert ( Am != NULL );
   Bm = matrixf_rand ( Bm, 5, 3 ); cu_assert ( Bm != NULL );
   BmT = matrixf_transpose ( Bm, BmT ); cu_assert ( BmT != NULL );
   Cmp = matrixf_matrixf_mult ( Am, BmT, Cmp ); cu_assert (Cmp != NULL);
   Cm = gan_squmatf_rmultT_q ( A, Bm, Cm ); cu_assert ( Cm != NULL );
   Cmp = matrixf_add ( Cmp, 1.0F, Cm, -1.0F, Cmp ); cu_assert (Cmp != NULL);
   cu_assert ( gan_matf_Fnorm(Cmp) < 1.0e-4F );

   /* test right multiplication of transpose of square matrix by generic
      matrix */
   A = squmatrixf_rand ( A, type, 3, 3 ); cu_assert ( A != NULL );
   Am = gan_matf_from_squmatf_q ( A, Am ); cu_assert ( Am != NULL );
   AmT = matrixf_transpose ( Am, AmT ); cu_assert ( AmT != NULL );
   Bm = matrixf_rand ( Bm, 3, 5 ); cu_assert ( Bm != NULL );
   Cmp = matrixf_matrixf_mult ( AmT, Bm, Cmp ); cu_assert (Cmp != NULL);
   Cm = gan_squmatTf_rmult_q ( A, Bm, Cm ); cu_assert ( Cm != NULL );
   Cmp = matrixf_add ( Cmp, 1.0F, Cm, -1.0F, Cmp ); cu_assert (Cmp != NULL);
   cu_assert ( gan_matf_Fnorm(Cmp) < 1.0e-4F );

   /* test right multiplication of transpose of square matrix by transpose of
      generic matrix */
   A = squmatrixf_rand ( A, type, 3, 3 ); cu_assert ( A != NULL );
   Am = gan_matf_from_squmatf_q ( A, Am ); cu_assert ( Am != NULL );
   AmT = matrixf_transpose ( Am, AmT ); cu_assert ( AmT != NULL );
   Bm = matrixf_rand ( Bm, 5, 3 ); cu_assert ( Bm != NULL );
   BmT = matrixf_transpose ( Bm, BmT ); cu_assert ( BmT != NULL );
   Cmp = matrixf_matrixf_mult ( AmT, BmT, Cmp ); cu_assert (Cmp != NULL);
   Cm = gan_squmatTf_rmultT_q ( A, Bm, Cm ); cu_assert ( Cm != NULL );
   Cmp = matrixf_add ( Cmp, 1.0F, Cm, -1.0F, Cmp ); cu_assert (Cmp != NULL);
   cu_assert ( gan_matf_Fnorm(Cmp) < 1.0e-4F );

   if ( type != GAN_SYMMETRIC_MATRIX )
   {
      /* test right multiplication of square matrix by generic matrix
         in-place */
      A = squmatrixf_rand ( A, type, 3, 3 ); cu_assert ( A != NULL );
      Am = gan_matf_from_squmatf_q ( A, Am ); cu_assert ( Am != NULL );
      Bm = matrixf_rand ( Bm, 3, 5 ); cu_assert ( Bm != NULL );
      Cmp = matrixf_matrixf_mult ( Am, Bm, Cmp ); cu_assert (Cmp != NULL);
      Bm = gan_squmatf_rmult_i ( A, Bm ); cu_assert ( Bm != NULL );
      Cmp = matrixf_add ( Cmp, 1.0F, Bm, -1.0F, Cmp );
      cu_assert ( Cmp != NULL );
      cu_assert ( gan_matf_Fnorm(Cmp) < 1.0e-4F );

      /* test right multiplication of transpose of square matrix by generic
         matrix in-place */
      A = squmatrixf_rand ( A, type, 3, 3 ); cu_assert ( A != NULL );
      Am = gan_matf_from_squmatf_q ( A, Am ); cu_assert ( Am != NULL );
      AmT = matrixf_transpose ( Am, AmT ); cu_assert ( AmT != NULL );
      Bm = matrixf_rand ( Bm, 3, 5 ); cu_assert ( Bm != NULL );
      Cmp = matrixf_matrixf_mult ( AmT, Bm, Cmp ); cu_assert (Cmp != NULL);
      Bm = gan_squmatTf_rmult_i ( A, Bm ); cu_assert ( Bm != NULL );
      Cmp = matrixf_add ( Cmp, 1.0F, Bm, -1.0F, Cmp );
      cu_assert ( Cmp != NULL );
      cu_assert ( gan_matf_Fnorm(Cmp) < 1.0e-4F );

      /* test right multiplication of inverse of square matrix by generic
         matrix */
      A = squmatrixf_rand ( A, type, 3, 3 ); cu_assert ( A != NULL );
      Am = gan_matf_from_squmatf_q ( A, Am ); cu_assert ( Am != NULL );
      AmI = gan_matf_invert_q ( Am, AmI ); cu_assert ( AmI != NULL );
      Bm = matrixf_rand ( Bm, 3, 5 ); cu_assert ( Bm != NULL );
      Cmp = matrixf_matrixf_mult ( AmI, Bm, Cmp ); cu_assert (Cmp != NULL);
      Cm = gan_squmatIf_rmult_q ( A, Bm, Cm ); cu_assert ( Cm != NULL );
      Cmp = matrixf_add ( Cmp, 1.0F, Cm, -1.0F, Cmp );
      cu_assert ( Cmp != NULL );
      cu_assert ( gan_matf_Fnorm(Cmp) < 1.0e-4F );

      /* test right multiplication of inverse of square matrix by generic
         matrix in-place */
      A = squmatrixf_rand ( A, type, 3, 3 ); cu_assert ( A != NULL );
      Am = gan_matf_from_squmatf_q ( A, Am ); cu_assert ( Am != NULL );
      AmI = gan_matf_invert_q ( Am, AmI ); cu_assert ( AmI != NULL );
      Bm = matrixf_rand ( Bm, 3, 5 ); cu_assert ( Bm != NULL );
      Cmp = matrixf_matrixf_mult ( AmI, Bm, Cmp ); cu_assert (Cmp != NULL);
      Bm = gan_squmatIf_rmult_i ( A, Bm ); cu_assert ( Bm != NULL );
      Cmp = matrixf_add ( Cmp, 1.0F, Bm, -1.0F, Cmp );
      cu_assert ( Cmp != NULL );
      cu_assert ( gan_matf_Fnorm(Cmp) < 1.0e-4F );

      /* test right multiplication of inverse of square matrix by transpose of
         generic matrix */
      A = squmatrixf_rand ( A, type, 3, 3 ); cu_assert ( A != NULL );
      Am = gan_matf_from_squmatf_q ( A, Am ); cu_assert ( Am != NULL );
      AmI = gan_matf_invert_q ( Am, AmI ); cu_assert ( AmI != NULL );
      Bm = matrixf_rand ( Bm, 5, 3 ); cu_assert ( Bm != NULL );
      BmT = matrixf_transpose ( Bm, BmT ); cu_assert ( BmT != NULL );
      Cmp = matrixf_matrixf_mult ( AmI, BmT, Cmp );cu_assert (Cmp != NULL);
      Cm = gan_squmatIf_rmultT_q ( A, Bm, Cm ); cu_assert (Cm != NULL);
      Cmp = matrixf_add ( Cmp, 1.0F, Cm, -1.0F, Cmp );
      cu_assert ( Cmp != NULL );
      cu_assert ( gan_matf_Fnorm(Cmp) < 1.0e-4F );

      /* test right multiplication of inverse transpose of square matrix by
         generic matrix */
      A = squmatrixf_rand ( A, type, 3, 3 ); cu_assert ( A != NULL );
      Am = gan_matf_from_squmatf_q ( A, Am ); cu_assert ( Am != NULL );
      AmI = gan_matf_invert_q ( Am, AmI ); cu_assert ( AmI != NULL );
      AmIT = matrixf_transpose ( AmI, AmIT ); cu_assert (AmIT != NULL);
      Bm = matrixf_rand ( Bm, 3, 5 ); cu_assert ( Bm != NULL );
      Cmp = matrixf_matrixf_mult ( AmIT, Bm, Cmp );cu_assert (Cmp != NULL);
      Cm = gan_squmatITf_rmult_q ( A, Bm, Cm ); cu_assert (Cm != NULL);
      Cmp = matrixf_add ( Cmp, 1.0F, Cm, -1.0F, Cmp );
      cu_assert ( Cmp != NULL );
      cu_assert ( gan_matf_Fnorm(Cmp) < 1.0e-4F );

      /* test right multiplication of inverse transpose of square matrix by
         generic matrix in-place */
      A = squmatrixf_rand ( A, type, 3, 3 ); cu_assert ( A != NULL );
      Am = gan_matf_from_squmatf_q ( A, Am ); cu_assert ( Am != NULL );
      AmI = gan_matf_invert_q ( Am, AmI ); cu_assert ( AmI != NULL );
      AmIT = matrixf_transpose ( AmI, AmIT ); cu_assert (AmIT != NULL);
      Bm = matrixf_rand ( Bm, 3, 5 ); cu_assert ( Bm != NULL );
      Cmp = matrixf_matrixf_mult ( AmIT, Bm, Cmp );cu_assert (Cmp != NULL);
      Bm = gan_squmatITf_rmult_i ( A, Bm ); cu_assert ( Bm != NULL );
      Cmp = matrixf_add ( Cmp, 1.0F, Bm, -1.0F, Cmp );
      cu_assert ( Cmp != NULL );
      cu_assert ( gan_matf_Fnorm(Cmp) < 1.0e-4F );

      /* test right multiplication of inverse transpose of square matrix by
         transpose of generic matrix */
      A = squmatrixf_rand ( A, type, 3, 3 ); cu_assert ( A != NULL );
      Am = gan_matf_from_squmatf_q ( A, Am ); cu_assert ( Am != NULL );
      AmI = gan_matf_invert_q ( Am, AmI ); cu_assert ( AmI != NULL );
      AmIT = matrixf_transpose ( AmI, AmIT ); cu_assert (AmIT != NULL);
      Bm = matrixf_rand ( Bm, 5, 3 ); cu_assert ( Bm != NULL );
      BmT = matrixf_transpose ( Bm, BmT ); cu_assert ( BmT != NULL );
      Cmp = matrixf_matrixf_mult ( AmIT, BmT, Cmp ); cu_assert ( Cmp != NULL );
      Cm = gan_squmatITf_rmultT_q ( A, Bm, Cm ); cu_assert (Cm != NULL);
      Cmp = matrixf_add ( Cmp, 1.0F, Cm, -1.0F, Cmp );
      cu_assert ( Cmp != NULL );
      cu_assert ( gan_matf_Fnorm(Cmp) < 1.0e-4F );
   }

   /* test left multiplication of square matrix by generic matrix */
   A = squmatrixf_rand ( A, type, 3, 3 ); cu_assert ( A != NULL );
   Am = gan_matf_from_squmatf_q ( A, Am ); cu_assert ( Am != NULL );
   Bm = matrixf_rand ( Bm, 5, 3 ); cu_assert ( Bm != NULL );
   Cmp = matrixf_matrixf_mult ( Bm, Am, Cmp ); cu_assert ( Cmp != NULL );
   Cm = gan_squmatf_lmult_q ( A, Bm, Cm ); cu_assert ( Cm != NULL );
   Cmp = matrixf_add ( Cmp, 1.0F, Cm, -1.0F, Cmp ); cu_assert (Cmp != NULL);
   cu_assert ( gan_matf_Fnorm(Cmp) < 1.0e-4F );

   /* test left multiplication of square matrix by transpose of generic
      matrix */
   A = squmatrixf_rand ( A, type, 3, 3 ); cu_assert ( A != NULL );
   Am = gan_matf_from_squmatf_q ( A, Am ); cu_assert ( Am != NULL );
   Bm = matrixf_rand ( Bm, 3, 5 ); cu_assert ( Bm != NULL );
   BmT = matrixf_transpose ( Bm, BmT ); cu_assert ( BmT != NULL );
   Cmp = matrixf_matrixf_mult ( BmT, Am, Cmp ); cu_assert (Cmp != NULL);
   Cm = gan_squmatf_lmultT_q ( A, Bm, Cm ); cu_assert ( Cm != NULL );
   Cmp = matrixf_add ( Cmp, 1.0F, Cm, -1.0F, Cmp ); cu_assert (Cmp != NULL);
   cu_assert ( gan_matf_Fnorm(Cmp) < 1.0e-4F );

   /* test left multiplication of transpose of square matrix by generic
      matrix */
   A = squmatrixf_rand ( A, type, 3, 3 ); cu_assert ( A != NULL );
   Am = gan_matf_from_squmatf_q ( A, Am ); cu_assert ( Am != NULL );
   AmT = matrixf_transpose ( Am, AmT ); cu_assert ( AmT != NULL );
   Bm = matrixf_rand ( Bm, 5, 3 ); cu_assert ( Bm != NULL );
   Cmp = matrixf_matrixf_mult ( Bm, AmT, Cmp ); cu_assert (Cmp != NULL);
   Cm = gan_squmatTf_lmult_q ( A, Bm, Cm ); cu_assert ( Cm != NULL );
   Cmp = matrixf_add ( Cmp, 1.0F, Cm, -1.0F, Cmp ); cu_assert (Cmp != NULL);
   cu_assert ( gan_matf_Fnorm(Cmp) < 1.0e-4F );

   /* test left multiplication of transpose of square matrix by transpose of
      generic matrix */
   A = squmatrixf_rand ( A, type, 1, 1 ); cu_assert ( A != NULL );
   Am = gan_matf_from_squmatf_q ( A, Am ); cu_assert ( Am != NULL );
   AmT = matrixf_transpose ( Am, AmT ); cu_assert ( AmT != NULL );
   Bm = matrixf_rand ( Bm, 1, 1 ); cu_assert ( Bm != NULL );
   BmT = matrixf_transpose ( Bm, BmT ); cu_assert ( BmT != NULL );
   Cmp = matrixf_matrixf_mult ( BmT, AmT, Cmp ); cu_assert (Cmp != NULL);
   Cm = gan_squmatTf_lmultT_q ( A, Bm, Cm ); cu_assert ( Cm != NULL );
   Cmp = matrixf_add ( Cmp, 1.0F, Cm, -1.0F, Cmp ); cu_assert (Cmp != NULL);
   cu_assert ( gan_matf_Fnorm(Cmp) < 1.0e-4F );

   if ( type != GAN_SYMMETRIC_MATRIX )
   {
      /* test left multiplication of square matrix by generic matrix
         in-place */
      A = squmatrixf_rand ( A, type, 3, 3 ); cu_assert ( A != NULL );
      Am = gan_matf_from_squmatf_q ( A, Am ); cu_assert ( Am != NULL );
      Bm = matrixf_rand ( Bm, 5, 3 ); cu_assert ( Bm != NULL );
      Cmp = matrixf_matrixf_mult ( Bm, Am, Cmp ); cu_assert (Cmp != NULL);
      Bm = gan_squmatf_lmult_i ( A, Bm ); cu_assert ( Bm != NULL );
      Cmp = matrixf_add ( Cmp, 1.0F, Bm, -1.0F, Cmp );
      cu_assert ( Cmp != NULL );
      cu_assert ( gan_matf_Fnorm(Cmp) < 1.0e-4F );

      /* test left multiplication of transpose of square matrix by generic
         matrix in-place */
      A = squmatrixf_rand ( A, type, 3, 3 ); cu_assert ( A != NULL );
      Am = gan_matf_from_squmatf_q ( A, Am ); cu_assert ( Am != NULL );
      AmT = matrixf_transpose ( Am, AmT ); cu_assert ( AmT != NULL );
      Bm = matrixf_rand ( Bm, 5, 3 ); cu_assert ( Bm != NULL );
      Cmp = matrixf_matrixf_mult ( Bm, AmT, Cmp ); cu_assert (Cmp != NULL);
      Bm = gan_squmatTf_lmult_i ( A, Bm ); cu_assert ( Bm != NULL );
      Cmp = matrixf_add ( Cmp, 1.0F, Bm, -1.0F, Cmp );
      cu_assert ( Cmp != NULL );
      cu_assert ( gan_matf_Fnorm(Cmp) < 1.0e-4F );

      /* test left multiplication of inverse of square matrix by generic
         matrix */
      A = squmatrixf_rand ( A, type, 3, 3 ); cu_assert ( A != NULL );
      Am = gan_matf_from_squmatf_q ( A, Am ); cu_assert ( Am != NULL );
      AmI = gan_matf_invert_q ( Am, AmI ); cu_assert ( AmI != NULL );
      Bm = matrixf_rand ( Bm, 5, 3 ); cu_assert ( Bm != NULL );
      Cmp = matrixf_matrixf_mult ( Bm, AmI, Cmp ); cu_assert (Cmp != NULL);
      Cm = gan_squmatIf_lmult_q ( A, Bm, Cm ); cu_assert ( Cm != NULL );
      Cmp = matrixf_add ( Cmp, 1.0F, Cm, -1.0F, Cmp );
      cu_assert ( Cmp != NULL );
      cu_assert ( gan_matf_Fnorm(Cmp) < 1.0e-4F );

      /* test left multiplication of inverse of square matrix by generic
         matrix in-place */
      A = squmatrixf_rand ( A, type, 3, 3 ); cu_assert ( A != NULL );
      Am = gan_matf_from_squmatf_q ( A, Am ); cu_assert ( Am != NULL );
      AmI = gan_matf_invert_q ( Am, AmI ); cu_assert ( AmI != NULL );
      Bm = matrixf_rand ( Bm, 5, 3 ); cu_assert ( Bm != NULL );
      Cmp = matrixf_matrixf_mult ( Bm, AmI, Cmp ); cu_assert (Cmp != NULL);
      Bm = gan_squmatIf_lmult_i ( A, Bm ); cu_assert ( Bm != NULL );
      Cmp = matrixf_add ( Cmp, 1.0F, Bm, -1.0F, Cmp );
      cu_assert ( Cmp != NULL );
      cu_assert ( gan_matf_Fnorm(Cmp) < 1.0e-4F );

      /* test left multiplication of inverse of square matrix by transpose of
         generic matrix */
      A = squmatrixf_rand ( A, type, 3, 3 ); cu_assert ( A != NULL );
      Am = gan_matf_from_squmatf_q ( A, Am ); cu_assert ( Am != NULL );
      AmI = gan_matf_invert_q ( Am, AmI ); cu_assert ( AmI != NULL );
      Bm = matrixf_rand ( Bm, 3, 5 ); cu_assert ( Bm != NULL );
      BmT = matrixf_transpose ( Bm, BmT ); cu_assert ( BmT != NULL );
      Cmp = matrixf_matrixf_mult ( BmT, AmI, Cmp );cu_assert (Cmp != NULL);
      Cm = gan_squmatIf_lmultT_q ( A, Bm, Cm ); cu_assert (Cm != NULL);
      Cmp = matrixf_add ( Cmp, 1.0F, Cm, -1.0F, Cmp );
      cu_assert ( Cmp != NULL );
      cu_assert ( gan_matf_Fnorm(Cmp) < 1.0e-4F );

      /* test left multiplication of inverse transpose of square matrix by
         generic matrix */
      A = squmatrixf_rand ( A, type, 3, 3 ); cu_assert ( A != NULL );
      Am = gan_matf_from_squmatf_q ( A, Am ); cu_assert ( Am != NULL );
      AmI = gan_matf_invert_q ( Am, AmI ); cu_assert ( AmI != NULL );
      AmIT = matrixf_transpose ( AmI, AmIT ); cu_assert (AmIT != NULL);
      Bm = matrixf_rand ( Bm, 5, 3 ); cu_assert ( Bm != NULL );
      Cmp = matrixf_matrixf_mult ( Bm, AmIT, Cmp );cu_assert (Cmp != NULL);
      Cm = gan_squmatITf_lmult_q ( A, Bm, Cm ); cu_assert (Cm != NULL);
      Cmp = matrixf_add ( Cmp, 1.0F, Cm, -1.0F, Cmp );
      cu_assert ( Cmp != NULL );
      cu_assert ( gan_matf_Fnorm(Cmp) < 1.0e-4F );

      /* test left multiplication of inverse transpose of square matrix by
         generic matrix in-place */
      A = squmatrixf_rand ( A, type, 3, 3 ); cu_assert ( A != NULL );
      Am = gan_matf_from_squmatf_q ( A, Am ); cu_assert ( Am != NULL );
      AmI = gan_matf_invert_q ( Am, AmI ); cu_assert ( AmI != NULL );
      AmIT = matrixf_transpose ( AmI, AmIT ); cu_assert (AmIT != NULL);
      Bm = matrixf_rand ( Bm, 5, 3 ); cu_assert ( Bm != NULL );
      Cmp = matrixf_matrixf_mult ( Bm, AmIT, Cmp );cu_assert (Cmp != NULL);
      Bm = gan_squmatITf_lmult_i ( A, Bm ); cu_assert ( Bm != NULL );
      Cmp = matrixf_add ( Cmp, 1.0F, Bm, -1.0F, Cmp );
      cu_assert ( Cmp != NULL );
      cu_assert ( gan_matf_Fnorm(Cmp) < 1.0e-4F );

      /* test left multiplication of inverse transpose of square matrix by
         transpose of generic matrix */
      A = squmatrixf_rand ( A, type, 3, 3 ); cu_assert ( A != NULL );
      Am = gan_matf_from_squmatf_q ( A, Am ); cu_assert ( Am != NULL );
      AmI = gan_matf_invert_q ( Am, AmI ); cu_assert ( AmI != NULL );
      AmIT = matrixf_transpose ( AmI, AmIT ); cu_assert (AmIT != NULL);
      Bm = matrixf_rand ( Bm, 3, 5 ); cu_assert ( Bm != NULL );
      BmT = matrixf_transpose ( Bm, BmT ); cu_assert ( BmT != NULL );
      Cmp = matrixf_matrixf_mult ( BmT, AmIT, Cmp ); cu_assert ( Cmp != NULL );
      Cm = gan_squmatITf_lmultT_q ( A, Bm, Cm ); cu_assert (Cm != NULL);
      Cmp = matrixf_add ( Cmp, 1.0F, Cm, -1.0F, Cmp );
      cu_assert ( Cmp != NULL );
      cu_assert ( gan_matf_Fnorm(Cmp) < 1.0e-4F );

      /* test right multiplication of square matrix by the transpose of the
         same matrix */
      A = squmatrixf_rand ( A, type, 3, 3 ); cu_assert ( A != NULL );
      Am = gan_matf_from_squmatf_q ( A, Am ); cu_assert ( Am != NULL );
      AmT = matrixf_transpose ( Am, AmT ); cu_assert ( AmT != NULL );
      Cmp = matrixf_matrixf_mult ( Am, AmT, Cmp ); cu_assert ( Cmp != NULL );
      B = gan_squmatf_srmultT_squ_q ( A, B ); cu_assert ( B != NULL );
      Bm = gan_matf_from_squmatf_q ( B, Bm ); cu_assert ( Bm != NULL );
      Cmp = matrixf_add ( Cmp, 1.0F, Bm, -1.0F, Cmp );
      cu_assert ( Cmp != NULL );
      cu_assert ( gan_matf_Fnorm(Cmp) < 1.0e-4F );

      /* test right multiplication of square matrix by the transpose of the
         same matrix in-place */
      A = squmatrixf_rand ( A, type, 3, 3 ); cu_assert ( A != NULL );
      Am = gan_matf_from_squmatf_q ( A, Am ); cu_assert ( Am != NULL );
      AmT = matrixf_transpose ( Am, AmT ); cu_assert ( AmT != NULL );
      Cmp = matrixf_matrixf_mult ( Am, AmT, Cmp ); cu_assert ( Cmp != NULL );
      A = gan_squmatf_srmultT_squ_i ( A ); cu_assert ( A != NULL );
      Am = gan_matf_from_squmatf_q ( A, Am ); cu_assert ( Am != NULL );
      Cmp = matrixf_add ( Cmp, 1.0F, Am, -1.0F, Cmp );
      cu_assert ( Cmp != NULL );
      cu_assert ( gan_matf_Fnorm(Cmp) < 1.0e-4F );

      /* test right multiplication of inverse of square matrix by the
         inverse transpose of the same matrix in-place */
      A = squmatrixf_rand ( A, type, 3, 3 ); cu_assert ( A != NULL );
      Am = gan_matf_from_squmatf_q ( A, Am ); cu_assert ( Am != NULL );
      AmI = gan_matf_invert_q ( Am, AmI ); cu_assert ( AmI != NULL );
      AmIT = matrixf_transpose ( AmI, AmIT ); cu_assert (AmIT != NULL);
      Cmp = matrixf_matrixf_mult ( AmI, AmIT, Cmp ); cu_assert ( Cmp != NULL );
      A = gan_squmatIf_srmultIT_squ_i ( A ); cu_assert ( A != NULL );
      Am = gan_matf_from_squmatf_q ( A, Am ); cu_assert ( Am != NULL );
      Cmp = matrixf_add ( Cmp, 1.0F, Am, -1.0F, Cmp );
      cu_assert ( Cmp != NULL );
      cu_assert ( gan_matf_Fnorm(Cmp) < 1.0e-4F );

      /* test right multiplication of transpose of square matrix by itself */
      A = squmatrixf_rand ( A, type, 3, 3 ); cu_assert ( A != NULL );
      Am = gan_matf_from_squmatf_q ( A, Am ); cu_assert ( Am != NULL );
      AmT = matrixf_transpose ( Am, AmT ); cu_assert ( AmT != NULL );
      Cmp = matrixf_matrixf_mult ( AmT, Am, Cmp ); cu_assert ( Cmp != NULL );
      B = gan_squmatTf_srmult_squ_q ( A, B ); cu_assert ( B != NULL );
      Bm = gan_matf_from_squmatf_q ( B, Bm ); cu_assert ( Bm != NULL );
      Cmp = matrixf_add ( Cmp, 1.0F, Bm, -1.0F, Cmp );
      cu_assert ( Cmp != NULL );
      cu_assert ( gan_matf_Fnorm(Cmp) < 1.0e-4F );

      /* test right multiplication of transpose of square matrix by itself
         in-place */
      A = squmatrixf_rand ( A, type, 3, 3 ); cu_assert ( A != NULL );
      Am = gan_matf_from_squmatf_q ( A, Am ); cu_assert ( Am != NULL );
      AmT = matrixf_transpose ( Am, AmT ); cu_assert ( AmT != NULL );
      Cmp = matrixf_matrixf_mult ( AmT, Am, Cmp ); cu_assert ( Cmp != NULL );
      A = gan_squmatTf_srmult_squ_i ( A ); cu_assert ( A != NULL );
      Am = gan_matf_from_squmatf_q ( A, Am ); cu_assert ( Am != NULL );
      Cmp = matrixf_add ( Cmp, 1.0F, Am, -1.0F, Cmp );
      cu_assert ( Cmp != NULL );
      cu_assert ( gan_matf_Fnorm(Cmp) < 1.0e-4F );

      /* test right multiplication of inverse transpose of square matrix by
         inverse of the same matrix */
      A = squmatrixf_rand ( A, type, 3, 3 ); cu_assert ( A != NULL );
      Am = gan_matf_from_squmatf_q ( A, Am ); cu_assert ( Am != NULL );
      AmI = gan_matf_invert_q ( Am, AmI ); cu_assert ( AmI != NULL );
      AmIT = matrixf_transpose ( AmI, AmIT ); cu_assert (AmIT != NULL);
      Cmp = matrixf_matrixf_mult ( AmIT, AmI, Cmp ); cu_assert ( Cmp != NULL );
      B = gan_squmatITf_srmultI_squ_q ( A, B ); cu_assert (B != NULL);
      Bm = gan_matf_from_squmatf_q ( B, Bm ); cu_assert ( Bm != NULL );
      Cmp = matrixf_add ( Cmp, 1.0F, Bm, -1.0F, Cmp );
      cu_assert ( Cmp != NULL );
      cu_assert ( gan_matf_Fnorm(Cmp) < 1.0e-4F );

      /* test right multiplication of inverse transpose of square matrix by
         inverse of the same matrix in-place */
      A = squmatrixf_rand ( A, type, 3, 3 ); cu_assert ( A != NULL );
      Am = gan_matf_from_squmatf_q ( A, Am ); cu_assert ( Am != NULL );
      AmI = gan_matf_invert_q ( Am, AmI ); cu_assert ( AmI != NULL );
      AmIT = matrixf_transpose ( AmI, AmIT ); cu_assert (AmIT != NULL);
      Cmp = matrixf_matrixf_mult ( AmIT, AmI, Cmp ); cu_assert ( Cmp != NULL );
      A = gan_squmatITf_srmultI_squ_i ( A ); cu_assert ( A != NULL );
      Am = gan_matf_from_squmatf_q ( A, Am ); cu_assert ( Am != NULL );
      Cmp = matrixf_add ( Cmp, 1.0F, Am, -1.0F, Cmp );
      cu_assert ( Cmp != NULL );
      cu_assert ( gan_matf_Fnorm(Cmp) < 1.0e-4F );
   }

   /* check whether the currently tested type is a symmetric type */
   A = squmatrixf_rand ( A, type, 3, 3 ); cu_assert ( A != NULL );
   if ( gan_squmatf_symmetric(A) )
   {
      /* test multiplication of symmetric matrix on left by a generic matrix
         on left and on the right by its transpose */
      A = squmatrixf_rand ( A, type, 3, 3 ); cu_assert ( A != NULL );
      Bm = matrixf_rand ( Bm, 5, 3 ); cu_assert ( Bm != NULL );
      BmT = matrixf_transpose ( Bm, BmT ); cu_assert ( BmT != NULL );
      Am = gan_matf_from_squmatf_q ( A, Am ); cu_assert ( Am != NULL );
      AmT = matrixf_matrixf_mult ( Am, BmT, AmT ); cu_assert (AmT != NULL);
      Cmp = matrixf_matrixf_mult ( Bm, AmT, Cmp ); cu_assert ( Cmp != NULL );
      C = gan_symmatf_lrmult_q ( A, Bm, BmT, C ); cu_assert (C != NULL);
      Cm = gan_matf_from_squmatf_q ( C, Cm ); cu_assert ( Cm != NULL );
      Cmp = matrixf_add ( Cmp, 1.0F, Cm, -1.0F, Cmp );
      cu_assert ( Cmp != NULL );
      cu_assert ( gan_matf_Fnorm(Cmp) < 1.0e-4F );
      AmT = matrixf_add ( AmT, 1.0F, BmT, -1.0F, AmT );
      cu_assert ( AmT != NULL );
      cu_assert ( gan_matf_Fnorm(AmT) < 1.0e-4F );

      /* test multiplication of symmetric matrix on left by the transpose of
         a generic matrix on the left and on the right by the matrix */
      A = squmatrixf_rand ( A, type, 3, 3 ); cu_assert ( A != NULL );
      Bm = matrixf_rand ( Bm, 3, 5 ); cu_assert ( Bm != NULL );
      BmT = matrixf_transpose ( Bm, BmT ); cu_assert ( BmT != NULL );
      Am = gan_matf_from_squmatf_q ( A, Am ); cu_assert ( Am != NULL );
      AmT = matrixf_matrixf_mult ( Am, Bm, AmT ); cu_assert (AmT != NULL);
      Cmp = matrixf_matrixf_mult ( BmT, AmT, Cmp ); cu_assert ( Cmp != NULL );
      C = gan_symmatf_lrmultT_q ( A, Bm, BmT, C ); cu_assert (C != NULL);
      Cm = gan_matf_from_squmatf_q ( C, Cm ); cu_assert ( Cm != NULL );
      Cmp = matrixf_add ( Cmp, 1.0F, Cm, -1.0F, Cmp );
      cu_assert ( Cmp != NULL );
      cu_assert ( gan_matf_Fnorm(Cmp) < 1.0e-4F );
      AmT = matrixf_add ( AmT, 1.0F, BmT, -1.0F, AmT );
      cu_assert ( AmT != NULL );
      cu_assert ( gan_matf_Fnorm(AmT) < 1.0e-4F );
   }

   /* test square matrix inversion */
   A = squmatrixf_rand ( A, type, 4, 4 ); cu_assert ( A != NULL );
   gan_symmatf_fill_va ( A, 4,
                         1.0,
                         1.0, 5.0,
                         0.0, 0.0, 1.0,
                         0.0, 0.0, 1.0, 5.0 );
   Am = gan_matf_from_squmatf_q ( A, Am ); cu_assert ( Am != NULL );
   B = gan_squmatf_invert_q ( A, B ); cu_assert ( B != NULL );
   Bm = gan_matf_from_squmatf_q ( B, Bm ); cu_assert ( Bm != NULL );
   Cm = matrixf_matrixf_mult ( Am, Bm, Cm ); cu_assert ( Cm != NULL );
   Cmp = matrixf_ident ( Cmp, Cm->rows ); cu_assert ( Cmp != NULL );
   Cmp = matrixf_add ( Cmp, 1.0F, Cm, -1.0F, Cmp ); cu_assert (Cmp != NULL);
   cu_assert ( gan_matf_Fnorm(Cmp) < 1.0e-4F );

   if ( type != GAN_SYMMETRIC_MATRIX )
   {
      /* test square matrix inversion in-place */
      A = squmatrixf_rand ( A, type, 4, 4 ); cu_assert ( A != NULL );
      Am = gan_matf_from_squmatf_q ( A, Am ); cu_assert ( Am != NULL );
      A = gan_squmatf_invert_i ( A ); cu_assert ( A != NULL );
      Bm = gan_matf_from_squmatf_q ( A, Bm ); cu_assert ( Bm != NULL );
      Cm = matrixf_matrixf_mult ( Am, Bm, Cm ); cu_assert ( Cm != NULL );
      Cmp = matrixf_ident ( Cmp, Cm->rows ); cu_assert ( Cmp != NULL );
      Cmp = matrixf_add ( Cmp, 1.0F, Cm, -1.0F, Cmp );
      cu_assert ( Cmp != NULL );
      cu_assert ( gan_matf_Fnorm(Cmp) < 1.0e-4F );
   }
   
   /* check whether the currently tested type is a symmetric type */
   A = squmatrixf_rand ( A, type, 3, 3 ); cu_assert ( A != NULL );
   if ( gan_squmatf_symmetric(A) )
   {
      /* test square matrix Cholesky factorisation */
      A = squmatrixf_rand ( A, type, 4, 4 ); cu_assert ( A != NULL );
      Am = gan_matf_from_squmatf_q ( A, Am ); cu_assert ( Am != NULL );
      B = gan_squmatf_cholesky_q ( A, B ); cu_assert ( B != NULL );
      Bm = gan_matf_from_squmatf_q ( B, Bm ); cu_assert ( Bm != NULL );
      BmT = matrixf_transpose ( Bm, BmT ); cu_assert ( BmT != NULL );
      Cmp = matrixf_matrixf_mult ( Bm, BmT, Cmp ); cu_assert (Cmp != NULL);
      Cmp = matrixf_add ( Cmp, 1.0F, Am, -1.0F, Cmp );
      cu_assert ( Cmp != NULL );
      cu_assert ( gan_matf_Fnorm(Cmp) < 1.0e-4F );

      /* test square matrix Cholesky factorisation in-place */
      A = squmatrixf_rand ( A, type, 4, 4 ); cu_assert ( A != NULL );
      Am = gan_matf_from_squmatf_q ( A, Am ); cu_assert ( Am != NULL );
      A = gan_squmatf_cholesky_i ( A ); cu_assert ( A != NULL );
      Bm = gan_matf_from_squmatf_q ( A, Bm ); cu_assert ( Bm != NULL );
      BmT = matrixf_transpose ( Bm, BmT ); cu_assert ( BmT != NULL );
      Cmp = matrixf_matrixf_mult ( Bm, BmT, Cmp ); cu_assert (Cmp != NULL);
      Cmp = matrixf_add ( Cmp, 1.0F, Am, -1.0F, Cmp );
      cu_assert ( Cmp != NULL );
      cu_assert ( gan_matf_Fnorm(Cmp) < 1.0e-4F );
   }

   /* free matrices & vectors */
   gan_vecf_free_va ( bp, b, a, NULL );
   gan_matf_free_va ( Cmp, Cm, BmT, Bm, AmIT, AmI, AmT, Am, NULL );
   gan_squmatf_free_va ( Cp, C, B, A, NULL );

   /* return with success */
   return GAN_TRUE;
}

static Gan_Bool test_svd(void)
{
   Gan_Matrix *A, *Ap, *U, *VT, *I;
   Gan_SquMatrix *S;
   int i;

   /* allocate matrices */
   A = gan_mat_alloc(0,0);
   Ap = gan_mat_alloc(0,0);
   U = gan_mat_alloc(0,0);
   S = gan_diagmat_alloc(0);
   VT = gan_mat_alloc(0,0);
   I = gan_mat_alloc(0,0);
   cu_assert ( A != NULL && Ap != NULL && U != NULL && S != NULL &&
               VT != NULL && I != NULL );

   /* build matrix A with row size < column size */
   A = gan_mat_fill_va ( A, 4, 5,
                          1.0,  4.0,  7.0, 10.0,  2.0,
                         -5.0,  8.0, 12.0, -3.0,  6.0,
                          9.0, 14.0,  5.0,  2.0, -1.0,
                          8.0,  3.0,  4.0,  1.0, 17.0 );
   cu_assert ( A != NULL );

   /* apply SVD to matrix A */
   Ap = gan_mat_copy_q(A,Ap); cu_assert ( Ap != NULL );
   cu_assert ( gan_mat_svd ( Ap, U, S, VT, NULL, 0 ) );

   /* test orthogonality of U */
   Ap = gan_matT_rmult_q ( U, U, Ap ); cu_assert ( Ap != NULL );
   I = gan_mat_fill_zero_q ( I, Ap->rows, Ap->cols );
   for ( i = (int)Ap->rows-1; i >= 0; i-- )
      gan_mat_set_el ( I, i, i, 1.0 );
   
   cu_assert ( gan_mat_decrement ( Ap, I ) != NULL );
   cu_assert ( gan_mat_Fnorm(Ap) < 1.0e-6 );
   
   /* test orthogonality of V^T */
   Ap = gan_mat_rmultT_q ( VT, VT, Ap ); cu_assert ( Ap != NULL );
   I = gan_mat_fill_zero_q ( I, Ap->rows, Ap->cols );
   for ( i = (int)Ap->rows-1; i >= 0; i-- )
      gan_mat_set_el ( I, i, i, 1.0 );
   
   cu_assert ( gan_mat_decrement ( Ap, I ) != NULL );
   cu_assert ( gan_mat_Fnorm(Ap) < 1.0e-6 );
   
   /* rebuild matrix from factors */
   cu_assert ( gan_squmat_rmult_i ( S, VT ) != NULL );
   Ap = gan_mat_rmult_q ( U, VT, Ap ); cu_assert ( Ap != NULL );
   cu_assert ( gan_mat_decrement ( Ap, A ) != NULL );
   cu_assert ( gan_mat_Fnorm(Ap) < 1.0e-6 );

   /* check ordering of singular values is monotonically decreasing */
   for ( i = (int)S->size-2; i >= 0; i-- )
      cu_assert ( gan_squmat_get_el(S,i,i) >= gan_squmat_get_el(S,i+1,i+1) );

   /* build matrix A with row size > column size */
   A = gan_mat_fill_va ( A, 5, 4,
                          1.0,  4.0,  7.0, 10.0,
                          2.0, -5.0,  8.0, 12.0,
                         -3.0,  6.0,  9.0, 14.0,
                          5.0,  2.0, -1.0,  8.0,
                          3.0,  4.0,  1.0, 17.0 );
   cu_assert ( A != NULL );

   /* apply SVD to matrix A */
   Ap = gan_mat_copy_q(A,Ap); cu_assert ( Ap != NULL );
   cu_assert ( gan_mat_svd ( Ap, U, S, VT, NULL, 0 ) );

   /* test orthogonality of U */
   Ap = gan_matT_rmult_q ( U, U, Ap ); cu_assert ( Ap != NULL );
   I = gan_mat_fill_zero_q ( I, Ap->rows, Ap->cols );
   for ( i = (int)Ap->rows-1; i >= 0; i-- )
      gan_mat_set_el ( I, i, i, 1.0 );
   
   cu_assert ( gan_mat_decrement ( Ap, I ) != NULL );
   cu_assert ( gan_mat_Fnorm(Ap) < 1.0e-6 );
   
   /* test orthogonality of V^T */
   Ap = gan_mat_rmultT_q ( VT, VT, Ap ); cu_assert ( Ap != NULL );
   I = gan_mat_fill_zero_q ( I, Ap->rows, Ap->cols );
   for ( i = (int)Ap->rows-1; i >= 0; i-- )
      gan_mat_set_el ( I, i, i, 1.0 );
   
   cu_assert ( gan_mat_decrement ( Ap, I ) != NULL );
   cu_assert ( gan_mat_Fnorm(Ap) < 1.0e-6 );
   
   /* rebuild matrix from factors */
   cu_assert ( gan_squmat_rmult_i ( S, VT ) != NULL );
   Ap = gan_mat_rmult_q ( U, VT, Ap ); cu_assert ( Ap != NULL );
   cu_assert ( gan_mat_decrement ( Ap, A ) != NULL );
   cu_assert ( gan_mat_Fnorm(Ap) < 1.0e-6 );

   /* check ordering of singular values is monotonically decreasing */
   for ( i = (int)S->size-2; i >= 0; i-- )
      cu_assert ( gan_squmat_get_el(S,i,i) >= gan_squmat_get_el(S,i+1,i+1) );

   /* build rank-deficient matrix A */
   A = gan_mat_fill_va ( A, 3, 3,
                         0.0,                   0.0,                  0.0,
                         0.070592885897144339,  0.0,                  0.70357419257716536,
                         0.0,                  -0.70710678118662018, -1e-15 );
   cu_assert ( A != NULL );

   /* apply SVD to matrix A */
   Ap = gan_mat_copy_q(A,Ap); cu_assert ( Ap != NULL );
   cu_assert ( gan_mat_svd ( Ap, U, S, VT, NULL, 0 ) );

   /* test orthogonality of U */
   Ap = gan_matT_rmult_q ( U, U, Ap ); cu_assert ( Ap != NULL );
   I = gan_mat_fill_zero_q ( I, Ap->rows, Ap->cols );
   for ( i = (int)Ap->rows-1; i >= 0; i-- )
      gan_mat_set_el ( I, i, i, 1.0 );
   
   cu_assert ( gan_mat_decrement ( Ap, I ) != NULL );
   cu_assert ( gan_mat_Fnorm(Ap) < 1.0e-6 );
   
   /* test orthogonality of V^T */
   Ap = gan_mat_rmultT_q ( VT, VT, Ap ); cu_assert ( Ap != NULL );
   I = gan_mat_fill_zero_q ( I, Ap->rows, Ap->cols );
   for ( i = (int)Ap->rows-1; i >= 0; i-- )
      gan_mat_set_el ( I, i, i, 1.0 );
   
   cu_assert ( gan_mat_decrement ( Ap, I ) != NULL );
   cu_assert ( gan_mat_Fnorm(Ap) < 1.0e-6 );
   
   /* rebuild matrix from factors */
   cu_assert ( gan_squmat_rmult_i ( S, VT ) != NULL );
   Ap = gan_mat_rmult_q ( U, VT, Ap ); cu_assert ( Ap != NULL );
   cu_assert ( gan_mat_decrement ( Ap, A ) != NULL );
   cu_assert ( gan_mat_Fnorm(Ap) < 1.0e-6 );

   /* check ordering of singular values is monotonically decreasing */
   for ( i = (int)S->size-2; i >= 0; i-- )
      cu_assert ( gan_squmat_get_el(S,i,i) >= gan_squmat_get_el(S,i+1,i+1) );

   /* free matrices and return with success */
   gan_mat_free_va ( I, VT, U, Ap, A, NULL );
   gan_squmat_free ( S );
   return GAN_TRUE;
}

static Gan_Bool test_svdf(void)
{
   Gan_Matrix_f *A, *Ap, *U, *VT, *I;
   Gan_SquMatrix_f *S;
   int i;

   /* allocate matrices */
   A = gan_matf_alloc(0,0);
   Ap = gan_matf_alloc(0,0);
   U = gan_matf_alloc(0,0);
   S = gan_diagmatf_alloc(0);
   VT = gan_matf_alloc(0,0);
   I = gan_matf_alloc(0,0);
   cu_assert ( A != NULL && Ap != NULL && U != NULL && S != NULL &&
               VT != NULL && I != NULL );

   /* build matrix A with row size < column size */
   A = gan_matf_fill_va ( A, 4, 5,
                           1.0,  4.0,  7.0, 10.0,  2.0,
                          -5.0,  8.0, 12.0, -3.0,  6.0,
                           9.0, 14.0,  5.0,  2.0, -1.0,
                           8.0,  3.0,  4.0,  1.0, 17.0 );
   cu_assert ( A != NULL );

   /* apply SVD to matrix A */
   Ap = gan_matf_copy_q(A,Ap); cu_assert ( Ap != NULL );
   cu_assert ( gan_matf_svd ( Ap, U, S, VT, NULL, 0 ) );

   /* test orthogonality of U */
   Ap = gan_matTf_rmult_q ( U, U, Ap ); cu_assert ( Ap != NULL );
   I = gan_matf_fill_zero_q ( I, Ap->rows, Ap->cols );
   for ( i = (int)Ap->rows-1; i >= 0; i-- )
      gan_matf_set_el ( I, i, i, 1.0F );
   
   cu_assert ( gan_matf_decrement ( Ap, I ) != NULL );
   cu_assert ( gan_matf_Fnorm(Ap) < 1.0e-4F );
   
   /* test orthogonality of V^T */
   Ap = gan_matf_rmultT_q ( VT, VT, Ap ); cu_assert ( Ap != NULL );
   I = gan_matf_fill_zero_q ( I, Ap->rows, Ap->cols );
   for ( i = (int)Ap->rows-1; i >= 0; i-- )
      gan_matf_set_el ( I, i, i, 1.0F );
   
   cu_assert ( gan_matf_decrement ( Ap, I ) != NULL );
   cu_assert ( gan_matf_Fnorm(Ap) < 1.0e-4F );
   
   /* rebuild matrix from factors */
   cu_assert ( gan_squmatf_rmult_i ( S, VT ) != NULL );
   Ap = gan_matf_rmult_q ( U, VT, Ap ); cu_assert ( Ap != NULL );
   cu_assert ( gan_matf_decrement ( Ap, A ) != NULL );
   cu_assert ( gan_matf_Fnorm(Ap) < 1.0e-4F );

   /* check ordering of singular values is monotonically decreasing */
   for ( i = (int)S->size-2; i >= 0; i-- )
      cu_assert ( gan_squmatf_get_el(S,i,i) >= gan_squmatf_get_el(S,i+1,i+1) );

   /* build matrix A with row size > column size */
   A = gan_matf_fill_va ( A, 5, 4,
                           1.0,  4.0,  7.0, 10.0,
                           2.0, -5.0,  8.0, 12.0,
                          -3.0,  6.0,  9.0, 14.0,
                           5.0,  2.0, -1.0,  8.0,
                           3.0,  4.0,  1.0, 17.0 );
   cu_assert ( A != NULL );

   /* apply SVD to matrix A */
   Ap = gan_matf_copy_q(A,Ap); cu_assert ( Ap != NULL );
   cu_assert ( gan_matf_svd ( Ap, U, S, VT, NULL, 0 ) );

   /* test orthogonality of U */
   Ap = gan_matTf_rmult_q ( U, U, Ap ); cu_assert ( Ap != NULL );
   I = gan_matf_fill_zero_q ( I, Ap->rows, Ap->cols );
   for ( i = (int)Ap->rows-1; i >= 0; i-- )
      gan_matf_set_el ( I, i, i, 1.0F );
   
   cu_assert ( gan_matf_decrement ( Ap, I ) != NULL );
   cu_assert ( gan_matf_Fnorm(Ap) < 1.0e-4F );
   
   /* test orthogonality of V^T */
   Ap = gan_matf_rmultT_q ( VT, VT, Ap ); cu_assert ( Ap != NULL );
   I = gan_matf_fill_zero_q ( I, Ap->rows, Ap->cols );
   for ( i = (int)Ap->rows-1; i >= 0; i-- )
      gan_matf_set_el ( I, i, i, 1.0F );
   
   cu_assert ( gan_matf_decrement ( Ap, I ) != NULL );
   cu_assert ( gan_matf_Fnorm(Ap) < 1.0e-4F );
   
   /* rebuild matrix from factors */
   cu_assert ( gan_squmatf_rmult_i ( S, VT ) != NULL );
   Ap = gan_matf_rmult_q ( U, VT, Ap ); cu_assert ( Ap != NULL );
   cu_assert ( gan_matf_decrement ( Ap, A ) != NULL );
   cu_assert ( gan_matf_Fnorm(Ap) < 1.0e-4F );

   /* check ordering of singular values is monotonically decreasing */
   for ( i = (int)S->size-2; i >= 0; i-- )
      cu_assert ( gan_squmatf_get_el(S,i,i) >= gan_squmatf_get_el(S,i+1,i+1) );

   /* free matrices and return with success */
   gan_matf_free_va ( I, VT, U, Ap, A, NULL );
   gan_squmatf_free ( S );
   return GAN_TRUE;
}

/* test QR matrix factorisation */
static Gan_Bool test_qr(void)
{
   Gan_Matrix *A, *Q, *QP, *Ap;
   Gan_SquMatrix *R;

   /* build matrix A */
   A = gan_mat_fill_va ( NULL, 4, 3,  1.0,  2.0, -3.0,
                                      4.0, -5.0,  6.0,
                                      7.0,  8.0,  9.0,
                                     10.0, 12.0, 14.0 );
   cu_assert ( A != NULL );

   /* apply QR factorisation of matrix A */
   Q = gan_mat_copy_s(A); cu_assert ( Q != NULL );
   R = gan_utmat_alloc(3); cu_assert ( R != NULL );
   cu_assert ( gan_mat_qr ( A, Q, R, NULL, 0 ) );

   /* build QP matrix with first three columns of Q */
   QP = gan_mat_alloc ( 4, 3 ); cu_assert ( QP != NULL );
   cu_assert ( gan_mat_insert ( QP, 0, 0, Q, 0, 0, 4, 3 ) != NULL );

   /* reconstruct matrix using factors */
   Ap = gan_squmat_lmult_s ( R, QP ); cu_assert ( Ap != NULL );

   /* compare with original matrix */
   cu_assert ( gan_mat_decrement ( Ap, A ) != NULL );
   cu_assert ( gan_mat_Fnorm(Ap) < 1.0e-6 );

   /* free matrices and return with success */
   gan_mat_free_va ( Ap, QP, Q, A, NULL );
   gan_squmat_free ( R );
   return GAN_TRUE;
}

/* test single precision QR matrix factorisation */
static Gan_Bool test_qrf(void)
{
   Gan_Matrix_f *A, *Q, *QP, *Ap;
   Gan_SquMatrix_f *R;

   /* build matrix A */
   A = gan_matf_fill_va ( NULL, 4, 3,  1.0,  2.0, -3.0,
                                       4.0, -5.0,  6.0,
                                       7.0,  8.0,  9.0,
                                      10.0, 12.0, 14.0 );
   cu_assert ( A != NULL );

   /* apply QR factorisation of matrix A */
   Q = gan_matf_copy_s(A); cu_assert ( Q != NULL );
   R = gan_utmatf_alloc(3); cu_assert ( R != NULL );
   cu_assert ( gan_matf_qr ( A, Q, R, NULL, 0 ) );

   /* build QP matrix with first three columns of Q */
   QP = gan_matf_alloc ( 4, 3 ); cu_assert ( QP != NULL );
   cu_assert ( gan_matf_insert ( QP, 0, 0, Q, 0, 0, 4, 3 ) != NULL );

   /* reconstruct matrix using factors */
   Ap = gan_squmatf_lmult_s ( R, QP ); cu_assert ( Ap != NULL );

   /* compare with original matrix */
   cu_assert ( gan_matf_decrement ( Ap, A ) != NULL );
   cu_assert ( gan_matf_Fnorm(Ap) < 1.0e-4F );

   /* free matrices and return with success */
   gan_matf_free_va ( Ap, QP, Q, A, NULL );
   gan_squmatf_free ( R );
   return GAN_TRUE;
}

/* test LQ matrix factorisation */
static Gan_Bool test_lq(void)
{
   Gan_Matrix *A, *Q, *QP, *Ap;
   Gan_SquMatrix *L;

   /* build matrix A */
   A = gan_mat_fill_va ( NULL, 3, 4,  1.0,  4.0,  7.0, 10.0,
                                      2.0, -5.0,  8.0, 12.0,
                                     -3.0,  6.0,  9.0, 14.0 );
   cu_assert ( A != NULL );

   /* apply QR factorisation of matrix A */
   Q = gan_mat_copy_s(A); cu_assert ( Q != NULL );
   L = gan_ltmat_alloc(3); cu_assert ( L != NULL );
   cu_assert ( gan_mat_lq ( A, L, Q, NULL, 0 ) );

   /* build QP matrix with first three columns of Q */
   QP = gan_mat_alloc ( 3, 4 ); cu_assert ( QP != NULL );
   cu_assert ( gan_mat_insert ( QP, 0, 0, Q, 0, 0, 3, 4 ) != NULL );

   /* reconstruct matrix using factors */
   Ap = gan_squmat_rmult_s ( L, QP ); cu_assert ( Ap != NULL );

   /* compare with original matrix */
   cu_assert ( gan_mat_decrement ( Ap, A ) != NULL );
   cu_assert ( gan_mat_Fnorm(Ap) < 1.0e-6 );

   /* free matrices and return with success */
   gan_mat_free_va ( Ap, QP, Q, A, NULL );
   gan_squmat_free ( L );
   return GAN_TRUE;
}

/* test single precision LQ matrix factorisation */
static Gan_Bool test_lqf(void)
{
   Gan_Matrix_f *A, *Q, *QP, *Ap;
   Gan_SquMatrix_f *L;

   /* build matrix A */
   A = gan_matf_fill_va ( NULL, 3, 4,  1.0,  4.0,  7.0, 10.0,
                                       2.0, -5.0,  8.0, 12.0,
                                      -3.0,  6.0,  9.0, 14.0 );
   cu_assert ( A != NULL );

   /* apply QR factorisation of matrix A */
   Q = gan_matf_copy_s(A); cu_assert ( Q != NULL );
   L = gan_ltmatf_alloc(3); cu_assert ( L != NULL );
   cu_assert ( gan_matf_lq ( A, L, Q, NULL, 0 ) );

   /* build QP matrix with first three columns of Q */
   QP = gan_matf_alloc ( 3, 4 ); cu_assert ( QP != NULL );
   cu_assert ( gan_matf_insert ( QP, 0, 0, Q, 0, 0, 3, 4 ) != NULL );

   /* reconstruct matrix using factors */
   Ap = gan_squmatf_rmult_s ( L, QP ); cu_assert ( Ap != NULL );

   /* compare with original matrix */
   cu_assert ( gan_matf_decrement ( Ap, A ) != NULL );
   cu_assert ( gan_matf_Fnorm(Ap) < 1.0e-4F );

   /* free matrices and return with success */
   gan_matf_free_va ( Ap, QP, Q, A, NULL );
   gan_squmatf_free ( L );
   return GAN_TRUE;
}

static Gan_Bool test_symeigen(void)
{
   Gan_SymMatEigenStruct sme;
   Gan_Matrix *Zp, *I;
   Gan_SquMatrix *SxxT, *Sp;
   int i;

   /* form eigensystem structure */
   cu_assert ( gan_symeigen_form(&sme,6) != NULL );

   /* build matrix */
   for ( i = 0; i < 100; i++ )
      cu_assert ( gan_symeigen_increment (&sme, 1.0,
                                          gan_random_m11(), gan_random_m11(),
                                          gan_random_m11(), gan_random_m11(),
                                          gan_random_m11(), gan_random_m11()));

   /* copy symmetric matrix */
   cu_assert ( (SxxT = gan_squmat_copy_s(&sme.SxxT)) != NULL );

   /* solve eigensystem */
   cu_assert ( gan_symeigen_solve ( &sme ) );

   /* check orthogonality of eigenvector matrix */
   cu_assert ( (Zp = gan_matT_rmult_s ( &sme.Z, &sme.Z )) != NULL );
   I = gan_mat_fill_zero_s ( Zp->rows, Zp->cols );
   for ( i = (int)Zp->rows-1; i >= 0; i-- )
      gan_mat_set_el ( I, i, i, 1.0 );
   
   cu_assert ( gan_mat_decrement ( Zp, I ) != NULL );
   cu_assert ( gan_mat_Fnorm(Zp) < 1.0e-6 );
   
   /* rebuild matrix from eigenfactors */
   cu_assert ( (Zp = gan_squmat_lmult_q ( &sme.W, &sme.Z, Zp )) != NULL );
   cu_assert ( (Sp = gan_mat_rmultT_sym_s ( Zp, &sme.Z )) != NULL );

   /* check that matrices agree */
   cu_assert ( gan_squmat_decrement ( Sp, SxxT ) != NULL );
   cu_assert ( gan_squmat_Fnorm(Sp) < 1.0e-4 );

   gan_squmat_free ( Sp );
   gan_mat_free ( I );
   gan_mat_free ( Zp );
   gan_squmat_free ( SxxT );
   gan_symeigen_free ( &sme );
   return GAN_TRUE;
}

/* test incremental pseudo-inverse algorithm */
static Gan_Bool test_pseudoinv(void)
{
   Gan_PseudoInverseStruct *pis;
   Gan_Matrix *A, *B, *X, *AX, *C1, *D1, *C2, *D2;
   Gan_Vector *null_vector, *AN;
   int i, error_code;

   pis = gan_pseudoinv_alloc ( 4, 2 );
   cu_assert ( pis != NULL );

   /* build matrices A, B to be used to solve A*X = B, solving for the case
      of # equations equal to # variables */
   A = gan_mat_fill_va ( NULL, 4, 4,  2.0,  3.0,  4.0,  5.0,
                                     -6.0, 13.0, 24.0, -5.0,
                                      7.0, -8.0, 16.0, 15.0,
                                      9.0, 11.0, 12.0, 31.0 );
   B = gan_mat_fill_va ( NULL, 4, 2,  6.0, -2.0,
                                      36.0,  8.0,
                                      23.0,  5.0,
                                      33.0, 35.0 );
   cu_assert ( A != NULL && B != NULL );

   /* build pseudo-inverse equations row by row */
   cu_assert ( gan_pseudoinv_reset ( pis, 4, 2 ) );
   for ( i = 0; i < A->rows; i++ )
      cu_assert ( gan_pseudoinv_increment ( pis, 1.0,
                                            gel(A,i,0), gel(A,i,1),
                                            gel(A,i,2), gel(A,i,3),
                                            gel(B,i,0), gel(B,i,1) ) );

   /* solve equations using pseudo-inverse */
   cu_assert ( gan_pseudoinv_solve ( pis, &error_code ) );

   /* get solution */
   X = gan_pseudoinv_get_solution(pis);
   cu_assert ( X != NULL );

   /* print B and A*X to make sure they are the same */
   AX = gan_mat_rmult_s ( A, X ); cu_assert ( AX != NULL );
   cu_assert ( gan_mat_decrement ( AX, B ) != NULL );
   cu_assert ( gan_mat_Fnorm(AX) < 1.0e-6 );

   /* rebuild matrices A, B to be used to solve A*X = B, for the case
      # equations < # variables */
   cu_assert ( gan_mat_fill_va ( A, 2, 4,  2.0,  3.0,  4.0,  5.0,
                                          -6.0, 13.0, 24.0, -5.0 ) != NULL &&
               gan_mat_fill_va ( B, 2, 2,  6.0, -2.0,
                                          36.0,  8.0 ) != NULL );

   /* build pseudo-inverse equations row by row */
   cu_assert ( gan_pseudoinv_reset ( pis, 4, 2 ) );
   for ( i = 0; i < A->rows; i++ )
      cu_assert ( gan_pseudoinv_increment ( pis, 1.0,
                                            gel(A,i,0), gel(A,i,1),
                                            gel(A,i,2), gel(A,i,3),
                                            gel(B,i,0), gel(B,i,1) ) );

   /* we have only two equations in four variables, so set the null-space
      to dimension two */
   cu_assert ( gan_pseudoinv_set_null_space ( pis, 2 ) );

   /* solve equations using pseudo-inverse */
   cu_assert ( gan_pseudoinv_solve ( pis, &error_code ) );

   /* get solution */
   X = gan_pseudoinv_get_solution(pis);
   cu_assert ( X != NULL );

   /* print B and A*X to make sure they are the same */
   cu_assert ( gan_mat_rmult_q ( A, X, AX ) != NULL );
   cu_assert ( gan_mat_decrement ( AX, B ) != NULL );
   cu_assert ( gan_mat_Fnorm(AX) < 1.0e-6 );

   /* get null-vectors and check they work */
   null_vector = gan_pseudoinv_get_null_vector(pis);
   cu_assert ( null_vector != NULL );
   AN = gan_mat_multv_s ( A, null_vector );
   cu_assert ( AN != NULL );
   cu_assert ( gan_vec_sqrlen(AN) < 1.0e-6 );
   null_vector = gan_pseudoinv_get_null_vector(pis);
   cu_assert ( null_vector != NULL );
   cu_assert ( gan_mat_multv_q ( A, null_vector, AN ) != NULL );
   cu_assert ( gan_vec_sqrlen(AN) < 1.0e-6 );
   gan_vec_free ( AN );

   /* rebuild matrices A, B to be used to solve A*X = B, for the case
      # equations < # variables, with specified constraints making up the
      degrees of freedom */
   cu_assert ( gan_mat_fill_va ( A, 3, 5,  2.0,  3.0,  4.0,  5.0, -1.0,
                                          -6.0, 13.0, 24.0, -5.0, 43.0,
                                          -3.0, -8.0, 14.0, 32.0, -7.0 )
               != NULL &&
               gan_mat_fill_va ( B, 3, 2,  6.0, -2.0,
                                          36.0,  8.0,
                                          25.0, -4.0 ) != NULL );

   /* build pseudo-inverse equations row by row */
   cu_assert ( gan_pseudoinv_reset ( pis, 5, 2 ) );
   for ( i = 0; i < A->rows; i++ )
      cu_assert ( gan_pseudoinv_increment ( pis, 1.0,
                    gel(A,i,0), gel(A,i,1), gel(A,i,2), gel(A,i,3), gel(A,i,4),
                                            gel(B,i,0), gel(B,i,1) ) );

   /* we have only two equations in four variables, so apply the constraints
      to complete the system */

   /* build first constraint matrix */
   C1 = gan_mat_fill_va ( NULL, 1, 5, 20.0, 13.0, 14.0, -5.0, 11.0 );
   D1 = gan_mat_fill_va ( NULL, 1, 2, -6.0, 22.0 );
   cu_assert ( C1 != NULL && D1 != NULL );

   /* apply constraint */
   cu_assert ( gan_pseudoinv_set_constraint ( pis, C1, D1 ) );

   /* build second constraint matrix */
   C2 = gan_mat_fill_va ( NULL, 1, 5, 16.0, 33.0, -4.0, 25.0, 15.0 );
   D2 = gan_mat_fill_va ( NULL, 1, 2, 16.0, -3.0 );
   cu_assert ( C2 != NULL && D2 != NULL );

   /* apply constraint */
   cu_assert ( gan_pseudoinv_set_constraint ( pis, C2, D2 ) );
                                   
   /* solve equations using pseudo-inverse */
   cu_assert ( gan_pseudoinv_solve ( pis, &error_code ) );

   /* get solution */
   X = gan_pseudoinv_get_solution(pis);
   cu_assert ( X != NULL );

   /* print B and A*X to make sure they are the same */
   cu_assert ( gan_mat_rmult_q ( A, X, AX ) != NULL );
   cu_assert ( gan_mat_decrement ( AX, B ) != NULL );
   cu_assert ( gan_mat_Fnorm(AX) < 1.0e-6 );

   /* check that constraints have been imposed correctly */
   cu_assert ( gan_mat_rmult_q ( C1, X, AX ) != NULL );
   cu_assert ( gan_mat_decrement ( AX, D1 ) != NULL );
   cu_assert ( gan_mat_Fnorm(AX) < 1.0e-6 );
   cu_assert ( gan_mat_rmult_q ( C2, X, AX ) != NULL );
   cu_assert ( gan_mat_decrement ( AX, D2 ) != NULL );
   cu_assert ( gan_mat_Fnorm(AX) < 1.0e-6 );

   /* free matrices and structures and return with success */
   gan_mat_free_va ( D2, C2, D1, C1, AX, B, A, NULL );
   gan_pseudoinv_free(pis);
   return GAN_TRUE;
}

/* test incremental pseudo-inverse algorithm in single precision */
static Gan_Bool test_pseudoinvf(void)
{
   Gan_PseudoInverseStruct_f *pis;
   Gan_Matrix_f *A, *B, *X, *AX, *C1, *D1, *C2, *D2;
   Gan_Vector_f *null_vector, *AN;
   int i, error_code;

   pis = gan_pseudoinvf_alloc ( 4, 2 );
   cu_assert ( pis != NULL );

   /* build matrices A, B to be used to solve A*X = B, solving for the case
      of # equations equal to # variables */
   A = gan_matf_fill_va ( NULL, 4, 4,  2.0,  3.0,  4.0,  5.0,
                                      -6.0, 13.0, 24.0, -5.0,
                                       7.0, -8.0, 16.0, 15.0,
                                       9.0, 11.0, 12.0, 31.0 );
   B = gan_matf_fill_va ( NULL, 4, 2,  6.0, -2.0,
                                      36.0,  8.0,
                                      23.0,  5.0,
                                      33.0, 35.0 );
   cu_assert ( A != NULL && B != NULL );

   /* build pseudo-inverse equations row by row */
   cu_assert ( gan_pseudoinvf_reset ( pis, 4, 2 ) );
   for ( i = 0; i < A->rows; i++ )
      cu_assert ( gan_pseudoinvf_increment ( pis, 1.0,
                                             (double)gelf(A,i,0),
                                             (double)gelf(A,i,1),
                                             (double)gelf(A,i,2),
                                             (double)gelf(A,i,3),
                                             (double)gelf(B,i,0),
                                             (double)gelf(B,i,1) ) );

   /* solve equations using pseudo-inverse */
   cu_assert ( gan_pseudoinvf_solve ( pis, &error_code ) );

   /* get solution */
   X = gan_pseudoinvf_get_solution(pis);
   cu_assert ( X != NULL );

   /* print B and A*X to make sure they are the same */
   AX = gan_matf_rmult_s ( A, X ); cu_assert ( AX != NULL );
   cu_assert ( gan_matf_decrement ( AX, B ) != NULL );
   cu_assert ( gan_matf_Fnorm(AX) < 1.0e-3 );

   /* rebuild matrices A, B to be used to solve A*X = B, for the case
      # equations < # variables */
   cu_assert ( gan_matf_fill_va ( A, 2, 4,  2.0,  3.0,  4.0,  5.0,
                                           -6.0, 13.0, 24.0, -5.0 ) != NULL &&
               gan_matf_fill_va ( B, 2, 2,  6.0, -2.0,
                                           36.0,  8.0 ) != NULL );

   /* build pseudo-inverse equations row by row */
   cu_assert ( gan_pseudoinvf_reset ( pis, 4, 2 ) );
   for ( i = 0; i < A->rows; i++ )
      cu_assert ( gan_pseudoinvf_increment ( pis, 1.0,
                                             (double)gelf(A,i,0),
                                             (double)gelf(A,i,1),
                                             (double)gelf(A,i,2),
                                             (double)gelf(A,i,3),
                                             (double)gelf(B,i,0),
                                             (double)gelf(B,i,1) ) );

   /* we have only two equations in four variables, so set the null-space
      to dimension two */
   cu_assert ( gan_pseudoinvf_set_null_space ( pis, 2 ) );

   /* solve equations using pseudo-inverse */
   cu_assert ( gan_pseudoinvf_solve ( pis, &error_code ) );

   /* get solution */
   X = gan_pseudoinvf_get_solution(pis);
   cu_assert ( X != NULL );

   /* print B and A*X to make sure they are the same */
   cu_assert ( gan_matf_rmult_q ( A, X, AX ) != NULL );
   cu_assert ( gan_matf_decrement ( AX, B ) != NULL );
   cu_assert ( gan_matf_Fnorm(AX) < 1.0e-4F );

   /* get null-vectors and check they work */
   null_vector = gan_pseudoinvf_get_null_vector(pis);
   cu_assert ( null_vector != NULL );
   AN = gan_matf_multv_s ( A, null_vector );
   cu_assert ( AN != NULL );
   cu_assert ( gan_vecf_sqrlen(AN) < 1.0e-4F );
   null_vector = gan_pseudoinvf_get_null_vector(pis);
   cu_assert ( null_vector != NULL );
   cu_assert ( gan_matf_multv_q ( A, null_vector, AN ) != NULL );
   cu_assert ( gan_vecf_sqrlen(AN) < 1.0e-4F );
   gan_vecf_free ( AN );

   /* rebuild matrices A, B to be used to solve A*X = B, for the case
      # equations < # variables, with specified constraints making up the
      degrees of freedom */
   cu_assert ( gan_matf_fill_va ( A, 3, 5,  2.0,  3.0,  4.0,  5.0, -1.0
                                           -6.0, 13.0, 24.0, -5.0, 43.0,
                                           -3.0, -8.0, 14.0, 32.0, -7.0 )
               != NULL &&
               gan_matf_fill_va ( B, 3, 2,  6.0, -2.0,
                                           36.0,  8.0,
                                           25.0, -4.0 ) != NULL );

   /* build pseudo-inverse equations row by row */
   cu_assert ( gan_pseudoinvf_reset ( pis, 5, 2 ) );
   for ( i = 0; i < A->rows; i++ )
      cu_assert ( gan_pseudoinvf_increment ( pis, 1.0,
               gelf(A,i,0), gelf(A,i,1), gelf(A,i,2), gelf(A,i,3), gelf(A,i,4),
                                             gelf(B,i,0), gelf(B,i,1) ) );

   /* we have only two equations in four variables, so apply the constraints
      to complete the system */

   /* build first constraint matrix */
   C1 = gan_matf_fill_va ( NULL, 1, 5, 20.0, 13.0, 14.0, -5.0, 11.0 );
   D1 = gan_matf_fill_va ( NULL, 1, 2, -6.0, 22.0 );
   cu_assert ( C1 != NULL && D1 != NULL );

   /* apply constraint */
   cu_assert ( gan_pseudoinvf_set_constraint ( pis, C1, D1 ) );

   /* build second constraint matrix */
   C2 = gan_matf_fill_va ( NULL, 1, 5, 16.0, 33.0, -4.0, 25.0, 15.0 );
   D2 = gan_matf_fill_va ( NULL, 1, 2, 16.0, -3.0 );
   cu_assert ( C2 != NULL && D2 != NULL );

   /* apply constraint */
   cu_assert ( gan_pseudoinvf_set_constraint ( pis, C2, D2 ) );
                                   
   /* solve equations using pseudo-inverse */
   cu_assert ( gan_pseudoinvf_solve ( pis, &error_code ) );

   /* get solution */
   X = gan_pseudoinvf_get_solution(pis);
   cu_assert ( X != NULL );

   /* print B and A*X to make sure they are the same */
   cu_assert ( gan_matf_rmult_q ( A, X, AX ) != NULL );
   cu_assert ( gan_matf_decrement ( AX, B ) != NULL );
   cu_assert ( gan_matf_Fnorm(AX) < 2.0e-4F );

   /* check that constraints have been imposed correctly */
   cu_assert ( gan_matf_rmult_q ( C1, X, AX ) != NULL );
   cu_assert ( gan_matf_decrement ( AX, D1 ) != NULL );
   cu_assert ( gan_matf_Fnorm(AX) < 1.0e-4F );
   cu_assert ( gan_matf_rmult_q ( C2, X, AX ) != NULL );
   cu_assert ( gan_matf_decrement ( AX, D2 ) != NULL );
   cu_assert ( gan_matf_Fnorm(AX) < 1.0e-4F );

   /* free matrices and structures and return with success */
   gan_matf_free_va ( D2, C2, D1, C1, AX, B, A, NULL );
   gan_pseudoinvf_free(pis);
   return GAN_TRUE;
}

#ifdef HAVE_LAPACK
static Gan_Bool test_schur(void)
{
   Gan_Matrix *A, *T, *Z, *ZT, *ZTZT;

   A = gan_mat_alloc ( 3, 3 );
   Z = gan_mat_alloc ( 3, 3 );
   T = gan_mat_alloc ( 3, 3 );
   ZT = gan_mat_alloc ( 3, 3 );
   ZTZT = gan_mat_alloc ( 3, 3 );
   cu_assert ( A != NULL && Z != NULL && T != NULL && ZT != NULL &&
               ZTZT != NULL );
   gan_mat_fill_va ( A, 3, 3,
                     gan_random_m11(), gan_random_m11(), gan_random_m11(),
                     gan_random_m11(), gan_random_m11(), gan_random_m11(),
                     gan_random_m11(), gan_random_m11(), gan_random_m11() );
   gan_mat_copy_q ( A, T );
   cu_assert ( gan_clapack_gees ( T, Z, NULL, 0 ) );
   gan_mat_rmult_q ( Z, T, ZT );
   gan_mat_rmultT_q ( ZT, Z, ZTZT );
   gan_mat_free_va ( ZTZT, ZT, T, Z, A, NULL );
   return GAN_TRUE;
}
#endif

static Gan_Bool setup_test(void)
{
   printf("\nSetup for linalg_test completed!\n\n");
   return GAN_TRUE;
}

static Gan_Bool teardown_test(void)
{
   printf("\nTeardown for linalg_test completed!\n\n");
   return GAN_TRUE;
}

/* Runs the linalg test functions */
static Gan_Bool run_test(void)
{
   int it;

   for ( it = 1; it <= 50; it++ )
   {
#ifdef HAVE_LAPACK
      cu_assert ( test_schur() );
#endif
      cu_assert ( test_mat22() );
      cu_assert ( test_mat33() );
      cu_assert ( test_mat44() );
      cu_assert ( test_mat23() );
      cu_assert ( test_mat24() );
      cu_assert ( test_mat34() );
      cu_assert ( test_symmat22() );
      cu_assert ( test_symmat33() );
      cu_assert ( test_symmat44() );
      cu_assert ( test_ltmat22() );
      cu_assert ( test_ltmat33() );
      cu_assert ( test_ltmat44() );
      cu_assert ( test_mat22f() );
      cu_assert ( test_mat33f() );
      cu_assert ( test_mat44f() );
      cu_assert ( test_mat23f() );
      cu_assert ( test_mat24f() );
      cu_assert ( test_mat34f() );
      cu_assert ( test_symmat22f() );
      cu_assert ( test_symmat33f() );
      cu_assert ( test_symmat44f() );
      cu_assert ( test_ltmat22f() );
      cu_assert ( test_ltmat33f() );
      cu_assert ( test_ltmat44f() );
      cu_assert ( test_vec_gen() );
      cu_assert ( test_mat_gen() );
      cu_assert ( test_mat_square(GAN_SYMMETRIC_MATRIX) )   ;
      cu_assert ( test_mat_square(GAN_DIAGONAL_MATRIX) )    ;
      cu_assert ( test_mat_square(GAN_SCALED_IDENT_MATRIX) );
      cu_assert ( test_mat_square(GAN_LOWER_TRI_MATRIX) )   ;
      cu_assert ( test_mat_square(GAN_UPPER_TRI_MATRIX) )   ;
      cu_assert ( test_vecf_gen() );
      cu_assert ( test_matf_gen() );
      cu_assert ( test_matf_square(GAN_SYMMETRIC_MATRIX) )   ;
      cu_assert ( test_matf_square(GAN_DIAGONAL_MATRIX) )    ;
      cu_assert ( test_matf_square(GAN_SCALED_IDENT_MATRIX) );
      cu_assert ( test_matf_square(GAN_LOWER_TRI_MATRIX) )   ;
      cu_assert ( test_matf_square(GAN_UPPER_TRI_MATRIX) )   ;
      cu_assert ( test_svd() );
      cu_assert ( test_svdf() );
      cu_assert ( test_qr() );
      cu_assert ( test_qrf() );
      cu_assert ( test_lq() );
      cu_assert ( test_lqf() );
      cu_assert ( test_symeigen() );
      cu_assert ( test_pseudoinv() );
      cu_assert ( test_pseudoinvf() );
      cu_assert ( test_vec3() );
      cu_assert ( test_vec3f() );
   }

   return GAN_TRUE;
}

#ifdef LINALG_TEST_MAIN

int main ( int argc, char *argv[] )
{
   /* set default Gandalf error handler without trace handling */
   gan_err_set_reporter ( gan_err_default_reporter );
   gan_err_set_trace ( GAN_ERR_TRACE_OFF );

   setup_test();
   if ( run_test() )
      printf ( "Tests ran successfully!\n" );
   else
      printf ( "At least one test failed\n" );

   teardown_test();
   gan_heap_report(NULL);
   return 0;
}

#else

/* This function registers the unit tests to a cUnit_test_suite. */
cUnit_test_suite *linalg_test_build_suite(void)
{
   cUnit_test_suite *sp;
        
   /* Get a new test session */
   sp = cUnit_new_test_suite("linalg_test suite");
        
   cUnit_add_test(sp, "linalg_test", run_test);
        
   /* register a setup and teardown on the test suite 'linalg_test' */
   if (cUnit_register_setup(sp, setup_test) != GAN_TRUE)
      printf("Error while setting up test suite linalg_test");
        
   if (cUnit_register_teardown(sp, teardown_test) != GAN_TRUE)
      printf("Error while tearing down test suite linalg_test");
        
   return( sp );
}

#endif /* #ifdef LINALG_TEST_MAIN */

/**
 * File:          $RCSfile: matf_svd.c,v $
 * Module:        Matrix singular value decomposition (single precision)
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

#include <gandalf/linalg/matf_svd.h>
#include <gandalf/linalg/matf_gen.h>
#include <gandalf/linalg/matf_square.h>
#include <gandalf/linalg/clapack.h>
#include <gandalf/common/misc_error.h>
#include <gandalf/common/compare.h>
#include <gandalf/common/array.h>
#include <gandalf/common/allocate.h>

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
 * \addtogroup GeneralSizeMatrixSVD
 * \{
 */

#ifndef HAVE_LAPACK
typedef struct SVDOrder
{
   long index;
   float sing_val;
} SVDOrder;
 
static int compar ( const void *ptr1, const void *ptr2 )
{
   SVDOrder *i1 = (SVDOrder *) ptr1, *i2 = (SVDOrder *) ptr2;

   return ( (i1->sing_val < i2->sing_val) ? 1 :
            ((i1->sing_val > i2->sing_val) ? -1 : -1) );
}

#endif /* #ifndef HAVE_LAPACK */

/**
 * \brief Computes singular value decomposition of matrix.
 * \param A Pointer to the input matrix
 * \param U Matrix_f of left singular vectors
 * \param S Diagonal matrix of singular values
 * \param VT Matrix_f of transposed right singular vectors
 * \param work Workspace array
 * \param work_size size of the work array
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Computes singular value decomposition of matrix. The SVD of matrix \a A is
 * \f[
 *     A = U\: S\: VT
 * \f]
 * where \a U and \a VT are orthogonal matrices of singular vectors (left and
 * right singular vectors respectively) and \a S is a diagonal matrix of
 * singular values. The singular values are returned in descending order.
 *
 * \a work is a workspace array of size \a work_size, which should be at least
 * \f$ \mbox{max}(3 \mbox{min}(m,n)+\mbox{max}(m,n),5*\mbox{min}(m,n)-4) \f$
 * where \f$ m,n \f$ are the row, column dimensions of \a A. If \a work is
 * passed as \c NULL, it is allocated and freed internally.
 */
Gan_Bool
 gan_matf_svd ( Gan_Matrix_f *A,
                Gan_Matrix_f *U, Gan_SquMatrix_f *S, Gan_Matrix_f *VT,
                float *work, unsigned long work_size )
{
#ifdef HAVE_LAPACK
   long minmn = gan_min2(A->rows,A->cols), info;
   long onei = 1;
   Gan_Bool work_alloc = GAN_FALSE;

   /* set correct sizes of U, S and VT */
   if ( gan_matf_set_dims ( U, A->rows, minmn ) == NULL ||
        gan_squmatf_set_type_size ( S, GAN_DIAGONAL_MATRIX, minmn ) == NULL ||
        gan_matf_set_dims ( VT, minmn, A->cols ) == NULL )
   {
      gan_err_register ( "gan_matf_svd", GAN_ERROR_FAILURE,
                         "faied to set up output matrices");
      return GAN_FALSE;
   }

   /* return immediately if the matrix is zero size */
   if ( A->rows == 0 || A->cols == 0 ) return GAN_TRUE;

   if ( work == NULL )
   {
      /* allocate workspace of minimum size */
      long maxmn = gan_max2(A->rows,A->cols);
#if 0
      /* this is what the CLAPACK docs say is the minimum */
      work_size = gan_max2 ( 3*minmn+maxmn, 5*minmn-4 );
#else
      /* but this actually works (on SGI) */
      work_size = 10*maxmn;
#endif
      work = gan_malloc_array ( float, work_size );
      if ( work == NULL )
      {
         gan_err_flush_trace();
         gan_err_register_with_number ( "gan_matf_svd", GAN_ERROR_MALLOC_FAILED, "", work_size );
         return GAN_FALSE;
      }

      work_alloc = GAN_TRUE;
   }
   else
      work_alloc = GAN_FALSE;

   /* call CLAPACK SVD routine */
   sgesvd_ ( "S", "S", (long *)&A->rows, (long *)&A->cols,
             A->data, (A->rows == 0) ? &onei : (long *)&A->rows,
             S->data, U->data, (A->rows == 0) ? &onei : (long *)&A->rows,
             VT->data, (minmn == 0) ? &onei : &minmn,
             work, (long *)&work_size, &info );

   /* free workspace array if it was allocated */
   if ( work_alloc ) free ( work );

   if ( info != 0 )
   {
      gan_err_flush_trace();
      if ( info < 0 )
      {
         gan_err_register ( "gan_matf_gesvd", GAN_ERROR_CLAPACK_ILLEGAL_ARG,
                            "in sgesvd_()" );
      }
      else /* info > 0 */
         gan_err_register ( "gan_matf_gesvd", GAN_ERROR_NO_CONVERGENCE,
                            "in sgesvd_()" );

      return GAN_FALSE;
   }

#else
   int ccmf_svduv(float *d,float *a,float *u,int m,float *v,int n);
   long i;
   SVDOrder *index;

   /* for CCM SVD, row size must be >= column size, which in our
      matrix element ordering scheme corresponds to the reverse... */
   if ( A->rows < A->cols )
   {
      /* set correct sizes of U, S and VT */
      if ( gan_matf_set_dims ( U, A->rows, A->rows ) == NULL ||
           gan_squmatf_set_type_size ( S, GAN_DIAGONAL_MATRIX, A->rows )
           == NULL ||
           gan_matf_set_dims ( VT, A->cols, A->cols ) == NULL )
      {
         gan_err_register ( "gan_matf_svd", GAN_ERROR_FAILURE,
                            "faied to set up output matrices");
         return GAN_FALSE;
      }

      /* return immediately if the matrix is zero size */
      if ( A->rows == 0 || A->cols == 0 ) return GAN_TRUE;

      /* call CCM SVD routine */
      if ( ccmf_svduv ( S->data, A->data, VT->data, A->cols, U->data, A->rows )
           != 0 )
      {
         gan_err_flush_trace();
         gan_err_register ( "gan_matf_svd", GAN_ERROR_CCMATH_FAILED, "" );
         return GAN_FALSE;
      }

      /* transpose U in-place */
      if ( gan_matf_tpose_i ( U ) == NULL )
      {
         gan_err_register ( "gan_matf_svd", GAN_ERROR_FAILURE, "" );
         return GAN_FALSE;
      }

      /* ignore bottom rows of VT */
      if ( gan_matf_copy_q ( VT, A ) == NULL )
      {
         gan_err_register ( "gan_matf_svd", GAN_ERROR_FAILURE, "" );
         return GAN_FALSE;
      }

      if ( gan_matf_set_dims ( VT, U->rows, VT->cols ) == NULL ||
           gan_matf_insert ( VT, 0, 0, A, 0, 0, VT->rows, VT->cols ) == NULL )
      {
         gan_err_register ( "gan_matf_svd", GAN_ERROR_FAILURE, "" );
         return GAN_FALSE;
      }
   }
   else
   {
      Gan_Matrix_f *AT;

      /* transpose A using temporary matrix */
      if ( (AT = gan_matf_tpose_s(A)) == NULL ||
           gan_matf_copy_q ( AT, A ) == NULL )
      {
         gan_err_register ( "gan_matf_svd", GAN_ERROR_FAILURE, "" );
         return GAN_FALSE;
      }

      gan_matf_free(AT);

      /* set correct sizes of U, S and VT */
      if ( gan_matf_set_dims ( U, A->cols, A->cols ) == NULL ||
           gan_squmatf_set_type_size ( S, GAN_DIAGONAL_MATRIX, A->rows )
           == NULL ||
           gan_matf_set_dims ( VT, A->rows, A->rows ) == NULL )
      {
         gan_err_register ( "gan_matf_svd", GAN_ERROR_FAILURE,
                            "faied to set up output matrices");
         return GAN_FALSE;
      }

      /* return immediately if the matrix is zero size */
      if ( A->rows == 0 || A->cols == 0 ) return GAN_TRUE;

      /* call CCM SVD routine */
      if ( ccmf_svduv ( S->data, A->data, U->data, A->cols, VT->data, A->rows )
           != 0 )
      {
         gan_err_flush_trace();
         gan_err_register ( "gan_matf_svd", GAN_ERROR_CCMATH_FAILED, "" );
         return GAN_FALSE;
      }

      /* transpose U in-place */
      if ( gan_matf_tpose_i ( U ) == NULL )
      {
         gan_err_register ( "gan_matf_svd", GAN_ERROR_FAILURE, "" );
         return GAN_FALSE;
      }

      /* ignore right-hand columns of U */
      if ( gan_matf_copy_q ( U, A ) == NULL )
      {
         gan_err_register ( "gan_matf_svd", GAN_ERROR_FAILURE, "" );
         return GAN_FALSE;
      }

      if ( gan_matf_set_dims ( U, U->rows, VT->cols ) == NULL ||
           gan_matf_insert ( U, 0, 0, A, 0, 0, U->rows, U->cols ) == NULL )
      {
         gan_err_register ( "gan_matf_svd", GAN_ERROR_FAILURE, "" );
         return GAN_FALSE;
      }
   }

   /* sort singular values */
   index = gan_malloc_array(SVDOrder, S->size);
   if ( index == NULL )
   {
      gan_err_flush_trace();
      gan_err_register_with_number ( "gan_matf_svd", GAN_ERROR_MALLOC_FAILED, "", S->size*sizeof(SVDOrder) );
      return GAN_FALSE;
   }

   /* fill initial ordering */
   for ( i = (long)S->size-1; i >= 0; i-- )
   {
      index[i].index = i;
      index[i].sing_val = gan_squmatf_get_el(S,i,i);
   }

   /* sort to obtain permutation */
   qsort ( index, S->size, sizeof(SVDOrder), compar );

   /* check whether existing ordering is correct */
   for ( i = (long)S->size-1; i >= 0; i-- )
      if ( index[i].index != i ) break;

   if ( i >= 0 )
   {
      /* some reordering needed */
      Gan_Matrix_f *Up, *VTp;
      Gan_SquMatrix_f *Sp;
      long ip;

      /* copy existing factors */
      Up  = gan_matf_copy_s(U);
      VTp = gan_matf_copy_s(VT);
      Sp  = gan_squmatf_copy_s(S);
      if ( Up == NULL || VTp == NULL || Sp == NULL )
      {
         gan_err_register ( "gan_matf_svd", GAN_ERROR_FAILURE, "" );
         free(index);
         return GAN_FALSE;
      }

      /* fill matrices, row by row and column by column, ignoring rows and
         columns which are already correct */
      for ( i = (long)S->size-1; i >= 0; i-- )
         if ( (ip = index[i].index) != i )
         {
            /* copy column of U and row of VT from copies we've just made,
               and also set element of S */
            if ( gan_matf_insert ( U, 0, i, Up, 0, ip, U->rows, 1 ) == NULL ||
                 gan_matf_insert ( VT, i, 0, VTp, ip, 0, 1, VT->cols)
                 == NULL ||
                 !gan_squmatf_set_el ( S, i, i,
                                       gan_squmatf_get_el ( Sp, ip, ip ) ) )
            {
               gan_err_register ( "gan_matf_svd", GAN_ERROR_FAILURE, "" );
               free(index);
               return GAN_FALSE;
            }
         }

      /* free temporary copies */
      gan_squmatf_free(Sp);
      gan_matf_free_va ( VTp, Up, NULL );
   }

   free(index);
#endif /* #ifdef HAVE_LAPACK */
   
   return GAN_TRUE;
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

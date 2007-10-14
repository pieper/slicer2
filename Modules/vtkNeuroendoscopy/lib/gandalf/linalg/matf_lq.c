/**
 * File:          $RCSfile: matf_lq.c,v $
 * Module:        Matrix LQ decomposition (single precision)
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

#include <gandalf/linalg/matf_lq.h>
#include <gandalf/linalg/matf_gen.h>
#include <gandalf/linalg/matf_square.h>
#include <gandalf/linalg/matf_triangular.h>
#include <gandalf/linalg/vecf_gen.h>
#include <gandalf/linalg/matvecf_clapack.h>
#include <gandalf/common/allocate.h>
#include <gandalf/common/compare.h>
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

/**
 * \addtogroup GeneralSizeMatrixLQ
 * \{
 */

/**
 * \brief Performs LQ decomposition of matrix.
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Performs LQ decomposition of matrix \a A:
 * \f[
 *    A = L \: Q
 * \f]
 * where \a A is an \f$ m \f$ by \f$ n \f$ matrix where \f$ m \geq n \f$,
 * \a L is a lower triangular matrix and \a Q is an orthogonal matrix.
 * 
 * \a work is a workspace array of size \a work_size, which should
 * be \f$ \geq n+\mbox{max}(1,m) \f$.
 *
 * \a L may be passed as \c NULL, in which case the triangular factor is
 * not computed.
 */
Gan_Bool
 gan_matf_lq ( Gan_Matrix_f *A, Gan_SquMatrix_f *L, Gan_Matrix_f *Q,
               float *work, unsigned long work_size )
{
   Gan_Bool work_alloc = GAN_FALSE;
   Gan_Vector_f tau;
   unsigned long i, j;

   gan_err_test_bool ( A->rows <= A->cols, "gan_matf_lq",
                       GAN_ERROR_INCOMPATIBLE, "restricted to fat matrix" );

   if ( work == NULL )
   {
      /* allocate workspace */
      work_size = A->rows + gan_max2(A->cols,1);
      work = gan_malloc_array ( float, work_size );
      if ( work == NULL )
      {
         gan_err_register_with_number ( "gan_matf_lq", GAN_ERROR_MALLOC_FAILED, "", work_size );
         return GAN_FALSE;
      }

      work_alloc = GAN_TRUE;
   }
   else
      gan_err_test_bool ( work_size >= A->rows + gan_max2(A->cols,1),
                          "gan_matf_lq", GAN_ERROR_INCOMPATIBLE,
                          "workspace array not big enough" );

   /* copy A into Q */
   assert ( Q != NULL );
   gan_matf_copy_q ( A, Q );

   /* form tau vector */
   gan_vecf_form_data ( &tau, A->rows, work, A->rows );

   /* perform LQ decomposition */
   if ( !gan_clapackf_gelqf ( Q, &tau, work+A->rows, work_size-A->rows ) )
   {
      /* failure */
      gan_err_register ( "gan_matf_lq", GAN_ERROR_FAILURE,
                         "LQ decomposition failed" );
      if ( work_alloc ) free ( work );
      return GAN_FALSE;
   }

   if ( L != NULL )
   {
      L = gan_ltmatf_set_size ( L, A->rows );
      if ( L == NULL )
      {
         gan_err_register ( "gan_matf_lq", GAN_ERROR_FAILURE, "" );
         if ( work_alloc ) free ( work );
         return GAN_FALSE;
      }

      /* read off L matrix */
      for ( i = 0; i < A->rows; i++ )
         for ( j = 0; j <= i; j++ )
            gan_squmatf_set_el ( L, i, j, gan_matf_get_el(Q,i,j) );
   }

   /* extract orthogonal Q as product of elementary reflectors */
   if ( !gan_clapackf_orglq ( Q, &tau, work+A->rows, work_size-A->rows ) )
   {
      /* failure */
      gan_err_register ( "gan_matf_lq", GAN_ERROR_FAILURE,
                         "extracting orthogonal factor failed" );
      if ( work_alloc ) free ( work );
      return GAN_FALSE;
   }
   
   /* free workspace if it was allocated */
   if ( work_alloc ) free ( work );

   /* return with success */
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

/**
 * File:          $RCSfile: symmat_eigen.c,v $
 * Module:        Symmetric matrix eigenvalues & eigenvectors (double precision)
 * Part of:       Gandalf Library
 *
 * Revision:      $Revision: 1.1.2.1 $
 * Last edited:   $Date: 2007/10/14 03:16:24 $
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
#include <gandalf/linalg/symmat_eigen.h>
#include <gandalf/linalg/clapack.h>
#include <gandalf/linalg/matvec_blas.h>
#include <gandalf/linalg/matvec_clapack.h>
#include <gandalf/linalg/mat_symmetric.h>
#include <gandalf/linalg/mat_diagonal.h>
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
 * \addtogroup GeneralSizeMatrixEigen
 * \{
 */

/**
 * \brief Forms a symmetric matrix eigendecomposition structure.
 * \param sme A pointer to a structure or NULL
 * \param max_size The envisaged largest size of matrix
 * \param work_data Workspace array for CLAPACK eigendecomposition
 * \param work_data_size Size of work_data array
 * \return Pointer to structure, or \c NULL on failure.
 *
 * Allocates/fills a structure with initial values to compute the
 * eigendecomposition of a sum of vector outer products.
 *
 * \sa gan_symeigen_form(), gan_symeigen_alloc().
 */
Gan_SymMatEigenStruct *
 gan_symeigen_form_data ( Gan_SymMatEigenStruct *sme, long max_size,
                          double *work_data, unsigned long work_data_size )
{
   Gan_Vector *result;

   if ( sme == NULL )
   {
      /* allocate structure dynamically */
      sme = gan_malloc_object(Gan_SymMatEigenStruct);
      if ( sme == NULL )
      {
         gan_err_flush_trace();
         gan_err_register_with_number ( "gan_symeigen_form_data", GAN_ERROR_MALLOC_FAILED, "", sizeof(*sme) );
         return NULL;
      }

      sme->alloc = GAN_TRUE;
   }
   else
      sme->alloc = GAN_FALSE;

   /* allocate matrices */
   if ( gan_symmat_form ( &sme->SxxT, max_size ) == NULL ||
        gan_diagmat_form ( &sme->W, max_size ) == NULL ||
        gan_mat_form ( &sme->Z, max_size, max_size ) == NULL )
   {
      gan_err_register ( "gan_symeigen_form_data", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   if ( work_data == NULL )
      result = gan_vec_form ( &sme->work, 3*max_size );
   else
      result = gan_vec_form_data ( &sme->work, work_data_size,
                                   work_data, work_data_size );

   if ( result == NULL )
   {
      gan_err_register ( "gan_symeigen_form_data", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* clear accumulated outer product to zero */
   if ( !gan_symeigen_reset ( sme, max_size ) )
   {
      gan_err_register ( "gan_symeigen_form_data", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* successful exit */
   return sme;
}

/**
 * \brief Frees a symmetric matrix eigendecomposition structure.
 * \param sme A pointer to a structure
 * \return No value.
 *
 * Frees a structure created to compute the eigenvalues and eigenvectors of a
 * symmetric matrix accumulated as a sum of vector outer products.
 *
 * \sa gan_symeigen_alloc().
 */
void
 gan_symeigen_free ( Gan_SymMatEigenStruct *sme )
{
   /* free fields of structure followed by the structure itself */
   gan_vec_free ( &sme->work );
   gan_mat_free ( &sme->Z );
   gan_squmat_free ( &sme->W );
   gan_squmat_free ( &sme->SxxT );
   if ( sme->alloc ) free ( sme );
}

/**
 * \brief Initialises an accumulated outer product matrix to zero.
 * \param sme A pointer to a structure
 * \param size The size of the matrix
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Initialises an accumulated outer product matrix to zero. The \a size of the
 * matrix is also set.
 *
 * \sa gan_symeigen_alloc().
 */
Gan_Bool
 gan_symeigen_reset ( Gan_SymMatEigenStruct *sme, long size )
{
   gan_symmat_fill_const_q ( &sme->SxxT, size, 0.0 );
   return GAN_TRUE;
}

/**
 * \brief Increments an accumulated outer product matrix.
 * \param sme A pointer to a structure
 * \param scale A scaling for the outer product
 * \param ... A variable argument list of vector components
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Increments an accumulated outer product matrix stored inside the \a sme
 * structure with the outer product of a vector passed into the function in a
 * variable argument list. The outer product is scaled by the given scaling
 * factor \a scale before the matrix is incremented (equivalent to scaling
 * the vector by the square-root of \a scale).
 *
 * \sa gan_symeigen_alloc().
 */
Gan_Bool
 gan_symeigen_increment ( Gan_SymMatEigenStruct *sme, double scale, ... )
{
   va_list ap;
   unsigned long i;

   /* use workspace vector inside structure as temporary vector */
   gan_vec_set_size ( &sme->work, sme->SxxT.size );

   /* read variable argument list into temporary vector */
   va_start ( ap, scale );
   for ( i = 0; i < sme->SxxT.size; i++ )
#if 1
      gan_vec_set_el ( &sme->work, i, va_arg ( ap, double ) );
#else   
      (i >= sme->work.rows ? GAN_FALSE
       : (sme->work.data[i] = 1.0, GAN_TRUE));
#endif
   va_end ( ap );

   /* accumulate outer product sum matrix */
   gan_blas_spr ( &sme->SxxT, &sme->work, scale );

   return GAN_TRUE;
}

/**
 * \brief Eigendecomposes a matrix accumulated by summing outer products.
 * \param sme A pointer to a structure
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Solves for the eigenvalues and eigenvectors of a matrix accumulated within
 * the sme structure as a sum of vector outer products.
 *
 * \sa gan_symeigen_alloc(), gan_symeigen_increment().
 */
Gan_Bool
 gan_symeigen_solve ( Gan_SymMatEigenStruct *sme )
{
   return gan_symmat_eigen ( &sme->SxxT, &sme->W, &sme->Z, GAN_TRUE,
                             sme->work.data, sme->work.data_size );
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

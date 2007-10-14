/**
 * File:          $RCSfile: 3x3matrix_eigsym.c,v $
 * Module:        3x3 symmetric matrix eigenvalues/vectors (double precision)
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

#include <gandalf/linalg/3x3matrix_eigsym.h>
#include <gandalf/linalg/mat_symmetric.h>
#include <gandalf/linalg/mat_diagonal.h>
#include <gandalf/common/numerics.h>
#include <gandalf/common/misc_error.h>

/**
 * \addtogroup LinearAlgebra
 * \{
 */

/**
 * \addtogroup FixedSizeMatVec
 * \{
 */

/**
 * \addtogroup FixedSizeMatrix
 * \{
 */

/**
 * \addtogroup FixedSizeMatrixEigen
 * \{
 */

#define gel(M,i,j)   gan_mat_get_el(M,i,j)
#define sel(M,i,j,v) gan_mat_set_el(M,i,j,v)
#define sgel(M,i,j)   gan_squmat_get_el(M,i,j)
#define ssel(M,i,j,v) gan_squmat_set_el(M,i,j,v)

#ifdef HAVE_LAPACK
#define WORKSPACE_SIZE 200
#else
#define WORKSPACE_SIZE 0
#endif

/**
 * \brief Eigenvalues and maybe eigenvectors of symmetric 3x3 matrix.
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Computes the eigenvalues and, optionally, the eigenvectors of a symmetric
 * 3x3 input matrix \a A, i.e. computing diagonal matrix \a W as the
 * eigenvalues \a W, and orthogonal matrix of eigenvectors \a Z such that
 * \f[
 *   A\:Z = Z\:W, \;\; Z^{\top}\:A = W\:`Z^{\top}
 * \f]
 * If \a Z is passed as \c NULL then the eigenvectors are not computed.
 *
 * NOTE: the contents of \a A are NOT destroyed.
 *
 * The eigenvalues \a W are provided in ascending order.
 */
Gan_Bool
 gan_symmat33_eigen ( Gan_SquMatrix33 *A, Gan_Vector3 *W, Gan_Matrix33 *Z )
{
   Gan_SquMatrix Am, Wm;
   Gan_Matrix    Zm;
   double        Adata[3*(3+1)/2], Wdata[3], Zdata[3*3],
   #ifdef HAVE_LAPACK
                 wkdata[WORKSPACE_SIZE];
   #else
                 wkdata[1]; /* Can't declare an array with 0 elements */
   #endif

   /* convert to general matrices */
   if ( gan_symmat_form_data ( &Am, 3, Adata, 3*(3+1)/2 ) == NULL ||
        gan_diagmat_form_data ( &Wm, 3, Wdata, 3 ) == NULL ||
        ( Z != NULL && gan_mat_form_data ( &Zm, 3, 3, Zdata, 3*3 ) == NULL) )
   {
      gan_err_register ( "gan_symmat33_eigen", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   /* copy data from matrix A into general matrix Am */
   if ( gan_symmat_fill_va ( &Am, 3, A->xx,
                                     A->yx, A->yy,
                                     A->zx, A->zy, A->zz ) == NULL )
   {
      gan_err_register ( "gan_symmat33_eigen", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   /* perform eigendecomposition */
   if ( !gan_symmat_eigen ( &Am, &Wm, (Z == NULL) ? NULL : &Zm, GAN_TRUE,
                            wkdata, WORKSPACE_SIZE ) )
   {
      gan_err_register ( "gan_symmat33_eigen", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   /* copy eigenvalues and eigenvectors */
   gan_vec3_fill_q ( W, sgel(&Wm,0,0), sgel(&Wm,1,1), sgel(&Wm,2,2) );
   if ( Z != NULL )
      gan_mat33_fill_q ( Z,
                         gel(&Zm,0,0), gel(&Zm,0,1), gel(&Zm,0,2),
                         gel(&Zm,1,0), gel(&Zm,1,1), gel(&Zm,1,2),
                         gel(&Zm,2,0), gel(&Zm,2,1), gel(&Zm,2,2) );

   /* success */
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

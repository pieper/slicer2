/**
 * File:          $RCSfile: 3x3matrix_eigen.c,v $
 * Module:        3x3 matrix eigenvalues and eigenvectors (double precision)
 * Part of:       Gandalf Library
 *
 * Revision:      $Revision: 1.1.2.1 $
 * Last edited:   $Date: 2007/10/14 03:16:21 $
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

#include <gandalf/linalg/3x3matrix_eigen.h>
#include <gandalf/linalg/matvec_clapack.h>
#include <gandalf/linalg/mat_gen.h>
#include <gandalf/linalg/mat_square.h>
#include <gandalf/linalg/mat_diagonal.h>
#include <gandalf/linalg/vec_gen.h>

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

#define WORKSPACE_SIZE 100

/**
 * \brief Computes eigenvalues and eigenvectors of a 3x3 matrix.
 * \param A Pointer to the input 3x3 matrix
 * \param W 3-vector of complex eigenvalues
 * \param VL 3x3 matrix of left eigenvectors or NULL
 * \param VR 3x3 matrix of right eigenvectors or NULL
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Computes the eigenvalues and optionally the left and/or right eigenvectors
 * of 3x3 matrix \a A. If either of the eigenvector matrices \a VL or \a VR
 * are \c NULL, the corresponding eigenvectors are not computed.
 *
 * \sa gan_clapack_geev()
 */
Gan_Bool
 gan_mat33_eigen ( Gan_Matrix33 *A, Gan_Complex W[3],
                   Gan_Matrix33 *VL, Gan_Matrix33 *VR )
{
#ifdef HAVE_LAPACK
   Gan_Matrix Am, VLm, VRm;
   Gan_SquMatrix WRm, WIm;
   double Adata[3*3], WRdata[3], WIdata[3], VLdata[3*3], VRdata[3*3],
          wkdata[WORKSPACE_SIZE];

   /* convert to general matrices */
   gan_mat_form_data ( &Am, 3, 3, Adata, 3*3 );
   gan_diagmat_form_data ( &WRm, 3, WRdata, 3 );
   gan_diagmat_form_data ( &WIm, 3, WIdata, 3 );
   if ( VL != NULL )
      gan_mat_form_data ( &VLm, 3, 3, VLdata, 3*3 );

   if ( VR != NULL )
      gan_mat_form_data ( &VRm, 3, 3, VRdata, 3*3 );

   /* copy data from matrix A into general matrix Am */
   gan_mat_fill_va ( &Am, 3, 3, A->xx, A->xy, A->xz,
                                A->yx, A->yy, A->yz,
                                A->zx, A->zy, A->zz );

   /* perform eigendecomposition */
   gan_clapack_geev ( &Am, &WRm, &WIm,
                      (VL == NULL) ? NULL : &VLm, (VR == NULL) ? NULL : &VRm,
                      wkdata, WORKSPACE_SIZE );

   /* copy eigenvalues and eigenvectors */
   W[0].r = sgel(&WRm,0,0); W[1].r = sgel(&WRm,1,1); W[2].r = sgel(&WRm,2,2);
   W[0].i = sgel(&WIm,0,0); W[1].i = sgel(&WIm,1,1); W[2].i = sgel(&WIm,2,2);
   if ( VL != NULL )
      gan_mat33_fill_q ( VL, gel(&VLm,0,0), gel(&VLm,0,1), gel(&VLm,0,2),
                             gel(&VLm,1,0), gel(&VLm,1,1), gel(&VLm,1,2),
                             gel(&VLm,2,0), gel(&VLm,2,1), gel(&VLm,2,2) );

   if ( VR != NULL )
      gan_mat33_fill_q ( VR, gel(&VRm,0,0), gel(&VRm,0,1), gel(&VRm,0,2),
                             gel(&VRm,1,0), gel(&VRm,1,1), gel(&VRm,1,2),
                             gel(&VRm,2,0), gel(&VRm,2,1), gel(&VRm,2,2) );
#else
   assert(0);
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

/**
 * File:          $RCSfile: 3x3matrixf_eigen.c,v $
 * Module:        3x3 matrix eigenvalues and eigenvectors (single precision)
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

#include <gandalf/linalg/3x3matrixf_eigen.h>
#include <gandalf/linalg/matvecf_clapack.h>
#include <gandalf/linalg/matf_gen.h>
#include <gandalf/linalg/matf_square.h>
#include <gandalf/linalg/matf_diagonal.h>
#include <gandalf/linalg/vecf_gen.h>

#define gel(M,i,j)   gan_matf_get_el(M,i,j)
#define sel(M,i,j,v) gan_matf_set_el(M,i,j,v)
#define sgel(M,i,j)   gan_squmatf_get_el(M,i,j)
#define ssel(M,i,j,v) gan_squmatf_set_el(M,i,j,v)

#define WORKSPACE_SIZE 100

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
 * \sa gan_clapackf_geev()
 */
Gan_Bool
 gan_mat33f_eigen ( Gan_Matrix33_f *A, Gan_Complex_f W[3],
                    Gan_Matrix33_f *VL, Gan_Matrix33_f *VR )
{
#ifdef HAVE_LAPACK
   Gan_Matrix_f Am, VLm, VRm;
   Gan_SquMatrix_f WRm, WIm;
   float   Adata[3*3], WRdata[3], WIdata[3], VLdata[3*3], VRdata[3*3],
           wkdata[WORKSPACE_SIZE];

   /* convert to general matrices */
   gan_matf_form_data ( &Am, 3, 3, Adata, 3*3 );
   gan_diagmatf_form_data ( &WRm, 3, WRdata, 3 );
   gan_diagmatf_form_data ( &WIm, 3, WIdata, 3 );
   if ( VL != NULL )
      gan_matf_form_data ( &VLm, 3, 3, VLdata, 3*3 );

   if ( VR != NULL )
      gan_matf_form_data ( &VRm, 3, 3, VRdata, 3*3 );

   /* copy data from matrix A into general matrix Am */
   gan_matf_fill_va ( &Am, 3, 3, (double)A->xx, (double)A->xy, (double)A->xz,
                                 (double)A->yx, (double)A->yy, (double)A->yz,
                                 (double)A->zx, (double)A->zy, (double)A->zz );

   /* perform eigendecomposition */
   gan_clapackf_geev ( &Am, &WRm, &WIm,
                       (VL == NULL) ? NULL : &VLm, (VR == NULL) ? NULL : &VRm,
                       wkdata, WORKSPACE_SIZE );

   /* copy eigenvalues and eigenvectors */
   W[0].r = sgel(&WRm,0,0); W[1].r = sgel(&WRm,1,1); W[2].r = sgel(&WRm,2,2);
   W[0].i = sgel(&WIm,0,0); W[1].i = sgel(&WIm,1,1); W[2].i = sgel(&WIm,2,2);
   if ( VL != NULL )
      gan_mat33f_fill_q ( VL, gel(&VLm,0,0), gel(&VLm,0,1), gel(&VLm,0,2),
                              gel(&VLm,1,0), gel(&VLm,1,1), gel(&VLm,1,2),
                              gel(&VLm,2,0), gel(&VLm,2,1), gel(&VLm,2,2) );

   if ( VR != NULL )
      gan_mat33f_fill_q ( VR, gel(&VRm,0,0), gel(&VRm,0,1), gel(&VRm,0,2),
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

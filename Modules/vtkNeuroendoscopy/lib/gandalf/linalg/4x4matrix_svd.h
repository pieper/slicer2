/**
 * File:          $RCSfile: 4x4matrix_svd.h,v $
 * Module:        4x4 matrix singular value decomposition (double precision)
 * Part of:       Gandalf Library
 *
 * Revision:      $Revision: 1.1.2.1 $
 * Last edited:   $Date: 2007/10/14 03:16:26 $
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

#ifndef _GAN_MATRIX44_SVD_H
#define _GAN_MATRIX44_SVD_H

#include <gandalf/linalg/4x4matrix.h>

#ifdef __cplusplus
extern "C" {
#endif

GANDALF_API Gan_Bool gan_mat44_svd ( Gan_Matrix44 *A,
                         Gan_Matrix44 *U, Gan_Vector4 *W, Gan_Matrix44 *VT );
GANDALF_API Gan_Bool gan_mat44T_svd ( Gan_Matrix44 *A,
                          Gan_Matrix44 *U, Gan_Vector4 *W, Gan_Matrix44 *VT );

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _GAN_MATRIX44_SVD_H */

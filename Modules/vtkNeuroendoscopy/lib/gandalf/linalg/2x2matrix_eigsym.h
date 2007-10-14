/**
 * File:          $RCSfile: 2x2matrix_eigsym.h,v $
 * Module:        2x2 symmetric matrix eigenvalues/vectors (double precision)
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

#ifndef _GAN_MATRIX22_EIGSYM_H
#define _GAN_MATRIX22_EIGSYM_H

#include <gandalf/linalg/2x2matrix.h>

#ifdef __cplusplus
extern "C" {
#endif

GANDALF_API Gan_Bool gan_symmat22_eigen ( Gan_SquMatrix22 *A,
                              Gan_Vector2 *W, Gan_Matrix22 *Z );

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _GAN_MATRIX22_EIGSYM_H */

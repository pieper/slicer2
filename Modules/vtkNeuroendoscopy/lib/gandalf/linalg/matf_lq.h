/**
 * File:          $RCSfile: matf_lq.h,v $
 * Module:        Matrix LQ decomposition (single precision)
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

#ifndef _GAN_MATF_LQ_H
#define _GAN_MATF_LQ_H

#include <gandalf/common/misc_defs.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef GAN_MATRIXF_STRUCT_DEFINED
struct Gan_Matrix_f;
#endif

#ifndef GAN_SQUMATRIXF_STRUCT_DEFINED
struct Gan_SquMatrix_f;
#endif

Gan_Bool gan_matf_lq ( struct Gan_Matrix_f *A,
                       struct Gan_SquMatrix_f *L, struct Gan_Matrix_f *Q,
                       float *work, unsigned long work_size );

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _GAN_MATF_LQ_H */

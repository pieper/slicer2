/**
 * File:          $RCSfile: modified_fht2D.h,v $
 * Module:        Fast Hough Transform implentation
 * Part of:       Gandalf Library
 *
 * Revision:      $Revision: 1.1.2.1 $
 * Last edited:   $Date: 2007/10/14 02:33:20 $
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

#ifndef _GAN_MODIFIED_FHT2D_H
#define _GAN_MODIFIED_FHT2D_H

#include <gandalf/common/bit_array.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup Vision
 * \{
 */

/**
 * \addtogroup HoughTransform
 * \{
 */

GANDALF_API Gan_Bool gan_modified_fht2D ( double *x, double *y, int *weight,
                              int no_points, double m_range, double c_range,
                              double c_root, int max_level, int T_thres,
                              Gan_MemoryStack *memory_stack,
                              double *m_best, double *c_best, int *level_best,
                              int *accum_best, Gan_BitArray *list_best );

/**
 * \}
 */

/**
 * \}
 */

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _GAN_MODIFIED_FHT2D_H */

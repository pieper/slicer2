/**
 * File:          $RCSfile: edge_disp.h,v $
 * Module:        Edge feature display using Open GL
 * Part of:       Gandalf Library
 *
 * Revision:      $Revision: 1.1.2.1 $
 * Last edited:   $Date: 2007/10/14 02:33:18 $
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

#ifndef _GAN_EDGE_DISP_H
#define _GAN_EDGE_DISP_H

#include <gandalf/vision/edge_feature.h>
#include <gandalf/image/pixel.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup Vision
 * \{
 */

/**
 * \addtogroup FeatureDetection
 * \{
 */

#ifdef HAVE_OPENGL

GANDALF_API Gan_Bool gan_edge_feature_display ( Gan_EdgeFeature *edge,
                                    float size, Gan_Matrix23_f *A,
                                    Gan_RGBPixel_f *edge_colour );
GANDALF_API Gan_Bool gan_edge_feature_map_display ( Gan_EdgeFeatureMap *emap,
                                        float size, Gan_Matrix23_f *A,
                                        Gan_RGBPixel_f *edge_colour,
                                        Gan_RGBPixel_f *string_colour,
                                        Gan_RGBPixel_f *first_colour,
                                        Gan_RGBPixel_f *last_colour,
                                        Gan_RGBPixel_f *bbox_colour );

#endif /* #ifdef HAVE_OPENGL */

/**
 * \}
 */

/**
 * \}
 */

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _GAN_EDGE_DISP_H */

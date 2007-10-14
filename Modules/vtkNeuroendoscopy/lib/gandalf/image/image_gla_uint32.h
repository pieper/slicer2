/**
 * File:          $RCSfile: image_gla_uint32.h,v $
 * Module:        grey-level 32-bit images with alpha
 * Part of:       Gandalf Library
 *
 * Revision:      $Revision: 1.1.2.1 $
 * Last edited:   $Date: 2007/10/14 02:20:20 $
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

#ifndef _GAN_IMAGE_GLA_UINT32_H
#define _GAN_IMAGE_GLA_UINT32_H

#include <gandalf/common/misc_defs.h>

#ifdef GAN_UINT32

#ifdef GAN_GENERATE_DOCUMENTATION

#define GAN_PIXEL Gan_GLAPixel_ui32
#define GAN_PIXEL_FORMAT grey-level alpha
#define GAN_PIXEL_TYPE 32-bit unsigned integer

#define GAN_IMAGE_FORM_GEN gan_image_form_gen_gla_ui32
#define GAN_IMAGE_SET_GEN gan_image_set_gen_gla_ui32
#define GAN_IMAGE_ALLOC gan_image_alloc_gla_ui32
#define GAN_IMAGE_ALLOC_DATA gan_image_alloc_data_gla_ui32
#define GAN_IMAGE_FORM gan_image_form_gla_ui32
#define GAN_IMAGE_FORM_DATA gan_image_form_data_gla_ui32
#define GAN_IMAGE_SET gan_image_set_gla_ui32
#define GAN_IMAGE_SET_PIX gan_image_set_pix_gla_ui32
#define GAN_IMAGE_GET_PIX gan_image_get_pix_gla_ui32
#define GAN_IMAGE_GET_PIXPTR gan_image_get_pixptr_gla_ui32
#define GAN_IMAGE_GET_PIXARR gan_image_get_pixarr_gla_ui32
#define GAN_IMAGE_FILL_CONST gan_image_fill_const_gla_ui32
#define GAN_IMAGE_GET_ACTIVE_SUBWINDOW gan_image_get_active_subwindow_gla_ui32
#define GAN_IMAGE_MASK_WINDOW gan_image_mask_window_gla_ui32
#define GAN_IMAGE_CLEAR_WINDOW gan_image_clear_window_gla_ui32

#include <gandalf/image/image_colour_noc.h>

#else

#if (SIZEOF_INT == 4)

#include <gandalf/image/image_gla_uint.h>

#define gan_image_form_gen_gla_ui32(img,h,w,s,a,pdata,pdsize,rdata,rdsize) \
        gan_image_form_gen_gla_ui(img,h,w,s,a,pdata,pdsize,rdata,rdsize)
#define gan_image_set_gen_gla_ui32(img,h,w,s,a) \
        gan_image_set_gen_gla_ui(img,h,w,s,a)
#define gan_image_set_pix_gla_ui32(img,row,col,val) \
        gan_image_set_pix_gla_ui(img,row,col,val)
#define gan_image_get_pix_gla_ui32(img,row,col) \
        gan_image_get_pix_gla_ui(img,row,col)
#define gan_image_get_pixptr_gla_ui32(img,row,col) \
        gan_image_get_pixptr_gla_ui(img,row,col)
#define gan_image_get_pixarr_gla_ui32(img) \
        gan_image_get_pixarr_gla_ui(img)
#define gan_image_fill_const_gla_ui32(img,val) \
        gan_image_fill_const_gla_ui(img,val)
#define gan_image_alloc_gla_ui32(h,w) gan_image_alloc_gla_ui(h,w)
#define gan_image_form_gla_ui32(img,h,w) gan_image_form_gla_ui(img,h,w)
#define gan_image_alloc_data_gla_ui32(h,w,s,pd,pds,rd,rds) \
        gan_image_alloc_data_gla_ui(h,w,s,pd,pds,rd,rds)
#define gan_image_form_data_gla_ui32(img,h,w,s,pd,pds,rd,rds) \
        gan_image_form_data_gla_ui(img,h,w,s,pd,pds,rd,rds)
#define gan_image_set_gla_ui32(img,h,w) gan_image_set_gla_ui(img,h,w)
#define gan_assert_image_gla_ui32(img) gan_assert_image_gla_ui(img)

#else

#error "32-bit type conflict"

#endif /* #if (SIZEOF_INT == 4) */

#endif /* #ifdef GAN_GENERATE_DOCUMENTATION */

#endif /* #ifdef GAN_UINT32 */

#endif /* #ifndef _GAN_IMAGE_GLA_UINT32_H */

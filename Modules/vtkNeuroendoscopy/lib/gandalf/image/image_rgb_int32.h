/**
 * File:          $RCSfile: image_rgb_int32.h,v $
 * Module:        RGB colour signed 32-bit images
 * Part of:       Gandalf Library
 *
 * Revision:      $Revision: 1.1.2.1 $
 * Last edited:   $Date: 2007/10/14 02:20:24 $
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

#ifndef _GAN_IMAGE_RGB_INT32_H
#define _GAN_IMAGE_RGB_INT32_H

#include <gandalf/common/misc_defs.h>

#ifdef GAN_INT32

#ifdef GAN_GENERATE_DOCUMENTATION

#define GAN_PIXEL Gan_RGBPixel_i32
#define GAN_PIXEL_FORMAT RGB colour
#define GAN_PIXEL_TYPE 32-bit signed integer

#define GAN_IMAGE_FORM_GEN gan_image_form_gen_rgb_i32
#define GAN_IMAGE_SET_GEN gan_image_set_gen_rgb_i32
#define GAN_IMAGE_ALLOC gan_image_alloc_rgb_i32
#define GAN_IMAGE_ALLOC_DATA gan_image_alloc_data_rgb_i32
#define GAN_IMAGE_FORM gan_image_form_rgb_i32
#define GAN_IMAGE_FORM_DATA gan_image_form_data_rgb_i32
#define GAN_IMAGE_SET gan_image_set_rgb_i32
#define GAN_IMAGE_SET_PIX gan_image_set_pix_rgb_i32
#define GAN_IMAGE_GET_PIX gan_image_get_pix_rgb_i32
#define GAN_IMAGE_GET_PIXPTR gan_image_get_pixptr_rgb_i32
#define GAN_IMAGE_GET_PIXARR gan_image_get_pixarr_rgb_i32
#define GAN_IMAGE_FILL_CONST gan_image_fill_const_rgb_i32
#define GAN_IMAGE_GET_ACTIVE_SUBWINDOW gan_image_get_active_subwindow_rgb_i32
#define GAN_IMAGE_MASK_WINDOW gan_image_mask_window_rgb_i32
#define GAN_IMAGE_CLEAR_WINDOW gan_image_clear_window_rgb_i32

#include <gandalf/image/image_colour_noc.h>

#else

#if (SIZEOF_INT == 4)

#include <gandalf/image/image_rgb_int.h>

#define gan_image_form_gen_rgb_i32(img,h,w,s,a,pdata,pdsize,rdata,rdsize) \
        gan_image_form_gen_rgb_i(img,h,w,s,a,pdata,pdsize,rdata,rdsize)
#define gan_image_set_gen_rgb_i32(img,h,w,s,a) \
        gan_image_set_gen_rgb_i(img,h,w,s,a)
#define gan_image_set_pix_rgb_i32(img,row,col,val) \
        gan_image_set_pix_rgb_i(img,row,col,val)
#define gan_image_get_pix_rgb_i32(img,row,col) \
        gan_image_get_pix_rgb_i(img,row,col)
#define gan_image_get_pixptr_rgb_i32(img,row,col) \
        gan_image_get_pixptr_rgb_i(img,row,col)
#define gan_image_get_pixarr_rgb_i32(img) \
        gan_image_get_pixarr_rgb_i(img)
#define gan_image_fill_const_rgb_i32(img,val) \
        gan_image_fill_const_rgb_i(img,val)
#define gan_image_alloc_rgb_i32(h,w) gan_image_alloc_rgb_i(h,w)
#define gan_image_form_rgb_i32(img,h,w) gan_image_form_rgb_i(img,h,w)
#define gan_image_alloc_data_rgb_i32(h,w,s,pd,pds,rd,rds) \
        gan_image_alloc_data_rgb_i(h,w,s,pd,pds,rd,rds)
#define gan_image_form_data_rgb_i32(img,h,w,s,pd,pds,rd,rds) \
        gan_image_form_data_rgb_i(img,h,w,s,pd,pds,rd,rds)
#define gan_image_set_rgb_i32(img,h,w) gan_image_set_rgb_i(img,h,w)
#define gan_assert_image_grey_i32(img) gan_assert_image_grey_i(img)

#else

#error "32-bit type conflict"

#endif /* #if (SIZEOF_INT == 4) */

#endif /* #ifdef GAN_GENERATE_DOCUMENTATION */

#endif /* #ifdef GAN_INT32 */

#endif /* #ifndef _GAN_IMAGE_RGB_INT32_H */

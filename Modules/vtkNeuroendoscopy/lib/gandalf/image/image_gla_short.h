/**
 * File:          $RCSfile: image_gla_short.h,v $
 * Module:        Grey level signed short integer images with alpha channel
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

#ifndef _GAN_IMAGE_GLA_SHORT_H
#define _GAN_IMAGE_GLA_SHORT_H

#define GAN_PIXEL struct Gan_GLAPixel_s
#define GAN_PIXEL_FORMAT grey-level alpha
#define GAN_PIXEL_TYPE short
#define GAN_IMTYPE s
#define GAN_IMAGE_FORM_GEN gan_image_form_gen_gla_s
#define GAN_IMAGE_SET_GEN gan_image_set_gen_gla_s
#define GAN_IMAGE_ALLOC gan_image_alloc_gla_s
#define GAN_IMAGE_ALLOC_DATA gan_image_alloc_data_gla_s
#define GAN_IMAGE_FORM gan_image_form_gla_s
#define GAN_IMAGE_FORM_DATA gan_image_form_data_gla_s
#define GAN_IMAGE_SET gan_image_set_gla_s
#define GAN_IMAGE_SET_PIX gan_image_set_pix_gla_s
#define GAN_IMAGE_GET_PIX gan_image_get_pix_gla_s
#define GAN_IMAGE_GET_PIXPTR gan_image_get_pixptr_gla_s
#define GAN_IMAGE_GET_PIXARR gan_image_get_pixarr_gla_s
#define GAN_IMAGE_FILL_CONST gan_image_fill_const_gla_s
#define GAN_IMAGE_GET_ACTIVE_SUBWINDOW gan_image_get_active_subwindow_gla_s
#define GAN_IMAGE_MASK_WINDOW gan_image_mask_window_gla_s
#define GAN_IMAGE_CLEAR_WINDOW gan_image_clear_window_gla_s

#include <gandalf/image/image_colour_noc.h>

#ifndef GAN_GENERATE_DOCUMENTATION
#define gan_image_alloc_gla_s(h,w)\
           gan_image_form_gen_gla_s(NULL,h,w,(w)*sizeof(Gan_GLAPixel_s),GAN_TRUE,NULL,0,NULL,0)
#define gan_image_form_gla_s(img,h,w)\
           gan_image_form_gen_gla_s(img,h,w,(w)*sizeof(Gan_GLAPixel_s),GAN_TRUE,NULL,0,NULL,0)
#define gan_image_alloc_data_gla_s(h,w,s,pd,pds,rd,rds)\
           gan_image_form_gen_gla_s(NULL,h,w,s,GAN_FALSE,pd,pds,rd,rds)
#define gan_image_form_data_gla_s(img,h,w,s,pd,pds,rd,rds)\
           gan_image_form_gen_gla_s(img,h,w,s,GAN_FALSE,pd,pds,rd,rds)
#define gan_image_set_gla_s(img,h,w)\
           gan_image_set_gen_gla_s(img,h,w,(w)*sizeof(Gan_GLAPixel_s),GAN_TRUE)
#define gan_assert_image_gla_s(img)\
        (assert((img)->format == GAN_GREY_LEVEL_ALPHA_IMAGE &&\
                (img)->type == GAN_SHORT))
#ifdef NDEBUG
#define gan_image_set_pix_gla_s(img,row,col,val)\
       ((img)->row_data.gla.s[row][col]=*(val),GAN_TRUE)
#define gan_image_get_pix_gla_s(img,row,col) \
       ((img)->row_data.gla.s[row][col])
#define gan_image_get_pixptr_gla_s(img,row,col) (&(img)->row_data.gla.s[row][col])
#define gan_image_get_pixarr_gla_s(img) ((img)->row_data.gla.s)
#else
#define gan_image_set_pix_gla_s(img,row,col,val)\
       (gan_assert_image_gla_s(img), (img)->set_pix.gla.s(img,row,col,val))
#define gan_image_get_pix_gla_s(img,row,col)\
       (gan_assert_image_gla_s(img), (img)->get_pix.gla.s(img,row,col))
#endif
#define gan_image_fill_const_gla_s(img,val)\
       (gan_assert_image_gla_s(img),\
        (img)->fill_const.gla.s(img,val))
#endif /* #ifndef GAN_GENERATE_DOCUMENTATION */

#endif /* #ifndef _GAN_IMAGE_GLA_SHORT_H */

/**
 * File:          $RCSfile: image_gl_uint.h,v $
 * Module:        Grey level unsigned int images
 * Part of:       Gandalf Library
 *
 * Revision:      $Revision: 1.1.2.1 $
 * Last edited:   $Date: 2007/10/14 02:20:22 $
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

#ifndef _GAN_IMAGE_GL_UINT_H
#define _GAN_IMAGE_GL_UINT_H

#define GAN_PIXEL unsigned int
#define GAN_PIXEL_FORMAT grey-level
#define GAN_PIXEL_TYPE unsigned int
#define GAN_IMTYPE ui
#define GAN_IMAGE_FORM_GEN gan_image_form_gen_gl_ui
#define GAN_IMAGE_SET_GEN gan_image_set_gen_gl_ui
#define GAN_IMAGE_ALLOC gan_image_alloc_gl_ui
#define GAN_IMAGE_ALLOC_DATA gan_image_alloc_data_gl_ui
#define GAN_IMAGE_FORM gan_image_form_gl_ui
#define GAN_IMAGE_FORM_DATA gan_image_form_data_gl_ui
#define GAN_IMAGE_SET gan_image_set_gl_ui
#define GAN_IMAGE_SET_PIX gan_image_set_pix_gl_ui
#define GAN_IMAGE_GET_PIX gan_image_get_pix_gl_ui
#define GAN_IMAGE_GET_PIXPTR gan_image_get_pixptr_gl_ui
#define GAN_IMAGE_GET_PIXARR gan_image_get_pixarr_gl_ui
#define GAN_IMAGE_FILL_CONST gan_image_fill_const_gl_ui
#define GAN_IMAGE_GET_ACTIVE_SUBWINDOW gan_image_get_active_subwindow_gl_ui
#define GAN_IMAGE_MASK_WINDOW gan_image_mask_window_gl_ui
#define GAN_IMAGE_CLEAR_WINDOW gan_image_clear_window_gl_ui
#define GAN_IMAGE_GET_MINIMUM_PIXEL gan_image_get_minimum_pixel_gl_ui
#define GAN_IMAGE_GET_MAXIMUM_PIXEL gan_image_get_maximum_pixel_gl_ui

#include <gandalf/image/image_grey_noc.h>

#ifndef GAN_GENERATE_DOCUMENTATION
#define gan_image_alloc_gl_ui(h,w)\
           gan_image_form_gen_gl_ui(NULL,h,w,(w)*sizeof(unsigned int),GAN_TRUE,NULL,0,NULL,0)
#define gan_image_form_gl_ui(img,h,w)\
           gan_image_form_gen_gl_ui(img,h,w,(w)*sizeof(unsigned int),GAN_TRUE,NULL,0,NULL,0)
#define gan_image_alloc_data_gl_ui(h,w,s,pd,pds,rd,rds)\
           gan_image_form_gen_gl_ui(NULL,h,w,s,GAN_FALSE,pd,pds,rd,rds)
#define gan_image_form_data_gl_ui(img,h,w,s,pd,pds,rd,rds)\
           gan_image_form_gen_gl_ui(img,h,w,s,GAN_FALSE,pd,pds,rd,rds)
#define gan_image_set_gl_ui(img,h,w) gan_image_set_gen_gl_ui(img,h,w,(w)*sizeof(unsigned int),GAN_TRUE)
#define gan_assert_image_grey_ui(img)\
        (assert((img)->format == GAN_GREY_LEVEL_IMAGE &&\
                (img)->type == GAN_UINT))
#ifdef NDEBUG
#define gan_image_set_pix_gl_ui(img,row,col,val)\
       ((img)->row_data.gl.ui[row][col]=(val),GAN_TRUE)
#define gan_image_get_pix_gl_ui(img,row,col) ((img)->row_data.gl.ui[row][col])
#define gan_image_get_pixptr_gl_ui(img,row,col) (&(img)->row_data.gl.ui[row][col])
#define gan_image_get_pixarr_gl_ui(img) ((img)->row_data.gl.ui)
#else
#define gan_image_set_pix_gl_ui(img,row,col,val)\
       (gan_assert_image_grey_ui(img), (img)->set_pix.gl.ui(img,row,col,val))
#define gan_image_get_pix_gl_ui(img,row,col)\
       (gan_assert_image_grey_ui(img), (img)->get_pix.gl.ui(img,row,col))
#endif
#define gan_image_fill_const_gl_ui(img,val)\
       (gan_assert_image_grey_ui(img), (img)->fill_const.gl.ui(img,val))
#endif /* #ifndef GAN_GENERATE_DOCUMENTATION */

#endif /* #ifndef _GAN_IMAGE_GL_UINT_H */

/**
 * File:          $RCSfile: image_defs.c,v $
 * Module:        Image definitions and general functions
 * Part of:       Gandalf Library
 *
 * Revision:      $Revision: 1.1.2.1 $
 * Last edited:   $Date: 2007/10/14 02:20:18 $
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

#include <stdarg.h>
#include <string.h>
#include <gandalf/image/image_defs.h>
#include <gandalf/image/image_gl_uchar.h>
#include <gandalf/image/image_gl_short.h>
#include <gandalf/image/image_gl_ushort.h>
#include <gandalf/image/image_gl_int.h>
#include <gandalf/image/image_gl_uint.h>
#include <gandalf/image/image_gl_float.h>
#include <gandalf/image/image_gl_double.h>
#include <gandalf/image/image_bit.h>
#include <gandalf/image/image_pointer.h>
#include <gandalf/image/image_gla_uchar.h>
#include <gandalf/image/image_gla_short.h>
#include <gandalf/image/image_gla_ushort.h>
#include <gandalf/image/image_gla_int.h>
#include <gandalf/image/image_gla_uint.h>
#include <gandalf/image/image_gla_float.h>
#include <gandalf/image/image_gla_double.h>
#include <gandalf/image/image_rgb_uchar.h>
#include <gandalf/image/image_rgb_short.h>
#include <gandalf/image/image_rgb_ushort.h>
#include <gandalf/image/image_rgb_int.h>
#include <gandalf/image/image_rgb_uint.h>
#include <gandalf/image/image_rgb_float.h>
#include <gandalf/image/image_rgb_double.h>
#include <gandalf/image/image_rgba_uchar.h>
#include <gandalf/image/image_rgba_short.h>
#include <gandalf/image/image_rgba_ushort.h>
#include <gandalf/image/image_rgba_int.h>
#include <gandalf/image/image_rgba_uint.h>
#include <gandalf/image/image_rgba_float.h>
#include <gandalf/image/image_rgba_double.h>
#include <gandalf/image/image_bgr_uchar.h>
#include <gandalf/image/image_bgr_short.h>
#include <gandalf/image/image_bgr_ushort.h>
#include <gandalf/image/image_bgr_int.h>
#include <gandalf/image/image_bgr_uint.h>
#include <gandalf/image/image_bgr_float.h>
#include <gandalf/image/image_bgr_double.h>
#include <gandalf/image/image_bgra_uchar.h>
#include <gandalf/image/image_bgra_short.h>
#include <gandalf/image/image_bgra_ushort.h>
#include <gandalf/image/image_bgra_int.h>
#include <gandalf/image/image_bgra_uint.h>
#include <gandalf/image/image_bgra_float.h>
#include <gandalf/image/image_bgra_double.h>
#include <gandalf/image/image_vfield2D_float.h>
#include <gandalf/image/image_vfield2D_double.h>
#include <gandalf/image/image_vfield2D_short.h>
#include <gandalf/image/image_vfield2D_int.h>
#include <gandalf/image/image_vfield3D_float.h>
#include <gandalf/image/image_vfield3D_double.h>
#include <gandalf/image/image_vfield3D_short.h>
#include <gandalf/image/image_vfield3D_int.h>
#include <gandalf/image/image_extract.h>
#include <gandalf/common/bit_array.h>
#include <gandalf/common/misc_error.h>
#include <gandalf/common/compare.h>

/* specialised formats */
#include <gandalf/image/image_rgbx_uint8.h>
#include <gandalf/image/image_rgbx_uint10.h>
#include <gandalf/image/image_rgbas_uint10.h>
#include <gandalf/image/image_rgba_uint12.h>
#include <gandalf/image/image_yuvx444_uint8.h>
#include <gandalf/image/image_yuva444_uint8.h>
#include <gandalf/image/image_yuv422_uint8.h>
#include <gandalf/image/image_gl_uint10.h>
#include <gandalf/image/image_gl_uint12.h>

/**
 * \addtogroup ImagePackage
 * \{
 */

/**
 * \addtogroup ImageAllocate
 * \{
 */

/**
 * \brief Form image.
 *
 * Forms and returns an image \a img with given \a format, \a type,
 * dimensions \a height and \a width. If the provided \a pix_data and/or
 * \a row_data pointers are \c NULL (in any combination), the pixel data array
 * and/or the row start pointer array are dynamically allocated.
 * If either are not \c NULL they must be big enough to hold the pixel/row
 * pointer array data. If necessary use gan_image_data_size() to compute the
 * necessary size of the \a pix_data array (the necessary size of \a row_data
 * is \a height pointers).
 *
 * The \a stride indicates the separation in memory between adjacent rows
 * of the image, and is measured in bytes. Without any padding of the image it
 * will correspond to \a width pixels, in which case you should use the
 * gan_image_form_data() macro instead.
 *
 * If you want to dynamically allocate the pixel data array and/or the
 * row pointer array with sizes greater than that necessary for the
 * given \a height and \a width, pass \a pix_data and/or \a row_data
 * as \c NULL and set \a pix_data_size and/or \a row_data_size appropriately.
 *
 * See also gan_image_alloc() and gan_image_form().
 */
Gan_Image *
 gan_image_form_gen ( Gan_Image *img,
                      Gan_ImageFormat format, Gan_Type type,
                      unsigned long height, unsigned long width,
                      unsigned long stride,
                      Gan_Bool alloc_pix_data,
                      void *pix_data, size_t pix_data_size,
                      void *row_data, size_t row_data_size )
{
   gan_heap_push();
   switch ( format )
   {
      case GAN_GREY_LEVEL_IMAGE:
        switch ( type )
        {
           case GAN_UCHAR:
             img = gan_image_form_gen_gl_uc ( img, height, width, stride,
                                              alloc_pix_data,
                                              (unsigned char *)  pix_data, pix_data_size,
                                              (unsigned char **) row_data, row_data_size );
             break;

           case GAN_SHORT:
             img = gan_image_form_gen_gl_s ( img, height, width, stride,
                                             alloc_pix_data,
                                             (short *)  pix_data, pix_data_size,
                                             (short **) row_data, row_data_size );
             break;

           case GAN_USHORT:
             img = gan_image_form_gen_gl_us ( img, height, width, stride,
                                              alloc_pix_data,
                                              (unsigned short *)  pix_data, pix_data_size,
                                              (unsigned short **) row_data, row_data_size );
             break;

           case GAN_INT:
             img = gan_image_form_gen_gl_i ( img, height, width, stride,
                                             alloc_pix_data,
                                             (int *)  pix_data, pix_data_size,
                                             (int **) row_data, row_data_size );
             break;

           case GAN_UINT:
             img = gan_image_form_gen_gl_ui ( img, height, width, stride,
                                              alloc_pix_data,
                                              (unsigned int *)  pix_data, pix_data_size,
                                              (unsigned int **) row_data, row_data_size );
             break;

           case GAN_FLOAT:
             img = gan_image_form_gen_gl_f ( img, height, width, stride,
                                             alloc_pix_data,
                                             (float *)  pix_data, pix_data_size,
                                             (float **) row_data, row_data_size);
             break;

           case GAN_DOUBLE:
             img = gan_image_form_gen_gl_d (img, height, width, stride,
                                            alloc_pix_data,
                                            (double *)  pix_data, pix_data_size,
                                            (double **) row_data, row_data_size);
             break;

           case GAN_BOOL:
             img = gan_image_form_gen_b ( img, height, width, stride,
                                          alloc_pix_data,
                                          (Gan_BitWord  *) pix_data,  pix_data_size,
                                          (Gan_BitWord **) row_data, row_data_size );
             break;

           case GAN_POINTER:
             img = gan_image_form_gen_p ( img, height, width, stride,
                                          alloc_pix_data,
                                          (void **)  pix_data, pix_data_size,
                                          (void ***) row_data, row_data_size);
             break;

           case GAN_UINT10:
             img = gan_image_form_gen_gl_ui10 (img, height, width, stride,
                                               alloc_pix_data,
                                               (Gan_YXPixel_ui10 *)  pix_data, pix_data_size,
                                               (Gan_YXPixel_ui10 **) row_data, row_data_size);
             break;

           case GAN_UINT12:
             img = gan_image_form_gen_gl_ui12 (img, height, width, stride,
                                               alloc_pix_data,
                                               (Gan_YXPixel_ui12 *)  pix_data, pix_data_size,
                                               (Gan_YXPixel_ui12 **) row_data, row_data_size);
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_form_gen", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             return NULL;
        }
        break;

      case GAN_GREY_LEVEL_ALPHA_IMAGE:
        switch ( type )
        {
           case GAN_UCHAR:
             img = gan_image_form_gen_gla_uc ( img, height, width, stride,
                                               alloc_pix_data,
                                               (Gan_GLAPixel_uc *)  pix_data, pix_data_size,
                                               (Gan_GLAPixel_uc **) row_data, row_data_size );
             break;

           case GAN_SHORT:
             img = gan_image_form_gen_gla_s ( img, height, width, stride,
                                              alloc_pix_data,
                                              (Gan_GLAPixel_s *)  pix_data, pix_data_size,
                                              (Gan_GLAPixel_s **) row_data, row_data_size );
             break;

           case GAN_USHORT:
             img = gan_image_form_gen_gla_us ( img, height, width, stride,
                                               alloc_pix_data,
                                               (Gan_GLAPixel_us *)  pix_data, pix_data_size,
                                               (Gan_GLAPixel_us **) row_data, row_data_size );
             break;

           case GAN_INT:
             img = gan_image_form_gen_gla_i ( img, height, width, stride,
                                              alloc_pix_data,
                                              (Gan_GLAPixel_i *)  pix_data, pix_data_size,
                                              (Gan_GLAPixel_i **) row_data, row_data_size );
             break;

           case GAN_UINT:
             img = gan_image_form_gen_gla_ui ( img, height, width, stride,
                                               alloc_pix_data,
                                               (Gan_GLAPixel_ui *)  pix_data, pix_data_size,
                                               (Gan_GLAPixel_ui **) row_data, row_data_size );
             break;

           case GAN_FLOAT:
             img = gan_image_form_gen_gla_f ( img, height, width, stride,
                                              alloc_pix_data,
                                              (Gan_GLAPixel_f *)  pix_data, pix_data_size,
                                              (Gan_GLAPixel_f **) row_data, row_data_size );
             break;

           case GAN_DOUBLE:
             img = gan_image_form_gen_gla_d ( img, height, width, stride,
                                              alloc_pix_data,
                                              (Gan_GLAPixel_d *)  pix_data, pix_data_size,
                                              (Gan_GLAPixel_d **) row_data, row_data_size );
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_form_gen",
                                GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             return NULL;
        }
        break;

      case GAN_RGB_COLOUR_IMAGE:
        switch ( type )
        {
           case GAN_UCHAR:
             img = gan_image_form_gen_rgb_uc ( img, height, width, stride,
                                               alloc_pix_data,
                                               (Gan_RGBPixel_uc *)  pix_data, pix_data_size,
                                               (Gan_RGBPixel_uc **) row_data, row_data_size );
             break;

           case GAN_SHORT:
             img = gan_image_form_gen_rgb_s ( img, height, width, stride,
                                              alloc_pix_data,
                                              (Gan_RGBPixel_s *)  pix_data, pix_data_size,
                                              (Gan_RGBPixel_s **) row_data, row_data_size );
             break;

           case GAN_USHORT:
             img = gan_image_form_gen_rgb_us ( img, height, width, stride,
                                               alloc_pix_data,
                                               (Gan_RGBPixel_us *)  pix_data, pix_data_size,
                                               (Gan_RGBPixel_us **) row_data, row_data_size );
             break;

           case GAN_INT:
             img = gan_image_form_gen_rgb_i ( img, height, width, stride,
                                              alloc_pix_data,
                                              (Gan_RGBPixel_i *)  pix_data, pix_data_size,
                                              (Gan_RGBPixel_i **) row_data, row_data_size );
             break;

           case GAN_UINT:
             img = gan_image_form_gen_rgb_ui ( img, height, width, stride,
                                               alloc_pix_data,
                                               (Gan_RGBPixel_ui *)  pix_data, pix_data_size,
                                               (Gan_RGBPixel_ui **) row_data, row_data_size );
             break;

           case GAN_FLOAT:
             img = gan_image_form_gen_rgb_f ( img, height, width, stride,
                                              alloc_pix_data,
                                              (Gan_RGBPixel_f *)  pix_data, pix_data_size,
                                              (Gan_RGBPixel_f **) row_data, row_data_size );
             break;

           case GAN_DOUBLE:
             img = gan_image_form_gen_rgb_d ( img, height, width, stride,
                                              alloc_pix_data,
                                              (Gan_RGBPixel_d *)  pix_data, pix_data_size,
                                              (Gan_RGBPixel_d **) row_data, row_data_size );
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_form_gen",
                                GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             return NULL;
        }
        break;

      case GAN_RGB_COLOUR_ALPHA_IMAGE:
        switch ( type )
        {
           case GAN_UCHAR:
             img = gan_image_form_gen_rgba_uc ( img, height, width, stride,
                                                alloc_pix_data,
                                                (Gan_RGBAPixel_uc *)  pix_data, pix_data_size,
                                                (Gan_RGBAPixel_uc **) row_data, row_data_size );
             break;

           case GAN_SHORT:
             img = gan_image_form_gen_rgba_s ( img, height, width, stride,
                                               alloc_pix_data,
                                               (Gan_RGBAPixel_s *)  pix_data, pix_data_size,
                                               (Gan_RGBAPixel_s **) row_data, row_data_size );
             break;

           case GAN_USHORT:
             img = gan_image_form_gen_rgba_us ( img, height, width, stride,
                                                alloc_pix_data,
                                                (Gan_RGBAPixel_us *)  pix_data, pix_data_size,
                                                (Gan_RGBAPixel_us **) row_data, row_data_size );
             break;

           case GAN_INT:
             img = gan_image_form_gen_rgba_i ( img, height, width, stride,
                                               alloc_pix_data,
                                               (Gan_RGBAPixel_i *)  pix_data, pix_data_size,
                                               (Gan_RGBAPixel_i **) row_data, row_data_size );
             break;

           case GAN_UINT:
             img = gan_image_form_gen_rgba_ui ( img, height, width, stride,
                                                alloc_pix_data,
                                                (Gan_RGBAPixel_ui *)  pix_data, pix_data_size,
                                                (Gan_RGBAPixel_ui **) row_data, row_data_size );
             break;

           case GAN_FLOAT:
             img = gan_image_form_gen_rgba_f ( img, height, width, stride,
                                               alloc_pix_data,
                                               (Gan_RGBAPixel_f *)  pix_data, pix_data_size,
                                               (Gan_RGBAPixel_f **) row_data, row_data_size );
             break;

           case GAN_DOUBLE:
             img = gan_image_form_gen_rgba_d ( img, height, width, stride,
                                               alloc_pix_data,
                                               (Gan_RGBAPixel_d *)  pix_data, pix_data_size,
                                               (Gan_RGBAPixel_d **) row_data, row_data_size );
             break;

           case GAN_UINT12:
             img = gan_image_form_gen_rgba_ui12 ( img, height, width, stride,
                                                  alloc_pix_data,
                                                  (Gan_RGBAPixel_ui12 *)  pix_data, pix_data_size,
                                                  (Gan_RGBAPixel_ui12 **) row_data, row_data_size );
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_form_gen",
                                GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             return NULL;
        }
        break;

      case GAN_BGR_COLOUR_IMAGE:
        switch ( type )
        {
           case GAN_UCHAR:
             img = gan_image_form_gen_bgr_uc ( img, height, width, stride,
                                               alloc_pix_data,
                                               (Gan_BGRPixel_uc *)  pix_data, pix_data_size,
                                               (Gan_BGRPixel_uc **) row_data, row_data_size );
             break;

           case GAN_SHORT:
             img = gan_image_form_gen_bgr_s ( img, height, width, stride,
                                              alloc_pix_data,
                                              (Gan_BGRPixel_s *)  pix_data, pix_data_size,
                                              (Gan_BGRPixel_s **) row_data, row_data_size );
             break;

           case GAN_USHORT:
             img = gan_image_form_gen_bgr_us ( img, height, width, stride,
                                               alloc_pix_data,
                                               (Gan_BGRPixel_us *)  pix_data, pix_data_size,
                                               (Gan_BGRPixel_us **) row_data, row_data_size );
             break;

           case GAN_INT:
             img = gan_image_form_gen_bgr_i ( img, height, width, stride,
                                              alloc_pix_data,
                                              (Gan_BGRPixel_i *)  pix_data, pix_data_size,
                                              (Gan_BGRPixel_i **) row_data, row_data_size );
             break;

           case GAN_UINT:
             img = gan_image_form_gen_bgr_ui ( img, height, width, stride,
                                               alloc_pix_data,
                                               (Gan_BGRPixel_ui *)  pix_data, pix_data_size,
                                               (Gan_BGRPixel_ui **) row_data, row_data_size );
             break;

           case GAN_FLOAT:
             img = gan_image_form_gen_bgr_f ( img, height, width, stride,
                                              alloc_pix_data,
                                              (Gan_BGRPixel_f *)  pix_data, pix_data_size,
                                              (Gan_BGRPixel_f **) row_data, row_data_size );
             break;

           case GAN_DOUBLE:
             img = gan_image_form_gen_bgr_d ( img, height, width, stride,
                                              alloc_pix_data,
                                              (Gan_BGRPixel_d *)  pix_data, pix_data_size,
                                              (Gan_BGRPixel_d **) row_data, row_data_size );
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_form_gen",
                                GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             return NULL;
        }
        break;

      case GAN_BGR_COLOUR_ALPHA_IMAGE:
        switch ( type )
        {
           case GAN_UCHAR:
             img = gan_image_form_gen_bgra_uc ( img, height, width, stride,
                                                alloc_pix_data,
                                                (Gan_BGRAPixel_uc *)  pix_data, pix_data_size,
                                                (Gan_BGRAPixel_uc **) row_data, row_data_size );
             break;

           case GAN_SHORT:
             img = gan_image_form_gen_bgra_s ( img, height, width, stride,
                                               alloc_pix_data,
                                               (Gan_BGRAPixel_s *)  pix_data, pix_data_size,
                                               (Gan_BGRAPixel_s **) row_data, row_data_size );
             break;

           case GAN_USHORT:
             img = gan_image_form_gen_bgra_us ( img, height, width, stride,
                                                alloc_pix_data,
                                                (Gan_BGRAPixel_us *)  pix_data, pix_data_size,
                                                (Gan_BGRAPixel_us **) row_data, row_data_size );
             break;

           case GAN_INT:
             img = gan_image_form_gen_bgra_i ( img, height, width, stride,
                                               alloc_pix_data,
                                               (Gan_BGRAPixel_i *)  pix_data, pix_data_size,
                                               (Gan_BGRAPixel_i **) row_data, row_data_size );
             break;

           case GAN_UINT:
             img = gan_image_form_gen_bgra_ui ( img, height, width, stride,
                                                alloc_pix_data,
                                                (Gan_BGRAPixel_ui *)  pix_data, pix_data_size,
                                                (Gan_BGRAPixel_ui **) row_data, row_data_size );
             break;

           case GAN_FLOAT:
             img = gan_image_form_gen_bgra_f ( img, height, width, stride,
                                               alloc_pix_data,
                                               (Gan_BGRAPixel_f *)  pix_data, pix_data_size,
                                               (Gan_BGRAPixel_f **) row_data, row_data_size );
             break;

           case GAN_DOUBLE:
             img = gan_image_form_gen_bgra_d ( img, height, width, stride,
                                               alloc_pix_data,
                                               (Gan_BGRAPixel_d *)  pix_data, pix_data_size,
                                               (Gan_BGRAPixel_d **) row_data, row_data_size );
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_form_gen",
                                GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             return NULL;
        }
        break;

      case GAN_VECTOR_FIELD_2D:
        switch ( type )
        {
           case GAN_FLOAT:
             img = gan_image_form_gen_vfield2D_f ( img, height, width, stride,
                                                   alloc_pix_data,
                                                   (Gan_Vector2_f *)  pix_data, pix_data_size,
                                                   (Gan_Vector2_f **) row_data, row_data_size );
             break;

           case GAN_DOUBLE:
             img = gan_image_form_gen_vfield2D_d ( img, height, width, stride,
                                                   alloc_pix_data,
                                                   (Gan_Vector2 *)  pix_data, pix_data_size,
                                                   (Gan_Vector2 **) row_data, row_data_size );
             break;

           case GAN_SHORT:
             img = gan_image_form_gen_vfield2D_s ( img, height, width, stride,
                                                   alloc_pix_data,
                                                   (Gan_Vector2_s *)  pix_data, pix_data_size,
                                                   (Gan_Vector2_s **) row_data, row_data_size );
             break;

           case GAN_INT:
             img = gan_image_form_gen_vfield2D_i ( img, height, width, stride,
                                                   alloc_pix_data,
                                                   (Gan_Vector2_i *)  pix_data, pix_data_size,
                                                   (Gan_Vector2_i **) row_data, row_data_size );
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_form_gen",
                                GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             return NULL;
        }
        break;

      case GAN_VECTOR_FIELD_3D:
        switch ( type )
        {
           case GAN_FLOAT:
             img = gan_image_form_gen_vfield3D_f ( img, height, width, stride,
                                                   alloc_pix_data,
                                                   (Gan_Vector3_f *)  pix_data, pix_data_size,
                                                   (Gan_Vector3_f **) row_data, row_data_size );
             break;

           case GAN_DOUBLE:
             img = gan_image_form_gen_vfield3D_d ( img, height, width, stride,
                                                   alloc_pix_data,
                                                   (Gan_Vector3 *)  pix_data, pix_data_size,
                                                   (Gan_Vector3 **) row_data, row_data_size );
             break;

           case GAN_SHORT:
             img = gan_image_form_gen_vfield3D_s ( img, height, width, stride,
                                                   alloc_pix_data,
                                                   (Gan_Vector3_s *)  pix_data, pix_data_size,
                                                   (Gan_Vector3_s **) row_data, row_data_size );
             break;

           case GAN_INT:
             img = gan_image_form_gen_vfield3D_i ( img, height, width, stride,
                                                   alloc_pix_data,
                                                   (Gan_Vector3_i *)  pix_data, pix_data_size,
                                                   (Gan_Vector3_i **) row_data, row_data_size );
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_form_gen", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             return NULL;
        }
        break;

      case GAN_RGBX:
        switch ( type )
        {
           case GAN_UINT8:
             img = gan_image_form_gen_rgbx_ui8 ( img, height, width, stride,
                                                 alloc_pix_data,
                                                 (Gan_RGBXPixel_ui8 *)  pix_data, pix_data_size,
                                                 (Gan_RGBXPixel_ui8 **) row_data, row_data_size );
             break;

           case GAN_UINT10:
             img = gan_image_form_gen_rgbx_ui10 ( img, height, width, stride,
                                                  alloc_pix_data,
                                                  (Gan_RGBXPixel_ui10 *)  pix_data, pix_data_size,
                                                  (Gan_RGBXPixel_ui10 **) row_data, row_data_size );
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_form_gen", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             return NULL;
        }
        break;

      case GAN_RGBAS:
        switch ( type )
        {
           case GAN_UINT10:
             img = gan_image_form_gen_rgbas_ui10 ( img, height, width, stride,
                                                  alloc_pix_data,
                                                  (Gan_RGBASPixel_ui10 *)  pix_data, pix_data_size,
                                                  (Gan_RGBASPixel_ui10 **) row_data, row_data_size );
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_form_gen", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             return NULL;
        }
        break;

      case GAN_YUVX444:
        switch ( type )
        {
           case GAN_UINT8:
             img = gan_image_form_gen_yuvx444_ui8 ( img, height, width, stride,
                                                    alloc_pix_data,
                                                    (Gan_YUVX444Pixel_ui8 *)  pix_data, pix_data_size,
                                                    (Gan_YUVX444Pixel_ui8 **) row_data, row_data_size );
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_form_gen", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             return NULL;
        }
        break;

      case GAN_YUVA444:
        switch ( type )
        {
           case GAN_UINT8:
             img = gan_image_form_gen_yuva444_ui8 ( img, height, width, stride,
                                                    alloc_pix_data,
                                                    (Gan_YUVA444Pixel_ui8 *)  pix_data, pix_data_size,
                                                    (Gan_YUVA444Pixel_ui8 **) row_data, row_data_size );
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_form_gen", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             return NULL;
        }
        break;

      case GAN_YUV422:
        switch ( type )
        {
           case GAN_UINT8:
             img = gan_image_form_gen_yuv422_ui8 ( img, height, width, stride,
                                                   alloc_pix_data,
                                                   (Gan_YUV422Pixel_ui8 *)  pix_data, pix_data_size,
                                                   (Gan_YUV422Pixel_ui8 **) row_data, row_data_size );
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_form_gen", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             return NULL;
        }
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_image_form_gen", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
        return NULL;
   }

   gan_heap_pop();
   return img;
}

/**
 * \}
 */

/**
 * \addtogroup ImageSet
 * \{
 */

/**
 * \brief Sets the function to free the image data buffer.
 * \param img The image
 * \param data_free_func The free function
 * \return #GAN_TRUE on success, or #GAN_FALSE on failure.
 *
 * For images that have been created using a user-supplied data buffer,
 * provide a function to use to free the user data buffer.
 */
Gan_Bool
 gan_image_set_data_free_func ( Gan_Image *img,
                                void (*data_free_func)(void *) )
{
   /* we can't provide a function to free the data when it was allocated
      internally using malloc() */
   gan_err_test_bool ( !img->pix_data_alloc, "gan_image_set_data_free_func",
                       GAN_ERROR_INCOMPATIBLE, "" );

   /* set free function */
   img->data_free_func = data_free_func;

   /* success */
   return GAN_TRUE;
}

/**
 * \}
 */

/**
 * \addtogroup ImageSize
 * \{
 */

/**
 * \brief Returns the size of an image pixel in bytes.
 * \param format The format of the image pixel
 * \param type The type of the image pixel
 * \return The size of the image pixel.
 *
 * Returns the size of an image pixel in bytes.
 */
size_t
 gan_image_pixel_size ( Gan_ImageFormat format, Gan_Type type )
{
   switch ( format )
   {
      case GAN_GREY_LEVEL_IMAGE:
        switch ( type )
        {
           case GAN_UCHAR:   return sizeof(unsigned char);
           case GAN_USHORT:  return sizeof(unsigned short);
           case GAN_SHORT:   return sizeof(short);
           case GAN_UINT:    return sizeof(unsigned int);
           case GAN_INT:     return sizeof(int);
           case GAN_FLOAT:   return sizeof(float);
           case GAN_DOUBLE:  return sizeof(double);
           case GAN_POINTER: return sizeof(void *);
           case GAN_UINT10:  return sizeof(Gan_YXPixel_ui10);
           case GAN_UINT12:  return sizeof(Gan_YXPixel_ui12);
           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_pixel_size", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_GREY_LEVEL_ALPHA_IMAGE:
        switch ( type )
        {
           case GAN_UCHAR:  return sizeof(Gan_GLAPixel_uc);
           case GAN_USHORT: return sizeof(Gan_GLAPixel_us);
           case GAN_SHORT:  return sizeof(Gan_GLAPixel_s);
           case GAN_UINT:   return sizeof(Gan_GLAPixel_ui);
           case GAN_INT:    return sizeof(Gan_GLAPixel_i);
           case GAN_FLOAT:  return sizeof(Gan_GLAPixel_f);
           case GAN_DOUBLE: return sizeof(Gan_GLAPixel_d);
           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_pixel_size", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_RGB_COLOUR_IMAGE:
        switch ( type )
        {
           case GAN_UCHAR:  return sizeof(Gan_RGBPixel_uc);
           case GAN_USHORT: return sizeof(Gan_RGBPixel_us);
           case GAN_SHORT:  return sizeof(Gan_RGBPixel_s);
           case GAN_UINT:   return sizeof(Gan_RGBPixel_ui);
           case GAN_INT:    return sizeof(Gan_RGBPixel_i);
           case GAN_FLOAT:  return sizeof(Gan_RGBPixel_f);
           case GAN_DOUBLE: return sizeof(Gan_RGBPixel_d);
           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_pixel_size", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_RGB_COLOUR_ALPHA_IMAGE:
        switch ( type )
        {
           case GAN_UCHAR:  return sizeof(Gan_RGBAPixel_uc);
           case GAN_USHORT: return sizeof(Gan_RGBAPixel_us);
           case GAN_SHORT:  return sizeof(Gan_RGBAPixel_s);
           case GAN_UINT:   return sizeof(Gan_RGBAPixel_ui);
           case GAN_INT:    return sizeof(Gan_RGBAPixel_i);
           case GAN_FLOAT:  return sizeof(Gan_RGBAPixel_f);
           case GAN_DOUBLE: return sizeof(Gan_RGBAPixel_d);
           case GAN_UINT12: return sizeof(Gan_RGBAPixel_ui12);
           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_pixel_size", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_BGR_COLOUR_IMAGE:
        switch ( type )
        {
           case GAN_UCHAR:  return sizeof(Gan_BGRPixel_uc);
           case GAN_USHORT: return sizeof(Gan_BGRPixel_us);
           case GAN_SHORT:  return sizeof(Gan_BGRPixel_s);
           case GAN_UINT:   return sizeof(Gan_BGRPixel_ui);
           case GAN_INT:    return sizeof(Gan_BGRPixel_i);
           case GAN_FLOAT:  return sizeof(Gan_BGRPixel_f);
           case GAN_DOUBLE: return sizeof(Gan_BGRPixel_d);
           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_pixel_size", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_BGR_COLOUR_ALPHA_IMAGE:
        switch ( type )
        {
           case GAN_UCHAR:  return sizeof(Gan_BGRAPixel_uc);
           case GAN_USHORT: return sizeof(Gan_BGRAPixel_us);
           case GAN_SHORT:  return sizeof(Gan_BGRAPixel_s);
           case GAN_UINT:   return sizeof(Gan_BGRAPixel_ui);
           case GAN_INT:    return sizeof(Gan_BGRAPixel_i);
           case GAN_FLOAT:  return sizeof(Gan_BGRAPixel_f);
           case GAN_DOUBLE: return sizeof(Gan_BGRAPixel_d);
           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_pixel_size", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_VECTOR_FIELD_2D:
        switch ( type )
        {
           case GAN_FLOAT:  return sizeof(Gan_Vector2_f);
           case GAN_DOUBLE: return sizeof(Gan_Vector2);
           case GAN_SHORT:  return sizeof(Gan_Vector2_s);
           case GAN_INT:    return sizeof(Gan_Vector2_i);
           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_pixel_size", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_VECTOR_FIELD_3D:
        switch ( type )
        {
           case GAN_FLOAT:  return sizeof(Gan_Vector3_f);
           case GAN_DOUBLE: return sizeof(Gan_Vector3);
           case GAN_SHORT:  return sizeof(Gan_Vector3_s);
           case GAN_INT:    return sizeof(Gan_Vector3_i);
           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_pixel_size", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_RGBX:
        switch ( type )
        {
           case GAN_UINT8:  return sizeof(Gan_RGBXPixel_ui8);
           case GAN_UINT10: return sizeof(Gan_RGBXPixel_ui10);
           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_pixel_size", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_RGBAS:
        switch ( type )
        {
           case GAN_UINT10: return sizeof(Gan_RGBASPixel_ui10);
           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_pixel_size", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_YUVX444:
        switch ( type )
        {
           case GAN_UINT8: return sizeof(Gan_YUVX444Pixel_ui8);
           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_pixel_size", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_YUVA444:
        switch ( type )
        {
           case GAN_UINT8: return sizeof(Gan_YUVA444Pixel_ui8);
           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_pixel_size", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_YUV422:
        switch ( type )
        {
           case GAN_UINT8: return sizeof(Gan_YUV422Pixel_ui8);
           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_pixel_size", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_image_pixel_size", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
        break;
   }

   /* shouldn't get here */
   return 0;
}

/**
 * \brief Returns the minimum size of a row of image pixels in bytes.
 * \param format The format of the image pixel
 * \param type The type of the image pixel
 * \param width The number of image pixels on a row
 * \param alignment The byte alignment of a row of the image or zero
 * \return The number of bytes in a row.
 *
 * Returns the minimum size of a row of image pixels in bytes, given the
 * \a format, \a type and \a width of the image. The size is padded to a
 * multiple of the \a alignment value, unless \a alignment is passed as zero,
 * in which case no padding occurs.
 */
size_t
 gan_image_min_stride ( Gan_ImageFormat format, Gan_Type type,
                        unsigned long width, size_t alignment )
{
   switch ( format )
   {
      case GAN_GREY_LEVEL_IMAGE:
        switch ( type )
        {
           case GAN_UCHAR:   width *= sizeof(unsigned char);    break;
           case GAN_USHORT:  width *= sizeof(unsigned short);   break;
           case GAN_SHORT:   width *= sizeof(short);            break;
           case GAN_UINT:    width *= sizeof(unsigned int);     break;
           case GAN_INT:     width *= sizeof(int);              break;
           case GAN_FLOAT:   width *= sizeof(float);            break;
           case GAN_DOUBLE:  width *= sizeof(double);           break;
           case GAN_POINTER: width *= sizeof(void *);           break;
           case GAN_UINT10:  width *= sizeof(Gan_YXPixel_ui10); break;
           case GAN_UINT12:  width *= sizeof(Gan_YXPixel_ui12); break;
           case GAN_BOOL:
             width = ((width + GAN_BITWORD_SIZE - 1)/GAN_BITWORD_SIZE)
             *sizeof(Gan_BitWord);
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_min_stride", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_GREY_LEVEL_ALPHA_IMAGE:
        switch ( type )
        {
           case GAN_UCHAR:  width *= sizeof(Gan_GLAPixel_uc); break;
           case GAN_USHORT: width *= sizeof(Gan_GLAPixel_us); break;
           case GAN_SHORT:  width *= sizeof(Gan_GLAPixel_s);  break;
           case GAN_UINT:   width *= sizeof(Gan_GLAPixel_ui); break;
           case GAN_INT:    width *= sizeof(Gan_GLAPixel_i);  break;
           case GAN_FLOAT:  width *= sizeof(Gan_GLAPixel_f);  break;
           case GAN_DOUBLE: width *= sizeof(Gan_GLAPixel_d);  break;
           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_min_stride", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_RGB_COLOUR_IMAGE:
        switch ( type )
        {
           case GAN_UCHAR:  width *= sizeof(Gan_RGBPixel_uc); break;
           case GAN_USHORT: width *= sizeof(Gan_RGBPixel_us); break;
           case GAN_SHORT:  width *= sizeof(Gan_RGBPixel_s);  break;
           case GAN_UINT:   width *= sizeof(Gan_RGBPixel_ui); break;
           case GAN_INT:    width *= sizeof(Gan_RGBPixel_i);  break;
           case GAN_FLOAT:  width *= sizeof(Gan_RGBPixel_f);  break;
           case GAN_DOUBLE: width *= sizeof(Gan_RGBPixel_d);  break;
           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_min_stride", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_RGB_COLOUR_ALPHA_IMAGE:
        switch ( type )
        {
           case GAN_UCHAR:  width *= sizeof(Gan_RGBAPixel_uc);   break;
           case GAN_USHORT: width *= sizeof(Gan_RGBAPixel_us);   break;
           case GAN_SHORT:  width *= sizeof(Gan_RGBAPixel_s);    break;
           case GAN_UINT:   width *= sizeof(Gan_RGBAPixel_ui);   break;
           case GAN_INT:    width *= sizeof(Gan_RGBAPixel_i);    break;
           case GAN_FLOAT:  width *= sizeof(Gan_RGBAPixel_f);    break;
           case GAN_DOUBLE: width *= sizeof(Gan_RGBAPixel_d);    break;
           case GAN_UINT12: width *= sizeof(Gan_RGBAPixel_ui12); break;
           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_min_stride", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_BGR_COLOUR_IMAGE:
        switch ( type )
        {
           case GAN_UCHAR:  width *= sizeof(Gan_BGRPixel_uc); break;
           case GAN_USHORT: width *= sizeof(Gan_BGRPixel_us); break;
           case GAN_SHORT:  width *= sizeof(Gan_BGRPixel_s);  break;
           case GAN_UINT:   width *= sizeof(Gan_BGRPixel_ui); break;
           case GAN_INT:    width *= sizeof(Gan_BGRPixel_i);  break;
           case GAN_FLOAT:  width *= sizeof(Gan_BGRPixel_f);  break;
           case GAN_DOUBLE: width *= sizeof(Gan_BGRPixel_d);  break;
           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_min_stride", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_BGR_COLOUR_ALPHA_IMAGE:
        switch ( type )
        {
           case GAN_UCHAR:  width *= sizeof(Gan_BGRAPixel_uc);   break;
           case GAN_USHORT: width *= sizeof(Gan_BGRAPixel_us);   break;
           case GAN_SHORT:  width *= sizeof(Gan_BGRAPixel_s);    break;
           case GAN_UINT:   width *= sizeof(Gan_BGRAPixel_ui);   break;
           case GAN_INT:    width *= sizeof(Gan_BGRAPixel_i);    break;
           case GAN_FLOAT:  width *= sizeof(Gan_BGRAPixel_f);    break;
           case GAN_DOUBLE: width *= sizeof(Gan_BGRAPixel_d);    break;
           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_min_stride", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_VECTOR_FIELD_2D:
        switch ( type )
        {
           case GAN_FLOAT:  width *= sizeof(Gan_Vector2_f); break;
           case GAN_DOUBLE: width *= sizeof(Gan_Vector2);   break;
           case GAN_SHORT:  width *= sizeof(Gan_Vector2_s); break;
           case GAN_INT:    width *= sizeof(Gan_Vector2_i); break;
           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_min_stride", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_VECTOR_FIELD_3D:
        switch ( type )
        {
           case GAN_FLOAT:  width *= sizeof(Gan_Vector3_f); break;
           case GAN_DOUBLE: width *= sizeof(Gan_Vector3);   break;
           case GAN_SHORT:  width *= sizeof(Gan_Vector3_s); break;
           case GAN_INT:    width *= sizeof(Gan_Vector3_i); break;
           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_min_stride", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_RGBX:
        switch ( type )
        {
           case GAN_UINT8:  width *= sizeof(Gan_RGBXPixel_ui8);  break;
           case GAN_UINT10: width *= sizeof(Gan_RGBXPixel_ui10); break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_min_stride", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_RGBAS:
        switch ( type )
        {
           case GAN_UINT10: width *= sizeof(Gan_RGBASPixel_ui10); break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_min_stride", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_YUVX444:
        switch ( type )
        {
           case GAN_UINT8: width *= sizeof(Gan_YUVX444Pixel_ui8); break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_min_stride", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_YUVA444:
        switch ( type )
        {
           case GAN_UINT8: width *= sizeof(Gan_YUVA444Pixel_ui8); break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_min_stride", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_YUV422:
        switch ( type )
        {
           case GAN_UINT8: width *= sizeof(Gan_YUV422Pixel_ui8); break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_min_stride", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_image_min_stride", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
        break;
   }

   /* if no alignment was specified, return without padding */
   if ( alignment == 0 ) return width;

   /* pad the size out to a multiple of the byte alignment */
   return ((width + alignment - 1)/alignment)*alignment;
}

/**
 * \}
 */

/**
 * \addtogroup ImageSet
 * \{
 */

/**
 * \brief Sets the format, type, dimensions and stride of an image.
 * \param img Pointer to the image
 * \param format The format to set the image
 * \param type The type to set the image
 * \param height The height to set the image
 * \param width The width to set the image
 * \param stride The stride of the image in bytes
 * \param alloc_pix_data Whether or not to allocate the pixel data
 * \return Pointer to the successfully modified image, or \c NULL on failure.
 *
 * Sets the \a format, \a type, \a height, \a width and \a stride of an image.
 */
Gan_Image *
 gan_image_set_format_type_dims_gen  ( Gan_Image *img,
                                       Gan_ImageFormat format, Gan_Type type,
                                       unsigned long height,
                                       unsigned long width,
                                       unsigned long stride,
                                       Gan_Bool alloc_pix_data )
{
   switch ( format )
   {
      case GAN_GREY_LEVEL_IMAGE:
        switch ( type )
        {
           case GAN_UCHAR:   return gan_image_set_gen_gl_uc   ( img, height, width, stride, alloc_pix_data );
           case GAN_USHORT:  return gan_image_set_gen_gl_us   ( img, height, width, stride, alloc_pix_data );
           case GAN_SHORT:   return gan_image_set_gen_gl_s    ( img, height, width, stride, alloc_pix_data );
           case GAN_UINT:    return gan_image_set_gen_gl_ui   ( img, height, width, stride, alloc_pix_data );
           case GAN_INT:     return gan_image_set_gen_gl_i    ( img, height, width, stride, alloc_pix_data );
           case GAN_FLOAT:   return gan_image_set_gen_gl_f    ( img, height, width, stride, alloc_pix_data );
           case GAN_DOUBLE:  return gan_image_set_gen_gl_d    ( img, height, width, stride, alloc_pix_data );
           case GAN_BOOL:    return gan_image_set_gen_b       ( img, height, width, stride, alloc_pix_data );
           case GAN_POINTER: return gan_image_set_gen_p       ( img, height, width, stride, alloc_pix_data );
           case GAN_UINT10:  return gan_image_set_gen_gl_ui10 ( img, height, width, stride, alloc_pix_data );
           case GAN_UINT12:  return gan_image_set_gen_gl_ui12 ( img, height, width, stride, alloc_pix_data );

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_set_format_type_dims", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             return NULL;
        }

      case GAN_GREY_LEVEL_ALPHA_IMAGE:
        switch ( type )
        {
           case GAN_UCHAR:  return gan_image_set_gen_gla_uc ( img, height, width, stride, alloc_pix_data );
           case GAN_USHORT: return gan_image_set_gen_gla_us ( img, height, width, stride, alloc_pix_data );
           case GAN_SHORT:  return gan_image_set_gen_gla_s ( img, height, width, stride, alloc_pix_data );
           case GAN_UINT:   return gan_image_set_gen_gla_ui ( img, height, width, stride, alloc_pix_data );
           case GAN_INT:    return gan_image_set_gen_gla_i ( img, height, width, stride, alloc_pix_data );
           case GAN_FLOAT:  return gan_image_set_gen_gla_f ( img, height, width, stride, alloc_pix_data );
           case GAN_DOUBLE: return gan_image_set_gen_gla_d ( img, height, width, stride, alloc_pix_data );

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_set_format_type_dims", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             return NULL;
        }

      case GAN_RGB_COLOUR_IMAGE:
        switch ( type )
        {
           case GAN_UCHAR:  return gan_image_set_gen_rgb_uc ( img, height, width, stride, alloc_pix_data );
           case GAN_USHORT: return gan_image_set_gen_rgb_us ( img, height, width, stride, alloc_pix_data );
           case GAN_SHORT:  return gan_image_set_gen_rgb_s ( img, height, width, stride, alloc_pix_data );
           case GAN_UINT:   return gan_image_set_gen_rgb_ui ( img, height, width, stride, alloc_pix_data );
           case GAN_INT:    return gan_image_set_gen_rgb_i ( img, height, width, stride, alloc_pix_data );
           case GAN_FLOAT:  return gan_image_set_gen_rgb_f ( img, height, width, stride, alloc_pix_data );
           case GAN_DOUBLE: return gan_image_set_gen_rgb_d ( img, height, width, stride, alloc_pix_data );

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_set_format_type_dims", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             return NULL;
        }

      case GAN_RGB_COLOUR_ALPHA_IMAGE:
        switch ( type )
        {
           case GAN_UCHAR:  return gan_image_set_gen_rgba_uc ( img, height, width, stride, alloc_pix_data );
           case GAN_USHORT: return gan_image_set_gen_rgba_us ( img, height, width, stride, alloc_pix_data );
           case GAN_SHORT:  return gan_image_set_gen_rgba_s ( img, height, width, stride, alloc_pix_data );
           case GAN_UINT:   return gan_image_set_gen_rgba_ui ( img, height, width, stride, alloc_pix_data );
           case GAN_INT:    return gan_image_set_gen_rgba_i ( img, height, width, stride, alloc_pix_data );
           case GAN_FLOAT:  return gan_image_set_gen_rgba_f ( img, height, width, stride, alloc_pix_data );
           case GAN_DOUBLE: return gan_image_set_gen_rgba_d ( img, height, width, stride, alloc_pix_data );
           case GAN_UINT12: return gan_image_set_gen_rgba_ui12 ( img, height, width, stride, alloc_pix_data );

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_set_format_type_dims", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             return NULL;
        }

      case GAN_BGR_COLOUR_IMAGE:
        switch ( type )
        {
           case GAN_UCHAR:  return gan_image_set_gen_bgr_uc ( img, height, width, stride, alloc_pix_data );
           case GAN_USHORT: return gan_image_set_gen_bgr_us ( img, height, width, stride, alloc_pix_data );
           case GAN_SHORT:  return gan_image_set_gen_bgr_s ( img, height, width, stride, alloc_pix_data );
           case GAN_UINT:   return gan_image_set_gen_bgr_ui ( img, height, width, stride, alloc_pix_data );
           case GAN_INT:    return gan_image_set_gen_bgr_i ( img, height, width, stride, alloc_pix_data );
           case GAN_FLOAT:  return gan_image_set_gen_bgr_f ( img, height, width, stride, alloc_pix_data );
           case GAN_DOUBLE: return gan_image_set_gen_bgr_d ( img, height, width, stride, alloc_pix_data );

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_set_format_type_dims", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             return NULL;
        }

      case GAN_BGR_COLOUR_ALPHA_IMAGE:
        switch ( type )
        {
           case GAN_UCHAR:  return gan_image_set_gen_bgra_uc ( img, height, width, stride, alloc_pix_data );
           case GAN_USHORT: return gan_image_set_gen_bgra_us ( img, height, width, stride, alloc_pix_data );
           case GAN_SHORT:  return gan_image_set_gen_bgra_s ( img, height, width, stride, alloc_pix_data );
           case GAN_UINT:   return gan_image_set_gen_bgra_ui ( img, height, width, stride, alloc_pix_data );
           case GAN_INT:    return gan_image_set_gen_bgra_i ( img, height, width, stride, alloc_pix_data );
           case GAN_FLOAT:  return gan_image_set_gen_bgra_f ( img, height, width, stride, alloc_pix_data );
           case GAN_DOUBLE: return gan_image_set_gen_bgra_d ( img, height, width, stride, alloc_pix_data );

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_set_format_type_dims", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             return NULL;
        }

      case GAN_VECTOR_FIELD_2D:
        switch ( type )
        {
           case GAN_SHORT:  return gan_image_set_gen_vfield2D_s ( img, height, width, stride, alloc_pix_data );
           case GAN_INT:    return gan_image_set_gen_vfield2D_i ( img, height, width, stride, alloc_pix_data );
           case GAN_FLOAT:  return gan_image_set_gen_vfield2D_f ( img, height, width, stride, alloc_pix_data );
           case GAN_DOUBLE: return gan_image_set_gen_vfield2D_d ( img, height, width, stride, alloc_pix_data );

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_set_format_type_dims", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             return NULL;
        }

      case GAN_VECTOR_FIELD_3D:
        switch ( type )
        {
           case GAN_SHORT:  return gan_image_set_gen_vfield3D_s ( img, height, width, stride, alloc_pix_data );
           case GAN_INT:    return gan_image_set_gen_vfield3D_i ( img, height, width, stride, alloc_pix_data );
           case GAN_FLOAT:  return gan_image_set_gen_vfield3D_f ( img, height, width, stride, alloc_pix_data );
           case GAN_DOUBLE: return gan_image_set_gen_vfield3D_d ( img, height, width, stride, alloc_pix_data );

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_set_format_type_dims", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             return NULL;
        }

      case GAN_RGBX:
        switch ( type )
        {
           case GAN_UINT8:  return gan_image_set_gen_rgbx_ui8  ( img, height, width, stride, alloc_pix_data );
           case GAN_UINT10: return gan_image_set_gen_rgbx_ui10 ( img, height, width, stride, alloc_pix_data );

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_set_format_type_dims", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             return NULL;
        }

      case GAN_RGBAS:
        switch ( type )
        {
           case GAN_UINT10: return gan_image_set_gen_rgbas_ui10 ( img, height, width, stride, alloc_pix_data );

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_set_format_type_dims", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             return NULL;
        }

      case GAN_YUVX444:
        switch ( type )
        {
           case GAN_UINT8: return gan_image_set_gen_yuvx444_ui8 ( img, height, width, stride, alloc_pix_data );

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_set_format_type_dims", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             return NULL;
        }

      case GAN_YUVA444:
        switch ( type )
        {
           case GAN_UINT8: return gan_image_set_gen_yuva444_ui8 ( img, height, width, stride, alloc_pix_data );

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_set_format_type_dims", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             return NULL;
        }

      case GAN_YUV422:
        switch ( type )
        {
           case GAN_UINT8: return gan_image_set_gen_yuv422_ui8 ( img, height, width, stride, alloc_pix_data );

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_set_format_type_dims", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             return NULL;
        }

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_image_set_format_type_dims", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
        return NULL;
   }

   /* shouldn't get here */
   return NULL;
}

/**
 * \}
 */

/**
 * \addtogroup ImageFill
 * \{
 */

/**
 * \brief Fill an image with a constant value.
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Fill image \a img with constant value \a pixel.
 */
Gan_Bool
 gan_image_fill_const ( Gan_Image *img, Gan_Pixel *pixel )
{
   gan_err_test_bool ( img->format == pixel->format &&
                       img->type == pixel->type, "gan_image_fill_const",
                       GAN_ERROR_INCOMPATIBLE, "" );
   switch ( img->format )
   {
      case GAN_GREY_LEVEL_IMAGE:
        switch ( img->type )
        {
           case GAN_UCHAR:   return gan_image_fill_const_gl_uc   ( img, pixel->data.gl.uc );
           case GAN_SHORT:   return gan_image_fill_const_gl_s    ( img, pixel->data.gl.s );
           case GAN_USHORT:  return gan_image_fill_const_gl_us   ( img, pixel->data.gl.us );
           case GAN_INT:     return gan_image_fill_const_gl_i    ( img, pixel->data.gl.i );
           case GAN_UINT:    return gan_image_fill_const_gl_ui   ( img, pixel->data.gl.ui );
           case GAN_FLOAT:   return gan_image_fill_const_gl_f    ( img, pixel->data.gl.f );
           case GAN_DOUBLE:  return gan_image_fill_const_gl_d    ( img, pixel->data.gl.d );
           case GAN_BOOL:    return gan_image_fill_const_b       ( img, pixel->data.gl.b );
           case GAN_POINTER: return gan_image_fill_const_p       ( img, pixel->data.gl.p );
           case GAN_UINT10:  return gan_image_fill_const_gl_ui10 ( img, &pixel->data.gl.ui10 );
           case GAN_UINT12:  return gan_image_fill_const_gl_ui12 ( img, &pixel->data.gl.ui12 );

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_fill_const", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_GREY_LEVEL_ALPHA_IMAGE:
        switch ( img->type )
        {
           case GAN_UCHAR:  return gan_image_fill_const_gla_uc ( img, &pixel->data.gla.uc );
           case GAN_SHORT:  return gan_image_fill_const_gla_s  ( img, &pixel->data.gla.s );
           case GAN_USHORT: return gan_image_fill_const_gla_us ( img, &pixel->data.gla.us );
           case GAN_INT:    return gan_image_fill_const_gla_i  ( img, &pixel->data.gla.i );
           case GAN_UINT:   return gan_image_fill_const_gla_ui ( img, &pixel->data.gla.ui );
           case GAN_FLOAT:  return gan_image_fill_const_gla_f  ( img, &pixel->data.gla.f );
           case GAN_DOUBLE: return gan_image_fill_const_gla_d  ( img, &pixel->data.gla.d );

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_fill_const", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_RGB_COLOUR_IMAGE:
        switch ( img->type )
        {
           case GAN_UCHAR:  return gan_image_fill_const_rgb_uc ( img, &pixel->data.rgb.uc );
           case GAN_SHORT:  return gan_image_fill_const_rgb_s  ( img, &pixel->data.rgb.s );
           case GAN_USHORT: return gan_image_fill_const_rgb_us ( img, &pixel->data.rgb.us );
           case GAN_INT:    return gan_image_fill_const_rgb_i  ( img, &pixel->data.rgb.i );
           case GAN_UINT:   return gan_image_fill_const_rgb_ui ( img, &pixel->data.rgb.ui );
           case GAN_FLOAT:  return gan_image_fill_const_rgb_f  ( img, &pixel->data.rgb.f );
           case GAN_DOUBLE: return gan_image_fill_const_rgb_d  ( img, &pixel->data.rgb.d );

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_fill_const", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_RGB_COLOUR_ALPHA_IMAGE:
        switch ( img->type )
        {
           case GAN_UCHAR:  return gan_image_fill_const_rgba_uc   ( img, &pixel->data.rgba.uc );
           case GAN_SHORT:  return gan_image_fill_const_rgba_s    ( img, &pixel->data.rgba.s );
           case GAN_USHORT: return gan_image_fill_const_rgba_us   ( img, &pixel->data.rgba.us );
           case GAN_INT:    return gan_image_fill_const_rgba_i    ( img, &pixel->data.rgba.i );
           case GAN_UINT:   return gan_image_fill_const_rgba_ui   ( img, &pixel->data.rgba.ui );
           case GAN_FLOAT:  return gan_image_fill_const_rgba_f    ( img, &pixel->data.rgba.f );
           case GAN_DOUBLE: return gan_image_fill_const_rgba_d    ( img, &pixel->data.rgba.d );
           case GAN_UINT12: return gan_image_fill_const_rgba_ui12 ( img, &pixel->data.rgba.ui12 );

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_fill_const", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_BGR_COLOUR_IMAGE:
        switch ( img->type )
        {
           case GAN_UCHAR:  return gan_image_fill_const_bgr_uc ( img, &pixel->data.bgr.uc );
           case GAN_SHORT:  return gan_image_fill_const_bgr_s  ( img, &pixel->data.bgr.s );
           case GAN_USHORT: return gan_image_fill_const_bgr_us ( img, &pixel->data.bgr.us );
           case GAN_INT:    return gan_image_fill_const_bgr_i  ( img, &pixel->data.bgr.i );
           case GAN_UINT:   return gan_image_fill_const_bgr_ui ( img, &pixel->data.bgr.ui );
           case GAN_FLOAT:  return gan_image_fill_const_bgr_f  ( img, &pixel->data.bgr.f );
           case GAN_DOUBLE: return gan_image_fill_const_bgr_d  ( img, &pixel->data.bgr.d );

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_fill_const", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_BGR_COLOUR_ALPHA_IMAGE:
        switch ( img->type )
        {
           case GAN_UCHAR:  return gan_image_fill_const_bgra_uc   ( img, &pixel->data.bgra.uc );
           case GAN_SHORT:  return gan_image_fill_const_bgra_s    ( img, &pixel->data.bgra.s );
           case GAN_USHORT: return gan_image_fill_const_bgra_us   ( img, &pixel->data.bgra.us );
           case GAN_INT:    return gan_image_fill_const_bgra_i    ( img, &pixel->data.bgra.i );
           case GAN_UINT:   return gan_image_fill_const_bgra_ui   ( img, &pixel->data.bgra.ui );
           case GAN_FLOAT:  return gan_image_fill_const_bgra_f    ( img, &pixel->data.bgra.f );
           case GAN_DOUBLE: return gan_image_fill_const_bgra_d    ( img, &pixel->data.bgra.d );

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_fill_const", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_VECTOR_FIELD_2D:
        switch ( img->type )
        {
           case GAN_FLOAT:  return gan_image_fill_const_vfield2D_f(img,&pixel->data.vfield2D.f);
           case GAN_DOUBLE: return gan_image_fill_const_vfield2D_d(img,&pixel->data.vfield2D.d);
           case GAN_SHORT:  return gan_image_fill_const_vfield2D_s(img,&pixel->data.vfield2D.s);
           case GAN_INT:    return gan_image_fill_const_vfield2D_i(img,&pixel->data.vfield2D.i);

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_fill_const", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_VECTOR_FIELD_3D:
        switch ( img->type )
        {
           case GAN_FLOAT:  return gan_image_fill_const_vfield3D_f(img,&pixel->data.vfield3D.f);
           case GAN_DOUBLE: return gan_image_fill_const_vfield3D_d(img,&pixel->data.vfield3D.d);
           case GAN_SHORT:  return gan_image_fill_const_vfield3D_s(img,&pixel->data.vfield3D.s);
           case GAN_INT:    return gan_image_fill_const_vfield3D_i(img,&pixel->data.vfield3D.i);

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_fill_const", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_RGBX:
        switch ( img->type )
        {
           case GAN_UINT8:  return gan_image_fill_const_rgbx_ui8  ( img, &pixel->data.rgbx.ui8 );
           case GAN_UINT10: return gan_image_fill_const_rgbx_ui10 ( img, &pixel->data.rgbx.ui10 );

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_fill_const", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_RGBAS:
        switch ( img->type )
        {
           case GAN_UINT10: return gan_image_fill_const_rgbas_ui10 ( img, &pixel->data.rgbas.ui10 );

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_fill_const", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_YUVX444:
        switch ( img->type )
        {
           case GAN_UINT8: return gan_image_fill_const_yuvx444_ui8 ( img, &pixel->data.yuvx444.ui8 );

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_fill_const", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_YUVA444:
        switch ( img->type )
        {
           case GAN_UINT8: return gan_image_fill_const_yuva444_ui8 ( img, &pixel->data.yuva444.ui8 );

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_fill_const", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_YUV422:
        switch ( img->type )
        {
           case GAN_UINT8: return gan_image_fill_const_yuv422_ui8 ( img, &pixel->data.yuv422.ui8 );

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_fill_const", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_image_fill_const", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
        break;
   }

   /* shouldn't get here */
   assert(0);
   return GAN_FALSE;
}

/**
 * \brief Fills an image with zero pixels.
 * \param img The image to fill
 * \return #GAN_TRUE on success, or #GAN_FALSE on failure.
 *
 * Fills image \a img with zero pixels.
 */
Gan_Bool
 gan_image_fill_zero ( Gan_Image *img )
{
   switch ( img->format )
   {
      case GAN_GREY_LEVEL_IMAGE:
        switch ( img->type )
        {
           case GAN_UCHAR:   return gan_image_fill_const_gl_uc ( img, 0 );
           case GAN_SHORT:   return gan_image_fill_const_gl_s ( img, 0 );
           case GAN_USHORT:  return gan_image_fill_const_gl_us ( img, 0 );
           case GAN_INT:     return gan_image_fill_const_gl_i ( img, 0 );
           case GAN_UINT:    return gan_image_fill_const_gl_ui ( img, 0 );
           case GAN_FLOAT:   return gan_image_fill_const_gl_f ( img, 0.0F );
           case GAN_DOUBLE:  return gan_image_fill_const_gl_d ( img, 0.0 );
           case GAN_BOOL:    return gan_image_fill_const_b ( img, GAN_FALSE );
           case GAN_POINTER: return gan_image_fill_const_p ( img, NULL );
           case GAN_UINT10:
           {
              Gan_YXPixel_ui10 pix = { 0, 0 };
            
              return gan_image_fill_const_gl_ui10 ( img, &pix );
           }

           case GAN_UINT12:
           {
              Gan_YXPixel_ui12 pix = { 0, 0 };
            
              return gan_image_fill_const_gl_ui12 ( img, &pix );
           }

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_fill_zero", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_GREY_LEVEL_ALPHA_IMAGE:
        switch ( img->type )
        {
           case GAN_UCHAR:
           {
              Gan_GLAPixel_uc pix = { 0, 0 };

              return gan_image_fill_const_gla_uc ( img, &pix );
           }

           case GAN_SHORT:
           {
              Gan_GLAPixel_s pix = { 0, 0 };

              return gan_image_fill_const_gla_s ( img, &pix );
           }

           case GAN_USHORT:
           {
              Gan_GLAPixel_us pix = { 0, 0 };

              return gan_image_fill_const_gla_us ( img, &pix );
           }

           case GAN_INT:
           {
              Gan_GLAPixel_i pix = { 0, 0 };

              return gan_image_fill_const_gla_i ( img, &pix );
           }

           case GAN_UINT:
           {
              Gan_GLAPixel_ui pix = { 0, 0 };

              return gan_image_fill_const_gla_ui ( img, &pix );
           }

           case GAN_FLOAT:
           {
              Gan_GLAPixel_f pix = { 0.0F, 0.0F };

              return gan_image_fill_const_gla_f ( img, &pix );
           }

           case GAN_DOUBLE:
           {
              Gan_GLAPixel_d pix = { 0.0, 0.0 };

              return gan_image_fill_const_gla_d ( img, &pix );
           }

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_fill_zero", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_RGB_COLOUR_IMAGE:
        switch ( img->type )
        {
           case GAN_UCHAR:
           {
              Gan_RGBPixel_uc pix = { 0, 0, 0 };

              return gan_image_fill_const_rgb_uc ( img, &pix );
           }

           case GAN_SHORT:
           {
              Gan_RGBPixel_s pix = { 0, 0, 0 };

              return gan_image_fill_const_rgb_s ( img, &pix );
           }

           case GAN_USHORT:
           {
              Gan_RGBPixel_us pix = { 0, 0, 0 };

              return gan_image_fill_const_rgb_us ( img, &pix );
           }

           case GAN_INT:
           {
              Gan_RGBPixel_i pix = { 0, 0, 0 };

              return gan_image_fill_const_rgb_i ( img, &pix );
           }

           case GAN_UINT:
           {
              Gan_RGBPixel_ui pix = { 0, 0, 0 };

              return gan_image_fill_const_rgb_ui ( img, &pix );
           }

           case GAN_FLOAT:
           {
              Gan_RGBPixel_f pix = { 0.0F, 0.0F, 0.0F };

              return gan_image_fill_const_rgb_f ( img, &pix );
           }

           case GAN_DOUBLE:
           {
              Gan_RGBPixel_d pix = { 0.0, 0.0, 0.0 };

              return gan_image_fill_const_rgb_d ( img, &pix );
           }

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_fill_zero", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_RGB_COLOUR_ALPHA_IMAGE:
        switch ( img->type )
        {
           case GAN_UCHAR:
           {
              Gan_RGBAPixel_uc pix = { 0, 0, 0, 0 };

              return gan_image_fill_const_rgba_uc ( img, &pix );
           }

           case GAN_SHORT:
           {
              Gan_RGBAPixel_s pix = { 0, 0, 0, 0 };

              return gan_image_fill_const_rgba_s ( img, &pix );
           }

           case GAN_USHORT:
           {
              Gan_RGBAPixel_us pix = { 0, 0, 0, 0 };

              return gan_image_fill_const_rgba_us ( img, &pix );
           }

           case GAN_INT:
           {
              Gan_RGBAPixel_i pix = { 0, 0, 0, 0 };

              return gan_image_fill_const_rgba_i ( img, &pix );
           }

           case GAN_UINT:
           {
              Gan_RGBAPixel_ui pix = { 0, 0, 0, 0 };

              return gan_image_fill_const_rgba_ui ( img, &pix );
           }

           case GAN_FLOAT:
           {
              Gan_RGBAPixel_f pix = { 0.0F, 0.0F, 0.0F, 0.0F };

              return gan_image_fill_const_rgba_f ( img, &pix );
           }

           case GAN_DOUBLE:
           {
              Gan_RGBAPixel_d pix = { 0.0, 0.0, 0.0, 0.0 };

              return gan_image_fill_const_rgba_d ( img, &pix );
           }

           case GAN_UINT12:
           {
              Gan_RGBAPixel_ui12 pix = { 0, 0, 0, 0 };

              return gan_image_fill_const_rgba_ui12 ( img, &pix );
           }

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_fill_zero", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_BGR_COLOUR_IMAGE:
        switch ( img->type )
        {
           case GAN_UCHAR:
           {
              Gan_BGRPixel_uc pix = { 0, 0, 0 };

              return gan_image_fill_const_bgr_uc ( img, &pix );
           }

           case GAN_SHORT:
           {
              Gan_BGRPixel_s pix = { 0, 0, 0 };

              return gan_image_fill_const_bgr_s ( img, &pix );
           }

           case GAN_USHORT:
           {
              Gan_BGRPixel_us pix = { 0, 0, 0 };

              return gan_image_fill_const_bgr_us ( img, &pix );
           }

           case GAN_INT:
           {
              Gan_BGRPixel_i pix = { 0, 0, 0 };

              return gan_image_fill_const_bgr_i ( img, &pix );
           }

           case GAN_UINT:
           {
              Gan_BGRPixel_ui pix = { 0, 0, 0 };

              return gan_image_fill_const_bgr_ui ( img, &pix );
           }

           case GAN_FLOAT:
           {
              Gan_BGRPixel_f pix = { 0.0F, 0.0F, 0.0F };

              return gan_image_fill_const_bgr_f ( img, &pix );
           }

           case GAN_DOUBLE:
           {
              Gan_BGRPixel_d pix = { 0.0, 0.0, 0.0 };

              return gan_image_fill_const_bgr_d ( img, &pix );
           }

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_fill_zero", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_BGR_COLOUR_ALPHA_IMAGE:
        switch ( img->type )
        {
           case GAN_UCHAR:
           {
              Gan_BGRAPixel_uc pix = { 0, 0, 0, 0 };

              return gan_image_fill_const_bgra_uc ( img, &pix );
           }

           case GAN_SHORT:
           {
              Gan_BGRAPixel_s pix = { 0, 0, 0, 0 };

              return gan_image_fill_const_bgra_s ( img, &pix );
           }

           case GAN_USHORT:
           {
              Gan_BGRAPixel_us pix = { 0, 0, 0, 0 };

              return gan_image_fill_const_bgra_us ( img, &pix );
           }

           case GAN_INT:
           {
              Gan_BGRAPixel_i pix = { 0, 0, 0, 0 };

              return gan_image_fill_const_bgra_i ( img, &pix );
           }

           case GAN_UINT:
           {
              Gan_BGRAPixel_ui pix = { 0, 0, 0, 0 };

              return gan_image_fill_const_bgra_ui ( img, &pix );
           }

           case GAN_FLOAT:
           {
              Gan_BGRAPixel_f pix = { 0.0F, 0.0F, 0.0F, 0.0F };

              return gan_image_fill_const_bgra_f ( img, &pix );
           }

           case GAN_DOUBLE:
           {
              Gan_BGRAPixel_d pix = { 0.0, 0.0, 0.0, 0.0 };

              return gan_image_fill_const_bgra_d ( img, &pix );
           }

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_fill_zero", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_VECTOR_FIELD_2D:
        switch ( img->type )
        {
           case GAN_FLOAT:
           {
              Gan_Vector2_f vec = { 0.0F, 0.0F };

              return gan_image_fill_const_vfield2D_f ( img, &vec );
           }

           case GAN_DOUBLE:
           {
              Gan_Vector2 vec = { 0.0, 0.0 };

              return gan_image_fill_const_vfield2D_d ( img, &vec );
           }

           case GAN_SHORT:
           {
              Gan_Vector2_s vec = { 0, 0 };

              return gan_image_fill_const_vfield2D_s ( img, &vec );
           }

           case GAN_INT:
           {
              Gan_Vector2_i vec = { 0, 0 };

              return gan_image_fill_const_vfield2D_i ( img, &vec );
           }

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_fill_zero", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_VECTOR_FIELD_3D:
        switch ( img->type )
        {
           case GAN_FLOAT:
           {
              Gan_Vector3_f vec = { 0.0F, 0.0F, 0.0F };

              return gan_image_fill_const_vfield3D_f ( img, &vec );
           }

           case GAN_DOUBLE:
           {
              Gan_Vector3 vec = { 0.0, 0.0, 0.0 };

              return gan_image_fill_const_vfield3D_d ( img, &vec );
           }

           case GAN_SHORT:
           {
              Gan_Vector3_s vec = { 0, 0, 0 };

              return gan_image_fill_const_vfield3D_s ( img, &vec );
           }

           case GAN_INT:
           {
              Gan_Vector3_i vec = { 0, 0, 0 };

              return gan_image_fill_const_vfield3D_i ( img, &vec );
           }

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_fill_zero", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_RGBX:
        switch ( img->type )
        {
           case GAN_UINT8:
           {
              Gan_RGBXPixel_ui8 pix = { 0, 0, 0 };

              return gan_image_fill_const_rgbx_ui8 ( img, &pix );
           }

           case GAN_UINT10:
           {
              Gan_RGBXPixel_ui10 pix = { 0, 0, 0 };

              return gan_image_fill_const_rgbx_ui10 ( img, &pix );
           }

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_fill_zero", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_RGBAS:
        switch ( img->type )
        {
           case GAN_UINT10:
           {
              Gan_RGBASPixel_ui10 pix = { 0, 0, 0 };

              return gan_image_fill_const_rgbas_ui10 ( img, &pix );
           }

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_fill_zero", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_YUVX444:
        switch ( img->type )
        {
           case GAN_UINT8:
           {
              Gan_YUVX444Pixel_ui8 pix = { 0, 0, 0 };

              return gan_image_fill_const_yuvx444_ui8 ( img, &pix );
           }

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_fill_zero", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_YUVA444:
        switch ( img->type )
        {
           case GAN_UINT8:
           {
              Gan_YUVA444Pixel_ui8 pix = { 0, 0, 0 };

              return gan_image_fill_const_yuva444_ui8 ( img, &pix );
           }

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_fill_zero", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_YUV422:
        switch ( img->type )
        {
           case GAN_UINT8:
           {
              Gan_YUV422Pixel_ui8 pix = { 0, 0 };

              return gan_image_fill_const_yuv422_ui8 ( img, &pix );
           }

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_fill_zero", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_image_fill_zero", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
        break;
   }

   /* shouldn't get here */
   assert(0);
   return GAN_FALSE;
}

/**
 * \brief Fill a subwindow of an image.
 * \param img The image to fill
 * \param r0 The top row of the subwindow
 * \param c0 The left column of the subwindow
 * \param height The height of the subwindow
 * \param width The width of the subwindow
 * \param pixel The pixel value with which to fill the subwindow
 * \return #GAN_TRUE on success, or #GAN_FALSE on failure.
 *
 * Fills a subwindow of image \a img with value \a pixel, the subwindow being
 * defined by the \a r0, \a c0, \a height and \a width parameters.
 * A \c NULL \a pixel pointer indicates zero value.
 */
Gan_Bool
 gan_image_fill_const_window ( Gan_Image *img, unsigned r0, unsigned c0,
                               unsigned height, unsigned width,
                               Gan_Pixel *pixel )
{
   Gan_Image *sub_image;

   if ( img->type == GAN_BOOL )
   {
      unsigned i;

      gan_err_test_bool ( img->format == GAN_GREY_LEVEL_IMAGE && (pixel == NULL || (pixel->format == GAN_GREY_LEVEL_IMAGE &&
                                                                                    pixel->type == GAN_BOOL)),
                          "gan_image_fill_const_window", GAN_ERROR_INCOMPATIBLE, "" );
      gan_err_test_bool ( r0+height <= img->height, "gan_image_fill_const_window", GAN_ERROR_INCOMPATIBLE, "" );
      if ( pixel == NULL || !pixel->data.gl.b )
      {
         for ( i = 0; i < height; i++ )
            if ( !gan_bit_array_fill_part ( &img->ba[r0+i], c0, width, GAN_FALSE ) )
            {
               gan_err_register ( "gan_image_fill_const_window", GAN_ERROR_INCOMPATIBLE, "" );
               return GAN_FALSE;
            }
      }
      else
      {
         for ( i = 0; i < height; i++ )
            if ( !gan_bit_array_fill_part ( &img->ba[r0+i], c0, width, GAN_TRUE ) )
            {
               gan_err_register ( "gan_image_fill_const_window", GAN_ERROR_INCOMPATIBLE, "" );
               return GAN_FALSE;
            }
      }
               
      /* success */
      return GAN_TRUE;
   }

   /* point sub-image into main image */
   sub_image = gan_image_extract_s ( img, r0, c0, height, width,
                                     img->format, img->type, GAN_FALSE );
   if ( sub_image == NULL )
   {
      gan_err_register ( "gan_image_fill_const_window", GAN_ERROR_FAILURE, "");
      return GAN_FALSE;
   }

   /* fill sub-image with constant value */
   if ( pixel == NULL ? !gan_image_fill_zero ( sub_image )
                      : !gan_image_fill_const ( sub_image, pixel ) )
   {
      gan_err_register ( "gan_image_fill_const_window", GAN_ERROR_FAILURE, "");
      return GAN_FALSE;
   }

   /* success */
   gan_image_free(sub_image);
   return GAN_TRUE;
}
 
/**
 * \brief Fill a region of an image.
 * \param img The image to fill
 * \param mask Mask for the region to fill
 * \param pixel The pixel value with which to fill the subwindow
 * \return #GAN_TRUE on success, or #GAN_FALSE on failure.
 *
 * Fills a subwindow of image \a img with value \a pixel, the subwindow being
 * defined by the \a mask image.
 * A \c NULL \a pixel pointer indicates zero value.
 */
Gan_Bool
 gan_image_fill_const_mask ( Gan_Image *img, Gan_Image *mask,
                             Gan_Pixel *pixel )
{
   Gan_ImageWindow window;
   unsigned int row_max, col_max, c, r;

   if(mask == NULL)
   {
      if(pixel == NULL)
      {
         if(!gan_image_fill_zero(img))
         {
            gan_err_register ( "gan_image_fill_const_mask", GAN_ERROR_INCOMPATIBLE, "" );
            return GAN_FALSE;
         }
      }
      else
      {
         if(!gan_image_fill_const(img, pixel))
         {
            gan_err_register ( "gan_image_fill_const_mask", GAN_ERROR_INCOMPATIBLE, "" );
            return GAN_FALSE;
         }
      }

      return GAN_TRUE;
   }

   if(!gan_image_get_active_subwindow_b(mask, GAN_BIT_ALIGNMENT, &window))
   {
      gan_err_register ( "gan_image_fill_const_mask", GAN_ERROR_INCOMPATIBLE, "" );
      return GAN_FALSE;
   }

   row_max = window.r0+window.height;
   col_max = window.c0+window.width;

   if(pixel == NULL)
   {
      // set region to zero
      for(r = window.r0; r < row_max; r++)
         for(c = window.c0; c < col_max; c++)
            if(gan_image_get_pix_b(mask, r, c))
               if(!gan_image_set_pix_zero(img, r, c))
               {
                  gan_err_register ( "gan_image_fill_const_mask", GAN_ERROR_INCOMPATIBLE, "" );
                  return GAN_FALSE;
               }
   }
   else
   {
      for(r = window.r0; r < row_max; r++)
         for(c = window.c0; c < col_max; c++)
            if(gan_image_get_pix_b(mask, r, c))
               if(!gan_image_set_pix(img, r, c, pixel))
               {
                  gan_err_register ( "gan_image_fill_const_mask", GAN_ERROR_INCOMPATIBLE, "" );
                  return GAN_FALSE;
               }
   }

   return GAN_TRUE;
}

/**
 * \brief Merges a field into an image
 * \param field The field to merge, or its whole image if \a whole_image is \c GAN_TRUE
 * \param flip Whether to flip the image data in \a field
 * \param upper Whether to merge the upper field
 * \param whole_image Whether the field is stored in a whole frame or not
 * \param image The image to merge the field into
 * \return #GAN_TRUE on success, or #GAN_FALSE on failure.
 */
Gan_Bool gan_merge_field_into_image(const Gan_Image *field, Gan_Bool flip, Gan_Bool upper, Gan_Bool whole_image, Gan_Image *image)
{
   int irow;

   if(field->format != image->format || field->type != image->type || field->width != image->width)
   {
      gan_err_flush_trace();
      gan_err_register("gan_merge_field_into_image", GAN_ERROR_INCOMPATIBLE, "");
      return GAN_FALSE;
   }

   if((whole_image && field->height != image->height) || (!whole_image && 2*field->height != image->height))
   {
      gan_err_flush_trace();
      gan_err_register("gan_merge_field_into_image", GAN_ERROR_INCOMPATIBLE, "");
      return GAN_FALSE;
   }

   for(irow=(upper ? ((int)image->height-2) : ((int)image->height-1)); irow>=0; irow-=2)
   {
      /* get row in field */
      int frow;

      if(whole_image)
         frow = (flip ? (image->height-irow-1) : irow);
      else
         frow = (flip ? (field->height-irow/2-1) : (irow/2));
         
      memcpy(gan_image_get_pixptr(image, irow, 0), gan_image_get_pixptr(field, frow, 0),
             field->width*gan_image_pixel_size(image->format, image->type));
   }

   return GAN_TRUE;
}


/**
 * \}
 */

/**
 * \addtogroup ImageAccessPixel
 * \{
 */

/**
 * \brief Gets a pixel from an image.
 * \param img The image
 * \param row The row position of the pixel
 * \param col The column position of the pixel
 * \return The pixel at the given position
 *
 * Gets the pixel from image \a img at position \a row, \a col.
 * \sa gan_image_set_pix().
 */
Gan_Pixel
 gan_image_get_pix ( const Gan_Image *img, unsigned row, unsigned col )
{
   Gan_Pixel pixel;

   /* set format and type of pixel */
   pixel.format = img->format;
   pixel.type   = img->type;
   switch ( img->format )
   {
      case GAN_GREY_LEVEL_IMAGE:
        switch ( img->type )
        {
           case GAN_UCHAR:
             pixel.data.gl.uc = gan_image_get_pix_gl_uc ( img, row, col );
             break;

           case GAN_SHORT:
             pixel.data.gl.s = gan_image_get_pix_gl_s ( img, row, col );
             break;

           case GAN_USHORT:
             pixel.data.gl.us = gan_image_get_pix_gl_us ( img, row, col );
             break;

           case GAN_INT:
             pixel.data.gl.i = gan_image_get_pix_gl_i ( img, row, col );
             break;

           case GAN_UINT:
             pixel.data.gl.ui = gan_image_get_pix_gl_ui ( img, row, col );
             break;

           case GAN_FLOAT:
             pixel.data.gl.f = gan_image_get_pix_gl_f ( img, row, col );
             break;

           case GAN_DOUBLE:
             pixel.data.gl.d = gan_image_get_pix_gl_d ( img, row, col );
             break;

           case GAN_BOOL:
             pixel.data.gl.b = gan_image_get_pix_b ( img, row, col );
             break;

           case GAN_POINTER:
             pixel.data.gl.p = gan_image_get_pix_p ( img, row, col );
             break;

           case GAN_UINT10:
             pixel.data.gl.ui10 = gan_image_get_pix_gl_ui10 ( img, row, col );
             break;

           case GAN_UINT12:
             pixel.data.gl.ui12 = gan_image_get_pix_gl_ui12 ( img, row, col );
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_get_pix", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_GREY_LEVEL_ALPHA_IMAGE:
        switch ( img->type )
        {
           case GAN_UCHAR:
             pixel.data.gla.uc = gan_image_get_pix_gla_uc ( img, row, col );
             break;

           case GAN_SHORT:
             pixel.data.gla.s = gan_image_get_pix_gla_s ( img, row, col );
             break;

           case GAN_USHORT:
             pixel.data.gla.us = gan_image_get_pix_gla_us ( img, row, col );
             break;

           case GAN_INT:
             pixel.data.gla.i = gan_image_get_pix_gla_i ( img, row, col );
             break;

           case GAN_UINT:
             pixel.data.gla.ui = gan_image_get_pix_gla_ui ( img, row, col );
             break;

           case GAN_FLOAT:
             pixel.data.gla.f = gan_image_get_pix_gla_f ( img, row, col );
             break;

           case GAN_DOUBLE:
             pixel.data.gla.d = gan_image_get_pix_gla_d ( img, row, col );
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_get_pix", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_RGB_COLOUR_IMAGE:
        switch ( img->type )
        {
           case GAN_UCHAR:
             pixel.data.rgb.uc = gan_image_get_pix_rgb_uc ( img, row, col );
             break;

           case GAN_SHORT:
             pixel.data.rgb.s = gan_image_get_pix_rgb_s ( img, row, col );
             break;

           case GAN_USHORT:
             pixel.data.rgb.us = gan_image_get_pix_rgb_us ( img, row, col );
             break;

           case GAN_INT:
             pixel.data.rgb.i = gan_image_get_pix_rgb_i ( img, row, col );
             break;

           case GAN_UINT:
             pixel.data.rgb.ui = gan_image_get_pix_rgb_ui ( img, row, col );
             break;

           case GAN_FLOAT:
             pixel.data.rgb.f = gan_image_get_pix_rgb_f ( img, row, col );
             break;

           case GAN_DOUBLE:
             pixel.data.rgb.d = gan_image_get_pix_rgb_d ( img, row, col );
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_get_pix", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_RGB_COLOUR_ALPHA_IMAGE:
        switch ( img->type )
        {
           case GAN_UCHAR:
             pixel.data.rgba.uc = gan_image_get_pix_rgba_uc (img, row, col);
             break;

           case GAN_SHORT:
             pixel.data.rgba.s = gan_image_get_pix_rgba_s (img, row, col);
             break;

           case GAN_USHORT:
             pixel.data.rgba.us = gan_image_get_pix_rgba_us (img, row, col);
             break;

           case GAN_INT:
             pixel.data.rgba.i = gan_image_get_pix_rgba_i (img, row, col);
             break;

           case GAN_UINT:
             pixel.data.rgba.ui = gan_image_get_pix_rgba_ui (img, row, col);
             break;

           case GAN_FLOAT:
             pixel.data.rgba.f = gan_image_get_pix_rgba_f (img, row, col);
             break;

           case GAN_DOUBLE:
             pixel.data.rgba.d = gan_image_get_pix_rgba_d (img, row, col);
             break;

           case GAN_UINT12:
             pixel.data.rgba.ui12 = gan_image_get_pix_rgba_ui12 (img, row, col);
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_get_pix", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_BGR_COLOUR_IMAGE:
        switch ( img->type )
        {
           case GAN_UCHAR:
             pixel.data.bgr.uc = gan_image_get_pix_bgr_uc ( img, row, col );
             break;

           case GAN_SHORT:
             pixel.data.bgr.s = gan_image_get_pix_bgr_s ( img, row, col );
             break;

           case GAN_USHORT:
             pixel.data.bgr.us = gan_image_get_pix_bgr_us ( img, row, col );
             break;

           case GAN_INT:
             pixel.data.bgr.i = gan_image_get_pix_bgr_i ( img, row, col );
             break;

           case GAN_UINT:
             pixel.data.bgr.ui = gan_image_get_pix_bgr_ui ( img, row, col );
             break;

           case GAN_FLOAT:
             pixel.data.bgr.f = gan_image_get_pix_bgr_f ( img, row, col );
             break;

           case GAN_DOUBLE:
             pixel.data.bgr.d = gan_image_get_pix_bgr_d ( img, row, col );
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_get_pix", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_BGR_COLOUR_ALPHA_IMAGE:
        switch ( img->type )
        {
           case GAN_UCHAR:
             pixel.data.bgra.uc = gan_image_get_pix_bgra_uc (img, row, col);
             break;

           case GAN_SHORT:
             pixel.data.bgra.s = gan_image_get_pix_bgra_s (img, row, col);
             break;

           case GAN_USHORT:
             pixel.data.bgra.us = gan_image_get_pix_bgra_us (img, row, col);
             break;

           case GAN_INT:
             pixel.data.bgra.i = gan_image_get_pix_bgra_i (img, row, col);
             break;

           case GAN_UINT:
             pixel.data.bgra.ui = gan_image_get_pix_bgra_ui (img, row, col);
             break;

           case GAN_FLOAT:
             pixel.data.bgra.f = gan_image_get_pix_bgra_f (img, row, col);
             break;

           case GAN_DOUBLE:
             pixel.data.bgra.d = gan_image_get_pix_bgra_d (img, row, col);
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_get_pix", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_VECTOR_FIELD_2D:
        switch ( img->type )
        {
           case GAN_FLOAT:
             pixel.data.vfield2D.f = gan_image_get_pix_vfield2D_f(img, row, col);
             break;

           case GAN_DOUBLE:
             pixel.data.vfield2D.d = gan_image_get_pix_vfield2D_d(img, row, col);
             break;

           case GAN_SHORT:
             pixel.data.vfield2D.s = gan_image_get_pix_vfield2D_s(img, row, col);
             break;

           case GAN_INT:
             pixel.data.vfield2D.i = gan_image_get_pix_vfield2D_i(img, row, col);
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_get_pix", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_VECTOR_FIELD_3D:
        switch ( img->type )
        {
           case GAN_FLOAT:
             pixel.data.vfield3D.f = gan_image_get_pix_vfield3D_f(img, row, col);
             break;

           case GAN_DOUBLE:
             pixel.data.vfield3D.d = gan_image_get_pix_vfield3D_d(img, row, col);
             break;

           case GAN_SHORT:
             pixel.data.vfield3D.s = gan_image_get_pix_vfield3D_s(img, row, col);
             break;

           case GAN_INT:
             pixel.data.vfield3D.i = gan_image_get_pix_vfield3D_i(img, row, col);
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_get_pix", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_RGBX:
        switch ( img->type )
        {
           case GAN_UINT8:
             pixel.data.rgbx.ui8 = gan_image_get_pix_rgbx_ui8 ( img, row, col );
             break;

           case GAN_UINT10:
             pixel.data.rgbx.ui10 = gan_image_get_pix_rgbx_ui10 ( img, row, col );
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_get_pix", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_RGBAS:
        switch ( img->type )
        {
           case GAN_UINT10:
             pixel.data.rgbas.ui10 = gan_image_get_pix_rgbas_ui10 ( img, row, col );
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_get_pix", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_YUVX444:
        switch ( img->type )
        {
           case GAN_UINT8:
             pixel.data.yuvx444.ui8 = gan_image_get_pix_yuvx444_ui8 ( img, row, col );
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_get_pix", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_YUVA444:
        switch ( img->type )
        {
           case GAN_UINT8:
             pixel.data.yuva444.ui8 = gan_image_get_pix_yuva444_ui8 ( img, row, col );
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_get_pix", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_YUV422:
        switch ( img->type )
        {
           case GAN_UINT8:
             pixel.data.yuv422.ui8 = gan_image_get_pix_yuv422_ui8 ( img, row, col );
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_get_pix", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_image_get_pix", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
        break;
   }

   /* success */
   return pixel;
}

/**
 * \brief Gets a pointer to a pixel from an image.
 * \param img The image
 * \param row The row position of the pixel
 * \param col The column position of the pixel
 * \return A pointer to the pixel at the given position
 *
 * Gets the pixel pointer from image \a img at position \a row, \a col.
 * \sa gan_image_get_pix().
 */
void *
 gan_image_get_pixptr ( const Gan_Image *img, unsigned row, unsigned col )
{
   void *result=NULL;

   switch ( img->format )
   {
      case GAN_GREY_LEVEL_IMAGE:
        switch ( img->type )
        {
           case GAN_UCHAR:
             result = gan_image_get_pixptr_gl_uc ( img, row, col );
             break;

           case GAN_SHORT:
             result = gan_image_get_pixptr_gl_s ( img, row, col );
             break;

           case GAN_USHORT:
             result = gan_image_get_pixptr_gl_us ( img, row, col );
             break;

           case GAN_INT:
             result = gan_image_get_pixptr_gl_i ( img, row, col );
             break;

           case GAN_UINT:
             result = gan_image_get_pixptr_gl_ui ( img, row, col );
             break;

           case GAN_FLOAT:
             result = gan_image_get_pixptr_gl_f ( img, row, col );
             break;

           case GAN_DOUBLE:
             result = gan_image_get_pixptr_gl_d ( img, row, col );
             break;

           case GAN_POINTER:
             result = gan_image_get_pixptr_p ( img, row, col );
             break;

           case GAN_UINT10:
             result = gan_image_get_pixptr_gl_ui10 ( img, row, col );
             break;

           case GAN_UINT12:
             result = gan_image_get_pixptr_gl_ui12 ( img, row, col );
             break;

           case GAN_BOOL:
             if(col != 0)
             {
                gan_err_flush_trace();
                gan_err_register ( "gan_image_get_pixptr", GAN_ERROR_INCOMPATIBLE, "" );
                break;
             }

             result = img->ba[row].data;
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_get_pixptr", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_GREY_LEVEL_ALPHA_IMAGE:
        switch ( img->type )
        {
           case GAN_UCHAR:
             result = gan_image_get_pixptr_gla_uc ( img, row, col );
             break;

           case GAN_SHORT:
             result = gan_image_get_pixptr_gla_s ( img, row, col );
             break;

           case GAN_USHORT:
             result = gan_image_get_pixptr_gla_us ( img, row, col );
             break;

           case GAN_INT:
             result = gan_image_get_pixptr_gla_i ( img, row, col );
             break;

           case GAN_UINT:
             result = gan_image_get_pixptr_gla_ui ( img, row, col );
             break;

           case GAN_FLOAT:
             result = gan_image_get_pixptr_gla_f ( img, row, col );
             break;

           case GAN_DOUBLE:
             result = gan_image_get_pixptr_gla_d ( img, row, col );
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_get_pixptr", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_RGB_COLOUR_IMAGE:
        switch ( img->type )
        {
           case GAN_UCHAR:
             result = gan_image_get_pixptr_rgb_uc ( img, row, col );
             break;

           case GAN_SHORT:
             result = gan_image_get_pixptr_rgb_s ( img, row, col );
             break;

           case GAN_USHORT:
             result = gan_image_get_pixptr_rgb_us ( img, row, col );
             break;

           case GAN_INT:
             result = gan_image_get_pixptr_rgb_i ( img, row, col );
             break;

           case GAN_UINT:
             result = gan_image_get_pixptr_rgb_ui ( img, row, col );
             break;

           case GAN_FLOAT:
             result = gan_image_get_pixptr_rgb_f ( img, row, col );
             break;

           case GAN_DOUBLE:
             result = gan_image_get_pixptr_rgb_d ( img, row, col );
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_get_pixptr", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_RGB_COLOUR_ALPHA_IMAGE:
        switch ( img->type )
        {
           case GAN_UCHAR:
             result = gan_image_get_pixptr_rgba_uc (img, row, col);
             break;

           case GAN_SHORT:
             result = gan_image_get_pixptr_rgba_s (img, row, col);
             break;

           case GAN_USHORT:
             result = gan_image_get_pixptr_rgba_us (img, row, col);
             break;

           case GAN_INT:
             result = gan_image_get_pixptr_rgba_i (img, row, col);
             break;

           case GAN_UINT:
             result = gan_image_get_pixptr_rgba_ui (img, row, col);
             break;

           case GAN_FLOAT:
             result = gan_image_get_pixptr_rgba_f (img, row, col);
             break;

           case GAN_DOUBLE:
             result = gan_image_get_pixptr_rgba_d (img, row, col);
             break;

           case GAN_UINT12:
             result = gan_image_get_pixptr_rgba_ui12 (img, row, col);
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_get_pixptr", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_BGR_COLOUR_IMAGE:
        switch ( img->type )
        {
           case GAN_UCHAR:
             result = gan_image_get_pixptr_bgr_uc ( img, row, col );
             break;

           case GAN_SHORT:
             result = gan_image_get_pixptr_bgr_s ( img, row, col );
             break;

           case GAN_USHORT:
             result = gan_image_get_pixptr_bgr_us ( img, row, col );
             break;

           case GAN_INT:
             result = gan_image_get_pixptr_bgr_i ( img, row, col );
             break;

           case GAN_UINT:
             result = gan_image_get_pixptr_bgr_ui ( img, row, col );
             break;

           case GAN_FLOAT:
             result = gan_image_get_pixptr_bgr_f ( img, row, col );
             break;

           case GAN_DOUBLE:
             result = gan_image_get_pixptr_bgr_d ( img, row, col );
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_get_pixptr", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_BGR_COLOUR_ALPHA_IMAGE:
        switch ( img->type )
        {
           case GAN_UCHAR:
             result = gan_image_get_pixptr_bgra_uc (img, row, col);
             break;

           case GAN_SHORT:
             result = gan_image_get_pixptr_bgra_s (img, row, col);
             break;

           case GAN_USHORT:
             result = gan_image_get_pixptr_bgra_us (img, row, col);
             break;

           case GAN_INT:
             result = gan_image_get_pixptr_bgra_i (img, row, col);
             break;

           case GAN_UINT:
             result = gan_image_get_pixptr_bgra_ui (img, row, col);
             break;

           case GAN_FLOAT:
             result = gan_image_get_pixptr_bgra_f (img, row, col);
             break;

           case GAN_DOUBLE:
             result = gan_image_get_pixptr_bgra_d (img, row, col);
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_get_pixptr", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_VECTOR_FIELD_2D:
        switch ( img->type )
        {
           case GAN_FLOAT:
             result = gan_image_get_pixptr_vfield2D_f(img, row, col);
             break;

           case GAN_DOUBLE:
             result = gan_image_get_pixptr_vfield2D_d(img, row, col);
             break;

           case GAN_SHORT:
             result = gan_image_get_pixptr_vfield2D_s(img, row, col);
             break;

           case GAN_INT:
             result = gan_image_get_pixptr_vfield2D_i(img, row, col);
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_get_pixptr", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_VECTOR_FIELD_3D:
        switch ( img->type )
        {
           case GAN_FLOAT:
             result = gan_image_get_pixptr_vfield3D_f(img, row, col);
             break;

           case GAN_DOUBLE:
             result = gan_image_get_pixptr_vfield3D_d(img, row, col);
             break;

           case GAN_SHORT:
             result = gan_image_get_pixptr_vfield3D_s(img, row, col);
             break;

           case GAN_INT:
             result = gan_image_get_pixptr_vfield3D_i(img, row, col);
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_get_pixptr", GAN_ERROR_ILLEGAL_IMAGE_TYPE,
                                "" );
             break;
        }
        break;

      case GAN_RGBX:
        switch ( img->type )
        {
           case GAN_UINT8:
             result = gan_image_get_pixptr_rgbx_ui8 ( img, row, col );
             break;

           case GAN_UINT10:
             result = gan_image_get_pixptr_rgbx_ui10 ( img, row, col );
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_get_pixptr", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_RGBAS:
        switch ( img->type )
        {
           case GAN_UINT10:
             result = gan_image_get_pixptr_rgbas_ui10 ( img, row, col );
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_get_pixptr", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_YUVX444:
        switch ( img->type )
        {
           case GAN_UINT8:
             result = gan_image_get_pixptr_yuvx444_ui8 ( img, row, col );
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_get_pixptr", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_YUVA444:
        switch ( img->type )
        {
           case GAN_UINT8:
             result = gan_image_get_pixptr_yuva444_ui8 ( img, row, col );
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_get_pixptr", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_YUV422:
        switch ( img->type )
        {
           case GAN_UINT8:
             result = gan_image_get_pixptr_yuv422_ui8 ( img, row, col );
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_get_pixptr", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_image_get_pixptr", GAN_ERROR_ILLEGAL_IMAGE_FORMAT,
                           "" );
        break;
   }

   if ( result == NULL )
   {
      gan_err_register ( "gan_image_get_pixptr", GAN_ERROR_FAILURE, "" );
      return NULL;
   }
      
   /* success */
   return result;
}

/**
 * \brief Gets a pixel from an image and compares it with zero.
 * \param img The image
 * \param row The row position of the pixel
 * \param col The column position of the pixel
 * \return #GAN_TRUE if the pixel is zero, #GAN_FALSE otherwise.
 *
 * Gets a pixel from an image and compares it with zero.
 * \sa gan_image_get_pix().
 */
Gan_Bool
 gan_image_get_pix_zero ( const Gan_Image *img, unsigned row, unsigned col )
{
   switch ( img->format )
   {
      case GAN_GREY_LEVEL_IMAGE:
        switch ( img->type )
        {
           case GAN_UCHAR:   return (Gan_Bool)( gan_image_get_pix_gl_uc ( img, row, col ) == 0 );
           case GAN_SHORT:   return (Gan_Bool)( gan_image_get_pix_gl_s ( img, row, col ) == 0 );
           case GAN_USHORT:  return (Gan_Bool)( gan_image_get_pix_gl_us ( img, row, col ) == 0 );
           case GAN_INT:     return (Gan_Bool)( gan_image_get_pix_gl_i ( img, row, col ) == 0 );
           case GAN_UINT:    return (Gan_Bool)( gan_image_get_pix_gl_ui ( img, row, col ) == 0 );
           case GAN_FLOAT:   return (Gan_Bool)( gan_image_get_pix_gl_f ( img, row, col ) == 0.0F );
           case GAN_DOUBLE:  return (Gan_Bool)( gan_image_get_pix_gl_d ( img, row, col ) == 0.0 );
           case GAN_BOOL:    return (Gan_Bool)( gan_image_get_pix_b ( img, row, col ) == GAN_FALSE );
           case GAN_POINTER: return (Gan_Bool)( gan_image_get_pix_p ( img, row, col ) == NULL );
           case GAN_UINT10:
           {
              Gan_YXPixel_ui10 pix = gan_image_get_pix_gl_ui10 ( img, row, col );

              return (Gan_Bool) ( pix.Y == 0 );
           }

           case GAN_UINT12:
           {
              Gan_YXPixel_ui12 pix = gan_image_get_pix_gl_ui12 ( img, row, col );

              return (Gan_Bool) ( pix.Y == 0 );
           }

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_get_pix_zero", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_GREY_LEVEL_ALPHA_IMAGE:
        switch ( img->type )
        {
           case GAN_UCHAR:
           {
              Gan_GLAPixel_uc pix = gan_image_get_pix_gla_uc ( img, row, col );

              return (Gan_Bool) ( pix.I == 0 && pix.A == 0 );
           }

           case GAN_SHORT:
           {
              Gan_GLAPixel_s pix = gan_image_get_pix_gla_s ( img, row, col );

              return (Gan_Bool) ( pix.I == 0 && pix.A == 0 );
           }

           case GAN_USHORT:
           {
              Gan_GLAPixel_us pix = gan_image_get_pix_gla_us ( img, row, col );

              return (Gan_Bool) ( pix.I == 0 && pix.A == 0 );
           }

           case GAN_INT:
           {
              Gan_GLAPixel_i pix = gan_image_get_pix_gla_i ( img, row, col );

              return (Gan_Bool) ( pix.I == 0 && pix.A == 0 );
           }

           case GAN_UINT:
           {
              Gan_GLAPixel_ui pix = gan_image_get_pix_gla_ui ( img, row, col );

              return (Gan_Bool) ( pix.I == 0 && pix.A == 0 );
           }

           case GAN_FLOAT:
           {
              Gan_GLAPixel_f pix = gan_image_get_pix_gla_f ( img, row, col );

              return (Gan_Bool) ( pix.I == 0.0F && pix.A == 0.0F );
           }

           case GAN_DOUBLE:
           {
              Gan_GLAPixel_d pix = gan_image_get_pix_gla_d ( img, row, col );

              return (Gan_Bool) ( pix.I == 0.0 && pix.A == 0.0 );
           }

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_get_pix_zero", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_RGB_COLOUR_IMAGE:
        switch ( img->type )
        {
           case GAN_UCHAR:
           {
              Gan_RGBPixel_uc pix = gan_image_get_pix_rgb_uc ( img, row, col );

              return (Gan_Bool) ( pix.R == 0 && pix.G == 0 && pix.B == 0 );
           }

           case GAN_SHORT:
           {
              Gan_RGBPixel_s pix = gan_image_get_pix_rgb_s ( img, row, col );

              return (Gan_Bool) ( pix.R == 0 && pix.G == 0 && pix.B == 0 );
           }

           case GAN_USHORT:
           {
              Gan_RGBPixel_us pix = gan_image_get_pix_rgb_us ( img, row, col );

              return (Gan_Bool) ( pix.R == 0 && pix.G == 0 && pix.B == 0 );
           }

           case GAN_INT:
           {
              Gan_RGBPixel_i pix = gan_image_get_pix_rgb_i ( img, row, col );

              return (Gan_Bool) ( pix.R == 0 && pix.G == 0 && pix.B == 0 );
           }

           case GAN_UINT:
           {
              Gan_RGBPixel_ui pix = gan_image_get_pix_rgb_ui ( img, row, col );

              return (Gan_Bool) ( pix.R == 0 && pix.G == 0 && pix.B == 0 );
           }

           case GAN_FLOAT:
           {
              Gan_RGBPixel_f pix = gan_image_get_pix_rgb_f ( img, row, col );

              return (Gan_Bool) ( pix.R == 0.0F && pix.G == 0.0F &&
                                  pix.B == 0.0F );
           }

           case GAN_DOUBLE:
           {
              Gan_RGBPixel_d pix = gan_image_get_pix_rgb_d ( img, row, col );

              return (Gan_Bool) ( pix.R == 0.0 && pix.G == 0.0 && pix.B == 0.0 );
           }

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_get_pix_zero", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_RGB_COLOUR_ALPHA_IMAGE:
        switch ( img->type )
        {
           case GAN_UCHAR:
           {
              Gan_RGBAPixel_uc pix = gan_image_get_pix_rgba_uc ( img, row, col );

              return (Gan_Bool) ( pix.R == 0 && pix.G == 0 &&
                                  pix.B == 0 && pix.A == 0 );
           }

           case GAN_SHORT:
           {
              Gan_RGBAPixel_s pix = gan_image_get_pix_rgba_s ( img, row, col );

              return (Gan_Bool) ( pix.R == 0 && pix.G == 0 &&
                                  pix.B == 0 && pix.A == 0 );
           }

           case GAN_USHORT:
           {
              Gan_RGBAPixel_us pix = gan_image_get_pix_rgba_us ( img, row, col );

              return (Gan_Bool) ( pix.R == 0 && pix.G == 0 &&
                                  pix.B == 0 && pix.A == 0 );
           }

           case GAN_INT:
           {
              Gan_RGBAPixel_i pix = gan_image_get_pix_rgba_i ( img, row, col );

              return (Gan_Bool) ( pix.R == 0 && pix.G == 0 &&
                                  pix.B == 0 && pix.A == 0 );
           }

           case GAN_UINT:
           {
              Gan_RGBAPixel_ui pix = gan_image_get_pix_rgba_ui ( img, row, col );

              return (Gan_Bool) ( pix.R == 0 && pix.G == 0 &&
                                  pix.B == 0 && pix.A == 0 );
           }

           case GAN_FLOAT:
           {
              Gan_RGBAPixel_f pix = gan_image_get_pix_rgba_f ( img, row, col );

              return (Gan_Bool) ( pix.R == 0.0F && pix.G == 0.0F &&
                                  pix.B == 0.0F && pix.A == 0.0F );
           }

           case GAN_DOUBLE:
           {
              Gan_RGBAPixel_d pix = gan_image_get_pix_rgba_d ( img, row, col );

              return (Gan_Bool) ( pix.R == 0.0 && pix.G == 0.0 &&
                                  pix.B == 0.0 && pix.A == 0.0 );
           }

           case GAN_UINT12:
           {
              Gan_RGBAPixel_ui12 pix = gan_image_get_pix_rgba_ui12 ( img, row, col );

              return (Gan_Bool) ( pix.R == 0 && pix.G == 0 &&
                                  pix.B == 0 && pix.A == 0 );
           }

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_get_pix_zero", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_BGR_COLOUR_IMAGE:
        switch ( img->type )
        {
           case GAN_UCHAR:
           {
              Gan_BGRPixel_uc pix = gan_image_get_pix_bgr_uc ( img, row, col );

              return (Gan_Bool) ( pix.R == 0 && pix.G == 0 && pix.B == 0 );
           }

           case GAN_SHORT:
           {
              Gan_BGRPixel_s pix = gan_image_get_pix_bgr_s ( img, row, col );

              return (Gan_Bool) ( pix.R == 0 && pix.G == 0 && pix.B == 0 );
           }

           case GAN_USHORT:
           {
              Gan_BGRPixel_us pix = gan_image_get_pix_bgr_us ( img, row, col );

              return (Gan_Bool) ( pix.R == 0 && pix.G == 0 && pix.B == 0 );
           }

           case GAN_INT:
           {
              Gan_BGRPixel_i pix = gan_image_get_pix_bgr_i ( img, row, col );

              return (Gan_Bool) ( pix.R == 0 && pix.G == 0 && pix.B == 0 );
           }

           case GAN_UINT:
           {
              Gan_BGRPixel_ui pix = gan_image_get_pix_bgr_ui ( img, row, col );

              return (Gan_Bool) ( pix.R == 0 && pix.G == 0 && pix.B == 0 );
           }

           case GAN_FLOAT:
           {
              Gan_BGRPixel_f pix = gan_image_get_pix_bgr_f ( img, row, col );

              return (Gan_Bool) ( pix.R == 0.0F && pix.G == 0.0F &&
                                  pix.B == 0.0F );
           }

           case GAN_DOUBLE:
           {
              Gan_BGRPixel_d pix = gan_image_get_pix_bgr_d ( img, row, col );

              return (Gan_Bool) ( pix.R == 0.0 && pix.G == 0.0 && pix.B == 0.0 );
           }

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_get_pix_zero", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_BGR_COLOUR_ALPHA_IMAGE:
        switch ( img->type )
        {
           case GAN_UCHAR:
           {
              Gan_BGRAPixel_uc pix = gan_image_get_pix_bgra_uc ( img, row, col );

              return (Gan_Bool) ( pix.R == 0 && pix.G == 0 &&
                                  pix.B == 0 && pix.A == 0 );
           }

           case GAN_SHORT:
           {
              Gan_BGRAPixel_s pix = gan_image_get_pix_bgra_s ( img, row, col );

              return (Gan_Bool) ( pix.R == 0 && pix.G == 0 &&
                                  pix.B == 0 && pix.A == 0 );
           }

           case GAN_USHORT:
           {
              Gan_BGRAPixel_us pix = gan_image_get_pix_bgra_us ( img, row, col );

              return (Gan_Bool) ( pix.R == 0 && pix.G == 0 &&
                                  pix.B == 0 && pix.A == 0 );
           }

           case GAN_INT:
           {
              Gan_BGRAPixel_i pix = gan_image_get_pix_bgra_i ( img, row, col );

              return (Gan_Bool) ( pix.R == 0 && pix.G == 0 &&
                                  pix.B == 0 && pix.A == 0 );
           }

           case GAN_UINT:
           {
              Gan_BGRAPixel_ui pix = gan_image_get_pix_bgra_ui ( img, row, col );

              return (Gan_Bool) ( pix.R == 0 && pix.G == 0 &&
                                  pix.B == 0 && pix.A == 0 );
           }

           case GAN_FLOAT:
           {
              Gan_BGRAPixel_f pix = gan_image_get_pix_bgra_f ( img, row, col );

              return (Gan_Bool) ( pix.R == 0.0F && pix.G == 0.0F &&
                                  pix.B == 0.0F && pix.A == 0.0F );
           }

           case GAN_DOUBLE:
           {
              Gan_BGRAPixel_d pix = gan_image_get_pix_bgra_d ( img, row, col );

              return (Gan_Bool) ( pix.R == 0.0 && pix.G == 0.0 &&
                                  pix.B == 0.0 && pix.A == 0.0 );
           }

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_get_pix_zero", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_VECTOR_FIELD_2D:
        switch ( img->type )
        {
           case GAN_FLOAT:
           {
              Gan_Vector2_f pix = gan_image_get_pix_vfield2D_f ( img, row, col );

              return (Gan_Bool) ( pix.x == 0.0F && pix.y == 0.0F );
           }

           case GAN_DOUBLE:
           {
              Gan_Vector2 pix = gan_image_get_pix_vfield2D_d ( img, row, col );

              return (Gan_Bool) ( pix.x == 0.0 && pix.y == 0.0 );
           }

           case GAN_SHORT:
           {
              Gan_Vector2_s pix = gan_image_get_pix_vfield2D_s ( img, row, col );

              return (Gan_Bool) ( pix.x == 0 && pix.y == 0 );
           }

           case GAN_INT:
           {
              Gan_Vector2_i pix = gan_image_get_pix_vfield2D_i ( img, row, col );

              return (Gan_Bool) ( pix.x == 0 && pix.y == 0 );
           }

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_get_pix_zero", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_VECTOR_FIELD_3D:
        switch ( img->type )
        {
           case GAN_FLOAT:
           {
              Gan_Vector3_f pix = gan_image_get_pix_vfield3D_f ( img, row, col );

              return (Gan_Bool) ( pix.x == 0.0F && pix.y == 0.0F &&
                                  pix.z == 0.0F );
           }

           case GAN_DOUBLE:
           {
              Gan_Vector3 pix = gan_image_get_pix_vfield3D_d ( img, row, col );

              return (Gan_Bool) ( pix.x == 0.0 && pix.y == 0.0 && pix.z == 0.0 );
           }

           case GAN_SHORT:
           {
              Gan_Vector3_s pix = gan_image_get_pix_vfield3D_s ( img, row, col );

              return (Gan_Bool) ( pix.x == 0 && pix.y == 0 && pix.z == 0 );
           }

           case GAN_INT:
           {
              Gan_Vector3_i pix = gan_image_get_pix_vfield3D_i ( img, row, col );

              return (Gan_Bool) ( pix.x == 0 && pix.y == 0 && pix.z == 0 );
           }

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_get_pix_zero", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_RGBX:
        switch ( img->type )
        {
           case GAN_UINT8:
           {
              Gan_RGBXPixel_ui8 pix = gan_image_get_pix_rgbx_ui8 ( img, row, col );

              return (Gan_Bool) ( pix.R == 0 && pix.G == 0 && pix.B == 0 );
           }

           case GAN_UINT10:
           {
              Gan_RGBXPixel_ui10 pix = gan_image_get_pix_rgbx_ui10 ( img, row, col );

              return (Gan_Bool) ( pix.R == 0 && pix.G == 0 && pix.B == 0 );
           }

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_get_pix_zero", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_RGBAS:
        switch ( img->type )
        {
           case GAN_UINT10:
           {
              Gan_RGBASPixel_ui10 pix = gan_image_get_pix_rgbas_ui10 ( img, row, col );

              return (Gan_Bool) ( pix.R == 0 && pix.G == 0 && pix.B == 0 );
           }

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_get_pix_zero", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_YUVX444:
        switch ( img->type )
        {
           case GAN_UINT8:
           {
              Gan_YUVX444Pixel_ui8 pix = gan_image_get_pix_yuvx444_ui8 ( img, row, col );

              return (Gan_Bool) ( pix.Y == 0 && pix.U == 0 && pix.V == 0 );
           }

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_get_pix_zero", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_YUVA444:
        switch ( img->type )
        {
           case GAN_UINT8:
           {
              Gan_YUVA444Pixel_ui8 pix = gan_image_get_pix_yuva444_ui8 ( img, row, col );

              return (Gan_Bool) ( pix.Y == 0 && pix.U == 0 && pix.V == 0 );
           }

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_get_pix_zero", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_YUV422:
        switch ( img->type )
        {
           case GAN_UINT8:
           {
              Gan_YUV422Pixel_ui8 pix = gan_image_get_pix_yuv422_ui8 ( img, row, col );

              return (Gan_Bool) ( pix.UV == 0 && pix.Y == 0 );
           }

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_get_pix_zero", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_image_get_pix_zero", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
        break;
   }

   /* shouldn't get here */
   assert(0);
   return GAN_FALSE;
}

/**
 * \brief Sets a pixel in an image.
 * \param img The image
 * \param row The row position of the pixel
 * \param col The column position of the pixel
 * \param pix The pixel to set
 * \return #GAN_TRUE on success, or #GAN_FALSE on failure.
 *
 * Sets the pixel an image \a img at position \a row, \a col to the given
 * value \a pix.
 *
 * \sa gan_image_get_pix().
 */
Gan_Bool
 gan_image_set_pix ( Gan_Image *img, unsigned row, unsigned col,
                     Gan_Pixel *pix )
{
   /* consistency check */
   gan_err_test_bool ( pix->format == img->format && pix->type == img->type,
                       "gan_image_set_pix", GAN_ERROR_INCOMPATIBLE, "" );

   switch ( img->format )
   {
      case GAN_GREY_LEVEL_IMAGE:
        switch ( img->type )
        {
           case GAN_UCHAR:   return gan_image_set_pix_gl_uc ( img, row, col, pix->data.gl.uc );
           case GAN_SHORT:   return gan_image_set_pix_gl_s ( img, row, col, pix->data.gl.s );
           case GAN_USHORT:  return gan_image_set_pix_gl_us ( img, row, col, pix->data.gl.us );
           case GAN_INT:     return gan_image_set_pix_gl_i ( img, row, col, pix->data.gl.i );
           case GAN_UINT:    return gan_image_set_pix_gl_ui ( img, row, col, pix->data.gl.ui );
           case GAN_FLOAT:   return gan_image_set_pix_gl_f ( img, row, col, pix->data.gl.f );
           case GAN_DOUBLE:  return gan_image_set_pix_gl_d ( img, row, col, pix->data.gl.d );
           case GAN_BOOL:    return gan_image_set_pix_b ( img, row, col, pix->data.gl.b );
           case GAN_POINTER: return gan_image_set_pix_p ( img, row, col, pix->data.gl.p );
           case GAN_UINT10:  return gan_image_set_pix_gl_ui10 ( img, row, col, &pix->data.gl.ui10 );
           case GAN_UINT12:  return gan_image_set_pix_gl_ui12 ( img, row, col, &pix->data.gl.ui12 );

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_set_pix", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_GREY_LEVEL_ALPHA_IMAGE:
        switch ( img->type )
        {
           case GAN_UCHAR:   return gan_image_set_pix_gla_uc ( img, row, col, &pix->data.gla.uc);
           case GAN_SHORT:   return gan_image_set_pix_gla_s ( img, row, col, &pix->data.gla.s );
           case GAN_USHORT:  return gan_image_set_pix_gla_us ( img, row, col, &pix->data.gla.us);
           case GAN_INT:     return gan_image_set_pix_gla_i ( img, row, col, &pix->data.gla.i );
           case GAN_UINT:    return gan_image_set_pix_gla_ui ( img, row, col, &pix->data.gla.ui);
           case GAN_FLOAT:   return gan_image_set_pix_gla_f ( img, row, col, &pix->data.gla.f);
           case GAN_DOUBLE:  return gan_image_set_pix_gla_d ( img, row, col, &pix->data.gla.d);

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_set_pix", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_RGB_COLOUR_IMAGE:
        switch ( img->type )
        {
           case GAN_UCHAR:  return gan_image_set_pix_rgb_uc ( img, row, col, &pix->data.rgb.uc);
           case GAN_SHORT:  return gan_image_set_pix_rgb_s ( img, row, col, &pix->data.rgb.s );
           case GAN_USHORT: return gan_image_set_pix_rgb_us ( img, row, col, &pix->data.rgb.us);
           case GAN_INT:    return gan_image_set_pix_rgb_i ( img, row, col, &pix->data.rgb.i );
           case GAN_UINT:   return gan_image_set_pix_rgb_ui ( img, row, col, &pix->data.rgb.ui);
           case GAN_FLOAT:  return gan_image_set_pix_rgb_f ( img, row, col, &pix->data.rgb.f );
           case GAN_DOUBLE: return gan_image_set_pix_rgb_d ( img, row, col, &pix->data.rgb.d );

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_set_pix", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_RGB_COLOUR_ALPHA_IMAGE:
        switch ( img->type )
        {
           case GAN_UCHAR:   return gan_image_set_pix_rgba_uc ( img, row, col, &pix->data.rgba.uc );
           case GAN_SHORT:   return gan_image_set_pix_rgba_s ( img, row, col, &pix->data.rgba.s );
           case GAN_USHORT:  return gan_image_set_pix_rgba_us ( img, row, col, &pix->data.rgba.us );
           case GAN_INT:     return gan_image_set_pix_rgba_i ( img, row, col, &pix->data.rgba.i );
           case GAN_UINT:    return gan_image_set_pix_rgba_ui ( img, row, col, &pix->data.rgba.ui );
           case GAN_FLOAT:   return gan_image_set_pix_rgba_f ( img, row, col, &pix->data.rgba.f );
           case GAN_DOUBLE:  return gan_image_set_pix_rgba_d ( img, row, col, &pix->data.rgba.d );
           case GAN_UINT12:  return gan_image_set_pix_rgba_ui12 ( img, row, col, &pix->data.rgba.ui12 );

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_set_pix", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_BGR_COLOUR_IMAGE:
        switch ( img->type )
        {
           case GAN_UCHAR:  return gan_image_set_pix_bgr_uc ( img, row, col, &pix->data.bgr.uc);
           case GAN_SHORT:  return gan_image_set_pix_bgr_s ( img, row, col, &pix->data.bgr.s );
           case GAN_USHORT: return gan_image_set_pix_bgr_us ( img, row, col, &pix->data.bgr.us);
           case GAN_INT:    return gan_image_set_pix_bgr_i ( img, row, col, &pix->data.bgr.i );
           case GAN_UINT:   return gan_image_set_pix_bgr_ui ( img, row, col, &pix->data.bgr.ui);
           case GAN_FLOAT:  return gan_image_set_pix_bgr_f ( img, row, col, &pix->data.bgr.f );
           case GAN_DOUBLE: return gan_image_set_pix_bgr_d ( img, row, col, &pix->data.bgr.d );

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_set_pix", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_BGR_COLOUR_ALPHA_IMAGE:
        switch ( img->type )
        {
           case GAN_UCHAR:   return gan_image_set_pix_bgra_uc ( img, row, col, &pix->data.bgra.uc );
           case GAN_SHORT:   return gan_image_set_pix_bgra_s ( img, row, col, &pix->data.bgra.s );
           case GAN_USHORT:  return gan_image_set_pix_bgra_us ( img, row, col, &pix->data.bgra.us );
           case GAN_INT:     return gan_image_set_pix_bgra_i ( img, row, col, &pix->data.bgra.i );
           case GAN_UINT:    return gan_image_set_pix_bgra_ui ( img, row, col, &pix->data.bgra.ui );
           case GAN_FLOAT:   return gan_image_set_pix_bgra_f ( img, row, col, &pix->data.bgra.f );
           case GAN_DOUBLE:  return gan_image_set_pix_bgra_d ( img, row, col, &pix->data.bgra.d );

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_set_pix", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_VECTOR_FIELD_2D:
        switch ( img->type )
        {
           case GAN_FLOAT:  return gan_image_set_pix_vfield2D_f ( img, row, col, &pix->data.vfield2D.f );
           case GAN_DOUBLE: return gan_image_set_pix_vfield2D_d ( img, row, col, &pix->data.vfield2D.d );
           case GAN_SHORT:  return gan_image_set_pix_vfield2D_s ( img, row, col, &pix->data.vfield2D.s );
           case GAN_INT:    return gan_image_set_pix_vfield2D_i ( img, row, col, &pix->data.vfield2D.i );

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_set_pix", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_VECTOR_FIELD_3D:
        switch ( img->type )
        {
           case GAN_FLOAT:  return gan_image_set_pix_vfield3D_f ( img, row, col, &pix->data.vfield3D.f );
           case GAN_DOUBLE: return gan_image_set_pix_vfield3D_d ( img, row, col, &pix->data.vfield3D.d );
           case GAN_SHORT:  return gan_image_set_pix_vfield3D_s ( img, row, col, &pix->data.vfield3D.s );
           case GAN_INT:    return gan_image_set_pix_vfield3D_i ( img, row, col,&pix->data.vfield3D.i );

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_set_pix", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_RGBX:
        switch ( img->type )
        {
           case GAN_UINT8:  return gan_image_set_pix_rgbx_ui8 ( img, row, col, &pix->data.rgbx.ui8);
           case GAN_UINT10: return gan_image_set_pix_rgbx_ui10 ( img, row, col, &pix->data.rgbx.ui10);

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_set_pix", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_RGBAS:
        switch ( img->type )
        {
           case GAN_UINT10: return gan_image_set_pix_rgbas_ui10 ( img, row, col, &pix->data.rgbas.ui10);

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_set_pix", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_YUVX444:
        switch ( img->type )
        {
           case GAN_UINT8: return gan_image_set_pix_yuvx444_ui8 ( img, row, col, &pix->data.yuvx444.ui8);

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_set_pix", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_YUVA444:
        switch ( img->type )
        {
           case GAN_UINT8: return gan_image_set_pix_yuva444_ui8 ( img, row, col, &pix->data.yuva444.ui8);

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_set_pix", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_YUV422:
        switch ( img->type )
        {
           case GAN_UINT8: return gan_image_set_pix_yuv422_ui8 ( img, row, col, &pix->data.yuv422.ui8);

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_set_pix", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_image_set_pix", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
        break;
   }

   /* shouldn't get here */
   assert(0);
   return GAN_FALSE;
}

/**
 * \brief Sets a pixel in an image to zero.
 * \param img The image
 * \param row The row position of the pixel
 * \param col The column position of the pixel
 * \return #GAN_TRUE on success, or #GAN_FALSE on failure.
 *
 * Sets the pixel in image \a img at position \a row, \a col to zero.
 * \sa gan_image_set_pix().
 */
Gan_Bool
 gan_image_set_pix_zero ( Gan_Image *img, unsigned row, unsigned col )
{
   switch ( img->format )
   {
      case GAN_GREY_LEVEL_IMAGE:
        switch ( img->type )
        {
           case GAN_UCHAR:   return gan_image_set_pix_gl_uc ( img, row, col, 0 );
           case GAN_SHORT:   return gan_image_set_pix_gl_s ( img, row, col, 0 );
           case GAN_USHORT:  return gan_image_set_pix_gl_us ( img, row, col, 0 );
           case GAN_INT:     return gan_image_set_pix_gl_i ( img, row, col, 0 );
           case GAN_UINT:    return gan_image_set_pix_gl_ui ( img, row, col, 0 );
           case GAN_FLOAT:   return gan_image_set_pix_gl_f ( img, row, col, 0.0F );
           case GAN_DOUBLE:  return gan_image_set_pix_gl_d ( img, row, col, 0.0 );
           case GAN_BOOL:    return gan_image_set_pix_b ( img, row, col, GAN_FALSE );
           case GAN_POINTER: return gan_image_set_pix_p ( img, row, col, NULL );
           case GAN_UINT10:
           {
              Gan_YXPixel_ui10 pix = {0,0};
            
              return gan_image_set_pix_gl_ui10 ( img, row, col, &pix );
           }

           case GAN_UINT12:
           {
              Gan_YXPixel_ui12 pix = {0,0};
            
              return gan_image_set_pix_gl_ui12 ( img, row, col, &pix );
           }

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_set_pix_zero", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_GREY_LEVEL_ALPHA_IMAGE:
        switch ( img->type )
        {
           case GAN_UCHAR:
           {
              Gan_GLAPixel_uc pix = {0,0};
            
              return gan_image_set_pix_gla_uc ( img, row, col, &pix );
           }

           case GAN_SHORT:
           {
              Gan_GLAPixel_s pix = {0,0};
            
              return gan_image_set_pix_gla_s ( img, row, col, &pix );
           }

           case GAN_USHORT:
           {
              Gan_GLAPixel_us pix = {0,0};
            
              return gan_image_set_pix_gla_us ( img, row, col, &pix );
           }

           case GAN_INT:
           {
              Gan_GLAPixel_i pix = {0,0};
            
              return gan_image_set_pix_gla_i ( img, row, col, &pix );
           }

           case GAN_UINT:
           {
              Gan_GLAPixel_ui pix = {0,0};
            
              return gan_image_set_pix_gla_ui ( img, row, col, &pix );
           }

           case GAN_FLOAT:
           {
              Gan_GLAPixel_f pix = {0.0F,0.0F};
            
              return gan_image_set_pix_gla_f ( img, row, col, &pix );
           }

           case GAN_DOUBLE:
           {
              Gan_GLAPixel_d pix = {0.0,0.0};
            
              return gan_image_set_pix_gla_d ( img, row, col, &pix );
           }

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_set_pix_zero", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_RGB_COLOUR_IMAGE:
        switch ( img->type )
        {
           case GAN_UCHAR:
           {
              Gan_RGBPixel_uc pix = {0,0,0};
            
              return gan_image_set_pix_rgb_uc ( img, row, col, &pix );
           }

           case GAN_SHORT:
           {
              Gan_RGBPixel_s pix = {0,0,0};
            
              return gan_image_set_pix_rgb_s ( img, row, col, &pix );
           }

           case GAN_USHORT:
           {
              Gan_RGBPixel_us pix = {0,0,0};
            
              return gan_image_set_pix_rgb_us ( img, row, col, &pix );
           }

           case GAN_INT:
           {
              Gan_RGBPixel_i pix = {0,0,0};
            
              return gan_image_set_pix_rgb_i ( img, row, col, &pix );
           }

           case GAN_UINT:
           {
              Gan_RGBPixel_ui pix = {0,0,0};
            
              return gan_image_set_pix_rgb_ui ( img, row, col, &pix );
           }

           case GAN_FLOAT:
           {
              Gan_RGBPixel_f pix = {0.0F,0.0F,0.0F};
            
              return gan_image_set_pix_rgb_f ( img, row, col, &pix );
           }

           case GAN_DOUBLE:
           {
              Gan_RGBPixel_d pix = {0.0,0.0,0.0};
            
              return gan_image_set_pix_rgb_d ( img, row, col, &pix );
           }

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_set_pix_zero", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_RGB_COLOUR_ALPHA_IMAGE:
        switch ( img->type )
        {
           case GAN_UCHAR:
           {
              Gan_RGBAPixel_uc pix = {0,0,0,0};
            
              return gan_image_set_pix_rgba_uc ( img, row, col, &pix );
           }

           case GAN_SHORT:
           {
              Gan_RGBAPixel_s pix = {0,0,0,0};
            
              return gan_image_set_pix_rgba_s ( img, row, col, &pix );
           }

           case GAN_USHORT:
           {
              Gan_RGBAPixel_us pix = {0,0,0,0};
            
              return gan_image_set_pix_rgba_us ( img, row, col, &pix );
           }

           case GAN_INT:
           {
              Gan_RGBAPixel_i pix = {0,0,0,0};
            
              return gan_image_set_pix_rgba_i ( img, row, col, &pix );
           }

           case GAN_UINT:
           {
              Gan_RGBAPixel_ui pix = {0,0,0,0};
            
              return gan_image_set_pix_rgba_ui ( img, row, col, &pix );
           }

           case GAN_FLOAT:
           {
              Gan_RGBAPixel_f pix = {0.0F,0.0F,0.0F,0.0F};
            
              return gan_image_set_pix_rgba_f ( img, row, col, &pix );
           }

           case GAN_DOUBLE:
           {
              Gan_RGBAPixel_d pix = {0.0,0.0,0.0,0.0};
            
              return gan_image_set_pix_rgba_d ( img, row, col, &pix );
           }

           case GAN_UINT12:
           {
              Gan_RGBAPixel_ui12 pix = {0,0,0,0};
            
              return gan_image_set_pix_rgba_ui12 ( img, row, col, &pix );
           }

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_set_pix_zero", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_BGR_COLOUR_IMAGE:
        switch ( img->type )
        {
           case GAN_UCHAR:
           {
              Gan_BGRPixel_uc pix = {0,0,0};
            
              return gan_image_set_pix_bgr_uc ( img, row, col, &pix );
           }

           case GAN_SHORT:
           {
              Gan_BGRPixel_s pix = {0,0,0};
            
              return gan_image_set_pix_bgr_s ( img, row, col, &pix );
           }

           case GAN_USHORT:
           {
              Gan_BGRPixel_us pix = {0,0,0};
            
              return gan_image_set_pix_bgr_us ( img, row, col, &pix );
           }

           case GAN_INT:
           {
              Gan_BGRPixel_i pix = {0,0,0};
            
              return gan_image_set_pix_bgr_i ( img, row, col, &pix );
           }

           case GAN_UINT:
           {
              Gan_BGRPixel_ui pix = {0,0,0};
            
              return gan_image_set_pix_bgr_ui ( img, row, col, &pix );
           }

           case GAN_FLOAT:
           {
              Gan_BGRPixel_f pix = {0.0F,0.0F,0.0F};
            
              return gan_image_set_pix_bgr_f ( img, row, col, &pix );
           }

           case GAN_DOUBLE:
           {
              Gan_BGRPixel_d pix = {0.0,0.0,0.0};
            
              return gan_image_set_pix_bgr_d ( img, row, col, &pix );
           }

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_set_pix_zero", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_BGR_COLOUR_ALPHA_IMAGE:
        switch ( img->type )
        {
           case GAN_UCHAR:
           {
              Gan_BGRAPixel_uc pix = {0,0,0,0};
            
              return gan_image_set_pix_bgra_uc ( img, row, col, &pix );
           }

           case GAN_SHORT:
           {
              Gan_BGRAPixel_s pix = {0,0,0,0};
            
              return gan_image_set_pix_bgra_s ( img, row, col, &pix );
           }

           case GAN_USHORT:
           {
              Gan_BGRAPixel_us pix = {0,0,0,0};
            
              return gan_image_set_pix_bgra_us ( img, row, col, &pix );
           }

           case GAN_INT:
           {
              Gan_BGRAPixel_i pix = {0,0,0,0};
            
              return gan_image_set_pix_bgra_i ( img, row, col, &pix );
           }

           case GAN_UINT:
           {
              Gan_BGRAPixel_ui pix = {0,0,0,0};
            
              return gan_image_set_pix_bgra_ui ( img, row, col, &pix );
           }

           case GAN_FLOAT:
           {
              Gan_BGRAPixel_f pix = {0.0F,0.0F,0.0F,0.0F};
            
              return gan_image_set_pix_bgra_f ( img, row, col, &pix );
           }

           case GAN_DOUBLE:
           {
              Gan_BGRAPixel_d pix = {0.0,0.0,0.0,0.0};
            
              return gan_image_set_pix_bgra_d ( img, row, col, &pix );
           }

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_set_pix_zero", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_VECTOR_FIELD_2D:
        switch ( img->type )
        {
           case GAN_FLOAT:
           {
              Gan_Vector2_f pix = {0.0F,0.0F};
            
              return gan_image_set_pix_vfield2D_f ( img, row, col, &pix );
           }

           case GAN_DOUBLE:
           {
              Gan_Vector2 pix = {0.0,0.0};
            
              return gan_image_set_pix_vfield2D_d ( img, row, col, &pix );
           }

           case GAN_SHORT:
           {
              Gan_Vector2_s pix = {0,0};
            
              return gan_image_set_pix_vfield2D_s ( img, row, col, &pix );
           }

           case GAN_INT:
           {
              Gan_Vector2_i pix = {0,0};
            
              return gan_image_set_pix_vfield2D_i ( img, row, col, &pix );
           }

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_set_pix_zero", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_VECTOR_FIELD_3D:
        switch ( img->type )
        {
           case GAN_FLOAT:
           {
              Gan_Vector3_f pix = {0.0F,0.0F,0.0F};
            
              return gan_image_set_pix_vfield3D_f ( img, row, col, &pix );
           }

           case GAN_DOUBLE:
           {
              Gan_Vector3 pix = {0.0,0.0,0.0};
            
              return gan_image_set_pix_vfield3D_d ( img, row, col, &pix );
           }

           case GAN_SHORT:
           {
              Gan_Vector3_s pix = {0,0,0};
            
              return gan_image_set_pix_vfield3D_s ( img, row, col, &pix );
           }

           case GAN_INT:
           {
              Gan_Vector3_i pix = {0,0,0};
            
              return gan_image_set_pix_vfield3D_i ( img, row, col, &pix );
           }

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_set_pix_zero", GAN_ERROR_ILLEGAL_IMAGE_TYPE,
                                "" );
             break;
        }
        break;

      case GAN_RGBX:
        switch ( img->type )
        {
           case GAN_UINT8:
           {
              Gan_RGBXPixel_ui8 pix = {0,0,0};
            
              return gan_image_set_pix_rgbx_ui8 ( img, row, col, &pix );
           }

           case GAN_UINT10:
           {
              Gan_RGBXPixel_ui10 pix = {0,0,0};
            
              return gan_image_set_pix_rgbx_ui10 ( img, row, col, &pix );
           }

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_set_pix_zero", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_RGBAS:
        switch ( img->type )
        {
           case GAN_UINT10:
           {
              Gan_RGBASPixel_ui10 pix = {0,0,0};
            
              return gan_image_set_pix_rgbas_ui10 ( img, row, col, &pix );
           }

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_set_pix_zero", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_YUVX444:
        switch ( img->type )
        {
           case GAN_UINT8:
           {
              Gan_YUVX444Pixel_ui8 pix = {0,0,0};
            
              return gan_image_set_pix_yuvx444_ui8 ( img, row, col, &pix );
           }

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_set_pix_zero", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_YUVA444:
        switch ( img->type )
        {
           case GAN_UINT8:
           {
              Gan_YUVA444Pixel_ui8 pix = {0,0,0};
            
              return gan_image_set_pix_yuva444_ui8 ( img, row, col, &pix );
           }

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_set_pix_zero", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_YUV422:
        switch ( img->type )
        {
           case GAN_UINT8:
           {
              Gan_YUV422Pixel_ui8 pix = {0,0};
            
              return gan_image_set_pix_yuv422_ui8 ( img, row, col, &pix );
           }

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_set_pix_zero", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_image_set_pix_zero", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
        break;
   }

   /* shouldn't get here */
   assert(0);
   return GAN_FALSE;
}

/**
 * \}
 */

/**
 * \addtogroup ImageExtract
 * \{
 */

/**
 * \brief Computes bounding box of non-zero pixels in grey-level image.
 * \return #GAN_TRUE on success, or #GAN_FALSE on failure.
 *
 * Computes image window \a subwin which covers all pixels in given grey-level
 * \a image which are non-zero. If \a image is zero everywhere the fields of
 * the \a subwin structure will be set to zero.
 */
Gan_Bool
 gan_image_get_active_subwindow ( const Gan_Image *image,
                                  Gan_ImageWindow *subwin )
{
   switch ( image->format )
   {
      case GAN_GREY_LEVEL_IMAGE:
        switch ( image->type )
        {
           case GAN_UCHAR:   return gan_image_get_active_subwindow_gl_uc ( image, subwin );
           case GAN_SHORT:   return gan_image_get_active_subwindow_gl_s ( image, subwin );
           case GAN_USHORT:  return gan_image_get_active_subwindow_gl_us ( image, subwin );
           case GAN_INT:     return gan_image_get_active_subwindow_gl_i ( image, subwin );
           case GAN_UINT:    return gan_image_get_active_subwindow_gl_ui ( image, subwin );
           case GAN_FLOAT:   return gan_image_get_active_subwindow_gl_f ( image, subwin );
           case GAN_DOUBLE:  return gan_image_get_active_subwindow_gl_d ( image, subwin );
           case GAN_BOOL:    return gan_image_get_active_subwindow_b ( image, GAN_BIT_ALIGNMENT, subwin );
           case GAN_POINTER: return gan_image_get_active_subwindow_p ( image, subwin );
           case GAN_UINT10:  return gan_image_get_active_subwindow_gl_ui10 ( image, subwin );
           case GAN_UINT12:  return gan_image_get_active_subwindow_gl_ui12 ( image, subwin );

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_get_active_subregion", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;
#if 0
      case GAN_GREY_LEVEL_ALPHA_IMAGE:
        switch ( image->type )
        {
           case GAN_UCHAR:  return gan_image_get_active_subwindow_gla_uc ( image, subwin );
           case GAN_SHORT:  return gan_image_get_active_subwindow_gla_s ( image, subwin );
           case GAN_USHORT: return gan_image_get_active_subwindow_gla_us ( image, subwin );
           case GAN_INT:    return gan_image_get_active_subwindow_gla_i ( image, subwin );
           case GAN_UINT:   return gan_image_get_active_subwindow_gla_ui ( image, subwin );
           case GAN_FLOAT:  return gan_image_get_active_subwindow_gla_f ( image, subwin );
           case GAN_DOUBLE: return gan_image_get_active_subwindow_gla_d ( image, subwin );

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_get_active_subregion",
                                GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_RGB_COLOUR_IMAGE:
        switch ( image->type )
        {
           case GAN_UCHAR:   return gan_image_get_active_subwindow_rgb_uc ( image, subwin );
           case GAN_SHORT:   return gan_image_get_active_subwindow_rgb_s ( image, subwin );
           case GAN_USHORT:  return gan_image_get_active_subwindow_rgb_us ( image, subwin );
           case GAN_INT:     return gan_image_get_active_subwindow_rgb_i ( image, subwin );
           case GAN_UINT:    return gan_image_get_active_subwindow_rgb_ui ( image, subwin );
           case GAN_FLOAT:   return gan_image_get_active_subwindow_rgb_f ( image, subwin );
           case GAN_DOUBLE:  return gan_image_get_active_subwindow_rgb_d ( image, subwin );

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_get_active_subregion", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_RGB_COLOUR_ALPHA_IMAGE:
        switch ( image->type )
        {
           case GAN_UCHAR:   return gan_image_get_active_subwindow_rgba_uc ( image, subwin );
           case GAN_SHORT:   return gan_image_get_active_subwindow_rgba_s ( image, subwin );
           case GAN_USHORT:  return gan_image_get_active_subwindow_rgba_us ( image, subwin );
           case GAN_INT:     return gan_image_get_active_subwindow_rgba_i ( image, subwin );
           case GAN_UINT:    return gan_image_get_active_subwindow_rgba_ui ( image, subwin );
           case GAN_FLOAT:   return gan_image_get_active_subwindow_rgba_f ( image, subwin );
           case GAN_DOUBLE:  return gan_image_get_active_subwindow_rgba_d ( image, subwin );
           case GAN_UINT12:  return gan_image_get_active_subwindow_rgba_ui12 ( image, subwin );

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_get_active_subregion", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_BGR_COLOUR_IMAGE:
        switch ( image->type )
        {
           case GAN_UCHAR:   return gan_image_get_active_subwindow_bgr_uc ( image, subwin );
           case GAN_SHORT:   return gan_image_get_active_subwindow_bgr_s ( image, subwin );
           case GAN_USHORT:  return gan_image_get_active_subwindow_bgr_us ( image, subwin );
           case GAN_INT:     return gan_image_get_active_subwindow_bgr_i ( image, subwin );
           case GAN_UINT:    return gan_image_get_active_subwindow_bgr_ui ( image, subwin );
           case GAN_FLOAT:   return gan_image_get_active_subwindow_bgr_f ( image, subwin );
           case GAN_DOUBLE:  return gan_image_get_active_subwindow_bgr_d ( image, subwin );

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_get_active_subregion", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_BGR_COLOUR_ALPHA_IMAGE:
        switch ( image->type )
        {
           case GAN_UCHAR:   return gan_image_get_active_subwindow_bgra_uc ( image, subwin );
           case GAN_SHORT:   return gan_image_get_active_subwindow_bgra_s ( image, subwin );
           case GAN_USHORT:  return gan_image_get_active_subwindow_bgra_us ( image, subwin );
           case GAN_INT:     return gan_image_get_active_subwindow_bgra_i ( image, subwin );
           case GAN_UINT:    return gan_image_get_active_subwindow_bgra_ui ( image, subwin );
           case GAN_FLOAT:   return gan_image_get_active_subwindow_bgra_f ( image, subwin );
           case GAN_DOUBLE:  return gan_image_get_active_subwindow_bgra_d ( image, subwin );

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_get_active_subregion", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_VECTOR_FIELD_2D:
        switch ( image->type )
        {
           case GAN_SHORT:  return gan_image_get_active_subwindow_vfield2D_s ( image, subwin );
           case GAN_INT:    return gan_image_get_active_subwindow_vfield2D_i ( image, subwin );
           case GAN_FLOAT:  return gan_image_get_active_subwindow_vfield2D_f ( image, subwin );
           case GAN_DOUBLE: return gan_image_get_active_subwindow_vfield2D_d ( image, subwin );

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_get_active_subregion", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_VECTOR_FIELD_3D:
        switch ( image->type )
        {
           case GAN_SHORT:  return gan_image_get_active_subwindow_vfield3D_s ( image, subwin );
           case GAN_INT:    return gan_image_get_active_subwindow_vfield3D_i ( image, subwin );
           case GAN_FLOAT:  return gan_image_get_active_subwindow_vfield3D_f ( image, subwin );
           case GAN_DOUBLE: return gan_image_get_active_subwindow_vfield3D_d ( image, subwin );

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_get_active_subregion", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;
#endif
      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_image_get_active_subwindow", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
        break;
   }

   /* shouldn't get here */
   assert(0);
   return GAN_FALSE;
}

/**
 * \brief Intersect subwindows.
 * \param window1 First input subwindow
 * \param window2 First input subwindow
 * \param resultwindow Result window
 *
 * Intersect subwindows, writing the intersection into \a resultwindow. If the intersection is empty,
 * \a resultwindow is filled with zeros.
 */
void gan_image_intersect_subwindows_q ( const Gan_ImageWindow *window1, const Gan_ImageWindow *window2,
                                        Gan_ImageWindow *resultwindow )
{
   *resultwindow = gan_image_intersect_subwindows_s(window1, window2);
}

/**
 * \brief Intersect subwindows.
 * \param window1 First input subwindow
 * \param window2 First input subwindow
 *
 * Intersect subwindows, returning the intersection as a new subwindow.
 * If the intersection is empty, the result is filled with zeros.
 */
Gan_ImageWindow gan_image_intersect_subwindows_s ( const Gan_ImageWindow *window1, const Gan_ImageWindow *window2 )
{
   Gan_ImageWindow iwResult;

   if(window1->c0+window1->width  < window2->c0 || window2->c0+window2->width  < window1->c0 ||
      window1->r0+window1->height < window2->r0 || window2->r0+window2->height < window1->r0)
      iwResult.c0 = iwResult.r0 = iwResult.width = iwResult.height = 0;
   else
   {
      iwResult.c0 = gan_max2_ui(window1->c0, window2->c0);
      iwResult.r0 = gan_max2_ui(window1->r0, window2->r0);
      iwResult.width  = gan_min2_ui(window1->c0+window1->width,  window2->c0+window2->width)  - iwResult.c0;
      iwResult.height = gan_min2_ui(window1->r0+window1->height, window2->r0+window2->height) - iwResult.r0;
   }
   
   return iwResult;
}

/**
 * \brief Intersect subwindows in-place.
 * \param sourcedestwindow Input/output subwindow
 * \param window Second input subwindow
 *
 * Intersect subwindows, writing the intersection in-place back into \a sourcedestwindow.
 * If the intersection is empty, \a sourcedestwindow is filled with zeros.
 */
void gan_image_intersect_subwindows_i ( Gan_ImageWindow *sourcedestwindow, const Gan_ImageWindow *window )
{
   *sourcedestwindow = gan_image_intersect_subwindows_s(sourcedestwindow, window);
}

/**
 * \brief Adjust a subwindow to fit inside an image
 * \param subwin Input/output subwindow
 * \param image Image to use to fit
 */
void gan_image_fit_subwindow ( Gan_ImageWindow *subwin, const Gan_Image *image )
{
   if(subwin->c0+subwin->width > (int)image->width)
   {
      subwin->width = (int)image->width-subwin->c0;
      if(subwin->width <= 0)
      {
         subwin->c0 = subwin->r0 = subwin->width = subwin->height = 0;
         return;
      }
   }

   if(subwin->r0+subwin->height > (int)image->height)
   {
      subwin->height = (int)image->height-subwin->r0;
      if(subwin->height <= 0)
      {
         subwin->c0 = subwin->r0 = subwin->width = subwin->height = 0;
         return;
      }
   }
   
   if(subwin->c0 < 0)
   {
      subwin->width += subwin->c0;
      if(subwin->width <= 0)
      {
         subwin->c0 = subwin->r0 = subwin->width = subwin->height = 0;
         return;
      }

      subwin->c0 = 0;
   }

   if(subwin->r0 < 0)
   {
      subwin->height += subwin->r0;
      if(subwin->height <= 0)
      {
         subwin->c0 = subwin->r0 = subwin->width = subwin->height = 0;
         return;
      }

      subwin->r0 = 0;
   }
}


/**
 * \}
 */

/**
 * \addtogroup ImageFill
 * \{
 */

/**
 * \brief Clear image to zero except in specified rectangular region.
 * \return #GAN_TRUE on success, or #GAN_FALSE on failure.
 *
 * Clear \a image to zero except in specified rectangular region.
 */
Gan_Bool
 gan_image_mask_window ( Gan_Image *image,
                         unsigned r0,     unsigned c0,
                         unsigned height, unsigned width )
{
   switch ( image->format )
   {
      case GAN_GREY_LEVEL_IMAGE:
        switch ( image->type )
        {
           case GAN_UCHAR:
             return gan_image_mask_window_gl_uc ( image, r0, c0, height, width );

           case GAN_SHORT:
             return gan_image_mask_window_gl_s ( image, r0, c0, height, width );

           case GAN_USHORT:
             return gan_image_mask_window_gl_us ( image, r0, c0, height, width );

           case GAN_INT:
             return gan_image_mask_window_gl_i ( image, r0, c0, height, width );

           case GAN_UINT:
             return gan_image_mask_window_gl_ui ( image, r0, c0, height, width );

           case GAN_FLOAT:
             return gan_image_mask_window_gl_f ( image, r0, c0, height, width );

           case GAN_DOUBLE:
             return gan_image_mask_window_gl_d ( image, r0, c0, height, width );

           case GAN_BOOL:
             return gan_image_mask_window_b ( image, r0, c0, height, width );

           case GAN_POINTER:
             return gan_image_mask_window_p ( image, r0, c0, height, width );

           case GAN_UINT10:
             return gan_image_mask_window_gl_ui10 ( image, r0, c0, height, width );

           case GAN_UINT12:
             return gan_image_mask_window_gl_ui12 ( image, r0, c0, height, width );

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_mask_window", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_GREY_LEVEL_ALPHA_IMAGE:
        switch ( image->type )
        {
           case GAN_UCHAR:
             return gan_image_mask_window_gla_uc ( image, r0, c0, height, width );

           case GAN_SHORT:
             return gan_image_mask_window_gla_s ( image, r0, c0, height, width );

           case GAN_USHORT:
             return gan_image_mask_window_gla_us ( image, r0, c0, height, width );

           case GAN_INT:
             return gan_image_mask_window_gla_i ( image, r0, c0, height, width );

           case GAN_UINT:
             return gan_image_mask_window_gla_ui ( image, r0, c0, height, width );

           case GAN_FLOAT:
             return gan_image_mask_window_gla_f ( image, r0, c0, height, width );

           case GAN_DOUBLE:
             return gan_image_mask_window_gla_d ( image, r0, c0, height, width );

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_mask_window", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_RGB_COLOUR_IMAGE:
        switch ( image->type )
        {
           case GAN_UCHAR:
             return gan_image_mask_window_rgb_uc ( image, r0, c0, height, width );

           case GAN_SHORT:
             return gan_image_mask_window_rgb_s ( image, r0, c0, height, width );

           case GAN_USHORT:
             return gan_image_mask_window_rgb_us ( image, r0, c0, height, width );

           case GAN_INT:
             return gan_image_mask_window_rgb_i ( image, r0, c0, height, width );

           case GAN_UINT:
             return gan_image_mask_window_rgb_ui ( image, r0, c0, height, width );

           case GAN_FLOAT:
             return gan_image_mask_window_rgb_f ( image, r0, c0, height, width );

           case GAN_DOUBLE:
             return gan_image_mask_window_rgb_d ( image, r0, c0, height, width );

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_mask_window", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_RGB_COLOUR_ALPHA_IMAGE:
        switch ( image->type )
        {
           case GAN_UCHAR:
             return gan_image_mask_window_rgba_uc ( image, r0, c0, height, width );

           case GAN_SHORT:
             return gan_image_mask_window_rgba_s ( image, r0, c0, height, width );

           case GAN_USHORT:
             return gan_image_mask_window_rgba_us ( image, r0, c0, height, width );

           case GAN_INT:
             return gan_image_mask_window_rgba_i ( image, r0, c0, height, width );

           case GAN_UINT:
             return gan_image_mask_window_rgba_ui ( image, r0, c0, height, width );

           case GAN_FLOAT:
             return gan_image_mask_window_rgba_f ( image, r0, c0, height, width );

           case GAN_DOUBLE:
             return gan_image_mask_window_rgba_d ( image, r0, c0, height, width );

           case GAN_UINT12:
             return gan_image_mask_window_rgba_ui12 ( image, r0, c0, height, width );

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_mask_window", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_BGR_COLOUR_IMAGE:
        switch ( image->type )
        {
           case GAN_UCHAR:
             return gan_image_mask_window_bgr_uc ( image, r0, c0, height, width );

           case GAN_SHORT:
             return gan_image_mask_window_bgr_s ( image, r0, c0, height, width );

           case GAN_USHORT:
             return gan_image_mask_window_bgr_us ( image, r0, c0, height, width );

           case GAN_INT:
             return gan_image_mask_window_bgr_i ( image, r0, c0, height, width );

           case GAN_UINT:
             return gan_image_mask_window_bgr_ui ( image, r0, c0, height, width );

           case GAN_FLOAT:
             return gan_image_mask_window_bgr_f ( image, r0, c0, height, width );

           case GAN_DOUBLE:
             return gan_image_mask_window_bgr_d ( image, r0, c0, height, width );

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_mask_window", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_BGR_COLOUR_ALPHA_IMAGE:
        switch ( image->type )
        {
           case GAN_UCHAR:
             return gan_image_mask_window_bgra_uc ( image, r0, c0, height, width );

           case GAN_SHORT:
             return gan_image_mask_window_bgra_s ( image, r0, c0, height, width );

           case GAN_USHORT:
             return gan_image_mask_window_bgra_us ( image, r0, c0, height, width );

           case GAN_INT:
             return gan_image_mask_window_bgra_i ( image, r0, c0, height, width );

           case GAN_UINT:
             return gan_image_mask_window_bgra_ui ( image, r0, c0, height, width );

           case GAN_FLOAT:
             return gan_image_mask_window_bgra_f ( image, r0, c0, height, width );

           case GAN_DOUBLE:
             return gan_image_mask_window_bgra_d ( image, r0, c0, height, width );

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_mask_window", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_VECTOR_FIELD_2D:
        switch ( image->type )
        {
           case GAN_SHORT:
             return gan_image_mask_window_vfield2D_s ( image, r0, c0, height, width );

           case GAN_INT:
             return gan_image_mask_window_vfield2D_i ( image, r0, c0, height, width );

           case GAN_FLOAT:
             return gan_image_mask_window_vfield2D_f ( image, r0, c0, height, width );

           case GAN_DOUBLE:
             return gan_image_mask_window_vfield2D_d ( image, r0, c0, height, width );

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_mask_window", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_VECTOR_FIELD_3D:
        switch ( image->type )
        {
           case GAN_SHORT:
             return gan_image_mask_window_vfield3D_s ( image, r0, c0, height, width );

           case GAN_INT:
             return gan_image_mask_window_vfield3D_i ( image, r0, c0, height, width );

           case GAN_FLOAT:
             return gan_image_mask_window_vfield3D_f ( image, r0, c0, height, width );

           case GAN_DOUBLE:
             return gan_image_mask_window_vfield3D_d ( image, r0, c0, height, width );

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_mask_window", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
      break;

      case GAN_RGBX:
        switch ( image->type )
        {
           case GAN_UINT8:
             return gan_image_mask_window_rgbx_ui8 ( image, r0, c0, height, width );

           case GAN_UINT10:
             return gan_image_mask_window_rgbx_ui10 ( image, r0, c0, height, width );

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_mask_window", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_RGBAS:
        switch ( image->type )
        {
           case GAN_UINT10:
             return gan_image_mask_window_rgbas_ui10 ( image, r0, c0, height, width );

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_mask_window", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_YUVX444:
        switch ( image->type )
        {
           case GAN_UINT8:
             return gan_image_mask_window_yuvx444_ui8 ( image, r0, c0, height, width );

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_mask_window", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_YUVA444:
        switch ( image->type )
        {
           case GAN_UINT8:
             return gan_image_mask_window_yuva444_ui8 ( image, r0, c0, height, width );

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_mask_window", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_YUV422:
        switch ( image->type )
        {
           case GAN_UINT8:
             return gan_image_mask_window_yuv422_ui8 ( image, r0, c0, height, width );

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_mask_window", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_image_mask_window", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
        break;
   }

   /* shouldn't get here */
   assert(0);
   return GAN_FALSE;
}

/**
 * \brief Clear image to zero in specified rectangular region.
 * \return #GAN_TRUE on success, or #GAN_FALSE on failure.
 *
 * Clear \a image to zero in specified rectangular region.
 */
Gan_Bool
 gan_image_clear_window ( Gan_Image *image,
                          unsigned r0,     unsigned c0,
                          unsigned height, unsigned width )
{
   switch ( image->format )
   {
      case GAN_GREY_LEVEL_IMAGE:
        switch ( image->type )
        {
           case GAN_UCHAR:
             return gan_image_clear_window_gl_uc ( image, r0, c0, height, width );
             break;

           case GAN_SHORT:
             return gan_image_clear_window_gl_s ( image, r0, c0, height, width );
             break;

           case GAN_USHORT:
             return gan_image_clear_window_gl_us ( image, r0, c0, height, width );
             break;

           case GAN_INT:
             return gan_image_clear_window_gl_i ( image, r0, c0, height, width );
             break;

           case GAN_UINT:
             return gan_image_clear_window_gl_ui ( image, r0, c0, height, width );
             break;

           case GAN_FLOAT:
             return gan_image_clear_window_gl_f ( image, r0, c0, height, width );
             break;

           case GAN_DOUBLE:
             return gan_image_clear_window_gl_d ( image, r0, c0, height, width );
             break;

           case GAN_BOOL:
             return gan_image_clear_window_b ( image, r0, c0, height, width );
             break;

           case GAN_POINTER:
             return gan_image_clear_window_p ( image, r0, c0, height, width );
             break;

           case GAN_UINT10:
             return gan_image_clear_window_gl_ui10 ( image, r0, c0, height, width );
             break;

           case GAN_UINT12:
             return gan_image_clear_window_gl_ui12 ( image, r0, c0, height, width );
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_clear_window", GAN_ERROR_ILLEGAL_IMAGE_TYPE,
                                "" );
             return GAN_FALSE;
             break;
      }
      break;

      case GAN_GREY_LEVEL_ALPHA_IMAGE:
        switch ( image->type )
        {
           case GAN_UCHAR:
             return gan_image_clear_window_gla_uc ( image, r0, c0, height, width );

           case GAN_SHORT:
             return gan_image_clear_window_gla_s ( image, r0, c0, height, width );

           case GAN_USHORT:
             return gan_image_clear_window_gla_us ( image, r0, c0, height, width );

           case GAN_INT:
             return gan_image_clear_window_gla_i ( image, r0, c0, height, width );

           case GAN_UINT:
             return gan_image_clear_window_gla_ui ( image, r0, c0, height, width );

           case GAN_FLOAT:
             return gan_image_clear_window_gla_f ( image, r0, c0, height, width );

           case GAN_DOUBLE:
             return gan_image_clear_window_gla_d ( image, r0, c0, height, width );

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_clear_window", GAN_ERROR_ILLEGAL_IMAGE_TYPE,
                                "" );
             break;
        }
        break;

      case GAN_RGB_COLOUR_IMAGE:
        switch ( image->type )
        {
           case GAN_UCHAR:
             return gan_image_clear_window_rgb_uc ( image, r0, c0, height, width );

           case GAN_SHORT:
             return gan_image_clear_window_rgb_s ( image, r0, c0, height, width );

           case GAN_USHORT:
             return gan_image_clear_window_rgb_us ( image, r0, c0, height, width );

           case GAN_INT:
             return gan_image_clear_window_rgb_i ( image, r0, c0, height, width );

           case GAN_UINT:
             return gan_image_clear_window_rgb_ui ( image, r0, c0, height, width );

           case GAN_FLOAT:
             return gan_image_clear_window_rgb_f ( image, r0, c0, height, width );

           case GAN_DOUBLE:
             return gan_image_clear_window_rgb_d ( image, r0, c0, height, width );

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_clear_window", GAN_ERROR_ILLEGAL_IMAGE_TYPE,
                                "" );
             break;
        }
        break;

      case GAN_RGB_COLOUR_ALPHA_IMAGE:
        switch ( image->type )
        {
           case GAN_UCHAR:
             return gan_image_clear_window_rgba_uc ( image, r0, c0, height, width );

           case GAN_SHORT:
             return gan_image_clear_window_rgba_s ( image, r0, c0, height, width );

           case GAN_USHORT:
             return gan_image_clear_window_rgba_us ( image, r0, c0, height, width );

           case GAN_INT:
             return gan_image_clear_window_rgba_i ( image, r0, c0, height, width );

           case GAN_UINT:
             return gan_image_clear_window_rgba_ui ( image, r0, c0, height, width );

           case GAN_FLOAT:
             return gan_image_clear_window_rgba_f ( image, r0, c0, height, width );

           case GAN_DOUBLE:
             return gan_image_clear_window_rgba_d ( image, r0, c0, height, width );

           case GAN_UINT12:
             return gan_image_clear_window_rgba_ui12 ( image, r0, c0, height, width );

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_clear_window", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_BGR_COLOUR_IMAGE:
        switch ( image->type )
        {
           case GAN_UCHAR:
             return gan_image_clear_window_bgr_uc ( image, r0, c0, height, width );

           case GAN_SHORT:
             return gan_image_clear_window_bgr_s ( image, r0, c0, height, width );

           case GAN_USHORT:
             return gan_image_clear_window_bgr_us ( image, r0, c0, height, width );

           case GAN_INT:
             return gan_image_clear_window_bgr_i ( image, r0, c0, height, width );

           case GAN_UINT:
             return gan_image_clear_window_bgr_ui ( image, r0, c0, height, width );

           case GAN_FLOAT:
             return gan_image_clear_window_bgr_f ( image, r0, c0, height, width );

           case GAN_DOUBLE:
             return gan_image_clear_window_bgr_d ( image, r0, c0, height, width );

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_clear_window", GAN_ERROR_ILLEGAL_IMAGE_TYPE,
                                "" );
             break;
        }
        break;

      case GAN_BGR_COLOUR_ALPHA_IMAGE:
        switch ( image->type )
        {
           case GAN_UCHAR:
             return gan_image_clear_window_bgra_uc ( image, r0, c0, height, width );

           case GAN_SHORT:
             return gan_image_clear_window_bgra_s ( image, r0, c0, height, width );

           case GAN_USHORT:
             return gan_image_clear_window_bgra_us ( image, r0, c0, height, width );

           case GAN_INT:
             return gan_image_clear_window_bgra_i ( image, r0, c0, height, width );

           case GAN_UINT:
             return gan_image_clear_window_bgra_ui ( image, r0, c0, height, width );

           case GAN_FLOAT:
             return gan_image_clear_window_bgra_f ( image, r0, c0, height, width );

           case GAN_DOUBLE:
             return gan_image_clear_window_bgra_d ( image, r0, c0, height, width );

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_clear_window", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_VECTOR_FIELD_2D:
        switch ( image->type )
        {
           case GAN_SHORT:
             return gan_image_clear_window_vfield2D_s ( image, r0, c0, height, width );

           case GAN_INT:
             return gan_image_clear_window_vfield2D_i ( image, r0, c0, height, width );

           case GAN_FLOAT:
             return gan_image_clear_window_vfield2D_f ( image, r0, c0, height, width );

           case GAN_DOUBLE:
             return gan_image_clear_window_vfield2D_d ( image, r0, c0, height, width );

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_clear_window", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_VECTOR_FIELD_3D:
        switch ( image->type )
        {
           case GAN_SHORT:
             return gan_image_clear_window_vfield3D_s ( image, r0, c0, height, width );

           case GAN_INT:
             return gan_image_clear_window_vfield3D_i ( image, r0, c0, height, width );

           case GAN_FLOAT:
             return gan_image_clear_window_vfield3D_f ( image, r0, c0, height, width );

           case GAN_DOUBLE:
             return gan_image_clear_window_vfield3D_d ( image, r0, c0, height, width );

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_clear_window", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;


      case GAN_RGBX:
        switch ( image->type )
        {
           case GAN_UINT8:
             return gan_image_clear_window_rgbx_ui8 ( image, r0, c0, height, width );

           case GAN_UINT10:
             return gan_image_clear_window_rgbx_ui10 ( image, r0, c0, height, width );

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_clear_window", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_RGBAS:
        switch ( image->type )
        {
           case GAN_UINT10:
             return gan_image_clear_window_rgbas_ui10 ( image, r0, c0, height, width );

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_clear_window", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_YUVX444:
        switch ( image->type )
        {
           case GAN_UINT8:
             return gan_image_clear_window_yuvx444_ui8 ( image, r0, c0, height, width );

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_clear_window", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_YUVA444:
        switch ( image->type )
        {
           case GAN_UINT8:
             return gan_image_clear_window_yuva444_ui8 ( image, r0, c0, height, width );

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_clear_window", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      case GAN_YUV422:
        switch ( image->type )
        {
           case GAN_UINT8:
             return gan_image_clear_window_yuv422_ui8 ( image, r0, c0, height, width );

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_clear_window", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             break;
        }
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_image_clear_window", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
        return GAN_FALSE;
        break;
   }

   return GAN_TRUE;
}

/**
 * \}
 */

/**
 * \addtogroup ImageMinMax
 * \{
 */

/**
 * \brief Calculates the minimum pixel value in an image
 *
 * Calculates the minimum pixel value in \a pImage over the mask \a pMask,
 * which can be passed as \c NULL to consider all the pixels. The result is
 * returned in \a minpix.
 *
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 */
Gan_Bool gan_image_get_minimum_pixel ( Gan_Image *image, Gan_Image *mask, Gan_Pixel *minpix )
{
   Gan_Bool result;

   minpix->format = GAN_GREY_LEVEL_IMAGE;
   minpix->type = image->type;
   switch ( image->format )
   {
      case GAN_GREY_LEVEL_IMAGE:
        switch ( image->type )
        {
           case GAN_UCHAR:
             result = gan_image_get_minimum_pixel_gl_uc ( image, mask, &minpix->data.gl.uc );
             break;

           case GAN_SHORT:
             result = gan_image_get_minimum_pixel_gl_s ( image, mask, &minpix->data.gl.s );
             break;

           case GAN_USHORT:
             result = gan_image_get_minimum_pixel_gl_us ( image, mask, &minpix->data.gl.us );
             break;

           case GAN_INT:
             result = gan_image_get_minimum_pixel_gl_i ( image, mask, &minpix->data.gl.i );
             break;

           case GAN_UINT:
             result = gan_image_get_minimum_pixel_gl_ui ( image, mask, &minpix->data.gl.ui );
             break;

           case GAN_FLOAT:
             result = gan_image_get_minimum_pixel_gl_f ( image, mask, &minpix->data.gl.f );
             break;

           case GAN_DOUBLE:
             result = gan_image_get_minimum_pixel_gl_d ( image, mask, &minpix->data.gl.d );
             break;

           case GAN_UINT10:
           {
              gan_uint16 tmpmin;

              result = gan_image_get_minimum_pixel_gl_ui10 ( image, mask, &tmpmin );
              if(result)
                 minpix->data.gl.ui10.Y = tmpmin;
           }
           break;

           case GAN_UINT12:
           {
              gan_uint16 tmpmin;

              result = gan_image_get_minimum_pixel_gl_ui12 ( image, mask, &tmpmin );
              if(result)
                 minpix->data.gl.ui12.Y = tmpmin;
           }
           break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_get_minimum_pixel", GAN_ERROR_ILLEGAL_IMAGE_TYPE,
                                "" );
             return GAN_FALSE;
             break;
      }
      break;

      case GAN_RGB_COLOUR_IMAGE:
        switch ( image->type )
        {
           case GAN_UCHAR:
             result = gan_image_get_minimum_pixel_rgb_uc ( image, mask, &minpix->data.gl.uc );
             break;

           case GAN_SHORT:
             result = gan_image_get_minimum_pixel_rgb_s ( image, mask, &minpix->data.gl.s );
             break;

           case GAN_USHORT:
             result = gan_image_get_minimum_pixel_rgb_us ( image, mask, &minpix->data.gl.us );
             break;

           case GAN_INT:
             result = gan_image_get_minimum_pixel_rgb_i ( image, mask, &minpix->data.gl.i );
             break;

           case GAN_UINT:
             result = gan_image_get_minimum_pixel_rgb_ui ( image, mask, &minpix->data.gl.ui );
             break;

           case GAN_FLOAT:
             result = gan_image_get_minimum_pixel_rgb_f ( image, mask, &minpix->data.gl.f );
             break;

           case GAN_DOUBLE:
             result = gan_image_get_minimum_pixel_rgb_d ( image, mask, &minpix->data.gl.d );
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_get_minimum_pixel", GAN_ERROR_ILLEGAL_IMAGE_TYPE,
                                "" );
             return GAN_FALSE;
             break;
      }
      break;

      case GAN_BGR_COLOUR_IMAGE:
        switch ( image->type )
        {
           case GAN_UCHAR:
             result = gan_image_get_minimum_pixel_bgr_uc ( image, mask, &minpix->data.gl.uc );
             break;

           case GAN_SHORT:
             result = gan_image_get_minimum_pixel_bgr_s ( image, mask, &minpix->data.gl.s );
             break;

           case GAN_USHORT:
             result = gan_image_get_minimum_pixel_bgr_us ( image, mask, &minpix->data.gl.us );
             break;

           case GAN_INT:
             result = gan_image_get_minimum_pixel_bgr_i ( image, mask, &minpix->data.gl.i );
             break;

           case GAN_UINT:
             result = gan_image_get_minimum_pixel_bgr_ui ( image, mask, &minpix->data.gl.ui );
             break;

           case GAN_FLOAT:
             result = gan_image_get_minimum_pixel_bgr_f ( image, mask, &minpix->data.gl.f );
             break;

           case GAN_DOUBLE:
             result = gan_image_get_minimum_pixel_bgr_d ( image, mask, &minpix->data.gl.d );
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_get_minimum_pixel", GAN_ERROR_ILLEGAL_IMAGE_TYPE,
                                "" );
             return GAN_FALSE;
             break;
      }
      break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_image_get_minimum_pixel", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
        return GAN_FALSE;
        break;
   }

   if(!result)
   {
      gan_err_register ( "gan_image_get_minimum_pixel", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   return GAN_TRUE;
}

/**
 * \brief Calculates the maximum pixel value in an image
 *
 * Calculates the maximum pixel value in \a pImage over the mask \a pMask,
 * which can be passed as \c NULL to consider all the pixels. The result is
 * returned in \a maxpix.
 *
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 */
Gan_Bool gan_image_get_maximum_pixel ( Gan_Image *image, Gan_Image *mask, Gan_Pixel *maxpix )
{
   Gan_Bool result;

   maxpix->format = GAN_GREY_LEVEL_IMAGE;
   maxpix->type = image->type;
   switch ( image->format )
   {
      case GAN_GREY_LEVEL_IMAGE:
        switch ( image->type )
        {
           case GAN_UCHAR:
             result = gan_image_get_maximum_pixel_gl_uc ( image, mask, &maxpix->data.gl.uc );
             break;

           case GAN_SHORT:
             result = gan_image_get_maximum_pixel_gl_s ( image, mask, &maxpix->data.gl.s );
             break;

           case GAN_USHORT:
             result = gan_image_get_maximum_pixel_gl_us ( image, mask, &maxpix->data.gl.us );
             break;

           case GAN_INT:
             result = gan_image_get_maximum_pixel_gl_i ( image, mask, &maxpix->data.gl.i );
             break;

           case GAN_UINT:
             result = gan_image_get_maximum_pixel_gl_ui ( image, mask, &maxpix->data.gl.ui );
             break;

           case GAN_FLOAT:
             result = gan_image_get_maximum_pixel_gl_f ( image, mask, &maxpix->data.gl.f );
             break;

           case GAN_DOUBLE:
             result = gan_image_get_maximum_pixel_gl_d ( image, mask, &maxpix->data.gl.d );
             break;

           case GAN_UINT10:
           {
              gan_uint16 tmpmax;

              result = gan_image_get_maximum_pixel_gl_ui10 ( image, mask, &tmpmax );
              if(result)
                 maxpix->data.gl.ui10.Y = tmpmax;
           }
           break;

           case GAN_UINT12:
           {
              gan_uint16 tmpmax;

              result = gan_image_get_maximum_pixel_gl_ui12 ( image, mask, &tmpmax );
              if(result)
                 maxpix->data.gl.ui12.Y = tmpmax;
           }
           break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_get_maximum_pixel", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             return GAN_FALSE;
             break;
      }
      break;

      case GAN_RGB_COLOUR_IMAGE:
        switch ( image->type )
        {
           case GAN_UCHAR:
             result = gan_image_get_maximum_pixel_rgb_uc ( image, mask, &maxpix->data.gl.uc );
             break;

           case GAN_SHORT:
             result = gan_image_get_maximum_pixel_rgb_s ( image, mask, &maxpix->data.gl.s );
             break;

           case GAN_USHORT:
             result = gan_image_get_maximum_pixel_rgb_us ( image, mask, &maxpix->data.gl.us );
             break;

           case GAN_INT:
             result = gan_image_get_maximum_pixel_rgb_i ( image, mask, &maxpix->data.gl.i );
             break;

           case GAN_UINT:
             result = gan_image_get_maximum_pixel_rgb_ui ( image, mask, &maxpix->data.gl.ui );
             break;

           case GAN_FLOAT:
             result = gan_image_get_maximum_pixel_rgb_f ( image, mask, &maxpix->data.gl.f );
             break;

           case GAN_DOUBLE:
             result = gan_image_get_maximum_pixel_rgb_d ( image, mask, &maxpix->data.gl.d );
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_get_maximum_pixel", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             return GAN_FALSE;
             break;
      }
      break;

      case GAN_BGR_COLOUR_IMAGE:
        switch ( image->type )
        {
           case GAN_UCHAR:
             result = gan_image_get_maximum_pixel_bgr_uc ( image, mask, &maxpix->data.gl.uc );
             break;

           case GAN_SHORT:
             result = gan_image_get_maximum_pixel_bgr_s ( image, mask, &maxpix->data.gl.s );
             break;

           case GAN_USHORT:
             result = gan_image_get_maximum_pixel_bgr_us ( image, mask, &maxpix->data.gl.us );
             break;

           case GAN_INT:
             result = gan_image_get_maximum_pixel_bgr_i ( image, mask, &maxpix->data.gl.i );
             break;

           case GAN_UINT:
             result = gan_image_get_maximum_pixel_bgr_ui ( image, mask, &maxpix->data.gl.ui );
             break;

           case GAN_FLOAT:
             result = gan_image_get_maximum_pixel_bgr_f ( image, mask, &maxpix->data.gl.f );
             break;

           case GAN_DOUBLE:
             result = gan_image_get_maximum_pixel_bgr_d ( image, mask, &maxpix->data.gl.d );
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_get_maximum_pixel", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             return GAN_FALSE;
             break;
      }
      break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_image_get_maximum_pixel", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
        return GAN_FALSE;
        break;
   }

   if(!result)
   {
      gan_err_register ( "gan_image_get_maximum_pixel", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   return GAN_TRUE;
}

/**
 * \}
 */

/**
 * \addtogroup ImageAllocate
 * \{
 */

/**
 * \brief Free a \c NULL terminated variable argument list of images.
 * \return No value.
 *
 * Free a \c NULL terminated variable argument list of images, starting
 * with image \a img.
 */
void
 gan_image_free_va ( Gan_Image *img, ... )
{
   va_list ap;

   va_start ( ap, img );
   while ( img != NULL )
   {
      /* free next image */
      gan_image_free ( img );

      /* get next image in list */
      img = va_arg ( ap, Gan_Image * );
   }

   va_end ( ap );
}

/**
 * \}
 */

/**
 * \addtogroup ImageChannel
 * \{
 */

/**
 * \brief Returns value indicating whether an image has an alpha channel
 * \param img Input image
 * \return #GAN_TRUE if the image has an alpha channel, #GAN_FALSE if it doesn't
 */
Gan_Bool gan_image_has_alpha_channel ( const Gan_Image *img )
{
   switch ( img->format )
   {
      case GAN_GREY_LEVEL_IMAGE:       return GAN_FALSE; break;
      case GAN_GREY_LEVEL_ALPHA_IMAGE: return GAN_TRUE; break;
      case GAN_RGB_COLOUR_IMAGE:       return GAN_FALSE; break;
      case GAN_RGB_COLOUR_ALPHA_IMAGE: return GAN_TRUE; break;
      case GAN_BGR_COLOUR_IMAGE:       return GAN_FALSE; break;
      case GAN_BGR_COLOUR_ALPHA_IMAGE: return GAN_TRUE; break;
      case GAN_VECTOR_FIELD_2D:        return GAN_FALSE; break;
      case GAN_VECTOR_FIELD_3D:        return GAN_FALSE; break;
      case GAN_RGBX:                   return GAN_FALSE; break;
      case GAN_RGBAS:                  return GAN_TRUE; break;
      case GAN_YUVX444:                return GAN_FALSE; break;
      case GAN_YUVA444:                return GAN_TRUE; break;
      case GAN_YUV422:                 return GAN_FALSE; break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_image_clear_window", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
        return GAN_FALSE;
        break;
   }
}


/**
 * \}
 */

/**
 * \}
 */

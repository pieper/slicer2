/**
 * File:          $RCSfile: image_channel.c,v $
 * Module:        Extract/fill selected channel(s) of image
 * Part of:       Gandalf Library
 *
 * Revision:      $Revision: 1.1.2.1 $
 * Last edited:   $Date: 2007/10/14 02:20:17 $
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

#include <gandalf/image/image_channel.h>
#include <gandalf/image/image_extract.h>
#include <gandalf/image/image_gl_uchar.h>
#include <gandalf/image/image_gl_short.h>
#include <gandalf/image/image_gl_ushort.h>
#include <gandalf/image/image_gl_int.h>
#include <gandalf/image/image_gl_uint.h>
#include <gandalf/image/image_gl_float.h>
#include <gandalf/image/image_gl_double.h>
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
#include <gandalf/image/image_vfield2D_short.h>
#include <gandalf/image/image_vfield2D_int.h>
#include <gandalf/image/image_vfield2D_float.h>
#include <gandalf/image/image_vfield2D_double.h>
#include <gandalf/image/image_vfield3D_short.h>
#include <gandalf/image/image_vfield3D_int.h>
#include <gandalf/image/image_vfield3D_float.h>
#include <gandalf/image/image_vfield3D_double.h>
#include <gandalf/image/image_bit.h>

/**
 * \addtogroup ImagePackage
 * \{
 */

/**
 * \defgroup ImageChannel Access Whole Channels of an Image
 * \{
 */

/* extract the intensity channel from a grey-level image with alpha channel */
static Gan_Image *
 gla_extract_intensity ( const Gan_Image *source,
                         unsigned r0,     unsigned c0,
                         unsigned height, unsigned width,
                         Gan_Image *dest )
{
   int i, j;

   /* consistency check */
   gan_err_test_ptr ( source->format == GAN_GREY_LEVEL_ALPHA_IMAGE,
                      "gla_extract_intensity", GAN_ERROR_INCOMPATIBLE, "" );

   switch ( source->type )
   {
      case GAN_UCHAR:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_uc ( dest, i, j,
                                        gan_image_get_pixptr_gla_uc(source,
                                                                    r0+i,
                                                                    c0+j)->I );
        break;

      case GAN_SHORT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_s ( dest, i, j,
                                       gan_image_get_pixptr_gla_s(source,
                                                                  r0+i,
                                                                  c0+j)->I );
        break;

      case GAN_USHORT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_us ( dest, i, j,
                                        gan_image_get_pixptr_gla_us(source,
                                                                    r0+i,
                                                                    c0+j)->I );
        break;

      case GAN_INT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_i ( dest, i, j,
                                       gan_image_get_pixptr_gla_i(source,
                                                                  r0+i,
                                                                  c0+j)->I );
        break;

      case GAN_UINT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_ui ( dest, i, j,
                                        gan_image_get_pixptr_gla_ui(source,
                                                                    r0+i,
                                                                    c0+j)->I );
        break;

      case GAN_FLOAT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_f ( dest, i, j,
                                       gan_image_get_pixptr_gla_f(source,
                                                                  r0+i,
                                                                  c0+j)->I );
        break;

      default:
        gan_err_flush_trace();
        gan_err_register (  "gla_extract_intensity",
                            GAN_ERROR_NOT_IMPLEMENTED, "image conversion" );
        return NULL;
   }

   /* success */
   return dest;
}

/* extract the alpha channel from a grey-level image with alpha channel */
static Gan_Image *
 gla_extract_alpha ( const Gan_Image *source,
                     unsigned r0,     unsigned c0,
                     unsigned height, unsigned width,
                     Gan_Image *dest )
{
   int i, j;

   /* consistency check */
   gan_err_test_ptr ( source->format == GAN_GREY_LEVEL_ALPHA_IMAGE,
                      "gla_extract_alpha", GAN_ERROR_INCOMPATIBLE, "" );

   switch ( source->type )
   {
      case GAN_UCHAR:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_uc ( dest, i, j,
                                        gan_image_get_pixptr_gla_uc(source,
                                                                    r0+i,
                                                                    c0+j)->A );

        break;

      case GAN_SHORT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_s ( dest, i, j,
                                       gan_image_get_pixptr_gla_s(source,
                                                                  r0+i,
                                                                  c0+j)->A );
        break;

      case GAN_USHORT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_us ( dest, i, j,
                                        gan_image_get_pixptr_gla_us(source,
                                                                    r0+i,
                                                                    c0+j)->A );
        break;

      case GAN_INT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_i ( dest, i, j,
                                       gan_image_get_pixptr_gla_i(source,
                                                                  r0+i,
                                                                  c0+j)->A );
        break;

      case GAN_UINT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_ui ( dest, i, j,
                                        gan_image_get_pixptr_gla_ui(source,
                                                                    r0+i,
                                                                    c0+j)->A );
        break;

      case GAN_FLOAT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_f ( dest, i, j,
                                       gan_image_get_pixptr_gla_f(source,
                                                                  r0+i,
                                                                  c0+j)->A );
        break;

      default:
        gan_err_flush_trace();
        gan_err_register (  "gla_extract_alpha",
                            GAN_ERROR_NOT_IMPLEMENTED, "image conversion" );
        return NULL;
   }

   /* success */
   return dest;
}

/* extract the red channel from an RGB colour image */
static Gan_Image *
 rgb_extract_red ( const Gan_Image *source,
                   unsigned r0,     unsigned c0,
                   unsigned height, unsigned width,
                   Gan_Image *dest )
{
   int i, j;

   /* consistency check */
   gan_err_test_ptr ( source->format == GAN_RGB_COLOUR_IMAGE,
                      "rgb_extract_red", GAN_ERROR_INCOMPATIBLE, "" );

   switch ( source->type )
   {
      case GAN_UCHAR:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_uc ( dest, i, j,
                                        gan_image_get_pixptr_rgb_uc(source,
                                                                    r0+i,
                                                                    c0+j)->R );
        break;

      case GAN_SHORT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_s ( dest, i, j,
                                       gan_image_get_pixptr_rgb_s(source,
                                                                  r0+i,
                                                                  c0+j)->R );
        break;

      case GAN_USHORT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_us ( dest, i, j,
                                        gan_image_get_pixptr_rgb_us(source,
                                                                    r0+i,
                                                                    c0+j)->R );
        break;

      case GAN_INT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_i ( dest, i, j,
                                       gan_image_get_pixptr_rgb_i(source,
                                                                  r0+i,
                                                                  c0+j)->R );
        break;

      case GAN_UINT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_ui ( dest, i, j,
                                        gan_image_get_pixptr_rgb_ui(source,
                                                                    r0+i,
                                                                    c0+j)->R );
        break;

      case GAN_FLOAT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_f ( dest, i, j,
                                       gan_image_get_pixptr_rgb_f(source,
                                                                  r0+i,
                                                                  c0+j)->R );
        break;

      default:
        gan_err_flush_trace();
        gan_err_register (  "rgb_extract_red",
                            GAN_ERROR_NOT_IMPLEMENTED, "image conversion" );
        return NULL;
   }

   /* success */
   return dest;
}

/* extract the green channel from an RGB colour image */
static Gan_Image *
 rgb_extract_green ( const Gan_Image *source,
                     unsigned r0,     unsigned c0,
                     unsigned height, unsigned width,
                     Gan_Image *dest )
{
   int i, j;

   /* consistency check */
   gan_err_test_ptr ( source->format == GAN_RGB_COLOUR_IMAGE,
                      "rgb_extract_green", GAN_ERROR_INCOMPATIBLE, "" );

   switch ( source->type )
   {
      case GAN_UCHAR:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_uc ( dest, i, j,
                                        gan_image_get_pixptr_rgb_uc(source,
                                                                    r0+i,
                                                                    c0+j)->G );
        break;

      case GAN_SHORT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_s ( dest, i, j,
                                       gan_image_get_pixptr_rgb_s(source,
                                                                  r0+i,
                                                                  c0+j)->G );
        break;

      case GAN_USHORT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_us ( dest, i, j,
                                        gan_image_get_pixptr_rgb_us(source,
                                                                    r0+i,
                                                                    c0+j)->G );
        break;

      case GAN_INT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_i ( dest, i, j,
                                       gan_image_get_pixptr_rgb_i(source,
                                                                  r0+i,
                                                                  c0+j)->G );
        break;

      case GAN_UINT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_ui ( dest, i, j,
                                        gan_image_get_pixptr_rgb_ui(source,
                                                                    r0+i,
                                                                    c0+j)->G );
        break;

      case GAN_FLOAT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_f ( dest, i, j,
                                       gan_image_get_pixptr_rgb_f(source,
                                                                  r0+i,
                                                                  c0+j)->G );
        break;

      default:
        gan_err_flush_trace();
        gan_err_register (  "rgb_extract_green",
                            GAN_ERROR_NOT_IMPLEMENTED, "image conversion" );
        return NULL;
   }

   /* success */
   return dest;
}

/* extract the blue channel from an RGB colour image */
static Gan_Image *
 rgb_extract_blue ( const Gan_Image *source,
                    unsigned r0,     unsigned c0,
                    unsigned height, unsigned width,
                    Gan_Image *dest )
{
   int i, j;

   /* consistency check */
   gan_err_test_ptr ( source->format == GAN_RGB_COLOUR_IMAGE,
                      "rgb_extract_blue", GAN_ERROR_INCOMPATIBLE, "" );

   switch ( source->type )
   {
      case GAN_UCHAR:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_uc ( dest, i, j,
                                        gan_image_get_pixptr_rgb_uc(source,
                                                                    r0+i,
                                                                    c0+j)->B );
        break;

      case GAN_SHORT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_s ( dest, i, j,
                                       gan_image_get_pixptr_rgb_s(source,
                                                                  r0+i,
                                                                  c0+j)->B );
        break;

      case GAN_USHORT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_us ( dest, i, j,
                                        gan_image_get_pixptr_rgb_us(source,
                                                                    r0+i,
                                                                    c0+j)->B );
        break;

      case GAN_INT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_i ( dest, i, j,
                                       gan_image_get_pixptr_rgb_i(source,
                                                                  r0+i,
                                                                  c0+j)->B );
        break;

      case GAN_UINT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_ui ( dest, i, j,
                                        gan_image_get_pixptr_rgb_ui(source,
                                                                    r0+i,
                                                                    c0+j)->B );
        break;

      case GAN_FLOAT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_f ( dest, i, j,
                                       gan_image_get_pixptr_rgb_f(source,
                                                                  r0+i,
                                                                  c0+j)->B );
        break;

      default:
        gan_err_flush_trace();
        gan_err_register (  "rgb_extract_blue",
                            GAN_ERROR_NOT_IMPLEMENTED, "image conversion" );
        return NULL;
   }

   /* success */
   return dest;
}

/* extract the red channel from an RGB colour image with alpha channel */
static Gan_Image *
 rgba_extract_red ( const Gan_Image *source,
                    unsigned r0,     unsigned c0,
                    unsigned height, unsigned width,
                    Gan_Image *dest )
{
   int i, j;

   /* consistency check */
   gan_err_test_ptr ( source->format == GAN_RGB_COLOUR_ALPHA_IMAGE,
                      "rgba_extract_red", GAN_ERROR_INCOMPATIBLE, "" );

   switch ( source->type )
   {
      case GAN_UCHAR:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_uc ( dest, i, j,
                                        gan_image_get_pixptr_rgba_uc(source,
                                                                     r0+i,
                                                                     c0+j)->R);
        break;

      case GAN_SHORT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_s ( dest, i, j,
                                       gan_image_get_pixptr_rgba_s(source,
                                                                   r0+i,
                                                                   c0+j)->R);
        break;

      case GAN_USHORT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_us ( dest, i, j,
                                        gan_image_get_pixptr_rgba_us(source,
                                                                     r0+i,
                                                                     c0+j)->R);
        break;

      case GAN_INT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_i ( dest, i, j,
                                       gan_image_get_pixptr_rgba_i(source,
                                                                   r0+i,
                                                                   c0+j)->R);
        break;

      case GAN_UINT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_ui ( dest, i, j,
                                        gan_image_get_pixptr_rgba_ui(source,
                                                                     r0+i,
                                                                     c0+j)->R);
        break;

      case GAN_FLOAT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_f ( dest, i, j,
                                       gan_image_get_pixptr_rgba_f(source,
                                                                   r0+i,
                                                                   c0+j)->R);
        break;

      default:
        gan_err_flush_trace();
        gan_err_register (  "rgba_extract_red",
                            GAN_ERROR_NOT_IMPLEMENTED, "image conversion" );
        return NULL;
   }

   /* success */
   return dest;
}

/* extract the green channel from an RGB colour image with alpha channel */
static Gan_Image *
 rgba_extract_green ( const Gan_Image *source,
                      unsigned r0,     unsigned c0,
                      unsigned height, unsigned width,
                      Gan_Image *dest )
{
   int i, j;

   /* consistency check */
   gan_err_test_ptr ( source->format == GAN_RGB_COLOUR_ALPHA_IMAGE,
                      "rgba_extract_green", GAN_ERROR_INCOMPATIBLE, "" );

   switch ( source->type )
   {
      case GAN_UCHAR:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_uc ( dest, i, j,
                                        gan_image_get_pixptr_rgba_uc(source,
                                                                     r0+i,
                                                                     c0+j)->G);
        break;

      case GAN_SHORT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_s ( dest, i, j,
                                       gan_image_get_pixptr_rgba_s(source,
                                                                   r0+i,
                                                                   c0+j)->G);
        break;

      case GAN_USHORT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_us ( dest, i, j,
                                        gan_image_get_pixptr_rgba_us(source,
                                                                     r0+i,
                                                                     c0+j)->G);
        break;

      case GAN_INT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_i ( dest, i, j,
                                       gan_image_get_pixptr_rgba_i(source,
                                                                   r0+i,
                                                                   c0+j)->G);
        break;

      case GAN_UINT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_ui ( dest, i, j,
                                        gan_image_get_pixptr_rgba_ui(source,
                                                                     r0+i,
                                                                     c0+j)->G);
        break;

      case GAN_FLOAT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_f ( dest, i, j,
                                       gan_image_get_pixptr_rgba_f(source,
                                                                   r0+i,
                                                                   c0+j)->G);
        break;

      default:
        gan_err_flush_trace();
        gan_err_register (  "rgba_extract_green",
                            GAN_ERROR_NOT_IMPLEMENTED, "image conversion" );
        return NULL;
   }

   /* success */
   return dest;
}

/* extract the blue channel from an RGB colour image with alpha channel */
static Gan_Image *
 rgba_extract_blue ( const Gan_Image *source,
                     unsigned r0,     unsigned c0,
                     unsigned height, unsigned width,
                     Gan_Image *dest )
{
   int i, j;

   /* consistency check */
   gan_err_test_ptr ( source->format == GAN_RGB_COLOUR_ALPHA_IMAGE,
                      "rgba_extract_blue", GAN_ERROR_INCOMPATIBLE, "" );

   switch ( source->type )
   {
      case GAN_UCHAR:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_uc ( dest, i, j,
                                        gan_image_get_pixptr_rgba_uc(source,
                                                                     r0+i,
                                                                     c0+j)->B);
        break;

      case GAN_SHORT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_s ( dest, i, j,
                                       gan_image_get_pixptr_rgba_s(source,
                                                                   r0+i,
                                                                   c0+j)->B);
        break;

      case GAN_USHORT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_us ( dest, i, j,
                                        gan_image_get_pixptr_rgba_us(source,
                                                                     r0+i,
                                                                     c0+j)->B);
        break;

      case GAN_INT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_i ( dest, i, j,
                                       gan_image_get_pixptr_rgba_i(source,
                                                                   r0+i,
                                                                   c0+j)->B);
        break;

      case GAN_UINT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_ui ( dest, i, j,
                                        gan_image_get_pixptr_rgba_ui(source,
                                                                     r0+i,
                                                                     c0+j)->B);
        break;

      case GAN_FLOAT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_f ( dest, i, j,
                                       gan_image_get_pixptr_rgba_f(source,
                                                                   r0+i,
                                                                   c0+j)->B);
        break;

      default:
        gan_err_flush_trace();
        gan_err_register (  "rgba_extract_blue",
                            GAN_ERROR_NOT_IMPLEMENTED, "image conversion" );
        return NULL;
   }

   /* success */
   return dest;
}

/* extract the alpha channel from an RGB colour image with alpha channel */
static Gan_Image *
 rgba_extract_alpha ( const Gan_Image *source,
                      unsigned r0,     unsigned c0,
                      unsigned height, unsigned width,
                      Gan_Image *dest )
{
   int i, j;

   /* consistency check */
   gan_err_test_ptr ( source->format == GAN_RGB_COLOUR_ALPHA_IMAGE,
                      "rgba_extract_alpha", GAN_ERROR_INCOMPATIBLE, "" );

   switch ( source->type )
   {
      case GAN_UCHAR:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_uc ( dest, i, j,
                                        gan_image_get_pixptr_rgba_uc(source,
                                                                     r0+i,
                                                                     c0+j)->A);
        break;

      case GAN_SHORT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_s ( dest, i, j,
                                       gan_image_get_pixptr_rgba_s(source,
                                                                   r0+i,
                                                                   c0+j)->A);
        break;

      case GAN_USHORT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_us ( dest, i, j,
                                        gan_image_get_pixptr_rgba_us(source,
                                                                     r0+i,
                                                                     c0+j)->A);
        break;

      case GAN_INT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_i ( dest, i, j,
                                       gan_image_get_pixptr_rgba_i(source,
                                                                   r0+i,
                                                                   c0+j)->A);
        break;

      case GAN_UINT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_ui ( dest, i, j,
                                        gan_image_get_pixptr_rgba_ui(source,
                                                                     r0+i,
                                                                     c0+j)->A);
        break;

      case GAN_FLOAT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_f ( dest, i, j,
                                       gan_image_get_pixptr_rgba_f(source,
                                                                   r0+i,
                                                                   c0+j)->A);
        break;

      default:
        gan_err_flush_trace();
        gan_err_register (  "rgba_extract_alpha",
                            GAN_ERROR_NOT_IMPLEMENTED, "image conversion" );
        return NULL;
   }

   /* success */
   return dest;
}

/* extract the red channel from an BGR colour image */
static Gan_Image *
 bgr_extract_red ( const Gan_Image *source,
                   unsigned r0,     unsigned c0,
                   unsigned height, unsigned width,
                   Gan_Image *dest )
{
   int i, j;

   /* consistency check */
   gan_err_test_ptr ( source->format == GAN_BGR_COLOUR_IMAGE,
                      "bgr_extract_red", GAN_ERROR_INCOMPATIBLE, "" );

   switch ( source->type )
   {
      case GAN_UCHAR:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_uc ( dest, i, j,
                                        gan_image_get_pixptr_bgr_uc(source,
                                                                    r0+i,
                                                                    c0+j)->R );
        break;

      case GAN_SHORT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_s ( dest, i, j,
                                       gan_image_get_pixptr_bgr_s(source,
                                                                  r0+i,
                                                                  c0+j)->R );
        break;

      case GAN_USHORT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_us ( dest, i, j,
                                        gan_image_get_pixptr_bgr_us(source,
                                                                    r0+i,
                                                                    c0+j)->R );
        break;

      case GAN_INT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_i ( dest, i, j,
                                       gan_image_get_pixptr_bgr_i(source,
                                                                  r0+i,
                                                                  c0+j)->R );
        break;

      case GAN_UINT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_ui ( dest, i, j,
                                        gan_image_get_pixptr_bgr_ui(source,
                                                                    r0+i,
                                                                    c0+j)->R );
        break;

      case GAN_FLOAT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_f ( dest, i, j,
                                       gan_image_get_pixptr_bgr_f(source,
                                                                  r0+i,
                                                                  c0+j)->R );
        break;

      default:
        gan_err_flush_trace();
        gan_err_register (  "bgr_extract_red",
                            GAN_ERROR_NOT_IMPLEMENTED, "image conversion" );
        return NULL;
   }

   /* success */
   return dest;
}

/* extract the green channel from an BGR colour image */
static Gan_Image *
 bgr_extract_green ( const Gan_Image *source,
                     unsigned r0,     unsigned c0,
                     unsigned height, unsigned width,
                     Gan_Image *dest )
{
   int i, j;

   /* consistency check */
   gan_err_test_ptr ( source->format == GAN_BGR_COLOUR_IMAGE,
                      "bgr_extract_green", GAN_ERROR_INCOMPATIBLE, "" );

   switch ( source->type )
   {
      case GAN_UCHAR:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_uc ( dest, i, j,
                                        gan_image_get_pixptr_bgr_uc(source,
                                                                    r0+i,
                                                                    c0+j)->G );
        break;

      case GAN_SHORT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_s ( dest, i, j,
                                       gan_image_get_pixptr_bgr_s(source,
                                                                  r0+i,
                                                                  c0+j)->G );
        break;

      case GAN_USHORT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_us ( dest, i, j,
                                        gan_image_get_pixptr_bgr_us(source,
                                                                    r0+i,
                                                                    c0+j)->G );
        break;

      case GAN_INT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_i ( dest, i, j,
                                       gan_image_get_pixptr_bgr_i(source,
                                                                  r0+i,
                                                                  c0+j)->G );
        break;

      case GAN_UINT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_ui ( dest, i, j,
                                        gan_image_get_pixptr_bgr_ui(source,
                                                                    r0+i,
                                                                    c0+j)->G );
        break;

      case GAN_FLOAT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_f ( dest, i, j,
                                       gan_image_get_pixptr_bgr_f(source,
                                                                  r0+i,
                                                                  c0+j)->G );
        break;

      default:
        gan_err_flush_trace();
        gan_err_register (  "bgr_extract_green",
                            GAN_ERROR_NOT_IMPLEMENTED, "image conversion" );
        return NULL;
   }

   /* success */
   return dest;
}

/* extract the blue channel from an BGR colour image */
static Gan_Image *
 bgr_extract_blue ( const Gan_Image *source,
                    unsigned r0,     unsigned c0,
                    unsigned height, unsigned width,
                    Gan_Image *dest )
{
   int i, j;

   /* consistency check */
   gan_err_test_ptr ( source->format == GAN_BGR_COLOUR_IMAGE,
                      "bgr_extract_blue", GAN_ERROR_INCOMPATIBLE, "" );

   switch ( source->type )
   {
      case GAN_UCHAR:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_uc ( dest, i, j,
                                        gan_image_get_pixptr_bgr_uc(source,
                                                                    r0+i,
                                                                    c0+j)->B );
        break;

      case GAN_SHORT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_s ( dest, i, j,
                                       gan_image_get_pixptr_bgr_s(source,
                                                                  r0+i,
                                                                  c0+j)->B );
        break;

      case GAN_USHORT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_us ( dest, i, j,
                                        gan_image_get_pixptr_bgr_us(source,
                                                                    r0+i,
                                                                    c0+j)->B );
        break;

      case GAN_INT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_i ( dest, i, j,
                                       gan_image_get_pixptr_bgr_i(source,
                                                                  r0+i,
                                                                  c0+j)->B );
        break;

      case GAN_UINT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_ui ( dest, i, j,
                                        gan_image_get_pixptr_bgr_ui(source,
                                                                    r0+i,
                                                                    c0+j)->B );
        break;

      case GAN_FLOAT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_f ( dest, i, j,
                                       gan_image_get_pixptr_bgr_f(source,
                                                                  r0+i,
                                                                  c0+j)->B );
        break;

      default:
        gan_err_flush_trace();
        gan_err_register (  "bgr_extract_blue",
                            GAN_ERROR_NOT_IMPLEMENTED, "image conversion" );
        return NULL;
   }

   /* success */
   return dest;
}

/* extract the red channel from an BGR colour image with alpha channel */
static Gan_Image *
 bgra_extract_red ( const Gan_Image *source,
                    unsigned r0,     unsigned c0,
                    unsigned height, unsigned width,
                    Gan_Image *dest )
{
   int i, j;

   /* consistency check */
   gan_err_test_ptr ( source->format == GAN_BGR_COLOUR_ALPHA_IMAGE,
                      "bgra_extract_red", GAN_ERROR_INCOMPATIBLE, "" );

   switch ( source->type )
   {
      case GAN_UCHAR:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_uc ( dest, i, j,
                                        gan_image_get_pixptr_bgra_uc(source,
                                                                     r0+i,
                                                                     c0+j)->R);
        break;

      case GAN_SHORT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_s ( dest, i, j,
                                       gan_image_get_pixptr_bgra_s(source,
                                                                   r0+i,
                                                                   c0+j)->R);
        break;

      case GAN_USHORT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_us ( dest, i, j,
                                        gan_image_get_pixptr_bgra_us(source,
                                                                     r0+i,
                                                                     c0+j)->R);
        break;

      case GAN_INT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_i ( dest, i, j,
                                       gan_image_get_pixptr_bgra_i(source,
                                                                   r0+i,
                                                                   c0+j)->R);
        break;

      case GAN_UINT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_ui ( dest, i, j,
                                        gan_image_get_pixptr_bgra_ui(source,
                                                                     r0+i,
                                                                     c0+j)->R);
        break;

      case GAN_FLOAT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_f ( dest, i, j,
                                       gan_image_get_pixptr_bgra_f(source,
                                                                   r0+i,
                                                                   c0+j)->R);
        break;

      default:
        gan_err_flush_trace();
        gan_err_register (  "bgra_extract_red",
                            GAN_ERROR_NOT_IMPLEMENTED, "image conversion" );
        return NULL;
   }

   /* success */
   return dest;
}

/* extract the green channel from an BGR colour image with alpha channel */
static Gan_Image *
 bgra_extract_green ( const Gan_Image *source,
                      unsigned r0,     unsigned c0,
                      unsigned height, unsigned width,
                      Gan_Image *dest )
{
   int i, j;

   /* consistency check */
   gan_err_test_ptr ( source->format == GAN_BGR_COLOUR_ALPHA_IMAGE,
                      "bgra_extract_green", GAN_ERROR_INCOMPATIBLE, "" );

   switch ( source->type )
   {
      case GAN_UCHAR:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_uc ( dest, i, j,
                                        gan_image_get_pixptr_bgra_uc(source,
                                                                     r0+i,
                                                                     c0+j)->G);
        break;

      case GAN_SHORT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_s ( dest, i, j,
                                       gan_image_get_pixptr_bgra_s(source,
                                                                   r0+i,
                                                                   c0+j)->G);
        break;

      case GAN_USHORT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_us ( dest, i, j,
                                        gan_image_get_pixptr_bgra_us(source,
                                                                     r0+i,
                                                                     c0+j)->G);
        break;

      case GAN_INT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_i ( dest, i, j,
                                       gan_image_get_pixptr_bgra_i(source,
                                                                   r0+i,
                                                                   c0+j)->G);
        break;

      case GAN_UINT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_ui ( dest, i, j,
                                        gan_image_get_pixptr_bgra_ui(source,
                                                                     r0+i,
                                                                     c0+j)->G);
        break;

      case GAN_FLOAT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_f ( dest, i, j,
                                       gan_image_get_pixptr_bgra_f(source,
                                                                   r0+i,
                                                                   c0+j)->G);
        break;

      default:
        gan_err_flush_trace();
        gan_err_register (  "bgra_extract_green",
                            GAN_ERROR_NOT_IMPLEMENTED, "image conversion" );
        return NULL;
   }

   /* success */
   return dest;
}

/* extract the blue channel from an BGR colour image with alpha channel */
static Gan_Image *
 bgra_extract_blue ( const Gan_Image *source,
                     unsigned r0,     unsigned c0,
                     unsigned height, unsigned width,
                     Gan_Image *dest )
{
   int i, j;

   /* consistency check */
   gan_err_test_ptr ( source->format == GAN_BGR_COLOUR_ALPHA_IMAGE,
                      "bgra_extract_blue", GAN_ERROR_INCOMPATIBLE, "" );

   switch ( source->type )
   {
      case GAN_UCHAR:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_uc ( dest, i, j,
                                        gan_image_get_pixptr_bgra_uc(source,
                                                                     r0+i,
                                                                     c0+j)->B);
        break;

      case GAN_SHORT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_s ( dest, i, j,
                                       gan_image_get_pixptr_bgra_s(source,
                                                                   r0+i,
                                                                   c0+j)->B);
        break;

      case GAN_USHORT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_us ( dest, i, j,
                                        gan_image_get_pixptr_bgra_us(source,
                                                                     r0+i,
                                                                     c0+j)->B);
        break;

      case GAN_INT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_i ( dest, i, j,
                                       gan_image_get_pixptr_bgra_i(source,
                                                                   r0+i,
                                                                   c0+j)->B);
        break;

      case GAN_UINT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_ui ( dest, i, j,
                                        gan_image_get_pixptr_bgra_ui(source,
                                                                     r0+i,
                                                                     c0+j)->B);
        break;

      case GAN_FLOAT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_f ( dest, i, j,
                                       gan_image_get_pixptr_bgra_f(source,
                                                                   r0+i,
                                                                   c0+j)->B);
        break;

      default:
        gan_err_flush_trace();
        gan_err_register (  "bgra_extract_blue",
                            GAN_ERROR_NOT_IMPLEMENTED, "image conversion" );
        return NULL;
   }

   /* success */
   return dest;
}

/* extract the alpha channel from an BGR colour image with alpha channel */
static Gan_Image *
 bgra_extract_alpha ( const Gan_Image *source,
                      unsigned r0,     unsigned c0,
                      unsigned height, unsigned width,
                      Gan_Image *dest )
{
   int i, j;

   /* consistency check */
   gan_err_test_ptr ( source->format == GAN_BGR_COLOUR_ALPHA_IMAGE,
                      "bgra_extract_alpha", GAN_ERROR_INCOMPATIBLE, "" );

   switch ( source->type )
   {
      case GAN_UCHAR:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_uc ( dest, i, j,
                                        gan_image_get_pixptr_bgra_uc(source,
                                                                     r0+i,
                                                                     c0+j)->A);
        break;

      case GAN_SHORT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_s ( dest, i, j,
                                       gan_image_get_pixptr_bgra_s(source,
                                                                   r0+i,
                                                                   c0+j)->A);
        break;

      case GAN_USHORT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_us ( dest, i, j,
                                        gan_image_get_pixptr_bgra_us(source,
                                                                     r0+i,
                                                                     c0+j)->A);
        break;

      case GAN_INT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_i ( dest, i, j,
                                       gan_image_get_pixptr_bgra_i(source,
                                                                   r0+i,
                                                                   c0+j)->A);
        break;

      case GAN_UINT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_ui ( dest, i, j,
                                        gan_image_get_pixptr_bgra_ui(source,
                                                                     r0+i,
                                                                     c0+j)->A);
        break;

      case GAN_FLOAT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_f ( dest, i, j,
                                       gan_image_get_pixptr_bgra_f(source,
                                                                   r0+i,
                                                                   c0+j)->A);
        break;

      default:
        gan_err_flush_trace();
        gan_err_register (  "bgra_extract_alpha",
                            GAN_ERROR_NOT_IMPLEMENTED, "image conversion" );
        return NULL;
   }

   /* success */
   return dest;
}

/* extract the x coordinate channel from a 2D vector field image */
static Gan_Image *
 vfield2D_extract_x ( const Gan_Image *source,
                      unsigned r0,     unsigned c0,
                      unsigned height, unsigned width,
                      Gan_Image *dest )
{
   int i, j;

   /* consistency check */
   gan_err_test_ptr ( source->format == GAN_VECTOR_FIELD_2D,
                      "vfield2D_extract_x", GAN_ERROR_INCOMPATIBLE, "" );

   switch ( source->type )
   {
      case GAN_SHORT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_s(dest, i, j,
                                     gan_image_get_pixptr_vfield2D_s(source,
                                                                     r0+i,
                                                                     c0+j)->x);
        break;

      case GAN_INT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_i(dest, i, j,
                                     gan_image_get_pixptr_vfield2D_i(source,
                                                                     r0+i,
                                                                     c0+j)->x);
        break;

      case GAN_FLOAT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_f(dest, i, j,
                                     gan_image_get_pixptr_vfield2D_f(source,
                                                                     r0+i,
                                                                     c0+j)->x);
        break;

      case GAN_DOUBLE:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_d(dest, i, j,
                                     gan_image_get_pixptr_vfield2D_d(source,
                                                                     r0+i,
                                                                     c0+j)->x);
        break;

      default:
        gan_err_flush_trace();
        gan_err_register (  "vfield2D_extract_x",
                            GAN_ERROR_NOT_IMPLEMENTED, "image conversion" );
        return NULL;
   }

   /* success */
   return dest;
}

/* extract the y coordinate channel from a 2D vector field image */
static Gan_Image *
 vfield2D_extract_y ( const Gan_Image *source,
                      unsigned r0,     unsigned c0,
                      unsigned height, unsigned width,
                      Gan_Image *dest )
{
   int i, j;

   /* consistency check */
   gan_err_test_ptr ( source->format == GAN_VECTOR_FIELD_2D,
                      "vfield2D_extract_y", GAN_ERROR_INCOMPATIBLE, "" );

   switch ( source->type )
   {
      case GAN_SHORT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_s(dest, i, j,
                                     gan_image_get_pixptr_vfield2D_s(source,
                                                                     r0+i,
                                                                     c0+j)->y);
        break;

      case GAN_INT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_i(dest, i, j,
                                     gan_image_get_pixptr_vfield2D_i(source,
                                                                     r0+i,
                                                                     c0+j)->y);
        break;

      case GAN_FLOAT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_f(dest, i, j,
                                     gan_image_get_pixptr_vfield2D_f(source,
                                                                     r0+i,
                                                                     c0+j)->y);
        break;

      case GAN_DOUBLE:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_d(dest, i, j,
                                     gan_image_get_pixptr_vfield2D_d(source,
                                                                     r0+i,
                                                                     c0+j)->y);
        break;

      default:
        gan_err_flush_trace();
        gan_err_register (  "vfield2D_extract_y",
                            GAN_ERROR_NOT_IMPLEMENTED, "image conversion" );
        return NULL;
   }

   /* success */
   return dest;
}

/* extract the x coordinate channel from a 3D vector field image */
static Gan_Image *
 vfield3D_extract_x ( const Gan_Image *source,
                      unsigned r0,     unsigned c0,
                      unsigned height, unsigned width,
                      Gan_Image *dest )
{
   int i, j;

   /* consistency check */
   gan_err_test_ptr ( source->format == GAN_VECTOR_FIELD_3D,
                      "vfield3D_extract_x", GAN_ERROR_INCOMPATIBLE, "" );

   switch ( source->type )
   {
      case GAN_SHORT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_s(dest, i, j,
                                     gan_image_get_pixptr_vfield3D_s(source,
                                                                     r0+i,
                                                                     c0+j)->x);
        break;

      case GAN_INT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_i(dest, i, j,
                                     gan_image_get_pixptr_vfield3D_i(source,
                                                                     r0+i,
                                                                     c0+j)->x);
        break;

      case GAN_FLOAT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_f(dest, i, j,
                                     gan_image_get_pixptr_vfield3D_f(source,
                                                                     r0+i,
                                                                     c0+j)->x);
        break;

      case GAN_DOUBLE:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_d(dest, i, j,
                                     gan_image_get_pixptr_vfield3D_d(source,
                                                                     r0+i,
                                                                     c0+j)->x);
        break;

      default:
        gan_err_flush_trace();
        gan_err_register (  "vfield3D_extract_x",
                            GAN_ERROR_NOT_IMPLEMENTED, "image conversion" );
        return NULL;
   }

   /* success */
   return dest;
}

/* extract the y coordinate channel from a 3D vector field image */
static Gan_Image *
 vfield3D_extract_y ( const Gan_Image *source,
                      unsigned r0,     unsigned c0,
                      unsigned height, unsigned width,
                      Gan_Image *dest )
{
   int i, j;

   /* consistency check */
   gan_err_test_ptr ( source->format == GAN_VECTOR_FIELD_3D,
                      "vfield3D_extract_y", GAN_ERROR_INCOMPATIBLE, "" );

   switch ( source->type )
   {
      case GAN_SHORT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_s(dest, i, j,
                                     gan_image_get_pixptr_vfield3D_s(source,
                                                                     r0+i,
                                                                     c0+j)->y);
        break;

      case GAN_INT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_i(dest, i, j,
                                     gan_image_get_pixptr_vfield3D_i(source,
                                                                     r0+i,
                                                                     c0+j)->y);
        break;

      case GAN_FLOAT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_f(dest, i, j,
                                     gan_image_get_pixptr_vfield3D_f(source,
                                                                     r0+i,
                                                                     c0+j)->y);
        break;

      case GAN_DOUBLE:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_d(dest, i, j,
                                     gan_image_get_pixptr_vfield3D_d(source,
                                                                     r0+i,
                                                                     c0+j)->y);
        break;

      default:
        gan_err_flush_trace();
        gan_err_register (  "vfield3D_extract_y",
                            GAN_ERROR_NOT_IMPLEMENTED, "image conversion" );
        return NULL;
   }

   /* success */
   return dest;
}

/* extract the z coordinate channel from a 3D vector field image */
static Gan_Image *
 vfield3D_extract_z ( const Gan_Image *source,
                      unsigned r0,     unsigned c0,
                      unsigned height, unsigned width,
                      Gan_Image *dest )
{
   int i, j;

   /* consistency check */
   gan_err_test_ptr ( source->format == GAN_VECTOR_FIELD_3D,
                      "vfield3D_extract_z", GAN_ERROR_INCOMPATIBLE, "" );

   switch ( source->type )
   {
      case GAN_SHORT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_s(dest, i, j,
                                     gan_image_get_pixptr_vfield3D_s(source,
                                                                     r0+i,
                                                                     c0+j)->z);
        break;

      case GAN_INT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_i(dest, i, j,
                                     gan_image_get_pixptr_vfield3D_i(source,
                                                                     r0+i,
                                                                     c0+j)->z);
        break;

      case GAN_FLOAT:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_f(dest, i, j,
                                     gan_image_get_pixptr_vfield3D_f(source,
                                                                     r0+i,
                                                                     c0+j)->z);
        break;

      case GAN_DOUBLE:
        for ( i = (int)height-1; i >= 0; i-- )
           for ( j = (int)width-1; j >= 0; j-- )
              gan_image_set_pix_gl_d(dest, i, j,
                                     gan_image_get_pixptr_vfield3D_d(source,
                                                                     r0+i,
                                                                     c0+j)->z);
        break;

      default:
        gan_err_flush_trace();
        gan_err_register (  "vfield3D_extract_z",
                            GAN_ERROR_NOT_IMPLEMENTED, "image conversion" );
        return NULL;
   }

   /* success */
   return dest;
}

/**
 * \brief Extracts a specific channel from part of an image.
 * \param source The input image
 * \param channel The channel to extract from the image
 * \param r0 Vertical offset of start of region
 * \param c0 Horizontal offset of start of region
 * \param height Height of region in pixels
 * \param width Width of region in pixels
 * \param dest The destination image
 * \return A pointer to the extracted image, or \c NULL on failure.
 *
 * Extracts a specific channel in a sub-part of an image,
 *
 * \sa gan_image_extract_channel_s().
 */
Gan_Image *
 gan_image_extract_channel_q ( const Gan_Image *source,
                               Gan_ImageChannelType channel,
                               unsigned r0,     unsigned c0,
                               unsigned height, unsigned width,
                               Gan_Image *dest )
{
   gan_err_test_ptr ( r0+height <= source->height && c0+width <= source->width,
                      "gan_image_extract_channel_q", GAN_ERROR_INCOMPATIBLE,
                      "illegal portion of source image" );

   /* allocate/set image */
   if ( dest == NULL )
      dest = gan_image_alloc ( GAN_GREY_LEVEL_IMAGE, source->type,
                               height, width );
   else
      dest = gan_image_set_format_type_dims ( dest, GAN_GREY_LEVEL_IMAGE,
                                              source->type, height, width );

   if ( dest == NULL )
   {
      gan_err_register ( "gan_image_extract_channel_q", GAN_ERROR_FAILURE, "");
      return NULL;
   }

   switch ( source->format )
   {
      case GAN_GREY_LEVEL_IMAGE:
        switch ( channel )
        {
           case GAN_INTENSITY_CHANNEL:
             dest = gan_image_extract_q ( source, r0, c0, height, width,
                                          source->format, source->type,
                                          GAN_TRUE, dest );
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_extract_channel_q", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;

      case GAN_GREY_LEVEL_ALPHA_IMAGE:
        switch ( channel )
        {
           case GAN_INTENSITY_CHANNEL:
             dest = gla_extract_intensity ( source, r0, c0, height, width, dest );
             break;

           case GAN_ALPHA_CHANNEL:
             dest = gla_extract_alpha ( source, r0, c0, height, width, dest );
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_extract_channel_q", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;

      case GAN_RGB_COLOUR_IMAGE:
        switch ( channel )
        {
           case GAN_RED_CHANNEL:
             dest = rgb_extract_red ( source, r0, c0, height, width, dest );
             break;

           case GAN_GREEN_CHANNEL:
             dest = rgb_extract_green ( source, r0, c0, height, width, dest );
             break;

           case GAN_BLUE_CHANNEL:
             dest = rgb_extract_blue ( source, r0, c0, height, width, dest );
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_extract_channel_q", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;

      case GAN_RGB_COLOUR_ALPHA_IMAGE:
        switch ( channel )
        {
           case GAN_RED_CHANNEL:
             dest = rgba_extract_red ( source, r0, c0, height, width, dest );
             break;

           case GAN_GREEN_CHANNEL:
             dest = rgba_extract_green ( source, r0, c0, height, width, dest );
             break;

           case GAN_BLUE_CHANNEL:
             dest = rgba_extract_blue ( source, r0, c0, height, width, dest );
             break;

           case GAN_ALPHA_CHANNEL:
             dest = rgba_extract_alpha ( source, r0, c0, height, width, dest );
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_extract_channel_q",
                                GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;

      case GAN_BGR_COLOUR_IMAGE:
        switch ( channel )
        {
           case GAN_RED_CHANNEL:
             dest = bgr_extract_red ( source, r0, c0, height, width, dest );
             break;

           case GAN_GREEN_CHANNEL:
             dest = bgr_extract_green ( source, r0, c0, height, width, dest );
             break;

           case GAN_BLUE_CHANNEL:
             dest = bgr_extract_blue ( source, r0, c0, height, width, dest );
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_extract_channel_q", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;

      case GAN_BGR_COLOUR_ALPHA_IMAGE:
        switch ( channel )
        {
           case GAN_RED_CHANNEL:
             dest = bgra_extract_red ( source, r0, c0, height, width, dest );
             break;

           case GAN_GREEN_CHANNEL:
             dest = bgra_extract_green ( source, r0, c0, height, width, dest );
             break;

           case GAN_BLUE_CHANNEL:
             dest = bgra_extract_blue ( source, r0, c0, height, width, dest );
             break;

           case GAN_ALPHA_CHANNEL:
             dest = bgra_extract_alpha ( source, r0, c0, height, width, dest );
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_extract_channel_q",
                                GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;

      case GAN_VECTOR_FIELD_2D:
        switch ( channel )
        {
           case GAN_X_CHANNEL:
             dest = vfield2D_extract_x ( source, r0, c0, height, width, dest );
             break;

           case GAN_Y_CHANNEL:
             dest = vfield2D_extract_y ( source, r0, c0, height, width, dest );
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_extract_channel_q",
                                GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;

      case GAN_VECTOR_FIELD_3D:
        switch ( channel )
        {
           case GAN_X_CHANNEL:
             dest = vfield3D_extract_x ( source, r0, c0, height, width, dest );
             break;

           case GAN_Y_CHANNEL:
             dest = vfield3D_extract_y ( source, r0, c0, height, width, dest );
             break;

           case GAN_Z_CHANNEL:
             dest = vfield3D_extract_z ( source, r0, c0, height, width, dest );
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_extract_channel_q",
                                GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_image_extract_channel_q", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
        return NULL;
   }
                                          
   if ( dest == NULL )
      gan_err_register ( "gan_image_extract_channel_q", GAN_ERROR_FAILURE,
                         "" );

   /* set offsets */
   dest->offset_x = source->offset_x+(int)c0;
   dest->offset_y = source->offset_x+(int)r0;

   /* success */
   return dest;
}

static Gan_Bool insert_gl_into_rgb ( const Gan_Image *source,
                                     Gan_ImageChannelType schannel,
                                     Gan_Image *dest,
                                     Gan_ImageChannelType dchannel )
{
   int r, c;

   switch(source->type)
   {
      case GAN_UCHAR:
      {
         unsigned char *spix;
         Gan_RGBPixel_uc *dpix;

         switch(schannel)
         {
            case GAN_INTENSITY_CHANNEL:
              switch(dchannel)
              {
                 case GAN_RED_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_gl_uc(source,r,0),
                          dpix = gan_image_get_pixptr_rgb_uc(dest,r,0); c>=0; c--, dpix++)
                         dpix->R = *spix++;
                   break;

                 case GAN_GREEN_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_gl_uc(source,r,0),
                          dpix = gan_image_get_pixptr_rgb_uc(dest,r,0); c>=0; c--, dpix++)
                         dpix->G = *spix++;
                   break;

                 case GAN_BLUE_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_gl_uc(source,r,0),
                          dpix = gan_image_get_pixptr_rgb_uc(dest,r,0); c>=0; c--, dpix++)
                         dpix->B = *spix++;
                   break;

                default:
                  gan_err_flush_trace();
                  gan_err_register("insert_gl_into_rgb", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
                  return GAN_FALSE;
             }
             break;

           default:
             gan_err_flush_trace();
             gan_err_register("insert_gl_into_rgb", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
             return GAN_FALSE;
         }
         break;
      }
      
      case GAN_USHORT:
      {
         unsigned short *spix;
         Gan_RGBPixel_us *dpix;

         switch(schannel)
         {
            case GAN_INTENSITY_CHANNEL:
              switch(dchannel)
              {
                 case GAN_RED_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_gl_us(source,r,0),
                          dpix = gan_image_get_pixptr_rgb_us(dest,r,0); c>=0; c--, dpix++)
                         dpix->R = *spix++;
                   break;

                 case GAN_GREEN_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_gl_us(source,r,0),
                          dpix = gan_image_get_pixptr_rgb_us(dest,r,0); c>=0; c--, dpix++)
                         dpix->G = *spix++;
                   break;

                 case GAN_BLUE_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_gl_us(source,r,0),
                          dpix = gan_image_get_pixptr_rgb_us(dest,r,0); c>=0; c--, dpix++)
                         dpix->B = *spix++;
                   break;

                default:
                  gan_err_flush_trace();
                  gan_err_register("insert_gl_into_rgb", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
                  return GAN_FALSE;
             }
             break;

           default:
             gan_err_flush_trace();
             gan_err_register("insert_gl_into_rgb", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
             return GAN_FALSE;
         }
         break;
      }
      
      case GAN_FLOAT:
      {
         float *spix;
         Gan_RGBPixel_f *dpix;

         switch(schannel)
         {
            case GAN_INTENSITY_CHANNEL:
              switch(dchannel)
              {
                 case GAN_RED_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_gl_f(source,r,0),
                          dpix = gan_image_get_pixptr_rgb_f(dest,r,0); c>=0; c--, dpix++)
                         dpix->R = *spix++;
                   break;

                 case GAN_GREEN_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_gl_f(source,r,0),
                          dpix = gan_image_get_pixptr_rgb_f(dest,r,0); c>=0; c--, dpix++)
                         dpix->G = *spix++;
                   break;

                 case GAN_BLUE_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_gl_f(source,r,0),
                          dpix = gan_image_get_pixptr_rgb_f(dest,r,0); c>=0; c--, dpix++)
                         dpix->B = *spix++;
                   break;

                default:
                  gan_err_flush_trace();
                  gan_err_register("insert_gl_into_rgb", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
                  return GAN_FALSE;
             }
             break;

           default:
             gan_err_flush_trace();
             gan_err_register("insert_gl_into_rgb", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
             return GAN_FALSE;
         }
         break;
      }
      
      default:
        gan_err_flush_trace();
        gan_err_register("insert_gl_into_rgb", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "");
        return GAN_FALSE;
   }

   return GAN_TRUE;
}

static Gan_Bool insert_gl_into_rgba ( const Gan_Image *source,
                                      Gan_ImageChannelType schannel,
                                      Gan_Image *dest,
                                      Gan_ImageChannelType dchannel )
{
   int r, c;

   switch(source->type)
   {
      case GAN_UCHAR:
      {
         unsigned char *spix;
         Gan_RGBAPixel_uc *dpix;

         switch(schannel)
         {
            case GAN_INTENSITY_CHANNEL:
              switch(dchannel)
              {
                 case GAN_RED_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_gl_uc(source,r,0),
                          dpix = gan_image_get_pixptr_rgba_uc(dest,r,0); c>=0; c--, dpix++)
                         dpix->R = *spix++;
                   break;

                 case GAN_GREEN_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_gl_uc(source,r,0),
                          dpix = gan_image_get_pixptr_rgba_uc(dest,r,0); c>=0; c--, dpix++)
                         dpix->G = *spix++;
                   break;

                 case GAN_BLUE_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_gl_uc(source,r,0),
                          dpix = gan_image_get_pixptr_rgba_uc(dest,r,0); c>=0; c--, dpix++)
                         dpix->B = *spix++;
                   break;

                 case GAN_ALPHA_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_gl_uc(source,r,0),
                          dpix = gan_image_get_pixptr_rgba_uc(dest,r,0); c>=0; c--, dpix++)
                         dpix->A = *spix++;
                   break;

                default:
                  gan_err_flush_trace();
                  gan_err_register("insert_gl_into_rgba", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
                  return GAN_FALSE;
             }
             break;

           default:
             gan_err_flush_trace();
             gan_err_register("insert_gl_into_rgba", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
             return GAN_FALSE;
         }
         break;
      }
      
      case GAN_USHORT:
      {
         unsigned short *spix;
         Gan_RGBAPixel_us *dpix;

         switch(schannel)
         {
            case GAN_INTENSITY_CHANNEL:
              switch(dchannel)
              {
                 case GAN_RED_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_gl_us(source,r,0),
                          dpix = gan_image_get_pixptr_rgba_us(dest,r,0); c>=0; c--, dpix++)
                         dpix->R = *spix++;
                   break;

                 case GAN_GREEN_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_gl_us(source,r,0),
                          dpix = gan_image_get_pixptr_rgba_us(dest,r,0); c>=0; c--, dpix++)
                         dpix->G = *spix++;
                   break;

                 case GAN_BLUE_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_gl_us(source,r,0),
                          dpix = gan_image_get_pixptr_rgba_us(dest,r,0); c>=0; c--, dpix++)
                         dpix->B = *spix++;
                   break;

                 case GAN_ALPHA_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_gl_us(source,r,0),
                          dpix = gan_image_get_pixptr_rgba_us(dest,r,0); c>=0; c--, dpix++)
                         dpix->A = *spix++;
                   break;

                default:
                  gan_err_flush_trace();
                  gan_err_register("insert_gl_into_rgba", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
                  return GAN_FALSE;
             }
             break;

           default:
             gan_err_flush_trace();
             gan_err_register("insert_gl_into_rgba", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
             return GAN_FALSE;
         }
         break;
      }
      
      case GAN_FLOAT:
      {
         float *spix;
         Gan_RGBAPixel_f *dpix;

         switch(schannel)
         {
            case GAN_INTENSITY_CHANNEL:
              switch(dchannel)
              {
                 case GAN_RED_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_gl_f(source,r,0),
                          dpix = gan_image_get_pixptr_rgba_f(dest,r,0); c>=0; c--, dpix++)
                         dpix->R = *spix++;
                   break;

                 case GAN_GREEN_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_gl_f(source,r,0),
                          dpix = gan_image_get_pixptr_rgba_f(dest,r,0); c>=0; c--, dpix++)
                         dpix->G = *spix++;
                   break;

                 case GAN_BLUE_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_gl_f(source,r,0),
                          dpix = gan_image_get_pixptr_rgba_f(dest,r,0); c>=0; c--, dpix++)
                         dpix->B = *spix++;
                   break;

                 case GAN_ALPHA_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_gl_f(source,r,0),
                          dpix = gan_image_get_pixptr_rgba_f(dest,r,0); c>=0; c--, dpix++)
                         dpix->A = *spix++;
                   break;

                default:
                  gan_err_flush_trace();
                  gan_err_register("insert_gl_into_rgba", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
                  return GAN_FALSE;
             }
             break;

           default:
             gan_err_flush_trace();
             gan_err_register("insert_gl_into_rgba", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
             return GAN_FALSE;
         }
         break;
      }
      
      default:
        gan_err_flush_trace();
        gan_err_register("insert_gl_into_rgba", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "");
        return GAN_FALSE;
   }

   return GAN_TRUE;
}

static Gan_Bool insert_rgba_into_rgb ( const Gan_Image *source,
                                       Gan_ImageChannelType schannel,
                                       Gan_Image *dest,
                                       Gan_ImageChannelType dchannel )
{
   int r, c;

   switch(source->type)
   {
      case GAN_UCHAR:
      {
         Gan_RGBAPixel_uc *spix;
         Gan_RGBPixel_uc *dpix;

         switch(schannel)
         {
            case GAN_RED_CHANNEL:
              switch(dchannel)
              {
                 case GAN_RED_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_uc(source,r,0),
                          dpix = gan_image_get_pixptr_rgb_uc(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->R = spix->R;
                   break;

                 case GAN_GREEN_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_uc(source,r,0),
                          dpix = gan_image_get_pixptr_rgb_uc(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->G = spix->R;
                   break;

                 case GAN_BLUE_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_uc(source,r,0),
                          dpix = gan_image_get_pixptr_rgb_uc(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->B = spix->R;
                   break;

                default:
                  gan_err_flush_trace();
                  gan_err_register("insert_rgba_into_rgb", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
                  return GAN_FALSE;
             }
             break;

            case GAN_GREEN_CHANNEL:
              switch(dchannel)
              {
                 case GAN_RED_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_uc(source,r,0),
                          dpix = gan_image_get_pixptr_rgb_uc(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->R = spix->G;
                   break;

                 case GAN_GREEN_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_uc(source,r,0),
                          dpix = gan_image_get_pixptr_rgb_uc(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->G = spix->G;
                   break;

                 case GAN_BLUE_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_uc(source,r,0),
                          dpix = gan_image_get_pixptr_rgb_uc(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->B = spix->G;
                   break;

                default:
                  gan_err_flush_trace();
                  gan_err_register("insert_rgba_into_rgb", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
                  return GAN_FALSE;
             }
             break;

            case GAN_BLUE_CHANNEL:
              switch(dchannel)
              {
                 case GAN_RED_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_uc(source,r,0),
                          dpix = gan_image_get_pixptr_rgb_uc(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->R = spix->B;
                   break;

                 case GAN_GREEN_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_uc(source,r,0),
                          dpix = gan_image_get_pixptr_rgb_uc(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->G = spix->B;
                   break;

                 case GAN_BLUE_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_uc(source,r,0),
                          dpix = gan_image_get_pixptr_rgb_uc(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->B = spix->B;
                   break;

                default:
                  gan_err_flush_trace();
                  gan_err_register("insert_rgba_into_rgb", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
                  return GAN_FALSE;
             }
             break;

            case GAN_ALPHA_CHANNEL:
              switch(dchannel)
              {
                 case GAN_RED_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_uc(source,r,0),
                          dpix = gan_image_get_pixptr_rgb_uc(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->R = spix->A;
                   break;

                 case GAN_GREEN_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_uc(source,r,0),
                          dpix = gan_image_get_pixptr_rgb_uc(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->G = spix->A;
                   break;

                 case GAN_BLUE_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_uc(source,r,0),
                          dpix = gan_image_get_pixptr_rgb_uc(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->B = spix->A;
                   break;

                default:
                  gan_err_flush_trace();
                  gan_err_register("insert_rgba_into_rgb", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
                  return GAN_FALSE;
             }
             break;

           default:
             gan_err_flush_trace();
             gan_err_register("insert_rgba_into_rgb", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
             return GAN_FALSE;
         }
         break;
      }
      
      case GAN_USHORT:
      {
         Gan_RGBAPixel_us *spix;
         Gan_RGBPixel_us *dpix;

         switch(schannel)
         {
            case GAN_RED_CHANNEL:
              switch(dchannel)
              {
                 case GAN_RED_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_us(source,r,0),
                          dpix = gan_image_get_pixptr_rgb_us(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->R = spix->R;
                   break;

                 case GAN_GREEN_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_us(source,r,0),
                          dpix = gan_image_get_pixptr_rgb_us(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->G = spix->R;
                   break;

                 case GAN_BLUE_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_us(source,r,0),
                          dpix = gan_image_get_pixptr_rgb_us(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->B = spix->R;
                   break;

                default:
                  gan_err_flush_trace();
                  gan_err_register("insert_rgba_into_rgb", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
                  return GAN_FALSE;
             }
             break;

            case GAN_GREEN_CHANNEL:
              switch(dchannel)
              {
                 case GAN_RED_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_us(source,r,0),
                          dpix = gan_image_get_pixptr_rgb_us(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->R = spix->G;
                   break;

                 case GAN_GREEN_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_us(source,r,0),
                          dpix = gan_image_get_pixptr_rgb_us(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->G = spix->G;
                   break;

                 case GAN_BLUE_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_us(source,r,0),
                          dpix = gan_image_get_pixptr_rgb_us(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->B = spix->G;
                   break;

                default:
                  gan_err_flush_trace();
                  gan_err_register("insert_rgba_into_rgb", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
                  return GAN_FALSE;
             }
             break;

            case GAN_BLUE_CHANNEL:
              switch(dchannel)
              {
                 case GAN_RED_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_us(source,r,0),
                          dpix = gan_image_get_pixptr_rgb_us(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->R = spix->B;
                   break;

                 case GAN_GREEN_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_us(source,r,0),
                          dpix = gan_image_get_pixptr_rgb_us(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->G = spix->B;
                   break;

                 case GAN_BLUE_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_us(source,r,0),
                          dpix = gan_image_get_pixptr_rgb_us(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->B = spix->B;
                   break;

                default:
                  gan_err_flush_trace();
                  gan_err_register("insert_rgba_into_rgb", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
                  return GAN_FALSE;
             }
             break;

            case GAN_ALPHA_CHANNEL:
              switch(dchannel)
              {
                 case GAN_RED_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_us(source,r,0),
                          dpix = gan_image_get_pixptr_rgb_us(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->R = spix->A;
                   break;

                 case GAN_GREEN_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_us(source,r,0),
                          dpix = gan_image_get_pixptr_rgb_us(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->G = spix->A;
                   break;

                 case GAN_BLUE_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_us(source,r,0),
                          dpix = gan_image_get_pixptr_rgb_us(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->B = spix->A;
                   break;

                default:
                  gan_err_flush_trace();
                  gan_err_register("insert_rgba_into_rgb", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
                  return GAN_FALSE;
             }
             break;

           default:
             gan_err_flush_trace();
             gan_err_register("insert_rgba_into_rgb", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
             return GAN_FALSE;
         }
         break;
      }
      
      case GAN_FLOAT:
      {
         Gan_RGBAPixel_f *spix;
         Gan_RGBPixel_f *dpix;

         switch(schannel)
         {
            case GAN_RED_CHANNEL:
              switch(dchannel)
              {
                 case GAN_RED_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_f(source,r,0),
                          dpix = gan_image_get_pixptr_rgb_f(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->R = spix->R;
                   break;

                 case GAN_GREEN_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_f(source,r,0),
                          dpix = gan_image_get_pixptr_rgb_f(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->G = spix->R;
                   break;

                 case GAN_BLUE_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_f(source,r,0),
                          dpix = gan_image_get_pixptr_rgb_f(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->B = spix->R;
                   break;

                default:
                  gan_err_flush_trace();
                  gan_err_register("insert_rgba_into_rgb", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
                  return GAN_FALSE;
             }
             break;

            case GAN_GREEN_CHANNEL:
              switch(dchannel)
              {
                 case GAN_RED_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_f(source,r,0),
                          dpix = gan_image_get_pixptr_rgb_f(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->R = spix->G;
                   break;

                 case GAN_GREEN_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_f(source,r,0),
                          dpix = gan_image_get_pixptr_rgb_f(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->G = spix->G;
                   break;

                 case GAN_BLUE_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_f(source,r,0),
                          dpix = gan_image_get_pixptr_rgb_f(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->B = spix->G;
                   break;

                default:
                  gan_err_flush_trace();
                  gan_err_register("insert_rgba_into_rgb", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
                  return GAN_FALSE;
             }
             break;

            case GAN_BLUE_CHANNEL:
              switch(dchannel)
              {
                 case GAN_RED_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_f(source,r,0),
                          dpix = gan_image_get_pixptr_rgb_f(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->R = spix->B;
                   break;

                 case GAN_GREEN_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_f(source,r,0),
                          dpix = gan_image_get_pixptr_rgb_f(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->G = spix->B;
                   break;

                 case GAN_BLUE_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_f(source,r,0),
                          dpix = gan_image_get_pixptr_rgb_f(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->B = spix->B;
                   break;

                default:
                  gan_err_flush_trace();
                  gan_err_register("insert_rgba_into_rgb", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
                  return GAN_FALSE;
             }
             break;

            case GAN_ALPHA_CHANNEL:
              switch(dchannel)
              {
                 case GAN_RED_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_f(source,r,0),
                          dpix = gan_image_get_pixptr_rgb_f(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->R = spix->A;
                   break;

                 case GAN_GREEN_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_f(source,r,0),
                          dpix = gan_image_get_pixptr_rgb_f(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->G = spix->A;
                   break;

                 case GAN_BLUE_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_f(source,r,0),
                          dpix = gan_image_get_pixptr_rgb_f(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->B = spix->A;
                   break;

                default:
                  gan_err_flush_trace();
                  gan_err_register("insert_rgba_into_rgb", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
                  return GAN_FALSE;
             }
             break;

           default:
             gan_err_flush_trace();
             gan_err_register("insert_rgba_into_rgb", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
             return GAN_FALSE;
         }
         break;
      }
      
      default:
        gan_err_flush_trace();
        gan_err_register("insert_rgba_into_rgb", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "");
        return GAN_FALSE;
   }

   return GAN_TRUE;
}

static Gan_Bool insert_rgba_into_rgba ( const Gan_Image *source,
                                      Gan_ImageChannelType schannel,
                                      Gan_Image *dest,
                                      Gan_ImageChannelType dchannel )
{
   int r, c;

   switch(source->type)
   {
      case GAN_UCHAR:
      {
         Gan_RGBAPixel_uc *spix;
         Gan_RGBAPixel_uc *dpix;

         switch(schannel)
         {
            case GAN_RED_CHANNEL:
              switch(dchannel)
              {
                 case GAN_RED_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_uc(source,r,0),
                          dpix = gan_image_get_pixptr_rgba_uc(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->R = spix->R;
                   break;

                 case GAN_GREEN_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_uc(source,r,0),
                          dpix = gan_image_get_pixptr_rgba_uc(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->G = spix->R;
                   break;

                 case GAN_BLUE_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_uc(source,r,0),
                          dpix = gan_image_get_pixptr_rgba_uc(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->B = spix->R;
                   break;

                 case GAN_ALPHA_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_uc(source,r,0),
                          dpix = gan_image_get_pixptr_rgba_uc(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->A = spix->R;
                   break;

                default:
                  gan_err_flush_trace();
                  gan_err_register("insert_rgba_into_rgba", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
                  return GAN_FALSE;
             }
             break;

            case GAN_GREEN_CHANNEL:
              switch(dchannel)
              {
                 case GAN_RED_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_uc(source,r,0),
                          dpix = gan_image_get_pixptr_rgba_uc(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->R = spix->G;
                   break;

                 case GAN_GREEN_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_uc(source,r,0),
                          dpix = gan_image_get_pixptr_rgba_uc(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->G = spix->G;
                   break;

                 case GAN_BLUE_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_uc(source,r,0),
                          dpix = gan_image_get_pixptr_rgba_uc(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->B = spix->G;
                   break;

                 case GAN_ALPHA_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_uc(source,r,0),
                          dpix = gan_image_get_pixptr_rgba_uc(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->A = spix->G;
                   break;

                default:
                  gan_err_flush_trace();
                  gan_err_register("insert_rgba_into_rgba", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
                  return GAN_FALSE;
             }
             break;

            case GAN_BLUE_CHANNEL:
              switch(dchannel)
              {
                 case GAN_RED_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_uc(source,r,0),
                          dpix = gan_image_get_pixptr_rgba_uc(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->R = spix->B;
                   break;

                 case GAN_GREEN_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_uc(source,r,0),
                          dpix = gan_image_get_pixptr_rgba_uc(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->G = spix->B;
                   break;

                 case GAN_BLUE_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_uc(source,r,0),
                          dpix = gan_image_get_pixptr_rgba_uc(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->B = spix->B;
                   break;

                 case GAN_ALPHA_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_uc(source,r,0),
                          dpix = gan_image_get_pixptr_rgba_uc(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->A = spix->B;
                   break;

                default:
                  gan_err_flush_trace();
                  gan_err_register("insert_rgba_into_rgba", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
                  return GAN_FALSE;
             }
             break;

            case GAN_ALPHA_CHANNEL:
              switch(dchannel)
              {
                 case GAN_RED_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_uc(source,r,0),
                          dpix = gan_image_get_pixptr_rgba_uc(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->R = spix->A;
                   break;

                 case GAN_GREEN_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_uc(source,r,0),
                          dpix = gan_image_get_pixptr_rgba_uc(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->G = spix->A;
                   break;

                 case GAN_BLUE_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_uc(source,r,0),
                          dpix = gan_image_get_pixptr_rgba_uc(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->B = spix->A;
                   break;

                 case GAN_ALPHA_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_uc(source,r,0),
                          dpix = gan_image_get_pixptr_rgba_uc(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->A = spix->A;
                   break;

                default:
                  gan_err_flush_trace();
                  gan_err_register("insert_rgba_into_rgba", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
                  return GAN_FALSE;
             }
             break;

           default:
             gan_err_flush_trace();
             gan_err_register("insert_rgba_into_rgba", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
             return GAN_FALSE;
         }
         break;
      }
      break;

      case GAN_USHORT:
      {
         Gan_RGBAPixel_us *spix;
         Gan_RGBAPixel_us *dpix;

         switch(schannel)
         {
            case GAN_RED_CHANNEL:
              switch(dchannel)
              {
                 case GAN_RED_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_us(source,r,0),
                          dpix = gan_image_get_pixptr_rgba_us(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->R = spix->R;
                   break;

                 case GAN_GREEN_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_us(source,r,0),
                          dpix = gan_image_get_pixptr_rgba_us(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->G = spix->R;
                   break;

                 case GAN_BLUE_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_us(source,r,0),
                          dpix = gan_image_get_pixptr_rgba_us(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->B = spix->R;
                   break;

                 case GAN_ALPHA_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_us(source,r,0),
                          dpix = gan_image_get_pixptr_rgba_us(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->A = spix->R;
                   break;

                default:
                  gan_err_flush_trace();
                  gan_err_register("insert_rgba_into_rgba", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
                  return GAN_FALSE;
             }
             break;

            case GAN_GREEN_CHANNEL:
              switch(dchannel)
              {
                 case GAN_RED_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_us(source,r,0),
                          dpix = gan_image_get_pixptr_rgba_us(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->R = spix->G;
                   break;

                 case GAN_GREEN_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_us(source,r,0),
                          dpix = gan_image_get_pixptr_rgba_us(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->G = spix->G;
                   break;

                 case GAN_BLUE_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_us(source,r,0),
                          dpix = gan_image_get_pixptr_rgba_us(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->B = spix->G;
                   break;

                 case GAN_ALPHA_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_us(source,r,0),
                          dpix = gan_image_get_pixptr_rgba_us(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->A = spix->G;
                   break;

                default:
                  gan_err_flush_trace();
                  gan_err_register("insert_rgba_into_rgba", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
                  return GAN_FALSE;
             }
             break;

            case GAN_BLUE_CHANNEL:
              switch(dchannel)
              {
                 case GAN_RED_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_us(source,r,0),
                          dpix = gan_image_get_pixptr_rgba_us(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->R = spix->B;
                   break;

                 case GAN_GREEN_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_us(source,r,0),
                          dpix = gan_image_get_pixptr_rgba_us(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->G = spix->B;
                   break;

                 case GAN_BLUE_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_us(source,r,0),
                          dpix = gan_image_get_pixptr_rgba_us(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->B = spix->B;
                   break;

                 case GAN_ALPHA_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_us(source,r,0),
                          dpix = gan_image_get_pixptr_rgba_us(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->A = spix->B;
                   break;

                default:
                  gan_err_flush_trace();
                  gan_err_register("insert_rgba_into_rgba", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
                  return GAN_FALSE;
             }
             break;

            case GAN_ALPHA_CHANNEL:
              switch(dchannel)
              {
                 case GAN_RED_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_us(source,r,0),
                          dpix = gan_image_get_pixptr_rgba_us(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->R = spix->A;
                   break;

                 case GAN_GREEN_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_us(source,r,0),
                          dpix = gan_image_get_pixptr_rgba_us(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->G = spix->A;
                   break;

                 case GAN_BLUE_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_us(source,r,0),
                          dpix = gan_image_get_pixptr_rgba_us(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->B = spix->A;
                   break;

                 case GAN_ALPHA_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_us(source,r,0),
                          dpix = gan_image_get_pixptr_rgba_us(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->A = spix->A;
                   break;

                default:
                  gan_err_flush_trace();
                  gan_err_register("insert_rgba_into_rgba", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
                  return GAN_FALSE;
             }
             break;

           default:
             gan_err_flush_trace();
             gan_err_register("insert_rgba_into_rgba", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
             return GAN_FALSE;
         }
         break;
      }
      break;
      
      case GAN_FLOAT:
      {
         Gan_RGBAPixel_f *spix;
         Gan_RGBAPixel_f *dpix;

         switch(schannel)
         {
            case GAN_RED_CHANNEL:
              switch(dchannel)
              {
                 case GAN_RED_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_f(source,r,0),
                          dpix = gan_image_get_pixptr_rgba_f(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->R = spix->R;
                   break;

                 case GAN_GREEN_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_f(source,r,0),
                          dpix = gan_image_get_pixptr_rgba_f(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->G = spix->R;
                   break;

                 case GAN_BLUE_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_f(source,r,0),
                          dpix = gan_image_get_pixptr_rgba_f(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->B = spix->R;
                   break;

                 case GAN_ALPHA_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_f(source,r,0),
                          dpix = gan_image_get_pixptr_rgba_f(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->A = spix->R;
                   break;

                default:
                  gan_err_flush_trace();
                  gan_err_register("insert_rgba_into_rgba", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
                  return GAN_FALSE;
             }
             break;

            case GAN_GREEN_CHANNEL:
              switch(dchannel)
              {
                 case GAN_RED_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_f(source,r,0),
                          dpix = gan_image_get_pixptr_rgba_f(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->R = spix->G;
                   break;

                 case GAN_GREEN_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_f(source,r,0),
                          dpix = gan_image_get_pixptr_rgba_f(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->G = spix->G;
                   break;

                 case GAN_BLUE_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_f(source,r,0),
                          dpix = gan_image_get_pixptr_rgba_f(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->B = spix->G;
                   break;

                 case GAN_ALPHA_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_f(source,r,0),
                          dpix = gan_image_get_pixptr_rgba_f(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->A = spix->G;
                   break;

                default:
                  gan_err_flush_trace();
                  gan_err_register("insert_rgba_into_rgba", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
                  return GAN_FALSE;
             }
             break;

            case GAN_BLUE_CHANNEL:
              switch(dchannel)
              {
                 case GAN_RED_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_f(source,r,0),
                          dpix = gan_image_get_pixptr_rgba_f(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->R = spix->B;
                   break;

                 case GAN_GREEN_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_f(source,r,0),
                          dpix = gan_image_get_pixptr_rgba_f(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->G = spix->B;
                   break;

                 case GAN_BLUE_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_f(source,r,0),
                          dpix = gan_image_get_pixptr_rgba_f(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->B = spix->B;
                   break;

                 case GAN_ALPHA_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_f(source,r,0),
                          dpix = gan_image_get_pixptr_rgba_f(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->A = spix->B;
                   break;

                default:
                  gan_err_flush_trace();
                  gan_err_register("insert_rgba_into_rgba", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
                  return GAN_FALSE;
             }
             break;

            case GAN_ALPHA_CHANNEL:
              switch(dchannel)
              {
                 case GAN_RED_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_f(source,r,0),
                          dpix = gan_image_get_pixptr_rgba_f(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->R = spix->A;
                   break;

                 case GAN_GREEN_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_f(source,r,0),
                          dpix = gan_image_get_pixptr_rgba_f(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->G = spix->A;
                   break;

                 case GAN_BLUE_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_f(source,r,0),
                          dpix = gan_image_get_pixptr_rgba_f(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->B = spix->A;
                   break;

                 case GAN_ALPHA_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_rgba_f(source,r,0),
                          dpix = gan_image_get_pixptr_rgba_f(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->A = spix->A;
                   break;

                default:
                  gan_err_flush_trace();
                  gan_err_register("insert_rgba_into_rgba", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
                  return GAN_FALSE;
             }
             break;

           default:
             gan_err_flush_trace();
             gan_err_register("insert_rgba_into_rgba", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
             return GAN_FALSE;
         }
         break;
      }
      break;
      
      default:
        gan_err_flush_trace();
        gan_err_register("insert_rgba_into_rgba", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "");
        return GAN_FALSE;
   }

   return GAN_TRUE;
}

static Gan_Bool insert_gl_into_bgr ( const Gan_Image *source,
                                     Gan_ImageChannelType schannel,
                                     Gan_Image *dest,
                                     Gan_ImageChannelType dchannel )
{
   int r, c;

   switch(source->type)
   {
      case GAN_UCHAR:
      {
         unsigned char *spix;
         Gan_BGRPixel_uc *dpix;

         switch(schannel)
         {
            case GAN_INTENSITY_CHANNEL:
              switch(dchannel)
              {
                 case GAN_RED_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_gl_uc(source,r,0),
                          dpix = gan_image_get_pixptr_bgr_uc(dest,r,0); c>=0; c--, dpix++)
                         dpix->R = *spix++;
                   break;

                 case GAN_GREEN_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_gl_uc(source,r,0),
                          dpix = gan_image_get_pixptr_bgr_uc(dest,r,0); c>=0; c--, dpix++)
                         dpix->G = *spix++;
                   break;

                 case GAN_BLUE_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_gl_uc(source,r,0),
                          dpix = gan_image_get_pixptr_bgr_uc(dest,r,0); c>=0; c--, dpix++)
                         dpix->B = *spix++;
                   break;

                default:
                  gan_err_flush_trace();
                  gan_err_register("insert_gl_into_bgr", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
                  return GAN_FALSE;
             }
             break;

           default:
             gan_err_flush_trace();
             gan_err_register("insert_gl_into_bgr", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
             return GAN_FALSE;
         }
         break;
      }
      
      case GAN_USHORT:
      {
         unsigned short *spix;
         Gan_BGRPixel_us *dpix;

         switch(schannel)
         {
            case GAN_INTENSITY_CHANNEL:
              switch(dchannel)
              {
                 case GAN_RED_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_gl_us(source,r,0),
                          dpix = gan_image_get_pixptr_bgr_us(dest,r,0); c>=0; c--, dpix++)
                         dpix->R = *spix++;
                   break;

                 case GAN_GREEN_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_gl_us(source,r,0),
                          dpix = gan_image_get_pixptr_bgr_us(dest,r,0); c>=0; c--, dpix++)
                         dpix->G = *spix++;
                   break;

                 case GAN_BLUE_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_gl_us(source,r,0),
                          dpix = gan_image_get_pixptr_bgr_us(dest,r,0); c>=0; c--, dpix++)
                         dpix->B = *spix++;
                   break;

                default:
                  gan_err_flush_trace();
                  gan_err_register("insert_gl_into_bgr", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
                  return GAN_FALSE;
             }
             break;

           default:
             gan_err_flush_trace();
             gan_err_register("insert_gl_into_bgr", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
             return GAN_FALSE;
         }
         break;
      }
      
      case GAN_FLOAT:
      {
         float *spix;
         Gan_BGRPixel_f *dpix;

         switch(schannel)
         {
            case GAN_INTENSITY_CHANNEL:
              switch(dchannel)
              {
                 case GAN_RED_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_gl_f(source,r,0),
                          dpix = gan_image_get_pixptr_bgr_f(dest,r,0); c>=0; c--, dpix++)
                         dpix->R = *spix++;
                   break;

                 case GAN_GREEN_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_gl_f(source,r,0),
                          dpix = gan_image_get_pixptr_bgr_f(dest,r,0); c>=0; c--, dpix++)
                         dpix->G = *spix++;
                   break;

                 case GAN_BLUE_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_gl_f(source,r,0),
                          dpix = gan_image_get_pixptr_bgr_f(dest,r,0); c>=0; c--, dpix++)
                         dpix->B = *spix++;
                   break;

                default:
                  gan_err_flush_trace();
                  gan_err_register("insert_gl_into_bgr", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
                  return GAN_FALSE;
             }
             break;

           default:
             gan_err_flush_trace();
             gan_err_register("insert_gl_into_bgr", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
             return GAN_FALSE;
         }
         break;
      }
      
      default:
        gan_err_flush_trace();
        gan_err_register("insert_gl_into_bgr", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "");
        return GAN_FALSE;
   }

   return GAN_TRUE;
}

static Gan_Bool insert_gl_into_bgra ( const Gan_Image *source,
                                      Gan_ImageChannelType schannel,
                                      Gan_Image *dest,
                                      Gan_ImageChannelType dchannel )
{
   int r, c;

   switch(source->type)
   {
      case GAN_UCHAR:
      {
         unsigned char *spix;
         Gan_BGRAPixel_uc *dpix;

         switch(schannel)
         {
            case GAN_INTENSITY_CHANNEL:
              switch(dchannel)
              {
                 case GAN_RED_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_gl_uc(source,r,0),
                          dpix = gan_image_get_pixptr_bgra_uc(dest,r,0); c>=0; c--, dpix++)
                         dpix->R = *spix++;
                   break;

                 case GAN_GREEN_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_gl_uc(source,r,0),
                          dpix = gan_image_get_pixptr_bgra_uc(dest,r,0); c>=0; c--, dpix++)
                         dpix->G = *spix++;
                   break;

                 case GAN_BLUE_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_gl_uc(source,r,0),
                          dpix = gan_image_get_pixptr_bgra_uc(dest,r,0); c>=0; c--, dpix++)
                         dpix->B = *spix++;
                   break;

                 case GAN_ALPHA_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_gl_uc(source,r,0),
                          dpix = gan_image_get_pixptr_bgra_uc(dest,r,0); c>=0; c--, dpix++)
                         dpix->A = *spix++;
                   break;

                default:
                  gan_err_flush_trace();
                  gan_err_register("insert_gl_into_bgra", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
                  return GAN_FALSE;
             }
             break;

           default:
             gan_err_flush_trace();
             gan_err_register("insert_gl_into_bgra", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
             return GAN_FALSE;
         }
         break;
      }
      
      case GAN_USHORT:
      {
         unsigned short *spix;
         Gan_BGRAPixel_us *dpix;

         switch(schannel)
         {
            case GAN_INTENSITY_CHANNEL:
              switch(dchannel)
              {
                 case GAN_RED_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_gl_us(source,r,0),
                          dpix = gan_image_get_pixptr_bgra_us(dest,r,0); c>=0; c--, dpix++)
                         dpix->R = *spix++;
                   break;

                 case GAN_GREEN_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_gl_us(source,r,0),
                          dpix = gan_image_get_pixptr_bgra_us(dest,r,0); c>=0; c--, dpix++)
                         dpix->G = *spix++;
                   break;

                 case GAN_BLUE_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_gl_us(source,r,0),
                          dpix = gan_image_get_pixptr_bgra_us(dest,r,0); c>=0; c--, dpix++)
                         dpix->B = *spix++;
                   break;

                 case GAN_ALPHA_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_gl_us(source,r,0),
                          dpix = gan_image_get_pixptr_bgra_us(dest,r,0); c>=0; c--, dpix++)
                         dpix->A = *spix++;
                   break;

                default:
                  gan_err_flush_trace();
                  gan_err_register("insert_gl_into_bgra", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
                  return GAN_FALSE;
             }
             break;

           default:
             gan_err_flush_trace();
             gan_err_register("insert_gl_into_bgra", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
             return GAN_FALSE;
         }
         break;
      }
      
      case GAN_FLOAT:
      {
         float *spix;
         Gan_BGRAPixel_f *dpix;

         switch(schannel)
         {
            case GAN_INTENSITY_CHANNEL:
              switch(dchannel)
              {
                 case GAN_RED_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_gl_f(source,r,0),
                          dpix = gan_image_get_pixptr_bgra_f(dest,r,0); c>=0; c--, dpix++)
                         dpix->R = *spix++;
                   break;

                 case GAN_GREEN_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_gl_f(source,r,0),
                          dpix = gan_image_get_pixptr_bgra_f(dest,r,0); c>=0; c--, dpix++)
                         dpix->G = *spix++;
                   break;

                 case GAN_BLUE_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_gl_f(source,r,0),
                          dpix = gan_image_get_pixptr_bgra_f(dest,r,0); c>=0; c--, dpix++)
                         dpix->B = *spix++;
                   break;

                 case GAN_ALPHA_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_gl_f(source,r,0),
                          dpix = gan_image_get_pixptr_bgra_f(dest,r,0); c>=0; c--, dpix++)
                         dpix->A = *spix++;
                   break;

                default:
                  gan_err_flush_trace();
                  gan_err_register("insert_gl_into_bgra", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
                  return GAN_FALSE;
             }
             break;

           default:
             gan_err_flush_trace();
             gan_err_register("insert_gl_into_bgra", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
             return GAN_FALSE;
         }
         break;
      }
      
      default:
        gan_err_flush_trace();
        gan_err_register("insert_gl_into_bgra", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "");
        return GAN_FALSE;
   }

   return GAN_TRUE;
}

static Gan_Bool insert_bgra_into_bgr ( const Gan_Image *source,
                                       Gan_ImageChannelType schannel,
                                       Gan_Image *dest,
                                       Gan_ImageChannelType dchannel )
{
   int r, c;

   switch(source->type)
   {
      case GAN_UCHAR:
      {
         Gan_BGRAPixel_uc *spix;
         Gan_BGRPixel_uc *dpix;

         switch(schannel)
         {
            case GAN_RED_CHANNEL:
              switch(dchannel)
              {
                 case GAN_RED_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_uc(source,r,0),
                          dpix = gan_image_get_pixptr_bgr_uc(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->R = spix->R;
                   break;

                 case GAN_GREEN_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_uc(source,r,0),
                          dpix = gan_image_get_pixptr_bgr_uc(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->G = spix->R;
                   break;

                 case GAN_BLUE_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_uc(source,r,0),
                          dpix = gan_image_get_pixptr_bgr_uc(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->B = spix->R;
                   break;

                default:
                  gan_err_flush_trace();
                  gan_err_register("insert_bgra_into_bgr", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
                  return GAN_FALSE;
             }
             break;

            case GAN_GREEN_CHANNEL:
              switch(dchannel)
              {
                 case GAN_RED_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_uc(source,r,0),
                          dpix = gan_image_get_pixptr_bgr_uc(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->R = spix->G;
                   break;

                 case GAN_GREEN_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_uc(source,r,0),
                          dpix = gan_image_get_pixptr_bgr_uc(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->G = spix->G;
                   break;

                 case GAN_BLUE_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_uc(source,r,0),
                          dpix = gan_image_get_pixptr_bgr_uc(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->B = spix->G;
                   break;

                default:
                  gan_err_flush_trace();
                  gan_err_register("insert_bgra_into_bgr", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
                  return GAN_FALSE;
             }
             break;

            case GAN_BLUE_CHANNEL:
              switch(dchannel)
              {
                 case GAN_RED_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_uc(source,r,0),
                          dpix = gan_image_get_pixptr_bgr_uc(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->R = spix->B;
                   break;

                 case GAN_GREEN_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_uc(source,r,0),
                          dpix = gan_image_get_pixptr_bgr_uc(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->G = spix->B;
                   break;

                 case GAN_BLUE_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_uc(source,r,0),
                          dpix = gan_image_get_pixptr_bgr_uc(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->B = spix->B;
                   break;

                default:
                  gan_err_flush_trace();
                  gan_err_register("insert_bgra_into_bgr", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
                  return GAN_FALSE;
             }
             break;

            case GAN_ALPHA_CHANNEL:
              switch(dchannel)
              {
                 case GAN_RED_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_uc(source,r,0),
                          dpix = gan_image_get_pixptr_bgr_uc(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->R = spix->A;
                   break;

                 case GAN_GREEN_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_uc(source,r,0),
                          dpix = gan_image_get_pixptr_bgr_uc(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->G = spix->A;
                   break;

                 case GAN_BLUE_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_uc(source,r,0),
                          dpix = gan_image_get_pixptr_bgr_uc(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->B = spix->A;
                   break;

                default:
                  gan_err_flush_trace();
                  gan_err_register("insert_bgra_into_bgr", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
                  return GAN_FALSE;
             }
             break;

           default:
             gan_err_flush_trace();
             gan_err_register("insert_bgra_into_bgr", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
             return GAN_FALSE;
         }
         break;
      }
      
      case GAN_USHORT:
      {
         Gan_BGRAPixel_us *spix;
         Gan_BGRPixel_us *dpix;

         switch(schannel)
         {
            case GAN_RED_CHANNEL:
              switch(dchannel)
              {
                 case GAN_RED_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_us(source,r,0),
                          dpix = gan_image_get_pixptr_bgr_us(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->R = spix->R;
                   break;

                 case GAN_GREEN_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_us(source,r,0),
                          dpix = gan_image_get_pixptr_bgr_us(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->G = spix->R;
                   break;

                 case GAN_BLUE_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_us(source,r,0),
                          dpix = gan_image_get_pixptr_bgr_us(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->B = spix->R;
                   break;

                default:
                  gan_err_flush_trace();
                  gan_err_register("insert_bgra_into_bgr", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
                  return GAN_FALSE;
             }
             break;

            case GAN_GREEN_CHANNEL:
              switch(dchannel)
              {
                 case GAN_RED_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_us(source,r,0),
                          dpix = gan_image_get_pixptr_bgr_us(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->R = spix->G;
                   break;

                 case GAN_GREEN_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_us(source,r,0),
                          dpix = gan_image_get_pixptr_bgr_us(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->G = spix->G;
                   break;

                 case GAN_BLUE_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_us(source,r,0),
                          dpix = gan_image_get_pixptr_bgr_us(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->B = spix->G;
                   break;

                default:
                  gan_err_flush_trace();
                  gan_err_register("insert_bgra_into_bgr", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
                  return GAN_FALSE;
             }
             break;

            case GAN_BLUE_CHANNEL:
              switch(dchannel)
              {
                 case GAN_RED_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_us(source,r,0),
                          dpix = gan_image_get_pixptr_bgr_us(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->R = spix->B;
                   break;

                 case GAN_GREEN_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_us(source,r,0),
                          dpix = gan_image_get_pixptr_bgr_us(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->G = spix->B;
                   break;

                 case GAN_BLUE_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_us(source,r,0),
                          dpix = gan_image_get_pixptr_bgr_us(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->B = spix->B;
                   break;

                default:
                  gan_err_flush_trace();
                  gan_err_register("insert_bgra_into_bgr", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
                  return GAN_FALSE;
             }
             break;

            case GAN_ALPHA_CHANNEL:
              switch(dchannel)
              {
                 case GAN_RED_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_us(source,r,0),
                          dpix = gan_image_get_pixptr_bgr_us(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->R = spix->A;
                   break;

                 case GAN_GREEN_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_us(source,r,0),
                          dpix = gan_image_get_pixptr_bgr_us(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->G = spix->A;
                   break;

                 case GAN_BLUE_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_us(source,r,0),
                          dpix = gan_image_get_pixptr_bgr_us(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->B = spix->A;
                   break;

                default:
                  gan_err_flush_trace();
                  gan_err_register("insert_bgra_into_bgr", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
                  return GAN_FALSE;
             }
             break;

           default:
             gan_err_flush_trace();
             gan_err_register("insert_bgra_into_bgr", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
             return GAN_FALSE;
         }
         break;
      }
      
      case GAN_FLOAT:
      {
         Gan_BGRAPixel_f *spix;
         Gan_BGRPixel_f *dpix;

         switch(schannel)
         {
            case GAN_RED_CHANNEL:
              switch(dchannel)
              {
                 case GAN_RED_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_f(source,r,0),
                          dpix = gan_image_get_pixptr_bgr_f(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->R = spix->R;
                   break;

                 case GAN_GREEN_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_f(source,r,0),
                          dpix = gan_image_get_pixptr_bgr_f(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->G = spix->R;
                   break;

                 case GAN_BLUE_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_f(source,r,0),
                          dpix = gan_image_get_pixptr_bgr_f(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->B = spix->R;
                   break;

                default:
                  gan_err_flush_trace();
                  gan_err_register("insert_bgra_into_bgr", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
                  return GAN_FALSE;
             }
             break;

            case GAN_GREEN_CHANNEL:
              switch(dchannel)
              {
                 case GAN_RED_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_f(source,r,0),
                          dpix = gan_image_get_pixptr_bgr_f(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->R = spix->G;
                   break;

                 case GAN_GREEN_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_f(source,r,0),
                          dpix = gan_image_get_pixptr_bgr_f(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->G = spix->G;
                   break;

                 case GAN_BLUE_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_f(source,r,0),
                          dpix = gan_image_get_pixptr_bgr_f(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->B = spix->G;
                   break;

                default:
                  gan_err_flush_trace();
                  gan_err_register("insert_bgra_into_bgr", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
                  return GAN_FALSE;
             }
             break;

            case GAN_BLUE_CHANNEL:
              switch(dchannel)
              {
                 case GAN_RED_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_f(source,r,0),
                          dpix = gan_image_get_pixptr_bgr_f(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->R = spix->B;
                   break;

                 case GAN_GREEN_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_f(source,r,0),
                          dpix = gan_image_get_pixptr_bgr_f(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->G = spix->B;
                   break;

                 case GAN_BLUE_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_f(source,r,0),
                          dpix = gan_image_get_pixptr_bgr_f(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->B = spix->B;
                   break;

                default:
                  gan_err_flush_trace();
                  gan_err_register("insert_bgra_into_bgr", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
                  return GAN_FALSE;
             }
             break;

            case GAN_ALPHA_CHANNEL:
              switch(dchannel)
              {
                 case GAN_RED_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_f(source,r,0),
                          dpix = gan_image_get_pixptr_bgr_f(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->R = spix->A;
                   break;

                 case GAN_GREEN_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_f(source,r,0),
                          dpix = gan_image_get_pixptr_bgr_f(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->G = spix->A;
                   break;

                 case GAN_BLUE_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_f(source,r,0),
                          dpix = gan_image_get_pixptr_bgr_f(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->B = spix->A;
                   break;

                default:
                  gan_err_flush_trace();
                  gan_err_register("insert_bgra_into_bgr", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
                  return GAN_FALSE;
             }
             break;

           default:
             gan_err_flush_trace();
             gan_err_register("insert_bgra_into_bgr", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
             return GAN_FALSE;
         }
         break;
      }
      
      default:
        gan_err_flush_trace();
        gan_err_register("insert_bgra_into_bgr", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "");
        return GAN_FALSE;
   }

   return GAN_TRUE;
}

static Gan_Bool insert_bgra_into_bgra ( const Gan_Image *source,
                                      Gan_ImageChannelType schannel,
                                      Gan_Image *dest,
                                      Gan_ImageChannelType dchannel )
{
   int r, c;

   switch(source->type)
   {
      case GAN_UCHAR:
      {
         Gan_BGRAPixel_uc *spix;
         Gan_BGRAPixel_uc *dpix;

         switch(schannel)
         {
            case GAN_RED_CHANNEL:
              switch(dchannel)
              {
                 case GAN_RED_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_uc(source,r,0),
                          dpix = gan_image_get_pixptr_bgra_uc(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->R = spix->R;
                   break;

                 case GAN_GREEN_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_uc(source,r,0),
                          dpix = gan_image_get_pixptr_bgra_uc(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->G = spix->R;
                   break;

                 case GAN_BLUE_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_uc(source,r,0),
                          dpix = gan_image_get_pixptr_bgra_uc(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->B = spix->R;
                   break;

                 case GAN_ALPHA_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_uc(source,r,0),
                          dpix = gan_image_get_pixptr_bgra_uc(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->A = spix->R;
                   break;

                default:
                  gan_err_flush_trace();
                  gan_err_register("insert_bgra_into_bgra", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
                  return GAN_FALSE;
             }
             break;

            case GAN_GREEN_CHANNEL:
              switch(dchannel)
              {
                 case GAN_RED_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_uc(source,r,0),
                          dpix = gan_image_get_pixptr_bgra_uc(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->R = spix->G;
                   break;

                 case GAN_GREEN_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_uc(source,r,0),
                          dpix = gan_image_get_pixptr_bgra_uc(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->G = spix->G;
                   break;

                 case GAN_BLUE_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_uc(source,r,0),
                          dpix = gan_image_get_pixptr_bgra_uc(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->B = spix->G;
                   break;

                 case GAN_ALPHA_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_uc(source,r,0),
                          dpix = gan_image_get_pixptr_bgra_uc(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->A = spix->G;
                   break;

                default:
                  gan_err_flush_trace();
                  gan_err_register("insert_bgra_into_bgra", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
                  return GAN_FALSE;
             }
             break;

            case GAN_BLUE_CHANNEL:
              switch(dchannel)
              {
                 case GAN_RED_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_uc(source,r,0),
                          dpix = gan_image_get_pixptr_bgra_uc(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->R = spix->B;
                   break;

                 case GAN_GREEN_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_uc(source,r,0),
                          dpix = gan_image_get_pixptr_bgra_uc(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->G = spix->B;
                   break;

                 case GAN_BLUE_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_uc(source,r,0),
                          dpix = gan_image_get_pixptr_bgra_uc(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->B = spix->B;
                   break;

                 case GAN_ALPHA_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_uc(source,r,0),
                          dpix = gan_image_get_pixptr_bgra_uc(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->A = spix->B;
                   break;

                default:
                  gan_err_flush_trace();
                  gan_err_register("insert_bgra_into_bgra", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
                  return GAN_FALSE;
             }
             break;

            case GAN_ALPHA_CHANNEL:
              switch(dchannel)
              {
                 case GAN_RED_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_uc(source,r,0),
                          dpix = gan_image_get_pixptr_bgra_uc(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->R = spix->A;
                   break;

                 case GAN_GREEN_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_uc(source,r,0),
                          dpix = gan_image_get_pixptr_bgra_uc(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->G = spix->A;
                   break;

                 case GAN_BLUE_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_uc(source,r,0),
                          dpix = gan_image_get_pixptr_bgra_uc(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->B = spix->A;
                   break;

                 case GAN_ALPHA_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_uc(source,r,0),
                          dpix = gan_image_get_pixptr_bgra_uc(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->A = spix->A;
                   break;

                default:
                  gan_err_flush_trace();
                  gan_err_register("insert_bgra_into_bgra", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
                  return GAN_FALSE;
             }
             break;

           default:
             gan_err_flush_trace();
             gan_err_register("insert_bgra_into_bgra", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
             return GAN_FALSE;
         }
         break;
      }
      break;

      case GAN_USHORT:
      {
         Gan_BGRAPixel_us *spix;
         Gan_BGRAPixel_us *dpix;

         switch(schannel)
         {
            case GAN_RED_CHANNEL:
              switch(dchannel)
              {
                 case GAN_RED_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_us(source,r,0),
                          dpix = gan_image_get_pixptr_bgra_us(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->R = spix->R;
                   break;

                 case GAN_GREEN_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_us(source,r,0),
                          dpix = gan_image_get_pixptr_bgra_us(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->G = spix->R;
                   break;

                 case GAN_BLUE_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_us(source,r,0),
                          dpix = gan_image_get_pixptr_bgra_us(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->B = spix->R;
                   break;

                 case GAN_ALPHA_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_us(source,r,0),
                          dpix = gan_image_get_pixptr_bgra_us(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->A = spix->R;
                   break;

                default:
                  gan_err_flush_trace();
                  gan_err_register("insert_bgra_into_bgra", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
                  return GAN_FALSE;
             }
             break;

            case GAN_GREEN_CHANNEL:
              switch(dchannel)
              {
                 case GAN_RED_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_us(source,r,0),
                          dpix = gan_image_get_pixptr_bgra_us(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->R = spix->G;
                   break;

                 case GAN_GREEN_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_us(source,r,0),
                          dpix = gan_image_get_pixptr_bgra_us(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->G = spix->G;
                   break;

                 case GAN_BLUE_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_us(source,r,0),
                          dpix = gan_image_get_pixptr_bgra_us(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->B = spix->G;
                   break;

                 case GAN_ALPHA_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_us(source,r,0),
                          dpix = gan_image_get_pixptr_bgra_us(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->A = spix->G;
                   break;

                default:
                  gan_err_flush_trace();
                  gan_err_register("insert_bgra_into_bgra", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
                  return GAN_FALSE;
             }
             break;

            case GAN_BLUE_CHANNEL:
              switch(dchannel)
              {
                 case GAN_RED_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_us(source,r,0),
                          dpix = gan_image_get_pixptr_bgra_us(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->R = spix->B;
                   break;

                 case GAN_GREEN_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_us(source,r,0),
                          dpix = gan_image_get_pixptr_bgra_us(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->G = spix->B;
                   break;

                 case GAN_BLUE_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_us(source,r,0),
                          dpix = gan_image_get_pixptr_bgra_us(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->B = spix->B;
                   break;

                 case GAN_ALPHA_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_us(source,r,0),
                          dpix = gan_image_get_pixptr_bgra_us(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->A = spix->B;
                   break;

                default:
                  gan_err_flush_trace();
                  gan_err_register("insert_bgra_into_bgra", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
                  return GAN_FALSE;
             }
             break;

            case GAN_ALPHA_CHANNEL:
              switch(dchannel)
              {
                 case GAN_RED_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_us(source,r,0),
                          dpix = gan_image_get_pixptr_bgra_us(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->R = spix->A;
                   break;

                 case GAN_GREEN_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_us(source,r,0),
                          dpix = gan_image_get_pixptr_bgra_us(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->G = spix->A;
                   break;

                 case GAN_BLUE_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_us(source,r,0),
                          dpix = gan_image_get_pixptr_bgra_us(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->B = spix->A;
                   break;

                 case GAN_ALPHA_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_us(source,r,0),
                          dpix = gan_image_get_pixptr_bgra_us(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->A = spix->A;
                   break;

                default:
                  gan_err_flush_trace();
                  gan_err_register("insert_bgra_into_bgra", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
                  return GAN_FALSE;
             }
             break;

           default:
             gan_err_flush_trace();
             gan_err_register("insert_bgra_into_bgra", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
             return GAN_FALSE;
         }
         break;
      }
      break;
      
      case GAN_FLOAT:
      {
         Gan_BGRAPixel_f *spix;
         Gan_BGRAPixel_f *dpix;

         switch(schannel)
         {
            case GAN_RED_CHANNEL:
              switch(dchannel)
              {
                 case GAN_RED_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_f(source,r,0),
                          dpix = gan_image_get_pixptr_bgra_f(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->R = spix->R;
                   break;

                 case GAN_GREEN_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_f(source,r,0),
                          dpix = gan_image_get_pixptr_bgra_f(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->G = spix->R;
                   break;

                 case GAN_BLUE_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_f(source,r,0),
                          dpix = gan_image_get_pixptr_bgra_f(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->B = spix->R;
                   break;

                 case GAN_ALPHA_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_f(source,r,0),
                          dpix = gan_image_get_pixptr_bgra_f(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->A = spix->R;
                   break;

                default:
                  gan_err_flush_trace();
                  gan_err_register("insert_bgra_into_bgra", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
                  return GAN_FALSE;
             }
             break;

            case GAN_GREEN_CHANNEL:
              switch(dchannel)
              {
                 case GAN_RED_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_f(source,r,0),
                          dpix = gan_image_get_pixptr_bgra_f(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->R = spix->G;
                   break;

                 case GAN_GREEN_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_f(source,r,0),
                          dpix = gan_image_get_pixptr_bgra_f(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->G = spix->G;
                   break;

                 case GAN_BLUE_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_f(source,r,0),
                          dpix = gan_image_get_pixptr_bgra_f(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->B = spix->G;
                   break;

                 case GAN_ALPHA_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_f(source,r,0),
                          dpix = gan_image_get_pixptr_bgra_f(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->A = spix->G;
                   break;

                default:
                  gan_err_flush_trace();
                  gan_err_register("insert_bgra_into_bgra", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
                  return GAN_FALSE;
             }
             break;

            case GAN_BLUE_CHANNEL:
              switch(dchannel)
              {
                 case GAN_RED_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_f(source,r,0),
                          dpix = gan_image_get_pixptr_bgra_f(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->R = spix->B;
                   break;

                 case GAN_GREEN_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_f(source,r,0),
                          dpix = gan_image_get_pixptr_bgra_f(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->G = spix->B;
                   break;

                 case GAN_BLUE_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_f(source,r,0),
                          dpix = gan_image_get_pixptr_bgra_f(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->B = spix->B;
                   break;

                 case GAN_ALPHA_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_f(source,r,0),
                          dpix = gan_image_get_pixptr_bgra_f(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->A = spix->B;
                   break;

                default:
                  gan_err_flush_trace();
                  gan_err_register("insert_bgra_into_bgra", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
                  return GAN_FALSE;
             }
             break;

            case GAN_ALPHA_CHANNEL:
              switch(dchannel)
              {
                 case GAN_RED_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_f(source,r,0),
                          dpix = gan_image_get_pixptr_bgra_f(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->R = spix->A;
                   break;

                 case GAN_GREEN_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_f(source,r,0),
                          dpix = gan_image_get_pixptr_bgra_f(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->G = spix->A;
                   break;

                 case GAN_BLUE_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_f(source,r,0),
                          dpix = gan_image_get_pixptr_bgra_f(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->B = spix->A;
                   break;

                 case GAN_ALPHA_CHANNEL:
                   for(r=(int)source->height-1; r>=0; r--)
                      for(c=(int)source->width-1, spix = gan_image_get_pixptr_bgra_f(source,r,0),
                          dpix = gan_image_get_pixptr_bgra_f(dest,r,0); c>=0; c--, spix++, dpix++)
                         dpix->A = spix->A;
                   break;

                default:
                  gan_err_flush_trace();
                  gan_err_register("insert_bgra_into_bgra", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
                  return GAN_FALSE;
             }
             break;

           default:
             gan_err_flush_trace();
             gan_err_register("insert_bgra_into_bgra", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "");
             return GAN_FALSE;
         }
         break;
      }
      break;
      
      default:
        gan_err_flush_trace();
        gan_err_register("insert_bgra_into_bgra", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "");
        return GAN_FALSE;
   }

   return GAN_TRUE;
}

/**
 * \brief Inserts a specific channel from one image into another.
 * \param source The input image
 * \param schannel The channel in the input image
 * \param dest The output image
 * \param dchannel The channel in the output image
 * \return A pointer to the extracted image, or \c NULL on failure.
 *
 * Extracts a specific channel in a sub-part of an image,
 *
 * \sa gan_image_extract_channel_s().
 */
Gan_Bool gan_image_insert_channel ( const Gan_Image *source,
                                    Gan_ImageChannelType schannel,
                                    Gan_Image *dest,
                                    Gan_ImageChannelType dchannel )
{
   Gan_Bool result;

   if(source->type != dest->type || source->width != dest->width || source->height != dest->height)
   {
      gan_err_flush_trace();
      gan_err_register("gan_image_insert_channel", GAN_ERROR_INCOMPATIBLE, "");
      return GAN_FALSE;
   }

   switch(source->format)
   {
      case GAN_GREY_LEVEL_IMAGE:
        switch(dest->format)
        {
           case GAN_RGB_COLOUR_IMAGE:
             result = insert_gl_into_rgb(source, schannel, dest, dchannel);
             break;

           case GAN_RGB_COLOUR_ALPHA_IMAGE:
             result = insert_gl_into_rgba(source, schannel, dest, dchannel);
             break;

           case GAN_BGR_COLOUR_IMAGE:
             result = insert_gl_into_bgr(source, schannel, dest, dchannel);
             break;

           case GAN_BGR_COLOUR_ALPHA_IMAGE:
             result = insert_gl_into_bgra(source, schannel, dest, dchannel);
             break;

           default:
             gan_err_flush_trace();
             gan_err_register("gan_image_insert_channel", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "");
             return GAN_FALSE;
        }

        break;
        
      case GAN_RGB_COLOUR_ALPHA_IMAGE:
        switch(dest->format)
        {
           case GAN_RGB_COLOUR_IMAGE:
             result = insert_rgba_into_rgb(source, schannel, dest, dchannel);
             break;

           case GAN_RGB_COLOUR_ALPHA_IMAGE:
             result = insert_rgba_into_rgba(source, schannel, dest, dchannel);
             break;

           default:
             gan_err_flush_trace();
             gan_err_register("gan_image_insert_channel", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "");
             return GAN_FALSE;
        }

        break;
        
        
      case GAN_BGR_COLOUR_ALPHA_IMAGE:
        switch(dest->format)
        {
           case GAN_BGR_COLOUR_IMAGE:
             result = insert_bgra_into_bgr(source, schannel, dest, dchannel);
             break;

           case GAN_BGR_COLOUR_ALPHA_IMAGE:
             result = insert_bgra_into_bgra(source, schannel, dest, dchannel);
             break;

           default:
             gan_err_flush_trace();
             gan_err_register("gan_image_insert_channel", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "");
             return GAN_FALSE;
        }

        break;
        
      default:
        gan_err_flush_trace();
        gan_err_register("gan_image_insert_channel", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "");
        return GAN_FALSE;
   }
        
   return GAN_TRUE;
}

static Gan_Bool
 gla_fill_intensity ( Gan_Image *image, Gan_Pixel *pixel )
{
   int iRow, iCol;

   switch ( image->type )
   {
      case GAN_UCHAR:
      {
         unsigned char val = (pixel == NULL) ? 0 : pixel->data.gl.uc;
         Gan_GLAPixel_uc *pix;

         for ( iRow = (int)image->height-1; iRow >= 0; iRow-- )
            for ( pix = gan_image_get_pixptr_gla_uc(image,iRow,0),
                  iCol = (int)image->width-1; iCol >= 0; iCol-- )
               pix[iCol].I = val;
      }
      break;

      case GAN_USHORT:
      {
         unsigned short val = (pixel == NULL) ? 0 : pixel->data.gl.us;
         Gan_GLAPixel_us *pix;

         for ( iRow = (int)image->height-1; iRow >= 0; iRow-- )
            for ( pix = gan_image_get_pixptr_gla_us(image,iRow,0),
                  iCol = (int)image->width-1; iCol >= 0; iCol-- )
               pix[iCol].I = val;
      }
      break;

      case GAN_UINT:
      {
         unsigned int val = (pixel == NULL) ? 0 : pixel->data.gl.ui;
         Gan_GLAPixel_ui *pix;

         for ( iRow = (int)image->height-1; iRow >= 0; iRow-- )
            for ( pix = gan_image_get_pixptr_gla_ui(image,iRow,0),
                  iCol = (int)image->width-1; iCol >= 0; iCol-- )
               pix[iCol].I = val;
      }
      break;

      case GAN_FLOAT:
      {
         float val = (pixel == NULL) ? 0.0F : pixel->data.gl.f;
         Gan_GLAPixel_f *pix;

         for ( iRow = (int)image->height-1; iRow >= 0; iRow-- )
            for ( pix = gan_image_get_pixptr_gla_f(image,iRow,0),
                  iCol = (int)image->width-1; iCol >= 0; iCol-- )
               pix[iCol].I = val;
      }
      break;

      case GAN_DOUBLE:
      {
         double val = (pixel == NULL) ? 0.0 : pixel->data.gl.d;
         Gan_GLAPixel_d *pix;

         for ( iRow = (int)image->height-1; iRow >= 0; iRow-- )
            for ( pix = gan_image_get_pixptr_gla_d(image,iRow,0),
                  iCol = (int)image->width-1; iCol >= 0; iCol-- )
               pix[iCol].I = val;
      }
      break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gla_fill_intensity", GAN_ERROR_NOT_IMPLEMENTED,
                           "" );
        return GAN_FALSE;
   }

   /* success */
   return GAN_TRUE;
}

static Gan_Bool
 gla_fill_alpha ( Gan_Image *image, Gan_Pixel *pixel )
{
   int iRow, iCol;

   switch ( image->type )
   {
      case GAN_UCHAR:
      {
         unsigned char val = (pixel == NULL) ? 0 : pixel->data.gl.uc;
         Gan_GLAPixel_uc *pix;

         for ( iRow = (int)image->height-1; iRow >= 0; iRow-- )
            for ( pix = gan_image_get_pixptr_gla_uc(image,iRow,0),
                  iCol = (int)image->width-1; iCol >= 0; iCol-- )
               pix[iCol].A = val;
      }
      break;

      case GAN_USHORT:
      {
         unsigned short val = (pixel == NULL) ? 0 : pixel->data.gl.us;
         Gan_GLAPixel_us *pix;

         for ( iRow = (int)image->height-1; iRow >= 0; iRow-- )
            for ( pix = gan_image_get_pixptr_gla_us(image,iRow,0),
                  iCol = (int)image->width-1; iCol >= 0; iCol-- )
               pix[iCol].A = val;
      }
      break;

      case GAN_UINT:
      {
         unsigned int val = (pixel == NULL) ? 0 : pixel->data.gl.ui;
         Gan_GLAPixel_ui *pix;

         for ( iRow = (int)image->height-1; iRow >= 0; iRow-- )
            for ( pix = gan_image_get_pixptr_gla_ui(image,iRow,0),
                  iCol = (int)image->width-1; iCol >= 0; iCol-- )
               pix[iCol].A = val;
      }
      break;

      case GAN_FLOAT:
      {
         float val = (pixel == NULL) ? 0.0F : pixel->data.gl.f;
         Gan_GLAPixel_f *pix;

         for ( iRow = (int)image->height-1; iRow >= 0; iRow-- )
            for ( pix = gan_image_get_pixptr_gla_f(image,iRow,0),
                  iCol = (int)image->width-1; iCol >= 0; iCol-- )
               pix[iCol].A = val;
      }
      break;

      case GAN_DOUBLE:
      {
         double val = (pixel == NULL) ? 0.0 : pixel->data.gl.d;
         Gan_GLAPixel_d *pix;

         for ( iRow = (int)image->height-1; iRow >= 0; iRow-- )
            for ( pix = gan_image_get_pixptr_gla_d(image,iRow,0),
                  iCol = (int)image->width-1; iCol >= 0; iCol-- )
               pix[iCol].A = val;
      }
      break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gla_fill_alpha", GAN_ERROR_NOT_IMPLEMENTED,
                           "" );
        return GAN_FALSE;
   }

   /* success */
   return GAN_TRUE;
}

static Gan_Bool
 rgb_fill_red ( Gan_Image *image, Gan_Pixel *pixel )
{
   int iRow, iCol;

   switch ( image->type )
   {
      case GAN_UCHAR:
      {
         unsigned char val = (pixel == NULL) ? 0 : pixel->data.gl.uc;
         Gan_RGBPixel_uc *pix;

         for ( iRow = (int)image->height-1; iRow >= 0; iRow-- )
            for ( pix = gan_image_get_pixptr_rgb_uc(image,iRow,0),
                  iCol = (int)image->width-1; iCol >= 0; iCol-- )
               pix[iCol].R = val;
      }
      break;

      case GAN_USHORT:
      {
         unsigned short val = (pixel == NULL) ? 0 : pixel->data.gl.us;
         Gan_RGBPixel_us *pix;

         for ( iRow = (int)image->height-1; iRow >= 0; iRow-- )
            for ( pix = gan_image_get_pixptr_rgb_us(image,iRow,0),
                  iCol = (int)image->width-1; iCol >= 0; iCol-- )
               pix[iCol].R = val;
      }
      break;

      case GAN_UINT:
      {
         unsigned int val = (pixel == NULL) ? 0 : pixel->data.gl.ui;
         Gan_RGBPixel_ui *pix;

         for ( iRow = (int)image->height-1; iRow >= 0; iRow-- )
            for ( pix = gan_image_get_pixptr_rgb_ui(image,iRow,0),
                  iCol = (int)image->width-1; iCol >= 0; iCol-- )
               pix[iCol].R = val;
      }
      break;

      case GAN_FLOAT:
      {
         float val = (pixel == NULL) ? 0.0F : pixel->data.gl.f;
         Gan_RGBPixel_f *pix;

         for ( iRow = (int)image->height-1; iRow >= 0; iRow-- )
            for ( pix = gan_image_get_pixptr_rgb_f(image,iRow,0),
                  iCol = (int)image->width-1; iCol >= 0; iCol-- )
               pix[iCol].R = val;
      }
      break;

      case GAN_DOUBLE:
      {
         double val = (pixel == NULL) ? 0.0 : pixel->data.gl.d;
         Gan_RGBPixel_d *pix;

         for ( iRow = (int)image->height-1; iRow >= 0; iRow-- )
            for ( pix = gan_image_get_pixptr_rgb_d(image,iRow,0),
                  iCol = (int)image->width-1; iCol >= 0; iCol-- )
               pix[iCol].R = val;
      }
      break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "rgb_fill_red", GAN_ERROR_NOT_IMPLEMENTED,
                           "" );
        return GAN_FALSE;
   }

   /* success */
   return GAN_TRUE;
}

static Gan_Bool
 rgb_fill_green ( Gan_Image *image, Gan_Pixel *pixel )
{
   int iRow, iCol;

   switch ( image->type )
   {
      case GAN_UCHAR:
      {
         unsigned char val = (pixel == NULL) ? 0 : pixel->data.gl.uc;
         Gan_RGBPixel_uc *pix;

         for ( iRow = (int)image->height-1; iRow >= 0; iRow-- )
            for ( pix = gan_image_get_pixptr_rgb_uc(image,iRow,0),
                  iCol = (int)image->width-1; iCol >= 0; iCol-- )
               pix[iCol].G = val;
      }
      break;

      case GAN_USHORT:
      {
         unsigned short val = (pixel == NULL) ? 0 : pixel->data.gl.us;
         Gan_RGBPixel_us *pix;

         for ( iRow = (int)image->height-1; iRow >= 0; iRow-- )
            for ( pix = gan_image_get_pixptr_rgb_us(image,iRow,0),
                  iCol = (int)image->width-1; iCol >= 0; iCol-- )
               pix[iCol].G = val;
      }
      break;

      case GAN_UINT:
      {
         unsigned int val = (pixel == NULL) ? 0 : pixel->data.gl.ui;
         Gan_RGBPixel_ui *pix;

         for ( iRow = (int)image->height-1; iRow >= 0; iRow-- )
            for ( pix = gan_image_get_pixptr_rgb_ui(image,iRow,0),
                  iCol = (int)image->width-1; iCol >= 0; iCol-- )
               pix[iCol].G = val;
      }
      break;

      case GAN_FLOAT:
      {
         float val = (pixel == NULL) ? 0.0F : pixel->data.gl.f;
         Gan_RGBPixel_f *pix;

         for ( iRow = (int)image->height-1; iRow >= 0; iRow-- )
            for ( pix = gan_image_get_pixptr_rgb_f(image,iRow,0),
                  iCol = (int)image->width-1; iCol >= 0; iCol-- )
               pix[iCol].G = val;
      }
      break;

      case GAN_DOUBLE:
      {
         double val = (pixel == NULL) ? 0.0 : pixel->data.gl.d;
         Gan_RGBPixel_d *pix;

         for ( iRow = (int)image->height-1; iRow >= 0; iRow-- )
            for ( pix = gan_image_get_pixptr_rgb_d(image,iRow,0),
                  iCol = (int)image->width-1; iCol >= 0; iCol-- )
               pix[iCol].G = val;
      }
      break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "rgb_fill_green", GAN_ERROR_NOT_IMPLEMENTED,
                           "" );
        return GAN_FALSE;
   }

   /* success */
   return GAN_TRUE;
}

static Gan_Bool
 rgb_fill_blue ( Gan_Image *image, Gan_Pixel *pixel )
{
   int iRow, iCol;

   switch ( image->type )
   {
      case GAN_UCHAR:
      {
         unsigned char val = (pixel == NULL) ? 0 : pixel->data.gl.uc;
         Gan_RGBPixel_uc *pix;

         for ( iRow = (int)image->height-1; iRow >= 0; iRow-- )
            for ( pix = gan_image_get_pixptr_rgb_uc(image,iRow,0),
                  iCol = (int)image->width-1; iCol >= 0; iCol-- )
               pix[iCol].B = val;
      }
      break;

      case GAN_USHORT:
      {
         unsigned short val = (pixel == NULL) ? 0 : pixel->data.gl.us;
         Gan_RGBPixel_us *pix;

         for ( iRow = (int)image->height-1; iRow >= 0; iRow-- )
            for ( pix = gan_image_get_pixptr_rgb_us(image,iRow,0),
                  iCol = (int)image->width-1; iCol >= 0; iCol-- )
               pix[iCol].B = val;
      }
      break;

      case GAN_UINT:
      {
         unsigned int val = (pixel == NULL) ? 0 : pixel->data.gl.ui;
         Gan_RGBPixel_ui *pix;

         for ( iRow = (int)image->height-1; iRow >= 0; iRow-- )
            for ( pix = gan_image_get_pixptr_rgb_ui(image,iRow,0),
                  iCol = (int)image->width-1; iCol >= 0; iCol-- )
               pix[iCol].B = val;
      }
      break;

      case GAN_FLOAT:
      {
         float val = (pixel == NULL) ? 0.0F : pixel->data.gl.f;
         Gan_RGBPixel_f *pix;

         for ( iRow = (int)image->height-1; iRow >= 0; iRow-- )
            for ( pix = gan_image_get_pixptr_rgb_f(image,iRow,0),
                  iCol = (int)image->width-1; iCol >= 0; iCol-- )
               pix[iCol].B = val;
      }
      break;

      case GAN_DOUBLE:
      {
         double val = (pixel == NULL) ? 0.0 : pixel->data.gl.d;
         Gan_RGBPixel_d *pix;

         for ( iRow = (int)image->height-1; iRow >= 0; iRow-- )
            for ( pix = gan_image_get_pixptr_rgb_d(image,iRow,0),
                  iCol = (int)image->width-1; iCol >= 0; iCol-- )
               pix[iCol].B = val;
      }
      break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "rgb_fill_blue", GAN_ERROR_NOT_IMPLEMENTED,
                           "" );
        return GAN_FALSE;
   }

   /* success */
   return GAN_TRUE;
}

static Gan_Bool
 rgba_fill_red ( Gan_Image *image, Gan_Pixel *pixel )
{
   int iRow, iCol;

   switch ( image->type )
   {
      case GAN_UCHAR:
      {
         unsigned char val = (pixel == NULL) ? 0 : pixel->data.gl.uc;
         Gan_RGBAPixel_uc *pix;

         for ( iRow = (int)image->height-1; iRow >= 0; iRow-- )
            for ( pix = gan_image_get_pixptr_rgba_uc(image,iRow,0),
                  iCol = (int)image->width-1; iCol >= 0; iCol-- )
               pix[iCol].R = val;
      }
      break;

      case GAN_USHORT:
      {
         unsigned short val = (pixel == NULL) ? 0 : pixel->data.gl.us;
         Gan_RGBAPixel_us *pix;

         for ( iRow = (int)image->height-1; iRow >= 0; iRow-- )
            for ( pix = gan_image_get_pixptr_rgba_us(image,iRow,0),
                  iCol = (int)image->width-1; iCol >= 0; iCol-- )
               pix[iCol].R = val;
      }
      break;

      case GAN_UINT:
      {
         unsigned int val = (pixel == NULL) ? 0 : pixel->data.gl.ui;
         Gan_RGBAPixel_ui *pix;

         for ( iRow = (int)image->height-1; iRow >= 0; iRow-- )
            for ( pix = gan_image_get_pixptr_rgba_ui(image,iRow,0),
                  iCol = (int)image->width-1; iCol >= 0; iCol-- )
               pix[iCol].R = val;
      }
      break;

      case GAN_FLOAT:
      {
         float val = (pixel == NULL) ? 0.0F : pixel->data.gl.f;
         Gan_RGBAPixel_f *pix;

         for ( iRow = (int)image->height-1; iRow >= 0; iRow-- )
            for ( pix = gan_image_get_pixptr_rgba_f(image,iRow,0),
                  iCol = (int)image->width-1; iCol >= 0; iCol-- )
               pix[iCol].R = val;
      }
      break;

      case GAN_DOUBLE:
      {
         double val = (pixel == NULL) ? 0.0 : pixel->data.gl.d;
         Gan_RGBAPixel_d *pix;

         for ( iRow = (int)image->height-1; iRow >= 0; iRow-- )
            for ( pix = gan_image_get_pixptr_rgba_d(image,iRow,0),
                  iCol = (int)image->width-1; iCol >= 0; iCol-- )
               pix[iCol].R = val;
      }
      break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "rgba_fill_red", GAN_ERROR_NOT_IMPLEMENTED,
                           "" );
        return GAN_FALSE;
   }

   /* success */
   return GAN_TRUE;
}

static Gan_Bool
 rgba_fill_green ( Gan_Image *image, Gan_Pixel *pixel )
{
   int iRow, iCol;

   switch ( image->type )
   {
      case GAN_UCHAR:
      {
         unsigned char val = (pixel == NULL) ? 0 : pixel->data.gl.uc;
         Gan_RGBAPixel_uc *pix;

         for ( iRow = (int)image->height-1; iRow >= 0; iRow-- )
            for ( pix = gan_image_get_pixptr_rgba_uc(image,iRow,0),
                  iCol = (int)image->width-1; iCol >= 0; iCol-- )
               pix[iCol].G = val;
      }
      break;

      case GAN_USHORT:
      {
         unsigned short val = (pixel == NULL) ? 0 : pixel->data.gl.us;
         Gan_RGBAPixel_us *pix;

         for ( iRow = (int)image->height-1; iRow >= 0; iRow-- )
            for ( pix = gan_image_get_pixptr_rgba_us(image,iRow,0),
                  iCol = (int)image->width-1; iCol >= 0; iCol-- )
               pix[iCol].G = val;
      }
      break;

      case GAN_UINT:
      {
         unsigned int val = (pixel == NULL) ? 0 : pixel->data.gl.ui;
         Gan_RGBAPixel_ui *pix;

         for ( iRow = (int)image->height-1; iRow >= 0; iRow-- )
            for ( pix = gan_image_get_pixptr_rgba_ui(image,iRow,0),
                  iCol = (int)image->width-1; iCol >= 0; iCol-- )
               pix[iCol].G = val;
      }
      break;

      case GAN_FLOAT:
      {
         float val = (pixel == NULL) ? 0.0F : pixel->data.gl.f;
         Gan_RGBAPixel_f *pix;

         for ( iRow = (int)image->height-1; iRow >= 0; iRow-- )
            for ( pix = gan_image_get_pixptr_rgba_f(image,iRow,0),
                  iCol = (int)image->width-1; iCol >= 0; iCol-- )
               pix[iCol].G = val;
      }
      break;

      case GAN_DOUBLE:
      {
         double val = (pixel == NULL) ? 0.0 : pixel->data.gl.d;
         Gan_RGBAPixel_d *pix;

         for ( iRow = (int)image->height-1; iRow >= 0; iRow-- )
            for ( pix = gan_image_get_pixptr_rgba_d(image,iRow,0),
                  iCol = (int)image->width-1; iCol >= 0; iCol-- )
               pix[iCol].G = val;
      }
      break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "rgba_fill_green", GAN_ERROR_NOT_IMPLEMENTED,
                           "" );
        return GAN_FALSE;
   }

   /* success */
   return GAN_TRUE;
}

static Gan_Bool
 rgba_fill_blue ( Gan_Image *image, Gan_Pixel *pixel )
{
   int iRow, iCol;

   switch ( image->type )
   {
      case GAN_UCHAR:
      {
         unsigned char val = (pixel == NULL) ? 0 : pixel->data.gl.uc;
         Gan_RGBAPixel_uc *pix;

         for ( iRow = (int)image->height-1; iRow >= 0; iRow-- )
            for ( pix = gan_image_get_pixptr_rgba_uc(image,iRow,0),
                  iCol = (int)image->width-1; iCol >= 0; iCol-- )
               pix[iCol].B = val;
      }
      break;

      case GAN_USHORT:
      {
         unsigned short val = (pixel == NULL) ? 0 : pixel->data.gl.us;
         Gan_RGBAPixel_us *pix;

         for ( iRow = (int)image->height-1; iRow >= 0; iRow-- )
            for ( pix = gan_image_get_pixptr_rgba_us(image,iRow,0),
                  iCol = (int)image->width-1; iCol >= 0; iCol-- )
               pix[iCol].B = val;
      }
      break;

      case GAN_UINT:
      {
         unsigned int val = (pixel == NULL) ? 0 : pixel->data.gl.ui;
         Gan_RGBAPixel_ui *pix;

         for ( iRow = (int)image->height-1; iRow >= 0; iRow-- )
            for ( pix = gan_image_get_pixptr_rgba_ui(image,iRow,0),
                  iCol = (int)image->width-1; iCol >= 0; iCol-- )
               pix[iCol].B = val;
      }
      break;

      case GAN_FLOAT:
      {
         float val = (pixel == NULL) ? 0.0F : pixel->data.gl.f;
         Gan_RGBAPixel_f *pix;

         for ( iRow = (int)image->height-1; iRow >= 0; iRow-- )
            for ( pix = gan_image_get_pixptr_rgba_f(image,iRow,0),
                  iCol = (int)image->width-1; iCol >= 0; iCol-- )
               pix[iCol].B = val;
      }
      break;

      case GAN_DOUBLE:
      {
         double val = (pixel == NULL) ? 0.0 : pixel->data.gl.d;
         Gan_RGBAPixel_d *pix;

         for ( iRow = (int)image->height-1; iRow >= 0; iRow-- )
            for ( pix = gan_image_get_pixptr_rgba_d(image,iRow,0),
                  iCol = (int)image->width-1; iCol >= 0; iCol-- )
               pix[iCol].B = val;
      }
      break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "rgba_fill_blue", GAN_ERROR_NOT_IMPLEMENTED,
                           "" );
        return GAN_FALSE;
   }

   /* success */
   return GAN_TRUE;
}

static Gan_Bool
 rgba_fill_alpha ( Gan_Image *image, Gan_Pixel *pixel )
{
   int iRow, iCol;

   switch ( image->type )
   {
      case GAN_UCHAR:
      {
         unsigned char val = (pixel == NULL) ? 0 : pixel->data.gl.uc;
         Gan_RGBAPixel_uc *pix;

         for ( iRow = (int)image->height-1; iRow >= 0; iRow-- )
            for ( pix = gan_image_get_pixptr_rgba_uc(image,iRow,0),
                  iCol = (int)image->width-1; iCol >= 0; iCol-- )
               pix[iCol].A = val;
      }
      break;

      case GAN_USHORT:
      {
         unsigned short val = (pixel == NULL) ? 0 : pixel->data.gl.us;
         Gan_RGBAPixel_us *pix;

         for ( iRow = (int)image->height-1; iRow >= 0; iRow-- )
            for ( pix = gan_image_get_pixptr_rgba_us(image,iRow,0),
                  iCol = (int)image->width-1; iCol >= 0; iCol-- )
               pix[iCol].A = val;
      }
      break;

      case GAN_UINT:
      {
         unsigned int val = (pixel == NULL) ? 0 : pixel->data.gl.ui;
         Gan_RGBAPixel_ui *pix;

         for ( iRow = (int)image->height-1; iRow >= 0; iRow-- )
            for ( pix = gan_image_get_pixptr_rgba_ui(image,iRow,0),
                  iCol = (int)image->width-1; iCol >= 0; iCol-- )
               pix[iCol].A = val;
      }
      break;

      case GAN_FLOAT:
      {
         float val = (pixel == NULL) ? 0.0F : pixel->data.gl.f;
         Gan_RGBAPixel_f *pix;

         for ( iRow = (int)image->height-1; iRow >= 0; iRow-- )
            for ( pix = gan_image_get_pixptr_rgba_f(image,iRow,0),
                  iCol = (int)image->width-1; iCol >= 0; iCol-- )
               pix[iCol].A = val;
      }
      break;

      case GAN_DOUBLE:
      {
         double val = (pixel == NULL) ? 0.0 : pixel->data.gl.d;
         Gan_RGBAPixel_d *pix;

         for ( iRow = (int)image->height-1; iRow >= 0; iRow-- )
            for ( pix = gan_image_get_pixptr_rgba_d(image,iRow,0),
                  iCol = (int)image->width-1; iCol >= 0; iCol-- )
               pix[iCol].A = val;
      }
      break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "rgba_fill_alpha", GAN_ERROR_NOT_IMPLEMENTED,
                           "" );
        return GAN_FALSE;
   }

   /* success */
   return GAN_TRUE;
}

static Gan_Bool
 bgr_fill_red ( Gan_Image *image, Gan_Pixel *pixel )
{
   int iRow, iCol;

   switch ( image->type )
   {
      case GAN_UCHAR:
      {
         unsigned char val = (pixel == NULL) ? 0 : pixel->data.gl.uc;
         Gan_BGRPixel_uc *pix;

         for ( iRow = (int)image->height-1; iRow >= 0; iRow-- )
            for ( pix = gan_image_get_pixptr_bgr_uc(image,iRow,0),
                  iCol = (int)image->width-1; iCol >= 0; iCol-- )
               pix[iCol].R = val;
      }
      break;

      case GAN_USHORT:
      {
         unsigned short val = (pixel == NULL) ? 0 : pixel->data.gl.us;
         Gan_BGRPixel_us *pix;

         for ( iRow = (int)image->height-1; iRow >= 0; iRow-- )
            for ( pix = gan_image_get_pixptr_bgr_us(image,iRow,0),
                  iCol = (int)image->width-1; iCol >= 0; iCol-- )
               pix[iCol].R = val;
      }
      break;

      case GAN_UINT:
      {
         unsigned int val = (pixel == NULL) ? 0 : pixel->data.gl.ui;
         Gan_BGRPixel_ui *pix;

         for ( iRow = (int)image->height-1; iRow >= 0; iRow-- )
            for ( pix = gan_image_get_pixptr_bgr_ui(image,iRow,0),
                  iCol = (int)image->width-1; iCol >= 0; iCol-- )
               pix[iCol].R = val;
      }
      break;

      case GAN_FLOAT:
      {
         float val = (pixel == NULL) ? 0.0F : pixel->data.gl.f;
         Gan_BGRPixel_f *pix;

         for ( iRow = (int)image->height-1; iRow >= 0; iRow-- )
            for ( pix = gan_image_get_pixptr_bgr_f(image,iRow,0),
                  iCol = (int)image->width-1; iCol >= 0; iCol-- )
               pix[iCol].R = val;
      }
      break;

      case GAN_DOUBLE:
      {
         double val = (pixel == NULL) ? 0.0 : pixel->data.gl.d;
         Gan_BGRPixel_d *pix;

         for ( iRow = (int)image->height-1; iRow >= 0; iRow-- )
            for ( pix = gan_image_get_pixptr_bgr_d(image,iRow,0),
                  iCol = (int)image->width-1; iCol >= 0; iCol-- )
               pix[iCol].R = val;
      }
      break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "bgr_fill_red", GAN_ERROR_NOT_IMPLEMENTED,
                           "" );
        return GAN_FALSE;
   }

   /* success */
   return GAN_TRUE;
}

static Gan_Bool
 bgr_fill_green ( Gan_Image *image, Gan_Pixel *pixel )
{
   int iRow, iCol;

   switch ( image->type )
   {
      case GAN_UCHAR:
      {
         unsigned char val = (pixel == NULL) ? 0 : pixel->data.gl.uc;
         Gan_BGRPixel_uc *pix;

         for ( iRow = (int)image->height-1; iRow >= 0; iRow-- )
            for ( pix = gan_image_get_pixptr_bgr_uc(image,iRow,0),
                  iCol = (int)image->width-1; iCol >= 0; iCol-- )
               pix[iCol].G = val;
      }
      break;

      case GAN_USHORT:
      {
         unsigned short val = (pixel == NULL) ? 0 : pixel->data.gl.us;
         Gan_BGRPixel_us *pix;

         for ( iRow = (int)image->height-1; iRow >= 0; iRow-- )
            for ( pix = gan_image_get_pixptr_bgr_us(image,iRow,0),
                  iCol = (int)image->width-1; iCol >= 0; iCol-- )
               pix[iCol].G = val;
      }
      break;

      case GAN_UINT:
      {
         unsigned int val = (pixel == NULL) ? 0 : pixel->data.gl.ui;
         Gan_BGRPixel_ui *pix;

         for ( iRow = (int)image->height-1; iRow >= 0; iRow-- )
            for ( pix = gan_image_get_pixptr_bgr_ui(image,iRow,0),
                  iCol = (int)image->width-1; iCol >= 0; iCol-- )
               pix[iCol].G = val;
      }
      break;

      case GAN_FLOAT:
      {
         float val = (pixel == NULL) ? 0.0F : pixel->data.gl.f;
         Gan_BGRPixel_f *pix;

         for ( iRow = (int)image->height-1; iRow >= 0; iRow-- )
            for ( pix = gan_image_get_pixptr_bgr_f(image,iRow,0),
                  iCol = (int)image->width-1; iCol >= 0; iCol-- )
               pix[iCol].G = val;
      }
      break;

      case GAN_DOUBLE:
      {
         double val = (pixel == NULL) ? 0.0 : pixel->data.gl.d;
         Gan_BGRPixel_d *pix;

         for ( iRow = (int)image->height-1; iRow >= 0; iRow-- )
            for ( pix = gan_image_get_pixptr_bgr_d(image,iRow,0),
                  iCol = (int)image->width-1; iCol >= 0; iCol-- )
               pix[iCol].G = val;
      }
      break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "bgr_fill_green", GAN_ERROR_NOT_IMPLEMENTED,
                           "" );
        return GAN_FALSE;
   }

   /* success */
   return GAN_TRUE;
}

static Gan_Bool
 bgr_fill_blue ( Gan_Image *image, Gan_Pixel *pixel )
{
   int iRow, iCol;

   switch ( image->type )
   {
      case GAN_UCHAR:
      {
         unsigned char val = (pixel == NULL) ? 0 : pixel->data.gl.uc;
         Gan_BGRPixel_uc *pix;

         for ( iRow = (int)image->height-1; iRow >= 0; iRow-- )
            for ( pix = gan_image_get_pixptr_bgr_uc(image,iRow,0),
                  iCol = (int)image->width-1; iCol >= 0; iCol-- )
               pix[iCol].B = val;
      }
      break;

      case GAN_USHORT:
      {
         unsigned short val = (pixel == NULL) ? 0 : pixel->data.gl.us;
         Gan_BGRPixel_us *pix;

         for ( iRow = (int)image->height-1; iRow >= 0; iRow-- )
            for ( pix = gan_image_get_pixptr_bgr_us(image,iRow,0),
                  iCol = (int)image->width-1; iCol >= 0; iCol-- )
               pix[iCol].B = val;
      }
      break;

      case GAN_UINT:
      {
         unsigned int val = (pixel == NULL) ? 0 : pixel->data.gl.ui;
         Gan_BGRPixel_ui *pix;

         for ( iRow = (int)image->height-1; iRow >= 0; iRow-- )
            for ( pix = gan_image_get_pixptr_bgr_ui(image,iRow,0),
                  iCol = (int)image->width-1; iCol >= 0; iCol-- )
               pix[iCol].B = val;
      }
      break;

      case GAN_FLOAT:
      {
         float val = (pixel == NULL) ? 0.0F : pixel->data.gl.f;
         Gan_BGRPixel_f *pix;

         for ( iRow = (int)image->height-1; iRow >= 0; iRow-- )
            for ( pix = gan_image_get_pixptr_bgr_f(image,iRow,0),
                  iCol = (int)image->width-1; iCol >= 0; iCol-- )
               pix[iCol].B = val;
      }
      break;

      case GAN_DOUBLE:
      {
         double val = (pixel == NULL) ? 0.0 : pixel->data.gl.d;
         Gan_BGRPixel_d *pix;

         for ( iRow = (int)image->height-1; iRow >= 0; iRow-- )
            for ( pix = gan_image_get_pixptr_bgr_d(image,iRow,0),
                  iCol = (int)image->width-1; iCol >= 0; iCol-- )
               pix[iCol].B = val;
      }
      break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "bgr_fill_blue", GAN_ERROR_NOT_IMPLEMENTED,
                           "" );
        return GAN_FALSE;
   }

   /* success */
   return GAN_TRUE;
}

static Gan_Bool
 bgra_fill_red ( Gan_Image *image, Gan_Pixel *pixel )
{
   int iRow, iCol;

   switch ( image->type )
   {
      case GAN_UCHAR:
      {
         unsigned char val = (pixel == NULL) ? 0 : pixel->data.gl.uc;
         Gan_BGRAPixel_uc *pix;

         for ( iRow = (int)image->height-1; iRow >= 0; iRow-- )
            for ( pix = gan_image_get_pixptr_bgra_uc(image,iRow,0),
                  iCol = (int)image->width-1; iCol >= 0; iCol-- )
               pix[iCol].R = val;
      }
      break;

      case GAN_USHORT:
      {
         unsigned short val = (pixel == NULL) ? 0 : pixel->data.gl.us;
         Gan_BGRAPixel_us *pix;

         for ( iRow = (int)image->height-1; iRow >= 0; iRow-- )
            for ( pix = gan_image_get_pixptr_bgra_us(image,iRow,0),
                  iCol = (int)image->width-1; iCol >= 0; iCol-- )
               pix[iCol].R = val;
      }
      break;

      case GAN_UINT:
      {
         unsigned int val = (pixel == NULL) ? 0 : pixel->data.gl.ui;
         Gan_BGRAPixel_ui *pix;

         for ( iRow = (int)image->height-1; iRow >= 0; iRow-- )
            for ( pix = gan_image_get_pixptr_bgra_ui(image,iRow,0),
                  iCol = (int)image->width-1; iCol >= 0; iCol-- )
               pix[iCol].R = val;
      }
      break;

      case GAN_FLOAT:
      {
         float val = (pixel == NULL) ? 0.0F : pixel->data.gl.f;
         Gan_BGRAPixel_f *pix;

         for ( iRow = (int)image->height-1; iRow >= 0; iRow-- )
            for ( pix = gan_image_get_pixptr_bgra_f(image,iRow,0),
                  iCol = (int)image->width-1; iCol >= 0; iCol-- )
               pix[iCol].R = val;
      }
      break;

      case GAN_DOUBLE:
      {
         double val = (pixel == NULL) ? 0.0 : pixel->data.gl.d;
         Gan_BGRAPixel_d *pix;

         for ( iRow = (int)image->height-1; iRow >= 0; iRow-- )
            for ( pix = gan_image_get_pixptr_bgra_d(image,iRow,0),
                  iCol = (int)image->width-1; iCol >= 0; iCol-- )
               pix[iCol].R = val;
      }
      break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "bgra_fill_red", GAN_ERROR_NOT_IMPLEMENTED,
                           "" );
        return GAN_FALSE;
   }

   /* success */
   return GAN_TRUE;
}

static Gan_Bool
 bgra_fill_green ( Gan_Image *image, Gan_Pixel *pixel )
{
   int iRow, iCol;

   switch ( image->type )
   {
      case GAN_UCHAR:
      {
         unsigned char val = (pixel == NULL) ? 0 : pixel->data.gl.uc;
         Gan_BGRAPixel_uc *pix;

         for ( iRow = (int)image->height-1; iRow >= 0; iRow-- )
            for ( pix = gan_image_get_pixptr_bgra_uc(image,iRow,0),
                  iCol = (int)image->width-1; iCol >= 0; iCol-- )
               pix[iCol].G = val;
      }
      break;

      case GAN_USHORT:
      {
         unsigned short val = (pixel == NULL) ? 0 : pixel->data.gl.us;
         Gan_BGRAPixel_us *pix;

         for ( iRow = (int)image->height-1; iRow >= 0; iRow-- )
            for ( pix = gan_image_get_pixptr_bgra_us(image,iRow,0),
                  iCol = (int)image->width-1; iCol >= 0; iCol-- )
               pix[iCol].G = val;
      }
      break;

      case GAN_UINT:
      {
         unsigned int val = (pixel == NULL) ? 0 : pixel->data.gl.ui;
         Gan_BGRAPixel_ui *pix;

         for ( iRow = (int)image->height-1; iRow >= 0; iRow-- )
            for ( pix = gan_image_get_pixptr_bgra_ui(image,iRow,0),
                  iCol = (int)image->width-1; iCol >= 0; iCol-- )
               pix[iCol].G = val;
      }
      break;

      case GAN_FLOAT:
      {
         float val = (pixel == NULL) ? 0.0F : pixel->data.gl.f;
         Gan_BGRAPixel_f *pix;

         for ( iRow = (int)image->height-1; iRow >= 0; iRow-- )
            for ( pix = gan_image_get_pixptr_bgra_f(image,iRow,0),
                  iCol = (int)image->width-1; iCol >= 0; iCol-- )
               pix[iCol].G = val;
      }
      break;

      case GAN_DOUBLE:
      {
         double val = (pixel == NULL) ? 0.0 : pixel->data.gl.d;
         Gan_BGRAPixel_d *pix;

         for ( iRow = (int)image->height-1; iRow >= 0; iRow-- )
            for ( pix = gan_image_get_pixptr_bgra_d(image,iRow,0),
                  iCol = (int)image->width-1; iCol >= 0; iCol-- )
               pix[iCol].G = val;
      }
      break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "bgra_fill_green", GAN_ERROR_NOT_IMPLEMENTED,
                           "" );
        return GAN_FALSE;
   }

   /* success */
   return GAN_TRUE;
}

static Gan_Bool
 bgra_fill_blue ( Gan_Image *image, Gan_Pixel *pixel )
{
   int iRow, iCol;

   switch ( image->type )
   {
      case GAN_UCHAR:
      {
         unsigned char val = (pixel == NULL) ? 0 : pixel->data.gl.uc;
         Gan_BGRAPixel_uc *pix;

         for ( iRow = (int)image->height-1; iRow >= 0; iRow-- )
            for ( pix = gan_image_get_pixptr_bgra_uc(image,iRow,0),
                  iCol = (int)image->width-1; iCol >= 0; iCol-- )
               pix[iCol].B = val;
      }
      break;

      case GAN_USHORT:
      {
         unsigned short val = (pixel == NULL) ? 0 : pixel->data.gl.us;
         Gan_BGRAPixel_us *pix;

         for ( iRow = (int)image->height-1; iRow >= 0; iRow-- )
            for ( pix = gan_image_get_pixptr_bgra_us(image,iRow,0),
                  iCol = (int)image->width-1; iCol >= 0; iCol-- )
               pix[iCol].B = val;
      }
      break;

      case GAN_UINT:
      {
         unsigned int val = (pixel == NULL) ? 0 : pixel->data.gl.ui;
         Gan_BGRAPixel_ui *pix;

         for ( iRow = (int)image->height-1; iRow >= 0; iRow-- )
            for ( pix = gan_image_get_pixptr_bgra_ui(image,iRow,0),
                  iCol = (int)image->width-1; iCol >= 0; iCol-- )
               pix[iCol].B = val;
      }
      break;

      case GAN_FLOAT:
      {
         float val = (pixel == NULL) ? 0.0F : pixel->data.gl.f;
         Gan_BGRAPixel_f *pix;

         for ( iRow = (int)image->height-1; iRow >= 0; iRow-- )
            for ( pix = gan_image_get_pixptr_bgra_f(image,iRow,0),
                  iCol = (int)image->width-1; iCol >= 0; iCol-- )
               pix[iCol].B = val;
      }
      break;

      case GAN_DOUBLE:
      {
         double val = (pixel == NULL) ? 0.0 : pixel->data.gl.d;
         Gan_BGRAPixel_d *pix;

         for ( iRow = (int)image->height-1; iRow >= 0; iRow-- )
            for ( pix = gan_image_get_pixptr_bgra_d(image,iRow,0),
                  iCol = (int)image->width-1; iCol >= 0; iCol-- )
               pix[iCol].B = val;
      }
      break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "bgra_fill_blue", GAN_ERROR_NOT_IMPLEMENTED,
                           "" );
        return GAN_FALSE;
   }

   /* success */
   return GAN_TRUE;
}

static Gan_Bool
 bgra_fill_alpha ( Gan_Image *image, Gan_Pixel *pixel )
{
   int iRow, iCol;

   switch ( image->type )
   {
      case GAN_UCHAR:
      {
         unsigned char val = (pixel == NULL) ? 0 : pixel->data.gl.uc;
         Gan_BGRAPixel_uc *pix;

         for ( iRow = (int)image->height-1; iRow >= 0; iRow-- )
            for ( pix = gan_image_get_pixptr_bgra_uc(image,iRow,0),
                  iCol = (int)image->width-1; iCol >= 0; iCol-- )
               pix[iCol].A = val;
      }
      break;

      case GAN_USHORT:
      {
         unsigned short val = (pixel == NULL) ? 0 : pixel->data.gl.us;
         Gan_BGRAPixel_us *pix;

         for ( iRow = (int)image->height-1; iRow >= 0; iRow-- )
            for ( pix = gan_image_get_pixptr_bgra_us(image,iRow,0),
                  iCol = (int)image->width-1; iCol >= 0; iCol-- )
               pix[iCol].A = val;
      }
      break;

      case GAN_UINT:
      {
         unsigned int val = (pixel == NULL) ? 0 : pixel->data.gl.ui;
         Gan_BGRAPixel_ui *pix;

         for ( iRow = (int)image->height-1; iRow >= 0; iRow-- )
            for ( pix = gan_image_get_pixptr_bgra_ui(image,iRow,0),
                  iCol = (int)image->width-1; iCol >= 0; iCol-- )
               pix[iCol].A = val;
      }
      break;

      case GAN_FLOAT:
      {
         float val = (pixel == NULL) ? 0.0F : pixel->data.gl.f;
         Gan_BGRAPixel_f *pix;

         for ( iRow = (int)image->height-1; iRow >= 0; iRow-- )
            for ( pix = gan_image_get_pixptr_bgra_f(image,iRow,0),
                  iCol = (int)image->width-1; iCol >= 0; iCol-- )
               pix[iCol].A = val;
      }
      break;

      case GAN_DOUBLE:
      {
         double val = (pixel == NULL) ? 0.0 : pixel->data.gl.d;
         Gan_BGRAPixel_d *pix;

         for ( iRow = (int)image->height-1; iRow >= 0; iRow-- )
            for ( pix = gan_image_get_pixptr_bgra_d(image,iRow,0),
                  iCol = (int)image->width-1; iCol >= 0; iCol-- )
               pix[iCol].A = val;
      }
      break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "bgra_fill_alpha", GAN_ERROR_NOT_IMPLEMENTED,
                           "" );
        return GAN_FALSE;
   }

   /* success */
   return GAN_TRUE;
}

/**
 * \brief Sets a single channel of an image to a constant value.
 * \param image The input image
 * \param channel The channel to set to constant value
 * \param pixel Pointer to grey-level pixel value
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Sets a single channel of an \a image to a constant value. The \a channel
 * should correspond to one of the available channels in the \a image.
 * The type of the \a pixel structure should be the same as the \a image,
 * and the format should be #GAN_GREY_LEVEL_IMAGE. If \a pixel is passed as
 * \c NULL, the channel is filled with zeros.
 *
 * \sa gan_image_fill_channel_zero().
 */
Gan_Bool
 gan_image_fill_channel_const ( Gan_Image *image,
                                Gan_ImageChannelType channel,
                                Gan_Pixel *pixel )
{
   Gan_Bool bResult;

   gan_err_test_bool ( pixel == NULL ||
                       (pixel->format == GAN_GREY_LEVEL_IMAGE &&
                        pixel->type == image->type),
                       "gan_image_fill_channel_const", GAN_ERROR_INCOMPATIBLE,
                       "" );
   
   switch ( image->format )
   {
      case GAN_GREY_LEVEL_IMAGE:
        switch ( channel )
        {
           case GAN_INTENSITY_CHANNEL:
             bResult = gan_image_fill_const ( image, pixel );
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_fill_channel_const",
                                GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return GAN_FALSE;
        }
        break;

      case GAN_GREY_LEVEL_ALPHA_IMAGE:
        switch ( channel )
        {
           case GAN_INTENSITY_CHANNEL:
             bResult = gla_fill_intensity ( image, pixel );
             break;

           case GAN_ALPHA_CHANNEL:
             bResult = gla_fill_alpha ( image, pixel );
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_fill_channel_const",
                                GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return GAN_FALSE;
        }
        break;

      case GAN_RGB_COLOUR_IMAGE:
        switch ( channel )
        {
           case GAN_RED_CHANNEL:
             bResult = rgb_fill_red ( image, pixel );
             break;

           case GAN_GREEN_CHANNEL:
             bResult = rgb_fill_green ( image, pixel );
             break;

           case GAN_BLUE_CHANNEL:
             bResult = rgb_fill_blue ( image, pixel );
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_fill_channel_const",
                                GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return GAN_FALSE;
        }
        break;

      case GAN_RGB_COLOUR_ALPHA_IMAGE:
        switch ( channel )
        {
           case GAN_RED_CHANNEL:
             bResult = rgba_fill_red ( image, pixel );
             break;

           case GAN_GREEN_CHANNEL:
             bResult = rgba_fill_green ( image, pixel );
             break;

           case GAN_BLUE_CHANNEL:
             bResult = rgba_fill_blue ( image, pixel );
             break;

           case GAN_ALPHA_CHANNEL:
             bResult = rgba_fill_alpha ( image, pixel );
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_fill_channel_const",
                                GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return GAN_FALSE;
        }
        break;

      case GAN_BGR_COLOUR_IMAGE:
        switch ( channel )
        {
           case GAN_RED_CHANNEL:
             bResult = bgr_fill_red ( image, pixel );
             break;

           case GAN_GREEN_CHANNEL:
             bResult = bgr_fill_green ( image, pixel );
             break;

           case GAN_BLUE_CHANNEL:
             bResult = bgr_fill_blue ( image, pixel );
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_fill_channel_const",
                                GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return GAN_FALSE;
        }
        break;

      case GAN_BGR_COLOUR_ALPHA_IMAGE:
        switch ( channel )
        {
           case GAN_RED_CHANNEL:
             bResult = bgra_fill_red ( image, pixel );
             break;

           case GAN_GREEN_CHANNEL:
             bResult = bgra_fill_green ( image, pixel );
             break;

           case GAN_BLUE_CHANNEL:
             bResult = bgra_fill_blue ( image, pixel );
             break;

           case GAN_ALPHA_CHANNEL:
             bResult = bgra_fill_alpha ( image, pixel );
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_fill_channel_const",
                                GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return GAN_FALSE;
        }
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_image_fill_channel_const", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
        return GAN_FALSE;
   }
                                          
   if ( !bResult )
   {
      gan_err_register ( "gan_image_fill_channel_const", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   /* success */
   return GAN_TRUE;
}

/**
 * \brief Finds the minimum value in the specified channel of an image
 * \param image The input image
 * \param mask Only test pixels in this mask (may be \c NULL)
 * \param channel The channel to set to constant value
 * \param minval Pointer to the result minimum pixel value
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Finds the minimum value in the specified channel of an image.
 */
Gan_Bool gan_image_get_minimum_channel_value ( Gan_Image *image,
                                               Gan_Image *mask,
                                               Gan_ImageChannelType channel,
                                               Gan_Pixel *minval )
{
   Gan_ImageWindow window;
   unsigned int row, col, row_max, col_max;

   if(image->format == GAN_GREY_LEVEL_IMAGE)
   {
      if(channel != GAN_INTENSITY_CHANNEL)
      {
         gan_err_flush_trace();
         gan_err_register("gan_image_get_minimum_channel_value",
                          GAN_ERROR_INCOMPATIBLE, "");
         return GAN_FALSE;
      }
   
      if(!gan_image_get_minimum_pixel(image, mask, minval))
      {
         gan_err_register("gan_image_get_minimum_channel_value",
                          GAN_ERROR_FAILURE, "");
         return GAN_FALSE;
      }

      return GAN_TRUE;
   }

   if(image->width == 0 || image->height == 0)
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_image_get_minimum_channel_value",
                         GAN_ERROR_NO_DATA, "" );
      return GAN_FALSE;
   }

   gan_err_test_bool ( mask == NULL ||
                       (mask->width == image->width &&
                        mask->height == image->height),
                       "gan_image_get_minimum_channel_value",
                       GAN_ERROR_INCOMPATIBLE, "" );
   minval->format = GAN_GREY_LEVEL_IMAGE;
   minval->type = image->type;

   if(mask == NULL)
   {
      window.r0 = window.c0 = 0;
      window.width = image->width;
      window.height = image->height;
   }
   else
   {
      if(!gan_image_get_active_subwindow_b ( mask, GAN_BIT_ALIGNMENT,
                                             &window ))
      {
         gan_err_register ( "gan_image_get_minimum_channel_value",
                            GAN_ERROR_FAILURE, "" );
         return GAN_FALSE;
      }

      if(window.width == 0 || window.height == 0)
      {
         gan_err_flush_trace();
         gan_err_register ( "gan_image_get_minimum_channel_value",
                            GAN_ERROR_NO_DATA, "" );
         return GAN_FALSE;
      }
   }

   col_max = window.c0 + window.width;
   row_max = window.r0 + window.height;

   switch ( image->format )
   {
      case GAN_RGB_COLOUR_ALPHA_IMAGE:
        switch ( channel )
        {
           case GAN_ALPHA_CHANNEL:
             switch(image->type)
             {
                case GAN_UCHAR:
                {
                   unsigned char ucminval = UCHAR_MAX, ucval;

                   if(mask == NULL)
                   {
                      for ( row = window.r0; row < row_max; row++ )
                         for ( col = window.c0; col < col_max; col++ )
                         {
                            ucval = gan_image_get_pixptr_rgba_uc(image, row, col)->A;
                            if(ucval < ucminval) ucminval = ucval;
                         }
                   }
                   else
                   {
                      for ( row = window.r0; row < row_max; row++ )
                         for ( col = window.c0; col < col_max; col++ )
                            if(gan_image_get_pix_b(mask, row, col))
                            {
                               ucval = gan_image_get_pixptr_rgba_uc(image, row, col)->A;
                               if(ucval < ucminval) ucminval = ucval;
                            }
                   }

                   minval->data.rgba.uc.A = ucminval;
                }
                break;

                case GAN_USHORT:
                {
                   unsigned short usminval = USHRT_MAX, usval;

                   if(mask == NULL)
                   {
                      for ( row = window.r0; row < row_max; row++ )
                         for ( col = window.c0; col < col_max; col++ )
                         {
                            usval = gan_image_get_pixptr_rgba_us(image, row, col)->A;
                            if(usval < usminval) usminval = usval;
                         }
                   }
                   else
                   {
                      for ( row = window.r0; row < row_max; row++ )
                         for ( col = window.c0; col < col_max; col++ )
                            if(gan_image_get_pix_b(mask, row, col))
                            {
                               usval = gan_image_get_pixptr_rgba_us(image, row, col)->A;
                               if(usval < usminval) usminval = usval;
                            }
                   }

                   minval->data.rgba.us.A = usminval;
                }
                break;

                case GAN_UINT:
                {
                   unsigned int uiminval = UINT_MAX, uival;

                   if(mask == NULL)
                   {
                      for ( row = window.r0; row < row_max; row++ )
                         for ( col = window.c0; col < col_max; col++ )
                         {
                            uival = gan_image_get_pixptr_rgba_ui(image, row, col)->A;
                            if(uival < uiminval) uiminval = uival;
                         }
                   }
                   else
                   {
                      for ( row = window.r0; row < row_max; row++ )
                         for ( col = window.c0; col < col_max; col++ )
                            if(gan_image_get_pix_b(mask, row, col))
                            {
                               uival = gan_image_get_pixptr_rgba_ui(image, row, col)->A;
                               if(uival < uiminval) uiminval = uival;
                            }
                   }

                   minval->data.rgba.ui.A = uiminval;
                }
                break;

                case GAN_FLOAT:
                {
                   float fminval = FLT_MAX, fval;

                   if(mask == NULL)
                   {
                      for ( row = window.r0; row < row_max; row++ )
                         for ( col = window.c0; col < col_max; col++ )
                         {
                            fval = gan_image_get_pixptr_rgba_f(image, row, col)->A;
                            if(fval < fminval) fminval = fval;
                         }
                   }
                   else
                   {
                      for ( row = window.r0; row < row_max; row++ )
                         for ( col = window.c0; col < col_max; col++ )
                            if(gan_image_get_pix_b(mask, row, col))
                            {
                               fval = gan_image_get_pixptr_rgba_f(image, row, col)->A;
                               if(fval < fminval) fminval = fval;
                            }
                   }

                   minval->data.rgba.f.A = fminval;
                }
                break;

                default:
                  gan_err_flush_trace();
                  gan_err_register ( "gan_image_get_minimum_channel_value", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
                  return GAN_FALSE;
             }
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_get_minimum_channel_value", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return GAN_FALSE;
        }
        break;

      case GAN_BGR_COLOUR_ALPHA_IMAGE:
        switch ( channel )
        {
           case GAN_ALPHA_CHANNEL:
             switch(image->type)
             {
                case GAN_UCHAR:
                {
                   unsigned char ucminval = UCHAR_MAX, ucval;

                   if(mask == NULL)
                   {
                      for ( row = window.r0; row < row_max; row++ )
                         for ( col = window.c0; col < col_max; col++ )
                         {
                            ucval = gan_image_get_pixptr_bgra_uc(image, row, col)->A;
                            if(ucval < ucminval) ucminval = ucval;
                         }
                   }
                   else
                   {
                      for ( row = window.r0; row < row_max; row++ )
                         for ( col = window.c0; col < col_max; col++ )
                            if(gan_image_get_pix_b(mask, row, col))
                            {
                               ucval = gan_image_get_pixptr_bgra_uc(image, row, col)->A;
                               if(ucval < ucminval) ucminval = ucval;
                            }
                   }

                   minval->data.bgra.uc.A = ucminval;
                }
                break;

                case GAN_USHORT:
                {
                   unsigned short usminval = USHRT_MAX, usval;

                   if(mask == NULL)
                   {
                      for ( row = window.r0; row < row_max; row++ )
                         for ( col = window.c0; col < col_max; col++ )
                         {
                            usval = gan_image_get_pixptr_bgra_us(image, row, col)->A;
                            if(usval < usminval) usminval = usval;
                         }
                   }
                   else
                   {
                      for ( row = window.r0; row < row_max; row++ )
                         for ( col = window.c0; col < col_max; col++ )
                            if(gan_image_get_pix_b(mask, row, col))
                            {
                               usval = gan_image_get_pixptr_bgra_us(image, row, col)->A;
                               if(usval < usminval) usminval = usval;
                            }
                   }

                   minval->data.bgra.us.A = usminval;
                }
                break;

                case GAN_UINT:
                {
                   unsigned int uiminval = UINT_MAX, uival;

                   if(mask == NULL)
                   {
                      for ( row = window.r0; row < row_max; row++ )
                         for ( col = window.c0; col < col_max; col++ )
                         {
                            uival = gan_image_get_pixptr_bgra_ui(image, row, col)->A;
                            if(uival < uiminval) uiminval = uival;
                         }
                   }
                   else
                   {
                      for ( row = window.r0; row < row_max; row++ )
                         for ( col = window.c0; col < col_max; col++ )
                            if(gan_image_get_pix_b(mask, row, col))
                            {
                               uival = gan_image_get_pixptr_bgra_ui(image, row, col)->A;
                               if(uival < uiminval) uiminval = uival;
                            }
                   }

                   minval->data.bgra.ui.A = uiminval;
                }
                break;

                case GAN_FLOAT:
                {
                   float fminval = FLT_MAX, fval;

                   if(mask == NULL)
                   {
                      for ( row = window.r0; row < row_max; row++ )
                         for ( col = window.c0; col < col_max; col++ )
                         {
                            fval = gan_image_get_pixptr_bgra_f(image, row, col)->A;
                            if(fval < fminval) fminval = fval;
                         }
                   }
                   else
                   {
                      for ( row = window.r0; row < row_max; row++ )
                         for ( col = window.c0; col < col_max; col++ )
                            if(gan_image_get_pix_b(mask, row, col))
                            {
                               fval = gan_image_get_pixptr_bgra_f(image, row, col)->A;
                               if(fval < fminval) fminval = fval;
                            }
                   }

                   minval->data.bgra.f.A = fminval;
                }
                break;

                default:
                  gan_err_flush_trace();
                  gan_err_register ( "gan_image_get_minimum_channel_value", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
                  return GAN_FALSE;
             }
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_get_minimum_channel_value", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return GAN_FALSE;
        }
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_image_get_minimum_channel_value", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
        return GAN_FALSE;
   }

   return GAN_TRUE;
}

/**
 * \brief Finds the maximum value in the specified channel of an image
 * \param image The input image
 * \param mask Only test pixels in this mask (may be \c NULL)
 * \param channel The channel to set to constant value
 * \param maxval Pointer to the result maximum pixel value
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Finds the maximum value in the specified channel of an image.
 */
Gan_Bool gan_image_get_maximum_channel_value ( Gan_Image *image,
                                               Gan_Image *mask,
                                               Gan_ImageChannelType channel,
                                               Gan_Pixel *maxval )
{
   Gan_ImageWindow window;
   unsigned int row, col, row_max, col_max;

   if(image->format == GAN_GREY_LEVEL_IMAGE)
   {
      if(channel != GAN_INTENSITY_CHANNEL)
      {
         gan_err_flush_trace();
         gan_err_register("gan_image_get_maximum_channel_value",
                          GAN_ERROR_INCOMPATIBLE, "");
         return GAN_FALSE;
      }
   
      if(!gan_image_get_maximum_pixel(image, mask, maxval))
      {
         gan_err_register("gan_image_get_maximum_channel_value",
                          GAN_ERROR_FAILURE, "");
         return GAN_FALSE;
      }

      return GAN_TRUE;
   }

   if(image->width == 0 || image->height == 0)
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_image_get_maximum_channel_value",
                         GAN_ERROR_NO_DATA, "" );
      return GAN_FALSE;
   }

   gan_err_test_bool ( mask == NULL ||
                       (mask->width == image->width &&
                        mask->height == image->height),
                       "gan_image_get_maximum_channel_value",
                       GAN_ERROR_INCOMPATIBLE, "" );
   maxval->format = GAN_GREY_LEVEL_IMAGE;
   maxval->type = image->type;

   if(mask == NULL)
   {
      window.r0 = window.c0 = 0;
      window.width = image->width;
      window.height = image->height;
   }
   else
   {
      if(!gan_image_get_active_subwindow_b ( mask, GAN_BIT_ALIGNMENT,
                                             &window ))
      {
         gan_err_register ( "gan_image_get_maximum_channel_value",
                            GAN_ERROR_FAILURE, "" );
         return GAN_FALSE;
      }

      if(window.width == 0 || window.height == 0)
      {
         gan_err_flush_trace();
         gan_err_register ( "gan_image_get_maximum_channel_value",
                            GAN_ERROR_NO_DATA, "" );
         return GAN_FALSE;
      }
   }

   col_max = window.c0 + window.width;
   row_max = window.r0 + window.height;

   switch ( image->format )
   {
      case GAN_RGB_COLOUR_ALPHA_IMAGE:
        switch ( channel )
        {
           case GAN_ALPHA_CHANNEL:
             switch(image->type)
             {
                case GAN_UCHAR:
                {
                   unsigned char ucmaxval = 0, ucval;

                   if(mask == NULL)
                   {
                      for ( row = window.r0; row < row_max; row++ )
                         for ( col = window.c0; col < col_max; col++ )
                         {
                            ucval = gan_image_get_pixptr_rgba_uc(image, row, col)->A;
                            if(ucval > ucmaxval) ucmaxval = ucval;
                         }
                   }
                   else
                   {
                      for ( row = window.r0; row < row_max; row++ )
                         for ( col = window.c0; col < col_max; col++ )
                            if(gan_image_get_pix_b(mask, row, col))
                            {
                               ucval = gan_image_get_pixptr_rgba_uc(image, row, col)->A;
                               if(ucval > ucmaxval) ucmaxval = ucval;
                            }
                   }

                   maxval->data.gl.uc = ucmaxval;
                }
                break;

                case GAN_USHORT:
                {
                   unsigned short usmaxval = 0, usval;

                   if(mask == NULL)
                   {
                      for ( row = window.r0; row < row_max; row++ )
                         for ( col = window.c0; col < col_max; col++ )
                         {
                            usval = gan_image_get_pixptr_rgba_us(image, row, col)->A;
                            if(usval > usmaxval) usmaxval = usval;
                         }
                   }
                   else
                   {
                      for ( row = window.r0; row < row_max; row++ )
                         for ( col = window.c0; col < col_max; col++ )
                            if(gan_image_get_pix_b(mask, row, col))
                            {
                               usval = gan_image_get_pixptr_rgba_us(image, row, col)->A;
                               if(usval > usmaxval) usmaxval = usval;
                            }
                   }

                   maxval->data.gl.us = usmaxval;
                }
                break;

                case GAN_UINT:
                {
                   unsigned int uimaxval = 0, uival;

                   if(mask == NULL)
                   {
                      for ( row = window.r0; row < row_max; row++ )
                         for ( col = window.c0; col < col_max; col++ )
                         {
                            uival = gan_image_get_pixptr_rgba_ui(image, row, col)->A;
                            if(uival > uimaxval) uimaxval = uival;
                         }
                   }
                   else
                   {
                      for ( row = window.r0; row < row_max; row++ )
                         for ( col = window.c0; col < col_max; col++ )
                            if(gan_image_get_pix_b(mask, row, col))
                            {
                               uival = gan_image_get_pixptr_rgba_ui(image, row, col)->A;
                               if(uival > uimaxval) uimaxval = uival;
                            }
                   }

                   maxval->data.gl.ui = uimaxval;
                }
                break;

                case GAN_FLOAT:
                {
                   float fmaxval = 0, fval;

                   if(mask == NULL)
                   {
                      for ( row = window.r0; row < row_max; row++ )
                         for ( col = window.c0; col < col_max; col++ )
                         {
                            fval = gan_image_get_pixptr_rgba_f(image, row, col)->A;
                            if(fval > fmaxval) fmaxval = fval;
                         }
                   }
                   else
                   {
                      for ( row = window.r0; row < row_max; row++ )
                         for ( col = window.c0; col < col_max; col++ )
                            if(gan_image_get_pix_b(mask, row, col))
                            {
                               fval = gan_image_get_pixptr_rgba_f(image, row, col)->A;
                               if(fval > fmaxval) fmaxval = fval;
                            }
                   }

                   maxval->data.gl.f = fmaxval;
                }
                break;

                default:
                  gan_err_flush_trace();
                  gan_err_register ( "gan_image_get_maximum_channel_value", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
                  return GAN_FALSE;
             }
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_get_maximum_channel_value", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return GAN_FALSE;
        }
        break;

      case GAN_BGR_COLOUR_ALPHA_IMAGE:
        switch ( channel )
        {
           case GAN_ALPHA_CHANNEL:
             switch(image->type)
             {
                case GAN_UCHAR:
                {
                   unsigned char ucmaxval = 0, ucval;

                   if(mask == NULL)
                   {
                      for ( row = window.r0; row < row_max; row++ )
                         for ( col = window.c0; col < col_max; col++ )
                         {
                            ucval = gan_image_get_pixptr_bgra_uc(image, row, col)->A;
                            if(ucval > ucmaxval) ucmaxval = ucval;
                         }
                   }
                   else
                   {
                      for ( row = window.r0; row < row_max; row++ )
                         for ( col = window.c0; col < col_max; col++ )
                            if(gan_image_get_pix_b(mask, row, col))
                            {
                               ucval = gan_image_get_pixptr_bgra_uc(image, row, col)->A;
                               if(ucval > ucmaxval) ucmaxval = ucval;
                            }
                   }

                   maxval->data.gl.uc = ucmaxval;
                }
                break;

                case GAN_USHORT:
                {
                   unsigned short usmaxval = 0, usval;

                   if(mask == NULL)
                   {
                      for ( row = window.r0; row < row_max; row++ )
                         for ( col = window.c0; col < col_max; col++ )
                         {
                            usval = gan_image_get_pixptr_bgra_us(image, row, col)->A;
                            if(usval > usmaxval) usmaxval = usval;
                         }
                   }
                   else
                   {
                      for ( row = window.r0; row < row_max; row++ )
                         for ( col = window.c0; col < col_max; col++ )
                            if(gan_image_get_pix_b(mask, row, col))
                            {
                               usval = gan_image_get_pixptr_bgra_us(image, row, col)->A;
                               if(usval > usmaxval) usmaxval = usval;
                            }
                   }

                   maxval->data.gl.us = usmaxval;
                }
                break;

                case GAN_UINT:
                {
                   unsigned int uimaxval = 0, uival;

                   if(mask == NULL)
                   {
                      for ( row = window.r0; row < row_max; row++ )
                         for ( col = window.c0; col < col_max; col++ )
                         {
                            uival = gan_image_get_pixptr_bgra_ui(image, row, col)->A;
                            if(uival > uimaxval) uimaxval = uival;
                         }
                   }
                   else
                   {
                      for ( row = window.r0; row < row_max; row++ )
                         for ( col = window.c0; col < col_max; col++ )
                            if(gan_image_get_pix_b(mask, row, col))
                            {
                               uival = gan_image_get_pixptr_bgra_ui(image, row, col)->A;
                               if(uival > uimaxval) uimaxval = uival;
                            }
                   }

                   maxval->data.gl.ui = uimaxval;
                }
                break;

                case GAN_FLOAT:
                {
                   float fmaxval = 0, fval;

                   if(mask == NULL)
                   {
                      for ( row = window.r0; row < row_max; row++ )
                         for ( col = window.c0; col < col_max; col++ )
                         {
                            fval = gan_image_get_pixptr_bgra_f(image, row, col)->A;
                            if(fval > fmaxval) fmaxval = fval;
                         }
                   }
                   else
                   {
                      for ( row = window.r0; row < row_max; row++ )
                         for ( col = window.c0; col < col_max; col++ )
                            if(gan_image_get_pix_b(mask, row, col))
                            {
                               fval = gan_image_get_pixptr_bgra_f(image, row, col)->A;
                               if(fval > fmaxval) fmaxval = fval;
                            }
                   }

                   maxval->data.gl.f = fmaxval;
                }
                break;

                default:
                  gan_err_flush_trace();
                  gan_err_register ( "gan_image_get_maximum_channel_value", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
                  return GAN_FALSE;
             }
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_get_maximum_channel_value", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return GAN_FALSE;
        }
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_image_get_maximum_channel_value", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
        return GAN_FALSE;
   }

   return GAN_TRUE;
}

/**
 * \brief Thresholds a channel of an image
 * \param source The input image
 * \param mask Mask of pixels to test
 * \param channel The channel to extract from the image
 * \param threshold The threshold in the range [0..1]
 * \param dest Destination mask image
 *
 * Thresholds a specific channel of an image and returns a mask of pixels above the threshold.
 * If a \a mask is specified, all pixels in \a dest outside the mask are set to zero.
 *
 * \sa gan_image_threshold_channel_s().
 */
Gan_Image* gan_image_threshold_channel_q ( const Gan_Image *source,
                                           const Gan_Image *mask,
                                           Gan_ImageChannelType channel,
                                           float threshold,
                                           Gan_Image *dest )
{
   Gan_ImageWindow window;
   unsigned int row, col, row_max, col_max;
   Gan_Pixel pix_thres;

   if(source->width == 0 || source->height == 0)
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_image_threshold_channel_q", GAN_ERROR_NO_DATA, "" );
      return NULL;
   }

   gan_err_test_ptr ( mask == NULL || (mask->width == source->width && mask->height == source->height), "gan_image_threshold_channel_q", GAN_ERROR_INCOMPATIBLE, "" );

   if(mask == NULL)
   {
      window.r0 = window.c0 = 0;
      window.width = source->width;
      window.height = source->height;
   }
   else
   {
      if(!gan_image_get_active_subwindow_b ( mask, GAN_BIT_ALIGNMENT,
                                             &window ))
      {
         gan_err_register ( "gan_image_threshold_channel_q", GAN_ERROR_FAILURE, "" );
         return NULL;
      }
   }

   if(dest == NULL)
      dest = gan_image_alloc_b(source->height, source->width);
   else if (dest != (Gan_Image*)mask)
      dest = gan_image_set_b(dest, source->height, source->width);

   if(dest == NULL)
   {
      gan_err_register ( "gan_image_threshold_channel_q", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   if (dest != (Gan_Image*)mask)
      gan_image_fill_const_b(dest, GAN_TRUE);

   if(window.width == 0 || window.height == 0)
      return dest;

   col_max = window.c0 + window.width;
   row_max = window.r0 + window.height;

   pix_thres.format = GAN_GREY_LEVEL_IMAGE;
   pix_thres.type = GAN_FLOAT;
   pix_thres.data.gl.f = threshold;
   if(!gan_image_convert_pixel_i(&pix_thres, GAN_GREY_LEVEL_IMAGE, source->type))
   {
      gan_err_register ( "gan_image_threshold_channel_q", GAN_ERROR_FAILURE, "" );
      return NULL;
   }
   
   switch(source->format)
   {
      case GAN_RGB_COLOUR_ALPHA_IMAGE:
        switch ( channel )
        {
           case GAN_ALPHA_CHANNEL:
             switch(source->type)
             {
                case GAN_UCHAR:
                {
                   unsigned char ucval;

                   if(mask == NULL)
                   {
                      for ( row = window.r0; row < row_max; row++ )
                         for ( col = window.c0; col < col_max; col++ )
                         {
                            ucval = gan_image_get_pixptr_rgba_uc(source, row, col)->A;
                            if(ucval < pix_thres.data.gl.uc)
                               gan_image_set_pix_b(dest, row, col, GAN_FALSE);
                         }
                   }
                   else
                   {
                      for ( row = window.r0; row < row_max; row++ )
                         for ( col = window.c0; col < col_max; col++ )
                            if(gan_image_get_pix_b(mask, row, col))
                            {
                               ucval = gan_image_get_pixptr_rgba_uc(source, row, col)->A;
                               if(ucval < pix_thres.data.gl.uc)
                                  gan_image_set_pix_b(dest, row, col, GAN_FALSE);
                            }
                   }
                }
                break;

                case GAN_USHORT:
                {
                   unsigned short usval;

                   if(mask == NULL)
                   {
                      for ( row = window.r0; row < row_max; row++ )
                         for ( col = window.c0; col < col_max; col++ )
                         {
                            usval = gan_image_get_pixptr_rgba_us(source, row, col)->A;
                            if(usval < pix_thres.data.gl.us)
                               gan_image_set_pix_b(dest, row, col, GAN_FALSE);
                         }
                   }
                   else
                   {
                      for ( row = window.r0; row < row_max; row++ )
                         for ( col = window.c0; col < col_max; col++ )
                            if(gan_image_get_pix_b(mask, row, col))
                            {
                               usval = gan_image_get_pixptr_rgba_us(source, row, col)->A;
                               if(usval < pix_thres.data.gl.us)
                                  gan_image_set_pix_b(dest, row, col, GAN_FALSE);
                            }
                   }
                }
                break;

                case GAN_FLOAT:
                {
                   float fval;

                   if(mask == NULL)
                   {
                      for ( row = window.r0; row < row_max; row++ )
                         for ( col = window.c0; col < col_max; col++ )
                         {
                            fval = gan_image_get_pixptr_rgba_f(source, row, col)->A;
                            if(fval < pix_thres.data.gl.f)
                               gan_image_set_pix_b(dest, row, col, GAN_FALSE);
                         }
                   }
                   else
                   {
                      for ( row = window.r0; row < row_max; row++ )
                         for ( col = window.c0; col < col_max; col++ )
                            if(gan_image_get_pix_b(mask, row, col))
                            {
                               fval = gan_image_get_pixptr_rgba_f(source, row, col)->A;
                               if(fval < pix_thres.data.gl.f)
                                  gan_image_set_pix_b(dest, row, col, GAN_FALSE);
                            }
                   }
                }
                break;

                default:
                  gan_err_flush_trace();
                  gan_err_register ( "gan_image_threshold_channel_q", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
                  return NULL;
             }
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_threshold_channel_q", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;

      case GAN_BGR_COLOUR_ALPHA_IMAGE:
        switch ( channel )
        {
           case GAN_ALPHA_CHANNEL:
             switch(source->type)
             {
                case GAN_UCHAR:
                {
                   unsigned char ucval;

                   if(mask == NULL)
                   {
                      for ( row = window.r0; row < row_max; row++ )
                         for ( col = window.c0; col < col_max; col++ )
                         {
                            ucval = gan_image_get_pixptr_bgra_uc(source, row, col)->A;
                            if(ucval < pix_thres.data.gl.uc)
                               gan_image_set_pix_b(dest, row, col, GAN_FALSE);
                         }
                   }
                   else
                   {
                      for ( row = window.r0; row < row_max; row++ )
                         for ( col = window.c0; col < col_max; col++ )
                            if(gan_image_get_pix_b(mask, row, col))
                            {
                               ucval = gan_image_get_pixptr_bgra_uc(source, row, col)->A;
                               if(ucval < pix_thres.data.gl.uc)
                                  gan_image_set_pix_b(dest, row, col, GAN_FALSE);
                            }
                   }
                }
                break;

                case GAN_USHORT:
                {
                   unsigned short usval;

                   if(mask == NULL)
                   {
                      for ( row = window.r0; row < row_max; row++ )
                         for ( col = window.c0; col < col_max; col++ )
                         {
                            usval = gan_image_get_pixptr_bgra_us(source, row, col)->A;
                            if(usval < pix_thres.data.gl.us)
                               gan_image_set_pix_b(dest, row, col, GAN_FALSE);
                         }
                   }
                   else
                   {
                      for ( row = window.r0; row < row_max; row++ )
                         for ( col = window.c0; col < col_max; col++ )
                            if(gan_image_get_pix_b(mask, row, col))
                            {
                               usval = gan_image_get_pixptr_bgra_us(source, row, col)->A;
                               if(usval < pix_thres.data.gl.us)
                                  gan_image_set_pix_b(dest, row, col, GAN_FALSE);
                            }
                   }
                }
                break;

                case GAN_FLOAT:
                {
                   float fval;

                   if(mask == NULL)
                   {
                      for ( row = window.r0; row < row_max; row++ )
                         for ( col = window.c0; col < col_max; col++ )
                         {
                            fval = gan_image_get_pixptr_bgra_f(source, row, col)->A;
                            if(fval < pix_thres.data.gl.f)
                               gan_image_set_pix_b(dest, row, col, GAN_FALSE);
                         }
                   }
                   else
                   {
                      for ( row = window.r0; row < row_max; row++ )
                         for ( col = window.c0; col < col_max; col++ )
                            if(gan_image_get_pix_b(mask, row, col))
                            {
                               fval = gan_image_get_pixptr_bgra_f(source, row, col)->A;
                               if(fval < pix_thres.data.gl.f)
                                  gan_image_set_pix_b(dest, row, col, GAN_FALSE);
                            }
                   }
                }
                break;

                default:
                  gan_err_flush_trace();
                  gan_err_register ( "gan_image_threshold_channel_q", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
                  return NULL;
             }
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_threshold_channel_q", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_image_threshold_channel_q", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
        return NULL;
   }

   return dest;
}

/**
 * \}
 */

/**
 * \}
 */

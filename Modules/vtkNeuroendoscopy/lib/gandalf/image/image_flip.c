/**
 * File:          $RCSfile: image_flip.c,v $
 * Module:        Extract/fill selected channel(s) of image
 * Part of:       Gandalf Library
 *
 * Revision:      $Revision: 1.1.2.1 $
 * Last edited:   $Date: 2007/10/14 02:20:19 $
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

#include <gandalf/image/image_flip.h>
#include <gandalf/image/image_bit.h>
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
#include <gandalf/common/array.h>

/**
 * \addtogroup ImagePackage
 * \{
 */

/**
 * \defgroup ImageFlip Flip an Image vertically
 * \{
 */

/**
 * \brief Flips an image.
 * \param source The input image
 * \param interlaced Whether the source image is interlaced or not
 * \param dest The destination image
 * \return Pointer to the flipped image, or \c NULL on failure.
 *
 * Flips image \a source in \a dest. If \a bInterlaced is #GAN_TRUE the upper
 * and lower fields are flipped separately.
 *
 * \sa gan_image_flip_q().
 */
Gan_Image *
 gan_image_flip_q ( Gan_Image *source, Gan_Bool interlaced, Gan_Image *dest )
{
   int row, width=(int)source->width, height=(int)source->height;
   int evenH, oddH;

   /* interlaced frames must have a height which is a multiple of two */
   if ( interlaced && (source->height % 2) != 0 )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_image_flip_q", GAN_ERROR_INCOMPATIBLE, "" );
      return NULL;
   }

   /* calculate heights to use for computing rows */
   if ( interlaced )
   {
      evenH = height-2;
      oddH  = height;
   }
   else
      evenH = oddH = height-1;

   /* allocate/set output image */
   if ( dest == NULL )
   {
      dest = gan_image_copy_s ( source );
      if ( dest == NULL )
      {
         gan_err_register ( "gan_image_flip_q", GAN_ERROR_FAILURE, "" );
         return NULL;
      }
   }
   else if ( source != dest )
   {
      dest = gan_image_set_format_type_dims ( dest, source->format, source->type,
                                              source->height, source->width );
      if ( dest == NULL )
      {
         gan_err_register ( "gan_image_flip_q", GAN_ERROR_FAILURE, "" );
         return NULL;
      }

      dest->offset_x = source->offset_x;
      dest->offset_y = source->offset_y;
   }

   /* if the image is interlaced, the flipping is in-place and the height is
      not a multiple of four, the fields will be processed in a different
      order */
   if ( interlaced && (source == dest) && (source->height % 4) != 0 )
   {
      int tmp;

      tmp = evenH;
      evenH = oddH;
      oddH = tmp;
   }
      
   /* flip image */
   switch ( source->format )
   {
      case GAN_GREY_LEVEL_IMAGE:
        switch ( source->type )
        {
           case GAN_BOOL:
             if ( source == dest )
             {
                Gan_BitArray batmp;

                if ( gan_bit_array_form ( &batmp, source->width ) == NULL )
                {
                   gan_err_register ( "gan_image_flip_q", GAN_ERROR_FAILURE,
                                      "" );
                   return NULL;
                }

                for ( row = height/2-1; row >= 0; row -= 2 )
                {
                   if ( !gan_bit_array_copy_q ( &source->ba[row], &batmp ) ||
                        !gan_bit_array_copy_q ( &source->ba[oddH-row],
                                                &source->ba[row] ) ||
                        !gan_bit_array_copy_q ( &batmp,
                                                &source->ba[oddH-row] ))
                   {
                      gan_err_register ( "gan_image_flip_q",
                                         GAN_ERROR_FAILURE, "" );
                      return NULL;
                   }
                }
                
                for ( row = height/2-2; row >= 0; row -= 2 )
                {
                   if ( !gan_bit_array_copy_q ( &source->ba[row], &batmp ) ||
                        !gan_bit_array_copy_q ( &source->ba[evenH-row],
                                                &source->ba[row] ) ||
                        !gan_bit_array_copy_q ( &batmp,
                                                &source->ba[evenH-row] ))
                   {
                      gan_err_register ( "gan_image_flip_q",
                                         GAN_ERROR_FAILURE, "" );
                      return NULL;
                   }
                }
                
                gan_bit_array_free(&batmp);
             }
             else
             {
                for ( row = height-1; row >= 0; row -= 2 )
                   if ( !gan_bit_array_copy_q ( &source->ba[row],
                                                &dest->ba[oddH-row] ) )
                   {
                      gan_err_register ( "gan_image_flip_q", GAN_ERROR_FAILURE,
                                         "" );
                      return NULL;
                   }

                for ( row = height-2; row >= 0; row -= 2 )
                   if ( !gan_bit_array_copy_q ( &source->ba[row],
                                                &dest->ba[evenH-row] ) )
                   {
                      gan_err_register ( "gan_image_flip_q", GAN_ERROR_FAILURE,
                                         "" );
                      return NULL;
                   }
             }

             break;

           case GAN_UCHAR:
             if ( source == dest )
             {
                unsigned char *ucarr;

                ucarr = gan_malloc_array(unsigned char, width);
                if ( ucarr == NULL )
                {
                   gan_err_flush_trace();
                   gan_err_register_with_number ( "gan_image_flip_q", GAN_ERROR_MALLOC_FAILED, "", width*sizeof(unsigned char) );
                   return NULL;
                }

                for ( row = height/2-1; row >= 0; row -= 2 )
                {
                   gan_copy_array_uc (
                       gan_image_get_pixptr_gl_uc(source,row,0), 1,
                       source->width, ucarr, 1 );
                   gan_copy_array_uc (
                       gan_image_get_pixptr_gl_uc(source,oddH-row,0), 1,
                       source->width,
                       gan_image_get_pixptr_gl_uc(source,row,0), 1 );
                   gan_copy_array_uc (
                       ucarr, 1, source->width,
                       gan_image_get_pixptr_gl_uc(source,oddH-row,0), 1 );
                }
                                      
                for ( row = height/2-2; row >= 0; row -= 2 )
                {
                   gan_copy_array_uc (
                       gan_image_get_pixptr_gl_uc(source,row,0), 1,
                       source->width, ucarr, 1 );
                   gan_copy_array_uc (
                       gan_image_get_pixptr_gl_uc(source,evenH-row,0), 1,
                       source->width,
                       gan_image_get_pixptr_gl_uc(source,row,0), 1 );
                   gan_copy_array_uc (
                       ucarr, 1, source->width,
                       gan_image_get_pixptr_gl_uc(source,evenH-row,0), 1 );
                }
                                      
                free(ucarr);
             }
             else
             {
                for ( row = height-1; row >= 0; row -= 2 )
                   gan_copy_array_uc (
                       gan_image_get_pixptr_gl_uc(source,row,0), 1,
                       source->width,
                       gan_image_get_pixptr_gl_uc(dest,oddH-row,0), 1 );

                for ( row = height-2; row >= 0; row -= 2 )
                   gan_copy_array_uc (
                       gan_image_get_pixptr_gl_uc(source,row,0), 1,
                       source->width,
                       gan_image_get_pixptr_gl_uc(dest,evenH-row,0), 1 );
             }
             
             break;

           case GAN_USHORT:
             if ( source == dest )
             {
                unsigned short *usarr;

                usarr = gan_malloc_array(unsigned short, width);
                if ( usarr == NULL )
                {
                   gan_err_flush_trace();
                   gan_err_register_with_number ( "gan_image_flip_q", GAN_ERROR_MALLOC_FAILED, "", width*sizeof(unsigned short) );
                   return NULL;
                }

                for ( row = height/2-1; row >= 0; row -= 2 )
                {
                   gan_copy_array_us (
                       gan_image_get_pixptr_gl_us(source,row,0), 1,
                       source->width, usarr, 1 );
                   gan_copy_array_us (
                       gan_image_get_pixptr_gl_us(source,oddH-row,0), 1,
                       source->width,
                       gan_image_get_pixptr_gl_us(source,row,0), 1 );
                   gan_copy_array_us (
                       usarr, 1, source->width,
                       gan_image_get_pixptr_gl_us(source,oddH-row,0), 1 );
                }
                                      
                for ( row = height/2-2; row >= 0; row -= 2 )
                {
                   gan_copy_array_us (
                       gan_image_get_pixptr_gl_us(source,row,0), 1,
                       source->width, usarr, 1 );
                   gan_copy_array_us (
                       gan_image_get_pixptr_gl_us(source,evenH-row,0), 1,
                       source->width,
                       gan_image_get_pixptr_gl_us(source,row,0), 1 );
                   gan_copy_array_us (
                       usarr, 1, source->width,
                       gan_image_get_pixptr_gl_us(source,evenH-row,0), 1 );
                }
                                      
                free(usarr);
             }
             else
             {
                for ( row = height-1; row >= 0; row -= 2 )
                   gan_copy_array_us (
                       gan_image_get_pixptr_gl_us(source,row,0), 1,
                       source->width,
                       gan_image_get_pixptr_gl_us(dest,oddH-row,0), 1 );

                for ( row = height-2; row >= 0; row -= 2 )
                   gan_copy_array_us (
                       gan_image_get_pixptr_gl_us(source,row,0), 1,
                       source->width,
                       gan_image_get_pixptr_gl_us(dest,evenH-row,0), 1 );
             }
             
             break;

           case GAN_UINT:
             if ( source == dest )
             {
                unsigned int *uiarr;

                uiarr = gan_malloc_array(unsigned int, width);
                if ( uiarr == NULL )
                {
                   gan_err_flush_trace();
                   gan_err_register_with_number ( "gan_image_flip_q", GAN_ERROR_MALLOC_FAILED, "", width*sizeof(unsigned int) );
                   return NULL;
                }

                for ( row = height/2-1; row >= 0; row -= 2 )
                {
                   gan_copy_array_ui (
                       gan_image_get_pixptr_gl_ui(source,row,0), 1,
                       source->width, uiarr, 1 );
                   gan_copy_array_ui (
                       gan_image_get_pixptr_gl_ui(source,oddH-row,0), 1,
                       source->width,
                       gan_image_get_pixptr_gl_ui(source,row,0), 1 );
                   gan_copy_array_ui (
                       uiarr, 1, source->width,
                       gan_image_get_pixptr_gl_ui(source,oddH-row,0), 1 );
                }
                                      
                for ( row = height/2-2; row >= 0; row -= 2 )
                {
                   gan_copy_array_ui (
                       gan_image_get_pixptr_gl_ui(source,row,0), 1,
                       source->width, uiarr, 1 );
                   gan_copy_array_ui (
                       gan_image_get_pixptr_gl_ui(source,evenH-row,0), 1,
                       source->width,
                       gan_image_get_pixptr_gl_ui(source,row,0), 1 );
                   gan_copy_array_ui (
                       uiarr, 1, source->width,
                       gan_image_get_pixptr_gl_ui(source,evenH-row,0), 1 );
                }
                                      
                free(uiarr);
             }
             else
             {
                for ( row = height-1; row >= 0; row -= 2 )
                   gan_copy_array_ui (
                       gan_image_get_pixptr_gl_ui(source,row,0), 1,
                       source->width,
                       gan_image_get_pixptr_gl_ui(dest,oddH-row,0), 1 );

                for ( row = height-2; row >= 0; row -= 2 )
                   gan_copy_array_ui (
                       gan_image_get_pixptr_gl_ui(source,row,0), 1,
                       source->width,
                       gan_image_get_pixptr_gl_ui(dest,evenH-row,0), 1 );
             }

             break;

           case GAN_FLOAT:
             if ( source == dest )
             {
                float *farr;

                farr = gan_malloc_array(float, width);
                if ( farr == NULL )
                {
                   gan_err_flush_trace();
                   gan_err_register_with_number ( "gan_image_flip_q", GAN_ERROR_MALLOC_FAILED, "", width*sizeof(float) );
                   return NULL;
                }

                for ( row = height/2-1; row >= 0; row -= 2 )
                {
                   gan_copy_array_f (
                       gan_image_get_pixptr_gl_f(source,row,0), 1,
                       source->width, farr, 1 );
                   gan_copy_array_f (
                       gan_image_get_pixptr_gl_f(source,oddH-row,0), 1,
                       source->width,
                       gan_image_get_pixptr_gl_f(source,row,0), 1 );
                   gan_copy_array_f (
                       farr, 1, source->width,
                       gan_image_get_pixptr_gl_f(source,oddH-row,0), 1 );
                }
                                      
                for ( row = height/2-2; row >= 0; row -= 2 )
                {
                   gan_copy_array_f (
                       gan_image_get_pixptr_gl_f(source,row,0), 1,
                       source->width, farr, 1 );
                   gan_copy_array_f (
                       gan_image_get_pixptr_gl_f(source,evenH-row,0), 1,
                       source->width,
                       gan_image_get_pixptr_gl_f(source,row,0), 1 );
                   gan_copy_array_f (
                       farr, 1, source->width,
                       gan_image_get_pixptr_gl_f(source,evenH-row,0), 1 );
                }
                                      
                free(farr);
             }
             else
             {
                for ( row = height-1; row >= 0; row -= 2 )
                   gan_copy_array_f (
                       gan_image_get_pixptr_gl_f(source,row,0), 1,
                       source->width,
                       gan_image_get_pixptr_gl_f(dest,oddH-row,0), 1 );

                for ( row = height-2; row >= 0; row -= 2 )
                   gan_copy_array_f (
                       gan_image_get_pixptr_gl_f(source,row,0), 1,
                       source->width,
                       gan_image_get_pixptr_gl_f(dest,evenH-row,0), 1 );
             }

             break;

           case GAN_DOUBLE:
             if ( source == dest )
             {
                double *darr;

                darr = gan_malloc_array(double, width);
                if ( darr == NULL )
                {
                   gan_err_flush_trace();
                   gan_err_register_with_number ( "gan_image_flip_q", GAN_ERROR_MALLOC_FAILED, "", width*sizeof(double) );
                   return NULL;
                }

                for ( row = height/2-1; row >= 0; row -= 2 )
                {
                   gan_copy_array_d (
                       gan_image_get_pixptr_gl_d(source,row,0), 1,
                       source->width, darr, 1 );
                   gan_copy_array_d (
                       gan_image_get_pixptr_gl_d(source,oddH-row,0), 1,
                       source->width,
                       gan_image_get_pixptr_gl_d(source,row,0), 1 );
                   gan_copy_array_d (
                       darr, 1, source->width,
                       gan_image_get_pixptr_gl_d(source,oddH-row,0), 1 );
                }
                                      
                for ( row = height/2-2; row >= 0; row -= 2 )
                {
                   gan_copy_array_d (
                       gan_image_get_pixptr_gl_d(source,row,0), 1,
                       source->width, darr, 1 );
                   gan_copy_array_d (
                       gan_image_get_pixptr_gl_d(source,evenH-row,0), 1,
                       source->width,
                       gan_image_get_pixptr_gl_d(source,row,0), 1 );
                   gan_copy_array_d (
                       darr, 1, source->width,
                       gan_image_get_pixptr_gl_d(source,evenH-row,0), 1 );
                }
                                      
                free(darr);
             }
             else
             {
                for ( row = height-1; row >= 0; row -= 2 )
                   gan_copy_array_d (
                       gan_image_get_pixptr_gl_d(source,row,0), 1,
                       source->width,
                       gan_image_get_pixptr_gl_d(dest,oddH-row,0), 1 );

                for ( row = height-2; row >= 0; row -= 2 )
                   gan_copy_array_d (
                       gan_image_get_pixptr_gl_d(source,row,0), 1,
                       source->width,
                       gan_image_get_pixptr_gl_d(dest,evenH-row,0), 1 );
             }

             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_flip_q", GAN_ERROR_ILLEGAL_IMAGE_TYPE,
                                "" );
             return NULL;
        }

        break;

      case GAN_RGB_COLOUR_IMAGE:
        switch ( source->type )
        {
           case GAN_UCHAR:
             if ( source == dest )
             {
                Gan_RGBPixel_uc *rgbucarr;

                rgbucarr = gan_malloc_array(Gan_RGBPixel_uc, width);
                if ( rgbucarr == NULL )
                {
                   gan_err_flush_trace();
                   gan_err_register_with_number ( "gan_image_flip_q", GAN_ERROR_MALLOC_FAILED, "", width*sizeof(Gan_RGBPixel_uc) );
                   return NULL;
                }

                for ( row = height/2-1; row >= 0; row -= 2 )
                {
                   gan_copy_array_uc (
                       (unsigned char *)
                       gan_image_get_pixptr_rgb_uc(source,row,0), 1,
                       3*source->width, (unsigned char *)rgbucarr, 1 );
                   gan_copy_array_uc (
                       (unsigned char *)
                       gan_image_get_pixptr_rgb_uc(source,oddH-row,0), 1,
                       3*source->width, (unsigned char *)
                       gan_image_get_pixptr_rgb_uc(source,row,0), 1 );
                   gan_copy_array_uc (
                       (unsigned char *)rgbucarr, 1, 3*source->width,
                       (unsigned char *)
                       gan_image_get_pixptr_rgb_uc(source,oddH-row,0), 1 );
                }
                                      
                for ( row = height/2-2; row >= 0; row -= 2 )
                {
                   gan_copy_array_uc (
                       (unsigned char *)
                       gan_image_get_pixptr_rgb_uc(source,row,0), 1,
                       3*source->width, (unsigned char *)rgbucarr, 1 );
                   gan_copy_array_uc (
                       (unsigned char *)
                       gan_image_get_pixptr_rgb_uc(source,evenH-row,0), 1,
                       3*source->width, (unsigned char *)
                       gan_image_get_pixptr_rgb_uc(source,row,0), 1 );
                   gan_copy_array_uc (
                       (unsigned char *)rgbucarr, 1, 3*source->width,
                       (unsigned char *)
                       gan_image_get_pixptr_rgb_uc(source,evenH-row,0), 1 );
                }
                                      
                free(rgbucarr);
             }
             else
             {
                for ( row = height-1; row >= 0; row -= 2 )
                   gan_copy_array_uc (
                       (unsigned char *)
                       gan_image_get_pixptr_rgb_uc(source,row,0), 1,
                       3*source->width,
                       (unsigned char *)
                       gan_image_get_pixptr_rgb_uc(dest,oddH-row,0), 1 );

                for ( row = height-2; row >= 0; row -= 2 )
                   gan_copy_array_uc (
                       (unsigned char *)
                       gan_image_get_pixptr_rgb_uc(source,row,0), 1,
                       3*source->width,
                       (unsigned char *)
                       gan_image_get_pixptr_rgb_uc(dest,evenH-row,0), 1 );
             }

             break;

           case GAN_USHORT:
             if ( source == dest )
             {
                Gan_RGBPixel_us *rgbusarr;

                rgbusarr = gan_malloc_array(Gan_RGBPixel_us, width);
                if ( rgbusarr == NULL )
                {
                   gan_err_flush_trace();
                   gan_err_register_with_number ( "gan_image_flip_q", GAN_ERROR_MALLOC_FAILED, "", width*sizeof(Gan_RGBPixel_us) );
                   return NULL;
                }

                for ( row = height/2-1; row >= 0; row -= 2 )
                {
                   gan_copy_array_us (
                       (unsigned short *)
                       gan_image_get_pixptr_rgb_us(source,row,0), 1,
                       3*source->width, (unsigned short *)rgbusarr, 1 );
                   gan_copy_array_us (
                       (unsigned short *)
                       gan_image_get_pixptr_rgb_us(source,oddH-row,0), 1,
                       3*source->width, (unsigned short *)
                       gan_image_get_pixptr_rgb_us(source,row,0), 1 );
                   gan_copy_array_us (
                       (unsigned short *)rgbusarr, 1, 3*source->width,
                       (unsigned short *)
                       gan_image_get_pixptr_rgb_us(source,oddH-row,0), 1 );
                }
                                      
                for ( row = height/2-2; row >= 0; row -= 2 )
                {
                   gan_copy_array_us (
                       (unsigned short *)
                       gan_image_get_pixptr_rgb_us(source,row,0), 1,
                       3*source->width, (unsigned short *)rgbusarr, 1 );
                   gan_copy_array_us (
                       (unsigned short *)
                       gan_image_get_pixptr_rgb_us(source,evenH-row,0), 1,
                       3*source->width, (unsigned short *)
                       gan_image_get_pixptr_rgb_us(source,row,0), 1 );
                   gan_copy_array_us (
                       (unsigned short *)rgbusarr, 1, 3*source->width,
                       (unsigned short *)
                       gan_image_get_pixptr_rgb_us(source,evenH-row,0), 1 );
                }
                                      
                free(rgbusarr);
             }
             else
             {
                for ( row = height-1; row >= 0; row -= 2 )
                   gan_copy_array_us (
                       (unsigned short *)
                       gan_image_get_pixptr_rgb_us(source,row,0), 1,
                       3*source->width,
                       (unsigned short *)
                       gan_image_get_pixptr_rgb_us(dest,oddH-row,0), 1 );

                for ( row = height-2; row >= 0; row -= 2 )
                   gan_copy_array_us (
                       (unsigned short *)
                       gan_image_get_pixptr_rgb_us(source,row,0), 1,
                       3*source->width,
                       (unsigned short *)
                       gan_image_get_pixptr_rgb_us(dest,evenH-row,0), 1 );
             }

             break;

           case GAN_UINT:
             if ( source == dest )
             {
                Gan_RGBPixel_ui *rgbuiarr;

                rgbuiarr = gan_malloc_array(Gan_RGBPixel_ui, width);
                if ( rgbuiarr == NULL )
                {
                   gan_err_flush_trace();
                   gan_err_register_with_number ( "gan_image_flip_q", GAN_ERROR_MALLOC_FAILED, "", width*sizeof(Gan_RGBPixel_ui) );
                   return NULL;
                }

                for ( row = height/2-1; row >= 0; row -= 2 )
                {
                   gan_copy_array_ui (
                       (unsigned int *)
                       gan_image_get_pixptr_rgb_ui(source,row,0), 1,
                       3*source->width, (unsigned int *)rgbuiarr, 1 );
                   gan_copy_array_ui (
                       (unsigned int *)
                       gan_image_get_pixptr_rgb_ui(source,oddH-row,0), 1,
                       3*source->width, (unsigned int *)
                       gan_image_get_pixptr_rgb_ui(source,row,0), 1 );
                   gan_copy_array_ui (
                       (unsigned int *)rgbuiarr, 1, 3*source->width,
                       (unsigned int *)
                       gan_image_get_pixptr_rgb_ui(source,oddH-row,0), 1 );
                }
                                      
                for ( row = height/2-2; row >= 0; row -= 2 )
                {
                   gan_copy_array_ui (
                       (unsigned int *)
                       gan_image_get_pixptr_rgb_ui(source,row,0), 1,
                       3*source->width, (unsigned int *)rgbuiarr, 1 );
                   gan_copy_array_ui (
                       (unsigned int *)
                       gan_image_get_pixptr_rgb_ui(source,evenH-row,0), 1,
                       3*source->width, (unsigned int *)
                       gan_image_get_pixptr_rgb_ui(source,row,0), 1 );
                   gan_copy_array_ui (
                       (unsigned int *)rgbuiarr, 1, 3*source->width,
                       (unsigned int *)
                       gan_image_get_pixptr_rgb_ui(source,evenH-row,0), 1 );
                }
                                      
                free(rgbuiarr);
             }
             else
             {
                for ( row = height-1; row >= 0; row -= 2 )
                   gan_copy_array_ui (
                       (unsigned int *)
                       gan_image_get_pixptr_rgb_ui(source,row,0), 1,
                       3*source->width,
                       (unsigned int *)
                       gan_image_get_pixptr_rgb_ui(dest,oddH-row,0), 1 );

                for ( row = height-2; row >= 0; row -= 2 )
                   gan_copy_array_ui (
                       (unsigned int *)
                       gan_image_get_pixptr_rgb_ui(source,row,0), 1,
                       3*source->width,
                       (unsigned int *)
                       gan_image_get_pixptr_rgb_ui(dest,evenH-row,0), 1 );
             }

             break;

           case GAN_FLOAT:
             if ( source == dest )
             {
                Gan_RGBPixel_f *rgbfarr;

                rgbfarr = gan_malloc_array(Gan_RGBPixel_f, width);
                if ( rgbfarr == NULL )
                {
                   gan_err_flush_trace();
                   gan_err_register_with_number ( "gan_image_flip_q", GAN_ERROR_MALLOC_FAILED, "", width*sizeof(Gan_RGBPixel_f) );
                   return NULL;
                }

                for ( row = height/2-1; row >= 0; row -= 2 )
                {
                   gan_copy_array_f (
                       (float *)
                       gan_image_get_pixptr_rgb_f(source,row,0), 1,
                       3*source->width, (float *)rgbfarr, 1 );
                   gan_copy_array_f (
                       (float *)
                       gan_image_get_pixptr_rgb_f(source,oddH-row,0), 1,
                       3*source->width, (float *)
                       gan_image_get_pixptr_rgb_f(source,row,0), 1 );
                   gan_copy_array_f (
                       (float *)rgbfarr, 1, 3*source->width,
                       (float *)
                       gan_image_get_pixptr_rgb_f(source,oddH-row,0), 1 );
                }
                                      
                for ( row = height/2-2; row >= 0; row -= 2 )
                {
                   gan_copy_array_f (
                       (float *)
                       gan_image_get_pixptr_rgb_f(source,row,0), 1,
                       3*source->width, (float *)rgbfarr, 1 );
                   gan_copy_array_f (
                       (float *)
                       gan_image_get_pixptr_rgb_f(source,evenH-row,0), 1,
                       3*source->width, (float *)
                       gan_image_get_pixptr_rgb_f(source,row,0), 1 );
                   gan_copy_array_f (
                       (float *)rgbfarr, 1, 3*source->width,
                       (float *)
                       gan_image_get_pixptr_rgb_f(source,evenH-row,0), 1 );
                }
                                      
                free(rgbfarr);
             }
             else
             {
                for ( row = height-1; row >= 0; row -= 2 )
                   gan_copy_array_f (
                       (float *)
                       gan_image_get_pixptr_rgb_f(source,row,0), 1,
                       3*source->width,
                       (float *)
                       gan_image_get_pixptr_rgb_f(dest,oddH-row,0), 1 );

                for ( row = height-2; row >= 0; row -= 2 )
                   gan_copy_array_f (
                       (float *)
                       gan_image_get_pixptr_rgb_f(source,row,0), 1,
                       3*source->width,
                       (float *)
                       gan_image_get_pixptr_rgb_f(dest,evenH-row,0), 1 );
             }

             break;

           case GAN_DOUBLE:
             if ( source == dest )
             {
                Gan_RGBPixel_d *rgbdarr;

                rgbdarr = gan_malloc_array(Gan_RGBPixel_d, width);
                if ( rgbdarr == NULL )
                {
                   gan_err_flush_trace();
                   gan_err_register_with_number ( "gan_image_flip_q", GAN_ERROR_MALLOC_FAILED, "", width*sizeof(Gan_RGBPixel_d) );
                   return NULL;
                }

                for ( row = height/2-1; row >= 0; row -= 2 )
                {
                   gan_copy_array_d (
                       (double *)
                       gan_image_get_pixptr_rgb_d(source,row,0), 1,
                       3*source->width, (double *)rgbdarr, 1 );
                   gan_copy_array_d (
                       (double *)
                       gan_image_get_pixptr_rgb_d(source,oddH-row,0), 1,
                       3*source->width, (double *)
                       gan_image_get_pixptr_rgb_d(source,row,0), 1 );
                   gan_copy_array_d (
                       (double *)rgbdarr, 1, 3*source->width,
                       (double *)
                       gan_image_get_pixptr_rgb_d(source,oddH-row,0), 1 );
                }
                                      
                for ( row = height/2-2; row >= 0; row -= 2 )
                {
                   gan_copy_array_d (
                       (double *)
                       gan_image_get_pixptr_rgb_d(source,row,0), 1,
                       3*source->width, (double *)rgbdarr, 1 );
                   gan_copy_array_d (
                       (double *)
                       gan_image_get_pixptr_rgb_d(source,evenH-row,0), 1,
                       3*source->width, (double *)
                       gan_image_get_pixptr_rgb_d(source,row,0), 1 );
                   gan_copy_array_d (
                       (double *)rgbdarr, 1, 3*source->width,
                       (double *)
                       gan_image_get_pixptr_rgb_d(source,evenH-row,0), 1 );
                }
                                      
                free(rgbdarr);
             }
             else
             {
                for ( row = height-1; row >= 0; row -= 2 )
                   gan_copy_array_d (
                       (double *)
                       gan_image_get_pixptr_rgb_d(source,row,0), 1,
                       3*source->width,
                       (double *)
                       gan_image_get_pixptr_rgb_d(dest,oddH-row,0), 1 );

                for ( row = height-2; row >= 0; row -= 2 )
                   gan_copy_array_d (
                       (double *)
                       gan_image_get_pixptr_rgb_d(source,row,0), 1,
                       3*source->width,
                       (double *)
                       gan_image_get_pixptr_rgb_d(dest,evenH-row,0), 1 );
             }

             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_flip_q", GAN_ERROR_ILLEGAL_IMAGE_TYPE,
                                "" );
             return NULL;
        }

        break;

      case GAN_RGB_COLOUR_ALPHA_IMAGE:
        switch ( source->type )
        {
           case GAN_UCHAR:
             if ( source == dest )
             {
                Gan_RGBAPixel_uc *rgbaucarr;

                rgbaucarr = gan_malloc_array(Gan_RGBAPixel_uc, width);
                if ( rgbaucarr == NULL )
                {
                   gan_err_flush_trace();
                   gan_err_register_with_number ( "gan_image_flip_q", GAN_ERROR_MALLOC_FAILED, "", width*sizeof(Gan_RGBAPixel_uc) );
                   return NULL;
                }

                for ( row = height/2-1; row >= 0; row -= 2 )
                {
                   gan_copy_array_uc (
                      (unsigned char *)
                      gan_image_get_pixptr_rgba_uc(source,row,0), 1,
                      4*source->width, (unsigned char *)rgbaucarr, 1 );
                   gan_copy_array_uc (
                      (unsigned char *)
                      gan_image_get_pixptr_rgba_uc(source,oddH-row,0), 1,
                      4*source->width, (unsigned char *)
                      gan_image_get_pixptr_rgba_uc(source,row,0), 1 );
                   gan_copy_array_uc (
                      (unsigned char *)rgbaucarr, 1, 4*source->width,
                      (unsigned char *)
                      gan_image_get_pixptr_rgba_uc(source,oddH-row,0), 1 );
                }
                                      
                for ( row = height/2-2; row >= 0; row -= 2 )
                {
                   gan_copy_array_uc (
                      (unsigned char *)
                      gan_image_get_pixptr_rgba_uc(source,row,0), 1,
                      4*source->width, (unsigned char *)rgbaucarr, 1 );
                   gan_copy_array_uc (
                      (unsigned char *)
                      gan_image_get_pixptr_rgba_uc(source,evenH-row,0), 1,
                      4*source->width, (unsigned char *)
                      gan_image_get_pixptr_rgba_uc(source,row,0), 1 );
                   gan_copy_array_uc (
                      (unsigned char *)rgbaucarr, 1, 4*source->width,
                      (unsigned char *)
                      gan_image_get_pixptr_rgba_uc(source,evenH-row,0), 1 );
                }
                                      
                free(rgbaucarr);
             }
             else
             {
                for ( row = height-1; row >= 0; row -= 2 )
                   gan_copy_array_uc (
                       (unsigned char *)
                       gan_image_get_pixptr_rgba_uc(source,row,0), 1,
                       4*source->width,
                       (unsigned char *)
                       gan_image_get_pixptr_rgba_uc(dest,oddH-row,0), 1 );

                for ( row = height-2; row >= 0; row -= 2 )
                   gan_copy_array_uc (
                       (unsigned char *)
                       gan_image_get_pixptr_rgba_uc(source,row,0), 1,
                       4*source->width,
                       (unsigned char *)
                       gan_image_get_pixptr_rgba_uc(dest,evenH-row,0), 1 );
             }

             break;

           case GAN_USHORT:
             if ( source == dest )
             {
                Gan_RGBAPixel_us *rgbausarr;

                rgbausarr = gan_malloc_array(Gan_RGBAPixel_us, width);
                if ( rgbausarr == NULL )
                {
                   gan_err_flush_trace();
                   gan_err_register_with_number ( "gan_image_flip_q", GAN_ERROR_MALLOC_FAILED, "", width*sizeof(Gan_RGBAPixel_us) );
                   return NULL;
                }

                for ( row = height/2-1; row >= 0; row -= 2 )
                {
                   gan_copy_array_us (
                       (unsigned short *)
                       gan_image_get_pixptr_rgba_us(source,row,0), 1,
                       4*source->width, (unsigned short *)rgbausarr, 1 );
                   gan_copy_array_us (
                       (unsigned short *)
                       gan_image_get_pixptr_rgba_us(source,oddH-row,0), 1,
                       4*source->width, (unsigned short *)
                       gan_image_get_pixptr_rgba_us(source,row,0), 1 );
                   gan_copy_array_us (
                       (unsigned short *)rgbausarr, 1, 4*source->width,
                       (unsigned short *)
                       gan_image_get_pixptr_rgba_us(source,oddH-row,0), 1);
                }
                                      
                for ( row = height/2-2; row >= 0; row -= 2 )
                {
                   gan_copy_array_us (
                       (unsigned short *)
                       gan_image_get_pixptr_rgba_us(source,row,0), 1,
                       4*source->width, (unsigned short *)rgbausarr, 1 );
                   gan_copy_array_us (
                       (unsigned short *)
                       gan_image_get_pixptr_rgba_us(source,evenH-row,0), 1,
                       4*source->width, (unsigned short *)
                       gan_image_get_pixptr_rgba_us(source,row,0), 1 );
                   gan_copy_array_us (
                       (unsigned short *)rgbausarr, 1, 4*source->width,
                       (unsigned short *)
                       gan_image_get_pixptr_rgba_us(source,evenH-row,0), 1);
                }
                                      
                free(rgbausarr);
             }
             else
             {
                for ( row = height-1; row >= 0; row -= 2 )
                   gan_copy_array_us (
                       (unsigned short *)
                       gan_image_get_pixptr_rgba_us(source,row,0), 1,
                       4*source->width,
                       (unsigned short *)
                       gan_image_get_pixptr_rgba_us(dest,oddH-row,0), 1 );

                for ( row = height-2; row >= 0; row -= 2 )
                   gan_copy_array_us (
                       (unsigned short *)
                       gan_image_get_pixptr_rgba_us(source,row,0), 1,
                       4*source->width,
                       (unsigned short *)
                       gan_image_get_pixptr_rgba_us(dest,evenH-row,0), 1 );
             }

             break;

           case GAN_UINT:
             if ( source == dest )
             {
                Gan_RGBAPixel_ui *rgbauiarr;

                rgbauiarr = gan_malloc_array(Gan_RGBAPixel_ui, width);
                if ( rgbauiarr == NULL )
                {
                   gan_err_flush_trace();
                   gan_err_register_with_number ( "gan_image_flip_q", GAN_ERROR_MALLOC_FAILED, "", width*sizeof(Gan_RGBAPixel_ui) );
                   return NULL;
                }

                for ( row = height/2-1; row >= 0; row -= 2 )
                {
                   gan_copy_array_ui (
                       (unsigned int *)
                       gan_image_get_pixptr_rgba_ui(source,row,0), 1,
                       4*source->width, (unsigned int *)rgbauiarr, 1 );
                   gan_copy_array_ui (
                       (unsigned int *)
                       gan_image_get_pixptr_rgba_ui(source,oddH-row,0), 1,
                       4*source->width, (unsigned int *)
                       gan_image_get_pixptr_rgba_ui(source,row,0), 1 );
                   gan_copy_array_ui (
                       (unsigned int *)rgbauiarr, 1, 4*source->width,
                       (unsigned int *)
                       gan_image_get_pixptr_rgba_ui(source,oddH-row,0), 1);
                }
                                      
                for ( row = height/2-2; row >= 0; row -= 2 )
                {
                   gan_copy_array_ui (
                       (unsigned int *)
                       gan_image_get_pixptr_rgba_ui(source,row,0), 1,
                       4*source->width, (unsigned int *)rgbauiarr, 1 );
                   gan_copy_array_ui (
                       (unsigned int *)
                       gan_image_get_pixptr_rgba_ui(source,evenH-row,0), 1,
                       4*source->width, (unsigned int *)
                       gan_image_get_pixptr_rgba_ui(source,row,0), 1 );
                   gan_copy_array_ui (
                       (unsigned int *)rgbauiarr, 1, 4*source->width,
                       (unsigned int *)
                       gan_image_get_pixptr_rgba_ui(source,evenH-row,0), 1);
                }
                                      
                free(rgbauiarr);
             }
             else
             {
                for ( row = height-1; row >= 0; row -= 2 )
                   gan_copy_array_ui (
                       (unsigned int *)
                       gan_image_get_pixptr_rgba_ui(source,row,0), 1,
                       4*source->width,
                       (unsigned int *)
                       gan_image_get_pixptr_rgba_ui(dest,oddH-row,0), 1 );

                for ( row = height-2; row >= 0; row -= 2 )
                   gan_copy_array_ui (
                       (unsigned int *)
                       gan_image_get_pixptr_rgba_ui(source,row,0), 1,
                       4*source->width,
                       (unsigned int *)
                       gan_image_get_pixptr_rgba_ui(dest,evenH-row,0), 1 );
             }

             break;

           case GAN_FLOAT:
             if ( source == dest )
             {
                Gan_RGBAPixel_f *rgbafarr;

                rgbafarr = gan_malloc_array(Gan_RGBAPixel_f, width);
                if ( rgbafarr == NULL )
                {
                   gan_err_flush_trace();
                   gan_err_register_with_number ( "gan_image_flip_q", GAN_ERROR_MALLOC_FAILED, "", width*sizeof(Gan_RGBAPixel_f) );
                   return NULL;
                }

                for ( row = height/2-1; row >= 0; row -= 2 )
                {
                   gan_copy_array_f (
                       (float *)
                       gan_image_get_pixptr_rgba_f(source,row,0), 1,
                       4*source->width, (float *)rgbafarr, 1 );
                   gan_copy_array_f (
                       (float *)
                       gan_image_get_pixptr_rgba_f(source,oddH-row,0), 1,
                       4*source->width, (float *)
                       gan_image_get_pixptr_rgba_f(source,row,0), 1 );
                   gan_copy_array_f (
                       (float *)rgbafarr, 1, 4*source->width,
                       (float *)
                       gan_image_get_pixptr_rgba_f(source,oddH-row,0), 1 );
                }
                                      
                for ( row = height/2-2; row >= 0; row -= 2 )
                {
                   gan_copy_array_f (
                       (float *)
                       gan_image_get_pixptr_rgba_f(source,row,0), 1,
                       4*source->width, (float *)rgbafarr, 1 );
                   gan_copy_array_f (
                       (float *)
                       gan_image_get_pixptr_rgba_f(source,evenH-row,0), 1,
                       4*source->width, (float *)
                       gan_image_get_pixptr_rgba_f(source,row,0), 1 );
                   gan_copy_array_f (
                       (float *)rgbafarr, 1, 4*source->width,
                       (float *)
                       gan_image_get_pixptr_rgba_f(source,evenH-row,0), 1 );
                }
                                      
                free(rgbafarr);
             }
             else
             {
                for ( row = height-1; row >= 0; row -= 2 )
                   gan_copy_array_f (
                       (float *)
                       gan_image_get_pixptr_rgba_f(source,row,0), 1,
                       4*source->width,
                       (float *)
                       gan_image_get_pixptr_rgba_f(dest,oddH-row,0), 1 );

                for ( row = height-2; row >= 0; row -= 2 )
                   gan_copy_array_f (
                       (float *)
                       gan_image_get_pixptr_rgba_f(source,row,0), 1,
                       4*source->width,
                       (float *)
                       gan_image_get_pixptr_rgba_f(dest,evenH-row,0), 1 );
             }

             break;

           case GAN_DOUBLE:
             if ( source == dest )
             {
                Gan_RGBAPixel_d *rgbadarr;

                rgbadarr = gan_malloc_array(Gan_RGBAPixel_d, width);
                if ( rgbadarr == NULL )
                {
                   gan_err_flush_trace();
                   gan_err_register_with_number ( "gan_image_flip_q", GAN_ERROR_MALLOC_FAILED, "", width*sizeof(Gan_RGBAPixel_d) );
                   return NULL;
                }

                for ( row = height/2-1; row >= 0; row -= 2 )
                {
                   gan_copy_array_d (
                       (double *)
                       gan_image_get_pixptr_rgba_d(source,row,0), 1,
                       4*source->width, (double *)rgbadarr, 1 );
                   gan_copy_array_d (
                       (double *)
                       gan_image_get_pixptr_rgba_d(source,oddH-row,0), 1,
                       4*source->width, (double *)
                       gan_image_get_pixptr_rgba_d(source,row,0), 1 );
                   gan_copy_array_d (
                       (double *)rgbadarr, 1, 4*source->width,
                       (double *)
                       gan_image_get_pixptr_rgba_d(source,oddH-row,0), 1 );
                }
                                      
                for ( row = height/2-2; row >= 0; row -= 2 )
                {
                   gan_copy_array_d (
                       (double *)
                       gan_image_get_pixptr_rgba_d(source,row,0), 1,
                       4*source->width, (double *)rgbadarr, 1 );
                   gan_copy_array_d (
                       (double *)
                       gan_image_get_pixptr_rgba_d(source,evenH-row,0), 1,
                       4*source->width, (double *)
                       gan_image_get_pixptr_rgba_d(source,row,0), 1 );
                   gan_copy_array_d (
                       (double *)rgbadarr, 1, 4*source->width,
                       (double *)
                       gan_image_get_pixptr_rgba_d(source,evenH-row,0), 1 );
                }
                                      
                free(rgbadarr);
             }
             else
             {
                for ( row = height-1; row >= 0; row -= 2 )
                   gan_copy_array_d (
                       (double *)
                       gan_image_get_pixptr_rgba_d(source,row,0), 1,
                       4*source->width,
                       (double *)
                       gan_image_get_pixptr_rgba_d(dest,oddH-row,0), 1 );

                for ( row = height-2; row >= 0; row -= 2 )
                   gan_copy_array_d (
                       (double *)
                       gan_image_get_pixptr_rgba_d(source,row,0), 1,
                       4*source->width,
                       (double *)
                       gan_image_get_pixptr_rgba_d(dest,evenH-row,0), 1 );
             }

             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_flip_q", GAN_ERROR_ILLEGAL_IMAGE_TYPE,
                                "" );
             return NULL;
        }

        break;

      case GAN_BGR_COLOUR_IMAGE:
        switch ( source->type )
        {
           case GAN_UCHAR:
             if ( source == dest )
             {
                Gan_BGRPixel_uc *bgrucarr;

                bgrucarr = gan_malloc_array(Gan_BGRPixel_uc, width);
                if ( bgrucarr == NULL )
                {
                   gan_err_flush_trace();
                   gan_err_register_with_number ( "gan_image_flip_q", GAN_ERROR_MALLOC_FAILED, "", width*sizeof(Gan_BGRPixel_uc) );
                   return NULL;
                }

                for ( row = height/2-1; row >= 0; row -= 2 )
                {
                   gan_copy_array_uc (
                       (unsigned char *)
                       gan_image_get_pixptr_bgr_uc(source,row,0), 1,
                       3*source->width, (unsigned char *)bgrucarr, 1 );
                   gan_copy_array_uc (
                       (unsigned char *)
                       gan_image_get_pixptr_bgr_uc(source,oddH-row,0), 1,
                       3*source->width, (unsigned char *)
                       gan_image_get_pixptr_bgr_uc(source,row,0), 1 );
                   gan_copy_array_uc (
                       (unsigned char *)bgrucarr, 1, 3*source->width,
                       (unsigned char *)
                       gan_image_get_pixptr_bgr_uc(source,oddH-row,0), 1 );
                }
                                      
                for ( row = height/2-2; row >= 0; row -= 2 )
                {
                   gan_copy_array_uc (
                       (unsigned char *)
                       gan_image_get_pixptr_bgr_uc(source,row,0), 1,
                       3*source->width, (unsigned char *)bgrucarr, 1 );
                   gan_copy_array_uc (
                       (unsigned char *)
                       gan_image_get_pixptr_bgr_uc(source,evenH-row,0), 1,
                       3*source->width, (unsigned char *)
                       gan_image_get_pixptr_bgr_uc(source,row,0), 1 );
                   gan_copy_array_uc (
                       (unsigned char *)bgrucarr, 1, 3*source->width,
                       (unsigned char *)
                       gan_image_get_pixptr_bgr_uc(source,evenH-row,0), 1 );
                }
                                      
                free(bgrucarr);
             }
             else
             {
                for ( row = height-1; row >= 0; row -= 2 )
                   gan_copy_array_uc (
                       (unsigned char *)
                       gan_image_get_pixptr_bgr_uc(source,row,0), 1,
                       3*source->width,
                       (unsigned char *)
                       gan_image_get_pixptr_bgr_uc(dest,oddH-row,0), 1 );

                for ( row = height-2; row >= 0; row -= 2 )
                   gan_copy_array_uc (
                       (unsigned char *)
                       gan_image_get_pixptr_bgr_uc(source,row,0), 1,
                       3*source->width,
                       (unsigned char *)
                       gan_image_get_pixptr_bgr_uc(dest,evenH-row,0), 1 );
             }

             break;

           case GAN_USHORT:
             if ( source == dest )
             {
                Gan_BGRPixel_us *bgrusarr;

                bgrusarr = gan_malloc_array(Gan_BGRPixel_us, width);
                if ( bgrusarr == NULL )
                {
                   gan_err_flush_trace();
                   gan_err_register_with_number ( "gan_image_flip_q", GAN_ERROR_MALLOC_FAILED, "", width*sizeof(Gan_BGRPixel_us) );
                   return NULL;
                }

                for ( row = height/2-1; row >= 0; row -= 2 )
                {
                   gan_copy_array_us (
                       (unsigned short *)
                       gan_image_get_pixptr_bgr_us(source,row,0), 1,
                       3*source->width, (unsigned short *)bgrusarr, 1 );
                   gan_copy_array_us (
                       (unsigned short *)
                       gan_image_get_pixptr_bgr_us(source,oddH-row,0), 1,
                       3*source->width, (unsigned short *)
                       gan_image_get_pixptr_bgr_us(source,row,0), 1 );
                   gan_copy_array_us (
                       (unsigned short *)bgrusarr, 1, 3*source->width,
                       (unsigned short *)
                       gan_image_get_pixptr_bgr_us(source,oddH-row,0), 1 );
                }
                                      
                for ( row = height/2-2; row >= 0; row -= 2 )
                {
                   gan_copy_array_us (
                       (unsigned short *)
                       gan_image_get_pixptr_bgr_us(source,row,0), 1,
                       3*source->width, (unsigned short *)bgrusarr, 1 );
                   gan_copy_array_us (
                       (unsigned short *)
                       gan_image_get_pixptr_bgr_us(source,evenH-row,0), 1,
                       3*source->width, (unsigned short *)
                       gan_image_get_pixptr_bgr_us(source,row,0), 1 );
                   gan_copy_array_us (
                       (unsigned short *)bgrusarr, 1, 3*source->width,
                       (unsigned short *)
                       gan_image_get_pixptr_bgr_us(source,evenH-row,0), 1 );
                }
                                      
                free(bgrusarr);
             }
             else
             {
                for ( row = height-1; row >= 0; row -= 2 )
                   gan_copy_array_us (
                       (unsigned short *)
                       gan_image_get_pixptr_bgr_us(source,row,0), 1,
                       3*source->width,
                       (unsigned short *)
                       gan_image_get_pixptr_bgr_us(dest,oddH-row,0), 1 );

                for ( row = height-2; row >= 0; row -= 2 )
                   gan_copy_array_us (
                       (unsigned short *)
                       gan_image_get_pixptr_bgr_us(source,row,0), 1,
                       3*source->width,
                       (unsigned short *)
                       gan_image_get_pixptr_bgr_us(dest,evenH-row,0), 1 );
             }

             break;

           case GAN_UINT:
             if ( source == dest )
             {
                Gan_BGRPixel_ui *bgruiarr;

                bgruiarr = gan_malloc_array(Gan_BGRPixel_ui, width);
                if ( bgruiarr == NULL )
                {
                   gan_err_flush_trace();
                   gan_err_register_with_number ( "gan_image_flip_q", GAN_ERROR_MALLOC_FAILED, "", width*sizeof(Gan_BGRPixel_ui) );
                   return NULL;
                }

                for ( row = height/2-1; row >= 0; row -= 2 )
                {
                   gan_copy_array_ui (
                       (unsigned int *)
                       gan_image_get_pixptr_bgr_ui(source,row,0), 1,
                       3*source->width, (unsigned int *)bgruiarr, 1 );
                   gan_copy_array_ui (
                       (unsigned int *)
                       gan_image_get_pixptr_bgr_ui(source,oddH-row,0), 1,
                       3*source->width, (unsigned int *)
                       gan_image_get_pixptr_bgr_ui(source,row,0), 1 );
                   gan_copy_array_ui (
                       (unsigned int *)bgruiarr, 1, 3*source->width,
                       (unsigned int *)
                       gan_image_get_pixptr_bgr_ui(source,oddH-row,0), 1 );
                }
                                      
                for ( row = height/2-2; row >= 0; row -= 2 )
                {
                   gan_copy_array_ui (
                       (unsigned int *)
                       gan_image_get_pixptr_bgr_ui(source,row,0), 1,
                       3*source->width, (unsigned int *)bgruiarr, 1 );
                   gan_copy_array_ui (
                       (unsigned int *)
                       gan_image_get_pixptr_bgr_ui(source,evenH-row,0), 1,
                       3*source->width, (unsigned int *)
                       gan_image_get_pixptr_bgr_ui(source,row,0), 1 );
                   gan_copy_array_ui (
                       (unsigned int *)bgruiarr, 1, 3*source->width,
                       (unsigned int *)
                       gan_image_get_pixptr_bgr_ui(source,evenH-row,0), 1 );
                }
                                      
                free(bgruiarr);
             }
             else
             {
                for ( row = height-1; row >= 0; row -= 2 )
                   gan_copy_array_ui (
                       (unsigned int *)
                       gan_image_get_pixptr_bgr_ui(source,row,0), 1,
                       3*source->width,
                       (unsigned int *)
                       gan_image_get_pixptr_bgr_ui(dest,oddH-row,0), 1 );

                for ( row = height-2; row >= 0; row -= 2 )
                   gan_copy_array_ui (
                       (unsigned int *)
                       gan_image_get_pixptr_bgr_ui(source,row,0), 1,
                       3*source->width,
                       (unsigned int *)
                       gan_image_get_pixptr_bgr_ui(dest,evenH-row,0), 1 );
             }

             break;

           case GAN_FLOAT:
             if ( source == dest )
             {
                Gan_BGRPixel_f *bgrfarr;

                bgrfarr = gan_malloc_array(Gan_BGRPixel_f, width);
                if ( bgrfarr == NULL )
                {
                   gan_err_flush_trace();
                   gan_err_register_with_number ( "gan_image_flip_q", GAN_ERROR_MALLOC_FAILED, "", width*sizeof(Gan_BGRPixel_f) );
                   return NULL;
                }

                for ( row = height/2-1; row >= 0; row -= 2 )
                {
                   gan_copy_array_f (
                       (float *)
                       gan_image_get_pixptr_bgr_f(source,row,0), 1,
                       3*source->width, (float *)bgrfarr, 1 );
                   gan_copy_array_f (
                       (float *)
                       gan_image_get_pixptr_bgr_f(source,oddH-row,0), 1,
                       3*source->width, (float *)
                       gan_image_get_pixptr_bgr_f(source,row,0), 1 );
                   gan_copy_array_f (
                       (float *)bgrfarr, 1, 3*source->width,
                       (float *)
                       gan_image_get_pixptr_bgr_f(source,oddH-row,0), 1 );
                }
                                      
                for ( row = height/2-2; row >= 0; row -= 2 )
                {
                   gan_copy_array_f (
                       (float *)
                       gan_image_get_pixptr_bgr_f(source,row,0), 1,
                       3*source->width, (float *)bgrfarr, 1 );
                   gan_copy_array_f (
                       (float *)
                       gan_image_get_pixptr_bgr_f(source,evenH-row,0), 1,
                       3*source->width, (float *)
                       gan_image_get_pixptr_bgr_f(source,row,0), 1 );
                   gan_copy_array_f (
                       (float *)bgrfarr, 1, 3*source->width,
                       (float *)
                       gan_image_get_pixptr_bgr_f(source,evenH-row,0), 1 );
                }
                                      
                free(bgrfarr);
             }
             else
             {
                for ( row = height-1; row >= 0; row -= 2 )
                   gan_copy_array_f (
                       (float *)
                       gan_image_get_pixptr_bgr_f(source,row,0), 1,
                       3*source->width,
                       (float *)
                       gan_image_get_pixptr_bgr_f(dest,oddH-row,0), 1 );

                for ( row = height-2; row >= 0; row -= 2 )
                   gan_copy_array_f (
                       (float *)
                       gan_image_get_pixptr_bgr_f(source,row,0), 1,
                       3*source->width,
                       (float *)
                       gan_image_get_pixptr_bgr_f(dest,evenH-row,0), 1 );
             }

             break;

           case GAN_DOUBLE:
             if ( source == dest )
             {
                Gan_BGRPixel_d *bgrdarr;

                bgrdarr = gan_malloc_array(Gan_BGRPixel_d, width);
                if ( bgrdarr == NULL )
                {
                   gan_err_flush_trace();
                   gan_err_register_with_number ( "gan_image_flip_q", GAN_ERROR_MALLOC_FAILED, "", width*sizeof(Gan_BGRPixel_d) );
                   return NULL;
                }

                for ( row = height/2-1; row >= 0; row -= 2 )
                {
                   gan_copy_array_d (
                       (double *)
                       gan_image_get_pixptr_bgr_d(source,row,0), 1,
                       3*source->width, (double *)bgrdarr, 1 );
                   gan_copy_array_d (
                       (double *)
                       gan_image_get_pixptr_bgr_d(source,oddH-row,0), 1,
                       3*source->width, (double *)
                       gan_image_get_pixptr_bgr_d(source,row,0), 1 );
                   gan_copy_array_d (
                       (double *)bgrdarr, 1, 3*source->width,
                       (double *)
                       gan_image_get_pixptr_bgr_d(source,oddH-row,0), 1 );
                }
                                      
                for ( row = height/2-2; row >= 0; row -= 2 )
                {
                   gan_copy_array_d (
                       (double *)
                       gan_image_get_pixptr_bgr_d(source,row,0), 1,
                       3*source->width, (double *)bgrdarr, 1 );
                   gan_copy_array_d (
                       (double *)
                       gan_image_get_pixptr_bgr_d(source,evenH-row,0), 1,
                       3*source->width, (double *)
                       gan_image_get_pixptr_bgr_d(source,row,0), 1 );
                   gan_copy_array_d (
                       (double *)bgrdarr, 1, 3*source->width,
                       (double *)
                       gan_image_get_pixptr_bgr_d(source,evenH-row,0), 1 );
                }
                                      
                free(bgrdarr);
             }
             else
             {
                for ( row = height-1; row >= 0; row -= 2 )
                   gan_copy_array_d (
                       (double *)
                       gan_image_get_pixptr_bgr_d(source,row,0), 1,
                       3*source->width,
                       (double *)
                       gan_image_get_pixptr_bgr_d(dest,oddH-row,0), 1 );

                for ( row = height-2; row >= 0; row -= 2 )
                   gan_copy_array_d (
                       (double *)
                       gan_image_get_pixptr_bgr_d(source,row,0), 1,
                       3*source->width,
                       (double *)
                       gan_image_get_pixptr_bgr_d(dest,evenH-row,0), 1 );
             }

             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_flip_q", GAN_ERROR_ILLEGAL_IMAGE_TYPE,
                                "" );
             return NULL;
        }

        break;

      case GAN_BGR_COLOUR_ALPHA_IMAGE:
        switch ( source->type )
        {
           case GAN_UCHAR:
             if ( source == dest )
             {
                Gan_BGRAPixel_uc *bgraucarr;

                bgraucarr = gan_malloc_array(Gan_BGRAPixel_uc, width);
                if ( bgraucarr == NULL )
                {
                   gan_err_flush_trace();
                   gan_err_register_with_number ( "gan_image_flip_q", GAN_ERROR_MALLOC_FAILED, "", width*sizeof(Gan_BGRAPixel_uc) );
                   return NULL;
                }

                for ( row = height/2-1; row >= 0; row -= 2 )
                {
                   gan_copy_array_uc (
                      (unsigned char *)
                      gan_image_get_pixptr_bgra_uc(source,row,0), 1,
                      4*source->width, (unsigned char *)bgraucarr, 1 );
                   gan_copy_array_uc (
                      (unsigned char *)
                      gan_image_get_pixptr_bgra_uc(source,oddH-row,0), 1,
                      4*source->width, (unsigned char *)
                      gan_image_get_pixptr_bgra_uc(source,row,0), 1 );
                   gan_copy_array_uc (
                      (unsigned char *)bgraucarr, 1, 4*source->width,
                      (unsigned char *)
                      gan_image_get_pixptr_bgra_uc(source,oddH-row,0), 1 );
                }
                                      
                for ( row = height/2-2; row >= 0; row -= 2 )
                {
                   gan_copy_array_uc (
                      (unsigned char *)
                      gan_image_get_pixptr_bgra_uc(source,row,0), 1,
                      4*source->width, (unsigned char *)bgraucarr, 1 );
                   gan_copy_array_uc (
                      (unsigned char *)
                      gan_image_get_pixptr_bgra_uc(source,evenH-row,0), 1,
                      4*source->width, (unsigned char *)
                      gan_image_get_pixptr_bgra_uc(source,row,0), 1 );
                   gan_copy_array_uc (
                      (unsigned char *)bgraucarr, 1, 4*source->width,
                      (unsigned char *)
                      gan_image_get_pixptr_bgra_uc(source,evenH-row,0), 1 );
                }
                                      
                free(bgraucarr);
             }
             else
             {
                for ( row = height-1; row >= 0; row -= 2 )
                   gan_copy_array_uc (
                       (unsigned char *)
                       gan_image_get_pixptr_bgra_uc(source,row,0), 1,
                       4*source->width,
                       (unsigned char *)
                       gan_image_get_pixptr_bgra_uc(dest,oddH-row,0), 1 );

                for ( row = height-2; row >= 0; row -= 2 )
                   gan_copy_array_uc (
                       (unsigned char *)
                       gan_image_get_pixptr_bgra_uc(source,row,0), 1,
                       4*source->width,
                       (unsigned char *)
                       gan_image_get_pixptr_bgra_uc(dest,evenH-row,0), 1 );
             }

             break;

           case GAN_USHORT:
             if ( source == dest )
             {
                Gan_BGRAPixel_us *bgrausarr;

                bgrausarr = gan_malloc_array(Gan_BGRAPixel_us, width);
                if ( bgrausarr == NULL )
                {
                   gan_err_flush_trace();
                   gan_err_register_with_number ( "gan_image_flip_q", GAN_ERROR_MALLOC_FAILED, "", width*sizeof(Gan_BGRAPixel_us) );
                   return NULL;
                }

                for ( row = height/2-1; row >= 0; row -= 2 )
                {
                   gan_copy_array_us (
                       (unsigned short *)
                       gan_image_get_pixptr_bgra_us(source,row,0), 1,
                       4*source->width, (unsigned short *)bgrausarr, 1 );
                   gan_copy_array_us (
                       (unsigned short *)
                       gan_image_get_pixptr_bgra_us(source,oddH-row,0), 1,
                       4*source->width, (unsigned short *)
                       gan_image_get_pixptr_bgra_us(source,row,0), 1 );
                   gan_copy_array_us (
                       (unsigned short *)bgrausarr, 1, 4*source->width,
                       (unsigned short *)
                       gan_image_get_pixptr_bgra_us(source,oddH-row,0), 1);
                }
                                      
                for ( row = height/2-2; row >= 0; row -= 2 )
                {
                   gan_copy_array_us (
                       (unsigned short *)
                       gan_image_get_pixptr_bgra_us(source,row,0), 1,
                       4*source->width, (unsigned short *)bgrausarr, 1 );
                   gan_copy_array_us (
                       (unsigned short *)
                       gan_image_get_pixptr_bgra_us(source,evenH-row,0), 1,
                       4*source->width, (unsigned short *)
                       gan_image_get_pixptr_bgra_us(source,row,0), 1 );
                   gan_copy_array_us (
                       (unsigned short *)bgrausarr, 1, 4*source->width,
                       (unsigned short *)
                       gan_image_get_pixptr_bgra_us(source,evenH-row,0), 1);
                }
                                      
                free(bgrausarr);
             }
             else
             {
                for ( row = height-1; row >= 0; row -= 2 )
                   gan_copy_array_us (
                       (unsigned short *)
                       gan_image_get_pixptr_bgra_us(source,row,0), 1,
                       4*source->width,
                       (unsigned short *)
                       gan_image_get_pixptr_bgra_us(dest,oddH-row,0), 1 );

                for ( row = height-2; row >= 0; row -= 2 )
                   gan_copy_array_us (
                       (unsigned short *)
                       gan_image_get_pixptr_bgra_us(source,row,0), 1,
                       4*source->width,
                       (unsigned short *)
                       gan_image_get_pixptr_bgra_us(dest,evenH-row,0), 1 );
             }

             break;

           case GAN_UINT:
             if ( source == dest )
             {
                Gan_BGRAPixel_ui *bgrauiarr;

                bgrauiarr = gan_malloc_array(Gan_BGRAPixel_ui, width);
                if ( bgrauiarr == NULL )
                {
                   gan_err_flush_trace();
                   gan_err_register_with_number ( "gan_image_flip_q", GAN_ERROR_MALLOC_FAILED, "", width*sizeof(Gan_BGRAPixel_ui) );
                   return NULL;
                }

                for ( row = height/2-1; row >= 0; row -= 2 )
                {
                   gan_copy_array_ui (
                       (unsigned int *)
                       gan_image_get_pixptr_bgra_ui(source,row,0), 1,
                       4*source->width, (unsigned int *)bgrauiarr, 1 );
                   gan_copy_array_ui (
                       (unsigned int *)
                       gan_image_get_pixptr_bgra_ui(source,oddH-row,0), 1,
                       4*source->width, (unsigned int *)
                       gan_image_get_pixptr_bgra_ui(source,row,0), 1 );
                   gan_copy_array_ui (
                       (unsigned int *)bgrauiarr, 1, 4*source->width,
                       (unsigned int *)
                       gan_image_get_pixptr_bgra_ui(source,oddH-row,0), 1);
                }
                                      
                for ( row = height/2-2; row >= 0; row -= 2 )
                {
                   gan_copy_array_ui (
                       (unsigned int *)
                       gan_image_get_pixptr_bgra_ui(source,row,0), 1,
                       4*source->width, (unsigned int *)bgrauiarr, 1 );
                   gan_copy_array_ui (
                       (unsigned int *)
                       gan_image_get_pixptr_bgra_ui(source,evenH-row,0), 1,
                       4*source->width, (unsigned int *)
                       gan_image_get_pixptr_bgra_ui(source,row,0), 1 );
                   gan_copy_array_ui (
                       (unsigned int *)bgrauiarr, 1, 4*source->width,
                       (unsigned int *)
                       gan_image_get_pixptr_bgra_ui(source,evenH-row,0), 1);
                }
                                      
                free(bgrauiarr);
             }
             else
             {
                for ( row = height-1; row >= 0; row -= 2 )
                   gan_copy_array_ui (
                       (unsigned int *)
                       gan_image_get_pixptr_bgra_ui(source,row,0), 1,
                       4*source->width,
                       (unsigned int *)
                       gan_image_get_pixptr_bgra_ui(dest,oddH-row,0), 1 );

                for ( row = height-2; row >= 0; row -= 2 )
                   gan_copy_array_ui (
                       (unsigned int *)
                       gan_image_get_pixptr_bgra_ui(source,row,0), 1,
                       4*source->width,
                       (unsigned int *)
                       gan_image_get_pixptr_bgra_ui(dest,evenH-row,0), 1 );
             }

             break;

           case GAN_FLOAT:
             if ( source == dest )
             {
                Gan_BGRAPixel_f *bgrafarr;

                bgrafarr = gan_malloc_array(Gan_BGRAPixel_f, width);
                if ( bgrafarr == NULL )
                {
                   gan_err_flush_trace();
                   gan_err_register_with_number ( "gan_image_flip_q", GAN_ERROR_MALLOC_FAILED, "", width*sizeof(Gan_BGRAPixel_f) );
                   return NULL;
                }

                for ( row = height/2-1; row >= 0; row -= 2 )
                {
                   gan_copy_array_f (
                       (float *)
                       gan_image_get_pixptr_bgra_f(source,row,0), 1,
                       4*source->width, (float *)bgrafarr, 1 );
                   gan_copy_array_f (
                       (float *)
                       gan_image_get_pixptr_bgra_f(source,oddH-row,0), 1,
                       4*source->width, (float *)
                       gan_image_get_pixptr_bgra_f(source,row,0), 1 );
                   gan_copy_array_f (
                       (float *)bgrafarr, 1, 4*source->width,
                       (float *)
                       gan_image_get_pixptr_bgra_f(source,oddH-row,0), 1 );
                }
                                      
                for ( row = height/2-2; row >= 0; row -= 2 )
                {
                   gan_copy_array_f (
                       (float *)
                       gan_image_get_pixptr_bgra_f(source,row,0), 1,
                       4*source->width, (float *)bgrafarr, 1 );
                   gan_copy_array_f (
                       (float *)
                       gan_image_get_pixptr_bgra_f(source,evenH-row,0), 1,
                       4*source->width, (float *)
                       gan_image_get_pixptr_bgra_f(source,row,0), 1 );
                   gan_copy_array_f (
                       (float *)bgrafarr, 1, 4*source->width,
                       (float *)
                       gan_image_get_pixptr_bgra_f(source,evenH-row,0), 1 );
                }
                                      
                free(bgrafarr);
             }
             else
             {
                for ( row = height-1; row >= 0; row -= 2 )
                   gan_copy_array_f (
                       (float *)
                       gan_image_get_pixptr_bgra_f(source,row,0), 1,
                       4*source->width,
                       (float *)
                       gan_image_get_pixptr_bgra_f(dest,oddH-row,0), 1 );

                for ( row = height-2; row >= 0; row -= 2 )
                   gan_copy_array_f (
                       (float *)
                       gan_image_get_pixptr_bgra_f(source,row,0), 1,
                       4*source->width,
                       (float *)
                       gan_image_get_pixptr_bgra_f(dest,evenH-row,0), 1 );
             }

             break;

           case GAN_DOUBLE:
             if ( source == dest )
             {
                Gan_BGRAPixel_d *bgradarr;

                bgradarr = gan_malloc_array(Gan_BGRAPixel_d, width);
                if ( bgradarr == NULL )
                {
                   gan_err_flush_trace();
                   gan_err_register_with_number ( "gan_image_flip_q", GAN_ERROR_MALLOC_FAILED, "", width*sizeof(Gan_BGRAPixel_d) );
                   return NULL;
                }

                for ( row = height/2-1; row >= 0; row -= 2 )
                {
                   gan_copy_array_d (
                       (double *)
                       gan_image_get_pixptr_bgra_d(source,row,0), 1,
                       4*source->width, (double *)bgradarr, 1 );
                   gan_copy_array_d (
                       (double *)
                       gan_image_get_pixptr_bgra_d(source,oddH-row,0), 1,
                       4*source->width, (double *)
                       gan_image_get_pixptr_bgra_d(source,row,0), 1 );
                   gan_copy_array_d (
                       (double *)bgradarr, 1, 4*source->width,
                       (double *)
                       gan_image_get_pixptr_bgra_d(source,oddH-row,0), 1 );
                }
                                      
                for ( row = height/2-2; row >= 0; row -= 2 )
                {
                   gan_copy_array_d (
                       (double *)
                       gan_image_get_pixptr_bgra_d(source,row,0), 1,
                       4*source->width, (double *)bgradarr, 1 );
                   gan_copy_array_d (
                       (double *)
                       gan_image_get_pixptr_bgra_d(source,evenH-row,0), 1,
                       4*source->width, (double *)
                       gan_image_get_pixptr_bgra_d(source,row,0), 1 );
                   gan_copy_array_d (
                       (double *)bgradarr, 1, 4*source->width,
                       (double *)
                       gan_image_get_pixptr_bgra_d(source,evenH-row,0), 1 );
                }
                                      
                free(bgradarr);
             }
             else
             {
                for ( row = height-1; row >= 0; row -= 2 )
                   gan_copy_array_d (
                       (double *)
                       gan_image_get_pixptr_bgra_d(source,row,0), 1,
                       4*source->width,
                       (double *)
                       gan_image_get_pixptr_bgra_d(dest,oddH-row,0), 1 );

                for ( row = height-2; row >= 0; row -= 2 )
                   gan_copy_array_d (
                       (double *)
                       gan_image_get_pixptr_bgra_d(source,row,0), 1,
                       4*source->width,
                       (double *)
                       gan_image_get_pixptr_bgra_d(dest,evenH-row,0), 1 );
             }

             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_flip_q", GAN_ERROR_ILLEGAL_IMAGE_TYPE,
                                "" );
             return NULL;
        }

        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_image_flip_q", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
        return NULL;
   }

   /* success */
   return dest;
}

/**
 * \}
 */

/**
 * \}
 */

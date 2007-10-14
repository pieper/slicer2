/**
 * File:          $RCSfile: image_invert.c,v $
 * Module:        Extract/fill selected channel(s) of image
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

#include <gandalf/image/image_invert.h>
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

/**
 * \addtogroup ImagePackage
 * \{
 */

/**
 * \defgroup ImageInvert Invert an Image
 * \{
 */

/**
 * \brief Inverts an image.
 * \param source The input image
 * \param dest The destination image
 * \return Pointer to the inverted image, or \c NULL on failure.
 *
 * Inverts image \a source into result image \a dest.
 *
 * \sa gan_image_invert_q().
 */
Gan_Image *
 gan_image_invert_q ( Gan_Image *source, Gan_Image *dest )
{
   int row, col;

   /* allocate/set output image */
   if ( dest == NULL )
   {
      dest = gan_image_copy_s ( source );
      if ( dest == NULL )
      {
         gan_err_register ( "gan_image_invert_q", GAN_ERROR_FAILURE, "" );
         return NULL;
      }
   }
   else if ( source != dest )
   {
      dest = gan_image_set_format_type_dims ( dest, dest->format, dest->type,
                                              dest->height, dest->width );
      if ( dest == NULL )
      {
         gan_err_register ( "gan_image_invert_q", GAN_ERROR_FAILURE, "" );
         return NULL;
      }

      dest->offset_x = source->offset_x;
      dest->offset_y = source->offset_y;
   }

   /* invert image */
   switch ( source->format )
   {
      case GAN_GREY_LEVEL_IMAGE:
        switch ( source->type )
        {
           case GAN_BOOL:
             if ( gan_image_bit_invert_q ( source, dest ) == NULL )
             {
                gan_err_register ( "gan_image_invert_q", GAN_ERROR_FAILURE,
                                   "" );
                return NULL;
             }

             break;

           case GAN_UCHAR:
             for ( row = (int)source->height-1; row >= 0; row-- )
                for ( col = (int)source->width-1; col >= 0; col-- )
                   gan_image_set_pix_gl_uc ( dest, row, col,
                      UCHAR_MAX-gan_image_get_pix_gl_uc ( source, row, col ) );
             
             break;

           case GAN_USHORT:
             for ( row = (int)source->height-1; row >= 0; row-- )
                for ( col = (int)source->width-1; col >= 0; col-- )
                   gan_image_set_pix_gl_us ( dest, row, col,
                      USHRT_MAX-gan_image_get_pix_gl_us ( source, row, col ) );
             
             break;

           case GAN_UINT:
             for ( row = (int)source->height-1; row >= 0; row-- )
                for ( col = (int)source->width-1; col >= 0; col-- )
                   gan_image_set_pix_gl_ui ( dest, row, col,
                      UINT_MAX-gan_image_get_pix_gl_ui ( source, row, col ) );
             
             break;

           case GAN_FLOAT:
             for ( row = (int)source->height-1; row >= 0; row-- )
                for ( col = (int)source->width-1; col >= 0; col-- )
                   gan_image_set_pix_gl_f ( dest, row, col,
                      1.0F-gan_image_get_pix_gl_f ( source, row, col ) );
             
             break;

           case GAN_DOUBLE:
             for ( row = (int)source->height-1; row >= 0; row-- )
                for ( col = (int)source->width-1; col >= 0; col-- )
                   gan_image_set_pix_gl_d ( dest, row, col,
                      1.0-gan_image_get_pix_gl_d ( source, row, col ) );
             
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_invert_q", GAN_ERROR_ILLEGAL_IMAGE_TYPE,
                                "" );
             return NULL;
        }

        break;

      case GAN_RGB_COLOUR_IMAGE:
        switch ( source->type )
        {
           case GAN_UCHAR:
           {
              Gan_RGBPixel_uc pix;
              
              for ( row = (int)source->height-1; row >= 0; row-- )
                 for ( col = (int)source->width-1; col >= 0; col-- )
                 {
                    pix = gan_image_get_pix_rgb_uc ( source, row, col );
                    pix.R = UCHAR_MAX - pix.R;
                    pix.G = UCHAR_MAX - pix.G;
                    pix.B = UCHAR_MAX - pix.B;
                    gan_image_set_pix_rgb_uc ( dest, row, col, &pix );
                 }
           }
           break;

           case GAN_USHORT:
           {
              Gan_RGBPixel_us pix;
              
              for ( row = (int)source->height-1; row >= 0; row-- )
                 for ( col = (int)source->width-1; col >= 0; col-- )
                 {
                    pix = gan_image_get_pix_rgb_us ( source, row, col );
                    pix.R = USHRT_MAX - pix.R;
                    pix.G = USHRT_MAX - pix.G;
                    pix.B = USHRT_MAX - pix.B;
                    gan_image_set_pix_rgb_us ( dest, row, col, &pix );
                 }
           }
           break;

           case GAN_UINT:
           {
              Gan_RGBPixel_ui pix;
              
              for ( row = (int)source->height-1; row >= 0; row-- )
                 for ( col = (int)source->width-1; col >= 0; col-- )
                 {
                    pix = gan_image_get_pix_rgb_ui ( source, row, col );
                    pix.R = UINT_MAX - pix.R;
                    pix.G = UINT_MAX - pix.G;
                    pix.B = UINT_MAX - pix.B;
                    gan_image_set_pix_rgb_ui ( dest, row, col, &pix );
                 }
           }
           break;

           case GAN_FLOAT:
           {
              Gan_RGBPixel_f pix;
              
              for ( row = (int)source->height-1; row >= 0; row-- )
                 for ( col = (int)source->width-1; col >= 0; col-- )
                 {
                    pix = gan_image_get_pix_rgb_f ( source, row, col );
                    pix.R = 1.0F - pix.R;
                    pix.G = 1.0F - pix.G;
                    pix.B = 1.0F - pix.B;
                    gan_image_set_pix_rgb_f ( dest, row, col, &pix );
                 }
           }
           break;

           case GAN_DOUBLE:
           {
              Gan_RGBPixel_d pix;
              
              for ( row = (int)source->height-1; row >= 0; row-- )
                 for ( col = (int)source->width-1; col >= 0; col-- )
                 {
                    pix = gan_image_get_pix_rgb_d ( source, row, col );
                    pix.R = 1.0 - pix.R;
                    pix.G = 1.0 - pix.G;
                    pix.B = 1.0 - pix.B;
                    gan_image_set_pix_rgb_d ( dest, row, col, &pix );
                 }
           }
           break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_invert_q", GAN_ERROR_ILLEGAL_IMAGE_TYPE,
                                "" );
             return NULL;
        }

        break;

      case GAN_RGB_COLOUR_ALPHA_IMAGE:
        switch ( source->type )
        {
           case GAN_UCHAR:
           {
              Gan_RGBAPixel_uc pix;
              
              for ( row = (int)source->height-1; row >= 0; row-- )
                 for ( col = (int)source->width-1; col >= 0; col-- )
                 {
                    pix = gan_image_get_pix_rgba_uc ( source, row, col );
                    pix.R = UCHAR_MAX - pix.R;
                    pix.G = UCHAR_MAX - pix.G;
                    pix.B = UCHAR_MAX - pix.B;
                    pix.A = UCHAR_MAX - pix.A;
                    gan_image_set_pix_rgba_uc ( dest, row, col, &pix );
                 }
           }
           break;

           case GAN_USHORT:
           {
              Gan_RGBAPixel_us pix;
              
              for ( row = (int)source->height-1; row >= 0; row-- )
                 for ( col = (int)source->width-1; col >= 0; col-- )
                 {
                    pix = gan_image_get_pix_rgba_us ( source, row, col );
                    pix.R = USHRT_MAX - pix.R;
                    pix.G = USHRT_MAX - pix.G;
                    pix.B = USHRT_MAX - pix.B;
                    pix.A = USHRT_MAX - pix.A;
                    gan_image_set_pix_rgba_us ( dest, row, col, &pix );
                 }
           }
           break;

           case GAN_UINT:
           {
              Gan_RGBAPixel_ui pix;
              
              for ( row = (int)source->height-1; row >= 0; row-- )
                 for ( col = (int)source->width-1; col >= 0; col-- )
                 {
                    pix = gan_image_get_pix_rgba_ui ( source, row, col );
                    pix.R = UINT_MAX - pix.R;
                    pix.G = UINT_MAX - pix.G;
                    pix.B = UINT_MAX - pix.B;
                    pix.A = UINT_MAX - pix.A;
                    gan_image_set_pix_rgba_ui ( dest, row, col, &pix );
                 }
           }
           break;

           case GAN_FLOAT:
           {
              Gan_RGBAPixel_f pix;
              
              for ( row = (int)source->height-1; row >= 0; row-- )
                 for ( col = (int)source->width-1; col >= 0; col-- )
                 {
                    pix = gan_image_get_pix_rgba_f ( source, row, col );
                    pix.R = 1.0F - pix.R;
                    pix.G = 1.0F - pix.G;
                    pix.B = 1.0F - pix.B;
                    pix.A = 1.0F - pix.A;
                    gan_image_set_pix_rgba_f ( dest, row, col, &pix );
                 }
           }
           break;

           case GAN_DOUBLE:
           {
              Gan_RGBAPixel_d pix;
              
              for ( row = (int)source->height-1; row >= 0; row-- )
                 for ( col = (int)source->width-1; col >= 0; col-- )
                 {
                    pix = gan_image_get_pix_rgba_d ( source, row, col );
                    pix.R = 1.0 - pix.R;
                    pix.G = 1.0 - pix.G;
                    pix.B = 1.0 - pix.B;
                    pix.A = 1.0 - pix.A;
                    gan_image_set_pix_rgba_d ( dest, row, col, &pix );
                 }
           }
           break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_invert_q", GAN_ERROR_ILLEGAL_IMAGE_TYPE,
                                "" );
             return NULL;
        }

        break;

      case GAN_BGR_COLOUR_IMAGE:
        switch ( source->type )
        {
           case GAN_UCHAR:
           {
              Gan_BGRPixel_uc pix;
              
              for ( row = (int)source->height-1; row >= 0; row-- )
                 for ( col = (int)source->width-1; col >= 0; col-- )
                 {
                    pix = gan_image_get_pix_bgr_uc ( source, row, col );
                    pix.R = UCHAR_MAX - pix.R;
                    pix.G = UCHAR_MAX - pix.G;
                    pix.B = UCHAR_MAX - pix.B;
                    gan_image_set_pix_bgr_uc ( dest, row, col, &pix );
                 }
           }
           break;

           case GAN_USHORT:
           {
              Gan_BGRPixel_us pix;
              
              for ( row = (int)source->height-1; row >= 0; row-- )
                 for ( col = (int)source->width-1; col >= 0; col-- )
                 {
                    pix = gan_image_get_pix_bgr_us ( source, row, col );
                    pix.R = USHRT_MAX - pix.R;
                    pix.G = USHRT_MAX - pix.G;
                    pix.B = USHRT_MAX - pix.B;
                    gan_image_set_pix_bgr_us ( dest, row, col, &pix );
                 }
           }
           break;

           case GAN_UINT:
           {
              Gan_BGRPixel_ui pix;
              
              for ( row = (int)source->height-1; row >= 0; row-- )
                 for ( col = (int)source->width-1; col >= 0; col-- )
                 {
                    pix = gan_image_get_pix_bgr_ui ( source, row, col );
                    pix.R = UINT_MAX - pix.R;
                    pix.G = UINT_MAX - pix.G;
                    pix.B = UINT_MAX - pix.B;
                    gan_image_set_pix_bgr_ui ( dest, row, col, &pix );
                 }
           }
           break;

           case GAN_FLOAT:
           {
              Gan_BGRPixel_f pix;
              
              for ( row = (int)source->height-1; row >= 0; row-- )
                 for ( col = (int)source->width-1; col >= 0; col-- )
                 {
                    pix = gan_image_get_pix_bgr_f ( source, row, col );
                    pix.R = 1.0F - pix.R;
                    pix.G = 1.0F - pix.G;
                    pix.B = 1.0F - pix.B;
                    gan_image_set_pix_bgr_f ( dest, row, col, &pix );
                 }
           }
           break;

           case GAN_DOUBLE:
           {
              Gan_BGRPixel_d pix;
              
              for ( row = (int)source->height-1; row >= 0; row-- )
                 for ( col = (int)source->width-1; col >= 0; col-- )
                 {
                    pix = gan_image_get_pix_bgr_d ( source, row, col );
                    pix.R = 1.0 - pix.R;
                    pix.G = 1.0 - pix.G;
                    pix.B = 1.0 - pix.B;
                    gan_image_set_pix_bgr_d ( dest, row, col, &pix );
                 }
           }
           break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_invert_q", GAN_ERROR_ILLEGAL_IMAGE_TYPE,
                                "" );
             return NULL;
        }

        break;

      case GAN_BGR_COLOUR_ALPHA_IMAGE:
        switch ( source->type )
        {
           case GAN_UCHAR:
           {
              Gan_BGRAPixel_uc pix;
              
              for ( row = (int)source->height-1; row >= 0; row-- )
                 for ( col = (int)source->width-1; col >= 0; col-- )
                 {
                    pix = gan_image_get_pix_bgra_uc ( source, row, col );
                    pix.R = UCHAR_MAX - pix.R;
                    pix.G = UCHAR_MAX - pix.G;
                    pix.B = UCHAR_MAX - pix.B;
                    pix.A = UCHAR_MAX - pix.A;
                    gan_image_set_pix_bgra_uc ( dest, row, col, &pix );
                 }
           }
           break;

           case GAN_USHORT:
           {
              Gan_BGRAPixel_us pix;
              
              for ( row = (int)source->height-1; row >= 0; row-- )
                 for ( col = (int)source->width-1; col >= 0; col-- )
                 {
                    pix = gan_image_get_pix_bgra_us ( source, row, col );
                    pix.R = USHRT_MAX - pix.R;
                    pix.G = USHRT_MAX - pix.G;
                    pix.B = USHRT_MAX - pix.B;
                    pix.A = USHRT_MAX - pix.A;
                    gan_image_set_pix_bgra_us ( dest, row, col, &pix );
                 }
           }
           break;

           case GAN_UINT:
           {
              Gan_BGRAPixel_ui pix;
              
              for ( row = (int)source->height-1; row >= 0; row-- )
                 for ( col = (int)source->width-1; col >= 0; col-- )
                 {
                    pix = gan_image_get_pix_bgra_ui ( source, row, col );
                    pix.R = UINT_MAX - pix.R;
                    pix.G = UINT_MAX - pix.G;
                    pix.B = UINT_MAX - pix.B;
                    pix.A = UINT_MAX - pix.A;
                    gan_image_set_pix_bgra_ui ( dest, row, col, &pix );
                 }
           }
           break;

           case GAN_FLOAT:
           {
              Gan_BGRAPixel_f pix;
              
              for ( row = (int)source->height-1; row >= 0; row-- )
                 for ( col = (int)source->width-1; col >= 0; col-- )
                 {
                    pix = gan_image_get_pix_bgra_f ( source, row, col );
                    pix.R = 1.0F - pix.R;
                    pix.G = 1.0F - pix.G;
                    pix.B = 1.0F - pix.B;
                    pix.A = 1.0F - pix.A;
                    gan_image_set_pix_bgra_f ( dest, row, col, &pix );
                 }
           }
           break;

           case GAN_DOUBLE:
           {
              Gan_BGRAPixel_d pix;
              
              for ( row = (int)source->height-1; row >= 0; row-- )
                 for ( col = (int)source->width-1; col >= 0; col-- )
                 {
                    pix = gan_image_get_pix_bgra_d ( source, row, col );
                    pix.R = 1.0 - pix.R;
                    pix.G = 1.0 - pix.G;
                    pix.B = 1.0 - pix.B;
                    pix.A = 1.0 - pix.A;
                    gan_image_set_pix_bgra_d ( dest, row, col, &pix );
                 }
           }
           break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_invert_q", GAN_ERROR_ILLEGAL_IMAGE_TYPE,
                                "" );
             return NULL;
        }

        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_image_invert_q", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
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

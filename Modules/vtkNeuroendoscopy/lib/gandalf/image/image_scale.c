/**
 * File:          $RCSfile: image_scale.c,v $
 * Module:        Extract/fill selected channel(s) of image
 * Part of:       Gandalf Library
 *
 * Revision:      $Revision: 1.1.2.1 $
 * Last edited:   $Date: 2007/10/14 02:20:25 $
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

#include <gandalf/image/image_scale.h>
#include <gandalf/image/image_bit.h>
#include <gandalf/image/image_gl_float.h>
#include <gandalf/image/image_gl_double.h>
#include <gandalf/image/image_gla_float.h>
#include <gandalf/image/image_gla_double.h>
#include <gandalf/image/image_rgb_float.h>
#include <gandalf/image/image_rgb_double.h>
#include <gandalf/image/image_rgba_float.h>
#include <gandalf/image/image_rgba_double.h>
#include <gandalf/image/image_bgr_float.h>
#include <gandalf/image/image_bgr_double.h>
#include <gandalf/image/image_bgra_float.h>
#include <gandalf/image/image_bgra_double.h>

/**
 * \addtogroup ImagePackage
 * \{
 */

/**
 * \defgroup ImageScale Scale an Image
 * \{
 */

/**
 * \brief Scales the pixels in an image.
 * \param source The input image
 * \param mask A binary mask or \c NULL
 * \param scale Pixel defining the scale factor
 * \param dest The destination image
 * \return Pointer to the scaled image, or \c NULL on failure.
 *
 * Scales all the pixels in image \a source by given scale factor \a scale
 * into result image \a dest. If \a mask is not \c NULL then only pixels
 * in the mask are scaled.
 *
 * The \a scale pixel must be of \c GAN_FLOAT type.
 *
 * \sa gan_image_scale_q().
 */
Gan_Image *
 gan_image_scale_q ( Gan_Image *source, Gan_Image *mask, Gan_Pixel *scale,
                     Gan_Image *dest )
{
   Gan_ImageWindow window;
   unsigned int row, col, row_max, col_max;

   gan_err_test_ptr ( scale->type == GAN_FLOAT,
                      "gan_image_scale_q", GAN_ERROR_INCOMPATIBLE, "" );
   gan_err_test_ptr ( mask == NULL ||
                      (mask->width == source->width &&
                       mask->height == source->height),
                      "gan_image_scale_q", GAN_ERROR_INCOMPATIBLE, "" );
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
         gan_err_register ( "gan_image_scale_q", GAN_ERROR_FAILURE, "" );
         return NULL;
      }
   }

   col_max = window.c0 + window.width;
   row_max = window.r0 + window.height;
      
   /* allocate/set output image */
   if ( dest == NULL )
   {
      dest = gan_image_copy_s ( source );
      if ( dest == NULL )
      {
         gan_err_register ( "gan_image_scale_q", GAN_ERROR_FAILURE, "" );
         return NULL;
      }
   }
   else if ( source != dest )
   {
      dest = gan_image_set_format_type_dims ( dest, dest->format, dest->type,
                                              dest->height, dest->width );
      if ( dest == NULL )
      {
         gan_err_register ( "gan_image_scale_q", GAN_ERROR_FAILURE, "" );
         return NULL;
      }

      dest->offset_x = source->offset_x;
      dest->offset_y = source->offset_y;
   }

   /* scale image */
   switch ( source->format )
   {
      case GAN_GREY_LEVEL_IMAGE:
        switch ( source->type )
        {
           case GAN_FLOAT:
           {
              float scalef;

              gan_err_test_ptr ( scale->format == GAN_GREY_LEVEL_IMAGE,
                                 "gan_image_scale_q", GAN_ERROR_INCOMPATIBLE,
                                 "" );
              scalef = scale->data.gl.f;

              if(mask == NULL)
              {
                 for ( row = window.r0; row < row_max; row++ )
                    for ( col = window.c0; col < col_max; col++ )
                       gan_image_set_pix_gl_f ( dest, row, col,
                          scalef*gan_image_get_pix_gl_f ( source, row, col ) );
              }
              else
              {
                 for ( row = window.r0; row < row_max; row++ )
                    for ( col = window.c0; col < col_max; col++ )
                       if(gan_image_get_pix_b(mask, row, col))
                          gan_image_set_pix_gl_f ( dest, row, col,
                              scalef*gan_image_get_pix_gl_f(source, row, col));
              }
           }
           break;

           case GAN_DOUBLE:
           {
              double scaled;

              gan_err_test_ptr ( scale->format == GAN_GREY_LEVEL_IMAGE,
                                 "gan_image_scale_q", GAN_ERROR_INCOMPATIBLE,
                                 "" );
              scaled = (double)scale->data.gl.f;

              if(mask == NULL)
              {
                 for ( row = window.r0; row < row_max; row++ )
                    for ( col = window.c0; col < col_max; col++ )
                       gan_image_set_pix_gl_d ( dest, row, col,
                          scaled*gan_image_get_pix_gl_d ( source, row, col ) );
              }
              else
              {
                 for ( row = window.r0; row < row_max; row++ )
                    for ( col = window.c0; col < col_max; col++ )
                       if(gan_image_get_pix_b(mask, row, col))
                          gan_image_set_pix_gl_d ( dest, row, col,
                             scaled*gan_image_get_pix_gl_d(source, row, col));
              }
           }
           break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_scale_q", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             return NULL;
        }

        break;

      case GAN_RGB_COLOUR_IMAGE:
        switch ( source->type )
        {
           case GAN_FLOAT:
           {
              Gan_RGBPixel_f pix;

              switch(scale->format)
              {
                 case GAN_GREY_LEVEL_IMAGE:
                 {
                    float scalef = scale->data.gl.f;

                    if(mask == NULL)
                    {
                       for ( row = window.r0; row < row_max; row++ )
                          for ( col = window.c0; col < col_max; col++ )
                          {
                             pix = gan_image_get_pix_rgb_f(source, row, col);
                             pix.R = scalef*pix.R;
                             pix.G = scalef*pix.G;
                             pix.B = scalef*pix.B;
                             gan_image_set_pix_rgb_f ( dest, row, col, &pix );
                          }
                    }
                    else
                    {
                       for ( row = window.r0; row < row_max; row++ )
                          for ( col = window.c0; col < col_max; col++ )
                             if(gan_image_get_pix_b(mask, row, col))
                             {
                                pix = gan_image_get_pix_rgb_f(source, row,col);
                                pix.R = scalef*pix.R;
                                pix.G = scalef*pix.G;
                                pix.B = scalef*pix.B;
                                gan_image_set_pix_rgb_f(dest, row, col, &pix);
                             }
                    }
                 }
                 break;

                 case GAN_RGB_COLOUR_IMAGE:
                   if(mask == NULL)
                   {
                      for ( row = window.r0; row < row_max; row++ )
                         for ( col = window.c0; col < col_max; col++ )
                         {
                            pix = gan_image_get_pix_rgb_f(source, row, col);
                            pix.R = scale->data.rgb.f.R*pix.R;
                            pix.G = scale->data.rgb.f.G*pix.G;
                            pix.B = scale->data.rgb.f.B*pix.B;
                            gan_image_set_pix_rgb_f ( dest, row, col, &pix );
                         }
                   }
                   else
                   {
                      for ( row = window.r0; row < row_max; row++ )
                         for ( col = window.c0; col < col_max; col++ )
                             if(gan_image_get_pix_b(mask, row, col))
                             {
                                pix = gan_image_get_pix_rgb_f(source, row,col);
                                pix.R = scale->data.rgb.f.R*pix.R;
                                pix.G = scale->data.rgb.f.G*pix.G;
                                pix.B = scale->data.rgb.f.B*pix.B;
                                gan_image_set_pix_rgb_f(dest, row, col, &pix);
                             }
                   }

                   break;

                 default:
                   gan_err_flush_trace();
                   gan_err_register ( "gan_image_scale_q", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
                   return NULL;
              }
           }

           break;

           case GAN_DOUBLE:
           {
              Gan_RGBPixel_d pix;
              
              switch(scale->format)
              {
                 case GAN_GREY_LEVEL_IMAGE:
                 {
                    double scaled = (double)scale->data.gl.f;

                    if(mask == NULL)
                    {
                       for ( row = window.r0; row < row_max; row++ )
                          for ( col = window.c0; col < col_max; col++ )
                          {
                             pix = gan_image_get_pix_rgb_d (source, row, col);
                             pix.R = scaled*pix.R;
                             pix.G = scaled*pix.G;
                             pix.B = scaled*pix.B;
                             gan_image_set_pix_rgb_d ( dest, row, col, &pix );
                          }
                    }
                    else
                    {
                       for ( row = window.r0; row < row_max; row++ )
                          for ( col = window.c0; col < col_max; col++ )
                             if(gan_image_get_pix_b(mask, row, col))
                             {
                                pix = gan_image_get_pix_rgb_d(source, row,col);
                                pix.R = scaled*pix.R;
                                pix.G = scaled*pix.G;
                                pix.B = scaled*pix.B;
                                gan_image_set_pix_rgb_d (dest, row, col, &pix);
                             }
                    }
                 }
                 break;

                 case GAN_RGB_COLOUR_IMAGE:
                 {
                    Gan_RGBPixel_d pixd;

                    pixd.R = (double)scale->data.rgb.f.R;
                    pixd.G = (double)scale->data.rgb.f.G;
                    pixd.B = (double)scale->data.rgb.f.B;
                    
                    if(mask == NULL)
                    {
                       for ( row = window.r0; row < row_max; row++ )
                          for ( col = window.c0; col < col_max; col++ )
                          {
                             pix = gan_image_get_pix_rgb_d(source, row, col);
                             pix.R = pixd.R*pix.R;
                             pix.G = pixd.G*pix.G;
                             pix.B = pixd.B*pix.B;
                             gan_image_set_pix_rgb_d ( dest, row, col, &pix );
                          }
                    }
                    else
                    {
                       for ( row = window.r0; row < row_max; row++ )
                          for ( col = window.c0; col < col_max; col++ )
                             if(gan_image_get_pix_b(mask, row, col))
                             {
                                pix = gan_image_get_pix_rgb_d(source, row,col);
                                pix.R = pixd.R*pix.R;
                                pix.G = pixd.G*pix.G;
                                pix.B = pixd.B*pix.B;
                                gan_image_set_pix_rgb_d(dest, row, col, &pix);
                             }
                    }
                 }
                 break;

                 default:
                   gan_err_flush_trace();
                   gan_err_register ( "gan_image_scale_q", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
                   return NULL;
              }
           }
           break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_scale_q", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             return NULL;
        }

        break;

      case GAN_RGB_COLOUR_ALPHA_IMAGE:
        switch ( source->type )
        {
           case GAN_FLOAT:
           {
              Gan_RGBAPixel_f pix;

              switch(scale->format)
              {
                 case GAN_GREY_LEVEL_IMAGE:
                 {
                    float scalef = scale->data.gl.f;

                    if(mask == NULL)
                    {
                       for ( row = window.r0; row < row_max; row++ )
                          for ( col = window.c0; col < col_max; col++ )
                          {
                             pix = gan_image_get_pix_rgba_f(source, row, col);
                             pix.R = scalef*pix.R;
                             pix.G = scalef*pix.G;
                             pix.B = scalef*pix.B;
                             pix.A = scalef*pix.A;
                             gan_image_set_pix_rgba_f ( dest, row, col, &pix );
                          }
                    }
                    else
                    {
                       for ( row = window.r0; row < row_max; row++ )
                          for ( col = window.c0; col < col_max; col++ )
                             if(gan_image_get_pix_b(mask, row, col))
                             {
                                pix = gan_image_get_pix_rgba_f(source, row,col);
                                pix.R = scalef*pix.R;
                                pix.G = scalef*pix.G;
                                pix.B = scalef*pix.B;
                                pix.A = scalef*pix.A;
                                gan_image_set_pix_rgba_f(dest, row, col, &pix);
                             }
                    }
                 }
                 break;

                 case GAN_RGB_COLOUR_ALPHA_IMAGE:
                   if(mask == NULL)
                   {
                      for ( row = window.r0; row < row_max; row++ )
                         for ( col = window.c0; col < col_max; col++ )
                         {
                            pix = gan_image_get_pix_rgba_f(source, row, col);
                            pix.R = scale->data.rgba.f.R*pix.R;
                            pix.G = scale->data.rgba.f.G*pix.G;
                            pix.B = scale->data.rgba.f.B*pix.B;
                            pix.A = scale->data.rgba.f.A*pix.A;
                            gan_image_set_pix_rgba_f ( dest, row, col, &pix );
                         }
                   }
                   else
                   {
                      for ( row = window.r0; row < row_max; row++ )
                         for ( col = window.c0; col < col_max; col++ )
                             if(gan_image_get_pix_b(mask, row, col))
                             {
                                pix = gan_image_get_pix_rgba_f(source, row,col);
                                pix.R = scale->data.rgba.f.R*pix.R;
                                pix.G = scale->data.rgba.f.G*pix.G;
                                pix.B = scale->data.rgba.f.B*pix.B;
                                pix.A = scale->data.rgba.f.A*pix.A;
                                gan_image_set_pix_rgba_f(dest, row, col, &pix);
                             }
                   }

                   break;

                 default:
                   gan_err_flush_trace();
                   gan_err_register ( "gan_image_scale_q", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
                   return NULL;
              }
           }

           break;

           case GAN_DOUBLE:
           {
              Gan_RGBAPixel_d pix;
              
              switch(scale->format)
              {
                 case GAN_GREY_LEVEL_IMAGE:
                 {
                    double scaled = (double)scale->data.gl.f;

                    if(mask == NULL)
                    {
                       for ( row = window.r0; row < row_max; row++ )
                          for ( col = window.c0; col < col_max; col++ )
                          {
                             pix = gan_image_get_pix_rgba_d (source, row, col);
                             pix.R = scaled*pix.R;
                             pix.G = scaled*pix.G;
                             pix.B = scaled*pix.B;
                             pix.A = scaled*pix.A;
                             gan_image_set_pix_rgba_d ( dest, row, col, &pix );
                          }
                    }
                    else
                    {
                       for ( row = window.r0; row < row_max; row++ )
                          for ( col = window.c0; col < col_max; col++ )
                             if(gan_image_get_pix_b(mask, row, col))
                             {
                                pix = gan_image_get_pix_rgba_d(source, row,col);
                                pix.R = scaled*pix.R;
                                pix.G = scaled*pix.G;
                                pix.B = scaled*pix.B;
                                pix.A = scaled*pix.A;
                                gan_image_set_pix_rgba_d (dest, row, col, &pix);
                             }
                    }
                 }
                 break;

                 case GAN_RGB_COLOUR_ALPHA_IMAGE:
                 {
                    Gan_RGBAPixel_d pixd;

                    pixd.R = (double)scale->data.rgba.f.R;
                    pixd.G = (double)scale->data.rgba.f.G;
                    pixd.B = (double)scale->data.rgba.f.B;
                    pixd.A = (double)scale->data.rgba.f.A;
                    
                    if(mask == NULL)
                    {
                       for ( row = window.r0; row < row_max; row++ )
                          for ( col = window.c0; col < col_max; col++ )
                          {
                             pix = gan_image_get_pix_rgba_d(source, row, col);
                             pix.R = pixd.R*pix.R;
                             pix.G = pixd.G*pix.G;
                             pix.B = pixd.B*pix.B;
                             pix.A = pixd.A*pix.A;
                             gan_image_set_pix_rgba_d ( dest, row, col, &pix );
                          }
                    }
                    else
                    {
                       for ( row = window.r0; row < row_max; row++ )
                          for ( col = window.c0; col < col_max; col++ )
                             if(gan_image_get_pix_b(mask, row, col))
                             {
                                pix = gan_image_get_pix_rgba_d(source, row,col);
                                pix.R = pixd.R*pix.R;
                                pix.G = pixd.G*pix.G;
                                pix.B = pixd.B*pix.B;
                                pix.A = pixd.A*pix.A;
                                gan_image_set_pix_rgba_d(dest, row, col, &pix);
                             }
                    }
                 }
                 break;

                 default:
                   gan_err_flush_trace();
                   gan_err_register ( "gan_image_scale_q", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
                   return NULL;
              }
           }
           break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_scale_q", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             return NULL;
        }

        break;

      case GAN_BGR_COLOUR_IMAGE:
        switch ( source->type )
        {
           case GAN_FLOAT:
           {
              Gan_BGRPixel_f pix;

              switch(scale->format)
              {
                 case GAN_GREY_LEVEL_IMAGE:
                 {
                    float scalef = scale->data.gl.f;

                    if(mask == NULL)
                    {
                       for ( row = window.r0; row < row_max; row++ )
                          for ( col = window.c0; col < col_max; col++ )
                          {
                             pix = gan_image_get_pix_bgr_f(source, row, col);
                             pix.R = scalef*pix.R;
                             pix.G = scalef*pix.G;
                             pix.B = scalef*pix.B;
                             gan_image_set_pix_bgr_f ( dest, row, col, &pix );
                          }
                    }
                    else
                    {
                       for ( row = window.r0; row < row_max; row++ )
                          for ( col = window.c0; col < col_max; col++ )
                             if(gan_image_get_pix_b(mask, row, col))
                             {
                                pix = gan_image_get_pix_bgr_f(source, row,col);
                                pix.R = scalef*pix.R;
                                pix.G = scalef*pix.G;
                                pix.B = scalef*pix.B;
                                gan_image_set_pix_bgr_f(dest, row, col, &pix);
                             }
                    }
                 }
                 break;

                 case GAN_BGR_COLOUR_IMAGE:
                   if(mask == NULL)
                   {
                      for ( row = window.r0; row < row_max; row++ )
                         for ( col = window.c0; col < col_max; col++ )
                         {
                            pix = gan_image_get_pix_bgr_f(source, row, col);
                            pix.R = scale->data.bgr.f.R*pix.R;
                            pix.G = scale->data.bgr.f.G*pix.G;
                            pix.B = scale->data.bgr.f.B*pix.B;
                            gan_image_set_pix_bgr_f ( dest, row, col, &pix );
                         }
                   }
                   else
                   {
                      for ( row = window.r0; row < row_max; row++ )
                         for ( col = window.c0; col < col_max; col++ )
                             if(gan_image_get_pix_b(mask, row, col))
                             {
                                pix = gan_image_get_pix_bgr_f(source, row,col);
                                pix.R = scale->data.bgr.f.R*pix.R;
                                pix.G = scale->data.bgr.f.G*pix.G;
                                pix.B = scale->data.bgr.f.B*pix.B;
                                gan_image_set_pix_bgr_f(dest, row, col, &pix);
                             }
                   }

                   break;

                 default:
                   gan_err_flush_trace();
                   gan_err_register ( "gan_image_scale_q", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
                   return NULL;
              }
           }

           break;

           case GAN_DOUBLE:
           {
              Gan_BGRPixel_d pix;
              
              switch(scale->format)
              {
                 case GAN_GREY_LEVEL_IMAGE:
                 {
                    double scaled = (double)scale->data.gl.f;

                    if(mask == NULL)
                    {
                       for ( row = window.r0; row < row_max; row++ )
                          for ( col = window.c0; col < col_max; col++ )
                          {
                             pix = gan_image_get_pix_bgr_d (source, row, col);
                             pix.R = scaled*pix.R;
                             pix.G = scaled*pix.G;
                             pix.B = scaled*pix.B;
                             gan_image_set_pix_bgr_d ( dest, row, col, &pix );
                          }
                    }
                    else
                    {
                       for ( row = window.r0; row < row_max; row++ )
                          for ( col = window.c0; col < col_max; col++ )
                             if(gan_image_get_pix_b(mask, row, col))
                             {
                                pix = gan_image_get_pix_bgr_d(source, row,col);
                                pix.R = scaled*pix.R;
                                pix.G = scaled*pix.G;
                                pix.B = scaled*pix.B;
                                gan_image_set_pix_bgr_d (dest, row, col, &pix);
                             }
                    }
                 }
                 break;

                 case GAN_BGR_COLOUR_IMAGE:
                 {
                    Gan_BGRPixel_d pixd;

                    pixd.R = (double)scale->data.bgr.f.R;
                    pixd.G = (double)scale->data.bgr.f.G;
                    pixd.B = (double)scale->data.bgr.f.B;
                    
                    if(mask == NULL)
                    {
                       for ( row = window.r0; row < row_max; row++ )
                          for ( col = window.c0; col < col_max; col++ )
                          {
                             pix = gan_image_get_pix_bgr_d(source, row, col);
                             pix.R = pixd.R*pix.R;
                             pix.G = pixd.G*pix.G;
                             pix.B = pixd.B*pix.B;
                             gan_image_set_pix_bgr_d ( dest, row, col, &pix );
                          }
                    }
                    else
                    {
                       for ( row = window.r0; row < row_max; row++ )
                          for ( col = window.c0; col < col_max; col++ )
                             if(gan_image_get_pix_b(mask, row, col))
                             {
                                pix = gan_image_get_pix_bgr_d(source, row,col);
                                pix.R = pixd.R*pix.R;
                                pix.G = pixd.G*pix.G;
                                pix.B = pixd.B*pix.B;
                                gan_image_set_pix_bgr_d(dest, row, col, &pix);
                             }
                    }
                 }
                 break;

                 default:
                   gan_err_flush_trace();
                   gan_err_register ( "gan_image_scale_q", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
                   return NULL;
              }
           }
           break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_scale_q", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             return NULL;
        }

        break;

      case GAN_BGR_COLOUR_ALPHA_IMAGE:
        switch ( source->type )
        {
           case GAN_FLOAT:
           {
              Gan_BGRAPixel_f pix;

              switch(scale->format)
              {
                 case GAN_GREY_LEVEL_IMAGE:
                 {
                    float scalef = scale->data.gl.f;

                    if(mask == NULL)
                    {
                       for ( row = window.r0; row < row_max; row++ )
                          for ( col = window.c0; col < col_max; col++ )
                          {
                             pix = gan_image_get_pix_bgra_f(source, row, col);
                             pix.R = scalef*pix.R;
                             pix.G = scalef*pix.G;
                             pix.B = scalef*pix.B;
                             pix.A = scalef*pix.A;
                             gan_image_set_pix_bgra_f ( dest, row, col, &pix );
                          }
                    }
                    else
                    {
                       for ( row = window.r0; row < row_max; row++ )
                          for ( col = window.c0; col < col_max; col++ )
                             if(gan_image_get_pix_b(mask, row, col))
                             {
                                pix = gan_image_get_pix_bgra_f(source, row,col);
                                pix.R = scalef*pix.R;
                                pix.G = scalef*pix.G;
                                pix.B = scalef*pix.B;
                                pix.A = scalef*pix.A;
                                gan_image_set_pix_bgra_f(dest, row, col, &pix);
                             }
                    }
                 }
                 break;

                 case GAN_BGR_COLOUR_ALPHA_IMAGE:
                   if(mask == NULL)
                   {
                      for ( row = window.r0; row < row_max; row++ )
                         for ( col = window.c0; col < col_max; col++ )
                         {
                            pix = gan_image_get_pix_bgra_f(source, row, col);
                            pix.R = scale->data.bgra.f.R*pix.R;
                            pix.G = scale->data.bgra.f.G*pix.G;
                            pix.B = scale->data.bgra.f.B*pix.B;
                            pix.A = scale->data.bgra.f.A*pix.A;
                            gan_image_set_pix_bgra_f ( dest, row, col, &pix );
                         }
                   }
                   else
                   {
                      for ( row = window.r0; row < row_max; row++ )
                         for ( col = window.c0; col < col_max; col++ )
                             if(gan_image_get_pix_b(mask, row, col))
                             {
                                pix = gan_image_get_pix_bgra_f(source, row,col);
                                pix.R = scale->data.bgra.f.R*pix.R;
                                pix.G = scale->data.bgra.f.G*pix.G;
                                pix.B = scale->data.bgra.f.B*pix.B;
                                pix.A = scale->data.bgra.f.A*pix.A;
                                gan_image_set_pix_bgra_f(dest, row, col, &pix);
                             }
                   }

                   break;

                 default:
                   gan_err_flush_trace();
                   gan_err_register ( "gan_image_scale_q", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
                   return NULL;
              }
           }

           break;

           case GAN_DOUBLE:
           {
              Gan_BGRAPixel_d pix;
              
              switch(scale->format)
              {
                 case GAN_GREY_LEVEL_IMAGE:
                 {
                    double scaled = (double)scale->data.gl.f;

                    if(mask == NULL)
                    {
                       for ( row = window.r0; row < row_max; row++ )
                          for ( col = window.c0; col < col_max; col++ )
                          {
                             pix = gan_image_get_pix_bgra_d (source, row, col);
                             pix.R = scaled*pix.R;
                             pix.G = scaled*pix.G;
                             pix.B = scaled*pix.B;
                             pix.A = scaled*pix.A;
                             gan_image_set_pix_bgra_d ( dest, row, col, &pix );
                          }
                    }
                    else
                    {
                       for ( row = window.r0; row < row_max; row++ )
                          for ( col = window.c0; col < col_max; col++ )
                             if(gan_image_get_pix_b(mask, row, col))
                             {
                                pix = gan_image_get_pix_bgra_d(source, row,col);
                                pix.R = scaled*pix.R;
                                pix.G = scaled*pix.G;
                                pix.B = scaled*pix.B;
                                pix.A = scaled*pix.A;
                                gan_image_set_pix_bgra_d (dest, row, col, &pix);
                             }
                    }
                 }
                 break;

                 case GAN_BGR_COLOUR_ALPHA_IMAGE:
                 {
                    Gan_BGRAPixel_d pixd;

                    pixd.R = (double)scale->data.bgra.f.R;
                    pixd.G = (double)scale->data.bgra.f.G;
                    pixd.B = (double)scale->data.bgra.f.B;
                    pixd.A = (double)scale->data.bgra.f.A;
                    
                    if(mask == NULL)
                    {
                       for ( row = window.r0; row < row_max; row++ )
                          for ( col = window.c0; col < col_max; col++ )
                          {
                             pix = gan_image_get_pix_bgra_d(source, row, col);
                             pix.R = pixd.R*pix.R;
                             pix.G = pixd.G*pix.G;
                             pix.B = pixd.B*pix.B;
                             pix.A = pixd.A*pix.A;
                             gan_image_set_pix_bgra_d ( dest, row, col, &pix );
                          }
                    }
                    else
                    {
                       for ( row = window.r0; row < row_max; row++ )
                          for ( col = window.c0; col < col_max; col++ )
                             if(gan_image_get_pix_b(mask, row, col))
                             {
                                pix = gan_image_get_pix_bgra_d(source, row,col);
                                pix.R = pixd.R*pix.R;
                                pix.G = pixd.G*pix.G;
                                pix.B = pixd.B*pix.B;
                                pix.A = pixd.A*pix.A;
                                gan_image_set_pix_bgra_d(dest, row, col, &pix);
                             }
                    }
                 }
                 break;

                 default:
                   gan_err_flush_trace();
                   gan_err_register ( "gan_image_scale_q", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
                   return NULL;
              }
           }
           break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_scale_q", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             return NULL;
        }

        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_image_scale_q", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
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

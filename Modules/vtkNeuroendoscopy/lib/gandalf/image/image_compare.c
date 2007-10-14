/**
 * File:          $RCSfile: image_compare.c,v $
 * Module:        Extract/fill selected channel(s) of image
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

#include <gandalf/image/image_compare.h>
#include <gandalf/image/pixel.h>
#include <gandalf/image/image_bit.h>

/**
 * \addtogroup ImagePackage
 * \{
 */

/**
 * \defgroup ImageCompare Image Comparison
 * \{
 */

/**
 * \brief Compares two images.
 * \param image1 The first image
 * \param image2 The second image
 * \param pixel_thres The threshold to decide if two pixels are the same
 * \param proportion_thres Thhreshold on the proportion of similar pixels
 * \param result Pointer to result enumerated variable
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Compares the format, dimensions and contents of two images. The pixel
 * comparison is to within the provided threshold \a pixel_thres. 
 * If \a proportion_thres is greater than zero, the \a result is returned as
 * #GAN_IMAGES_SIMILAR if the ratio of different pixels is
 * less than \a proportion_thres, or #GAN_IMAGES_NOT_SIMILAR if the ratio is
 * greater than \a proportion_thres. If \a proportion_thres if zero, a single
 * different pixel causes #GAN_IMAGE_PIXELS_DIFFERENT to be returned.
 * If all pixel differences are within \a pixel_thres then
 * #GAN_IMAGES_IDENTICAL is returned in \a result.
 */
Gan_Bool
 gan_image_compare ( Gan_Image *image1, Gan_Image *image2,
                     double pixel_thres, double proportion_thres,
                     Gan_PixelComparison *result )
{
   int row, col;
   Gan_Pixel pixel1, pixel2;
   unsigned long ulNoDiff = 0;
   
   if ( image1->format != image2->format )
   {
      *result = GAN_IMAGE_FORMATS_DIFFERENT;
      return GAN_TRUE;
   }
   
   if ( image1->width  != image2->width ||
        image1->height != image2->height )
   {
      *result = GAN_IMAGE_SIZES_DIFFERENT;
      return GAN_TRUE;
   }

   /* special case of boolean images; threshold is ignored */
   if ( image1->format == GAN_GREY_LEVEL_IMAGE && image1->type == GAN_BOOL &&
        image2->format == GAN_GREY_LEVEL_IMAGE && image2->type == GAN_BOOL )
   {
      Gan_Bool pix1, pix2;
      
      if ( proportion_thres == 0.0 )
      {
         for ( row = (int)image1->height-1; row >= 0; row-- )
            for ( col = (int)image1->width-1; col >= 0; col-- )
            {
               pix1 = gan_image_get_pix_b ( image1, row, col );
               pix2 = gan_image_get_pix_b ( image2, row, col );
               if ( (pix1 && !pix2) || (!pix1 && pix2) )
               {
                  *result = GAN_IMAGE_PIXELS_DIFFERENT;
                  return GAN_TRUE;
               }
            }

         *result = GAN_IMAGES_IDENTICAL;
      }
      else
      {
         for ( row = (int)image1->height-1; row >= 0; row-- )
            for ( col = (int)image1->width-1; col >= 0; col-- )
            {
               pix1 = gan_image_get_pix_b ( image1, row, col );
               pix2 = gan_image_get_pix_b ( image2, row, col );
               if ( (pix1 && !pix2) || (!pix1 && pix2) )
                  ulNoDiff++;
            }

         if ( (double)ulNoDiff/((double) (image1->height * image1->width))
              > proportion_thres )
            *result = GAN_IMAGES_NOT_SIMILAR;
         else
            *result = GAN_IMAGES_SIMILAR;
      }      

      return GAN_TRUE;         
   }


   /* compare images */
   switch ( image1->format )
   {
      case GAN_GREY_LEVEL_IMAGE:
        for ( row = (int)image1->height-1; row >= 0; row-- )
           for ( col = (int)image1->width-1; col >= 0; col-- )
           {
              /* get pixels and convert them to double precision */
              pixel1 = gan_image_get_pix ( image1, row, col );
              pixel2 = gan_image_get_pix ( image2, row, col );
              
              if ( !gan_image_convert_pixel_i ( &pixel1,
                                                GAN_GREY_LEVEL_IMAGE,
                                                GAN_DOUBLE ) ||
                   !gan_image_convert_pixel_i ( &pixel2,
                                                GAN_GREY_LEVEL_IMAGE,
                                                GAN_DOUBLE ) )
              {
                 gan_err_register ( "gan_image_compare",
                                    GAN_ERROR_FAILURE, "" );
                 return GAN_FALSE;
              }

              /* compare the pixel values */
              if ( fabs ( pixel1.data.gl.d - pixel2.data.gl.d ) > pixel_thres )
              {
                 if ( proportion_thres == 0.0 )
                 {
                    *result = GAN_IMAGE_PIXELS_DIFFERENT;
                    return GAN_TRUE;
                 }
                 else
                    ulNoDiff++;
              }
           }
        
        if ( proportion_thres > 0.0 )
        {
           if ( (double)ulNoDiff/((double) (image1->height * image1->width))
                > proportion_thres )
           {
              *result = GAN_IMAGES_NOT_SIMILAR;
              return GAN_TRUE;
           }
           else
           {
              *result = GAN_IMAGES_SIMILAR;
              return GAN_TRUE;         
           }
        }
        break;

      case GAN_GREY_LEVEL_ALPHA_IMAGE:
        for ( row = (int)image1->height-1; row >= 0; row-- )
           for ( col = (int)image1->width-1; col >= 0; col-- )
           {
              /* get pixels and convert them to double precision */
              pixel1 = gan_image_get_pix ( image1, row, col );
              pixel2 = gan_image_get_pix ( image2, row, col );
              
              if ( !gan_image_convert_pixel_i ( &pixel1,
                                                GAN_GREY_LEVEL_ALPHA_IMAGE,
                                                GAN_DOUBLE ) ||
                   !gan_image_convert_pixel_i ( &pixel2,
                                                GAN_GREY_LEVEL_ALPHA_IMAGE,
                                                GAN_DOUBLE ) )
              {
                 gan_err_register ( "gan_image_compare",
                                    GAN_ERROR_FAILURE, "" );
                 return GAN_FALSE;
              }

              
              /* compare the pixel values */
              if ( fabs ( pixel1.data.gla.d.I - pixel2.data.gla.d.I ) > pixel_thres ||
                   fabs ( pixel1.data.gla.d.A - pixel2.data.gla.d.A ) > pixel_thres )
              {
                 if ( proportion_thres == 0.0 )
                 {
                    *result = GAN_IMAGE_PIXELS_DIFFERENT;
                    return GAN_TRUE;
                 }
                 else
                    ulNoDiff++;
              }
              
           }
        
        if ( proportion_thres > 0.0 )
        {
           if ( (double)ulNoDiff/((double) (image1->height * image1->width))
                > proportion_thres )
           {
              *result = GAN_IMAGES_NOT_SIMILAR;
              return GAN_TRUE;
           }
           else
           {
              *result = GAN_IMAGES_SIMILAR;
              return GAN_TRUE;         
           }
        } 
        break;

      case GAN_RGB_COLOUR_IMAGE:
        for ( row = (int)image1->height-1; row >= 0; row-- )
           for ( col = (int)image1->width-1; col >= 0; col-- )
           {
              /* get pixels and convert them to double precision */
              pixel1 = gan_image_get_pix ( image1, row, col );
              pixel2 = gan_image_get_pix ( image2, row, col );
              
              if ( !gan_image_convert_pixel_i ( &pixel1,
                                                GAN_RGB_COLOUR_IMAGE,
                                                GAN_DOUBLE ) ||
                   !gan_image_convert_pixel_i ( &pixel2,
                                                GAN_RGB_COLOUR_IMAGE,
                                                GAN_DOUBLE ) )
              {
                 gan_err_register ( "gan_image_compare",
                                    GAN_ERROR_FAILURE, "" );
                 return GAN_FALSE;
              }

              /* compare the pixel values */
              if ( fabs (pixel1.data.rgb.d.R - pixel2.data.rgb.d.R) > pixel_thres ||
                   fabs (pixel1.data.rgb.d.G - pixel2.data.rgb.d.G) > pixel_thres ||
                   fabs (pixel1.data.rgb.d.B - pixel2.data.rgb.d.B) > pixel_thres )
              {
                 if ( proportion_thres == 0.0 )
                 {
                    *result = GAN_IMAGE_PIXELS_DIFFERENT;
                    return GAN_TRUE;
                 }
                 else
                    ulNoDiff++;
              }
           }

        if ( proportion_thres > 0.0 )
        {
           if ( (double)ulNoDiff/((double) (image1->height * image1->width))
                > proportion_thres )
           {
              *result = GAN_IMAGES_NOT_SIMILAR;
              return GAN_TRUE;
           }
           else
           {
              *result = GAN_IMAGES_SIMILAR;
              return GAN_TRUE;         
           }
        }
        break;

      case GAN_RGB_COLOUR_ALPHA_IMAGE:
        for ( row = (int)image1->height-1; row >= 0; row-- )
           for ( col = (int)image1->width-1; col >= 0; col-- )
           {
              /* get pixels and convert them to double precision */
              pixel1 = gan_image_get_pix ( image1, row, col );
              pixel2 = gan_image_get_pix ( image2, row, col );
              
              if ( !gan_image_convert_pixel_i ( &pixel1,
                                                GAN_RGB_COLOUR_ALPHA_IMAGE,
                                                GAN_DOUBLE ) ||
                   !gan_image_convert_pixel_i ( &pixel2,
                                                GAN_RGB_COLOUR_ALPHA_IMAGE,
                                                GAN_DOUBLE ) )
              {
                 gan_err_register ( "gan_image_compare",
                                    GAN_ERROR_FAILURE, "" );
                 return GAN_FALSE;
              }
              
              /* compare the pixel values */
              if (fabs (pixel1.data.rgba.d.R - pixel2.data.rgba.d.R) > pixel_thres ||
                  fabs (pixel1.data.rgba.d.G - pixel2.data.rgba.d.G) > pixel_thres ||
                  fabs (pixel1.data.rgba.d.B - pixel2.data.rgba.d.B) > pixel_thres ||
                  fabs (pixel1.data.rgba.d.A - pixel2.data.rgba.d.A) > pixel_thres)
              {
                 if ( proportion_thres == 0.0 )
                 {
                    *result = GAN_IMAGE_PIXELS_DIFFERENT;
                    return GAN_TRUE;
                 }
                 else
                    ulNoDiff++;
              }

           }
        
        if ( proportion_thres > 0.0 )
        {
           if ( (double)ulNoDiff/((double) (image1->height * image1->width))
                > proportion_thres )
           {
              *result = GAN_IMAGES_NOT_SIMILAR;
              return GAN_TRUE;
           }
           else
           {
              *result = GAN_IMAGES_SIMILAR;
              return GAN_TRUE;         
           }
        } 
        break;
        
      case GAN_BGR_COLOUR_IMAGE:
        for ( row = (int)image1->height-1; row >= 0; row-- )
           for ( col = (int)image1->width-1; col >= 0; col-- )
           {
              /* get pixels and convert them to double precision */
              pixel1 = gan_image_get_pix ( image1, row, col );
              pixel2 = gan_image_get_pix ( image2, row, col );
              
              if ( !gan_image_convert_pixel_i ( &pixel1,
                                                GAN_BGR_COLOUR_IMAGE,
                                                GAN_DOUBLE ) ||
                   !gan_image_convert_pixel_i ( &pixel2,
                                                GAN_BGR_COLOUR_IMAGE,
                                                GAN_DOUBLE ) )
              {
                 gan_err_register ( "gan_image_compare",
                                    GAN_ERROR_FAILURE, "" );
                 return GAN_FALSE;
              }

              /* compare the pixel values */
              if ( fabs (pixel1.data.bgr.d.R - pixel2.data.bgr.d.R) > pixel_thres ||
                   fabs (pixel1.data.bgr.d.G - pixel2.data.bgr.d.G) > pixel_thres ||
                   fabs (pixel1.data.bgr.d.B - pixel2.data.bgr.d.B) > pixel_thres )
              {
                 if ( proportion_thres == 0.0 )
                 {
                    *result = GAN_IMAGE_PIXELS_DIFFERENT;
                    return GAN_TRUE;
                 }
                 else
                    ulNoDiff++;
              }
           }

        if ( proportion_thres > 0.0 )
        {
           if ( (double)ulNoDiff/((double) (image1->height * image1->width))
                > proportion_thres )
           {
              *result = GAN_IMAGES_NOT_SIMILAR;
              return GAN_TRUE;
           }
           else
           {
              *result = GAN_IMAGES_SIMILAR;
              return GAN_TRUE;         
           }
        }
        break;

      case GAN_BGR_COLOUR_ALPHA_IMAGE:
        for ( row = (int)image1->height-1; row >= 0; row-- )
           for ( col = (int)image1->width-1; col >= 0; col-- )
           {
              /* get pixels and convert them to double precision */
              pixel1 = gan_image_get_pix ( image1, row, col );
              pixel2 = gan_image_get_pix ( image2, row, col );
              
              if ( !gan_image_convert_pixel_i ( &pixel1,
                                                GAN_BGR_COLOUR_ALPHA_IMAGE,
                                                GAN_DOUBLE ) ||
                   !gan_image_convert_pixel_i ( &pixel2,
                                                GAN_BGR_COLOUR_ALPHA_IMAGE,
                                                GAN_DOUBLE ) )
              {
                 gan_err_register ( "gan_image_compare",
                                    GAN_ERROR_FAILURE, "" );
                 return GAN_FALSE;
              }
              
              /* compare the pixel values */
              if (fabs (pixel1.data.bgra.d.R - pixel2.data.bgra.d.R) > pixel_thres ||
                  fabs (pixel1.data.bgra.d.G - pixel2.data.bgra.d.G) > pixel_thres ||
                  fabs (pixel1.data.bgra.d.B - pixel2.data.bgra.d.B) > pixel_thres ||
                  fabs (pixel1.data.bgra.d.A - pixel2.data.bgra.d.A) > pixel_thres)
              {
                 if ( proportion_thres == 0.0 )
                 {
                    *result = GAN_IMAGE_PIXELS_DIFFERENT;
                    return GAN_TRUE;
                 }
                 else
                    ulNoDiff++;
              }

           }
        
        if ( proportion_thres > 0.0 )
        {
           if ( (double)ulNoDiff/((double) (image1->height * image1->width))
                > proportion_thres )
           {
              *result = GAN_IMAGES_NOT_SIMILAR;
              return GAN_TRUE;
           }
           else
           {
              *result = GAN_IMAGES_SIMILAR;
              return GAN_TRUE;         
           }
        } 
        break;
        
      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_image_compare", GAN_ERROR_ILLEGAL_TYPE, "" );
        return GAN_FALSE;
   }

   /* success */
   *result = GAN_IMAGES_IDENTICAL;
   return GAN_TRUE;
}

/**
 * \}
 */

/**
 * \}
 */

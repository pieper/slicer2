/**
 * File:          $RCSfile: image_composite.c,v $
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

#include <gandalf/image/image_composite.h>
#include <gandalf/image/image_bit.h>

/**
 * \addtogroup ImagePackage
 * \{
 */

/**
 * \defgroup ImageComposite Composite one image on another
 * \{
 */

/**
 * \brief Composite images
 * \param source The input image 
 * \param source_with_alpha The input image with the alpha channel
 * \param source_mask Binary mask for source image or \c NULL
 * \param dest The destination image 
 * \return Composited \a dest image, or \c NULL on failure.
 *
 * Composites the image \a source_with_alpha onto the given image \a source,
 * writing the result onto \a dest.
 *
 * \sa gan_image_composite_i().
 */
Gan_Image *
 gan_image_composite_q ( Gan_Image *source,
                         const Gan_Image *source_with_alpha,
                         const Gan_Image *source_mask,
                         Gan_Image *dest )
{
   Gan_Pixel Pixel, SAPixel;

   if(dest == source_with_alpha)
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_image_composite_q", GAN_ERROR_INCOMPATIBLE, "");
      return NULL;
   }

   if ( dest == NULL )
      dest = gan_image_alloc ( source->format, source->type,
                               source->height, source->width );
   else if(source != dest &&
           (source->format != dest->format || source->type != dest->type ||
            source->width != dest->width || source->height != dest->height))
      dest = gan_image_set_format_type_dims(dest, source->format, source->type,
                                            source->height, source->width);

   if ( dest == NULL )
   {
      gan_err_register ( "gan_image_composite_q", GAN_ERROR_FAILURE, "");
      return NULL;
   }

   if(source_mask == NULL)
   {
      int iRow, iCol;

      for ( iRow = (int)source_with_alpha->height-1; iRow >= 0; iRow-- )
         for ( iCol = (int)source_with_alpha->width-1; iCol >= 0; iCol-- )
         {
            Pixel = gan_image_get_pix ( source, iRow+source_with_alpha->offset_y, iCol+source_with_alpha->offset_x );
            SAPixel = gan_image_get_pix ( source_with_alpha, iRow, iCol );
            if ( !gan_image_convert_pixel_i ( &Pixel,
                                              GAN_RGB_COLOUR_IMAGE,
                                              GAN_FLOAT ) ||
                 !gan_image_convert_pixel_i ( &SAPixel,
                                              GAN_RGB_COLOUR_ALPHA_IMAGE,
                                              GAN_FLOAT ) )
            {
               gan_err_register ( "gan_image_composite_q", GAN_ERROR_FAILURE, "");
               return NULL;
            }

            /* blend pixel */
            Pixel.data.rgb.f.R = SAPixel.data.rgba.f.A*SAPixel.data.rgba.f.R
                                 + (1.0F-SAPixel.data.rgba.f.A)*Pixel.data.rgb.f.R;
            Pixel.data.rgb.f.G = SAPixel.data.rgba.f.A*SAPixel.data.rgba.f.G
                                 + (1.0F-SAPixel.data.rgba.f.A)*Pixel.data.rgb.f.G;
            Pixel.data.rgb.f.B = SAPixel.data.rgba.f.A*SAPixel.data.rgba.f.B
                                 + (1.0F-SAPixel.data.rgba.f.A)*Pixel.data.rgb.f.B;

            /* replace blended pixel in image */
            if ( !gan_image_convert_pixel_i ( &Pixel, dest->format, dest->type )||
                 !gan_image_set_pix ( dest, iRow+source_with_alpha->offset_y, iCol+source_with_alpha->offset_x, &Pixel ) )
            {
               gan_err_register ( "gan_image_composite_q", GAN_ERROR_FAILURE, "");
               return NULL;
            }
         }
   }
   else
   {
      unsigned int uiRow, uiCol;
      Gan_ImageWindow window;
      unsigned int max_row, max_col;

      if(!gan_image_get_active_subwindow(source_mask, &window))
      {
         gan_err_register ( "gan_image_composite_q", GAN_ERROR_FAILURE, "");
         return NULL;
      }

      max_col = window.c0+window.width;
      max_row = window.r0+window.height;

      gan_err_test_ptr(source_mask->format == GAN_GREY_LEVEL_IMAGE, "gan_image_composite_q", GAN_ERROR_INCOMPATIBLE, "");

      if(dest != source)
      {
         if(gan_image_copy_q(source, dest) == NULL)
         {
            gan_err_register ( "gan_image_composite_q", GAN_ERROR_FAILURE, "");
            return NULL;
         }
      }
      
      switch(source_mask->type)
      {
         case GAN_BOOL:
           for ( uiRow = window.r0; uiRow < max_row; uiRow++)
              for ( uiCol = window.c0; uiCol < max_col; uiCol++)
                 if(gan_image_get_pix_b(source_mask, uiRow, uiCol))
                 {
                    Pixel = gan_image_get_pix ( source,
                                                uiRow+(unsigned int)source_with_alpha->offset_y,
                                                uiCol+(unsigned int)source_with_alpha->offset_x );
                    SAPixel = gan_image_get_pix ( source_with_alpha, uiRow, uiCol );
                    if ( !gan_image_convert_pixel_i ( &Pixel, GAN_RGB_COLOUR_IMAGE, GAN_FLOAT ) ||
                         !gan_image_convert_pixel_i ( &SAPixel, GAN_RGB_COLOUR_ALPHA_IMAGE, GAN_FLOAT ) )
                    {
                       gan_err_register ( "gan_image_composite_q", GAN_ERROR_FAILURE, "");
                       return NULL;
                    }

                    /* blend pixel */
                    Pixel.data.rgb.f.R = SAPixel.data.rgba.f.A*SAPixel.data.rgba.f.R + (1.0F-SAPixel.data.rgba.f.A)*Pixel.data.rgb.f.R;
                    Pixel.data.rgb.f.G = SAPixel.data.rgba.f.A*SAPixel.data.rgba.f.G + (1.0F-SAPixel.data.rgba.f.A)*Pixel.data.rgb.f.G;
                    Pixel.data.rgb.f.B = SAPixel.data.rgba.f.A*SAPixel.data.rgba.f.B + (1.0F-SAPixel.data.rgba.f.A)*Pixel.data.rgb.f.B;

                    /* replace blended pixel in image */
                    if ( !gan_image_convert_pixel_i ( &Pixel, dest->format, dest->type )||
                         !gan_image_set_pix ( dest,
                                              uiRow+(unsigned int)source_with_alpha->offset_y,
                                              uiCol+(unsigned int)source_with_alpha->offset_x, &Pixel ) )
                    {
                       gan_err_register ( "gan_image_composite_q", GAN_ERROR_FAILURE, "");
                       return NULL;
                    }
                 }

           break;

         default:
         {
            Gan_Pixel MaskPixel;
            float fAlpha;

            for ( uiRow = window.r0; uiRow < max_row; uiRow++)
               for ( uiCol = window.c0; uiCol < max_col; uiCol++)
                  if(!gan_image_get_pix_zero(source_mask, uiRow, uiCol))
                  {
                     Pixel = gan_image_get_pix ( source,
                                                 uiRow+(unsigned int)source_with_alpha->offset_y,
                                                 uiCol+(unsigned int)source_with_alpha->offset_x );
                     SAPixel = gan_image_get_pix ( source_with_alpha, uiRow, uiCol );
                     MaskPixel = gan_image_get_pix ( source_mask, uiRow, uiCol );
                     if ( !gan_image_convert_pixel_i ( &Pixel, GAN_RGB_COLOUR_IMAGE, GAN_FLOAT ) ||
                          !gan_image_convert_pixel_i ( &SAPixel, GAN_RGB_COLOUR_ALPHA_IMAGE, GAN_FLOAT ) ||
                          !gan_image_convert_pixel_i ( &MaskPixel, GAN_GREY_LEVEL_IMAGE, GAN_FLOAT ) )
                     {
                        gan_err_register ( "gan_image_composite_q", GAN_ERROR_FAILURE, "");
                        return NULL;
                     }

                     /* blend pixel */
                     fAlpha = MaskPixel.data.gl.f*SAPixel.data.rgba.f.A;
                     Pixel.data.rgb.f.R = fAlpha*SAPixel.data.rgb.f.R + (1.0F-fAlpha)*Pixel.data.rgb.f.R;
                     Pixel.data.rgb.f.G = fAlpha*SAPixel.data.rgb.f.G + (1.0F-fAlpha)*Pixel.data.rgb.f.G;
                     Pixel.data.rgb.f.B = fAlpha*SAPixel.data.rgb.f.B + (1.0F-fAlpha)*Pixel.data.rgb.f.B;

                     /* replace blended pixel in image */
                     if ( !gan_image_convert_pixel_i ( &Pixel, dest->format, dest->type )||
                          !gan_image_set_pix ( dest,
                                               uiRow+(unsigned int)source_with_alpha->offset_y,
                                               uiCol+(unsigned int)source_with_alpha->offset_x, &Pixel ) )
                     {
                        gan_err_register ( "gan_image_composite_q", GAN_ERROR_FAILURE, "");
                        return NULL;
                     }
                  }
         }
         break;
      }
   }

   /* success */
   return dest;
}

/**
 * \brief Blend images
 * \param source1 The first input image 
 * \param source2 The second input image
 * \param alpha Alpha image
 * \param invert_alpha Whether to invert the alpha image
 * \param mask Mask for blend
 * \param dest The destination image
 * \return Blended \a dest image, or \c NULL on failure.
 *
 * Blends the image \a source1 with \a source2, writing the result onto \a dest.
 *
 * \sa gan_image_blend_i().
 */
Gan_Image *
 gan_image_blend_q ( Gan_Image *source1,
                     const Gan_Image *source2,
                     const Gan_Image *alpha,
                     Gan_Bool invert_alpha,
                     const Gan_Image *mask,
                     Gan_Image *dest )
{
   Gan_Pixel Pixel, SAPixel;
   unsigned int uiRow, uiCol;
   Gan_ImageWindow window;
   unsigned int max_row, max_col;

   if(dest == source2)
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_image_blend_q", GAN_ERROR_INCOMPATIBLE, "");
      return NULL;
   }

   if ( dest == NULL )
      dest = gan_image_alloc ( source1->format, source1->type, source1->height, source1->width );
   else if(source1 != dest &&
           (source1->format != dest->format || source1->type != dest->type ||
            source1->width != dest->width || source1->height != dest->height))
      dest = gan_image_set_format_type_dims(dest, source1->format, source1->type, source1->height, source1->width);

   if ( dest == NULL )
   {
      gan_err_register ( "gan_image_blend_q", GAN_ERROR_FAILURE, "");
      return NULL;
   }

   if(dest != source1)
   {
      if(gan_image_copy_q(source1, dest) == NULL)
      {
         gan_err_register ( "gan_image_blend_q", GAN_ERROR_FAILURE, "");
         return NULL;
      }
   }

   if(invert_alpha)
   {
      window.c0 = window.r0 = 0;
      window.width = alpha->width;
      window.height = alpha->height;
   }
   else if(!gan_image_get_active_subwindow(alpha, &window))
   {
      gan_err_register ( "gan_image_blend_q", GAN_ERROR_FAILURE, "");
      return NULL;
   }

   if(window.width == 0)
      return dest;

   if(mask != NULL)
   {
      /* constrain window further using mask */
      Gan_ImageWindow mwindow;

      if(!gan_image_get_active_subwindow(mask, &mwindow))
      {
         gan_err_register ( "gan_image_blend_q", GAN_ERROR_FAILURE, "");
         return NULL;
      }

      gan_image_intersect_subwindows_i(&window, &mwindow);
      if(window.width == 0)
         return dest;
   }

   max_col = window.c0+window.width;
   max_row = window.r0+window.height;
   
   gan_err_test_ptr(alpha->format == GAN_GREY_LEVEL_IMAGE, "gan_image_blend_q", GAN_ERROR_INCOMPATIBLE, "");

   switch(alpha->type)
   {
      case GAN_BOOL:
        if(mask == NULL)
        {
           if(invert_alpha)
           {
              for ( uiRow = window.r0; uiRow < max_row; uiRow++)
                 for ( uiCol = window.c0; uiCol < max_col; uiCol++)
                    if(!gan_image_get_pix_b(alpha, uiRow, uiCol))
                    {
                       Pixel = gan_image_get_pix ( source1,
                                                   uiRow+(unsigned int)source2->offset_y,
                                                   uiCol+(unsigned int)source2->offset_x );
                       SAPixel = gan_image_get_pix ( source2, uiRow, uiCol );
                       if ( !gan_image_convert_pixel_i ( &Pixel, GAN_RGB_COLOUR_IMAGE, GAN_FLOAT ) ||
                            !gan_image_convert_pixel_i ( &SAPixel, GAN_RGB_COLOUR_ALPHA_IMAGE, GAN_FLOAT ) )
                       {
                          gan_err_register ( "gan_image_blend_q", GAN_ERROR_FAILURE, "");
                          return NULL;
                       }

                       /* blend pixel */
                       Pixel.data.rgb.f.R = SAPixel.data.rgba.f.A*SAPixel.data.rgba.f.R + (1.0F-SAPixel.data.rgba.f.A)*Pixel.data.rgb.f.R;
                       Pixel.data.rgb.f.G = SAPixel.data.rgba.f.A*SAPixel.data.rgba.f.G + (1.0F-SAPixel.data.rgba.f.A)*Pixel.data.rgb.f.G;
                       Pixel.data.rgb.f.B = SAPixel.data.rgba.f.A*SAPixel.data.rgba.f.B + (1.0F-SAPixel.data.rgba.f.A)*Pixel.data.rgb.f.B;

                       /* replace blended pixel in image */
                       if ( !gan_image_convert_pixel_i ( &Pixel, dest->format, dest->type )||
                            !gan_image_set_pix ( dest,
                                                 uiRow+(unsigned int)source2->offset_y,
                                                 uiCol+(unsigned int)source2->offset_x, &Pixel ) )
                       {
                          gan_err_register ( "gan_image_blend_q", GAN_ERROR_FAILURE, "");
                          return NULL;
                       }
                    }
           }
           else
           {
              for ( uiRow = window.r0; uiRow < max_row; uiRow++)
                 for ( uiCol = window.c0; uiCol < max_col; uiCol++)
                    if(gan_image_get_pix_b(alpha, uiRow, uiCol))
                    {
                       Pixel = gan_image_get_pix ( source1,
                                                   uiRow+(unsigned int)source2->offset_y,
                                                   uiCol+(unsigned int)source2->offset_x );
                       SAPixel = gan_image_get_pix ( source2, uiRow, uiCol );
                       if ( !gan_image_convert_pixel_i ( &Pixel, GAN_RGB_COLOUR_IMAGE, GAN_FLOAT ) ||
                            !gan_image_convert_pixel_i ( &SAPixel, GAN_RGB_COLOUR_ALPHA_IMAGE, GAN_FLOAT ) )
                       {
                          gan_err_register ( "gan_image_blend_q", GAN_ERROR_FAILURE, "");
                          return NULL;
                       }

                       /* blend pixel */
                       Pixel.data.rgb.f.R = SAPixel.data.rgba.f.A*SAPixel.data.rgba.f.R + (1.0F-SAPixel.data.rgba.f.A)*Pixel.data.rgb.f.R;
                       Pixel.data.rgb.f.G = SAPixel.data.rgba.f.A*SAPixel.data.rgba.f.G + (1.0F-SAPixel.data.rgba.f.A)*Pixel.data.rgb.f.G;
                       Pixel.data.rgb.f.B = SAPixel.data.rgba.f.A*SAPixel.data.rgba.f.B + (1.0F-SAPixel.data.rgba.f.A)*Pixel.data.rgb.f.B;

                       /* replace blended pixel in image */
                       if ( !gan_image_convert_pixel_i ( &Pixel, dest->format, dest->type )||
                            !gan_image_set_pix ( dest,
                                                 uiRow+(unsigned int)source2->offset_y,
                                                 uiCol+(unsigned int)source2->offset_x, &Pixel ) )
                       {
                          gan_err_register ( "gan_image_blend_q", GAN_ERROR_FAILURE, "");
                          return NULL;
                       }
                    }
           }
        }
        else
        {
           if(invert_alpha)
           {
              for ( uiRow = window.r0; uiRow < max_row; uiRow++)
                 for ( uiCol = window.c0; uiCol < max_col; uiCol++)
                    if(!gan_image_get_pix_b(alpha, uiRow, uiCol) && gan_image_get_pix_b(mask, uiRow, uiCol))
                    {
                       Pixel = gan_image_get_pix ( source1,
                                                   uiRow+(unsigned int)source2->offset_y,
                                                   uiCol+(unsigned int)source2->offset_x );
                       SAPixel = gan_image_get_pix ( source2, uiRow, uiCol );
                       if ( !gan_image_convert_pixel_i ( &Pixel, GAN_RGB_COLOUR_IMAGE, GAN_FLOAT ) ||
                            !gan_image_convert_pixel_i ( &SAPixel, GAN_RGB_COLOUR_ALPHA_IMAGE, GAN_FLOAT ) )
                       {
                          gan_err_register ( "gan_image_blend_q", GAN_ERROR_FAILURE, "");
                          return NULL;
                       }

                       /* blend pixel */
                       Pixel.data.rgb.f.R = SAPixel.data.rgba.f.A*SAPixel.data.rgba.f.R + (1.0F-SAPixel.data.rgba.f.A)*Pixel.data.rgb.f.R;
                       Pixel.data.rgb.f.G = SAPixel.data.rgba.f.A*SAPixel.data.rgba.f.G + (1.0F-SAPixel.data.rgba.f.A)*Pixel.data.rgb.f.G;
                       Pixel.data.rgb.f.B = SAPixel.data.rgba.f.A*SAPixel.data.rgba.f.B + (1.0F-SAPixel.data.rgba.f.A)*Pixel.data.rgb.f.B;

                       /* replace blended pixel in image */
                       if ( !gan_image_convert_pixel_i ( &Pixel, dest->format, dest->type )||
                            !gan_image_set_pix ( dest,
                                                 uiRow+(unsigned int)source2->offset_y,
                                                 uiCol+(unsigned int)source2->offset_x, &Pixel ) )
                       {
                          gan_err_register ( "gan_image_blend_q", GAN_ERROR_FAILURE, "");
                          return NULL;
                       }
                    }
           }
           else
           {
              for ( uiRow = window.r0; uiRow < max_row; uiRow++)
                 for ( uiCol = window.c0; uiCol < max_col; uiCol++)
                    if(gan_image_get_pix_b(alpha, uiRow, uiCol) && gan_image_get_pix_b(mask, uiRow, uiCol))
                    {
                       Pixel = gan_image_get_pix ( source1,
                                                   uiRow+(unsigned int)source2->offset_y,
                                                   uiCol+(unsigned int)source2->offset_x );
                       SAPixel = gan_image_get_pix ( source2, uiRow, uiCol );
                       if ( !gan_image_convert_pixel_i ( &Pixel, GAN_RGB_COLOUR_IMAGE, GAN_FLOAT ) ||
                            !gan_image_convert_pixel_i ( &SAPixel, GAN_RGB_COLOUR_ALPHA_IMAGE, GAN_FLOAT ) )
                       {
                          gan_err_register ( "gan_image_blend_q", GAN_ERROR_FAILURE, "");
                          return NULL;
                       }

                       /* blend pixel */
                       Pixel.data.rgb.f.R = SAPixel.data.rgba.f.A*SAPixel.data.rgba.f.R + (1.0F-SAPixel.data.rgba.f.A)*Pixel.data.rgb.f.R;
                       Pixel.data.rgb.f.G = SAPixel.data.rgba.f.A*SAPixel.data.rgba.f.G + (1.0F-SAPixel.data.rgba.f.A)*Pixel.data.rgb.f.G;
                       Pixel.data.rgb.f.B = SAPixel.data.rgba.f.A*SAPixel.data.rgba.f.B + (1.0F-SAPixel.data.rgba.f.A)*Pixel.data.rgb.f.B;

                       /* replace blended pixel in image */
                       if ( !gan_image_convert_pixel_i ( &Pixel, dest->format, dest->type )||
                            !gan_image_set_pix ( dest,
                                                 uiRow+(unsigned int)source2->offset_y,
                                                 uiCol+(unsigned int)source2->offset_x, &Pixel ) )
                       {
                          gan_err_register ( "gan_image_blend_q", GAN_ERROR_FAILURE, "");
                          return NULL;
                       }
                    }
           }
        }

        break;

      default:
      {
         Gan_Pixel MaskPixel;

         if(mask == NULL)
         {
            if(invert_alpha)
            {
               for ( uiRow = window.r0; uiRow < max_row; uiRow++)
                  for ( uiCol = window.c0; uiCol < max_col; uiCol++)
                  {
                     Pixel = gan_image_get_pix ( source1,
                                                 uiRow+(unsigned int)source2->offset_y,
                                                 uiCol+(unsigned int)source2->offset_x );
                     SAPixel = gan_image_get_pix ( source2, uiRow, uiCol );
                     MaskPixel = gan_image_get_pix ( alpha, uiRow, uiCol );
                     if ( !gan_image_convert_pixel_i ( &Pixel, GAN_RGB_COLOUR_IMAGE, GAN_FLOAT ) ||
                          !gan_image_convert_pixel_i ( &SAPixel, GAN_RGB_COLOUR_IMAGE, GAN_FLOAT ) ||
                          !gan_image_convert_pixel_i ( &MaskPixel, GAN_GREY_LEVEL_IMAGE, GAN_FLOAT ) )
                     {
                        gan_err_register ( "gan_image_blend_q", GAN_ERROR_FAILURE, "");
                        return NULL;
                     }

                     /* blend pixel */
                     Pixel.data.rgb.f.R = (1.0F-MaskPixel.data.gl.f)*SAPixel.data.rgb.f.R + MaskPixel.data.gl.f*Pixel.data.rgb.f.R;
                     Pixel.data.rgb.f.G = (1.0F-MaskPixel.data.gl.f)*SAPixel.data.rgb.f.G + MaskPixel.data.gl.f*Pixel.data.rgb.f.G;
                     Pixel.data.rgb.f.B = (1.0F-MaskPixel.data.gl.f)*SAPixel.data.rgb.f.B + MaskPixel.data.gl.f*Pixel.data.rgb.f.B;

                     /* replace blended pixel in image */
                     if ( !gan_image_convert_pixel_i ( &Pixel, dest->format, dest->type )||
                          !gan_image_set_pix ( dest,
                                               uiRow+(unsigned int)source2->offset_y,
                                               uiCol+(unsigned int)source2->offset_x, &Pixel ) )
                     {
                        gan_err_register ( "gan_image_blend_q", GAN_ERROR_FAILURE, "");
                        return NULL;
                     }
                  }
            }
            else
            {
               for ( uiRow = window.r0; uiRow < max_row; uiRow++)
                  for ( uiCol = window.c0; uiCol < max_col; uiCol++)
                     if(!gan_image_get_pix_zero(alpha, uiRow, uiCol))
                     {
                        Pixel = gan_image_get_pix ( source1,
                                                    uiRow+(unsigned int)source2->offset_y,
                                                    uiCol+(unsigned int)source2->offset_x );
                        SAPixel = gan_image_get_pix ( source2, uiRow, uiCol );
                        MaskPixel = gan_image_get_pix ( alpha, uiRow, uiCol );
                        if ( !gan_image_convert_pixel_i ( &Pixel, GAN_RGB_COLOUR_IMAGE, GAN_FLOAT ) ||
                             !gan_image_convert_pixel_i ( &SAPixel, GAN_RGB_COLOUR_IMAGE, GAN_FLOAT ) ||
                             !gan_image_convert_pixel_i ( &MaskPixel, GAN_GREY_LEVEL_IMAGE, GAN_FLOAT ) )
                        {
                           gan_err_register ( "gan_image_blend_q", GAN_ERROR_FAILURE, "");
                           return NULL;
                        }

                        /* blend pixel */
                        Pixel.data.rgb.f.R = MaskPixel.data.gl.f*SAPixel.data.rgb.f.R + (1.0F-MaskPixel.data.gl.f)*Pixel.data.rgb.f.R;
                        Pixel.data.rgb.f.G = MaskPixel.data.gl.f*SAPixel.data.rgb.f.G + (1.0F-MaskPixel.data.gl.f)*Pixel.data.rgb.f.G;
                        Pixel.data.rgb.f.B = MaskPixel.data.gl.f*SAPixel.data.rgb.f.B + (1.0F-MaskPixel.data.gl.f)*Pixel.data.rgb.f.B;

                        /* replace blended pixel in image */
                        if ( !gan_image_convert_pixel_i ( &Pixel, dest->format, dest->type )||
                             !gan_image_set_pix ( dest,
                                                  uiRow+(unsigned int)source2->offset_y,
                                                  uiCol+(unsigned int)source2->offset_x, &Pixel ) )
                        {
                           gan_err_register ( "gan_image_blend_q", GAN_ERROR_FAILURE, "");
                           return NULL;
                        }
                     }
            }
         }
         else
         {
            if(invert_alpha)
            {
               for ( uiRow = window.r0; uiRow < max_row; uiRow++)
                  for ( uiCol = window.c0; uiCol < max_col; uiCol++)
                     if(gan_image_get_pix_b(mask, uiRow, uiCol))
                     {
                        Pixel = gan_image_get_pix ( source1,
                                                    uiRow+(unsigned int)source2->offset_y,
                                                    uiCol+(unsigned int)source2->offset_x );
                        SAPixel = gan_image_get_pix ( source2, uiRow, uiCol );
                        MaskPixel = gan_image_get_pix ( alpha, uiRow, uiCol );
                        if ( !gan_image_convert_pixel_i ( &Pixel, GAN_RGB_COLOUR_IMAGE, GAN_FLOAT ) ||
                             !gan_image_convert_pixel_i ( &SAPixel, GAN_RGB_COLOUR_IMAGE, GAN_FLOAT ) ||
                             !gan_image_convert_pixel_i ( &MaskPixel, GAN_GREY_LEVEL_IMAGE, GAN_FLOAT ) )
                        {
                           gan_err_register ( "gan_image_blend_q", GAN_ERROR_FAILURE, "");
                           return NULL;
                        }

                        /* blend pixel */
                        Pixel.data.rgb.f.R = (1.0F-MaskPixel.data.gl.f)*SAPixel.data.rgb.f.R + MaskPixel.data.gl.f*Pixel.data.rgb.f.R;
                        Pixel.data.rgb.f.G = (1.0F-MaskPixel.data.gl.f)*SAPixel.data.rgb.f.G + MaskPixel.data.gl.f*Pixel.data.rgb.f.G;
                        Pixel.data.rgb.f.B = (1.0F-MaskPixel.data.gl.f)*SAPixel.data.rgb.f.B + MaskPixel.data.gl.f*Pixel.data.rgb.f.B;

                        /* replace blended pixel in image */
                        if ( !gan_image_convert_pixel_i ( &Pixel, dest->format, dest->type )||
                             !gan_image_set_pix ( dest,
                                                  uiRow+(unsigned int)source2->offset_y,
                                                  uiCol+(unsigned int)source2->offset_x, &Pixel ) )
                        {
                           gan_err_register ( "gan_image_blend_q", GAN_ERROR_FAILURE, "");
                           return NULL;
                        }
                     }
            }
            else
            {
               for ( uiRow = window.r0; uiRow < max_row; uiRow++)
                  for ( uiCol = window.c0; uiCol < max_col; uiCol++)
                     if(gan_image_get_pix_b(mask, uiRow, uiCol) && !gan_image_get_pix_zero(alpha, uiRow, uiCol))
                     {
                        Pixel = gan_image_get_pix ( source1,
                                                    uiRow+(unsigned int)source2->offset_y,
                                                    uiCol+(unsigned int)source2->offset_x );
                        SAPixel = gan_image_get_pix ( source2, uiRow, uiCol );
                        MaskPixel = gan_image_get_pix ( alpha, uiRow, uiCol );
                        if ( !gan_image_convert_pixel_i ( &Pixel, GAN_RGB_COLOUR_IMAGE, GAN_FLOAT ) ||
                             !gan_image_convert_pixel_i ( &SAPixel, GAN_RGB_COLOUR_IMAGE, GAN_FLOAT ) ||
                             !gan_image_convert_pixel_i ( &MaskPixel, GAN_GREY_LEVEL_IMAGE, GAN_FLOAT ) )
                        {
                           gan_err_register ( "gan_image_blend_q", GAN_ERROR_FAILURE, "");
                           return NULL;
                        }

                        /* blend pixel */
                        Pixel.data.rgb.f.R = MaskPixel.data.gl.f*SAPixel.data.rgb.f.R + (1.0F-MaskPixel.data.gl.f)*Pixel.data.rgb.f.R;
                        Pixel.data.rgb.f.G = MaskPixel.data.gl.f*SAPixel.data.rgb.f.G + (1.0F-MaskPixel.data.gl.f)*Pixel.data.rgb.f.G;
                        Pixel.data.rgb.f.B = MaskPixel.data.gl.f*SAPixel.data.rgb.f.B + (1.0F-MaskPixel.data.gl.f)*Pixel.data.rgb.f.B;

                        /* replace blended pixel in image */
                        if ( !gan_image_convert_pixel_i ( &Pixel, dest->format, dest->type )||
                             !gan_image_set_pix ( dest,
                                                  uiRow+(unsigned int)source2->offset_y,
                                                  uiCol+(unsigned int)source2->offset_x, &Pixel ) )
                        {
                           gan_err_register ( "gan_image_blend_q", GAN_ERROR_FAILURE, "");
                           return NULL;
                        }
                     }
            }
         }
      }
      break;
   }

   /* success */
   return dest;
}

/**
 * \brief Blend images with fixed alpha
 * \param source1 The first input image 
 * \param source2 The second input image
 * \param alpha Alpha value
 * \param mask Mask for blend
 * \param dest The destination image
 * \return Blended \a dest image, or \c NULL on failure.
 *
 * Blends the image \a source1 with \a source2, writing the result onto \a dest.
 *
 * \sa gan_image_blend_i().
 */
Gan_Image *
 gan_image_blend_fixed_alpha_q ( Gan_Image *source1,
                                 const Gan_Image *source2,
                                 float alpha,
                                 const Gan_Image *mask,
                                 Gan_Image *dest )
{
   Gan_Pixel Pixel, SAPixel;
   unsigned int uiRow, uiCol;
   Gan_ImageWindow window;
   unsigned int max_row, max_col;

   if(dest == source2)
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_image_blend_fixed_alpha_q", GAN_ERROR_INCOMPATIBLE, "");
      return NULL;
   }

   if ( dest == NULL )
      dest = gan_image_alloc ( source1->format, source1->type, source1->height, source1->width );
   else if(source1 != dest &&
           (source1->format != dest->format || source1->type != dest->type ||
            source1->width != dest->width || source1->height != dest->height))
      dest = gan_image_set_format_type_dims(dest, source1->format, source1->type, source1->height, source1->width);

   if ( dest == NULL )
   {
      gan_err_register ( "gan_image_blend_fixed_alpha_q", GAN_ERROR_FAILURE, "");
      return NULL;
   }

   if(dest != source1)
   {
      if(gan_image_copy_q(source1, dest) == NULL)
      {
         gan_err_register ( "gan_image_blend_fixed_alpha_q", GAN_ERROR_FAILURE, "");
         return NULL;
      }
   }

   if(mask == NULL)
   {
      window.c0 = window.r0 = 0;
      window.width = source1->width;
      window.height = source1->height;
   }
   else
   {
      /* constrain window using mask */
      if(!gan_image_get_active_subwindow(mask, &window))
      {
         gan_err_register ( "gan_image_blend_fixed_alpha_q", GAN_ERROR_FAILURE, "");
         return NULL;
      }

      if(window.width == 0)
         return dest;
   }

   max_col = window.c0+window.width;
   max_row = window.r0+window.height;
   
   if(mask == NULL)
   {
      for ( uiRow = window.r0; uiRow < max_row; uiRow++)
         for ( uiCol = window.c0; uiCol < max_col; uiCol++)
         {
            Pixel = gan_image_get_pix ( source1,
                                        uiRow+(unsigned int)source2->offset_y,
                                        uiCol+(unsigned int)source2->offset_x );
            SAPixel = gan_image_get_pix ( source2, uiRow, uiCol );
            if ( !gan_image_convert_pixel_i ( &Pixel, GAN_RGB_COLOUR_IMAGE, GAN_FLOAT ) ||
                 !gan_image_convert_pixel_i ( &SAPixel, GAN_RGB_COLOUR_IMAGE, GAN_FLOAT ))
            {
               gan_err_register ( "gan_image_blend_fixed_alpha_q", GAN_ERROR_FAILURE, "");
               return NULL;
            }

            /* blend pixel */
            Pixel.data.rgb.f.R = alpha*SAPixel.data.rgb.f.R + (1.0F-alpha)*Pixel.data.rgb.f.R;
            Pixel.data.rgb.f.G = alpha*SAPixel.data.rgb.f.G + (1.0F-alpha)*Pixel.data.rgb.f.G;
            Pixel.data.rgb.f.B = alpha*SAPixel.data.rgb.f.B + (1.0F-alpha)*Pixel.data.rgb.f.B;

            /* replace blended pixel in image */
            if ( !gan_image_convert_pixel_i ( &Pixel, dest->format, dest->type )||
                 !gan_image_set_pix ( dest,
                                      uiRow+(unsigned int)source2->offset_y,
                                      uiCol+(unsigned int)source2->offset_x, &Pixel ) )
            {
               gan_err_register ( "gan_image_blend_fixed_alpha_q", GAN_ERROR_FAILURE, "");
               return NULL;
            }
         }
   }
   else
   {
      for ( uiRow = window.r0; uiRow < max_row; uiRow++)
         for ( uiCol = window.c0; uiCol < max_col; uiCol++)
            if(gan_image_get_pix_b(mask, uiRow, uiCol))
            {
               Pixel = gan_image_get_pix ( source1,
                                           uiRow+(unsigned int)source2->offset_y,
                                           uiCol+(unsigned int)source2->offset_x );
               SAPixel = gan_image_get_pix ( source2, uiRow, uiCol );
               if ( !gan_image_convert_pixel_i ( &Pixel, GAN_RGB_COLOUR_IMAGE, GAN_FLOAT ) ||
                    !gan_image_convert_pixel_i ( &SAPixel, GAN_RGB_COLOUR_IMAGE, GAN_FLOAT ))
               {
                  gan_err_register ( "gan_image_blend_fixed_alpha_q", GAN_ERROR_FAILURE, "");
                  return NULL;
               }

               /* blend pixel */
               Pixel.data.rgb.f.R = alpha*SAPixel.data.rgb.f.R + (1.0F-alpha)*Pixel.data.rgb.f.R;
               Pixel.data.rgb.f.G = alpha*SAPixel.data.rgb.f.G + (1.0F-alpha)*Pixel.data.rgb.f.G;
               Pixel.data.rgb.f.B = alpha*SAPixel.data.rgb.f.B + (1.0F-alpha)*Pixel.data.rgb.f.B;

               /* replace blended pixel in image */
               if ( !gan_image_convert_pixel_i ( &Pixel, dest->format, dest->type )||
                    !gan_image_set_pix ( dest,
                                         uiRow+(unsigned int)source2->offset_y,
                                         uiCol+(unsigned int)source2->offset_x, &Pixel ) )
               {
                  gan_err_register ( "gan_image_blend_fixed_alpha_q", GAN_ERROR_FAILURE, "");
                  return NULL;
               }
            }
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

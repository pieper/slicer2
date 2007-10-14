/**
 * File:          $RCSfile: image_colour_noc.c,v $
 * Module:        Colour image macros & definitions
 * Part of:       Gandalf Library 
 *
 * Revision:      $Revision: 1.1.2.1 $
 * Last edited:   $Date: 2007/10/14 02:20:18 $
 * Author:        $Author: ruetz $
 * Copyright:     (c) 2000 Imagineer Software Limited
 *
 * Notes:         Not to be complled separately
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

#include <string.h>
#include <gandalf/image/image_defs.h>
#include <gandalf/common/array.h>
#include <gandalf/common/allocate.h>
#include <gandalf/common/misc_error.h>
#include <gandalf/common/compare.h>

/**
 * \addtogroup ImagePackage
 * \{
 */

/* this file contains image package functions common to all colour image
 * types, as controlled by preprocessor symbols. The meanings of GAN_PIXEL,
 * GAN_PIXFMT, GAN_PIXTYPE, GAN_IMFMT and GAN_IMTYPE are explained in
 * image_common_noc.c.
 */

/* forward declaration of function defined in image_common_noc.c */
static Gan_Bool
 image_realloc ( Gan_Image *img,
                 unsigned long height, unsigned long width,
                 unsigned long stride, Gan_Bool alloc_pix_data );

#ifndef NDEBUG

/* set pixel value */
static Gan_Bool
 image_set_pix ( Gan_Image *img, unsigned row, unsigned col, GAN_PIXEL *pix )
{
   /* consistency check */
   gan_err_test_bool ( img->format == GAN_PIXFMT && img->type == GAN_PIXTYPE,
                       "image_set_pix", GAN_ERROR_INCOMPATIBLE, "" );

   gan_err_test_bool ( row < img->height && col < img->width,
                       "image_set_pix", GAN_ERROR_ILLEGAL_ARGUMENT,
                       "pixel position" );

   /* set pixel */
   img->row_data.GAN_IMFMT.GAN_IMTYPE[row][col] = *pix;

   /* success */
   return GAN_TRUE;
}

/* get pixel value */
static GAN_PIXEL
 image_get_pix ( const Gan_Image *img, unsigned row, unsigned col )
{
   /* consistency check */
   assert ( img->format == GAN_PIXFMT && img->type == GAN_PIXTYPE );

   gan_assert ( row < img->height && col < img->width,
                "illegal pixel position in image_get_pix()" );

   /* get pixel */
   return img->row_data.GAN_IMFMT.GAN_IMTYPE[row][col];
}

#endif /* #ifndef NDEBUG */

/* fill image with constant pixel value */
static Gan_Bool
 image_fill_const ( Gan_Image *img, GAN_PIXEL *pix )
{
   int i;

   /* consistency check */
   gan_err_test_bool ( img->format == GAN_PIXFMT && img->type == GAN_PIXTYPE,
                       "image_fill_const_?", GAN_ERROR_INCOMPATIBLE,
                       "image format/type" );

   /* set all pixels */
   if ( img->height == 0 ) return GAN_TRUE;
   if ( img->stride == img->width*sizeof(GAN_PIXEL) )
      /* fill all pixels in one go */
      for ( i = img->height*img->width-1; i >= 0; i-- )
         img->row_data.GAN_IMFMT.GAN_IMTYPE[0][i] = *pix;
   else
   {
      /* fill image one row at a time */
      int r;

      for ( r = img->height-1; r >= 0; r-- )
         for ( i = img->width-1; i >= 0; i-- )
            img->row_data.GAN_IMFMT.GAN_IMTYPE[r][i] = *pix;
   }

   /* success */
   return GAN_TRUE;
}

/* copy image */
static Gan_Image *
 image_copy ( const Gan_Image *img_s, Gan_Image *img_d )
{
   /* consistency check */
   assert ( img_s->format == GAN_PIXFMT && img_s->type == GAN_PIXTYPE );

   /* return immediately if images are identical */
   if ( img_s == img_d ) return img_d;

   /* allocate image img_d if necessary */
   if ( img_d == NULL )
   {
      img_d = GAN_IMAGE_FORM_GEN ( NULL, img_s->height, img_s->width,
                                            gan_image_min_stride(GAN_PIXFMT,
                                                                 GAN_PIXTYPE,
                                                                 img_s->width,
                                                                 0),
                                            GAN_TRUE, NULL, 0, NULL, 0 );
      if ( img_d == NULL )
      {
         gan_err_register("image_copy", GAN_ERROR_FAILURE, "");
         return NULL;
      }
   }
   else
   {
      if ( img_d->format != img_s->format || img_d->type != img_s->type )
      {
         /* make formats and types compatible */
         img_d = GAN_IMAGE_SET_GEN ( img_d, img_s->height, img_s->width,
                                     gan_image_min_stride(GAN_PIXFMT, GAN_PIXTYPE, img_s->width,0),
                                     GAN_TRUE );
         if ( img_d == NULL )
         {
            gan_err_register("image_copy", GAN_ERROR_FAILURE, "");
            return NULL;
         }
      }
      else
      {
         /* reallocate image if necessary */
         if ( !image_realloc ( img_d, img_s->height, img_s->width,
                               gan_image_min_stride(GAN_PIXFMT, GAN_PIXTYPE, img_s->width, 0), GAN_TRUE ) )
         {
            gan_err_register("image_copy", GAN_ERROR_FAILURE, "");
            return NULL;
         }
      }
   }

   /* if the image has no pixels, return immediately */
   if ( img_s->width == 0 || img_s->height == 0 ) return img_d;

   /* copy image data */
   if ( img_s->stride == img_s->width*sizeof(GAN_PIXEL) &&
        img_d->stride == img_d->width*sizeof(GAN_PIXEL) )
      /* copy all pixels in one go */
      memcpy ( (void *)img_d->pix_data_ptr, (void *)img_s->pix_data_ptr,
               img_s->height*img_s->width*sizeof(GAN_PIXEL) );
   else
   {
      /* copy image one row at a time */
      int r;

      for ( r = img_s->height-1; r >= 0; r-- )
         memcpy ( (void *)img_d->row_data.GAN_IMFMT.GAN_IMTYPE[r],
                  (void *)img_s->row_data.GAN_IMFMT.GAN_IMTYPE[r],
                  img_s->width*sizeof(GAN_PIXEL) );
   }

   img_d->offset_x = img_s->offset_x;
   img_d->offset_y = img_s->offset_y;
   return img_d;
}

/**
 * \addtogroup ImageExtract
 * \{
 */

/**
 * \brief Computes bounding box of non-zero pixels in grey-level image.
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Computes image window which covers all pixels in given grey-level \a image
 * which are non-zero. If \a image is zero everywhere the fields of the
 * \a subwin structure will be set to zero.
 *
 */
Gan_Bool
 GAN_IMAGE_GET_ACTIVE_SUBWINDOW ( const Gan_Image *image,
                                  Gan_ImageWindow *subwin )
{
   int row, col;
   int xmin = image->width, xmax = -1, ymin = image->height, ymax = -1;
   GAN_PIXEL *pix;

   if ( image->format != GAN_GREY_LEVEL_IMAGE )
   {
      gan_err_flush_trace();
      gan_err_register ( "GAN_IMAGE_GET_ACTIVE_SUBWINDOW",
                         GAN_ERROR_ILLEGAL_ARGUMENT, "" );
      return GAN_FALSE;
   }

   for ( row = (int)image->height-1; row >= 0; row-- )
   {
      for ( col = 0; col < xmin; col++ )
      {
         pix = &image->row_data.GAN_IMFMT.GAN_IMTYPE[row][col];
         if (    pix->GAN_PIX_FIELD1 != GAN_IMAGE_PIXEL_ZERO_VAL
#ifdef GAN_PIX_FIELD2
              || pix->GAN_PIX_FIELD2 != GAN_IMAGE_PIXEL_ZERO_VAL
#endif
#ifdef GAN_PIX_FIELD3
              || pix->GAN_PIX_FIELD3 != GAN_IMAGE_PIXEL_ZERO_VAL
#endif
#ifdef GAN_PIX_FIELD4
              || pix->GAN_PIX_FIELD4 != GAN_IMAGE_PIXEL_ZERO_VAL
#endif
                 )
         {
            /* update bounds if necessary */
            ymin = row;
            if ( ymax == -1 ) ymax = row;
            xmin = col;
            break;
         }
      }

      for ( col = (int)image->width-1; col > xmax; col-- )
      {
         pix = &image->row_data.GAN_IMFMT.GAN_IMTYPE[row][col];
         if (    pix->GAN_PIX_FIELD1 != GAN_IMAGE_PIXEL_ZERO_VAL
#ifdef GAN_PIX_FIELD2
              || pix->GAN_PIX_FIELD2 != GAN_IMAGE_PIXEL_ZERO_VAL
#endif
#ifdef GAN_PIX_FIELD3
              || pix->GAN_PIX_FIELD3 != GAN_IMAGE_PIXEL_ZERO_VAL
#endif
#ifdef GAN_PIX_FIELD4
              || pix->GAN_PIX_FIELD4 != GAN_IMAGE_PIXEL_ZERO_VAL
#endif
                 )
         {
            /* update bounds if necessary */
            ymin = row;
            if ( ymax == -1 ) ymax = row;
            xmax = col;
            break;
         }
      }

      /* if we haven't found a non-zero pixel outside previous horizontal
         range, we need to check the other pixels to update the vertical
         range */
      if ( ymin != row )
         for ( col = xmin; col <= xmax; col++ )
         {
            pix = &image->row_data.GAN_IMFMT.GAN_IMTYPE[row][col];
            if (    pix->GAN_PIX_FIELD1 != GAN_IMAGE_PIXEL_ZERO_VAL
#ifdef GAN_PIX_FIELD2
                 || pix->GAN_PIX_FIELD2 != GAN_IMAGE_PIXEL_ZERO_VAL
#endif
#ifdef GAN_PIX_FIELD3
                 || pix->GAN_PIX_FIELD3 != GAN_IMAGE_PIXEL_ZERO_VAL
#endif
#ifdef GAN_PIX_FIELD4
                 || pix->GAN_PIX_FIELD4 != GAN_IMAGE_PIXEL_ZERO_VAL
#endif
                    )
            {
               ymin = row;
               if ( ymax == -1 ) ymax = row;
               break;
            }
         }
   }

   if ( xmax == -1 )
      /* no non-zero pixels, so fill fields of window structure with zero */
      subwin->c0 = subwin->r0 = subwin->width = subwin->height = 0;
   else
   {
      /* set window */
      subwin->c0 = xmin; subwin->width  = xmax-xmin+1;
      subwin->r0 = ymin; subwin->height = ymax-ymin+1;
   }
      
   /* success */
   return GAN_TRUE;
}

/**
 * \}
 */

#ifndef NDEBUG

/**
 * \addtogroup ImageAccessPixel
 * \{
 */

/**
 * \brief Return a pointer to a pixel from a GAN_PIXEL_FORMAT \c GAN_PIXEL_TYPE image.
 *
 * Returns the pointer to the pixel at position \a row, \a col in
 * GAN_PIXEL_FORMAT \c GAN_PIXEL_TYPE image \a img.
 */
GAN_PIXEL *
 GAN_IMAGE_GET_PIXPTR ( const Gan_Image *img, unsigned row, unsigned col )
{
   /* consistency check */
   gan_err_test_ptr ( img->format == GAN_PIXFMT && img->type == GAN_PIXTYPE,
                      "GAN_IMAGE_GET_PIXPTR", GAN_ERROR_INCOMPATIBLE, "" );

   if ( row >= img->height || col >= img->width )
   {
      gan_err_flush_trace();
      gan_err_register ( "GAN_IMAGE_GET_PIXPTR", GAN_ERROR_INCOMPATIBLE, "" );
      return NULL;
   }
   
   /* get pixel */
   return &img->row_data.GAN_IMFMT.GAN_IMTYPE[row][col];
}

/**
 * \brief Return the pixel array from a GAN_PIXEL_FORMAT \c GAN_PIXEL_TYPE image.
 *
 * Returns the pixel array for the GAN_PIXEL_FORMAT \c GAN_PIXEL_TYPE image
 * \a img.
 */
GAN_PIXEL **
 GAN_IMAGE_GET_PIXARR ( const Gan_Image *img )
{
   /* consistency check */
   gan_err_test_ptr ( img->format == GAN_PIXFMT && img->type == GAN_PIXTYPE,
                      "GAN_IMAGE_GET_PIXARR", GAN_ERROR_INCOMPATIBLE, "" );

   /* get pixel array */
   return img->row_data.GAN_IMFMT.GAN_IMTYPE;
}

/**
 * \}
 */

#endif /* #ifndef NDEBUG */

/**
 * \addtogroup ImageFill
 * \{
 */

/**
 * \brief Clear image except in specified rectangular region.
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 */
Gan_Bool
 GAN_IMAGE_MASK_WINDOW ( Gan_Image *img,
                         unsigned r0,     unsigned c0,
                         unsigned height, unsigned width )
{
   unsigned r;
   unsigned last_bit = img->width - c0 - width;
   unsigned pixsize = sizeof(GAN_PIXEL)/sizeof(GAN_PIXEL_TYPE);

   /* make sure that region is inside image */
   gan_err_test_bool ( r0+height <= img->height &&
                       c0+width <= img->width,
                       "GAN_IMAGE_MASK_WINDOW", GAN_ERROR_FAILURE, "" );

   for ( r = 0; r < r0; r++ )
      GAN_FILL_ARRAY ( (GAN_PIXEL_TYPE *)img->row_data.GAN_IMFMT.GAN_IMTYPE[r], pixsize*img->width, 1, GAN_IMAGE_PIXEL_ZERO_VAL );

   for ( ; r < r0+height; r++ )
   {
      GAN_FILL_ARRAY ( (GAN_PIXEL_TYPE *)&img->row_data.GAN_IMFMT.GAN_IMTYPE[r][0],        pixsize*c0,       1, GAN_IMAGE_PIXEL_ZERO_VAL );
      GAN_FILL_ARRAY ( (GAN_PIXEL_TYPE *)&img->row_data.GAN_IMFMT.GAN_IMTYPE[r][c0+width], pixsize*last_bit, 1, GAN_IMAGE_PIXEL_ZERO_VAL );
   }

   for ( ; r < img->height; r++ )
      GAN_FILL_ARRAY ( (GAN_PIXEL_TYPE *)img->row_data.GAN_IMFMT.GAN_IMTYPE[r], pixsize*img->width, 1, GAN_IMAGE_PIXEL_ZERO_VAL );

   /* success */
   return GAN_TRUE;
}

/**
 * \brief Clear image in specified rectangular region.
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 */
Gan_Bool
 GAN_IMAGE_CLEAR_WINDOW ( Gan_Image *img,
                          unsigned r0,     unsigned c0,
                          unsigned height, unsigned width )
{
   unsigned r;
   unsigned rowsize;

   /* make sure that region is inside image */
   gan_err_test_bool ( r0+height <= img->height &&
                       c0+width <= img->width,
                       "GAN_IMAGE_CLEAR_WINDOW", GAN_ERROR_FAILURE, "" );

   rowsize = width*sizeof(GAN_PIXEL)/sizeof(GAN_PIXEL_TYPE);
   for ( r = r0; r < r0+height; r++ )
      GAN_FILL_ARRAY ( (GAN_PIXEL_TYPE *)&img->row_data.GAN_IMFMT.GAN_IMTYPE[r][c0], rowsize, 1, GAN_IMAGE_PIXEL_ZERO_VAL );

   /* success */
   return GAN_TRUE;
}

/**
 * \}
 */

#ifdef GAN_IMAGE_GET_MINIMUM_PIXEL

/**
 * \addtogroup ImageMinMax
 * \{
 */

/**
 * \brief Calculates the minimum pixel value in an image
 *
 * Calculates the minimum pixel value in \a pImage over the mask \a pMask,
 * which can be passed as \c NULL to consider all the pixels. The result is
 * returned in \a minval.
 *
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 */
Gan_Bool
 GAN_IMAGE_GET_MINIMUM_PIXEL ( const Gan_Image *pImage, const Gan_Image *pMask, GAN_PIXEL_TYPE *minval )
{
   GAN_PIXEL_TYPE min_pixel = GAN_IMAGE_PIXEL_MAX_VAL, pixval;
   GAN_PIXEL *pix;

   gan_err_test_bool ( pMask == NULL ||
                       (pMask->width == pImage->width && pMask->height == pImage->height),
                       "GAN_IMAGE_GET_MINIMUM_PIXEL", GAN_ERROR_INCOMPATIBLE, "" );

   if(pImage->width == 0 || pImage->height == 0)
   {
      gan_err_flush_trace();
      gan_err_register ( "GAN_IMAGE_GET_MINIMUM_PIXEL", GAN_ERROR_NO_DATA, "" );
      return GAN_FALSE;
   }

   if(pMask == NULL)
   {
      int r, c;

      for(r = (int)pImage->height-1; r >= 0; r--)
         for(c = (int)pImage->width-1, pix = pImage->row_data.GAN_IMFMT.GAN_IMTYPE[r]; c >= 0; c--, pix++)
         {
#ifdef GAN_PIX_FIELD4
            pixval = gan_min4(pix->GAN_PIX_FIELD1, pix->GAN_PIX_FIELD2, pix->GAN_PIX_FIELD3, pix->GAN_PIX_FIELD4);
#elif defined(GAN_PIX_FIELD3)
            pixval = gan_min3(pix->GAN_PIX_FIELD1, pix->GAN_PIX_FIELD2, pix->GAN_PIX_FIELD3);
#elif defined(GAN_PIX_FIELD2)
            pixval = gan_min2(pix->GAN_PIX_FIELD1, pix->GAN_PIX_FIELD2);
#else
            pixval = pix->GAN_PIX_FIELD1;
#endif            
            if(pixval < min_pixel)
               min_pixel = pixval;
         }
   }
   else
   {
      Gan_ImageWindow window;
      unsigned int row_max, col_max, r, c;

      if(!gan_image_get_active_subwindow_b ( pMask, GAN_BIT_ALIGNMENT, &window ))
      {
         gan_err_register ( "GAN_IMAGE_GET_MINIMUM_PIXEL", GAN_ERROR_FAILURE, "" );
         return GAN_FALSE;
      }

      if(window.width == 0 || window.height == 0)
      {
         gan_err_flush_trace();
         gan_err_register ( "GAN_IMAGE_GET_MINIMUM_PIXEL", GAN_ERROR_NO_DATA, "" );
         return GAN_FALSE;
      }

      col_max = window.c0 + window.width;
      row_max = window.r0 + window.height;
      for ( r = window.r0; r < row_max; r++ )
         for ( c = window.c0, pix = &pImage->row_data.GAN_IMFMT.GAN_IMTYPE[r][window.c0]; c < col_max; c++, pix++ )
         {
#ifdef GAN_PIX_FIELD4
            pixval = gan_min4(pix->GAN_PIX_FIELD1, pix->GAN_PIX_FIELD2, pix->GAN_PIX_FIELD3, pix->GAN_PIX_FIELD4);
#elif defined(GAN_PIX_FIELD3)
            pixval = gan_min3(pix->GAN_PIX_FIELD1, pix->GAN_PIX_FIELD2, pix->GAN_PIX_FIELD3);
#elif defined(GAN_PIX_FIELD2)
            pixval = gan_min2(pix->GAN_PIX_FIELD1, pix->GAN_PIX_FIELD2);
#else
            pixval = pix->GAN_PIX_FIELD1;
#endif            
            if(pixval < min_pixel)
               min_pixel = pixval;
         }
   }

   *minval = min_pixel;
   return GAN_TRUE;
}

/**
 * \brief Calculates the maximum pixel value in an image
 *
 * Calculates the maximum pixel value in \a pImage over the mask \a pMask,
 * which can be passed as \c NULL to consider all the pixels. The result is
 * returned in \a maxval.
 *
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 */
Gan_Bool
 GAN_IMAGE_GET_MAXIMUM_PIXEL ( const Gan_Image *pImage, const Gan_Image *pMask, GAN_PIXEL_TYPE *maxval )
{
   GAN_PIXEL_TYPE max_pixel = GAN_IMAGE_PIXEL_MIN_VAL, pixval;
   GAN_PIXEL *pix;

   gan_err_test_bool ( pMask == NULL ||
                       (pMask->width == pImage->width &&
                        pMask->height == pImage->height),
                       "GAN_IMAGE_GET_MAXIMUM_PIXEL", GAN_ERROR_INCOMPATIBLE,
                       "" );

   if(pImage->width == 0 || pImage->height == 0)
   {
      gan_err_flush_trace();
      gan_err_register ( "GAN_IMAGE_GET_MAXIMUM_PIXEL", GAN_ERROR_NO_DATA,
                         "" );
      return GAN_FALSE;
   }

   if(pMask == NULL)
   {
      int r, c;

      for(r = (int)pImage->height-1; r >= 0; r--)
         for(c = (int)pImage->width-1, pix = pImage->row_data.GAN_IMFMT.GAN_IMTYPE[r]; c >= 0; c--, pix++)
         {
#ifdef GAN_PIX_FIELD4
            pixval = gan_max4(pix->GAN_PIX_FIELD1, pix->GAN_PIX_FIELD2, pix->GAN_PIX_FIELD3, pix->GAN_PIX_FIELD4);
#elif defined(GAN_PIX_FIELD3)
            pixval = gan_max3(pix->GAN_PIX_FIELD1, pix->GAN_PIX_FIELD2, pix->GAN_PIX_FIELD3);
#elif defined(GAN_PIX_FIELD2)
            pixval = gan_max2(pix->GAN_PIX_FIELD1, pix->GAN_PIX_FIELD2);
#else
            pixval = pix->GAN_PIX_FIELD1;
#endif            
            if(pixval > max_pixel)
               max_pixel = pixval;
         }
   }
   else
   {
      Gan_ImageWindow window;
      unsigned int row_max, col_max, r, c;

      if(!gan_image_get_active_subwindow_b ( pMask, GAN_BIT_ALIGNMENT,
                                             &window ))
      {
         gan_err_register ( "GAN_IMAGE_GET_MAXIMUM_PIXEL", GAN_ERROR_FAILURE,
                            "" );
         return GAN_FALSE;
      }

      if(window.width == 0 || window.height == 0)
      {
         gan_err_flush_trace();
         gan_err_register ( "GAN_IMAGE_GET_MAXIMUM_PIXEL", GAN_ERROR_NO_DATA,
                            "" );
         return GAN_FALSE;
      }

      col_max = window.c0 + window.width;
      row_max = window.r0 + window.height;
      for ( r = window.r0; r < row_max; r++ )
         for ( c = window.c0, pix = &pImage->row_data.GAN_IMFMT.GAN_IMTYPE[r][window.c0]; c < col_max; c++, pix++ )
         {
#ifdef GAN_PIX_FIELD4
            pixval = gan_max4(pix->GAN_PIX_FIELD1, pix->GAN_PIX_FIELD2, pix->GAN_PIX_FIELD3, pix->GAN_PIX_FIELD4);
#elif defined(GAN_PIX_FIELD3)
            pixval = gan_max3(pix->GAN_PIX_FIELD1, pix->GAN_PIX_FIELD2, pix->GAN_PIX_FIELD3);
#elif defined(GAN_PIX_FIELD2)
            pixval = gan_max2(pix->GAN_PIX_FIELD1, pix->GAN_PIX_FIELD2);
#else
            pixval = pix->GAN_PIX_FIELD1;
#endif            
            if(pixval > max_pixel)
               max_pixel = pixval;
         }
   }

   *maxval = max_pixel;
   return GAN_TRUE;
}

/**
 * \}
 */

#endif /* #ifdef GAN_IMAGE_GET_MINIMUM_PIXEL */

/* functions written in templated manner common to all image formats and
   types */
#include <gandalf/image/image_common_noc.c>

/**
 * \}
 */

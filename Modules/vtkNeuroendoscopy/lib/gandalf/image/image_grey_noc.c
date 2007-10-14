/**
 * File:          $RCSfile: image_grey_noc.c,v $
 * Module:        Grey level image functions
 * Part of:       Gandalf Library 
 *
 * Revision:      $Revision: 1.1.2.1 $
 * Last edited:   $Date: 2007/10/14 02:20:22 $
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

/**
 * \addtogroup ImagePackage
 * \{
 */

/* this file contains image package functions common to all grey-level image
 * types, as controlled by preprocessor symbols. The meanings of GAN_PIXEL,
 * GAN_PIXFMT, GAN_PIXTYPE, GAN_IMFMT and GAN_IMTYPE are explained in
 * image_common_noc.c.
 */

/* define symbols specific to grey-level images */
#define GAN_PIXFMT GAN_GREY_LEVEL_IMAGE
#define GAN_IMFMT gl

/* forward declaration of function defined in image_common_noc.c */
static Gan_Bool
 image_realloc ( Gan_Image *img,
                 unsigned long height, unsigned long width,
                 unsigned long stride, Gan_Bool alloc_pix_data );

#ifdef GAN_BITMAP

#ifndef NDEBUG

/* set pixel value */
static Gan_Bool
 image_set_pix ( Gan_Image *img, unsigned row, unsigned col, Gan_Bool pix )
{
   /* consistency check */
   gan_err_test_bool ( row < img->height && col < img->width,
                       "image_set_pix", GAN_ERROR_ILLEGAL_ARGUMENT,
                       "pixel position" );

   /* set pixel */
   gan_bit_array_twiddle_bit ( &img->ba[row], col, pix );

   /* success */
   return GAN_TRUE;
}

static Gan_Bool
 image_get_pix ( const Gan_Image *img, unsigned row, unsigned col )
{
   /* consistency check */
   assert ( img->format == GAN_PIXFMT && img->type == GAN_PIXTYPE );

   gan_assert ( row < img->height && col < img->width,
                "illegal pixel position in image_get_pix()" );

   /* get pixel */
   return (gan_bit_array_get_bit ( &img->ba[row], col )
           ? GAN_TRUE : GAN_FALSE);
}

#endif /* #ifndef NDEBUG */

static Gan_Bool
 image_fill_const ( Gan_Image *img, Gan_Bool pix )
{
   int r;

   /* consistency check */
   gan_err_test_bool ( img->format == GAN_PIXFMT && img->type == GAN_PIXTYPE,
                       "image_fill_const", GAN_ERROR_INCOMPATIBLE,
                       "image format/type" );
      
   /* fill image one row at a time */
   for ( r = img->height-1; r >= 0; r-- )
      gan_bit_array_fill ( &img->ba[r], pix );

   /* success */
   return GAN_TRUE;
}

#else

#ifndef NDEBUG

/* set pixel value */
static Gan_Bool
 image_set_pix ( Gan_Image *img, unsigned row, unsigned col, GAN_PIXEL pix )
{
   /* consistency check */
   gan_err_test_bool ( row < img->height && col < img->width,
                       "image_set_pix", GAN_ERROR_ILLEGAL_ARGUMENT,
                       "pixel position" );

   /* set pixel */
   img->row_data.gl.GAN_IMTYPE[row][col] = pix;

   /* success */
   return GAN_TRUE;
}

static GAN_PIXEL
 image_get_pix ( const Gan_Image *img, unsigned row, unsigned col )

{
   /* consistency check */
   assert ( img->format == GAN_PIXFMT && img->type == GAN_PIXTYPE );

   gan_assert ( row < img->height && col < img->width,
                "illegal pixel position in image_get_pix()" );

   /* get pixel */
   return img->row_data.gl.GAN_IMTYPE[row][col];
}

#endif /* #ifndef NDEBUG */

/* fill image with constant pixel value */
static Gan_Bool
 image_fill_const ( Gan_Image *img, GAN_PIXEL pix )
{
   /* consistency check */
   gan_err_test_bool ( img->format == GAN_PIXFMT && img->type == GAN_PIXTYPE,
                       "image_fill_const_?", GAN_ERROR_INCOMPATIBLE,
                       "image format/type" );

   /* set all pixels */
   if ( img->height == 0 ) return GAN_TRUE;
   if ( img->stride == img->width*sizeof(GAN_PIXEL) )
      /* fill all pixels in one go */
      GAN_FILL_ARRAY ( img->row_data.gl.GAN_IMTYPE[0], img->height*img->width, 1, pix );
   else
   {
      /* fill image one row at a time */
      int r;

      for ( r = img->height-1; r >= 0; r-- )
         GAN_FILL_ARRAY ( img->row_data.gl.GAN_IMTYPE[r], img->width, 1, pix );
   }

   /* success */
   return GAN_TRUE;
}

#endif /* #ifdef GAN_BITMAP */

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
                                     gan_image_min_stride(GAN_PIXFMT, GAN_PIXTYPE,
                                                          img_s->width, 0),
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

#ifdef GAN_BITMAP
   {
      int r;

      /* copy image one row at a time */
      for ( r = (int)img_s->height-1; r >= 0; r-- )
         gan_bit_array_copy_q ( &img_s->ba[r], &img_d->ba[r] );
   }
#else

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
#endif

   img_d->offset_x = img_s->offset_x;
   img_d->offset_y = img_s->offset_y;
   return img_d;
}

/* functions written in templated manner common to all image formats and
   types */
#include <gandalf/image/image_common_noc.c>

#ifdef GAN_BITMAP
/**
 * \addtogroup ImageExtract
 * \{
 */

/**
 * \brief Computes bounding box of non-zero pixels in binary image.
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Computes image window which covers all bits in the binary \a image which
 * have the value one (true). The value of the \c c0 and \c width fields of
 * \a subwin will be provided with precision indicated by the given
 * \a alignment, one of word, byte or bit alignment, the last being the
 * slowest to compute. If \a image is zero everywhere the fields of the
 * \a subwin structure will be set to zero.
 */
Gan_Bool
 GAN_IMAGE_GET_ACTIVE_SUBWINDOW ( const Gan_Image *image,
                                  Gan_Alignment alignment,
                                  Gan_ImageWindow *subwin )
{
   unsigned int first_row, last_row, first_col, last_col;
   unsigned int tmp_first_col=0, tmp_last_col=0, words_per_row;
   int ctr_row, ctr_col;
   Gan_Bool last_col_set, last_row_set, non_zero_row;
   Gan_BitWord *w;

   /* byte alignment stuff */
   int row_limit, first_byte, last_byte;
   unsigned char *ucptr;

   /* bit alignment stuff */
   int bit;

   if ( image == NULL )
   {
      gan_err_flush_trace();
      gan_err_register ( "GAN_IMAGE_GET_ACTIVE_SUBWINDOW",
                         GAN_ERROR_ILLEGAL_ARGUMENT, "" );
      return GAN_FALSE;
   }

   if ( image->format != GAN_GREY_LEVEL_IMAGE || image->type != GAN_BOOL )
   {
      gan_err_flush_trace();
      gan_err_register ( "GAN_IMAGE_GET_ACTIVE_SUBWINDOW",
                         GAN_ERROR_ILLEGAL_ARGUMENT, "" );
      return GAN_FALSE;
   }

   if ( image->width == 0 || image->height == 0 )
   {
      /* no set pixels found: set window to zero size */
      subwin->c0    = subwin->r0 =
      subwin->width = subwin->height = 0;
      return GAN_TRUE;
   }
      
   /* Get bitarray length */
   words_per_row = GAN_NO_BITWORDS(image->width);

   /* Initialise Variables */
   first_col = words_per_row;
   last_col = 0;

   first_row = image->height; 
   last_row = 0;
   last_row_set = GAN_FALSE;

   /* Get subimage coordinates */
   for(ctr_row = (int)image->height-1; ctr_row >= 0; ctr_row-- )
   {
      last_col_set = GAN_FALSE;
      non_zero_row = GAN_FALSE;

      w = image->row_data.gl.b[ctr_row];

      for(ctr_col = (int)words_per_row - 1; ctr_col >= 0; ctr_col--)
      {
         if(w[ctr_col] != 0)
         {
            tmp_first_col = (unsigned int) ctr_col;

            if(!last_col_set)
            {
               non_zero_row = GAN_TRUE;
               last_col_set = GAN_TRUE;
               tmp_last_col = (unsigned int) ctr_col;
            }
         }
      }

      if(non_zero_row)
      {
         /* Set the first/last column pointers */
                
        if(tmp_last_col > last_col)
           last_col = tmp_last_col;

        if(tmp_first_col < first_col)
           first_col = tmp_first_col;

        /* Set the first/last row pointers */
        first_row = ctr_row;

        if(!last_row_set)
        {
           last_row_set = GAN_TRUE;
           last_row = (unsigned int) ctr_row;
        }
      }
   }

   if ( first_col > last_col || first_row > last_row )
   {
      /* no set pixels found: set window to zero size */
      subwin->c0    = subwin->r0 =
      subwin->width = subwin->height = 0;
      return GAN_TRUE;
   }

   /* fill in subwindow with result */
   subwin->c0 = (int)first_col*GAN_BITWORD_SIZE;
   subwin->r0 = (int)first_row;
   subwin->width = (int)(last_col - first_col + 1)*GAN_BITWORD_SIZE;
   subwin->height = (int)(last_row - first_row)+1;

   /* if word alignment was requested, return immediately */
   if ( alignment == GAN_WORD_ALIGNMENT )
   {
      /* adjust subwindow to make sure it doesn't go outside image */
      if ( subwin->c0 + subwin->width > (int)image->width )
         subwin->width = (int)image->width - subwin->c0;

      return GAN_TRUE;
   }

   /* compute byte alignment */
   row_limit = (int)(subwin->r0+subwin->height);

   /* search for first non-zero byte */
   first_byte = INT_MAX;
   for ( ctr_row = subwin->r0; ctr_row < row_limit; ctr_row++ )
   {
      if ( image->row_data.gl.b[ctr_row][first_col] != 0 )
      {
         ucptr = (unsigned char *) &image->row_data.gl.b[ctr_row][first_col];

#if (GAN_BITWORD_SIZE == 32)
         if ( ucptr[0] != 0 ) { first_byte = 0; break; }
         if ( ucptr[1] != 0 ) first_byte = 1;
         else if (ucptr[2] != 0) first_byte = (first_byte>2) ? 2 : first_byte;
         else if (ucptr[3] != 0) first_byte = (first_byte>3) ? 3 : first_byte;
#elif (GAN_BITWORD_SIZE == 64)
         if ( ucptr[0] != 0 ) { first_byte = 0; break; }
         if ( ucptr[1] != 0 ) first_byte = 1;
         else if (ucptr[2] != 0) first_byte = (first_byte>2) ? 2 : first_byte;
         else if (ucptr[3] != 0) first_byte = (first_byte>3) ? 3 : first_byte;
         else if (ucptr[4] != 0) first_byte = (first_byte>4) ? 4 : first_byte;
         else if (ucptr[5] != 0) first_byte = (first_byte>5) ? 5 : first_byte;
         else if (ucptr[6] != 0) first_byte = (first_byte>6) ? 6 : first_byte;
         else if (ucptr[7] != 0) first_byte = (first_byte>7) ? 7 : first_byte;
#else /* (GAN_BITWORD_SIZE > 64) */
#error "Can't handle larger bit-word size"            
#endif /* #if (GAN_BITWORD_SIZE == 32) */
      }
   }

   /* make sure we found a non-zero value */
   gan_err_test_bool ( first_byte != INT_MAX,
                       "GAN_IMAGE_GET_ACTIVE_SUBWINDOW", GAN_ERROR_FAILURE,
                       "" );

   /* adjust start of row */
   subwin->c0 += first_byte*8;
   subwin->width -= first_byte*8;

   /* now check end of row */
   last_byte = INT_MIN;
   for ( ctr_row = subwin->r0; ctr_row < row_limit; ctr_row++ )
   {
      if ( image->row_data.gl.b[ctr_row][last_col] != 0 )
      {
         ucptr = (unsigned char *) &image->row_data.gl.b[ctr_row][last_col];

#if (GAN_BITWORD_SIZE == 32)
         if ( ucptr[3] != 0 ) { last_byte = 3; break; }
         if ( ucptr[2] != 0 ) last_byte = 2;
         else if ( ucptr[1] != 0 ) last_byte = (last_byte < 1) ? 1 : last_byte;
         else if ( ucptr[0] != 0 ) last_byte = (last_byte < 0) ? 0 : last_byte;
#elif (GAN_BITWORD_SIZE == 64)
         if ( ucptr[7] != 0 ) { last_byte = 7; break; }
         if ( ucptr[6] != 0 ) last_byte = 6;
         else if ( ucptr[5] != 0 ) last_byte = (last_byte < 5) ? 5 : last_byte;
         else if ( ucptr[4] != 0 ) last_byte = (last_byte < 4) ? 4 : last_byte;
         else if ( ucptr[3] != 0 ) last_byte = (last_byte < 3) ? 3 : last_byte;
         else if ( ucptr[2] != 0 ) last_byte = (last_byte < 2) ? 2 : last_byte;
         else if ( ucptr[1] != 0 ) last_byte = (last_byte < 1) ? 1 : last_byte;
         else if ( ucptr[0] != 0 ) last_byte = (last_byte < 0) ? 0 : last_byte;
#else /* (GAN_BITWORD_SIZE > 64) */
#error "Can't handle larger bit-word size"            
#endif /* #if (GAN_BITWORD_SIZE == 32) */
      }
   }

   /* make sure we found a non-zero value */
   gan_err_test_bool ( last_byte != INT_MIN, "GAN_IMAGE_GET_ACTIVE_SUBWINDOW",
                       GAN_ERROR_FAILURE, "" );

   /* adjust end of row */
   subwin->width -= (GAN_BITWORD_SIZE - (last_byte+1)*8);
   
   /* if byte alignment was requested, return immediately */
   if ( alignment == GAN_BYTE_ALIGNMENT )
   {
      /* adjust subwindow to make sure it doesn't go outside image */
      if ( subwin->c0 + subwin->width > (int)image->width )
         subwin->width = (int)image->width - subwin->c0;

      return GAN_TRUE;
   }

   /* bit-alignment must have been requested */
   gan_err_test_bool ( alignment == GAN_BIT_ALIGNMENT,
                       "GAN_IMAGE_GET_ACTIVE_SUBWINDOW",
                       GAN_ERROR_FAILURE, "" );

   /* compute bit alignment for start of row */
   bit = INT_MAX;
   for ( ctr_row = subwin->r0; ctr_row < row_limit; ctr_row++ )
   {
      int byte;
      unsigned char ucval;

#if (SIZEOF_CHAR == 1)
      if ( image->row_data.gl.b[ctr_row][first_col] != 0 )
      {
         /* check that this row is aligned to the first limiting byte */
         ucptr = (unsigned char *) &image->row_data.gl.b[ctr_row][first_col];
         for ( byte = 0; byte < GAN_BITWORD_SIZE/8; byte++ )
            if ( ucptr[byte] != 0 ) break;

         if ( byte != first_byte ) continue;

         ucval = ucptr[first_byte];

         /* now count individual bits */
#ifdef WORDS_BIGENDIAN
         if ( ucval & 0x80 ) {bit = 0; break;}
         if ( ucval & 0x40 ) bit = 1;
         else if ( ucval & 0x20 ) bit = (bit > 2) ? 2 : bit;
         else if ( ucval & 0x10 ) bit = (bit > 3) ? 3 : bit;
         else if ( ucval &  0x8 ) bit = (bit > 4) ? 4 : bit;
         else if ( ucval &  0x4 ) bit = (bit > 5) ? 5 : bit;
         else if ( ucval &  0x2 ) bit = (bit > 6) ? 6 : bit;
         else if ( ucval &  0x1 ) bit = (bit > 7) ? 7 : bit;
#else /* #ifndef WORDS_BIGENDIAN */
         if ( ucval & 0x1 ) {bit = 0; break;}
         if ( ucval & 0x2 ) bit = 1;
         else if ( ucval &  0x4 ) bit = (bit > 2) ? 2 : bit;
         else if ( ucval &  0x8 ) bit = (bit > 3) ? 3 : bit;
         else if ( ucval & 0x10 ) bit = (bit > 4) ? 4 : bit;
         else if ( ucval & 0x20 ) bit = (bit > 5) ? 5 : bit;
         else if ( ucval & 0x40 ) bit = (bit > 6) ? 6 : bit;
         else if ( ucval & 0x80 ) bit = (bit > 7) ? 7 : bit;
#endif /* #ifdef WORDS_BIGENDIAN */
      }
#else
#error "Only 8-bit characters supported currently"
#endif /* #if (SIZEOF_CHAR == 1) */
   }

   /* make sure we found a valid bit */
   gan_err_test_bool ( bit != INT_MAX, "GAN_IMAGE_GET_ACTIVE_SUBWINDOW",
                       GAN_ERROR_FAILURE, "" );

   /* adjust start of row */
   subwin->c0 += bit;
   subwin->width -= bit;
   
   /* compute bit alignment for end of row */
   bit = INT_MIN;
   for ( ctr_row = subwin->r0; ctr_row < row_limit; ctr_row++ )
   {
      int byte;
      unsigned char ucval;

#if (SIZEOF_CHAR == 1)
      if ( image->row_data.gl.b[ctr_row][last_col] != 0 )
      {
         /* check that this row is aligned to the first limiting byte */
         ucptr = (unsigned char *) &image->row_data.gl.b[ctr_row][last_col];
         for ( byte = GAN_BITWORD_SIZE/8-1; byte >= 0; byte-- )
            if ( ucptr[byte] != 0 ) break;

         if ( byte != last_byte ) continue;

         ucval = ucptr[last_byte];

         /* now count individual bits */
#ifdef WORDS_BIGENDIAN
         if ( ucval & 0x1 ) {bit = 7; break;}
         else if ( ucval &  0x2 ) bit = 6;
         else if ( ucval &  0x4 ) bit = (bit < 5) ? 5 : bit;
         else if ( ucval &  0x8 ) bit = (bit < 4) ? 4 : bit;
         else if ( ucval & 0x10 ) bit = (bit < 3) ? 3 : bit;
         else if ( ucval & 0x20 ) bit = (bit < 2) ? 2 : bit;
         else if ( ucval & 0x40 ) bit = (bit < 1) ? 1 : bit;
         else if ( ucval & 0x80 ) bit = (bit < 0) ? 0 : bit;
#else /* #ifndef WORDS_BIGENDIAN */
         if ( ucval & 0x80 ) {bit = 7; break;}
         else if ( ucval & 0x40 ) bit = 6;
         else if ( ucval & 0x20 ) bit = (bit < 5) ? 5 : bit;
         else if ( ucval & 0x10 ) bit = (bit < 4) ? 4 : bit;
         else if ( ucval &  0x8 ) bit = (bit < 3) ? 3 : bit;
         else if ( ucval &  0x4 ) bit = (bit < 2) ? 2 : bit;
         else if ( ucval &  0x2 ) bit = (bit < 1) ? 1 : bit;
         else if ( ucval &  0x1 ) bit = (bit < 0) ? 0 : bit;
#endif /* #ifdef WORDS_BIGENDIAN */
      }
#else
#error "Only 8-bit characters supported currently"
#endif /* #if (SIZEOF_CHAR == 1) */
   }

   /* make sure we found a valid bit */
   gan_err_test_bool ( bit != INT_MIN, "GAN_IMAGE_GET_ACTIVE_SUBWINDOW",
                       GAN_ERROR_FAILURE, "" );

   /* adjust end of row */
   subwin->width -= (8-1-bit);
   
   /* adjust subwindow to make sure it doesn't go outside image */
   if ( subwin->c0 + subwin->width > (int)image->width )
      subwin->width = (int)image->width - subwin->c0;

   /* success */
   return GAN_TRUE;
}

/**
 * \}
 */

#else /* #ifndef GAN_BITMAP */

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
         if ( image->row_data.gl.GAN_IMTYPE[row][col] != GAN_IMAGE_PIXEL_ZERO_VAL )
         {
            /* update bounds if necessary */
            ymin = row;
            if ( ymax == -1 ) ymax = row;
            xmin = col;
            break;
         }

      for ( col = (int)image->width-1; col > xmax; col-- )
         if ( image->row_data.gl.GAN_IMTYPE[row][col] != GAN_IMAGE_PIXEL_ZERO_VAL )
         {
            /* update bounds if necessary */
            ymin = row;
            if ( ymax == -1 ) ymax = row;
            xmax = col;
            break;
         }

      /* if we haven't found a non-zero pixel outside previous horizontal
         range, we need to check the other pixels to update the vertical
         range */
      if ( ymin != row )
         for ( col = xmin; col <= xmax; col++ )
            if ( image->row_data.gl.GAN_IMTYPE[row][col] != GAN_IMAGE_PIXEL_ZERO_VAL )
            {
               ymin = row;
               if ( ymax == -1 ) ymax = row;
               break;
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
GAN_PIXEL *GAN_IMAGE_GET_PIXPTR ( const Gan_Image *img,
                                  unsigned row, unsigned col )
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
   return &img->row_data.gl.GAN_IMTYPE[row][col];
}

/**
 * \brief Return the pixel array from a GAN_PIXEL_FORMAT \c GAN_PIXEL_TYPE image.
 *
 * Returns the pixel array from the GAN_PIXEL_FORMAT \c GAN_PIXEL_TYPE image
 * \a img.
 */
GAN_PIXEL **GAN_IMAGE_GET_PIXARR ( const Gan_Image *img )
{
   /* consistency check */
   gan_err_test_ptr ( img->format == GAN_PIXFMT && img->type == GAN_PIXTYPE,
                      "GAN_IMAGE_GET_PIXARR", GAN_ERROR_INCOMPATIBLE, "" );

   /* get pixel */
   return img->row_data.gl.GAN_IMTYPE;
}

/**
 * \}
 */

#endif /* #ifndef NDEBUG */

#endif /* #ifdef GAN_BITMAP */

/**
 * \addtogroup ImageFill
 * \{
 */

#ifdef GAN_BITMAP

/**
 * \brief Clear binary image except in specified rectangular region.
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 */
Gan_Bool
 GAN_IMAGE_MASK_WINDOW ( Gan_Image *img,
                         unsigned r0,     unsigned c0,
                         unsigned height, unsigned width )
{
   unsigned r;

   /* make sure that region is inside image */
   gan_err_test_bool ( r0+height <= img->height &&
                       c0+width <= img->width,
                       "GAN_IMAGE_MASK_WINDOW", GAN_ERROR_FAILURE, "" );

   for ( r = 0; r < r0; r++ )
      if ( !gan_image_bit_fill_row ( img, r, 0, img->width, GAN_FALSE ) )
      {
         gan_err_register ( "GAN_IMAGE_MASK_WINDOW", GAN_ERROR_FAILURE, "" );
         return GAN_FALSE;
      }

   for ( ; r < r0+height; r++ )
      if ( !gan_image_bit_fill_row ( img, r, 0, c0, GAN_FALSE ) ||
           !gan_image_bit_fill_row ( img, r, c0+width,
                                     img->width-c0-width, GAN_FALSE ) )
      {
         gan_err_register ( "GAN_IMAGE_MASK_WINDOW", GAN_ERROR_FAILURE, "" );
         return GAN_FALSE;
      }
      
   for ( ; r < img->height; r++ )
      if ( !gan_image_bit_fill_row ( img, r, 0, img->width, GAN_FALSE ) )
      {
         gan_err_register ( "GAN_IMAGE_MASK_WINDOW", GAN_ERROR_FAILURE, "" );
         return GAN_FALSE;
      }

   /* success */
   return GAN_TRUE;
}

/**
 * \brief Clear binary image in specified rectangular region.
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 */
Gan_Bool
 GAN_IMAGE_CLEAR_WINDOW ( Gan_Image *img,
                          unsigned r0,     unsigned c0,
                          unsigned height, unsigned width )
{
   unsigned r;

   /* make sure that region is inside image */
   gan_err_test_bool ( r0+height <= img->height &&
                       c0+width <= img->width,
                       "GAN_IMAGE_CLEAR_WINDOW", GAN_ERROR_FAILURE, "" );

   for ( r = r0; r < r0+height; r++ )
      if ( !gan_image_bit_fill_row ( img, r, c0, width, GAN_FALSE ) )
      {
         gan_err_register ( "GAN_IMAGE_CLEAR_WINDOW", GAN_ERROR_FAILURE, "" );
         return GAN_FALSE;
      }

   /* success */
   return GAN_TRUE;
}

#else

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

   /* make sure that region is inside image */
   gan_err_test_bool ( r0+height <= img->height &&
                       c0+width <= img->width,
                       "GAN_IMAGE_MASK_WINDOW", GAN_ERROR_FAILURE, "" );

   for ( r = 0; r < r0; r++ )
      GAN_FILL_ARRAY ( img->row_data.gl.GAN_IMTYPE[r], img->width, 1, GAN_IMAGE_PIXEL_ZERO_VAL );

   for ( ; r < r0+height; r++ )
   {
      GAN_FILL_ARRAY ( &img->row_data.gl.GAN_IMTYPE[r][0],        c0,       1, GAN_IMAGE_PIXEL_ZERO_VAL );
      GAN_FILL_ARRAY ( &img->row_data.gl.GAN_IMTYPE[r][c0+width], last_bit, 1, GAN_IMAGE_PIXEL_ZERO_VAL );
   }

   for ( ; r < img->height; r++ )
      GAN_FILL_ARRAY ( img->row_data.gl.GAN_IMTYPE[r], img->width, 1, GAN_IMAGE_PIXEL_ZERO_VAL );

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

   /* make sure that region is inside image */
   gan_err_test_bool ( r0+height <= img->height &&
                       c0+width <= img->width,
                       "GAN_IMAGE_CLEAR_WINDOW", GAN_ERROR_FAILURE, "" );

   for ( r = r0; r < r0+height; r++ )
      GAN_FILL_ARRAY ( &img->row_data.gl.GAN_IMTYPE[r][c0], width, 1, GAN_IMAGE_PIXEL_ZERO_VAL );

   /* success */
   return GAN_TRUE;
}

#endif /* #ifdef GAN_BITMAP */

/**
 * \}
 */

#ifdef GAN_IMAGE_GET_MINIMUM_PIXEL
#ifndef GAN_BITMAP

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
 GAN_IMAGE_GET_MINIMUM_PIXEL ( const Gan_Image *pImage, const Gan_Image *pMask, GAN_PIXEL *minval )
{
   GAN_PIXEL min_pixel = GAN_IMAGE_PIXEL_MAX_VAL;

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
         for(c = (int)pImage->width-1; c >= 0; c--)
            if(GAN_IMAGE_GET_PIX(pImage, r, c) < min_pixel)
               min_pixel = GAN_IMAGE_GET_PIX(pImage, r, c);
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
         for ( c = window.c0; c < col_max; c++ )
            if(GAN_IMAGE_GET_PIX(pImage, r, c) < min_pixel)
               min_pixel = GAN_IMAGE_GET_PIX(pImage, r, c);
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
 GAN_IMAGE_GET_MAXIMUM_PIXEL ( const Gan_Image *pImage, const Gan_Image *pMask, GAN_PIXEL *maxval )
{
   GAN_PIXEL max_pixel = GAN_IMAGE_PIXEL_MIN_VAL;

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
         for(c = (int)pImage->width-1; c >= 0; c--)
            if(GAN_IMAGE_GET_PIX(pImage, r, c) > max_pixel)
               max_pixel = GAN_IMAGE_GET_PIX(pImage, r, c);
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
         for ( c = window.c0; c < col_max; c++ )
            if(gan_image_get_pix_b(pMask, r, c) &&
               GAN_IMAGE_GET_PIX(pImage, r, c) > max_pixel)
               max_pixel = GAN_IMAGE_GET_PIX(pImage, r, c);
   }

   *maxval = max_pixel;
   return GAN_TRUE;
}

/**
 * \}
 */

#endif /* #ifndef GAN_BITMAP */
#endif /* #ifdef GAN_IMAGE_GET_MINIMUM_PIXEL */

/**
 * \}
 */

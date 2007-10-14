/**
 * File:          $RCSfile: image_bit.c,v $
 * Module:        Binary images
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

#include <gandalf/image/image_bit.h>

/**
 * \defgroup ImagePackage Image Package
 * \{
 */

/* start other groups */

/**
 * \defgroup ImageAllocate Allocate/Free an Image
 * \{
 */

/**
 * \}
 */

/**
 * \defgroup ImageSet Set Attributes of an Image
 * \{
 */

/**
 * \}
 */

/**
 * \defgroup ImageTest Test Attributes of an Image
 * \{
 */

/**
 * \}
 */

/**
 * \defgroup ImageExtract Extract a Part of an Image
 * \{
 */

/**
 * \}
 */

/**
 * \defgroup ImageAccessPixel Access Individual Pixels of an Image
 * \{
 */

/**
 * \}
 */

/**
 * \defgroup ImageSize Return Size Attributes of a Pixel or an Image
 * \{
 */

/**
 * \}
 */

/**
 * \defgroup ImageFill Fill an Image
 * \{
 */

/**
 * \}
 */

/**
 * \defgroup ImageMinMax Minumum/maximum pixel value across an image
 * \{
 */

/**
 * \}
 */

/**
 * \}
 */

#define GAN_PIXEL Gan_BitWord
#define GAN_PIXEL_FORMAT grey-level
#define GAN_PIXEL_TYPE Gan_Bool
#define GAN_PIXTYPE GAN_BOOL
#define GAN_IMTYPE b
#define GAN_IMAGE_FORM_GEN gan_image_form_gen_b
#define GAN_IMAGE_SET_GEN gan_image_set_gen_b
#define GAN_IMAGE_ALLOC gan_image_alloc_b
#define GAN_IMAGE_ALLOC_DATA gan_image_alloc_data_b
#define GAN_IMAGE_FORM gan_image_form_b
#define GAN_IMAGE_FORM_DATA gan_image_form_data_b
#define GAN_IMAGE_SET gan_image_set_b
#define GAN_IMAGE_SET_PIX gan_image_set_pix_b
#define GAN_IMAGE_GET_PIX gan_image_get_pix_b
#define GAN_IMAGE_FILL_CONST gan_image_fill_const_b
#define GAN_IMAGE_GET_ACTIVE_SUBWINDOW gan_image_get_active_subwindow_b
#define GAN_IMAGE_MASK_WINDOW gan_image_mask_window_b
#define GAN_IMAGE_CLEAR_WINDOW gan_image_clear_window_b
#define GAN_IMAGE_PIXEL_ZERO_VAL GAN_FALSE
#define GAN_FILL_ARRAY gan_fill_array_b
#define GAN_COPY_ARRAY gan_copy_array_b

/* to generate bitmap-specific code */
#define GAN_BITMAP

#include <gandalf/image/image_grey_noc.c>

#undef GAN_BITMAP
#undef GAN_PIXEL_FORMAT
#undef GAN_PIXEL_TYPE
#undef GAN_PIXEL
#undef GAN_PIXTYPE
#undef GAN_IMTYPE
#undef GAN_IMAGE_FORM_GEN
#undef GAN_IMAGE_SET_GEN
#undef GAN_IMAGE_ALLOC
#undef GAN_IMAGE_ALLOC_DATA
#undef GAN_IMAGE_FORM
#undef GAN_IMAGE_FORM_DATA
#undef GAN_IMAGE_SET
#undef GAN_IMAGE_SET_PIX
#undef GAN_IMAGE_GET_PIX
#undef GAN_IMAGE_FILL_CONST
#undef GAN_IMAGE_GET_ACTIVE_SUBWINDOW
#undef GAN_IMAGE_MASK_WINDOW
#undef GAN_IMAGE_CLEAR_WINDOW
#undef GAN_IMAGE_PIXEL_ZERO_VAL
#undef GAN_FILL_ARRAY
#undef GAN_COPY_ARRAY

#include <gandalf/common/misc_error.h>

/**
 * \addtogroup ImagePackage
 * \{
 */

/**
 * \defgroup ImageBinary Binary Image Operations
 * \{
 */

/* if NDEBUG is defined, the functions below are implemented as macros */
#ifndef NDEBUG

/**
 * \brief Test local group of four binary pixels.
 *
 * Return #GAN_TRUE if bits at positions \a (row,col), \a (row,col+1),
 * \a (row+1,col) and \a (row+1,col+1) are all set to one (true),
 * or #GAN_FALSE otherwise.
 */
Gan_Bool
 gan_image_bit_get_pix_4group ( const Gan_Image *image,
                                unsigned row, unsigned col )
{
   /* consistency check */
   assert ( image->format == GAN_GREY_LEVEL_IMAGE && image->type == GAN_BOOL );

   gan_assert ( row < image->height-1 && col < image->width-1,
                "illegal pixel position in gan_image_bit_get_pix_4group()" );

   /* get pixel */
   return ((gan_bit_array_get_bit ( &image->ba[row],   col   ) &&
            gan_bit_array_get_bit ( &image->ba[row],   col+1 ) &&
            gan_bit_array_get_bit ( &image->ba[row+1], col   ) &&
            gan_bit_array_get_bit ( &image->ba[row+1], col+1 ))
           ? GAN_TRUE : GAN_FALSE);
}

/**
 * \brief Test local group of five binary pixels.
 *
 * Return #GAN_TRUE if bits at positions \a (row,col), \a (row,col-1),
 * \a (row,col+1), \a (row-1,col) and \a (row+1,col) are all set to one
 * (true), or #GAN_FALSE otherwise.
 */
Gan_Bool
 gan_image_bit_get_pix_5group ( const Gan_Image *image,
                                unsigned row, unsigned col )
{
   /* consistency check */
   assert ( image->format == GAN_GREY_LEVEL_IMAGE && image->type == GAN_BOOL );

   gan_assert ( row > 0 && row < image->height-1 &&
                col > 0 && col < image->width-1,
                "illegal pixel position in gan_image_bit_get_pix_5group()" );

   /* get pixel */
   return ((gan_bit_array_get_bit ( &image->ba[row-1], col   ) &&
            gan_bit_array_get_bit ( &image->ba[row],   col-1 ) &&
            gan_bit_array_get_bit ( &image->ba[row],   col   ) &&
            gan_bit_array_get_bit ( &image->ba[row],   col+1 ) &&
            gan_bit_array_get_bit ( &image->ba[row+1], col   ))
           ? GAN_TRUE : GAN_FALSE);
}

/**
 * \brief Test local group of nine binary pixels.
 *
 * Return #GAN_TRUE if bits at positions \a (row-1,col-1), \a (row,col),
 * \a (row+1,col-1), \a (row,col-1), \a (row,col+1), \a (row-1,col),
 * \a (row+1,col-1), \a (row+1,col) and \a (row+1,col+1) are all set
 * to one (true), or #GAN_FALSE otherwise.
 */
Gan_Bool
 gan_image_bit_get_pix_9group ( const Gan_Image *image,
                                unsigned row, unsigned col )
{
   /* consistency check */
   assert ( image->format == GAN_GREY_LEVEL_IMAGE && image->type == GAN_BOOL );

   gan_assert ( row > 0 && row < image->height-1 &&
                col > 0 && col < image->width-1,
                "illegal pixel position in gan_image_bit_get_pix_9group()" );

   /* get pixel */
   return ((gan_bit_array_get_bit ( &image->ba[row-1], col-1 ) &&
            gan_bit_array_get_bit ( &image->ba[row-1], col   ) &&
            gan_bit_array_get_bit ( &image->ba[row-1], col+1 ) &&
            gan_bit_array_get_bit ( &image->ba[row],   col-1 ) &&
            gan_bit_array_get_bit ( &image->ba[row],   col   ) &&
            gan_bit_array_get_bit ( &image->ba[row],   col+1 ) &&
            gan_bit_array_get_bit ( &image->ba[row+1], col-1 ) &&
            gan_bit_array_get_bit ( &image->ba[row+1], col   ) &&
            gan_bit_array_get_bit ( &image->ba[row+1], col+1 ))
           ? GAN_TRUE : GAN_FALSE);
}

/**
 * \brief Test local group of three binary pixels in a row.
 *
 * Return #GAN_TRUE if bits at positions \a (row,col), \a (row,col-1) and
 * \a (row,col+1) are all set to one (true), or #GAN_FALSE otherwise.
 */
Gan_Bool
 gan_image_bit_get_pix_3group_horiz ( const Gan_Image *image,
                                      unsigned row, unsigned col )
{
   /* consistency check */
   assert ( image->format == GAN_GREY_LEVEL_IMAGE && image->type == GAN_BOOL );

   gan_assert ( row < image->height &&
                image->width > 2 && col > 0 && col < image->width-1,
                "illegal pixel in gan_image_bit_get_pix_3group_horiz()" );

   /* get pixel */
   return ((gan_bit_array_get_bit ( &image->ba[row], col-1 ) &&
            gan_bit_array_get_bit ( &image->ba[row], col   ) &&
            gan_bit_array_get_bit ( &image->ba[row], col+1 ))
           ? GAN_TRUE : GAN_FALSE);
}

/**
 * \brief Test local group of three binary pixels in a column.
 *
 * Return #GAN_TRUE if bits at positions \a (row,col), \a (row-1,col) and
 * \a (row+1,col) are all set to one (true), or #GAN_FALSE otherwise.
 */
Gan_Bool
 gan_image_bit_get_pix_3group_vert ( const Gan_Image *image,
                                     unsigned row, unsigned col )
{
   /* consistency check */
   assert ( image->format == GAN_GREY_LEVEL_IMAGE && image->type == GAN_BOOL );

   gan_assert ( row > 0 && row < image->height-1 && image->width > 0 &&
                col < image->width,
                "illegal pixel in gan_image_bit_get_pix_3group_vert()" );

   /* get pixel */
   return ((gan_bit_array_get_bit ( &image->ba[row-1], col ) &&
            gan_bit_array_get_bit ( &image->ba[row],   col   ) &&
            gan_bit_array_get_bit ( &image->ba[row+1], col ))
           ? GAN_TRUE : GAN_FALSE);
}

#endif /* #ifndef NDEBUG */

/**
 * \brief Returns number of pixels set or unset in the given binary image.
 *
 * If \a val is passed as #GAN_TRUE, returns the number of pixels set to
 * one (true) in the given \a image. If \a val is #GAN_FALSE, counts the
 * number of zeros instead. If \a subwin is not \c NULL, counts the pixels
 * only inside the given subwindow.
 */
int
 gan_image_get_pixel_count_b ( const Gan_Image *image, 
                               Gan_Bool val,
                               const Gan_ImageWindow *psubwin )
{
   Gan_ImageWindow subwin;
   unsigned int max_row, row;
   unsigned int max_col, col;
   int pixel_count = 0;
   Gan_Bool success;

   if ( image == NULL )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_image_get_pixel_count",
                         GAN_ERROR_ILLEGAL_ARGUMENT, "NULL image pointer" );
      return -1;
   }

   if(psubwin == NULL)
   {
      success = gan_image_get_active_subwindow_b(image, GAN_BYTE_ALIGNMENT,
                                                 &subwin);
      if(!success)
      {
         gan_err_flush_trace();
         gan_err_register ( "gan_image_get_pixel_count",
                            GAN_ERROR_NO_DATA,
                            "Unable to determine active subimage" );
         return -1;
      }
   }
   else
      subwin = *psubwin;

   max_row = subwin.r0 + subwin.height;
   max_col = subwin.c0 + subwin.width;

   for(row = subwin.r0; row < max_row; row++)
      for(col = subwin.c0; col < max_col; col++)
         if(gan_image_get_pix_b( image, row, col ))
            pixel_count++;

   /* if we wanted the number of unset pixels, adjust pixel count */
   if ( !val )
      pixel_count = (int)(image->height*image->width) - pixel_count;

   return pixel_count;
}

/**
 * \brief Invert binary image.
 * \return The result image \a result_image.
 *
 * Invert all the bits in the given \a image, writing the result into
 * \a result_image.
 */
Gan_Image *
 gan_image_bit_invert_q ( Gan_Image *image, Gan_Image *result_image )
{
   int iRow;

   gan_err_test_ptr ( image->format == GAN_GREY_LEVEL_IMAGE &&
                      image->type == GAN_BOOL, "image_bit_and_q",
                      GAN_ERROR_INCOMPATIBLE, "" );

   if ( result_image == NULL )
      result_image = gan_image_copy_s ( image );
   else if ( image != result_image )
      result_image = gan_image_copy_q ( image, result_image );
   
   if ( result_image == NULL )
   {
      gan_err_register ( "gan_image_bit_invert_q", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* apply bitwise inversion operation */
   for ( iRow = (int)image->height-1; iRow >= 0; iRow-- )
      if ( !gan_bit_array_invert_i ( &result_image->ba[iRow] ) )
      {
         gan_err_register ( "gan_image_bit_invert_q", GAN_ERROR_FAILURE, "" );
         return NULL;
      }
   
   /* success */
   return result_image;
}

/**
 * \brief Binary \c AND of all pixels in a binary image.
 * \return Result binary image \a result.
 *
 * Apply \c AND operation to two input binary images \a image1 and \a image2,
 * writing the result into \a result.
 */
Gan_Image *
 gan_image_bit_and_q ( Gan_Image *image1, Gan_Image *image2,
                       Gan_Image *result )
{
   int iRow;

   gan_err_test_ptr ( image1->format == GAN_GREY_LEVEL_IMAGE &&
                      image1->type == GAN_BOOL, "image_bit_and_q",
                      GAN_ERROR_INCOMPATIBLE, "" );
   gan_err_test_ptr ( image2->format == GAN_GREY_LEVEL_IMAGE &&
                      image2->type == GAN_BOOL, "image_bit_and_q",
                      GAN_ERROR_INCOMPATIBLE, "" );
   gan_err_test_ptr ( gan_image_test_dims(image1,image2),
                      "image_bit_and_q", GAN_ERROR_INCOMPATIBLE, "" );

   if ( result == NULL )
      result = gan_image_copy_s ( image1 );
   else if ( image1 != result )
      result = gan_image_copy_q ( image1, result );
   
   if ( result == NULL )
   {
      gan_err_register ( "gan_image_bit_and_q", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* apply AND operation */
   for ( iRow = (int)image1->height-1; iRow >= 0; iRow-- )
      if ( !gan_bit_array_and_i ( &result->ba[iRow], &image2->ba[iRow] ) )
      {
         gan_err_register ( "gan_image_bit_and_q", GAN_ERROR_FAILURE, "" );
         return NULL;
      }
   
   /* success */
   return result;
}

/**
 * \brief Binary \c NAND of all pixels in a binary image.
 * \return Result binary image \a result.
 *
 * Apply \c NAND operation (not \c AND) to two input binary images \a image1
 * and \a image2, writing the result into \a result.
 */
Gan_Image *
 gan_image_bit_nand_q ( Gan_Image *image1, Gan_Image *image2,
                        Gan_Image *result )
{
   int iRow;

   gan_err_test_ptr ( image1->format == GAN_GREY_LEVEL_IMAGE &&
                      image1->type == GAN_BOOL, "image_bit_nand_q",
                      GAN_ERROR_INCOMPATIBLE, "" );
   gan_err_test_ptr ( image2->format == GAN_GREY_LEVEL_IMAGE &&
                      image2->type == GAN_BOOL, "image_bit_nand_q",
                      GAN_ERROR_INCOMPATIBLE, "" );
   gan_err_test_ptr ( gan_image_test_dims(image1,image2),
                      "image_bit_nand_q", GAN_ERROR_INCOMPATIBLE, "" );

   if ( result == NULL )
      result = gan_image_copy_s ( image1 );
   else if ( image1 != result )
      result = gan_image_copy_q ( image1, result );
   
   if ( result == NULL )
   {
      gan_err_register ( "gan_image_bit_nand_q", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* apply NAND operation */
   for ( iRow = (int)image1->height-1; iRow >= 0; iRow-- )
      if ( !gan_bit_array_nand_i ( &result->ba[iRow], &image2->ba[iRow] ) )
      {
         gan_err_register ( "gan_image_bit_nand_q", GAN_ERROR_FAILURE, "" );
         return NULL;
      }
   
   /* success */
   return result;
}

/**
 * \brief Binary \c OR of all pixels in a binary image.
 * \return Result binary image \a result.
 *
 * Apply \c OR operation to two input binary images \a image1 and \a image2,
 * writing the result into \a result.
 */
Gan_Image *
 gan_image_bit_or_q ( Gan_Image *image1, Gan_Image *image2,
                      Gan_Image *result )
{
   int iRow;

   gan_err_test_ptr ( image1->format == GAN_GREY_LEVEL_IMAGE &&
                      image1->type == GAN_BOOL, "image_bit_or_q",
                      GAN_ERROR_INCOMPATIBLE, "" );
   gan_err_test_ptr ( image2->format == GAN_GREY_LEVEL_IMAGE &&
                      image2->type == GAN_BOOL, "image_bit_or_q",
                      GAN_ERROR_INCOMPATIBLE, "" );
   gan_err_test_ptr ( gan_image_test_dims(image1,image2),
                      "image_bit_or_q", GAN_ERROR_INCOMPATIBLE, "" );

   if ( result == NULL )
      result = gan_image_copy_s ( image1 );
   else if ( image1 != result )
      result = gan_image_copy_q ( image1, result );
   
   if ( result == NULL )
   {
      gan_err_register ( "gan_image_bit_or_q", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* apply OR operation */
   for ( iRow = (int)image1->height-1; iRow >= 0; iRow-- )
      if ( !gan_bit_array_or_i ( &result->ba[iRow], &image2->ba[iRow] ) )
      {
         gan_err_register ( "gan_image_bit_or_q", GAN_ERROR_FAILURE, "" );
         return NULL;
      }
   
   /* success */
   return result;
}

/**
 * \brief Binary \c EOR of all pixels in a binary image.
 * \return Result binary image \a result.
 *
 * Apply \c EOR operation (exclusive \c OR) to two input binary images
 * \a image1 and \a image2, writing the result into \a result.
 */
Gan_Image *
 gan_image_bit_eor_q ( Gan_Image *image1, Gan_Image *image2,
                       Gan_Image *result )
{
   int iRow;

   gan_err_test_ptr ( image1->format == GAN_GREY_LEVEL_IMAGE &&
                      image1->type == GAN_BOOL, "image_bit_eor_q",
                      GAN_ERROR_INCOMPATIBLE, "" );
   gan_err_test_ptr ( image2->format == GAN_GREY_LEVEL_IMAGE &&
                      image2->type == GAN_BOOL, "image_bit_eor_q",
                      GAN_ERROR_INCOMPATIBLE, "" );
   gan_err_test_ptr ( gan_image_test_dims(image1,image2),
                      "image_bit_eor_q", GAN_ERROR_INCOMPATIBLE, "" );

   if ( result == NULL )
      result = gan_image_copy_s ( image1 );
   else if ( image1 != result )
      result = gan_image_copy_q ( image1, result );
   
   if ( result == NULL )
   {
      gan_err_register ( "gan_image_bit_eor_q", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* apply exclusive OR operation */
   for ( iRow = (int)image1->height-1; iRow >= 0; iRow-- )
      if ( !gan_bit_array_eor_i ( &result->ba[iRow], &image2->ba[iRow] ) )
      {
         gan_err_register ( "gan_image_bit_eor_q", GAN_ERROR_FAILURE, "" );
         return NULL;
      }
   
   /* success */
   return result;
}

/**
 * \brief Binary \c AND-NOT of all pixels in a binary image.
 * \return Result binary image \a result.
 *
 * Apply \c AND-NOT operation to two input binary images \a image1
 * and \a image2, writing the result into \a result.
 */
Gan_Image *
 gan_image_bit_andnot_q ( Gan_Image *image1, Gan_Image *image2,
                          Gan_Image *result )
{
   int iRow;

   gan_err_test_ptr ( image1->format == GAN_GREY_LEVEL_IMAGE &&
                      image1->type == GAN_BOOL, "image_bit_andnot_q",
                      GAN_ERROR_INCOMPATIBLE, "" );
   gan_err_test_ptr ( image2->format == GAN_GREY_LEVEL_IMAGE &&
                      image2->type == GAN_BOOL, "image_bit_andnot_q",
                      GAN_ERROR_INCOMPATIBLE, "" );
   gan_err_test_ptr ( gan_image_test_dims(image1,image2),
                      "image_bit_andnot_q", GAN_ERROR_INCOMPATIBLE, "" );

   if ( result == NULL )
      result = gan_image_copy_s ( image1 );
   else if ( image1 != result )
      result = gan_image_copy_q ( image1, result );
   
   if ( result == NULL )
   {
      gan_err_register ( "gan_image_bit_andnot_q", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* apply AND-NOT operation */
   for ( iRow = (int)image1->height-1; iRow >= 0; iRow-- )
      if ( !gan_bit_array_andnot_i ( &result->ba[iRow], &image2->ba[iRow] ) )
      {
         gan_err_register ( "gan_image_bit_andnot_q", GAN_ERROR_FAILURE, "" );
         return NULL;
      }
   
   /* success */
   return result;
}

/**
 * \brief Fill part of a row of a binary image.
 *
 * Fill part of a row of a binary \a image, starting at position \a x, \a y
 * and filling \a width pixels to the right.
 */
Gan_Bool
 gan_image_bit_fill_row ( Gan_Image *image, unsigned y,
                          unsigned x, unsigned width, Gan_Bool pix )
{
   /* compatibility checks */
   gan_err_test_bool ( image->format == GAN_GREY_LEVEL_IMAGE &&
                       image->type == GAN_BOOL,
                       "gan_image_bit_fill_row", GAN_ERROR_ILLEGAL_ARGUMENT,
                       "" );
   gan_err_test_bool ( x+width <= image->width && y < image->height,
                       "gan_image_bit_fill_row", GAN_ERROR_ILLEGAL_ARGUMENT,
                       "" );

   if ( !gan_bit_array_fill_part ( &image->ba[y], x, width, pix ) )
   {
      gan_err_register ( "gan_image_bit_fill_row", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   /* success */
   return GAN_TRUE;
}

/**
 * \brief Invert part of a row of a binary image.
 *
 * Invert part of a row of a binary \a image, starting at position \a x, \a y
 * and filling \a width pixels to the right.
 */
Gan_Bool
 gan_image_bit_invert_row ( Gan_Image *image, unsigned y,
                            unsigned x, unsigned width )
{
   /* compatibility checks */
   gan_err_test_bool ( image->format == GAN_GREY_LEVEL_IMAGE &&
                       image->type == GAN_BOOL,
                       "gan_image_bit_invert_row", GAN_ERROR_ILLEGAL_ARGUMENT,
                       "" );
   gan_err_test_bool ( x+width <= image->width && y < image->height,
                       "gan_image_bit_invert_row", GAN_ERROR_ILLEGAL_ARGUMENT,
                       "" );

   for ( ; --width > 0; x++ )
      gan_image_set_pix_b ( image, y, x,
                            (Gan_Bool)!gan_image_get_pix_b(image,y,x) );

   /* success */
   return GAN_TRUE;
}

/**
 * \brief Dilate binary image horizontally
 * \param image Binary image to dilate
 * \param no_pixels Number of pixels to dilate image by
 * \param restrict_mask Mask to restrict dilation or \c NULL
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Dilate binary image \a image horizontally by given number of pixels
 * \a no_pixels, optionally specifying a mask \a restrict_mask to restrict
 * the dilation.
 */
Gan_Bool
 gan_image_bit_dilate_horiz ( Gan_Image *image, int no_pixels,
                              Gan_Image *restrict_mask )
{
   Gan_ImageWindow subwin;
   int row, row_max;

   /* return immediately if zero expansion is requested */
   if ( no_pixels == 0 ) return GAN_TRUE;

   if ( !gan_image_get_active_subwindow_b( image, GAN_BIT_ALIGNMENT, &subwin) )
   {
      gan_err_register ( "gan_image_bit_dilate_horiz", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   if ( subwin.width == 0 || subwin.height == 0 )
      return GAN_TRUE;

   row_max = (int)(subwin.r0 + subwin.height);

   /* expand horizontally */
   if ( restrict_mask == NULL )
      for ( row = (int)subwin.r0; row < row_max; row++ )
      {
         if ( !gan_bit_array_dilate_i ( &image->ba[row],
                                        (unsigned)no_pixels ) )
         {
            gan_err_register ( "gan_image_bit_dilate_horiz", GAN_ERROR_FAILURE,
                               "" );
            return GAN_FALSE;
         }
      }
   else
   {
      Gan_BitArray *bit_array=NULL;

      bit_array = gan_bit_array_alloc(image->width);
      if ( bit_array == NULL )
      {
         gan_err_register ( "gan_image_bit_dilate_horiz", GAN_ERROR_FAILURE,
                            "" );
         return GAN_FALSE;
      }
      
      for ( row = (int)subwin.r0; row < row_max; row++ )
      {
         if ( !gan_bit_array_dilate_q ( &image->ba[row], (unsigned)no_pixels,
                                        bit_array ) ||
              !gan_bit_array_and_i ( bit_array, &restrict_mask->ba[row] ) ||
              !gan_bit_array_or_i ( &image->ba[row], bit_array ) )
         {
            gan_err_register ( "gan_image_bit_dilate_horiz", GAN_ERROR_FAILURE,
                               "" );
            return GAN_FALSE;
         }
      }

      gan_bit_array_free(bit_array);
   }

   return GAN_TRUE;
}

/**
 * \brief Dilate binary image vertically
 * \param image Binary image to dilate
 * \param no_pixels Number of pixels to dilate image by
 * \param restrict_mask Mask to restrict dilation or \c NULL
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Dilate binary image \a image vertically by given number of pixels
 * \a no_pixels, optionally specifying a mask \a restrict_mask to restrict
 * the dilation.
 */
Gan_Bool
 gan_image_bit_dilate_vert ( Gan_Image *image, int no_pixels,
                             Gan_Image *restrict_mask )
{
   Gan_ImageWindow subwin;
   int row, row_max, pixel;
   Gan_BitArray *bit_array=NULL, **array=NULL;

   /* return immediately if zero expansion is requested */
   if ( no_pixels == 0 ) return GAN_TRUE;

   if ( !gan_image_get_active_subwindow_b( image, GAN_BIT_ALIGNMENT, &subwin) )
   {
      gan_err_register ( "gan_image_bit_dilate_vert", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   if ( subwin.width == 0 || subwin.height == 0 )
      return GAN_TRUE;

   bit_array = gan_bit_array_alloc(image->width);
   if ( bit_array == NULL )
   {
      gan_err_register ( "gan_image_bit_dilate_vert", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   /* expand vertically */
   array = gan_malloc_array ( Gan_BitArray *, no_pixels+1 );
   if ( array == NULL )
   {
      gan_err_flush_trace();
      gan_err_register_with_number ( "gan_image_bit_dilate_vert", GAN_ERROR_MALLOC_FAILED, "", (no_pixels+1)*sizeof(Gan_BitArray *) );
      return GAN_FALSE;
   }

   gan_fill_array_p ( (void **)array, no_pixels+1, 1, NULL );

   if ( (int)subwin.r0 < no_pixels )
   {
      subwin.height += subwin.r0;
      subwin.r0 = 0;
   }
   else
   {
      subwin.r0 -= (unsigned)no_pixels;
      subwin.height += (unsigned)no_pixels;
   }

   subwin.height += (unsigned)no_pixels;
   if ( subwin.r0+subwin.height > (int)image->height )
      subwin.height = (int)image->height-subwin.r0;

   row_max = (int)(subwin.r0 + subwin.height);
   if ( restrict_mask == NULL )
      for ( row = (int)subwin.r0; row < row_max; row++ )
      {
         Gan_BitArray *ptmp;

         /* shift array of bit arrays */
         ptmp = array[0];
         for ( pixel = 0; pixel < no_pixels; pixel++ )
            array[pixel] = array[pixel+1];

         array[no_pixels] = ptmp;

         /* copy this row */
         if ( array[no_pixels] == NULL )
         {
            array[no_pixels] = gan_bit_array_alloc(image->width);
            if ( array[no_pixels] == NULL )
            {
               gan_err_register ( "gan_image_bit_dilate_vert",
                                  GAN_ERROR_FAILURE, "" );
               return GAN_FALSE;
            }
         }

         if ( !gan_bit_array_copy_q ( &image->ba[row], array[no_pixels] ) )
         {
            gan_err_register ( "gan_image_bit_dilate_vert", GAN_ERROR_FAILURE,
                               "" );
            return GAN_FALSE;
         }

         /* compute results for this row */
         for ( pixel = no_pixels-1; pixel >= 0; pixel-- )
         {
            if ( array[pixel] != NULL &&
                 !gan_bit_array_or_i ( &image->ba[row], array[pixel] ) )
            {
               gan_err_register ( "gan_image_bit_dilate_vert",
                                  GAN_ERROR_FAILURE, "" );
               return GAN_FALSE;
            }

            if ( row+pixel+1 < (int)image->height &&
                 !gan_bit_array_or_i ( &image->ba[row],
                                       &image->ba[row+pixel+1] ) )
            {
               gan_err_register ( "gan_image_bit_dilate_vert",
                                  GAN_ERROR_FAILURE, "" );
               return GAN_FALSE;
            }
         }
      }
   else
      for ( row = (int)subwin.r0; row < row_max; row++ )
      {
         Gan_BitArray *ptmp;

         /* shift array of bit arrays */
         ptmp = array[0];
         for ( pixel = 0; pixel < no_pixels; pixel++ )
            array[pixel] = array[pixel+1];

         array[no_pixels] = ptmp;

         /* copy this row */
         if ( array[no_pixels] == NULL )
         {
            array[no_pixels] = gan_bit_array_alloc(image->width);
            if ( array[no_pixels] == NULL )
            {
               gan_err_register ( "gan_image_bit_dilate_vert",
                                  GAN_ERROR_FAILURE, "" );
               return GAN_FALSE;
            }
         }

         if ( !gan_bit_array_copy_q ( &image->ba[row], array[no_pixels] ) ||
              !gan_bit_array_copy_q ( &image->ba[row], bit_array ) )
         {
            gan_err_register ( "gan_image_bit_dilate_vert", GAN_ERROR_FAILURE,
                               "" );
            return GAN_FALSE;
         }

         /* compute results for this row */
         for ( pixel = no_pixels-1; pixel >= 0; pixel-- )
         {
            if ( array[pixel] != NULL &&
                 !gan_bit_array_or_i ( bit_array, array[pixel] ) )
            {
               gan_err_register ( "gan_image_bit_dilate_vert",
                                  GAN_ERROR_FAILURE, "" );
               return GAN_FALSE;
            }

            if ( row+pixel+1 < (int)image->height &&
                 !gan_bit_array_or_i ( bit_array, &image->ba[row+pixel+1] ) )
            {
               gan_err_register ( "gan_image_bit_dilate_vert",
                                  GAN_ERROR_FAILURE, "" );
               return GAN_FALSE;
            }
         }

         /* apply restriction mask */
         if ( !gan_bit_array_and_i ( bit_array, &restrict_mask->ba[row] ) ||
              !gan_bit_array_or_i ( &image->ba[row], bit_array ) )
         {
            gan_err_register ( "gan_image_bit_dilate_vert", GAN_ERROR_FAILURE,
                               "" );
            return GAN_FALSE;
         }
      }

   /* success */
   for ( pixel = no_pixels; pixel >= 0; pixel-- )
      if ( array[pixel] != NULL )
         gan_bit_array_free(array[pixel]);

   free(array);
   gan_bit_array_free(bit_array);
         
   return GAN_TRUE;
}

/**
 * \brief Shift binary image horizontally and vertically
 * \param image Binary image to shift
 * \param vshift Vertical shift
 * \param hshift Horizontal shift
 * \param result_image Binary result image
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 */
Gan_Bool gan_image_bit_shift ( const Gan_Image *image, int vshift, int hshift,
                               Gan_Image *result_image )
{
   Gan_ImageWindow subwin;
   int row, row_max, rheight = result_image->height;

   if(image->format != GAN_GREY_LEVEL_IMAGE || image->type != GAN_BOOL ||
      result_image->format != GAN_GREY_LEVEL_IMAGE ||
      result_image->type != GAN_BOOL)
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_image_bit_shift", GAN_ERROR_INCOMPATIBLE, "" );
      return GAN_FALSE;
   }

   // can't do in-place shift
   if(image == result_image)
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_image_bit_shift", GAN_ERROR_INCOMPATIBLE, "" );
      return GAN_FALSE;
   }

   // fill result image with zeros
   gan_image_fill_const_b(result_image, GAN_FALSE);

   // get subwindow
   if ( !gan_image_get_active_subwindow_b( image, GAN_WORD_ALIGNMENT, &subwin) )
   {
      gan_err_register ( "gan_image_bit_shift", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   // shift each row in turn
   row_max = (int)(subwin.r0+subwin.height);
   for(row = (int)subwin.r0; row < row_max; row++)
   {
      if(row+vshift >= 0 && row+vshift<rheight)
      {
         if(!gan_bit_array_shift_q( &image->ba[row], hshift, &result_image->ba[row+vshift] ))
         {
            gan_err_register ( "gan_image_bit_shift", GAN_ERROR_FAILURE, "" );
            return GAN_FALSE;
         }
      }
   }

   return GAN_TRUE;
}

/**
 * \}
 */

/**
 * \}
 */

/**
 * File:          $RCSfile: image_common_noc.c,v $
 * Module:        Image functions common across all formats & types
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

/* this file contains image package functions common to all image formats
 * and types, as controlled by the following preprocessor symbols:
 *
 *   GAN_PIXEL:   the C type used to store a pixel, e.g. int, unsigned int.
 *   GAN_PIXFMT:  the Gandalf format identifier for the image format,
 *                e.g. GAN_GREY_LEVEL_IMAGE, GAN_RGB_COLOUR_IMAGE.
 *   GAN_PIXTYPE: the Gandalf type identifier for the image type,
 *                e.g. GAN_UCHAR, GAN_UINT, GAN_DOUBLE.
 *   GAN_IMFMT:   the short format identifier used in structure fields and
 *                function names, e.g. gl for grey-level images, rgb for
 *                RGB colour images.
 *   GAN_IMTYPE:  the short type identifier, e.g. ui for unsigned int,
 *                f for float.
 *   GAN_IMAGE_SET_GEN: the function name to use for the image format,
 *                      type and dimension setting function,
 *                      e.g. gan_image_set_gl_uc for grey-level unsigned
 *                      character images, or gan_image_set_rgb_d for
 *                      RGB colour real images.
 *   GAN_IMAGE_FORM_GEN: the function name to use for the general
 *                       image allocation routine,
 *                       e.g. gan_image_form_gen_gl_uc,
 *                       gan_image_form_gen_rgb_d.
 */

/* function to re-allocate image data */
static Gan_Bool
 image_realloc ( Gan_Image *img,
                 unsigned long height, unsigned long width,
                 unsigned long stride, Gan_Bool alloc_pix_data )
{
   int r;

   /* consistency check */
   if ( img->format != GAN_PIXFMT || img->type != GAN_PIXTYPE )
   {
      gan_err_flush_trace();
      gan_err_register ( "image_realloc", GAN_ERROR_INCOMPATIBLE, "" );
      return GAN_FALSE;
   }

   /* set image dimensions and stride */
   img->height = height;
   img->width  = width;
   img->stride = stride;

#ifdef GAN_BITMAP
   gan_err_test_bool ( img->format == GAN_GREY_LEVEL_IMAGE, "image_realloc", GAN_ERROR_INCOMPATIBLE, "" );

   /* stride must be a multiple of bit-word size, so that rows are aligned
      to bit-word boundaries */
   gan_err_test_bool ( stride % sizeof(Gan_BitWord) == 0, "image_realloc", GAN_ERROR_INCOMPATIBLE, "" );

   /* check that stride is big enough given the width */
   gan_err_test_bool ( stride/sizeof(Gan_BitWord) >= (width + GAN_BITWORD_SIZE - 1)/GAN_BITWORD_SIZE, "image_realloc", GAN_ERROR_INCOMPATIBLE, "" );
#else
   /* check that stride is big enough given the width */
   gan_err_test_bool ( stride >= width*sizeof(GAN_PIXEL), "image_realloc", GAN_ERROR_INCOMPATIBLE, "" );
#endif

   if ( alloc_pix_data )
   {
      if ( img->pix_data_size < height*stride )
      {
         /* re-allocate image pixel data */
         if ( !img->pix_data_alloc )
         {
            gan_err_flush_trace();
            gan_err_register ( "image_realloc", GAN_ERROR_INCOMPATIBLE, "cannot reallocate image data" );
            return GAN_FALSE;
         }

         /* reallocate pixel data */
         img->pix_data_size = height*stride;
         img->pix_data_ptr = (unsigned char *) realloc ( img->pix_data_ptr, img->pix_data_size );
         if ( img->pix_data_ptr == NULL )
         {
            gan_err_flush_trace();
            gan_err_register_with_number ( "image_realloc", GAN_ERROR_MALLOC_FAILED, "", img->pix_data_size );
            return GAN_FALSE;
         }
#if 0 // ndef NDEBUG
         if(img->pix_data_size > 100000)
            printf("Image data allocated: %d KBytes\n", img->pix_data_size/1024);
#endif
      }
   }

   if ( img->row_data_size < height*sizeof(GAN_PIXEL *) )
   {
      /* re-allocate image row pointer data */
      if ( !img->row_data_alloc )
      {
         gan_err_flush_trace();
         gan_err_register ( "image_realloc", GAN_ERROR_INCOMPATIBLE, "cannot reallocate row data" );
         return GAN_FALSE;
      }
      
      img->row_data_size = height*sizeof(GAN_PIXEL *);
      img->row_data.GAN_IMFMT.GAN_IMTYPE = (GAN_PIXEL **)realloc ( img->row_data_ptr, img->row_data_size );

      /* reset generic pointer to row pointer data */
      img->row_data_ptr = (void *) img->row_data.GAN_IMFMT.GAN_IMTYPE;
   }
   else
      /* just reset pointer in case format/type has changed */
      img->row_data.GAN_IMFMT.GAN_IMTYPE = (GAN_PIXEL **) img->row_data_ptr;

   if ( img->pix_data_ptr == NULL )
   {
      /* reset row pointers to NULL */
      for ( r = (int)height-1; r >= 0; r-- )
         img->row_data.GAN_IMFMT.GAN_IMTYPE[r] = NULL;
   }
   else /* img->pix_data_ptr != NULL  */
   {
      /* reset row pointers */
      for ( r = (int)height-1; r >= 0; r-- )
         img->row_data.GAN_IMFMT.GAN_IMTYPE[r] = (GAN_PIXEL *) &img->pix_data_ptr[r*stride];
   }

#ifdef GAN_BITMAP
   if ( height == 0 )
      img->ba = NULL;
   else
   {
      // if the row data size is big enough, use it
      if ( !img->row_data_alloc
           && img->row_data_size >= height*sizeof(GAN_PIXEL *) + height*sizeof(Gan_BitArray) )
      {
         img->ba = (Gan_BitArray *) (((GAN_PIXEL **) img->row_data_ptr) + height);
         img->ba_alloc = GAN_FALSE;
      }
      else
      {
         img->ba = gan_realloc_array ( Gan_BitArray, img->ba, height );
         if ( img->ba == NULL )
         {
            gan_err_flush_trace();
            gan_err_register_with_number ( "image_realloc", GAN_ERROR_MALLOC_FAILED, "", height*sizeof(img->ba) );
            return GAN_FALSE;
         }

         img->ba_alloc = GAN_TRUE;
      }
   }

   if ( img->pix_data_ptr == NULL )
   {
      for ( r = (int)height-1; r >= 0; r-- )
         gan_bit_array_form_data ( &img->ba[r], NULL, 0, 0 );
   }
   else /* img->pix_data_ptr != NULL */
   {
      for ( r = (int)height-1; r >= 0; r-- )
         gan_bit_array_form_data (&img->ba[r], (Gan_BitWord *) &img->pix_data_ptr[r*stride], stride/sizeof(Gan_BitWord), width );
#if 0
      stride*GAN_BITWORD_SIZE/sizeof(Gan_BitWord));
#endif
   }                                  
#endif

   /* set user-defined offsets to default zero value */
   img->offset_x = img->offset_y = 0;

   /* success */
   return GAN_TRUE;
}

/* Function to free an image */
static void
 image_free ( Gan_Image *img )
{
   /* consistency check */
   assert ( img->format == GAN_PIXFMT && img->type == GAN_PIXTYPE );

#ifdef GAN_BITMAP
   assert ( img->format == GAN_GREY_LEVEL_IMAGE );
   if ( img->ba_alloc && img->height > 0 ) free ( img->ba );
#endif

   /* free array of row pointers */
   if ( img->row_data_alloc && img->row_data.GAN_IMFMT.GAN_IMTYPE != NULL )
      free ( img->row_data.GAN_IMFMT.GAN_IMTYPE );

#if 0 // ndef NDEBUG
   if(img->pix_data_size > 100000)
      printf("Image data freed: %d KBytes\n", img->pix_data_size/1024);
#endif

   /* free pixel data array */
   if ( img->pix_data_alloc )
   {
      if ( img->pix_data_ptr != NULL )
         free ( img->pix_data_ptr );
   }
   else
      /* use any user-supplied function to free data that was not allocated
         internally by Gandalf */
      if ( img->data_free_func != NULL && img->pix_data_ptr != NULL )
         img->data_free_func ( img->pix_data_ptr );

   /* free structure */
   if ( img->struct_alloc ) free(img);
}

/**
 * \addtogroup ImageSet
 * \{
 */

/**
 * \brief Set format, type, dimension and stride attributes of an image.
 * \return Result image \a img on success, \c NULL on failure.
 *
 * Set image \a img to GAN_PIXEL_FORMAT format and \c GAN_PIXEL_TYPE type,
 * and given dimensions \a height, \a width, and given \a stride.
 * \a img should be a previously created image, although it may have have
 * any format and type before this function is called. Gandalf will reorganise
 * and reallocate data accordingly in order to correspond to the requested
 * new format and type. \a alloc_pix_data specifies whether to allocate
 * the pixel data.
 */
Gan_Image *
 GAN_IMAGE_SET_GEN ( Gan_Image *img,
                     unsigned long height, unsigned long width,
                     unsigned long stride, Gan_Bool alloc_pix_data )
{
#ifndef GAN_BITMAP
   /* if we're changing a bit image into another type, free the bit array */
   if ( img->type == GAN_BOOL && img->ba != NULL )
   {
      gan_err_test_ptr ( img->format == GAN_GREY_LEVEL_IMAGE, "gan_image_set_gen_?", GAN_ERROR_INCOMPATIBLE, "" );
      free ( img->ba );
      img->ba = NULL;
   }
#endif

   /* set format and type of image */
   img->format = GAN_PIXFMT;
   img->type = GAN_PIXTYPE;

   /* re-allocate image pixel data if necessary */
   if ( !image_realloc ( img, height, width, stride, alloc_pix_data ) )
   {
      gan_err_register ( "gan_image_set_gen_?", GAN_ERROR_FAILURE, "" );
      return NULL;         
   }

   /* set member functions for image */
   img->free_func                       = image_free;
#ifndef NDEBUG
   img->set_pix.GAN_IMFMT.GAN_IMTYPE    = image_set_pix;
   img->get_pix.GAN_IMFMT.GAN_IMTYPE    = image_get_pix;
#endif
   img->fill_const.GAN_IMFMT.GAN_IMTYPE = image_fill_const;
   img->copy                            = image_copy;

   return img;
}

/**
 * \}
 */

/**
 * \addtogroup ImageAllocate
 * \{
 */

/**
 * \brief Form an image with specific format, type, dimensions and stride.
 *
 * Forms and returns a new image with GAN_PIXEL_FORMAT format,
 * \c GAN_PIXEL_TYPE type, dimensions \a height, \a width and given \a stride.
 * If the \a img image pointer is not \c NULL, \a img is formed; otherwise the
 * image is dynamically allocated. Likewise if the provided \a pix_data and/or
 * \a row_data pointers are \c NULL (in any combination), the pixel data array
 * and/or the row start pointer array are dynamically allocated.
 * If either are not \c NULL they must be big enough to hold the pixel/row
 * pointer array data. If necessary use gan_image_pixel_size() to compute the
 * necessary size of the \a pix_data array (the necessary size of \a row_data
 * is \a height pointers).
 *
 * If you want to dynamically allocate the pixel data array and/or the
 * row pointer array with sizes greater than that necessary for the
 * given \a height and \a stride, pass \a pix_data and/or \a row_data
 * as \c NULL and set \a pix_data_size and/or \a row_data_size appropriately.
 *
 * \sa The specific image creation functions, e.g. gan_image_alloc_gl_uc().
 */
Gan_Image *
 GAN_IMAGE_FORM_GEN ( Gan_Image *img,
                      unsigned long height, unsigned long width,
                      unsigned long stride, Gan_Bool alloc_pix_data,
                      GAN_PIXEL  *pix_data, size_t pix_data_size,
                      GAN_PIXEL **row_data, size_t row_data_size )
{
   gan_heap_push();
   if ( img == NULL )
   {
      /* dyamically allocate image */
      img = gan_malloc_object(Gan_Image);

      /* set dynamic allocation flag */
      img->struct_alloc = GAN_TRUE;
   }
   else
      /* indicate image was not dynamically allocated */
      img->struct_alloc = GAN_FALSE;

#ifdef GAN_BITMAP
   gan_err_test_ptr ( GAN_PIXFMT == GAN_GREY_LEVEL_IMAGE, "gan_image_form_gen_?", GAN_ERROR_INCOMPATIBLE, "" );

   /* stride must be a multiple of bit-word size, so that rows are aligned
      to bit-word boundaries */
   gan_err_test_ptr ( stride % sizeof(Gan_BitWord) == 0,
                      "gan_image_form_gen_?", GAN_ERROR_INCOMPATIBLE, "" );
#endif

   if ( alloc_pix_data )
   {
      gan_err_test_ptr ( pix_data == NULL, "gan_image_form_gen_?", GAN_ERROR_INCOMPATIBLE, "data specified illegally" );

      if ( pix_data_size == 0 )
         /* set size of image data to be allocated */
         img->pix_data_size = height*stride;
      else
      {
         /* a non-zero data size has been requested */
         gan_err_test_ptr ( pix_data_size >= height*stride, "gan_image_form_gen_?", GAN_ERROR_INCOMPATIBLE, "data array not large enough" );
         img->pix_data_size = pix_data_size;
      }

      if ( img->pix_data_size == 0 )
         img->pix_data_ptr = NULL;
      else
      {
         /* allocate image data */
         img->pix_data_ptr = (unsigned char *)malloc(img->pix_data_size);
         if ( img->pix_data_ptr == NULL )
         {
            gan_err_flush_trace();
            gan_err_register_with_number ( "gan_image_form_gen_?", GAN_ERROR_MALLOC_FAILED, "", img->pix_data_size );
            return NULL;
         }
      }

      /* set flag indicating pixel data is to be freed */
      img->pix_data_alloc = GAN_TRUE;
   }
   else /* !alloc_pix_data */
   {
      if ( pix_data == NULL )
      {
         gan_err_test_ptr ( pix_data_size == 0, "gan_image_form_gen_?",
                            GAN_ERROR_INCOMPATIBLE, "data/size" );
         img->pix_data_ptr = NULL;
         img->pix_data_size = 0;
      }
      else
      {
         /* pixel data for this image is provided */
         gan_err_test_ptr ( pix_data_size >= height*sizeof(GAN_PIXEL), "gan_image_form_gen_?", GAN_ERROR_INCOMPATIBLE, "data array not large enough" );

         /* set pixel data pointer in image */
         img->pix_data_ptr = (unsigned char *) pix_data;
         img->pix_data_size = pix_data_size;
      }

      /* we shouldn't free the image pixel data */
      img->pix_data_alloc = GAN_FALSE;
   }

   if ( row_data == NULL )
   {
      if ( row_data_size == 0 )
         /* set size of image row pointer data to be allocated */
         img->row_data_size = height*sizeof(GAN_PIXEL *);
      else
      {
         /* a non-zero row pointer data size has been requested */
         gan_err_test_ptr ( row_data_size >= height*sizeof(GAN_PIXEL *), "gan_image_form_gen_?", GAN_ERROR_INCOMPATIBLE, "data array not large enough" );
         img->row_data_size = row_data_size;
      }

      /* allocate row pointer data */
      if ( img->row_data_size == 0 )
         img->row_data.GAN_IMFMT.GAN_IMTYPE = NULL;
      else
      {
         img->row_data.GAN_IMFMT.GAN_IMTYPE = (GAN_PIXEL **)malloc (img->row_data_size);
         if ( img->row_data.GAN_IMFMT.GAN_IMTYPE == NULL )
         {
            gan_err_flush_trace();
            gan_err_register_with_number ( "gan_image_form_gen_?", GAN_ERROR_MALLOC_FAILED, "", img->row_data_size );
            return NULL;
         }
      }

      /* reset generic pointer to row pointer data */
      img->row_data_ptr = (void *) img->row_data.GAN_IMFMT.GAN_IMTYPE;

      /* set flag indicating the row pointer is to be freed */
      img->row_data_alloc = GAN_TRUE;
   }
   else
   {
      /* pixel data for this image is provided */
      gan_err_test_ptr ( row_data_size >= height*sizeof(GAN_PIXEL *), "gan_image_form_gen_?", GAN_ERROR_INCOMPATIBLE, "data array not large enough" );

      /* set pixel data pointer in image */
      img->row_data.GAN_IMFMT.GAN_IMTYPE = row_data;
      img->row_data_ptr  = row_data;
      img->row_data_size = row_data_size;

      /* we shouldn't free the image pixel data */
      img->row_data_alloc = GAN_FALSE;
   }

   /* set bitmap array to NULL. It will be set properly inside
      GAN_IMAGE_SET_GEN if needed */
   img->ba = NULL;
   img->ba_alloc = GAN_FALSE;

   /* set format and type of image */
   img->format = GAN_PIXFMT;
   img->type = GAN_PIXTYPE;

   /* set class member functions */
   img = GAN_IMAGE_SET_GEN ( img, height, width, stride, alloc_pix_data );
   gan_heap_pop();

   if ( img == NULL )
   {
      gan_err_register ( "gan_image_form_gen_?", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* set data free function to NULL to indicate free() default */
   img->data_free_func = NULL;

   return img;
}

/**
 * \}
 */

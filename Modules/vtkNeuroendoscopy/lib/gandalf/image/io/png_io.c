/**
 * File:          $RCSfile: png_io.c,v $
 * Module:        PNG format image file I/O functions
 * Part of:       Gandalf Library
 *
 * Revision:      $Revision: 1.1.2.1 $
 * Last edited:   $Date: 2007/10/14 02:20:29 $
 * Author:        $Author: ruetz $
 * Copyright:     (c) 2000 Imagineer Software Limited
 *
 * Notes:         Works with the libpng & lz libraries
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

#include <gandalf/image/io/png_io.h>
#include <gandalf/image/io/image_io.h>
#include <gandalf/image/image_bit.h>
#include <gandalf/common/misc_error.h>
#include <gandalf/common/compare.h>

/* only compile if you have PNG */
#ifdef HAVE_PNG

#include <png.h>

/**
 * \addtogroup ImagePackage
 * \{
 */

/**
 * \addtogroup ImageIO
 * \{
 */

/* Prototypes */
static Gan_Bool decode_header_info ( int png_colour_type, 
                                     int png_bit_depth, 
                                     Gan_ImageFormat *format, 
                                     Gan_Type *type );

static Gan_Bool encode_header_info ( const Gan_Image *image,
                                     int *png_colour_type, 
                                     int *png_bit_depth,
                                     Gan_Bool *has_alpha,
                                     int *error_code);

static Gan_Bool read_header(FILE *infile,
                            png_structp *png_ptr,
                            png_infop *info_ptr,
                            int *pcolour_type,
                            int *pbit_depth,
                            unsigned int *pheight,
                            unsigned int *pwidth);


/* Function which converts the format specific information into
 * Gandalf format and type information */
static Gan_Bool decode_header_info(int png_colour_type,
                                   int png_bit_depth,
                                   Gan_ImageFormat *format,
                                   Gan_Type *type)
{
   /* Determine format */
   switch(png_colour_type)
   {
      case PNG_COLOR_TYPE_GRAY:
        *format = GAN_GREY_LEVEL_IMAGE;
        break;
        
      case PNG_COLOR_TYPE_GRAY_ALPHA:
        *format = GAN_GREY_LEVEL_ALPHA_IMAGE;
        break;

      case PNG_COLOR_TYPE_PALETTE:
      case PNG_COLOR_TYPE_RGB:
        *format = GAN_RGB_COLOUR_IMAGE;
        break;

      case PNG_COLOR_TYPE_RGB_ALPHA:
        *format = GAN_RGB_COLOUR_ALPHA_IMAGE;
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "decode_header_info", GAN_ERROR_ILLEGAL_TYPE, "" );
        return GAN_FALSE;
   }

   /* type of pixel values: unsigned char, float etc.
    * and allocate memory*/
   switch(png_bit_depth)
   {
      case  1: *type = GAN_BOOL;   break;
      case  8: *type = GAN_UINT8;  break;
      case 16: *type = GAN_UINT16; break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "decode_header_info", GAN_ERROR_ILLEGAL_TYPE, "" );
        return GAN_FALSE;
   }

   return GAN_TRUE;
}

static Gan_Bool encode_header_info (const Gan_Image *image, int *png_colour_type, int *png_bit_depth, Gan_Bool *has_alpha, int *error_code )
{
   *has_alpha = GAN_FALSE;

   switch(image->format)
   {
      case GAN_GREY_LEVEL_IMAGE:
        *png_colour_type = PNG_COLOR_TYPE_GRAY;
        break;

      case GAN_GREY_LEVEL_ALPHA_IMAGE:
        *png_colour_type = PNG_COLOR_TYPE_GRAY_ALPHA;
        *has_alpha = GAN_TRUE;
        break;
                                        
      case GAN_RGB_COLOUR_IMAGE:
        *png_colour_type = PNG_COLOR_TYPE_RGB;
        break;

      case GAN_RGB_COLOUR_ALPHA_IMAGE:
        *png_colour_type = PNG_COLOR_TYPE_RGB_ALPHA;
        *has_alpha = GAN_TRUE;
        break;

      default:
        *error_code = GAN_ERROR_ILLEGAL_IMAGE_FORMAT;
        return GAN_FALSE;
   }

   switch(image->type)
   {
      case GAN_BOOL:   *png_bit_depth =  1; break;
      case GAN_UINT8:  *png_bit_depth =  8; break;
      case GAN_UINT16: *png_bit_depth = 16; break;

      default:
        *error_code = GAN_ERROR_ILLEGAL_IMAGE_TYPE;
        return GAN_FALSE;
   }

   return GAN_TRUE;
}

/* Function which reads the header of the image file to extract the image 
 * information */
static Gan_Bool read_header(FILE *infile,
                            png_structp *png_ptr,
                            png_infop *info_ptr,
                            int *pcolour_type,
                            int *pbit_depth,
                            unsigned int *pheight,
                            unsigned int *pwidth)
{
   gan_uint8 sig[8];
   // on 64-bit machines png_uint_32 is 64 bits!!!!!!!
   png_uint_32 pngui32Width, pngui32Height;

   /* Check signature */

   fread(sig, 1, 8, infile);
   if (!png_check_sig(sig, 8))
   {
      gan_err_flush_trace();
      gan_err_register("read_header", GAN_ERROR_CORRUPTED_FILE, "");
      return GAN_FALSE;   
   }

   /* Create read_struct */
   *png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
   if (!*png_ptr)
   {
      gan_err_flush_trace();
      gan_err_register("read_header", GAN_ERROR_EXTERNAL_LIBRARY_CALL, "");
      return GAN_FALSE;
   }

   /* Create info_struct */
   *info_ptr = png_create_info_struct(*png_ptr);
   if (!(*info_ptr)) 
   {
      gan_err_flush_trace();
      gan_err_register("read_header", GAN_ERROR_EXTERNAL_LIBRARY_CALL, "");
      return GAN_FALSE;
   }

   /* setjmp() call */
   if (setjmp(png_jmpbuf(*png_ptr)))
   {
      png_destroy_read_struct(png_ptr, info_ptr, NULL);
      gan_err_flush_trace();
      gan_err_register("read_header", GAN_ERROR_FAILURE, "");
      return GAN_FALSE;        
   }

   /* Initialise IO */
   png_init_io(*png_ptr, infile);
   png_set_sig_bytes(*png_ptr, 8); 

   /* Read image info */
   png_read_info(*png_ptr, *info_ptr); 

   /* Extract necessary image info */
   //printf("Sizeof(int)=%d sizeof(long)=%d sizeof(png_uint_32)=%d\n",(int)sizeof(int), (int)sizeof(long), (int)sizeof(png_uint_32));

   png_get_IHDR ( *png_ptr, *info_ptr, &pngui32Width, &pngui32Height, pbit_depth, pcolour_type,
                  NULL, NULL, NULL );
   *pwidth  = (unsigned int)pngui32Width;
   *pheight = (unsigned int)pngui32Height;
        
   return GAN_TRUE;
}

static void read_cleanup ( Gan_Image *image, 
                           png_structp png_ptr, 
                           png_infop info_ptr )
{
   if (image)
   {
      gan_image_free(image);
      image = NULL;
   }

   if (png_ptr && info_ptr)
   {
      png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
      png_ptr = NULL;
      info_ptr = NULL;
   } 
}

/**
 * \brief Determines whether an image is a PNG image using the magic number
 */
Gan_Bool gan_image_is_png(const unsigned char *magic_string, size_t length)
{
   if (length < 8)
      return(GAN_FALSE);

   if (memcmp(magic_string,"\211PNG\r\n\032\n",8) == 0)
      return(GAN_TRUE);

   return(GAN_FALSE);
}

/**
 * \brief Reads an image file in PNG format from a file stream.
 * \param infile The file stream to be read
 * \param image The image structure to read the image data into or NULL
 * \param ictrlstr Pointer to structure controlling input or \c NULL
 * \param header Pointer to file header structure to be filled, or \c NULL
 * \return Pointer to image structure, or \c NULL on failure.
 *
 * Reads the PNG image from the file stream \a infile into the given \a image.
 * If \a image is \c NULL a new image is dynamically allocated, otherwise the
 * already allocated \a image structure is reused. \a gam identifies the
 * gamma correction which the opened image should have. It should be one if no
 * gamma correction needs to be applied.
 *
 * \sa gan_write_png_image_stream().
 */
Gan_Image *
 gan_read_png_image_stream(FILE *infile, Gan_Image *image,
                           const struct Gan_ImageReadControlStruct *ictrlstr, struct Gan_ImageHeaderStruct *header )
{
   double file_gamma;
   Gan_ImageFormat format;
   Gan_Type type;
   unsigned int uiHeight, uiWidth, uiInternalHeight;
   int iColourType, iBitDepth;
   png_structp png_ptr = NULL;
   png_infop info_ptr = NULL;
   Gan_Bool flip=GAN_FALSE, single_field=GAN_FALSE, upper=GAN_FALSE, whole_image=GAN_FALSE;

   if ( !read_header ( infile, &png_ptr, &info_ptr, &iColourType, &iBitDepth, &uiHeight, &uiWidth ) )
   {
      read_cleanup(NULL, png_ptr, info_ptr);
      /* ErrorMessage set in read_header */
      gan_err_register("gan_read_png_image_stream", GAN_ERROR_FAILURE, "");
      return NULL;
   }

   /* Establish the Gandalf format and type of image needed */
   if(!decode_header_info(iColourType, iBitDepth, &format, &type))
   {
      read_cleanup(NULL, png_ptr, info_ptr);
      /* ErrorMessage set in decode_header_info */
      gan_err_register("gan_read_png_image_stream", GAN_ERROR_FAILURE, "");
      return NULL;
   }
   
   /* setjmp() call */
   if (setjmp(png_jmpbuf(png_ptr)))
   {
      png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
      gan_err_flush_trace();
      gan_err_register ( "gan_read_png_image_stream", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* Transformations */
   /* Expand palette images to RGB */
   if (iColourType == PNG_COLOR_TYPE_PALETTE)
      png_set_expand(png_ptr);
   
   /* Expand 2,4 and 6 bit grey level to 8 bits */
   if (iColourType == PNG_COLOR_TYPE_GRAY &&
       (1 < iBitDepth) && (iBitDepth < 8))
      png_set_expand(png_ptr);

   /* Expand transparency bits to full alpha channel;*/
   if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
      png_set_expand(png_ptr);

   /* Get gamma from file. If present use it together with the supplied display gamma */
   if (header != NULL && png_get_gAMA(png_ptr, info_ptr, &file_gamma))
      header->info.png.gamma = (float)file_gamma;

#ifndef WORDS_BIGENDIAN
   /* swap bytes if little endian architecture */
   if(iBitDepth == 16)
   {
      png_set_swap(png_ptr);
   }
   else
      if (iBitDepth == 1)
      {
         png_set_packswap(png_ptr);
      }
#endif

   /* Update info_ptr data */
   png_read_update_info(png_ptr, info_ptr);

   /* read elements of control structure if one was provided */
   if(ictrlstr != NULL)
   {
      flip         = ictrlstr->flip;
      single_field = ictrlstr->single_field;
      upper        = ictrlstr->upper;
      if(single_field) /* whole_image only relevant for field-based images */
         whole_image  = ictrlstr->whole_image;
   }

   /* interlaced frames must have even height */
   if(single_field)
   {
      if((uiHeight % 2) != 0)
      {
         read_cleanup(NULL, png_ptr, info_ptr);
         gan_err_flush_trace();
         gan_err_register ( "gan_read_png_image_stream", GAN_ERROR_INCOMPATIBLE, "" );
         return NULL;
      }

      uiInternalHeight = uiHeight/2;
   }
   else
      uiInternalHeight = uiHeight;
      
   if(header != NULL)
   {
      header->file_format = GAN_PNG_FORMAT;
      header->width = uiWidth;
      header->height = whole_image ? uiHeight : uiInternalHeight;
      header->format = format;
      header->type = type;
   }

   if(ictrlstr != NULL && ictrlstr->header_only)
   {
      read_cleanup(NULL, png_ptr, info_ptr);
      return (Gan_Image*)-1; /* special value */
   }

   if(image == NULL)
   {
      /* Allocate memory for the new image */
      image = gan_image_alloc(format, type, whole_image ? uiHeight : uiInternalHeight, (unsigned long) uiWidth);
      if(image == NULL)
      {
         read_cleanup(image, NULL, NULL);
         gan_err_register ( "gan_read_png_image_stream", GAN_ERROR_FAILURE, "" );
         return NULL;
      }
   }
   else
   {
      /*Use an already allocated image struct*/
      if ( gan_image_set_format_type_dims(image, format, type, whole_image ? uiHeight : uiInternalHeight, uiWidth) == NULL )
      {
         read_cleanup(image, NULL, NULL);
         gan_err_register("gan_read_png_image_stream",GAN_ERROR_FAILURE, "");
         return NULL;
      }
   }

   /* fill binary image with zeros in case PNG and Gandalf differ in the word size used to store binary images */
   if ( image->format == GAN_GREY_LEVEL_IMAGE && image->type == GAN_BOOL )
      gan_image_fill_const_b ( image, GAN_FALSE );

   /* Read the image */
   if(single_field)
   {
      unsigned int uiRow;
      Gan_Image *imbuf;

      /* allocate a one row image */
      imbuf = gan_image_alloc(format, type, 1, (unsigned long) uiWidth);
      if(imbuf == NULL)
      {
         read_cleanup(NULL, png_ptr, info_ptr);
         gan_err_register("gan_read_png_image_stream",GAN_ERROR_FAILURE, "");
         return NULL;
      }

      for(uiRow=0; uiRow<uiHeight; uiRow++)
      {
         /* only transfer even rows for upper field, or odd rows for upper field */
         if((upper && (uiRow % 2) == 0) || (!upper && (uiRow % 2) == 1))
            png_read_row(png_ptr, ((png_bytepp)image->row_data_ptr)[whole_image ? (flip ? (uiHeight-uiRow-1) : uiRow) : (flip ? (uiInternalHeight-uiRow/2-1) : uiRow/2)], NULL);
         else
            png_read_row(png_ptr, ((png_bytepp)imbuf->row_data_ptr)[0], NULL);
      }

      gan_image_free(imbuf);
   }
   else
   {
      if(flip)
      {
         /* flip the image */
         int row;

         for(row=(int)uiHeight-1; row>=0; row--)
            png_read_row(png_ptr, ((png_bytepp)image->row_data_ptr)[row], NULL);
      }
      else
         png_read_image(png_ptr, (png_bytepp)image->row_data_ptr);
   }
   
   png_read_end(png_ptr, NULL);
   
   /* Cleanup */
   read_cleanup(NULL, png_ptr, info_ptr);

   return image;
}

/**
 * \brief Reads an image file in PNG format.
 * \param filename The name of the image file
 * \param image The image structure to read the image data into or NULL
 * \param ictrlstr Pointer to structure controlling input or \c NULL
 * \param header Pointer to file header structure to be filled, or \c NULL
 * \return Pointer to image structure, or \c NULL on failure.
 *
 * Reads the PNG image stored in the file \a filename into the given \a image.
 * If \a image is passed as \c NULL a new image is dynamically allocated;
 * otherwise the already allocated \a image structure is reused. \a gam
 * identifies the gamma correction which the opened image should have.
 * It should be one if no gamma correction needs to be applied.
 *
 * \sa gan_write_png_image().
 */
Gan_Image *
 gan_read_png_image ( const char *filename, Gan_Image *image, const struct Gan_ImageReadControlStruct *ictrlstr, struct Gan_ImageHeaderStruct *header )
{
   FILE *infile;

   /* attempt to open file */
   infile = fopen ( filename, "rb" );
   if ( infile == NULL )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_read_png_image", GAN_ERROR_OPENING_FILE, filename );
      return NULL;
   }
 
   image = gan_read_png_image_stream ( infile, image, ictrlstr, header );
   fclose(infile);
   if ( image == NULL )
   {
      gan_err_register ( "gan_read_png_image", GAN_ERROR_FAILURE, filename );
      return NULL;
   }

   return image;
}

static void write_cleanup ( png_structp *p_png_ptr, png_infop *p_info_ptr )
{
   if(p_png_ptr || p_info_ptr)
      png_destroy_write_struct(p_png_ptr,  p_info_ptr);
}

/**
 * \brief Initialises the write control structure for PNG files
 *
 * This function should be called on the structure to set the parameters to default values.
 * Then set any non-default values yourself before calling gan_write_png_image() or
 * gan_write_png_image_stream().
 */
void gan_initialise_png_header_struct(Gan_PNGHeaderStruct *octrlstr)
{
   octrlstr->gamma = 1.0F;
   octrlstr->compression_level = 0;
}

/**
 * \brief Writes an image file to a stream in PNG format.
 * \param outfile The file stream
 * \param image The image structure to write to the file
 * \param new_file Whether the file does not currently exist
 * \param octrlstr Pointer to structure controlling PNG output or \c NULL
 * \return #GAN_TRUE on successful write operation, #GAN_FALSE on failure.
 *
 * Writes the given \a image into a PNG file using a file stream \a outfile.
 * If \a gam is not one a gamma correction of \a gam is applied.
 */
Gan_Bool
 gan_write_png_image_stream(FILE *outfile, const Gan_Image *image, Gan_Bool new_file, const struct Gan_ImageWriteControlStruct *octrlstr)
{
   png_structp png_ptr;
   png_infop info_ptr;
   int iColourType, iBitDepth;
   Gan_Bool has_alpha;
   int error_code = 0;
   Gan_Bool flip=GAN_FALSE, single_field=GAN_FALSE, upper=GAN_FALSE, whole_image=GAN_FALSE;
   unsigned long height = image->height;

   /* read generic control parameters */
   if(octrlstr != NULL)
   {
      flip = octrlstr->flip;
      flip         = octrlstr->flip;
      single_field = octrlstr->single_field;
      upper        = octrlstr->upper;
      if(single_field) /* whole_image only relevant for field-based images */
         whole_image = octrlstr->whole_image;
   }

   if(single_field && !whole_image) height *= 2;

   if(outfile != NULL)
   {
      /* Create the write structure */
      png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

      if (png_ptr == NULL)
      {
         gan_err_flush_trace();
         gan_err_register ( "gan_write_png_image",  GAN_ERROR_EXTERNAL_LIBRARY_CALL, "" );
         write_cleanup(NULL, NULL);
         return GAN_FALSE;
      }

      /* Allocate/initialize the image information data.*/
      info_ptr = png_create_info_struct(png_ptr);
      if (info_ptr == NULL)
      {
         gan_err_flush_trace();
         gan_err_register ( "gan_write_png_image", GAN_ERROR_EXTERNAL_LIBRARY_CALL, "" );
         write_cleanup(&png_ptr, NULL);
         return GAN_FALSE;
      }

      /* Set error handling.*/
      if (setjmp(png_jmpbuf(png_ptr)))
      {
         gan_err_flush_trace();
         gan_err_register ( "gan_write_png_image", GAN_ERROR_EXTERNAL_LIBRARY_CALL, "" );
         write_cleanup(&png_ptr, &info_ptr);
         return GAN_FALSE;
      }

      /* Initialise the IO streams */
      png_init_io(png_ptr, outfile);
   }

   if(!encode_header_info(image, &iColourType, &iBitDepth, &has_alpha, &error_code))
   {
      if(outfile != NULL)
      {
         gan_err_register ( "gan_write_png_image", error_code, "" );
         write_cleanup(&png_ptr, &info_ptr);
      }

      return GAN_FALSE;
   }

   if(outfile != NULL)
   {
      /* Set the image information */
      png_set_IHDR ( png_ptr, info_ptr, image->width, height, iBitDepth,
                     iColourType, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE,
                     PNG_FILTER_TYPE_BASE );

      /* If the gamma correction applied when reading the image was not 1.0 */
      if ( octrlstr != NULL )
         png_set_gAMA(png_ptr, info_ptr, (double)octrlstr->info.png.gamma);

      /* Write the file header information */
      png_write_info(png_ptr, info_ptr);

      /* pack pixels into bytes if not a boolean image */
      if( 1 < iBitDepth && iBitDepth < 8)
         png_set_packing(png_ptr);

#ifndef WORDS_BIGENDIAN
      /* swap bytes if little endian architecture */
      if(iBitDepth == 16)
         png_set_swap(png_ptr);
      else
         if (iBitDepth == 1)
            png_set_packswap(png_ptr);
#endif

      png_set_compression_mem_level(png_ptr, 9);
      if (octrlstr != NULL && octrlstr->info.png.compression_level>0)
         png_set_compression_level(png_ptr, gan_min2(octrlstr->info.png.compression_level, 9));
      else
         png_set_compression_strategy(png_ptr, Z_HUFFMAN_ONLY);

      /* Write out the image */
      if ( !flip && !single_field && image->type != GAN_BOOL && image->stride == image->width*gan_image_pixel_size(image->format, image->type) )
         /* we can write it in one go */
         png_write_image(png_ptr, (png_bytepp) image->row_data_ptr);
      else
      {
         char* achBlankRow=NULL;
         unsigned long r;

         if(single_field)
         {
            unsigned int row_size;

            /* create blank row */
            if(image->type == GAN_BOOL)
               row_size = GAN_NO_BITWORDS(image->width)*sizeof(Gan_BitWord);
            else
               row_size = image->width*gan_image_pixel_size(image->format, image->type);

            achBlankRow = malloc(row_size);
            if(achBlankRow == NULL)
            {
               gan_err_flush_trace();
               gan_err_register_with_number ( "gan_write_png_image_stream", GAN_ERROR_MALLOC_FAILED, "", row_size );
               return GAN_FALSE;
            }

            memset(achBlankRow, 0, row_size);
         }

         /* write one line of image data at a time */
         for ( r=0; r < height; r++ )
         {
            /* ignore row if it's in a field we're not writing */
            if(single_field && ((upper && ((r % 2) == 1)) || (!upper && ((r % 2) == 0))))
               png_write_row(png_ptr, (png_bytep)achBlankRow);
            else
            {
               unsigned long rp;

               if(single_field)
               {
                  if(whole_image)
                     rp = (flip ? (image->height-r-1) : r);
                  else
                     rp = (flip ? (image->height-r/2-1) : r/2);
               }
               else
                  rp = (flip ? (image->height-r-1) : r);

               png_write_row(png_ptr, gan_image_get_pixptr(image, rp, 0));
            }
         }

         if(achBlankRow != NULL)
            free(achBlankRow);
      }

      png_write_end(png_ptr, info_ptr);

      /* clean up after the write, and free any memory allocated */
      write_cleanup(&png_ptr, &info_ptr);
   }

   /* that's it */
   return GAN_TRUE;
}

/**
 * \brief Writes an image file in PNG format.
 * \param filename The name of the image file
 * \param image The image structure to write to the file
 * \param octrlstr Pointer to structure controlling PNG output or \c NULL
 * \return #GAN_TRUE on successful write operation, #GAN_FALSE on failure.
 *
 * Writes the given \a image into PNG file \a filename. If \a gam is not one
 * gamma correction of \a gam is applied.
 *
 * \sa gan_read_png_image().
 */
Gan_Bool
 gan_write_png_image ( const char *filename, const Gan_Image *image, const struct Gan_ImageWriteControlStruct *octrlstr )
{
   if(filename == NULL)
      /* check format works OK */
      return gan_write_png_image_stream ( NULL, image, GAN_TRUE, octrlstr );
   else
   {
      FILE *outfile;
      Gan_Bool result=GAN_FALSE;

      if(octrlstr != NULL && octrlstr->single_field)
      {
         Gan_Bool merged=GAN_FALSE;

         /* check if the file exists */
         outfile = fopen ( filename, "r" );
         if(outfile != NULL)
         {
            /* read temporary image */
            Gan_ImageHeaderStruct header;

            Gan_Image* pimage = gan_read_png_image_stream ( outfile, NULL, NULL, &header );
            fclose(outfile);

            if(pimage != NULL)
            {
               unsigned int height = (octrlstr->whole_image ? image->height : (2*image->height));

               if(pimage->width == image->width && pimage->height == height && pimage->format == image->format && pimage->type == image->type)
               {
                  Gan_ImageWriteControlStruct poctrlstr;

                  /* merge images */
                  gan_merge_field_into_image(image, octrlstr->flip, octrlstr->upper, octrlstr->whole_image, pimage);

                  /* attempt to open file */
                  outfile = fopen ( filename, "wb" );
                  if ( outfile == NULL )
                  {
                     gan_err_flush_trace();
                     gan_err_register ( "gan_write_png_image", GAN_ERROR_OPENING_FILE, filename );
                     return GAN_FALSE;
                  }

                  gan_initialise_image_write_control_struct(&poctrlstr, GAN_PNG_FORMAT, pimage->format, pimage->type);
                  poctrlstr.info.png = octrlstr->info.png;
                  result = gan_write_png_image_stream ( outfile, pimage, GAN_TRUE, &poctrlstr );
                  fclose(outfile);
                  merged = GAN_TRUE;
               }

               gan_image_free(pimage);
            }
         }

         if(merged) return result;
      }

      /* attempt to open file */
      outfile = fopen ( filename, "wb" );
      if ( outfile == NULL )
      {
         gan_err_flush_trace();
         gan_err_register ( "gan_write_png_image", GAN_ERROR_OPENING_FILE, filename );
         return GAN_FALSE;
      }

      result = gan_write_png_image_stream ( outfile, image, GAN_TRUE, octrlstr );
      fclose(outfile);
      return result;
   }
}

/**
 * \}
 */

/**
 * \}
 */

#endif /* #ifdef HAVE_PNG */

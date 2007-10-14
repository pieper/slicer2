/**
 * File:          $RCSfile: jpeg_io.c,v $
 * Module:        JPEG format image file I/O functions
 * Part of:       Gandalf Library
 *
 * Revision:      $Revision: 1.1.2.1 $
 * Last edited:   $Date: 2007/10/14 02:20:29 $
 * Author:        $Author: ruetz $
 * Copyright:     (c) 2000 Imagineer Software Limited
 *
 * Notes:         Works with the libjpeg library
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

#include <stdlib.h>
#include <stdio.h>
#include <gandalf/image/io/jpeg_io.h>
#include <gandalf/image/io/image_io.h>
#include <gandalf/image/image_bit.h>
#include <gandalf/image/image_gl_uint8.h>
#include <gandalf/image/image_rgb_uint8.h>
#include <gandalf/image/image_extract.h>
#include <gandalf/common/misc_error.h>

/* only compile if you have JPEG */
#ifdef HAVE_JPEG

#include <jpeglib.h>

#if BITS_IN_JSAMPLE != 8
#error Only 8-bit JPEG supported by Gandalf
#endif

#include <setjmp.h>
#include <string.h>

/**
 * \addtogroup ImagePackage
 * \{
 */

/**
 * \addtogroup ImageIO
 * \{
 */

struct my_error_mgr
{
   struct jpeg_error_mgr pub;
   jmp_buf setjmp_buffer;
};

typedef struct my_error_mgr *my_error_ptr;

METHODDEF(void)
my_error_exit (j_common_ptr cinfo)
{
   /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
   my_error_ptr myerr = (my_error_ptr) cinfo->err;

   /* Always display the message. */
   /* We could postpone this until after returning, if we chose. */
   (*cinfo->err->output_message) (cinfo);

   /* Return control to the setjmp point */
   longjmp(myerr->setjmp_buffer, 1);
}

/**
 * \brief Determines whether an image is a JPEG image using the magic number
 */
Gan_Bool gan_image_is_jpeg(const unsigned char *magic_string, size_t length)
{
   if (length < 3)
      return(GAN_FALSE);

   if (memcmp(magic_string,"\377\330\377",3) == 0)
      return(GAN_TRUE);

   return(GAN_FALSE);
}

/**
 * \brief Reads an image file in JPEG format from a file stream
 * \param infile The file stream to be read
 * \param image The image structure to read the image data into or \c NULL
 * \param ictrlstr Pointer to structure controlling input or \c NULL
 * \param header Pointer to file header structure to be filled, or \c NULL
 * \return Pointer to image structure, or \c NULL on failure.
 *
 * Reads the JPEG image from the file stream \a infile into the given \a image.
 * If \a image is passed as \c NULL a new image is dynamically allocated;
 * otherwise the already allocated \a image structure is reused.
 *
 * \sa gan_write_jpeg_image_stream().
 */
Gan_Image *
 gan_read_jpeg_image_stream(FILE *infile, Gan_Image *image,
                            const struct Gan_ImageReadControlStruct *ictrlstr, struct Gan_ImageHeaderStruct *header )
{
   struct jpeg_decompress_struct cinfo;
   struct my_error_mgr jerr;
   Gan_ImageFormat format;
   JSAMPROW rowptr[1];
   Gan_Bool flip=GAN_FALSE, single_field=GAN_FALSE, upper=GAN_FALSE, whole_image=GAN_FALSE;
   unsigned int uiInternalHeight;
   
   cinfo.err = jpeg_std_error(&jerr.pub);
   jerr.pub.error_exit = my_error_exit;

   /* Establish the setjmp return context for my_error_exit to use. */
   if (setjmp(jerr.setjmp_buffer))
   {
      /* If we get here, the JPEG code has signaled an error.
       * We need to clean up the JPEG object, close the input file, and return.
       */
      jpeg_destroy_decompress(&cinfo);
      gan_err_flush_trace();
      gan_err_register ( "gan_read_jpeg_image_stream", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   jpeg_create_decompress(&cinfo);
   jpeg_stdio_src(&cinfo, infile);
   (void) jpeg_read_header(&cinfo, TRUE);

   if ( cinfo.jpeg_color_space == JCS_GRAYSCALE )
   {
      format = GAN_GREY_LEVEL_IMAGE;
      cinfo.out_color_space = JCS_GRAYSCALE;
   }
   else
   {
      format = GAN_RGB_COLOUR_IMAGE;
      cinfo.out_color_space = JCS_RGB;
   }

   jpeg_calc_output_dimensions(&cinfo);

   if ( cinfo.output_components != 1 && cinfo.output_components != 3 )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_read_jpeg_image_stream", GAN_ERROR_INCOMPATIBLE, "" );
      jpeg_destroy_decompress(&cinfo);
      return NULL;
   }

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
      if((cinfo.output_height % 2) != 0)
      {
         gan_err_flush_trace();
         gan_err_register ( "gan_read_jpeg_image_stream", GAN_ERROR_INCOMPATIBLE, "" );
         return NULL;
      }

      uiInternalHeight = (unsigned int) cinfo.output_height/2;
   }
   else
      uiInternalHeight = (unsigned int) cinfo.output_height;

   if(header != NULL)
   {
      header->file_format = GAN_JPEG_FORMAT;
      header->width = (unsigned int)cinfo.output_width;
      header->height = (unsigned int)(whole_image ? ((unsigned int) cinfo.output_height) : uiInternalHeight);
      header->format = format;
      header->type = GAN_UINT8;
   }

   if(ictrlstr != NULL && ictrlstr->header_only)
   {
      jpeg_destroy_decompress(&cinfo);
      return (Gan_Image*)-1; /* special value */
   }

   if(image == NULL)
   {
      /* Allocate memory for the new image */
      image = gan_image_alloc ( format, GAN_UINT8, whole_image ? ((unsigned int) cinfo.output_height) : uiInternalHeight, (unsigned long) cinfo.output_width );
      if ( image == NULL )
      {
         gan_err_register ( "gan_read_jpeg_image_stream", GAN_ERROR_FAILURE, "" );
         jpeg_destroy_decompress(&cinfo);
         return NULL;
      }
   }
   else
   {
      /*Use an already allocated image struct*/
      if ( gan_image_set_format_type_dims ( image, format, GAN_UINT8, whole_image ? ((unsigned int) cinfo.output_height) : uiInternalHeight, (unsigned long) cinfo.output_width ) == NULL )
      {
         gan_err_register ( "gan_read_jpeg_image_stream", GAN_ERROR_FAILURE, "" );
         jpeg_destroy_decompress(&cinfo);
         return NULL;
      }
   }

   jpeg_start_decompress(&cinfo);

   if(ictrlstr != NULL && ictrlstr->flip)
      flip = GAN_TRUE;

   switch ( image->format )
   {
      case GAN_GREY_LEVEL_IMAGE:
        if(single_field)
        {
           gan_uint8 *aui8Buf = gan_malloc_array(gan_uint8, (size_t)cinfo.output_width);

           if(aui8Buf == NULL)
           {
              gan_err_flush_trace();
              gan_err_register_with_number ( "gan_read_jpeg_image_stream", GAN_ERROR_MALLOC_FAILED, "", (size_t)cinfo.output_width*sizeof(gan_uint8) );
              jpeg_finish_decompress(&cinfo);
              jpeg_destroy_decompress(&cinfo);
              return NULL;
           }

           while ( cinfo.output_scanline < cinfo.output_height )
           {
              /* only transfer even rows for upper field, or odd rows for upper field */
              if((upper && ((unsigned int)cinfo.output_scanline % 2) == 0) || (!upper && ((unsigned int)cinfo.output_scanline % 2) == 1))
                 rowptr[0] = (JSAMPROW)gan_image_get_pixptr_gl_ui8(image, whole_image ? (flip ? (cinfo.output_height-cinfo.output_scanline-1) : cinfo.output_scanline) : (flip ? (uiInternalHeight-(unsigned int)cinfo.output_scanline/2-1) : (unsigned int)cinfo.output_scanline/2), 0);
              else
                 rowptr[0] = (JSAMPROW)aui8Buf;

              (void) jpeg_read_scanlines ( &cinfo, rowptr, (JDIMENSION)1);
           }

           free(aui8Buf);
        }
        else
        {
           while ( cinfo.output_scanline < cinfo.output_height )
           {
              rowptr[0] = (JSAMPROW)gan_image_get_pixptr_gl_ui8(image, flip ? (cinfo.output_height-cinfo.output_scanline-1) : cinfo.output_scanline, 0);
              (void) jpeg_read_scanlines ( &cinfo, rowptr, (JDIMENSION)1);
           }
        }

        break;
        
      case GAN_RGB_COLOUR_IMAGE:
        if(single_field)
        {
           Gan_RGBPixel_ui8 *argbui8Buf = gan_malloc_array(Gan_RGBPixel_ui8, (size_t)cinfo.output_width);

           if(argbui8Buf == NULL)
           {
              gan_err_flush_trace();
              gan_err_register_with_number ( "gan_read_jpeg_image_stream", GAN_ERROR_MALLOC_FAILED, "", (size_t)cinfo.output_width*sizeof(Gan_RGBPixel_ui8) );
              jpeg_finish_decompress(&cinfo);
              jpeg_destroy_decompress(&cinfo);
              return NULL;
           }

           while ( cinfo.output_scanline < cinfo.output_height )
           {
              /* only transfer even rows for upper field, or odd rows for upper field */
              if((upper && ((unsigned int)cinfo.output_scanline % 2) == 0) || (!upper && ((unsigned int)cinfo.output_scanline % 2) == 1))
                 rowptr[0] = (JSAMPROW)gan_image_get_pixptr_rgb_ui8(image, whole_image ? (flip ? (cinfo.output_height-cinfo.output_scanline-1) : cinfo.output_scanline) : (flip ? (uiInternalHeight-(unsigned int)cinfo.output_scanline/2-1) : (unsigned int)cinfo.output_scanline/2), 0);
              else
                 rowptr[0] = (JSAMPROW)argbui8Buf;

              (void) jpeg_read_scanlines ( &cinfo, rowptr, (JDIMENSION)1);
           }

           free(argbui8Buf);
        }
        else
        {
           while ( cinfo.output_scanline < cinfo.output_height )
           {
              rowptr[0] = (JSAMPROW)gan_image_get_pixptr_rgb_ui8(image, flip ? (cinfo.output_height-cinfo.output_scanline-1) : cinfo.output_scanline, 0);
              (void) jpeg_read_scanlines ( &cinfo, rowptr, (JDIMENSION)1 );
           }
        }

        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_read_jpeg_image_stream", GAN_ERROR_ILLEGAL_IMAGE_FORMAT,  "" );
        jpeg_finish_decompress(&cinfo);
        jpeg_destroy_decompress(&cinfo);
        return NULL;
   }

   jpeg_finish_decompress(&cinfo);
   jpeg_destroy_decompress(&cinfo);
   return image;
}

/**
 * \brief Reads an image file in JPEG format.
 * \param filename The name of the image file
 * \param image The image structure to read the image data into or NULL
 * \param ictrlstr Pointer to structure controlling input or \c NULL
 * \param header Pointer to file header structure to be filled, or \c NULL
 * \return Pointer to image structure, or \c NULL on failure.
 *
 * Reads the JPEG image stored in the file \a filename into the given \a image.
 * If \a image is \c NULL a new image is dynamically allocated; otherwise the
 * already allocated image structure is reused.
 *
 * \sa gan_write_jpeg_image().
 */
Gan_Image *
 gan_read_jpeg_image ( const char *filename, Gan_Image *image,
                       const struct Gan_ImageReadControlStruct *ictrlstr, struct Gan_ImageHeaderStruct *header )
{
   FILE *infile;
   Gan_Image *result;

   /* attempt to open file */
   infile = fopen ( filename, "rb" );
   if ( infile == NULL )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_read_jpeg_image", GAN_ERROR_OPENING_FILE, filename );
      return NULL;
   }

   result = gan_read_jpeg_image_stream ( infile, image, ictrlstr, header );
   fclose(infile);
   return result;
}

/**
 * \brief Initialises the write control structure for JPEG files
 *
 * This function should be called on the structure to set the parameters to default values.
 * Then set any non-default values yourself before calling gan_write_jpeg_image() or
 * gan_write_jpeg_image_stream().
 */
void gan_initialise_jpeg_header_struct(Gan_JPEGHeaderStruct *octrlstr)
{
   octrlstr->quality = 75;
}

/**
 * \brief Writes an image file to a stream in JPEG format.
 * \param outfile The file stream
 * \param image The image structure to write to the file
 * \param new_file Whether the file does not currently exist
 * \param octrlstr Pointer to structure controlling output or \c NULL
 * \return #GAN_TRUE on successful write operation, #GAN_FALSE on failure.
 *
 * Writes the given \a image into a JPEG file using a file stream \a outfile.
 *
 * \sa gan_read_jpeg_image_stream().
 */
Gan_Bool
 gan_write_jpeg_image_stream(FILE *outfile, const Gan_Image *image, Gan_Bool new_file, const struct Gan_ImageWriteControlStruct *octrlstr)
{
   Gan_Image *scanline=NULL;
   struct jpeg_compress_struct cinfo;
   struct jpeg_error_mgr error_mgr;
   JSAMPROW rowptr[1];
   Gan_Bool flip = GAN_FALSE;

   /* read generic control parameters */
   if(octrlstr != NULL)
   {
      flip = octrlstr->flip;
      if(octrlstr->single_field)
      {
         gan_err_flush_trace();
         gan_err_register("gan_write_jpeg_image_stream", GAN_ERROR_INCOMPATIBLE, "" );
      }
   }

   if(outfile != NULL)
   {
      cinfo.err = jpeg_std_error(&error_mgr);
      jpeg_create_compress ( &cinfo );
      jpeg_stdio_dest ( &cinfo, outfile );
   }

   /* set JPEG image format */
   switch ( image->format )
   {
      case GAN_GREY_LEVEL_IMAGE:
        if(outfile != NULL)
        {
           cinfo.input_components = 1;
           cinfo.in_color_space = JCS_GRAYSCALE;
        }

        break;

      case GAN_RGB_COLOUR_IMAGE:
        if(outfile != NULL)
        {
           cinfo.input_components = 3;
           cinfo.in_color_space = JCS_RGB;
        }

        break;

      default:
        if(outfile != NULL)
        {
           gan_err_flush_trace();
           gan_err_register ( "gan_write_jpeg_image_stream", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
        }

        if(outfile != NULL)
        {
           jpeg_finish_compress(&cinfo);
           jpeg_destroy_compress(&cinfo);
        }

        return GAN_FALSE;
   }

   if(outfile != NULL)
   {
      /* set JPEG image dimensions */
      cinfo.image_width  = image->width;
      cinfo.image_height = image->height;

      jpeg_set_defaults ( &cinfo );
      if(octrlstr != NULL)
         jpeg_set_quality ( &cinfo, octrlstr->info.jpeg.quality, TRUE );

      jpeg_start_compress ( &cinfo, TRUE );
   }

   switch ( image->format )
   {
      case GAN_GREY_LEVEL_IMAGE:
        if(outfile != NULL)
        {
           while ( cinfo.next_scanline < cinfo.image_height )
           {
              if(image->type == GAN_UINT8)
                 rowptr[0] = (JSAMPROW)image->row_data.gl.uc[flip ? (cinfo.image_height-cinfo.next_scanline-1) : cinfo.next_scanline];
              else
              {
                 /* convert scanline to 8-bit */
                 scanline = gan_image_extract_q ( image,
                                                  flip ? (cinfo.image_height-cinfo.next_scanline-1) : cinfo.next_scanline, /* r0 */
                                                  0, /* c0 */
                                                  1, /* height */
                                                  cinfo.image_width, /* width */
                                                  image->format, GAN_UINT8,
                                                  GAN_TRUE, scanline );
                 if(scanline == NULL)
                 {
                    gan_err_register ( "gan_write_jpeg_image_stream", GAN_ERROR_FAILURE, "" );
                    jpeg_finish_compress(&cinfo);
                    jpeg_destroy_compress(&cinfo);
                    return GAN_FALSE;
                 }

                 rowptr[0] = (JSAMPROW)scanline->row_data.gl.uc[0];
              }
           
              (void)jpeg_write_scanlines(&cinfo, rowptr, (JDIMENSION) 1);
           }
        }

        break;

      case GAN_RGB_COLOUR_IMAGE:
        if(outfile != NULL)
        {
           while ( cinfo.next_scanline < cinfo.image_height )
           {
              if(image->type == GAN_UINT8)
                 rowptr[0] = (JSAMPROW)image->row_data.rgb.uc[flip ? (cinfo.image_height-cinfo.next_scanline-1) : cinfo.next_scanline];
              else
              {
                 /* convert scanline to 8-bit */
                 scanline = gan_image_extract_q ( image,
                                                  flip ? (cinfo.image_height-cinfo.next_scanline-1) : cinfo.next_scanline, /* r0 */
                                                  0, /* c0 */
                                                  1, /* height */
                                                  cinfo.image_width, /* width */
                                                  image->format, GAN_UINT8,
                                                  GAN_TRUE, scanline );
                 if(scanline == NULL)
                 {
                    gan_err_register ( "gan_write_jpeg_image_stream", GAN_ERROR_FAILURE, "" );
                    jpeg_finish_compress(&cinfo);
                    jpeg_destroy_compress(&cinfo);
                    return GAN_FALSE;
                 }

                 rowptr[0] = (JSAMPROW)scanline->row_data.rgb.uc[0];
              }
           
              (void)jpeg_write_scanlines(&cinfo, rowptr, (JDIMENSION) 1);
           }
        }

        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_write_jpeg_image_stream", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
        if(outfile != NULL)
        {
           if ( scanline != NULL ) gan_image_free ( scanline );
           jpeg_finish_compress(&cinfo);
           jpeg_destroy_compress(&cinfo);
        }

        return GAN_FALSE;
   }
  
   /* that's it */
   if(outfile != NULL)
   {
      if ( scanline != NULL ) gan_image_free ( scanline );
      jpeg_finish_compress(&cinfo);
      jpeg_destroy_compress(&cinfo);
   }

   return GAN_TRUE;
}

/**
 * \brief Writes an image file in JPEG format.
 * \param filename The name of the image file
 * \param image The image structure to write to the file
 * \param octrlstr Pointer to structure controlling output or \c NULL
 * \return #GAN_TRUE on successful write operation, #GAN_FALSE on failure.
 *
 * Writes the given \a image into the JPEG file \a filename.
 *
 * \sa gan_read_jpeg_image().
 */
Gan_Bool
 gan_write_jpeg_image ( const char *filename, const Gan_Image *image, const struct Gan_ImageWriteControlStruct *octrlstr )
{
   if(filename == NULL)
      return gan_write_jpeg_image_stream ( NULL, image, GAN_FALSE, octrlstr );
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

            Gan_Image* pimage = gan_read_jpeg_image_stream ( outfile, NULL, NULL, &header );
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
                     gan_err_register ( "gan_write_jpeg_image", GAN_ERROR_OPENING_FILE, filename );
                     return GAN_FALSE;
                  }

                  gan_initialise_image_write_control_struct(&poctrlstr, GAN_JPEG_FORMAT, pimage->format, pimage->type);
                  poctrlstr.info.jpeg = octrlstr->info.jpeg;
                  result = gan_write_jpeg_image_stream ( outfile, pimage, GAN_TRUE, &poctrlstr );
                  fclose(outfile);
                  merged = GAN_TRUE;
               }

               gan_image_free(pimage);
            }
         }

         return result;
      }

      /* attempt to open file */
      outfile = fopen ( filename, "wb" );
      if ( outfile == NULL )
      {
         gan_err_flush_trace();
         gan_err_register ( "gan_write_jpeg_image", GAN_ERROR_OPENING_FILE, filename );
         return GAN_FALSE;
      }

      result = gan_write_jpeg_image_stream ( outfile, image, GAN_TRUE, octrlstr );
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

#endif /* #ifdef HAVE_JPEG */

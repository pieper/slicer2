/**
 * File:          $RCSfile: ppm_io.c,v $
 * Module:        PPM format image file I/O functions
 * Part of:       Gandalf Library
 *
 * Revision:      $Revision: 1.1.2.1 $
 * Last edited:   $Date: 2007/10/14 02:20:30 $
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

#include <string.h>
#include <gandalf/image/io/ppm_io.h>
#include <gandalf/image/io/image_io.h>
#include <gandalf/image/image_rgb_uint8.h>
#include <gandalf/common/misc_error.h>

/**
 * \addtogroup ImagePackage
 * \{
 */

/**
 * \addtogroup ImageIO
 * \{
 */

static Gan_Bool read_line ( FILE *infile, char *s )
{
  char *news, *sptr;
  static char line[80]="";

  if ((strlen (line) == 0) || ((sptr = strtok (NULL, " \n\t")) == NULL))
  {
     while ((news = fgets (line, 80, infile)) != NULL)
        if ( (line[0] != '#') && ((sptr = strtok (line, " \n\t")) != NULL) )
           break;

     if (news == NULL)
     {
        gan_err_flush_trace();
        gan_err_register ( "read_line", GAN_ERROR_CORRUPTED_FILE,
                           "reading line");
        return GAN_FALSE;
     }
  }

  strcpy ( s, sptr );
  return GAN_TRUE;
}

/**
 * \brief Determines whether an image is a PPM image using the magic number
 */
Gan_Bool gan_image_is_ppm(const unsigned char *magic_string, size_t length)
{
   if (length < 2)
      return(GAN_FALSE);

   if (magic_string[0] == 'P' && magic_string[1] == '6')
      return(GAN_TRUE);

   return(GAN_FALSE);
}

/**
 * \brief Reads a RGB colour image file in PPM format from a stream.
 * \param infile The file stream to be read
 * \param image The image structure to read the image data into or \c NULL
 * \param ictrlstr Pointer to structure controlling input or \c NULL
 * \param header Pointer to file header structure to be filled, or \c NULL
 * \return Pointer to image structure, or \c NULL on failure.
 *
 * Reads the PPM image from the given file stream \a infile into the given
 * \a image. If \a image is \c NULL a new image is dynamically allocated,
 * otherwise the already allocated \a image structure is reused.
 *
 * \sa gan_write_ppm_image_stream().
 */
Gan_Image *
 gan_read_ppm_image_stream ( FILE *infile, Gan_Image *image, const struct Gan_ImageReadControlStruct *ictrlstr, struct Gan_ImageHeaderStruct *header )
{
   char s[80]="";
   int iWidth, iHeight, iInternalHeight, iRow;
   Gan_Bool result=GAN_TRUE;
   Gan_Bool flip=GAN_FALSE, single_field=GAN_FALSE, upper=GAN_FALSE, whole_image=GAN_FALSE;

   fgets ( s, 4, infile );
   if ( strcmp ( s, "P6\n" ) != 0 )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_read_ppm_image_stream", GAN_ERROR_CORRUPTED_FILE, "corrupted PPM file header (no P6)" );
      return NULL;
   }

   result = (Gan_Bool)(result & read_line ( infile, s )); iWidth  = atoi(s);
   result = (Gan_Bool)(result & read_line ( infile, s )); iHeight = atoi(s);
   result = (Gan_Bool)(result & read_line ( infile, s )); /*maxval = atoi(s);*/

   /* check whether any of the header lines was corrupted */
   if ( !result )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_read_ppm_image_stream", GAN_ERROR_CORRUPTED_FILE, "corrupted PPM file header" );
      return NULL;
   }

   /* read elements of control structure if one was provided */
   if(ictrlstr != NULL)
   {
      flip         = ictrlstr->flip;
      single_field = ictrlstr->single_field;
      upper        = ictrlstr->upper;
      if(single_field) /* whole_image only relevant for field-based images */
         whole_image = ictrlstr->whole_image;
   }

   /* interlaced frames must have even height */
   if(single_field)
   {
      if((iHeight % 2) != 0)
      {
         gan_err_flush_trace();
         gan_err_register ( "gan_read_ppm_image_stream", GAN_ERROR_INCOMPATIBLE, "" );
         return NULL;
      }

      iInternalHeight = iHeight/2;
   }
   else
      iInternalHeight = iHeight;

   if(header != NULL)
   {
      header->file_format = GAN_PPM_FORMAT;
      header->width = (unsigned int)iWidth;
      header->height = (unsigned int)(whole_image ? iHeight : iInternalHeight);
      header->format = GAN_RGB_COLOUR_IMAGE;
      header->type = GAN_UINT8;
   }

   if(ictrlstr != NULL && ictrlstr->header_only)
      return (Gan_Image*)-1; /* special value */

   if ( image == NULL )
      image = gan_image_alloc_rgb_ui8 ( whole_image ? iHeight : iInternalHeight, iWidth );
   else
      image = gan_image_set_rgb_ui8 ( image, whole_image ? iHeight : iInternalHeight, iWidth );

   if ( image == NULL )
   {
      gan_err_register ( "gan_read_ppm_image_stream", GAN_ERROR_FAILURE, "setting up image" );
      return NULL;
   }
   
   /* if the image has zero size then we have finished */
   if ( iWidth == 0 || iHeight == 0 ) return image;

   if(single_field)
   {
      for(iRow=0; iRow<iHeight; iRow++)
      {
         /* only transfer even rows for upper field, or odd rows for upper field */
         if((upper && (iRow % 2) == 0) || (!upper && (iRow % 2) == 1))
         {
            if ( fread ( gan_image_get_pixptr_rgb_ui8(image, whole_image ? (flip?(iHeight-iRow-1):iRow) : (flip?(iInternalHeight-iRow/2-1):iRow/2), 0), sizeof(Gan_RGBPixel_ui8), iWidth, infile ) != (size_t)iWidth )
            {
               gan_err_flush_trace();
               gan_err_register ( "gan_read_ppm_image_stream", GAN_ERROR_TRUNCATED_FILE, "truncated PPM file" );
               return NULL;
            }
         }
         else
            /* miss out this row */
            if(fseek(infile, iWidth*sizeof(Gan_RGBPixel_ui8), SEEK_CUR) != 0)
            {
               gan_err_flush_trace();
               gan_err_register ( "gan_read_ppm_image_stream", GAN_ERROR_TRUNCATED_FILE, "truncated PPM file" );
               return NULL;
            }
      }
   }
   else
   {
      for(iRow=0; iRow<iHeight; iRow++)
         if ( fread ( image->row_data.rgb.uc[flip ? (iHeight-iRow-1) : iRow], sizeof(Gan_RGBPixel_ui8), iWidth, infile ) != (size_t)iWidth )
         {
            gan_err_flush_trace();
            gan_err_register ( "gan_read_ppm_image_stream", GAN_ERROR_TRUNCATED_FILE, "truncated PPM file" );
            return NULL;
         }
   }

   /* success */
   return image;
}

/**
 * \brief Reads a RGB colour image file in PPM format.
 * \param filename The name of the image file
 * \param image The image structure to read the image data into or \c NULL
 * \param ictrlstr Pointer to structure controlling input or \c NULL
 * \param header Pointer to file header structure to be filled, or \c NULL
 * \return Pointer to image structure, or \c NULL on failure.
 *
 * Reads the PPM image with the in the file \a filename into the given
 * \a image. If \a image is \c NULL a new image is dynamically allocated;
 * otherwise the already allocated \a image structure is reused.
 *
 * \sa gan_write_ppm_image().
 */
Gan_Image *
 gan_read_ppm_image ( const char *filename, Gan_Image *image, const struct Gan_ImageReadControlStruct *ictrlstr, struct Gan_ImageHeaderStruct *header )
{
   FILE *infile;
   Gan_Image *result;

   /* attempt to open file */
   infile = fopen ( filename, "rb" );
   if ( infile == NULL )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_read_ppm_image", GAN_ERROR_OPENING_FILE,
                         filename );
      return NULL;
   }

   result = gan_read_ppm_image_stream ( infile, image, ictrlstr, header );
   fclose(infile);
   return result;
}

/**
 * \brief Writes a RGB colour image to a file stream in PPM format.
 * \param outfile The file stream
 * \param image The image structure to write to the file
 * \param new_file Whether the file does not currently exist
 * \param octrlstr Pointer to structure controlling output or \c NULL
 * \return #GAN_TRUE on successful write operation, or #GAN_FALSE on failure.
 *
 * Writes the \a image into the file stream \a outfile in PPM format.
 *
 * \sa gan_read_ppm_image_stream().
 */
Gan_Bool
 gan_write_ppm_image_stream ( FILE *outfile, const Gan_Image *image, Gan_Bool new_file, const struct Gan_ImageWriteControlStruct *octrlstr )
{
   Gan_Bool flip = GAN_FALSE;

   /* read generic control parameters */
   if(octrlstr != NULL)
   {
      flip = octrlstr->flip;
      if(octrlstr->single_field)
      {
         gan_err_flush_trace();
         gan_err_register("gan_write_ppm_image_stream", GAN_ERROR_INCOMPATIBLE, "" );
      }
   }

   if ( image->format != GAN_RGB_COLOUR_IMAGE || image->type != GAN_UINT8 )
   {
      if(outfile != NULL)
      {
         gan_err_flush_trace();
         gan_err_register("gan_write_ppm_image_stream", GAN_ERROR_INCOMPATIBLE, "" );
      }

      return GAN_FALSE;
   }

   if(outfile != NULL)
      fprintf ( outfile, "P6\n#Constructed by Gandalf\n%d %d\n255\n", (int)image->width, (int)image->height );

   /* if the image has zero size then we have finished */
   if ( image->width == 0 || image->height == 0 ) return GAN_TRUE;

   if(outfile != NULL)
   {
      /* write image data to stream */
      if ( !flip && image->stride == image->width*gan_image_pixel_size(image->format, image->type) )
         /* we can write it in one go */
         fwrite ( image->row_data.rgb.uc[0], sizeof(Gan_RGBPixel_ui8), image->height*image->width, outfile );
      else
      {
         /* write one line of image data at a time */
         unsigned long r;

         for ( r=0; r < image->height; r++ )
            fwrite ( image->row_data.rgb.uc[flip ? (image->height-r-1) : r], sizeof(Gan_RGBPixel_ui8), image->width, outfile );
      }
   }

   /* success */
   return GAN_TRUE;
}

/**
 * \brief Writes a RGB colour image file in PPM format.
 * \param filename The name of the image file
 * \param image The image structure to write to the file
 * \param octrlstr Pointer to structure controlling output or \c NULL
 * \return #GAN_TRUE on successful write operation, #GAN_FALSE on failure.
 *
 * Writes the \a image into PPM file \a filename.
 *
 * \sa gan_read_ppm_image().
 */
Gan_Bool
 gan_write_ppm_image ( const char *filename, const Gan_Image *image, const struct Gan_ImageWriteControlStruct *octrlstr )
{
   if(filename == NULL)
      return gan_write_ppm_image_stream ( NULL, image, GAN_FALSE, octrlstr );
   else
   {
      FILE *outfile;
      Gan_Bool new_file=GAN_TRUE, result;

      /* attempt to open file */
      if(new_file)
         outfile = fopen ( filename, "wb" );
      else
         outfile = fopen ( filename, "rb+" );

      if ( outfile == NULL )
      {
         gan_err_flush_trace();
         gan_err_register ( "gan_write_ppm_image", GAN_ERROR_OPENING_FILE, filename );
         return GAN_FALSE;
      }

      result = gan_write_ppm_image_stream ( outfile, image, new_file, octrlstr );
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

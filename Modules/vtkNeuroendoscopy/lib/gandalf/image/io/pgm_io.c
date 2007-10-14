/**
 * File:          $RCSfile: pgm_io.c,v $
 * Module:        PGM format image file I/O functions
 * Part of:       Gandalf Library
 *
 * Revision:      $Revision: 1.1.2.1 $
 * Last edited:   $Date: 2007/10/14 02:20:29 $
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
#include <gandalf/image/io/pgm_io.h>
#include <gandalf/image/io/image_io.h>
#include <gandalf/image/image_gl_uint8.h>
#include <gandalf/image/image_bit.h>
#include <gandalf/common/misc_error.h>
#include <gandalf/common/allocate.h>
#include <gandalf/common/array.h>

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
 * \brief Determines whether an image is a PGM image using the magic number
 */
Gan_Bool gan_image_is_pgm(const unsigned char *magic_string, size_t length)
{
   if (length < 2)
      return(GAN_FALSE);

   if (magic_string[0] == 'P' && magic_string[1] == '5')
      return(GAN_TRUE);

   return(GAN_FALSE);
}

/**
 * \brief Reads a grey level image file in PGM format from a stream.
 * \param infile The file stream to be read
 * \param image The image structure to read the image data into or \c NULL
 * \param ictrlstr Pointer to structure controlling input or \c NULL
 * \param header Pointer to file header structure to be filled, or \c NULL
 * \return Pointer to image structure, or \c NULL on failure.
 *
 * Reads the PGM image from the given file stream \a infile into the given
 * \a image. If \a image is \c NULL a new image is dynamically allocated,
 * otherwise the already allocated \a image structure is reused.
 *
 * \sa gan_write_pgm_image_stream().
 */
Gan_Image *
 gan_read_pgm_image_stream ( FILE *infile, Gan_Image *image, const struct Gan_ImageReadControlStruct *ictrlstr, struct Gan_ImageHeaderStruct *header )
{
   char s[80]="";
   char *signature = "P5\n";
   int iWidth, iHeight, iInternalHeight, iMaxVal, iRow;
   Gan_Bool result=GAN_TRUE;
   Gan_Bool flip=GAN_FALSE, single_field=GAN_FALSE, upper=GAN_FALSE, whole_image=GAN_FALSE;

   fgets ( s, 3, infile );
   s[2] = '\n';
   if ( strcmp ( s, signature ) != 0 )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_read_pgm_image_stream", GAN_ERROR_CORRUPTED_FILE, "corrupted PGM file header (no P5)" );

      return NULL;
   }

   result = (Gan_Bool)(result & read_line ( infile, s )); iWidth  = atoi(s);
   result = (Gan_Bool)(result & read_line ( infile, s )); iHeight = atoi(s);
   result = (Gan_Bool)(result & read_line ( infile, s )); iMaxVal = atoi(s);

   /* check whether any of the header lines was corrupted */
   if ( !result )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_read_pgm_image_stream", GAN_ERROR_CORRUPTED_FILE, "" );
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
      if((iHeight % 2) != 0)
      {
         gan_err_flush_trace();
         gan_err_register ( "gan_read_pgm_image_stream_stream", GAN_ERROR_INCOMPATIBLE, "" );
         return NULL;
      }

      iInternalHeight = iHeight/2;
   }
   else
      iInternalHeight = iHeight;

   if(header != NULL)
   {
      header->file_format = GAN_PGM_FORMAT;
      header->width = (unsigned int)iWidth;
      header->height = (unsigned int)(whole_image ? iHeight : iInternalHeight);
      header->format = GAN_GREY_LEVEL_IMAGE;
      header->type = (iMaxVal == 255) ? GAN_UINT8 : GAN_BOOL;
   }

   if(ictrlstr != NULL && ictrlstr->header_only)
      return (Gan_Image*)-1; /* special value */

   /* binary and 8-bit types supported */
   switch ( iMaxVal )
   {
      /* we require characters to be single bytes */
#if (SIZEOF_CHAR != 1)
#error "characters not represented by single bytes"
#endif      
      case 255:
        if ( image == NULL )
           image = gan_image_alloc_gl_ui8 ( whole_image ? iHeight : iInternalHeight, iWidth );
        else
           image = gan_image_set_gl_ui8 ( image, whole_image ? iHeight : iInternalHeight, iWidth );

        if ( image == NULL )
        {
           gan_err_register ( "gan_read_pgm_image_stream", GAN_ERROR_FAILURE, "" );
           return NULL;
        }
   
        /* if the image has zero size then we have finished */
        if ( iWidth == 0 || iHeight == 0 ) return image;

        if(single_field)
        {
           for ( iRow = 0; iRow < iHeight; iRow++ )
           {
              /* only transfer even rows for upper field, or odd rows for upper field */
              if((upper && (iRow % 2) == 0) || (!upper && (iRow % 2) == 1))
              {
                 if ( fread ( gan_image_get_pixptr_gl_ui8 ( image, whole_image ? (flip?(iHeight-iRow-1):iRow) : (flip?(iInternalHeight-iRow/2-1):iRow/2), 0 ), sizeof(gan_uint8), iWidth, infile )
                      != (size_t)iWidth )
                 {
                    gan_err_flush_trace();
                    gan_err_register ( "gan_read_pgm_image_stream", GAN_ERROR_TRUNCATED_FILE, "" );
                    return NULL;
                 }
              }
              else
                 /* miss out this row */
                 if(fseek(infile, iWidth, SEEK_CUR) != 0)
                 {
                    gan_err_flush_trace();
                    gan_err_register ( "gan_read_pgm_image_stream", GAN_ERROR_TRUNCATED_FILE, "truncated PPM file" );
                    return NULL;
            }
           }
        }
        else
        {
           for ( iRow = 0; iRow < iHeight; iRow++ )
              if ( fread ( gan_image_get_pixptr_gl_ui8 ( image, flip ? (iHeight-iRow-1) : iRow, 0 ), sizeof(gan_uint8), iWidth, infile )
                   != (size_t)iWidth )
              {
                 gan_err_flush_trace();
                 gan_err_register ( "gan_read_pgm_image_stream", GAN_ERROR_TRUNCATED_FILE, "" );
                 return NULL;
              }
        }

        break;

      case 1:
        /* read binary image */
        if ( image == NULL )
           image = gan_image_alloc_b ( whole_image ? iHeight : iInternalHeight, iWidth );
        else
           image = gan_image_set_b ( image, whole_image ? iHeight : iInternalHeight, iWidth );

        if ( image == NULL )
        {
           gan_err_register ( "gan_read_pgm_image_stream", GAN_ERROR_FAILURE, "" );
           return NULL;
        }
   
        /* if the image has zero size then we have finished */
        if ( iWidth == 0 || iHeight == 0 ) return image;
        else
        {
           /* create temporary array to hold bytes */
           gan_uint8 *ui8arr = gan_malloc_array ( gan_uint8, iHeight*iWidth ), *ui8ptr;
           int j;

           if ( ui8arr == NULL )
           {
              gan_err_flush_trace();
              gan_err_register_with_number ( "gan_read_pgm_image_stream", GAN_ERROR_MALLOC_FAILED, "", iHeight*iWidth*sizeof(gan_uint8) );
              return NULL;
           }

           /* read image data into temporary array */
           if ( fread ( ui8arr, 1, iHeight*iWidth, infile ) != (size_t)(iHeight*iWidth) )
           {
              gan_err_flush_trace();
              gan_err_register ( "gan_read_pgm_image_stream", GAN_ERROR_TRUNCATED_FILE, "" );
              return NULL;
           }

           /* convert data to binary format */
           gan_image_fill_zero ( image );
           if(single_field)
           {
              for ( iRow = 0, ui8ptr = ui8arr; iRow < iHeight; iRow++ )
                 /* only transfer even rows for upper field, or odd rows for upper field */
                 if((upper && (iRow % 2) == 0) || (!upper && (iRow % 2) == 1))
                 {
                    for ( j = 0; j < iWidth; j++ )
                       if ( *ui8ptr++ )
                          gan_image_set_pix_b ( image, whole_image ? (flip?(iHeight-iRow-1):iRow) : (flip?(iInternalHeight-iRow/2-1):iRow/2), j, GAN_TRUE );
                 }
           }
           else
           {
              for ( iRow = 0, ui8ptr = ui8arr; iRow < iHeight; iRow++ )
                 for ( j = 0; j < iWidth; j++ )
                    if ( *ui8ptr++ )
                       gan_image_set_pix_b ( image, flip ? (iHeight-iRow-1) : iRow, j, GAN_TRUE );
           }

           /* free temporary array */
           free(ui8arr);
        }
        
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_read_pgm_image_stream", GAN_ERROR_INCOMPATIBLE, "" );
        return NULL;
   }
   
   /* success */
   return image;
}

/**
 * \brief Reads a grey level image file in PGM format.
 * \param filename The name of the image file
 * \param image The image structure to read the image data into or \c NULL
 * \param ictrlstr Pointer to structure controlling input or \c NULL
 * \param header Pointer to file header structure to be filled, or \c NULL
 * \return Pointer to image structure, or \c NULL on failure.
 *
 * Reads the PGM image with the in the file \a filename into the given
 * \a image. If \a image is \c NULL a new image is dynamically allocated;
 * otherwise the already allocated \a image structure is reused.
 *
 * \sa gan_write_pgm_image().
 */
Gan_Image *
 gan_read_pgm_image ( const char *filename, Gan_Image *image, const struct Gan_ImageReadControlStruct *ictrlstr, struct Gan_ImageHeaderStruct *header )
{
   FILE *infile;
   Gan_Image *result;

   /* attempt to open file */
   infile = fopen ( filename, "rb" );
   if ( infile == NULL )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_read_pgm_image", GAN_ERROR_OPENING_FILE, filename );
      return NULL;
   }

   result = gan_read_pgm_image_stream ( infile, image, ictrlstr, header );
   fclose(infile);
   return result;
}

/**
 * \brief Writes a grey level image to a file stream in PGM format.
 * \param outfile The file stream
 * \param image The image structure to write to the file
 * \param new_file Whether the file does not currently exist
 * \param octrlstr Pointer to structure controlling output or \c NULL
 * \return #GAN_TRUE on successful write operation, or #GAN_FALSE on failure.
 *
 * Writes the \a image into the file stream \a outfile in PGM format.
 *
 * \sa gan_read_pgm_image_stream().
 */
Gan_Bool
 gan_write_pgm_image_stream ( FILE *outfile, const Gan_Image *image, Gan_Bool new_file, const struct Gan_ImageWriteControlStruct *octrlstr )
{
   Gan_Bool flip = GAN_FALSE;

   /* read generic control parameters */
   if(octrlstr != NULL)
   {
      flip = octrlstr->flip;
      if(octrlstr->single_field)
      {
         gan_err_flush_trace();
         gan_err_register("gan_write_pgm_image_stream", GAN_ERROR_INCOMPATIBLE, "" );
      }
   }

   if ( image->format != GAN_GREY_LEVEL_IMAGE )
   {
      if(outfile != NULL)
      {
         gan_err_flush_trace();
         gan_err_register("gan_write_pgm_image_stream", GAN_ERROR_INCOMPATIBLE, "" );
      }

      return GAN_FALSE;
   }

   switch ( image->type )
   {
      case GAN_UINT8:
        if(outfile != NULL)
           fprintf ( outfile, "P5\n#Constructed by Gandalf\n%d %d\n255\n",
                     (int)image->width, (int)image->height );

        /* if the image has zero size then we have finished */
        if ( image->width == 0 || image->height == 0 ) return GAN_TRUE;

        if(outfile != NULL)
        {
           /* write image data to stream */
           if ( image->stride == image->width*gan_image_pixel_size(image->format, image->type) )
              /* we can write it in one go */
              fwrite ( gan_image_get_pixptr_gl_ui8 ( image, 0, 0 ), sizeof(gan_uint8), image->height*image->width, outfile );
           else
           {
              /* write one line of image data at a time */
              unsigned long r;

              for ( r=0; r < image->height; r++ )
                 fwrite ( gan_image_get_pixptr_gl_ui8 ( image, r, 0 ), sizeof(gan_uint8), image->width, outfile );
           }
        }
        
        break;

      case GAN_BOOL:
      {
         if(outfile != NULL)
            fprintf ( outfile, "P5\n#Constructed by Gandalf\n%d %d\n1\n", (int)image->width, (int)image->height );

         /* if the image has zero size then we have finished */
         if ( image->width == 0 || image->height == 0 ) return GAN_TRUE;

         if(outfile != NULL)
         {
            gan_uint8 *ui8arr, *ui8ptr;
            int i, j;

            /* construct temporary array of bytes */
            ui8arr = gan_malloc_array ( gan_uint8, image->height*image->width );
            if ( ui8arr == NULL )
            {
               gan_err_flush_trace();
               gan_err_register_with_number ( "gan_write_pgm_image_stream", GAN_ERROR_MALLOC_FAILED, "", image->height*image->width*sizeof(gan_uint8) );
               return GAN_FALSE;
            }

            /* initialise array of bytes to zero */
            gan_fill_array_ui8 ( ui8arr, image->height*image->width, 1, 0 );

            /* make ucptr point to end of temporary array */
            ui8ptr = ui8arr + image->height*image->width - 1;

            /* fill temporary with ones where a pixel is set */
            for ( i = (int)image->height-1; i >= 0; i-- )
               for ( j = (int)image->width-1; j >= 0; j--, ui8ptr-- )
                  if ( gan_image_get_pix_b ( image, flip ? ((int)image->height-i-1) : i, j ) )
                     *ui8ptr = 1;
                  
            /* write image data to stream */
            fwrite ( ui8arr, 1, image->height*image->width, outfile );

            /* free temporary array */
            free(ui8arr);
         }
      }
      break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_write_pgm_image_stream", GAN_ERROR_INCOMPATIBLE, "" );
        return GAN_FALSE;
   }

   /* success */
   return GAN_TRUE;
}

/**
 * \brief Writes a grey level image file in PGM format.
 * \param filename The name of the image file
 * \param image The image structure to write to the file
 * \param octrlstr Pointer to structure controlling output or \c NULL
 * \return #GAN_TRUE on successful write operation, #GAN_FALSE on failure.
 *
 * Writes the \a image into PGM file \a filename.
 *
 * \sa gan_read_pgm_image().
 */
Gan_Bool
 gan_write_pgm_image ( const char *filename, const Gan_Image *image, const struct Gan_ImageWriteControlStruct *octrlstr )
{
   if(filename == NULL)
      return gan_write_pgm_image_stream ( NULL, image, GAN_FALSE, octrlstr );
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
         gan_err_register ( "gan_write_pgm_image", GAN_ERROR_OPENING_FILE, filename );
         return GAN_FALSE;
      }

      result = gan_write_pgm_image_stream ( outfile, image, new_file, octrlstr );
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

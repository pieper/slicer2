/**
 * File:          $RCSfile: tiff_io.c,v $
 * Module:        TIFF format image file I/O functions
 * Part of:       Gandalf Library
 *
 * Revision:      $Revision: 1.1.2.1 $
 * Last edited:   $Date: 2007/10/14 02:20:30 $
 * Author:        $Author: ruetz $
 * Copyright:     (c) 2000 Imagineer Software Limited
 *
 * Notes:         Works with the libtiff library
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

#include <gandalf/image/io/tiff_io.h>
#include <gandalf/image/io/image_io.h>
#include <gandalf/image/image_bit.h>
#include <gandalf/image/image_gl_uint8.h>
#include <gandalf/image/image_gl_uint16.h>
#include <gandalf/image/image_gl_float32.h>
#include <gandalf/image/image_gla_uint8.h>
#include <gandalf/image/image_gla_uint16.h>
#include <gandalf/image/image_gla_float32.h>
#include <gandalf/image/image_rgb_uint8.h>
#include <gandalf/image/image_rgb_uint16.h>
#include <gandalf/image/image_rgb_float32.h>
#include <gandalf/image/image_rgba_uint8.h>
#include <gandalf/image/image_rgba_uint16.h>
#include <gandalf/image/image_rgba_float32.h>
#include <gandalf/common/array.h>
#include <gandalf/common/misc_error.h>
#include <string.h>

/* only compile if you have TIFF */
#ifdef HAVE_TIFF

#include <tiffio.h>

/**
 * \addtogroup ImagePackage
 * \{
 */

/**
 * \addtogroup ImageIO
 * \{
 */

/* fill given sample in each pixel on row */
static Gan_Bool
 fill_row_samples ( Gan_Image *pImage, unsigned uiRow, uint16 s, tdata_t buf )
{
   int iCol;

   gan_err_test_bool ( uiRow < pImage->height, "fill_row_samples", GAN_ERROR_INCOMPATIBLE, "" );

   switch ( pImage->format )
   {
      case GAN_RGB_COLOUR_IMAGE:
        switch ( pImage->type )
        {
           case GAN_UINT8:
           {
              Gan_RGBPixel_ui8 *pixptr = gan_image_get_pixptr_rgb_ui8(pImage, uiRow, 0);
              uint8 *bufptr = (uint8 *)buf;

              switch ( s )
              {
                 case 0:
                   for ( iCol = (int)pImage->width-1; iCol >= 0; iCol--, pixptr++ )
                      pixptr->R = *bufptr++;

                   break;

                 case 1:
                   for ( iCol = (int)pImage->width-1; iCol >= 0; iCol--, pixptr++ )
                      pixptr->G = *bufptr++;

                   break;

                 case 2:
                   for ( iCol = (int)pImage->width-1; iCol >= 0; iCol--, pixptr++ )
                      pixptr->B = *bufptr++;

                   break;
                   
                 default:
                   gan_err_flush_trace();
                   gan_err_register ( "fill_row_samples", GAN_ERROR_ILLEGAL_TYPE, "" );
                   return GAN_FALSE;
              }
           }
           break;
             
           case GAN_UINT16:
           {
              Gan_RGBPixel_ui16 *pixptr = gan_image_get_pixptr_rgb_ui16(pImage, uiRow, 0);
              uint16 *bufptr = (uint16 *)buf;

              switch ( s )
              {
                 case 0:
                   for ( iCol = (int)pImage->width-1; iCol >= 0; iCol--, pixptr++ )
                      pixptr->R = *bufptr++;

                   break;

                 case 1:
                   for ( iCol = (int)pImage->width-1; iCol >= 0; iCol--, pixptr++ )
                      pixptr->G = *bufptr++;

                   break;

                 case 2:
                   for ( iCol = (int)pImage->width-1; iCol >= 0; iCol--, pixptr++ )
                      pixptr->B = *bufptr++;

                   break;
                   
                 default:
                   gan_err_flush_trace();
                   gan_err_register ( "fill_row_samples", GAN_ERROR_ILLEGAL_TYPE, "" );
                   return GAN_FALSE;
              }
           }
           break;
             
           case GAN_FLOAT32:
           {
              Gan_RGBPixel_f32 *pixptr = gan_image_get_pixptr_rgb_f32(pImage, uiRow, 0);
              gan_float32 *bufptr = (gan_float32 *)buf;

              switch ( s )
              {
                 case 0:
                   for ( iCol = (int)pImage->width-1; iCol >= 0; iCol--, pixptr++ )
                      pixptr->R = *bufptr++;

                   break;

                 case 1:
                   for ( iCol = (int)pImage->width-1; iCol >= 0; iCol--, pixptr++ )
                      pixptr->G = *bufptr++;

                   break;

                 case 2:
                   for ( iCol = (int)pImage->width-1; iCol >= 0; iCol--, pixptr++ )
                      pixptr->B = *bufptr++;

                   break;
                   
                 default:
                   gan_err_flush_trace();
                   gan_err_register ( "fill_row_samples", GAN_ERROR_ILLEGAL_TYPE, "" );
                   return GAN_FALSE;
              }
           }
           break;
             
           default:
             gan_err_flush_trace();
             gan_err_register ( "fill_row_samples", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             return GAN_FALSE;
        }
        break;
        
      case GAN_RGB_COLOUR_ALPHA_IMAGE:
        switch ( pImage->type )
        {
           case GAN_UINT8:
           {
              Gan_RGBAPixel_ui8 *pixptr = gan_image_get_pixptr_rgba_ui8(pImage, uiRow, 0);
              uint8 *bufptr = (uint8 *)buf;

              switch ( s )
              {
                 case 0:
                   for ( iCol = (int)pImage->width-1; iCol >= 0; iCol--, pixptr++ )
                      pixptr->R = *bufptr++;

                   break;

                 case 1:
                   for ( iCol = (int)pImage->width-1; iCol >= 0; iCol--, pixptr++ )
                      pixptr->G = *bufptr++;

                   break;

                 case 2:
                   for ( iCol = (int)pImage->width-1; iCol >= 0; iCol--, pixptr++ )
                      pixptr->B = *bufptr++;

                   break;
                   
                 case 3:
                   for ( iCol = (int)pImage->width-1; iCol >= 0; iCol--, pixptr++ )
                      pixptr->A = *bufptr++;

                   break;
                   
                 default:
                   gan_err_flush_trace();
                   gan_err_register ( "fill_row_samples", GAN_ERROR_ILLEGAL_TYPE, "" );
                   return GAN_FALSE;
              }
           }
           break;
             
           case GAN_UINT16:
           {
              Gan_RGBAPixel_ui16 *pixptr = gan_image_get_pixptr_rgba_ui16(pImage,
                                                                      uiRow,0);
              uint16 *bufptr = (uint16 *)buf;

              switch ( s )
              {
                 case 0:
                   for ( iCol = (int)pImage->width-1; iCol >= 0; iCol--, pixptr++ )
                      pixptr->R = *bufptr++;

                   break;

                 case 1:
                   for ( iCol = (int)pImage->width-1; iCol >= 0; iCol--, pixptr++ )
                      pixptr->G = *bufptr++;

                   break;

                 case 2:
                   for ( iCol = (int)pImage->width-1; iCol >= 0; iCol--, pixptr++ )
                      pixptr->B = *bufptr++;

                   break;
                   
                 case 3:
                   for ( iCol = (int)pImage->width-1; iCol >= 0; iCol--, pixptr++ )
                      pixptr->A = *bufptr++;

                   break;
                   
                 default:
                   gan_err_flush_trace();
                   gan_err_register ( "fill_row_samples", GAN_ERROR_ILLEGAL_TYPE, "" );
                   return GAN_FALSE;
              }
           }
           break;
             
           case GAN_FLOAT32:
           {
              Gan_RGBAPixel_f32 *pixptr = gan_image_get_pixptr_rgba_f32(pImage, uiRow,0);
              gan_float32 *bufptr = (gan_float32 *)buf;

              switch ( s )
              {
                 case 0:
                   for ( iCol = (int)pImage->width-1; iCol >= 0; iCol--, pixptr++ )
                      pixptr->R = *bufptr++;

                   break;

                 case 1:
                   for ( iCol = (int)pImage->width-1; iCol >= 0; iCol--, pixptr++ )
                      pixptr->G = *bufptr++;

                   break;

                 case 2:
                   for ( iCol = (int)pImage->width-1; iCol >= 0; iCol--, pixptr++ )
                      pixptr->B = *bufptr++;

                   break;
                   
                 case 3:
                   for ( iCol = (int)pImage->width-1; iCol >= 0; iCol--, pixptr++ )
                      pixptr->A = *bufptr++;

                   break;
                   
                 default:
                   gan_err_flush_trace();
                   gan_err_register ( "fill_row_samples", GAN_ERROR_ILLEGAL_TYPE, "" );
                   return GAN_FALSE;
              }
           }
           break;
             
           default:
             gan_err_flush_trace();
             gan_err_register ( "fill_row_samples", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             return GAN_FALSE;
        }
        break;
        
      default:
        gan_err_flush_trace();
        gan_err_register ( "fill_row_samples", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
        return GAN_FALSE;
   }

   /* success */
   return GAN_TRUE;
}

/**
 * \brief Determines whether an image is a TIFF image using the magic number
 */
Gan_Bool gan_image_is_tiff(const unsigned char *magic_string, size_t length)
{
   if (length < 4)
      return(GAN_FALSE);

   if (memcmp(magic_string,"\115\115\000\052",4) == 0)
      return(GAN_TRUE);

   if (memcmp(magic_string,"\111\111\052\000",4) == 0)
      return(GAN_TRUE);

   return(GAN_FALSE);
}

/**
 * \brief Reads an image file in TIFF format.
 * \param filename The name of the image file
 * \param image The image structure to read the image data into or NULL
 * \param ictrlstr Pointer to structure controlling TIFF input or \c NULL
 * \param header Pointer to file header structure to be filled, or \c NULL
 * \return Pointer to image structure, or \c NULL on ailure.
 *
 * Reads the TIFF image stored in the file \a filename into an \a image.
 * If image is \c NULL a new image is dynamically allocated, otherwise the
 * already allocated \a image structure is reused.
 *
 * \sa gan_write_tiff_image().
 */
Gan_Image *
 gan_read_tiff_image ( const char *filename, Gan_Image *image, const struct Gan_ImageReadControlStruct *ictrlstr, struct Gan_ImageHeaderStruct *header )
{
   TIFF *tif;
    uint32 ui32Width, ui32Height, ui32SampleFormat;
   unsigned int uiInternalHeight;
   unsigned uiRow;
   tsize_t scanlinesize;
   uint16 bitspersample, config;
   tsample_t nsamples;
    tdata_t buf;
   Gan_ImageFormat format;
   Gan_Type type;
   Gan_Bool flip=GAN_FALSE, single_field=GAN_FALSE, upper=GAN_FALSE, whole_image=GAN_FALSE;

   /* attempt to open file */
   tif = TIFFOpen(filename, "r");
   if ( tif == NULL )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_read_tiff_image", GAN_ERROR_OPENING_FILE, filename );
      return NULL;
   }

   /* read image details */
    TIFFGetField ( tif, TIFFTAG_IMAGEWIDTH,      &ui32Width );
    TIFFGetField ( tif, TIFFTAG_IMAGELENGTH,     &ui32Height );
    TIFFGetField ( tif, TIFFTAG_BITSPERSAMPLE,   &bitspersample );
    TIFFGetField ( tif, TIFFTAG_SAMPLESPERPIXEL, &nsamples );
   TIFFGetField ( tif, TIFFTAG_PLANARCONFIG,    &config );
   TIFFGetField ( tif, TIFFTAG_SAMPLEFORMAT,    &ui32SampleFormat );

   /* make sure that we will have origin at top-left */
   TIFFSetField ( tif, TIFFTAG_ORIENTATION, ORIENTATION_BOTLEFT );

   /* compute total number of bytes */
   scanlinesize = TIFFScanlineSize(tif);

   /* allocate image data */
   buf = _TIFFmalloc(scanlinesize);
   if ( buf == NULL )
   {
      TIFFClose(tif);
      gan_err_flush_trace();
      gan_err_register_with_number ( "gan_read_tiff_image_stream", GAN_ERROR_MALLOC_FAILED, "", sizeof(scanlinesize) );
      return NULL;
   }

   switch ( nsamples )
   {
      case 1: format = GAN_GREY_LEVEL_IMAGE;       break;
      case 2: format = GAN_GREY_LEVEL_ALPHA_IMAGE; break;
      case 3: format = GAN_RGB_COLOUR_IMAGE;       break;
      case 4: format = GAN_RGB_COLOUR_ALPHA_IMAGE; break;

      default:
        _TIFFfree ( buf );
        TIFFClose(tif);
        gan_err_flush_trace();
        gan_err_register ( "gan_read_tiff_image", GAN_ERROR_ILLEGAL_TYPE, "" );
        return NULL;
   }

   switch ( bitspersample )
   {
      case  8: type = GAN_UINT8;   break;
      case 16: type = GAN_UINT16;  break;
      case 32: type = GAN_FLOAT32; break;

      default:
        _TIFFfree ( buf );
        TIFFClose(tif);
        gan_err_flush_trace();
        gan_err_register ( "gan_read_tiff_image", GAN_ERROR_ILLEGAL_TYPE, "" );
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
      if((ui32Height % 2) != 0)
      {
         _TIFFfree ( buf );
         TIFFClose(tif);
         gan_err_flush_trace();
         gan_err_register ( "gan_read_tiff_image_stream", GAN_ERROR_INCOMPATIBLE, "" );
         return NULL;
      }

      uiInternalHeight = ui32Height/2;
   }
   else
      uiInternalHeight = ui32Height;
      
   if(header != NULL)
   {
      header->file_format = GAN_TIFF_FORMAT;
      header->width = ui32Width;
      header->height = whole_image ? ui32Height : uiInternalHeight;
      header->format = format;
      header->type = type;
   }

   if(ictrlstr != NULL && ictrlstr->header_only)
   {
      _TIFFfree ( buf );
      TIFFClose(tif);
      return (Gan_Image*)-1; /* special value */
   }

   if ( image == NULL )
      /* allocate memory for the new image */
      image = gan_image_alloc ( format, type, whole_image ? ui32Height : uiInternalHeight, (unsigned long) ui32Width );
   else
      /* use an already allocated image */
      image = gan_image_set_format_type_dims ( image, format, type, whole_image ? ui32Height : uiInternalHeight, (unsigned long) ui32Width );
   
   if ( image == NULL )
   {
      _TIFFfree ( buf );
      TIFFClose(tif);
      gan_err_register ( "gan_read_tiff_image", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* read image data */
   switch ( config )
   {
      case PLANARCONFIG_CONTIG:
        if(single_field)
        {
           for ( uiRow = 0; uiRow < ui32Height; uiRow++ )
           {
              /* read a line of data */
              TIFFReadScanline ( tif, buf, uiRow, 0 );

              /* only transfer even rows for upper field, or odd rows for upper field */
              if((upper && (uiRow % 2) == 0) || (!upper && (uiRow % 2) == 1))
                 memcpy ( gan_image_get_pixptr(image, whole_image ? (flip ? (image->height-uiRow-1) : uiRow) : (flip ? (uiInternalHeight-uiRow/2-1) : uiRow/2), 0), (const void *)buf, scanlinesize );
           }
        }
        else
        {
           for ( uiRow = 0; uiRow < image->height; uiRow++ )
           {
              /* read a line of data and copy in into the image */
              TIFFReadScanline ( tif, buf, uiRow, 0 );
              memcpy ( gan_image_get_pixptr(image, flip ? (image->height-uiRow-1) : uiRow, 0), (const void *)buf, scanlinesize );
           }
        }

        break;

      case PLANARCONFIG_SEPARATE:
      {
         uint16 s;

         if(single_field)
         {
            for (s = 0; s < nsamples; s++)
               for ( uiRow = 0; uiRow < ui32Height; uiRow++ )
               {
                  TIFFReadScanline ( tif, buf, uiRow, s );

                  /* only transfer even rows for upper field, or odd rows for upper field */
                  if((upper && (uiRow % 2) == 0) || (!upper && (uiRow % 2) == 1))
                     /* fill given sample in each pixel on row */
                     fill_row_samples ( image, whole_image ? (flip ? (image->height-uiRow-1) : uiRow) : (flip ? (uiInternalHeight-uiRow/2-1) : uiRow/2), s, buf );
               }
         }
         else
         {
            for (s = 0; s < nsamples; s++)
               for ( uiRow = 0; uiRow < image->height; uiRow++ )
               {
                  TIFFReadScanline ( tif, buf, uiRow, s );

                  /* fill given sample in each pixel on row */
                  fill_row_samples ( image, flip ? (image->height-uiRow-1) : uiRow, s, buf );
               }
         }
      }
      break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_read_tiff_image", GAN_ERROR_ILLEGAL_TYPE, "" );
        _TIFFfree ( buf );
        TIFFClose(tif);
        return NULL;
   }

   /* free buffer, close TIFF instance and return image */
   _TIFFfree ( buf );
   TIFFClose(tif);
   return image;
}

/**
 * \brief Initialises the write control structure for TIFF files
 *
 * This function should be called on the structure to set the parameters to default values.
 * Then set any non-default values yourself before calling gan_write_tiff_image() or
 * gan_write_tiff_image_stream().
 */
void gan_initialise_tiff_header_struct(Gan_TIFFHeaderStruct *octrlstr)
{
}

/**
 * \brief Writes an image file to a stream in TIFF format.
 * \param outfile The file stream
 * \param image The image structure to write to the file
 * \param octrlstr Pointer to structure controlling output or \c NULL
 * \return #GAN_TRUE on successful write operation, or #GAN_FALSE on failure.
 *
 * Writes the \a image into a TIFF file using a file stream \a outfile.
 * Uses the basic function TIFFReadRGBAImage(), so currently assumes an
 * unsigned character RGBA image.
 *
 * \sa gan_read_tiff_image_stream().
 */
Gan_Bool
 gan_write_tiff_image_stream ( FILE *outfile, const Gan_Image *image, const struct Gan_ImageWriteControlStruct *octrlstr )
{
   gan_assert ( 0, "file streaming not compatible with TIFF library design" );
   return GAN_FALSE;
}

/**
 * \brief Writes an image file in TIFF format.
 * \param filename The name of the image file
 * \param image The image structure to write to the file
 * \param octrlstr Pointer to structure controlling output or \c NULL
 * \return #GAN_TRUE on successful write operation, #GAN_FALSE on failure.
 *
 * Writes the \a image into a TIFF file \a filename.
 *
 * \sa gan_read_tiff_image().
 */
Gan_Bool
 gan_write_tiff_image ( const char *filename, const Gan_Image *image, const struct Gan_ImageWriteControlStruct *octrlstr )
{
   TIFF *tif=NULL;
   unsigned uiRow;
   Gan_Bool flip=GAN_FALSE, single_field=GAN_FALSE, upper=GAN_FALSE, whole_image=GAN_FALSE;
   Gan_Bool result=GAN_FALSE;
   unsigned int height = image->height;
   char* achBlankRow=NULL;

   /* read generic control parameters */
   if(octrlstr != NULL)
   {
      flip         = octrlstr->flip;
      single_field = octrlstr->single_field;
      upper        = octrlstr->upper;
      if(single_field) /* whole_image only relevant for field-based images */
         whole_image  = octrlstr->whole_image;
   }

   if(single_field && !whole_image)
      height *= 2;

   if(filename != NULL && single_field)
   {
      Gan_Bool merged=GAN_FALSE;
      Gan_ImageHeaderStruct header;
      FILE *pfFile;

      /* check if the file exists */
      pfFile = fopen(filename, "r");
      if(pfFile != NULL)
      {
         Gan_Image* pimage = gan_read_tiff_image ( filename, NULL, NULL, &header );
         fclose(pfFile);
         if(pimage != NULL)
         {
            if(pimage->width == image->width && pimage->height == height && pimage->format == image->format && pimage->type == image->type)
            {
               Gan_ImageWriteControlStruct poctrlstr;

               /* merge images */
               gan_merge_field_into_image(image, octrlstr->flip, octrlstr->upper, octrlstr->whole_image, pimage);

               /* attempt to write file */
               gan_initialise_image_write_control_struct(&poctrlstr, GAN_TIFF_FORMAT, pimage->format, pimage->type);
               poctrlstr.info.tiff = octrlstr->info.tiff;
               result = gan_write_tiff_image ( filename, pimage, &poctrlstr );
               if(result)
                  merged = GAN_TRUE;
            }

            gan_image_free(pimage);
         }
      }

      if(merged)
         return result;
   }

   /* attempt to open file */
   if(filename != NULL)
   {
      tif = TIFFOpen(filename, "w");
      if ( tif == NULL )
      {
         gan_err_flush_trace();
         gan_err_register ( "gan_write_tiff_image", GAN_ERROR_OPENING_FILE, filename );
         return GAN_FALSE;
      }

      /* set image details */
      TIFFSetField ( tif, TIFFTAG_IMAGEWIDTH,  image->width );
      TIFFSetField ( tif, TIFFTAG_IMAGELENGTH, image->height );
      TIFFSetField ( tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG );
      TIFFSetField ( tif, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
   }

   switch ( image->format )
   {
      case GAN_GREY_LEVEL_IMAGE:
        if(tif != NULL)
        {
           TIFFSetField ( tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK );
           TIFFSetField ( tif, TIFFTAG_SAMPLESPERPIXEL, 1 );
        }

        break;

      case GAN_GREY_LEVEL_ALPHA_IMAGE:
        if(tif != NULL)
        {
           gan_uint16 aui16SampleInfo[1];
           aui16SampleInfo[0]=EXTRASAMPLE_UNASSALPHA;
           TIFFSetField ( tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK );
           TIFFSetField ( tif, TIFFTAG_SAMPLESPERPIXEL, 2 );
           TIFFSetField ( tif, TIFFTAG_EXTRASAMPLES, 1, &aui16SampleInfo );
        }

        break;

      case GAN_RGB_COLOUR_IMAGE:
        if(tif != NULL)
        {
           TIFFSetField ( tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB );
           TIFFSetField ( tif, TIFFTAG_SAMPLESPERPIXEL, 3 );
        }

        break;

      case GAN_RGB_COLOUR_ALPHA_IMAGE:
        if(tif != NULL)
        {
           gan_uint16 aui16SampleInfo[1];
           aui16SampleInfo[0]=EXTRASAMPLE_UNASSALPHA;
           TIFFSetField ( tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB );
           TIFFSetField ( tif, TIFFTAG_SAMPLESPERPIXEL, 4 );
           TIFFSetField ( tif, TIFFTAG_EXTRASAMPLES, 1, &aui16SampleInfo );
        }

        break;

      default:
        if(tif != NULL)
        {
           TIFFClose(tif);
           gan_err_flush_trace();
           gan_err_register ( "gan_write_tiff_image", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
        }

        return GAN_FALSE;
   }

   switch ( image->type )
   {
      case GAN_UINT8:
        if(tif != NULL)
        {
           TIFFSetField ( tif, TIFFTAG_SAMPLEFORMAT, SAMPLEFORMAT_UINT );
           TIFFSetField ( tif, TIFFTAG_BITSPERSAMPLE, 8 );
        }

        break;
           
      case GAN_UINT16:
        if(tif != NULL)
        {
           TIFFSetField ( tif, TIFFTAG_SAMPLEFORMAT, SAMPLEFORMAT_UINT );
           TIFFSetField ( tif, TIFFTAG_BITSPERSAMPLE, 16 );
        }

        break;

      case GAN_FLOAT32:
        if(tif != NULL)
        {
           TIFFSetField ( tif, TIFFTAG_SAMPLEFORMAT, SAMPLEFORMAT_IEEEFP );
           TIFFSetField ( tif, TIFFTAG_MINSAMPLEVALUE, 0 );
           TIFFSetField ( tif, TIFFTAG_MAXSAMPLEVALUE, 1 );
           TIFFSetField ( tif, TIFFTAG_BITSPERSAMPLE, 32 );
        }
                
        break;

      default:
        if(tif != NULL)
        {
           TIFFClose(tif);
           gan_err_flush_trace();
           gan_err_register ( "gan_write_tiff_image", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
        }

        return GAN_FALSE;
   }

   if(tif == NULL) return GAN_TRUE;
      
   if(single_field)
   {
      /* create blank row */
      achBlankRow = malloc(image->width*gan_image_pixel_size(image->format, image->type));
      if(achBlankRow == NULL)
      {
         if(tif != NULL) TIFFClose(tif);
         gan_err_flush_trace();
         gan_err_register_with_number ( "gan_write_tiff_image_stream", GAN_ERROR_MALLOC_FAILED, "",
                                        image->width*gan_image_pixel_size(image->format, image->type) );
         return GAN_FALSE;
      }

      memset(achBlankRow, 0, image->width*gan_image_pixel_size(image->format, image->type));
   }

   for ( uiRow = 0; uiRow < height; uiRow++ )
   {
      char *achRow;

      /* ignore row if it's in a field we're not writing */
      if(single_field && ((upper && ((uiRow % 2) == 1)) || (!upper && ((uiRow % 2) == 0))))
         achRow = achBlankRow;
      else
      {
         unsigned long rp;

         if(single_field)
         {
            if(whole_image)
               rp = (flip ? (image->height-uiRow-1) : uiRow);
            else
               rp = (flip ? (image->height-uiRow/2-1) : uiRow/2);
         }
         else
            rp = (flip ? (image->height-uiRow-1) : uiRow);

         achRow = (char *)gan_image_get_pixptr(image, rp, 0);
      }
      
      TIFFWriteScanline ( tif, (tdata_t)achRow, uiRow, 0 );
   }

   /* that's it */
   if(tif != NULL) TIFFClose(tif);

   if(achBlankRow != NULL)
      free(achBlankRow);

   return GAN_TRUE;
}

/**
 * \}
 */

/**
 * \}
 */

#endif /* #ifdef HAVE_TIFF */

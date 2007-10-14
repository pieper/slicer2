/**
 * File:          $RCSfile: image_io.c,v $
 * Module:        Image file I/O functions
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

#include <stdio.h>
#include <string.h>
#include <gandalf/image/io/image_io.h>
#include <gandalf/image/io/png_io.h>
#include <gandalf/image/io/jpeg_io.h>
#include <gandalf/image/io/tiff_io.h>
#include <gandalf/image/io/pbm_io.h>
#include <gandalf/image/io/pgm_io.h>
#include <gandalf/image/io/ppm_io.h>
#include <gandalf/image/io/dpx_io.h>
#include <gandalf/image/io/cineon_io.h>
#include <gandalf/image/io/sgi_io.h>
#include <gandalf/image/io/targa_io.h>
#include <gandalf/common/misc_error.h>

/**
 * \addtogroup ImagePackage
 * \{
 */

/**
 * \addtogroup ImageIO
 * \{
 */

/**
 * \brief Initialise structure controlling image reading with default parameters
 */
void gan_initialise_image_read_control_struct(Gan_ImageReadControlStruct *ictrlstr)
{
   ictrlstr->header_only  = GAN_FALSE;
   ictrlstr->flip         = GAN_FALSE;
   ictrlstr->single_field = GAN_FALSE;
   ictrlstr->upper        = GAN_FALSE;
   ictrlstr->whole_image  = GAN_FALSE;
#if 0
   ictrlstr->itype = GAN_UNDEFINED_TYPE;
   ictrlstr->otype = GAN_UNDEFINED_TYPE;
#endif
}

/**
 * \brief Determine the file format of the given image file
 */
Gan_ImageFileFormat gan_image_determine_file_format_stream ( FILE *infile )
{
   /* read the first eight bytes of the file */
#define MAGIC_STRING_SIZE 18
   unsigned char magic_string[MAGIC_STRING_SIZE];

   if ( fread ( magic_string, 1, MAGIC_STRING_SIZE, infile ) != MAGIC_STRING_SIZE )
      return GAN_UNKNOWN_FORMAT;

   /* reset file pointer */
   fseek ( infile, -MAGIC_STRING_SIZE, SEEK_CUR );

   /* now test the data we've just read */
#ifdef HAVE_PNG
   if(gan_image_is_png(magic_string, MAGIC_STRING_SIZE)) return GAN_PNG_FORMAT;
#endif

#ifdef HAVE_JPEG
   if(gan_image_is_jpeg(magic_string, MAGIC_STRING_SIZE)) return GAN_JPEG_FORMAT;
#endif

#ifdef HAVE_TIFF
   if(gan_image_is_tiff(magic_string, MAGIC_STRING_SIZE)) return GAN_TIFF_FORMAT;
#endif

   if(gan_image_is_pbm(magic_string, MAGIC_STRING_SIZE)) return GAN_PBM_FORMAT;
   if(gan_image_is_pgm(magic_string, MAGIC_STRING_SIZE)) return GAN_PGM_FORMAT;
   if(gan_image_is_ppm(magic_string, MAGIC_STRING_SIZE)) return GAN_PPM_FORMAT;
   if(gan_image_is_dpx(magic_string, MAGIC_STRING_SIZE)) return GAN_DPX_FORMAT;
   if(gan_image_is_cineon(magic_string, MAGIC_STRING_SIZE)) return GAN_CINEON_FORMAT;
   if(gan_image_is_sgi(magic_string, MAGIC_STRING_SIZE)) return GAN_SGI_FORMAT;
   if(gan_image_is_targa(magic_string, MAGIC_STRING_SIZE)) return GAN_TARGA_FORMAT;

   /* format not recognised */
   return GAN_UNKNOWN_FORMAT;
}

/**
 * \brief Determine the file format of the given image file
 */
Gan_ImageFileFormat gan_image_determine_file_format ( const char *filename )
{
   FILE *infile;
   Gan_ImageFileFormat file_format;

   /* attempt to open file */
   infile = fopen ( filename, "rb" );
   if ( infile == NULL )
      return GAN_UNKNOWN_FORMAT;

   file_format = gan_image_determine_file_format_stream(infile);
   fclose(infile);

   return file_format;
}

/**
 * \brief Interprets a string as an image file format.
 * \param format_string File format string, e.g. "png"
 * \return #GAN_UNKNOWN_FORMAT on failure to interpret string, or else va
 *         successfully interpreted file format, e.g. GAN_PNG_FORMAT.
 *
 * Attempts to interpret the given string \a format_string as a file name
 * suffix or other identifying string for a specific image format known by
 * Gandalf, which is returned.
 */
Gan_ImageFileFormat
 gan_image_interpret_format_string ( const char *format_string )
{
#ifdef HAVE_PNG
   if ( strstr ( format_string, "PNG" ) != NULL || strstr ( format_string, "png" ) != NULL )
      return GAN_PNG_FORMAT;
#endif

#ifdef HAVE_JPEG
   if ( strstr ( format_string, "JPG" ) != NULL || strstr ( format_string, "jpg" ) != NULL ||
        strstr ( format_string, "JPEG" ) != NULL || strstr ( format_string, "jpeg" ) != NULL)
      return GAN_JPEG_FORMAT;
#endif

#ifdef HAVE_TIFF
   if ( strstr ( format_string, "TIF" ) != NULL || strstr ( format_string, "tif" ) != NULL ||
        strstr ( format_string, "TIFF" ) != NULL || strstr ( format_string, "tiff" ) != NULL)
      return GAN_TIFF_FORMAT;
#endif

   if ( strstr ( format_string, "PBM" ) != NULL || strstr ( format_string, "pbm" ) != NULL )
      return GAN_PBM_FORMAT;

   if ( strstr ( format_string, "PGM" ) != NULL || strstr ( format_string, "pgm" ) != NULL )
      return GAN_PGM_FORMAT;

   if ( strstr ( format_string, "PPM" ) != NULL || strstr ( format_string, "ppm" ) != NULL )
      return GAN_PPM_FORMAT;

   if ( strstr ( format_string, "DPX" ) != NULL || strstr ( format_string, "dpx" ) != NULL )
      return GAN_DPX_FORMAT;

   if ( strstr ( format_string, "CIN" ) != NULL || strstr ( format_string, "cin" ) != NULL )
      return GAN_CINEON_FORMAT;

   if ( strstr ( format_string, "SGI" ) != NULL || strstr ( format_string, "sgi" ) != NULL )
      return GAN_SGI_FORMAT;

   if ( strstr ( format_string, "TGA" ) != NULL || strstr ( format_string, "tga" ) != NULL ||
        strstr ( format_string, "TARGA" ) != NULL || strstr ( format_string, "targa" ) != NULL )
      return GAN_TARGA_FORMAT;

   return GAN_UNKNOWN_FORMAT;
}

/**
 * \brief Determines whether the given data format is supported by the given file format.
 */
Gan_Bool gan_image_format_type_supported ( Gan_ImageFileFormat file_format, Gan_ImageFormat image_format, Gan_Type type,
                                           const Gan_ImageHeaderInfo* info)
{
   Gan_Image image;

   Gan_ImageWriteControlStruct octrlstr;
   gan_image_form_data(&image, image_format, type, 0, 0, NULL, 0, NULL, 0);
   gan_initialise_image_write_control_struct(&octrlstr, file_format, image_format, type);
   if(info != NULL)
      octrlstr.info = *info;

   switch(file_format)
   {
#ifdef HAVE_PNG
      case GAN_PNG_FORMAT: return gan_write_png_image(NULL, &image, &octrlstr); break;
#endif
#ifdef HAVE_JPEG
      case GAN_JPEG_FORMAT: return gan_write_jpeg_image(NULL, &image, &octrlstr); break;
#endif
#ifdef HAVE_TIFF
      case GAN_TIFF_FORMAT: return gan_write_tiff_image(NULL, &image, &octrlstr); break;
#endif
      case GAN_PBM_FORMAT: return gan_write_pbm_image(NULL, &image, &octrlstr); break;
      case GAN_PGM_FORMAT: return gan_write_pgm_image(NULL, &image, &octrlstr); break;
      case GAN_PPM_FORMAT: return gan_write_ppm_image(NULL, &image, &octrlstr); break;
      case GAN_DPX_FORMAT: return gan_write_dpx_image(NULL, &image, &octrlstr); break;
      case GAN_CINEON_FORMAT: return gan_write_cineon_image(NULL, &image, &octrlstr); break;
      case GAN_SGI_FORMAT: return gan_write_sgi_image(NULL, &image, &octrlstr); break;
      case GAN_TARGA_FORMAT: return gan_write_targa_image(NULL, &image, &octrlstr); break;
      case GAN_UNKNOWN_FORMAT: return GAN_FALSE; break;
      default:
        break;
   }

   /* unknown format */
   gan_err_flush_trace();
   gan_err_register ( "gan_image_format_type_supported", GAN_ERROR_ILLEGAL_IMAGE_FILE_FORMAT, "" );
   return GAN_FALSE;
}

/**
 * \brief Determines whether the given data format supports writing a single field
 */
Gan_Bool gan_image_write_field_supported ( Gan_ImageFileFormat file_format )
{
   switch(file_format)
   {
#ifdef HAVE_PNG
      case GAN_PNG_FORMAT:    return GAN_TRUE; break;
#endif
#ifdef HAVE_JPEG
      case GAN_JPEG_FORMAT:   return GAN_TRUE; break;
#endif
#ifdef HAVE_TIFF
      case GAN_TIFF_FORMAT:   return GAN_TRUE; break;
#endif
      case GAN_PBM_FORMAT:    return GAN_FALSE; break;
      case GAN_PGM_FORMAT:    return GAN_FALSE; break;
      case GAN_PPM_FORMAT:    return GAN_FALSE; break;
      case GAN_DPX_FORMAT:    return GAN_TRUE;  break;
      case GAN_CINEON_FORMAT: return GAN_TRUE;  break;
      case GAN_SGI_FORMAT:    return GAN_TRUE;  break;
      case GAN_TARGA_FORMAT:  return GAN_TRUE;  break;
      default:
        break;
   }

   /* unknown format */
   gan_err_flush_trace();
   gan_err_register ( "gan_image_format_type_supported", GAN_ERROR_ILLEGAL_IMAGE_FILE_FORMAT, "" );
   return GAN_FALSE;
}

#if 0
static Gan_ImageFileFormat
 get_file_format ( const char *filename )
{
   int i;

   /* locate suffix string following last dot in file name */
   for ( i = strlen(filename)-1; i >= 0; i-- )
      if ( filename[i] == '.' ) break;

   if ( i < 0 )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_file_format", GAN_ERROR_FAILURE, "" );
      return GAN_UNKNOWN_FORMAT;
   }
      
   return gan_image_interpret_format_string ( &filename[i+1] );
}
#endif

/**
 * \brief Generates a string identifier for an image file format.
 * \param format Image file format, e.g. #GAN_PNG_FORMAT
 * \return String identifier, e.g. "PNG", or "" for unknown format.
 */
const char *gan_image_file_format_string ( Gan_ImageFileFormat format )
{
   switch(format)
   {
#ifdef HAVE_PNG
      case GAN_PNG_FORMAT: return "PNG"; break;
#endif
#ifdef HAVE_JPEG
      case GAN_JPEG_FORMAT: return "JPEG"; break;
#endif
#ifdef HAVE_TIFF
      case GAN_TIFF_FORMAT: return "TIFF"; break;
#endif
      case GAN_PBM_FORMAT: return "PBM"; break;
      case GAN_PGM_FORMAT: return "PGM"; break;
      case GAN_PPM_FORMAT: return "PPM"; break;
      case GAN_DPX_FORMAT: return "DPX"; break;
      case GAN_CINEON_FORMAT: return "CIN"; break;
      case GAN_SGI_FORMAT: return "SGI"; break;
      case GAN_TARGA_FORMAT: return "TGA"; break;
      default:
        break;
   }

   /* unknown format */
   gan_err_flush_trace();
   gan_err_register ( "gan_image_file_format_string", GAN_ERROR_ILLEGAL_IMAGE_FILE_FORMAT, "" );
   return "";
}

/**
 * \brief Returns boolean value indicating whether the given file format is compressed
 * \param format Image file format, e.g. #GAN_PNG_FORMAT
 * \return #GAN_TRUE is the format is a compressed format, #GAN_FALSE if uncompressed
 */
Gan_Bool gan_image_file_format_compressed ( Gan_ImageFileFormat format )
{
   switch(format)
   {
#ifdef HAVE_PNG
      case GAN_PNG_FORMAT: return GAN_TRUE; break;
#endif
#ifdef HAVE_JPEG
      case GAN_JPEG_FORMAT: return GAN_TRUE; break;
#endif
#ifdef HAVE_TIFF
      case GAN_TIFF_FORMAT: return GAN_FALSE; break;
#endif
      case GAN_PBM_FORMAT: return GAN_FALSE; break;
      case GAN_PGM_FORMAT: return GAN_FALSE; break;
      case GAN_PPM_FORMAT: return GAN_FALSE; break;
      case GAN_DPX_FORMAT: return GAN_FALSE; break;
      case GAN_CINEON_FORMAT: return GAN_FALSE; break;
      case GAN_SGI_FORMAT: return GAN_FALSE; break;
      case GAN_TARGA_FORMAT: return GAN_FALSE; break;
      default:
        break;
   }

   /* unknown format */
   gan_err_flush_trace();
   gan_err_register ( "gan_image_file_format_compressed", GAN_ERROR_ILLEGAL_IMAGE_FILE_FORMAT, "" );
   return GAN_FALSE;
}


/**
 * \brief Reads an image file from a file stream.
 * \param infile Input file stream
 * \param file_format Image file format, e.g. GAN_PNG_FORMAT, or #GAN_UNKNOWN_FORMAT
 * \param image The image structure to read the image data into or \c NULL
 * \param ictrlstr Structure controlling read or \c NULL
 * \param header Output information structure containing format-dependent header data, or \c NULL
 * \return Pointer to image structure, or \c NULL on failure.
 *
 * Reads an image from the given file stream \a infile into the provided
 * \a image structure. If \a image is \c NULL a new image is dynamically
 * allocated; otherwise the already allocated \a image structure is reused.
 *
 * If the file format is known, it should be specified by \a file_format;
 * otherwise pass \a file_format as #GAN_UNKNOWN_FORMAT and the function will
 * try to determine the file format.
 *
 * \sa gan_write_image_stream().
 */
Gan_Image *
 gan_image_read_stream ( FILE *infile, Gan_ImageFileFormat file_format, Gan_Image *image,
                         const Gan_ImageReadControlStruct *ictrlstr, Gan_ImageHeaderStruct *header )
{
   /* determine actual file format if necessary */
   if(file_format == GAN_UNKNOWN_FORMAT)
      file_format = gan_image_determine_file_format_stream(infile);

   if(file_format == GAN_UNKNOWN_FORMAT)
   {
      gan_err_register ( "gan_image_read_stream", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* initialise header info */
   if(header)
   {
      header->file_format = GAN_UNKNOWN_FORMAT;
      header->width = 0;
      header->height = 0;
      header->format = GAN_UNDEFINED_IMAGE_FORMAT;
      header->type = GAN_UNDEFINED_TYPE;
      gan_initialise_image_header_info(&header->info, GAN_UNKNOWN_FORMAT, GAN_UNDEFINED_TYPE);
   }

   switch ( file_format )
   {
#ifdef HAVE_PNG
      case GAN_PNG_FORMAT:
        image = gan_read_png_image_stream ( infile, image, ictrlstr, header );
        break;
#endif

#ifdef HAVE_JPEG
      case GAN_JPEG_FORMAT:
        image = gan_read_jpeg_image_stream ( infile, image, ictrlstr, header );
        break;
#endif

      case GAN_PBM_FORMAT:
        image = gan_read_pbm_image_stream ( infile, image, ictrlstr, header );
        break;

      case GAN_PGM_FORMAT:
        image = gan_read_pgm_image_stream ( infile, image, ictrlstr, header );
        break;

      case GAN_PPM_FORMAT:
        image = gan_read_ppm_image_stream ( infile, image, ictrlstr, header );
        break;

      case GAN_DPX_FORMAT:
        image = gan_read_dpx_image_stream ( infile, image, ictrlstr, header );
        break;

      case GAN_CINEON_FORMAT:
        image = gan_read_cineon_image_stream ( infile, image, ictrlstr, header );
        break;

      case GAN_SGI_FORMAT:
        image = gan_read_sgi_image_stream ( infile, image, ictrlstr, header );
        break;

      case GAN_TARGA_FORMAT:
        image = gan_read_targa_image_stream ( infile, image, ictrlstr, header );
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_image_read_stream", GAN_ERROR_ILLEGAL_IMAGE_FILE_FORMAT, "" );
        return NULL;
   }

   /* check that image was read successfully */
   if ( image == NULL )
   {
      gan_err_register ( "gan_image_read_stream", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* success */
   return image;
}

/**
 * \brief Reads an image file.
 * \param filename The name of the image file
 * \param file_format The format of the file, e.g. GAN_PNG_FORMAT
 * \param image The image structure to read the image data into or NULL
 * \param ictrlstr Structure controlling read or \c NULL
 * \param header Output information structure containing format-dependent header data, or \c NULL
 * \return Pointer to \a image structure, or \c NULL on failure.
 *
 * Reads an image from the given file \a filename into the provided \a image
 * structure. If \a image is \c NULL a new image is dynamically allocated;
 * otherwise the already allocated \a image structure is reused.
 *
 * If the file format is known, it should be specified in \a file_format;
 * otherwise pass \a file_format as #GAN_UNKNOWN_FORMAT and the function will
 * try to determine the file format.
 *
 * \sa gan_write_image().
 */
Gan_Image *
 gan_image_read ( const char *filename, Gan_ImageFileFormat file_format, Gan_Image *image,
                  const Gan_ImageReadControlStruct *ictrlstr, Gan_ImageHeaderStruct *header )
{
   /* determine actual file format if necessary */
   if(file_format == GAN_UNKNOWN_FORMAT)
      file_format = gan_image_determine_file_format(filename);

   if(file_format == GAN_UNKNOWN_FORMAT)
   {
      gan_err_register ( "gan_image_read", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* initialise header info */
   if(header)
   {
      header->file_format = GAN_UNKNOWN_FORMAT;
      header->width = 0;
      header->height = 0;
      header->format = GAN_UNDEFINED_IMAGE_FORMAT;
      header->type = GAN_UNDEFINED_TYPE;
      gan_initialise_image_header_info(&header->info, GAN_UNKNOWN_FORMAT, GAN_UNDEFINED_TYPE);
   }

   switch ( file_format )
   {
#ifdef HAVE_PNG
      case GAN_PNG_FORMAT:
        image = gan_read_png_image ( filename, image, ictrlstr, header );
        break;
#endif

#ifdef HAVE_JPEG
      case GAN_JPEG_FORMAT:
        image = gan_read_jpeg_image ( filename, image, ictrlstr, header );
        break;
#endif

#ifdef HAVE_TIFF
      case GAN_TIFF_FORMAT:
        image = gan_read_tiff_image ( filename, image, ictrlstr, header );
        break;
#endif

      case GAN_PBM_FORMAT:
        image = gan_read_pbm_image ( filename, image, ictrlstr, header );
        break;

      case GAN_PGM_FORMAT:
        image = gan_read_pgm_image ( filename, image, ictrlstr, header );
        break;

      case GAN_PPM_FORMAT:
        image = gan_read_ppm_image ( filename, image, ictrlstr, header );
        break;

      case GAN_DPX_FORMAT:
        image = gan_read_dpx_image ( filename, image, ictrlstr, header );
        break;

      case GAN_CINEON_FORMAT:
        image = gan_read_cineon_image ( filename, image, ictrlstr, header );
        break;

      case GAN_SGI_FORMAT:
        image = gan_read_sgi_image ( filename, image, ictrlstr, header );
        break;

      case GAN_TARGA_FORMAT:
        image = gan_read_targa_image ( filename, image, ictrlstr, header );
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_image_read", GAN_ERROR_ILLEGAL_IMAGE_FILE_FORMAT, "" );
        return NULL;
   }

   /* check that image was read successfully */
   if ( image == NULL )
   {
      gan_err_register ( "gan_image_read", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* success */
   return image;
}

/**
 * \brief Initialise image header structure
 * \param image_format Image pixel format, e.g. #GAN_GREY_LEVEL_IMAGE
 * \param type Image pixel type, e.g. #GAN_UINT8
 * \return #GAN_FALSE on error, #GAN_TRUE otherwise
 */
GANDALF_API Gan_Bool gan_initialise_image_header_info(Gan_ImageHeaderInfo *hdrinfo, Gan_ImageFormat image_format, Gan_Type type)
{
   hdrinfo->time_code = 0;
   hdrinfo->user_bits = 0;
   hdrinfo->interlaced = GAN_FALSE;
   hdrinfo->frame_rate = 0.0F;

#ifdef HAVE_PNG
   gan_initialise_png_header_struct(&hdrinfo->png);
#endif

#ifdef HAVE_JPEG
   gan_initialise_jpeg_header_struct(&hdrinfo->jpeg);
#endif

#ifdef HAVE_TIFF
   gan_initialise_tiff_header_struct(&hdrinfo->tiff);
#endif

#if 0
   /* no structure defined at the moment */
   gan_initialise_pbm_header_struct(&hdrinfo->pbm);
   gan_initialise_pgm_header_struct(&hdrinfo->pgm);
   gan_initialise_ppm_header_struct(&hdrinfo->ppm);
#endif
   gan_initialise_dpx_header_struct(&hdrinfo->dpx, image_format, type);
   gan_initialise_cineon_header_struct(&hdrinfo->cineon, image_format, type);
   gan_initialise_sgi_header_struct(&hdrinfo->sgi);
   gan_initialise_targa_header_struct(&hdrinfo->targa);
   return GAN_TRUE;
}

/**
 * \brief Initialise structure controlling image writing with default parameters
 * \param octrlstr Pointer to structure to be filled with default values
 * \param file_format File format that will be used to write the image
 */
Gan_Bool gan_initialise_image_write_control_struct(Gan_ImageWriteControlStruct *octrlstr,
                                                   Gan_ImageFileFormat file_format, Gan_ImageFormat image_format, Gan_Type type)
{
   octrlstr->flip          = GAN_FALSE;
   octrlstr->single_field  = GAN_FALSE;
   octrlstr->upper         = GAN_FALSE;
   octrlstr->whole_image   = GAN_FALSE;
   octrlstr->reverse_bytes = GAN_FALSE;
   if(!gan_initialise_image_header_info(&octrlstr->info, image_format, type))
   {
      gan_err_register ( "gan_image_write_stream", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   return GAN_TRUE;
}


/**
 * \brief Writes an image file to a file stream.
 * \param outfile Output file stream
 * \param file_format The format of the file, e.g. #GAN_PNG_FORMAT, or #GAN_UNKNOWN_FORMAT
 * \param image The image structure to be written to the file
 * \param new_file Whether the file does not currently exist
 * \param octrlstr Format-dependent control structure
 * \return #GAN_TRUE on successful write operation, #GAN_FALSE on failure.
 *
 * Writes an image from the provided image structure into the given file
 * stream.
 *
 * The given file format \a file_format should support the format and type of
 * the \a image; otherwise an error is reported and #GAN_FALSE is returned.
 *
 * \sa gan_read_image().
 */
Gan_Bool
 gan_image_write_stream ( FILE *outfile, Gan_ImageFileFormat file_format, const Gan_Image *image,
                          Gan_Bool new_file, const Gan_ImageWriteControlStruct *octrlstr )
{
   Gan_Bool result;
   switch ( file_format )
   {
#ifdef HAVE_PNG
      case GAN_PNG_FORMAT:
        result = gan_write_png_image_stream ( outfile, image, new_file, octrlstr );
        break;
#endif

#ifdef HAVE_JPEG
      case GAN_JPEG_FORMAT:
        result = gan_write_jpeg_image_stream ( outfile, image, new_file, octrlstr );
        break;
#endif

      case GAN_PBM_FORMAT:
        result = gan_write_pbm_image_stream ( outfile, image, new_file, octrlstr );
        break;

      case GAN_PGM_FORMAT:
        result = gan_write_pgm_image_stream ( outfile, image, new_file, octrlstr );
        break;

      case GAN_PPM_FORMAT:
        result = gan_write_ppm_image_stream ( outfile, image, new_file, octrlstr );
        break;

      case GAN_DPX_FORMAT:
        result = gan_write_dpx_image_stream ( outfile, image, new_file, octrlstr );
        break;

      case GAN_CINEON_FORMAT:
        result = gan_write_cineon_image_stream ( outfile, image, new_file, octrlstr );
        break;

      case GAN_SGI_FORMAT:
        result = gan_write_sgi_image_stream ( outfile, image, new_file, octrlstr );
        break;

      case GAN_TARGA_FORMAT:
        result = gan_write_targa_image_stream ( outfile, image, new_file, octrlstr );
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_image_write_stream", GAN_ERROR_ILLEGAL_IMAGE_FILE_FORMAT, "" );
        return GAN_FALSE;
   }

   if ( !result )
   {
      gan_err_register ( "gan_image_write_stream", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   /* success */
   return GAN_TRUE;
}

/**
 * \brief Writes an image file.
 * \param filename The name of the image file
 * \param file_format The format of the file, e.g. #GAN_PNG_FORMAT
 * \param image The image structure to be written to the file
 * \param octrlstr Format-dependent control structure
 * \return #GAN_TRUE on successful write operation, #GAN_FALSE on failure.
 *
 * Writes an image from the provided \a image structure into the given file.
 *
 * The given file format \a file_format should support the format and type
 * of the image; otherwise an error is reported and #GAN_FALSE is returned.
 * If you want the format to be determined from the file name suffix,
 * pass #GAN_UNKNOWN_FORMAT.
 *
 * \sa gan_read_image().
 */
Gan_Bool
 gan_image_write ( const char *filename, Gan_ImageFileFormat file_format, const Gan_Image *image,
                   const Gan_ImageWriteControlStruct *octrlstr )
{
   Gan_Bool result;
   switch ( file_format )
   {
#ifdef HAVE_PNG
      case GAN_PNG_FORMAT:
        result = gan_write_png_image ( filename, image, octrlstr );
        break;
#endif

#ifdef HAVE_JPEG
      case GAN_JPEG_FORMAT:
        result = gan_write_jpeg_image ( filename, image, octrlstr );
        break;
#endif

#ifdef HAVE_TIFF
      case GAN_TIFF_FORMAT:
        result = gan_write_tiff_image ( filename, image, octrlstr );
        break;
#endif

      case GAN_PBM_FORMAT:
        result = gan_write_pbm_image ( filename, image, octrlstr );
        break;

      case GAN_PGM_FORMAT:
        result = gan_write_pgm_image ( filename, image, octrlstr );
        break;

      case GAN_PPM_FORMAT:
        result = gan_write_ppm_image ( filename, image, octrlstr );
        break;

      case GAN_DPX_FORMAT:
        result = gan_write_dpx_image ( filename, image, octrlstr );
        break;

      case GAN_CINEON_FORMAT:
        result = gan_write_cineon_image ( filename, image, octrlstr );
        break;

      case GAN_SGI_FORMAT:
        result = gan_write_sgi_image ( filename, image, octrlstr );
        break;

      case GAN_TARGA_FORMAT:
        result = gan_write_targa_image ( filename, image, octrlstr );
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_image_write", GAN_ERROR_ILLEGAL_IMAGE_FILE_FORMAT, "" );
        return GAN_FALSE;
   }

   if ( !result )
   {
      gan_err_register ( "gan_image_write", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   /* success */
   return GAN_TRUE;
}

/**
 * File:          $RCSfile: cineon_io.c,v $
 * Module:        DPX format image file I/O functions
 * Part of:       Gandalf Library
 *
 * Revision:      $Revision: 1.1.2.1 $
 * Last edited:   $Date: 2007/10/14 02:20:28 $
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
#include <gandalf/image/io/cineon_io.h>
#include <gandalf/image/io/dpx_io.h>
#include <gandalf/image/io/image_io.h>
#include <gandalf/image/image_gl_uint8.h>
#include <gandalf/image/image_rgb_uint8.h>
#include <gandalf/image/image_rgba_uint8.h>
#include <gandalf/image/image_rgb_uint16.h>
#include <gandalf/common/misc_error.h>
#include <gandalf/common/allocate.h>
#include <gandalf/common/compare.h>

/**
 * \addtogroup ImagePackage
 * \{
 */

/**
 * \defgroup ImageIO Image I/O
 * \{
 */

/* file information offsets */
#define OFFSET_MAGIC          0
#define OFFSET_IMAGEOFFSET    4
#define OFFSET_GENERICSIZE    8
#define OFFSET_INDUSTRYSIZE  12
#define OFFSET_VARIABLESIZE  16
#define OFFSET_FILESIZE      20
#define OFFSET_VERSION       24
#define OFFSET_FILENAME      32
#define OFFSET_CREATIONDATE 132
#define OFFSET_CREATIONTIME 144
#define OFFSET_RESERVED     156

/* image information offsets */
#define OFFSET_ORIENTATION                192
#define OFFSET_NUMBEROFCHANNELS           193
#define OFFSET_DESIGNATORBYTE0            196
#define OFFSET_DESIGNATORBYTE1            197
#define OFFSET_BITSPERPIXEL               198
#define OFFSET_PIXELSPERLINE              200
#define OFFSET_LINESPERIMAGE              204
#define OFFSET_MINIMUMDATAVALUE           208
#define OFFSET_MINIMUMQUANTITYREPRESENTED 212
#define OFFSET_MAXIMUMDATAVALUE           216
#define OFFSET_MAXIMUMQUANTITYREPRESENTED 220
#define OFFSET_CHROMATICITY               420
#define OFFSET_DATAINTERLEAVE             680
#define OFFSET_PACKING                    681
#define OFFSET_SIGNED                     682
#define OFFSET_EOLPADDING                 684
#define OFFSET_EOIMAGEPADDING             688
#define OFFSET_XINPUTDEVICEPITCH          972
#define OFFSET_YINPUTDEVICEPITCH          976
#define OFFSET_IMAGEGAMMA                 980

#define BIG_BUFFER_SIZE 2048

#define CINEON_PACKING_TIGHTEST                       0
#define CINEON_PACKING_BYTE_BOUNDARY_LEFT_JUSTIFIED   1
#define CINEON_PACKING_BYTE_BOUNDARY_RIGHT_JUSTIFIED  2
#define CINEON_PACKING_2BYTE_BOUNDARY_LEFT_JUSTIFIED  3
#define CINEON_PACKING_2BYTE_BOUNDARY_RIGHT_JUSTIFIED 4
#define CINEON_PACKING_4BYTE_BOUNDARY_LEFT_JUSTIFIED  5
#define CINEON_PACKING_4BYTE_BOUNDARY_RIGHT_JUSTIFIED 6

/**
 * \brief Determines whether an image is a Cineon image using the magic number
 */
Gan_Bool gan_image_is_cineon(const unsigned char *magic_string, size_t length)
{
  if (length < 4)
    return(GAN_FALSE);

  if (memcmp(magic_string,"\200\52\137\327",4) == 0 || memcmp(magic_string,"\327\137\52\200",4) == 0)
    return(GAN_TRUE);

  return(GAN_FALSE);
}

/**
 * \brief Reads a RGB colour image file in Cineon format from a stream.
 * \param infile The file stream to be read
 * \param image The image structure to read the image data into or \c NULL
 * \param ictrlstr Pointer to structure controlling input or \c NULL
 * \param header Pointer to file header structure to be filled, or \c NULL
 * \return Pointer to image structure, or \c NULL on failure.
 *
 * Reads the Cineon image from the given file stream \a infile into the given
 * \a image. If \a image is \c NULL a new image is dynamically allocated,
 * otherwise the already allocated \a image structure is reused.
 *
 * \sa gan_write_cineon_image_stream().
 */
Gan_Image *
 gan_read_cineon_image_stream ( FILE *infile, Gan_Image *image, const struct Gan_ImageReadControlStruct *ictrlstr, struct Gan_ImageHeaderStruct *header )
{
   char acHeader[BIG_BUFFER_SIZE], *acAlignedHeader;
   Gan_ImageFormat eFormat;
   Gan_Type eType;
   Gan_Bool bReversedEndianness=GAN_FALSE, bFlip=GAN_FALSE, bPacked=GAN_FALSE;
   Gan_Bool bSingleField=GAN_FALSE, bUpper=GAN_FALSE, bWholeImage=GAN_FALSE;

   /* Cineon file header info */
   gan_uint32 ui32Magic;          /* Magic number */
   gan_uint32 ui32ImageOffset;    /* Offset to start of image data in bytes */

   /* Cineon image information variables */
   gan_uint8  ui8Orientation;           /* image orientation */
   gan_uint8  ui8NumberOfChannels;      /* number of image channels */
   gan_uint32 ui32PixelsPerLine;        /* or x value */
   gan_uint32 ui32LinesPerImageEle;     /* or y value, per element */
   gan_uint8  ui8BitsPerPixel;          /* bit size for channel */
   gan_uint8  ui8DataInterleave;        /* data interleave */
   gan_uint8  ui8Packing;               /* packing type*/
   gan_uint32 ui32eolPadding;           /* end of line padding used in element */
   gan_uint32 ui32eoImagePadding;       /* end of image padding used in element */

   /* align the header array */
   acAlignedHeader = (char*)((unsigned long int)acHeader + 7 - (((unsigned long int)acHeader + 7) % 8));

   /* read the generic file header */
   if(fread(acAlignedHeader, 1, 1024, infile) != 1024)
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_read_cineon_image_stream", GAN_ERROR_CORRUPTED_FILE, "corrupted Cineon file header (truncated file?)" );
      return NULL;
   }

   /* determine endianness from magic number */
   ui32Magic = *((gan_uint32*)(acAlignedHeader + OFFSET_MAGIC));
   if(ui32Magic == 0x802a5fd7)
      bReversedEndianness = GAN_FALSE;
   else if(ui32Magic == 0xd75f2a80)
      bReversedEndianness = GAN_TRUE;
   else
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_read_cineon_image_stream", GAN_ERROR_CORRUPTED_FILE, "corrupted Cineon file header (illegal magic number)" );
      return NULL;
   }

   /* get offset of image data */
   ui32ImageOffset = *((gan_uint32*)(acAlignedHeader + OFFSET_IMAGEOFFSET));
   if(bReversedEndianness)
      vReverseEndianness32(&ui32ImageOffset);

   /* decode image information header stuff */
   ui8Orientation = *((gan_uint8*)(acAlignedHeader + OFFSET_ORIENTATION));

   /* we only support left-to-right orientation */
   if(ui8Orientation != 0 && ui8Orientation != 1)
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_read_cineon_image_stream", GAN_ERROR_NOT_IMPLEMENTED, "unsupported Cineon image orientation" );
      return NULL;
   }

   ui8NumberOfChannels = *((gan_uint8*)(acAlignedHeader + OFFSET_NUMBEROFCHANNELS));
   ui8BitsPerPixel     = *((gan_uint8*)(acAlignedHeader + OFFSET_BITSPERPIXEL));
   if(header != NULL)
      header->info.cineon.bit_size = (unsigned int)ui8BitsPerPixel;

   ui32PixelsPerLine = *((gan_uint32*)(acAlignedHeader + OFFSET_PIXELSPERLINE));
   if(bReversedEndianness)
      vReverseEndianness32(&ui32PixelsPerLine);

   ui32LinesPerImageEle = *((gan_uint32*)(acAlignedHeader + OFFSET_LINESPERIMAGE));
   if(bReversedEndianness)
      vReverseEndianness32(&ui32LinesPerImageEle);

   /* determine image format */
   switch(ui8NumberOfChannels)
   {
      case 1:
        eFormat = GAN_GREY_LEVEL_IMAGE;
        break;

      case 3:
        eFormat = GAN_RGB_COLOUR_IMAGE;
        break;

      case 4:
        eFormat = GAN_RGB_COLOUR_ALPHA_IMAGE;
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_read_cineon_image_stream", GAN_ERROR_NOT_IMPLEMENTED, "unsupported Cineon descriptor" );
        return NULL;
   }

   /* Determine interleave and packing type */
   ui8DataInterleave = *((gan_uint8*)(acAlignedHeader + OFFSET_DATAINTERLEAVE));
   if(ui8DataInterleave != 0)
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_read_cineon_image_stream", GAN_ERROR_NOT_IMPLEMENTED, "unsupported Cineon data interleave" );
      return NULL;
   }

   ui8Packing = *((gan_uint8*)(acAlignedHeader + OFFSET_PACKING));
   if(ui8Packing != CINEON_PACKING_4BYTE_BOUNDARY_LEFT_JUSTIFIED)
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_read_cineon_image_stream", GAN_ERROR_NOT_IMPLEMENTED, "unsupported Cineon packing type" );
      return NULL;
   }

   bPacked = GAN_FALSE;
   if(header != NULL)
      header->info.cineon.packed = bPacked;

   switch(ui8BitsPerPixel)
   {
      case 1:
        eType = GAN_BOOL;
        break;

      case 8:
        eType = GAN_UINT8;
        break;

      case 10:
      case 12:
      case 16:
        eType = GAN_UINT16;
        eType = GAN_UINT16;
        eType = GAN_UINT16;
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_read_cineon_image_stream", GAN_ERROR_NOT_IMPLEMENTED, "unsupported Cineon bit size" );
        return NULL;
        break;
   }
   
   /* Determine end-of-line padding */
   ui32eolPadding = *((gan_uint32*)(acAlignedHeader + OFFSET_EOLPADDING));
   if(bReversedEndianness)
      vReverseEndianness32(&ui32eolPadding);

   /* Determine end-of-image padding */
   ui32eoImagePadding = *((gan_uint32*)(acAlignedHeader + OFFSET_EOIMAGEPADDING));
   if(bReversedEndianness)
      vReverseEndianness32(&ui32eoImagePadding);

   /* read elements of control structure if one was provided */
   if(ictrlstr != NULL)
   {
      bFlip        = ictrlstr->flip;
      bSingleField = ictrlstr->single_field;
      bUpper       = ictrlstr->upper;
      if(bSingleField) /* bWholeImage only relevant for field-based images */
         bWholeImage  = ictrlstr->whole_image;
   }

   if(header != NULL)
   {
      header->file_format = GAN_CINEON_FORMAT;
      header->width = ui32PixelsPerLine;
      header->height = (bSingleField && !bWholeImage) ? (ui32LinesPerImageEle/2) : ui32LinesPerImageEle;
      header->format = eFormat;
      header->type = eType;
   }

   if(ictrlstr != NULL && ictrlstr->header_only)
      return (Gan_Image*)-1; /* special value */

   if(ui8Orientation == 1)
   {
      bFlip = !bFlip;
      bUpper = !bUpper;
   }

   /* reset file point to start of image data */
   if(fseek(infile, ui32ImageOffset, SEEK_SET) != 0)
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_read_cineon_image_stream", GAN_ERROR_CORRUPTED_FILE, "corrupted Cineon image orientation header (truncated file?)" );
      return NULL;
   }

   /* now read the image data */
   switch(ui8BitsPerPixel)
   {
      case 8:
        image = pgiRead8BitDPXImageData(infile, ui32eolPadding, ui32eoImagePadding,
                                        eFormat, eType, ui32PixelsPerLine, ui32LinesPerImageEle, image, bFlip, bSingleField, bUpper, bWholeImage);
        if(image == NULL)
        {
           gan_err_register ( "gan_read_cineon_image_stream", GAN_ERROR_FAILURE, "" );
           return NULL;
        }

        break;

      case 10:
        image = pgiRead10BitDPXImageData(infile, bReversedEndianness, bPacked, ui32eolPadding, ui32eoImagePadding,
                                         eFormat, eType, ui32PixelsPerLine, ui32LinesPerImageEle, image, bFlip, bSingleField, bUpper, bWholeImage);
        if(image == NULL)
        {
           gan_err_register ( "gan_read_cineon_image_stream", GAN_ERROR_FAILURE, "" );
           return NULL;
        }

        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_read_cineon_image_stream", GAN_ERROR_NOT_IMPLEMENTED, "unsupported Cineon bit depth" );
        return NULL;
   }
        
   /* success */
   return image;
}

/**
 * \brief Reads a RGB colour image file in Cineon format.
 * \param filename The name of the image file
 * \param image The image structure to read the image data into or \c NULL
 * \param ictrlstr Pointer to structure controlling input or \c NULL
 * \param header Pointer to file header structure to be filled, or \c NULL
 * \return Pointer to image structure, or \c NULL on failure.
 *
 * Reads the Cineon image with the in the file \a filename into the given
 * \a image. If \a image is \c NULL a new image is dynamically allocated;
 * otherwise the already allocated \a image structure is reused.
 *
 * \sa gan_write_cineon_image().
 */
Gan_Image *
 gan_read_cineon_image ( const char *filename, Gan_Image *image, const struct Gan_ImageReadControlStruct *ictrlstr, struct Gan_ImageHeaderStruct *header )
{
   FILE *infile;
   Gan_Image *result;

   /* attempt to open file */
   infile = fopen ( filename, "rb" );
   if ( infile == NULL )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_read_cineon_image", GAN_ERROR_OPENING_FILE, filename );
      return NULL;
   }

   result = gan_read_cineon_image_stream ( infile, image, ictrlstr, header );
   fclose(infile);
   return result;
}

/**
 * \brief Initialises the header structure for Cineon files
 *
 * This function should be called on the structure to set the parameters to default values.
 * Then set any non-default values yourself before calling gan_write_cineon_image() or
 * gan_write_cineon_image_stream().
 */
void gan_initialise_cineon_header_struct(Gan_CineonHeaderStruct *octrlstr, Gan_ImageFormat image_format, Gan_Type type)
{
   switch(type)
   {
      case GAN_BOOL:
        octrlstr->bit_size = 1;
        break;
        
      case GAN_UINT8:
        octrlstr->bit_size = 8;
        break;
        
      case GAN_UINT16:
        octrlstr->bit_size = 10;
        break;
        
      case GAN_FLOAT32:
        octrlstr->bit_size = 32;
        break;

      default:
        octrlstr->bit_size = 10;
   }

   octrlstr->packed = GAN_FALSE;
}

#define IMAGE_DATA_OFFSET 0x7e00

static gan_uint8 ui8GetCineonBitSize(const Gan_Image *image, const struct Gan_ImageWriteControlStruct *octrlstr)
{
   gan_uint8 ui8BitSize = GAN_UINT8_MAX;
   
   if(octrlstr != NULL && octrlstr->info.dpx.image_info.bit_size != 0)
   {
      ui8BitSize = (gan_uint8)octrlstr->info.cineon.bit_size;

      // check compatibility
      switch(ui8BitSize)
      {
         case 8:
           if(image->type != GAN_UINT8)
              ui8BitSize = GAN_UINT8_MAX;
           break;

         case 10:
           if(image->type != GAN_UINT16)
              ui8BitSize = GAN_UINT8_MAX;
           break;

         case 16:
           if(image->type != GAN_UINT16)
              ui8BitSize = GAN_UINT8_MAX;
           break;

         case 32:
           if(image->type != GAN_FLOAT32)
              ui8BitSize = GAN_UINT8_MAX;
           break;

         default:
           break;
      }
   }

   if(ui8BitSize == GAN_UINT8_MAX)
      switch(image->type)
      {
         case GAN_UINT8:   ui8BitSize =  8; break;
         case GAN_UINT16:  ui8BitSize = 16; break;
         case GAN_FLOAT32: ui8BitSize = 32; break;

         default:
           gan_err_flush_trace();
           gan_err_register ( "ui8GetCineonBitSize", GAN_ERROR_NOT_IMPLEMENTED, "unsupported image type" );
           return GAN_UINT8_MAX;
           break;
      }

   return ui8BitSize;
}

/**
 * \brief Writes a RGB colour image to a file stream in Cineon format.
 * \param outfile The file stream
 * \param image The image structure to write to the file
 * \param new_file Whether the file does not currently exist
 * \param octrlstr Pointer to structure controlling output or \c NULL
 * \return #GAN_TRUE on successful write operation, or #GAN_FALSE on failure.
 *
 * Writes the \a image into the file stream \a outfile in Cineon format.
 *
 * \sa gan_read_cineon_image_stream().
 */
Gan_Bool
 gan_write_cineon_image_stream ( FILE *outfile, const Gan_Image *image, Gan_Bool new_file, const struct Gan_ImageWriteControlStruct *octrlstr )
{
   char acHeader[BIG_BUFFER_SIZE], *acAlignedHeader;
   gan_uint8 ui8BitSize;
   gan_uint32 ui32eolPadding = 0;
   gan_uint8 ui8NumberOfChannels = 0, ui8Channel;
   unsigned int uiVal;
   Gan_Bool bPacked=GAN_FALSE;
   Gan_Bool flip=GAN_FALSE, single_field=GAN_FALSE, upper=GAN_FALSE, whole_image=GAN_FALSE, reverse_bytes=GAN_FALSE;
   unsigned int uiHeight = image->height;

   /* read generic control parameters */
   if(octrlstr != NULL)
   {
      flip          = octrlstr->flip;
      single_field  = octrlstr->single_field;
      upper         = octrlstr->upper;
      whole_image   = octrlstr->whole_image;
      reverse_bytes = octrlstr->reverse_bytes;
   }

   if(single_field && !whole_image)
      uiHeight *= 2;

   /* determine bit size to use */
   ui8BitSize = ui8GetCineonBitSize(image, octrlstr);
   if(ui8BitSize == GAN_UINT8_MAX)
   {
      gan_err_register ( "gan_write_cineon_image_stream", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   /* align the header array */
   acAlignedHeader = (char*)((unsigned long int)acHeader + 7 - (((unsigned long int)acHeader + 7) % 8));

   /* build header */
   memset((void*)acAlignedHeader, 0, 1024);

   /* determine whether to pack the data */
   if(octrlstr != NULL)
      bPacked = octrlstr->info.cineon.packed;
   
   *((gan_uint32*)(acAlignedHeader + OFFSET_MAGIC)) = 0x802a5fd7; /* Magic number */
   *((gan_uint32*)(acAlignedHeader + OFFSET_IMAGEOFFSET)) = IMAGE_DATA_OFFSET;
   *((gan_uint32*)(acAlignedHeader + OFFSET_GENERICSIZE)) = 0x400;
   *((gan_uint32*)(acAlignedHeader + OFFSET_INDUSTRYSIZE)) = 0x400;
   *((gan_uint32*)(acAlignedHeader + OFFSET_VARIABLESIZE)) = 0x7600;
   *((gan_uint32*)(acAlignedHeader + OFFSET_FILESIZE)) = ui32DPXFileSize(IMAGE_DATA_OFFSET, image->format, ui8BitSize, bPacked,
                                                                         uiHeight, image->width, &ui32eolPadding);
   if(reverse_bytes)
   {
      vReverseEndianness32((gan_uint32*)(acAlignedHeader + OFFSET_MAGIC));
      vReverseEndianness32((gan_uint32*)(acAlignedHeader + OFFSET_IMAGEOFFSET));
      vReverseEndianness32((gan_uint32*)(acAlignedHeader + OFFSET_GENERICSIZE));
      vReverseEndianness32((gan_uint32*)(acAlignedHeader + OFFSET_INDUSTRYSIZE));
      vReverseEndianness32((gan_uint32*)(acAlignedHeader + OFFSET_VARIABLESIZE));
      vReverseEndianness32((gan_uint32*)(acAlignedHeader + OFFSET_FILESIZE));
   }

   strcpy((char *)(acAlignedHeader + OFFSET_VERSION), "V4.5");

   switch(image->format)
   {
      case GAN_GREY_LEVEL_IMAGE:
        ui8NumberOfChannels = 1;
        *((gan_uint8*)(acAlignedHeader + OFFSET_DESIGNATORBYTE1)) = 0; /* B/W */
        break;

      case GAN_RGB_COLOUR_IMAGE:
        ui8NumberOfChannels = 3;
        *((gan_uint8*)(acAlignedHeader + OFFSET_DESIGNATORBYTE1)) = 1; /* red */
        *((gan_uint8*)(acAlignedHeader + OFFSET_DESIGNATORBYTE1 + 28)) = 2; /* green */
        *((gan_uint8*)(acAlignedHeader + OFFSET_DESIGNATORBYTE1 + 56)) = 3; /* blue */
        break;

      case GAN_RGB_COLOUR_ALPHA_IMAGE:
        ui8NumberOfChannels = 4;
        *((gan_uint8*)(acAlignedHeader + OFFSET_DESIGNATORBYTE1)) = 1; /* red */
        *((gan_uint8*)(acAlignedHeader + OFFSET_DESIGNATORBYTE1 + 28)) = 2; /* green */
        *((gan_uint8*)(acAlignedHeader + OFFSET_DESIGNATORBYTE1 + 56)) = 3; /* blue */
        *((gan_uint8*)(acAlignedHeader + OFFSET_DESIGNATORBYTE1 + 84)) = 0; /* B/W */
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_write_cineon_image_stream", GAN_ERROR_NOT_IMPLEMENTED, "unsupported image format" );
        return GAN_FALSE;
   }

   *((gan_uint8*)(acAlignedHeader + OFFSET_ORIENTATION)) = 0;
   *((gan_uint8*)(acAlignedHeader + OFFSET_NUMBEROFCHANNELS)) = ui8NumberOfChannels;
   for(ui8Channel=0; ui8Channel<ui8NumberOfChannels; ui8Channel++)
   {
      *((gan_uint8*)(acAlignedHeader + OFFSET_DESIGNATORBYTE0             + 28*ui8Channel)) = 0;
      *((gan_uint8*)(acAlignedHeader + OFFSET_BITSPERPIXEL                + 28*ui8Channel)) = ui8BitSize; /* bit size, e.g. 10-bit */
      *((gan_uint32*)(acAlignedHeader + OFFSET_PIXELSPERLINE              + 28*ui8Channel)) = image->width;
      *((gan_uint32*)(acAlignedHeader + OFFSET_LINESPERIMAGE              + 28*ui8Channel)) = uiHeight;
      *((gan_uint32*)(acAlignedHeader + OFFSET_MINIMUMDATAVALUE           + 28*ui8Channel)) = 0;
      *((gan_uint32*)(acAlignedHeader + OFFSET_MINIMUMQUANTITYREPRESENTED + 28*ui8Channel)) = 0;
      *((gan_uint32*)(acAlignedHeader + OFFSET_MAXIMUMDATAVALUE           + 28*ui8Channel)) = 0x447fc000;
      *((gan_uint32*)(acAlignedHeader + OFFSET_MAXIMUMQUANTITYREPRESENTED + 28*ui8Channel)) = 0x4002f1aa;

      if(reverse_bytes)
      {
         vReverseEndianness32((gan_uint32*)(acAlignedHeader + OFFSET_PIXELSPERLINE              + 28*ui8Channel));
         vReverseEndianness32((gan_uint32*)(acAlignedHeader + OFFSET_LINESPERIMAGE              + 28*ui8Channel));
         vReverseEndianness32((gan_uint32*)(acAlignedHeader + OFFSET_MINIMUMDATAVALUE           + 28*ui8Channel));
         vReverseEndianness32((gan_uint32*)(acAlignedHeader + OFFSET_MINIMUMQUANTITYREPRESENTED + 28*ui8Channel));
         vReverseEndianness32((gan_uint32*)(acAlignedHeader + OFFSET_MAXIMUMDATAVALUE           + 28*ui8Channel));
         vReverseEndianness32((gan_uint32*)(acAlignedHeader + OFFSET_MAXIMUMQUANTITYREPRESENTED + 28*ui8Channel));
      }
   }

   for(uiVal=0; uiVal<8; uiVal++)
   {
      *((gan_uint32*)(acAlignedHeader + OFFSET_CHROMATICITY + uiVal*4)) = 0x7f800000;
      if(reverse_bytes)
         vReverseEndianness32((gan_uint32*)(acAlignedHeader + OFFSET_CHROMATICITY + uiVal*4));
   }

   *((gan_uint8*)(acAlignedHeader + OFFSET_DATAINTERLEAVE)) = 0;
   if(bPacked)
   {
      if(image->format == GAN_RGB_COLOUR_IMAGE && image->type == GAN_UINT16)
         /* save a bit of space by aligning 16 bit RGB images to 16-bit boundaries */
         *((gan_uint8*)(acAlignedHeader + OFFSET_PACKING)) = CINEON_PACKING_2BYTE_BOUNDARY_LEFT_JUSTIFIED;
      else
         *((gan_uint8*)(acAlignedHeader + OFFSET_PACKING)) = CINEON_PACKING_4BYTE_BOUNDARY_LEFT_JUSTIFIED;
   }
   else
      *((gan_uint8*)(acAlignedHeader + OFFSET_PACKING)) = CINEON_PACKING_4BYTE_BOUNDARY_LEFT_JUSTIFIED;

   *((gan_uint32*)(acAlignedHeader + OFFSET_EOLPADDING)) = ui32eolPadding; /* end-of-line padding */
   *((gan_uint32*)(acAlignedHeader + OFFSET_EOIMAGEPADDING)) = 0; /* no end-of-image padding */

   *((gan_uint32*)(acAlignedHeader + OFFSET_XINPUTDEVICEPITCH)) = 0x7f800000;
   *((gan_uint32*)(acAlignedHeader + OFFSET_YINPUTDEVICEPITCH)) = 0x7f800000;
   *((gan_uint32*)(acAlignedHeader + OFFSET_IMAGEGAMMA)) = 0x7f800000;

   if(reverse_bytes)
   {
      vReverseEndianness32((gan_uint32*)(acAlignedHeader + OFFSET_EOLPADDING));
      vReverseEndianness32((gan_uint32*)(acAlignedHeader + OFFSET_EOIMAGEPADDING));
      vReverseEndianness32((gan_uint32*)(acAlignedHeader + OFFSET_XINPUTDEVICEPITCH));
      vReverseEndianness32((gan_uint32*)(acAlignedHeader + OFFSET_YINPUTDEVICEPITCH));
      vReverseEndianness32((gan_uint32*)(acAlignedHeader + OFFSET_IMAGEGAMMA));
   }

   /* write image information header */
   if(outfile != NULL && fwrite((const void *)acAlignedHeader, 1, 1024, outfile) != 1024)
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_write_cineon_image_stream", GAN_ERROR_WRITING_TO_FILE, "");
      return GAN_FALSE;
   }

   /* motion picture header info */
   memset((void*)acAlignedHeader, 0, 1024);
   for(uiVal=0; uiVal<3; uiVal++)
   {
      *((gan_uint32*)(acAlignedHeader + uiVal*4)) = 0xffffffff;
      if(reverse_bytes)
         vReverseEndianness32((gan_uint32*)(acAlignedHeader + uiVal*4));
   }
   
   *((gan_uint32*)(acAlignedHeader + 44)) = 0xffffffff;
   *((gan_uint32*)(acAlignedHeader + 48)) = 0x7f800000;
   if(reverse_bytes)
   {
      vReverseEndianness32((gan_uint32*)(acAlignedHeader + 44));
      vReverseEndianness32((gan_uint32*)(acAlignedHeader + 48));
   }
   
   /* write motion picture info header */
   if(outfile != NULL && fwrite((const void *)acAlignedHeader, 1, 1024, outfile) != 1024)
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_write_cineon_image", GAN_ERROR_WRITING_TO_FILE, "");
      return GAN_FALSE;
   }

   /* write zeros for rest of header */
   {
      unsigned int uiNumBytesLeftToWrite = IMAGE_DATA_OFFSET - 1024 - 1024;

      memset((void*)acHeader, 0, BIG_BUFFER_SIZE);
      while(uiNumBytesLeftToWrite > 0)
      {
         unsigned int uiNumBytes = gan_min2(uiNumBytesLeftToWrite, BIG_BUFFER_SIZE);

         if(outfile != NULL && fwrite((const void*)acHeader, 1, uiNumBytes, outfile) != uiNumBytes)
         {
            gan_err_flush_trace();
            gan_err_register ( "gan_write_cineon_image_stream", GAN_ERROR_WRITING_TO_FILE, "");
            return GAN_FALSE;
         }

         uiNumBytesLeftToWrite -= uiNumBytes;
      }
   }

   /* now write the image data */
   switch(ui8BitSize)
   {
      case 8:
        if(!bWrite8BitDPXImageData(outfile, image, new_file, ui32eolPadding, flip, single_field, upper, whole_image))
        {
           gan_err_register ( "gan_write_cineon_image_stream", GAN_ERROR_FAILURE, "" );
           return GAN_FALSE;
        }

        break;

      case 10:
        if(!bWrite10BitDPXImageData(outfile, image, new_file, reverse_bytes, ui32eolPadding, bPacked, flip, single_field, upper, whole_image))
        {
           gan_err_register ( "gan_write_cineon_image_stream", GAN_ERROR_FAILURE, "" );
           return GAN_FALSE;
        }

        break;

      default:
        if(outfile != NULL)
        {
           gan_err_flush_trace();
           gan_err_register ( "gan_write_cineon_image_stream", GAN_ERROR_NOT_IMPLEMENTED, "unsupported Cineon bit depth" );
        }

        return GAN_FALSE;
   }

   /* success */
   return GAN_TRUE;
}

/**
 * \brief Writes a RGB colour image file in Cineon format.
 * \param filename The name of the image file
 * \param image The image structure to write to the file
 * \param octrlstr Pointer to structure controlling output or \c NULL
 * \return #GAN_TRUE on successful write operation, #GAN_FALSE on failure.
 *
 * Writes the \a image into Cineon file \a filename.
 *
 * \sa gan_read_cineon_image().
 */
Gan_Bool
 gan_write_cineon_image ( const char *filename, const Gan_Image *image, const struct Gan_ImageWriteControlStruct *octrlstr )
{
   if(filename == NULL)
      return gan_write_cineon_image_stream ( NULL, image, GAN_FALSE, octrlstr );
   else
   {
      FILE *outfile;
      Gan_Bool new_file=GAN_TRUE, result;

      if(octrlstr != NULL && octrlstr->single_field)
      {
         /* check if the file exists */
         outfile = fopen ( filename, "r" );
         if(outfile != NULL)
         {
            /* compute file size we will generate */
            gan_uint8 ui8BitSize;
            unsigned int uiFileSize;
            unsigned int uiHeight = image->height;

            ui8BitSize = ui8GetCineonBitSize(image, octrlstr);
            if(ui8BitSize == GAN_UINT8_MAX)
            {
               gan_err_register ( "gan_write_cineon_image", GAN_ERROR_FAILURE, filename );
               return GAN_FALSE;
            }

            if(!octrlstr->whole_image)
               uiHeight *= 2;

            uiFileSize = ui32DPXFileSize(IMAGE_DATA_OFFSET, image->format, ui8BitSize, (octrlstr == NULL) ? GAN_FALSE : octrlstr->info.cineon.packed,
                                         uiHeight, image->width, NULL);

            /* compare file length */
            fseek(outfile, 0, SEEK_END);
            if(ftell(outfile) == uiFileSize)
               new_file = GAN_FALSE;
         
            fclose(outfile);
         }
      }

      /* attempt to open file */
      if(new_file)
         outfile = fopen ( filename, "wb" );
      else
         outfile = fopen ( filename, "rb+" );

      if ( outfile == NULL )
      {
         gan_err_flush_trace();
         gan_err_register ( "gan_write_cineon_image", GAN_ERROR_OPENING_FILE, filename );
         return GAN_FALSE;
      }

      result = gan_write_cineon_image_stream ( outfile, image, new_file, octrlstr );
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

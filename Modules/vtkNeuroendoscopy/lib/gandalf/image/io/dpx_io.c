/**
 * File:          $RCSfile: dpx_io.c,v $
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
#include <gandalf/image/io/dpx_io.h>
#include <gandalf/image/io/image_io.h>
#include <gandalf/image/image_gl_uint8.h>
#include <gandalf/image/image_gl_uint16.h>
#include <gandalf/image/image_gl_float32.h>
#include <gandalf/image/image_rgb_uint8.h>
#include <gandalf/image/image_rgb_uint16.h>
#include <gandalf/image/image_rgb_float32.h>
#include <gandalf/image/image_rgba_uint8.h>
#include <gandalf/image/image_rgba_uint16.h>
#include <gandalf/image/image_rgba_float32.h>
#include <gandalf/common/misc_error.h>
#include <gandalf/common/allocate.h>
#include <gandalf/common/compare.h>

/**
 * \addtogroup ImagePackage
 * \{
 */

/**
 * \addtogroup ImageIO
 * \{
 */

/* file header offsets */
#define OFFSET_MAGIC          0
#define OFFSET_IMAGEOFFSET    4
#define OFFSET_VERSION        8
#define OFFSET_FILESIZE      16
#define OFFSET_DITTOKEY      20
#define OFFSET_GENERICSIZE   24
#define OFFSET_INDUSTRYSIZE  28
#define OFFSET_USERSIZE      32
#define OFFSET_FILENAME      36
#define OFFSET_CREATETIME   136
#define OFFSET_CREATOR      160
#define OFFSET_PROJECT      260
#define OFFSET_COPYRIGHT    460
#define OFFSET_ENCRYPTKEY   660
#define OFFSET_RESERVED     664

/* image information offsets */
#define OFFSET_ORIENTATION       0
#define OFFSET_ELEMENTNUMBER     2
#define OFFSET_PIXELSPERLINE     4
#define OFFSET_LINESPERIMAGEELE  8
#define OFFSET_ORIENTATION       0
#define OFFSET_ELEMENTNUMBER     2
#define OFFSET_PIXELSPERLINE     4
#define OFFSET_LINESPERIMAGEELE  8
#define OFFSET_DATASIGN0        12
#define OFFSET_REFLOWDATA0      16
#define OFFSET_REFLOWQUANTITY0  20
#define OFFSET_REFHIGHDATA0     24
#define OFFSET_REFHIGHQUANTITY0 28
#define OFFSET_DESCRIPTOR0      32
#define OFFSET_TRANSFER0        33
#define OFFSET_COLORIMETRIC0    34
#define OFFSET_BITSIZE0         35
#define OFFSET_PACKING0         36
#define OFFSET_ENCODING0        38
#define OFFSET_DATAOFFSET0      40
#define OFFSET_EOLPADDING0      44
#define OFFSET_EOIMAGEPADDING0  48
#define OFFSET_DESCRIPTION0     52

/* orientation header offsets */
#define OFFSET_ORIENTATION_XOFFSET        0
#define OFFSET_ORIENTATION_YOFFSET        4
#define OFFSET_ORIENTATION_XCENTRE        8
#define OFFSET_ORIENTATION_YCENTRE       12
#define OFFSET_ORIENTATION_XORIGSIZE     16
#define OFFSET_ORIENTATION_YORIGSIZE     20
#define OFFSET_ORIENTATION_FILENAME      24
#define OFFSET_ORIENTATION_CREATIONTIME 124
#define OFFSET_ORIENTATION_INPUTDEV     148
#define OFFSET_ORIENTATION_INPUTSERIAL  180
#define OFFSET_ORIENTATION_BORDER       212
#define OFFSET_ORIENTATION_PIXELASPECT  220

/* Film information offsets */
#define OFFSET_FILM_MFGID           0
#define OFFSET_FILM_FILMTYPE        2
#define OFFSET_FILM_OFFSET          4
#define OFFSET_FILM_PREFIX          6
#define OFFSET_FILM_COUNT          12
#define OFFSET_FILM_FORMAT         16
#define OFFSET_FILM_FRAMEPOSITION  48
#define OFFSET_FILM_SEQUENCE_LEN   52
#define OFFSET_FILM_HELDCOUNT      56
#define OFFSET_FILM_FRAMERATE      60
#define OFFSET_FILM_SHUTTERANGLE   64
#define OFFSET_FILM_FRAMEID        68
#define OFFSET_FILM_SLATEINFO     100

/* TV information offsets */
#define OFFSET_TV_TIMECODE         0
#define OFFSET_TV_USERBITS         4
#define OFFSET_TV_INTERLACE        8
#define OFFSET_TV_FIELD_NUMBER     9
#define OFFSET_TV_VIDEO_SIGNAL    10
#define OFFSET_TV_HORZSAMPLERATE  12
#define OFFSET_TV_VERTSAMPLERATE  16
#define OFFSET_TV_FRAMERATE       20
#define OFFSET_TV_TIMEOFFSET      24
#define OFFSET_TV_GAMMA           28
#define OFFSET_TV_BLACKLEVEL      32
#define OFFSET_TV_BLACKGAIN       36
#define OFFSET_TV_BREAKPOINT      40
#define OFFSET_TV_WHITELEVEL      44
#define OFFSET_TV_INTEGRATIONTIME 48

#define BIG_BUFFER_SIZE 2048

#define DPX_PACKED 0
#define DPX_FILLED 1

/**
 * \brief Reverse the bytes in a 32-bit word.
 */
void
 vReverseEndianness32(gan_uint32* pui32Val)
{
   char *acVal = (char*)pui32Val, cTmp;

   cTmp = acVal[0]; acVal[0] = acVal[3]; acVal[3] = cTmp;
   cTmp = acVal[1]; acVal[1] = acVal[2]; acVal[2] = cTmp;
}

/**
 * \brief Reverse the bytes in a 16-bit word.
 */
void
 vReverseEndianness16(gan_uint16* pui16Val)
{
   char *acVal = (char*)pui16Val, cTmp;

   cTmp = acVal[0]; acVal[0] = acVal[1]; acVal[1] = cTmp;
}

/**
 * \brief Reverse the bytes in a 32-bit word, returning the new value.
 */
gan_uint32
 ui32ReverseEndianness(gan_uint32 ui32Val)
{
   char *acVal = (char*)&ui32Val, cTmp;

   cTmp = acVal[0]; acVal[0] = acVal[3]; acVal[3] = cTmp;
   cTmp = acVal[1]; acVal[1] = acVal[2]; acVal[2] = cTmp;
   return ui32Val;
}

/**
 * \brief Reverse the bytes in a 16-bit word, returning the new value.
 */
gan_uint16
 ui16ReverseEndianness(gan_uint16 ui16Val)
{
   char *acVal = (char*)&ui16Val, cTmp;

   cTmp = acVal[0]; acVal[0] = acVal[1]; acVal[1] = cTmp;
   return ui16Val;
}

/**
 * \brief Reverse the bytes in an array of 16-bit words.
 */
void
 vReverseEndiannessArray16(gan_uint16* aui16Val, unsigned int uiNumberOfVals)
{
   while(uiNumberOfVals--)
   {
      char *acVal = (char*)aui16Val, cTmp;

      cTmp = acVal[0]; acVal[0] = acVal[1]; acVal[1] = cTmp;
      aui16Val++;
   }
}

/**
 * \brief Reverse the bytes in an array of 32-bit words.
 */
void
 vReverseEndiannessArray32(gan_uint32* aui32Val, unsigned int uiNumberOfVals)
{
   while(uiNumberOfVals--)
   {
      char *acVal = (char*)aui32Val, cTmp;

      cTmp = acVal[0]; acVal[0] = acVal[3]; acVal[3] = cTmp;
      cTmp = acVal[1]; acVal[1] = acVal[2]; acVal[2] = cTmp;
      aui32Val++;
   }
}

/**
 * \brief Not a user function
 */
Gan_Image *
 pgiRead1BitDPXImageData(FILE* pfInFile,
                         gan_uint32 ui32eolPadding, gan_uint32 ui32eoImagePadding,
                         Gan_ImageFormat eFormat, Gan_Type eType,
                         gan_uint32 ui32PixelsPerLine, gan_uint32 ui32LinesPerImageEle,
                         Gan_Image* pgiImage, Gan_Bool bFlip, Gan_Bool bSingleField, Gan_Bool bUpper, Gan_Bool bWholeImage)
{
   Gan_Bool bAllocatedImage=GAN_FALSE;
   unsigned int uiRow;
   unsigned int uiRowSizeInBytes, uiInternalHeight;
   char *acBuffer, *acAlignedBuffer;
   Gan_BitArray gbaAlignedBuffer;

   if(eType != GAN_BOOL)
   {
      gan_err_flush_trace();
      gan_err_register ( "pgiRead1BitDPXImageData", GAN_ERROR_NOT_IMPLEMENTED, "no 8 bit conversions supported" );
      return NULL;
   }

   /* interlaced frames must have even height */
   if(bSingleField)
   {
      if((ui32LinesPerImageEle % 2) != 0)
      {
         gan_err_flush_trace();
         gan_err_register ( "pgiRead1BitDPXImageData", GAN_ERROR_INCOMPATIBLE, "" );
         return NULL;
      }

      uiInternalHeight = ui32LinesPerImageEle/2;
   }
   else
      uiInternalHeight = ui32LinesPerImageEle;

   /* determine row data size in bytes */
   switch(eFormat)
   {
      case GAN_GREY_LEVEL_IMAGE:
        uiRowSizeInBytes = ui32PixelsPerLine;
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "pgiRead1BitDPXImageData", GAN_ERROR_NOT_IMPLEMENTED, "image format not supported" );
        return NULL;
   }        

   /* build a buffer to hold a single line of image data and align it to a four byte boundary*/
   acBuffer = malloc(uiRowSizeInBytes+3);
   if(acBuffer == NULL)
   {
      gan_err_flush_trace();
      gan_err_register_with_number ( "pgiRead1BitDPXImageData", GAN_ERROR_MALLOC_FAILED, "", uiRowSizeInBytes+3 );
      return NULL;
   }

   acAlignedBuffer = (char*)((unsigned long int)acBuffer + 3 - (((unsigned long int)acBuffer + 3) % 4));

   /* form bit array around the aligned buffer */
   gan_bit_array_form_data ( &gbaAlignedBuffer, (Gan_BitWord *)acAlignedBuffer, GAN_NO_BITWORDS(pgiImage->width), pgiImage->width );

   /* allocate the image */
   if ( pgiImage == NULL )
   {
      pgiImage = gan_image_alloc(eFormat, eType, bWholeImage ? ui32LinesPerImageEle : uiInternalHeight, ui32PixelsPerLine );
      bAllocatedImage = GAN_TRUE;
   }
   else
      pgiImage = gan_image_set_format_type_dims ( pgiImage, eFormat, eType, bWholeImage ? ui32LinesPerImageEle : uiInternalHeight, ui32PixelsPerLine );

   if ( pgiImage == NULL )
   {
      gan_err_register ( "pgiRead1BitDPXImageData", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* read the image data */
   switch(eFormat)
   {
      case GAN_GREY_LEVEL_IMAGE:
        for(uiRow = 0; uiRow<ui32LinesPerImageEle; uiRow++)
        {
           if(bSingleField)
           {
              /* only transfer even rows for upper field, or odd rows for upper field */
              if((bUpper && (uiRow % 2) == 0) || (!bUpper && (uiRow % 2) == 1))
              {
                 if ( fread ( acAlignedBuffer, 1, uiRowSizeInBytes, pfInFile ) != uiRowSizeInBytes )
                 {
                    if(bAllocatedImage) gan_image_free(pgiImage);
                    gan_err_flush_trace();
                    gan_err_register ( "pgiRead1BitDPXImageData", GAN_ERROR_TRUNCATED_FILE, "truncated DPX file" );
                    return NULL;
                 }
                 
                 gan_bit_array_copy_q ( &gbaAlignedBuffer, &pgiImage->ba[bWholeImage ? (bFlip ? (ui32LinesPerImageEle-uiRow-1) : uiRow) : (bFlip ? (uiInternalHeight-uiRow/2-1) : uiRow/2)] );
              }
              /* otherwise ignore this scanline */
              else if(fseek(pfInFile, uiRowSizeInBytes, SEEK_CUR) != 0)
              {
                 if(bAllocatedImage) gan_image_free(pgiImage);
                 gan_err_flush_trace();
                 gan_err_register ( "pgiRead1BitDPXImageData", GAN_ERROR_TRUNCATED_FILE, "truncated DPX file" );
                 return NULL;
              }
           }
           else
           {
              if ( fread ( acAlignedBuffer, 1, uiRowSizeInBytes, pfInFile ) != uiRowSizeInBytes )
              {
                 if(bAllocatedImage) gan_image_free(pgiImage);
                 gan_err_flush_trace();
                 gan_err_register ( "pgiRead1BitDPXImageData", GAN_ERROR_TRUNCATED_FILE, "truncated DPX file" );
                 return NULL;
              }

              gan_bit_array_copy_q ( &gbaAlignedBuffer, &pgiImage->ba[bFlip ? (ui32LinesPerImageEle-uiRow-1) : uiRow] );
           }

           // allow for padding at end of line
           if(fseek(pfInFile, ui32eolPadding, SEEK_CUR) != 0)
           {
              if(bAllocatedImage) gan_image_free(pgiImage);
              gan_err_flush_trace();
              gan_err_register ( "pgiRead1BitDPXImageData", GAN_ERROR_CORRUPTED_FILE, "corrupted DPX data (truncated file?)" );
              return NULL;
           }
        }

        break;

      default:
        if(bAllocatedImage) gan_image_free(pgiImage);
        gan_err_flush_trace();
        gan_err_register ( "pgiRead1BitDPXImageData", GAN_ERROR_NOT_IMPLEMENTED, "unsupported format" );
        return NULL;
   }        

   free(acBuffer);

   // allow for padding at end of image
   if(fseek(pfInFile, ui32eoImagePadding, SEEK_CUR) != 0)
   {
      if(bAllocatedImage) gan_image_free(pgiImage);
      gan_err_flush_trace();
      gan_err_register ( "pgiRead1BitDPXImageData", GAN_ERROR_CORRUPTED_FILE, "corrupted DPX data (truncated file?)" );
      return NULL;
   }

   gan_bit_array_free ( &gbaAlignedBuffer );
   return pgiImage;
}

/**
 * \brief Not a user function
 */
Gan_Image *
 pgiRead8BitDPXImageData(FILE* pfInFile,
                         gan_uint32 ui32eolPadding, gan_uint32 ui32eoImagePadding,
                         Gan_ImageFormat eFormat, Gan_Type eType,
                         gan_uint32 ui32PixelsPerLine, gan_uint32 ui32LinesPerImageEle,
                         Gan_Image* pgiImage, Gan_Bool bFlip, Gan_Bool bSingleField, Gan_Bool bUpper, Gan_Bool bWholeImage)
{
   Gan_Bool bAllocatedImage=GAN_FALSE;
   unsigned int uiRow;
   int iCol;
   unsigned int uiRowSizeInBytes, uiInternalHeight;
   char *acBuffer, *acAlignedBuffer;

   if(eType != GAN_UINT8)
   {
      gan_err_flush_trace();
      gan_err_register ( "pgiRead8BitDPXImageData", GAN_ERROR_NOT_IMPLEMENTED, "no 8 bit conversions supported" );
      return NULL;
   }

   /* interlaced frames must have even height */
   if(bSingleField)
   {
      if((ui32LinesPerImageEle % 2) != 0)
      {
         gan_err_flush_trace();
         gan_err_register ( "pgiRead8BitDPXImageData", GAN_ERROR_INCOMPATIBLE, "" );
         return NULL;
      }

      uiInternalHeight = ui32LinesPerImageEle/2;
   }
   else
      uiInternalHeight = ui32LinesPerImageEle;

   /* determine row data size in bytes */
   switch(eFormat)
   {
      case GAN_GREY_LEVEL_IMAGE:
        uiRowSizeInBytes = ui32PixelsPerLine;
        break;

      case GAN_RGB_COLOUR_IMAGE:
        uiRowSizeInBytes = 3*ui32PixelsPerLine;
        break;

      case GAN_RGB_COLOUR_ALPHA_IMAGE:
        uiRowSizeInBytes = 4*ui32PixelsPerLine;
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "pgiRead8BitDPXImageData", GAN_ERROR_NOT_IMPLEMENTED, "image format not supported" );
        return NULL;
   }        

   /* build a buffer to hold a single line of image data and align it to a four byte boundary*/
   acBuffer = malloc(uiRowSizeInBytes+3);
   if(acBuffer == NULL)
   {
      gan_err_flush_trace();
      gan_err_register_with_number ( "pgiRead8BitDPXImageData", GAN_ERROR_MALLOC_FAILED, "", uiRowSizeInBytes+3 );
      return NULL;
   }

   acAlignedBuffer = (char*)((unsigned long int)acBuffer + 3 - (((unsigned long int)acBuffer + 3) % 4));

   /* allocate the image */
   if ( pgiImage == NULL )
   {
      pgiImage = gan_image_alloc(eFormat, eType, bWholeImage ? ui32LinesPerImageEle : uiInternalHeight, ui32PixelsPerLine );
      bAllocatedImage = GAN_TRUE;
   }
   else
      pgiImage = gan_image_set_format_type_dims ( pgiImage, eFormat, eType, bWholeImage ? ui32LinesPerImageEle : uiInternalHeight, ui32PixelsPerLine );

   if ( pgiImage == NULL )
   {
      gan_err_register ( "pgiRead8BitDPXImageData", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* read the image data */
   switch(eFormat)
   {
      case GAN_GREY_LEVEL_IMAGE:
        for(uiRow = 0; uiRow<ui32LinesPerImageEle; uiRow++)
        {
           gan_uint8 *pui8ImPix, *pui8BufPix;

           if(bSingleField)
           {
              /* only transfer even rows for upper field, or odd rows for upper field */
              if((bUpper && (uiRow % 2) == 0) || (!bUpper && (uiRow % 2) == 1))
              {
                 if ( fread ( acAlignedBuffer, 1, uiRowSizeInBytes, pfInFile ) != uiRowSizeInBytes )
                 {
                    if(bAllocatedImage) gan_image_free(pgiImage);
                    gan_err_flush_trace();
                    gan_err_register ( "pgiRead8BitDPXImageData", GAN_ERROR_TRUNCATED_FILE, "truncated DPX file" );
                    return NULL;
                 }
                 
                 for(iCol=(int)ui32PixelsPerLine-1,
                     pui8ImPix = gan_image_get_pixptr_gl_ui8(pgiImage, bWholeImage ? (bFlip ? (ui32LinesPerImageEle-uiRow-1) : uiRow) : (bFlip ? (uiInternalHeight-uiRow/2-1) : uiRow/2), iCol),
                     pui8BufPix = ((gan_uint8*)acAlignedBuffer) + iCol; iCol >= 0; iCol--)
                    *pui8ImPix-- = *pui8BufPix--;
              }
              /* otherwise ignore this scanline */
              else if(fseek(pfInFile, uiRowSizeInBytes, SEEK_CUR) != 0)
              {
                 if(bAllocatedImage) gan_image_free(pgiImage);
                 gan_err_flush_trace();
                 gan_err_register ( "pgiRead8BitDPXImageData", GAN_ERROR_TRUNCATED_FILE, "truncated DPX file" );
                 return NULL;
              }
           }
           else
           {
              if ( fread ( acAlignedBuffer, 1, uiRowSizeInBytes, pfInFile ) != uiRowSizeInBytes )
              {
                 if(bAllocatedImage) gan_image_free(pgiImage);
                 gan_err_flush_trace();
                 gan_err_register ( "pgiRead8BitDPXImageData", GAN_ERROR_TRUNCATED_FILE, "truncated DPX file" );
                 return NULL;
              }

              for(iCol=(int)ui32PixelsPerLine-1,
                  pui8ImPix = gan_image_get_pixptr_gl_ui8(pgiImage, bFlip ? (ui32LinesPerImageEle-uiRow-1) : uiRow, iCol),
                  pui8BufPix = ((gan_uint8*)acAlignedBuffer) + iCol; iCol >= 0; iCol--)
                 *pui8ImPix-- = *pui8BufPix--;
           }

           // allow for padding at end of line
           if(fseek(pfInFile, ui32eolPadding, SEEK_CUR) != 0)
           {
              if(bAllocatedImage) gan_image_free(pgiImage);
              gan_err_flush_trace();
              gan_err_register ( "pgiRead8BitDPXImageData", GAN_ERROR_CORRUPTED_FILE, "corrupted DPX data (truncated file?)" );
              return NULL;
           }
        }

        break;

      case GAN_RGB_COLOUR_IMAGE:
        for(uiRow = 0; uiRow<ui32LinesPerImageEle; uiRow++)
        {
           Gan_RGBPixel_ui8 *pui8rgbImPix, *pui8rgbBufPix;

           if(bSingleField)
           {
              /* only transfer even rows for upper field, or odd rows for upper field */
              if((bUpper && (uiRow % 2) == 0) || (!bUpper && (uiRow % 2) == 1))
              {
                 if ( fread ( acAlignedBuffer, 1, uiRowSizeInBytes, pfInFile ) != uiRowSizeInBytes )
                 {
                    if(bAllocatedImage) gan_image_free(pgiImage);
                    gan_err_flush_trace();
                    gan_err_register ( "pgiRead8BitDPXImageData", GAN_ERROR_TRUNCATED_FILE, "truncated DPX file" );
                    return NULL;
                 }

                 for(iCol=(int)ui32PixelsPerLine-1,
                     pui8rgbImPix = gan_image_get_pixptr_rgb_ui8(pgiImage, bWholeImage ? (bFlip ? (ui32LinesPerImageEle-uiRow-1) : uiRow) : (bFlip ? (uiInternalHeight-uiRow/2-1) : uiRow/2), iCol),
                     pui8rgbBufPix = ((Gan_RGBPixel_ui8*)acAlignedBuffer) + iCol; iCol >= 0; iCol--)
                    *pui8rgbImPix-- = *pui8rgbBufPix--;
              }
              /* otherwise ignore this scanline */
              else if(fseek(pfInFile, uiRowSizeInBytes, SEEK_CUR) != 0)
              {
                 if(bAllocatedImage) gan_image_free(pgiImage);
                 gan_err_flush_trace();
                 gan_err_register ( "pgiRead8BitDPXImageData", GAN_ERROR_TRUNCATED_FILE, "truncated DPX file" );
                 return NULL;



                 
              }
           }
           else
           {
              if ( fread ( acAlignedBuffer, 1, uiRowSizeInBytes, pfInFile ) != uiRowSizeInBytes )
              {
                 if(bAllocatedImage) gan_image_free(pgiImage);
                 gan_err_flush_trace();
                 gan_err_register ( "pgiRead8BitDPXImageData", GAN_ERROR_TRUNCATED_FILE, "truncated DPX file" );
                 return NULL;
              }
                 
              for(iCol=(int)ui32PixelsPerLine-1,
                  pui8rgbImPix = gan_image_get_pixptr_rgb_ui8(pgiImage, bFlip ? (ui32LinesPerImageEle-uiRow-1) : uiRow, iCol),
                  pui8rgbBufPix = ((Gan_RGBPixel_ui8*)acAlignedBuffer) + iCol; iCol >= 0; iCol--)
                 *pui8rgbImPix-- = *pui8rgbBufPix--;
           }

           // allow for padding at end of line
           if(fseek(pfInFile, ui32eolPadding, SEEK_CUR) != 0)
           {
              if(bAllocatedImage) gan_image_free(pgiImage);
              gan_err_flush_trace();
              gan_err_register ( "pgiRead8BitDPXImageData", GAN_ERROR_CORRUPTED_FILE, "corrupted DPX data (truncated file?)" );
              return NULL;
           }
        }

        break;

      case GAN_RGB_COLOUR_ALPHA_IMAGE:
        for(uiRow = 0; uiRow<ui32LinesPerImageEle; uiRow++)
        {
           Gan_RGBAPixel_ui8 *pui8rgbaImPix, *pui8rgbaBufPix;

           if(bSingleField)
           {
              /* only transfer even rows for upper field, or odd rows for upper field */
              if((bUpper && (uiRow % 2) == 0) || (!bUpper && (uiRow % 2) == 1))
              {
                 if ( fread ( acAlignedBuffer, 1, uiRowSizeInBytes, pfInFile ) != uiRowSizeInBytes )
                 {
                    if(bAllocatedImage) gan_image_free(pgiImage);
                    gan_err_flush_trace();
                    gan_err_register ( "pgiRead8BitDPXImageData", GAN_ERROR_TRUNCATED_FILE, "truncated DPX file" );
                    return NULL;
                 }

                 for(iCol=(int)ui32PixelsPerLine-1,
                     pui8rgbaImPix = gan_image_get_pixptr_rgba_ui8(pgiImage, bWholeImage ? (bFlip ? (ui32LinesPerImageEle-uiRow-1) : uiRow) : (bFlip ? (uiInternalHeight-uiRow/2-1) : uiRow/2), iCol),
                     pui8rgbaBufPix = ((Gan_RGBAPixel_ui8*)acAlignedBuffer) + iCol; iCol >= 0; iCol--)
                    *pui8rgbaImPix-- = *pui8rgbaBufPix--;
              }
              /* otherwise ignore this scanline */
              else if(fseek(pfInFile, uiRowSizeInBytes, SEEK_CUR) != 0)
              {
                 if(bAllocatedImage) gan_image_free(pgiImage);
                 gan_err_flush_trace();
                 gan_err_register ( "pgiRead8BitDPXImageData", GAN_ERROR_TRUNCATED_FILE, "truncated DPX file" );
                 return NULL;
              }
           }
           else
           {
              if ( fread ( acAlignedBuffer, 1, uiRowSizeInBytes, pfInFile ) != uiRowSizeInBytes )
              {
                 if(bAllocatedImage) gan_image_free(pgiImage);
                 gan_err_flush_trace();
                 gan_err_register ( "pgiRead8BitDPXImageData", GAN_ERROR_TRUNCATED_FILE, "truncated DPX file" );
                 return NULL;
              }

              for(iCol=(int)ui32PixelsPerLine-1,
                  pui8rgbaImPix = gan_image_get_pixptr_rgba_ui8(pgiImage, bFlip ? (ui32LinesPerImageEle-uiRow-1) : uiRow, iCol),
                  pui8rgbaBufPix = ((Gan_RGBAPixel_ui8*)acAlignedBuffer) + iCol; iCol >= 0; iCol--)
                 *pui8rgbaImPix-- = *pui8rgbaBufPix--;
           }

           // allow for padding at end of line
           if(fseek(pfInFile, ui32eolPadding, SEEK_CUR) != 0)
           {
              if(bAllocatedImage) gan_image_free(pgiImage);
              gan_err_flush_trace();
              gan_err_register ( "pgiRead8BitDPXImageData", GAN_ERROR_CORRUPTED_FILE, "corrupted DPX data (truncated file?)" );
              return NULL;
           }
        }

        break;

      default:
        if(bAllocatedImage) gan_image_free(pgiImage);
        gan_err_flush_trace();
        gan_err_register ( "pgiRead8BitDPXImageData", GAN_ERROR_NOT_IMPLEMENTED, "unsupported format" );
        return NULL;
   }        

   free(acBuffer);

   // allow for padding at end of image
   if(fseek(pfInFile, ui32eoImagePadding, SEEK_CUR) != 0)
   {
      if(bAllocatedImage) gan_image_free(pgiImage);
      gan_err_flush_trace();
      gan_err_register ( "pgiRead8BitDPXImageData", GAN_ERROR_CORRUPTED_FILE, "corrupted DPX data (truncated file?)" );
      return NULL;
   }

   return pgiImage;
}

/**
 * \brief Not a user function
 */
Gan_Image *
 pgiRead10BitDPXImageData(FILE* pfInFile, Gan_Bool bReversedEndianness, Gan_Bool bPacked,
                          gan_uint32 ui32eolPadding, gan_uint32 ui32eoImagePadding,
                          Gan_ImageFormat eFormat, Gan_Type eType,
                          gan_uint32 ui32PixelsPerLine, gan_uint32 ui32LinesPerImageEle,
                          Gan_Image* pgiImage, Gan_Bool bFlip, Gan_Bool bSingleField, Gan_Bool bUpper, Gan_Bool bWholeImage)
{
   Gan_Bool bAllocatedImage=GAN_FALSE;
   unsigned int uiRow;
   int iCol;
   unsigned int uiRowSizeInBytes, uiInternalHeight;
   char *acBuffer, *acAlignedBuffer;

   if(bPacked)
   {
      gan_err_flush_trace();
      gan_err_register ( "pgiRead10BitDPXImageData", GAN_ERROR_NOT_IMPLEMENTED, "only unpacked 10 bit format supported" );
      return NULL;
   }

   if(eType != GAN_UINT16)
   {
      gan_err_flush_trace();
      gan_err_register ( "pgiRead10BitDPXImageData", GAN_ERROR_NOT_IMPLEMENTED, "only 10 --> 16 bit conversion supported" );
      return NULL;
   }

   /* interlaced frames must have even height */
   if(bSingleField)
   {
      if((ui32LinesPerImageEle % 2) != 0)
      {
         gan_err_flush_trace();
         gan_err_register ( "pgiRead10BitDPXImageData", GAN_ERROR_INCOMPATIBLE, "" );
         return NULL;
      }

      uiInternalHeight = ui32LinesPerImageEle/2;
   }
   else
      uiInternalHeight = ui32LinesPerImageEle;

   /* determine row data size in bytes */
   switch(eFormat)
   {
      case GAN_RGB_COLOUR_IMAGE:
        uiRowSizeInBytes = 4*ui32PixelsPerLine;
        break;

      case GAN_RGB_COLOUR_ALPHA_IMAGE:
      {
         gan_uint32 ui32PadTest=0;

         uiRowSizeInBytes = ((4*ui32PixelsPerLine+2)/3)*4;
         /* check and adjust end-of-line padding */
         switch((4*ui32PixelsPerLine) % 3)
         {
            case 0: ui32PadTest = 0; break;
            case 1: ui32PadTest = 2; break;
            case 2: ui32PadTest = 1; break;
         }
         
         if((ui32eolPadding % 4) != ui32PadTest)
         {
            gan_err_flush_trace();
            gan_err_register ( "pgiRead10BitDPXImageData", GAN_ERROR_NOT_IMPLEMENTED, "padding not compatible" );
            return NULL;
         }

         // reduce pad to multiple of four bytes because we are going to read the data on each line as a multiple of four bytes
         ui32eolPadding -= ui32PadTest;
      }
      break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "pgiRead10BitDPXImageData", GAN_ERROR_NOT_IMPLEMENTED, "image format not supported" );
        return NULL;
   }        

   /* build a buffer to hold a single line of image data and align it to a four byte boundary*/
   acBuffer = malloc(uiRowSizeInBytes+3);
   if(acBuffer == NULL)
   {
      gan_err_flush_trace();
      gan_err_register_with_number ( "pgiRead10BitDPXImageData", GAN_ERROR_MALLOC_FAILED, "", uiRowSizeInBytes+3 );
      return NULL;
   }

   acAlignedBuffer = (char*)((unsigned long int)acBuffer + 3 - (((unsigned long int)acBuffer + 3) % 4));

   /* allocate the image */
   if ( pgiImage == NULL )
   {
      pgiImage = gan_image_alloc(eFormat, eType, bWholeImage ? ui32LinesPerImageEle : uiInternalHeight, ui32PixelsPerLine );
      bAllocatedImage = GAN_TRUE;
   }
   else
      pgiImage = gan_image_set_format_type_dims ( pgiImage, eFormat, eType, bWholeImage ? ui32LinesPerImageEle : uiInternalHeight, ui32PixelsPerLine );

   if ( pgiImage == NULL )
   {
      gan_err_register ( "pgiRead10BitDPXImageData", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* read the image data */
   switch(eFormat)
   {
      case GAN_RGB_COLOUR_IMAGE:
        for(uiRow = 0; uiRow<ui32LinesPerImageEle; uiRow++)
        {
           Gan_RGBPixel_ui16* pui16rgbPix;
           gan_uint32* pui32Pix;

           if(bSingleField)
           {
              /* only transfer even rows for upper field, or odd rows for upper field */
              if((bUpper && (uiRow % 2) == 0) || (!bUpper && (uiRow % 2) == 1))
              {
                 if ( fread ( acAlignedBuffer, 1, uiRowSizeInBytes, pfInFile ) != uiRowSizeInBytes )
                 {
                    if(bAllocatedImage) gan_image_free(pgiImage);
                    gan_err_flush_trace();
                    gan_err_register ( "pgiRead10BitDPXImageData", GAN_ERROR_TRUNCATED_FILE, "truncated DPX file" );
                    return NULL;
                 }

                 for(iCol=(int)ui32PixelsPerLine-1,
                     pui16rgbPix = gan_image_get_pixptr_rgb_ui16(pgiImage, bWholeImage ? (bFlip ? (ui32LinesPerImageEle-uiRow-1) : uiRow) : (bFlip ? (uiInternalHeight-uiRow/2-1) : uiRow/2), iCol),
                     pui32Pix = ((gan_uint32*)acAlignedBuffer) + iCol; iCol >= 0; iCol--, pui16rgbPix--, pui32Pix--)
                 {
                    if(bReversedEndianness)
                       vReverseEndianness32(pui32Pix);

                    pui16rgbPix->R = (gan_uint16)(((*pui32Pix) & 0xffc00000) >> 16);
                    pui16rgbPix->G = (gan_uint16)(((*pui32Pix) &   0x3ff000) >>  6);
                    pui16rgbPix->B = (gan_uint16)(((*pui32Pix) &      0xffc) <<  4);
                 }
              }
              /* otherwise ignore this scanline */
              else if(fseek(pfInFile, uiRowSizeInBytes, SEEK_CUR) != 0)
              {
                 if(bAllocatedImage) gan_image_free(pgiImage);
                 gan_err_flush_trace();
                 gan_err_register ( "pgiRead10BitDPXImageData", GAN_ERROR_TRUNCATED_FILE, "truncated DPX file" );
                 return NULL;
              }
           }
           else
           {
              if ( fread ( acAlignedBuffer, 1, uiRowSizeInBytes, pfInFile ) != uiRowSizeInBytes )
              {
                 if(bAllocatedImage) gan_image_free(pgiImage);
                 gan_err_flush_trace();
                 gan_err_register ( "pgiRead10BitDPXImageData", GAN_ERROR_TRUNCATED_FILE, "truncated DPX file" );
                 return NULL;
              }

              for(iCol=(int)ui32PixelsPerLine-1,
                  pui16rgbPix = gan_image_get_pixptr_rgb_ui16(pgiImage, bFlip ? (ui32LinesPerImageEle-uiRow-1) : uiRow, iCol),
                  pui32Pix = ((gan_uint32*)acAlignedBuffer) + iCol; iCol >= 0; iCol--, pui16rgbPix--, pui32Pix--)
              {
                 if(bReversedEndianness)
                    vReverseEndianness32(pui32Pix);

                 pui16rgbPix->R = (gan_uint16)(((*pui32Pix) & 0xffc00000) >> 16);
                 pui16rgbPix->G = (gan_uint16)(((*pui32Pix) &   0x3ff000) >>  6);
                 pui16rgbPix->B = (gan_uint16)(((*pui32Pix) &      0xffc) <<  4);
              }
           }

           // allow for padding at end of line
           if(fseek(pfInFile, ui32eolPadding, SEEK_CUR) != 0)
           {
              if(bAllocatedImage) gan_image_free(pgiImage);
              gan_err_flush_trace();
              gan_err_register ( "pgiRead10BitDPXImageData", GAN_ERROR_CORRUPTED_FILE, "corrupted DPX data (truncated file?)" );
              return NULL;
           }
        }

        break;

      case GAN_RGB_COLOUR_ALPHA_IMAGE:
        for(uiRow = 0; uiRow<ui32LinesPerImageEle; uiRow++)
        {
           Gan_RGBAPixel_ui16* pui16rgbaPix;
           gan_uint32* pui32Pix;
           unsigned int uiCol;

           if(bSingleField)
           {
              /* only transfer even rows for upper field, or odd rows for upper field */
              if((bUpper && (uiRow % 2) == 0) || (!bUpper && (uiRow % 2) == 1))
              {
                 if ( fread ( acAlignedBuffer, 1, uiRowSizeInBytes, pfInFile ) != uiRowSizeInBytes )
                 {
                    if(bAllocatedImage) gan_image_free(pgiImage);
                    gan_err_flush_trace();
                    gan_err_register ( "pgiRead10BitDPXImageData", GAN_ERROR_TRUNCATED_FILE, "truncated DPX file" );
                    return NULL;
                 }

                 for(uiCol=0,
                     pui16rgbaPix = gan_image_get_pixptr_rgba_ui16(pgiImage, bWholeImage ? (bFlip ? (ui32LinesPerImageEle-uiRow-1) : uiRow) : (bFlip ? (uiInternalHeight-uiRow/2-1) : uiRow/2), 0),
                     pui32Pix = (gan_uint32*)acAlignedBuffer; uiCol < ui32PixelsPerLine; uiCol++, pui16rgbaPix++)
                 {
                    switch(uiCol % 3)
                    {
                       case 0:
                         if(bReversedEndianness)
                            vReverseEndianness32(pui32Pix);

                         pui16rgbaPix->R = (gan_uint16)(((*pui32Pix) & 0xffc00000) >> 16);
                         pui16rgbaPix->G = (gan_uint16)(((*pui32Pix) &   0x3ff000) >>  6);
                         pui16rgbaPix->B = (gan_uint16)(((*pui32Pix) &      0xffc) <<  4);
                         pui32Pix++;
                         if(bReversedEndianness)
                            vReverseEndianness32(pui32Pix);
                         
                         pui16rgbaPix->A = (gan_uint16)(((*pui32Pix) & 0xffc00000) >> 16);
                         break;

                       case 1:
                         pui16rgbaPix->R = (gan_uint16)(((*pui32Pix) &   0x3ff000) >>  6);
                         pui16rgbaPix->G = (gan_uint16)(((*pui32Pix) &      0xffc) <<  4);
                         pui32Pix++;
                         if(bReversedEndianness)
                            vReverseEndianness32(pui32Pix);

                         pui16rgbaPix->B = (gan_uint16)(((*pui32Pix) & 0xffc00000) >> 16);
                         pui16rgbaPix->A = (gan_uint16)(((*pui32Pix) &   0x3ff000) >>  6);
                         break;

                       case 2:
                         pui16rgbaPix->R = (gan_uint16)(((*pui32Pix) &      0xffc) <<  4);
                         pui32Pix++;
                         if(bReversedEndianness)
                            vReverseEndianness32(pui32Pix);

                         pui16rgbaPix->G = (gan_uint16)(((*pui32Pix) & 0xffc00000) >> 16);
                         pui16rgbaPix->B = (gan_uint16)(((*pui32Pix) &   0x3ff000) >>  6);
                         pui16rgbaPix->A = (gan_uint16)(((*pui32Pix) &      0xffc) <<  4);
                         pui32Pix++;
                    }
                 }
              }
              /* otherwise ignore this scanline */
              else if(fseek(pfInFile, uiRowSizeInBytes, SEEK_CUR) != 0)
              {
                 if(bAllocatedImage) gan_image_free(pgiImage);
                 gan_err_flush_trace();
                 gan_err_register ( "pgiRead10BitDPXImageData", GAN_ERROR_TRUNCATED_FILE, "truncated DPX file" );
                 return NULL;
              }
           }
           else
           {
              if ( fread ( acAlignedBuffer, 1, uiRowSizeInBytes, pfInFile ) != uiRowSizeInBytes )
              {
                 if(bAllocatedImage) gan_image_free(pgiImage);
                 gan_err_flush_trace();
                 gan_err_register ( "pgiRead10BitDPXImageData", GAN_ERROR_TRUNCATED_FILE, "truncated DPX file" );
                 return NULL;
              }

              for(uiCol=0,
                  pui16rgbaPix = gan_image_get_pixptr_rgba_ui16(pgiImage, bFlip ? (ui32LinesPerImageEle-uiRow-1) : uiRow, 0),
                  pui32Pix = (gan_uint32*)acAlignedBuffer; uiCol < ui32PixelsPerLine; uiCol++, pui16rgbaPix++)
              {
                 switch(uiCol % 3)
                 {
                    case 0:
                      if(bReversedEndianness)
                         vReverseEndianness32(pui32Pix);

                      pui16rgbaPix->R = (gan_uint16)(((*pui32Pix) & 0xffc00000) >> 16);
                      pui16rgbaPix->G = (gan_uint16)(((*pui32Pix) &   0x3ff000) >>  6);
                      pui16rgbaPix->B = (gan_uint16)(((*pui32Pix) &      0xffc) <<  4);
                      pui32Pix++;
                      if(bReversedEndianness)
                         vReverseEndianness32(pui32Pix);
                         
                      pui16rgbaPix->A = (gan_uint16)(((*pui32Pix) & 0xffc00000) >> 16);
                      break;

                    case 1:
                      pui16rgbaPix->R = (gan_uint16)(((*pui32Pix) &   0x3ff000) >>  6);
                      pui16rgbaPix->G = (gan_uint16)(((*pui32Pix) &      0xffc) <<  4);
                      pui32Pix++;
                      if(bReversedEndianness)
                         vReverseEndianness32(pui32Pix);

                      pui16rgbaPix->B = (gan_uint16)(((*pui32Pix) & 0xffc00000) >> 16);
                      pui16rgbaPix->A = (gan_uint16)(((*pui32Pix) &   0x3ff000) >>  6);
                      break;

                    case 2:
                      pui16rgbaPix->R = (gan_uint16)(((*pui32Pix) &      0xffc) <<  4);
                      pui32Pix++;
                      if(bReversedEndianness)
                         vReverseEndianness32(pui32Pix);

                      pui16rgbaPix->G = (gan_uint16)(((*pui32Pix) & 0xffc00000) >> 16);
                      pui16rgbaPix->B = (gan_uint16)(((*pui32Pix) &   0x3ff000) >>  6);
                      pui16rgbaPix->A = (gan_uint16)(((*pui32Pix) &      0xffc) <<  4);
                      pui32Pix++;
                 }
              }
           }

           // allow for padding at end of line
           if(fseek(pfInFile, ui32eolPadding, SEEK_CUR) != 0)
           {
              if(bAllocatedImage) gan_image_free(pgiImage);
              gan_err_flush_trace();
              gan_err_register ( "pgiRead10BitDPXImageData", GAN_ERROR_CORRUPTED_FILE, "corrupted DPX data (truncated file?)" );
              return NULL;
           }
        }

        break;

      default:
        if(bAllocatedImage) gan_image_free(pgiImage);
        gan_err_flush_trace();
        gan_err_register ( "pgiRead10BitDPXImageData", GAN_ERROR_NOT_IMPLEMENTED, "image format not supported" );
        return NULL;
   }        

   free(acBuffer);

   // allow for padding at end of image
   if(fseek(pfInFile, ui32eoImagePadding, SEEK_CUR) != 0)
   {
      if(bAllocatedImage) gan_image_free(pgiImage);
      gan_err_flush_trace();
      gan_err_register ( "pgiRead10BitDPXImageData", GAN_ERROR_CORRUPTED_FILE, "corrupted DPX data (truncated file?)" );
      return NULL;
   }

   return pgiImage;
}

/**
 * \brief Not a user function
 */
Gan_Image *
 pgiRead12BitDPXImageData(FILE* pfInFile, Gan_Bool bReversedEndianness, Gan_Bool bPacked,
                          gan_uint32 ui32eolPadding, gan_uint32 ui32eoImagePadding,
                          Gan_ImageFormat eFormat, Gan_Type eType,
                          gan_uint32 ui32PixelsPerLine, gan_uint32 ui32LinesPerImageEle,
                          Gan_Image* pgiImage, Gan_Bool bFlip, Gan_Bool bSingleField, Gan_Bool bUpper, Gan_Bool bWholeImage)
{
   Gan_Bool bAllocatedImage=GAN_FALSE;
   unsigned int uiRow;
   int iCol;
   unsigned int uiRowSizeInBytes, uiInternalHeight;
   char *acBuffer, *acAlignedBuffer;

   gan_err_flush_trace();
   gan_err_register ( "pgiRead12BitDPXImageData", GAN_ERROR_NOT_IMPLEMENTED, "");
   return NULL;

   if(eType != GAN_UINT16)
   {
      gan_err_flush_trace();
      gan_err_register ( "pgiRead12BitDPXImageData", GAN_ERROR_NOT_IMPLEMENTED, "only 12 --> 16 bit conversion supported" );
      return NULL;
   }

   /* interlaced frames must have even height */
   if(bSingleField)
   {
      if((ui32LinesPerImageEle % 2) != 0)
      {
         gan_err_flush_trace();
         gan_err_register ( "pgiRead12BitDPXImageData", GAN_ERROR_INCOMPATIBLE, "" );
         return NULL;
      }

      uiInternalHeight = ui32LinesPerImageEle/2;
   }
   else
      uiInternalHeight = ui32LinesPerImageEle;

   /* determine row data size in bytes */
   switch(eFormat)
   {
      case GAN_RGB_COLOUR_IMAGE:
      {
         if(bPacked)
         {
            gan_uint32 ui32PadTest=0;

            uiRowSizeInBytes = (9*ui32PixelsPerLine + 1)/2;
            ui32PadTest = 3 - ((uiRowSizeInBytes - 1) % 4);

            if((ui32eolPadding % 4) != ui32PadTest)
            {
               gan_err_flush_trace();
               gan_err_register ( "pgiRead12BitDPXImageData", GAN_ERROR_NOT_IMPLEMENTED, "padding not compatible" );
               return NULL;
            }
         }
         else
            uiRowSizeInBytes = 6*ui32PixelsPerLine;
      }
      break;

      case GAN_RGB_COLOUR_ALPHA_IMAGE:
      {
         if(bPacked)
         {
            gan_uint32 ui32PadTest=0;

            uiRowSizeInBytes = (12*ui32PixelsPerLine + 1)/2;
            ui32PadTest = 3 - ((uiRowSizeInBytes - 1) % 4);

            if((ui32eolPadding % 4) != ui32PadTest)
            {
               gan_err_flush_trace();
               gan_err_register ( "pgiRead12BitDPXImageData", GAN_ERROR_NOT_IMPLEMENTED, "padding not compatible" );
               return NULL;
            }
         }
         else
            uiRowSizeInBytes = 8*ui32PixelsPerLine;
      }
      break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "pgiRead12BitDPXImageData", GAN_ERROR_NOT_IMPLEMENTED, "image format not supported" );
        return NULL;
   }        

   /* build a buffer to hold a single line of image data and align it to a four byte boundary*/
   acBuffer = malloc(uiRowSizeInBytes+3);
   if(acBuffer == NULL)
   {
      gan_err_flush_trace();
      gan_err_register_with_number ( "pgiRead12BitDPXImageData", GAN_ERROR_MALLOC_FAILED, "", uiRowSizeInBytes+3 );
      return NULL;
   }

   acAlignedBuffer = (char*)((unsigned long int)acBuffer + 3 - (((unsigned long int)acBuffer + 3) % 4));

   /* allocate the image */
   if ( pgiImage == NULL )
   {
      pgiImage = gan_image_alloc(eFormat, eType, bWholeImage ? ui32LinesPerImageEle : uiInternalHeight, ui32PixelsPerLine );
      bAllocatedImage = GAN_TRUE;
   }
   else
      pgiImage = gan_image_set_format_type_dims ( pgiImage, eFormat, eType, bWholeImage ? ui32LinesPerImageEle : uiInternalHeight, ui32PixelsPerLine );

   if ( pgiImage == NULL )
   {
      gan_err_register ( "pgiRead12BitDPXImageData", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* read the image data */
   switch(eFormat)
   {
      case GAN_RGB_COLOUR_IMAGE:
        for(uiRow = 0; uiRow<ui32LinesPerImageEle; uiRow++)
        {
           Gan_RGBPixel_ui16* pui16rgbPix;
           gan_uint32* pui32Pix;

           if(bSingleField)
           {
              /* only transfer even rows for upper field, or odd rows for upper field */
              if((bUpper && (uiRow % 2) == 0) || (!bUpper && (uiRow % 2) == 1))
              {
                 if ( fread ( acAlignedBuffer, 1, uiRowSizeInBytes, pfInFile ) != uiRowSizeInBytes )
                 {
                    if(bAllocatedImage) gan_image_free(pgiImage);
                    gan_err_flush_trace();
                    gan_err_register ( "pgiRead12BitDPXImageData", GAN_ERROR_TRUNCATED_FILE, "truncated DPX file" );
                    return NULL;
                 }

                 for(iCol=(int)ui32PixelsPerLine-1,
                     pui16rgbPix = gan_image_get_pixptr_rgb_ui16(pgiImage, bWholeImage ? (bFlip ? (ui32LinesPerImageEle-uiRow-1) : uiRow) : (bFlip ? (uiInternalHeight-uiRow/2-1) : uiRow/2), iCol),
                     pui32Pix = ((gan_uint32*)acAlignedBuffer) + iCol; iCol >= 0; iCol--, pui16rgbPix--, pui32Pix--)
                 {
                    if(bReversedEndianness)
                       vReverseEndianness32(pui32Pix);

                    pui16rgbPix->R = (gan_uint16)(((*pui32Pix) & 0xffc00000) >> 16);
                    pui16rgbPix->G = (gan_uint16)(((*pui32Pix) &   0x3ff000) >>  6);
                    pui16rgbPix->B = (gan_uint16)(((*pui32Pix) &      0xffc) <<  4);
                 }
              }
              /* otherwise ignore this scanline */
              else if(fseek(pfInFile, uiRowSizeInBytes, SEEK_CUR) != 0)
              {
                 if(bAllocatedImage) gan_image_free(pgiImage);
                 gan_err_flush_trace();
                 gan_err_register ( "pgiRead12BitDPXImageData", GAN_ERROR_TRUNCATED_FILE, "truncated DPX file" );
                 return NULL;
              }
           }
           else
           {
              if ( fread ( acAlignedBuffer, 1, uiRowSizeInBytes, pfInFile ) != uiRowSizeInBytes )
              {
                 if(bAllocatedImage) gan_image_free(pgiImage);
                 gan_err_flush_trace();
                 gan_err_register ( "pgiRead12BitDPXImageData", GAN_ERROR_TRUNCATED_FILE, "truncated DPX file" );
                 return NULL;
              }

              for(iCol=(int)ui32PixelsPerLine-1,
                  pui16rgbPix = gan_image_get_pixptr_rgb_ui16(pgiImage, bFlip ? (ui32LinesPerImageEle-uiRow-1) : uiRow, iCol),
                  pui32Pix = ((gan_uint32*)acAlignedBuffer) + iCol; iCol >= 0; iCol--, pui16rgbPix--, pui32Pix--)
              {
                 if(bReversedEndianness)
                    vReverseEndianness32(pui32Pix);

                 pui16rgbPix->R = (gan_uint16)(((*pui32Pix) & 0xffc00000) >> 16);
                 pui16rgbPix->G = (gan_uint16)(((*pui32Pix) &   0x3ff000) >>  6);
                 pui16rgbPix->B = (gan_uint16)(((*pui32Pix) &      0xffc) <<  4);
              }
           }

           // allow for padding at end of line
           if(fseek(pfInFile, ui32eolPadding, SEEK_CUR) != 0)
           {
              if(bAllocatedImage) gan_image_free(pgiImage);
              gan_err_flush_trace();
              gan_err_register ( "pgiRead12BitDPXImageData", GAN_ERROR_CORRUPTED_FILE, "corrupted DPX data (truncated file?)" );
              return NULL;
           }
        }

        break;

      default:
        if(bAllocatedImage) gan_image_free(pgiImage);
        gan_err_flush_trace();
        gan_err_register ( "pgiRead12BitDPXImageData", GAN_ERROR_NOT_IMPLEMENTED, "image format not supported" );
        return NULL;
   }        

   free(acBuffer);

   // allow for padding at end of image
   if(fseek(pfInFile, ui32eoImagePadding, SEEK_CUR) != 0)
   {
      if(bAllocatedImage) gan_image_free(pgiImage);
      gan_err_flush_trace();
      gan_err_register ( "pgiRead12BitDPXImageData", GAN_ERROR_CORRUPTED_FILE, "corrupted DPX data (truncated file?)" );
      return NULL;
   }

   return pgiImage;
}

/**
 * \brief Not a user function
 */
Gan_Image *
 pgiRead16BitDPXImageData(FILE* pfInFile, Gan_Bool bReversedEndianness,
                          gan_uint32 ui32eolPadding, gan_uint32 ui32eoImagePadding,
                          Gan_ImageFormat eFormat, Gan_Type eType,
                          gan_uint32 ui32PixelsPerLine, gan_uint32 ui32LinesPerImageEle,
                          Gan_Image* pgiImage, Gan_Bool bFlip, Gan_Bool bSingleField, Gan_Bool bUpper, Gan_Bool bWholeImage)
{
   Gan_Bool bAllocatedImage=GAN_FALSE;
   unsigned int uiRow;
   int iCol;
   unsigned int uiRowSizeInBytes, uiInternalHeight;
   char *acBuffer, *acAlignedBuffer;

   if(eType != GAN_UINT16)
   {
      gan_err_flush_trace();
      gan_err_register ( "pgiRead16BitDPXImageData", GAN_ERROR_NOT_IMPLEMENTED, "no 16 bit conversions supported" );
      return NULL;
   }

   /* interlaced frames must have even height */
   if(bSingleField)
   {
      if((ui32LinesPerImageEle % 2) != 0)
      {
         gan_err_flush_trace();
         gan_err_register ( "pgiRead16BitDPXImageData", GAN_ERROR_INCOMPATIBLE, "" );
         return NULL;
      }

      uiInternalHeight = ui32LinesPerImageEle/2;
   }
   else
      uiInternalHeight = ui32LinesPerImageEle;

   /* determine row data size in bytes */
   switch(eFormat)
   {
      case GAN_GREY_LEVEL_IMAGE:
        uiRowSizeInBytes = 2*ui32PixelsPerLine;
        break;

      case GAN_RGB_COLOUR_IMAGE:
        uiRowSizeInBytes = 6*ui32PixelsPerLine;
        break;

      case GAN_RGB_COLOUR_ALPHA_IMAGE:
        uiRowSizeInBytes = 8*ui32PixelsPerLine;
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "pgiRead16BitDPXImageData", GAN_ERROR_NOT_IMPLEMENTED, "image format not supported" );
        return NULL;
   }        

   /* build a buffer to hold a single line of image data and align it to a four byte boundary*/
   acBuffer = malloc(uiRowSizeInBytes+3);
   if(acBuffer == NULL)
   {
      gan_err_flush_trace();
      gan_err_register_with_number ( "pgiRead16BitDPXImageData", GAN_ERROR_MALLOC_FAILED, "", uiRowSizeInBytes+3 );
      return NULL;
   }

   acAlignedBuffer = (char*)((unsigned long int)acBuffer + 3 - (((unsigned long int)acBuffer + 3) % 4));

   /* allocate the image */
   if ( pgiImage == NULL )
   {
      pgiImage = gan_image_alloc(eFormat, eType, bWholeImage ? ui32LinesPerImageEle : uiInternalHeight, ui32PixelsPerLine );
      bAllocatedImage = GAN_TRUE;
   }
   else
      pgiImage = gan_image_set_format_type_dims ( pgiImage, eFormat, eType, bWholeImage ? ui32LinesPerImageEle : uiInternalHeight, ui32PixelsPerLine );

   if ( pgiImage == NULL )
   {
      gan_err_register ( "pgiRead16BitDPXImageData", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* read the image data */
   switch(eFormat)
   {
      case GAN_GREY_LEVEL_IMAGE:
        for(uiRow = 0; uiRow<ui32LinesPerImageEle; uiRow++)
        {
           gan_uint16 *pui16ImPix, *pui16BufPix;

           if(bSingleField)
           {
              /* only transfer even rows for upper field, or odd rows for upper field */
              if((bUpper && (uiRow % 2) == 0) || (!bUpper && (uiRow % 2) == 1))
              {
                 if ( fread ( acAlignedBuffer, 1, uiRowSizeInBytes, pfInFile ) != uiRowSizeInBytes )
                 {
                    if(bAllocatedImage) gan_image_free(pgiImage);
                    gan_err_flush_trace();
                    gan_err_register ( "pgiRead16BitDPXImageData", GAN_ERROR_TRUNCATED_FILE, "truncated DPX file" );
                    return NULL;
                 }
                 
                 if(bReversedEndianness)
                 {
                    for(iCol=(int)ui32PixelsPerLine-1,
                        pui16ImPix = gan_image_get_pixptr_gl_ui16(pgiImage, bWholeImage ? (bFlip ? (ui32LinesPerImageEle-uiRow-1) : uiRow) : (bFlip ? (uiInternalHeight-uiRow/2-1) : uiRow/2), iCol),
                        pui16BufPix = ((gan_uint16*)acAlignedBuffer) + iCol; iCol >= 0;
                        iCol--, pui16ImPix--, pui16BufPix--)
                    {
                       *pui16ImPix = *pui16BufPix;
                       vReverseEndianness16(pui16ImPix);
                    }
                 }
                 else
                    for(iCol=(int)ui32PixelsPerLine-1,
                        pui16ImPix = gan_image_get_pixptr_gl_ui16(pgiImage, bWholeImage ? (bFlip ? (ui32LinesPerImageEle-uiRow-1) : uiRow) : (bFlip ? (uiInternalHeight-uiRow/2-1) : uiRow/2), iCol),
                        pui16BufPix = ((gan_uint16*)acAlignedBuffer) + iCol; iCol >= 0; iCol--)
                       *pui16ImPix-- = *pui16BufPix--;
              }
              /* otherwise ignore this scanline */
              else if(fseek(pfInFile, uiRowSizeInBytes, SEEK_CUR) != 0)
              {
                 if(bAllocatedImage) gan_image_free(pgiImage);
                 gan_err_flush_trace();
                 gan_err_register ( "pgiRead16BitDPXImageData", GAN_ERROR_TRUNCATED_FILE, "truncated DPX file" );
                 return NULL;
              }
           }
           else
           {
              if ( fread ( acAlignedBuffer, 1, uiRowSizeInBytes, pfInFile ) != uiRowSizeInBytes )
              {
                 if(bAllocatedImage) gan_image_free(pgiImage);
                 gan_err_flush_trace();
                 gan_err_register ( "pgiRead16BitDPXImageData", GAN_ERROR_TRUNCATED_FILE, "truncated DPX file" );
                 return NULL;
              }

              if(bReversedEndianness)
              {
                 for(iCol=(int)ui32PixelsPerLine-1,
                     pui16ImPix = gan_image_get_pixptr_gl_ui16(pgiImage, bFlip ? (ui32LinesPerImageEle-uiRow-1) : uiRow, iCol),
                     pui16BufPix = ((gan_uint16*)acAlignedBuffer) + iCol; iCol >= 0; iCol--, pui16ImPix--, pui16BufPix--)
                 {
                    *pui16ImPix = *pui16BufPix;
                    vReverseEndianness16(pui16ImPix);
                 }
              }
              else
                 for(iCol=(int)ui32PixelsPerLine-1,
                     pui16ImPix = gan_image_get_pixptr_gl_ui16(pgiImage, bFlip ? (ui32LinesPerImageEle-uiRow-1) : uiRow, iCol),
                     pui16BufPix = ((gan_uint16*)acAlignedBuffer) + iCol; iCol >= 0; iCol--)
                    *pui16ImPix-- = *pui16BufPix--;
           }

           // allow for padding at end of line
           if(fseek(pfInFile, ui32eolPadding, SEEK_CUR) != 0)
           {
              if(bAllocatedImage) gan_image_free(pgiImage);
              gan_err_flush_trace();
              gan_err_register ( "pgiRead16BitDPXImageData", GAN_ERROR_CORRUPTED_FILE, "corrupted DPX data (truncated file?)" );
              return NULL;
           }
        }

        break;

      case GAN_RGB_COLOUR_IMAGE:
        for(uiRow = 0; uiRow<ui32LinesPerImageEle; uiRow++)
        {
           Gan_RGBPixel_ui16 *pui16rgbImPix, *pui16rgbBufPix;

           if(bSingleField)
           {
              /* only transfer even rows for upper field, or odd rows for upper field */
              if((bUpper && (uiRow % 2) == 0) || (!bUpper && (uiRow % 2) == 1))
              {
                 if ( fread ( acAlignedBuffer, 1, uiRowSizeInBytes, pfInFile ) != uiRowSizeInBytes )
                 {
                    if(bAllocatedImage) gan_image_free(pgiImage);
                    gan_err_flush_trace();
                    gan_err_register ( "pgiRead16BitDPXImageData", GAN_ERROR_TRUNCATED_FILE, "truncated DPX file" );
                    return NULL;
                 }
                 
                 if(bReversedEndianness)
                 {
                    for(iCol=(int)ui32PixelsPerLine-1,
                        pui16rgbImPix = gan_image_get_pixptr_rgb_ui16(pgiImage, bWholeImage ? (bFlip ? (ui32LinesPerImageEle-uiRow-1) : uiRow) : (bFlip ? (uiInternalHeight-uiRow/2-1) : uiRow/2), iCol),
                        pui16rgbBufPix = ((Gan_RGBPixel_ui16*)acAlignedBuffer) + iCol; iCol >= 0; iCol--, pui16rgbImPix--, pui16rgbBufPix--)
                    {
                       *pui16rgbImPix = *pui16rgbBufPix;
                       vReverseEndianness16(&pui16rgbImPix->R);
                       vReverseEndianness16(&pui16rgbImPix->G);
                       vReverseEndianness16(&pui16rgbImPix->B);
                    }
                 }
                 else
                    for(iCol=(int)ui32PixelsPerLine-1,
                        pui16rgbImPix = gan_image_get_pixptr_rgb_ui16(pgiImage, bWholeImage ? (bFlip ? (ui32LinesPerImageEle-uiRow-1) : uiRow) : (bFlip ? (uiInternalHeight-uiRow/2-1) : uiRow/2), iCol),
                        pui16rgbBufPix = ((Gan_RGBPixel_ui16*)acAlignedBuffer) + iCol; iCol >= 0; iCol--)
                       *pui16rgbImPix-- = *pui16rgbBufPix--;
              }
              /* otherwise ignore this scanline */
              else if(fseek(pfInFile, uiRowSizeInBytes, SEEK_CUR) != 0)
              {
                 if(bAllocatedImage) gan_image_free(pgiImage);
                 gan_err_flush_trace();
                 gan_err_register ( "pgiRead16BitDPXImageData", GAN_ERROR_TRUNCATED_FILE, "truncated DPX file" );
                 return NULL;
              }
           }
           else
           {
              if ( fread ( acAlignedBuffer, 1, uiRowSizeInBytes, pfInFile ) != uiRowSizeInBytes )
              {
                 if(bAllocatedImage) gan_image_free(pgiImage);
                 gan_err_flush_trace();
                 gan_err_register ( "pgiRead16BitDPXImageData", GAN_ERROR_TRUNCATED_FILE, "truncated DPX file" );
                 return NULL;
              }

              if(bReversedEndianness)
              {
                 for(iCol=(int)ui32PixelsPerLine-1,
                     pui16rgbImPix = gan_image_get_pixptr_rgb_ui16(pgiImage, bFlip ? (ui32LinesPerImageEle-uiRow-1) : uiRow, iCol),
                     pui16rgbBufPix = ((Gan_RGBPixel_ui16*)acAlignedBuffer) + iCol; iCol >= 0;
                     iCol--, pui16rgbImPix--, pui16rgbBufPix--)
                 {
                    *pui16rgbImPix = *pui16rgbBufPix;
                    vReverseEndianness16(&pui16rgbImPix->R);
                    vReverseEndianness16(&pui16rgbImPix->G);
                    vReverseEndianness16(&pui16rgbImPix->B);
                 }
              }
              else
                 for(iCol=(int)ui32PixelsPerLine-1,
                     pui16rgbImPix = gan_image_get_pixptr_rgb_ui16(pgiImage, bFlip ? (ui32LinesPerImageEle-uiRow-1) : uiRow, iCol),
                     pui16rgbBufPix = ((Gan_RGBPixel_ui16*)acAlignedBuffer) + iCol; iCol >= 0; iCol--)
                    *pui16rgbImPix-- = *pui16rgbBufPix--;
           }

           // allow for padding at end of line
           if(fseek(pfInFile, ui32eolPadding, SEEK_CUR) != 0)
           {
              if(bAllocatedImage) gan_image_free(pgiImage);
              gan_err_flush_trace();
              gan_err_register ( "pgiRead16BitDPXImageData", GAN_ERROR_CORRUPTED_FILE, "corrupted DPX data (truncated file?)" );
              return NULL;
           }
        }

        break;

      case GAN_RGB_COLOUR_ALPHA_IMAGE:
        for(uiRow = 0; uiRow<ui32LinesPerImageEle; uiRow++)
        {
           Gan_RGBAPixel_ui16 *pui16rgbaImPix, *pui16rgbaBufPix;

           if(bSingleField)
           {
              /* only transfer even rows for upper field, or odd rows for upper field */
              if((bUpper && (uiRow % 2) == 0) || (!bUpper && (uiRow % 2) == 1))
              {
                 if ( fread ( acAlignedBuffer, 1, uiRowSizeInBytes, pfInFile ) != uiRowSizeInBytes )
                 {
                    if(bAllocatedImage) gan_image_free(pgiImage);
                    gan_err_flush_trace();
                    gan_err_register ( "pgiRead16BitDPXImageData", GAN_ERROR_TRUNCATED_FILE, "truncated DPX file" );
                    return NULL;
                 }
                 
                 if(bReversedEndianness)
                 {
                    for(iCol=(int)ui32PixelsPerLine-1,
                        pui16rgbaImPix = gan_image_get_pixptr_rgba_ui16(pgiImage, bWholeImage ? (bFlip ? (ui32LinesPerImageEle-uiRow-1) : uiRow) : (bFlip ? (uiInternalHeight-uiRow/2-1) : uiRow/2), iCol),
                        pui16rgbaBufPix = ((Gan_RGBAPixel_ui16*)acAlignedBuffer) + iCol; iCol >= 0; iCol--, pui16rgbaImPix--, pui16rgbaBufPix--)
                    {
                       *pui16rgbaImPix = *pui16rgbaBufPix;
                       vReverseEndianness16(&pui16rgbaImPix->R);
                       vReverseEndianness16(&pui16rgbaImPix->G);
                       vReverseEndianness16(&pui16rgbaImPix->B);
                       vReverseEndianness16(&pui16rgbaImPix->A);
                    }
                 }
                 else
                    for(iCol=(int)ui32PixelsPerLine-1,
                        pui16rgbaImPix = gan_image_get_pixptr_rgba_ui16(pgiImage, bWholeImage ? (bFlip ? (ui32LinesPerImageEle-uiRow-1) : uiRow) : (bFlip ? (uiInternalHeight-uiRow/2-1) : uiRow/2), iCol),
                        pui16rgbaBufPix = ((Gan_RGBAPixel_ui16*)acAlignedBuffer) + iCol; iCol >= 0; iCol--)
                       *pui16rgbaImPix-- = *pui16rgbaBufPix--;
              }
              /* otherwise ignore this scanline */
              else if(fseek(pfInFile, uiRowSizeInBytes, SEEK_CUR) != 0)
              {
                 if(bAllocatedImage) gan_image_free(pgiImage);
                 gan_err_flush_trace();
                 gan_err_register ( "pgiRead16BitDPXImageData", GAN_ERROR_TRUNCATED_FILE, "truncated DPX file" );
                 return NULL;
              }
           }
           else
           {
              if ( fread ( acAlignedBuffer, 1, uiRowSizeInBytes, pfInFile ) != uiRowSizeInBytes )
              {
                 if(bAllocatedImage) gan_image_free(pgiImage);
                 gan_err_flush_trace();
                 gan_err_register ( "pgiRead16BitDPXImageData", GAN_ERROR_TRUNCATED_FILE, "truncated DPX file" );
                 return NULL;
              }

              if(bReversedEndianness)
              {
                 for(iCol=(int)ui32PixelsPerLine-1,
                     pui16rgbaImPix = gan_image_get_pixptr_rgba_ui16(pgiImage, bFlip ? (ui32LinesPerImageEle-uiRow-1) : uiRow, iCol),
                     pui16rgbaBufPix = ((Gan_RGBAPixel_ui16*)acAlignedBuffer) + iCol; iCol >= 0; iCol--, pui16rgbaImPix--, pui16rgbaBufPix--)
                 {
                    *pui16rgbaImPix = *pui16rgbaBufPix;
                    vReverseEndianness16(&pui16rgbaImPix->R);
                    vReverseEndianness16(&pui16rgbaImPix->G);
                    vReverseEndianness16(&pui16rgbaImPix->B);
                    vReverseEndianness16(&pui16rgbaImPix->A);
                 }
              }
              else
                 for(iCol=(int)ui32PixelsPerLine-1,
                     pui16rgbaImPix = gan_image_get_pixptr_rgba_ui16(pgiImage, bFlip ? (ui32LinesPerImageEle-uiRow-1) : uiRow, iCol),
                     pui16rgbaBufPix = ((Gan_RGBAPixel_ui16*)acAlignedBuffer) + iCol; iCol >= 0; iCol--)
                    *pui16rgbaImPix-- = *pui16rgbaBufPix--;
           }

           // allow for padding at end of line
           if(fseek(pfInFile, ui32eolPadding, SEEK_CUR) != 0)
           {
              if(bAllocatedImage) gan_image_free(pgiImage);
              gan_err_flush_trace();
              gan_err_register ( "pgiRead16BitDPXImageData", GAN_ERROR_CORRUPTED_FILE, "corrupted DPX data (truncated file?)" );
              return NULL;
           }
        }

        break;

      default:
        if(bAllocatedImage) gan_image_free(pgiImage);
        gan_err_flush_trace();
        gan_err_register ( "pgiRead16BitDPXImageData", GAN_ERROR_NOT_IMPLEMENTED, "unsupported format" );
        return NULL;
   }        

   free(acBuffer);

   // allow for padding at end of image
   if(fseek(pfInFile, ui32eoImagePadding, SEEK_CUR) != 0)
   {
      if(bAllocatedImage) gan_image_free(pgiImage);
      gan_err_flush_trace();
      gan_err_register ( "pgiRead16BitDPXImageData", GAN_ERROR_CORRUPTED_FILE, "corrupted DPX data (truncated file?)" );
      return NULL;
   }

   return pgiImage;
}

/**
 * \brief Not a user function
 */
Gan_Image *
 pgiRead32BitFloatDPXImageData(FILE* pfInFile, Gan_Bool bReversedEndianness,
                               gan_uint32 ui32eolPadding, gan_uint32 ui32eoImagePadding,
                               Gan_ImageFormat eFormat, Gan_Type eType,
                               gan_uint32 ui32PixelsPerLine, gan_uint32 ui32LinesPerImageEle,
                               Gan_Image* pgiImage, Gan_Bool bFlip, Gan_Bool bSingleField, Gan_Bool bUpper, Gan_Bool bWholeImage)
{
   Gan_Bool bAllocatedImage=GAN_FALSE;
   unsigned int uiRow;
   int iCol;
   unsigned int uiRowSizeInBytes, uiInternalHeight;
   char *acBuffer, *acAlignedBuffer;

   if(eType != GAN_FLOAT32)
   {
      gan_err_flush_trace();
      gan_err_register ( "pgiRead32BitFloatDPXImageData", GAN_ERROR_NOT_IMPLEMENTED, "no 8 bit conversions supported" );
      return NULL;
   }

   /* interlaced frames must have even height */
   if(bSingleField)
   {
      if((ui32LinesPerImageEle % 2) != 0)
      {
         gan_err_flush_trace();
         gan_err_register ( "pgiRead32BitFloatDPXImageData", GAN_ERROR_INCOMPATIBLE, "" );
         return NULL;
      }

      uiInternalHeight = ui32LinesPerImageEle/2;
   }
   else
      uiInternalHeight = ui32LinesPerImageEle;

   /* determine row data size in bytes */
   switch(eFormat)
   {
      case GAN_GREY_LEVEL_IMAGE:
        uiRowSizeInBytes = 4*ui32PixelsPerLine;
        break;

      case GAN_RGB_COLOUR_IMAGE:
        uiRowSizeInBytes = 12*ui32PixelsPerLine;
        break;

      case GAN_RGB_COLOUR_ALPHA_IMAGE:
        uiRowSizeInBytes = 16*ui32PixelsPerLine;
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "pgiRead32BitFloatDPXImageData", GAN_ERROR_NOT_IMPLEMENTED, "image format not supported" );
        return NULL;
   }        

   /* build a buffer to hold a single line of image data and align it to a four byte boundary*/
   acBuffer = malloc(uiRowSizeInBytes+3);
   if(acBuffer == NULL)
   {
      gan_err_flush_trace();
      gan_err_register_with_number ( "pgiRead32BitFloatDPXImageData", GAN_ERROR_MALLOC_FAILED, "", uiRowSizeInBytes+3 );
      return NULL;
   }

   acAlignedBuffer = (char*)((unsigned long int)acBuffer + 3 - (((unsigned long int)acBuffer + 3) % 4));

   /* allocate the image */
   if ( pgiImage == NULL )
   {
      pgiImage = gan_image_alloc(eFormat, eType, bWholeImage ? ui32LinesPerImageEle : uiInternalHeight, ui32PixelsPerLine );
      bAllocatedImage = GAN_TRUE;
   }
   else
      pgiImage = gan_image_set_format_type_dims ( pgiImage, eFormat, eType, bWholeImage ? ui32LinesPerImageEle : uiInternalHeight, ui32PixelsPerLine );

   if ( pgiImage == NULL )
   {
      gan_err_register ( "pgiRead32BitFloatDPXImageData", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* read the image data */
   switch(eFormat)
   {
      case GAN_GREY_LEVEL_IMAGE:
        for(uiRow = 0; uiRow<ui32LinesPerImageEle; uiRow++)
        {
           gan_float32 *pf32ImPix, *pf32BufPix;

           if(bSingleField)
           {
              /* only transfer even rows for upper field, or odd rows for upper field */
              if((bUpper && (uiRow % 2) == 0) || (!bUpper && (uiRow % 2) == 1))
              {
                 if ( fread ( acAlignedBuffer, 1, uiRowSizeInBytes, pfInFile ) != uiRowSizeInBytes )
                 {
                    if(bAllocatedImage) gan_image_free(pgiImage);
                    gan_err_flush_trace();
                    gan_err_register ( "pgiRead16BitDPXImageData", GAN_ERROR_TRUNCATED_FILE, "truncated DPX file" );
                    return NULL;
                 }
                 
                 if(bReversedEndianness)
                 {
                    for(iCol=(int)ui32PixelsPerLine-1,
                        pf32ImPix = gan_image_get_pixptr_gl_f32(pgiImage, bWholeImage ? (bFlip ? (ui32LinesPerImageEle-uiRow-1) : uiRow) : (bFlip ? (uiInternalHeight-uiRow/2-1) : uiRow/2), iCol),
                        pf32BufPix = ((gan_float32*)acAlignedBuffer) + iCol; iCol >= 0; iCol--, pf32ImPix--, pf32BufPix--)
                    {
                       *pf32ImPix = *pf32BufPix;
                       vReverseEndianness32((gan_uint32*)pf32ImPix);
                    }
                 }
                 else
                    for(iCol=(int)ui32PixelsPerLine-1,
                        pf32ImPix = gan_image_get_pixptr_gl_f32(pgiImage, bWholeImage ? (bFlip ? (ui32LinesPerImageEle-uiRow-1) : uiRow) : (bFlip ? (uiInternalHeight-uiRow/2-1) : uiRow/2), iCol),
                        pf32BufPix = ((gan_float32*)acAlignedBuffer) + iCol; iCol >= 0; iCol--)
                       *pf32ImPix-- = *pf32BufPix--;
              }
              /* otherwise ignore this scanline */
              else if(fseek(pfInFile, uiRowSizeInBytes, SEEK_CUR) != 0)
              {
                 if(bAllocatedImage) gan_image_free(pgiImage);
                 gan_err_flush_trace();
                 gan_err_register ( "pgiRead32BitFloatDPXImageData", GAN_ERROR_TRUNCATED_FILE, "truncated DPX file" );
                 return NULL;
              }
           }
           else
           {
              if ( fread ( acAlignedBuffer, 1, uiRowSizeInBytes, pfInFile ) != uiRowSizeInBytes )
              {
                 if(bAllocatedImage) gan_image_free(pgiImage);
                 gan_err_flush_trace();
                 gan_err_register ( "pgiRead32BitFloatDPXImageData", GAN_ERROR_TRUNCATED_FILE, "truncated DPX file" );
                 return NULL;
              }

              if(bReversedEndianness)
              {
                 for(iCol=(int)ui32PixelsPerLine-1,
                     pf32ImPix = gan_image_get_pixptr_gl_f32(pgiImage, bFlip ? (ui32LinesPerImageEle-uiRow-1) : uiRow, iCol),
                     pf32BufPix = ((gan_float32*)acAlignedBuffer) + iCol; iCol >= 0; iCol--, pf32ImPix--, pf32BufPix--)
                 {
                    *pf32ImPix = *pf32BufPix;
                    vReverseEndianness32((gan_uint32*)pf32ImPix);
                 }
              }
              else
                 for(iCol=(int)ui32PixelsPerLine-1,
                     pf32ImPix = gan_image_get_pixptr_gl_f32(pgiImage, bFlip ? (ui32LinesPerImageEle-uiRow-1) : uiRow, iCol),
                     pf32BufPix = ((gan_float32*)acAlignedBuffer) + iCol; iCol >= 0; iCol--)
                    *pf32ImPix-- = *pf32BufPix--;
           }

           // allow for padding at end of line
           if(fseek(pfInFile, ui32eolPadding, SEEK_CUR) != 0)
           {
              if(bAllocatedImage) gan_image_free(pgiImage);
              gan_err_flush_trace();
              gan_err_register ( "pgiRead32BitFloatDPXImageData", GAN_ERROR_CORRUPTED_FILE, "corrupted DPX data (truncated file?)" );
              return NULL;
           }
        }

        break;

      case GAN_RGB_COLOUR_IMAGE:
        for(uiRow = 0; uiRow<ui32LinesPerImageEle; uiRow++)
        {
           Gan_RGBPixel_f32 *pf32rgbImPix, *pf32rgbBufPix;

           if(bSingleField)
           {
              /* only transfer even rows for upper field, or odd rows for upper field */
              if((bUpper && (uiRow % 2) == 0) || (!bUpper && (uiRow % 2) == 1))
              {
                 if ( fread ( acAlignedBuffer, 1, uiRowSizeInBytes, pfInFile ) != uiRowSizeInBytes )
                 {
                    if(bAllocatedImage) gan_image_free(pgiImage);
                    gan_err_flush_trace();
                    gan_err_register ( "pgiRead32BitFloatDPXImageData", GAN_ERROR_TRUNCATED_FILE, "truncated DPX file" );
                    return NULL;
                 }
                 
                 if(bReversedEndianness)
                 {
                    for(iCol=(int)ui32PixelsPerLine-1,
                        pf32rgbImPix = gan_image_get_pixptr_rgb_f32(pgiImage, bWholeImage ? (bFlip ? (ui32LinesPerImageEle-uiRow-1) : uiRow) : (bFlip ? (uiInternalHeight-uiRow/2-1) : uiRow/2), iCol),
                        pf32rgbBufPix = ((Gan_RGBPixel_f32*)acAlignedBuffer) + iCol; iCol >= 0; iCol--, pf32rgbImPix--, pf32rgbBufPix--)
                    {
                       *pf32rgbImPix = *pf32rgbBufPix;
                       vReverseEndianness32((gan_uint32*)&pf32rgbImPix->R);
                       vReverseEndianness32((gan_uint32*)&pf32rgbImPix->G);
                       vReverseEndianness32((gan_uint32*)&pf32rgbImPix->B);
                    }
                 }
                 else
                    for(iCol=(int)ui32PixelsPerLine-1,
                        pf32rgbImPix = gan_image_get_pixptr_rgb_f32(pgiImage, bWholeImage ? (bFlip ? (ui32LinesPerImageEle-uiRow-1) : uiRow) : (bFlip ? (uiInternalHeight-uiRow/2-1) : uiRow/2), iCol),
                        pf32rgbBufPix = ((Gan_RGBPixel_f32*)acAlignedBuffer) + iCol; iCol >= 0; iCol--)
                       *pf32rgbImPix-- = *pf32rgbBufPix--;
              }
              /* otherwise ignore this scanline */
              else if(fseek(pfInFile, uiRowSizeInBytes, SEEK_CUR) != 0)
              {
                 if(bAllocatedImage) gan_image_free(pgiImage);
                 gan_err_flush_trace();
                 gan_err_register ( "pgiRead32BitFloatDPXImageData", GAN_ERROR_TRUNCATED_FILE, "truncated DPX file" );
                 return NULL;
              }
           }
           else
           {
              if ( fread ( acAlignedBuffer, 1, uiRowSizeInBytes, pfInFile ) != uiRowSizeInBytes )
              {
                 if(bAllocatedImage) gan_image_free(pgiImage);
                 gan_err_flush_trace();
                 gan_err_register ( "pgiRead32BitFloatDPXImageData", GAN_ERROR_TRUNCATED_FILE, "truncated DPX file" );
                 return NULL;
              }

              if(bReversedEndianness)
              {
                 for(iCol=(int)ui32PixelsPerLine-1,
                     pf32rgbImPix = gan_image_get_pixptr_rgb_f32(pgiImage, bFlip ? (ui32LinesPerImageEle-uiRow-1) : uiRow, iCol),
                     pf32rgbBufPix = ((Gan_RGBPixel_f32*)acAlignedBuffer) + iCol; iCol >= 0; iCol--, pf32rgbImPix--, pf32rgbBufPix--)
                 {
                    *pf32rgbImPix = *pf32rgbBufPix;
                    vReverseEndianness32((gan_uint32*)&pf32rgbImPix->R);
                    vReverseEndianness32((gan_uint32*)&pf32rgbImPix->G);
                    vReverseEndianness32((gan_uint32*)&pf32rgbImPix->B);
                 }
              }
              else
                 for(iCol=(int)ui32PixelsPerLine-1,
                     pf32rgbImPix = gan_image_get_pixptr_rgb_f32(pgiImage, bFlip ? (ui32LinesPerImageEle-uiRow-1) : uiRow, iCol),
                     pf32rgbBufPix = ((Gan_RGBPixel_f32*)acAlignedBuffer) + iCol; iCol >= 0; iCol--)
                    *pf32rgbImPix-- = *pf32rgbBufPix--;
           }

           // allow for padding at end of line
           if(fseek(pfInFile, ui32eolPadding, SEEK_CUR) != 0)
           {
              if(bAllocatedImage) gan_image_free(pgiImage);
              gan_err_flush_trace();
              gan_err_register ( "pgiRead32BitFloatDPXImageData", GAN_ERROR_CORRUPTED_FILE, "corrupted DPX data (truncated file?)" );
              return NULL;
           }
        }

        break;

      case GAN_RGB_COLOUR_ALPHA_IMAGE:
        for(uiRow = 0; uiRow<ui32LinesPerImageEle; uiRow++)
        {
           Gan_RGBAPixel_f32 *pf32rgbaImPix, *pf32rgbaBufPix;

           if(bSingleField)
           {
              /* only transfer even rows for upper field, or odd rows for upper field */
              if((bUpper && (uiRow % 2) == 0) || (!bUpper && (uiRow % 2) == 1))
              {
                 if ( fread ( acAlignedBuffer, 1, uiRowSizeInBytes, pfInFile ) != uiRowSizeInBytes )
                 {
                    if(bAllocatedImage) gan_image_free(pgiImage);
                    gan_err_flush_trace();
                    gan_err_register ( "pgiRead32BitFloatDPXImageData", GAN_ERROR_TRUNCATED_FILE, "truncated DPX file" );
                    return NULL;
                 }
                 
                 if(bReversedEndianness)
                 {
                    for(iCol=(int)ui32PixelsPerLine-1,
                        pf32rgbaImPix = gan_image_get_pixptr_rgba_f32(pgiImage, bWholeImage ? (bFlip ? (ui32LinesPerImageEle-uiRow-1) : uiRow) : (bFlip ? (uiInternalHeight-uiRow/2-1) : uiRow/2), iCol),
                        pf32rgbaBufPix = ((Gan_RGBAPixel_f32*)acAlignedBuffer) + iCol; iCol >= 0; iCol--, pf32rgbaImPix--, pf32rgbaBufPix--)
                    {
                       *pf32rgbaImPix = *pf32rgbaBufPix;
                       vReverseEndianness32((gan_uint32*)&pf32rgbaImPix->R);
                       vReverseEndianness32((gan_uint32*)&pf32rgbaImPix->G);
                       vReverseEndianness32((gan_uint32*)&pf32rgbaImPix->B);
                       vReverseEndianness32((gan_uint32*)&pf32rgbaImPix->A);
                    }
                 }
                 else
                 {
                    for(iCol=(int)ui32PixelsPerLine-1,
                        pf32rgbaImPix = gan_image_get_pixptr_rgba_f32(pgiImage, bWholeImage ? (bFlip ? (ui32LinesPerImageEle-uiRow-1) : uiRow) : (bFlip ? (uiInternalHeight-uiRow/2-1) : uiRow/2), iCol),
                        pf32rgbaBufPix = ((Gan_RGBAPixel_f32*)acAlignedBuffer) + iCol; iCol >= 0; iCol--)
                       *pf32rgbaImPix-- = *pf32rgbaBufPix--;
                 }
              }
              /* otherwise ignore this scanline */
              else if(fseek(pfInFile, uiRowSizeInBytes, SEEK_CUR) != 0)
              {
                 if(bAllocatedImage) gan_image_free(pgiImage);
                 gan_err_flush_trace();
                 gan_err_register ( "pgiRead32BitFloatDPXImageData", GAN_ERROR_TRUNCATED_FILE, "truncated DPX file" );
                 return NULL;
              }
           }
           else
           {
              if ( fread ( acAlignedBuffer, 1, uiRowSizeInBytes, pfInFile ) != uiRowSizeInBytes )
              {
                 if(bAllocatedImage) gan_image_free(pgiImage);
                 gan_err_flush_trace();
                 gan_err_register ( "pgiRead32BitFloatDPXImageData", GAN_ERROR_TRUNCATED_FILE, "truncated DPX file" );
                 return NULL;
              }

              if(bReversedEndianness)
              {
                 for(iCol=(int)ui32PixelsPerLine-1,
                     pf32rgbaImPix = gan_image_get_pixptr_rgba_f32(pgiImage, bFlip ? (ui32LinesPerImageEle-uiRow-1) : uiRow, iCol),
                     pf32rgbaBufPix = ((Gan_RGBAPixel_f32*)acAlignedBuffer) + iCol; iCol >= 0; iCol--, pf32rgbaImPix--, pf32rgbaBufPix--)
                 {
                    *pf32rgbaImPix = *pf32rgbaBufPix;
                    vReverseEndianness32((gan_uint32*)&pf32rgbaImPix->R);
                    vReverseEndianness32((gan_uint32*)&pf32rgbaImPix->G);
                    vReverseEndianness32((gan_uint32*)&pf32rgbaImPix->B);
                    vReverseEndianness32((gan_uint32*)&pf32rgbaImPix->A);
                 }
              }
              else
              {
                 for(iCol=(int)ui32PixelsPerLine-1,
                     pf32rgbaImPix = gan_image_get_pixptr_rgba_f32(pgiImage, bFlip ? (ui32LinesPerImageEle-uiRow-1) : uiRow, iCol),
                     pf32rgbaBufPix = ((Gan_RGBAPixel_f32*)acAlignedBuffer) + iCol; iCol >= 0; iCol--)
                    *pf32rgbaImPix-- = *pf32rgbaBufPix--;
              }
           }

           // allow for padding at end of line
           if(fseek(pfInFile, ui32eolPadding, SEEK_CUR) != 0)
           {
              if(bAllocatedImage) gan_image_free(pgiImage);
              gan_err_flush_trace();
              gan_err_register ( "pgiRead32BitFloatDPXImageData", GAN_ERROR_CORRUPTED_FILE, "corrupted DPX data (truncated file?)" );
              return NULL;
           }
        }

        break;

      default:
        if(bAllocatedImage) gan_image_free(pgiImage);
        gan_err_flush_trace();
        gan_err_register ( "pgiRead32BitFloatDPXImageData", GAN_ERROR_NOT_IMPLEMENTED, "unsupported format" );
        return NULL;
   }        

   free(acBuffer);

   // allow for padding at end of image
   if(fseek(pfInFile, ui32eoImagePadding, SEEK_CUR) != 0)
   {
      if(bAllocatedImage) gan_image_free(pgiImage);
      gan_err_flush_trace();
      gan_err_register ( "pgiRead32BitFloatDPXImageData", GAN_ERROR_CORRUPTED_FILE, "corrupted DPX data (truncated file?)" );
      return NULL;
   }

   return pgiImage;
}

/**
 * \brief Determines whether an image is a DPX image using the magic number.
 */
Gan_Bool gan_image_is_dpx(const unsigned char *magic_string, size_t length)
{
  if (length < 4) return(GAN_FALSE);

  if (memcmp(magic_string,"SDPX",4) == 0 || memcmp(magic_string,"XPDS",4) == 0)
    return(GAN_TRUE);

  return(GAN_FALSE);
}

static Gan_Bool gan_read_dpx_generic_header ( char *acAlignedHeader, FILE *infile,
                                              Gan_Bool *pbReversedEndianness, gan_uint32 *pui32ImageOffset,
                                              struct Gan_ImageHeaderStruct *header )
{
   gan_uint32 ui32Magic;
   gan_uint32 ui32GenericHeaderSize;  /* Generic header size */
   gan_uint32 ui32IndustryHeaderSize; /* Generic header size */

   if(fread(acAlignedHeader, 1, 768, infile) != 768)
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_read_dpx_generic_header", GAN_ERROR_CORRUPTED_FILE, "corrupted DPX file header (truncated file?)" );
      return GAN_FALSE;
   }

   /* determine endianness from magic number */
   ui32Magic = *((gan_uint32*)(acAlignedHeader + OFFSET_MAGIC));
   if(ui32Magic == 0x53445058)
      *pbReversedEndianness = GAN_FALSE;
   else if(ui32Magic == 0x58504453)
      *pbReversedEndianness = GAN_TRUE;
   else
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_read_dpx_generic_header", GAN_ERROR_CORRUPTED_FILE, "corrupted DPX file header (illegal magic number)" );
      return GAN_FALSE;
   }

   /* get offset of image data */
   *pui32ImageOffset = *((gan_uint32*)(acAlignedHeader + OFFSET_IMAGEOFFSET));
   if(*pbReversedEndianness)
      vReverseEndianness32(pui32ImageOffset);

   /* get header sizes */
   ui32GenericHeaderSize  = *((gan_uint32*)(acAlignedHeader + OFFSET_GENERICSIZE));
   ui32IndustryHeaderSize = *((gan_uint32*)(acAlignedHeader + OFFSET_INDUSTRYSIZE));
   if(*pbReversedEndianness)
   {
      vReverseEndianness32(&ui32GenericHeaderSize);
      vReverseEndianness32(&ui32IndustryHeaderSize);
   }

   if(header != NULL)
   {
      strncpy(header->info.dpx.generic.version, (char *)(acAlignedHeader + OFFSET_VERSION), 8);
      header->info.dpx.generic.ditto_key = *((gan_uint32*)(acAlignedHeader + OFFSET_DITTOKEY));
      strncpy(header->info.dpx.generic.file_name, (char *)(acAlignedHeader + OFFSET_FILENAME), 100);
      strncpy(header->info.dpx.generic.create_time, (char *)(acAlignedHeader + OFFSET_CREATETIME), 24);
      strncpy(header->info.dpx.generic.creator, (char *)(acAlignedHeader + OFFSET_CREATOR), 100);
      strncpy(header->info.dpx.generic.project, (char *)(acAlignedHeader + OFFSET_PROJECT), 200);
      strncpy(header->info.dpx.generic.copyright, (char *)(acAlignedHeader + OFFSET_COPYRIGHT), 200);
      header->info.dpx.generic.encryption_key = *((gan_uint32*)(acAlignedHeader + OFFSET_ENCRYPTKEY));
      if(*pbReversedEndianness)
      {
         vReverseEndianness32(&header->info.dpx.generic.ditto_key);
         vReverseEndianness32(&header->info.dpx.generic.encryption_key);
      }
   }
      
   return GAN_TRUE;
}

static Gan_Bool gan_read_dpx_image_information_header ( char *acAlignedHeader, FILE *infile, Gan_Bool bReversedEndianness,
                                                        gan_uint32 ui32ImageOffset,
                                                        gan_uint16 *pui16Orientation, /* image orientation */
                                                        gan_uint32 *pui32PixelsPerLine,
                                                        gan_uint32 *pui32LinesPerImageEle,
                                                        Gan_ImageFormat *peFormat, Gan_Type *peType,
                                                        gan_uint8 *pui8BitSize, /* bit size for element */
                                                        Gan_Bool *pbPacked,
                                                        gan_uint32 *pui32eolPadding, /* end of line padding used in element */
                                                        gan_uint32 *pui32eoImagePadding, /* end of image padding used in element */
                                                        struct Gan_ImageHeaderStruct *header )
{
   gan_uint8  ui8DataSign;
   gan_uint32 ui32RefLowData;
   gan_float32 f32RefLowQuantity;
   gan_uint32 ui32RefHighData;
   gan_float32 f32RefHighQuantity;
   gan_uint16 ui16ElementNumber;        /* number of image elements */
   gan_uint8  ui8Descriptor;            /* descriptor for image element */
   gan_uint16 ui16Packing;              /* packing for element */
   gan_uint16 ui16Encoding;             /* encoding for element */
   gan_uint32 ui32DataOffset;           /* offset to data of element */

   if(fread(acAlignedHeader, 1, 640, infile) != 640)
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_read_dpx_image_information_header", GAN_ERROR_CORRUPTED_FILE, "corrupted DPX image information header (truncated file?)" );
      return GAN_FALSE;
   }

   /* decode image information header stuff */
   *pui16Orientation = *((gan_uint16*)(acAlignedHeader + OFFSET_ORIENTATION));
   if(bReversedEndianness)
      vReverseEndianness16(pui16Orientation);

   /* we only support left-to-right orientation */
   if((*pui16Orientation) != 0 && (*pui16Orientation) != 2)
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_read_dpx_image_information_header", GAN_ERROR_NOT_IMPLEMENTED, "unsupported DPX image orientation" );
      return GAN_FALSE;
   }

   ui16ElementNumber = *((gan_uint16*)(acAlignedHeader + OFFSET_ELEMENTNUMBER));
   if(bReversedEndianness)
      vReverseEndianness16(&ui16ElementNumber);

   *pui32PixelsPerLine = *((gan_uint32*)(acAlignedHeader + OFFSET_PIXELSPERLINE));
   if(bReversedEndianness)
      vReverseEndianness32(pui32PixelsPerLine);

   *pui32LinesPerImageEle = *((gan_uint32*)(acAlignedHeader + OFFSET_LINESPERIMAGEELE));
   if(bReversedEndianness)
      vReverseEndianness32(pui32LinesPerImageEle);

#if 0
   /* let's just read the first image of a multi-image file, rather than reporting an error */

   /* we only support one DPX image per file */
   if(ui16ElementNumber != 1)
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_read_dpx_image_information_header", GAN_ERROR_NOT_IMPLEMENTED, "unsupported DPX element number" );
      return GAN_FALSE;
   }
#endif

   /* determine image signedness */
   ui8DataSign = *((gan_uint8*)(acAlignedHeader + OFFSET_DATASIGN0));

   ui32RefLowData      =  *((gan_uint32*)(acAlignedHeader + OFFSET_REFLOWDATA0));
   f32RefLowQuantity  =  *((gan_float32*)(acAlignedHeader + OFFSET_REFLOWQUANTITY0));
   ui32RefHighData     =  *((gan_uint32*)(acAlignedHeader + OFFSET_REFHIGHDATA0));
   f32RefHighQuantity =  *((gan_float32*)(acAlignedHeader + OFFSET_REFHIGHQUANTITY0));
   if(bReversedEndianness)
   {
      vReverseEndianness32(&ui32RefLowData);
      vReverseEndianness32((gan_uint32*)(void *)&f32RefLowQuantity);
      vReverseEndianness32(&ui32RefHighData);
      vReverseEndianness32((gan_uint32*)(void *)&f32RefHighQuantity);
   }

   /* determine image format */
   ui8Descriptor = *((gan_uint8*)(acAlignedHeader + OFFSET_DESCRIPTOR0));
   switch(ui8Descriptor)
   {
      case 1: /* red */
      case 2: /* green */
      case 3: /* blue */
      case 4: /* alpha */
      case 6: /* luminance */
        *peFormat = GAN_GREY_LEVEL_IMAGE;
        break;

      case 50: /* rgb */
        *peFormat = GAN_RGB_COLOUR_IMAGE;
        break;

      case 51: /* rgba */
        *peFormat = GAN_RGB_COLOUR_ALPHA_IMAGE;
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_read_dpx_image_information_header", GAN_ERROR_NOT_IMPLEMENTED, "unsupported DPX descriptor" );
        return GAN_FALSE;
   }

   /* read bits/channel/pixel */
   *pui8BitSize = *((gan_uint8*)(acAlignedHeader + OFFSET_BITSIZE0));

   /* Determine packing type */
   ui16Packing = *((gan_uint16*)(acAlignedHeader + OFFSET_PACKING0));
   if(bReversedEndianness)
      vReverseEndianness16(&ui16Packing);

   if(ui16Packing == DPX_PACKED)
      *pbPacked = GAN_TRUE;
   else if(ui16Packing == DPX_FILLED)
      *pbPacked = GAN_FALSE;
   else
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_read_dpx_image_information_header", GAN_ERROR_NOT_IMPLEMENTED, "unsupported DPX descriptor" );
      return GAN_FALSE;
   }

   switch(*pui8BitSize)
   {
      case 1:
        *peType = GAN_BOOL;
        break;

      case 8:
        *peType = GAN_UINT8;
        break;

      case 10:
      case 12:
      case 16:
        *peType = GAN_UINT16;
        *peType = GAN_UINT16;
        *peType = GAN_UINT16;
        break;

      case 32:
        *peType = GAN_FLOAT32;
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_read_dpx_image_information_header", GAN_ERROR_NOT_IMPLEMENTED, "unsupported DPX bit size" );
        return GAN_FALSE;
        break;
   }

   /* Determine encoding */
   ui16Encoding = *((gan_uint16*)(acAlignedHeader + OFFSET_ENCODING0));
   if(bReversedEndianness)
      vReverseEndianness16(&ui16Encoding);

   /* read transfer and image information header attributes */
   if(header != NULL)
   {
      header->info.dpx.image_info.data_sign = (Gan_Bool)ui8DataSign;
      header->info.dpx.image_info.ref_low_data     = ui32RefLowData;
      header->info.dpx.image_info.ref_low_quantity = f32RefLowQuantity;
      header->info.dpx.image_info.ref_high_data     = ui32RefHighData;
      header->info.dpx.image_info.ref_high_quantity = f32RefHighQuantity;
      header->info.dpx.image_info.descriptor = ui8Descriptor;
      header->info.dpx.image_info.transfer = (Gan_DPXTransferCharacteristic)(*((gan_uint8*)(acAlignedHeader + OFFSET_TRANSFER0)));
      header->info.dpx.image_info.colorimetric = (Gan_DPXColorimetricCharacteristics)(*((gan_uint8*)(acAlignedHeader + OFFSET_COLORIMETRIC0)));
      header->info.dpx.image_info.bit_size = (unsigned int)*pui8BitSize;
      header->info.dpx.image_info.packed = *pbPacked;
      header->info.dpx.image_info.encoding = ui16Encoding;
      strncpy(header->info.dpx.image_info.description, (char *)(acAlignedHeader + OFFSET_DESCRIPTION0), 32);
   }
   
   /* Determine data offset */
   ui32DataOffset = *((gan_uint32*)(acAlignedHeader + OFFSET_DATAOFFSET0));
   if(bReversedEndianness)
      vReverseEndianness32(&ui32DataOffset);

   /* Determine end-of-line padding */
   *pui32eolPadding = *((gan_uint32*)(acAlignedHeader + OFFSET_EOLPADDING0));
   if(bReversedEndianness)
      vReverseEndianness32(pui32eolPadding);

   /* Determine end-of-image padding */
   *pui32eoImagePadding = *((gan_uint32*)(acAlignedHeader + OFFSET_EOIMAGEPADDING0));
   if(bReversedEndianness)
      vReverseEndianness32(pui32eoImagePadding);

   if(ui32DataOffset != ui32ImageOffset)
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_read_dpx_image_information_header", GAN_ERROR_NOT_IMPLEMENTED, "incompatible DPX data offsets" );
      return GAN_FALSE;
   }

   return GAN_TRUE;
}

static Gan_Bool gan_read_dpx_orientation_header ( char *acAlignedHeader, FILE *infile, Gan_Bool bReversedEndianness,
                                                  struct Gan_ImageHeaderStruct *header )
{
   if(fread(acAlignedHeader, 1, 256, infile) != 256)
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_read_dpx_orientation_header", GAN_ERROR_CORRUPTED_FILE, "corrupted DPX orientation header (truncated file?)" );
      return GAN_FALSE;
   }

   if(header != NULL)
   {
      header->info.dpx.orientation.x_offset = *((gan_uint32 *)(acAlignedHeader + OFFSET_ORIENTATION_XOFFSET));
      header->info.dpx.orientation.y_offset = *((gan_uint32 *)(acAlignedHeader + OFFSET_ORIENTATION_YOFFSET));
      header->info.dpx.orientation.x_centre = *((gan_uint32 *)(acAlignedHeader + OFFSET_ORIENTATION_XCENTRE));
      header->info.dpx.orientation.y_centre = *((gan_uint32 *)(acAlignedHeader + OFFSET_ORIENTATION_YCENTRE));
      header->info.dpx.orientation.x_orig_size = *((gan_uint32 *)(acAlignedHeader + OFFSET_ORIENTATION_XORIGSIZE));
      header->info.dpx.orientation.y_orig_size = *((gan_uint32 *)(acAlignedHeader + OFFSET_ORIENTATION_YORIGSIZE));
      strncpy(header->info.dpx.orientation.file_name,     acAlignedHeader + OFFSET_ORIENTATION_FILENAME,     100);
      strncpy(header->info.dpx.orientation.creation_time, acAlignedHeader + OFFSET_ORIENTATION_CREATIONTIME,  24);
      strncpy(header->info.dpx.orientation.input_dev,     acAlignedHeader + OFFSET_ORIENTATION_INPUTDEV,      32);
      strncpy(header->info.dpx.orientation.input_serial,  acAlignedHeader + OFFSET_ORIENTATION_INPUTSERIAL,   32);
      header->info.dpx.orientation.border[0] = *((gan_uint16 *)(acAlignedHeader + OFFSET_ORIENTATION_BORDER));
      header->info.dpx.orientation.border[1] = *((gan_uint16 *)(acAlignedHeader + OFFSET_ORIENTATION_BORDER + 2));
      header->info.dpx.orientation.border[2] = *((gan_uint16 *)(acAlignedHeader + OFFSET_ORIENTATION_BORDER + 4));
      header->info.dpx.orientation.border[3] = *((gan_uint16 *)(acAlignedHeader + OFFSET_ORIENTATION_BORDER + 6));
      header->info.dpx.orientation.pixel_aspect[0] = *((gan_uint32 *)(acAlignedHeader + OFFSET_ORIENTATION_PIXELASPECT));
      header->info.dpx.orientation.pixel_aspect[1] = *((gan_uint32 *)(acAlignedHeader + OFFSET_ORIENTATION_PIXELASPECT + 4));
      if(bReversedEndianness)
      {
         vReverseEndianness32(&header->info.dpx.orientation.x_offset);
         vReverseEndianness32(&header->info.dpx.orientation.y_offset);
         vReverseEndianness32(&header->info.dpx.orientation.x_centre);
         vReverseEndianness32(&header->info.dpx.orientation.y_centre);
         vReverseEndianness32(&header->info.dpx.orientation.x_orig_size);
         vReverseEndianness32(&header->info.dpx.orientation.y_orig_size);
         vReverseEndianness16(&header->info.dpx.orientation.border[0]);
         vReverseEndianness16(&header->info.dpx.orientation.border[1]);
         vReverseEndianness16(&header->info.dpx.orientation.border[2]);
         vReverseEndianness16(&header->info.dpx.orientation.border[3]);
         vReverseEndianness32(&header->info.dpx.orientation.pixel_aspect[0]);
         vReverseEndianness32(&header->info.dpx.orientation.pixel_aspect[1]);
      }
   }
      
#define OFFSET_ORIENTATION_FILENAME      24
#define OFFSET_ORIENTATION_CREATIONTIME 124
#define OFFSET_ORIENTATION_INPUTDEV     148
#define OFFSET_ORIENTATION_INPUTSERIAL  180
#define OFFSET_ORIENTATION_BORDER       212
#define OFFSET_ORIENTATION_PIXELASPECT  220
   return GAN_TRUE;
}

static Gan_Bool gan_read_dpx_film_information_header ( char *acAlignedHeader, FILE *infile, Gan_Bool bReversedEndianness,
                                                       struct Gan_ImageHeaderStruct *header )
{
   if(fread(acAlignedHeader, 1, 256, infile) != 256)
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_read_dpx_film_information_header", GAN_ERROR_CORRUPTED_FILE, "corrupted DPX film information header (truncated file?)" );
      return GAN_FALSE;
   }

   /* read bits of film header */
   if(header != NULL)
   {
      header->info.dpx.film.film_mfg_id[0] = acAlignedHeader[OFFSET_FILM_MFGID];
      header->info.dpx.film.film_mfg_id[1] = acAlignedHeader[OFFSET_FILM_MFGID+1];
      header->info.dpx.film.film_type[0] = acAlignedHeader[OFFSET_FILM_FILMTYPE];
      header->info.dpx.film.film_type[1] = acAlignedHeader[OFFSET_FILM_FILMTYPE+1];
      header->info.dpx.film.offset[0] = acAlignedHeader[OFFSET_FILM_OFFSET];
      header->info.dpx.film.offset[1] = acAlignedHeader[OFFSET_FILM_OFFSET+1];
      strncpy(header->info.dpx.film.prefix, (char *)(acAlignedHeader + OFFSET_FILM_PREFIX), 6);
      strncpy(header->info.dpx.film.count, (char *)(acAlignedHeader + OFFSET_FILM_COUNT), 4);
      strncpy(header->info.dpx.film.format, (char *)(acAlignedHeader + OFFSET_FILM_FORMAT), 32);
      header->info.dpx.film.frame_position = *((gan_uint32 *)(acAlignedHeader + OFFSET_FILM_FRAMEPOSITION));
      header->info.dpx.film.frame_rate = *((gan_float32*)(acAlignedHeader + OFFSET_FILM_FRAMERATE));
      header->info.dpx.film.shutter_angle = *((gan_float32*)(acAlignedHeader + OFFSET_FILM_SHUTTERANGLE));
      strncpy(header->info.dpx.film.frame_id, (char *)(acAlignedHeader + OFFSET_FILM_FRAMEID), 32);
      strncpy(header->info.dpx.film.slate_info, (char *)(acAlignedHeader + OFFSET_FILM_SLATEINFO), 100);

      if(bReversedEndianness)
      {
         vReverseEndianness32(&header->info.dpx.film.frame_position);
         vReverseEndianness32((gan_uint32 *)&header->info.dpx.film.frame_rate);
         vReverseEndianness32((gan_uint32 *)&header->info.dpx.film.shutter_angle);
      }
#if 0  // comment out until we work out how to read the frame rate from DPX files

      // set common frame rate
      if(header->info.frame_rate == 0.0F && header->info.dpx.film.frame_rate != 0.0F)
         header->info.frame_rate = header->info.dpx.film.frame_rate;
#endif
   }

   return GAN_TRUE;
}

static Gan_Bool
 bValidTimeCode(gan_uint32 ui32TimeCode, gan_float32 f32FrameRate)
{
   unsigned uiHourUnits, uiHourTens, uiMinuteUnits, uiMinuteTens, uiSecondUnits, uiSecondTens, uiFrameUnits, uiFrameTens;

   uiHourTens   = (unsigned int) ((ui32TimeCode >> 28) & 0x0fU); uiHourUnits   = (unsigned int) ((ui32TimeCode >> 24) & 0x0fU);
   if(uiHourUnits > 9) return GAN_FALSE;
   uiMinuteTens = (unsigned int) ((ui32TimeCode >> 20) & 0x0fU); uiMinuteUnits = (unsigned int) ((ui32TimeCode >> 16) & 0x0fU);
   if(uiMinuteTens > 5 || uiMinuteUnits > 9) return GAN_FALSE;
   uiSecondTens = (unsigned int) ((ui32TimeCode >> 12) & 0x0fU); uiSecondUnits = (unsigned int) ((ui32TimeCode >>  8) & 0x0fU);
   if(uiSecondTens > 5 || uiSecondUnits > 9) return GAN_FALSE;
   uiFrameTens  = (unsigned int) ((ui32TimeCode >>  4) & 0x0fU); uiFrameUnits  = (unsigned int) ((ui32TimeCode >>  0) & 0x0fU);
   if(uiFrameUnits > 9) return GAN_FALSE;

   return GAN_TRUE;
}

static Gan_Bool gan_read_dpx_tv_information_header ( char *acAlignedHeader, FILE *infile, Gan_Bool bReversedEndianness,
                                                     struct Gan_ImageHeaderStruct *header )
{
   if(fread(acAlignedHeader, 1, 128, infile) != 128)
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_read_dpx_tv_information_header", GAN_ERROR_CORRUPTED_FILE, "corrupted DPX TV information header (truncated file?)" );
      return GAN_FALSE;
   }

   /* read bits of TV header */
   if(header != NULL)
   {
      gan_uint8 ui8Interlaced;

      header->info.time_code = *((gan_uint32 *)(acAlignedHeader + OFFSET_TV_TIMECODE));
      header->info.user_bits = *((gan_uint32 *)(acAlignedHeader + OFFSET_TV_USERBITS));
      ui8Interlaced = *((gan_uint8*)(acAlignedHeader + OFFSET_TV_INTERLACE));
      header->info.interlaced = (ui8Interlaced == 1) ? GAN_TRUE : GAN_FALSE;
      header->info.dpx.tv.field_num = *((gan_uint8 *)(acAlignedHeader + OFFSET_TV_FIELD_NUMBER));
      header->info.dpx.tv.video_signal = *((gan_uint8 *)(acAlignedHeader + OFFSET_TV_VIDEO_SIGNAL));
      header->info.dpx.tv.hor_sample_rate = *((gan_float32 *)(acAlignedHeader + OFFSET_TV_HORZSAMPLERATE));
      header->info.dpx.tv.ver_sample_rate = *((gan_float32 *)(acAlignedHeader + OFFSET_TV_VERTSAMPLERATE));
      header->info.dpx.tv.frame_rate = *((gan_float32 *)(acAlignedHeader + OFFSET_TV_FRAMERATE));
      header->info.dpx.tv.time_offset = *((gan_float32 *)(acAlignedHeader + OFFSET_TV_TIMEOFFSET));
      header->info.dpx.tv.gamma = *((gan_float32 *)(acAlignedHeader + OFFSET_TV_GAMMA));
      header->info.dpx.tv.black_level = *((gan_float32 *)(acAlignedHeader + OFFSET_TV_BLACKLEVEL));
      header->info.dpx.tv.black_gain = *((gan_float32 *)(acAlignedHeader + OFFSET_TV_BLACKGAIN));
      header->info.dpx.tv.break_point = *((gan_float32 *)(acAlignedHeader + OFFSET_TV_BREAKPOINT));
      header->info.dpx.tv.white_level = *((gan_float32 *)(acAlignedHeader + OFFSET_TV_WHITELEVEL));
      header->info.dpx.tv.integration_times = *((gan_float32 *)(acAlignedHeader + OFFSET_TV_INTEGRATIONTIME));

      if(bReversedEndianness)
      {
         vReverseEndianness32(&header->info.time_code);
         vReverseEndianness32(&header->info.user_bits);
         vReverseEndianness32((gan_uint32 *)&header->info.dpx.tv.hor_sample_rate);
         vReverseEndianness32((gan_uint32 *)&header->info.dpx.tv.ver_sample_rate);
         vReverseEndianness32((gan_uint32 *)&header->info.dpx.tv.frame_rate);
         vReverseEndianness32((gan_uint32 *)&header->info.dpx.tv.time_offset);
         vReverseEndianness32((gan_uint32 *)&header->info.dpx.tv.gamma);
         vReverseEndianness32((gan_uint32 *)&header->info.dpx.tv.black_level);
         vReverseEndianness32((gan_uint32 *)&header->info.dpx.tv.black_gain);
         vReverseEndianness32((gan_uint32 *)&header->info.dpx.tv.break_point);
         vReverseEndianness32((gan_uint32 *)&header->info.dpx.tv.white_level);
         vReverseEndianness32((gan_uint32 *)&header->info.dpx.tv.integration_times);
      }

      /* reset time code if invalid */
      if(!bValidTimeCode(header->info.time_code, header->info.dpx.tv.frame_rate))
      {
         header->info.time_code = 0;
         header->info.user_bits = 0;
      }

#if 0  // comment out until we work out how to read the frame rate from DPX files

      // set common frame rate
      if(header->info.frame_rate == 0.0F && header->info.dpx.tv.frame_rate != 0.0F)
         header->info.frame_rate = header->info.dpx.tv.frame_rate;
#endif
   }

   return GAN_TRUE;
}

/**
 * \brief Reads a RGB colour image file in DPX format from a stream.
 * \param infile The file stream to be read
 * \param image The image structure to read the image data into or \c NULL
 * \param ictrlstr Pointer to structure controlling input or \c NULL
 * \param header Pointer to file header structure to be filled, or \c NULL
 * \return Pointer to image structure, or \c NULL on failure.
 *
 * Reads the DPX image from the given file stream \a infile into the given
 * \a image. If \a image is \c NULL a new image is dynamically allocated,
 * otherwise the already allocated \a image structure is reused.
 *
 * \sa gan_write_dpx_image_stream().
 */
Gan_Image *
 gan_read_dpx_image_stream ( FILE *infile, Gan_Image *image, const struct Gan_ImageReadControlStruct *ictrlstr, struct Gan_ImageHeaderStruct *header )
{
   char acHeader[BIG_BUFFER_SIZE], *acAlignedHeader;
   Gan_ImageFormat eFormat;
   Gan_Type eType;
   Gan_Bool bReversedEndianness=GAN_FALSE, bFlip=GAN_FALSE;
   Gan_Bool bSingleField=GAN_FALSE, bUpper=GAN_FALSE, bWholeImage=GAN_FALSE;

   /* image information fields */
   gan_uint16 ui16Orientation;
   gan_uint32 ui32PixelsPerLine, ui32LinesPerImageEle;
   gan_uint8 ui8BitSize;
   Gan_Bool bPacked;
   gan_uint32 ui32eolPadding, ui32eoImagePadding;

   /* DPX file header info */
   gan_uint32 ui32ImageOffset;  /* Offset to start of image data in bytes */

   /* align the header array */
   acAlignedHeader = (char*)((unsigned long int)acHeader + 7 - (((unsigned long int)acHeader + 7) % 8));

   /* read the generic file header */
   if(!gan_read_dpx_generic_header(acAlignedHeader, infile, &bReversedEndianness, &ui32ImageOffset, header))
   {
      gan_err_register ( "gan_read_dpx_image_stream", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* read the image information header */
   if(!gan_read_dpx_image_information_header(acAlignedHeader, infile, bReversedEndianness, ui32ImageOffset,
                                             &ui16Orientation, &ui32PixelsPerLine, &ui32LinesPerImageEle,
                                             &eFormat, &eType, &ui8BitSize, &bPacked, &ui32eolPadding, &ui32eoImagePadding,
                                             header))
   {
      gan_err_register ( "gan_read_dpx_image_stream", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* read the orientation header */
   if(!gan_read_dpx_orientation_header(acAlignedHeader, infile, bReversedEndianness, header))
   {
      gan_err_register ( "gan_read_dpx_image_stream", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* read the film information header */
   if(!gan_read_dpx_film_information_header(acAlignedHeader, infile, bReversedEndianness, header))
   {
      gan_err_register ( "gan_read_dpx_image_stream", GAN_ERROR_FAILURE, "" );
      return NULL;
   }
      
   /* read the TV information header */
   if(!gan_read_dpx_tv_information_header(acAlignedHeader, infile, bReversedEndianness, header))
   {
      gan_err_register ( "gan_read_dpx_image_stream", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

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
      header->file_format = GAN_DPX_FORMAT;
      header->width = ui32PixelsPerLine;
      header->height = (bSingleField && !bWholeImage) ? (ui32LinesPerImageEle/2) : ui32LinesPerImageEle;
      header->format = eFormat;
      header->type = eType;
   }

   if(ictrlstr != NULL && ictrlstr->header_only)
      return (Gan_Image*)-1; /* special value */

   if(ui16Orientation == 2)
   {
      bFlip = !bFlip;
      bUpper = !bUpper;
   }

   /* reset file point to start of image data */
   if(fseek(infile, ui32ImageOffset, SEEK_SET) != 0)
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_read_dpx_image_stream", GAN_ERROR_CORRUPTED_FILE, "corrupted DPX image orientation header (truncated file?)" );
      return NULL;
   }

   /* now read the image data */
   switch(ui8BitSize)
   {
      case 1:
        image = pgiRead1BitDPXImageData(infile, ui32eolPadding, ui32eoImagePadding,
                                        eFormat, eType, ui32PixelsPerLine, ui32LinesPerImageEle, image, bFlip, bSingleField, bUpper, bWholeImage);
        break;

      case 8:
        image = pgiRead8BitDPXImageData(infile, ui32eolPadding, ui32eoImagePadding,
                                        eFormat, eType, ui32PixelsPerLine, ui32LinesPerImageEle, image, bFlip, bSingleField, bUpper, bWholeImage);
        break;

      case 10:
        image = pgiRead10BitDPXImageData(infile, bReversedEndianness, bPacked, ui32eolPadding, ui32eoImagePadding,
                                         eFormat, eType, ui32PixelsPerLine, ui32LinesPerImageEle, image, bFlip, bSingleField, bUpper, bWholeImage);
        break;

      case 12:
        image = pgiRead12BitDPXImageData(infile, bReversedEndianness, bPacked, ui32eolPadding, ui32eoImagePadding,
                                         eFormat, eType, ui32PixelsPerLine, ui32LinesPerImageEle, image, bFlip, bSingleField, bUpper, bWholeImage);
        break;

      case 16:
        image = pgiRead16BitDPXImageData(infile, bReversedEndianness, ui32eolPadding, ui32eoImagePadding,
                                         eFormat, eType, ui32PixelsPerLine, ui32LinesPerImageEle, image, bFlip, bSingleField, bUpper, bWholeImage);
        break;

      case 32:
        image = pgiRead32BitFloatDPXImageData(infile, bReversedEndianness, ui32eolPadding, ui32eoImagePadding,
                                              eFormat, eType, ui32PixelsPerLine, ui32LinesPerImageEle, image, bFlip, bSingleField, bUpper, bWholeImage);
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_read_dpx_image_stream", GAN_ERROR_NOT_IMPLEMENTED, "unsupported DPX bit depth" );
        return NULL;
   }
        
   if(image == NULL)
   {
      gan_err_register ( "gan_read_dpx_image_stream", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* success */
   return image;
}

/**
 * \brief Reads a RGB colour image file in DPX format.
 * \param filename The name of the image file
 * \param image The image structure to read the image data into or \c NULL
 * \param ictrlstr Pointer to structure controlling input or \c NULL
 * \param header Pointer to file header structure to be filled, or \c NULL
 * \return Pointer to image structure, or \c NULL on failure.
 *
 * Reads the DPX image with the in the file \a filename into the given
 * \a image. If \a image is \c NULL a new image is dynamically allocated;
 * otherwise the already allocated \a image structure is reused.
 *
 * \sa gan_write_dpx_image().
 */
Gan_Image *
 gan_read_dpx_image ( const char *filename, Gan_Image *image, const struct Gan_ImageReadControlStruct *ictrlstr, struct Gan_ImageHeaderStruct *header )
{
   FILE *infile;
   Gan_Image *result;

   /* attempt to open file */
   infile = fopen ( filename, "rb" );
   if ( infile == NULL )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_read_dpx_image", GAN_ERROR_OPENING_FILE, filename );
      return NULL;
   }

   result = gan_read_dpx_image_stream ( infile, image, ictrlstr, header );
   fclose(infile);
   return result;
}

/**
 * \brief Initialises the header structure for DPX files
 *
 * This function should be called on the structure to set the parameters to default values.
 * Then set any non-default values yourself before calling gan_write_dpx_image() or
 * gan_write_dpx_image_stream().
 */
void gan_initialise_dpx_header_struct(Gan_DPXHeaderStruct *octrlstr, Gan_ImageFormat image_format, Gan_Type type)
{
   memset(octrlstr, 0, sizeof(Gan_DPXHeaderStruct));
   octrlstr->generic.encryption_key = 0xffffffff;
   switch(type)
   {
      case GAN_BOOL:
        octrlstr->image_info.bit_size = 1;
        break;
        
      case GAN_UINT8:
        octrlstr->image_info.bit_size = 8;
        break;
        
      case GAN_UINT16:
        if(image_format == GAN_RGB_COLOUR_IMAGE)
           octrlstr->image_info.bit_size = 10;
        else
           octrlstr->image_info.bit_size = 16;

        break;
        
      case GAN_FLOAT32:
        octrlstr->image_info.bit_size = 32;
        break;

      default:
        if(image_format == GAN_RGB_COLOUR_IMAGE)
           octrlstr->image_info.bit_size = 10;
        else
           octrlstr->image_info.bit_size = 16;
   }

   octrlstr->image_info.transfer = GAN_DPXTRANSFER_DEFAULT;
   octrlstr->image_info.colorimetric = GAN_DPXCOLORIMETRIC_DEFAULT;
   octrlstr->image_info.packed = GAN_FALSE;
}

#define IMAGE_DATA_OFFSET 8192

/**
 * \brief Not a user function.
 */
gan_uint32
 ui32DPXFileSize(unsigned int uiImageDataOffset,
                 Gan_ImageFormat eFormat, gan_uint8 ui8BitSize, Gan_Bool bPacked, unsigned int uiHeight, unsigned int uiWidth,
                 gan_uint32* pui32eolPadding)
{
   /* file size needs to be written into the file, so we need a function for it */
   unsigned int uiRowBytes, uieolPadding;

   switch(ui8BitSize)
   {
      case 1:
        switch(eFormat)
        {
           case GAN_GREY_LEVEL_IMAGE:
             uiRowBytes = (uiWidth+7)/8;
             uieolPadding = ((uiWidth+31)/32)*4 - uiRowBytes;
             break;

           default: return UINT_MAX;
        }
        break;
        
      case 8:
        switch(eFormat)
        {
           case GAN_GREY_LEVEL_IMAGE:
             uiRowBytes = uiWidth;
             uieolPadding = ((uiWidth+3)/4)*4 - uiRowBytes;
             break;

           case GAN_RGB_COLOUR_IMAGE:
             uiRowBytes = 3*uiWidth;
             uieolPadding = ((3*uiWidth+3)/4)*4 - uiRowBytes;
             break;

           case GAN_RGB_COLOUR_ALPHA_IMAGE:
             uiRowBytes = 4*uiWidth;
             uieolPadding = 0;
             break;

           default: return UINT_MAX;
        }
        break;

      case 10:
        switch(eFormat)
        {
           case GAN_RGB_COLOUR_IMAGE:
             if(bPacked)
                return UINT_MAX; /* unsupported */
             else
             {
                uiRowBytes = 4*uiWidth;
                uieolPadding = 0;
             }

             break;
             
           default: return UINT_MAX;
        }
        break;
        
      case 12:
        switch(eFormat)
        {
           case GAN_RGB_COLOUR_IMAGE:
             if(bPacked)
             {
                uiRowBytes = (9*uiWidth + 1)/2;
                uieolPadding = 3 - ((uiRowBytes - 1) % 4);
             }
             else
             {
                uiRowBytes = 6*uiWidth;
                if((uiWidth % 2) == 0)
                   uieolPadding = 0;
                else
                   uieolPadding = 2;
             }

             break;
             
           default: return UINT_MAX;
        }
        break;

      case 16:
        switch(eFormat)
        {
           case GAN_GREY_LEVEL_IMAGE:
             uiRowBytes = 2*uiWidth;
             uieolPadding = ((uiWidth+1)/2)*4 - uiRowBytes;
             break;

           case GAN_RGB_COLOUR_IMAGE:
             uiRowBytes = 6*uiWidth;
             uieolPadding = ((3*uiWidth+1)/2)*4 - uiRowBytes;
             break;

           case GAN_RGB_COLOUR_ALPHA_IMAGE:
             uiRowBytes = 8*uiWidth;
             uieolPadding = 0;
             break;

           default: return UINT_MAX;
        }
        break;

      case 32:
        switch(eFormat)
        {
           case GAN_GREY_LEVEL_IMAGE:
             uiRowBytes = 4*uiWidth;
             uieolPadding = 0;
             break;

           case GAN_RGB_COLOUR_IMAGE:
             uiRowBytes = 12*uiWidth;
             uieolPadding = 0;
             break;

           case GAN_RGB_COLOUR_ALPHA_IMAGE:
             uiRowBytes = 16*uiWidth;
             uieolPadding = 0;
             break;

           default: return UINT_MAX;
        }
        break;

      default: return UINT_MAX;
   }

   if(pui32eolPadding != NULL)
      *pui32eolPadding = uieolPadding;
   
   return (uiImageDataOffset + uiHeight*(uiRowBytes + uieolPadding));
}

/**
 * \brief Not a user function
 */
Gan_Bool
 bWrite1BitDPXImageData(FILE* pfOutFile, const Gan_Image* pgiImage, Gan_Bool bNewFile, unsigned int uieolPadding, Gan_Bool bFlip, Gan_Bool bSingleField, Gan_Bool bUpper, Gan_Bool bWholeImage)
{
   unsigned int uiRow, uiHeight=pgiImage->height;
   unsigned int uiRowSizeInBytes;
   char *acBuffer, *acAlignedBuffer;
   Gan_BitArray gbaAlignedBuffer;

   if(bSingleField && !bWholeImage)
      uiHeight *= 2;

   if(pgiImage->type != GAN_BOOL)
   {
      gan_err_flush_trace();
      gan_err_register ( "pgiWrite1BitDPXImageData", GAN_ERROR_NOT_IMPLEMENTED, "no 1 bit conversions supported" );
      return GAN_FALSE;
   }

   /* determine row data size in bytes, and padding to a 32-bit boundary */
   switch(pgiImage->format)
   {
      case GAN_GREY_LEVEL_IMAGE:
        uiRowSizeInBytes = GAN_NO_BITWORDS(pgiImage->width)*sizeof(Gan_BitWord);
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "pgiWrite1BitDPXImageData", GAN_ERROR_NOT_IMPLEMENTED, "image format not supported" );
        return GAN_FALSE;
   }

   /* build a buffer to hold a single line of image data and align it to a four byte boundary*/
   acBuffer = malloc(uiRowSizeInBytes+uieolPadding+sizeof(Gan_BitWord)-1);
   if(acBuffer == NULL)
   {
      gan_err_flush_trace();
      gan_err_register_with_number ( "pgiWrite1BitDPXImageData", GAN_ERROR_MALLOC_FAILED, "", uiRowSizeInBytes+uieolPadding+3 );
      return GAN_FALSE;
   }

   acAlignedBuffer = (char*)((unsigned long int)acBuffer + sizeof(Gan_BitWord) - 1 - (((unsigned long int)acBuffer + sizeof(Gan_BitWord) - 1) % sizeof(Gan_BitWord)));

   /* form bit array around the aligned buffer */
   gan_bit_array_form_data ( &gbaAlignedBuffer, (Gan_BitWord *)acAlignedBuffer, GAN_NO_BITWORDS(pgiImage->width), pgiImage->width );

   /* write the image data */
   for(uiRow = 0; uiRow<uiHeight; uiRow++)
   {
      /* ignore row if it's in a field we're not writing */
      if(bSingleField && ((bUpper && ((uiRow % 2) == 1)) || (!bUpper && ((uiRow % 2) == 0))))
      {
         if(bNewFile)
         {
            /* write a blank row */
            memset((void*)acAlignedBuffer, 0, uiRowSizeInBytes+uieolPadding);
            if ( pfOutFile != NULL &&
                 fwrite ( acAlignedBuffer, 1, uiRowSizeInBytes+uieolPadding, pfOutFile ) != uiRowSizeInBytes+uieolPadding )
            {
               gan_err_flush_trace();
               gan_err_register ( "pgiWrite1BitDPXImageData", GAN_ERROR_TRUNCATED_FILE, "truncated DPX file" );
               return GAN_FALSE;
            }
         }
         else
         {
            /* leave this row as is was */
            if(pfOutFile != NULL && fseek(pfOutFile, uiRowSizeInBytes+uieolPadding, SEEK_CUR) != 0)
            {
               gan_err_flush_trace();
               gan_err_register ( "pgiWrite1BitDPXImageData", GAN_ERROR_TRUNCATED_FILE, "truncated DPX file" );
               return GAN_FALSE;
            }
         }
      }
      else
      {
         unsigned long uiImRow;

         if(bSingleField)
         {
            if(bWholeImage)
               uiImRow = (bFlip ? (pgiImage->height-uiRow-1) : uiRow);
            else
               uiImRow = (bFlip ? (pgiImage->height-uiRow/2-1) : uiRow/2);
         }
         else
            uiImRow = (bFlip ? (pgiImage->height-uiRow-1) : uiRow);
      
         switch(pgiImage->format)
         {
            case GAN_GREY_LEVEL_IMAGE:
            {
               memset((void*)acAlignedBuffer, 0, uiRowSizeInBytes+uieolPadding);
               gan_bit_array_copy_q ( &pgiImage->ba[uiImRow], &gbaAlignedBuffer );

               // write scan-line of data, including padding
               if ( pfOutFile != NULL &&
                    fwrite ( acAlignedBuffer, 1, uiRowSizeInBytes+uieolPadding, pfOutFile ) != uiRowSizeInBytes+uieolPadding )
               {
                  gan_err_flush_trace();
                  gan_err_register ( "pgiWrite1BitDPXImageData", GAN_ERROR_TRUNCATED_FILE, "truncated DPX file" );
                  return GAN_FALSE;
               }
            }

            break;

            default:
              gan_err_flush_trace();
              gan_err_register ( "pgiWrite1BitDPXImageData", GAN_ERROR_NOT_IMPLEMENTED, "image format not supported" );
              return GAN_FALSE;
         }
      }
   }
   
   free(acBuffer);
   gan_bit_array_free ( &gbaAlignedBuffer );
   return GAN_TRUE;
}

/**
 * \brief Not a user function
 */
Gan_Bool
 bWrite8BitDPXImageData(FILE* pfOutFile, const Gan_Image* pgiImage, Gan_Bool bNewFile, unsigned int uieolPadding, Gan_Bool bFlip, Gan_Bool bSingleField, Gan_Bool bUpper, Gan_Bool bWholeImage)
{
   unsigned int uiRow, uiHeight=pgiImage->height;
   int iCol;
   unsigned int uiRowSizeInBytes;
   char *acBuffer, *acAlignedBuffer;

   if(bSingleField && !bWholeImage)
      uiHeight *= 2;

   if(pgiImage->type != GAN_UINT8)
   {
      gan_err_flush_trace();
      gan_err_register ( "pgiWrite8BitDPXImageData", GAN_ERROR_NOT_IMPLEMENTED, "no 8 bit conversions supported" );
      return GAN_FALSE;
   }

   /* determine row data size in bytes, and padding to a 32-bit boundary */
   switch(pgiImage->format)
   {
      case GAN_GREY_LEVEL_IMAGE:
        uiRowSizeInBytes = pgiImage->width;
        break;

      case GAN_RGB_COLOUR_IMAGE:
        uiRowSizeInBytes = 3*pgiImage->width;
        break;

      case GAN_RGB_COLOUR_ALPHA_IMAGE:
        uiRowSizeInBytes = 4*pgiImage->width;
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "pgiWrite8BitDPXImageData", GAN_ERROR_NOT_IMPLEMENTED, "image format not supported" );
        return GAN_FALSE;
   }

   /* build a buffer to hold a single line of image data and align it to a four byte boundary*/
   acBuffer = malloc(uiRowSizeInBytes+uieolPadding+3);
   if(acBuffer == NULL)
   {
      gan_err_flush_trace();
      gan_err_register_with_number ( "pgiWrite8BitDPXImageData", GAN_ERROR_MALLOC_FAILED, "", uiRowSizeInBytes+uieolPadding+3 );
      return GAN_FALSE;
   }

   acAlignedBuffer = (char*)((unsigned long int)acBuffer + 3 - (((unsigned long int)acBuffer + 3) % 4));

   /* write the image data */
   for(uiRow = 0; uiRow<uiHeight; uiRow++)
   {
      /* ignore row if it's in a field we're not writing */
      if(bSingleField && ((bUpper && ((uiRow % 2) == 1)) || (!bUpper && ((uiRow % 2) == 0))))
      {
         if(bNewFile)
         {
            /* write a blank row */
            memset((void*)acAlignedBuffer, 0, uiRowSizeInBytes+uieolPadding);
            if ( pfOutFile != NULL &&
                 fwrite ( acAlignedBuffer, 1, uiRowSizeInBytes+uieolPadding, pfOutFile ) != uiRowSizeInBytes+uieolPadding )
            {
               gan_err_flush_trace();
               gan_err_register ( "pgiWrite8BitDPXImageData", GAN_ERROR_TRUNCATED_FILE, "truncated DPX file" );
               return GAN_FALSE;
            }
         }
         else
         {
            /* leave this row as is was */
            if(pfOutFile != NULL && fseek(pfOutFile, uiRowSizeInBytes+uieolPadding, SEEK_CUR) != 0)
            {
               gan_err_flush_trace();
               gan_err_register ( "pgiWrite8BitDPXImageData", GAN_ERROR_TRUNCATED_FILE, "truncated DPX file" );
               return GAN_FALSE;
            }
         }
      }
      else
      {
         unsigned long uiImRow;

         if(bSingleField)
         {
            if(bWholeImage)
               uiImRow = (bFlip ? (pgiImage->height-uiRow-1) : uiRow);
            else
               uiImRow = (bFlip ? (pgiImage->height-uiRow/2-1) : uiRow/2);
         }
         else
            uiImRow = (bFlip ? (pgiImage->height-uiRow-1) : uiRow);
      
         switch(pgiImage->format)
         {
            case GAN_GREY_LEVEL_IMAGE:
            {
               gan_uint8 *pui8ImPix, *pui8BufPix;

               memset((void*)acAlignedBuffer, 0, uiRowSizeInBytes+uieolPadding);
               for(iCol=(int)pgiImage->width-1,
                   pui8ImPix = gan_image_get_pixptr_gl_ui8(pgiImage, uiImRow, iCol),
                   pui8BufPix = ((gan_uint8*)acAlignedBuffer) + iCol; iCol >= 0; iCol--)
                  *pui8BufPix-- = *pui8ImPix--;

               // write scan-line of data, including padding
               if ( pfOutFile != NULL &&
                    fwrite ( acAlignedBuffer, 1, uiRowSizeInBytes+uieolPadding, pfOutFile ) != uiRowSizeInBytes+uieolPadding )
               {
                  gan_err_flush_trace();
                  gan_err_register ( "pgiWrite8BitDPXImageData", GAN_ERROR_TRUNCATED_FILE, "truncated DPX file" );
                  return GAN_FALSE;
               }
            }

            break;

            case GAN_RGB_COLOUR_IMAGE:
            {
               Gan_RGBPixel_ui8 *pui8rgbImPix, *pui8rgbBufPix;

               memset((void*)acAlignedBuffer, 0, uiRowSizeInBytes+uieolPadding);
               for(iCol=(int)pgiImage->width-1,
                   pui8rgbImPix = gan_image_get_pixptr_rgb_ui8(pgiImage, uiImRow, iCol),
                   pui8rgbBufPix = ((Gan_RGBPixel_ui8*)acAlignedBuffer) + iCol; iCol >= 0; iCol--)
                  *pui8rgbBufPix-- = *pui8rgbImPix--;

               // write scan-line of data, including padding
               if ( pfOutFile != NULL &&
                    fwrite ( acAlignedBuffer, 1, uiRowSizeInBytes+uieolPadding, pfOutFile ) != uiRowSizeInBytes+uieolPadding )
               {
                  gan_err_flush_trace();
                  gan_err_register ( "pgiWrite8BitDPXImageData", GAN_ERROR_TRUNCATED_FILE, "truncated DPX file" );
                  return GAN_FALSE;
               }
            }

            break;

            case GAN_RGB_COLOUR_ALPHA_IMAGE:
            {
               Gan_RGBAPixel_ui8 *pui8rgbaImPix, *pui8rgbaBufPix;

               memset((void*)acAlignedBuffer, 0, uiRowSizeInBytes+uieolPadding);
               for(iCol=(int)pgiImage->width-1,
                   pui8rgbaImPix = gan_image_get_pixptr_rgba_ui8(pgiImage, uiImRow, iCol),
                   pui8rgbaBufPix = ((Gan_RGBAPixel_ui8*)acAlignedBuffer) + iCol; iCol >= 0; iCol--)
                  *pui8rgbaBufPix-- = *pui8rgbaImPix--;

               // write scan-line of data, including padding
               if ( pfOutFile != NULL &&
                    fwrite ( acAlignedBuffer, 1, uiRowSizeInBytes+uieolPadding, pfOutFile ) != uiRowSizeInBytes+uieolPadding )
               {
                  gan_err_flush_trace();
                  gan_err_register ( "pgiWrite8BitDPXImageData", GAN_ERROR_TRUNCATED_FILE, "truncated DPX file" );
                  return GAN_FALSE;
               }
            }

            break;

            default:
              gan_err_flush_trace();
              gan_err_register ( "pgiWrite8BitDPXImageData", GAN_ERROR_NOT_IMPLEMENTED, "image format not supported" );
              return GAN_FALSE;
         }
      }
   }
   
   free(acBuffer);
   return GAN_TRUE;
}

/**
 * \brief Not a user function
 */
Gan_Bool
 bWrite10BitDPXImageData(FILE* pfOutFile, const Gan_Image* pgiImage, Gan_Bool bNewFile, Gan_Bool bReverseBytes, unsigned int uieolPadding, Gan_Bool bPacked, Gan_Bool bFlip, Gan_Bool bSingleField, Gan_Bool bUpper, Gan_Bool bWholeImage)
{
   unsigned int uiRow, uiHeight=pgiImage->height;
   int iCol;
   unsigned int uiRowSizeInBytes;
   char *acBuffer, *acAlignedBuffer;

   if(bSingleField && !bWholeImage)
      uiHeight *= 2;

   if(bPacked)
   {
      gan_err_flush_trace();
      gan_err_register ( "pgiWrite10BitDPXImageData", GAN_ERROR_NOT_IMPLEMENTED, "10 bit packed format not supported" );
      return GAN_FALSE;
   }

   if(pgiImage->type != GAN_UINT16)
   {
      gan_err_flush_trace();
      gan_err_register ( "pgiWrite10BitDPXImageData", GAN_ERROR_NOT_IMPLEMENTED, "only 10 --> 16 bit conversion supported" );
      return GAN_FALSE;
   }

   /* determine row data size in bytes, excluding padding */
   switch(pgiImage->format)
   {
      case GAN_RGB_COLOUR_IMAGE:
        uiRowSizeInBytes = 4*pgiImage->width;
        break;

      case GAN_RGB_COLOUR_ALPHA_IMAGE:
        uiRowSizeInBytes = ((4*pgiImage->width+2)/3)*4;
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "pgiWrite10BitDPXImageData", GAN_ERROR_NOT_IMPLEMENTED, "only RGB format supported" );
        return GAN_FALSE;
   }        

   /* build a buffer to hold a single line of image data and align it to a four byte boundary*/
   acBuffer = malloc(uiRowSizeInBytes+uieolPadding+3);
   if(acBuffer == NULL)
   {
      gan_err_flush_trace();
      gan_err_register_with_number ( "pgiWrite10BitDPXImageData", GAN_ERROR_MALLOC_FAILED, "", uiRowSizeInBytes+uieolPadding+3 );
      return GAN_FALSE;
   }

   acAlignedBuffer = (char*)((unsigned long int)acBuffer + 3 - (((unsigned long int)acBuffer + 3) % 4));

   /* write the image data */
   for(uiRow = 0; uiRow<uiHeight; uiRow++)
   {
      /* ignore row if it's in a field we're not writing */
      if(bSingleField && ((bUpper && ((uiRow % 2) == 1)) || (!bUpper && ((uiRow % 2) == 0))))
      {
         if(bNewFile)
         {
            /* write a blank row */
            memset((void*)acAlignedBuffer, 0, uiRowSizeInBytes+uieolPadding);
            if ( pfOutFile != NULL &&
                 fwrite ( acAlignedBuffer, 1, uiRowSizeInBytes+uieolPadding, pfOutFile ) != uiRowSizeInBytes+uieolPadding )
            {
               gan_err_flush_trace();
               gan_err_register ( "pgiWrite10BitDPXImageData", GAN_ERROR_TRUNCATED_FILE, "truncated DPX file" );
               return GAN_FALSE;
            }
         }
         else
         {
            /* leave this row as is was */
            if(pfOutFile != NULL && fseek(pfOutFile, uiRowSizeInBytes+uieolPadding, SEEK_CUR) != 0)
            {
               gan_err_flush_trace();
               gan_err_register ( "pgiWrite10BitDPXImageData", GAN_ERROR_TRUNCATED_FILE, "truncated DPX file" );
               return GAN_FALSE;
            }
         }
      }
      else
      {
         unsigned long uiImRow;

         if(bSingleField)
         {
            if(bWholeImage)
               uiImRow = (bFlip ? (pgiImage->height-uiRow-1) : uiRow);
            else
               uiImRow = (bFlip ? (pgiImage->height-uiRow/2-1) : uiRow/2);
         }
         else
            uiImRow = (bFlip ? (pgiImage->height-uiRow-1) : uiRow);

         switch(pgiImage->format)
         {
            case GAN_RGB_COLOUR_IMAGE:
            {
               Gan_RGBPixel_ui16* pui16rgbPix;
               gan_uint32* pui32Pix;

               memset((void*)acAlignedBuffer, 0, uiRowSizeInBytes+uieolPadding);
               if(bReverseBytes)
               {
                  for(iCol=(int)pgiImage->width-1, pui16rgbPix = gan_image_get_pixptr_rgb_ui16(pgiImage, uiImRow, iCol),
                      pui32Pix = ((gan_uint32*)acAlignedBuffer) + iCol; iCol >= 0; iCol--, pui16rgbPix--, pui32Pix--)
                  {
                     (*pui32Pix) |= ((pui16rgbPix->R >> 6) << 22);
                     (*pui32Pix) |= ((pui16rgbPix->G >> 6) << 12);
                     (*pui32Pix) |= ((pui16rgbPix->B >> 6) <<  2);
                     vReverseEndianness32(pui32Pix);
                  }
               }
               else
               {
                  for(iCol=(int)pgiImage->width-1, pui16rgbPix = gan_image_get_pixptr_rgb_ui16(pgiImage, uiImRow, iCol),
                      pui32Pix = ((gan_uint32*)acAlignedBuffer) + iCol; iCol >= 0; iCol--, pui16rgbPix--, pui32Pix--)
                  {
                     (*pui32Pix) |= ((pui16rgbPix->R >> 6) << 22);
                     (*pui32Pix) |= ((pui16rgbPix->G >> 6) << 12);
                     (*pui32Pix) |= ((pui16rgbPix->B >> 6) <<  2);
                  }
               }

               if ( pfOutFile != NULL &&
                    fwrite ( acAlignedBuffer, 1, uiRowSizeInBytes+uieolPadding, pfOutFile ) != uiRowSizeInBytes+uieolPadding )
               {
                  gan_err_flush_trace();
                  gan_err_register ( "bWrite10BitDPXImageData", GAN_ERROR_TRUNCATED_FILE, "truncated DPX file" );
                  return GAN_FALSE;
               }
            }

            break;

            case GAN_RGB_COLOUR_ALPHA_IMAGE:
            {
               Gan_RGBAPixel_ui16* pui16rgbaPix;
               gan_uint32* pui32Pix;
               unsigned int uiCol;

               memset((void*)acAlignedBuffer, 0, uiRowSizeInBytes+uieolPadding);
               if(bReverseBytes)
               {
                  for(uiCol=0, pui16rgbaPix = gan_image_get_pixptr_rgba_ui16(pgiImage, uiImRow, 0),
                      pui32Pix = (gan_uint32*)acAlignedBuffer; uiCol < pgiImage->width; uiCol++, pui16rgbaPix++)
                  {
                     switch(uiCol % 3)
                     {
                        case 0:
                          (*pui32Pix) |= ((pui16rgbaPix->R >> 6) << 22);
                          (*pui32Pix) |= ((pui16rgbaPix->G >> 6) << 12);
                          (*pui32Pix) |= ((pui16rgbaPix->B >> 6) <<  2);
                          pui32Pix++;
                          (*pui32Pix) |= ((pui16rgbaPix->A >> 6) << 22);
                          break;

                        case 1:
                          (*pui32Pix) |= ((pui16rgbaPix->R >> 6) << 12);
                          (*pui32Pix) |= ((pui16rgbaPix->G >> 6) <<  2);
                          pui32Pix++;
                          (*pui32Pix) |= ((pui16rgbaPix->B >> 6) << 22);
                          (*pui32Pix) |= ((pui16rgbaPix->A >> 6) << 12);
                          break;

                        case 2:
                          (*pui32Pix) |= ((pui16rgbaPix->R >> 6) <<  2);
                          pui32Pix++;
                          (*pui32Pix) |= ((pui16rgbaPix->G >> 6) << 22);
                          (*pui32Pix) |= ((pui16rgbaPix->B >> 6) << 12);
                          (*pui32Pix) |= ((pui16rgbaPix->A >> 6) <<  2);
                          pui32Pix++;
                     }
                  }
               }
               else
               {
                  for(uiCol=0, pui16rgbaPix = gan_image_get_pixptr_rgba_ui16(pgiImage, uiImRow, 0),
                      pui32Pix = (gan_uint32*)acAlignedBuffer; uiCol < pgiImage->width; uiCol++, pui16rgbaPix++)
                  {
                     switch(uiCol % 3)
                     {
                        case 0:
                          (*pui32Pix) |= ((pui16rgbaPix->R >> 6) << 22);
                          (*pui32Pix) |= ((pui16rgbaPix->G >> 6) << 12);
                          (*pui32Pix) |= ((pui16rgbaPix->B >> 6) <<  2);
                          pui32Pix++;
                          (*pui32Pix) |= ((pui16rgbaPix->A >> 6) << 22);
                          break;

                        case 1:
                          (*pui32Pix) |= ((pui16rgbaPix->R >> 6) << 12);
                          (*pui32Pix) |= ((pui16rgbaPix->G >> 6) <<  2);
                          pui32Pix++;
                          (*pui32Pix) |= ((pui16rgbaPix->B >> 6) << 22);
                          (*pui32Pix) |= ((pui16rgbaPix->A >> 6) << 12);
                          break;

                        case 2:
                          (*pui32Pix) |= ((pui16rgbaPix->R >> 6) <<  2);
                          pui32Pix++;
                          (*pui32Pix) |= ((pui16rgbaPix->G >> 6) << 22);
                          (*pui32Pix) |= ((pui16rgbaPix->B >> 6) << 12);
                          (*pui32Pix) |= ((pui16rgbaPix->A >> 6) <<  2);
                          pui32Pix++;
                     }
                  }
               }

               if ( pfOutFile != NULL &&
                    fwrite ( acAlignedBuffer, 1, uiRowSizeInBytes+uieolPadding, pfOutFile ) != uiRowSizeInBytes+uieolPadding )
               {
                  gan_err_flush_trace();
                  gan_err_register ( "bWrite10BitDPXImageData", GAN_ERROR_TRUNCATED_FILE, "truncated DPX file" );
                  return GAN_FALSE;
               }
            }

            break;

            default:
              gan_err_flush_trace();
              gan_err_register ( "pgiWrite10BitDPXImageData", GAN_ERROR_NOT_IMPLEMENTED, "only RGB and RGBA formats supported" );
              return GAN_FALSE;
         }
      }        
   }

   free(acBuffer);
   return GAN_TRUE;
}

/**
 * \brief Not a user function
 */
Gan_Bool
 bWrite12BitDPXImageData(FILE* pfOutFile, const Gan_Image* pgiImage, Gan_Bool bNewFile, Gan_Bool bReverseBytes, unsigned int uieolPadding, Gan_Bool bPacked, Gan_Bool bFlip, Gan_Bool bSingleField, Gan_Bool bUpper, Gan_Bool bWholeImage)
{
   unsigned int uiRow, uiHeight=pgiImage->height;
   int iCol;
   unsigned int uiRowSizeInBytes;
   char *acBuffer, *acAlignedBuffer;

   gan_err_flush_trace();
   gan_err_register ( "pgiWrite12BitDPXImageData", GAN_ERROR_NOT_IMPLEMENTED, "" );
   return GAN_FALSE;

   if(bSingleField && !bWholeImage)
      uiHeight *= 2;

   if(bPacked)
   {
      gan_err_flush_trace();
      gan_err_register ( "pgiWrite12BitDPXImageData", GAN_ERROR_NOT_IMPLEMENTED, "12 bit packed format not supported" );
      return GAN_FALSE;
   }

   if(pgiImage->type != GAN_UINT16)
   {
      gan_err_flush_trace();
      gan_err_register ( "pgiWrite12BitDPXImageData", GAN_ERROR_NOT_IMPLEMENTED, "only 12 --> 16 bit conversion supported" );
      return GAN_FALSE;
   }

   /* determine row data size in bytes, excluding padding */
   switch(pgiImage->format)
   {
      case GAN_RGB_COLOUR_IMAGE:
        uiRowSizeInBytes = 4*pgiImage->width;
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "pgiWrite12BitDPXImageData", GAN_ERROR_NOT_IMPLEMENTED, "only RGB format supported" );
        return GAN_FALSE;
   }        

   /* build a buffer to hold a single line of image data and align it to a four byte boundary*/
   acBuffer = malloc(uiRowSizeInBytes+uieolPadding+3);
   if(acBuffer == NULL)
   {
      gan_err_flush_trace();
      gan_err_register_with_number ( "pgiWrite12BitDPXImageData", GAN_ERROR_MALLOC_FAILED, "", uiRowSizeInBytes+uieolPadding+3 );
      return GAN_FALSE;
   }

   acAlignedBuffer = (char*)((unsigned long int)acBuffer + 3 - (((unsigned long int)acBuffer + 3) % 4));

   /* write the image data */
   for(uiRow = 0; uiRow<uiHeight; uiRow++)
   {
      /* ignore row if it's in a field we're not writing */
      if(bSingleField && ((bUpper && ((uiRow % 2) == 1)) || (!bUpper && ((uiRow % 2) == 0))))
      {
         if(bNewFile)
         {
            /* write a blank row */
            memset((void*)acAlignedBuffer, 0, uiRowSizeInBytes+uieolPadding);
            if ( pfOutFile != NULL &&
                 fwrite ( acAlignedBuffer, 1, uiRowSizeInBytes+uieolPadding, pfOutFile ) != uiRowSizeInBytes+uieolPadding )
            {
               gan_err_flush_trace();
               gan_err_register ( "pgiWrite12BitDPXImageData", GAN_ERROR_TRUNCATED_FILE, "truncated DPX file" );
               return GAN_FALSE;
            }
         }
         else
         {
            /* leave this row as is was */
            if(pfOutFile != NULL && fseek(pfOutFile, uiRowSizeInBytes+uieolPadding, SEEK_CUR) != 0)
            {
               gan_err_flush_trace();
               gan_err_register ( "pgiWrite12BitDPXImageData", GAN_ERROR_TRUNCATED_FILE, "truncated DPX file" );
               return GAN_FALSE;
            }
         }
      }
      else
      {
         unsigned long uiImRow;

         if(bSingleField)
         {
            if(bWholeImage)
               uiImRow = (bFlip ? (pgiImage->height-uiRow-1) : uiRow);
            else
               uiImRow = (bFlip ? (pgiImage->height-uiRow/2-1) : uiRow/2);
         }
         else
            uiImRow = (bFlip ? (pgiImage->height-uiRow-1) : uiRow);

         switch(pgiImage->format)
         {
            case GAN_RGB_COLOUR_IMAGE:
            {
               Gan_RGBPixel_ui16* pui16rgbPix;
               gan_uint32* pui32Pix;

               memset((void*)acAlignedBuffer, 0, uiRowSizeInBytes+uieolPadding);
               if(bReverseBytes)
               {
                  for(iCol=(int)pgiImage->width-1, pui16rgbPix = gan_image_get_pixptr_rgb_ui16(pgiImage, uiImRow, iCol),
                      pui32Pix = ((gan_uint32*)acAlignedBuffer) + iCol; iCol >= 0; iCol--, pui16rgbPix--, pui32Pix--)
                  {
                     (*pui32Pix) |= ((pui16rgbPix->R >> 6) << 22);
                     (*pui32Pix) |= ((pui16rgbPix->G >> 6) << 12);
                     (*pui32Pix) |= ((pui16rgbPix->B >> 6) <<  2);
                     vReverseEndianness32(pui32Pix);
                  }
               }
               else
               {
                  for(iCol=(int)pgiImage->width-1, pui16rgbPix = gan_image_get_pixptr_rgb_ui16(pgiImage, uiImRow, iCol),
                      pui32Pix = ((gan_uint32*)acAlignedBuffer) + iCol; iCol >= 0; iCol--, pui16rgbPix--, pui32Pix--)
                  {
                     (*pui32Pix) |= ((pui16rgbPix->R >> 6) << 22);
                     (*pui32Pix) |= ((pui16rgbPix->G >> 6) << 12);
                     (*pui32Pix) |= ((pui16rgbPix->B >> 6) <<  2);
                  }
               }

               if ( pfOutFile != NULL &&
                    fwrite ( acAlignedBuffer, 1, uiRowSizeInBytes+uieolPadding, pfOutFile ) != uiRowSizeInBytes+uieolPadding )
               {
                  gan_err_flush_trace();
                  gan_err_register ( "bWrite12BitDPXImageData", GAN_ERROR_TRUNCATED_FILE, "truncated DPX file" );
                  return GAN_FALSE;
               }
            }

            break;

            default:
              gan_err_flush_trace();
              gan_err_register ( "pgiWrite12BitDPXImageData", GAN_ERROR_NOT_IMPLEMENTED, "only RGB format supported" );
              return GAN_FALSE;
         }
      }        
   }

   free(acBuffer);
   return GAN_TRUE;
}

/**
 * \brief Not a user function
 */
Gan_Bool
 bWrite16BitDPXImageData(FILE* pfOutFile, const Gan_Image* pgiImage, Gan_Bool bNewFile, Gan_Bool bReverseBytes, unsigned int uieolPadding, Gan_Bool bFlip, Gan_Bool bSingleField, Gan_Bool bUpper, Gan_Bool bWholeImage)
{
   unsigned int uiRow, uiHeight=pgiImage->height;
   int iCol;
   unsigned int uiRowSizeInBytes;
   char *acBuffer, *acAlignedBuffer;

   if(bSingleField && !bWholeImage)
      uiHeight *= 2;

   if(pgiImage->type != GAN_UINT16)
   {
      gan_err_flush_trace();
      gan_err_register ( "pgiWrite16BitDPXImageData", GAN_ERROR_NOT_IMPLEMENTED, "no 16 bit conversions supported" );
      return GAN_FALSE;
   }

   /* determine row data size in bytes, and padding to a 32-bit boundary */
   switch(pgiImage->format)
   {
      case GAN_GREY_LEVEL_IMAGE:
        uiRowSizeInBytes = 2*pgiImage->width;
        break;

      case GAN_RGB_COLOUR_IMAGE:
        uiRowSizeInBytes = 6*pgiImage->width;
        break;

      case GAN_RGB_COLOUR_ALPHA_IMAGE:
        uiRowSizeInBytes = 8*pgiImage->width;
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "pgiWrite16BitDPXImageData", GAN_ERROR_NOT_IMPLEMENTED, "image format not supported" );
        return GAN_FALSE;
   }

   /* build a buffer to hold a single line of image data and align it to a four byte boundary*/
   acBuffer = malloc(uiRowSizeInBytes+uieolPadding+3);
   if(acBuffer == NULL)
   {
      gan_err_flush_trace();
      gan_err_register_with_number ( "pgiWrite16BitDPXImageData", GAN_ERROR_MALLOC_FAILED, "", uiRowSizeInBytes+uieolPadding+3 );
      return GAN_FALSE;
   }

   acAlignedBuffer = (char*)((unsigned long int)acBuffer + 3 - (((unsigned long int)acBuffer + 3) % 4));

   /* write the image data */
   for(uiRow = 0; uiRow<uiHeight; uiRow++)
   {
      /* ignore row if it's in a field we're not writing */
      if(bSingleField && ((bUpper && ((uiRow % 2) == 1)) || (!bUpper && ((uiRow % 2) == 0))))
      {
         if(bNewFile)
         {
            /* write a blank row */
            memset((void*)acAlignedBuffer, 0, uiRowSizeInBytes+uieolPadding);
            if ( pfOutFile != NULL &&
                 fwrite ( acAlignedBuffer, 1, uiRowSizeInBytes+uieolPadding, pfOutFile ) != uiRowSizeInBytes+uieolPadding )
            {
               gan_err_flush_trace();
               gan_err_register ( "pgiWrite16BitDPXImageData", GAN_ERROR_TRUNCATED_FILE, "truncated DPX file" );
               return GAN_FALSE;
            }
         }
         else
         {
            /* leave this row as is was */
            if(pfOutFile != NULL && fseek(pfOutFile, uiRowSizeInBytes+uieolPadding, SEEK_CUR) != 0)
            {
               gan_err_flush_trace();
               gan_err_register ( "pgiWrite16BitDPXImageData", GAN_ERROR_TRUNCATED_FILE, "truncated DPX file" );
               return GAN_FALSE;
            }
         }
      }
      else
      {
         unsigned long uiImRow;

         if(bSingleField)
         {
            if(bWholeImage)
               uiImRow = (bFlip ? (pgiImage->height-uiRow-1) : uiRow);
            else
               uiImRow = (bFlip ? (pgiImage->height-uiRow/2-1) : uiRow/2);
         }
         else
            uiImRow = (bFlip ? (pgiImage->height-uiRow-1) : uiRow);

         switch(pgiImage->format)
         {
            case GAN_GREY_LEVEL_IMAGE:
            {
               gan_uint16 *pui16ImPix, *pui16BufPix;

               memset((void*)acAlignedBuffer, 0, uiRowSizeInBytes+uieolPadding);
               if(bReverseBytes)
               {
                  for(iCol=(int)pgiImage->width-1, pui16ImPix = gan_image_get_pixptr_gl_ui16(pgiImage, uiImRow, iCol),
                      pui16BufPix = ((gan_uint16*)acAlignedBuffer) + iCol; iCol >= 0; iCol--, pui16BufPix--, pui16ImPix--)
                  {
                     *pui16BufPix = *pui16ImPix;
                     vReverseEndianness16(pui16BufPix);
                  }
               }
               else
               {
                  for(iCol=(int)pgiImage->width-1, pui16ImPix = gan_image_get_pixptr_gl_ui16(pgiImage, uiImRow, iCol),
                      pui16BufPix = ((gan_uint16*)acAlignedBuffer) + iCol; iCol >= 0; iCol--)
                     *pui16BufPix-- = *pui16ImPix--;
               }

               // write scan-line of data, including padding
               if ( pfOutFile != NULL &&
                    fwrite ( acAlignedBuffer, 1, uiRowSizeInBytes+uieolPadding, pfOutFile ) != uiRowSizeInBytes+uieolPadding )
               {
                  gan_err_flush_trace();
                  gan_err_register ( "pgiWrite16BitDPXImageData", GAN_ERROR_TRUNCATED_FILE, "truncated DPX file" );
                  return GAN_FALSE;
               }
            }

            break;

            case GAN_RGB_COLOUR_IMAGE:
            {
               Gan_RGBPixel_ui16 *pui16rgbImPix, *pui16rgbBufPix;

               memset((void*)acAlignedBuffer, 0, uiRowSizeInBytes+uieolPadding);
               if(bReverseBytes)
               {
                  for(iCol=(int)pgiImage->width-1, pui16rgbImPix = gan_image_get_pixptr_rgb_ui16(pgiImage, uiImRow, iCol),
                      pui16rgbBufPix = ((Gan_RGBPixel_ui16*)acAlignedBuffer) + iCol; iCol >= 0; iCol--, pui16rgbBufPix--, pui16rgbImPix--)
                  {
                     *pui16rgbBufPix = *pui16rgbImPix;
                     vReverseEndianness16(&pui16rgbBufPix->R);
                     vReverseEndianness16(&pui16rgbBufPix->G);
                     vReverseEndianness16(&pui16rgbBufPix->B);
                  }
               }
               else
               {
                  for(iCol=(int)pgiImage->width-1, pui16rgbImPix = gan_image_get_pixptr_rgb_ui16(pgiImage, uiImRow, iCol),
                      pui16rgbBufPix = ((Gan_RGBPixel_ui16*)acAlignedBuffer) + iCol; iCol >= 0; iCol--)
                     *pui16rgbBufPix-- = *pui16rgbImPix--;
               }

               // write scan-line of data, including padding
               if ( pfOutFile != NULL &&
                    fwrite ( acAlignedBuffer, 1, uiRowSizeInBytes+uieolPadding, pfOutFile ) != uiRowSizeInBytes+uieolPadding )
               {
                  gan_err_flush_trace();
                  gan_err_register ( "pgiWrite16BitDPXImageData", GAN_ERROR_TRUNCATED_FILE, "truncated DPX file" );
                  return GAN_FALSE;
               }
            }

            break;

            case GAN_RGB_COLOUR_ALPHA_IMAGE:
            {
               Gan_RGBAPixel_ui16 *pui16rgbaImPix, *pui16rgbaBufPix;

               memset((void*)acAlignedBuffer, 0, uiRowSizeInBytes+uieolPadding);
               if(bReverseBytes)
               {
                  for(iCol=(int)pgiImage->width-1, pui16rgbaImPix = gan_image_get_pixptr_rgba_ui16(pgiImage, uiImRow, iCol),
                      pui16rgbaBufPix = ((Gan_RGBAPixel_ui16*)acAlignedBuffer) + iCol; iCol >= 0; iCol--, pui16rgbaBufPix--, pui16rgbaImPix--)
                  {
                     *pui16rgbaBufPix = *pui16rgbaImPix;
                     vReverseEndianness16(&pui16rgbaBufPix->R);
                     vReverseEndianness16(&pui16rgbaBufPix->G);
                     vReverseEndianness16(&pui16rgbaBufPix->B);
                     vReverseEndianness16(&pui16rgbaBufPix->A);
                  }
               }
               else
               {
                  for(iCol=(int)pgiImage->width-1, pui16rgbaImPix = gan_image_get_pixptr_rgba_ui16(pgiImage, uiImRow, iCol),
                      pui16rgbaBufPix = ((Gan_RGBAPixel_ui16*)acAlignedBuffer) + iCol; iCol >= 0; iCol--)
                     *pui16rgbaBufPix-- = *pui16rgbaImPix--;
               }

               // write scan-line of data, including padding
               if ( pfOutFile != NULL &&
                    fwrite ( acAlignedBuffer, 1, uiRowSizeInBytes+uieolPadding, pfOutFile ) != uiRowSizeInBytes+uieolPadding )
               {
                  gan_err_flush_trace();
                  gan_err_register ( "pgiWrite16BitDPXImageData", GAN_ERROR_TRUNCATED_FILE, "truncated DPX file" );
                  return GAN_FALSE;
               }
            }

            break;

            default:
              gan_err_flush_trace();
              gan_err_register ( "pgiWrite16BitDPXImageData", GAN_ERROR_NOT_IMPLEMENTED, "image format not supported" );
              return GAN_FALSE;
         }        
      }
   }

   free(acBuffer);
   return GAN_TRUE;
}

/**
 * \brief Not a user function
 */
Gan_Bool
 bWrite32BitFloatDPXImageData(FILE* pfOutFile, const Gan_Image* pgiImage, Gan_Bool bNewFile, Gan_Bool bReverseBytes, unsigned int uieolPadding, Gan_Bool bFlip, Gan_Bool bSingleField, Gan_Bool bUpper, Gan_Bool bWholeImage)
{
   unsigned int uiRow, uiHeight=pgiImage->height;
   int iCol;
   unsigned int uiRowSizeInBytes;
   char *acBuffer, *acAlignedBuffer;

   if(bSingleField && !bWholeImage)
      uiHeight *= 2;

   if(pgiImage->type != GAN_FLOAT32)
   {
      gan_err_flush_trace();
      gan_err_register ( "pgiWrite32BitFloatDPXImageData", GAN_ERROR_NOT_IMPLEMENTED, "no 32 bit float conversions supported" );
      return GAN_FALSE;
   }

   /* determine row data size in bytes, and padding to a 32-bit boundary */
   switch(pgiImage->format)
   {
      case GAN_GREY_LEVEL_IMAGE:
        uiRowSizeInBytes = 4*pgiImage->width;
        break;

      case GAN_RGB_COLOUR_IMAGE:
        uiRowSizeInBytes = 12*pgiImage->width;
        break;

      case GAN_RGB_COLOUR_ALPHA_IMAGE:
        uiRowSizeInBytes = 16*pgiImage->width;
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "pgiWrite32BitFloatDPXImageData", GAN_ERROR_NOT_IMPLEMENTED, "image format not supported" );
        return GAN_FALSE;
   }

   /* build a buffer to hold a single line of image data and align it to a four byte boundary*/
   acBuffer = malloc(uiRowSizeInBytes+uieolPadding+3);
   if(acBuffer == NULL)
   {
      gan_err_flush_trace();
      gan_err_register_with_number ( "pgiWrite32BitFloatDPXImageData", GAN_ERROR_MALLOC_FAILED, "", uiRowSizeInBytes+uieolPadding+3 );
      return GAN_FALSE;
   }

   acAlignedBuffer = (char*)((unsigned long int)acBuffer + 3 - (((unsigned long int)acBuffer + 3) % 4));

   /* write the image data */
   for(uiRow = 0; uiRow<uiHeight; uiRow++)
   {
      /* ignore row if it's in a field we're not writing */
      if(bSingleField && ((bUpper && ((uiRow % 2) == 1)) || (!bUpper && ((uiRow % 2) == 0))))
      {
         if(bNewFile)
         {
            /* write a blank row */
            memset((void*)acAlignedBuffer, 0, uiRowSizeInBytes+uieolPadding);
            if ( pfOutFile != NULL &&
                 fwrite ( acAlignedBuffer, 1, uiRowSizeInBytes+uieolPadding, pfOutFile ) != uiRowSizeInBytes+uieolPadding )
            {
               gan_err_flush_trace();
               gan_err_register ( "pgiWrite32BitFloatDPXImageData", GAN_ERROR_TRUNCATED_FILE, "truncated DPX file" );
               return GAN_FALSE;
            }
         }
         else
         {
            /* leave this row as is was */
            if(pfOutFile != NULL && fseek(pfOutFile, uiRowSizeInBytes+uieolPadding, SEEK_CUR) != 0)
            {
               gan_err_flush_trace();
               gan_err_register ( "pgiWrite32BitFloatDPXImageData", GAN_ERROR_TRUNCATED_FILE, "truncated DPX file" );
               return GAN_FALSE;
            }
         }
      }
      else
      {
         unsigned long uiImRow;

         if(bSingleField)
         {
            if(bWholeImage)
               uiImRow = (bFlip ? (pgiImage->height-uiRow-1) : uiRow);
            else
               uiImRow = (bFlip ? (pgiImage->height-uiRow/2-1) : uiRow/2);
         }
         else
            uiImRow = (bFlip ? (pgiImage->height-uiRow-1) : uiRow);

         switch(pgiImage->format)
         {
            case GAN_GREY_LEVEL_IMAGE:
            {
               gan_float32 *pf32ImPix, *pf32BufPix;

               memset((void*)acAlignedBuffer, 0, uiRowSizeInBytes+uieolPadding);
               if(bReverseBytes)
               {
                  for(iCol=(int)pgiImage->width-1, pf32ImPix = gan_image_get_pixptr_gl_f32(pgiImage, uiImRow, iCol),
                      pf32BufPix = ((gan_float32*)acAlignedBuffer) + iCol; iCol >= 0; iCol--, pf32BufPix--, pf32ImPix--)
                  {
                     *pf32BufPix = *pf32ImPix;
                     vReverseEndianness32((gan_uint32*)pf32BufPix);
                  }
               }
               else
               {
                  for(iCol=(int)pgiImage->width-1, pf32ImPix = gan_image_get_pixptr_gl_f32(pgiImage, uiImRow, iCol),
                      pf32BufPix = ((gan_float32*)acAlignedBuffer) + iCol; iCol >= 0; iCol--)
                     *pf32BufPix-- = *pf32ImPix--;
               }

               // write scan-line of data, including padding
               if ( pfOutFile != NULL &&
                    fwrite ( acAlignedBuffer, 1, uiRowSizeInBytes+uieolPadding, pfOutFile ) != uiRowSizeInBytes+uieolPadding )
               {
                  gan_err_flush_trace();
                  gan_err_register ( "pgiWrite32BitFloatDPXImageData", GAN_ERROR_TRUNCATED_FILE, "truncated DPX file" );
                  return GAN_FALSE;
               }
            }

            break;

            case GAN_RGB_COLOUR_IMAGE:
            {
               Gan_RGBPixel_f32 *pf32rgbImPix, *pf32rgbBufPix;

               memset((void*)acAlignedBuffer, 0, uiRowSizeInBytes+uieolPadding);
               if(bReverseBytes)
               {
                  for(iCol=(int)pgiImage->width-1, pf32rgbImPix = gan_image_get_pixptr_rgb_f32(pgiImage, uiImRow, iCol),
                      pf32rgbBufPix = ((Gan_RGBPixel_f32*)acAlignedBuffer) + iCol; iCol >= 0; iCol--, pf32rgbBufPix--, pf32rgbImPix--)
                  {
                     *pf32rgbBufPix = *pf32rgbImPix;
                     vReverseEndianness32((gan_uint32*)&pf32rgbBufPix->R);
                     vReverseEndianness32((gan_uint32*)&pf32rgbBufPix->G);
                     vReverseEndianness32((gan_uint32*)&pf32rgbBufPix->B);
                  }
               }
               else
               {
                  for(iCol=(int)pgiImage->width-1, pf32rgbImPix = gan_image_get_pixptr_rgb_f32(pgiImage, uiImRow, iCol),
                      pf32rgbBufPix = ((Gan_RGBPixel_f32*)acAlignedBuffer) + iCol; iCol >= 0; iCol--)
                     *pf32rgbBufPix-- = *pf32rgbImPix--;
               }

               // write scan-line of data, including padding
               if ( pfOutFile != NULL &&
                    fwrite ( acAlignedBuffer, 1, uiRowSizeInBytes+uieolPadding, pfOutFile ) != uiRowSizeInBytes+uieolPadding )
               {
                  gan_err_flush_trace();
                  gan_err_register ( "pgiWrite32BitFloatDPXImageData", GAN_ERROR_TRUNCATED_FILE, "truncated DPX file" );
                  return GAN_FALSE;
               }
            }

            break;

            case GAN_RGB_COLOUR_ALPHA_IMAGE:
            {
               Gan_RGBAPixel_f32 *pf32rgbaImPix, *pf32rgbaBufPix;

               memset((void*)acAlignedBuffer, 0, uiRowSizeInBytes+uieolPadding);
               if(bReverseBytes)
               {
                  for(iCol=(int)pgiImage->width-1, pf32rgbaImPix = gan_image_get_pixptr_rgba_f32(pgiImage, uiImRow, iCol),
                      pf32rgbaBufPix = ((Gan_RGBAPixel_f32*)acAlignedBuffer) + iCol; iCol >= 0; iCol--, pf32rgbaBufPix--, pf32rgbaImPix--)
                  {
                     *pf32rgbaBufPix = *pf32rgbaImPix;
                     vReverseEndianness32((gan_uint32*)&pf32rgbaBufPix->R);
                     vReverseEndianness32((gan_uint32*)&pf32rgbaBufPix->G);
                     vReverseEndianness32((gan_uint32*)&pf32rgbaBufPix->B);
                     vReverseEndianness32((gan_uint32*)&pf32rgbaBufPix->A);
                  }
               }
               else
               {
                  for(iCol=(int)pgiImage->width-1, pf32rgbaImPix = gan_image_get_pixptr_rgba_f32(pgiImage, uiImRow, iCol),
                      pf32rgbaBufPix = ((Gan_RGBAPixel_f32*)acAlignedBuffer) + iCol; iCol >= 0; iCol--)
                     *pf32rgbaBufPix-- = *pf32rgbaImPix--;
               }

               // write scan-line of data, including padding
               if ( pfOutFile != NULL &&
                    fwrite ( acAlignedBuffer, 1, uiRowSizeInBytes+uieolPadding, pfOutFile ) != uiRowSizeInBytes+uieolPadding )
               {
                  gan_err_flush_trace();
                  gan_err_register ( "pgiWrite32BitFloatDPXImageData", GAN_ERROR_TRUNCATED_FILE, "truncated DPX file" );
                  return GAN_FALSE;
               }
            }

            break;

            default:
              gan_err_flush_trace();
              gan_err_register ( "pgiWrite32BitFloatDPXImageData", GAN_ERROR_NOT_IMPLEMENTED, "image format not supported" );
              return GAN_FALSE;
         }        
      }
   }

   free(acBuffer);
   return GAN_TRUE;
}

static gan_uint8 ui8GetDPXBitSize(const Gan_Image *image, const Gan_ImageWriteControlStruct *octrlstr)
{
   gan_uint8 ui8BitSize = GAN_UINT8_MAX;
   
   if(octrlstr != NULL && octrlstr->info.dpx.image_info.bit_size != 0)
   {
      ui8BitSize = (gan_uint8)octrlstr->info.dpx.image_info.bit_size;

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

         case 12:
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
         case GAN_BOOL:    ui8BitSize =  1; break;
         case GAN_UINT8:   ui8BitSize =  8; break;
         case GAN_UINT16:  ui8BitSize = 16; break;
         case GAN_FLOAT32: ui8BitSize = 32; break;

         default:
           gan_err_flush_trace();
           gan_err_register ( "ui8GetDPXBitSize", GAN_ERROR_NOT_IMPLEMENTED, "unsupported image type" );
           return GAN_UINT8_MAX;
           break;
      }

   return ui8BitSize;
}

static Gan_Bool gan_write_dpx_generic_header ( char *acAlignedHeader, FILE *outfile, Gan_Bool bReversedEndianness,
                                               gan_uint32 ui32FileSize, const Gan_ImageWriteControlStruct *octrlstr )
{
   gan_uint32 ui32GenericSize = 1664;
   gan_uint32 ui32IndustrySize = 384;
   gan_uint32 ui32DittoKey = 0;
   gan_uint32 ui32EncryptionKey = 0xffffffff;

   if(octrlstr != NULL)
   {
      ui32DittoKey      = octrlstr->info.dpx.generic.ditto_key;
      ui32EncryptionKey = octrlstr->info.dpx.generic.encryption_key;
   }

   if(bReversedEndianness)
   {
      *((gan_uint32*)(acAlignedHeader + OFFSET_MAGIC)) = 0x58504453; /* Magic number */
      *((gan_uint32*)(acAlignedHeader + OFFSET_IMAGEOFFSET)) = ui32ReverseEndianness(IMAGE_DATA_OFFSET);
      ui32FileSize = ui32ReverseEndianness(ui32FileSize);
      ui32DittoKey = ui32ReverseEndianness(ui32DittoKey);
      ui32GenericSize = ui32ReverseEndianness(ui32GenericSize);
      ui32IndustrySize = ui32ReverseEndianness(ui32IndustrySize);
      ui32EncryptionKey = ui32ReverseEndianness(ui32EncryptionKey);
   }
   else
   {
      *((gan_uint32*)(acAlignedHeader + OFFSET_MAGIC)) = 0x53445058; /* Magic number */
      *((gan_uint32*)(acAlignedHeader + OFFSET_IMAGEOFFSET)) = IMAGE_DATA_OFFSET;
   }

   if(octrlstr != NULL && strlen(octrlstr->info.dpx.generic.version) > 0)
      strcpy((char *)(acAlignedHeader + OFFSET_VERSION), octrlstr->info.dpx.generic.version);
   else
      strcpy((char *)(acAlignedHeader + OFFSET_VERSION), "V1.0");

   *((gan_uint32*)(acAlignedHeader + OFFSET_DITTOKEY)) = ui32DittoKey;
   if(octrlstr != NULL)
   {
      strncpy((char *)(acAlignedHeader + OFFSET_FILENAME),   octrlstr->info.dpx.generic.file_name,  100);
      strncpy((char *)(acAlignedHeader + OFFSET_CREATETIME), octrlstr->info.dpx.generic.create_time, 24);
      strncpy((char *)(acAlignedHeader + OFFSET_CREATOR),    octrlstr->info.dpx.generic.creator,    100);
      strncpy((char *)(acAlignedHeader + OFFSET_PROJECT),    octrlstr->info.dpx.generic.project,    200);
      strncpy((char *)(acAlignedHeader + OFFSET_COPYRIGHT),  octrlstr->info.dpx.generic.copyright,  200);
   }

   *((gan_uint32*)(acAlignedHeader + OFFSET_FILESIZE)) = ui32FileSize;
   *((gan_uint32*)(acAlignedHeader + OFFSET_GENERICSIZE)) = ui32GenericSize;
   *((gan_uint32*)(acAlignedHeader + OFFSET_INDUSTRYSIZE)) = ui32IndustrySize;
   *((gan_uint32*)(acAlignedHeader + OFFSET_ENCRYPTKEY)) = ui32EncryptionKey;

   /* write file header */
   if(outfile != NULL && fwrite((const void *)acAlignedHeader, 1, 768, outfile) != 768)
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_write_dpx_generic_header", GAN_ERROR_WRITING_TO_FILE, "");
      return GAN_FALSE;
   }

   return GAN_TRUE;
}

static Gan_Bool gan_write_dpx_image_information_header ( char *acAlignedHeader, FILE *outfile, Gan_Bool bReversedEndianness,
                                                         gan_uint8 ui8BitSize, Gan_Bool bPacked, gan_uint32 ui32eolPadding,
                                                         const Gan_Image *image, unsigned int uiHeight,
                                                         const Gan_ImageWriteControlStruct *octrlstr )
{
   gan_uint16 ui16Orientation = 0; /* left-to-right, top-to-bottom */
   gan_uint16 ui16ElementNumber = 1;
   gan_uint32 ui32PixelsPerLine = (gan_uint32)image->width;
   gan_uint32 ui32LinesPerImageElement = (gan_uint32)uiHeight;
   gan_uint32 ui32DataSign = 0; /* data sign, 0 = unsigned, the default */
   gan_uint32 ui32RefLowData=0, ui32RefHighData=0;
   gan_float32 f32RefLowQuantity=0.0F, f32RefHighQuantity=0.0F;

   /* build the image information header */
   memset((void*)acAlignedHeader, 0, 640);

   if(octrlstr != NULL)
   {
      ui32DataSign = (gan_uint32)octrlstr->info.dpx.image_info.data_sign;
      ui32RefLowData = octrlstr->info.dpx.image_info.ref_low_data;
      f32RefLowQuantity = octrlstr->info.dpx.image_info.ref_low_quantity;
      ui32RefHighData = octrlstr->info.dpx.image_info.ref_high_data;
      f32RefHighQuantity = octrlstr->info.dpx.image_info.ref_high_quantity;
   }
   
   if(bReversedEndianness)
   {
      ui16Orientation   = ui16ReverseEndianness(ui16Orientation);
      ui16ElementNumber = ui16ReverseEndianness(ui16ElementNumber);
      ui32PixelsPerLine       = ui32ReverseEndianness(ui32PixelsPerLine);
      ui32LinesPerImageElement = ui32ReverseEndianness(ui32LinesPerImageElement);
      ui32DataSign = ui32ReverseEndianness(ui32DataSign);
      vReverseEndianness32(&ui32RefLowData);
      vReverseEndianness32((gan_uint32 *)(void *)&f32RefLowQuantity);
      vReverseEndianness32(&ui32RefHighData);
      vReverseEndianness32((gan_uint32 *)(void *)&f32RefHighQuantity);
   }

   *((gan_uint16*)(acAlignedHeader + OFFSET_ORIENTATION)) = ui16Orientation;
   *((gan_uint16*)(acAlignedHeader + OFFSET_ELEMENTNUMBER)) = ui16ElementNumber;
   *((gan_uint32*)(acAlignedHeader + OFFSET_PIXELSPERLINE)) = ui32PixelsPerLine;
   *((gan_uint32*)(acAlignedHeader + OFFSET_LINESPERIMAGEELE)) = ui32LinesPerImageElement;
   *((gan_uint32*)(acAlignedHeader + OFFSET_DATASIGN0)) = ui32DataSign;
   *((gan_uint32*)(acAlignedHeader + OFFSET_REFLOWDATA0)) = ui32RefLowData;
   *((gan_float32*)(acAlignedHeader + OFFSET_REFLOWQUANTITY0)) = f32RefLowQuantity;
   *((gan_uint32*)(acAlignedHeader + OFFSET_REFHIGHDATA0)) = ui32RefHighData;
   *((gan_float32*)(acAlignedHeader + OFFSET_REFHIGHQUANTITY0)) = f32RefHighQuantity;
   
   switch(image->format)
   {
      case GAN_GREY_LEVEL_IMAGE:
        *((gan_uint8*)(acAlignedHeader + OFFSET_DESCRIPTOR0)) = 6; /* luminance; might use 4 for alpha instead */
        break;

      case GAN_RGB_COLOUR_IMAGE:
        *((gan_uint8*)(acAlignedHeader + OFFSET_DESCRIPTOR0)) = 50; /* rgb */
        break;

      case GAN_RGB_COLOUR_ALPHA_IMAGE:
        *((gan_uint8*)(acAlignedHeader + OFFSET_DESCRIPTOR0)) = 51; /* rgba */
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_write_dpx_image_information_header", GAN_ERROR_NOT_IMPLEMENTED, "unsupported image format" );
        return GAN_FALSE;
   }

   if(octrlstr == NULL || octrlstr->info.dpx.image_info.transfer == GAN_DPXTRANSFER_DEFAULT)
      *((gan_uint8*)(acAlignedHeader + OFFSET_TRANSFER0)) = (gan_uint8)GAN_DPXTRANSFER_LINEAR;
   else
      *((gan_uint8*)(acAlignedHeader + OFFSET_TRANSFER0)) = (gan_uint8)octrlstr->info.dpx.image_info.transfer;

   if(octrlstr == NULL || octrlstr->info.dpx.image_info.colorimetric == GAN_DPXCOLORIMETRIC_DEFAULT)
      *((gan_uint8*)(acAlignedHeader + OFFSET_COLORIMETRIC0)) = (gan_uint8)GAN_DPXCOLORIMETRIC_PAL;
   else
      *((gan_uint8*)(acAlignedHeader + OFFSET_COLORIMETRIC0)) = (gan_uint8)octrlstr->info.dpx.image_info.colorimetric;

   *((gan_uint8*)(acAlignedHeader + OFFSET_BITSIZE0)) = ui8BitSize; /* bit size, e.g. 10-bit */

   if(bReversedEndianness)
   {
      *((gan_uint16*)(acAlignedHeader + OFFSET_PACKING0)) = ui16ReverseEndianness(bPacked ? DPX_PACKED : DPX_FILLED);
      *((gan_uint16*)(acAlignedHeader + OFFSET_ENCODING0)) = ui16ReverseEndianness(0); /* no encoding */
      *((gan_uint32*)(acAlignedHeader + OFFSET_DATAOFFSET0)) = ui32ReverseEndianness(IMAGE_DATA_OFFSET);   /* Determine data offset */
      *((gan_uint32*)(acAlignedHeader + OFFSET_EOLPADDING0)) = ui32ReverseEndianness(ui32eolPadding); /* end-of-line padding */
      *((gan_uint32*)(acAlignedHeader + OFFSET_EOIMAGEPADDING0)) = ui32ReverseEndianness(0); /* no end-of-image padding */
   }
   else
   {
      *((gan_uint16*)(acAlignedHeader + OFFSET_PACKING0)) = bPacked ? DPX_PACKED : DPX_FILLED;
      *((gan_uint16*)(acAlignedHeader + OFFSET_ENCODING0)) = 0; /* no encoding */
      *((gan_uint32*)(acAlignedHeader + OFFSET_DATAOFFSET0)) = IMAGE_DATA_OFFSET;   /* Determine data offset */
      *((gan_uint32*)(acAlignedHeader + OFFSET_EOLPADDING0)) = ui32eolPadding; /* end-of-line padding */
      *((gan_uint32*)(acAlignedHeader + OFFSET_EOIMAGEPADDING0)) = 0; /* no end-of-image padding */
   }

   if(octrlstr != NULL)
      strncpy((char *)(acAlignedHeader + OFFSET_DESCRIPTION0), octrlstr->info.dpx.image_info.description,  200);
      
   /* write image information header */
   if(outfile != NULL && fwrite((const void *)acAlignedHeader, 1, 640, outfile) != 640)
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_write_dpx_image_information_header", GAN_ERROR_WRITING_TO_FILE, "");
      return GAN_FALSE;
   }

   return GAN_TRUE;
}

static Gan_Bool gan_write_dpx_orientation_header ( char *acAlignedHeader, FILE *outfile, Gan_Bool bReversedEndianness,
                                                   const Gan_ImageWriteControlStruct *octrlstr )
{
   gan_uint32 ui32XOffset=0, ui32YOffset=0, ui32XCentre=0, ui32YCentre=0, ui32XOrigSize=0, ui32YOrigSize=0;
   gan_uint16 aui16Border[4] = {0,0,0,0};
   gan_uint32 aui32PixelAspect[2] = {1,1};
       
   memset((void*)acAlignedHeader, 0, 256);

   if(octrlstr != NULL)
   {
      ui32XOffset = octrlstr->info.dpx.orientation.x_offset;
      ui32YOffset = octrlstr->info.dpx.orientation.y_offset;
      ui32XCentre = octrlstr->info.dpx.orientation.x_centre;
      ui32YCentre = octrlstr->info.dpx.orientation.y_centre;
      ui32XOrigSize = octrlstr->info.dpx.orientation.x_orig_size;
      ui32YOrigSize = octrlstr->info.dpx.orientation.y_orig_size;
      aui16Border[0] = octrlstr->info.dpx.orientation.border[0];
      aui16Border[1] = octrlstr->info.dpx.orientation.border[1];
      aui16Border[2] = octrlstr->info.dpx.orientation.border[2];
      aui16Border[3] = octrlstr->info.dpx.orientation.border[3];
      aui32PixelAspect[0] = octrlstr->info.dpx.orientation.pixel_aspect[0];
      aui32PixelAspect[1] = octrlstr->info.dpx.orientation.pixel_aspect[1];
   }

   if(bReversedEndianness)
   {
      vReverseEndianness32(&ui32XOffset);
      vReverseEndianness32(&ui32YOffset);
      vReverseEndianness32(&ui32XCentre);
      vReverseEndianness32(&ui32YCentre);
      vReverseEndianness32(&ui32XOrigSize);
      vReverseEndianness32(&ui32YOrigSize);
      vReverseEndianness16(&aui16Border[0]);
      vReverseEndianness16(&aui16Border[1]);
      vReverseEndianness16(&aui16Border[2]);
      vReverseEndianness16(&aui16Border[3]);
      vReverseEndianness32(&aui32PixelAspect[0]);
      vReverseEndianness32(&aui32PixelAspect[1]);
   }

   *((gan_uint32*)(acAlignedHeader + OFFSET_ORIENTATION_XOFFSET)) = ui32XOffset;
   *((gan_uint32*)(acAlignedHeader + OFFSET_ORIENTATION_YOFFSET)) = ui32YOffset;
   *((gan_uint32*)(acAlignedHeader + OFFSET_ORIENTATION_XCENTRE)) = ui32XCentre;
   *((gan_uint32*)(acAlignedHeader + OFFSET_ORIENTATION_YCENTRE)) = ui32YCentre;
   *((gan_uint32*)(acAlignedHeader + OFFSET_ORIENTATION_XORIGSIZE)) = ui32XOrigSize;
   *((gan_uint32*)(acAlignedHeader + OFFSET_ORIENTATION_YORIGSIZE)) = ui32YOrigSize;

   if(octrlstr != NULL)
   {
      strncpy((char *)(acAlignedHeader + OFFSET_ORIENTATION_FILENAME), octrlstr->info.dpx.orientation.file_name, 100);
      strncpy((char *)(acAlignedHeader + OFFSET_ORIENTATION_CREATIONTIME), octrlstr->info.dpx.orientation.creation_time, 24);
      strncpy((char *)(acAlignedHeader + OFFSET_ORIENTATION_INPUTDEV), octrlstr->info.dpx.orientation.input_dev, 24);
      strncpy((char *)(acAlignedHeader + OFFSET_ORIENTATION_INPUTSERIAL), octrlstr->info.dpx.orientation.input_serial, 32);
   }
      
   *((gan_uint16*)(acAlignedHeader + OFFSET_ORIENTATION_BORDER + 0)) = aui16Border[0];
   *((gan_uint16*)(acAlignedHeader + OFFSET_ORIENTATION_BORDER + 2)) = aui16Border[1];
   *((gan_uint16*)(acAlignedHeader + OFFSET_ORIENTATION_BORDER + 4)) = aui16Border[2];
   *((gan_uint16*)(acAlignedHeader + OFFSET_ORIENTATION_BORDER + 6)) = aui16Border[3];
   *((gan_uint32*)(acAlignedHeader + OFFSET_ORIENTATION_PIXELASPECT + 0)) = aui32PixelAspect[0];
   *((gan_uint32*)(acAlignedHeader + OFFSET_ORIENTATION_PIXELASPECT + 4)) = aui32PixelAspect[1];

   if(outfile != NULL && fwrite((const void *)acAlignedHeader, 1, 256, outfile) != 256)
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_write_dpx_orientation_header", GAN_ERROR_WRITING_TO_FILE, "");
      return GAN_FALSE;
   }

   return GAN_TRUE;
}

static Gan_Bool gan_write_dpx_film_header ( char *acAlignedHeader, FILE *outfile, Gan_Bool bReversedEndianness,
                                            const Gan_ImageWriteControlStruct *octrlstr )
{
   gan_float32 f32FrameRate=24.0F, f32ShutterAngle=25.0F;
   gan_uint32 ui32FramePosition=0, ui32SequenceLen=1, ui32HeldCount=1;
   
   memset((void*)acAlignedHeader, 0, 256);
   if(octrlstr != NULL)
   {
      acAlignedHeader[OFFSET_FILM_MFGID+0] = octrlstr->info.dpx.film.film_mfg_id[0];
      acAlignedHeader[OFFSET_FILM_MFGID+1] = octrlstr->info.dpx.film.film_mfg_id[1];
      acAlignedHeader[OFFSET_FILM_FILMTYPE+0] = octrlstr->info.dpx.film.film_type[0];
      acAlignedHeader[OFFSET_FILM_FILMTYPE+1] = octrlstr->info.dpx.film.film_type[1];
      acAlignedHeader[OFFSET_FILM_OFFSET+0] = octrlstr->info.dpx.film.offset[0];
      acAlignedHeader[OFFSET_FILM_OFFSET+1] = octrlstr->info.dpx.film.offset[1];
      strncpy((char *)(acAlignedHeader + OFFSET_FILM_PREFIX), octrlstr->info.dpx.film.prefix, 6);
      strncpy((char *)(acAlignedHeader + OFFSET_FILM_COUNT), octrlstr->info.dpx.film.count, 4);
      strncpy((char *)(acAlignedHeader + OFFSET_FILM_FORMAT), octrlstr->info.dpx.film.format, 32);
      strncpy((char *)(acAlignedHeader + OFFSET_FILM_FRAMEID), octrlstr->info.dpx.film.frame_id, 32);
      strncpy((char *)(acAlignedHeader + OFFSET_FILM_SLATEINFO), octrlstr->info.dpx.film.slate_info, 100);

      ui32FramePosition = octrlstr->info.dpx.film.frame_position;
      ui32SequenceLen = octrlstr->info.dpx.film.sequence_len;
      ui32HeldCount = octrlstr->info.dpx.film.held_count;
      f32FrameRate = octrlstr->info.dpx.film.frame_rate;
      f32ShutterAngle = octrlstr->info.dpx.film.shutter_angle;
   }

   if(bReversedEndianness)
   {
      vReverseEndianness32(&ui32FramePosition);
      vReverseEndianness32(&ui32SequenceLen);
      vReverseEndianness32(&ui32HeldCount);
      vReverseEndianness32((gan_uint32 *)(void *)&f32FrameRate);
      vReverseEndianness32((gan_uint32 *)(void *)&f32ShutterAngle);
   }

   *((gan_uint32*)(acAlignedHeader + OFFSET_FILM_FRAMEPOSITION)) = ui32FramePosition;
   *((gan_uint32*)(acAlignedHeader + OFFSET_FILM_SEQUENCE_LEN)) = ui32SequenceLen;
   *((gan_uint32*)(acAlignedHeader + OFFSET_FILM_HELDCOUNT)) = ui32HeldCount;
   *((gan_float32*)(acAlignedHeader + OFFSET_FILM_FRAMERATE))    = f32FrameRate;
   *((gan_float32*)(acAlignedHeader + OFFSET_FILM_SHUTTERANGLE)) = f32ShutterAngle;

   if(outfile != NULL && fwrite((const void *)acAlignedHeader, 1, 256, outfile) != 256)
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_write_dpx_film_header", GAN_ERROR_WRITING_TO_FILE, "");
      return GAN_FALSE;
   }

   return GAN_TRUE;
}

static Gan_Bool gan_write_dpx_tv_header ( char *acAlignedHeader, FILE *outfile, Gan_Bool bReversedEndianness,
                                          const Gan_ImageWriteControlStruct *octrlstr )
{
   gan_uint32 ui32TimeCode=0, ui32UserBits=0;
   gan_float32 f32HorSampleRate=1.0F, f32VerSampleRate=1.0F, f32FrameRate=30.0F, f32TimeOffset=0.0F, f32Gamma=1.0F, f32BlackLevel=0.0F;
   gan_float32 f32BlackGain=1.0F, f32BreakPoint=0.0F, f32WhiteLevel=1.0F, f32IntegrationTimes=0.01F;

   memset((void*)acAlignedHeader, 0, 128);

   if(octrlstr != NULL)
   {
      ui32TimeCode = octrlstr->info.time_code;
      ui32UserBits = octrlstr->info.user_bits;

      *((gan_uint8*)(acAlignedHeader + OFFSET_TV_INTERLACE)) = octrlstr->info.interlaced ? 1 : 0;
      *((gan_uint8*)(acAlignedHeader + OFFSET_TV_FIELD_NUMBER)) = octrlstr->info.dpx.tv.field_num;
      *((gan_uint8*)(acAlignedHeader + OFFSET_TV_VIDEO_SIGNAL)) = octrlstr->info.dpx.tv.video_signal;

      f32HorSampleRate = octrlstr->info.dpx.tv.hor_sample_rate;
      f32VerSampleRate = octrlstr->info.dpx.tv.ver_sample_rate;
      f32FrameRate = octrlstr->info.dpx.tv.frame_rate;
      f32TimeOffset = octrlstr->info.dpx.tv.time_offset;
      f32Gamma = octrlstr->info.dpx.tv.gamma;
      f32BlackLevel = octrlstr->info.dpx.tv.black_level;
      f32BlackGain = octrlstr->info.dpx.tv.black_gain;
      f32BreakPoint = octrlstr->info.dpx.tv.break_point;
      f32WhiteLevel = octrlstr->info.dpx.tv.white_level;
      f32IntegrationTimes = octrlstr->info.dpx.tv.integration_times;
   }

   if(bReversedEndianness)
   {
      vReverseEndianness32(&ui32TimeCode);
      vReverseEndianness32(&ui32UserBits);
      vReverseEndianness32((gan_uint32 *)(void *)&f32HorSampleRate);
      vReverseEndianness32((gan_uint32 *)(void *)&f32VerSampleRate);
      vReverseEndianness32((gan_uint32 *)(void *)&f32FrameRate);
      vReverseEndianness32((gan_uint32 *)(void *)&f32TimeOffset);
      vReverseEndianness32((gan_uint32 *)(void *)&f32Gamma);
      vReverseEndianness32((gan_uint32 *)(void *)&f32BlackLevel);
      vReverseEndianness32((gan_uint32 *)(void *)&f32BlackGain);
      vReverseEndianness32((gan_uint32 *)(void *)&f32BreakPoint);
      vReverseEndianness32((gan_uint32 *)(void *)&f32WhiteLevel);
      vReverseEndianness32((gan_uint32 *)(void *)&f32IntegrationTimes);
   }
   
   *((gan_uint32*)(acAlignedHeader + OFFSET_TV_TIMECODE)) = ui32TimeCode;
   *((gan_uint32*)(acAlignedHeader + OFFSET_TV_USERBITS)) = ui32UserBits;
   *((gan_float32*)(acAlignedHeader + OFFSET_TV_HORZSAMPLERATE)) = f32HorSampleRate;
   *((gan_float32*)(acAlignedHeader + OFFSET_TV_VERTSAMPLERATE)) = f32HorSampleRate;
   *((gan_float32*)(acAlignedHeader + OFFSET_TV_FRAMERATE)) = f32FrameRate;
   *((gan_float32*)(acAlignedHeader + OFFSET_TV_TIMEOFFSET)) = f32TimeOffset;
   *((gan_float32*)(acAlignedHeader + OFFSET_TV_GAMMA)) = f32Gamma;
   *((gan_float32*)(acAlignedHeader + OFFSET_TV_BLACKLEVEL)) = f32BlackLevel;
   *((gan_float32*)(acAlignedHeader + OFFSET_TV_BLACKGAIN)) = f32BlackGain;
   *((gan_float32*)(acAlignedHeader + OFFSET_TV_BREAKPOINT)) = f32BreakPoint;
   *((gan_float32*)(acAlignedHeader + OFFSET_TV_WHITELEVEL)) = f32WhiteLevel;
   *((gan_float32*)(acAlignedHeader + OFFSET_TV_INTEGRATIONTIME)) = f32IntegrationTimes;

   if(outfile != NULL && fwrite((const void *)acAlignedHeader, 1, 128, outfile) != 128)
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_write_dpx_tv_header", GAN_ERROR_WRITING_TO_FILE, "");
      return GAN_FALSE;
   }

   return GAN_TRUE;
}

/**
 * \brief Writes a RGB colour image to a file stream in DPX format.
 * \param outfile The file stream
 * \param image The image structure to write to the file
 * \param new_file Whether the file does not currently exist
 * \param octrlstr Pointer to structure controlling output or \c NULL
 * \return #GAN_TRUE on successful write operation, or #GAN_FALSE on failure.
 *
 * Writes the \a image into the file stream \a outfile in DPX format.
 *
 * \sa gan_read_dpx_image_stream().
 */
Gan_Bool
 gan_write_dpx_image_stream ( FILE *outfile, const Gan_Image *image, Gan_Bool new_file, const struct Gan_ImageWriteControlStruct *octrlstr )
{
   char acHeader[BIG_BUFFER_SIZE], *acAlignedHeader;
   gan_uint8 ui8BitSize;
   gan_uint32 ui32eolPadding = 0;
   Gan_Bool bPacked=GAN_FALSE;
   Gan_Bool flip=GAN_FALSE, single_field=GAN_FALSE, upper=GAN_FALSE, whole_image=GAN_FALSE, bReversedEndianness=GAN_FALSE;
   unsigned int uiHeight = image->height;

   /* read generic control parameters */
   if(octrlstr != NULL)
   {
      flip         = octrlstr->flip;
      single_field = octrlstr->single_field;
      upper        = octrlstr->upper;
      whole_image  = octrlstr->whole_image;
      bReversedEndianness = octrlstr->reverse_bytes;
   }

   if(single_field && !whole_image)
      uiHeight *= 2;

   /* determine bit size to use */
   ui8BitSize = ui8GetDPXBitSize(image, octrlstr);
   if(ui8BitSize == GAN_UINT8_MAX)
   {
      gan_err_register ( "gan_write_dpx_image_stream", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   /* align the header array */
   acAlignedHeader = (char*)((unsigned long int)acHeader + 7 - (((unsigned long int)acHeader + 7) % 8));

   /* build file header */
   memset((void*)acAlignedHeader, 0, 768);
   
   /* determine whether to pack the data */
   if(octrlstr != NULL)
      bPacked = octrlstr->info.dpx.image_info.packed;

   /* write generic header */
   if(!gan_write_dpx_generic_header ( acAlignedHeader, outfile, bReversedEndianness,
                                      ui32DPXFileSize(IMAGE_DATA_OFFSET, image->format, ui8BitSize, bPacked, uiHeight, image->width, &ui32eolPadding),
                                      octrlstr ))
   {
      gan_err_register ( "gan_write_dpx_image_stream", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   /* write image information header */
   if(!gan_write_dpx_image_information_header ( acAlignedHeader, outfile, bReversedEndianness, ui8BitSize, bPacked, ui32eolPadding, image, uiHeight, octrlstr ))
   {
      gan_err_register ( "gan_write_dpx_image_stream", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }
   
   /* write the orientation header */
   if(!gan_write_dpx_orientation_header ( acAlignedHeader, outfile, bReversedEndianness, octrlstr ))
   {
      gan_err_register ( "gan_write_dpx_image_stream", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   /* write the film information header */
   if(!gan_write_dpx_film_header ( acAlignedHeader, outfile, bReversedEndianness, octrlstr ))
   {
      gan_err_register ( "gan_write_dpx_image_stream", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }
   
   /* write the TV information header */
   if(!gan_write_dpx_tv_header ( acAlignedHeader, outfile, bReversedEndianness, octrlstr ))
   {
      gan_err_register ( "gan_write_dpx_image_stream", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   /* write zeros for rest of header */
   {
      unsigned int uiNumBytesLeftToWrite = IMAGE_DATA_OFFSET - 768 - 640 - 256 - 256 - 128;

      memset((void*)acHeader, 0, BIG_BUFFER_SIZE);
      while(uiNumBytesLeftToWrite > 0)
      {
         unsigned int uiNumBytes = gan_min2(uiNumBytesLeftToWrite, BIG_BUFFER_SIZE);

         if(outfile != NULL && fwrite((const void*)acHeader, 1, uiNumBytes, outfile) != uiNumBytes)
         {
            gan_err_flush_trace();
            gan_err_register ( "gan_write_dpx_image_stream", GAN_ERROR_WRITING_TO_FILE, "");
            return GAN_FALSE;
         }

         uiNumBytesLeftToWrite -= uiNumBytes;
      }
   }

   /* now write the image data */
   switch(ui8BitSize)
   {
      case 1:
        if(!bWrite1BitDPXImageData(outfile, image, new_file, ui32eolPadding, flip, single_field, upper, whole_image))
        {
           gan_err_register ( "gan_write_dpx_image_stream", GAN_ERROR_FAILURE, "" );
           return GAN_FALSE;
        }

        break;

      case 8:
        if(!bWrite8BitDPXImageData(outfile, image, new_file, ui32eolPadding, flip, single_field, upper, whole_image))
        {
           gan_err_register ( "gan_write_dpx_image_stream", GAN_ERROR_FAILURE, "" );
           return GAN_FALSE;
        }

        break;

      case 10:
        if(!bWrite10BitDPXImageData(outfile, image, new_file, bReversedEndianness, ui32eolPadding, bPacked, flip, single_field, upper, whole_image))
        {
           gan_err_register ( "gan_write_dpx_image_stream", GAN_ERROR_FAILURE, "" );
           return GAN_FALSE;
        }

        break;

      case 12:
        if(!bWrite12BitDPXImageData(outfile, image, new_file, bReversedEndianness, ui32eolPadding, bPacked, flip, single_field, upper, whole_image))
        {
           gan_err_register ( "gan_write_dpx_image_stream", GAN_ERROR_FAILURE, "" );
           return GAN_FALSE;
        }

        break;

      case 16:
        if(!bWrite16BitDPXImageData(outfile, image, new_file, bReversedEndianness, ui32eolPadding, flip, single_field, upper, whole_image))
        {
           gan_err_register ( "gan_write_dpx_image_stream", GAN_ERROR_FAILURE, "" );
           return GAN_FALSE;
        }

        break;

      case 32:
        if(!bWrite32BitFloatDPXImageData(outfile, image, new_file, bReversedEndianness, ui32eolPadding, flip, single_field, upper, whole_image))
        {
           gan_err_register ( "gan_write_dpx_image_stream", GAN_ERROR_FAILURE, "" );
           return GAN_FALSE;
        }

        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_write_dpx_image_stream", GAN_ERROR_NOT_IMPLEMENTED, "unsupported DPX bit depth" );
        return GAN_FALSE;
   }

   /* success */
   return GAN_TRUE;
}

/**
 * \brief Writes a RGB colour image file in DPX format.
 * \param filename The name of the image file
 * \param image The image structure to write to the file
 * \param octrlstr Pointer to structure controlling output or \c NULL
 * \return #GAN_TRUE on successful write operation, #GAN_FALSE on failure.
 *
 * Writes the \a image into DPX file \a filename.
 *
 * \sa gan_read_dpx_image().
 */
Gan_Bool
 gan_write_dpx_image ( const char *filename, const Gan_Image *image, const struct Gan_ImageWriteControlStruct *octrlstr )
{
   if(filename == NULL)
      return gan_write_dpx_image_stream ( NULL, image, GAN_FALSE, octrlstr );
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

            ui8BitSize = ui8GetDPXBitSize(image, octrlstr);
            if(ui8BitSize == GAN_UINT8_MAX)
            {
               fclose(outfile);
               gan_err_register ( "gan_write_dpx_image", GAN_ERROR_FAILURE, filename );
               return GAN_FALSE;
            }

            if(!octrlstr->whole_image)
               uiHeight *= 2;

            uiFileSize = ui32DPXFileSize(IMAGE_DATA_OFFSET, image->format, ui8BitSize, (octrlstr == NULL) ? GAN_FALSE : octrlstr->info.dpx.image_info.packed,
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
         gan_err_register ( "gan_write_dpx_image", GAN_ERROR_OPENING_FILE, filename );
         return GAN_FALSE;
      }

      result = gan_write_dpx_image_stream ( outfile, image, new_file, octrlstr );
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

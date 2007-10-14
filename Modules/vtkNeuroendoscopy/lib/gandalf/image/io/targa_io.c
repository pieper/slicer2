/**
 * File:          $RCSfile: targa_io.c,v $
 * Module:        TARGA format image file I/O functions
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
#include <gandalf/image/io/targa_io.h>
#include <gandalf/image/io/image_io.h>
#include <gandalf/image/image_rgb_uint8.h>
#include <gandalf/image/image_rgba_uint8.h>
#include <gandalf/common/misc_error.h>

/**
 * \addtogroup ImagePackage
 * \{
 */

/**
 * \addtogroup ImageIO
 * \{
 */

#define TARGA_ID_LENGTH_OFFSET               0
#define TARGA_COLOURMAP_TYPE_OFFSET          1
#define TARGA_DATA_TYPE_OFFSET               2
#define TARGA_COLOURMAP_ORIGIN_OFFSET        3
#define TARGA_COLOURMAP_LENGTH_OFFSET        5
#define TARGA_COLOURMAP_ENTRYSIZE_OFFSET     7
#define TARGA_XORIGIN_OFFSET                 8
#define TARGA_YORIGIN_OFFSET                10
#define TARGA_WIDTH_OFFSET                  12
#define TARGA_HEIGHT_OFFSET                 14
#define TARGA_PIXEL_SIZE_OFFSET             16
#define TARGA_IMAGE_DESCRIPTOR_OFFSET       17

#define TARGA_DATA_TYPE_MAPPED      1
#define TARGA_DATA_TYPE_RGB         2
#define TARGA_DATA_TYPE_RLE_MAPPED  9
#define TARGA_DATA_TYPE_RLE_RGB    10


/**
 * \brief Determines whether an image is a TARGA image using the magic number
 */
Gan_Bool gan_image_is_targa(const unsigned char *magic_string, size_t length)
{
   if (length < 18)
      return(GAN_FALSE);

   if ((magic_string[TARGA_COLOURMAP_TYPE_OFFSET] == 0 || magic_string[TARGA_COLOURMAP_TYPE_OFFSET] == 1)
       && (magic_string[TARGA_DATA_TYPE_OFFSET] == TARGA_DATA_TYPE_MAPPED     || magic_string[TARGA_DATA_TYPE_OFFSET] == TARGA_DATA_TYPE_RGB ||
           magic_string[TARGA_DATA_TYPE_OFFSET] == TARGA_DATA_TYPE_RLE_MAPPED || magic_string[TARGA_DATA_TYPE_OFFSET] == TARGA_DATA_TYPE_RLE_RGB)
       && (magic_string[TARGA_PIXEL_SIZE_OFFSET] == 24 || magic_string[TARGA_PIXEL_SIZE_OFFSET] == 32))
      return(GAN_TRUE);

   return(GAN_FALSE);
}

#define BIG_BUFFER_SIZE 264

/* swap bytes */
static void
 swap_rgb_to_bgr(Gan_RGBPixel_ui8* argbPix, int iSize)
{
   gan_uint8 ui8Tmp;

   for(iSize--; iSize>=0; iSize--, argbPix++)
   {
      ui8Tmp = argbPix->R;
      argbPix->R = argbPix->B;
      argbPix->B = ui8Tmp;
   }
}

static void
 swap_rgba_to_bgra(Gan_RGBAPixel_ui8* argbaPix, int iSize)
{
   gan_uint8 ui8Tmp;

   for(iSize--; iSize>=0; iSize--, argbaPix++)
   {
      ui8Tmp = argbaPix->R;
      argbaPix->R = argbaPix->B;
      argbaPix->B = ui8Tmp;
   }
}

/**
 * \brief Reads a RGB colour image file in TARGA format from a stream.
 * \param infile The file stream to be read
 * \param image The image structure to read the image data into or \c NULL
 * \param ictrlstr Pointer to structure controlling input or \c NULL
 * \param header Pointer to file header structure to be filled, or \c NULL
 * \return Pointer to image structure, or \c NULL on failure.
 *
 * Reads the TARGA image from the given file stream \a infile into the given
 * \a image. If \a image is \c NULL a new image is dynamically allocated,
 * otherwise the already allocated \a image structure is reused.
 *
 * \sa gan_write_targa_image_stream().
 */
Gan_Image *
 gan_read_targa_image_stream ( FILE *infile, Gan_Image *image, const struct Gan_ImageReadControlStruct *ictrlstr, struct Gan_ImageHeaderStruct *header )
{
   char acHeader[BIG_BUFFER_SIZE], *acAlignedHeader;
   int iWidth, iHeight, iInternalHeight;
   gan_uint8 ui8IDLength, ui8ColourmapType, ui8DataType, ui8PixelSize, ui8ImageDescriptor;
   Gan_Bool flip=GAN_FALSE, single_field=GAN_FALSE, upper=GAN_FALSE, whole_image=GAN_FALSE;
   Gan_ImageFormat eFormat;
   Gan_Pixel* apxlColourmap=NULL;

   /* align the header array */
   acAlignedHeader = (char*)((unsigned long int)acHeader + 7 - (((unsigned long int)acHeader + 7) % 8));

   /* read the file header */
   if(fread(acAlignedHeader, 1, 18, infile) != 18)
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_read_targa_image_stream", GAN_ERROR_CORRUPTED_FILE, "corrupted Targa file header (truncated file?)" );
      return NULL;
   }

   ui8IDLength        = *((gan_uint8*)&acAlignedHeader[TARGA_ID_LENGTH_OFFSET]);
   ui8ColourmapType   = *((gan_uint8*)&acAlignedHeader[TARGA_COLOURMAP_TYPE_OFFSET]);
   ui8DataType        = *((gan_uint8*)&acAlignedHeader[TARGA_DATA_TYPE_OFFSET]);
   iWidth             = (int) (*((gan_uint8*)&acAlignedHeader[TARGA_WIDTH_OFFSET])  + 0x100**((gan_uint8*)&acAlignedHeader[TARGA_WIDTH_OFFSET+1]));
   iHeight            = (int) (*((gan_uint8*)&acAlignedHeader[TARGA_HEIGHT_OFFSET]) + 0x100**((gan_uint8*)&acAlignedHeader[TARGA_HEIGHT_OFFSET+1]));
   ui8PixelSize       = *((gan_uint8*)&acAlignedHeader[TARGA_PIXEL_SIZE_OFFSET]);
   ui8ImageDescriptor = *((gan_uint8*)&acAlignedHeader[TARGA_IMAGE_DESCRIPTOR_OFFSET]);

   if(header != NULL)
      header->info.interlaced = (Gan_Bool)((ui8ImageDescriptor & 0x40) != 0);

   /* jump over comment */
   if(ui8IDLength != 0 && fseek(infile, (long)ui8IDLength, SEEK_CUR) != 0)
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_read_targa_image_stream", GAN_ERROR_TRUNCATED_FILE, "truncated TARGA file" );
      return NULL;
   }

   /* read colourmap entries */
   if(ui8ColourmapType != 0)
   {
      /* gan_uint16 ui16ColourmapOrigin = *((gan_uint16*)&acAlignedHeader[TARGA_COLOURMAP_ORIGIN_OFFSET]); align properly as below */
      gan_uint16 ui16ColourmapLength = *((gan_uint8*)&acAlignedHeader[TARGA_COLOURMAP_LENGTH_OFFSET])
                                       + 0x100**((gan_uint8*)&acAlignedHeader[TARGA_COLOURMAP_LENGTH_OFFSET+1]);
      gan_uint16 ui16Entry;
      gan_uint8 ui8ColourmapEntrySize = *((gan_uint8*)&acAlignedHeader[TARGA_COLOURMAP_ENTRYSIZE_OFFSET]);

      apxlColourmap = gan_malloc_array(Gan_Pixel, ui16ColourmapLength);
      if(apxlColourmap == NULL)
      {
         gan_err_flush_trace();
         gan_err_register_with_number ( "gan_read_targa_image_stream", GAN_ERROR_MALLOC_FAILED, "", ui16ColourmapLength*sizeof(Gan_Pixel) );
         return NULL;
      }

      switch(ui8ColourmapEntrySize)
      {
         case 8:
         default:
         {
            gan_uint8* aui8Colourmap;

            aui8Colourmap = gan_malloc_array(gan_uint8, ui16ColourmapLength);
            if(aui8Colourmap == NULL)
            {
               gan_err_flush_trace();
               gan_err_register_with_number ( "gan_read_targa_image_stream", GAN_ERROR_MALLOC_FAILED, "", ui16ColourmapLength*sizeof(gan_uint8) );
               return NULL;
            }
            
            /* read the colourmap */
            if(fread(aui8Colourmap, 1, ui16ColourmapLength, infile) != ui16ColourmapLength)
            {
               gan_err_flush_trace();
               gan_err_register ( "gan_read_targa_image_stream", GAN_ERROR_CORRUPTED_FILE, "corrupted Targa file header (truncated file?)" );
               return NULL;
            }

            /* convert the colourmap */
            for(ui16Entry=0; ui16Entry<ui16ColourmapLength; ui16Entry++)
            {
               apxlColourmap[ui16Entry].format = GAN_RGB_COLOUR_IMAGE;
               apxlColourmap[ui16Entry].type = GAN_UINT8;
               apxlColourmap[ui16Entry].data.rgb.ui8.R = aui8Colourmap[ui16Entry];
               apxlColourmap[ui16Entry].data.rgb.ui8.G = aui8Colourmap[ui16Entry];
               apxlColourmap[ui16Entry].data.rgb.ui8.B = aui8Colourmap[ui16Entry];
            }
            
            free(aui8Colourmap);
         }
         break;

         case 15:
         case 16:
         {
            gan_uint8* aui8Colourmap;
            gan_uint16 ui16Val;

            aui8Colourmap = gan_malloc_array(gan_uint8, 2*ui16ColourmapLength);
            if(aui8Colourmap == NULL)
            {
               gan_err_flush_trace();
               gan_err_register_with_number ( "gan_read_targa_image_stream", GAN_ERROR_MALLOC_FAILED, "", ui16ColourmapLength*sizeof(gan_uint8) );
               return NULL;
            }
            
            /* read the colourmap */
            if(fread(aui8Colourmap, 2, ui16ColourmapLength, infile) != ui16ColourmapLength)
            {
               gan_err_flush_trace();
               gan_err_register ( "gan_read_targa_image_stream", GAN_ERROR_CORRUPTED_FILE, "corrupted Targa file header (truncated file?)" );
               return NULL;
            }

            /* convert the colourmap */
            for(ui16Entry=0; ui16Entry<ui16ColourmapLength; ui16Entry++)
            {
               apxlColourmap[ui16Entry].format = GAN_RGB_COLOUR_IMAGE;
               apxlColourmap[ui16Entry].type = GAN_UINT8;
               ui16Val = aui8Colourmap[2*ui16Entry] + 0x100*aui8Colourmap[2*ui16Entry+1];
               apxlColourmap[ui16Entry].data.rgb.ui8.R = (ui16Val & 0x7c00) >> 7;
               apxlColourmap[ui16Entry].data.rgb.ui8.G = (ui16Val &  0x3e0) >> 2;
               apxlColourmap[ui16Entry].data.rgb.ui8.B = (ui16Val &   0x1f) << 3;
            }
            
            free(aui8Colourmap);
         }
         break;

         case 24:
         case 32:
         {
            gan_uint8* aui8Colourmap;

            aui8Colourmap = gan_malloc_array(gan_uint8, ui16ColourmapLength);
            if(aui8Colourmap == NULL)
            {
               gan_err_flush_trace();
               gan_err_register_with_number ( "gan_read_targa_image_stream", GAN_ERROR_MALLOC_FAILED, "", ui16ColourmapLength*sizeof(gan_uint8) );
               return NULL;
            }
            
            /* read the colourmap */
            if(fread(aui8Colourmap, 3, ui16ColourmapLength, infile) != ui16ColourmapLength)
            {
               gan_err_flush_trace();
               gan_err_register ( "gan_read_targa_image_stream", GAN_ERROR_CORRUPTED_FILE, "corrupted Targa file header (truncated file?)" );
               return NULL;
            }

            /* convert the colourmap */
            for(ui16Entry=0; ui16Entry<ui16ColourmapLength; ui16Entry++)
            {
               apxlColourmap[ui16Entry].format = GAN_RGB_COLOUR_IMAGE;
               apxlColourmap[ui16Entry].type   = GAN_UINT8;
               apxlColourmap[ui16Entry].data.rgb.ui8.R = aui8Colourmap[3*ui16Entry+2];
               apxlColourmap[ui16Entry].data.rgb.ui8.G = aui8Colourmap[3*ui16Entry+1];
               apxlColourmap[ui16Entry].data.rgb.ui8.B = aui8Colourmap[3*ui16Entry+0];
            }
            
            free(aui8Colourmap);
         }

         break;
      }
   }
   
   if(ui8PixelSize == 32)
      eFormat = GAN_RGB_COLOUR_ALPHA_IMAGE;
   else if(ui8PixelSize == 24)
      eFormat = GAN_RGB_COLOUR_IMAGE;
   else
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_read_targa_image_stream", GAN_ERROR_CORRUPTED_FILE, "corrupted Targa file header" );
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

   /* if the image specifies a lower left reference, swap our flip variable */
   if((ui8ImageDescriptor & 0x20) == 0)
   {
      flip = !flip;

      /* we also need to change the field */
      upper = !upper;
   }

   /* interlaced frames must have even height */
   if(single_field)
   {
      if((iHeight % 2) != 0)
      {
         gan_err_flush_trace();
         gan_err_register ( "gan_read_targa_image_stream", GAN_ERROR_INCOMPATIBLE, "" );
         return NULL;
      }

      iInternalHeight = iHeight/2;
   }
   else
      iInternalHeight = iHeight;

   if(header != NULL)
   {
      header->file_format = GAN_TARGA_FORMAT;
      header->width = (unsigned int)iWidth;
      header->height = (unsigned int)(whole_image ? iHeight : iInternalHeight);
      header->format = eFormat;
      header->type = GAN_UINT8;
   }

   if(ictrlstr != NULL && ictrlstr->header_only)
      return (Gan_Image*)-1; /* special value */

   if ( image == NULL )
      image = gan_image_alloc ( eFormat, GAN_UINT8, whole_image ? iHeight : iInternalHeight, iWidth );
   else
      image = gan_image_set_format_type_dims ( image, eFormat, GAN_UINT8, whole_image ? iHeight : iInternalHeight, iWidth );

   if ( image == NULL )
   {
      gan_err_register ( "gan_read_targa_image_stream", GAN_ERROR_FAILURE, "setting up image" );
      return NULL;
   }
   
   /* if the image has zero size then we have finished */
   if ( iWidth == 0 || iHeight == 0 ) return image;

   switch(ui8DataType)
   {
      case TARGA_DATA_TYPE_RGB:
      {
         int iRow;

         if(single_field)
         {
            for(iRow=0; iRow<iHeight; iRow++)
            {
               /* only transfer even rows for upper field, or odd rows for upper field */
               if((upper && (iRow % 2) == 0) || (!upper && (iRow % 2) == 1))
               {
                  if(ui8PixelSize == 32)
                  {
                     if ( fread ( gan_image_get_pixptr_rgba_ui8(image, whole_image ? (flip?(iHeight-iRow-1):iRow) : (flip?(iInternalHeight-iRow/2-1):iRow/2), 0), sizeof(Gan_RGBAPixel_ui8), iWidth, infile ) != (size_t)iWidth )
                     {
                        gan_err_flush_trace();
                        gan_err_register ( "gan_read_targa_image_stream", GAN_ERROR_TRUNCATED_FILE, "truncated TARGA file" );
                        return NULL;
                     }

                     swap_rgba_to_bgra(gan_image_get_pixptr_rgba_ui8(image, whole_image ? (flip?(iHeight-iRow-1):iRow) : (flip?(iInternalHeight-iRow/2-1):iRow/2), 0), iWidth);
                  }
                  else /* ui8PixelSize == 24 */
                  {
                     if ( fread ( gan_image_get_pixptr_rgb_ui8(image, whole_image ? (flip?(iHeight-iRow-1):iRow) : (flip?(iInternalHeight-iRow/2-1):iRow/2), 0), sizeof(Gan_RGBPixel_ui8), iWidth, infile ) != (size_t)iWidth )
                     {
                        gan_err_flush_trace();
                        gan_err_register ( "gan_read_targa_image_stream", GAN_ERROR_TRUNCATED_FILE, "truncated TARGA file" );
                        return NULL;
                     }

                     swap_rgb_to_bgr(gan_image_get_pixptr_rgb_ui8(image, whole_image ? (flip?(iHeight-iRow-1):iRow) : (flip?(iInternalHeight-iRow/2-1):iRow/2), 0), iWidth);
                  }
               }
               else
               {
                  /* miss out this row */
                  if(ui8PixelSize == 32)
                  {
                     if(fseek(infile, iWidth*sizeof(Gan_RGBAPixel_ui8), SEEK_CUR) != 0)
                     {
                        gan_err_flush_trace();
                        gan_err_register ( "gan_read_targa_image_stream", GAN_ERROR_TRUNCATED_FILE, "truncated TARGA file" );
                        return NULL;
                     }
                  }
                  else /* ui8PixelSize == 24 */
                  {
                     if(fseek(infile, iWidth*sizeof(Gan_RGBPixel_ui8), SEEK_CUR) != 0)
                     {
                        gan_err_flush_trace();
                        gan_err_register ( "gan_read_targa_image_stream", GAN_ERROR_TRUNCATED_FILE, "truncated TARGA file" );
                        return NULL;
                     }
                  }
               }
            }
         }
         else
         {
            if(ui8PixelSize == 32)
            {
               for(iRow=0; iRow<iHeight; iRow++)
               {
                  if ( fread ( image->row_data.rgba.uc[flip ? (iHeight-iRow-1) : iRow], sizeof(Gan_RGBAPixel_ui8), iWidth, infile ) != (size_t)iWidth )
                  {
                     gan_err_flush_trace();
                     gan_err_register ( "gan_read_targa_image_stream", GAN_ERROR_TRUNCATED_FILE, "truncated TARGA file" );
                     return NULL;
                  }

                  swap_rgba_to_bgra(gan_image_get_pixptr_rgba_ui8(image, flip ? (iHeight-iRow-1) : iRow, 0), iWidth);
               }
            }
            else /* ui8PixelSize == 24 */
            {
               for(iRow=0; iRow<iHeight; iRow++)
               {
                  if ( fread ( image->row_data.rgb.uc[flip ? (iHeight-iRow-1) : iRow], sizeof(Gan_RGBPixel_ui8), iWidth, infile ) != (size_t)iWidth )
                  {
                     gan_err_flush_trace();
                     gan_err_register ( "gan_read_targa_image_stream", GAN_ERROR_TRUNCATED_FILE, "truncated TARGA file" );
                     return NULL;
                  }

                  swap_rgb_to_bgr(gan_image_get_pixptr_rgb_ui8(image, flip ? (iHeight-iRow-1) : iRow, 0), iWidth);
               }
            }
         }
      }
      break;

      case TARGA_DATA_TYPE_RLE_RGB:
      {
         int iPixelPos, iNumberOfPixels = iWidth*iHeight, iRow, iCol;
         gan_uint8 ui8PacketHeader;
         int iPacketSize, iPacketPos;

         switch(ui8PixelSize)
         {
            case 24:
            {
               Gan_RGBPixel_ui8* argbPix, rgbPixTmp;

               argbPix = gan_malloc_array(Gan_RGBPixel_ui8, 127);
               if(argbPix == NULL)
               {
                  gan_err_flush_trace();
                  gan_err_register_with_number ( "gan_read_targa_image_stream", GAN_ERROR_MALLOC_FAILED, "", 127*sizeof(Gan_RGBPixel_ui8) );
                  return NULL;
               }

               iPixelPos=0;
               while(iPixelPos<iNumberOfPixels)
               {
                  /* read next packet header */
                  if(fread(&ui8PacketHeader, 1, 1, infile) != 1)
                  {
                     gan_err_flush_trace();
                     gan_err_register ( "gan_read_targa_image_stream", GAN_ERROR_TRUNCATED_FILE, "truncated TARGA file" );
                     return NULL;
                  }

                  /* read next packet */
                  iPacketSize = 1+(ui8PacketHeader & 0x7f);
                  if(ui8PacketHeader & 0x80)
                  {
                     /* run-length encoded packet */
                     if(fread(argbPix, 3, 1, infile) != 1)
                     {
                        gan_err_flush_trace();
                        gan_err_register ( "gan_read_targa_image_stream", GAN_ERROR_TRUNCATED_FILE, "truncated TARGA file" );
                        return NULL;
                     }

                     /* swap R & B */
                     rgbPixTmp.R = argbPix[0].B;
                     rgbPixTmp.G = argbPix[0].G;
                     rgbPixTmp.B = argbPix[0].R;

                     for(iPacketPos=0; iPacketPos<iPacketSize; iPacketPos++, iPixelPos++)
                     {
                        /* get current position in image */
                        iRow = iPixelPos/iWidth;
                        iCol = iPixelPos%iWidth;

                        if(single_field)
                        {
                           if((upper && (iRow % 2) == 0) || (!upper && (iRow % 2) == 1))
                              gan_image_set_pix_rgb_ui8(image, whole_image ? (flip?(iHeight-iRow-1):iRow) : (flip?(iInternalHeight-iRow/2-1):iRow/2), iCol, &rgbPixTmp);
                        }
                        else
                           gan_image_set_pix_rgb_ui8(image, flip ? (iHeight-iRow-1) : iRow, iCol, &rgbPixTmp);
                     }
                  }
                  else
                  {
                     /* raw packet */
                     if(fread(argbPix, 3, iPacketSize, infile) != iPacketSize)
                     {
                        gan_err_flush_trace();
                        gan_err_register ( "gan_read_targa_image_stream", GAN_ERROR_TRUNCATED_FILE, "truncated TARGA file" );
                        return NULL;
                     }

                     for(iPacketPos=0; iPacketPos<iPacketSize; iPacketPos++, iPixelPos++)
                     {
                        /* get current position in image */
                        iRow = iPixelPos/iWidth;
                        iCol = iPixelPos%iWidth;

                        /* swap R & B */
                        rgbPixTmp.R = argbPix[iPacketPos].B;
                        rgbPixTmp.G = argbPix[iPacketPos].G;
                        rgbPixTmp.B = argbPix[iPacketPos].R;
                        
                        if(single_field)
                        {
                           if((upper && (iRow % 2) == 0) || (!upper && (iRow % 2) == 1))
                              gan_image_set_pix_rgb_ui8(image, whole_image ? (flip?(iHeight-iRow-1):iRow) : (flip?(iInternalHeight-iRow/2-1):iRow/2), iCol, &rgbPixTmp);
                        }
                        else
                           gan_image_set_pix_rgb_ui8(image, flip ? (iHeight-iRow-1) : iRow, iCol, &rgbPixTmp);
                     }
                  }
               }
               
               free(argbPix);
            }
            break;

            case 32:
            {
               Gan_RGBAPixel_ui8* argbaPix, rgbaPixTmp;


               argbaPix = gan_malloc_array(Gan_RGBAPixel_ui8, 128);
               if(argbaPix == NULL)
               {
                  gan_err_flush_trace();
                  gan_err_register_with_number ( "gan_read_targa_image_stream", GAN_ERROR_MALLOC_FAILED, "", 128*sizeof(Gan_RGBAPixel_ui8) );
                  return NULL;
               }

               iPixelPos=0;
               while(iPixelPos<iNumberOfPixels)
               {
                  /* read next packet header */
                  if(fread(&ui8PacketHeader, 1, 1, infile) != 1)
                  {
                     gan_err_flush_trace();
                     gan_err_register ( "gan_read_targa_image_stream", GAN_ERROR_TRUNCATED_FILE, "truncated TARGA file" );
                     return NULL;
                  }

                  /* read next packet */
                  iPacketSize = 1+(ui8PacketHeader & 0x7f);
                  if(ui8PacketHeader & 0x80)
                  {
                     /* run-length encoded packet */
                     if(fread(argbaPix, 4, 1, infile) != 1)
                     {
                        gan_err_flush_trace();
                        gan_err_register ( "gan_read_targa_image_stream", GAN_ERROR_TRUNCATED_FILE, "truncated TARGA file" );
                        return NULL;
                     }

                     /* swap R & B */
                     rgbaPixTmp.R = argbaPix[0].B;
                     rgbaPixTmp.G = argbaPix[0].G;
                     rgbaPixTmp.B = argbaPix[0].R;
                     rgbaPixTmp.A = argbaPix[0].A;

                     for(iPacketPos=0; iPacketPos<iPacketSize; iPacketPos++, iPixelPos++)
                     {
                        /* get current position in image */
                        iRow = iPixelPos/iWidth;
                        iCol = iPixelPos%iWidth;

                        if(single_field)
                        {
                           if((upper && (iRow % 2) == 0) || (!upper && (iRow % 2) == 1))
                              gan_image_set_pix_rgba_ui8(image, whole_image ? (flip?(iHeight-iRow-1):iRow) : (flip?(iInternalHeight-iRow/2-1):iRow/2), iCol, &rgbaPixTmp);
                        }
                        else
                           gan_image_set_pix_rgba_ui8(image, flip ? (iHeight-iRow-1) : iRow, iCol, &rgbaPixTmp);
                     }
                  }
                  else
                  {
                     /* raw packet */
                     if(fread(argbaPix, 4, iPacketSize, infile) != iPacketSize)
                     {
                        gan_err_flush_trace();
                        gan_err_register ( "gan_read_targa_image_stream", GAN_ERROR_TRUNCATED_FILE, "truncated TARGA file" );
                        return NULL;
                     }

                     for(iPacketPos=0; iPacketPos<iPacketSize; iPacketPos++, iPixelPos++)
                     {
                        /* get current position in image */
                        iRow = iPixelPos/iWidth;
                        iCol = iPixelPos%iWidth;

                        /* swap R & B */
                        rgbaPixTmp.R = argbaPix[iPacketPos].B;
                        rgbaPixTmp.G = argbaPix[iPacketPos].G;
                        rgbaPixTmp.B = argbaPix[iPacketPos].R;
                        rgbaPixTmp.A = argbaPix[iPacketPos].A;
                        
                        if(single_field)
                        {
                           if((upper && (iRow % 2) == 0) || (!upper && (iRow % 2) == 1))
                              gan_image_set_pix_rgba_ui8(image, whole_image ? (flip?(iHeight-iRow-1):iRow) : (flip?(iInternalHeight-iRow/2-1):iRow/2), iCol, &rgbaPixTmp);
                        }
                        else
                           gan_image_set_pix_rgba_ui8(image, flip ? (iHeight-iRow-1) : iRow, iCol, &rgbaPixTmp);
                     }
                  }
               }
               
               free(argbaPix);
            }
            break;

            default:
              gan_err_flush_trace();
              gan_err_register ( "gan_read_targa_image_stream", GAN_ERROR_ILLEGAL_TYPE, "" );
              return NULL;
            }
         }
         
      break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_read_targa_image_stream", GAN_ERROR_ILLEGAL_TYPE, "" );
        return NULL;
   }

   /* free colourmap if necessary */
   if(apxlColourmap != NULL)
      free(apxlColourmap);

   /* success */
   return image;
}

/**
 * \brief Reads a RGB colour image file in TARGA format.
 * \param filename The name of the image file
 * \param image The image structure to read the image data into or \c NULL
 * \param ictrlstr Pointer to structure controlling input or \c NULL
 * \param header Pointer to file header structure to be filled, or \c NULL
 * \return Pointer to image structure, or \c NULL on failure.
 *
 * Reads the TARGA image with the in the file \a filename into the given
 * \a image. If \a image is \c NULL a new image is dynamically allocated;
 * otherwise the already allocated \a image structure is reused.
 *
 * \sa gan_write_targa_image().
 */
Gan_Image *
 gan_read_targa_image ( const char *filename, Gan_Image *image, const struct Gan_ImageReadControlStruct *ictrlstr, struct Gan_ImageHeaderStruct *header )
{
   FILE *infile;
   Gan_Image *result;

   /* attempt to open file */
   infile = fopen ( filename, "rb" );
   if ( infile == NULL )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_read_targa_image", GAN_ERROR_OPENING_FILE, filename );
      return NULL;
   }

   result = gan_read_targa_image_stream ( infile, image, ictrlstr, header );
   fclose(infile);
   return result;
}

/**
 * \brief Initialises the output control structure for Targa files
 *
 * This function should be called on the structure to set the parameters to default values.
 * Then set any non-default values yourself before calling gan_write_targa_image() or
 * gan_write_targa_image_stream().
 */
void gan_initialise_targa_header_struct(Gan_TargaHeaderStruct *octrlstr)
{
}

/**
 * \brief Writes a RGB colour image to a file stream in TARGA format.
 * \param outfile The file stream
 * \param image The image structure to write to the file
 * \param new_file Whether the file does not currently exist
 * \param octrlstr Pointer to structure controlling output or \c NULL
 * \return #GAN_TRUE on successful write operation, or #GAN_FALSE on failure.
 *
 * Writes the \a image into the file stream \a outfile in TARGA format.
 *
 * If single-field mode is specified in the control structure, and the file already
 * existed the file stream must have been opened read-write.
 *
 * \sa gan_read_targa_image_stream().
 */
Gan_Bool
 gan_write_targa_image_stream ( FILE *outfile, const Gan_Image *image, Gan_Bool new_file, const struct Gan_ImageWriteControlStruct *octrlstr )
{
   Gan_Bool flip=GAN_FALSE, single_field=GAN_FALSE, upper=GAN_FALSE, whole_image=GAN_FALSE;
   unsigned int height=image->height;
   char* achBlankRow=NULL;

   /* read generic control parameters */
   if(octrlstr != NULL)
   {
      flip         = octrlstr->flip;
      single_field = octrlstr->single_field;
      upper        = octrlstr->upper;
      whole_image  = octrlstr->whole_image;
   }

   if(single_field && !whole_image)
      height *= 2;

   if ( image->format != GAN_RGB_COLOUR_IMAGE && image->format != GAN_RGB_COLOUR_ALPHA_IMAGE )
   {
      if(outfile != NULL)
      {
         gan_err_flush_trace();
         gan_err_register("gan_write_targa_image_stream", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
      }

      return GAN_FALSE;
   }

   if ( image->type != GAN_UINT8 )
   {
      if(outfile != NULL)
      {
         gan_err_flush_trace();
         gan_err_register("gan_write_targa_image_stream", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
      }

      return GAN_FALSE;
   }

   /* write header */
   if(outfile != NULL)
   {
      char acHeader[BIG_BUFFER_SIZE], *acAlignedHeader;

      /* align the header array */
      acAlignedHeader = (char*)((unsigned long int)acHeader + 7 - (((unsigned long int)acHeader + 7) % 8));

      acAlignedHeader[TARGA_ID_LENGTH_OFFSET]      = 0; /* no user comment */
      acAlignedHeader[TARGA_COLOURMAP_TYPE_OFFSET] = 0; /* no colourmap */
      *((gan_uint8*)&acAlignedHeader[TARGA_DATA_TYPE_OFFSET]) = TARGA_DATA_TYPE_RGB; /* unmapped RGB/RGBA */

      /* colourmap entries */
      acAlignedHeader[TARGA_COLOURMAP_ORIGIN_OFFSET]    = 0;
      acAlignedHeader[TARGA_COLOURMAP_ORIGIN_OFFSET+1]  = 0;
      acAlignedHeader[TARGA_COLOURMAP_LENGTH_OFFSET]    = 0;
      acAlignedHeader[TARGA_COLOURMAP_LENGTH_OFFSET+1]  = 0;
      acAlignedHeader[TARGA_COLOURMAP_ENTRYSIZE_OFFSET] = 0;

      acAlignedHeader[TARGA_XORIGIN_OFFSET]   = 0;
      acAlignedHeader[TARGA_XORIGIN_OFFSET+1] = 0;
      acAlignedHeader[TARGA_YORIGIN_OFFSET]   = 0;
      acAlignedHeader[TARGA_YORIGIN_OFFSET+1] = 0;
      *((gan_uint8*)&acAlignedHeader[TARGA_WIDTH_OFFSET+0]) = (gan_uint8)(image->width % 0x100);
      *((gan_uint8*)&acAlignedHeader[TARGA_WIDTH_OFFSET+1]) = (gan_uint8)(image->width / 0x100);
      *((gan_uint8*)&acAlignedHeader[TARGA_HEIGHT_OFFSET+0]) = height % 0x100;
      *((gan_uint8*)&acAlignedHeader[TARGA_HEIGHT_OFFSET+1]) = height / 0x100;
      *((gan_uint8*)&acAlignedHeader[TARGA_PIXEL_SIZE_OFFSET]) = ((image->format == GAN_RGB_COLOUR_IMAGE) ? 24 : 32);
      *((gan_uint8*)&acAlignedHeader[TARGA_IMAGE_DESCRIPTOR_OFFSET]) = (((image->format == GAN_RGB_COLOUR_IMAGE) ? 0 : 8)
                                                                        + (flip ? 32 : 0)
                                                                        + ((octrlstr != NULL && octrlstr->info.interlaced) ? 64 : 0));
      fwrite ( acAlignedHeader, 1, 18, outfile );
   }

   /* if the image has zero size then we have finished */
   if ( image->width == 0 || image->height == 0 ) return GAN_TRUE;

   if(single_field && new_file)
   {
      /* create blank row */
      achBlankRow = malloc(image->width*gan_image_pixel_size(image->format, image->type));
      if(achBlankRow == NULL)
      {
         gan_err_flush_trace();
         gan_err_register_with_number ( "gan_write_targa_image_stream", GAN_ERROR_MALLOC_FAILED, "",
                                        image->width*gan_image_pixel_size(image->format, image->type) );
         return GAN_FALSE;
      }

      memset(achBlankRow, 0, image->width*gan_image_pixel_size(image->format, image->type));
   }

   if(outfile != NULL)
   {
      /* write image data to stream */
      if ( !flip && !single_field && image->stride == image->width*gan_image_pixel_size(image->format, image->type) )
      {
         /* we can write it in one go */
         if(image->format == GAN_RGB_COLOUR_IMAGE)
         {
            swap_rgb_to_bgr(image->row_data.rgb.uc[0], image->height*image->width);
            fwrite ( image->row_data.rgb.uc[0], sizeof(Gan_RGBPixel_ui8), image->height*image->width, outfile );
            swap_rgb_to_bgr(image->row_data.rgb.uc[0], image->height*image->width);
         }
         else /* image->format == GAN_RGB_COLOUR_ALPHA_IMAGE */
         {
            swap_rgba_to_bgra(image->row_data.rgba.uc[0], image->height*image->width);
            fwrite ( image->row_data.rgba.uc[0], sizeof(Gan_RGBAPixel_ui8), image->height*image->width, outfile );
            swap_rgba_to_bgra(image->row_data.rgba.uc[0], image->height*image->width);
         }
      }
      else
      {
         /* write one line of image data at a time */
         unsigned long r;

         for ( r=0; r < height; r++ )
         {
            /* ignore row if it's in a field we're not writing */
            if(single_field && ((upper && ((r % 2) == 1)) || (!upper && ((r % 2) == 0))))
            {
               if(new_file)
                  /* write a blank row */
                  fwrite(achBlankRow, 1, image->width*gan_image_pixel_size(image->format, image->type), outfile);
               else
                  /* leave this row as is was */
                  fseek(outfile, image->width*gan_image_pixel_size(image->format, image->type), SEEK_CUR);
            }
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

               if(image->format == GAN_RGB_COLOUR_IMAGE)
               {
                  swap_rgb_to_bgr(image->row_data.rgb.uc[rp], image->width);
                  fwrite ( image->row_data.rgb.uc[rp], sizeof(Gan_RGBPixel_ui8), image->width, outfile );
                  swap_rgb_to_bgr(image->row_data.rgb.uc[rp], image->width);
               }
               else /* image->format == GAN_RGB_COLOUR_ALPHA_IMAGE */
               {
                  swap_rgba_to_bgra(image->row_data.rgba.uc[rp], image->width);
                  fwrite ( image->row_data.rgba.uc[rp], sizeof(Gan_RGBAPixel_ui8), image->width, outfile );
                  swap_rgba_to_bgra(image->row_data.rgba.uc[rp], image->width);
               }
            }
         }
      }
   }

   if(achBlankRow != NULL)
      free(achBlankRow);

   /* success */
   return GAN_TRUE;
}

/**
 * \brief Writes a RGB colour image file in TARGA format.
 * \param filename The name of the image file
 * \param image The image structure to write to the file
 * \param octrlstr Pointer to structure controlling output or \c NULL
 * \return #GAN_TRUE on successful write operation, #GAN_FALSE on failure.
 *
 * Writes the \a image into TARGA file \a filename.
 *
 * \sa gan_read_targa_image().
 */
Gan_Bool
 gan_write_targa_image ( const char *filename, const Gan_Image *image, const struct Gan_ImageWriteControlStruct *octrlstr )
{
   if(filename == NULL)
      return gan_write_targa_image_stream ( NULL, image, GAN_FALSE, octrlstr );
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
            unsigned int uiFileSize;

            if(octrlstr->whole_image)
               uiFileSize = 18+image->height*image->width*gan_image_pixel_size(image->format, image->type);
            else
               uiFileSize = 18+2*image->height*image->width*gan_image_pixel_size(image->format, image->type);

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
         gan_err_register ( "gan_write_targa_image", GAN_ERROR_OPENING_FILE, filename );
         return GAN_FALSE;
      }

      result = gan_write_targa_image_stream ( outfile, image, new_file, octrlstr );
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

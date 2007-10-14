/**
 * File:          $RCSfile: sgi_io.c,v $
 * Module:        SGI format image file I/O functions
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
#include <gandalf/image/io/sgi_io.h>
#include <gandalf/image/io/image_io.h>
#include <gandalf/image/image_gl_uint8.h>
#include <gandalf/image/image_gl_uint16.h>
#include <gandalf/image/image_rgb_uint8.h>
#include <gandalf/image/image_rgb_uint16.h>
#include <gandalf/image/image_rgba_uint8.h>
#include <gandalf/image/image_rgba_uint16.h>
#include <gandalf/common/misc_error.h>

/**
 * \addtogroup ImagePackage
 * \{
 */

/**
 * \addtogroup ImageIO
 * \{
 */

#define SGI_MAGIC_NUMBER_OFFSET               0
#define SGI_STORAGE_OFFSET                    2
#define SGI_BPC_OFFSET                        3
#define SGI_DIMENSION_OFFSET                  4
#define SGI_XSIZE_OFFSET                      6
#define SGI_YSIZE_OFFSET                      8
#define SGI_ZSIZE_OFFSET                     10
#define SGI_PIXMIN_OFFSET                    12
#define SGI_PIXMAX_OFFSET                    16
#define SGI_COLOURMAP_OFFSET                104

/**
 * \brief Determines whether an image is a SGI image using the magic number
 */
Gan_Bool gan_image_is_sgi(const unsigned char *magic_string, size_t length)
{
   if (length < 4)
      return(GAN_FALSE);

   if ( (*((gan_uint16*)&magic_string[SGI_MAGIC_NUMBER_OFFSET]) == 474
         || *((gan_uint16*)&magic_string[SGI_MAGIC_NUMBER_OFFSET]) == 55809))
      return(GAN_TRUE);

   return(GAN_FALSE);
}

#define BIG_BUFFER_SIZE 530

static Gan_Bool
 convert_scanline(char* achScanline, Gan_Image* image, int iPlane, int iRow, Gan_Bool bReversedEndianness)
{
   int iCol;

   switch(image->format)
   {
      case GAN_GREY_LEVEL_IMAGE:
        if(iPlane != 0)
        {
           gan_err_flush_trace();
           gan_err_register("convert_scanline", GAN_ERROR_ILLEGAL_TYPE, "");
           return GAN_FALSE;
        }           

        switch(image->type)
        {
           case GAN_UINT8:
           {
              gan_uint8* aui8Scanline = (gan_uint8 *)achScanline, *aui8Imptr = gan_image_get_pixptr_gl_ui8(image, iRow, 0);

              for(iCol = (int)image->width-1; iCol>=0; iCol--)
                 *aui8Imptr++ = *aui8Scanline++;
           }
           break;

           case GAN_UINT16:
           {
              gan_uint16* aui16Scanline = (gan_uint16 *)achScanline, *aui16Imptr = gan_image_get_pixptr_gl_ui16(image, iRow, 0);

              if(bReversedEndianness)
              {
                 for(iCol = (int)image->width-1; iCol>=0; iCol--, aui16Imptr++, aui16Scanline++)
                 {
                    vReverseEndianness16(aui16Scanline);
                    *aui16Imptr = *aui16Scanline;
                 }
              }
              else
                 for(iCol = (int)image->width-1; iCol>=0; iCol--)
                    *aui16Imptr++ = *aui16Scanline++;
           }
           break;

           default:
             gan_err_flush_trace();
             gan_err_register("convert_scanline", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "");
             return GAN_FALSE;
        }
        break;

      case GAN_RGB_COLOUR_IMAGE:
        if(iPlane < 0 || iPlane > 2)
        {
           gan_err_flush_trace();
           gan_err_register("convert_scanline", GAN_ERROR_ILLEGAL_TYPE, "");
           return GAN_FALSE;
        }           

        switch(image->type)
        {
           case GAN_UINT8:
           {
              gan_uint8* aui8Scanline = (gan_uint8 *)achScanline;
              Gan_RGBPixel_ui8 *argbui8Imptr = gan_image_get_pixptr_rgb_ui8(image, iRow, 0);

              for(iCol = (int)image->width-1; iCol>=0; iCol--, argbui8Imptr++, aui8Scanline++)
                 ((gan_uint8*)argbui8Imptr)[iPlane] = *aui8Scanline;
           }
           break;

           case GAN_UINT16:
           {
              gan_uint16* aui16Scanline = (gan_uint16 *)achScanline;
              Gan_RGBPixel_ui16 *argbui16Imptr = gan_image_get_pixptr_rgb_ui16(image, iRow, 0);

              if(bReversedEndianness)
              {
                 for(iCol = (int)image->width-1; iCol>=0; iCol--, argbui16Imptr++, aui16Scanline++)
                 {
                    vReverseEndianness16(aui16Scanline);
                    ((gan_uint16*)argbui16Imptr)[iPlane] = *aui16Scanline;
                 }
              }
              else
                 for(iCol = (int)image->width-1; iCol>=0; iCol--, argbui16Imptr++, aui16Scanline++)
                    ((gan_uint16*)argbui16Imptr)[iPlane] = *aui16Scanline;
           }
           break;

           default:
             gan_err_flush_trace();
             gan_err_register("convert_scanline", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "");
             return GAN_FALSE;
        }
        break;

      case GAN_RGB_COLOUR_ALPHA_IMAGE:
        if(iPlane < 0 || iPlane > 3)
        {
           gan_err_flush_trace();
           gan_err_register("convert_scanline", GAN_ERROR_ILLEGAL_TYPE, "");
           return GAN_FALSE;
        }           

        switch(image->type)
        {
           case GAN_UINT8:
           {
              gan_uint8* aui8Scanline = (gan_uint8 *)achScanline;
              Gan_RGBAPixel_ui8 *argbaui8Imptr = gan_image_get_pixptr_rgba_ui8(image, iRow, 0);

              for(iCol = (int)image->width-1; iCol>=0; iCol--, argbaui8Imptr++, aui8Scanline++)
                 ((gan_uint8*)argbaui8Imptr)[iPlane] = *aui8Scanline;
           }
           break;

           case GAN_UINT16:
           {
              gan_uint16* aui16Scanline = (gan_uint16 *)achScanline;
              Gan_RGBAPixel_ui16 *argbaui16Imptr = gan_image_get_pixptr_rgba_ui16(image, iRow, 0);

              if(bReversedEndianness)
              {
                 for(iCol = (int)image->width-1; iCol>=0; iCol--, argbaui16Imptr++, aui16Scanline++)
                 {
                    vReverseEndianness16(aui16Scanline);
                    ((gan_uint16*)argbaui16Imptr)[iPlane] = *aui16Scanline;
                 }
              }
              else
                 for(iCol = (int)image->width-1; iCol>=0; iCol--, argbaui16Imptr++, aui16Scanline++)
                    ((gan_uint16*)argbaui16Imptr)[iPlane] = *aui16Scanline;
           }
           break;

           default:
             gan_err_flush_trace();
             gan_err_register("convert_scanline", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "");
             return GAN_FALSE;
        }
        break;

      default:
        gan_err_flush_trace();
        gan_err_register("convert_scanline", GAN_ERROR_ILLEGAL_TYPE, "");
        return GAN_FALSE;
   }

   return GAN_TRUE;
}

static Gan_Bool
 convert_to_scanline(const Gan_Image *image, int iRow, int iPlane, char *achScanline, Gan_Bool bReversedEndianness)
{
   int iCol;

   switch(image->format)
   {
      case GAN_GREY_LEVEL_IMAGE:
        if(iPlane != 0)
        {
           gan_err_flush_trace();
           gan_err_register("convert_to_scanline", GAN_ERROR_ILLEGAL_TYPE, "");
           return GAN_FALSE;
        }           

        switch(image->type)
        {
           case GAN_UINT8:
           {
              gan_uint8* aui8Scanline = (gan_uint8 *)achScanline, *aui8Imptr = gan_image_get_pixptr_gl_ui8(image, iRow, 0);

              for(iCol = (int)image->width-1; iCol>=0; iCol--)
                 *aui8Scanline++ = *aui8Imptr++;
           }
           break;

           case GAN_UINT16:
           {
              gan_uint16* aui16Scanline = (gan_uint16 *)achScanline, *aui16Imptr = gan_image_get_pixptr_gl_ui16(image, iRow, 0);

              if(bReversedEndianness)
              {
                 for(iCol = (int)image->width-1; iCol>=0; iCol--, aui16Scanline++, aui16Imptr++)
                 {
                    *aui16Scanline = *aui16Imptr;
                    vReverseEndianness16(aui16Scanline);
                 }
              }
              else
                 for(iCol = (int)image->width-1; iCol>=0; iCol--)
                    *aui16Scanline++ = *aui16Imptr++;
           }
           break;

           default:
             gan_err_flush_trace();
             gan_err_register("convert_to_scanline", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "");
             return GAN_FALSE;
        }
        break;

      case GAN_RGB_COLOUR_IMAGE:
        if(iPlane < 0 || iPlane > 2)
        {
           gan_err_flush_trace();
           gan_err_register("convert_to_scanline", GAN_ERROR_ILLEGAL_TYPE, "");
           return GAN_FALSE;
        }           

        switch(image->type)
        {
           case GAN_UINT8:
           {
              gan_uint8* aui8Scanline = (gan_uint8 *)achScanline;
              Gan_RGBPixel_ui8 *argbui8Imptr = gan_image_get_pixptr_rgb_ui8(image, iRow, 0);

              for(iCol = (int)image->width-1; iCol>=0; iCol--, argbui8Imptr++, aui8Scanline++)
                 *aui8Scanline = ((gan_uint8*)argbui8Imptr)[iPlane];
           }
           break;

           case GAN_UINT16:
           {
              gan_uint16* aui16Scanline = (gan_uint16 *)achScanline;
              Gan_RGBPixel_ui16 *argbui16Imptr = gan_image_get_pixptr_rgb_ui16(image, iRow, 0);

              if(bReversedEndianness)
              {
                 for(iCol = (int)image->width-1; iCol>=0; iCol--, argbui16Imptr++, aui16Scanline++)
                 {
                    *aui16Scanline = ((gan_uint16*)argbui16Imptr)[iPlane];
                    vReverseEndianness16(aui16Scanline);
                 }
              }
              else
                 for(iCol = (int)image->width-1; iCol>=0; iCol--, argbui16Imptr++, aui16Scanline++)
                    *aui16Scanline = ((gan_uint16*)argbui16Imptr)[iPlane];
           }
           break;

           default:
             gan_err_flush_trace();
             gan_err_register("convert_to_scanline", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "");
             return GAN_FALSE;
        }
        break;

      case GAN_RGB_COLOUR_ALPHA_IMAGE:
        if(iPlane < 0 || iPlane > 3)
        {
           gan_err_flush_trace();
           gan_err_register("convert_to_scanline", GAN_ERROR_ILLEGAL_TYPE, "");
           return GAN_FALSE;
        }           

        switch(image->type)
        {
           case GAN_UINT8:
           {
              gan_uint8* aui8Scanline = (gan_uint8 *)achScanline;
              Gan_RGBAPixel_ui8 *argbaui8Imptr = gan_image_get_pixptr_rgba_ui8(image, iRow, 0);

              for(iCol = (int)image->width-1; iCol>=0; iCol--, argbaui8Imptr++, aui8Scanline++)
                 *aui8Scanline = ((gan_uint8*)argbaui8Imptr)[iPlane];
           }
           break;

           case GAN_UINT16:
           {
              gan_uint16* aui16Scanline = (gan_uint16 *)achScanline;
              Gan_RGBAPixel_ui16 *argbaui16Imptr = gan_image_get_pixptr_rgba_ui16(image, iRow, 0);

              if(bReversedEndianness)
              {
                 for(iCol = (int)image->width-1; iCol>=0; iCol--, argbaui16Imptr++, aui16Scanline++)
                 {
                    *aui16Scanline = ((gan_uint16*)argbaui16Imptr)[iPlane];
                    vReverseEndianness16(aui16Scanline);
                 }
              }
              else
                 for(iCol = (int)image->width-1; iCol>=0; iCol--, argbaui16Imptr++, aui16Scanline++)
                    *aui16Scanline = ((gan_uint16*)argbaui16Imptr)[iPlane];
           }
           break;

           default:
             gan_err_flush_trace();
             gan_err_register("convert_to_scanline", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "");
             return GAN_FALSE;
        }
        break;

      default:
        gan_err_flush_trace();
        gan_err_register("convert_to_scanline", GAN_ERROR_ILLEGAL_TYPE, "");
        return GAN_FALSE;
   }

   return GAN_TRUE;
}

static Gan_Bool
 bExpandRLEData8Bit(gan_uint8* aui8RLEBuffer, unsigned int uiRow, int iPlane, Gan_Image* pgiImage, Gan_Bool bFlip, Gan_Bool bSingleField, Gan_Bool bUpper, Gan_Bool bWholeImage)
{
   gan_uint8 ui8Pixel, ui8Count;

   gan_err_test_bool(pgiImage->type == GAN_UINT8, "bExpandRLEData8Bit", GAN_ERROR_INCOMPATIBLE, "");

   switch(pgiImage->format)
   {
      case GAN_GREY_LEVEL_IMAGE:
      {
         gan_uint8 *aui8Imptr = NULL;

         if(bSingleField)
         {
            if(bFlip)
            {
               if((bUpper && (uiRow % 2) == 1) || (!bUpper && (uiRow % 2) == 0))
                  aui8Imptr = gan_image_get_pixptr_gl_ui8(pgiImage, bWholeImage ? (pgiImage->height-uiRow-1) : (pgiImage->height-uiRow/2-1), 0);
            }
            else
            {
               if((bUpper && (uiRow % 2) == 0) || (!bUpper && (uiRow % 2) == 1))
                  aui8Imptr = gan_image_get_pixptr_gl_ui8(pgiImage, bWholeImage ? uiRow : (uiRow/2), 0);
            }
         }
         else
            aui8Imptr = gan_image_get_pixptr_gl_ui8(pgiImage, bFlip ? (pgiImage->height-uiRow-1) : uiRow, 0);

         if(iPlane != 0)
         {
            gan_err_flush_trace();
            gan_err_register("bExpandRLEData8Bit", GAN_ERROR_ILLEGAL_TYPE, "");
            return GAN_FALSE;
         }           

         if(aui8Imptr == NULL)
            for(;;)
            {
               ui8Pixel = *aui8RLEBuffer++;
               if(!(ui8Count = (ui8Pixel & 0x7f)))
                  break;

               if(ui8Pixel & 0x80)
               {
                  while(ui8Count--)
                     aui8RLEBuffer++;
               }
            }
         else
            for(;;)
            {
               ui8Pixel = *aui8RLEBuffer++;
               if(!(ui8Count = (ui8Pixel & 0x7f)))
                  break;

               if(ui8Pixel & 0x80)
               {
                  while(ui8Count--)
                     *aui8Imptr++ = *aui8RLEBuffer++;
               }
               else
               {
                  ui8Pixel = *aui8RLEBuffer++;
                  while(ui8Count--)
                     *aui8Imptr++ = ui8Pixel;
               }
            }
      }
      break;

      case GAN_RGB_COLOUR_IMAGE:
      {
         Gan_RGBPixel_ui8 *argbui8Imptr = NULL;

         if(bSingleField)
         {
            if(bFlip)
            {
               if((bUpper && (uiRow % 2) == 1) || (!bUpper && (uiRow % 2) == 0))
                  argbui8Imptr = gan_image_get_pixptr_rgb_ui8(pgiImage, bWholeImage ? (pgiImage->height-uiRow-1) : (pgiImage->height-uiRow/2-1), 0);
            }
            else
            {
               if((bUpper && (uiRow % 2) == 0) || (!bUpper && (uiRow % 2) == 1))
                  argbui8Imptr = gan_image_get_pixptr_rgb_ui8(pgiImage, bWholeImage ? uiRow : (uiRow/2), 0);
            }
         }
         else
            argbui8Imptr = gan_image_get_pixptr_rgb_ui8(pgiImage, bFlip ? (pgiImage->height-uiRow-1) : uiRow, 0);

         if(iPlane < 0 || iPlane > 2)
         {
            gan_err_flush_trace();
            gan_err_register("bExpandRLEData8Bit", GAN_ERROR_ILLEGAL_TYPE, "");
            return GAN_FALSE;
         }           

         if(argbui8Imptr == NULL)
            for(;;)
            {
               ui8Pixel = *aui8RLEBuffer++;
               if(!(ui8Count = (ui8Pixel & 0x7f)))
                  break;

               if(ui8Pixel & 0x80)
               {
                  while(ui8Count--)
                     aui8RLEBuffer++;
               }
            }
         else
            for(;;)
            {
               ui8Pixel = *aui8RLEBuffer++;
               if(!(ui8Count = (ui8Pixel & 0x7f)))
                  break;

               if(ui8Pixel & 0x80)
               {
                  while(ui8Count--)
                  {
                     ((gan_uint8*)argbui8Imptr)[iPlane] = *aui8RLEBuffer++;
                     argbui8Imptr++;
                  }
               }
               else
               {
                  ui8Pixel = *aui8RLEBuffer++;
                  while(ui8Count--)
                  {
                     ((gan_uint8*)argbui8Imptr)[iPlane] = ui8Pixel;
                     argbui8Imptr++;
                  }
               }
            }
      }
      break;

      case GAN_RGB_COLOUR_ALPHA_IMAGE:
      {
         Gan_RGBAPixel_ui8 *argbaui8Imptr = NULL;

         if(bSingleField)
         {
            if(bFlip)
            {
               if((bUpper && (uiRow % 2) == 1) || (!bUpper && (uiRow % 2) == 0))
                  argbaui8Imptr = gan_image_get_pixptr_rgba_ui8(pgiImage, bWholeImage ? (pgiImage->height-uiRow-1) : (pgiImage->height-uiRow/2-1), 0);
            }
            else
            {
               if((bUpper && (uiRow % 2) == 0) || (!bUpper && (uiRow % 2) == 1))
                  argbaui8Imptr = gan_image_get_pixptr_rgba_ui8(pgiImage, bWholeImage ? uiRow : (uiRow/2), 0);
            }
         }
         else
            argbaui8Imptr = gan_image_get_pixptr_rgba_ui8(pgiImage, bFlip ? (pgiImage->height-uiRow-1) : uiRow, 0);

         if(iPlane < 0 || iPlane > 3)
         {
            gan_err_flush_trace();
            gan_err_register("bExpandRLEData8Bit", GAN_ERROR_ILLEGAL_TYPE, "");
            return GAN_FALSE;
         }           

         if(argbaui8Imptr == NULL)
            for(;;)
            {
               ui8Pixel = *aui8RLEBuffer++;
               if(!(ui8Count = (ui8Pixel & 0x7f)))
                  break;

               if(ui8Pixel & 0x80)
               {
                  while(ui8Count--)
                     aui8RLEBuffer++;
               }
            }
         else
            for(;;)
            {
               ui8Pixel = *aui8RLEBuffer++;
               if(!(ui8Count = (ui8Pixel & 0x7f)))
                  break;

               if(ui8Pixel & 0x80)
               {
                  while(ui8Count--)
                  {
                     ((gan_uint8*)argbaui8Imptr)[iPlane] = *aui8RLEBuffer++;
                     argbaui8Imptr++;
                  }
               }
               else
               {
                  ui8Pixel = *aui8RLEBuffer++;
                  while(ui8Count--)
                  {
                     ((gan_uint8*)argbaui8Imptr)[iPlane] = ui8Pixel;
                     argbaui8Imptr++;
                  }
               }
            }
      }
      break;

      default:
        gan_err_flush_trace();
        gan_err_register("bExpandRLEData8Bit", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "");
        return GAN_FALSE;
   }

   return GAN_TRUE;
}

static Gan_Bool
 bExpandRLEData16Bit(gan_uint16* aui16RLEBuffer, unsigned int uiRow, int iPlane, Gan_Image* pgiImage, Gan_Bool bFlip, Gan_Bool bSingleField, Gan_Bool bUpper, Gan_Bool bWholeImage)
{
   gan_uint16 ui16Pixel, ui16Count;

   gan_err_test_bool(pgiImage->type == GAN_UINT16, "bExpandRLEData16Bit", GAN_ERROR_INCOMPATIBLE, "");

   switch(pgiImage->format)
   {
      case GAN_GREY_LEVEL_IMAGE:
      {
         gan_uint16 *aui16Imptr = NULL;

         if(bSingleField)
         {
            if(bFlip)
            {
               if((bUpper && (uiRow % 2) == 1) || (!bUpper && (uiRow % 2) == 0))
                  aui16Imptr = gan_image_get_pixptr_gl_ui16(pgiImage, bWholeImage ? (pgiImage->height-uiRow-1) : (pgiImage->height-uiRow/2-1), 0);
            }
            else
            {
               if((bUpper && (uiRow % 2) == 0) || (!bUpper && (uiRow % 2) == 1))
                  aui16Imptr = gan_image_get_pixptr_gl_ui16(pgiImage, bWholeImage ? uiRow : (uiRow/2), 0);
            }
         }
         else
            aui16Imptr = gan_image_get_pixptr_gl_ui16(pgiImage, bFlip ? (pgiImage->height-uiRow-1) : uiRow, 0);

         if(iPlane != 0)
         {
            gan_err_flush_trace();
            gan_err_register("bExpandRLEData16Bit", GAN_ERROR_ILLEGAL_TYPE, "");
            return GAN_FALSE;
         }           

         if(aui16Imptr == NULL)
            for(;;)
            {
               ui16Pixel = *aui16RLEBuffer++;
               if(!(ui16Count = (ui16Pixel & 0x7f)))
                  break;

               if(ui16Pixel & 0x160)
               {
                  while(ui16Count--)
                     aui16RLEBuffer++;
               }
            }
         else
            for(;;)
            {
               ui16Pixel = *aui16RLEBuffer++;
               if(!(ui16Count = (ui16Pixel & 0x7f)))
                  break;

               if(ui16Pixel & 0x80)
               {
                  while(ui16Count--)
                     *aui16Imptr++ = *aui16RLEBuffer++;
               }
               else
               {
                  ui16Pixel = *aui16RLEBuffer++;
                  while(ui16Count--)
                     *aui16Imptr++ = ui16Pixel;
               }
            }
      }
      break;

      case GAN_RGB_COLOUR_IMAGE:
      {
         Gan_RGBPixel_ui16 *argbui16Imptr = NULL;

         if(bSingleField)
         {
            if(bFlip)
            {
               if((bUpper && (uiRow % 2) == 1) || (!bUpper && (uiRow % 2) == 0))
                  argbui16Imptr = gan_image_get_pixptr_rgb_ui16(pgiImage, bWholeImage ? (pgiImage->height-uiRow-1) : (pgiImage->height-uiRow/2-1), 0);
            }
            else
            {
               if((bUpper && (uiRow % 2) == 0) || (!bUpper && (uiRow % 2) == 1))
                  argbui16Imptr = gan_image_get_pixptr_rgb_ui16(pgiImage, bWholeImage ? uiRow : (uiRow/2), 0);
            }
         }
         else
            argbui16Imptr = gan_image_get_pixptr_rgb_ui16(pgiImage, bFlip ? (pgiImage->height-uiRow-1) : uiRow, 0);

         if(iPlane < 0 || iPlane > 2)
         {
            gan_err_flush_trace();
            gan_err_register("bExpandRLEData16Bit", GAN_ERROR_ILLEGAL_TYPE, "");
            return GAN_FALSE;
         }           

         if(argbui16Imptr == NULL)
            for(;;)
            {
               ui16Pixel = *aui16RLEBuffer++;
               if(!(ui16Count = (ui16Pixel & 0x7f)))
                  break;

               if(ui16Pixel & 0x160)
               {
                  while(ui16Count--)
                     aui16RLEBuffer++;
               }
            }
         else
            for(;;)
            {
               ui16Pixel = *aui16RLEBuffer++;
               if(!(ui16Count = (ui16Pixel & 0x7f)))
                  break;

               if(ui16Pixel & 0x80)
               {
                  while(ui16Count--)
                  {
                     ((gan_uint16*)argbui16Imptr)[iPlane] = *aui16RLEBuffer++;
                     argbui16Imptr++;
                  }
               }
               else
               {
                  ui16Pixel = *aui16RLEBuffer++;
                  while(ui16Count--)
                  {
                     ((gan_uint16*)argbui16Imptr)[iPlane] = ui16Pixel;
                     argbui16Imptr++;
                  }
               }
            }
      }
      break;

      case GAN_RGB_COLOUR_ALPHA_IMAGE:
      {
         Gan_RGBAPixel_ui16 *argbaui16Imptr = NULL;

         if(bSingleField)
         {
            if(bFlip)
            {
               if((bUpper && (uiRow % 2) == 1) || (!bUpper && (uiRow % 2) == 0))
                  argbaui16Imptr = gan_image_get_pixptr_rgba_ui16(pgiImage, bWholeImage ? (pgiImage->height-uiRow-1) : (pgiImage->height-uiRow/2-1), 0);
            }
            else
            {
               if((bUpper && (uiRow % 2) == 0) || (!bUpper && (uiRow % 2) == 1))
                  argbaui16Imptr = gan_image_get_pixptr_rgba_ui16(pgiImage, bWholeImage ? uiRow : (uiRow/2), 0);
            }
         }
         else
            argbaui16Imptr = gan_image_get_pixptr_rgba_ui16(pgiImage, bFlip ? (pgiImage->height-uiRow-1) : uiRow, 0);

         if(iPlane < 0 || iPlane > 3)
         {
            gan_err_flush_trace();
            gan_err_register("bExpandRLEData16Bit", GAN_ERROR_ILLEGAL_TYPE, "");
            return GAN_FALSE;
         }           

         if(argbaui16Imptr == NULL)
            for(;;)
            {
               ui16Pixel = *aui16RLEBuffer++;
               if(!(ui16Count = (ui16Pixel & 0x7f)))
                  break;

               if(ui16Pixel & 0x160)
               {
                  while(ui16Count--)
                     aui16RLEBuffer++;
               }
            }
         else
            for(;;)
            {
               ui16Pixel = *aui16RLEBuffer++;
               if(!(ui16Count = (ui16Pixel & 0x7f)))
                  break;

               if(ui16Pixel & 0x80)
               {
                  while(ui16Count--)
                  {
                     ((gan_uint16*)argbaui16Imptr)[iPlane] = *aui16RLEBuffer++;
                     argbaui16Imptr++;
                  }
               }
               else
               {
                  ui16Pixel = *aui16RLEBuffer++;
                  while(ui16Count--)
                  {
                     ((gan_uint16*)argbaui16Imptr)[iPlane] = ui16Pixel;
                     argbaui16Imptr++;
                  }
               }
            }
      }
      break;

      default:
        gan_err_flush_trace();
        gan_err_register("bExpandRLEData16Bit", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "");
        return GAN_FALSE;
   }

   return GAN_TRUE;
}
   
/**
 * \brief Reads a RGB colour image file in SGI format from a stream.
 * \param infile The file stream to be read
 * \param image The image structure to read the image data into or \c NULL
 * \param ictrlstr Pointer to structure controlling input or \c NULL
 * \param header Pointer to file header structure to be filled, or \c NULL
 * \return Pointer to image structure, or \c NULL on failure.
 *
 * Reads the SGI image from the given file stream \a infile into the given
 * \a image. If \a image is \c NULL a new image is dynamically allocated,
 * otherwise the already allocated \a image structure is reused.
 *
 * \sa gan_write_sgi_image_stream().
 */
Gan_Image *
 gan_read_sgi_image_stream ( FILE *infile, Gan_Image *image, const struct Gan_ImageReadControlStruct *ictrlstr, struct Gan_ImageHeaderStruct *header )
{
   char acHeader[BIG_BUFFER_SIZE], *acAlignedHeader;
   int iWidth, iHeight, iInternalHeight;
   gan_uint16 ui16Magic, ui16ZSize, ui16Dimension, ui16Width, ui16Height;
   gan_uint8 ui8Storage, ui8BytesPerPixel;
   gan_uint32 ui32Colourmap;
   Gan_Bool bReversedEndianness=GAN_FALSE, flip=GAN_FALSE, single_field=GAN_FALSE, upper=GAN_FALSE, whole_image=GAN_FALSE;
   Gan_ImageFormat eFormat;
   Gan_Type eType;

   /* align the header array */
   acAlignedHeader = (char*)((unsigned long int)acHeader + 7 - (((unsigned long int)acHeader + 7) % 8));

   /* read the file header */
   if(fread(acAlignedHeader, 1, 512, infile) != 512)
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_read_sgi_image_stream", GAN_ERROR_CORRUPTED_FILE, "corrupted SGI file header (truncated file?)" );
      return NULL;
   }

   ui16Magic        = *((gan_uint16*)&acAlignedHeader[SGI_MAGIC_NUMBER_OFFSET]);
   ui8Storage       = *((gan_uint8*)&acAlignedHeader[SGI_STORAGE_OFFSET]);
   ui8BytesPerPixel = *((gan_uint8*)&acAlignedHeader[SGI_BPC_OFFSET]);
   ui16Dimension    = *((gan_uint16*)&acAlignedHeader[SGI_DIMENSION_OFFSET]);
   ui16Width        = *((gan_uint16*)&acAlignedHeader[SGI_XSIZE_OFFSET]);
   ui16Height       = *((gan_uint16*)&acAlignedHeader[SGI_YSIZE_OFFSET]);
   ui16ZSize        = *((gan_uint16*)&acAlignedHeader[SGI_ZSIZE_OFFSET]);
   ui32Colourmap    = *((gan_uint32*)&acAlignedHeader[SGI_COLOURMAP_OFFSET]);

   if(ui16Magic == 474)
      bReversedEndianness = GAN_FALSE;
   else if(ui16Magic == 55809)
      bReversedEndianness = GAN_TRUE;
   else
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_read_sgi_image_stream", GAN_ERROR_CORRUPTED_FILE, "corrupted SGI file header (truncated file?)" );
      return NULL;
   }

   if(bReversedEndianness)
   {
      vReverseEndianness16(&ui16Dimension);
      vReverseEndianness16(&ui16Width);
      vReverseEndianness16(&ui16Height);
      vReverseEndianness16(&ui16ZSize);
      vReverseEndianness32(&ui32Colourmap);
   }

   iWidth  = (int)ui16Width;
   iHeight = (int)ui16Height;

   /* determine image format */
   switch(ui16ZSize)
   {
      case 1:
        eFormat = GAN_GREY_LEVEL_IMAGE;
        if(ui16Dimension != 2)
        {
           gan_err_flush_trace();
           gan_err_register ( "gan_read_sgi_image_stream", GAN_ERROR_CORRUPTED_FILE, "corrupted SGI file header (truncated file?)" );
           return NULL;
        }

        break;
        
      case 3:
        eFormat = GAN_RGB_COLOUR_IMAGE;
        if(ui16Dimension != 3)
        {
           gan_err_flush_trace();
           gan_err_register ( "gan_read_sgi_image_stream", GAN_ERROR_CORRUPTED_FILE, "corrupted SGI file header (truncated file?)" );
           return NULL;
        }

        break;

      case 4:
        eFormat = GAN_RGB_COLOUR_ALPHA_IMAGE;
        if(ui16Dimension != 3)
        {
           gan_err_flush_trace();
           gan_err_register ( "gan_read_sgi_image_stream", GAN_ERROR_CORRUPTED_FILE, "corrupted SGI file header (truncated file?)" );
           return NULL;
        }

        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_read_sgi_image_stream", GAN_ERROR_CORRUPTED_FILE, "z-size" );
        return NULL;
   }

   /* determine image type */
   switch(ui8BytesPerPixel)
   {
      case 1: eType = GAN_UINT8; break;
      case 2: eType = GAN_UINT16; break;
      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_read_sgi_image_stream", GAN_ERROR_CORRUPTED_FILE, "bpc" );
        return NULL;
   }
      
   /* check colourmap type */
   if(ui32Colourmap != 0)
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_read_sgi_image_stream", GAN_ERROR_CORRUPTED_FILE, "colourmap" );
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

   /* SGI format specifies a lower-left reference, so flip */
   flip = !flip;
   upper = !upper;

   /* interlaced frames must have even height */
   if(single_field)
   {
      if((iHeight % 2) != 0)
      {
         gan_err_flush_trace();
         gan_err_register ( "gan_read_sgi_image_stream", GAN_ERROR_INCOMPATIBLE, "" );
         return NULL;
      }

      iInternalHeight = iHeight/2;
   }
   else
      iInternalHeight = iHeight;

   if(header != NULL)
   {
      header->file_format = GAN_SGI_FORMAT;
      header->width = (unsigned int)iWidth;
      header->height = (unsigned int)(whole_image ? iHeight : iInternalHeight);
      header->format = eFormat;
      header->type = eType;
   }

   if(ictrlstr != NULL && ictrlstr->header_only)
      return (Gan_Image*)-1; /* special value */

   if ( image == NULL )
      image = gan_image_alloc ( eFormat, eType, whole_image ? iHeight : iInternalHeight, iWidth );
   else
      image = gan_image_set_format_type_dims ( image, eFormat, eType, whole_image ? iHeight : iInternalHeight, iWidth );

   if ( image == NULL )
   {
      gan_err_register ( "gan_read_sgi_image_stream", GAN_ERROR_FAILURE, "setting up image" );
      return NULL;
   }

   /* if the image has zero size then we have finished */
   if ( iWidth == 0 || iHeight == 0 ) return image;

   switch(ui8Storage)
   {
      case 0:
      {
         char *achScanline;
         int iPlane, iRow;

         achScanline = gan_malloc_array(char, ui16ZSize*iWidth);
         if(achScanline == NULL)
         {
            gan_err_flush_trace();
            gan_err_register_with_number ( "gan_read_sgi_image_stream", GAN_ERROR_MALLOC_FAILED, "", ui16ZSize*iWidth*sizeof(char) );
            return NULL;
         }

         for(iPlane=0; iPlane<(int)ui16ZSize; iPlane++)
         {
            for(iRow=0; iRow<iHeight; iRow++)
            {
               /* read a scanline */
               if(fread(achScanline, ui8BytesPerPixel, iWidth, infile) != iWidth)
               {
                  gan_err_flush_trace();
                  gan_err_register ( "gan_read_sgi_image_stream", GAN_ERROR_CORRUPTED_FILE, "corrupted SGI file header (truncated file?)" );
                  return NULL;
               }

               /* write into image */
               if(single_field)
               {
                  /* only transfer even rows for upper field, or odd rows for upper field */
                  if((upper && (iRow % 2) == 0) || (!upper && (iRow % 2) == 1))
                  {
                     if(!convert_scanline(achScanline, image, iPlane, whole_image ? (flip?(iHeight-iRow-1):iRow) : (flip?(iInternalHeight-iRow/2-1):iRow/2), bReversedEndianness))
                     {
                        gan_err_register ( "gan_read_sgi_image_stream", GAN_ERROR_FAILURE, "" );
                        return NULL;
                     }
                  }
               }
               else
               {
                  if(!convert_scanline(achScanline, image, iPlane, flip ? (iHeight-iRow-1) : iRow, bReversedEndianness))
                  {
                     gan_err_register ( "gan_read_sgi_image_stream", GAN_ERROR_FAILURE, "" );
                     return NULL;
                  }
               }
            }
         }

         free(achScanline);
      }
      break;

      case 1:
      {
         /* read run-length encoded SGI image */
         gan_uint32 *aui32StartTab, *aui32LengthTab;
         unsigned int uiTabLen = ui16Height*ui16ZSize;
         unsigned int uiCtr, uiMaxRLELength = 4*ui16Width+10;
         char *acRLEBuffer;
         unsigned ui32FileOffset=0;

         aui32StartTab  = gan_malloc_array(gan_uint32, uiTabLen);
         aui32LengthTab = gan_malloc_array(gan_uint32, uiTabLen);
         if(ui8BytesPerPixel == 1)
            acRLEBuffer = (char*)gan_malloc_array(gan_uint8, uiMaxRLELength);
         else
            acRLEBuffer = (char*)gan_malloc_array(gan_uint16, uiMaxRLELength);

         if(aui32StartTab == NULL || aui32LengthTab == NULL || acRLEBuffer == NULL)
         {
            gan_err_flush_trace();
            gan_err_register_with_number ( "gan_read_sgi_image_stream", GAN_ERROR_MALLOC_FAILED, "", uiMaxRLELength*sizeof(gan_uint16) );
            return NULL;
         }

         if(fread((void*)aui32StartTab,  sizeof(gan_uint32), uiTabLen, infile) != uiTabLen ||
            fread((void*)aui32LengthTab, sizeof(gan_uint32), uiTabLen, infile) != uiTabLen)
         {
            gan_err_flush_trace();
            gan_err_register ( "gan_read_sgi_image_stream", GAN_ERROR_CORRUPTED_FILE, "corrupted SGI file RLE offsets (truncated file?)" );
            gan_free_va(acRLEBuffer, aui32LengthTab, aui32StartTab, NULL);
            return NULL;
         }

         if(bReversedEndianness)
         {
            vReverseEndiannessArray32(aui32StartTab, uiTabLen);
            vReverseEndiannessArray32(aui32LengthTab, uiTabLen);
         }
         
         for(uiCtr=0; uiCtr<uiTabLen; uiCtr++)
         {
            if(aui32StartTab[uiCtr] != ui32FileOffset)
            {
               if(fseek(infile, aui32StartTab[uiCtr], SEEK_SET) != 0)
               {
                  gan_err_flush_trace();
                  gan_err_register ( "gan_read_sgi_image_stream", GAN_ERROR_CORRUPTED_FILE, "corrupted SGI file (truncated file?)" );
                  gan_free_va(acRLEBuffer, aui32LengthTab, aui32StartTab, NULL);
                  return NULL;
               }

               ui32FileOffset = aui32StartTab[uiCtr]+aui32LengthTab[uiCtr];
            }

            if(fread(acRLEBuffer, 1, aui32LengthTab[uiCtr], infile) != aui32LengthTab[uiCtr])
            {
               gan_err_flush_trace();
               gan_err_register ( "gan_read_sgi_image_stream", GAN_ERROR_CORRUPTED_FILE, "corrupted SGI file RLE offsets (truncated file?)" );
               gan_free_va(acRLEBuffer, aui32LengthTab, aui32StartTab, NULL);
               return NULL;
            }

            if(ui8BytesPerPixel == 2 && bReversedEndianness)
               vReverseEndiannessArray16((gan_uint16*)acRLEBuffer, aui32LengthTab[uiCtr]/2);

            if(ui8BytesPerPixel == 1 ? (!bExpandRLEData8Bit((gan_uint8*)acRLEBuffer, uiCtr % ui16Height, (int)(uiCtr / ui16Height), image, flip, single_field, upper, whole_image))
                                     : (!bExpandRLEData16Bit((gan_uint16*)acRLEBuffer, uiCtr % ui16Height, (int)(uiCtr / ui16Height), image, flip, single_field, upper, whole_image)))
            {
               gan_err_flush_trace();
               gan_err_register ( "gan_read_sgi_image_stream", GAN_ERROR_FAILURE, "" );
               gan_free_va(acRLEBuffer, aui32LengthTab, aui32StartTab, NULL);
               return NULL;
            }
         }

         gan_free_va(acRLEBuffer, aui32LengthTab, aui32StartTab, NULL);
      }
      break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_read_sgi_image_stream", GAN_ERROR_ILLEGAL_TYPE, "" );
        return NULL;
   }

   /* success */
   return image;
}

/**
 * \brief Reads a RGB colour image file in SGI format.
 * \param filename The name of the image file
 * \param image The image structure to read the image data into or \c NULL
 * \param ictrlstr Pointer to structure controlling input or \c NULL
 * \param header Pointer to file header structure to be filled, or \c NULL
 * \return Pointer to image structure, or \c NULL on failure.
 *
 * Reads the SGI image with the in the file \a filename into the given
 * \a image. If \a image is \c NULL a new image is dynamically allocated;
 * otherwise the already allocated \a image structure is reused.
 *
 * \sa gan_write_sgi_image().
 */
Gan_Image *
 gan_read_sgi_image ( const char *filename, Gan_Image *image, const struct Gan_ImageReadControlStruct *ictrlstr, struct Gan_ImageHeaderStruct *header )
{
   FILE *infile;
   Gan_Image *result;

   /* attempt to open file */
   infile = fopen ( filename, "rb" );
   if ( infile == NULL )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_read_sgi_image", GAN_ERROR_OPENING_FILE, filename );
      return NULL;
   }

   result = gan_read_sgi_image_stream ( infile, image, ictrlstr, header );
   fclose(infile);
   return result;
}

/**
 * \brief Initialises the output control structure for SGI files
 *
 * This function should be called on the structure to set the parameters to default values.
 * Then set any non-default values yourself before calling gan_write_sgi_image() or
 * gan_write_sgi_image_stream().
 */
void gan_initialise_sgi_header_struct(Gan_SGIHeaderStruct *octrlstr)
{
}

/**
 * \brief Writes a RGB colour image to a file stream in SGI format.
 * \param outfile The file stream
 * \param image The image structure to write to the file
 * \param new_file Whether the file does not currently exist
 * \param octrlstr Pointer to structure controlling output or \c NULL
 * \return #GAN_TRUE on successful write operation, or #GAN_FALSE on failure.
 *
 * Writes the \a image into the file stream \a outfile in SGI format.
 *
 * \sa gan_read_sgi_image_stream().
 */
Gan_Bool
 gan_write_sgi_image_stream ( FILE *outfile, const Gan_Image *image, Gan_Bool new_file, const struct Gan_ImageWriteControlStruct *octrlstr )
{
   gan_uint8 ui8BytesPerPixel;
   gan_uint16 ui16ZSize;
   Gan_Bool flip=GAN_FALSE, single_field=GAN_FALSE, upper=GAN_FALSE, whole_image=GAN_FALSE;
   Gan_Bool bReversedEndianness = GAN_FALSE;
   unsigned int height=image->height;
   char* achBlankRow=NULL;

#ifndef WORDS_BIGENDIAN
   bReversedEndianness = GAN_TRUE;
#endif

   /* read generic control parameters */
   if(octrlstr != NULL)
   {
      flip         = octrlstr->flip;
      single_field = octrlstr->single_field;
      upper        = octrlstr->upper;
      whole_image  = octrlstr->whole_image;
   }

   /* SGI format specifies a lower-left reference, so flip */
   flip = !flip;
   upper = !upper;

   if(single_field && !whole_image)
      height *= 2;

   if ( (image->format != GAN_GREY_LEVEL_IMAGE && image->format != GAN_RGB_COLOUR_IMAGE && image->format != GAN_RGB_COLOUR_ALPHA_IMAGE)
        || (image->type != GAN_UINT8 && image->type != GAN_UINT16) )
   {
      if(outfile != NULL)
      {
         gan_err_flush_trace();
         gan_err_register("gan_write_sgi_image_stream", GAN_ERROR_INCOMPATIBLE, "" );
      }

      return GAN_FALSE;
   }

   /* numbers of channels and bytes per pixel */
   ui16ZSize = (image->format == GAN_GREY_LEVEL_IMAGE ? 1 : (image->format == GAN_RGB_COLOUR_IMAGE) ? 3 : 4);
   ui8BytesPerPixel = (image->type == GAN_UINT8) ? 1 : 2;

   /* write header */
   if(outfile != NULL)
   {
      char acHeader[BIG_BUFFER_SIZE], *acAlignedHeader;
      gan_uint16 ui16Magic = 474; /* magic number */
      gan_uint16 ui16Dimension = (image->format == GAN_GREY_LEVEL_IMAGE) ? 2 : 3;
      gan_uint16 ui16Width = (gan_uint16)image->width;
      gan_uint16 ui16Height = (gan_uint16)height;
      gan_uint32 ui32PixMin = 0; /* minimum pixel value */
      gan_uint32 ui32PixMax = (image->type == GAN_UINT8) ? 0xff : 0xffff; /* maximum pixel value */

         
      /* align the header array */
      acAlignedHeader = (char*)((unsigned long int)acHeader + 7 - (((unsigned long int)acHeader + 7) % 8));
      memset(acAlignedHeader, 0, 512);

      *((gan_uint16*)&acAlignedHeader[SGI_MAGIC_NUMBER_OFFSET])     = ui16Magic;
      *((gan_uint8*)&acAlignedHeader[SGI_STORAGE_OFFSET])           = 0;   /* no compression */
      *((gan_uint8*)&acAlignedHeader[SGI_BPC_OFFSET])               = ui8BytesPerPixel;
      *((gan_uint16*)&acAlignedHeader[SGI_DIMENSION_OFFSET])        = ui16Dimension;
      *((gan_uint16*)&acAlignedHeader[SGI_XSIZE_OFFSET])            = ui16Width;
      *((gan_uint16*)&acAlignedHeader[SGI_YSIZE_OFFSET])            = ui16Height;
      *((gan_uint16*)&acAlignedHeader[SGI_ZSIZE_OFFSET])            = ui16ZSize;
      *((gan_uint32*)&acAlignedHeader[SGI_PIXMIN_OFFSET])           = ui32PixMin;
      *((gan_uint32*)&acAlignedHeader[SGI_PIXMAX_OFFSET])           = ui32PixMax;
      *((gan_uint32*)&acAlignedHeader[SGI_COLOURMAP_OFFSET])        = 0;
      if(bReversedEndianness)
      {
         vReverseEndianness16((gan_uint16*)&acAlignedHeader[SGI_MAGIC_NUMBER_OFFSET]);
         vReverseEndianness16((gan_uint16*)&acAlignedHeader[SGI_DIMENSION_OFFSET]);
         vReverseEndianness16((gan_uint16*)&acAlignedHeader[SGI_XSIZE_OFFSET]);
         vReverseEndianness16((gan_uint16*)&acAlignedHeader[SGI_YSIZE_OFFSET]);
         vReverseEndianness16((gan_uint16*)&acAlignedHeader[SGI_ZSIZE_OFFSET]);
         vReverseEndianness32((gan_uint32*)&acAlignedHeader[SGI_PIXMIN_OFFSET]);
         vReverseEndianness32((gan_uint32*)&acAlignedHeader[SGI_PIXMAX_OFFSET]);
         vReverseEndianness32((gan_uint32*)&acAlignedHeader[SGI_COLOURMAP_OFFSET]);
      }

      fwrite ( acAlignedHeader, 1, 512, outfile );
   }

   /* if the image has zero size then we have finished */
   if ( image->width == 0 || image->height == 0 ) return GAN_TRUE;

   if(single_field && new_file)
   {
      /* create blank row */
      achBlankRow = gan_malloc_array(char, (size_t)ui8BytesPerPixel*image->width);
      if(achBlankRow == NULL)
      {
         gan_err_flush_trace();
         gan_err_register_with_number ( "gan_write_sgi_image_stream", GAN_ERROR_MALLOC_FAILED, "", (size_t)ui8BytesPerPixel*image->width );
         return GAN_FALSE;
      }

      memset(achBlankRow, 0, (size_t)ui8BytesPerPixel*image->width);
   }

   if(outfile != NULL)
   {
      char *achScanline;
      int iPlane, r;

      achScanline = gan_malloc_array(char, (size_t)ui8BytesPerPixel*image->width);
      if(achScanline == NULL)
      {
         gan_err_flush_trace();
         gan_err_register_with_number ( "gan_write_sgi_image_stream", GAN_ERROR_MALLOC_FAILED, "", (size_t)ui8BytesPerPixel*sizeof(char) );
         return GAN_FALSE;
      }

      for(iPlane=0; iPlane<(int)ui16ZSize; iPlane++)
         for ( r=0; r < height; r++ )
         {
            /* ignore row if it's in a field we're not writing */
            if(single_field && ((upper && ((r % 2) == 1)) || (!upper && ((r % 2) == 0))))
            {
               if(new_file)
                  /* write a blank row */
                  fwrite(achBlankRow, (size_t)ui8BytesPerPixel, image->width, outfile );
               else
                  /* leave this row as is was */
                  fseek(outfile, (size_t)ui8BytesPerPixel*image->width, SEEK_CUR);
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

               convert_to_scanline(image, rp, iPlane, achScanline, bReversedEndianness);
               fwrite ( achScanline, (size_t)ui8BytesPerPixel, image->width, outfile );
            }
         }

      free(achScanline);
   }

   if(achBlankRow != NULL)
      free(achBlankRow);

   /* success */
   return GAN_TRUE;
}

/**
 * \brief Writes a RGB colour image file in SGI format.
 * \param filename The name of the image file
 * \param image The image structure to write to the file
 * \param octrlstr Pointer to structure controlling output or \c NULL
 * \return #GAN_TRUE on successful write operation, #GAN_FALSE on failure.
 *
 * Writes the \a image into SGI file \a filename.
 *
 * \sa gan_read_sgi_image().
 */
Gan_Bool
 gan_write_sgi_image ( const char *filename, const Gan_Image *image, const struct Gan_ImageWriteControlStruct *octrlstr )
{
   if(filename == NULL)
      return gan_write_sgi_image_stream ( NULL, image, GAN_FALSE, octrlstr );
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
               uiFileSize = 512+image->height*image->width*gan_image_pixel_size(image->format, image->type);
            else
               uiFileSize = 512+2*image->height*image->width*gan_image_pixel_size(image->format, image->type);

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
         gan_err_register ( "gan_write_sgi_image", GAN_ERROR_OPENING_FILE, filename );
         return GAN_FALSE;
      }

      result = gan_write_sgi_image_stream ( outfile, image, new_file, octrlstr );
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

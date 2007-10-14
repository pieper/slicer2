/**
 * File:          $RCSfile: image_convert.c,v $
 * Module:        Convert (part of) an image
 * Part of:       Gandalf Library
 *
 * Revision:      $Revision: 1.1.2.1 $
 * Last edited:   $Date: 2007/10/14 02:20:18 $
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

#include <math.h>
#include <gandalf/image/image_convert.h>
#include <gandalf/image/image_rgb_uint8.h>
#include <gandalf/image/image_rgb_uint16.h>
#include <gandalf/image/image_rgb_float32.h>
#include <gandalf/image/image_rgba_uint8.h>
#include <gandalf/image/image_rgba_uint16.h>
#include <gandalf/image/image_rgba_float32.h>

/**
 * \addtogroup ImagePackage
 * \{
 */

/**
 * \defgroup ImageConvert Convert an image
 * \{
 */

/**
 * \brief Converts an image ARGB to RGBA, returning a new image
 * \param source The input image
 * \param dest The output image
 * \return Pointer to the converted RGBA image, or \c NULL on failure.
 */
Gan_Image *gan_image_swap_RandB_q(Gan_Image *source, Gan_Image *dest)
{
   if(source->format != GAN_RGB_COLOUR_IMAGE)
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_image_swap_RandB_q", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
      return NULL;
   }
      
   if(dest == NULL)
      dest = gan_image_alloc(GAN_RGB_COLOUR_IMAGE, source->type, source->height, source->width);
   else if(dest != source)
      dest = gan_image_set_format_type_dims(dest, GAN_RGB_COLOUR_IMAGE, source->type, source->height, source->width);

   if(dest == NULL)
   {
      gan_err_register ( "gan_image_swap_RandB_q", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   switch(source->type)
   {
      case GAN_UINT8:
      {
         int iRow, iCol;

         if(dest == source)
         {
            Gan_RGBPixel_ui8 *pPix, Pix;
            
            for(iRow=(int)source->height-1; iRow>=0; iRow--)
               for(iCol=(int)source->width-1, pPix = gan_image_get_pixptr_rgb_ui8(source,iRow,0); iCol>=0; iCol--, pPix++)
               {
                  Pix = *pPix;
                  pPix->R = Pix.B;
                  pPix->G = Pix.G;
                  pPix->B = Pix.R;
               }
         }
         else
         {
            Gan_RGBPixel_ui8 *pPixI, *pPixO;
               
            for(iRow=(int)source->height-1; iRow>=0; iRow--)
               for(iCol=(int)source->width-1, pPixI = gan_image_get_pixptr_rgb_ui8(source,iRow,0), pPixO = gan_image_get_pixptr_rgb_ui8(source,iRow,0); iCol>=0; iCol--, pPixI++, pPixO++)
               {
                  pPixO->R = pPixI->B;
                  pPixO->G = pPixI->G;
                  pPixO->B = pPixI->R;
               }
         }
      }
      break;

      case GAN_UINT16:
      {
         int iRow, iCol;

         if(dest == source)
         {
            Gan_RGBPixel_ui16 *pPix, Pix;
            
            for(iRow=(int)source->height-1; iRow>=0; iRow--)
               for(iCol=(int)source->width-1, pPix = gan_image_get_pixptr_rgb_ui16(source,iRow,0); iCol>=0; iCol--, pPix++)
               {
                  Pix = *pPix;
                  pPix->R = Pix.B;
                  pPix->G = Pix.G;
                  pPix->B = Pix.R;
               }
         }
         else
         {
            Gan_RGBPixel_ui16 *pPixI, *pPixO;
               
            for(iRow=(int)source->height-1; iRow>=0; iRow--)
               for(iCol=(int)source->width-1, pPixI = gan_image_get_pixptr_rgb_ui16(source,iRow,0), pPixO = gan_image_get_pixptr_rgb_ui16(source,iRow,0); iCol>=0; iCol--, pPixI++, pPixO++)
               {
                  pPixO->R = pPixI->B;
                  pPixO->G = pPixI->G;
                  pPixO->B = pPixI->R;
               }
         }
      }
      break;

      case GAN_FLOAT32:
      {
         int iRow, iCol;

         if(dest == source)
         {
            Gan_RGBPixel_f32 *pPix, Pix;
            
            for(iRow=(int)source->height-1; iRow>=0; iRow--)
               for(iCol=(int)source->width-1, pPix = gan_image_get_pixptr_rgb_f32(source,iRow,0); iCol>=0; iCol--, pPix++)
               {
                  Pix = *pPix;
                  pPix->R = Pix.B;
                  pPix->G = Pix.G;
                  pPix->B = Pix.R;
               }
         }
         else
         {
            Gan_RGBPixel_f32 *pPixI, *pPixO;
               
            for(iRow=(int)source->height-1; iRow>=0; iRow--)
               for(iCol=(int)source->width-1, pPixI = gan_image_get_pixptr_rgb_f32(source,iRow,0), pPixO = gan_image_get_pixptr_rgb_f32(source,iRow,0); iCol>=0; iCol--, pPixI++, pPixO++)
               {
                  pPixO->R = pPixI->B;
                  pPixO->G = pPixI->G;
                  pPixO->B = pPixI->R;
               }
         }
      }
      break;

      default:
      gan_err_flush_trace();
      gan_err_register ( "gan_image_swap_RandB_q", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
      return NULL;
   }
      
   return dest;
}

/**
 * \brief Converts an image ARGB to RGBA, returning a new image
 * \param source The input image
 * \param dest The output image
 * \return Pointer to the converted RGBA image, or \c NULL on failure.
 */
Gan_Image *gan_image_convert_ARGBtoRGBA_q(Gan_Image *source, Gan_Image *dest)
{
   if(source->format != GAN_RGB_COLOUR_ALPHA_IMAGE)
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_image_convert_ARGBtoRGBA_q", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
      return NULL;
   }
      
   if(dest == NULL)
      dest = gan_image_alloc(GAN_RGB_COLOUR_ALPHA_IMAGE, source->type, source->height, source->width);
   else if(dest != source)
      dest = gan_image_set_format_type_dims(dest, GAN_RGB_COLOUR_ALPHA_IMAGE, source->type, source->height, source->width);

   if(dest == NULL)
   {
      gan_err_register ( "gan_image_convert_ARGBtoRGBA_q", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   switch(source->type)
   {
      case GAN_UINT8:
      {
         int iRow, iCol;

         if(dest == source)
         {
            Gan_RGBAPixel_ui8 *pPix, Pix;
            
            for(iRow=(int)source->height-1; iRow>=0; iRow--)
               for(iCol=(int)source->width-1, pPix = gan_image_get_pixptr_rgba_ui8(source,iRow,0); iCol>=0; iCol--, pPix++)
               {
                  Pix = *pPix;
                  pPix->R = Pix.G;
                  pPix->G = Pix.B;
                  pPix->B = Pix.A;
                  pPix->A = Pix.R;
               }
         }
         else
         {
            Gan_RGBAPixel_ui8 *pPixI, *pPixO;
               
            for(iRow=(int)source->height-1; iRow>=0; iRow--)
               for(iCol=(int)source->width-1, pPixI = gan_image_get_pixptr_rgba_ui8(source,iRow,0), pPixO = gan_image_get_pixptr_rgba_ui8(source,iRow,0); iCol>=0; iCol--, pPixI++, pPixO++)
               {
                  pPixO->R = pPixI->G;
                  pPixO->G = pPixI->B;
                  pPixO->B = pPixI->A;
                  pPixO->A = pPixI->R;
               }
         }
      }
      break;

      case GAN_UINT16:
      {
         int iRow, iCol;

         if(dest == source)
         {
            Gan_RGBAPixel_ui16 *pPix, Pix;
            
            for(iRow=(int)source->height-1; iRow>=0; iRow--)
               for(iCol=(int)source->width-1, pPix = gan_image_get_pixptr_rgba_ui16(source,iRow,0); iCol>=0; iCol--, pPix++)
               {
                  Pix = *pPix;
                  pPix->R = Pix.G;
                  pPix->G = Pix.B;
                  pPix->B = Pix.A;
                  pPix->A = Pix.R;
               }
         }
         else
         {
            Gan_RGBAPixel_ui16 *pPixI, *pPixO;
               
            for(iRow=(int)source->height-1; iRow>=0; iRow--)
               for(iCol=(int)source->width-1, pPixI = gan_image_get_pixptr_rgba_ui16(source,iRow,0), pPixO = gan_image_get_pixptr_rgba_ui16(source,iRow,0); iCol>=0; iCol--, pPixI++, pPixO++)
               {
                  pPixO->R = pPixI->G;
                  pPixO->G = pPixI->B;
                  pPixO->B = pPixI->A;
                  pPixO->A = pPixI->R;
               }
         }
      }
      break;

      case GAN_FLOAT32:
      {
         int iRow, iCol;

         if(dest == source)
         {
            Gan_RGBAPixel_f32 *pPix, Pix;
            
            for(iRow=(int)source->height-1; iRow>=0; iRow--)
               for(iCol=(int)source->width-1, pPix = gan_image_get_pixptr_rgba_f32(source,iRow,0); iCol>=0; iCol--, pPix++)
               {
                  Pix = *pPix;
                  pPix->R = Pix.G;
                  pPix->G = Pix.B;
                  pPix->B = Pix.A;
                  pPix->A = Pix.R;
               }
         }
         else
         {
            Gan_RGBAPixel_f32 *pPixI, *pPixO;
               
            for(iRow=(int)source->height-1; iRow>=0; iRow--)
               for(iCol=(int)source->width-1, pPixI = gan_image_get_pixptr_rgba_f32(source,iRow,0), pPixO = gan_image_get_pixptr_rgba_f32(source,iRow,0); iCol>=0; iCol--, pPixI++, pPixO++)
               {
                  pPixO->R = pPixI->G;
                  pPixO->G = pPixI->B;
                  pPixO->B = pPixI->A;
                  pPixO->A = pPixI->R;
               }
         }
      }
      break;

      default:
      gan_err_flush_trace();
      gan_err_register ( "gan_image_convert_ARGBtoRGBA_q", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
      return NULL;
   }
      
   return dest;
}

/**
 * \brief Converts an image RGBA to ARGB, returning a new image
 * \param source The input image
 * \param dest The output image
 * \return Pointer to the converted ARGB image, or \c NULL on failure.
 */
Gan_Image *gan_image_convert_RGBAtoARGB_q(Gan_Image *source, Gan_Image *dest)
{
   if(source->format != GAN_RGB_COLOUR_ALPHA_IMAGE)
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_image_convert_RGBAtoARGB_q", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
      return NULL;
   }
      
   if(dest == NULL)
      dest = gan_image_alloc(GAN_RGB_COLOUR_ALPHA_IMAGE, source->type, source->height, source->width);
   else if(dest != source)
      dest = gan_image_set_format_type_dims(dest, GAN_RGB_COLOUR_ALPHA_IMAGE, source->type, source->height, source->width);

   if(dest == NULL)
   {
      gan_err_register ( "gan_image_convert_RGBAtoARGB_q", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   switch(source->type)
   {
      case GAN_UINT8:
      {
         int iRow, iCol;

         if(dest == source)
         {
            Gan_RGBAPixel_ui8 *pPix, Pix;
            
            for(iRow=(int)source->height-1; iRow>=0; iRow--)
               for(iCol=(int)source->width-1, pPix = gan_image_get_pixptr_rgba_ui8(source,iRow,0); iCol>=0; iCol--, pPix++)
               {
                  Pix = *pPix;
                  pPix->A = Pix.B;
                  pPix->R = Pix.A;
                  pPix->G = Pix.R;
                  pPix->B = Pix.G;
               }
         }
         else
         {
            Gan_RGBAPixel_ui8 *pPixI, *pPixO;
               
            for(iRow=(int)source->height-1; iRow>=0; iRow--)
               for(iCol=(int)source->width-1, pPixI = gan_image_get_pixptr_rgba_ui8(source,iRow,0), pPixO = gan_image_get_pixptr_rgba_ui8(source,iRow,0); iCol>=0; iCol--, pPixI++, pPixO++)
               {
                  pPixO->A = pPixI->B;
                  pPixO->R = pPixI->A;
                  pPixO->G = pPixI->R;
                  pPixO->B = pPixI->G;
               }
         }
      }
      break;

      case GAN_UINT16:
      {
         int iRow, iCol;

         if(dest == source)
         {
            Gan_RGBAPixel_ui16 *pPix, Pix;
            
            for(iRow=(int)source->height-1; iRow>=0; iRow--)
               for(iCol=(int)source->width-1, pPix = gan_image_get_pixptr_rgba_ui16(source,iRow,0); iCol>=0; iCol--, pPix++)
               {
                  Pix = *pPix;
                  pPix->A = Pix.B;
                  pPix->R = Pix.A;
                  pPix->G = Pix.R;
                  pPix->B = Pix.G;
               }
         }
         else
         {
            Gan_RGBAPixel_ui16 *pPixI, *pPixO;
               
            for(iRow=(int)source->height-1; iRow>=0; iRow--)
               for(iCol=(int)source->width-1, pPixI = gan_image_get_pixptr_rgba_ui16(source,iRow,0), pPixO = gan_image_get_pixptr_rgba_ui16(source,iRow,0); iCol>=0; iCol--, pPixI++, pPixO++)
               {
                  pPixO->A = pPixI->B;
                  pPixO->R = pPixI->A;
                  pPixO->G = pPixI->R;
                  pPixO->B = pPixI->G;
               }
         }
      }
      break;

      case GAN_FLOAT32:
      {
         int iRow, iCol;

         if(dest == source)
         {
            Gan_RGBAPixel_f32 *pPix, Pix;
            
            for(iRow=(int)source->height-1; iRow>=0; iRow--)
               for(iCol=(int)source->width-1, pPix = gan_image_get_pixptr_rgba_f32(source,iRow,0); iCol>=0; iCol--, pPix++)
               {
                  Pix = *pPix;
                  pPix->A = Pix.B;
                  pPix->R = Pix.A;
                  pPix->G = Pix.R;
                  pPix->B = Pix.G;
               }
         }
         else
         {
            Gan_RGBAPixel_f32 *pPixI, *pPixO;
               
            for(iRow=(int)source->height-1; iRow>=0; iRow--)
               for(iCol=(int)source->width-1, pPixI = gan_image_get_pixptr_rgba_f32(source,iRow,0), pPixO = gan_image_get_pixptr_rgba_f32(source,iRow,0); iCol>=0; iCol--, pPixI++, pPixO++)
               {
                  pPixO->A = pPixI->B;
                  pPixO->R = pPixI->A;
                  pPixO->G = pPixI->R;
                  pPixO->B = pPixI->G;
               }
         }
      }
      break;

      default:
      gan_err_flush_trace();
      gan_err_register ( "gan_image_convert_RGBAtoARGB_q", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
      return NULL;
   }
      
   return dest;
}

/**
 * \}
 */

/**
 * \}
 */

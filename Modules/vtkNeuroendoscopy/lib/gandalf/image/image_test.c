/**
 * File:          $RCSfile: image_test.c,v $
 * Module:        Image package test program
 * Part of:       Gandalf Library
 *
 * Revision:      $Revision: 1.1.2.1 $
 * Last edited:   $Date: 2007/10/14 02:20:25 $
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

#include <gandalf/TestFramework/cUnit.h>
#include <gandalf/image/image_test.h>
#include <stdio.h>
#include <assert.h>
#include <gandalf/image/image_defs.h>
#include <gandalf/image/image_convert.h>
#include <gandalf/image/image_flip.h>
#include <gandalf/image/image_pointer.h>
#include <gandalf/image/image_gl_uint8.h>
#include <gandalf/image/image_gl_uint16.h>
#include <gandalf/image/image_gl_uint32.h>
#include <gandalf/image/image_gl_uchar.h>
#include <gandalf/image/image_gl_short.h>
#include <gandalf/image/image_gl_ushort.h>
#include <gandalf/image/image_gl_int.h>
#include <gandalf/image/image_gl_uint.h>
#include <gandalf/image/image_gl_float.h>
#include <gandalf/image/image_gl_double.h>
#include <gandalf/image/image_gla_uint8.h>
#include <gandalf/image/image_gla_uint16.h>
#include <gandalf/image/image_gla_uint32.h>
#include <gandalf/image/image_gla_uchar.h>
#include <gandalf/image/image_gla_short.h>
#include <gandalf/image/image_gla_ushort.h>
#include <gandalf/image/image_gla_int.h>
#include <gandalf/image/image_gla_uint.h>
#include <gandalf/image/image_gla_float.h>
#include <gandalf/image/image_gla_double.h>
#include <gandalf/image/image_rgb_uint8.h>
#include <gandalf/image/image_rgb_uint16.h>
#include <gandalf/image/image_rgb_uint32.h>
#include <gandalf/image/image_rgb_uchar.h>
#include <gandalf/image/image_rgb_short.h>
#include <gandalf/image/image_rgb_ushort.h>
#include <gandalf/image/image_rgb_int.h>
#include <gandalf/image/image_rgb_uint.h>
#include <gandalf/image/image_rgb_float.h>
#include <gandalf/image/image_rgb_double.h>
#include <gandalf/image/image_rgba_uint8.h>
#include <gandalf/image/image_rgba_uint16.h>
#include <gandalf/image/image_rgba_uint32.h>
#include <gandalf/image/image_rgba_uchar.h>
#include <gandalf/image/image_rgba_short.h>
#include <gandalf/image/image_rgba_ushort.h>
#include <gandalf/image/image_rgba_int.h>
#include <gandalf/image/image_rgba_uint.h>
#include <gandalf/image/image_rgba_float.h>
#include <gandalf/image/image_rgba_double.h>
#include <gandalf/image/image_bit.h>
#include <gandalf/image/io/image_io.h>
#include <gandalf/common/misc_error.h>
#include <gandalf/common/numerics.h>
#include <gandalf/common/compare.h>

/* specialised formats */
#include <gandalf/image/image_rgbx_uint8.h>
#include <gandalf/image/image_rgbx_uint10.h>
#include <gandalf/image/image_rgbas_uint10.h>
#include <gandalf/image/image_rgba_uint12.h>
#include <gandalf/image/image_yuvx444_uint8.h>
#include <gandalf/image/image_yuva444_uint8.h>
#include <gandalf/image/image_yuv422_uint8.h>
#include <gandalf/image/image_gl_uint10.h>
#include <gandalf/image/image_gl_uint12.h>

#if 0
/* Prints the image */
static Gan_Bool print_image ( Gan_Image *img )
{
   unsigned int r, c;

   printf ( "Image (%ld,%ld):\n", img->width, img->height );
        
   switch ( img->format )
   {
      case GAN_GREY_LEVEL_IMAGE:
      {
         switch ( img->type )
         {
            case GAN_UCHAR:
            {
               for ( r = 0; r < img->height; r++ )
               {
                  for ( c = 0; c < img->width; c++ )
                     printf ( " %2x", gan_image_get_pix_gl_uc(img,r,c) );
                  
                  printf ( "\n" );
               }
            } /* End of case */
            break;

            case GAN_INT:
            {
               for ( r = 0; r < img->height; r++ )
               {
                  for ( c = 0; c < img->width; c++ )
                     printf ( " %3x", gan_image_get_pix_gl_i(img,r,c) );
                  
                  printf ( "\n" );
               }
            } /* End of case */
            break;
                                
            case GAN_UINT:
            {
               for ( r = 0; r < img->height; r++ )
               {
                  for ( c = 0; c < img->width; c++ )
                     printf ( " %3x", gan_image_get_pix_gl_ui(img,r,c) );
                  
                  printf ( "\n" );
               }
            } /* End of case */
            break;
                                
            default:
              assert(0);
              break;
         }
      }
      break;
                
      case GAN_RGB_COLOUR_IMAGE:
      {
         switch ( img->type )
         {
            case GAN_UCHAR:
            {
               Gan_RGBPixel_uc rgb;
               
               for ( r = 0; r < img->height; r++ )
               {
                  for ( c = 0; c < img->width; c++ )
                  {
                     rgb = gan_image_get_pix_rgb_uc(img,r,c);
                     printf ( " (%x,%x,%x)", rgb.R, rgb.G, rgb.B );
                  }
                  
                  printf ( "\n" );
               }
            }
            break;
            
            default:
              assert(0);
              break;
         }
      }
      break;
      
      default:
        assert(0);
        break;
   }
   
   printf ( "\n" );
   return GAN_TRUE;
}
#endif

static Gan_Bool ramp_image ( Gan_Image *img )
{
   unsigned int r, c;
   
   switch ( img->format )
   {
      case GAN_GREY_LEVEL_IMAGE:
      {
         switch ( img->type )
         {
            case GAN_UCHAR:
            {
               for ( r = 0; r < img->height; r++ )
               {
                  for ( c = 0; c < img->width; c++ )
                     gan_image_set_pix_gl_uc ( img, r, c, r+c );
               }
            }
            break;
                                
            case GAN_UINT:
            {
               for ( r = 0; r < img->height; r++ )
               {
                  for ( c = 0; c < img->width; c++ )
                     gan_image_set_pix_gl_ui ( img, r, c, r+c );
               }
            }
            break;
                                
            default:
              assert(0);
              break;
         } /* End of nested switch */
      } /* End of case */
      break;
                
      case GAN_RGB_COLOUR_IMAGE:
      {
         switch ( img->type )
         {
            case GAN_UCHAR:
            {        
               Gan_RGBPixel_uc rgb;
               
               for ( r = 0; r < img->height; r++ )
               {
                  for ( c = 0; c < img->width; c++ )
                  {
                     rgb.R = rgb.G = rgb.B = r+c;
                     gan_image_set_pix_rgb_uc ( img, r, c, &rgb );
                  }
               }
            } /* End of case */
            break;
            
            default:
              assert(0);
              break;
         } /* End of switch */
         break;
      } /* End of case */        
      default:
        assert(0);
        break;
   } /* End of main switch */
   return GAN_TRUE;
}

static Gan_Bool
 image_bit_dilate_horiz_safe ( Gan_Image *image, int no_pixels,
                               Gan_Image *restrict_mask )
{
   Gan_Image *image_copy, *copy2;
   int row, col, pixel;

   image_copy = gan_image_copy_s(image);
   copy2 = gan_image_copy_s(image);
   cu_assert(image_copy != NULL && copy2 != NULL);
   for ( pixel = 0; pixel < no_pixels; pixel++ )
   {
      for ( row = (int)image->height-1; row >= 0; row-- )
         for ( col = (int)image->width-1; col >= 0; col-- )
            if ( gan_image_get_pix_b ( copy2, row, col ) )
            {
               if ( col > 0 )
                  cu_assert ( gan_image_set_pix_b ( image_copy, row, col-1,
                                                    GAN_TRUE ) );

               if ( col < (int)image->width-1 )
                  cu_assert ( gan_image_set_pix_b ( image_copy, row, col+1,
                                                    GAN_TRUE ) );
            }

      cu_assert ( gan_image_copy_q ( image_copy, copy2 ) != NULL );
   }

   if ( restrict_mask == NULL )
   {
      cu_assert ( gan_image_copy_q ( copy2, image ) != NULL );
   }
   else
   {
      cu_assert ( gan_image_bit_and_i ( copy2, restrict_mask ) );
      cu_assert ( gan_image_bit_or_i ( image, copy2 ) );
   }

   gan_image_free(copy2);
   gan_image_free(image_copy);
   return GAN_TRUE;
}

static Gan_Bool
 image_bit_dilate_vert_safe ( Gan_Image *image, int no_pixels,
                              Gan_Image *restrict_mask )
{
   Gan_Image *image_copy, *copy2;
   int row, col, pixel;

   image_copy = gan_image_copy_s(image);
   copy2 = gan_image_copy_s(image);
   cu_assert(image_copy != NULL && copy2 != NULL);
   for ( pixel = 0; pixel < no_pixels; pixel++ )
   {
      for ( row = (int)image->height-1; row >= 0; row-- )
         for ( col = (int)image->width-1; col >= 0; col-- )
            if ( gan_image_get_pix_b ( copy2, row, col ) )
            {
               if ( row > 0 )
                  cu_assert ( gan_image_set_pix_b ( image_copy, row-1, col,
                                                    GAN_TRUE ) );

               if ( row < (int)image->height-1 )
                  cu_assert ( gan_image_set_pix_b ( image_copy, row+1, col,
                                                    GAN_TRUE ) );
            }

      cu_assert ( gan_image_copy_q ( image_copy, copy2 ) != NULL );
   }

   if ( restrict_mask == NULL )
   {
      cu_assert ( gan_image_copy_q ( copy2, image ) != NULL );
   }
   else
   {
      cu_assert ( gan_image_bit_and_i ( copy2, restrict_mask ) );
      cu_assert ( gan_image_bit_or_i ( image, copy2 ) );
   }

   gan_image_free(copy2);
   gan_image_free(image_copy);
   return GAN_TRUE;
}

static Gan_Bool
 gan_no_bits_for_type ( Gan_Type type, unsigned *no_bits )
{
   switch ( type )
   {
      case GAN_UINT8:  *no_bits = 8;  return GAN_TRUE;
      case GAN_UINT16: *no_bits = 16; return GAN_TRUE;
      case GAN_UINT32: *no_bits = 32; return GAN_TRUE;
      case GAN_FLOAT:  *no_bits = 22; return GAN_TRUE;
      case GAN_DOUBLE: *no_bits = 46; return GAN_TRUE;
      default: cu_assert(0);
   }

   /* shouldn't get here */
   cu_assert(0);
}

#define NO_TESTS 10

/* precompiled test values */
static gan_uint8 val_ui8[NO_TESTS] =
{ 0u, 26u, 59u, 83u, 111u, 137u, 163u, 189u, 223u, 255u };

static gan_uint16 val_ui16[NO_TESTS] =
{ 0u, 6866u, 15258u, 21347u, 28538u, 35228u, 41953u, 48583u, 57278u, 65535u };

static gan_uint32 val_ui32[NO_TESTS] =
{ 0u, 450000000u, 1000000000u, 1399034893u, 1870283845u, 2308758392u,
  2749485920u, 3183948561u, 3753829104u, 4294967295u };

static float val_f[NO_TESTS] =
{ 0.0F, 0.10477378966863589F, 0.23283064370807974F, 0.32573819470725446F,
  0.43545939154817243F, 0.53754970257579104F, 0.64016457661990178F,
  0.74132079299104414F, 0.87400644665444416F, 1.0F };

static double val_d[NO_TESTS] =
{ 0.0, 0.10477378966863589, 0.23283064370807974, 0.32573819470725446,
  0.43545939154817243, 0.53754970257579104, 0.64016457661990178,
  0.74132079299104414, 0.87400644665444416, 1.0 };

static Gan_Image *from_image=NULL, *to_image=NULL;

static Gan_Bool
 fill_pixel ( Gan_ImageFormat format, Gan_Type type, int i, Gan_Pixel *pixel )
{
   pixel->format = format;
   pixel->type   = type;
   switch ( format )
   {
      case GAN_GREY_LEVEL_IMAGE:
        switch ( type )
        {
           case GAN_UINT8:  pixel->data.gl.ui8  = val_ui8[i];  break;
           case GAN_UINT16: pixel->data.gl.ui16 = val_ui16[i]; break;
           case GAN_UINT32: pixel->data.gl.ui32 = val_ui32[i]; break;
           case GAN_FLOAT:  pixel->data.gl.f    = val_f[i];    break;
           case GAN_DOUBLE: pixel->data.gl.d    = val_d[i];    break;
           default: cu_assert(0);
        }
        break;
           
      case GAN_GREY_LEVEL_ALPHA_IMAGE:
        switch ( type )
        {
           case GAN_UINT8:
             pixel->data.gla.ui8.I = val_ui8[i];
             pixel->data.gla.ui8.A = val_ui8[(i+1) % NO_TESTS];
             break;

           case GAN_UINT16:
             pixel->data.gla.ui16.I = val_ui16[i];
             pixel->data.gla.ui16.A = val_ui16[(i+1) % NO_TESTS];
             break;

           case GAN_UINT32:
             pixel->data.gla.ui32.I = val_ui32[i];
             pixel->data.gla.ui32.A = val_ui32[(i+1) % NO_TESTS];
             break;

           case GAN_FLOAT:
             pixel->data.gla.f.I = val_f[i];
             pixel->data.gla.f.A = val_f[(i+1) % NO_TESTS];
             break;

           case GAN_DOUBLE:
             pixel->data.gla.d.I = val_d[i];
             pixel->data.gla.d.A = val_d[(i+1) % NO_TESTS];
             break;

           default: cu_assert(0);
        }
        break;

      case GAN_RGB_COLOUR_IMAGE:
        switch ( type )
        {
           case GAN_UINT8:
             pixel->data.rgb.ui8.R = val_ui8[i];
             pixel->data.rgb.ui8.G = val_ui8[(i+1) % NO_TESTS];
             pixel->data.rgb.ui8.B = val_ui8[(i+2) % NO_TESTS];
             break;

           case GAN_UINT16:
             pixel->data.rgb.ui16.R = val_ui16[i];
             pixel->data.rgb.ui16.G = val_ui16[(i+1) % NO_TESTS];
             pixel->data.rgb.ui16.B = val_ui16[(i+2) % NO_TESTS];
             break;

           case GAN_UINT32:
             pixel->data.rgb.ui32.R = val_ui32[i];
             pixel->data.rgb.ui32.G = val_ui32[(i+1) % NO_TESTS];
             pixel->data.rgb.ui32.B = val_ui32[(i+2) % NO_TESTS];
             break;

           case GAN_FLOAT:
             pixel->data.rgb.f.R = val_f[i];
             pixel->data.rgb.f.G = val_f[(i+1) % NO_TESTS];
             pixel->data.rgb.f.B = val_f[(i+2) % NO_TESTS];
             break;

           case GAN_DOUBLE:
             pixel->data.rgb.d.R = val_d[i];
             pixel->data.rgb.d.G = val_d[(i+1) % NO_TESTS];
             pixel->data.rgb.d.B = val_d[(i+2) % NO_TESTS];
             break;

           default: cu_assert(0);
        }
        break;

      case GAN_RGB_COLOUR_ALPHA_IMAGE:
        switch ( type )
        {
           case GAN_UINT8:
             pixel->data.rgba.ui8.R = val_ui8[i];
             pixel->data.rgba.ui8.G = val_ui8[(i+1) % NO_TESTS];
             pixel->data.rgba.ui8.B = val_ui8[(i+2) % NO_TESTS];
             pixel->data.rgba.ui8.A = val_ui8[(i+3) % NO_TESTS];
             break;

           case GAN_UINT16:
             pixel->data.rgba.ui16.R = val_ui16[i];
             pixel->data.rgba.ui16.G = val_ui16[(i+1) % NO_TESTS];
             pixel->data.rgba.ui16.B = val_ui16[(i+2) % NO_TESTS];
             pixel->data.rgba.ui16.A = val_ui16[(i+3) % NO_TESTS];
             break;

           case GAN_UINT32:
             pixel->data.rgba.ui32.R = val_ui32[i];
             pixel->data.rgba.ui32.G = val_ui32[(i+1) % NO_TESTS];
             pixel->data.rgba.ui32.B = val_ui32[(i+2) % NO_TESTS];
             pixel->data.rgba.ui32.A = val_ui32[(i+3) % NO_TESTS];
             break;

           case GAN_FLOAT:
             pixel->data.rgba.f.R = val_f[i];
             pixel->data.rgba.f.G = val_f[(i+1) % NO_TESTS];
             pixel->data.rgba.f.B = val_f[(i+2) % NO_TESTS];
             pixel->data.rgba.f.A = val_f[(i+3) % NO_TESTS];
             break;

           case GAN_DOUBLE:
             pixel->data.rgba.d.R = val_d[i];
             pixel->data.rgba.d.G = val_d[(i+1) % NO_TESTS];
             pixel->data.rgba.d.B = val_d[(i+2) % NO_TESTS];
             pixel->data.rgba.d.A = val_d[(i+3) % NO_TESTS];
             break;

           default: cu_assert(0);
        }
        break;

      default: cu_assert(0);
   }

   return GAN_TRUE;
}

static Gan_Bool
 bComparePixels ( Gan_Pixel *pixel1, Gan_Pixel *pixel2, double precision )
{
   if ( fabs(pixel1->data.rgba.d.R - pixel2->data.rgba.d.R) >= precision ||
        fabs(pixel1->data.rgba.d.G - pixel2->data.rgba.d.G) >= precision ||
        fabs(pixel1->data.rgba.d.B - pixel2->data.rgba.d.B) >= precision ||
        fabs(pixel1->data.rgba.d.A - pixel2->data.rgba.d.A) >= precision )
      printf("Here\n");

   if(!(fabs(pixel1->data.rgba.d.R - pixel2->data.rgba.d.R) < precision))
      printf("Here\n");

   cu_assert (fabs(pixel1->data.rgba.d.R - pixel2->data.rgba.d.R) < precision);
   cu_assert (fabs(pixel1->data.rgba.d.G - pixel2->data.rgba.d.G) < precision);
   cu_assert (fabs(pixel1->data.rgba.d.B - pixel2->data.rgba.d.B) < precision);
   cu_assert (fabs(pixel1->data.rgba.d.A - pixel2->data.rgba.d.A) < precision);
   return GAN_TRUE;
}

static Gan_Bool
 test_conversion4 ( Gan_ImageFormat from_format, Gan_Type from_type,
                    Gan_ImageFormat to_format,   Gan_Type to_type )
{
   Gan_Bool test_reversal;
   Gan_Pixel from_pixel, to_pixel, test_pixel;
   int i;
   unsigned from_bits, to_bits;
   double from_precision, to_precision, precision;

   /* determine whether to test reversal of conversion given formats */
   switch ( from_format )
   {
      case GAN_GREY_LEVEL_IMAGE:
        switch ( to_format )
        {
           case GAN_GREY_LEVEL_IMAGE:       test_reversal = GAN_TRUE; break;
           case GAN_GREY_LEVEL_ALPHA_IMAGE: test_reversal = GAN_TRUE; break;
           case GAN_RGB_COLOUR_IMAGE:       test_reversal = GAN_TRUE; break;
           case GAN_RGB_COLOUR_ALPHA_IMAGE: test_reversal = GAN_TRUE; break;
           default: cu_assert(0);
        }
        break;

      case GAN_GREY_LEVEL_ALPHA_IMAGE:
        switch ( to_format )
        {
           case GAN_GREY_LEVEL_IMAGE:       test_reversal = GAN_FALSE; break;
           case GAN_GREY_LEVEL_ALPHA_IMAGE: test_reversal = GAN_TRUE;  break;
           case GAN_RGB_COLOUR_IMAGE:       test_reversal = GAN_FALSE; break;
           case GAN_RGB_COLOUR_ALPHA_IMAGE: test_reversal = GAN_TRUE;  break;
           default: cu_assert(0);
        }
        break;

      case GAN_RGB_COLOUR_IMAGE:
        switch ( to_format )
        {
           case GAN_GREY_LEVEL_IMAGE:       test_reversal = GAN_FALSE; break;
           case GAN_GREY_LEVEL_ALPHA_IMAGE: test_reversal = GAN_FALSE; break;
           case GAN_RGB_COLOUR_IMAGE:       test_reversal = GAN_TRUE;  break;
           case GAN_RGB_COLOUR_ALPHA_IMAGE: test_reversal = GAN_TRUE;  break;
           default: cu_assert(0);
        }
        break;

      case GAN_RGB_COLOUR_ALPHA_IMAGE:
        switch ( to_format )
        {
           case GAN_GREY_LEVEL_IMAGE:       test_reversal = GAN_FALSE; break;
           case GAN_GREY_LEVEL_ALPHA_IMAGE: test_reversal = GAN_FALSE; break;
           case GAN_RGB_COLOUR_IMAGE:       test_reversal = GAN_FALSE; break;
           case GAN_RGB_COLOUR_ALPHA_IMAGE: test_reversal = GAN_TRUE;  break;
           default: cu_assert(0);
        }
        break;

      default: cu_assert(0);
   }

   /* read number of bits of precision in pixel range */
   cu_assert ( gan_no_bits_for_type ( from_type, &from_bits ) );
   cu_assert ( gan_no_bits_for_type ( to_type,   &to_bits   ) );

   /* compute precision */
   from_precision = 1.5/((double)(1 << from_bits));
   to_precision   = 1.5/((double)(1 <<   to_bits));
   precision = gan_max2_d ( from_precision, to_precision );

   if ( test_reversal )
   {
      /* confirm test of reversed conversion using type */
      if ( to_bits < from_bits )
         test_reversal = GAN_FALSE;
   }

   /* reset format and type of from image */
   from_image = gan_image_set_format_type_dims ( from_image, from_format,
                                                 from_type, 1, 1 );
   cu_assert ( from_image != NULL );

   /* build array of pixels to be converted */
   for ( i = NO_TESTS-1; i >= 0; i-- )
   {
      cu_assert ( fill_pixel ( from_format, from_type, i, &from_pixel ) );

      /* convert pixel in two ways */
      cu_assert ( gan_image_convert_pixel_q ( &from_pixel, to_format,
                                              GAN_DOUBLE,
                                              &test_pixel ) );
      cu_assert ( gan_image_convert_pixel_i ( &test_pixel,
                                              GAN_RGB_COLOUR_ALPHA_IMAGE,
                                              GAN_DOUBLE ) );
      cu_assert ( gan_image_convert_pixel_q ( &from_pixel, to_format,
                                              to_type, &to_pixel ) );
      cu_assert ( gan_image_convert_pixel_i ( &to_pixel,
                                              GAN_RGB_COLOUR_ALPHA_IMAGE,
                                              GAN_DOUBLE ) );

      /* compare results */
      cu_assert ( bComparePixels ( &to_pixel, &test_pixel, precision ) );

      /* compare with precompiled values */
      cu_assert ( fill_pixel ( from_format, GAN_DOUBLE, i, &test_pixel ));
      cu_assert ( gan_image_convert_pixel_i ( &test_pixel, to_format,
                                              GAN_DOUBLE ) );
      cu_assert ( gan_image_convert_pixel_i ( &test_pixel,
                                              GAN_RGB_COLOUR_ALPHA_IMAGE,
                                              GAN_DOUBLE ) );
      cu_assert ( bComparePixels ( &to_pixel, &test_pixel, precision ) );
      
      /* build pixel in image and convert image */
      cu_assert ( gan_image_set_pix ( from_image, 0, 0, &from_pixel ) );
      cu_assert ( gan_image_convert_q ( from_image, to_format, to_type,
                                        to_image ) != NULL );
      to_pixel = gan_image_get_pix ( to_image, 0, 0 );
      cu_assert ( gan_image_convert_pixel_i ( &to_pixel,
                                              GAN_RGB_COLOUR_ALPHA_IMAGE,
                                              GAN_DOUBLE ) );

      /* compare results */
      cu_assert ( bComparePixels ( &to_pixel, &test_pixel, precision ) );
   }
   
   return GAN_TRUE;
}

static Gan_Bool
 test_conversion3 ( Gan_ImageFormat from_format, Gan_Type from_type,
                    Gan_ImageFormat to_format )
{
#if 0
   cu_assert ( test_conversion4 ( from_format, from_type, to_format, GAN_UINT8 ) );
   cu_assert ( test_conversion4 ( from_format, from_type, to_format, GAN_UINT16 ) );
   cu_assert ( test_conversion4 ( from_format, from_type, to_format, GAN_UINT32 ) );
#endif
   cu_assert ( test_conversion4 ( from_format, from_type, to_format, GAN_FLOAT ) );
   cu_assert ( test_conversion4 ( from_format, from_type, to_format, GAN_DOUBLE ) );
   return GAN_TRUE;
}
                         
static Gan_Bool
 test_conversion2 ( Gan_ImageFormat from_format, Gan_ImageFormat to_format )
{
   cu_assert ( test_conversion3 ( from_format, GAN_UINT8,  to_format ) );
   cu_assert ( test_conversion3 ( from_format, GAN_UINT16, to_format ) );
   cu_assert ( test_conversion3 ( from_format, GAN_UINT32, to_format ) );
   cu_assert ( test_conversion3 ( from_format, GAN_FLOAT,  to_format ) );
   cu_assert ( test_conversion3 ( from_format, GAN_DOUBLE, to_format ) );
   return GAN_TRUE;
}
                         
static Gan_Bool test_conversion ( Gan_ImageFormat format )
{
   cu_assert ( test_conversion2 ( format, GAN_GREY_LEVEL_IMAGE ) );
   cu_assert ( test_conversion2 ( format, GAN_GREY_LEVEL_ALPHA_IMAGE ) );
   cu_assert ( test_conversion2 ( format, GAN_RGB_COLOUR_IMAGE ) );
   cu_assert ( test_conversion2 ( format, GAN_RGB_COLOUR_ALPHA_IMAGE ) );
   return GAN_TRUE;
}

/* test pixel pointer stuff */
static Gan_Bool
 test_pixel_pointer(void)
{
   Gan_Image *img;

   {
      /* test pointer images */
      void **ptr, ***arr;

      img = gan_image_alloc_p ( 10, 12 );
      cu_assert ( img != NULL );
      ptr = gan_image_get_pixptr_p ( img, 3, 4 );
      cu_assert ( ptr == &img->row_data.gl.p[3][4] );
      arr = gan_image_get_pixarr_p ( img );
      cu_assert ( arr == img->row_data.gl.p );
      gan_image_free ( img );
   }
   
   {
      /* test grey-level 8-bit unsigned integer images */
      gan_uint8 *ptr, **arr;

      img = gan_image_alloc_gl_ui8 ( 10, 12 );
      cu_assert ( img != NULL );
      ptr = gan_image_get_pixptr_gl_ui8 ( img, 3, 4 );
      arr = gan_image_get_pixarr_gl_ui8 ( img );
      gan_image_free ( img );
   }
   
   {
      /* test grey-level 16-bit unsigned integer images */
      gan_uint16 *ptr, **arr;

      img = gan_image_alloc_gl_ui16 ( 10, 12 );
      cu_assert ( img != NULL );
      ptr = gan_image_get_pixptr_gl_ui16 ( img, 3, 4 );
      arr = gan_image_get_pixarr_gl_ui16 ( img );
      gan_image_free ( img );
   }
   
   {
      /* test grey-level 32-bit unsigned integer images */
      gan_uint32 *ptr, **arr;

      img = gan_image_alloc_gl_ui32 ( 10, 12 );
      cu_assert ( img != NULL );
      ptr = gan_image_get_pixptr_gl_ui32 ( img, 3, 4 );
      arr = gan_image_get_pixarr_gl_ui32 ( img );
      gan_image_free ( img );
   }
   
   {
      /* test grey-level unsigned character images */
      unsigned char *ptr, **arr;

      img = gan_image_alloc_gl_uc ( 10, 12 );
      cu_assert ( img != NULL );
      ptr = gan_image_get_pixptr_gl_uc ( img, 3, 4 );
      cu_assert ( ptr == &img->row_data.gl.uc[3][4] );
      arr = gan_image_get_pixarr_gl_uc ( img );
      cu_assert ( arr == img->row_data.gl.uc );
      gan_image_free ( img );
   }
   
   {
      /* test grey-level short images */
      short *ptr, **arr;

      img = gan_image_alloc_gl_s ( 10, 12 );
      cu_assert ( img != NULL );
      ptr = gan_image_get_pixptr_gl_s ( img, 3, 4 );
      cu_assert ( ptr == &img->row_data.gl.s[3][4] );
      arr = gan_image_get_pixarr_gl_s ( img );
      cu_assert ( arr == img->row_data.gl.s );
      gan_image_free ( img );
   }

   {
      /* test grey-level unsigned short images */
      unsigned short *ptr, **arr;

      img = gan_image_alloc_gl_us ( 10, 12 );
      cu_assert ( img != NULL );
      ptr = gan_image_get_pixptr_gl_us ( img, 3, 4 );
      cu_assert ( ptr == &img->row_data.gl.us[3][4] );
      arr = gan_image_get_pixarr_gl_us ( img );
      cu_assert ( arr == img->row_data.gl.us );
      gan_image_free ( img );
   }

   {
      /* test grey-level integer images */
      int *ptr, **arr;

      img = gan_image_alloc_gl_i ( 10, 12 );
      cu_assert ( img != NULL );
      ptr = gan_image_get_pixptr_gl_i ( img, 3, 4 );
      cu_assert ( ptr == &img->row_data.gl.i[3][4] );
      arr = gan_image_get_pixarr_gl_i ( img );
      cu_assert ( arr == img->row_data.gl.i );
      gan_image_free ( img );
   }

   {
      /* test grey-level unsigned int images */
      unsigned int *ptr, **arr;

      img = gan_image_alloc_gl_ui ( 10, 12 );
      cu_assert ( img != NULL );
      ptr = gan_image_get_pixptr_gl_ui ( img, 3, 4 );
      cu_assert ( ptr == &img->row_data.gl.ui[3][4] );
      arr = gan_image_get_pixarr_gl_ui ( img );
      cu_assert ( arr == img->row_data.gl.ui );
      gan_image_free ( img );
   }
   
   {
      /* test grey-level float images */
      float *ptr, **arr;

      img = gan_image_alloc_gl_f ( 10, 12 );
      cu_assert ( img != NULL );
      ptr = gan_image_get_pixptr_gl_f ( img, 3, 4 );
      cu_assert ( ptr == &img->row_data.gl.f[3][4] );
      arr = gan_image_get_pixarr_gl_f ( img );
      cu_assert ( arr == img->row_data.gl.f );
      gan_image_free ( img );
   }
   
   {
      /* test grey-level double images */
      double *ptr, **arr;

      img = gan_image_alloc_gl_d ( 10, 12 );
      cu_assert ( img != NULL );
      ptr = gan_image_get_pixptr_gl_d ( img, 3, 4 );
      cu_assert ( ptr == &img->row_data.gl.d[3][4] );
      arr = gan_image_get_pixarr_gl_d ( img );
      cu_assert ( arr == img->row_data.gl.d );
      gan_image_free ( img );
   }
   
   {
      /* test grey level/alpha 8-bit unsigned integer images */
      Gan_GLAPixel_ui8 *ptr, **arr;

      img = gan_image_alloc_gla_ui8 ( 10, 12 );
      cu_assert ( img != NULL );
      ptr = gan_image_get_pixptr_gla_ui8 ( img, 3, 4 );
      arr = gan_image_get_pixarr_gla_ui8 ( img );
      gan_image_free ( img );
   }

   {
      /* test grey level/alpha 16-bit unsigned integer images */
      Gan_GLAPixel_ui16 *ptr, **arr;

      img = gan_image_alloc_gla_ui16 ( 10, 12 );
      cu_assert ( img != NULL );
      ptr = gan_image_get_pixptr_gla_ui16 ( img, 3, 4 );
      arr = gan_image_get_pixarr_gla_ui16 ( img );
      gan_image_free ( img );
   }

   {
      /* test grey level/alpha 32-bit unsigned integer images */
      Gan_GLAPixel_ui32 *ptr, **arr;

      img = gan_image_alloc_gla_ui32 ( 10, 12 );
      cu_assert ( img != NULL );
      ptr = gan_image_get_pixptr_gla_ui32 ( img, 3, 4 );
      arr = gan_image_get_pixarr_gla_ui32 ( img );
      gan_image_free ( img );
   }

   {
      /* test grey level/alpha unsigned character images */
      Gan_GLAPixel_uc *ptr, **arr;

      img = gan_image_alloc_gla_uc ( 10, 12 );
      cu_assert ( img != NULL );
      ptr = gan_image_get_pixptr_gla_uc ( img, 3, 4 );
      cu_assert ( ptr == &img->row_data.gla.uc[3][4] );
      arr = gan_image_get_pixarr_gla_uc ( img );
      cu_assert ( arr == img->row_data.gla.uc );
      gan_image_free ( img );
   }

   {
      /* test grey level/alpha short images */
      Gan_GLAPixel_s *ptr, **arr;

      img = gan_image_alloc_gla_s ( 10, 12 );
      cu_assert ( img != NULL );
      ptr = gan_image_get_pixptr_gla_s ( img, 3, 4 );
      cu_assert ( ptr == &img->row_data.gla.s[3][4] );
      arr = gan_image_get_pixarr_gla_s ( img );
      cu_assert ( arr == img->row_data.gla.s );
      gan_image_free ( img );
   }

   {
      /* test grey level/alpha unsigned short images */
      Gan_GLAPixel_us *ptr, **arr;

      img = gan_image_alloc_gla_us ( 10, 12 );
      cu_assert ( img != NULL );
      ptr = gan_image_get_pixptr_gla_us ( img, 3, 4 );
      cu_assert ( ptr == &img->row_data.gla.us[3][4] );
      arr = gan_image_get_pixarr_gla_us ( img );
      cu_assert ( arr == img->row_data.gla.us );
      gan_image_free ( img );
   }

   {
      /* test grey level/alpha integer images */
      Gan_GLAPixel_i *ptr, **arr;

      img = gan_image_alloc_gla_i ( 10, 12 );
      cu_assert ( img != NULL );
      ptr = gan_image_get_pixptr_gla_i ( img, 3, 4 );
      cu_assert ( ptr == &img->row_data.gla.i[3][4] );
      arr = gan_image_get_pixarr_gla_i ( img );
      cu_assert ( arr == img->row_data.gla.i );
      gan_image_free ( img );
   }

   {
      /* test grey level/alpha unsigned int images */
      Gan_GLAPixel_ui *ptr, **arr;

      img = gan_image_alloc_gla_ui ( 10, 12 );
      cu_assert ( img != NULL );
      ptr = gan_image_get_pixptr_gla_ui ( img, 3, 4 );
      cu_assert ( ptr == &img->row_data.gla.ui[3][4] );
      arr = gan_image_get_pixarr_gla_ui ( img );
      cu_assert ( arr == img->row_data.gla.ui );
      gan_image_free ( img );
   }
   
   {
      /* test grey level/alpha float images */
      Gan_GLAPixel_f *ptr, **arr;

      img = gan_image_alloc_gla_f ( 10, 12 );
      cu_assert ( img != NULL );
      ptr = gan_image_get_pixptr_gla_f ( img, 3, 4 );
      cu_assert ( ptr == &img->row_data.gla.f[3][4] );
      arr = gan_image_get_pixarr_gla_f ( img );
      cu_assert ( arr == img->row_data.gla.f );
      gan_image_free ( img );
   }
   
   {
      /* test grey level/alpha double images */
      Gan_GLAPixel_d *ptr, **arr;

      img = gan_image_alloc_gla_d ( 10, 12 );
      cu_assert ( img != NULL );
      ptr = gan_image_get_pixptr_gla_d ( img, 3, 4 );
      cu_assert ( ptr == &img->row_data.gla.d[3][4] );
      arr = gan_image_get_pixarr_gla_d ( img );
      cu_assert ( arr == img->row_data.gla.d );
      gan_image_free ( img );
   }
   
   {
      /* test RGB colour 8-bit unsigned integer images */
      Gan_RGBPixel_ui8 *ptr, **arr;

      img = gan_image_alloc_rgb_ui8 ( 10, 12 );
      cu_assert ( img != NULL );
      ptr = gan_image_get_pixptr_rgb_ui8 ( img, 3, 4 );
      arr = gan_image_get_pixarr_rgb_ui8 ( img );
      gan_image_free ( img );
   }
   
   {
      /* test RGB colour 16-bit unsigned integer images */
      Gan_RGBPixel_ui16 *ptr, **arr;

      img = gan_image_alloc_rgb_ui16 ( 10, 12 );
      cu_assert ( img != NULL );
      ptr = gan_image_get_pixptr_rgb_ui16 ( img, 3, 4 );
      arr = gan_image_get_pixarr_rgb_ui16 ( img );
      gan_image_free ( img );
   }
   
   {
      /* test RGB colour 32-bit unsigned integer images */
      Gan_RGBPixel_ui32 *ptr, **arr;

      img = gan_image_alloc_rgb_ui32 ( 10, 12 );
      cu_assert ( img != NULL );
      ptr = gan_image_get_pixptr_rgb_ui32 ( img, 3, 4 );
      arr = gan_image_get_pixarr_rgb_ui32 ( img );
      gan_image_free ( img );
   }
   
   {
      /* test RGB colour unsigned character images */
      Gan_RGBPixel_uc *ptr, **arr;

      img = gan_image_alloc_rgb_uc ( 10, 12 );
      cu_assert ( img != NULL );
      ptr = gan_image_get_pixptr_rgb_uc ( img, 3, 4 );
      cu_assert ( ptr == &img->row_data.rgb.uc[3][4] );
      arr = gan_image_get_pixarr_rgb_uc ( img );
      cu_assert ( arr == img->row_data.rgb.uc );
      gan_image_free ( img );
   }
   
   {
      /* test RGB colour short images */
      Gan_RGBPixel_s *ptr, **arr;

      img = gan_image_alloc_rgb_s ( 10, 12 );
      cu_assert ( img != NULL );
      ptr = gan_image_get_pixptr_rgb_s ( img, 3, 4 );
      cu_assert ( ptr == &img->row_data.rgb.s[3][4] );
      arr = gan_image_get_pixarr_rgb_s ( img );
      cu_assert ( arr == img->row_data.rgb.s );
      gan_image_free ( img );
   }

   {
      /* test RGB colour unsigned short images */
      Gan_RGBPixel_us *ptr, **arr;

      img = gan_image_alloc_rgb_us ( 10, 12 );
      cu_assert ( img != NULL );
      ptr = gan_image_get_pixptr_rgb_us ( img, 3, 4 );
      cu_assert ( ptr == &img->row_data.rgb.us[3][4] );
      arr = gan_image_get_pixarr_rgb_us ( img );
      cu_assert ( arr == img->row_data.rgb.us );
      gan_image_free ( img );
   }

   {
      /* test RGB colour integer images */
      Gan_RGBPixel_i *ptr, **arr;

      img = gan_image_alloc_rgb_i ( 10, 12 );
      cu_assert ( img != NULL );
      ptr = gan_image_get_pixptr_rgb_i ( img, 3, 4 );
      cu_assert ( ptr == &img->row_data.rgb.i[3][4] );
      arr = gan_image_get_pixarr_rgb_i ( img );
      cu_assert ( arr == img->row_data.rgb.i );
      gan_image_free ( img );
   }

   {
      /* test RGB colour unsigned int images */
      Gan_RGBPixel_ui *ptr, **arr;

      img = gan_image_alloc_rgb_ui ( 10, 12 );
      cu_assert ( img != NULL );
      ptr = gan_image_get_pixptr_rgb_ui ( img, 3, 4 );
      cu_assert ( ptr == &img->row_data.rgb.ui[3][4] );
      arr = gan_image_get_pixarr_rgb_ui ( img );
      cu_assert ( arr == img->row_data.rgb.ui );
      gan_image_free ( img );
   }
   
   {
      /* test RGB colour float images */
      Gan_RGBPixel_f *ptr, **arr;

      img = gan_image_alloc_rgb_f ( 10, 12 );
      cu_assert ( img != NULL );
      ptr = gan_image_get_pixptr_rgb_f ( img, 3, 4 );
      cu_assert ( ptr == &img->row_data.rgb.f[3][4] );
      arr = gan_image_get_pixarr_rgb_f ( img );
      cu_assert ( arr == img->row_data.rgb.f );
      gan_image_free ( img );
   }
   
   {
      /* test RGB colour double images */
      Gan_RGBPixel_d *ptr, **arr;

      img = gan_image_alloc_rgb_d ( 10, 12 );
      cu_assert ( img != NULL );
      ptr = gan_image_get_pixptr_rgb_d ( img, 3, 4 );
      cu_assert ( ptr == &img->row_data.rgb.d[3][4] );
      arr = gan_image_get_pixarr_rgb_d ( img );
      cu_assert ( arr == img->row_data.rgb.d );
      gan_image_free ( img );
   }
   
   {
      /* test RGB-alpha colour 8-bit unsigned integer images */
      Gan_RGBAPixel_ui8 *ptr, **arr;

      img = gan_image_alloc_rgba_ui8 ( 10, 12 );
      cu_assert ( img != NULL );
      ptr = gan_image_get_pixptr_rgba_ui8 ( img, 3, 4 );
      arr = gan_image_get_pixarr_rgba_ui8 ( img );
      gan_image_free ( img );
   }
   
   {
      /* test RGB-alpha colour 16-bit unsigned integer images */
      Gan_RGBAPixel_ui16 *ptr, **arr;

      img = gan_image_alloc_rgba_ui16 ( 10, 12 );
      cu_assert ( img != NULL );
      ptr = gan_image_get_pixptr_rgba_ui16 ( img, 3, 4 );
      arr = gan_image_get_pixarr_rgba_ui16 ( img );
      gan_image_free ( img );
   }
   
   {
      /* test RGB-alpha colour 32-bit unsigned integer images */
      Gan_RGBAPixel_ui32 *ptr, **arr;

      img = gan_image_alloc_rgba_ui32 ( 10, 12 );
      cu_assert ( img != NULL );
      ptr = gan_image_get_pixptr_rgba_ui32 ( img, 3, 4 );
      arr = gan_image_get_pixarr_rgba_ui32 ( img );
      gan_image_free ( img );
   }
   
   {
      /* test RGB-alpha colour unsigned character images */
      Gan_RGBAPixel_uc *ptr, **arr;

      img = gan_image_alloc_rgba_uc ( 10, 12 );
      cu_assert ( img != NULL );
      ptr = gan_image_get_pixptr_rgba_uc ( img, 3, 4 );
      cu_assert ( ptr == &img->row_data.rgba.uc[3][4] );
      arr = gan_image_get_pixarr_rgba_uc ( img );
      cu_assert ( arr == img->row_data.rgba.uc );
      gan_image_free ( img );
   }
   
   {
      /* test RGB-alpha colour short images */
      Gan_RGBAPixel_s *ptr, **arr;

      img = gan_image_alloc_rgba_s ( 10, 12 );
      cu_assert ( img != NULL );
      ptr = gan_image_get_pixptr_rgba_s ( img, 3, 4 );
      cu_assert ( ptr == &img->row_data.rgba.s[3][4] );
      arr = gan_image_get_pixarr_rgba_s ( img );
      cu_assert ( arr == img->row_data.rgba.s );
      gan_image_free ( img );
   }

   {
      /* test RGB-alpha colour unsigned short images */
      Gan_RGBAPixel_us *ptr, **arr;

      img = gan_image_alloc_rgba_us ( 10, 12 );
      cu_assert ( img != NULL );
      ptr = gan_image_get_pixptr_rgba_us ( img, 3, 4 );
      cu_assert ( ptr == &img->row_data.rgba.us[3][4] );
      arr = gan_image_get_pixarr_rgba_us ( img );
      cu_assert ( arr == img->row_data.rgba.us );
      gan_image_free ( img );
   }

   {
      /* test RGB-alpha colour integer images */
      Gan_RGBAPixel_i *ptr, **arr;

      img = gan_image_alloc_rgba_i ( 10, 12 );
      cu_assert ( img != NULL );
      ptr = gan_image_get_pixptr_rgba_i ( img, 3, 4 );
      cu_assert ( ptr == &img->row_data.rgba.i[3][4] );
      arr = gan_image_get_pixarr_rgba_i ( img );
      cu_assert ( arr == img->row_data.rgba.i );
      gan_image_free ( img );
   }

   {
      /* test RGB-alpha colour unsigned int images */
      Gan_RGBAPixel_ui *ptr, **arr;

      img = gan_image_alloc_rgba_ui ( 10, 12 );
      cu_assert ( img != NULL );
      ptr = gan_image_get_pixptr_rgba_ui ( img, 3, 4 );
      cu_assert ( ptr == &img->row_data.rgba.ui[3][4] );
      arr = gan_image_get_pixarr_rgba_ui ( img );
      cu_assert ( arr == img->row_data.rgba.ui );
      gan_image_free ( img );
   }
   
   {
      /* test RGB-alpha colour float images */
      Gan_RGBAPixel_f *ptr, **arr;

      img = gan_image_alloc_rgba_f ( 10, 12 );
      cu_assert ( img != NULL );
      ptr = gan_image_get_pixptr_rgba_f ( img, 3, 4 );
      cu_assert ( ptr == &img->row_data.rgba.f[3][4] );
      arr = gan_image_get_pixarr_rgba_f ( img );
      cu_assert ( arr == img->row_data.rgba.f );
      gan_image_free ( img );
   }
   
   {
      /* test RGB-alpha colour double images */
      Gan_RGBAPixel_d *ptr, **arr;

      img = gan_image_alloc_rgba_d ( 10, 12 );
      cu_assert ( img != NULL );
      ptr = gan_image_get_pixptr_rgba_d ( img, 3, 4 );
      cu_assert ( ptr == &img->row_data.rgba.d[3][4] );
      arr = gan_image_get_pixarr_rgba_d ( img );
      cu_assert ( arr == img->row_data.rgba.d );
      gan_image_free ( img );
   }
   
   {
      /* test RGBX colour 8-bit images */
      Gan_RGBXPixel_ui8 *ptr, **arr;

      img = gan_image_alloc_rgbx_ui8 ( 10, 12 );
      cu_assert ( img != NULL );
      ptr = gan_image_get_pixptr_rgbx_ui8 ( img, 3, 4 );
      cu_assert ( ptr == &img->row_data.rgbx.ui8[3][4] );
      arr = gan_image_get_pixarr_rgbx_ui8 ( img );
      cu_assert ( arr == img->row_data.rgbx.ui8 );
      gan_image_free ( img );
   }

   {
      /* test RGBX colour 10-bit images */
      Gan_RGBXPixel_ui10 *ptr, **arr;

      img = gan_image_alloc_rgbx_ui10 ( 10, 12 );
      cu_assert ( img != NULL );
      ptr = gan_image_get_pixptr_rgbx_ui10 ( img, 3, 4 );
      cu_assert ( ptr == &img->row_data.rgbx.ui10[3][4] );
      arr = gan_image_get_pixarr_rgbx_ui10 ( img );
      cu_assert ( arr == img->row_data.rgbx.ui10 );
      gan_image_free ( img );
   }

   {
      /* test RGBA colour 10-bit images with small alpha channel*/
      Gan_RGBASPixel_ui10 *ptr, **arr;

      img = gan_image_alloc_rgbas_ui10 ( 10, 12 );
      cu_assert ( img != NULL );
      ptr = gan_image_get_pixptr_rgbas_ui10 ( img, 3, 4 );
      cu_assert ( ptr == &img->row_data.rgbas.ui10[3][4] );
      arr = gan_image_get_pixarr_rgbas_ui10 ( img );
      cu_assert ( arr == img->row_data.rgbas.ui10 );
      gan_image_free ( img );
   }

   {
      /* test RGBA colour 12-bit images */
      Gan_RGBAPixel_ui12 *ptr, **arr;

      img = gan_image_alloc_rgba_ui12 ( 10, 12 );
      cu_assert ( img != NULL );
      ptr = gan_image_get_pixptr_rgba_ui12 ( img, 3, 4 );
      cu_assert ( ptr == &img->row_data.rgba.ui12[3][4] );
      arr = gan_image_get_pixarr_rgba_ui12 ( img );
      cu_assert ( arr == img->row_data.rgba.ui12 );
      gan_image_free ( img );
   }

   {
      /* test YUV 4-4-4 padded 8-bit colour images */
      Gan_YUVX444Pixel_ui8 *ptr, **arr;

      img = gan_image_alloc_yuvx444_ui8 ( 10, 12 );
      cu_assert ( img != NULL );
      ptr = gan_image_get_pixptr_yuvx444_ui8 ( img, 3, 4 );
      cu_assert ( ptr == &img->row_data.yuvx444.ui8[3][4] );
      arr = gan_image_get_pixarr_yuvx444_ui8 ( img );
      cu_assert ( arr == img->row_data.yuvx444.ui8 );
      gan_image_free ( img );
   }

   {
      /* test YUVA 4-4-4 8-bit colour images */
      Gan_YUVA444Pixel_ui8 *ptr, **arr;

      img = gan_image_alloc_yuva444_ui8 ( 10, 12 );
      cu_assert ( img != NULL );
      ptr = gan_image_get_pixptr_yuva444_ui8 ( img, 3, 4 );
      cu_assert ( ptr == &img->row_data.yuva444.ui8[3][4] );
      arr = gan_image_get_pixarr_yuva444_ui8 ( img );
      cu_assert ( arr == img->row_data.yuva444.ui8 );
      gan_image_free ( img );
   }

   {
      /* test YUV 4-2-2 8-bit colour images */
      Gan_YUV422Pixel_ui8 *ptr, **arr;

      img = gan_image_alloc_yuv422_ui8 ( 10, 12 );
      cu_assert ( img != NULL );
      ptr = gan_image_get_pixptr_yuv422_ui8 ( img, 3, 4 );
      cu_assert ( ptr == &img->row_data.yuv422.ui8[3][4] );
      arr = gan_image_get_pixarr_yuv422_ui8 ( img );
      cu_assert ( arr == img->row_data.yuv422.ui8 );
      gan_image_free ( img );
   }

   {
      /* test grey-level 10-bit images */
      Gan_YXPixel_ui10 *ptr, **arr;

      img = gan_image_alloc_gl_ui10 ( 10, 12 );
      cu_assert ( img != NULL );
      ptr = gan_image_get_pixptr_gl_ui10 ( img, 3, 4 );
      cu_assert ( ptr == &img->row_data.gl.ui10[3][4] );
      arr = gan_image_get_pixarr_gl_ui10 ( img );
      cu_assert ( arr == img->row_data.gl.ui10 );
      gan_image_free ( img );
   }

   {
      /* test grey-level 12-bit images */
      Gan_YXPixel_ui12 *ptr, **arr;

      img = gan_image_alloc_gl_ui12 ( 10, 12 );
      cu_assert ( img != NULL );
      ptr = gan_image_get_pixptr_gl_ui12 ( img, 3, 4 );
      cu_assert ( ptr == &img->row_data.gl.ui12[3][4] );
      arr = gan_image_get_pixarr_gl_ui12 ( img );
      cu_assert ( arr == img->row_data.gl.ui12 );
      gan_image_free ( img );
   }

   return GAN_TRUE;
}

static Gan_Bool setup_test(void)
{
   printf("\nSetup for image_test completed!\n\n");
   return GAN_TRUE;
}

static Gan_Bool teardown_test(void)
{
   printf("\nTeardown for image_test completed!\n\n");
   return GAN_TRUE;
}

/* Runs the image test functions */
static Gan_Bool run_test(void)
{
   Gan_Image *img, *img2, *img3=NULL;
   int pix[12], i, j, count, it;

   if(img3!=NULL)gan_image_write("",GAN_PNG_FORMAT,NULL,NULL);

   /* build image */
   for ( i = 11; i >= 0; i-- )
      pix[i] = i;

   img = gan_image_form_data_gl_i ( NULL, 3, 4, 4*sizeof(int),
                                    pix, 12*sizeof(int), NULL, 0 );
   cu_assert ( img != NULL );

   /* check pixel values */
   for ( i = (int)img->height-1, count=11; i >= 0; i-- )
      for ( j = (int)img->width-1; j >= 0; j--, count-- )
         cu_assert ( gan_image_get_pix_gl_i(img,i,j) == count );

   gan_image_free ( img );

   /* build new image */
   img = gan_image_alloc_gl_uc ( 5, 3 );
   cu_assert ( img != NULL );

   /* fill image with constant pixel value */
   cu_assert ( gan_image_fill_const_gl_uc ( img, 100 ) );

   /* set specific pixel and check value */
   cu_assert ( gan_image_set_pix_gl_uc ( img, 2, 1, 150 ) );
   cu_assert ( gan_image_get_pix_gl_uc ( img, 2, 1 ) == 150 );

   /* check that pixel pointer functions work */
   cu_assert ( *(gan_image_get_pixptr_gl_uc ( img, 2, 1 )) == 150 );

   /* reset image dimensions. The new dimensions are larger so there will
    * be reallocation of the pixel and row pointer arrays inside the
    * function. Here we set stride = width.
    */
   img = gan_image_set_gl_uc ( img, 7, 9 );
   cu_assert ( img != NULL );
        
   /* fill image with linear ramp */
   cu_assert ( ramp_image ( img ) );
   for ( i = (int)img->height-1; i >= 0; i-- )
      for ( j = (int)img->width-1; j >= 0; j-- )
         cu_assert ( gan_image_get_pix_gl_uc(img,i,j) == i+j );
        
   /* copy image. In Gandalf, the functions with a "_s" suffix are `slow'
    * functions which create a new object for the result
    */
   img2 = gan_image_copy_s(img);
   cu_assert ( img2 != NULL );
   for ( i = (int)img2->height-1; i >= 0; i-- )
      for ( j = (int)img2->width-1; j >= 0; j-- )
         cu_assert ( gan_image_get_pix_gl_uc(img2,i,j) == i+j );

   /* copy image, flipping it vertically along the way
    */
   img2 = gan_image_flip_q ( img, GAN_FALSE, img2 );
   cu_assert ( img2 != NULL );
   for ( i = (int)img2->height-1; i >= 0; i-- )
      for ( j = (int)img2->width-1; j >= 0; j-- )
         cu_assert ( gan_image_get_pix_gl_uc(img2,(int)img2->height-1-i,j)
                     == i+j );

   /* modify image and copy it back. Functions with a "_q" suffix are `quick'
    * functions which copy the result into a pre-existing object.
    */
   cu_assert ( gan_image_fill_const_gl_uc ( img2, 254 ) );
   for ( i = (int)img2->height-1; i >= 0; i-- )
      for ( j = (int)img2->width-1; j >= 0; j-- )
         cu_assert ( gan_image_get_pix_gl_uc(img2,i,j) == 254 );

   img = gan_image_copy_q ( img2, img );
   cu_assert ( img != NULL );
   for ( i = (int)img->height-1; i >= 0; i-- )
      for ( j = (int)img->width-1; j >= 0; j-- )
         cu_assert ( gan_image_get_pix_gl_uc(img,i,j) == 254 );
        
   /* convert img2 to unsigned integer type */
   img2 = gan_image_set_type ( img2, GAN_UINT );
   cu_assert ( img2 != NULL );
        
   /* fill with ramp for new image type */
   cu_assert ( ramp_image ( img2 ) );
   for ( i = (int)img2->height-1; i >= 0; i-- )
      for ( j = (int)img2->width-1; j >= 0; j-- )
         cu_assert ( gan_image_get_pix_gl_ui(img2,i,j) == i+j );
        
   /* convert img to RGB colour */
   img = gan_image_set_format_type ( img, GAN_RGB_COLOUR_IMAGE, GAN_UCHAR );
   cu_assert ( img != NULL );
        
   /* fill with ramp for new image format and type */
   cu_assert ( ramp_image ( img ) );
   for ( i = (int)img->height-1; i >= 0; i-- )
      for ( j = (int)img->width-1; j >= 0; j-- )
      {
         Gan_RGBPixel_uc pixel;

         pixel = gan_image_get_pix_rgb_uc(img,i,j);
         cu_assert ( pixel.R == i+j && pixel.G == i+j && pixel.B == i+j );
      }

   /* fill sub-region with zero */
   cu_assert ( gan_image_fill_zero_window ( img, 0, 0,
                                            img->height/2, img->width/2 ) );
   for ( i = (int)img->height-1; i >= 0; i-- )
      for ( j = (int)img->width-1; j >= 0; j-- )
      {
         Gan_RGBPixel_uc pixel;

         pixel = gan_image_get_pix_rgb_uc(img,i,j);
         if ( i < img->height/2 && j < img->width/2 )
         {
            cu_assert ( pixel.R == 0   && pixel.G == 0   && pixel.B == 0 );
         }
         else
            cu_assert ( pixel.R == i+j && pixel.G == i+j && pixel.B == i+j );
      }
   
   /* test exclusive-OR operation on binary images */
   img = gan_image_set_b ( img, img->height, img->width );
   img2 = gan_image_set_b ( img2, img->height, img->width );
   cu_assert ( img != NULL && img2 != NULL );

   /* fill input images with random bits */
   for ( i = (int)img->height-1; i >= 0; i-- )
      for ( j = (int)img->width-1; j >= 0; j-- )
      {
         gan_image_set_pix_b ( img, i, j,
                         ((gan_random() & 256) < 128) ? GAN_FALSE : GAN_TRUE );
         gan_image_set_pix_b ( img2, i, j,
                         ((gan_random() & 256) < 128) ? GAN_FALSE : GAN_TRUE );
      }

   /* perform OR operation */
   img3 = gan_image_copy_s(img);
   cu_assert(img3 != NULL);
   cu_assert ( gan_image_bit_or_i ( img3, img2 ) );

   /* test result */
   for ( i = (int)img->height-1; i >= 0; i-- )
      for ( j = (int)img->width-1; j >= 0; j-- )
      {
         Gan_Bool bit, bit2, bit3;

         bit = gan_image_get_pix_b ( img, i, j );
         bit2 = gan_image_get_pix_b ( img2, i, j );
         bit3 = gan_image_get_pix_b ( img3, i, j );
         cu_assert ( (bit || bit2) ? bit3 : !bit3 );
      }
   
   /* perform exclusive-OR operation */
   img3 = gan_image_copy_q(img,img3);
   cu_assert(img3 != NULL);
   cu_assert ( gan_image_bit_eor_i ( img3, img2 ) );

   /* test result */
   for ( i = (int)img->height-1; i >= 0; i-- )
      for ( j = (int)img->width-1; j >= 0; j-- )
      {
         Gan_Bool bit, bit2, bit3;

         bit = gan_image_get_pix_b ( img, i, j );
         bit2 = gan_image_get_pix_b ( img2, i, j );
         bit3 = gan_image_get_pix_b ( img3, i, j );
         cu_assert ( ((bit && bit2) ? !bit3 :
                      ((bit && !bit2) ? bit3 :
                       ((!bit && bit2) ? bit3 : !bit3))) );
      }
   
   /* perform AND operation */
   img3 = gan_image_copy_q(img,img3);
   cu_assert(img3 != NULL);
   cu_assert ( gan_image_bit_and_i ( img3, img2 ) );

   /* test result */
   for ( i = (int)img->height-1; i >= 0; i-- )
      for ( j = (int)img->width-1; j >= 0; j-- )
      {
         Gan_Bool bit, bit2, bit3;

         bit = gan_image_get_pix_b ( img, i, j );
         bit2 = gan_image_get_pix_b ( img2, i, j );
         bit3 = gan_image_get_pix_b ( img3, i, j );
         cu_assert ( (bit && bit2) ? bit3 : !bit3 );
      }
   
   /* perform NAND operation */
   img3 = gan_image_copy_q(img,img3);
   cu_assert(img3 != NULL);
   cu_assert ( gan_image_bit_nand_i ( img3, img2 ) );

   /* test result */
   for ( i = (int)img->height-1; i >= 0; i-- )
      for ( j = (int)img->width-1; j >= 0; j-- )
      {
         Gan_Bool bit, bit2, bit3;

         bit = gan_image_get_pix_b ( img, i, j );
         bit2 = gan_image_get_pix_b ( img2, i, j );
         bit3 = gan_image_get_pix_b ( img3, i, j );
         cu_assert ( (bit && bit2) ? !bit3 : bit3 );
      }
   
   /* perform AND-NOT operation */
   img3 = gan_image_copy_q(img,img3);
   cu_assert(img3 != NULL);
   cu_assert ( gan_image_bit_andnot_i ( img3, img2 ) );

   /* test result */
   for ( i = (int)img->height-1; i >= 0; i-- )
      for ( j = (int)img->width-1; j >= 0; j-- )
      {
         Gan_Bool bit, bit2, bit3;

         bit = gan_image_get_pix_b ( img, i, j );
         bit2 = gan_image_get_pix_b ( img2, i, j );
         bit3 = gan_image_get_pix_b ( img3, i, j );
         cu_assert ( (bit && !bit2) ? bit3 : !bit3 );
      }
   
   /* perform inversion operation */
   img3 = gan_image_bit_invert_q(img,img3);
   cu_assert(img3 != NULL);

   /* test result */
   for ( i = (int)img->height-1; i >= 0; i-- )
      for ( j = (int)img->width-1; j >= 0; j-- )
      {
         Gan_Bool bit, bit3;

         bit = gan_image_get_pix_b ( img, i, j );
         bit3 = gan_image_get_pix_b ( img3, i, j );
         cu_assert ( bit ? !bit3 : bit3 );
      }

   /* test binary image active region function */
   for ( it = 0; it < 100; it++ )
   {
      Gan_ImageWindow subwin;
      int xmin, xmax, ymin, ymax;
      unsigned width = 1+(gan_random()%256), height = 1+(gan_random()%256);
      unsigned row, col;
      
      img = gan_image_set_b ( img, height, width );
      cu_assert ( img != NULL );

      /* build bit image with a random number of bits */
      gan_image_fill_zero ( img );
      for ( count = gan_random()%32; count >= 0; count-- )
      {
         row = gan_random()%img->height;
         col = gan_random()%img->width;
         cu_assert ( gan_image_set_pix_b ( img, row, col, GAN_TRUE ) );
      }

      /* compute the active region the hard way */
      xmin = ymin = INT_MAX;
      xmax = ymax = INT_MIN;
      for ( i = (int)img->height-1; i >= 0; i-- )
         for ( j = (int)img->width-1; j >= 0; j-- )
            if ( gan_image_get_pix_b(img,i,j) )
            {
               if ( i > ymax ) ymax = i;
               if ( i < ymin ) ymin = i;
               if ( j > xmax ) xmax = j;
               if ( j < xmin ) xmin = j;
            }

      if ( xmin == INT_MAX ) continue;

      /* compute word-aligned region and compare */
      cu_assert ( gan_image_get_active_subwindow_b ( img, GAN_WORD_ALIGNMENT,
                                                    &subwin ) );
      cu_assert ( subwin.c0 == xmin - (xmin % GAN_BITWORD_SIZE) &&
                  subwin.width == gan_min2_ui ( xmax - (xmax%GAN_BITWORD_SIZE)
                                                + GAN_BITWORD_SIZE - subwin.c0,
                                                img->width-subwin.c0 ) );
      cu_assert ( subwin.r0 == ymin && subwin.height == ymax - ymin + 1 );

      /* compute byte-aligned region and compare */
      cu_assert ( gan_image_get_active_subwindow_b ( img, GAN_BYTE_ALIGNMENT,
                                                    &subwin ) );
      cu_assert ( subwin.c0 == xmin - (xmin % (SIZEOF_CHAR*8)) &&
                  subwin.width == gan_min2_ui ( xmax - (xmax%(SIZEOF_CHAR*8))
                                                + SIZEOF_CHAR*8 - subwin.c0,
                                                img->width-subwin.c0 ) );
      cu_assert ( subwin.r0 == ymin && subwin.height == ymax - ymin + 1 );

      /* compute bit-aligned region and compare */
      cu_assert ( gan_image_get_active_subwindow_b ( img, GAN_BIT_ALIGNMENT,
                                                    &subwin ) );
      cu_assert ( subwin.c0 == xmin && subwin.width  == xmax - xmin + 1 );
      cu_assert ( subwin.r0 == ymin && subwin.height == ymax - ymin + 1 );
   }

   /* test binary image dilation */
   for ( it = 0; it < 100; it++ )
   {
      unsigned width = 1+(gan_random()%256), height = 1+(gan_random()%256);
      int hdilate = 1+(gan_random()%7), vdilate = 1+(gan_random()%7);
      unsigned row, col;
      
      img  = gan_image_set_b ( img, height, width );
      cu_assert ( img != NULL );

      /* build bit image with a random number of bits */
      gan_image_fill_zero ( img );
      for ( count = gan_random()%32; count >= 0; count-- )
      {
         row = gan_random()%img->height;
         col = gan_random()%img->width;
         cu_assert ( gan_image_set_pix_b ( img, row, col, GAN_TRUE ) );
      }

      cu_assert ( gan_image_copy_q ( img, img2 ) != NULL );
      cu_assert ( gan_image_bit_dilate_horiz ( img, hdilate, NULL ) );
      cu_assert ( image_bit_dilate_horiz_safe ( img2, hdilate, NULL ) );
      cu_assert ( gan_image_bit_dilate_vert ( img, vdilate, NULL ) );
      cu_assert ( image_bit_dilate_vert_safe ( img2, vdilate, NULL ) );

      /* test result */
      for ( i = (int)img->height-1; i >= 0; i-- )
         for ( j = (int)img->width-1; j >= 0; j-- )
         {
            Gan_Bool bit, bit2;

            bit = gan_image_get_pix_b ( img, i, j );
            bit2 = gan_image_get_pix_b ( img2, i, j );
            cu_assert ( bit == bit2 );
         }

      /* build bit image with a random number of bits */
      gan_image_fill_zero ( img );
      for ( count = gan_random()%32; count >= 0; count-- )
      {
         row = gan_random()%img->height;
         col = gan_random()%img->width;
         cu_assert ( gan_image_set_pix_b ( img, row, col, GAN_TRUE ) );
      }

      /* build restriction mask */
      img3 = gan_image_set_b ( img3, height, width );
      cu_assert ( img3 != NULL );

      /* build bit image with a random number of bits */
      gan_image_fill_zero ( img3 );
      for ( count = gan_random()%1000; count >= 0; count-- )
      {
         row = gan_random()%img3->height;
         col = gan_random()%img3->width;
         cu_assert ( gan_image_set_pix_b ( img3, row, col, GAN_TRUE ) );
      }

      cu_assert ( gan_image_copy_q ( img, img2 ) != NULL );
      cu_assert ( gan_image_bit_dilate_horiz ( img, hdilate, img3 ) );
      cu_assert ( image_bit_dilate_horiz_safe ( img2, hdilate, img3 ) );
      cu_assert ( gan_image_bit_dilate_vert ( img, vdilate, img3 ) );
      cu_assert ( image_bit_dilate_vert_safe ( img2, vdilate, img3 ) );

      /* test result */
      for ( i = (int)img->height-1; i >= 0; i-- )
         for ( j = (int)img->width-1; j >= 0; j-- )
         {
            Gan_Bool bit, bit2;

            bit = gan_image_get_pix_b ( img, i, j );
            bit2 = gan_image_get_pix_b ( img2, i, j );
            cu_assert ( bit == bit2 );
         }
   }

   /* free all images and exit */
   gan_image_free_va ( img3, img2, img, NULL );

   /* test pixel/image conversions */
   from_image = gan_image_alloc ( GAN_GREY_LEVEL_IMAGE, GAN_UINT8, 1, 1 );
   to_image = gan_image_alloc ( GAN_GREY_LEVEL_IMAGE, GAN_UINT8, 1, 1 );
   cu_assert ( from_image != NULL && to_image != NULL );
   cu_assert ( test_conversion4 ( GAN_RGB_COLOUR_ALPHA_IMAGE, GAN_UCHAR,
                                  GAN_GREY_LEVEL_ALPHA_IMAGE, GAN_UCHAR ) );
   cu_assert ( test_conversion ( GAN_GREY_LEVEL_IMAGE ) );
   cu_assert ( test_conversion ( GAN_GREY_LEVEL_ALPHA_IMAGE ) );
   cu_assert ( test_conversion ( GAN_RGB_COLOUR_IMAGE ) );
   cu_assert ( test_conversion ( GAN_RGB_COLOUR_ALPHA_IMAGE ) );
   gan_image_free_va ( to_image, from_image, NULL );

   /* test pixel pointer stuff */
   cu_assert ( test_pixel_pointer() );

   return GAN_TRUE;
}

#ifdef IMAGE_TEST_MAIN

int main ( int argc, char *argv[] )
{
   /* set default Gandalf error handler without trace handling */
   gan_err_set_reporter ( gan_err_default_reporter );
   gan_err_set_trace ( GAN_ERR_TRACE_OFF );

   setup_test();
   if ( run_test() )
      printf ( "Tests ran successfully!\n" );
   else
      printf ( "At least one test failed\n" );

   teardown_test();
   gan_heap_report(NULL);
   return 0;
}

#else

/* This function registers the unit tests to a cUnit_test_suite. */
cUnit_test_suite *image_test_build_suite(void)
{
   cUnit_test_suite *sp;
        
   /* Get a new test session */
   sp = cUnit_new_test_suite("image_test suite");
        
   cUnit_add_test(sp, "image_test", run_test);
        
   /* register a setup and teardown on the test suite 'image_test' */
   if (cUnit_register_setup(sp, setup_test) != GAN_TRUE)
      printf("Error while setting up test suite image_test");
        
   if (cUnit_register_teardown(sp, teardown_test) != GAN_TRUE)
      printf("Error while tearing down test suite image_test");
        
   return( sp );
}

#endif /* #ifdef IMAGE_TEST_MAIN */

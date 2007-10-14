/**
 * File:          $RCSfile: image_extract.c,v $
 * Module:        Extract/convert part of an image
 * Part of:       Gandalf Library
 *
 * Revision:      $Revision: 1.1.2.1 $
 * Last edited:   $Date: 2007/10/14 02:20:19 $
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
#include <gandalf/image/image_extract.h>
#include <gandalf/image/image_gl_float32.h>
#include <gandalf/image/image_gl_float64.h>
#include <gandalf/image/image_bit.h>
#include <gandalf/image/image_gl_uint8.h>
#include <gandalf/image/image_gl_uint16.h>
#include <gandalf/image/image_gl_int32.h>
#include <gandalf/image/image_gl_uint32.h>
#include <gandalf/image/image_gla_uint8.h>
#include <gandalf/image/image_gla_uint16.h>
#include <gandalf/image/image_gla_int32.h>
#include <gandalf/image/image_gla_uint32.h>
#include <gandalf/image/image_gla_float32.h>
#include <gandalf/image/image_gla_float64.h>
#include <gandalf/image/image_rgb_uint8.h>
#include <gandalf/image/image_rgb_uint16.h>
#include <gandalf/image/image_rgb_int32.h>
#include <gandalf/image/image_rgb_uint32.h>
#include <gandalf/image/image_rgb_float64.h>
#include <gandalf/image/image_rgb_float32.h>
#include <gandalf/image/image_rgba_uint8.h>
#include <gandalf/image/image_rgba_uint16.h>
#include <gandalf/image/image_rgba_int32.h>
#include <gandalf/image/image_rgba_uint32.h>
#include <gandalf/image/image_rgba_float64.h>
#include <gandalf/image/image_rgba_float32.h>
#include <gandalf/image/image_vfield2D_float32.h>
#include <gandalf/common/misc_error.h>
#include <gandalf/common/array.h>
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

/**
 * \addtogroup ImagePackage
 * \{
 */

/**
 * \addtogroup ImageExtract
 * \{
 */

/* temporary definitions until we get the bit size stuff sorted out */
#define ui8 uc
#define ui16 us
#define ui32 ui
#define f32 f
#define f64 d
#define i32 i
#define ui32 ui

/* converts grey-level image from one type to another */
static Gan_Image *
 extract_grey ( const Gan_Image *source,
                unsigned r0s,    unsigned c0s,
                unsigned height, unsigned width,
                Gan_Bool copy_pixels,
                Gan_Image *dest, unsigned r0d, unsigned c0d )
{
   unsigned int r, c;

   gan_err_test_ptr ( source->format == GAN_GREY_LEVEL_IMAGE && dest->format == GAN_GREY_LEVEL_IMAGE,
                      "extract_grey", GAN_ERROR_INCOMPATIBLE, "" );

   switch ( dest->type )
   {
      case GAN_UINT8:
        switch ( source->type )
        {
           case GAN_UINT8:
             if ( copy_pixels )
                for ( r=0; r < height; r++ )
                   gan_copy_array_ui8 ( gan_image_get_pixptr_gl_ui8(source,r0s+r,c0s), 1, width,
                                        gan_image_get_pixptr_gl_ui8(dest,r0d+r,c0d), 1 );
             else
             {
                /* make destination image point into source image */
                gan_err_test_ptr ( r0d == 0 && c0d == 0, "extract_grey", GAN_ERROR_INCOMPATIBLE, "" );
                for ( r=0; r < height; r++ )
                   dest->row_data.gl.ui8[r] = gan_image_get_pixptr_gl_ui8(source,r0s+r,c0s);

                dest->pix_data_ptr = gan_image_get_pixptr_gl_ui8(source, r0s, c0s);
             }

             break;
                  
           case GAN_UINT16:
             gan_err_test_ptr ( copy_pixels, "extract_grey", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_ui8 ( dest, r0d+r, c0d+c,gan_pixel_ui16_to_ui8 ( gan_image_get_pix_gl_ui16(source,r0s+r,c0s+c) ) );

             break;
         
           case GAN_UINT32:
             gan_err_test_ptr ( copy_pixels, "extract_grey", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_ui8 ( dest, r0d+r, c0d+c, gan_pixel_ui32_to_ui8 ( gan_image_get_pix_gl_ui32(source,r0s+r,c0s+c) ) );

             break;

           case GAN_FLOAT32:
             gan_err_test_ptr ( copy_pixels, "extract_grey", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_ui8 ( dest, r0d+r, c0d+c, gan_pixel_f32_to_ui8 ( gan_image_get_pix_gl_f32(source,r0s+r,c0s+c) ) );

             break;
         
           case GAN_FLOAT64:
             gan_err_test_ptr ( copy_pixels, "extract_grey", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_ui8 ( dest, r0d+r, c0d+c, gan_pixel_f64_to_ui8 ( gan_image_get_pix_gl_f64(source,r0s+r,c0s+c) ) );

             break;
         
           case GAN_BOOL:
             gan_err_test_ptr ( copy_pixels, "extract_grey", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_ui8 ( dest, r0d+r, c0d+c,(gan_uint8)(gan_image_get_pix_b ( source, r0s+r, c0s+c )
                                                                             ? GAN_UINT8_MAX : 0) );

             break;
            
           case GAN_UINT12:
             gan_err_test_ptr ( copy_pixels, "extract_grey", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_ui8 ( dest, r0d+r, c0d+c, (gan_uint8)(gan_image_get_pix_gl_ui12(source,r0s+r,c0s+c).Y >> 4) );

             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_grey", GAN_ERROR_NOT_IMPLEMENTED, "");
             return NULL;
        }
        break;
      
      case GAN_UINT16:
        switch ( source->type )
        {
           case GAN_UINT8:
             gan_err_test_ptr ( copy_pixels, "extract_grey", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_ui16 ( dest, r0d+r, c0d+c,
                      gan_pixel_ui8_to_ui16 (
                          gan_image_get_pix_gl_ui8(source,r0s+r,c0s+c) ) );

             break;
         
           case GAN_UINT16:
             if ( copy_pixels )
                for ( r=0; r < height; r++ )
                   gan_copy_array_ui16 ( gan_image_get_pixptr_gl_ui16(source, r0s+r, c0s), 1, width,
                                         gan_image_get_pixptr_gl_ui16(dest, r0d+r, c0d), 1 );
             else
             {
                /* make destination image point into source image */
                gan_err_test_ptr ( r0d == 0 && c0d == 0, "extract_grey", GAN_ERROR_INCOMPATIBLE, "" );
                for ( r=0; r < height; r++ )
                   dest->row_data.gl.ui16[r] = gan_image_get_pixptr_gl_ui16(source, r0s+r, c0s);

                dest->pix_data_ptr = (unsigned char *)gan_image_get_pixptr_gl_ui16(source, r0s, c0s);
             }

             break;
                  
           case GAN_UINT32:
             gan_err_test_ptr ( copy_pixels, "extract_grey", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_ui16 ( dest, r0d+r, c0d+c,
                      gan_pixel_ui32_to_ui16 (
                          gan_image_get_pix_gl_ui32(source,r0s+r,c0s+c) ) );

             break;
         
           case GAN_FLOAT32:
             gan_err_test_ptr ( copy_pixels, "extract_grey", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_ui16 ( dest, r0d+r, c0d+c,
                      gan_pixel_f32_to_ui16 (
                          gan_image_get_pix_gl_f32(source,r0s+r,c0s+c) ) );

             break;
            
           case GAN_FLOAT64:
             gan_err_test_ptr ( copy_pixels, "extract_grey", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_ui16 ( dest, r0d+r, c0d+c,
                      gan_pixel_f64_to_ui16 (
                          gan_image_get_pix_gl_f64(source,r0s+r,c0s+c) ) );
             break;
            
           case GAN_BOOL:
             gan_err_test_ptr ( copy_pixels, "extract_grey", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_ui16 ( dest, r0d+r, c0d+c,(gan_uint16)(gan_image_get_pix_b ( source, r0s+r, c0s+c )
                                                                                   ? GAN_UINT16_MAX : 0) );

             break;
            
           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_grey", GAN_ERROR_NOT_IMPLEMENTED, "");
             return NULL;
        }
        break;
      
      case GAN_UINT32:
        switch ( source->type )
        {
           case GAN_UINT8:
             gan_err_test_ptr ( copy_pixels, "extract_grey", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_ui32 ( dest, r0d+r, c0d+c,
                      gan_pixel_ui8_to_ui32 (
                          gan_image_get_pix_gl_ui8(source,r0s+r,c0s+c) ) );

             break;
         
           case GAN_UINT16:
             gan_err_test_ptr ( copy_pixels, "extract_grey", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_ui32 ( dest, r0d+r, c0d+c,
                      gan_pixel_ui16_to_ui32 (
                          gan_image_get_pix_gl_ui16(source,r0s+r,c0s+c) ) );

           break;
         
           case GAN_UINT32:
             if ( copy_pixels )
                for ( r=0; r < height; r++ )
                   gan_copy_array_ui32 (
                       gan_image_get_pixptr_gl_ui32(source, r0s+r, c0s),
                       1, width,
                       gan_image_get_pixptr_gl_ui32(dest, r0d+r, c0d), 1 );
             else
             {
                /* make destination image point into source image */
                gan_err_test_ptr ( r0d == 0 && c0d == 0, "extract_grey", GAN_ERROR_INCOMPATIBLE, "" );
                for ( r=0; r < height; r++ )
                   dest->row_data.gl.ui32[r] = gan_image_get_pixptr_gl_ui32(source, r0s+r, c0s);

                dest->pix_data_ptr = (unsigned char *)gan_image_get_pixptr_gl_ui32(source, r0s, c0s);
             }

             break;
                  
           case GAN_FLOAT32:
             gan_err_test_ptr ( copy_pixels, "extract_grey", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_ui32 ( dest, r0d+r, c0d+c,
                      gan_pixel_f32_to_ui32 (
                          gan_image_get_pix_gl_f32(source,r0s+r,c0s+c) ) );

             break;
         
           case GAN_FLOAT64:
             gan_err_test_ptr ( copy_pixels, "extract_grey", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_ui32 ( dest, r0d+r, c0d+c,
                      gan_pixel_f64_to_ui32 (
                          gan_image_get_pix_gl_f64(source,r0s+r,c0s+c) ) );

             break;
         
           case GAN_BOOL:
             gan_err_test_ptr ( copy_pixels, "extract_grey", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_ui32 ( dest, r0d+r, c0d+c, (gan_image_get_pix_b ( source, r0s+r, c0s+c ) ? GAN_UINT32_MAX : 0) );

             break;
            
           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_grey", GAN_ERROR_NOT_IMPLEMENTED, "");
             return NULL;
        }
        break;
      
      case GAN_INT32:
        switch ( source->type )
        {
           case GAN_FLOAT32:
             gan_err_test_ptr ( copy_pixels, "extract_grey", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_i32 ( dest, r0d+r, c0d+c, gan_pixel_f32_to_i32 ( gan_image_get_pix_gl_f32(source,r0s+r,c0s+c) ) );

             break;
         
           case GAN_FLOAT64:
             gan_err_test_ptr ( copy_pixels, "extract_grey", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_i32 ( dest, r0d+r, c0d+c, gan_pixel_f64_to_i32(gan_image_get_pix_gl_f64(source,r0s+r,c0s+c) ) );

             break;
         
           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_grey", GAN_ERROR_NOT_IMPLEMENTED, "");
             return NULL;
        }
        break;
      
      case GAN_FLOAT32:
        switch ( source->type )
        {
           case GAN_UINT8:
             gan_err_test_ptr ( copy_pixels, "extract_grey", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_f32 ( dest, r0d+r, c0d+c,
                      gan_pixel_ui8_to_f32 (
                          gan_image_get_pix_gl_ui8(source,r0s+r,c0s+c) ) );

             break;
               
           case GAN_UINT16:
             gan_err_test_ptr ( copy_pixels, "extract_grey", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_f32 ( dest, r0d+r, c0d+c,
                      gan_pixel_ui16_to_f32 (
                          gan_image_get_pix_gl_ui16(source,r0s+r,c0s+c) ) );

             break;
               
           case GAN_INT32:
             gan_err_test_ptr ( copy_pixels, "extract_grey", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_f32 ( dest, r0d+r, c0d+c,
                      gan_pixel_i32_to_f32 (
                          gan_image_get_pix_gl_i(source,r0s+r,c0s+c) ) );

             break;
               
           case GAN_UINT32:
             gan_err_test_ptr ( copy_pixels, "extract_grey", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_f32 ( dest, r0d+r, c0d+c, gan_pixel_ui32_to_f32 (gan_image_get_pix_gl_ui32(source,r0s+r,c0s+c) ) );

             break;
               
           case GAN_FLOAT32:
             if ( copy_pixels )
                for ( r=0; r < height; r++ )
                   gan_copy_array_f32 (gan_image_get_pixptr_gl_f32(source, r0s+r, c0s), 1, width,
                                       gan_image_get_pixptr_gl_f32(dest, r0d+r, c0d), 1 );
             else
             {
                /* make destination image point into source image */
                gan_err_test_ptr ( r0d == 0 && c0d == 0, "extract_grey",
                                   GAN_ERROR_INCOMPATIBLE, "" );
                for ( r=0; r < height; r++ )
                   dest->row_data.gl.f32[r] = gan_image_get_pixptr_gl_f32(source, r0s+r, c0s);

                dest->pix_data_ptr = (unsigned char *)gan_image_get_pixptr_gl_f32(source, r0s, c0s);
             }

             break;
                     
           case GAN_FLOAT64:
             gan_err_test_ptr ( copy_pixels, "extract_grey",
                                GAN_ERROR_ILLEGAL_ARGUMENT, "" );
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_f32 ( dest, r0d+r, c0d+c, (gan_float32)gan_image_get_pix_gl_f64 ( source, r0s+r, c0s+c ) );

             break;
               
           case GAN_BOOL:
             gan_err_test_ptr ( copy_pixels, "extract_grey",
                                GAN_ERROR_ILLEGAL_ARGUMENT, "" );
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_f32 ( dest, r0d+r, c0d+c,
                                              gan_image_get_pix_b ( source, r0s+r, c0s+c)? 1.0F : 0.0F );
             break;
               
           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_grey", GAN_ERROR_NOT_IMPLEMENTED, "");
             return NULL;
        }
        break;
         
      case GAN_FLOAT64:
        switch ( source->type )
        {
           case GAN_UINT8:
             gan_err_test_ptr ( copy_pixels, "extract_grey", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_f64 ( dest, r0d+r, c0d+c,
                      gan_pixel_ui8_to_f64 (
                          gan_image_get_pix_gl_ui8(source,r0s+r,c0s+c) ) );

             break;
               
           case GAN_UINT16:
             gan_err_test_ptr ( copy_pixels, "extract_grey", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_f64 ( dest, r0d+r, c0d+c,
                      gan_pixel_ui16_to_f64 (
                          gan_image_get_pix_gl_ui16(source,r0s+r,c0s+c) ) );

             break;
               
           case GAN_INT32:
             gan_err_test_ptr ( copy_pixels, "extract_grey", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_f64 ( dest, r0d+r, c0d+c,
                      gan_pixel_i32_to_f64 (
                          gan_image_get_pix_gl_i(source,r0s+r,c0s+c) ) );

             break;
               
           case GAN_UINT32:
             gan_err_test_ptr ( copy_pixels, "extract_grey", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_f64 ( dest, r0d+r, c0d+c,
                      gan_pixel_ui32_to_f64 (
                          gan_image_get_pix_gl_ui32(source,r0s+r,c0s+c) ) );

             break;
               
           case GAN_FLOAT64:
             if ( copy_pixels )
                for ( r=0; r < height; r++ )
                   gan_copy_array_f64 ( gan_image_get_pixptr_gl_f64(source, r0s+r, c0s), 1, width,
                                        gan_image_get_pixptr_gl_f64(dest, r0d+r, c0d), 1 );
             else
             {
                /* make destination image point into source image */
                gan_err_test_ptr ( r0d == 0 && c0d == 0, "extract_grey", GAN_ERROR_INCOMPATIBLE, "" );
                for ( r=0; r < height; r++ )
                   dest->row_data.gl.f64[r] = gan_image_get_pixptr_gl_f64(source, r0s+r, c0s);

                dest->pix_data_ptr = (unsigned char *)gan_image_get_pixptr_gl_f64(source, r0s, c0s);
             }

             break;
                     
           case GAN_FLOAT32:
             gan_err_test_ptr ( copy_pixels, "extract_grey", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_f64 ( dest, r0d+r, c0d+c, (gan_float64)gan_image_get_pix_gl_f32 ( source, r0s+r, c0s+c ) );

             break;
               
           case GAN_BOOL:
             gan_err_test_ptr ( copy_pixels, "extract_grey", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_f64 ( dest, r0d+r, c0d+c, gan_image_get_pix_b ( source, r0s+r, c0s+c) ? 1.0 : 0.0 );

             break;
               
           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_grey", GAN_ERROR_NOT_IMPLEMENTED, "");
             return NULL;
        }
        break;
         
      case GAN_BOOL:
        switch ( source->type )
        {
           case GAN_UINT8:
             gan_err_test_ptr ( copy_pixels, "extract_grey", GAN_ERROR_INCOMPATIBLE, "" );
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_b ( dest, r0d+r, c0d+c, gan_image_get_pix_gl_ui8 ( source, r0s+r, c0s+c ) ? GAN_TRUE : GAN_FALSE );
             break;

           case GAN_UINT16:
             gan_err_test_ptr ( copy_pixels, "extract_grey", GAN_ERROR_INCOMPATIBLE, "" );
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_b ( dest, r0d+r, c0d+c, gan_image_get_pix_gl_ui16 ( source, r0s+r, c0s+c ) ? GAN_TRUE : GAN_FALSE );
             break;

           case GAN_UINT32:
             gan_err_test_ptr ( copy_pixels, "extract_grey",
                                GAN_ERROR_INCOMPATIBLE, "" );
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_b ( dest, r0d+r, c0d+c, gan_image_get_pix_gl_ui32 ( source, r0s+r, c0s+c ) ? GAN_TRUE : GAN_FALSE );
             break;

           case GAN_BOOL:
             if ( copy_pixels )
                for ( r=0; r < height; r++ )
                   for ( c=0; c < width; c++ )
                      gan_image_set_pix_b ( dest, r0d+r, c0d+c, gan_image_get_pix_b ( source, r0s+r, c0s+c ) );
             else
             {
                /* make destination image point into source image */
                gan_err_test_ptr ( r0d == 0 && c0d == 0, "extract_grey", GAN_ERROR_INCOMPATIBLE, "" );

                /* insertion of one bit array into another must conform to
                   word boundaries */
                gan_err_test_ptr ( (c0s % GAN_BITWORD_SIZE) == 0, "extract_grey", GAN_ERROR_INCOMPATIBLE, "" );
                for ( r=0; r < height; r++ )
                {
                   dest->row_data.gl.b[r] = dest->ba[r].data =
                   &source->row_data.gl.b[r0s+r][c0s/GAN_BITWORD_SIZE];
                   dest->ba[r].no_bits = width;
                   dest->ba[r].no_words = GAN_NO_BITWORDS(width);
                }

                dest->pix_data_ptr = (unsigned char *)&source->row_data.gl.b[r0s][c0s/GAN_BITWORD_SIZE];
             }

             break;
                  
           case GAN_FLOAT32:
             gan_err_test_ptr ( copy_pixels, "extract_grey", GAN_ERROR_INCOMPATIBLE, "" );
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_b ( dest, r0d+r, c0d+c, (gan_image_get_pix_gl_f32 ( source, r0s+r, c0s+c ) == 0.0F) ? GAN_FALSE : GAN_TRUE );
             break;

           case GAN_FLOAT64:
             gan_err_test_ptr ( copy_pixels, "extract_grey", GAN_ERROR_INCOMPATIBLE, "" );
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_b ( dest, r0d+r, c0d+c, (gan_image_get_pix_gl_f64 ( source, r0s+r, c0s+c ) == 0.0) ? GAN_FALSE : GAN_TRUE );
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_grey", GAN_ERROR_NOT_IMPLEMENTED, "");
             return NULL;
        }
        break;
            
      case GAN_UINT10:
        switch ( source->type )
        {
           case GAN_UINT10:
             if ( copy_pixels )
                for ( r=0; r < height; r++ )
                   gan_copy_array_ui16 ((gan_uint16*)gan_image_get_pixptr_gl_ui10(source,r0s+r,c0s), 1, width,
                                        (gan_uint16*)gan_image_get_pixptr_gl_ui10(dest,r0d+r,c0d), 1 );
             else
             {
                /* make destination image point into source image */
                gan_err_test_ptr ( r0d == 0 && c0d == 0, "extract_grey", GAN_ERROR_INCOMPATIBLE, "" );
                for ( r=0; r < height; r++ )
                   dest->row_data.gl.ui10[r] = gan_image_get_pixptr_gl_ui10(source,r0s+r,c0s);

                dest->pix_data_ptr = (unsigned char *)gan_image_get_pixptr_gl_ui10(source, r0s, c0s);
             }

             break;
                  
           case GAN_UINT8:
             gan_err_test_ptr ( copy_pixels, "extract_grey", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   Gan_YXPixel_ui10 pix;

                   pix.Y = gan_image_get_pix_gl_ui8(source,r0s+r,c0s+c) << 2;
                   pix.X = 0;
                   gan_image_set_pix_gl_ui10 ( dest, r0d+r, c0d+c, &pix );
                }

             break;
         
           case GAN_UINT16:
             gan_err_test_ptr ( copy_pixels, "extract_grey", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   Gan_YXPixel_ui10 pix;

                   pix.Y = gan_image_get_pix_gl_ui16(source,r0s+r,c0s+c) >> 6;
                   pix.X = 0;
                   gan_image_set_pix_gl_ui10 ( dest, r0d+r, c0d+c, &pix );
                }

             break;
         
           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_grey", GAN_ERROR_NOT_IMPLEMENTED, "");
             return NULL;
        }
        break;
      
      case GAN_UINT12:
        switch ( source->type )
        {
           case GAN_UINT12:
             if ( copy_pixels )
                for ( r=0; r < height; r++ )
                   gan_copy_array_ui16 ((gan_uint16*)gan_image_get_pixptr_gl_ui12(source,r0s+r,c0s), 1, width,
                                        (gan_uint16*)gan_image_get_pixptr_gl_ui12(dest,r0d+r,c0d), 1 );
             else
             {
                /* make destination image point into source image */
                gan_err_test_ptr ( r0d == 0 && c0d == 0, "extract_grey", GAN_ERROR_INCOMPATIBLE, "" );
                for ( r=0; r < height; r++ )
                   dest->row_data.gl.ui12[r] = gan_image_get_pixptr_gl_ui12(source,r0s+r,c0s);

                dest->pix_data_ptr = (unsigned char *)gan_image_get_pixptr_gl_ui12(source, r0s, c0s);
             }

             break;
                  
           case GAN_UINT8:
             gan_err_test_ptr ( copy_pixels, "extract_grey", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   Gan_YXPixel_ui12 pix;

                   pix.Y = gan_image_get_pix_gl_ui8(source,r0s+r,c0s+c) << 4;
                   pix.X = 0;
                   gan_image_set_pix_gl_ui12 ( dest, r0d+r, c0d+c, &pix );
                }

             break;
         
           case GAN_UINT16:
             gan_err_test_ptr ( copy_pixels, "extract_grey", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   Gan_YXPixel_ui12 pix;

                   pix.Y = gan_image_get_pix_gl_ui16(source,r0s+r,c0s+c) >> 4;
                   pix.X = 0;
                   gan_image_set_pix_gl_ui12 ( dest, r0d+r, c0d+c, &pix );
                }

             break;
         
           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_grey", GAN_ERROR_NOT_IMPLEMENTED, "");
             return NULL;
        }
        break;
      
      default:
        gan_err_flush_trace();
        gan_err_register ( "extract_grey", GAN_ERROR_NOT_IMPLEMENTED, "" );
        return NULL;
   }

   return dest;
}

static Gan_Image *
 extract_grey_to_gla ( const Gan_Image *grey, unsigned r0s, unsigned c0s,
                       unsigned height, unsigned width,
                       Gan_Image *gla, unsigned r0d, unsigned c0d )
{
   unsigned int r, c;
   
   gan_err_test_ptr ( grey->format == GAN_GREY_LEVEL_IMAGE && gla->format == GAN_GREY_LEVEL_ALPHA_IMAGE,
                      "extract_grey_to_gla", GAN_ERROR_INCOMPATIBLE, "" );
   switch ( gla->type )
   {
      case GAN_UINT8:
        switch ( grey->type )
        {
           Gan_GLAPixel_ui8 glaPix;

           case GAN_UINT8:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   glaPix.I = gan_image_get_pix_gl_ui8 ( grey, r0s+r, c0s+c );
                   glaPix.A = GAN_UINT8_MAX;
                   gan_image_set_pix_gla_ui8 ( gla, r0d+r, c0d+c, &glaPix );
                }
             break;

           case GAN_UINT16:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   glaPix.I = gan_pixel_ui16_to_ui8 ( gan_image_get_pix_gl_ui16 ( grey, r0s+r, c0s+c ) );
                   glaPix.A = GAN_UINT8_MAX;
                   gan_image_set_pix_gla_ui8 ( gla, r0d+r, c0d+c, &glaPix );
                }
             break;

           case GAN_UINT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   glaPix.I = gan_pixel_ui32_to_ui8 ( gan_image_get_pix_gl_ui32 ( grey, r0s+r, c0s+c ) );
                   glaPix.A = GAN_UINT8_MAX;
                   gan_image_set_pix_gla_ui8 ( gla, r0d+r, c0d+c, &glaPix );
                }
             break;

           case GAN_FLOAT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   glaPix.I = gan_pixel_f32_to_ui8 ( gan_image_get_pix_gl_f32 ( grey, r0s+r, c0s+c ) );
                   glaPix.A = GAN_UINT8_MAX;
                   gan_image_set_pix_gla_ui8 ( gla, r0d+r, c0d+c, &glaPix );
                }
             break;
             
           case GAN_FLOAT64:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   glaPix.I = gan_pixel_f64_to_ui8 ( gan_image_get_pix_gl_f64 ( grey, r0s+r, c0s+c ) );
                   glaPix.A = GAN_UINT8_MAX;
                   gan_image_set_pix_gla_ui8 ( gla, r0d+r, c0d+c, &glaPix );
                }
             break;
             
           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_grey_to_gla", GAN_ERROR_NOT_IMPLEMENTED, "" );
             return NULL;
        }
        break;
        
      case GAN_UINT16:
        switch ( grey->type )
        {
           Gan_GLAPixel_ui16 glaPix;

           case GAN_UINT8:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   glaPix.I = gan_pixel_ui8_to_ui16 ( gan_image_get_pix_gl_ui8 ( grey, r0s+r, c0s+c ) );
                   glaPix.A = GAN_UINT16_MAX;
                   gan_image_set_pix_gla_ui16 ( gla, r0d+r, c0d+c, &glaPix );
                }
             break;

           case GAN_UINT16:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   glaPix.I = gan_image_get_pix_gl_ui16 ( grey, r0s+r, c0s+c );
                   glaPix.A = GAN_UINT16_MAX;
                   gan_image_set_pix_gla_ui16 ( gla, r0d+r, c0d+c, &glaPix );
                }
             break;

           case GAN_UINT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   glaPix.I = gan_pixel_ui32_to_ui16 ( gan_image_get_pix_gl_ui32 ( grey, r0s+r, c0s+c ) );
                   glaPix.A = GAN_UINT16_MAX;
                   gan_image_set_pix_gla_ui16 ( gla, r0d+r, c0d+c, &glaPix );
                }
             break;

           case GAN_FLOAT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   glaPix.I = gan_pixel_f32_to_ui16 ( gan_image_get_pix_gl_f32 ( grey, r0s+r, c0s+c ) );
                   glaPix.A = GAN_UINT16_MAX;
                   gan_image_set_pix_gla_ui16 ( gla, r0d+r, c0d+c, &glaPix );
                }
             break;
             
           case GAN_FLOAT64:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   glaPix.I = gan_pixel_f64_to_ui16 ( gan_image_get_pix_gl_f64 ( grey, r0s+r, c0s+c ) );
                   glaPix.A = GAN_UINT16_MAX;
                   gan_image_set_pix_gla_ui16 ( gla, r0d+r, c0d+c, &glaPix );
                }
             break;
             
           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_grey_to_gla", GAN_ERROR_NOT_IMPLEMENTED, "" );
             return NULL;
        }
        break;
        
      case GAN_UINT32:
        switch ( grey->type )
        {
           Gan_GLAPixel_ui32 glaPix;

           case GAN_UINT8:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   glaPix.I = gan_pixel_ui8_to_ui32 ( gan_image_get_pix_gl_ui8 ( grey, r0s+r, c0s+c ) );
                   glaPix.A = GAN_UINT32_MAX;
                   gan_image_set_pix_gla_ui32 ( gla, r0d+r, c0d+c, &glaPix );
                }
             break;

           case GAN_UINT16:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   glaPix.I = gan_pixel_ui16_to_ui32 ( gan_image_get_pix_gl_ui16 ( grey, r0s+r, c0s+c ) );
                   glaPix.A = GAN_UINT32_MAX;
                   gan_image_set_pix_gla_ui32 ( gla, r0d+r, c0d+c, &glaPix );
                }
             break;

           case GAN_UINT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   glaPix.I = gan_image_get_pix_gl_ui32 ( grey, r0s+r, c0s+c );
                   glaPix.A = GAN_UINT32_MAX;
                   gan_image_set_pix_gla_ui32 ( gla, r0d+r, c0d+c, &glaPix );
                }
             break;

           case GAN_FLOAT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   glaPix.I = gan_pixel_f32_to_ui32 ( gan_image_get_pix_gl_f32 ( grey, r0s+r, c0s+c ) );
                   glaPix.A = GAN_UINT32_MAX;
                   gan_image_set_pix_gla_ui32 ( gla, r0d+r, c0d+c, &glaPix );
                }
             break;
             
           case GAN_FLOAT64:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   glaPix.I = gan_pixel_f64_to_ui32 ( gan_image_get_pix_gl_f64 ( grey, r0s+r, c0s+c ) );
                   glaPix.A = GAN_UINT32_MAX;
                   gan_image_set_pix_gla_ui32 ( gla, r0d+r, c0d+c, &glaPix );
                }
             break;
             
           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_grey_to_gla", GAN_ERROR_NOT_IMPLEMENTED, "" );
             return NULL;
        }
        break;
        
      case GAN_FLOAT32:
        switch ( grey->type )
        {
           Gan_GLAPixel_f32 glaPix;

           case GAN_UINT8:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   glaPix.I = gan_pixel_ui8_to_f32 ( gan_image_get_pix_gl_ui8 ( grey, r0s+r, c0s+c ) );
                   glaPix.A = 1.0F;
                   gan_image_set_pix_gla_f32 ( gla, r0d+r, c0d+c, &glaPix );
                }
             break;

           case GAN_UINT16:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   glaPix.I = gan_pixel_ui16_to_f32 ( gan_image_get_pix_gl_ui16 ( grey, r0s+r, c0s+c ) );
                   glaPix.A = 1.0F;
                   gan_image_set_pix_gla_f32 ( gla, r0d+r, c0d+c, &glaPix );
                }
             break;

           case GAN_UINT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   glaPix.I = gan_pixel_ui32_to_f32 ( gan_image_get_pix_gl_ui32 ( grey, r0s+r, c0s+c ) );
                   glaPix.A = 1.0F;
                   gan_image_set_pix_gla_f32 ( gla, r0d+r, c0d+c, &glaPix );
                }
             break;

           case GAN_FLOAT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   glaPix.I = gan_image_get_pix_gl_f32 ( grey, r0s+r, c0s+c );
                   glaPix.A = 1.0F;
                   gan_image_set_pix_gla_f32 ( gla, r0d+r, c0d+c, &glaPix );
                }
             break;
             
           case GAN_FLOAT64:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   glaPix.I = (gan_float32)
                              gan_image_get_pix_gl_f64 ( grey, r0s+r, c0s+c );
                   glaPix.A = 1.0F;
                   gan_image_set_pix_gla_f32 ( gla, r0d+r, c0d+c, &glaPix );
                }
             break;
             
           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_grey_to_gla",
                                GAN_ERROR_NOT_IMPLEMENTED, "" );
             return NULL;
        }
        break;
        
      case GAN_FLOAT64:
        switch ( grey->type )
        {
           Gan_GLAPixel_f64 glaPix;

           case GAN_UINT8:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   glaPix.I = gan_pixel_ui8_to_f64(gan_image_get_pix_gl_ui8 ( grey, r0s+r, c0s+c ) );
                   glaPix.A = 1.0;
                   gan_image_set_pix_gla_f64 ( gla, r0d+r, c0d+c, &glaPix );
                }
             break;

           case GAN_UINT16:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   glaPix.I = gan_pixel_ui16_to_f64 (gan_image_get_pix_gl_ui16 ( grey, r0s+r, c0s+c ) );
                   glaPix.A = 1.0;
                   gan_image_set_pix_gla_f64 ( gla, r0d+r, c0d+c, &glaPix );
                }
             break;

           case GAN_UINT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   glaPix.I = gan_pixel_ui32_to_f64 (gan_image_get_pix_gl_ui32 ( grey, r0s+r, c0s+c ) );
                   glaPix.A = 1.0;
                   gan_image_set_pix_gla_f64 ( gla, r0d+r, c0d+c, &glaPix );
                }
             break;

           case GAN_FLOAT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   glaPix.I = (gan_float64)gan_image_get_pix_gl_f32 ( grey, r0s+r, c0s+c );
                   glaPix.A = 1.0;
                   gan_image_set_pix_gla_f64 ( gla, r0d+r, c0d+c, &glaPix );
                }
             break;
             
           case GAN_FLOAT64:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   glaPix.I = gan_image_get_pix_gl_f64 ( grey, r0s+r, c0s+c );
                   glaPix.A = 1.0;
                   gan_image_set_pix_gla_f64 ( gla, r0d+r, c0d+c, &glaPix );
                }
             break;
             
           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_grey_to_gla", GAN_ERROR_NOT_IMPLEMENTED, "" );
             return NULL;
        }
        break;
        
      default:
        gan_err_flush_trace();
        gan_err_register ( "extract_grey_to_gla", GAN_ERROR_NOT_IMPLEMENTED, "" );
        return NULL;
   }
             
   return gla;
}

/* Converts an RGB image with alpha channel to a different type
 *
 * The converted RGB image is returned.
 */
static Gan_Image *
 extract_gla ( const Gan_Image *source, unsigned r0s, unsigned c0s,
               unsigned height, unsigned width, Gan_Bool copy_pixels,
               Gan_Image *dest, unsigned r0d, unsigned c0d )
{
   unsigned int r, c;
   
   gan_err_test_ptr ( source->format == GAN_GREY_LEVEL_ALPHA_IMAGE && dest->format == GAN_GREY_LEVEL_ALPHA_IMAGE,
                      "extract_gla", GAN_ERROR_INCOMPATIBLE, "" );
   
   switch ( dest->type )
   {
      case GAN_UINT8:
        switch ( source->type )
        {
           Gan_GLAPixel_ui8 ui8pix;

           case GAN_UINT8:
             if ( copy_pixels )
                for ( r=0; r < height; r++ )
                   gan_copy_array_ui8 ( (gan_uint8 *)gan_image_get_pixptr_gla_ui8(source, r0s+r, c0s), 1, 2*width,
                                        (gan_uint8 *)gan_image_get_pixptr_gla_ui8(dest, r0d+r, c0d), 1 );
             else
             {
                /* make destination image point into source image */
                gan_err_test_ptr ( r0d == 0 && c0d == 0, "extract_gla", GAN_ERROR_INCOMPATIBLE, "" );
                for ( r=0; r < height; r++ )
                   dest->row_data.gla.ui8[r] = gan_image_get_pixptr_gla_ui8(source, r0s+r, c0s);

                dest->pix_data_ptr = (unsigned char *)gan_image_get_pixptr_gla_ui8(source, r0s, c0s);
             }
             break;
         
           case GAN_UINT16:
           {
              Gan_GLAPixel_ui16 ui16pix;
            
              gan_err_test_ptr ( copy_pixels, "extract_gla", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    /* clip GLA pixel values to gan_uint8 limits */
                    ui16pix = gan_image_get_pix_gla_ui16 ( source, r0s+r, c0s+c );
                    ui8pix.I = gan_pixel_ui16_to_ui8(ui16pix.I);
                    ui8pix.A = gan_pixel_ui16_to_ui8(ui16pix.A);
                    gan_image_set_pix_gla_ui8 ( dest, r0d+r, c0d+c, &ui8pix );
                 }
           }
           break;
         
           case GAN_UINT32:
           {
              Gan_GLAPixel_ui32 ui32pix;
            
              gan_err_test_ptr ( copy_pixels, "extract_gla", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    /* clip GLA pixel values to gan_uint8 limits */
                    ui32pix = gan_image_get_pix_gla_ui32 ( source, r0s+r, c0s+c );
                    ui8pix.I = gan_pixel_ui32_to_ui8(ui32pix.I);
                    ui8pix.A = gan_pixel_ui32_to_ui8(ui32pix.A);
                    gan_image_set_pix_gla_ui8 ( dest, r0d+r, c0d+c, &ui8pix );
                 }
           }
           break;
         
           case GAN_FLOAT32:
           {
              Gan_GLAPixel_f32 f32pix;
            
              gan_err_test_ptr ( copy_pixels, "extract_gla", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    /* clip GLA pixel values to gan_uint8 limits */
                    f32pix = gan_image_get_pix_gla_f32 ( source, r0s+r, c0s+c );
                    ui8pix.I = gan_pixel_f32_to_ui8(f32pix.I);
                    ui8pix.A = gan_pixel_f32_to_ui8(f32pix.A);
                    gan_image_set_pix_gla_ui8 ( dest, r0d+r, c0d+c, &ui8pix );
                 }
           }
           break;
         
           case GAN_FLOAT64:
           {
              Gan_GLAPixel_f64 f64pix;
            
              gan_err_test_ptr ( copy_pixels, "extract_gla", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    /* clip GLA pixel values to gan_uint8 limits */
                    f64pix = gan_image_get_pix_gla_f64 ( source, r0s+r, c0s+c );
                    ui8pix.I = gan_pixel_f64_to_ui8(f64pix.I);
                    ui8pix.A = gan_pixel_f64_to_ui8(f64pix.A);
                    gan_image_set_pix_gla_ui8 ( dest, r0d+r, c0d+c, &ui8pix );
                 }
           }
           break;
         
           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_gla", GAN_ERROR_NOT_IMPLEMENTED, "" );
             return NULL;
        }
        break;
      
      case GAN_UINT16:
        switch ( source->type )
        {
           Gan_GLAPixel_ui16 ui16pix;

           case GAN_UINT8:
           {
              Gan_GLAPixel_ui8 ui8pix;
            
              gan_err_test_ptr ( copy_pixels, "extract_gla", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    /* clip GLA pixel values to gan_uint8 limits */
                    ui8pix = gan_image_get_pix_gla_ui8 ( source, r0s+r, c0s+c );
                    ui16pix.I = gan_pixel_ui8_to_ui16(ui8pix.I);
                    ui16pix.A = gan_pixel_ui8_to_ui16(ui8pix.A);
                    gan_image_set_pix_gla_ui16 ( dest, r0d+r, c0d+c, &ui16pix );
                 }
           }
           break;
         
           case GAN_UINT16:
             if ( copy_pixels )
                for ( r=0; r < height; r++ )
                   gan_copy_array_ui16 ((gan_uint16 *)gan_image_get_pixptr_gla_ui16(source, r0s+r, c0s), 1, 2*width, (gan_uint16 *)
                                        gan_image_get_pixptr_gla_ui16(dest, r0d+r, c0d), 1 );
             else
             {
                /* make destination image point into source image */
                gan_err_test_ptr ( r0d == 0 && c0d == 0, "extract_gla", GAN_ERROR_INCOMPATIBLE, "" );
                for ( r=0; r < height; r++ )
                   dest->row_data.gla.ui16[r] = gan_image_get_pixptr_gla_ui16(source, r0s+r, c0s);

                dest->pix_data_ptr = (unsigned char *)gan_image_get_pixptr_gla_ui16(source, r0s, c0s);
             }
             break;
         
           case GAN_UINT32:
           {
              Gan_GLAPixel_ui32 ui32pix;
            
              gan_err_test_ptr ( copy_pixels, "extract_gla", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    /* clip GLA pixel values to gan_uint8 limits */
                    ui32pix = gan_image_get_pix_gla_ui32 ( source, r0s+r, c0s+c );
                    ui16pix.I = gan_pixel_ui32_to_ui16(ui32pix.I);
                    ui16pix.A = gan_pixel_ui32_to_ui16(ui32pix.A);
                    gan_image_set_pix_gla_ui16 ( dest, r0d+r, c0d+c, &ui16pix );
                 }
           }
           break;
         
           case GAN_FLOAT32:
           {
              Gan_GLAPixel_f32 f32pix;
            
              gan_err_test_ptr ( copy_pixels, "extract_gla", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    /* clip GLA pixel values to gan_uint8 limits */
                    f32pix = gan_image_get_pix_gla_f32 ( source, r0s+r, c0s+c );
                    ui16pix.I = gan_pixel_f32_to_ui16(f32pix.I);
                    ui16pix.A = gan_pixel_f32_to_ui16(f32pix.A);
                    gan_image_set_pix_gla_ui16 ( dest, r0d+r, c0d+c, &ui16pix );
                 }
           }
           break;
         
           case GAN_FLOAT64:
           {
              Gan_GLAPixel_f64 f64pix;
            
              gan_err_test_ptr ( copy_pixels, "extract_gla", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    /* clip GLA pixel values to gan_uint8 limits */
                    f64pix = gan_image_get_pix_gla_f64 ( source, r0s+r, c0s+c );
                    ui16pix.I = gan_pixel_f64_to_ui16(f64pix.I);
                    ui16pix.A = gan_pixel_f64_to_ui16(f64pix.A);
                    gan_image_set_pix_gla_ui16 ( dest, r0d+r, c0d+c, &ui16pix );
                 }
           }
           break;
         
           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_gla", GAN_ERROR_NOT_IMPLEMENTED, "");
             return NULL;
        }
        break;
      
      case GAN_UINT32:
        switch ( source->type )
        {
           Gan_GLAPixel_ui32 ui32pix;

           case GAN_UINT8:
           {
              Gan_GLAPixel_ui8 ui8pix;
            
              gan_err_test_ptr ( copy_pixels, "extract_gla", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    /* clip GLA pixel values to gan_uint8 limits */
                    ui8pix = gan_image_get_pix_gla_ui8 ( source, r0s+r, c0s+c );
                    ui32pix.I = gan_pixel_ui8_to_ui32(ui8pix.I);
                    ui32pix.A = gan_pixel_ui8_to_ui32(ui8pix.A);
                    gan_image_set_pix_gla_ui32 ( dest, r0d+r, c0d+c, &ui32pix );
                 }
           }
           break;
         
           case GAN_UINT16:
           {
              Gan_GLAPixel_ui16 ui16pix;
            
              gan_err_test_ptr ( copy_pixels, "extract_gla", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    /* clip GLA pixel values to gan_uint8 limits */
                    ui16pix = gan_image_get_pix_gla_ui16 ( source, r0s+r, c0s+c );
                    ui32pix.I = gan_pixel_ui16_to_ui32(ui16pix.I);
                    ui32pix.A = gan_pixel_ui16_to_ui32(ui16pix.A);
                    gan_image_set_pix_gla_ui32 ( dest, r0d+r, c0d+c, &ui32pix );
                 }
           }
           break;
         
           case GAN_UINT32:
             if ( copy_pixels )
                for ( r=0; r < height; r++ )
                   gan_copy_array_ui32 ((gan_uint32 *)gan_image_get_pixptr_gla_ui32(source, r0s+r, c0s), 1, 2*width, (gan_uint32 *)
                                        gan_image_get_pixptr_gla_ui32(dest, r0d+r, c0d), 1 );
             else
             {
                /* make destination image point into source image */
                gan_err_test_ptr ( r0d == 0 && c0d == 0, "extract_gla", GAN_ERROR_INCOMPATIBLE, "" );
                for ( r=0; r < height; r++ )
                   dest->row_data.gla.ui32[r] = gan_image_get_pixptr_gla_ui32(source, r0s+r, c0s);

                dest->pix_data_ptr = (unsigned char *)gan_image_get_pixptr_gla_ui32(source, r0s, c0s);
             }
             break;
         
           case GAN_FLOAT32:
           {
              Gan_GLAPixel_f32 f32pix;
            
              gan_err_test_ptr ( copy_pixels, "extract_gla", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    /* clip GLA pixel values to gan_uint8 limits */
                    f32pix = gan_image_get_pix_gla_f32 ( source, r0s+r, c0s+c );
                    ui32pix.I = gan_pixel_f32_to_ui32(f32pix.I);
                    ui32pix.A = gan_pixel_f32_to_ui32(f32pix.A);
                    gan_image_set_pix_gla_ui32 ( dest, r0d+r, c0d+c, &ui32pix );
                 }
           }
           break;
         
           case GAN_FLOAT64:
           {
              Gan_GLAPixel_f64 f64pix;
            
              gan_err_test_ptr ( copy_pixels, "extract_gla", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    /* clip GLA pixel values to gan_uint8 limits */
                    f64pix = gan_image_get_pix_gla_f64 ( source, r0s+r, c0s+c );
                    ui32pix.I = gan_pixel_f64_to_ui32(f64pix.I);
                    ui32pix.A = gan_pixel_f64_to_ui32(f64pix.A);
                    gan_image_set_pix_gla_ui32 ( dest, r0d+r, c0d+c, &ui32pix );
                 }
           }
           break;
         
           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_gla", GAN_ERROR_NOT_IMPLEMENTED, "");
             return NULL;
        }
        break;
      
      case GAN_FLOAT32:
        switch ( source->type )
        {
           Gan_GLAPixel_f32 f32pix;

           case GAN_UINT8:
           {
              Gan_GLAPixel_ui8 ui8pix;
               
              gan_err_test_ptr ( copy_pixels, "extract_gla", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    ui8pix = gan_image_get_pix_gla_ui8 ( source, r0s+r, c0s+c );
                    f32pix.I = gan_pixel_ui8_to_f32(ui8pix.I);
                    f32pix.A = gan_pixel_ui8_to_f32(ui8pix.A);
                    gan_image_set_pix_gla_f32 ( dest, r0d+r, c0d+c, &f32pix );
                 }
           }
           break;
            
           case GAN_UINT16:
           {
              Gan_GLAPixel_ui16 ui16pix;
               
              gan_err_test_ptr ( copy_pixels, "extract_gla", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    ui16pix = gan_image_get_pix_gla_ui16 ( source, r0s+r, c0s+c );
                    f32pix.I = gan_pixel_ui16_to_f32(ui16pix.I);
                    f32pix.A = gan_pixel_ui16_to_f32(ui16pix.A);
                    gan_image_set_pix_gla_f32 ( dest, r0d+r, c0d+c, &f32pix );
                 }
           }
           break;
            
           case GAN_UINT32:
           {
              Gan_GLAPixel_ui32 ui32pix;
               
              gan_err_test_ptr ( copy_pixels, "extract_gla", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    ui32pix = gan_image_get_pix_gla_ui32 ( source, r0s+r, c0s+c );
                    f32pix.I = gan_pixel_ui32_to_f32(ui32pix.I);
                    f32pix.A = gan_pixel_ui32_to_f32(ui32pix.A);
                    gan_image_set_pix_gla_f32 ( dest, r0d+r, c0d+c, &f32pix );
                 }
           }
           break;
            
           case GAN_FLOAT32:
             if ( copy_pixels )
                for ( r=0; r < height; r++ )
                   gan_copy_array_f32 ( (gan_float32 *)
                             gan_image_get_pixptr_gla_f32(source, r0s+r, c0s),
                             1, 2*width, (gan_float32 *)
                             gan_image_get_pixptr_gla_f32(dest, r0d+r, c0d), 1 );
             else
             {
                /* make destination image point into source image */
                gan_err_test_ptr ( r0d == 0 && c0d == 0, "extract_gla", GAN_ERROR_INCOMPATIBLE, "" );
                for ( r=0; r < height; r++ )
                   dest->row_data.gla.f32[r] = gan_image_get_pixptr_gla_f32(source, r0s+r, c0s);

                dest->pix_data_ptr = (unsigned char *)gan_image_get_pixptr_gla_f32(source, r0s, c0s);
             }
             break;
            
           case GAN_FLOAT64:
           {
              Gan_GLAPixel_f64 f64pix;
               
              gan_err_test_ptr ( copy_pixels, "extract_gla", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    f64pix = gan_image_get_pix_gla_f64 ( source, r0s+r, c0s+c );
                    f32pix.I = (gan_float32)f64pix.I;
                    f32pix.A = (gan_float32)f64pix.A;
                    gan_image_set_pix_gla_f32 ( dest, r0d+r, c0d+c, &f32pix );
                 }
           }
           break;
            
           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_gla", GAN_ERROR_NOT_IMPLEMENTED, "");
             return NULL;
        }
        break;
         
      case GAN_FLOAT64:
        switch ( source->type )
        {
           Gan_GLAPixel_f64 f64pix;

           case GAN_UINT8:
           {
              Gan_GLAPixel_ui8 ui8pix;

              gan_err_test_ptr ( copy_pixels, "extract_gla", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    ui8pix = gan_image_get_pix_gla_ui8 ( source, r0s+r, c0s+c );
                    f64pix.I = gan_pixel_ui8_to_f64(ui8pix.I);
                    f64pix.A = gan_pixel_ui8_to_f64(ui8pix.A);
                    gan_image_set_pix_gla_f64 ( dest, r0d+r, c0d+c, &f64pix );
                 }
           }
           break;
            
           case GAN_UINT16:
           {
              Gan_GLAPixel_ui16 ui16pix;
               
              gan_err_test_ptr ( copy_pixels, "extract_gla", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    ui16pix = gan_image_get_pix_gla_ui16 ( source, r0s+r, c0s+c );
                    f64pix.I = gan_pixel_ui16_to_f64(ui16pix.I);
                    f64pix.A = gan_pixel_ui16_to_f64(ui16pix.A);
                    gan_image_set_pix_gla_f64 ( dest, r0d+r, c0d+c, &f64pix );
                 }
           }
           break;
            
           case GAN_UINT32:
           {
              Gan_GLAPixel_ui32 ui32pix;
               
              gan_err_test_ptr ( copy_pixels, "extract_gla", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    ui32pix = gan_image_get_pix_gla_ui32 ( source, r0s+r, c0s+c );
                    f64pix.I = gan_pixel_ui32_to_f64(ui32pix.I);
                    f64pix.A = gan_pixel_ui32_to_f64(ui32pix.A);
                    gan_image_set_pix_gla_f64 ( dest, r0d+r, c0d+c, &f64pix );
                 }
           }
           break;
            
           case GAN_FLOAT32:
           {
              Gan_GLAPixel_f32 f32pix;
               
              gan_err_test_ptr ( copy_pixels, "extract_gla", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    f32pix = gan_image_get_pix_gla_f32 ( source, r0s+r, c0s+c );
                    f64pix.I = (gan_float64)f32pix.I;
                    f64pix.A = (gan_float64)f32pix.A;
                    gan_image_set_pix_gla_f64 ( dest, r0d+r, c0d+c, &f64pix );
                 }
           }
           break;
            
           case GAN_FLOAT64:
             if ( copy_pixels )
                for ( r=0; r < height; r++ )
                   gan_copy_array_f64 ( (gan_float64 *)gan_image_get_pixptr_gla_f64(source, r0s+r, c0s), 1, 2*width, (gan_float64 *)
                                        gan_image_get_pixptr_gla_f64(dest, r0d+r, c0d), 1 );
             else
             {
                /* make destination image point into source image */
                gan_err_test_ptr ( r0d == 0 && c0d == 0, "extract_gla", GAN_ERROR_INCOMPATIBLE, "" );
                for ( r=0; r < height; r++ )
                   dest->row_data.gla.f64[r] = gan_image_get_pixptr_gla_f64(source, r0s+r, c0s);

                dest->pix_data_ptr = (unsigned char *)gan_image_get_pixptr_gla_f64(source, r0s, c0s);
             }
             break;
            
           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_gla", GAN_ERROR_NOT_IMPLEMENTED, "");
             return NULL;
        }
        break;
         
      default:
        gan_err_flush_trace();
        gan_err_register ( "extract_gla", GAN_ERROR_NOT_IMPLEMENTED, "" );
        return NULL;
   }
   
   return dest;
}

static Gan_Image *
 extract_grey_to_rgb ( const Gan_Image *grey, unsigned r0s, unsigned c0s,
                       unsigned height, unsigned width,
                       Gan_Image *rgb, unsigned r0d, unsigned c0d )
{
   unsigned int r, c;
   
   switch ( rgb->type )
   {
      case GAN_UINT8:
        switch ( grey->type )
        {
           Gan_RGBPixel_ui8 rgbPix;

           case GAN_UINT8:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   rgbPix.R = rgbPix.G = rgbPix.B =
                       gan_image_get_pix_gl_ui8 ( grey, r0s+r, c0s+c );
                   gan_image_set_pix_rgb_ui8 ( rgb, r0d+r, c0d+c, &rgbPix );
                }
             break;

           case GAN_UINT16:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   rgbPix.R = rgbPix.G = rgbPix.B =
                   gan_pixel_ui16_to_ui8(gan_image_get_pix_gl_ui16 (grey,
                                                               r0s+r, c0s+c));
                   gan_image_set_pix_rgb_ui8 ( rgb, r0d+r, c0d+c, &rgbPix );
                }
             break;

           case GAN_UINT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   rgbPix.R = rgbPix.G = rgbPix.B =
                   gan_pixel_ui32_to_ui8(gan_image_get_pix_gl_ui32 (grey,
                                                               r0s+r, c0s+c));
                   gan_image_set_pix_rgb_ui8 ( rgb, r0d+r, c0d+c, &rgbPix );
                }
             break;

           case GAN_FLOAT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   rgbPix.R = rgbPix.G = rgbPix.B =
                   gan_pixel_f32_to_ui8(gan_image_get_pix_gl_f32 (grey,
                                                             r0s+r, c0s+c));
                   gan_image_set_pix_rgb_ui8 ( rgb, r0d+r, c0d+c, &rgbPix );
                }
             break;
             
           case GAN_FLOAT64:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   rgbPix.R = rgbPix.G = rgbPix.B =
                   gan_pixel_f64_to_ui8(gan_image_get_pix_gl_f64 (grey,
                                                             r0s+r, c0s+c));
                   gan_image_set_pix_rgb_ui8 ( rgb, r0d+r, c0d+c, &rgbPix );
                }
             break;
             
           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_grey_to_rgb",
                                GAN_ERROR_NOT_IMPLEMENTED, "" );
             return NULL;
        }
        break;
        
      case GAN_UINT16:
        switch ( grey->type )
        {
           Gan_RGBPixel_ui16 rgbPix;

           case GAN_UINT8:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   rgbPix.R = rgbPix.G = rgbPix.B =
                   gan_pixel_ui8_to_ui16(gan_image_get_pix_gl_ui8 (grey,
                                                               r0s+r, c0s+c));
                   gan_image_set_pix_rgb_ui16 ( rgb, r0d+r, c0d+c, &rgbPix );
                }
             break;

           case GAN_UINT16:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   rgbPix.R = rgbPix.G = rgbPix.B =
                   gan_image_get_pix_gl_ui16 ( grey, r0s+r, c0s+c);
                   gan_image_set_pix_rgb_ui16 ( rgb, r0d+r, c0d+c, &rgbPix );
                }
             break;

           case GAN_UINT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   rgbPix.R = rgbPix.G = rgbPix.B =
                   gan_pixel_ui32_to_ui16(gan_image_get_pix_gl_ui32 (grey,
                                                               r0s+r, c0s+c));
                   gan_image_set_pix_rgb_ui16 ( rgb, r0d+r, c0d+c, &rgbPix );
                }
             break;

           case GAN_FLOAT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   rgbPix.R = rgbPix.G = rgbPix.B =
                   gan_pixel_f32_to_ui16(gan_image_get_pix_gl_f32 (grey,
                                                             r0s+r, c0s+c));
                   gan_image_set_pix_rgb_ui16 ( rgb, r0d+r, c0d+c, &rgbPix );
                }
             break;
             
           case GAN_FLOAT64:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   rgbPix.R = rgbPix.G = rgbPix.B =
                   gan_pixel_f64_to_ui16(gan_image_get_pix_gl_f64 (grey,
                                                             r0s+r, c0s+c));
                   gan_image_set_pix_rgb_ui16 ( rgb, r0d+r, c0d+c, &rgbPix );
                }
             break;
             
           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_grey_to_rgb",
                                GAN_ERROR_NOT_IMPLEMENTED, "" );
             return NULL;
        }
        break;
        
      case GAN_INT32:
        switch ( grey->type )
        {
           Gan_RGBPixel_i32 rgbPix;

           case GAN_INT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   rgbPix.R = rgbPix.G = rgbPix.B =
                   gan_image_get_pix_gl_i32 ( grey, r0s+r, c0s+c );
                   gan_image_set_pix_rgb_i32 ( rgb, r0d+r, c0d+c, &rgbPix );
                }
             break;

           case GAN_FLOAT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   rgbPix.R = rgbPix.G = rgbPix.B =
                   gan_pixel_f32_to_i32(gan_image_get_pix_gl_f32 (grey, r0s+r, c0s+c));
                   gan_image_set_pix_rgb_i32 ( rgb, r0d+r, c0d+c, &rgbPix );
                }
             break;
             
           case GAN_FLOAT64:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   rgbPix.R = rgbPix.G = rgbPix.B =
                   gan_pixel_f64_to_i32(gan_image_get_pix_gl_f64 (grey, r0s+r, c0s+c));
                   gan_image_set_pix_rgb_i32 ( rgb, r0d+r, c0d+c, &rgbPix );
                }
             break;
             
           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_grey_to_rgb", GAN_ERROR_NOT_IMPLEMENTED, "" );
             return NULL;
        }
        break;
        
      case GAN_UINT32:
        switch ( grey->type )
        {
           Gan_RGBPixel_ui32 rgbPix;

           case GAN_UINT8:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   rgbPix.R = rgbPix.G = rgbPix.B =
                   gan_pixel_ui8_to_ui32(gan_image_get_pix_gl_ui8 (grey, r0s+r, c0s+c));
                   gan_image_set_pix_rgb_ui32 ( rgb, r0d+r, c0d+c, &rgbPix );
                }
             break;

           case GAN_UINT16:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   rgbPix.R = rgbPix.G = rgbPix.B =
                   gan_pixel_ui16_to_ui32(gan_image_get_pix_gl_ui16 (grey, r0s+r, c0s+c));
                   gan_image_set_pix_rgb_ui32 ( rgb, r0d+r, c0d+c, &rgbPix );
                }
             break;

           case GAN_UINT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   rgbPix.R = rgbPix.G = rgbPix.B =
                   gan_image_get_pix_gl_ui32 ( grey, r0s+r, c0s+c );
                   gan_image_set_pix_rgb_ui32 ( rgb, r0d+r, c0d+c, &rgbPix );
                }
             break;

           case GAN_FLOAT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   rgbPix.R = rgbPix.G = rgbPix.B =
                   gan_pixel_f32_to_ui32(gan_image_get_pix_gl_f32 (grey, r0s+r, c0s+c));
                   gan_image_set_pix_rgb_ui32 ( rgb, r0d+r, c0d+c, &rgbPix );
                }
             break;
             
           case GAN_FLOAT64:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   rgbPix.R = rgbPix.G = rgbPix.B =
                   gan_pixel_f64_to_ui32(gan_image_get_pix_gl_f64 (grey, r0s+r, c0s+c));
                   gan_image_set_pix_rgb_ui32 ( rgb, r0d+r, c0d+c, &rgbPix );
                }
             break;
             
           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_grey_to_rgb",
                                GAN_ERROR_NOT_IMPLEMENTED, "" );
             return NULL;
        }
        break;
        
      case GAN_FLOAT32:
        switch ( grey->type )
        {
           Gan_RGBPixel_f32 rgbPix;

           case GAN_UINT8:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   rgbPix.R = rgbPix.G = rgbPix.B =
                   gan_pixel_ui8_to_f32(gan_image_get_pix_gl_ui8 (grey, r0s+r, c0s+c));
                   gan_image_set_pix_rgb_f32 ( rgb, r0d+r, c0d+c, &rgbPix );
                }
             break;

           case GAN_UINT16:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   rgbPix.R = rgbPix.G = rgbPix.B = gan_pixel_ui16_to_f32(gan_image_get_pix_gl_ui16 (grey, r0s+r, c0s+c));
                   gan_image_set_pix_rgb_f32 ( rgb, r0d+r, c0d+c, &rgbPix );
                }
             break;

           case GAN_UINT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   rgbPix.R = rgbPix.G = rgbPix.B = gan_pixel_ui32_to_f32(gan_image_get_pix_gl_ui32 (grey, r0s+r, c0s+c));
                   gan_image_set_pix_rgb_f32 ( rgb, r0d+r, c0d+c, &rgbPix );
                }
             break;

           case GAN_FLOAT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   rgbPix.R = rgbPix.G = rgbPix.B = gan_image_get_pix_gl_f32 ( grey, r0s+r, c0s+c );
                   gan_image_set_pix_rgb_f32 ( rgb, r0d+r, c0d+c, &rgbPix );
                }
             break;
             
           case GAN_FLOAT64:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   rgbPix.R = rgbPix.G = rgbPix.B = (gan_float32)gan_image_get_pix_gl_f64 ( grey, r0s+r, c0s+c );
                   gan_image_set_pix_rgb_f32 ( rgb, r0d+r, c0d+c, &rgbPix );
                }
             break;
             
           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_grey_to_rgb",
                                GAN_ERROR_NOT_IMPLEMENTED, "" );
             return NULL;
        }
        break;
        
      case GAN_FLOAT64:
        switch ( grey->type )
        {
           Gan_RGBPixel_f64 rgbPix;

           case GAN_UINT8:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   rgbPix.R = rgbPix.G = rgbPix.B = gan_pixel_ui8_to_f64(gan_image_get_pix_gl_ui8 (grey, r0s+r, c0s+c));
                   gan_image_set_pix_rgb_f64 ( rgb, r0d+r, c0d+c, &rgbPix );
                }
             break;

           case GAN_UINT16:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   rgbPix.R = rgbPix.G = rgbPix.B = gan_pixel_ui16_to_f64(gan_image_get_pix_gl_ui16 (grey, r0s+r, c0s+c));
                   gan_image_set_pix_rgb_f64 ( rgb, r0d+r, c0d+c, &rgbPix );
                }
             break;

           case GAN_UINT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   rgbPix.R = rgbPix.G = rgbPix.B = gan_pixel_ui32_to_f64(gan_image_get_pix_gl_ui32 (grey, r0s+r, c0s+c));
                   gan_image_set_pix_rgb_f64 ( rgb, r0d+r, c0d+c, &rgbPix );
                }
             break;

           case GAN_FLOAT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   rgbPix.R = rgbPix.G = rgbPix.B = (gan_float64)gan_image_get_pix_gl_f32 ( grey, r0s+r, c0s+c );
                   gan_image_set_pix_rgb_f64 ( rgb, r0d+r, c0d+c, &rgbPix );
                }
             break;
             
           case GAN_FLOAT64:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   rgbPix.R = rgbPix.G = rgbPix.B = gan_image_get_pix_gl_f64 ( grey, r0s+r, c0s+c );
                   gan_image_set_pix_rgb_f64 ( rgb, r0d+r, c0d+c, &rgbPix );
                }
             break;
             
           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_grey_to_rgb", GAN_ERROR_NOT_IMPLEMENTED, "" );
             return NULL;
        }
        break;
        
      default:
        gan_err_flush_trace();
        gan_err_register ( "extract_grey_to_rgb", GAN_ERROR_NOT_IMPLEMENTED, "" );
        return NULL;
   }
             
   return rgb;
}

static Gan_Image *
 extract_grey_to_rgba ( const Gan_Image *grey, unsigned r0s, unsigned c0s,
                        unsigned height, unsigned width,
                        Gan_Image *rgba, unsigned r0d, unsigned c0d )
{
   unsigned int r, c;

   switch ( rgba->type )
   {
      case GAN_UINT8:
        switch ( grey->type )
        {
           Gan_RGBAPixel_ui8 rgbaPix;

           case GAN_UINT8:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   rgbaPix.R = rgbaPix.G = rgbaPix.B =
                       gan_image_get_pix_gl_ui8 ( grey, r0s+r, c0s+c );
                   rgbaPix.A = GAN_UINT8_MAX;
                   gan_image_set_pix_rgba_ui8 ( rgba, r0d+r, c0d+c, &rgbaPix );
                }
             break;

           case GAN_UINT16:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   rgbaPix.R = rgbaPix.G = rgbaPix.B =
                   gan_pixel_ui16_to_ui8(gan_image_get_pix_gl_ui16 (grey,
                                                               r0s+r, c0s+c));
                   rgbaPix.A = GAN_UINT8_MAX;
                   gan_image_set_pix_rgba_ui8 ( rgba, r0d+r, c0d+c, &rgbaPix );
                }
             break;

           case GAN_UINT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   rgbaPix.R = rgbaPix.G = rgbaPix.B =
                   gan_pixel_ui32_to_ui8(gan_image_get_pix_gl_ui32 (grey,
                                                               r0s+r, c0s+c));
                   rgbaPix.A = GAN_UINT8_MAX;
                   gan_image_set_pix_rgba_ui8 ( rgba, r0d+r, c0d+c, &rgbaPix );
                }
             break;

           case GAN_FLOAT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   rgbaPix.R = rgbaPix.G = rgbaPix.B =
                   gan_pixel_f32_to_ui8(gan_image_get_pix_gl_f32 (grey,
                                                             r0s+r, c0s+c));
                   rgbaPix.A = GAN_UINT8_MAX;
                   gan_image_set_pix_rgba_ui8 ( rgba, r0d+r, c0d+c, &rgbaPix );
                }
             break;
             
           case GAN_FLOAT64:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   rgbaPix.R = rgbaPix.G = rgbaPix.B =
                   gan_pixel_f64_to_ui8(gan_image_get_pix_gl_f64 (grey,
                                                             r0s+r, c0s+c));
                   rgbaPix.A = GAN_UINT8_MAX;
                   gan_image_set_pix_rgba_ui8 ( rgba, r0d+r, c0d+c, &rgbaPix );
                }
             break;
             
           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_grey_to_rgba",
                                GAN_ERROR_NOT_IMPLEMENTED, "" );
             return NULL;
        }
        break;
        
      case GAN_UINT16:
        switch ( grey->type )
        {
           Gan_RGBAPixel_ui16 rgbaPix;

           case GAN_BOOL:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   if( gan_image_get_pix_b(grey, r0s+r, c0s+c) == GAN_TRUE)
                   {
                     rgbaPix.R = rgbaPix.G = rgbaPix.B = rgbaPix.A = GAN_UINT16_MAX;
                   }
                   else
                   {
                     rgbaPix.R = rgbaPix.G = rgbaPix.B = 0;
                     rgbaPix.A = GAN_UINT16_MAX;
                   }

                   gan_image_set_pix_rgba_ui16 ( rgba, r0d+r, c0d+c, &rgbaPix );
                }
             break;

           case GAN_UINT8:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   rgbaPix.R = rgbaPix.G = rgbaPix.B =
                   gan_pixel_ui8_to_ui16(gan_image_get_pix_gl_ui8 (grey,
                                                               r0s+r, c0s+c));
                   rgbaPix.A = GAN_UINT16_MAX;
                   gan_image_set_pix_rgba_ui16 ( rgba, r0d+r, c0d+c, &rgbaPix );
                }
             break;

           case GAN_UINT16:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   rgbaPix.R = rgbaPix.G = rgbaPix.B =
                   gan_image_get_pix_gl_ui16 ( grey, r0s+r, c0s+c);
                   rgbaPix.A = GAN_UINT16_MAX;
                   gan_image_set_pix_rgba_ui16 ( rgba, r0d+r, c0d+c, &rgbaPix );
                }
             break;

           case GAN_UINT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   rgbaPix.R = rgbaPix.G = rgbaPix.B =
                   gan_pixel_ui32_to_ui16(gan_image_get_pix_gl_ui32 (grey,
                                                               r0s+r, c0s+c));
                   rgbaPix.A = GAN_UINT16_MAX;
                   gan_image_set_pix_rgba_ui16 ( rgba, r0d+r, c0d+c, &rgbaPix );
                }
             break;

           case GAN_FLOAT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   rgbaPix.R = rgbaPix.G = rgbaPix.B =
                   gan_pixel_f32_to_ui16(gan_image_get_pix_gl_f32 (grey,
                                                             r0s+r, c0s+c));
                   rgbaPix.A = GAN_UINT16_MAX;
                   gan_image_set_pix_rgba_ui16 ( rgba, r0d+r, c0d+c, &rgbaPix );
                }
             break;
             
           case GAN_FLOAT64:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   rgbaPix.R = rgbaPix.G = rgbaPix.B =
                   gan_pixel_f64_to_ui16(gan_image_get_pix_gl_f64 (grey,
                                                             r0s+r, c0s+c));
                   rgbaPix.A = GAN_UINT16_MAX;
                   gan_image_set_pix_rgba_ui16 ( rgba, r0d+r, c0d+c, &rgbaPix );
                }
             break;
             
           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_grey_to_rgba",
                                GAN_ERROR_NOT_IMPLEMENTED, "" );
             return NULL;
        }
        break;
        
      case GAN_INT32:
        switch ( grey->type )
        {
           Gan_RGBAPixel_i32 rgbaPix;

           case GAN_INT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   rgbaPix.R = rgbaPix.G = rgbaPix.B =
                   gan_image_get_pix_gl_i32 ( grey, r0s+r, c0s+c );
                   rgbaPix.A = INT_MAX;
                   gan_image_set_pix_rgba_i32 ( rgba, r0d+r, c0d+c, &rgbaPix );
                }
             break;

           case GAN_FLOAT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   rgbaPix.R = rgbaPix.G = rgbaPix.B =
                   gan_pixel_f32_to_i32(gan_image_get_pix_gl_f32 (grey,
                                                            r0s+r, c0s+c));
                   rgbaPix.A = INT_MAX;
                   gan_image_set_pix_rgba_i32 ( rgba, r0d+r, c0d+c, &rgbaPix );
                }
             break;
             
           case GAN_FLOAT64:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   rgbaPix.R = rgbaPix.G = rgbaPix.B =
                   gan_pixel_f64_to_i32(gan_image_get_pix_gl_f64 (grey,
                                                            r0s+r, c0s+c));
                   rgbaPix.A = INT_MAX;
                   gan_image_set_pix_rgba_i32 ( rgba, r0d+r, c0d+c, &rgbaPix );
                }
             break;
             
           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_grey_to_rgba",
                                GAN_ERROR_NOT_IMPLEMENTED, "" );
             return NULL;
        }
        break;
        
      case GAN_UINT32:
        switch ( grey->type )
        {
           Gan_RGBAPixel_ui32 rgbaPix;

           case GAN_UINT8:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   rgbaPix.R = rgbaPix.G = rgbaPix.B =
                   gan_pixel_ui8_to_ui32(gan_image_get_pix_gl_ui8 (grey,
                                                               r0s+r, c0s+c));
                   rgbaPix.A = GAN_UINT32_MAX;
                   gan_image_set_pix_rgba_ui32 ( rgba, r0d+r, c0d+c, &rgbaPix );
                }
             break;

           case GAN_UINT16:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   rgbaPix.R = rgbaPix.G = rgbaPix.B =
                   gan_pixel_ui16_to_ui32(gan_image_get_pix_gl_ui16 (grey,
                                                               r0s+r, c0s+c));
                   rgbaPix.A = GAN_UINT32_MAX;
                   gan_image_set_pix_rgba_ui32 ( rgba, r0d+r, c0d+c, &rgbaPix );
                }
             break;

           case GAN_UINT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   rgbaPix.R = rgbaPix.G = rgbaPix.B =
                   gan_image_get_pix_gl_ui32 ( grey, r0s+r, c0s+c );
                   rgbaPix.A = GAN_UINT32_MAX;
                   gan_image_set_pix_rgba_ui32 ( rgba, r0d+r, c0d+c, &rgbaPix );
                }
             break;

           case GAN_FLOAT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   rgbaPix.R = rgbaPix.G = rgbaPix.B =
                   gan_pixel_f32_to_ui32(gan_image_get_pix_gl_f32 (grey,
                                                             r0s+r, c0s+c));
                   rgbaPix.A = GAN_UINT32_MAX;
                   gan_image_set_pix_rgba_ui32 ( rgba, r0d+r, c0d+c, &rgbaPix );
                }
             break;
             
           case GAN_FLOAT64:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   rgbaPix.R = rgbaPix.G = rgbaPix.B =
                   gan_pixel_f64_to_ui32(gan_image_get_pix_gl_f64 (grey,
                                                             r0s+r, c0s+c));
                   rgbaPix.A = GAN_UINT32_MAX;
                   gan_image_set_pix_rgba_ui32 ( rgba, r0d+r, c0d+c, &rgbaPix );
                }
             break;
             
           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_grey_to_rgba",
                                GAN_ERROR_NOT_IMPLEMENTED, "" );
             return NULL;
        }
        break;
        
      case GAN_FLOAT32:
        switch ( grey->type )
        {
           Gan_RGBAPixel_f32 rgbaPix;

           case GAN_UINT8:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   rgbaPix.R = rgbaPix.G = rgbaPix.B =
                   gan_pixel_ui8_to_f32(gan_image_get_pix_gl_ui8 (grey,
                                                              r0s+r, c0s+c));
                   rgbaPix.A = 1.0F;
                   gan_image_set_pix_rgba_f32 ( rgba, r0d+r, c0d+c, &rgbaPix );
                }
             break;

           case GAN_UINT16:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   rgbaPix.R = rgbaPix.G = rgbaPix.B =
                   gan_pixel_ui16_to_f32(gan_image_get_pix_gl_ui16 (grey,
                                                              r0s+r, c0s+c));
                   rgbaPix.A = 1.0F;
                   gan_image_set_pix_rgba_f32 ( rgba, r0d+r, c0d+c, &rgbaPix );
                }
             break;

           case GAN_UINT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   rgbaPix.R = rgbaPix.G = rgbaPix.B =
                   gan_pixel_ui32_to_f32(gan_image_get_pix_gl_ui32 (grey,
                                                              r0s+r, c0s+c));
                   rgbaPix.A = 1.0F;
                   gan_image_set_pix_rgba_f32 ( rgba, r0d+r, c0d+c, &rgbaPix );
                }
             break;

           case GAN_FLOAT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   rgbaPix.R = rgbaPix.G = rgbaPix.B =
                   gan_image_get_pix_gl_f32 ( grey, r0s+r, c0s+c );
                   rgbaPix.A = 1.0F;
                   gan_image_set_pix_rgba_f32 ( rgba, r0d+r, c0d+c, &rgbaPix );
                }
             break;
             
           case GAN_FLOAT64:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   rgbaPix.R = rgbaPix.G = rgbaPix.B =
                   (gan_float32)gan_image_get_pix_gl_f64 ( grey, r0s+r, c0s+c );
                   rgbaPix.A = 1.0F;
                   gan_image_set_pix_rgba_f32 ( rgba, r0d+r, c0d+c, &rgbaPix );
                }
             break;
             
           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_grey_to_rgba",
                                GAN_ERROR_NOT_IMPLEMENTED, "" );
             return NULL;
        }
        break;
        
      case GAN_FLOAT64:
        switch ( grey->type )
        {
           Gan_RGBAPixel_f64 rgbaPix;

           case GAN_UINT8:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   rgbaPix.R = rgbaPix.G = rgbaPix.B =
                   gan_pixel_ui8_to_f64(gan_image_get_pix_gl_ui8 (grey,
                                                              r0s+r, c0s+c));
                   rgbaPix.A = 1.0;
                   gan_image_set_pix_rgba_f64 ( rgba, r0d+r, c0d+c, &rgbaPix );
                }
             break;

           case GAN_UINT16:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   rgbaPix.R = rgbaPix.G = rgbaPix.B =
                   gan_pixel_ui16_to_f64(gan_image_get_pix_gl_ui16 (grey,
                                                              r0s+r, c0s+c));
                   rgbaPix.A = 1.0;
                   gan_image_set_pix_rgba_f64 ( rgba, r0d+r, c0d+c, &rgbaPix );
                }
             break;

           case GAN_UINT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   rgbaPix.R = rgbaPix.G = rgbaPix.B =
                   gan_pixel_ui32_to_f64(gan_image_get_pix_gl_ui32 (grey,
                                                              r0s+r, c0s+c));
                   rgbaPix.A = 1.0;
                   gan_image_set_pix_rgba_f64 ( rgba, r0d+r, c0d+c, &rgbaPix );
                }
             break;

           case GAN_FLOAT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   rgbaPix.R = rgbaPix.G = rgbaPix.B =
                   (gan_float64)gan_image_get_pix_gl_f32 ( grey, r0s+r, c0s+c );
                   rgbaPix.A = 1.0;
                   gan_image_set_pix_rgba_f64 ( rgba, r0d+r, c0d+c, &rgbaPix );
                }
             break;
             
           case GAN_FLOAT64:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   rgbaPix.R = rgbaPix.G = rgbaPix.B =
                   gan_image_get_pix_gl_f64 ( grey, r0s+r, c0s+c );
                   rgbaPix.A = 1.0;
                   gan_image_set_pix_rgba_f64 ( rgba, r0d+r, c0d+c, &rgbaPix );
                }
             break;
             
           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_grey_to_rgba",
                                GAN_ERROR_NOT_IMPLEMENTED, "" );
             return NULL;
        }
        break;
        
      default:
        gan_err_flush_trace();
        gan_err_register ( "extract_grey_to_rgba",
                           GAN_ERROR_NOT_IMPLEMENTED, "" );
        return NULL;
   }
             
   return rgba;
}

static Gan_Image *
 extract_gla_to_grey ( const Gan_Image *gla, unsigned r0s, unsigned c0s,
                       unsigned height, unsigned width,
                       Gan_Image *grey, unsigned r0d, unsigned c0d )
{
   unsigned int r, c;

   gan_err_test_ptr ( gla->format == GAN_GREY_LEVEL_ALPHA_IMAGE &&
                      grey->format == GAN_GREY_LEVEL_IMAGE,
                      "extract_gla_to_grey", GAN_ERROR_INCOMPATIBLE, "" );

   switch ( gla->type )
   {
      case GAN_UINT8:
        switch ( grey->type )
        {
           case GAN_UINT8:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_ui8 ( grey, r0d+r, c0d+c,
                                        gla->row_data.gla.ui8[r0s+r][c0s+c].I );
             break;

           case GAN_UINT16:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_ui16 ( grey, r0d+r, c0d+c,
                               gan_pixel_ui8_to_ui16 (
                                   gla->row_data.gla.ui8[r0s+r][c0s+c].I ) );
             break;

           case GAN_UINT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_ui32 ( grey, r0d+r, c0d+c,
                               gan_pixel_ui8_to_ui32 (
                                   gla->row_data.gla.ui8[r0s+r][c0s+c].I ) );
             break;

           case GAN_BOOL:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_b ( grey, r0d+r, c0d+c,
                                         gla->row_data.gla.ui8[r0s+r][c0s+c].I
                                         ? GAN_TRUE : GAN_FALSE );
             break;

           case GAN_FLOAT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_f32 ( grey, r0d+r, c0d+c,
                               gan_pixel_ui8_to_f32 (
                                   gla->row_data.gla.ui8[r0s+r][c0s+c].I ) );
             break;

           case GAN_FLOAT64:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_f64 ( grey, r0d+r, c0d+c,
                               gan_pixel_ui8_to_f64 (
                                   gla->row_data.gla.ui8[r0s+r][c0s+c].I ) );
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_gla_to_grey",
                                GAN_ERROR_NOT_IMPLEMENTED, "" );
             return NULL;
        }
        break;

      case GAN_UINT16:
        switch ( grey->type )
        {
           case GAN_UINT8:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_ui8 ( grey, r0d+r, c0d+c,
                               gan_pixel_ui16_to_ui8 (
                                   gla->row_data.gla.ui16[r0s+r][c0s+c].I ) );
             break;

           case GAN_UINT16:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_ui16 ( grey, r0d+r, c0d+c,
                                        gla->row_data.gla.ui16[r0s+r][c0s+c].I );
             break;

           case GAN_UINT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_ui32 ( grey, r0d+r, c0d+c,
                               gan_pixel_ui16_to_ui32 (
                                   gla->row_data.gla.ui16[r0s+r][c0s+c].I ) );
             break;

           case GAN_BOOL:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_b ( grey, r0d+r, c0d+c,
                                         gla->row_data.gla.ui16[r0s+r][c0s+c].I
                                         ? GAN_TRUE : GAN_FALSE );
             break;

           case GAN_FLOAT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_f32 ( grey, r0d+r, c0d+c,
                               gan_pixel_ui16_to_f32 (
                                   gla->row_data.gla.ui16[r0s+r][c0s+c].I ) );
             break;

           case GAN_FLOAT64:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_f64 ( grey, r0d+r, c0d+c,
                               gan_pixel_ui16_to_f64 (
                                   gla->row_data.gla.ui16[r0s+r][c0s+c].I ) );
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_gla_to_grey",
                                GAN_ERROR_NOT_IMPLEMENTED, "" );
             return NULL;
        }
        break;

      case GAN_UINT32:
        switch ( grey->type )
        {
           case GAN_UINT8:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_ui8 ( grey, r0d+r, c0d+c,
                               gan_pixel_ui32_to_ui8 (             
                                   gla->row_data.gla.ui32[r0s+r][c0s+c].I ) );
             break;

           case GAN_UINT16:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_ui16 ( grey, r0d+r, c0d+c,
                               gan_pixel_ui32_to_ui16 (
                                   gla->row_data.gla.ui32[r0s+r][c0s+c].I ) );
             break;

           case GAN_UINT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_ui32 ( grey, r0d+r, c0d+c,
                                        gla->row_data.gla.ui32[r0s+r][c0s+c].I );
             break;

           case GAN_BOOL:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_b ( grey, r0d+r, c0d+c,
                                         gla->row_data.gla.ui32[r0s+r][c0s+c].I
                                         ? GAN_TRUE : GAN_FALSE );
             break;

           case GAN_FLOAT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_f32 ( grey, r0d+r, c0d+c,
                               gan_pixel_ui32_to_f32 (
                                   gla->row_data.gla.ui32[r0s+r][c0s+c].I ) );
             break;

           case GAN_FLOAT64:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_f64 ( grey, r0d+r, c0d+c,
                               gan_pixel_ui32_to_f64 (
                                   gla->row_data.gla.ui32[r0s+r][c0s+c].I ) );
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_gla_to_grey",
                                GAN_ERROR_NOT_IMPLEMENTED, "" );
             return NULL;
        }
        break;

      case GAN_FLOAT32:
        switch ( grey->type )
        {
           case GAN_UINT8:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_ui8 ( grey, r0d+r, c0d+c,
                               gan_pixel_f32_to_ui8 (             
                                   gla->row_data.gla.f32[r0s+r][c0s+c].I ) );
             break;

           case GAN_UINT16:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_ui16 ( grey, r0d+r, c0d+c,
                               gan_pixel_f32_to_ui16 (
                                   gla->row_data.gla.f32[r0s+r][c0s+c].I ) );
             break;

           case GAN_UINT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_ui32 ( grey, r0d+r, c0d+c,
                               gan_pixel_f32_to_ui32 (
                                   gla->row_data.gla.f32[r0s+r][c0s+c].I ) );
             break;

           case GAN_BOOL:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_b ( grey, r0d+r, c0d+c,
                                         (gla->row_data.gla.f32[r0s+r][c0s+c].I
                                          == 0.0F) ? GAN_FALSE : GAN_TRUE );
             break;

           case GAN_FLOAT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_f32(grey, r0d+r, c0d+c,
                                          gla->row_data.gla.f32[r0s+r][c0s+c].I);
             break;

           case GAN_FLOAT64:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_f64(grey, r0d+r, c0d+c, (gan_float64)
                                          gla->row_data.gla.f32[r0s+r][c0s+c].I);
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_gla_to_grey",
                                GAN_ERROR_NOT_IMPLEMENTED, "" );
             return NULL;
        }
        break;

      case GAN_FLOAT64:
        switch ( grey->type )
        {
           case GAN_UINT8:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_ui8 ( grey, r0d+r, c0d+c,
                               gan_pixel_f64_to_ui8 (
                                   gla->row_data.gla.f64[r0s+r][c0s+c].I ) );
             break;

           case GAN_UINT16:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_ui16 ( grey, r0d+r, c0d+c,
                               gan_pixel_f64_to_ui16 (
                                   gla->row_data.gla.f64[r0s+r][c0s+c].I ) );
             break;

           case GAN_UINT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_ui32 ( grey, r0d+r, c0d+c,
                               gan_pixel_f64_to_ui32 (
                                   gla->row_data.gla.f64[r0s+r][c0s+c].I ) );
             break;

           case GAN_BOOL:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_b ( grey, r0d+r, c0d+c,
                                         (gla->row_data.gla.f64[r0s+r][c0s+c].I
                                          == 0.0) ? GAN_FALSE : GAN_TRUE );
             break;

           case GAN_FLOAT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_f32(grey, r0d+r, c0d+c, (gan_float32)
                                          gla->row_data.gla.f64[r0s+r][c0s+c].I);
             break;

           case GAN_FLOAT64:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_f64(grey, r0d+r, c0d+c,
                                          gla->row_data.gla.f64[r0s+r][c0s+c].I);
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_gla_to_grey",
                                GAN_ERROR_NOT_IMPLEMENTED, "" );
             return NULL;
        }
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "extract_gla_to_grey", GAN_ERROR_NOT_IMPLEMENTED,
                           "" );
        return NULL;
   }

   return grey;
}

static Gan_Image *
 extract_rgb_to_grey ( const Gan_Image *rgb, unsigned r0s, unsigned c0s,
                       unsigned height, unsigned width,
                       Gan_Image *grey, unsigned r0d, unsigned c0d )
{
   unsigned int r, c;
   
   gan_err_test_ptr ( rgb->format == GAN_RGB_COLOUR_IMAGE && grey->format == GAN_GREY_LEVEL_IMAGE,
                      "extract_rgb_to_grey", GAN_ERROR_INCOMPATIBLE, "" );

   switch ( rgb->type )
   {
      case GAN_UINT8:
        switch ( grey->type )
        {
           case GAN_UINT8:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_ui8 ( grey, r0d+r, c0d+c,
                       gan_pixel_rgbui8_to_yui8 (
                           gan_image_get_pixptr_rgb_ui8(rgb, r0s+r, c0s+c)) );
             break;

           case GAN_UINT16:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_ui16 ( grey, r0d+r, c0d+c,
                       gan_pixel_rgbui8_to_yui16 (
                           gan_image_get_pixptr_rgb_ui8(rgb, r0s+r, c0s+c)) );
             break;

           case GAN_UINT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_ui32 ( grey, r0d+r, c0d+c,
                       gan_pixel_rgbui8_to_yui32 (
                           gan_image_get_pixptr_rgb_ui8(rgb, r0s+r, c0s+c)) );
             break;

           case GAN_BOOL:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_b ( grey, r0d+r, c0d+c,
                       gan_pixel_rgbui8_to_yui16 (
                                gan_image_get_pixptr_rgb_ui8(rgb, r0s+r, c0s+c)) ? GAN_TRUE : GAN_FALSE );
             break;

           case GAN_FLOAT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_f32 ( grey, r0d+r, c0d+c,
                       gan_pixel_ui16_to_f32 (
                           gan_pixel_rgbui8_to_yui16 (
                               gan_image_get_pixptr_rgb_ui8(rgb,r0s+r,c0s+c))));

             break;

           case GAN_FLOAT64:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_f64 ( grey, r0d+r, c0d+c,
                       gan_pixel_ui16_to_f64 (
                           gan_pixel_rgbui8_to_yui16 (
                               gan_image_get_pixptr_rgb_ui8(rgb,r0s+r,c0s+c))));

             break;

           case GAN_UINT10:
           {
              Gan_YXPixel_ui10 yxPixel_ui10;

              yxPixel_ui10.X = 0;
              for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   yxPixel_ui10.Y = gan_pixel_rgbui8_to_yui16(gan_image_get_pixptr_rgb_ui8(rgb, r0s+r, c0s+c)) >> 6;
                   gan_image_set_pix_gl_ui10 ( grey, r0d+r, c0d+c, &yxPixel_ui10 );
                }
           }
           break;

           case GAN_UINT12:
           {
              Gan_YXPixel_ui12 yxPixel_ui12;

              yxPixel_ui12.X = 0;
              for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   yxPixel_ui12.Y = gan_pixel_rgbui8_to_yui16(gan_image_get_pixptr_rgb_ui8(rgb, r0s+r, c0s+c)) >> 4;
                   gan_image_set_pix_gl_ui12 ( grey, r0d+r, c0d+c, &yxPixel_ui12 );
                }
           }
           break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_rgb_to_grey",
                                GAN_ERROR_NOT_IMPLEMENTED, "" );
             return NULL;
        }
        break;

      case GAN_UINT16:
        switch ( grey->type )
        {
           case GAN_UINT8:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_ui8 ( grey, r0d+r, c0d+c,
                       gan_pixel_rgbui16_to_yui8 (
                           gan_image_get_pixptr_rgb_ui16(rgb, r0s+r, c0s+c)) );
             break;

           case GAN_UINT16:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_ui16 ( grey, r0d+r, c0d+c,
                       gan_pixel_rgbui16_to_yui16 (
                           gan_image_get_pixptr_rgb_ui16(rgb, r0s+r, c0s+c)) );
             break;

           case GAN_UINT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_ui32 ( grey, r0d+r, c0d+c,
                       gan_pixel_ui16_to_ui32 (
                       gan_pixel_rgbui16_to_yui16 (
                           gan_image_get_pixptr_rgb_ui16(rgb, r0s+r, c0s+c)) ) );
             break;

           case GAN_BOOL:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_b ( grey, r0d+r, c0d+c,
                         gan_pixel_rgbui16_to_yui16 (
                             gan_image_get_pixptr_rgb_ui16(rgb, r0s+r, c0s+c))
                             ? GAN_TRUE : GAN_FALSE );
             break;

           case GAN_FLOAT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_f32 ( grey, r0d+r, c0d+c,
                       gan_pixel_ui16_to_f32 (
                           gan_pixel_rgbui16_to_yui16 (
                               gan_image_get_pixptr_rgb_ui16(rgb,r0s+r,c0s+c))));

             break;

           case GAN_FLOAT64:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_f64 ( grey, r0d+r, c0d+c,
                        gan_pixel_ui16_to_f64 (
                           gan_pixel_rgbui16_to_yui16 (
                               gan_image_get_pixptr_rgb_ui16(rgb,r0s+r,c0s+c))));

             break;

           case GAN_UINT10:
           {
              Gan_YXPixel_ui10 yxPixel_ui10;

              yxPixel_ui10.X = 0;
              for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   yxPixel_ui10.Y = gan_pixel_rgbui16_to_yui16(gan_image_get_pixptr_rgb_ui16(rgb, r0s+r, c0s+c)) >> 6;
                   gan_image_set_pix_gl_ui10 ( grey, r0d+r, c0d+c, &yxPixel_ui10 );
                }
           }
           break;

           case GAN_UINT12:
           {
              Gan_YXPixel_ui12 yxPixel_ui12;

              yxPixel_ui12.X = 0;
              for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   yxPixel_ui12.Y = gan_pixel_rgbui16_to_yui16(gan_image_get_pixptr_rgb_ui16(rgb, r0s+r, c0s+c)) >> 4;
                   gan_image_set_pix_gl_ui12 ( grey, r0d+r, c0d+c, &yxPixel_ui12 );
                }
           }
           break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_rgb_to_grey",
                                GAN_ERROR_NOT_IMPLEMENTED, "" );
             return NULL;
        }
        break;

      case GAN_UINT32:
        switch ( grey->type )
        {
           case GAN_UINT8:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_ui8 ( grey, r0d+r, c0d+c,
                       gan_pixel_f32_to_ui8(gan_pixel_rgbui32_to_yf32(gan_image_get_pixptr_rgb_ui32(rgb,r0s+r,c0s+c))));
             break;

           case GAN_UINT16:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_ui16 ( grey, r0d+r, c0d+c,
                       gan_pixel_f32_to_ui16(gan_pixel_rgbui32_to_yf32(gan_image_get_pixptr_rgb_ui32(rgb,r0s+r,c0s+c))));
             break;

           case GAN_UINT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_ui32 ( grey, r0d+r, c0d+c,
                       gan_pixel_f32_to_ui32 (
                           gan_pixel_rgbui32_to_yf32 (
                               gan_image_get_pixptr_rgb_ui32(rgb,r0s+r,c0s+c))));
             break;

           case GAN_BOOL:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_b ( grey, r0d+r, c0d+c,
                         gan_pixel_rgbui32_to_yf32 (
                             gan_image_get_pixptr_rgb_ui32(rgb, r0s+r, c0s+c))
                             ? GAN_TRUE : GAN_FALSE );
             break;

           case GAN_FLOAT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_f32 ( grey, r0d+r, c0d+c,
                           gan_pixel_rgbui32_to_yf32 (
                               gan_image_get_pixptr_rgb_ui32(rgb,r0s+r,c0s+c)));

             break;

           case GAN_FLOAT64:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_f64 ( grey, r0d+r, c0d+c, (gan_float64)
                           gan_pixel_rgbui32_to_yf32 (
                               gan_image_get_pixptr_rgb_ui32(rgb,r0s+r,c0s+c)));

             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_rgb_to_grey",
                                GAN_ERROR_NOT_IMPLEMENTED, "" );
             return NULL;
        }
        break;

      case GAN_FLOAT32:
        switch ( grey->type )
        {
           case GAN_UINT8:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_ui8 ( grey, r0d+r, c0d+c,
                           gan_pixel_f32_to_ui8(gan_pixel_rgbf32_to_yf32(gan_image_get_pixptr_rgb_f32(rgb,r0s+r,c0s+c))));
             break;

           case GAN_UINT16:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_ui16 ( grey, r0d+r, c0d+c,
                           gan_pixel_f32_to_ui16(gan_pixel_rgbf32_to_yf32(gan_image_get_pixptr_rgb_f32(rgb,r0s+r,c0s+c))));
             break;

           case GAN_UINT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_ui32 ( grey, r0d+r, c0d+c,
                           gan_pixel_f32_to_ui32(gan_pixel_rgbf32_to_yf32(gan_image_get_pixptr_rgb_f32(rgb,r0s+r,c0s+c))));
             break;

           case GAN_BOOL:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_b ( grey, r0d+r, c0d+c,
                                         gan_pixel_rgbf32_to_yf32(gan_image_get_pixptr_rgb_f32(rgb, r0s+r, c0s+c) ) ? GAN_TRUE : GAN_FALSE );
             break;

           case GAN_FLOAT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_f32 ( grey, r0d+r, c0d+c,
                                              gan_pixel_rgbf32_to_yf32(gan_image_get_pixptr_rgb_f32(rgb, r0s+r, c0s+c)));
             break;

           case GAN_FLOAT64:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_f64 ( grey, r0d+r, c0d+c, (gan_float64)
                           gan_pixel_rgbf32_to_yf32 (
                               gan_image_get_pixptr_rgb_f32(rgb, r0s+r, c0s+c)));
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_rgb_to_grey",
                                GAN_ERROR_NOT_IMPLEMENTED, "" );
             return NULL;
        }
        break;

      case GAN_FLOAT64:
        switch ( grey->type )
        {
           case GAN_UINT8:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_ui8 ( grey, r0d+r, c0d+c,
                           gan_pixel_f64_to_ui8 (
                               gan_pixel_rgbd_to_yd (
                                gan_image_get_pixptr_rgb_f64(rgb,r0s+r,c0s+c))));
             break;

           case GAN_UINT16:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_ui16 ( grey, r0d+r, c0d+c,
                           gan_pixel_f64_to_ui16 (
                               gan_pixel_rgbd_to_yd (
                                gan_image_get_pixptr_rgb_f64(rgb,r0s+r,c0s+c))));
             break;

           case GAN_INT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_i32 ( grey, r0d+r, c0d+c,
                           gan_pixel_f64_to_i32 (
                               gan_pixel_rgbd_to_yd (
                                gan_image_get_pixptr_rgb_f64(rgb,r0s+r,c0s+c))));
             break;

           case GAN_UINT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_ui32 ( grey, r0d+r, c0d+c,
                           gan_pixel_f64_to_ui32 (
                               gan_pixel_rgbd_to_yd (
                                gan_image_get_pixptr_rgb_f64(rgb,r0s+r,c0s+c))));
             break;

           case GAN_BOOL:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_b ( grey, r0d+r, c0d+c,
                           gan_pixel_rgbd_to_yd (
                               gan_image_get_pixptr_rgb_f64(rgb, r0s+r, c0s+c) )
                           ? GAN_TRUE : GAN_FALSE );
             break;

           case GAN_FLOAT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_f32 ( grey, r0d+r, c0d+c, (gan_float32)
                           gan_pixel_rgbd_to_yd (
                               gan_image_get_pixptr_rgb_f64(rgb, r0s+r, c0s+c)));
             break;

           case GAN_FLOAT64:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_f64 ( grey, r0d+r, c0d+c,
                           gan_pixel_rgbd_to_yd (
                               gan_image_get_pixptr_rgb_f64(rgb, r0s+r, c0s+c)));
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_rgb_to_grey",
                                GAN_ERROR_NOT_IMPLEMENTED, "" );
             return NULL;
        }
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "extract_rgb_to_grey", GAN_ERROR_NOT_IMPLEMENTED, "" );
        return NULL;
   }

   return grey;
}

static Gan_Image *
 extract_rgba_to_grey ( const Gan_Image *rgba, unsigned r0s, unsigned c0s,
                        unsigned height, unsigned width,
                        Gan_Image *grey, unsigned r0d, unsigned c0d )
{
   unsigned int r, c;
   
   gan_err_test_ptr ( rgba->format == GAN_RGB_COLOUR_ALPHA_IMAGE &&
                      grey->format == GAN_GREY_LEVEL_IMAGE,
                      "extract_rgba_to_grey", GAN_ERROR_INCOMPATIBLE, "" );
   
   switch ( rgba->type )
   {
      case GAN_UINT8:
        switch ( grey->type )
        {
           case GAN_UINT8:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_ui8 ( grey, r0d+r, c0d+c,
                       gan_pixel_rgbaui8_to_yui8 (gan_image_get_pixptr_rgba_ui8(rgba, r0s+r, c0s+c)) );
             
             break;

           case GAN_UINT10:
           {
              Gan_YXPixel_ui10 yxPixel_ui10;

              yxPixel_ui10.X = 0;
              for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   yxPixel_ui10.Y = gan_pixel_rgbaui8_to_yui8(gan_image_get_pixptr_rgba_ui8(rgba, r0s+r, c0s+c)) << 2;
                   gan_image_set_pix_gl_ui10 ( grey, r0d+r, c0d+c, &yxPixel_ui10 );
                }
           }
           break;

           case GAN_UINT12:
           {
              Gan_YXPixel_ui12 yxPixel_ui12;

              yxPixel_ui12.X = 0;
              for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   yxPixel_ui12.Y = gan_pixel_rgbaui8_to_yui8(gan_image_get_pixptr_rgba_ui8(rgba, r0s+r, c0s+c)) << 4;
                   gan_image_set_pix_gl_ui12 ( grey, r0d+r, c0d+c, &yxPixel_ui12 );
                }
           }
           break;

           case GAN_UINT16:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_ui16 ( grey, r0d+r, c0d+c,
                       gan_pixel_rgbaui8_to_yui16 (
                           gan_image_get_pixptr_rgba_ui8(rgba, r0s+r, c0s+c)) );

             break;

           case GAN_UINT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_ui32 ( grey, r0d+r, c0d+c,
                       gan_pixel_rgbaui8_to_yui32 (
                           gan_image_get_pixptr_rgba_ui8(rgba, r0s+r, c0s+c)) );

             break;

           case GAN_BOOL:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_b ( grey, r0d+r, c0d+c,
                       gan_pixel_rgbaui8_to_yui16 (
                           gan_image_get_pixptr_rgba_ui8(rgba, r0s+r, c0s+c))
                           ? GAN_TRUE : GAN_FALSE );
             break;

           case GAN_FLOAT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_f32 ( grey, r0d+r, c0d+c,
                       gan_pixel_ui16_to_f32 (
                         gan_pixel_rgbaui8_to_yui16 (
                           gan_image_get_pixptr_rgba_ui8(rgba, r0s+r, c0s+c))));

             break;

           case GAN_FLOAT64:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_f64 ( grey, r0d+r, c0d+c,
                       gan_pixel_ui16_to_f64 (
                         gan_pixel_rgbaui8_to_yui16 (
                           gan_image_get_pixptr_rgba_ui8(rgba, r0s+r, c0s+c))));

             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_rgba_to_grey",
                                GAN_ERROR_NOT_IMPLEMENTED, "" );
             return NULL;
        }
        break;

      case GAN_UINT16:
        switch ( grey->type )
        {
           case GAN_UINT8:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_ui8 ( grey, r0d+r, c0d+c,
                       gan_pixel_rgbaui16_to_yui8 (
                           gan_image_get_pixptr_rgba_ui16(rgba, r0s+r, c0s+c)) );
             break;

           case GAN_UINT10:
           {
              Gan_YXPixel_ui10 yxPixel_ui10;

              yxPixel_ui10.X = 0;
              for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   yxPixel_ui10.Y = gan_pixel_rgbaui16_to_yui16(gan_image_get_pixptr_rgba_ui16(rgba, r0s+r, c0s+c)) >> 6;
                   gan_image_set_pix_gl_ui10 ( grey, r0d+r, c0d+c, &yxPixel_ui10 );
                }
           }
           break;

           case GAN_UINT12:
           {
              Gan_YXPixel_ui12 yxPixel_ui12;

              yxPixel_ui12.X = 0;
              for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   yxPixel_ui12.Y = gan_pixel_rgbaui16_to_yui16(gan_image_get_pixptr_rgba_ui16(rgba, r0s+r, c0s+c)) >> 4;
                   gan_image_set_pix_gl_ui12 ( grey, r0d+r, c0d+c, &yxPixel_ui12 );
                }
           }
           break;

           case GAN_UINT16:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_ui16 ( grey, r0d+r, c0d+c,
                       gan_pixel_rgbaui16_to_yui16 (
                           gan_image_get_pixptr_rgba_ui16(rgba, r0s+r, c0s+c)) );
             break;

           case GAN_UINT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_ui32 ( grey, r0d+r, c0d+c,
                       gan_pixel_ui16_to_ui32 (
                         gan_pixel_rgbaui16_to_yui16 (
                           gan_image_get_pixptr_rgba_ui16(rgba, r0s+r, c0s+c))));
             break;

           case GAN_BOOL:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_b ( grey, r0d+r, c0d+c,
                       gan_pixel_rgbaui16_to_yui16 (
                           gan_image_get_pixptr_rgba_ui16(rgba, r0s+r, c0s+c))
                           ? GAN_TRUE : GAN_FALSE );
             break;

           case GAN_FLOAT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_f32 ( grey, r0d+r, c0d+c,
                       gan_pixel_ui16_to_f32 (
                         gan_pixel_rgbaui16_to_yui16 (
                           gan_image_get_pixptr_rgba_ui16(rgba, r0s+r, c0s+c))));

             break;

           case GAN_FLOAT64:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_f64 ( grey, r0d+r, c0d+c,
                       gan_pixel_ui16_to_f64 (
                         gan_pixel_rgbaui16_to_yui16 (
                           gan_image_get_pixptr_rgba_ui16(rgba, r0s+r, c0s+c))));

             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_rgba_to_grey",
                                GAN_ERROR_NOT_IMPLEMENTED, "" );
             return NULL;
        }
        break;

      case GAN_UINT32:
        switch ( grey->type )
        {
           case GAN_UINT8:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_ui8 ( grey, r0d+r, c0d+c,
                       gan_pixel_f32_to_ui8 (
                         gan_pixel_rgbaui32_to_yf32 (
                           gan_image_get_pixptr_rgba_ui32(rgba, r0s+r, c0s+c))));
             break;

           case GAN_UINT16:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_ui16 ( grey, r0d+r, c0d+c,
                       gan_pixel_f32_to_ui16 (
                         gan_pixel_rgbaui32_to_yf32 (
                           gan_image_get_pixptr_rgba_ui32(rgba, r0s+r, c0s+c))));
             break;

           case GAN_UINT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_ui32 ( grey, r0d+r, c0d+c,
                       gan_pixel_f32_to_ui32 (
                         gan_pixel_rgbaui32_to_yf32 (
                           gan_image_get_pixptr_rgba_ui32(rgba, r0s+r, c0s+c))));
             break;

           case GAN_BOOL:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_b ( grey, r0d+r, c0d+c,
                       (gan_pixel_rgbaui32_to_yf32 (
                           gan_image_get_pixptr_rgba_ui32(rgba, r0s+r, c0s+c))
                        == 0.0F) ? GAN_FALSE : GAN_TRUE );
             break;

           case GAN_FLOAT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_f32 ( grey, r0d+r, c0d+c,
                         gan_pixel_rgbaui32_to_yf32 (
                            gan_image_get_pixptr_rgba_ui32(rgba, r0s+r, c0s+c)));

             break;

           case GAN_FLOAT64:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_f64 ( grey, r0d+r, c0d+c,
                        (gan_float64)gan_pixel_rgbaui32_to_yf32 (
                            gan_image_get_pixptr_rgba_ui32(rgba, r0s+r, c0s+c)));

             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_rgba_to_grey",
                                GAN_ERROR_NOT_IMPLEMENTED, "" );
             return NULL;
        }
        break;

      case GAN_FLOAT32:
        switch ( grey->type )
        {
           case GAN_UINT8:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_ui8 ( grey, r0d+r, c0d+c,
                       gan_pixel_f32_to_ui8 (
                          gan_pixel_rgbaf32_to_yf32 (
                            gan_image_get_pixptr_rgba_f32(rgba, r0s+r, c0s+c))));
             break;

           case GAN_UINT16:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_ui16 ( grey, r0d+r, c0d+c,
                       gan_pixel_f32_to_ui16 (
                          gan_pixel_rgbaf32_to_yf32 (
                            gan_image_get_pixptr_rgba_f32(rgba, r0s+r, c0s+c))));
             break;

           case GAN_UINT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_ui32 ( grey, r0d+r, c0d+c,
                       gan_pixel_f32_to_ui32 (
                          gan_pixel_rgbaf32_to_yf32 (
                            gan_image_get_pixptr_rgba_f32(rgba, r0s+r, c0s+c))));
             break;

           case GAN_BOOL:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_b ( grey, r0d+r, c0d+c,
                       (gan_pixel_rgbaf32_to_yf32 (
                           gan_image_get_pixptr_rgba_f32(rgba, r0s+r, c0s+c))
                        == 0.0F) ? GAN_FALSE : GAN_TRUE );
             break;

           case GAN_FLOAT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_f32 ( grey, r0d+r, c0d+c,
                         gan_pixel_rgbaf32_to_yf32 (
                             gan_image_get_pixptr_rgba_f32(rgba, r0s+r, c0s+c)));

             break;

           case GAN_FLOAT64:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_f64 ( grey, r0d+r, c0d+c,
                        (gan_float64)gan_pixel_rgbaf32_to_yf32 (
                             gan_image_get_pixptr_rgba_f32(rgba, r0s+r, c0s+c)));

             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_rgba_to_grey",
                                GAN_ERROR_NOT_IMPLEMENTED, "" );
             return NULL;
        }
        break;

      case GAN_FLOAT64:
        switch ( grey->type )
        {
           case GAN_UINT8:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_ui8 ( grey, r0d+r, c0d+c,
                       gan_pixel_f64_to_ui8 (
                          gan_pixel_rgbad_to_yd (
                            gan_image_get_pixptr_rgba_f64(rgba, r0s+r, c0s+c))));
             break;

           case GAN_UINT16:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_ui16 ( grey, r0d+r, c0d+c,
                       gan_pixel_f64_to_ui16 (
                          gan_pixel_rgbad_to_yd (
                            gan_image_get_pixptr_rgba_f64(rgba, r0s+r, c0s+c))));
             break;

           case GAN_UINT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_ui32 ( grey, r0d+r, c0d+c,
                       gan_pixel_f64_to_ui32 (
                          gan_pixel_rgbad_to_yd (
                            gan_image_get_pixptr_rgba_f64(rgba, r0s+r, c0s+c))));
             break;

           case GAN_BOOL:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_b ( grey, r0d+r, c0d+c,
                       (gan_pixel_rgbad_to_yd (
                           gan_image_get_pixptr_rgba_f64(rgba, r0s+r, c0s+c))
                        == 0.0F) ? GAN_FALSE : GAN_TRUE );
             break;

           case GAN_FLOAT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_f32 ( grey, r0d+r, c0d+c, (gan_float32)
                          gan_pixel_rgbad_to_yd (
                             gan_image_get_pixptr_rgba_f64(rgba, r0s+r, c0s+c)));

             break;

           case GAN_FLOAT64:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                   gan_image_set_pix_gl_f64 ( grey, r0d+r, c0d+c,
                        (gan_float64)gan_pixel_rgbad_to_yd (
                             gan_image_get_pixptr_rgba_f64(rgba, r0s+r, c0s+c)));

             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_rgba_to_grey",
                                GAN_ERROR_NOT_IMPLEMENTED, "" );
             return NULL;
        }
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "extract_rgba_to_grey", GAN_ERROR_NOT_IMPLEMENTED,
                           "" );
        return NULL;
   }

   return grey;
}

static Gan_Image *
 extract_rgb_to_gla ( const Gan_Image *rgb, unsigned r0s, unsigned c0s,
                      unsigned height, unsigned width,
                      Gan_Image *gla, unsigned r0d, unsigned c0d )
{
   unsigned int r, c;
   
   gan_err_test_ptr ( rgb->format == GAN_RGB_COLOUR_IMAGE &&
                      gla->format == GAN_GREY_LEVEL_ALPHA_IMAGE,
                      "extract_rgb_to_gla", GAN_ERROR_INCOMPATIBLE, "" );

   switch ( gla->type )
   {
      case GAN_UINT8:
        switch ( rgb->type )
        {
           Gan_GLAPixel_ui8 ui8pix;

           case GAN_UINT8:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   ui8pix.I = gan_pixel_rgbui8_to_yui8 (
                               gan_image_get_pixptr_rgb_ui8(rgb, r0s+r, c0s+c));
                   ui8pix.A = GAN_UINT8_MAX;
                   gan_image_set_pix_gla_ui8 ( gla, r0d+r, c0d+c, &ui8pix );
                }
             break;

           case GAN_UINT16:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   ui8pix.I = gan_pixel_rgbui16_to_yui8 (
                               gan_image_get_pixptr_rgb_ui16(rgb, r0s+r, c0s+c));
                   ui8pix.A = GAN_UINT8_MAX;
                   gan_image_set_pix_gla_ui8 ( gla, r0d+r, c0d+c, &ui8pix );
                }
             break;

           case GAN_UINT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   ui8pix.I = gan_pixel_f32_to_ui8 (
                              gan_pixel_rgbui32_to_yf32 (
                                gan_image_get_pixptr_rgb_ui32(rgb,r0s+r,c0s+c)));
                   ui8pix.A = GAN_UINT8_MAX;
                   gan_image_set_pix_gla_ui8 ( gla, r0d+r, c0d+c, &ui8pix );
                }
             break;

           case GAN_FLOAT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   ui8pix.I = gan_pixel_f64_to_ui8 (
                               gan_pixel_rgbf32_to_yf32 (
                                 gan_image_get_pixptr_rgb_f32(rgb,r0s+r,c0s+c)));
                   ui8pix.A = GAN_UINT8_MAX;
                   gan_image_set_pix_gla_ui8 ( gla, r0d+r, c0d+c, &ui8pix );
                }
             break;

           case GAN_FLOAT64:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   ui8pix.I = gan_pixel_f64_to_ui8 (
                               gan_pixel_rgbd_to_yd (
                                 gan_image_get_pixptr_rgb_f64(rgb,r0s+r,c0s+c)));
                   ui8pix.A = GAN_UINT8_MAX;
                   gan_image_set_pix_gla_ui8 ( gla, r0d+r, c0d+c, &ui8pix );
                }
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_rgb_to_gla",
                                GAN_ERROR_NOT_IMPLEMENTED, "" );
             return NULL;
        }
        break;

      case GAN_UINT16:
        switch ( rgb->type )
        {
           Gan_GLAPixel_ui16 ui16pix;

           case GAN_UINT8:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   ui16pix.I = gan_pixel_rgbui8_to_yui16 (
                               gan_image_get_pixptr_rgb_ui8(rgb, r0s+r, c0s+c));
                   ui16pix.A = GAN_UINT16_MAX;
                   gan_image_set_pix_gla_ui16 ( gla, r0d+r, c0d+c, &ui16pix );
                }
             break;

           case GAN_UINT16:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   ui16pix.I = gan_pixel_rgbui16_to_yui16 (
                               gan_image_get_pixptr_rgb_ui16(rgb, r0s+r, c0s+c));
                   ui16pix.A = GAN_UINT16_MAX;
                   gan_image_set_pix_gla_ui16 ( gla, r0d+r, c0d+c, &ui16pix );
                }
             break;

           case GAN_UINT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   ui16pix.I = gan_pixel_f32_to_ui16 (
                              gan_pixel_rgbui32_to_yf32 (
                                gan_image_get_pixptr_rgb_ui32(rgb,r0s+r,c0s+c)));
                   ui16pix.A = GAN_UINT16_MAX;
                   gan_image_set_pix_gla_ui16 ( gla, r0d+r, c0d+c, &ui16pix );
                }
             break;

           case GAN_FLOAT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   ui16pix.I = gan_pixel_f32_to_ui16 (
                               gan_pixel_rgbf32_to_yf32 (
                                 gan_image_get_pixptr_rgb_f32(rgb,r0s+r,c0s+c)));
                   ui16pix.A = GAN_UINT16_MAX;
                   gan_image_set_pix_gla_ui16 ( gla, r0d+r, c0d+c, &ui16pix );
                }
             break;

           case GAN_FLOAT64:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   ui16pix.I = gan_pixel_f64_to_ui16 (
                               gan_pixel_rgbd_to_yd (
                                 gan_image_get_pixptr_rgb_f64(rgb,r0s+r,c0s+c)));
                   ui16pix.A = GAN_UINT16_MAX;
                   gan_image_set_pix_gla_ui16 ( gla, r0d+r, c0d+c, &ui16pix );
                }
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_rgb_to_gla",
                                GAN_ERROR_NOT_IMPLEMENTED, "" );
             return NULL;
        }
        break;

      case GAN_UINT32:
        switch ( rgb->type )
        {
           Gan_GLAPixel_ui32 ui32pix;

           case GAN_UINT8:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   ui32pix.I = gan_pixel_rgbui8_to_yui32 (
                               gan_image_get_pixptr_rgb_ui8(rgb, r0s+r, c0s+c));
                   ui32pix.A = GAN_UINT32_MAX;
                   gan_image_set_pix_gla_ui32 ( gla, r0d+r, c0d+c, &ui32pix );
                }
             break;

           case GAN_UINT16:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   ui32pix.I = gan_pixel_ui16_to_ui32 (
                               gan_pixel_rgbui16_to_yui16 (
                                gan_image_get_pixptr_rgb_ui16(rgb,r0s+r,c0s+c)));
                   ui32pix.A = GAN_UINT32_MAX;
                   gan_image_set_pix_gla_ui32 ( gla, r0d+r, c0d+c, &ui32pix );
                }
             break;

           case GAN_UINT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   ui32pix.I = gan_pixel_f32_to_ui32 (
                               gan_pixel_rgbui32_to_yf32 (
                                gan_image_get_pixptr_rgb_ui32(rgb,r0s+r,c0s+c)));
                   ui32pix.A = GAN_UINT32_MAX;
                   gan_image_set_pix_gla_ui32 ( gla, r0d+r, c0d+c, &ui32pix );
                }
             break;

           case GAN_FLOAT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   ui32pix.I = gan_pixel_f32_to_ui32 (
                               gan_pixel_rgbf32_to_yf32 (
                                 gan_image_get_pixptr_rgb_f32(rgb,r0s+r,c0s+c)));
                   ui32pix.A = GAN_UINT32_MAX;
                   gan_image_set_pix_gla_ui32 ( gla, r0d+r, c0d+c, &ui32pix );
                }
             break;

           case GAN_FLOAT64:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   ui32pix.I = gan_pixel_f64_to_ui32 (
                               gan_pixel_rgbd_to_yd (
                                 gan_image_get_pixptr_rgb_f64(rgb,r0s+r,c0s+c)));
                   ui32pix.A = GAN_UINT32_MAX;
                   gan_image_set_pix_gla_ui32 ( gla, r0d+r, c0d+c, &ui32pix );
                }
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_rgb_to_gla",
                                GAN_ERROR_NOT_IMPLEMENTED, "" );
             return NULL;
        }
        break;

      case GAN_FLOAT32:
        switch ( rgb->type )
        {
           Gan_GLAPixel_f32 f32pix;

           case GAN_UINT8:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   f32pix.I = gan_pixel_ui16_to_f32 (
                              gan_pixel_rgbui8_to_yui16 (
                                gan_image_get_pixptr_rgb_ui8(rgb,r0s+r,c0s+c)));
                   f32pix.A = 1.0F;
                   gan_image_set_pix_gla_f32 ( gla, r0d+r, c0d+c, &f32pix );
                }
             break;

           case GAN_UINT16:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   f32pix.I = gan_pixel_ui16_to_f32 (
                              gan_pixel_rgbui16_to_yui16 (
                                gan_image_get_pixptr_rgb_ui16(rgb,r0s+r,c0s+c)));
                   f32pix.A = 1.0F;
                   gan_image_set_pix_gla_f32 ( gla, r0d+r, c0d+c, &f32pix );
                }
             break;

           case GAN_UINT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   f32pix.I = gan_pixel_rgbui32_to_yf32 (
                                gan_image_get_pixptr_rgb_ui32(rgb,r0s+r,c0s+c));
                   f32pix.A = 1.0F;
                   gan_image_set_pix_gla_f32 ( gla, r0d+r, c0d+c, &f32pix );
                }
             break;

           case GAN_FLOAT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   f32pix.I = gan_pixel_rgbf32_to_yf32 (
                                gan_image_get_pixptr_rgb_f32(rgb, r0s+r, c0s+c));
                   f32pix.A = 1.0F;
                   gan_image_set_pix_gla_f32 ( gla, r0d+r, c0d+c, &f32pix );
                }
             break;

           case GAN_FLOAT64:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   f32pix.I = (gan_float32)gan_pixel_rgbd_to_yd (
                                  gan_image_get_pixptr_rgb_f64(rgb,r0s+r,c0s+c));
                   f32pix.A = 1.0F;
                   gan_image_set_pix_gla_f32 ( gla, r0d+r, c0d+c, &f32pix );
                }
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_rgb_to_gla",
                                GAN_ERROR_NOT_IMPLEMENTED, "" );
             return NULL;
        }
        break;

      case GAN_FLOAT64:
        switch ( rgb->type )
        {
           Gan_GLAPixel_f64 f64pix;

           case GAN_UINT8:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   f64pix.I = gan_pixel_ui16_to_f64 (
                              gan_pixel_rgbui8_to_yui16 (
                                gan_image_get_pixptr_rgb_ui8(rgb,r0s+r,c0s+c)));
                   f64pix.A = 1.0;
                   gan_image_set_pix_gla_f64 ( gla, r0d+r, c0d+c, &f64pix );
                }
             break;

           case GAN_UINT16:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   f64pix.I = gan_pixel_ui16_to_f64 (
                              gan_pixel_rgbui16_to_yui16 (
                                gan_image_get_pixptr_rgb_ui16(rgb,r0s+r,c0s+c)));
                   f64pix.A = 1.0;
                   gan_image_set_pix_gla_f64 ( gla, r0d+r, c0d+c, &f64pix );
                }
             break;

           case GAN_UINT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   f64pix.I = (gan_float64)gan_pixel_rgbui32_to_yf32 (
                               gan_image_get_pixptr_rgb_ui32(rgb, r0s+r, c0s+c));
                   f64pix.A = 1.0;
                   gan_image_set_pix_gla_f64 ( gla, r0d+r, c0d+c, &f64pix );
                }
             break;

           case GAN_FLOAT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   f64pix.I = (gan_float64)gan_pixel_rgbf32_to_yf32 (
                               gan_image_get_pixptr_rgb_f32(rgb, r0s+r, c0s+c) );
                   f64pix.A = 1.0;
                   gan_image_set_pix_gla_f64 ( gla, r0d+r, c0d+c, &f64pix );
                }
             break;

           case GAN_FLOAT64:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   f64pix.I = gan_pixel_rgbd_to_yd (
                                gan_image_get_pixptr_rgb_f64(rgb, r0s+r, c0s+c));
                   f64pix.A = 1.0;
                   gan_image_set_pix_gla_f64 ( gla, r0d+r, c0d+c, &f64pix );
                }
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_rgb_to_gla",
                                GAN_ERROR_NOT_IMPLEMENTED, "" );
             return NULL;
        }
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "extract_rgb_to_gla", GAN_ERROR_NOT_IMPLEMENTED,
                           "" );
        return NULL;
   }

   return gla;
}

static Gan_Image *
 extract_rgba_to_gla ( const Gan_Image *rgba, unsigned r0s, unsigned c0s,
                       unsigned height, unsigned width,
                       Gan_Image *gla, unsigned r0d, unsigned c0d )
{
   unsigned int r, c;
   
   gan_err_test_ptr ( rgba->format == GAN_RGB_COLOUR_ALPHA_IMAGE &&
                      gla->format == GAN_GREY_LEVEL_ALPHA_IMAGE,
                      "extract_rgba_to_gla", GAN_ERROR_INCOMPATIBLE, "" );

   switch ( gla->type )
   {
      case GAN_UINT8:
        switch ( rgba->type )
        {
           Gan_GLAPixel_ui8 ui8pix;

           case GAN_UINT8:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   ui8pix.I = gan_pixel_rgbaui8_to_yui8 (gan_image_get_pixptr_rgba_ui8(rgba, r0s+r, c0s+c));
                   ui8pix.A = rgba->row_data.rgba.ui8[r0s+r][c0s+c].A;
                   gan_image_set_pix_gla_ui8 ( gla, r0d+r, c0d+c, &ui8pix );
                }
             break;

           case GAN_UINT16:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   ui8pix.I = gan_pixel_rgbaui16_to_yui8 (gan_image_get_pixptr_rgba_ui16(rgba, r0s+r, c0s+c));
                   ui8pix.A = gan_pixel_ui16_to_ui8 (rgba->row_data.rgba.ui16[r0s+r][c0s+c].A );
                   gan_image_set_pix_gla_ui8 ( gla, r0d+r, c0d+c, &ui8pix );
                }
             break;

           case GAN_UINT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   ui8pix.I = gan_pixel_f32_to_ui8 (gan_pixel_rgbaui32_to_yf32 (gan_image_get_pixptr_rgba_ui32(rgba,r0s+r,c0s+c)));
                   ui8pix.A = gan_pixel_ui32_to_ui8 (rgba->row_data.rgba.ui32[r0s+r][c0s+c].A );
                   gan_image_set_pix_gla_ui8 ( gla, r0d+r, c0d+c, &ui8pix );
                }
             break;

           case GAN_FLOAT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   ui8pix.I = gan_pixel_f32_to_ui8 (gan_pixel_rgbaf32_to_yf32 (gan_image_get_pixptr_rgba_f32(rgba,r0s+r,c0s+c)));
                   ui8pix.A = gan_pixel_f32_to_ui8 (rgba->row_data.rgba.f32[r0s+r][c0s+c].A );
                   gan_image_set_pix_gla_ui8 ( gla, r0d+r, c0d+c, &ui8pix );
                }
             break;

           case GAN_FLOAT64:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   ui8pix.I = gan_pixel_f64_to_ui8 (gan_pixel_rgbad_to_yd (gan_image_get_pixptr_rgba_f64(rgba,r0s+r,c0s+c)));
                   ui8pix.A = gan_pixel_f64_to_ui8 (rgba->row_data.rgba.f64[r0s+r][c0s+c].A );
                   gan_image_set_pix_gla_ui8 ( gla, r0d+r, c0d+c, &ui8pix );
                }
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_rgba_to_gla", GAN_ERROR_NOT_IMPLEMENTED, "" );
             return NULL;
        }
        break;

      case GAN_UINT16:
        switch ( rgba->type )
        {
           Gan_GLAPixel_ui16 ui16pix;

           case GAN_UINT8:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   ui16pix.I = gan_pixel_rgbaui8_to_yui16 (gan_image_get_pixptr_rgba_ui8(rgba, r0s+r, c0s+c));
                   ui16pix.A = gan_pixel_ui8_to_ui16 (rgba->row_data.rgba.ui8[r0s+r][c0s+c].A );
                   gan_image_set_pix_gla_ui16 ( gla, r0d+r, c0d+c, &ui16pix );
                }
             break;

           case GAN_UINT16:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   ui16pix.I = gan_pixel_rgbaui16_to_yui16 (gan_image_get_pixptr_rgba_ui16(rgba, r0s+r, c0s+c));
                   ui16pix.A = rgba->row_data.rgba.ui16[r0s+r][c0s+c].A;
                   gan_image_set_pix_gla_ui16 ( gla, r0d+r, c0d+c, &ui16pix );
                }
             break;

           case GAN_UINT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   ui16pix.I = gan_pixel_f32_to_ui16 (
                              gan_pixel_rgbaui32_to_yf32 (
                              gan_image_get_pixptr_rgba_ui32(rgba,r0s+r,c0s+c)));
                   ui16pix.A = gan_pixel_ui32_to_ui16 (
                                  rgba->row_data.rgba.ui32[r0s+r][c0s+c].A );
                   gan_image_set_pix_gla_ui16 ( gla, r0d+r, c0d+c, &ui16pix );
                }
             break;

           case GAN_FLOAT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   ui16pix.I = gan_pixel_f32_to_ui16 (
                              gan_pixel_rgbaf32_to_yf32 (
                               gan_image_get_pixptr_rgba_f32(rgba,r0s+r,c0s+c)));
                   ui16pix.A = gan_pixel_f32_to_ui16 (
                                  rgba->row_data.rgba.f32[r0s+r][c0s+c].A );
                   gan_image_set_pix_gla_ui16 ( gla, r0d+r, c0d+c, &ui16pix );
                }
             break;

           case GAN_FLOAT64:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   ui16pix.I = gan_pixel_f64_to_ui16 (
                              gan_pixel_rgbad_to_yd (
                               gan_image_get_pixptr_rgba_f64(rgba,r0s+r,c0s+c)));
                   ui16pix.A = gan_pixel_f64_to_ui16 (
                                  rgba->row_data.rgba.f64[r0s+r][c0s+c].A );
                   gan_image_set_pix_gla_ui16 ( gla, r0d+r, c0d+c, &ui16pix );
                }
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_rgba_to_gla",
                                GAN_ERROR_NOT_IMPLEMENTED, "" );
             return NULL;
        }
        break;

      case GAN_UINT32:
        switch ( rgba->type )
        {
           Gan_GLAPixel_ui32 ui32pix;

           case GAN_UINT8:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   ui32pix.I = gan_pixel_rgbaui8_to_yui32 (
                               gan_image_get_pixptr_rgba_ui8(rgba,r0s+r,c0s+c));
                   ui32pix.A = gan_pixel_ui8_to_ui32 (
                                  rgba->row_data.rgba.ui8[r0s+r][c0s+c].A );
                   gan_image_set_pix_gla_ui32 ( gla, r0d+r, c0d+c, &ui32pix );
                }
             break;

           case GAN_UINT16:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   ui32pix.I = gan_pixel_ui16_to_ui32 (
                              gan_pixel_rgbaui16_to_yui16 (
                              gan_image_get_pixptr_rgba_ui16(rgba,r0s+r,c0s+c)));
                   ui32pix.A = gan_pixel_ui16_to_ui32 (
                                  rgba->row_data.rgba.ui16[r0s+r][c0s+c].A );
                   gan_image_set_pix_gla_ui32 ( gla, r0d+r, c0d+c, &ui32pix );
                }
             break;

           case GAN_UINT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   ui32pix.I = gan_pixel_f32_to_ui32 (
                              gan_pixel_rgbaui32_to_yf32 (
                              gan_image_get_pixptr_rgba_ui32(rgba,r0s+r,c0s+c)));
                   ui32pix.A = rgba->row_data.rgba.ui32[r0s+r][c0s+c].A;
                   gan_image_set_pix_gla_ui32 ( gla, r0d+r, c0d+c, &ui32pix );
                }
             break;

           case GAN_FLOAT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   ui32pix.I = gan_pixel_f32_to_ui32 (
                              gan_pixel_rgbaf32_to_yf32 (
                               gan_image_get_pixptr_rgba_f32(rgba,r0s+r,c0s+c)));
                   ui32pix.A = gan_pixel_f32_to_ui32 (
                                  rgba->row_data.rgba.f32[r0s+r][c0s+c].A );
                   gan_image_set_pix_gla_ui32 ( gla, r0d+r, c0d+c, &ui32pix );
                }
             break;

           case GAN_FLOAT64:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   ui32pix.I = gan_pixel_f64_to_ui32 (
                              gan_pixel_rgbad_to_yd (
                               gan_image_get_pixptr_rgba_f64(rgba,r0s+r,c0s+c)));
                   ui32pix.A = gan_pixel_f64_to_ui32 (
                                  rgba->row_data.rgba.f64[r0s+r][c0s+c].A );
                   gan_image_set_pix_gla_ui32 ( gla, r0d+r, c0d+c, &ui32pix );
                }
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_rgba_to_gla",
                                GAN_ERROR_NOT_IMPLEMENTED, "" );
             return NULL;
        }
        break;

      case GAN_FLOAT32:
        switch ( rgba->type )
        {
           Gan_GLAPixel_f32 f32pix;

           case GAN_UINT8:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   f32pix.I = gan_pixel_ui16_to_f32 (
                             gan_pixel_rgbaui8_to_yui16 (
                              gan_image_get_pixptr_rgba_ui8(rgba,r0s+r,c0s+c)));
                   f32pix.A = gan_pixel_ui8_to_f32 (
                                  rgba->row_data.rgba.ui8[r0s+r][c0s+c].A );
                   gan_image_set_pix_gla_f32 ( gla, r0d+r, c0d+c, &f32pix );
                }
             break;

           case GAN_UINT16:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   f32pix.I = gan_pixel_ui16_to_f32 (
                             gan_pixel_rgbaui16_to_yui16 (
                              gan_image_get_pixptr_rgba_ui16(rgba,r0s+r,c0s+c)));
                   f32pix.A = gan_pixel_ui16_to_f32 (
                                  rgba->row_data.rgba.ui16[r0s+r][c0s+c].A );
                   gan_image_set_pix_gla_f32 ( gla, r0d+r, c0d+c, &f32pix );
                }
             break;

           case GAN_UINT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   f32pix.I = gan_pixel_rgbaui32_to_yf32 (
                             gan_image_get_pixptr_rgba_ui32(rgba, r0s+r, c0s+c));
                   f32pix.A = gan_pixel_ui32_to_f32 (
                                  rgba->row_data.rgba.ui32[r0s+r][c0s+c].A );
                   gan_image_set_pix_gla_f32 ( gla, r0d+r, c0d+c, &f32pix );
                }
             break;

           case GAN_FLOAT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   f32pix.I = gan_pixel_rgbaf32_to_yf32 (
                              gan_image_get_pixptr_rgba_f32(rgba, r0s+r, c0s+c));
                   f32pix.A = rgba->row_data.rgba.f32[r0s+r][c0s+c].A;
                   gan_image_set_pix_gla_f32 ( gla, r0d+r, c0d+c, &f32pix );
                }
             break;

           case GAN_FLOAT64:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   f32pix.I = (gan_float32)gan_pixel_rgbad_to_yd (
                                gan_image_get_pixptr_rgba_f64(rgba,r0s+r,c0s+c));
                   f32pix.A = (gan_float32)rgba->row_data.rgba.f64[r0s+r][c0s+c].A;
                   gan_image_set_pix_gla_f32 ( gla, r0d+r, c0d+c, &f32pix );
                }
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_rgba_to_gla",
                                GAN_ERROR_NOT_IMPLEMENTED, "" );
             return NULL;
        }
        break;

      case GAN_FLOAT64:
        switch ( rgba->type )
        {
           Gan_GLAPixel_f64 f64pix;

           case GAN_UINT8:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   f64pix.I = gan_pixel_ui16_to_f64 ( gan_pixel_rgbaui8_to_yui16 ( gan_image_get_pixptr_rgba_ui8(rgba,r0s+r,c0s+c)));
                   f64pix.A = gan_pixel_ui8_to_f64 ( rgba->row_data.rgba.ui8[r0s+r][c0s+c].A );
                   gan_image_set_pix_gla_f64 ( gla, r0d+r, c0d+c, &f64pix );
                }
             break;

           case GAN_UINT16:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   f64pix.I = gan_pixel_ui16_to_f64 ( gan_pixel_rgbaui16_to_yui16 (gan_image_get_pixptr_rgba_ui16(rgba,r0s+r,c0s+c)));
                   f64pix.A = gan_pixel_ui16_to_f64 ( rgba->row_data.rgba.ui16[r0s+r][c0s+c].A );
                   gan_image_set_pix_gla_f64 ( gla, r0d+r, c0d+c, &f64pix );
                }
             break;

           case GAN_UINT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   f64pix.I = (gan_float64)gan_pixel_rgbaui32_to_yf32 (
                             gan_image_get_pixptr_rgba_ui32(rgba, r0s+r, c0s+c));
                   f64pix.A = gan_pixel_ui32_to_f64 (
                                  rgba->row_data.rgba.ui32[r0s+r][c0s+c].A );
                   gan_image_set_pix_gla_f64 ( gla, r0d+r, c0d+c, &f64pix );
                }
             break;

           case GAN_FLOAT32:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   f64pix.I = (gan_float64)gan_pixel_rgbaf32_to_yf32 (
                              gan_image_get_pixptr_rgba_f32(rgba, r0s+r, c0s+c));
                   f64pix.A = (gan_float64)rgba->row_data.rgba.f32[r0s+r][c0s+c].A;
                   gan_image_set_pix_gla_f64 ( gla, r0d+r, c0d+c, &f64pix );
                }
             break;

           case GAN_FLOAT64:
             for ( r=0; r < height; r++ )
                for ( c=0; c < width; c++ )
                {
                   f64pix.I = gan_pixel_rgbad_to_yd (
                              gan_image_get_pixptr_rgba_f64(rgba, r0s+r, c0s+c));
                   f64pix.A = rgba->row_data.rgba.f64[r0s+r][c0s+c].A;
                   gan_image_set_pix_gla_f64 ( gla, r0d+r, c0d+c, &f64pix );
                }
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_rgba_to_gla",
                                GAN_ERROR_NOT_IMPLEMENTED, "" );
             return NULL;
        }
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "extract_rgba_to_gla", GAN_ERROR_NOT_IMPLEMENTED,
                           "" );
        return NULL;
   }

   return gla;
}

/* Converts an RGB image to a different type
 *
 * The converted RGB image is returned.
 */
static Gan_Image *
 extract_rgb ( const Gan_Image *source, unsigned r0s, unsigned c0s,
               unsigned height, unsigned width, Gan_Bool copy_pixels,
               Gan_Image *dest, unsigned r0d, unsigned c0d )
{
   unsigned int r, c;
   
   gan_err_test_ptr ( source->format == GAN_RGB_COLOUR_IMAGE &&
                      dest->format == GAN_RGB_COLOUR_IMAGE,
                      "extract_rgb", GAN_ERROR_INCOMPATIBLE, "" );

   switch ( dest->type )
   {
      case GAN_UINT8:
        switch ( source->type )
        {
           Gan_RGBPixel_ui8 ui8pix;

           case GAN_UINT8:
             if ( copy_pixels )
                for ( r=0; r < height; r++ )
                   gan_copy_array_ui8 ( (gan_uint8 *)
                                       &source->row_data.rgb.ui8[r0s+r][c0s],
                                       1, 3*width, (gan_uint8 *)
                                       &dest->row_data.rgb.ui8[r0d+r][c0d],1);
             else
             {
                /* make destination image point into source image */
                gan_err_test_ptr ( r0d == 0 && c0d == 0, "extract_rgb",
                                   GAN_ERROR_INCOMPATIBLE, "" );
                for ( r=0; r < height; r++ )
                   dest->row_data.rgb.ui8[r] =
                                          &source->row_data.rgb.ui8[r0s+r][c0s];

                dest->pix_data_ptr = (unsigned char *)
                                     gan_image_get_pixptr_rgb_ui8(source, r0s, c0s);
             }
             break;
         
           case GAN_UINT16:
           {
              Gan_RGBPixel_ui16 ui16pix;

              gan_err_test_ptr ( copy_pixels, "extract_rgb",
                                 GAN_ERROR_ILLEGAL_ARGUMENT, "" );
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    /* clip RGB pixel values to gan_uint8 limits */
                    ui16pix = gan_image_get_pix_rgb_ui16 ( source, r0s+r, c0s+c );
                    ui8pix.R = gan_pixel_ui16_to_ui8(ui16pix.R);
                    ui8pix.G = gan_pixel_ui16_to_ui8(ui16pix.G);
                    ui8pix.B = gan_pixel_ui16_to_ui8(ui16pix.B);
                    gan_image_set_pix_rgb_ui8 ( dest, r0d+r, c0d+c, &ui8pix );
                 }
           }
           break;
         
           case GAN_UINT32:
           {
              Gan_RGBPixel_ui32 ui32pix;
            
              gan_err_test_ptr ( copy_pixels, "extract_rgb",
                                 GAN_ERROR_ILLEGAL_ARGUMENT, "" );
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    /* clip RGB pixel values to gan_uint8 limits */
                    ui32pix = gan_image_get_pix_rgb_ui32 ( source, r0s+r, c0s+c );
                    ui8pix.R = gan_pixel_ui32_to_ui8(ui32pix.R);
                    ui8pix.G = gan_pixel_ui32_to_ui8(ui32pix.G);
                    ui8pix.B = gan_pixel_ui32_to_ui8(ui32pix.B);
                    gan_image_set_pix_rgb_ui8 ( dest, r0d+r, c0d+c, &ui8pix );
                 }
           }
           break;
         
           case GAN_FLOAT32:
           {
              Gan_RGBPixel_f32 f32pix;
            
              gan_err_test_ptr ( copy_pixels, "extract_rgb",
                                 GAN_ERROR_ILLEGAL_ARGUMENT, "" );
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    /* clip RGB pixel values to gan_uint8 limits */
                    f32pix = gan_image_get_pix_rgb_f32 ( source, r0s+r, c0s+c );
                    ui8pix.R = gan_pixel_f32_to_ui8(f32pix.R);
                    ui8pix.G = gan_pixel_f32_to_ui8(f32pix.G);
                    ui8pix.B = gan_pixel_f32_to_ui8(f32pix.B);
                    gan_image_set_pix_rgb_ui8 ( dest, r0d+r, c0d+c, &ui8pix );
                 }
           }
           break;
         
           case GAN_FLOAT64:
           {
              Gan_RGBPixel_f64 f64pix;
            
              gan_err_test_ptr ( copy_pixels, "extract_rgb",
                                 GAN_ERROR_ILLEGAL_ARGUMENT, "" );
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    /* clip RGB pixel values to gan_uint8 limits */
                    f64pix = gan_image_get_pix_rgb_f64 ( source, r0s+r, c0s+c );
                    ui8pix.R = gan_pixel_f64_to_ui8(f64pix.R);
                    ui8pix.G = gan_pixel_f64_to_ui8(f64pix.G);
                    ui8pix.B = gan_pixel_f64_to_ui8(f64pix.B);
                    gan_image_set_pix_rgb_ui8 ( dest, r0d+r, c0d+c, &ui8pix );
                 }
           }
           break;
         
           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_rgb", GAN_ERROR_NOT_IMPLEMENTED, "" );
             return NULL;
        }
        break;
      
      case GAN_UINT16:
        switch ( source->type )
        {
           Gan_RGBPixel_ui16 ui16pix;

           case GAN_UINT8:
           {
              Gan_RGBPixel_ui8 ui8pix;
            
              gan_err_test_ptr ( copy_pixels, "extract_rgb",
                                 GAN_ERROR_ILLEGAL_ARGUMENT, "" );
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    /* clip RGB pixel values to gan_uint8 limits */
                    ui8pix = gan_image_get_pix_rgb_ui8 ( source, r0s+r, c0s+c );
                    ui16pix.R = gan_pixel_ui8_to_ui16(ui8pix.R);
                    ui16pix.G = gan_pixel_ui8_to_ui16(ui8pix.G);
                    ui16pix.B = gan_pixel_ui8_to_ui16(ui8pix.B);
                    gan_image_set_pix_rgb_ui16 ( dest, r0d+r, c0d+c, &ui16pix );
                 }
           }
           break;
         
           case GAN_UINT16:
             if ( copy_pixels )
                for ( r=0; r < height; r++ )
                   gan_copy_array_ui16 ( (gan_uint16 *)
                                       &source->row_data.rgb.ui16[r0s+r][c0s],
                                       1, 3*width, (gan_uint16 *)
                                       &dest->row_data.rgb.ui16[r0d+r][c0d],1);
             else
             {
                /* make destination image point into source image */
                gan_err_test_ptr ( r0d == 0 && c0d == 0, "extract_rgb",
                                   GAN_ERROR_INCOMPATIBLE, "" );
                for ( r=0; r < height; r++ )
                   dest->row_data.rgb.ui16[r] =
                                          &source->row_data.rgb.ui16[r0s+r][c0s];

                dest->pix_data_ptr = (unsigned char *)
                                     gan_image_get_pixptr_rgb_ui16(source, r0s, c0s);
             }
             break;
         
           case GAN_UINT32:
           {
              Gan_RGBPixel_ui32 ui32pix;
            
              gan_err_test_ptr ( copy_pixels, "extract_rgb",
                                 GAN_ERROR_ILLEGAL_ARGUMENT, "" );
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    /* clip RGB pixel values to gan_uint16 limits */
                    ui32pix = gan_image_get_pix_rgb_ui32 ( source, r0s+r, c0s+c );
                    ui16pix.R = gan_pixel_ui32_to_ui16(ui32pix.R);
                    ui16pix.G = gan_pixel_ui32_to_ui16(ui32pix.G);
                    ui16pix.B = gan_pixel_ui32_to_ui16(ui32pix.B);
                    gan_image_set_pix_rgb_ui16 ( dest, r0d+r, c0d+c, &ui16pix );
                 }
           }
           break;
         
           case GAN_FLOAT32:
           {
              Gan_RGBPixel_f32 f32pix;
            
              gan_err_test_ptr ( copy_pixels, "extract_rgb",
                                 GAN_ERROR_ILLEGAL_ARGUMENT, "" );
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    /* clip RGB pixel values to gan_uint8 limits */
                    f32pix = gan_image_get_pix_rgb_f32 ( source, r0s+r, c0s+c );
                    ui16pix.R = gan_pixel_f32_to_ui16(f32pix.R);
                    ui16pix.G = gan_pixel_f32_to_ui16(f32pix.G);
                    ui16pix.B = gan_pixel_f32_to_ui16(f32pix.B);
                    gan_image_set_pix_rgb_ui16 ( dest, r0d+r, c0d+c, &ui16pix );
                 }
           }
           break;
         
           case GAN_FLOAT64:
           {
              Gan_RGBPixel_f64 f64pix;
            
              gan_err_test_ptr ( copy_pixels, "extract_rgb",
                                 GAN_ERROR_ILLEGAL_ARGUMENT, "" );
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    /* clip RGB pixel values to gan_uint8 limits */
                    f64pix = gan_image_get_pix_rgb_f64 ( source, r0s+r, c0s+c );
                    ui16pix.R = gan_pixel_f64_to_ui16(f64pix.R);
                    ui16pix.G = gan_pixel_f64_to_ui16(f64pix.G);
                    ui16pix.B = gan_pixel_f64_to_ui16(f64pix.B);
                    gan_image_set_pix_rgb_ui16 ( dest, r0d+r, c0d+c, &ui16pix );
                 }
           }
           break;
         
           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_rgb", GAN_ERROR_NOT_IMPLEMENTED, "" );
             return NULL;
        }
        break;
      
      case GAN_INT32:
        switch ( source->type )
        {
           Gan_RGBPixel_i32 i32pix;

           case GAN_INT32:
             if ( copy_pixels )
                for ( r=0; r < height; r++ )
                   gan_copy_array_i32 ( (gan_int32 *)&source->row_data.rgb.i32[r0s+r][c0s], 1, 3*width, (gan_int32 *)
                                        &dest->row_data.rgb.i32[r0d+r][c0d], 1 );
             else
             {
                /* make destination image point into source image */
                gan_err_test_ptr ( r0d == 0 && c0d == 0, "extract_rgb",
                                   GAN_ERROR_INCOMPATIBLE, "" );
                for ( r=0; r < height; r++ )
                   dest->row_data.rgb.i32[r]=&source->row_data.rgb.i32[r0s+r][c0s];
             }
             break;
         
           case GAN_FLOAT32:
           {
              Gan_RGBPixel_f32 f32pix;
            
              gan_err_test_ptr ( copy_pixels, "extract_rgb",
                                 GAN_ERROR_ILLEGAL_ARGUMENT, "" );
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    /* clip RGB pixel values to gan_uint8 limits */
                    f32pix = gan_image_get_pix_rgb_f32 ( source, r0s+r, c0s+c );
                    i32pix.R = gan_pixel_f32_to_i32(f32pix.R);
                    i32pix.G = gan_pixel_f32_to_i32(f32pix.G);
                    i32pix.B = gan_pixel_f32_to_i32(f32pix.B);
                    gan_image_set_pix_rgb_i32 ( dest, r0d+r, c0d+c, &i32pix );
                 }
           }
           break;
         
           case GAN_FLOAT64:
           {
              Gan_RGBPixel_f64 f64pix;
            
              gan_err_test_ptr ( copy_pixels, "extract_rgb",
                                 GAN_ERROR_ILLEGAL_ARGUMENT, "" );
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    /* clip RGB pixel values to gan_uint8 limits */
                    f64pix = gan_image_get_pix_rgb_f64 ( source, r0s+r, c0s+c );
                    i32pix.R = gan_pixel_f64_to_i32(f64pix.R);
                    i32pix.G = gan_pixel_f64_to_i32(f64pix.G);
                    i32pix.B = gan_pixel_f64_to_i32(f64pix.B);
                    gan_image_set_pix_rgb_i32 ( dest, r0d+r, c0d+c, &i32pix );
                 }
           }
           break;
         
           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_rgb", GAN_ERROR_NOT_IMPLEMENTED, "" );
             return NULL;
        }
        break;
      
      case GAN_UINT32:
        switch ( source->type )
        {
           Gan_RGBPixel_ui32 ui32pix;

           case GAN_UINT8:
           {
              Gan_RGBPixel_ui8 ui8pix;
            
              gan_err_test_ptr ( copy_pixels, "extract_rgb",
                                 GAN_ERROR_ILLEGAL_ARGUMENT, "" );
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    /* clip RGB pixel values to gan_uint8 limits */
                    ui8pix = gan_image_get_pix_rgb_ui8 ( source, r0s+r, c0s+c );
                    ui32pix.R = gan_pixel_ui8_to_ui32(ui8pix.R);
                    ui32pix.G = gan_pixel_ui8_to_ui32(ui8pix.G);
                    ui32pix.B = gan_pixel_ui8_to_ui32(ui8pix.B);
                    gan_image_set_pix_rgb_ui32 ( dest, r0d+r, c0d+c, &ui32pix );
                 }
           }
           break;
         
           case GAN_UINT16:
           {
              Gan_RGBPixel_ui16 ui16pix;
            
              gan_err_test_ptr ( copy_pixels, "extract_rgb",
                                 GAN_ERROR_ILLEGAL_ARGUMENT, "" );
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    /* clip RGB pixel values to gan_uint8 limits */
                    ui16pix = gan_image_get_pix_rgb_ui16 ( source, r0s+r, c0s+c );
                    ui32pix.R = gan_pixel_ui16_to_ui32(ui16pix.R);
                    ui32pix.G = gan_pixel_ui16_to_ui32(ui16pix.G);
                    ui32pix.B = gan_pixel_ui16_to_ui32(ui16pix.B);
                    gan_image_set_pix_rgb_ui32 ( dest, r0d+r, c0d+c, &ui32pix );
                 }
           }
           break;
         
           case GAN_UINT32:
             if ( copy_pixels )
                for ( r=0; r < height; r++ )
                   gan_copy_array_ui32 ( (gan_uint32 *)
                                       &source->row_data.rgb.ui32[r0s+r][c0s],
                                       1, 3*width, (gan_uint32 *)
                                       &dest->row_data.rgb.ui32[r0d+r][c0d],1);
             else
             {
                /* make destination image point into source image */
                gan_err_test_ptr ( r0d == 0 && c0d == 0, "extract_rgb",
                                   GAN_ERROR_INCOMPATIBLE, "" );
                for ( r=0; r < height; r++ )
                   dest->row_data.rgb.ui32[r] =
                                          &source->row_data.rgb.ui32[r0s+r][c0s];

                dest->pix_data_ptr = (unsigned char *)
                                     gan_image_get_pixptr_rgb_ui32(source, r0s, c0s);
             }
             break;

           case GAN_FLOAT32:
           {
              Gan_RGBPixel_f32 f32pix;
            
              gan_err_test_ptr ( copy_pixels, "extract_rgb",
                                 GAN_ERROR_ILLEGAL_ARGUMENT, "" );
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    /* clip RGB pixel values to gan_uint8 limits */
                    f32pix = gan_image_get_pix_rgb_f32 ( source, r0s+r, c0s+c );
                    ui32pix.R = gan_pixel_f32_to_ui32(f32pix.R);
                    ui32pix.G = gan_pixel_f32_to_ui32(f32pix.G);
                    ui32pix.B = gan_pixel_f32_to_ui32(f32pix.B);
                    gan_image_set_pix_rgb_ui32 ( dest, r0d+r, c0d+c, &ui32pix );
                 }
           }
           break;
         
           case GAN_FLOAT64:
           {
              Gan_RGBPixel_f64 f64pix;
            
              gan_err_test_ptr ( copy_pixels, "extract_rgb",
                                 GAN_ERROR_ILLEGAL_ARGUMENT, "" );
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    /* clip RGB pixel values to gan_uint8 limits */
                    f64pix = gan_image_get_pix_rgb_f64 ( source, r0s+r, c0s+c );
                    ui32pix.R = gan_pixel_f64_to_ui32(f64pix.R);
                    ui32pix.G = gan_pixel_f64_to_ui32(f64pix.G);
                    ui32pix.B = gan_pixel_f64_to_ui32(f64pix.B);
                    gan_image_set_pix_rgb_ui32 ( dest, r0d+r, c0d+c, &ui32pix );
                 }
           }
           break;
         
           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_rgb", GAN_ERROR_NOT_IMPLEMENTED, "" );
             return NULL;
        }
        break;
      
      case GAN_FLOAT32:
        switch ( source->type )
        {
           Gan_RGBPixel_f32 f32pix;

           case GAN_UINT8:
           {
              Gan_RGBPixel_ui8 ui8pix;
               
              gan_err_test_ptr ( copy_pixels, "extract_rgb",
                                 GAN_ERROR_ILLEGAL_ARGUMENT, "" );
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    ui8pix = gan_image_get_pix_rgb_ui8 ( source, r0s+r, c0s+c );
                    f32pix.R = gan_pixel_ui8_to_f32(ui8pix.R);
                    f32pix.G = gan_pixel_ui8_to_f32(ui8pix.G);
                    f32pix.B = gan_pixel_ui8_to_f32(ui8pix.B);
                    gan_image_set_pix_rgb_f32 ( dest, r0d+r, c0d+c, &f32pix );
                 }
           }
           break;
            
           case GAN_UINT16:
           {
              Gan_RGBPixel_ui16 ui16pix;
               
              gan_err_test_ptr ( copy_pixels, "extract_rgb",
                                 GAN_ERROR_ILLEGAL_ARGUMENT, "" );
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    ui16pix = gan_image_get_pix_rgb_ui16 ( source, r0s+r, c0s+c );
                    f32pix.R = gan_pixel_ui16_to_f32(ui16pix.R);
                    f32pix.G = gan_pixel_ui16_to_f32(ui16pix.G);
                    f32pix.B = gan_pixel_ui16_to_f32(ui16pix.B);
                    gan_image_set_pix_rgb_f32 ( dest, r0d+r, c0d+c, &f32pix );
                 }
           }
           break;
            
           case GAN_UINT32:
           {
              Gan_RGBPixel_ui32 ui32pix;
               
              gan_err_test_ptr ( copy_pixels, "extract_rgb",
                                 GAN_ERROR_ILLEGAL_ARGUMENT, "" );
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    ui32pix = gan_image_get_pix_rgb_ui32 ( source, r0s+r, c0s+c );
                    f32pix.R = gan_pixel_ui32_to_f32(ui32pix.R);
                    f32pix.G = gan_pixel_ui32_to_f32(ui32pix.G);
                    f32pix.B = gan_pixel_ui32_to_f32(ui32pix.B);
                    gan_image_set_pix_rgb_f32 ( dest, r0d+r, c0d+c, &f32pix );
                 }
           }
           break;
            
           case GAN_FLOAT32:
             if ( copy_pixels )
                for ( r=0; r < height; r++ )
                   gan_copy_array_f32 ( (gan_float32 *)
                                      &source->row_data.rgb.f32[r0s+r][c0s],
                                      1, 3*width, (gan_float32 *)
                                      &dest->row_data.rgb.f32[r0d+r][c0d], 1 );
             else
             {
                /* make destination image point into source image */
                gan_err_test_ptr ( r0d == 0 && c0d == 0, "extract_rgb",
                                   GAN_ERROR_INCOMPATIBLE, "" );
                for ( r=0; r < height; r++ )
                   dest->row_data.rgb.f32[r] =
                                          &source->row_data.rgb.f32[r0s+r][c0s];

                dest->pix_data_ptr = (unsigned char *)
                                     gan_image_get_pixptr_rgb_f32(source, r0s, c0s);
             }
             break;
            
           case GAN_FLOAT64:
           {
              Gan_RGBPixel_f64 f64pix;
               
              gan_err_test_ptr ( copy_pixels, "extract_rgb",
                                 GAN_ERROR_ILLEGAL_ARGUMENT, "" );
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    f64pix = gan_image_get_pix_rgb_f64 ( source, r0s+r, c0s+c );
                    f32pix.R = (gan_float32)f64pix.R;
                    f32pix.G = (gan_float32)f64pix.G;
                    f32pix.B = (gan_float32)f64pix.B;
                    gan_image_set_pix_rgb_f32 ( dest, r0d+r, c0d+c, &f32pix );
                 }
           }
           break;
            
           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_rgb", GAN_ERROR_NOT_IMPLEMENTED, "" );
             return NULL;
        }
        break;
         
      case GAN_FLOAT64:
        switch ( source->type )
        {
           Gan_RGBPixel_f64 f64pix;

           case GAN_UINT8:
           {
              Gan_RGBPixel_ui8 ui8pix;
               
              gan_err_test_ptr ( copy_pixels, "extract_rgb",
                                 GAN_ERROR_ILLEGAL_ARGUMENT, "" );
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    ui8pix = gan_image_get_pix_rgb_ui8 ( source, r0s+r, c0s+c );
                    f64pix.R = gan_pixel_ui8_to_f64(ui8pix.R);
                    f64pix.G = gan_pixel_ui8_to_f64(ui8pix.G);
                    f64pix.B = gan_pixel_ui8_to_f64(ui8pix.B);
                    gan_image_set_pix_rgb_f64 ( dest, r0d+r, c0d+c, &f64pix );
                 }
           }
           break;
            
           case GAN_UINT16:
           {
              Gan_RGBPixel_ui16 ui16pix;
               
              gan_err_test_ptr ( copy_pixels, "extract_rgb",
                                 GAN_ERROR_ILLEGAL_ARGUMENT, "" );
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    ui16pix = gan_image_get_pix_rgb_ui16 ( source, r0s+r, c0s+c );
                    f64pix.R = gan_pixel_ui16_to_f64(ui16pix.R);
                    f64pix.G = gan_pixel_ui16_to_f64(ui16pix.G);
                    f64pix.B = gan_pixel_ui16_to_f64(ui16pix.B);
                    gan_image_set_pix_rgb_f64 ( dest, r0d+r, c0d+c, &f64pix );
                 }
           }
           break;
            
           case GAN_UINT32:
           {
              Gan_RGBPixel_ui32 ui32pix;
               
              gan_err_test_ptr ( copy_pixels, "extract_rgb",
                                 GAN_ERROR_ILLEGAL_ARGUMENT, "" );
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    ui32pix = gan_image_get_pix_rgb_ui32 ( source, r0s+r, c0s+c );
                    f64pix.R = gan_pixel_ui32_to_f64(ui32pix.R);
                    f64pix.G = gan_pixel_ui32_to_f64(ui32pix.G);
                    f64pix.B = gan_pixel_ui32_to_f64(ui32pix.B);
                    gan_image_set_pix_rgb_f64 ( dest, r0d+r, c0d+c, &f64pix );
                 }
           }
           break;
            
           case GAN_FLOAT32:
           {
              Gan_RGBPixel_f32 f32pix;
               
              gan_err_test_ptr ( copy_pixels, "extract_rgb",
                                 GAN_ERROR_ILLEGAL_ARGUMENT, "" );
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    f32pix = gan_image_get_pix_rgb_f32 ( source, r0s+r, c0s+c );
                    f64pix.R = (gan_float64)f32pix.R;
                    f64pix.G = (gan_float64)f32pix.G;
                    f64pix.B = (gan_float64)f32pix.B;
                    gan_image_set_pix_rgb_f64 ( dest, r0d+r, c0d+c, &f64pix );
                 }
           }
           break;
            
           case GAN_FLOAT64:
             if ( copy_pixels )
                for ( r=0; r < height; r++ )
                   gan_copy_array_f64 ( (gan_float64 *)
                                      &source->row_data.rgb.f64[r0s+r][c0s],
                                      1, 3*width, (gan_float64 *)
                                      &dest->row_data.rgb.f64[r0d+r][c0d], 1 );
             else
             {
                /* make destination image point into source image */
                gan_err_test_ptr ( r0d == 0 && c0d == 0, "extract_rgb",
                                   GAN_ERROR_INCOMPATIBLE, "" );
                for ( r=0; r < height; r++ )
                   dest->row_data.rgb.f64[r] =
                                          &source->row_data.rgb.f64[r0s+r][c0s];

                dest->pix_data_ptr = (unsigned char *)
                                     gan_image_get_pixptr_rgb_f64(source, r0s, c0s);
             }
             break;
            
           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_rgb", GAN_ERROR_NOT_IMPLEMENTED, "" );
             return NULL;
        }
        break;
         
      default:
        gan_err_flush_trace();
        gan_err_register ( "extract_rgb", GAN_ERROR_NOT_IMPLEMENTED, "" );
        return NULL;
   }
   
   return dest;
}

/*
 * Converts the RGB image with alpha channel rgba to an RGB image rgb.
 * The RGB values are copied and the alpha channel is stripped out.
 *
 * The RGB image is returned.
 */
static Gan_Image *
 extract_rgba_to_rgb ( const Gan_Image *rgba, unsigned r0s, unsigned c0s,
                       unsigned height, unsigned width,
                       Gan_Image *rgb, unsigned r0d, unsigned c0d )
{
   unsigned int r, c;
   
   gan_err_test_ptr ( rgba->format == GAN_RGB_COLOUR_ALPHA_IMAGE && rgb->format == GAN_RGB_COLOUR_IMAGE,
                      "extract_rgba_to_rgb", GAN_ERROR_INCOMPATIBLE, "" );
   switch ( rgba->type )
   {
      case GAN_UINT8:
        switch ( rgb->type )
        {
           case GAN_UINT8:
           {
              Gan_RGBAPixel_ui8 pixa;
              Gan_RGBPixel_ui8 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pixa = gan_image_get_pix_rgba_ui8(rgba,r0s+r,c0s+c);
                    pix.R = pixa.R;
                    pix.G = pixa.G;
                    pix.B = pixa.B;
                    gan_image_set_pix_rgb_ui8(rgb,r0d+r,c0d+c,&pix);
                 }
           }
           break;

           case GAN_UINT16:
           {
              Gan_RGBAPixel_ui8 pixa;
              Gan_RGBPixel_ui16 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pixa = gan_image_get_pix_rgba_ui8(rgba,r0s+r,c0s+c);
                    pix.R = gan_pixel_ui8_to_ui16(pixa.R);
                    pix.G = gan_pixel_ui8_to_ui16(pixa.G);
                    pix.B = gan_pixel_ui8_to_ui16(pixa.B);
                    gan_image_set_pix_rgb_ui16(rgb,r0d+r,c0d+c,&pix);
                 }
           }
           break;

           case GAN_UINT32:
           {
              Gan_RGBAPixel_ui8 pixa;
              Gan_RGBPixel_ui32 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pixa = gan_image_get_pix_rgba_ui8(rgba,r0s+r,c0s+c);
                    pix.R = gan_pixel_ui8_to_ui32(pixa.R);
                    pix.G = gan_pixel_ui8_to_ui32(pixa.G);
                    pix.B = gan_pixel_ui8_to_ui32(pixa.B);
                    gan_image_set_pix_rgb_ui32(rgb,r0d+r,c0d+c,&pix);
                 }
           }
           break;

           case GAN_FLOAT32:
           {
              Gan_RGBAPixel_ui8 pixa;
              Gan_RGBPixel_f32 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pixa = gan_image_get_pix_rgba_ui8(rgba,r0s+r,c0s+c);
                    pix.R = gan_pixel_ui8_to_f32(pixa.R);
                    pix.G = gan_pixel_ui8_to_f32(pixa.G);
                    pix.B = gan_pixel_ui8_to_f32(pixa.B);
                    gan_image_set_pix_rgb_f32(rgb,r0d+r,c0d+c,&pix);
                 }
           }
           break;

           case GAN_FLOAT64:
           {
              Gan_RGBAPixel_ui8 pixa;
              Gan_RGBPixel_f64 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pixa = gan_image_get_pix_rgba_ui8(rgba,r0s+r,c0s+c);
                    pix.R = gan_pixel_ui8_to_f64(pixa.R);
                    pix.G = gan_pixel_ui8_to_f64(pixa.G);
                    pix.B = gan_pixel_ui8_to_f64(pixa.B);
                    gan_image_set_pix_rgb_f64(rgb,r0d+r,c0d+c,&pix);
                 }
           }
           break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_rgba_to_rgb", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             return NULL;
        }
        break;
        
      case GAN_UINT16:
        switch ( rgb->type )
        {
           case GAN_UINT8:
           {
              Gan_RGBAPixel_ui16 pixa;
              Gan_RGBPixel_ui8 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pixa = gan_image_get_pix_rgba_ui16(rgba,r0s+r,c0s+c);
                    pix.R = gan_pixel_ui16_to_ui8(pixa.R);
                    pix.G = gan_pixel_ui16_to_ui8(pixa.G);
                    pix.B = gan_pixel_ui16_to_ui8(pixa.B);
                    gan_image_set_pix_rgb_ui8(rgb,r0d+r,c0d+c,&pix);
                 }
           }
           break;

           case GAN_UINT16:
           {
              Gan_RGBAPixel_ui16 pixa;
              Gan_RGBPixel_ui16 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pixa = gan_image_get_pix_rgba_ui16(rgba,r0s+r,c0s+c);
                    pix.R = pixa.R;
                    pix.G = pixa.G;
                    pix.B = pixa.B;
                    gan_image_set_pix_rgb_ui16(rgb,r0d+r,c0d+c,&pix);
                 }
           }
           break;

           case GAN_UINT32:
           {
              Gan_RGBAPixel_ui16 pixa;
              Gan_RGBPixel_ui32 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pixa = gan_image_get_pix_rgba_ui16(rgba,r0s+r,c0s+c);
                    pix.R = gan_pixel_ui16_to_ui32(pixa.R);
                    pix.G = gan_pixel_ui16_to_ui32(pixa.G);
                    pix.B = gan_pixel_ui16_to_ui32(pixa.B);
                    gan_image_set_pix_rgb_ui32(rgb,r0d+r,c0d+c,&pix);
                 }
           }
           break;

           case GAN_FLOAT32:
           {
              Gan_RGBAPixel_ui16 pixa;
              Gan_RGBPixel_f32 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pixa = gan_image_get_pix_rgba_ui16(rgba,r0s+r,c0s+c);
                    pix.R = gan_pixel_ui16_to_f32(pixa.R);
                    pix.G = gan_pixel_ui16_to_f32(pixa.G);
                    pix.B = gan_pixel_ui16_to_f32(pixa.B);
                    gan_image_set_pix_rgb_f32(rgb,r0d+r,c0d+c,&pix);
                 }
           }
           break;

           case GAN_FLOAT64:
           {
              Gan_RGBAPixel_ui16 pixa;
              Gan_RGBPixel_f64 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pixa = gan_image_get_pix_rgba_ui16(rgba,r0s+r,c0s+c);
                    pix.R = gan_pixel_ui16_to_f64(pixa.R);
                    pix.G = gan_pixel_ui16_to_f64(pixa.G);
                    pix.B = gan_pixel_ui16_to_f64(pixa.B);
                    gan_image_set_pix_rgb_f64(rgb,r0d+r,c0d+c,&pix);
                 }
           }
           break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_rgba_to_rgb", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             return NULL;
        }
        break;

      case GAN_UINT32:
        switch ( rgb->type )
        {
           case GAN_UINT8:
           {
              Gan_RGBAPixel_ui32 pixa;
              Gan_RGBPixel_ui8 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pixa = gan_image_get_pix_rgba_ui32(rgba,r0s+r,c0s+c);
                    pix.R = gan_pixel_ui32_to_ui8(pixa.R);
                    pix.G = gan_pixel_ui32_to_ui8(pixa.G);
                    pix.B = gan_pixel_ui32_to_ui8(pixa.B);
                    gan_image_set_pix_rgb_ui8(rgb,r0d+r,c0d+c,&pix);
                 }
           }
           break;

           case GAN_UINT16:
           {
              Gan_RGBAPixel_ui32 pixa;
              Gan_RGBPixel_ui16 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pixa = gan_image_get_pix_rgba_ui32(rgba,r0s+r,c0s+c);
                    pix.R = gan_pixel_ui32_to_ui16(pixa.R);
                    pix.G = gan_pixel_ui32_to_ui16(pixa.G);
                    pix.B = gan_pixel_ui32_to_ui16(pixa.B);
                    gan_image_set_pix_rgb_ui16(rgb,r0d+r,c0d+c,&pix);
                 }
           }
           break;

           case GAN_UINT32:
           {
              Gan_RGBAPixel_ui32 pixa;
              Gan_RGBPixel_ui32 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pixa = gan_image_get_pix_rgba_ui32(rgba,r0s+r,c0s+c);
                    pix.R = pixa.R;
                    pix.G = pixa.G;
                    pix.B = pixa.B;
                    gan_image_set_pix_rgb_ui32(rgb,r0d+r,c0d+c,&pix);
                 }
           }
           break;

           case GAN_FLOAT32:
           {
              Gan_RGBAPixel_ui32 pixa;
              Gan_RGBPixel_f32 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pixa = gan_image_get_pix_rgba_ui32(rgba,r0s+r,c0s+c);
                    pix.R = gan_pixel_ui32_to_f32(pixa.R);
                    pix.G = gan_pixel_ui32_to_f32(pixa.G);
                    pix.B = gan_pixel_ui32_to_f32(pixa.B);
                    gan_image_set_pix_rgb_f32(rgb,r0d+r,c0d+c,&pix);
                 }
           }
           break;

           case GAN_FLOAT64:
           {
              Gan_RGBAPixel_ui32 pixa;
              Gan_RGBPixel_f64 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pixa = gan_image_get_pix_rgba_ui32(rgba,r0s+r,c0s+c);
                    pix.R = gan_pixel_ui32_to_f64(pixa.R);
                    pix.G = gan_pixel_ui32_to_f64(pixa.G);
                    pix.B = gan_pixel_ui32_to_f64(pixa.B);
                    gan_image_set_pix_rgb_f64(rgb,r0d+r,c0d+c,&pix);
                 }
           }
           break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_rgba_to_rgb", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             return NULL;
        }
        break;

      case GAN_FLOAT32:
        switch ( rgb->type )
        {
           case GAN_UINT8:
           {
              Gan_RGBAPixel_f32 pixa;
              Gan_RGBPixel_ui8 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pixa = gan_image_get_pix_rgba_f32(rgba,r0s+r,c0s+c);
                    pix.R = gan_pixel_f32_to_ui8(pixa.R);
                    pix.G = gan_pixel_f32_to_ui8(pixa.G);
                    pix.B = gan_pixel_f32_to_ui8(pixa.B);
                    gan_image_set_pix_rgb_ui8(rgb,r0d+r,c0d+c,&pix);
                 }
           }
           break;

           case GAN_UINT16:
           {
              Gan_RGBAPixel_f32 pixa;
              Gan_RGBPixel_ui16 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pixa = gan_image_get_pix_rgba_f32(rgba,r0s+r,c0s+c);
                    pix.R = gan_pixel_f32_to_ui16(pixa.R);
                    pix.G = gan_pixel_f32_to_ui16(pixa.G);
                    pix.B = gan_pixel_f32_to_ui16(pixa.B);
                    gan_image_set_pix_rgb_ui16(rgb,r0d+r,c0d+c,&pix);
                 }
           }
           break;

           case GAN_UINT32:
           {
              Gan_RGBAPixel_f32 pixa;
              Gan_RGBPixel_ui32 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pixa = gan_image_get_pix_rgba_f32(rgba,r0s+r,c0s+c);
                    pix.R = gan_pixel_f32_to_ui32(pixa.R);
                    pix.G = gan_pixel_f32_to_ui32(pixa.G);
                    pix.B = gan_pixel_f32_to_ui32(pixa.B);
                    gan_image_set_pix_rgb_ui32(rgb,r0d+r,c0d+c,&pix);
                 }
           }
           break;

           case GAN_FLOAT32:
           {
              Gan_RGBAPixel_f32 pixa;
              Gan_RGBPixel_f32 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pixa = gan_image_get_pix_rgba_f32(rgba,r0s+r,c0s+c);
                    pix.R = pixa.R;
                    pix.G = pixa.G;
                    pix.B = pixa.B;
                    gan_image_set_pix_rgb_f32(rgb,r0d+r,c0d+c,&pix);
                 }
           }
           break;

           case GAN_FLOAT64:
           {
              Gan_RGBAPixel_f32 pixa;
              Gan_RGBPixel_f64 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pixa = gan_image_get_pix_rgba_f32(rgba,r0s+r,c0s+c);
                    pix.R = (gan_float64)pixa.R;
                    pix.G = (gan_float64)pixa.G;
                    pix.B = (gan_float64)pixa.B;
                    gan_image_set_pix_rgb_f64(rgb,r0d+r,c0d+c,&pix);
                 }
           }
           break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_rgba_to_rgb", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             return NULL;
        }
        break;

      case GAN_FLOAT64:
        switch ( rgb->type )
        {
           case GAN_UINT8:
           {
              Gan_RGBAPixel_f64 pixa;
              Gan_RGBPixel_ui8 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pixa = gan_image_get_pix_rgba_f64(rgba,r0s+r,c0s+c);
                    pix.R = gan_pixel_f64_to_ui8(pixa.R);
                    pix.G = gan_pixel_f64_to_ui8(pixa.G);
                    pix.B = gan_pixel_f64_to_ui8(pixa.B);
                    gan_image_set_pix_rgb_ui8(rgb,r0d+r,c0d+c,&pix);
                 }
           }
           break;

           case GAN_UINT16:
           {
              Gan_RGBAPixel_f64 pixa;
              Gan_RGBPixel_ui16 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pixa = gan_image_get_pix_rgba_f64(rgba,r0s+r,c0s+c);
                    pix.R = gan_pixel_f64_to_ui16(pixa.R);
                    pix.G = gan_pixel_f64_to_ui16(pixa.G);
                    pix.B = gan_pixel_f64_to_ui16(pixa.B);
                    gan_image_set_pix_rgb_ui16(rgb,r0d+r,c0d+c,&pix);
                 }
           }
           break;

           case GAN_UINT32:
           {
              Gan_RGBAPixel_f64 pixa;
              Gan_RGBPixel_ui32 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pixa = gan_image_get_pix_rgba_f64(rgba,r0s+r,c0s+c);
                    pix.R = gan_pixel_f64_to_ui32(pixa.R);
                    pix.G = gan_pixel_f64_to_ui32(pixa.G);
                    pix.B = gan_pixel_f64_to_ui32(pixa.B);
                    gan_image_set_pix_rgb_ui32(rgb,r0d+r,c0d+c,&pix);
                 }
           }
           break;

           case GAN_FLOAT32:
           {
              Gan_RGBAPixel_f64 pixa;
              Gan_RGBPixel_f32 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pixa = gan_image_get_pix_rgba_f64(rgba,r0s+r,c0s+c);
                    pix.R = (gan_float32)pixa.R;
                    pix.G = (gan_float32)pixa.G;
                    pix.B = (gan_float32)pixa.B;
                    gan_image_set_pix_rgb_f32(rgb,r0d+r,c0d+c,&pix);
                 }
           }
           break;

           case GAN_FLOAT64:
           {
              Gan_RGBAPixel_f64 pixa;
              Gan_RGBPixel_f64 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pixa = gan_image_get_pix_rgba_f64(rgba,r0s+r,c0s+c);
                    pix.R = pixa.R;
                    pix.G = pixa.G;
                    pix.B = pixa.B;
                    gan_image_set_pix_rgb_f64(rgb,r0d+r,c0d+c,&pix);
                 }
           }
           break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_rgba_to_rgb", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             return NULL;
        }
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "extract_rgba_to_rgb", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
        return NULL;
   }

   /* success */
   return rgb;
}

/*
 * Converts the grey level image gla with alpha channel into RGB image
 * rgb. The alpha channel of gla is stripped out.
 */
static Gan_Image *
 extract_gla_to_rgb ( const Gan_Image *gla, unsigned r0s, unsigned c0s,
                      unsigned height, unsigned width,
                      Gan_Image *rgb, unsigned r0d, unsigned c0d )
{
   unsigned int r, c;
   
   gan_err_test_ptr ( gla->format == GAN_GREY_LEVEL_ALPHA_IMAGE && rgb->format == GAN_RGB_COLOUR_IMAGE,
                      "extract_gla_to_rgb", GAN_ERROR_INCOMPATIBLE, "" );
   switch ( gla->type )
   {
      case GAN_UINT8:
        switch ( rgb->type )
        {
           Gan_GLAPixel_ui8 pixa;

           case GAN_UINT8:
           {
              Gan_RGBPixel_ui8 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pixa = gan_image_get_pix_gla_ui8(gla,r0s+r,c0s+c);
                    pix.R = pix.G = pix.B = pixa.I;
                    gan_image_set_pix_rgb_ui8(rgb,r0d+r,c0d+c,&pix);
                 }
           }
           break;

           case GAN_UINT16:
           {
              Gan_RGBPixel_ui16 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pixa = gan_image_get_pix_gla_ui8(gla,r0s+r,c0s+c);
                    pix.R = pix.G = pix.B = gan_pixel_ui8_to_ui16(pixa.I);
                    gan_image_set_pix_rgb_ui16(rgb,r0d+r,c0d+c,&pix);
                 }
           }
           break;

           case GAN_UINT32:
           {
              Gan_RGBPixel_ui32 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pixa = gan_image_get_pix_gla_ui8(gla,r0s+r,c0s+c);
                    pix.R = pix.G = pix.B = gan_pixel_ui8_to_ui32(pixa.I);
                    gan_image_set_pix_rgb_ui32(rgb,r0d+r,c0d+c,&pix);
                 }
           }
           break;

           case GAN_FLOAT32:
           {
              Gan_RGBPixel_f32 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pixa = gan_image_get_pix_gla_ui8(gla,r0s+r,c0s+c);
                    pix.R = pix.G = pix.B = gan_pixel_ui8_to_f32(pixa.I);
                    gan_image_set_pix_rgb_f32(rgb,r0d+r,c0d+c,&pix);
                 }
           }
           break;

           case GAN_FLOAT64:
           {
              Gan_RGBPixel_f64 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pixa = gan_image_get_pix_gla_ui8(gla,r0s+r,c0s+c);
                    pix.R = pix.G = pix.B = gan_pixel_ui8_to_f64(pixa.I);
                    gan_image_set_pix_rgb_f64(rgb,r0d+r,c0d+c,&pix);
                 }
           }
           break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_gla_to_rgb", GAN_ERROR_ILLEGAL_IMAGE_TYPE,
                                "" );
             return NULL;
        }
        break;
        
      case GAN_UINT16:
        switch ( rgb->type )
        {
           Gan_GLAPixel_ui16 pixa;

           case GAN_UINT8:
           {
              Gan_RGBPixel_ui8 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pixa = gan_image_get_pix_gla_ui16(gla,r0s+r,c0s+c);
                    pix.R = pix.G = pix.B = gan_pixel_ui16_to_ui8(pixa.I);
                    gan_image_set_pix_rgb_ui8(rgb,r0d+r,c0d+c,&pix);
                 }
           }
           break;

           case GAN_UINT16:
           {
              Gan_RGBPixel_ui16 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pixa = gan_image_get_pix_gla_ui16(gla,r0s+r,c0s+c);
                    pix.R = pix.G = pix.B = pixa.I;
                    gan_image_set_pix_rgb_ui16(rgb,r0d+r,c0d+c,&pix);
                 }
           }
           break;

           case GAN_UINT32:
           {
              Gan_RGBPixel_ui32 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pixa = gan_image_get_pix_gla_ui16(gla,r0s+r,c0s+c);
                    pix.R = pix.G = pix.B = gan_pixel_ui16_to_ui32(pixa.I);
                    gan_image_set_pix_rgb_ui32(rgb,r0d+r,c0d+c,&pix);
                 }
           }
           break;

           case GAN_FLOAT32:
           {
              Gan_RGBPixel_f32 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pixa = gan_image_get_pix_gla_ui16(gla,r0s+r,c0s+c);
                    pix.R = pix.G = pix.B = gan_pixel_ui16_to_f32(pixa.I);
                    gan_image_set_pix_rgb_f32(rgb,r0d+r,c0d+c,&pix);
                 }
           }
           break;

           case GAN_FLOAT64:
           {
              Gan_RGBPixel_f64 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pixa = gan_image_get_pix_gla_ui16(gla,r0s+r,c0s+c);
                    pix.R = pix.G = pix.B = gan_pixel_ui16_to_f64(pixa.I);
                    gan_image_set_pix_rgb_f64(rgb,r0d+r,c0d+c,&pix);
                 }
           }
           break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_gla_to_rgb", GAN_ERROR_ILLEGAL_IMAGE_TYPE,
                                "" );
             return NULL;
        }
        break;

      case GAN_UINT32:
        switch ( rgb->type )
        {
           Gan_GLAPixel_ui32 pixa;

           case GAN_UINT8:
           {
              Gan_RGBPixel_ui8 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pixa = gan_image_get_pix_gla_ui32(gla,r0s+r,c0s+c);
                    pix.R = pix.G = pix.B = gan_pixel_ui32_to_ui8(pixa.I);
                    gan_image_set_pix_rgb_ui8(rgb,r0d+r,c0d+c,&pix);
                 }
           }
           break;

           case GAN_UINT16:
           {
              Gan_RGBPixel_ui16 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pixa = gan_image_get_pix_gla_ui32(gla,r0s+r,c0s+c);
                    pix.R = pix.G = pix.B = gan_pixel_ui32_to_ui16(pixa.I);
                    gan_image_set_pix_rgb_ui16(rgb,r0d+r,c0d+c,&pix);
                 }
           }
           break;

           case GAN_UINT32:
           {
              Gan_RGBPixel_ui32 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pixa = gan_image_get_pix_gla_ui32(gla,r0s+r,c0s+c);
                    pix.R = pix.G = pix.B = pixa.I;
                    gan_image_set_pix_rgb_ui32(rgb,r0d+r,c0d+c,&pix);
                 }
           }
           break;

           case GAN_FLOAT32:
           {
              Gan_RGBPixel_f32 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pixa = gan_image_get_pix_gla_ui32(gla,r0s+r,c0s+c);
                    pix.R = pix.G = pix.B = gan_pixel_ui32_to_f32(pixa.I);
                    gan_image_set_pix_rgb_f32(rgb,r0d+r,c0d+c,&pix);
                 }
           }
           break;

           case GAN_FLOAT64:
           {
              Gan_RGBPixel_f64 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pixa = gan_image_get_pix_gla_ui32(gla,r0s+r,c0s+c);
                    pix.R = pix.G = pix.B = gan_pixel_ui32_to_f64(pixa.I);
                    gan_image_set_pix_rgb_f64(rgb,r0d+r,c0d+c,&pix);
                 }
           }
           break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_gla_to_rgb", GAN_ERROR_ILLEGAL_IMAGE_TYPE,
                                "" );
             return NULL;
        }
        break;

      case GAN_FLOAT32:
        switch ( rgb->type )
        {
           Gan_GLAPixel_f32 pixa;

           case GAN_UINT8:
           {
              Gan_RGBPixel_ui8 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pixa = gan_image_get_pix_gla_f32(gla,r0s+r,c0s+c);
                    pix.R = pix.G = pix.B = gan_pixel_f32_to_ui8(pixa.I);
                    gan_image_set_pix_rgb_ui8(rgb,r0d+r,c0d+c,&pix);
                 }
           }
           break;

           case GAN_UINT16:
           {
              Gan_RGBPixel_ui16 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pixa = gan_image_get_pix_gla_f32(gla,r0s+r,c0s+c);
                    pix.R = pix.G = pix.B = gan_pixel_f32_to_ui16(pixa.I);
                    gan_image_set_pix_rgb_ui16(rgb,r0d+r,c0d+c,&pix);
                 }
           }
           break;

           case GAN_UINT32:
           {
              Gan_RGBPixel_ui32 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pixa = gan_image_get_pix_gla_f32(gla,r0s+r,c0s+c);
                    pix.R = pix.G = pix.B = gan_pixel_f32_to_ui32(pixa.I);
                    gan_image_set_pix_rgb_ui32(rgb,r0d+r,c0d+c,&pix);
                 }
           }
           break;

           case GAN_FLOAT32:
           {
              Gan_RGBPixel_f32 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pixa = gan_image_get_pix_gla_f32(gla,r0s+r,c0s+c);
                    pix.R = pix.G = pix.B = pixa.I;
                    gan_image_set_pix_rgb_f32(rgb,r0d+r,c0d+c,&pix);
                 }
           }
           break;

           case GAN_FLOAT64:
           {
              Gan_RGBPixel_f64 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pixa = gan_image_get_pix_gla_f32(gla,r0s+r,c0s+c);
                    pix.R = pix.G = pix.B = (gan_float64)pixa.I;
                    gan_image_set_pix_rgb_f64(rgb,r0d+r,c0d+c,&pix);
                 }
           }
           break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_gla_to_rgb", GAN_ERROR_ILLEGAL_IMAGE_TYPE,
                                "" );
             return NULL;
        }
        break;

      case GAN_FLOAT64:
        switch ( rgb->type )
        {
           Gan_GLAPixel_f64 pixa;

           case GAN_UINT8:
           {
              Gan_RGBPixel_ui8 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pixa = gan_image_get_pix_gla_f64(gla,r0s+r,c0s+c);
                    pix.R = pix.G = pix.B = gan_pixel_f64_to_ui8(pixa.I);
                    gan_image_set_pix_rgb_ui8(rgb,r0d+r,c0d+c,&pix);
                 }
           }
           break;

           case GAN_UINT16:
           {
              Gan_RGBPixel_ui16 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pixa = gan_image_get_pix_gla_f64(gla,r0s+r,c0s+c);
                    pix.R = pix.G = pix.B = gan_pixel_f64_to_ui16(pixa.I);
                    gan_image_set_pix_rgb_ui16(rgb,r0d+r,c0d+c,&pix);
                 }
           }
           break;

           case GAN_UINT32:
           {
              Gan_RGBPixel_ui32 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pixa = gan_image_get_pix_gla_f64(gla,r0s+r,c0s+c);
                    pix.R = pix.G = pix.B = gan_pixel_f64_to_ui32(pixa.I);
                    gan_image_set_pix_rgb_ui32(rgb,r0d+r,c0d+c,&pix);
                 }
           }
           break;

           case GAN_FLOAT32:
           {
              Gan_RGBPixel_f32 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pixa = gan_image_get_pix_gla_f64(gla,r0s+r,c0s+c);
                    pix.R = pix.G = pix.B = (gan_float32)pixa.I;
                    gan_image_set_pix_rgb_f32(rgb,r0d+r,c0d+c,&pix);
                 }
           }
           break;

           case GAN_FLOAT64:
           {
              Gan_RGBPixel_f64 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pixa = gan_image_get_pix_gla_f64(gla,r0s+r,c0s+c);
                    pix.R = pix.G = pix.B = pixa.I;
                    gan_image_set_pix_rgb_f64(rgb,r0d+r,c0d+c,&pix);
                 }
           }
           break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_gla_to_rgb", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             return NULL;
        }
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "extract_gla_to_rgb", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
        return NULL;
   }

   /* success */
   return rgb;
}

/*
 * Converts the grey level image gla with alpha channel into RGB image
 * with alpha channel rgba.
 */
static Gan_Image *
 extract_gla_to_rgba ( const Gan_Image *gla, unsigned r0s, unsigned c0s,
                       unsigned height, unsigned width,
                       Gan_Image *rgba, unsigned r0d, unsigned c0d )
{
   unsigned int r, c;
   
   gan_err_test_ptr ( gla->format == GAN_GREY_LEVEL_ALPHA_IMAGE && rgba->format == GAN_RGB_COLOUR_ALPHA_IMAGE,
                      "extract_gla_to_rgba", GAN_ERROR_INCOMPATIBLE, "" );
   switch ( gla->type )
   {
      case GAN_UINT8:
        switch ( rgba->type )
        {
           Gan_GLAPixel_ui8 pixa;

           case GAN_UINT8:
           {
              Gan_RGBAPixel_ui8 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pixa = gan_image_get_pix_gla_ui8(gla,r0s+r,c0s+c);
                    pix.R = pix.G = pix.B = pixa.I;
                    pix.A = pixa.A;
                    gan_image_set_pix_rgba_ui8(rgba,r0d+r,c0d+c,&pix);
                 }
           }
           break;

           case GAN_UINT16:
           {
              Gan_RGBAPixel_ui16 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pixa = gan_image_get_pix_gla_ui8(gla,r0s+r,c0s+c);
                    pix.R = pix.G = pix.B = gan_pixel_ui8_to_ui16(pixa.I);
                    pix.A = gan_pixel_ui8_to_ui16(pixa.A);
                    gan_image_set_pix_rgba_ui16(rgba,r0d+r,c0d+c,&pix);
                 }
           }
           break;

           case GAN_UINT32:
           {
              Gan_RGBAPixel_ui32 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pixa = gan_image_get_pix_gla_ui8(gla,r0s+r,c0s+c);
                    pix.R = pix.G = pix.B = gan_pixel_ui8_to_ui32(pixa.I);
                    pix.A = gan_pixel_ui8_to_ui32(pixa.A);
                    gan_image_set_pix_rgba_ui32(rgba,r0d+r,c0d+c,&pix);
                 }
           }
           break;

           case GAN_FLOAT32:
           {
              Gan_RGBAPixel_f32 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pixa = gan_image_get_pix_gla_ui8(gla,r0s+r,c0s+c);
                    pix.R = pix.G = pix.B = gan_pixel_ui8_to_f32(pixa.I);
                    pix.A = gan_pixel_ui8_to_f32(pixa.A);
                    gan_image_set_pix_rgba_f32(rgba,r0d+r,c0d+c,&pix);
                 }
           }
           break;

           case GAN_FLOAT64:
           {
              Gan_RGBAPixel_f64 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pixa = gan_image_get_pix_gla_ui8(gla,r0s+r,c0s+c);
                    pix.R = pix.G = pix.B = gan_pixel_ui8_to_f64(pixa.I);
                    pix.A = gan_pixel_ui8_to_f64(pixa.A);
                    gan_image_set_pix_rgba_f64(rgba,r0d+r,c0d+c,&pix);
                 }
           }
           break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_gla_to_rgba", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             return NULL;
        }
        break;
        
      case GAN_UINT16:
        switch ( rgba->type )
        {
           Gan_GLAPixel_ui16 pixa;

           case GAN_UINT8:
           {
              Gan_RGBAPixel_ui8 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pixa = gan_image_get_pix_gla_ui16(gla,r0s+r,c0s+c);
                    pix.R = pix.G = pix.B = gan_pixel_ui16_to_ui8(pixa.I);
                    pix.A = gan_pixel_ui16_to_ui8(pixa.A);
                    gan_image_set_pix_rgba_ui8(rgba,r0d+r,c0d+c,&pix);
                 }
           }
           break;

           case GAN_UINT16:
           {
              Gan_RGBAPixel_ui16 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pixa = gan_image_get_pix_gla_ui16(gla,r0s+r,c0s+c);
                    pix.R = pix.G = pix.B = pixa.I;
                    pix.A = pixa.A;
                    gan_image_set_pix_rgba_ui16(rgba,r0d+r,c0d+c,&pix);
                 }
           }
           break;

           case GAN_UINT32:
           {
              Gan_RGBAPixel_ui32 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pixa = gan_image_get_pix_gla_ui16(gla,r0s+r,c0s+c);
                    pix.R = pix.G = pix.B = gan_pixel_ui16_to_ui32(pixa.I);
                    pix.A = gan_pixel_ui16_to_ui32(pixa.A);
                    gan_image_set_pix_rgba_ui32(rgba,r0d+r,c0d+c,&pix);
                 }
           }
           break;

           case GAN_FLOAT32:
           {
              Gan_RGBAPixel_f32 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pixa = gan_image_get_pix_gla_ui16(gla,r0s+r,c0s+c);
                    pix.R = pix.G = pix.B = gan_pixel_ui16_to_f32(pixa.I);
                    pix.A = gan_pixel_ui16_to_f32(pixa.A);
                    gan_image_set_pix_rgba_f32(rgba,r0d+r,c0d+c,&pix);
                 }
           }
           break;

           case GAN_FLOAT64:
           {
              Gan_RGBAPixel_f64 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pixa = gan_image_get_pix_gla_ui16(gla,r0s+r,c0s+c);
                    pix.R = pix.G = pix.B = gan_pixel_ui16_to_f64(pixa.I);
                    pix.A = gan_pixel_ui16_to_f64(pixa.A);
                    gan_image_set_pix_rgba_f64(rgba,r0d+r,c0d+c,&pix);
                 }
           }
           break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_gla_to_rgba", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             return NULL;
        }
        break;

      case GAN_UINT32:
        switch ( rgba->type )
        {
           Gan_GLAPixel_ui32 pixa;

           case GAN_UINT8:
           {
              Gan_RGBAPixel_ui8 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pixa = gan_image_get_pix_gla_ui32(gla,r0s+r,c0s+c);
                    pix.R = pix.G = pix.B = gan_pixel_ui32_to_ui8(pixa.I);
                    pix.A = gan_pixel_ui32_to_ui8(pixa.A);
                    gan_image_set_pix_rgba_ui8(rgba,r0d+r,c0d+c,&pix);
                 }
           }
           break;

           case GAN_UINT16:
           {
              Gan_RGBAPixel_ui16 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pixa = gan_image_get_pix_gla_ui32(gla,r0s+r,c0s+c);
                    pix.R = pix.G = pix.B = gan_pixel_ui32_to_ui16(pixa.I);
                    pix.A = gan_pixel_ui32_to_ui16(pixa.A);
                    gan_image_set_pix_rgba_ui16(rgba,r0d+r,c0d+c,&pix);
                 }
           }
           break;

           case GAN_UINT32:
           {
              Gan_RGBAPixel_ui32 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pixa = gan_image_get_pix_gla_ui32(gla,r0s+r,c0s+c);
                    pix.R = pix.G = pix.B = pixa.I;
                    pix.A = pixa.A;
                    gan_image_set_pix_rgba_ui32(rgba,r0d+r,c0d+c,&pix);
                 }
           }
           break;

           case GAN_FLOAT32:
           {
              Gan_RGBAPixel_f32 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pixa = gan_image_get_pix_gla_ui32(gla,r0s+r,c0s+c);
                    pix.R = pix.G = pix.B = gan_pixel_ui32_to_f32(pixa.I);
                    pix.A = gan_pixel_ui32_to_f32(pixa.A);
                    gan_image_set_pix_rgba_f32(rgba,r0d+r,c0d+c,&pix);
                 }
           }
           break;

           case GAN_FLOAT64:
           {
              Gan_RGBAPixel_f64 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pixa = gan_image_get_pix_gla_ui32(gla,r0s+r,c0s+c);
                    pix.R = pix.G = pix.B = gan_pixel_ui32_to_f64(pixa.I);
                    pix.A = gan_pixel_ui32_to_f64(pixa.A);
                    gan_image_set_pix_rgba_f64(rgba,r0d+r,c0d+c,&pix);
                 }
           }
           break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_gla_to_rgba", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             return NULL;
        }
        break;

      case GAN_FLOAT32:
        switch ( rgba->type )
        {
           Gan_GLAPixel_f32 pixa;

           case GAN_UINT8:
           {
              Gan_RGBAPixel_ui8 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pixa = gan_image_get_pix_gla_f32(gla,r0s+r,c0s+c);
                    pix.R = pix.G = pix.B = gan_pixel_f32_to_ui8(pixa.I);
                    pix.A = gan_pixel_f32_to_ui8(pixa.A);
                    gan_image_set_pix_rgba_ui8(rgba,r0d+r,c0d+c,&pix);
                 }
           }
           break;

           case GAN_UINT16:
           {
              Gan_RGBAPixel_ui16 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pixa = gan_image_get_pix_gla_f32(gla,r0s+r,c0s+c);
                    pix.R = pix.G = pix.B = gan_pixel_f32_to_ui16(pixa.I);
                    pix.A = gan_pixel_f32_to_ui16(pixa.A);
                    gan_image_set_pix_rgba_ui16(rgba,r0d+r,c0d+c,&pix);
                 }
           }
           break;

           case GAN_UINT32:
           {
              Gan_RGBAPixel_ui32 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pixa = gan_image_get_pix_gla_f32(gla,r0s+r,c0s+c);
                    pix.R = pix.G = pix.B = gan_pixel_f32_to_ui32(pixa.I);
                    pix.A = gan_pixel_f32_to_ui32(pixa.A);
                    gan_image_set_pix_rgba_ui32(rgba,r0d+r,c0d+c,&pix);
                 }
           }
           break;

           case GAN_FLOAT32:
           {
              Gan_RGBAPixel_f32 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pixa = gan_image_get_pix_gla_f32(gla,r0s+r,c0s+c);
                    pix.R = pix.G = pix.B = pixa.I;
                    pix.A = pixa.A;
                    gan_image_set_pix_rgba_f32(rgba,r0d+r,c0d+c,&pix);
                 }
           }
           break;

           case GAN_FLOAT64:
           {
              Gan_RGBAPixel_f64 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pixa = gan_image_get_pix_gla_f32(gla,r0s+r,c0s+c);
                    pix.R = pix.G = pix.B = (gan_float64)pixa.I;
                    pix.A = (gan_float64)pixa.A;
                    gan_image_set_pix_rgba_f64(rgba,r0d+r,c0d+c,&pix);
                 }
           }
           break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_gla_to_rgba", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             return NULL;
        }
        break;

      case GAN_FLOAT64:
        switch ( rgba->type )
        {
           Gan_GLAPixel_f64 pixa;

           case GAN_UINT8:
           {
              Gan_RGBAPixel_ui8 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pixa = gan_image_get_pix_gla_f64(gla,r0s+r,c0s+c);
                    pix.R = pix.G = pix.B = gan_pixel_f64_to_ui8(pixa.I);
                    pix.A = gan_pixel_f64_to_ui8(pixa.A);
                    gan_image_set_pix_rgba_ui8(rgba,r0d+r,c0d+c,&pix);
                 }
           }
           break;

           case GAN_UINT16:
           {
              Gan_RGBAPixel_ui16 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pixa = gan_image_get_pix_gla_f64(gla,r0s+r,c0s+c);
                    pix.R = pix.G = pix.B = gan_pixel_f64_to_ui16(pixa.I);
                    pix.A = gan_pixel_f64_to_ui16(pixa.A);
                    gan_image_set_pix_rgba_ui16(rgba,r0d+r,c0d+c,&pix);
                 }
           }
           break;

           case GAN_UINT32:
           {
              Gan_RGBAPixel_ui32 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pixa = gan_image_get_pix_gla_f64(gla,r0s+r,c0s+c);
                    pix.R = pix.G = pix.B = gan_pixel_f64_to_ui32(pixa.I);
                    pix.A = gan_pixel_f64_to_ui32(pixa.A);
                    gan_image_set_pix_rgba_ui32(rgba,r0d+r,c0d+c,&pix);
                 }
           }
           break;

           case GAN_FLOAT32:
           {
              Gan_RGBAPixel_f32 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pixa = gan_image_get_pix_gla_f64(gla,r0s+r,c0s+c);
                    pix.R = pix.G = pix.B = (gan_float32)pixa.I;
                    pix.A = (gan_float32)pixa.A;
                    gan_image_set_pix_rgba_f32(rgba,r0d+r,c0d+c,&pix);
                 }
           }
           break;

           case GAN_FLOAT64:
           {
              Gan_RGBAPixel_f64 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pixa = gan_image_get_pix_gla_f64(gla,r0s+r,c0s+c);
                    pix.R = pix.G = pix.B = pixa.I;
                    pix.A = pixa.A;
                    gan_image_set_pix_rgba_f64(rgba,r0d+r,c0d+c,&pix);
                 }
           }
           break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_gla_to_rgba", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             return NULL;
        }
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "extract_gla_to_rgba", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
        return NULL;
   }

   /* success */
   return rgba;
}

/* Converts an RGB image with alpha channel to a different type
 *
 * The converted RGB image is returned.
 */
static Gan_Image *
 extract_rgba ( const Gan_Image *source, unsigned r0s, unsigned c0s,
                unsigned height, unsigned width, Gan_Bool copy_pixels,
                Gan_Image *dest, unsigned r0d, unsigned c0d )
{
   unsigned int r, c;
   
   gan_err_test_ptr ( source->format == GAN_RGB_COLOUR_ALPHA_IMAGE && dest->format == GAN_RGB_COLOUR_ALPHA_IMAGE,
                      "extract_rgba", GAN_ERROR_INCOMPATIBLE, "" );
   
   switch ( dest->type )
   {
      case GAN_UINT8:
        switch ( source->type )
        {
           Gan_RGBAPixel_ui8 ui8pix;

           case GAN_UINT8:
             if ( copy_pixels )
                for ( r=0; r < height; r++ )
                   gan_copy_array_ui8 ((gan_uint8 *)&source->row_data.rgba.ui8[r0s+r][c0s], 1, 4*width, (gan_uint8 *)
                                       &dest->row_data.rgba.ui8[r0d+r][c0d],1);
             else
             {
                /* make destination image point into source image */
                gan_err_test_ptr ( r0d == 0 && c0d == 0, "extract_rgba", GAN_ERROR_INCOMPATIBLE, "" );
                for ( r=0; r < height; r++ )
                   dest->row_data.rgba.ui8[r] = gan_image_get_pixptr_rgba_ui8(source, r0s+r, c0s);

                dest->pix_data_ptr = (unsigned char *)gan_image_get_pixptr_rgba_ui8(source, r0s, c0s);
             }
             break;
         
           case GAN_UINT12:
           {
              Gan_RGBAPixel_ui12 ui12pix;
            
              gan_err_test_ptr ( copy_pixels, "extract_rgba", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    /* clip RGBA pixel values to gan_uint8 limits */
                    ui12pix = gan_image_get_pix_rgba_ui12 ( source, r0s+r, c0s+c );
                    ui8pix.R = ui12pix.R >> 4;
                    ui8pix.G = ui12pix.G >> 4;
                    ui8pix.B = ui12pix.B >> 4;
                    ui8pix.A = ui12pix.A >> 4;
                    gan_image_set_pix_rgba_ui8 ( dest, r0d+r, c0d+c, &ui8pix );
                 }
           }
           break;
         
           case GAN_UINT16:
           {
              Gan_RGBAPixel_ui16 ui16pix;
            
              gan_err_test_ptr ( copy_pixels, "extract_rgba", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    /* clip RGBA pixel values to gan_uint8 limits */
                    ui16pix = gan_image_get_pix_rgba_ui16 ( source, r0s+r, c0s+c );
                    ui8pix.R = gan_pixel_ui16_to_ui8(ui16pix.R);
                    ui8pix.G = gan_pixel_ui16_to_ui8(ui16pix.G);
                    ui8pix.B = gan_pixel_ui16_to_ui8(ui16pix.B);
                    ui8pix.A = gan_pixel_ui16_to_ui8(ui16pix.A);
                    gan_image_set_pix_rgba_ui8 ( dest, r0d+r, c0d+c, &ui8pix );
                 }
           }
           break;
         
           case GAN_UINT32:
           {
              Gan_RGBAPixel_ui32 ui32pix;
            
              gan_err_test_ptr ( copy_pixels, "extract_rgba", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    /* clip RGBA pixel values to gan_uint8 limits */
                    ui32pix = gan_image_get_pix_rgba_ui32 ( source, r0s+r, c0s+c );
                    ui8pix.R = gan_pixel_ui32_to_ui8(ui32pix.R);
                    ui8pix.G = gan_pixel_ui32_to_ui8(ui32pix.G);
                    ui8pix.B = gan_pixel_ui32_to_ui8(ui32pix.B);
                    ui8pix.A = gan_pixel_ui32_to_ui8(ui32pix.A);
                    gan_image_set_pix_rgba_ui8 ( dest, r0d+r, c0d+c, &ui8pix );
                 }
           }
           break;
         
           case GAN_FLOAT32:
           {
              Gan_RGBAPixel_f32 f32pix;
            
              gan_err_test_ptr ( copy_pixels, "extract_rgba", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    /* clip RGBA pixel values to gan_uint8 limits */
                    f32pix = gan_image_get_pix_rgba_f32 ( source, r0s+r, c0s+c );
                    ui8pix.R = gan_pixel_f32_to_ui8(f32pix.R);
                    ui8pix.G = gan_pixel_f32_to_ui8(f32pix.G);
                    ui8pix.B = gan_pixel_f32_to_ui8(f32pix.B);
                    ui8pix.A = gan_pixel_f32_to_ui8(f32pix.A);
                    gan_image_set_pix_rgba_ui8 ( dest, r0d+r, c0d+c, &ui8pix );
                 }
           }
           break;
         
           case GAN_FLOAT64:
           {
              Gan_RGBAPixel_f64 f64pix;
            
              gan_err_test_ptr ( copy_pixels, "extract_rgba", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    /* clip RGBA pixel values to gan_uint8 limits */
                    f64pix = gan_image_get_pix_rgba_f64 ( source, r0s+r, c0s+c );
                    ui8pix.R = gan_pixel_f64_to_ui8(f64pix.R);
                    ui8pix.G = gan_pixel_f64_to_ui8(f64pix.G);
                    ui8pix.B = gan_pixel_f64_to_ui8(f64pix.B);
                    ui8pix.A = gan_pixel_f64_to_ui8(f64pix.A);
                    gan_image_set_pix_rgba_ui8 ( dest, r0d+r, c0d+c, &ui8pix );
                 }
           }
           break;
         
           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_rgba", GAN_ERROR_NOT_IMPLEMENTED,
                                "" );
             return NULL;
        }
        break;
      
      case GAN_UINT12:
        switch ( source->type )
        {
           Gan_RGBAPixel_ui12 ui12pix;

           case GAN_UINT8:
           {
              Gan_RGBAPixel_ui8 ui8pix;
            
              gan_err_test_ptr ( copy_pixels, "extract_rgba", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    /* clip RGBA pixel values to gan_uint12 limits */
                    ui8pix = gan_image_get_pix_rgba_ui8 ( source, r0s+r, c0s+c );
                    ui12pix.R = ui8pix.R << 4;
                    ui12pix.G = ui8pix.G << 4;
                    ui12pix.B = ui8pix.B << 4;
                    ui12pix.A = ui8pix.A << 4;
                    gan_image_set_pix_rgba_ui12 ( dest, r0d+r, c0d+c, &ui12pix );
                 }
           }
           break;
         
           case GAN_UINT12:
             if ( copy_pixels )
                for ( r=0; r < height; r++ )
                   gan_copy_array_ui8 ((gan_uint8 *)&source->row_data.rgba.ui12[r0s+r][c0s], 1, 8*width,
                                       (gan_uint8 *)&dest->row_data.rgba.ui12[r0d+r][c0d], 1);
             else
             {
                /* make destination image point into source image */
                gan_err_test_ptr ( r0d == 0 && c0d == 0, "extract_rgba", GAN_ERROR_INCOMPATIBLE, "" );
                for ( r=0; r < height; r++ )
                   dest->row_data.rgba.ui12[r] = gan_image_get_pixptr_rgba_ui12(source, r0s+r, c0s);

                dest->pix_data_ptr = (unsigned char *)gan_image_get_pixptr_rgba_ui12(source, r0s, c0s);
             }
             break;
         
           case GAN_UINT16:
           {
              Gan_RGBAPixel_ui16 ui16pix;
            
              gan_err_test_ptr ( copy_pixels, "extract_rgba", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    /* clip RGBA pixel values to gan_uint12 limits */
                    ui16pix = gan_image_get_pix_rgba_ui16 ( source, r0s+r, c0s+c );
                    ui12pix.R = ui16pix.R >> 4;
                    ui12pix.G = ui16pix.G >> 4;
                    ui12pix.B = ui16pix.B >> 4;
                    ui12pix.A = ui16pix.A >> 4;
                    gan_image_set_pix_rgba_ui12 ( dest, r0d+r, c0d+c, &ui12pix );
                 }
           }
           break;
         
           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_rgba", GAN_ERROR_NOT_IMPLEMENTED, "" );
             return NULL;
        }
        break;
      
      case GAN_UINT16:
        switch ( source->type )
        {
           Gan_RGBAPixel_ui16 ui16pix;

           case GAN_UINT8:
           {
              Gan_RGBAPixel_ui8 ui8pix;
            
              gan_err_test_ptr ( copy_pixels, "extract_rgba", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    /* clip RGBA pixel values to gan_uint8 limits */
                    ui8pix = gan_image_get_pix_rgba_ui8 ( source, r0s+r, c0s+c );
                    ui16pix.R = gan_pixel_ui8_to_ui16(ui8pix.R);
                    ui16pix.G = gan_pixel_ui8_to_ui16(ui8pix.G);
                    ui16pix.B = gan_pixel_ui8_to_ui16(ui8pix.B);
                    ui16pix.A = gan_pixel_ui8_to_ui16(ui8pix.A);
                    gan_image_set_pix_rgba_ui16 ( dest, r0d+r, c0d+c, &ui16pix );
                 }
           }
           break;
         
           case GAN_UINT12:
           {
              Gan_RGBAPixel_ui12 ui12pix;
            
              gan_err_test_ptr ( copy_pixels, "extract_rgba", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    /* clip RGBA pixel values to gan_uint16 limits */
                    ui12pix = gan_image_get_pix_rgba_ui12 ( source, r0s+r, c0s+c );
                    ui16pix.R = ui12pix.R << 4;
                    ui16pix.G = ui12pix.G << 4;
                    ui16pix.B = ui12pix.B << 4;
                    ui16pix.A = ui12pix.A << 4;
                    gan_image_set_pix_rgba_ui16 ( dest, r0d+r, c0d+c, &ui16pix );
                 }
           }
           break;
         
           case GAN_UINT16:
             if ( copy_pixels )
                for ( r=0; r < height; r++ )
                   gan_copy_array_ui16 ((gan_uint16 *)&source->row_data.rgba.ui16[r0s+r][c0s], 1, 4*width, (gan_uint16 *)
                                        &dest->row_data.rgba.ui16[r0d+r][c0d], 1);
             else
             {
                /* make destination image point into source image */
                gan_err_test_ptr ( r0d == 0 && c0d == 0, "extract_rgba", GAN_ERROR_INCOMPATIBLE, "" );
                for ( r=0; r < height; r++ )
                   dest->row_data.rgba.ui16[r] = &source->row_data.rgba.ui16[r0s+r][c0s];

                dest->pix_data_ptr = (unsigned char *)gan_image_get_pixptr_rgba_ui16(source, r0s, c0s);
             }
             break;

           case GAN_UINT32:
           {
              Gan_RGBAPixel_ui32 ui32pix;
            
              gan_err_test_ptr ( copy_pixels, "extract_rgba",
                                 GAN_ERROR_ILLEGAL_ARGUMENT, "" );
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    /* clip RGBA pixel values to gan_uint8 limits */
                    ui32pix = gan_image_get_pix_rgba_ui32 ( source, r0s+r, c0s+c );
                    ui16pix.R = gan_pixel_ui32_to_ui16(ui32pix.R);
                    ui16pix.G = gan_pixel_ui32_to_ui16(ui32pix.G);
                    ui16pix.B = gan_pixel_ui32_to_ui16(ui32pix.B);
                    ui16pix.A = gan_pixel_ui32_to_ui16(ui32pix.A);
                    gan_image_set_pix_rgba_ui16 ( dest, r0d+r, c0d+c, &ui16pix );
                 }
           }
           break;
         
           case GAN_FLOAT32:
           {
              Gan_RGBAPixel_f32 f32pix;
            
              gan_err_test_ptr ( copy_pixels, "extract_rgba", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    /* clip RGBA pixel values to gan_uint8 limits */
                    f32pix = gan_image_get_pix_rgba_f32 ( source, r0s+r, c0s+c );
                    ui16pix.R = gan_pixel_f32_to_ui16(f32pix.R);
                    ui16pix.G = gan_pixel_f32_to_ui16(f32pix.G);
                    ui16pix.B = gan_pixel_f32_to_ui16(f32pix.B);
                    ui16pix.A = gan_pixel_f32_to_ui16(f32pix.A);
                    gan_image_set_pix_rgba_ui16 ( dest, r0d+r, c0d+c, &ui16pix );
                 }
           }
           break;
         
           case GAN_FLOAT64:
           {
              Gan_RGBAPixel_f64 f64pix;
            
              gan_err_test_ptr ( copy_pixels, "extract_rgba", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    /* clip RGBA pixel values to gan_uint8 limits */
                    f64pix = gan_image_get_pix_rgba_f64 ( source, r0s+r, c0s+c );
                    ui16pix.R = gan_pixel_f64_to_ui16(f64pix.R);
                    ui16pix.G = gan_pixel_f64_to_ui16(f64pix.G);
                    ui16pix.B = gan_pixel_f64_to_ui16(f64pix.B);
                    ui16pix.A = gan_pixel_f64_to_ui16(f64pix.A);
                    gan_image_set_pix_rgba_ui16 ( dest, r0d+r, c0d+c, &ui16pix );
                 }
           }
           break;
         
           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_rgba", GAN_ERROR_NOT_IMPLEMENTED, "");
             return NULL;
        }
        break;
      
      case GAN_INT32:
        switch ( source->type )
        {
           Gan_RGBAPixel_i32 i32pix;

           case GAN_INT32:
             if ( copy_pixels )
                for ( r=0; r < height; r++ )
                   gan_copy_array_i32 ((gan_int32 *)&source->row_data.rgba.i32[r0s+r][c0s], 1, 4*width, (gan_int32 *)
                                       &dest->row_data.rgba.i32[r0d+r][c0d], 1);
             else
             {
                /* make destination image point into source image */
                gan_err_test_ptr ( r0d == 0 && c0d == 0, "extract_rgba", GAN_ERROR_INCOMPATIBLE, "" );
                for ( r=0; r < height; r++ )
                   dest->row_data.rgba.i32[r] = gan_image_get_pixptr_rgba_i32(source, r0s+r, c0s);

                dest->pix_data_ptr = (unsigned char *)gan_image_get_pixptr_rgba_i32(source, r0s, c0s);
             }
             break;
         
           case GAN_FLOAT32:
           {
              Gan_RGBAPixel_f32 f32pix;
            
              gan_err_test_ptr ( copy_pixels, "extract_rgba",
                                 GAN_ERROR_ILLEGAL_ARGUMENT, "" );
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    /* clip RGBA pixel values to gan_uint8 limits */
                    f32pix = gan_image_get_pix_rgba_f32 ( source, r0s+r, c0s+c );
                    i32pix.R = gan_pixel_f32_to_i32(f32pix.R);
                    i32pix.G = gan_pixel_f32_to_i32(f32pix.G);
                    i32pix.B = gan_pixel_f32_to_i32(f32pix.B);
                    i32pix.A = gan_pixel_f32_to_i32(f32pix.A);
                    gan_image_set_pix_rgba_i32 ( dest, r0d+r, c0d+c, &i32pix );
                 }
           }
           break;
         
           case GAN_FLOAT64:
           {
              Gan_RGBAPixel_f64 f64pix;
            
              gan_err_test_ptr ( copy_pixels, "extract_rgba", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    /* clip RGBA pixel values to gan_uint8 limits */
                    f64pix = gan_image_get_pix_rgba_f64 ( source, r0s+r, c0s+c );
                    i32pix.R = gan_pixel_f64_to_i32(f64pix.R);
                    i32pix.G = gan_pixel_f64_to_i32(f64pix.G);
                    i32pix.B = gan_pixel_f64_to_i32(f64pix.B);
                    i32pix.A = gan_pixel_f64_to_i32(f64pix.A);
                    gan_image_set_pix_rgba_i32 ( dest, r0d+r, c0d+c, &i32pix );
                 }
           }
           break;
         
           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_rgba", GAN_ERROR_NOT_IMPLEMENTED, "");
             return NULL;
        }
        break;
      
      case GAN_UINT32:
        switch ( source->type )
        {
           Gan_RGBAPixel_ui32 ui32pix;

           case GAN_UINT8:
           {
              Gan_RGBAPixel_ui8 ui8pix;
            
              gan_err_test_ptr ( copy_pixels, "extract_rgba", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    /* clip RGBA pixel values to gan_uint8 limits */
                    ui8pix = gan_image_get_pix_rgba_ui8 ( source, r0s+r, c0s+c );
                    ui32pix.R = gan_pixel_ui8_to_ui32(ui8pix.R);
                    ui32pix.G = gan_pixel_ui8_to_ui32(ui8pix.G);
                    ui32pix.B = gan_pixel_ui8_to_ui32(ui8pix.B);
                    ui32pix.A = gan_pixel_ui8_to_ui32(ui8pix.A);
                    gan_image_set_pix_rgba_ui32 ( dest, r0d+r, c0d+c, &ui32pix );
                 }
           }
           break;
         
           case GAN_UINT16:
           {
              Gan_RGBAPixel_ui16 ui16pix;
            
              gan_err_test_ptr ( copy_pixels, "extract_rgba",
                                 GAN_ERROR_ILLEGAL_ARGUMENT, "" );
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    /* clip RGBA pixel values to gan_uint8 limits */
                    ui16pix = gan_image_get_pix_rgba_ui16 ( source, r0s+r, c0s+c );
                    ui32pix.R = gan_pixel_ui16_to_ui32(ui16pix.R);
                    ui32pix.G = gan_pixel_ui16_to_ui32(ui16pix.G);
                    ui32pix.B = gan_pixel_ui16_to_ui32(ui16pix.B);
                    ui32pix.A = gan_pixel_ui16_to_ui32(ui16pix.A);
                    gan_image_set_pix_rgba_ui32 ( dest, r0d+r, c0d+c, &ui32pix );
                 }
           }
           break;
         
           case GAN_UINT32:
             if ( copy_pixels )
                for ( r=0; r < height; r++ )
                   gan_copy_array_ui32 ((gan_uint32 *)&source->row_data.rgba.ui32[r0s+r][c0s], 1, 4*width, (gan_uint32 *)
                                        &dest->row_data.rgba.ui32[r0d+r][c0d], 1);
             else
             {
                /* make destination image point into source image */
                gan_err_test_ptr ( r0d == 0 && c0d == 0, "extract_rgba", GAN_ERROR_INCOMPATIBLE, "" );
                for ( r=0; r < height; r++ )
                   dest->row_data.rgba.ui32[r] = &source->row_data.rgba.ui32[r0s+r][c0s];

                dest->pix_data_ptr = (unsigned char *)gan_image_get_pixptr_rgba_ui32(source, r0s, c0s);
             }
             break;
         
           case GAN_FLOAT32:
           {
              Gan_RGBAPixel_f32 f32pix;
            
              gan_err_test_ptr ( copy_pixels, "extract_rgba",
                                 GAN_ERROR_ILLEGAL_ARGUMENT, "" );
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    /* clip RGBA pixel values to gan_uint8 limits */
                    f32pix = gan_image_get_pix_rgba_f32 ( source, r0s+r, c0s+c );
                    ui32pix.R = gan_pixel_f32_to_ui32(f32pix.R);
                    ui32pix.G = gan_pixel_f32_to_ui32(f32pix.G);
                    ui32pix.B = gan_pixel_f32_to_ui32(f32pix.B);
                    ui32pix.A = gan_pixel_f32_to_ui32(f32pix.A);
                    gan_image_set_pix_rgba_ui32 ( dest, r0d+r, c0d+c, &ui32pix );
                 }
           }
           break;
         
           case GAN_FLOAT64:
           {
              Gan_RGBAPixel_f64 f64pix;
            
              gan_err_test_ptr ( copy_pixels, "extract_rgba",
                                 GAN_ERROR_ILLEGAL_ARGUMENT, "" );
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    /* clip RGBA pixel values to gan_uint8 limits */
                    f64pix = gan_image_get_pix_rgba_f64 ( source, r0s+r, c0s+c );
                    ui32pix.R = gan_pixel_f64_to_ui32(f64pix.R);
                    ui32pix.G = gan_pixel_f64_to_ui32(f64pix.G);
                    ui32pix.B = gan_pixel_f64_to_ui32(f64pix.B);
                    ui32pix.A = gan_pixel_f64_to_ui32(f64pix.A);
                    gan_image_set_pix_rgba_ui32 ( dest, r0d+r, c0d+c, &ui32pix );
                 }
           }
           break;
         
           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_rgba", GAN_ERROR_NOT_IMPLEMENTED, "");
             return NULL;
        }
        break;
      
      case GAN_FLOAT32:
        switch ( source->type )
        {
           Gan_RGBAPixel_f32 f32pix;

           case GAN_UINT8:
           {
              Gan_RGBAPixel_ui8 ui8pix;
               
              gan_err_test_ptr ( copy_pixels, "extract_rgba", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    ui8pix = gan_image_get_pix_rgba_ui8 ( source, r0s+r, c0s+c );
                    f32pix.R = gan_pixel_ui8_to_f32(ui8pix.R);
                    f32pix.G = gan_pixel_ui8_to_f32(ui8pix.G);
                    f32pix.B = gan_pixel_ui8_to_f32(ui8pix.B);
                    f32pix.A = gan_pixel_ui8_to_f32(ui8pix.A);
                    gan_image_set_pix_rgba_f32 ( dest, r0d+r, c0d+c, &f32pix );
                 }
           }
           break;
            
           case GAN_UINT16:
           {
              Gan_RGBAPixel_ui16 ui16pix;
               
              gan_err_test_ptr ( copy_pixels, "extract_rgba", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    ui16pix = gan_image_get_pix_rgba_ui16 ( source, r0s+r, c0s+c );
                    f32pix.R = gan_pixel_ui16_to_f32(ui16pix.R);
                    f32pix.G = gan_pixel_ui16_to_f32(ui16pix.G);
                    f32pix.B = gan_pixel_ui16_to_f32(ui16pix.B);
                    f32pix.A = gan_pixel_ui16_to_f32(ui16pix.A);
                    gan_image_set_pix_rgba_f32 ( dest, r0d+r, c0d+c, &f32pix );
                 }
           }
           break;
            
           case GAN_UINT32:
           {
              Gan_RGBAPixel_ui32 ui32pix;
               
              gan_err_test_ptr ( copy_pixels, "extract_rgba", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    ui32pix = gan_image_get_pix_rgba_ui32 ( source, r0s+r, c0s+c );
                    f32pix.R = gan_pixel_ui32_to_f32(ui32pix.R);
                    f32pix.G = gan_pixel_ui32_to_f32(ui32pix.G);
                    f32pix.B = gan_pixel_ui32_to_f32(ui32pix.B);
                    f32pix.A = gan_pixel_ui32_to_f32(ui32pix.A);
                    gan_image_set_pix_rgba_f32 ( dest, r0d+r, c0d+c, &f32pix );
                 }
           }
           break;
            
           case GAN_FLOAT32:
             if ( copy_pixels )
                for ( r=0; r < height; r++ )
                   gan_copy_array_f32 ( (gan_float32 *)&source->row_data.rgba.f32[r0s+r][c0s], 1, 4*width, (gan_float32 *)
                                      &dest->row_data.rgba.f32[r0d+r][c0d], 1);
             else
             {
                /* make destination image point into source image */
                gan_err_test_ptr ( r0d == 0 && c0d == 0, "extract_rgba", GAN_ERROR_INCOMPATIBLE, "" );
                for ( r=0; r < height; r++ )
                   dest->row_data.rgba.f32[r] = &source->row_data.rgba.f32[r0s+r][c0s];

                dest->pix_data_ptr = (unsigned char *)gan_image_get_pixptr_rgba_f32(source, r0s, c0s);
             }
             break;
            
           case GAN_FLOAT64:
           {
              Gan_RGBAPixel_f64 f64pix;
               
              gan_err_test_ptr ( copy_pixels, "extract_rgba", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    f64pix = gan_image_get_pix_rgba_f64 ( source, r0s+r, c0s+c );
                    f32pix.R = (gan_float32)f64pix.R;
                    f32pix.G = (gan_float32)f64pix.G;
                    f32pix.B = (gan_float32)f64pix.B;
                    f32pix.A = (gan_float32)f64pix.A;
                    gan_image_set_pix_rgba_f32 ( dest, r0d+r, c0d+c, &f32pix );
                 }
           }
           break;
            
           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_rgba", GAN_ERROR_NOT_IMPLEMENTED, "");
             return NULL;
        }
        break;
         
      case GAN_FLOAT64:
        switch ( source->type )
        {
           Gan_RGBAPixel_f64 f64pix;

           case GAN_UINT8:
           {
              Gan_RGBAPixel_ui8 ui8pix;
               
              gan_err_test_ptr ( copy_pixels, "extract_rgba", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    ui8pix = gan_image_get_pix_rgba_ui8 ( source, r0s+r, c0s+c );
                    f64pix.R = gan_pixel_ui8_to_f64(ui8pix.R);
                    f64pix.G = gan_pixel_ui8_to_f64(ui8pix.G);
                    f64pix.B = gan_pixel_ui8_to_f64(ui8pix.B);
                    f64pix.A = gan_pixel_ui8_to_f64(ui8pix.A);
                    gan_image_set_pix_rgba_f64 ( dest, r0d+r, c0d+c, &f64pix );
                 }
           }
           break;
            
           case GAN_UINT16:
           {
              Gan_RGBAPixel_ui16 ui16pix;
               
              gan_err_test_ptr ( copy_pixels, "extract_rgba", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    ui16pix = gan_image_get_pix_rgba_ui16 ( source, r0s+r, c0s+c );
                    f64pix.R = gan_pixel_ui16_to_f64(ui16pix.R);
                    f64pix.G = gan_pixel_ui16_to_f64(ui16pix.G);
                    f64pix.B = gan_pixel_ui16_to_f64(ui16pix.B);
                    f64pix.A = gan_pixel_ui16_to_f64(ui16pix.A);
                    gan_image_set_pix_rgba_f64 ( dest, r0d+r, c0d+c, &f64pix );
                 }
           }
           break;
            
           case GAN_UINT32:
           {
              Gan_RGBAPixel_ui32 ui32pix;
               
              gan_err_test_ptr ( copy_pixels, "extract_rgba", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    ui32pix = gan_image_get_pix_rgba_ui32 ( source, r0s+r, c0s+c );
                    f64pix.R = gan_pixel_ui32_to_f64(ui32pix.R);
                    f64pix.G = gan_pixel_ui32_to_f64(ui32pix.G);
                    f64pix.B = gan_pixel_ui32_to_f64(ui32pix.B);
                    f64pix.A = gan_pixel_ui32_to_f64(ui32pix.A);
                    gan_image_set_pix_rgba_f64 ( dest, r0d+r, c0d+c, &f64pix );
                 }
           }
           break;
            
           case GAN_FLOAT32:
           {
              Gan_RGBAPixel_f32 f32pix;
               
              gan_err_test_ptr ( copy_pixels, "extract_rgba", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    f32pix = gan_image_get_pix_rgba_f32 ( source, r0s+r, c0s+c );
                    f64pix.R = (gan_float64)f32pix.R;
                    f64pix.G = (gan_float64)f32pix.G;
                    f64pix.B = (gan_float64)f32pix.B;
                    f64pix.A = (gan_float64)f32pix.A;
                    gan_image_set_pix_rgba_f64 ( dest, r0d+r, c0d+c, &f64pix );
                 }
           }
           break;
            
           case GAN_FLOAT64:
             if ( copy_pixels )
                for ( r=0; r < height; r++ )
                   gan_copy_array_f64 ( (gan_float64 *)&source->row_data.rgba.f64[r0s+r][c0s], 1, 4*width, (gan_float64 *)
                                        &dest->row_data.rgba.f64[r0d+r][c0d], 1);
             else
             {
                /* make destination image point into source image */
                gan_err_test_ptr ( r0d == 0 && c0d == 0, "extract_rgba", GAN_ERROR_INCOMPATIBLE, "" );
                for ( r=0; r < height; r++ )
                   dest->row_data.rgba.f64[r] = gan_image_get_pixptr_rgba_f64(source, r0s+r, c0s);

                dest->pix_data_ptr = (unsigned char *)gan_image_get_pixptr_rgba_f64(source, r0s, c0s);
             }
             break;
            
           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_rgba", GAN_ERROR_NOT_IMPLEMENTED, "");
             return NULL;
        }
        break;
         
      default:
        gan_err_flush_trace();
        gan_err_register ( "extract_rgba", GAN_ERROR_NOT_IMPLEMENTED, "" );
        return NULL;
   }
   
   return dest;
}

/*
 * Converts the RGB image rgb to an RGB image with alpha channel rgba.
 * The alpha channel is set to one.
 *
 * The RGBA image is returned.
 */
static Gan_Image *
 extract_rgb_to_rgba ( const Gan_Image *rgb, unsigned r0s, unsigned c0s,
                       unsigned height, unsigned width,
                       Gan_Image *rgba, unsigned r0d, unsigned c0d )
{
   unsigned int r, c;
   
   gan_err_test_ptr ( rgb->format == GAN_RGB_COLOUR_IMAGE && rgba->format == GAN_RGB_COLOUR_ALPHA_IMAGE,
                      "extract_rgb_to_rgba", GAN_ERROR_INCOMPATIBLE, "" );
   switch ( rgb->type )
   {
      case GAN_UINT8:
        switch ( rgba->type )
        {
           Gan_RGBPixel_ui8 pix;

           case GAN_UINT8:
           {
              Gan_RGBAPixel_ui8 pixa;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pix = gan_image_get_pix_rgb_ui8(rgb,r0s+r,c0s+c);
                    pixa.R = pix.R;
                    pixa.G = pix.G;
                    pixa.B = pix.B;
                    pixa.A = GAN_UINT8_MAX;
                    gan_image_set_pix_rgba_ui8(rgba,r0d+r,c0d+c,&pixa);
                 }
           }
           break;

           case GAN_UINT16:
           {
              Gan_RGBAPixel_ui16 pixa;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pix = gan_image_get_pix_rgb_ui8(rgb,r0s+r,c0s+c);
                    pixa.R = gan_pixel_ui8_to_ui16(pix.R);
                    pixa.G = gan_pixel_ui8_to_ui16(pix.G);
                    pixa.B = gan_pixel_ui8_to_ui16(pix.B);
                    pixa.A = GAN_UINT16_MAX;
                    gan_image_set_pix_rgba_ui16(rgba,r0d+r,c0d+c,&pixa);
                 }
           }
           break;

           case GAN_UINT12:
           {
              Gan_RGBAPixel_ui12 pixa;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pix = gan_image_get_pix_rgb_ui8(rgb,r0s+r,c0s+c);
                    pixa.R = pix.R << 4;
                    pixa.G = pix.G << 4;
                    pixa.B = pix.B << 4;
                    pixa.A = 0xfff;
                    gan_image_set_pix_rgba_ui12(rgba,r0d+r,c0d+c,&pixa);
                 }
           }
           break;

           case GAN_UINT32:
           {
              Gan_RGBAPixel_ui32 pixa;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pix = gan_image_get_pix_rgb_ui8(rgb,r0s+r,c0s+c);
                    pixa.R = gan_pixel_ui8_to_ui32(pix.R);
                    pixa.G = gan_pixel_ui8_to_ui32(pix.G);
                    pixa.B = gan_pixel_ui8_to_ui32(pix.B);
                    pixa.A = GAN_UINT32_MAX;
                    gan_image_set_pix_rgba_ui32(rgba,r0d+r,c0d+c,&pixa);
                 }
           }
           break;

           case GAN_FLOAT32:
           {
              Gan_RGBAPixel_f32 pixa;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pix = gan_image_get_pix_rgb_ui8(rgb,r0s+r,c0s+c);
                    pixa.R = gan_pixel_ui8_to_f32(pix.R);
                    pixa.G = gan_pixel_ui8_to_f32(pix.G);
                    pixa.B = gan_pixel_ui8_to_f32(pix.B);
                    pixa.A = 1.0F;
                    gan_image_set_pix_rgba_f32(rgba,r0d+r,c0d+c,&pixa);
                 }
           }
           break;

           case GAN_FLOAT64:
           {
              Gan_RGBAPixel_f64 pixa;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pix = gan_image_get_pix_rgb_ui8(rgb,r0s+r,c0s+c);
                    pixa.R = gan_pixel_ui8_to_f64(pix.R);
                    pixa.G = gan_pixel_ui8_to_f64(pix.G);
                    pixa.B = gan_pixel_ui8_to_f64(pix.B);
                    pixa.A = 1.0;
                    gan_image_set_pix_rgba_f64(rgba,r0d+r,c0d+c,&pixa);
                 }
           }
           break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_rgb_to_rgba", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             return NULL;
        }
        break;
        
      case GAN_UINT16:
        switch ( rgba->type )
        {
           Gan_RGBPixel_ui16 pix;

           case GAN_UINT8:
           {
              Gan_RGBAPixel_ui8 pixa;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pix = gan_image_get_pix_rgb_ui16(rgb,r0s+r,c0s+c);
                    pixa.R = gan_pixel_ui16_to_ui8(pix.R);
                    pixa.G = gan_pixel_ui16_to_ui8(pix.G);
                    pixa.B = gan_pixel_ui16_to_ui8(pix.B);
                    pixa.A = GAN_UINT8_MAX;
                    gan_image_set_pix_rgba_ui8(rgba,r0d+r,c0d+c,&pixa);
                 }
           }
           break;

           case GAN_UINT12:
           {
              Gan_RGBAPixel_ui12 pixa;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pix = gan_image_get_pix_rgb_ui16(rgb,r0s+r,c0s+c);
                    pixa.R = pix.R >> 4;
                    pixa.G = pix.G >> 4;
                    pixa.B = pix.B >> 4;
                    pixa.A = 0xfff;
                    gan_image_set_pix_rgba_ui12(rgba,r0d+r,c0d+c,&pixa);
                 }
           }
           break;

           case GAN_UINT16:
           {
              Gan_RGBAPixel_ui16 pixa;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pix = gan_image_get_pix_rgb_ui16(rgb,r0s+r,c0s+c);
                    pixa.R = pix.R;
                    pixa.G = pix.G;
                    pixa.B = pix.B;
                    pixa.A = GAN_UINT16_MAX;
                    gan_image_set_pix_rgba_ui16(rgba,r0d+r,c0d+c,&pixa);
                 }
           }
           break;

           case GAN_UINT32:
           {
              Gan_RGBAPixel_ui32 pixa;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pix = gan_image_get_pix_rgb_ui16(rgb,r0s+r,c0s+c);
                    pixa.R = gan_pixel_ui16_to_ui32(pix.R);
                    pixa.G = gan_pixel_ui16_to_ui32(pix.G);
                    pixa.B = gan_pixel_ui16_to_ui32(pix.B);
                    pixa.A = GAN_UINT32_MAX;
                    gan_image_set_pix_rgba_ui32(rgba,r0d+r,c0d+c,&pixa);
                 }
           }
           break;

           case GAN_FLOAT32:
           {
              Gan_RGBAPixel_f32 pixa;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pix = gan_image_get_pix_rgb_ui16(rgb,r0s+r,c0s+c);
                    pixa.R = gan_pixel_ui16_to_f32(pix.R);
                    pixa.G = gan_pixel_ui16_to_f32(pix.G);
                    pixa.B = gan_pixel_ui16_to_f32(pix.B);
                    pixa.A = 1.0F;
                    gan_image_set_pix_rgba_f32(rgba,r0d+r,c0d+c,&pixa);
                 }
           }
           break;

           case GAN_FLOAT64:
           {
              Gan_RGBAPixel_f64 pixa;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pix = gan_image_get_pix_rgb_ui16(rgb,r0s+r,c0s+c);
                    pixa.R = gan_pixel_ui16_to_f64(pix.R);
                    pixa.G = gan_pixel_ui16_to_f64(pix.G);
                    pixa.B = gan_pixel_ui16_to_f64(pix.B);
                    pixa.A = 1.0;
                    gan_image_set_pix_rgba_f64(rgba,r0d+r,c0d+c,&pixa);
                 }
           }
           break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_rgb_to_rgba", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             return NULL;
        }
        break;

      case GAN_UINT32:
        switch ( rgba->type )
        {
           Gan_RGBPixel_ui32 pix;

           case GAN_UINT8:
           {
              Gan_RGBAPixel_ui8 pixa;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pix = gan_image_get_pix_rgb_ui32(rgb,r0s+r,c0s+c);
                    pixa.R = gan_pixel_ui32_to_ui8(pix.R);
                    pixa.G = gan_pixel_ui32_to_ui8(pix.G);
                    pixa.B = gan_pixel_ui32_to_ui8(pix.B);
                    pixa.A = GAN_UINT8_MAX;
                    gan_image_set_pix_rgba_ui8(rgba,r0d+r,c0d+c,&pixa);
                 }
           }
           break;

           case GAN_UINT16:
           {
              Gan_RGBAPixel_ui16 pixa;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pix = gan_image_get_pix_rgb_ui32(rgb,r0s+r,c0s+c);
                    pixa.R = gan_pixel_ui32_to_ui16(pix.R);
                    pixa.G = gan_pixel_ui32_to_ui16(pix.G);
                    pixa.B = gan_pixel_ui32_to_ui16(pix.B);
                    pixa.A = GAN_UINT16_MAX;
                    gan_image_set_pix_rgba_ui16(rgba,r0d+r,c0d+c,&pixa);
                 }
           }
           break;

           case GAN_UINT32:
           {
              Gan_RGBAPixel_ui32 pixa;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pix = gan_image_get_pix_rgb_ui32(rgb,r0s+r,c0s+c);
                    pixa.R = pix.R;
                    pixa.G = pix.G;
                    pixa.B = pix.B;
                    pixa.A = GAN_UINT32_MAX;
                    gan_image_set_pix_rgba_ui32(rgba,r0d+r,c0d+c,&pixa);
                 }
           }
           break;

           case GAN_FLOAT32:
           {
              Gan_RGBAPixel_f32 pixa;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pix = gan_image_get_pix_rgb_ui32(rgb,r0s+r,c0s+c);
                    pixa.R = gan_pixel_ui32_to_f32(pix.R);
                    pixa.G = gan_pixel_ui32_to_f32(pix.G);
                    pixa.B = gan_pixel_ui32_to_f32(pix.B);
                    pixa.A = 1.0F;
                    gan_image_set_pix_rgba_f32(rgba,r0d+r,c0d+c,&pixa);
                 }
           }
           break;

           case GAN_FLOAT64:
           {
              Gan_RGBAPixel_f64 pixa;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pix = gan_image_get_pix_rgb_ui32(rgb,r0s+r,c0s+c);
                    pixa.R = gan_pixel_ui32_to_f64(pix.R);
                    pixa.G = gan_pixel_ui32_to_f64(pix.G);
                    pixa.B = gan_pixel_ui32_to_f64(pix.B);
                    pixa.A = 1.0;
                    gan_image_set_pix_rgba_f64(rgba,r0d+r,c0d+c,&pixa);
                 }
           }
           break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_rgb_to_rgba", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             return NULL;
        }
        break;

      case GAN_FLOAT32:
        switch ( rgba->type )
        {
           Gan_RGBPixel_f32 pix;

           case GAN_UINT8:
           {
              Gan_RGBAPixel_ui8 pixa;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pix = gan_image_get_pix_rgb_f32(rgb,r0s+r,c0s+c);
                    pixa.R = gan_pixel_f32_to_ui8(pix.R);
                    pixa.G = gan_pixel_f32_to_ui8(pix.G);
                    pixa.B = gan_pixel_f32_to_ui8(pix.B);
                    pixa.A = GAN_UINT8_MAX;
                    gan_image_set_pix_rgba_ui8(rgba,r0d+r,c0d+c,&pixa);
                 }
           }
           break;

           case GAN_UINT16:
           {
              Gan_RGBAPixel_ui16 pixa;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pix = gan_image_get_pix_rgb_f32(rgb,r0s+r,c0s+c);
                    pixa.R = gan_pixel_f32_to_ui16(pix.R);
                    pixa.G = gan_pixel_f32_to_ui16(pix.G);
                    pixa.B = gan_pixel_f32_to_ui16(pix.B);
                    pixa.A = GAN_UINT16_MAX;
                    gan_image_set_pix_rgba_ui16(rgba,r0d+r,c0d+c,&pixa);
                 }
           }
           break;

           case GAN_UINT32:
           {
              Gan_RGBAPixel_ui32 pixa;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pix = gan_image_get_pix_rgb_f32(rgb,r0s+r,c0s+c);
                    pixa.R = gan_pixel_f32_to_ui32(pix.R);
                    pixa.G = gan_pixel_f32_to_ui32(pix.G);
                    pixa.B = gan_pixel_f32_to_ui32(pix.B);
                    pixa.A = GAN_UINT32_MAX;
                    gan_image_set_pix_rgba_ui32(rgba,r0d+r,c0d+c,&pixa);
                 }
           }
           break;

           case GAN_FLOAT32:
           {
              Gan_RGBAPixel_f32 pixa;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pix = gan_image_get_pix_rgb_f32(rgb,r0s+r,c0s+c);
                    pixa.R = pix.R;
                    pixa.G = pix.G;
                    pixa.B = pix.B;
                    pixa.A = 1.0F;
                    gan_image_set_pix_rgba_f32(rgba,r0d+r,c0d+c,&pixa);
                 }
           }
           break;

           case GAN_FLOAT64:
           {
              Gan_RGBAPixel_f64 pixa;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pix = gan_image_get_pix_rgb_f32(rgb,r0s+r,c0s+c);
                    pixa.R = (gan_float64)pix.R;
                    pixa.G = (gan_float64)pix.G;
                    pixa.B = (gan_float64)pix.B;
                    pixa.A = 1.0;
                    gan_image_set_pix_rgba_f64(rgba,r0d+r,c0d+c,&pixa);
                 }
           }
           break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_rgb_to_rgba", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             return NULL;
        }
        break;

      case GAN_FLOAT64:
        switch ( rgba->type )
        {
           Gan_RGBPixel_f64 pix;

           case GAN_UINT8:
           {
              Gan_RGBAPixel_ui8 pixa;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pix = gan_image_get_pix_rgb_f64(rgb,r0s+r,c0s+c);
                    pixa.R = gan_pixel_f64_to_ui8(pix.R);
                    pixa.G = gan_pixel_f64_to_ui8(pix.G);
                    pixa.B = gan_pixel_f64_to_ui8(pix.B);
                    pixa.A = GAN_UINT8_MAX;
                    gan_image_set_pix_rgba_ui8(rgba,r0d+r,c0d+c,&pixa);
                 }
           }
           break;

           case GAN_UINT16:
           {
              Gan_RGBAPixel_ui16 pixa;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pix = gan_image_get_pix_rgb_f64(rgb,r0s+r,c0s+c);
                    pixa.R = gan_pixel_f64_to_ui16(pix.R);
                    pixa.G = gan_pixel_f64_to_ui16(pix.G);
                    pixa.B = gan_pixel_f64_to_ui16(pix.B);
                    pixa.A = GAN_UINT16_MAX;
                    gan_image_set_pix_rgba_ui16(rgba,r0d+r,c0d+c,&pixa);
                 }
           }
           break;

           case GAN_INT32:
           {
              Gan_RGBAPixel_i32 pixa;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pix = gan_image_get_pix_rgb_f64(rgb,r0s+r,c0s+c);
                    pixa.R = gan_pixel_f64_to_i32(pix.R);
                    pixa.G = gan_pixel_f64_to_i32(pix.G);
                    pixa.B = gan_pixel_f64_to_i32(pix.B);
                    pixa.A = INT_MAX;
                    gan_image_set_pix_rgba_i32(rgba,r0d+r,c0d+c,&pixa);
                 }
           }
           break;

           case GAN_UINT32:
           {
              Gan_RGBAPixel_ui32 pixa;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pix = gan_image_get_pix_rgb_f64(rgb,r0s+r,c0s+c);
                    pixa.R = gan_pixel_f64_to_ui32(pix.R);
                    pixa.G = gan_pixel_f64_to_ui32(pix.G);
                    pixa.B = gan_pixel_f64_to_ui32(pix.B);
                    pixa.A = GAN_UINT32_MAX;
                    gan_image_set_pix_rgba_ui32(rgba,r0d+r,c0d+c,&pixa);
                 }
           }
           break;

           case GAN_FLOAT32:
           {
              Gan_RGBAPixel_f32 pixa;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pix = gan_image_get_pix_rgb_f64(rgb,r0s+r,c0s+c);
                    pixa.R = (gan_float32)pix.R;
                    pixa.G = (gan_float32)pix.G;
                    pixa.B = (gan_float32)pix.B;
                    pixa.A = 1.0F;
                    gan_image_set_pix_rgba_f32(rgba,r0d+r,c0d+c,&pixa);
                 }
           }
           break;

           case GAN_FLOAT64:
           {
              Gan_RGBAPixel_f64 pixa;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pix = gan_image_get_pix_rgb_f64(rgb,r0s+r,c0s+c);
                    pixa.R = pix.R;
                    pixa.G = pix.G;
                    pixa.B = pix.B;
                    pixa.A = 1.0;
                    gan_image_set_pix_rgba_f64(rgba,r0d+r,c0d+c,&pixa);
                 }
           }
           break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_rgb_to_rgba", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" ); return NULL;
        }
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "extract_rgb_to_rgba", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
        return NULL;
   }

   /* success */
   return rgba;
}

#undef ui8
#undef ui10

/* Converts a padded RGB image to a different type
 *
 * The converted RGB image is returned.
 */
static Gan_Image *
 extract_rgbx ( const Gan_Image *source, unsigned r0s, unsigned c0s,
                unsigned height, unsigned width, Gan_Bool copy_pixels,
                Gan_Image *dest, unsigned r0d, unsigned c0d )
{
   unsigned int r, c;
   
   gan_err_test_ptr ( source->format == GAN_RGBX && dest->format == GAN_RGBX, "extract_rgbx", GAN_ERROR_INCOMPATIBLE, "" );
   
   switch ( dest->type )
   {
      case GAN_UINT8:
        switch ( source->type )
        {
           Gan_RGBXPixel_ui8 ui8pix;

           case GAN_UINT8:
             if ( copy_pixels )
                for ( r=0; r < height; r++ )
                   gan_copy_array_ui8 ((gan_uint8 *)&source->row_data.rgbx.ui8[r0s+r][c0s], 1, 4*width,
                                       (gan_uint8 *)&dest->row_data.rgbx.ui8[r0d+r][c0d],1);
             else
             {
                /* make destination image point into source image */
                gan_err_test_ptr ( r0d == 0 && c0d == 0, "extract_rgbx", GAN_ERROR_INCOMPATIBLE, "" );
                for ( r=0; r < height; r++ )
                   dest->row_data.rgbx.ui8[r] = gan_image_get_pixptr_rgbx_ui8(source, r0s+r, c0s);

                dest->pix_data_ptr = (unsigned char *)gan_image_get_pixptr_rgbx_ui8(source, r0s, c0s);
             }
             break;
         
           case GAN_UINT10:
           {
              Gan_RGBXPixel_ui10 ui10pix;
            
              gan_err_test_ptr ( copy_pixels, "extract_rgbx", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    /* clip RGBX pixel values to gan_uint8 limits */
                    ui10pix = gan_image_get_pix_rgbx_ui10 ( source, r0s+r, c0s+c );
                    ui8pix.R = ui10pix.R >> 2;
                    ui8pix.G = ui10pix.G >> 2;
                    ui8pix.B = ui10pix.B >> 2;
                    ui8pix.X = 0;
                    gan_image_set_pix_rgbx_ui8 ( dest, r0d+r, c0d+c, &ui8pix );
                 }
           }
           break;
         
           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_rgbx", GAN_ERROR_NOT_IMPLEMENTED, "" );
             return NULL;
        }
        break;
      
      case GAN_UINT10:
        switch ( source->type )
        {
           Gan_RGBXPixel_ui10 ui10pix;

           case GAN_UINT8:
           {
              Gan_RGBXPixel_ui8 ui8pix;
            
              gan_err_test_ptr ( copy_pixels, "extract_rgbx", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    /* clip RGBX pixel values to gan_uint8 limits */
                    ui8pix = gan_image_get_pix_rgbx_ui8 ( source, r0s+r, c0s+c );
                    ui10pix.R = ui8pix.R << 2;
                    ui10pix.G = ui8pix.G << 2;
                    ui10pix.B = ui8pix.B << 2;
                    ui10pix.X = 0;
                    gan_image_set_pix_rgbx_ui10 ( dest, r0d+r, c0d+c, &ui10pix );
                 }
           }
           break;
         
           case GAN_UINT10:
             if ( copy_pixels )
                for ( r=0; r < height; r++ )
                   gan_copy_array_ui32 ((gan_uint32 *)&source->row_data.rgbx.ui10[r0s+r][c0s], 1, width,
                                        (gan_uint32 *)&dest->row_data.rgbx.ui10[r0d+r][c0d], 1);
             else
             {
                /* make destination image point into source image */
                gan_err_test_ptr ( r0d == 0 && c0d == 0, "extract_rgbx", GAN_ERROR_INCOMPATIBLE, "" );
                for ( r=0; r < height; r++ )
                   dest->row_data.rgbx.ui10[r] = &source->row_data.rgbx.ui10[r0s+r][c0s];

                dest->pix_data_ptr = (unsigned char *)gan_image_get_pixptr_rgbx_ui10(source, r0s, c0s);
             }
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_rgbx", GAN_ERROR_NOT_IMPLEMENTED, "");
             return NULL;
        }
        break;
      
      default:
        gan_err_flush_trace();
        gan_err_register ( "extract_rgbx", GAN_ERROR_NOT_IMPLEMENTED, "" );
        return NULL;
   }
   
   return dest;
}

/* Converts a padded RGB image to a different type
 *
 * The converted RGB image is returned.
 */
static Gan_Image *
 extract_rgbas ( const Gan_Image *source, unsigned r0s, unsigned c0s,
                unsigned height, unsigned width, Gan_Bool copy_pixels,
                Gan_Image *dest, unsigned r0d, unsigned c0d )
{
   unsigned int r;
   
   gan_err_test_ptr ( source->format == GAN_RGBAS && dest->format == GAN_RGBAS, "extract_rgbas", GAN_ERROR_INCOMPATIBLE, "" );
   
   switch ( dest->type )
   {
      case GAN_UINT10:
        switch ( source->type )
        {
           case GAN_UINT10:
             if ( copy_pixels )
                for ( r=0; r < height; r++ )
                   gan_copy_array_ui32 ((gan_uint32 *)&source->row_data.rgbas.ui10[r0s+r][c0s], 1, width,
                                        (gan_uint32 *)&dest->row_data.rgbas.ui10[r0d+r][c0d], 1);
             else
             {
                /* make destination image point into source image */
                gan_err_test_ptr ( r0d == 0 && c0d == 0, "extract_rgbas", GAN_ERROR_INCOMPATIBLE, "" );
                for ( r=0; r < height; r++ )
                   dest->row_data.rgbas.ui10[r] = &source->row_data.rgbas.ui10[r0s+r][c0s];

                dest->pix_data_ptr = (unsigned char *)gan_image_get_pixptr_rgbas_ui10(source, r0s, c0s);
             }
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_rgbas", GAN_ERROR_NOT_IMPLEMENTED, "");
             return NULL;
        }
        break;
      
      default:
        gan_err_flush_trace();
        gan_err_register ( "extract_rgbas", GAN_ERROR_NOT_IMPLEMENTED, "" );
        return NULL;
   }
   
   return dest;
}

/* Converts a 2D vector field image to a different type
 *
 * The converted image is returned.
 */
static Gan_Image *
 extract_vfield2D ( const Gan_Image *source, unsigned r0s, unsigned c0s,
                    unsigned height, unsigned width, Gan_Bool copy_pixels,
                    Gan_Image *dest, unsigned r0d, unsigned c0d )
{
   unsigned int r;
   
   gan_err_test_ptr ( source->format == GAN_VECTOR_FIELD_2D && dest->format == GAN_VECTOR_FIELD_2D, "extract_vfield2D", GAN_ERROR_INCOMPATIBLE, "" );
   
   switch ( dest->type )
   {
      case GAN_FLOAT32:
        switch ( source->type )
        {
           case GAN_FLOAT32:
             if ( copy_pixels )
                for ( r=0; r < height; r++ )
                   gan_copy_array_f32 ((gan_float32 *)&source->row_data.vfield2D.f32[r0s+r][c0s], 1, width,
                                       (gan_float32 *)&dest->row_data.vfield2D.f32[r0d+r][c0d],1);
             else
             {
                /* make destination image point into source image */
                gan_err_test_ptr ( r0d == 0 && c0d == 0, "extract_vfield2D", GAN_ERROR_INCOMPATIBLE, "" );
                for ( r=0; r < height; r++ )
                   dest->row_data.vfield2D.f32[r] = gan_image_get_pixptr_vfield2D_f32(source, r0s+r, c0s);

                dest->pix_data_ptr = (unsigned char *)gan_image_get_pixptr_vfield2D_f32(source, r0s, c0s);
             }
             break;
         
           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_vfield2D", GAN_ERROR_NOT_IMPLEMENTED, "" );
             return NULL;
        }
        break;
      
      default:
        gan_err_flush_trace();
        gan_err_register ( "extract_vfield2D", GAN_ERROR_NOT_IMPLEMENTED, "" );
        return NULL;
   }
   
   return dest;
}

/* Converts a YUV-444 padded image to a different type
 *
 * The converted image is returned.
 */
static Gan_Image *
 extract_yuvx444 ( const Gan_Image *source, unsigned r0s, unsigned c0s,
                   unsigned height, unsigned width, Gan_Bool copy_pixels,
                   Gan_Image *dest, unsigned r0d, unsigned c0d )
{
   unsigned int r;
   
   gan_err_test_ptr ( source->format == GAN_YUVX444 && dest->format == GAN_YUVX444, "extract_yuvx444", GAN_ERROR_INCOMPATIBLE, "" );
   
   switch ( dest->type )
   {
      case GAN_UINT8:
        switch ( source->type )
        {
           case GAN_UINT8:
             if ( copy_pixels )
                for ( r=0; r < height; r++ )
                   gan_copy_array_ui32 ((gan_uint32 *)&source->row_data.yuvx444.ui8[r0s+r][c0s], 1, width,
                                        (gan_uint32 *)&dest->row_data.yuvx444.ui8[r0d+r][c0d],1);
             else
             {
                /* make destination image point into source image */
                gan_err_test_ptr ( r0d == 0 && c0d == 0, "extract_yuvx444", GAN_ERROR_INCOMPATIBLE, "" );
                for ( r=0; r < height; r++ )
                   dest->row_data.yuvx444.ui8[r] = gan_image_get_pixptr_yuvx444_ui8(source, r0s+r, c0s);

                dest->pix_data_ptr = (unsigned char *)gan_image_get_pixptr_yuvx444_ui8(source, r0s, c0s);
             }
             break;
         
           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_yuvx444", GAN_ERROR_NOT_IMPLEMENTED, "" );
             return NULL;
        }
        break;
      
      default:
        gan_err_flush_trace();
        gan_err_register ( "extract_yuvx444", GAN_ERROR_NOT_IMPLEMENTED, "" );
        return NULL;
   }
   
   return dest;
}

/* Converts a YUV-444 padded image to a different type
 *
 * The converted image is returned.
 */
static Gan_Image *
 extract_yuva444 ( const Gan_Image *source, unsigned r0s, unsigned c0s,
                   unsigned height, unsigned width, Gan_Bool copy_pixels,
                   Gan_Image *dest, unsigned r0d, unsigned c0d )
{
   unsigned int r;
   
   gan_err_test_ptr ( source->format == GAN_YUVA444 && dest->format == GAN_YUVA444, "extract_yuva444", GAN_ERROR_INCOMPATIBLE, "" );
   
   switch ( dest->type )
   {
      case GAN_UINT8:
        switch ( source->type )
        {
           case GAN_UINT8:
             if ( copy_pixels )
                for ( r=0; r < height; r++ )
                   gan_copy_array_ui32 ((gan_uint32 *)&source->row_data.yuva444.ui8[r0s+r][c0s], 1, width,
                                        (gan_uint32 *)&dest->row_data.yuva444.ui8[r0d+r][c0d],1);
             else
             {
                /* make destination image point into source image */
                gan_err_test_ptr ( r0d == 0 && c0d == 0, "extract_yuva444", GAN_ERROR_INCOMPATIBLE, "" );
                for ( r=0; r < height; r++ )
                   dest->row_data.yuva444.ui8[r] = gan_image_get_pixptr_yuva444_ui8(source, r0s+r, c0s);

                dest->pix_data_ptr = (unsigned char *)gan_image_get_pixptr_yuva444_ui8(source, r0s, c0s);
             }
             break;
         
           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_yuva444", GAN_ERROR_NOT_IMPLEMENTED, "" );
             return NULL;
        }
        break;
      
      default:
        gan_err_flush_trace();
        gan_err_register ( "extract_yuva444", GAN_ERROR_NOT_IMPLEMENTED, "" );
        return NULL;
   }
   
   return dest;
}

/* Converts a YUV-422 image to a different type
 *
 * The converted image is returned.
 */
static Gan_Image *
 extract_yuv422 ( const Gan_Image *source, unsigned r0s, unsigned c0s,
                  unsigned height, unsigned width, Gan_Bool copy_pixels,
                  Gan_Image *dest, unsigned r0d, unsigned c0d )
{
   unsigned int r;
   
   gan_err_test_ptr ( source->format == GAN_YUV422 && dest->format == GAN_YUV422, "extract_yuv422", GAN_ERROR_INCOMPATIBLE, "" );

   /* because the data is spread across two elements, we should not split the elements */
   gan_err_test_ptr ( (c0s % 2) == 0 && (width % 2) == 0 && (c0d % 2) == 0, "extract_yuv422", GAN_ERROR_INCOMPATIBLE, "" );

   switch ( dest->type )
   {
      case GAN_UINT8:
        switch ( source->type )
        {
           case GAN_UINT8:
             if ( copy_pixels )
                for ( r=0; r < height; r++ )
                   gan_copy_array_ui32 ((gan_uint32 *)&source->row_data.yuv422.ui8[r0s+r][c0s], 1, width,
                                        (gan_uint32 *)&dest->row_data.yuv422.ui8[r0d+r][c0d],1);
             else
             {
                /* make destination image point into source image */
                gan_err_test_ptr ( r0d == 0 && c0d == 0, "extract_yuv422", GAN_ERROR_INCOMPATIBLE, "" );
                for ( r=0; r < height; r++ )
                   dest->row_data.yuv422.ui8[r] = gan_image_get_pixptr_yuv422_ui8(source, r0s+r, c0s);

                dest->pix_data_ptr = (unsigned char *)gan_image_get_pixptr_yuv422_ui8(source, r0s, c0s);
             }
             break;
         
           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_yuv422", GAN_ERROR_NOT_IMPLEMENTED, "" );
             return NULL;
        }
        break;
      
      default:
        gan_err_flush_trace();
        gan_err_register ( "extract_yuv422", GAN_ERROR_NOT_IMPLEMENTED, "" );
        return NULL;
   }
   
   return dest;
}

/*
 * Converts the vector field image vfield2D to an RGB image rgb.
 *
 * The RGB image is returned.
 */
static Gan_Image *
 extract_vfield2D_to_rgb ( const Gan_Image *vfield2D, unsigned r0s, unsigned c0s,
                           unsigned height, unsigned width,
                           Gan_Image *rgb, unsigned r0d, unsigned c0d )
{
   unsigned int r, c;
   
   gan_err_test_ptr ( vfield2D->format == GAN_VECTOR_FIELD_2D && rgb->format == GAN_RGB_COLOUR_IMAGE,
                      "extract_vfield2D_to_rgb", GAN_ERROR_INCOMPATIBLE, "" );
   switch ( vfield2D->type )
   {
      case GAN_FLOAT32:
        switch ( rgb->type )
        {
           case GAN_FLOAT32:
           {
              Gan_Vector2_f pixv;
              Gan_RGBPixel_f32 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pixv = gan_image_get_pix_vfield2D_f32(vfield2D,r0s+r,c0s+c);
                    pix.R = pixv.x;
                    pix.G = pixv.y;
                    pix.B = 0.0F;
                    gan_image_set_pix_rgb_f32(rgb,r0d+r,c0d+c,&pix);
                 }
           }
           break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_vfield2D_to_rgb", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             return NULL;
        }
        break;
        
      default:
        gan_err_flush_trace();
        gan_err_register ( "extract_vfield2D_to_rgb", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
        return NULL;
   }

   /* success */
   return rgb;
}

/*
 * Converts the padded RGB image rgbx to an RGB image rgb.
 * The RGB values are copied and the alpha channel is stripped out.
 *
 * The RGB image is returned.
 */
static Gan_Image *
 extract_rgbx_to_rgb ( const Gan_Image *rgbx, unsigned r0s, unsigned c0s,
                       unsigned height, unsigned width,
                       Gan_Image *rgb, unsigned r0d, unsigned c0d )
{
   unsigned int r, c;
   
   gan_err_test_ptr ( rgbx->format == GAN_RGBX && rgb->format == GAN_RGB_COLOUR_IMAGE,
                      "extract_rgbx_to_rgb", GAN_ERROR_INCOMPATIBLE, "" );
   switch ( rgbx->type )
   {
      case GAN_UINT8:
        switch ( rgb->type )
        {
           case GAN_UINT8:
           {
              Gan_RGBXPixel_ui8 pixx;
              Gan_RGBPixel_ui8 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pixx = gan_image_get_pix_rgbx_ui8(rgbx,r0s+r,c0s+c);
                    pix.R = pixx.R;
                    pix.G = pixx.G;
                    pix.B = pixx.B;
                    gan_image_set_pix_rgb_ui8(rgb,r0d+r,c0d+c,&pix);
                 }
           }
           break;

           case GAN_UINT16:
           {
              Gan_RGBXPixel_ui8 pixx;
              Gan_RGBPixel_ui16 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pixx = gan_image_get_pix_rgbx_ui8(rgbx,r0s+r,c0s+c);
                    pix.R = gan_pixel_ui8_to_ui16(pixx.R);
                    pix.G = gan_pixel_ui8_to_ui16(pixx.G);
                    pix.B = gan_pixel_ui8_to_ui16(pixx.B);
                    gan_image_set_pix_rgb_ui16(rgb,r0d+r,c0d+c,&pix);
                 }
           }
           break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_rgbx_to_rgb", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             return NULL;
        }
        break;
        
      case GAN_UINT10:
        switch ( rgb->type )
        {
           case GAN_UINT8:
           {
              Gan_RGBXPixel_ui10 pixx;
              Gan_RGBPixel_ui8 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pixx = gan_image_get_pix_rgbx_ui10(rgbx,r0s+r,c0s+c);
                    pix.R = pixx.R >> 2;
                    pix.G = pixx.G >> 2;
                    pix.B = pixx.B >> 2;
                    gan_image_set_pix_rgb_ui8(rgb,r0d+r,c0d+c,&pix);
                 }
           }
           break;

           case GAN_UINT16:
           {
              Gan_RGBXPixel_ui10 pixx;
              Gan_RGBPixel_ui16 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pixx = gan_image_get_pix_rgbx_ui10(rgbx,r0s+r,c0s+c);
                    pix.R = pixx.R << 6;
                    pix.G = pixx.G << 6;
                    pix.B = pixx.B << 6;
                    gan_image_set_pix_rgb_ui16(rgb,r0d+r,c0d+c,&pix);
                 }
           }
           break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_rgbx_to_rgb", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             return NULL;
        }
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "extract_rgbx_to_rgb", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
        return NULL;
   }

   /* success */
   return rgb;
}

/*
 * Converts the padded YUVX-444 image yuvx444 to an RGB image rgb.
 *
 * According to CCIR 601, specs:
 *   R = Y + (1.4075 * (V - 128));
 *   G = Y - (0.3455 * (U - 128) - (0.7169 * (V - 128));
 *   B = Y + (1.7790 * (U - 128);
 *
 * But we have to take into account the fact that the YUV values are clamped to 16-235(Y) and 16-240(U,V).
 *
 * So the scale coeffients are
 *   R = 1.1643*Y              + 1.59596*V  - 224.288;
 *   G = 1.1643*Y - 0.391712*U - 0.813013*V + 135.596
 *   B = 1.1643*Y + 2.01726*U                -278.403
 *
 * The RGB image is returned.
 */
static Gan_Image *
 extract_yuvx444_to_rgb ( const Gan_Image *yuvx444, unsigned r0s, unsigned c0s,
                          unsigned height, unsigned width,
                          Gan_Image *rgb, unsigned r0d, unsigned c0d )
{
   unsigned int r, c;
   
   gan_err_test_ptr ( yuvx444->format == GAN_YUVX444 && rgb->format == GAN_RGB_COLOUR_IMAGE,
                      "extract_yuvx444_to_rgb", GAN_ERROR_INCOMPATIBLE, "" );
   switch ( yuvx444->type )
   {
      case GAN_UINT8:
        switch ( rgb->type )
        {
           case GAN_UINT8:
           {
              Gan_YUVX444Pixel_ui8 pixyuv;
              int pixi;
              Gan_RGBPixel_ui8 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
#define CLAMP_0_TO_255(p) (((p) < 0) ? 0 : (((p) > 255) ? 255 : (p)))
                    pixyuv = gan_image_get_pix_yuvx444_ui8(yuvx444,r0s+r,c0s+c);
                    pixi = (116430*(int)pixyuv.Y                        + 159596*(int)pixyuv.V - 22428800)/100000;
                    pix.R = (gan_uint8) CLAMP_0_TO_255(pixi);
                    pixi = (116430*(int)pixyuv.Y -  39171*(int)pixyuv.U -  81301*(int)pixyuv.V + 13559600)/100000;
                    pix.G = (gan_uint8) CLAMP_0_TO_255(pixi);
                    pixi = (116430*(int)pixyuv.Y + 201726*(int)pixyuv.U                        - 27840300)/100000;
                    pix.B = (gan_uint8) CLAMP_0_TO_255(pixi);
                    gan_image_set_pix_rgb_ui8(rgb,r0d+r,c0d+c,&pix);
#undef CLAMP_0_TO_255
                 }
           }
           break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_yuvx444_to_rgb", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             return NULL;
        }
        break;
        
      default:
        gan_err_flush_trace();
        gan_err_register ( "extract_yuvx444_to_rgb", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
        return NULL;
   }

   /* success */
   return rgb;
}

/*
 * Converts the RGBA image rgba to a YUVA-444 image
 *
 * The YUVA444 image is returned.
 *
 * According to CCIR 601, specs:
 *   Y = R *  .299 + G *  .587 + B *  .114;
 *   U = R * -.169 + G * -.332 + B *  .500 + 128.;
 *   V = R *  .500 + G * -.419 + B * -.0813 + 128.;
 *
 * The fixed point version, clamped to 16-235 (Y) and (16-240) (U,V) is
 *
 *   Y = min(abs(r * 2104 + g * 4130 + b * 802 + 4096 + 131072) >> 13, 235)
 *   U = min(abs(r * -1214 + g * -2384 + b * 3598 + 4096 + 1048576) >> 13, 240)
 *   V = min(abs(r * 3598 + g * -3013 + b * -585 + 4096 + 1048576) >> 13, 240)
 *
 */
static Gan_Image *
 extract_rgba_to_yuva444 ( const Gan_Image *rgba, unsigned r0s, unsigned c0s,
                           unsigned height, unsigned width,
                           Gan_Image *yuva444, unsigned r0d, unsigned c0d )
{
   unsigned int r, c;
   
   gan_err_test_ptr ( rgba->format == GAN_RGB_COLOUR_ALPHA_IMAGE && yuva444->format == GAN_YUVA444,
                      "extract_rgba_to_yuva444", GAN_ERROR_INCOMPATIBLE, "" );
   switch ( rgba->type )
   {
      case GAN_UINT8:
        switch ( yuva444->type )
        {
           Gan_RGBAPixel_ui8 pix;

           case GAN_UINT8:
           {
              Gan_YUVA444Pixel_ui8 pixyuv;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pix = gan_image_get_pix_rgba_ui8(rgba,r0s+r,c0s+c);
                    pixyuv.Y = (gan_uint8) gan_min2_i(( (int)pix.R*2104 + (int)pix.G*4130 + (int)pix.B* 802 + 4096 +  131072) >> 13, 235);
                    pixyuv.U = (gan_uint8) gan_min2_i((-(int)pix.R*1214 - (int)pix.G*2384 + (int)pix.B*3598 + 4096 + 1048576) >> 13, 240);
                    pixyuv.V = (gan_uint8) gan_min2_i(( (int)pix.R*3598 - (int)pix.G*3013 - (int)pix.B* 585 + 4096 + 1048576) >> 13, 240);
                    pixyuv.A = pix.A;
                    gan_image_set_pix_yuva444_ui8(yuva444,r0d+r,c0d+c,&pixyuv);
                 }
           }
           break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_rgba_to_yuva444", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             return NULL;
        }
        break;
        
      default:
        gan_err_flush_trace();
        gan_err_register ( "extract_rgba_to_yuva444", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
        return NULL;
   }

   /* success */
   return yuva444;
}

/*
 * Converts the RGB image rgb to an RGBA image with a small alpha channel
 *
 * The RGBAS image is returned.
 */
static Gan_Image *
 extract_rgb_to_rgbas ( const Gan_Image *rgb, unsigned r0s, unsigned c0s,
                         unsigned height, unsigned width,
                         Gan_Image *rgbas, unsigned r0d, unsigned c0d )
{
   unsigned int r, c;
   
   gan_err_test_ptr ( rgb->format == GAN_RGB_COLOUR_IMAGE && rgbas->format == GAN_RGBAS,
                      "extract_rgb_to_rgbas", GAN_ERROR_INCOMPATIBLE, "" );
   switch ( rgb->type )
   {
      case GAN_UINT8:
        switch ( rgbas->type )
        {
           Gan_RGBPixel_ui8 pix;

           case GAN_UINT10:
           {
              Gan_RGBASPixel_ui10 pixrgbas;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pix = gan_image_get_pix_rgb_ui8(rgb,r0s+r,c0s+c);
                    pixrgbas.R = pix.R << 2;
                    pixrgbas.G = pix.G << 2;
                    pixrgbas.B = pix.B << 2;
                    pixrgbas.A = 0x3;
                    gan_image_set_pix_rgbas_ui10(rgbas,r0d+r,c0d+c,&pixrgbas);
                 }
           }
           break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_rgb_to_rgbas", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             return NULL;
        }
        break;
        
      case GAN_UINT16:
        switch ( rgbas->type )
        {
           Gan_RGBPixel_ui16 pix;

           case GAN_UINT10:
           {
              Gan_RGBASPixel_ui10 pixrgbas;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pix = gan_image_get_pix_rgb_ui16(rgb,r0s+r,c0s+c);
                    pixrgbas.R = pix.R >>  6;
                    pixrgbas.G = pix.G >>  6;
                    pixrgbas.B = pix.B >>  6;
                    pixrgbas.A = 0x3;
                    gan_image_set_pix_rgbas_ui10(rgbas,r0d+r,c0d+c,&pixrgbas);
                 }
           }
           break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_rgb_to_rgbas", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             return NULL;
        }
        break;
        
      default:
        gan_err_flush_trace();
        gan_err_register ( "extract_rgb_to_rgbas", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
        return NULL;
   }

   /* success */
   return rgbas;
}

/*
 * Converts the RGBA image rgba to an RGBA image with a small alpha channel
 *
 * The RGBAS image is returned.
 */
static Gan_Image *
 extract_rgba_to_rgbas ( const Gan_Image *rgba, unsigned r0s, unsigned c0s,
                         unsigned height, unsigned width,
                         Gan_Image *rgbas, unsigned r0d, unsigned c0d )
{
   unsigned int r, c;
   
   gan_err_test_ptr ( rgba->format == GAN_RGB_COLOUR_ALPHA_IMAGE && rgbas->format == GAN_RGBAS,
                      "extract_rgba_to_rgbas", GAN_ERROR_INCOMPATIBLE, "" );
   switch ( rgba->type )
   {
      case GAN_UINT8:
        switch ( rgbas->type )
        {
           Gan_RGBAPixel_ui8 pix;

           case GAN_UINT10:
           {
              Gan_RGBASPixel_ui10 pixrgbas;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pix = gan_image_get_pix_rgba_ui8(rgba,r0s+r,c0s+c);
                    pixrgbas.R = pix.R << 2;
                    pixrgbas.G = pix.G << 2;
                    pixrgbas.B = pix.B << 2;
                    pixrgbas.A = pix.A >> 6;
                    gan_image_set_pix_rgbas_ui10(rgbas,r0d+r,c0d+c,&pixrgbas);
                 }
           }
           break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_rgba_to_rgbas", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             return NULL;
        }
        break;
        
      case GAN_UINT16:
        switch ( rgbas->type )
        {
           Gan_RGBAPixel_ui16 pix;

           case GAN_UINT10:
           {
              Gan_RGBASPixel_ui10 pixrgbas;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pix = gan_image_get_pix_rgba_ui16(rgba,r0s+r,c0s+c);
                    pixrgbas.R = pix.R >>  6;
                    pixrgbas.G = pix.G >>  6;
                    pixrgbas.B = pix.B >>  6;
                    pixrgbas.A = pix.A >> 14;
                    gan_image_set_pix_rgbas_ui10(rgbas,r0d+r,c0d+c,&pixrgbas);
                 }
           }
           break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_rgba_to_rgbas", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             return NULL;
        }
        break;
        
      default:
        gan_err_flush_trace();
        gan_err_register ( "extract_rgba_to_rgbas", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
        return NULL;
   }

   /* success */
   return rgbas;
}

/*
 * Converts the padded RGB image rgbas with small alpha channel to an RGBA image rgba.
 *
 * The RGBA image is returned.
 */
static Gan_Image *
 extract_rgbas_to_rgba ( const Gan_Image *rgbas, unsigned r0s, unsigned c0s,
                         unsigned height, unsigned width,
                         Gan_Image *rgba, unsigned r0d, unsigned c0d )
{
   unsigned int r, c;
   
   gan_err_test_ptr ( rgbas->format == GAN_RGBAS && rgba->format == GAN_RGB_COLOUR_ALPHA_IMAGE,
                      "extract_rgbas_to_rgba", GAN_ERROR_INCOMPATIBLE, "" );
   switch ( rgbas->type )
   {
      case GAN_UINT10:
        switch ( rgba->type )
        {
           case GAN_UINT8:
           {
              Gan_RGBASPixel_ui10 pixas;
              Gan_RGBAPixel_ui8 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pixas = gan_image_get_pix_rgbas_ui10(rgbas,r0s+r,c0s+c);
                    pix.R = pixas.R >> 2;
                    pix.G = pixas.G >> 2;
                    pix.B = pixas.B >> 2;
                    pix.A = pixas.A << 6;
                    gan_image_set_pix_rgba_ui8(rgba,r0d+r,c0d+c,&pix);
                 }
           }
           break;

           case GAN_UINT16:
           {
              Gan_RGBASPixel_ui10 pixas;
              Gan_RGBAPixel_ui16 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pixas = gan_image_get_pix_rgbas_ui10(rgbas,r0s+r,c0s+c);
                    pix.R = pixas.R <<  6;
                    pix.G = pixas.G <<  6;
                    pix.B = pixas.B <<  6;
                    pix.A = pixas.A << 14;
                    gan_image_set_pix_rgba_ui16(rgba,r0d+r,c0d+c,&pix);
                 }
           }
           break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_rgbas_to_rgba", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             return NULL;
        }
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "extract_rgbas_to_rgba", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
        return NULL;
   }

   /* success */
   return rgba;
}

/*
 * Converts the YUVA-444 image yuva444 to an RGBA image rgba.
 *
 * According to CCIR 601, specs:
 *   R = Y + (1.4075 * (V - 128));
 *   G = Y - (0.3455 * (U - 128) - (0.7169 * (V - 128));
 *   B = Y + (1.7790 * (U - 128);
 *
 * But we have to take into account the fact that the YUV values are clamped to 16-235(Y) and 16-240(U,V).
 *
 * So the scale coeffients are
 *   R = 1.1643*Y              + 1.59596*V  - 224.288;
 *   G = 1.1643*Y - 0.391712*U - 0.813013*V + 135.596
 *   B = 1.1643*Y + 2.01726*U                -278.403
 *
 * The RGBA image is returned.
 */
static Gan_Image *
 extract_yuva444_to_rgba ( const Gan_Image *yuva444, unsigned r0s, unsigned c0s,
                           unsigned height, unsigned width,
                           Gan_Image *rgba, unsigned r0d, unsigned c0d )
{
   unsigned int r, c;
   
   gan_err_test_ptr ( yuva444->format == GAN_YUVA444 && rgba->format == GAN_RGB_COLOUR_ALPHA_IMAGE,
                      "extract_yuva444_to_rgba", GAN_ERROR_INCOMPATIBLE, "" );
   switch ( yuva444->type )
   {
      case GAN_UINT8:
        switch ( rgba->type )
        {
           case GAN_UINT8:
           {
              Gan_YUVA444Pixel_ui8 pixyuva;
              int pixi;
              Gan_RGBAPixel_ui8 pix;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
#define CLAMP_0_TO_255(p) (((p) < 0) ? 0 : (((p) > 255) ? 255 : (p)))
                    pixyuva = gan_image_get_pix_yuva444_ui8(yuva444,r0s+r,c0s+c);
                    pixi = (116430*(int)pixyuva.Y                        + 159596*(int)pixyuva.V - 22428800)/100000;
                    pix.R = (gan_uint8) CLAMP_0_TO_255(pixi);
                    pixi = (116430*(int)pixyuva.Y -  39171*(int)pixyuva.U -  81301*(int)pixyuva.V + 13559600)/100000;
                    pix.G = (gan_uint8) CLAMP_0_TO_255(pixi);
                    pixi = (116430*(int)pixyuva.Y + 201726*(int)pixyuva.U                        - 27840300)/100000;
                    pix.B = (gan_uint8) CLAMP_0_TO_255(pixi);
                    pix.A = pixyuva.A;
                    gan_image_set_pix_rgba_ui8(rgba,r0d+r,c0d+c,&pix);
#undef CLAMP_0_TO_255
                 }
           }
           break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_yuva444_to_rgba", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             return NULL;
        }
        break;
        
      default:
        gan_err_flush_trace();
        gan_err_register ( "extract_yuva444_to_rgba", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
        return NULL;
   }

   /* success */
   return rgba;
}

/*
 * Converts the padded YUV-422 image yuv422 to an RGB image rgb.
 *
 * According to CCIR 601, specs:
 *   R = Y + (1.4075 * (V - 128));
 *   G = Y - (0.3455 * (U - 128) - (0.7169 * (V - 128));
 *   B = Y + (1.7790 * (U - 128);
 *
 * But we have to take into account the fact that the YUV values are clamped to 16-235(Y) and 16-240(U,V).
 *
 * So the scale coeffients are
 *   R = 1.1643*Y              + 1.59596*V  - 224.562;
 *   G = 1.1643*Y - 0.391712*U - 0.813013*V + 135.762
 *   B = 1.1643*Y + 2.01726*U                -278.744
 *
 * The RGB image is returned.
 */
static Gan_Image *
 extract_yuv422_to_rgb ( const Gan_Image *yuv422, unsigned r0s, unsigned c0s,
                         unsigned height, unsigned width,
                         Gan_Image *rgb, unsigned r0d, unsigned c0d )
{
   unsigned int r, c;
   
   gan_err_test_ptr ( yuv422->format == GAN_YUV422 && rgb->format == GAN_RGB_COLOUR_IMAGE,
                      "extract_yuv422_to_rgb", GAN_ERROR_INCOMPATIBLE, "" );

   /* because the data is spread across two elements, we should not split the elements */
   gan_err_test_ptr ( (c0s % 2) == 0 && (width % 2) == 0 && (c0d % 2) == 0, "extract_yuv422_to_rgb", GAN_ERROR_INCOMPATIBLE, "" );

   switch ( yuv422->type )
   {
      case GAN_UINT8:
        switch ( rgb->type )
        {
           case GAN_UINT8:
           {
              Gan_YUV422Pixel_ui8 pixyuv1, pixyuv2;
              int pixi;
              Gan_RGBPixel_ui8 pix1, pix2;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c+=2 )
                 {
#define CLAMP_0_TO_255(p) (((p) < 0) ? 0 : (((p) > 255) ? 255 : (p)))
                    pixyuv1 = gan_image_get_pix_yuv422_ui8(yuv422,r0s+r,c0s+c);
                    pixyuv2 = gan_image_get_pix_yuv422_ui8(yuv422,r0s+r,c0s+c+1);
                    pixi = (int)(116430*(int)pixyuv1.Y                    + 159596*pixyuv2.UV  - 22428800)/100000;
                    pix1.R = CLAMP_0_TO_255(pixi);
                    pixi = (int)(116430*(int)pixyuv1.Y -  39171*pixyuv1.UV -  81301*pixyuv2.UV + 13559600)/100000;
                    pix1.G = CLAMP_0_TO_255(pixi);
                    pixi = (int)(116430*(int)pixyuv1.Y + 201726*pixyuv1.UV                     - 27840300)/100000;
                    pix1.B = CLAMP_0_TO_255(pixi);
                    pixi = (int)(116430*(int)pixyuv2.Y                    + 159596*pixyuv2.UV  - 22428800)/100000;
                    pix2.R = CLAMP_0_TO_255(pixi);
                    pixi = (int)(116430*(int)pixyuv2.Y -  39171*pixyuv1.UV -  81301*pixyuv2.UV + 13559600)/100000;
                    pix2.G = CLAMP_0_TO_255(pixi);
                    pixi = (int)(116430*(int)pixyuv2.Y + 201726*pixyuv1.UV                     - 27840300)/100000;
                    pix2.B = CLAMP_0_TO_255(pixi);
                    gan_image_set_pix_rgb_ui8(rgb,r0d+r,c0d+c,  &pix1);
                    gan_image_set_pix_rgb_ui8(rgb,r0d+r,c0d+c+1,&pix2);
#undef CLAMP_0_TO_255
                 }
           }
           break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_yuv422_to_rgb", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             return NULL;
        }
        break;
        
      default:
        gan_err_flush_trace();
        gan_err_register ( "extract_yuv422_to_rgb", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
        return NULL;
   }

   /* success */
   return rgb;
}

/*
 * Converts the RGB image rgb to an RGB image with padding
 *
 * The RGBX image is returned.
 */
static Gan_Image *
 extract_rgb_to_rgbx ( const Gan_Image *rgb, unsigned r0s, unsigned c0s,
                       unsigned height, unsigned width,
                       Gan_Image *rgbx, unsigned r0d, unsigned c0d )
{
   unsigned int r, c;
   
   gan_err_test_ptr ( rgb->format == GAN_RGB_COLOUR_IMAGE && rgbx->format == GAN_RGBX,
                      "extract_rgb_to_rgbx", GAN_ERROR_INCOMPATIBLE, "" );
   switch ( rgb->type )
   {
      case GAN_UINT8:
        switch ( rgbx->type )
        {
           Gan_RGBPixel_ui8 pix;

           case GAN_UINT8:
           {
              Gan_RGBXPixel_ui8 pixx;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pix = gan_image_get_pix_rgb_ui8(rgb,r0s+r,c0s+c);
                    pixx.R = pix.R;
                    pixx.G = pix.G;
                    pixx.B = pix.B;
                    pixx.X = 0;
                    gan_image_set_pix_rgbx_ui8(rgbx,r0d+r,c0d+c,&pixx);
                 }
           }
           break;

           case GAN_UINT10:
           {
              Gan_RGBXPixel_ui10 pixx;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pix = gan_image_get_pix_rgb_ui8(rgb,r0s+r,c0s+c);
                    pixx.R = pix.R << 2;
                    pixx.G = pix.G << 2;
                    pixx.B = pix.B << 2;
                    pixx.X = 0;
                    gan_image_set_pix_rgbx_ui10(rgbx,r0d+r,c0d+c,&pixx);
                 }
           }
           break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_rgb_to_rgbx", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             return NULL;
        }
        break;
        
      case GAN_UINT16:
        switch ( rgbx->type )
        {
           Gan_RGBPixel_ui16 pix;

           case GAN_UINT8:
           {
              Gan_RGBXPixel_ui8 pixx;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pix = gan_image_get_pix_rgb_ui16(rgb,r0s+r,c0s+c);
                    pixx.R = gan_pixel_ui16_to_ui8(pix.R);
                    pixx.G = gan_pixel_ui16_to_ui8(pix.G);
                    pixx.B = gan_pixel_ui16_to_ui8(pix.B);
                    pixx.X = 0;
                    gan_image_set_pix_rgbx_ui8(rgbx,r0d+r,c0d+c,&pixx);
                 }
           }
           break;

           case GAN_UINT10:
           {
              Gan_RGBXPixel_ui10 pixx;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pix = gan_image_get_pix_rgb_ui16(rgb,r0s+r,c0s+c);
                    pixx.R = pix.R >> 6;
                    pixx.G = pix.G >> 6;
                    pixx.B = pix.B >> 6;
                    pixx.X = 0;
                    gan_image_set_pix_rgbx_ui10(rgbx,r0d+r,c0d+c,&pixx);
                 }
           }
           break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_rgb_to_rgbx", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             return NULL;
        }
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "extract_rgb_to_rgbx", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
        return NULL;
   }

   /* success */
   return rgbx;
}

/*
 * Converts the RGBA image rgba to an RGBA image with padding
 *
 * The RGBX image is returned.
 */
static Gan_Image *
 extract_rgba_to_rgbx ( const Gan_Image *rgba, unsigned r0s, unsigned c0s,
                       unsigned height, unsigned width,
                       Gan_Image *rgbx, unsigned r0d, unsigned c0d )
{
   unsigned int r, c;
   
   gan_err_test_ptr ( rgba->format == GAN_RGB_COLOUR_ALPHA_IMAGE && rgbx->format == GAN_RGBX,
                      "extract_rgba_to_rgbx", GAN_ERROR_INCOMPATIBLE, "" );
   switch ( rgba->type )
   {
      case GAN_UINT8:
        switch ( rgbx->type )
        {
           Gan_RGBAPixel_ui8 pix;

           case GAN_UINT8:
           {
              Gan_RGBXPixel_ui8 pixx;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pix = gan_image_get_pix_rgba_ui8(rgba,r0s+r,c0s+c);
                    pixx.R = pix.R;
                    pixx.G = pix.G;
                    pixx.B = pix.B;
                    pixx.X = 0;
                    gan_image_set_pix_rgbx_ui8(rgbx,r0d+r,c0d+c,&pixx);
                 }
           }
           break;

           case GAN_UINT10:
           {
              Gan_RGBXPixel_ui10 pixx;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pix = gan_image_get_pix_rgba_ui8(rgba,r0s+r,c0s+c);
                    pixx.R = pix.R << 2;
                    pixx.G = pix.G << 2;
                    pixx.B = pix.B << 2;
                    pixx.X = 0;
                    gan_image_set_pix_rgbx_ui10(rgbx,r0d+r,c0d+c,&pixx);
                 }
           }
           break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_rgba_to_rgbx", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             return NULL;
        }
        break;
        
      case GAN_UINT16:
        switch ( rgbx->type )
        {
           Gan_RGBAPixel_ui16 pix;

           case GAN_UINT8:
           {
              Gan_RGBXPixel_ui8 pixx;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pix = gan_image_get_pix_rgba_ui16(rgba,r0s+r,c0s+c);
                    pixx.R = gan_pixel_ui16_to_ui8(pix.R);
                    pixx.G = gan_pixel_ui16_to_ui8(pix.G);
                    pixx.B = gan_pixel_ui16_to_ui8(pix.B);
                    pixx.X = 0;
                    gan_image_set_pix_rgbx_ui8(rgbx,r0d+r,c0d+c,&pixx);
                 }
           }
           break;

           case GAN_UINT10:
           {
              Gan_RGBXPixel_ui10 pixx;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pix = gan_image_get_pix_rgba_ui16(rgba,r0s+r,c0s+c);
                    pixx.R = pix.R >> 6;
                    pixx.G = pix.G >> 6;
                    pixx.B = pix.B >> 6;
                    pixx.X = 0;
                    gan_image_set_pix_rgbx_ui10(rgbx,r0d+r,c0d+c,&pixx);
                 }
           }
           break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_rgba_to_rgbx", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             return NULL;
        }
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "extract_rgba_to_rgbx", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
        return NULL;
   }

   /* success */
   return rgbx;
}

/*
 * Converts the RGB image rgb to a 2D vector field image
 *
 * The 2D vector field image is returned.
 */
static Gan_Image *
 extract_rgb_to_vfield2D ( const Gan_Image *rgb, unsigned r0s, unsigned c0s,
                           unsigned height, unsigned width,
                           Gan_Image *vfield2D, unsigned r0d, unsigned c0d )
{
   unsigned int r, c;
   
   gan_err_test_ptr ( rgb->format == GAN_RGB_COLOUR_IMAGE && vfield2D->format == GAN_VECTOR_FIELD_2D,
                      "extract_rgb_to_vfield2D", GAN_ERROR_INCOMPATIBLE, "" );
   switch ( rgb->type )
   {
      case GAN_FLOAT32:
        switch ( vfield2D->type )
        {
           Gan_RGBPixel_f32 pix;

           case GAN_FLOAT32:
           {
              Gan_Vector2_f pixv;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pix = gan_image_get_pix_rgb_f32(rgb,r0s+r,c0s+c);
                    pixv.x = pix.R;
                    pixv.y = pix.G;
                    gan_image_set_pix_vfield2D_f32(vfield2D,r0d+r,c0d+c,&pixv);
                 }
           }
           break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_rgb_to_vfield2D", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             return NULL;
        }
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "extract_rgb_to_vfield2D", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
        return NULL;
   }

   /* success */
   return vfield2D;
}

/*
 * Converts the RGB image rgb to a padded YUV-444 image
 *
 * The YUVX444 image is returned.
 *
 * According to CCIR 601, specs:
 *   Y = R *  .299 + G *  .587 + B *  .114;
 *   U = R * -.169 + G * -.332 + B *  .500 + 128.;
 *   V = R *  .500 + G * -.419 + B * -.0813 + 128.;
 *
 * The fixed point version, clamped to 16-235 (Y) and (16-240) (U,V) is
 *
 *   Y = min(abs(r * 2104 + g * 4130 + b * 802 + 4096 + 131072) >> 13, 235)
 *   U = min(abs(r * -1214 + g * -2384 + b * 3598 + 4096 + 1048576) >> 13, 240)
 *   V = min(abs(r * 3598 + g * -3013 + b * -585 + 4096 + 1048576) >> 13, 240)
 *
 */
static Gan_Image *
 extract_rgb_to_yuvx444 ( const Gan_Image *rgb, unsigned r0s, unsigned c0s,
                          unsigned height, unsigned width,
                          Gan_Image *yuvx444, unsigned r0d, unsigned c0d )
{
   unsigned int r, c;
   
   gan_err_test_ptr ( rgb->format == GAN_RGB_COLOUR_IMAGE && yuvx444->format == GAN_YUVX444,
                      "extract_rgb_to_yuvx444", GAN_ERROR_INCOMPATIBLE, "" );
   switch ( rgb->type )
   {
      case GAN_UINT8:
        switch ( yuvx444->type )
        {
           Gan_RGBPixel_ui8 pix;

           case GAN_UINT8:
           {
              Gan_YUVX444Pixel_ui8 pixyuv;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pix = gan_image_get_pix_rgb_ui8(rgb,r0s+r,c0s+c);
                    pixyuv.Y = (gan_uint8) gan_min2_i(( (int)pix.R*2104 + (int)pix.G*4130 + (int)pix.B* 802 + 4096 +  131072) >> 13, 235);
                    pixyuv.U = (gan_uint8) gan_min2_i((-(int)pix.R*1214 - (int)pix.G*2384 + (int)pix.B*3598 + 4096 + 1048576) >> 13, 240);
                    pixyuv.V = (gan_uint8) gan_min2_i(( (int)pix.R*3598 - (int)pix.G*3013 - (int)pix.B* 585 + 4096 + 1048576) >> 13, 240);
                    gan_image_set_pix_yuvx444_ui8(yuvx444,r0d+r,c0d+c,&pixyuv);
                 }
           }
           break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_rgb_to_yuvx444", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             return NULL;
        }
        break;
        
      default:
        gan_err_flush_trace();
        gan_err_register ( "extract_rgb_to_yuvx444", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
        return NULL;
   }

   /* success */
   return yuvx444;
}

/*
 * Converts the RGBA image rgba to a padded YUV-444 image
 *
 * The YUVX444 image is returned.
 *
 * According to CCIR 601, specs:
 *   Y = R *  .299 + G *  .587 + B *  .114;
 *   U = R * -.169 + G * -.332 + B *  .500 + 128.;
 *   V = R *  .500 + G * -.419 + B * -.0813 + 128.;
 *
 * The fixed point version, clamped to 16-235 (Y) and (16-240) (U,V) is
 *
 *   Y = min(abs(r * 2104 + g * 4130 + b * 802 + 4096 + 131072) >> 13, 235)
 *   U = min(abs(r * -1214 + g * -2384 + b * 3598 + 4096 + 1048576) >> 13, 240)
 *   V = min(abs(r * 3598 + g * -3013 + b * -585 + 4096 + 1048576) >> 13, 240)
 *
 */
static Gan_Image *
 extract_rgba_to_yuvx444 ( const Gan_Image *rgba, unsigned r0s, unsigned c0s,
                           unsigned height, unsigned width,
                           Gan_Image *yuvx444, unsigned r0d, unsigned c0d )
{
   unsigned int r, c;
   
   gan_err_test_ptr ( rgba->format == GAN_RGB_COLOUR_ALPHA_IMAGE && yuvx444->format == GAN_YUVX444,
                      "extract_rgba_to_yuvx444", GAN_ERROR_INCOMPATIBLE, "" );
   switch ( rgba->type )
   {
      case GAN_UINT8:
        switch ( yuvx444->type )
        {
           Gan_RGBAPixel_ui8 pix;

           case GAN_UINT8:
           {
              Gan_YUVX444Pixel_ui8 pixyuv;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pix = gan_image_get_pix_rgba_ui8(rgba,r0s+r,c0s+c);
                    pixyuv.Y = (gan_uint8) gan_min2_i(( (int)pix.R*2104 + (int)pix.G*4130 + (int)pix.B* 802 + 4096 +  131072) >> 13, 235);
                    pixyuv.U = (gan_uint8) gan_min2_i((-(int)pix.R*1214 - (int)pix.G*2384 + (int)pix.B*3598 + 4096 + 1048576) >> 13, 240);
                    pixyuv.V = (gan_uint8) gan_min2_i(( (int)pix.R*3598 - (int)pix.G*3013 - (int)pix.B* 585 + 4096 + 1048576) >> 13, 240);
                    gan_image_set_pix_yuvx444_ui8(yuvx444,r0d+r,c0d+c,&pixyuv);
                 }
           }
           break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_rgba_to_yuvx444", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             return NULL;
        }
        break;
        
      default:
        gan_err_flush_trace();
        gan_err_register ( "extract_rgba_to_yuvx444", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
        return NULL;
   }

   /* success */
   return yuvx444;
}

/*
 * Converts the RGB image rgb to a YUVA-444 image
 *
 * The YUVA444 image is returned.
 *
 * According to CCIR 601, specs:
 *   Y = R *  .299 + G *  .587 + B *  .114;
 *   U = R * -.169 + G * -.332 + B *  .500 + 128.;
 *   V = R *  .500 + G * -.419 + B * -.0813 + 128.;
 *
 * The fixed point version, clamped to 16-235 (Y) and (16-240) (U,V) is
 *
 *   Y = min(abs(r * 2104 + g * 4130 + b * 802 + 4096 + 131072) >> 13, 235)
 *   U = min(abs(r * -1214 + g * -2384 + b * 3598 + 4096 + 1048576) >> 13, 240)
 *   V = min(abs(r * 3598 + g * -3013 + b * -585 + 4096 + 1048576) >> 13, 240)
 *
 */
static Gan_Image *
 extract_rgb_to_yuva444 ( const Gan_Image *rgb, unsigned r0s, unsigned c0s,
                          unsigned height, unsigned width,
                          Gan_Image *yuva444, unsigned r0d, unsigned c0d )
{
   unsigned int r, c;
   
   gan_err_test_ptr ( rgb->format == GAN_RGB_COLOUR_IMAGE && yuva444->format == GAN_YUVA444,
                      "extract_rgb_to_yuva444", GAN_ERROR_INCOMPATIBLE, "" );
   switch ( rgb->type )
   {
      case GAN_UINT8:
        switch ( yuva444->type )
        {
           Gan_RGBPixel_ui8 pix;

           case GAN_UINT8:
           {
              Gan_YUVA444Pixel_ui8 pixyuv;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c++ )
                 {
                    pix = gan_image_get_pix_rgb_ui8(rgb,r0s+r,c0s+c);
                    pixyuv.Y = (gan_uint8) gan_min2_i(( (int)pix.R*2104 + (int)pix.G*4130 + (int)pix.B* 802 + 4096 +  131072) >> 13, 235);
                    pixyuv.U = (gan_uint8) gan_min2_i((-(int)pix.R*1214 - (int)pix.G*2384 + (int)pix.B*3598 + 4096 + 1048576) >> 13, 240);
                    pixyuv.V = (gan_uint8) gan_min2_i(( (int)pix.R*3598 - (int)pix.G*3013 - (int)pix.B* 585 + 4096 + 1048576) >> 13, 240);
                    pixyuv.A = 0xff;
                    gan_image_set_pix_yuva444_ui8(yuva444,r0d+r,c0d+c,&pixyuv);
                 }
           }
           break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_rgb_to_yuva444", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             return NULL;
        }
        break;
        
      default:
        gan_err_flush_trace();
        gan_err_register ( "extract_rgb_to_yuva444", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
        return NULL;
   }

   /* success */
   return yuva444;
}

/*
 * Converts the RGB image rgb to a YUV-422 image
 *
 * The YUV422 image is returned.
 *
 *   R = Y + (1.4075 * (V - 128));
 *   G = Y - (0.3455 * (U - 128) - (0.7169 * (V - 128));
 *   B = Y + (1.7790 * (U - 128);
 *
 *   Y = R *  .299 + G *  .587 + B *  .114;
 *   U = R * -.169 + G * -.332 + B *  .500 + 128.;
 *   V = R *  .500 + G * -.419 + B * -.0813 + 128.;
 */
static Gan_Image *
 extract_rgb_to_yuv422 ( const Gan_Image *rgb, unsigned r0s, unsigned c0s,
                         unsigned height, unsigned width,
                         Gan_Image *yuv422, unsigned r0d, unsigned c0d )
{
   unsigned int r, c;
   
   gan_err_test_ptr ( rgb->format == GAN_RGB_COLOUR_IMAGE && yuv422->format == GAN_YUV422,
                      "extract_rgb_to_yuv422", GAN_ERROR_INCOMPATIBLE, "" );

   /* because the data is spread across two elements, we should not split the elements */
   gan_err_test_ptr ( (c0s % 2) == 0 && (width % 2) == 0 && (c0d % 2) == 0, "extract_rgb_to_yuv422", GAN_ERROR_INCOMPATIBLE, "" );

   switch ( rgb->type )
   {
      case GAN_UINT8:
        switch ( yuv422->type )
        {
           Gan_RGBPixel_ui8 pix1, pix2;

           case GAN_UINT8:
           {
              Gan_YUV422Pixel_ui8 pixuy, pixvy;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c+=2 )
                 {
                    pix1 = gan_image_get_pix_rgb_ui8(rgb,r0s+r,c0s+c);
                    pix2 = gan_image_get_pix_rgb_ui8(rgb,r0s+r,c0s+c+1);
                    pixuy.Y = (gan_uint8) gan_min2_i(( (int)pix1.R*2104 + (int)pix1.G*4130 + (int)pix1.B* 802 + 4096 +  131072) >> 13, 235);
                    pixvy.Y = (gan_uint8) gan_min2_i(( (int)pix2.R*2104 + (int)pix2.G*4130 + (int)pix2.B* 802 + 4096 +  131072) >> 13, 235);
                    pixuy.UV = (gan_uint8) gan_min2_i(((-((int)pix1.R+(int)pix2.R)*1214 - ((int)pix1.G+(int)pix2.G)*2384 + ((int)pix1.B+(int)pix2.B)*3598)/2 + 4096 + 1048576) >> 13, 240);
                    pixvy.UV = (gan_uint8) gan_min2_i(( (((int)pix1.R+(int)pix2.R)*3598 - ((int)pix1.G+(int)pix2.G)*3013 - ((int)pix1.B+(int)pix2.B)* 585)/2 + 4096 + 1048576) >> 13, 240);
                    gan_image_set_pix_yuv422_ui8(yuv422,r0d+r,c0d+c,  &pixuy);
                    gan_image_set_pix_yuv422_ui8(yuv422,r0d+r,c0d+c+1,&pixvy);
                 }
           }
           break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_rgb_to_yuv422", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             return NULL;
        }
        break;
        
      default:
        gan_err_flush_trace();
        gan_err_register ( "extract_rgb_to_yuv422", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
        return NULL;
   }

   /* success */
   return yuv422;
}

/*
 * Converts the RGBA image rgba to a YUV-422 image
 *
 * The YUV422 image is returned.
 *
 *   R = Y + (1.4075 * (V - 128));
 *   G = Y - (0.3455 * (U - 128) - (0.7169 * (V - 128));
 *   B = Y + (1.7790 * (U - 128);
 *
 *   Y = R *  .299 + G *  .587 + B *  .114;
 *   U = R * -.169 + G * -.332 + B *  .500 + 128.;
 *   V = R *  .500 + G * -.419 + B * -.0813 + 128.;
 */
static Gan_Image *
 extract_rgba_to_yuv422 ( const Gan_Image *rgba, unsigned r0s, unsigned c0s,
                         unsigned height, unsigned width,
                         Gan_Image *yuv422, unsigned r0d, unsigned c0d )
{
   unsigned int r, c;
   
   gan_err_test_ptr ( rgba->format == GAN_RGB_COLOUR_ALPHA_IMAGE && yuv422->format == GAN_YUV422,
                      "extract_rgba_to_yuv422", GAN_ERROR_INCOMPATIBLE, "" );

   /* because the data is spread across two elements, we should not split the elements */
   gan_err_test_ptr ( (c0s % 2) == 0 && (width % 2) == 0 && (c0d % 2) == 0, "extract_rgba_to_yuv422", GAN_ERROR_INCOMPATIBLE, "" );

   switch ( rgba->type )
   {
      case GAN_UINT8:
        switch ( yuv422->type )
        {
           Gan_RGBAPixel_ui8 pix1, pix2;

           case GAN_UINT8:
           {
              Gan_YUV422Pixel_ui8 pixuy, pixvy;
   
              for ( r=0; r < height; r++ )
                 for ( c=0; c < width; c+=2 )
                 {
                    pix1 = gan_image_get_pix_rgba_ui8(rgba,r0s+r,c0s+c);
                    pix2 = gan_image_get_pix_rgba_ui8(rgba,r0s+r,c0s+c+1);
                    pixuy.Y = (gan_uint8) gan_min2_i(( (int)pix1.R*2104 + (int)pix1.G*4130 + (int)pix1.B* 802 + 4096 +  131072) >> 13, 235);
                    pixvy.Y = (gan_uint8) gan_min2_i(( (int)pix2.R*2104 + (int)pix2.G*4130 + (int)pix2.B* 802 + 4096 +  131072) >> 13, 235);
                    pixuy.UV = (gan_uint8) gan_min2_i(((-((int)pix1.R+(int)pix2.R)*1214 - ((int)pix1.G+(int)pix2.G)*2384 + ((int)pix1.B+(int)pix2.B)*3598)/2 + 4096 + 1048576) >> 13, 240);
                    pixvy.UV = (gan_uint8) gan_min2_i(( (((int)pix1.R+(int)pix2.R)*3598 - ((int)pix1.G+(int)pix2.G)*3013 - ((int)pix1.B+(int)pix2.B)* 585)/2 + 4096 + 1048576) >> 13, 240);
                    gan_image_set_pix_yuv422_ui8(yuv422,r0d+r,c0d+c,  &pixuy);
                    gan_image_set_pix_yuv422_ui8(yuv422,r0d+r,c0d+c+1,&pixvy);
                 }
           }
           break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "extract_rgba_to_yuv422", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             return NULL;
        }
        break;
        
      default:
        gan_err_flush_trace();
        gan_err_register ( "extract_rgba_to_yuv422", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
        return NULL;
   }

   /* success */
   return yuv422;
}

/**
 * \brief Extracts part of an image.
 * \param source The input image
 * \param r0 Vertical offset of start of region
 * \param c0 Horizontal offset of start of region
 * \param height Height of region in pixels
 * \param width Width of region in pixels
 * \param format The format of the output image
 * \param type The type of the output image
 * \param copy_pixels Whether to copy the pixels from the source image
 * \param dest Destination image
 * \return Pointer to the extracted/converted image, or \c NULL on failure.
 *
 * Extracts a sub-part of an image, converting the image to a different format
 * and type if desired. If the \a copy_pixels flag is set to #GAN_TRUE,
 * the pixel data is copied into the destination image; otherwise the
 * destination image is set to point into the source image.
 *
 * \sa gan_image_extract_s().
 */
Gan_Image *
 gan_image_extract_q ( const Gan_Image *source,
                       int      r0,     int      c0,
                       unsigned height, unsigned width,
                       Gan_ImageFormat format, Gan_Type type,
                       Gan_Bool copy_pixels, Gan_Image *dest )
{
   unsigned long stride;
   unsigned r0d=0, c0d=0, offset;
   Gan_Bool alloc_dest = (Gan_Bool) (dest == NULL), fill_dest = GAN_FALSE;

   if ( copy_pixels && !alloc_dest && !dest->pix_data_alloc )
   {
      /* we're copying pixels into a sub-image which already points into
         another image, so we can't change any property of the sub-image */
      gan_err_test_ptr ( format == dest->format && type == dest->type &&
                         width == dest->width && height == dest->height,
                         "gan_image_extract_q", GAN_ERROR_INCOMPATIBLE,
                         "format/type/dimensions can't change" );
   }
   else
   {
      /* set stride of output image */
      if ( copy_pixels )
         /* we're copying the data, so set the stride to match the output
            width */
         stride = gan_image_min_stride ( format, type, width, 0 );
      else
      {
         /* the destination image will point into the input image, so the
            format and type cannot change */
         gan_err_test_ptr ( format == source->format && type == source->type,
                            "gan_image_extract_q", GAN_ERROR_INCOMPATIBLE,
                            "format/type can't change" );

         /* the stride should be set to that of the input image */
         stride = source->stride;
      }

      /* allocate/set output image */
      if ( alloc_dest )
         dest = gan_image_form_gen ( NULL, format, type, height, width, stride,
                                     copy_pixels, NULL, 0, NULL, 0 );
      else
         dest = gan_image_set_format_type_dims_gen ( dest, format, type,
                                                     height, width, stride,
                                                     copy_pixels );

      if ( dest == NULL )
      {
         gan_err_register ( "gan_image_extract_q", GAN_ERROR_FAILURE, "" );
         return NULL;
      }
   }
   
   /* set offsets in destination image*/
   dest->offset_x = source->offset_x + (int)c0;
   dest->offset_y = source->offset_y + (int)r0;
      
   /* compute actual region to copy, compressing it to be inside the source
      image, and computing the offsets in the destination image */
   if ( c0 < 0 )
   {
      offset = (unsigned)(-c0);

      if ( width < offset )
      {
         /* no pixels to extract: fill with zeros if destination image was
            created and return immediately */
         if ( alloc_dest )
            gan_image_fill_zero(dest);

         return dest;
      }

      width -= offset;
      c0d = offset;
      c0 = 0;
      fill_dest = GAN_TRUE;
   }

   offset = (unsigned)c0;
   if ( offset + width > source->width )
   {
      if ( offset >= source->width )
      {
         /* no pixels to extract: fill with zeros if destination image was
            created and return immediately */
         if ( alloc_dest )
            gan_image_fill_zero(dest);

         return dest;
      }

      width = source->width - offset;
      fill_dest = GAN_TRUE;
   }

   if ( r0 < 0 )
   {
      offset = (unsigned)(-r0);

      if ( height < offset )
      {
         /* no pixels to extract: fill with zeros if destination image was
            created and return immediately */
         if ( alloc_dest )
            gan_image_fill_zero(dest);

         return dest;
      }

      height -= offset;
      r0d = offset;
      r0 = 0;
      fill_dest = GAN_TRUE;
   }

   offset = (unsigned)r0;
   if ( offset + height > source->height )
   {
      if ( offset >= source->height )
      {
         /* no pixels to extract: fill with zeros if destination image was
            created and return immediately */
         if ( alloc_dest )
            gan_image_fill_zero(dest);

         return dest;
      }

      height = source->height - offset;
      fill_dest = GAN_TRUE;
   }

   if ( width == 0 || height == 0 )
   {
      /* no pixels to extract: fill with zeros if destination image was
         created and return immediately */
      if ( alloc_dest )
         gan_image_fill_zero(dest);

      return dest;
   }

   /* fill destination image with zeros if it has a border of non-filled
      pixels */
   if ( fill_dest ) gan_image_fill_zero(dest);

   switch ( format )
   {
      case GAN_GREY_LEVEL_IMAGE:
        switch(source->format)
        {
           case GAN_GREY_LEVEL_IMAGE:
             dest = extract_grey ( source, r0, c0, height, width, copy_pixels, dest, r0d, c0d );
             break;
         
           case GAN_GREY_LEVEL_ALPHA_IMAGE:
             dest = extract_gla_to_grey ( source, r0, c0, height, width, dest, r0d, c0d );
             break;
         
           case GAN_RGB_COLOUR_IMAGE:
             dest = extract_rgb_to_grey ( source, r0, c0, height, width, dest, r0d, c0d );
             break;

           case GAN_RGB_COLOUR_ALPHA_IMAGE:
             dest = extract_rgba_to_grey ( source, r0, c0, height, width, dest, r0d, c0d );
             break;
         
           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_extract_q", GAN_ERROR_NOT_IMPLEMENTED, "image conversion");
             return NULL;
        }
        break;
      
      case GAN_GREY_LEVEL_ALPHA_IMAGE:
        switch(source->format)
        {
           case GAN_GREY_LEVEL_IMAGE:
             dest = extract_grey_to_gla ( source, r0, c0, height, width, dest, r0d, c0d );
             break;
            
           case GAN_GREY_LEVEL_ALPHA_IMAGE:
             dest = extract_gla ( source, r0, c0, height, width, copy_pixels, dest, r0d, c0d );
             break;
            
           case GAN_RGB_COLOUR_IMAGE:
             dest = extract_rgb_to_gla ( source, r0, c0, height, width, dest, r0d, c0d );
             break;

           case GAN_RGB_COLOUR_ALPHA_IMAGE:
             dest = extract_rgba_to_gla ( source, r0, c0, height, width, dest, r0d, c0d );
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_extract_q", GAN_ERROR_NOT_IMPLEMENTED, "image conversion");
             return NULL;
        }
        break;
         
      case GAN_RGB_COLOUR_IMAGE:
        switch(source->format)
        {
           case GAN_GREY_LEVEL_IMAGE:
             dest = extract_grey_to_rgb ( source, r0, c0, height, width, dest, r0d, c0d );
             break;

           case GAN_GREY_LEVEL_ALPHA_IMAGE:
             dest = extract_gla_to_rgb ( source, r0, c0, height, width, dest, r0d, c0d );
             break;

           case GAN_RGB_COLOUR_IMAGE:
             dest = extract_rgb ( source, r0, c0, height, width, copy_pixels, dest, r0d, c0d );
             break;
               
           case GAN_RGB_COLOUR_ALPHA_IMAGE:
             dest = extract_rgba_to_rgb ( source, r0, c0, height, width, dest, r0d, c0d );
             break;
               
           case GAN_RGBX:
             dest = extract_rgbx_to_rgb ( source, r0, c0, height, width, dest, r0d, c0d );
             break;
               
           case GAN_VECTOR_FIELD_2D:
             dest = extract_vfield2D_to_rgb ( source, r0, c0, height, width, dest, r0d, c0d );
             break;
               
           case GAN_YUVX444:
             dest = extract_yuvx444_to_rgb ( source, r0, c0, height, width, dest, r0d, c0d );
             break;
               
           case GAN_YUV422:
             dest = extract_yuv422_to_rgb ( source, r0, c0, height, width, dest, r0d, c0d );
             break;
               
           default:
             gan_err_flush_trace();
             gan_err_register (  "gan_image_extract_q", GAN_ERROR_NOT_IMPLEMENTED, "image conversion" );
             return NULL;
        }
        break;
            
      case GAN_RGB_COLOUR_ALPHA_IMAGE:
        switch(source->format)
        {
           case GAN_GREY_LEVEL_IMAGE:
             dest = extract_grey_to_rgba ( source, r0, c0, height, width, dest, r0d, c0d );
             break;
               
           case GAN_GREY_LEVEL_ALPHA_IMAGE:
             dest = extract_gla_to_rgba ( source, r0, c0, height, width, dest, r0d, c0d );
             break;
               
           case GAN_RGB_COLOUR_IMAGE:
             dest = extract_rgb_to_rgba ( source, r0, c0, height, width, dest, r0d, c0d );
             break;
               
           case GAN_RGB_COLOUR_ALPHA_IMAGE:
             dest = extract_rgba ( source, r0, c0, height, width, copy_pixels, dest, r0d, c0d );
             break;
               
           case GAN_RGBAS:
             dest = extract_rgbas_to_rgba ( source, r0, c0, height, width, dest, r0d, c0d );
             break;
               
           case GAN_YUVA444:
             dest = extract_yuva444_to_rgba ( source, r0, c0, height, width, dest, r0d, c0d );
             break;
               
           default:
             gan_err_flush_trace();
             gan_err_register (  "gan_image_extract_q", GAN_ERROR_NOT_IMPLEMENTED, "image conversion" );
             return NULL;
        }
        break;
            
      case GAN_RGBX:
        switch(source->format)
        {
           case GAN_RGBX:
             dest = extract_rgbx ( source, r0, c0, height, width, copy_pixels, dest, r0d, c0d );
             break;
               
           case GAN_RGB_COLOUR_IMAGE:
             dest = extract_rgb_to_rgbx ( source, r0, c0, height, width, dest, r0d, c0d );
             break;
               
           case GAN_RGB_COLOUR_ALPHA_IMAGE:
             dest = extract_rgba_to_rgbx ( source, r0, c0, height, width, dest, r0d, c0d );
             break;
               
           default:
             gan_err_flush_trace();
             gan_err_register (  "gan_image_extract_q", GAN_ERROR_NOT_IMPLEMENTED, "image conversion" );
             return NULL;
        }
        break;
            
      case GAN_RGBAS:
        switch(source->format)
        {
           case GAN_RGB_COLOUR_IMAGE:
             dest = extract_rgb_to_rgbas ( source, r0, c0, height, width, dest, r0d, c0d );
             break;
               
           case GAN_RGB_COLOUR_ALPHA_IMAGE:
             dest = extract_rgba_to_rgbas ( source, r0, c0, height, width, dest, r0d, c0d );
             break;
               
           case GAN_RGBAS:
             dest = extract_rgbas ( source, r0, c0, height, width, copy_pixels, dest, r0d, c0d );
             break;
               
           default:
             gan_err_flush_trace();
             gan_err_register (  "gan_image_extract_q", GAN_ERROR_NOT_IMPLEMENTED, "image conversion" );
             return NULL;
        }
        break;
            
      case GAN_VECTOR_FIELD_2D:
        switch(source->format)
        {
           case GAN_RGB_COLOUR_IMAGE:
             dest = extract_rgb_to_vfield2D ( source, r0, c0, height, width, dest, r0d, c0d );
             break;
               
           case GAN_VECTOR_FIELD_2D:
             dest = extract_vfield2D ( source, r0, c0, height, width, copy_pixels, dest, r0d, c0d );
             break;
               
           default:
             gan_err_flush_trace();
             gan_err_register (  "gan_image_extract_q", GAN_ERROR_NOT_IMPLEMENTED, "image conversion" );
             return NULL;
        }
        break;
            
      case GAN_YUVX444:
        switch(source->format)
        {
           case GAN_RGB_COLOUR_IMAGE:
             dest = extract_rgb_to_yuvx444 ( source, r0, c0, height, width, dest, r0d, c0d );
             break;
               
           case GAN_RGB_COLOUR_ALPHA_IMAGE:
             dest = extract_rgba_to_yuvx444 ( source, r0, c0, height, width, dest, r0d, c0d );
             break;
               
           case GAN_YUVX444:
             dest = extract_yuvx444 ( source, r0, c0, height, width, copy_pixels, dest, r0d, c0d );
             break;
               
           default:
             gan_err_flush_trace();
             gan_err_register (  "gan_image_extract_q", GAN_ERROR_NOT_IMPLEMENTED, "image conversion" );
             return NULL;
        }
        break;
            
      case GAN_YUVA444:
        switch(source->format)
        {
           case GAN_RGB_COLOUR_IMAGE:
             dest = extract_rgb_to_yuva444 ( source, r0, c0, height, width, dest, r0d, c0d );
             break;
               
           case GAN_RGB_COLOUR_ALPHA_IMAGE:
             dest = extract_rgba_to_yuva444 ( source, r0, c0, height, width, dest, r0d, c0d );
             break;
               
           case GAN_YUVA444:
             dest = extract_yuva444 ( source, r0, c0, height, width, copy_pixels, dest, r0d, c0d );
             break;
               
           default:
             gan_err_flush_trace();
             gan_err_register (  "gan_image_extract_q", GAN_ERROR_NOT_IMPLEMENTED, "image conversion" );
             return NULL;
        }
        break;
            
      case GAN_YUV422:
        switch(source->format)
        {
           case GAN_RGB_COLOUR_IMAGE:
             dest = extract_rgb_to_yuv422 ( source, r0, c0, height, width, dest, r0d, c0d );
             break;
               
           case GAN_RGB_COLOUR_ALPHA_IMAGE:
             dest = extract_rgba_to_yuv422 ( source, r0, c0, height, width, dest, r0d, c0d );
             break;
               
           case GAN_YUV422:
             dest = extract_yuv422 ( source, r0, c0, height, width, copy_pixels, dest, r0d, c0d );
             break;
               
           default:
             gan_err_flush_trace();
             gan_err_register (  "gan_image_extract_q", GAN_ERROR_NOT_IMPLEMENTED, "image conversion" );
             return NULL;
        }
        break;
            
      default:
        gan_err_flush_trace();
        gan_err_register (  "gan_image_extract_q", GAN_ERROR_NOT_IMPLEMENTED, "image conversion" );
        return NULL;
   }
   
   if ( dest == NULL )
      gan_err_register ( "gan_image_extract_q", GAN_ERROR_FAILURE, "" );
   
   /* success */
   return dest;
}

/**
 * \}
 */

/**
 * \}
 */

/**
 * File:          $RCSfile: image_pyramid.c,v $
 * Module:        Construct multi-resolution image pyramid
 * Part of:       Gandalf Library
 *
 * Revision:      $Revision: 1.1.2.1 $
 * Last edited:   $Date: 2007/10/14 02:20:22 $
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
#include <gandalf/image/image_pyramid.h>
#include <gandalf/image/image_gl_uint8.h>
#include <gandalf/image/image_gl_uint16.h>
#include <gandalf/image/image_gl_uint32.h>
#include <gandalf/image/image_gl_float32.h>
#include <gandalf/image/image_gl_float64.h>
#include <gandalf/image/image_rgb_float32.h>
#include <gandalf/image/image_rgb_float64.h>
#include <gandalf/image/image_rgb_uint8.h>
#include <gandalf/image/image_rgb_uint16.h>
#include <gandalf/image/image_rgb_uint32.h>
#include <gandalf/image/image_rgba_float32.h>
#include <gandalf/image/image_rgba_float64.h>
#include <gandalf/image/image_rgba_uint8.h>
#include <gandalf/image/image_rgba_uint16.h>
#include <gandalf/image/image_rgba_uint32.h>
#include <gandalf/image/image_bgr_float32.h>
#include <gandalf/image/image_bgr_float64.h>
#include <gandalf/image/image_bgr_uint8.h>
#include <gandalf/image/image_bgr_uint16.h>
#include <gandalf/image/image_bgr_uint32.h>
#include <gandalf/image/image_bgra_float32.h>
#include <gandalf/image/image_bgra_float64.h>
#include <gandalf/image/image_bgra_uint8.h>
#include <gandalf/image/image_bgra_uint16.h>
#include <gandalf/image/image_bgra_uint32.h>
#include <gandalf/image/image_vfield2D_int16.h>
#include <gandalf/image/image_vfield2D_int32.h>
#include <gandalf/image/image_vfield2D_float32.h>
#include <gandalf/image/image_vfield2D_float64.h>
#include <gandalf/image/image_vfield3D_int16.h>
#include <gandalf/image/image_vfield3D_int32.h>
#include <gandalf/image/image_vfield3D_float32.h>
#include <gandalf/image/image_vfield3D_float64.h>
#include <gandalf/image/image_bit.h>
#include <gandalf/image/image_convert.h>
#include <gandalf/common/allocate.h>
#include <gandalf/common/misc_error.h>

/**
 * \addtogroup ImagePackage
 * \{
 */

/**
 * \defgroup ImagePyramid Image Pyramids
 * \{
 */

static Gan_Bool
 halve_size_b ( Gan_Image *image, Gan_Image *mask, Gan_ImagePyramidAverage average_type,
                unsigned no_neighbours, Gan_Image *himage, Gan_Image *hmask )
{
   int i, j;

   gan_err_test_bool ( mask == NULL && hmask == NULL, "halve_size_b", GAN_ERROR_FAILURE, "" );
   if ( no_neighbours == 4 )
   {
      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
            if ( gan_image_bit_get_pix_4group(image,i*2,j*2) )
               gan_image_set_pix_b ( himage, i, j, GAN_TRUE );
   }
   else /* no_neighbours < 4 */
   {
      unsigned num;

      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
            if ( gan_image_bit_get_pix_4group(image,i*2,j*2) )
               gan_image_set_pix_b ( himage, i, j, GAN_TRUE );
            else
            {
               num = 0;
               if ( gan_image_get_pix_b ( image, i*2,   j*2 )   ) num++;
               if ( gan_image_get_pix_b ( image, i*2,   j*2+1 ) ) num++;
               if ( gan_image_get_pix_b ( image, i*2+1, j*2 )   ) num++;
               if ( gan_image_get_pix_b ( image, i*2+1, j*2+1 ) ) num++;
               if ( num >= no_neighbours )
                  gan_image_set_pix_b ( himage, i, j, GAN_TRUE );
            }
   }
   
   /* success */
   return GAN_TRUE;
}

static Gan_Bool
 halve_size_gl_ui8 ( Gan_Image *image, Gan_Image *mask, Gan_ImagePyramidAverage average_type,
                     unsigned no_neighbours, Gan_Image *himage, Gan_Image *hmask, gan_uint32 thres )
{
   int i, j;

   /* if mask is NULL, other parameters are irrelevant */
   if ( mask == NULL )
   {
      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
            gan_image_set_pix_gl_ui8 ( himage, i, j, (gan_uint8)(((gan_uint32) gan_image_get_pix_gl_ui8(image,i*2,j*2) +
                                                                  (gan_uint32) gan_image_get_pix_gl_ui8(image,i*2,j*2+1) +
                                                                  (gan_uint32) gan_image_get_pix_gl_ui8(image,i*2+1,j*2) +
                                                                  (gan_uint32) gan_image_get_pix_gl_ui8(image,i*2+1,j*2+1)
                                                                  + 2)/4) );
   }
   /* mask != NULL, action depends on other parameters */
   else if ( no_neighbours == 4 )
   {
      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
            if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
            {
               gan_image_set_pix_gl_ui8 ( himage, i, j, (gan_uint8)(((gan_uint32)gan_image_get_pix_gl_ui8(image,i*2,j*2) +
                                                                     (gan_uint32)gan_image_get_pix_gl_ui8(image,i*2,j*2+1) +
                                                                     (gan_uint32)gan_image_get_pix_gl_ui8(image,i*2+1,j*2) +
                                                                     (gan_uint32)gan_image_get_pix_gl_ui8(image,i*2+1,j*2+1)
                                                                     + 2)/4) );
               gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
            }
   }
   else if ( average_type == GAN_AVERAGE_IN_MASK )
   {
      if ( mask->type == GAN_BOOL )
      {
         for ( i = (int)image->height/2-1; i >= 0; i-- )
            for ( j = (int)image->width/2-1; j >= 0; j-- )
               if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
               {
                  gan_image_set_pix_gl_ui8 ( himage, i, j, (gan_uint8)(((gan_uint32)gan_image_get_pix_gl_ui8(image,i*2,j*2) +
                                                                        (gan_uint32)gan_image_get_pix_gl_ui8(image,i*2,j*2+1) +
                                                                        (gan_uint32)gan_image_get_pix_gl_ui8(image,i*2+1,j*2) +
                                                                        (gan_uint32)gan_image_get_pix_gl_ui8(image,i*2+1,j*2+1)
                                                                        + 2)/4) );
                  gan_image_set_pix_gl_ui16 ( hmask, i, j, 4 );
               }
               else
               {
                  gan_uint16 num=0, total=0;

                  if ( gan_image_get_pix_b(mask,i*2,j*2) )
                  {
                     total += (gan_uint16)gan_image_get_pix_gl_ui8(image,i*2,j*2);
                     num++;
                  }
                    
                  if ( gan_image_get_pix_b(mask,i*2,j*2+1) )
                  {
                     total += (gan_uint16) gan_image_get_pix_gl_ui8(image,i*2,j*2+1);
                     num++;
                  }

                  if ( gan_image_get_pix_b(mask,i*2+1,j*2) )
                  {
                     total += (gan_uint16) gan_image_get_pix_gl_ui8(image,i*2+1,j*2);
                     num++;
                  }
                    
                  if ( gan_image_get_pix_b(mask,i*2+1,j*2+1) )
                  {
                     total += (gan_uint32) gan_image_get_pix_gl_ui8(image,i*2+1,j*2+1);
                     num++;
                  }

                  if ( num >= thres )
                  {
                     gan_image_set_pix_gl_ui8 ( himage, i, j, (gan_uint8)((total + num/2)/num) );
                     gan_image_set_pix_gl_ui16 ( hmask, i, j, num );
                  }
               }             
      }
      else
      {
         for ( i = (int)image->height/2-1; i >= 0; i-- )
            for ( j = (int)image->width/2-1; j >= 0; j-- )
            {
               gan_uint32 mval, num=0, total=0;

               mval = (gan_uint32) gan_image_get_pix_gl_ui16(mask,i*2,j*2);
               if ( mval > 0 )
               {
                  total += mval*(gan_uint32) gan_image_get_pix_gl_ui8(image,i*2,j*2);
                  num += mval;
               }
                    
               mval = (gan_uint32) gan_image_get_pix_gl_ui16(mask,i*2,j*2+1);
               if ( mval > 0 )
               {
                  total += mval*(gan_uint32) gan_image_get_pix_gl_ui8(image,i*2,j*2+1);
                  num += mval;
               }

               mval = (gan_uint32) gan_image_get_pix_gl_ui16(mask,i*2+1,j*2);
               if ( mval > 0 )
               {
                  total += mval*(gan_uint32) gan_image_get_pix_gl_ui8(image,i*2+1,j*2);
                  num += mval;
               }
                    
               mval = (gan_uint32) gan_image_get_pix_gl_ui16(mask,i*2+1,j*2+1);
               if ( mval > 0 )
               {
                  total += mval*(gan_uint32) gan_image_get_pix_gl_ui8(image,i*2+1,j*2+1);
                  num += mval;
               }

               if ( num >= thres )
               {
                  gan_image_set_pix_gl_ui8 ( himage, i, j, (gan_uint8)((total + num/2)/num) );
                  gan_image_set_pix_gl_ui16 ( hmask, i, j, num );
               }             
            }
      }
   }
   else
   {
      gan_uint16 total, num;

      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
            if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
            {
               gan_image_set_pix_gl_ui8 ( himage, i, j, (gan_uint8)(((gan_uint32)gan_image_get_pix_gl_ui8(image,i*2,j*2) +
                                                                     (gan_uint32)gan_image_get_pix_gl_ui8(image,i*2,j*2+1) +
                                                                     (gan_uint32)gan_image_get_pix_gl_ui8(image,i*2+1,j*2) +
                                                                     (gan_uint32)gan_image_get_pix_gl_ui8(image,i*2+1,j*2+1)
                                                                     + 2)/4) );
               gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
            }
            else
            {
               num = total = 0;
               if ( gan_image_get_pix_b(mask,i*2,j*2) )
               {
                  total += (gan_uint16)gan_image_get_pix_gl_ui8(image,i*2,j*2);
                  num++;
               }
                    
               if ( gan_image_get_pix_b(mask,i*2,j*2+1) )
               {
                  total += (gan_uint16) gan_image_get_pix_gl_ui8(image,i*2,j*2+1);
                  num++;
               }

               if ( gan_image_get_pix_b(mask,i*2+1,j*2) )
               {
                  total += (gan_uint16) gan_image_get_pix_gl_ui8(image,i*2+1,j*2);
                  num++;
               }
                    
               if ( gan_image_get_pix_b(mask,i*2+1,j*2+1) )
               {
                  total += (gan_uint16) gan_image_get_pix_gl_ui8(image,i*2+1,j*2+1);
                  num++;
               }

               if ( num >= no_neighbours )
               {
                  gan_image_set_pix_gl_ui8 ( himage, i, j, (gan_uint8) ((total + 2)/4) );
                  gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
               }
            }
   }

   /* success */
   return GAN_TRUE;
}
 
static Gan_Bool
 halve_size_gl_ui16 ( Gan_Image *image, Gan_Image *mask, Gan_ImagePyramidAverage average_type,
                      unsigned no_neighbours, Gan_Image *himage, Gan_Image *hmask, gan_uint32 thres )
{
   int i, j;

   /* if mask is NULL, other parameters are irrelevant */
   if ( mask == NULL )
   {
      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
            gan_image_set_pix_gl_ui16 ( himage, i, j, (gan_uint16) (((gan_uint32)gan_image_get_pix_gl_ui16(image,i*2,j*2) +
                                                                     (gan_uint32)gan_image_get_pix_gl_ui16(image,i*2,j*2+1) +
                                                                     (gan_uint32)gan_image_get_pix_gl_ui16(image,i*2+1,j*2) +
                                                                     (gan_uint32)gan_image_get_pix_gl_ui16(image,i*2+1,j*2+1)
                                                                     + 2)/4) );
   }
   /* mask != NULL, action depends on other parameters */
   else if ( no_neighbours == 4 )
   {
      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
            if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
            {
               gan_image_set_pix_gl_ui16 ( himage, i, j,(gan_uint16) (((gan_uint32)gan_image_get_pix_gl_ui16(image,i*2,j*2) +
                                                                       (gan_uint32)gan_image_get_pix_gl_ui16(image,i*2,j*2+1) +
                                                                       (gan_uint32)gan_image_get_pix_gl_ui16(image,i*2+1,j*2) +
                                                                       (gan_uint32)gan_image_get_pix_gl_ui16(image,i*2+1,j*2+1)
                                                                       + 2)/4) );
               gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
            }
   }
   else if ( average_type == GAN_AVERAGE_IN_MASK )
   {
      if ( mask->type == GAN_BOOL )
      {
         for ( i = (int)image->height/2-1; i >= 0; i-- )
            for ( j = (int)image->width/2-1; j >= 0; j-- )
               if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
               {
                  gan_image_set_pix_gl_ui16 ( himage, i, j, (gan_uint16)(((gan_uint32)gan_image_get_pix_gl_ui16(image,i*2,j*2) +
                                                                          (gan_uint32)gan_image_get_pix_gl_ui16(image,i*2,j*2+1) +
                                                                          (gan_uint32)gan_image_get_pix_gl_ui16(image,i*2+1,j*2) +
                                                                          (gan_uint32)gan_image_get_pix_gl_ui16(image,i*2+1,j*2+1)
                                                                          + 2)/4) );
                  gan_image_set_pix_gl_ui16 ( hmask, i, j, 4 );
               }
               else
               {
                  gan_uint16 num=0;
                  gan_uint32 total=0;

                  if ( gan_image_get_pix_b(mask,i*2,j*2) )
                  {
                     total += (gan_uint32)gan_image_get_pix_gl_ui16(image,i*2,j*2);
                     num++;
                  }
                    
                  if ( gan_image_get_pix_b(mask,i*2,j*2+1) )
                  {
                     total += (gan_uint32) gan_image_get_pix_gl_ui16(image,i*2,j*2+1);
                     num++;
                  }

                  if ( gan_image_get_pix_b(mask,i*2+1,j*2) )
                  {
                     total += (gan_uint32) gan_image_get_pix_gl_ui16(image,i*2+1,j*2);
                     num++;
                  }
                    
                  if ( gan_image_get_pix_b(mask,i*2+1,j*2+1) )
                  {
                     total += (gan_uint32) gan_image_get_pix_gl_ui16(image,i*2+1,j*2+1);
                     num++;
                  }

                  if ( num >= thres )
                  {
                     gan_image_set_pix_gl_ui16 ( himage, i, j, (gan_uint16)((total + num/2)/num) );
                     gan_image_set_pix_gl_ui16 ( hmask, i, j, num );
                  }
               }             
      }
      else
      {
         for ( i = (int)image->height/2-1; i >= 0; i-- )
            for ( j = (int)image->width/2-1; j >= 0; j-- )
            {
               gan_uint32 mval, num=0, total=0;

               mval = (gan_uint32) gan_image_get_pix_gl_ui16(mask,i*2,j*2);
               if ( mval > 0 )
               {
                  total += mval*(gan_uint32) gan_image_get_pix_gl_ui16(image,i*2,j*2);
                  num += mval;
               }
                    
               mval = (gan_uint32) gan_image_get_pix_gl_ui16(mask,i*2,j*2+1);
               if ( mval > 0 )
               {
                  total += mval*(gan_uint32) gan_image_get_pix_gl_ui16(image,i*2,j*2+1);
                  num += mval;
               }

               mval = (gan_uint32) gan_image_get_pix_gl_ui16(mask,i*2+1,j*2);
               if ( mval > 0 )
               {
                  total += mval*(gan_uint32) gan_image_get_pix_gl_ui16(image,i*2+1,j*2);
                  num += mval;
               }
                    
               mval = (gan_uint32) gan_image_get_pix_gl_ui16(mask,i*2+1,j*2+1);
               if ( mval > 0 )
               {
                  total += mval*(gan_uint32) gan_image_get_pix_gl_ui16(image,i*2+1,j*2+1);
                  num += mval;
               }

               if ( num >= thres )
               {
                  gan_image_set_pix_gl_ui16 ( himage, i, j, (gan_uint16)((total + num/2)/num) );
                  gan_image_set_pix_gl_ui16 ( hmask, i, j, num );
               }             
            }
      }
   }
   else
   {
      unsigned total, num;

      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
            if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
            {
               gan_image_set_pix_gl_ui16 ( himage, i, j, (gan_uint16) (((gan_uint32)gan_image_get_pix_gl_ui16(image,i*2,j*2) +
                                                                        (gan_uint32)gan_image_get_pix_gl_ui16(image,i*2,j*2+1) +
                                                                        (gan_uint32)gan_image_get_pix_gl_ui16(image,i*2+1,j*2) +
                                                                        (gan_uint32)gan_image_get_pix_gl_ui16(image,i*2+1,j*2+1)
                                             + 2)/4) );
               gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
            }
            else
            {
               num = total = 0;
               if ( gan_image_get_pix_b(mask,i*2,j*2) )
               {
                  total += (gan_uint32)gan_image_get_pix_gl_ui16(image,i*2,j*2);
                  num++;
               }
                    
               if ( gan_image_get_pix_b(mask,i*2,j*2+1) )
               {
                  total += (gan_uint32) gan_image_get_pix_gl_ui16(image,i*2,j*2+1);
                  num++;
               }

               if ( gan_image_get_pix_b(mask,i*2+1,j*2) )
               {
                  total += (gan_uint32) gan_image_get_pix_gl_ui16(image,i*2+1,j*2);
                  num++;
               }
                    
               if ( gan_image_get_pix_b(mask,i*2+1,j*2+1) )
               {
                  total += (gan_uint32) gan_image_get_pix_gl_ui16(image,i*2+1,j*2+1);
                  num++;
               }

               if ( num >= no_neighbours )
               {
                  gan_image_set_pix_gl_ui16 ( himage, i, j, (gan_uint16) ((total + 2)/4) );
                  gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
               }
            }
   }

   /* success */
   return GAN_TRUE;
}

#ifdef GAN_UINT64
static Gan_Bool
 halve_size_gl_ui32 ( Gan_Image *image, Gan_Image *mask, Gan_ImagePyramidAverage average_type,
                      unsigned no_neighbours,Gan_Image *himage, Gan_Image *hmask, gan_uint32 thres )
{
   int i, j;

   /* if mask is NULL, other parameters are irrelevant */
   if ( mask == NULL )
   {
      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
            gan_image_set_pix_gl_ui32 ( himage, i, j, (gan_uint32) (((gan_uint64)gan_image_get_pix_gl_ui32(image,i*2,j*2) +
                                                                     (gan_uint64)gan_image_get_pix_gl_ui32(image,i*2,j*2+1) +
                                                                     (gan_uint64)gan_image_get_pix_gl_ui32(image,i*2+1,j*2) +
                                                                     (gan_uint64)gan_image_get_pix_gl_ui32(image,i*2+1,j*2+1) + 2)/4) );
   }
   /* mask != NULL, action depends on other parameters */
   else if ( no_neighbours == 4 )
   {
      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
            if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
            {
               gan_image_set_pix_gl_ui32 ( himage, i, j, (gan_uint32) (((gan_uint64)gan_image_get_pix_gl_ui32(image,i*2,j*2) +
                                                                        (gan_uint64)gan_image_get_pix_gl_ui32(image,i*2,j*2+1) +
                                                                        (gan_uint64)gan_image_get_pix_gl_ui32(image,i*2+1,j*2) +
                                                                        (gan_uint64)gan_image_get_pix_gl_ui32(image,i*2+1,j*2+1) + 2)/4) );
               gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
            }
   }
   else if ( average_type == GAN_AVERAGE_IN_MASK )
   {
      if ( mask->type == GAN_BOOL )
      {
         for ( i = (int)image->height/2-1; i >= 0; i-- )
            for ( j = (int)image->width/2-1; j >= 0; j-- )
               if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
               {
                  gan_image_set_pix_gl_ui32 ( himage, i, j, (gan_uint32)(((gan_uint64)gan_image_get_pix_gl_ui32(image,i*2,j*2) +
                                                                          (gan_uint64)gan_image_get_pix_gl_ui32(image,i*2,j*2+1) +
                                                                          (gan_uint64)gan_image_get_pix_gl_ui32(image,i*2+1,j*2) +
                                                                          (gan_uint64)gan_image_get_pix_gl_ui32(image,i*2+1,j*2+1)
                                                                          + 2)/4) );
                  gan_image_set_pix_gl_ui16 ( hmask, i, j, 4 );
               }
               else
               {
                  gan_uint16 num=0;
                  gan_uint64 total=0;

                  if ( gan_image_get_pix_b(mask,i*2,j*2) )
                  {
                     total += (gan_uint64)gan_image_get_pix_gl_ui32(image,i*2,j*2);
                     num++;
                  }
                    
                  if ( gan_image_get_pix_b(mask,i*2,j*2+1) )
                  {
                     total += (gan_uint64) gan_image_get_pix_gl_ui32(image,i*2,j*2+1);
                     num++;
                  }

                  if ( gan_image_get_pix_b(mask,i*2+1,j*2) )
                  {
                     total += (gan_uint64) gan_image_get_pix_gl_ui32(image,i*2+1,j*2);
                     num++;
                  }
                    
                  if ( gan_image_get_pix_b(mask,i*2+1,j*2+1) )
                  {
                     total += (gan_uint64) gan_image_get_pix_gl_ui32(image,i*2+1,j*2+1);
                     num++;
                  }

                  if ( num >= thres )
                  {
                     gan_image_set_pix_gl_ui32 ( himage, i, j, (gan_uint32)((total + num/2)/num) );
                     gan_image_set_pix_gl_ui16 ( hmask, i, j, num );
                  }
               }             
      }
      else
      {
         for ( i = (int)image->height/2-1; i >= 0; i-- )
            for ( j = (int)image->width/2-1; j >= 0; j-- )
            {
               gan_uint64 mval, num=0, total=0;

               mval = (gan_uint64) gan_image_get_pix_gl_ui16(mask,i*2,j*2);
               if ( mval > 0 )
               {
                  total += mval*(gan_uint64) gan_image_get_pix_gl_ui32(image,i*2,j*2);
                  num += mval;
               }
                    
               mval = (gan_uint64) gan_image_get_pix_gl_ui16(mask,i*2,j*2+1);
               if ( mval > 0 )
               {
                  total += mval*(gan_uint64) gan_image_get_pix_gl_ui32(image,i*2,j*2+1);
                  num += mval;
               }

               mval = (gan_uint64) gan_image_get_pix_gl_ui16(mask,i*2+1,j*2);
               if ( mval > 0 )
               {
                  total += mval*(gan_uint64) gan_image_get_pix_gl_ui32(image,i*2+1,j*2);
                  num += mval;
               }
                    
               mval = (gan_uint64) gan_image_get_pix_gl_ui16(mask,i*2+1,j*2+1);
               if ( mval > 0 )
               {
                  total += mval*(gan_uint64) gan_image_get_pix_gl_ui32(image,i*2+1,j*2+1);
                  num += mval;
               }

               if ( num >= thres )
               {
                  gan_image_set_pix_gl_ui32 ( himage, i, j, (gan_uint32)((total + num/2)/num) );
                  gan_image_set_pix_gl_ui16 ( hmask, i, j, num );
               }             
            }
      }
   }
   else
   {
      gan_uint64 total, num;

      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
            if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
            {
               gan_image_set_pix_gl_ui32 ( himage, i, j, (gan_uint32)
                                           (((gan_uint64)gan_image_get_pix_gl_ui32(image,i*2,j*2) +
                                             (gan_uint64)gan_image_get_pix_gl_ui32(image,i*2,j*2+1) +
                                             (gan_uint64)gan_image_get_pix_gl_ui32(image,i*2+1,j*2) +
                                             (gan_uint64)gan_image_get_pix_gl_ui32(image,i*2+1,j*2+1) + 2)/4) );
               gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
            }
            else
            {
               num = total = 0;
               if ( gan_image_get_pix_b(mask,i*2,j*2) )
               {
                  total += (gan_uint64) gan_image_get_pix_gl_ui32(image,i*2,j*2);
                  num++;
               }
                    
               if ( gan_image_get_pix_b(mask,i*2,j*2+1) )
               {
                  total += (gan_uint64) gan_image_get_pix_gl_ui32(image,i*2,j*2+1);
                  num++;
               }

               if ( gan_image_get_pix_b(mask,i*2+1,j*2) )
               {
                  total += (gan_uint64) gan_image_get_pix_gl_ui32(image,i*2+1,j*2);
                  num++;
               }
                    
               if ( gan_image_get_pix_b(mask,i*2+1,j*2+1) )
               {
                  total += (gan_uint64) gan_image_get_pix_gl_ui32(image,i*2+1,j*2+1);
                  num++;
               }

               if ( num >= no_neighbours )
               {
                  gan_image_set_pix_gl_ui32 ( himage, i, j, (gan_uint32)((total + 2)/4) );
                  gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
               }
            }
   }

   /* success */
   return GAN_TRUE;
}
#else
/* we don't have a 64-bit integer type, so we lose a bit of precision by dividing by four before averaging */
static Gan_Bool
 halve_size_gl_ui32 ( Gan_Image *image, Gan_Image *mask, Gan_ImagePyramidAverage average_type,
                      unsigned no_neighbours, Gan_Image *himage, Gan_Image *hmask, gan_uint32 thres )
{
   int i, j;

   /* if mask is NULL, other parameters are irrelevant */
   if ( mask == NULL )
   {
      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
            gan_image_set_pix_gl_ui32 ( himage, i, j,
                                        gan_image_get_pix_gl_ui32(image,i*2,j*2)/4 +
                                        gan_image_get_pix_gl_ui32(image,i*2,j*2+1)/4 +
                                        gan_image_get_pix_gl_ui32(image,i*2+1,j*2)/4 +
                                        gan_image_get_pix_gl_ui32(image,i*2+1,j*2+1)/4 );
   }
   /* mask != NULL, action depends on other parameters */
   else if ( no_neighbours == 4 )
   {
      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
            if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
            {
               gan_image_set_pix_gl_ui32 ( himage, i, j,
                                           gan_image_get_pix_gl_ui32(image,i*2,j*2)/4 +
                                           gan_image_get_pix_gl_ui32(image,i*2,j*2+1)/4 +
                                           gan_image_get_pix_gl_ui32(image,i*2+1,j*2)/4 +
                                           gan_image_get_pix_gl_ui32(image,i*2+1,j*2+1)/4 );
               gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
            }
   }
   else if ( average_type == GAN_AVERAGE_IN_MASK )
   {
      if ( mask->type == GAN_BOOL )
      {
         for ( i = (int)image->height/2-1; i >= 0; i-- )
            for ( j = (int)image->width/2-1; j >= 0; j-- )
               if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
               {
                  gan_image_set_pix_gl_ui32 ( himage, i, j,
                                              gan_image_get_pix_gl_ui32(image,i*2,j*2)/4 +
                                              gan_image_get_pix_gl_ui32(image,i*2,j*2+1)/4 +
                                              gan_image_get_pix_gl_ui32(image,i*2+1,j*2)/4 +
                                              gan_image_get_pix_gl_ui32(image,i*2+1,j*2+1)/4 );
                  gan_image_set_pix_gl_ui16 ( hmask, i, j, 4 );
               }
               else
               {
                  gan_uint16 num=0, total=0;

                  if ( gan_image_get_pix_b(mask,i*2,j*2) )
                  {
                     total += gan_image_get_pix_gl_ui32(image,i*2,j*2)/4;
                     num++;
                  }
                    
                  if ( gan_image_get_pix_b(mask,i*2,j*2+1) )
                  {
                     total += gan_image_get_pix_gl_ui32(image,i*2,j*2+1)/4;
                     num++;
                  }

                  if ( gan_image_get_pix_b(mask,i*2+1,j*2) )
                  {
                     total += gan_image_get_pix_gl_ui32(image,i*2+1,j*2)/4;
                     num++;
                  }
                    
                  if ( gan_image_get_pix_b(mask,i*2+1,j*2+1) )
                  {
                     total += gan_image_get_pix_gl_ui32(image,i*2+1,j*2+1)/4;
                     num++;
                  }

                  if ( num >= thres )
                  {
                     gan_image_set_pix_gl_ui32 ( himage, i, j, (total/num)*4 );
                     gan_image_set_pix_gl_ui16 ( hmask, i, j, num );
                  }
               }             
      }
      else
      {
         for ( i = (int)image->height/2-1; i >= 0; i-- )
            for ( j = (int)image->width/2-1; j >= 0; j-- )
            {
               gan_uint32 mval, num=0, total=0;

               mval = (gan_uint32) gan_image_get_pix_gl_ui16(mask,i*2,j*2);
               if ( mval > 0 )
               {
                  total += mval*(gan_image_get_pix_gl_ui32(image,i*2,j*2) >> 18);
                  num += mval;
               }
                    
               mval = (gan_uint32) gan_image_get_pix_gl_ui16(mask,i*2,j*2+1);
               if ( mval > 0 )
               {
                  total += mval*(gan_image_get_pix_gl_ui32(image,i*2,j*2+1) >> 18);
                  num += mval;
               }

               mval = (gan_uint32) gan_image_get_pix_gl_ui16(mask,i*2+1,j*2);
               if ( mval > 0 )
               {
                  total += mval*(gan_image_get_pix_gl_ui32(image,i*2+1,j*2) >> 18);
                  num += mval;
               }
                    
               mval = (gan_uint32) gan_image_get_pix_gl_ui16(mask,i*2+1,j*2+1);
               if ( mval > 0 )
               {
                  total += mval*(gan_image_get_pix_gl_ui32(image,i*2+1,j*2+1) >> 18);
                  num += mval;
               }

               if ( num >= thres )
               {
                  gan_image_set_pix_gl_ui32 ( himage, i, j, ((total)/num) << 18 );
                  gan_image_set_pix_gl_ui16 ( hmask, i, j, num );
               }             
            }
      }
   }
   else
   {
      unsigned total, num;

      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
            if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
            {
               gan_image_set_pix_gl_ui32 ( himage, i, j,
                                           gan_image_get_pix_gl_ui32(image,i*2,j*2)/4 +
                                           gan_image_get_pix_gl_ui32(image,i*2,j*2+1)/4 +
                                           gan_image_get_pix_gl_ui32(image,i*2+1,j*2)/4 +
                                           gan_image_get_pix_gl_ui32(image,i*2+1,j*2+1)/4 );
               gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
            }
            else
            {
               num = total = 0;
               if ( gan_image_get_pix_b(mask,i*2,j*2) )
               {
                  total += gan_image_get_pix_gl_ui32(image,i*2,j*2)/4;
                  num++;
               }
                    
               if ( gan_image_get_pix_b(mask,i*2,j*2+1) )
               {
                  total += gan_image_get_pix_gl_ui32(image,i*2,j*2+1)/4;
                  num++;
               }

               if ( gan_image_get_pix_b(mask,i*2+1,j*2) )
               {
                  total += gan_image_get_pix_gl_ui32(image,i*2+1,j*2)/4;
                  num++;
               }
                    
               if ( gan_image_get_pix_b(mask,i*2+1,j*2+1) )
               {
                  total += gan_image_get_pix_gl_ui32(image,i*2+1,j*2+1)/4;
                  num++;
               }

               if ( num >= no_neighbours )
               {
                  gan_image_set_pix_gl_ui32 ( himage, i, j, total );
                  gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
               }
            }
   }

   /* success */
   return GAN_TRUE;
}
#endif /* #ifdef GAN_UINT64 */

static Gan_Bool
 halve_size_gl_f32 ( Gan_Image *image, Gan_Image *mask, Gan_ImagePyramidAverage average_type,
                     unsigned no_neighbours, Gan_Image *himage, Gan_Image *hmask, gan_uint32 thres )
{
   int i, j;

   /* if mask is NULL, other parameters are irrelevant */
   if ( mask == NULL )
   {
      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
            gan_image_set_pix_gl_f32 ( himage, i, j, 0.25F*(gan_image_get_pix_gl_f32(image,i*2,j*2) +
                                                            gan_image_get_pix_gl_f32(image,i*2,j*2+1) +
                                                            gan_image_get_pix_gl_f32(image,i*2+1,j*2) +
                                                            gan_image_get_pix_gl_f32(image,i*2+1,j*2+1)) );
   }
   /* mask != NULL, action depends on other parameters */
   else if ( no_neighbours == 4 )
   {
      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
            if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
            {
               gan_image_set_pix_gl_f32 ( himage, i, j, 0.25F*(gan_image_get_pix_gl_f32(image,i*2,j*2) +
                                                               gan_image_get_pix_gl_f32(image,i*2,j*2+1) +
                                                               gan_image_get_pix_gl_f32(image,i*2+1,j*2) +
                                                               gan_image_get_pix_gl_f32(image,i*2+1,j*2+1)) );
               gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
            }
   }
   else if ( average_type == GAN_AVERAGE_IN_MASK )
   {
      if ( mask->type == GAN_BOOL )
      {
         for ( i = (int)image->height/2-1; i >= 0; i-- )
            for ( j = (int)image->width/2-1; j >= 0; j-- )
               if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
               {
                  gan_image_set_pix_gl_f32 ( himage, i, j, 0.25F*(gan_image_get_pix_gl_f32(image,i*2,j*2) +
                                                                  gan_image_get_pix_gl_f32(image,i*2,j*2+1) +
                                                                  gan_image_get_pix_gl_f32(image,i*2+1,j*2) +
                                                                  gan_image_get_pix_gl_f32(image,i*2+1,j*2+1)) );
                  gan_image_set_pix_gl_ui16 ( hmask, i, j, 4 );
               }
               else
               {
                  gan_uint16 num=0;
                  gan_float32 total=0.0F;

                  if ( gan_image_get_pix_b(mask,i*2,j*2) )
                  {
                     total += gan_image_get_pix_gl_f32(image,i*2,j*2);
                     num++;
                  }
                    
                  if ( gan_image_get_pix_b(mask,i*2,j*2+1) )
                  {
                     total += gan_image_get_pix_gl_f32(image,i*2,j*2+1);
                     num++;
                  }

                  if ( gan_image_get_pix_b(mask,i*2+1,j*2) )
                  {
                     total += gan_image_get_pix_gl_f32(image,i*2+1,j*2);
                     num++;
                  }
                    
                  if ( gan_image_get_pix_b(mask,i*2+1,j*2+1) )
                  {
                     total += gan_image_get_pix_gl_f32(image,i*2+1,j*2+1);
                     num++;
                  }

                  if ( num >= thres )
                  {
                     gan_image_set_pix_gl_f32 ( himage, i, j, total/((gan_float32)num) );
                     gan_image_set_pix_gl_ui16 ( hmask, i, j, num );
                  }
               }             
      }
      else
      {
         for ( i = (int)image->height/2-1; i >= 0; i-- )
            for ( j = (int)image->width/2-1; j >= 0; j-- )
            {
               gan_uint32 mval, num=0;
               gan_float32 total=0.0F;

               mval = (gan_uint32) gan_image_get_pix_gl_ui16(mask,i*2,j*2);
               if ( mval > 0 )
               {
                  total += (gan_float32)mval*gan_image_get_pix_gl_f32(image,i*2,j*2);
                  num += mval;
               }
                    
               mval = (gan_uint32) gan_image_get_pix_gl_ui16(mask,i*2,j*2+1);
               if ( mval > 0 )
               {
                  total += (gan_float32)mval*gan_image_get_pix_gl_f32(image,i*2,j*2+1);
                  num += mval;
               }

               mval = (gan_uint32) gan_image_get_pix_gl_ui16(mask,i*2+1,j*2);
               if ( mval > 0 )
               {
                  total += (gan_float32)mval*gan_image_get_pix_gl_f32(image,i*2+1,j*2);
                  num += mval;
               }
                    
               mval = (gan_uint32) gan_image_get_pix_gl_ui16(mask,i*2+1,j*2+1);
               if ( mval > 0 )
               {
                  total += (gan_float32)mval*gan_image_get_pix_gl_f32(image,i*2+1,j*2+1);
                  num += mval;
               }

               if ( num >= thres )
               {
                  gan_image_set_pix_gl_f32 ( himage, i, j, total/((gan_float32)num) );
                  gan_image_set_pix_gl_ui16 ( hmask, i, j, num );
               }             
            }
      }
   }
   else
   {
      gan_float32 total;
      unsigned num;

      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
            if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
            {
               gan_image_set_pix_gl_f32 ( himage, i, j, 0.25F*(gan_image_get_pix_gl_f32(image,i*2,j*2) +
                                                               gan_image_get_pix_gl_f32(image,i*2,j*2+1) +
                                                               gan_image_get_pix_gl_f32(image,i*2+1,j*2) +
                                                               gan_image_get_pix_gl_f32(image,i*2+1,j*2+1)) );
               gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
            }
            else
            {
               num = 0; total = 0.0F;
               if ( gan_image_get_pix_b(mask,i*2,j*2) )
               {
                  total += gan_image_get_pix_gl_f32(image,i*2,j*2);
                  num++;
               }
                    
               if ( gan_image_get_pix_b(mask,i*2,j*2+1) )
               {
                  total += gan_image_get_pix_gl_f32(image,i*2,j*2+1);
                  num++;
               }

               if ( gan_image_get_pix_b(mask,i*2+1,j*2) )
               {
                  total += gan_image_get_pix_gl_f32(image,i*2+1,j*2);
                  num++;
               }
                    
               if ( gan_image_get_pix_b(mask,i*2+1,j*2+1) )
               {
                  total += gan_image_get_pix_gl_f32(image,i*2+1,j*2+1);
                  num++;
               }

               if ( num >= no_neighbours )
               {
                  gan_image_set_pix_gl_f32 ( himage, i, j, 0.25F*total );
                  gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
               }
            }
   }

   /* success */
   return GAN_TRUE;
}

static Gan_Bool
 halve_size_gl_f64 ( Gan_Image *image, Gan_Image *mask, Gan_ImagePyramidAverage average_type,
                     unsigned no_neighbours, Gan_Image *himage, Gan_Image *hmask, gan_uint32 thres )
{
   int i, j;

   /* if mask is NULL, other parameters are irrelevant */
   if ( mask == NULL )
   {
      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
            gan_image_set_pix_gl_f64 ( himage, i, j,
                                       0.25*(gan_image_get_pix_gl_f64(image,i*2,j*2) +
                                             gan_image_get_pix_gl_f64(image,i*2,j*2+1) +
                                             gan_image_get_pix_gl_f64(image,i*2+1,j*2) +
                                             gan_image_get_pix_gl_f64(image,i*2+1,j*2+1)) );
   }
   /* mask != NULL, action depends on other parameters */
   else if ( no_neighbours == 4 )
   {
      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
            if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
            {
               gan_image_set_pix_gl_f64 ( himage, i, j,
                                          0.25*(gan_image_get_pix_gl_f64(image,i*2,j*2) +
                                                gan_image_get_pix_gl_f64(image,i*2,j*2+1) +
                                                gan_image_get_pix_gl_f64(image,i*2+1,j*2) +
                                                gan_image_get_pix_gl_f64(image,i*2+1,j*2+1)) );
               gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
            }
   }
   else if ( average_type == GAN_AVERAGE_IN_MASK )
   {
      if ( mask->type == GAN_BOOL )
      {
         for ( i = (int)image->height/2-1; i >= 0; i-- )
            for ( j = (int)image->width/2-1; j >= 0; j-- )
               if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
               {
                  gan_image_set_pix_gl_f64 ( himage, i, j, 0.25*(gan_image_get_pix_gl_f64(image,i*2,j*2) +
                                                                 gan_image_get_pix_gl_f64(image,i*2,j*2+1) +
                                                                 gan_image_get_pix_gl_f64(image,i*2+1,j*2) +
                                                                 gan_image_get_pix_gl_f64(image,i*2+1,j*2+1)) );
                  gan_image_set_pix_gl_ui16 ( hmask, i, j, 4 );
               }
               else
               {
                  gan_uint16 num=0;
                  gan_float64 total=0.0;

                  if ( gan_image_get_pix_b(mask,i*2,j*2) )
                  {
                     total += gan_image_get_pix_gl_f64(image,i*2,j*2);
                     num++;
                  }
                    
                  if ( gan_image_get_pix_b(mask,i*2,j*2+1) )
                  {
                     total += gan_image_get_pix_gl_f64(image,i*2,j*2+1);
                     num++;
                  }

                  if ( gan_image_get_pix_b(mask,i*2+1,j*2) )
                  {
                     total += gan_image_get_pix_gl_f64(image,i*2+1,j*2);
                     num++;
                  }
                    
                  if ( gan_image_get_pix_b(mask,i*2+1,j*2+1) )
                  {
                     total += gan_image_get_pix_gl_f64(image,i*2+1,j*2+1);
                     num++;
                  }

                  if ( num >= thres )
                  {
                     gan_image_set_pix_gl_f64 ( himage, i, j, total/((gan_float64)num) );
                     gan_image_set_pix_gl_ui16 ( hmask, i, j, num );
                  }
               }             
      }
      else
      {
         for ( i = (int)image->height/2-1; i >= 0; i-- )
            for ( j = (int)image->width/2-1; j >= 0; j-- )
            {
               gan_uint32 mval, num=0;
               gan_float64 total=0.0;

               mval = (gan_uint32) gan_image_get_pix_gl_ui16(mask,i*2,j*2);
               if ( mval > 0 )
               {
                  total += (gan_float64)mval*gan_image_get_pix_gl_f64(image,i*2,j*2);
                  num += mval;
               }
                    
               mval = (gan_uint32) gan_image_get_pix_gl_ui16(mask,i*2,j*2+1);
               if ( mval > 0 )
               {
                  total += (gan_float64)mval*gan_image_get_pix_gl_f64(image,i*2,j*2+1);
                  num += mval;
               }

               mval = (gan_uint32) gan_image_get_pix_gl_ui16(mask,i*2+1,j*2);
               if ( mval > 0 )
               {
                  total += (gan_float64)mval*gan_image_get_pix_gl_f64(image,i*2+1,j*2);
                  num += mval;
               }
                    
               mval = (gan_uint32) gan_image_get_pix_gl_ui16(mask,i*2+1,j*2+1);
               if ( mval > 0 )
               {
                  total += (gan_float64)mval*gan_image_get_pix_gl_f64(image,i*2+1,j*2+1);
                  num += mval;
               }

               if ( num >= thres )
               {
                  gan_image_set_pix_gl_f64 ( himage, i, j, total/((gan_float64)num) );
                  gan_image_set_pix_gl_ui16 ( hmask, i, j, num );
               }             
            }
      }
   }
   else
   {
      gan_float64 total;
      unsigned num;

      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
            if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
            {
               gan_image_set_pix_gl_f64 ( himage, i, j,
                                          0.25*(gan_image_get_pix_gl_f64(image,i*2,j*2) +
                                                gan_image_get_pix_gl_f64(image,i*2,j*2+1) +
                                                gan_image_get_pix_gl_f64(image,i*2+1,j*2) +
                                                gan_image_get_pix_gl_f64(image,i*2+1,j*2+1)) );
               gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
            }
            else
            {
               num = 0; total = 0.0;
               if ( gan_image_get_pix_b(mask,i*2,j*2) )
               {
                  total += gan_image_get_pix_gl_f64(image,i*2,j*2);
                  num++;
               }
                    
               if ( gan_image_get_pix_b(mask,i*2,j*2+1) )
               {
                  total += gan_image_get_pix_gl_f64(image,i*2,j*2+1);
                  num++;
               }

               if ( gan_image_get_pix_b(mask,i*2+1,j*2) )
               {
                  total += gan_image_get_pix_gl_f64(image,i*2+1,j*2);
                  num++;
               }
                    
               if ( gan_image_get_pix_b(mask,i*2+1,j*2+1) )
               {
                  total += gan_image_get_pix_gl_f64(image,i*2+1,j*2+1);
                  num++;
               }

               if ( num >= no_neighbours )
               {
                  gan_image_set_pix_gl_f64 ( himage, i, j, 0.25*total );
                  gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
               }
            }
   }

   /* success */
   return GAN_TRUE;
}

static Gan_Bool
 halve_size_rgb_ui8 ( Gan_Image *image, Gan_Image *mask, Gan_ImagePyramidAverage average_type,
                      unsigned no_neighbours, Gan_Image *himage, Gan_Image *hmask, gan_uint32 thres )
{
   int i, j;
   Gan_RGBPixel_ui8 pix1, pix2, pix3, pix4, pix;

   /* if mask is NULL, other parameters are irrelevant */
   if ( mask == NULL )
   {
      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
         {
            pix1 = gan_image_get_pix_rgb_ui8 ( image, i*2,   j*2   );
            pix2 = gan_image_get_pix_rgb_ui8 ( image, i*2,   j*2+1 );
            pix3 = gan_image_get_pix_rgb_ui8 ( image, i*2+1, j*2   );
            pix4 = gan_image_get_pix_rgb_ui8 ( image, i*2+1, j*2+1 );
            pix.R = (gan_uint8)(((gan_uint32) pix1.R + (gan_uint32) pix2.R + (gan_uint32) pix3.R + (gan_uint32) pix4.R + 2)/4);
            pix.G = (gan_uint8)(((gan_uint32) pix1.G + (gan_uint32) pix2.G + (gan_uint32) pix3.G + (gan_uint32) pix4.G + 2)/4);
            pix.B = (gan_uint8)(((gan_uint32) pix1.B + (gan_uint32) pix2.B + (gan_uint32) pix3.B + (gan_uint32) pix4.B + 2)/4);
            gan_image_set_pix_rgb_ui8 ( himage, i, j, &pix );
         }
   }
   /* mask != NULL, action depends on other parameters */
   else if ( no_neighbours == 4 )
   {
      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
            if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
            {
               pix1 = gan_image_get_pix_rgb_ui8 ( image, i*2,   j*2  );
               pix2 = gan_image_get_pix_rgb_ui8 ( image, i*2,   j*2+1);
               pix3 = gan_image_get_pix_rgb_ui8 ( image, i*2+1, j*2  );
               pix4 = gan_image_get_pix_rgb_ui8 ( image, i*2+1, j*2+1);
               pix.R = (gan_uint8)(((gan_uint32) pix1.R + (gan_uint32) pix2.R + (gan_uint32) pix3.R + (gan_uint32) pix4.R + 2)/4);
               pix.G = (gan_uint8)(((gan_uint32) pix1.G + (gan_uint32) pix2.G + (gan_uint32) pix3.G + (gan_uint32) pix4.G + 2)/4);
               pix.B = (gan_uint8)(((gan_uint32) pix1.B + (gan_uint32) pix2.B + (gan_uint32) pix3.B + (gan_uint32) pix4.B + 2)/4);
               gan_image_set_pix_rgb_ui8 ( himage, i, j, &pix );
               gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
            }
   }
   else if ( average_type == GAN_AVERAGE_IN_MASK )
   {
      if ( mask->type == GAN_BOOL )
      {
         for ( i = (int)image->height/2-1; i >= 0; i-- )
            for ( j = (int)image->width/2-1; j >= 0; j-- )
               if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
               {
                  pix1 = gan_image_get_pix_rgb_ui8 ( image, i*2,   j*2  );
                  pix2 = gan_image_get_pix_rgb_ui8 ( image, i*2,   j*2+1);
                  pix3 = gan_image_get_pix_rgb_ui8 ( image, i*2+1, j*2  );
                  pix4 = gan_image_get_pix_rgb_ui8 ( image, i*2+1, j*2+1);
                  pix.R = (gan_uint8)(((gan_uint32) pix1.R + (gan_uint32) pix2.R + (gan_uint32) pix3.R + (gan_uint32) pix4.R + 2)/4);
                  pix.G = (gan_uint8)(((gan_uint32) pix1.G + (gan_uint32) pix2.G + (gan_uint32) pix3.G + (gan_uint32) pix4.G + 2)/4);
                  pix.B = (gan_uint8)(((gan_uint32) pix1.B + (gan_uint32) pix2.B + (gan_uint32) pix3.B + (gan_uint32) pix4.B + 2)/4);
                  gan_image_set_pix_rgb_ui8 ( himage, i, j, &pix );
                  gan_image_set_pix_gl_ui16 ( hmask, i, j, 4 );
               }
               else
               {
                  gan_uint16 num=0;
                  Gan_RGBPixel_ui16 total;

                  total.R = total.G = total.B = 0;
                  if ( gan_image_get_pix_b(mask,i*2,j*2) )
                  {
                     pix = gan_image_get_pix_rgb_ui8 ( image, i*2, j*2 );
                     total.R += (gan_uint16)pix.R;
                     total.G += (gan_uint16)pix.G;
                     total.B += (gan_uint16)pix.B;
                     num++;
                  }
                    
                  if ( gan_image_get_pix_b(mask,i*2,j*2+1) )
                  {
                     pix = gan_image_get_pix_rgb_ui8 ( image, i*2, j*2+1 );
                     total.R += (gan_uint16)pix.R;
                     total.G += (gan_uint16)pix.G;
                     total.B += (gan_uint16)pix.B;
                     num++;
                  }

                  if ( gan_image_get_pix_b(mask,i*2+1,j*2) )
                  {
                     pix = gan_image_get_pix_rgb_ui8 ( image, i*2+1, j*2 );
                     total.R += (gan_uint16)pix.R;
                     total.G += (gan_uint16)pix.G;
                     total.B += (gan_uint16)pix.B;
                     num++;
                  }
                    
                  if ( gan_image_get_pix_b(mask,i*2+1,j*2+1) )
                  {
                     pix = gan_image_get_pix_rgb_ui8 ( image, i*2+1, j*2+1 );
                     total.R += (gan_uint16)pix.R;
                     total.G += (gan_uint16)pix.G;
                     total.B += (gan_uint16)pix.B;
                     num++;
                  }

                  if ( num >= thres )
                  {
                     pix.R = (gan_uint8) ((total.R + num/2)/num);
                     pix.G = (gan_uint8) ((total.G + num/2)/num);
                     pix.B = (gan_uint8) ((total.B + num/2)/num);
                     gan_image_set_pix_rgb_ui8 ( himage, i, j, &pix );
                     gan_image_set_pix_gl_ui16 ( hmask, i, j, num );
                  }
               }             
      }
      else
      {
         for ( i = (int)image->height/2-1; i >= 0; i-- )
            for ( j = (int)image->width/2-1; j >= 0; j-- )
            {
               gan_uint32 mval, num=0;
               Gan_RGBPixel_ui32 total;

               total.R = total.G = total.B = 0;

               mval = (gan_uint32) gan_image_get_pix_gl_ui16(mask,i*2,j*2);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_rgb_ui8 ( image, i*2, j*2 );
                  total.R += mval*(gan_uint32)pix.R;
                  total.G += mval*(gan_uint32)pix.G;
                  total.B += mval*(gan_uint32)pix.B;
                  num += mval;
               }
                    
               mval = (gan_uint32) gan_image_get_pix_gl_ui16(mask,i*2,j*2+1);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_rgb_ui8 ( image, i*2, j*2+1 );
                  total.R += mval*(gan_uint32)pix.R;
                  total.G += mval*(gan_uint32)pix.G;
                  total.B += mval*(gan_uint32)pix.B;
                  num += mval;
               }

               mval = (gan_uint32) gan_image_get_pix_gl_ui16(mask,i*2+1,j*2);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_rgb_ui8 ( image, i*2+1, j*2 );
                  total.R += mval*(gan_uint32)pix.R;
                  total.G += mval*(gan_uint32)pix.G;
                  total.B += mval*(gan_uint32)pix.B;
                  num += mval;
               }
                    
               mval = (gan_uint32) gan_image_get_pix_gl_ui16(mask,i*2+1,j*2+1);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_rgb_ui8 ( image, i*2+1, j*2+1 );
                  total.R += mval*(gan_uint32)pix.R;
                  total.G += mval*(gan_uint32)pix.G;
                  total.B += mval*(gan_uint32)pix.B;
                  num += mval;
               }

               if ( num >= thres )
               {
                  pix.R = (gan_uint8) ((total.R + num/2)/num);
                  pix.G = (gan_uint8) ((total.G + num/2)/num);
                  pix.B = (gan_uint8) ((total.B + num/2)/num);
                  gan_image_set_pix_rgb_ui8 ( himage, i, j, &pix );
                  gan_image_set_pix_gl_ui16 ( hmask, i, j, num );
               }             
            }
      }
   }
   else
   {
      unsigned num;
      Gan_RGBPixel_ui32 total;

      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
            if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
            {
               pix1 = gan_image_get_pix_rgb_ui8 ( image, i*2,   j*2  );
               pix2 = gan_image_get_pix_rgb_ui8 ( image, i*2,   j*2+1);
               pix3 = gan_image_get_pix_rgb_ui8 ( image, i*2+1, j*2  );
               pix4 = gan_image_get_pix_rgb_ui8 ( image, i*2+1, j*2+1);
               pix.R = (gan_uint8)(((gan_uint32) pix1.R + (gan_uint32) pix2.R + (gan_uint32) pix3.R + (gan_uint32) pix4.R + 2)/4);
               pix.G = (gan_uint8)(((gan_uint32) pix1.G + (gan_uint32) pix2.G + (gan_uint32) pix3.G + (gan_uint32) pix4.G + 2)/4);
               pix.B = (gan_uint8)(((gan_uint32) pix1.B + (gan_uint32) pix2.B + (gan_uint32) pix3.B + (gan_uint32) pix4.B + 2)/4);
               gan_image_set_pix_rgb_ui8 ( himage, i, j, &pix );
               gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
            }
            else
            {
               num = total.R = total.G = total.B = 0;
               if ( gan_image_get_pix_b(mask,i*2,j*2) )
               {
                  pix = gan_image_get_pix_rgb_ui8 ( image, i*2, j*2 );
                  total.R += (gan_uint32)pix.R;
                  total.G += (gan_uint32)pix.G;
                  total.B += (gan_uint32)pix.B;
                  num++;
               }
                    
               if ( gan_image_get_pix_b(mask,i*2,j*2+1) )
               {
                  pix = gan_image_get_pix_rgb_ui8 ( image, i*2, j*2+1 );
                  total.R += (gan_uint32)pix.R;
                  total.G += (gan_uint32)pix.G;
                  total.B += (gan_uint32)pix.B;
                  num++;
               }

               if ( gan_image_get_pix_b(mask,i*2+1,j*2) )
               {
                  pix = gan_image_get_pix_rgb_ui8 ( image, i*2+1, j*2 );
                  total.R += (gan_uint32)pix.R;
                  total.G += (gan_uint32)pix.G;
                  total.B += (gan_uint32)pix.B;
                  num++;
               }
                    
               if ( gan_image_get_pix_b(mask,i*2+1,j*2+1) )
               {
                  pix = gan_image_get_pix_rgb_ui8 ( image, i*2+1, j*2+1 );
                  total.R += (gan_uint32)pix.R;
                  total.G += (gan_uint32)pix.G;
                  total.B += (gan_uint32)pix.B;
                  num++;
               }

               if ( num >= no_neighbours )
               {
                  pix.R = (gan_uint8) ((total.R + 2)/4);
                  pix.G = (gan_uint8) ((total.G + 2)/4);
                  pix.B = (gan_uint8) ((total.B + 2)/4);
                  gan_image_set_pix_rgb_ui8 ( himage, i, j, &pix );
                  gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
               }
            }
   }

   /* success */
   return GAN_TRUE;
}

static Gan_Bool
 halve_size_rgb_ui16 ( Gan_Image *image, Gan_Image *mask, Gan_ImagePyramidAverage average_type,
                       unsigned no_neighbours, Gan_Image *himage, Gan_Image *hmask, gan_uint32 thres )
{
   int i, j;
   Gan_RGBPixel_ui16 pix1, pix2, pix3, pix4, pix;

   /* if mask is NULL, other parameters are irrelevant */
   if ( mask == NULL )
   {
      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
         {
            pix1 = gan_image_get_pix_rgb_ui16 ( image, i*2,   j*2   );
            pix2 = gan_image_get_pix_rgb_ui16 ( image, i*2,   j*2+1 );
            pix3 = gan_image_get_pix_rgb_ui16 ( image, i*2+1, j*2   );
            pix4 = gan_image_get_pix_rgb_ui16 ( image, i*2+1, j*2+1 );
            pix.R = (gan_uint16)(((gan_uint32) pix1.R + (gan_uint32) pix2.R + (gan_uint32) pix3.R + (gan_uint32) pix4.R + 2)/4);
            pix.G = (gan_uint16)(((gan_uint32) pix1.G + (gan_uint32) pix2.G + (gan_uint32) pix3.G + (gan_uint32) pix4.G + 2)/4);
            pix.B = (gan_uint16)(((gan_uint32) pix1.B + (gan_uint32) pix2.B + (gan_uint32) pix3.B + (gan_uint32) pix4.B + 2)/4);
            gan_image_set_pix_rgb_ui16 ( himage, i, j, &pix );
         }
   }
   /* mask != NULL, action depends on other parameters */
   else if ( no_neighbours == 4 )
   {
      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
            if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
            {
               pix1 = gan_image_get_pix_rgb_ui16 ( image, i*2,   j*2  );
               pix2 = gan_image_get_pix_rgb_ui16 ( image, i*2,   j*2+1);
               pix3 = gan_image_get_pix_rgb_ui16 ( image, i*2+1, j*2  );
               pix4 = gan_image_get_pix_rgb_ui16 ( image, i*2+1, j*2+1);
               pix.R = (gan_uint16)(((gan_uint32) pix1.R + (gan_uint32) pix2.R + (gan_uint32) pix3.R + (gan_uint32) pix4.R + 2)/4);
               pix.G = (gan_uint16)(((gan_uint32) pix1.G + (gan_uint32) pix2.G + (gan_uint32) pix3.G + (gan_uint32) pix4.G + 2)/4);
               pix.B = (gan_uint16)(((gan_uint32) pix1.B + (gan_uint32) pix2.B + (gan_uint32) pix3.B + (gan_uint32) pix4.B + 2)/4);
               gan_image_set_pix_rgb_ui16 ( himage, i, j, &pix );
               gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
            }
   }
   else if ( average_type == GAN_AVERAGE_IN_MASK )
   {
      if ( mask->type == GAN_BOOL )
      {
         for ( i = (int)image->height/2-1; i >= 0; i-- )
            for ( j = (int)image->width/2-1; j >= 0; j-- )
               if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
               {
                  pix1 = gan_image_get_pix_rgb_ui16 ( image, i*2,   j*2  );
                  pix2 = gan_image_get_pix_rgb_ui16 ( image, i*2,   j*2+1);
                  pix3 = gan_image_get_pix_rgb_ui16 ( image, i*2+1, j*2  );
                  pix4 = gan_image_get_pix_rgb_ui16 ( image, i*2+1, j*2+1);
                  pix.R = (gan_uint16)(((gan_uint32) pix1.R + (gan_uint32) pix2.R + (gan_uint32) pix3.R + (gan_uint32) pix4.R + 2)/4);
                  pix.G = (gan_uint16)(((gan_uint32) pix1.G + (gan_uint32) pix2.G + (gan_uint32) pix3.G + (gan_uint32) pix4.G + 2)/4);
                  pix.B = (gan_uint16)(((gan_uint32) pix1.B + (gan_uint32) pix2.B + (gan_uint32) pix3.B + (gan_uint32) pix4.B + 2)/4);
                  gan_image_set_pix_rgb_ui16 ( himage, i, j, &pix );
                  gan_image_set_pix_gl_ui16 ( hmask, i, j, 4 );
               }
               else
               {
                  gan_uint16 num=0;
                  Gan_RGBPixel_ui32 total;

                  total.R = total.G = total.B = 0;
                  if ( gan_image_get_pix_b(mask,i*2,j*2) )
                  {
                     pix = gan_image_get_pix_rgb_ui16 ( image, i*2, j*2 );
                     total.R += (gan_uint32)pix.R;
                     total.G += (gan_uint32)pix.G;
                     total.B += (gan_uint32)pix.B;
                     num++;
                  }
                    
                  if ( gan_image_get_pix_b(mask,i*2,j*2+1) )
                  {
                     pix = gan_image_get_pix_rgb_ui16 ( image, i*2, j*2+1 );
                     total.R += (gan_uint32)pix.R;
                     total.G += (gan_uint32)pix.G;
                     total.B += (gan_uint32)pix.B;
                     num++;
                  }

                  if ( gan_image_get_pix_b(mask,i*2+1,j*2) )
                  {
                     pix = gan_image_get_pix_rgb_ui16 ( image, i*2+1, j*2 );
                     total.R += (gan_uint32)pix.R;
                     total.G += (gan_uint32)pix.G;
                     total.B += (gan_uint32)pix.B;
                     num++;
                  }
                    
                  if ( gan_image_get_pix_b(mask,i*2+1,j*2+1) )
                  {
                     pix = gan_image_get_pix_rgb_ui16 ( image, i*2+1, j*2+1 );
                     total.R += (gan_uint32)pix.R;
                     total.G += (gan_uint32)pix.G;
                     total.B += (gan_uint32)pix.B;
                     num++;
                  }

                  if ( num >= thres )
                  {
                     pix.R = (gan_uint16) ((total.R + num/2)/num);
                     pix.G = (gan_uint16) ((total.G + num/2)/num);
                     pix.B = (gan_uint16) ((total.B + num/2)/num);
                     gan_image_set_pix_rgb_ui16 ( himage, i, j, &pix );
                     gan_image_set_pix_gl_ui16 ( hmask, i, j, num );
                  }
               }             
      }
      else
      {
         for ( i = (int)image->height/2-1; i >= 0; i-- )
            for ( j = (int)image->width/2-1; j >= 0; j-- )
            {
               gan_uint32 mval, num=0;
               Gan_RGBPixel_ui32 total;

               total.R = total.G = total.B = 0;

               mval = (gan_uint32) gan_image_get_pix_gl_ui16(mask,i*2,j*2);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_rgb_ui16 ( image, i*2, j*2 );
                  total.R += mval*(gan_uint32)pix.R;
                  total.G += mval*(gan_uint32)pix.G;
                  total.B += mval*(gan_uint32)pix.B;
                  num += mval;
               }
                    
               mval = (gan_uint32) gan_image_get_pix_gl_ui16(mask,i*2,j*2+1);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_rgb_ui16 ( image, i*2, j*2+1 );
                  total.R += mval*(gan_uint32)pix.R;
                  total.G += mval*(gan_uint32)pix.G;
                  total.B += mval*(gan_uint32)pix.B;
                  num += mval;
               }

               mval = (gan_uint32) gan_image_get_pix_gl_ui16(mask,i*2+1,j*2);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_rgb_ui16 ( image, i*2+1, j*2 );
                  total.R += mval*(gan_uint32)pix.R;
                  total.G += mval*(gan_uint32)pix.G;
                  total.B += mval*(gan_uint32)pix.B;
                  num += mval;
               }
                    
               mval = (gan_uint32) gan_image_get_pix_gl_ui16(mask,i*2+1,j*2+1);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_rgb_ui16 ( image, i*2+1, j*2+1 );
                  total.R += mval*(gan_uint32)pix.R;
                  total.G += mval*(gan_uint32)pix.G;
                  total.B += mval*(gan_uint32)pix.B;
                  num += mval;
               }

               if ( num >= thres )
               {
                  pix.R = (gan_uint16) ((total.R + num/2)/num);
                  pix.G = (gan_uint16) ((total.G + num/2)/num);
                  pix.B = (gan_uint16) ((total.B + num/2)/num);
                  gan_image_set_pix_rgb_ui16 ( himage, i, j, &pix );
                  gan_image_set_pix_gl_ui16 ( hmask, i, j, num );
               }             
            }
      }
   }
   else
   {
      unsigned num;
      Gan_RGBPixel_ui32 total;

      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
            if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
            {
               pix1 = gan_image_get_pix_rgb_ui16 ( image, i*2,   j*2  );
               pix2 = gan_image_get_pix_rgb_ui16 ( image, i*2,   j*2+1);
               pix3 = gan_image_get_pix_rgb_ui16 ( image, i*2+1, j*2  );
               pix4 = gan_image_get_pix_rgb_ui16 ( image, i*2+1, j*2+1);
               pix.R = (gan_uint16)(((gan_uint32) pix1.R + (gan_uint32) pix2.R + (gan_uint32) pix3.R + (gan_uint32) pix4.R + 2)/4);
               pix.G = (gan_uint16)(((gan_uint32) pix1.G + (gan_uint32) pix2.G + (gan_uint32) pix3.G + (gan_uint32) pix4.G + 2)/4);
               pix.B = (gan_uint16)(((gan_uint32) pix1.B + (gan_uint32) pix2.B + (gan_uint32) pix3.B + (gan_uint32) pix4.B + 2)/4);
               gan_image_set_pix_rgb_ui16 ( himage, i, j, &pix );
               gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
            }
            else
            {
               num = total.R = total.G = total.B = 0;
               if ( gan_image_get_pix_b(mask,i*2,j*2) )
               {
                  pix = gan_image_get_pix_rgb_ui16 ( image, i*2, j*2 );
                  total.R += (gan_uint32)pix.R;
                  total.G += (gan_uint32)pix.G;
                  total.B += (gan_uint32)pix.B;
                  num++;
               }
                    
               if ( gan_image_get_pix_b(mask,i*2,j*2+1) )
               {
                  pix = gan_image_get_pix_rgb_ui16 ( image, i*2, j*2+1 );
                  total.R += (gan_uint32)pix.R;
                  total.G += (gan_uint32)pix.G;
                  total.B += (gan_uint32)pix.B;
                  num++;
               }

               if ( gan_image_get_pix_b(mask,i*2+1,j*2) )
               {
                  pix = gan_image_get_pix_rgb_ui16 ( image, i*2+1, j*2 );
                  total.R += (gan_uint32)pix.R;
                  total.G += (gan_uint32)pix.G;
                  total.B += (gan_uint32)pix.B;
                  num++;
               }
                    
               if ( gan_image_get_pix_b(mask,i*2+1,j*2+1) )
               {
                  pix = gan_image_get_pix_rgb_ui16 ( image, i*2+1, j*2+1 );
                  total.R += (gan_uint32)pix.R;
                  total.G += (gan_uint32)pix.G;
                  total.B += (gan_uint32)pix.B;
                  num++;
               }

               if ( num >= no_neighbours )
               {
                  pix.R = (gan_uint16) ((total.R + 2)/4);
                  pix.G = (gan_uint16) ((total.G + 2)/4);
                  pix.B = (gan_uint16) ((total.B + 2)/4);
                  gan_image_set_pix_rgb_ui16 ( himage, i, j, &pix );
                  gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
               }
            }
   }

   /* success */
   return GAN_TRUE;
}

static Gan_Bool
 halve_size_bgr_ui8 ( Gan_Image *image, Gan_Image *mask, Gan_ImagePyramidAverage average_type,
                      unsigned no_neighbours, Gan_Image *himage, Gan_Image *hmask, gan_uint32 thres )
{
   int i, j;
   Gan_BGRPixel_ui8 pix1, pix2, pix3, pix4, pix;

   /* if mask is NULL, other parameters are irrelevant */
   if ( mask == NULL )
   {
      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
         {
            pix1 = gan_image_get_pix_bgr_ui8 ( image, i*2,   j*2   );
            pix2 = gan_image_get_pix_bgr_ui8 ( image, i*2,   j*2+1 );
            pix3 = gan_image_get_pix_bgr_ui8 ( image, i*2+1, j*2   );
            pix4 = gan_image_get_pix_bgr_ui8 ( image, i*2+1, j*2+1 );
            pix.R = (gan_uint8)(((gan_uint32) pix1.R + (gan_uint32) pix2.R + (gan_uint32) pix3.R + (gan_uint32) pix4.R + 2)/4);
            pix.G = (gan_uint8)(((gan_uint32) pix1.G + (gan_uint32) pix2.G + (gan_uint32) pix3.G + (gan_uint32) pix4.G + 2)/4);
            pix.B = (gan_uint8)(((gan_uint32) pix1.B + (gan_uint32) pix2.B + (gan_uint32) pix3.B + (gan_uint32) pix4.B + 2)/4);
            gan_image_set_pix_bgr_ui8 ( himage, i, j, &pix );
         }
   }
   /* mask != NULL, action depends on other parameters */
   else if ( no_neighbours == 4 )
   {
      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
            if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
            {
               pix1 = gan_image_get_pix_bgr_ui8 ( image, i*2,   j*2  );
               pix2 = gan_image_get_pix_bgr_ui8 ( image, i*2,   j*2+1);
               pix3 = gan_image_get_pix_bgr_ui8 ( image, i*2+1, j*2  );
               pix4 = gan_image_get_pix_bgr_ui8 ( image, i*2+1, j*2+1);
               pix.R = (gan_uint8)(((gan_uint32) pix1.R + (gan_uint32) pix2.R + (gan_uint32) pix3.R + (gan_uint32) pix4.R + 2)/4);
               pix.G = (gan_uint8)(((gan_uint32) pix1.G + (gan_uint32) pix2.G + (gan_uint32) pix3.G + (gan_uint32) pix4.G + 2)/4);
               pix.B = (gan_uint8)(((gan_uint32) pix1.B + (gan_uint32) pix2.B + (gan_uint32) pix3.B + (gan_uint32) pix4.B + 2)/4);
               gan_image_set_pix_bgr_ui8 ( himage, i, j, &pix );
               gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
            }
   }
   else if ( average_type == GAN_AVERAGE_IN_MASK )
   {
      if ( mask->type == GAN_BOOL )
      {
         for ( i = (int)image->height/2-1; i >= 0; i-- )
            for ( j = (int)image->width/2-1; j >= 0; j-- )
               if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
               {
                  pix1 = gan_image_get_pix_bgr_ui8 ( image, i*2,   j*2  );
                  pix2 = gan_image_get_pix_bgr_ui8 ( image, i*2,   j*2+1);
                  pix3 = gan_image_get_pix_bgr_ui8 ( image, i*2+1, j*2  );
                  pix4 = gan_image_get_pix_bgr_ui8 ( image, i*2+1, j*2+1);
                  pix.R = (gan_uint8)(((gan_uint32) pix1.R + (gan_uint32) pix2.R + (gan_uint32) pix3.R + (gan_uint32) pix4.R + 2)/4);
                  pix.G = (gan_uint8)(((gan_uint32) pix1.G + (gan_uint32) pix2.G + (gan_uint32) pix3.G + (gan_uint32) pix4.G + 2)/4);
                  pix.B = (gan_uint8)(((gan_uint32) pix1.B + (gan_uint32) pix2.B + (gan_uint32) pix3.B + (gan_uint32) pix4.B + 2)/4);
                  gan_image_set_pix_bgr_ui8 ( himage, i, j, &pix );
                  gan_image_set_pix_gl_ui16 ( hmask, i, j, 4 );
               }
               else
               {
                  gan_uint16 num=0;
                  Gan_BGRPixel_ui16 total;

                  total.R = total.G = total.B = 0;
                  if ( gan_image_get_pix_b(mask,i*2,j*2) )
                  {
                     pix = gan_image_get_pix_bgr_ui8 ( image, i*2, j*2 );
                     total.R += (gan_uint16)pix.R;
                     total.G += (gan_uint16)pix.G;
                     total.B += (gan_uint16)pix.B;
                     num++;
                  }
                    
                  if ( gan_image_get_pix_b(mask,i*2,j*2+1) )
                  {
                     pix = gan_image_get_pix_bgr_ui8 ( image, i*2, j*2+1 );
                     total.R += (gan_uint16)pix.R;
                     total.G += (gan_uint16)pix.G;
                     total.B += (gan_uint16)pix.B;
                     num++;
                  }

                  if ( gan_image_get_pix_b(mask,i*2+1,j*2) )
                  {
                     pix = gan_image_get_pix_bgr_ui8 ( image, i*2+1, j*2 );
                     total.R += (gan_uint16)pix.R;
                     total.G += (gan_uint16)pix.G;
                     total.B += (gan_uint16)pix.B;
                     num++;
                  }
                    
                  if ( gan_image_get_pix_b(mask,i*2+1,j*2+1) )
                  {
                     pix = gan_image_get_pix_bgr_ui8 ( image, i*2+1, j*2+1 );
                     total.R += (gan_uint16)pix.R;
                     total.G += (gan_uint16)pix.G;
                     total.B += (gan_uint16)pix.B;
                     num++;
                  }

                  if ( num >= thres )
                  {
                     pix.R = (gan_uint8) ((total.R + num/2)/num);
                     pix.G = (gan_uint8) ((total.G + num/2)/num);
                     pix.B = (gan_uint8) ((total.B + num/2)/num);
                     gan_image_set_pix_bgr_ui8 ( himage, i, j, &pix );
                     gan_image_set_pix_gl_ui16 ( hmask, i, j, num );
                  }
               }             
      }
      else
      {
         for ( i = (int)image->height/2-1; i >= 0; i-- )
            for ( j = (int)image->width/2-1; j >= 0; j-- )
            {
               gan_uint32 mval, num=0;
               Gan_BGRPixel_ui32 total;

               total.R = total.G = total.B = 0;

               mval = (gan_uint32) gan_image_get_pix_gl_ui16(mask,i*2,j*2);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_bgr_ui8 ( image, i*2, j*2 );
                  total.R += mval*(gan_uint32)pix.R;
                  total.G += mval*(gan_uint32)pix.G;
                  total.B += mval*(gan_uint32)pix.B;
                  num += mval;
               }
                    
               mval = (gan_uint32) gan_image_get_pix_gl_ui16(mask,i*2,j*2+1);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_bgr_ui8 ( image, i*2, j*2+1 );
                  total.R += mval*(gan_uint32)pix.R;
                  total.G += mval*(gan_uint32)pix.G;
                  total.B += mval*(gan_uint32)pix.B;
                  num += mval;
               }

               mval = (gan_uint32) gan_image_get_pix_gl_ui16(mask,i*2+1,j*2);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_bgr_ui8 ( image, i*2+1, j*2 );
                  total.R += mval*(gan_uint32)pix.R;
                  total.G += mval*(gan_uint32)pix.G;
                  total.B += mval*(gan_uint32)pix.B;
                  num += mval;
               }
                    
               mval = (gan_uint32) gan_image_get_pix_gl_ui16(mask,i*2+1,j*2+1);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_bgr_ui8 ( image, i*2+1, j*2+1 );
                  total.R += mval*(gan_uint32)pix.R;
                  total.G += mval*(gan_uint32)pix.G;
                  total.B += mval*(gan_uint32)pix.B;
                  num += mval;
               }

               if ( num >= thres )
               {
                  pix.R = (gan_uint8) ((total.R + num/2)/num);
                  pix.G = (gan_uint8) ((total.G + num/2)/num);
                  pix.B = (gan_uint8) ((total.B + num/2)/num);
                  gan_image_set_pix_bgr_ui8 ( himage, i, j, &pix );
                  gan_image_set_pix_gl_ui16 ( hmask, i, j, num );
               }             
            }
      }
   }
   else
   {
      unsigned num;
      Gan_BGRPixel_ui32 total;

      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
            if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
            {
               pix1 = gan_image_get_pix_bgr_ui8 ( image, i*2,   j*2  );
               pix2 = gan_image_get_pix_bgr_ui8 ( image, i*2,   j*2+1);
               pix3 = gan_image_get_pix_bgr_ui8 ( image, i*2+1, j*2  );
               pix4 = gan_image_get_pix_bgr_ui8 ( image, i*2+1, j*2+1);
               pix.R = (gan_uint8)(((gan_uint32) pix1.R + (gan_uint32) pix2.R + (gan_uint32) pix3.R + (gan_uint32) pix4.R + 2)/4);
               pix.G = (gan_uint8)(((gan_uint32) pix1.G + (gan_uint32) pix2.G + (gan_uint32) pix3.G + (gan_uint32) pix4.G + 2)/4);
               pix.B = (gan_uint8)(((gan_uint32) pix1.B + (gan_uint32) pix2.B + (gan_uint32) pix3.B + (gan_uint32) pix4.B + 2)/4);
               gan_image_set_pix_bgr_ui8 ( himage, i, j, &pix );
               gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
            }
            else
            {
               num = total.R = total.G = total.B = 0;
               if ( gan_image_get_pix_b(mask,i*2,j*2) )
               {
                  pix = gan_image_get_pix_bgr_ui8 ( image, i*2, j*2 );
                  total.R += (gan_uint32)pix.R;
                  total.G += (gan_uint32)pix.G;
                  total.B += (gan_uint32)pix.B;
                  num++;
               }
                    
               if ( gan_image_get_pix_b(mask,i*2,j*2+1) )
               {
                  pix = gan_image_get_pix_bgr_ui8 ( image, i*2, j*2+1 );
                  total.R += (gan_uint32)pix.R;
                  total.G += (gan_uint32)pix.G;
                  total.B += (gan_uint32)pix.B;
                  num++;
               }

               if ( gan_image_get_pix_b(mask,i*2+1,j*2) )
               {
                  pix = gan_image_get_pix_bgr_ui8 ( image, i*2+1, j*2 );
                  total.R += (gan_uint32)pix.R;
                  total.G += (gan_uint32)pix.G;
                  total.B += (gan_uint32)pix.B;
                  num++;
               }
                    
               if ( gan_image_get_pix_b(mask,i*2+1,j*2+1) )
               {
                  pix = gan_image_get_pix_bgr_ui8 ( image, i*2+1, j*2+1 );
                  total.R += (gan_uint32)pix.R;
                  total.G += (gan_uint32)pix.G;
                  total.B += (gan_uint32)pix.B;
                  num++;
               }

               if ( num >= no_neighbours )
               {
                  pix.R = (gan_uint8) ((total.R + 2)/4);
                  pix.G = (gan_uint8) ((total.G + 2)/4);
                  pix.B = (gan_uint8) ((total.B + 2)/4);
                  gan_image_set_pix_bgr_ui8 ( himage, i, j, &pix );
                  gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
               }
            }
   }

   /* success */
   return GAN_TRUE;
}

static Gan_Bool
 halve_size_bgr_ui16 ( Gan_Image *image, Gan_Image *mask, Gan_ImagePyramidAverage average_type,
                       unsigned no_neighbours, Gan_Image *himage, Gan_Image *hmask, gan_uint32 thres )
{
   int i, j;
   Gan_BGRPixel_ui16 pix1, pix2, pix3, pix4, pix;

   /* if mask is NULL, other parameters are irrelevant */
   if ( mask == NULL )
   {
      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
         {
            pix1 = gan_image_get_pix_bgr_ui16 ( image, i*2,   j*2   );
            pix2 = gan_image_get_pix_bgr_ui16 ( image, i*2,   j*2+1 );
            pix3 = gan_image_get_pix_bgr_ui16 ( image, i*2+1, j*2   );
            pix4 = gan_image_get_pix_bgr_ui16 ( image, i*2+1, j*2+1 );
            pix.R = (gan_uint16)(((gan_uint32) pix1.R + (gan_uint32) pix2.R + (gan_uint32) pix3.R + (gan_uint32) pix4.R + 2)/4);
            pix.G = (gan_uint16)(((gan_uint32) pix1.G + (gan_uint32) pix2.G + (gan_uint32) pix3.G + (gan_uint32) pix4.G + 2)/4);
            pix.B = (gan_uint16)(((gan_uint32) pix1.B + (gan_uint32) pix2.B + (gan_uint32) pix3.B + (gan_uint32) pix4.B + 2)/4);
            gan_image_set_pix_bgr_ui16 ( himage, i, j, &pix );
         }
   }
   /* mask != NULL, action depends on other parameters */
   else if ( no_neighbours == 4 )
   {
      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
            if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
            {
               pix1 = gan_image_get_pix_bgr_ui16 ( image, i*2,   j*2  );
               pix2 = gan_image_get_pix_bgr_ui16 ( image, i*2,   j*2+1);
               pix3 = gan_image_get_pix_bgr_ui16 ( image, i*2+1, j*2  );
               pix4 = gan_image_get_pix_bgr_ui16 ( image, i*2+1, j*2+1);
               pix.R = (gan_uint16)(((gan_uint32) pix1.R + (gan_uint32) pix2.R + (gan_uint32) pix3.R + (gan_uint32) pix4.R + 2)/4);
               pix.G = (gan_uint16)(((gan_uint32) pix1.G + (gan_uint32) pix2.G + (gan_uint32) pix3.G + (gan_uint32) pix4.G + 2)/4);
               pix.B = (gan_uint16)(((gan_uint32) pix1.B + (gan_uint32) pix2.B + (gan_uint32) pix3.B + (gan_uint32) pix4.B + 2)/4);
               gan_image_set_pix_bgr_ui16 ( himage, i, j, &pix );
               gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
            }
   }
   else if ( average_type == GAN_AVERAGE_IN_MASK )
   {
      if ( mask->type == GAN_BOOL )
      {
         for ( i = (int)image->height/2-1; i >= 0; i-- )
            for ( j = (int)image->width/2-1; j >= 0; j-- )
               if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
               {
                  pix1 = gan_image_get_pix_bgr_ui16 ( image, i*2,   j*2  );
                  pix2 = gan_image_get_pix_bgr_ui16 ( image, i*2,   j*2+1);
                  pix3 = gan_image_get_pix_bgr_ui16 ( image, i*2+1, j*2  );
                  pix4 = gan_image_get_pix_bgr_ui16 ( image, i*2+1, j*2+1);
                  pix.R = (gan_uint16)(((gan_uint32) pix1.R + (gan_uint32) pix2.R + (gan_uint32) pix3.R + (gan_uint32) pix4.R + 2)/4);
                  pix.G = (gan_uint16)(((gan_uint32) pix1.G + (gan_uint32) pix2.G + (gan_uint32) pix3.G + (gan_uint32) pix4.G + 2)/4);
                  pix.B = (gan_uint16)(((gan_uint32) pix1.B + (gan_uint32) pix2.B + (gan_uint32) pix3.B + (gan_uint32) pix4.B + 2)/4);
                  gan_image_set_pix_bgr_ui16 ( himage, i, j, &pix );
                  gan_image_set_pix_gl_ui16 ( hmask, i, j, 4 );
               }
               else
               {
                  gan_uint16 num=0;
                  Gan_BGRPixel_ui32 total;

                  total.R = total.G = total.B = 0;
                  if ( gan_image_get_pix_b(mask,i*2,j*2) )
                  {
                     pix = gan_image_get_pix_bgr_ui16 ( image, i*2, j*2 );
                     total.R += (gan_uint32)pix.R;
                     total.G += (gan_uint32)pix.G;
                     total.B += (gan_uint32)pix.B;
                     num++;
                  }
                    
                  if ( gan_image_get_pix_b(mask,i*2,j*2+1) )
                  {
                     pix = gan_image_get_pix_bgr_ui16 ( image, i*2, j*2+1 );
                     total.R += (gan_uint32)pix.R;
                     total.G += (gan_uint32)pix.G;
                     total.B += (gan_uint32)pix.B;
                     num++;
                  }

                  if ( gan_image_get_pix_b(mask,i*2+1,j*2) )
                  {
                     pix = gan_image_get_pix_bgr_ui16 ( image, i*2+1, j*2 );
                     total.R += (gan_uint32)pix.R;
                     total.G += (gan_uint32)pix.G;
                     total.B += (gan_uint32)pix.B;
                     num++;
                  }
                    
                  if ( gan_image_get_pix_b(mask,i*2+1,j*2+1) )
                  {
                     pix = gan_image_get_pix_bgr_ui16 ( image, i*2+1, j*2+1 );
                     total.R += (gan_uint32)pix.R;
                     total.G += (gan_uint32)pix.G;
                     total.B += (gan_uint32)pix.B;
                     num++;
                  }

                  if ( num >= thres )
                  {
                     pix.R = (gan_uint16) ((total.R + num/2)/num);
                     pix.G = (gan_uint16) ((total.G + num/2)/num);
                     pix.B = (gan_uint16) ((total.B + num/2)/num);
                     gan_image_set_pix_bgr_ui16 ( himage, i, j, &pix );
                     gan_image_set_pix_gl_ui16 ( hmask, i, j, num );
                  }
               }             
      }
      else
      {
         for ( i = (int)image->height/2-1; i >= 0; i-- )
            for ( j = (int)image->width/2-1; j >= 0; j-- )
            {
               gan_uint32 mval, num=0;
               Gan_BGRPixel_ui32 total;

               total.R = total.G = total.B = 0;

               mval = (gan_uint32) gan_image_get_pix_gl_ui16(mask,i*2,j*2);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_bgr_ui16 ( image, i*2, j*2 );
                  total.R += mval*(gan_uint32)pix.R;
                  total.G += mval*(gan_uint32)pix.G;
                  total.B += mval*(gan_uint32)pix.B;
                  num += mval;
               }
                    
               mval = (gan_uint32) gan_image_get_pix_gl_ui16(mask,i*2,j*2+1);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_bgr_ui16 ( image, i*2, j*2+1 );
                  total.R += mval*(gan_uint32)pix.R;
                  total.G += mval*(gan_uint32)pix.G;
                  total.B += mval*(gan_uint32)pix.B;
                  num += mval;
               }

               mval = (gan_uint32) gan_image_get_pix_gl_ui16(mask,i*2+1,j*2);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_bgr_ui16 ( image, i*2+1, j*2 );
                  total.R += mval*(gan_uint32)pix.R;
                  total.G += mval*(gan_uint32)pix.G;
                  total.B += mval*(gan_uint32)pix.B;
                  num += mval;
               }
                    
               mval = (gan_uint32) gan_image_get_pix_gl_ui16(mask,i*2+1,j*2+1);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_bgr_ui16 ( image, i*2+1, j*2+1 );
                  total.R += mval*(gan_uint32)pix.R;
                  total.G += mval*(gan_uint32)pix.G;
                  total.B += mval*(gan_uint32)pix.B;
                  num += mval;
               }

               if ( num >= thres )
               {
                  pix.R = (gan_uint16) ((total.R + num/2)/num);
                  pix.G = (gan_uint16) ((total.G + num/2)/num);
                  pix.B = (gan_uint16) ((total.B + num/2)/num);
                  gan_image_set_pix_bgr_ui16 ( himage, i, j, &pix );
                  gan_image_set_pix_gl_ui16 ( hmask, i, j, num );
               }             
            }
      }
   }
   else
   {
      unsigned num;
      Gan_BGRPixel_ui32 total;

      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
            if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
            {
               pix1 = gan_image_get_pix_bgr_ui16 ( image, i*2,   j*2  );
               pix2 = gan_image_get_pix_bgr_ui16 ( image, i*2,   j*2+1);
               pix3 = gan_image_get_pix_bgr_ui16 ( image, i*2+1, j*2  );
               pix4 = gan_image_get_pix_bgr_ui16 ( image, i*2+1, j*2+1);
               pix.R = (gan_uint16)(((gan_uint32) pix1.R + (gan_uint32) pix2.R + (gan_uint32) pix3.R + (gan_uint32) pix4.R + 2)/4);
               pix.G = (gan_uint16)(((gan_uint32) pix1.G + (gan_uint32) pix2.G + (gan_uint32) pix3.G + (gan_uint32) pix4.G + 2)/4);
               pix.B = (gan_uint16)(((gan_uint32) pix1.B + (gan_uint32) pix2.B + (gan_uint32) pix3.B + (gan_uint32) pix4.B + 2)/4);
               gan_image_set_pix_bgr_ui16 ( himage, i, j, &pix );
               gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
            }
            else
            {
               num = total.R = total.G = total.B = 0;
               if ( gan_image_get_pix_b(mask,i*2,j*2) )
               {
                  pix = gan_image_get_pix_bgr_ui16 ( image, i*2, j*2 );
                  total.R += (gan_uint32)pix.R;
                  total.G += (gan_uint32)pix.G;
                  total.B += (gan_uint32)pix.B;
                  num++;
               }
                    
               if ( gan_image_get_pix_b(mask,i*2,j*2+1) )
               {
                  pix = gan_image_get_pix_bgr_ui16 ( image, i*2, j*2+1 );
                  total.R += (gan_uint32)pix.R;
                  total.G += (gan_uint32)pix.G;
                  total.B += (gan_uint32)pix.B;
                  num++;
               }

               if ( gan_image_get_pix_b(mask,i*2+1,j*2) )
               {
                  pix = gan_image_get_pix_bgr_ui16 ( image, i*2+1, j*2 );
                  total.R += (gan_uint32)pix.R;
                  total.G += (gan_uint32)pix.G;
                  total.B += (gan_uint32)pix.B;
                  num++;
               }
                    
               if ( gan_image_get_pix_b(mask,i*2+1,j*2+1) )
               {
                  pix = gan_image_get_pix_bgr_ui16 ( image, i*2+1, j*2+1 );
                  total.R += (gan_uint32)pix.R;
                  total.G += (gan_uint32)pix.G;
                  total.B += (gan_uint32)pix.B;
                  num++;
               }

               if ( num >= no_neighbours )
               {
                  pix.R = (gan_uint16) ((total.R + 2)/4);
                  pix.G = (gan_uint16) ((total.G + 2)/4);
                  pix.B = (gan_uint16) ((total.B + 2)/4);
                  gan_image_set_pix_bgr_ui16 ( himage, i, j, &pix );
                  gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
               }
            }
   }

   /* success */
   return GAN_TRUE;
}

#ifdef GAN_UINT64
static Gan_Bool
 halve_size_rgb_ui32 ( Gan_Image *image, Gan_Image *mask, Gan_ImagePyramidAverage average_type,
                       unsigned no_neighbours, Gan_Image *himage, Gan_Image *hmask, gan_uint32 thres )
{
   int i, j;
   Gan_RGBPixel_ui32 pix1, pix2, pix3, pix4, pix;

   /* if mask is NULL, other parameters are irrelevant */
   if ( mask == NULL )
   {
      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
         {
            pix1 = gan_image_get_pix_rgb_ui32 ( image, i*2,   j*2   );
            pix2 = gan_image_get_pix_rgb_ui32 ( image, i*2,   j*2+1 );
            pix3 = gan_image_get_pix_rgb_ui32 ( image, i*2+1, j*2   );
            pix4 = gan_image_get_pix_rgb_ui32 ( image, i*2+1, j*2+1 );
            pix.R = (gan_uint32)(((gan_uint64) pix1.R + (gan_uint64) pix2.R + (gan_uint64) pix3.R + (gan_uint64) pix4.R + 2)/4);
            pix.G = (gan_uint32)(((gan_uint64) pix1.G + (gan_uint64) pix2.G + (gan_uint64) pix3.G + (gan_uint64) pix4.G + 2)/4);
            pix.B = (gan_uint32)(((gan_uint64) pix1.B + (gan_uint64) pix2.B + (gan_uint64) pix3.B + (gan_uint64) pix4.B + 2)/4);
            gan_image_set_pix_rgb_ui32 ( himage, i, j, &pix );
         }
   }
   /* mask != NULL, action depends on other parameters */
   else if ( no_neighbours == 4 )
   {
      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
            if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
            {
               pix1 = gan_image_get_pix_rgb_ui32 ( image, i*2,   j*2  );
               pix2 = gan_image_get_pix_rgb_ui32 ( image, i*2,   j*2+1);
               pix3 = gan_image_get_pix_rgb_ui32 ( image, i*2+1, j*2  );
               pix4 = gan_image_get_pix_rgb_ui32 ( image, i*2+1, j*2+1);
               pix.R = (gan_uint32)(((gan_uint64) pix1.R + (gan_uint64) pix2.R + (gan_uint64) pix3.R + (gan_uint64) pix4.R + 2)/4);
               pix.G = (gan_uint32)(((gan_uint64) pix1.G + (gan_uint64) pix2.G + (gan_uint64) pix3.G + (gan_uint64) pix4.G + 2)/4);
               pix.B = (gan_uint32)(((gan_uint64) pix1.B + (gan_uint64) pix2.B + (gan_uint64) pix3.B + (gan_uint64) pix4.B + 2)/4);
               gan_image_set_pix_rgb_ui32 ( himage, i, j, &pix );
               gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
            }
   }
   else if ( average_type == GAN_AVERAGE_IN_MASK )
   {
      if ( mask->type == GAN_BOOL )
      {
         for ( i = (int)image->height/2-1; i >= 0; i-- )
            for ( j = (int)image->width/2-1; j >= 0; j-- )
               if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
               {
                  pix1 = gan_image_get_pix_rgb_ui32 ( image, i*2,   j*2  );
                  pix2 = gan_image_get_pix_rgb_ui32 ( image, i*2,   j*2+1);
                  pix3 = gan_image_get_pix_rgb_ui32 ( image, i*2+1, j*2  );
                  pix4 = gan_image_get_pix_rgb_ui32 ( image, i*2+1, j*2+1);
                  pix.R = (gan_uint32)(((gan_uint64) pix1.R + (gan_uint64) pix2.R + (gan_uint64) pix3.R + (gan_uint64) pix4.R + 2)/4);
                  pix.G = (gan_uint32)(((gan_uint64) pix1.G + (gan_uint64) pix2.G + (gan_uint64) pix3.G + (gan_uint64) pix4.G + 2)/4);
                  pix.B = (gan_uint32)(((gan_uint64) pix1.B + (gan_uint64) pix2.B + (gan_uint64) pix3.B + (gan_uint64) pix4.B + 2)/4);
                  gan_image_set_pix_rgb_ui32 ( himage, i, j, &pix );
                  gan_image_set_pix_gl_ui16 ( hmask, i, j, 4 );
               }
               else
               {
                  gan_uint32 num=0;
                  Gan_RGBPixel_ui64 total;

                  total.R = total.G = total.B = 0;
                  if ( gan_image_get_pix_b(mask,i*2,j*2) )
                  {
                     pix = gan_image_get_pix_rgb_ui32 ( image, i*2, j*2 );
                     total.R += (gan_uint64)pix.R;
                     total.G += (gan_uint64)pix.G;
                     total.B += (gan_uint64)pix.B;
                     num++;
                  }
                    
                  if ( gan_image_get_pix_b(mask,i*2,j*2+1) )
                  {
                     pix = gan_image_get_pix_rgb_ui32 ( image, i*2, j*2+1 );
                     total.R += (gan_uint64)pix.R;
                     total.G += (gan_uint64)pix.G;
                     total.B += (gan_uint64)pix.B;
                     num++;
                  }

                  if ( gan_image_get_pix_b(mask,i*2+1,j*2) )
                  {
                     pix = gan_image_get_pix_rgb_ui32 ( image, i*2+1, j*2 );
                     total.R += (gan_uint64)pix.R;
                     total.G += (gan_uint64)pix.G;
                     total.B += (gan_uint64)pix.B;
                     num++;
                  }
                    
                  if ( gan_image_get_pix_b(mask,i*2+1,j*2+1) )
                  {
                     pix = gan_image_get_pix_rgb_ui32 ( image, i*2+1, j*2+1 );
                     total.R += (gan_uint64)pix.R;
                     total.G += (gan_uint64)pix.G;
                     total.B += (gan_uint64)pix.B;
                     num++;
                  }

                  if ( num >= thres )
                  {
                     pix.R = (gan_uint32) ((total.R + num/2)/num);
                     pix.G = (gan_uint32) ((total.G + num/2)/num);
                     pix.B = (gan_uint32) ((total.B + num/2)/num);
                     gan_image_set_pix_rgb_ui32 ( himage, i, j, &pix );
                     gan_image_set_pix_gl_ui16 ( hmask, i, j, num );
                  }
               }
      }
      else
      {
         for ( i = (int)image->height/2-1; i >= 0; i-- )
            for ( j = (int)image->width/2-1; j >= 0; j-- )
            {
               gan_uint64 mval, num=0;
               Gan_RGBPixel_ui64 total;

               total.R = total.G = total.B = 0;

               mval = (gan_uint64) gan_image_get_pix_gl_ui16(mask,i*2,j*2);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_rgb_ui32 ( image, i*2, j*2 );
                  total.R += mval*(gan_uint64)pix.R;
                  total.G += mval*(gan_uint64)pix.G;
                  total.B += mval*(gan_uint64)pix.B;
                  num += mval;
               }
                    
               mval = (gan_uint64) gan_image_get_pix_gl_ui16(mask,i*2,j*2+1);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_rgb_ui32 ( image, i*2, j*2+1 );
                  total.R += mval*(gan_uint64)pix.R;
                  total.G += mval*(gan_uint64)pix.G;
                  total.B += mval*(gan_uint64)pix.B;
                  num += mval;
               }

               mval = (gan_uint64) gan_image_get_pix_gl_ui16(mask,i*2+1,j*2);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_rgb_ui32 ( image, i*2+1, j*2 );
                  total.R += mval*(gan_uint64)pix.R;
                  total.G += mval*(gan_uint64)pix.G;
                  total.B += mval*(gan_uint64)pix.B;
                  num += mval;
               }
                    
               mval = (gan_uint64) gan_image_get_pix_gl_ui16(mask,i*2+1,j*2+1);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_rgb_ui32 ( image, i*2+1, j*2+1 );
                  total.R += mval*(gan_uint64)pix.R;
                  total.G += mval*(gan_uint64)pix.G;
                  total.B += mval*(gan_uint64)pix.B;
                  num += mval;
               }

               if ( num >= thres )
               {
                  pix.R = (gan_uint32) ((total.R + num/2)/num);
                  pix.G = (gan_uint32) ((total.G + num/2)/num);
                  pix.B = (gan_uint32) ((total.B + num/2)/num);
                  gan_image_set_pix_rgb_ui32 ( himage, i, j, &pix );
                  gan_image_set_pix_gl_ui16 ( hmask, i, j, num );
               }             
            }
      }
   }
   else
   {
      gan_uint64 num;
      Gan_RGBPixel_ui32 total;

      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
            if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
            {
               pix1 = gan_image_get_pix_rgb_ui32 ( image, i*2,   j*2  );
               pix2 = gan_image_get_pix_rgb_ui32 ( image, i*2,   j*2+1);
               pix3 = gan_image_get_pix_rgb_ui32 ( image, i*2+1, j*2  );
               pix4 = gan_image_get_pix_rgb_ui32 ( image, i*2+1, j*2+1);
               pix.R = (gan_uint32)(((gan_uint64) pix1.R + (gan_uint64) pix2.R +
                                     (gan_uint64) pix3.R + (gan_uint64) pix4.R + 2)/4);
               pix.G = (gan_uint32)(((gan_uint64) pix1.G + (gan_uint64) pix2.G +
                                     (gan_uint64) pix3.G + (gan_uint64) pix4.G + 2)/4);
               pix.B = (gan_uint32)(((gan_uint64) pix1.B + (gan_uint64) pix2.B +
                                     (gan_uint64) pix3.B + (gan_uint64) pix4.B + 2)/4);
               gan_image_set_pix_rgb_ui32 ( himage, i, j, &pix );
               gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
            }
            else
            {
               num = total.R = total.G = total.B = 0;
               if ( gan_image_get_pix_b(mask,i*2,j*2) )
               {
                  pix = gan_image_get_pix_rgb_ui32 ( image, i*2, j*2 );
                  total.R += (gan_uint64)pix.R;
                  total.G += (gan_uint64)pix.G;
                  total.B += (gan_uint64)pix.B;
                  num++;
               }
                    
               if ( gan_image_get_pix_b(mask,i*2,j*2+1) )
               {
                  pix = gan_image_get_pix_rgb_ui32 ( image, i*2, j*2+1 );
                  total.R += (gan_uint64)pix.R;
                  total.G += (gan_uint64)pix.G;
                  total.B += (gan_uint64)pix.B;
                  num++;
               }

               if ( gan_image_get_pix_b(mask,i*2+1,j*2) )
               {
                  pix = gan_image_get_pix_rgb_ui32 ( image, i*2+1, j*2 );
                  total.R += (gan_uint64)pix.R;
                  total.G += (gan_uint64)pix.G;
                  total.B += (gan_uint64)pix.B;
                  num++;
               }
                    
               if ( gan_image_get_pix_b(mask,i*2+1,j*2+1) )
               {
                  pix = gan_image_get_pix_rgb_ui32 ( image, i*2+1, j*2+1 );
                  total.R += (gan_uint64)pix.R;
                  total.G += (gan_uint64)pix.G;
                  total.B += (gan_uint64)pix.B;
                  num++;
               }

               if ( num >= no_neighbours )
               {
                  pix.R = (gan_uint32) ((total.R + 2)/4);
                  pix.G = (gan_uint32) ((total.G + 2)/4);
                  pix.B = (gan_uint32) ((total.B + 2)/4);
                  gan_image_set_pix_rgb_ui32 ( himage, i, j, &pix );
                  gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
               }
            }
   }

   /* success */
   return GAN_TRUE;
}

static Gan_Bool
 halve_size_bgr_ui32 ( Gan_Image *image, Gan_Image *mask, Gan_ImagePyramidAverage average_type,
                       unsigned no_neighbours, Gan_Image *himage, Gan_Image *hmask, gan_uint32 thres )
{
   int i, j;
   Gan_BGRPixel_ui32 pix1, pix2, pix3, pix4, pix;

   /* if mask is NULL, other parameters are irrelevant */
   if ( mask == NULL )
   {
      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
         {
            pix1 = gan_image_get_pix_bgr_ui32 ( image, i*2,   j*2   );
            pix2 = gan_image_get_pix_bgr_ui32 ( image, i*2,   j*2+1 );
            pix3 = gan_image_get_pix_bgr_ui32 ( image, i*2+1, j*2   );
            pix4 = gan_image_get_pix_bgr_ui32 ( image, i*2+1, j*2+1 );
            pix.R = (gan_uint32)(((gan_uint64) pix1.R + (gan_uint64) pix2.R + (gan_uint64) pix3.R + (gan_uint64) pix4.R + 2)/4);
            pix.G = (gan_uint32)(((gan_uint64) pix1.G + (gan_uint64) pix2.G + (gan_uint64) pix3.G + (gan_uint64) pix4.G + 2)/4);
            pix.B = (gan_uint32)(((gan_uint64) pix1.B + (gan_uint64) pix2.B + (gan_uint64) pix3.B + (gan_uint64) pix4.B + 2)/4);
            gan_image_set_pix_bgr_ui32 ( himage, i, j, &pix );
         }
   }
   /* mask != NULL, action depends on other parameters */
   else if ( no_neighbours == 4 )
   {
      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
            if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
            {
               pix1 = gan_image_get_pix_bgr_ui32 ( image, i*2,   j*2  );
               pix2 = gan_image_get_pix_bgr_ui32 ( image, i*2,   j*2+1);
               pix3 = gan_image_get_pix_bgr_ui32 ( image, i*2+1, j*2  );
               pix4 = gan_image_get_pix_bgr_ui32 ( image, i*2+1, j*2+1);
               pix.R = (gan_uint32)(((gan_uint64) pix1.R + (gan_uint64) pix2.R + (gan_uint64) pix3.R + (gan_uint64) pix4.R + 2)/4);
               pix.G = (gan_uint32)(((gan_uint64) pix1.G + (gan_uint64) pix2.G + (gan_uint64) pix3.G + (gan_uint64) pix4.G + 2)/4);
               pix.B = (gan_uint32)(((gan_uint64) pix1.B + (gan_uint64) pix2.B + (gan_uint64) pix3.B + (gan_uint64) pix4.B + 2)/4);
               gan_image_set_pix_bgr_ui32 ( himage, i, j, &pix );
               gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
            }
   }
   else if ( average_type == GAN_AVERAGE_IN_MASK )
   {
      if ( mask->type == GAN_BOOL )
      {
         for ( i = (int)image->height/2-1; i >= 0; i-- )
            for ( j = (int)image->width/2-1; j >= 0; j-- )
               if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
               {
                  pix1 = gan_image_get_pix_bgr_ui32 ( image, i*2,   j*2  );
                  pix2 = gan_image_get_pix_bgr_ui32 ( image, i*2,   j*2+1);
                  pix3 = gan_image_get_pix_bgr_ui32 ( image, i*2+1, j*2  );
                  pix4 = gan_image_get_pix_bgr_ui32 ( image, i*2+1, j*2+1);
                  pix.R = (gan_uint32)(((gan_uint64) pix1.R + (gan_uint64) pix2.R + (gan_uint64) pix3.R + (gan_uint64) pix4.R + 2)/4);
                  pix.G = (gan_uint32)(((gan_uint64) pix1.G + (gan_uint64) pix2.G + (gan_uint64) pix3.G + (gan_uint64) pix4.G + 2)/4);
                  pix.B = (gan_uint32)(((gan_uint64) pix1.B + (gan_uint64) pix2.B + (gan_uint64) pix3.B + (gan_uint64) pix4.B + 2)/4);
                  gan_image_set_pix_bgr_ui32 ( himage, i, j, &pix );
                  gan_image_set_pix_gl_ui16 ( hmask, i, j, 4 );
               }
               else
               {
                  gan_uint32 num=0;
                  Gan_BGRPixel_ui64 total;

                  total.R = total.G = total.B = 0;
                  if ( gan_image_get_pix_b(mask,i*2,j*2) )
                  {
                     pix = gan_image_get_pix_bgr_ui32 ( image, i*2, j*2 );
                     total.R += (gan_uint64)pix.R;
                     total.G += (gan_uint64)pix.G;
                     total.B += (gan_uint64)pix.B;
                     num++;
                  }
                    
                  if ( gan_image_get_pix_b(mask,i*2,j*2+1) )
                  {
                     pix = gan_image_get_pix_bgr_ui32 ( image, i*2, j*2+1 );
                     total.R += (gan_uint64)pix.R;
                     total.G += (gan_uint64)pix.G;
                     total.B += (gan_uint64)pix.B;
                     num++;
                  }

                  if ( gan_image_get_pix_b(mask,i*2+1,j*2) )
                  {
                     pix = gan_image_get_pix_bgr_ui32 ( image, i*2+1, j*2 );
                     total.R += (gan_uint64)pix.R;
                     total.G += (gan_uint64)pix.G;
                     total.B += (gan_uint64)pix.B;
                     num++;
                  }
                    
                  if ( gan_image_get_pix_b(mask,i*2+1,j*2+1) )
                  {
                     pix = gan_image_get_pix_bgr_ui32 ( image, i*2+1, j*2+1 );
                     total.R += (gan_uint64)pix.R;
                     total.G += (gan_uint64)pix.G;
                     total.B += (gan_uint64)pix.B;
                     num++;
                  }

                  if ( num >= thres )
                  {
                     pix.R = (gan_uint32) ((total.R + num/2)/num);
                     pix.G = (gan_uint32) ((total.G + num/2)/num);
                     pix.B = (gan_uint32) ((total.B + num/2)/num);
                     gan_image_set_pix_bgr_ui32 ( himage, i, j, &pix );
                     gan_image_set_pix_gl_ui16 ( hmask, i, j, num );
                  }
               }
      }
      else
      {
         for ( i = (int)image->height/2-1; i >= 0; i-- )
            for ( j = (int)image->width/2-1; j >= 0; j-- )
            {
               gan_uint64 mval, num=0;
               Gan_BGRPixel_ui64 total;

               total.R = total.G = total.B = 0;

               mval = (gan_uint64) gan_image_get_pix_gl_ui16(mask,i*2,j*2);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_bgr_ui32 ( image, i*2, j*2 );
                  total.R += mval*(gan_uint64)pix.R;
                  total.G += mval*(gan_uint64)pix.G;
                  total.B += mval*(gan_uint64)pix.B;
                  num += mval;
               }
                    
               mval = (gan_uint64) gan_image_get_pix_gl_ui16(mask,i*2,j*2+1);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_bgr_ui32 ( image, i*2, j*2+1 );
                  total.R += mval*(gan_uint64)pix.R;
                  total.G += mval*(gan_uint64)pix.G;
                  total.B += mval*(gan_uint64)pix.B;
                  num += mval;
               }

               mval = (gan_uint64) gan_image_get_pix_gl_ui16(mask,i*2+1,j*2);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_bgr_ui32 ( image, i*2+1, j*2 );
                  total.R += mval*(gan_uint64)pix.R;
                  total.G += mval*(gan_uint64)pix.G;
                  total.B += mval*(gan_uint64)pix.B;
                  num += mval;
               }
                    
               mval = (gan_uint64) gan_image_get_pix_gl_ui16(mask,i*2+1,j*2+1);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_bgr_ui32 ( image, i*2+1, j*2+1 );
                  total.R += mval*(gan_uint64)pix.R;
                  total.G += mval*(gan_uint64)pix.G;
                  total.B += mval*(gan_uint64)pix.B;
                  num += mval;
               }

               if ( num >= thres )
               {
                  pix.R = (gan_uint32) ((total.R + num/2)/num);
                  pix.G = (gan_uint32) ((total.G + num/2)/num);
                  pix.B = (gan_uint32) ((total.B + num/2)/num);
                  gan_image_set_pix_bgr_ui32 ( himage, i, j, &pix );
                  gan_image_set_pix_gl_ui16 ( hmask, i, j, num );
               }             
            }
      }
   }
   else
   {
      gan_uint64 num;
      Gan_BGRPixel_ui32 total;

      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
            if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
            {
               pix1 = gan_image_get_pix_bgr_ui32 ( image, i*2,   j*2  );
               pix2 = gan_image_get_pix_bgr_ui32 ( image, i*2,   j*2+1);
               pix3 = gan_image_get_pix_bgr_ui32 ( image, i*2+1, j*2  );
               pix4 = gan_image_get_pix_bgr_ui32 ( image, i*2+1, j*2+1);
               pix.R = (gan_uint32)(((gan_uint64) pix1.R + (gan_uint64) pix2.R +
                                     (gan_uint64) pix3.R + (gan_uint64) pix4.R + 2)/4);
               pix.G = (gan_uint32)(((gan_uint64) pix1.G + (gan_uint64) pix2.G +
                                     (gan_uint64) pix3.G + (gan_uint64) pix4.G + 2)/4);
               pix.B = (gan_uint32)(((gan_uint64) pix1.B + (gan_uint64) pix2.B +
                                     (gan_uint64) pix3.B + (gan_uint64) pix4.B + 2)/4);
               gan_image_set_pix_bgr_ui32 ( himage, i, j, &pix );
               gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
            }
            else
            {
               num = total.R = total.G = total.B = 0;
               if ( gan_image_get_pix_b(mask,i*2,j*2) )
               {
                  pix = gan_image_get_pix_bgr_ui32 ( image, i*2, j*2 );
                  total.R += (gan_uint64)pix.R;
                  total.G += (gan_uint64)pix.G;
                  total.B += (gan_uint64)pix.B;
                  num++;
               }
                    
               if ( gan_image_get_pix_b(mask,i*2,j*2+1) )
               {
                  pix = gan_image_get_pix_bgr_ui32 ( image, i*2, j*2+1 );
                  total.R += (gan_uint64)pix.R;
                  total.G += (gan_uint64)pix.G;
                  total.B += (gan_uint64)pix.B;
                  num++;
               }

               if ( gan_image_get_pix_b(mask,i*2+1,j*2) )
               {
                  pix = gan_image_get_pix_bgr_ui32 ( image, i*2+1, j*2 );
                  total.R += (gan_uint64)pix.R;
                  total.G += (gan_uint64)pix.G;
                  total.B += (gan_uint64)pix.B;
                  num++;
               }
                    
               if ( gan_image_get_pix_b(mask,i*2+1,j*2+1) )
               {
                  pix = gan_image_get_pix_bgr_ui32 ( image, i*2+1, j*2+1 );
                  total.R += (gan_uint64)pix.R;
                  total.G += (gan_uint64)pix.G;
                  total.B += (gan_uint64)pix.B;
                  num++;
               }

               if ( num >= no_neighbours )
               {
                  pix.R = (gan_uint32) ((total.R + 2)/4);
                  pix.G = (gan_uint32) ((total.G + 2)/4);
                  pix.B = (gan_uint32) ((total.B + 2)/4);
                  gan_image_set_pix_bgr_ui32 ( himage, i, j, &pix );
                  gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
               }
            }
   }

   /* success */
   return GAN_TRUE;
}
#else
/* we don't have a 64-bit integer type, so we lose a bit of precision by dividing by four before averaging */
static Gan_Bool
 halve_size_rgb_ui32 ( Gan_Image *image, Gan_Image *mask, Gan_ImagePyramidAverage average_type,
                     unsigned no_neighbours, Gan_Image *himage, Gan_Image *hmask, gan_uint32 thres )
{
   int i, j;
   Gan_RGBPixel_ui32 pix1, pix2, pix3, pix4, pix;

   /* if mask is NULL, other parameters are irrelevant */
   if ( mask == NULL )
   {
      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
         {
            pix1 = gan_image_get_pix_rgb_ui32 ( image, i*2,   j*2   );
            pix2 = gan_image_get_pix_rgb_ui32 ( image, i*2,   j*2+1 );
            pix3 = gan_image_get_pix_rgb_ui32 ( image, i*2+1, j*2   );
            pix4 = gan_image_get_pix_rgb_ui32 ( image, i*2+1, j*2+1 );
            pix.R = pix1.R/4 + pix2.R/4 + pix3.R/4 + pix4.R/4;
            pix.G = pix1.G/4 + pix2.G/4 + pix3.G/4 + pix4.G/4;
            pix.B = pix1.B/4 + pix2.B/4 + pix3.B/4 + pix4.B/4;
            gan_image_set_pix_rgb_ui32 ( himage, i, j, &pix );
         }
   }
   /* mask != NULL, action depends on other parameters */
   else if ( no_neighbours == 4 )
   {
      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
            if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
            {
               pix1 = gan_image_get_pix_rgb_ui32 ( image, i*2,   j*2   );
               pix2 = gan_image_get_pix_rgb_ui32 ( image, i*2,   j*2+1 );
               pix3 = gan_image_get_pix_rgb_ui32 ( image, i*2+1, j*2   );
               pix4 = gan_image_get_pix_rgb_ui32 ( image, i*2+1, j*2+1 );
               pix.R = pix1.R/4 + pix2.R/4 + pix3.R/4 + pix4.R/4;
               pix.G = pix1.G/4 + pix2.G/4 + pix3.G/4 + pix4.G/4;
               pix.B = pix1.B/4 + pix2.B/4 + pix3.B/4 + pix4.B/4;
               gan_image_set_pix_rgb_ui32 ( himage, i, j, &pix );
               gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
            }
   }
   else if ( average_type == GAN_AVERAGE_IN_MASK )
   {
      if ( mask->type == GAN_BOOL )
      {
         for ( i = (int)image->height/2-1; i >= 0; i-- )
            for ( j = (int)image->width/2-1; j >= 0; j-- )
               if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
               {
                  pix1 = gan_image_get_pix_rgb_ui32 ( image, i*2,   j*2  );
                  pix2 = gan_image_get_pix_rgb_ui32 ( image, i*2,   j*2+1);
                  pix3 = gan_image_get_pix_rgb_ui32 ( image, i*2+1, j*2  );
                  pix4 = gan_image_get_pix_rgb_ui32 ( image, i*2+1, j*2+1);
                  pix.R = pix1.R/4 + pix2.R/4 + pix3.R/4 + pix4.R/4;
                  pix.G = pix1.G/4 + pix2.G/4 + pix3.G/4 + pix4.G/4;
                  pix.B = pix1.B/4 + pix2.B/4 + pix3.B/4 + pix4.B/4;
                  gan_image_set_pix_rgb_ui32 ( himage, i, j, &pix );
                  gan_image_set_pix_gl_ui16 ( hmask, i, j, 4 );
               }
               else
               {
                  gan_uint32 num=0;
                  Gan_RGBPixel_ui32 total;

                  total.R = total.G = total.B = 0;
                  if ( gan_image_get_pix_b(mask,i*2,j*2) )
                  {
                     pix = gan_image_get_pix_rgb_ui32 ( image, i*2, j*2 );
                     total.R += pix.R/4;
                     total.G += pix.G/4;
                     total.B += pix.B/4;
                     num++;
                  }
                    
                  if ( gan_image_get_pix_b(mask,i*2,j*2+1) )
                  {
                     pix = gan_image_get_pix_rgb_ui32 ( image, i*2, j*2+1 );
                     total.R += pix.R/4;
                     total.G += pix.G/4;
                     total.B += pix.B/4;
                     num++;
                  }

                  if ( gan_image_get_pix_b(mask,i*2+1,j*2) )
                  {
                     pix = gan_image_get_pix_rgb_ui32 ( image, i*2+1, j*2 );
                     total.R += pix.R/4;
                     total.G += pix.G/4;
                     total.B += pix.B/4;
                     num++;
                  }
                    
                  if ( gan_image_get_pix_b(mask,i*2+1,j*2+1) )
                  {
                     pix = gan_image_get_pix_rgb_ui32 ( image, i*2+1, j*2+1 );
                     total.R += pix.R/4;
                     total.G += pix.G/4;
                     total.B += pix.B/4;
                     num++;
                  }

                  if ( num >= thres )
                  {
                     pix.R = ((total.R)/num)*4;
                     pix.G = ((total.G)/num)*4;
                     pix.B = ((total.B)/num)*4;
                     gan_image_set_pix_rgb_ui32 ( himage, i, j, &pix );
                     gan_image_set_pix_gl_ui16 ( hmask, i, j, num );
                  }
               }
      }
      else
      {
         for ( i = (int)image->height/2-1; i >= 0; i-- )
            for ( j = (int)image->width/2-1; j >= 0; j-- )
            {
               gan_uint32 mval, num=0;
               Gan_RGBPixel_ui32 total;

               total.R = total.G = total.B = 0;

               mval = (gan_uint32) gan_image_get_pix_gl_ui16(mask,i*2,j*2);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_rgb_ui32 ( image, i*2, j*2 );
                  total.R += mval*(pix.R >> 18);
                  total.G += mval*(pix.G >> 18);
                  total.B += mval*(pix.B >> 18);
                  num += mval;
               }
                    
               mval = (gan_uint32) gan_image_get_pix_gl_ui16(mask,i*2,j*2+1);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_rgb_ui32 ( image, i*2, j*2+1 );
                  total.R += mval*(pix.R >> 18);
                  total.G += mval*(pix.G >> 18);
                  total.B += mval*(pix.B >> 18);
                  num += mval;
               }

               mval = (gan_uint32) gan_image_get_pix_gl_ui16(mask,i*2+1,j*2);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_rgb_ui32 ( image, i*2+1, j*2 );
                  total.R += mval*(pix.R >> 18);
                  total.G += mval*(pix.G >> 18);
                  total.B += mval*(pix.B >> 18);
                  num += mval;
               }
                    
               mval = (gan_uint32) gan_image_get_pix_gl_ui16(mask,i*2+1,j*2+1);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_rgb_ui32 ( image, i*2+1, j*2+1 );
                  total.R += mval*(pix.R >> 18);
                  total.G += mval*(pix.G >> 18);
                  total.B += mval*(pix.B >> 18);
                  num += mval;
               }

               if ( num >= thres )
               {
                  pix.R = (total.R/num) << 18;
                  pix.G = (total.G/num) << 18;
                  pix.B = (total.B/num) << 18;
                  gan_image_set_pix_rgb_ui32 ( himage, i, j, &pix );
                  gan_image_set_pix_gl_ui16 ( hmask, i, j, num );
               }             
            }
      }
   }
   else
   {
      unsigned num;

      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
            if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
            {
               pix1 = gan_image_get_pix_rgb_ui32 ( image, i*2,   j*2   );
               pix2 = gan_image_get_pix_rgb_ui32 ( image, i*2,   j*2+1 );
               pix3 = gan_image_get_pix_rgb_ui32 ( image, i*2+1, j*2   );
               pix4 = gan_image_get_pix_rgb_ui32 ( image, i*2+1, j*2+1 );
               pix.R = pix1.R/4 + pix2.R/4 + pix3.R/4 + pix4.R/4;
               pix.G = pix1.G/4 + pix2.G/4 + pix3.G/4 + pix4.G/4;
               pix.B = pix1.B/4 + pix2.B/4 + pix3.B/4 + pix4.B/4;
               gan_image_set_pix_rgb_ui32 ( himage, i, j, &pix );
               gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
            }
            else
            {
               num = pix.R = pix.G = pix.B = 0;
               if ( gan_image_get_pix_b(mask,i*2,j*2) )
               {
                  pix = gan_image_get_pix_rgb_ui32 ( image, i*2, j*2 );
                  pix.R += pix.R/4;
                  pix.G += pix.G/4;
                  pix.B += pix.B/4;
                  num++;
               }
                    
               if ( gan_image_get_pix_b(mask,i*2,j*2+1) )
               {
                  pix = gan_image_get_pix_rgb_ui32 ( image, i*2, j*2+1 );
                  pix.R += pix.R/4;
                  pix.G += pix.G/4;
                  pix.B += pix.B/4;
                  num++;
               }

               if ( gan_image_get_pix_b(mask,i*2+1,j*2) )
               {
                  pix = gan_image_get_pix_rgb_ui32 ( image, i*2+1, j*2 );
                  pix.R += pix.R/4;
                  pix.G += pix.G/4;
                  pix.B += pix.B/4;
                  num++;
               }
                    
               if ( gan_image_get_pix_b(mask,i*2+1,j*2+1) )
               {
                  pix = gan_image_get_pix_rgb_ui32 ( image, i*2+1, j*2+1 );
                  pix.R += pix.R/4;
                  pix.G += pix.G/4;
                  pix.B += pix.B/4;
                  num++;
               }

               if ( num >= no_neighbours )
               {
                  gan_image_set_pix_rgb_ui32 ( himage, i, j, &pix );
                  gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
               }
            }
   }

   /* success */
   return GAN_TRUE;
}

/* we don't have a 64-bit integer type, so we lose a bit of precision by dividing by four before averaging */
static Gan_Bool
 halve_size_bgr_ui32 ( Gan_Image *image, Gan_Image *mask, Gan_ImagePyramidAverage average_type,
                     unsigned no_neighbours, Gan_Image *himage, Gan_Image *hmask, gan_uint32 thres )
{
   int i, j;
   Gan_BGRPixel_ui32 pix1, pix2, pix3, pix4, pix;

   /* if mask is NULL, other parameters are irrelevant */
   if ( mask == NULL )
   {
      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
         {
            pix1 = gan_image_get_pix_bgr_ui32 ( image, i*2,   j*2   );
            pix2 = gan_image_get_pix_bgr_ui32 ( image, i*2,   j*2+1 );
            pix3 = gan_image_get_pix_bgr_ui32 ( image, i*2+1, j*2   );
            pix4 = gan_image_get_pix_bgr_ui32 ( image, i*2+1, j*2+1 );
            pix.R = pix1.R/4 + pix2.R/4 + pix3.R/4 + pix4.R/4;
            pix.G = pix1.G/4 + pix2.G/4 + pix3.G/4 + pix4.G/4;
            pix.B = pix1.B/4 + pix2.B/4 + pix3.B/4 + pix4.B/4;
            gan_image_set_pix_bgr_ui32 ( himage, i, j, &pix );
         }
   }
   /* mask != NULL, action depends on other parameters */
   else if ( no_neighbours == 4 )
   {
      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
            if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
            {
               pix1 = gan_image_get_pix_bgr_ui32 ( image, i*2,   j*2   );
               pix2 = gan_image_get_pix_bgr_ui32 ( image, i*2,   j*2+1 );
               pix3 = gan_image_get_pix_bgr_ui32 ( image, i*2+1, j*2   );
               pix4 = gan_image_get_pix_bgr_ui32 ( image, i*2+1, j*2+1 );
               pix.R = pix1.R/4 + pix2.R/4 + pix3.R/4 + pix4.R/4;
               pix.G = pix1.G/4 + pix2.G/4 + pix3.G/4 + pix4.G/4;
               pix.B = pix1.B/4 + pix2.B/4 + pix3.B/4 + pix4.B/4;
               gan_image_set_pix_bgr_ui32 ( himage, i, j, &pix );
               gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
            }
   }
   else if ( average_type == GAN_AVERAGE_IN_MASK )
   {
      if ( mask->type == GAN_BOOL )
      {
         for ( i = (int)image->height/2-1; i >= 0; i-- )
            for ( j = (int)image->width/2-1; j >= 0; j-- )
               if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
               {
                  pix1 = gan_image_get_pix_bgr_ui32 ( image, i*2,   j*2  );
                  pix2 = gan_image_get_pix_bgr_ui32 ( image, i*2,   j*2+1);
                  pix3 = gan_image_get_pix_bgr_ui32 ( image, i*2+1, j*2  );
                  pix4 = gan_image_get_pix_bgr_ui32 ( image, i*2+1, j*2+1);
                  pix.R = pix1.R/4 + pix2.R/4 + pix3.R/4 + pix4.R/4;
                  pix.G = pix1.G/4 + pix2.G/4 + pix3.G/4 + pix4.G/4;
                  pix.B = pix1.B/4 + pix2.B/4 + pix3.B/4 + pix4.B/4;
                  gan_image_set_pix_bgr_ui32 ( himage, i, j, &pix );
                  gan_image_set_pix_gl_ui16 ( hmask, i, j, 4 );
               }
               else
               {
                  gan_uint32 num=0;
                  Gan_BGRPixel_ui32 total;

                  total.R = total.G = total.B = 0;
                  if ( gan_image_get_pix_b(mask,i*2,j*2) )
                  {
                     pix = gan_image_get_pix_bgr_ui32 ( image, i*2, j*2 );
                     total.R += pix.R/4;
                     total.G += pix.G/4;
                     total.B += pix.B/4;
                     num++;
                  }
                    
                  if ( gan_image_get_pix_b(mask,i*2,j*2+1) )
                  {
                     pix = gan_image_get_pix_bgr_ui32 ( image, i*2, j*2+1 );
                     total.R += pix.R/4;
                     total.G += pix.G/4;
                     total.B += pix.B/4;
                     num++;
                  }

                  if ( gan_image_get_pix_b(mask,i*2+1,j*2) )
                  {
                     pix = gan_image_get_pix_bgr_ui32 ( image, i*2+1, j*2 );
                     total.R += pix.R/4;
                     total.G += pix.G/4;
                     total.B += pix.B/4;
                     num++;
                  }
                    
                  if ( gan_image_get_pix_b(mask,i*2+1,j*2+1) )
                  {
                     pix = gan_image_get_pix_bgr_ui32 ( image, i*2+1, j*2+1 );
                     total.R += pix.R/4;
                     total.G += pix.G/4;
                     total.B += pix.B/4;
                     num++;
                  }

                  if ( num >= thres )
                  {
                     pix.R = ((total.R)/num)*4;
                     pix.G = ((total.G)/num)*4;
                     pix.B = ((total.B)/num)*4;
                     gan_image_set_pix_bgr_ui32 ( himage, i, j, &pix );
                     gan_image_set_pix_gl_ui16 ( hmask, i, j, num );
                  }
               }
      }
      else
      {
         for ( i = (int)image->height/2-1; i >= 0; i-- )
            for ( j = (int)image->width/2-1; j >= 0; j-- )
            {
               gan_uint32 mval, num=0;
               Gan_BGRPixel_ui32 total;

               total.R = total.G = total.B = 0;

               mval = (gan_uint32) gan_image_get_pix_gl_ui16(mask,i*2,j*2);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_bgr_ui32 ( image, i*2, j*2 );
                  total.R += mval*(pix.R >> 18);
                  total.G += mval*(pix.G >> 18);
                  total.B += mval*(pix.B >> 18);
                  num += mval;
               }
                    
               mval = (gan_uint32) gan_image_get_pix_gl_ui16(mask,i*2,j*2+1);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_bgr_ui32 ( image, i*2, j*2+1 );
                  total.R += mval*(pix.R >> 18);
                  total.G += mval*(pix.G >> 18);
                  total.B += mval*(pix.B >> 18);
                  num += mval;
               }

               mval = (gan_uint32) gan_image_get_pix_gl_ui16(mask,i*2+1,j*2);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_bgr_ui32 ( image, i*2+1, j*2 );
                  total.R += mval*(pix.R >> 18);
                  total.G += mval*(pix.G >> 18);
                  total.B += mval*(pix.B >> 18);
                  num += mval;
               }
                    
               mval = (gan_uint32) gan_image_get_pix_gl_ui16(mask,i*2+1,j*2+1);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_bgr_ui32 ( image, i*2+1, j*2+1 );
                  total.R += mval*(pix.R >> 18);
                  total.G += mval*(pix.G >> 18);
                  total.B += mval*(pix.B >> 18);
                  num += mval;
               }

               if ( num >= thres )
               {
                  pix.R = (total.R/num) << 18;
                  pix.G = (total.G/num) << 18;
                  pix.B = (total.B/num) << 18;
                  gan_image_set_pix_bgr_ui32 ( himage, i, j, &pix );
                  gan_image_set_pix_gl_ui16 ( hmask, i, j, num );
               }             
            }
      }
   }
   else
   {
      unsigned num;

      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
            if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
            {
               pix1 = gan_image_get_pix_bgr_ui32 ( image, i*2,   j*2   );
               pix2 = gan_image_get_pix_bgr_ui32 ( image, i*2,   j*2+1 );
               pix3 = gan_image_get_pix_bgr_ui32 ( image, i*2+1, j*2   );
               pix4 = gan_image_get_pix_bgr_ui32 ( image, i*2+1, j*2+1 );
               pix.R = pix1.R/4 + pix2.R/4 + pix3.R/4 + pix4.R/4;
               pix.G = pix1.G/4 + pix2.G/4 + pix3.G/4 + pix4.G/4;
               pix.B = pix1.B/4 + pix2.B/4 + pix3.B/4 + pix4.B/4;
               gan_image_set_pix_bgr_ui32 ( himage, i, j, &pix );
               gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
            }
            else
            {
               num = pix.R = pix.G = pix.B = 0;
               if ( gan_image_get_pix_b(mask,i*2,j*2) )
               {
                  pix = gan_image_get_pix_bgr_ui32 ( image, i*2, j*2 );
                  pix.R += pix.R/4;
                  pix.G += pix.G/4;
                  pix.B += pix.B/4;
                  num++;
               }
                    
               if ( gan_image_get_pix_b(mask,i*2,j*2+1) )
               {
                  pix = gan_image_get_pix_bgr_ui32 ( image, i*2, j*2+1 );
                  pix.R += pix.R/4;
                  pix.G += pix.G/4;
                  pix.B += pix.B/4;
                  num++;
               }

               if ( gan_image_get_pix_b(mask,i*2+1,j*2) )
               {
                  pix = gan_image_get_pix_bgr_ui32 ( image, i*2+1, j*2 );
                  pix.R += pix.R/4;
                  pix.G += pix.G/4;
                  pix.B += pix.B/4;
                  num++;
               }
                    
               if ( gan_image_get_pix_b(mask,i*2+1,j*2+1) )
               {
                  pix = gan_image_get_pix_bgr_ui32 ( image, i*2+1, j*2+1 );
                  pix.R += pix.R/4;
                  pix.G += pix.G/4;
                  pix.B += pix.B/4;
                  num++;
               }

               if ( num >= no_neighbours )
               {
                  gan_image_set_pix_bgr_ui32 ( himage, i, j, &pix );
                  gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
               }
            }
   }

   /* success */
   return GAN_TRUE;
}
#endif /* #ifdef GAN_UINT64 */

static Gan_Bool
 halve_size_rgb_f32 ( Gan_Image *image, Gan_Image *mask, Gan_ImagePyramidAverage average_type,
                      unsigned no_neighbours, Gan_Image *himage, Gan_Image *hmask, gan_uint32 thres )
{
   int i, j;
   Gan_RGBPixel_f32 pix1, pix2, pix3, pix4, pix;

   /* if mask is NULL, other parameters are irrelevant */
   if ( mask == NULL )
   {
      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
         {
            pix1 = gan_image_get_pix_rgb_f32 ( image, i*2,   j*2   );
            pix2 = gan_image_get_pix_rgb_f32 ( image, i*2,   j*2+1 );
            pix3 = gan_image_get_pix_rgb_f32 ( image, i*2+1, j*2   );
            pix4 = gan_image_get_pix_rgb_f32 ( image, i*2+1, j*2+1 );
            pix.R = 0.25F*(pix1.R + pix2.R + pix3.R + pix4.R);
            pix.G = 0.25F*(pix1.G + pix2.G + pix3.G + pix4.G);
            pix.B = 0.25F*(pix1.B + pix2.B + pix3.B + pix4.B);
            gan_image_set_pix_rgb_f32 ( himage, i, j, &pix );
         }
   }
   /* mask != NULL, action depends on other parameters */
   else if ( no_neighbours == 4 )
   {
      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
            if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
            {
               pix1 = gan_image_get_pix_rgb_f32 ( image, i*2,   j*2   );
               pix2 = gan_image_get_pix_rgb_f32 ( image, i*2,   j*2+1 );
               pix3 = gan_image_get_pix_rgb_f32 ( image, i*2+1, j*2   );
               pix4 = gan_image_get_pix_rgb_f32 ( image, i*2+1, j*2+1 );
               pix.R = 0.25F*(pix1.R + pix2.R + pix3.R + pix4.R);
               pix.G = 0.25F*(pix1.G + pix2.G + pix3.G + pix4.G);
               pix.B = 0.25F*(pix1.B + pix2.B + pix3.B + pix4.B);
               gan_image_set_pix_rgb_f32 ( himage, i, j, &pix );
               gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
            }
   }
   else if ( average_type == GAN_AVERAGE_IN_MASK )
   {
      if ( mask->type == GAN_BOOL )
      {
         for ( i = (int)image->height/2-1; i >= 0; i-- )
            for ( j = (int)image->width/2-1; j >= 0; j-- )
               if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
               {
                  pix1 = gan_image_get_pix_rgb_f32 ( image, i*2,   j*2   );
                  pix2 = gan_image_get_pix_rgb_f32 ( image, i*2,   j*2+1 );
                  pix3 = gan_image_get_pix_rgb_f32 ( image, i*2+1, j*2   );
                  pix4 = gan_image_get_pix_rgb_f32 ( image, i*2+1, j*2+1 );
                  pix.R = 0.25F*(pix1.R + pix2.R + pix3.R + pix4.R);
                  pix.G = 0.25F*(pix1.G + pix2.G + pix3.G + pix4.G);
                  pix.B = 0.25F*(pix1.B + pix2.B + pix3.B + pix4.B);
                  gan_image_set_pix_rgb_f32 ( himage, i, j, &pix );
                  gan_image_set_pix_gl_ui16 ( hmask, i, j, 4 );
               }
               else
               {
                  gan_uint16 num=0;
                  Gan_RGBPixel_f32 total;

                  total.R = total.G = total.B = 0.0F;
                  if ( gan_image_get_pix_b(mask,i*2,j*2) )
                  {
                     pix = gan_image_get_pix_rgb_f32 ( image, i*2, j*2 );
                     total.R += pix.R;
                     total.G += pix.G;
                     total.B += pix.B;
                     num++;
                  }
                    
                  if ( gan_image_get_pix_b(mask,i*2,j*2+1) )
                  {
                     pix = gan_image_get_pix_rgb_f32 ( image, i*2, j*2+1 );
                     total.R += pix.R;
                     total.G += pix.G;
                     total.B += pix.B;
                     num++;
                  }

                  if ( gan_image_get_pix_b(mask,i*2+1,j*2) )
                  {
                     pix = gan_image_get_pix_rgb_f32 ( image, i*2+1, j*2 );
                     total.R += pix.R;
                     total.G += pix.G;
                     total.B += pix.B;
                     num++;
                  }
                    
                  if ( gan_image_get_pix_b(mask,i*2+1,j*2+1) )
                  {
                     pix = gan_image_get_pix_rgb_f32 ( image, i*2+1, j*2+1 );
                     total.R += pix.R;
                     total.G += pix.G;
                     total.B += pix.B;
                     num++;
                  }

                  if ( num >= thres )
                  {
                     gan_float32 fnum = (gan_float32)num;
                     pix.R = total.R/fnum;
                     pix.G = total.G/fnum;
                     pix.B = total.B/fnum;
                     gan_image_set_pix_rgb_f32 ( himage, i, j, &pix );
                     gan_image_set_pix_gl_ui16 ( hmask, i, j, num );
                  }
               }             
      }
      else
      {
         for ( i = (int)image->height/2-1; i >= 0; i-- )
            for ( j = (int)image->width/2-1; j >= 0; j-- )
            {
               gan_uint16 mval, num=0;
               float fmval;
               Gan_RGBPixel_f32 total;

               total.R = total.G = total.B = 0.0F;

               mval = gan_image_get_pix_gl_ui16(mask,i*2,j*2);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_rgb_f32 ( image, i*2, j*2 );
                  fmval = (gan_float32)mval;
                  total.R += fmval*pix.R;
                  total.G += fmval*pix.G;
                  total.B += fmval*pix.B;
                  num += mval;
               }

               mval = gan_image_get_pix_gl_ui16(mask,i*2,j*2+1);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_rgb_f32 ( image, i*2, j*2+1 );
                  fmval = (gan_float32)mval;
                  total.R += fmval*pix.R;
                  total.G += fmval*pix.G;
                  total.B += fmval*pix.B;
                  num += mval;
               }

               mval = gan_image_get_pix_gl_ui16(mask,i*2+1,j*2);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_rgb_f32 ( image, i*2+1, j*2 );
                  fmval = (gan_float32)mval;
                  total.R += fmval*pix.R;
                  total.G += fmval*pix.G;
                  total.B += fmval*pix.B;
                  num += mval;
               }
                    
               mval = gan_image_get_pix_gl_ui16(mask,i*2+1,j*2+1);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_rgb_f32 ( image, i*2+1, j*2+1 );
                  fmval = (gan_float32)mval;
                  total.R += fmval*pix.R;
                  total.G += fmval*pix.G;
                  total.B += fmval*pix.B;
                  num += mval;
               }

               if ( num >= thres )
               {
                  gan_float32 fnum = (gan_float32)num;
                  pix.R = total.R/fnum;
                  pix.G = total.G/fnum;
                  pix.B = total.B/fnum;
                  gan_image_set_pix_rgb_f32 ( himage, i, j, &pix );
                  gan_image_set_pix_gl_ui16 ( hmask, i, j, num );
               }             
            }
      }
   }
   else
   {
      unsigned num;

      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
            if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
            {
               pix1 = gan_image_get_pix_rgb_f32 ( image, i*2,   j*2   );
               pix2 = gan_image_get_pix_rgb_f32 ( image, i*2,   j*2+1 );
               pix3 = gan_image_get_pix_rgb_f32 ( image, i*2+1, j*2   );
               pix4 = gan_image_get_pix_rgb_f32 ( image, i*2+1, j*2+1 );
               pix.R = 0.25F*(pix1.R + pix2.R + pix3.R + pix4.R);
               pix.G = 0.25F*(pix1.G + pix2.G + pix3.G + pix4.G);
               pix.B = 0.25F*(pix1.B + pix2.B + pix3.B + pix4.B);
               gan_image_set_pix_rgb_f32 ( himage, i, j, &pix );
               gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
            }
            else
            {
               num = 0; pix.R = pix.G = pix.B = 0.0F;
               if ( gan_image_get_pix_b(mask,i*2,j*2) )
               {
                  pix = gan_image_get_pix_rgb_f32 ( image, i*2, j*2 );
                  pix.R += pix.R;
                  pix.G += pix.G;
                  pix.B += pix.B;
                  num++;
               }
                    
               if ( gan_image_get_pix_b(mask,i*2,j*2+1) )
               {
                  pix = gan_image_get_pix_rgb_f32 ( image, i*2, j*2+1 );
                  pix.R += pix.R;
                  pix.G += pix.G;
                  pix.B += pix.B;
                  num++;
               }

               if ( gan_image_get_pix_b(mask,i*2+1,j*2) )
               {
                  pix = gan_image_get_pix_rgb_f32 ( image, i*2+1, j*2 );
                  pix.R += pix.R;
                  pix.G += pix.G;
                  pix.B += pix.B;
                  num++;
               }
                    
               if ( gan_image_get_pix_b(mask,i*2+1,j*2+1) )
               {
                  pix = gan_image_get_pix_rgb_f32 ( image, i*2+1, j*2+1 );
                  pix.R += pix.R;
                  pix.G += pix.G;
                  pix.B += pix.B;
                  num++;
               }

               if ( num >= no_neighbours )
               {
                  pix.R *= 0.25F;
                  pix.G *= 0.25F;
                  pix.B *= 0.25F;
                  gan_image_set_pix_rgb_f32 ( himage, i, j, &pix );
                  gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
               }
            }
   }

   /* success */
   return GAN_TRUE;
}

static Gan_Bool
 halve_size_rgb_f64 ( Gan_Image *image, Gan_Image *mask, Gan_ImagePyramidAverage average_type,
                      unsigned no_neighbours, Gan_Image *himage, Gan_Image *hmask, gan_uint32 thres )
{
   int i, j;
   Gan_RGBPixel_f64 pix1, pix2, pix3, pix4, pix;

   /* if mask is NULL, other parameters are irrelevant */
   if ( mask == NULL )
   {
      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
         {
            pix1 = gan_image_get_pix_rgb_f64 ( image, i*2,   j*2   );
            pix2 = gan_image_get_pix_rgb_f64 ( image, i*2,   j*2+1 );
            pix3 = gan_image_get_pix_rgb_f64 ( image, i*2+1, j*2   );
            pix4 = gan_image_get_pix_rgb_f64 ( image, i*2+1, j*2+1 );
            pix.R = 0.25*(pix1.R + pix2.R + pix3.R + pix4.R);
            pix.G = 0.25*(pix1.G + pix2.G + pix3.G + pix4.G);
            pix.B = 0.25*(pix1.B + pix2.B + pix3.B + pix4.B);
            gan_image_set_pix_rgb_f64 ( himage, i, j, &pix );
         }
   }
   /* mask != NULL, action depends on other parameters */
   else if ( no_neighbours == 4 )
   {
      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
            if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
            {
               pix1 = gan_image_get_pix_rgb_f64 ( image, i*2,   j*2   );
               pix2 = gan_image_get_pix_rgb_f64 ( image, i*2,   j*2+1 );
               pix3 = gan_image_get_pix_rgb_f64 ( image, i*2+1, j*2   );
               pix4 = gan_image_get_pix_rgb_f64 ( image, i*2+1, j*2+1 );
               pix.R = 0.25*(pix1.R + pix2.R + pix3.R + pix4.R);
               pix.G = 0.25*(pix1.G + pix2.G + pix3.G + pix4.G);
               pix.B = 0.25*(pix1.B + pix2.B + pix3.B + pix4.B);
               gan_image_set_pix_rgb_f64 ( himage, i, j, &pix );
               gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
            }
   }
   else if ( average_type == GAN_AVERAGE_IN_MASK )
   {
      if ( mask->type == GAN_BOOL )
      {
         for ( i = (int)image->height/2-1; i >= 0; i-- )
            for ( j = (int)image->width/2-1; j >= 0; j-- )
               if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
               {
                  pix1 = gan_image_get_pix_rgb_f64 ( image, i*2,   j*2   );
                  pix2 = gan_image_get_pix_rgb_f64 ( image, i*2,   j*2+1 );
                  pix3 = gan_image_get_pix_rgb_f64 ( image, i*2+1, j*2   );
                  pix4 = gan_image_get_pix_rgb_f64 ( image, i*2+1, j*2+1 );
                  pix.R = 0.25F*(pix1.R + pix2.R + pix3.R + pix4.R);
                  pix.G = 0.25F*(pix1.G + pix2.G + pix3.G + pix4.G);
                  pix.B = 0.25F*(pix1.B + pix2.B + pix3.B + pix4.B);
                  gan_image_set_pix_rgb_f64 ( himage, i, j, &pix );
                  gan_image_set_pix_gl_ui16 ( hmask, i, j, 4 );
               }
               else
               {
                  gan_uint16 num=0;
                  Gan_RGBPixel_f64 total;

                  total.R = total.G = total.B = 0.0;
                  if ( gan_image_get_pix_b(mask,i*2,j*2) )
                  {
                     pix = gan_image_get_pix_rgb_f64 ( image, i*2, j*2 );
                     total.R += pix.R;
                     total.G += pix.G;
                     total.B += pix.B;
                     num++;
                  }
                    
                  if ( gan_image_get_pix_b(mask,i*2,j*2+1) )
                  {
                     pix = gan_image_get_pix_rgb_f64 ( image, i*2, j*2+1 );
                     total.R += pix.R;
                     total.G += pix.G;
                     total.B += pix.B;
                     num++;
                  }

                  if ( gan_image_get_pix_b(mask,i*2+1,j*2) )
                  {
                     pix = gan_image_get_pix_rgb_f64 ( image, i*2+1, j*2 );
                     total.R += pix.R;
                     total.G += pix.G;
                     total.B += pix.B;
                     num++;
                  }
                    
                  if ( gan_image_get_pix_b(mask,i*2+1,j*2+1) )
                  {
                     pix = gan_image_get_pix_rgb_f64 ( image, i*2+1, j*2+1 );
                     total.R += pix.R;
                     total.G += pix.G;
                     total.B += pix.B;
                     num++;
                  }

                  if ( num >= thres )
                  {
                     gan_float64 fnum = (gan_float64)num;
                     pix.R = total.R/fnum;
                     pix.G = total.G/fnum;
                     pix.B = total.B/fnum;
                     gan_image_set_pix_rgb_f64 ( himage, i, j, &pix );
                     gan_image_set_pix_gl_ui16 ( hmask, i, j, num );
                  }
               }             
      }
      else
      {
         for ( i = (int)image->height/2-1; i >= 0; i-- )
            for ( j = (int)image->width/2-1; j >= 0; j-- )
            {
               gan_uint16 mval, num=0;
               gan_float64 fmval;
               Gan_RGBPixel_f64 total;

               total.R = total.G = total.B = 0.0;

               mval = gan_image_get_pix_gl_ui16(mask,i*2,j*2);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_rgb_f64 ( image, i*2, j*2 );
                  fmval = (gan_float64)mval;
                  total.R += fmval*pix.R;
                  total.G += fmval*pix.G;
                  total.B += fmval*pix.B;
                  num += mval;
               }

               mval = gan_image_get_pix_gl_ui16(mask,i*2,j*2+1);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_rgb_f64 ( image, i*2, j*2+1 );
                  fmval = (gan_float64)mval;
                  total.R += fmval*pix.R;
                  total.G += fmval*pix.G;
                  total.B += fmval*pix.B;
                  num += mval;
               }

               mval = gan_image_get_pix_gl_ui16(mask,i*2+1,j*2);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_rgb_f64 ( image, i*2+1, j*2 );
                  fmval = (gan_float64)mval;
                  total.R += fmval*pix.R;
                  total.G += fmval*pix.G;
                  total.B += fmval*pix.B;
                  num += mval;
               }
                    
               mval = gan_image_get_pix_gl_ui16(mask,i*2+1,j*2+1);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_rgb_f64 ( image, i*2+1, j*2+1 );
                  fmval = (gan_float64)mval;
                  total.R += fmval*pix.R;
                  total.G += fmval*pix.G;
                  total.B += fmval*pix.B;
                  num += mval;
               }

               if ( num >= thres )
               {
                  gan_float64 fnum = (gan_float64)num;
                  pix.R = total.R/fnum;
                  pix.G = total.G/fnum;
                  pix.B = total.B/fnum;
                  gan_image_set_pix_rgb_f64 ( himage, i, j, &pix );
                  gan_image_set_pix_gl_ui16 ( hmask, i, j, num );
               }             
            }
      }
   }
   else
   {
      unsigned num;

      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
            if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
            {
               pix1 = gan_image_get_pix_rgb_f64 ( image, i*2,   j*2   );
               pix2 = gan_image_get_pix_rgb_f64 ( image, i*2,   j*2+1 );
               pix3 = gan_image_get_pix_rgb_f64 ( image, i*2+1, j*2   );
               pix4 = gan_image_get_pix_rgb_f64 ( image, i*2+1, j*2+1 );
               pix.R = 0.25*(pix1.R + pix2.R + pix3.R + pix4.R);
               pix.G = 0.25*(pix1.G + pix2.G + pix3.G + pix4.G);
               pix.B = 0.25*(pix1.B + pix2.B + pix3.B + pix4.B);
               gan_image_set_pix_rgb_f64 ( himage, i, j, &pix );
               gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
            }
            else
            {
               num = 0; pix.R = pix.G = pix.B = 0.0;
               if ( gan_image_get_pix_b(mask,i*2,j*2) )
               {
                  pix = gan_image_get_pix_rgb_f64 ( image, i*2, j*2 );
                  pix.R += pix.R;
                  pix.G += pix.G;
                  pix.B += pix.B;
                  num++;
               }
                    
               if ( gan_image_get_pix_b(mask,i*2,j*2+1) )
               {
                  pix = gan_image_get_pix_rgb_f64 ( image, i*2, j*2+1 );
                  pix.R += pix.R;
                  pix.G += pix.G;
                  pix.B += pix.B;
                  num++;
               }

               if ( gan_image_get_pix_b(mask,i*2+1,j*2) )
               {
                  pix = gan_image_get_pix_rgb_f64 ( image, i*2+1, j*2 );
                  pix.R += pix.R;
                  pix.G += pix.G;
                  pix.B += pix.B;
                  num++;
               }
                    
               if ( gan_image_get_pix_b(mask,i*2+1,j*2+1) )
               {
                  pix = gan_image_get_pix_rgb_f64 ( image, i*2+1, j*2+1 );
                  pix.R += pix.R;
                  pix.G += pix.G;
                  pix.B += pix.B;
                  num++;
               }

               if ( num >= no_neighbours )
               {
                  pix.R *= 0.25;
                  pix.G *= 0.25;
                  pix.B *= 0.25;
                  gan_image_set_pix_rgb_f64 ( himage, i, j, &pix );
                  gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
               }
            }
   }

   /* success */
   return GAN_TRUE;
}

static Gan_Bool
 halve_size_rgba_ui8 ( Gan_Image *image, Gan_Image *mask, Gan_ImagePyramidAverage average_type,
                       unsigned no_neighbours, Gan_Image *himage, Gan_Image *hmask, gan_uint32 thres )
{
   int i, j;
   Gan_RGBAPixel_ui8 pix1, pix2, pix3, pix4, pix;

   /* if mask is NULL, other parameters are irrelevant */
   if ( mask == NULL )
   {
      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
         {
            pix1 = gan_image_get_pix_rgba_ui8 ( image, i*2,   j*2   );
            pix2 = gan_image_get_pix_rgba_ui8 ( image, i*2,   j*2+1 );
            pix3 = gan_image_get_pix_rgba_ui8 ( image, i*2+1, j*2   );
            pix4 = gan_image_get_pix_rgba_ui8 ( image, i*2+1, j*2+1 );
            pix.R = (gan_uint8)(((gan_uint32) pix1.R + (gan_uint32) pix2.R + (gan_uint32) pix3.R + (gan_uint32) pix4.R + 2)/4);
            pix.G = (gan_uint8)(((gan_uint32) pix1.G + (gan_uint32) pix2.G + (gan_uint32) pix3.G + (gan_uint32) pix4.G + 2)/4);
            pix.B = (gan_uint8)(((gan_uint32) pix1.B + (gan_uint32) pix2.B + (gan_uint32) pix3.B + (gan_uint32) pix4.B + 2)/4);
            pix.A = (gan_uint8)(((gan_uint32) pix1.A + (gan_uint32) pix2.A + (gan_uint32) pix3.A + (gan_uint32) pix4.A + 2)/4);
            gan_image_set_pix_rgba_ui8 ( himage, i, j, &pix );
         }
   }
   /* mask != NULL, action depends on other parameters */
   else if ( no_neighbours == 4 )
   {
      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
            if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
            {
               pix1 = gan_image_get_pix_rgba_ui8 ( image, i*2,   j*2  );
               pix2 = gan_image_get_pix_rgba_ui8 ( image, i*2,   j*2+1);
               pix3 = gan_image_get_pix_rgba_ui8 ( image, i*2+1, j*2  );
               pix4 = gan_image_get_pix_rgba_ui8 ( image, i*2+1, j*2+1);
               pix.R = (gan_uint8)(((gan_uint32) pix1.R + (gan_uint32) pix2.R + (gan_uint32) pix3.R + (gan_uint32) pix4.R + 2)/4);
               pix.G = (gan_uint8)(((gan_uint32) pix1.G + (gan_uint32) pix2.G + (gan_uint32) pix3.G + (gan_uint32) pix4.G + 2)/4);
               pix.B = (gan_uint8)(((gan_uint32) pix1.B + (gan_uint32) pix2.B + (gan_uint32) pix3.B + (gan_uint32) pix4.B + 2)/4);
               pix.A = (gan_uint8)(((gan_uint32) pix1.A + (gan_uint32) pix2.A + (gan_uint32) pix3.A + (gan_uint32) pix4.A + 2)/4);
               gan_image_set_pix_rgba_ui8 ( himage, i, j, &pix );
               gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
            }
   }
   else if ( average_type == GAN_AVERAGE_IN_MASK )
   {
      if ( mask->type == GAN_BOOL )
      {
         for ( i = (int)image->height/2-1; i >= 0; i-- )
            for ( j = (int)image->width/2-1; j >= 0; j-- )
               if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
               {
                  pix1 = gan_image_get_pix_rgba_ui8 ( image, i*2,   j*2  );
                  pix2 = gan_image_get_pix_rgba_ui8 ( image, i*2,   j*2+1);
                  pix3 = gan_image_get_pix_rgba_ui8 ( image, i*2+1, j*2  );
                  pix4 = gan_image_get_pix_rgba_ui8 ( image, i*2+1, j*2+1);
                  pix.R = (gan_uint8)(((gan_uint32) pix1.R + (gan_uint32) pix2.R + (gan_uint32) pix3.R + (gan_uint32) pix4.R + 2)/4);
                  pix.G = (gan_uint8)(((gan_uint32) pix1.G + (gan_uint32) pix2.G + (gan_uint32) pix3.G + (gan_uint32) pix4.G + 2)/4);
                  pix.B = (gan_uint8)(((gan_uint32) pix1.B + (gan_uint32) pix2.B + (gan_uint32) pix3.B + (gan_uint32) pix4.B + 2)/4);
                  pix.A = (gan_uint8)(((gan_uint32) pix1.A + (gan_uint32) pix2.A + (gan_uint32) pix3.A + (gan_uint32) pix4.A + 2)/4);
                  gan_image_set_pix_rgba_ui8 ( himage, i, j, &pix );
                  gan_image_set_pix_gl_ui16 ( hmask, i, j, 4 );
               }
               else
               {
                  gan_uint16 num=0;
                  Gan_RGBAPixel_ui16 total;

                  total.R = total.G = total.B = total.A = 0;
                  if ( gan_image_get_pix_b(mask,i*2,j*2) )
                  {
                     pix = gan_image_get_pix_rgba_ui8 ( image, i*2, j*2 );
                     total.R += (gan_uint16)pix.R;
                     total.G += (gan_uint16)pix.G;
                     total.B += (gan_uint16)pix.B;
                     total.A += (gan_uint16)pix.A;
                     num++;
                  }
                    
                  if ( gan_image_get_pix_b(mask,i*2,j*2+1) )
                  {
                     pix = gan_image_get_pix_rgba_ui8 ( image, i*2, j*2+1 );
                     total.R += (gan_uint16)pix.R;
                     total.G += (gan_uint16)pix.G;
                     total.B += (gan_uint16)pix.B;
                     total.A += (gan_uint16)pix.A;
                     num++;
                  }

                  if ( gan_image_get_pix_b(mask,i*2+1,j*2) )
                  {
                     pix = gan_image_get_pix_rgba_ui8 ( image, i*2+1, j*2 );
                     total.R += (gan_uint16)pix.R;
                     total.G += (gan_uint16)pix.G;
                     total.B += (gan_uint16)pix.B;
                     total.A += (gan_uint16)pix.A;
                     num++;
                  }
                    
                  if ( gan_image_get_pix_b(mask,i*2+1,j*2+1) )
                  {
                     pix = gan_image_get_pix_rgba_ui8 ( image, i*2+1, j*2+1 );
                     total.R += (gan_uint16)pix.R;
                     total.G += (gan_uint16)pix.G;
                     total.B += (gan_uint16)pix.B;
                     total.A += (gan_uint16)pix.A;
                     num++;
                  }

                  if ( num >= thres )
                  {
                     pix.R = (gan_uint8) ((total.R + num/2)/num);
                     pix.G = (gan_uint8) ((total.G + num/2)/num);
                     pix.B = (gan_uint8) ((total.B + num/2)/num);
                     pix.A = (gan_uint8) ((total.A + num/2)/num);
                     gan_image_set_pix_rgba_ui8 ( himage, i, j, &pix );
                     gan_image_set_pix_gl_ui16 ( hmask, i, j, num );
                  }
               }             
      }
      else
      {
         for ( i = (int)image->height/2-1; i >= 0; i-- )
            for ( j = (int)image->width/2-1; j >= 0; j-- )
            {
               gan_uint32 mval, num=0;
               Gan_RGBAPixel_ui32 total;

               total.R = total.G = total.B = total.A = 0;

               mval = (gan_uint32) gan_image_get_pix_gl_ui16(mask,i*2,j*2);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_rgba_ui8 ( image, i*2, j*2 );
                  total.R += mval*(gan_uint32)pix.R;
                  total.G += mval*(gan_uint32)pix.G;
                  total.B += mval*(gan_uint32)pix.B;
                  total.A += mval*(gan_uint32)pix.A;
                  num += mval;
               }
                    
               mval = (gan_uint32) gan_image_get_pix_gl_ui16(mask,i*2,j*2+1);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_rgba_ui8 ( image, i*2, j*2+1 );
                  total.R += mval*(gan_uint32)pix.R;
                  total.G += mval*(gan_uint32)pix.G;
                  total.B += mval*(gan_uint32)pix.B;
                  total.A += mval*(gan_uint32)pix.A;
                  num += mval;
               }

               mval = (gan_uint32) gan_image_get_pix_gl_ui16(mask,i*2+1,j*2);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_rgba_ui8 ( image, i*2+1, j*2 );
                  total.R += mval*(gan_uint32)pix.R;
                  total.G += mval*(gan_uint32)pix.G;
                  total.B += mval*(gan_uint32)pix.B;
                  total.A += mval*(gan_uint32)pix.A;
                  num += mval;
               }
                    
               mval = (gan_uint32) gan_image_get_pix_gl_ui16(mask,i*2+1,j*2+1);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_rgba_ui8 ( image, i*2+1, j*2+1 );
                  total.R += mval*(gan_uint32)pix.R;
                  total.G += mval*(gan_uint32)pix.G;
                  total.B += mval*(gan_uint32)pix.B;
                  total.A += mval*(gan_uint32)pix.A;
                  num += mval;
               }

               if ( num >= thres )
               {
                  pix.R = (gan_uint8) ((total.R + num/2)/num);
                  pix.G = (gan_uint8) ((total.G + num/2)/num);
                  pix.B = (gan_uint8) ((total.B + num/2)/num);
                  pix.A = (gan_uint8) ((total.A + num/2)/num);
                  gan_image_set_pix_rgba_ui8 ( himage, i, j, &pix );
                  gan_image_set_pix_gl_ui16 ( hmask, i, j, num );
               }             
            }
      }
   }
   else
   {
      unsigned num;
      Gan_RGBAPixel_ui32 total;

      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
            if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
            {
               pix1 = gan_image_get_pix_rgba_ui8 ( image, i*2,   j*2  );
               pix2 = gan_image_get_pix_rgba_ui8 ( image, i*2,   j*2+1);
               pix3 = gan_image_get_pix_rgba_ui8 ( image, i*2+1, j*2  );
               pix4 = gan_image_get_pix_rgba_ui8 ( image, i*2+1, j*2+1);
               pix.R = (gan_uint8)(((gan_uint32) pix1.R + (gan_uint32) pix2.R + (gan_uint32) pix3.R + (gan_uint32) pix4.R + 2)/4);
               pix.G = (gan_uint8)(((gan_uint32) pix1.G + (gan_uint32) pix2.G + (gan_uint32) pix3.G + (gan_uint32) pix4.G + 2)/4);
               pix.B = (gan_uint8)(((gan_uint32) pix1.B + (gan_uint32) pix2.B + (gan_uint32) pix3.B + (gan_uint32) pix4.B + 2)/4);
               pix.A = (gan_uint8)(((gan_uint32) pix1.A + (gan_uint32) pix2.A + (gan_uint32) pix3.A + (gan_uint32) pix4.A + 2)/4);
               gan_image_set_pix_rgba_ui8 ( himage, i, j, &pix );
               gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
            }
            else
            {
               num = total.R = total.G = total.B = total.A = 0;
               if ( gan_image_get_pix_b(mask,i*2,j*2) )
               {
                  pix = gan_image_get_pix_rgba_ui8 ( image, i*2, j*2 );
                  total.R += (gan_uint32)pix.R;
                  total.G += (gan_uint32)pix.G;
                  total.B += (gan_uint32)pix.B;
                  total.A += (gan_uint32)pix.A;
                  num++;
               }
                    
               if ( gan_image_get_pix_b(mask,i*2,j*2+1) )
               {
                  pix = gan_image_get_pix_rgba_ui8 ( image, i*2, j*2+1 );
                  total.R += (gan_uint32)pix.R;
                  total.G += (gan_uint32)pix.G;
                  total.B += (gan_uint32)pix.B;
                  total.A += (gan_uint32)pix.A;
                  num++;
               }

               if ( gan_image_get_pix_b(mask,i*2+1,j*2) )
               {
                  pix = gan_image_get_pix_rgba_ui8 ( image, i*2+1, j*2 );
                  total.R += (gan_uint32)pix.R;
                  total.G += (gan_uint32)pix.G;
                  total.B += (gan_uint32)pix.B;
                  total.A += (gan_uint32)pix.A;
                  num++;
               }
                    
               if ( gan_image_get_pix_b(mask,i*2+1,j*2+1) )
               {
                  pix = gan_image_get_pix_rgba_ui8 ( image, i*2+1, j*2+1 );
                  total.R += (gan_uint32)pix.R;
                  total.G += (gan_uint32)pix.G;
                  total.B += (gan_uint32)pix.B;
                  total.A += (gan_uint32)pix.A;
                  num++;
               }

               if ( num >= no_neighbours )
               {
                  pix.R = (gan_uint8) ((total.R + 2)/4);
                  pix.G = (gan_uint8) ((total.G + 2)/4);
                  pix.B = (gan_uint8) ((total.B + 2)/4);
                  pix.A = (gan_uint8) ((total.A + 2)/4);
                  gan_image_set_pix_rgba_ui8 ( himage, i, j, &pix );
                  gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
               }
            }
   }

   /* success */
   return GAN_TRUE;
}

static Gan_Bool
 halve_size_rgba_ui16 ( Gan_Image *image, Gan_Image *mask, Gan_ImagePyramidAverage average_type,
                        unsigned no_neighbours, Gan_Image *himage, Gan_Image *hmask, gan_uint32 thres )
{
   int i, j;
   Gan_RGBAPixel_ui16 pix1, pix2, pix3, pix4, pix;

   /* if mask is NULL, other parameters are irrelevant */
   if ( mask == NULL )
   {
      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
         {
            pix1 = gan_image_get_pix_rgba_ui16 ( image, i*2,   j*2   );
            pix2 = gan_image_get_pix_rgba_ui16 ( image, i*2,   j*2+1 );
            pix3 = gan_image_get_pix_rgba_ui16 ( image, i*2+1, j*2   );
            pix4 = gan_image_get_pix_rgba_ui16 ( image, i*2+1, j*2+1 );
            pix.R = (gan_uint16)(((gan_uint32) pix1.R + (gan_uint32) pix2.R + (gan_uint32) pix3.R + (gan_uint32) pix4.R + 2)/4);
            pix.G = (gan_uint16)(((gan_uint32) pix1.G + (gan_uint32) pix2.G + (gan_uint32) pix3.G + (gan_uint32) pix4.G + 2)/4);
            pix.B = (gan_uint16)(((gan_uint32) pix1.B + (gan_uint32) pix2.B + (gan_uint32) pix3.B + (gan_uint32) pix4.B + 2)/4);
            pix.A = (gan_uint16)(((gan_uint32) pix1.A + (gan_uint32) pix2.A + (gan_uint32) pix3.A + (gan_uint32) pix4.A + 2)/4);
            gan_image_set_pix_rgba_ui16 ( himage, i, j, &pix );
         }
   }
   /* mask != NULL, action depends on other parameters */
   else if ( no_neighbours == 4 )
   {
      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
            if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
            {
               pix1 = gan_image_get_pix_rgba_ui16 ( image, i*2,   j*2  );
               pix2 = gan_image_get_pix_rgba_ui16 ( image, i*2,   j*2+1);
               pix3 = gan_image_get_pix_rgba_ui16 ( image, i*2+1, j*2  );
               pix4 = gan_image_get_pix_rgba_ui16 ( image, i*2+1, j*2+1);
               pix.R = (gan_uint16)(((gan_uint32) pix1.R + (gan_uint32) pix2.R + (gan_uint32) pix3.R + (gan_uint32) pix4.R + 2)/4);
               pix.G = (gan_uint16)(((gan_uint32) pix1.G + (gan_uint32) pix2.G + (gan_uint32) pix3.G + (gan_uint32) pix4.G + 2)/4);
               pix.B = (gan_uint16)(((gan_uint32) pix1.B + (gan_uint32) pix2.B + (gan_uint32) pix3.B + (gan_uint32) pix4.B + 2)/4);
               pix.A = (gan_uint16)(((gan_uint32) pix1.A + (gan_uint32) pix2.A + (gan_uint32) pix3.A + (gan_uint32) pix4.A + 2)/4);
               gan_image_set_pix_rgba_ui16 ( himage, i, j, &pix );
               gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
            }
   }
   else if ( average_type == GAN_AVERAGE_IN_MASK )
   {
      if ( mask->type == GAN_BOOL )
      {
         for ( i = (int)image->height/2-1; i >= 0; i-- )
            for ( j = (int)image->width/2-1; j >= 0; j-- )
               if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
               {
                  pix1 = gan_image_get_pix_rgba_ui16 ( image, i*2,   j*2  );
                  pix2 = gan_image_get_pix_rgba_ui16 ( image, i*2,   j*2+1);
                  pix3 = gan_image_get_pix_rgba_ui16 ( image, i*2+1, j*2  );
                  pix4 = gan_image_get_pix_rgba_ui16 ( image, i*2+1, j*2+1);
                  pix.R = (gan_uint16)(((gan_uint32) pix1.R + (gan_uint32) pix2.R + (gan_uint32) pix3.R + (gan_uint32) pix4.R + 2)/4);
                  pix.G = (gan_uint16)(((gan_uint32) pix1.G + (gan_uint32) pix2.G + (gan_uint32) pix3.G + (gan_uint32) pix4.G + 2)/4);
                  pix.B = (gan_uint16)(((gan_uint32) pix1.B + (gan_uint32) pix2.B + (gan_uint32) pix3.B + (gan_uint32) pix4.B + 2)/4);
                  pix.A = (gan_uint16)(((gan_uint32) pix1.A + (gan_uint32) pix2.A + (gan_uint32) pix3.A + (gan_uint32) pix4.A + 2)/4);
                  gan_image_set_pix_rgba_ui16 ( himage, i, j, &pix );
                  gan_image_set_pix_gl_ui16 ( hmask, i, j, 4 );
               }
               else
               {
                  gan_uint16 num=0;
                  Gan_RGBAPixel_ui32 total;

                  total.R = total.G = total.B = total.A = 0;
                  if ( gan_image_get_pix_b(mask,i*2,j*2) )
                  {
                     pix = gan_image_get_pix_rgba_ui16 ( image, i*2, j*2 );
                     total.R += (gan_uint32)pix.R;
                     total.G += (gan_uint32)pix.G;
                     total.B += (gan_uint32)pix.B;
                     total.A += (gan_uint32)pix.A;
                     num++;
                  }
                    
                  if ( gan_image_get_pix_b(mask,i*2,j*2+1) )
                  {
                     pix = gan_image_get_pix_rgba_ui16 ( image, i*2, j*2+1 );
                     total.R += (gan_uint32)pix.R;
                     total.G += (gan_uint32)pix.G;
                     total.B += (gan_uint32)pix.B;
                     total.A += (gan_uint32)pix.A;
                     num++;
                  }

                  if ( gan_image_get_pix_b(mask,i*2+1,j*2) )
                  {
                     pix = gan_image_get_pix_rgba_ui16 ( image, i*2+1, j*2 );
                     total.R += (gan_uint32)pix.R;
                     total.G += (gan_uint32)pix.G;
                     total.B += (gan_uint32)pix.B;
                     total.A += (gan_uint32)pix.A;
                     num++;
                  }
                    
                  if ( gan_image_get_pix_b(mask,i*2+1,j*2+1) )
                  {
                     pix = gan_image_get_pix_rgba_ui16 ( image, i*2+1, j*2+1 );
                     total.R += (gan_uint32)pix.R;
                     total.G += (gan_uint32)pix.G;
                     total.B += (gan_uint32)pix.B;
                     total.A += (gan_uint32)pix.A;
                     num++;
                  }

                  if ( num >= thres )
                  {
                     pix.R = (gan_uint16) ((total.R + num/2)/num);
                     pix.G = (gan_uint16) ((total.G + num/2)/num);
                     pix.B = (gan_uint16) ((total.B + num/2)/num);
                     pix.A = (gan_uint16) ((total.A + num/2)/num);
                     gan_image_set_pix_rgba_ui16 ( himage, i, j, &pix );
                     gan_image_set_pix_gl_ui16 ( hmask, i, j, num );
                  }
               }             
      }
      else
      {
         for ( i = (int)image->height/2-1; i >= 0; i-- )
            for ( j = (int)image->width/2-1; j >= 0; j-- )
            {
               gan_uint32 mval, num=0;
               Gan_RGBAPixel_ui32 total;

               total.R = total.G = total.B = total.A = 0;

               mval = (gan_uint32) gan_image_get_pix_gl_ui16(mask,i*2,j*2);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_rgba_ui16 ( image, i*2, j*2 );
                  total.R += mval*(gan_uint32)pix.R;
                  total.G += mval*(gan_uint32)pix.G;
                  total.B += mval*(gan_uint32)pix.B;
                  total.A += mval*(gan_uint32)pix.A;
                  num += mval;
               }
                    
               mval = (gan_uint32) gan_image_get_pix_gl_ui16(mask,i*2,j*2+1);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_rgba_ui16 ( image, i*2, j*2+1 );
                  total.R += mval*(gan_uint32)pix.R;
                  total.G += mval*(gan_uint32)pix.G;
                  total.B += mval*(gan_uint32)pix.B;
                  total.A += mval*(gan_uint32)pix.A;
                  num += mval;
               }

               mval = (gan_uint32) gan_image_get_pix_gl_ui16(mask,i*2+1,j*2);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_rgba_ui16 ( image, i*2+1, j*2 );
                  total.R += mval*(gan_uint32)pix.R;
                  total.G += mval*(gan_uint32)pix.G;
                  total.B += mval*(gan_uint32)pix.B;
                  total.A += mval*(gan_uint32)pix.A;
                  num += mval;
               }
                    
               mval = (gan_uint32) gan_image_get_pix_gl_ui16(mask,i*2+1,j*2+1);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_rgba_ui16 ( image, i*2+1, j*2+1 );
                  total.R += mval*(gan_uint32)pix.R;
                  total.G += mval*(gan_uint32)pix.G;
                  total.B += mval*(gan_uint32)pix.B;
                  total.A += mval*(gan_uint32)pix.A;
                  num += mval;
               }

               if ( num >= thres )
               {
                  pix.R = (gan_uint16) ((total.R + num/2)/num);
                  pix.G = (gan_uint16) ((total.G + num/2)/num);
                  pix.B = (gan_uint16) ((total.B + num/2)/num);
                  pix.A = (gan_uint16) ((total.A + num/2)/num);
                  gan_image_set_pix_rgba_ui16 ( himage, i, j, &pix );
                  gan_image_set_pix_gl_ui16 ( hmask, i, j, num );
               }             
            }
      }
   }
   else
   {
      unsigned num;
      Gan_RGBAPixel_ui32 total;

      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
            if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
            {
               pix1 = gan_image_get_pix_rgba_ui16 ( image, i*2,   j*2  );
               pix2 = gan_image_get_pix_rgba_ui16 ( image, i*2,   j*2+1);
               pix3 = gan_image_get_pix_rgba_ui16 ( image, i*2+1, j*2  );
               pix4 = gan_image_get_pix_rgba_ui16 ( image, i*2+1, j*2+1);
               pix.R = (gan_uint16)(((gan_uint32) pix1.R + (gan_uint32) pix2.R + (gan_uint32) pix3.R + (gan_uint32) pix4.R + 2)/4);
               pix.G = (gan_uint16)(((gan_uint32) pix1.G + (gan_uint32) pix2.G + (gan_uint32) pix3.G + (gan_uint32) pix4.G + 2)/4);
               pix.B = (gan_uint16)(((gan_uint32) pix1.B + (gan_uint32) pix2.B + (gan_uint32) pix3.B + (gan_uint32) pix4.B + 2)/4);
               pix.A = (gan_uint16)(((gan_uint32) pix1.A + (gan_uint32) pix2.A + (gan_uint32) pix3.A + (gan_uint32) pix4.A + 2)/4);
               gan_image_set_pix_rgba_ui16 ( himage, i, j, &pix );
               gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
            }
            else
            {
               num = total.R = total.G = total.B = total.A = 0;
               if ( gan_image_get_pix_b(mask,i*2,j*2) )
               {
                  pix = gan_image_get_pix_rgba_ui16 ( image, i*2, j*2 );
                  total.R += (gan_uint32)pix.R;
                  total.G += (gan_uint32)pix.G;
                  total.B += (gan_uint32)pix.B;
                  total.A += (gan_uint32)pix.A;
                  num++;
               }
                    
               if ( gan_image_get_pix_b(mask,i*2,j*2+1) )
               {
                  pix = gan_image_get_pix_rgba_ui16 ( image, i*2, j*2+1 );
                  total.R += (gan_uint32)pix.R;
                  total.G += (gan_uint32)pix.G;
                  total.B += (gan_uint32)pix.B;
                  total.A += (gan_uint32)pix.A;
                  num++;
               }

               if ( gan_image_get_pix_b(mask,i*2+1,j*2) )
               {
                  pix = gan_image_get_pix_rgba_ui16 ( image, i*2+1, j*2 );
                  total.R += (gan_uint32)pix.R;
                  total.G += (gan_uint32)pix.G;
                  total.B += (gan_uint32)pix.B;
                  total.A += (gan_uint32)pix.A;
                  num++;
               }
                    
               if ( gan_image_get_pix_b(mask,i*2+1,j*2+1) )
               {
                  pix = gan_image_get_pix_rgba_ui16 ( image, i*2+1, j*2+1 );
                  total.R += (gan_uint32)pix.R;
                  total.G += (gan_uint32)pix.G;
                  total.B += (gan_uint32)pix.B;
                  total.A += (gan_uint32)pix.A;
                  num++;
               }

               if ( num >= no_neighbours )
               {
                  pix.R = (gan_uint16) ((total.R + 2)/4);
                  pix.G = (gan_uint16) ((total.G + 2)/4);
                  pix.B = (gan_uint16) ((total.B + 2)/4);
                  pix.A = (gan_uint16) ((total.A + 2)/4);
                  gan_image_set_pix_rgba_ui16 ( himage, i, j, &pix );
                  gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
               }
            }
   }

   /* success */
   return GAN_TRUE;
}

static Gan_Bool
 halve_size_bgr_f32 ( Gan_Image *image, Gan_Image *mask, Gan_ImagePyramidAverage average_type,
                      unsigned no_neighbours, Gan_Image *himage, Gan_Image *hmask, gan_uint32 thres )
{
   int i, j;
   Gan_BGRPixel_f32 pix1, pix2, pix3, pix4, pix;

   /* if mask is NULL, other parameters are irrelevant */
   if ( mask == NULL )
   {
      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
         {
            pix1 = gan_image_get_pix_bgr_f32 ( image, i*2,   j*2   );
            pix2 = gan_image_get_pix_bgr_f32 ( image, i*2,   j*2+1 );
            pix3 = gan_image_get_pix_bgr_f32 ( image, i*2+1, j*2   );
            pix4 = gan_image_get_pix_bgr_f32 ( image, i*2+1, j*2+1 );
            pix.R = 0.25F*(pix1.R + pix2.R + pix3.R + pix4.R);
            pix.G = 0.25F*(pix1.G + pix2.G + pix3.G + pix4.G);
            pix.B = 0.25F*(pix1.B + pix2.B + pix3.B + pix4.B);
            gan_image_set_pix_bgr_f32 ( himage, i, j, &pix );
         }
   }
   /* mask != NULL, action depends on other parameters */
   else if ( no_neighbours == 4 )
   {
      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
            if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
            {
               pix1 = gan_image_get_pix_bgr_f32 ( image, i*2,   j*2   );
               pix2 = gan_image_get_pix_bgr_f32 ( image, i*2,   j*2+1 );
               pix3 = gan_image_get_pix_bgr_f32 ( image, i*2+1, j*2   );
               pix4 = gan_image_get_pix_bgr_f32 ( image, i*2+1, j*2+1 );
               pix.R = 0.25F*(pix1.R + pix2.R + pix3.R + pix4.R);
               pix.G = 0.25F*(pix1.G + pix2.G + pix3.G + pix4.G);
               pix.B = 0.25F*(pix1.B + pix2.B + pix3.B + pix4.B);
               gan_image_set_pix_bgr_f32 ( himage, i, j, &pix );
               gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
            }
   }
   else if ( average_type == GAN_AVERAGE_IN_MASK )
   {
      if ( mask->type == GAN_BOOL )
      {
         for ( i = (int)image->height/2-1; i >= 0; i-- )
            for ( j = (int)image->width/2-1; j >= 0; j-- )
               if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
               {
                  pix1 = gan_image_get_pix_bgr_f32 ( image, i*2,   j*2   );
                  pix2 = gan_image_get_pix_bgr_f32 ( image, i*2,   j*2+1 );
                  pix3 = gan_image_get_pix_bgr_f32 ( image, i*2+1, j*2   );
                  pix4 = gan_image_get_pix_bgr_f32 ( image, i*2+1, j*2+1 );
                  pix.R = 0.25F*(pix1.R + pix2.R + pix3.R + pix4.R);
                  pix.G = 0.25F*(pix1.G + pix2.G + pix3.G + pix4.G);
                  pix.B = 0.25F*(pix1.B + pix2.B + pix3.B + pix4.B);
                  gan_image_set_pix_bgr_f32 ( himage, i, j, &pix );
                  gan_image_set_pix_gl_ui16 ( hmask, i, j, 4 );
               }
               else
               {
                  gan_uint16 num=0;
                  Gan_BGRPixel_f32 total;

                  total.R = total.G = total.B = 0.0F;
                  if ( gan_image_get_pix_b(mask,i*2,j*2) )
                  {
                     pix = gan_image_get_pix_bgr_f32 ( image, i*2, j*2 );
                     total.R += pix.R;
                     total.G += pix.G;
                     total.B += pix.B;
                     num++;
                  }
                    
                  if ( gan_image_get_pix_b(mask,i*2,j*2+1) )
                  {
                     pix = gan_image_get_pix_bgr_f32 ( image, i*2, j*2+1 );
                     total.R += pix.R;
                     total.G += pix.G;
                     total.B += pix.B;
                     num++;
                  }

                  if ( gan_image_get_pix_b(mask,i*2+1,j*2) )
                  {
                     pix = gan_image_get_pix_bgr_f32 ( image, i*2+1, j*2 );
                     total.R += pix.R;
                     total.G += pix.G;
                     total.B += pix.B;
                     num++;
                  }
                    
                  if ( gan_image_get_pix_b(mask,i*2+1,j*2+1) )
                  {
                     pix = gan_image_get_pix_bgr_f32 ( image, i*2+1, j*2+1 );
                     total.R += pix.R;
                     total.G += pix.G;
                     total.B += pix.B;
                     num++;
                  }

                  if ( num >= thres )
                  {
                     gan_float32 fnum = (gan_float32)num;
                     pix.R = total.R/fnum;
                     pix.G = total.G/fnum;
                     pix.B = total.B/fnum;
                     gan_image_set_pix_bgr_f32 ( himage, i, j, &pix );
                     gan_image_set_pix_gl_ui16 ( hmask, i, j, num );
                  }
               }             
      }
      else
      {
         for ( i = (int)image->height/2-1; i >= 0; i-- )
            for ( j = (int)image->width/2-1; j >= 0; j-- )
            {
               gan_uint16 mval, num=0;
               float fmval;
               Gan_BGRPixel_f32 total;

               total.R = total.G = total.B = 0.0F;

               mval = gan_image_get_pix_gl_ui16(mask,i*2,j*2);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_bgr_f32 ( image, i*2, j*2 );
                  fmval = (gan_float32)mval;
                  total.R += fmval*pix.R;
                  total.G += fmval*pix.G;
                  total.B += fmval*pix.B;
                  num += mval;
               }

               mval = gan_image_get_pix_gl_ui16(mask,i*2,j*2+1);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_bgr_f32 ( image, i*2, j*2+1 );
                  fmval = (gan_float32)mval;
                  total.R += fmval*pix.R;
                  total.G += fmval*pix.G;
                  total.B += fmval*pix.B;
                  num += mval;
               }

               mval = gan_image_get_pix_gl_ui16(mask,i*2+1,j*2);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_bgr_f32 ( image, i*2+1, j*2 );
                  fmval = (gan_float32)mval;
                  total.R += fmval*pix.R;
                  total.G += fmval*pix.G;
                  total.B += fmval*pix.B;
                  num += mval;
               }
                    
               mval = gan_image_get_pix_gl_ui16(mask,i*2+1,j*2+1);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_bgr_f32 ( image, i*2+1, j*2+1 );
                  fmval = (gan_float32)mval;
                  total.R += fmval*pix.R;
                  total.G += fmval*pix.G;
                  total.B += fmval*pix.B;
                  num += mval;
               }

               if ( num >= thres )
               {
                  gan_float32 fnum = (gan_float32)num;
                  pix.R = total.R/fnum;
                  pix.G = total.G/fnum;
                  pix.B = total.B/fnum;
                  gan_image_set_pix_bgr_f32 ( himage, i, j, &pix );
                  gan_image_set_pix_gl_ui16 ( hmask, i, j, num );
               }             
            }
      }
   }
   else
   {
      unsigned num;

      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
            if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
            {
               pix1 = gan_image_get_pix_bgr_f32 ( image, i*2,   j*2   );
               pix2 = gan_image_get_pix_bgr_f32 ( image, i*2,   j*2+1 );
               pix3 = gan_image_get_pix_bgr_f32 ( image, i*2+1, j*2   );
               pix4 = gan_image_get_pix_bgr_f32 ( image, i*2+1, j*2+1 );
               pix.R = 0.25F*(pix1.R + pix2.R + pix3.R + pix4.R);
               pix.G = 0.25F*(pix1.G + pix2.G + pix3.G + pix4.G);
               pix.B = 0.25F*(pix1.B + pix2.B + pix3.B + pix4.B);
               gan_image_set_pix_bgr_f32 ( himage, i, j, &pix );
               gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
            }
            else
            {
               num = 0; pix.R = pix.G = pix.B = 0.0F;
               if ( gan_image_get_pix_b(mask,i*2,j*2) )
               {
                  pix = gan_image_get_pix_bgr_f32 ( image, i*2, j*2 );
                  pix.R += pix.R;
                  pix.G += pix.G;
                  pix.B += pix.B;
                  num++;
               }
                    
               if ( gan_image_get_pix_b(mask,i*2,j*2+1) )
               {
                  pix = gan_image_get_pix_bgr_f32 ( image, i*2, j*2+1 );
                  pix.R += pix.R;
                  pix.G += pix.G;
                  pix.B += pix.B;
                  num++;
               }

               if ( gan_image_get_pix_b(mask,i*2+1,j*2) )
               {
                  pix = gan_image_get_pix_bgr_f32 ( image, i*2+1, j*2 );
                  pix.R += pix.R;
                  pix.G += pix.G;
                  pix.B += pix.B;
                  num++;
               }
                    
               if ( gan_image_get_pix_b(mask,i*2+1,j*2+1) )
               {
                  pix = gan_image_get_pix_bgr_f32 ( image, i*2+1, j*2+1 );
                  pix.R += pix.R;
                  pix.G += pix.G;
                  pix.B += pix.B;
                  num++;
               }

               if ( num >= no_neighbours )
               {
                  pix.R *= 0.25F;
                  pix.G *= 0.25F;
                  pix.B *= 0.25F;
                  gan_image_set_pix_bgr_f32 ( himage, i, j, &pix );
                  gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
               }
            }
   }

   /* success */
   return GAN_TRUE;
}

static Gan_Bool
 halve_size_bgr_f64 ( Gan_Image *image, Gan_Image *mask, Gan_ImagePyramidAverage average_type,
                      unsigned no_neighbours, Gan_Image *himage, Gan_Image *hmask, gan_uint32 thres )
{
   int i, j;
   Gan_BGRPixel_f64 pix1, pix2, pix3, pix4, pix;

   /* if mask is NULL, other parameters are irrelevant */
   if ( mask == NULL )
   {
      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
         {
            pix1 = gan_image_get_pix_bgr_f64 ( image, i*2,   j*2   );
            pix2 = gan_image_get_pix_bgr_f64 ( image, i*2,   j*2+1 );
            pix3 = gan_image_get_pix_bgr_f64 ( image, i*2+1, j*2   );
            pix4 = gan_image_get_pix_bgr_f64 ( image, i*2+1, j*2+1 );
            pix.R = 0.25*(pix1.R + pix2.R + pix3.R + pix4.R);
            pix.G = 0.25*(pix1.G + pix2.G + pix3.G + pix4.G);
            pix.B = 0.25*(pix1.B + pix2.B + pix3.B + pix4.B);
            gan_image_set_pix_bgr_f64 ( himage, i, j, &pix );
         }
   }
   /* mask != NULL, action depends on other parameters */
   else if ( no_neighbours == 4 )
   {
      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
            if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
            {
               pix1 = gan_image_get_pix_bgr_f64 ( image, i*2,   j*2   );
               pix2 = gan_image_get_pix_bgr_f64 ( image, i*2,   j*2+1 );
               pix3 = gan_image_get_pix_bgr_f64 ( image, i*2+1, j*2   );
               pix4 = gan_image_get_pix_bgr_f64 ( image, i*2+1, j*2+1 );
               pix.R = 0.25*(pix1.R + pix2.R + pix3.R + pix4.R);
               pix.G = 0.25*(pix1.G + pix2.G + pix3.G + pix4.G);
               pix.B = 0.25*(pix1.B + pix2.B + pix3.B + pix4.B);
               gan_image_set_pix_bgr_f64 ( himage, i, j, &pix );
               gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
            }
   }
   else if ( average_type == GAN_AVERAGE_IN_MASK )
   {
      if ( mask->type == GAN_BOOL )
      {
         for ( i = (int)image->height/2-1; i >= 0; i-- )
            for ( j = (int)image->width/2-1; j >= 0; j-- )
               if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
               {
                  pix1 = gan_image_get_pix_bgr_f64 ( image, i*2,   j*2   );
                  pix2 = gan_image_get_pix_bgr_f64 ( image, i*2,   j*2+1 );
                  pix3 = gan_image_get_pix_bgr_f64 ( image, i*2+1, j*2   );
                  pix4 = gan_image_get_pix_bgr_f64 ( image, i*2+1, j*2+1 );
                  pix.R = 0.25F*(pix1.R + pix2.R + pix3.R + pix4.R);
                  pix.G = 0.25F*(pix1.G + pix2.G + pix3.G + pix4.G);
                  pix.B = 0.25F*(pix1.B + pix2.B + pix3.B + pix4.B);
                  gan_image_set_pix_bgr_f64 ( himage, i, j, &pix );
                  gan_image_set_pix_gl_ui16 ( hmask, i, j, 4 );
               }
               else
               {
                  gan_uint16 num=0;
                  Gan_BGRPixel_f64 total;

                  total.R = total.G = total.B = 0.0;
                  if ( gan_image_get_pix_b(mask,i*2,j*2) )
                  {
                     pix = gan_image_get_pix_bgr_f64 ( image, i*2, j*2 );
                     total.R += pix.R;
                     total.G += pix.G;
                     total.B += pix.B;
                     num++;
                  }
                    
                  if ( gan_image_get_pix_b(mask,i*2,j*2+1) )
                  {
                     pix = gan_image_get_pix_bgr_f64 ( image, i*2, j*2+1 );
                     total.R += pix.R;
                     total.G += pix.G;
                     total.B += pix.B;
                     num++;
                  }

                  if ( gan_image_get_pix_b(mask,i*2+1,j*2) )
                  {
                     pix = gan_image_get_pix_bgr_f64 ( image, i*2+1, j*2 );
                     total.R += pix.R;
                     total.G += pix.G;
                     total.B += pix.B;
                     num++;
                  }
                    
                  if ( gan_image_get_pix_b(mask,i*2+1,j*2+1) )
                  {
                     pix = gan_image_get_pix_bgr_f64 ( image, i*2+1, j*2+1 );
                     total.R += pix.R;
                     total.G += pix.G;
                     total.B += pix.B;
                     num++;
                  }

                  if ( num >= thres )
                  {
                     gan_float64 fnum = (gan_float64)num;
                     pix.R = total.R/fnum;
                     pix.G = total.G/fnum;
                     pix.B = total.B/fnum;
                     gan_image_set_pix_bgr_f64 ( himage, i, j, &pix );
                     gan_image_set_pix_gl_ui16 ( hmask, i, j, num );
                  }
               }             
      }
      else
      {
         for ( i = (int)image->height/2-1; i >= 0; i-- )
            for ( j = (int)image->width/2-1; j >= 0; j-- )
            {
               gan_uint16 mval, num=0;
               gan_float64 fmval;
               Gan_BGRPixel_f64 total;

               total.R = total.G = total.B = 0.0;

               mval = gan_image_get_pix_gl_ui16(mask,i*2,j*2);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_bgr_f64 ( image, i*2, j*2 );
                  fmval = (gan_float64)mval;
                  total.R += fmval*pix.R;
                  total.G += fmval*pix.G;
                  total.B += fmval*pix.B;
                  num += mval;
               }

               mval = gan_image_get_pix_gl_ui16(mask,i*2,j*2+1);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_bgr_f64 ( image, i*2, j*2+1 );
                  fmval = (gan_float64)mval;
                  total.R += fmval*pix.R;
                  total.G += fmval*pix.G;
                  total.B += fmval*pix.B;
                  num += mval;
               }

               mval = gan_image_get_pix_gl_ui16(mask,i*2+1,j*2);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_bgr_f64 ( image, i*2+1, j*2 );
                  fmval = (gan_float64)mval;
                  total.R += fmval*pix.R;
                  total.G += fmval*pix.G;
                  total.B += fmval*pix.B;
                  num += mval;
               }
                    
               mval = gan_image_get_pix_gl_ui16(mask,i*2+1,j*2+1);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_bgr_f64 ( image, i*2+1, j*2+1 );
                  fmval = (gan_float64)mval;
                  total.R += fmval*pix.R;
                  total.G += fmval*pix.G;
                  total.B += fmval*pix.B;
                  num += mval;
               }

               if ( num >= thres )
               {
                  gan_float64 fnum = (gan_float64)num;
                  pix.R = total.R/fnum;
                  pix.G = total.G/fnum;
                  pix.B = total.B/fnum;
                  gan_image_set_pix_bgr_f64 ( himage, i, j, &pix );
                  gan_image_set_pix_gl_ui16 ( hmask, i, j, num );
               }             
            }
      }
   }
   else
   {
      unsigned num;

      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
            if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
            {
               pix1 = gan_image_get_pix_bgr_f64 ( image, i*2,   j*2   );
               pix2 = gan_image_get_pix_bgr_f64 ( image, i*2,   j*2+1 );
               pix3 = gan_image_get_pix_bgr_f64 ( image, i*2+1, j*2   );
               pix4 = gan_image_get_pix_bgr_f64 ( image, i*2+1, j*2+1 );
               pix.R = 0.25*(pix1.R + pix2.R + pix3.R + pix4.R);
               pix.G = 0.25*(pix1.G + pix2.G + pix3.G + pix4.G);
               pix.B = 0.25*(pix1.B + pix2.B + pix3.B + pix4.B);
               gan_image_set_pix_bgr_f64 ( himage, i, j, &pix );
               gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
            }
            else
            {
               num = 0; pix.R = pix.G = pix.B = 0.0;
               if ( gan_image_get_pix_b(mask,i*2,j*2) )
               {
                  pix = gan_image_get_pix_bgr_f64 ( image, i*2, j*2 );
                  pix.R += pix.R;
                  pix.G += pix.G;
                  pix.B += pix.B;
                  num++;
               }
                    
               if ( gan_image_get_pix_b(mask,i*2,j*2+1) )
               {
                  pix = gan_image_get_pix_bgr_f64 ( image, i*2, j*2+1 );
                  pix.R += pix.R;
                  pix.G += pix.G;
                  pix.B += pix.B;
                  num++;
               }

               if ( gan_image_get_pix_b(mask,i*2+1,j*2) )
               {
                  pix = gan_image_get_pix_bgr_f64 ( image, i*2+1, j*2 );
                  pix.R += pix.R;
                  pix.G += pix.G;
                  pix.B += pix.B;
                  num++;
               }
                    
               if ( gan_image_get_pix_b(mask,i*2+1,j*2+1) )
               {
                  pix = gan_image_get_pix_bgr_f64 ( image, i*2+1, j*2+1 );
                  pix.R += pix.R;
                  pix.G += pix.G;
                  pix.B += pix.B;
                  num++;
               }

               if ( num >= no_neighbours )
               {
                  pix.R *= 0.25;
                  pix.G *= 0.25;
                  pix.B *= 0.25;
                  gan_image_set_pix_bgr_f64 ( himage, i, j, &pix );
                  gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
               }
            }
   }

   /* success */
   return GAN_TRUE;
}

static Gan_Bool
 halve_size_bgra_ui8 ( Gan_Image *image, Gan_Image *mask, Gan_ImagePyramidAverage average_type,
                       unsigned no_neighbours, Gan_Image *himage, Gan_Image *hmask, gan_uint32 thres )
{
   int i, j;
   Gan_BGRAPixel_ui8 pix1, pix2, pix3, pix4, pix;

   /* if mask is NULL, other parameters are irrelevant */
   if ( mask == NULL )
   {
      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
         {
            pix1 = gan_image_get_pix_bgra_ui8 ( image, i*2,   j*2   );
            pix2 = gan_image_get_pix_bgra_ui8 ( image, i*2,   j*2+1 );
            pix3 = gan_image_get_pix_bgra_ui8 ( image, i*2+1, j*2   );
            pix4 = gan_image_get_pix_bgra_ui8 ( image, i*2+1, j*2+1 );
            pix.R = (gan_uint8)(((gan_uint32) pix1.R + (gan_uint32) pix2.R + (gan_uint32) pix3.R + (gan_uint32) pix4.R + 2)/4);
            pix.G = (gan_uint8)(((gan_uint32) pix1.G + (gan_uint32) pix2.G + (gan_uint32) pix3.G + (gan_uint32) pix4.G + 2)/4);
            pix.B = (gan_uint8)(((gan_uint32) pix1.B + (gan_uint32) pix2.B + (gan_uint32) pix3.B + (gan_uint32) pix4.B + 2)/4);
            pix.A = (gan_uint8)(((gan_uint32) pix1.A + (gan_uint32) pix2.A + (gan_uint32) pix3.A + (gan_uint32) pix4.A + 2)/4);
            gan_image_set_pix_bgra_ui8 ( himage, i, j, &pix );
         }
   }
   /* mask != NULL, action depends on other parameters */
   else if ( no_neighbours == 4 )
   {
      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
            if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
            {
               pix1 = gan_image_get_pix_bgra_ui8 ( image, i*2,   j*2  );
               pix2 = gan_image_get_pix_bgra_ui8 ( image, i*2,   j*2+1);
               pix3 = gan_image_get_pix_bgra_ui8 ( image, i*2+1, j*2  );
               pix4 = gan_image_get_pix_bgra_ui8 ( image, i*2+1, j*2+1);
               pix.R = (gan_uint8)(((gan_uint32) pix1.R + (gan_uint32) pix2.R + (gan_uint32) pix3.R + (gan_uint32) pix4.R + 2)/4);
               pix.G = (gan_uint8)(((gan_uint32) pix1.G + (gan_uint32) pix2.G + (gan_uint32) pix3.G + (gan_uint32) pix4.G + 2)/4);
               pix.B = (gan_uint8)(((gan_uint32) pix1.B + (gan_uint32) pix2.B + (gan_uint32) pix3.B + (gan_uint32) pix4.B + 2)/4);
               pix.A = (gan_uint8)(((gan_uint32) pix1.A + (gan_uint32) pix2.A + (gan_uint32) pix3.A + (gan_uint32) pix4.A + 2)/4);
               gan_image_set_pix_bgra_ui8 ( himage, i, j, &pix );
               gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
            }
   }
   else if ( average_type == GAN_AVERAGE_IN_MASK )
   {
      if ( mask->type == GAN_BOOL )
      {
         for ( i = (int)image->height/2-1; i >= 0; i-- )
            for ( j = (int)image->width/2-1; j >= 0; j-- )
               if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
               {
                  pix1 = gan_image_get_pix_bgra_ui8 ( image, i*2,   j*2  );
                  pix2 = gan_image_get_pix_bgra_ui8 ( image, i*2,   j*2+1);
                  pix3 = gan_image_get_pix_bgra_ui8 ( image, i*2+1, j*2  );
                  pix4 = gan_image_get_pix_bgra_ui8 ( image, i*2+1, j*2+1);
                  pix.R = (gan_uint8)(((gan_uint32) pix1.R + (gan_uint32) pix2.R + (gan_uint32) pix3.R + (gan_uint32) pix4.R + 2)/4);
                  pix.G = (gan_uint8)(((gan_uint32) pix1.G + (gan_uint32) pix2.G + (gan_uint32) pix3.G + (gan_uint32) pix4.G + 2)/4);
                  pix.B = (gan_uint8)(((gan_uint32) pix1.B + (gan_uint32) pix2.B + (gan_uint32) pix3.B + (gan_uint32) pix4.B + 2)/4);
                  pix.A = (gan_uint8)(((gan_uint32) pix1.A + (gan_uint32) pix2.A + (gan_uint32) pix3.A + (gan_uint32) pix4.A + 2)/4);
                  gan_image_set_pix_bgra_ui8 ( himage, i, j, &pix );
                  gan_image_set_pix_gl_ui16 ( hmask, i, j, 4 );
               }
               else
               {
                  gan_uint16 num=0;
                  Gan_BGRAPixel_ui16 total;

                  total.R = total.G = total.B = total.A = 0;
                  if ( gan_image_get_pix_b(mask,i*2,j*2) )
                  {
                     pix = gan_image_get_pix_bgra_ui8 ( image, i*2, j*2 );
                     total.R += (gan_uint16)pix.R;
                     total.G += (gan_uint16)pix.G;
                     total.B += (gan_uint16)pix.B;
                     total.A += (gan_uint16)pix.A;
                     num++;
                  }
                    
                  if ( gan_image_get_pix_b(mask,i*2,j*2+1) )
                  {
                     pix = gan_image_get_pix_bgra_ui8 ( image, i*2, j*2+1 );
                     total.R += (gan_uint16)pix.R;
                     total.G += (gan_uint16)pix.G;
                     total.B += (gan_uint16)pix.B;
                     total.A += (gan_uint16)pix.A;
                     num++;
                  }

                  if ( gan_image_get_pix_b(mask,i*2+1,j*2) )
                  {
                     pix = gan_image_get_pix_bgra_ui8 ( image, i*2+1, j*2 );
                     total.R += (gan_uint16)pix.R;
                     total.G += (gan_uint16)pix.G;
                     total.B += (gan_uint16)pix.B;
                     total.A += (gan_uint16)pix.A;
                     num++;
                  }
                    
                  if ( gan_image_get_pix_b(mask,i*2+1,j*2+1) )
                  {
                     pix = gan_image_get_pix_bgra_ui8 ( image, i*2+1, j*2+1 );
                     total.R += (gan_uint16)pix.R;
                     total.G += (gan_uint16)pix.G;
                     total.B += (gan_uint16)pix.B;
                     total.A += (gan_uint16)pix.A;
                     num++;
                  }

                  if ( num >= thres )
                  {
                     pix.R = (gan_uint8) ((total.R + num/2)/num);
                     pix.G = (gan_uint8) ((total.G + num/2)/num);
                     pix.B = (gan_uint8) ((total.B + num/2)/num);
                     pix.A = (gan_uint8) ((total.A + num/2)/num);
                     gan_image_set_pix_bgra_ui8 ( himage, i, j, &pix );
                     gan_image_set_pix_gl_ui16 ( hmask, i, j, num );
                  }
               }             
      }
      else
      {
         for ( i = (int)image->height/2-1; i >= 0; i-- )
            for ( j = (int)image->width/2-1; j >= 0; j-- )
            {
               gan_uint32 mval, num=0;
               Gan_BGRAPixel_ui32 total;

               total.R = total.G = total.B = total.A = 0;

               mval = (gan_uint32) gan_image_get_pix_gl_ui16(mask,i*2,j*2);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_bgra_ui8 ( image, i*2, j*2 );
                  total.R += mval*(gan_uint32)pix.R;
                  total.G += mval*(gan_uint32)pix.G;
                  total.B += mval*(gan_uint32)pix.B;
                  total.A += mval*(gan_uint32)pix.A;
                  num += mval;
               }
                    
               mval = (gan_uint32) gan_image_get_pix_gl_ui16(mask,i*2,j*2+1);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_bgra_ui8 ( image, i*2, j*2+1 );
                  total.R += mval*(gan_uint32)pix.R;
                  total.G += mval*(gan_uint32)pix.G;
                  total.B += mval*(gan_uint32)pix.B;
                  total.A += mval*(gan_uint32)pix.A;
                  num += mval;
               }

               mval = (gan_uint32) gan_image_get_pix_gl_ui16(mask,i*2+1,j*2);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_bgra_ui8 ( image, i*2+1, j*2 );
                  total.R += mval*(gan_uint32)pix.R;
                  total.G += mval*(gan_uint32)pix.G;
                  total.B += mval*(gan_uint32)pix.B;
                  total.A += mval*(gan_uint32)pix.A;
                  num += mval;
               }
                    
               mval = (gan_uint32) gan_image_get_pix_gl_ui16(mask,i*2+1,j*2+1);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_bgra_ui8 ( image, i*2+1, j*2+1 );
                  total.R += mval*(gan_uint32)pix.R;
                  total.G += mval*(gan_uint32)pix.G;
                  total.B += mval*(gan_uint32)pix.B;
                  total.A += mval*(gan_uint32)pix.A;
                  num += mval;
               }

               if ( num >= thres )
               {
                  pix.R = (gan_uint8) ((total.R + num/2)/num);
                  pix.G = (gan_uint8) ((total.G + num/2)/num);
                  pix.B = (gan_uint8) ((total.B + num/2)/num);
                  pix.A = (gan_uint8) ((total.A + num/2)/num);
                  gan_image_set_pix_bgra_ui8 ( himage, i, j, &pix );
                  gan_image_set_pix_gl_ui16 ( hmask, i, j, num );
               }             
            }
      }
   }
   else
   {
      unsigned num;
      Gan_BGRAPixel_ui32 total;

      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
            if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
            {
               pix1 = gan_image_get_pix_bgra_ui8 ( image, i*2,   j*2  );
               pix2 = gan_image_get_pix_bgra_ui8 ( image, i*2,   j*2+1);
               pix3 = gan_image_get_pix_bgra_ui8 ( image, i*2+1, j*2  );
               pix4 = gan_image_get_pix_bgra_ui8 ( image, i*2+1, j*2+1);
               pix.R = (gan_uint8)(((gan_uint32) pix1.R + (gan_uint32) pix2.R + (gan_uint32) pix3.R + (gan_uint32) pix4.R + 2)/4);
               pix.G = (gan_uint8)(((gan_uint32) pix1.G + (gan_uint32) pix2.G + (gan_uint32) pix3.G + (gan_uint32) pix4.G + 2)/4);
               pix.B = (gan_uint8)(((gan_uint32) pix1.B + (gan_uint32) pix2.B + (gan_uint32) pix3.B + (gan_uint32) pix4.B + 2)/4);
               pix.A = (gan_uint8)(((gan_uint32) pix1.A + (gan_uint32) pix2.A + (gan_uint32) pix3.A + (gan_uint32) pix4.A + 2)/4);
               gan_image_set_pix_bgra_ui8 ( himage, i, j, &pix );
               gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
            }
            else
            {
               num = total.R = total.G = total.B = total.A = 0;
               if ( gan_image_get_pix_b(mask,i*2,j*2) )
               {
                  pix = gan_image_get_pix_bgra_ui8 ( image, i*2, j*2 );
                  total.R += (gan_uint32)pix.R;
                  total.G += (gan_uint32)pix.G;
                  total.B += (gan_uint32)pix.B;
                  total.A += (gan_uint32)pix.A;
                  num++;
               }
                    
               if ( gan_image_get_pix_b(mask,i*2,j*2+1) )
               {
                  pix = gan_image_get_pix_bgra_ui8 ( image, i*2, j*2+1 );
                  total.R += (gan_uint32)pix.R;
                  total.G += (gan_uint32)pix.G;
                  total.B += (gan_uint32)pix.B;
                  total.A += (gan_uint32)pix.A;
                  num++;
               }

               if ( gan_image_get_pix_b(mask,i*2+1,j*2) )
               {
                  pix = gan_image_get_pix_bgra_ui8 ( image, i*2+1, j*2 );
                  total.R += (gan_uint32)pix.R;
                  total.G += (gan_uint32)pix.G;
                  total.B += (gan_uint32)pix.B;
                  total.A += (gan_uint32)pix.A;
                  num++;
               }
                    
               if ( gan_image_get_pix_b(mask,i*2+1,j*2+1) )
               {
                  pix = gan_image_get_pix_bgra_ui8 ( image, i*2+1, j*2+1 );
                  total.R += (gan_uint32)pix.R;
                  total.G += (gan_uint32)pix.G;
                  total.B += (gan_uint32)pix.B;
                  total.A += (gan_uint32)pix.A;
                  num++;
               }

               if ( num >= no_neighbours )
               {
                  pix.R = (gan_uint8) ((total.R + 2)/4);
                  pix.G = (gan_uint8) ((total.G + 2)/4);
                  pix.B = (gan_uint8) ((total.B + 2)/4);
                  pix.A = (gan_uint8) ((total.A + 2)/4);
                  gan_image_set_pix_bgra_ui8 ( himage, i, j, &pix );
                  gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
               }
            }
   }

   /* success */
   return GAN_TRUE;
}

static Gan_Bool
 halve_size_bgra_ui16 ( Gan_Image *image, Gan_Image *mask, Gan_ImagePyramidAverage average_type,
                        unsigned no_neighbours, Gan_Image *himage, Gan_Image *hmask, gan_uint32 thres )
{
   int i, j;
   Gan_BGRAPixel_ui16 pix1, pix2, pix3, pix4, pix;

   /* if mask is NULL, other parameters are irrelevant */
   if ( mask == NULL )
   {
      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
         {
            pix1 = gan_image_get_pix_bgra_ui16 ( image, i*2,   j*2   );
            pix2 = gan_image_get_pix_bgra_ui16 ( image, i*2,   j*2+1 );
            pix3 = gan_image_get_pix_bgra_ui16 ( image, i*2+1, j*2   );
            pix4 = gan_image_get_pix_bgra_ui16 ( image, i*2+1, j*2+1 );
            pix.R = (gan_uint16)(((gan_uint32) pix1.R + (gan_uint32) pix2.R + (gan_uint32) pix3.R + (gan_uint32) pix4.R + 2)/4);
            pix.G = (gan_uint16)(((gan_uint32) pix1.G + (gan_uint32) pix2.G + (gan_uint32) pix3.G + (gan_uint32) pix4.G + 2)/4);
            pix.B = (gan_uint16)(((gan_uint32) pix1.B + (gan_uint32) pix2.B + (gan_uint32) pix3.B + (gan_uint32) pix4.B + 2)/4);
            pix.A = (gan_uint16)(((gan_uint32) pix1.A + (gan_uint32) pix2.A + (gan_uint32) pix3.A + (gan_uint32) pix4.A + 2)/4);
            gan_image_set_pix_bgra_ui16 ( himage, i, j, &pix );
         }
   }
   /* mask != NULL, action depends on other parameters */
   else if ( no_neighbours == 4 )
   {
      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
            if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
            {
               pix1 = gan_image_get_pix_bgra_ui16 ( image, i*2,   j*2  );
               pix2 = gan_image_get_pix_bgra_ui16 ( image, i*2,   j*2+1);
               pix3 = gan_image_get_pix_bgra_ui16 ( image, i*2+1, j*2  );
               pix4 = gan_image_get_pix_bgra_ui16 ( image, i*2+1, j*2+1);
               pix.R = (gan_uint16)(((gan_uint32) pix1.R + (gan_uint32) pix2.R + (gan_uint32) pix3.R + (gan_uint32) pix4.R + 2)/4);
               pix.G = (gan_uint16)(((gan_uint32) pix1.G + (gan_uint32) pix2.G + (gan_uint32) pix3.G + (gan_uint32) pix4.G + 2)/4);
               pix.B = (gan_uint16)(((gan_uint32) pix1.B + (gan_uint32) pix2.B + (gan_uint32) pix3.B + (gan_uint32) pix4.B + 2)/4);
               pix.A = (gan_uint16)(((gan_uint32) pix1.A + (gan_uint32) pix2.A + (gan_uint32) pix3.A + (gan_uint32) pix4.A + 2)/4);
               gan_image_set_pix_bgra_ui16 ( himage, i, j, &pix );
               gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
            }
   }
   else if ( average_type == GAN_AVERAGE_IN_MASK )
   {
      if ( mask->type == GAN_BOOL )
      {
         for ( i = (int)image->height/2-1; i >= 0; i-- )
            for ( j = (int)image->width/2-1; j >= 0; j-- )
               if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
               {
                  pix1 = gan_image_get_pix_bgra_ui16 ( image, i*2,   j*2  );
                  pix2 = gan_image_get_pix_bgra_ui16 ( image, i*2,   j*2+1);
                  pix3 = gan_image_get_pix_bgra_ui16 ( image, i*2+1, j*2  );
                  pix4 = gan_image_get_pix_bgra_ui16 ( image, i*2+1, j*2+1);
                  pix.R = (gan_uint16)(((gan_uint32) pix1.R + (gan_uint32) pix2.R + (gan_uint32) pix3.R + (gan_uint32) pix4.R + 2)/4);
                  pix.G = (gan_uint16)(((gan_uint32) pix1.G + (gan_uint32) pix2.G + (gan_uint32) pix3.G + (gan_uint32) pix4.G + 2)/4);
                  pix.B = (gan_uint16)(((gan_uint32) pix1.B + (gan_uint32) pix2.B + (gan_uint32) pix3.B + (gan_uint32) pix4.B + 2)/4);
                  pix.A = (gan_uint16)(((gan_uint32) pix1.A + (gan_uint32) pix2.A + (gan_uint32) pix3.A + (gan_uint32) pix4.A + 2)/4);
                  gan_image_set_pix_bgra_ui16 ( himage, i, j, &pix );
                  gan_image_set_pix_gl_ui16 ( hmask, i, j, 4 );
               }
               else
               {
                  gan_uint16 num=0;
                  Gan_BGRAPixel_ui32 total;

                  total.R = total.G = total.B = total.A = 0;
                  if ( gan_image_get_pix_b(mask,i*2,j*2) )
                  {
                     pix = gan_image_get_pix_bgra_ui16 ( image, i*2, j*2 );
                     total.R += (gan_uint32)pix.R;
                     total.G += (gan_uint32)pix.G;
                     total.B += (gan_uint32)pix.B;
                     total.A += (gan_uint32)pix.A;
                     num++;
                  }
                    
                  if ( gan_image_get_pix_b(mask,i*2,j*2+1) )
                  {
                     pix = gan_image_get_pix_bgra_ui16 ( image, i*2, j*2+1 );
                     total.R += (gan_uint32)pix.R;
                     total.G += (gan_uint32)pix.G;
                     total.B += (gan_uint32)pix.B;
                     total.A += (gan_uint32)pix.A;
                     num++;
                  }

                  if ( gan_image_get_pix_b(mask,i*2+1,j*2) )
                  {
                     pix = gan_image_get_pix_bgra_ui16 ( image, i*2+1, j*2 );
                     total.R += (gan_uint32)pix.R;
                     total.G += (gan_uint32)pix.G;
                     total.B += (gan_uint32)pix.B;
                     total.A += (gan_uint32)pix.A;
                     num++;
                  }
                    
                  if ( gan_image_get_pix_b(mask,i*2+1,j*2+1) )
                  {
                     pix = gan_image_get_pix_bgra_ui16 ( image, i*2+1, j*2+1 );
                     total.R += (gan_uint32)pix.R;
                     total.G += (gan_uint32)pix.G;
                     total.B += (gan_uint32)pix.B;
                     total.A += (gan_uint32)pix.A;
                     num++;
                  }

                  if ( num >= thres )
                  {
                     pix.R = (gan_uint16) ((total.R + num/2)/num);
                     pix.G = (gan_uint16) ((total.G + num/2)/num);
                     pix.B = (gan_uint16) ((total.B + num/2)/num);
                     pix.A = (gan_uint16) ((total.A + num/2)/num);
                     gan_image_set_pix_bgra_ui16 ( himage, i, j, &pix );
                     gan_image_set_pix_gl_ui16 ( hmask, i, j, num );
                  }
               }             
      }
      else
      {
         for ( i = (int)image->height/2-1; i >= 0; i-- )
            for ( j = (int)image->width/2-1; j >= 0; j-- )
            {
               gan_uint32 mval, num=0;
               Gan_BGRAPixel_ui32 total;

               total.R = total.G = total.B = total.A = 0;

               mval = (gan_uint32) gan_image_get_pix_gl_ui16(mask,i*2,j*2);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_bgra_ui16 ( image, i*2, j*2 );
                  total.R += mval*(gan_uint32)pix.R;
                  total.G += mval*(gan_uint32)pix.G;
                  total.B += mval*(gan_uint32)pix.B;
                  total.A += mval*(gan_uint32)pix.A;
                  num += mval;
               }
                    
               mval = (gan_uint32) gan_image_get_pix_gl_ui16(mask,i*2,j*2+1);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_bgra_ui16 ( image, i*2, j*2+1 );
                  total.R += mval*(gan_uint32)pix.R;
                  total.G += mval*(gan_uint32)pix.G;
                  total.B += mval*(gan_uint32)pix.B;
                  total.A += mval*(gan_uint32)pix.A;
                  num += mval;
               }

               mval = (gan_uint32) gan_image_get_pix_gl_ui16(mask,i*2+1,j*2);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_bgra_ui16 ( image, i*2+1, j*2 );
                  total.R += mval*(gan_uint32)pix.R;
                  total.G += mval*(gan_uint32)pix.G;
                  total.B += mval*(gan_uint32)pix.B;
                  total.A += mval*(gan_uint32)pix.A;
                  num += mval;
               }
                    
               mval = (gan_uint32) gan_image_get_pix_gl_ui16(mask,i*2+1,j*2+1);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_bgra_ui16 ( image, i*2+1, j*2+1 );
                  total.R += mval*(gan_uint32)pix.R;
                  total.G += mval*(gan_uint32)pix.G;
                  total.B += mval*(gan_uint32)pix.B;
                  total.A += mval*(gan_uint32)pix.A;
                  num += mval;
               }

               if ( num >= thres )
               {
                  pix.R = (gan_uint16) ((total.R + num/2)/num);
                  pix.G = (gan_uint16) ((total.G + num/2)/num);
                  pix.B = (gan_uint16) ((total.B + num/2)/num);
                  pix.A = (gan_uint16) ((total.A + num/2)/num);
                  gan_image_set_pix_bgra_ui16 ( himage, i, j, &pix );
                  gan_image_set_pix_gl_ui16 ( hmask, i, j, num );
               }             
            }
      }
   }
   else
   {
      unsigned num;
      Gan_BGRAPixel_ui32 total;

      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
            if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
            {
               pix1 = gan_image_get_pix_bgra_ui16 ( image, i*2,   j*2  );
               pix2 = gan_image_get_pix_bgra_ui16 ( image, i*2,   j*2+1);
               pix3 = gan_image_get_pix_bgra_ui16 ( image, i*2+1, j*2  );
               pix4 = gan_image_get_pix_bgra_ui16 ( image, i*2+1, j*2+1);
               pix.R = (gan_uint16)(((gan_uint32) pix1.R + (gan_uint32) pix2.R + (gan_uint32) pix3.R + (gan_uint32) pix4.R + 2)/4);
               pix.G = (gan_uint16)(((gan_uint32) pix1.G + (gan_uint32) pix2.G + (gan_uint32) pix3.G + (gan_uint32) pix4.G + 2)/4);
               pix.B = (gan_uint16)(((gan_uint32) pix1.B + (gan_uint32) pix2.B + (gan_uint32) pix3.B + (gan_uint32) pix4.B + 2)/4);
               pix.A = (gan_uint16)(((gan_uint32) pix1.A + (gan_uint32) pix2.A + (gan_uint32) pix3.A + (gan_uint32) pix4.A + 2)/4);
               gan_image_set_pix_bgra_ui16 ( himage, i, j, &pix );
               gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
            }
            else
            {
               num = total.R = total.G = total.B = total.A = 0;
               if ( gan_image_get_pix_b(mask,i*2,j*2) )
               {
                  pix = gan_image_get_pix_bgra_ui16 ( image, i*2, j*2 );
                  total.R += (gan_uint32)pix.R;
                  total.G += (gan_uint32)pix.G;
                  total.B += (gan_uint32)pix.B;
                  total.A += (gan_uint32)pix.A;
                  num++;
               }
                    
               if ( gan_image_get_pix_b(mask,i*2,j*2+1) )
               {
                  pix = gan_image_get_pix_bgra_ui16 ( image, i*2, j*2+1 );
                  total.R += (gan_uint32)pix.R;
                  total.G += (gan_uint32)pix.G;
                  total.B += (gan_uint32)pix.B;
                  total.A += (gan_uint32)pix.A;
                  num++;
               }

               if ( gan_image_get_pix_b(mask,i*2+1,j*2) )
               {
                  pix = gan_image_get_pix_bgra_ui16 ( image, i*2+1, j*2 );
                  total.R += (gan_uint32)pix.R;
                  total.G += (gan_uint32)pix.G;
                  total.B += (gan_uint32)pix.B;
                  total.A += (gan_uint32)pix.A;
                  num++;
               }
                    
               if ( gan_image_get_pix_b(mask,i*2+1,j*2+1) )
               {
                  pix = gan_image_get_pix_bgra_ui16 ( image, i*2+1, j*2+1 );
                  total.R += (gan_uint32)pix.R;
                  total.G += (gan_uint32)pix.G;
                  total.B += (gan_uint32)pix.B;
                  total.A += (gan_uint32)pix.A;
                  num++;
               }

               if ( num >= no_neighbours )
               {
                  pix.R = (gan_uint16) ((total.R + 2)/4);
                  pix.G = (gan_uint16) ((total.G + 2)/4);
                  pix.B = (gan_uint16) ((total.B + 2)/4);
                  pix.A = (gan_uint16) ((total.A + 2)/4);
                  gan_image_set_pix_bgra_ui16 ( himage, i, j, &pix );
                  gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
               }
            }
   }

   /* success */
   return GAN_TRUE;
}

#ifdef GAN_UINT64
static Gan_Bool
 halve_size_rgba_ui32 ( Gan_Image *image, Gan_Image *mask, Gan_ImagePyramidAverage average_type,
                        unsigned no_neighbours, Gan_Image *himage, Gan_Image *hmask, gan_uint32 thres )
{
   int i, j;
   Gan_RGBAPixel_ui32 pix1, pix2, pix3, pix4, pix;

   /* if mask is NULL, other parameters are irrelevant */
   if ( mask == NULL )
   {
      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
         {
            pix1 = gan_image_get_pix_rgba_ui32 ( image, i*2,   j*2   );
            pix2 = gan_image_get_pix_rgba_ui32 ( image, i*2,   j*2+1 );
            pix3 = gan_image_get_pix_rgba_ui32 ( image, i*2+1, j*2   );
            pix4 = gan_image_get_pix_rgba_ui32 ( image, i*2+1, j*2+1 );
            pix.R = (gan_uint32)(((gan_uint64) pix1.R + (gan_uint64) pix2.R + (gan_uint64) pix3.R + (gan_uint64) pix4.R + 2)/4);
            pix.G = (gan_uint32)(((gan_uint64) pix1.G + (gan_uint64) pix2.G + (gan_uint64) pix3.G + (gan_uint64) pix4.G + 2)/4);
            pix.B = (gan_uint32)(((gan_uint64) pix1.B + (gan_uint64) pix2.B + (gan_uint64) pix3.B + (gan_uint64) pix4.B + 2)/4);
            pix.A = (gan_uint32)(((gan_uint64) pix1.A + (gan_uint64) pix2.A + (gan_uint64) pix3.A + (gan_uint64) pix4.A + 2)/4);
            gan_image_set_pix_rgba_ui32 ( himage, i, j, &pix );
         }
   }
   /* mask != NULL, action depends on other parameters */
   else if ( no_neighbours == 4 )
   {
      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
            if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
            {
               pix1 = gan_image_get_pix_rgba_ui32 ( image, i*2,   j*2  );
               pix2 = gan_image_get_pix_rgba_ui32 ( image, i*2,   j*2+1);
               pix3 = gan_image_get_pix_rgba_ui32 ( image, i*2+1, j*2  );
               pix4 = gan_image_get_pix_rgba_ui32 ( image, i*2+1, j*2+1);
               pix.R = (gan_uint32)(((gan_uint64) pix1.R + (gan_uint64) pix2.R + (gan_uint64) pix3.R + (gan_uint64) pix4.R + 2)/4);
               pix.G = (gan_uint32)(((gan_uint64) pix1.G + (gan_uint64) pix2.G + (gan_uint64) pix3.G + (gan_uint64) pix4.G + 2)/4);
               pix.B = (gan_uint32)(((gan_uint64) pix1.B + (gan_uint64) pix2.B + (gan_uint64) pix3.B + (gan_uint64) pix4.B + 2)/4);
               pix.A = (gan_uint32)(((gan_uint64) pix1.A + (gan_uint64) pix2.A + (gan_uint64) pix3.A + (gan_uint64) pix4.A + 2)/4);
               gan_image_set_pix_rgba_ui32 ( himage, i, j, &pix );
               gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
            }
   }
   else if ( average_type == GAN_AVERAGE_IN_MASK )
   {
      if ( mask->type == GAN_BOOL )
      {
         for ( i = (int)image->height/2-1; i >= 0; i-- )
            for ( j = (int)image->width/2-1; j >= 0; j-- )
               if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
               {
                  pix1 = gan_image_get_pix_rgba_ui32 ( image, i*2,   j*2  );
                  pix2 = gan_image_get_pix_rgba_ui32 ( image, i*2,   j*2+1);
                  pix3 = gan_image_get_pix_rgba_ui32 ( image, i*2+1, j*2  );
                  pix4 = gan_image_get_pix_rgba_ui32 ( image, i*2+1, j*2+1);
                  pix.R = (gan_uint32)(((gan_uint64) pix1.R + (gan_uint64) pix2.R + (gan_uint64) pix3.R + (gan_uint64) pix4.R + 2)/4);
                  pix.G = (gan_uint32)(((gan_uint64) pix1.G + (gan_uint64) pix2.G + (gan_uint64) pix3.G + (gan_uint64) pix4.G + 2)/4);
                  pix.B = (gan_uint32)(((gan_uint64) pix1.B + (gan_uint64) pix2.B + (gan_uint64) pix3.B + (gan_uint64) pix4.B + 2)/4);
                  pix.A = (gan_uint32)(((gan_uint64) pix1.A + (gan_uint64) pix2.A + (gan_uint64) pix3.A + (gan_uint64) pix4.A + 2)/4);
                  gan_image_set_pix_rgba_ui32 ( himage, i, j, &pix );
                  gan_image_set_pix_gl_ui16 ( hmask, i, j, 4 );
               }
               else
               {
                  gan_uint32 num=0;
                  Gan_RGBAPixel_ui64 total;

                  total.R = total.G = total.B = total.A = 0;
                  if ( gan_image_get_pix_b(mask,i*2,j*2) )
                  {
                     pix = gan_image_get_pix_rgba_ui32 ( image, i*2, j*2 );
                     total.R += (gan_uint64)pix.R;
                     total.G += (gan_uint64)pix.G;
                     total.B += (gan_uint64)pix.B;
                     total.A += (gan_uint64)pix.A;
                     num++;
                  }
                    
                  if ( gan_image_get_pix_b(mask,i*2,j*2+1) )
                  {
                     pix = gan_image_get_pix_rgba_ui32 ( image, i*2, j*2+1 );
                     total.R += (gan_uint64)pix.R;
                     total.G += (gan_uint64)pix.G;
                     total.B += (gan_uint64)pix.B;
                     total.A += (gan_uint64)pix.A;
                     num++;
                  }

                  if ( gan_image_get_pix_b(mask,i*2+1,j*2) )
                  {
                     pix = gan_image_get_pix_rgba_ui32 ( image, i*2+1, j*2 );
                     total.R += (gan_uint64)pix.R;
                     total.G += (gan_uint64)pix.G;
                     total.B += (gan_uint64)pix.B;
                     total.A += (gan_uint64)pix.A;
                     num++;
                  }
                    
                  if ( gan_image_get_pix_b(mask,i*2+1,j*2+1) )
                  {
                     pix = gan_image_get_pix_rgba_ui32 ( image, i*2+1, j*2+1 );
                     total.R += (gan_uint64)pix.R;
                     total.G += (gan_uint64)pix.G;
                     total.B += (gan_uint64)pix.B;
                     total.A += (gan_uint64)pix.A;
                     num++;
                  }

                  if ( num >= thres )
                  {
                     pix.R = (gan_uint32) ((total.R + num/2)/num);
                     pix.G = (gan_uint32) ((total.G + num/2)/num);
                     pix.B = (gan_uint32) ((total.B + num/2)/num);
                     pix.A = (gan_uint32) ((total.A + num/2)/num);
                     gan_image_set_pix_rgba_ui32 ( himage, i, j, &pix );
                     gan_image_set_pix_gl_ui16 ( hmask, i, j, num );
                  }
               }
      }
      else
      {
         for ( i = (int)image->height/2-1; i >= 0; i-- )
            for ( j = (int)image->width/2-1; j >= 0; j-- )
            {
               gan_uint64 mval, num=0;
               Gan_RGBAPixel_ui64 total;

               total.R = total.G = total.B = total.A = 0;

               mval = (gan_uint64) gan_image_get_pix_gl_ui16(mask,i*2,j*2);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_rgba_ui32 ( image, i*2, j*2 );
                  total.R += mval*(gan_uint64)pix.R;
                  total.G += mval*(gan_uint64)pix.G;
                  total.B += mval*(gan_uint64)pix.B;
                  total.A += mval*(gan_uint64)pix.A;
                  num += mval;
               }
                    
               mval = (gan_uint64) gan_image_get_pix_gl_ui16(mask,i*2,j*2+1);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_rgba_ui32 ( image, i*2, j*2+1 );
                  total.R += mval*(gan_uint64)pix.R;
                  total.G += mval*(gan_uint64)pix.G;
                  total.B += mval*(gan_uint64)pix.B;
                  total.A += mval*(gan_uint64)pix.A;
                  num += mval;
               }

               mval = (gan_uint64) gan_image_get_pix_gl_ui16(mask,i*2+1,j*2);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_rgba_ui32 ( image, i*2+1, j*2 );
                  total.R += mval*(gan_uint64)pix.R;
                  total.G += mval*(gan_uint64)pix.G;
                  total.B += mval*(gan_uint64)pix.B;
                  total.A += mval*(gan_uint64)pix.A;
                  num += mval;
               }
                    
               mval = (gan_uint64) gan_image_get_pix_gl_ui16(mask,i*2+1,j*2+1);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_rgba_ui32 ( image, i*2+1, j*2+1 );
                  total.R += mval*(gan_uint64)pix.R;
                  total.G += mval*(gan_uint64)pix.G;
                  total.B += mval*(gan_uint64)pix.B;
                  total.A += mval*(gan_uint64)pix.A;
                  num += mval;
               }

               if ( num >= thres )
               {
                  pix.R = (gan_uint32) ((total.R + num/2)/num);
                  pix.G = (gan_uint32) ((total.G + num/2)/num);
                  pix.B = (gan_uint32) ((total.B + num/2)/num);
                  pix.A = (gan_uint32) ((total.A + num/2)/num);
                  gan_image_set_pix_rgba_ui32 ( himage, i, j, &pix );
                  gan_image_set_pix_gl_ui16 ( hmask, i, j, num );
               }             
            }
      }
   }
   else
   {
      gan_uint64 num;
      Gan_RGBAPixel_ui32 total;

      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
            if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
            {
               pix1 = gan_image_get_pix_rgba_ui32 ( image, i*2,   j*2  );
               pix2 = gan_image_get_pix_rgba_ui32 ( image, i*2,   j*2+1);
               pix3 = gan_image_get_pix_rgba_ui32 ( image, i*2+1, j*2  );
               pix4 = gan_image_get_pix_rgba_ui32 ( image, i*2+1, j*2+1);
               pix.R = (gan_uint32)(((gan_uint64) pix1.R + (gan_uint64) pix2.R + (gan_uint64) pix3.R + (gan_uint64) pix4.R + 2)/4);
               pix.G = (gan_uint32)(((gan_uint64) pix1.G + (gan_uint64) pix2.G + (gan_uint64) pix3.G + (gan_uint64) pix4.G + 2)/4);
               pix.B = (gan_uint32)(((gan_uint64) pix1.B + (gan_uint64) pix2.B + (gan_uint64) pix3.B + (gan_uint64) pix4.B + 2)/4);
               pix.A = (gan_uint32)(((gan_uint64) pix1.A + (gan_uint64) pix2.A + (gan_uint64) pix3.A + (gan_uint64) pix4.A + 2)/4);
               gan_image_set_pix_rgba_ui32 ( himage, i, j, &pix );
               gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
            }
            else
            {
               num = total.R = total.G = total.B = total.A = 0;
               if ( gan_image_get_pix_b(mask,i*2,j*2) )
               {
                  pix = gan_image_get_pix_rgba_ui32 ( image, i*2, j*2 );
                  total.R += (gan_uint64)pix.R;
                  total.G += (gan_uint64)pix.G;
                  total.B += (gan_uint64)pix.B;
                  total.A += (gan_uint64)pix.A;
                  num++;
               }
                    
               if ( gan_image_get_pix_b(mask,i*2,j*2+1) )
               {
                  pix = gan_image_get_pix_rgba_ui32 ( image, i*2, j*2+1 );
                  total.R += (gan_uint64)pix.R;
                  total.G += (gan_uint64)pix.G;
                  total.B += (gan_uint64)pix.B;
                  total.A += (gan_uint64)pix.A;
                  num++;
               }

               if ( gan_image_get_pix_b(mask,i*2+1,j*2) )
               {
                  pix = gan_image_get_pix_rgba_ui32 ( image, i*2+1, j*2 );
                  total.R += (gan_uint64)pix.R;
                  total.G += (gan_uint64)pix.G;
                  total.B += (gan_uint64)pix.B;
                  total.A += (gan_uint64)pix.A;
                  num++;
               }
                    
               if ( gan_image_get_pix_b(mask,i*2+1,j*2+1) )
               {
                  pix = gan_image_get_pix_rgba_ui32 ( image, i*2+1, j*2+1 );
                  total.R += (gan_uint64)pix.R;
                  total.G += (gan_uint64)pix.G;
                  total.B += (gan_uint64)pix.B;
                  total.A += (gan_uint64)pix.A;
                  num++;
               }

               if ( num >= no_neighbours )
               {
                  pix.R = (gan_uint32) ((total.R + 2)/4);
                  pix.G = (gan_uint32) ((total.G + 2)/4);
                  pix.B = (gan_uint32) ((total.B + 2)/4);
                  pix.A = (gan_uint32) ((total.A + 2)/4);
                  gan_image_set_pix_rgba_ui32 ( himage, i, j, &pix );
                  gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
               }
            }
   }

   /* success */
   return GAN_TRUE;
}

static Gan_Bool
 halve_size_bgra_ui32 ( Gan_Image *image, Gan_Image *mask, Gan_ImagePyramidAverage average_type,
                        unsigned no_neighbours, Gan_Image *himage, Gan_Image *hmask, gan_uint32 thres )
{
   int i, j;
   Gan_BGRAPixel_ui32 pix1, pix2, pix3, pix4, pix;

   /* if mask is NULL, other parameters are irrelevant */
   if ( mask == NULL )
   {
      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
         {
            pix1 = gan_image_get_pix_bgra_ui32 ( image, i*2,   j*2   );
            pix2 = gan_image_get_pix_bgra_ui32 ( image, i*2,   j*2+1 );
            pix3 = gan_image_get_pix_bgra_ui32 ( image, i*2+1, j*2   );
            pix4 = gan_image_get_pix_bgra_ui32 ( image, i*2+1, j*2+1 );
            pix.R = (gan_uint32)(((gan_uint64) pix1.R + (gan_uint64) pix2.R + (gan_uint64) pix3.R + (gan_uint64) pix4.R + 2)/4);
            pix.G = (gan_uint32)(((gan_uint64) pix1.G + (gan_uint64) pix2.G + (gan_uint64) pix3.G + (gan_uint64) pix4.G + 2)/4);
            pix.B = (gan_uint32)(((gan_uint64) pix1.B + (gan_uint64) pix2.B + (gan_uint64) pix3.B + (gan_uint64) pix4.B + 2)/4);
            pix.A = (gan_uint32)(((gan_uint64) pix1.A + (gan_uint64) pix2.A + (gan_uint64) pix3.A + (gan_uint64) pix4.A + 2)/4);
            gan_image_set_pix_bgra_ui32 ( himage, i, j, &pix );
         }
   }
   /* mask != NULL, action depends on other parameters */
   else if ( no_neighbours == 4 )
   {
      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
            if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
            {
               pix1 = gan_image_get_pix_bgra_ui32 ( image, i*2,   j*2  );
               pix2 = gan_image_get_pix_bgra_ui32 ( image, i*2,   j*2+1);
               pix3 = gan_image_get_pix_bgra_ui32 ( image, i*2+1, j*2  );
               pix4 = gan_image_get_pix_bgra_ui32 ( image, i*2+1, j*2+1);
               pix.R = (gan_uint32)(((gan_uint64) pix1.R + (gan_uint64) pix2.R + (gan_uint64) pix3.R + (gan_uint64) pix4.R + 2)/4);
               pix.G = (gan_uint32)(((gan_uint64) pix1.G + (gan_uint64) pix2.G + (gan_uint64) pix3.G + (gan_uint64) pix4.G + 2)/4);
               pix.B = (gan_uint32)(((gan_uint64) pix1.B + (gan_uint64) pix2.B + (gan_uint64) pix3.B + (gan_uint64) pix4.B + 2)/4);
               pix.A = (gan_uint32)(((gan_uint64) pix1.A + (gan_uint64) pix2.A + (gan_uint64) pix3.A + (gan_uint64) pix4.A + 2)/4);
               gan_image_set_pix_bgra_ui32 ( himage, i, j, &pix );
               gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
            }
   }
   else if ( average_type == GAN_AVERAGE_IN_MASK )
   {
      if ( mask->type == GAN_BOOL )
      {
         for ( i = (int)image->height/2-1; i >= 0; i-- )
            for ( j = (int)image->width/2-1; j >= 0; j-- )
               if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
               {
                  pix1 = gan_image_get_pix_bgra_ui32 ( image, i*2,   j*2  );
                  pix2 = gan_image_get_pix_bgra_ui32 ( image, i*2,   j*2+1);
                  pix3 = gan_image_get_pix_bgra_ui32 ( image, i*2+1, j*2  );
                  pix4 = gan_image_get_pix_bgra_ui32 ( image, i*2+1, j*2+1);
                  pix.R = (gan_uint32)(((gan_uint64) pix1.R + (gan_uint64) pix2.R + (gan_uint64) pix3.R + (gan_uint64) pix4.R + 2)/4);
                  pix.G = (gan_uint32)(((gan_uint64) pix1.G + (gan_uint64) pix2.G + (gan_uint64) pix3.G + (gan_uint64) pix4.G + 2)/4);
                  pix.B = (gan_uint32)(((gan_uint64) pix1.B + (gan_uint64) pix2.B + (gan_uint64) pix3.B + (gan_uint64) pix4.B + 2)/4);
                  pix.A = (gan_uint32)(((gan_uint64) pix1.A + (gan_uint64) pix2.A + (gan_uint64) pix3.A + (gan_uint64) pix4.A + 2)/4);
                  gan_image_set_pix_bgra_ui32 ( himage, i, j, &pix );
                  gan_image_set_pix_gl_ui16 ( hmask, i, j, 4 );
               }
               else
               {
                  gan_uint32 num=0;
                  Gan_BGRAPixel_ui64 total;

                  total.R = total.G = total.B = total.A = 0;
                  if ( gan_image_get_pix_b(mask,i*2,j*2) )
                  {
                     pix = gan_image_get_pix_bgra_ui32 ( image, i*2, j*2 );
                     total.R += (gan_uint64)pix.R;
                     total.G += (gan_uint64)pix.G;
                     total.B += (gan_uint64)pix.B;
                     total.A += (gan_uint64)pix.A;
                     num++;
                  }
                    
                  if ( gan_image_get_pix_b(mask,i*2,j*2+1) )
                  {
                     pix = gan_image_get_pix_bgra_ui32 ( image, i*2, j*2+1 );
                     total.R += (gan_uint64)pix.R;
                     total.G += (gan_uint64)pix.G;
                     total.B += (gan_uint64)pix.B;
                     total.A += (gan_uint64)pix.A;
                     num++;
                  }

                  if ( gan_image_get_pix_b(mask,i*2+1,j*2) )
                  {
                     pix = gan_image_get_pix_bgra_ui32 ( image, i*2+1, j*2 );
                     total.R += (gan_uint64)pix.R;
                     total.G += (gan_uint64)pix.G;
                     total.B += (gan_uint64)pix.B;
                     total.A += (gan_uint64)pix.A;
                     num++;
                  }
                    
                  if ( gan_image_get_pix_b(mask,i*2+1,j*2+1) )
                  {
                     pix = gan_image_get_pix_bgra_ui32 ( image, i*2+1, j*2+1 );
                     total.R += (gan_uint64)pix.R;
                     total.G += (gan_uint64)pix.G;
                     total.B += (gan_uint64)pix.B;
                     total.A += (gan_uint64)pix.A;
                     num++;
                  }

                  if ( num >= thres )
                  {
                     pix.R = (gan_uint32) ((total.R + num/2)/num);
                     pix.G = (gan_uint32) ((total.G + num/2)/num);
                     pix.B = (gan_uint32) ((total.B + num/2)/num);
                     pix.A = (gan_uint32) ((total.A + num/2)/num);
                     gan_image_set_pix_bgra_ui32 ( himage, i, j, &pix );
                     gan_image_set_pix_gl_ui16 ( hmask, i, j, num );
                  }
               }
      }
      else
      {
         for ( i = (int)image->height/2-1; i >= 0; i-- )
            for ( j = (int)image->width/2-1; j >= 0; j-- )
            {
               gan_uint64 mval, num=0;
               Gan_BGRAPixel_ui64 total;

               total.R = total.G = total.B = total.A = 0;

               mval = (gan_uint64) gan_image_get_pix_gl_ui16(mask,i*2,j*2);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_bgra_ui32 ( image, i*2, j*2 );
                  total.R += mval*(gan_uint64)pix.R;
                  total.G += mval*(gan_uint64)pix.G;
                  total.B += mval*(gan_uint64)pix.B;
                  total.A += mval*(gan_uint64)pix.A;
                  num += mval;
               }
                    
               mval = (gan_uint64) gan_image_get_pix_gl_ui16(mask,i*2,j*2+1);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_bgra_ui32 ( image, i*2, j*2+1 );
                  total.R += mval*(gan_uint64)pix.R;
                  total.G += mval*(gan_uint64)pix.G;
                  total.B += mval*(gan_uint64)pix.B;
                  total.A += mval*(gan_uint64)pix.A;
                  num += mval;
               }

               mval = (gan_uint64) gan_image_get_pix_gl_ui16(mask,i*2+1,j*2);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_bgra_ui32 ( image, i*2+1, j*2 );
                  total.R += mval*(gan_uint64)pix.R;
                  total.G += mval*(gan_uint64)pix.G;
                  total.B += mval*(gan_uint64)pix.B;
                  total.A += mval*(gan_uint64)pix.A;
                  num += mval;
               }
                    
               mval = (gan_uint64) gan_image_get_pix_gl_ui16(mask,i*2+1,j*2+1);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_bgra_ui32 ( image, i*2+1, j*2+1 );
                  total.R += mval*(gan_uint64)pix.R;
                  total.G += mval*(gan_uint64)pix.G;
                  total.B += mval*(gan_uint64)pix.B;
                  total.A += mval*(gan_uint64)pix.A;
                  num += mval;
               }

               if ( num >= thres )
               {
                  pix.R = (gan_uint32) ((total.R + num/2)/num);
                  pix.G = (gan_uint32) ((total.G + num/2)/num);
                  pix.B = (gan_uint32) ((total.B + num/2)/num);
                  pix.A = (gan_uint32) ((total.A + num/2)/num);
                  gan_image_set_pix_bgra_ui32 ( himage, i, j, &pix );
                  gan_image_set_pix_gl_ui16 ( hmask, i, j, num );
               }             
            }
      }
   }
   else
   {
      gan_uint64 num;
      Gan_BGRAPixel_ui32 total;

      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
            if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
            {
               pix1 = gan_image_get_pix_bgra_ui32 ( image, i*2,   j*2  );
               pix2 = gan_image_get_pix_bgra_ui32 ( image, i*2,   j*2+1);
               pix3 = gan_image_get_pix_bgra_ui32 ( image, i*2+1, j*2  );
               pix4 = gan_image_get_pix_bgra_ui32 ( image, i*2+1, j*2+1);
               pix.R = (gan_uint32)(((gan_uint64) pix1.R + (gan_uint64) pix2.R + (gan_uint64) pix3.R + (gan_uint64) pix4.R + 2)/4);
               pix.G = (gan_uint32)(((gan_uint64) pix1.G + (gan_uint64) pix2.G + (gan_uint64) pix3.G + (gan_uint64) pix4.G + 2)/4);
               pix.B = (gan_uint32)(((gan_uint64) pix1.B + (gan_uint64) pix2.B + (gan_uint64) pix3.B + (gan_uint64) pix4.B + 2)/4);
               pix.A = (gan_uint32)(((gan_uint64) pix1.A + (gan_uint64) pix2.A + (gan_uint64) pix3.A + (gan_uint64) pix4.A + 2)/4);
               gan_image_set_pix_bgra_ui32 ( himage, i, j, &pix );
               gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
            }
            else
            {
               num = total.R = total.G = total.B = total.A = 0;
               if ( gan_image_get_pix_b(mask,i*2,j*2) )
               {
                  pix = gan_image_get_pix_bgra_ui32 ( image, i*2, j*2 );
                  total.R += (gan_uint64)pix.R;
                  total.G += (gan_uint64)pix.G;
                  total.B += (gan_uint64)pix.B;
                  total.A += (gan_uint64)pix.A;
                  num++;
               }
                    
               if ( gan_image_get_pix_b(mask,i*2,j*2+1) )
               {
                  pix = gan_image_get_pix_bgra_ui32 ( image, i*2, j*2+1 );
                  total.R += (gan_uint64)pix.R;
                  total.G += (gan_uint64)pix.G;
                  total.B += (gan_uint64)pix.B;
                  total.A += (gan_uint64)pix.A;
                  num++;
               }

               if ( gan_image_get_pix_b(mask,i*2+1,j*2) )
               {
                  pix = gan_image_get_pix_bgra_ui32 ( image, i*2+1, j*2 );
                  total.R += (gan_uint64)pix.R;
                  total.G += (gan_uint64)pix.G;
                  total.B += (gan_uint64)pix.B;
                  total.A += (gan_uint64)pix.A;
                  num++;
               }
                    
               if ( gan_image_get_pix_b(mask,i*2+1,j*2+1) )
               {
                  pix = gan_image_get_pix_bgra_ui32 ( image, i*2+1, j*2+1 );
                  total.R += (gan_uint64)pix.R;
                  total.G += (gan_uint64)pix.G;
                  total.B += (gan_uint64)pix.B;
                  total.A += (gan_uint64)pix.A;
                  num++;
               }

               if ( num >= no_neighbours )
               {
                  pix.R = (gan_uint32) ((total.R + 2)/4);
                  pix.G = (gan_uint32) ((total.G + 2)/4);
                  pix.B = (gan_uint32) ((total.B + 2)/4);
                  pix.A = (gan_uint32) ((total.A + 2)/4);
                  gan_image_set_pix_bgra_ui32 ( himage, i, j, &pix );
                  gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
               }
            }
   }

   /* success */
   return GAN_TRUE;
}
#else
/* we don't have a 64-bit integer type, so we lose a bit of precision by dividing by four before averaging */
static Gan_Bool
 halve_size_rgba_ui32 ( Gan_Image *image, Gan_Image *mask, Gan_ImagePyramidAverage average_type,
                        unsigned no_neighbours, Gan_Image *himage, Gan_Image *hmask, gan_uint32 thres )
{
   int i, j;
   Gan_RGBAPixel_ui32 pix1, pix2, pix3, pix4, pix;

   /* if mask is NULL, other parameters are irrelevant */
   if ( mask == NULL )
   {
      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
         {
            pix1 = gan_image_get_pix_rgba_ui32 ( image, i*2,   j*2   );
            pix2 = gan_image_get_pix_rgba_ui32 ( image, i*2,   j*2+1 );
            pix3 = gan_image_get_pix_rgba_ui32 ( image, i*2+1, j*2   );
            pix4 = gan_image_get_pix_rgba_ui32 ( image, i*2+1, j*2+1 );
            pix.R = pix1.R/4 + pix2.R/4 + pix3.R/4 + pix4.R/4;
            pix.G = pix1.G/4 + pix2.G/4 + pix3.G/4 + pix4.G/4;
            pix.B = pix1.B/4 + pix2.B/4 + pix3.B/4 + pix4.B/4;
            pix.A = pix1.A/4 + pix2.A/4 + pix3.A/4 + pix4.A/4;
            gan_image_set_pix_rgba_ui32 ( himage, i, j, &pix );
         }
   }
   /* mask != NULL, action depends on other parameters */
   else if ( no_neighbours == 4 )
   {
      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
            if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
            {
               pix1 = gan_image_get_pix_rgba_ui32 ( image, i*2,   j*2   );
               pix2 = gan_image_get_pix_rgba_ui32 ( image, i*2,   j*2+1 );
               pix3 = gan_image_get_pix_rgba_ui32 ( image, i*2+1, j*2   );
               pix4 = gan_image_get_pix_rgba_ui32 ( image, i*2+1, j*2+1 );
               pix.R = pix1.R/4 + pix2.R/4 + pix3.R/4 + pix4.R/4;
               pix.G = pix1.G/4 + pix2.G/4 + pix3.G/4 + pix4.G/4;
               pix.B = pix1.B/4 + pix2.B/4 + pix3.B/4 + pix4.B/4;
               pix.A = pix1.A/4 + pix2.A/4 + pix3.A/4 + pix4.A/4;
               gan_image_set_pix_rgba_ui32 ( himage, i, j, &pix );
               gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
            }
   }
   else if ( average_type == GAN_AVERAGE_IN_MASK )
   {
      if ( mask->type == GAN_BOOL )
      {
         for ( i = (int)image->height/2-1; i >= 0; i-- )
            for ( j = (int)image->width/2-1; j >= 0; j-- )
               if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
               {
                  pix1 = gan_image_get_pix_rgba_ui32 ( image, i*2,   j*2  );
                  pix2 = gan_image_get_pix_rgba_ui32 ( image, i*2,   j*2+1);
                  pix3 = gan_image_get_pix_rgba_ui32 ( image, i*2+1, j*2  );
                  pix4 = gan_image_get_pix_rgba_ui32 ( image, i*2+1, j*2+1);
                  pix.R = pix1.R/4 + pix2.R/4 + pix3.R/4 + pix4.R/4;
                  pix.G = pix1.G/4 + pix2.G/4 + pix3.G/4 + pix4.G/4;
                  pix.B = pix1.B/4 + pix2.B/4 + pix3.B/4 + pix4.B/4;
                  pix.A = pix1.A/4 + pix2.A/4 + pix3.A/4 + pix4.A/4;
                  gan_image_set_pix_rgba_ui32 ( himage, i, j, &pix );
                  gan_image_set_pix_gl_ui16 ( hmask, i, j, 4 );
               }
               else
               {
                  gan_uint32 num=0;
                  Gan_RGBAPixel_ui32 total;

                  total.R = total.G = total.B = total.A = 0;
                  if ( gan_image_get_pix_b(mask,i*2,j*2) )
                  {
                     pix = gan_image_get_pix_rgba_ui32 ( image, i*2, j*2 );
                     total.R += pix.R/4;
                     total.G += pix.G/4;
                     total.B += pix.B/4;
                     total.A += pix.A/4;
                     num++;
                  }
                    
                  if ( gan_image_get_pix_b(mask,i*2,j*2+1) )
                  {
                     pix = gan_image_get_pix_rgba_ui32 ( image, i*2, j*2+1 );
                     total.R += pix.R/4;
                     total.G += pix.G/4;
                     total.B += pix.B/4;
                     total.A += pix.A/4;
                     num++;
                  }

                  if ( gan_image_get_pix_b(mask,i*2+1,j*2) )
                  {
                     pix = gan_image_get_pix_rgba_ui32 ( image, i*2+1, j*2 );
                     total.R += pix.R/4;
                     total.G += pix.G/4;
                     total.B += pix.B/4;
                     total.A += pix.A/4;
                     num++;
                  }
                    
                  if ( gan_image_get_pix_b(mask,i*2+1,j*2+1) )
                  {
                     pix = gan_image_get_pix_rgba_ui32 ( image, i*2+1, j*2+1 );
                     total.R += pix.R/4;
                     total.G += pix.G/4;
                     total.B += pix.B/4;
                     total.A += pix.A/4;
                     num++;
                  }

                  if ( num >= thres )
                  {
                     pix.R = ((total.R)/num)*4;
                     pix.G = ((total.G)/num)*4;
                     pix.B = ((total.B)/num)*4;
                     pix.A = ((total.A)/num)*4;
                     gan_image_set_pix_rgba_ui32 ( himage, i, j, &pix );
                     gan_image_set_pix_gl_ui16 ( hmask, i, j, num );
                  }
               }
      }
      else
      {
         for ( i = (int)image->height/2-1; i >= 0; i-- )
            for ( j = (int)image->width/2-1; j >= 0; j-- )
            {
               gan_uint32 mval, num=0;
               Gan_RGBAPixel_ui32 total;

               total.R = total.G = total.B = total.A = 0;

               mval = (gan_uint32) gan_image_get_pix_gl_ui16(mask,i*2,j*2);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_rgba_ui32 ( image, i*2, j*2 );
                  total.R += mval*(pix.R >> 18);
                  total.G += mval*(pix.G >> 18);
                  total.B += mval*(pix.B >> 18);
                  total.A += mval*(pix.A >> 18);
                  num += mval;
               }
                    
               mval = (gan_uint32) gan_image_get_pix_gl_ui16(mask,i*2,j*2+1);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_rgba_ui32 ( image, i*2, j*2+1 );
                  total.R += mval*(pix.R >> 18);
                  total.G += mval*(pix.G >> 18);
                  total.B += mval*(pix.B >> 18);
                  total.A += mval*(pix.A >> 18);
                  num += mval;
               }

               mval = (gan_uint32) gan_image_get_pix_gl_ui16(mask,i*2+1,j*2);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_rgba_ui32 ( image, i*2+1, j*2 );
                  total.R += mval*(pix.R >> 18);
                  total.G += mval*(pix.G >> 18);
                  total.B += mval*(pix.B >> 18);
                  total.A += mval*(pix.A >> 18);
                  num += mval;
               }
                    
               mval = (gan_uint32) gan_image_get_pix_gl_ui16(mask,i*2+1,j*2+1);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_rgba_ui32 ( image, i*2+1, j*2+1 );
                  total.R += mval*(pix.R >> 18);
                  total.G += mval*(pix.G >> 18);
                  total.B += mval*(pix.B >> 18);
                  total.A += mval*(pix.A >> 18);
                  num += mval;
               }

               if ( num >= thres )
               {
                  pix.R = (total.R/num) << 18;
                  pix.G = (total.G/num) << 18;
                  pix.B = (total.B/num) << 18;
                  pix.A = (total.A/num) << 18;
                  gan_image_set_pix_rgba_ui32 ( himage, i, j, &pix );
                  gan_image_set_pix_gl_ui16 ( hmask, i, j, num );
               }             
            }
      }
   }
   else
   {
      unsigned num;

      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
            if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
            {
               pix1 = gan_image_get_pix_rgba_ui32 ( image, i*2,   j*2   );
               pix2 = gan_image_get_pix_rgba_ui32 ( image, i*2,   j*2+1 );
               pix3 = gan_image_get_pix_rgba_ui32 ( image, i*2+1, j*2   );
               pix4 = gan_image_get_pix_rgba_ui32 ( image, i*2+1, j*2+1 );
               pix.R = pix1.R/4 + pix2.R/4 + pix3.R/4 + pix4.R/4;
               pix.G = pix1.G/4 + pix2.G/4 + pix3.G/4 + pix4.G/4;
               pix.B = pix1.B/4 + pix2.B/4 + pix3.B/4 + pix4.B/4;
               pix.A = pix1.A/4 + pix2.A/4 + pix3.A/4 + pix4.A/4;
               gan_image_set_pix_rgba_ui32 ( himage, i, j, &pix );
               gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
            }
            else
            {
               num = pix.R = pix.G = pix.B = pix.A = 0;
               if ( gan_image_get_pix_b(mask,i*2,j*2) )
               {
                  pix = gan_image_get_pix_rgba_ui32 ( image, i*2, j*2 );
                  pix.R += pix.R/4;
                  pix.G += pix.G/4;
                  pix.B += pix.B/4;
                  pix.A += pix.A/4;
                  num++;
               }
                    
               if ( gan_image_get_pix_b(mask,i*2,j*2+1) )
               {
                  pix = gan_image_get_pix_rgba_ui32 ( image, i*2, j*2+1 );
                  pix.R += pix.R/4;
                  pix.G += pix.G/4;
                  pix.B += pix.B/4;
                  pix.A += pix.A/4;
                  num++;
               }

               if ( gan_image_get_pix_b(mask,i*2+1,j*2) )
               {
                  pix = gan_image_get_pix_rgba_ui32 ( image, i*2+1, j*2 );
                  pix.R += pix.R/4;
                  pix.G += pix.G/4;
                  pix.B += pix.B/4;
                  pix.A += pix.A/4;
                  num++;
               }
                    
               if ( gan_image_get_pix_b(mask,i*2+1,j*2+1) )
               {
                  pix = gan_image_get_pix_rgba_ui32 ( image, i*2+1, j*2+1 );
                  pix.R += pix.R/4;
                  pix.G += pix.G/4;
                  pix.B += pix.B/4;
                  pix.A += pix.A/4;
                  num++;
               }

               if ( num >= no_neighbours )
               {
                  gan_image_set_pix_rgba_ui32 ( himage, i, j, &pix );
                  gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
               }
            }
   }

   /* success */
   return GAN_TRUE;
}

/* we don't have a 64-bit integer type, so we lose a bit of precision by dividing by four before averaging */
static Gan_Bool
 halve_size_bgra_ui32 ( Gan_Image *image, Gan_Image *mask, Gan_ImagePyramidAverage average_type,
                        unsigned no_neighbours, Gan_Image *himage, Gan_Image *hmask, gan_uint32 thres )
{
   int i, j;
   Gan_BGRAPixel_ui32 pix1, pix2, pix3, pix4, pix;

   /* if mask is NULL, other parameters are irrelevant */
   if ( mask == NULL )
   {
      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
         {
            pix1 = gan_image_get_pix_bgra_ui32 ( image, i*2,   j*2   );
            pix2 = gan_image_get_pix_bgra_ui32 ( image, i*2,   j*2+1 );
            pix3 = gan_image_get_pix_bgra_ui32 ( image, i*2+1, j*2   );
            pix4 = gan_image_get_pix_bgra_ui32 ( image, i*2+1, j*2+1 );
            pix.R = pix1.R/4 + pix2.R/4 + pix3.R/4 + pix4.R/4;
            pix.G = pix1.G/4 + pix2.G/4 + pix3.G/4 + pix4.G/4;
            pix.B = pix1.B/4 + pix2.B/4 + pix3.B/4 + pix4.B/4;
            pix.A = pix1.A/4 + pix2.A/4 + pix3.A/4 + pix4.A/4;
            gan_image_set_pix_bgra_ui32 ( himage, i, j, &pix );
         }
   }
   /* mask != NULL, action depends on other parameters */
   else if ( no_neighbours == 4 )
   {
      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
            if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
            {
               pix1 = gan_image_get_pix_bgra_ui32 ( image, i*2,   j*2   );
               pix2 = gan_image_get_pix_bgra_ui32 ( image, i*2,   j*2+1 );
               pix3 = gan_image_get_pix_bgra_ui32 ( image, i*2+1, j*2   );
               pix4 = gan_image_get_pix_bgra_ui32 ( image, i*2+1, j*2+1 );
               pix.R = pix1.R/4 + pix2.R/4 + pix3.R/4 + pix4.R/4;
               pix.G = pix1.G/4 + pix2.G/4 + pix3.G/4 + pix4.G/4;
               pix.B = pix1.B/4 + pix2.B/4 + pix3.B/4 + pix4.B/4;
               pix.A = pix1.A/4 + pix2.A/4 + pix3.A/4 + pix4.A/4;
               gan_image_set_pix_bgra_ui32 ( himage, i, j, &pix );
               gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
            }
   }
   else if ( average_type == GAN_AVERAGE_IN_MASK )
   {
      if ( mask->type == GAN_BOOL )
      {
         for ( i = (int)image->height/2-1; i >= 0; i-- )
            for ( j = (int)image->width/2-1; j >= 0; j-- )
               if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
               {
                  pix1 = gan_image_get_pix_bgra_ui32 ( image, i*2,   j*2  );
                  pix2 = gan_image_get_pix_bgra_ui32 ( image, i*2,   j*2+1);
                  pix3 = gan_image_get_pix_bgra_ui32 ( image, i*2+1, j*2  );
                  pix4 = gan_image_get_pix_bgra_ui32 ( image, i*2+1, j*2+1);
                  pix.R = pix1.R/4 + pix2.R/4 + pix3.R/4 + pix4.R/4;
                  pix.G = pix1.G/4 + pix2.G/4 + pix3.G/4 + pix4.G/4;
                  pix.B = pix1.B/4 + pix2.B/4 + pix3.B/4 + pix4.B/4;
                  pix.A = pix1.A/4 + pix2.A/4 + pix3.A/4 + pix4.A/4;
                  gan_image_set_pix_bgra_ui32 ( himage, i, j, &pix );
                  gan_image_set_pix_gl_ui16 ( hmask, i, j, 4 );
               }
               else
               {
                  gan_uint32 num=0;
                  Gan_BGRAPixel_ui32 total;

                  total.R = total.G = total.B = total.A = 0;
                  if ( gan_image_get_pix_b(mask,i*2,j*2) )
                  {
                     pix = gan_image_get_pix_bgra_ui32 ( image, i*2, j*2 );
                     total.R += pix.R/4;
                     total.G += pix.G/4;
                     total.B += pix.B/4;
                     total.A += pix.A/4;
                     num++;
                  }
                    
                  if ( gan_image_get_pix_b(mask,i*2,j*2+1) )
                  {
                     pix = gan_image_get_pix_bgra_ui32 ( image, i*2, j*2+1 );
                     total.R += pix.R/4;
                     total.G += pix.G/4;
                     total.B += pix.B/4;
                     total.A += pix.A/4;
                     num++;
                  }

                  if ( gan_image_get_pix_b(mask,i*2+1,j*2) )
                  {
                     pix = gan_image_get_pix_bgra_ui32 ( image, i*2+1, j*2 );
                     total.R += pix.R/4;
                     total.G += pix.G/4;
                     total.B += pix.B/4;
                     total.A += pix.A/4;
                     num++;
                  }
                    
                  if ( gan_image_get_pix_b(mask,i*2+1,j*2+1) )
                  {
                     pix = gan_image_get_pix_bgra_ui32 ( image, i*2+1, j*2+1 );
                     total.R += pix.R/4;
                     total.G += pix.G/4;
                     total.B += pix.B/4;
                     total.A += pix.A/4;
                     num++;
                  }

                  if ( num >= thres )
                  {
                     pix.R = ((total.R)/num)*4;
                     pix.G = ((total.G)/num)*4;
                     pix.B = ((total.B)/num)*4;
                     pix.A = ((total.A)/num)*4;
                     gan_image_set_pix_bgra_ui32 ( himage, i, j, &pix );
                     gan_image_set_pix_gl_ui16 ( hmask, i, j, num );
                  }
               }
      }
      else
      {
         for ( i = (int)image->height/2-1; i >= 0; i-- )
            for ( j = (int)image->width/2-1; j >= 0; j-- )
            {
               gan_uint32 mval, num=0;
               Gan_BGRAPixel_ui32 total;

               total.R = total.G = total.B = total.A = 0;

               mval = (gan_uint32) gan_image_get_pix_gl_ui16(mask,i*2,j*2);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_bgra_ui32 ( image, i*2, j*2 );
                  total.R += mval*(pix.R >> 18);
                  total.G += mval*(pix.G >> 18);
                  total.B += mval*(pix.B >> 18);
                  total.A += mval*(pix.A >> 18);
                  num += mval;
               }
                    
               mval = (gan_uint32) gan_image_get_pix_gl_ui16(mask,i*2,j*2+1);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_bgra_ui32 ( image, i*2, j*2+1 );
                  total.R += mval*(pix.R >> 18);
                  total.G += mval*(pix.G >> 18);
                  total.B += mval*(pix.B >> 18);
                  total.A += mval*(pix.A >> 18);
                  num += mval;
               }

               mval = (gan_uint32) gan_image_get_pix_gl_ui16(mask,i*2+1,j*2);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_bgra_ui32 ( image, i*2+1, j*2 );
                  total.R += mval*(pix.R >> 18);
                  total.G += mval*(pix.G >> 18);
                  total.B += mval*(pix.B >> 18);
                  total.A += mval*(pix.A >> 18);
                  num += mval;
               }
                    
               mval = (gan_uint32) gan_image_get_pix_gl_ui16(mask,i*2+1,j*2+1);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_bgra_ui32 ( image, i*2+1, j*2+1 );
                  total.R += mval*(pix.R >> 18);
                  total.G += mval*(pix.G >> 18);
                  total.B += mval*(pix.B >> 18);
                  total.A += mval*(pix.A >> 18);
                  num += mval;
               }

               if ( num >= thres )
               {
                  pix.R = (total.R/num) << 18;
                  pix.G = (total.G/num) << 18;
                  pix.B = (total.B/num) << 18;
                  pix.A = (total.A/num) << 18;
                  gan_image_set_pix_bgra_ui32 ( himage, i, j, &pix );
                  gan_image_set_pix_gl_ui16 ( hmask, i, j, num );
               }             
            }
      }
   }
   else
   {
      unsigned num;

      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
            if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
            {
               pix1 = gan_image_get_pix_bgra_ui32 ( image, i*2,   j*2   );
               pix2 = gan_image_get_pix_bgra_ui32 ( image, i*2,   j*2+1 );
               pix3 = gan_image_get_pix_bgra_ui32 ( image, i*2+1, j*2   );
               pix4 = gan_image_get_pix_bgra_ui32 ( image, i*2+1, j*2+1 );
               pix.R = pix1.R/4 + pix2.R/4 + pix3.R/4 + pix4.R/4;
               pix.G = pix1.G/4 + pix2.G/4 + pix3.G/4 + pix4.G/4;
               pix.B = pix1.B/4 + pix2.B/4 + pix3.B/4 + pix4.B/4;
               pix.A = pix1.A/4 + pix2.A/4 + pix3.A/4 + pix4.A/4;
               gan_image_set_pix_bgra_ui32 ( himage, i, j, &pix );
               gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
            }
            else
            {
               num = pix.R = pix.G = pix.B = pix.A = 0;
               if ( gan_image_get_pix_b(mask,i*2,j*2) )
               {
                  pix = gan_image_get_pix_bgra_ui32 ( image, i*2, j*2 );
                  pix.R += pix.R/4;
                  pix.G += pix.G/4;
                  pix.B += pix.B/4;
                  pix.A += pix.A/4;
                  num++;
               }
                    
               if ( gan_image_get_pix_b(mask,i*2,j*2+1) )
               {
                  pix = gan_image_get_pix_bgra_ui32 ( image, i*2, j*2+1 );
                  pix.R += pix.R/4;
                  pix.G += pix.G/4;
                  pix.B += pix.B/4;
                  pix.A += pix.A/4;
                  num++;
               }

               if ( gan_image_get_pix_b(mask,i*2+1,j*2) )
               {
                  pix = gan_image_get_pix_bgra_ui32 ( image, i*2+1, j*2 );
                  pix.R += pix.R/4;
                  pix.G += pix.G/4;
                  pix.B += pix.B/4;
                  pix.A += pix.A/4;
                  num++;
               }
                    
               if ( gan_image_get_pix_b(mask,i*2+1,j*2+1) )
               {
                  pix = gan_image_get_pix_bgra_ui32 ( image, i*2+1, j*2+1 );
                  pix.R += pix.R/4;
                  pix.G += pix.G/4;
                  pix.B += pix.B/4;
                  pix.A += pix.A/4;
                  num++;
               }

               if ( num >= no_neighbours )
               {
                  gan_image_set_pix_bgra_ui32 ( himage, i, j, &pix );
                  gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
               }
            }
   }

   /* success */
   return GAN_TRUE;
}
#endif /* #ifdef GAN_UINT64 */

static Gan_Bool
 halve_size_rgba_f32 ( Gan_Image *image, Gan_Image *mask, Gan_ImagePyramidAverage average_type,
                       unsigned no_neighbours, Gan_Image *himage, Gan_Image *hmask, gan_uint32 thres )
{
   int i, j;
   Gan_RGBAPixel_f32 pix1, pix2, pix3, pix4, pix;

   /* if mask is NULL, other parameters are irrelevant */
   if ( mask == NULL )
   {
      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
         {
            pix1 = gan_image_get_pix_rgba_f32 ( image, i*2,   j*2   );
            pix2 = gan_image_get_pix_rgba_f32 ( image, i*2,   j*2+1 );
            pix3 = gan_image_get_pix_rgba_f32 ( image, i*2+1, j*2   );
            pix4 = gan_image_get_pix_rgba_f32 ( image, i*2+1, j*2+1 );
            pix.R = 0.25F*(pix1.R + pix2.R + pix3.R + pix4.R);
            pix.G = 0.25F*(pix1.G + pix2.G + pix3.G + pix4.G);
            pix.B = 0.25F*(pix1.B + pix2.B + pix3.B + pix4.B);
            pix.A = 0.25F*(pix1.A + pix2.A + pix3.A + pix4.A);
            gan_image_set_pix_rgba_f32 ( himage, i, j, &pix );
         }
   }
   /* mask != NULL, action depends on other parameters */
   else if ( no_neighbours == 4 )
   {
      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
            if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
            {
               pix1 = gan_image_get_pix_rgba_f32 ( image, i*2,   j*2   );
               pix2 = gan_image_get_pix_rgba_f32 ( image, i*2,   j*2+1 );
               pix3 = gan_image_get_pix_rgba_f32 ( image, i*2+1, j*2   );
               pix4 = gan_image_get_pix_rgba_f32 ( image, i*2+1, j*2+1 );
               pix.R = 0.25F*(pix1.R + pix2.R + pix3.R + pix4.R);
               pix.G = 0.25F*(pix1.G + pix2.G + pix3.G + pix4.G);
               pix.B = 0.25F*(pix1.B + pix2.B + pix3.B + pix4.B);
               pix.A = 0.25F*(pix1.A + pix2.A + pix3.A + pix4.A);
               gan_image_set_pix_rgba_f32 ( himage, i, j, &pix );
               gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
            }
   }
   else if ( average_type == GAN_AVERAGE_IN_MASK )
   {
      if ( mask->type == GAN_BOOL )
      {
         for ( i = (int)image->height/2-1; i >= 0; i-- )
            for ( j = (int)image->width/2-1; j >= 0; j-- )
               if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
               {
                  pix1 = gan_image_get_pix_rgba_f32 ( image, i*2,   j*2   );
                  pix2 = gan_image_get_pix_rgba_f32 ( image, i*2,   j*2+1 );
                  pix3 = gan_image_get_pix_rgba_f32 ( image, i*2+1, j*2   );
                  pix4 = gan_image_get_pix_rgba_f32 ( image, i*2+1, j*2+1 );
                  pix.R = 0.25F*(pix1.R + pix2.R + pix3.R + pix4.R);
                  pix.G = 0.25F*(pix1.G + pix2.G + pix3.G + pix4.G);
                  pix.B = 0.25F*(pix1.B + pix2.B + pix3.B + pix4.B);
                  pix.A = 0.25F*(pix1.A + pix2.A + pix3.A + pix4.A);
                  gan_image_set_pix_rgba_f32 ( himage, i, j, &pix );
                  gan_image_set_pix_gl_ui16 ( hmask, i, j, 4 );
               }
               else
               {
                  gan_uint16 num=0;
                  Gan_RGBAPixel_f32 total;

                  total.R = total.G = total.B = total.A = 0.0F;
                  if ( gan_image_get_pix_b(mask,i*2,j*2) )
                  {
                     pix = gan_image_get_pix_rgba_f32 ( image, i*2, j*2 );
                     total.R += pix.R;
                     total.G += pix.G;
                     total.B += pix.B;
                     total.A += pix.A;
                     num++;
                  }
                    
                  if ( gan_image_get_pix_b(mask,i*2,j*2+1) )
                  {
                     pix = gan_image_get_pix_rgba_f32 ( image, i*2, j*2+1 );
                     total.R += pix.R;
                     total.G += pix.G;
                     total.B += pix.B;
                     total.A += pix.A;
                     num++;
                  }

                  if ( gan_image_get_pix_b(mask,i*2+1,j*2) )
                  {
                     pix = gan_image_get_pix_rgba_f32 ( image, i*2+1, j*2 );
                     total.R += pix.R;
                     total.G += pix.G;
                     total.B += pix.B;
                     total.A += pix.A;
                     num++;
                  }
                    
                  if ( gan_image_get_pix_b(mask,i*2+1,j*2+1) )
                  {
                     pix = gan_image_get_pix_rgba_f32 ( image, i*2+1, j*2+1 );
                     total.R += pix.R;
                     total.G += pix.G;
                     total.B += pix.B;
                     total.A += pix.A;
                     num++;
                  }

                  if ( num >= thres )
                  {
                     gan_float32 fnum = (gan_float32)num;
                     pix.R = total.R/fnum;
                     pix.G = total.G/fnum;
                     pix.B = total.B/fnum;
                     pix.A = total.A/fnum;
                     gan_image_set_pix_rgba_f32 ( himage, i, j, &pix );
                     gan_image_set_pix_gl_ui16 ( hmask, i, j, num );
                  }
               }             
      }
      else
      {
         for ( i = (int)image->height/2-1; i >= 0; i-- )
            for ( j = (int)image->width/2-1; j >= 0; j-- )
            {
               gan_uint16 mval, num=0;
               float fmval;
               Gan_RGBAPixel_f32 total;

               total.R = total.G = total.B = total.A = 0.0F;

               mval = gan_image_get_pix_gl_ui16(mask,i*2,j*2);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_rgba_f32 ( image, i*2, j*2 );
                  fmval = (gan_float32)mval;
                  total.R += fmval*pix.R;
                  total.G += fmval*pix.G;
                  total.B += fmval*pix.B;
                  total.A += fmval*pix.A;
                  num += mval;
               }

               mval = gan_image_get_pix_gl_ui16(mask,i*2,j*2+1);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_rgba_f32 ( image, i*2, j*2+1 );
                  fmval = (gan_float32)mval;
                  total.R += fmval*pix.R;
                  total.G += fmval*pix.G;
                  total.B += fmval*pix.B;
                  total.A += fmval*pix.A;
                  num += mval;
               }

               mval = gan_image_get_pix_gl_ui16(mask,i*2+1,j*2);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_rgba_f32 ( image, i*2+1, j*2 );
                  fmval = (gan_float32)mval;
                  total.R += fmval*pix.R;
                  total.G += fmval*pix.G;
                  total.B += fmval*pix.B;
                  total.A += fmval*pix.A;
                  num += mval;
               }
                    
               mval = gan_image_get_pix_gl_ui16(mask,i*2+1,j*2+1);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_rgba_f32 ( image, i*2+1, j*2+1 );
                  fmval = (gan_float32)mval;
                  total.R += fmval*pix.R;
                  total.G += fmval*pix.G;
                  total.B += fmval*pix.B;
                  total.A += fmval*pix.A;
                  num += mval;
               }

               if ( num >= thres )
               {
                  gan_float32 fnum = (gan_float32)num;
                  pix.R = total.R/fnum;
                  pix.G = total.G/fnum;
                  pix.B = total.B/fnum;
                  pix.A = total.A/fnum;
                  gan_image_set_pix_rgba_f32 ( himage, i, j, &pix );
                  gan_image_set_pix_gl_ui16 ( hmask, i, j, num );
               }             
            }
      }
   }
   else
   {
      unsigned num;

      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
            if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
            {
               pix1 = gan_image_get_pix_rgba_f32 ( image, i*2,   j*2   );
               pix2 = gan_image_get_pix_rgba_f32 ( image, i*2,   j*2+1 );
               pix3 = gan_image_get_pix_rgba_f32 ( image, i*2+1, j*2   );
               pix4 = gan_image_get_pix_rgba_f32 ( image, i*2+1, j*2+1 );
               pix.R = 0.25F*(pix1.R + pix2.R + pix3.R + pix4.R);
               pix.G = 0.25F*(pix1.G + pix2.G + pix3.G + pix4.G);
               pix.B = 0.25F*(pix1.B + pix2.B + pix3.B + pix4.B);
               pix.A = 0.25F*(pix1.A + pix2.A + pix3.A + pix4.A);
               gan_image_set_pix_rgba_f32 ( himage, i, j, &pix );
               gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
            }
            else
            {
               num = 0; pix.R = pix.G = pix.B = pix.A = 0.0F;
               if ( gan_image_get_pix_b(mask,i*2,j*2) )
               {
                  pix = gan_image_get_pix_rgba_f32 ( image, i*2, j*2 );
                  pix.R += pix.R;
                  pix.G += pix.G;
                  pix.B += pix.B;
                  pix.A += pix.A;
                  num++;
               }
                    
               if ( gan_image_get_pix_b(mask,i*2,j*2+1) )
               {
                  pix = gan_image_get_pix_rgba_f32 ( image, i*2, j*2+1 );
                  pix.R += pix.R;
                  pix.G += pix.G;
                  pix.B += pix.B;
                  pix.A += pix.A;
                  num++;
               }

               if ( gan_image_get_pix_b(mask,i*2+1,j*2) )
               {
                  pix = gan_image_get_pix_rgba_f32 ( image, i*2+1, j*2 );
                  pix.R += pix.R;
                  pix.G += pix.G;
                  pix.B += pix.B;
                  pix.A += pix.A;
                  num++;
               }
                    
               if ( gan_image_get_pix_b(mask,i*2+1,j*2+1) )
               {
                  pix = gan_image_get_pix_rgba_f32 ( image, i*2+1, j*2+1 );
                  pix.R += pix.R;
                  pix.G += pix.G;
                  pix.B += pix.B;
                  pix.A += pix.A;
                  num++;
               }

               if ( num >= no_neighbours )
               {
                  pix.R *= 0.25F;
                  pix.G *= 0.25F;
                  pix.B *= 0.25F;
                  pix.A *= 0.25F;
                  gan_image_set_pix_rgba_f32 ( himage, i, j, &pix );
                  gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
               }
            }
   }

   /* success */
   return GAN_TRUE;
}

static Gan_Bool
 halve_size_rgba_f64 ( Gan_Image *image, Gan_Image *mask, Gan_ImagePyramidAverage average_type,
                    unsigned no_neighbours, Gan_Image *himage, Gan_Image *hmask, gan_uint32 thres )
{
   int i, j;
   Gan_RGBAPixel_f64 pix1, pix2, pix3, pix4, pix;

   /* if mask is NULL, other parameters are irrelevant */
   if ( mask == NULL )
   {
      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
         {
            pix1 = gan_image_get_pix_rgba_f64 ( image, i*2,   j*2   );
            pix2 = gan_image_get_pix_rgba_f64 ( image, i*2,   j*2+1 );
            pix3 = gan_image_get_pix_rgba_f64 ( image, i*2+1, j*2   );
            pix4 = gan_image_get_pix_rgba_f64 ( image, i*2+1, j*2+1 );
            pix.R = 0.25*(pix1.R + pix2.R + pix3.R + pix4.R);
            pix.G = 0.25*(pix1.G + pix2.G + pix3.G + pix4.G);
            pix.B = 0.25*(pix1.B + pix2.B + pix3.B + pix4.B);
            pix.A = 0.25*(pix1.A + pix2.A + pix3.A + pix4.A);
            gan_image_set_pix_rgba_f64 ( himage, i, j, &pix );
         }
   }
   /* mask != NULL, action depends on other parameters */
   else if ( no_neighbours == 4 )
   {
      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
            if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
            {
               pix1 = gan_image_get_pix_rgba_f64 ( image, i*2,   j*2   );
               pix2 = gan_image_get_pix_rgba_f64 ( image, i*2,   j*2+1 );
               pix3 = gan_image_get_pix_rgba_f64 ( image, i*2+1, j*2   );
               pix4 = gan_image_get_pix_rgba_f64 ( image, i*2+1, j*2+1 );
               pix.R = 0.25*(pix1.R + pix2.R + pix3.R + pix4.R);
               pix.G = 0.25*(pix1.G + pix2.G + pix3.G + pix4.G);
               pix.B = 0.25*(pix1.B + pix2.B + pix3.B + pix4.B);
               pix.A = 0.25*(pix1.A + pix2.A + pix3.A + pix4.A);
               gan_image_set_pix_rgba_f64 ( himage, i, j, &pix );
               gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
            }
   }
   else if ( average_type == GAN_AVERAGE_IN_MASK )
   {
      if ( mask->type == GAN_BOOL )
      {
         for ( i = (int)image->height/2-1; i >= 0; i-- )
            for ( j = (int)image->width/2-1; j >= 0; j-- )
               if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
               {
                  pix1 = gan_image_get_pix_rgba_f64 ( image, i*2,   j*2   );
                  pix2 = gan_image_get_pix_rgba_f64 ( image, i*2,   j*2+1 );
                  pix3 = gan_image_get_pix_rgba_f64 ( image, i*2+1, j*2   );
                  pix4 = gan_image_get_pix_rgba_f64 ( image, i*2+1, j*2+1 );
                  pix.R = 0.25F*(pix1.R + pix2.R + pix3.R + pix4.R);
                  pix.G = 0.25F*(pix1.G + pix2.G + pix3.G + pix4.G);
                  pix.B = 0.25F*(pix1.B + pix2.B + pix3.B + pix4.B);
                  pix.A = 0.25F*(pix1.A + pix2.A + pix3.A + pix4.A);
                  gan_image_set_pix_rgba_f64 ( himage, i, j, &pix );
                  gan_image_set_pix_gl_ui16 ( hmask, i, j, 4 );
               }
               else
               {
                  gan_uint16 num=0;
                  Gan_RGBAPixel_f64 total;

                  total.R = total.G = total.B = total.A = 0.0;
                  if ( gan_image_get_pix_b(mask,i*2,j*2) )
                  {
                     pix = gan_image_get_pix_rgba_f64 ( image, i*2, j*2 );
                     total.R += pix.R;
                     total.G += pix.G;
                     total.B += pix.B;
                     total.A += pix.A;
                     num++;
                  }
                    
                  if ( gan_image_get_pix_b(mask,i*2,j*2+1) )
                  {
                     pix = gan_image_get_pix_rgba_f64 ( image, i*2, j*2+1 );
                     total.R += pix.R;
                     total.G += pix.G;
                     total.B += pix.B;
                     total.A += pix.A;
                     num++;
                  }

                  if ( gan_image_get_pix_b(mask,i*2+1,j*2) )
                  {
                     pix = gan_image_get_pix_rgba_f64 ( image, i*2+1, j*2 );
                     total.R += pix.R;
                     total.G += pix.G;
                     total.B += pix.B;
                     total.A += pix.A;
                     num++;
                  }
                    
                  if ( gan_image_get_pix_b(mask,i*2+1,j*2+1) )
                  {
                     pix = gan_image_get_pix_rgba_f64 ( image, i*2+1, j*2+1 );
                     total.R += pix.R;
                     total.G += pix.G;
                     total.B += pix.B;
                     total.A += pix.A;
                     num++;
                  }

                  if ( num >= thres )
                  {
                     gan_float64 fnum = (gan_float64)num;
                     pix.R = total.R/fnum;
                     pix.G = total.G/fnum;
                     pix.B = total.B/fnum;
                     pix.A = total.A/fnum;
                     gan_image_set_pix_rgba_f64 ( himage, i, j, &pix );
                     gan_image_set_pix_gl_ui16 ( hmask, i, j, num );
                  }
               }             
      }
      else
      {
         for ( i = (int)image->height/2-1; i >= 0; i-- )
            for ( j = (int)image->width/2-1; j >= 0; j-- )
            {
               gan_uint16 mval, num=0;
               gan_float64 fmval;
               Gan_RGBAPixel_f64 total;

               total.R = total.G = total.B = total.A = 0.0;

               mval = gan_image_get_pix_gl_ui16(mask,i*2,j*2);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_rgba_f64 ( image, i*2, j*2 );
                  fmval = (gan_float64)mval;
                  total.R += fmval*pix.R;
                  total.G += fmval*pix.G;
                  total.B += fmval*pix.B;
                  total.A += fmval*pix.A;
                  num += mval;
               }

               mval = gan_image_get_pix_gl_ui16(mask,i*2,j*2+1);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_rgba_f64 ( image, i*2, j*2+1 );
                  fmval = (gan_float64)mval;
                  total.R += fmval*pix.R;
                  total.G += fmval*pix.G;
                  total.B += fmval*pix.B;
                  total.A += fmval*pix.A;
                  num += mval;
               }

               mval = gan_image_get_pix_gl_ui16(mask,i*2+1,j*2);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_rgba_f64 ( image, i*2+1, j*2 );
                  fmval = (gan_float64)mval;
                  total.R += fmval*pix.R;
                  total.G += fmval*pix.G;
                  total.B += fmval*pix.B;
                  total.A += fmval*pix.A;
                  num += mval;
               }
                    
               mval = gan_image_get_pix_gl_ui16(mask,i*2+1,j*2+1);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_rgba_f64 ( image, i*2+1, j*2+1 );
                  fmval = (gan_float64)mval;
                  total.R += fmval*pix.R;
                  total.G += fmval*pix.G;
                  total.B += fmval*pix.B;
                  total.A += fmval*pix.A;
                  num += mval;
               }

               if ( num >= thres )
               {
                  gan_float64 fnum = (gan_float64)num;
                  pix.R = total.R/fnum;
                  pix.G = total.G/fnum;
                  pix.B = total.B/fnum;
                  pix.A = total.A/fnum;
                  gan_image_set_pix_rgba_f64 ( himage, i, j, &pix );
                  gan_image_set_pix_gl_ui16 ( hmask, i, j, num );
               }             
            }
      }
   }
   else
   {
      unsigned num;

      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
            if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
            {
               pix1 = gan_image_get_pix_rgba_f64 ( image, i*2,   j*2   );
               pix2 = gan_image_get_pix_rgba_f64 ( image, i*2,   j*2+1 );
               pix3 = gan_image_get_pix_rgba_f64 ( image, i*2+1, j*2   );
               pix4 = gan_image_get_pix_rgba_f64 ( image, i*2+1, j*2+1 );
               pix.R = 0.25*(pix1.R + pix2.R + pix3.R + pix4.R);
               pix.G = 0.25*(pix1.G + pix2.G + pix3.G + pix4.G);
               pix.B = 0.25*(pix1.B + pix2.B + pix3.B + pix4.B);
               pix.A = 0.25*(pix1.A + pix2.A + pix3.A + pix4.A);
               gan_image_set_pix_rgba_f64 ( himage, i, j, &pix );
               gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
            }
            else
            {
               num = 0; pix.R = pix.G = pix.B = pix.A = 0.0;
               if ( gan_image_get_pix_b(mask,i*2,j*2) )
               {
                  pix = gan_image_get_pix_rgba_f64 ( image, i*2, j*2 );
                  pix.R += pix.R;
                  pix.G += pix.G;
                  pix.B += pix.B;
                  pix.A += pix.A;
                  num++;
               }
                    
               if ( gan_image_get_pix_b(mask,i*2,j*2+1) )
               {
                  pix = gan_image_get_pix_rgba_f64 ( image, i*2, j*2+1 );
                  pix.R += pix.R;
                  pix.G += pix.G;
                  pix.B += pix.B;
                  pix.A += pix.A;
                  num++;
               }

               if ( gan_image_get_pix_b(mask,i*2+1,j*2) )
               {
                  pix = gan_image_get_pix_rgba_f64 ( image, i*2+1, j*2 );
                  pix.R += pix.R;
                  pix.G += pix.G;
                  pix.B += pix.B;
                  pix.A += pix.A;
                  num++;
               }
                    
               if ( gan_image_get_pix_b(mask,i*2+1,j*2+1) )
               {
                  pix = gan_image_get_pix_rgba_f64 ( image, i*2+1, j*2+1 );
                  pix.R += pix.R;
                  pix.G += pix.G;
                  pix.B += pix.B;
                  pix.A += pix.A;
                  num++;
               }

               if ( num >= no_neighbours )
               {
                  pix.R *= 0.25;
                  pix.G *= 0.25;
                  pix.B *= 0.25;
                  pix.A *= 0.25;
                  gan_image_set_pix_rgba_f64 ( himage, i, j, &pix );
                  gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
               }
            }
   }

   /* success */
   return GAN_TRUE;
}

static Gan_Bool
 halve_size_bgra_f32 ( Gan_Image *image, Gan_Image *mask, Gan_ImagePyramidAverage average_type,
                       unsigned no_neighbours, Gan_Image *himage, Gan_Image *hmask, gan_uint32 thres )
{
   int i, j;
   Gan_BGRAPixel_f32 pix1, pix2, pix3, pix4, pix;

   /* if mask is NULL, other parameters are irrelevant */
   if ( mask == NULL )
   {
      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
         {
            pix1 = gan_image_get_pix_bgra_f32 ( image, i*2,   j*2   );
            pix2 = gan_image_get_pix_bgra_f32 ( image, i*2,   j*2+1 );
            pix3 = gan_image_get_pix_bgra_f32 ( image, i*2+1, j*2   );
            pix4 = gan_image_get_pix_bgra_f32 ( image, i*2+1, j*2+1 );
            pix.R = 0.25F*(pix1.R + pix2.R + pix3.R + pix4.R);
            pix.G = 0.25F*(pix1.G + pix2.G + pix3.G + pix4.G);
            pix.B = 0.25F*(pix1.B + pix2.B + pix3.B + pix4.B);
            pix.A = 0.25F*(pix1.A + pix2.A + pix3.A + pix4.A);
            gan_image_set_pix_bgra_f32 ( himage, i, j, &pix );
         }
   }
   /* mask != NULL, action depends on other parameters */
   else if ( no_neighbours == 4 )
   {
      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
            if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
            {
               pix1 = gan_image_get_pix_bgra_f32 ( image, i*2,   j*2   );
               pix2 = gan_image_get_pix_bgra_f32 ( image, i*2,   j*2+1 );
               pix3 = gan_image_get_pix_bgra_f32 ( image, i*2+1, j*2   );
               pix4 = gan_image_get_pix_bgra_f32 ( image, i*2+1, j*2+1 );
               pix.R = 0.25F*(pix1.R + pix2.R + pix3.R + pix4.R);
               pix.G = 0.25F*(pix1.G + pix2.G + pix3.G + pix4.G);
               pix.B = 0.25F*(pix1.B + pix2.B + pix3.B + pix4.B);
               pix.A = 0.25F*(pix1.A + pix2.A + pix3.A + pix4.A);
               gan_image_set_pix_bgra_f32 ( himage, i, j, &pix );
               gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
            }
   }
   else if ( average_type == GAN_AVERAGE_IN_MASK )
   {
      if ( mask->type == GAN_BOOL )
      {
         for ( i = (int)image->height/2-1; i >= 0; i-- )
            for ( j = (int)image->width/2-1; j >= 0; j-- )
               if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
               {
                  pix1 = gan_image_get_pix_bgra_f32 ( image, i*2,   j*2   );
                  pix2 = gan_image_get_pix_bgra_f32 ( image, i*2,   j*2+1 );
                  pix3 = gan_image_get_pix_bgra_f32 ( image, i*2+1, j*2   );
                  pix4 = gan_image_get_pix_bgra_f32 ( image, i*2+1, j*2+1 );
                  pix.R = 0.25F*(pix1.R + pix2.R + pix3.R + pix4.R);
                  pix.G = 0.25F*(pix1.G + pix2.G + pix3.G + pix4.G);
                  pix.B = 0.25F*(pix1.B + pix2.B + pix3.B + pix4.B);
                  pix.A = 0.25F*(pix1.A + pix2.A + pix3.A + pix4.A);
                  gan_image_set_pix_bgra_f32 ( himage, i, j, &pix );
                  gan_image_set_pix_gl_ui16 ( hmask, i, j, 4 );
               }
               else
               {
                  gan_uint16 num=0;
                  Gan_BGRAPixel_f32 total;

                  total.R = total.G = total.B = total.A = 0.0F;
                  if ( gan_image_get_pix_b(mask,i*2,j*2) )
                  {
                     pix = gan_image_get_pix_bgra_f32 ( image, i*2, j*2 );
                     total.R += pix.R;
                     total.G += pix.G;
                     total.B += pix.B;
                     total.A += pix.A;
                     num++;
                  }
                    
                  if ( gan_image_get_pix_b(mask,i*2,j*2+1) )
                  {
                     pix = gan_image_get_pix_bgra_f32 ( image, i*2, j*2+1 );
                     total.R += pix.R;
                     total.G += pix.G;
                     total.B += pix.B;
                     total.A += pix.A;
                     num++;
                  }

                  if ( gan_image_get_pix_b(mask,i*2+1,j*2) )
                  {
                     pix = gan_image_get_pix_bgra_f32 ( image, i*2+1, j*2 );
                     total.R += pix.R;
                     total.G += pix.G;
                     total.B += pix.B;
                     total.A += pix.A;
                     num++;
                  }
                    
                  if ( gan_image_get_pix_b(mask,i*2+1,j*2+1) )
                  {
                     pix = gan_image_get_pix_bgra_f32 ( image, i*2+1, j*2+1 );
                     total.R += pix.R;
                     total.G += pix.G;
                     total.B += pix.B;
                     total.A += pix.A;
                     num++;
                  }

                  if ( num >= thres )
                  {
                     gan_float32 fnum = (gan_float32)num;
                     pix.R = total.R/fnum;
                     pix.G = total.G/fnum;
                     pix.B = total.B/fnum;
                     pix.A = total.A/fnum;
                     gan_image_set_pix_bgra_f32 ( himage, i, j, &pix );
                     gan_image_set_pix_gl_ui16 ( hmask, i, j, num );
                  }
               }             
      }
      else
      {
         for ( i = (int)image->height/2-1; i >= 0; i-- )
            for ( j = (int)image->width/2-1; j >= 0; j-- )
            {
               gan_uint16 mval, num=0;
               float fmval;
               Gan_BGRAPixel_f32 total;

               total.R = total.G = total.B = total.A = 0.0F;

               mval = gan_image_get_pix_gl_ui16(mask,i*2,j*2);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_bgra_f32 ( image, i*2, j*2 );
                  fmval = (gan_float32)mval;
                  total.R += fmval*pix.R;
                  total.G += fmval*pix.G;
                  total.B += fmval*pix.B;
                  total.A += fmval*pix.A;
                  num += mval;
               }

               mval = gan_image_get_pix_gl_ui16(mask,i*2,j*2+1);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_bgra_f32 ( image, i*2, j*2+1 );
                  fmval = (gan_float32)mval;
                  total.R += fmval*pix.R;
                  total.G += fmval*pix.G;
                  total.B += fmval*pix.B;
                  total.A += fmval*pix.A;
                  num += mval;
               }

               mval = gan_image_get_pix_gl_ui16(mask,i*2+1,j*2);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_bgra_f32 ( image, i*2+1, j*2 );
                  fmval = (gan_float32)mval;
                  total.R += fmval*pix.R;
                  total.G += fmval*pix.G;
                  total.B += fmval*pix.B;
                  total.A += fmval*pix.A;
                  num += mval;
               }
                    
               mval = gan_image_get_pix_gl_ui16(mask,i*2+1,j*2+1);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_bgra_f32 ( image, i*2+1, j*2+1 );
                  fmval = (gan_float32)mval;
                  total.R += fmval*pix.R;
                  total.G += fmval*pix.G;
                  total.B += fmval*pix.B;
                  total.A += fmval*pix.A;
                  num += mval;
               }

               if ( num >= thres )
               {
                  gan_float32 fnum = (gan_float32)num;
                  pix.R = total.R/fnum;
                  pix.G = total.G/fnum;
                  pix.B = total.B/fnum;
                  pix.A = total.A/fnum;
                  gan_image_set_pix_bgra_f32 ( himage, i, j, &pix );
                  gan_image_set_pix_gl_ui16 ( hmask, i, j, num );
               }             
            }
      }
   }
   else
   {
      unsigned num;

      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
            if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
            {
               pix1 = gan_image_get_pix_bgra_f32 ( image, i*2,   j*2   );
               pix2 = gan_image_get_pix_bgra_f32 ( image, i*2,   j*2+1 );
               pix3 = gan_image_get_pix_bgra_f32 ( image, i*2+1, j*2   );
               pix4 = gan_image_get_pix_bgra_f32 ( image, i*2+1, j*2+1 );
               pix.R = 0.25F*(pix1.R + pix2.R + pix3.R + pix4.R);
               pix.G = 0.25F*(pix1.G + pix2.G + pix3.G + pix4.G);
               pix.B = 0.25F*(pix1.B + pix2.B + pix3.B + pix4.B);
               pix.A = 0.25F*(pix1.A + pix2.A + pix3.A + pix4.A);
               gan_image_set_pix_bgra_f32 ( himage, i, j, &pix );
               gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
            }
            else
            {
               num = 0; pix.R = pix.G = pix.B = pix.A = 0.0F;
               if ( gan_image_get_pix_b(mask,i*2,j*2) )
               {
                  pix = gan_image_get_pix_bgra_f32 ( image, i*2, j*2 );
                  pix.R += pix.R;
                  pix.G += pix.G;
                  pix.B += pix.B;
                  pix.A += pix.A;
                  num++;
               }
                    
               if ( gan_image_get_pix_b(mask,i*2,j*2+1) )
               {
                  pix = gan_image_get_pix_bgra_f32 ( image, i*2, j*2+1 );
                  pix.R += pix.R;
                  pix.G += pix.G;
                  pix.B += pix.B;
                  pix.A += pix.A;
                  num++;
               }

               if ( gan_image_get_pix_b(mask,i*2+1,j*2) )
               {
                  pix = gan_image_get_pix_bgra_f32 ( image, i*2+1, j*2 );
                  pix.R += pix.R;
                  pix.G += pix.G;
                  pix.B += pix.B;
                  pix.A += pix.A;
                  num++;
               }
                    
               if ( gan_image_get_pix_b(mask,i*2+1,j*2+1) )
               {
                  pix = gan_image_get_pix_bgra_f32 ( image, i*2+1, j*2+1 );
                  pix.R += pix.R;
                  pix.G += pix.G;
                  pix.B += pix.B;
                  pix.A += pix.A;
                  num++;
               }

               if ( num >= no_neighbours )
               {
                  pix.R *= 0.25F;
                  pix.G *= 0.25F;
                  pix.B *= 0.25F;
                  pix.A *= 0.25F;
                  gan_image_set_pix_bgra_f32 ( himage, i, j, &pix );
                  gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
               }
            }
   }

   /* success */
   return GAN_TRUE;
}

static Gan_Bool
 halve_size_bgra_f64 ( Gan_Image *image, Gan_Image *mask, Gan_ImagePyramidAverage average_type,
                    unsigned no_neighbours, Gan_Image *himage, Gan_Image *hmask, gan_uint32 thres )
{
   int i, j;
   Gan_BGRAPixel_f64 pix1, pix2, pix3, pix4, pix;

   /* if mask is NULL, other parameters are irrelevant */
   if ( mask == NULL )
   {
      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
         {
            pix1 = gan_image_get_pix_bgra_f64 ( image, i*2,   j*2   );
            pix2 = gan_image_get_pix_bgra_f64 ( image, i*2,   j*2+1 );
            pix3 = gan_image_get_pix_bgra_f64 ( image, i*2+1, j*2   );
            pix4 = gan_image_get_pix_bgra_f64 ( image, i*2+1, j*2+1 );
            pix.R = 0.25*(pix1.R + pix2.R + pix3.R + pix4.R);
            pix.G = 0.25*(pix1.G + pix2.G + pix3.G + pix4.G);
            pix.B = 0.25*(pix1.B + pix2.B + pix3.B + pix4.B);
            pix.A = 0.25*(pix1.A + pix2.A + pix3.A + pix4.A);
            gan_image_set_pix_bgra_f64 ( himage, i, j, &pix );
         }
   }
   /* mask != NULL, action depends on other parameters */
   else if ( no_neighbours == 4 )
   {
      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
            if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
            {
               pix1 = gan_image_get_pix_bgra_f64 ( image, i*2,   j*2   );
               pix2 = gan_image_get_pix_bgra_f64 ( image, i*2,   j*2+1 );
               pix3 = gan_image_get_pix_bgra_f64 ( image, i*2+1, j*2   );
               pix4 = gan_image_get_pix_bgra_f64 ( image, i*2+1, j*2+1 );
               pix.R = 0.25*(pix1.R + pix2.R + pix3.R + pix4.R);
               pix.G = 0.25*(pix1.G + pix2.G + pix3.G + pix4.G);
               pix.B = 0.25*(pix1.B + pix2.B + pix3.B + pix4.B);
               pix.A = 0.25*(pix1.A + pix2.A + pix3.A + pix4.A);
               gan_image_set_pix_bgra_f64 ( himage, i, j, &pix );
               gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
            }
   }
   else if ( average_type == GAN_AVERAGE_IN_MASK )
   {
      if ( mask->type == GAN_BOOL )
      {
         for ( i = (int)image->height/2-1; i >= 0; i-- )
            for ( j = (int)image->width/2-1; j >= 0; j-- )
               if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
               {
                  pix1 = gan_image_get_pix_bgra_f64 ( image, i*2,   j*2   );
                  pix2 = gan_image_get_pix_bgra_f64 ( image, i*2,   j*2+1 );
                  pix3 = gan_image_get_pix_bgra_f64 ( image, i*2+1, j*2   );
                  pix4 = gan_image_get_pix_bgra_f64 ( image, i*2+1, j*2+1 );
                  pix.R = 0.25F*(pix1.R + pix2.R + pix3.R + pix4.R);
                  pix.G = 0.25F*(pix1.G + pix2.G + pix3.G + pix4.G);
                  pix.B = 0.25F*(pix1.B + pix2.B + pix3.B + pix4.B);
                  pix.A = 0.25F*(pix1.A + pix2.A + pix3.A + pix4.A);
                  gan_image_set_pix_bgra_f64 ( himage, i, j, &pix );
                  gan_image_set_pix_gl_ui16 ( hmask, i, j, 4 );
               }
               else
               {
                  gan_uint16 num=0;
                  Gan_BGRAPixel_f64 total;

                  total.R = total.G = total.B = total.A = 0.0;
                  if ( gan_image_get_pix_b(mask,i*2,j*2) )
                  {
                     pix = gan_image_get_pix_bgra_f64 ( image, i*2, j*2 );
                     total.R += pix.R;
                     total.G += pix.G;
                     total.B += pix.B;
                     total.A += pix.A;
                     num++;
                  }
                    
                  if ( gan_image_get_pix_b(mask,i*2,j*2+1) )
                  {
                     pix = gan_image_get_pix_bgra_f64 ( image, i*2, j*2+1 );
                     total.R += pix.R;
                     total.G += pix.G;
                     total.B += pix.B;
                     total.A += pix.A;
                     num++;
                  }

                  if ( gan_image_get_pix_b(mask,i*2+1,j*2) )
                  {
                     pix = gan_image_get_pix_bgra_f64 ( image, i*2+1, j*2 );
                     total.R += pix.R;
                     total.G += pix.G;
                     total.B += pix.B;
                     total.A += pix.A;
                     num++;
                  }
                    
                  if ( gan_image_get_pix_b(mask,i*2+1,j*2+1) )
                  {
                     pix = gan_image_get_pix_bgra_f64 ( image, i*2+1, j*2+1 );
                     total.R += pix.R;
                     total.G += pix.G;
                     total.B += pix.B;
                     total.A += pix.A;
                     num++;
                  }

                  if ( num >= thres )
                  {
                     gan_float64 fnum = (gan_float64)num;
                     pix.R = total.R/fnum;
                     pix.G = total.G/fnum;
                     pix.B = total.B/fnum;
                     pix.A = total.A/fnum;
                     gan_image_set_pix_bgra_f64 ( himage, i, j, &pix );
                     gan_image_set_pix_gl_ui16 ( hmask, i, j, num );
                  }
               }             
      }
      else
      {
         for ( i = (int)image->height/2-1; i >= 0; i-- )
            for ( j = (int)image->width/2-1; j >= 0; j-- )
            {
               gan_uint16 mval, num=0;
               gan_float64 fmval;
               Gan_BGRAPixel_f64 total;

               total.R = total.G = total.B = total.A = 0.0;

               mval = gan_image_get_pix_gl_ui16(mask,i*2,j*2);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_bgra_f64 ( image, i*2, j*2 );
                  fmval = (gan_float64)mval;
                  total.R += fmval*pix.R;
                  total.G += fmval*pix.G;
                  total.B += fmval*pix.B;
                  total.A += fmval*pix.A;
                  num += mval;
               }

               mval = gan_image_get_pix_gl_ui16(mask,i*2,j*2+1);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_bgra_f64 ( image, i*2, j*2+1 );
                  fmval = (gan_float64)mval;
                  total.R += fmval*pix.R;
                  total.G += fmval*pix.G;
                  total.B += fmval*pix.B;
                  total.A += fmval*pix.A;
                  num += mval;
               }

               mval = gan_image_get_pix_gl_ui16(mask,i*2+1,j*2);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_bgra_f64 ( image, i*2+1, j*2 );
                  fmval = (gan_float64)mval;
                  total.R += fmval*pix.R;
                  total.G += fmval*pix.G;
                  total.B += fmval*pix.B;
                  total.A += fmval*pix.A;
                  num += mval;
               }
                    
               mval = gan_image_get_pix_gl_ui16(mask,i*2+1,j*2+1);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_bgra_f64 ( image, i*2+1, j*2+1 );
                  fmval = (gan_float64)mval;
                  total.R += fmval*pix.R;
                  total.G += fmval*pix.G;
                  total.B += fmval*pix.B;
                  total.A += fmval*pix.A;
                  num += mval;
               }

               if ( num >= thres )
               {
                  gan_float64 fnum = (gan_float64)num;
                  pix.R = total.R/fnum;
                  pix.G = total.G/fnum;
                  pix.B = total.B/fnum;
                  pix.A = total.A/fnum;
                  gan_image_set_pix_bgra_f64 ( himage, i, j, &pix );
                  gan_image_set_pix_gl_ui16 ( hmask, i, j, num );
               }             
            }
      }
   }
   else
   {
      unsigned num;

      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
            if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
            {
               pix1 = gan_image_get_pix_bgra_f64 ( image, i*2,   j*2   );
               pix2 = gan_image_get_pix_bgra_f64 ( image, i*2,   j*2+1 );
               pix3 = gan_image_get_pix_bgra_f64 ( image, i*2+1, j*2   );
               pix4 = gan_image_get_pix_bgra_f64 ( image, i*2+1, j*2+1 );
               pix.R = 0.25*(pix1.R + pix2.R + pix3.R + pix4.R);
               pix.G = 0.25*(pix1.G + pix2.G + pix3.G + pix4.G);
               pix.B = 0.25*(pix1.B + pix2.B + pix3.B + pix4.B);
               pix.A = 0.25*(pix1.A + pix2.A + pix3.A + pix4.A);
               gan_image_set_pix_bgra_f64 ( himage, i, j, &pix );
               gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
            }
            else
            {
               num = 0; pix.R = pix.G = pix.B = pix.A = 0.0;
               if ( gan_image_get_pix_b(mask,i*2,j*2) )
               {
                  pix = gan_image_get_pix_bgra_f64 ( image, i*2, j*2 );
                  pix.R += pix.R;
                  pix.G += pix.G;
                  pix.B += pix.B;
                  pix.A += pix.A;
                  num++;
               }
                    
               if ( gan_image_get_pix_b(mask,i*2,j*2+1) )
               {
                  pix = gan_image_get_pix_bgra_f64 ( image, i*2, j*2+1 );
                  pix.R += pix.R;
                  pix.G += pix.G;
                  pix.B += pix.B;
                  pix.A += pix.A;
                  num++;
               }

               if ( gan_image_get_pix_b(mask,i*2+1,j*2) )
               {
                  pix = gan_image_get_pix_bgra_f64 ( image, i*2+1, j*2 );
                  pix.R += pix.R;
                  pix.G += pix.G;
                  pix.B += pix.B;
                  pix.A += pix.A;
                  num++;
               }
                    
               if ( gan_image_get_pix_b(mask,i*2+1,j*2+1) )
               {
                  pix = gan_image_get_pix_bgra_f64 ( image, i*2+1, j*2+1 );
                  pix.R += pix.R;
                  pix.G += pix.G;
                  pix.B += pix.B;
                  pix.A += pix.A;
                  num++;
               }

               if ( num >= no_neighbours )
               {
                  pix.R *= 0.25;
                  pix.G *= 0.25;
                  pix.B *= 0.25;
                  pix.A *= 0.25;
                  gan_image_set_pix_bgra_f64 ( himage, i, j, &pix );
                  gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
               }
            }
   }

   /* success */
   return GAN_TRUE;
}

static Gan_Bool
 halve_size_vfield2D_i16 ( Gan_Image *image, Gan_Image *mask, Gan_ImagePyramidAverage average_type,
                           unsigned no_neighbours, Gan_Image *himage, Gan_Image *hmask, gan_uint32 thres )
{
   int i, j;
   Gan_Vector2_i16 pix1, pix2, pix3, pix4, pix;

   /* if mask is NULL, other parameters are irrelevant */
   if ( mask == NULL )
   {
      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
         {
            pix1 = gan_image_get_pix_vfield2D_i16 ( image, i*2,   j*2   );
            pix2 = gan_image_get_pix_vfield2D_i16 ( image, i*2,   j*2+1 );
            pix3 = gan_image_get_pix_vfield2D_i16 ( image, i*2+1, j*2   );
            pix4 = gan_image_get_pix_vfield2D_i16 ( image, i*2+1, j*2+1 );
            pix.x = (gan_int16) (((gan_int32) pix1.x + (gan_int32) pix2.x + (gan_int32) pix3.x + (gan_int32) pix4.x + 2)/4);
            pix.y = (gan_int16) (((gan_int32) pix1.y + (gan_int32) pix2.y + (gan_int32) pix3.y + (gan_int32) pix4.y + 2)/4);
            gan_image_set_pix_vfield2D_i16 ( himage, i, j, &pix );
         }
   }
   /* mask != NULL, action depends on other parameters */
   else if ( no_neighbours == 4 )
   {
      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
            if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
            {
               pix1 = gan_image_get_pix_vfield2D_i16 ( image, i*2,   j*2   );
               pix2 = gan_image_get_pix_vfield2D_i16 ( image, i*2,   j*2+1 );
               pix3 = gan_image_get_pix_vfield2D_i16 ( image, i*2+1, j*2   );
               pix4 = gan_image_get_pix_vfield2D_i16 ( image, i*2+1, j*2+1 );
               pix.x = (gan_int16) (((gan_int32) pix1.x + (gan_int32) pix2.x + (gan_int32) pix3.x + (gan_int32) pix4.x + 2)/4);
               pix.y = (gan_int16) (((gan_int32) pix1.y + (gan_int32) pix2.y + (gan_int32) pix3.y + (gan_int32) pix4.y + 2)/4);
               gan_image_set_pix_vfield2D_i16 ( himage, i, j, &pix );
               gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
            }
   }
   else if ( average_type == GAN_AVERAGE_IN_MASK )
   {
      if ( mask->type == GAN_BOOL )
      {
         for ( i = (int)image->height/2-1; i >= 0; i-- )
            for ( j = (int)image->width/2-1; j >= 0; j-- )
               if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
               {
                  pix1 = gan_image_get_pix_vfield2D_i16 ( image, i*2,   j*2   );
                  pix2 = gan_image_get_pix_vfield2D_i16 ( image, i*2,   j*2+1 );
                  pix3 = gan_image_get_pix_vfield2D_i16 ( image, i*2+1, j*2   );
                  pix4 = gan_image_get_pix_vfield2D_i16 ( image, i*2+1, j*2+1 );
                  pix.x = (gan_int16) (((gan_int32) pix1.x + (gan_int32) pix2.x + (gan_int32) pix3.x + (gan_int32) pix4.x + 2)/4);
                  pix.y = (gan_int16) (((gan_int32) pix1.y + (gan_int32) pix2.y + (gan_int32) pix3.y + (gan_int32) pix4.y + 2)/4);
                  gan_image_set_pix_vfield2D_i16 ( himage, i, j, &pix );
                  gan_image_set_pix_gl_ui16 ( hmask, i, j, 4 );
               }
               else
               {
                  gan_uint16 num=0;
                  Gan_Vector2_i32 total;

                  total.x = total.y = 0;
                  if ( gan_image_get_pix_b(mask,i*2,j*2) )
                  {
                     pix = gan_image_get_pix_vfield2D_i16 ( image, i*2, j*2 );
                     total.x += (gan_int32)pix.x;
                     total.y += (gan_int32)pix.y;
                     num++;
                  }
                    
                  if ( gan_image_get_pix_b(mask,i*2,j*2+1) )
                  {
                     pix = gan_image_get_pix_vfield2D_i16 ( image, i*2, j*2+1 );
                     total.x += (gan_int32)pix.x;
                     total.y += (gan_int32)pix.y;
                     num++;
                  }

                  if ( gan_image_get_pix_b(mask,i*2+1,j*2) )
                  {
                     pix = gan_image_get_pix_vfield2D_i16 ( image, i*2+1, j*2 );
                     total.x += (gan_int32)pix.x;
                     total.y += (gan_int32)pix.y;
                     num++;
                  }
                    
                  if ( gan_image_get_pix_b(mask,i*2+1,j*2+1) )
                  {
                     pix = gan_image_get_pix_vfield2D_i16 ( image, i*2+1, j*2+1 );
                     total.x += (gan_int32)pix.x;
                     total.y += (gan_int32)pix.y;
                     num++;
                  }

                  if ( num >= thres )
                  {
                     pix.x = (gan_int16) ((total.x + num/2)/num);
                     pix.y = (gan_int16) ((total.y + num/2)/num);
                     gan_image_set_pix_vfield2D_i16 ( himage, i, j, &pix );
                     gan_image_set_pix_gl_ui16 ( hmask, i, j, num );
                  }
               }
      }
      else
      {
         for ( i = (int)image->height/2-1; i >= 0; i-- )
            for ( j = (int)image->width/2-1; j >= 0; j-- )
            {
               gan_int32 mval, num=0;
               Gan_Vector2_i32 total;

               total.x = total.y = 0;

               mval = (gan_int32) gan_image_get_pix_gl_ui16(mask,i*2,j*2);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_vfield2D_i16 ( image, i*2, j*2 );
                  total.x += mval*(gan_int32)pix.x;
                  total.y += mval*(gan_int32)pix.y;
                  num += mval;
               }
                    
               mval = (gan_int32) gan_image_get_pix_gl_ui16(mask,i*2,j*2+1);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_vfield2D_i16 ( image, i*2, j*2+1 );
                  total.x += mval*(gan_int32)pix.x;
                  total.y += mval*(gan_int32)pix.y;
                  num += mval;
               }

               mval = (gan_int32) gan_image_get_pix_gl_ui16(mask,i*2+1,j*2);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_vfield2D_i16 ( image, i*2+1, j*2 );
                  total.x += mval*(gan_int32)pix.x;
                  total.y += mval*(gan_int32)pix.y;
                  num += mval;
               }
                    
               mval = (gan_int32) gan_image_get_pix_gl_ui16(mask,i*2+1,j*2+1);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_vfield2D_i16 ( image, i*2+1, j*2+1 );
                  total.x += mval*(gan_int32)pix.x;
                  total.y += mval*(gan_int32)pix.y;
                  num += mval;
               }

               if ( (gan_uint32)num >= thres )
               {
                  pix.x = (gan_int16) ((total.x + num/2)/num);
                  pix.y = (gan_int16) ((total.y + num/2)/num);
                  gan_image_set_pix_vfield2D_i16 ( himage, i, j, &pix );
                  gan_image_set_pix_gl_ui16 ( hmask, i, j, num );
               }             
            }
      }
   }
   else
   {
      int num;
      Gan_Vector2_i32 total;

      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
            if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
            {
               pix1 = gan_image_get_pix_vfield2D_i16 ( image, i*2,   j*2   );
               pix2 = gan_image_get_pix_vfield2D_i16 ( image, i*2,   j*2+1 );
               pix3 = gan_image_get_pix_vfield2D_i16 ( image, i*2+1, j*2   );
               pix4 = gan_image_get_pix_vfield2D_i16 ( image, i*2+1, j*2+1 );
               pix.x = (gan_int16) (((gan_int32) pix1.x + (gan_int32) pix2.x + (gan_int32) pix3.x + (gan_int32) pix4.x + 2)/4);
               pix.y = (gan_int16) (((gan_int32) pix1.y + (gan_int32) pix2.y + (gan_int32) pix3.y + (gan_int32) pix4.y + 2)/4);
               gan_image_set_pix_vfield2D_i16 ( himage, i, j, &pix );
               gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
            }
            else
            {
               num = total.x = total.y = 0;
               if ( gan_image_get_pix_b(mask,i*2,j*2) )
               {
                  pix = gan_image_get_pix_vfield2D_i16 ( image, i*2, j*2 );
                  total.x += (gan_int32)pix.x;
                  total.y += (gan_int32)pix.y;
                  num++;
               }
                    
               if ( gan_image_get_pix_b(mask,i*2,j*2+1) )
               {
                  pix = gan_image_get_pix_vfield2D_i16 ( image, i*2, j*2+1 );
                  total.x += (gan_int32)pix.x;
                  total.y += (gan_int32)pix.y;
                  num++;
               }

               if ( gan_image_get_pix_b(mask,i*2+1,j*2) )
               {
                  pix = gan_image_get_pix_vfield2D_i16 ( image, i*2+1, j*2 );
                  total.x += (gan_int32)pix.x;
                  total.y += (gan_int32)pix.y;
                  num++;
               }
                    
               if ( gan_image_get_pix_b(mask,i*2+1,j*2+1) )
               {
                  pix = gan_image_get_pix_vfield2D_i16 ( image, i*2+1, j*2+1 );
                  total.x += (gan_int32)pix.x;
                  total.y += (gan_int32)pix.y;
                  num++;
               }

               if ( (gan_uint32)num >= no_neighbours )
               {
                  if ( average_type == GAN_AVERAGE_IN_MASK )
                  {
                     pix.x = (gan_int16) ((total.x + num/2)/num);
                     pix.y = (gan_int16) ((total.y + num/2)/num);
                  }
                  else /* average_type == GAN_AVERAGE_ALL */
                  {
                     pix.x = (gan_int16) ((total.x + 2)/4);
                     pix.y = (gan_int16) ((total.y + 2)/4);
                  }

                  gan_image_set_pix_vfield2D_i16 ( himage, i, j, &pix );
                  gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
               }
            }
   }

   /* success */
   return GAN_TRUE;
}

static Gan_Bool
 halve_size_vfield2D_f32 ( Gan_Image *image, Gan_Image *mask, Gan_ImagePyramidAverage average_type,
                           unsigned no_neighbours, Gan_Image *himage, Gan_Image *hmask, gan_uint32 thres )
{
   int i, j;
   Gan_Vector2_f32 pix1, pix2, pix3, pix4, pix;

   /* if mask is NULL, other parameters are irrelevant */
   if ( mask == NULL )
   {
      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
         {
            pix1 = gan_image_get_pix_vfield2D_f32 ( image, i*2,   j*2   );
            pix2 = gan_image_get_pix_vfield2D_f32 ( image, i*2,   j*2+1 );
            pix3 = gan_image_get_pix_vfield2D_f32 ( image, i*2+1, j*2   );
            pix4 = gan_image_get_pix_vfield2D_f32 ( image, i*2+1, j*2+1 );
            pix.x = 0.25F*(pix1.x + pix2.x + pix3.x + pix4.x);
            pix.y = 0.25F*(pix1.y + pix2.y + pix3.y + pix4.y);
            gan_image_set_pix_vfield2D_f32 ( himage, i, j, &pix );
         }
   }
   /* mask != NULL, action depends on other parameters */
   else if ( no_neighbours == 4 )
   {
      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
            if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
            {
               pix1 = gan_image_get_pix_vfield2D_f32 ( image, i*2,   j*2   );
               pix2 = gan_image_get_pix_vfield2D_f32 ( image, i*2,   j*2+1 );
               pix3 = gan_image_get_pix_vfield2D_f32 ( image, i*2+1, j*2   );
               pix4 = gan_image_get_pix_vfield2D_f32 ( image, i*2+1, j*2+1 );
               pix.x = 0.25F*(pix1.x + pix2.x + pix3.x + pix4.x);
               pix.y = 0.25F*(pix1.y + pix2.y + pix3.y + pix4.y);
               gan_image_set_pix_vfield2D_f32 ( himage, i, j, &pix );
               gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
            }
   }
   else if ( average_type == GAN_AVERAGE_IN_MASK )
   {
      if ( mask->type == GAN_BOOL )
      {
         for ( i = (int)image->height/2-1; i >= 0; i-- )
            for ( j = (int)image->width/2-1; j >= 0; j-- )
               if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
               {
                  pix1 = gan_image_get_pix_vfield2D_f32 ( image, i*2,   j*2   );
                  pix2 = gan_image_get_pix_vfield2D_f32 ( image, i*2,   j*2+1 );
                  pix3 = gan_image_get_pix_vfield2D_f32 ( image, i*2+1, j*2   );
                  pix4 = gan_image_get_pix_vfield2D_f32 ( image, i*2+1, j*2+1 );
                  pix.x = 0.25F*(pix1.x + pix2.x + pix3.x + pix4.x);
                  pix.y = 0.25F*(pix1.y + pix2.y + pix3.y + pix4.y);
                  gan_image_set_pix_vfield2D_f32 ( himage, i, j, &pix );
                  gan_image_set_pix_gl_ui16 ( hmask, i, j, 4 );
               }
               else
               {
                  gan_uint16 num=0;
                  Gan_Vector2_f32 total;

                  total.x = total.y = 0.0F;
                  if ( gan_image_get_pix_b(mask,i*2,j*2) )
                  {
                     pix = gan_image_get_pix_vfield2D_f32 ( image, i*2, j*2 );
                     total.x += pix.x;
                     total.y += pix.y;
                     num++;
                  }
                    
                  if ( gan_image_get_pix_b(mask,i*2,j*2+1) )
                  {
                     pix = gan_image_get_pix_vfield2D_f32 ( image, i*2, j*2+1 );
                     total.x += pix.x;
                     total.y += pix.y;
                     num++;
                  }

                  if ( gan_image_get_pix_b(mask,i*2+1,j*2) )
                  {
                     pix = gan_image_get_pix_vfield2D_f32 ( image, i*2+1, j*2 );
                     total.x += pix.x;
                     total.y += pix.y;
                     num++;
                  }
                    
                  if ( gan_image_get_pix_b(mask,i*2+1,j*2+1) )
                  {
                     pix = gan_image_get_pix_vfield2D_f32 ( image, i*2+1, j*2+1 );
                     total.x += pix.x;
                     total.y += pix.y;
                     num++;
                  }

                  if ( num >= thres )
                  {
                     gan_float32 fnum = (gan_float32)num;
                     pix.x = total.x/fnum;
                     pix.y = total.y/fnum;
                     gan_image_set_pix_vfield2D_f32 ( himage, i, j, &pix );
                     gan_image_set_pix_gl_ui16 ( hmask, i, j, num );
                  }
               }             
      }
      else
      {
         for ( i = (int)image->height/2-1; i >= 0; i-- )
            for ( j = (int)image->width/2-1; j >= 0; j-- )
            {
               gan_uint16 mval, num=0;
               float fmval;
               Gan_Vector2_f32 total;

               total.x = total.y = 0.0F;

               mval = gan_image_get_pix_gl_ui16(mask,i*2,j*2);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_vfield2D_f32 ( image, i*2, j*2 );
                  fmval = (gan_float32)mval;
                  total.x += fmval*pix.x;
                  total.y += fmval*pix.y;
                  num += mval;
               }

               mval = gan_image_get_pix_gl_ui16(mask,i*2,j*2+1);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_vfield2D_f32 ( image, i*2, j*2+1 );
                  fmval = (gan_float32)mval;
                  total.x += fmval*pix.x;
                  total.y += fmval*pix.y;
                  num += mval;
               }

               mval = gan_image_get_pix_gl_ui16(mask,i*2+1,j*2);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_vfield2D_f32 ( image, i*2+1, j*2 );
                  fmval = (gan_float32)mval;
                  total.x += fmval*pix.x;
                  total.y += fmval*pix.y;
                  num += mval;
               }
                    
               mval = gan_image_get_pix_gl_ui16(mask,i*2+1,j*2+1);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_vfield2D_f32 ( image, i*2+1, j*2+1 );
                  fmval = (gan_float32)mval;
                  total.x += fmval*pix.x;
                  total.y += fmval*pix.y;
                  num += mval;
               }

               if ( num >= thres )
               {
                  gan_float32 fnum = (gan_float32)num;
                  pix.x = total.x/fnum;
                  pix.y = total.y/fnum;
                  gan_image_set_pix_vfield2D_f32 ( himage, i, j, &pix );
                  gan_image_set_pix_gl_ui16 ( hmask, i, j, num );
               }             
            }
      }
   }
   else
   {
      gan_float32 num;

      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
            if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
            {
               pix1 = gan_image_get_pix_vfield2D_f32 ( image, i*2,   j*2   );
               pix2 = gan_image_get_pix_vfield2D_f32 ( image, i*2,   j*2+1 );
               pix3 = gan_image_get_pix_vfield2D_f32 ( image, i*2+1, j*2   );
               pix4 = gan_image_get_pix_vfield2D_f32 ( image, i*2+1, j*2+1 );
               pix.x = 0.25F*(pix1.x + pix2.x + pix3.x + pix4.x);
               pix.y = 0.25F*(pix1.y + pix2.y + pix3.y + pix4.y);
               gan_image_set_pix_vfield2D_f32 ( himage, i, j, &pix );
               gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
            }
            else
            {
               num = 0; pix.x = pix.y = 0.0F;
               if ( gan_image_get_pix_b(mask,i*2,j*2) )
               {
                  pix = gan_image_get_pix_vfield2D_f32 ( image, i*2, j*2 );
                  (void)gan_vec2f_increment ( &pix, &pix );
                  num++;
               }
                    
               if ( gan_image_get_pix_b(mask,i*2,j*2+1) )
               {
                  pix = gan_image_get_pix_vfield2D_f32 ( image, i*2, j*2+1 );
                  (void)gan_vec2f_increment ( &pix, &pix );
                  num++;
               }

               if ( gan_image_get_pix_b(mask,i*2+1,j*2) )
               {
                  pix = gan_image_get_pix_vfield2D_f32 ( image, i*2+1, j*2 );
                  (void)gan_vec2f_increment ( &pix, &pix );
                  num++;
               }
                    
               if ( gan_image_get_pix_b(mask,i*2+1,j*2+1) )
               {
                  pix = gan_image_get_pix_vfield2D_f32 ( image, i*2+1, j*2+1 );
                  (void)gan_vec2f_increment ( &pix, &pix );
                  num++;
               }

               if ( num >= no_neighbours )
               {
                  pix.x *= 0.25F;
                  pix.y *= 0.25F;
                  gan_image_set_pix_vfield2D_f32 ( himage, i, j, &pix );
                  gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
               }
            }
   }

   /* success */
   return GAN_TRUE;
}

static Gan_Bool
 halve_size_vfield2D_f64 ( Gan_Image *image, Gan_Image *mask, Gan_ImagePyramidAverage average_type,
                           unsigned no_neighbours, Gan_Image *himage, Gan_Image *hmask, gan_uint32 thres )
{
   int i, j;
   Gan_Vector2_f64 pix1, pix2, pix3, pix4, pix;

   /* if mask is NULL, other parameters are irrelevant */
   if ( mask == NULL )
   {
      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
         {
            pix1 = gan_image_get_pix_vfield2D_f64 ( image, i*2,   j*2   );
            pix2 = gan_image_get_pix_vfield2D_f64 ( image, i*2,   j*2+1 );
            pix3 = gan_image_get_pix_vfield2D_f64 ( image, i*2+1, j*2   );
            pix4 = gan_image_get_pix_vfield2D_f64 ( image, i*2+1, j*2+1 );
            pix.x = 0.25*(pix1.x + pix2.x + pix3.x + pix4.x);
            pix.y = 0.25*(pix1.y + pix2.y + pix3.y + pix4.y);
            gan_image_set_pix_vfield2D_f64 ( himage, i, j, &pix );
         }
   }
   /* mask != NULL, action depends on other parameters */
   else if ( no_neighbours == 4 )
   {
      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
            if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
            {
               pix1 = gan_image_get_pix_vfield2D_f64 ( image, i*2,   j*2   );
               pix2 = gan_image_get_pix_vfield2D_f64 ( image, i*2,   j*2+1 );
               pix3 = gan_image_get_pix_vfield2D_f64 ( image, i*2+1, j*2   );
               pix4 = gan_image_get_pix_vfield2D_f64 ( image, i*2+1, j*2+1 );
               pix.x = 0.25*(pix1.x + pix2.x + pix3.x + pix4.x);
               pix.y = 0.25*(pix1.y + pix2.y + pix3.y + pix4.y);
               gan_image_set_pix_vfield2D_f64 ( himage, i, j, &pix );
               gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
            }
   }
   else if ( average_type == GAN_AVERAGE_IN_MASK )
   {
      if ( mask->type == GAN_BOOL )
      {
         for ( i = (int)image->height/2-1; i >= 0; i-- )
            for ( j = (int)image->width/2-1; j >= 0; j-- )
               if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
               {
                  pix1 = gan_image_get_pix_vfield2D_f64 ( image, i*2,   j*2   );
                  pix2 = gan_image_get_pix_vfield2D_f64 ( image, i*2,   j*2+1 );
                  pix3 = gan_image_get_pix_vfield2D_f64 ( image, i*2+1, j*2   );
                  pix4 = gan_image_get_pix_vfield2D_f64 ( image, i*2+1, j*2+1 );
                  pix.x = 0.25F*(pix1.x + pix2.x + pix3.x + pix4.x);
                  pix.y = 0.25F*(pix1.y + pix2.y + pix3.y + pix4.y);
                  gan_image_set_pix_vfield2D_f64 ( himage, i, j, &pix );
                  gan_image_set_pix_gl_ui16 ( hmask, i, j, 4 );
               }
               else
               {
                  gan_uint16 num=0;
                  Gan_Vector2_f64 total;

                  total.x = total.y = 0.0;
                  if ( gan_image_get_pix_b(mask,i*2,j*2) )
                  {
                     pix = gan_image_get_pix_vfield2D_f64 ( image, i*2, j*2 );
                     total.x += pix.x;
                     total.y += pix.y;
                     num++;
                  }
                    
                  if ( gan_image_get_pix_b(mask,i*2,j*2+1) )
                  {
                     pix = gan_image_get_pix_vfield2D_f64 ( image, i*2, j*2+1 );
                     total.x += pix.x;
                     total.y += pix.y;
                     num++;
                  }

                  if ( gan_image_get_pix_b(mask,i*2+1,j*2) )
                  {
                     pix = gan_image_get_pix_vfield2D_f64 ( image, i*2+1, j*2 );
                     total.x += pix.x;
                     total.y += pix.y;
                     num++;
                  }
                    
                  if ( gan_image_get_pix_b(mask,i*2+1,j*2+1) )
                  {
                     pix = gan_image_get_pix_vfield2D_f64 ( image, i*2+1, j*2+1 );
                     total.x += pix.x;
                     total.y += pix.y;
                     num++;
                  }

                  if ( num >= thres )
                  {
                     gan_float64 fnum = (gan_float64)num;
                     pix.x = total.x/fnum;
                     pix.y = total.y/fnum;
                     gan_image_set_pix_vfield2D_f64 ( himage, i, j, &pix );
                     gan_image_set_pix_gl_ui16 ( hmask, i, j, num );
                  }
               }             
      }
      else
      {
         for ( i = (int)image->height/2-1; i >= 0; i-- )
            for ( j = (int)image->width/2-1; j >= 0; j-- )
            {
               gan_uint16 mval, num=0;
               gan_float64 fmval;
               Gan_Vector2_f64 total;

               total.x = total.y = 0.0;

               mval = gan_image_get_pix_gl_ui16(mask,i*2,j*2);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_vfield2D_f64 ( image, i*2, j*2 );
                  fmval = (gan_float64)mval;
                  total.x += fmval*pix.x;
                  total.y += fmval*pix.y;
                  num += mval;
               }

               mval = gan_image_get_pix_gl_ui16(mask,i*2,j*2+1);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_vfield2D_f64 ( image, i*2, j*2+1 );
                  fmval = (gan_float64)mval;
                  total.x += fmval*pix.x;
                  total.y += fmval*pix.y;
                  num += mval;
               }

               mval = gan_image_get_pix_gl_ui16(mask,i*2+1,j*2);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_vfield2D_f64 ( image, i*2+1, j*2 );
                  fmval = (gan_float64)mval;
                  total.x += fmval*pix.x;
                  total.y += fmval*pix.y;
                  num += mval;
               }
                    
               mval = gan_image_get_pix_gl_ui16(mask,i*2+1,j*2+1);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_vfield2D_f64 ( image, i*2+1, j*2+1 );
                  fmval = (gan_float64)mval;
                  total.x += fmval*pix.x;
                  total.y += fmval*pix.y;
                  num += mval;
               }

               if ( num >= thres )
               {
                  gan_float64 fnum = (gan_float64)num;
                  pix.x = total.x/fnum;
                  pix.y = total.y/fnum;
                  gan_image_set_pix_vfield2D_f64 ( himage, i, j, &pix );
                  gan_image_set_pix_gl_ui16 ( hmask, i, j, num );
               }             
            }
      }
   }
   else
   {
      unsigned num;

      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
            if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
            {
               pix1 = gan_image_get_pix_vfield2D_f64 ( image, i*2,   j*2   );
               pix2 = gan_image_get_pix_vfield2D_f64 ( image, i*2,   j*2+1 );
               pix3 = gan_image_get_pix_vfield2D_f64 ( image, i*2+1, j*2   );
               pix4 = gan_image_get_pix_vfield2D_f64 ( image, i*2+1, j*2+1 );
               pix.x = 0.25*(pix1.x + pix2.x + pix3.x + pix4.x);
               pix.y = 0.25*(pix1.y + pix2.y + pix3.y + pix4.y);
               gan_image_set_pix_vfield2D_f64 ( himage, i, j, &pix );
               gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
            }
            else
            {
               num = 0; pix.x = pix.y = 0.0;
               if ( gan_image_get_pix_b(mask,i*2,j*2) )
               {
                  pix = gan_image_get_pix_vfield2D_f64 ( image, i*2, j*2 );
                  (void)gan_vec2_increment ( &pix, &pix );
                  num++;
               }
                    
               if ( gan_image_get_pix_b(mask,i*2,j*2+1) )
               {
                  pix = gan_image_get_pix_vfield2D_f64 ( image, i*2, j*2+1 );
                  (void)gan_vec2_increment ( &pix, &pix );
                  num++;
               }

               if ( gan_image_get_pix_b(mask,i*2+1,j*2) )
               {
                  pix = gan_image_get_pix_vfield2D_f64 ( image, i*2+1, j*2 );
                  (void)gan_vec2_increment ( &pix, &pix );
                  num++;
               }
                    
               if ( gan_image_get_pix_b(mask,i*2+1,j*2+1) )
               {
                  pix = gan_image_get_pix_vfield2D_f64 ( image, i*2+1, j*2+1 );
                  (void)gan_vec2_increment ( &pix, &pix );
                  num++;
               }

               if ( num >= no_neighbours )
               {
                  pix.x *= 0.25;
                  pix.y *= 0.25;
                  gan_image_set_pix_vfield2D_f64 ( himage, i, j, &pix );
                  gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
               }
            }
   }

   /* success */
   return GAN_TRUE;
}

static Gan_Bool
 halve_size_vfield3D_i16 ( Gan_Image *image, Gan_Image *mask, Gan_ImagePyramidAverage average_type,
                           unsigned no_neighbours, Gan_Image *himage, Gan_Image *hmask, gan_uint32 thres )
{
   int i, j;
   Gan_Vector3_i16 pix1, pix2, pix3, pix4, pix;

   /* if mask is NULL, other parameters are irrelevant */
   if ( mask == NULL )
   {
      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
         {
            pix1 = gan_image_get_pix_vfield3D_i16 ( image, i*2,   j*2   );
            pix2 = gan_image_get_pix_vfield3D_i16 ( image, i*2,   j*2+1 );
            pix3 = gan_image_get_pix_vfield3D_i16 ( image, i*2+1, j*2   );
            pix4 = gan_image_get_pix_vfield3D_i16 ( image, i*2+1, j*2+1 );
            pix.x = (gan_int16) (((gan_int32) pix1.x + (gan_int32) pix2.x + (gan_int32) pix3.x + (gan_int32) pix4.x + 2)/4);
            pix.y = (gan_int16) (((gan_int32) pix1.y + (gan_int32) pix2.y + (gan_int32) pix3.y + (gan_int32) pix4.y + 2)/4);
            pix.z = (gan_int16) (((gan_int32) pix1.z + (gan_int32) pix2.z + (gan_int32) pix3.z + (gan_int32) pix4.z + 2)/4);
            gan_image_set_pix_vfield3D_i16 ( himage, i, j, &pix );
         }
   }
   /* mask != NULL, action depends on other parameters */
   else if ( no_neighbours == 4 )
   {
      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
            if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
            {
               pix1 = gan_image_get_pix_vfield3D_i16 ( image, i*2,   j*2   );
               pix2 = gan_image_get_pix_vfield3D_i16 ( image, i*2,   j*2+1 );
               pix3 = gan_image_get_pix_vfield3D_i16 ( image, i*2+1, j*2   );
               pix4 = gan_image_get_pix_vfield3D_i16 ( image, i*2+1, j*2+1 );
               pix.x = (gan_int16) (((gan_int32) pix1.x + (gan_int32) pix2.x + (gan_int32) pix3.x + (gan_int32) pix4.x + 2)/4);
               pix.y = (gan_int16) (((gan_int32) pix1.y + (gan_int32) pix2.y + (gan_int32) pix3.y + (gan_int32) pix4.y + 2)/4);
               pix.z = (gan_int16) (((gan_int32) pix1.z + (gan_int32) pix2.z + (gan_int32) pix3.z + (gan_int32) pix4.z + 2)/4);
               gan_image_set_pix_vfield3D_i16 ( himage, i, j, &pix );
               gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
            }
   }
   else if ( average_type == GAN_AVERAGE_IN_MASK )
   {
      if ( mask->type == GAN_BOOL )
      {
         for ( i = (int)image->height/2-1; i >= 0; i-- )
            for ( j = (int)image->width/2-1; j >= 0; j-- )
               if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
               {
                  pix1 = gan_image_get_pix_vfield3D_i16 ( image, i*2,   j*2   );
                  pix2 = gan_image_get_pix_vfield3D_i16 ( image, i*2,   j*2+1 );
                  pix3 = gan_image_get_pix_vfield3D_i16 ( image, i*2+1, j*2   );
                  pix4 = gan_image_get_pix_vfield3D_i16 ( image, i*2+1, j*2+1 );
                  pix.x = (gan_int16) (((gan_int32) pix1.x + (gan_int32) pix2.x + (gan_int32) pix3.x + (gan_int32) pix4.x + 2)/4);
                  pix.y = (gan_int16) (((gan_int32) pix1.y + (gan_int32) pix2.y + (gan_int32) pix3.y + (gan_int32) pix4.y + 2)/4);
                  pix.z = (gan_int16) (((gan_int32) pix1.z + (gan_int32) pix2.z + (gan_int32) pix3.z + (gan_int32) pix4.z + 2)/4);
                  gan_image_set_pix_vfield3D_i16 ( himage, i, j, &pix );
                  gan_image_set_pix_gl_ui16 ( hmask, i, j, 4 );
               }
               else
               {
                  gan_uint16 num=0;
                  Gan_Vector3_i32 total;

                  total.x = total.y = total.z = 0;
                  if ( gan_image_get_pix_b(mask,i*2,j*2) )
                  {
                     pix = gan_image_get_pix_vfield3D_i16 ( image, i*2, j*2 );
                     total.x += (gan_int32)pix.x;
                     total.y += (gan_int32)pix.y;
                     total.z += (gan_int32)pix.z;
                     num++;
                  }
                    
                  if ( gan_image_get_pix_b(mask,i*2,j*2+1) )
                  {
                     pix = gan_image_get_pix_vfield3D_i16 ( image, i*2, j*2+1 );
                     total.x += (gan_int32)pix.x;
                     total.y += (gan_int32)pix.y;
                     total.z += (gan_int32)pix.z;
                     num++;
                  }

                  if ( gan_image_get_pix_b(mask,i*2+1,j*2) )
                  {
                     pix = gan_image_get_pix_vfield3D_i16 ( image, i*2+1, j*2 );
                     total.x += (gan_int32)pix.x;
                     total.y += (gan_int32)pix.y;
                     total.z += (gan_int32)pix.z;
                     num++;
                  }
                    
                  if ( gan_image_get_pix_b(mask,i*2+1,j*2+1) )
                  {
                     pix = gan_image_get_pix_vfield3D_i16 ( image, i*2+1, j*2+1 );
                     total.x += (gan_int32)pix.x;
                     total.y += (gan_int32)pix.y;
                     total.z += (gan_int32)pix.z;
                     num++;
                  }

                  if ( num >= thres )
                  {
                     pix.x = (gan_int16) ((total.x + num/2)/num);
                     pix.y = (gan_int16) ((total.y + num/2)/num);
                     pix.z = (gan_int16) ((total.z + num/2)/num);
                     gan_image_set_pix_vfield3D_i16 ( himage, i, j, &pix );
                     gan_image_set_pix_gl_ui16 ( hmask, i, j, num );
                  }
               }
      }
      else
      {
         for ( i = (int)image->height/2-1; i >= 0; i-- )
            for ( j = (int)image->width/2-1; j >= 0; j-- )
            {
               gan_int32 mval, num=0;
               Gan_Vector3_i32 total;

               total.x = total.y = total.z = 0;

               mval = (gan_int32) gan_image_get_pix_gl_ui16(mask,i*2,j*2);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_vfield3D_i16 ( image, i*2, j*2 );
                  total.x += mval*(gan_int32)pix.x;
                  total.y += mval*(gan_int32)pix.y;
                  total.z += mval*(gan_int32)pix.z;
                  num += mval;
               }
                    
               mval = (gan_int32) gan_image_get_pix_gl_ui16(mask,i*2,j*2+1);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_vfield3D_i16 ( image, i*2, j*2+1 );
                  total.x += mval*(gan_int32)pix.x;
                  total.y += mval*(gan_int32)pix.y;
                  total.z += mval*(gan_int32)pix.z;
                  num += mval;
               }

               mval = (gan_int32) gan_image_get_pix_gl_ui16(mask,i*2+1,j*2);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_vfield3D_i16 ( image, i*2+1, j*2 );
                  total.x += mval*(gan_int32)pix.x;
                  total.y += mval*(gan_int32)pix.y;
                  total.z += mval*(gan_int32)pix.z;
                  num += mval;
               }
                    
               mval = (gan_int32) gan_image_get_pix_gl_ui16(mask,i*2+1,j*2+1);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_vfield3D_i16 ( image, i*2+1, j*2+1 );
                  total.x += mval*(gan_int32)pix.x;
                  total.y += mval*(gan_int32)pix.y;
                  total.z += mval*(gan_int32)pix.z;
                  num += mval;
               }

               if ( (gan_uint32)num >= thres )
               {
                  pix.x = (gan_int16) ((total.x + num/2)/num);
                  pix.y = (gan_int16) ((total.y + num/2)/num);
                  pix.z = (gan_int16) ((total.z + num/2)/num);
                  gan_image_set_pix_vfield3D_i16 ( himage, i, j, &pix );
                  gan_image_set_pix_gl_ui16 ( hmask, i, j, num );
               }             
            }
      }
   }
   else
   {
      int num;
      Gan_Vector3_i total;

      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
            if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
            {
               pix1 = gan_image_get_pix_vfield3D_i16 ( image, i*2,   j*2   );
               pix2 = gan_image_get_pix_vfield3D_i16 ( image, i*2,   j*2+1 );
               pix3 = gan_image_get_pix_vfield3D_i16 ( image, i*2+1, j*2   );
               pix4 = gan_image_get_pix_vfield3D_i16 ( image, i*2+1, j*2+1 );
               pix.x = (gan_int16) (((gan_int32) pix1.x + (gan_int32) pix2.x + (gan_int32) pix3.x + (gan_int32) pix4.x + 2)/4);
               pix.y = (gan_int16) (((gan_int32) pix1.y + (gan_int32) pix2.y + (gan_int32) pix3.y + (gan_int32) pix4.y + 2)/4);
               pix.z = (gan_int16) (((gan_int32) pix1.z + (gan_int32) pix2.z + (gan_int32) pix3.z + (gan_int32) pix4.z + 2)/4);
               gan_image_set_pix_vfield3D_i16 ( himage, i, j, &pix );
               gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
            }
            else
            {
               num = total.x = total.y = total.z = 0;
               if ( gan_image_get_pix_b(mask,i*2,j*2) )
               {
                  pix = gan_image_get_pix_vfield3D_i16 ( image, i*2, j*2 );
                  total.x += (gan_int32)pix.x;
                  total.y += (gan_int32)pix.y;
                  total.z += (gan_int32)pix.z;
                  num++;
               }
                    
               if ( gan_image_get_pix_b(mask,i*2,j*2+1) )
               {
                  pix = gan_image_get_pix_vfield3D_i16 ( image, i*2, j*2+1 );
                  total.x += (gan_int32)pix.x;
                  total.y += (gan_int32)pix.y;
                  total.z += (gan_int32)pix.z;
                  num++;
               }

               if ( gan_image_get_pix_b(mask,i*2+1,j*2) )
               {
                  pix = gan_image_get_pix_vfield3D_i16 ( image, i*2+1, j*2 );
                  total.x += (gan_int32)pix.x;
                  total.y += (gan_int32)pix.y;
                  total.z += (gan_int32)pix.z;
                  num++;
               }
                    
               if ( gan_image_get_pix_b(mask,i*2+1,j*2+1) )
               {
                  pix = gan_image_get_pix_vfield3D_i16 ( image, i*2+1, j*2+1 );
                  total.x += (gan_int32)pix.x;
                  total.y += (gan_int32)pix.y;
                  total.z += (gan_int32)pix.z;
                  num++;
               }

               if ( (gan_uint32)num >= no_neighbours )
               {
                  pix.x = (gan_int16) ((total.x + 2)/4);
                  pix.y = (gan_int16) ((total.y + 2)/4);
                  pix.z = (gan_int16) ((total.z + 2)/4);
                  gan_image_set_pix_vfield3D_i16 ( himage, i, j, &pix );
                  gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
               }
            }
   }

   /* success */
   return GAN_TRUE;
}

static Gan_Bool
 halve_size_vfield3D_f32 ( Gan_Image *image, Gan_Image *mask, Gan_ImagePyramidAverage average_type,
                           unsigned no_neighbours, Gan_Image *himage, Gan_Image *hmask, gan_uint32 thres )
{
   int i, j;
   Gan_Vector3_f32 pix1, pix2, pix3, pix4, pix;

   /* if mask is NULL, other parameters are irrelevant */
   if ( mask == NULL )
   {
      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
         {
            pix1 = gan_image_get_pix_vfield3D_f32 ( image, i*2,   j*2   );
            pix2 = gan_image_get_pix_vfield3D_f32 ( image, i*2,   j*2+1 );
            pix3 = gan_image_get_pix_vfield3D_f32 ( image, i*2+1, j*2   );
            pix4 = gan_image_get_pix_vfield3D_f32 ( image, i*2+1, j*2+1 );
            pix.x = 0.25F*(pix1.x + pix2.x + pix3.x + pix4.x);
            pix.y = 0.25F*(pix1.y + pix2.y + pix3.y + pix4.y);
            pix.z = 0.25F*(pix1.z + pix2.z + pix3.z + pix4.z);
            gan_image_set_pix_vfield3D_f32 ( himage, i, j, &pix );
         }
   }
   /* mask != NULL, action depends on other parameters */
   else if ( no_neighbours == 4 )
   {
      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
            if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
            {
               pix1 = gan_image_get_pix_vfield3D_f32 ( image, i*2,   j*2   );
               pix2 = gan_image_get_pix_vfield3D_f32 ( image, i*2,   j*2+1 );
               pix3 = gan_image_get_pix_vfield3D_f32 ( image, i*2+1, j*2   );
               pix4 = gan_image_get_pix_vfield3D_f32 ( image, i*2+1, j*2+1 );
               pix.x = 0.25F*(pix1.x + pix2.x + pix3.x + pix4.x);
               pix.y = 0.25F*(pix1.y + pix2.y + pix3.y + pix4.y);
               pix.z = 0.25F*(pix1.z + pix2.z + pix3.z + pix4.z);
               gan_image_set_pix_vfield3D_f32 ( himage, i, j, &pix );
               gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
            }
   }
   else if ( average_type == GAN_AVERAGE_IN_MASK )
   {
      if ( mask->type == GAN_BOOL )
      {
         for ( i = (int)image->height/2-1; i >= 0; i-- )
            for ( j = (int)image->width/2-1; j >= 0; j-- )
               if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
               {
                  pix1 = gan_image_get_pix_vfield3D_f32 ( image, i*2,   j*2   );
                  pix2 = gan_image_get_pix_vfield3D_f32 ( image, i*2,   j*2+1 );
                  pix3 = gan_image_get_pix_vfield3D_f32 ( image, i*2+1, j*2   );
                  pix4 = gan_image_get_pix_vfield3D_f32 ( image, i*2+1, j*2+1 );
                  pix.x = 0.25F*(pix1.x + pix2.x + pix3.x + pix4.x);
                  pix.y = 0.25F*(pix1.y + pix2.y + pix3.y + pix4.y);
                  pix.z = 0.25F*(pix1.z + pix2.z + pix3.z + pix4.z);
                  gan_image_set_pix_vfield3D_f32 ( himage, i, j, &pix );
                  gan_image_set_pix_gl_ui16 ( hmask, i, j, 4 );
               }
               else
               {
                  gan_uint16 num=0;
                  Gan_Vector3_f32 total;

                  total.x = total.y = total.z = 0.0F;
                  if ( gan_image_get_pix_b(mask,i*2,j*2) )
                  {
                     pix = gan_image_get_pix_vfield3D_f32 ( image, i*2, j*2 );
                     total.x += pix.x;
                     total.y += pix.y;
                     total.z += pix.z;
                     num++;
                  }
                    
                  if ( gan_image_get_pix_b(mask,i*2,j*2+1) )
                  {
                     pix = gan_image_get_pix_vfield3D_f32 ( image, i*2, j*2+1 );
                     total.x += pix.x;
                     total.y += pix.y;
                     total.z += pix.z;
                     num++;
                  }

                  if ( gan_image_get_pix_b(mask,i*2+1,j*2) )
                  {
                     pix = gan_image_get_pix_vfield3D_f32 ( image, i*2+1, j*2 );
                     total.x += pix.x;
                     total.y += pix.y;
                     total.z += pix.z;
                     num++;
                  }
                    
                  if ( gan_image_get_pix_b(mask,i*2+1,j*2+1) )
                  {
                     pix = gan_image_get_pix_vfield3D_f32 ( image, i*2+1, j*2+1 );
                     total.x += pix.x;
                     total.y += pix.y;
                     total.z += pix.z;
                     num++;
                  }

                  if ( num >= thres )
                  {
                     gan_float32 fnum = (gan_float32)num;
                     pix.x = total.x/fnum;
                     pix.y = total.y/fnum;
                     pix.z = total.z/fnum;
                     gan_image_set_pix_vfield3D_f32 ( himage, i, j, &pix );
                     gan_image_set_pix_gl_ui16 ( hmask, i, j, num );
                  }
               }             
      }
      else
      {
         for ( i = (int)image->height/2-1; i >= 0; i-- )
            for ( j = (int)image->width/2-1; j >= 0; j-- )
            {
               gan_uint16 mval, num=0;
               float fmval;
               Gan_Vector3_f32 total;

               total.x = total.y = total.z = 0.0F;

               mval = gan_image_get_pix_gl_ui16(mask,i*2,j*2);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_vfield3D_f32 ( image, i*2, j*2 );
                  fmval = (gan_float32)mval;
                  total.x += fmval*pix.x;
                  total.y += fmval*pix.y;
                  total.z += fmval*pix.z;
                  num += mval;
               }

               mval = gan_image_get_pix_gl_ui16(mask,i*2,j*2+1);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_vfield3D_f32 ( image, i*2, j*2+1 );
                  fmval = (gan_float32)mval;
                  total.x += fmval*pix.x;
                  total.y += fmval*pix.y;
                  total.z += fmval*pix.z;
                  num += mval;
               }

               mval = gan_image_get_pix_gl_ui16(mask,i*2+1,j*2);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_vfield3D_f32 ( image, i*2+1, j*2 );
                  fmval = (gan_float32)mval;
                  total.x += fmval*pix.x;
                  total.y += fmval*pix.y;
                  total.z += fmval*pix.z;
                  num += mval;
               }
                    
               mval = gan_image_get_pix_gl_ui16(mask,i*2+1,j*2+1);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_vfield3D_f32 ( image, i*2+1, j*2+1 );
                  fmval = (gan_float32)mval;
                  total.x += fmval*pix.x;
                  total.y += fmval*pix.y;
                  total.z += fmval*pix.z;
                  num += mval;
               }

               if ( num >= thres )
               {
                  gan_float32 fnum = (gan_float32)num;
                  pix.x = total.x/fnum;
                  pix.y = total.y/fnum;
                  pix.z = total.z/fnum;
                  gan_image_set_pix_vfield3D_f32 ( himage, i, j, &pix );
                  gan_image_set_pix_gl_ui16 ( hmask, i, j, num );
               }             
            }
      }
   }
   else
   {
      gan_float32 num;

      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
            if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
            {
               pix1 = gan_image_get_pix_vfield3D_f32 ( image, i*2,   j*2   );
               pix2 = gan_image_get_pix_vfield3D_f32 ( image, i*2,   j*2+1 );
               pix3 = gan_image_get_pix_vfield3D_f32 ( image, i*2+1, j*2   );
               pix4 = gan_image_get_pix_vfield3D_f32 ( image, i*2+1, j*2+1 );
               pix.x = 0.25F*(pix1.x + pix2.x + pix3.x + pix4.x);
               pix.y = 0.25F*(pix1.y + pix2.y + pix3.y + pix4.y);
               pix.z = 0.25F*(pix1.z + pix2.z + pix3.z + pix4.z);
               gan_image_set_pix_vfield3D_f32 ( himage, i, j, &pix );
               gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
            }
            else
            {
               num = 0; pix.x = pix.y = pix.z = 0.0F;
               if ( gan_image_get_pix_b(mask,i*2,j*2) )
               {
                  pix = gan_image_get_pix_vfield3D_f32 ( image, i*2, j*2 );
                  (void)gan_vec3f_increment ( &pix, &pix );
                  num++;
               }
                    
               if ( gan_image_get_pix_b(mask,i*2,j*2+1) )
               {
                  pix = gan_image_get_pix_vfield3D_f32 ( image, i*2, j*2+1 );
                  (void)gan_vec3f_increment ( &pix, &pix );
                  num++;
               }

               if ( gan_image_get_pix_b(mask,i*2+1,j*2) )
               {
                  pix = gan_image_get_pix_vfield3D_f32 ( image, i*2+1, j*2 );
                  (void)gan_vec3f_increment ( &pix, &pix );
                  num++;
               }
                    
               if ( gan_image_get_pix_b(mask,i*2+1,j*2+1) )
               {
                  pix = gan_image_get_pix_vfield3D_f32 ( image, i*2+1, j*2+1 );
                  (void)gan_vec3f_increment ( &pix, &pix );
                  num++;
               }

               if ( num >= no_neighbours )
               {
                  pix.x *= 0.25F;
                  pix.y *= 0.25F;
                  pix.z *= 0.25F;
                  gan_image_set_pix_vfield3D_f32 ( himage, i, j, &pix );
                  gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
               }
            }
   }

   /* success */
   return GAN_TRUE;
}

static Gan_Bool
 halve_size_vfield3D_f64 ( Gan_Image *image, Gan_Image *mask, Gan_ImagePyramidAverage average_type,
                           unsigned no_neighbours, Gan_Image *himage, Gan_Image *hmask, gan_uint32 thres )
{
   int i, j;
   Gan_Vector3_f64 pix1, pix2, pix3, pix4, pix;

   /* if mask is NULL, other parameters are irrelevant */
   if ( mask == NULL )
   {
      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
         {
            pix1 = gan_image_get_pix_vfield3D_f64 ( image, i*2,   j*2   );
            pix2 = gan_image_get_pix_vfield3D_f64 ( image, i*2,   j*2+1 );
            pix3 = gan_image_get_pix_vfield3D_f64 ( image, i*2+1, j*2   );
            pix4 = gan_image_get_pix_vfield3D_f64 ( image, i*2+1, j*2+1 );
            pix.x = 0.25*(pix1.x + pix2.x + pix3.x + pix4.x);
            pix.y = 0.25*(pix1.y + pix2.y + pix3.y + pix4.y);
            pix.z = 0.25*(pix1.z + pix2.z + pix3.z + pix4.z);
            gan_image_set_pix_vfield3D_f64 ( himage, i, j, &pix );
         }
   }
   /* mask != NULL, action depends on other parameters */
   else if ( no_neighbours == 4 )
   {
      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
            if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
            {
               pix1 = gan_image_get_pix_vfield3D_f64 ( image, i*2,   j*2   );
               pix2 = gan_image_get_pix_vfield3D_f64 ( image, i*2,   j*2+1 );
               pix3 = gan_image_get_pix_vfield3D_f64 ( image, i*2+1, j*2   );
               pix4 = gan_image_get_pix_vfield3D_f64 ( image, i*2+1, j*2+1 );
               pix.x = 0.25*(pix1.x + pix2.x + pix3.x + pix4.x);
               pix.y = 0.25*(pix1.y + pix2.y + pix3.y + pix4.y);
               pix.z = 0.25*(pix1.z + pix2.z + pix3.z + pix4.z);
               gan_image_set_pix_vfield3D_f64 ( himage, i, j, &pix );
               gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
            }
   }
   else if ( average_type == GAN_AVERAGE_IN_MASK )
   {
      if ( mask->type == GAN_BOOL )
      {
         for ( i = (int)image->height/2-1; i >= 0; i-- )
            for ( j = (int)image->width/2-1; j >= 0; j-- )
               if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
               {
                  pix1 = gan_image_get_pix_vfield3D_f64 ( image, i*2,   j*2   );
                  pix2 = gan_image_get_pix_vfield3D_f64 ( image, i*2,   j*2+1 );
                  pix3 = gan_image_get_pix_vfield3D_f64 ( image, i*2+1, j*2   );
                  pix4 = gan_image_get_pix_vfield3D_f64 ( image, i*2+1, j*2+1 );
                  pix.x = 0.25F*(pix1.x + pix2.x + pix3.x + pix4.x);
                  pix.y = 0.25F*(pix1.y + pix2.y + pix3.y + pix4.y);
                  pix.z = 0.25F*(pix1.z + pix2.z + pix3.z + pix4.z);
                  gan_image_set_pix_vfield3D_f64 ( himage, i, j, &pix );
                  gan_image_set_pix_gl_ui16 ( hmask, i, j, 4 );
               }
               else
               {
                  gan_uint16 num=0;
                  Gan_Vector3_f64 total;

                  total.x = total.y = total.z = 0.0;
                  if ( gan_image_get_pix_b(mask,i*2,j*2) )
                  {
                     pix = gan_image_get_pix_vfield3D_f64 ( image, i*2, j*2 );
                     total.x += pix.x;
                     total.y += pix.y;
                     total.z += pix.z;
                     num++;
                  }
                    
                  if ( gan_image_get_pix_b(mask,i*2,j*2+1) )
                  {
                     pix = gan_image_get_pix_vfield3D_f64 ( image, i*2, j*2+1 );
                     total.x += pix.x;
                     total.y += pix.y;
                     total.z += pix.z;
                     num++;
                  }

                  if ( gan_image_get_pix_b(mask,i*2+1,j*2) )
                  {
                     pix = gan_image_get_pix_vfield3D_f64 ( image, i*2+1, j*2 );
                     total.x += pix.x;
                     total.y += pix.y;
                     total.z += pix.z;
                     num++;
                  }
                    
                  if ( gan_image_get_pix_b(mask,i*2+1,j*2+1) )
                  {
                     pix = gan_image_get_pix_vfield3D_f64 ( image, i*2+1, j*2+1 );
                     total.x += pix.x;
                     total.y += pix.y;
                     total.z += pix.z;
                     num++;
                  }

                  if ( num >= thres )
                  {
                     gan_float64 fnum = (gan_float64)num;
                     pix.x = total.x/fnum;
                     pix.y = total.y/fnum;
                     pix.z = total.z/fnum;
                     gan_image_set_pix_vfield3D_f64 ( himage, i, j, &pix );
                     gan_image_set_pix_gl_ui16 ( hmask, i, j, num );
                  }
               }             
      }
      else
      {
         for ( i = (int)image->height/2-1; i >= 0; i-- )
            for ( j = (int)image->width/2-1; j >= 0; j-- )
            {
               gan_uint16 mval, num=0;
               gan_float64 fmval;
               Gan_Vector3_f64 total;

               total.x = total.y = total.z = 0.0;

               mval = gan_image_get_pix_gl_ui16(mask,i*2,j*2);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_vfield3D_f64 ( image, i*2, j*2 );
                  fmval = (gan_float64)mval;
                  total.x += fmval*pix.x;
                  total.y += fmval*pix.y;
                  total.z += fmval*pix.z;
                  num += mval;
               }

               mval = gan_image_get_pix_gl_ui16(mask,i*2,j*2+1);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_vfield3D_f64 ( image, i*2, j*2+1 );
                  fmval = (gan_float64)mval;
                  total.x += fmval*pix.x;
                  total.y += fmval*pix.y;
                  total.z += fmval*pix.z;
                  num += mval;
               }

               mval = gan_image_get_pix_gl_ui16(mask,i*2+1,j*2);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_vfield3D_f64 ( image, i*2+1, j*2 );
                  fmval = (gan_float64)mval;
                  total.x += fmval*pix.x;
                  total.y += fmval*pix.y;
                  total.z += fmval*pix.z;
                  num += mval;
               }
                    
               mval = gan_image_get_pix_gl_ui16(mask,i*2+1,j*2+1);
               if ( mval > 0 )
               {
                  pix = gan_image_get_pix_vfield3D_f64 ( image, i*2+1, j*2+1 );
                  fmval = (gan_float64)mval;
                  total.x += fmval*pix.x;
                  total.y += fmval*pix.y;
                  total.z += fmval*pix.z;
                  num += mval;
               }

               if ( num >= thres )
               {
                  gan_float64 fnum = (gan_float64)num;
                  pix.x = total.x/fnum;
                  pix.y = total.y/fnum;
                  pix.z = total.z/fnum;
                  gan_image_set_pix_vfield3D_f64 ( himage, i, j, &pix );
                  gan_image_set_pix_gl_ui16 ( hmask, i, j, num );
               }             
            }
      }
   }
   else
   {
      unsigned num;

      for ( i = (int)image->height/2-1; i >= 0; i-- )
         for ( j = (int)image->width/2-1; j >= 0; j-- )
            if ( gan_image_bit_get_pix_4group(mask,i*2,j*2) )
            {
               pix1 = gan_image_get_pix_vfield3D_f64 ( image, i*2,   j*2   );
               pix2 = gan_image_get_pix_vfield3D_f64 ( image, i*2,   j*2+1 );
               pix3 = gan_image_get_pix_vfield3D_f64 ( image, i*2+1, j*2   );
               pix4 = gan_image_get_pix_vfield3D_f64 ( image, i*2+1, j*2+1 );
               pix.x = 0.25*(pix1.x + pix2.x + pix3.x + pix4.x);
               pix.y = 0.25*(pix1.y + pix2.y + pix3.y + pix4.y);
               pix.z = 0.25*(pix1.z + pix2.z + pix3.z + pix4.z);
               gan_image_set_pix_vfield3D_f64 ( himage, i, j, &pix );
               gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
            }
            else
            {
               num = 0; pix.x = pix.y = pix.z = 0.0;
               if ( gan_image_get_pix_b(mask,i*2,j*2) )
               {
                  pix = gan_image_get_pix_vfield3D_f64 ( image, i*2, j*2 );
                  (void)gan_vec3_increment ( &pix, &pix );
                  num++;
               }
                    
               if ( gan_image_get_pix_b(mask,i*2,j*2+1) )
               {
                  pix = gan_image_get_pix_vfield3D_f64 ( image, i*2, j*2+1 );
                  (void)gan_vec3_increment ( &pix, &pix );
                  num++;
               }

               if ( gan_image_get_pix_b(mask,i*2+1,j*2) )
               {
                  pix = gan_image_get_pix_vfield3D_f64 ( image, i*2+1, j*2 );
                  (void)gan_vec3_increment ( &pix, &pix );
                  num++;
               }
                    
               if ( gan_image_get_pix_b(mask,i*2+1,j*2+1) )
               {
                  pix = gan_image_get_pix_vfield3D_f64 ( image, i*2+1, j*2+1 );
                  (void)gan_vec3_increment ( &pix, &pix );
                  num++;
               }

               if ( num >= no_neighbours )
               {
                  pix.x *= 0.25;
                  pix.y *= 0.25;
                  pix.z *= 0.25;
                  gan_image_set_pix_vfield3D_f64 ( himage, i, j, &pix );
                  gan_image_set_pix_b ( hmask, i, j, GAN_TRUE );
               }
            }
   }

   /* success */
   return GAN_TRUE;
}

/* Halve the size of an image requiring all four mask pixels in a neighbourhood
 * to be set.
 */
static Gan_Bool
 halve_size ( Gan_Image *image, Gan_Image *mask, Gan_ImagePyramidAverage average_type,
              unsigned no_neighbours, Gan_Image **himage, Gan_Image **hmask, unsigned int thres )
{
   Gan_Bool result;

   /* allocate half-size image and initialize it to zero */
   if((*himage) == NULL)
      *himage = gan_image_alloc ( image->format, image->type, image->height/2, image->width/2 );
   else
      *himage = gan_image_set_format_type_dims ( *himage, image->format, image->type, image->height/2, image->width/2 );

   if ( *himage == NULL )
   {
      gan_err_register ( "halve_size", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   gan_image_fill_zero ( *himage );

   if ( mask == NULL )
   {
      gan_err_test_bool ( hmask == NULL, "halve_size",
                          GAN_ERROR_INCOMPATIBLE, "mask images" );
   }
   else
   {
      /* allocate half-size mask and initialize it to zero (false) */
      if(average_type == GAN_AVERAGE_IN_MASK && no_neighbours < 4)
      {
         if((*hmask) == NULL)
            *hmask = gan_image_alloc ( GAN_GREY_LEVEL_IMAGE, GAN_UINT16, image->height/2, image->width/2 );
         else
            *hmask = gan_image_set_format_type_dims ( *hmask, GAN_GREY_LEVEL_IMAGE, GAN_UINT16, image->height/2, image->width/2 );
      }
      else
      {
         if((*hmask) == NULL)
            *hmask = gan_image_alloc ( GAN_GREY_LEVEL_IMAGE, GAN_BOOL, image->height/2, image->width/2 );
         else
            *hmask = gan_image_set_format_type_dims ( *hmask, GAN_GREY_LEVEL_IMAGE, GAN_BOOL, image->height/2, image->width/2 );
      }

      if ( *hmask == NULL )
      {
         gan_err_register ( "halve_size", GAN_ERROR_FAILURE, "" );
         return GAN_FALSE;
      }

      gan_image_fill_zero ( *hmask );
   }

   switch ( image->format )
   {
      case GAN_GREY_LEVEL_IMAGE:
        switch ( image->type )
        {
           case GAN_BOOL:
             result = halve_size_b ( image, mask, average_type, no_neighbours, *himage, (mask == NULL) ? NULL : *hmask );
             break;

           case GAN_UINT8:
             result = halve_size_gl_ui8 ( image, mask, average_type, no_neighbours, *himage, (mask == NULL) ? NULL : *hmask, thres );
             break;

           case GAN_UINT16:
             result = halve_size_gl_ui16 ( image, mask, average_type, no_neighbours, *himage, (mask == NULL) ? NULL : *hmask, thres );
             break;

           case GAN_UINT32:
             result = halve_size_gl_ui32 ( image, mask, average_type, no_neighbours, *himage, (mask == NULL) ? NULL : *hmask, thres );
             break;

           case GAN_FLOAT32:
             result = halve_size_gl_f32 ( image, mask, average_type, no_neighbours, *himage, (mask == NULL) ? NULL : *hmask, thres );
             break;

           case GAN_FLOAT64:
             result = halve_size_gl_f64 ( image, mask, average_type, no_neighbours, *himage, (mask == NULL) ? NULL : *hmask, thres );
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "halve_size", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
             gan_image_free ( *himage );
             if ( mask != NULL ) gan_image_free ( *hmask );
             return GAN_FALSE;
        }
        break;

      case GAN_RGB_COLOUR_IMAGE:
        
        switch ( image->type )
        {
           case GAN_FLOAT32:
             result = halve_size_rgb_f32 ( image, mask, average_type, no_neighbours, *himage, (mask == NULL) ? NULL : *hmask, thres );
             break;
         
           case GAN_FLOAT64:
             result = halve_size_rgb_f64 ( image, mask, average_type, no_neighbours, *himage, (mask == NULL) ? NULL : *hmask, thres );
             break;
         
           case GAN_UINT8:
             result = halve_size_rgb_ui8 ( image, mask, average_type, no_neighbours, *himage, (mask == NULL) ? NULL : *hmask, thres );
             break;

           case GAN_UINT16:
             result = halve_size_rgb_ui16 ( image, mask, average_type, no_neighbours, *himage, (mask == NULL) ? NULL : *hmask, thres );
             break;

           case GAN_UINT32:
             result = halve_size_rgb_ui32 ( image, mask, average_type, no_neighbours, *himage, (mask == NULL) ? NULL : *hmask, thres );
             break;

         default:
           gan_err_flush_trace();
           gan_err_register ( "halve_size", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
           gan_image_free ( *himage );
           if ( mask != NULL ) gan_image_free ( *hmask );
           return GAN_FALSE;
      }
      break;

      case GAN_RGB_COLOUR_ALPHA_IMAGE:
        switch ( image->type )
        {
           case GAN_FLOAT32:
             result = halve_size_rgba_f32 ( image, mask, average_type, no_neighbours, *himage, (mask == NULL) ? NULL : *hmask, thres );
             break;
         
           case GAN_FLOAT64:
             result = halve_size_rgba_f64 ( image, mask, average_type, no_neighbours, *himage, (mask == NULL) ? NULL : *hmask, thres );
             break;
         
           case GAN_UINT8:
             result = halve_size_rgba_ui8 ( image, mask, average_type, no_neighbours, *himage, (mask == NULL) ? NULL : *hmask, thres );
             break;

           case GAN_UINT16:
             result = halve_size_rgba_ui16 ( image, mask, average_type, no_neighbours, *himage, (mask == NULL) ? NULL : *hmask, thres );
             break;

           case GAN_UINT32:
             result = halve_size_rgba_ui32 ( image, mask, average_type, no_neighbours, *himage, (mask == NULL) ? NULL : *hmask, thres );
             break;

         default:
           gan_err_flush_trace();
           gan_err_register ( "halve_size", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
           gan_image_free ( *himage );
           if ( mask != NULL ) gan_image_free ( *hmask );
           return GAN_FALSE;
      }
      break;

      case GAN_BGR_COLOUR_IMAGE:
        
        switch ( image->type )
        {
           case GAN_FLOAT32:
             result = halve_size_bgr_f32 ( image, mask, average_type, no_neighbours, *himage, (mask == NULL) ? NULL : *hmask, thres );
             break;
         
           case GAN_FLOAT64:
             result = halve_size_bgr_f64 ( image, mask, average_type, no_neighbours, *himage, (mask == NULL) ? NULL : *hmask, thres );
             break;
         
           case GAN_UINT8:
             result = halve_size_bgr_ui8 ( image, mask, average_type, no_neighbours, *himage, (mask == NULL) ? NULL : *hmask, thres );
             break;

           case GAN_UINT16:
             result = halve_size_bgr_ui16 ( image, mask, average_type, no_neighbours, *himage, (mask == NULL) ? NULL : *hmask, thres );
             break;

           case GAN_UINT32:
             result = halve_size_bgr_ui32 ( image, mask, average_type, no_neighbours, *himage, (mask == NULL) ? NULL : *hmask, thres );
             break;

         default:
           gan_err_flush_trace();
           gan_err_register ( "halve_size", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
           gan_image_free ( *himage );
           if ( mask != NULL ) gan_image_free ( *hmask );
           return GAN_FALSE;
      }
      break;

      case GAN_BGR_COLOUR_ALPHA_IMAGE:
        switch ( image->type )
        {
           case GAN_FLOAT32:
             result = halve_size_bgra_f32 ( image, mask, average_type, no_neighbours, *himage, (mask == NULL) ? NULL : *hmask, thres );
             break;
         
           case GAN_FLOAT64:
             result = halve_size_bgra_f64 ( image, mask, average_type, no_neighbours, *himage, (mask == NULL) ? NULL : *hmask, thres );
             break;
         
           case GAN_UINT8:
             result = halve_size_bgra_ui8 ( image, mask, average_type, no_neighbours, *himage, (mask == NULL) ? NULL : *hmask, thres );
             break;

           case GAN_UINT16:
             result = halve_size_bgra_ui16 ( image, mask, average_type, no_neighbours, *himage, (mask == NULL) ? NULL : *hmask, thres );
             break;

           case GAN_UINT32:
             result = halve_size_bgra_ui32 ( image, mask, average_type, no_neighbours, *himage, (mask == NULL) ? NULL : *hmask, thres );
             break;

         default:
           gan_err_flush_trace();
           gan_err_register ( "halve_size", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
           gan_image_free ( *himage );
           if ( mask != NULL ) gan_image_free ( *hmask );
           return GAN_FALSE;
      }
      break;

      case GAN_VECTOR_FIELD_2D:
        switch ( image->type )
        {
           case GAN_SHORT:
             result = halve_size_vfield2D_i16 ( image, mask, average_type, no_neighbours, *himage, (mask == NULL) ? NULL : *hmask, thres );
             break;

         case GAN_FLOAT32:
             result = halve_size_vfield2D_f32 ( image, mask, average_type, no_neighbours, *himage, (mask == NULL) ? NULL : *hmask, thres );
             break;

         case GAN_FLOAT64:
             result = halve_size_vfield2D_f64 ( image, mask, average_type, no_neighbours, *himage, (mask == NULL) ? NULL : *hmask, thres );
             break;

         default:
           gan_err_flush_trace();
           gan_err_register ( "halve_size", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
           gan_image_free ( *himage );
           if ( mask != NULL ) gan_image_free ( *hmask );
           return GAN_FALSE;
      }
      break;

      case GAN_VECTOR_FIELD_3D:
      switch ( image->type )
      {
         case GAN_SHORT:
           result = halve_size_vfield3D_i16 ( image, mask, average_type, no_neighbours, *himage, (mask == NULL) ? NULL : *hmask, thres );
           break;

         case GAN_FLOAT32:
             result = halve_size_vfield3D_f32 ( image, mask, average_type, no_neighbours, *himage, (mask == NULL) ? NULL : *hmask, thres );
             break;

         case GAN_FLOAT64:
             result = halve_size_vfield3D_f64 ( image, mask, average_type, no_neighbours, *himage, (mask == NULL) ? NULL : *hmask, thres );
             break;

         default:
           gan_err_flush_trace();
           gan_err_register ( "halve_size", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
           gan_image_free ( *himage );
           if ( mask != NULL ) gan_image_free ( *hmask );
           return GAN_FALSE;
      }
      break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "halve_size", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
        gan_image_free ( *himage );
        if ( mask != NULL ) gan_image_free ( *hmask );
        return GAN_FALSE;
   }

   if ( !result )
   {
      gan_err_register ( "halve_size", GAN_ERROR_FAILURE, "" );
      gan_image_free ( *himage );
      if ( mask != NULL ) gan_image_free ( *hmask );
      return GAN_FALSE;
   }

   /* success */
   return GAN_TRUE;
}

/**
 * \brief Halve the size of an image
 * \param image Input image
 * \param himage Output image or \c NULL
 * \return Result image or \c NULL with error on failure.
 */
Gan_Image *
 gan_image_halve_size ( Gan_Image *image, Gan_Image *himage)
{
   if(!halve_size(image, NULL, GAN_AVERAGE_ALL, 4, &himage, NULL, 4))
   {
      gan_err_register ( "gan_image_halve_size", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   return himage;
}

static Gan_Bool
 shift_mask_down ( Gan_Image *mask )
{
   int i;
   gan_uint16 *ptr;

   gan_err_test_bool ( mask->format == GAN_GREY_LEVEL_IMAGE && mask->type == GAN_UINT16, "shift_mask_down", GAN_ERROR_INCOMPATIBLE, "" );

   for ( i = (int)(mask->width*mask->height)-1, ptr = gan_image_get_pixptr_gl_ui16(mask,0,0); i >= 0; i--, ptr++ )
      *ptr = (*ptr) >> 2;

   return GAN_TRUE;
}


/**
 * \brief Builds an image pyramid.using preallocated image buffers passed in
 * \param img The top-level image in the pyramid
 * \param mask The mask corresponding to the top-level image or NULL
 * \param no_levels The number of levels in the pyramid
 * \param average_type The type of averaging of four-pixel neighbourhoods
 * \param no_neighbours Minimum number of neighbours set to transfer resolution
 * \param imgarray Array of at least no_levels-1 preallocated image buffers or \c NULL
 * \param maskarray Array of at least no_levels-1 preallocated mask buffers or \c NULL
 * \param pyramid Pointer to the pyramid
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Builds an image pyramid. If the \a mask is \c NULL, the pyramid is built
 * without masks, i.e. assuming that all the pixels are valid.
 *
 * \a no_neighbours is the least number of number of pixels in any four-pixel
 * neighbourhood of mask pixels needed to be one in order for the lower
 * resolution mask to be set to one.
 *
 * \sa gan_image_pyramid_free().
 */
Gan_Bool
 gan_image_pyramid_build_preallocated ( Gan_Image *img, Gan_Image *mask, unsigned no_levels,
                                        Gan_ImagePyramidAverage average_type,
                                        unsigned no_neighbours,
                                        Gan_Image **imgarray,
                                        Gan_Image **maskarray,
                                        Gan_ImagePyramid **pyramid )
{
   unsigned i, thres = no_neighbours;

   gan_err_test_bool ( no_levels > 0 && no_neighbours > 0 && no_neighbours <= 4, "gan_image_pyramid_build", GAN_ERROR_ILLEGAL_ARGUMENT, "" );

   /* allocate pyramid array */
   *pyramid = gan_malloc_array ( Gan_ImagePyramid, no_levels );

   if ( *pyramid == NULL )
   {
      gan_err_flush_trace();
      gan_err_register_with_number ( "gan_image_pyramid_build", GAN_ERROR_MALLOC_FAILED, "", no_levels*sizeof(Gan_ImagePyramid) );
      return GAN_FALSE;
   }

   /* set top level of pyramid */
   (*pyramid)[0].img  = img;  (*pyramid)[0].img_alloc  = GAN_FALSE;
   (*pyramid)[0].mask = mask; (*pyramid)[0].mask_alloc = GAN_FALSE;

   /* initialise other levels */
   for ( i = 1; i < no_levels; i++ )
   {
      if(imgarray == NULL || imgarray[i-1] == NULL)
      {
         (*pyramid)[i].img  = NULL;
         (*pyramid)[i].img_alloc = GAN_TRUE;
      }
      else
      {
         (*pyramid)[i].img  = imgarray[i-1];
         (*pyramid)[i].img_alloc = GAN_FALSE;
      }

      if(maskarray == NULL || maskarray[i-1] == NULL)
      {
         (*pyramid)[i].mask = NULL;
         (*pyramid)[i].mask_alloc = GAN_TRUE;
      }
      else
      {
         (*pyramid)[i].mask = maskarray[i-1];
         (*pyramid)[i].mask_alloc = GAN_FALSE;
      }
   }

   /* build pyramid of images and masks */
   for ( i = 1; i < no_levels; i++ )
   {
      if ( !halve_size ( (*pyramid)[i-1].img, (*pyramid)[i-1].mask, average_type, no_neighbours,
                         &(*pyramid)[i].img, (mask == NULL) ? NULL : &(*pyramid)[i].mask, thres ) )
      {
         gan_err_register ( "gan_image_pyramid_build", GAN_ERROR_FAILURE, "" );
         return GAN_FALSE;
      }

      /* if we're averaging in the mask, ensure we don't exceed the 16 bit limit */
      if ( mask != NULL && average_type == GAN_AVERAGE_IN_MASK && no_neighbours < 4 )
      {
         if ( i >= 7 )
         {
            if ( !shift_mask_down((*pyramid)[i].mask) )
            {
               gan_err_register ( "gan_image_pyramid_build", GAN_ERROR_FAILURE, "" );
               return GAN_FALSE;
            }
         }
         else
            thres *= 4;
      }
   }

   /* convert 16-bit masks to binary masks if necessary */
   if ( mask != NULL && average_type == GAN_AVERAGE_IN_MASK && no_neighbours < 4 )
   {
      Gan_Image* pgiTmpMask = gan_image_alloc_b((*pyramid)[1].mask->height, (*pyramid)[1].mask->width);
      for ( i = 1; i < no_levels; i++ )
      {
         if(gan_image_convert_q((*pyramid)[i].mask, GAN_GREY_LEVEL_IMAGE, GAN_BOOL, pgiTmpMask) == NULL
            || gan_image_copy_q(pgiTmpMask, (*pyramid)[i].mask) == NULL)
         {
            gan_err_register ( "gan_image_pyramid_build", GAN_ERROR_FAILURE, "" );
            return GAN_FALSE;
         }
      }

      gan_image_free(pgiTmpMask);
   }

   /* success */
   return GAN_TRUE;
}

/**
 * \brief Frees an image pyramid.
 * \param pyramid Pointer to the pyramid
 * \param no_levels The number of levels in the pyramid
 * \param free_top_level Whether to free the top level image
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Frees an image \a pyramid. The \a free_top_levels flag indicates whether
 * the top-level image should be freed.
 *
 * \sa gan_image_pyramid_build().
 */
void
 gan_image_pyramid_free ( Gan_ImagePyramid *pyramid, unsigned no_levels, Gan_Bool free_top_level )
{
   unsigned i;

   for ( i = no_levels-1; i > 0; i-- )
   {
      if ( pyramid[i].mask != NULL && pyramid[i].mask_alloc)
      {
         gan_image_free ( pyramid[i].mask );
         pyramid[i].mask = NULL;
      }

      if(pyramid[i].img_alloc)
         gan_image_free ( pyramid[i].img );

      pyramid[i].img = NULL;
   }

   if ( free_top_level )
   {
      if ( pyramid[0].mask != NULL && pyramid[0].mask_alloc)
      { 
         gan_image_free ( pyramid[0].mask );
         pyramid[0].mask = NULL;
      }

      if( pyramid[0].img_alloc)
         gan_image_free ( pyramid[0].img );

      pyramid[i].img = NULL;
   }

   free ( pyramid );
}

/**
 * \}
 */

/**
 * \}
 */

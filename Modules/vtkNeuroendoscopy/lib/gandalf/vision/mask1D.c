/**
 * File:          $RCSfile: mask1D.c,v $
 * Module:        Constructing 1D Gaussian convolution masks
 * Part of:       Gandalf Library
 *
 * Revision:      $Revision: 1.1.2.1 $
 * Last edited:   $Date: 2007/10/14 02:33:19 $
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

#include <stdlib.h>
#include <math.h>
#include <gandalf/vision/mask1D.h>
#include <gandalf/common/allocate.h>
#include <gandalf/common/array.h>
#include <gandalf/common/misc_error.h>

/**
 * \addtogroup Vision
 * \{
 */

/**
 * \addtogroup Convolution
 * \{
 */

/**
 * \brief Form a new 1D convolution mask.
 *
 * Don't call this function directly. Use the macro gan_mask1D_form_data()
 * instead.
 */
Gan_Mask1D *
 gan_mask1D_form_gen ( Gan_Mask1D *mask,
                       Gan_Mask1DFormat format, Gan_Type type,
                       void *data, unsigned int size )
{
   gan_heap_push();

   /* make sure mask size is odd */
   gan_err_test_ptr ( (size % 2) == 1, "gan_mask1D_form_gen",
                      GAN_ERROR_INCOMPATIBLE, "mask size must be odd" );

   /* create mask structure if necessary */
   if ( mask == NULL )
   {
      mask = gan_malloc_object(Gan_Mask1D);
      mask->alloc = GAN_TRUE;
   }
   else
      mask->alloc = GAN_FALSE;

   /* allocate mask data if necessary */
   if ( data == NULL )
   {
      switch ( format )
      {
         case GAN_MASK1D_SYMMETRIC:
           mask->size_alloc = 1+size/2;
           switch ( type )
           {
              case GAN_FLOAT:
                mask->data.f = gan_malloc_array(float, mask->size_alloc);
                break;

              case GAN_DOUBLE:
                mask->data.d = gan_malloc_array(double, mask->size_alloc);
                break;

              case GAN_INT:
                mask->data.i = gan_malloc_array(int, mask->size_alloc);
                break;

              case GAN_USHORT:
                mask->data.us = gan_malloc_array(unsigned short, mask->size_alloc);
                break;

              case GAN_UCHAR:
                mask->data.uc = gan_malloc_array(unsigned char, mask->size_alloc);
                break;

              default:
                gan_err_flush_trace();
                gan_err_register ( "gan_mask1D_form_gen", GAN_ERROR_ILLEGAL_TYPE, "" );
                return NULL;
           }
           break;

         case GAN_MASK1D_ANTISYMMETRIC:
           mask->size_alloc = size/2;
           switch ( type )
           {
              case GAN_FLOAT:
                mask->data.f = gan_malloc_array(float, mask->size_alloc);
                break;

              case GAN_DOUBLE:
                mask->data.d = gan_malloc_array(double, mask->size_alloc);
                break;

              case GAN_INT:
                mask->data.i = gan_malloc_array(int, mask->size_alloc);
                break;

              case GAN_USHORT:
                mask->data.us = gan_malloc_array(unsigned short, mask->size_alloc);
                break;

              case GAN_UCHAR:
                mask->data.uc = gan_malloc_array(unsigned char, mask->size_alloc);
                break;

              default:
                gan_err_flush_trace();
                gan_err_register ( "gan_mask1D_form_gen", GAN_ERROR_ILLEGAL_TYPE, "" );
                return NULL;
           }
           break;
           
         case GAN_MASK1D_GENERIC:
           mask->size_alloc = size;
           switch ( type )
           {
              case GAN_FLOAT:
                mask->data.f = gan_malloc_array(float, mask->size_alloc);
                break;

              case GAN_DOUBLE:
                mask->data.d = gan_malloc_array(double, mask->size_alloc);
                break;

              case GAN_INT:
                mask->data.i = gan_malloc_array(int, mask->size_alloc);
                break;

              case GAN_USHORT:
                mask->data.us = gan_malloc_array(unsigned short, mask->size_alloc);
                break;

              case GAN_UCHAR:
                mask->data.uc = gan_malloc_array(unsigned char, mask->size_alloc);
                break;

              default:
                gan_err_flush_trace();
                gan_err_register ( "gan_mask1D_form_gen", GAN_ERROR_ILLEGAL_TYPE, "" );
                return NULL;
         }
         break;

         default:
           gan_err_flush_trace();
           gan_err_register ( "gan_mask1D_form_gen", GAN_ERROR_ILLEGAL_TYPE, "" );
           return NULL;
      }

      mask->data_alloc = GAN_TRUE;
   }
   else
   {
      /* calling routine provides data */
      switch ( type )
      {
         case GAN_FLOAT:
           mask->data.f = (float *) data;
           break;

         case GAN_DOUBLE:
           mask->data.d = (double *) data;
           break;

         case GAN_INT:
           mask->data.i = (int *) data;
           break;

         case GAN_USHORT:
            mask->data.us = (unsigned short *) data;
            break;
            
         case GAN_UCHAR:
            mask->data.uc = (unsigned char *) data;
            break;
            
         default:
           gan_err_flush_trace();
           gan_err_register ( "gan_mask1D_form_gen", GAN_ERROR_ILLEGAL_TYPE, "" );
           return NULL;
      }

      mask->data_alloc = GAN_FALSE;
   }

   /* set mask format, type and size */
   mask->format = format;
   mask->type   = type;
   mask->size   = size;

   gan_heap_pop();
   return mask;
}

/**
 * \brief Copies a convolution mask.
 * \param source The input convolution mask
 * \param dest The output convolution mask
 *
 * Copy 1D convolution mask from source mask to dest mask
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 */
Gan_Bool
 gan_mask1D_copy_q ( Gan_Mask1D *source, Gan_Mask1D *dest )
{
   gan_err_test_bool ( source->type == dest->type, "gan_mask1D_copy_q",
                       GAN_ERROR_INCOMPATIBLE, "convolution masks" );
   switch ( source->format )
   {
      case GAN_MASK1D_SYMMETRIC:
      
      gan_err_test_bool ( dest->size_alloc >= 1+source->size/2,
                          "gan_mask1D_copy_q", GAN_ERROR_INCOMPATIBLE,
                          "convolution masks" );
      switch ( source->type )
      {
         case GAN_FLOAT:
           gan_copy_array_f ( source->data.f, 1, 1+source->size/2,
                              dest->data.f, 1 );
           break;

         case GAN_DOUBLE:
           gan_copy_array_d ( source->data.d, 1, 1+source->size/2,
                              dest->data.d, 1 );
           break;

         case GAN_INT:
           gan_copy_array_i ( source->data.i, 1, 1+source->size/2,
                              dest->data.i, 1 );
           break;

         case GAN_USHORT:
           gan_copy_array_us ( source->data.us, 1, 1+source->size/2,
                              dest->data.us, 1 );
           break;

         case GAN_UCHAR:
            gan_copy_array_uc ( source->data.uc, 1, 1+source->size/2,
               dest->data.uc, 1 );
            break;
         
         default:
           gan_err_flush_trace();
           gan_err_register ( "gan_mask1D_copy_q", GAN_ERROR_ILLEGAL_TYPE, "convolution mask" );
           return GAN_FALSE;
      }
      break;

      case GAN_MASK1D_ANTISYMMETRIC:
      gan_err_test_bool ( dest->size_alloc >= source->size/2,
                          "gan_mask1D_copy_q", GAN_ERROR_INCOMPATIBLE,
                          "convolution masks" );
      switch ( source->type )
      {
         case GAN_FLOAT:
           gan_copy_array_f ( source->data.f, 1, source->size/2,
                              dest->data.f, 1 );
           break;

         case GAN_DOUBLE:
           gan_copy_array_d ( source->data.d, 1, source->size/2,
                              dest->data.d, 1 );
           break;

         case GAN_INT:
           gan_copy_array_i ( source->data.i, 1, source->size/2,
                              dest->data.i, 1 );
           break;

         case GAN_USHORT:
           gan_copy_array_us ( source->data.us, 1, 1+source->size/2,
                              dest->data.us, 1 );
           break;

         case GAN_UCHAR:
            gan_copy_array_uc ( source->data.uc, 1, 1+source->size/2,
               dest->data.uc, 1 );
            break;
         
         default:
           gan_err_flush_trace();
           gan_err_register ( "gan_mask1D_copy_q", GAN_ERROR_ILLEGAL_TYPE, "convolution mask" );
           return GAN_FALSE;
      }
      break;

      case GAN_MASK1D_GENERIC:
      gan_err_test_bool ( dest->size_alloc >= source->size,
                          "gan_mask1D_copy_q", GAN_ERROR_INCOMPATIBLE,
                          "convolution masks" );
      switch ( source->type )
      {
         case GAN_FLOAT:
           gan_copy_array_f ( source->data.f, 1, source->size, dest->data.f,1);
           break;

         case GAN_DOUBLE:
           gan_copy_array_d ( source->data.d, 1, source->size, dest->data.d,1);
           break;

         case GAN_INT:
           gan_copy_array_i ( source->data.i, 1, source->size, dest->data.i,1);
           break;

         case GAN_USHORT:
           gan_copy_array_us ( source->data.us, 1, 1+source->size/2,
                              dest->data.us, 1 );
           break;

         case GAN_UCHAR:
            gan_copy_array_uc ( source->data.uc, 1, 1+source->size/2,
               dest->data.uc, 1 );
            break;
         
         default:
           gan_err_flush_trace();
           gan_err_register ( "gan_mask1D_copy_q", GAN_ERROR_ILLEGAL_TYPE, "convolution mask" );
           return GAN_FALSE;
      }
      break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_mask1D_copy_q", GAN_ERROR_ILLEGAL_TYPE, "convolution mask format" );
        return GAN_FALSE;
   }

   dest->format = source->format;
   dest->size   = source->size;
   return GAN_TRUE;
}

/**
 * \brief Frees a convolution mask.
 * \param mask The convolution mask to be freed
 *
 * Free memory associated with 1D convolution mask.
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 */
Gan_Bool
 gan_mask1D_free ( Gan_Mask1D *mask )
{
   if ( mask->data_alloc )
      switch ( mask->type )
      {
         case GAN_FLOAT:
           free ( mask->data.f );
           break;

         case GAN_DOUBLE:
           free ( mask->data.d );
           break;

         case GAN_INT:
           free ( mask->data.i );
           break;

         case GAN_USHORT:
           free ( mask->data.us );
           break;

         case GAN_UCHAR:
           free ( mask->data.uc );
           break;

         default:
           gan_err_flush_trace();
           gan_err_register ( "gan_mask1D_free", GAN_ERROR_ILLEGAL_TYPE, "convolution mask type" );
           return GAN_FALSE;
      }

   if ( mask->alloc ) free ( mask );
   return GAN_TRUE;
}

/**
 * \brief Make new Gaussian convolution mask.
 * \param type Type of mask element, e.g. GAN_DOUBLE
 * \param sigma Standard deviation of convolution
 * \param mask_size Mask dimension, must be odd
 * \param scale Scale factor for mask entries
 * \param mask_data Pointer to mask data or NULL
 *
 * Allocates, fills and returns a pointer to a new Gaussian convolution mask
 * structure with given size and standard deviation.
 *
 * \return Non-\c NULL Pointer to the created convolution mask structure,
 *   or \c NULL on failure.
 * \sa gan_mask1D_free().
 */
Gan_Mask1D *
 gan_gauss_mask_new ( Gan_Type type, double sigma, unsigned mask_size,
                      double scale, void *mask_data )
{
   Gan_Mask1D *mask;
   int i;

   /* create mask */
   mask = gan_mask1D_alloc_data ( GAN_MASK1D_SYMMETRIC, type, mask_data, mask_size );
   if(mask == NULL)
   {
      gan_err_register ( "gan_gauss_mask_new", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   switch ( type )
   {
      case GAN_FLOAT:
      {
         float x, norm;

         norm = (mask->data.f[0] = 1.0F);
         for ( i = (int)mask_size/2; i > 0; i-- )
         {
            x = (float) i;
            norm += 2.0F*((mask->data.f[i] = (float)exp(-0.5*x*x/(sigma*sigma))));
         }

         /* normalise mask so that elements add up to "scale" parameter */
         norm /= (float)scale;
         for ( i = (int)mask_size/2; i >= 0; i-- )
            mask->data.f[i] /= norm;
      }
      break;

      case GAN_DOUBLE:
      {
         double x, norm;

         norm = (mask->data.d[0] = 1.0);
         for ( i = (int)mask_size/2; i > 0; i-- )
         {
            x = (double) i;
            norm += 2.0*((mask->data.d[i] = exp(-0.5*x*x/(sigma*sigma))));
         }

         /* normalise mask so that elements add up to "scale" parameter */
         norm /= scale;
         for ( i = (int)mask_size/2; i >= 0; i-- )
            mask->data.d[i] /= norm;
      }
      break;

      case GAN_INT:
      {
         double x, norm;

         norm = 1.0;
         for ( i = (int)mask_size/2; i > 0; i-- )
         {
            x = (double) i;
            norm += 2.0*exp(-0.5*x*x/(sigma*sigma));
         }

         /* normalise mask so that elements add up to "scale" parameter */
         norm /= scale;

         mask->data.i[0] = (int) (1.0/norm + 0.5);
         for ( i = (int)mask_size/2; i > 0; i-- )
         {
            x = (double) i;
            mask->data.i[i] = (int) (exp(-0.5*x*x/(sigma*sigma))/norm + 0.5);
         }
      }
      break;

      case GAN_USHORT:
      {
         double x, norm;

         norm = 1.0;
         for ( i = (int)mask_size/2; i > 0; i-- )
         {
            x = (double) i;
            norm += 2.0*exp(-0.5*x*x/(sigma*sigma));
         }

         /* normalise mask so that elements add up to "scale" parameter */
         norm /= scale;

         mask->data.us[0] = (unsigned short) (1.0/norm + 0.5);
         for ( i = (int)mask_size/2; i > 0; i-- )
         {
            x = (double) i;
            mask->data.us[i] = (unsigned short) (exp(-0.5*x*x/(sigma*sigma))/norm + 0.5);
         }
      }
      break;

      case GAN_UCHAR:
      {
         double x, norm;

         norm = 1.0;
         for ( i = (int)mask_size/2; i > 0; i-- )
         {
            x = (double) i;
            norm += 2.0*exp(-0.5*x*x/(sigma*sigma));
         }

         /* normalise mask so that elements add up to "scale" parameter */
         norm /= scale;

         mask->data.uc[0] = (unsigned char) (1.0/norm + 0.5);
         for ( i = (int)mask_size/2; i > 0; i-- )
         {
            x = (double) i;
            mask->data.uc[i] = (unsigned char) (exp(-0.5*x*x/(sigma*sigma))/norm + 0.5);
         }
      }
      break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_gauss_mask_new", GAN_ERROR_ILLEGAL_TYPE, "convolution mask type" );
        return NULL;
   }

   return mask;
}

/**
 * \}
 */

/**
 * \}
 */

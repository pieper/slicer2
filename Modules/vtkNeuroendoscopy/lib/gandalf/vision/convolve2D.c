/**
 * File:          $RCSfile: convolve2D.c,v $
 * Module:        Image 2D convolution routines
 * Part of:       Gandalf Library
 *
 * Revision:      $Revision: 1.1.2.1 $
 * Last edited:   $Date: 2007/10/14 02:33:17 $
 * Author:        $Author: ruetz $
 * Copyright:     (c) 2004 <a href="http://frav.escet.urjc.es">FRAV (Face Recognition & Artificial Vision Group)</a>
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
#include <gandalf/vision/convolve2D.h>
#include <gandalf/common/misc_error.h>
#include <gandalf/image/image_gl_uchar.h>
#include <gandalf/image/image_gl_ushort.h>
#include <gandalf/image/image_gl_int.h>
#include <gandalf/image/image_gl_uint.h>
#include <gandalf/image/image_gl_float.h>
#include <gandalf/image/image_gl_double.h>
#include <gandalf/image/image_rgb_uchar.h>
#include <gandalf/image/image_rgb_ushort.h>
#include <gandalf/image/image_rgb_int.h>
#include <gandalf/image/image_rgb_uint.h>
#include <gandalf/image/image_rgb_float.h>
#include <gandalf/image/image_rgb_double.h>
#include <gandalf/image/image_rgba_uchar.h>
#include <gandalf/image/image_rgba_ushort.h>
#include <gandalf/image/image_rgba_int.h>
#include <gandalf/image/image_rgba_uint.h>
#include <gandalf/image/image_rgba_float.h>
#include <gandalf/image/image_rgba_double.h>
#include <gandalf/image/image_bgr_uchar.h>
#include <gandalf/image/image_bgr_ushort.h>
#include <gandalf/image/image_bgr_int.h>
#include <gandalf/image/image_bgr_uint.h>
#include <gandalf/image/image_bgr_float.h>
#include <gandalf/image/image_bgr_double.h>
#include <gandalf/image/image_bgra_uchar.h>
#include <gandalf/image/image_bgra_ushort.h>
#include <gandalf/image/image_bgra_int.h>
#include <gandalf/image/image_bgra_uint.h>
#include <gandalf/image/image_bgra_float.h>
#include <gandalf/image/image_bgra_double.h>
#include <gandalf/image/image_vfield3D_float.h>
#include <gandalf/image/image_vfield3D_double.h>
#include <gandalf/image/image_vfield3D_int.h>

/**
 * \addtogroup Vision
 * \{
 */

/**
 * \defgroup Convolution Convolution Operations
 * \{
 */

/**
 * \brief 2D convolution function for float arrays.
 * \param source The input array to be convolved.
 * \param sstride The stride of the source array.
 * \param dest The destination array for the convolution.
 * \param dstride The stride of the dest array.
 * \param mask The convolution mask.
 * \param width The width of the source image.
 *
 * Applies a two-dimensional convolution operation to the given \a source
 * array. Both arrays must be of type \c float.
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 * \sa gan_convolve2D_i().
 */
Gan_Bool
 gan_convolve2D_f ( float *source, 
                    int sstride,
                    float *dest,
                    int dstride,
                    Gan_Mask2D *mask, 
                    unsigned long width)
{
   int i, j;
   double total = 0.;

   switch ( mask->format )
   {
      case GAN_MASK2D_SYMMETRIC:
      {
         int di,dj;

         total = 0.;
         for ( i = 0; i< (mask->rows)/2+1; i++, source+=width*sstride ) 
         {
            for ( j = 0; j<(mask->cols)/2+1; j++) 
            {
               if( (mask->cols-2*j-1)==0 ) 
                  dj = 0;
               else 
                  dj = 1; 

               if ( (mask->rows-2*i-1)==0 ) 
                  di = 0;
               else 
                  di = 1;

               total += gan_mat_get_el(mask->data,i,j) *
               ((double)source[j*sstride] + 
                (double)source[(mask->cols-j-1)*sstride]*dj +
                (double)source[(width*(mask->rows-2*i-1)+j)*sstride]*di +
                (double)source[(width*(mask->rows-2*i-1)+
                                mask->cols-j-1)*sstride]*di*dj);
            }
         }
      }
      break;

      case GAN_MASK2D_ANTISYMMETRIC:
      {
         total = 0.;
         for ( i = 0; i< (mask->rows)/2; i++, source+=width*sstride ) 
         {
            for ( j = 0; j<(mask->cols)/2; j++) 
            {
               total += gan_mat_get_el(mask->data,i,j) *
               ((double)source[j*sstride] - 
                (double)source[(mask->cols-j-1)*sstride] -
                (double)source[(width*(mask->rows-2*i-1)+j)*sstride] +
                (double)source[(width*(mask->rows-2*i-1)+mask->cols-j-1)*sstride]);
            }
         }
      }
      break;
            
      case GAN_MASK2D_GENERIC:
      {
         total = 0.;
         for ( i = 0; i< (mask->rows); i++, source+=width*sstride ) 
         {
            for ( j = 0; j<(mask->cols); j++) 
            {
               total += gan_mat_get_el(mask->data,i,j)*((double)source[j*sstride]);
            }
         }
      }
      break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_convolve2D_f", GAN_ERROR_ILLEGAL_TYPE, "" );
        break;
   }

   *dest = (float) total;

   /* success */ 
   return GAN_TRUE;
}


/**
 * \brief 2D convolution function for double arrays.
 * \param source The input array to be convolved.
 * \param sstride The stride of the source array.
 * \param dest The destination array for the convolution.
 * \param dstride The stride of the dest array.
 * \param mask The convolution mask.
 * \param width The width of the source image.
 *
 * Applies a two-dimensional convolution operation to the given \a source
 * array. Both arrays must be of type \c double.
 *
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 * \sa gan_convolve2D_i().
 */
Gan_Bool
 gan_convolve2D_d ( double *source,
                    int sstride,
                    double *dest,
                    int dstride,
                    Gan_Mask2D *mask, 
                    unsigned long width)
{
   int i, j;
   double total = 0.;

   switch ( mask->format )
   {
      case GAN_MASK2D_SYMMETRIC:
      {
         int di,dj;

         total = 0.;
         for ( i = 0; i< (mask->rows)/2+1; i++, source+=width*sstride ) 
         {
            for ( j = 0; j<(mask->cols)/2+1; j++) 
            {
               if( (mask->cols-2*j-1)==0 ) 
                  dj = 0;
               else 
                  dj = 1; 

               if ( (mask->rows-2*i-1)==0 ) 
                  di = 0;
               else 
                  di = 1;

               total += gan_mat_get_el(mask->data,i,j) *
               ((double)source[j*sstride] + 
                (double)source[(mask->cols-j-1)*sstride]*dj +
                (double)source[(width*(mask->rows-2*i-1)+j)*sstride]*di +
                (double)source[(width*(mask->rows-2*i-1)+
                                mask->cols-j-1)*sstride]*di*dj);
            }
         }
      }
      break;

      case GAN_MASK2D_ANTISYMMETRIC:
      {
         total = 0.;
         for ( i = 0; i< (mask->rows)/2; i++, source+=width*sstride ) 
         {
            for ( j = 0; j<(mask->cols)/2; j++) 
            {
               total += gan_mat_get_el(mask->data,i,j) *
               ((double)source[j*sstride] - 
                (double)source[(mask->cols-j-1)*sstride] -
                (double)source[(width*(mask->rows-2*i-1)+j)*sstride] +
                (double)source[(width*(mask->rows-2*i-1)+mask->cols-j-1)*sstride]);
            }
         }
      }
      break;
            
      case GAN_MASK2D_GENERIC:
      {
         total = 0.;
         for ( i = 0; i< (mask->rows); i++, source+=width*sstride ) 
         {
            for ( j = 0; j<(mask->cols); j++) 
            {
               total += gan_mat_get_el(mask->data,i,j)*((double)source[j*sstride]);
            }
         }
      }
      break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_convolve2D_d", GAN_ERROR_ILLEGAL_TYPE, "" );
        break;
   }

   *dest = total;

   /* success */ 
   return GAN_TRUE;
}


/**
 * \brief 2D convolution function for integer arrays.
 * \param source The input array to be convolved.
 * \param sstride The stride of the source array.
 * \param dest The destination array for the convolution.
 * \param dstride The stride of the dest array.
 * \param mask The convolution mask.
 * \param width The width of the source image.
 *
 * Applies a two-dimensional convolution operation to the given \a source
 * array. Both arrays must be of type \c int. There is no checking
 * for overflow.
 *
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 * \sa gan_convolve2D_uc().
 */
Gan_Bool
 gan_convolve2D_i ( int *source,
                    int sstride,
                    int *dest,  
                    int dstride,
                    Gan_Mask2D *mask, 
                    unsigned long width )
{
   int i, j;
   double total = 0.;

   switch ( mask->format )
   {
      case GAN_MASK2D_SYMMETRIC:
      {
         int di,dj;

         total = 0.;
         for ( i = 0; i< (mask->rows)/2+1; i++, source+=width*sstride ) 
         {
            for ( j = 0; j<(mask->cols)/2+1; j++) 
            {
               if( (mask->cols-2*j-1)==0 ) 
                  dj = 0;
               else 
                  dj = 1; 

               if ( (mask->rows-2*i-1)==0 ) 
                  di = 0;
               else 
                  di = 1;

               total += gan_mat_get_el(mask->data,i,j) *
               ((double)source[j*sstride] + 
                (double)source[(mask->cols-j-1)*sstride]*dj +
                (double)source[(width*(mask->rows-2*i-1)+j)*sstride]*di +
                (double)source[(width*(mask->rows-2*i-1)+
                                mask->cols-j-1)*sstride]*di*dj);
            }
         }
      }
      break;

      case GAN_MASK2D_ANTISYMMETRIC:
      {
         total = 0.;
         for ( i = 0; i< (mask->rows)/2; i++, source+=width*sstride ) 
         {
            for ( j = 0; j<(mask->cols)/2; j++) 
            {
               total += gan_mat_get_el(mask->data,i,j) *
               ((double)source[j*sstride] - 
                (double)source[(mask->cols-j-1)*sstride] -
                (double)source[(width*(mask->rows-2*i-1)+j)*sstride] +
                (double)source[(width*(mask->rows-2*i-1)+mask->cols-j-1)*sstride]);
            }
         }
      }
      break;
            
      case GAN_MASK2D_GENERIC:
      {
         total = 0.;
         for ( i = 0; i< (mask->rows); i++, source+=width*sstride ) 
         {
            for ( j = 0; j<(mask->cols); j++) 
            {
               total += gan_mat_get_el(mask->data,i,j)*((double)source[j*sstride]);
            }
         }
      }
      break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_convolve2D_i", GAN_ERROR_ILLEGAL_TYPE, "" );
        break;
   }

   *dest = (int) total;

   /* success */ 
   return GAN_TRUE;
}


/**
 * \brief 2D convolution function for unsigned character arrays.
 * \param source The input array to be convolved.
 * \param sstride The stride of the source array.
 * \param dest The destination array for the convolution.
 * \param dstride The stride of the dest array.
 * \param mask The convolution mask.
 * \param width The width of the source image.
 *
 * Applies a two-dimensional convolution operation to the given \a source
 * array. Both arrays must be of type \c unsigned \c char.
 * Overflow is considered.
 *
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 */
Gan_Bool
 gan_convolve2D_uc ( unsigned char *source, 
                     int sstride,
                     unsigned char *dest,  
                     int dstride,
                     Gan_Mask2D *mask,
                     unsigned long width)
{
   int i, j;
   double total = 0.;

   switch ( mask->format )
   {
      case GAN_MASK2D_SYMMETRIC:
      {
         int di,dj;

         total = 0.;
         for ( i = 0; i< (mask->rows)/2+1; i++, source+=width*sstride ) 
         {
            for ( j = 0; j<(mask->cols)/2+1; j++) 
            {
               if( (mask->cols-2*j-1)==0 ) 
                  dj = 0;
               else 
                  dj = 1; 

               if ( (mask->rows-2*i-1)==0 ) 
                  di = 0;
               else 
                  di = 1;

               total += gan_mat_get_el(mask->data,i,j) *
               ((double)source[j*sstride] + 
                (double)source[(mask->cols-j-1)*sstride]*dj +
                (double)source[(width*(mask->rows-2*i-1)+j)*sstride]*di +
                (double)source[(width*(mask->rows-2*i-1)+
                                mask->cols-j-1)*sstride]*di*dj);
            }
         }
      }
      break;

      case GAN_MASK2D_ANTISYMMETRIC:
      {
         total = 0.;
         for ( i = 0; i< (mask->rows)/2; i++, source+=width*sstride ) 
         {
            for ( j = 0; j<(mask->cols)/2; j++) 
            {
               total += gan_mat_get_el(mask->data,i,j) *
               ((double)source[j*sstride] - 
                (double)source[(mask->cols-j-1)*sstride] -
                (double)source[(width*(mask->rows-2*i-1)+j)*sstride] +
                (double)source[(width*(mask->rows-2*i-1)+mask->cols-j-1)*sstride]);
            }
         }
      }
      break;
            
      case GAN_MASK2D_GENERIC:
      {
         total = 0.;
         for ( i = 0; i< (mask->rows); i++, source+=width*sstride ) 
         {
            for ( j = 0; j<(mask->cols); j++) 
            {
               total += gan_mat_get_el(mask->data,i,j)*((double)source[j*sstride]);
            }
         }
      }
      break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_convolve2D_uc", GAN_ERROR_ILLEGAL_TYPE, "" );
        break;
   }

   // Reassignment when there is overflow
   if (total>=0. && total<=255.) 
      *dest = (unsigned char) total;
   else if (total<=0.) 
      *dest = 0;
   else
      *dest = 255;

   /* success */ 
   return GAN_TRUE;
}

/**
 * \brief 2D convolution function for unsigned short arrays.
 * \param source The input array to be convolved.
 * \param sstride The stride of the source array.
 * \param dest The destination array for the convolution.
 * \param dstride The stride of the dest array.
 * \param mask The convolution mask.
 * \param width The width of the source image.
 *
 * Applies a two-dimensional convolution operation to the given \a source
 * array. Both arrays must be of type \c unsigned \c short.
 * There is no checking for overflow.
 *
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 * \sa gan_convolve2D_us().
 */
Gan_Bool
 gan_convolve2D_us ( unsigned short *source, 
                     int sstride,
                     unsigned short *dest,   
                     int dstride,
                     Gan_Mask2D *mask, 
                     unsigned long width)
{
   int i, j;
   double total = 0.;

   switch ( mask->format )
   {
      case GAN_MASK2D_SYMMETRIC:
      {
         int di,dj;

         total = 0.;
         for ( i = 0; i< (mask->rows)/2+1; i++, source+=width*sstride ) 
         {
            for ( j = 0; j<(mask->cols)/2+1; j++) 
            {
               if( (mask->cols-2*j-1)==0 ) 
                  dj = 0;
               else 
                  dj = 1; 

               if ( (mask->rows-2*i-1)==0 ) 
                  di = 0;
               else 
                  di = 1;

               total += gan_mat_get_el(mask->data,i,j) *
               ((double)source[j*sstride] + 
                (double)source[(mask->cols-j-1)*sstride]*dj +
                (double)source[(width*(mask->rows-2*i-1)+j)*sstride]*di +
                (double)source[(width*(mask->rows-2*i-1)+
                                mask->cols-j-1)*sstride]*di*dj);
            }
         }
      }
      break;

      case GAN_MASK2D_ANTISYMMETRIC:
      {
         total = 0.;
         for ( i = 0; i< (mask->rows)/2; i++, source+=width*sstride ) 
         {
            for ( j = 0; j<(mask->cols)/2; j++) 
            {
               total += gan_mat_get_el(mask->data,i,j) *
               ((double)source[j*sstride] - 
                (double)source[(mask->cols-j-1)*sstride] -
                (double)source[(width*(mask->rows-2*i-1)+j)*sstride] +
                (double)source[(width*(mask->rows-2*i-1)+mask->cols-j-1)*sstride]);
            }
         }
      }
      break;
            
      case GAN_MASK2D_GENERIC:
      {
         total = 0.;
         for ( i = 0; i< (mask->rows); i++, source+=width*sstride ) 
         {
            for ( j = 0; j<(mask->cols); j++) 
            {
               total += gan_mat_get_el(mask->data,i,j)*((double)source[j*sstride]);
            }
         }
      }
      break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_convolve2D_us", GAN_ERROR_ILLEGAL_TYPE, "" );
        break;
   }

   *dest = (unsigned short) total;

   /* success */ 
   return GAN_TRUE;
}


/**
 * \brief 2D convolution function for unsigned integer arrays.
 * \param source The input array to be convolved.
 * \param sstride The stride of the source array.
 * \param dest The destination array for the convolution.
 * \param dstride The stride of the dest array.
 * \param mask The convolution mask.
 * \param width The width of the source image.
 *
 * Applies a two-dimensional convolution operation to the given \a source
 * array. Both arrays must be of type \c unsigned \c int.
 * There is no checking for overflow.
 *
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 */
Gan_Bool
 gan_convolve2D_ui ( unsigned int *source, 
                     int sstride,
                     unsigned int *dest,  
                     int dstride,
                     Gan_Mask2D *mask, 
                     unsigned long width)
{
   int i, j;
   double total = 0.;

   switch ( mask->format )
   {
      case GAN_MASK2D_SYMMETRIC:
      {
         int di,dj;

         total = 0.;
         for ( i = 0; i< (mask->rows)/2+1; i++, source+=width*sstride ) 
         {
            for ( j = 0; j<(mask->cols)/2+1; j++) 
            {
               if( (mask->cols-2*j-1)==0 ) 
                  dj = 0;
               else 
                  dj = 1; 

               if ( (mask->rows-2*i-1)==0 ) 
                  di = 0;
               else 
                  di = 1;

               total += gan_mat_get_el(mask->data,i,j) *
               ((double)source[j*sstride] + 
                (double)source[(mask->cols-j-1)*sstride]*dj +
                (double)source[(width*(mask->rows-2*i-1)+j)*sstride]*di +
                (double)source[(width*(mask->rows-2*i-1)+
                                mask->cols-j-1)*sstride]*di*dj);
            }
         }
      }
      break;

      case GAN_MASK2D_ANTISYMMETRIC:
      {
         total = 0.;
         for ( i = 0; i< (mask->rows)/2; i++, source+=width*sstride ) 
         {
            for ( j = 0; j<(mask->cols)/2; j++) 
            {
               total += gan_mat_get_el(mask->data,i,j) *
               ((double)source[j*sstride] - 
                (double)source[(mask->cols-j-1)*sstride] -
                (double)source[(width*(mask->rows-2*i-1)+j)*sstride] +
                (double)source[(width*(mask->rows-2*i-1)+mask->cols-j-1)*sstride]);
            }
         }
      }
      break;
            
      case GAN_MASK2D_GENERIC:
      {
         total = 0.;
         for ( i = 0; i< (mask->rows); i++, source+=width*sstride ) 
         {
            for ( j = 0; j<(mask->cols); j++) 
            {
               total += gan_mat_get_el(mask->data,i,j)*((double)source[j*sstride]);
            }
         }
      }
      break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_convolve2D_ui", GAN_ERROR_ILLEGAL_TYPE, "" );
        break;
   }

   *dest = (unsigned int) total;

   /* success */ 
   return GAN_TRUE;
}

static Gan_Image * 
 image_convolve2D_gl_gl ( Gan_Image *image, 
                          Gan_Mask2D *mask, 
                          Gan_Image *dest ) 
{ 
   int i,j; 

   gan_err_test_ptr ( image->format == GAN_GREY_LEVEL_IMAGE, 
                      "image_convolve2D_gl_gl", GAN_ERROR_INCOMPATIBLE, "" ); 
   gan_err_test_ptr ( image->width >= mask->cols, "image_convolve2D_gl_gl", 
                      GAN_ERROR_IMAGE_TOO_SMALL, ""); 
   gan_err_test_ptr ( image->height >= mask->rows, "image_convolve2D_gl_gl", 
                      GAN_ERROR_IMAGE_TOO_SMALL, ""); 

   if ( dest == NULL )
      dest = gan_image_alloc ( GAN_GREY_LEVEL_IMAGE, 
                               image->type,
                               image->height-mask->rows+1, 
                               image->width-mask->cols+1 );
   else
      dest = gan_image_set_format_type_dims ( dest,
                                              GAN_GREY_LEVEL_IMAGE,
                                              image->type,
                                              image->height-mask->rows+1,
                                              image->width-mask->cols+1 );

   if ( dest == NULL )
   {
      gan_err_register ( "image_convolve2D_gl_gl", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   dest->offset_x = image->offset_x+(int)mask->cols/2;
   dest->offset_y = image->offset_y+(int)mask->rows/2;

   switch ( image->type )
   {
      case GAN_FLOAT:
        for ( i = 0; i<(int)dest->height; i++ ) 
        {
           for ( j = 0; j<(int)dest->width; j++ ) 
           {
              if ( !gan_convolve2D_f ( gan_image_get_pixptr_gl_f(image,i,j),
                                       1,
                                       gan_image_get_pixptr_gl_f(dest,i,j), 
                                       1,
                                       mask, 
                                       image->width))
              {
                 gan_err_register ( "image_convolve2D_gl_gl", 
                                    GAN_ERROR_FAILURE,
                                    "" );
                 return NULL;
              }
           }
        }
        break;

      case GAN_DOUBLE:
        for ( i = 0; i<(int)dest->height; i++ ) 
        {
           for ( j = 0; j<(int)dest->width; j++ ) 
           {
              if ( !gan_convolve2D_d ( gan_image_get_pixptr_gl_d(image,i,j),
                                       1,
                                       gan_image_get_pixptr_gl_d(dest,i,j), 
                                       1,
                                       mask, 
                                       image->width ) )
              {
                 gan_err_register ( "image_convolve2D_gl_gl", 
                                    GAN_ERROR_FAILURE,
                                    "" );
                 return NULL;
              }
           }
        }
        break;

      case GAN_INT:
        for ( i = 0; i<(int)dest->height; i++ ) 
        {
           for ( j = 0; j<(int)dest->width; j++ ) 
           {
              if ( !gan_convolve2D_i ( gan_image_get_pixptr_gl_i(image,i,j),
                                       1, 
                                       gan_image_get_pixptr_gl_i(dest,i,j), 
                                       1, 
                                       mask, 
                                       image->width ) )
              {
                 gan_err_register ( "image_convolve2D_gl_gl", 
                                    GAN_ERROR_FAILURE,
                                    "" );
                 return NULL;
              }
           }
        }     
        break;

      case GAN_UINT:
        for ( i = 0; i<(int)dest->height; i++ ) 
        {
           for ( j = 0; j<(int)dest->width; j++ ) 
           {
              if ( !gan_convolve2D_ui ( gan_image_get_pixptr_gl_ui(image,i,j),
                                        1,
                                        gan_image_get_pixptr_gl_ui(dest,i,j), 
                                        1,
                                        mask, 
                                        image->width ) )
              {
                 gan_err_register ( "image_convolve2D_gl_gl", 
                                    GAN_ERROR_FAILURE,
                                    "" );
                 return NULL;
              }
           }
        }
        break;

      case GAN_UCHAR:
        for ( i = 0; i<(int)dest->height; i++ ) 
        {
           for ( j = 0; j<(int)dest->width; j++ ) 
           {
              if ( !gan_convolve2D_uc ( gan_image_get_pixptr_gl_uc(image,i,j),
                                        1,
                                        gan_image_get_pixptr_gl_uc(dest,i,j), 
                                        1,
                                        mask, 
                                        image->width ) )
              {
                 gan_err_register ( "image_convolve2D_gl_gl", 
                                    GAN_ERROR_FAILURE,
                                    "" );
                 return NULL;
              }
           }
        }         
        break;

      case GAN_USHORT:
        for ( i = 0; i<(int)dest->height; i++ ) 
        {
           for ( j = 0; j<(int)dest->width; j++ ) 
           {
              if ( !gan_convolve2D_us ( gan_image_get_pixptr_gl_us(image,i,j),
                                        1,
                                        gan_image_get_pixptr_gl_us(dest,i,j), 
                                        1,
                                        mask, 
                                        image->width ) )
              {
                 gan_err_register ( "image_convolve2D_gl_gl", 
                                    GAN_ERROR_FAILURE,
                                    "" );
                 return NULL;
              }
           }
        }
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "image_convolve2D_gl_gl", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
        return NULL;
   }

   /* success */ 
   return dest; 
} 

static Gan_Image * 
 image_convolve2D_v3D_gl ( Gan_Image *image, 
                           Gan_ImageChannelType channel, 
                           Gan_Mask2D *mask, 
                           Gan_Image *dest ) 
{ 
   int i,j;

   gan_err_test_ptr ( image->format == GAN_VECTOR_FIELD_3D,
                      "image_convolve2D_v3D_gl", GAN_ERROR_INCOMPATIBLE, "" );
   gan_err_test_ptr ( image->width >= mask->cols, "image_convolve2D_v3D_gl",
                      GAN_ERROR_IMAGE_TOO_SMALL, "" );
   gan_err_test_ptr ( image->width >= mask->rows, "image_convolve2D_v3D_gl",
                      GAN_ERROR_IMAGE_TOO_SMALL, "" );

   if ( dest == NULL )
      dest = gan_image_alloc ( GAN_GREY_LEVEL_IMAGE, 
                               image->type,
                               image->height-mask->rows+1, 
                               image->width-mask->cols+1 );
   else
      dest = gan_image_set_format_type_dims ( dest,
                                              GAN_GREY_LEVEL_IMAGE,
                                              image->type,
                                              image->height-mask->rows+1,
                                              image->width-mask->cols+1 );


   if ( dest == NULL )
   {
      gan_err_register ( "image_convolve2D_v3D_gl", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   dest->offset_x = image->offset_x+(int)mask->cols/2;
   dest->offset_y = image->offset_y+(int)mask->rows/2;

   switch ( image->type )
   {
      case GAN_INT:

        switch ( channel )
        {
           case GAN_X_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_i ( 
                            &gan_image_get_pixptr_vfield3D_i(image,i,j)->x, 
                            3,
                            gan_image_get_pixptr_gl_i(dest,i,j), 
                            1,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;

           case GAN_Y_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_i ( 
                            &gan_image_get_pixptr_vfield3D_i(image,i,j)->y, 
                            3,
                            gan_image_get_pixptr_gl_i(dest,i,j), 
                            1,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;

           case GAN_Z_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_i ( 
                            &gan_image_get_pixptr_vfield3D_i(image,i,j)->z, 
                            3,
                            gan_image_get_pixptr_gl_i(dest,i,j), 
                            1,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "image_convolve2D_v3D_gl", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;   

      case GAN_FLOAT:

        switch ( channel )
        {
           case GAN_X_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_f ( 
                            &gan_image_get_pixptr_vfield3D_f(image,i,j)->x, 
                            3,
                            gan_image_get_pixptr_gl_f(dest,i,j), 
                            1,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;

           case GAN_Y_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_f ( 
                            &gan_image_get_pixptr_vfield3D_f(image,i,j)->y, 
                            3,
                            gan_image_get_pixptr_gl_f(dest,i,j), 
                            1,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;

           case GAN_Z_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_f ( 
                            &gan_image_get_pixptr_vfield3D_f(image,i,j)->z, 
                            3,
                            gan_image_get_pixptr_gl_f(dest,i,j), 
                            1,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "image_convolve2D_v3D_gl",
                                GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;    

      case GAN_DOUBLE:

        switch ( channel )
        {
           case GAN_X_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_d ( 
                            &gan_image_get_pixptr_vfield3D_d(image,i,j)->x, 
                            3,
                            gan_image_get_pixptr_gl_d(dest,i,j), 
                            1,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;

           case GAN_Y_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_d ( 
                            &gan_image_get_pixptr_vfield3D_d(image,i,j)->y, 
                            3,
                            gan_image_get_pixptr_gl_d(dest,i,j), 
                            1,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;

           case GAN_Z_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_d ( 
                            &gan_image_get_pixptr_vfield3D_d(image,i,j)->z, 
                            3,
                            gan_image_get_pixptr_gl_d(dest,i,j), 
                            1,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "image_convolve2D_v3D_gl",
                                GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;    
      
      default:
        gan_err_flush_trace();
        gan_err_register ( "image_convolve2D_v3D_gl", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
        return NULL;
   }

   /* check if we aborted the loop early because of an error */ 
   if ( i < dest->height-1 )
   {
      gan_err_register ( "image_convolve2D_v3D_gl", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* success */
   return dest; 
} 

static Gan_Image * 
 image_convolve2D_rgb_gl ( Gan_Image *image, 
                           Gan_ImageChannelType channel, 
                           Gan_Mask2D *mask, 
                           Gan_Image *dest ) 
{ 
   int i,j;

   gan_err_test_ptr ( image->format == GAN_RGB_COLOUR_IMAGE,
                      "image_convolve2D_rgb_gl", GAN_ERROR_INCOMPATIBLE, "" );
   gan_err_test_ptr ( image->width >= mask->cols, "image_convolve2D_rgb_gl",
                      GAN_ERROR_IMAGE_TOO_SMALL, "" );
   gan_err_test_ptr ( image->width >= mask->rows, "image_convolve2D_rgb_gl",
                      GAN_ERROR_IMAGE_TOO_SMALL, "" );

   if ( dest == NULL )
      dest = gan_image_alloc ( GAN_GREY_LEVEL_IMAGE, 
                               image->type,
                               image->height-mask->rows+1, 
                               image->width-mask->cols+1 );
   else
      dest = gan_image_set_format_type_dims ( dest,
                                              GAN_GREY_LEVEL_IMAGE,
                                              image->type,
                                              image->height-mask->rows+1,
                                              image->width-mask->cols+1 );


   if ( dest == NULL )
   {
      gan_err_register ( "image_convolve2D_rgb_gl", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   dest->offset_x = image->offset_x+(int)mask->cols/2;
   dest->offset_y = image->offset_y+(int)mask->rows/2;

   switch ( image->type )
   {
      case GAN_INT:

        switch ( channel )
        {
           case GAN_RED_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_i ( 
                            &gan_image_get_pixptr_rgb_i(image,i,j)->R, 
                            3,
                            gan_image_get_pixptr_gl_i(dest,i,j), 
                            1,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;

           case GAN_GREEN_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_i ( 
                            &gan_image_get_pixptr_rgb_i(image,i,j)->G, 
                            3,
                            gan_image_get_pixptr_gl_i(dest,i,j), 
                            1,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;

           case GAN_BLUE_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_i ( 
                            &gan_image_get_pixptr_rgb_i(image,i,j)->B, 
                            3,
                            gan_image_get_pixptr_gl_i(dest,i,j), 
                            1,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "image_convolve2D_rgb_gl",
                                GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;   

      case GAN_UINT:

        switch ( channel )
        {
           case GAN_RED_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_ui ( 
                            &gan_image_get_pixptr_rgb_ui(image,i,j)->R, 
                            3,
                            gan_image_get_pixptr_gl_ui(dest,i,j), 
                            1,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;

           case GAN_GREEN_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_ui ( 
                            &gan_image_get_pixptr_rgb_ui(image,i,j)->G, 
                            3,
                            gan_image_get_pixptr_gl_ui(dest,i,j), 
                            1,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;

           case GAN_BLUE_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             { 
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_ui ( 
                            &gan_image_get_pixptr_rgb_ui(image,i,j)->B, 
                            3,
                            gan_image_get_pixptr_gl_ui(dest,i,j), 
                            1,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "image_convolve2D_rgb_gl",
                                GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;    

      case GAN_FLOAT:

        switch ( channel )
        {
           case GAN_RED_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_f ( 
                            &gan_image_get_pixptr_rgb_f(image,i,j)->R, 
                            3,
                            gan_image_get_pixptr_gl_f(dest,i,j), 
                            1,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;

           case GAN_GREEN_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_f ( 
                            &gan_image_get_pixptr_rgb_f(image,i,j)->G, 
                            3,
                            gan_image_get_pixptr_gl_f(dest,i,j), 
                            1,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;

           case GAN_BLUE_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_f ( 
                            &gan_image_get_pixptr_rgb_f(image,i,j)->B, 
                            3,
                            gan_image_get_pixptr_gl_f(dest,i,j), 
                            1,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "image_convolve2D_rgb_gl",
                                GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;    

      case GAN_DOUBLE:

        switch ( channel )
        {
           case GAN_RED_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_d ( 
                            &gan_image_get_pixptr_rgb_d(image,i,j)->R, 
                            3,
                            gan_image_get_pixptr_gl_d(dest,i,j), 
                            1,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;

           case GAN_GREEN_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_d ( 
                            &gan_image_get_pixptr_rgb_d(image,i,j)->G, 
                            3,
                            gan_image_get_pixptr_gl_d(dest,i,j), 
                            1,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;

           case GAN_BLUE_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_d ( 
                            &gan_image_get_pixptr_rgb_d(image,i,j)->B, 
                            3,
                            gan_image_get_pixptr_gl_d(dest,i,j), 
                            1,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "image_convolve2D_rgb_gl",
                                GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;    
     
      case GAN_UCHAR:

        switch ( channel )
        {
           case GAN_RED_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_uc ( 
                            &gan_image_get_pixptr_rgb_uc(image,i,j)->R, 
                            3,
                            gan_image_get_pixptr_gl_uc(dest,i,j), 
                            1,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;

           case GAN_GREEN_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_uc ( 
                            &gan_image_get_pixptr_rgb_uc(image,i,j)->G, 
                            3,
                            gan_image_get_pixptr_gl_uc(dest,i,j), 
                            1,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;

           case GAN_BLUE_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_uc ( 
                            &gan_image_get_pixptr_rgb_uc(image,i,j)->B, 
                            3,
                            gan_image_get_pixptr_gl_uc(dest,i,j), 
                            1,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "image_convolve2D_rgb_gl",
                                GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;    

      case GAN_USHORT:

        switch ( channel )
        {
           case GAN_RED_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_us ( 
                            &gan_image_get_pixptr_rgb_us(image,i,j)->R, 
                            3,
                            gan_image_get_pixptr_gl_us(dest,i,j), 
                            1,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;

           case GAN_GREEN_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_us ( 
                            &gan_image_get_pixptr_rgb_us(image,i,j)->G, 
                            3,
                            gan_image_get_pixptr_gl_us(dest,i,j), 
                            1,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;

           case GAN_BLUE_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_us ( 
                            &gan_image_get_pixptr_rgb_us(image,i,j)->B, 
                            3,
                            gan_image_get_pixptr_gl_us(dest,i,j), 
                            1,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "image_convolve2D_rgb_gl",
                                GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "image_convolve2D_rgb_gl", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
        return NULL;
   }

   /* check if we aborted the loop early because of an error */ 
   if ( i < dest->height-1 )
   {
      gan_err_register ( "image_convolve2D_rgb_gl", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* success */
   return dest; 
} 


static Gan_Image * 
 image_convolve2D_rgba_gl ( Gan_Image *image, 
                            Gan_ImageChannelType channel, 
                            Gan_Mask2D *mask, 
                            Gan_Image *dest ) 
{ 
   int i,j;

   gan_err_test_ptr ( image->format == GAN_RGB_COLOUR_ALPHA_IMAGE,
                      "image_convolve2D_rgba_gl", GAN_ERROR_INCOMPATIBLE, "" );
   gan_err_test_ptr ( image->width >= mask->cols, "image_convolve2D_rgba_gl",
                      GAN_ERROR_IMAGE_TOO_SMALL, "" );
   gan_err_test_ptr ( image->width >= mask->rows, "image_convolve2D_rgba_gl",
                      GAN_ERROR_IMAGE_TOO_SMALL, "" );

   if ( dest == NULL )
      dest = gan_image_alloc ( GAN_GREY_LEVEL_IMAGE, 
                               image->type,
                               image->height-mask->rows+1, 
                               image->width-mask->cols+1 );
   else
      dest = gan_image_set_format_type_dims ( dest,
                                              GAN_GREY_LEVEL_IMAGE,
                                              image->type,
                                              image->height-mask->rows+1,
                                              image->width-mask->cols+1 );


   if ( dest == NULL )
   {
      gan_err_register ( "image_convolve2D_rgba_gl", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   dest->offset_x = image->offset_x+(int)mask->cols/2;
   dest->offset_y = image->offset_y+(int)mask->rows/2;

   switch ( image->type )
   {
      case GAN_INT:

        switch ( channel )
        {
           case GAN_RED_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_i ( 
                            &gan_image_get_pixptr_rgba_i(image,i,j)->R, 
                            4,
                            gan_image_get_pixptr_gl_i(dest,i,j), 
                            1,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;

           case GAN_GREEN_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_i ( 
                            &gan_image_get_pixptr_rgba_i(image,i,j)->G, 
                            4,
                            gan_image_get_pixptr_gl_i(dest,i,j), 
                            1,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;

           case GAN_BLUE_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_i ( 
                            &gan_image_get_pixptr_rgba_i(image,i,j)->B, 
                            4,
                            gan_image_get_pixptr_gl_i(dest,i,j), 
                            1,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;
           
           default:
             gan_err_flush_trace();
             gan_err_register ( "image_convolve2D_rgba_gl",
                                GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;   
      
      case GAN_UINT:

        switch ( channel )
        {
           case GAN_RED_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_ui ( 
                            &gan_image_get_pixptr_rgba_ui(image,i,j)->R, 
                            4,
                            gan_image_get_pixptr_gl_ui(dest,i,j), 
                            1,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;

           case GAN_GREEN_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_ui ( 
                            &gan_image_get_pixptr_rgba_ui(image,i,j)->G, 
                            4,
                            gan_image_get_pixptr_gl_ui(dest,i,j), 
                            1,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;

           case GAN_BLUE_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_ui ( 
                            &gan_image_get_pixptr_rgba_ui(image,i,j)->B, 
                            4,
                            gan_image_get_pixptr_gl_ui(dest,i,j), 
                            1,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;
           
           default:
             gan_err_flush_trace();
             gan_err_register ( "image_convolve2D_rgba_gl",
                                GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;    

      case GAN_FLOAT:

        switch ( channel )
        {
           case GAN_RED_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_f ( 
                            &gan_image_get_pixptr_rgba_f(image,i,j)->R, 
                            4,
                            gan_image_get_pixptr_gl_f(dest,i,j), 
                            1,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;
           
           case GAN_GREEN_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_f ( 
                            &gan_image_get_pixptr_rgba_f(image,i,j)->G, 
                            4,
                            gan_image_get_pixptr_gl_f(dest,i,j), 
                            1,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;

           case GAN_BLUE_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_f ( 
                            &gan_image_get_pixptr_rgba_f(image,i,j)->B, 
                            4,
                            gan_image_get_pixptr_gl_f(dest,i,j), 
                            1,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "image_convolve2D_rgba_gl",
                                GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;    

      case GAN_DOUBLE:

        switch ( channel )
        {
           case GAN_RED_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_d ( 
                            &gan_image_get_pixptr_rgba_d(image,i,j)->R, 
                            4,
                            gan_image_get_pixptr_gl_d(dest,i,j), 
                            1,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;

           case GAN_GREEN_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_d ( 
                            &gan_image_get_pixptr_rgba_d(image,i,j)->G, 
                            4,
                            gan_image_get_pixptr_gl_d(dest,i,j), 
                            1,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;

           case GAN_BLUE_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_d ( 
                            &gan_image_get_pixptr_rgba_d(image,i,j)->B, 
                            4,
                            gan_image_get_pixptr_gl_d(dest,i,j), 
                            1,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "image_convolve2D_rgba_gl",
                                GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;    
      
      case GAN_UCHAR:

        switch ( channel )
        {
           case GAN_RED_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_uc ( 
                            &gan_image_get_pixptr_rgba_uc(image,i,j)->R, 
                            4,
                            gan_image_get_pixptr_gl_uc(dest,i,j), 
                            1,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;

           case GAN_GREEN_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_uc ( 
                            &gan_image_get_pixptr_rgba_uc(image,i,j)->G, 
                            4,
                            gan_image_get_pixptr_gl_uc(dest,i,j), 
                            1,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;

           case GAN_BLUE_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_uc ( 
                            &gan_image_get_pixptr_rgba_uc(image,i,j)->B, 
                            4,
                            gan_image_get_pixptr_gl_uc(dest,i,j), 
                            1,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "image_convolve2D_rgba_gl",
                                GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;    

      case GAN_USHORT:

        switch ( channel )
        {
           case GAN_RED_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_us ( 
                            &gan_image_get_pixptr_rgba_us(image,i,j)->R, 
                            4,
                            gan_image_get_pixptr_gl_us(dest,i,j), 
                            1,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;

           case GAN_GREEN_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_us ( 
                            &gan_image_get_pixptr_rgba_us(image,i,j)->G, 
                            4,
                            gan_image_get_pixptr_gl_us(dest,i,j), 
                            1,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;

           case GAN_BLUE_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_us ( 
                            &gan_image_get_pixptr_rgba_us(image,i,j)->B, 
                            4,
                            gan_image_get_pixptr_gl_us(dest,i,j), 
                            1,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "image_convolve2D_rgba_gl",
                                GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "image_convolve2D_rgba_gl", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
        return NULL;
   }

   /* check if we aborted the loop early because of an error */ 
   if ( i < dest->height-1 )
   {
      gan_err_register ( "image_convolve2D_rgba_gl", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* success */
   return dest; 
} 
   
static Gan_Image * 
 image_convolve2D_bgr_gl ( Gan_Image *image, 
                           Gan_ImageChannelType channel, 
                           Gan_Mask2D *mask, 
                           Gan_Image *dest ) 
{ 
   int i,j;

   gan_err_test_ptr ( image->format == GAN_BGR_COLOUR_IMAGE,
                      "image_convolve2D_bgr_gl", GAN_ERROR_INCOMPATIBLE, "" );
   gan_err_test_ptr ( image->width >= mask->cols, "image_convolve2D_bgr_gl",
                      GAN_ERROR_IMAGE_TOO_SMALL, "" );
   gan_err_test_ptr ( image->width >= mask->rows, "image_convolve2D_bgr_gl",
                      GAN_ERROR_IMAGE_TOO_SMALL, "" );

   if ( dest == NULL )
      dest = gan_image_alloc ( GAN_GREY_LEVEL_IMAGE, 
                               image->type,
                               image->height-mask->rows+1, 
                               image->width-mask->cols+1 );
   else
      dest = gan_image_set_format_type_dims ( dest,
                                              GAN_GREY_LEVEL_IMAGE,
                                              image->type,
                                              image->height-mask->rows+1,
                                              image->width-mask->cols+1 );


   if ( dest == NULL )
   {
      gan_err_register ( "image_convolve2D_bgr_gl", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   dest->offset_x = image->offset_x+(int)mask->cols/2;
   dest->offset_y = image->offset_y+(int)mask->rows/2;

   switch ( image->type )
   {
      case GAN_INT:

        switch ( channel )
        {
           case GAN_RED_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_i ( 
                            &gan_image_get_pixptr_bgr_i(image,i,j)->R, 
                            3,
                            gan_image_get_pixptr_gl_i(dest,i,j), 
                            1,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;

           case GAN_GREEN_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_i ( 
                            &gan_image_get_pixptr_bgr_i(image,i,j)->G, 
                            3,
                            gan_image_get_pixptr_gl_i(dest,i,j), 
                            1,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;

           case GAN_BLUE_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_i ( 
                            &gan_image_get_pixptr_bgr_i(image,i,j)->B, 
                            3,
                            gan_image_get_pixptr_gl_i(dest,i,j), 
                            1,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "image_convolve2D_bgr_gl",
                                GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;   

      case GAN_UINT:

        switch ( channel )
        {
           case GAN_RED_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_ui ( 
                            &gan_image_get_pixptr_bgr_ui(image,i,j)->R, 
                            3,
                            gan_image_get_pixptr_gl_ui(dest,i,j), 
                            1,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;

           case GAN_GREEN_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_ui ( 
                            &gan_image_get_pixptr_bgr_ui(image,i,j)->G, 
                            3,
                            gan_image_get_pixptr_gl_ui(dest,i,j), 
                            1,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;

           case GAN_BLUE_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             { 
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_ui ( 
                            &gan_image_get_pixptr_bgr_ui(image,i,j)->B, 
                            3,
                            gan_image_get_pixptr_gl_ui(dest,i,j), 
                            1,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "image_convolve2D_bgr_gl",
                                GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;    

      case GAN_FLOAT:

        switch ( channel )
        {
           case GAN_RED_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_f ( 
                            &gan_image_get_pixptr_bgr_f(image,i,j)->R, 
                            3,
                            gan_image_get_pixptr_gl_f(dest,i,j), 
                            1,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;

           case GAN_GREEN_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_f ( 
                            &gan_image_get_pixptr_bgr_f(image,i,j)->G, 
                            3,
                            gan_image_get_pixptr_gl_f(dest,i,j), 
                            1,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;

           case GAN_BLUE_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_f ( 
                            &gan_image_get_pixptr_bgr_f(image,i,j)->B, 
                            3,
                            gan_image_get_pixptr_gl_f(dest,i,j), 
                            1,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "image_convolve2D_bgr_gl",
                                GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;    

      case GAN_DOUBLE:

        switch ( channel )
        {
           case GAN_RED_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_d ( 
                            &gan_image_get_pixptr_bgr_d(image,i,j)->R, 
                            3,
                            gan_image_get_pixptr_gl_d(dest,i,j), 
                            1,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;

           case GAN_GREEN_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_d ( 
                            &gan_image_get_pixptr_bgr_d(image,i,j)->G, 
                            3,
                            gan_image_get_pixptr_gl_d(dest,i,j), 
                            1,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;

           case GAN_BLUE_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_d ( 
                            &gan_image_get_pixptr_bgr_d(image,i,j)->B, 
                            3,
                            gan_image_get_pixptr_gl_d(dest,i,j), 
                            1,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "image_convolve2D_bgr_gl",
                                GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;    
     
      case GAN_UCHAR:

        switch ( channel )
        {
           case GAN_RED_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_uc ( 
                            &gan_image_get_pixptr_bgr_uc(image,i,j)->R, 
                            3,
                            gan_image_get_pixptr_gl_uc(dest,i,j), 
                            1,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;

           case GAN_GREEN_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_uc ( 
                            &gan_image_get_pixptr_bgr_uc(image,i,j)->G, 
                            3,
                            gan_image_get_pixptr_gl_uc(dest,i,j), 
                            1,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;

           case GAN_BLUE_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_uc ( 
                            &gan_image_get_pixptr_bgr_uc(image,i,j)->B, 
                            3,
                            gan_image_get_pixptr_gl_uc(dest,i,j), 
                            1,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "image_convolve2D_bgr_gl",
                                GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;    

      case GAN_USHORT:

        switch ( channel )
        {
           case GAN_RED_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_us ( 
                            &gan_image_get_pixptr_bgr_us(image,i,j)->R, 
                            3,
                            gan_image_get_pixptr_gl_us(dest,i,j), 
                            1,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;

           case GAN_GREEN_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_us ( 
                            &gan_image_get_pixptr_bgr_us(image,i,j)->G, 
                            3,
                            gan_image_get_pixptr_gl_us(dest,i,j), 
                            1,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;

           case GAN_BLUE_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_us ( 
                            &gan_image_get_pixptr_bgr_us(image,i,j)->B, 
                            3,
                            gan_image_get_pixptr_gl_us(dest,i,j), 
                            1,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "image_convolve2D_bgr_gl",
                                GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "image_convolve2D_bgr_gl", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
        return NULL;
   }

   /* check if we aborted the loop early because of an error */ 
   if ( i < dest->height-1 )
   {
      gan_err_register ( "image_convolve2D_bgr_gl", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* success */
   return dest; 
} 


static Gan_Image * 
 image_convolve2D_bgra_gl ( Gan_Image *image, 
                            Gan_ImageChannelType channel, 
                            Gan_Mask2D *mask, 
                            Gan_Image *dest ) 
{ 
   int i,j;

   gan_err_test_ptr ( image->format == GAN_BGR_COLOUR_ALPHA_IMAGE,
                      "image_convolve2D_bgra_gl", GAN_ERROR_INCOMPATIBLE, "" );
   gan_err_test_ptr ( image->width >= mask->cols, "image_convolve2D_bgra_gl",
                      GAN_ERROR_IMAGE_TOO_SMALL, "" );
   gan_err_test_ptr ( image->width >= mask->rows, "image_convolve2D_bgra_gl",
                      GAN_ERROR_IMAGE_TOO_SMALL, "" );

   if ( dest == NULL )
      dest = gan_image_alloc ( GAN_GREY_LEVEL_IMAGE, 
                               image->type,
                               image->height-mask->rows+1, 
                               image->width-mask->cols+1 );
   else
      dest = gan_image_set_format_type_dims ( dest,
                                              GAN_GREY_LEVEL_IMAGE,
                                              image->type,
                                              image->height-mask->rows+1,
                                              image->width-mask->cols+1 );


   if ( dest == NULL )
   {
      gan_err_register ( "image_convolve2D_bgra_gl", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   dest->offset_x = image->offset_x+(int)mask->cols/2;
   dest->offset_y = image->offset_y+(int)mask->rows/2;

   switch ( image->type )
   {
      case GAN_INT:

        switch ( channel )
        {
           case GAN_RED_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_i ( 
                            &gan_image_get_pixptr_bgra_i(image,i,j)->R, 
                            4,
                            gan_image_get_pixptr_gl_i(dest,i,j), 
                            1,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;

           case GAN_GREEN_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_i ( 
                            &gan_image_get_pixptr_bgra_i(image,i,j)->G, 
                            4,
                            gan_image_get_pixptr_gl_i(dest,i,j), 
                            1,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;

           case GAN_BLUE_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_i ( 
                            &gan_image_get_pixptr_bgra_i(image,i,j)->B, 
                            4,
                            gan_image_get_pixptr_gl_i(dest,i,j), 
                            1,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;
           
           default:
             gan_err_flush_trace();
             gan_err_register ( "image_convolve2D_bgra_gl",
                                GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;   
      
      case GAN_UINT:

        switch ( channel )
        {
           case GAN_RED_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_ui ( 
                            &gan_image_get_pixptr_bgra_ui(image,i,j)->R, 
                            4,
                            gan_image_get_pixptr_gl_ui(dest,i,j), 
                            1,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;

           case GAN_GREEN_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_ui ( 
                            &gan_image_get_pixptr_bgra_ui(image,i,j)->G, 
                            4,
                            gan_image_get_pixptr_gl_ui(dest,i,j), 
                            1,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;

           case GAN_BLUE_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_ui ( 
                            &gan_image_get_pixptr_bgra_ui(image,i,j)->B, 
                            4,
                            gan_image_get_pixptr_gl_ui(dest,i,j), 
                            1,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;
           
           default:
             gan_err_flush_trace();
             gan_err_register ( "image_convolve2D_bgra_gl",
                                GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;    

      case GAN_FLOAT:

        switch ( channel )
        {
           case GAN_RED_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_f ( 
                            &gan_image_get_pixptr_bgra_f(image,i,j)->R, 
                            4,
                            gan_image_get_pixptr_gl_f(dest,i,j), 
                            1,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;
           
           case GAN_GREEN_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_f ( 
                            &gan_image_get_pixptr_bgra_f(image,i,j)->G, 
                            4,
                            gan_image_get_pixptr_gl_f(dest,i,j), 
                            1,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;

           case GAN_BLUE_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_f ( 
                            &gan_image_get_pixptr_bgra_f(image,i,j)->B, 
                            4,
                            gan_image_get_pixptr_gl_f(dest,i,j), 
                            1,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "image_convolve2D_bgra_gl",
                                GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;    

      case GAN_DOUBLE:

        switch ( channel )
        {
           case GAN_RED_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_d ( 
                            &gan_image_get_pixptr_bgra_d(image,i,j)->R, 
                            4,
                            gan_image_get_pixptr_gl_d(dest,i,j), 
                            1,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;

           case GAN_GREEN_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_d ( 
                            &gan_image_get_pixptr_bgra_d(image,i,j)->G, 
                            4,
                            gan_image_get_pixptr_gl_d(dest,i,j), 
                            1,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;

           case GAN_BLUE_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_d ( 
                            &gan_image_get_pixptr_bgra_d(image,i,j)->B, 
                            4,
                            gan_image_get_pixptr_gl_d(dest,i,j), 
                            1,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "image_convolve2D_bgra_gl",
                                GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;    
      
      case GAN_UCHAR:

        switch ( channel )
        {
           case GAN_RED_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_uc ( 
                            &gan_image_get_pixptr_bgra_uc(image,i,j)->R, 
                            4,
                            gan_image_get_pixptr_gl_uc(dest,i,j), 
                            1,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;

           case GAN_GREEN_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_uc ( 
                            &gan_image_get_pixptr_bgra_uc(image,i,j)->G, 
                            4,
                            gan_image_get_pixptr_gl_uc(dest,i,j), 
                            1,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;

           case GAN_BLUE_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_uc ( 
                            &gan_image_get_pixptr_bgra_uc(image,i,j)->B, 
                            4,
                            gan_image_get_pixptr_gl_uc(dest,i,j), 
                            1,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "image_convolve2D_bgra_gl",
                                GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;    

      case GAN_USHORT:

        switch ( channel )
        {
           case GAN_RED_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_us ( 
                            &gan_image_get_pixptr_bgra_us(image,i,j)->R, 
                            4,
                            gan_image_get_pixptr_gl_us(dest,i,j), 
                            1,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;

           case GAN_GREEN_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_us ( 
                            &gan_image_get_pixptr_bgra_us(image,i,j)->G, 
                            4,
                            gan_image_get_pixptr_gl_us(dest,i,j), 
                            1,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;

           case GAN_BLUE_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_us ( 
                            &gan_image_get_pixptr_bgra_us(image,i,j)->B, 
                            4,
                            gan_image_get_pixptr_gl_us(dest,i,j), 
                            1,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "image_convolve2D_bgra_gl",
                                GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "image_convolve2D_bgra_gl", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
        return NULL;
   }

   /* check if we aborted the loop early because of an error */ 
   if ( i < dest->height-1 )
   {
      gan_err_register ( "image_convolve2D_bgra_gl", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* success */
   return dest; 
} 
   
static Gan_Image * 
 image_convolve2D_gl_v3D ( Gan_Image *image, 
                           Gan_ImageChannelType channel, 
                           Gan_Mask2D *mask, 
                           Gan_Image *dest ) 
{ 
   int i,j;

   gan_err_test_ptr ( image->format == GAN_GREY_LEVEL_IMAGE,
                      "image_convolve2D_gl_v3D", GAN_ERROR_INCOMPATIBLE, "" );
   gan_err_test_ptr ( image->width >= mask->cols, "image_convolve2D_gl_v3D",
                      GAN_ERROR_IMAGE_TOO_SMALL, "" );
   gan_err_test_ptr ( image->width >= mask->rows, "image_convolve2D_gl_v3D",
                      GAN_ERROR_IMAGE_TOO_SMALL, "" );

   if ( dest == NULL )
      dest = gan_image_alloc ( GAN_VECTOR_FIELD_3D,
                               image->type,
                               image->height-mask->rows+1, 
                               image->width-mask->cols+1 );
   else
      dest = gan_image_set_format_type_dims ( dest,
                                              GAN_VECTOR_FIELD_3D,
                                              image->type,
                                              image->height-mask->rows+1,
                                              image->width-mask->cols+1 );


   if ( dest == NULL )
   {
      gan_err_register ( "image_convolve2D_gl_v3D", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   dest->offset_x = image->offset_x+(int)mask->cols/2;
   dest->offset_y = image->offset_y+(int)mask->rows/2;

   switch ( image->type )
   {
      case GAN_INT:

        switch ( channel )
        {
           case GAN_X_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_i ( 
                            gan_image_get_pixptr_gl_i(image,i,j), 
                            1,
                            &gan_image_get_pixptr_vfield3D_i(dest,i,j)->x, 
                            3,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;

           case GAN_Y_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_i ( 
                            gan_image_get_pixptr_gl_i(image,i,j), 
                            1,
                            &gan_image_get_pixptr_vfield3D_i(dest,i,j)->y, 
                            3,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;

           case GAN_Z_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_i ( 
                            gan_image_get_pixptr_gl_i(image,i,j), 
                            1,
                            &gan_image_get_pixptr_vfield3D_i(dest,i,j)->z, 
                            3,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "image_convolve2D_v3D_gl",
                                GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;   

      case GAN_DOUBLE:

        switch ( channel )
        {
           case GAN_X_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_d ( 
                            gan_image_get_pixptr_gl_d(image,i,j), 
                            1,
                            &gan_image_get_pixptr_vfield3D_d(dest,i,j)->x, 
                            3,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;

           case GAN_Y_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_d ( 
                            gan_image_get_pixptr_gl_d(image,i,j), 
                            1,
                            &gan_image_get_pixptr_vfield3D_d(dest,i,j)->y, 
                            3,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;

           case GAN_Z_CHANNEL:
             for ( i = 0; i<(int)dest->height; i++ ) 
             {
                for ( j = 0; j<(int)dest->width; j++ ) 
                {
                   if ( !gan_convolve2D_d ( 
                            gan_image_get_pixptr_gl_d(image,i,j), 
                            1,
                            &gan_image_get_pixptr_vfield3D_d(dest,i,j)->z, 
                            3,
                            mask, 
                            image->width ) )
                   {               
                      break;
                   }
                }
             }
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "image_convolve2D_v3D_gl",
                                GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;   



      default:
        gan_err_flush_trace();
        gan_err_register ( "image_convolve2D_gl_v3D", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
        return NULL;
   }

   /* check if we aborted the loop early because of an error */ /**/
   if ( i >= 0 )
   {
      gan_err_register ( "image_convolve2D_gl_v3D", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* success */ 
   return dest; 
} 

static Gan_Image * 
 image_convolve2D_v3D_v3D ( Gan_Image *image, 
                            Gan_Mask2D *mask, 
                            Gan_Image *dest ) 
{ 
   int i,j;

   gan_err_test_ptr ( image->format == GAN_VECTOR_FIELD_3D,
                      "image_convolve2D_v3D_v3D", GAN_ERROR_INCOMPATIBLE, "");
   gan_err_test_ptr ( image->width >= mask->cols, "image_convolve2D_v3D_v3D",
                      GAN_ERROR_IMAGE_TOO_SMALL, "" );
   gan_err_test_ptr ( image->width >= mask->rows, "image_convolve2D_v3D_v3D",
                      GAN_ERROR_IMAGE_TOO_SMALL, "" );

   if ( dest == NULL )
      dest = gan_image_alloc ( GAN_VECTOR_FIELD_3D,
                               image->type,
                               image->height-mask->rows+1, 
                               image->width-mask->cols+1 );
   else
      dest = gan_image_set_format_type_dims ( dest,
                                              GAN_VECTOR_FIELD_3D,
                                              image->type,
                                              image->height-mask->rows+1,
                                              image->width-mask->cols+1 );

   if ( dest == NULL )
   {
      gan_err_register ( "image_convolve2D_v3D_v3D", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   dest->offset_x = image->offset_x+(int)mask->cols/2;
   dest->offset_y = image->offset_y+(int)mask->rows/2;

   switch ( image->type )
   {
      case GAN_DOUBLE:
        for ( i = 0; i<(int)dest->height; i++ ) 
        {
           for ( j = 0; j<(int)dest->width; j++ ) 
           {
              if ( !gan_convolve2D_d ( 
                       &gan_image_get_pixptr_vfield3D_d(image,i,j)->x, 
                       3,
                       &gan_image_get_pixptr_vfield3D_d(dest,i,j)->x, 
                       3,
                       mask, 
                       image->width ) ||
                   !gan_convolve2D_d ( 
                       &gan_image_get_pixptr_vfield3D_d(image,i,j)->y, 
                       3,
                       &gan_image_get_pixptr_vfield3D_d(dest,i,j)->y, 
                       3,
                       mask, 
                       image->width ) ||
                   !gan_convolve2D_d ( 
                       &gan_image_get_pixptr_vfield3D_d(image,i,j)->z, 
                       3,
                       &gan_image_get_pixptr_vfield3D_d(dest,i,j)->z, 
                       3,
                       mask, 
                       image->width ) )
              {               
                 break;
              }
           }
        }
        break;

      case GAN_FLOAT:
        for ( i = 0; i<(int)dest->height; i++ ) 
        {
           for ( j = 0; j<(int)dest->width; j++ ) 
           {
              if ( !gan_convolve2D_f ( 
                       &gan_image_get_pixptr_vfield3D_f(image,i,j)->x, 
                       3,
                       &gan_image_get_pixptr_vfield3D_f(dest,i,j)->x, 
                       3,
                       mask, 
                       image->width ) ||
                   !gan_convolve2D_f ( 
                       &gan_image_get_pixptr_vfield3D_f(image,i,j)->y, 
                       3,
                       &gan_image_get_pixptr_vfield3D_f(dest,i,j)->y, 
                       3,
                       mask, 
                       image->width ) ||
                   !gan_convolve2D_f ( 
                       &gan_image_get_pixptr_vfield3D_f(image,i,j)->z, 
                       3,
                       &gan_image_get_pixptr_vfield3D_f(dest,i,j)->z, 
                       3,
                       mask, 
                       image->width ) )
              {               
                 break;
              }
           }
        }
        break;
        
      case GAN_INT:
        for ( i = 0; i<(int)dest->height; i++ ) 
        {
           for ( j = 0; j<(int)dest->width; j++ ) 
           {
              if ( !gan_convolve2D_i ( 
                       &gan_image_get_pixptr_vfield3D_i(image,i,j)->x, 
                       3,
                       &gan_image_get_pixptr_vfield3D_i(dest,i,j)->x, 
                       3,
                       mask, 
                       image->width ) ||
                   !gan_convolve2D_i ( 
                       &gan_image_get_pixptr_vfield3D_i(image,i,j)->y, 
                       3,
                       &gan_image_get_pixptr_vfield3D_i(dest,i,j)->y, 
                       3,
                       mask, 
                       image->width ) ||
                   !gan_convolve2D_i ( 
                       &gan_image_get_pixptr_vfield3D_i(image,i,j)->z, 
                       3,
                       &gan_image_get_pixptr_vfield3D_i(dest,i,j)->z, 
                       3,
                       mask, 
                       image->width ) )
              {               
                 break;
              }
           }
        }
        break;
        
      default:
        gan_err_flush_trace();
        gan_err_register ( "image_convolve2D_v3D_v3D", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
        return NULL;
   }

   /* check if we aborted the loop early because of an error */ 
   if ( i < dest->height-1 )
   {
      gan_err_register ( "image_convolve2D_v3D_v3D", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* success */ 
   return dest; 
} 

static Gan_Image * 
 image_convolve2D_rgb_rgb ( Gan_Image *image, 
                            Gan_Mask2D *mask, 
                            Gan_Image *dest ) 
{ 
   int i,j;

   gan_err_test_ptr ( image->format == GAN_RGB_COLOUR_IMAGE,
                      "image_convolve2D_rgb_rgb", GAN_ERROR_INCOMPATIBLE, "");
   gan_err_test_ptr ( image->width >= mask->cols, "image_convolve2D_rgb_rgb",
                      GAN_ERROR_IMAGE_TOO_SMALL, "" );
   gan_err_test_ptr ( image->width >= mask->rows, "image_convolve2D_rgb_rgb",
                      GAN_ERROR_IMAGE_TOO_SMALL, "" );

   if ( dest == NULL )
      dest = gan_image_alloc ( image->format, 
                               image->type,
                               image->height-mask->rows+1, 
                               image->width-mask->cols+1 );
   else
      dest = gan_image_set_format_type_dims ( dest,
                                              image->format,
                                              image->type,
                                              image->height-mask->rows+1,
                                              image->width-mask->cols+1 );

   if ( dest == NULL )
   {
      gan_err_register ( "image_convolve2D_rgb_rgb", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   dest->offset_x = image->offset_x+(int)mask->cols/2;
   dest->offset_y = image->offset_y+(int)mask->rows/2;

   switch ( image->type )
   {
      case GAN_DOUBLE:
        for ( i = 0; i<(int)dest->height; i++ ) 
        {
           for ( j = 0; j<(int)dest->width; j++ ) 
           {
              if ( !gan_convolve2D_d ( 
                       &gan_image_get_pixptr_rgb_d(image,i,j)->R, 
                       3,
                       &gan_image_get_pixptr_rgb_d(dest,i,j)->R, 
                       3,
                       mask, 
                       image->width ) ||
                   !gan_convolve2D_d ( 
                       &gan_image_get_pixptr_rgb_d(image,i,j)->G, 
                       3,
                       &gan_image_get_pixptr_rgb_d(dest,i,j)->G, 
                       3,
                       mask, 
                       image->width ) ||
                   !gan_convolve2D_d ( 
                       &gan_image_get_pixptr_rgb_d(image,i,j)->B, 
                       3,
                       &gan_image_get_pixptr_rgb_d(dest,i,j)->B, 
                       3,
                       mask, 
                       image->width ) )
              {               
                 break;
              }
           }
        }
        break;

      case GAN_FLOAT:
        for ( i = 0; i<(int)dest->height; i++ ) 
        {
           for ( j = 0; j<(int)dest->width; j++ ) 
           {
              if ( !gan_convolve2D_f ( 
                       &gan_image_get_pixptr_rgb_f(image,i,j)->R, 
                       3,
                       &gan_image_get_pixptr_rgb_f(dest,i,j)->R, 
                       3,
                       mask, 
                       image->width ) ||
                   !gan_convolve2D_f ( 
                       &gan_image_get_pixptr_rgb_f(image,i,j)->G, 
                       3,
                       &gan_image_get_pixptr_rgb_f(dest,i,j)->G, 
                       3,
                       mask, 
                       image->width ) ||
                   !gan_convolve2D_f ( 
                       &gan_image_get_pixptr_rgb_f(image,i,j)->B, 
                       3,
                       &gan_image_get_pixptr_rgb_f(dest,i,j)->B, 
                       3,
                       mask, 
                       image->width ) )
              {               
                 break;
              }
           }
        }
        break;
        
      case GAN_INT:
        for ( i = 0; i<(int)dest->height; i++ ) 
        {
           for ( j = 0; j<(int)dest->width; j++ ) 
           {
              if ( !gan_convolve2D_i ( 
                       &gan_image_get_pixptr_rgb_i(image,i,j)->R, 
                       3,
                       &gan_image_get_pixptr_rgb_i(dest,i,j)->R, 
                       3,
                       mask, 
                       image->width ) ||
                   !gan_convolve2D_i ( 
                       &gan_image_get_pixptr_rgb_i(image,i,j)->G, 
                       3,
                       &gan_image_get_pixptr_rgb_i(dest,i,j)->G, 
                       3,
                       mask, 
                       image->width ) ||
                   !gan_convolve2D_i ( 
                       &gan_image_get_pixptr_rgb_i(image,i,j)->B, 
                       3,
                       &gan_image_get_pixptr_rgb_i(dest,i,j)->B, 
                       3,
                       mask, 
                       image->width ) )
              {               
                 break;
              }
           }
        }
        break;
        
      case GAN_UINT:
        for ( i = 0; i<(int)dest->height; i++ ) 
        {
           for ( j = 0; j<(int)dest->width; j++ ) 
           {
              if ( !gan_convolve2D_ui ( 
                       &gan_image_get_pixptr_rgb_ui(image,i,j)->R, 
                       3,
                       &gan_image_get_pixptr_rgb_ui(dest,i,j)->R, 
                       3,
                       mask, 
                       image->width ) ||
                   !gan_convolve2D_ui ( 
                       &gan_image_get_pixptr_rgb_ui(image,i,j)->G, 
                       3,
                       &gan_image_get_pixptr_rgb_ui(dest,i,j)->G, 
                       3,
                       mask, 
                       image->width ) ||
                   !gan_convolve2D_ui ( 
                       &gan_image_get_pixptr_rgb_ui(image,i,j)->B, 
                       3,
                       &gan_image_get_pixptr_rgb_ui(dest,i,j)->B, 
                       3,
                       mask, 
                       image->width ) )
              {               
                 break;
              }
           }
        }
        break;
        
      case GAN_UCHAR:
        for ( i = 0; i<(int)dest->height; i++ ) 
        {
           for ( j = 0; j<(int)dest->width; j++ ) 
           {
              if ( !gan_convolve2D_uc ( 
                       &gan_image_get_pixptr_rgb_uc(image,i,j)->R, 
                       3,
                       &gan_image_get_pixptr_rgb_uc(dest,i,j)->R, 
                       3,
                       mask, 
                       image->width ) ||
                   !gan_convolve2D_uc ( 
                       &gan_image_get_pixptr_rgb_uc(image,i,j)->G, 
                       3,
                       &gan_image_get_pixptr_rgb_uc(dest,i,j)->G, 
                       3,
                       mask, 
                       image->width ) ||
                   !gan_convolve2D_uc ( 
                       &gan_image_get_pixptr_rgb_uc(image,i,j)->B, 
                       3,
                       &gan_image_get_pixptr_rgb_uc(dest,i,j)->B, 
                       3,
                       mask, 
                       image->width ) )
              {               
                 break;
              }
           }
        }
        break;

      case GAN_USHORT: 
        for ( i = 0; i<(int)dest->height; i++ ) 
        {
           for ( j = 0; j<(int)dest->width; j++ ) 
           {
              if ( !gan_convolve2D_us ( 
                       &gan_image_get_pixptr_rgb_us(image,i,j)->R, 
                       3,
                       &gan_image_get_pixptr_rgb_us(dest,i,j)->R, 
                       3,
                       mask, 
                       image->width ) ||
                   !gan_convolve2D_us ( 
                       &gan_image_get_pixptr_rgb_us(image,i,j)->G, 
                       3,
                       &gan_image_get_pixptr_rgb_us(dest,i,j)->G, 
                       3,
                       mask, 
                       image->width ) ||
                   !gan_convolve2D_us ( 
                       &gan_image_get_pixptr_rgb_us(image,i,j)->B, 
                       3,
                       &gan_image_get_pixptr_rgb_us(dest,i,j)->B, 
                       3,
                       mask, 
                       image->width ) )
              {               
                 break;
              }
           }
        }
        break;
        
      default:
        gan_err_flush_trace();
        gan_err_register ( "image_convolve2D_rgb_rgb", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
        return NULL;
   }

   /* check if we aborted the loop early because of an error */ 
   if ( i < dest->height-1 )
   {
      gan_err_register ( "image_convolve2D_rgb_rgb", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* success */ 
   return dest; 
} 

static Gan_Image * 
 image_convolve2D_rgba_rgba ( Gan_Image *image, 
                              Gan_Mask2D *mask, 
                              Gan_Image *dest ) 
{ 
   int i,j;

   gan_err_test_ptr ( image->format == GAN_RGB_COLOUR_ALPHA_IMAGE,
                      "image_convolve2D_rgba_rgba", 
                      GAN_ERROR_INCOMPATIBLE, "");
   gan_err_test_ptr ( image->width >= mask->cols, "image_convolve2D_rgba_rgba",
                      GAN_ERROR_IMAGE_TOO_SMALL, "" );
   gan_err_test_ptr ( image->width >= mask->rows, "image_convolve2D_rgba_rgba",
                      GAN_ERROR_IMAGE_TOO_SMALL, "" );

   if ( dest == NULL )
      dest = gan_image_alloc ( image->format, 
                               image->type,
                               image->height-mask->rows+1, 
                               image->width-mask->cols+1 );
   else
      dest = gan_image_set_format_type_dims ( dest,
                                              image->format,
                                              image->type,
                                              image->height-mask->rows+1,
                                              image->width-mask->cols+1 );

   if ( dest == NULL )
   {
      gan_err_register ( "image_convolve2D_rgba_rgba", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   dest->offset_x = image->offset_x+(int)mask->cols/2;
   dest->offset_y = image->offset_y+(int)mask->rows/2;

   switch ( image->type )
   {
      case GAN_DOUBLE:
        for ( i = 0; i<(int)dest->height; i++ ) 
        {
           for ( j = 0; j<(int)dest->width; j++ ) 
           {
              if ( !gan_convolve2D_d ( 
                       &gan_image_get_pixptr_rgba_d(image,i,j)->R, 
                       4,
                       &gan_image_get_pixptr_rgba_d(dest,i,j)->R, 
                       4,
                       mask, 
                       image->width ) ||
                   !gan_convolve2D_d ( 
                       &gan_image_get_pixptr_rgba_d(image,i,j)->G, 
                       4,
                       &gan_image_get_pixptr_rgba_d(dest,i,j)->G, 
                       4,
                       mask, 
                       image->width ) ||
                   !gan_convolve2D_d ( 
                       &gan_image_get_pixptr_rgba_d(image,i,j)->B, 
                       4,
                       &gan_image_get_pixptr_rgba_d(dest,i,j)->B, 
                       4,
                       mask, 
                       image->width ) )
              {               
                 break;
              }
           }
        }
        break;

      case GAN_FLOAT:
        for ( i = 0; i<(int)dest->height; i++ ) 
        {
           for ( j = 0; j<(int)dest->width; j++ ) 
           {
              if ( !gan_convolve2D_f ( 
                       &gan_image_get_pixptr_rgba_f(image,i,j)->R, 
                       4,
                       &gan_image_get_pixptr_rgba_f(dest,i,j)->R, 
                       4,
                       mask, 
                       image->width ) ||
                   !gan_convolve2D_f ( 
                       &gan_image_get_pixptr_rgba_f(image,i,j)->G, 
                       4,
                       &gan_image_get_pixptr_rgba_f(dest,i,j)->G, 
                       4,
                       mask, 
                       image->width ) ||
                   !gan_convolve2D_f ( 
                       &gan_image_get_pixptr_rgba_f(image,i,j)->B, 
                       4,
                       &gan_image_get_pixptr_rgba_f(dest,i,j)->B, 
                       4,
                       mask, 
                       image->width ) )
              {               
                 break;
              }
           }
        }
        break;
        
      case GAN_INT:
        for ( i = 0; i<(int)dest->height; i++ ) 
        {
           for ( j = 0; j<(int)dest->width; j++ ) 
           {
              if ( !gan_convolve2D_i ( 
                       &gan_image_get_pixptr_rgba_i(image,i,j)->R, 
                       4,
                       &gan_image_get_pixptr_rgba_i(dest,i,j)->R, 
                       4,
                       mask, 
                       image->width ) ||
                   !gan_convolve2D_i ( 
                       &gan_image_get_pixptr_rgba_i(image,i,j)->G, 
                       4,
                       &gan_image_get_pixptr_rgba_i(dest,i,j)->G, 
                       4,
                       mask, 
                       image->width ) ||
                   !gan_convolve2D_i ( 
                       &gan_image_get_pixptr_rgba_i(image,i,j)->B, 
                       4,
                       &gan_image_get_pixptr_rgba_i(dest,i,j)->B, 
                       4,
                       mask, 
                       image->width ) )
              {               
                 break;
              }
           }
        }
        break;
        
      case GAN_UINT:
        for ( i = 0; i<(int)dest->height; i++ ) 
        {
           for ( j = 0; j<(int)dest->width; j++ ) 
           {
              if ( !gan_convolve2D_ui ( 
                       &gan_image_get_pixptr_rgba_ui(image,i,j)->R, 
                       4,
                       &gan_image_get_pixptr_rgba_ui(dest,i,j)->R, 
                       4,
                       mask, 
                       image->width ) ||
                   !gan_convolve2D_ui ( 
                       &gan_image_get_pixptr_rgba_ui(image,i,j)->G, 
                       4,
                       &gan_image_get_pixptr_rgba_ui(dest,i,j)->G, 
                       4,
                       mask, 
                       image->width ) ||
                   !gan_convolve2D_ui ( 
                       &gan_image_get_pixptr_rgba_ui(image,i,j)->B, 
                       4,
                       &gan_image_get_pixptr_rgba_ui(dest,i,j)->B, 
                       4,
                       mask, 
                       image->width ) )
              {               
                 break;
              }
           }
        }
        break;
        
      case GAN_UCHAR:
        for ( i = 0; i<(int)dest->height; i++ ) 
        {
           for ( j = 0; j<(int)dest->width; j++ ) 
           {
              if ( !gan_convolve2D_uc ( 
                       &gan_image_get_pixptr_rgba_uc(image,i,j)->R, 
                       4,
                       &gan_image_get_pixptr_rgba_uc(dest,i,j)->R, 
                       4,
                       mask, 
                       image->width ) ||
                   !gan_convolve2D_uc ( 
                       &gan_image_get_pixptr_rgba_uc(image,i,j)->G, 
                       4,
                       &gan_image_get_pixptr_rgba_uc(dest,i,j)->G, 
                       4,
                       mask, 
                       image->width ) ||
                   !gan_convolve2D_uc ( 
                       &gan_image_get_pixptr_rgba_uc(image,i,j)->B, 
                       4,
                       &gan_image_get_pixptr_rgba_uc(dest,i,j)->B, 
                       4,
                       mask, 
                       image->width ) )
              {               
                 break;
              }
           }
        }
        break;

      case GAN_USHORT: 
        for ( i = 0; i<(int)dest->height; i++ ) 
        {
           for ( j = 0; j<(int)dest->width; j++ ) 
           {
              if ( !gan_convolve2D_us ( 
                       &gan_image_get_pixptr_rgba_us(image,i,j)->R, 
                       4,
                       &gan_image_get_pixptr_rgba_us(dest,i,j)->R, 
                       4,
                       mask, 
                       image->width ) ||
                   !gan_convolve2D_us ( 
                       &gan_image_get_pixptr_rgba_us(image,i,j)->G, 
                       4,
                       &gan_image_get_pixptr_rgba_us(dest,i,j)->G, 
                       4,
                       mask, 
                       image->width ) ||
                   !gan_convolve2D_us ( 
                       &gan_image_get_pixptr_rgba_us(image,i,j)->B, 
                       4,
                       &gan_image_get_pixptr_rgba_us(dest,i,j)->B, 
                       4,
                       mask, 
                       image->width ) )
              {               
                 break;
              }
           }
        }
        break;
        
      default:
        gan_err_flush_trace();
        gan_err_register ( "image_convolve2D_rgba_rgba", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
        return NULL;
   }

   /* check if we aborted the loop early because of an error */ 
   if ( i < dest->height-1 )
   {
      gan_err_register ( "image_convolve2D_rgba_rgba", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* success */ 
   return dest; 
}

static Gan_Image * 
 image_convolve2D_bgr_bgr ( Gan_Image *image, 
                            Gan_Mask2D *mask, 
                            Gan_Image *dest ) 
{ 
   int i,j;

   gan_err_test_ptr ( image->format == GAN_BGR_COLOUR_IMAGE,
                      "image_convolve2D_bgr_bgr", GAN_ERROR_INCOMPATIBLE, "");
   gan_err_test_ptr ( image->width >= mask->cols, "image_convolve2D_bgr_bgr",
                      GAN_ERROR_IMAGE_TOO_SMALL, "" );
   gan_err_test_ptr ( image->width >= mask->rows, "image_convolve2D_bgr_bgr",
                      GAN_ERROR_IMAGE_TOO_SMALL, "" );

   if ( dest == NULL )
      dest = gan_image_alloc ( image->format, 
                               image->type,
                               image->height-mask->rows+1, 
                               image->width-mask->cols+1 );
   else
      dest = gan_image_set_format_type_dims ( dest,
                                              image->format,
                                              image->type,
                                              image->height-mask->rows+1,
                                              image->width-mask->cols+1 );

   if ( dest == NULL )
   {
      gan_err_register ( "image_convolve2D_bgr_bgr", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   dest->offset_x = image->offset_x+(int)mask->cols/2;
   dest->offset_y = image->offset_y+(int)mask->rows/2;

   switch ( image->type )
   {
      case GAN_DOUBLE:
        for ( i = 0; i<(int)dest->height; i++ ) 
        {
           for ( j = 0; j<(int)dest->width; j++ ) 
           {
              if ( !gan_convolve2D_d ( 
                       &gan_image_get_pixptr_bgr_d(image,i,j)->R, 
                       3,
                       &gan_image_get_pixptr_bgr_d(dest,i,j)->R, 
                       3,
                       mask, 
                       image->width ) ||
                   !gan_convolve2D_d ( 
                       &gan_image_get_pixptr_bgr_d(image,i,j)->G, 
                       3,
                       &gan_image_get_pixptr_bgr_d(dest,i,j)->G, 
                       3,
                       mask, 
                       image->width ) ||
                   !gan_convolve2D_d ( 
                       &gan_image_get_pixptr_bgr_d(image,i,j)->B, 
                       3,
                       &gan_image_get_pixptr_bgr_d(dest,i,j)->B, 
                       3,
                       mask, 
                       image->width ) )
              {               
                 break;
              }
           }
        }
        break;

      case GAN_FLOAT:
        for ( i = 0; i<(int)dest->height; i++ ) 
        {
           for ( j = 0; j<(int)dest->width; j++ ) 
           {
              if ( !gan_convolve2D_f ( 
                       &gan_image_get_pixptr_bgr_f(image,i,j)->R, 
                       3,
                       &gan_image_get_pixptr_bgr_f(dest,i,j)->R, 
                       3,
                       mask, 
                       image->width ) ||
                   !gan_convolve2D_f ( 
                       &gan_image_get_pixptr_bgr_f(image,i,j)->G, 
                       3,
                       &gan_image_get_pixptr_bgr_f(dest,i,j)->G, 
                       3,
                       mask, 
                       image->width ) ||
                   !gan_convolve2D_f ( 
                       &gan_image_get_pixptr_bgr_f(image,i,j)->B, 
                       3,
                       &gan_image_get_pixptr_bgr_f(dest,i,j)->B, 
                       3,
                       mask, 
                       image->width ) )
              {               
                 break;
              }
           }
        }
        break;
        
      case GAN_INT:
        for ( i = 0; i<(int)dest->height; i++ ) 
        {
           for ( j = 0; j<(int)dest->width; j++ ) 
           {
              if ( !gan_convolve2D_i ( 
                       &gan_image_get_pixptr_bgr_i(image,i,j)->R, 
                       3,
                       &gan_image_get_pixptr_bgr_i(dest,i,j)->R, 
                       3,
                       mask, 
                       image->width ) ||
                   !gan_convolve2D_i ( 
                       &gan_image_get_pixptr_bgr_i(image,i,j)->G, 
                       3,
                       &gan_image_get_pixptr_bgr_i(dest,i,j)->G, 
                       3,
                       mask, 
                       image->width ) ||
                   !gan_convolve2D_i ( 
                       &gan_image_get_pixptr_bgr_i(image,i,j)->B, 
                       3,
                       &gan_image_get_pixptr_bgr_i(dest,i,j)->B, 
                       3,
                       mask, 
                       image->width ) )
              {               
                 break;
              }
           }
        }
        break;
        
      case GAN_UINT:
        for ( i = 0; i<(int)dest->height; i++ ) 
        {
           for ( j = 0; j<(int)dest->width; j++ ) 
           {
              if ( !gan_convolve2D_ui ( 
                       &gan_image_get_pixptr_bgr_ui(image,i,j)->R, 
                       3,
                       &gan_image_get_pixptr_bgr_ui(dest,i,j)->R, 
                       3,
                       mask, 
                       image->width ) ||
                   !gan_convolve2D_ui ( 
                       &gan_image_get_pixptr_bgr_ui(image,i,j)->G, 
                       3,
                       &gan_image_get_pixptr_bgr_ui(dest,i,j)->G, 
                       3,
                       mask, 
                       image->width ) ||
                   !gan_convolve2D_ui ( 
                       &gan_image_get_pixptr_bgr_ui(image,i,j)->B, 
                       3,
                       &gan_image_get_pixptr_bgr_ui(dest,i,j)->B, 
                       3,
                       mask, 
                       image->width ) )
              {               
                 break;
              }
           }
        }
        break;
        
      case GAN_UCHAR:
        for ( i = 0; i<(int)dest->height; i++ ) 
        {
           for ( j = 0; j<(int)dest->width; j++ ) 
           {
              if ( !gan_convolve2D_uc ( 
                       &gan_image_get_pixptr_bgr_uc(image,i,j)->R, 
                       3,
                       &gan_image_get_pixptr_bgr_uc(dest,i,j)->R, 
                       3,
                       mask, 
                       image->width ) ||
                   !gan_convolve2D_uc ( 
                       &gan_image_get_pixptr_bgr_uc(image,i,j)->G, 
                       3,
                       &gan_image_get_pixptr_bgr_uc(dest,i,j)->G, 
                       3,
                       mask, 
                       image->width ) ||
                   !gan_convolve2D_uc ( 
                       &gan_image_get_pixptr_bgr_uc(image,i,j)->B, 
                       3,
                       &gan_image_get_pixptr_bgr_uc(dest,i,j)->B, 
                       3,
                       mask, 
                       image->width ) )
              {               
                 break;
              }
           }
        }
        break;

      case GAN_USHORT: 
        for ( i = 0; i<(int)dest->height; i++ ) 
        {
           for ( j = 0; j<(int)dest->width; j++ ) 
           {
              if ( !gan_convolve2D_us ( 
                       &gan_image_get_pixptr_bgr_us(image,i,j)->R, 
                       3,
                       &gan_image_get_pixptr_bgr_us(dest,i,j)->R, 
                       3,
                       mask, 
                       image->width ) ||
                   !gan_convolve2D_us ( 
                       &gan_image_get_pixptr_bgr_us(image,i,j)->G, 
                       3,
                       &gan_image_get_pixptr_bgr_us(dest,i,j)->G, 
                       3,
                       mask, 
                       image->width ) ||
                   !gan_convolve2D_us ( 
                       &gan_image_get_pixptr_bgr_us(image,i,j)->B, 
                       3,
                       &gan_image_get_pixptr_bgr_us(dest,i,j)->B, 
                       3,
                       mask, 
                       image->width ) )
              {               
                 break;
              }
           }
        }
        break;
        
      default:
        gan_err_flush_trace();
        gan_err_register ( "image_convolve2D_bgr_bgr", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
        return NULL;
   }

   /* check if we aborted the loop early because of an error */ 
   if ( i < dest->height-1 )
   {
      gan_err_register ( "image_convolve2D_bgr_bgr", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* success */ 
   return dest; 
} 

static Gan_Image * 
 image_convolve2D_bgra_bgra ( Gan_Image *image, 
                              Gan_Mask2D *mask, 
                              Gan_Image *dest ) 
{ 
   int i,j;

   gan_err_test_ptr ( image->format == GAN_BGR_COLOUR_ALPHA_IMAGE,
                      "image_convolve2D_bgra_bgra", 
                      GAN_ERROR_INCOMPATIBLE, "");
   gan_err_test_ptr ( image->width >= mask->cols, "image_convolve2D_bgra_bgra",
                      GAN_ERROR_IMAGE_TOO_SMALL, "" );
   gan_err_test_ptr ( image->width >= mask->rows, "image_convolve2D_bgra_bgra",
                      GAN_ERROR_IMAGE_TOO_SMALL, "" );

   if ( dest == NULL )
      dest = gan_image_alloc ( image->format, 
                               image->type,
                               image->height-mask->rows+1, 
                               image->width-mask->cols+1 );
   else
      dest = gan_image_set_format_type_dims ( dest,
                                              image->format,
                                              image->type,
                                              image->height-mask->rows+1,
                                              image->width-mask->cols+1 );

   if ( dest == NULL )
   {
      gan_err_register ( "image_convolve2D_bgra_bgra", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   dest->offset_x = image->offset_x+(int)mask->cols/2;
   dest->offset_y = image->offset_y+(int)mask->rows/2;

   switch ( image->type )
   {
      case GAN_DOUBLE:
        for ( i = 0; i<(int)dest->height; i++ ) 
        {
           for ( j = 0; j<(int)dest->width; j++ ) 
           {
              if ( !gan_convolve2D_d ( 
                       &gan_image_get_pixptr_bgra_d(image,i,j)->R, 
                       4,
                       &gan_image_get_pixptr_bgra_d(dest,i,j)->R, 
                       4,
                       mask, 
                       image->width ) ||
                   !gan_convolve2D_d ( 
                       &gan_image_get_pixptr_bgra_d(image,i,j)->G, 
                       4,
                       &gan_image_get_pixptr_bgra_d(dest,i,j)->G, 
                       4,
                       mask, 
                       image->width ) ||
                   !gan_convolve2D_d ( 
                       &gan_image_get_pixptr_bgra_d(image,i,j)->B, 
                       4,
                       &gan_image_get_pixptr_bgra_d(dest,i,j)->B, 
                       4,
                       mask, 
                       image->width ) )
              {               
                 break;
              }
           }
        }
        break;

      case GAN_FLOAT:
        for ( i = 0; i<(int)dest->height; i++ ) 
        {
           for ( j = 0; j<(int)dest->width; j++ ) 
           {
              if ( !gan_convolve2D_f ( 
                       &gan_image_get_pixptr_bgra_f(image,i,j)->R, 
                       4,
                       &gan_image_get_pixptr_bgra_f(dest,i,j)->R, 
                       4,
                       mask, 
                       image->width ) ||
                   !gan_convolve2D_f ( 
                       &gan_image_get_pixptr_bgra_f(image,i,j)->G, 
                       4,
                       &gan_image_get_pixptr_bgra_f(dest,i,j)->G, 
                       4,
                       mask, 
                       image->width ) ||
                   !gan_convolve2D_f ( 
                       &gan_image_get_pixptr_bgra_f(image,i,j)->B, 
                       4,
                       &gan_image_get_pixptr_bgra_f(dest,i,j)->B, 
                       4,
                       mask, 
                       image->width ) )
              {               
                 break;
              }
           }
        }
        break;
        
      case GAN_INT:
        for ( i = 0; i<(int)dest->height; i++ ) 
        {
           for ( j = 0; j<(int)dest->width; j++ ) 
           {
              if ( !gan_convolve2D_i ( 
                       &gan_image_get_pixptr_bgra_i(image,i,j)->R, 
                       4,
                       &gan_image_get_pixptr_bgra_i(dest,i,j)->R, 
                       4,
                       mask, 
                       image->width ) ||
                   !gan_convolve2D_i ( 
                       &gan_image_get_pixptr_bgra_i(image,i,j)->G, 
                       4,
                       &gan_image_get_pixptr_bgra_i(dest,i,j)->G, 
                       4,
                       mask, 
                       image->width ) ||
                   !gan_convolve2D_i ( 
                       &gan_image_get_pixptr_bgra_i(image,i,j)->B, 
                       4,
                       &gan_image_get_pixptr_bgra_i(dest,i,j)->B, 
                       4,
                       mask, 
                       image->width ) )
              {               
                 break;
              }
           }
        }
        break;
        
      case GAN_UINT:
        for ( i = 0; i<(int)dest->height; i++ ) 
        {
           for ( j = 0; j<(int)dest->width; j++ ) 
           {
              if ( !gan_convolve2D_ui ( 
                       &gan_image_get_pixptr_bgra_ui(image,i,j)->R, 
                       4,
                       &gan_image_get_pixptr_bgra_ui(dest,i,j)->R, 
                       4,
                       mask, 
                       image->width ) ||
                   !gan_convolve2D_ui ( 
                       &gan_image_get_pixptr_bgra_ui(image,i,j)->G, 
                       4,
                       &gan_image_get_pixptr_bgra_ui(dest,i,j)->G, 
                       4,
                       mask, 
                       image->width ) ||
                   !gan_convolve2D_ui ( 
                       &gan_image_get_pixptr_bgra_ui(image,i,j)->B, 
                       4,
                       &gan_image_get_pixptr_bgra_ui(dest,i,j)->B, 
                       4,
                       mask, 
                       image->width ) )
              {               
                 break;
              }
           }
        }
        break;
        
      case GAN_UCHAR:
        for ( i = 0; i<(int)dest->height; i++ ) 
        {
           for ( j = 0; j<(int)dest->width; j++ ) 
           {
              if ( !gan_convolve2D_uc ( 
                       &gan_image_get_pixptr_bgra_uc(image,i,j)->R, 
                       4,
                       &gan_image_get_pixptr_bgra_uc(dest,i,j)->R, 
                       4,
                       mask, 
                       image->width ) ||
                   !gan_convolve2D_uc ( 
                       &gan_image_get_pixptr_bgra_uc(image,i,j)->G, 
                       4,
                       &gan_image_get_pixptr_bgra_uc(dest,i,j)->G, 
                       4,
                       mask, 
                       image->width ) ||
                   !gan_convolve2D_uc ( 
                       &gan_image_get_pixptr_bgra_uc(image,i,j)->B, 
                       4,
                       &gan_image_get_pixptr_bgra_uc(dest,i,j)->B, 
                       4,
                       mask, 
                       image->width ) )
              {               
                 break;
              }
           }
        }
        break;

      case GAN_USHORT: 
        for ( i = 0; i<(int)dest->height; i++ ) 
        {
           for ( j = 0; j<(int)dest->width; j++ ) 
           {
              if ( !gan_convolve2D_us ( 
                       &gan_image_get_pixptr_bgra_us(image,i,j)->R, 
                       4,
                       &gan_image_get_pixptr_bgra_us(dest,i,j)->R, 
                       4,
                       mask, 
                       image->width ) ||
                   !gan_convolve2D_us ( 
                       &gan_image_get_pixptr_bgra_us(image,i,j)->G, 
                       4,
                       &gan_image_get_pixptr_bgra_us(dest,i,j)->G, 
                       4,
                       mask, 
                       image->width ) ||
                   !gan_convolve2D_us ( 
                       &gan_image_get_pixptr_bgra_us(image,i,j)->B, 
                       4,
                       &gan_image_get_pixptr_bgra_us(dest,i,j)->B, 
                       4,
                       mask, 
                       image->width ) )
              {               
                 break;
              }
           }
        }
        break;
        
      default:
        gan_err_flush_trace();
        gan_err_register ( "image_convolve2D_bgra_bgra", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
        return NULL;
   }

   /* check if we aborted the loop early because of an error */ 
   if ( i < dest->height-1 )
   {
      gan_err_register ( "image_convolve2D_bgra_bgra", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* success */ 
   return dest; 
}


/**
 * \brief Convolves an image in both directions.
 * \param image The input image
 * \param channel Colour channel to be convolved where applicable
 * \param mask The image convolution mask
 * \param dest The destination image for the convolution operation
 *
 * Applies a two-dimensional convolution operation to the
 * given image. When the image contains
 * colour or vector field data, a particular colour channel/vector
 * field element can be specified by the channel argument, which
 * should otherwise be passed as #GAN_ALL_CHANNELS.
 * There is no checking for overflow of integer values.
 *
 * \return Non-\c NULL on successfully returning the destination image \a dest,
 * \c NULL on failure.
 * \sa gan_image_convolve2Dy_q, gan_gauss_mask_new().
 */
Gan_Image *
 gan_image_convolve2D_q ( Gan_Image *image, Gan_ImageChannelType channel,
                          Gan_Mask2D *mask, Gan_Image *dest )
{
   switch ( image->format )
   {
      case GAN_GREY_LEVEL_IMAGE:
        switch ( channel )
        {
           case GAN_INTENSITY_CHANNEL:
           case GAN_ALL_CHANNELS:
             dest = image_convolve2D_gl_gl ( image, mask, dest );
             break;

           case GAN_X_CHANNEL:
           case GAN_Y_CHANNEL:
           case GAN_Z_CHANNEL:
             dest = image_convolve2D_gl_v3D ( image, channel, mask, dest );
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_convolve2D_q", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;

      case GAN_VECTOR_FIELD_3D:
        switch ( channel )
        {
           case GAN_ALL_CHANNELS:
             dest = image_convolve2D_v3D_v3D ( image, mask, dest );
             break;

           case GAN_X_CHANNEL:
           case GAN_Y_CHANNEL:
           case GAN_Z_CHANNEL:
             dest = image_convolve2D_v3D_gl ( image, channel, mask, dest );
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_convolve2D_q", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;

      case GAN_RGB_COLOUR_IMAGE:
        switch ( channel )
        {
           case GAN_ALL_CHANNELS:
             dest = image_convolve2D_rgb_rgb ( image, mask, dest );
             break;

           case GAN_RED_CHANNEL:
           case GAN_GREEN_CHANNEL:
           case GAN_BLUE_CHANNEL:
             dest = image_convolve2D_rgb_gl ( image, channel, mask, dest );
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_convolve2D_q", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;
 
      case GAN_RGB_COLOUR_ALPHA_IMAGE:
        switch ( channel )
        {
           case GAN_ALL_CHANNELS:
             dest = image_convolve2D_rgba_rgba ( image, mask, dest );
             break;

           case GAN_RED_CHANNEL:
           case GAN_GREEN_CHANNEL:
           case GAN_BLUE_CHANNEL:
             dest = image_convolve2D_rgba_gl ( image, channel, mask, dest );
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_convolve2D_q", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;      

      case GAN_BGR_COLOUR_IMAGE:
        switch ( channel )
        {
           case GAN_ALL_CHANNELS:
             dest = image_convolve2D_bgr_bgr ( image, mask, dest );
             break;

           case GAN_RED_CHANNEL:
           case GAN_GREEN_CHANNEL:
           case GAN_BLUE_CHANNEL:
             dest = image_convolve2D_bgr_gl ( image, channel, mask, dest );
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_convolve2D_q", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;
 
      case GAN_BGR_COLOUR_ALPHA_IMAGE:
        switch ( channel )
        {
           case GAN_ALL_CHANNELS:
             dest = image_convolve2D_bgra_bgra ( image, mask, dest );
             break;

           case GAN_RED_CHANNEL:
           case GAN_GREEN_CHANNEL:
           case GAN_BLUE_CHANNEL:
             dest = image_convolve2D_bgra_gl ( image, channel, mask, dest );
             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "gan_image_convolve2D_q", GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;      

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_image_convolve2D_q", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
        return NULL;
   }

   if ( dest == NULL )
   {
      gan_err_register ( "gan_image_convolve2D_q", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* success */ 
   return dest;
}

/**
 * \}
 */

/**
 * \}
 */

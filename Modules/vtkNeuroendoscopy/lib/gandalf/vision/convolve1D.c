/**
 * File:          $RCSfile: convolve1D.c,v $
 * Module:        Image 1D convolution routines
 * Part of:       Gandalf Library
 *
 * Revision:      $Revision: 1.1.2.1 $
 * Last edited:   $Date: 2007/10/14 02:33:16 $
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
#include <gandalf/vision/convolve1D.h>
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

#undef ALTIVEC_CONVOLUTION

#if defined(__APPLE__) && defined(ALTIVEC_CONVOLUTION)
typedef union
{
   float            flt[4];
   vector float    vFlt;
} floatToVector;
#endif

/**
 * \addtogroup Vision
 * \{
 */

/**
 * \defgroup Convolution Convolution Operations
 * \{
 */

/* shift up/down into position either 0 or s-1 */
#define SHIFT_DOWN_REPEAT(v,s) (((v) < (s)) ? (v) : ((s)-1))
#define SHIFT_UP_REPEAT(v,s) (((v) < 0) ? 0 : (v))
#define SHIFT_IN_REPEAT(v,s) (((v) < 0) ? 0 : (((v) < (s)) ? (v) : ((s)-1)))

/* shift up/down into range 0,..,s-1 */
#define SHIFT_DOWN_CIRCULAR(v,s) (((v) < (s)) ? (v) : ((v)-(s)))
#define SHIFT_UP_CIRCULAR(v,s)   (((v) < 0) ? ((v)+(s)) : (v))
#define SHIFT_IN_CIRCULAR(v,s) (((v) < 0) ? ((v)+(s)) : (((v) < (s)) ? (v) : ((v)-(s))))

/**
 * \brief 1D convolution function for float arrays.
 * \param source The input array to be convolved
 * \param sstride The stride of the source array in units of floats
 * \param behaviour Behaviour of data beyond edge
 * \param zero_outside Whether to assume zeros outside the source data
 * \param dest The destination array for the convolution
 * \param dstride The stride of the dest array in units of floats
 * \param mask The convolution mask
 * \param dsize The number of output elements to compute
 *
 * Applies a one-dimensional convolution operation to the given \a source
 * array. Both arrays must be of type \c float.
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 * \sa gan_convolve1D_i().
 */
Gan_Bool
 gan_convolve1D_f ( float *source, int sstride,
                    Gan_EdgeBehaviour behaviour, Gan_Bool zero_outside,
                    float *dest,   int dstride,
                    Gan_Mask1D *mask, int dsize )
{
   int i, j;
   float total;

   gan_err_test_bool ( mask->type == GAN_FLOAT, "gan_convolve1D_f",
                       GAN_ERROR_INCOMPATIBLE, "" );
   switch ( mask->format )
   {
      case GAN_MASK1D_SYMMETRIC:
      {
         int half_size = (int)mask->size/2;

         switch ( behaviour )
         {
            case GAN_EDGE_BEHAVIOUR_CLIP:
            {
               source += half_size*sstride;
               for ( i = dsize-1; i >= 0;
                     i--, dest += dstride, source += sstride )
               {
                  total = mask->data.f[0]*source[0];
                  for ( j = half_size; j > 0; j-- )
                     total += mask->data.f[j]
                              *(source[j*sstride] + source[-j*sstride]);

                  *dest = total;
               }
            }
            break;

            case GAN_EDGE_BEHAVIOUR_REPEAT:
            {
               for ( i = dsize-1; i >= dsize-half_size; i-- )
               {
                  total = mask->data.f[0]*source[i*sstride];
                  for ( j = half_size; j > 0; j-- )
                     total += mask->data.f[j]
                     *(source[SHIFT_DOWN_REPEAT(i+j,dsize)*sstride] +
                       source[(i-j)*sstride]);

                  dest[i*dstride] = total;
               }

               for ( ; i >= half_size; i-- )
               {
                  total = mask->data.f[0]*source[i*sstride];
                  for ( j = half_size; j > 0; j-- )
                     total += mask->data.f[j]*(source[(i+j)*sstride] +
                                               source[(i-j)*sstride]);

                  dest[i*dstride] = total;
               }

               for ( ; i >= 0; i-- )
               {
                  total = mask->data.f[0]*source[i*sstride];
                  for ( j = half_size; j > 0; j-- )
                     total += mask->data.f[j]
                     *(source[(i+j)*sstride] +
                       source[SHIFT_UP_REPEAT((i-j),dsize)*sstride]);

                  dest[i*dstride] = total;
               }
            }
            break;

            case GAN_EDGE_BEHAVIOUR_CIRCULAR:
            {
               for ( i = dsize-1; i >= 0; i-- )
               {
                  total = mask->data.f[0]*source[i*sstride];
                  for ( j = half_size; j > 0; j-- )
                     total += mask->data.f[j]
                              *(source[SHIFT_DOWN_CIRCULAR(i+j,dsize)*sstride]+
                                source[SHIFT_UP_CIRCULAR(i-j,dsize)*sstride]);

                  dest[i*dstride] = total;
               }
            }
            break;

            case GAN_EDGE_BEHAVIOUR_EXPAND:
            {
               if(zero_outside)
                  for ( i = dsize-1; i >= dsize-2*half_size; i-- )
                  {
                     total = 0.0F;
                     for ( j = 0; j <= half_size; j++ )
                     {
                        if(i+j >= dsize-half_size)
                           break;

                        total += mask->data.f[j]*source[(i-half_size+j)*sstride];
                     }

                     for ( j = half_size; j > 0; j-- )
                     {
                        if(i-j >= dsize-half_size)
                           break;

                        total += mask->data.f[j]*source[(i-half_size-j)*sstride];
                     }

                     dest[i*dstride] = total;
                  }
               else
                  for ( i = dsize-1; i >= dsize-2*half_size; i-- )
                  {
                     total = 0.0F;
                     for ( j = 0; j <= half_size; j++ )
                        if(i+j >= dsize-half_size)
                           total += mask->data.f[j]*source[(dsize-2*half_size-1)*sstride];
                        else
                           total += mask->data.f[j]*source[(i-half_size+j)*sstride];

                     for ( j = half_size; j > 0; j-- )
                        if(i-j >= dsize-half_size)
                           total += mask->data.f[j]*source[(dsize-2*half_size-1)*sstride];
                        else
                           total += mask->data.f[j]*source[(i-half_size-j)*sstride];

                     dest[i*dstride] = total;
                  }

               for ( ; i >= 2*half_size; i-- )
               {
                  total = mask->data.f[0]*source[(i-half_size)*sstride];
                  for ( j = half_size; j > 0; j-- )
                     total += mask->data.f[j]*(source[(i-half_size+j)*sstride] +
                                               source[(i-half_size-j)*sstride]);

                  dest[i*dstride] = total;
               }

               if(zero_outside)
                  for ( ; i >= 0; i-- )
                  {
                     total = 0.0F;
                     for ( j = half_size; j >= 0; j-- )
                     {
                        if(i+j < half_size)
                           break;

                        total += mask->data.f[j]*source[(i-half_size+j)*sstride];
                     }

                     for ( j = 1; j <= half_size; j++ )
                     {
                        if(i-j < half_size)
                           break;

                        total += mask->data.f[j]*source[(i-half_size-j)*sstride];
                     }

                     dest[i*dstride] = total;
                  }
               else
                  for ( ; i >= 0; i-- )
                  {
                     total = 0.0F;
                     for ( j = half_size; j >= 0; j-- )
                        if(i+j < half_size)
                           total += mask->data.f[j]*source[0];
                        else
                           total += mask->data.f[j]*source[(i-half_size+j)*sstride];

                     for ( j = 1; j <= half_size; j++ )
                        if(i-j < half_size)
                           total += mask->data.f[j]*source[0];
                        else
                           total += mask->data.f[j]*source[(i-half_size-j)*sstride];

                     dest[i*dstride] = total;
                  }
            }
            break;

            default:
              gan_err_flush_trace();
              gan_err_register ( "gan_convolve1D_f", GAN_ERROR_ILLEGAL_TYPE, "" );
              break;
         }
      }
      break;

      case GAN_MASK1D_ANTISYMMETRIC:
      {
         int half_size = (int)mask->size/2;

         switch ( behaviour )
         {
            case GAN_EDGE_BEHAVIOUR_CLIP:
            {
               source += half_size*sstride;
               for ( i = dsize-1; i >= 0;
                     i--, dest += dstride, source += sstride )
               {
                  total = 0.0;
                  for ( j = half_size; j > 0; j-- )
                     total += mask->data.f[j-1]
                     *(source[j*sstride] - source[-j*sstride]);

                  *dest = total;
               }
            }
            break;

            case GAN_EDGE_BEHAVIOUR_REPEAT:
            {
               for ( i = dsize-1; i >= 0; i-- )
               {
                  total = 0.0;
                  for ( j = half_size; j > 0; j-- )
                     total += mask->data.f[j-1]
                        *(source[SHIFT_DOWN_REPEAT(i+j,dsize)*sstride] +
                          source[SHIFT_UP_REPEAT(i-j,dsize)*sstride]);

                  dest[i*dstride] = total;
               }
            }
            break;
            
            case GAN_EDGE_BEHAVIOUR_CIRCULAR:
            {
               for ( i = dsize-1; i >= 0; i-- )
               {
                  total = 0.0;
                  for ( j = half_size; j > 0; j-- )
                     total += mask->data.f[j-1]
                        *(source[SHIFT_DOWN_CIRCULAR(i+j,dsize)*sstride] +
                          source[SHIFT_UP_CIRCULAR(i-j,dsize)*sstride]);

                  dest[i*dstride] = total;
               }
            }
            break;

            default:
              gan_err_flush_trace();
              gan_err_register ( "gan_convolve1D_f", GAN_ERROR_ILLEGAL_TYPE, "" );
              break;
         }
      }
      break;

      case GAN_MASK1D_GENERIC:
      {
         switch ( behaviour )
         {
            case GAN_EDGE_BEHAVIOUR_CLIP:
            {
               source += (mask->size-1)*sstride;
               for ( i = dsize-1; i >= 0;
                     i--, dest += dstride, source += sstride )
               {
                  total = 0.0;
                  for ( j = (int)mask->size-1; j >= 0; j-- )
                     total += mask->data.f[j]*source[-j*sstride];

                  *dest = total;
               }
            }
            break;

            case GAN_EDGE_BEHAVIOUR_REPEAT:
            {
               int half_size = (int)mask->size/2;
               float *mdata = mask->data.f + half_size;

               for ( i = dsize-1; i >= 0; i-- )
               {
                  total = 0.0;
                  for ( j = -half_size; j <= half_size; j++ )
                     total += mdata[j]*source[SHIFT_IN_REPEAT(i-j,dsize)*sstride];

                  dest[i*dstride] = total;
               }
            }
            break;

            case GAN_EDGE_BEHAVIOUR_CIRCULAR:
            {
               int half_size = (int)mask->size/2;
               float *mdata = mask->data.f + half_size;

               for ( i = dsize-1; i >= 0; i-- )
               {
                  total = 0.0;
                  for ( j = -half_size; j <= half_size; j++ )
                     total += mdata[j]*source[SHIFT_IN_CIRCULAR(i-j,dsize)*sstride];

                  dest[i*dstride] = total;
               }
            }
            break;

            default:
              gan_err_flush_trace();
              gan_err_register ( "gan_convolve1D_f", GAN_ERROR_ILLEGAL_TYPE, "" );
              break;
         }
      }
      break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_convolve1D_f", GAN_ERROR_ILLEGAL_TYPE, "" );
        break;
   }

   /* success */
   return GAN_TRUE;
}

/**
 * \brief 1D convolution function for double arrays.
 * \param source The input array to be convolved
 * \param sstride The stride of the source array in units of doubles
 * \param behaviour Behaviour of data beyond edge
 * \param zero_outside Whether to assume zeros outside the source data
 * \param dest The destination array for the convolution
 * \param dstride The stride of the dest array in units of doubles
 * \param mask The convolution mask
 * \param dsize The number of output elements to compute
 *
 * Applies a one-dimensional convolution operation to the given \a source
 * array. Both arrays must be of type \c double.
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 * \sa gan_convolve1D_i().
 */
Gan_Bool
 gan_convolve1D_d ( double *source, int sstride,
                    Gan_EdgeBehaviour behaviour, Gan_Bool zero_outside,
                    double *dest,   int dstride,
                    Gan_Mask1D *mask, int dsize )
{
   int i, j;
   double total;

   gan_err_test_bool ( mask->type == GAN_DOUBLE, "gan_convolve1D_d",
                       GAN_ERROR_INCOMPATIBLE, "" );
   switch ( mask->format )
   {
      case GAN_MASK1D_SYMMETRIC:
      {
         int half_size = (int)mask->size/2;

         switch ( behaviour )
         {
            case GAN_EDGE_BEHAVIOUR_CLIP:
            {
               source += half_size*sstride;
               for ( i = dsize-1; i >= 0;
                     i--, dest += dstride, source += sstride )
               {
                  total = mask->data.d[0]*source[0];
                  for ( j = half_size; j > 0; j-- )
                     total += mask->data.d[j]
                              *(source[j*sstride] + source[-j*sstride]);

                  *dest = total;
               }
            }
            break;

            case GAN_EDGE_BEHAVIOUR_REPEAT:
            {
               for ( i = dsize-1; i >= dsize-half_size; i-- )
               {
                  total = mask->data.d[0]*source[i*sstride];
                  for ( j = half_size; j > 0; j-- )
                     total += mask->data.d[j]
                     *(source[SHIFT_DOWN_REPEAT(i+j,dsize)*sstride] +
                       source[(i-j)*sstride]);

                  dest[i*dstride] = total;
               }

               for ( ; i >= half_size; i-- )
               {
                  total = mask->data.d[0]*source[i*sstride];
                  for ( j = half_size; j > 0; j-- )
                     total += mask->data.d[j]*(source[(i+j)*sstride] +
                                               source[(i-j)*sstride]);

                  dest[i*dstride] = total;
               }

               for ( ; i >= 0; i-- )
               {
                  total = mask->data.d[0]*source[i*sstride];
                  for ( j = half_size; j > 0; j-- )
                     total += mask->data.d[j]
                     *(source[(i+j)*sstride] +
                       source[SHIFT_UP_REPEAT((i-j),dsize)*sstride]);

                  dest[i*dstride] = total;
               }
            }
            break;

            case GAN_EDGE_BEHAVIOUR_CIRCULAR:
            {
               for ( i = dsize-1; i >= 0; i-- )
               {
                  total = mask->data.d[0]*source[i*sstride];
                  for ( j = half_size; j > 0; j-- )
                     total += mask->data.d[j]
                              *(source[SHIFT_DOWN_CIRCULAR(i+j,dsize)*sstride]+
                                source[SHIFT_UP_CIRCULAR(i-j,dsize)*sstride]);

                  dest[i*dstride] = total;
               }
            }
            break;

            default:
              gan_err_flush_trace();
              gan_err_register ( "gan_convolve1D_d", GAN_ERROR_ILLEGAL_TYPE, "" );
              break;
         }
      }
      break;

      case GAN_MASK1D_ANTISYMMETRIC:
      {
         int half_size = (int)mask->size/2;

         switch ( behaviour )
         {
            case GAN_EDGE_BEHAVIOUR_CLIP:
            {
               source += half_size*sstride;
               for ( i = dsize-1; i >= 0;
                     i--, dest += dstride, source += sstride )
               {
                  total = 0.0;
                  for ( j = half_size; j > 0; j-- )
                     total += mask->data.d[j-1]
                     *(source[j*sstride] - source[-j*sstride]);

                  *dest = total;
               }
            }
            break;

            case GAN_EDGE_BEHAVIOUR_REPEAT:
            {
               for ( i = dsize-1; i >= 0; i-- )
               {
                  total = 0.0;
                  for ( j = half_size; j > 0; j-- )
                     total += mask->data.d[j-1]
                        *(source[SHIFT_DOWN_REPEAT(i+j,dsize)*sstride] +
                          source[SHIFT_UP_REPEAT(i-j,dsize)*sstride]);

                  dest[i*dstride] = total;
               }
            }
            break;
            
            case GAN_EDGE_BEHAVIOUR_CIRCULAR:
            {
               for ( i = dsize-1; i >= 0; i-- )
               {
                  total = 0.0;
                  for ( j = half_size; j > 0; j-- )
                     total += mask->data.d[j-1]
                        *(source[SHIFT_DOWN_CIRCULAR(i+j,dsize)*sstride] +
                          source[SHIFT_UP_CIRCULAR(i-j,dsize)*sstride]);

                  dest[i*dstride] = total;
               }
            }
            break;

            default:
              gan_err_flush_trace();
              gan_err_register ( "gan_convolve1D_d", GAN_ERROR_ILLEGAL_TYPE, "" );
              break;
         }
      }
      break;

      case GAN_MASK1D_GENERIC:
      {
         switch ( behaviour )
         {
            case GAN_EDGE_BEHAVIOUR_CLIP:
            {
               source += (mask->size-1)*sstride;
               for ( i = dsize-1; i >= 0;
                     i--, dest += dstride, source += sstride )
               {
                  total = 0.0;
                  for ( j = (int)mask->size-1; j >= 0; j-- )
                     total += mask->data.d[j]*source[-j*sstride];

                  *dest = total;
               }
            }
            break;

            case GAN_EDGE_BEHAVIOUR_REPEAT:
            {
               int half_size = (int)mask->size/2;
               double *mdata = mask->data.d + half_size;

               for ( i = dsize-1; i >= 0; i-- )
               {
                  total = 0.0;
                  for ( j = -half_size; j <= half_size; j++ )
                     total += mdata[j]*source[SHIFT_IN_REPEAT(i-j,dsize)*sstride];

                  dest[i*dstride] = total;
               }
            }
            break;

            case GAN_EDGE_BEHAVIOUR_CIRCULAR:
            {
               int half_size = (int)mask->size/2;
               double *mdata = mask->data.d + half_size;

               for ( i = dsize-1; i >= 0; i-- )
               {
                  total = 0.0;
                  for ( j = -half_size; j <= half_size; j++ )
                     total += mdata[j]*source[SHIFT_IN_CIRCULAR(i-j,dsize)*sstride];

                  dest[i*dstride] = total;
               }
            }
            break;

            default:
              gan_err_flush_trace();
              gan_err_register ( "gan_convolve1D_d", GAN_ERROR_ILLEGAL_TYPE, "" );
              break;
         }
      }
      break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_convolve1D_d", GAN_ERROR_ILLEGAL_TYPE, "" );
        break;
   }

   /* success */
   return GAN_TRUE;
}

/**
 * \brief 1D convolution function for int arrays.
 * \param source The input array to be convolved
 * \param sstride The stride of the source array in units of ints
 * \param behaviour Behaviour of data beyond edge
 * \param zero_outside Whether to assume zeros outside the source data
 * \param dest The destination array for the convolution
 * \param dstride The stride of the dest array in units of ints
 * \param mask The convolution mask
 * \param dsize The number of output elements to compute
 *
 * Applies a one-dimensional convolution operation to the given \a source
 * array. Both arrays must be of type \c int.
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 * \sa gan_convolve1D_i().
 */
Gan_Bool
 gan_convolve1D_i ( int *source, int sstride,
                    Gan_EdgeBehaviour behaviour, Gan_Bool zero_outside,
                    int *dest,   int dstride,
                    Gan_Mask1D *mask, int dsize )
{
   int i, j;
   int total;

   gan_err_test_bool ( mask->type == GAN_INT, "gan_convolve1D_i",
                       GAN_ERROR_INCOMPATIBLE, "" );
   switch ( mask->format )
   {
      case GAN_MASK1D_SYMMETRIC:
      {
         int half_size = (int)mask->size/2;

         switch ( behaviour )
         {
            case GAN_EDGE_BEHAVIOUR_CLIP:
            {
               source += half_size*sstride;
               for ( i = dsize-1; i >= 0;
                     i--, dest += dstride, source += sstride )
               {
                  total = mask->data.i[0]*source[0];
                  for ( j = half_size; j > 0; j-- )
                     total += mask->data.i[j]
                              *(source[j*sstride] + source[-j*sstride]);

                  *dest = total;
               }
            }
            break;

            case GAN_EDGE_BEHAVIOUR_REPEAT:
            {
               for ( i = dsize-1; i >= dsize-half_size; i-- )
               {
                  total = mask->data.i[0]*source[i*sstride];
                  for ( j = half_size; j > 0; j-- )
                     total += mask->data.i[j]
                     *(source[SHIFT_DOWN_REPEAT(i+j,dsize)*sstride] +
                       source[(i-j)*sstride]);

                  dest[i*dstride] = total;
               }

               for ( ; i >= half_size; i-- )
               {
                  total = mask->data.i[0]*source[i*sstride];
                  for ( j = half_size; j > 0; j-- )
                     total += mask->data.i[j]*(source[(i+j)*sstride] +
                                               source[(i-j)*sstride]);

                  dest[i*dstride] = total;
               }

               for ( ; i >= 0; i-- )
               {
                  total = mask->data.i[0]*source[i*sstride];
                  for ( j = half_size; j > 0; j-- )
                     total += mask->data.i[j]
                     *(source[(i+j)*sstride] +
                       source[SHIFT_UP_REPEAT((i-j),dsize)*sstride]);

                  dest[i*dstride] = total;
               }
            }
            break;

            case GAN_EDGE_BEHAVIOUR_CIRCULAR:
            {
               for ( i = dsize-1; i >= 0; i-- )
               {
                  total = mask->data.i[0]*source[i*sstride];
                  for ( j = half_size; j > 0; j-- )
                     total += mask->data.i[j]
                              *(source[SHIFT_DOWN_CIRCULAR(i+j,dsize)*sstride]+
                                source[SHIFT_UP_CIRCULAR(i-j,dsize)*sstride]);

                  dest[i*dstride] = total;
               }
            }
            break;

            default:
              gan_err_flush_trace();
              gan_err_register ( "gan_convolve1D_i", GAN_ERROR_ILLEGAL_TYPE, "" );
              break;
         }
      }
      break;

      case GAN_MASK1D_ANTISYMMETRIC:
      {
         int half_size = (int)mask->size/2;

         switch ( behaviour )
         {
            case GAN_EDGE_BEHAVIOUR_CLIP:
            {
               source += half_size*sstride;
               for ( i = dsize-1; i >= 0;
                     i--, dest += dstride, source += sstride )
               {
                  total = 0;
                  for ( j = half_size; j > 0; j-- )
                     total += mask->data.i[j-1]
                     *(source[j*sstride] - source[-j*sstride]);

                  *dest = total;
               }
            }
            break;

            case GAN_EDGE_BEHAVIOUR_REPEAT:
            {
               for ( i = dsize-1; i >= 0; i-- )
               {
                  total = 0;
                  for ( j = half_size; j > 0; j-- )
                     total += mask->data.i[j-1]
                        *(source[SHIFT_DOWN_REPEAT(i+j,dsize)*sstride] +
                          source[SHIFT_UP_REPEAT(i-j,dsize)*sstride]);

                  dest[i*dstride] = total;
               }
            }
            break;
            
            case GAN_EDGE_BEHAVIOUR_CIRCULAR:
            {
               for ( i = dsize-1; i >= 0; i-- )
               {
                  total = 0;
                  for ( j = half_size; j > 0; j-- )
                     total += mask->data.i[j-1]
                        *(source[SHIFT_DOWN_CIRCULAR(i+j,dsize)*sstride] +
                          source[SHIFT_UP_CIRCULAR(i-j,dsize)*sstride]);

                  dest[i*dstride] = total;
               }
            }
            break;

            default:
              gan_err_flush_trace();
              gan_err_register ( "gan_convolve1D_i", GAN_ERROR_ILLEGAL_TYPE, "" );
              break;
         }
      }
      break;

      case GAN_MASK1D_GENERIC:
      {
         switch ( behaviour )
         {
            case GAN_EDGE_BEHAVIOUR_CLIP:
            {
               source += (mask->size-1)*sstride;
               for ( i = dsize-1; i >= 0;
                     i--, dest += dstride, source += sstride )
               {
                  total = 0;
                  for ( j = (int)mask->size-1; j >= 0; j-- )
                     total += mask->data.i[j]*source[-j*sstride];

                  *dest = total;
               }
            }
            break;

            case GAN_EDGE_BEHAVIOUR_REPEAT:
            {
               int half_size = (int)mask->size/2;
               int *mdata = mask->data.i + half_size;

               for ( i = dsize-1; i >= 0; i-- )
               {
                  total = 0;
                  for ( j = -half_size; j <= half_size; j++ )
                     total += mdata[j]*source[SHIFT_IN_REPEAT(i-j,dsize)*sstride];

                  dest[i*dstride] = total;
               }
            }
            break;

            case GAN_EDGE_BEHAVIOUR_CIRCULAR:
            {
               int half_size = (int)mask->size/2;
               int *mdata = mask->data.i + half_size;

               for ( i = dsize-1; i >= 0; i-- )
               {
                  total = 0;
                  for ( j = -half_size; j <= half_size; j++ )
                     total += mdata[j]*source[SHIFT_IN_CIRCULAR(i-j,dsize)*sstride];

                  dest[i*dstride] = total;
               }
            }
            break;

            default:
              gan_err_flush_trace();
              gan_err_register ( "gan_convolve1D_i", GAN_ERROR_ILLEGAL_TYPE, "" );
              break;
         }
      }
      break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_convolve1D_i", GAN_ERROR_ILLEGAL_TYPE, "" );
        break;
   }

   /* success */
   return GAN_TRUE;
}

/**
 * \brief 1D convolution function for integer arrays.
 * \param source The input array to be convolved
 * \param sstride The stride of the source array
 * \param behaviour Behaviour of data beyond edge
 * \param zero_outside Whether to assume zeros outside the source data
 * \param dest The destination array for the convolution
 * \param dstride The stride of the dest array
 * \param mask The convolution mask
 * \param dsize The number of output elements to compute
 *
 * Applies a one-dimensional convolution operation to the given \a source
 * array. Both arrays must be of type \c unsigned \c char.
 * There is no checking for overflow.
 *
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 * \sa gan_convolve1D_u().
 */
Gan_Bool
 gan_convolve1D_uc ( unsigned char *source, int sstride,
                     Gan_EdgeBehaviour behaviour, Gan_Bool zero_outside,
                     unsigned char *dest,   int dstride,
                     Gan_Mask1D *mask, int dsize )
{
   int i, j;
   float total;

   gan_err_test_bool ( mask->type == GAN_FLOAT, "gan_convolve1D_uc",
                       GAN_ERROR_INCOMPATIBLE, "" );
   switch ( mask->format )
   {
      case GAN_MASK1D_SYMMETRIC:
      {
         int half_size = (int)mask->size/2;

         switch ( behaviour )
         {
            case GAN_EDGE_BEHAVIOUR_CLIP:
            {
               source += half_size*sstride;
               for ( i = dsize-1; i >= 0;
                     i--, dest += dstride, source += sstride )
               {
                  total = (mask->data.f[0]*(float)source[0]);
                  for ( j = (int)half_size; j > 0; j-- )
                     total += mask->data.f[j]*((float)source[j*sstride] +
                                               (float)source[-j*sstride]);

                  *dest = (unsigned char) (total+0.5F);
               }
            }
            break;

            case GAN_EDGE_BEHAVIOUR_REPEAT:
            {
               for ( i = dsize-1; i >= dsize-half_size; i-- )
               {
                  total = mask->data.f[0]*(float)source[i*sstride];
                  for ( j = half_size; j > 0; j-- )
                     total += mask->data.f[j]
                     *((float)source[SHIFT_DOWN_REPEAT(i+j,dsize)*sstride] +
                       (float)source[(i-j)*sstride]);

                  dest[i*dstride] = (unsigned char) (total+0.5F);
               }

               for ( ; i >= half_size; i-- )
               {
                  total = mask->data.f[0]*(float)source[i*sstride];
                  for ( j = half_size; j > 0; j-- )
                     total += mask->data.f[j]*((float)source[(i+j)*sstride] +
                                               (float)source[(i-j)*sstride]);

                  dest[i*dstride] = (unsigned char) (total+0.5F);
               }

               for ( ; i >= 0; i-- )
               {
                  total = mask->data.f[0]*(float)source[i*sstride];
                  for ( j = half_size; j > 0; j-- )
                     total += mask->data.f[j]
                     *((float)source[(i+j)*sstride] +
                       (float)source[SHIFT_UP_REPEAT((i-j),dsize)*sstride]);

                  dest[i*dstride] = (unsigned char) (total+0.5F);
               }
            }
            break;

            case GAN_EDGE_BEHAVIOUR_CIRCULAR:
            {
               for ( i = dsize-1; i >= 0; i-- )
               {
                  total = mask->data.f[0]*(float)source[i*sstride];
                  for ( j = half_size; j > 0; j-- )
                     total += mask->data.f[j]
                              *((float)source[SHIFT_DOWN_CIRCULAR(i+j,dsize)*sstride]+
                                (float)source[SHIFT_UP_CIRCULAR(i-j,dsize)*sstride]);

                  dest[i*dstride] = (unsigned char) (total+0.5F);
               }
            }
            break;

            case GAN_EDGE_BEHAVIOUR_EXPAND:
            {
               if(zero_outside)
                  for ( i = dsize-1; i >= dsize-2*half_size; i-- )
                  {
                     total = 0.0F;
                     for ( j = 0; j <= half_size; j++ )
                     {
                        if(i+j >= dsize-half_size)
                           break;

                        total += mask->data.f[j]*(float)source[(i-half_size+j)*sstride];
                     }

                     for ( j = half_size; j > 0; j-- )
                     {
                        if(i-j >= dsize-half_size)
                           break;

                        total += mask->data.f[j]*(float)source[(i-half_size-j)*sstride];
                     }

                     dest[i*dstride] = (unsigned char) (total+0.5F);
                  }
               else
                  for ( i = dsize-1; i >= dsize-2*half_size; i-- )
                  {
                     total = 0.0F;
                     for ( j = 0; j <= half_size; j++ )
                        if(i+j >= dsize-half_size)
                           total += mask->data.f[j]*(float)source[(dsize-2*half_size-1)*sstride];
                        else
                           total += mask->data.f[j]*(float)source[(i-half_size+j)*sstride];

                     for ( j = half_size; j > 0; j-- )
                        if(i-j >= dsize-half_size)
                           total += mask->data.f[j]*(float)source[(dsize-2*half_size-1)*sstride];
                        else
                           total += mask->data.f[j]*(float)source[(i-half_size-j)*sstride];

                     dest[i*dstride] = (unsigned char) (total+0.5F);
                  }

               for ( ; i >= 2*half_size; i-- )
               {
                  total = mask->data.f[0]*(float)source[(i-half_size)*sstride];
                  for ( j = half_size; j > 0; j-- )
                     total += mask->data.f[j]*(source[(i-half_size+j)*sstride] +
                                               source[(i-half_size-j)*sstride]);

                  dest[i*dstride] = (unsigned char) (total+0.5F);
               }

               if(zero_outside)
                  for ( ; i >= 0; i-- )
                  {
                     total = 0.0F;
                     for ( j = half_size; j >= 0; j-- )
                     {
                        if(i+j < half_size)
                           break;

                        total += mask->data.f[j]*(float)source[(i-half_size+j)*sstride];
                     }

                     for ( j = 1; j <= half_size; j++ )
                     {
                        if(i-j < half_size)
                           break;

                        total += mask->data.f[j]*(float)source[(i-half_size-j)*sstride];
                     }

                     dest[i*dstride] = (unsigned char) (total+0.5F);
                  }
               else
                  for ( ; i >= 0; i-- )
                  {
                     total = 0.0F;
                     for ( j = half_size; j >= 0; j-- )
                        if(i+j < half_size)
                           total += mask->data.f[j]*(float)source[0];
                        else
                           total += mask->data.f[j]*(float)source[(i-half_size+j)*sstride];

                     for ( j = 1; j <= half_size; j++ )
                        if(i-j < half_size)
                           total += mask->data.f[j]*(float)source[0];
                        else
                           total += mask->data.f[j]*(float)source[(i-half_size-j)*sstride];

                     dest[i*dstride] = (unsigned char) (total+0.5F);
                  }
            }
            break;

            default:
              gan_err_flush_trace();
              gan_err_register ( "gan_convolve1D_uc", GAN_ERROR_ILLEGAL_TYPE, "" );
              break;
         }
      }
      break;

      case GAN_MASK1D_ANTISYMMETRIC:
      {
         int half_size = (int)mask->size/2;

         switch ( behaviour )
         {
            case GAN_EDGE_BEHAVIOUR_CLIP:
            {
               source += half_size*sstride;
               for ( i = dsize-1; i >= 0;
                     i--, dest += dstride, source += sstride )
               {
                  total = 0.0F;
                  for ( j = half_size; j > 0; j-- )
                     total += mask->data.f[j-1]
                     *((float)source[j*sstride] - (float)source[-j*sstride]);

                  *dest = (unsigned char) (total+0.5F);
               }
            }
            break;

            case GAN_EDGE_BEHAVIOUR_REPEAT:
            {
               for ( i = dsize-1; i >= 0; i-- )
               {
                  total = 0.0F;
                  for ( j = half_size; j > 0; j-- )
                     total += mask->data.f[j-1]
                        *((float)source[SHIFT_DOWN_REPEAT(i+j,dsize)*sstride] +
                          (float)source[SHIFT_UP_REPEAT(i-j,dsize)*sstride]);

                  dest[i*dstride] = (unsigned char) (total+0.5F);
               }
            }
            break;
            
            case GAN_EDGE_BEHAVIOUR_CIRCULAR:
            {
               for ( i = dsize-1; i >= 0; i-- )
               {
                  total = 0.0F;
                  for ( j = half_size; j > 0; j-- )
                     total += mask->data.f[j-1]
                        *((float)source[SHIFT_DOWN_CIRCULAR(i+j,dsize)*sstride] +
                          (float)source[SHIFT_UP_CIRCULAR(i-j,dsize)*sstride]);

                  dest[i*dstride] = (unsigned char) (total+0.5F);
               }
            }
            break;

            default:
              gan_err_flush_trace();
              gan_err_register ( "gan_convolve1D_uc", GAN_ERROR_ILLEGAL_TYPE, "" );
              break;
         }
      }
      break;

      case GAN_MASK1D_GENERIC:
      {
         switch ( behaviour )
         {
            case GAN_EDGE_BEHAVIOUR_CLIP:
            {
               source += (mask->size-1)*sstride;
               for ( i = dsize-1; i >= 0;
                     i--, dest += dstride, source += sstride )
               {
                  total = 0.0F;
                  for ( j = (int)mask->size-1; j >= 0; j-- )
                     total += mask->data.f[j]*(float)source[-j*sstride];

                  *dest = (unsigned char) (total+0.5F);
               }
            }
            break;

            case GAN_EDGE_BEHAVIOUR_REPEAT:
            {
               int half_size = (int)mask->size/2;
               float *mdata = mask->data.f + half_size;

               for ( i = dsize-1; i >= 0; i-- )
               {
                  total = 0.0F;
                  for ( j = -half_size; j <= half_size; j++ )
                     total += mdata[j]*(float)source[SHIFT_IN_REPEAT(i-j,dsize)*sstride];

                  dest[i*dstride] = (unsigned char) (total+0.5F);
               }
            }
            break;

            case GAN_EDGE_BEHAVIOUR_CIRCULAR:
            {
               int half_size = (int)mask->size/2;
               float *mdata = mask->data.f + half_size;

               for ( i = dsize-1; i >= 0; i-- )
               {
                  total = 0.0F;
                  for ( j = -half_size; j <= half_size; j++ )
                     total += mdata[j]*(float)source[SHIFT_IN_CIRCULAR(i-j,dsize)*sstride];

                  dest[i*dstride] = (unsigned char) (total+0.5F);
               }
            }
            break;

            default:
              gan_err_flush_trace();
              gan_err_register ( "gan_convolve1D_uc", GAN_ERROR_ILLEGAL_TYPE, "" );
              break;
         }
      }
      break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_convolve1D_uc", GAN_ERROR_ILLEGAL_TYPE, "" );
        break;
   }

   /* success */
   return GAN_TRUE;
}

/**
 * \brief 1D convolution function for integer RGB arrays.
 * \param source The input array to be convolved
 * \param sstride The stride of the source array
 * \param dest The destination array for the convolution
 * \param dstride The stride of the dest array
 * \param mask The convolution mask
 * \param dsize The number of output elements to compute
 *
 * Applies a one-dimensional convolution operation to the given \a source
 * array. Both arrays must be of type \c unsigned \c char.
 * There is no checking for overflow.
 *
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 * \sa gan_convolve1D_rgb_uc().
 */
Gan_Bool
 gan_convolve1D_rgb_uc ( unsigned char *source, int sstride,
                         unsigned char *dest,   int dstride,
                         Gan_Mask1D *mask, unsigned int dsize )
{
   int i, j;
   float totalr, totalb, totalg, total;
#if defined(__APPLE__) && defined(ALTIVEC_CONVOLUTION)
   floatToVector vecfSourcePos, vecfSourceNeg, vecfTotal, vecfMask;
   int c;
#endif
   
   gan_err_test_bool ( mask->type == GAN_FLOAT, "gan_convolve1D_rgb_uc", GAN_ERROR_INCOMPATIBLE, "" );
   switch ( mask->format )
   {
      case GAN_MASK1D_SYMMETRIC:
      {
         unsigned half_size = mask->size/2;

         source += half_size*sstride;
         for ( i = (int)dsize-1; i >= 0;
               i--, dest += dstride, source += sstride )
         {
#if defined(__APPLE__) && defined(ALTIVEC_CONVOLUTION)
            vecfSourcePos.flt[0] = (float)source[0];
            vecfSourcePos.flt[1] = (float)source[1];
            vecfSourcePos.flt[2] = (float)source[2];

            vecfMask.flt[0] = mask->data.f[0];
            vecfMask.flt[1] = mask->data.f[0];
            vecfMask.flt[2] = mask->data.f[0];

            vecfSourceNeg.flt[0] = 0.0f;
            vecfSourceNeg.flt[1] = 0.0f;
            vecfSourceNeg.flt[2] = 0.0f;

            vecfTotal.vFlt = vec_madd(vecfSourcePos.vFlt, vecfMask.vFlt, vecfSourceNeg.vFlt);
            
            for ( j = (int)half_size; j > 0; j-- )
            {
               unsigned char* puchSourcePos = &source[j*sstride];
               unsigned char* puchSourceNeg = &source[-j*sstride];

               for (c = 0; c < 3; c++)
               {
                  vecfSourcePos.flt[c] = (float)*puchSourcePos;
                  vecfSourceNeg.flt[c] = (float)*puchSourceNeg;
                  vecfMask.flt[c] = mask->data.f[j];
                  puchSourcePos++;
                  puchSourceNeg++;
               }

               vecfTotal.vFlt = vec_madd(vecfSourcePos.vFlt, vecfMask.vFlt, vecfTotal.vFlt);
               vecfTotal.vFlt = vec_madd(vecfSourceNeg.vFlt, vecfMask.vFlt, vecfTotal.vFlt);
            }

            for (c = 0; c < 3; c++)
            {
               dest[c] = (unsigned char) vecfTotal.flt[c];
            }
#else
            totalr = (mask->data.f[0]*(float)source[0]);
            totalg = (mask->data.f[0]*(float)source[1]);
            totalb = (mask->data.f[0]*(float)source[2]);
            for ( j = (int)half_size; j > 0; j-- )
            {

               unsigned char* puchSourcePos = &source[j*sstride];
               unsigned char* puchSourceNeg = &source[-j*sstride];
               totalr += mask->data.f[j]*((float)*puchSourcePos +
                                         (float)*puchSourceNeg);
               puchSourcePos++;
               puchSourceNeg++;
               totalg += mask->data.f[j]*((float)*puchSourcePos +
                                         (float)*puchSourceNeg);
               puchSourcePos++;
               puchSourceNeg++;
               totalb += mask->data.f[j]*((float)*puchSourcePos +
                                         (float)*puchSourceNeg);
            }
            dest[0] = (unsigned char) totalr;
            dest[1] = (unsigned char) totalg;
            dest[2] = (unsigned char) totalb;
#endif
            }
      }
         break;

      case GAN_MASK1D_ANTISYMMETRIC:
      {
         unsigned half_size = mask->size/2;

         source += half_size*sstride;
         for ( i = (int)dsize-1; i >= 0;
               i--, dest += dstride, source += sstride )
         {
            total = 0;
            for ( j = (int)half_size; j > 0; j-- )
               total += mask->data.f[j-1]*((float)source[j*sstride] -
                                           (float)source[-j*sstride]);

            *dest = (unsigned char) total;
         }
      }
         break;

      case GAN_MASK1D_GENERIC:
      {
         source += mask->size*sstride;
         for ( i = (int)dsize-1; i >= 0;
               i--, dest += dstride, source += sstride )
         {
            total = 0;
            for ( j = (int)mask->size-1; j >= 0; j-- )
               total +=  (mask->data.f[j]*(float)source[-j*sstride]);

            *dest = (unsigned char) total;
         }
      }
         break;

      default:
         gan_err_flush_trace();
         gan_err_register ( "gan_convolve1D_uc", GAN_ERROR_ILLEGAL_TYPE, "" );
         break;
   }

   /* success */
   return GAN_TRUE;
}

/**
* \brief 1D convolution function for integer RGB arrays.
 * \param source The input array to be convolved
 * \param sstride The stride of the source array
 * \param dest The destination array for the convolution
 * \param dstride The stride of the dest array
 * \param mask The convolution mask
 * \param dsize The number of output elements to compute
 *
 * Applies a one-dimensional convolution operation to the given \a source
 * array. Both arrays must be of type \c unsigned \c char.
 * There is no checking for overflow.
 *
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 * \sa gan_convolve1D_rgb_uc().
 */
Gan_Bool
 gan_convolve1D_rgb_f (   float *source, int sstride,
                          float *dest,   int dstride,
                          Gan_Mask1D *mask, unsigned int dsize)
{
   int i, j;
   float totalr, totalb, totalg, total;
#if defined(__APPLE__) && defined(ALTIVEC_CONVOLUTION)
   floatToVector vecfSourcePos, vecfSourceNeg, vecfTotal, vecfMask;
#endif

   gan_err_test_bool ( mask->type == GAN_FLOAT, "gan_convolve1D_rgb_f",
                       GAN_ERROR_INCOMPATIBLE, "" );
   switch ( mask->format )
   {
      case GAN_MASK1D_SYMMETRIC:
      {
         unsigned half_size = mask->size/2;

         source += half_size*sstride;
         for ( i = (int)dsize-1; i >= 0;
               i--, dest += dstride, source += sstride )
         {
#if defined(__APPLE__) && defined(ALTIVEC_CONVOLUTION)
            vecfSourcePos.flt[0] = source[0];
            vecfSourcePos.flt[1] = source[1];
            vecfSourcePos.flt[2] = source[2];

            vecfMask.flt[0] = mask->data.f[0];
            vecfMask.flt[1] = mask->data.f[0];
            vecfMask.flt[2] = mask->data.f[0];

            vecfSourceNeg.flt[0] = 0.0f;
            vecfSourceNeg.flt[1] = 0.0f;
            vecfSourceNeg.flt[2] = 0.0f;

            vecfTotal.vFlt = vec_madd(vecfSourcePos.vFlt, vecfMask.vFlt, vecfSourceNeg.vFlt);

            for ( j = (int)half_size; j > 0; j-- )
            {
               float* pfSourcePos = &source[j*sstride];
               float* pfSourceNeg = &source[-j*sstride];

               for (c = 0; c < 3; c++)
               {
                  vecfSourcePos.flt[c] = *pfSourcePos;
                  vecfSourceNeg.flt[c] = *pfSourceNeg;
                  vecfMask.flt[c] = mask->data.f[j];
                  pfSourcePos++;
                  pfSourceNeg++;
               }

               vecfTotal.vFlt = vec_madd(vecfSourcePos.vFlt, vecfMask.vFlt, vecfTotal.vFlt);
               vecfTotal.vFlt = vec_madd(vecfSourceNeg.vFlt, vecfMask.vFlt, vecfTotal.vFlt);
            }

            for (c = 0; c < 3; c++)
            {
               dest[c] = vecfTotal.flt[c];
            }
#else
            totalr = (mask->data.f[0]*source[0]);
            totalg = (mask->data.f[0]*source[1]);
            totalb = (mask->data.f[0]*source[2]);
            for ( j = (int)half_size; j > 0; j-- )
            {

               float* pfSourcePos = &source[j*sstride];
               float* pfSourceNeg = &source[-j*sstride];
               totalr += mask->data.f[j]*(*pfSourcePos +*pfSourceNeg);
               pfSourcePos++;
               pfSourceNeg++;
               totalg += mask->data.f[j]*(*pfSourcePos + *pfSourceNeg);
               pfSourcePos++;
               pfSourceNeg++;
               totalb += mask->data.f[j]*(*pfSourcePos + *pfSourceNeg);
            }
            dest[0] = totalr;
            dest[1] = totalg;
            dest[2] = totalb;
#endif
         }
      }
         break;

      case GAN_MASK1D_ANTISYMMETRIC:
      {
         unsigned half_size = mask->size/2;

         source += half_size*sstride;
         for ( i = (int)dsize-1; i >= 0;
               i--, dest += dstride, source += sstride )
         {
            total = 0.0;
            for ( j = (int)half_size; j > 0; j-- )
               total += mask->data.f[j-1]
                  *(source[j*sstride] - source[-j*sstride]);

            *dest = total;
         }
      }
         break;

      case GAN_MASK1D_GENERIC:
      {
         source += (mask->size-1)*sstride;
         for ( i = (int)dsize-1; i >= 0;
               i--, dest += dstride, source += sstride )
         {
            total = 0.0;
            for ( j = (int)mask->size-1; j >= 0; j-- )
               total += mask->data.f[j]*source[-j*sstride];

            *dest = total;
         }
      }
         break;

      default:
         gan_err_flush_trace();
         gan_err_register ( "gan_convolve1D_rgb_f", GAN_ERROR_ILLEGAL_TYPE, "" );
         break;
   }

   /* success */
   return GAN_TRUE;
}

/**
 * \brief 1D convolution function for integer BGR arrays.
 * \param source The input array to be convolved
 * \param sstride The stride of the source array
 * \param dest The destination array for the convolution
 * \param dstride The stride of the dest array
 * \param mask The convolution mask
 * \param dsize The number of output elements to compute
 *
 * Applies a one-dimensional convolution operation to the given \a source
 * array. Both arrays must be of type \c unsigned \c char.
 * There is no checking for overflow.
 *
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 * \sa gan_convolve1D_bgr_uc().
 */
Gan_Bool
 gan_convolve1D_bgr_uc ( unsigned char *source, int sstride,
                         unsigned char *dest,   int dstride,
                         Gan_Mask1D *mask, unsigned int dsize )
{
   int i, j;
   float totalr, totalb, totalg, total;
#if defined(__APPLE__) && defined(ALTIVEC_CONVOLUTION)
   floatToVector vecfSourcePos, vecfSourceNeg, vecfTotal, vecfMask;
   int c;
#endif
   
   gan_err_test_bool ( mask->type == GAN_FLOAT, "gan_convolve1D_bgr_uc", GAN_ERROR_INCOMPATIBLE, "" );
   switch ( mask->format )
   {
      case GAN_MASK1D_SYMMETRIC:
      {
         unsigned half_size = mask->size/2;

         source += half_size*sstride;
         for ( i = (int)dsize-1; i >= 0;
               i--, dest += dstride, source += sstride )
         {
#if defined(__APPLE__) && defined(ALTIVEC_CONVOLUTION)
            vecfSourcePos.flt[0] = (float)source[0];
            vecfSourcePos.flt[1] = (float)source[1];
            vecfSourcePos.flt[2] = (float)source[2];

            vecfMask.flt[0] = mask->data.f[0];
            vecfMask.flt[1] = mask->data.f[0];
            vecfMask.flt[2] = mask->data.f[0];

            vecfSourceNeg.flt[0] = 0.0f;
            vecfSourceNeg.flt[1] = 0.0f;
            vecfSourceNeg.flt[2] = 0.0f;

            vecfTotal.vFlt = vec_madd(vecfSourcePos.vFlt, vecfMask.vFlt, vecfSourceNeg.vFlt);
            
            for ( j = (int)half_size; j > 0; j-- )
            {
               unsigned char* puchSourcePos = &source[j*sstride];
               unsigned char* puchSourceNeg = &source[-j*sstride];

               for (c = 0; c < 3; c++)
               {
                  vecfSourcePos.flt[c] = (float)*puchSourcePos;
                  vecfSourceNeg.flt[c] = (float)*puchSourceNeg;
                  vecfMask.flt[c] = mask->data.f[j];
                  puchSourcePos++;
                  puchSourceNeg++;
               }

               vecfTotal.vFlt = vec_madd(vecfSourcePos.vFlt, vecfMask.vFlt, vecfTotal.vFlt);
               vecfTotal.vFlt = vec_madd(vecfSourceNeg.vFlt, vecfMask.vFlt, vecfTotal.vFlt);
            }

            for (c = 0; c < 3; c++)
            {
               dest[c] = (unsigned char) vecfTotal.flt[c];
            }
#else
            totalr = (mask->data.f[0]*(float)source[0]);
            totalg = (mask->data.f[0]*(float)source[1]);
            totalb = (mask->data.f[0]*(float)source[2]);
            for ( j = (int)half_size; j > 0; j-- )
            {

               unsigned char* puchSourcePos = &source[j*sstride];
               unsigned char* puchSourceNeg = &source[-j*sstride];
               totalr += mask->data.f[j]*((float)*puchSourcePos +
                                         (float)*puchSourceNeg);
               puchSourcePos++;
               puchSourceNeg++;
               totalg += mask->data.f[j]*((float)*puchSourcePos +
                                         (float)*puchSourceNeg);
               puchSourcePos++;
               puchSourceNeg++;
               totalb += mask->data.f[j]*((float)*puchSourcePos +
                                         (float)*puchSourceNeg);
            }
            dest[0] = (unsigned char) totalr;
            dest[1] = (unsigned char) totalg;
            dest[2] = (unsigned char) totalb;
#endif
            }
      }
         break;

      case GAN_MASK1D_ANTISYMMETRIC:
      {
         unsigned half_size = mask->size/2;

         source += half_size*sstride;
         for ( i = (int)dsize-1; i >= 0;
               i--, dest += dstride, source += sstride )
         {
            total = 0;
            for ( j = (int)half_size; j > 0; j-- )
               total += mask->data.f[j-1]*((float)source[j*sstride] -
                                           (float)source[-j*sstride]);

            *dest = (unsigned char) total;
         }
      }
         break;

      case GAN_MASK1D_GENERIC:
      {
         source += mask->size*sstride;
         for ( i = (int)dsize-1; i >= 0;
               i--, dest += dstride, source += sstride )
         {
            total = 0;
            for ( j = (int)mask->size-1; j >= 0; j-- )
               total +=  (mask->data.f[j]*(float)source[-j*sstride]);

            *dest = (unsigned char) total;
         }
      }
         break;

      default:
         gan_err_flush_trace();
         gan_err_register ( "gan_convolve1D_uc", GAN_ERROR_ILLEGAL_TYPE, "" );
         break;
   }

   /* success */
   return GAN_TRUE;
}

/**
* \brief 1D convolution function for integer BGR arrays.
 * \param source The input array to be convolved
 * \param sstride The stride of the source array
 * \param dest The destination array for the convolution
 * \param dstride The stride of the dest array
 * \param mask The convolution mask
 * \param dsize The number of output elements to compute
 *
 * Applies a one-dimensional convolution operation to the given \a source
 * array. Both arrays must be of type \c unsigned \c char.
 * There is no checking for overflow.
 *
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 * \sa gan_convolve1D_bgr_uc().
 */
Gan_Bool
 gan_convolve1D_bgr_f (   float *source, int sstride,
                          float *dest,   int dstride,
                          Gan_Mask1D *mask, unsigned int dsize)
{
   int i, j;
   float totalr, totalb, totalg, total;
#if defined(__APPLE__) && defined(ALTIVEC_CONVOLUTION)
   floatToVector vecfSourcePos, vecfSourceNeg, vecfTotal, vecfMask;
#endif

   gan_err_test_bool ( mask->type == GAN_FLOAT, "gan_convolve1D_bgr_f",
                       GAN_ERROR_INCOMPATIBLE, "" );
   switch ( mask->format )
   {
      case GAN_MASK1D_SYMMETRIC:
      {
         unsigned half_size = mask->size/2;

         source += half_size*sstride;
         for ( i = (int)dsize-1; i >= 0;
               i--, dest += dstride, source += sstride )
         {
#if defined(__APPLE__) && defined(ALTIVEC_CONVOLUTION)
            vecfSourcePos.flt[0] = source[0];
            vecfSourcePos.flt[1] = source[1];
            vecfSourcePos.flt[2] = source[2];

            vecfMask.flt[0] = mask->data.f[0];
            vecfMask.flt[1] = mask->data.f[0];
            vecfMask.flt[2] = mask->data.f[0];

            vecfSourceNeg.flt[0] = 0.0f;
            vecfSourceNeg.flt[1] = 0.0f;
            vecfSourceNeg.flt[2] = 0.0f;

            vecfTotal.vFlt = vec_madd(vecfSourcePos.vFlt, vecfMask.vFlt, vecfSourceNeg.vFlt);

            for ( j = (int)half_size; j > 0; j-- )
            {
               float* pfSourcePos = &source[j*sstride];
               float* pfSourceNeg = &source[-j*sstride];

               for (c = 0; c < 3; c++)
               {
                  vecfSourcePos.flt[c] = *pfSourcePos;
                  vecfSourceNeg.flt[c] = *pfSourceNeg;
                  vecfMask.flt[c] = mask->data.f[j];
                  pfSourcePos++;
                  pfSourceNeg++;
               }

               vecfTotal.vFlt = vec_madd(vecfSourcePos.vFlt, vecfMask.vFlt, vecfTotal.vFlt);
               vecfTotal.vFlt = vec_madd(vecfSourceNeg.vFlt, vecfMask.vFlt, vecfTotal.vFlt);
            }

            for (c = 0; c < 3; c++)
            {
               dest[c] = vecfTotal.flt[c];
            }
#else
            totalr = (mask->data.f[0]*source[0]);
            totalg = (mask->data.f[0]*source[1]);
            totalb = (mask->data.f[0]*source[2]);
            for ( j = (int)half_size; j > 0; j-- )
            {

               float* pfSourcePos = &source[j*sstride];
               float* pfSourceNeg = &source[-j*sstride];
               totalr += mask->data.f[j]*(*pfSourcePos +*pfSourceNeg);
               pfSourcePos++;
               pfSourceNeg++;
               totalg += mask->data.f[j]*(*pfSourcePos + *pfSourceNeg);
               pfSourcePos++;
               pfSourceNeg++;
               totalb += mask->data.f[j]*(*pfSourcePos + *pfSourceNeg);
            }
            dest[0] = totalr;
            dest[1] = totalg;
            dest[2] = totalb;
#endif
         }
      }
         break;

      case GAN_MASK1D_ANTISYMMETRIC:
      {
         unsigned half_size = mask->size/2;

         source += half_size*sstride;
         for ( i = (int)dsize-1; i >= 0;
               i--, dest += dstride, source += sstride )
         {
            total = 0.0;
            for ( j = (int)half_size; j > 0; j-- )
               total += mask->data.f[j-1]
                  *(source[j*sstride] - source[-j*sstride]);

            *dest = total;
         }
      }
         break;

      case GAN_MASK1D_GENERIC:
      {
         source += (mask->size-1)*sstride;
         for ( i = (int)dsize-1; i >= 0;
               i--, dest += dstride, source += sstride )
         {
            total = 0.0;
            for ( j = (int)mask->size-1; j >= 0; j-- )
               total += mask->data.f[j]*source[-j*sstride];

            *dest = total;
         }
      }
         break;

      default:
         gan_err_flush_trace();
         gan_err_register ( "gan_convolve1D_bgr_f", GAN_ERROR_ILLEGAL_TYPE, "" );
         break;
   }

   /* success */
   return GAN_TRUE;
}

/**
 * \brief 1D convolution function for integer arrays.
 * \param source The input array to be convolved
 * \param sstride The stride of the source array
 * \param behaviour Behaviour of data beyond edge
 * \param zero_outside Whether to assume zeros outside the source data
 * \param dest The destination array for the convolution
 * \param dstride The stride of the dest array
 * \param mask The convolution mask
 * \param dsize The number of output elements to compute
 *
 * Applies a one-dimensional convolution operation to the given \a source
 * array. Both arrays must be of type \c unsigned \c short.
 * There is no checking for overflow.
 *
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 * \sa gan_convolve1D_uc().
 */
Gan_Bool
 gan_convolve1D_us ( unsigned short *source, int sstride,
                     Gan_EdgeBehaviour behaviour, Gan_Bool zero_outside,
                     unsigned short *dest,   int dstride,
                     Gan_Mask1D *mask, int dsize )
{
   int i, j;
   float total;

   gan_err_test_bool ( mask->type == GAN_FLOAT, "gan_convolve1D_us",
                       GAN_ERROR_INCOMPATIBLE, "" );
   switch ( mask->format )
   {
      case GAN_MASK1D_SYMMETRIC:
      {
         int half_size = (int)mask->size/2;

         switch ( behaviour )
         {
            case GAN_EDGE_BEHAVIOUR_CLIP:
            {
               source += half_size*sstride;
               for ( i = dsize-1; i >= 0;
                     i--, dest += dstride, source += sstride )
               {
                  total = (mask->data.f[0]*(float)source[0]);
                  for ( j = (int)half_size; j > 0; j-- )
                     total += mask->data.f[j]*((float)source[j*sstride] +
                                               (float)source[-j*sstride]);

                  *dest = (unsigned short) (total+0.5F);
               }
            }
            break;

            case GAN_EDGE_BEHAVIOUR_REPEAT:
            {
               for ( i = dsize-1; i >= dsize-half_size; i-- )
               {
                  total = mask->data.f[0]*(float)source[i*sstride];
                  for ( j = half_size; j > 0; j-- )
                     total += mask->data.f[j]
                     *((float)source[SHIFT_DOWN_REPEAT(i+j,dsize)*sstride] +
                       (float)source[(i-j)*sstride]);

                  dest[i*dstride] = (unsigned short) (total+0.5F);
               }

               for ( ; i >= half_size; i-- )
               {
                  total = mask->data.f[0]*(float)source[i*sstride];
                  for ( j = half_size; j > 0; j-- )
                     total += mask->data.f[j]*((float)source[(i+j)*sstride] +
                                               (float)source[(i-j)*sstride]);

                  dest[i*dstride] = (unsigned short) (total+0.5F);
               }

               for ( ; i >= 0; i-- )
               {
                  total = mask->data.f[0]*(float)source[i*sstride];
                  for ( j = half_size; j > 0; j-- )
                     total += mask->data.f[j]
                     *((float)source[(i+j)*sstride] +
                       (float)source[SHIFT_UP_REPEAT((i-j),dsize)*sstride]);

                  dest[i*dstride] = (unsigned short) (total+0.5F);
               }
            }
            break;

            case GAN_EDGE_BEHAVIOUR_CIRCULAR:
            {
               for ( i = dsize-1; i >= 0; i-- )
               {
                  total = mask->data.f[0]*(float)source[i*sstride];
                  for ( j = half_size; j > 0; j-- )
                     total += mask->data.f[j]
                              *((float)source[SHIFT_DOWN_CIRCULAR(i+j,dsize)*sstride]+
                                (float)source[SHIFT_UP_CIRCULAR(i-j,dsize)*sstride]);

                  dest[i*dstride] = (unsigned short) (total+0.5F);
               }
            }
            break;

            case GAN_EDGE_BEHAVIOUR_EXPAND:
            {
               if(zero_outside)
                  for ( i = dsize-1; i >= dsize-2*half_size; i-- )
                  {
                     total = 0.0F;
                     for ( j = 0; j <= half_size; j++ )
                     {
                        if(i+j >= dsize-half_size)
                           break;

                        total += mask->data.f[j]*(float)source[(i-half_size+j)*sstride];
                     }

                     for ( j = half_size; j > 0; j-- )
                     {
                        if(i-j >= dsize-half_size)
                           break;

                        total += mask->data.f[j]*(float)source[(i-half_size-j)*sstride];
                     }

                     dest[i*dstride] = (unsigned short) (total+0.5F);
                  }
               else
                  for ( i = dsize-1; i >= dsize-2*half_size; i-- )
                  {
                     total = 0.0F;
                     for ( j = 0; j <= half_size; j++ )
                        if(i+j >= dsize-half_size)
                           total += mask->data.f[j]*(float)source[(dsize-2*half_size-1)*sstride];
                        else
                           total += mask->data.f[j]*(float)source[(i-half_size+j)*sstride];

                     for ( j = half_size; j > 0; j-- )
                        if(i-j >= dsize-half_size)
                           total += mask->data.f[j]*(float)source[(dsize-2*half_size-1)*sstride];
                        else
                           total += mask->data.f[j]*(float)source[(i-half_size-j)*sstride];

                     dest[i*dstride] = (unsigned short) (total+0.5F);
                  }

               for ( ; i >= 2*half_size; i-- )
               {
                  total = mask->data.f[0]*(float)source[(i-half_size)*sstride];
                  for ( j = half_size; j > 0; j-- )
                     total += mask->data.f[j]*(source[(i-half_size+j)*sstride] +
                                               source[(i-half_size-j)*sstride]);

                  dest[i*dstride] = (unsigned short) (total+0.5F);
               }

               if(zero_outside)
                  for ( ; i >= 0; i-- )
                  {
                     total = 0.0F;
                     for ( j = half_size; j >= 0; j-- )
                     {
                        if(i+j < half_size)
                           break;

                        total += mask->data.f[j]*(float)source[(i-half_size+j)*sstride];
                     }

                     for ( j = 1; j <= half_size; j++ )
                     {
                        if(i-j < half_size)
                           break;

                        total += mask->data.f[j]*(float)source[(i-half_size-j)*sstride];
                     }

                     dest[i*dstride] = (unsigned short) (total+0.5F);
                  }
               else
                  for ( ; i >= 0; i-- )
                  {
                     total = 0.0F;
                     for ( j = half_size; j >= 0; j-- )
                        if(i+j < half_size)
                           total += mask->data.f[j]*(float)source[0];
                        else
                           total += mask->data.f[j]*(float)source[(i-half_size+j)*sstride];

                     for ( j = 1; j <= half_size; j++ )
                        if(i-j < half_size)
                           total += mask->data.f[j]*(float)source[0];
                        else
                           total += mask->data.f[j]*(float)source[(i-half_size-j)*sstride];

                     dest[i*dstride] = (unsigned short) (total+0.5F);
                  }
            }
            break;

            default:
              gan_err_flush_trace();
              gan_err_register ( "gan_convolve1D_us", GAN_ERROR_ILLEGAL_TYPE,
                                 "" );
              break;
         }
      }
      break;

      case GAN_MASK1D_ANTISYMMETRIC:
      {
         int half_size = (int)mask->size/2;

         switch ( behaviour )
         {
            case GAN_EDGE_BEHAVIOUR_CLIP:
            {
               source += half_size*sstride;
               for ( i = dsize-1; i >= 0;
                     i--, dest += dstride, source += sstride )
               {
                  total = 0.0F;
                  for ( j = half_size; j > 0; j-- )
                     total += mask->data.f[j-1]
                     *((float)source[j*sstride] - (float)source[-j*sstride]);

                  *dest = (unsigned short) (total+0.5F);
               }
            }
            break;

            case GAN_EDGE_BEHAVIOUR_REPEAT:
            {
               for ( i = dsize-1; i >= 0; i-- )
               {
                  total = 0.0F;
                  for ( j = half_size; j > 0; j-- )
                     total += mask->data.f[j-1]
                        *((float)source[SHIFT_DOWN_REPEAT(i+j,dsize)*sstride] +
                          (float)source[SHIFT_UP_REPEAT(i-j,dsize)*sstride]);

                  dest[i*dstride] = (unsigned short) (total+0.5F);
               }
            }
            break;
            
            case GAN_EDGE_BEHAVIOUR_CIRCULAR:
            {
               for ( i = dsize-1; i >= 0; i-- )
               {
                  total = 0.0F;
                  for ( j = half_size; j > 0; j-- )
                     total += mask->data.f[j-1]
                        *((float)source[SHIFT_DOWN_CIRCULAR(i+j,dsize)*sstride] +
                          (float)source[SHIFT_UP_CIRCULAR(i-j,dsize)*sstride]);

                  dest[i*dstride] = (unsigned short) (total+0.5F);
               }
            }
            break;

            default:
              gan_err_flush_trace();
              gan_err_register ( "gan_convolve1D_us", GAN_ERROR_ILLEGAL_TYPE,
                                 "" );
              break;
         }
      }
      break;

      case GAN_MASK1D_GENERIC:
      {
         switch ( behaviour )
         {
            case GAN_EDGE_BEHAVIOUR_CLIP:
            {
               source += (mask->size-1)*sstride;
               for ( i = dsize-1; i >= 0;
                     i--, dest += dstride, source += sstride )
               {
                  total = 0.0F;
                  for ( j = (int)mask->size-1; j >= 0; j-- )
                     total += mask->data.f[j]*(float)source[-j*sstride];

                  *dest = (unsigned short) (total+0.5F);
               }
            }
            break;

            case GAN_EDGE_BEHAVIOUR_REPEAT:
            {
               int half_size = (int)mask->size/2;
               float *mdata = mask->data.f + half_size;

               for ( i = dsize-1; i >= 0; i-- )
               {
                  total = 0.0F;
                  for ( j = -half_size; j <= half_size; j++ )
                     total += mdata[j]*(float)source[SHIFT_IN_REPEAT(i-j,dsize)*sstride];

                  dest[i*dstride] = (unsigned short) (total+0.5F);
               }
            }
            break;

            case GAN_EDGE_BEHAVIOUR_CIRCULAR:
            {
               int half_size = (int)mask->size/2;
               float *mdata = mask->data.f + half_size;

               for ( i = dsize-1; i >= 0; i-- )
               {
                  total = 0.0F;
                  for ( j = -half_size; j <= half_size; j++ )
                     total += mdata[j]*(float)source[SHIFT_IN_CIRCULAR(i-j,dsize)*sstride];

                  dest[i*dstride] = (unsigned short) (total+0.5F);
               }
            }
            break;

            default:
              gan_err_flush_trace();
              gan_err_register ( "gan_convolve1D_us", GAN_ERROR_ILLEGAL_TYPE,
                                 "" );
              break;
         }
      }
      break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_convolve1D_us", GAN_ERROR_ILLEGAL_TYPE, "" );
        break;
   }

   /* success */
   return GAN_TRUE;
}

/**
 * \brief 1D convolution function for unsigned integer arrays.
 * \param source The input array to be convolved
 * \param sstride The stride of the source array
 * \param behaviour Behaviour of data beyond edge
 * \param zero_outside Whether to assume zeros outside the source data
 * \param dest The destination array for the convolution
 * \param dstride The stride of the dest array
 * \param mask The convolution mask
 * \param dsize The number of output elements to compute
 *
 * Applies a one-dimensional convolution operation to the given \a source
 * array. Both arrays must be of type \c unsigned \c int.
 * There is no checking for overflow.
 *
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 * \sa gan_convolve1D_uc().
 */
Gan_Bool
 gan_convolve1D_ui ( unsigned int *source, int sstride,
                     Gan_EdgeBehaviour behaviour, Gan_Bool zero_outside,
                     unsigned int *dest,   int dstride,
                     Gan_Mask1D *mask, int dsize )
{
   int i, j;
   double total;

   gan_err_test_bool ( behaviour == GAN_EDGE_BEHAVIOUR_CLIP,
                       "gan_convolve1D_ui", GAN_ERROR_INCOMPATIBLE, "" );
   gan_err_test_bool ( mask->type == GAN_FLOAT, "gan_convolve1D_ui",
                       GAN_ERROR_INCOMPATIBLE, "" );
   switch ( mask->format )
   {
      case GAN_MASK1D_SYMMETRIC:
      {
         unsigned half_size = mask->size/2;
         
         source += half_size*sstride;
         for ( i = dsize-1; i >= 0;
               i--, dest += dstride, source += sstride )
         {
            total = ((double)mask->data.f[0]*(double)source[0]);
            for ( j = (int)half_size; j > 0; j-- )
               total += (double)mask->data.f[j]*((double)source[j*sstride] +
                                                 (double)source[-j*sstride]);

            *dest = (unsigned int) (total+0.5);
         }
      }
      break;

      case GAN_MASK1D_ANTISYMMETRIC:
      {
         unsigned half_size = mask->size/2;

         source += half_size*sstride;
         for ( i = dsize-1; i >= 0;
               i--, dest += dstride, source += sstride )
         {
            total = 0.0;
            for ( j = (int)half_size; j > 0; j-- )
               total += (double)mask->data.f[j-1]*((double)source[j*sstride] -
                                                   (double)source[-j*sstride]);

            *dest = (unsigned int) (total+0.5);
         }
      }
      break;
            
      case GAN_MASK1D_GENERIC:
      {
         source += mask->size*sstride;
         for ( i = dsize-1; i >= 0;
               i--, dest += dstride, source += sstride )
         {
            total = 0.0;
            for ( j = (int)mask->size-1; j >= 0; j-- )
               total +=  ((double)mask->data.f[j]*(double)source[-j*sstride]);

            *dest = (unsigned int) (total+0.5);
         }
      }
      break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_convolve1D_ui", GAN_ERROR_ILLEGAL_TYPE, "" );
        break;
   }

   /* success */
   return GAN_TRUE;
}

static Gan_Image *
 image_convolve1Dx_gl_gl ( const Gan_Image *image, Gan_EdgeBehaviour eEdgeBehaviour,
                           Gan_Mask1D *mask, Gan_Image *dest )
{
   int i;
   unsigned int dwidth;

   gan_err_test_ptr ( image->format == GAN_GREY_LEVEL_IMAGE,
                      "image_convolve1Dx_gl_gl", GAN_ERROR_INCOMPATIBLE, "" );
   gan_err_test_ptr ( image->width >= mask->size, "image_convolve1Dx_gl_gl",
                      GAN_ERROR_IMAGE_TOO_SMALL, "");

   if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_CLIP )
      dwidth = image->width-mask->size+1;
   else if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_EXPAND )
      dwidth = image->width+mask->size-1;
   else
      dwidth = image->width;

   if ( dest == NULL )
      dest = gan_image_alloc ( GAN_GREY_LEVEL_IMAGE, image->type,
                               image->height, dwidth );
   else if (    dest->format != GAN_GREY_LEVEL_IMAGE
             || dest->type   != image->type
             || dest->width  != dwidth
             || dest->height != image->height )
      dest = gan_image_set_format_type_dims ( dest,
                                              GAN_GREY_LEVEL_IMAGE,
                                              image->type,
                                              image->height,
                                              dwidth );

   if ( dest == NULL )
   {
      gan_err_register ( "image_convolve1Dx_gl_gl", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_CLIP )
      dest->offset_x = image->offset_x+(int)mask->size/2;
   else if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_EXPAND )
      dest->offset_x = image->offset_x-(int)mask->size/2;
   else
      dest->offset_x = image->offset_x;

   dest->offset_y = image->offset_y;

   switch ( image->type )
   {
      case GAN_FLOAT:
        for ( i = (int)dest->height-1; i >= 0; i-- )
           if ( !gan_convolve1D_f ( gan_image_get_pixptr_gl_f(image,i,0), 1,
                                    eEdgeBehaviour, GAN_FALSE,
                                    gan_image_get_pixptr_gl_f(dest,i,0),  1,
                                    mask, (int)dwidth ) )
           {
              gan_err_register ( "image_convolve1Dx_gl_gl", GAN_ERROR_FAILURE,
                                 "" );
              return NULL;
           }
                        
        break;

      case GAN_DOUBLE:
        for ( i = (int)dest->height-1; i >= 0; i-- )
           if ( !gan_convolve1D_d ( gan_image_get_pixptr_gl_d(image,i,0), 1,
                                    eEdgeBehaviour, GAN_FALSE,
                                    gan_image_get_pixptr_gl_d(dest,i,0),  1,
                                    mask, (int)dwidth ) )
           {
              gan_err_register ( "image_convolve1Dx_gl_gl", GAN_ERROR_FAILURE,
                                 "" );
              return NULL;
           }
                        
        break;

      case GAN_INT:
        for ( i = (int)dest->height-1; i >= 0; i-- )
           if ( !gan_convolve1D_i ( gan_image_get_pixptr_gl_i(image,i,0), 1,
                                    eEdgeBehaviour, GAN_FALSE,
                                    gan_image_get_pixptr_gl_i(dest,i,0),  1,
                                    mask, (int)dwidth ) )
           {
              gan_err_register ( "image_convolve1Dx_gl_gl", GAN_ERROR_FAILURE,
                                 "" );
              return NULL;
           }

        break;

      case GAN_UINT:
        for ( i = (int)dest->height-1; i >= 0; i-- )
           if ( !gan_convolve1D_ui ( gan_image_get_pixptr_gl_ui(image,i,0), 1,
                                     eEdgeBehaviour, GAN_FALSE,
                                     gan_image_get_pixptr_gl_ui(dest,i,0),  1,
                                     mask, (int)dwidth ) )
           {
              gan_err_register ( "image_convolve1Dx_gl_gl", GAN_ERROR_FAILURE,
                                 "" );
              return NULL;
           }

        break;

      case GAN_UCHAR:
        for ( i = (int)dest->height-1; i >= 0; i-- )
           if ( !gan_convolve1D_uc ( gan_image_get_pixptr_gl_uc(image,i,0), 1,
                                     eEdgeBehaviour, GAN_FALSE,
                                     gan_image_get_pixptr_gl_uc(dest,i,0),  1,
                                     mask, (int)dwidth ) )
           {
              gan_err_register ( "image_convolve1Dx_gl_gl", GAN_ERROR_FAILURE,
                                 "" );
              return NULL;
           }

        break;

      case GAN_USHORT:
        for ( i = (int)dest->height-1; i >= 0; i-- )
           if ( !gan_convolve1D_us ( gan_image_get_pixptr_gl_us(image,i,0), 1,
                                     eEdgeBehaviour, GAN_FALSE,
                                     gan_image_get_pixptr_gl_us(dest,i,0),  1,
                                     mask, (int)dwidth ) )
           {
              gan_err_register ( "image_convolve1Dx_gl_gl", GAN_ERROR_FAILURE,
                                 "" );
              return NULL;
           }

        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "image_convolve1Dx_gl_gl", GAN_ERROR_ILLEGAL_IMAGE_TYPE,
                           "" );
        return NULL;
   }

   /* success */
   return dest;
}


static Gan_Image *
 image_convolve1Dx_v3D_gl ( const Gan_Image *image, Gan_ImageChannelType channel,
                            Gan_EdgeBehaviour eEdgeBehaviour,
                            Gan_Mask1D *mask, Gan_Image *dest )
{
   int i;
   unsigned int dwidth;

   gan_err_test_ptr ( image->format == GAN_VECTOR_FIELD_3D,
                      "image_convolve1Dx_v3D_gl", GAN_ERROR_INCOMPATIBLE, "" );
   gan_err_test_ptr ( image->width >= mask->size, "image_convolve1Dx_v3D_gl",
                      GAN_ERROR_IMAGE_TOO_SMALL, "" );

   if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_CLIP )
      dwidth = image->width-mask->size+1;
   else if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_EXPAND )
      dwidth = image->width+mask->size-1;
   else
      dwidth = image->width;

   if ( dest == NULL )
      dest = gan_image_alloc ( GAN_GREY_LEVEL_IMAGE, image->type,
                               image->height, dwidth );
   else if (    dest->format != GAN_GREY_LEVEL_IMAGE
             || dest->type   != image->type
             || dest->width  != dwidth
             || dest->height != image->height )
      dest = gan_image_set_format_type_dims ( dest,
                                              GAN_GREY_LEVEL_IMAGE,
                                              image->type,
                                              image->height,
                                              dwidth );

   if ( dest == NULL )
   {
      gan_err_register ( "image_convolve1Dx_v3D_gl", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_CLIP )
      dest->offset_x = image->offset_x+(int)mask->size/2;
   else if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_EXPAND )
      dest->offset_x = image->offset_x-(int)mask->size/2;
   else
      dest->offset_x = image->offset_x;

   dest->offset_y = image->offset_y;

   switch ( image->type )
   {
      case GAN_INT:
        switch ( channel )
        {
           case GAN_X_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_i (
                         &gan_image_get_pixptr_vfield3D_i(image,i,0)->x, 3,
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_i(dest,i,0),            1,
                         mask, (int)dwidth ) )
                   break;
 
             break;

           case GAN_Y_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_i (
                         &gan_image_get_pixptr_vfield3D_i(image,i,0)->y, 3,
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_i(dest,i,0),            1,
                         mask, (int)dwidth ) )
                   break;

             break;

           case GAN_Z_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_i (
                         &gan_image_get_pixptr_vfield3D_i(image,i,0)->z, 3,
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_i(dest,i,0),            1,
                         mask, (int)dwidth ) )
                   break;

             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "image_convolve1Dx_v3D_gl",
                                GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;

      case GAN_FLOAT:
        switch ( channel )
        {
           case GAN_X_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_f (
                         &gan_image_get_pixptr_vfield3D_f(image,i,0)->x, 3,
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_f(dest,i,0),            1,
                         mask, (int)dwidth ) )
                   break;

             break;

           case GAN_Y_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_f (
                         &gan_image_get_pixptr_vfield3D_f(image,i,0)->y, 3,
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_f(dest,i,0),            1,
                         mask, (int)dwidth ) )
                   break;

             break;

           case GAN_Z_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_f (
                         &gan_image_get_pixptr_vfield3D_f(image,i,0)->z, 3,
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_f(dest,i,0),            1,
                         mask, (int)dwidth ) )
                   break;

             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "image_convolve1Dx_v3D_gl",
                                GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;

      case GAN_DOUBLE:
        switch ( channel )
        {
           case GAN_X_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_d (
                         &gan_image_get_pixptr_vfield3D_d(image,i,0)->x, 3,
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_d(dest,i,0),            1,
                         mask, (int)dwidth ) )
                   break;

             break;

           case GAN_Y_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_d (
                         &gan_image_get_pixptr_vfield3D_d(image,i,0)->y, 3,
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_d(dest,i,0),            1,
                         mask, (int)dwidth ) )
                   break;

             break;

           case GAN_Z_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_d (
                         &gan_image_get_pixptr_vfield3D_d(image,i,0)->z, 3,
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_d(dest,i,0),            1,
                         mask, (int)dwidth ) )
                   break;

             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "image_convolve1Dx_v3D_gl",
                                GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "image_convolve1Dx_v3D_gl",
                           GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
        return NULL;
   }

   /* check if we aborted the loop early because of an error */
   if ( i >= 0 )
   {
      gan_err_register ( "image_convolve1Dx_v3D_gl", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* success */
   return dest;
}

static Gan_Image *
 image_convolve1Dx_rgb_gl ( const Gan_Image *image, Gan_ImageChannelType channel,
                            Gan_EdgeBehaviour eEdgeBehaviour,
                            Gan_Mask1D *mask, Gan_Image *dest )
{
   int i;
   unsigned int dwidth;

   gan_err_test_ptr ( image->format == GAN_RGB_COLOUR_IMAGE,
                      "image_convolve1Dx_rgb_gl", GAN_ERROR_INCOMPATIBLE, "" );
   gan_err_test_ptr ( image->width >= mask->size, "image_convolve1Dx_rgb_gl",
                      GAN_ERROR_IMAGE_TOO_SMALL, "" );

   if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_CLIP )
      dwidth = image->width-mask->size+1;
   else if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_EXPAND )
      dwidth = image->width+mask->size-1;
   else
      dwidth = image->width;

   if ( dest == NULL )
      dest = gan_image_alloc ( GAN_GREY_LEVEL_IMAGE, image->type,
                               image->height, dwidth );
   else if (    dest->format != GAN_GREY_LEVEL_IMAGE
             || dest->type   != image->type
             || dest->width  != dwidth
             || dest->height != image->height )
      dest = gan_image_set_format_type_dims ( dest,
                                              GAN_GREY_LEVEL_IMAGE,
                                              image->type,
                                              image->height,
                                              dwidth );

   if ( dest == NULL )
   {
      gan_err_register ( "image_convolve1Dx_rgb_gl", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_CLIP )
      dest->offset_x = image->offset_x+(int)mask->size/2;
   else if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_EXPAND )
      dest->offset_x = image->offset_x-(int)mask->size/2;
   else
      dest->offset_x = image->offset_x;

   dest->offset_y = image->offset_y;

   switch ( image->type )
   {
      case GAN_INT:
        switch ( channel )
        {
           case GAN_RED_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_i (
                         &gan_image_get_pixptr_rgb_i(image,i,0)->R, 3,
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_i(dest,i,0),       1,
                         mask, (int)dwidth ) )
                   break;
 
             break;

           case GAN_GREEN_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_i (
                         &gan_image_get_pixptr_rgb_i(image,i,0)->G, 3,
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_i(dest,i,0),       1,
                         mask, (int)dwidth ) )
                   break;

             break;

           case GAN_BLUE_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_i (
                         &gan_image_get_pixptr_rgb_i(image,i,0)->B, 3,
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_i(dest,i,0),       1,
                         mask, (int)dwidth ) )
                   break;

             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "image_convolve1Dx_rgb_gl",
                                GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;

      case GAN_UINT:
        switch ( channel )
        {
           case GAN_RED_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_ui (
                         &gan_image_get_pixptr_rgb_ui(image,i,0)->R, 3,
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_ui(dest,i,0),       1,
                         mask, (int)dwidth ) )
                   break;
 
             break;

           case GAN_GREEN_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_ui (
                         &gan_image_get_pixptr_rgb_ui(image,i,0)->G, 3,
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_ui(dest,i,0),       1,
                         mask, (int)dwidth ) )
                   break;

             break;

           case GAN_BLUE_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_ui (
                         &gan_image_get_pixptr_rgb_ui(image,i,0)->B, 3,
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_ui(dest,i,0),       1,
                         mask, (int)dwidth ) )
                   break;

             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "image_convolve1Dx_rgb_gl",
                                GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;
      
      case GAN_FLOAT:
        switch ( channel )
        {
           case GAN_RED_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_f (
                         &gan_image_get_pixptr_rgb_f(image,i,0)->R, 3,
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_f(dest,i,0),       1,
                         mask, (int)dwidth ) )
                   break;

             break;

           case GAN_GREEN_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_f (
                         &gan_image_get_pixptr_rgb_f(image,i,0)->G, 3,
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_f(dest,i,0),       1,
                         mask, (int)dwidth ) )
                   break;

             break;

           case GAN_BLUE_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_f (
                         &gan_image_get_pixptr_rgb_f(image,i,0)->B, 3,
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_f(dest,i,0),       1,
                         mask, (int)dwidth ) )
                   break;

             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "image_convolve1Dx_rgb_gl",
                                GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;

      case GAN_DOUBLE:
        switch ( channel )
        {
           case GAN_RED_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_d (
                         &gan_image_get_pixptr_rgb_d(image,i,0)->R, 3,
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_d(dest,i,0),       1,
                         mask, (int)dwidth ) )
                   break;

             break;

           case GAN_GREEN_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_d (
                         &gan_image_get_pixptr_rgb_d(image,i,0)->G, 3,
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_d(dest,i,0),       1,
                         mask, (int)dwidth ) )
                   break;

             break;

           case GAN_BLUE_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_d (
                         &gan_image_get_pixptr_rgb_d(image,i,0)->B, 3,
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_d(dest,i,0),       1,
                         mask, (int)dwidth ) )
                   break;

             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "image_convolve1Dx_rgb_gl",
                                GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;

      case GAN_UCHAR:
        switch ( channel )
        {
           case GAN_RED_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_uc (
                         &gan_image_get_pixptr_rgb_uc(image,i,0)->R, 3,
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_uc(dest,i,0),       1,
                         mask, (int)dwidth ) )
                   break;
 
             break;

           case GAN_GREEN_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_uc (
                         &gan_image_get_pixptr_rgb_uc(image,i,0)->G, 3,
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_uc(dest,i,0),       1,
                         mask, (int)dwidth ) )
                   break;

             break;

           case GAN_BLUE_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_uc (
                         &gan_image_get_pixptr_rgb_uc(image,i,0)->B, 3,
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_uc(dest,i,0),       1,
                         mask, (int)dwidth ) )
                   break;

             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "image_convolve1Dx_rgb_gl",
                                GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;

      case GAN_USHORT:
        switch ( channel )
        {
           case GAN_RED_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_us (
                         &gan_image_get_pixptr_rgb_us(image,i,0)->R, 3,
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_us(dest,i,0),       1,
                         mask, (int)dwidth ) )
                   break;
 
             break;

           case GAN_GREEN_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_us (
                         &gan_image_get_pixptr_rgb_us(image,i,0)->G, 3,
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_us(dest,i,0),       1,
                         mask, (int)dwidth ) )
                   break;

             break;

           case GAN_BLUE_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_us (
                         &gan_image_get_pixptr_rgb_us(image,i,0)->B, 3,
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_us(dest,i,0),       1,
                         mask, (int)dwidth ) )
                   break;

             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "image_convolve1Dx_rgb_gl",
                                GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;
      
      default:
        gan_err_flush_trace();
        gan_err_register ( "image_convolve1Dx_rgb_gl", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
        return NULL;
   }

   /* check if we aborted the loop early because of an error */
   if ( i >= 0 )
   {
      gan_err_register ( "image_convolve1Dx_rgb_gl", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* success */
   return dest;
}


static Gan_Image *
 image_convolve1Dx_rgba_gl ( const Gan_Image *image, Gan_ImageChannelType channel,
                             Gan_EdgeBehaviour eEdgeBehaviour,
                             Gan_Mask1D *mask, Gan_Image *dest )
{
   int i;
   unsigned int dwidth;

   gan_err_test_ptr ( image->format == GAN_RGB_COLOUR_ALPHA_IMAGE,
                      "image_convolve1Dx_rgba_gl", GAN_ERROR_INCOMPATIBLE, "");
   gan_err_test_ptr ( image->width >= mask->size, "image_convolve1Dx_rgba_gl",
                      GAN_ERROR_IMAGE_TOO_SMALL, "");

   if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_CLIP )
      dwidth = image->width-mask->size+1;
   else if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_EXPAND )
      dwidth = image->width+mask->size-1;
   else
      dwidth = image->width;

   if ( dest == NULL )
      dest = gan_image_alloc ( GAN_GREY_LEVEL_IMAGE, image->type,
                               image->height, dwidth );
   else if (    dest->format != GAN_GREY_LEVEL_IMAGE
             || dest->type   != image->type
             || dest->width  != dwidth
             || dest->height != image->height )
      dest = gan_image_set_format_type_dims ( dest,
                                              GAN_GREY_LEVEL_IMAGE,
                                              image->type,
                                              image->height,
                                              dwidth );

   if ( dest == NULL )
   {
      gan_err_register ( "image_convolve1Dx_rgba_gl", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_CLIP )
      dest->offset_x = image->offset_x+(int)mask->size/2;
   else if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_EXPAND )
      dest->offset_x = image->offset_x-(int)mask->size/2;
   else
      dest->offset_x = image->offset_x;

   dest->offset_y = image->offset_y;

   switch ( image->type )
   {
      case GAN_UINT:
        switch ( channel )
        {
           case GAN_RED_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_ui (
                         &gan_image_get_pixptr_rgba_ui(image,i,0)->R, 4,
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_ui(dest,i,0),        1,
                         mask, (int)dwidth ) )
                   break;
 
             break;

           case GAN_GREEN_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_ui (
                         &gan_image_get_pixptr_rgba_ui(image,i,0)->G, 4,
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_ui(dest,i,0),        1,
                         mask, (int)dwidth ) )
                   break;

             break;

           case GAN_BLUE_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_ui (
                         &gan_image_get_pixptr_rgba_ui(image,i,0)->B, 4,
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_ui(dest,i,0),        1,
                         mask, (int)dwidth ) )
                   break;

             break;

           case GAN_ALPHA_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_ui (
                         &gan_image_get_pixptr_rgba_ui(image,i,0)->A, 4,
                         eEdgeBehaviour, GAN_TRUE,
                         gan_image_get_pixptr_gl_ui(dest,i,0),        1,
                         mask, (int)dwidth ) )
                   break;

             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "image_convolve1Dx_rgb_gl",
                                GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;

      case GAN_INT:
        switch ( channel )
        {
           case GAN_RED_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_i (
                         &gan_image_get_pixptr_rgba_i(image,i,0)->R, 4,
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_i(dest,i,0),        1,
                         mask, (int)dwidth ) )
                   break;
 
             break;

           case GAN_GREEN_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_i (
                         &gan_image_get_pixptr_rgba_i(image,i,0)->G, 4,
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_i(dest,i,0),        1,
                         mask, (int)dwidth ) )
                   break;

             break;

           case GAN_BLUE_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_i (
                         &gan_image_get_pixptr_rgba_i(image,i,0)->B, 4,
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_i(dest,i,0),        1,
                         mask, (int)dwidth ) )
                   break;

             break;

           case GAN_ALPHA_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_i (
                         &gan_image_get_pixptr_rgba_i(image,i,0)->A, 4,
                         eEdgeBehaviour, GAN_TRUE,
                         gan_image_get_pixptr_gl_i(dest,i,0),        1,
                         mask, (int)dwidth ) )
                   break;

             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "image_convolve1Dx_rgb_gl",
                                GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;

      case GAN_FLOAT:
        switch ( channel )
        {
           case GAN_RED_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_f (
                         &gan_image_get_pixptr_rgba_f(image,i,0)->R, 4,
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_f(dest,i,0),        1,
                         mask, (int)dwidth ) )
                   break;

             break;

           case GAN_GREEN_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_f (
                         &gan_image_get_pixptr_rgba_f(image,i,0)->G, 4,
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_f(dest,i,0),        1,
                         mask, (int)dwidth ) )
                   break;

             break;

           case GAN_BLUE_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_f (
                         &gan_image_get_pixptr_rgba_f(image,i,0)->B, 4,
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_f(dest,i,0),        1,
                         mask, (int)dwidth ) )
                   break;

             break;

           case GAN_ALPHA_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_f (
                         &gan_image_get_pixptr_rgba_f(image,i,0)->A, 4,
                         eEdgeBehaviour, GAN_TRUE,
                         gan_image_get_pixptr_gl_f(dest,i,0),        1,
                         mask, (int)dwidth ) )
                   break;

             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "image_convolve1Dx_rgba_gl",
                                GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;

      case GAN_DOUBLE:
        switch ( channel )
        {
           case GAN_RED_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_d (
                         &gan_image_get_pixptr_rgba_d(image,i,0)->R, 4,
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_d(dest,i,0),        1,
                         mask, (int)dwidth ) )
                   break;

             break;

           case GAN_GREEN_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_d (
                         &gan_image_get_pixptr_rgba_d(image,i,0)->G, 4,
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_d(dest,i,0),        1,
                         mask, (int)dwidth ) )
                   break;

             break;

           case GAN_BLUE_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_d (
                         &gan_image_get_pixptr_rgba_d(image,i,0)->B, 4,
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_d(dest,i,0),        1,
                         mask, (int)dwidth ) )
                   break;

             break;

           case GAN_ALPHA_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_d (
                         &gan_image_get_pixptr_rgba_d(image,i,0)->A, 4,
                         eEdgeBehaviour, GAN_TRUE,
                         gan_image_get_pixptr_gl_d(dest,i,0),        1,
                         mask, (int)dwidth ) )
                   break;

             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "image_convolve1Dx_rgba_gl",
                                GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;

      case GAN_UCHAR:
        switch ( channel )
        {
           case GAN_RED_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_uc (
                         &gan_image_get_pixptr_rgba_uc(image,i,0)->R, 4,
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_uc(dest,i,0),        1,
                         mask, (int)dwidth ) )
                   break;
 
             break;

           case GAN_GREEN_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_uc (
                         &gan_image_get_pixptr_rgba_uc(image,i,0)->G, 4,
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_uc(dest,i,0),        1,
                         mask, (int)dwidth ) )
                   break;

             break;

           case GAN_BLUE_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_uc (
                         &gan_image_get_pixptr_rgba_uc(image,i,0)->B, 4,
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_uc(dest,i,0),        1,
                         mask, (int)dwidth ) )
                   break;

             break;

           case GAN_ALPHA_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_uc (
                         &gan_image_get_pixptr_rgba_uc(image,i,0)->A, 4,
                         eEdgeBehaviour, GAN_TRUE,
                         gan_image_get_pixptr_gl_uc(dest,i,0),        1,
                         mask, (int)dwidth ) )
                   break;

             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "image_convolve1Dx_rgba_gl",
                                GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;

      case GAN_USHORT:
        switch ( channel )
        {
           case GAN_RED_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_us (
                         &gan_image_get_pixptr_rgba_us(image,i,0)->R, 4,
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_us(dest,i,0),        1,
                         mask, (int)dwidth ) )
                   break;
 
             break;

           case GAN_GREEN_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_us (
                         &gan_image_get_pixptr_rgba_us(image,i,0)->G, 4,
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_us(dest,i,0),        1,
                         mask, (int)dwidth ) )
                   break;

             break;

           case GAN_BLUE_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_us (
                         &gan_image_get_pixptr_rgba_us(image,i,0)->B, 4,
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_us(dest,i,0),        1,
                         mask, (int)dwidth ) )
                   break;

             break;

           case GAN_ALPHA_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_us (
                         &gan_image_get_pixptr_rgba_us(image,i,0)->A, 4,
                         eEdgeBehaviour, GAN_TRUE,
                         gan_image_get_pixptr_gl_us(dest,i,0),        1,
                         mask, (int)dwidth ) )
                   break;

             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "image_convolve1Dx_rgba_gl",
                                GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;

      
      default:
        gan_err_flush_trace();
        gan_err_register ( "image_convolve1Dx_rgba_gl",
                           GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
        return NULL;
   }

   /* check if we aborted the loop early because of an error */
   if ( i >= 0 )
   {
      gan_err_register ( "image_convolve1Dx_rgba_gl", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* success */
   return dest;
}
   
static Gan_Image *
 image_convolve1Dx_bgr_gl ( const Gan_Image *image, Gan_ImageChannelType channel,
                            Gan_EdgeBehaviour eEdgeBehaviour,
                            Gan_Mask1D *mask, Gan_Image *dest )
{
   int i;
   unsigned int dwidth;

   gan_err_test_ptr ( image->format == GAN_BGR_COLOUR_IMAGE,
                      "image_convolve1Dx_bgr_gl", GAN_ERROR_INCOMPATIBLE, "" );
   gan_err_test_ptr ( image->width >= mask->size, "image_convolve1Dx_bgr_gl",
                      GAN_ERROR_IMAGE_TOO_SMALL, "" );

   if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_CLIP )
      dwidth = image->width-mask->size+1;
   else if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_EXPAND )
      dwidth = image->width+mask->size-1;
   else
      dwidth = image->width;

   if ( dest == NULL )
      dest = gan_image_alloc ( GAN_GREY_LEVEL_IMAGE, image->type,
                               image->height, dwidth );
   else if (    dest->format != GAN_GREY_LEVEL_IMAGE
             || dest->type   != image->type
             || dest->width  != dwidth
             || dest->height != image->height )
      dest = gan_image_set_format_type_dims ( dest,
                                              GAN_GREY_LEVEL_IMAGE,
                                              image->type,
                                              image->height,
                                              dwidth );

   if ( dest == NULL )
   {
      gan_err_register ( "image_convolve1Dx_bgr_gl", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_CLIP )
      dest->offset_x = image->offset_x+(int)mask->size/2;
   else if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_EXPAND )
      dest->offset_x = image->offset_x-(int)mask->size/2;
   else
      dest->offset_x = image->offset_x;

   dest->offset_y = image->offset_y;

   switch ( image->type )
   {
      case GAN_INT:
        switch ( channel )
        {
           case GAN_RED_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_i (
                         &gan_image_get_pixptr_bgr_i(image,i,0)->R, 3,
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_i(dest,i,0),       1,
                         mask, (int)dwidth ) )
                   break;
 
             break;

           case GAN_GREEN_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_i (
                         &gan_image_get_pixptr_bgr_i(image,i,0)->G, 3,
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_i(dest,i,0),       1,
                         mask, (int)dwidth ) )
                   break;

             break;

           case GAN_BLUE_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_i (
                         &gan_image_get_pixptr_bgr_i(image,i,0)->B, 3,
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_i(dest,i,0),       1,
                         mask, (int)dwidth ) )
                   break;

             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "image_convolve1Dx_bgr_gl",
                                GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;

      case GAN_UINT:
        switch ( channel )
        {
           case GAN_RED_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_ui (
                         &gan_image_get_pixptr_bgr_ui(image,i,0)->R, 3,
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_ui(dest,i,0),       1,
                         mask, (int)dwidth ) )
                   break;
 
             break;

           case GAN_GREEN_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_ui (
                         &gan_image_get_pixptr_bgr_ui(image,i,0)->G, 3,
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_ui(dest,i,0),       1,
                         mask, (int)dwidth ) )
                   break;

             break;

           case GAN_BLUE_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_ui (
                         &gan_image_get_pixptr_bgr_ui(image,i,0)->B, 3,
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_ui(dest,i,0),       1,
                         mask, (int)dwidth ) )
                   break;

             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "image_convolve1Dx_bgr_gl",
                                GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;
      
      case GAN_FLOAT:
        switch ( channel )
        {
           case GAN_RED_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_f (
                         &gan_image_get_pixptr_bgr_f(image,i,0)->R, 3,
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_f(dest,i,0),       1,
                         mask, (int)dwidth ) )
                   break;

             break;

           case GAN_GREEN_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_f (
                         &gan_image_get_pixptr_bgr_f(image,i,0)->G, 3,
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_f(dest,i,0),       1,
                         mask, (int)dwidth ) )
                   break;

             break;

           case GAN_BLUE_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_f (
                         &gan_image_get_pixptr_bgr_f(image,i,0)->B, 3,
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_f(dest,i,0),       1,
                         mask, (int)dwidth ) )
                   break;

             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "image_convolve1Dx_bgr_gl",
                                GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;

      case GAN_DOUBLE:
        switch ( channel )
        {
           case GAN_RED_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_d (
                         &gan_image_get_pixptr_bgr_d(image,i,0)->R, 3,
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_d(dest,i,0),       1,
                         mask, (int)dwidth ) )
                   break;

             break;

           case GAN_GREEN_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_d (
                         &gan_image_get_pixptr_bgr_d(image,i,0)->G, 3,
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_d(dest,i,0),       1,
                         mask, (int)dwidth ) )
                   break;

             break;

           case GAN_BLUE_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_d (
                         &gan_image_get_pixptr_bgr_d(image,i,0)->B, 3,
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_d(dest,i,0),       1,
                         mask, (int)dwidth ) )
                   break;

             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "image_convolve1Dx_bgr_gl",
                                GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;

      case GAN_UCHAR:
        switch ( channel )
        {
           case GAN_RED_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_uc (
                         &gan_image_get_pixptr_bgr_uc(image,i,0)->R, 3,
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_uc(dest,i,0),       1,
                         mask, (int)dwidth ) )
                   break;
 
             break;

           case GAN_GREEN_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_uc (
                         &gan_image_get_pixptr_bgr_uc(image,i,0)->G, 3,
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_uc(dest,i,0),       1,
                         mask, (int)dwidth ) )
                   break;

             break;

           case GAN_BLUE_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_uc (
                         &gan_image_get_pixptr_bgr_uc(image,i,0)->B, 3,
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_uc(dest,i,0),       1,
                         mask, (int)dwidth ) )
                   break;

             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "image_convolve1Dx_bgr_gl",
                                GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;

      case GAN_USHORT:
        switch ( channel )
        {
           case GAN_RED_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_us (
                         &gan_image_get_pixptr_bgr_us(image,i,0)->R, 3,
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_us(dest,i,0),       1,
                         mask, (int)dwidth ) )
                   break;
 
             break;

           case GAN_GREEN_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_us (
                         &gan_image_get_pixptr_bgr_us(image,i,0)->G, 3,
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_us(dest,i,0),       1,
                         mask, (int)dwidth ) )
                   break;

             break;

           case GAN_BLUE_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_us (
                         &gan_image_get_pixptr_bgr_us(image,i,0)->B, 3,
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_us(dest,i,0),       1,
                         mask, (int)dwidth ) )
                   break;

             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "image_convolve1Dx_bgr_gl",
                                GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;
      
      default:
        gan_err_flush_trace();
        gan_err_register ( "image_convolve1Dx_bgr_gl", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
        return NULL;
   }

   /* check if we aborted the loop early because of an error */
   if ( i >= 0 )
   {
      gan_err_register ( "image_convolve1Dx_bgr_gl", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* success */
   return dest;
}


static Gan_Image *
 image_convolve1Dx_bgra_gl ( const Gan_Image *image, Gan_ImageChannelType channel,
                             Gan_EdgeBehaviour eEdgeBehaviour,
                             Gan_Mask1D *mask, Gan_Image *dest )
{
   int i;
   unsigned int dwidth;

   gan_err_test_ptr ( image->format == GAN_BGR_COLOUR_ALPHA_IMAGE,
                      "image_convolve1Dx_bgra_gl", GAN_ERROR_INCOMPATIBLE, "");
   gan_err_test_ptr ( image->width >= mask->size, "image_convolve1Dx_bgra_gl",
                      GAN_ERROR_IMAGE_TOO_SMALL, "");

   if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_CLIP )
      dwidth = image->width-mask->size+1;
   else if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_EXPAND )
      dwidth = image->width+mask->size-1;
   else
      dwidth = image->width;

   if ( dest == NULL )
      dest = gan_image_alloc ( GAN_GREY_LEVEL_IMAGE, image->type,
                               image->height, dwidth );
   else if (    dest->format != GAN_GREY_LEVEL_IMAGE
             || dest->type   != image->type
             || dest->width  != dwidth
             || dest->height != image->height )
      dest = gan_image_set_format_type_dims ( dest,
                                              GAN_GREY_LEVEL_IMAGE,
                                              image->type,
                                              image->height,
                                              dwidth );

   if ( dest == NULL )
   {
      gan_err_register ( "image_convolve1Dx_bgra_gl", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_CLIP )
      dest->offset_x = image->offset_x+(int)mask->size/2;
   else if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_EXPAND )
      dest->offset_x = image->offset_x-(int)mask->size/2;
   else
      dest->offset_x = image->offset_x;

   dest->offset_y = image->offset_y;

   switch ( image->type )
   {
      case GAN_UINT:
        switch ( channel )
        {
           case GAN_RED_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_ui (
                         &gan_image_get_pixptr_bgra_ui(image,i,0)->R, 4,
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_ui(dest,i,0),        1,
                         mask, (int)dwidth ) )
                   break;
 
             break;

           case GAN_GREEN_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_ui (
                         &gan_image_get_pixptr_bgra_ui(image,i,0)->G, 4,
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_ui(dest,i,0),        1,
                         mask, (int)dwidth ) )
                   break;

             break;

           case GAN_BLUE_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_ui (
                         &gan_image_get_pixptr_bgra_ui(image,i,0)->B, 4,
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_ui(dest,i,0),        1,
                         mask, (int)dwidth ) )
                   break;

             break;

           case GAN_ALPHA_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_ui (
                         &gan_image_get_pixptr_bgra_ui(image,i,0)->A, 4,
                         eEdgeBehaviour, GAN_TRUE,
                         gan_image_get_pixptr_gl_ui(dest,i,0),        1,
                         mask, (int)dwidth ) )
                   break;

             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "image_convolve1Dx_bgr_gl",
                                GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;

      case GAN_INT:
        switch ( channel )
        {
           case GAN_RED_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_i (
                         &gan_image_get_pixptr_bgra_i(image,i,0)->R, 4,
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_i(dest,i,0),        1,
                         mask, (int)dwidth ) )
                   break;
 
             break;

           case GAN_GREEN_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_i (
                         &gan_image_get_pixptr_bgra_i(image,i,0)->G, 4,
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_i(dest,i,0),        1,
                         mask, (int)dwidth ) )
                   break;

             break;

           case GAN_BLUE_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_i (
                         &gan_image_get_pixptr_bgra_i(image,i,0)->B, 4,
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_i(dest,i,0),        1,
                         mask, (int)dwidth ) )
                   break;

             break;

           case GAN_ALPHA_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_i (
                         &gan_image_get_pixptr_bgra_i(image,i,0)->A, 4,
                         eEdgeBehaviour, GAN_TRUE,
                         gan_image_get_pixptr_gl_i(dest,i,0),        1,
                         mask, (int)dwidth ) )
                   break;

             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "image_convolve1Dx_bgr_gl",
                                GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;

      case GAN_FLOAT:
        switch ( channel )
        {
           case GAN_RED_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_f (
                         &gan_image_get_pixptr_bgra_f(image,i,0)->R, 4,
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_f(dest,i,0),        1,
                         mask, (int)dwidth ) )
                   break;

             break;

           case GAN_GREEN_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_f (
                         &gan_image_get_pixptr_bgra_f(image,i,0)->G, 4,
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_f(dest,i,0),        1,
                         mask, (int)dwidth ) )
                   break;

             break;

           case GAN_BLUE_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_f (
                         &gan_image_get_pixptr_bgra_f(image,i,0)->B, 4,
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_f(dest,i,0),        1,
                         mask, (int)dwidth ) )
                   break;

             break;

           case GAN_ALPHA_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_f (
                         &gan_image_get_pixptr_bgra_f(image,i,0)->A, 4,
                         eEdgeBehaviour, GAN_TRUE,
                         gan_image_get_pixptr_gl_f(dest,i,0),        1,
                         mask, (int)dwidth ) )
                   break;

             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "image_convolve1Dx_bgra_gl",
                                GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;

      case GAN_DOUBLE:
        switch ( channel )
        {
           case GAN_RED_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_d (
                         &gan_image_get_pixptr_bgra_d(image,i,0)->R, 4,
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_d(dest,i,0),        1,
                         mask, (int)dwidth ) )
                   break;

             break;

           case GAN_GREEN_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_d (
                         &gan_image_get_pixptr_bgra_d(image,i,0)->G, 4,
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_d(dest,i,0),        1,
                         mask, (int)dwidth ) )
                   break;

             break;

           case GAN_BLUE_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_d (
                         &gan_image_get_pixptr_bgra_d(image,i,0)->B, 4,
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_d(dest,i,0),        1,
                         mask, (int)dwidth ) )
                   break;

             break;

           case GAN_ALPHA_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_d (
                         &gan_image_get_pixptr_bgra_d(image,i,0)->A, 4,
                         eEdgeBehaviour, GAN_TRUE,
                         gan_image_get_pixptr_gl_d(dest,i,0),        1,
                         mask, (int)dwidth ) )
                   break;

             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "image_convolve1Dx_bgra_gl",
                                GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;

      case GAN_UCHAR:
        switch ( channel )
        {
           case GAN_RED_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_uc (
                         &gan_image_get_pixptr_bgra_uc(image,i,0)->R, 4,
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_uc(dest,i,0),        1,
                         mask, (int)dwidth ) )
                   break;
 
             break;

           case GAN_GREEN_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_uc (
                         &gan_image_get_pixptr_bgra_uc(image,i,0)->G, 4,
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_uc(dest,i,0),        1,
                         mask, (int)dwidth ) )
                   break;

             break;

           case GAN_BLUE_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_uc (
                         &gan_image_get_pixptr_bgra_uc(image,i,0)->B, 4,
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_uc(dest,i,0),        1,
                         mask, (int)dwidth ) )
                   break;

             break;

           case GAN_ALPHA_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_uc (
                         &gan_image_get_pixptr_bgra_uc(image,i,0)->A, 4,
                         eEdgeBehaviour, GAN_TRUE,
                         gan_image_get_pixptr_gl_uc(dest,i,0),        1,
                         mask, (int)dwidth ) )
                   break;

             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "image_convolve1Dx_bgra_gl",
                                GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;

      case GAN_USHORT:
        switch ( channel )
        {
           case GAN_RED_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_us (
                         &gan_image_get_pixptr_bgra_us(image,i,0)->R, 4,
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_us(dest,i,0),        1,
                         mask, (int)dwidth ) )
                   break;
 
             break;

           case GAN_GREEN_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_us (
                         &gan_image_get_pixptr_bgra_us(image,i,0)->G, 4,
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_us(dest,i,0),        1,
                         mask, (int)dwidth ) )
                   break;

             break;

           case GAN_BLUE_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_us (
                         &gan_image_get_pixptr_bgra_us(image,i,0)->B, 4,
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_us(dest,i,0),        1,
                         mask, (int)dwidth ) )
                   break;

             break;

           case GAN_ALPHA_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_us (
                         &gan_image_get_pixptr_bgra_us(image,i,0)->A, 4,
                         eEdgeBehaviour, GAN_TRUE,
                         gan_image_get_pixptr_gl_us(dest,i,0),        1,
                         mask, (int)dwidth ) )
                   break;

             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "image_convolve1Dx_bgra_gl",
                                GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;

      
      default:
        gan_err_flush_trace();
        gan_err_register ( "image_convolve1Dx_bgra_gl",
                           GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
        return NULL;
   }

   /* check if we aborted the loop early because of an error */
   if ( i >= 0 )
   {
      gan_err_register ( "image_convolve1Dx_bgra_gl", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* success */
   return dest;
}
   
static Gan_Image *
 image_convolve1Dx_gl_v3D ( const Gan_Image *image, Gan_ImageChannelType channel,
                            Gan_EdgeBehaviour eEdgeBehaviour,
                            Gan_Mask1D *mask, Gan_Image *dest )
{
   int i;
   unsigned int dwidth;

   gan_err_test_ptr ( image->format == GAN_GREY_LEVEL_IMAGE,
                      "image_convolve1Dx_gl_v3D", GAN_ERROR_INCOMPATIBLE, "" );
   gan_err_test_ptr ( image->width >= mask->size, "image_convolve1Dx_gl_v3D",
                      GAN_ERROR_IMAGE_TOO_SMALL, "" );

   if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_CLIP )
      dwidth = image->width-mask->size+1;
   else if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_EXPAND )
      dwidth = image->width+mask->size-1;
   else
      dwidth = image->width;

   if ( dest == NULL )
      dest = gan_image_alloc ( GAN_VECTOR_FIELD_3D, image->type,
                               image->height, dwidth );
   else if (    dest->format != GAN_VECTOR_FIELD_3D
             || dest->type   != image->type
             || dest->width  != dwidth
             || dest->height != image->height )
      dest = gan_image_set_format_type_dims ( dest,
                                              GAN_VECTOR_FIELD_3D,
                                              image->type,
                                              image->height,
                                              dwidth );

   if ( dest == NULL )
   {
      gan_err_register ( "image_convolve1Dx_gl_v3D", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_CLIP )
      dest->offset_x = image->offset_x+(int)mask->size/2;
   else if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_EXPAND )
      dest->offset_x = image->offset_x-(int)mask->size/2;
   else
      dest->offset_x = image->offset_x;

   dest->offset_y = image->offset_y;

   switch ( image->type )
   {
      case GAN_DOUBLE:
        switch ( channel )
        {
           case GAN_X_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_d (
                         gan_image_get_pixptr_gl_d(image,i,0),           1,
                         eEdgeBehaviour, GAN_FALSE,
                         &gan_image_get_pixptr_vfield3D_d(image,i,0)->x, 3,
                         mask, (int)dwidth ) )
                   break;

             break;

           case GAN_Y_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_d (
                         gan_image_get_pixptr_gl_d(image,i,0),           1,
                         eEdgeBehaviour, GAN_FALSE,
                         &gan_image_get_pixptr_vfield3D_d(image,i,0)->y, 3,
                         mask, (int)dwidth ) )
                   break;

             break;

           case GAN_Z_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_d (
                         gan_image_get_pixptr_gl_d(image,i,0),           1,
                         eEdgeBehaviour, GAN_FALSE,
                         &gan_image_get_pixptr_vfield3D_d(image,i,0)->z, 3,
                         mask, (int)dwidth ) )
                   break;

             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "image_convolve1Dx_gl_v3D",
                                GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;

      case GAN_INT:
        switch ( channel )
        {
           case GAN_X_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_i (
                         gan_image_get_pixptr_gl_i(image,i,0),           1,
                         eEdgeBehaviour, GAN_FALSE,
                         &gan_image_get_pixptr_vfield3D_i(image,i,0)->x, 3,
                         mask, (int)dwidth ) )
                   break;

             break;

           case GAN_Y_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_i (
                         gan_image_get_pixptr_gl_i(image,i,0),           1,
                         eEdgeBehaviour, GAN_FALSE,
                         &gan_image_get_pixptr_vfield3D_i(image,i,0)->y, 3,
                         mask, (int)dwidth ) )
                   break;

             break;

           case GAN_Z_CHANNEL:
             for ( i = (int)dest->height-1; i >= 0; i-- )
                if ( !gan_convolve1D_i (
                         gan_image_get_pixptr_gl_i(image,i,0),           1,
                         eEdgeBehaviour, GAN_FALSE,
                         &gan_image_get_pixptr_vfield3D_i(image,i,0)->z, 3,
                         mask, (int)dwidth ) )
                   break;

             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "image_convolve1Dx_gl_v3D",
                                GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "image_convolve1Dx_gl_v3D",
                           GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
        return NULL;
   }

   /* check if we aborted the loop early because of an error */
   if ( i >= 0 )
   {
      gan_err_register ( "image_convolve1Dx_gl_v3D", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* success */
   return dest;
}

static Gan_Image *
 image_convolve1Dx_v3D_v3D ( const Gan_Image *image,
                             Gan_EdgeBehaviour eEdgeBehaviour,Gan_Mask1D *mask,
                             Gan_Image *dest )
{
   int i;
   unsigned int dwidth;

   gan_err_test_ptr ( image->format == GAN_VECTOR_FIELD_3D,
                      "image_convolve1Dx_v3D_v3D", GAN_ERROR_INCOMPATIBLE, "");
   gan_err_test_ptr ( image->width >= mask->size, "image_convolve1Dx_v3D_v3D",
                      GAN_ERROR_IMAGE_TOO_SMALL, "" );

   if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_CLIP )
      dwidth = image->width-mask->size+1;
   else if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_EXPAND )
      dwidth = image->width+mask->size-1;
   else
      dwidth = image->width;

   if ( dest == NULL )
      dest = gan_image_alloc ( GAN_VECTOR_FIELD_3D, image->type,
                               image->height, dwidth );
   else if (    dest->format != GAN_VECTOR_FIELD_3D
             || dest->type   != image->type
             || dest->width  != dwidth
             || dest->height != image->height )
      dest = gan_image_set_format_type_dims ( dest,
                                              GAN_VECTOR_FIELD_3D,
                                              image->type,
                                              image->height,
                                              dwidth );

   if ( dest == NULL )
   {
      gan_err_register ( "image_convolve1Dx_v3D_v3D", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_CLIP )
      dest->offset_x = image->offset_x+(int)mask->size/2;
   else if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_EXPAND )
      dest->offset_x = image->offset_x-(int)mask->size/2;
   else
      dest->offset_x = image->offset_x;

   dest->offset_y = image->offset_y;

   switch ( image->type )
   {
      case GAN_FLOAT:
        for ( i = (int)dest->height-1; i >= 0; i-- )
           if ( !gan_convolve1D_f (
                     &gan_image_get_pixptr_vfield3D_f(image,i,0)->x, 3,
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_vfield3D_f(dest,i,0)->x,  3,
                     mask, (int)dwidth ) ||
                !gan_convolve1D_f (
                     &gan_image_get_pixptr_vfield3D_f(image,i,0)->y, 3,
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_vfield3D_f(dest,i,0)->y,  3,
                     mask, (int)dwidth ) ||
                !gan_convolve1D_f (
                     &gan_image_get_pixptr_vfield3D_f(image,i,0)->z, 3,
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_vfield3D_f(dest,i,0)->z,  3,
                     mask, (int)dwidth ) )
              break;

        break;

      case GAN_DOUBLE:
        for ( i = (int)dest->height-1; i >= 0; i-- )
           if ( !gan_convolve1D_d (
                     &gan_image_get_pixptr_vfield3D_d(image,i,0)->x, 3,
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_vfield3D_d(dest,i,0)->x,  3,
                     mask, (int)dwidth ) ||
                !gan_convolve1D_d (
                     &gan_image_get_pixptr_vfield3D_d(image,i,0)->y, 3,
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_vfield3D_d(dest,i,0)->y,  3,
                     mask, (int)dwidth ) ||
                !gan_convolve1D_d (
                     &gan_image_get_pixptr_vfield3D_d(image,i,0)->z, 3,
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_vfield3D_d(dest,i,0)->z,  3,
                     mask, (int)dwidth ) )
              break;

        break;

      case GAN_INT:
        for ( i = (int)dest->height-1; i >= 0; i-- )
           if ( !gan_convolve1D_i (
                     &gan_image_get_pixptr_vfield3D_i(image,i,0)->x, 3,
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_vfield3D_i(dest,i,0)->x,  3,
                     mask, (int)dwidth ) ||
                !gan_convolve1D_i (
                     &gan_image_get_pixptr_vfield3D_i(image,i,0)->y, 3,
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_vfield3D_i(dest,i,0)->y,  3,
                     mask, (int)dwidth ) ||
                !gan_convolve1D_i (
                     &gan_image_get_pixptr_vfield3D_i(image,i,0)->z, 3,
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_vfield3D_i(dest,i,0)->z,  3,
                     mask, (int)dwidth ) )
              break;

        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "image_convolve1Dx_v3D_v3D",
                           GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
        return NULL;
   }

   /* check if we aborted the loop early because of an error */
   if ( i >= 0 )
   {
      gan_err_register ( "image_convolve1Dx_v3D_v3D", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* success */
   return dest;
}

static Gan_Image *
 image_convolve1Dx_rgb_rgb ( const Gan_Image *image,
                             Gan_EdgeBehaviour eEdgeBehaviour,
                             Gan_Mask1D *mask,
                             Gan_Image *dest )
{
   int i;
   unsigned int dwidth;

   gan_err_test_ptr ( image->format == GAN_RGB_COLOUR_IMAGE,
                      "image_convolve1Dx_rgb_rgb", GAN_ERROR_INCOMPATIBLE, "");
   gan_err_test_ptr ( image->width >= mask->size, "image_convolve1Dx_rgb_rgb",
                      GAN_ERROR_IMAGE_TOO_SMALL, "" );

   if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_CLIP )
      dwidth = image->width-mask->size+1;
   else if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_EXPAND )
      dwidth = image->width+mask->size-1;
   else
      dwidth = image->width;

   if ( dest == NULL )
      dest = gan_image_alloc ( GAN_RGB_COLOUR_IMAGE, image->type,
                               image->height, dwidth );
   else if (    dest->format != GAN_RGB_COLOUR_IMAGE
             || dest->type   != image->type
             || dest->width  != dwidth
             || dest->height != image->height )
      dest = gan_image_set_format_type_dims ( dest,
                                              image->format,
                                              image->type,
                                              image->height,
                                              dwidth );

   if ( dest == NULL )
   {
      gan_err_register ( "image_convolve1Dx_rgb_rgb", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_CLIP )
      dest->offset_x = image->offset_x+(int)mask->size/2;
   else if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_EXPAND )
      dest->offset_x = image->offset_x-(int)mask->size/2;
   else
      dest->offset_x = image->offset_x;

   dest->offset_y = image->offset_y;

   switch ( image->type )
   {
      case GAN_DOUBLE:
        for ( i = (int)dest->height-1; i >= 0; i-- )
           if ( !gan_convolve1D_d (
                     &gan_image_get_pixptr_rgb_d(image,i,0)->R, 3,
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_rgb_d(dest,i,0)->R,  3,
                     mask, (int)dwidth ) ||
                !gan_convolve1D_d (
                     &gan_image_get_pixptr_rgb_d(image,i,0)->G, 3,
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_rgb_d(dest,i,0)->G,  3,
                     mask, (int)dwidth ) ||
                !gan_convolve1D_d (
                     &gan_image_get_pixptr_rgb_d(image,i,0)->B, 3,
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_rgb_d(dest,i,0)->B,  3,
                     mask, (int)dwidth ) )
              break;

        break;

      case GAN_FLOAT:
#if 1
         for ( i = (int)dest->height-1; i >= 0; i-- )
           if ( !gan_convolve1D_f (
                     &gan_image_get_pixptr_rgb_f(image,i,0)->R, 3,
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_rgb_f(dest,i,0)->R,  3,
                     mask, (int)dwidth ) ||
                !gan_convolve1D_f (
                     &gan_image_get_pixptr_rgb_f(image,i,0)->G, 3,
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_rgb_f(dest,i,0)->G,  3,
                     mask, (int)dwidth ) ||
                !gan_convolve1D_f (
                     &gan_image_get_pixptr_rgb_f(image,i,0)->B, 3,
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_rgb_f(dest,i,0)->B,  3,
                     mask, (int)dwidth ) )
              break;
#else
         for ( i = (int)dest->height-1; i >= 0; i-- )
            if ( !gan_convolve1D_rgb_f (gan_image_get_pixptr_rgb_f(image,i,0), 3,
                                        gan_image_get_pixptr_rgb_f(dest,i,0),  3,
                                        mask, dest->width )
                 )
               break;
#endif
            
        break;
        
      case GAN_INT:
        for ( i = (int)dest->height-1; i >= 0; i-- )
           if ( !gan_convolve1D_i (
                     &gan_image_get_pixptr_rgb_i(image,i,0)->R, 3,
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_rgb_i(dest,i,0)->R,  3,
                     mask, (int)dwidth ) ||
                !gan_convolve1D_i (
                     &gan_image_get_pixptr_rgb_i(image,i,0)->G, 3,
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_rgb_i(dest,i,0)->G,  3,
                     mask, (int)dwidth ) ||
                !gan_convolve1D_i (
                     &gan_image_get_pixptr_rgb_i(image,i,0)->B, 3,
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_rgb_i(dest,i,0)->B,  3,
                     mask, (int)dwidth ) )
              break;

        break;
        
      case GAN_UINT:
        for ( i = (int)dest->height-1; i >= 0; i-- )
           if ( !gan_convolve1D_ui (
                     &gan_image_get_pixptr_rgb_ui(image,i,0)->R, 3,
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_rgb_ui(dest,i,0)->R,  3,
                     mask, (int)dwidth ) ||
                !gan_convolve1D_ui (
                     &gan_image_get_pixptr_rgb_ui(image,i,0)->G, 3,
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_rgb_ui(dest,i,0)->G,  3,
                     mask, (int)dwidth ) ||
                !gan_convolve1D_ui (
                     &gan_image_get_pixptr_rgb_ui(image,i,0)->B, 3,
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_rgb_ui(dest,i,0)->B,  3,
                     mask, (int)dwidth ) )
              break;

        break;
        
      case GAN_UCHAR:
#if 1
         for ( i = (int)dest->height-1; i >= 0; i-- )
           if ( !gan_convolve1D_uc (
                     &gan_image_get_pixptr_rgb_uc(image,i,0)->R, 3,
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_rgb_uc(dest,i,0)->R,  3,
                     mask, (int)dwidth ) ||
                !gan_convolve1D_uc (
                     &gan_image_get_pixptr_rgb_uc(image,i,0)->G, 3,
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_rgb_uc(dest,i,0)->G,  3,
                     mask, (int)dwidth ) ||
                !gan_convolve1D_uc (
                     &gan_image_get_pixptr_rgb_uc(image,i,0)->B, 3,
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_rgb_uc(dest,i,0)->B,  3,
                     mask, (int)dwidth ) )
              break;
#else
         for ( i = (int)dest->height-1; i >= 0; i-- )
            if ( !gan_convolve1D_rgb_uc (
                                     (unsigned char*)gan_image_get_pixptr_rgb_uc(image,i,0), 3,
                                     (unsigned char*)gan_image_get_pixptr_rgb_uc(dest,i,0),  3,
                                     mask, dest->width )
                 )
               break;
#endif       
        break;

      case GAN_USHORT:
        for ( i = (int)dest->height-1; i >= 0; i-- )
           if ( !gan_convolve1D_us (
                     &gan_image_get_pixptr_rgb_us(image,i,0)->R, 3,
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_rgb_us(dest,i,0)->R,  3,
                     mask, (int)dwidth ) ||
                !gan_convolve1D_us (
                     &gan_image_get_pixptr_rgb_us(image,i,0)->G, 3,
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_rgb_us(dest,i,0)->G,  3,
                     mask, (int)dwidth ) ||
                !gan_convolve1D_us (
                     &gan_image_get_pixptr_rgb_us(image,i,0)->B, 3,
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_rgb_us(dest,i,0)->B,  3,
                     mask, (int)dwidth ) )
              break;

        break;
        
      default:
        gan_err_flush_trace();
        gan_err_register ( "image_convolve1Dx_rgb_rgb",
                           GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
        return NULL;
   }

   /* check if we aborted the loop early because of an error */
   if ( i >= 0 )
   {
      gan_err_register ( "image_convolve1Dx_rgb_rgb", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* success */
   return dest;
}

static Gan_Image *
 image_convolve1Dx_rgba_rgba ( const Gan_Image *image,
                               Gan_Bool bConvolveInAlphaChannel,
                               Gan_EdgeBehaviour eEdgeBehaviour,
                               Gan_Mask1D *mask,
                               Gan_Image *dest )
{
   int i;
   unsigned int dwidth;

   gan_err_test_ptr ( image->format == GAN_RGB_COLOUR_ALPHA_IMAGE,
                      "image_convolve1Dx_rgba_rgba", GAN_ERROR_INCOMPATIBLE,
                      "" );
   gan_err_test_ptr ( image->width >= mask->size,"image_convolve1Dx_rgba_rgba",
                      GAN_ERROR_IMAGE_TOO_SMALL, "" );

   if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_CLIP )
      dwidth = image->width-mask->size+1;
   else if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_EXPAND )
      dwidth = image->width+mask->size-1;
   else
      dwidth = image->width;

   if ( dest == NULL )
      dest = gan_image_alloc ( GAN_RGB_COLOUR_ALPHA_IMAGE, image->type,
                               image->height, dwidth );
   else if (    dest->format != GAN_RGB_COLOUR_ALPHA_IMAGE
             || dest->type   != image->type
             || dest->width  != dwidth
             || dest->height != image->height )
      dest = gan_image_set_format_type_dims ( dest,
                                              image->format,
                                              image->type,
                                              image->height,
                                              dwidth );

   if ( dest == NULL )
   {
      gan_err_register ( "image_convolve1Dx_rgba_rgba", GAN_ERROR_FAILURE, "");
      return NULL;
   }

   if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_CLIP )
      dest->offset_x = image->offset_x+(int)mask->size/2;
   else if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_EXPAND )
      dest->offset_x = image->offset_x-(int)mask->size/2;
   else
      dest->offset_x = image->offset_x;

   dest->offset_y = image->offset_y;

   switch ( image->type )
   {
      case GAN_DOUBLE:
        for ( i = (int)dest->height-1; i >= 0; i-- )
           if ( !gan_convolve1D_d (
                     &gan_image_get_pixptr_rgba_d(image,i,0)->R, 4,
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_rgba_d(dest,i,0)->R,  4,
                     mask, (int)dwidth ) ||
                !gan_convolve1D_d (
                     &gan_image_get_pixptr_rgba_d(image,i,0)->G, 4,
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_rgba_d(dest,i,0)->G,  4,
                     mask, (int)dwidth ) ||
                !gan_convolve1D_d (
                     &gan_image_get_pixptr_rgba_d(image,i,0)->B, 4,
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_rgba_d(dest,i,0)->B,  4,
                     mask, (int)dwidth ) ||
                (bConvolveInAlphaChannel &&
                 !gan_convolve1D_d (
                     &gan_image_get_pixptr_rgba_d(image,i,0)->A, 4,
                     eEdgeBehaviour, GAN_TRUE,
                     &gan_image_get_pixptr_rgba_d(dest,i,0)->A,  4,
                     mask, (int)dwidth )) )
              break;

        break;

      case GAN_FLOAT:
        for ( i = (int)dest->height-1; i >= 0; i-- )
           if ( !gan_convolve1D_f (
                     &gan_image_get_pixptr_rgba_f(image,i,0)->R, 4,
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_rgba_f(dest,i,0)->R,  4,
                     mask, (int)dwidth ) ||
                !gan_convolve1D_f (
                     &gan_image_get_pixptr_rgba_f(image,i,0)->G, 4,
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_rgba_f(dest,i,0)->G,  4,
                     mask, (int)dwidth ) ||
                !gan_convolve1D_f (
                     &gan_image_get_pixptr_rgba_f(image,i,0)->B, 4,
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_rgba_f(dest,i,0)->B,  4,
                     mask, (int)dwidth ) ||
                (bConvolveInAlphaChannel &&
                 !gan_convolve1D_f (
                     &gan_image_get_pixptr_rgba_f(image,i,0)->A, 4,
                     eEdgeBehaviour, GAN_TRUE,
                     &gan_image_get_pixptr_rgba_f(dest,i,0)->A,  4,
                     mask, (int)dwidth )) )
              break;

        break;        
        
      case GAN_INT:
        for ( i = (int)dest->height-1; i >= 0; i-- )
           if ( !gan_convolve1D_i (
                     &gan_image_get_pixptr_rgba_i(image,i,0)->R, 4,
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_rgba_i(dest,i,0)->R,  4,
                     mask, (int)dwidth ) ||
                !gan_convolve1D_i (
                     &gan_image_get_pixptr_rgba_i(image,i,0)->G, 4,
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_rgba_i(dest,i,0)->G,  4,
                     mask, (int)dwidth ) ||
                !gan_convolve1D_i (
                     &gan_image_get_pixptr_rgba_i(image,i,0)->B, 4,
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_rgba_i(dest,i,0)->B,  4,
                     mask, (int)dwidth ) ||
                (bConvolveInAlphaChannel &&
                 !gan_convolve1D_i (
                     &gan_image_get_pixptr_rgba_i(image,i,0)->A, 4,
                     eEdgeBehaviour, GAN_TRUE,
                     &gan_image_get_pixptr_rgba_i(dest,i,0)->A,  4,
                     mask, (int)dwidth )) )
              break;

        break;

      case GAN_UINT:
        for ( i = (int)dest->height-1; i >= 0; i-- )
           if ( !gan_convolve1D_ui (
                     &gan_image_get_pixptr_rgba_ui(image,i,0)->R, 4,
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_rgba_ui(dest,i,0)->R,  4,
                     mask, (int)dwidth ) ||
                !gan_convolve1D_ui (
                     &gan_image_get_pixptr_rgba_ui(image,i,0)->G, 4,
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_rgba_ui(dest,i,0)->G,  4,
                     mask, (int)dwidth ) ||
                !gan_convolve1D_ui (
                     &gan_image_get_pixptr_rgba_ui(image,i,0)->B, 4,
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_rgba_ui(dest,i,0)->B,  4,
                     mask, (int)dwidth ) ||
                (bConvolveInAlphaChannel &&
                 !gan_convolve1D_ui (
                     &gan_image_get_pixptr_rgba_ui(image,i,0)->A, 4,
                     eEdgeBehaviour, GAN_TRUE,
                     &gan_image_get_pixptr_rgba_ui(dest,i,0)->A,  4,
                     mask, (int)dwidth )) )
              break;

        break;
        
      case GAN_UCHAR:
        for ( i = (int)dest->height-1; i >= 0; i-- )
           if ( !gan_convolve1D_uc (
                     &gan_image_get_pixptr_rgba_uc(image,i,0)->R, 4,
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_rgba_uc(dest,i,0)->R,  4,
                     mask, (int)dwidth ) ||
                !gan_convolve1D_uc (
                     &gan_image_get_pixptr_rgba_uc(image,i,0)->G, 4,
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_rgba_uc(dest,i,0)->G,  4,
                     mask, (int)dwidth ) ||
                !gan_convolve1D_uc (
                     &gan_image_get_pixptr_rgba_uc(image,i,0)->B, 4,
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_rgba_uc(dest,i,0)->B,  4,
                     mask, (int)dwidth ) ||
                (bConvolveInAlphaChannel &&
                 !gan_convolve1D_uc (
                     &gan_image_get_pixptr_rgba_uc(image,i,0)->A, 4,
                     eEdgeBehaviour, GAN_TRUE,
                     &gan_image_get_pixptr_rgba_uc(dest,i,0)->A,  4,
                     mask, (int)dwidth )) )
              break;

        break;

      case GAN_USHORT:
        for ( i = (int)dest->height-1; i >= 0; i-- )
           if ( !gan_convolve1D_us (
                     &gan_image_get_pixptr_rgba_us(image,i,0)->R, 4,
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_rgba_us(dest,i,0)->R,  4,
                     mask, (int)dwidth ) ||
                !gan_convolve1D_us (
                     &gan_image_get_pixptr_rgba_us(image,i,0)->G, 4,
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_rgba_us(dest,i,0)->G,  4,
                     mask, (int)dwidth ) ||
                !gan_convolve1D_us (
                     &gan_image_get_pixptr_rgba_us(image,i,0)->B, 4,
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_rgba_us(dest,i,0)->B,  4,
                     mask, (int)dwidth ) ||
                (bConvolveInAlphaChannel &&
                 !gan_convolve1D_us (
                     &gan_image_get_pixptr_rgba_us(image,i,0)->A, 4,
                     eEdgeBehaviour, GAN_TRUE,
                     &gan_image_get_pixptr_rgba_us(dest,i,0)->A,  4,
                     mask, (int)dwidth )) )
              break;

        break;
        
      default:
        gan_err_flush_trace();
        gan_err_register ( "image_convolve1Dx_rgba_rgba",
                           GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
        return NULL;
   }

   /* check if we aborted the loop early because of an error */
   if ( i >= 0 )
   {
      gan_err_register ( "image_convolve1Dx_rgba_rgba", GAN_ERROR_FAILURE, "");
      return NULL;
   }

   /* success */
   return dest;
}

static Gan_Image *
 image_convolve1Dx_bgr_bgr ( const Gan_Image *image,
                             Gan_EdgeBehaviour eEdgeBehaviour,
                             Gan_Mask1D *mask,
                             Gan_Image *dest )
{
   int i;
   unsigned int dwidth;

   gan_err_test_ptr ( image->format == GAN_BGR_COLOUR_IMAGE,
                      "image_convolve1Dx_bgr_bgr", GAN_ERROR_INCOMPATIBLE, "");
   gan_err_test_ptr ( image->width >= mask->size, "image_convolve1Dx_bgr_bgr",
                      GAN_ERROR_IMAGE_TOO_SMALL, "" );

   if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_CLIP )
      dwidth = image->width-mask->size+1;
   else if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_EXPAND )
      dwidth = image->width+mask->size-1;
   else
      dwidth = image->width;

   if ( dest == NULL )
      dest = gan_image_alloc ( GAN_BGR_COLOUR_IMAGE, image->type,
                               image->height, dwidth );
   else if (    dest->format != GAN_BGR_COLOUR_IMAGE
             || dest->type   != image->type
             || dest->width  != dwidth
             || dest->height != image->height )
      dest = gan_image_set_format_type_dims ( dest,
                                              image->format,
                                              image->type,
                                              image->height,
                                              dwidth );

   if ( dest == NULL )
   {
      gan_err_register ( "image_convolve1Dx_bgr_bgr", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_CLIP )
      dest->offset_x = image->offset_x+(int)mask->size/2;
   else if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_EXPAND )
      dest->offset_x = image->offset_x-(int)mask->size/2;
   else
      dest->offset_x = image->offset_x;

   dest->offset_y = image->offset_y;

   switch ( image->type )
   {
      case GAN_DOUBLE:
        for ( i = (int)dest->height-1; i >= 0; i-- )
           if ( !gan_convolve1D_d (
                     &gan_image_get_pixptr_bgr_d(image,i,0)->R, 3,
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_bgr_d(dest,i,0)->R,  3,
                     mask, (int)dwidth ) ||
                !gan_convolve1D_d (
                     &gan_image_get_pixptr_bgr_d(image,i,0)->G, 3,
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_bgr_d(dest,i,0)->G,  3,
                     mask, (int)dwidth ) ||
                !gan_convolve1D_d (
                     &gan_image_get_pixptr_bgr_d(image,i,0)->B, 3,
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_bgr_d(dest,i,0)->B,  3,
                     mask, (int)dwidth ) )
              break;

        break;

      case GAN_FLOAT:
#if 1
         for ( i = (int)dest->height-1; i >= 0; i-- )
           if ( !gan_convolve1D_f (
                     &gan_image_get_pixptr_bgr_f(image,i,0)->R, 3,
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_bgr_f(dest,i,0)->R,  3,
                     mask, (int)dwidth ) ||
                !gan_convolve1D_f (
                     &gan_image_get_pixptr_bgr_f(image,i,0)->G, 3,
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_bgr_f(dest,i,0)->G,  3,
                     mask, (int)dwidth ) ||
                !gan_convolve1D_f (
                     &gan_image_get_pixptr_bgr_f(image,i,0)->B, 3,
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_bgr_f(dest,i,0)->B,  3,
                     mask, (int)dwidth ) )
              break;
#else
         for ( i = (int)dest->height-1; i >= 0; i-- )
            if ( !gan_convolve1D_bgr_f (gan_image_get_pixptr_bgr_f(image,i,0), 3,
                                        gan_image_get_pixptr_bgr_f(dest,i,0),  3,
                                        mask, dest->width )
                 )
               break;
#endif
            
        break;
        
      case GAN_INT:
        for ( i = (int)dest->height-1; i >= 0; i-- )
           if ( !gan_convolve1D_i (
                     &gan_image_get_pixptr_bgr_i(image,i,0)->R, 3,
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_bgr_i(dest,i,0)->R,  3,
                     mask, (int)dwidth ) ||
                !gan_convolve1D_i (
                     &gan_image_get_pixptr_bgr_i(image,i,0)->G, 3,
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_bgr_i(dest,i,0)->G,  3,
                     mask, (int)dwidth ) ||
                !gan_convolve1D_i (
                     &gan_image_get_pixptr_bgr_i(image,i,0)->B, 3,
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_bgr_i(dest,i,0)->B,  3,
                     mask, (int)dwidth ) )
              break;

        break;
        
      case GAN_UINT:
        for ( i = (int)dest->height-1; i >= 0; i-- )
           if ( !gan_convolve1D_ui (
                     &gan_image_get_pixptr_bgr_ui(image,i,0)->R, 3,
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_bgr_ui(dest,i,0)->R,  3,
                     mask, (int)dwidth ) ||
                !gan_convolve1D_ui (
                     &gan_image_get_pixptr_bgr_ui(image,i,0)->G, 3,
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_bgr_ui(dest,i,0)->G,  3,
                     mask, (int)dwidth ) ||
                !gan_convolve1D_ui (
                     &gan_image_get_pixptr_bgr_ui(image,i,0)->B, 3,
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_bgr_ui(dest,i,0)->B,  3,
                     mask, (int)dwidth ) )
              break;

        break;
        
      case GAN_UCHAR:
#if 1
         for ( i = (int)dest->height-1; i >= 0; i-- )
           if ( !gan_convolve1D_uc (
                     &gan_image_get_pixptr_bgr_uc(image,i,0)->R, 3,
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_bgr_uc(dest,i,0)->R,  3,
                     mask, (int)dwidth ) ||
                !gan_convolve1D_uc (
                     &gan_image_get_pixptr_bgr_uc(image,i,0)->G, 3,
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_bgr_uc(dest,i,0)->G,  3,
                     mask, (int)dwidth ) ||
                !gan_convolve1D_uc (
                     &gan_image_get_pixptr_bgr_uc(image,i,0)->B, 3,
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_bgr_uc(dest,i,0)->B,  3,
                     mask, (int)dwidth ) )
              break;
#else
         for ( i = (int)dest->height-1; i >= 0; i-- )
            if ( !gan_convolve1D_bgr_uc (
                                     (unsigned char*)gan_image_get_pixptr_bgr_uc(image,i,0), 3,
                                     (unsigned char*)gan_image_get_pixptr_bgr_uc(dest,i,0),  3,
                                     mask, dest->width )
                 )
               break;
#endif       
        break;

      case GAN_USHORT:
        for ( i = (int)dest->height-1; i >= 0; i-- )
           if ( !gan_convolve1D_us (
                     &gan_image_get_pixptr_bgr_us(image,i,0)->R, 3,
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_bgr_us(dest,i,0)->R,  3,
                     mask, (int)dwidth ) ||
                !gan_convolve1D_us (
                     &gan_image_get_pixptr_bgr_us(image,i,0)->G, 3,
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_bgr_us(dest,i,0)->G,  3,
                     mask, (int)dwidth ) ||
                !gan_convolve1D_us (
                     &gan_image_get_pixptr_bgr_us(image,i,0)->B, 3,
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_bgr_us(dest,i,0)->B,  3,
                     mask, (int)dwidth ) )
              break;

        break;
        
      default:
        gan_err_flush_trace();
        gan_err_register ( "image_convolve1Dx_bgr_bgr",
                           GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
        return NULL;
   }

   /* check if we aborted the loop early because of an error */
   if ( i >= 0 )
   {
      gan_err_register ( "image_convolve1Dx_bgr_bgr", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* success */
   return dest;
}

static Gan_Image *
 image_convolve1Dx_bgra_bgra ( const Gan_Image *image,
                               Gan_Bool bConvolveInAlphaChannel,
                               Gan_EdgeBehaviour eEdgeBehaviour,
                               Gan_Mask1D *mask,
                               Gan_Image *dest )
{
   int i;
   unsigned int dwidth;

   gan_err_test_ptr ( image->format == GAN_BGR_COLOUR_ALPHA_IMAGE,
                      "image_convolve1Dx_bgra_bgra", GAN_ERROR_INCOMPATIBLE,
                      "" );
   gan_err_test_ptr ( image->width >= mask->size,"image_convolve1Dx_bgra_bgra",
                      GAN_ERROR_IMAGE_TOO_SMALL, "" );

   if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_CLIP )
      dwidth = image->width-mask->size+1;
   else if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_EXPAND )
      dwidth = image->width+mask->size-1;
   else
      dwidth = image->width;

   if ( dest == NULL )
      dest = gan_image_alloc ( GAN_BGR_COLOUR_ALPHA_IMAGE, image->type,
                               image->height, dwidth );
   else if (    dest->format != GAN_BGR_COLOUR_ALPHA_IMAGE
             || dest->type   != image->type
             || dest->width  != dwidth
             || dest->height != image->height )
      dest = gan_image_set_format_type_dims ( dest,
                                              image->format,
                                              image->type,
                                              image->height,
                                              dwidth );

   if ( dest == NULL )
   {
      gan_err_register ( "image_convolve1Dx_bgra_bgra", GAN_ERROR_FAILURE, "");
      return NULL;
   }

   if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_CLIP )
      dest->offset_x = image->offset_x+(int)mask->size/2;
   else if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_EXPAND )
      dest->offset_x = image->offset_x-(int)mask->size/2;
   else
      dest->offset_x = image->offset_x;

   dest->offset_y = image->offset_y;

   switch ( image->type )
   {
      case GAN_DOUBLE:
        for ( i = (int)dest->height-1; i >= 0; i-- )
           if ( !gan_convolve1D_d (
                     &gan_image_get_pixptr_bgra_d(image,i,0)->R, 4,
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_bgra_d(dest,i,0)->R,  4,
                     mask, (int)dwidth ) ||
                !gan_convolve1D_d (
                     &gan_image_get_pixptr_bgra_d(image,i,0)->G, 4,
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_bgra_d(dest,i,0)->G,  4,
                     mask, (int)dwidth ) ||
                !gan_convolve1D_d (
                     &gan_image_get_pixptr_bgra_d(image,i,0)->B, 4,
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_bgra_d(dest,i,0)->B,  4,
                     mask, (int)dwidth ) ||
                (bConvolveInAlphaChannel &&
                 !gan_convolve1D_d (
                     &gan_image_get_pixptr_bgra_d(image,i,0)->A, 4,
                     eEdgeBehaviour, GAN_TRUE,
                     &gan_image_get_pixptr_bgra_d(dest,i,0)->A,  4,
                     mask, (int)dwidth )) )
              break;

        break;

      case GAN_FLOAT:
        for ( i = (int)dest->height-1; i >= 0; i-- )
           if ( !gan_convolve1D_f (
                     &gan_image_get_pixptr_bgra_f(image,i,0)->R, 4,
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_bgra_f(dest,i,0)->R,  4,
                     mask, (int)dwidth ) ||
                !gan_convolve1D_f (
                     &gan_image_get_pixptr_bgra_f(image,i,0)->G, 4,
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_bgra_f(dest,i,0)->G,  4,
                     mask, (int)dwidth ) ||
                !gan_convolve1D_f (
                     &gan_image_get_pixptr_bgra_f(image,i,0)->B, 4,
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_bgra_f(dest,i,0)->B,  4,
                     mask, (int)dwidth ) ||
                (bConvolveInAlphaChannel &&
                 !gan_convolve1D_f (
                     &gan_image_get_pixptr_bgra_f(image,i,0)->A, 4,
                     eEdgeBehaviour, GAN_TRUE,
                     &gan_image_get_pixptr_bgra_f(dest,i,0)->A,  4,
                     mask, (int)dwidth )) )
              break;

        break;        
        
      case GAN_INT:
        for ( i = (int)dest->height-1; i >= 0; i-- )
           if ( !gan_convolve1D_i (
                     &gan_image_get_pixptr_bgra_i(image,i,0)->R, 4,
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_bgra_i(dest,i,0)->R,  4,
                     mask, (int)dwidth ) ||
                !gan_convolve1D_i (
                     &gan_image_get_pixptr_bgra_i(image,i,0)->G, 4,
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_bgra_i(dest,i,0)->G,  4,
                     mask, (int)dwidth ) ||
                !gan_convolve1D_i (
                     &gan_image_get_pixptr_bgra_i(image,i,0)->B, 4,
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_bgra_i(dest,i,0)->B,  4,
                     mask, (int)dwidth ) ||
                (bConvolveInAlphaChannel &&
                 !gan_convolve1D_i (
                     &gan_image_get_pixptr_bgra_i(image,i,0)->A, 4,
                     eEdgeBehaviour, GAN_TRUE,
                     &gan_image_get_pixptr_bgra_i(dest,i,0)->A,  4,
                     mask, (int)dwidth )) )
              break;

        break;

      case GAN_UINT:
        for ( i = (int)dest->height-1; i >= 0; i-- )
           if ( !gan_convolve1D_ui (
                     &gan_image_get_pixptr_bgra_ui(image,i,0)->R, 4,
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_bgra_ui(dest,i,0)->R,  4,
                     mask, (int)dwidth ) ||
                !gan_convolve1D_ui (
                     &gan_image_get_pixptr_bgra_ui(image,i,0)->G, 4,
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_bgra_ui(dest,i,0)->G,  4,
                     mask, (int)dwidth ) ||
                !gan_convolve1D_ui (
                     &gan_image_get_pixptr_bgra_ui(image,i,0)->B, 4,
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_bgra_ui(dest,i,0)->B,  4,
                     mask, (int)dwidth ) ||
                (bConvolveInAlphaChannel &&
                 !gan_convolve1D_ui (
                     &gan_image_get_pixptr_bgra_ui(image,i,0)->A, 4,
                     eEdgeBehaviour, GAN_TRUE,
                     &gan_image_get_pixptr_bgra_ui(dest,i,0)->A,  4,
                     mask, (int)dwidth )) )
              break;

        break;
        
      case GAN_UCHAR:
        for ( i = (int)dest->height-1; i >= 0; i-- )
           if ( !gan_convolve1D_uc (
                     &gan_image_get_pixptr_bgra_uc(image,i,0)->R, 4,
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_bgra_uc(dest,i,0)->R,  4,
                     mask, (int)dwidth ) ||
                !gan_convolve1D_uc (
                     &gan_image_get_pixptr_bgra_uc(image,i,0)->G, 4,
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_bgra_uc(dest,i,0)->G,  4,
                     mask, (int)dwidth ) ||
                !gan_convolve1D_uc (
                     &gan_image_get_pixptr_bgra_uc(image,i,0)->B, 4,
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_bgra_uc(dest,i,0)->B,  4,
                     mask, (int)dwidth ) ||
                (bConvolveInAlphaChannel &&
                 !gan_convolve1D_uc (
                     &gan_image_get_pixptr_bgra_uc(image,i,0)->A, 4,
                     eEdgeBehaviour, GAN_TRUE,
                     &gan_image_get_pixptr_bgra_uc(dest,i,0)->A,  4,
                     mask, (int)dwidth )) )
              break;

        break;

      case GAN_USHORT:
        for ( i = (int)dest->height-1; i >= 0; i-- )
           if ( !gan_convolve1D_us (
                     &gan_image_get_pixptr_bgra_us(image,i,0)->R, 4,
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_bgra_us(dest,i,0)->R,  4,
                     mask, (int)dwidth ) ||
                !gan_convolve1D_us (
                     &gan_image_get_pixptr_bgra_us(image,i,0)->G, 4,
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_bgra_us(dest,i,0)->G,  4,
                     mask, (int)dwidth ) ||
                !gan_convolve1D_us (
                     &gan_image_get_pixptr_bgra_us(image,i,0)->B, 4,
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_bgra_us(dest,i,0)->B,  4,
                     mask, (int)dwidth ) ||
                (bConvolveInAlphaChannel &&
                 !gan_convolve1D_us (
                     &gan_image_get_pixptr_bgra_us(image,i,0)->A, 4,
                     eEdgeBehaviour, GAN_TRUE,
                     &gan_image_get_pixptr_bgra_us(dest,i,0)->A,  4,
                     mask, (int)dwidth )) )
              break;

        break;
        
      default:
        gan_err_flush_trace();
        gan_err_register ( "image_convolve1Dx_bgra_bgra",
                           GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
        return NULL;
   }

   /* check if we aborted the loop early because of an error */
   if ( i >= 0 )
   {
      gan_err_register ( "image_convolve1Dx_bgra_bgra", GAN_ERROR_FAILURE, "");
      return NULL;
   }

   /* success */
   return dest;
}

/**
 * \brief Convolves an image in the x-direction.
 * \param image The input image
 * \param channel Colour channel to be convolved where applicable
 * \param eEdgeBehaviour Behaviour of data beyond edge
 * \param mask The image convolution mask
 * \param dest The destination image for the convolution operation
 *
 * Applies a one-dimensional convolution operation to the given image in the
 * x-direction. When the image contains colour or vector field data,
 * a particular colour channel/vector field element can be specified by the
 * channel argument, which should otherwise be passed as #GAN_ALL_CHANNELS.
 * There is no checking for overflow of integer values.
 *
 * Macro call to gan_image_convolve1Dx_q().
 * \return Non-\c NULL on successfully returning the destination image \a dest,
 * \c NULL on failure.
 * \sa gan_image_convolve1Dx_q, gan_gauss_mask_new().
 */
Gan_Image *
 gan_image_convolve1Dx_q ( const Gan_Image *image, Gan_ImageChannelType channel,
                           Gan_EdgeBehaviour eEdgeBehaviour,
                           Gan_Mask1D *mask, Gan_Image *dest )
{
   switch ( image->format )
   {
      case GAN_GREY_LEVEL_IMAGE:
      switch ( channel )
      {
         case GAN_INTENSITY_CHANNEL:
         case GAN_ALL_CHANNELS:
           dest = image_convolve1Dx_gl_gl ( image, eEdgeBehaviour, mask,
                                            dest );
           break;

         case GAN_X_CHANNEL:
         case GAN_Y_CHANNEL:
         case GAN_Z_CHANNEL:
           dest = image_convolve1Dx_gl_v3D ( image, channel, eEdgeBehaviour,
                                             mask, dest );
           break;

         default:
           gan_err_flush_trace();
           gan_err_register ( "gan_image_convolve1Dx_q",
                              GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
           return NULL;
      }
      break;

      case GAN_VECTOR_FIELD_3D:
      switch ( channel )
      {
         case GAN_ALL_CHANNELS:
           dest = image_convolve1Dx_v3D_v3D ( image, eEdgeBehaviour, mask,
                                              dest );
           break;

         case GAN_X_CHANNEL:
         case GAN_Y_CHANNEL:
         case GAN_Z_CHANNEL:
           dest = image_convolve1Dx_v3D_gl ( image, channel, eEdgeBehaviour,
                                             mask, dest );
           break;

         default:
           gan_err_flush_trace();
           gan_err_register ( "gan_image_convolve1Dx_q",
                              GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
           return NULL;
      }
      break;
      
      case GAN_RGB_COLOUR_IMAGE:
      switch ( channel )
      {
         case GAN_ALL_CHANNELS:
           dest = image_convolve1Dx_rgb_rgb ( image, eEdgeBehaviour, mask,
                                              dest );
           break;

         case GAN_RED_CHANNEL:
         case GAN_GREEN_CHANNEL:
         case GAN_BLUE_CHANNEL:
           dest = image_convolve1Dx_rgb_gl ( image, channel, eEdgeBehaviour,
                                             mask, dest );
           break;

         default:
           gan_err_flush_trace();
           gan_err_register ( "gan_image_convolve1Dx_q",
                              GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
           return NULL;
      }
      break;

      case GAN_RGB_COLOUR_ALPHA_IMAGE:
      switch ( channel )
      {
         case GAN_ALL_CHANNELS:
           dest = image_convolve1Dx_rgba_rgba ( image, GAN_TRUE, eEdgeBehaviour, mask, dest );
           break;

         case GAN_ALL_COLOUR_CHANNELS:
           dest = image_convolve1Dx_rgba_rgba ( image, GAN_FALSE, eEdgeBehaviour, mask, dest );
           break;

         case GAN_RED_CHANNEL:
         case GAN_GREEN_CHANNEL:
         case GAN_BLUE_CHANNEL:
         case GAN_ALPHA_CHANNEL:  
           dest = image_convolve1Dx_rgba_gl ( image, channel, eEdgeBehaviour,
                                              mask, dest );
           break;

         default:
           gan_err_flush_trace();
           gan_err_register ( "gan_image_convolve1Dx_q",
                              GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
           return NULL;
      }
      break;
      
      case GAN_BGR_COLOUR_IMAGE:
      switch ( channel )
      {
         case GAN_ALL_CHANNELS:
           dest = image_convolve1Dx_bgr_bgr ( image, eEdgeBehaviour, mask,
                                              dest );
           break;

         case GAN_RED_CHANNEL:
         case GAN_GREEN_CHANNEL:
         case GAN_BLUE_CHANNEL:
           dest = image_convolve1Dx_bgr_gl ( image, channel, eEdgeBehaviour,
                                             mask, dest );
           break;

         default:
           gan_err_flush_trace();
           gan_err_register ( "gan_image_convolve1Dx_q",
                              GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
           return NULL;
      }
      break;

      case GAN_BGR_COLOUR_ALPHA_IMAGE:
      switch ( channel )
      {
         case GAN_ALL_CHANNELS:
           dest = image_convolve1Dx_bgra_bgra ( image, GAN_TRUE, eEdgeBehaviour, mask, dest );
           break;

         case GAN_ALL_COLOUR_CHANNELS:
           dest = image_convolve1Dx_bgra_bgra ( image, GAN_FALSE, eEdgeBehaviour, mask, dest );
           break;

         case GAN_RED_CHANNEL:
         case GAN_GREEN_CHANNEL:
         case GAN_BLUE_CHANNEL:
         case GAN_ALPHA_CHANNEL:  
           dest = image_convolve1Dx_bgra_gl ( image, channel, eEdgeBehaviour,
                                              mask, dest );
           break;

         default:
           gan_err_flush_trace();
           gan_err_register ( "gan_image_convolve1Dx_q",
                              GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
           return NULL;
      }
      break;
      
      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_image_convolve1Dx_q", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
        return NULL;
   }

   if ( dest == NULL )
   {
      gan_err_register ( "gan_image_convolve1Dx_q", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* success */
   return dest;
}


static Gan_Image *
 image_convolve1Dy_gl_gl ( const Gan_Image *image, Gan_EdgeBehaviour eEdgeBehaviour,
                           Gan_Mask1D *mask, Gan_Image *dest )
{
   int j;
   unsigned int dheight;

   gan_err_test_ptr ( image->format == GAN_GREY_LEVEL_IMAGE,
                      "image_convolve1Dy_gl_gl", GAN_ERROR_INCOMPATIBLE, "" );
   gan_err_test_ptr ( image->height >= mask->size, "image_convolve1Dy_gl_gl",
                      GAN_ERROR_IMAGE_TOO_SMALL, "" );

   if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_CLIP )
      dheight = image->height-mask->size+1;
   else if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_EXPAND )
      dheight = image->height+mask->size-1;
   else
      dheight = image->height;

   if ( dest == NULL )
      dest = gan_image_alloc ( GAN_GREY_LEVEL_IMAGE, image->type,
                               dheight, image->width );
   else if (    dest->format != GAN_GREY_LEVEL_IMAGE
             || dest->type   != image->type
             || dest->width  != image->width
             || dest->height != dheight )
      dest = gan_image_set_format_type_dims ( dest,
                                              GAN_GREY_LEVEL_IMAGE,
                                              image->type,
                                              dheight,
                                              image->width );

   if ( dest == NULL )
   {
      gan_err_register ( "image_convolve1Dy_gl_gl", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   dest->offset_x = image->offset_x;
   if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_CLIP )
      dest->offset_y = image->offset_y+(int)mask->size/2;
   else if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_EXPAND )
      dest->offset_y = image->offset_y-(int)mask->size/2;
   else
      dest->offset_y = image->offset_y;

   switch ( image->type )
   {
      case GAN_FLOAT:
        gan_err_test_ptr ( (image->stride % sizeof(float)) == 0 &&
                           (dest->stride % sizeof(float)) == 0,
                           "image_convolve1Dy_gl_gl", GAN_ERROR_INCOMPATIBLE,
                           "" );
        for ( j = (int)dest->width-1; j >= 0; j-- )
           if ( !gan_convolve1D_f ( gan_image_get_pixptr_gl_f(image,0,j),
                                    image->stride/sizeof(float),
                                    eEdgeBehaviour, GAN_FALSE,
                                    gan_image_get_pixptr_gl_f(dest,0,j),
                                    dest->stride/sizeof(float),
                                    mask, (int)dheight ) )
           {
              gan_err_register ( "image_convolve1Dy_gl_gl", GAN_ERROR_FAILURE,
                                 "" );
              return NULL;
           }
                        
        break;

      case GAN_DOUBLE:
        gan_err_test_ptr ( (image->stride % sizeof(double)) == 0 &&
                           (dest->stride % sizeof(double)) == 0,
                           "image_convolve1Dy_gl_gl", GAN_ERROR_INCOMPATIBLE,
                           "" );
        for ( j = (int)dest->width-1; j >= 0; j-- )
           if ( !gan_convolve1D_d ( gan_image_get_pixptr_gl_d(image,0,j),
                                    image->stride/sizeof(double),
                                    eEdgeBehaviour, GAN_FALSE,
                                    gan_image_get_pixptr_gl_d(dest,0,j),
                                    dest->stride/sizeof(double),
                                    mask, (int)dheight ) )
           {
              gan_err_register ( "image_convolve1Dy_gl_gl", GAN_ERROR_FAILURE,
                                 "" );
              return NULL;
           }
                        
        break;

      case GAN_INT:
        gan_err_test_ptr ( (image->stride % sizeof(int)) == 0 &&
                           (dest->stride % sizeof(int)) == 0,
                           "image_convolve1Dy_gl_gl", GAN_ERROR_INCOMPATIBLE,
                           "" );
        for ( j = (int)dest->width-1; j >= 0; j-- )
           if ( !gan_convolve1D_i ( gan_image_get_pixptr_gl_i(image,0,j),
                                    image->stride/sizeof(int),
                                    eEdgeBehaviour, GAN_FALSE,
                                    gan_image_get_pixptr_gl_i(dest,0,j),
                                    dest->stride/sizeof(int),
                                    mask, (int)dheight ) )
           {
              gan_err_register ( "image_convolve1Dy_gl_gl", GAN_ERROR_FAILURE,
                                 "" );
              return NULL;
           }

        break;

      case GAN_UINT:
        gan_err_test_ptr ( (image->stride % sizeof(unsigned int)) == 0 &&
                           (dest->stride % sizeof(unsigned int)) == 0,
                           "image_convolve1Dy_gl_gl", GAN_ERROR_INCOMPATIBLE,
                           "" );
        for ( j = (int)dest->width-1; j >= 0; j-- )
           if ( !gan_convolve1D_ui ( gan_image_get_pixptr_gl_ui(image,0,j), image->stride/sizeof(unsigned int),
                                     eEdgeBehaviour, GAN_FALSE,
                                     gan_image_get_pixptr_gl_ui(dest,0,j), image->stride/sizeof(unsigned int),
                                     mask, (int)dheight ) )
           {
              gan_err_register ( "image_convolve1Dy_gl_gl", GAN_ERROR_FAILURE,
                                 "" );
              return NULL;
           }

        break;

      case GAN_UCHAR:
        gan_err_test_ptr ( (image->stride % sizeof(unsigned char)) == 0 &&
                           (dest->stride % sizeof(unsigned char)) == 0,
                           "image_convolve1Dy_gl_gl", GAN_ERROR_INCOMPATIBLE,
                           "" );
        for ( j = (int)dest->width-1; j >= 0; j-- )
           if ( !gan_convolve1D_uc ( gan_image_get_pixptr_gl_uc(image,0,j), image->stride/sizeof(unsigned char),
                                     eEdgeBehaviour, GAN_FALSE,
                                     gan_image_get_pixptr_gl_uc(dest,0,j), image->stride/sizeof(unsigned char),
                                     mask, (int)dheight ) )
           {
              gan_err_register ( "image_convolve1Dy_gl_gl", GAN_ERROR_FAILURE,
                                 "" );
              return NULL;
           }

        break;

      case GAN_USHORT:
        gan_err_test_ptr ( (image->stride % sizeof(unsigned short)) == 0 &&
                           (dest->stride % sizeof(unsigned short)) == 0,
                           "image_convolve1Dy_gl_gl", GAN_ERROR_INCOMPATIBLE,
                           "" );
        for ( j = (int)dest->width-1; j >= 0; j-- )
           if ( !gan_convolve1D_us ( gan_image_get_pixptr_gl_us(image,j,0), image->stride/sizeof(unsigned short),
                                     eEdgeBehaviour, GAN_FALSE,
                                     gan_image_get_pixptr_gl_us(dest,0,j), image->stride/sizeof(unsigned short),
                                     mask, (int)dheight ) )
           {
              gan_err_register ( "image_convolve1Dy_gl_gl", GAN_ERROR_FAILURE,
                                 "" );
              return NULL;
           }

        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "image_convolve1Dy_gl_gl", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
        return NULL;
   }

   /* success */
   return dest;
}

static Gan_Image *
 image_convolve1Dy_v3D_gl ( const Gan_Image *image, Gan_ImageChannelType channel,
                            Gan_EdgeBehaviour eEdgeBehaviour, Gan_Mask1D *mask,
                            Gan_Image *dest )
{
   int j;
   unsigned int dheight;

   gan_err_test_ptr ( image->format == GAN_VECTOR_FIELD_3D,
                      "image_convolve1Dy_v3D_gl", GAN_ERROR_INCOMPATIBLE, "" );
   gan_err_test_ptr ( image->height >= mask->size, "image_convolve1Dy_v3D_gl",
                      GAN_ERROR_IMAGE_TOO_SMALL, "");

   if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_CLIP )
      dheight = image->height-mask->size+1;
   else if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_EXPAND )
      dheight = image->height+mask->size-1;
   else
      dheight = image->height;

   if ( dest == NULL )
      dest = gan_image_alloc ( GAN_GREY_LEVEL_IMAGE, image->type,
                               dheight, image->width );
   else if (    dest->format != GAN_GREY_LEVEL_IMAGE
             || dest->type   != image->type
             || dest->width  != image->width
             || dest->height != dheight )
      dest = gan_image_set_format_type_dims ( dest,
                                              GAN_GREY_LEVEL_IMAGE,
                                              image->type,
                                              dheight,
                                              image->width );

   if ( dest == NULL )
   {
      gan_err_register ( "image_convolve1Dy_v3D_gl", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   dest->offset_x = image->offset_x;
   if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_CLIP )
      dest->offset_y = image->offset_y+(int)mask->size/2;
   else if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_EXPAND )
      dest->offset_y = image->offset_y-(int)mask->size/2;
   else
      dest->offset_y = image->offset_y;

   switch ( image->type )
   {
      case GAN_FLOAT:
        gan_err_test_ptr ( (image->stride % sizeof(float)) == 0 &&
                           (dest->stride % sizeof(float)) == 0,
                           "image_convolve1Dy_v3D_gl", GAN_ERROR_INCOMPATIBLE,
                           "" );
        switch ( channel )
        {
           case GAN_X_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_f (
                          &gan_image_get_pixptr_vfield3D_f(image,0,j)->x,
                          image->stride/sizeof(float),
                          eEdgeBehaviour, GAN_FALSE,
                          gan_image_get_pixptr_gl_f(dest,0,j),
                          dest->stride/sizeof(float), mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_v3D_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

             break;

           case GAN_Y_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_f (
                          &gan_image_get_pixptr_vfield3D_f(image,0,j)->y,
                          image->stride/sizeof(float),
                          eEdgeBehaviour, GAN_FALSE,
                          gan_image_get_pixptr_gl_f(dest,0,j),
                          dest->stride/sizeof(float), mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_v3D_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

           break;

           case GAN_Z_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_f (
                          &gan_image_get_pixptr_vfield3D_f(image,0,j)->z,
                          image->stride/sizeof(float),
                          eEdgeBehaviour, GAN_FALSE,
                          gan_image_get_pixptr_gl_f(dest,0,j),
                          dest->stride/sizeof(float), mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_v3D_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

           break;

         default:
           gan_err_flush_trace();
           gan_err_register ( "image_convolve1Dy_v3D_gl",
                              GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
           return NULL;
      }
      break;

      case GAN_DOUBLE:
        gan_err_test_ptr ( (image->stride % sizeof(double)) == 0 &&
                           (dest->stride % sizeof(double)) == 0,
                           "image_convolve1Dy_v3D_gl", GAN_ERROR_INCOMPATIBLE,
                           "" );
        switch ( channel )
        {
           case GAN_X_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_d (
                          &gan_image_get_pixptr_vfield3D_d(image,0,j)->x,
                          image->stride/sizeof(double),
                          eEdgeBehaviour, GAN_FALSE,
                          gan_image_get_pixptr_gl_d(dest,0,j),
                          dest->stride/sizeof(double), mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_v3D_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

             break;

           case GAN_Y_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_d (
                          &gan_image_get_pixptr_vfield3D_d(image,0,j)->y,
                          image->stride/sizeof(double),
                          eEdgeBehaviour, GAN_FALSE,
                          gan_image_get_pixptr_gl_d(dest,0,j),
                          dest->stride/sizeof(double), mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_v3D_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

           break;

           case GAN_Z_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_d (
                          &gan_image_get_pixptr_vfield3D_d(image,0,j)->z,
                          image->stride/sizeof(double),
                          eEdgeBehaviour, GAN_FALSE,
                          gan_image_get_pixptr_gl_d(dest,0,j),
                          dest->stride/sizeof(double), mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_v3D_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

           break;

         default:
           gan_err_flush_trace();
           gan_err_register ( "image_convolve1Dy_v3D_gl",
                              GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
           return NULL;
      }
      break;

      case GAN_INT:
        gan_err_test_ptr ( (image->stride % sizeof(int)) == 0 &&
                           (dest->stride % sizeof(int)) == 0,
                           "image_convolve1Dy_v3D_gl", GAN_ERROR_INCOMPATIBLE,
                           "" );
        switch ( channel )
        {
           case GAN_X_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_i (
                          &gan_image_get_pixptr_vfield3D_i(image,0,j)->x,
                          image->stride/sizeof(int),
                          eEdgeBehaviour, GAN_FALSE,
                          gan_image_get_pixptr_gl_i(dest,0,j),
                          dest->stride/sizeof(int), mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_v3D_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }
 
             break;

           case GAN_Y_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_i (
                          &gan_image_get_pixptr_vfield3D_i(image,0,j)->y,
                          image->stride/sizeof(int),
                          eEdgeBehaviour, GAN_FALSE,
                          gan_image_get_pixptr_gl_i(dest,0,j),
                          dest->stride/sizeof(int), mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_v3D_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

             break;

           case GAN_Z_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_i (
                          &gan_image_get_pixptr_vfield3D_i(image,0,j)->z,
                          image->stride/sizeof(int),
                          eEdgeBehaviour, GAN_FALSE,
                          gan_image_get_pixptr_gl_i(dest,0,j),
                          dest->stride/sizeof(int), mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_v3D_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "image_convolve1Dy_v3D_gl",
                              GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "image_convolve1Dy_v3D_gl", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
        return NULL;
   }

   /* check if we aborted the loop early because of an error */
   if ( j >= 0 )
   {
      gan_err_register ( "image_convolve1Dy_v3D_gl", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* success */
   return dest;
}

static Gan_Image *
 image_convolve1Dy_rgb_gl ( const Gan_Image *image, Gan_ImageChannelType channel,
                            Gan_EdgeBehaviour eEdgeBehaviour,
                            Gan_Mask1D *mask, Gan_Image *dest )
{
   int j;
   unsigned int dheight;

   gan_err_test_ptr ( image->format == GAN_RGB_COLOUR_IMAGE,
                      "image_convolve1Dy_rgb_gl", GAN_ERROR_INCOMPATIBLE, "" );
   gan_err_test_ptr ( image->height >= mask->size, "image_convolve1Dy_rgb_gl",
                      GAN_ERROR_IMAGE_TOO_SMALL, "" );

   if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_CLIP )
      dheight = image->height-mask->size+1;
   else if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_EXPAND )
      dheight = image->height+mask->size-1;
   else
      dheight = image->height;

   if ( dest == NULL )
      dest = gan_image_alloc ( GAN_GREY_LEVEL_IMAGE, image->type,
                               dheight, image->width );
   else if (    dest->format != GAN_GREY_LEVEL_IMAGE
             || dest->type   != image->type
             || dest->width  != image->width
             || dest->height != dheight )
      dest = gan_image_set_format_type_dims ( dest,
                                              GAN_GREY_LEVEL_IMAGE,
                                              image->type,
                                              dheight,
                                              image->width );

   if ( dest == NULL )
   {
      gan_err_register ( "image_convolve1Dy_rgb_gl", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   dest->offset_x = image->offset_x;
   if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_CLIP )
      dest->offset_y = image->offset_y+(int)mask->size/2;
   else if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_EXPAND )
      dest->offset_y = image->offset_y-(int)mask->size/2;
   else
      dest->offset_y = image->offset_y;

   switch ( image->type )
   {
      case GAN_FLOAT:
        gan_err_test_ptr ( (image->stride % sizeof(float)) == 0 &&
                           (dest->stride % sizeof(float)) == 0,
                           "image_convolve1Dy_rgb_gl", GAN_ERROR_INCOMPATIBLE,
                           "" );
        switch ( channel )
        {
           case GAN_RED_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_f (
                          &gan_image_get_pixptr_rgb_f(image,0,j)->R,
                          image->stride/sizeof(float),
                          eEdgeBehaviour, GAN_FALSE,
                          gan_image_get_pixptr_gl_f(dest,0,j),
                          dest->stride/sizeof(float), mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_rgb_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

             break;

           case GAN_GREEN_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_f (
                          &gan_image_get_pixptr_rgb_f(image,0,j)->G,
                          image->stride/sizeof(float),
                          eEdgeBehaviour, GAN_FALSE,
                          gan_image_get_pixptr_gl_f(dest,0,j),
                          dest->stride/sizeof(float), mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_rgb_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

           break;

           case GAN_BLUE_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_f (
                          &gan_image_get_pixptr_rgb_f(image,0,j)->B,
                          image->stride/sizeof(float),
                          eEdgeBehaviour, GAN_FALSE,
                          gan_image_get_pixptr_gl_f(dest,0,j),
                          dest->stride/sizeof(float), mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_rgb_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

           break;

         default:
           gan_err_flush_trace();
           gan_err_register ( "image_convolve1Dy_rgb_gl",
                              GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
           return NULL;
      }
      break;

      case GAN_DOUBLE:
        gan_err_test_ptr ( (image->stride % sizeof(double)) == 0 &&
                           (dest->stride % sizeof(double)) == 0,
                           "image_convolve1Dy_rgb_gl", GAN_ERROR_INCOMPATIBLE,
                           "" );
        switch ( channel )
        {
           case GAN_RED_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_d (
                          &gan_image_get_pixptr_rgb_d(image,0,j)->R,
                          image->stride/sizeof(double),
                          eEdgeBehaviour, GAN_FALSE,
                          gan_image_get_pixptr_gl_d(dest,0,j),
                          dest->stride/sizeof(double), mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_rgb_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

             break;

           case GAN_GREEN_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_d (
                          &gan_image_get_pixptr_rgb_d(image,0,j)->G,
                          image->stride/sizeof(double),
                          eEdgeBehaviour, GAN_FALSE,
                          gan_image_get_pixptr_gl_d(dest,0,j),
                          dest->stride/sizeof(double), mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_rgb_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

           break;

           case GAN_BLUE_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_d (
                          &gan_image_get_pixptr_rgb_d(image,0,j)->B,
                          image->stride/sizeof(double),
                          eEdgeBehaviour, GAN_FALSE,
                          gan_image_get_pixptr_gl_d(dest,0,j),
                          dest->stride/sizeof(double), mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_rgb_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

           break;

         default:
           gan_err_flush_trace();
           gan_err_register ( "image_convolve1Dy_rgb_gl",
                              GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
           return NULL;
      }
      break;

      case GAN_INT:
        gan_err_test_ptr ( (image->stride % sizeof(int)) == 0 &&
                           (dest->stride % sizeof(int)) == 0,
                           "image_convolve1Dy_rgb_gl", GAN_ERROR_INCOMPATIBLE,
                           "" );
        switch ( channel )
        {
           case GAN_RED_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_i (
                          &gan_image_get_pixptr_rgb_i(image,0,j)->R,
                          image->stride/sizeof(int),
                          eEdgeBehaviour, GAN_FALSE,
                          gan_image_get_pixptr_gl_i(dest,0,j),
                          dest->stride/sizeof(int), mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_rgb_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }
 
             break;

           case GAN_GREEN_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_i (
                          &gan_image_get_pixptr_rgb_i(image,0,j)->G,
                          image->stride/sizeof(int),
                          eEdgeBehaviour, GAN_FALSE,
                          gan_image_get_pixptr_gl_i(dest,0,j),
                          dest->stride/sizeof(int), mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_rgb_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

             break;

           case GAN_BLUE_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_i (
                          &gan_image_get_pixptr_rgb_i(image,0,j)->B,
                          image->stride/sizeof(int),
                          eEdgeBehaviour, GAN_FALSE,
                          gan_image_get_pixptr_gl_i(dest,0,j),
                          dest->stride/sizeof(int), mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_rgb_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "image_convolve1Dy_rgb_gl",
                              GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;
        
      case GAN_UINT:
        gan_err_test_ptr ( (image->stride % sizeof(unsigned int)) == 0 &&
                           (dest->stride % sizeof(unsigned int)) == 0,
                           "image_convolve1Dy_rgb_gl", GAN_ERROR_INCOMPATIBLE,
                           "" );
        switch ( channel )
        {
           case GAN_RED_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_ui (
                         &gan_image_get_pixptr_rgb_ui(image,0,j)->R,
                         image->stride/sizeof(unsigned int),
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_ui(dest,0,j),
                         dest->stride/sizeof(unsigned int), mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_rgb_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }
 
             break;

           case GAN_GREEN_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_ui (
                         &gan_image_get_pixptr_rgb_ui(image,0,j)->G,
                         image->stride/sizeof(unsigned int),
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_ui(dest,0,j),
                         dest->stride/sizeof(unsigned int), mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_rgb_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

             break;

           case GAN_BLUE_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_ui (
                         &gan_image_get_pixptr_rgb_ui(image,0,j)->B,
                         image->stride/sizeof(unsigned int),
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_ui(dest,0,j),
                         dest->stride/sizeof(unsigned int), mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_rgb_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "image_convolve1Dy_rgb_gl",
                              GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;
        
      case GAN_UCHAR:
        gan_err_test_ptr ( (image->stride % sizeof(unsigned char)) == 0 &&
                           (dest->stride % sizeof(unsigned char)) == 0,
                           "image_convolve1Dy_rgb_gl", GAN_ERROR_INCOMPATIBLE,
                           "" );
        switch ( channel )
        {
           case GAN_RED_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_uc (
                          &gan_image_get_pixptr_rgb_uc(image,0,j)->R,
                          image->stride/sizeof(unsigned char),
                          eEdgeBehaviour, GAN_FALSE,
                          gan_image_get_pixptr_gl_uc(dest,0,j),
                          dest->stride/sizeof(unsigned char),
                          mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_rgb_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }
 
             break;

           case GAN_GREEN_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_uc (
                          &gan_image_get_pixptr_rgb_uc(image,0,j)->G,
                          image->stride/sizeof(unsigned char),
                          eEdgeBehaviour, GAN_FALSE,
                          gan_image_get_pixptr_gl_uc(dest,0,j),
                          dest->stride/sizeof(unsigned char),
                          mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_rgb_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

             break;

           case GAN_BLUE_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_uc (
                          &gan_image_get_pixptr_rgb_uc(image,0,j)->B,
                          image->stride/sizeof(unsigned char),
                          eEdgeBehaviour, GAN_FALSE,
                          gan_image_get_pixptr_gl_uc(dest,0,j),
                          dest->stride/sizeof(unsigned char),
                          mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_rgb_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "image_convolve1Dy_rgb_gl",
                                GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;
        
      case GAN_USHORT:
        gan_err_test_ptr ( (image->stride % sizeof(unsigned short)) == 0 &&
                           (dest->stride % sizeof(unsigned short)) == 0,
                           "image_convolve1Dy_rgb_gl", GAN_ERROR_INCOMPATIBLE,
                           "" );
        switch ( channel )
        {
           case GAN_RED_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_us (
                          &gan_image_get_pixptr_rgb_us(image,0,j)->R,
                          image->stride/sizeof(unsigned short),
                          eEdgeBehaviour, GAN_FALSE,
                          gan_image_get_pixptr_gl_us(dest,0,j),
                          dest->stride/sizeof(unsigned short),
                          mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_rgb_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }
 
             break;

           case GAN_GREEN_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_us (
                          &gan_image_get_pixptr_rgb_us(image,0,j)->G,
                          image->stride/sizeof(unsigned short),
                          eEdgeBehaviour, GAN_FALSE,
                          gan_image_get_pixptr_gl_us(dest,0,j),
                          dest->stride/sizeof(unsigned short),
                          mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_rgb_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

             break;

           case GAN_BLUE_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_us (
                          &gan_image_get_pixptr_rgb_us(image,0,j)->B,
                          image->stride/sizeof(unsigned short),
                          eEdgeBehaviour, GAN_FALSE,
                          gan_image_get_pixptr_gl_us(dest,0,j),
                          dest->stride/sizeof(unsigned short),
                          mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_rgb_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "image_convolve1Dy_rgb_gl",
                                GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;
        
      default:
        gan_err_flush_trace();
        gan_err_register ( "image_convolve1Dy_rgb_gl", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
        return NULL;
   }

   /* check if we aborted the loop early because of an error */
   if ( j >= 0 )
   {
      gan_err_register ( "image_convolve1Dy_rgb_gl", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* success */
   return dest;
}

static Gan_Image *
 image_convolve1Dy_rgba_gl ( const Gan_Image *image, Gan_ImageChannelType channel,
                             Gan_EdgeBehaviour eEdgeBehaviour,
                             Gan_Mask1D *mask, Gan_Image *dest )
{
   int j;
   unsigned int dheight;

   gan_err_test_ptr ( image->format == GAN_RGB_COLOUR_ALPHA_IMAGE,
                      "image_convolve1Dy_rgba_gl", GAN_ERROR_INCOMPATIBLE,
                      "" );
   gan_err_test_ptr ( image->height >= mask->size, "image_convolve1Dy_rgba_gl",
                      GAN_ERROR_IMAGE_TOO_SMALL, "" );

   if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_CLIP )
      dheight = image->height-mask->size+1;
   else if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_EXPAND )
      dheight = image->height+mask->size-1;
   else
      dheight = image->height;

   if ( dest == NULL )
      dest = gan_image_alloc ( GAN_GREY_LEVEL_IMAGE, image->type,
                               dheight, image->width );
   else if (    dest->format != GAN_GREY_LEVEL_IMAGE
             || dest->type   != image->type
             || dest->width  != image->width
             || dest->height != dheight )
      dest = gan_image_set_format_type_dims ( dest,
                                              GAN_GREY_LEVEL_IMAGE,
                                              image->type,
                                              dheight,
                                              image->width );

   if ( dest == NULL )
   {
      gan_err_register ( "image_convolve1Dy_rgba_gl", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   dest->offset_x = image->offset_x;
   if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_CLIP )
      dest->offset_y = image->offset_y+(int)mask->size/2;
   else if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_EXPAND )
      dest->offset_y = image->offset_y-(int)mask->size/2;
   else
      dest->offset_y = image->offset_y;

   switch ( image->type )
   {
      case GAN_FLOAT:
        gan_err_test_ptr ( (image->stride % sizeof(float)) == 0 &&
                           (dest->stride % sizeof(float)) == 0,
                           "image_convolve1Dy_rgba_gl", GAN_ERROR_INCOMPATIBLE,
                           "" );
        switch ( channel )
        {
           case GAN_RED_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_f (
                          &gan_image_get_pixptr_rgba_f(image,0,j)->R,
                          image->stride/sizeof(float),
                          eEdgeBehaviour, GAN_FALSE,
                          gan_image_get_pixptr_gl_f(dest,0,j),
                          dest->stride/sizeof(float), mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_rgba_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

             break;

           case GAN_GREEN_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_f (
                          &gan_image_get_pixptr_rgba_f(image,0,j)->G,
                          image->stride/sizeof(float),
                          eEdgeBehaviour, GAN_FALSE,
                          gan_image_get_pixptr_gl_f(dest,0,j),
                          dest->stride/sizeof(float), mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_rgba_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

           break;

           case GAN_BLUE_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_f (
                          &gan_image_get_pixptr_rgba_f(image,0,j)->B,
                          image->stride/sizeof(float),
                          eEdgeBehaviour, GAN_FALSE,
                          gan_image_get_pixptr_gl_f(dest,0,j),
                          dest->stride/sizeof(float), mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_rgba_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

           break;

           case GAN_ALPHA_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_f (
                          &gan_image_get_pixptr_rgba_f(image,0,j)->A,
                          image->stride/sizeof(float),
                          eEdgeBehaviour, GAN_TRUE,
                          gan_image_get_pixptr_gl_f(dest,0,j),
                          dest->stride/sizeof(float), mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_rgba_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

           break;

         default:
           gan_err_flush_trace();
           gan_err_register ( "image_convolve1Dy_rgba_gl",
                              GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
           return NULL;
      }
      break;

      case GAN_DOUBLE:
        gan_err_test_ptr ( (image->stride % sizeof(double)) == 0 &&
                           (dest->stride % sizeof(double)) == 0,
                           "image_convolve1Dy_rgba_gl", GAN_ERROR_INCOMPATIBLE,
                           "" );
        switch ( channel )
        {
           case GAN_RED_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_d (
                          &gan_image_get_pixptr_rgba_d(image,0,j)->R,
                          image->stride/sizeof(double),
                          eEdgeBehaviour, GAN_FALSE,
                          gan_image_get_pixptr_gl_d(dest,0,j),
                          dest->stride/sizeof(double), mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_rgb_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

             break;

           case GAN_GREEN_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_d (
                          &gan_image_get_pixptr_rgba_d(image,0,j)->G,
                          image->stride/sizeof(double),
                          eEdgeBehaviour, GAN_FALSE,
                          gan_image_get_pixptr_gl_d(dest,0,j),
                          dest->stride/sizeof(double), mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_rgba_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

           break;

           case GAN_BLUE_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_d (
                          &gan_image_get_pixptr_rgba_d(image,0,j)->B,
                          image->stride/sizeof(double),
                          eEdgeBehaviour, GAN_FALSE,
                          gan_image_get_pixptr_gl_d(dest,0,j),
                          dest->stride/sizeof(double), mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_rgba_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

           break;

           case GAN_ALPHA_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_d (
                          &gan_image_get_pixptr_rgba_d(image,0,j)->A,
                          image->stride/sizeof(double),
                          eEdgeBehaviour, GAN_TRUE,
                          gan_image_get_pixptr_gl_d(dest,0,j),
                          dest->stride/sizeof(double), mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_rgba_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

           break;

         default:
           gan_err_flush_trace();
           gan_err_register ( "image_convolve1Dy_rgba_gl",
                              GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
           return NULL;
      }
      break;

      case GAN_UINT:
        gan_err_test_ptr ( (image->stride % sizeof(unsigned int)) == 0 &&
                           (dest->stride % sizeof(unsigned int)) == 0,
                           "image_convolve1Dy_rgba_gl", GAN_ERROR_INCOMPATIBLE,
                           "" );
        switch ( channel )
        {
           case GAN_RED_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_ui (
                          &gan_image_get_pixptr_rgba_ui(image,0,j)->R,
                          image->stride/sizeof(unsigned int),
                          eEdgeBehaviour, GAN_FALSE,
                          gan_image_get_pixptr_gl_ui(dest,0,j),
                          dest->stride/sizeof(unsigned int),
                          mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_rgba_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }
 
             break;

           case GAN_GREEN_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_ui (
                          &gan_image_get_pixptr_rgba_ui(image,0,j)->G,
                          image->stride/sizeof(unsigned int),
                          eEdgeBehaviour, GAN_FALSE,
                          gan_image_get_pixptr_gl_ui(dest,0,j),
                          dest->stride/sizeof(unsigned int),
                          mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_rgba_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

             break;

           case GAN_BLUE_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_ui (
                          &gan_image_get_pixptr_rgba_ui(image,0,j)->B,
                          image->stride/sizeof(unsigned int),
                          eEdgeBehaviour, GAN_FALSE,
                          gan_image_get_pixptr_gl_ui(dest,0,j),
                          dest->stride/sizeof(unsigned int),
                          mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_rgba_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

             break;

           case GAN_ALPHA_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_ui (
                          &gan_image_get_pixptr_rgba_ui(image,0,j)->A,
                          image->stride/sizeof(unsigned int),
                          eEdgeBehaviour, GAN_TRUE,
                          gan_image_get_pixptr_gl_ui(dest,0,j),
                          dest->stride/sizeof(unsigned int),
                          mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_rgba_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "image_convolve1Dy_rgba_gl",
                                GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;
        
      case GAN_INT:
        gan_err_test_ptr ( (image->stride % sizeof(int)) == 0 &&
                           (dest->stride % sizeof(int)) == 0,
                           "image_convolve1Dy_rgba_gl", GAN_ERROR_INCOMPATIBLE,
                           "" );
        switch ( channel )
        {
           case GAN_RED_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_i (
                          &gan_image_get_pixptr_rgba_i(image,0,j)->R,
                          image->stride/sizeof(int),
                          eEdgeBehaviour, GAN_FALSE,
                          gan_image_get_pixptr_gl_i(dest,0,j),
                          dest->stride/sizeof(int), mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_rgba_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }
 
             break;

           case GAN_GREEN_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_i (
                          &gan_image_get_pixptr_rgba_i(image,0,j)->G,
                          image->stride/sizeof(int),
                          eEdgeBehaviour, GAN_FALSE,
                          gan_image_get_pixptr_gl_i(dest,0,j),
                          dest->stride/sizeof(int), mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_rgba_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

             break;

           case GAN_BLUE_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_i (
                          &gan_image_get_pixptr_rgba_i(image,0,j)->B,
                          image->stride/sizeof(int),
                          eEdgeBehaviour, GAN_FALSE,
                          gan_image_get_pixptr_gl_i(dest,0,j),
                          dest->stride/sizeof(int), mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_rgba_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

             break;

           case GAN_ALPHA_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_i (
                          &gan_image_get_pixptr_rgba_i(image,0,j)->A,
                          image->stride/sizeof(int),
                          eEdgeBehaviour, GAN_TRUE,
                          gan_image_get_pixptr_gl_i(dest,0,j),
                          dest->stride/sizeof(int), mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_rgba_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "image_convolve1Dy_rgba_gl",
                              GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;
        
      case GAN_UCHAR:
        gan_err_test_ptr ( (image->stride % sizeof(unsigned char)) == 0 &&
                           (dest->stride % sizeof(unsigned char)) == 0,
                           "image_convolve1Dy_rgba_gl", GAN_ERROR_INCOMPATIBLE,
                           "" );
        switch ( channel )
        {
           case GAN_RED_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_uc (
                          &gan_image_get_pixptr_rgba_uc(image,0,j)->R,
                          image->stride/sizeof(unsigned char),
                          eEdgeBehaviour, GAN_FALSE,
                          gan_image_get_pixptr_gl_uc(dest,0,j),
                          dest->stride/sizeof(unsigned char),
                          mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_rgba_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }
 
             break;

           case GAN_GREEN_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_uc (
                          &gan_image_get_pixptr_rgba_uc(image,0,j)->G,
                          image->stride/sizeof(unsigned char),
                          eEdgeBehaviour, GAN_FALSE,
                          gan_image_get_pixptr_gl_uc(dest,0,j),
                          dest->stride/sizeof(unsigned char),
                          mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_rgba_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

             break;

           case GAN_BLUE_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_uc (
                          &gan_image_get_pixptr_rgba_uc(image,0,j)->B,
                          image->stride/sizeof(unsigned char),
                          eEdgeBehaviour, GAN_FALSE,
                          gan_image_get_pixptr_gl_uc(dest,0,j),
                          dest->stride/sizeof(unsigned char),
                          mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_rgba_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

             break;

           case GAN_ALPHA_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_uc (
                          &gan_image_get_pixptr_rgba_uc(image,0,j)->A,
                          image->stride/sizeof(unsigned char),
                          eEdgeBehaviour, GAN_TRUE,
                          gan_image_get_pixptr_gl_uc(dest,0,j),
                          dest->stride/sizeof(unsigned char),
                          mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_rgba_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "image_convolve1Dy_rgba_gl",
                                GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;
        
      case GAN_USHORT:
        gan_err_test_ptr ( (image->stride % sizeof(unsigned short)) == 0 &&
                           (dest->stride % sizeof(unsigned short)) == 0,
                           "image_convolve1Dy_rgba_gl", GAN_ERROR_INCOMPATIBLE,
                           "" );
        switch ( channel )
        {
           case GAN_RED_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_us (
                          &gan_image_get_pixptr_rgba_us(image,0,j)->R,
                          image->stride/sizeof(unsigned short),
                          eEdgeBehaviour, GAN_FALSE,
                          gan_image_get_pixptr_gl_us(dest,0,j),
                          dest->stride/sizeof(unsigned short),
                          mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_rgba_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }
 
             break;

           case GAN_GREEN_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_us (
                          &gan_image_get_pixptr_rgba_us(image,0,j)->G,
                          image->stride/sizeof(unsigned short),
                          eEdgeBehaviour, GAN_FALSE,
                          gan_image_get_pixptr_gl_us(dest,0,j),
                          dest->stride/sizeof(unsigned short),
                          mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_rgba_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

             break;

           case GAN_BLUE_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_us (
                          &gan_image_get_pixptr_rgba_us(image,0,j)->B,
                          image->stride/sizeof(unsigned short),
                          eEdgeBehaviour, GAN_FALSE,
                          gan_image_get_pixptr_gl_us(dest,0,j),
                          dest->stride/sizeof(unsigned short),
                          mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_rgba_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

             break;

           case GAN_ALPHA_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_us (
                          &gan_image_get_pixptr_rgba_us(image,0,j)->A,
                          image->stride/sizeof(unsigned short),
                          eEdgeBehaviour, GAN_TRUE,
                          gan_image_get_pixptr_gl_us(dest,0,j),
                          dest->stride/sizeof(unsigned short),
                          mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_rgba_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "image_convolve1Dy_rgba_gl",
                                GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;
        
      default:
        gan_err_flush_trace();
        gan_err_register ( "image_convolve1Dy_rgba_gl", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
        return NULL;
   }

   /* check if we aborted the loop early because of an error */
   if ( j >= 0 )
   {
      gan_err_register ( "image_convolve1Dy_rgb_gl", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* success */
   return dest;
}

static Gan_Image *
 image_convolve1Dy_bgr_gl ( const Gan_Image *image, Gan_ImageChannelType channel,
                            Gan_EdgeBehaviour eEdgeBehaviour,
                            Gan_Mask1D *mask, Gan_Image *dest )
{
   int j;
   unsigned int dheight;

   gan_err_test_ptr ( image->format == GAN_BGR_COLOUR_IMAGE,
                      "image_convolve1Dy_bgr_gl", GAN_ERROR_INCOMPATIBLE, "" );
   gan_err_test_ptr ( image->height >= mask->size, "image_convolve1Dy_bgr_gl",
                      GAN_ERROR_IMAGE_TOO_SMALL, "" );

   if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_CLIP )
      dheight = image->height-mask->size+1;
   else if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_EXPAND )
      dheight = image->height+mask->size-1;
   else
      dheight = image->height;

   if ( dest == NULL )
      dest = gan_image_alloc ( GAN_GREY_LEVEL_IMAGE, image->type,
                               dheight, image->width );
   else if (    dest->format != GAN_GREY_LEVEL_IMAGE
             || dest->type   != image->type
             || dest->width  != image->width
             || dest->height != dheight )
      dest = gan_image_set_format_type_dims ( dest,
                                              GAN_GREY_LEVEL_IMAGE,
                                              image->type,
                                              dheight,
                                              image->width );

   if ( dest == NULL )
   {
      gan_err_register ( "image_convolve1Dy_bgr_gl", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   dest->offset_x = image->offset_x;
   if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_CLIP )
      dest->offset_y = image->offset_y+(int)mask->size/2;
   else if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_EXPAND )
      dest->offset_y = image->offset_y-(int)mask->size/2;
   else
      dest->offset_y = image->offset_y;

   switch ( image->type )
   {
      case GAN_FLOAT:
        gan_err_test_ptr ( (image->stride % sizeof(float)) == 0 &&
                           (dest->stride % sizeof(float)) == 0,
                           "image_convolve1Dy_bgr_gl", GAN_ERROR_INCOMPATIBLE,
                           "" );
        switch ( channel )
        {
           case GAN_RED_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_f (
                          &gan_image_get_pixptr_bgr_f(image,0,j)->R,
                          image->stride/sizeof(float),
                          eEdgeBehaviour, GAN_FALSE,
                          gan_image_get_pixptr_gl_f(dest,0,j),
                          dest->stride/sizeof(float), mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_bgr_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

             break;

           case GAN_GREEN_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_f (
                          &gan_image_get_pixptr_bgr_f(image,0,j)->G,
                          image->stride/sizeof(float),
                          eEdgeBehaviour, GAN_FALSE,
                          gan_image_get_pixptr_gl_f(dest,0,j),
                          dest->stride/sizeof(float), mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_bgr_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

           break;

           case GAN_BLUE_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_f (
                          &gan_image_get_pixptr_bgr_f(image,0,j)->B,
                          image->stride/sizeof(float),
                          eEdgeBehaviour, GAN_FALSE,
                          gan_image_get_pixptr_gl_f(dest,0,j),
                          dest->stride/sizeof(float), mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_bgr_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

           break;

         default:
           gan_err_flush_trace();
           gan_err_register ( "image_convolve1Dy_bgr_gl",
                              GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
           return NULL;
      }
      break;

      case GAN_DOUBLE:
        gan_err_test_ptr ( (image->stride % sizeof(double)) == 0 &&
                           (dest->stride % sizeof(double)) == 0,
                           "image_convolve1Dy_bgr_gl", GAN_ERROR_INCOMPATIBLE,
                           "" );
        switch ( channel )
        {
           case GAN_RED_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_d (
                          &gan_image_get_pixptr_bgr_d(image,0,j)->R,
                          image->stride/sizeof(double),
                          eEdgeBehaviour, GAN_FALSE,
                          gan_image_get_pixptr_gl_d(dest,0,j),
                          dest->stride/sizeof(double), mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_bgr_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

             break;

           case GAN_GREEN_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_d (
                          &gan_image_get_pixptr_bgr_d(image,0,j)->G,
                          image->stride/sizeof(double),
                          eEdgeBehaviour, GAN_FALSE,
                          gan_image_get_pixptr_gl_d(dest,0,j),
                          dest->stride/sizeof(double), mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_bgr_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

           break;

           case GAN_BLUE_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_d (
                          &gan_image_get_pixptr_bgr_d(image,0,j)->B,
                          image->stride/sizeof(double),
                          eEdgeBehaviour, GAN_FALSE,
                          gan_image_get_pixptr_gl_d(dest,0,j),
                          dest->stride/sizeof(double), mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_bgr_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

           break;

         default:
           gan_err_flush_trace();
           gan_err_register ( "image_convolve1Dy_bgr_gl",
                              GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
           return NULL;
      }
      break;

      case GAN_INT:
        gan_err_test_ptr ( (image->stride % sizeof(int)) == 0 &&
                           (dest->stride % sizeof(int)) == 0,
                           "image_convolve1Dy_bgr_gl", GAN_ERROR_INCOMPATIBLE,
                           "" );
        switch ( channel )
        {
           case GAN_RED_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_i (
                          &gan_image_get_pixptr_bgr_i(image,0,j)->R,
                          image->stride/sizeof(int),
                          eEdgeBehaviour, GAN_FALSE,
                          gan_image_get_pixptr_gl_i(dest,0,j),
                          dest->stride/sizeof(int), mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_bgr_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }
 
             break;

           case GAN_GREEN_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_i (
                          &gan_image_get_pixptr_bgr_i(image,0,j)->G,
                          image->stride/sizeof(int),
                          eEdgeBehaviour, GAN_FALSE,
                          gan_image_get_pixptr_gl_i(dest,0,j),
                          dest->stride/sizeof(int), mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_bgr_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

             break;

           case GAN_BLUE_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_i (
                          &gan_image_get_pixptr_bgr_i(image,0,j)->B,
                          image->stride/sizeof(int),
                          eEdgeBehaviour, GAN_FALSE,
                          gan_image_get_pixptr_gl_i(dest,0,j),
                          dest->stride/sizeof(int), mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_bgr_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "image_convolve1Dy_bgr_gl",
                              GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;
        
      case GAN_UINT:
        gan_err_test_ptr ( (image->stride % sizeof(unsigned int)) == 0 &&
                           (dest->stride % sizeof(unsigned int)) == 0,
                           "image_convolve1Dy_bgr_gl", GAN_ERROR_INCOMPATIBLE,
                           "" );
        switch ( channel )
        {
           case GAN_RED_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_ui (
                         &gan_image_get_pixptr_bgr_ui(image,0,j)->R,
                         image->stride/sizeof(unsigned int),
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_ui(dest,0,j),
                         dest->stride/sizeof(unsigned int), mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_bgr_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }
 
             break;

           case GAN_GREEN_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_ui (
                         &gan_image_get_pixptr_bgr_ui(image,0,j)->G,
                         image->stride/sizeof(unsigned int),
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_ui(dest,0,j),
                         dest->stride/sizeof(unsigned int), mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_bgr_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

             break;

           case GAN_BLUE_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_ui (
                         &gan_image_get_pixptr_bgr_ui(image,0,j)->B,
                         image->stride/sizeof(unsigned int),
                         eEdgeBehaviour, GAN_FALSE,
                         gan_image_get_pixptr_gl_ui(dest,0,j),
                         dest->stride/sizeof(unsigned int), mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_bgr_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "image_convolve1Dy_bgr_gl",
                              GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;
        
      case GAN_UCHAR:
        gan_err_test_ptr ( (image->stride % sizeof(unsigned char)) == 0 &&
                           (dest->stride % sizeof(unsigned char)) == 0,
                           "image_convolve1Dy_bgr_gl", GAN_ERROR_INCOMPATIBLE,
                           "" );
        switch ( channel )
        {
           case GAN_RED_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_uc (
                          &gan_image_get_pixptr_bgr_uc(image,0,j)->R,
                          image->stride/sizeof(unsigned char),
                          eEdgeBehaviour, GAN_FALSE,
                          gan_image_get_pixptr_gl_uc(dest,0,j),
                          dest->stride/sizeof(unsigned char),
                          mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_bgr_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }
 
             break;

           case GAN_GREEN_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_uc (
                          &gan_image_get_pixptr_bgr_uc(image,0,j)->G,
                          image->stride/sizeof(unsigned char),
                          eEdgeBehaviour, GAN_FALSE,
                          gan_image_get_pixptr_gl_uc(dest,0,j),
                          dest->stride/sizeof(unsigned char),
                          mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_bgr_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

             break;

           case GAN_BLUE_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_uc (
                          &gan_image_get_pixptr_bgr_uc(image,0,j)->B,
                          image->stride/sizeof(unsigned char),
                          eEdgeBehaviour, GAN_FALSE,
                          gan_image_get_pixptr_gl_uc(dest,0,j),
                          dest->stride/sizeof(unsigned char),
                          mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_bgr_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "image_convolve1Dy_bgr_gl",
                                GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;
        
      case GAN_USHORT:
        gan_err_test_ptr ( (image->stride % sizeof(unsigned short)) == 0 &&
                           (dest->stride % sizeof(unsigned short)) == 0,
                           "image_convolve1Dy_bgr_gl", GAN_ERROR_INCOMPATIBLE,
                           "" );
        switch ( channel )
        {
           case GAN_RED_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_us (
                          &gan_image_get_pixptr_bgr_us(image,0,j)->R,
                          image->stride/sizeof(unsigned short),
                          eEdgeBehaviour, GAN_FALSE,
                          gan_image_get_pixptr_gl_us(dest,0,j),
                          dest->stride/sizeof(unsigned short),
                          mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_bgr_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }
 
             break;

           case GAN_GREEN_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_us (
                          &gan_image_get_pixptr_bgr_us(image,0,j)->G,
                          image->stride/sizeof(unsigned short),
                          eEdgeBehaviour, GAN_FALSE,
                          gan_image_get_pixptr_gl_us(dest,0,j),
                          dest->stride/sizeof(unsigned short),
                          mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_bgr_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

             break;

           case GAN_BLUE_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_us (
                          &gan_image_get_pixptr_bgr_us(image,0,j)->B,
                          image->stride/sizeof(unsigned short),
                          eEdgeBehaviour, GAN_FALSE,
                          gan_image_get_pixptr_gl_us(dest,0,j),
                          dest->stride/sizeof(unsigned short),
                          mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_bgr_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "image_convolve1Dy_bgr_gl",
                                GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;
        
      default:
        gan_err_flush_trace();
        gan_err_register ( "image_convolve1Dy_bgr_gl", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
        return NULL;
   }

   /* check if we aborted the loop early because of an error */
   if ( j >= 0 )
   {
      gan_err_register ( "image_convolve1Dy_bgr_gl", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* success */
   return dest;
}

static Gan_Image *
 image_convolve1Dy_bgra_gl ( const Gan_Image *image, Gan_ImageChannelType channel,
                             Gan_EdgeBehaviour eEdgeBehaviour,
                             Gan_Mask1D *mask, Gan_Image *dest )
{
   int j;
   unsigned int dheight;

   gan_err_test_ptr ( image->format == GAN_BGR_COLOUR_ALPHA_IMAGE,
                      "image_convolve1Dy_bgra_gl", GAN_ERROR_INCOMPATIBLE,
                      "" );
   gan_err_test_ptr ( image->height >= mask->size, "image_convolve1Dy_bgra_gl",
                      GAN_ERROR_IMAGE_TOO_SMALL, "" );

   if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_CLIP )
      dheight = image->height-mask->size+1;
   else if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_EXPAND )
      dheight = image->height+mask->size-1;
   else
      dheight = image->height;

   if ( dest == NULL )
      dest = gan_image_alloc ( GAN_GREY_LEVEL_IMAGE, image->type,
                               dheight, image->width );
   else if (    dest->format != GAN_GREY_LEVEL_IMAGE
             || dest->type   != image->type
             || dest->width  != image->width
             || dest->height != dheight )
      dest = gan_image_set_format_type_dims ( dest,
                                              GAN_GREY_LEVEL_IMAGE,
                                              image->type,
                                              dheight,
                                              image->width );

   if ( dest == NULL )
   {
      gan_err_register ( "image_convolve1Dy_bgra_gl", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   dest->offset_x = image->offset_x;
   if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_CLIP )
      dest->offset_y = image->offset_y+(int)mask->size/2;
   else if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_EXPAND )
      dest->offset_y = image->offset_y-(int)mask->size/2;
   else
      dest->offset_y = image->offset_y;

   switch ( image->type )
   {
      case GAN_FLOAT:
        gan_err_test_ptr ( (image->stride % sizeof(float)) == 0 &&
                           (dest->stride % sizeof(float)) == 0,
                           "image_convolve1Dy_bgra_gl", GAN_ERROR_INCOMPATIBLE,
                           "" );
        switch ( channel )
        {
           case GAN_RED_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_f (
                          &gan_image_get_pixptr_bgra_f(image,0,j)->R,
                          image->stride/sizeof(float),
                          eEdgeBehaviour, GAN_FALSE,
                          gan_image_get_pixptr_gl_f(dest,0,j),
                          dest->stride/sizeof(float), mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_bgra_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

             break;

           case GAN_GREEN_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_f (
                          &gan_image_get_pixptr_bgra_f(image,0,j)->G,
                          image->stride/sizeof(float),
                          eEdgeBehaviour, GAN_FALSE,
                          gan_image_get_pixptr_gl_f(dest,0,j),
                          dest->stride/sizeof(float), mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_bgra_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

           break;

           case GAN_BLUE_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_f (
                          &gan_image_get_pixptr_bgra_f(image,0,j)->B,
                          image->stride/sizeof(float),
                          eEdgeBehaviour, GAN_FALSE,
                          gan_image_get_pixptr_gl_f(dest,0,j),
                          dest->stride/sizeof(float), mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_bgra_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

           break;

           case GAN_ALPHA_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_f (
                          &gan_image_get_pixptr_bgra_f(image,0,j)->A,
                          image->stride/sizeof(float),
                          eEdgeBehaviour, GAN_TRUE,
                          gan_image_get_pixptr_gl_f(dest,0,j),
                          dest->stride/sizeof(float), mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_bgra_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

           break;

         default:
           gan_err_flush_trace();
           gan_err_register ( "image_convolve1Dy_bgra_gl",
                              GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
           return NULL;
      }
      break;

      case GAN_DOUBLE:
        gan_err_test_ptr ( (image->stride % sizeof(double)) == 0 &&
                           (dest->stride % sizeof(double)) == 0,
                           "image_convolve1Dy_bgra_gl", GAN_ERROR_INCOMPATIBLE,
                           "" );
        switch ( channel )
        {
           case GAN_RED_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_d (
                          &gan_image_get_pixptr_bgra_d(image,0,j)->R,
                          image->stride/sizeof(double),
                          eEdgeBehaviour, GAN_FALSE,
                          gan_image_get_pixptr_gl_d(dest,0,j),
                          dest->stride/sizeof(double), mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_bgr_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

             break;

           case GAN_GREEN_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_d (
                          &gan_image_get_pixptr_bgra_d(image,0,j)->G,
                          image->stride/sizeof(double),
                          eEdgeBehaviour, GAN_FALSE,
                          gan_image_get_pixptr_gl_d(dest,0,j),
                          dest->stride/sizeof(double), mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_bgra_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

           break;

           case GAN_BLUE_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_d (
                          &gan_image_get_pixptr_bgra_d(image,0,j)->B,
                          image->stride/sizeof(double),
                          eEdgeBehaviour, GAN_FALSE,
                          gan_image_get_pixptr_gl_d(dest,0,j),
                          dest->stride/sizeof(double), mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_bgra_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

           break;

           case GAN_ALPHA_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_d (
                          &gan_image_get_pixptr_bgra_d(image,0,j)->A,
                          image->stride/sizeof(double),
                          eEdgeBehaviour, GAN_TRUE,
                          gan_image_get_pixptr_gl_d(dest,0,j),
                          dest->stride/sizeof(double), mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_bgra_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

           break;

         default:
           gan_err_flush_trace();
           gan_err_register ( "image_convolve1Dy_bgra_gl",
                              GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
           return NULL;
      }
      break;

      case GAN_UINT:
        gan_err_test_ptr ( (image->stride % sizeof(unsigned int)) == 0 &&
                           (dest->stride % sizeof(unsigned int)) == 0,
                           "image_convolve1Dy_bgra_gl", GAN_ERROR_INCOMPATIBLE,
                           "" );
        switch ( channel )
        {
           case GAN_RED_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_ui (
                          &gan_image_get_pixptr_bgra_ui(image,0,j)->R,
                          image->stride/sizeof(unsigned int),
                          eEdgeBehaviour, GAN_FALSE,
                          gan_image_get_pixptr_gl_ui(dest,0,j),
                          dest->stride/sizeof(unsigned int),
                          mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_bgra_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }
 
             break;

           case GAN_GREEN_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_ui (
                          &gan_image_get_pixptr_bgra_ui(image,0,j)->G,
                          image->stride/sizeof(unsigned int),
                          eEdgeBehaviour, GAN_FALSE,
                          gan_image_get_pixptr_gl_ui(dest,0,j),
                          dest->stride/sizeof(unsigned int),
                          mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_bgra_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

             break;

           case GAN_BLUE_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_ui (
                          &gan_image_get_pixptr_bgra_ui(image,0,j)->B,
                          image->stride/sizeof(unsigned int),
                          eEdgeBehaviour, GAN_FALSE,
                          gan_image_get_pixptr_gl_ui(dest,0,j),
                          dest->stride/sizeof(unsigned int),
                          mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_bgra_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

             break;

           case GAN_ALPHA_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_ui (
                          &gan_image_get_pixptr_bgra_ui(image,0,j)->A,
                          image->stride/sizeof(unsigned int),
                          eEdgeBehaviour, GAN_TRUE,
                          gan_image_get_pixptr_gl_ui(dest,0,j),
                          dest->stride/sizeof(unsigned int),
                          mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_bgra_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "image_convolve1Dy_bgra_gl",
                                GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;
        
      case GAN_INT:
        gan_err_test_ptr ( (image->stride % sizeof(int)) == 0 &&
                           (dest->stride % sizeof(int)) == 0,
                           "image_convolve1Dy_bgra_gl", GAN_ERROR_INCOMPATIBLE,
                           "" );
        switch ( channel )
        {
           case GAN_RED_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_i (
                          &gan_image_get_pixptr_bgra_i(image,0,j)->R,
                          image->stride/sizeof(int),
                          eEdgeBehaviour, GAN_FALSE,
                          gan_image_get_pixptr_gl_i(dest,0,j),
                          dest->stride/sizeof(int), mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_bgra_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }
 
             break;

           case GAN_GREEN_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_i (
                          &gan_image_get_pixptr_bgra_i(image,0,j)->G,
                          image->stride/sizeof(int),
                          eEdgeBehaviour, GAN_FALSE,
                          gan_image_get_pixptr_gl_i(dest,0,j),
                          dest->stride/sizeof(int), mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_bgra_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

             break;

           case GAN_BLUE_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_i (
                          &gan_image_get_pixptr_bgra_i(image,0,j)->B,
                          image->stride/sizeof(int),
                          eEdgeBehaviour, GAN_FALSE,
                          gan_image_get_pixptr_gl_i(dest,0,j),
                          dest->stride/sizeof(int), mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_bgra_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

             break;

           case GAN_ALPHA_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_i (
                          &gan_image_get_pixptr_bgra_i(image,0,j)->A,
                          image->stride/sizeof(int),
                          eEdgeBehaviour, GAN_TRUE,
                          gan_image_get_pixptr_gl_i(dest,0,j),
                          dest->stride/sizeof(int), mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_bgra_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "image_convolve1Dy_bgra_gl",
                              GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;
        
      case GAN_UCHAR:
        gan_err_test_ptr ( (image->stride % sizeof(unsigned char)) == 0 &&
                           (dest->stride % sizeof(unsigned char)) == 0,
                           "image_convolve1Dy_bgra_gl", GAN_ERROR_INCOMPATIBLE,
                           "" );
        switch ( channel )
        {
           case GAN_RED_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_uc (
                          &gan_image_get_pixptr_bgra_uc(image,0,j)->R,
                          image->stride/sizeof(unsigned char),
                          eEdgeBehaviour, GAN_FALSE,
                          gan_image_get_pixptr_gl_uc(dest,0,j),
                          dest->stride/sizeof(unsigned char),
                          mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_bgra_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }
 
             break;

           case GAN_GREEN_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_uc (
                          &gan_image_get_pixptr_bgra_uc(image,0,j)->G,
                          image->stride/sizeof(unsigned char),
                          eEdgeBehaviour, GAN_FALSE,
                          gan_image_get_pixptr_gl_uc(dest,0,j),
                          dest->stride/sizeof(unsigned char),
                          mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_bgra_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

             break;

           case GAN_BLUE_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_uc (
                          &gan_image_get_pixptr_bgra_uc(image,0,j)->B,
                          image->stride/sizeof(unsigned char),
                          eEdgeBehaviour, GAN_FALSE,
                          gan_image_get_pixptr_gl_uc(dest,0,j),
                          dest->stride/sizeof(unsigned char),
                          mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_bgra_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

             break;

           case GAN_ALPHA_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_uc (
                          &gan_image_get_pixptr_bgra_uc(image,0,j)->A,
                          image->stride/sizeof(unsigned char),
                          eEdgeBehaviour, GAN_TRUE,
                          gan_image_get_pixptr_gl_uc(dest,0,j),
                          dest->stride/sizeof(unsigned char),
                          mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_bgra_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "image_convolve1Dy_bgra_gl",
                                GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;
        
      case GAN_USHORT:
        gan_err_test_ptr ( (image->stride % sizeof(unsigned short)) == 0 &&
                           (dest->stride % sizeof(unsigned short)) == 0,
                           "image_convolve1Dy_bgra_gl", GAN_ERROR_INCOMPATIBLE,
                           "" );
        switch ( channel )
        {
           case GAN_RED_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_us (
                          &gan_image_get_pixptr_bgra_us(image,0,j)->R,
                          image->stride/sizeof(unsigned short),
                          eEdgeBehaviour, GAN_FALSE,
                          gan_image_get_pixptr_gl_us(dest,0,j),
                          dest->stride/sizeof(unsigned short),
                          mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_bgra_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }
 
             break;

           case GAN_GREEN_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_us (
                          &gan_image_get_pixptr_bgra_us(image,0,j)->G,
                          image->stride/sizeof(unsigned short),
                          eEdgeBehaviour, GAN_FALSE,
                          gan_image_get_pixptr_gl_us(dest,0,j),
                          dest->stride/sizeof(unsigned short),
                          mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_bgra_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

             break;

           case GAN_BLUE_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_us (
                          &gan_image_get_pixptr_bgra_us(image,0,j)->B,
                          image->stride/sizeof(unsigned short),
                          eEdgeBehaviour, GAN_FALSE,
                          gan_image_get_pixptr_gl_us(dest,0,j),
                          dest->stride/sizeof(unsigned short),
                          mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_bgra_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

             break;

           case GAN_ALPHA_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_us (
                          &gan_image_get_pixptr_bgra_us(image,0,j)->A,
                          image->stride/sizeof(unsigned short),
                          eEdgeBehaviour, GAN_TRUE,
                          gan_image_get_pixptr_gl_us(dest,0,j),
                          dest->stride/sizeof(unsigned short),
                          mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_bgra_gl",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "image_convolve1Dy_bgra_gl",
                                GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;
        
      default:
        gan_err_flush_trace();
        gan_err_register ( "image_convolve1Dy_bgra_gl", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
        return NULL;
   }

   /* check if we aborted the loop early because of an error */
   if ( j >= 0 )
   {
      gan_err_register ( "image_convolve1Dy_bgr_gl", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* success */
   return dest;
}

static Gan_Image *
 image_convolve1Dy_gl_v3D ( const Gan_Image *image, Gan_ImageChannelType channel,
                            Gan_EdgeBehaviour eEdgeBehaviour, Gan_Mask1D *mask,
                            Gan_Image *dest )
{
   int j;
   unsigned int dheight;

   gan_err_test_ptr ( image->format == GAN_GREY_LEVEL_IMAGE,
                      "image_convolve1Dy_gl_v3D", GAN_ERROR_INCOMPATIBLE,
                      "" );
   gan_err_test_ptr ( image->height >= mask->size, "image_convolve1Dy_gl_v3D",
                      GAN_ERROR_IMAGE_TOO_SMALL, "" );

   if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_CLIP )
      dheight = image->height-mask->size+1;
   else if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_EXPAND )
      dheight = image->height+mask->size-1;
   else
      dheight = image->height;

   if ( dest == NULL )
      dest = gan_image_alloc ( GAN_VECTOR_FIELD_3D, image->type,
                               dheight, image->width );
   else if (    dest->format != GAN_VECTOR_FIELD_3D
             || dest->type   != image->type
             || dest->width  != image->width
             || dest->height != dheight )
      dest = gan_image_set_format_type_dims ( dest,
                                              GAN_VECTOR_FIELD_3D,
                                              image->type,
                                              dheight,
                                              image->width );

   if ( dest == NULL )
   {
      gan_err_register ( "image_convolve1Dy_gl_v3D", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   dest->offset_x = image->offset_x;
   if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_CLIP )
      dest->offset_y = image->offset_y+(int)mask->size/2;
   else if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_EXPAND )
      dest->offset_y = image->offset_y-(int)mask->size/2;
   else
      dest->offset_y = image->offset_y;

   switch ( image->type )
   {
      case GAN_FLOAT:
        gan_err_test_ptr ( (image->stride % sizeof(float)) == 0 &&
                           (dest->stride % sizeof(float)) == 0,
                           "image_convolve1Dy_gl_v3D", GAN_ERROR_INCOMPATIBLE,
                           "" );
        switch ( channel )
        {
           case GAN_X_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_f (
                          gan_image_get_pixptr_gl_f(image,0,j),
                          image->stride/sizeof(float),
                          eEdgeBehaviour, GAN_FALSE,
                          &gan_image_get_pixptr_vfield3D_f(dest,0,j)->x,
                          dest->stride/sizeof(float), mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_gl_v3D",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

             break;

           case GAN_Y_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_f (
                          gan_image_get_pixptr_gl_f(image,0,j),
                          image->stride/sizeof(float),
                          eEdgeBehaviour, GAN_FALSE,
                          &gan_image_get_pixptr_vfield3D_f(dest,0,j)->y,
                          dest->stride/sizeof(float), mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_gl_v3D",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

             break;

           case GAN_Z_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_f (
                          gan_image_get_pixptr_gl_f(image,0,j),
                          image->stride/sizeof(float),
                          eEdgeBehaviour, GAN_FALSE,
                          &gan_image_get_pixptr_vfield3D_f(dest,0,j)->z,
                          dest->stride/sizeof(float), mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_gl_v3D",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "image_convolve1Dy_gl_v3D",
                                GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;

      case GAN_DOUBLE:
        gan_err_test_ptr ( (image->stride % sizeof(double)) == 0 &&
                           (dest->stride % sizeof(double)) == 0,
                           "image_convolve1Dy_gl_v3D", GAN_ERROR_INCOMPATIBLE,
                           "" );
        switch ( channel )
        {
           case GAN_X_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_d (
                          gan_image_get_pixptr_gl_d(image,0,j),
                          image->stride/sizeof(double),
                          eEdgeBehaviour, GAN_FALSE,
                          &gan_image_get_pixptr_vfield3D_d(dest,0,j)->x,
                          dest->stride/sizeof(double), mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_gl_v3D",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

             break;

           case GAN_Y_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_d (
                          gan_image_get_pixptr_gl_d(image,0,j),
                          image->stride/sizeof(double),
                          eEdgeBehaviour, GAN_FALSE,
                          &gan_image_get_pixptr_vfield3D_d(dest,0,j)->y,
                          dest->stride/sizeof(double), mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_gl_v3D",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

             break;

           case GAN_Z_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_d (
                          gan_image_get_pixptr_gl_d(image,0,j),
                          image->stride/sizeof(double),
                          eEdgeBehaviour, GAN_FALSE,
                          &gan_image_get_pixptr_vfield3D_d(dest,0,j)->z,
                          dest->stride/sizeof(double), mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_gl_v3D",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

             break;

           default:
             gan_err_flush_trace();
             gan_err_register ( "image_convolve1Dy_gl_v3D",
                                GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
             return NULL;
        }
        break;

      case GAN_INT:
        gan_err_test_ptr ( (image->stride % sizeof(int)) == 0 &&
                           (dest->stride % sizeof(int)) == 0,
                           "image_convolve1Dy_gl_v3D", GAN_ERROR_INCOMPATIBLE,
                           "" );
        switch ( channel )
        {
           case GAN_X_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_i (
                          gan_image_get_pixptr_gl_i(image,0,j),
                          image->stride/sizeof(int),
                          eEdgeBehaviour, GAN_FALSE,
                          &gan_image_get_pixptr_vfield3D_i(dest,0,j)->x,
                          dest->stride/sizeof(int), mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_gl_v3D",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

           break;

           case GAN_Y_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_i (
                          gan_image_get_pixptr_gl_i(image,0,j),
                          image->stride/sizeof(int),
                          eEdgeBehaviour, GAN_FALSE,
                          &gan_image_get_pixptr_vfield3D_i(dest,0,j)->y,
                          dest->stride/sizeof(int), mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_gl_v3D",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

           break;

           case GAN_Z_CHANNEL:
             for ( j = (int)dest->width-1; j >= 0; j-- )
                if ( !gan_convolve1D_i (
                          gan_image_get_pixptr_gl_i(image,0,j),
                          image->stride/sizeof(int),
                          eEdgeBehaviour, GAN_FALSE,
                          &gan_image_get_pixptr_vfield3D_i(dest,0,j)->z,
                          dest->stride/sizeof(int), mask, (int)dheight ) )
                {
                   gan_err_register ( "image_convolve1Dy_gl_v3D",
                                      GAN_ERROR_FAILURE, "" );
                   return NULL;
                }

           break;

         default:
           gan_err_flush_trace();
           gan_err_register ( "image_convolve1Dy_gl_v3D",
                              GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
           return NULL;
      }
      break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "image_convolve1Dy_gl_v3D", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
        return NULL;
   }

   /* check if we aborted the loop early because of an error */
   if ( j >= 0 )
   {
      gan_err_register ( "image_convolve1Dy_gl_v3D", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* success */
   return dest;
}

static Gan_Image *
 image_convolve1Dy_v3D_v3D ( const Gan_Image *image,
                             Gan_EdgeBehaviour eEdgeBehaviour,
                             Gan_Mask1D *mask, Gan_Image *dest )
{
   int j;
   unsigned int dheight;

   gan_err_test_ptr ( image->format == GAN_VECTOR_FIELD_3D,
                      "image_convolve1Dy_v3D_v3D", GAN_ERROR_INCOMPATIBLE,
                      "" );
   gan_err_test_ptr ( image->height >= mask->size, "image_convolve1Dy_v3D_v3D",
                      GAN_ERROR_IMAGE_TOO_SMALL, "in vertical direction");

   if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_CLIP )
      dheight = image->height-mask->size+1;
   else if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_EXPAND )
      dheight = image->height+mask->size-1;
   else
      dheight = image->height;

   if ( dest == NULL )
      dest = gan_image_alloc ( GAN_VECTOR_FIELD_3D, image->type,
                               dheight, image->width );
   else if (    dest->format != GAN_VECTOR_FIELD_3D
             || dest->type   != image->type
             || dest->width  != image->width
             || dest->height != dheight )
      dest = gan_image_set_format_type_dims ( dest,
                                              GAN_VECTOR_FIELD_3D,
                                              image->type,
                                              dheight,
                                              image->width );

   if ( dest == NULL )
   {
      gan_err_register ( "image_convolve1Dy_v3D_v3D", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   dest->offset_x = image->offset_x;
   if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_CLIP )
      dest->offset_y = image->offset_y+(int)mask->size/2;
   else if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_EXPAND )
      dest->offset_y = image->offset_y-(int)mask->size/2;
   else
      dest->offset_y = image->offset_y;

   switch ( image->type )
   {
      case GAN_FLOAT:
        gan_err_test_ptr ( (image->stride % sizeof(float)) == 0 &&
                           (dest->stride % sizeof(float)) == 0,
                           "image_convolve1Dy_v3D_v3D", GAN_ERROR_INCOMPATIBLE,
                           "" );
        for ( j = (int)dest->width-1; j >= 0; j-- )
           if ( !gan_convolve1D_f (
                     &gan_image_get_pixptr_vfield3D_f(image,0,j)->x,
                     image->stride/sizeof(float),
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_vfield3D_f(dest,0,j)->x,
                     dest->stride/sizeof(float), mask, (int)dheight ) ||
                !gan_convolve1D_f (
                     &gan_image_get_pixptr_vfield3D_f(image,0,j)->y,
                     image->stride/sizeof(float),
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_vfield3D_f(dest,0,j)->y,
                     dest->stride/sizeof(float), mask, (int)dheight ) ||
                !gan_convolve1D_f (
                     &gan_image_get_pixptr_vfield3D_f(image,0,j)->z,
                     image->stride/sizeof(float),
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_vfield3D_f(dest,0,j)->z,
                     dest->stride/sizeof(float), mask, (int)dheight ) )
           {
              gan_err_register ( "image_convolve1Dy_v3D_v3D",
                                 GAN_ERROR_FAILURE, "" );
              return NULL;
           }

        break;

      case GAN_DOUBLE:
        gan_err_test_ptr ( (image->stride % sizeof(double)) == 0 &&
                           (dest->stride % sizeof(double)) == 0,
                           "image_convolve1Dy_v3D_v3D", GAN_ERROR_INCOMPATIBLE,
                           "" );
        for ( j = (int)dest->width-1; j >= 0; j-- )
           if ( !gan_convolve1D_d (
                     &gan_image_get_pixptr_vfield3D_d(image,0,j)->x,
                     image->stride/sizeof(double),
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_vfield3D_d(dest,0,j)->x,
                     dest->stride/sizeof(double), mask, (int)dheight ) ||
                !gan_convolve1D_d (
                     &gan_image_get_pixptr_vfield3D_d(image,0,j)->y,
                     image->stride/sizeof(double),
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_vfield3D_d(dest,0,j)->y,
                     dest->stride/sizeof(double), mask, (int)dheight ) ||
                !gan_convolve1D_d (
                     &gan_image_get_pixptr_vfield3D_d(image,0,j)->z,
                     image->stride/sizeof(double),
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_vfield3D_d(dest,0,j)->z,
                     dest->stride/sizeof(double), mask, (int)dheight ) )
           {
              gan_err_register ( "image_convolve1Dy_v3D_v3D",
                                 GAN_ERROR_FAILURE, "" );
              return NULL;
           }

        break;

      case GAN_INT:
        gan_err_test_ptr ( (image->stride % sizeof(int)) == 0 &&
                           (dest->stride % sizeof(int)) == 0,
                           "image_convolve1Dy_v3D_v3D", GAN_ERROR_INCOMPATIBLE,
                           "" );
        for ( j = (int)dest->width-1; j >= 0; j-- )
           if ( !gan_convolve1D_i (
                     &gan_image_get_pixptr_vfield3D_i(image,0,j)->x,
                     image->stride/sizeof(int),
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_vfield3D_i(dest,0,j)->x,
                     dest->stride/sizeof(int), mask, (int)dheight ) ||
                !gan_convolve1D_i (
                     &gan_image_get_pixptr_vfield3D_i(image,0,j)->y,
                     image->stride/sizeof(int),
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_vfield3D_i(dest,0,j)->y,
                     dest->stride/sizeof(int), mask, (int)dheight ) ||
                !gan_convolve1D_i (
                     &gan_image_get_pixptr_vfield3D_i(image,0,j)->z,
                     image->stride/sizeof(int),
                     eEdgeBehaviour, GAN_FALSE,
                     &gan_image_get_pixptr_vfield3D_i(dest,0,j)->z,
                     dest->stride/sizeof(int), mask, (int)dheight ) )
           {
              gan_err_register ( "image_convolve1Dy_v3D_v3D",
                                 GAN_ERROR_FAILURE, "" );
              return NULL;
           }

        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "image_convolve1Dy_v3D_v3D", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
        return NULL;
   }

   /* check if we aborted the loop early because of an error */
   if ( j >= 0 )
   {
      gan_err_register ( "image_convolve1Dy_v3D_v3D", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* success */
   return dest;
}

static Gan_Image *
 image_convolve1Dy_rgb_rgb ( const Gan_Image *image,
                             Gan_EdgeBehaviour eEdgeBehaviour,
                             Gan_Mask1D *mask, Gan_Image *dest )
{
   int j;
   unsigned int dheight;

   gan_err_test_ptr ( image->format == GAN_RGB_COLOUR_IMAGE,
                      "image_convolve1Dy_rgb_rgb", GAN_ERROR_INCOMPATIBLE,
                      "" );
   gan_err_test_ptr ( image->height >= mask->size, "image_convolve1Dy_rgb_rgb",
                      GAN_ERROR_IMAGE_TOO_SMALL, "" );

   if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_CLIP )
      dheight = image->height-mask->size+1;
   else if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_EXPAND )
      dheight = image->height+mask->size-1;
   else
      dheight = image->height;

   if ( dest == NULL )
      dest = gan_image_alloc ( GAN_RGB_COLOUR_IMAGE, image->type,
                               dheight, image->width );
   else if (    dest->format != GAN_RGB_COLOUR_IMAGE
             || dest->type   != image->type
             || dest->width  != image->width
             || dest->height != dheight )
      dest = gan_image_set_format_type_dims ( dest,
                                              GAN_RGB_COLOUR_IMAGE,
                                              image->type,
                                              dheight,
                                              image->width );

   if ( dest == NULL )
   {
      gan_err_register ( "image_convolve1Dy_rgb_rgb", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   dest->offset_x = image->offset_x;
   if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_CLIP )
      dest->offset_y = image->offset_y+(int)mask->size/2;
   else if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_EXPAND )
      dest->offset_y = image->offset_y-(int)mask->size/2;
   else
      dest->offset_y = image->offset_y;

   switch ( image->type )
   {
      case GAN_FLOAT:
        gan_err_test_ptr ( (image->stride % sizeof(float)) == 0 &&
                           (dest->stride % sizeof(float)) == 0,
                           "image_convolve1Dy_rgb_rgb", GAN_ERROR_INCOMPATIBLE,
                           "" );
#if 1 /*__APPLE__*/
        for ( j = (int)dest->width-1; j >= 0; j-- )
           if ( !gan_convolve1D_f ( &gan_image_get_pixptr_rgb_f(image,0,j)->R,
                                    image->stride/sizeof(float),
                                    eEdgeBehaviour, GAN_FALSE,
                                    &gan_image_get_pixptr_rgb_f(dest,0,j)->R,
                                    dest->stride/sizeof(float),
                                    mask, (int)dheight ) ||
                !gan_convolve1D_f ( &gan_image_get_pixptr_rgb_f(image,0,j)->G,
                                    image->stride/sizeof(float),
                                    eEdgeBehaviour, GAN_FALSE,
                                    &gan_image_get_pixptr_rgb_f(dest,0,j)->G,
                                    dest->stride/sizeof(float),
                                    mask, (int)dheight ) ||
                !gan_convolve1D_f ( &gan_image_get_pixptr_rgb_f(image,0,j)->B,
                                    image->stride/sizeof(float),
                                    eEdgeBehaviour, GAN_FALSE,
                                    &gan_image_get_pixptr_rgb_f(dest,0,j)->B,
                                    dest->stride/sizeof(float),
                                    mask, (int)dheight ) )
           {
              gan_err_register ( "image_convolve1Dy_rgb_rgb",
                                 GAN_ERROR_FAILURE, "" );
              return NULL;
           }
#else
              for ( j = (int)dest->width-1; j >= 0; j-- )
              {
                 if ( !gan_convolve1D_rgb_f (gan_image_get_pixptr_rgb_f(image,0,j),
                                             image->stride/sizeof(float),
                                             gan_image_get_pixptr_rgb_f(dest,0,j),
                                             dest->stride/sizeof(float),
                                             mask, dest->height))
                 {
                    gan_err_register ( "image_convolve1Dy_rgb_rgb",
                                       GAN_ERROR_FAILURE, "" );
                    return NULL;
                 }
              }
#endif
              
        break;

      case GAN_DOUBLE:
        gan_err_test_ptr ( (image->stride % sizeof(double)) == 0 &&
                           (dest->stride % sizeof(double)) == 0,
                           "image_convolve1Dy_rgb_rgb", GAN_ERROR_INCOMPATIBLE,
                           "" );
         for ( j = (int)dest->width-1; j >= 0; j-- )
           if ( !gan_convolve1D_d ( &gan_image_get_pixptr_rgb_d(image,0,j)->R,
                                    image->stride/sizeof(double),
                                    eEdgeBehaviour, GAN_FALSE,
                                    &gan_image_get_pixptr_rgb_d(dest,0,j)->R,
                                    dest->stride/sizeof(double),
                                    mask, (int)dheight ) ||
                !gan_convolve1D_d ( &gan_image_get_pixptr_rgb_d(image,0,j)->G,
                                    image->stride/sizeof(double),
                                    eEdgeBehaviour, GAN_FALSE,
                                    &gan_image_get_pixptr_rgb_d(dest,0,j)->G,
                                    dest->stride/sizeof(double),
                                    mask, (int)dheight ) ||
                !gan_convolve1D_d ( &gan_image_get_pixptr_rgb_d(image,0,j)->B,
                                    image->stride/sizeof(double),
                                    eEdgeBehaviour, GAN_FALSE,
                                    &gan_image_get_pixptr_rgb_d(dest,0,j)->B,
                                    dest->stride/sizeof(double),
                                    mask, (int)dheight ) )
           {
              gan_err_register ( "image_convolve1Dy_rgb_rgb",
                                 GAN_ERROR_FAILURE, "" );
              return NULL;
           }
              
        break;

      case GAN_INT:
        gan_err_test_ptr ( (image->stride % sizeof(int)) == 0 &&
                           (dest->stride % sizeof(int)) == 0,
                           "image_convolve1Dy_rgb_rgb", GAN_ERROR_INCOMPATIBLE,
                           "image stride" );
        for ( j = (int)dest->width-1; j >= 0; j-- )
           if ( !gan_convolve1D_i ( &gan_image_get_pixptr_rgb_i(image,0,j)->R,
                                    image->stride/sizeof(int),
                                    eEdgeBehaviour, GAN_FALSE,
                                    &gan_image_get_pixptr_rgb_i(dest,0,j)->R,
                                    dest->stride/sizeof(int),
                                    mask, (int)dheight ) ||
                !gan_convolve1D_i ( &gan_image_get_pixptr_rgb_i(image,0,j)->G,
                                    image->stride/sizeof(int),
                                    eEdgeBehaviour, GAN_FALSE,
                                    &gan_image_get_pixptr_rgb_i(dest,0,j)->G,
                                    dest->stride/sizeof(int),
                                    mask, (int)dheight ) ||
                !gan_convolve1D_i ( &gan_image_get_pixptr_rgb_i(image,0,j)->B,
                                    image->stride/sizeof(int),
                                    eEdgeBehaviour, GAN_FALSE,
                                    &gan_image_get_pixptr_rgb_i(dest,0,j)->B,
                                    dest->stride/sizeof(int),
                                    mask, (int)dheight ) )
           {
              gan_err_register ( "image_convolve1Dy_rgb_rgb",
                                 GAN_ERROR_FAILURE, "" );
              return NULL;
           }

        break;

      case GAN_UINT:
        gan_err_test_ptr ( (image->stride % sizeof(unsigned int)) == 0 &&
                           (dest->stride % sizeof(unsigned int)) == 0,
                           "image_convolve1Dy_rgb_rgb", GAN_ERROR_INCOMPATIBLE,
                           "" );
        for ( j = (int)dest->width-1; j >= 0; j-- )
           if ( !gan_convolve1D_ui (&gan_image_get_pixptr_rgb_ui(image,0,j)->R,
                                    image->stride/sizeof(unsigned int),
                                    eEdgeBehaviour, GAN_FALSE,
                                    &gan_image_get_pixptr_rgb_ui(dest,0,j)->R,
                                    dest->stride/sizeof(unsigned int),
                                    mask, (int)dheight) ||
                !gan_convolve1D_ui (&gan_image_get_pixptr_rgb_ui(image,0,j)->G,
                                    image->stride/sizeof(unsigned int),
                                    eEdgeBehaviour, GAN_FALSE,
                                    &gan_image_get_pixptr_rgb_ui(dest,0,j)->G,
                                    dest->stride/sizeof(unsigned int),
                                    mask, (int)dheight) ||
                !gan_convolve1D_ui (&gan_image_get_pixptr_rgb_ui(image,0,j)->B,
                                    image->stride/sizeof(unsigned int),
                                    eEdgeBehaviour, GAN_FALSE,
                                    &gan_image_get_pixptr_rgb_ui(dest,0,j)->B,
                                    image->stride/sizeof(unsigned int),
                                    mask, (int)dheight) )
           {
              gan_err_register ( "image_convolve1Dy_rgb_rgb",
                                 GAN_ERROR_FAILURE, "" );
              return NULL;
           }

        break;
        
      case GAN_UCHAR:
        gan_err_test_ptr ( (image->stride % sizeof(unsigned char)) == 0 &&
                           (dest->stride % sizeof(unsigned char)) == 0,
                           "image_convolve1Dy_rgb_rgb", GAN_ERROR_INCOMPATIBLE,
                           "" );
#if 1 /*ndef __APPLE__ && defined(ALTIVEC_CONVOLUTION)*/
         for ( j = (int)dest->width-1; j >= 0; j-- )
           if ( !gan_convolve1D_uc (&gan_image_get_pixptr_rgb_uc(image,0,j)->R,
                                    image->stride/sizeof(unsigned char),
                                    eEdgeBehaviour, GAN_FALSE,
                                    &gan_image_get_pixptr_rgb_uc(dest,0,j)->R,
                                    dest->stride/sizeof(unsigned char),
                                    mask, (int)dheight) ||
                !gan_convolve1D_uc (&gan_image_get_pixptr_rgb_uc(image,0,j)->G,
                                    image->stride/sizeof(unsigned char),
                                    eEdgeBehaviour, GAN_FALSE,
                                    &gan_image_get_pixptr_rgb_uc(dest,0,j)->G,
                                    dest->stride/sizeof(unsigned char),
                                    mask, (int)dheight) ||
                !gan_convolve1D_uc (&gan_image_get_pixptr_rgb_uc(image,0,j)->B,
                                    image->stride/sizeof(unsigned char),
                                    eEdgeBehaviour, GAN_FALSE,
                                    &gan_image_get_pixptr_rgb_uc(dest,0,j)->B,
                                    dest->stride/sizeof(unsigned char),
                                    mask, (int)dheight) )
           {
              gan_err_register ( "image_convolve1Dy_rgb_rgb",
                                 GAN_ERROR_FAILURE, "" );
              return NULL;
           }

#else
              for ( j = (int)dest->width-1; j >= 0; j-- )
              {
                 if ( !gan_convolve1D_rgb_uc ((unsigned char*)gan_image_get_pixptr_rgb_uc(image,0,j),
                                              image->stride/sizeof(unsigned char),
                                              (unsigned char*)gan_image_get_pixptr_rgb_uc(dest,0,j),
                                              dest->stride/sizeof(unsigned char),
                                              mask, dest->height))
                 {
                    gan_err_register ( "image_convolve1Dy_rgb_rgb",
                                       GAN_ERROR_FAILURE, "" );
                    return NULL;
                 }
              }
#endif
        break;
        
      case GAN_USHORT:
        gan_err_test_ptr ( (image->stride % sizeof(unsigned short)) == 0 &&
                           (dest->stride % sizeof(unsigned short)) == 0,
                           "image_convolve1Dy_rgb_rgb", GAN_ERROR_INCOMPATIBLE,
                           "" );
        for ( j = (int)dest->width-1; j >= 0; j-- )
           if ( !gan_convolve1D_us (&gan_image_get_pixptr_rgb_us(image,0,j)->R,
                                    image->stride/sizeof(unsigned short),
                                    eEdgeBehaviour, GAN_FALSE,
                                    &gan_image_get_pixptr_rgb_us(dest,0,j)->R,
                                    dest->stride/sizeof(unsigned short),
                                    mask, (int)dheight) ||
                !gan_convolve1D_us (&gan_image_get_pixptr_rgb_us(image,0,j)->G,
                                    image->stride/sizeof(unsigned short),
                                    eEdgeBehaviour, GAN_FALSE,
                                    &gan_image_get_pixptr_rgb_us(dest,0,j)->G,
                                    dest->stride/sizeof(unsigned short),
                                    mask, (int)dheight) ||
                !gan_convolve1D_us (&gan_image_get_pixptr_rgb_us(image,0,j)->B,
                                    image->stride/sizeof(unsigned short),
                                    eEdgeBehaviour, GAN_FALSE,
                                    &gan_image_get_pixptr_rgb_us(dest,0,j)->B,
                                    dest->stride/sizeof(unsigned short),
                                    mask, (int)dheight) )
           {
              gan_err_register ( "image_convolve1Dy_rgb_rgb",
                                 GAN_ERROR_FAILURE, "" );
              return NULL;
           }

        break;
        
      default:
        gan_err_flush_trace();
        gan_err_register ( "image_convolve1Dy_rgb_rgb", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
        return NULL;
   }

   /* check if we aborted the loop early because of an error */
   if ( j >= 0 )
   {
      gan_err_register ( "image_convolve1Dy_rgb_rgb", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* success */
   return dest;
}

static Gan_Image *
 image_convolve1Dy_rgba_rgba ( const Gan_Image *image,
                               Gan_Bool bConvolveInAlphaChannel,
                               Gan_EdgeBehaviour eEdgeBehaviour,
                               Gan_Mask1D *mask, Gan_Image *dest )
{
   int j;
   unsigned int dheight;

   gan_err_test_ptr ( image->format == GAN_RGB_COLOUR_ALPHA_IMAGE,
                      "image_convolve1Dy_rgb_rgba", GAN_ERROR_INCOMPATIBLE,
                      "" );
   gan_err_test_ptr ( image->height >= mask->size,
                      "image_convolve1Dy_rgba_rgba",
                      GAN_ERROR_IMAGE_TOO_SMALL, "" );

   if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_CLIP )
      dheight = image->height-mask->size+1;
   else if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_EXPAND )
      dheight = image->height+mask->size-1;
   else
      dheight = image->height;

   if ( dest == NULL )
      dest = gan_image_alloc ( GAN_RGB_COLOUR_ALPHA_IMAGE, image->type,
                               dheight, image->width );
   else if (    dest->format != GAN_RGB_COLOUR_ALPHA_IMAGE
             || dest->type   != image->type
             || dest->width  != image->width
             || dest->height != dheight )
      dest = gan_image_set_format_type_dims ( dest,
                                              GAN_RGB_COLOUR_ALPHA_IMAGE,
                                              image->type,
                                              dheight,
                                              image->width );

   if ( dest == NULL )
   {
      gan_err_register ( "image_convolve1Dy_rgba_rgba", GAN_ERROR_FAILURE, "");
      return NULL;
   }

   dest->offset_x = image->offset_x;
   if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_CLIP )
      dest->offset_y = image->offset_y+(int)mask->size/2;
   else if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_EXPAND )
      dest->offset_y = image->offset_y-(int)mask->size/2;
   else
      dest->offset_y = image->offset_y;

   switch ( image->type )
   {
      case GAN_FLOAT:
        gan_err_test_ptr ( (image->stride % sizeof(float)) == 0 &&
                           (dest->stride % sizeof(float)) == 0,
                           "image_convolve1Dy_rgba_rgba",
                           GAN_ERROR_INCOMPATIBLE, "" );
        for ( j = (int)dest->width-1; j >= 0; j-- )
           if ( !gan_convolve1D_f ( &gan_image_get_pixptr_rgba_f(image,0,j)->R,
                                    image->stride/sizeof(float),
                                    eEdgeBehaviour, GAN_FALSE,
                                    &gan_image_get_pixptr_rgba_f(dest,0,j)->R,
                                    dest->stride/sizeof(float),
                                    mask, (int)dheight ) ||
                !gan_convolve1D_f ( &gan_image_get_pixptr_rgba_f(image,0,j)->G,
                                    image->stride/sizeof(float),
                                    eEdgeBehaviour, GAN_FALSE,
                                    &gan_image_get_pixptr_rgba_f(dest,0,j)->G,
                                    dest->stride/sizeof(float),
                                    mask, (int)dheight ) ||
                !gan_convolve1D_f ( &gan_image_get_pixptr_rgba_f(image,0,j)->B,
                                    image->stride/sizeof(float),
                                    eEdgeBehaviour, GAN_FALSE,
                                    &gan_image_get_pixptr_rgba_f(dest,0,j)->B,
                                    dest->stride/sizeof(float),
                                    mask, (int)dheight ) ||
                (bConvolveInAlphaChannel &&
                 !gan_convolve1D_f ( &gan_image_get_pixptr_rgba_f(image,0,j)->A,
                                     image->stride/sizeof(float),
                                     eEdgeBehaviour, GAN_TRUE,
                                     &gan_image_get_pixptr_rgba_f(dest,0,j)->A,
                                     dest->stride/sizeof(float),
                                     mask, (int)dheight )) )
           {
              gan_err_register ( "image_convolve1Dy_rgba_rgba", GAN_ERROR_FAILURE, "" );
              return NULL;
           }

        break;

      case GAN_DOUBLE:
        gan_err_test_ptr ( (image->stride % sizeof(double)) == 0 &&
                           (dest->stride % sizeof(double)) == 0,
                           "image_convolve1Dy_rgba_rgba",
                           GAN_ERROR_INCOMPATIBLE, "" );
        for ( j = (int)dest->width-1; j >= 0; j-- )
           if ( !gan_convolve1D_d ( &gan_image_get_pixptr_rgba_d(image,0,j)->R,
                                    image->stride/sizeof(double),
                                    eEdgeBehaviour, GAN_FALSE,
                                    &gan_image_get_pixptr_rgba_d(dest,0,j)->R,
                                    dest->stride/sizeof(double),
                                    mask, (int)dheight ) ||
                !gan_convolve1D_d ( &gan_image_get_pixptr_rgba_d(image,0,j)->G,
                                    image->stride/sizeof(double),
                                    eEdgeBehaviour, GAN_FALSE,
                                    &gan_image_get_pixptr_rgba_d(dest,0,j)->G,
                                    dest->stride/sizeof(double),
                                    mask, (int)dheight ) ||
                !gan_convolve1D_d ( &gan_image_get_pixptr_rgba_d(image,0,j)->B,
                                    image->stride/sizeof(double),
                                    eEdgeBehaviour, GAN_FALSE,
                                    &gan_image_get_pixptr_rgba_d(dest,0,j)->B,
                                    dest->stride/sizeof(double),
                                    mask, (int)dheight ) ||
                (bConvolveInAlphaChannel &&
                 !gan_convolve1D_d ( &gan_image_get_pixptr_rgba_d(image,0,j)->A,
                                     image->stride/sizeof(double),
                                     eEdgeBehaviour, GAN_TRUE,
                                     &gan_image_get_pixptr_rgba_d(dest,0,j)->A,
                                     dest->stride/sizeof(double),
                                     mask, (int)dheight )) )
           {
              gan_err_register ( "image_convolve1Dy_rgba_rgba", GAN_ERROR_FAILURE, "" );
              return NULL;
           }

        break;

      case GAN_INT:
        gan_err_test_ptr ( (image->stride % sizeof(int)) == 0 &&
                           (dest->stride % sizeof(int)) == 0,
                           "image_convolve1Dy_rgb_rgba",
                           GAN_ERROR_INCOMPATIBLE, "" );
        for ( j = (int)dest->width-1; j >= 0; j-- )
           if ( !gan_convolve1D_i ( &gan_image_get_pixptr_rgba_i(image,0,j)->R,
                                    image->stride/sizeof(int),
                                    eEdgeBehaviour, GAN_FALSE,
                                    &gan_image_get_pixptr_rgba_i(dest,0,j)->R,
                                    dest->stride/sizeof(int),
                                    mask, (int)dheight ) ||
                !gan_convolve1D_i ( &gan_image_get_pixptr_rgba_i(image,0,j)->G,
                                    image->stride/sizeof(int),
                                    eEdgeBehaviour, GAN_FALSE,
                                    &gan_image_get_pixptr_rgba_i(dest,0,j)->G,
                                    dest->stride/sizeof(int),
                                    mask, (int)dheight ) ||
                !gan_convolve1D_i ( &gan_image_get_pixptr_rgba_i(image,0,j)->B,
                                    image->stride/sizeof(int),
                                    eEdgeBehaviour, GAN_FALSE,
                                    &gan_image_get_pixptr_rgba_i(dest,0,j)->B,
                                    dest->stride/sizeof(int),
                                    mask, (int)dheight ) ||
                (bConvolveInAlphaChannel &&
                 !gan_convolve1D_i ( &gan_image_get_pixptr_rgba_i(image,0,j)->A,
                                     image->stride/sizeof(int),
                                     eEdgeBehaviour, GAN_TRUE,
                                     &gan_image_get_pixptr_rgba_i(dest,0,j)->A,
                                     dest->stride/sizeof(int),
                                     mask, (int)dheight )) )
           {
              gan_err_register ( "image_convolve1Dy_rgba_rgba", GAN_ERROR_FAILURE, "" );
              return NULL;
           }

        break;

      case GAN_UINT:
        gan_err_test_ptr ( (image->stride % sizeof(unsigned int)) == 0 &&
                           (dest->stride % sizeof(unsigned int)) == 0,
                           "image_convolve1Dy_rgba_rgba",
                           GAN_ERROR_INCOMPATIBLE, "" );
        for ( j = (int)dest->width-1; j >= 0; j-- )
           if ( !gan_convolve1D_ui(&gan_image_get_pixptr_rgba_ui(image,0,j)->R,
                                   image->stride/sizeof(unsigned int),
                                   eEdgeBehaviour, GAN_FALSE,
                                   &gan_image_get_pixptr_rgba_ui(dest,0,j)->R,
                                   dest->stride/sizeof(unsigned int),
                                   mask, (int)dheight ) ||
                !gan_convolve1D_ui(&gan_image_get_pixptr_rgba_ui(image,0,j)->G,
                                   image->stride/sizeof(unsigned int),
                                   eEdgeBehaviour, GAN_FALSE,
                                   &gan_image_get_pixptr_rgba_ui(dest,0,j)->G,
                                   dest->stride/sizeof(unsigned int),
                                   mask, (int)dheight ) ||
                !gan_convolve1D_ui(&gan_image_get_pixptr_rgba_ui(image,0,j)->B,
                                   image->stride/sizeof(unsigned int),
                                   eEdgeBehaviour, GAN_FALSE,
                                   &gan_image_get_pixptr_rgba_ui(dest,0,j)->B,
                                   dest->stride/sizeof(unsigned int),
                                   mask, (int)dheight ) ||
                (bConvolveInAlphaChannel &&
                 !gan_convolve1D_ui(&gan_image_get_pixptr_rgba_ui(image,0,j)->A,
                                    image->stride/sizeof(unsigned int),
                                    eEdgeBehaviour, GAN_TRUE,
                                    &gan_image_get_pixptr_rgba_ui(dest,0,j)->A,
                                    dest->stride/sizeof(unsigned int),
                                    mask, (int)dheight )) )
           {
              gan_err_register ( "image_convolve1Dy_rgba_rgba", GAN_ERROR_FAILURE, "" );
              return NULL;
           }

        break;
        
      case GAN_UCHAR:
        gan_err_test_ptr ( (image->stride % sizeof(unsigned char)) == 0 &&
                           (dest->stride % sizeof(unsigned char)) == 0,
                           "image_convolve1Dy_rgba_rgba",
                           GAN_ERROR_INCOMPATIBLE, "" );
        for ( j = (int)dest->width-1; j >= 0; j-- )
           if ( !gan_convolve1D_uc(&gan_image_get_pixptr_rgba_uc(image,0,j)->R,
                                   image->stride/sizeof(unsigned char),
                                   eEdgeBehaviour, GAN_FALSE,
                                   &gan_image_get_pixptr_rgba_uc(dest,0,j)->R,
                                   dest->stride/sizeof(unsigned char),
                                   mask, (int)dheight ) ||
                !gan_convolve1D_uc(&gan_image_get_pixptr_rgba_uc(image,0,j)->G,
                                   image->stride/sizeof(unsigned char),
                                   eEdgeBehaviour, GAN_FALSE,
                                   &gan_image_get_pixptr_rgba_uc(dest,0,j)->G,
                                   dest->stride/sizeof(unsigned char),
                                   mask, (int)dheight ) ||
                !gan_convolve1D_uc(&gan_image_get_pixptr_rgba_uc(image,0,j)->B,
                                   image->stride/sizeof(unsigned char),
                                   eEdgeBehaviour, GAN_FALSE,
                                   &gan_image_get_pixptr_rgba_uc(dest,0,j)->B,
                                   dest->stride/sizeof(unsigned char),
                                   mask, (int)dheight ) ||
                (bConvolveInAlphaChannel &&
                 !gan_convolve1D_uc(&gan_image_get_pixptr_rgba_uc(image,0,j)->A,
                                    image->stride/sizeof(unsigned char),
                                    eEdgeBehaviour, GAN_TRUE,
                                    &gan_image_get_pixptr_rgba_uc(dest,0,j)->A,
                                    dest->stride/sizeof(unsigned char),
                                    mask, (int)dheight )) )
           {
              gan_err_register ( "image_convolve1Dy_rgba_rgba", GAN_ERROR_FAILURE, "" );
              return NULL;
           }

        break;

      case GAN_USHORT:
        gan_err_test_ptr ( (image->stride % sizeof(unsigned short)) == 0 &&
                           (dest->stride % sizeof(unsigned short)) == 0,
                           "image_convolve1Dy_rgba_rgba",
                           GAN_ERROR_INCOMPATIBLE, "" );
        for ( j = (int)dest->width-1; j >= 0; j-- )
           if ( !gan_convolve1D_us(&gan_image_get_pixptr_rgba_us(image,0,j)->R,
                                   image->stride/sizeof(unsigned short),
                                   eEdgeBehaviour, GAN_FALSE,
                                   &gan_image_get_pixptr_rgba_us(dest,0,j)->R,
                                   dest->stride/sizeof(unsigned short),
                                   mask, (int)dheight ) ||
                !gan_convolve1D_us(&gan_image_get_pixptr_rgba_us(image,0,j)->G,
                                   image->stride/sizeof(unsigned short),
                                   eEdgeBehaviour, GAN_FALSE,
                                   &gan_image_get_pixptr_rgba_us(dest,0,j)->G,
                                   dest->stride/sizeof(unsigned short),
                                   mask, (int)dheight ) ||
                !gan_convolve1D_us(&gan_image_get_pixptr_rgba_us(image,0,j)->B,
                                   image->stride/sizeof(unsigned short),
                                   eEdgeBehaviour, GAN_FALSE,
                                   &gan_image_get_pixptr_rgba_us(dest,0,j)->B,
                                   dest->stride/sizeof(unsigned short),
                                   mask, (int)dheight ) ||
                (bConvolveInAlphaChannel &&
                 !gan_convolve1D_us(&gan_image_get_pixptr_rgba_us(image,0,j)->A,
                                    image->stride/sizeof(unsigned short),
                                    eEdgeBehaviour, GAN_TRUE,
                                    &gan_image_get_pixptr_rgba_us(dest,0,j)->A,
                                    dest->stride/sizeof(unsigned short),
                                    mask, (int)dheight )) )
           {
              gan_err_register ( "image_convolve1Dy_rgba_rgba", GAN_ERROR_FAILURE, "" );
              return NULL;
           }

        break;
        
      default:
        gan_err_flush_trace();
        gan_err_register ( "image_convolve1Dy_rgba_rgba", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
        return NULL;
   }

   /* check if we aborted the loop early because of an error */
   if ( j >= 0 )
   {
      gan_err_register ( "image_convolve1Dy_rgba_rgba", GAN_ERROR_FAILURE, "");
      return NULL;
   }

   /* success */
   return dest;
}

static Gan_Image *
 image_convolve1Dy_bgr_bgr ( const Gan_Image *image,
                             Gan_EdgeBehaviour eEdgeBehaviour,
                             Gan_Mask1D *mask, Gan_Image *dest )
{
   int j;
   unsigned int dheight;

   gan_err_test_ptr ( image->format == GAN_BGR_COLOUR_IMAGE,
                      "image_convolve1Dy_bgr_bgr", GAN_ERROR_INCOMPATIBLE,
                      "" );
   gan_err_test_ptr ( image->height >= mask->size, "image_convolve1Dy_bgr_bgr",
                      GAN_ERROR_IMAGE_TOO_SMALL, "" );

   if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_CLIP )
      dheight = image->height-mask->size+1;
   else if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_EXPAND )
      dheight = image->height+mask->size-1;
   else
      dheight = image->height;

   if ( dest == NULL )
      dest = gan_image_alloc ( GAN_BGR_COLOUR_IMAGE, image->type,
                               dheight, image->width );
   else if (    dest->format != GAN_BGR_COLOUR_IMAGE
             || dest->type   != image->type
             || dest->width  != image->width
             || dest->height != dheight )
      dest = gan_image_set_format_type_dims ( dest,
                                              GAN_BGR_COLOUR_IMAGE,
                                              image->type,
                                              dheight,
                                              image->width );

   if ( dest == NULL )
   {
      gan_err_register ( "image_convolve1Dy_bgr_bgr", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   dest->offset_x = image->offset_x;
   if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_CLIP )
      dest->offset_y = image->offset_y+(int)mask->size/2;
   else if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_EXPAND )
      dest->offset_y = image->offset_y-(int)mask->size/2;
   else
      dest->offset_y = image->offset_y;

   switch ( image->type )
   {
      case GAN_FLOAT:
        gan_err_test_ptr ( (image->stride % sizeof(float)) == 0 &&
                           (dest->stride % sizeof(float)) == 0,
                           "image_convolve1Dy_bgr_bgr", GAN_ERROR_INCOMPATIBLE,
                           "" );
#if 1 /*__APPLE__*/
        for ( j = (int)dest->width-1; j >= 0; j-- )
           if ( !gan_convolve1D_f ( &gan_image_get_pixptr_bgr_f(image,0,j)->R,
                                    image->stride/sizeof(float),
                                    eEdgeBehaviour, GAN_FALSE,
                                    &gan_image_get_pixptr_bgr_f(dest,0,j)->R,
                                    dest->stride/sizeof(float),
                                    mask, (int)dheight ) ||
                !gan_convolve1D_f ( &gan_image_get_pixptr_bgr_f(image,0,j)->G,
                                    image->stride/sizeof(float),
                                    eEdgeBehaviour, GAN_FALSE,
                                    &gan_image_get_pixptr_bgr_f(dest,0,j)->G,
                                    dest->stride/sizeof(float),
                                    mask, (int)dheight ) ||
                !gan_convolve1D_f ( &gan_image_get_pixptr_bgr_f(image,0,j)->B,
                                    image->stride/sizeof(float),
                                    eEdgeBehaviour, GAN_FALSE,
                                    &gan_image_get_pixptr_bgr_f(dest,0,j)->B,
                                    dest->stride/sizeof(float),
                                    mask, (int)dheight ) )
           {
              gan_err_register ( "image_convolve1Dy_bgr_bgr",
                                 GAN_ERROR_FAILURE, "" );
              return NULL;
           }
#else
              for ( j = (int)dest->width-1; j >= 0; j-- )
              {
                 if ( !gan_convolve1D_bgr_f (gan_image_get_pixptr_bgr_f(image,0,j),
                                             image->stride/sizeof(float),
                                             gan_image_get_pixptr_bgr_f(dest,0,j),
                                             dest->stride/sizeof(float),
                                             mask, dest->height))
                 {
                    gan_err_register ( "image_convolve1Dy_bgr_bgr",
                                       GAN_ERROR_FAILURE, "" );
                    return NULL;
                 }
              }
#endif
              
        break;

      case GAN_DOUBLE:
        gan_err_test_ptr ( (image->stride % sizeof(double)) == 0 &&
                           (dest->stride % sizeof(double)) == 0,
                           "image_convolve1Dy_bgr_bgr", GAN_ERROR_INCOMPATIBLE,
                           "" );
         for ( j = (int)dest->width-1; j >= 0; j-- )
           if ( !gan_convolve1D_d ( &gan_image_get_pixptr_bgr_d(image,0,j)->R,
                                    image->stride/sizeof(double),
                                    eEdgeBehaviour, GAN_FALSE,
                                    &gan_image_get_pixptr_bgr_d(dest,0,j)->R,
                                    dest->stride/sizeof(double),
                                    mask, (int)dheight ) ||
                !gan_convolve1D_d ( &gan_image_get_pixptr_bgr_d(image,0,j)->G,
                                    image->stride/sizeof(double),
                                    eEdgeBehaviour, GAN_FALSE,
                                    &gan_image_get_pixptr_bgr_d(dest,0,j)->G,
                                    dest->stride/sizeof(double),
                                    mask, (int)dheight ) ||
                !gan_convolve1D_d ( &gan_image_get_pixptr_bgr_d(image,0,j)->B,
                                    image->stride/sizeof(double),
                                    eEdgeBehaviour, GAN_FALSE,
                                    &gan_image_get_pixptr_bgr_d(dest,0,j)->B,
                                    dest->stride/sizeof(double),
                                    mask, (int)dheight ) )
           {
              gan_err_register ( "image_convolve1Dy_bgr_bgr",
                                 GAN_ERROR_FAILURE, "" );
              return NULL;
           }
              
        break;

      case GAN_INT:
        gan_err_test_ptr ( (image->stride % sizeof(int)) == 0 &&
                           (dest->stride % sizeof(int)) == 0,
                           "image_convolve1Dy_bgr_bgr", GAN_ERROR_INCOMPATIBLE,
                           "image stride" );
        for ( j = (int)dest->width-1; j >= 0; j-- )
           if ( !gan_convolve1D_i ( &gan_image_get_pixptr_bgr_i(image,0,j)->R,
                                    image->stride/sizeof(int),
                                    eEdgeBehaviour, GAN_FALSE,
                                    &gan_image_get_pixptr_bgr_i(dest,0,j)->R,
                                    dest->stride/sizeof(int),
                                    mask, (int)dheight ) ||
                !gan_convolve1D_i ( &gan_image_get_pixptr_bgr_i(image,0,j)->G,
                                    image->stride/sizeof(int),
                                    eEdgeBehaviour, GAN_FALSE,
                                    &gan_image_get_pixptr_bgr_i(dest,0,j)->G,
                                    dest->stride/sizeof(int),
                                    mask, (int)dheight ) ||
                !gan_convolve1D_i ( &gan_image_get_pixptr_bgr_i(image,0,j)->B,
                                    image->stride/sizeof(int),
                                    eEdgeBehaviour, GAN_FALSE,
                                    &gan_image_get_pixptr_bgr_i(dest,0,j)->B,
                                    dest->stride/sizeof(int),
                                    mask, (int)dheight ) )
           {
              gan_err_register ( "image_convolve1Dy_bgr_bgr",
                                 GAN_ERROR_FAILURE, "" );
              return NULL;
           }

        break;

      case GAN_UINT:
        gan_err_test_ptr ( (image->stride % sizeof(unsigned int)) == 0 &&
                           (dest->stride % sizeof(unsigned int)) == 0,
                           "image_convolve1Dy_bgr_bgr", GAN_ERROR_INCOMPATIBLE,
                           "" );
        for ( j = (int)dest->width-1; j >= 0; j-- )
           if ( !gan_convolve1D_ui (&gan_image_get_pixptr_bgr_ui(image,0,j)->R,
                                    image->stride/sizeof(unsigned int),
                                    eEdgeBehaviour, GAN_FALSE,
                                    &gan_image_get_pixptr_bgr_ui(dest,0,j)->R,
                                    dest->stride/sizeof(unsigned int),
                                    mask, (int)dheight) ||
                !gan_convolve1D_ui (&gan_image_get_pixptr_bgr_ui(image,0,j)->G,
                                    image->stride/sizeof(unsigned int),
                                    eEdgeBehaviour, GAN_FALSE,
                                    &gan_image_get_pixptr_bgr_ui(dest,0,j)->G,
                                    dest->stride/sizeof(unsigned int),
                                    mask, (int)dheight) ||
                !gan_convolve1D_ui (&gan_image_get_pixptr_bgr_ui(image,0,j)->B,
                                    image->stride/sizeof(unsigned int),
                                    eEdgeBehaviour, GAN_FALSE,
                                    &gan_image_get_pixptr_bgr_ui(dest,0,j)->B,
                                    image->stride/sizeof(unsigned int),
                                    mask, (int)dheight) )
           {
              gan_err_register ( "image_convolve1Dy_bgr_bgr",
                                 GAN_ERROR_FAILURE, "" );
              return NULL;
           }

        break;
        
      case GAN_UCHAR:
        gan_err_test_ptr ( (image->stride % sizeof(unsigned char)) == 0 &&
                           (dest->stride % sizeof(unsigned char)) == 0,
                           "image_convolve1Dy_bgr_bgr", GAN_ERROR_INCOMPATIBLE,
                           "" );
#if 1 /*ndef __APPLE__ && defined(ALTIVEC_CONVOLUTION)*/
         for ( j = (int)dest->width-1; j >= 0; j-- )
           if ( !gan_convolve1D_uc (&gan_image_get_pixptr_bgr_uc(image,0,j)->R,
                                    image->stride/sizeof(unsigned char),
                                    eEdgeBehaviour, GAN_FALSE,
                                    &gan_image_get_pixptr_bgr_uc(dest,0,j)->R,
                                    dest->stride/sizeof(unsigned char),
                                    mask, (int)dheight) ||
                !gan_convolve1D_uc (&gan_image_get_pixptr_bgr_uc(image,0,j)->G,
                                    image->stride/sizeof(unsigned char),
                                    eEdgeBehaviour, GAN_FALSE,
                                    &gan_image_get_pixptr_bgr_uc(dest,0,j)->G,
                                    dest->stride/sizeof(unsigned char),
                                    mask, (int)dheight) ||
                !gan_convolve1D_uc (&gan_image_get_pixptr_bgr_uc(image,0,j)->B,
                                    image->stride/sizeof(unsigned char),
                                    eEdgeBehaviour, GAN_FALSE,
                                    &gan_image_get_pixptr_bgr_uc(dest,0,j)->B,
                                    dest->stride/sizeof(unsigned char),
                                    mask, (int)dheight) )
           {
              gan_err_register ( "image_convolve1Dy_bgr_bgr",
                                 GAN_ERROR_FAILURE, "" );
              return NULL;
           }

#else
              for ( j = (int)dest->width-1; j >= 0; j-- )
              {
                 if ( !gan_convolve1D_bgr_uc ((unsigned char*)gan_image_get_pixptr_bgr_uc(image,0,j),
                                              image->stride/sizeof(unsigned char),
                                              (unsigned char*)gan_image_get_pixptr_bgr_uc(dest,0,j),
                                              dest->stride/sizeof(unsigned char),
                                              mask, dest->height))
                 {
                    gan_err_register ( "image_convolve1Dy_bgr_bgr",
                                       GAN_ERROR_FAILURE, "" );
                    return NULL;
                 }
              }
#endif
        break;
        
      case GAN_USHORT:
        gan_err_test_ptr ( (image->stride % sizeof(unsigned short)) == 0 &&
                           (dest->stride % sizeof(unsigned short)) == 0,
                           "image_convolve1Dy_bgr_bgr", GAN_ERROR_INCOMPATIBLE,
                           "" );
        for ( j = (int)dest->width-1; j >= 0; j-- )
           if ( !gan_convolve1D_us (&gan_image_get_pixptr_bgr_us(image,0,j)->R,
                                    image->stride/sizeof(unsigned short),
                                    eEdgeBehaviour, GAN_FALSE,
                                    &gan_image_get_pixptr_bgr_us(dest,0,j)->R,
                                    dest->stride/sizeof(unsigned short),
                                    mask, (int)dheight) ||
                !gan_convolve1D_us (&gan_image_get_pixptr_bgr_us(image,0,j)->G,
                                    image->stride/sizeof(unsigned short),
                                    eEdgeBehaviour, GAN_FALSE,
                                    &gan_image_get_pixptr_bgr_us(dest,0,j)->G,
                                    dest->stride/sizeof(unsigned short),
                                    mask, (int)dheight) ||
                !gan_convolve1D_us (&gan_image_get_pixptr_bgr_us(image,0,j)->B,
                                    image->stride/sizeof(unsigned short),
                                    eEdgeBehaviour, GAN_FALSE,
                                    &gan_image_get_pixptr_bgr_us(dest,0,j)->B,
                                    dest->stride/sizeof(unsigned short),
                                    mask, (int)dheight) )
           {
              gan_err_register ( "image_convolve1Dy_bgr_bgr",
                                 GAN_ERROR_FAILURE, "" );
              return NULL;
           }

        break;
        
      default:
        gan_err_flush_trace();
        gan_err_register ( "image_convolve1Dy_bgr_bgr", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
        return NULL;
   }

   /* check if we aborted the loop early because of an error */
   if ( j >= 0 )
   {
      gan_err_register ( "image_convolve1Dy_bgr_bgr", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* success */
   return dest;
}

static Gan_Image *
 image_convolve1Dy_bgra_bgra ( const Gan_Image *image,
                               Gan_Bool bConvolveInAlphaChannel,
                               Gan_EdgeBehaviour eEdgeBehaviour,
                               Gan_Mask1D *mask, Gan_Image *dest )
{
   int j;
   unsigned int dheight;

   gan_err_test_ptr ( image->format == GAN_BGR_COLOUR_ALPHA_IMAGE,
                      "image_convolve1Dy_bgr_bgra", GAN_ERROR_INCOMPATIBLE,
                      "" );
   gan_err_test_ptr ( image->height >= mask->size,
                      "image_convolve1Dy_bgra_bgra",
                      GAN_ERROR_IMAGE_TOO_SMALL, "" );

   if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_CLIP )
      dheight = image->height-mask->size+1;
   else if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_EXPAND )
      dheight = image->height+mask->size-1;
   else
      dheight = image->height;

   if ( dest == NULL )
      dest = gan_image_alloc ( GAN_BGR_COLOUR_ALPHA_IMAGE, image->type,
                               dheight, image->width );
   else if (    dest->format != GAN_BGR_COLOUR_ALPHA_IMAGE
             || dest->type   != image->type
             || dest->width  != image->width
             || dest->height != dheight )
      dest = gan_image_set_format_type_dims ( dest,
                                              GAN_BGR_COLOUR_ALPHA_IMAGE,
                                              image->type,
                                              dheight,
                                              image->width );

   if ( dest == NULL )
   {
      gan_err_register ( "image_convolve1Dy_bgra_bgra", GAN_ERROR_FAILURE, "");
      return NULL;
   }

   dest->offset_x = image->offset_x;
   if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_CLIP )
      dest->offset_y = image->offset_y+(int)mask->size/2;
   else if ( eEdgeBehaviour == GAN_EDGE_BEHAVIOUR_EXPAND )
      dest->offset_y = image->offset_y-(int)mask->size/2;
   else
      dest->offset_y = image->offset_y;

   switch ( image->type )
   {
      case GAN_FLOAT:
        gan_err_test_ptr ( (image->stride % sizeof(float)) == 0 &&
                           (dest->stride % sizeof(float)) == 0,
                           "image_convolve1Dy_bgra_bgra",
                           GAN_ERROR_INCOMPATIBLE, "" );
        for ( j = (int)dest->width-1; j >= 0; j-- )
           if ( !gan_convolve1D_f ( &gan_image_get_pixptr_bgra_f(image,0,j)->R,
                                    image->stride/sizeof(float),
                                    eEdgeBehaviour, GAN_FALSE,
                                    &gan_image_get_pixptr_bgra_f(dest,0,j)->R,
                                    dest->stride/sizeof(float),
                                    mask, (int)dheight ) ||
                !gan_convolve1D_f ( &gan_image_get_pixptr_bgra_f(image,0,j)->G,
                                    image->stride/sizeof(float),
                                    eEdgeBehaviour, GAN_FALSE,
                                    &gan_image_get_pixptr_bgra_f(dest,0,j)->G,
                                    dest->stride/sizeof(float),
                                    mask, (int)dheight ) ||
                !gan_convolve1D_f ( &gan_image_get_pixptr_bgra_f(image,0,j)->B,
                                    image->stride/sizeof(float),
                                    eEdgeBehaviour, GAN_FALSE,
                                    &gan_image_get_pixptr_bgra_f(dest,0,j)->B,
                                    dest->stride/sizeof(float),
                                    mask, (int)dheight ) ||
                (bConvolveInAlphaChannel &&
                 !gan_convolve1D_f ( &gan_image_get_pixptr_bgra_f(image,0,j)->A,
                                     image->stride/sizeof(float),
                                     eEdgeBehaviour, GAN_TRUE,
                                     &gan_image_get_pixptr_bgra_f(dest,0,j)->A,
                                     dest->stride/sizeof(float),
                                     mask, (int)dheight )) )
           {
              gan_err_register ( "image_convolve1Dy_bgra_bgra", GAN_ERROR_FAILURE, "" );
              return NULL;
           }

        break;

      case GAN_DOUBLE:
        gan_err_test_ptr ( (image->stride % sizeof(double)) == 0 &&
                           (dest->stride % sizeof(double)) == 0,
                           "image_convolve1Dy_bgra_bgra",
                           GAN_ERROR_INCOMPATIBLE, "" );
        for ( j = (int)dest->width-1; j >= 0; j-- )
           if ( !gan_convolve1D_d ( &gan_image_get_pixptr_bgra_d(image,0,j)->R,
                                    image->stride/sizeof(double),
                                    eEdgeBehaviour, GAN_FALSE,
                                    &gan_image_get_pixptr_bgra_d(dest,0,j)->R,
                                    dest->stride/sizeof(double),
                                    mask, (int)dheight ) ||
                !gan_convolve1D_d ( &gan_image_get_pixptr_bgra_d(image,0,j)->G,
                                    image->stride/sizeof(double),
                                    eEdgeBehaviour, GAN_FALSE,
                                    &gan_image_get_pixptr_bgra_d(dest,0,j)->G,
                                    dest->stride/sizeof(double),
                                    mask, (int)dheight ) ||
                !gan_convolve1D_d ( &gan_image_get_pixptr_bgra_d(image,0,j)->B,
                                    image->stride/sizeof(double),
                                    eEdgeBehaviour, GAN_FALSE,
                                    &gan_image_get_pixptr_bgra_d(dest,0,j)->B,
                                    dest->stride/sizeof(double),
                                    mask, (int)dheight ) ||
                (bConvolveInAlphaChannel &&
                 !gan_convolve1D_d ( &gan_image_get_pixptr_bgra_d(image,0,j)->A,
                                     image->stride/sizeof(double),
                                     eEdgeBehaviour, GAN_TRUE,
                                     &gan_image_get_pixptr_bgra_d(dest,0,j)->A,
                                     dest->stride/sizeof(double),
                                     mask, (int)dheight )) )
           {
              gan_err_register ( "image_convolve1Dy_bgra_bgra", GAN_ERROR_FAILURE, "" );
              return NULL;
           }

        break;

      case GAN_INT:
        gan_err_test_ptr ( (image->stride % sizeof(int)) == 0 &&
                           (dest->stride % sizeof(int)) == 0,
                           "image_convolve1Dy_bgr_bgra",
                           GAN_ERROR_INCOMPATIBLE, "" );
        for ( j = (int)dest->width-1; j >= 0; j-- )
           if ( !gan_convolve1D_i ( &gan_image_get_pixptr_bgra_i(image,0,j)->R,
                                    image->stride/sizeof(int),
                                    eEdgeBehaviour, GAN_FALSE,
                                    &gan_image_get_pixptr_bgra_i(dest,0,j)->R,
                                    dest->stride/sizeof(int),
                                    mask, (int)dheight ) ||
                !gan_convolve1D_i ( &gan_image_get_pixptr_bgra_i(image,0,j)->G,
                                    image->stride/sizeof(int),
                                    eEdgeBehaviour, GAN_FALSE,
                                    &gan_image_get_pixptr_bgra_i(dest,0,j)->G,
                                    dest->stride/sizeof(int),
                                    mask, (int)dheight ) ||
                !gan_convolve1D_i ( &gan_image_get_pixptr_bgra_i(image,0,j)->B,
                                    image->stride/sizeof(int),
                                    eEdgeBehaviour, GAN_FALSE,
                                    &gan_image_get_pixptr_bgra_i(dest,0,j)->B,
                                    dest->stride/sizeof(int),
                                    mask, (int)dheight ) ||
                (bConvolveInAlphaChannel &&
                 !gan_convolve1D_i ( &gan_image_get_pixptr_bgra_i(image,0,j)->A,
                                     image->stride/sizeof(int),
                                     eEdgeBehaviour, GAN_TRUE,
                                     &gan_image_get_pixptr_bgra_i(dest,0,j)->A,
                                     dest->stride/sizeof(int),
                                     mask, (int)dheight )) )
           {
              gan_err_register ( "image_convolve1Dy_bgra_bgra", GAN_ERROR_FAILURE, "" );
              return NULL;
           }

        break;

      case GAN_UINT:
        gan_err_test_ptr ( (image->stride % sizeof(unsigned int)) == 0 &&
                           (dest->stride % sizeof(unsigned int)) == 0,
                           "image_convolve1Dy_bgra_bgra",
                           GAN_ERROR_INCOMPATIBLE, "" );
        for ( j = (int)dest->width-1; j >= 0; j-- )
           if ( !gan_convolve1D_ui(&gan_image_get_pixptr_bgra_ui(image,0,j)->R,
                                   image->stride/sizeof(unsigned int),
                                   eEdgeBehaviour, GAN_FALSE,
                                   &gan_image_get_pixptr_bgra_ui(dest,0,j)->R,
                                   dest->stride/sizeof(unsigned int),
                                   mask, (int)dheight ) ||
                !gan_convolve1D_ui(&gan_image_get_pixptr_bgra_ui(image,0,j)->G,
                                   image->stride/sizeof(unsigned int),
                                   eEdgeBehaviour, GAN_FALSE,
                                   &gan_image_get_pixptr_bgra_ui(dest,0,j)->G,
                                   dest->stride/sizeof(unsigned int),
                                   mask, (int)dheight ) ||
                !gan_convolve1D_ui(&gan_image_get_pixptr_bgra_ui(image,0,j)->B,
                                   image->stride/sizeof(unsigned int),
                                   eEdgeBehaviour, GAN_FALSE,
                                   &gan_image_get_pixptr_bgra_ui(dest,0,j)->B,
                                   dest->stride/sizeof(unsigned int),
                                   mask, (int)dheight ) ||
                (bConvolveInAlphaChannel &&
                 !gan_convolve1D_ui(&gan_image_get_pixptr_bgra_ui(image,0,j)->A,
                                    image->stride/sizeof(unsigned int),
                                    eEdgeBehaviour, GAN_TRUE,
                                    &gan_image_get_pixptr_bgra_ui(dest,0,j)->A,
                                    dest->stride/sizeof(unsigned int),
                                    mask, (int)dheight )) )
           {
              gan_err_register ( "image_convolve1Dy_bgra_bgra", GAN_ERROR_FAILURE, "" );
              return NULL;
           }

        break;
        
      case GAN_UCHAR:
        gan_err_test_ptr ( (image->stride % sizeof(unsigned char)) == 0 &&
                           (dest->stride % sizeof(unsigned char)) == 0,
                           "image_convolve1Dy_bgra_bgra",
                           GAN_ERROR_INCOMPATIBLE, "" );
        for ( j = (int)dest->width-1; j >= 0; j-- )
           if ( !gan_convolve1D_uc(&gan_image_get_pixptr_bgra_uc(image,0,j)->R,
                                   image->stride/sizeof(unsigned char),
                                   eEdgeBehaviour, GAN_FALSE,
                                   &gan_image_get_pixptr_bgra_uc(dest,0,j)->R,
                                   dest->stride/sizeof(unsigned char),
                                   mask, (int)dheight ) ||
                !gan_convolve1D_uc(&gan_image_get_pixptr_bgra_uc(image,0,j)->G,
                                   image->stride/sizeof(unsigned char),
                                   eEdgeBehaviour, GAN_FALSE,
                                   &gan_image_get_pixptr_bgra_uc(dest,0,j)->G,
                                   dest->stride/sizeof(unsigned char),
                                   mask, (int)dheight ) ||
                !gan_convolve1D_uc(&gan_image_get_pixptr_bgra_uc(image,0,j)->B,
                                   image->stride/sizeof(unsigned char),
                                   eEdgeBehaviour, GAN_FALSE,
                                   &gan_image_get_pixptr_bgra_uc(dest,0,j)->B,
                                   dest->stride/sizeof(unsigned char),
                                   mask, (int)dheight ) ||
                (bConvolveInAlphaChannel &&
                 !gan_convolve1D_uc(&gan_image_get_pixptr_bgra_uc(image,0,j)->A,
                                    image->stride/sizeof(unsigned char),
                                    eEdgeBehaviour, GAN_TRUE,
                                    &gan_image_get_pixptr_bgra_uc(dest,0,j)->A,
                                    dest->stride/sizeof(unsigned char),
                                    mask, (int)dheight )) )
           {
              gan_err_register ( "image_convolve1Dy_bgra_bgra", GAN_ERROR_FAILURE, "" );
              return NULL;
           }

        break;

      case GAN_USHORT:
        gan_err_test_ptr ( (image->stride % sizeof(unsigned short)) == 0 &&
                           (dest->stride % sizeof(unsigned short)) == 0,
                           "image_convolve1Dy_bgra_bgra",
                           GAN_ERROR_INCOMPATIBLE, "" );
        for ( j = (int)dest->width-1; j >= 0; j-- )
           if ( !gan_convolve1D_us(&gan_image_get_pixptr_bgra_us(image,0,j)->R,
                                   image->stride/sizeof(unsigned short),
                                   eEdgeBehaviour, GAN_FALSE,
                                   &gan_image_get_pixptr_bgra_us(dest,0,j)->R,
                                   dest->stride/sizeof(unsigned short),
                                   mask, (int)dheight ) ||
                !gan_convolve1D_us(&gan_image_get_pixptr_bgra_us(image,0,j)->G,
                                   image->stride/sizeof(unsigned short),
                                   eEdgeBehaviour, GAN_FALSE,
                                   &gan_image_get_pixptr_bgra_us(dest,0,j)->G,
                                   dest->stride/sizeof(unsigned short),
                                   mask, (int)dheight ) ||
                !gan_convolve1D_us(&gan_image_get_pixptr_bgra_us(image,0,j)->B,
                                   image->stride/sizeof(unsigned short),
                                   eEdgeBehaviour, GAN_FALSE,
                                   &gan_image_get_pixptr_bgra_us(dest,0,j)->B,
                                   dest->stride/sizeof(unsigned short),
                                   mask, (int)dheight ) ||
                (bConvolveInAlphaChannel &&
                 !gan_convolve1D_us(&gan_image_get_pixptr_bgra_us(image,0,j)->A,
                                    image->stride/sizeof(unsigned short),
                                    eEdgeBehaviour, GAN_TRUE,
                                    &gan_image_get_pixptr_bgra_us(dest,0,j)->A,
                                    dest->stride/sizeof(unsigned short),
                                    mask, (int)dheight )) )
           {
              gan_err_register ( "image_convolve1Dy_bgra_bgra", GAN_ERROR_FAILURE, "" );
              return NULL;
           }

        break;
        
      default:
        gan_err_flush_trace();
        gan_err_register ( "image_convolve1Dy_bgra_bgra", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
        return NULL;
   }

   /* check if we aborted the loop early because of an error */
   if ( j >= 0 )
   {
      gan_err_register ( "image_convolve1Dy_bgra_bgra", GAN_ERROR_FAILURE, "");
      return NULL;
   }

   /* success */
   return dest;
}

/**
 * \brief Convolves an image in the y-direction.
 * \param image The input image
 * \param channel Colour channel to be convolved where applicable
 * \param eEdgeBehaviour Behaviour of data beyond edge
 * \param mask The image convolution mask
 * \param dest The destination image for the convolution operation
 *
 * Applies a one-dimensional convolution operation to the
 * given image in the y-direction. When the image contains
 * colour or vector field data, a particular colour channel/vector
 * field element can be specified by the channel argument, which
 * should otherwise be passed as #GAN_ALL_CHANNELS.
 * There is no checking for overflow of integer values.
 *
 * Macro call to gan_image_convolve1Dy_q().
 *
 * \return Non-\c NULL on successfully returning the destination image \a dest,
 * \c NULL on failure.
 * \sa gan_image_convolve1Dy_q, gan_gauss_mask_new().
 */
Gan_Image *
 gan_image_convolve1Dy_q ( const Gan_Image *image, Gan_ImageChannelType channel,
                           Gan_EdgeBehaviour eEdgeBehaviour,
                           Gan_Mask1D *mask, Gan_Image *dest )
{
   switch ( image->format )
   {
      case GAN_GREY_LEVEL_IMAGE:
      switch ( channel )
      {
         case GAN_INTENSITY_CHANNEL:
         case GAN_ALL_CHANNELS:
           dest = image_convolve1Dy_gl_gl ( image, eEdgeBehaviour, mask,
                                            dest );
           break;

         case GAN_X_CHANNEL:
         case GAN_Y_CHANNEL:
         case GAN_Z_CHANNEL:
           dest = image_convolve1Dy_gl_v3D ( image, channel, eEdgeBehaviour,
                                             mask, dest );
           break;

         default:
           gan_err_flush_trace();
           gan_err_register ( "gan_image_convolve1Dy_q",
                              GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
           return NULL;
      }
      break;

      case GAN_VECTOR_FIELD_3D:
      switch ( channel )
      {
         case GAN_ALL_CHANNELS:
           dest = image_convolve1Dy_v3D_v3D ( image, eEdgeBehaviour, mask,
                                              dest );
           break;

         case GAN_X_CHANNEL:
         case GAN_Y_CHANNEL:
         case GAN_Z_CHANNEL:
           dest = image_convolve1Dy_v3D_gl ( image, channel, eEdgeBehaviour,
                                             mask, dest );
           break;

         default:
           gan_err_flush_trace();
           gan_err_register ( "gan_image_convolve1Dy_q",
                              GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
           return NULL;
      }
      break;

      case GAN_RGB_COLOUR_IMAGE:
      switch ( channel )
      {
         case GAN_ALL_CHANNELS:
           dest = image_convolve1Dy_rgb_rgb ( image, eEdgeBehaviour, mask,
                                              dest );
           break;

         case GAN_RED_CHANNEL:
         case GAN_GREEN_CHANNEL:
         case GAN_BLUE_CHANNEL:
           dest = image_convolve1Dy_rgb_gl ( image, channel, eEdgeBehaviour,
                                             mask, dest );
           break;

         default:
           gan_err_flush_trace();
           gan_err_register ( "gan_image_convolve1Dy_q",
                              GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
           return NULL;
      }
      break;

      case GAN_RGB_COLOUR_ALPHA_IMAGE:
      switch ( channel )
      {
         case GAN_ALL_CHANNELS:
           dest = image_convolve1Dy_rgba_rgba ( image, GAN_TRUE, eEdgeBehaviour, mask, dest );
           break;

         case GAN_ALL_COLOUR_CHANNELS:
           dest = image_convolve1Dy_rgba_rgba ( image, GAN_FALSE, eEdgeBehaviour, mask, dest );
           break;

         case GAN_RED_CHANNEL:
         case GAN_GREEN_CHANNEL:
         case GAN_BLUE_CHANNEL:
         case GAN_ALPHA_CHANNEL:
           dest = image_convolve1Dy_rgba_gl ( image, channel, eEdgeBehaviour,
                                              mask, dest );
           break;

         default:
           gan_err_flush_trace();
           gan_err_register ( "gan_image_convolve1Dy_q",
                              GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
           return NULL;
      }
      break;      

      case GAN_BGR_COLOUR_IMAGE:
      switch ( channel )
      {
         case GAN_ALL_CHANNELS:
           dest = image_convolve1Dy_bgr_bgr ( image, eEdgeBehaviour, mask,
                                              dest );
           break;

         case GAN_RED_CHANNEL:
         case GAN_GREEN_CHANNEL:
         case GAN_BLUE_CHANNEL:
           dest = image_convolve1Dy_bgr_gl ( image, channel, eEdgeBehaviour,
                                             mask, dest );
           break;

         default:
           gan_err_flush_trace();
           gan_err_register ( "gan_image_convolve1Dy_q",
                              GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
           return NULL;
      }
      break;

      case GAN_BGR_COLOUR_ALPHA_IMAGE:
      switch ( channel )
      {
         case GAN_ALL_CHANNELS:
           dest = image_convolve1Dy_bgra_bgra ( image, GAN_TRUE, eEdgeBehaviour, mask, dest );
           break;

         case GAN_ALL_COLOUR_CHANNELS:
           dest = image_convolve1Dy_bgra_bgra ( image, GAN_FALSE, eEdgeBehaviour, mask, dest );
           break;

         case GAN_RED_CHANNEL:
         case GAN_GREEN_CHANNEL:
         case GAN_BLUE_CHANNEL:
         case GAN_ALPHA_CHANNEL:
           dest = image_convolve1Dy_bgra_gl ( image, channel, eEdgeBehaviour,
                                              mask, dest );
           break;

         default:
           gan_err_flush_trace();
           gan_err_register ( "gan_image_convolve1Dy_q",
                              GAN_ERROR_ILLEGAL_IMAGE_CHANNEL, "" );
           return NULL;
      }
      break;      

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_image_convolve1Dy_q", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
        return NULL;
   }

   if ( dest == NULL )
   {
      gan_err_register ( "gan_image_convolve1Dy_q", GAN_ERROR_FAILURE, "" );
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

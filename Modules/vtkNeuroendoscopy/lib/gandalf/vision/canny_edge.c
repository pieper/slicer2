/**
 * File:          $RCSfile: canny_edge.c,v $
 * Module:        Canny edge detection algorithm
 * Part of:       Gandalf Library
 *
 * Revision:      $Revision: 1.1.2.1 $
 * Last edited:   $Date: 2007/10/14 02:33:16 $
 * Author:        $Author: ruetz $
 * Copyright:     (c) 2000 Imagineer Software Limited
 *
 * History:       Modified from original TINA implementation
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
#include <float.h>
#include <gandalf/vision/canny_edge.h>
#include <gandalf/vision/convolve1D.h>
#include <gandalf/common/misc_error.h>
#include <gandalf/common/numerics.h>
#include <gandalf/common/allocate.h>
#include <gandalf/linalg/3x3matrix_eigsym.h>
#include <gandalf/image/image_gl_uchar.h>
#include <gandalf/image/image_gl_int.h>
#include <gandalf/image/image_gl_float.h>
#include <gandalf/image/image_gl_double.h>
#include <gandalf/image/image_rgb_uchar.h>
#include <gandalf/image/image_rgb_int.h>
#include <gandalf/image/image_rgb_float.h>
#include <gandalf/image/image_rgb_double.h>
#include <gandalf/image/image_vfield3D_float.h>
#include <gandalf/image/image_convert.h>
#include <gandalf/image/image_pointer.h>
#include <gandalf/image/image_bit.h>

/**
 * \addtogroup Vision
 * \{
 */

/**
 * \defgroup FeatureDetection Feature Detection
 * \{
 */

/* edge status field values */
#define GAN_ISOLATED     0
#define GAN_IN_STRING 1024

/* structure to hold RGB gradients for a pixel */
typedef struct RGBGrad
{
   float Rx, Gx, Bx, Ry, Gy, By;
} RGBGrad;
 
/* compute image x & y gradients, and the RMS image gradient (square-root
 * of the sum-of-squares of x & y gradients), writing the three values into
 * the three planes of a 3D vector field image
 */
static Gan_Image *
 compute_gradients ( Gan_Image *image,
                     Gan_RGBPixel_f *colour, Gan_Vector2_f *direction )
{
   Gan_Image *grad;

   gan_err_test_ptr ( image->width > 1 && image->height > 1,
                      "compute_gradients", GAN_ERROR_IMAGE_TOO_SMALL, "" );

   if ( direction == NULL )
   {
      if ( colour == NULL || image->format == GAN_GREY_LEVEL_IMAGE )
      {
         switch ( image->format )
         {
            case GAN_GREY_LEVEL_IMAGE:
            {
               Gan_Vector3_f *gptr, *gend;
               int r;

               /* create result image */
               grad = gan_image_alloc_vfield3D_f ( image->height-2, image->width-2 );
               if ( grad == NULL )
               {
                  gan_err_register ( "compute_gradients", GAN_ERROR_FAILURE, "" );
                  return NULL;
               }

               /* calculate gradients */
               switch ( image->type )
               {
                  case GAN_UCHAR:
                  {
                     unsigned char *pptr, *imptr, *nptr;

                     for ( r = (int)grad->height-1; r >= 0; r-- )
                        for ( pptr = gan_image_get_pixptr_gl_uc ( image, r, 1 ),
                              imptr = gan_image_get_pixptr_gl_uc ( image, r+1, 1 ),
                              nptr = gan_image_get_pixptr_gl_uc ( image, r+2, 1 ),
                              gptr = gan_image_get_pixptr_vfield3D_f ( grad, r, 0 ),
                              gend = gptr+grad->width; gptr != gend;
                              pptr++, imptr++, nptr++, gptr++ )
                        {
                           gptr->x = (float)imptr[1] - (float)imptr[-1];
                           gptr->y = (float)*nptr - (float)*pptr;
                           gptr->z = (float)sqrt(gptr->x*gptr->x + gptr->y*gptr->y);
                        }
                  }
                  break;

                  case GAN_INT:
                  {
                     int *pptr, *imptr, *nptr;

                     for ( r = (int)grad->height-1; r >= 0; r-- )
                        for ( pptr = gan_image_get_pixptr_gl_i ( image, r, 1 ),
                              imptr = gan_image_get_pixptr_gl_i ( image, r+1, 1 ),
                              nptr = gan_image_get_pixptr_gl_i ( image, r+2, 1 ),
                              gptr = gan_image_get_pixptr_vfield3D_f ( grad, r, 0 ),
                              gend = gptr+grad->width; gptr != gend;
                              pptr++, imptr++, nptr++, gptr++ )
                        {
                           gptr->x = (float)imptr[1] - (float)imptr[-1];
                           gptr->y = (float)*nptr - (float)*pptr;
                           gptr->z = (float)sqrt(gptr->x*gptr->x + gptr->y*gptr->y);
                        }
                  }
                  break;

                  case GAN_FLOAT:
                  {
                     float *pptr, *imptr, *nptr;

                     for ( r = (int)grad->height-1; r >= 0; r-- )
                        for ( pptr = gan_image_get_pixptr_gl_f ( image, r, 1 ),
                              imptr = gan_image_get_pixptr_gl_f ( image, r+1, 1 ),
                              nptr = gan_image_get_pixptr_gl_f ( image, r+2, 1 ),
                              gptr = gan_image_get_pixptr_vfield3D_f ( grad, r, 0 ),
                              gend = gptr+grad->width; gptr != gend;
                              pptr++, imptr++, nptr++, gptr++ )
                        {
                           gptr->x = imptr[1] - imptr[-1];
                           gptr->y = *nptr - *pptr;
                           gptr->z = (float)sqrt(gptr->x*gptr->x + gptr->y*gptr->y);
                        }
                  }
                  break;

                  case GAN_DOUBLE:
                  {
                     double *pptr, *imptr, *nptr;

                     for ( r = (int)grad->height-1; r >= 0; r-- )
                        for ( pptr = gan_image_get_pixptr_gl_d ( image, r, 1 ),
                              imptr = gan_image_get_pixptr_gl_d ( image, r+1, 1 ),
                              nptr = gan_image_get_pixptr_gl_d ( image, r+2, 1 ),
                              gptr = gan_image_get_pixptr_vfield3D_f ( grad, r, 0 ),
                              gend = gptr+grad->width; gptr != gend;
                              pptr++, imptr++, nptr++, gptr++ )
                        {
                           gptr->x = (float)(imptr[1] - imptr[-1]);
                           gptr->y = (float)(*nptr - *pptr);
                           gptr->z = (float)sqrt(gptr->x*gptr->x + gptr->y*gptr->y);
                        }
                  }
                  break;

                  default:
                    gan_err_flush_trace();
                    gan_err_register ( "compute_gradients", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
                    return NULL;
               }
            }
            break;

            case GAN_RGB_COLOUR_IMAGE:
            {
               RGBGrad *agGrad, **gptr, **gend;
               int r, c;

               /* create result pointer image, storing pointers to 6 gradient values
                  per pixel: (dR/dx dG/dx dB/dx dR/dy dG/dy dB/dy) */
               grad = gan_image_alloc_p ( image->height-2, image->width-2 );
               if ( grad == NULL )
               {
                  gan_err_register ( "compute_gradients", GAN_ERROR_FAILURE, "" );
                  return NULL;
               }

               /* create array of gradient values */
               agGrad = gan_malloc_array ( RGBGrad, image->height*image->width );
               if ( agGrad == NULL )
               {
                  gan_err_flush_trace();
                  gan_err_register_with_number ( "compute_gradients", GAN_ERROR_MALLOC_FAILED, "", image->width*image->height*sizeof(RGBGrad) );
                  return NULL;
               }

               /* insert pointers to gradient values */
               for ( r = (int)grad->height-1; r >= 0; r-- )
                  for ( c = (int)grad->width-1; c >= 0; c-- )
                     gan_image_set_pix_p ( grad, r, c, (void *)(agGrad++) );
         
               /* calculate gradients */
               switch ( image->type )
               {
                  case GAN_UCHAR:
                  {
                     Gan_RGBPixel_uc *pptr, *imptr, *nptr;

                     for ( r = (int)grad->height-1; r >= 0; r-- )
                        for ( pptr = gan_image_get_pixptr_rgb_uc ( image, r, 1 ),
                              imptr = gan_image_get_pixptr_rgb_uc ( image, r+1, 1 ),
                              nptr = gan_image_get_pixptr_rgb_uc ( image, r+2, 1 ),
                              gptr = (RGBGrad **)gan_image_get_pixptr_p (grad, r, 0),
                              gend = gptr+grad->width; gptr != gend;
                              pptr++, imptr++, nptr++, gptr++ )
                        {
                           (*gptr)->Rx = (float)imptr[1].R - (float)imptr[-1].R;
                           (*gptr)->Gx = (float)imptr[1].G - (float)imptr[-1].G;
                           (*gptr)->Bx = (float)imptr[1].B - (float)imptr[-1].B;
                           (*gptr)->Ry = (float)nptr->R - (float)pptr->R;
                           (*gptr)->Gy = (float)nptr->G - (float)pptr->G;
                           (*gptr)->By = (float)nptr->B - (float)pptr->B;
                        }
                  }
                  break;

                  case GAN_INT:
                  {
                     Gan_RGBPixel_i *pptr, *imptr, *nptr;

                     for ( r = (int)grad->height-1; r >= 0; r-- )
                        for ( pptr = gan_image_get_pixptr_rgb_i ( image, r, 1 ),
                              imptr = gan_image_get_pixptr_rgb_i ( image, r+1, 1 ),
                              nptr = gan_image_get_pixptr_rgb_i ( image, r+2, 1 ),
                              gptr = (RGBGrad **)gan_image_get_pixptr_p (grad, r, 0),
                              gend = gptr+grad->width; gptr != gend;
                              pptr++, imptr++, nptr++, gptr++ )
                        {
                           (*gptr)->Rx = (float)imptr[1].R - (float)imptr[-1].R;
                           (*gptr)->Gx = (float)imptr[1].G - (float)imptr[-1].G;
                           (*gptr)->Bx = (float)imptr[1].B - (float)imptr[-1].B;
                           (*gptr)->Ry = (float)nptr->R - (float)pptr->R;
                           (*gptr)->Gy = (float)nptr->G - (float)pptr->G;
                           (*gptr)->By = (float)nptr->B - (float)pptr->B;
                        }
                  }
                  break;

                  case GAN_FLOAT:
                  {
                     Gan_RGBPixel_f *pptr, *imptr, *nptr;

                     for ( r = (int)grad->height-1; r >= 0; r-- )
                        for ( pptr = gan_image_get_pixptr_rgb_f ( image, r, 1 ),
                              imptr = gan_image_get_pixptr_rgb_f ( image, r+1, 1 ),
                              nptr = gan_image_get_pixptr_rgb_f ( image, r+2, 1 ),
                              gptr = (RGBGrad **)gan_image_get_pixptr_p (grad, r, 0),
                              gend = gptr+grad->width; gptr != gend;
                              pptr++, imptr++, nptr++, gptr++ )
                        {
                           (*gptr)->Rx = imptr[1].R - imptr[-1].R;
                           (*gptr)->Gx = imptr[1].G - imptr[-1].G;
                           (*gptr)->Bx = imptr[1].B - imptr[-1].B;
                           (*gptr)->Ry = nptr->R - pptr->R;
                           (*gptr)->Gy = nptr->G - pptr->G;
                           (*gptr)->By = nptr->B - pptr->B;
                        }
                  }
                  break;

                  case GAN_DOUBLE:
                  {
                     Gan_RGBPixel_d *pptr, *imptr, *nptr;

                     for ( r = (int)grad->height-1; r >= 0; r-- )
                        for ( pptr = gan_image_get_pixptr_rgb_d ( image, r, 1 ),
                              imptr = gan_image_get_pixptr_rgb_d ( image, r+1, 1 ),
                              nptr = gan_image_get_pixptr_rgb_d ( image, r+2, 1 ),
                              gptr = (RGBGrad **)gan_image_get_pixptr_p (grad, r, 0),
                              gend = gptr+grad->width; gptr != gend;
                              pptr++, imptr++, nptr++, gptr++ )
                        {
                           (*gptr)->Rx = (float)(imptr[1].R - (float)imptr[-1].R);
                           (*gptr)->Gx = (float)(imptr[1].G - (float)imptr[-1].G);
                           (*gptr)->Bx = (float)(imptr[1].B - (float)imptr[-1].B);
                           (*gptr)->Ry = (float)(nptr->R - (float)pptr->R);
                           (*gptr)->Gy = (float)(nptr->G - (float)pptr->G);
                           (*gptr)->By = (float)(nptr->B - (float)pptr->B);
                        }
                  }
                  break;

                  default:
                    gan_err_flush_trace();
                    gan_err_register ( "compute_gradients", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
                    return NULL;
               }
            }
            break;

            default:
              gan_err_flush_trace();
              gan_err_register ( "compute_gradients", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
              return NULL;
         }
      }
      else
      {
         Gan_RGBPixel_f unit_colour;

         if ( gan_vec3f_unit_q ( (Gan_Vector3_f *)colour, (Gan_Vector3_f *)&unit_colour ) == NULL )
         {
            gan_err_register ( "compute_gradients", GAN_ERROR_FAILURE, "" );
            return NULL;
         }
            
         switch ( image->format )
         {
            case GAN_RGB_COLOUR_IMAGE:
            {
               Gan_Vector3_f *gptr, *gend;
               int r;

               /* create result image */
               grad = gan_image_alloc_vfield3D_f ( image->height-2, image->width-2 );
               if ( grad == NULL )
               {
                  gan_err_register ( "compute_gradients", GAN_ERROR_FAILURE, "" );
                  return NULL;
               }

               /* calculate gradients */
               switch ( image->type )
               {
                  case GAN_UCHAR:
                  {
                     Gan_RGBPixel_uc *pptr, *imptr, *nptr;

                     for ( r = (int)grad->height-1; r >= 0; r-- )
                        for ( pptr = gan_image_get_pixptr_rgb_uc ( image, r, 1 ),
                              imptr = gan_image_get_pixptr_rgb_uc ( image, r+1, 1 ),
                              nptr = gan_image_get_pixptr_rgb_uc ( image, r+2, 1 ),
                              gptr = gan_image_get_pixptr_vfield3D_f ( grad, r, 0 ),
                              gend = gptr+grad->width; gptr != gend;
                              pptr++, imptr++, nptr++, gptr++ )
                        {
                           gptr->x =   unit_colour.R*((float)imptr[1].R - (float)imptr[-1].R)
                                     + unit_colour.G*((float)imptr[1].G - (float)imptr[-1].G)
                                     + unit_colour.B*((float)imptr[1].B - (float)imptr[-1].B);
                           gptr->y =   unit_colour.R*((float)nptr->R - (float)pptr->R)
                                     + unit_colour.G*((float)nptr->G - (float)pptr->G)
                                     + unit_colour.B*((float)nptr->B - (float)pptr->B);
                           gptr->z = (float)sqrt(gptr->x*gptr->x + gptr->y*gptr->y);
                        }
                  }
                  break;

                  case GAN_INT:
                  {
                     Gan_RGBPixel_i *pptr, *imptr, *nptr;

                     for ( r = (int)grad->height-1; r >= 0; r-- )
                        for ( pptr = gan_image_get_pixptr_rgb_i ( image, r, 1 ),
                              imptr = gan_image_get_pixptr_rgb_i ( image, r+1, 1 ),
                              nptr = gan_image_get_pixptr_rgb_i ( image, r+2, 1 ),
                              gptr = gan_image_get_pixptr_vfield3D_f ( grad, r, 0 ),
                              gend = gptr+grad->width; gptr != gend;
                              pptr++, imptr++, nptr++, gptr++ )
                        {
                           gptr->x =   unit_colour.R*((float)imptr[1].R - (float)imptr[-1].R)
                                     + unit_colour.G*((float)imptr[1].G - (float)imptr[-1].G)
                                     + unit_colour.B*((float)imptr[1].B - (float)imptr[-1].B);
                           gptr->y =   unit_colour.R*((float)nptr->R - (float)pptr->R)
                                     + unit_colour.G*((float)nptr->G - (float)pptr->G)
                                     + unit_colour.B*((float)nptr->B - (float)pptr->B);
                           gptr->z = (float)sqrt(gptr->x*gptr->x + gptr->y*gptr->y);
                        }
                  }
                  break;

                  case GAN_FLOAT:
                  {
                     Gan_RGBPixel_f *pptr, *imptr, *nptr;

                     for ( r = (int)grad->height-1; r >= 0; r-- )
                        for ( pptr = gan_image_get_pixptr_rgb_f ( image, r, 1 ),
                              imptr = gan_image_get_pixptr_rgb_f ( image, r+1, 1 ),
                              nptr = gan_image_get_pixptr_rgb_f ( image, r+2, 1 ),
                              gptr = gan_image_get_pixptr_vfield3D_f ( grad, r, 0 ),
                              gend = gptr+grad->width; gptr != gend;
                              pptr++, imptr++, nptr++, gptr++ )
                        {
                           gptr->x =   unit_colour.R*(imptr[1].R - imptr[-1].R)
                                     + unit_colour.G*(imptr[1].G - imptr[-1].G)
                                     + unit_colour.B*(imptr[1].B - imptr[-1].B);
                           gptr->y =   unit_colour.R*(nptr->R - pptr->R)
                                     + unit_colour.G*(nptr->G - pptr->G)
                                     + unit_colour.B*(nptr->B - pptr->B);
                           gptr->z = (float)sqrt(gptr->x*gptr->x + gptr->y*gptr->y);
                        }
                  }
                  break;

                  case GAN_DOUBLE:
                  {
                     Gan_RGBPixel_d *pptr, *imptr, *nptr;

                     for ( r = (int)grad->height-1; r >= 0; r-- )
                        for ( pptr = gan_image_get_pixptr_rgb_d ( image, r, 1 ),
                              imptr = gan_image_get_pixptr_rgb_d ( image, r+1, 1 ),
                              nptr = gan_image_get_pixptr_rgb_d ( image, r+2, 1 ),
                              gptr = gan_image_get_pixptr_vfield3D_f ( grad, r, 0 ),
                              gend = gptr+grad->width; gptr != gend;
                              pptr++, imptr++, nptr++, gptr++ )
                        {
                           gptr->x =   unit_colour.R*(float)(imptr[1].R - imptr[-1].R)
                                     + unit_colour.G*(float)(imptr[1].G - imptr[-1].G)
                                     + unit_colour.B*(float)(imptr[1].B - imptr[-1].B);
                           gptr->y =   unit_colour.R*(float)(nptr->R - pptr->R)
                                     + unit_colour.G*(float)(nptr->G - pptr->G)
                                     + unit_colour.B*(float)(nptr->B - pptr->B);
                           gptr->z = (float)sqrt(gptr->x*gptr->x + gptr->y*gptr->y);
                        }
                  }
                  break;

                  default:
                    gan_err_flush_trace();
                    gan_err_register ( "compute_gradients", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
                    return NULL;
               }
            }
            break;

            default:
              gan_err_flush_trace();
              gan_err_register ( "compute_gradients", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
              return NULL;
         }
      }
   }
   else
   {
      Gan_Vector3_f dterms;
      float r2;
      Gan_Vector2_f gtmp;

      dterms.x = direction->x*direction->x;
      dterms.y = direction->y*direction->y;
      dterms.z = direction->x*direction->y;
      r2 = dterms.x+dterms.y;
      if ( r2 == 0.0F )
      {
         gan_err_flush_trace();
         gan_err_register ( "compute_gradients", GAN_ERROR_DIVISION_BY_ZERO, "" );
         return NULL;
      }

      (void)gan_vec3f_divide_i ( &dterms, r2 );
      
      if ( colour == NULL || image->format == GAN_GREY_LEVEL_IMAGE )
      {
         switch ( image->format )
         {
            case GAN_GREY_LEVEL_IMAGE:
            {
               Gan_Vector3_f *gptr, *gend;
               int r;

               /* create result image */
               grad = gan_image_alloc_vfield3D_f ( image->height-2, image->width-2 );
               if ( grad == NULL )
               {
                  gan_err_register ( "compute_gradients", GAN_ERROR_FAILURE, "" );
                  return NULL;
               }

               /* calculate gradients */
               switch ( image->type )
               {
                  case GAN_UCHAR:
                  {
                     unsigned char *pptr, *imptr, *nptr;

                     for ( r = (int)grad->height-1; r >= 0; r-- )
                        for ( pptr = gan_image_get_pixptr_gl_uc ( image, r, 1 ),
                              imptr = gan_image_get_pixptr_gl_uc ( image, r+1, 1 ),
                              nptr = gan_image_get_pixptr_gl_uc ( image, r+2, 1 ),
                              gptr = gan_image_get_pixptr_vfield3D_f ( grad, r, 0 ),
                              gend = gptr+grad->width; gptr != gend;
                              pptr++, imptr++, nptr++, gptr++ )
                        {
                           (void)gan_vec2f_fill_q ( &gtmp, (float)imptr[1] - (float)imptr[-1], (float)*nptr - (float)*pptr );
                           gptr->x = gtmp.x*dterms.x + gtmp.y*dterms.z;
                           gptr->y = gtmp.x*dterms.z + gtmp.y*dterms.y;
                           gptr->z = (float)sqrt(gptr->x*gptr->x + gptr->y*gptr->y);
                        }
                  }
                  break;

                  case GAN_INT:
                  {
                     int *pptr, *imptr, *nptr;

                     for ( r = (int)grad->height-1; r >= 0; r-- )
                        for ( pptr = gan_image_get_pixptr_gl_i ( image, r, 1 ),
                              imptr = gan_image_get_pixptr_gl_i ( image, r+1, 1 ),
                              nptr = gan_image_get_pixptr_gl_i ( image, r+2, 1 ),
                              gptr = gan_image_get_pixptr_vfield3D_f ( grad, r, 0 ),
                              gend = gptr+grad->width; gptr != gend;
                              pptr++, imptr++, nptr++, gptr++ )
                        {
                           (void)gan_vec2f_fill_q ( &gtmp, (float)imptr[1] - (float)imptr[-1], (float)*nptr - (float)*pptr );
                           gptr->x = gtmp.x*dterms.x + gtmp.y*dterms.z;
                           gptr->y = gtmp.x*dterms.z + gtmp.y*dterms.y;
                           gptr->z = (float)sqrt(gptr->x*gptr->x + gptr->y*gptr->y);
                        }
                  }
                  break;

                  case GAN_FLOAT:
                  {
                     float *pptr, *imptr, *nptr;

                     for ( r = (int)grad->height-1; r >= 0; r-- )
                        for ( pptr = gan_image_get_pixptr_gl_f ( image, r, 1 ),
                              imptr = gan_image_get_pixptr_gl_f ( image, r+1, 1 ),
                              nptr = gan_image_get_pixptr_gl_f ( image, r+2, 1 ),
                              gptr = gan_image_get_pixptr_vfield3D_f ( grad, r, 0 ),
                              gend = gptr+grad->width; gptr != gend;
                              pptr++, imptr++, nptr++, gptr++ )
                        {
                           (void)gan_vec2f_fill_q ( &gtmp, imptr[1] - imptr[-1], *nptr - *pptr );
                           gptr->x = gtmp.x*dterms.x + gtmp.y*dterms.z;
                           gptr->y = gtmp.x*dterms.z + gtmp.y*dterms.y;
                           gptr->z = (float)sqrt(gptr->x*gptr->x + gptr->y*gptr->y);
                        }
                  }
                  break;

                  case GAN_DOUBLE:
                  {
                     double *pptr, *imptr, *nptr;

                     for ( r = (int)grad->height-1; r >= 0; r-- )
                        for ( pptr = gan_image_get_pixptr_gl_d ( image, r, 1 ),
                              imptr = gan_image_get_pixptr_gl_d ( image, r+1, 1 ),
                              nptr = gan_image_get_pixptr_gl_d ( image, r+2, 1 ),
                              gptr = gan_image_get_pixptr_vfield3D_f ( grad, r, 0 ),
                              gend = gptr+grad->width; gptr != gend;
                              pptr++, imptr++, nptr++, gptr++ )
                        {
                           (void)gan_vec2f_fill_q ( &gtmp, (float)imptr[1] - (float)imptr[-1], (float)*nptr - (float)*pptr );
                           gptr->x = gtmp.x*dterms.x + gtmp.y*dterms.z;
                           gptr->y = gtmp.x*dterms.z + gtmp.y*dterms.y;
                           gptr->z = (float)sqrt(gptr->x*gptr->x + gptr->y*gptr->y);
                        }
                  }
                  break;

                  default:
                    gan_err_flush_trace();
                    gan_err_register ( "compute_gradients", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
                    return NULL;
               }
            }
            break;

            case GAN_RGB_COLOUR_IMAGE:
            {
               RGBGrad *agGrad, **gptr, **gend;
               int r, c;

               /* create result pointer image, storing pointers to 6 gradient values
                  per pixel: (dR/dx dG/dx dB/dx dR/dy dG/dy dB/dy) */
               grad = gan_image_alloc_p ( image->height-2, image->width-2 );
               if ( grad == NULL )
               {
                  gan_err_register ( "compute_gradients", GAN_ERROR_FAILURE, "" );
                  return NULL;
               }

               /* create array of gradient values */
               agGrad = gan_malloc_array ( RGBGrad, image->height*image->width );
               if ( agGrad == NULL )
               {
                  gan_err_flush_trace();
                  gan_err_register_with_number ( "compute_gradients", GAN_ERROR_MALLOC_FAILED, "", image->height*image->width*sizeof(RGBGrad) );
                  return NULL;
               }

               /* insert pointers to gradient values */
               for ( r = (int)grad->height-1; r >= 0; r-- )
                  for ( c = (int)grad->width-1; c >= 0; c-- )
                     gan_image_set_pix_p ( grad, r, c, (void *)(agGrad++) );
         
               /* calculate gradients */
               switch ( image->type )
               {
                  case GAN_UCHAR:
                  {
                     Gan_RGBPixel_uc *pptr, *imptr, *nptr;

                     for ( r = (int)grad->height-1; r >= 0; r-- )
                        for ( pptr = gan_image_get_pixptr_rgb_uc ( image, r, 1 ),
                              imptr = gan_image_get_pixptr_rgb_uc ( image, r+1, 1 ),
                              nptr = gan_image_get_pixptr_rgb_uc ( image, r+2, 1 ),
                              gptr = (RGBGrad **)gan_image_get_pixptr_p (grad, r, 0),
                              gend = gptr+grad->width; gptr != gend;
                              pptr++, imptr++, nptr++, gptr++ )
                        {
                           (void)gan_vec2f_fill_q ( &gtmp, (float)imptr[1].R - (float)imptr[-1].R, (float)nptr->R - (float)pptr->R );
                           (*gptr)->Rx = gtmp.x*dterms.x + gtmp.y*dterms.z;
                           (*gptr)->Ry = gtmp.x*dterms.z + gtmp.y*dterms.y;
                           (void)gan_vec2f_fill_q ( &gtmp, (float)imptr[1].G - (float)imptr[-1].G, (float)nptr->G - (float)pptr->G );
                           (*gptr)->Gx = gtmp.x*dterms.x + gtmp.y*dterms.z;
                           (*gptr)->Gy = gtmp.x*dterms.z + gtmp.y*dterms.y;
                           (void)gan_vec2f_fill_q ( &gtmp, (float)imptr[1].B - (float)imptr[-1].B, (float)nptr->B - (float)pptr->B );
                           (*gptr)->Bx = gtmp.x*dterms.x + gtmp.y*dterms.z;
                           (*gptr)->By = gtmp.x*dterms.z + gtmp.y*dterms.y;
                        }
                  }
                  break;

                  case GAN_INT:
                  {
                     Gan_RGBPixel_i *pptr, *imptr, *nptr;

                     for ( r = (int)grad->height-1; r >= 0; r-- )
                        for ( pptr = gan_image_get_pixptr_rgb_i ( image, r, 1 ),
                              imptr = gan_image_get_pixptr_rgb_i ( image, r+1, 1 ),
                              nptr = gan_image_get_pixptr_rgb_i ( image, r+2, 1 ),
                              gptr = (RGBGrad **)gan_image_get_pixptr_p (grad, r, 0),
                              gend = gptr+grad->width; gptr != gend;
                              pptr++, imptr++, nptr++, gptr++ )
                        {
                           (void)gan_vec2f_fill_q ( &gtmp, (float)imptr[1].R - (float)imptr[-1].R, (float)nptr->R - (float)pptr->R );
                           (*gptr)->Rx = gtmp.x*dterms.x + gtmp.y*dterms.z;
                           (*gptr)->Ry = gtmp.x*dterms.z + gtmp.y*dterms.y;
                           (void)gan_vec2f_fill_q ( &gtmp, (float)imptr[1].G - (float)imptr[-1].G, (float)nptr->G - (float)pptr->G );
                           (*gptr)->Gx = gtmp.x*dterms.x + gtmp.y*dterms.z;
                           (*gptr)->Gy = gtmp.x*dterms.z + gtmp.y*dterms.y;
                           (void)gan_vec2f_fill_q ( &gtmp, (float)imptr[1].B - (float)imptr[-1].B, (float)nptr->B - (float)pptr->B );
                           (*gptr)->Bx = gtmp.x*dterms.x + gtmp.y*dterms.z;
                           (*gptr)->By = gtmp.x*dterms.z + gtmp.y*dterms.y;
                        }
                  }
                  break;

                  case GAN_FLOAT:
                  {
                     Gan_RGBPixel_f *pptr, *imptr, *nptr;

                     for ( r = (int)grad->height-1; r >= 0; r-- )
                        for ( pptr = gan_image_get_pixptr_rgb_f ( image, r, 1 ),
                              imptr = gan_image_get_pixptr_rgb_f ( image, r+1, 1 ),
                              nptr = gan_image_get_pixptr_rgb_f ( image, r+2, 1 ),
                              gptr = (RGBGrad **)gan_image_get_pixptr_p (grad, r, 0),
                              gend = gptr+grad->width; gptr != gend;
                              pptr++, imptr++, nptr++, gptr++ )
                        {
                           (void)gan_vec2f_fill_q ( &gtmp, imptr[1].R - imptr[-1].R, nptr->R - pptr->R );
                           (*gptr)->Rx = gtmp.x*dterms.x + gtmp.y*dterms.z;
                           (*gptr)->Ry = gtmp.x*dterms.z + gtmp.y*dterms.y;
                           (void)gan_vec2f_fill_q ( &gtmp, imptr[1].G - imptr[-1].G, nptr->G - pptr->G );
                           (*gptr)->Gx = gtmp.x*dterms.x + gtmp.y*dterms.z;
                           (*gptr)->Gy = gtmp.x*dterms.z + gtmp.y*dterms.y;
                           (void)gan_vec2f_fill_q ( &gtmp, imptr[1].B - imptr[-1].B, nptr->B - pptr->B );
                           (*gptr)->Bx = gtmp.x*dterms.x + gtmp.y*dterms.z;
                           (*gptr)->By = gtmp.x*dterms.z + gtmp.y*dterms.y;
                        }
                  }
                  break;

                  case GAN_DOUBLE:
                  {
                     Gan_RGBPixel_d *pptr, *imptr, *nptr;

                     for ( r = (int)grad->height-1; r >= 0; r-- )
                        for ( pptr = gan_image_get_pixptr_rgb_d ( image, r, 1 ),
                              imptr = gan_image_get_pixptr_rgb_d ( image, r+1, 1 ),
                              nptr = gan_image_get_pixptr_rgb_d ( image, r+2, 1 ),
                              gptr = (RGBGrad **)gan_image_get_pixptr_p (grad, r, 0),
                              gend = gptr+grad->width; gptr != gend;
                              pptr++, imptr++, nptr++, gptr++ )
                        {
                           (void)gan_vec2f_fill_q ( &gtmp, (float)imptr[1].R - (float)imptr[-1].R, (float)nptr->R - (float)pptr->R );
                           (*gptr)->Rx = gtmp.x*dterms.x + gtmp.y*dterms.z;
                           (*gptr)->Ry = gtmp.x*dterms.z + gtmp.y*dterms.y;
                           (void)gan_vec2f_fill_q ( &gtmp, (float)imptr[1].G - (float)imptr[-1].G, (float)nptr->G - (float)pptr->G );
                           (*gptr)->Gx = gtmp.x*dterms.x + gtmp.y*dterms.z;
                           (*gptr)->Gy = gtmp.x*dterms.z + gtmp.y*dterms.y;
                           (void)gan_vec2f_fill_q ( &gtmp, (float)imptr[1].B - (float)imptr[-1].B, (float)nptr->B - (float)pptr->B );
                           (*gptr)->Bx = gtmp.x*dterms.x + gtmp.y*dterms.z;
                           (*gptr)->By = gtmp.x*dterms.z + gtmp.y*dterms.y;
                        }
                  }
                  break;

                  default:
                    gan_err_flush_trace();
                    gan_err_register ( "compute_gradients", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
                    return NULL;
               }
            }
            break;

            default:
              gan_err_flush_trace();
              gan_err_register ( "compute_gradients", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
              return NULL;
         }
      }
      else
      {
         Gan_RGBPixel_f unit_colour;

         if ( gan_vec3f_unit_q ( (Gan_Vector3_f *)colour, (Gan_Vector3_f *)&unit_colour ) == NULL )
         {
            gan_err_register ( "compute_gradients", GAN_ERROR_FAILURE, "" );
            return NULL;
         }
            
         switch ( image->format )
         {
            case GAN_RGB_COLOUR_IMAGE:
            {
               Gan_Vector3_f *gptr, *gend;
               int r;

               /* create result image */
               grad = gan_image_alloc_vfield3D_f ( image->height-2, image->width-2 );
               if ( grad == NULL )
               {
                  gan_err_register ( "compute_gradients", GAN_ERROR_FAILURE, "" );
                  return NULL;
               }

               /* calculate gradients */
               switch ( image->type )
               {
                  case GAN_UCHAR:
                  {
                     Gan_RGBPixel_uc *pptr, *imptr, *nptr;

                     for ( r = (int)grad->height-1; r >= 0; r-- )
                        for ( pptr = gan_image_get_pixptr_rgb_uc ( image, r, 1 ),
                              imptr = gan_image_get_pixptr_rgb_uc ( image, r+1, 1 ),
                              nptr = gan_image_get_pixptr_rgb_uc ( image, r+2, 1 ),
                              gptr = gan_image_get_pixptr_vfield3D_f ( grad, r, 0 ),
                              gend = gptr+grad->width; gptr != gend;
                              pptr++, imptr++, nptr++, gptr++ )
                        {
                           (void)gan_vec2f_fill_q ( &gtmp,
                                                      unit_colour.R*((float)imptr[1].R - (float)imptr[-1].R)
                                                    + unit_colour.G*((float)imptr[1].G - (float)imptr[-1].G)
                                                    + unit_colour.B*((float)imptr[1].B - (float)imptr[-1].B),
                                                      unit_colour.R*((float)nptr->R - (float)pptr->R)
                                                    + unit_colour.G*((float)nptr->G - (float)pptr->G)
                                                    + unit_colour.B*((float)nptr->B - (float)pptr->B) );
                           gptr->x = gtmp.x*dterms.x + gtmp.y*dterms.z;
                           gptr->y = gtmp.x*dterms.z + gtmp.y*dterms.y;
                           gptr->z = (float)sqrt(gptr->x*gptr->x + gptr->y*gptr->y);
                        }
                  }
                  break;

                  case GAN_INT:
                  {
                     Gan_RGBPixel_i *pptr, *imptr, *nptr;

                     for ( r = (int)grad->height-1; r >= 0; r-- )
                        for ( pptr = gan_image_get_pixptr_rgb_i ( image, r, 1 ),
                              imptr = gan_image_get_pixptr_rgb_i ( image, r+1, 1 ),
                              nptr = gan_image_get_pixptr_rgb_i ( image, r+2, 1 ),
                              gptr = gan_image_get_pixptr_vfield3D_f ( grad, r, 0 ),
                              gend = gptr+grad->width; gptr != gend;
                              pptr++, imptr++, nptr++, gptr++ )
                        {
                           (void)gan_vec2f_fill_q ( &gtmp,
                                                      unit_colour.R*((float)imptr[1].R - (float)imptr[-1].R)
                                                    + unit_colour.G*((float)imptr[1].G - (float)imptr[-1].G)
                                                    + unit_colour.B*((float)imptr[1].B - (float)imptr[-1].B),
                                                      unit_colour.R*((float)nptr->R - (float)pptr->R)
                                                    + unit_colour.G*((float)nptr->G - (float)pptr->G)
                                                    + unit_colour.B*((float)nptr->B - (float)pptr->B) );
                           gptr->x = gtmp.x*dterms.x + gtmp.y*dterms.z;
                           gptr->y = gtmp.x*dterms.z + gtmp.y*dterms.y;
                           gptr->z = (float)sqrt(gptr->x*gptr->x + gptr->y*gptr->y);
                        }
                  }
                  break;

                  case GAN_FLOAT:
                  {
                     Gan_RGBPixel_f *pptr, *imptr, *nptr;

                     for ( r = (int)grad->height-1; r >= 0; r-- )
                        for ( pptr = gan_image_get_pixptr_rgb_f ( image, r, 1 ),
                              imptr = gan_image_get_pixptr_rgb_f ( image, r+1, 1 ),
                              nptr = gan_image_get_pixptr_rgb_f ( image, r+2, 1 ),
                              gptr = gan_image_get_pixptr_vfield3D_f ( grad, r, 0 ),
                              gend = gptr+grad->width; gptr != gend;
                              pptr++, imptr++, nptr++, gptr++ )
                        {
                           (void)gan_vec2f_fill_q ( &gtmp,
                                                      unit_colour.R*(imptr[1].R - imptr[-1].R)
                                                    + unit_colour.G*(imptr[1].G - imptr[-1].G)
                                                    + unit_colour.B*(imptr[1].B - imptr[-1].B),
                                                      unit_colour.R*(nptr->R - pptr->R)
                                                    + unit_colour.G*(nptr->G - pptr->G)
                                                    + unit_colour.B*(nptr->B - pptr->B) );
                           gptr->x = gtmp.x*dterms.x + gtmp.y*dterms.z;
                           gptr->y = gtmp.x*dterms.z + gtmp.y*dterms.y;
                           gptr->z = (float)sqrt(gptr->x*gptr->x + gptr->y*gptr->y);
                        }
                  }
                  break;

                  case GAN_DOUBLE:
                  {
                     Gan_RGBPixel_d *pptr, *imptr, *nptr;

                     for ( r = (int)grad->height-1; r >= 0; r-- )
                        for ( pptr = gan_image_get_pixptr_rgb_d ( image, r, 1 ),
                              imptr = gan_image_get_pixptr_rgb_d ( image, r+1, 1 ),
                              nptr = gan_image_get_pixptr_rgb_d ( image, r+2, 1 ),
                              gptr = gan_image_get_pixptr_vfield3D_f ( grad, r, 0 ),
                              gend = gptr+grad->width; gptr != gend;
                              pptr++, imptr++, nptr++, gptr++ )
                        {
                           (void)gan_vec2f_fill_q ( &gtmp,
                                                      unit_colour.R*((float)imptr[1].R - (float)imptr[-1].R)
                                                    + unit_colour.G*((float)imptr[1].G - (float)imptr[-1].G)
                                                    + unit_colour.B*((float)imptr[1].B - (float)imptr[-1].B),
                                                      unit_colour.R*((float)nptr->R - (float)pptr->R)
                                                    + unit_colour.G*((float)nptr->G - (float)pptr->G)
                                                    + unit_colour.B*((float)nptr->B - (float)pptr->B) );
                           gptr->x = gtmp.x*dterms.x + gtmp.y*dterms.z;
                           gptr->y = gtmp.x*dterms.z + gtmp.y*dterms.y;
                           gptr->z = (float)sqrt(gptr->x*gptr->x + gptr->y*gptr->y);
                        }
                  }
                  break;

                  default:
                    gan_err_flush_trace();
                    gan_err_register ( "compute_gradients", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "" );
                    return NULL;
               }
            }
            break;

            default:
              gan_err_flush_trace();
              gan_err_register ( "compute_gradients", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
              return NULL;
         }
      }
   }

   /* success */
   return grad;
}

/* LUT for sets of three directions to look for the next edge, given the
 * current edge normal direction in the range 0 to 36 inclusive, in 10
 * degree steps.
 */
static unsigned short lcdleft[37][3] = {
  {6,5,7},{6,7,5},{6,7,5}, {7,6,0},{7,6,0},{7,0,6}, {7,0,6},{0,7,1},{0,7,1},
  {0,7,1},{0,1,7},{0,1,7}, {1,0,2},{1,0,2},{1,2,0}, {1,2,0},{2,1,3},{2,1,3},
  {2,1,3},{2,3,1},{2,3,1}, {3,2,4},{3,2,4},{3,4,2}, {3,4,2},{4,3,5},{4,3,5},
  {4,3,5},{4,5,3},{4,5,3}, {5,4,6},{5,4,6},{5,6,4}, {5,6,4},{6,5,7},{6,5,7},
  {6,5,7}};
static unsigned short lcdright[37][3] = {
  {2,1,3},{2,3,1},{2,3,1}, {3,2,4},{3,2,4},{3,4,2}, {3,4,2},{4,3,5},{4,3,5},
  {4,3,5},{4,5,3},{4,5,3}, {5,4,6},{5,4,6},{5,6,4}, {5,6,4},{6,5,7},{6,5,7},
  {6,5,7},{6,7,5},{6,7,5}, {7,6,0},{7,6,0},{7,0,6}, {7,0,6},{0,7,1},{0,7,1},
  {0,7,1},{0,1,7},{0,1,7}, {1,0,2},{1,0,2},{1,2,0}, {1,2,0},{2,1,3},{2,1,3},
  {2,1,3}};

/* mask for orientation step number */
#define ORIENT_MASK 0x3f

/* LUT for offsets of neighbour, given the direction of the neighbour as an
 * integer from 0 to 7, in steps of pi/4 radians anticlockwise from the
 * positive x-axis.
 */
static short rcoffs_r[8] = {0, -1, -1, -1,  0,  1, 1, 1},
             rcoffs_c[8] = {1,  1,  0, -1, -1, -1, 0, 1};

/* LUT for arc-tan values in radians, rescaling the range [0,1] into
 * integer values from 0 to ATAN_NUMBER, resulting in an angle in the
 * range [0,pi/4].
 */
#define ATAN_NUMBER   100
#define ATAN_NUMBER_F 100.0F
static float arctan[ATAN_NUMBER+1];
static Gan_Bool arctan_luts_initialised = GAN_FALSE;

/* init_arctan_luts()
 *
 * Set up look up tables for Canny operator.
 */
static void init_arctan_luts(void)
{
   int i;

   for ( i = 0; i <= ATAN_NUMBER; i++ )
      arctan[i] = (float) atan ((double)i/(double)ATAN_NUMBER);

   arctan_luts_initialised = GAN_TRUE;
}

/* orientf()
 *
 * Returns an angle between -pi and pi, corresponding to the edge normal
 * direction given x/y image gradients
 */
static float orientf ( float y, float x )
{ 
   unsigned tangent;

   if ( fabs(x) > fabs(y) )
   {
      tangent = (unsigned) (fabs ( y*ATAN_NUMBER_F/x ) + 0.5F);
      if ( y > 0.0F )
         return ( (x > 0.0F) ?               -arctan[tangent] :
                                 -(float)M_PI+arctan[tangent] );
      else
         return ( (x > 0.0F) ?                arctan[tangent] :
                                  (float)M_PI-arctan[tangent] );
   }
   else 
   {
      tangent = (unsigned) (fabs ( x*ATAN_NUMBER_F/y ) + 0.5F);
      if ( y > 0.0F )
         return ( (x > 0.0F) ? -(float)M_PI_2+arctan[tangent] :
                               -(float)M_PI_2-arctan[tangent] );
      else
         return ( (x > 0.0F) ?  (float)M_PI_2-arctan[tangent] :
                                (float)M_PI_2+arctan[tangent] );
   }
}

#define THREE_OVER_TWO      1.5F
#define TWO_OVER_THREE      0.6667F
#define THOUSAND         1000.0F
#define RADIAN_TEN          5.729577951F
#define EIGHTEEN_AND_A_BIT 18.5F

/* Non-maximum suppression of the gradient image. Compares the value of the
 * directional image gradient with the equivalent values either side, in the
 * direction of the gradient. An edgel is defined where the directional
 * gradient value is a maximum. The lower hysteresis threshold is applied to
 * eliminate very week edgels.
 */
static Gan_EdgeFeatureMap *
 non_maximum_suppress_grey ( Gan_Image *grad, Gan_Image *mask,
                             float lower_thres, Gan_Matrix23_f *A,
                             Gan_Camera *camera,
                             Gan_LocalFeatureMapParams *lpms,
                             unsigned offset_r, unsigned offset_c,
                             Gan_EdgeFeatureMap *edge_map )
{
   Gan_Vector3_f **gradarr, *gpptr, *gptr, *gnptr;
   short or;
   unsigned int c, r;
   float gratio, agratio, strength, a, b, del, dr, dc, theta;

   /* floating-point offsets */
   float offset_rf = (float) offset_r;
   float offset_cf = (float) offset_c;

   gan_err_test_ptr ( grad->format == GAN_VECTOR_FIELD_3D &&
                      grad->type == GAN_FLOAT,
                      "non_maximum_suppress_grey", GAN_ERROR_IMAGE_TOO_SMALL,
                      "" );

   /* set up look-up tables if this is the first call */
   if ( !arctan_luts_initialised ) init_arctan_luts();

   /* allocate edge map if necessary */
   if ( edge_map == NULL )
   {
      edge_map = gan_edge_feature_map_alloc(0,0);
      if ( edge_map == NULL )
      {
         gan_err_register ( "non_maximum_suppress_grey", GAN_ERROR_FAILURE,
                            "" );
         return NULL;
      }
   }

   /* get 2D array of image gradients */
   gradarr = gan_image_get_pixarr_vfield3D_f ( grad );

   /* set edge map attributes and clear edge map */
   if ( !gan_edge_feature_map_clear ( edge_map,
                                      grad->height+2*offset_r,
                                      grad->width+2*offset_c, A, camera,
                                      lpms ) )
   {
      gan_err_register ( "non_maximum_suppress_grey", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* return without any edges if the region is too small */
   if ( grad->width < 2 || grad->height < 2 )
   {
      gan_edge_feature_map_postprocess ( edge_map );
      return edge_map;
   }

   /* adjust height and width for start point */
   for ( r = grad->height-2; r > 0; r-- )
      for ( c = grad->width-2, gpptr = gradarr[r-1]+c, gptr = gradarr[r]+c,
            gnptr = gradarr[r+1]+c; c > 0; c--, gpptr--, gptr--, gnptr-- )
      {
         /* ignore pixel if mask is zero there */
         if ( mask != NULL &&
              !gan_image_get_pix_b ( mask, r+offset_r, c+offset_c ) )
            continue;

         if ( (strength = gptr[0].z) < lower_thres ) continue;

         if (gptr->y == 0.0F) gratio = THOUSAND;
         else                 gratio = gptr->x/gptr->y;

         agratio = (float)fabs(gratio);
         if ( agratio < THREE_OVER_TWO &&
              agratio > TWO_OVER_THREE )
         {
            if ( ( strength <= gptr[-1].z || strength < gptr[1].z ) &&
                 ( strength <= gpptr[0].z || strength < gnptr[0].z ) )
               continue; 

            if ( gratio > 0 ) /* right diagonal */
            {
               if ( strength <= gpptr[-1].z ||
                    strength < gnptr[1].z ) continue;

               a   = gpptr[-1].z;
               b   = gnptr[1].z;
               del = (a - b)/((a + b - strength*2.0F)*2.0F);
               if ( fabs(del) > 0.5F ) continue;

               dr = del + 0.5F; 
               dc = del + 0.5F;
            }
            else
            {
               if ( strength <= gnptr[-1].z || strength < gpptr[1].z )
                  continue;

               a   = gnptr[-1].z;
               b   = gpptr[1].z;
               del = (a - b)/((a + b - strength*2.0F)*2.0F);
               if ( fabs(del) > 0.5F ) continue;

               dr = 0.5F - del;    
               dc = del + 0.5F;
            }
         }
         else if ( fabs(gptr->x) > fabs(gptr->y) )
         {
            if ( strength <= gptr[-1].z || strength < gptr[1].z ) continue;

            a   = gptr[-1].z;
            b   = gptr[1].z;
            del = (a - b)/((a + b - strength*2.0F)*2.0F);
            if ( fabs(del) > 0.5F ) continue;

            dr = 0.5F;
            dc = del + 0.5F;
         }
         else
         {  
            if ( strength <= gpptr[0].z || strength < gnptr[0].z) continue;

            a   = gpptr[0].z;
            b   = gnptr[0].z;
            del = (a - b)/((a + b - strength*2.0F)*2.0F);
            if ( fabs(del) > 0.5F ) continue;

            dr = del + 0.5F;
            dc = 0.5F;   /* pixel centre */
         }

         /* compute edge normal orientation */
         theta = orientf ( gptr->y, gptr->x );

         /* determine edge normal orientation in units of 10 degrees,
            from 0 to 36 w.r.t. negative x-axis */
         or = (short) (theta*RADIAN_TEN + EIGHTEEN_AND_A_BIT);

         /* add edge to edge map */
         if ( gan_edge_feature_add ( edge_map,  offset_r+r, offset_c+c,
                                     offset_rf + (float) r + dr,
                                     offset_cf + (float) c + dc,
                                     -theta, 1.0F, GAN_ISOLATED, or, strength )
              == NULL )
         {
            gan_err_register ( "non_maximum_suppress_grey", GAN_ERROR_FAILURE,
                               "" );
            return NULL;
         }
      }

   /* postprocess feature map, building index array into local feature map */
   gan_edge_feature_map_postprocess ( edge_map );

   /* success */
   return edge_map;
}

static Gan_EdgeFeatureMap *
 non_maximum_suppress_rgb ( Gan_Image *grad, Gan_Image *mask,
                            float lower_thres, Gan_Matrix23_f *A,
                            Gan_Camera *camera,
                            Gan_LocalFeatureMapParams *lpms,
                            unsigned offset_r, unsigned offset_c,
                            Gan_EdgeFeatureMap *edge_map )
{
   RGBGrad ***gradarr, **gpptr, **gptr, **gnptr, *g;
   short or;
   double lower_thres2 = (double)lower_thres*(double)lower_thres;
   unsigned int c, r;
   float gratio, agratio, strength, a, b, del, dr, dc, theta;
   Gan_SquMatrix33 As;
   Gan_Vector3 W;
   Gan_Matrix33 Z;
   Gan_Vector3_f cvec;
   Gan_Vector2_f cgrad;
   float clg, crg, cpg, cng;

   /* floating-point offsets */
   float offset_rf = (float) offset_r;
   float offset_cf = (float) offset_c;

   gan_err_test_ptr ( grad->format == GAN_GREY_LEVEL_IMAGE &&
                      grad->type == GAN_POINTER,
                      "non_maximum_suppress_rgb", GAN_ERROR_IMAGE_TOO_SMALL,
                      "" );

   /* set up look-up tables if this is the first call */
   if ( !arctan_luts_initialised ) init_arctan_luts();

   /* allocate edge map if necessary */
   if ( edge_map == NULL )
   {
      edge_map = gan_edge_feature_map_alloc(0,0);
      if ( edge_map == NULL )
      {
         gan_err_register ( "non_maximum_suppress_rgb", GAN_ERROR_FAILURE,
                            "" );
         return NULL;
      }
   }

   /* get 2D array of image gradients */
   gradarr = (RGBGrad ***)gan_image_get_pixarr_p(grad);

   /* set edge map attributes and clear edge map */
   if ( !gan_edge_feature_map_clear ( edge_map,
                                      grad->height+2*offset_r,
                                      grad->width+2*offset_c, A, camera,
                                      lpms ) )
   {
      gan_err_register ( "non_maximum_suppress_rgb", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* return without any edges if the region is too small */
   if ( grad->width < 2 || grad->height < 2 )
   {
      gan_edge_feature_map_postprocess ( edge_map );
      return edge_map;
   }

   /* adjust height and width for start point */
   for ( r = grad->height-2; r > 0; r-- )
      for ( c = grad->width-2, gpptr = gradarr[r-1]+c, gptr = gradarr[r]+c,
            gnptr = gradarr[r+1]+c; c > 0; c--, gpptr--, gptr--, gnptr-- )
      {
         /* ignore pixel if mask is zero there */
         if ( mask != NULL &&
              !gan_image_get_pix_b ( mask, r+offset_r, c+offset_c ) )
            continue;

         /* build matrix of squared gradients */
         g = gptr[0];
         (void)gan_symmat33_fill_q ( &As,
                                     g->Rx*g->Rx + g->Ry*g->Ry,
                                     g->Gx*g->Rx + g->Gy*g->Ry,
                                     g->Gx*g->Gx + g->Gy*g->Gy,
                                     g->Bx*g->Rx + g->By*g->Ry,
                                     g->Bx*g->Gx + g->By*g->Gy,
                                     g->Bx*g->Bx + g->By*g->By );

         /* check that gradients are "reasonably" large */
         if ( gan_symmat33_trace_q(&As) < lower_thres2 ) continue;

         /* compute eigenvalues & eigenvectors */
         if ( !gan_symmat33_eigen ( &As, &W, &Z ) )
         {
            gan_err_register ( "non_maximum_suppress_rgb", GAN_ERROR_FAILURE,
                               "" );
            return NULL;
         }

         /* build coeffients of colour vector from eigenvector corresponding
            to largest eigenvalue */
         (void)gan_vec3f_fill_q ( &cvec,
                                  (float)Z.xz, (float)Z.yz, (float)Z.zz );

         /* change sign if colour vector points away from "real" colour
            space */
         if ( cvec.x + cvec.y + cvec.z < 0.0F )
            (void)gan_vec3f_negate_i ( &cvec );

         /* compute gradient and square root of sum of squares of gradients */
         cgrad.x = cvec.x*g->Rx + cvec.y*g->Gx + cvec.z*g->Bx;
         cgrad.y = cvec.x*g->Ry + cvec.y*g->Gy + cvec.z*g->By;
         
         if ( (strength = (float)sqrt(cgrad.x*cgrad.x + cgrad.y*cgrad.y))
              < lower_thres )
            continue;

         /* compute gradients for pixel on current row to left */
         g = gptr[-1];
         dc = cvec.x*g->Rx + cvec.y*g->Gx + cvec.z*g->Bx;
         dr = cvec.x*g->Ry + cvec.y*g->Gy + cvec.z*g->By;
         clg = (float)sqrt(dc*dc + dr*dr);

         /* compute gradients for pixel on current row to right */
         g = gptr[1];
         dc = cvec.x*g->Rx + cvec.y*g->Gx + cvec.z*g->Bx;
         dr = cvec.x*g->Ry + cvec.y*g->Gy + cvec.z*g->By;
         crg = (float)sqrt(dc*dc + dr*dr);

         /* pixel on previous row */
         g = gpptr[0];
         dc = cvec.x*g->Rx + cvec.y*g->Gx + cvec.z*g->Bx;
         dr = cvec.x*g->Ry + cvec.y*g->Gy + cvec.z*g->By;
         cpg = (float)sqrt(dc*dc + dr*dr);

         /* compute gradients for pixel on next row */
         g = gnptr[0];
         dc = cvec.x*g->Rx + cvec.y*g->Gx + cvec.z*g->Bx;
         dr = cvec.x*g->Ry + cvec.y*g->Gy + cvec.z*g->By;
         cng = (float)sqrt(dc*dc + dr*dr);

         if (cgrad.y == 0.0F) gratio = THOUSAND;
         else                 gratio = cgrad.x/cgrad.y;

         agratio = (float)fabs(gratio);
         if ( agratio < THREE_OVER_TWO &&
              agratio > TWO_OVER_THREE )
         {
            if ( ( strength <= clg || strength < crg ) &&
                 ( strength <= cpg || strength < cng ) )
               continue; 

            if ( gratio > 0 ) /* right diagonal */
            {
               /* compute gradients for pixel on previous row to left */
               g = gpptr[-1];
               dc = cvec.x*g->Rx + cvec.y*g->Gx + cvec.z*g->Bx;
               dr = cvec.x*g->Ry + cvec.y*g->Gy + cvec.z*g->By;
               a = (float)sqrt(dc*dc + dr*dr);

               /* compute gradients for pixel on next row to right */
               g = gnptr[1];
               dc = cvec.x*g->Rx + cvec.y*g->Gx + cvec.z*g->Bx;
               dr = cvec.x*g->Ry + cvec.y*g->Gy + cvec.z*g->By;
               b = (float)sqrt(dc*dc + dr*dr);

               if ( strength <= a || strength < b ) continue;

               del = (a - b)/((a + b - strength*2.0F)*2.0F);
               if ( fabs(del) > 0.5 ) continue;

               dr = del + 0.5F; 
               dc = del + 0.5F;
            }
            else
            {
               /* compute gradients for pixel on previous row to right */
               g = gpptr[1];
               dc = cvec.x*g->Rx + cvec.y*g->Gx + cvec.z*g->Bx;
               dr = cvec.x*g->Ry + cvec.y*g->Gy + cvec.z*g->By;
               a = (float)sqrt(dc*dc + dr*dr);

               /* compute gradients for pixel on next row to left */
               g = gnptr[-1];
               dc = cvec.x*g->Rx + cvec.y*g->Gx + cvec.z*g->Bx;
               dr = cvec.x*g->Ry + cvec.y*g->Gy + cvec.z*g->By;
               b = (float)sqrt(dc*dc + dr*dr);

               if ( strength <= a || strength < b ) continue;

               del = (a - b)/((a + b - strength*2.0F)*2.0F);
               if ( fabs(del) > 0.5F ) continue;

               dr = 0.5F - del;    
               dc = del + 0.5F;
            }
         }
         else if ( fabs(cgrad.x) > fabs(cgrad.y) )
         {
            if ( strength <= clg || strength < crg ) continue;

            del = (clg - crg)/((clg + crg - strength*2.0F)*2.0F);
            if ( fabs(del) > 0.5F ) continue;

            dr = 0.5F;
            dc = del + 0.5F;
         }
         else
         {  
            if ( strength <= cpg || strength < cng) continue;

            del = (cpg - cng)/((cpg + cng - strength*2.0F)*2.0F);
            if ( fabs(del) > 0.5F ) continue;

            dr = del + 0.5F;
            dc = 0.5F;   /* pixel centre */
         }

         /* compute edge normal orientation */
         theta = orientf ( cgrad.y, cgrad.x );

         /* determine edge normal orientation in units of 10 degrees,
            from 0 to 36 w.r.t. negative x-axis */
         or = (short) (theta*RADIAN_TEN + EIGHTEEN_AND_A_BIT);

         /* add edge to edge map */
         if ( gan_edge_feature_add ( edge_map,  offset_r+r, offset_c+c,
                                     offset_rf + (float) r + dr,
                                     offset_cf + (float) c + dc,
                                     -theta, 1.0F, GAN_ISOLATED, or, strength )
              == NULL )
         {
            gan_err_register ( "non_maximum_suppress_rgb", GAN_ERROR_FAILURE,
                               "" );
            return NULL;
         }
      }

   /* postprocess feature map, building index array into local feature map */
   gan_edge_feature_map_postprocess ( edge_map );

   /* success */
   return edge_map;
}

static Gan_EdgeFeature *
 prev_neighbour ( Gan_EdgeFeature *edgel, Gan_EdgeFeature ***edges )
{
   Gan_EdgeFeature *eptr;
   int position, neigh;

   for ( position = 0; position < 3; position++ )
   {
      neigh = lcdleft[edgel->index][position];
      assert ( neigh <= 36 );
      eptr  = edges[edgel->r + rcoffs_r[neigh]][edgel->c + rcoffs_c[neigh]];
      if ( eptr != NULL ) return eptr;
   }

   return NULL;
}

static Gan_EdgeFeature *
 next_neighbour ( Gan_EdgeFeature *edgel, Gan_EdgeFeature ***edges )
{
   Gan_EdgeFeature *eptr;
   int position, neigh;

   for ( position = 0; position < 3; position++ )
   {
      neigh = lcdright[edgel->index][position];
      assert ( neigh <= 36 );
      eptr  = edges[edgel->r + rcoffs_r[neigh]][edgel->c + rcoffs_c[neigh]];
      if ( eptr != NULL ) return eptr;
   }

   return NULL;
}

static Gan_EdgeFeature *
 grow_backwards ( Gan_EdgeFeature *start, Gan_EdgeFeature *new_edge,
                  Gan_EdgeFeature ***edges, Gan_EdgeString *string )
{
   if ( new_edge == NULL ) return NULL;

   if ( new_edge == start ) /* found closed contour */
   {
      /* make list circular */
      start->next = string->first;
      string->first->prev = start;
      string->first = start;
      return start;
   }

   /* check whether this string has hit another */
   if ( new_edge->status == GAN_IN_STRING ) return NULL;

   /* add new edgel to string */
   string->first->prev = new_edge;
   new_edge->prev  = NULL;
   new_edge->next = string->first;
   new_edge->status = GAN_IN_STRING;
   string->first = new_edge;
   string->length++;

   /* recursively extend string */
   return ( grow_backwards ( start,
                             prev_neighbour ( new_edge, edges ),
                             edges, string ) );
}

static Gan_EdgeFeature *
 grow_forwards ( Gan_EdgeFeature *start, Gan_EdgeFeature *new_edge,
                 Gan_EdgeFeature ***edges, Gan_EdgeString *string )
{
   if ( new_edge == NULL ) return NULL;

   if ( new_edge == start ) /* found closed contour */
   {
      /* make list circular */
      start->prev = string->last;
      string->last->next = start;
      string->last = start;
      return start;
   }

   /* check whether this string has hit another */
   if ( new_edge->status == GAN_IN_STRING ) return NULL;

   /* add new edgel to string */
   string->last->next = new_edge;
   new_edge->prev  = string->last;
   new_edge->next = NULL;
   new_edge->status = GAN_IN_STRING;
   string->last = new_edge;
   string->length++;

   /* recursively extend string */
   return ( grow_forwards ( start,
                            next_neighbour ( new_edge, edges ),
                            edges, string ) );
}

/* link_edges()
 *
 * Takes a edge map and forms the edges into connected strings.
 * Closed contours are detected.
 */
static Gan_Bool link_edges ( Gan_EdgeFeatureMap *edge_map )
{
   Gan_Image *edge_2Darray;
   Gan_EdgeFeature ***edges, *edge;
   Gan_EdgeString *string;
   int i;

   /* zero number of strings */
   edge_map->nstrings = 0;

   /* allocate image to hold 2D array of edgels */
   edge_2Darray = gan_image_alloc_p ( edge_map->height, edge_map->width );
   if ( edge_2Darray == NULL )
   {
      gan_err_register ( "link_edges", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   /* initialize 2D edgel array with NULL */
   gan_image_fill_const_p ( edge_2Darray, NULL );
   
   /* set 2D array of edges */
   edges = (Gan_EdgeFeature ***) gan_image_get_pixarr_p(edge_2Darray);
   for ( i = edge_map->nedges-1; i >= 0; i-- )
   {
      edge = &edge_map->edge[i];
      edges[edge->r][edge->c] = edge;
   }

   for ( i = edge_map->nedges-1; i >= 0; i-- )
   {
      edge = &edge_map->edge[i];
      if ( edge->status != GAN_IN_STRING )
      {
         /* we have a new string! */
         string = gan_edge_feature_string_add ( edge_map, edge );
         edge->status = GAN_IN_STRING;
         if ( grow_forwards  ( edge, next_neighbour ( edge, edges ),
                               edges, string ) == NULL )
            grow_backwards ( string->last,
                             prev_neighbour ( edge, edges ), edges, string );
      }
   }

   /* we don't need the 2D edge array any more */
   gan_image_free ( edge_2Darray );

   /* success */
   return GAN_TRUE;
}

/* hysteresis()
 *
 * Applies hysteresis stage of Canny edge detector. Edgels below the low
 * threshold have already been rejected and edge strings formed, so the
 * strings now have to be checked to see if they have at least one
 * above-upper-threshold edge in them. Strings having smaller than
 * string_length_thresh edges are removed.
 */
static void hysteresis ( Gan_EdgeFeatureMap *edge_map,
                         float upper_thres,
                         unsigned int string_length_threshold )
{
   Gan_EdgeString *string;
   Gan_EdgeFeature *edge, *start;
   int      i;
   Gan_Bool keep_string;

   for ( i = edge_map->nstrings-1; i >= 0; i-- )
   {
      string = &edge_map->string[i];
      keep_string = GAN_FALSE;
      if ( string->length >= string_length_threshold )
      {
         start = edge = string->first;
         if ( edge->strength > upper_thres ) /* found an above-threshold edge*/
            keep_string = GAN_TRUE;
         else
            for ( edge = edge->next; edge != NULL && edge != start;
                  edge = edge->next ) /* traverse edge list in string */
               if ( edge->strength > upper_thres ) /* found an above-threshold
                                                      edgel */
               {
                  keep_string = GAN_TRUE;
                  break;
               }
      }

      if ( keep_string )
         /* reset status flags of edgels in string to be kept */
         gan_edge_feature_string_set_status ( string, GAN_IN_STRING );
      else
      {
         /* reset status flags of edgels in deleted string */
         gan_edge_feature_string_set_status ( string, GAN_ISOLATED );

         /* move end string to position of deleted string */
         edge_map->string[i] = edge_map->string[--edge_map->nstrings];
      }
   }
}

/**
 * \brief Computes edges in an image using the Canny algorithm.
 * \param image The input image
 * \param mask Binary image indicating region to find edges or \c NULL
 * \param filter_y Image convolution filter in the y-direction or \c NULL
 * \param filter_x Image convolution filter in the x-direction or \c NULL
 * \param colour Colour on which to project gradients or \c NULL
 * \param direction Direction on which to project gradients or \c NULL
 * \param lower_thres Lower hysteresis threshold for string building
 * \param upper_thres Upper hysteresis threshold for string building
 * \param string_length_thres The smallest length of string allowed
 * \param A Affine transformation to apply to edgel coordinates or \c NULL
 * \param camera Pointer to the camera calibration or \c NULL
 * \param lpms Pointer to local feature map parameters or \c NULL
 * \param edge_map The edge map to fill with edges
 * \param set_progress Function to update progress from 1 to 100  or \c NULL
 * \param progress_obj Pointer passed into \a set_progress function or \c NULL
 *
 * Applies the Canny edge detector to find edges in the
 * input image. The image is smoothed using the provided
 * convolution masks. The image x/y gradients gx/gy and the RMS
 * gradient sqrt(gx^2+gy^2) are computed. Non-maximum suppression
 * is applied to compute the individual edge points (edgels).
 * These are then joined into strings using a hysteresis
 * procedure, wherein two thresholds are applied to keep weak
 * edges so long as they are part of a string with strong edges.
 *
 * An affine transformation \a A is applied to the edgel coordinates
 * if the \a A pointer is not \c NULL.
 *
 * If the camera calibration structure pointer is not \c NULL,
 * distortion correction is applied to the edgel positions.
 * camera should be the correct camera calibration for the image
 * coordinates obtained after applying the affine
 * transformation \a A.
 *
 * \return Non-\c NULL a pointer to the computed edge map structure, or
 *         \c NULL on failure.
 * \sa gan_canny_edge_s().
 */
Gan_EdgeFeatureMap *
 gan_canny_edge_q ( Gan_Image *image, Gan_Image *mask,
                    Gan_Mask1D *filter_y, Gan_Mask1D *filter_x,
                    Gan_RGBPixel_f *colour,
                    Gan_Vector2_f *direction,
                    double lower_thres, double upper_thres,
                    unsigned string_length_thres,
                    Gan_Matrix23_f *A, Gan_Camera *camera,
                    Gan_LocalFeatureMapParams *lpms,
                    Gan_EdgeFeatureMap *edge_map,
                    void (*set_progress) ( void *, unsigned ),
                    void *progress_obj )
{
   Gan_Image *float_img, *smooth_x, *smooth_xy, *grad;
   unsigned offset_r, offset_c;

   if ( set_progress != NULL )
      set_progress ( progress_obj, 0 );

   /* check format of image */
   gan_err_test_ptr ( image->format == GAN_GREY_LEVEL_IMAGE ||
                      image->format == GAN_RGB_COLOUR_IMAGE,
                      "gan_canny_edges_q", GAN_ERROR_INCOMPATIBLE, "" );

   /* check mask if non-NULL */
   gan_err_test_ptr ( mask == NULL ||
                      (mask->format == GAN_GREY_LEVEL_IMAGE &&
                       mask->type == GAN_BOOL &&
                       mask->height == image->height &&
                       mask->width == image->width),
                      "gan_canny_edges_q", GAN_ERROR_INCOMPATIBLE, "" );

   /* if image is too small, return zero-size edge map */
   if ( image->width  < ((filter_x == NULL) ? 1 : filter_x->size) + 4 ||
        image->height < ((filter_y == NULL) ? 1 : filter_y->size) + 4 )
   {
      if ( edge_map == NULL )
      {
         edge_map = gan_edge_feature_map_alloc(0,0);
         if ( edge_map == NULL )
         {
            gan_err_register ( "gan_canny_edge_q", GAN_ERROR_FAILURE, "" );
            return NULL;
         }
      }

      if ( !gan_edge_feature_map_clear ( edge_map, 0, 0, A, camera, lpms ) )
      {
         gan_err_register ( "gan_canny_edge_q", GAN_ERROR_FAILURE, "" );
         return NULL;
      }

      gan_edge_feature_map_postprocess ( edge_map );
      return edge_map;
   }

   /* convert image to floating-point if necessary */
   if ( image->type == GAN_FLOAT )
      float_img = image;
   else
      float_img = gan_image_convert_s ( image, image->format, GAN_FLOAT );

   gan_err_test_ptr ( float_img != NULL, "gan_canny_edge_q",
                      GAN_ERROR_FAILURE, "" );

   /* smooth in x-direction if a mask was specified */
   if ( filter_x == NULL )
      smooth_x = float_img;
   else
   {
      smooth_x = gan_image_convolve1Dx_s ( float_img, GAN_ALL_CHANNELS,
                                           GAN_EDGE_BEHAVIOUR_CLIP, filter_x );
      if ( smooth_x == NULL )
      {
         gan_err_register ( "gan_canny_edge_q", GAN_ERROR_FAILURE, "" );
         return NULL;
      }
   }

   if ( set_progress != NULL )
      set_progress ( progress_obj, 5 );

   /* free floating-point image if it was allocated */
   if ( image->type != GAN_FLOAT && filter_x != NULL )
      gan_image_free ( float_img );

   /* smooth in y-direction if a mask was specified */
   if ( filter_y == NULL )
      smooth_xy = smooth_x;
   else
   {
      smooth_xy = gan_image_convolve1Dy_s ( smooth_x, GAN_ALL_CHANNELS,
                                            GAN_EDGE_BEHAVIOUR_CLIP,
                                            filter_y );
      if ( smooth_xy == NULL )
      {
         gan_err_register ( "gan_canny_edge_q", GAN_ERROR_FAILURE, "" );
         return NULL;
      }
   }

   if ( filter_y != NULL )
      gan_image_free ( smooth_x );

   if ( set_progress != NULL )
      set_progress ( progress_obj, 10 );

   /* compute the image gradients */
   grad = compute_gradients ( smooth_xy, colour, direction );
   if ( !(filter_x == NULL && filter_y == NULL) )
      gan_image_free ( smooth_xy );

   /* check that gradient calculation worked */
   if ( grad == NULL )
   {
      gan_err_register ( "gan_canny_edge_q", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   if ( set_progress != NULL )
      set_progress ( progress_obj, 20 );

   /* calculate position offsets in gradient image */
   /* calculate position offsets in gradient image */
   offset_c = (filter_x == NULL) ? 1 : filter_x->size/2 + 1;
   offset_r = (filter_y == NULL) ? 1 : filter_y->size/2 + 1;

   /* if calibration is linear, there will be no distortion correction and
      the calibration pointer might as well be NULL */
   if ( camera != NULL && camera->type == GAN_LINEAR_CAMERA ) camera = NULL;

   /* apply non-maximum suppression */
   if ( image->format == GAN_GREY_LEVEL_IMAGE )
      edge_map = non_maximum_suppress_grey ( grad, mask, (float)lower_thres, A,
                                             camera, lpms, offset_r, offset_c,
                                             edge_map );
   else if ( image->format == GAN_RGB_COLOUR_IMAGE )
      edge_map = non_maximum_suppress_rgb ( grad, mask, (float)lower_thres, A,
                                            camera, lpms, offset_r, offset_c,
                                            edge_map );
   else
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_canny_edge_q", GAN_ERROR_ILLEGAL_IMAGE_FORMAT, "" );
      return NULL;
   }

   if ( set_progress != NULL )
      set_progress ( progress_obj, 60 );

   /* free array of gradient values stored for RGB image */
   if ( grad->type == GAN_POINTER )
      free ( gan_image_get_pix_p ( grad, grad->height-1, grad->width-1 ) );

   /* free gradient image */
   gan_image_free ( grad );

   if ( edge_map == NULL )
   {
      gan_err_register ( "gan_canny_edge_q", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* construct edge strings */
   if ( !link_edges ( edge_map ) )
   {
      gan_err_register ( "gan_canny_edge_q", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   if ( set_progress != NULL )
      set_progress ( progress_obj, 80 );

   /* perform hysteresis */
   hysteresis ( edge_map, (float)upper_thres, string_length_thres );

   if ( set_progress != NULL )
      set_progress ( progress_obj, 99 );

   /* success */
   return edge_map;
}

/**
 * \}
 */

/**
 * \}
 */

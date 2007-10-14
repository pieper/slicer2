/**
 * File:          $RCSfile: harris_corner.c,v $
 * Module:        Plessey corner detector algorithm
 * Part of:       Gandalf Library
 *
 * Revision:      $Revision: 1.1.2.1 $
 * Last edited:   $Date: 2007/10/14 02:33:18 $
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
#include <float.h>
#include <gandalf/vision/harris_corner.h>
#include <gandalf/vision/convolve1D.h>
#include <gandalf/common/misc_error.h>
#include <gandalf/common/numerics.h>
#include <gandalf/image/image_gl_uchar.h>
#include <gandalf/image/image_gl_ushort.h>
#include <gandalf/image/image_gl_int.h>
#include <gandalf/image/image_gl_float.h>
#include <gandalf/image/image_vfield3D_float.h>

/**
 * \addtogroup Vision
 * \{
 */

/**
 * \addtogroup FeatureDetection
 * \{
 */

/* compute squared image x & y gradients, and the product of the x & y
 * gradients, writing the three values into the three planes of a
 * 3D vector field image
 */
static Gan_Image *
 compute_gradients ( Gan_Image *image )
{
   Gan_Image *grad;
   int r;

   Gan_Vector3_f *gptr, *gend;
   register float gx, gy;

   gan_err_test_ptr ( image->format == GAN_GREY_LEVEL_IMAGE,
                      "compute_gradients", GAN_ERROR_INCOMPATIBLE, "image" );

   gan_err_test_ptr ( image->width > 1 && image->height > 1,
                      "compute_gradients", GAN_ERROR_IMAGE_TOO_SMALL, "" );

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
            for ( pptr = gan_image_get_pixptr_gl_uc(image,r,1),
                  imptr = gan_image_get_pixptr_gl_uc(image,r+1,1),
                  nptr = gan_image_get_pixptr_gl_uc(image,r+2,1),
                  gptr = gan_image_get_pixptr_vfield3D_f(grad,r,0),
                  gend = gptr+grad->width; gptr != gend;
                  pptr++, imptr++, nptr++, gptr++ )
            {
               gx = ((float)imptr[1] - (float)imptr[-1])*(0.5F/GAN_UCHAR_MAXF);
               gy = ((float)*nptr    - (float)*pptr)*(0.5F/GAN_UCHAR_MAXF);
               gptr->x = gx*gx; gptr->y = gy*gy; gptr->z = gx*gy;
            }
      }
      break;

      case GAN_USHORT:
      {
         unsigned short *pptr, *imptr, *nptr;

         for ( r = (int)grad->height-1; r >= 0; r-- )
            for ( pptr = gan_image_get_pixptr_gl_us(image,r,1),
                  imptr = gan_image_get_pixptr_gl_us(image,r+1,1),
                  nptr = gan_image_get_pixptr_gl_us(image,r+2,1),
                  gptr = gan_image_get_pixptr_vfield3D_f(grad,r,0),
                  gend = gptr+grad->width; gptr != gend;
                  pptr++, imptr++, nptr++, gptr++ )
            {
               gx = ((float)imptr[1] - (float)imptr[-1])*(0.5F/GAN_USHRT_MAXF);
               gy = ((float)*nptr    - (float)*pptr)*(0.5F/GAN_USHRT_MAXF);
               gptr->x = gx*gx; gptr->y = gy*gy; gptr->z = gx*gy;
            }
      }
      break;

      case GAN_INT:
      {
         int *pptr, *imptr, *nptr;

         for ( r = (int)grad->height-1; r >= 0; r-- )
            for ( pptr = gan_image_get_pixptr_gl_i(image,r,1),
                  imptr = gan_image_get_pixptr_gl_i(image,r+1,1),
                  nptr = gan_image_get_pixptr_gl_i(image,r+2,1),
                  gptr = gan_image_get_pixptr_vfield3D_f(grad,r,0),
                  gend = gptr+grad->width; gptr != gend;
                  pptr++, imptr++, nptr++, gptr++ )
            {
               gx = ((float)imptr[1] - (float)imptr[-1])*(0.5F/GAN_INT_MAXF);
               gy = ((float)*nptr    - (float)*pptr)*(0.5F/GAN_INT_MAXF);
               gptr->x = gx*gx; gptr->y = gy*gy; gptr->z = gx*gy;
            }
      }
      break;

      case GAN_FLOAT:
      {
         float *pptr, *imptr, *nptr;

         for ( r = (int)grad->height-1; r >= 0; r-- )
            for ( pptr = gan_image_get_pixptr_gl_f(image,r,1),
                  imptr = gan_image_get_pixptr_gl_f(image,r+1,1),
                  nptr = gan_image_get_pixptr_gl_f(image,r+2,1),
                  gptr = gan_image_get_pixptr_vfield3D_f(grad,r,0),
                  gend = gptr+grad->width; gptr != gend;
                  pptr++, imptr++, nptr++, gptr++ )
            {
               gx = 0.5F*(imptr[1] - imptr[-1]);
               gy = 0.5F*(*nptr    - *pptr);
               gptr->x = gx*gx; gptr->y = gy*gy; gptr->z = gx*gy;
            }
      }
      break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "compute_gradients", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "image" );
        return NULL;
   }

   /* success */
   return grad;
}

/* calculate the image squared gradients, optionally pre-smoothing
   the image */
static Gan_Image *
 smooth_and_grad ( Gan_Image *image,
                   Gan_Mask1D *image_filter_y, Gan_Mask1D *image_filter_x )
{
   Gan_Image *grad;

   /* check whether any pre-smoothing of the image is being applied */
   if ( image_filter_x == NULL )
      if ( image_filter_y == NULL )
         /* no pre-smoothing of the image, so compute the gradients directly
            from the original image */
         grad = compute_gradients ( image );
      else
      {
         /* pre-smoothing in y-direction only */
         Gan_Image *smooth_y;

         /* smooth the image */
         smooth_y = gan_image_convolve1Dy_s ( image, GAN_ALL_CHANNELS,
                                              GAN_EDGE_BEHAVIOUR_CLIP,
                                              image_filter_y );
         if ( smooth_y == NULL )
         {
            gan_err_register ( "smooth_and_grad", GAN_ERROR_FAILURE, "" );
            return NULL;
         }

         /* compute the gradients */
         grad = compute_gradients ( smooth_y );
         gan_image_free ( smooth_y );
      }
   else
      if ( image_filter_y == NULL )
      {
         /* pre-smoothing in x-direction only */
         Gan_Image *smooth_x;

         /* smooth the image */
         smooth_x = gan_image_convolve1Dx_s ( image, GAN_ALL_CHANNELS,
                                              GAN_EDGE_BEHAVIOUR_CLIP,
                                              image_filter_x );
         if ( smooth_x == NULL )
         {
            gan_err_register ( "smooth_and_grad", GAN_ERROR_FAILURE, "" );
            return NULL;
         }

         /* compute the gradients */
         grad = compute_gradients ( smooth_x );
         gan_image_free ( smooth_x );
      }
      else
      {
         /* pre-smoothing in x and y directions */
         Gan_Image *smooth_x, *smooth_xy;

         /* smooth in x-direction */
         smooth_x = gan_image_convolve1Dx_s ( image, GAN_ALL_CHANNELS,
                                              GAN_EDGE_BEHAVIOUR_CLIP,
                                              image_filter_x );
         if ( smooth_x == NULL )
         {
            gan_err_register ( "smooth_and_grad", GAN_ERROR_FAILURE, "" );
            return NULL;
         }

         /* smooth in y-direction */
         smooth_xy = gan_image_convolve1Dy_s ( smooth_x, GAN_ALL_CHANNELS,
                                               GAN_EDGE_BEHAVIOUR_CLIP,
                                               image_filter_y );
         if ( smooth_xy == NULL )
         {
            gan_err_register ( "smooth_and_grad", GAN_ERROR_FAILURE, "" );
            return NULL;
         }

         gan_image_free ( smooth_x );

         /* compute the gradients */
         grad = compute_gradients ( smooth_xy );
         gan_image_free ( smooth_xy );
      }

   /* check that gradient calculation worked */
   if ( grad == NULL )
   {
      gan_err_register ( "smooth_and_grad", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   return grad;
}

/* calculate corner strength */
static Gan_Bool
 calc_strength ( Gan_Image *grad, float kappa, Gan_Image *strength )
{
   float *sptr, *spend;
   int i;

   gan_err_test_bool ( grad->format == GAN_VECTOR_FIELD_3D,
                       "calc_strength", GAN_ERROR_INCOMPATIBLE, "" );

   /* set result image attributes */
   strength = gan_image_set_format_type_dims ( strength,
                                               GAN_GREY_LEVEL_IMAGE,
                                               GAN_FLOAT,
                                               grad->height, grad->width );
   if ( strength == NULL )
   {
      gan_err_register ( "calc_strength", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   switch ( grad->type )
   {
      case GAN_FLOAT:
      {
         Gan_Vector3_f *gptr;

         for ( i = (int)strength->height-1; i >= 0; i-- )
            for ( gptr = gan_image_get_pixptr_vfield3D_f(grad,i,0),
                  sptr = gan_image_get_pixptr_gl_f(strength,i,0),
                  spend = sptr + strength->width; sptr != spend;
                  gptr++, sptr++ )
            {
               *sptr = gptr->x*gptr->y - gptr->z*gptr->z
                     - kappa*gan_sqr_f(gptr->x+gptr->y);
               if ( *sptr < 0.0F ) *sptr = 0.0F;
            }
      }
      break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "calc_strength", GAN_ERROR_ILLEGAL_IMAGE_TYPE, "gradient image" );
        break;
   }

   /* success */
   return GAN_TRUE;
}

/* apply non-maximum suppression of corner strengths to find the corners */
static Gan_CornerFeatureMap *
 find_corners ( Gan_Image *strength, float thres, Gan_Matrix23_f *A,
                Gan_Camera *camera, unsigned offset_r, unsigned offset_c,
                int status, Gan_LocalFeatureMapParams *lpms,
                Gan_CornerFeatureMap *corner_map )
{
   unsigned r, c;
   float *pptr, *sptr, *nptr, v;
   float offset_rf = (float) offset_r + 0.5F,
         offset_cf = (float) offset_c + 0.5F;

   /* quadratic parameters */
   float Pxx, Pxy, Pyy, Px, Py, det, xcorr, ycorr;

   /* allocate corner map if necessary */
   if ( corner_map == NULL )
   {
      corner_map = gan_corner_feature_map_alloc(0);
      if ( corner_map == NULL )
      {
         gan_err_register ( "find_corners", GAN_ERROR_FAILURE, "" );
         return NULL;
      }
   }

   /* set corner map attributes and clear corner map */
   if ( !gan_corner_feature_map_clear ( corner_map,
                                        strength->height+2*offset_r,
                                        strength->width+2*offset_c, A, camera,
                                        lpms ) )
   {
      gan_err_register ( "find_corners", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* return without any corners if the region is too small */
   if ( strength->width < 2 || strength->height < 2 )
   {
      gan_corner_feature_map_postprocess ( corner_map );
      return corner_map;
   }

   /* threshold should be in linear pixel space, so take fourth power to
      put in in space of corner strengths */
   thres = (float)pow(thres,4.0);

   /* search for peaks in corner strength */
   for ( r = strength->height-2; r > 0; r-- )
      for ( c = strength->width-2,
            pptr = gan_image_get_pixptr_gl_f(strength,r-1,c),
            sptr = gan_image_get_pixptr_gl_f(strength,r,c),
            nptr = gan_image_get_pixptr_gl_f(strength,r+1,c); c > 0;
            c--, pptr--, sptr--, nptr-- )
      {
         v = sptr[0];
         if ( v >= thres &&
              v >  pptr[-1] && v >  pptr[0] && v >  pptr[1] &&
              v >= sptr[-1] &&                 v >  sptr[1]    &&
              v >= nptr[-1] && v >= nptr[0] && v >= nptr[1] )
         {
            /* apply geometric fitting in image-proportional coordinates */
            float spp = (float)pow(pptr[-1],0.25),
                  spc = (float)pow(pptr[0],0.25),
                  spn = (float)pow(pptr[1],0.25);
            float scp = (float)pow(sptr[-1],0.25),
                  scc = (float)pow(v,0.25),
                  scn = (float)pow(sptr[1],0.25);
            float snp = (float)pow(nptr[-1],0.25),
                  snc = (float)pow(nptr[0],0.25),
                  snn = (float)pow(nptr[1],0.25);

            /* use least-squares to fit quadratic to local corner strengths */
            Pxx =   (spp - 2.0F*spc + spn + scp - 2.0F*scc + scn
                   + snp - 2.0F*snc + snn)/3.0F;
            Pxy = (spp - spn - snp + snn)/4.0F;
            Pyy =        (spp +      spc +      spn
                   - 2.0F*scp - 2.0F*scc - 2.0F*scn
                   +      snp +      snc +      snn)/3.0F;
            Px = (- spp - scp - snp + spn + scn + snn)/6.0F;
            Py = (- spp - spc - spn + snp + snc + snn)/6.0F;
            det = Pxy*Pxy - Pxx*Pyy;
            gan_err_test_ptr ( det != 0.0F, "find_corners",
                               GAN_ERROR_DIVISION_BY_ZERO,
                               "computing corner position" );

            /* calculate sub-pixel corrections to the corner position */
            xcorr = (Pyy*Px - Pxy*Py)/det;
            ycorr = (Pxx*Py - Pxy*Px)/det;

            /* pull the corrections inside the pixel */
            if (xcorr > 0.5F) xcorr=0.5F; else if (xcorr < -0.5F) xcorr= -0.5F;
            if (ycorr > 0.5F) ycorr=0.5F; else if (ycorr < -0.5F) ycorr= -0.5F;

            /* add corner to corner map */
            if ( gan_corner_feature_add ( corner_map,  offset_r+r, offset_c+c,
                                          offset_rf + (float) r + ycorr,
                                          offset_cf + (float) c + xcorr,
                                          NULL, status, 0, scc )
                 == NULL )
            {
               gan_err_register ( "find_corners", GAN_ERROR_FAILURE, "" );
               return NULL;
            }
         }
      }

   /* postprocess feature map, building index array into local feature map */
   gan_corner_feature_map_postprocess ( corner_map );

   /* success */
   return corner_map;
}

/**
 * \brief Computes corners in an image using the Harris algorithm.
 * \param image The input image
 * \param mask Binary image indicating region to find corners or \c NULL
 * \param image_filter_y The image convolution mask in the y-direction
 * \param image_filter_x The image convolution mask in the x-direction
 * \param grad_filter_y The gradient convolution mask in the y-direction
 * \param grad_filter_x The gradient convolution mask in the x-direction
 * \param kappa The weighting factor used to compute the corner strength
 * \param thres The threshold on corner strength
 * \param A Affine transformation to apply to corner coordinates or \c NULL
 * \param status The initial status value set for each corner
 * \param camera Pointer to the camera calibration or \c NULL
 * \param lpms Pointer to local feature map parameters or \c NULL
 * \param corner_map The corner map to fill with corners
 *
 * Applies the Harris corner detector to find corners in the
 * input image. The squared image x/y gradients gx2/gy2 and
 * product of x & y gradients gxy are computed. These are
 * convolved with the given convolution masks grad_filter_y and
 * grad_filter_x in the y/x directions respectively. The smoothed
 * gradients sgx2, sgy2 and sgxy are then combined pixel-by-pixel
 * into a corner strength
 * \f[
 *       s = sgx2\:sgy2 - sgxy^2 - \kappa(sgx2+sgy2)^2.
 * \f]
 *
 * using the provided weighting factor \a kappa.
 * The strength is non-maximum suppressed and thresholded
 * by the given value thres.
 *
 * An extra feature (from Forster's corner detector) is a
 * pre-smoothing of the image given by the
 * \a image_filter_y and \a image_filter_x convolution masks.
 *
 * An affine transformation \a A is applied to the corner coordinates
 * if the \a A pointer is not \c NULL.
 *
 * If the camera calibration structure pointer is not \c NULL,
 * distortion correction is applied to the corner positions.
 * camera should be the correct camera calibration for the image
 * coordinates obtained after applying the affine
 * transformation \a A.
 *
 * \return Non-\c NULL a pointer to the computed corner map structure,
 *         or \c NULL on failure.
 * \sa gan_harris_corner_s().
 */
Gan_CornerFeatureMap *
 gan_harris_corner_q ( Gan_Image *image, Gan_Image *mask,
                       Gan_Mask1D *image_filter_y, Gan_Mask1D *image_filter_x,
                       Gan_Mask1D *grad_filter_y,  Gan_Mask1D *grad_filter_x,
                       float kappa, float thres,
                       Gan_Matrix23_f *A, int status, Gan_Camera *camera,
                       Gan_LocalFeatureMapParams *lpms,
                       Gan_CornerFeatureMap *corner_map )
{
   Gan_Image *grad, *imbf, *smgxy;
   unsigned offset_r, offset_c;

   /* can't deal with binary mask yet */
   gan_err_test_ptr ( mask == NULL, "gan_harris_corner_q",
                      GAN_ERROR_INCOMPATIBLE, "" );

   /* only grey-level images handled currently */
   gan_err_test_ptr ( image->format == GAN_GREY_LEVEL_IMAGE,
                      "gan_harris_corner_q", GAN_ERROR_INCOMPATIBLE, "" );

   /* calculate the image squared gradients, optionally pre-smoothing
      the image */
   grad = smooth_and_grad ( image, image_filter_y, image_filter_x );

   /* check that gradient calculation worked */
   if ( grad == NULL )
   {
      gan_err_register ( "gan_harris_corner_q", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* apply convolution to gradient image, taking each plane of vector
      field image separately to save some memory */

   /* smoothing the X plane allocates the smoothed gradient images */
   imbf  = gan_image_convolve1Dx_s ( grad, GAN_X_CHANNEL,
                                     GAN_EDGE_BEHAVIOUR_CLIP, grad_filter_x );
   smgxy = gan_image_convolve1Dy_s ( imbf, GAN_X_CHANNEL,
                                     GAN_EDGE_BEHAVIOUR_CLIP, grad_filter_y );

   /* smooth the Y and Z planes */
   gan_image_convolve1Dx_q ( grad, GAN_Y_CHANNEL, GAN_EDGE_BEHAVIOUR_CLIP,
                             grad_filter_x, imbf );
   gan_image_convolve1Dy_q ( imbf, GAN_Y_CHANNEL, GAN_EDGE_BEHAVIOUR_CLIP,
                             grad_filter_y, smgxy );

   gan_image_convolve1Dx_q ( grad, GAN_Z_CHANNEL, GAN_EDGE_BEHAVIOUR_CLIP,
                             grad_filter_x, imbf );
   gan_image_convolve1Dy_q ( imbf, GAN_Z_CHANNEL, GAN_EDGE_BEHAVIOUR_CLIP,
                             grad_filter_y, smgxy );

   /* free gradient image */
   gan_image_free ( grad );

   /* calculate corner strength in image imbf */
   if ( !calc_strength ( smgxy, kappa, imbf ) )
   {
      gan_image_free_va ( smgxy, imbf, NULL );
      gan_err_register ( "gan_harris_corner_q", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* we don't need the smoothed gradients any more */
   gan_image_free ( smgxy );

   /* if calibration is linear, there will be no distortion correction and
      the calibration pointer might as well be NULL */
   if ( camera != NULL && camera->type == GAN_LINEAR_CAMERA ) camera = NULL;

   /* compute offsets of corner positions from strength image due to image
      border downsizing effects */
   offset_r = ((image_filter_y == NULL) ? 0 : image_filter_y->size/2) +
              ((grad_filter_y == NULL) ? 0 : grad_filter_y->size/2) + 1;
   offset_c = ((image_filter_x == NULL) ? 0 : image_filter_x->size/2) +
              ((grad_filter_x == NULL) ? 0 : grad_filter_x->size/2) + 1;

   /* apply non-maximum suppression of corner strengths to find the corners */
   corner_map = find_corners ( imbf, thres, A, camera, offset_r, offset_c,
                               status, lpms, corner_map );

   /* free corner strength image */
   gan_image_free ( imbf );

   if ( corner_map == NULL )
   {
      gan_err_register ( "gan_harris_corner_q", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* success */
   return corner_map;
}

/**
 * \}
 */

/**
 * \}
 */

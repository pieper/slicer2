/**
 * File:          $RCSfile: corner_feature.c,v $
 * Module:        Image corner point definition and functions
 * Part of:       Gandalf Library
 *
 * Revision:      $Revision: 1.1.2.1 $
 * Last edited:   $Date: 2007/10/14 02:33:17 $
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
#include <gandalf/vision/corner_feature.h>
#include <gandalf/vision/cameraf_linear.h>
#include <gandalf/vision/camera_convert.h>
#include <gandalf/common/misc_error.h>
#include <gandalf/common/allocate.h>
#include <gandalf/image/image_gl_int.h>

/**
 * \addtogroup Vision
 * \{
 */

/**
 * \addtogroup FeatureDetection
 * \{
 */

/**
 * \brief Forms a corner feature map structure.
 * \param cmap A corner feature map
 * \param max_ncorners The number of corners initially to allocate for
 *
 * Fills a structure to hold corner features, and sets it to be empty.
 *
 * \return non-\c NULL the formed feature map \a cmap, or \c NULL on failure.
 * \sa gan_corner_feature_map_alloc(), gan_corner_feature_map_free().
 */
Gan_CornerFeatureMap *
 gan_corner_feature_map_form ( Gan_CornerFeatureMap *cmap,
                               unsigned max_ncorners )
{
   if ( cmap == NULL )
   {
      /* dynamically allocate the feature map structure */
      cmap = gan_malloc_object(Gan_CornerFeatureMap);
      if ( cmap == NULL )
      {
         gan_err_flush_trace();
         gan_err_register_with_number ( "gan_corner_feature_map_form", GAN_ERROR_MALLOC_FAILED, "", sizeof(*cmap) );
         return NULL;
      }

      /* mark the structure as dynamically allocated */
      cmap->alloc = GAN_TRUE;
   }
   else
      /* this structure is not dynamically allocated */
      cmap->alloc = GAN_FALSE;

   /* allocate array of corner features */
   if ( max_ncorners > 0 )
   {
      cmap->corner = gan_malloc_array ( Gan_CornerFeature, max_ncorners );
      if ( cmap->corner == NULL )
      {
         gan_err_flush_trace();
         gan_err_register_with_number ( "gan_corner_feature_map_form", GAN_ERROR_MALLOC_FAILED, "", max_ncorners*sizeof(Gan_CornerFeature) );
         return NULL;
      }
   }
   else
      cmap->corner = NULL;

   /* set allocated number of corners */
   cmap->max_ncorners = max_ncorners;

   /* affine coordinate transformation not set initially */
   cmap->A_set = GAN_FALSE;

   /* create empty local feature map for this feature map */
   if ( !gan_local_feature_map_form ( &cmap->local_fmap, 0, 0, NULL ) )
   {
      gan_err_register ( "gan_corner_feature_map_form", GAN_ERROR_FAILURE, "");
      return NULL;
   }

   /* clear corner feature map */
   if ( !gan_corner_feature_map_clear ( cmap, 0, 0, NULL, NULL, NULL ) )
   {
      gan_err_register ( "gan_corner_feature_map_form", GAN_ERROR_FAILURE, "");
      return NULL;
   }

   /* return with success */
   return cmap;
}

/**
 * \brief Frees a corner feature map structure.
 * \param cmap A corner feature map
 *
 * Frees a previously formed/allocated structure that holds corner features.
 *
 * \return No value.
 * \sa gan_corner_feature_map_form(), gan_corner_feature_map_alloc().
 */
void
 gan_corner_feature_map_free ( Gan_CornerFeatureMap *cmap )
{
   /* free arrays of corner features */
   if ( cmap->corner  != NULL ) free ( cmap->corner );

   /* free local feature array */
   gan_local_feature_map_free ( &cmap->local_fmap );

   /* free structure if it was dynamically allocated */
   if ( cmap->alloc ) free ( cmap );
}

/**
 * \brief Sets up and clears a corner feature map structure.
 * \param cmap A corner feature map
 * \param height The height of the feature map region
 * \param width The width of the feature map region
 * \param A Affine transformation from window to corner coords or NULL
 * \param camera A pointer to the camera calibration or NULL
 * \param lpms Local feature map parameters or NULL
 *
 * Sets attributes of a corner feature map prior to filling
 * the feature map with corners. The \a lpms argument, if not \c NULL, is
 * passed to gan_local_feature_map_form() to build
 * a local blocked feature map for the corner features.
 *
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 */
Gan_Bool
 gan_corner_feature_map_clear ( Gan_CornerFeatureMap *cmap,
                                unsigned height, unsigned width,
                                Gan_Matrix23_f *A, Gan_Camera *camera,
                                Gan_LocalFeatureMapParams *lpms )
{
   cmap->height = height;
   cmap->width  = width;

   if ( A == NULL )
      cmap->A_set = GAN_FALSE;
   else
   {
      /* copy matrix into version stored in structure */
      cmap->A = *A;

      /* compute inverse transformation */
      cmap->Ai = gan_mat23hf_invert_s ( &cmap->A, 1.0F, NULL );
      cmap->A_set = GAN_TRUE;
   }

   if ( camera == NULL )
      /* default ideal linear camera calibration */
      gan_cameraf_build_linear ( &cmap->camera, 1.0F, 1.0F, 1.0F, 0.0F, 0.0F );
   else
      /* copy camera parameters if they were specified */
      cmap->camera = gan_cameraf_from_camera_s(camera, NULL, NULL);

   /* clear out any existing corner features */
   cmap->ncorners = 0;

   /* clear local feature map for this feature map */
   if ( !gan_local_feature_map_clear (&cmap->local_fmap, height, width, lpms) )
   {
      gan_err_register ("gan_corner_feature_map_clear", GAN_ERROR_FAILURE, "");
      return GAN_FALSE;
   }

   return GAN_TRUE;
}

/* number of corners to allocate for at a time */
#define FEATURE_ARRAY_STEP_SIZE 500

/**
 * \brief Adds a corner to a corner feature map
 * \param cmap A corner feature map
 * \param r The row location of the corner in the feature map window
 * \param c The column location of the corner in the feature map window
 * \param rf The floating point row location of the corner
 * \param cf The floating point column location of the corner
 * \param N Covariance matrix of the corner position or \c NULL
 * \param status The status of the corner feature (user-defined)
 * \param index The index of the corner feature (user-defined)
 * \param strength The strength of the corner feature
 *
 * Adds a corner to a corner feature map with given coordinates,
 * both in integer and floating corner formats, which should both
 * be in the coordinate frame of the feature window, with (0,0)
 * at the top-left corner. This routine transforms the floating
 * corner coordinates appropriately given the affine
 * transformation defined by the feature map structure cmap,
 * removes any non-linear distortion specified by the camera part
 * of the feature map structure, and writes all the information
 * into a new feature point structure, a pointer to which is
 * returned.
 *
 * The \a strength should be in units linear with the image
 * intensity, if that is applicable.
 *
 * If the covariance \a N is passed as \c NULL, a default unit
 * covariance for the corner feature is assumed.
 *
 * \return Non-\c NULL pointer to the new feature structure, or \c NULL on
 *   failure.
 */
Gan_CornerFeature *
 gan_corner_feature_add ( Gan_CornerFeatureMap *cmap,
                          unsigned r,  unsigned c,
                          float rf, float cf, Gan_SquMatrix22_f *N,
                          int status, int index, float strength )
{
   Gan_CornerFeature *corner;
   Gan_Vector3_f p;

   if ( cmap->ncorners >= cmap->max_ncorners )
   {
      /* reallocate array of corners */
      cmap->max_ncorners = cmap->ncorners + FEATURE_ARRAY_STEP_SIZE;
      cmap->corner = gan_realloc_array ( Gan_CornerFeature, cmap->corner, cmap->max_ncorners );
      if ( cmap->corner == NULL )
      {
         gan_err_flush_trace();
         gan_err_register_with_number ( "gan_corner_feature_add", GAN_ERROR_MALLOC_FAILED, "", cmap->max_ncorners*sizeof(Gan_CornerFeature) );
         return NULL;
      }
   }

   /* get pointer to new corner feature structure and increment the number
      of stored corner features */
   corner = &cmap->corner[cmap->ncorners++];

   /* copy integer corner position */
   gan_err_test_ptr ( r < cmap->height && c < cmap->width,
                      "gan_corner_feature_add", GAN_ERROR_INCOMPATIBLE,
                      "corner feature coordinates outside window" );
   corner->r = (unsigned short)r;
   corner->c = (unsigned short)c;

   if ( cmap->A_set )
   {
      /* apply affine transformation to floating point corner coordinates */
      (void)gan_vec3f_fill_q ( &p, cf, rf, 1.0F );
      (void)gan_mat23f_multv3_q ( &cmap->A, &p, &corner->p );
   }
   else
      /* just copy corner coordinates */
      (void)gan_vec2f_fill_q ( &corner->p, cf, rf );

   if ( cmap->camera.type == GAN_LINEAR_CAMERA )
      /* distortion correction has no effect */
      corner->pu = corner->p;
   else
   {
      /* remove non-linear distortion */
      (void)gan_vec3f_set_parts_q ( &p, &corner->p, cmap->camera.zh );
      if ( !gan_cameraf_remove_distortion_i ( &cmap->camera, &p ) )
      {
         gan_err_register ( "gan_corner_feature_add", GAN_ERROR_FAILURE, "" );
         return NULL;
      }

      /* copy floating corner coordinates with distortion correction applied */
      (void)gan_vec3f_get_v2t_q ( &p, &corner->pu );
   }

   /* copy feature fields */
   corner->strength = strength;
   corner->status = status;
   corner->index = index;

   if ( N == NULL )
      /* default unit covariance */
      (void)gan_symmat22f_fill_q ( &corner->N, 1.0F, 0.0F, 1.0F );
   else
   {
      if ( cmap->A_set )
      {
         /* transform covariance using affine transformation */
         Gan_Matrix22_f Al = gan_mat23f_get_m22l_s ( &cmap->A ), NAl;

         (void)gan_symmat22f_lrmultm22_q ( N, &Al, &NAl, &corner->N );
      }
      else
         /* just copy covariance */
         corner->N = *N;
   }

   /* compute inverse covariance */
   gan_symmat22f_invert_q ( &corner->N, &corner->Ni );

   /* count the entries for this feature in the local feature map */
   if ( cmap->local_fmap.ystep != 0 )
      if ( !gan_local_feature_point_add ( &cmap->local_fmap, r, c, UINT_MAX ))
      {
         gan_err_register ( "gan_corner_feature_add", GAN_ERROR_FAILURE, "" );
         return NULL;
      }

   /* success */
   return corner;
}

/**
 * \brief Postprocess feature map.
 * \param cmap A corner feature map
 *
 * Postprocess feature map, building index array into local
 * feature map. Call this function after building local feature
 * counts with gan_corner_feature_add() using a \c UINT_MAX index
 * argument, so that the index array for the local features will be built.
 *
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 * \sa gan_corner_feature_add().
 */
Gan_Bool
 gan_corner_feature_map_postprocess ( Gan_CornerFeatureMap *cmap )
{
   Gan_LocalFeatureMap *local_fmap = &cmap->local_fmap;
   unsigned i;
   Gan_CornerFeature *corner;

   /* return immediately if the local feature map is not being used */
   if ( local_fmap->ystep == 0 ) return GAN_TRUE;

   /* set index pointers */
   gan_local_feature_map_set_indices ( local_fmap );

   /* set feature counters to zero */
   gan_image_fill_const_gl_i ( &local_fmap->nfeatures, 0 );

   /* add indices for each feature */
   for ( i = 0, corner = cmap->corner; i < cmap->ncorners; i++, corner++ )
      if ( !gan_local_feature_point_add ( local_fmap, corner->r, corner->c, i ) )
      {
         gan_err_register ( "gan_corner_feature_map_postprocess",
                            GAN_ERROR_FAILURE, "" );
         return GAN_FALSE;
      }

   /* success */
   return GAN_TRUE;
}

/**
 * \}
 */

/**
 * \}
 */

/**
 * File:          $RCSfile: bright_feature.c,v $
 * Module:        Image edge point definition and functions
 * Part of:       Gandalf Library
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
#include "bright_feature.h"

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

/// Forms an bright feature map structure.


Gan_brightFeatureMap * 
 gan_bright_feature_map_form ( Gan_brightFeatureMap *bmap,unsigned max_nbright  )
{
   if ( bmap == NULL )
   {
      /* dynamically allocate the feature map structure */
      bmap = gan_malloc_object(Gan_brightFeatureMap);
      if ( bmap == NULL )
      {
         gan_err_flush_trace();
         gan_err_register_with_number ( "gan_bright_feature_map_form", GAN_ERROR_MALLOC_FAILED, "", sizeof(*bmap) );
         return NULL;
      }

      /* mark the structure as dynamically allocated */
      bmap->alloc = GAN_TRUE;
   }
   else
      /* this structure is not dynamically allocated */
      bmap->alloc = GAN_FALSE;


   /* allocate array of edge features */
   if ( max_nbright > 0 )
   {
      bmap->bright = gan_malloc_array ( Gan_brightFeature, max_nbright );
      if (bmap->bright == NULL )
      {
         gan_err_flush_trace();
         gan_err_register_with_number ( "gan_bright_feature_map_form", GAN_ERROR_MALLOC_FAILED, "", max_nbright*sizeof(Gan_brightFeature) );
         return NULL;
      }
   }
   else
      bmap->bright = NULL;

   /* set allocated number of bright */
   bmap->max_nbright = max_nbright;

   /* affine coordinate transformation not set initially */
   bmap->A_set = GAN_FALSE;

   /* create empty local feature map for this feature map */
   if ( !gan_local_feature_map_form ( &bmap->local_fmap, 0, 0, NULL ) )
   {
      gan_err_register ( "gan_bright_feature_map_form", GAN_ERROR_FAILURE, "");
      return NULL;
   }

   /* clear bright feature map */
   if ( !gan_bright_feature_map_clear ( bmap, 0, 0, NULL, NULL, NULL ) )
   {
      gan_err_register ( "gan_bright_feature_map_form", GAN_ERROR_FAILURE, "");
      return NULL;
   }

   /* return with success */
   return bmap;
}

/// Frees an bright feature map structure.
/**
 * \param emap an edge feature map
 *
 * Frees a previously formed/allocated structure that holds edge features.
 *
 * \return No value.
 * \sa gan_edge_feature_map_form(), gan_edge_feature_map_alloc().
 */
void
 gan_bright_feature_map_free ( Gan_brightFeatureMap * bmap )
{
   /* free arrays of edge features and strings */
   if ( bmap->bright  != NULL ) free ( bmap->bright );


   /* free local feature array */
   gan_local_feature_map_free ( &bmap->local_fmap );

   /* free structure if it was dynamically allocated */
   if ( bmap->alloc ) free ( bmap );
}

/// Sets up and clears an bright feature map structure.
/**
 * \param emap An edge feature map
 * \param height The height of the feature map region
 * \param width The width of the feature map region
 * \param A Affine transformation from window to edge coords or NULL
 * \param camera A pointer to the camera calibration or NULL
 * \param lpms Local feature map parameters or NULL
 *
 * Sets attributes of an edge feature map prior to filling
 * the feature map with edges. The \a lpms argument, if not \c NULL, is
 * passed to gan_local_feature_map_form() to build a local blocked
 * feature map for the edge features.
 *
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 */
Gan_Bool
 gan_bright_feature_map_clear ( Gan_brightFeatureMap *bmap,
                              unsigned height, unsigned width,
                              Gan_Matrix23_f *A, Gan_Camera *camera,
                              Gan_LocalFeatureMapParams *lpms )
{
   bmap->height = height;
   bmap->width  = width;

   if ( A == NULL )
      bmap->A_set = GAN_FALSE;
   else
   {
      /* copy matrix into version stored in structure */
      bmap->A = *A;

      /* compute inverse transformation */
      bmap->Ai = gan_mat23hf_invert_s ( &bmap->A, 1.0, NULL );
      bmap->A_set = GAN_TRUE;
   }

   if ( camera == NULL )
      /* default ideal linear camera calibration */
      gan_cameraf_build_linear ( &bmap->camera, 1.0F, 1.0F, 1.0F, 0.0F, 0.0F );
   else
      /* copy camera parameters if they were specified */
      bmap->camera = gan_cameraf_from_camera_s(camera, NULL, NULL);

   /* clear out any existing edge features and strings */
   bmap->nbright =  0;

   /* clear local feature map for this feature map */
   if ( !gan_local_feature_map_clear (&bmap->local_fmap, height, width, lpms) )
   {
      gan_err_register ( "gan_bright_feature_map_clear", GAN_ERROR_FAILURE, "");
      return GAN_FALSE;
   }

   return GAN_TRUE;
}

/* number of edges to allocate for at a time */
#define FEATURE_ARRAY_STEP_SIZE 500

/// Adds an edge to an edge feature map.
/**
 * \param emap An edge feature map
 * \param r The row location of the edge in the feature map window
 * \param c The column location of the edge in the feature map window
 * \param rf The floating edge row location of the edge
 * \param cf The floating edge column location of the edge
 * \param angle The orientation of the edge "normal"
 * \param cov The covariance of the edge position in the normal direction
 * \param status The status of the edge feature (user-defined)
 * \param index The index of the edge feature (user-defined)
 * \param strength The strength of the edge feature
 *
 * Adds an edge to an edge feature map with given coordinates,
 * both in integer and floating edge formats, which should both
 * be in the coordinate frame of the feature window, with (0,0)
 * at the top-left corner. This routine transforms the floating
 * edge coordinates appropriately given the affine transformation
 * defined by the feature map structure emap, removes any
 * non-linear distortion specified by the camera part of the
 * feature map structure, and writes all the information into
 * a new feature edge structure, a pointer to which is
 * returned.
 *
 * The \a strength should be in units linear with the image
 * intensity, if that is applicable.
 *
 * \return Non-\c NULL pointer to the new feature structure, or \c NULL
 *         on failure.
 */
Gan_brightFeature *
 gan_bright_feature_add ( Gan_brightFeatureMap *bmap,
                        unsigned r,  unsigned c,
                          float rf, float cf,int status)
{
   Gan_brightFeature *bright;
   Gan_Vector3_f p3;

   if ( bmap->nbright >= bmap->max_nbright )
   {
      /* reallocate array of bright */
      bmap->max_nbright = bmap->nbright + FEATURE_ARRAY_STEP_SIZE;
      bmap->bright = gan_realloc_array ( Gan_brightFeature, bmap->bright, bmap->max_nbright );
      if ( bmap->nbright ==0)
      {
         gan_err_flush_trace();
         gan_err_register_with_number ( "gan_bright_feature_add", GAN_ERROR_MALLOC_FAILED, "", bmap->max_nbright*sizeof(Gan_brightFeature) );
         return NULL;
         }
   }

   /* get pointer to new edge feature structure and increment the number
      of stored bright features */
   bright = &bmap->bright[bmap->nbright++];

   /* copy integer bright position */
   gan_err_test_ptr ( r < bmap->height && c < bmap->width,
                      "gan_bright_feature_add", GAN_ERROR_INCOMPATIBLE,
                      "bright feature coordinates outside window" );
   bright->r = (unsigned short)r;
   bright->c = (unsigned short)c;

   if (bmap->A_set )
   {
      /* apply affine transformation to floating bright coordinates */
      (void)gan_vec3f_fill_q ( &p3, cf, rf, 1.0 );
      (void)gan_mat23f_multv3_q ( &bmap->A, &p3, &bright->p );
   }
   else
   {
      /* just copy bright coordinates */
     bright->p.x = cf;
     bright->p.y = rf;
   }

   if (bmap->camera.type == GAN_LINEAR_CAMERA )
      /* distortion correction has no effect */
     bright->pu =bright->p;
   else
   {
      /* remove non-linear distortion */
      (void)gan_vec3f_set_parts_q ( &p3, &bright->p, bmap->camera.zh );
      if ( !gan_cameraf_remove_distortion_i ( &bmap->camera, &p3 ) )
      {
         gan_err_register ( "gan_corner_feature_add", GAN_ERROR_FAILURE, "" );
         return NULL;
      }

      /* copy floating bright coordinates with distortion correction applied */
      (void)gan_vec3f_get_v2t_q ( &p3, &bright->pu );
   }
 
   /* count the entries for this feature in the local feature map */
   if ( bmap->local_fmap.ystep != 0 )
      if ( !gan_local_feature_point_add ( &bmap->local_fmap, r, c, UINT_MAX ) )
      {
         gan_err_register ( "gan_bright_feature_add", GAN_ERROR_FAILURE, "" );
         return NULL;
      }

   /* initialise previous and next pointers to NULL */
  bright->next = bright->prev = NULL;

   /* success */
   return bright;
}




/// Postprocess feature map.
/**
 * \param emap a bright feature map
 *
 * Postprocess feature map, building index array into local
 * feature map. Call this function after building local feature
 * counts with gan_bright_feature_add() using a \c UINT_MAX index
 * argument, so that the index array for the local features will be built.
 *
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 * \sa gan_bright_feature_add().
 */
Gan_Bool
 gan_bright_feature_map_postprocess ( Gan_brightFeatureMap *bmap )
{
   Gan_LocalFeatureMap *local_fmap = &bmap->local_fmap;
   unsigned i;
   Gan_brightFeature *bright;

   /* return immediately if the local feature map is not being used */
   if ( local_fmap->ystep == 0 ) return GAN_TRUE;

   /* set index pointers */
   gan_local_feature_map_set_indices ( local_fmap );

   /* set feature counters to zero */
   gan_image_fill_const_gl_i ( &local_fmap->nfeatures, 0 );

   /* add indices for each feature */
   for ( i = 0, bright = bmap->bright; i < bmap->nbright; i++, bright++ )
      if ( !gan_local_feature_point_add ( local_fmap, bright->r, bright->c, i ) )
      {
         gan_err_register ( "gan_bright_feature_map_postprocess",
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

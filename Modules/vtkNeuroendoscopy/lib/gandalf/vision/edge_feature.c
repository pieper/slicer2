/**
 * File:          $RCSfile: edge_feature.c,v $
 * Module:        Image edge point definition and functions
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
#include <gandalf/vision/edge_feature.h>
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

/// Forms an edge feature map structure.
/**
 * \param emap an edge feature map
 * \param max_nedges The number of edges initially to allocate for
 * \param max_nstrings The number of strings initially to allocate for
 *
 * Fills a structure to hold edge features, and sets it to be empty.
 *
 * \return non-\c NULL the formed feature map \a emap, or \c NULL on failure.
 * \sa gan_edge_feature_map_alloc(), gan_edge_feature_map_free().
 */
Gan_EdgeFeatureMap *
 gan_edge_feature_map_form ( Gan_EdgeFeatureMap *emap,
                             unsigned max_nedges,
                             unsigned max_nstrings )
{
   if ( emap == NULL )
   {
      /* dynamically allocate the feature map structure */
      emap = gan_malloc_object(Gan_EdgeFeatureMap);
      if ( emap == NULL )
      {
         gan_err_flush_trace();
         gan_err_register_with_number ( "gan_edge_feature_map_form", GAN_ERROR_MALLOC_FAILED, "", sizeof(*emap) );
         return NULL;
      }

      /* mark the structure as dynamically allocated */
      emap->alloc = GAN_TRUE;
   }
   else
      /* this structure is not dynamically allocated */
      emap->alloc = GAN_FALSE;


   /* allocate array of edge features */
   if ( max_nedges > 0 )
   {
      emap->edge = gan_malloc_array ( Gan_EdgeFeature, max_nedges );
      if ( emap->edge == NULL )
      {
         gan_err_flush_trace();
         gan_err_register_with_number ( "gan_edge_feature_map_form", GAN_ERROR_MALLOC_FAILED, "", max_nedges*sizeof(Gan_EdgeFeature) );
         return NULL;
      }
   }
   else
      emap->edge = NULL;

   /* set allocated number of edges */
   emap->max_nedges = max_nedges;

   /* allocate array of edge strings */
   if ( max_nstrings > 0 )
   {
      emap->string = gan_malloc_array ( Gan_EdgeString, max_nstrings );
      if ( emap->string == NULL )
      {
         gan_err_flush_trace();
         gan_err_register_with_number ( "gan_edge_feature_map_form", GAN_ERROR_MALLOC_FAILED, "", max_nstrings*sizeof(Gan_EdgeString) );
         return NULL;
      }
   }
   else
      emap->string = NULL;

   /* set allocated number of edges */
   emap->max_nstrings = max_nstrings;

   /* affine coordinate transformation not set initially */
   emap->A_set = GAN_FALSE;

   /* create empty local feature map for this feature map */
   if ( !gan_local_feature_map_form ( &emap->local_fmap, 0, 0, NULL ) )
   {
      gan_err_register ( "gan_edge_feature_map_form", GAN_ERROR_FAILURE, "");
      return NULL;
   }

   /* clear edge feature map */
   if ( !gan_edge_feature_map_clear ( emap, 0, 0, NULL, NULL, NULL ) )
   {
      gan_err_register ( "gan_edge_feature_map_form", GAN_ERROR_FAILURE, "");
      return NULL;
   }

   /* return with success */
   return emap;
}

/// Frees an edge feature map structure.
/**
 * \param emap an edge feature map
 *
 * Frees a previously formed/allocated structure that holds edge features.
 *
 * \return No value.
 * \sa gan_edge_feature_map_form(), gan_edge_feature_map_alloc().
 */
void
 gan_edge_feature_map_free ( Gan_EdgeFeatureMap *emap )
{
   /* free arrays of edge features and strings */
   if ( emap->edge  != NULL ) free ( emap->edge );
   if ( emap->string != NULL ) free ( emap->string );

   /* free local feature array */
   gan_local_feature_map_free ( &emap->local_fmap );

   /* free structure if it was dynamically allocated */
   if ( emap->alloc ) free ( emap );
}

/// Sets up and clears an edge feature map structure.
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
 gan_edge_feature_map_clear ( Gan_EdgeFeatureMap *emap,
                              unsigned height, unsigned width,
                              Gan_Matrix23_f *A, Gan_Camera *camera,
                              Gan_LocalFeatureMapParams *lpms )
{
   emap->height = height;
   emap->width  = width;

   if ( A == NULL )
      emap->A_set = GAN_FALSE;
   else
   {
      /* copy matrix into version stored in structure */
      emap->A = *A;

      /* compute inverse transformation */
      emap->Ai = gan_mat23hf_invert_s ( &emap->A, 1.0, NULL );
      emap->A_set = GAN_TRUE;
   }

   if ( camera == NULL )
      /* default ideal linear camera calibration */
      gan_cameraf_build_linear ( &emap->camera, 1.0F, 1.0F, 1.0F, 0.0F, 0.0F );
   else
      /* copy camera parameters if they were specified */
      emap->camera = gan_cameraf_from_camera_s(camera, NULL, NULL);

   /* clear out any existing edge features and strings */
   emap->nedges = emap->nstrings = 0;

   /* clear local feature map for this feature map */
   if ( !gan_local_feature_map_clear (&emap->local_fmap, height, width, lpms) )
   {
      gan_err_register ( "gan_edge_feature_map_clear", GAN_ERROR_FAILURE, "");
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
Gan_EdgeFeature *
 gan_edge_feature_add ( Gan_EdgeFeatureMap *emap,
                        unsigned r,  unsigned c,
                        float rf, float cf,
                        float angle, float cov,
                        int status, int index, float strength )
{
   Gan_EdgeFeature *edge;
   Gan_Vector3_f p3;

   if ( emap->nedges >= emap->max_nedges )
   {
      /* reallocate array of edges */
      emap->max_nedges = emap->nedges + FEATURE_ARRAY_STEP_SIZE;
      emap->edge = gan_realloc_array ( Gan_EdgeFeature, emap->edge, emap->max_nedges );
      if ( emap->edge == NULL )
      {
         gan_err_flush_trace();
         gan_err_register_with_number ( "gan_edge_feature_add", GAN_ERROR_MALLOC_FAILED, "", emap->max_nedges*sizeof(Gan_EdgeFeature) );
         return NULL;
      }
   }

   /* get pointer to new edge feature structure and increment the number
      of stored edge features */
   edge = &emap->edge[emap->nedges++];

   /* copy integer edge position */
   gan_err_test_ptr ( r < emap->height && c < emap->width,
                      "gan_edge_feature_add", GAN_ERROR_INCOMPATIBLE,
                      "edge feature coordinates outside window" );
   edge->r = (unsigned short)r;
   edge->c = (unsigned short)c;

   if ( emap->A_set )
   {
      /* apply affine transformation to floating edge coordinates */
      (void)gan_vec3f_fill_q ( &p3, cf, rf, 1.0 );
      (void)gan_mat23f_multv3_q ( &emap->A, &p3, &edge->p );
   }
   else
   {
      /* just copy edge coordinates */
      edge->p.x = cf;
      edge->p.y = rf;
   }

   if ( emap->camera.type == GAN_LINEAR_CAMERA )
      /* distortion correction has no effect */
      edge->pu = edge->p;
   else
   {
      /* remove non-linear distortion */
      (void)gan_vec3f_set_parts_q ( &p3, &edge->p, emap->camera.zh );
      if ( !gan_cameraf_remove_distortion_i ( &emap->camera, &p3 ) )
      {
         gan_err_register ( "gan_corner_feature_add", GAN_ERROR_FAILURE, "" );
         return NULL;
      }

      /* copy floating edge coordinates with distortion correction applied */
      (void)gan_vec3f_get_v2t_q ( &p3, &edge->pu );
   }

   /* copy feature fields */
   edge->angle = (float)angle;
   edge->strength = strength;
   edge->status = status;
   edge->index = index;

   /* fill covariance entry */
   if ( emap->A_set )
      /* scale covariance according to affine scaling */
      edge->cov = (float)(emap->A.xx*emap->A.xx*cov);
   else
      /* just copy covariance */
      edge->cov = (float)cov;

   /* count the entries for this feature in the local feature map */
   if ( emap->local_fmap.ystep != 0 )
      if ( !gan_local_feature_point_add ( &emap->local_fmap, r, c, UINT_MAX ) )
      {
         gan_err_register ( "gan_edge_feature_add", GAN_ERROR_FAILURE, "" );
         return NULL;
      }

   /* initialise previous and next pointers to NULL */
   edge->next = edge->prev = NULL;

   /* success */
   return edge;
}

/// Adds an edge string to an edge feature map.
/**
 * \param emap an edge feature map
 * \param edge The first edge in the edge string
 *
 * Adds a new edge string to an edge feature map, starting with the given edge.
 *
 * \return Non-\c NULL pointer to the new feature string structure, or
 *         \c NULL on failure.
 *
 */
Gan_EdgeString *
 gan_edge_feature_string_add ( Gan_EdgeFeatureMap *emap,
                               Gan_EdgeFeature *edge )
{
   Gan_EdgeString *string;

   edge->prev = edge->next = NULL;

   /* check if we need to allocate more strings */
   if ( emap->nstrings >= emap->max_nstrings )
   {
      /* reallocate array of edge strings */
      emap->max_nstrings = emap->nstrings + FEATURE_ARRAY_STEP_SIZE;
      emap->string = gan_realloc_array ( Gan_EdgeString, emap->string,
                                         emap->max_nstrings );
      if ( emap->string == NULL )
      {
         gan_err_flush_trace();
         gan_err_register_with_number ( "gan_edge_feature_string_add", GAN_ERROR_MALLOC_FAILED, "", emap->max_nstrings*sizeof(Gan_EdgeString) );
         return NULL;
      }
   }

   string = &emap->string[emap->nstrings++];
   string->first = string->last = edge;
   string->length = 1;
   return string;
}

/// Sets the status of all edges in the string.
/**
 * \param string an edge feature string
 * \param status A status value
 *
 * Sets the status of all edges in the string to the given status value.
 *
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 */
Gan_Bool
 gan_edge_feature_string_set_status ( Gan_EdgeString *string, int status )
{
   Gan_EdgeFeature *edge;
   int i;

   for ( i = string->length-1, edge = string->first; i >= 0;
         i--, edge = edge->next )
      edge->status = status;

   return GAN_TRUE;
}

/// Postprocess feature map.
/**
 * \param emap an edge feature map
 *
 * Postprocess feature map, building index array into local
 * feature map. Call this function after building local feature
 * counts with gan_edge_feature_add() using a \c UINT_MAX index
 * argument, so that the index array for the local features will be built.
 *
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 * \sa gan_edge_feature_add().
 */
Gan_Bool
 gan_edge_feature_map_postprocess ( Gan_EdgeFeatureMap *emap )
{
   Gan_LocalFeatureMap *local_fmap = &emap->local_fmap;
   unsigned i;
   Gan_EdgeFeature *edge;

   /* return immediately if the local feature map is not being used */
   if ( local_fmap->ystep == 0 ) return GAN_TRUE;

   /* set index pointers */
   gan_local_feature_map_set_indices ( local_fmap );

   /* set feature counters to zero */
   gan_image_fill_const_gl_i ( &local_fmap->nfeatures, 0 );

   /* add indices for each feature */
   for ( i = 0, edge = emap->edge; i < emap->nedges; i++, edge++ )
      if ( !gan_local_feature_point_add ( local_fmap, edge->r, edge->c, i ) )
      {
         gan_err_register ( "gan_edge_feature_map_postprocess",
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

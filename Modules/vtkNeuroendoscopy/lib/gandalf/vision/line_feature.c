/**
 * File:          $RCSfile: line_feature.c,v $
 * Module:        Image line segment definition and functions
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

#include <math.h>
#include <string.h>
#include <gandalf/vision/line_feature.h>
#include <gandalf/common/misc_error.h>
#include <gandalf/common/allocate.h>
#include <gandalf/common/compare.h>
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
 * \brief Forms a line feature map structure.
 * \param lmap A line feature map
 * \param max_nlines The number of lines initially to allocate for
 *
 * Fills a structure to hold line features, and sets it to be empty.
 *
 * \return non-\c NULL The formed feature map \a lmap, or \c NULL on failure.
 * \sa gan_line_feature_map_alloc(), gan_line_feature_map_free().
 */
Gan_LineFeatureMap *
 gan_line_feature_map_form ( Gan_LineFeatureMap *lmap,
                             unsigned max_nlines )
{
   if ( lmap == NULL )
   {
      /* dynamically allocate the feature map structure */
      lmap = gan_malloc_object(Gan_LineFeatureMap);
      if ( lmap == NULL )
      {
         gan_err_flush_trace();
         gan_err_register_with_number ( "gan_line_feature_map_form", GAN_ERROR_MALLOC_FAILED, "", sizeof(*lmap) );
         return NULL;
      }

      /* mark the structure as dynamically allocated */
      lmap->alloc = GAN_TRUE;
   }
   else
      /* this structure is not dynamically allocated */
      lmap->alloc = GAN_FALSE;


   /* allocate array of line features */
   if ( max_nlines > 0 )
   {
      lmap->line = gan_malloc_array ( Gan_LineFeature, max_nlines );
      if ( lmap->line == NULL )
      {
         gan_err_flush_trace();
         gan_err_register_with_number ( "gan_line_feature_map_form", GAN_ERROR_MALLOC_FAILED, "", max_nlines*sizeof(Gan_LineFeature) );
         return NULL;
      }
   }
   else
      lmap->line = NULL;

   /* set allocated number of lines */
   lmap->max_nlines = max_nlines;

   /* clear point array */
   lmap->point = NULL;
   lmap->npoints = lmap->max_npoints = 0;

   /* affine coordinate transformation not set initially */
   lmap->A_set = GAN_FALSE;

   /* create empty local feature map for this feature map */
   if ( !gan_local_feature_map_form ( &lmap->local_fmap, 0, 0, NULL ) )
   {
      gan_err_register ( "gan_line_feature_map_form", GAN_ERROR_FAILURE, "");
      return NULL;
   }

   /* clear line feature map */
   if ( !gan_line_feature_map_clear ( lmap, 0, 0, NULL, NULL ) )
   {
      gan_err_register ( "gan_line_feature_map_form", GAN_ERROR_FAILURE, "");
      return NULL;
   }

   /* return with success */
   return lmap;
}

/**
 * \brief Copies a line feature map structure.
 * \param lmap Input line feature map
 * \result Copy of input structure or \c NULL on error
 *
 * Copies a structure that holds line features.
 * \sa gan_line_feature_map_form(), gan_line_feature_map_alloc().
 */
Gan_LineFeatureMap *
 gan_line_feature_map_copy ( Gan_LineFeatureMap *lmap )
{
   Gan_LineFeatureMap* lmapnew;

   lmapnew = gan_line_feature_map_alloc(lmap->nlines);
   if(lmapnew == NULL)
   {
      gan_err_register ( "gan_line_feature_map_copy", GAN_ERROR_FAILURE, "");
      return NULL;
   }

   lmapnew->nlines = lmap->nlines;
   memcpy(lmapnew->line, lmap->line, lmap->nlines*sizeof(Gan_LineFeature));
   lmapnew->height = lmap->height;
   lmapnew->width  = lmap->width;
   lmapnew->A_set = lmap->A_set;
   lmapnew->A = lmap->A;
   lmapnew->Ai = lmap->Ai;
   lmapnew->local_fmap = lmap->local_fmap;

   if(lmap->npoints > 0 && lmap->point != NULL)
   {
      lmapnew->point = gan_malloc_array(Gan_Vector2_f, lmap->npoints);
      if(lmapnew->point == NULL)
      {
         gan_err_flush_trace();
         gan_err_register ( "gan_line_feature_map_copy", GAN_ERROR_MALLOC_FAILED, "");
         return NULL;
      }

      memcpy(lmapnew->point, lmap->point, lmap->npoints*sizeof(Gan_Vector2_f));
      lmapnew->npoints = lmapnew->max_npoints = lmap->npoints;
   }

   return lmapnew;
}

/**
 * \brief Frees a line feature map structure.
 * \param lmap A line feature map
 *
 * Frees a previously formed/allocated structure that holds line features.
 *
 * \return No value.
 * \sa gan_line_feature_map_form(), gan_line_feature_map_alloc().
 */
void
 gan_line_feature_map_free ( Gan_LineFeatureMap *lmap )
{
   /* free array of points */
   if ( lmap->point != NULL ) free ( lmap->point );

   /* free array of line features */
   if ( lmap->line != NULL ) free ( lmap->line );

   /* free local feature array */
   gan_local_feature_map_free ( &lmap->local_fmap );

   /* free structure if it was dynamically allocated */
   if ( lmap->alloc ) free ( lmap );
}

/**
 * \brief Sets up and clears a line feature map structure.
 * \param lmap A line feature map
 * \param height The height of the feature map region
 * \param width The width of the feature map region
 * \param A Affine transformation from window to line coords or NULL
 * \param lpms Local feature map parameters or NULL
 *
 * Sets attributes of a line feature map prior to filling the feature map
 * with lines. The \a lpms argument, if not \c NULL, is passed to
 * gan_local_feature_map_form() to build a local blocked feature map for
 * the line features.
 *
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 */
Gan_Bool
 gan_line_feature_map_clear ( Gan_LineFeatureMap *lmap,
                              unsigned height, unsigned width,
                              Gan_Matrix23_f *A,
                              Gan_LocalFeatureMapParams *lpms )
{
   lmap->height = height;
   lmap->width  = width;

   if ( A == NULL )
      lmap->A_set = GAN_FALSE;
   else
   {
      /* copy matrix into version stored in structure */
      lmap->A = *A;

      /* compute inverse transformation */
      lmap->Ai = gan_mat23hf_invert_s ( &lmap->A, 1.0F, NULL );
      lmap->A_set = GAN_TRUE;
   }

   /* clear out any existing line features */
   lmap->nlines = 0;

   /* clear out any existing points */
   lmap->npoints = 0;

   /* clear local feature map for this feature map */
   if ( !gan_local_feature_map_clear ( &lmap->local_fmap,
                                       height, width, lpms ) )
   {
      gan_err_register ( "gan_line_feature_map_clear", GAN_ERROR_FAILURE, "");
      return GAN_FALSE;
   }

   return GAN_TRUE;
}

/* number of lines to allocate for at a time */
#define FEATURE_ARRAY_STEP_SIZE 500

/* number of points to allocate for at a time */
#define POINT_ARRAY_REALLOC_SIZE 5000

/**
 * \brief Adds a line to a line feature map.
 * \param lmap A line feature map
 * \param y1 The row location of the first line endpoint
 * \param x1 The column location of the first line endpoint
 * \param y2 The row location of the second line endpoint
 * \param x2 The column location of the second line endpoint
 * \param N Covariance of canonical line parameters
 * \param status The status of the line feature (user-defined)
 * \param index The index of the line feature (user-defined)
 * \param strength The strength of the line feature
 * \param point Array of points or NULL
 * \param npoints Number of points in array or zero (if point is NULL)
 *
 * Adds a line to a line feature map with given coordinates,
 * both in integer and floating line formats, which should both
 * be in the coordinate frame of the feature window, with (0,0)
 * at the top-left corner. This routine transforms the floating
 * line coordinates appropriately given the affine transformation
 * defined by the feature map structure lmap, removes any
 * non-linear distortion specified by the camera part of the
 * feature map structure, and writes all the information into
 * a new feature line structure, a lineer to which is
 * returned.
 *
 * The \a strength should be in units linear with the image
 * intensity, if that is applicable.
 *
 * \a N is the covariance of the canonical line parameters a/b in
 * y=ax+b, with the x/y coordinate system centred on the midpoint
 * of the line (p1+p2)/2, with the positive x-axis along the line
 * towards the p2 endpoint, and the positive y-axis 90 degrees
 * anticlockwise from the x-axis.
 *
 * \return Non-\c NULL pointer to the new feature structure, or
 *         \c NULL on failure.
 */
Gan_LineFeature *
 gan_line_feature_add ( Gan_LineFeatureMap *lmap,
                        float y1, float x1,
                        float y2, float x2,
                        Gan_SquMatrix22_f *N,
                        int status, int index,
                        float strength,
                        Gan_Vector2_f *point,
                        unsigned npoints )
{
   Gan_LineFeature *line;
   Gan_Vector2_f p1, p2, pd;
   unsigned r1, c1, r2, c2;

   if ( lmap->nlines >= lmap->max_nlines )
   {
      /* reallocate array of lines */
      lmap->max_nlines = lmap->nlines + FEATURE_ARRAY_STEP_SIZE;
      lmap->line = gan_realloc_array ( Gan_LineFeature, lmap->line, lmap->max_nlines );
      if ( lmap->line == NULL )
      {
         gan_err_flush_trace();
         gan_err_register_with_number ( "gan_line_feature_add", GAN_ERROR_MALLOC_FAILED, "", lmap->max_nlines*sizeof(Gan_LineFeature) );
         return NULL;
      }
   }

   /* get linear to new line feature structure and increment the number
      of stored line features */
   line = &lmap->line[lmap->nlines++];

   /* copy line endpoint coordinates */
   (void)gan_vec2f_fill_q ( &line->p1, x1, y1 );
   (void)gan_vec2f_fill_q ( &line->p2, x2, y2 );

   /* compute line parameters lx,ly,lz so that lx*x + ly*y + lz = 0 scaled
      so that lx^2 + ly^2 = 1 */
   (void)gan_vec2f_sub_q ( &line->p2, &line->p1, &pd );
   gan_err_test_ptr ( pd.x != 0.0F || pd.y != 0.0F, "gan_line_feature_add",
                      GAN_ERROR_FAILURE, "zero length line" );
   (void)gan_vec3f_fill_q ( &line->l, -pd.y, pd.x,x1*y2 - x2*y1 );
   line->l = gan_vec3f_divide_s ( &line->l,
                                  (float)sqrt(gan_vec2f_sqrlen_q(&pd)) );

   if ( lmap->A_set )
   {
      /* transform coordinates into window coordinates */
      p1 = gan_mat23f_multv2h_s ( &lmap->Ai, &line->p1, 1.0F );
      p2 = gan_mat23f_multv2h_s ( &lmap->Ai, &line->p2, 1.0F );
   }
   else
   {
      /* simply copy coordinates */
      p1 = line->p1;
      p2 = line->p2;
   }

   /* check that coordinates are positive */
   gan_err_test_ptr ( p1.x >= 0.0F && p1.y >= 0.0F &&
                      p2.x >= 0.0F && p2.y >= 0.0F,
                      "gan_line_feature_add", GAN_ERROR_INCOMPATIBLE,
                      "line feature coordinates outside window" );

   /* convert to integer window coordinates */
   c1 = (unsigned) p1.x; r1 = (unsigned) p1.y;
   c2 = (unsigned) p2.x; r2 = (unsigned) p2.y;

   /* copy line position */
   gan_err_test_ptr ( r1 < lmap->height && c1 < lmap->width  &&
                      r2 < lmap->height && c2 < lmap->width,
                      "gan_line_feature_add", GAN_ERROR_INCOMPATIBLE,
                      "line feature coordinates outside window" );
   line->r1 = r1; line->c1 = c1;
   line->r2 = r2; line->c2 = c2;

   /* copy feature fields */
   line->strength = strength;
   line->status = status;
   line->index = index;

   /* fill covariance and inverse covariance entries */
   line->N = *N;
   line->Ni = gan_symmat22f_invert_s(N);

   /* count the entries for this feature in the local feature map */
   if ( lmap->local_fmap.ystep != 0 )
      if ( !gan_local_feature_line_add ( &lmap->local_fmap, r1, c1, r2, c2,
                                         UINT_MAX ) )
      {
         gan_err_register ( "gan_line_feature_add", GAN_ERROR_FAILURE, "" );
         return NULL;
      }

   if ( point == NULL )
   {
      /* set array of points to NULL */
      line->point = NULL;
      line->npoints = 0;
   }
   else
   {
      int i;
      Gan_Vector2_f *pptr;

      /* see whether we need to reallocate the point array */
      if ( lmap->npoints + npoints > lmap->max_npoints )
      {
         /* reallocate array of points */
         lmap->max_npoints = gan_max2_ui(
                                    lmap->max_npoints+POINT_ARRAY_REALLOC_SIZE,
                                    lmap->npoints + npoints );
         lmap->point = gan_realloc_array ( Gan_Vector2_f, lmap->point, lmap->max_npoints );
         if ( lmap->point == NULL )
         {
            gan_err_flush_trace();
            gan_err_register_with_number ( "gan_line_feature_add", GAN_ERROR_MALLOC_FAILED, "", lmap->max_npoints*sizeof(Gan_Vector2_f) );
            return NULL;
         }
      }

      /* add point array to array in line map */
      pptr = &lmap->point[lmap->npoints];
      for ( i = (int)npoints-1; i >= 0; i-- )
         pptr[i] = point[i];

      lmap->npoints += (line->npoints = npoints);
   }

   /* success */
   return line;
}

/**
 * \brief Postprocess feature map.
 * \param lmap A line feature map
 *
 * Postprocess feature map, building index array into local
 * feature map. Call this function after building local feature
 * counts with gan_line_feature_add() using a \c UINT_MAX index
 * argument, so that the index array for the local features will be built.
 *
 * \return #GAN_TRUE on success, or #GAN_FALSE on failure.
 * \sa gan_line_feature_add().
 */
Gan_Bool
 gan_line_feature_map_postprocess ( Gan_LineFeatureMap *lmap )
{
   Gan_LocalFeatureMap *local_fmap = &lmap->local_fmap;
   unsigned i;
   Gan_LineFeature *line;

   if ( lmap->npoints > 0 )
   {
      unsigned npoints;

      /* set pointers to point array */
      for ( i = 0, npoints = 0, line = lmap->line; i < lmap->nlines;
            i++, line++ )
      {
         line->point = &lmap->point[npoints];
         npoints += line->npoints;
      }

      gan_err_test_bool ( npoints == lmap->npoints,
                          "gan_line_feature_map_postprocess",
                          GAN_ERROR_FAILURE, "point counts don't match" );
   }
         
   /* return immediately if the local feature map is not being used */
   if ( local_fmap->ystep == 0 ) return GAN_TRUE;

   /* set index lineers */
   gan_local_feature_map_set_indices ( local_fmap );

   /* set feature counters to zero */
   gan_image_fill_const_gl_i ( &local_fmap->nfeatures, 0 );

   /* add indices for each feature */
   for ( i = 0, line = lmap->line; i < lmap->nlines; i++, line++ )
      if ( !gan_local_feature_line_add ( local_fmap, line->r1, line->c1,
                                                     line->r2, line->c2, i ) )
      {
         gan_err_register ( "gan_line_feature_map_postprocess",
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

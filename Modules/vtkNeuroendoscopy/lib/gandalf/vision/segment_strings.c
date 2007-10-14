/**
 * File:          $RCSfile: segment_strings.c,v $
 * Module:        Edge string segmentation algorithm
 * Part of:       Gandalf Library
 *
 * Revision:      $Revision: 1.1.2.1 $
 * Last edited:   $Date: 2007/10/14 02:33:20 $
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
#include <gandalf/vision/segment_strings.h>
#include <gandalf/common/numerics.h>
#include <gandalf/common/misc_error.h>
#include <gandalf/common/compare.h>
#include <gandalf/common/allocate.h>

/**
 * \addtogroup Vision
 * \{
 */

/**
 * \addtogroup FeatureDetection
 * \{
 */

#ifndef NDEBUG
static Gan_Bool
 check_edge_string ( Gan_EdgeString *string )
{
   Gan_EdgeFeature *edge;
   unsigned j;

   if ( string->first == NULL )
   {
      if ( string->last == NULL && string->length == 0 )
         return GAN_TRUE;
      else
         return GAN_FALSE;
   }

   if ( string->length < 1 ) return GAN_FALSE;

   if ( string->first == string->last )
   {
      if ( string->length == 1 ) return GAN_TRUE;

      for ( j = 1, edge = string->first->next;
            edge != string->last && edge != NULL && j < string->length;
            j++, edge = edge->next )
         ;

      if ( edge == string->last && j == string->length )
         return GAN_TRUE;
      else
         return GAN_FALSE;
   }
   else
   {
      for ( j = 0, edge = string->first; edge != NULL && j < string->length;
            j++, edge = edge->next )
         ;

      if ( edge == NULL && j == string->length )
         return GAN_TRUE;
      else
         return GAN_FALSE;
   }
}
#endif

static Gan_Bool
 segment_string ( Gan_EdgeFeatureMap *edge_map, Gan_EdgeString *string,
                  int test_length, float rms_error_thresh,
                  int string_length_thres )
{
   Gan_EdgeFeature *edge, *prev_edge=NULL;
   int      j;
   float    Sx, Sy, Sxx, Sxy, Syy, p, q, r, lambda, nf;
   float    rms_error_thresh_sqr = rms_error_thresh*rms_error_thresh;
   Gan_Vector2_f *pos, *prev_pos;
   int length_to_test;

   /* determine number of edges to test based on whether string is circular
      or not */
   if ( string->first == string->last )
      /* circular string */
      length_to_test = (int)string->length+test_length;
   else
      length_to_test = (int)string->length;

   /* initialize orthogonal regression fit */
   Sx = Sy = Sxx = Sxy = Syy = 0.0F;

   /* find first segmentation point in string */
   nf = (float) test_length;
   for ( j = 0, edge = string->first; j < length_to_test;
         j++, edge = edge->next )
   {
      /* initialize edge just before test region */
      if ( j == test_length )
         prev_edge = string->first;

      /* use edge position with distortion correction */
      pos = &edge->pu;
      if ( prev_edge != NULL )
         prev_pos = &prev_edge->pu;

      /* adjust line fitting parameters */
      Sx += pos->x; Sy += pos->y;
      Sxx += pos->x*pos->x; Sxy += pos->x*pos->y; Syy += pos->y*pos->y;
      if ( prev_edge != NULL )
      {
         Sx -= prev_pos->x; Sy -= prev_pos->y;
         Sxx -= prev_pos->x*prev_pos->x;
         Sxy -= prev_pos->x*prev_pos->y;
         Syy -= prev_pos->y*prev_pos->y;
      }
      
      /* check fit if we are far enough along the string*/
      if ( j >= test_length-1 )
      {
         /* compute sums normalized for centroid */
         p = Sxx - Sx*Sx/nf;
         q = Sxy - Sx*Sy/nf;
         r = Syy - Sy*Sy/nf;

         /* compute smallest non-zero eigenvalue of covariance
            matrix (p q 0)
                   (q r 0)
                   (0 0 0) */
         lambda = 0.5F*(p + r - (float)sqrt((p-r)*(p-r) + 4.0*q*q));

         /* segment string here if RMS error is too big */
         if ( lambda > nf*rms_error_thresh_sqr )
            break;
      }

      if ( prev_edge != NULL )
         prev_edge = prev_edge->next;
   }

   if ( j == length_to_test )
      /* no segmentation points found */
      return GAN_TRUE;

   /* set the status of the now unused edges to zero */
   {
      Gan_EdgeFeature *edgep;

      if ( prev_edge == NULL )
         edgep = string->first->next;
      else
         edgep = prev_edge->next->next;

      /* note that we keep the first and last edges in the segmentation
         window */
      for ( ; edgep != edge; edgep = edgep->next )
         edgep->status = 0;
   }

   /* if the string was circular, convert it to straight without creating
      a new string */
   if ( string->first == string->last )
   {
      /* set first edge in straight string to last tested edge */
      string->first = edge;
      if ( prev_edge == NULL )
         string->last = string->last;
      else
         string->last = prev_edge->next;

      string->first->prev = NULL;
      string->last->next = NULL;
      string->length -= (test_length-2);
   }
   else
   {
      /* normal straight string */
      int new_length;
      Gan_EdgeFeature *original_first_edge = string->first;

      /* cut off existing string from before segmentation point */
      string->first = edge;
      edge->prev = NULL;
      string->length -= j;

      /* check length of new string */
      new_length = j+2-test_length;
      if ( new_length >= string_length_thres )
      {
         /* build new string */
         Gan_EdgeString *new_string;
         
         /* remember next edge */
         edge = original_first_edge->next;

         /* create new string */
         new_string = gan_edge_feature_string_add ( edge_map,
                                                    original_first_edge );
         if ( new_string == NULL )
         {
            gan_err_register ( "segment_string", GAN_ERROR_FAILURE, "" );
            return GAN_FALSE;
         }

         /* reset next edge to recover original connection */
         original_first_edge->next = edge;

         /* set last edge in string to first edge in test window */
         if ( prev_edge == NULL )
            new_string->last = original_first_edge;
         else
            new_string->last = prev_edge->next;

         new_string->last->next = NULL;
         new_string->length = new_length;
#ifndef NDEBUG
         if ( !check_edge_string ( new_string ) )
         {
            gan_err_flush_trace();
            gan_err_register ( "segment_string", GAN_ERROR_FAILURE, "" );
            return GAN_FALSE;
         }
#endif
      }
      else
         /* throw away these edges */
         for ( j = 0, edge = original_first_edge; j < new_length;
               j++, edge = edge->next )
            edge->status = 0;
   }
      
#ifndef NDEBUG
   if ( !check_edge_string ( string ) )
   {
      gan_err_flush_trace();
      gan_err_register ( "segment_string", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }
#endif

   /* recursively segment reduced string */
   if ( string->length < string_length_thres )
   {
      /* reset status flags of edgels in deleted string */
      gan_edge_feature_string_set_status ( string, 0 );

      /* convert to empty string to be ignored */
      string->first = string->last = NULL;
      string->length = 0;
   }
   else
      if ( !segment_string ( edge_map, string, test_length,
                             rms_error_thresh, string_length_thres ) )
      {
         gan_err_register ( "segment_string", GAN_ERROR_FAILURE, "" );
         return GAN_FALSE;
      }

   /* success */
   return GAN_TRUE;
}

/**
 * \brief Segments strings in an edge map.
 * \param edge_map The input edge map
 * \param test_length The length of local strings to test
 * \param rms_error_thres Threshold on RMS error of a local line fit
 * \param string_length_thres The smallest length of string allowed
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Segments strings in an edge map by applying a local line fit along
 * the string, splitting it where the error residual is above the
 * given threshold.
 *
 * The parameter \a test_length must be odd and >= 3.
 */
Gan_Bool
 gan_segment_strings_q ( Gan_EdgeFeatureMap *edge_map, unsigned test_length,
                         float rms_error_thres, unsigned string_length_thres )
{
   Gan_EdgeString *string;
   int             i;

   gan_err_test_bool ( test_length >= 3 && (test_length % 2) == 1,
                       "gan_segment_strings_q", GAN_ERROR_ILLEGAL_ARGUMENT,
                       "" );
   for ( string = edge_map->string, i = edge_map->nstrings-1; i >= 0;
         string++, i-- )
      if ( !segment_string ( edge_map, string, (int)test_length,
                             rms_error_thres, (int)string_length_thres ) )
      {
         gan_err_register ( "gan_segment_strings_q", GAN_ERROR_FAILURE, "" );
         return GAN_FALSE;
      }

   /* postprocess feature map, building index array into local feature map */
   gan_edge_feature_map_postprocess ( edge_map );

   /* success */
   return GAN_TRUE;
}

/**
 * \}
 */

/**
 * \}
 */

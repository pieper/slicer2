/**
 * File:          $RCSfile: orthog_line.c,v $
 * Module:        Line segment fitting algorithm
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
#include <gandalf/vision/orthog_line.h>
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

static Gan_Bool
 fit_line_to_string ( Gan_EdgeString *string, unsigned min_length,
                      unsigned cut_size, float rms_error_thresh,
                      Gan_LineFeatureMap *line_map,
                      Gan_Bool copy_points,
                      Gan_Vector2_f *point, unsigned max_points )
{
   Gan_EdgeFeature *edge;
   int      j;
   unsigned n;
   float    Sx, Sy, Sxx, Sxy, Syy, Sstr, p, q, r, lambda, nf;
   float    Sxs=0.0F, Sys=0.0F, Sxxs=0.0F,
            Sxys=0.0F, Syys=0.0F, Sstrs=0.0F;
   float    rms_error_thresh_sqr = rms_error_thresh*rms_error_thresh;
   Gan_Vector2_f *pos, *pos1=NULL, *pos2=NULL;
   Gan_LineFeature *line;
   Gan_SquMatrix22_f N;

   /* initialize orthogonal regression fit */
   Sx = Sy = Sxx = Sxy = Syy = Sstr = 0.0F; n = 0;

   gan_err_test_bool ( min_length > 0, "fit_line_to_string",
                       GAN_ERROR_ILLEGAL_ARGUMENT, "" );
   for ( j = string->length-1, edge = string->first; j >= 0;
         j--, edge = edge->next )
   {
      /* use edge position with distortion correction */
      pos = &edge->pu;

      /* store start section to be cut off later */
      if ( n == cut_size )
      {
         Sxs = Sx; Sys = Sy; Sxxs = Sxx; Sxys = Sxy; Syys = Syy; Sstrs = Sstr;
         pos1 = pos;
      }

      Sx += pos->x; Sy += pos->y;
      Sxx += pos->x*pos->x; Sxy += pos->x*pos->y; Syy += pos->y*pos->y;
      Sstr += edge->strength;
      if ( copy_points )
      {
         assert ( n < max_points );
         point[n].x = pos->x;
         point[n].y = pos->y;
      }

      n++;

      /* check fit just before minimum length is reached */
      if ( n >= min_length-1 )
      {
         /* compute sums normalized for centroid */
         nf = (float) n;
         p = Sxx - Sx*Sx/nf;
         q = Sxy - Sx*Sy/nf;
         r = Syy - Sy*Sy/nf;

         /* compute smallest non-zero eigenvalue of covariance
            matrix (p q 0)
                   (q r 0)
                   (0 0 0) */
         lambda = 0.5F*(p + r - (float)sqrt((p-r)*(p-r) + 4.0*q*q));

         /* terminate fit if RMS error is too big or we are at the end
            of the string */
         if ( j == 0 || lambda > nf*rms_error_thresh_sqr )
         {
            if ( n >= min_length ) /* accept this line */
            {
               float D, d1, d2, mx, my, mz, s2, x1, y1, x2, y2;

               if ( cut_size > 0 )
               {
                  Gan_EdgeFeature *eptr = edge->prev;
                  unsigned i;

                  /* cut off segment at start */
                  Sx -= Sxs; Sy -= Sys; Sxx -= Sxxs; Sxy -= Sxys; Syy -= Syys;
                  Sstr -= Sstrs;

                  /* cut off end segment by back-tracking */
                  Sx -= pos->x; Sy -= pos->y;
                  Sxx -= pos->x*pos->x; Sxy -= pos->x*pos->y;
                  Syy -= pos->y*pos->y; Sstr -= edge->strength;
                  i = 1;
                  for(;;)
                  {
                     pos = &eptr->pu;
                     Sx -= pos->x; Sy -= pos->y;
                     Sxx -= pos->x*pos->x; Sxy -= pos->x*pos->y;
                     Syy -= pos->y*pos->y; Sstr -= eptr->strength;
                     if ( ++i == cut_size )
                     {
                        pos2 = pos;
                        break;
                     }

                     eptr = eptr->prev;
                  }

                  /* now both ends have been cut off */
                  n -= 2*cut_size;
               }
               else
                  pos2 = pos;

               /* compute line fit */
               nf = (float) n;
               p = Sxx - Sx*Sx/nf;
               q = Sxy - Sx*Sy/nf;
               r = Syy - Sy*Sy/nf;
               D = (float)sqrt((p-r)*(p-r) + 4.0*q*q);
               d1 = 0.5F*(r-p-D);
               d2 = 0.5F*(p-r-D);

               /* compute line parameters mx*x + my*y + mz = 0 */
               if ( d1*q > 0 )
               {
                  mx = q + d2;
                  my = d1 + q;
               }
               else
               {
                  mx = q - d2;
                  my = d1 - q;
               }

               mz = -(mx*Sx + my*Sy)/nf;
               s2 = mx*mx + my*my;

               /* project start and end edgels perpendicularly onto line
                  to compute endpoints */
               x1 = (my*my*pos1->x - mx*my*pos1->y - mx*mz)/s2;
               y1 = (mx*mx*pos1->y - mx*my*pos1->x - my*mz)/s2;
               x2 = (my*my*pos2->x - mx*my*pos2->y - mx*mz)/s2;
               y2 = (mx*mx*pos2->y - mx*my*pos2->x - my*mz)/s2;

               /* compute average edge strength along line */
               Sstr /= nf;

               /* set covariance matrix of line parameters a & b */
               (void)gan_symmat22f_fill_q(&N,
                                          2.0F*rms_error_thresh_sqr
                                          /(gan_sqr_f(x1-x2)+gan_sqr_f(y2-y1)),
                                          0.0F, rms_error_thresh_sqr*0.5F );

               /* add line to line map */
               line = gan_line_feature_add ( line_map, y1, x1, y2, x2, &N,
                                             0, 0, Sstr,
                                             copy_points ? point : NULL,
                                             copy_points ? n : 0 );
               if ( line == NULL )
               {
                  gan_err_register ( "fit_line_to_string", GAN_ERROR_FAILURE,
                                     "" );
                  return GAN_FALSE;
               }
            }

            /* re-initialize orthogonal regression fit for new line */
            if ( j != 0 )
            {
               Sx = Sy = Sxx = Sxy = Syy = Sstr = 0.0F; n = 0;
            }
         }
      }
   }

   /* success */
   return GAN_TRUE;
}

/**
 * \brief Computes image lines using an orthogonal regression algorithm.
 * \param edge_map The input edge map
 * \param min_length The minimum line length
 * \param cut_size The length to cut off the ends of each line
 * \param rms_error_thres Threshold on RMS error of line fit
 * \param lpms Pointer to local feature map parameters or NULL
 * \param copy_points Whether to copy the fitted edge points into a line
 * \param line_map Resulting line map
 * \param set_progress Function to update progress from 1 to 100  or \c NULL
 * \param progress_obj Pointer passed into \a set_proogress function or \c NULL
 *
 * Applies a recursive orthogonal regression line finder to edges found in an
 * image.
 *
 * \return Non-\c NULL a pointer to the computed line map structure, or
 *         \c NULL on failure.
 * \sa gan_orthog_line_s().
 */
Gan_LineFeatureMap *
 gan_orthog_line_q ( Gan_EdgeFeatureMap *edge_map, unsigned min_length,
                     unsigned cut_size, float rms_error_thres,
                     Gan_LocalFeatureMapParams *lpms, Gan_Bool copy_points,
                     Gan_LineFeatureMap *line_map,
                     void (*set_progress) ( void *, unsigned ),
                     void *progress_obj )
{
   Gan_EdgeString *string;
   int             i;

   /* array of points stored for one string */
   Gan_Vector2_f *point_array = NULL;
   unsigned max_npoints = 0;

   if ( set_progress != NULL )
      set_progress ( progress_obj, 0 );

   /* allocate line map if necessary */
   if ( line_map == NULL )
   {
      line_map = gan_line_feature_map_alloc(0);
      if ( line_map == NULL )
      {
         gan_err_register ( "gan_orthog_line_q", GAN_ERROR_FAILURE, "" );
         return NULL;
      }
   }

   if ( copy_points )
   {
      /* allocate point array as big as would normally be expected */
      max_npoints = 2*gan_max2_i(edge_map->width, edge_map->height);
      point_array = gan_malloc_array ( Gan_Vector2_f, max_npoints );
      if ( point_array == NULL )
      {
         gan_err_flush_trace();
         gan_err_register_with_number ( "gan_orthog_line_q", GAN_ERROR_MALLOC_FAILED, "", max_npoints*sizeof(Gan_Vector2_f) );
         return NULL;
      }
   }

   /* set line map attributes and clear line map */
   if ( !gan_line_feature_map_clear ( line_map, edge_map->height,
                                                edge_map->width,
                                      edge_map->A_set ? &edge_map->A : NULL,
                                      lpms ) )
   {
      gan_err_register ( "gan_orthog_line_q", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   for ( string = edge_map->string, i = edge_map->nstrings-1; i >= 0;
         string++, i-- )
   {
      if ( !fit_line_to_string ( string, min_length, cut_size, rms_error_thres,
                                 line_map,
                                 copy_points, point_array, max_npoints ) )
      {
         gan_err_register ( "gan_orthog_line_q", GAN_ERROR_FAILURE, "" );
         return NULL;
      }

      if ( set_progress != NULL && (i % 50) == 0 )
         set_progress ( progress_obj,
                        (int)(100.0*(double)(edge_map->nstrings-(unsigned)i-1)
                              /(double)edge_map->nstrings) );
   }

   if ( point_array != NULL )
      /* free point array */
      free ( point_array );

   /* postprocess feature map, building index array into local feature map */
   gan_line_feature_map_postprocess ( line_map );

   if ( set_progress != NULL )
      set_progress ( progress_obj, 99 );

   /* success */
   return line_map;
}

/**
 * \}
 */

/**
 * \}
 */

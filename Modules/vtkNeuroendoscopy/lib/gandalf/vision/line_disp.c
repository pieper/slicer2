/**
 * File:          $RCSfile: line_disp.c,v $
 * Module:        Line feature display using Open GL
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

#include <stdlib.h>
#include <stdio.h>
#include <gandalf/vision/line_disp.h>
#include <gandalf/image/image_defs.h>
#include <gandalf/common/misc_error.h>

/**
 * \addtogroup Vision
 * \{
 */

/**
 * \addtogroup FeatureDetection
 * \{
 */

#ifdef HAVE_OPENGL

#ifdef WIN32
        #include <windows.h>
#endif

#ifndef __APPLE__
#include <GL/gl.h>
#else
#include <OpenGL/gl.h>
#endif

static Gan_Bool line_display ( Gan_LineFeature *line,
                               float line_thickness,
                               Gan_Matrix23_f *A,
                               Gan_RGBPixel_f *line_colour,
                               Gan_RGBPixel_f *first_colour,
                               Gan_RGBPixel_f *last_colour,
                               Gan_RGBPixel_f *point_colour )
{
   int i;

   if ( A == NULL )
   {
      if ( line->point != NULL && point_colour != NULL )
      {
         glPushAttrib(GL_POLYGON_BIT);
         glPolygonMode ( GL_FRONT_AND_BACK, GL_LINE );
         glColor3f ( point_colour->R, point_colour->G, point_colour->B );
         glBegin ( GL_QUADS );
         for ( i = (int)line->npoints-1; i >= 0; i-- )
         {
            glVertex2f ( line->point[i].x-1.0F, line->point[i].y-1.0F );
            glVertex2f ( line->point[i].x-1.0F, line->point[i].y+1.0F );
            glVertex2f ( line->point[i].x+1.0F, line->point[i].y+1.0F );
            glVertex2f ( line->point[i].x-1.0F, line->point[i].y+1.0F );
         }

         glEnd();
         glPopAttrib();
      }

      glLineWidth ( line_thickness );
      glColor3f ( line_colour->R, line_colour->G, line_colour->B );
      glBegin ( GL_LINES );
      glVertex2f ( line->p1.x, line->p1.y );
      glVertex2f ( line->p2.x, line->p2.y );
      glEnd();

      glPointSize ( line_thickness );
      glBegin ( GL_POINTS );
      glColor3f ( first_colour->R, first_colour->G, first_colour->B );
      glVertex2f ( line->p1.x, line->p1.y );
      glColor3f ( last_colour->R, last_colour->G, last_colour->B );
      glVertex2f ( line->p2.x, line->p2.y );
      glEnd();
   }
   else
   {
      Gan_Vector2_f p1, p2;

      if ( line->point != NULL && point_colour != NULL )
      {
         glPushAttrib(GL_POLYGON_BIT);
         glPolygonMode ( GL_FRONT_AND_BACK, GL_LINE );
         glColor3f ( point_colour->R, point_colour->G, point_colour->B );
         glBegin ( GL_QUADS );
         for ( i = (int)line->npoints-1; i >= 0; i-- )
         {
            p1 = gan_vec2f_fill_s ( line->point[i].x, line->point[i].y );
            p1 = gan_mat23f_multv2h_s ( A, &p1, 1.0F );
            glVertex2f ( p1.x-1.0F, p1.y-1.0F );
            glVertex2f ( p1.x-1.0F, p1.y+1.0F );
            glVertex2f ( p1.x+1.0F, p1.y+1.0F );
            glVertex2f ( p1.x-1.0F, p1.y+1.0F );
         }

         glEnd();
         glPopAttrib();
      }

      p1 = gan_mat23f_multv2h_s ( A, &line->p1, 1.0F );
      p2 = gan_mat23f_multv2h_s ( A, &line->p2, 1.0F );

      glLineWidth ( line_thickness );
      glColor3f ( line_colour->R, line_colour->G, line_colour->B );
      glBegin ( GL_LINES );
      glVertex2f ( p1.x, p1.y );
      glVertex2f ( p2.x, p2.y );
      glEnd();

      glPointSize ( line_thickness );
      glBegin ( GL_POINTS );
      glColor3f ( first_colour->R, first_colour->G, first_colour->B );
      glVertex2f ( p1.x, p1.y );
      glColor3f ( last_colour->R, last_colour->G, last_colour->B );
      glVertex2f ( p2.x, p2.y );
      glEnd();
   }

   return GAN_TRUE;
}

/**
 * \brief Displays a single line feature
 * \param line A line feature
 * \param line_thickness Thickness of lines to render
 * \param A Affine transformation to apply to feature coordinates
 * \param line_colour The colour to display the line feature
 * \param first_colour The colour to highlight the first line endpoint
 * \param last_colour The colour to highlight the second line endpoint
 * \param point_colour The colour to display points, where applicable,
 *        or \c NULL if you don't want to display them
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Displays a single line feature.
 */
Gan_Bool
 gan_line_feature_display ( Gan_LineFeature *line,
                            float line_thickness,
                            Gan_Matrix23_f *A,
                            Gan_RGBPixel_f *line_colour,
                            Gan_RGBPixel_f *first_colour,
                            Gan_RGBPixel_f *last_colour,
                            Gan_RGBPixel_f *point_colour )
{
   if ( !line_display ( line, line_thickness, A,
                        line_colour, first_colour,
                        last_colour, point_colour ) )
   {
      gan_err_register ( "gan_line_feature_display", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   return GAN_TRUE;
}

/**
 * \brief Displays a line feature map
 * \param lmap The line feature map
 * \param line_thickness Thickness of lines to render
 * \param A Affine transformation to apply to feature coordinates
 * \param line_colour The colour to display the line features
 * \param first_colour The colour to highlight the first line endpoint
 * \param last_colour The colour to highlight the second line endpoint
 * \param point_colour The colour to display points, where applicable,
 *        or \c NULL if you don't want to display them
 * \param bbox_colour The colour to display the bounding box
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Displays a line feature map.
 */
Gan_Bool
 gan_line_feature_map_display ( Gan_LineFeatureMap *lmap,
                                float line_thickness,
                                Gan_Matrix23_f *A,
                                Gan_RGBPixel_f *line_colour,
                                Gan_RGBPixel_f *first_colour,
                                Gan_RGBPixel_f *last_colour,
                                Gan_RGBPixel_f *point_colour,
                                Gan_RGBPixel_f *bbox_colour )
{
   int i;
   Gan_Vector2_f b[4];

   /* fill initial bounding bow coordinates */
   (void)gan_vec2f_fill_q ( &b[0], 0.0F, 0.0F );
   (void)gan_vec2f_fill_q ( &b[1], 0.0F, (float)lmap->height );
   (void)gan_vec2f_fill_q ( &b[2], (float)lmap->width, (float)lmap->height );
   (void)gan_vec2f_fill_q ( &b[3], (float)lmap->width, 0.0F );

   if ( A != NULL )
      /* transform using provided transformation matrix */
      for ( i = 0; i < 4; i++ )
         b[i] = gan_mat23f_multv2h_s ( A, &b[i], 1.0F );

   /* draw bounding box */
   glPushAttrib(GL_POLYGON_BIT);
   glPolygonMode ( GL_FRONT_AND_BACK, GL_LINE );
   glColor3f ( bbox_colour->R, bbox_colour->G, bbox_colour->B );
   glBegin ( GL_QUADS );
   glVertex2f ( b[0].x, b[0].y );
   glVertex2f ( b[1].x, b[1].y );
   glVertex2f ( b[2].x, b[2].y );
   glVertex2f ( b[3].x, b[3].y );
   glEnd();
   glPopAttrib();

   /* display lines */
   for ( i = lmap->nlines-1; i >= 0; i-- )
      if ( !line_display ( &lmap->line[i], line_thickness, A,
                           line_colour, first_colour,
                           last_colour, point_colour ) )
      {
         gan_err_register ( "gan_line_feature_map_display", GAN_ERROR_FAILURE,
                            "" );
         return GAN_FALSE;
      }

   /* success */
   return GAN_TRUE;
}

#endif /* #ifdef HAVE_OPENGL */

/**
 * \}
 */

/**
 * \}
 */

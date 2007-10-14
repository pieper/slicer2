/**
 * File:          $RCSfile: edge_disp.c,v $
 * Module:        Edge feature display using Open GL
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

#include <stdlib.h>
#include <stdio.h>
#include <gandalf/vision/edge_disp.h>
#include <gandalf/image/image_defs.h>

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

/**
 * \brief Displays a single edge feature.
 * \param edge A edge feature
 * \param size The size of cross to draw on the display
 * \param A Affine transformation to apply to feature coordinates
 * \param edge_colour The colour to display the feature
 *
 * Displays a single edge feature. If \a size is passed as zero,
 * the edge is drawn as a point with current point size.
 * Otherwise the edge as drawn as a cross of the given \a size.
 *
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 */
Gan_Bool
 gan_edge_feature_display ( Gan_EdgeFeature *edge,
                            float size, Gan_Matrix23_f *A,
                            Gan_RGBPixel_f *edge_colour )
{
   glColor3f ( edge_colour->R, edge_colour->G, edge_colour->B );
   if ( A == NULL )
   {
      if ( size == 0.0F )
      {
         glBegin ( GL_POINTS );
         glVertex2f ( edge->p.x, edge->p.y );
         glEnd();
      }
      else
      {
         glBegin ( GL_LINES );
         glVertex2f ( edge->p.x - size, edge->p.y - size );
         glVertex2f ( edge->p.x + size, edge->p.y + size );
         glVertex2f ( edge->p.x + size, edge->p.y - size );
         glVertex2f ( edge->p.x - size, edge->p.y + size );
         glEnd();
      }
   }
   else
   {
      Gan_Vector2_f p;

      (void)gan_vec2f_fill_q ( &p,
                               A->xx*edge->p.x + A->xy*edge->p.y + A->xz,
                               A->yx*edge->p.x + A->yy*edge->p.y + A->yz );
      if ( size == 0.0F )
      {
         glBegin ( GL_POINTS );
         glVertex2f ( p.x, p.y );
         glEnd();
      }
      else
      {
         glBegin ( GL_LINES );
         glVertex2f ( p.x - size, p.y - size );
         glVertex2f ( p.x + size, p.y + size );
         glVertex2f ( p.x + size, p.y - size );
         glVertex2f ( p.x - size, p.y + size );
         glEnd();
      }
   }

   glFlush();
   return GAN_TRUE;
}

/**
 * \brief Displays a edge feature map.
 * \param emap The edge feature map
 * \param size The size of cross to draw on the display
 * \param A Affine transformation to apply to feature coordinates
 * \param edge_colour The colour to display the features
 * \param string_colour The colour to display any strings
 * \param first_colour The colour to display the start of a string
 * \param last_colour The colour to display the end of a string
 * \param bbox_colour The colour to display the bounding box
 *
 * Displays a edge feature map. If size is passed as zero,
 * the edge is drawn as a point with current point size.
 * Otherwise the edge as drawn as a cross of the given size.
 *
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 */
Gan_Bool
 gan_edge_feature_map_display ( Gan_EdgeFeatureMap *emap,
                                float size, Gan_Matrix23_f *A,
                                Gan_RGBPixel_f *edge_colour,
                                Gan_RGBPixel_f *string_colour,
                                Gan_RGBPixel_f *first_colour,
                                Gan_RGBPixel_f *last_colour,
                                Gan_RGBPixel_f *bbox_colour )
{
   int i, j;
   Gan_Vector2_f p, b[4];

   /* fill initial bounding bow coordinates */
   (void)gan_vec2f_fill_q ( &b[0], 0.0F, 0.0F );
   (void)gan_vec2f_fill_q ( &b[1], 0.0F, (float)emap->height );
   (void)gan_vec2f_fill_q ( &b[2], (float)emap->width, (float)emap->height );
   (void)gan_vec2f_fill_q ( &b[3], (float)emap->width, 0.0F );

   if ( emap->A_set )
      /* transform using feature map transformation */
      for ( i = 0; i < 4; i++ )
         b[i] = gan_mat23f_multv2h_s ( &emap->A, &b[i], 1.0F );

   if ( A != NULL )
      /* transform using provided transformation matrix */
      for ( i = 0; i < 4; i++ )
         b[i] = gan_mat23f_multv2h_s ( A, &b[i], 1.0F );

   /* draw bounding box */
   glPolygonMode ( GL_FRONT_AND_BACK, GL_LINE );
   glColor3f ( bbox_colour->R, bbox_colour->G, bbox_colour->B );
   glBegin ( GL_QUADS );
   glVertex2f ( b[0].x, b[0].y );
   glVertex2f ( b[1].x, b[1].y );
   glVertex2f ( b[2].x, b[2].y );
   glVertex2f ( b[3].x, b[3].y );
   glEnd();

   /* display edges */
   if ( size == 0.0F )
      glBegin ( GL_POINTS );
   else
      glBegin ( GL_LINES );

   glColor3f ( edge_colour->R, edge_colour->G, edge_colour->B );
   for ( i = emap->nedges-1; i >= 0; i-- )
   {
      (void)gan_vec2f_fill_q ( &p, emap->edge[i].p.x, emap->edge[i].p.y );

      if ( emap->A_set )
         /* transform using feature map transformation */
         p = gan_mat23f_multv2h_s ( &emap->A, &p, 1.0F );

      if ( A != NULL )
         /* transform using provided transformation matrix */
         p = gan_mat23f_multv2h_s ( A, &p, 1.0F );

      if ( size == 0.0F )
         glVertex2f ( p.x, p.y );
      else
      {
         glVertex2f ( p.x - size, p.y - size );
         glVertex2f ( p.x + size, p.y + size );
         glVertex2f ( p.x + size, p.y - size );
         glVertex2f ( p.x - size, p.y + size );
      }
   }

   glEnd();

   /* overlay strings */
   if ( size == 0.0F )
      glBegin ( GL_POINTS );
   else
      glBegin ( GL_LINES );

   for ( i = (int)emap->nstrings-1; i >= 0; i-- )
   {
      Gan_EdgeString *string = &emap->string[i];
      Gan_EdgeFeature *edge;

      for ( j = (int)string->length-1, edge = string->first; j >= 0;
            j--, edge = edge->next )
      {
         (void)gan_vec2f_fill_q ( &p, edge->p.x, edge->p.y );

         if ( emap->A_set )
            /* transform using feature map transformation */
            p = gan_mat23f_multv2h_s ( &emap->A, &p, 1.0F );

         if ( A != NULL )
            /* transform using provided transformation matrix */
            p = gan_mat23f_multv2h_s ( A, &p, 1.0F );

         /* get colour for this feature */
         if ( j == (int) string->length-1 )
            glColor3f ( first_colour->R, first_colour->G, first_colour->B );
         else if ( j == 0 )
            glColor3f ( last_colour->R, last_colour->G, last_colour->B );
         else
            glColor3f ( string_colour->R, string_colour->G, string_colour->B );

         if ( size == 0.0F )
            glVertex2f ( p.x, p.y );
         else
         {
            glVertex2f ( p.x - size, p.y - size );
            glVertex2f ( p.x + size, p.y + size );
            glVertex2f ( p.x + size, p.y - size );
            glVertex2f ( p.x - size, p.y + size );
         }
      }
   }

   glEnd();
   glFlush();

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

/**
 * File:          $RCSfile: local_feature.c,v $
 * Module:        Local blocked feature map definition & functions
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

#include <gandalf/vision/local_feature.h>
#include <gandalf/common/allocate.h>
#include <gandalf/common/misc_error.h>
#include <gandalf/image/image_gl_int.h>
#include <gandalf/image/image_pointer.h>

/**
 * \addtogroup Vision
 * \{
 */

/**
 * \addtogroup FeatureDetection
 * \{
 */

/**
 * \brief Forms a local feature map structure.
 * \param pmap The local feature map to form
 * \param height The vertical size of the local feature map
 * \param width The vertical size of the local feature map
 * \param pms Pointer to a structure holding other parameters or NULL
 *
 * Forms a structure to hold a local blocked representation of indices of
 * features stored in a separate feature map (e.g. a point/edge/line feature
 * map), and sets it to be empty. The \a pms structure contains the
 * vertical/horizontal offsets between blocks, and the "catchment area" or
 * size of each block.
 *
 * \return non-\c NULL the formed local feature map, or \c NULL on failure.
 * \sa gan_local_feature_map_form(), gan_local_feature_map_free().
 */
Gan_LocalFeatureMap *
 gan_local_feature_map_form ( Gan_LocalFeatureMap *pmap,
                              unsigned height, unsigned width,
                              Gan_LocalFeatureMapParams *pms )
{
   unsigned yblocks, xblocks;

   if ( pmap == NULL )
   {
      /* dynamically allocate local feature map */
      pmap = gan_malloc_object(Gan_LocalFeatureMap);
      if ( pmap == NULL )
      {
         gan_err_flush_trace();
         gan_err_register_with_number ( "gan_local_feature_map_form", GAN_ERROR_MALLOC_FAILED, "local feature map", sizeof(*pmap) );
         return NULL;
      }

      pmap->alloc = GAN_TRUE;
   }
   else
      pmap->alloc = GAN_FALSE;

   /* calculate number of blocks vertically & horizontally */
   if ( pms == NULL || pms->ystep == 0 )
   {
      /* special case: empty local feature map */
      gan_err_test_ptr ( pms == NULL || pms->xstep == 0,
                         "gan_local_feature_map_form",
                         GAN_ERROR_INCOMPATIBLE, "step parameters" );
      xblocks = yblocks = 0;
   }
   else
   {
      gan_err_test_ptr ( pms->xstep != 0, "gan_local_feature_map_form",
                         GAN_ERROR_INCOMPATIBLE, "step parameters" );
      gan_err_test_ptr ( pms->bsize % 2 == 1,
                          "gan_local_feature_map_form",GAN_ERROR_INCOMPATIBLE,
                          "local feature pmap catchment area should be odd");
      xblocks = (width + pms->xstep - 1)/pms->xstep + pms->bsize - 1;
      yblocks = (width + pms->ystep - 1)/pms->ystep + pms->bsize - 1;
   }

   /* allocate image of feature totals */
   if ( gan_image_form_gl_i ( &pmap->nfeatures, yblocks, xblocks ) == NULL )
   {
      gan_err_register ( "gan_local_feature_map_form", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* allocate image of feature indices */
   if ( gan_image_form_p ( &pmap->index, yblocks, xblocks ) == NULL )
   {
      gan_err_register ( "gan_local_feature_map_form", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* clear feature map */
   if ( !gan_local_feature_map_clear ( pmap, height, width, pms ) )
   {
      gan_err_register ( "gan_local_feature_map_form", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* success */
   return pmap;
}

/**
 * \brief Frees a local feature map structure.
 * \param pmap The local feature map to free
 *
 * Frees a structure holding a local blocked representation of indices of
 * features.
 *
 * \return No value
 * \sa gan_local_feature_map_form(), gan_local_feature_map_alloc().
 */
void
 gan_local_feature_map_free ( Gan_LocalFeatureMap *pmap )
{
   if ( pmap->index.width > 0 && pmap->index.height > 0 &&
        gan_image_get_pix_p ( &pmap->index, 0, 0 ) != NULL )
      free ( gan_image_get_pix_p ( &pmap->index, 0, 0 ) );

   gan_image_free ( &pmap->index );
   gan_image_free ( &pmap->nfeatures );
   if ( pmap->alloc ) free ( pmap );
}

/**
 * \brief Clears a local feature map structure.
 * \param pmap The local feature map
 * \param height The vertical size of the local feature map
 * \param width The vertical size of the local feature map
 * \param pms Pointer to a structure holding other parameters or NULL
 *
 * Clears all the stored indices from an existing local feature map, and
 * resets the dimensions of the feature map according to the provided
 * arguments.
 *
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 * \sa gan_local_feature_map_form(), gan_local_feature_map_free().
 */
Gan_Bool
 gan_local_feature_map_clear ( Gan_LocalFeatureMap *pmap,
                               unsigned height, unsigned width,
                               Gan_LocalFeatureMapParams *pms )
{
   /* calculate number of blocks vertically & horizontally */
   if ( pms == NULL || pms->ystep == 0 )
   {
      /* special case: empty local feature map */
      gan_err_test_bool ( pms == NULL || pms->xstep == 0,
                          "gan_local_feature_map_form",
                          GAN_ERROR_INCOMPATIBLE, "step parameters" );
      pmap->xblocks = pmap->yblocks = 0;
   }
   else
   {
      gan_err_test_bool ( pms->xstep != 0, "gan_local_feature_map_clear",
                          GAN_ERROR_INCOMPATIBLE, "step parameters" );
      gan_err_test_bool ( pms->bsize % 2 == 1,
                          "gan_local_feature_map_clear",GAN_ERROR_INCOMPATIBLE,
                          "local feature pmap catchment area should be odd");
      pmap->xblocks = (width  + pms->xstep - 1)/pms->xstep + pms->bsize - 1;
      pmap->yblocks = (height + pms->ystep - 1)/pms->ystep + pms->bsize - 1;
   }

   /* allocate image of feature totals */
   if ( gan_image_set_gl_i ( &pmap->nfeatures,
                             pmap->yblocks, pmap->xblocks ) == NULL )
   {
      gan_err_register ( "gan_local_feature_map_clear", GAN_ERROR_FAILURE, "");
      return GAN_FALSE;
   }

   /* free first element of index array in case it has been allocated */
   if ( pmap->index.width > 0 && pmap->index.height > 0 )
      if ( gan_image_get_pix_p ( &pmap->index, 0, 0 ) != NULL )
         free ( gan_image_get_pix_p ( &pmap->index, 0, 0 ) );

   /* allocate image of feature indices */
   if ( gan_image_set_p ( &pmap->index,
                          pmap->yblocks, pmap->xblocks ) == NULL )
   {
      gan_err_register ( "gan_local_feature_map_clear", GAN_ERROR_FAILURE, "");
      return GAN_FALSE;
   }

   /* set counters to zero */
   gan_image_fill_const_gl_i ( &pmap->nfeatures, 0 );
   gan_image_fill_const_p ( &pmap->index, NULL );

   if ( pms == NULL )
      pmap->xstep = pmap->ystep = pmap->bsize2 = 0;
   else
   {
      /* copy fields from parameter structure */
      pmap->xstep = pms->xstep;
      pmap->ystep = pms->ystep;
      gan_err_test_bool ( pms->ystep == 0 || pms->bsize % 2 == 1,
                          "gan_local_feature_map_clear",GAN_ERROR_INCOMPATIBLE,
                          "local feature pmap catchment area should be odd");
      pmap->bsize2 = pms->bsize/2;
   }

   /* success */
   return GAN_TRUE;
}

/**
 * \brief Adds a point feature to a local feature map.
 * \param pmap The local feature map
 * \param r Row position of the feature
 * \param c Column position of the feature
 * \param index Index of the feature in the feature map referenced
 *
 * Adds a point feature at position (r,c) to the given local feature map.
 * If index is \c UINT_MAX, the index is ignored;
 * otherwise it is added to the index array stored in the local feature map.
 *
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 * \sa gan_local_feature_line_add().
 */
Gan_Bool
 gan_local_feature_point_add ( Gan_LocalFeatureMap *pmap,
                               unsigned r, unsigned c, unsigned index )
{
   unsigned cs, rs, ce, re;
   int **nfeatures = gan_image_get_pixarr_gl_i(&pmap->nfeatures);

   /* exit if local feature map has illegal step size between blocks */
   gan_err_test_bool ( pmap->xstep != 0 && pmap->ystep != 0,
                       "gan_local_feature_point_add", GAN_ERROR_INCOMPATIBLE,
                       "" );

   /* compute block position of feature */
   r = r/pmap->ystep + pmap->bsize2;
   c = c/pmap->xstep + pmap->bsize2;

   /* calculate extent of catchment area for this feature */
   cs = c - pmap->bsize2;
   rs = r - pmap->bsize2;
   ce = c + pmap->bsize2;
   re = r + pmap->bsize2;

   /* add count for blocks either side, as specified by the catchment area */
   if ( index == UINT_MAX )
      for ( r = rs; r <= re; r++ )
         for ( c = cs; c <= ce; c++ )
         {
            gan_err_test_bool ( r < pmap->nfeatures.height && c < pmap->nfeatures.width,
                                "gan_local_feature_point_add", GAN_ERROR_INCOMPATIBLE, "" );
            nfeatures[r][c]++;
         }
   else
   {
      /* also build indices for the features at the same time */
      int ***indexarr = (int ***) gan_image_get_pixarr_p(&pmap->index);

      for ( r = rs; r <= re; r++ )
         for ( c = cs; c <= ce; c++ )
         {
            gan_err_test_bool ( r < pmap->nfeatures.height && c < pmap->nfeatures.width &&
                                r < pmap->index.height     && c < pmap->index.width,
                                "gan_local_feature_point_add", GAN_ERROR_INCOMPATIBLE, "" );
            indexarr[r][c][nfeatures[r][c]++] = index;
         }
   }

   /* success */
   return GAN_TRUE;
}

/**
 * \brief Adds a line feature to a local feature map.
 * \param pmap The local feature map
 * \param r1 Row position of first endpoint of the feature
 * \param c1 Column position of first endpoing of the feature
 * \param r2 Row position of second endpoint of the feature
 * \param c2 Column position of second endpoing of the feature
 * \param index Index of the feature in the feature map referenced
 *
 * Adds a line feature at endpoint positions (r1,c1) and (r2,c2)
 * to the given local feature map. If index is \c UINT_MAX, the
 * index is ignored; otherwise it is added to the index array
 * stored in the local feature map.
 *
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 * \sa gan_local_feature_point_add().
 */
Gan_Bool
 gan_local_feature_line_add ( Gan_LocalFeatureMap *pmap,
                              unsigned r1, unsigned c1,
                              unsigned r2, unsigned c2, unsigned index )
{
   int **nfeatures = gan_image_get_pixarr_gl_i(&pmap->nfeatures);
   int ***indexarr = (int ***) gan_image_get_pixarr_p(&pmap->index);
   unsigned r, c, rdiff, cdiff;

   /* exit if local feature map has illegal step size between blocks */
   gan_err_test_bool ( pmap->xstep != 0 && pmap->ystep != 0,
                       "gan_local_feature_line_add", GAN_ERROR_INCOMPATIBLE,
                       "" );

   /* compute block position of feature */
   r1 /= pmap->ystep; c1 /= pmap->xstep;
   r2 /= pmap->ystep; c2 /= pmap->xstep;

   /* if endpoint block coordinates are coincident, use point function */
   if ( c1 == c2 && r1 == r2 )
      return gan_local_feature_point_add ( pmap, r1*pmap->ystep,
                                                 c1*pmap->xstep,
                                           index );

   /* check whether extent of line is greater in the horizontal or vertical
      direction */
   cdiff = (c2 > c1) ? c2-c1 : c1-c2;
   rdiff = (r2 > r1) ? r2-r1 : r1-r2;
   if ( rdiff > cdiff )
   {
      /* line is oriented vertically */
      unsigned cs, ce;

      /* order endpoints so that r1 <= r2 */
      if ( r1 > r2 )
      {
         unsigned tmp;

         tmp = r1; r1 = r2; r2 = tmp;
         tmp = c1; c1 = c2; c2 = tmp;
      }

      for ( r = r1; r <= r2+2*pmap->bsize2;r++ )
      {
         if ( c2 > c1 )
            /* calculate row position on line c = c1 + (c2-c1)*(r-r1)/(r2-r1) */
            c = c1 + (cdiff*(r-r1) + (rdiff-cdiff)*pmap->bsize2 + rdiff/2)/rdiff;
         else
            /* calculate row position on line c = c2 + (c1-c2)*(r2-r)/(r2-r1) */
            c = c2 + (cdiff*(r2-r) + (rdiff-cdiff)*pmap->bsize2 + rdiff/2)/rdiff;

         ce = c+pmap->bsize2;
         if ( ce >= pmap->xblocks ) ce = pmap->yblocks-1;
         cs = (c >= pmap->bsize2) ? c-pmap->bsize2 : 0;
         if ( index == UINT_MAX )
            for ( c = cs; c <= ce; c++ )
               nfeatures[r][c]++;
         else
            for ( c = cs; c <= ce; c++ )
               indexarr[r][c][nfeatures[r][c]++] = index;
      }
   }
   else
   {
      /* line is oriented horizontally */
      unsigned rs, re;

      /* order endpoints so that c1 <= c2 */
      if ( c1 > c2 )
      {
         unsigned tmp;

         tmp = c1; c1 = c2; c2 = tmp;
         tmp = r1; r1 = r2; r2 = tmp;
      }

      for ( c = c1; c <= c2+2*pmap->bsize2; c++ )
      {
         if ( r2 > r1 )
            /* calculate row position on line r = r1 + (r2-r1)*(c-c1)/(c2-c1) */
            r = r1 + (rdiff*(c-c1) + (cdiff-rdiff)*pmap->bsize2 + cdiff/2)/cdiff;
         else
            /* calculate row position on line r = r2 + (r1-r2)*(c2-c)/(c2-c1) */
            r = r2 + (rdiff*(c2-c) + (cdiff-rdiff)*pmap->bsize2 + cdiff/2)/cdiff;

         re = r+pmap->bsize2;
         if ( re >= pmap->yblocks ) re = pmap->yblocks-1;
         rs = (r >= pmap->bsize2) ? r-pmap->bsize2 : 0;
         if ( index == UINT_MAX )
            for ( r = rs; r <= re; r++ )
               nfeatures[r][c]++;
         else
            for ( r = rs; r <= re; r++ )
               indexarr[r][c][nfeatures[r][c]++] = index;
      }
   }

   /* success */
   return GAN_TRUE;
}

/**
 * \brief Sets indices for a local feature map.
 * \param pmap The local feature map
 *
 * Having called gan_local_feature_point_add() and/or
 * gan_local_feature_line_add() to count the features in the
 * local feature map, this function allocates and builds
 * the array of indices for those features. It doesn't fill the
 * array with values, however. This is done by calling
 * gan_local_feature_point_add() and/or
 * gan_local_feature_line_add() again for each point/line feature
 * with the correct index argument.
 *
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 * \sa gan_local_feature_point_add().
 */
Gan_Bool
 gan_local_feature_map_set_indices ( Gan_LocalFeatureMap *pmap )
{
   unsigned total, r, c;
   int **nfeatures = gan_image_get_pixarr_gl_i(&pmap->nfeatures);
   int ***index = (int ***) gan_image_get_pixarr_p(&pmap->index), *iptr;

   /* return immediately if the local feature map is not being used */
   if ( pmap->ystep == 0 ) return GAN_TRUE;

   /* count total number of feature counts */
   for ( total = 0, r = 0; r < pmap->yblocks; r++ )
      for ( c = 0; c < pmap->xblocks; c++ )
         total += nfeatures[r][c];

   if ( total == 0 )
   {
      for ( iptr = index[0][0], r = 0; r < pmap->yblocks; r++ )
         for ( c = 0; c < pmap->xblocks; c++ )
            index[r][c] = NULL;
   }
   else
   {
      /* reallocate array of indices */
      index[0][0] = gan_realloc_array ( int, index[0][0], total );
      if ( index[0][0] == NULL )
      {
         gan_err_flush_trace();
         gan_err_register_with_number ( "gan_point_feature_map_set_indices", GAN_ERROR_MALLOC_FAILED, "", total*sizeof(int) );
         return GAN_FALSE;
      }

      /* build array */
      for ( iptr = index[0][0], r = 0; r < pmap->yblocks; r++ )
         for ( c = 0; c < pmap->xblocks; c++ )
         {
            index[r][c] = iptr;
            iptr += nfeatures[r][c];
         }
   }

   /* success */
   return GAN_TRUE;
}

/**
 * \brief Computes the block a feature position lies in.
 * \param pmap The local feature map
 * \param y The vertical feature position
 * \param x The horizontal feature position
 * \param Ai The transformation from x,y coords. into local feature map
 * \param cr Pointer to the resulting local feature map row
 * \param cc Pointer to the resulting local feature map column
 *
 * Given coordinates x,y for a feature, and a transformation \a Ai
 * to get back to the coordinates of the local feature map,
 * returns the block row/column in the \a cr & \a cc pointers.
 *
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 */
Gan_Bool
 gan_local_feature_map_find_block ( Gan_LocalFeatureMap *pmap,
                                    double y, double x, Gan_Matrix23 *Ai,
                                    unsigned *cr, unsigned *cc )
{
   if ( pmap->xstep == 0 || pmap->ystep == 0 )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_local_feature_map_find_block",
                         GAN_ERROR_INCOMPATIBLE, "local feature map" );
      return GAN_FALSE;
   }

   if ( Ai != NULL )
   {
      x = Ai->xx*x + Ai->xy*y + Ai->xz;
      y = Ai->yx*x + Ai->yy*y + Ai->yz;
   }

   x += (double)(pmap->bsize2*pmap->xstep);
   y += (double)(pmap->bsize2*pmap->ystep);

   *cc = x >= 0.0 ? ((unsigned) x)/pmap->xstep : 0;
   *cr = y >= 0.0 ? ((unsigned) y)/pmap->ystep : 0;
   if ( *cc > pmap->xblocks-1 ) *cc = pmap->xblocks-1;
   if ( *cr > pmap->yblocks-1 ) *cr = pmap->yblocks-1;
   return GAN_TRUE;
}

/**
 * \}
 */

/**
 * \}
 */

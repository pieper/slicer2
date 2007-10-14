/**
 * File:          $RCSfile: modified_fht2D.c,v $
 * Module:        Fast Hough Transform implentation
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

#include <gandalf/vision/modified_fht2D.h>
#include <gandalf/common/memory_stack.h>
#include <gandalf/common/misc_error.h>
#include <gandalf/common/numerics.h>

/**
 * \addtogroup Vision
 * \{
 */

/**
 * \addtogroup HoughTransform Hough Transform
 * \{
 */

/* program to perform Fast Hough Transform line finder */

/* FHT_Path: structure for recursively specifying hypercube path back to
   root hypercube */
typedef struct FHT_Path
{
   int    m_cube, c_cube;   /* either 0 or 1, specify which child out
                               of 2*2 set of child hypercubes this one is. */
   struct FHT_Path *parent; /* if NULL then this is the root hypercube. */
} FHT_Path;

/*
 * Prints the "family tree" of a hypercube specified by the node argument.
 * For each "generation", one of (0,0), (0,1), (1,0) or (1,1) is printed,
 * depending the parent/child relationship. The relationship to the root
 * hypercube is printed first.
 */
static void
 print_path ( FHT_Path *node ) /* hypercube line of descent */
{
   char array[80][2];
   int  ct;

   /* going from node to root (NULL) takes generations in reverse order,
      so we write data into array and then print it in the right order */
   for ( ct = 0; node != NULL; ct++, node = node->parent )
   {
      array[ct][0] = node->m_cube;
      array[ct][1] = node->c_cube;
   }

   printf ( "path ( ");
   if ( ct > 0 )
   {
      while ( --ct > 0 )
         printf ( "(%d,%d), ", array[ct][0], array[ct][1] );

      printf ( "(%d,%d)", array[0][0], array[0][1] );
   }

   printf ( " )\n" );
}

/*
 * Indents a line with the given number of spaces.
 */
static void indent ( int level )
{
   while ( level-- > 0 ) printf ( " " );
}

/**
 * Finds the values of m & c at the centre of a hypercube.
 */
static void
 calc_centre ( FHT_Path *node,   /* hypercube line of descent */
               double *m_centre, /* gradient and intercept are */
               double *c_centre, /* written into here */

               /* definition of root hypercube: */
               double m_range, /* range of gradient m */
               double c_range, /* range of intercept c */
               double c_root ) /* intercept at centre of root */
{
   double m_add[2], c_add[2];

   *m_centre = *c_centre = 0.0;
   m_add[0] = -m_range*0.25; m_add[1] = -m_add[0];
   c_add[0] = -c_range*0.25; c_add[1] = -c_add[0];
   for ( ; node != NULL; node = node->parent )
   {
      *m_centre = *m_centre*0.5 + m_add[node->m_cube];
      *c_centre = *c_centre*0.5 + c_add[node->c_cube];
   }

   *c_centre += c_root;
}

/**
 * Performs FHT recursive subdivision.
 */
static Gan_Bool
 divide_ranges ( Gan_BitArray *parent_list, /* points giving rise to
                                               lines intersecting
                                               parent hypercube */
                 double *R_parent, /* normalised distances for
                                      each point from parent hypercube */
                 int accum_value, /* accumulated vote weight for
                                     current hypercube. */
                 int level,       /* subdivision level for
                                     current hypercube. */
                 FHT_Path *parent, /* line of ascension towards
                                      root hypercube. */
                 Gan_MemoryStack *memory_stack, /* memory stack structure */

                 /* sums used to incrementally calculate normalised
                    distance (see page 142 of CVGIP FHT paper). */
                 double *sum_ab_ll, double *sum_ab_lh,
                 double *sum_ab_hl, double *sum_ab_hh,

                 int *pt_weight, /* weights for each point */
                 int no_points,  /* number of points (= number
                                    of hyperplanes intersecting
                                    root hypercube). */
                 int *subdivs, /* counts total number of subdivisions performed
                                  by the FHT, roughly proportional to time
                                  taken */

                 /* FHT parameters */
                 double m_range, /* gradient range for root. */
                 double c_range, /* intercept range for root. */
                 double c_root,  /* intercept centre for root. */
                 int    max_level, /* maximum subdivision level. */
                 int    T_thres,   /* vote weight threshold T. */

                 /* returned best-fit line data */
                 double *m_best,     /* gradient */
                 double *c_best,     /* intercept */
                 int      *level_best, /* subdivision level */
                 int      *accum_best, /* vote weight */
                 Gan_BitArray *list_best ) /* selected points have
                                              their bits set in this list */
{
   Gan_BitArray list_ll,  list_lh,  list_hl,  list_hh;
   int          accum_ll, accum_lh, accum_hl, accum_hh;
   double      *R_ll,    *R_lh,    *R_hl,    *R_hh;
   Gan_Bool     subdiv_flag;
   FHT_Path     child;
   int          pt;
   double       Rp2;

   /* allocate normalised distance arrays. */
   R_ll = gan_ms_malloc_array ( memory_stack, double, no_points );
   R_lh = gan_ms_malloc_array ( memory_stack, double, no_points );
   R_hl = gan_ms_malloc_array ( memory_stack, double, no_points );
   R_hh = gan_ms_malloc_array ( memory_stack, double, no_points );
   if ( R_ll == NULL || R_lh == NULL || R_hl == NULL || R_hh == NULL )
   {
      gan_err_register ( "divide_ranges", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   /* initialize accumulated vote weight for four children to zero. */
   accum_ll = accum_lh = accum_hl = accum_hh = 0;

   /* the following bit-lists have each bit set to one if the corresponding
      hyperplane passes through the relevant child hypersphere.
      They are initialised with all bits zero. */
   if ( gan_bit_array_ms_form ( memory_stack, &list_ll, no_points ) == NULL ||
        gan_bit_array_ms_form ( memory_stack, &list_lh, no_points ) == NULL ||
        gan_bit_array_ms_form ( memory_stack, &list_hl, no_points ) == NULL ||
        gan_bit_array_ms_form ( memory_stack, &list_hh, no_points ) == NULL )
   {
      gan_err_register ( "divide_ranges", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   gan_bit_array_fill ( &list_ll, GAN_FALSE );
   gan_bit_array_fill ( &list_lh, GAN_FALSE );
   gan_bit_array_fill ( &list_hl, GAN_FALSE );
   gan_bit_array_fill ( &list_hh, GAN_FALSE );

   /* increment total number of subdivisions. */
   (*subdivs)++;

   /* check intersection of hyperplane with hypersphere for each point and
      each child hypersphere. */
   for ( pt = 0; pt < no_points; pt++ )
      if ( gan_bit_array_get_bit ( parent_list, pt ) ) /* only test children if
                                                          hyperplane intersects
                                                          parent hypersphere */
      {
         /* calculate normalised distances for children incrementally from
            parent normalised distance. */
         Rp2 = R_parent[pt]*2.0;
         R_ll[pt] = Rp2 + sum_ab_ll[pt];
         R_lh[pt] = Rp2 + sum_ab_lh[pt];
         R_hl[pt] = Rp2 + sum_ab_hl[pt];
         R_hh[pt] = Rp2 + sum_ab_hh[pt];

         /* compare normalised distance of each hypercube from the hyperplane
            corresponding to point pt with sqrt(k)/2 = sqrt(2/2 = 1/sqrt(2.
            For the FHT line finder the parameter space dimension is k=2.
            If a normalised distance is < 1/sqrt(2 the corresponding
            vote accumulator is incremented and the corresponding bit in
            the relevant list set to one. */
         if ( fabs ( R_ll[pt] ) < M_SQRT1_2 )
         {
            accum_ll += pt_weight[pt];
            gan_bit_array_set_bit ( &list_ll, pt );
         }

         if ( fabs ( R_lh[pt] ) < M_SQRT1_2 )
         {
            accum_lh += pt_weight[pt];
            gan_bit_array_set_bit ( &list_lh, pt );
         }

         if ( fabs ( R_hl[pt] ) < M_SQRT1_2 )
         {
            accum_hl += pt_weight[pt];
            gan_bit_array_set_bit ( &list_hl, pt );
         }

         if ( fabs ( R_hh[pt] ) < M_SQRT1_2 )
         {
            accum_hh += pt_weight[pt];
            gan_bit_array_set_bit ( &list_hh, pt );
         }
      }

   {
      /* remove this debug block for normal running. */
      double mc, cc;

      calc_centre ( parent, &mc, &cc, m_range, c_range, c_root );
      indent ( level );
      printf ( "m:%f c:%f level:%d ll:%d lh:%d hl:%d hh:%d\n",
               mc, cc, level, accum_ll, accum_lh, accum_hl, accum_hh );
      indent ( level );
      print_path ( parent );
      gan_bit_array_print ( parent_list, level );
   }

   subdiv_flag = GAN_FALSE; /* is set to GAN_TRUE if current hypercube
                               is subdivided. */

   if ( level < max_level ) /* haven't reached maximum subdivision level yet */
   {
      child.parent = parent; /* extend family tree */

      /* Now compare accumulated vote weights for each child with T threshold.
         If any child has a vote > T_thres, recursively subdivide that child.*/
      if  ( accum_ll >= T_thres )
      {
         child.m_cube = 0;
         child.c_cube = 0;
         if ( !divide_ranges ( &list_ll, R_ll, accum_ll, level + 1, &child,
                               memory_stack,
                               sum_ab_ll, sum_ab_lh, sum_ab_hl, sum_ab_hh,
                               pt_weight, no_points, subdivs, m_range, c_range,
                               c_root, max_level, T_thres,
                               m_best, c_best, level_best, accum_best,
                               list_best ) )
         {
            gan_err_register ( "divide_ranges", GAN_ERROR_FAILURE, "" );
            return GAN_FALSE;
         }
         
         subdiv_flag = GAN_TRUE;
      }

      if  ( accum_lh >= T_thres )
      {
         child.m_cube = 0;
         child.c_cube = 1;
         if ( !divide_ranges ( &list_lh, R_lh, accum_lh, level + 1, &child,
                               memory_stack,
                               sum_ab_ll, sum_ab_lh, sum_ab_hl, sum_ab_hh,
                               pt_weight, no_points, subdivs, m_range, c_range,
                               c_root, max_level, T_thres,
                               m_best, c_best, level_best, accum_best,
                               list_best ) )
         {
            gan_err_register ( "divide_ranges", GAN_ERROR_FAILURE, "" );
            return GAN_FALSE;
         }

         subdiv_flag = GAN_TRUE;
      }

      if  ( accum_hl >= T_thres )
      {
         child.m_cube = 1;
         child.c_cube = 0;
         if ( !divide_ranges ( &list_hl, R_hl, accum_hl, level + 1, &child,
                               memory_stack,
                               sum_ab_ll, sum_ab_lh, sum_ab_hl, sum_ab_hh,
                               pt_weight, no_points, subdivs, m_range, c_range,
                               c_root, max_level, T_thres,
                               m_best, c_best, level_best, accum_best,
                               list_best ) )
         {
            gan_err_register ( "divide_ranges", GAN_ERROR_FAILURE, "" );
            return GAN_FALSE;
         }

         subdiv_flag = GAN_TRUE;
      }

      if  ( accum_hh >= T_thres )
      {
         child.m_cube = 1;
         child.c_cube = 1;
         if ( !divide_ranges ( &list_hh, R_hh, accum_hh, level + 1, &child,
                               memory_stack,
                               sum_ab_ll, sum_ab_lh, sum_ab_hl, sum_ab_hh,
                               pt_weight, no_points, subdivs, m_range, c_range,
                               c_root, max_level, T_thres,
                               m_best, c_best, level_best, accum_best,
                               list_best ) )
         {
            gan_err_register ( "divide_ranges", GAN_ERROR_FAILURE, "" );
            return GAN_FALSE;
         }

         subdiv_flag = GAN_TRUE;
      }
   }

   if ( !subdiv_flag ) /* we have stopped subdividing, i.e. this "branch"
                          of the family tree has tapered out. Test against
                          current best fit line, and update if this is
                          a better fit. Criterion for a "better" fit is
                          either a higher subdivision level or an equal
                          subdivision level and higher vote weight. */
      if ( (level > *level_best) ||
           ((level == *level_best) && (accum_value > *accum_best)) )
      {
         /* Current hypercube gives best fit yet */

         /* update subdivision level and vote weight */
         *level_best = level;
         *accum_best = accum_value;

         /* update line parameters to values of (m,c) at the centre of
            the current hypercube. */
         calc_centre ( parent, m_best, c_best, m_range, c_range, c_root );

         if ( list_best != NULL )
            /* update bit-list of selected points. */
            gan_bit_array_copy_q ( parent_list, list_best );
      }

   /* free all data associated with current hypercube before exiting. */
   gan_bit_array_free_va ( &list_hh, &list_hl, &list_lh, &list_ll, NULL );
   gan_ms_free_va ( memory_stack, R_hh, R_hl, R_lh, R_ll, NULL );

   /* success */
   return GAN_TRUE;
}

/**
 * \brief General purpose FHT line finder function.
 * \param x Array of x-coordinates of data points
 * \param y Array of y-coordinates of data points
 * \param weight Array of weights assigned to each point
 * \param no_points Number of data points
 * \param m_range Range of line gradient
 * \param c_range Range of line intercept
 * \param c_root Centre point of intercept for root hypercube
 * \param max_level Maximum subdivision level
 * \param T_thres Threshold T in FHT for deciding whether to subdivide
 * \param memory_stack Pointer to memory stack structure
 * \param m_best Output gradient of best-fit line
 * \param c_best Output intercept of best-fit line
 * \param level_best Output subdivision level reached by FHT
 * \param accum_best Output sum of weights of points with lines intersecting
 *                   final hypersphere
 * \param list_best Output bit-list, with 1's for points involved in best-fit
 *                  result
 *
 * Uses parametrisation
 * \f[
 *    y = m\:x + c
 * \f]
 * where \f$ x,y \f$ are the coordinates of a point on a plane and
 * \f$ (m,c) \f$ are gradient/intercept line parameters.
 * Each point \f$ (x_i,y_i) \f$ can be thought of as defining a line
 * \f[
 *    c_i = y_i - m_i \times x_i
 * \f]
 * in \f$ (m,c) \f$ space, each point on the line in \f$ (m,c) \f$ space
 * representing a line in \f$ (x,y) \f$ space passing through the point
 * \f$ (x_i,y_i) \f$. The FHT starts with a root "hypercube" (in this case
 * a rectangle) in \f$ (m,c) \f$ space defined by
 * \f[
 *    -m_{\rm range}/2 <= m <= m_{\rm range}/2,
 *    c_{\rm root}-c_{\rm range}/2 <= c <= c_{\rm root}+c_{\rm range}/2,
 * \f]
 * and subdivides into 2x2 "child" hypercubes, checking each child to see
 * whether enough \f$ (m_i,c_i) \f$ lines (greater than a threshold) intersect
 * a hypersphere circumscribed around the hypercube. In fact the slightly more
 * general approach here allows a weight to be assigned to each point, and the
 * threshold is applied to the sum of weights of intersecting lines.
 *  
 * This is a depth-first version of the FHT, i.e. the child hypercubes are
 * subdivided exhaustively before trying another child. This minimises memory
 * requirement by limiting it to
 * \f[ 
 *    memory~for~one~subdivision~\times~maximum~subdivision~level.
 * \f]
 */
Gan_Bool
 gan_modified_fht2D ( double *x, double *y, int *weight, int no_points,
                      double m_range, double c_range, double c_root,
                      int max_level, int T_thres,
                      Gan_MemoryStack *memory_stack,

                      /* following arguments return best-fit line details */
                      double *m_best, double *c_best, int *level_best,
                      int *accum_best, Gan_BitArray *list_best )
{
   double *sum_ab_ll, *sum_ab_lh, *sum_ab_hl, *sum_ab_hh;
   double  fid_fac, *start_fid_fac, *R_root, mfid_fac;
   int     pt, root_pts, *pt_weight, subdivs;
   Gan_BitArray start_list, root_list, try_list;

   /* determine which points give rise to hyperplanes intersecting root
      hypercube, set relevant bits in start_list, and initialize an array
      (start_fid_fac) to be used later to set up arrays of 1/2 sum_i a_i*b*i */
   start_fid_fac = gan_ms_malloc_array ( memory_stack, double, no_points );
   if ( start_fid_fac == NULL )
   {
      gan_err_register ( "gan_modified_fht2D", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   if ( gan_bit_array_ms_form ( memory_stack, &start_list, no_points )
        == NULL )
   {
      gan_err_register ( "gan_modified_fht2D", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   gan_bit_array_fill ( &start_list, GAN_FALSE );

   for ( pt = 0, root_pts = 0; pt < no_points; pt++ )
   {
      /* following expression is needed to form (virtual) circumscribing
         hypersphere for root hypercube. */
      fid_fac = sqrt ( gan_sqr(m_range)*gan_sqr(x[pt]) + gan_sqr(c_range) );

      /* now calculate normalised distance R and compare with sqrt(k)/2. */
      if ( fabs((c_root - y[pt])/fid_fac) < M_SQRT1_2 )
      {
         /* hyperplane intersects circumscribing hypersphere of root hypercube,
            so set bit of start_list, store fid_fac expression, and increment
            counter (root_pts). */
         gan_bit_array_set_bit ( &start_list, pt );
         start_fid_fac[pt] = fid_fac;
         root_pts++;
      }
   }

   /* from now on until the call to convert_list only consider points that
      intersect root hypercube. They are compressed together in the bit-lists
      root_list and try_list. This improves efficiency. */

   /* create a hyperplane intersection bit-list for the root hypercube and
      initialise it to all ones. */
   if ( gan_bit_array_ms_form ( memory_stack, &root_list, root_pts ) == NULL )
   {
      gan_err_register ( "gan_modified_fht2D", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   gan_bit_array_fill ( &root_list, GAN_TRUE );

   /* set ab-sums, normalised distances and weights. */
   sum_ab_ll = gan_ms_malloc_array ( memory_stack, double, root_pts );
   sum_ab_lh = gan_ms_malloc_array ( memory_stack, double, root_pts );
   sum_ab_hl = gan_ms_malloc_array ( memory_stack, double, root_pts );
   sum_ab_hh = gan_ms_malloc_array ( memory_stack, double, root_pts );
   R_root = gan_ms_malloc_array ( memory_stack, double, root_pts );
   pt_weight = gan_ms_malloc_array ( memory_stack, int, root_pts );
   if ( sum_ab_ll == NULL || sum_ab_lh == NULL ||
        sum_ab_hl == NULL || sum_ab_hh == NULL ||
        R_root == NULL || pt_weight == NULL )
   {
      gan_err_register ( "gan_modified_fht2D", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }
      
   for ( pt = 0, root_pts = 0; pt < no_points; pt++ )
      if ( gan_bit_array_get_bit ( &start_list, pt ) )
      {
         /* hyperplane intersects root */
         fid_fac = start_fid_fac[pt]*2.0;
         mfid_fac = m_range*x[pt];
         sum_ab_ll[root_pts] = (-mfid_fac-c_range)/fid_fac;
         sum_ab_lh[root_pts] = (-mfid_fac+c_range)/fid_fac;
         sum_ab_hl[root_pts] = (mfid_fac-c_range)/fid_fac;
         sum_ab_hh[root_pts] = (mfid_fac+c_range)/fid_fac;
         R_root[root_pts] = 2.0*(c_root-y[pt])/fid_fac;
         pt_weight[root_pts] = weight[pt];
         root_pts++;
      }

   /* initialise total number of subdivisions performed to zero */
   subdivs = 0;

   /* initialise maximum subdivision level reached to zero */
   *level_best = 0;

   /* try_list will contain the points for the best fit so far:
      initialise it with all zeroes. */
   if ( gan_bit_array_ms_form ( memory_stack, &try_list, root_pts ) == NULL )
   {
      gan_err_register ( "gan_modified_fht2D", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   gan_bit_array_fill ( &try_list, GAN_FALSE );

   /* enter FHT recursive subdivision algorithm: not a Black Hole! */
   if ( !divide_ranges ( &root_list, R_root, 0, 0, NULL, memory_stack,
                         sum_ab_ll, sum_ab_lh, sum_ab_hl, sum_ab_hh,
                         pt_weight, root_pts, &subdivs,
                         m_range, c_range, c_root, max_level, T_thres,
                         m_best, c_best, level_best, accum_best, &try_list ) )
   {
      gan_err_register ( "gan_modified_fht2D", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   /* expand point bit-list for best fit (try_list back to size of
      original point list by padding it with zeroes in the relevant places. */
   if ( list_best != NULL &&
        !gan_bit_array_expand_q ( &try_list, &start_list, list_best ) )
   {
      gan_err_register ( "gan_fast_hough_transform", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   printf ( "subdivisions: %d\n", subdivs );

   gan_bit_array_free ( &try_list );
   gan_ms_free_va ( memory_stack, pt_weight, R_root,
                    sum_ab_hh, sum_ab_hl, sum_ab_lh, sum_ab_ll, NULL );
   gan_bit_array_free_va ( &root_list, &start_list, NULL );
   gan_ms_free ( memory_stack, start_fid_fac );

   /* success */
   return GAN_TRUE;
}

/**
 * \}
 */

/**
 * \}
 */

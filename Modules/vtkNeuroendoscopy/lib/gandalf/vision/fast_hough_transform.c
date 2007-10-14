/**
 * File:          $RCSfile: fast_hough_transform.c,v $
 * Module:        Fast Hough Transform implentation
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

#include <gandalf/vision/fast_hough_transform.h>
#include <math.h>
#include <stdio.h>
#include <gandalf/common/memory_stack.h>
#include <gandalf/common/allocate.h>
#include <gandalf/common/numerics.h>
#include <gandalf/common/misc_error.h>

/**
 * \addtogroup Vision
 * \{
 */

/**
 * \defgroup HoughTransform Hough Transform
 * \{
 */

/* program to perform Fast-Hough-Transform line finder */

/* FHT_Path: structure for recursively specifying hypercube path back to
             root hypercube */
typedef struct FHT_Path
{
   int *s; /* each element is either 0 or 1, specify which
              child out of the 2*2*..*2 set of child hypercubes this one is. */
   struct FHT_Path *parent; /* if NULL then this is the root hypercube. */
} FHT_Path;

#define DEBUG 1

#if DEBUG
/* Indents a line with the given number of spaces.
 */
static void indent ( int level )
{
   while ( level-- > 0 ) printf ( " " );
}

/*
 * Prints the "family tree" of a hypercube specified by the node argument.
 * For each "generation", one of (0,0,...,0), (0,0,...,1), ..., (1,1,...,1) is
 * printed, depending the parent/child relationship. The relationship to the
 * root hypercube is printed first.
 */
static void
 print_path ( int       k,         /* parameter space dimension */
              FHT_Path *node )     /* hypercube line of descent */

{
   int       level;
   FHT_Path *ptr;

   printf ( "path ( " );
   for ( ptr = node, level = 0; ptr != NULL; ptr = ptr->parent, level++ )
      ;

   if ( level > 0 )
   {
      int   i;
      char *string;

      string = gan_malloc_array ( char, level*(k+3)+1 );
      string[level*(k+3)] = '\0';

      for ( ptr = node, level--; ptr != NULL; ptr = ptr->parent, level-- )
      {
         string[level*(k+3)] = '(';
         for ( i = 0; i < k; i++ )
            if ( ptr->s[i] ) string[level*(k+3)+i+1] = '1';
            else             string[level*(k+3)+i+1] = '0';

         string[level*(k+3)+k+1] = ')';
         string[level*(k+3)+k+2] = ' ';
      }

      printf ( "%s", string );
      free ( (void *) string );
   }

   printf ( ")\n" );
}
#endif

/*
 * Finds the parameter values at the centre of a hypercube.
 */
static Gan_Bool
 calc_centre ( int       k,    /* dimensions of parameter space */
               FHT_Path *node, /* hypercube line of descent */
               double *X,    /* hypercube parameter values */

               /* definition of root hypercube: */
               double *S0,  /* initial range of parameters */
               double *X0 ) /* centre of root hypercube */
{
   double *X_add = gan_malloc_array ( double, 2*k );
   int     i;

   if ( X_add == NULL )
   {
      gan_err_flush_trace();
      gan_err_register_with_number ( "calc_centre", GAN_ERROR_MALLOC_FAILED, "", 2*k*sizeof(double) );
      return GAN_FALSE;
   }
   
   for ( i = 0; i < k; i++ )
   {
      X[i] = 0.0;
      X_add[2*i] = -S0[i]*0.5; X_add[2*i+1] = -X_add[2*i];
   }

   for ( ; node != NULL; node = node->parent )
      for ( i = 0; i < k; i++ )
         X[i] = X[i]*0.5 + X_add[2*i+node->s[i]];

   for ( i = 0; i < k; i++ ) X[i] += X0[i];

   free ( X_add );
   return GAN_TRUE;
}

/*
 * Performs FHT recursive subdivision.
 */
static Gan_Bool
 divide_ranges ( int k, /* dimensions of parameter space */
                 double sqrt_k_over_2, /* hypercube/sphere distance
                                          threshold sqrt(k)/2 */
                 Gan_BitArray *parent_list, /* points giving rise to
                                               lines intersecting
                                               parent hypercube */
                 double *R_parent, /* normalised distances for each
                                      point from parent h'cube */
                 int accum_value, /* accumulated vote weight for
                                     current hypercube. */
                 int level,       /* subdivision level for
                                     current hypercube. */
                 FHT_Path *parent, /* line of ascension towards
                                      root hypercube. */
                 Gan_MemoryStack *memory_stack, /* memory stack structure */

                 /* sums used to incrementally calculate normalised
                    distance (see page 142 of CVGIP FHT paper). */
                 double **sum_ab,

                 int *pt_weight, /* weights for each point */
                 int  no_points, /* number of points (= number
                                    of hyperplanes intersecting
                                    root hypercube). */

                 /* definition of root hypercube: */
                 double *S0, /* initial range of parameters */
                 double *X0, /* centre of root hypercube */

                 /* FHT parameters */
                 int max_level, /* maximum subdivision level. */
                 int T_thres,   /* vote weight threshold T. */

                 /* returned best-fit parameters */
                 double  *X_best,     /* gradient */
                 int     *level_best, /* subdivision level */
                 int     *accum_best, /* vote weight */
                 Gan_BitArray *list_best, /* selected points have their bits
                                             set in this list */
                 int     *subdivs ) /* counts total number of subdivisions
                                       performed by the FHT, roughly
                                       proportional to the time taken */
{
   Gan_BitArray *list;
   int          *accum;
   double      **R, Rp2;
   Gan_Bool      subdiv_flag;
   FHT_Path      child;
   int           i, j, l;

   /* allocate normalised distance arrays. */
   R = gan_ms_malloc_array ( memory_stack, double *, 1<<k );
   for ( j = 0; j < 1<<k; j++ )
      R[j] = gan_ms_malloc_array ( memory_stack, double, no_points );

   /* initialize accumulated vote weight for four children to zero. */
   accum = gan_ms_malloc_array ( memory_stack, int, 1<<k );
   for ( j = 0; j < 1<<k; j++ ) accum[j] = 0;

   /* the following bit-lists have each bit set to one if the corresponding
      hyperplane passes through the relevant child hypersphere.
      They are initialised with all bits zero. */
   list = gan_ms_malloc_array ( memory_stack, Gan_BitArray, 1<<k );
   if ( list == NULL )
   {
      gan_err_register ( "divide_ranges", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   for ( j = 0; j < 1<<k; j++ )
   {
      if ( gan_bit_array_ms_form ( memory_stack, &list[j], no_points )
           == NULL )
      {
         gan_err_register ( "divide_ranges", GAN_ERROR_FAILURE, "" );
         return GAN_FALSE;
      }

      gan_bit_array_fill ( &list[j], GAN_FALSE );
   }
   
   /* increment total number of subdivisions. */
   (*subdivs)++;

   /* check intersection of hyperplane with hypersphere for each point and
      each child hypersphere. */
   for ( j = 0; j < no_points; j++ )
      /* only test children if hyperplane intersects parent hypersphere */
      if ( gan_bit_array_get_bit ( parent_list, j ) ) 
      {
         /* calculate normalised distances for children incrementally from
            parent normalised distance, and compare the
            normalised distance of each hypercube from the hyperplane
            corresponding to point j with sqrt(k)/2 = sqrt(2/2 = 1/sqrt(2.
            For the FHT line finder the parameter space dimension is k=2.
            If a normalised distance is < 1/sqrt(2 the corresponding
            vote accumulator is incremented and the corresponding bit in
            the relevant list set to one. */
         Rp2 = R_parent[j]*2.0;
         for ( l = 0; l < 1<<k; l++ )
            if ( fabs(R[l][j] = Rp2 + sum_ab[l][j]) < sqrt_k_over_2 )
            {
               accum[l] += pt_weight[j];
               gan_bit_array_set_bit ( &list[l], j );
            }
      }

#if DEBUG
   { /* remove this debug block for normal running. */
      double *X = gan_ms_malloc_array ( memory_stack, double, k );

      calc_centre ( k, parent, X, S0, X0 );
      indent ( level );
      for ( i = 0; i < k; i++ ) printf ( "X%d:%f ", i, X[i] );

      printf ( "level:%d\n", level );
      indent ( level );
      for ( l = 0; l < 1<<k; l++ )
      {
         for ( i = 0; i < k; i++ )
            if ( l & (1<<i) ) printf ( "h" );
            else              printf ( "l" );

         printf ( ":%d ", accum[l] );
      }

      printf ( "\n" );
      indent ( level );
      print_path ( k, parent );
      gan_bit_array_print ( parent_list, level );
      gan_ms_free ( memory_stack, X );
   }
#endif

   subdiv_flag = GAN_FALSE; /* is set to GAN_TRUE if current hypercube is
                               subdivided. */

   if ( level < max_level ) /* haven't reached maximum subdivision
                               level yet */
   {
      child.parent = parent; /* extend family tree */
      child.s = gan_ms_malloc_array ( memory_stack, int, k );

      /* Now compare accumulated vote weights for each child with T threshold.
       * If any child has a vote > T_thres, recursively subdivide that child.
       */
      for ( l = 0; l < 1<<k; l++ )
         if ( accum[l] >= T_thres )
         {
            for ( i = 0; i < k; i++ )
               if ( l & (1<<i) ) child.s[i] = 1;
               else              child.s[i] = 0;

            divide_ranges ( k, sqrt_k_over_2, &list[l], R[l], accum[l],
                            level+1, &child, memory_stack, sum_ab, pt_weight,
                            no_points, S0, X0, max_level, T_thres,
                            X_best, level_best, accum_best, list_best,
                            subdivs );
            subdiv_flag = GAN_TRUE;
         }

      gan_ms_free ( memory_stack, child.s );
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
         calc_centre ( k, parent, X_best, S0, X0 );

         /* update bit-list of selected points. */
         gan_bit_array_copy_q ( parent_list, list_best );
      }

   /* free all data associated with current hypercube before exiting. */
   for ( l = (1<<k) - 1; l >= 0; l-- ) gan_bit_array_free ( &list[l] );
   gan_ms_free_va ( memory_stack, list, accum, NULL );
   for ( l = (1<<k) - 1; l >= 0; l-- ) gan_ms_free ( memory_stack, R[l] );
   gan_ms_free ( memory_stack, R );

   /* success */
   return GAN_TRUE;
}

#define gel(M,i,j) gan_mat_get_el(M,i,j)

/**
 * \brief General purpose Fast Hough Transform (FHT) function.
 * \param k Dimensionality of parameter space
 * \param a Feature space (k+1)-vectors arranged as a matrix
 * \param weight Weights assigned to each feature vector
 * \param no_points Number of data points
 * \param S0 Half-range of parameters
 * \param X0 Centre of parameter ranges
 * \param max_level Maximum subdivision level
 * \param T_thres Threshold \a T in FHT for deciding whether subdivide or not
 * \param memory_stack Pointer to memory stack structure
 * \param X_best Output best-fit parameters
 * \param level_best Output subdivision level reached by FHT
 * \param accum_best Output sum of weights of points with lines intersecting
 *                   final hypersphere
 * \param list_best Output bit array bit-list, with 1's for points
 *                  involved in best-fit result
 * \param subdivs Output total number of subdivisions
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * For each feature point j the relationship between feature space
 * uses parametrisation
 * \f[
 *    a[j][0]*X[0] + ... + a[j][k-1]*X[k-1] + a[j][k] = 0
 * \f]
 * (not normalised) where \a X[i] are the parameters and \a k is the
 * dimensionality of parameter space. The starting parameter half-ranges
 * are given by the \a S0 vector, and the parameter origin by the \a X0 vector.
 * \a S0 and \a X0 thus define the root hypercube:
 * \f[
 *    X0[i] - S0[i] <= X[i] <= X0[i] + S0[i].
 * \f]
 * This is a depth-first version of the FHT, i.e. the child hypercubes are
 * subdivided exhaustively before trying another child. This minimises memory
 * requirement by limiting it to
 * \f[
 *   memory~for~one~subdivision~\times~maximum~subdivision~level.
 * \f]
 *
 * \a a contains feature space (k+1)-vectors arranged as a \a no_points by
 * \a k+1 matrix.
 */
Gan_Bool
 gan_fast_hough_transform ( int k, Gan_Matrix *a, int *weight,
                            int no_points, double *S0, double *X0,
                            int max_level, int T_thres,
                            Gan_MemoryStack *memory_stack,

                            /* following arguments return best-fit parameter
                               details */
                            double *X_best, int *level_best,
                            int *accum_best, Gan_BitArray *list_best,
                            int *subdivs )
{
   double **sum_ab, sqrt_k_over_2, sum;
   double   norm_fac, *start_norm_fac, norm_dist, *start_norm_dist;
   double  *R_root;
   int      root_pts, *pt_weight, i, j, l;
   Gan_BitArray start_list, root_list, try_list;

   if ( k < 1 || k > 32 || no_points <= 0 || max_level <= 0 || T_thres <= 0 )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_fast_hough_transform",
                         GAN_ERROR_ILLEGAL_ARGUMENT, "" );
      return GAN_FALSE;
   }

   /* determine which points give rise to hyperplanes intersecting root
      hypercube, set relevant bits in start_list, and initialize an array
      (start_norm_fac) to be used later to set up arrays of
      1/2 sum_i a_i*b_i */
   start_norm_fac  = gan_ms_malloc_array ( memory_stack, double, no_points );
   start_norm_dist = gan_ms_malloc_array ( memory_stack, double, no_points );
   if ( start_norm_fac == NULL || start_norm_dist == NULL )
   {
      gan_err_register ( "gan_fast_hough_transform", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   if ( gan_bit_array_ms_form ( memory_stack, &start_list, no_points )
        == NULL )
   {
      gan_err_register ( "gan_fast_hough_transform", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   gan_bit_array_fill ( &start_list, GAN_FALSE );

   sqrt_k_over_2 = sqrt((double) k)/2.0;

   for ( j = 0, root_pts = 0; j < no_points; j++ )
   {
      /* following expression is a normalising factor needed to form (virtual)
         circumscribing hypersphere for root hypercube. */
      for ( i = 0, norm_fac = 0.0; i < k; i++ )
         norm_fac += gan_sqr_d(S0[i]) * gan_sqr_d(gel(a,j,i));

      norm_fac = 2.0*sqrt(norm_fac);

      /* now calculate normalised distance R and compare with sqrt(k)/2. */
      for ( i = 0, norm_dist = gel(a,j,k); i < k; i++ )
         norm_dist += gel(a,j,i)*X0[i];

      norm_dist /= norm_fac;

      if ( fabs(norm_dist) < sqrt_k_over_2 )
      {
         /* hyperplane intersects circumscribing hypersphere of root hypercube,
            so set bit of start_list, store norm_fac expression, and increment
            counter (root_pts). */
         gan_bit_array_set_bit ( &start_list, j );
         start_norm_fac[j]  = norm_fac;
         start_norm_dist[j] = norm_dist;
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
      gan_err_register ( "gan_fast_hough_transform", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   gan_bit_array_fill ( &root_list, GAN_TRUE );

   /* set ab-sums, normalised distances and weights. */
   sum_ab = gan_ms_malloc_array ( memory_stack, double *, 1<<k );
   for ( l = 0; l < 1<<k; l++ )
   {
      sum_ab[l] = gan_ms_malloc_array ( memory_stack, double, root_pts );
      if ( sum_ab[l] == NULL )
      {
         gan_err_register ( "gan_fast_hough_transform", GAN_ERROR_FAILURE,
                            "" );
         return GAN_FALSE;
      }
   }

   R_root    = gan_ms_malloc_array ( memory_stack, double, root_pts );
   pt_weight = gan_ms_malloc_array ( memory_stack, int,    root_pts );
   if ( R_root == NULL || pt_weight == NULL )
   {
      gan_err_register ( "gan_fast_hough_transform", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   for ( j = 0, root_pts = 0; j < no_points; j++ )
      if ( gan_bit_array_get_bit ( &start_list, j ) ) /* hyperplane intersects
                                                         root */
      {
         norm_fac = start_norm_fac[j];
         for ( l = 0; l < 1<<k; l++ )
         {
            for ( i = 0, sum = 0.0; i < k; i++ )
               if ( l & (1<<i) ) sum += gel(a,j,i)*S0[i];
               else              sum -= gel(a,j,i)*S0[i];

            sum_ab[l][root_pts] = sum/norm_fac;
         }

         R_root[root_pts] = start_norm_dist[j];
         pt_weight[root_pts] = weight[j];
         root_pts++;
      }

   /* initialise total number of subdivisions performed to zero */
   *subdivs = 0;

   /* initialise maximum subdivision level reached to zero */
   *level_best = 0;

   /* try_list will contain the points for the best fit so far:
      initialise it with all zeroes. */
   if ( gan_bit_array_ms_form ( memory_stack, &try_list, root_pts ) == NULL )
   {
      gan_err_register ( "gan_fast_hough_transform", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   gan_bit_array_fill ( &try_list, GAN_FALSE );

   /* enter FHT recursive subdivision algorithm: not a Black Hole! */
   if ( !divide_ranges ( k, sqrt_k_over_2, &root_list, R_root, 0, 0, NULL,
                         memory_stack, sum_ab, pt_weight, root_pts, S0, X0,
                         max_level, T_thres,
                         X_best, level_best, accum_best, &try_list, subdivs ) )
   {
      gan_err_register ( "gan_fast_hough_transform", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   /* expand point bit-list for best fit (try_list) back to size of
      original point list by padding it with zeroes in the relevant
      places */
   if ( list_best != NULL &&
        !gan_bit_array_expand_q ( &try_list, &start_list, list_best ) )
   {
      gan_err_register ( "gan_fast_hough_transform", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   gan_bit_array_free ( &try_list );
   gan_ms_free_va ( memory_stack, pt_weight, R_root, NULL );
   for ( l = (1<<k)-1; l >= 0; l-- ) gan_ms_free ( memory_stack, sum_ab[l] );
   gan_ms_free ( memory_stack, sum_ab );
   gan_bit_array_free_va ( &root_list, &start_list, NULL );
   gan_ms_free_va ( memory_stack, start_norm_dist, start_norm_fac, NULL );
   gan_memory_stack_clean ( memory_stack );

   /* success */
   return GAN_TRUE;
}

/**
 * \}
 */

/**
 * \}
 */

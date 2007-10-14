/**
 * File:          $RCSfile: hough_transform_test.c,v $
 * Module:        Hough transform test program
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

#include <gandalf/TestFramework/cUnit.h>
#include <gandalf/vision/hough_transform_test.h>

#include <gandalf/vision/fast_hough_transform.h>
#include <gandalf/vision/modified_fht2D.h>
#include <gandalf/common/numerics.h>

#define gel(M,i,j)   gan_mat_get_el(M,i,j)
#define sel(M,i,j,v) gan_mat_set_el(M,i,j,v)

/* application specific code starts here. */
#define FHTP_NO_PARAMS        3   /* plane parameters x-gradient/y-gradient/
                     intercept. */
#define FHTP_NO_POINTS      100   /* number of data points. */
#define FHTP_NO_PLANE_POINTS 50   /* number of points on the plane. */
#define FHTP_A_TRUE          0.3 /* x-gradient. */
#define FHTP_B_TRUE          0.7 /* y-gradient. */
#define FHTP_C_TRUE          4.0 /* plane intercept. */
#define FHTP_A_RANGE         2.0 /* range of x-gradients for root hypercube. */
#define FHTP_B_RANGE         2.0 /* range of y-gradients for root hypercube. */
#define FHTP_C_RANGE        20.0 /* range of intercepts for root hypercube. */
#define FHTP_C_ROOT          0.0 /* intercept at centre of root hypercube. */

static Gan_Bool FHT_plane_test(void)
{
   Gan_Matrix *a;             /* 3D data points */
   int  weight[FHTP_NO_POINTS]; /* weights assigned to each point */
   int  j;
   double *S0, *X0;             /* half-range/centre of root hypercube */
   Gan_MemoryStack memory_stack;

   /* best-fit parameters */
   double       *X_best;                 /* gradient/intercept */
   int           level_best, accum_best; /* subdivision level/vote weight */
   Gan_BitArray *list_best;              /* bit-list with ones for selected
                                            points */
   int subdivs; /* total number of hypercube subdivisions performed */

   cu_assert ( gan_memory_stack_form ( &memory_stack, 50, 40000 ) );

   /* select points on the plane at random. */
   a = gan_mat_alloc ( FHTP_NO_POINTS, FHTP_NO_PARAMS+1 );
   cu_assert ( a != NULL );
   for ( j = 0; j < FHTP_NO_PLANE_POINTS; j++ )
   {
      sel(a,j,0, gan_random_01()*20.0 - 10.0); /* x */
      sel(a,j,1, gan_random_01()*20.0 - 10.0); /* y */
      sel(a,j,2, 1.0);

      /* -z */
      sel(a,j,3, -(FHTP_A_TRUE*gel(a,j,0) + FHTP_B_TRUE*gel(a,j,1)
                   + FHTP_C_TRUE));
      weight[j] = 1;
   }

   /* select noise points at random. */
   for ( j = FHTP_NO_PLANE_POINTS; j < FHTP_NO_POINTS; j++ )
   {
      sel(a,j,0, gan_random_01()*20.0 - 10.0); /* x */
      sel(a,j,1, gan_random_01()*20.0 - 10.0); /* y */
      sel(a,j,2, 1.0);
      sel(a,j,3, -gan_random_01()*20.0 - 10.0); /* z */
      weight[j] = 1;
   }

   S0     = gan_malloc_array ( double, FHTP_NO_PARAMS );
   X0     = gan_malloc_array ( double, FHTP_NO_PARAMS );
   X_best = gan_malloc_array ( double, FHTP_NO_PARAMS );
   list_best = gan_bit_array_alloc ( FHTP_NO_POINTS );
   cu_assert ( S0 != NULL && X0 != NULL && X_best != NULL &&
               list_best != NULL );

   S0[0] = 0.5*FHTP_A_RANGE; X0[0] = 0.0;
   S0[1] = 0.5*FHTP_B_RANGE; X0[1] = 0.0;
   S0[2] = 0.5*FHTP_C_RANGE; X0[2] = FHTP_C_ROOT;

   /* do plane-fitting using FHT algorithm. */
   cu_assert ( gan_fast_hough_transform ( FHTP_NO_PARAMS, a, weight,
                                          FHTP_NO_POINTS, S0, X0, 10, 50,
                                          &memory_stack,
                                          X_best, &level_best, &accum_best,
                                          list_best, &subdivs ) );

   /* print results. */
   printf ( "a=%f b=%f c=%f level=%d accum=%d subdivs=%d\n",
            X_best[0], X_best[1], X_best[2], level_best, accum_best, subdivs );
   gan_bit_array_print ( list_best, 0 );

   gan_memory_stack_free ( &memory_stack );
   gan_bit_array_free ( list_best );
   gan_free_va ( X_best, X0, S0, NULL );
   gan_mat_free ( a );
   /* all data should now be freed */

   return GAN_TRUE;
}

/* FHT line-fitter code */
#define FHTL_NO_POINTS      100   /* number of data points. */
#define FHTL_NO_LINE_POINTS  50   /* number of points on the line. */
#define FHTL_M_TRUE           0.3 /* line gradient. */
#define FHTL_C_TRUE           4.0 /* line intercept. */
#define FHTL_M_RANGE          2.0 /* range of gradients for root hypercube. */
#define FHTL_C_RANGE         20.0 /* range of intercepts for root hypercube. */
#define FHTL_C_ROOT           0.0 /* intercept at centre of root hypercube. */

static Gan_Bool
 FHT_line_test(void)
{
   double x[FHTL_NO_POINTS], y[FHTL_NO_POINTS]; /* 2D data points */
   int    weight[FHTL_NO_POINTS]; /* weights assigned to each point */
   int    pt;
   Gan_MemoryStack memory_stack;

   /* best-fit parameters */
   double m_best, c_best;         /* gradient/intercept */
   int    level_best, accum_best; /* subdivision level/vote weight */
   Gan_BitArray *list_best;       /* bit-list with ones for selected points */

   cu_assert ( gan_memory_stack_form ( &memory_stack, 50, 40000 ) );

   /* select points on the line at random. */
   for ( pt = 0; pt < FHTL_NO_LINE_POINTS; pt++ )
   {
      x[pt] = gan_random_01()*20.0 - 10.0;
      y[pt] = FHTL_M_TRUE*x[pt] + FHTL_C_TRUE;
      weight[pt] = 1;
   }

   /* select noise points at random. */
   for ( pt = FHTL_NO_LINE_POINTS; pt < FHTL_NO_POINTS; pt++ )
   {
      x[pt] = gan_random_01()*20.0 - 10.0;
      y[pt] = gan_random_01()*20.0 - 10.0;
      weight[pt] = 1;
   }

   /* allocate bit array for result */
   list_best = gan_bit_array_alloc ( FHTP_NO_POINTS );

   /* do line-fitting using FHT algorithm. */
   cu_assert ( gan_modified_fht2D ( x, y, weight, FHTL_NO_POINTS, FHTL_M_RANGE,
                                    FHTL_C_RANGE, FHTL_C_ROOT, 8, 50,
                                    &memory_stack,
                                    &m_best, &c_best, &level_best, &accum_best,
                                    list_best ) );

   /* print results. */
   printf ( "m=%f c=%f level=%d accum=%d\n",
            m_best, c_best, level_best, accum_best );
   gan_bit_array_print ( list_best, 0 );

   gan_memory_stack_free ( &memory_stack );
   gan_bit_array_free ( list_best );

   /* all data should now be freed */
   return GAN_TRUE;
}

static Gan_Bool setup_test(void)
{
   /* set default Gandalf error handler without trace handling */
   printf("\nSetup for hough_transform_test completed!\n\n");
   return GAN_TRUE;
}

static Gan_Bool teardown_test(void)
{
   printf("\nTeardown for hough_transform_test completed!\n\n");
   return GAN_TRUE;
}

/* Tests the Hough Transform functions */
static Gan_Bool run_test(void)
{
#ifndef NDEBUG
   gan_debug = GAN_TRUE;
#endif
   cu_assert ( FHT_plane_test() );
   cu_assert ( FHT_line_test() );

   /* success */
   return GAN_TRUE;
}

#ifdef HOUGH_TRANSFORM_TEST_MAIN

int main ( int argc, char *argv[] )
{
   /* turn on error tracing */
   gan_err_set_trace ( GAN_ERR_TRACE_ON );

   setup_test();
   if ( run_test() )
      printf ( "Tests ran successfully!\n" );
   else
   {
      printf ( "At least one test failed\n" );
      printf ( "Gandalf errors:\n" );
      gan_err_default_reporter();
   }
   
   teardown_test();
   gan_heap_report(NULL);
   return 0;
}

#else

/* This function registers the unit tests to a cUnit_test_suite. */
cUnit_test_suite *hough_transform_test_build_suite(void)
{
   cUnit_test_suite *sp;

   /* Get a new test session */
   sp = cUnit_new_test_suite("hough_transform_test suite");

   cUnit_add_test(sp, "hough_transform_test", run_test);

   /* register a setup and teardown on the test suite 'hough_transform_test' */
   if (cUnit_register_setup(sp, setup_test) != GAN_TRUE)
      printf("Error while setting up test suite hough_transform_test");

   if (cUnit_register_teardown(sp, teardown_test) != GAN_TRUE)
      printf("Error while tearing down test suite hough_transform_test");

   return( sp );
}

#endif /* #ifdef HOUGH_TRANSFORM_TEST_MAIN */

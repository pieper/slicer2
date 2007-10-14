/**
 * File:          $RCSfile: matrixf_eigen_test.c,v $
 * Module:        Fixed size matrix eigensystem test program
 * Part of:       Gandalf Library
 *
 * Revision:      $Revision: 1.1.2.1 $
 * Last edited:   $Date: 2007/10/14 03:16:23 $
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
#include <gandalf/linalg/matrixf_eigen_test.h>
#include <gandalf/linalg/3x3matrix_eigsym.h>
#include <gandalf/linalg/4x4matrix_eigsym.h>
#include <gandalf/vision/rotate3D.h>

static Gan_Bool setup_test(void)
{
   printf("\nSetup for matrixf_eigen_test completed!\n\n");
   return GAN_TRUE;
}

static Gan_Bool teardown_test(void)
{
   printf("\nTeardown for matrixf_eigen_test completed!\n\n");
   return GAN_TRUE;
}

static Gan_Bool
 eigsym3x3_test(void)
{
   Gan_Quaternion q;
   Gan_Rot3D Rot;
   Gan_SquMatrix33 sm33S, sm33Sp;
   Gan_Matrix33 m33Z;
   Gan_Vector3 v3W;

   /* fill quaternion */
   (void)gan_quat_fill_q ( &q, 1.0, -2.0, 3.0, -4.0 );
   q = gan_quat_unit_s(&q);

   /* fill rotation matrix */
   Rot = gan_rot3D_from_quaternion_s ( &q, GAN_ROT3D_MATRIX );

   /* build symmetric matrix S */
   (void)gan_symmat33_fill_q ( &sm33S,
                               1.0,
                               0.0, 2.0,
                               0.0, 0.0, 500.0 );
   sm33S = gan_symmat33_lrmultm33T_s ( &sm33S, &Rot.data.R, NULL );

   /* perform eigendecomposition */
   cu_assert ( gan_symmat33_eigen ( &sm33S, &v3W, &m33Z ) );

   /* check result */
   (void)gan_symmat33_fill_q ( &sm33Sp,
                               v3W.x,
                               0.0, v3W.y,
                               0.0, 0.0, v3W.z );
   sm33Sp = gan_symmat33_lrmultm33_s ( &sm33Sp, &m33Z, NULL );
   (void)gan_symmat33_decrement ( &sm33Sp, &sm33S );
   cu_assert ( gan_symmat33_Fnorm_q(&sm33Sp) < 0.0001 );

   /* success */
   return GAN_TRUE;
}

static Gan_Bool
 eigsym4x4_test(void)
{
   Gan_SquMatrix44 sm44S, sm44Sp;
   Gan_Matrix44 m44R, m44Z;
   Gan_Vector4 v4W;
   double dA;

   /* build diagonal matrix of eigenvalues */
   (void)gan_symmat44_fill_q ( &sm44S,
                               1.0,
                               0.0, 2.0,
                               0.0, 0.0, 500.0,
                               0.0, 0.0, 0.0, 10.0 );

   /* build orthogonal matrix R */
   (void)gan_mat44_ident_q ( &m44R );
   dA = 1.0;
   (void)gan_mat44_fill_q ( &m44Z, cos(dA), -sin(dA), 0.0, 0.0,
                                   sin(dA),  cos(dA), 0.0, 0.0,
                                       0.0,      0.0, 1.0, 0.0,
                                       0.0,      0.0, 0.0, 1.0 );
   m44R = gan_mat44_rmultm44_s ( &m44R, &m44Z );
   dA = -2.0;
   (void)gan_mat44_fill_q ( &m44Z, 1.0,     0.0,      0.0, 0.0,
                                   0.0, cos(dA), -sin(dA), 0.0,
                                   0.0, sin(dA),  cos(dA), 0.0,
                                   0.0,     0.0,      0.0, 1.0 );
   m44R = gan_mat44_rmultm44_s ( &m44R, &m44Z );
   dA = 3.0;
   (void)gan_mat44_fill_q ( &m44Z, 1.0, 0.0,     0.0,      0.0,
                                   0.0, 1.0,     0.0,      0.0,
                                   0.0, 0.0, cos(dA), -sin(dA),
                                   0.0, 0.0, sin(dA),  cos(dA) );
   m44R = gan_mat44_rmultm44_s ( &m44R, &m44Z );
   dA = 5.0;
   (void)gan_mat44_fill_q ( &m44Z, cos(dA), 0.0, 0.0, -sin(dA),
                                       0.0, 1.0, 0.0,      0.0,
                                       0.0, 0.0, 1.0,      0.0,
                                   sin(dA), 0.0, 0.0,  cos(dA) );
   m44R = gan_mat44_rmultm44_s ( &m44R, &m44Z );

   /* build symmetric matrix S */
   sm44S = gan_symmat44_lrmultm44T_s ( &sm44S, &m44R, NULL );

   /* perform eigendecomposition */
   cu_assert ( gan_symmat44_eigen ( &sm44S, &v4W, &m44Z ) );

   /* check result */
   (void)gan_symmat44_fill_q ( &sm44Sp,
                               v4W.x,
                               0.0, v4W.y,
                               0.0, 0.0, v4W.z,
                               0.0, 0.0, 0.0, v4W.w );
   sm44Sp = gan_symmat44_lrmultm44_s ( &sm44Sp, &m44Z, NULL );
   (void)gan_symmat44_decrement ( &sm44Sp, &sm44S );
   cu_assert ( gan_symmat44_Fnorm_q(&sm44Sp) < 0.0001 );

   /* success */
   return GAN_TRUE;
}

/* Tests the fixed size matrix eigensystem functions */
static Gan_Bool run_test(void)
{
   cu_assert ( eigsym3x3_test() );
   cu_assert ( eigsym4x4_test() );

   /* success */
   return GAN_TRUE;
}

#ifdef MATRIXF_EIGEN_TEST_MAIN

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
cUnit_test_suite *matrixf_eigen_test_build_suite(void)
{
   cUnit_test_suite *sp;

   /* Get a new test session */
   sp = cUnit_new_test_suite("matrixf_eigen_test suite");

   cUnit_add_test(sp, "matrixf_eigen_test", run_test);

   /* register a setup and teardown on the test suite 'matrixf_eigen_test' */
   if (cUnit_register_setup(sp, setup_test) != GAN_TRUE)
      printf("Error while setting up test suite matrixf_eigen_test");

   if (cUnit_register_teardown(sp, teardown_test) != GAN_TRUE)
      printf("Error while tearing down test suite matrixf_eigen_test");

   return( sp );
}

#endif /* #ifdef MATRIXF_EIGEN_TEST_MAIN */

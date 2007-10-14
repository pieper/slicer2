/**
 * File:          $RCSfile: essential.c,v $
 * Module:        Compute the essential matrix from point matches in two frames
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

#include <math.h>
#include <gandalf/vision/essential.h>
#include <gandalf/vision/camera.h>
#include <gandalf/common/allocate.h>
#include <gandalf/common/misc_error.h>
#include <gandalf/linalg/mat_symmetric.h>
#include <gandalf/linalg/matvec_clapack.h>
#include <gandalf/linalg/3x3matrix_svd.h>

/**
 * \addtogroup Vision
 * \{
 */

/**
 * \addtogroup GeomFitting
 * \{
 */

#define gel(M,i,j)   gan_mat_get_el(M,i,j)
#define sel(M,i,j,v) gan_mat_set_el(M,i,j,v)

/**
 * \brief Computes the essential matrix from point matches.
 * \param p1 The array of image points in image 1
 * \param p2 The array of image points in image 2
 * \param n The number of point matches
 * \param cal1 Camera for image 1
 * \param cal2 Camera for image 2
 * \param sme An eigensystem structure to use or \c NULL
 * \param euc The relative pose between the two cameras
 *
 * Applies the Longuet-Higgins eight-point algorithm to compute the essential
 * matrix for images 1 & 2. This is converted into the rotation & translation
 * (up to scale) between the cameras, which are written into the #euc
 * structure. If the #sme structure to compute eigenvectors \& eigenvalues is
 * passed as NULL, a suitable structure is allocated and freed inside the
 * function.
 *
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 * \warning The rank-2 constraint on E is not imposed.
 * \sa gan_fundamental_matrix_fit().
 */
Gan_Bool
 gan_essential_matrix_fit ( Gan_Vector3 *p1, Gan_Vector3 *p2, int n,
                            Gan_Camera *cal1, Gan_Camera *cal2,
                            Gan_SymMatEigenStruct *sme,
                            Gan_Euclid3D *euc )
{
   int i;
   Gan_Vector3 e2;
   Gan_Quaternion q;
   Gan_Matrix33 E;
   Gan_Rot3D rot;
   Gan_Matrix *Z;
   Gan_Bool sme_alloc = GAN_FALSE;

   /* SVD matrices */
   Gan_Vector3  S;
   Gan_Matrix33 U, VT;

   if ( n < 8 )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_essential_matrix_fit", GAN_ERROR_FAILURE,
                         "not enough points" );
      return GAN_FALSE;
   }

   /* allocate sme if necessary */
   if ( sme == NULL )
   {
      sme = gan_symeigen_alloc ( 9 );
      if ( sme == NULL )
      {
         gan_err_register ( "gan_essential_matrix_fit", GAN_ERROR_FAILURE,
                            "" );
         return GAN_FALSE;
      }

      sme_alloc = GAN_TRUE;
   }

   /* perform linear computation of E */
   gan_symeigen_reset ( sme, 9 );
   for ( i = 0; i < n; i++ )
   {
      /* compute "ideal" image coordinates */
      if ( !gan_camera_backproject_point_i ( cal2, &p2[i] ) ||
           !gan_camera_backproject_point_i ( cal1, &p1[i] ) )
      {
         gan_err_register ( "gan_essential_matrix_fit", GAN_ERROR_FAILURE, "");
         return GAN_FALSE;
      }

      /* scale image coordinates to unit length */
      gan_vec3_unit_i ( &p1[i] );
      gan_vec3_unit_i ( &p2[i] );

      gan_symeigen_increment ( sme, 1.0,
                           p2[i].x*p1[i].x, p2[i].x*p1[i].y, p2[i].x*p1[i].z,
                           p2[i].y*p1[i].x, p2[i].y*p1[i].y, p2[i].y*p1[i].z,
                           p2[i].z*p1[i].x, p2[i].z*p1[i].y, p2[i].z*p1[i].z );
   }

   gan_symeigen_solve ( sme );

   /* build essential matrix E from eigenvector corresponding to smallest
      eigenvalue */
   Z = &sme->Z;
   E = gan_mat33_fill_s ( gel(Z,0,0), gel(Z,1,0), gel(Z,2,0),
                          gel(Z,3,0), gel(Z,4,0), gel(Z,5,0),
                          gel(Z,6,0), gel(Z,7,0), gel(Z,8,0) );

   /* compute the singular value decomposition of E */
   gan_mat33T_svd ( &E, &U, &S, &VT );
   
#ifndef NDEBUG
   {
      /* check SVD */
      Gan_Matrix33 t = VT;
      t.xx *= S.x; t.xy *= S.x; t.xz *= S.x;
      t.yx *= S.y; t.yy *= S.y; t.yz *= S.y;
      t.zx *= S.z; t.zy *= S.z; t.zz *= S.z;
      t = gan_mat33_rmultm33_s(&U, &t);
      t = gan_mat33_tpose_s(&t);
      gan_mat33_print ( &E, "E^T", 0, "%f" );
      gan_mat33_print ( &t, "E^T?", 0, "%f" );
   }
#endif

   /* E^T should have a null-vector as its last singular vector */
   (void)gan_vec3_fill_q ( &e2, VT.zx, VT.zy, VT.zz );

   /* change sign if necessary */
   {
      double total;
      Gan_Vector3 x1, x2;

      for ( i = 0, total = 0.0; i < n; i++ )
      {
         x1 = gan_mat33_multv3_s ( &E, &p1[i] );
         x2 = gan_vec3_cross_s ( &e2, &p2[i] );
         total += gan_vec3_dot_q ( &x2, &x1 );
      }

      if ( total < 0.0 ) (void)gan_vec3_scale_i ( &e2, -1.0 );
   }

   /* compute quaternion representation of rotation */
   gan_symeigen_reset ( sme, 4 );

   gan_symeigen_increment ( sme, 1.0, 0.0, -E.xx, -e2.z-E.xy,  e2.y-E.xz );
   gan_symeigen_increment ( sme, 1.0, E.xx,  0.0, -e2.y-E.xz, -e2.z+E.xy );
   gan_symeigen_increment ( sme, 1.0, e2.z+E.xy, e2.y+E.xz,   0.0, -E.xx );
   gan_symeigen_increment ( sme, 1.0, -e2.y+E.xz, e2.z-E.xy, E.xx,   0.0 );

   gan_symeigen_increment ( sme, 1.0, 0.0,  e2.z-E.yx, -E.yy, -e2.x-E.yz );
   gan_symeigen_increment ( sme, 1.0, -e2.z+E.yx,   0.0, e2.x-E.yz, E.yy );
   gan_symeigen_increment ( sme, 1.0, E.yy, -e2.x+E.yz,  0.0, -e2.z-E.yx );
   gan_symeigen_increment ( sme, 1.0, e2.x+E.yz, -E.yy, e2.z+E.yx,   0.0 );

   gan_symeigen_increment ( sme, 1.0, 0.0, -e2.y-E.zx,  e2.x-E.zy, -E.zz );
   gan_symeigen_increment ( sme, 1.0, e2.y+E.zx,   0.0, -E.zz, e2.x+E.zy );
   gan_symeigen_increment ( sme, 1.0, -e2.x+E.zy, E.zz,   0.0, e2.y-E.zx );
   gan_symeigen_increment ( sme, 1.0, E.zz, -e2.x-E.zy, -e2.y+E.zx,  0.0 );

   gan_symeigen_solve ( sme );

   Z = &sme->Z;
   q = gan_quat_fill_s ( gel(Z,0,0), gel(Z,1,0),
                         gel(Z,2,0), gel(Z,3,0) );

   {
      Gan_Matrix33 R, mTx, ET, Rm;

      R = gan_mat33_fill_s (
          q.q0*q.q0+q.q1*q.q1-q.q2*q.q2-q.q3*q.q3, 2.0*(q.q1*q.q2-q.q0*q.q3),
                                                   2.0*(q.q1*q.q3+q.q0*q.q2),
          2.0*(q.q2*q.q1+q.q0*q.q3), q.q0*q.q0-q.q1*q.q1+q.q2*q.q2-q.q3*q.q3,
                                     2.0*(q.q2*q.q3-q.q0*q.q1),
          2.0*(q.q3*q.q1-q.q0*q.q2), 2.0*(q.q3*q.q2+q.q0*q.q1),
          q.q0*q.q0-q.q1*q.q1-q.q2*q.q2+q.q3*q.q3 );
      mTx = gan_mat33_cross_s ( &e2 );
      gan_mat33_tpose_i ( &mTx );
      ET = gan_mat33_tpose_s ( &E );
      Rm = gan_mat33_rmultm33_s ( &R, &mTx );
      (void)gan_mat33_scale_i ( &ET, gan_mat33_Fnorm_s(&Rm)
                                     /gan_mat33_Fnorm_s(&ET) );
#ifndef NDEBUG
      if ( gan_debug )
      {
         gan_mat33_print ( &ET, "E^T", 0, "%f" );
         gan_mat33_print ( &Rm, "R^-1*(-Tx)", 0, "%f" );
      }
#endif
   }
                          
   /* reverse q0 to invert the rotation */
   q.q0 = -q.q0;

   /* compute the minimal representation of rotation */
   euc->rot.type = GAN_ROT3D_QUATERNION;
   euc->rot.data.q = q;

   /* compute rotation matrix */
   rot = gan_rot3D_convert_s ( &euc->rot, GAN_ROT3D_MATRIX );

   /* find sign of translation vector */
   {
      double total;

      for ( i = 0, total = 0.0; i < n; i++ )
      {
         Gan_Vector3 x1, x2;

         x1 = gan_mat33_multv3_s ( &rot.data.R, &p1[i] );
         x1 = gan_vec3_cross_s ( &p2[i], &x1 );
         x2 = gan_vec3_cross_s ( &e2, &p2[i] );
         total += gan_vec3_dot_q ( &x1, &x2 );
      }

      if ( total < 0.0 ) (void)gan_vec3_scale_i ( &e2, -1.0 );

      euc->trans = e2;
#ifndef NDEBUG
      if ( gan_debug )
         gan_vec3_print ( &euc->trans, "t", 0, "%f" );
#endif
   }

   /* free eigensystem structure if it was allocated */
   if ( sme_alloc )
      gan_symeigen_free ( sme );

   /* return with success */
   return GAN_TRUE;
}

/**
 * \}
 */

/**
 * \}
 */

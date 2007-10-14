/**
 * File:          $RCSfile: camera_affine.c,v $
 * Module:        Affine camera functions
 * Part of:       Gandalf Library
 *
 * Revision:      $Revision: 1.1.2.1 $
 * Last edited:   $Date: 2007/10/14 02:33:12 $
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

#include <gandalf/vision/camera_affine.h>

/**
 * \addtogroup Vision
 * \{
 */

/**
 * \addtogroup Camera
 * \{
 */

#define CZH camera->zh
#define CFX camera->fx
#define CFY camera->fy
#define CX0 camera->x0
#define CY0 camera->y0
#define CSKEW camera->nonlinear.affine.skew
#define CKYX  camera->nonlinear.affine.kyx
#define CKIXX  camera->nonlinear.affine.Kinv.xx
#define CKIXY  camera->nonlinear.affine.Kinv.xy
#define CKIXZ  camera->nonlinear.affine.Kinv.xz
#define CKIYX  camera->nonlinear.affine.Kinv.yx
#define CKIYY  camera->nonlinear.affine.Kinv.yy
#define CKIYZ  camera->nonlinear.affine.Kinv.yz

/* point projection function */
static Gan_Bool
 point_project ( const Gan_Camera *camera,
                 Gan_Vector3 *X, Gan_Vector3 *p, Gan_PositionState *Xpprev,
                 Gan_Matrix22 *HX, Gan_Camera HC[2],
                 int *error_code )
{
   double XZ, YZ;

   /* consistency check */
   gan_err_test_bool ( camera->type == GAN_AFFINE_CAMERA, "point_project", GAN_ERROR_INCOMPATIBLE, "" );

   /* if we are computing Jacobians, the camera coordinates must have Z=1 */
#ifndef NDEBUG
   if ( (HX != NULL || HC != NULL) && X->z != 1.0 )
   {
      if ( error_code == NULL )
      {
         gan_err_flush_trace();
         gan_err_register ( "point_project", GAN_ERROR_INCOMPATIBLE, "" );
      }
      else
         *error_code = GAN_ERROR_INCOMPATIBLE;

      return GAN_FALSE;
   }
#endif

   if ( X->z == 0.0 )
   {
      if ( error_code == NULL )
      {
         gan_err_flush_trace();
         gan_err_register ( "point_project", GAN_ERROR_DIVISION_BY_ZERO, "" );
      }
      else
         *error_code = GAN_ERROR_DIVISION_BY_ZERO;

      return GAN_FALSE;
   }

   if ( X->z == 1.0 )
   {
      XZ = X->x;
      YZ = X->y;
   }
   else
   {
      /* divide through by Z coordinate to normalise it to 1 */
      XZ = X->x/X->z;
      YZ = X->y/X->z;
   }

   /* build homogeneous image coordinates of projected point */
   (void)gan_vec3_fill_q ( p, CX0 + CFX*XZ + CSKEW*YZ, CY0 + CKYX*XZ + CFY*YZ, CZH );

   /* compute Jacobians */
   if ( HX != NULL )
      (void)gan_mat22_fill_q ( HX, CFX,  CSKEW,
                                   CKYX, CFY );

   if ( HC != NULL )
   {
      HC[0].zh = 0.0;
      HC[0].fx = XZ;
      HC[0].fy = 0.0;
      HC[0].x0 = 1.0;
      HC[0].y0 = 0.0;
      HC[0].nonlinear.affine.skew = YZ;
      HC[0].nonlinear.affine.kyx  = 0.0;
      HC[0].type = GAN_AFFINE_CAMERA;
      HC[1].zh = 0.0;
      HC[1].fx = 0.0;
      HC[1].fy = YZ;
      HC[1].x0 = 0.0;
      HC[1].y0 = 1.0;
      HC[1].nonlinear.affine.skew = 0.0;
      HC[1].nonlinear.affine.kyx  = XZ;
      HC[1].type = GAN_AFFINE_CAMERA;
   }

   return GAN_TRUE;
}
   
/* point back-projection function */
static Gan_Bool
 point_backproject ( const Gan_Camera *camera,
                     Gan_Vector3 *p, Gan_Vector3 *X, Gan_PositionState *pXprev,
                     int *error_code )
{
   Gan_Vector2 Xd;

   /* consistency check */
   gan_err_test_bool ( camera->type == GAN_AFFINE_CAMERA, "point_backproject",
                       GAN_ERROR_INCOMPATIBLE, "" );

   if ( camera->fx == 0.0 || camera->fy == 0.0 || camera->zh == 0.0 )
   {
      if ( error_code == NULL )
      {
         gan_err_flush_trace();
         gan_err_register ( "point_backproject", GAN_ERROR_DIVISION_BY_ZERO, "" );
      }
      else
         *error_code = GAN_ERROR_DIVISION_BY_ZERO;

      return GAN_FALSE;
   }

   /* can't back-project a point on z=0 plane */
   if ( p->z == 0.0 )
   {
      if ( error_code == NULL )
      {
         gan_err_flush_trace();
         gan_err_register ( "point_backproject", GAN_ERROR_DIVISION_BY_ZERO, "" );
      }
      else
         *error_code = GAN_ERROR_DIVISION_BY_ZERO;

      return GAN_FALSE;
   }

   /* compute distorted X/Y coordinates of scene point Xd on plane Z=1 */
   (void)gan_vec2_fill_q ( &Xd, CZH*(CKIXX*p->x + CKIXY*p->y + CKIXZ*p->z)/p->z,
                                CZH*(CKIYX*p->x + CKIYY*p->y + CKIYZ*p->z)/p->z );

   /* build scene point */
   gan_vec3_set_parts_q ( X, &Xd, 1.0 );

   /* success */
   return GAN_TRUE;
}

/* function to add distortion to a point */
static Gan_Bool
 point_add_distortion ( const Gan_Camera *camera,
                        Gan_Vector3 *pu, Gan_Vector3 *p, Gan_PositionState *pupprev,
                        int *error_code )
{
   Gan_Vector2 x;

   /* consistency check */
   gan_err_test_bool ( camera->type == GAN_AFFINE_CAMERA,
                       "point_add_distortion", GAN_ERROR_INCOMPATIBLE, "" );

#ifndef NDEBUG
   if ( camera->fx == 0.0 || camera->fy == 0.0 || camera->zh == 0.0 )
   {
      if ( error_code == NULL )
      {
         gan_err_flush_trace();
         gan_err_register ( "point_add_distortion",
                            GAN_ERROR_DIVISION_BY_ZERO, "" );
      }
      else
         *error_code = GAN_ERROR_DIVISION_BY_ZERO;
   
      return GAN_FALSE;
   }
#endif /* #ifndef NDEBUG */

   if ( pu->z == 0.0 )
   {
      if ( error_code == NULL )
      {
         gan_err_flush_trace();
         gan_err_register ( "point_add_distortion",
                            GAN_ERROR_DIVISION_BY_ZERO, "" );
      }
      else
         *error_code = GAN_ERROR_DIVISION_BY_ZERO;
         
      return GAN_FALSE;
   }

   /* compute affine image coordinates */
   x = gan_vec2_fill_s ( CZH*pu->x/pu->z, CZH*pu->y/pu->z );

   /* fill image point with distortion added */
   gan_vec3_set_parts_q ( p, &x, CZH );

   /* success */
   return GAN_TRUE;
}
   
/* function to remove distortion from a point */
static Gan_Bool
 point_remove_distortion ( const Gan_Camera *camera,
                           Gan_Vector3 *p, Gan_Vector3 *pu, Gan_PositionState *ppuprev,
                           int *error_code)
{
   Gan_Vector2 xu;

   /* consistency check */
   gan_err_test_bool ( camera->type == GAN_AFFINE_CAMERA,
                       "point_remove_distortion", GAN_ERROR_INCOMPATIBLE, "" );

   if ( p->z == 0.0 )
   {
      if ( error_code == NULL )
      {
         gan_err_flush_trace();
         gan_err_register ( "point_remove_distortion",
                            GAN_ERROR_DIVISION_BY_ZERO, "" );
      }
      else
         *error_code = GAN_ERROR_DIVISION_BY_ZERO;

      return GAN_FALSE;
   }

   xu = gan_vec2_fill_s ( CZH*p->x/p->z, CZH*p->y/p->z );

   /* fill image point with distortion removed */
   gan_vec3_set_parts_q ( pu, &xu, CZH );

   /* success */
   return GAN_TRUE;
}

/* line functions */

/* line projection function */
static Gan_Bool
 line_project ( const Gan_Camera *camera,
                Gan_Vector3 *L, Gan_Vector3 *l )
{
   Gan_Vector3 line;

   /* consistency check */
   gan_err_test_bool ( camera->type == GAN_AFFINE_CAMERA, "line_project", GAN_ERROR_INCOMPATIBLE, "" );

   if ( CFX == 0.0 || CFY == 0.0 || CZH == 0.0 )
   {
      gan_err_flush_trace();
      gan_err_register ( "line_project", GAN_ERROR_DIVISION_BY_ZERO, "" );
   }

   /* not implemented */
   gan_err_flush_trace();
   gan_err_register ( "line_project", GAN_ERROR_NOT_IMPLEMENTED, "" );
   return GAN_FALSE;

   /* project line using l = K^-T*L */
   line.x = l->x/CFX;
   line.y = l->y/CFY;
   line.z = (l->z - line.x*camera->x0 - line.y*camera->y0)/CZH;

   /* fill image line */
   *l = line;

   /* success */
   return GAN_TRUE;
}
   
/* line back-projection function */
static Gan_Bool
 line_backproject ( const Gan_Camera *camera, Gan_Vector3 *l, Gan_Vector3 *L )
{
   Gan_Vector3 Line;

   /* consistency check */
   gan_err_test_bool ( camera->type == GAN_AFFINE_CAMERA,
                       "line_backproject", GAN_ERROR_INCOMPATIBLE, "" );

   /* not implemented */
   gan_err_flush_trace();
   gan_err_register ( "line_backproject", GAN_ERROR_NOT_IMPLEMENTED, "" );
   return GAN_FALSE;

   Line.x = CFX*l->x;
   Line.y = CFY*l->y;
   Line.z = l->x*camera->x0 + l->y*camera->y0 + l->z*CZH;

   /* fill scene line */
   *L = Line;

   /* success */
   return GAN_TRUE;
}

/**
 * \brief Builds a structure representing a affine camera.
 * \param camera Pointer to the camera structure to be filled
 * \param zh Third homogeneous image coordinate
 * \param fx Focal distance measured in x-pixels
 * \param fy Focal distance measured in y-pixels
 * \param x0 x-coordinate of image centre
 * \param y0 y-coordinate of image centre
 * \param skew Affine off-diagonal component xy
 * \param kyx  Affine off-diagonal component yx
 *
 * Constructs a structure representing a camera, under the affine pinhole
 * camera model. This is the double precision version.
 *
 * \return #GAN_TRUE on successfully building the camera structure,
 *         #GAN_FALSE on failure.
 * \sa gan_cameraf_build_affine().
 */
Gan_Bool
 gan_camera_build_affine ( Gan_Camera *camera,
                           double zh, double fx, double fy,
                           double x0, double y0, double skew, double kyx )
{
   Gan_Matrix23 K;

   if ( !gan_camera_set_common_fields ( camera, GAN_AFFINE_CAMERA, zh, fx, fy, x0, y0 ) )
   {
      gan_err_register ( "gan_camera_build_affine", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   /* add off-diagonal affine parameters */
   camera->nonlinear.affine.skew = skew;
   camera->nonlinear.affine.kyx  = kyx;
   (void)gan_mat23_fill_q(&K,  fx, skew, x0,
                              kyx,   fy, y0);
   if(!gan_mat23h_invert_q(&K, zh, &camera->nonlinear.affine.Kinv, NULL))
   {
      gan_err_register ( "gan_camera_build_affine", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   /* add point projection/backprojection/distortion functions */
   camera->point.project           = point_project;
   camera->point.backproject       = point_backproject;
   camera->point.add_distortion    = point_add_distortion;
   camera->point.remove_distortion = point_remove_distortion;

   /* add line projection/backprojection/distortion functions */
   camera->line.project     = line_project;
   camera->line.backproject = line_backproject;

   /* success */
   return GAN_TRUE;
}

#undef CFX
#undef CFY
#undef CX0
#undef CY0

/**
 * \}
 */

/**
 * \}
 */

/**
 * File:          $RCSfile: camera_xy_dist4.c,v $
 * Module:        Functions for camera with 4 assymmetric distortion parameters
 * Part of:       Gandalf Library
 *
 * Revision:      $Revision: 1.1.2.1 $
 * Last edited:   $Date: 2007/10/14 02:33:16 $
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

#include <gandalf/vision/camera_xy_dist4.h>

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
#define CXX camera->nonlinear.xydist4.cxx
#define CXY camera->nonlinear.xydist4.cxy
#define CYX camera->nonlinear.xydist4.cyx
#define CYY camera->nonlinear.xydist4.cyy

/* threshold on closness of camera coordinates to optical axis */
#define SMALL_DR_THRES_SQUARED 1.0e-6

/* termination threshold */
#define TERMINATION_THRESHOLD 1.0e-7

/* minimum/maximum number of iterations to apply when applying inverse
   distortion */
#define MIN_UND_ITERATIONS   6
#define MAX_UND_ITERATIONS 100

/* point projection function */
static Gan_Bool
 point_project ( const Gan_Camera *camera,
                 Gan_Vector3 *X, Gan_Vector3 *p, Gan_PositionState *Xpprev,
                 Gan_Matrix22 *HX, Gan_Camera HC[2],
                 int *error_code )
{
   Gan_Vector2 x;
   double XZ, YZ, XZ2, YZ2, dx, dy;

   /* consistency check */
   gan_err_test_bool ( camera->type == GAN_XY_DISTORTION_4,
                       "point_project", GAN_ERROR_INCOMPATIBLE, "" );

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

   if ( X->z != 1.0 )
   {
      /* divide through by Z coordinate to normalise it to 1 */
      XZ = X->x/X->z;
      YZ = X->y/X->z;
   }
   else
   {
      XZ = X->x;
      YZ = X->y;
   }

   XZ2 = XZ*XZ;
   YZ2 = YZ*YZ;
   dx = 1.0 + CXX*XZ2 + CXY*YZ2;
   dy = 1.0 + CYX*XZ2 + CYY*YZ2;
         
   x.x = CX0 + dx*CFX*XZ;
   x.y = CY0 + dy*CFY*YZ;

   /* compute Jacobians */
   if ( HX != NULL )
      (void)gan_mat22_fill_q ( HX,
                        CFX*(1.0 + 3.0*CXX*XZ2 + CXY*YZ2), 2.0*CFX*CXY*XZ*YZ,
                        2.0*CFY*CYX*XZ*YZ, CFY*(1.0 + CYX*XZ2 + 3.0*CYY*YZ2) );

   if ( HC != NULL )
   {
      HC[0].zh = 0.0;
      HC[0].fx = dx*XZ;
      HC[0].fy = 0.0;
      HC[0].x0 = 1.0;
      HC[0].y0 = 0.0;
      HC[0].type = GAN_XY_DISTORTION_4;
      HC[0].nonlinear.xydist4.cxx = CFX*XZ2*XZ;
      HC[0].nonlinear.xydist4.cxy = CFX*YZ2*XZ;
      HC[0].nonlinear.xydist4.cyx = 0.0;
      HC[0].nonlinear.xydist4.cyy = 0.0;
      HC[1].zh = 0.0;
      HC[1].fx = 0.0;
      HC[1].fy = dy*YZ;
      HC[1].x0 = 0.0;
      HC[1].y0 = 1.0;
      HC[1].type = GAN_XY_DISTORTION_4;
      HC[1].nonlinear.xydist4.cxx = 0.0;
      HC[1].nonlinear.xydist4.cxy = 0.0;
      HC[1].nonlinear.xydist4.cyx = CFY*XZ2*YZ;
      HC[1].nonlinear.xydist4.cyy = CFY*YZ2*YZ;
   }

   /* build homogeneous image coordinates of projected point */
   (void)gan_vec3_set_parts_q ( p, &x, CZH );

   return GAN_TRUE;
}

/* compute undistorted coordinates */
static Gan_Bool
 compute_undistorted_coordinates ( Gan_Vector2 *Xd, double cxx, double cxy,
                                                    double cyx, double cyy,
                                   Gan_Vector2 *Xu, int *error_code )
{
   Gan_Matrix22 G;
   Gan_Vector2 v, d;
   double X2, Y2, det;
   int it;

   /* return immediately with zero distortion if the distortion coefficients
      are zero or the distance from the origin is small */
   if ( (cxx == 0.0 && cxy == 0.0 && cyx == 0.0 && cyy == 0.0) ||
        Xd->x*Xd->x+Xd->y*Xd->y < SMALL_DR_THRES_SQUARED )
   {
      *Xu = *Xd;
      return GAN_TRUE;
   }

   /* initialize undistorted coordinates */
   (void)gan_vec2_fill_q ( Xu,
                           Xd->x*(1.0 - cxx*Xd->x*Xd->x - cxy*Xd->y*Xd->y),
                           Xd->y*(1.0 - cyx*Xd->x*Xd->x - cyy*Xd->y*Xd->y) );
   for ( it = 1; ; it++ )
   {
      X2 = Xu->x*Xu->x;
      Y2 = Xu->y*Xu->y;
      (void)gan_mat22_fill_q ( &G, 1.0+3.0*cxx*X2+cxy*Y2, 2.0*cxy*Xu->x*Xu->y,
                                   2.0*cyx*Xu->x*Xu->y, 1.0+3.0*cyy*Y2+cyx*X2);
      (void)gan_vec2_fill_q ( &v, Xd->x - Xu->x*(1.0+cxx*X2+cxy*Y2),
                                  Xd->y - Xu->y*(1.0+cyx*X2+cyy*Y2) );
      det = gan_mat22_det_q(&G);
      if ( det == 0.0 )
      {
         if ( error_code == NULL )
         {
            gan_err_flush_trace();
            gan_err_register ( "compute_undistorted_coordinates",
                               GAN_ERROR_DIVISION_BY_ZERO, "" );
         }
         else
            *error_code = GAN_ERROR_DIVISION_BY_ZERO;

         return GAN_FALSE;
      }

      (void)gan_vec2_fill_q ( &d, (G.yy*v.x - G.xy*v.y)/det,
                                  (G.xx*v.y - G.yx*v.x)/det );
      (void)gan_vec2_increment ( Xu, &d );

      /* check whether we have converged, observing the minimum
         number of iterations */
      if ( fabs(d.x)+fabs(d.y) < TERMINATION_THRESHOLD &&
           it >= MIN_UND_ITERATIONS )
         break;
        
      /* check whether we have exceeded the maximum number of
         iterations */
      if ( it > MAX_UND_ITERATIONS )
      {
         if ( error_code == NULL )
         {
            gan_err_flush_trace();
            gan_err_register ( "compute_undistorted_coordinates",
                               GAN_ERROR_OUTSIDE_RANGE, "" );
         }
         else
            *error_code = GAN_ERROR_OUTSIDE_RANGE;

         return GAN_FALSE;
      }
   }
   
   return GAN_TRUE;
}

/* point back-projection function */
static Gan_Bool
 point_backproject ( const Gan_Camera *camera,
                     Gan_Vector3 *p, Gan_Vector3 *X, Gan_PositionState *pXprev,
                     int *error_code )
{
   Gan_Vector2 Xd, Xu;

   /* consistency check */
   gan_err_test_bool ( camera->type == GAN_XY_DISTORTION_4,
                       "point_backproject", GAN_ERROR_INCOMPATIBLE, "" );

   if ( CFX == 0.0 || CFY == 0.0 || CZH == 0.0 )
   {
      if ( error_code == NULL )
      {
         gan_err_flush_trace();
         gan_err_register ( "point_backproject", GAN_ERROR_DIVISION_BY_ZERO,
                            "" );
      }
      else
         *error_code = GAN_ERROR_DIVISION_BY_ZERO;
   }

   /* can't back-project a point on z=0 plane */
   if ( p->z == 0.0 )
   {
      if ( error_code == NULL )
      {
         gan_err_flush_trace();
         gan_err_register ( "point_backproject", GAN_ERROR_DIVISION_BY_ZERO,
                            "" );
      }
      else
         *error_code = GAN_ERROR_DIVISION_BY_ZERO;

      return GAN_FALSE;
   }

   /* compute distorted X/Y coordinates of scene point Xd on plane Z=1 */
   Xd.x = (CZH*p->x - CX0*p->z)/(CFX*p->z);
   Xd.y = (CZH*p->y - CY0*p->z)/(CFY*p->z);

   /* compute undistorted coordinates */
   if ( !compute_undistorted_coordinates ( &Xd, CXX, CXY, CYX, CYY, &Xu,
                                           error_code ) )
   {
      if ( error_code == NULL )
         gan_err_register ( "point_backproject", GAN_ERROR_FAILURE, "" );

      return GAN_FALSE;
   }

   /* build undistorted scene point */
   gan_vec3_set_parts_q ( X, &Xu, 1.0 );

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
   double XZ, YZ, XZ2, YZ2, dx, dy;

   /* consistency check */
   gan_err_test_bool ( camera->type == GAN_XY_DISTORTION_4,
                       "point_add_distortion", GAN_ERROR_INCOMPATIBLE, "" );

#ifndef NDEBUG
   if ( CFX == 0.0 || CFY == 0.0 || CZH == 0.0 )
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

   /* compute image coordinates */
   x = gan_vec2_fill_s ( CZH*pu->x/pu->z, CZH*pu->y/pu->z );

   XZ = (x.x - CX0)/CFX;
   YZ = (x.y - CY0)/CFY;
   XZ2 = XZ*XZ;
   YZ2 = YZ*YZ;
   dx = 1.0 + CXX*XZ2 + CXY*YZ2;
   dy = 1.0 + CYX*XZ2 + CYY*YZ2;

   /* fill image point with distortion added */
   (void)gan_vec3_fill_q ( p, CX0 + dx*(x.x - CX0), CY0 + dy*(x.y - CY0),
                              CZH );

   /* success */
   return GAN_TRUE;
}
   
/* function to remove distortion from a point */
static Gan_Bool
 point_remove_distortion ( const Gan_Camera *camera,
                           Gan_Vector3 *p, Gan_Vector3 *pu, Gan_PositionState *ppuprev,
                           int *error_code)
{
   Gan_Vector2 Xd, Xu;

   /* consistency check */
   gan_err_test_bool ( camera->type == GAN_XY_DISTORTION_4,
                       "point_remove_distortion", GAN_ERROR_INCOMPATIBLE, "" );

   if ( p->z == 0.0 )
   {
      gan_err_flush_trace();
      gan_err_register ( "point_remove_distortion",
                         GAN_ERROR_DIVISION_BY_ZERO, "" );
      return GAN_FALSE;
   }

   /* compute distorted X/Y coordinates of scene point Xd on plane Z=1 */
   (void)gan_vec2_fill_q ( &Xd, (CZH*p->x - CX0*p->z)/(CFX*p->z),
                                (CZH*p->y - CY0*p->z)/(CFY*p->z) );

   /* compute undistorted coordinates */
   if ( !compute_undistorted_coordinates ( &Xd, CXX, CXY, CYX, CYY, &Xu,
                                           error_code ) )
   {
      gan_err_register ( "point_remove_distortion", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   /* fill image point with distortion removed */
   (void)gan_vec3_fill_q ( pu, CX0 + CFX*Xu.x, CY0 + CFY*Xu.y, CZH );

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
   gan_err_test_bool ( camera->type == GAN_XY_DISTORTION_4,
                       "line_project", GAN_ERROR_INCOMPATIBLE, "" );

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
   line.z = (l->z - line.x*CX0 - line.y*CY0)/CZH;

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
   gan_err_test_bool ( camera->type == GAN_XY_DISTORTION_4,
                       "line_backproject", GAN_ERROR_INCOMPATIBLE, "" );

   /* not implemented */
   gan_err_flush_trace();
   gan_err_register ( "line_backproject", GAN_ERROR_NOT_IMPLEMENTED, "" );
   return GAN_FALSE;

   Line.x = CFX*l->x;
   Line.y = CFY*l->y;
   Line.z = l->x*CX0 + l->y*CY0 + l->z*CZH;

   /* fill scene line */
   *L = Line;

   /* success */
   return GAN_TRUE;
}

/**
 * \brief Builds a structure representing a xy-distortion camera.
 * \param camera Pointer to the camera structure to be filled
 * \param zh Third homogeneous image coordinate
 * \param fx Focal distance measured in x-pixels
 * \param fy Focal distance measured in y-pixels
 * \param x0 x-coordinate of image centre
 * \param y0 y-coordinate of image centre
 * \param cxx Coefficient of x-distortion in x-direction
 * \param cxy Coefficient of y-distortion in x-direction
 * \param cyx Coefficient of x-distortion in y-direction
 * \param cyy Coefficient of y-distortion in y-direction
 *
 * Constructs a structure representing a camera, under the model that the
 * distortion has independent quadratic x & y components in both x & y
 * directions. This model is used in inverted form by 3D Equalizer.
 * This is the double precision version.
 *
 * \return #GAN_TRUE on successfully building the camera structure,
 *         #GAN_FALSE on failure.
 * \sa gan_cameraf_build_xy_distortion_4().
 */
Gan_Bool
 gan_camera_build_xy_distortion_4 ( Gan_Camera *camera,
                                    double zh,
                                    double fx, double fy,
                                    double x0, double y0,
                                    double cxx, double cxy,
                                    double cyx, double cyy )
{
   if ( !gan_camera_set_common_fields ( camera, GAN_XY_DISTORTION_4,
                                        zh, fx, fy, x0, y0 ) )
   {
      gan_err_register ( "gan_camera_build_xy_distortion_4",
                         GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   /* copy non-linear camera parameters */
   camera->nonlinear.xydist4.cxx = cxx; /* x-distortion dependent on x-value */
   camera->nonlinear.xydist4.cxy = cxy; /* x-distortion dependent on y-value */
   camera->nonlinear.xydist4.cyx = cyx; /* y-distortion dependent on x-value */
   camera->nonlinear.xydist4.cyy = cyy; /* y-distortion dependent on y-value */

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

/**
 * \}
 */

/**
 * \}
 */

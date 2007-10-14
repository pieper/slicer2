/**
 * File:          $RCSfile: camera_radial_dist1_inv.c,v $
 * Module:        Camera with 1 inverse radial distortion parameter
 * Part of:       Gandalf Library
 *
 * Revision:      $Revision: 1.1.2.1 $
 * Last edited:   $Date: 2007/10/14 02:33:15 $
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

#include <gandalf/vision/camera_radial_dist1_inv.h>
#include <gandalf/common/numerics.h>

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
#define CK1 camera->nonlinear.radial1.K1
#define CTR2 camera->nonlinear.radial1.thres_R2
#define CTDR camera->nonlinear.radial1.thres_dR
#define COA camera->nonlinear.radial1.outer_a
#define COB camera->nonlinear.radial1.outer_b

/* thresholds on change in radial distance R value */
#define R_CHANGE_THRES 1.0e-7

/* minimum/maximum number of iterations to apply when applying inverse
   distortion */
#define MIN_UND_ITERATIONS   6
#define MAX_UND_ITERATIONS 100

/* threshold on closness of camera coordinates to optical axis */
#define SMALL_DR_THRES 0.001

static Gan_Bool
 compute_distortion ( double K1, double thres_dR,
                      double outer_a, double outer_b, Gan_Vector2 *X,
                      double *dp, int *error_code )
{
   double dR = sqrt(gan_vec2_sqrlen_q(X)), R = DBL_MAX;
   Gan_Complex x[3];
   int nos;
   Gan_Bool found = GAN_FALSE;

   /* return immediately with zero distortion if the distortion coefficient
      is zero or the radial distance is small */
   if ( K1 == 0.0 || dR < SMALL_DR_THRES )
   {
      *dp = 1.0;
      return GAN_TRUE;
   }

   /* check whether d*R is outside range of distortion model */
   if ( dR > thres_dR )
   {
      /* use outer linear model */

      /* compute distortion value dR/R by solving linear equation for R */
      *dp = dR*outer_a/(dR - outer_b);

      return GAN_TRUE;
   }

   /* solve cubic for R given dR = d*R */
   nos = gan_solve_cubic ( K1, 0.0, 1.0, -dR, x );

   /* find the smallest positive solution for R */
   for ( nos--; nos >= 0; nos-- )
   {
      if ( x[nos].i != 0.0 ) continue;
      if ( x[nos].r > 0.0 && x[nos].r < R )
      {
         R = x[nos].r;
         found = GAN_TRUE;
      }
   }

   if ( !found )
   {
      if ( error_code == NULL )
      {
         gan_err_flush_trace();
         gan_err_register ( "compute_distortion", GAN_ERROR_NO_CONVERGENCE,
                            "" );
      }
      else
         *error_code = GAN_ERROR_NO_CONVERGENCE;

      return GAN_FALSE;
   }

   /* compute distortion scale factor */
   *dp = 1.0 + K1*R*R;

   /* success */
   return GAN_TRUE;
}

/* point projection function */
static Gan_Bool
 point_project ( const Gan_Camera *camera,
                 Gan_Vector3 *X, Gan_Vector3 *p, Gan_PositionState *Xpprev,
                 Gan_Matrix22 *HX, Gan_Camera HC[2],
                 int *error_code )
{
   Gan_Vector2 X2;
   double dp;

   /* consistency check */
   gan_err_test_bool ( camera->type == GAN_RADIAL_DISTORTION_1_INV,
                       "point_project", GAN_ERROR_INCOMPATIBLE, "" );

   /* no Jacobian computation allowed */
   if ( HX != NULL || HC != NULL )
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

   /* build undistorted X/Y coordinates of scene point X on plane Z=1 */
   if ( X->z != 1.0 )
   {
      /* divide through by Z coordinate to normalise it to 1 */
      (void)gan_vec2_fill_q ( &X2, X->x/X->z, X->y/X->z );
   }
   else
   {
      (void)gan_vec2_fill_q ( &X2, X->x, X->y );
   }

   if ( !compute_distortion ( CK1, CTDR, COA, COB, &X2, &dp, error_code ) )
   {
      if ( error_code == NULL )
         gan_err_register ( "point_project", GAN_ERROR_FAILURE, "" );

      return GAN_FALSE;
   }

   /* can't have zero distortion coefficient */
   if ( dp == 0.0 )
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

   /* build distorted camera coordinates */
   (void)gan_vec3_fill_q ( p, CX0 + CFX*X2.x/dp, CY0 + CFY*X2.y/dp, CZH );

   /* success */
   return GAN_TRUE;
}
   
/* point back-projection function */
static Gan_Bool
 point_backproject ( const Gan_Camera *camera,
                     Gan_Vector3 *p, Gan_Vector3 *X, Gan_PositionState *pXprev,
                     int *error_code )
{
   double XZp, YZp, XZp2, YZp2, Rp2, dp;

   /* consistency check */
   gan_err_test_bool ( camera->type == GAN_RADIAL_DISTORTION_1_INV,
                       "point_project", GAN_ERROR_INCOMPATIBLE, "" );

#ifndef NDEBUG
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
#endif /* #ifndef NDEBUG */

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

   /* compute distorted X/Y coordinates */
   XZp = (CZH*p->x - CX0*p->z)/(CFX*p->z);
   YZp = (CZH*p->y - CY0*p->z)/(CFY*p->z);

   /* compute squared radial distance R2 */
   XZp2 = XZp*XZp;
   YZp2 = YZp*YZp;
   Rp2 = XZp2 + YZp2;

   if ( Rp2 > CTR2 )
   {
      if ( error_code == NULL )
      {
         gan_err_flush_trace();
         gan_err_register ( "point_backproject", GAN_ERROR_OUTSIDE_RANGE, "" );
      }
      else
         *error_code = GAN_ERROR_OUTSIDE_RANGE;

      return GAN_FALSE;
   }

   /* calculate distortion and compute the undistorted X/Y */
   dp = 1.0 + CK1*Rp2;
   (void)gan_vec3_fill_q ( X, dp*XZp, dp*YZp, 1.0 );

   return GAN_TRUE;
}

/* function to add distortion to a point */
static Gan_Bool
 point_add_distortion ( const Gan_Camera *camera,
                        Gan_Vector3 *pu, Gan_Vector3 *p, Gan_PositionState *pupprev,
                        int *error_code )
{
   Gan_Vector2 X;
   double dp;

   /* consistency check */
   gan_err_test_bool ( camera->type == GAN_RADIAL_DISTORTION_1_INV,
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

   /* compute undistorted X/Y coordinates */
   (void)gan_vec2_fill_q ( &X, (CZH*pu->x - CX0*pu->z)/(CFX*pu->z),
                               (CZH*pu->y - CY0*pu->z)/(CFY*pu->z) );

   if ( !compute_distortion ( CK1, CTDR, COA, COB, &X, &dp, error_code ) )
   {
      if ( error_code == NULL )
         gan_err_register ( "add_distortion", GAN_ERROR_FAILURE, "" );

      return GAN_FALSE;
   }

   if ( dp == 0.0 )
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

   /* build distorted image coordinates */
   (void)gan_vec3_fill_q ( p, CX0 + CFX*X.x/dp, CY0 + CFY*X.y/dp, CZH );

   /* success */
   return GAN_TRUE;
}
   
/* function to remove distortion from a point */
static Gan_Bool
 point_remove_distortion ( const Gan_Camera *camera,
                           Gan_Vector3 *p, Gan_Vector3 *pu, Gan_PositionState *ppuprev,
                           int *error_code)
{
   Gan_Vector2 Xd;
   double Rp2, dp;

   /* consistency check */
   gan_err_test_bool ( camera->type == GAN_RADIAL_DISTORTION_1_INV,
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

   /* compute squared radial distance R2 */
   Rp2 = gan_vec2_sqrlen_q(&Xd);
   if ( Rp2 > CTR2 )
   {
      if ( error_code == NULL )
      {
         gan_err_flush_trace();
         gan_err_register ( "point_remove_distortion", GAN_ERROR_OUTSIDE_RANGE,
                            "" );
      }
      else
         *error_code = GAN_ERROR_OUTSIDE_RANGE;

      return GAN_FALSE;
   }

   /* compute distortion scale factor */
   dp = 1.0 + CK1*Rp2;

   /* fill image point with distortion removed */
   (void)gan_vec3_fill_q ( pu, CX0 + CFX*dp*Xd.x, CY0 + CFY*dp*Xd.y, CZH );

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
   gan_err_test_bool ( camera->type == GAN_RADIAL_DISTORTION_1,
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
   gan_err_test_bool ( camera->type == GAN_RADIAL_DISTORTION_1,
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

/* value less than but close to 1 indicating how close you want to get to the
 * model limit before switching to the outer linear model.
 */
#define MODEL_FRACTION 0.95

/**
 * \brief Builds a structure representing a non-linear camera.
 * \param camera Pointer to the camera structure to be filled
 * \param zh Third homogeneous image coordinate
 * \param fx Focal distance measured in x-pixels
 * \param fy Focal distance measured in y-pixels
 * \param x0 x-coordinate of image centre
 * \param y0 y-coordinate of image centre
 * \param K1 Radial distortion parameter
 *
 * Constructs a structure representing a camera, under an
 * inverse non-linear camera model with one parameter of radial
 * distortion, wherein the model is applied to the distorted
 * image to recover the undistorted image.
 * This is the double precision version.
 *
 * \return #GAN_TRUE on successfully building camera structure, or
 *         #GAN_FALSE on failure.
 * \sa gan_cameraf_build_radial_distortion_1_inv().
 */
Gan_Bool
 gan_camera_build_radial_distortion_1_inv ( Gan_Camera *camera,
                                            double zh,
                                            double fx, double fy,
                                            double x0, double y0,
                                            double K1 )
{
   /* copy calibration type and linear camera parameters */
   camera->type = GAN_RADIAL_DISTORTION_1_INV;
   camera->zh = zh; /* 3rd homogeneous image coordinate */
   camera->fx = fx; camera->fy = fy; /* focal distances */
   camera->x0 = x0; camera->y0 = y0; /* image centre coordinates */

   /* copy non-linear camera parameters */
   camera->nonlinear.radial1.K1 = K1; /* radial distortion cubic term */

   /* compute threshold on radial camera coordinate to see if position
      is within valid region */
   if ( K1 < 0.0 )
   {
      double R;

      /* compute threshold on squared radial distance R^2 and distorted
         radial distance d*R */
      CTR2 = -MODEL_FRACTION*MODEL_FRACTION*GAN_ONE_THIRD/K1;
      R = sqrt(CTR2);
      CTDR = (1.0 + K1*CTR2)*R;

      /* compute parameters of outer linear model */
      COA = 1.0 + 3.0*CTR2*K1;
      COB = CTDR - COA*R;
   }
   else
      CTR2 = CTDR = COA = COB = DBL_MAX;

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

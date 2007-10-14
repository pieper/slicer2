/**
 * File:          $RCSfile: cameraf_radial_dist3.c,v $
 * Module:        Functions for camera with 3 radial distortion parameters
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

#include <gandalf/vision/cameraf_radial_dist3.h>
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
#define CK1 camera->nonlinear.radial3.K1
#define CK2 camera->nonlinear.radial3.K2
#define CK3 camera->nonlinear.radial3.K3
#define CTR2 camera->nonlinear.radial3.thres_R2
#define CTDR camera->nonlinear.radial3.thres_dR
#define COA camera->nonlinear.radial3.outer_a
#define COB camera->nonlinear.radial3.outer_b

/* thresholds on change in radial distance R value */
#define R_CHANGE_THRES 1.0e-4F

/* minimum/maximum number of iterations to apply when applying inverse
   distortion */
#define MIN_UND_ITERATIONS   6
#define MAX_UND_ITERATIONS 100

/* threshold on closness of camera coordinates to optical axis */
#define SMALL_DR_THRES 0.01F

/* point projection function */
static Gan_Bool
 point_project ( const Gan_Camera_f *camera,
                 Gan_Vector3_f *X, Gan_Vector3_f *p, Gan_PositionState_f *Xpprev,
                 Gan_Matrix22_f *HX, Gan_Camera_f HC[2],
                 int *error_code )
{
   Gan_Vector2_f x;
   float XZ, YZ, XZ2, YZ2, R2, d;

   /* consistency check */
   gan_err_test_bool ( camera->type == GAN_RADIAL_DISTORTION_3,
                       "point_project", GAN_ERROR_INCOMPATIBLE, "" );

   /* if we are computing Jacobians, the camera coordinates must have Z=1 */
#ifndef NDEBUG
   if ( (HX != NULL || HC != NULL) && X->z != 1.0F )
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

   if ( X->z == 0.0F )
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

   if ( X->z != 1.0F )
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

   /* compute squared radial distance R2 */
   XZ2 = XZ*XZ;
   YZ2 = YZ*YZ; R2 = XZ2 + YZ2;

   /* check if we are at a point outside the distortion model */
   if ( R2 > CTR2 )
   {
      /* use outer linear model */
      float R = (float)sqrt(R2);

      /* compute distortion and image coordinates */
      d = COA + COB/R;
      x.x = CX0 + d*CFX*XZ;
      x.y = CY0 + d*CFY*YZ;

      /* compute Jacobians */
      if ( HX != NULL )
         (void)gan_mat22f_zero_q(HX);

      if ( HC != NULL )
      {
         HC[0].zh = 0.0F;
         HC[0].fx = 0.0F;
         HC[0].fy = 0.0F;
         HC[0].x0 = 0.0F;
         HC[0].y0 = 0.0F;
         HC[0].type = GAN_RADIAL_DISTORTION_3;
         HC[0].nonlinear.radial3.K1 = 0.0F;
         HC[0].nonlinear.radial3.K2 = 0.0F;
         HC[0].nonlinear.radial3.K3 = 0.0F;
         HC[1].zh = 0.0F;
         HC[1].fx = 0.0F;
         HC[1].fy = 0.0F;
         HC[1].x0 = 0.0F;
         HC[1].y0 = 0.0F;
         HC[1].type = GAN_RADIAL_DISTORTION_3;
         HC[1].nonlinear.radial3.K1 = 0.0F;
         HC[1].nonlinear.radial3.K2 = 0.0F;
         HC[1].nonlinear.radial3.K3 = 0.0F;
      }

      /* build homogeneous image coordinates of projected point */
      (void)gan_vec3f_set_parts_q ( p, &x, camera->zh );

      return GAN_TRUE;
   }

   /* calculate distortion and apply it to the image position */
   d = 1.0F + (CK1 + (CK2 + CK3*R2)*R2)*R2;
   x.x = CX0 + d*CFX*XZ;
   x.y = CY0 + d*CFY*YZ;

   /* compute Jacobians */
   if ( HX != NULL )
      (void)gan_mat22f_fill_q ( HX,
                    CFX*(1.0F + CK1*(3.0F*XZ2 + YZ2) + R2*(CK2*(5.0F*XZ2 + YZ2) + R2*CK3*(7.0F*XZ2 + YZ2))),
                    2.0F*CFX*XZ*YZ*(CK1 + R2*(2.0F*CK2 + 3.0F*R2*CK3)),
                    2.0F*CFY*XZ*YZ*(CK1 + R2*(2.0F*CK2 + 3.0F*R2*CK3)),
                    CFY*(1.0F + CK1*(3.0F*YZ2 + XZ2) + R2*(CK2*(5.0F*YZ2 + XZ2) + R2*CK3*(7.0F*YZ2 + XZ2))) );

   if ( HC != NULL )
   {
      HC[0].zh = 0.0F;
      HC[0].fx = d*XZ;
      HC[0].fy = 0.0F;
      HC[0].x0 = 1.0F;
      HC[0].y0 = 0.0F;
      HC[0].type = GAN_RADIAL_DISTORTION_3;
      HC[0].nonlinear.radial3.K1 = CFX*XZ*R2;
      HC[0].nonlinear.radial3.K2 = CFX*XZ*R2*R2;
      HC[0].nonlinear.radial3.K3 = CFX*XZ*R2*R2*R2;
      HC[1].zh = 0.0F;
      HC[1].fx = 0.0F;
      HC[1].fy = d*YZ;
      HC[1].x0 = 0.0F;
      HC[1].y0 = 1.0F;
      HC[1].type = GAN_RADIAL_DISTORTION_3;
      HC[1].nonlinear.radial3.K1 = CFY*YZ*R2;
      HC[1].nonlinear.radial3.K2 = CFY*YZ*R2*R2;
      HC[1].nonlinear.radial3.K3 = CFY*YZ*R2*R2*R2;
   }

   /* build homogeneous image coordinates of projected point */
   (void)gan_vec3f_set_parts_q ( p, &x, camera->zh );

   return GAN_TRUE;
}
   
static Gan_Bool
 compute_distortion ( float K1, float K2, float K3,
                      float thres_R2, float thres_dR,
                      float outer_a, float outer_b, Gan_Vector2_f *Xd,
                      float *dp, int *error_code )
{
   float R, R2, dR = (float)sqrt(gan_vec2f_sqrlen_q(Xd)), deriv, Rdiff;
   int it;

   /* return immediately with zero distortion if the distortion coefficients
      are zero or the distorted radial distance is small */
   if ( (K1 == 0.0F && K2 == 0.0F && K3 == 0.0F) || dR < SMALL_DR_THRES )
   {
      *dp = 1.0F;
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

   /* if initialization takes us outside the range of the camera,
      move the start point back to the middle of the range */
   if ( dR*dR >= thres_R2 )
      R = 0.5F*(float)sqrt(thres_R2);
   else
      /* initialise assuming zero distortion */
      R = dR;

   for ( it = 1; ; it++ )
   {
      R2 = R*R;
      deriv = 1.0F + R2*(3.0F*K1 + R2*(5.0F*K2 + 7.0F*R2*K3));
      if ( deriv == 0.0F )
      {
         if ( error_code == NULL )
         {
            gan_err_flush_trace();
            gan_err_register ( "compute_distortion",
                               GAN_ERROR_DIVISION_BY_ZERO, "" );
         }
         else
            *error_code = GAN_ERROR_DIVISION_BY_ZERO;

         return GAN_FALSE;
      }

      /* compute change to make to R */
      Rdiff = (R*(1.0F + R2*(K1 + R2*(K2 + R2*K3))) - dR)/deriv;
      R -= Rdiff;

      /* check whether the new R is outside the range */
      if ( R < 0.0F || R*R >= thres_R2 )
      {
         if ( error_code == NULL )
         {
            gan_err_flush_trace();
            gan_err_register ( "compute_distortion",
                               GAN_ERROR_OUTSIDE_RANGE, "" );
         }
         else
            *error_code = GAN_ERROR_OUTSIDE_RANGE;

         return GAN_FALSE;
      }

      /* check whether we have converged, observing the minimum
         number of iterations */
      if ( fabs(Rdiff) < R_CHANGE_THRES && it >= MIN_UND_ITERATIONS )
         break;

      /* check whether we have exceeded the maximum number of iterations */
      if ( it > MAX_UND_ITERATIONS )
      {
         if ( error_code == NULL )
         {
            gan_err_flush_trace();
            gan_err_register ( "compute_distortion",
                               GAN_ERROR_NO_CONVERGENCE, "" );
         }
         else
            *error_code = GAN_ERROR_NO_CONVERGENCE;

         return GAN_FALSE;
      }
   }

   /* compute distortion */
   *dp = 1.0F + R2*(K1 + R2*(K2 + R2*K3));

   /* success */
   return GAN_TRUE;
}

/* point back-projection function */
static Gan_Bool
 point_backproject ( const Gan_Camera_f *camera,
                     Gan_Vector3_f *p, Gan_Vector3_f *X, Gan_PositionState_f *pXprev,
                     int *error_code )
{
   Gan_Vector2_f Xd;
   float d;

   /* consistency check */
   gan_err_test_bool ( camera->type == GAN_RADIAL_DISTORTION_3,
                       "point_backproject", GAN_ERROR_INCOMPATIBLE, "" );

   if ( camera->fx == 0.0F || camera->fy == 0.0F || camera->zh == 0.0F )
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
   if ( p->z == 0.0F )
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
   Xd.x = (camera->zh*p->x - camera->x0*p->z)/(camera->fx*p->z);
   Xd.y = (camera->zh*p->y - camera->y0*p->z)/(camera->fy*p->z);

   if ( !compute_distortion ( CK1, CK2, CK3, CTR2, CTDR, COA, COB, &Xd,
                              &d, error_code ) )
   {
      if ( error_code == NULL )
         gan_err_register ( "point_backproject", GAN_ERROR_FAILURE, "" );

      return GAN_FALSE;
   }
                                          
   /* remove distortion from camera coordinates */
   (void)gan_vec2f_divide_i ( &Xd, d );

   /* build scene point */
   gan_vec3f_set_parts_q ( X, &Xd, 1.0F );

   /* success */
   return GAN_TRUE;
}

/* function to add distortion to a point */
static Gan_Bool
 point_add_distortion ( const Gan_Camera_f *camera,
                        Gan_Vector3_f *pu, Gan_Vector3_f *p, Gan_PositionState_f *pupprev,
                        int *error_code )
{
   Gan_Vector2_f x;

   /* consistency check */
   gan_err_test_bool ( camera->type == GAN_RADIAL_DISTORTION_3,
                       "point_add_distortion", GAN_ERROR_INCOMPATIBLE, "" );

#ifndef NDEBUG
   if ( camera->fx == 0.0F || camera->fy == 0.0F || camera->zh == 0.0F )
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

   if ( pu->z == 0.0F )
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
   x = gan_vec2f_fill_s ( camera->zh*pu->x/pu->z, camera->zh*pu->y/pu->z );

   if ( CK1 != 0.0F || CK2 != 0.0F )
   {
      float R2 = gan_sqr_f((x.x - camera->x0)/camera->fx) +
                 gan_sqr_f((x.y - camera->y0)/camera->fy);
      float d;

      /* compute distortion */
      if ( R2 > CTR2 )
         /* use outer linear model */
         d = COA + COB/(float)sqrt(R2);
      else
         /* use distortion model */
         d = 1.0F + R2*(CK1 + R2*(CK2 + R2*CK3));

      x.x = camera->x0 + (x.x-camera->x0)*d;
      x.y = camera->y0 + (x.y-camera->y0)*d;
   }

   /* fill image point with distortion added */
   gan_vec3f_set_parts_q ( p, &x, camera->zh );

   /* success */
   return GAN_TRUE;
}
   
/* function to remove distortion from a point */
static Gan_Bool
 point_remove_distortion ( const Gan_Camera_f *camera,
                           Gan_Vector3_f *p, Gan_Vector3_f *pu, Gan_PositionState_f *ppuprev,
                           int *error_code)
{
   Gan_Vector2_f Xd;
   float d;

   /* consistency check */
   gan_err_test_bool ( camera->type == GAN_RADIAL_DISTORTION_3,
                       "point_remove_distortion", GAN_ERROR_INCOMPATIBLE, "" );

   if ( p->z == 0.0F )
   {
      gan_err_flush_trace();
      gan_err_register ( "point_remove_distortion",
                         GAN_ERROR_DIVISION_BY_ZERO, "" );
      return GAN_FALSE;
   }

   /* compute distorted X/Y coordinates of scene point Xd on plane Z=1 */
   (void)gan_vec2f_fill_q ( &Xd, (CZH*p->x - CX0*p->z)/(CFX*p->z),
                                 (CZH*p->y - CY0*p->z)/(CFY*p->z) );

   if ( !compute_distortion ( CK1, CK2, CK3, CTR2, CTDR, COA, COB, &Xd,
                              &d, error_code ) )
   {
      if ( error_code == NULL )
         gan_err_register ( "remove_distortion", GAN_ERROR_FAILURE, "" );

      return GAN_FALSE;
   }
                                          
   /* fill image point with distortion removed */
   (void)gan_vec3f_fill_q ( pu, CX0 + CFX*Xd.x/d, CY0 + CFY*Xd.y/d, CZH );

   /* success */
   return GAN_TRUE;
}

/* line functions */

/* line projection function */
static Gan_Bool
 line_project ( const Gan_Camera_f *camera,
                Gan_Vector3_f *L, Gan_Vector3_f *l )
{
   Gan_Vector3_f line;

   /* consistency check */
   gan_err_test_bool ( camera->type == GAN_RADIAL_DISTORTION_3,
                       "line_project", GAN_ERROR_INCOMPATIBLE, "" );

   if ( camera->fx == 0.0F || camera->fy == 0.0F || camera->zh == 0.0F )
   {
      gan_err_flush_trace();
      gan_err_register ( "line_project", GAN_ERROR_DIVISION_BY_ZERO, "" );
   }

   /* not implemented */
   gan_err_flush_trace();
   gan_err_register ( "line_project", GAN_ERROR_NOT_IMPLEMENTED, "" );
   return GAN_FALSE;

   /* project line using l = K^-T*L */
   line.x = l->x/camera->fx;
   line.y = l->y/camera->fy;
   line.z = (l->z - line.x*camera->x0 - line.y*camera->y0)/camera->zh;

   /* fill image line */
   *l = line;

   /* success */
   return GAN_TRUE;
}
   
/* line back-projection function */
static Gan_Bool
 line_backproject ( const Gan_Camera_f *camera, Gan_Vector3_f *l, Gan_Vector3_f *L )
{
   Gan_Vector3_f Line;

   /* consistency check */
   gan_err_test_bool ( camera->type == GAN_RADIAL_DISTORTION_3,
                       "line_backproject", GAN_ERROR_INCOMPATIBLE, "" );

   /* not implemented */
   gan_err_flush_trace();
   gan_err_register ( "line_backproject", GAN_ERROR_NOT_IMPLEMENTED, "" );
   return GAN_FALSE;

   Line.x = camera->fx*l->x;
   Line.y = camera->fy*l->y;
   Line.z = l->x*camera->x0 + l->y*camera->y0 + l->z*camera->zh;

   /* fill scene line */
   *L = Line;

   /* success */
   return GAN_TRUE;
}

/* value less than but close to 1 indicating how close you want to get to the
 * model limit before switching to the outer linear model.
 */
#define MODEL_FRACTION 0.95F

/**
 * \brief Builds a structure representing a non-linear camera.
 * \param camera Pointer to the camera structure to be filled
 * \param zh Third homogeneous image coordinate
 * \param fx Focal distance measured in x-pixels
 * \param fy Focal distance measured in y-pixels
 * \param x0 x-coordinate of image centre
 * \param y0 y-coordinate of image centre
 * \param K1 First radial distortion parameter
 * \param K2 Second radial distortion parameter
 * \param K3 Third radial distortion parameter
 *
 * Constructs a structure representing a camera, under a non-linear camera
 * model with three parameters of radial distortion.
 * This is the single precision version.
 *
 * \return #GAN_TRUE on successfully building the camera structure,
 *         #GAN_FALSE on failure.
 * \sa gan_camera_build_radial_distortion_3().
 */
Gan_Bool
 gan_cameraf_build_radial_distortion_3 ( Gan_Camera_f *camera,
                                         float zh, float fx, float fy,
                                         float x0, float y0,
                                         float K1, float K2, float K3 )
{
   int nos;
   Gan_Complex x[3];
   double minR2 = -1.0;

   if ( !gan_cameraf_set_common_fields ( camera, GAN_RADIAL_DISTORTION_3,
                                         zh, fx, fy, x0, y0 ) )
   {
      gan_err_register ( "gan_cameraf_build_radial_distortion_3",
                         GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   /* copy non-linear camera parameters */
   camera->nonlinear.radial3.K1 = K1; /* radial distortion cubic term */
   camera->nonlinear.radial3.K2 = K2; /* radial distortion quintic term */
   camera->nonlinear.radial3.K3 = K3; /* radial distortion septic (?) term */

   /* compute threshold on radial camera coordinate to see if position
      is within valid region */

   /* first solve cubic for R^2 at zeroes of derivative of distortion
      function */
   nos = gan_solve_cubic ( 7.0*K3, 5.0*K2, 3.0*K1, 1.0, x );

   /* find the smallest positive solution for R^2 */
   for ( nos--; nos >= 0; nos-- )
   {
      if ( x[nos].i != 0.0 ) continue;

      if ( x[nos].r > 0.0 )
      {
         if ( minR2 < 0.0 ) minR2 = x[nos].r;
         else if ( x[nos].r < minR2 ) minR2 = x[nos].r;
      }
   }

   if ( minR2 > 0.0 )
   {
      float R;

      /* compute threshold on squared radial distance R^2 and distorted
         radial distance d*R */
      CTR2 = MODEL_FRACTION*MODEL_FRACTION*(float)minR2;
      R = (float)sqrt(CTR2);
      CTDR = (1.0F + CTR2*(K1 + CTR2*(K2 + CTR2*K3)))*R;

      /* compute parameters of outer linear model */
      COA = 1.0F + CTR2*(3.0F*K1 + CTR2*(5.0F*K2 + CTR2*7.0F*K3));
      COB = CTDR - COA*R;
   }
   else
      CTR2 = CTDR = COA = COB = FLT_MAX;

   /* add point projection/backprojection/distortion functions */
   camera->pointf.project           = point_project;
   camera->pointf.backproject       = point_backproject;
   camera->pointf.add_distortion    = point_add_distortion;
   camera->pointf.remove_distortion = point_remove_distortion;

   /* add line projection/backprojection/distortion functions */
   camera->linef.project     = line_project;
   camera->linef.backproject = line_backproject;

   /* success */
   return GAN_TRUE;
}

/**
 * \}
 */

/**
 * \}
 */

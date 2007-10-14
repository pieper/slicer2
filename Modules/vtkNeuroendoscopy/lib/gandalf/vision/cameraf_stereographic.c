/**
 * File:          $RCSfile: cameraf_stereographic.c,v $
 * Module:        Stereographic camera functions
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

#include <gandalf/vision/cameraf_stereographic.h>

/**
 * \addtogroup Vision
 * \{
 */

/**
 * \addtogroup Camera
 * \{
 */

#define CFX camera->fx
#define CFY camera->fy
#define CX0 camera->x0
#define CY0 camera->y0

/* point projection function */
static Gan_Bool
 point_project ( const Gan_Camera_f *camera,
                 Gan_Vector3_f *X, Gan_Vector3_f *p, Gan_PositionState_f *Xpprev,
                 Gan_Matrix22_f *HX, Gan_Camera_f HC[2],
                 int *error_code )
{
   Gan_Vector2_f x;
   float XZ, YZ, XZ2, YZ2, R2, sR2, d;

   /* consistency check */
   gan_err_test_bool ( camera->type == GAN_STEREOGRAPHIC_CAMERA,
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

   /* compute projection */
   XZ2 = XZ*XZ;
   YZ2 = YZ*YZ;
   R2 = XZ2 + YZ2;
   sR2 = (float)sqrt(1.0F+R2);
   d = 2.0F/(1.0F+sR2);

   /* compute image position */
   x.x = CX0 + d*CFX*XZ;
   x.y = CY0 + d*CFY*YZ;

   /* compute Jacobians */
   if ( HX != NULL )
      (void)gan_mat22f_fill_q ( HX,
                                d*CFX*(1.0F - 0.5F*XZ2*d/sR2),
                                -0.5F*CFX*XZ*YZ*d*d/sR2,
                                -0.5F*CFY*XZ*YZ*d*d/sR2,
                                d*CFY*(1.0F - 0.5F*YZ2*d/sR2) );

   if ( HC != NULL )
   {
      gan_cameraf_build_stereographic ( &HC[0], 0.0F, d*XZ, 0.0F, 1.0F, 0.0F);
      gan_cameraf_build_stereographic ( &HC[1], 0.0F, 0.0F, d*YZ, 0.0F, 1.0F);
   }

   /* build homogeneous image coordinates of projected point */
   (void)gan_vec3f_set_parts_q ( p, &x, camera->zh );

   return GAN_TRUE;
}
   
/* point back-projection function */
static Gan_Bool
 point_backproject ( const Gan_Camera_f *camera,
                     Gan_Vector3_f *p, Gan_Vector3_f *X, Gan_PositionState_f *pXprev,
                     int *error_code )
{
   Gan_Vector2_f Xd;
   float a2, k;

   /* consistency check */
   gan_err_test_bool ( camera->type == GAN_STEREOGRAPHIC_CAMERA,
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

   a2 = gan_vec2f_sqrlen_q(&Xd);
   k = 1.0F - 0.25F*a2;

   if ( k == 0.0F )
      gan_vec3f_set_parts_q ( X, &Xd, 0.0F );
   else if ( k < 0.0F )
   {
      /* point behind camera */
      k = -1.0F/k;
      Xd.x *= k; Xd.y *= k;
      gan_vec3f_set_parts_q ( X, &Xd, -1.0F );
      return GAN_TRUE;
   }
   else
      (void)gan_vec2f_divide_i ( &Xd, k );

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
   gan_err_test_bool ( camera->type == GAN_STEREOGRAPHIC_CAMERA,
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

   /* not implemented */
   gan_err_flush_trace();
   gan_err_register ( "point_add_distortion", GAN_ERROR_NOT_IMPLEMENTED, "" );
   return GAN_FALSE;

   /* compute stereographic image coordinates */
   x = gan_vec2f_fill_s ( camera->zh*pu->x/pu->z, camera->zh*pu->y/pu->z );

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
   Gan_Vector2_f xu;

   /* consistency check */
   gan_err_test_bool ( camera->type == GAN_STEREOGRAPHIC_CAMERA,
                       "point_remove_distortion", GAN_ERROR_INCOMPATIBLE, "" );

   if ( p->z == 0.0F )
   {
      gan_err_flush_trace();
      gan_err_register ( "point_remove_distortion",
                         GAN_ERROR_DIVISION_BY_ZERO, "" );
      return GAN_FALSE;
   }

   xu = gan_vec2f_fill_s ( camera->zh*p->x/p->z, camera->zh*p->y/p->z );

   /* not implemented */
   gan_err_flush_trace();
   gan_err_register ( "point_remove_distortion", GAN_ERROR_NOT_IMPLEMENTED,
                      "" );
   return GAN_FALSE;

   /* fill image point with distortion removed */
   gan_vec3f_set_parts_q ( pu, &xu, camera->zh );

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
   gan_err_test_bool ( camera->type == GAN_STEREOGRAPHIC_CAMERA,
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
   gan_err_test_bool ( camera->type == GAN_STEREOGRAPHIC_CAMERA,
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

/**
 * \brief Builds a structure representing a stereographic camera.
 * \param camera Pointer to the camera structure to be filled
 * \param zh Third homogeneous image coordinate
 * \param fx Focal distance measured in x-pixels
 * \param fy Focal distance measured in y-pixels
 * \param x0 x-coordinate of image centre
 * \param y0 y-coordinate of image centre
 *
 * Constructs a structure representing a camera, under the stereographic
 * camera model. This is the single precision version.
 *
 * \return #GAN_TRUE on successfully building the camera structure,
 *         #GAN_FALSE on failure.
 * \sa gan_camera_build_stereographic().
 */
Gan_Bool
 gan_cameraf_build_stereographic ( Gan_Camera_f *camera,
                                   float zh, float fx, float fy,
                                   float x0, float y0 )
{
   if ( !gan_cameraf_set_common_fields ( camera, GAN_STEREOGRAPHIC_CAMERA,
                                        zh, fx, fy, x0, y0 ) )
   {
      gan_err_register ( "gan_cameraf_build_stereographic", GAN_ERROR_FAILURE,
                         "" );
      return GAN_FALSE;
   }

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

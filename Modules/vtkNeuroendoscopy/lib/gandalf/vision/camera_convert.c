/**
 * File:          $RCSfile: camera_convert.c,v $
 * Module:        Camera conversion functions
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

#include <gandalf/vision/camera_convert.h>
#include <gandalf/vision/camera_linear.h>
#include <gandalf/vision/cameraf_linear.h>
#include <gandalf/vision/camera_affine.h>
#include <gandalf/vision/cameraf_affine.h>
#include <gandalf/vision/camera_radial_dist1.h>
#include <gandalf/vision/camera_radial_dist2.h>
#include <gandalf/vision/camera_radial_dist3.h>
#include <gandalf/vision/camera_radial_dist1_inv.h>
#include <gandalf/vision/cameraf_radial_dist1.h>
#include <gandalf/vision/cameraf_radial_dist2.h>
#include <gandalf/vision/cameraf_radial_dist3.h>
#include <gandalf/vision/cameraf_radial_dist1_inv.h>
#include <gandalf/vision/camera_stereographic.h>
#include <gandalf/vision/cameraf_stereographic.h>
#include <gandalf/vision/camera_equidistant.h>
#include <gandalf/vision/cameraf_equidistant.h>
#include <gandalf/vision/camera_sine_law.h>
#include <gandalf/vision/cameraf_sine_law.h>
#include <gandalf/vision/camera_equi_solid_angle.h>
#include <gandalf/vision/cameraf_equi_solid_angle.h>
#include <gandalf/vision/camera_xy_dist4.h>
#include <gandalf/vision/cameraf_xy_dist4.h>
#include <gandalf/vision/camera_cubic_Bspline.h>
#include <gandalf/vision/cameraf_cubic_Bspline.h>
#include <gandalf/vision/camera_cubic_Bspline_inv.h>
#include <gandalf/vision/cameraf_cubic_Bspline_inv.h>
#include <gandalf/common/allocate.h>

/**
 * \addtogroup Vision
 * \{
 */

/**
 * \addtogroup Camera
 * \{
 */

/**
 * \brief Converts camera from double to single precision representation.
 */
Gan_Bool
 gan_cameraf_from_camera_q ( const Gan_Camera *camera,
                             Gan_Camera_f     *cameraf,
                             Gan_CubicBSplineWeightsF **ppweight,
                             Gan_CubicBSplineSupportF **ppsupport )
{
   Gan_Bool result;

   /* initialise weight and support arrays */
   if(ppweight != NULL) *ppweight = NULL;
   if(ppsupport != NULL) *ppsupport = NULL;

   switch ( camera->type )
   {
      case GAN_LINEAR_CAMERA:
        result = gan_cameraf_build_linear ( cameraf,
                                            (float)camera->zh,
                                            (float)camera->fx, (float)camera->fy,
                                            (float)camera->x0, (float)camera->y0 );
        break;

      case GAN_AFFINE_CAMERA:
        result = gan_cameraf_build_affine ( cameraf,
                                            (float)camera->zh,
                                            (float)camera->fx, (float)camera->fy,
                                            (float)camera->x0, (float)camera->y0,
                                            (float)camera->nonlinear.affine.skew,
                                            (float)camera->nonlinear.affine.kyx );
        break;

      case GAN_STEREOGRAPHIC_CAMERA:
        result = gan_cameraf_build_stereographic ( cameraf,
                                                   (float)camera->zh,
                                                   (float)camera->fx, (float)camera->fy,
                                                   (float)camera->x0, (float)camera->y0 );
        break;

      case GAN_EQUIDISTANT_CAMERA:
        result = gan_cameraf_build_equidistant ( cameraf,
                                                 (float)camera->zh,
                                                 (float)camera->fx, (float)camera->fy,
                                                 (float)camera->x0, (float)camera->y0 );
        break;

      case GAN_SINE_LAW_CAMERA:
        result = gan_cameraf_build_sine_law ( cameraf,
                                              (float)camera->zh,
                                              (float)camera->fx, (float)camera->fy,
                                              (float)camera->x0, (float)camera->y0 );
        break;

      case GAN_EQUI_SOLID_ANGLE_CAMERA:
        result = gan_cameraf_build_equi_solid_angle ( cameraf,
                                                      (float)camera->zh,
                                                      (float)camera->fx, (float)camera->fy,
                                                      (float)camera->x0, (float)camera->y0 );
        break;
        
      case GAN_RADIAL_DISTORTION_1:
        result = gan_cameraf_build_radial_distortion_1 ( cameraf,
                                                         (float)camera->zh,
                                                         (float)camera->fx, (float)camera->fy,
                                                         (float)camera->x0, (float)camera->y0,
                                                         (float)camera->nonlinear.radial1.K1 );
        break;

      case GAN_RADIAL_DISTORTION_2:
        result = gan_cameraf_build_radial_distortion_2 ( cameraf,
                                                         (float)camera->zh,
                                                         (float)camera->fx, (float)camera->fy,
                                                         (float)camera->x0, (float)camera->y0,
                                                         (float)camera->nonlinear.radial2.K1,
                                                         (float)camera->nonlinear.radial2.K2 );
        break;
        
      case GAN_RADIAL_DISTORTION_3:
        result = gan_cameraf_build_radial_distortion_3 ( cameraf,
                                                         (float)camera->zh,
                                                         (float)camera->fx, (float)camera->fy,
                                                         (float)camera->x0, (float)camera->y0,
                                                         (float)camera->nonlinear.radial3.K1,
                                                         (float)camera->nonlinear.radial3.K2,
                                                         (float)camera->nonlinear.radial3.K3 );
        break;
        
      case GAN_RADIAL_DISTORTION_1_INV:
        result = gan_cameraf_build_radial_distortion_1_inv ( cameraf,
                                                             (float)camera->zh,
                                                             (float)camera->fx, (float)camera->fy,
                                                             (float)camera->x0, (float)camera->y0,
                                                             (float)camera->nonlinear.radial1.K1 );
        break;

      case GAN_XY_DISTORTION_4:
        result = gan_cameraf_build_xy_distortion_4 ( cameraf,
                                                     (float)camera->zh,
                                                     (float)camera->fx, (float)camera->fy,
                                                     (float)camera->x0, (float)camera->y0,
                                                     (float)camera->nonlinear.xydist4.cxx,
                                                     (float)camera->nonlinear.xydist4.cxy,
                                                     (float)camera->nonlinear.xydist4.cyx,
                                                     (float)camera->nonlinear.xydist4.cyy );
        break;

      case GAN_CUBIC_BSPLINE_CAMERA:
      case GAN_CUBIC_BSPLINE_CAMERA_INV:
      {
         int r, c;

         if(ppweight == NULL || ppsupport == NULL)
         {
            gan_err_flush_trace();
            gan_err_register ( "gan_cameraf_from_camera_q", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
            return GAN_FALSE;
         }

         *ppweight = gan_malloc_object(Gan_CubicBSplineWeightsF);
         *ppsupport = gan_malloc_object(Gan_CubicBSplineSupportF);
         if((*ppweight) == NULL || (*ppsupport) == NULL)
         {
            gan_err_flush_trace();
            gan_err_register_with_number ( "gan_cameraf_from_camera_q", GAN_ERROR_MALLOC_FAILED, "", sizeof(**ppweight) );
            return GAN_FALSE;
         }
         
         /* first convert weights */
         for(r = (1<<GAN_CUBIC_BSPLINE_CAMERA_LEVEL); r>=0; r--)
            for(c = (1<<GAN_CUBIC_BSPLINE_CAMERA_LEVEL); c>=0; c--)
            {
               (*ppweight)->weight[r][c].x = (float)camera->nonlinear.cbspline.weight[r][c].x;
               (*ppweight)->weight[r][c].y = (float)camera->nonlinear.cbspline.weight[r][c].y;
            }

         if(camera->type == GAN_CUBIC_BSPLINE_CAMERA)
            result = gan_cameraf_build_cubic_Bspline ( cameraf,
                                                       (float)camera->zh,
                                                       (float)camera->fx, (float)camera->fy,
                                                       (float)camera->x0, (float)camera->y0,
                                                       (float)camera->nonlinear.cbspline.K.xy,
                                                       (float)camera->nonlinear.cbspline.K.yx,
                                                       (float)camera->nonlinear.cbspline.K.zx,
                                                       (float)camera->nonlinear.cbspline.K.zy,
                                                       (*ppweight)->weight, *ppsupport);
         else
            result = gan_cameraf_build_cubic_Bspline_inv ( cameraf,
                                                           (float)camera->zh,
                                                           (float)camera->fx, (float)camera->fy,
                                                           (float)camera->x0, (float)camera->y0,
                                                           (float)camera->nonlinear.cbspline.K.xy,
                                                           (float)camera->nonlinear.cbspline.K.yx,
                                                           (float)camera->nonlinear.cbspline.K.zx,
                                                           (float)camera->nonlinear.cbspline.K.zy,
                                                           (*ppweight)->weight, *ppsupport );
      }
      break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_cameraf_from_camera_q", GAN_ERROR_ILLEGAL_TYPE, "" );
        return GAN_FALSE;
   }

   if ( !result )
   {
      gan_err_register ( "gan_cameraf_from_camera_q", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   /* success */
   return GAN_TRUE;
}

/**
 * \brief Converts camera from single to double precision representation.
 */
Gan_Bool
 gan_camera_from_cameraf_q ( const Gan_Camera_f *cameraf,
                             Gan_Camera         *camera,
                             Gan_CubicBSplineWeights **ppweight,
                             Gan_CubicBSplineSupport **ppsupport )
{
   Gan_Bool result;

   /* initialise weight and support arrays */
   if(ppweight != NULL) *ppweight = NULL;
   if(ppsupport != NULL) *ppsupport = NULL;

   switch ( cameraf->type )
   {
      case GAN_LINEAR_CAMERA:
        result = gan_camera_build_linear ( camera,
                                           (double)cameraf->zh,
                                           (double)cameraf->fx, (double)cameraf->fy,
                                           (double)cameraf->x0, (double)cameraf->y0 );
        break;

      case GAN_AFFINE_CAMERA:
        result = gan_camera_build_affine ( camera,
                                           (double)cameraf->zh,
                                           (double)cameraf->fx, (double)cameraf->fy,
                                           (double)cameraf->x0, (double)cameraf->y0,
                                           (double)cameraf->nonlinear.affine.skew,
                                           (double)cameraf->nonlinear.affine.kyx );
        break;

      case GAN_STEREOGRAPHIC_CAMERA:
        result = gan_camera_build_stereographic ( camera,
                                                  (double)cameraf->zh,
                                                  (double)cameraf->fx, (double)cameraf->fy,
                                                  (double)cameraf->x0, (double)cameraf->y0 );
        break;

      case GAN_EQUIDISTANT_CAMERA:
        result = gan_camera_build_equidistant ( camera,
                                                (double)cameraf->zh,
                                                (double)cameraf->fx, (double)cameraf->fy,
                                                (double)cameraf->x0, (double)cameraf->y0 );
        break;

      case GAN_SINE_LAW_CAMERA:
        result = gan_camera_build_sine_law ( camera,
                                             (double)cameraf->zh,
                                             (double)cameraf->fx, (double)cameraf->fy,
                                             (double)cameraf->x0, (double)cameraf->y0 );
        break;

      case GAN_EQUI_SOLID_ANGLE_CAMERA:
        result = gan_camera_build_equi_solid_angle ( camera,
                                                     (double)cameraf->zh,
                                                     (double)cameraf->fx, (double)cameraf->fy,
                                                     (double)cameraf->x0, (double)cameraf->y0 );
        break;

      case GAN_RADIAL_DISTORTION_1:
        result = gan_camera_build_radial_distortion_1 ( camera,
                                                        (double)cameraf->zh,
                                                        (double)cameraf->fx, (double)cameraf->fy,
                                                        (double)cameraf->x0, (double)cameraf->y0,
                                                        (double)cameraf->nonlinear.radial1.K1 );
        break;

      case GAN_RADIAL_DISTORTION_2:
        result = gan_camera_build_radial_distortion_2 ( camera,
                                                        (double)cameraf->zh,
                                                        (double)cameraf->fx, (double)cameraf->fy,
                                                        (double)cameraf->x0, (double)cameraf->y0,
                                                        (double)cameraf->nonlinear.radial2.K1,
                                                        (double)cameraf->nonlinear.radial2.K2 );
        break;
        
      case GAN_RADIAL_DISTORTION_3:
        result = gan_camera_build_radial_distortion_3 ( camera,
                                                        (double)cameraf->zh,
                                                        (double)cameraf->fx, (double)cameraf->fy,
                                                        (double)cameraf->x0, (double)cameraf->y0,
                                                        (double)cameraf->nonlinear.radial3.K1,
                                                        (double)cameraf->nonlinear.radial3.K2,
                                                        (double)cameraf->nonlinear.radial3.K3 );
        break;
        
      case GAN_RADIAL_DISTORTION_1_INV:
        result = gan_camera_build_radial_distortion_1_inv ( camera,
                                                            (double)cameraf->zh,
                                                            (double)cameraf->fx, (double)cameraf->fy,
                                                            (double)cameraf->x0, (double)cameraf->y0,
                                                            (double)cameraf->nonlinear.radial1.K1 );
        break;

      case GAN_XY_DISTORTION_4:
        result = gan_camera_build_xy_distortion_4 ( camera,
                                                    (double)cameraf->zh,
                                                    (double)cameraf->fx, (double)cameraf->fy,
                                                    (double)cameraf->x0, (double)cameraf->y0,
                                                    (double)cameraf->nonlinear.xydist4.cxx,
                                                    (double)cameraf->nonlinear.xydist4.cxy,
                                                    (double)cameraf->nonlinear.xydist4.cyx,
                                                    (double)cameraf->nonlinear.xydist4.cyy );
        break;

      case GAN_CUBIC_BSPLINE_CAMERA:
      case GAN_CUBIC_BSPLINE_CAMERA_INV:
      {
         int r, c;

         if(ppweight == NULL || ppsupport == NULL)
         {
            gan_err_flush_trace();
            gan_err_register ( "gan_camera_from_cameraf_q", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
            return GAN_FALSE;
         }

         *ppweight = gan_malloc_object(Gan_CubicBSplineWeights);
         *ppsupport = gan_malloc_object(Gan_CubicBSplineSupport);
         if((*ppweight) == NULL || (*ppsupport) == NULL)
         {
            gan_err_flush_trace();
            gan_err_register_with_number ( "gan_camera_from_cameraf_q", GAN_ERROR_MALLOC_FAILED, "", sizeof(**ppweight) );
            return GAN_FALSE;
         }
         
         /* first convert weights */
         for(r = (1<<GAN_CUBIC_BSPLINE_CAMERA_LEVEL); r>=0; r--)
            for(c = (1<<GAN_CUBIC_BSPLINE_CAMERA_LEVEL); c>=0; c--)
            {
               (*ppweight)->weight[r][c].x = (double)cameraf->nonlinear.cbspline.weight[r][c].x;
               (*ppweight)->weight[r][c].y = (double)cameraf->nonlinear.cbspline.weight[r][c].y;
            }

         if(cameraf->type == GAN_CUBIC_BSPLINE_CAMERA)
            result = gan_camera_build_cubic_Bspline ( camera,
                                                      (double)cameraf->zh,
                                                      (double)cameraf->fx, (double)cameraf->fy,
                                                      (double)cameraf->x0, (double)cameraf->y0,
                                                      (double)cameraf->nonlinear.cbspline.K.xy,
                                                      (double)cameraf->nonlinear.cbspline.K.yx,
                                                      (double)cameraf->nonlinear.cbspline.K.zx,
                                                      (double)cameraf->nonlinear.cbspline.K.zy,
                                                      (*ppweight)->weight,
                                                      *ppsupport);
         else
            result = gan_camera_build_cubic_Bspline_inv ( camera,
                                                          (double)cameraf->zh,
                                                          (double)cameraf->fx, (double)cameraf->fy,
                                                          (double)cameraf->x0, (double)cameraf->y0,
                                                          (double)cameraf->nonlinear.cbspline.K.xy,
                                                          (double)cameraf->nonlinear.cbspline.K.yx,
                                                          (double)cameraf->nonlinear.cbspline.K.zx,
                                                          (double)cameraf->nonlinear.cbspline.K.zy,
                                                          (*ppweight)->weight,
                                                          *ppsupport);
      }
      break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_camera_from_cameraf_q", GAN_ERROR_ILLEGAL_TYPE, "" );
        return GAN_FALSE;
   }

   if ( !result )
   {
      gan_err_register ( "gan_camera_from_cameraf_q", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   /* success */
   return GAN_TRUE;
}

/**
 * \brief Converts camera from double to single precision representation.
 */
Gan_Camera_f
 gan_cameraf_from_camera_s ( const Gan_Camera *camera,
                             Gan_CubicBSplineWeightsF **ppweight,
                             Gan_CubicBSplineSupportF **ppsupport )
{
   Gan_Camera_f cameraf;

   if ( !gan_cameraf_from_camera_q ( camera, &cameraf, ppweight, ppsupport ) )
   {
      gan_err_register ( "gan_cameraf_from_camera_s", GAN_ERROR_FAILURE, "" );
      assert(0);
   }

   return cameraf;
}

/**
 * \brief Converts camera from single to double precision representation.
 */
Gan_Camera
 gan_camera_from_cameraf_s ( const Gan_Camera_f *cameraf,
                             Gan_CubicBSplineWeights **ppweight,
                             Gan_CubicBSplineSupport **ppsupport )
{
   Gan_Camera camera;

   if ( !gan_camera_from_cameraf_q ( cameraf, &camera, ppweight, ppsupport ) )
   {
      gan_err_register ( "gan_camera_from_cameraf_s", GAN_ERROR_FAILURE, "" );
      assert(0);
   }

   return camera;
}

/**
 * \}
 */

/**
 * \}
 */

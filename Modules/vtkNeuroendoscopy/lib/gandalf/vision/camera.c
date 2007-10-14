/**
 * File:          $RCSfile: camera.c,v $
 * Module:        Camera definitions and building functions
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

#include <stdarg.h>
#include <gandalf/vision/camera.h>
#include <gandalf/vision/camera_linear.h>
#include <gandalf/vision/camera_affine.h>
#include <gandalf/vision/camera_radial_dist1.h>
#include <gandalf/vision/camera_radial_dist2.h>
#include <gandalf/vision/camera_radial_dist3.h>
#include <gandalf/vision/camera_stereographic.h>
#include <gandalf/vision/camera_equidistant.h>
#include <gandalf/vision/camera_sine_law.h>
#include <gandalf/vision/camera_equi_solid_angle.h>
#include <gandalf/vision/camera_xy_dist4.h>
#include <gandalf/vision/camera_radial_dist1_inv.h>
#include <gandalf/vision/camera_cubic_Bspline.h>
#include <gandalf/vision/camera_cubic_Bspline_inv.h>
#include <gandalf/common/numerics.h>

/**
 * \addtogroup Vision
 * \{
 */

/**
 * \defgroup Camera Camera geometric/photometric transformations
 * \{
 */

/**
 * \brief Constructs a structure representing a camera.
 * \param camera Pointer to the camera structure to be filled
 * \param type Type of camera e.g. #GAN_LINEAR_CAMERA
 * \param zh Third homogeneous image coordinate
 * \param fx Focal distance measured in x-pixels
 * \param fy Focal distance measured in y-pixels
 * \param x0 x-coordinate of image centre
 * \param y0 y-coordinate of image centre
 * \param ... Extra arguments depending on camera type
 * \return #GAN_TRUE on successfully building camera structure,
 *         #GAN_FALSE on failure.
 *
 * Constructs a structure representing a camera.
 *
 * \sa gan_cameraf_build_va().
 */
Gan_Bool
 gan_camera_build_va ( Gan_Camera *camera,
                       Gan_CameraType type,
                       double zh, double fx, double fy,
                       double x0, double y0, ... )
{
   va_list ap;
   Gan_Bool result;

   va_start ( ap, y0 );
   switch ( type )
   {
      case GAN_LINEAR_CAMERA:
        result = gan_camera_build_linear ( camera, zh, fx, fy, x0, y0 );
        break;

      case GAN_AFFINE_CAMERA:
      {
         double skew = va_arg ( ap, double );
         double kyx  = va_arg ( ap, double );

         result = gan_camera_build_affine ( camera, zh, fx, fy, x0, y0, skew, kyx );
      }
      break;

      case GAN_RADIAL_DISTORTION_1:
      {
         double K1 = va_arg ( ap, double );

         result = gan_camera_build_radial_distortion_1 ( camera, zh, fx, fy, x0, y0, K1 );
      }
      break;

      case GAN_RADIAL_DISTORTION_2:
      {
         double K1 = va_arg ( ap, double );
         double K2 = va_arg ( ap, double );

         result = gan_camera_build_radial_distortion_2 ( camera, zh, fx, fy, x0, y0, K1, K2 );
      }
      break;

      case GAN_RADIAL_DISTORTION_3:
      {
         double K1 = va_arg ( ap, double );
         double K2 = va_arg ( ap, double );
         double K3 = va_arg ( ap, double );

         result = gan_camera_build_radial_distortion_3 ( camera, zh, fx, fy, x0, y0, K1, K2, K3 );
      }
      break;

      case GAN_RADIAL_DISTORTION_1_INV:
      {
         double K1 = va_arg ( ap, double );

         result = gan_camera_build_radial_distortion_1_inv ( camera, zh, fx, fy, x0, y0, K1 );
      }
      break;

      case GAN_STEREOGRAPHIC_CAMERA:
        result = gan_camera_build_stereographic ( camera, zh, fx, fy, x0, y0 );
        break;

      case GAN_EQUIDISTANT_CAMERA:
        result = gan_camera_build_equidistant ( camera, zh, fx, fy, x0, y0 );
        break;

      case GAN_SINE_LAW_CAMERA:
        result = gan_camera_build_sine_law ( camera, zh, fx, fy, x0, y0 );
        break;

      case GAN_EQUI_SOLID_ANGLE_CAMERA:
        result = gan_camera_build_equi_solid_angle ( camera, zh, fx, fy, x0, y0 );
        break;

      case GAN_XY_DISTORTION_4:
      {
         double cxx = va_arg ( ap, double );
         double cxy = va_arg ( ap, double );
         double cyx = va_arg ( ap, double );
         double cyy = va_arg ( ap, double );

         result = gan_camera_build_xy_distortion_4 ( camera, zh, fx, fy,
                                                     x0, y0,
                                                     cxx, cxy, cyx, cyy );
      }
      break;

      case GAN_CUBIC_BSPLINE_CAMERA:
      {
         double skew = va_arg ( ap, double );
         double kyx  = va_arg ( ap, double );
         double kzx  = va_arg ( ap, double );
         double kzy  = va_arg ( ap, double );
         Gan_CubicBSplineWeightBlock *weight  = va_arg ( ap, Gan_CubicBSplineWeightBlock *);
         Gan_CubicBSplineSupport *support  = va_arg ( ap, Gan_CubicBSplineSupport *);
         
         result = gan_camera_build_cubic_Bspline ( camera, zh, fx, fy, x0, y0, skew, kyx, kzx, kzy, weight, support );
      }
      break;

      case GAN_CUBIC_BSPLINE_CAMERA_INV:
      {
         double skew = va_arg ( ap, double );
         double kyx  = va_arg ( ap, double );
         double kzx  = va_arg ( ap, double );
         double kzy  = va_arg ( ap, double );
         Gan_CubicBSplineWeightBlock *weight  = va_arg ( ap, Gan_CubicBSplineWeightBlock *);
         Gan_CubicBSplineSupport *support  = va_arg ( ap, Gan_CubicBSplineSupport *);

         result = gan_camera_build_cubic_Bspline_inv ( camera, zh, fx, fy, x0, y0, skew, kyx, kzx, kzy, weight, support );
      }
      break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_camera_build_va", GAN_ERROR_ILLEGAL_TYPE, "" );
        return GAN_FALSE;
   }
   
   va_end ( ap );

   if ( !result )
   {
      gan_err_register ( "gan_camera_build_va", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   /* success */
   return GAN_TRUE;
}

#if 0
/**
 * \brief Apply Radial Basis Function adjustment
 */
Gan_Bool 
 gan_rbf_apply_adjustment ( Gan_Camera *camera,
                            Gan_Vector3 *p,
                            Gan_Matrix22 *HX, Gan_Camera HC[2],
                            int *error_code )
{
   int irbf;
   double dist_squared, sumx=0.0, sumy=0.0, weight, weight_sum=0.0;
   Gan_RadialBasisFunction *prbf;

   for ( irbf = camera->nrbf-1, prbf = &camera->rbf[0]; irbf >= 0;
         irbf--, prbf++ )
   {
      dist_squared =   gan_sqr_d((p->x - camera->rbf[irbf].x)
                                 *camera->fy_over_fx)
                     + gan_sqr_d(p->y - camera->rbf[irbf].y);
      if ( dist_squared < prbf->dist2_thres )
      {
         weight = prbf->weight/(prbf->normaliser+dist_squared);
         sumx += weight*prbf->dx;
         sumy += weight*prbf->dy;
         weight_sum += weight;
      }
   }

   if ( weight_sum == 0.0 )
      /* return without change */
      return GAN_TRUE;
      
   p->x += sumx/weight_sum;
   p->y += sumy/weight_sum;
   
   return GAN_TRUE;
}

/**
 * \brief Undo Radial Basis Function adjustment
 */
Gan_Bool 
 gan_rbf_undo_adjustment ( Gan_Camera *camera,
                           Gan_Vector3 *p,
                           int *error_code )
{
   int irbf;
   double dist_squared, sumx=0.0, sumy=0.0, weight, weight_sum=0.0;
   Gan_RadialBasisFunction *prbf;

   for ( irbf = camera->nrbf-1, prbf = &camera->rbf[0]; irbf >= 0;
         irbf--, prbf++ )
   {
      dist_squared =   gan_sqr_d((p->x - camera->rbf[irbf].x)/camera->fx)
                     + gan_sqr_d((p->y - camera->rbf[irbf].y)/camera->fy);
      if ( dist_squared < prbf->dist2_thres )
      {
         weight = prbf->weight/(prbf->normaliser+dist_squared);
         sumx += weight*prbf->dx;
         sumy += weight*prbf->dy;
         weight_sum += weight;
      }
   }

   if ( weight_sum == 0.0 )
      /* return without change */
      return GAN_TRUE;
      
   p->x += sumx/weight_sum;
   p->y += sumy/weight_sum;
   
   return GAN_TRUE;
}
#endif

/**
 * \brief Builds the internals of a camera.
 * \param camera Pointer to the camera structure to be filled
 * \return #GAN_TRUE on successfully internalising camera structure,
 *         #GAN_FALSE on failure.
 *
 * Builds the internal thresholds and callbacks of a camera given that the
 * type and parameters have already been set.
 *
 * \sa gan_cameraf_internalize().
 */
Gan_Bool
 gan_camera_internalize ( Gan_Camera *camera )
{
   Gan_Bool result;

   switch ( camera->type )
   {
      case GAN_LINEAR_CAMERA:
        result = gan_camera_build_linear ( camera, camera->zh,
                                           camera->fx, camera->fy,
                                           camera->x0, camera->y0 );
        break;

      case GAN_AFFINE_CAMERA:
        result = gan_camera_build_affine ( camera, camera->zh,
                                           camera->fx, camera->fy,
                                           camera->x0, camera->y0,
                                           camera->nonlinear.affine.skew, camera->nonlinear.affine.kyx );
        break;

      case GAN_RADIAL_DISTORTION_1:
        result = gan_camera_build_radial_distortion_1 (
                     camera, camera->zh,
                     camera->fx, camera->fy,
                     camera->x0, camera->y0,
                     camera->nonlinear.radial1.K1 );
        break;
        
      case GAN_RADIAL_DISTORTION_2:
        result = gan_camera_build_radial_distortion_2 (
                     camera, camera->zh,
                     camera->fx, camera->fy,
                     camera->x0, camera->y0,
                     camera->nonlinear.radial2.K1,
                     camera->nonlinear.radial2.K2 );
        break;
        
      case GAN_RADIAL_DISTORTION_3:
        result = gan_camera_build_radial_distortion_3 (
                     camera, camera->zh,
                     camera->fx, camera->fy,
                     camera->x0, camera->y0,
                     camera->nonlinear.radial3.K1,
                     camera->nonlinear.radial3.K2,
                     camera->nonlinear.radial3.K3 );
        break;
        
      case GAN_RADIAL_DISTORTION_1_INV:
        result = gan_camera_build_radial_distortion_1_inv (
                     camera, camera->zh,
                     camera->fx, camera->fy,
                     camera->x0, camera->y0,
                     camera->nonlinear.radial1.K1 );
        break;
        
      case GAN_STEREOGRAPHIC_CAMERA:
        result = gan_camera_build_stereographic ( camera, camera->zh,
                                                  camera->fx, camera->fy,
                                                  camera->x0, camera->y0 );
        break;

      case GAN_EQUIDISTANT_CAMERA:
        result = gan_camera_build_equidistant ( camera, camera->zh,
                                                camera->fx, camera->fy,
                                                camera->x0, camera->y0 );
        break;

      case GAN_SINE_LAW_CAMERA:
        result = gan_camera_build_sine_law ( camera, camera->zh,
                                             camera->fx, camera->fy,
                                             camera->x0, camera->y0 );
        break;

      case GAN_EQUI_SOLID_ANGLE_CAMERA:
        result = gan_camera_build_equi_solid_angle ( camera, camera->zh,
                                                     camera->fx, camera->fy,
                                                     camera->x0, camera->y0 );
        break;

      case GAN_XY_DISTORTION_4:
        result = gan_camera_build_xy_distortion_4 (
                     camera, camera->zh,
                     camera->fx, camera->fy,
                     camera->x0, camera->y0,
                     camera->nonlinear.xydist4.cxx,
                     camera->nonlinear.xydist4.cxy,
                     camera->nonlinear.xydist4.cyx,
                     camera->nonlinear.xydist4.cyy );
        break;

      case GAN_CUBIC_BSPLINE_CAMERA:
        result = gan_camera_build_cubic_Bspline (
                     camera, camera->zh,
                     camera->fx, camera->fy,
                     camera->x0, camera->y0,
                     camera->zh*camera->nonlinear.cbspline.skew,
                     camera->zh*camera->nonlinear.cbspline.kyx,
                     camera->zh*camera->nonlinear.cbspline.kzx,
                     camera->zh*camera->nonlinear.cbspline.kzy,
                     camera->nonlinear.cbspline.weight,
                     camera->nonlinear.cbspline.support );
        break;

      case GAN_CUBIC_BSPLINE_CAMERA_INV:
        result = gan_camera_build_cubic_Bspline_inv (
                     camera, camera->zh,
                     camera->fx, camera->fy,
                     camera->x0, camera->y0,
                     camera->zh*camera->nonlinear.cbspline.skew,
                     camera->zh*camera->nonlinear.cbspline.kyx,
                     camera->zh*camera->nonlinear.cbspline.kzx,
                     camera->zh*camera->nonlinear.cbspline.kzy,
                     camera->nonlinear.cbspline.weight,
                     camera->nonlinear.cbspline.support );
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_camera_internalize", GAN_ERROR_ILLEGAL_TYPE, "" );
        return GAN_FALSE;
   }
   
   if ( !result )
   {
      gan_err_register ( "gan_camera_internalize", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   /* success */
   return GAN_TRUE;
}

/**
 * \brief Checks whether two cameras have identical parameters
 * \param camera1 Pointer to first camera
 * \param camera2 Pointer to second camera
 * \return #GAN_TRUE if they have the same parameters,
 *         #GAN_FALSE if they don't.
 */
Gan_Bool
 gan_camera_identical ( const Gan_Camera *camera1, const Gan_Camera *camera2 )
{
   if(camera1->type != camera2->type)
      return GAN_FALSE;

   if(camera1->zh != camera2->zh) return GAN_FALSE;
   if(camera1->fx != camera2->fx) return GAN_FALSE;
   if(camera1->fy != camera2->fy) return GAN_FALSE;
   if(camera1->x0 != camera2->x0) return GAN_FALSE;
   if(camera1->y0 != camera2->y0) return GAN_FALSE;
   
   switch ( camera1->type )
   {
      case GAN_LINEAR_CAMERA:
        break;

      case GAN_AFFINE_CAMERA:
        if(camera1->nonlinear.affine.skew != camera2->nonlinear.affine.skew) return GAN_FALSE;
        if(camera1->nonlinear.affine.kyx  != camera2->nonlinear.affine.kyx)  return GAN_FALSE;
        break;
        
      case GAN_RADIAL_DISTORTION_1:
      case GAN_RADIAL_DISTORTION_1_INV:
        if(camera1->nonlinear.radial1.K1 != camera2->nonlinear.radial1.K1) return GAN_FALSE;
        break;
        
      case GAN_RADIAL_DISTORTION_2:
        if(camera1->nonlinear.radial2.K1 != camera2->nonlinear.radial2.K1) return GAN_FALSE;
        if(camera1->nonlinear.radial2.K2 != camera2->nonlinear.radial2.K2) return GAN_FALSE;
        break;
        
      case GAN_RADIAL_DISTORTION_3:
        if(camera1->nonlinear.radial3.K1 != camera2->nonlinear.radial3.K1) return GAN_FALSE;
        if(camera1->nonlinear.radial3.K2 != camera2->nonlinear.radial3.K2) return GAN_FALSE;
        if(camera1->nonlinear.radial3.K3 != camera2->nonlinear.radial3.K3) return GAN_FALSE;
        break;
        
      case GAN_XY_DISTORTION_4:
        if(camera1->nonlinear.xydist4.cxx != camera2->nonlinear.xydist4.cxx) return GAN_FALSE;
        if(camera1->nonlinear.xydist4.cxy != camera2->nonlinear.xydist4.cxy) return GAN_FALSE;
        if(camera1->nonlinear.xydist4.cyx != camera2->nonlinear.xydist4.cyx) return GAN_FALSE;
        if(camera1->nonlinear.xydist4.cyy != camera2->nonlinear.xydist4.cyy) return GAN_FALSE;
        break;
        
      case GAN_FIXED_CORNER_WARP:
      case GAN_FIXED_CORNER_WARP_INV:
        if(camera1->nonlinear.fcwarp.xla != camera2->nonlinear.fcwarp.xla) return GAN_FALSE;
        if(camera1->nonlinear.fcwarp.xlp != camera2->nonlinear.fcwarp.xlp) return GAN_FALSE;
        if(camera1->nonlinear.fcwarp.xha != camera2->nonlinear.fcwarp.xha) return GAN_FALSE;
        if(camera1->nonlinear.fcwarp.xhp != camera2->nonlinear.fcwarp.xhp) return GAN_FALSE;
        if(camera1->nonlinear.fcwarp.yla != camera2->nonlinear.fcwarp.yla) return GAN_FALSE;
        if(camera1->nonlinear.fcwarp.ylp != camera2->nonlinear.fcwarp.ylp) return GAN_FALSE;
        if(camera1->nonlinear.fcwarp.yha != camera2->nonlinear.fcwarp.yha) return GAN_FALSE;
        if(camera1->nonlinear.fcwarp.yhp != camera2->nonlinear.fcwarp.yhp) return GAN_FALSE;
        if(camera1->nonlinear.fcwarp.axx != camera2->nonlinear.fcwarp.axx) return GAN_FALSE;
        if(camera1->nonlinear.fcwarp.axy != camera2->nonlinear.fcwarp.axy) return GAN_FALSE;
        if(camera1->nonlinear.fcwarp.ayx != camera2->nonlinear.fcwarp.ayx) return GAN_FALSE;
        if(camera1->nonlinear.fcwarp.ayy != camera2->nonlinear.fcwarp.ayy) return GAN_FALSE;
        break;
        
      case GAN_CUBIC_BSPLINE_CAMERA:
      case GAN_CUBIC_BSPLINE_CAMERA_INV:
      {
         int r, c;

         if(camera1->nonlinear.cbspline.skew != camera2->nonlinear.cbspline.skew) return GAN_FALSE;
         if(camera1->nonlinear.cbspline.kyx != camera2->nonlinear.cbspline.kyx) return GAN_FALSE;
         if(camera1->nonlinear.cbspline.kzx != camera2->nonlinear.cbspline.kzx) return GAN_FALSE;
         if(camera1->nonlinear.cbspline.kzy != camera2->nonlinear.cbspline.kzy) return GAN_FALSE;
         for(r = 1<<GAN_CUBIC_BSPLINE_CAMERA_LEVEL; r >= 0; r--)
            for(c = 1<<GAN_CUBIC_BSPLINE_CAMERA_LEVEL; c >= 0; c--)
            {
               if(camera1->nonlinear.cbspline.weight[r][c].x != camera2->nonlinear.cbspline.weight[r][c].x) return GAN_FALSE;
               if(camera1->nonlinear.cbspline.weight[r][c].y != camera2->nonlinear.cbspline.weight[r][c].y) return GAN_FALSE;
            }
      }
      break;
        
      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_camera_identical", GAN_ERROR_ILLEGAL_TYPE, "" );
        return GAN_FALSE;
   }

   /* all tests passed, so the cameras ARE identical */
   return GAN_TRUE;
}

/**
 * \brief Fills and returns a 3x3 upper triangular camera matrix.
 * \param camera Pointer to the camera structure to be read
 *
 * Fills and returns a 3x3 upper triangular camera matrix using the linear
 * parameters of the provided camera. Gandalf only stores lower triangular
 * matrices explicitly, so the transpose camera matrix is actually returned.
 *
 * Double precision version of gan_cameraf_fill_matrix_s().
 *
 * \return The filled 3x3 upper triangular matrix.
 */
Gan_SquMatrix33
 gan_camera_fill_matrix_s ( const Gan_Camera *camera )
{
   return gan_ltmat33_fill_s ( camera->fx,
                                      0.0, camera->fy,
                               camera->x0, camera->y0, camera->zh );
}

/**
 * \brief Fills and returns a full 3x3 camera matrix.
 * \param camera Pointer to the camera structure to be read
 *
 * Fills and returns a 3x3 camera matrix using the linear
 * parameters of the provided camera.
 *
 * Double precision version of gan_cameraf_fill_fullmatrix_s().
 *
 * \return The filled 3x3 matrix.
 */
Gan_Matrix33
 gan_camera_fill_fullmatrix_s ( const Gan_Camera *camera )
{
   switch(camera->type)
   {
      case GAN_CUBIC_BSPLINE_CAMERA:
      case GAN_CUBIC_BSPLINE_CAMERA_INV:
        return camera->nonlinear.cbspline.K;
        break;

      case GAN_AFFINE_CAMERA:
        return gan_mat33_fill_s ( camera->fx,                   camera->nonlinear.affine.skew, camera->x0,
                                  camera->nonlinear.affine.kyx, camera->fy,                    camera->y0,
                                  0.0,                          0.0,                           camera->zh );
        break;

      default:
        return gan_mat33_fill_s ( camera->fx, 0.0, camera->x0,
                                  0.0, camera->fy, camera->y0,
                                  0.0,        0.0, camera->zh );
        break;
   }

   /* shouldn't get here */
   return gan_mat33_zero_s();
}

/**
 * \brief Set common fields of camera structure
 *
 * Not a user function.
 */
Gan_Bool
 gan_camera_set_common_fields ( Gan_Camera *camera,
                                Gan_CameraType type,
                                double zh, double fx, double fy,
                                double x0, double y0 )
{
   if ( fx == 0.0 || fy == 0.0 )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_camera_set_common_fields", GAN_ERROR_DIVISION_BY_ZERO, "" );
      return GAN_FALSE;
   }

   /* copy calibration type and linear camera parameters */
   camera->type = type;
   camera->zh = zh; /* 3rd homogeneous image coordinate */
   camera->fx = fx; camera->fy = fy; /* focal distances */
   camera->x0 = x0; camera->y0 = y0; /* image centre coordinates */
   camera->fy_over_fx = fy/fx; /* ratio of focal distances */
#if 0
   /* no radial basis functions by default */
   camera->nrbf = 0;
#endif

   return GAN_TRUE;
}

/**
 * \}
 */

/**
 * \}
 */

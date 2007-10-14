/**
 * File:          $RCSfile: cameraf_cubic_Bspline_inv.c,v $
 * Module:        Functions for single precision camera defined as a projective warp and a B-spline warp
 * Part of:       Gandalf Library
 *
 * Revision:      $Revision: 1.1.2.1 $
 * Last edited:   $Date: 2007/10/14 02:33:13 $
 * Author:        $Author: ruetz $
 * Copyright:     (c) 2000 Imagineer Systems Limited
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

#include <gandalf/vision/cameraf_cubic_Bspline_inv.h>
#include <gandalf/common/array.h>

/**
 * \addtogroup Vision
 * \{
 */

/**
 * \addtogroup Camera
 * \{
 */

#if 0
#define DEBUG_ALL 1
#endif

/* termination threshold */
#define TERMINATION_THRESHOLD 1.0e-4

#define CAM_STRUCT Gan_Camera_f
#define GAN_POSITIONSTATE Gan_PositionState_f
#define CAM_MAT33_ZERO_Q gan_mat33f_zero_q
#define CAM_FILL_ARRAY_FLOAT gan_fill_array_f
#define CAM_MAT22_IDENT_Q gan_mat22f_ident_q
#define CAM_MAT22_ZERO_Q gan_mat22f_zero_q
#define CAM_MAT22_MULTV2_Q gan_mat22f_multv2_q
#define CAM_VEC3_SET_PARTS_Q gan_vec3f_set_parts_q
#define CAM_MAT33_MULTV3_Q gan_mat33f_multv3_q
#define CAM_MAT33_MULTV3_S gan_mat33f_multv3_s
#define CAM_MAT22_FILL_Q gan_mat22f_fill_q
#define CAM_MAT22_RMULTM22_S gan_mat22f_rmultm22_s
#define CAM_VEC2_SUB_Q gan_vec2f_sub_q
#define CAM_VEC2_DECREMENT gan_vec2f_decrement
#define CAM_MAT22_DET_Q gan_mat22f_det_q
#define CAM_VEC2_FILL_Q gan_vec2f_fill_q
#define GAN_MAT33_MULTV3_Q gan_mat33f_multv3_q
#define CAM_VECTOR2 Gan_Vector2_f
#define CAM_VECTOR3 Gan_Vector3_f
#define CAM_MATRIX22 Gan_Matrix22_f
#define CAM_FLOAT float
#define CAM_FLOAT_MAX FLT_MAX
#define CAM_SQR gan_sqr_f
#define CAM_ZERO 0.0F
#define CAM_ONE 1.0F
#define CAM_HALF 0.5F
#define CAM_TWO 2.0F
#define CAM_THREE 3.0F
#define CAM_FOUR 4.0F
#define CAM_ONE_SIXTH GAN_ONE_SIXTH_F
#define CAM_SMALL_NUMBER 1.0e-4F
#define CAM_SMALL_NUMBER_INV 1.0e4F
#define CAM_INVERT_WARP
#define CAM_DETERMINANT_LIMIT 1.0e-10F
#define CAM_LARGE_VALUE 1.0e9F
#define CAM_WEIGHTBLOCK Gan_CubicBSplineWeightBlockF
#define CAM_SUPPORTSTRUCT Gan_CubicBSplineSupportF

/* static functions in separate file to avoid duplication with double precision version of code */
#include "camera_cubic_Bspline_noc.c"

/**
 * \brief Builds a structure representing an inverse cubic B-spline mesh
 * \param camera Pointer to the camera structure to be filled
 * \param zh Third homogeneous image coordinate
 * \param fx Focal distance measured in x-pixels
 * \param fy Focal distance measured in y-pixels
 * \param x0 x-coordinate of image centre
 * \param y0 y-coordinate of image centre
 * \param skew Skew coefficient (xy element of camera matrix)
 * \param kyx Element of camera matrix at position YX
 * \param kzx Element of camera matrix at position ZX
 * \param kzy Element of camera matrix at position ZY
 * \param weight Array of B-spline basis function weights
 * \param support Pointer to support structure for projection
 *
 * Constructs a structure representing a camera, under the model that the
 * distortion follows a B-spline mesh.
 *
 * This is the single precision version.
 *
 * NB allocation/freeing of the weight and support arrays should be handled by calling function!
 *
 * \return #GAN_TRUE on successfully building the camera structure, #GAN_FALSE on failure.
 * \sa gan_camera_build_cubic_Bspline_inv().
 */
Gan_Bool
 gan_cameraf_build_cubic_Bspline_inv ( Gan_Camera_f *camera,
                                       float zh,
                                       float fx, float fy,
                                       float x0, float y0,
                                       float skew,
                                       float kyx,
                                       float kzx, float kzy,
                                       Gan_Vector2_f weight[1 + (1 << GAN_CUBIC_BSPLINE_CAMERA_LEVEL)][1 + (1 << GAN_CUBIC_BSPLINE_CAMERA_LEVEL)],
                                       Gan_CubicBSplineSupportF* support )
{
   int error_code;

   if ( !gan_cameraf_set_common_fields ( camera, GAN_CUBIC_BSPLINE_CAMERA_INV, zh, fx, fy, x0, y0 ) )
   {
      gan_err_register ( "gan_cameraf_build_cubic_Bspline_inv", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   /* copy linear camera parameters into matrix */
   camera->nonlinear.cbspline.K.xx = fx;
   camera->nonlinear.cbspline.K.yy = fy;
   camera->nonlinear.cbspline.K.xz = x0;
   camera->nonlinear.cbspline.K.yz = y0;
   camera->nonlinear.cbspline.K.zz = zh;

   /* copy non-linear camera parameters */
   camera->nonlinear.cbspline.K.xy = skew;
   camera->nonlinear.cbspline.K.yx = kyx;
   camera->nonlinear.cbspline.K.zx = kzx;
   camera->nonlinear.cbspline.K.zy = kzy;

   /* extra copies stored in a scale-independent manner */
   camera->nonlinear.cbspline.skew = skew/zh;
   camera->nonlinear.cbspline.kyx = kyx/zh;
   camera->nonlinear.cbspline.kzx = kzx/zh;
   camera->nonlinear.cbspline.kzy = kzy/zh;
   
   /* invert camera matrix */
   if(gan_mat33f_invert(&camera->nonlinear.cbspline.K, &camera->nonlinear.cbspline.Kinv, &error_code) == NULL)
   {
      gan_err_register ( "gan_cameraf_build_cubic_Bspline_inv", error_code, "" );
      return GAN_FALSE;
   }

   /* copy weight array and support structure */
   camera->nonlinear.cbspline.weight = weight;
   camera->nonlinear.cbspline.support = support;

   /* initialise warped positions */
   build_warped_positions ( (const Gan_CubicBSplineWeightBlockF *)camera->nonlinear.cbspline.weight, camera->nonlinear.cbspline.support );
   
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

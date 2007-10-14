/**
 * File:          $RCSfile: rotate3D.c,v $
 * Module:        Handling 3D rotations
 * Part of:       Gandalf Library
 *
 * Revision:      $Revision: 1.1.2.1 $
 * Last edited:   $Date: 2007/10/14 02:33:20 $
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
#include <gandalf/vision/rotate3D.h>
#include <gandalf/common/misc_error.h>
#include <gandalf/common/numerics.h>
#include <gandalf/linalg/3x3matrix_eigen.h>
#include <gandalf/linalg/3x3matrix_svd.h>

/**
 * \addtogroup Vision
 * \{
 */

/**
 * \defgroup Rotation 3D Rotation
 * \{
 */

/**
 * \brief Fill quaternion with values.
 * \param q Pointer to quaternion to fill
 * \param q0 First element of q, representing the rotation angle
 * \param q1 Second element of q, representing the rotation axis x-coordinate
 * \param q2 Second element of q, representing the rotation axis y-coordinate
 * \param q3 Second element of q, representing the rotation axis z-coordinate
 *
 * Fills a structure containing a quaternion with its four elements.
 *
 * \return no value.
 * \sa gan_quat_fill_s()
 */
void
 gan_quat_fill_q ( Gan_Quaternion *q,
                   double q0, double q1, double q2, double q3 )
{
   q->q0 = q0; q->q1 = q1; q->q2 = q2; q->q3 = q3;
}

/**
 * \brief Fill quaternion with values.
 * \param q0 First element of q, representing the rotation angle
 * \param q1 Second element of q, representing the rotation axis x-coordinate
 * \param q2 Second element of q, representing the rotation axis y-coordinate
 * \param q3 Second element of q, representing the rotation axis z-coordinate
 *
 * Fills a structure containing a quaternion with its four elements.
 *
 * \return a filled quaternion structure.
 * \sa gan_quat_fill_q()
 */
Gan_Quaternion
 gan_quat_fill_s ( double q0, double q1, double q2, double q3 )
{
   Gan_Quaternion q;

   gan_quat_fill_q ( &q, q0, q1, q2, q3 );
   return q;
}

/**
 * \brief Multiply all the elements of a quaternion by a scalar value.
 * \param q Pointer to quaternion to rescale
 * \param s Scalar value by which to multiply quaternion
 *
 * Multiplies all the elements of a quaternion by the same scalar value.
 *
 * \return a quaternion structure containing the new elements.
 * \sa gan_quat_divide_s()
 */
Gan_Quaternion
 gan_quat_scale_s ( Gan_Quaternion *q, double s )
{
   Gan_Quaternion qp;

   (void)gan_quat_scale_q ( q, s, &qp );
   return qp;
}

/**
 * \brief Divide quaternion by scalar.
 */
Gan_Quaternion
 gan_quat_divide_s ( Gan_Quaternion *q, double s )
{
   Gan_Quaternion qp;

   if ( gan_quat_divide_q ( q, s, &qp ) == NULL )
      assert(0);

   return qp;
}

/**
 * \brief Scale quaternion to unit length.
 */
Gan_Quaternion
 gan_quat_unit_s ( Gan_Quaternion *q )
{
   Gan_Quaternion qp;

   if ( gan_quat_unit_q ( q, &qp ) == NULL )
      assert(0);

   return qp;
}

/**
 * \brief Fill rotation structure from quaternion.
 */
Gan_Bool
 gan_rot3D_build_quaternion ( Gan_Rot3D *rot,
                              double q0, double q1, double q2, double q3 )
{
   rot->type = GAN_ROT3D_QUATERNION;
   rot->data.q.q0 = q0;
   rot->data.q.q1 = q1;
   rot->data.q.q2 = q2;
   rot->data.q.q3 = q3;
   return GAN_TRUE;
}

/**
 * \brief Fill rotation structure from exponential rotation parameters.
 */
Gan_Bool
 gan_rot3D_build_exponential ( Gan_Rot3D *rot,
                               double rx, double ry, double rz )
{
   rot->type = GAN_ROT3D_EXPONENTIAL;
   rot->data.r.x = rx;
   rot->data.r.y = ry;
   rot->data.r.z = rz;
   return GAN_TRUE;
}

/**
 * \brief Fill rotation structure from angle/axis rotation parameters.
 */
Gan_Bool
 gan_rot3D_build_angle_axis ( Gan_Rot3D *rot,
                              double angle, double ax, double ay, double az )
{
   rot->type = GAN_ROT3D_ANGLE_AXIS;
    rot->data.aa.angle = angle;
    rot->data.aa.axis.x = ax;
    rot->data.aa.axis.y = ay;
    rot->data.aa.axis.z = az;
   return GAN_TRUE;
}

/**
 * \brief Fill rotation structure from matrix rotation parameters.
 */
Gan_Bool
 gan_rot3D_build_matrix ( Gan_Rot3D *rot,
                          double Rxx, double Rxy, double Rxz,
                          double Ryx, double Ryy, double Ryz,
                          double Rzx, double Rzy, double Rzz )
{
   rot->type = GAN_ROT3D_MATRIX;
    rot->data.R.xx = Rxx; rot->data.R.xy = Rxy; rot->data.R.xz = Rxz;
    rot->data.R.yx = Ryx; rot->data.R.yy = Ryy; rot->data.R.yz = Ryz;
    rot->data.R.zx = Rzx; rot->data.R.zy = Rzy; rot->data.R.zz = Rzz;
   return GAN_TRUE;
}

/**
 * \brief Set null rotation with given type.
 */
Gan_Bool
 gan_rot3D_ident_q ( Gan_Rot3D *rot, Gan_Rot3D_Type type )
{
   switch ( type )
   {
      case GAN_ROT3D_QUATERNION:
        gan_quat_fill_q ( &rot->data.q, 1.0, 0.0, 0.0, 0.0 );
        break;

      case GAN_ROT3D_EXPONENTIAL:
        (void)gan_vec3_zero_q ( &rot->data.r );
        break;

      case GAN_ROT3D_ANGLE_AXIS:
        rot->data.aa.angle = 0.0;
        (void)gan_vec3_fill_q ( &rot->data.aa.axis, 0.0, 0.0, 1.0 );
        break;

      case GAN_ROT3D_MATRIX:
        (void)gan_mat33_ident_q ( &rot->data.R );
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_rot3D_ident_q", GAN_ERROR_ILLEGAL_TYPE, "" );
        return GAN_FALSE;
   }

   rot->type = type;
   return GAN_TRUE;
}

/**
 * \brief Return null rotation with given type.
 */
Gan_Rot3D
 gan_rot3D_ident_s ( Gan_Rot3D_Type type )
{
   Gan_Rot3D rot;

   gan_rot3D_ident_q ( &rot, type );
   return rot;
}

/**
 * \brief Apply a rotation to a 3D point.
 *
 * Rotation definition is \f$ X_{new} = R\:X \f$.
 */
Gan_Bool
 gan_rot3D_apply_v3_q ( Gan_Rot3D *rot,
                        Gan_Vector3 *X, Gan_Vector3 *X_new )
{
   gan_err_test_bool ( X != X_new, "gan_rot3D_apply_v3_q()",
                       GAN_ERROR_FAILURE, "cannot do in-place rotation" );
   if ( rot->type == GAN_ROT3D_MATRIX )
      gan_mat33_multv3_q ( &rot->data.R, X, X_new );
   else
   {
      /* convert rotation to matrix to make it easy */
      Gan_Rot3D rot_R;

      gan_rot3D_convert_q ( rot, GAN_ROT3D_MATRIX, &rot_R );
      gan_mat33_multv3_q ( &rot_R.data.R, X, X_new );
   }

   return GAN_TRUE;
}

/**
 * \brief Return the rotation of a 3D point.
 *
 * Returns \f$ R\:X \f$.
 */
Gan_Vector3
 gan_rot3D_apply_v3_s ( Gan_Rot3D *rot, Gan_Vector3 *X )
{
   Gan_Vector3 X_new;

   gan_rot3D_apply_v3_q ( rot, X, &X_new );
   return X_new;
}

#define Q1 rot1->data.q
#define Q2 rot2->data.q

/**
 * \brief Combine two rotations into a third.
 *
 * For matrices \f$ R_3 = R_1\:R_2 \f$.
 */
Gan_Bool
 gan_rot3D_mult_q ( Gan_Rot3D *rot1, Gan_Rot3D *rot2,
                    Gan_Rot3D *rot3 )
{
   /* we can only deal with rotations of the same type */
   gan_err_test_bool ( rot1->type == rot2->type, "gan_rot3D_mult_q",
                       GAN_ERROR_INCOMPATIBLE, "" );

   switch ( rot1->type )
   {
      case GAN_ROT3D_QUATERNION:
        gan_quat_fill_q ( &rot3->data.q,
                       Q1.q0*Q2.q0 - Q1.q1*Q2.q1 - Q1.q2*Q2.q2 - Q1.q3*Q2.q3,
                       Q1.q0*Q2.q1 + Q1.q1*Q2.q0 + Q1.q2*Q2.q3 - Q1.q3*Q2.q2,
                       Q1.q0*Q2.q2 + Q1.q2*Q2.q0 + Q1.q3*Q2.q1 - Q1.q1*Q2.q3,
                       Q1.q0*Q2.q3 + Q1.q3*Q2.q0 + Q1.q1*Q2.q2 - Q1.q2*Q2.q1 );

        /* renormalise to unit quaternion */
        gan_quat_unit_i ( &rot3->data.q );
        break;

      case GAN_ROT3D_MATRIX:
        (void)gan_mat33_rmultm33_q ( &rot1->data.R, &rot2->data.R,
                                     &rot3->data.R );
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_rot3D_mult_q", GAN_ERROR_ILLEGAL_TYPE, "" );
        return GAN_FALSE;
   }

   rot3->type = rot1->type;
   return GAN_TRUE;
}

/**
 * \brief Combine two rotations and return a third.
 *
 * For matrices return \f$ R_1\:R_2 \f$.
 */
Gan_Rot3D
 gan_rot3D_mult_s ( Gan_Rot3D *rot1, Gan_Rot3D *rot2 )
{
   Gan_Rot3D rot3;

   gan_rot3D_mult_q ( rot1, rot2, &rot3 );
   return rot3;
}

/**
 * \brief Combine two rotations into a third.
 *
 * For matrices \f$ R_3 = R_1\:R_2^{-1} \f$.
 */
Gan_Bool
 gan_rot3D_multI_q ( Gan_Rot3D *rot1, Gan_Rot3D *rot2,
                     Gan_Rot3D *rot3 )
{
   /* we can only deal with rotations of the same type */
   gan_err_test_bool ( rot1->type == rot2->type, "gan_rot3D_multI_q",
                       GAN_ERROR_INCOMPATIBLE, "" );

   switch ( rot1->type )
   {
      case GAN_ROT3D_QUATERNION:
        gan_quat_fill_q ( &rot3->data.q,
                      -Q1.q0*Q2.q0 - Q1.q1*Q2.q1 - Q1.q2*Q2.q2 - Q1.q3*Q2.q3,
                       Q1.q0*Q2.q1 - Q1.q1*Q2.q0 + Q1.q2*Q2.q3 - Q1.q3*Q2.q2,
                       Q1.q0*Q2.q2 - Q1.q2*Q2.q0 + Q1.q3*Q2.q1 - Q1.q1*Q2.q3,
                       Q1.q0*Q2.q3 - Q1.q3*Q2.q0 + Q1.q1*Q2.q2 - Q1.q2*Q2.q1 );

      break;

      case GAN_ROT3D_MATRIX:
        (void)gan_mat33_rmultm33T_q ( &rot1->data.R, &rot2->data.R,
                                      &rot3->data.R );
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_rot3D_multI_q", GAN_ERROR_ILLEGAL_TYPE, "" );
        return GAN_FALSE;
   }

   rot3->type = rot1->type;
   return GAN_TRUE;
}

/**
 * \brief Combine two rotations and return a third.
 *
 * For matrices returns \f$ R_1\:R_2^{-1} \f$.
 */
Gan_Rot3D
 gan_rot3D_multI_s ( Gan_Rot3D *rot1, Gan_Rot3D *rot2 )
{
   Gan_Rot3D rot3;

   gan_rot3D_multI_q ( rot1, rot2, &rot3 );
   return rot3;
}

#undef Q1
#undef Q2

/**
 * \brief Scale rotation parameters.
 */
Gan_Bool
 gan_rot3D_scale_q ( Gan_Rot3D *rot_s, double scale, Gan_Rot3D *rot_d )
{
   switch ( rot_s->type )
   {
      case GAN_ROT3D_QUATERNION:
        (void)gan_quat_scale_q ( &rot_s->data.q, scale, &rot_d->data.q );
        break;

      case GAN_ROT3D_EXPONENTIAL:
        (void)gan_vec3_scale_q ( &rot_s->data.r, scale, &rot_d->data.r );
        break;

      case GAN_ROT3D_ANGLE_AXIS:
        rot_d->data.aa.angle = scale*rot_s->data.aa.angle;
        (void)gan_vec3_scale_q ( &rot_s->data.aa.axis, scale,
                                 &rot_d->data.aa.axis );
        break;

      case GAN_ROT3D_MATRIX:
        (void)gan_mat33_scale_q ( &rot_s->data.R, scale, &rot_d->data.R );
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_rot3D_scale_q", GAN_ERROR_ILLEGAL_TYPE, "" );
        return GAN_FALSE;
   }

   rot_d->type = rot_s->type;
   return GAN_TRUE;
}

/**
 * \brief Multiply rotation parameters by scalar.
 */
Gan_Rot3D
 gan_rot3D_scale_s ( Gan_Rot3D *rot_s, double scale )
{
   Gan_Rot3D rot_d;

   gan_rot3D_scale_q ( rot_s, scale, &rot_d );
   return rot_d;
}

/**
 * \brief Divide rotation parameters by scalar.
 */
Gan_Bool
 gan_rot3D_divide_q ( Gan_Rot3D *rot_s, double scale, Gan_Rot3D *rot_d )
{
   gan_err_test_bool ( scale != 0.0, "gan_rot3D_divide_q",
                       GAN_ERROR_DIVISION_BY_ZERO, "" );
   switch ( rot_s->type )
   {
      case GAN_ROT3D_QUATERNION:
        gan_quat_divide_q ( &rot_s->data.q, scale, &rot_d->data.q );
        break;

      case GAN_ROT3D_EXPONENTIAL:
        gan_vec3_divide_q ( &rot_s->data.r, scale, &rot_d->data.r );
        break;

      case GAN_ROT3D_ANGLE_AXIS:
        rot_d->data.aa.angle = rot_s->data.aa.angle/scale;
        gan_vec3_divide_q ( &rot_s->data.aa.axis, scale,
                            &rot_d->data.aa.axis );
        break;

      case GAN_ROT3D_MATRIX:
        gan_mat33_divide_q ( &rot_s->data.R, scale, &rot_d->data.R );
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_rot3D_divide_q", GAN_ERROR_ILLEGAL_TYPE, "" );
        return GAN_FALSE;
   }

   rot_d->type = rot_s->type;
   return GAN_TRUE;
}

/**
 * \brief Divide rotation parameters by scalar.
 */
Gan_Rot3D
 gan_rot3D_divide_s ( Gan_Rot3D *rot_s, double scale )
{
   Gan_Rot3D rot_d;

   gan_rot3D_divide_q ( rot_s, scale, &rot_d );
   return rot_d;
}

/**
 * \brief Add two sets of rotation parameters together.
 */
Gan_Bool
 gan_rot3D_add_q ( Gan_Rot3D *rot1, Gan_Rot3D *rot2, Gan_Rot3D *rot3 )
{
   gan_err_test_bool ( rot1->type == rot2->type, "gan_rot3D_add_q",
                       GAN_ERROR_INCOMPATIBLE, "" );
   switch ( rot1->type )
   {
      case GAN_ROT3D_QUATERNION:
        (void)gan_quat_add_q ( &rot1->data.q, &rot2->data.q, &rot3->data.q );
        break;

      case GAN_ROT3D_EXPONENTIAL:
        (void)gan_vec3_add_q ( &rot1->data.r, &rot2->data.r, &rot3->data.r );
        break;

      case GAN_ROT3D_ANGLE_AXIS:
        rot3->data.aa.angle = rot1->data.aa.angle + rot2->data.aa.angle;
        (void)gan_vec3_add_q ( &rot1->data.aa.axis, &rot2->data.aa.axis,
                               &rot3->data.aa.axis );
        break;

      case GAN_ROT3D_MATRIX:
        (void)gan_mat33_add_q ( &rot1->data.R, &rot2->data.R, &rot3->data.R );
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_rot3D_add_q", GAN_ERROR_ILLEGAL_TYPE, "" );
        return GAN_FALSE;
   }

   rot3->type = rot1->type;
   return GAN_FALSE;
}

/**
 * \brief Add two sets of rotation parameters together.
 */
Gan_Rot3D
 gan_rot3D_add_s ( Gan_Rot3D *rot1, Gan_Rot3D *rot2 )
{
   Gan_Rot3D rot3;

   gan_rot3D_add_q ( rot1, rot2, &rot3 );
   return rot3;
}

/**
 * \brief Subtract two sets of rotation parameters from each other.
 */
Gan_Bool
 gan_rot3D_sub_q ( Gan_Rot3D *rot1, Gan_Rot3D *rot2, Gan_Rot3D *rot3 )
{
   gan_err_test_bool ( rot1->type == rot2->type, "gan_rot3D_sub_q",
                       GAN_ERROR_INCOMPATIBLE, "" );
   switch ( rot1->type )
   {
      case GAN_ROT3D_QUATERNION:
        (void)gan_quat_sub_q ( &rot1->data.q, &rot2->data.q, &rot3->data.q );
        break;

      case GAN_ROT3D_EXPONENTIAL:
        (void)gan_vec3_sub_q ( &rot1->data.r, &rot2->data.r, &rot3->data.r );
        break;

      case GAN_ROT3D_ANGLE_AXIS:
        rot3->data.aa.angle = rot1->data.aa.angle - rot2->data.aa.angle;
        (void)gan_vec3_sub_q ( &rot1->data.aa.axis, &rot2->data.aa.axis,
                               &rot3->data.aa.axis );
        break;

      case GAN_ROT3D_MATRIX:
        (void)gan_mat33_sub_q ( &rot1->data.R, &rot2->data.R, &rot3->data.R );
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_rot3D_sub_q", GAN_ERROR_ILLEGAL_TYPE, "" );
        return GAN_FALSE;
   }

   rot3->type = rot1->type;
   return GAN_TRUE;
}

/**
 * \brief Subtract two sets of rotation parameters from each other.
 */
Gan_Rot3D
 gan_rot3D_sub_s ( Gan_Rot3D *rot1, Gan_Rot3D *rot2 )
{
   Gan_Rot3D rot3;

   gan_rot3D_sub_q ( rot1, rot2, &rot3 );
   return rot3;
}

/**
 * \brief Convert rotation from one type to another.
 */
Gan_Bool
 gan_rot3D_convert_q ( Gan_Rot3D *rot_s, Gan_Rot3D_Type type,
                       Gan_Rot3D *rot_d )
{
   /* in-place conversion not allowed */
   gan_err_test_bool ( rot_s != rot_d, "gan_rot3D_convert_q",
                       GAN_ERROR_INCOMPATIBLE, "" );
   switch ( rot_s->type )
   {
      case GAN_ROT3D_QUATERNION:
        gan_rot3D_from_quaternion_q ( rot_d, &rot_s->data.q, type );
        break;

      case GAN_ROT3D_EXPONENTIAL:
        gan_rot3D_from_exponential_q ( rot_d, &rot_s->data.r, type );
        break;

      case GAN_ROT3D_MATRIX:
        gan_rot3D_from_matrix_q ( rot_d, &rot_s->data.R, type );
        break;

      case GAN_ROT3D_ANGLE_AXIS:
        gan_rot3D_from_angle_axis_q ( rot_d, rot_s->data.aa.angle,
                                      &rot_s->data.aa.axis, type );
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_rot3D_convert_q", GAN_ERROR_ILLEGAL_TYPE, "" );
        return GAN_FALSE;
   }

   return GAN_TRUE;
}

/**
 * \brief Return conversion of rotation from one type to another.
 */
Gan_Rot3D
 gan_rot3D_convert_s ( Gan_Rot3D *rot_s, Gan_Rot3D_Type type )
{
   Gan_Rot3D rot_d;

   gan_rot3D_convert_q ( rot_s, type, &rot_d );
   return rot_d;
}

#define Q0 qp.q0
#define Q1 qp.q1
#define Q2 qp.q2
#define Q3 qp.q3

/**
 * \brief Convert rotation from quaternion to another type.
 */
Gan_Bool
 gan_rot3D_from_quaternion_q ( Gan_Rot3D *rot,
                               Gan_Quaternion *q, Gan_Rot3D_Type type )
{
   Gan_Quaternion qp;

   if ( gan_quat_unit_q ( q, &qp ) == NULL )
   {
      gan_err_register ( "gan_rot3D_from_quaternion_q", GAN_ERROR_FAILURE, "");
      return GAN_FALSE;
   }
      
   switch ( type )
   {
      case GAN_ROT3D_QUATERNION:
        rot->data.q = qp;
        break;

      case GAN_ROT3D_EXPONENTIAL:
      {
         double vl2, fac;

         gan_err_test_bool ( !(Q0 == 0.0 && Q1 == 0.0 &&
                               Q2 == 0.0 && Q3 == 0.0),
                             "gan_rot3D_from_quaternion_q",
                             GAN_ERROR_DIVISION_BY_ZERO, "" );

         (void)gan_vec3_fill_q ( &rot->data.r, Q1, Q2, Q3 );
         vl2 = gan_vec3_sqrlen_q(&rot->data.r);
         if ( vl2 != 0.0 )
         {
            fac = (Q0 > 0.0) ?  2.0*atan2 ( sqrt(vl2),  Q0 )
                                : -2.0*atan2 ( sqrt(vl2), -Q0 );
            (void)gan_vec3_unit_i ( &rot->data.r );
            (void)gan_vec3_scale_i ( &rot->data.r, fac );
         }
      }
      break;

      case GAN_ROT3D_MATRIX:
        (void)gan_mat33_fill_q ( &rot->data.R,

               /* first row of matrix */
               Q0*Q0+Q1*Q1-Q2*Q2-Q3*Q3, 2.0*(Q1*Q2-Q0*Q3), 2.0*(Q1*Q3+Q0*Q2),

               /* second row of matrix */
               2.0*(Q2*Q1+Q0*Q3), Q0*Q0-Q1*Q1+Q2*Q2-Q3*Q3, 2.0*(Q2*Q3-Q0*Q1),

               /* third row of matrix */
               2.0*(Q3*Q1-Q0*Q2), 2.0*(Q3*Q2+Q0*Q1), Q0*Q0-Q1*Q1-Q2*Q2+Q3*Q3 );
      break;

      case GAN_ROT3D_ANGLE_AXIS:
      if ( Q1 == 0.0 && Q2 == 0.0 && Q3 == 0.0 )
      {
         /* zero rotation about arbitrary axis */
         rot->data.aa.angle = 0.0;
         (void)gan_vec3_fill_q ( &rot->data.aa.axis, 0.0, 0.0, 1.0 );
      }
      else
      {
         (void)gan_vec3_fill_q ( &rot->data.aa.axis, Q1, Q2, Q3 );
         (void)gan_vec3_unit_i ( &rot->data.aa.axis );
         rot->data.aa.angle = 2.0*acos(Q0);
      }

      break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_rot3D_from_quaternion_q",
                           GAN_ERROR_ILLEGAL_TYPE, "" );
        return GAN_FALSE;
   }

   rot->type = type;
   return GAN_TRUE;
}

/**
 * \brief Return conversion of rotation from quaternion to another type.
 */
Gan_Rot3D
 gan_rot3D_from_quaternion_s ( Gan_Quaternion *q, Gan_Rot3D_Type type )
{
   Gan_Rot3D rot;

   gan_rot3D_from_quaternion_q ( &rot, q, type );
   return rot;
}

#undef Q0
#undef Q1
#undef Q2
#undef Q3

#define RX r->x
#define RY r->y
#define RZ r->z

/**
 * \brief Convert rotation from exponential to another type.
 */
Gan_Bool
 gan_rot3D_from_exponential_q ( Gan_Rot3D *rot,
                                Gan_Vector3 *r, Gan_Rot3D_Type type )
{
   double theta;

   /* rotation angle is sqrt(||r||^2) in radians  */
   theta = sqrt(gan_vec3_sqrlen_q(r));

   if ( theta == 0.0 )
   {
      gan_rot3D_ident_q ( rot, type );
      return GAN_TRUE;
   }

   switch ( type )
   {
      case GAN_ROT3D_QUATERNION:
      {
         double s2 = sin(0.5*theta);
      
         gan_rot3D_build_quaternion ( rot, cos(0.5*theta),
                                      s2*RX/theta, s2*RY/theta, s2*RZ/theta );
      }
      break;

      case GAN_ROT3D_EXPONENTIAL:
        rot->data.r = *r;
        break;

      case GAN_ROT3D_MATRIX:
      {
         double ct = (1.0-cos(theta))/theta/theta, st = sin(theta)/theta;

         gan_rot3D_build_matrix ( rot,

                /* first row */
                1.0 - (RY*RY + RZ*RZ)*ct, RX*RY*ct - RZ*st, RZ*RX*ct + RY*st,

                /* second row */
                RX*RY*ct + RZ*st, 1.0 - (RZ*RZ + RX*RX)*ct, RY*RZ*ct - RX*st,

                /* third row */
                RZ*RX*ct - RY*st, RY*RZ*ct + RX*st, 1.0 - (RX*RX + RY*RY)*ct );
      }
      break;

      default:
        gan_err_test_bool ( 0, "gan_rot3D_from_exponential_q",
                            GAN_ERROR_ILLEGAL_TYPE, "" );
   }

   rot->type = type;
   return GAN_TRUE;
}

#undef RX
#undef RY
#undef RZ

/**
 * \brief Return conversion of rotation from exponential to another type.
 */
Gan_Rot3D
 gan_rot3D_from_exponential_s ( Gan_Vector3 *r, Gan_Rot3D_Type type )
{
   Gan_Rot3D rot;

   gan_rot3D_from_exponential_q ( &rot, r, type );
   return rot;
}

/**
 * \brief Convert rotation from angle/axis to another type.
 */
Gan_Bool
 gan_rot3D_from_angle_axis_q ( Gan_Rot3D *rot,
                               double angle, Gan_Vector3 *axis,
                               Gan_Rot3D_Type type )
{
   Gan_Vector3 axisu;

   /* special case if angle is zero: null rotation */
   if ( angle == 0.0 )
   {
      gan_rot3D_ident_q ( rot, type );
      return GAN_TRUE;
   }

   /* scale axis direction to a unit vector */
   axisu = *axis;
   gan_vec3_unit_i ( &axisu );

   switch ( type )
   {
      case GAN_ROT3D_QUATERNION:
      {
         double s2;
      
         s2 = sin(0.5*angle);
         rot->data.q = gan_quat_fill_s ( cos(0.5*angle),
                                         s2*axisu.x, s2*axisu.y, s2*axisu.z );
      }
      break;

      case GAN_ROT3D_ANGLE_AXIS:
        rot->data.aa.angle = angle;
        rot->data.aa.axis = *axis;
        break;

      case GAN_ROT3D_MATRIX:
      {
         Gan_Matrix33 S, uuT, I_uuT;

         (void)gan_mat33_cross_q ( &axisu, &S );
         (void)gan_vec33_outer_q ( &axisu, &axisu, &uuT );
         (void)gan_mat33_ident_q ( &I_uuT );
         (void)gan_mat33_decrement ( &I_uuT, &uuT );
         (void)gan_mat33_scale_i ( &I_uuT, cos(angle) );
         (void)gan_mat33_scale_i ( &S, sin(angle) );
         (void)gan_mat33_add_q ( &uuT, &I_uuT, &rot->data.R );
         (void)gan_mat33_increment ( &rot->data.R, &S );
      }
      break;

      default:
      gan_err_test_bool ( 0, "gan_rot3D_from_angle_axis_q",
                          GAN_ERROR_ILLEGAL_TYPE, "" );
   }

   rot->type = type;
   return GAN_TRUE;
}

/**
 * \brief Return conversion of rotation from angle/axis to another type.
 */
Gan_Rot3D gan_rot3D_from_angle_axis_s ( double angle, Gan_Vector3 *axis,
                                        Gan_Rot3D_Type type )
{
   Gan_Rot3D rot;

   gan_rot3D_from_angle_axis_q ( &rot, angle, axis, type );
   return rot;
}

/**
 * \brief Convert rotation from matrix to another type.
 */
Gan_Bool
 gan_rot3D_from_matrix_q ( Gan_Rot3D *rot,
                           Gan_Matrix33 *R, Gan_Rot3D_Type type )
{
   Gan_Matrix33 Rp = *R, VR;
   Gan_Complex W[3];
   double c, s;
   Gan_Vector3 axis;

   /* adjust matrix to orthogonal form */
   if ( !gan_rot3D_matrix_adjust ( &Rp ) )
   {
      gan_err_register ( "gan_rot3D_from_matrix_q", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   if ( type == GAN_ROT3D_MATRIX )
   {
      rot->data.R = Rp;
      rot->type = GAN_ROT3D_MATRIX;
      return GAN_TRUE;
   }
   
   /* compute eigenvalues/eigenvectors of R */
   if ( !gan_mat33_eigen ( &Rp, W, NULL, &VR ) )
   {
      gan_err_register ( "gan_rot3D_from_matrix_q", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   /* extract rotation axis and sine/cosine of rotation angle */
   if ( W[2].i == 0.0 )
   {
      (void)gan_vec3_fill_q ( &axis, VR.xz, VR.yz, VR.zz );
      s = W[0].i; c = W[0].r;
   }
   else if ( W[0].i == 0.0 )
   {
      (void)gan_vec3_fill_q ( &axis, VR.xx, VR.yx, VR.zx );
      s = W[1].i; c = W[1].r;
   }
   else
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_rot3D_from_matrix_q", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   /* check sign of rotation angle */
   if ( gan_mat33_det_q(&VR) > 0.0 )
      /* reverse angle */
      s = -s;

   rot->type = type;
   switch ( type )
   {
      case GAN_ROT3D_QUATERNION:
      {
         double s2;

         /* compute sine and cosine of half the rotation angle */
         if ( s < 0 ) s2 = -sqrt(0.5*(1.0-c));
         else         s2 =  sqrt(0.5*(1.0-c));

         rot->data.q = gan_quat_fill_s ( sqrt(0.5*(1.0+c)),
                                         s2*axis.x, s2*axis.y, s2*axis.z );
      }
      break;

      case GAN_ROT3D_ANGLE_AXIS:
        rot->data.aa.axis = axis;
        rot->data.aa.angle = atan2(s,c);
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_rot3D_from_matrix_q", GAN_ERROR_ILLEGAL_TYPE,
                           "" );
        
        return GAN_FALSE;
   }

   return GAN_TRUE;
}

/**
 * \brief Return conversion of rotation from matrix to another type.
 */
Gan_Rot3D
 gan_rot3D_from_matrix_s ( Gan_Matrix33 *R, Gan_Rot3D_Type type )
{
   Gan_Rot3D rot;

   gan_rot3D_from_matrix_q ( &rot, R, type );
   return rot;
}

/**
 * \brief Adjust rotation matrix to be orthogonal using SVD.
 */
Gan_Bool
 gan_rot3D_matrix_adjust ( Gan_Matrix33 *R )
{
   Gan_Matrix33 U, VT;
   Gan_Vector3 W;

   /* perform SVD on R */
   gan_mat33_svd ( R, &U, &W, &VT );

   /* make sure matrix was full-rank */
   if ( W.x == 0.0 || W.y == 0.0 || W.z == 0.0 )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_rot3D_matrix_adjust", GAN_ERROR_SINGULAR_MATRIX,
                         "" );
      return GAN_FALSE;
   }

   /* combine U*VT into R, implicitly adjusting W to identity */
   gan_mat33_rmultm33_q ( &U, &VT, R );

   /* negate matrix if determinant is wrong sign */
   if ( gan_mat33_det_q ( R ) < 0 )
      gan_mat33_negate_i ( R );

   /* success */
   return GAN_TRUE;
}

/**
 * \}
 */

/**
 * \}
 */

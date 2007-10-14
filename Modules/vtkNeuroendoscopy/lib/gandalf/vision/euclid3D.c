/**
 * File:          $RCSfile: euclid3D.c,v $
 * Module:        Euclidean 3D geometry
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

#include <gandalf/vision/euclid3D.h>

/**
 * \addtogroup Vision
 * \{
 */

/**
 * \defgroup Euclid Euclidean transformations
 * \{
 */

/**
 * \brief Fill parameters of Euclidean transformation.
 * \param euc Pointer to structure to fill
 * \param TX X-coordinate of translation
 * \param TY Y-coordinate of translation
 * \param TZ Z-coordinate of translation
 * \param q0 The first quaternion value, representing the rotation angle
 * \param q1 The second quaternion value, representing the axis x coordinate
 * \param q2 The second quaternion value, representing the axis y coordinate
 * \param q3 The second quaternion value, representing the axis z coordinate
 *
 * Fill the translation and rotation parameters of a Euclidean transformation.
 * The rotation is represented as a quaternion.
 *
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 * \sa gan_euclid3D_ident_q()
 */
Gan_Bool
 gan_euclid3D_build_quaternion ( Gan_Euclid3D *euc,
                                 double TX, double TY, double TZ,
                                 double q0, double q1, double q2, double q3 )
{
   (void)gan_vec3_fill_q ( &euc->trans, TX, TY, TZ );
   gan_rot3D_build_quaternion ( &euc->rot, q0, q1, q2, q3 );
   return GAN_TRUE;
}

/**
 * \brief Fill parameters of Euclidean transformation.
 * \param euc Pointer to structure to fill
 * \param TX X-coordinate of translation
 * \param TY Y-coordinate of translation
 * \param TZ Z-coordinate of translation
 * \param rx exponential rotation parameter x value
 * \param ry exponential rotation parameter y value
 * \param rz exponential rotation parameter z value
 *
 * Fill the translation & rotation parameters of a Euclidean transformation.
 * The rotation is represented as exponential vector parameters
 * \f$ R = e^{[r]_{\times}} \f$.
 *
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 * \sa gan_euclid3D_ident_q()
 */
Gan_Bool
 gan_euclid3D_build_exponential ( Gan_Euclid3D *euc,
                                  double TX, double TY, double TZ,
                                  double rx, double ry, double rz )
{
   (void)gan_vec3_fill_q ( &euc->trans, TX, TY, TZ );
   gan_rot3D_build_exponential ( &euc->rot, rx, ry, rz );
   return GAN_TRUE;
}

/**
 * \brief Fill parameters of Euclidean transformation.
 * \param euc Pointer to structure to fill
 * \param TX X-coordinate of translation
 * \param TY Y-coordinate of translation
 * \param TZ Z-coordinate of translation
 * \param angle Rotation angle
 * \param ax Rotation axis x-coordinate
 * \param ay Rotation axis y-coordinate
 * \param az Rotation axis z-coordinate
 *
 * Fill the translation & rotation parameters of a Euclidean transformation.
 * The rotation is represented as a rotation angle and axis.
 *
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 * \sa gan_euclid3D_ident_q()
 */
Gan_Bool
 gan_euclid3D_build_angle_axis ( Gan_Euclid3D *euc,
                                 double TX, double TY, double TZ,
                                 double angle,
                                 double ax, double ay, double az )
{
   (void)gan_vec3_fill_q ( &euc->trans, TX, TY, TZ );
   gan_rot3D_build_angle_axis ( &euc->rot, angle, ax, ay, az );
   return GAN_TRUE;
}

/**
 * \brief Fill parameters of Euclidean transformation.
 * \param euc Pointer to structure to fill
 * \param TX The X-coordinate of translation
 * \param TY The Y-coordinate of translation
 * \param TZ The Z-coordinate of translation
 * \param Rxx Rotation matrix element \f$ R_{xx} \f$
 * \param Rxy Rotation matrix element \f$ R_{xy} \f$
 * \param Rxz Rotation matrix element \f$ R_{xz} \f$
 * \param Ryx Rotation matrix element \f$ R_{yx} \f$
 * \param Ryy Rotation matrix element \f$ R_{yy} \f$
 * \param Ryz Rotation matrix element \f$ R_{yz} \f$
 * \param Rzx Rotation matrix element \f$ R_{zx} \f$
 * \param Rzy Rotation matrix element \f$ R_{zy} \f$
 * \param Rzz Rotation matrix element \f$ R_{zz} \f$
 *
 * Fill the translation and rotation parameters of a Euclidean transformation.
 * The rotation is represented as a rotation matrix \f$ R \f$.
 *
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 * \sa gan_euclid3D_ident_q()
 */
Gan_Bool
 gan_euclid3D_build_matrix ( Gan_Euclid3D *euc,
                             double TX, double TY, double TZ,
                             double Rxx, double Rxy, double Rxz,
                             double Ryx, double Ryy, double Ryz,
                             double Rzx, double Rzy, double Rzz )
{
   (void)gan_vec3_fill_q ( &euc->trans, TX, TY, TZ );
   gan_rot3D_build_matrix ( &euc->rot,
                            Rxx, Rxy, Rxz, Ryx, Ryy, Ryz, Rzx, Rzy, Rzz );
   return GAN_TRUE;
}

/**
 * \brief Fills a structure with an identity Euclidean transformation.
 * \param euc Pointer to a structure to fill
 * \param rot_type The represention for the rotation parameters
 *
 * Fills a structure with an identity Euclidean transformation, i.e. zero
 * translation and rotation.
 *
 * \return no value.
 * \sa gan_euclid3D_ident_s().
 */
void
 gan_euclid3D_ident_q ( Gan_Euclid3D *euc, Gan_Rot3D_Type rot_type )
{
   (void)gan_vec3_zero_q ( &euc->trans  );
   gan_rot3D_ident_q ( &euc->rot, rot_type );
}

/**
 * \brief Return identity Euclidean transformation.
 * \param rot_type The represention for the rotation parameters
 *
 * Fills a structure with an identity Euclidean transformation, i.e. zero
 * translation and rotation.
 *
 * \return the filled structure.
 * \sa gan_euclid3D_ident_q().
 */
Gan_Euclid3D
 gan_euclid3D_ident_s ( Gan_Rot3D_Type rot_type )
{
   Gan_Euclid3D euc;

   gan_euclid3D_ident_q ( &euc, rot_type );
   return euc;
}

/**
 * \brief Scales Euclidean pose parameters.
 * \param euc_src Pointer to a source transformation structure
 * \param scale Scale factor by which to multiply parameters
 * \param euc_dest Pointer to a destination transformation structure
 *
 * Scales rotation and translation parameters of Euclidean transformation by
 * given scale factor.
 *
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 * \sa gan_euclid3D_scale_s().
 */
Gan_Bool
 gan_euclid3D_scale_q ( Gan_Euclid3D *euc_src, double scale,
                        Gan_Euclid3D *euc_dest )
{
   (void)gan_vec3_scale_q ( &euc_src->trans, scale, &euc_dest->trans );
   if ( !gan_rot3D_scale_q ( &euc_src->rot, scale, &euc_dest->rot ) )
   {
      gan_err_register ( "gan_euclid3D_scale_q", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   return GAN_TRUE;
}

/**
 * \brief Scales Euclidean pose parameters.
 * \param euc Pointer to input transformation structure
 * \param scale Scale factor by which to multiply parameters
 *
 * Scales rotation and translation parameters of Euclidean transformation by
 * given scale factor.
 *
 * \return the result in a new structure.
 * \sa gan_euclid3D_scale_q().
 */
Gan_Euclid3D
 gan_euclid3D_scale_s ( Gan_Euclid3D *euc, double scale )
{
   Gan_Euclid3D euc2;

   if ( !gan_euclid3D_scale_q ( euc, scale, &euc2 ) )
      gan_assert ( 0, "gan_euclid3D_scale_s" );

   return euc2;
}

/**
 * \}
 */

/**
 * \}
 */

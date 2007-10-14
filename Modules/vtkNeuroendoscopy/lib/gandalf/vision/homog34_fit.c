/**
 * File:          $RCSfile: homog34_fit.c,v $
 * Module:        Fit a 3D --> 2D projective transformation to point/line data
 * Part of:       Gandalf Library
 *
 * Revision:      $Revision: 1.1.2.1 $
 * Last edited:   $Date: 2007/10/14 02:33:19 $
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
#include <gandalf/vision/homog34_fit.h>
#include <gandalf/linalg/mat_symmetric.h>
#include <gandalf/common/allocate.h>
#include <gandalf/common/misc_error.h>

/**
 * \addtogroup Vision
 * \{
 */

/**
 * \addtogroup GeomFitting
 * \{
 */

#define gel(M,i,j)   gan_mat_get_el(M,i,j)
#define sel(M,i,j,v) gan_mat_set_el(M,i,j,v)

/**
 * \brief Increments the calculation of a 3x4 homography for a point.
 * \param sme A pointer to a symmetric eigensystem structure
 * \param X A point in the source coordinate frame for the homography
 * \param x A point in the destination coordinate frame
 * \param scale A scaling on the outer product of X \& x
 * \param eq_count Pointer to the total number of equations or \c NULL
 *
 * Increments the accumulated sum of outer products of vectors to be used to
 * compute the homography between a projective 3D and a projective 2D
 * coordinate systems, given a point correspondence between the coordinate
 * systems.
 *
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 * \sa gan_homog34_increment_le().
 */
Gan_Bool
 gan_homog34_increment_p ( Gan_SymMatEigenStruct *sme,
                           Gan_Vector4 *X, Gan_Vector3 *x,
                           double scale, int *eq_count )
{
   if ( sme->SxxT.size != 12 )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_homog34_increment_p", GAN_ERROR_FAILURE,
                         "illegal eigensystem size" );
      return GAN_FALSE;
   }
     
   if ( !gan_symeigen_increment ( sme, scale,
                         -x->z*X->x, -x->z*X->y, -x->z*X->z, -x->z*X->w,
                                0.0,        0.0,        0.0,        0.0,
                          x->x*X->x,  x->x*X->y,  x->x*X->z,  x->x*X->w ) ||
        !gan_symeigen_increment ( sme, scale,
                                0.0,        0.0,        0.0,        0.0,
                         -x->z*X->x, -x->z*X->y, -x->z*X->z, -x->z*X->w,
                          x->y*X->x,  x->y*X->y,  x->y*X->z,  x->y*X->w ) )
   {
      gan_err_register ( "gan_homog34_increment_p", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   /* increment the number of equations by two if required */
   if ( eq_count != NULL ) (*eq_count) += 2;

   return GAN_TRUE;
}

/**
 * \brief Increments the calculation of a 3x4 homography.
 * \param sme A pointer to a symmetric eigensystem structure
 * \param X1 First point in the source 3D coordinate frame
 * \param X2 Second point in the source 3D coordinate frame
 * \param l Line in the destination 2D coordinate frame
 * \param scale A scaling on the outer product of X1/X2 and l
 * \param eq_count Pointer to the total number of equations or \c NULL
 *
 * Increments the accumulated sum of outer products of vectors to be used to
 * compute the homography between a projective 3D and a projective 2D
 * coordinate systems, given a correspondence between the two points in the 3D
 * coordinate system and a line in the 2D coordinate system.
 *
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 * \sa gan_homog34_increment_p().
 */
Gan_Bool
 gan_homog34_increment_le ( Gan_SymMatEigenStruct *sme,
                            Gan_Vector4 *X1, Gan_Vector4 *X2,
                            Gan_Vector3 *l, double scale, int *eq_count )
{
   if ( sme->SxxT.size != 12 )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_homog34_increment_le", GAN_ERROR_FAILURE,
                         "illegal eigensystem size" );
      return GAN_FALSE;
   }
     
   if ( !gan_symeigen_increment ( sme, scale,
                            l->x*X1->x, l->x*X1->y, l->x*X1->z, l->x*X1->w,
                            l->y*X1->x, l->y*X1->y, l->y*X1->z, l->y*X1->w,
                            l->z*X1->x, l->z*X1->y, l->z*X1->z, l->z*X1->w ) ||
        !gan_symeigen_increment ( sme, scale,
                            l->x*X2->x, l->x*X2->y, l->x*X2->z, l->x*X2->w,
                            l->y*X2->x, l->y*X2->y, l->y*X2->z, l->y*X2->w,
                            l->z*X2->x, l->z*X2->y, l->z*X2->z, l->z*X2->w ) )
   {
      gan_err_register ( "gan_homog34_increment_le", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   /* increment the number of equations by two if required */
   if ( eq_count != NULL ) (*eq_count) += 2;

   return GAN_TRUE;
}

/**
 * \brief Solves for the 3x4 homography between 2D \& 3D systems.
 * \param sme A pointer to a symmetric eigensystem structure
 * \param eq_count Total number of equations
 * \param P The homography matrix
 *
 * Solves for the 3x4 homography between 2D & 3D systems, having accumulated
 * point/line correspondences.
 *
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 * \sa gan_homog34_increment_p().
 */
Gan_Bool
 gan_homog34_solve ( Gan_SymMatEigenStruct *sme, int eq_count,
                     Gan_Matrix34 *P )
{
   Gan_Matrix *Z;

   if ( eq_count < 11 )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_homog34_solve", GAN_ERROR_FAILURE,
                         "not enough equations" );
      return GAN_FALSE;
   }

   if ( sme->SxxT.size != 12 )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_homog34_solve", GAN_ERROR_FAILURE,
                         "wrong size eigensystem" );
      return GAN_FALSE;
   }

   if ( !gan_symeigen_solve ( sme ) )
   {
      gan_err_register ( "gan_homog34_solve", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   /* extract eigenvector matrix */
   Z = &sme->Z;
   gan_mat34_fill_q ( P, gel(Z,0,0), gel(Z,1,0), gel(Z, 2,0), gel(Z, 3,0),
                         gel(Z,4,0), gel(Z,5,0), gel(Z, 6,0), gel(Z, 7,0),
                         gel(Z,8,0), gel(Z,9,0), gel(Z,10,0), gel(Z,11,0) );
   return GAN_TRUE;
}

/**
 * \}
 */

/**
 * \}
 */

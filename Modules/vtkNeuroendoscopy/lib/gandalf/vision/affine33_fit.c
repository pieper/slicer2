/**
 * File:          $RCSfile: affine33_fit.c,v $
 * Module:        Fit a 2D affine transformation to point/line data
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

#include <math.h>
#include <gandalf/vision/affine33_fit.h>
#include <gandalf/common/misc_error.h>
#include <gandalf/linalg/pseudo_inverse.h>

/**
 * \defgroup Vision Vision Package
 * \{
 */

/**
 * \defgroup GeomFitting Geometric Fitting
 * \{
 */

#define gel(M,i,j)   gan_mat_get_el(M,i,j)
#define sel(M,i,j,v) gan_mat_set_el(M,i,j,v)

/**
 * \brief Increments the calculation of a 3x3 affine homography for a point.
 * \param pis A pointer to a pseudo-inverse structure
 * \param X A point in the source coordinate frame for the homography
 * \param x A point in the destination coordinate frame
 * \param scale A scaling on the outer product of X and x
 * \param eq_count A pointer to the total number of equations or \c NULL
 * \return #GAN_TRUE success, #GAN_FALSE failure.
 *
 * Increments the accumulated sum of outer products of vectors to be used to
 * compute the homography between two projective 2D planes, given a point
 * correspondence between the planes.
 *
 * Call this function using points \a x having the same z coordinate.
 *
 * \sa gan_affine33_increment_l().
 */
Gan_Bool
 gan_affine33_increment_p ( Gan_PseudoInverseStruct *pis,
                            Gan_Vector3 *X, Gan_Vector3 *x,
                            double scale, int *eq_count )
{
#ifndef NDEBUG
   if ( pis->SaaT.size != 6 || pis->SabT.cols != 1 )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_affine33_increment_p", GAN_ERROR_FAILURE,
                         "illegal pseudo-inverse system size" );
      return GAN_FALSE;
   }
#endif     

   if ( !gan_pseudoinv_increment ( pis, 1.0, X->x, X->y, X->z,
                                              0.0,  0.0,  0.0, X->z*x->x ) ||
        !gan_pseudoinv_increment ( pis, 1.0,  0.0,  0.0,  0.0,
                                             X->x, X->y, X->z, X->z*x->y ) )
   {
      gan_err_register ( "gan_affine33_increment_p", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   /* increment the number of equations by two if required */
   if ( eq_count != NULL ) (*eq_count) += 2;

   return GAN_TRUE;
}

/**
 * \brief Increments the calculation of a 3x3 homography for a line.
 * \param pis A pointer to a pseudo-inverse structure
 * \param X A point in the source coordinate frame for the homography
 * \param l A line in the destination coordinate frame
 * \param scale A scaling on the outer product of #X & #l
 * \param eq_count Pointer to the total number of equations or \c NULL
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Increments the accumulated sum of outer products of vectors to be used to
 * compute the homography between two projective 2D planes, given a point in
 * one plane lying in a line in the other.
 *
 * Call this function using points \a x having the same z coordinate.
 *
 * \sa gan_affine33_increment_l().
 */
Gan_Bool
 gan_affine33_increment_le ( Gan_PseudoInverseStruct *pis,
                             Gan_Vector3 *X, Gan_Vector3 *l,
                             double scale, int *eq_count )
{
#ifndef NDEBUG
   if ( pis->SaaT.size != 6 || pis->SabT.cols != 1 )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_affine33_increment_le", GAN_ERROR_FAILURE,
                         "illegal eigensystem size" );
      return GAN_FALSE;
   }
#endif     

   /* incorporate equations for point <--> line matches:
      Hxx*X + Hxy*Y + Hxz = x, Hyx*X + Hyy*y + Hyz = y */
   if ( !gan_pseudoinv_increment ( pis, 1.0,
                                   l->x*X->x, l->x*X->y, l->x*X->z,
                                   l->y*X->x, l->y*X->y, l->y*X->z,
                                   -l->z*X->z ) )
   {
      gan_err_register ( "gan_affine33_increment_le", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   /* increment the number of equations by two if required */
   if ( eq_count != NULL ) (*eq_count)++;

   return GAN_TRUE;
}

/**
 * \brief Solves for the 3x3 affine homography between two planes.
 * \param pis A pointer to a pseudo-inverse structure
 * \param eq_count Pointer to the total number of equations or \c NULL
 * \param P The homography matrix
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Solves for the 3x3 homography between two planes, having accumulated
 * point/line correspondences.
 *
 * \sa gan_affine33_increment_l().
 */
Gan_Bool
 gan_affine33_solve ( Gan_PseudoInverseStruct *pis,
                      int eq_count, Gan_Matrix23 *P )
{
   Gan_Matrix *psol;

   if ( eq_count < 6 )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_affine33_solve", GAN_ERROR_FAILURE,
                         "not enough equations" );
      return GAN_FALSE;
   }

#ifndef NDEBUG
   if ( pis->SaaT.size != 6 || pis->SabT.cols != 1 )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_affine33_solve", GAN_ERROR_FAILURE,
                         "wrong size eigensystem" );
      return GAN_FALSE;
   }
#endif

   if ( !gan_pseudoinv_solve ( pis, NULL ) )
   {
      gan_err_register ( "gan_affine33_solve", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   psol = gan_pseudoinv_get_solution ( pis );

   /* build affine homography */
   gan_mat23_fill_q ( P, gel(psol,0,0), gel(psol,1,0), gel(psol,2,0),
                         gel(psol,3,0), gel(psol,4,0), gel(psol,5,0) );
   return GAN_TRUE;
}

/**
 * \brief Computes the 2D affine transformation between points and lines.
 * \param match Array of feature matches
 * \param nmatches The number of point/line matches
 * \param H The affine homography to be computed
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Solves for the affine homography between two planes, given point <--> point,
 * line <--> line and point <--> line correspondences between the two planes,
 * as given by the data array.
 *
 * \sa gan_homog33_fit().
 */
Gan_Bool
 gan_affine33_fit ( Gan_Homog33Match *match, unsigned nmatches,
                    Gan_Matrix23 *H )
{
   Gan_Homog33Match *mptr;
   int i, eq_count = 0;
   Gan_PseudoInverseStruct pis;
   Gan_Matrix *psol;

   /* allocate pseudo-inverse structure */
   gan_pseudoinv_form ( &pis, 6, 1 );

   for ( i = (int)nmatches-1, mptr = match; i >= 0; i--, mptr++ )
      switch ( mptr->type )
      {
         case GAN_HOMOG33_POINT:
         {
            Gan_Vector3 x = mptr->d.p.x, X = mptr->d.p.X;

            /* incorporate equations for point <--> point matches:
               Hxx*X + Hxy*Y + Hxz = x, Hyx*X + Hyy*y + Hyz = y */
            gan_pseudoinv_increment ( &pis, 1.0, X.x, X.y, 1.0, 0.0, 0.0, 0.0,
                                      x.x );
            gan_pseudoinv_increment ( &pis, 1.0, 0.0, 0.0, 0.0, X.x, X.y, 1.0,
                                      x.y );
            eq_count += 2;
         }
         break;

         case GAN_HOMOG33_LINE:
           gan_err_flush_trace();
           gan_err_register ( "gan_affine33_fit", GAN_ERROR_ILLEGAL_TYPE, "" );
           return GAN_FALSE;

         case GAN_HOMOG33_LINE_ENDPOINTS:
         {
            Gan_Vector3 l = mptr->d.le.l, X1 = mptr->d.le.X1,
                                          X2 = mptr->d.le.X2;

            /* incorporate equations for point <--> line matches:
               Hxx*X + Hxy*Y + Hxz = x, Hyx*X + Hyy*y + Hyz = y */
            gan_pseudoinv_increment ( &pis, 1.0,
                          l.x*X1.x, l.x*X1.y, l.x, l.y*X1.x, l.y*X1.y, l.y,
                          -l.z );
            gan_pseudoinv_increment ( &pis, 1.0,
                          l.x*X2.x, l.x*X2.y, l.x, l.y*X2.x, l.y*X2.y, l.y,
                          -l.z );
            eq_count += 2;
         }
         break;

         case GAN_HOMOG33_IGNORE:
         break;

         default:
           gan_err_flush_trace();
           gan_err_register ( "gan_affine33_fit", GAN_ERROR_ILLEGAL_TYPE, "" );
           return GAN_FALSE;
      }

   /* check that enough features were found */
   if ( eq_count < 6 )
   {
      /* free pseudo-inverse structure and return with failure */
      gan_pseudoinv_free ( &pis );
      gan_err_register ( "gan_affine33_fit", GAN_ERROR_NOT_ENOUGH_DATA, "" );
      return GAN_FALSE;
   }

   /* compute solution by pseudo-inverse */
   if ( !gan_pseudoinv_solve ( &pis, NULL ) )
   {
      gan_err_register ( "gan_affine33_fit", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   psol = gan_pseudoinv_get_solution ( &pis );

   /* build affine homography */
   gan_mat23_fill_q ( H, gel(psol,0,0), gel(psol,1,0), gel(psol,2,0),
                         gel(psol,3,0), gel(psol,4,0), gel(psol,5,0) );

   /* free pseudo-inverse structure */
   gan_pseudoinv_free ( &pis );

   /* success */
   return GAN_TRUE;
}

/**
 * \}
 */

/**
 * \}
 */

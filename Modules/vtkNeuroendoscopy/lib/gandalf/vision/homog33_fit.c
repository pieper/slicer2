/**
 * File:          $RCSfile: homog33_fit.c,v $
 * Module:        Fit a 2D projective transformation to point/line data
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

#include <math.h>
#include <string.h>
#include <gandalf/vision/homog33_fit.h>
#include <gandalf/common/allocate.h>
#include <gandalf/common/misc_error.h>
#include <gandalf/linalg/symmat_eigen.h>
#include <gandalf/linalg/3vector_norm.h>
#include <gandalf/linalg/mat_symmetric.h>

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
 * \brief Increments the calculation of a 3x3 homography for a point.
 * \param sme A pointer to a symmetric eigensystem structure
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
 * \sa gan_homog33_increment_l().
 */
Gan_Bool
 gan_homog33_increment_p ( Gan_SymMatEigenStruct *sme,
                           Gan_Vector3 *X, Gan_Vector3 *x,
                           double scale, int *eq_count )
{
#ifndef NDEBUG
   if ( sme->SxxT.size != 9 )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_homog33_increment_p", GAN_ERROR_FAILURE,
                         "illegal eigensystem size" );
      return GAN_FALSE;
   }
#endif     

   if ( !gan_symeigen_increment ( sme, scale,
                                  -x->z*X->x, -x->z*X->y, -x->z*X->z,
                                         0.0,        0.0,        0.0,
                                   x->x*X->x,  x->x*X->y,  x->x*X->z ) ||
        !gan_symeigen_increment ( sme, scale,
                                         0.0,        0.0,        0.0,
                                  -x->z*X->x, -x->z*X->y, -x->z*X->z,
                                   x->y*X->x,  x->y*X->y,  x->y*X->z ) )
   {
      gan_err_register ( "gan_homog33_increment_p", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   /* increment the number of equations by two if required */
   if ( eq_count != NULL ) (*eq_count) += 2;

   return GAN_TRUE;
}

/**
 * \brief Increments the calculation of a 3x3 homography for a line.
 * \param sme A pointer to a symmetric eigensystem structure
 * \param L A line in the source coordinate frame for the homography
 * \param l A line in the destination coordinate frame
 * \param scale A scaling on the outer product of L and l
 * \param eq_count A pointer to the total number of equations or \c NULL
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Increments the accumulated sum of outer products of vectors to be used to
 * compute the homography between two projective 2D planes, given a line
 * correspondence between the planes.
 *
 * \sa gan_homog33_increment_p().
 */
Gan_Bool
 gan_homog33_increment_l ( Gan_SymMatEigenStruct *sme,
                           Gan_Vector3 *L, Gan_Vector3 *l,
                           double scale, int *eq_count )
{
#ifndef NDEBUG
   if ( sme->SxxT.size != 9 )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_homog33_increment_l", GAN_ERROR_FAILURE,
                         "illegal eigensystem size" );
      return GAN_FALSE;
   }
#endif

   if ( !gan_symeigen_increment ( sme, scale,
                                  -L->z*l->x, 0.0, L->x*l->x,
                                  -L->z*l->y, 0.0, L->x*l->y,
                                  -L->z*l->z, 0.0, L->x*l->z ) ||
        !gan_symeigen_increment ( sme, scale,
                                  0.0, -L->z*l->x, L->y*l->x,
                                  0.0, -L->z*l->y, L->y*l->y,
                                  0.0, -L->z*l->z, L->y*l->z ) )
   {
      gan_err_register ( "gan_homog33_increment_l", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   /* increment the number of equations by two if required */
   if ( eq_count != NULL ) (*eq_count) += 2;

   return GAN_TRUE;
}

/**
 * \brief Increments the calculation of a 3x3 homography for a line.
 * \param sme A pointer to a symmetric eigensystem structure
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
 * \sa gan_homog33_increment_l().
 */
Gan_Bool
 gan_homog33_increment_le ( Gan_SymMatEigenStruct *sme,
                            Gan_Vector3 *X, Gan_Vector3 *l,
                            double scale, int *eq_count )
{
#ifndef NDEBUG
   if ( sme->SxxT.size != 9 )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_homog33_increment_le", GAN_ERROR_FAILURE,
                         "illegal eigensystem size" );
      return GAN_FALSE;
   }
#endif     

   if ( !gan_symeigen_increment ( sme, scale,
                                  l->x*X->x, l->x*X->y, l->x*X->z,
                                  l->y*X->x, l->y*X->y, l->y*X->z,
                                  l->z*X->x, l->z*X->y, l->z*X->z ) )
   {
      gan_err_register ( "gan_homog33_increment_le", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   /* increment the number of equations by two if required */
   if ( eq_count != NULL ) (*eq_count)++;

   return GAN_TRUE;
}

/**
 * \brief Solves for the 3x3 homography between two planes.
 * \param sme A pointer to a symmetric eigensystem structure
 * \param eq_count Pointer to the total number of equations or \c NULL
 * \param P The homography matrix
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Solves for the 3x3 homography between two planes, having accumulated
 * point/line correspondences.
 *
 * \sa gan_homog33_increment_l().
 */
Gan_Bool
 gan_homog33_solve ( Gan_SymMatEigenStruct *sme,
                     int eq_count, Gan_Matrix33 *P )
{
   Gan_Matrix *Z;

   if ( eq_count < 8 )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_homog33_solve", GAN_ERROR_FAILURE,
                         "not enough equations" );
      return GAN_FALSE;
   }

#ifndef NDEBUG
   if ( sme->SxxT.size != 9 )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_homog33_solve", GAN_ERROR_FAILURE,
                         "wrong size eigensystem" );
      return GAN_FALSE;
   }
#endif

   if ( !gan_symeigen_solve ( sme ) )
   {
      gan_err_register ( "gan_homog33_solve", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   /* extract eigenvector matrix */
   Z = &sme->Z;
   gan_mat33_fill_q ( P, gel(Z,0,0), gel(Z,1,0), gel(Z,2,0),
                         gel(Z,3,0), gel(Z,4,0), gel(Z,5,0),
                         gel(Z,6,0), gel(Z,7,0), gel(Z,8,0) );
   return GAN_TRUE;
}

/**
 * \brief Computes a 3x3 homography between two planes.
 * \param x An array of "image" points
 * \param X An array of "world" points
 * \param n The number of points in the arrays
 * \param H The homography to be computed
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Solves for the homography between two planes, given point <--> point
 * correspondences between the two planes.
 *
 * This function invokes projective normalisation to solve for the
 * homography, so there is no need to pre-normalise the data.
 */
Gan_Bool
 gan_homog33_fit_points ( const Gan_Vector3 *x, const Gan_Vector3 *X, unsigned int n, Gan_Matrix33 *H )
{
   Gan_SymMatEigenStruct sme;
   Gan_SquMatrix33 Lt, Lw;
   Gan_Vector3 *xnormalised = gan_malloc_array(Gan_Vector3, n);
   Gan_Vector3 *Xnormalised = gan_malloc_array(Gan_Vector3, n);
   int i, eq_count=0;

   if(xnormalised == NULL || Xnormalised == NULL)
   {
      gan_err_flush_trace();
      gan_err_register_with_number ( "gan_homog33_fit_points", GAN_ERROR_MALLOC_FAILED, "", n*sizeof(Gan_Vector3) );
      return GAN_FALSE;
   }

   memcpy(xnormalised, x, n*sizeof(Gan_Vector3));
   memcpy(Xnormalised, X, n*sizeof(Gan_Vector3));

   /* normalise image coordinates */
   if ( !gan_vec3_normalize ( Xnormalised, n, 1.0e-15, 1000, &Lt ) ||
        !gan_vec3_normalize ( xnormalised, n, 1.0e-15, 1000, &Lw ) )
   {
      gan_err_register ( "gan_homog33_fit_points", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }
   
   /* Propogate symmetric eigen system matrix with values */
   gan_homog33_init(&sme);
   
   for (i=(int)n-1; i>=0; i--)
      gan_homog33_increment_p ( &sme, &Xnormalised[i], &xnormalised[i], 1.0, &eq_count );

   /* Solve */
   if(!gan_homog33_solve(&sme, eq_count, H))
   {
      gan_err_register ( "gan_homog33_fit_points", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   /* determine sign of matrix */
   {
      Gan_Vector3 xtest;
      double total = 0.0;

      for (i=(int)n-1; i>=0; i--)
      {
         (void)gan_mat33_multv3_q ( H, &Xnormalised[i], &xtest);
         total += gan_vec3_dot_s(&xtest, &xnormalised[i]);
      }

      if(total < 0.0)
         (void)gan_mat33_negate_i(H);
   }

   /* adjust homography */
   *H = gan_mat33_lmultl33_s ( H, &Lw );
   *H = gan_mat33_rmultl33I_s ( H, &Lt );

   gan_homog33_free(&sme);
   gan_free_va(xnormalised, Xnormalised, NULL);

   return GAN_TRUE;
}

/**
 * \brief Computes a 3x3 homography between two planes.
 * \param match An array of point/line matches
 * \param nmatches The number of point/line matches unsigned nmatches
 * \param H The homography to be computed
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Solves for the homography between two planes, given point <--> point,
 * line <--> line and point <--> line correspondences between the two planes,
 * as given by the data array.
 *
 * \sa gan_homog33_increment_l().
 */
Gan_Bool
 gan_homog33_fit ( Gan_Homog33Match *match, unsigned nmatches,
                   Gan_Matrix33 *H )
{
   Gan_Homog33Match *mptr;
   int i, eq_count = 0;
   Gan_SymMatEigenStruct sme;

   /* initialize projective 2D motion parameter fitting algorithm */
   if ( gan_homog33_init ( &sme ) == NULL )
   {
      gan_err_register ( "gan_homog33_fit", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   for ( i = (int)nmatches-1, mptr = match; i >= 0; i--, mptr++ )
      switch ( mptr->type )
      {
         case GAN_HOMOG33_POINT:
           gan_homog33_increment_p ( &sme, &mptr->d.p.X, &mptr->d.p.x, 1.0,
                                     &eq_count );
           break;

         case GAN_HOMOG33_LINE:
           gan_homog33_increment_l ( &sme, &mptr->d.l.L, &mptr->d.l.l, 1.0,
                                     &eq_count );
           break;

         case GAN_HOMOG33_LINE_ENDPOINTS:
           gan_homog33_increment_le ( &sme, &mptr->d.le.X1, &mptr->d.le.l, 1.0,
                                      &eq_count );
           gan_homog33_increment_le ( &sme, &mptr->d.le.X2, &mptr->d.le.l, 1.0,
                                      &eq_count );
         break;

         case GAN_HOMOG33_IGNORE:
           break;

         default:
           gan_err_flush_trace();
           gan_err_register ( "gan_homog33_fit", GAN_ERROR_ILLEGAL_TYPE, "" );
           return GAN_FALSE;
      }

   /* solve eigensystem for solution and test that it was computed
      successfully */
   if ( !gan_homog33_solve ( &sme, eq_count, H ) )
   {
      /* free eigensystem matrices and return with failure */
      gan_err_register ( "gan_homog33_fit", GAN_ERROR_FAILURE, "" );
      gan_homog33_free ( &sme );

      return GAN_FALSE;
   }

#ifndef NDEBUG
   if ( gan_debug )
   {
      fprintf ( stderr, "Homography image plane errors\n" );
      for ( i = (int)nmatches-1, mptr = match; i >= 0; i--, mptr++ )
         switch ( mptr->type )
         {
            case GAN_HOMOG33_POINT:
            {
               Gan_Vector3 p;

               (void)gan_mat33_multv3_q ( H, &mptr->d.p.X, &p );
               fprintf ( stderr, "Point error (%f %f)\n",
                         p.x/p.z - mptr->d.p.x.x/mptr->d.p.x.z,
                         p.y/p.z - mptr->d.p.x.y/mptr->d.p.x.z );
            }
            break;

            case GAN_HOMOG33_LINE:
            fprintf ( stderr, "Line error (n/a n/a)\n" );
            break;

            case GAN_HOMOG33_LINE_ENDPOINTS:
            {
               Gan_Vector3 p1, p2, l;

               (void)gan_mat33_multv3_q ( H, &mptr->d.le.X1, &p1 );
               (void)gan_mat33_multv3_q ( H, &mptr->d.le.X2, &p2 );
               l = mptr->d.le.l;
               gan_vec3_divide_i ( &l, sqrt(l.x*l.x + l.y*l.y) );
               fprintf ( stderr, "Line error (%f %f)\n",
                         gan_vec3_dot_q(&l,&p1), gan_vec3_dot_q(&l,&p2) );
            }
            break;

            case GAN_HOMOG33_IGNORE:
            break;

            default:
              gan_err_flush_trace();
              gan_err_register ( "gan_proj2D_homography",
                                 GAN_ERROR_ILLEGAL_TYPE, "" );
              return GAN_FALSE;
         }
   }
#endif /* #ifndef NDEBUG */

   /* free eigensystem matrices and return with success */
   gan_homog33_free ( &sme );

   return GAN_TRUE;
}

/**
 * \}
 */

/**
 * \}
 */

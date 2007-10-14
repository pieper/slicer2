/**
 * File:         $RCSfile: fundamental.c,v $
 * Module:       Compute the fundamental matrix from point matches in two frames
 * Part of:      Gandalf Library
 *
 * Revision:     $Revision: 1.1.2.1 $
 * Last edited:  $Date: 2007/10/14 02:33:18 $
 * Author:       $Author: ruetz $
 * Copyright:    (c) 2000 Imagineer Software Limited
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
#include <gandalf/vision/fundamental.h>
#include <gandalf/common/allocate.h>
#include <gandalf/common/misc_error.h>
#include <gandalf/linalg/mat_symmetric.h>
#include <gandalf/linalg/matvec_clapack.h>

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
 * \brief Computes the fundamental matrix from point matches.
 * \param p1 The array of image points in image 1
 * \param p2 The array of image points in image 2
 * \param n The number of point matches
 * \param sme An eigensystem structure to use or \c NULL
 * \param F The fundamental matrix to be filled
 *
 * Applies the Longuet-Higgins eight-point algorithm to compute the
 * fundamental matrix for images 1 & 2. If the #sme structure to compute
 * eigenvectors & eigenvalues is passed as \c NULL, a suitable structure
 * is allocated and freed inside the function.
 *
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 * \warning The rank-2 constraint on \f$ F \f$ is not imposed.
 *          Any normalisation of the image data should be applied before
 *          using the function.
 * \sa gan_essential_matrix_fit().
 */
Gan_Bool
 gan_fundamental_matrix_fit ( Gan_Vector3 *p1, Gan_Vector3 *p2, int n,
                              Gan_SymMatEigenStruct *sme,
                              Gan_Matrix33 *F )
{
   Gan_Matrix *Z;
   Gan_Bool sme_alloc = GAN_FALSE;

   /* we need at least eight points */
   if ( n < 8 )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_fundamental_matrix_fit", GAN_ERROR_FAILURE,
                         "not enough points" );
      return GAN_FALSE;
   }

   /* allocate sme if necessary */
   if ( sme == NULL )
   {
      sme = gan_symeigen_alloc ( 9 );
      if ( sme == NULL )
      {
         gan_err_register ( "gan_fundamental_matrix_fit", GAN_ERROR_FAILURE,
                            "" );
         return GAN_FALSE;
      }

      sme_alloc = GAN_TRUE;
   }

   /* build matrix to be eigendecomposed */
   gan_symeigen_reset ( sme, 9 );
   for ( n--; n >= 0; n--, p1++, p2++ )
      gan_symeigen_increment ( sme, 1.0,
                               p2->x*p1->x, p2->x*p1->y, p2->x*p1->z,
                               p2->y*p1->x, p2->y*p1->y, p2->y*p1->z,
                               p2->z*p1->x, p2->z*p1->y, p2->z*p1->z );

   /* solve for eigenvectors & eigenvalues */
   gan_symeigen_solve ( sme );

   /* extract eigenvector matrix */
   Z = &sme->Z;

   /* F is built using eigenvector corresponding to smallest eigenvalue */
   gan_mat33_fill_q ( F, gel(Z,0,0), gel(Z,1,0), gel(Z,2,0),
                         gel(Z,3,0), gel(Z,4,0), gel(Z,5,0),
                         gel(Z,6,0), gel(Z,7,0), gel(Z,8,0) );

   /* free eigensystem structure if it was allocated */
   if ( sme_alloc )
      gan_symeigen_free ( sme );

   /* return with success */
   return GAN_TRUE;
}

/**
 * \}
 */

/**
 * \}
 */

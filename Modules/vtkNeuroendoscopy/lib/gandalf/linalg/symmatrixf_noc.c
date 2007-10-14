/**
 * File:          $RCSfile: symmatrixf_noc.c,v $
 * Module:        Symmetric fixed size matrix functions
 * Part of:       Gandalf Library
 *
 * Revision:      $Revision: 1.1.2.1 $
 * Last edited:   $Date: 2007/10/14 03:16:24 $
 * Author:        $Author: ruetz $
 * Copyright:     (c) 2000 Imagineer Software Limited
 *
 * Notes:         Not to be compiled separately
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

/* general small symmetric matrix function definitions. They assume that the
 * following symbols have been given values:
 *
 * GAN_MATTYPE: The matrix type for the functions being built, for instance
 *              Gan_SquMatrix22.
 * GAN_VECTYPE1: The type of vector corresponding to a column of the type
 *               represented by GAN_MATTYPE, e.g. Gan_Vector2 for 2x2 matrices.
 */

/**
 * \addtogroup FixedSizeMatrixVectorProduct
 * \{
 */

#ifdef GAN_VEC_OUTER_SYM_S
/**
 * \brief Returns the outer product of vector with itself
 *
 * Computes \f$ p p^{\top} \f$ and returns the matrix result.
 */
GAN_MATTYPE GAN_VEC_OUTER_SYM_S ( const GAN_VECTYPE1 *p )
{
   GAN_MATTYPE A;

   (void)GAN_VEC_OUTER_SYM_Q ( p, &A );
   return A;
}
#endif /* #ifdef GAN_VEC_OUTER_SYM_S */

/**
 * \}
 */

/**
 * \addtogroup FixedSizeMatrixCholesky
 * \{
 */

#ifdef GAN_SYMMAT_CHOLESKY_S
/**
 * \brief Compute Cholesky factorisation of fixed size symmetric matrix.
 *
 * Compute Cholesky factorisation \f$ \mbox{chol}(A) \f$ of symmetric fixed
 * size matrix \a A, returning the result as a lower-triangular matrix \a B,
 * such that
 * \f[
 *   B B^{\top} = A
 * \f]
 * \a A must be positive definite.
 */
GAN_MATTYPE GAN_SYMMAT_CHOLESKY_S ( const GAN_MATTYPE *A )
{
   GAN_MATTYPE B;

   if ( GAN_SYMMAT_CHOLESKY_Q ( (GAN_MATTYPE *)A, &B ) == NULL )
      gan_assert ( 0, "matrix Cholesky failed" );

   return B;
}
#endif /* #ifdef GAN_SYMMAT_CHOLESKY_S */

/**
 * \}
 */

#undef GAN_VEC_OUTER_SYM_Q
#undef GAN_VEC_OUTER_SYM_S
#undef GAN_SYMMAT_CHOLESKY_Q
#undef GAN_SYMMAT_CHOLESKY_S
#undef GAN_SYMMAT_CHOLESKY_I

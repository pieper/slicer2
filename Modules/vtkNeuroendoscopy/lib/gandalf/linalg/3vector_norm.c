/**
 * File:          $RCSfile: 3vector_norm.c,v $
 * Module:        Projective normalisation of 3-vector array (double precision)
 * Part of:       Gandalf Library
 *
 * Revision:      $Revision: 1.1.2.1 $
 * Last edited:   $Date: 2007/10/14 03:16:21 $
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

#include <stdio.h>
#include <math.h>
#include <gandalf/linalg/3vector_norm.h>
#include <gandalf/common/misc_error.h>

/**
 * \addtogroup LinearAlgebra
 * \{
 */

/**
 * \addtogroup FixedSizeMatVec
 * \{
 */

/**
 * \addtogroup FixedSizeVector
 * \{
 */

/**
 * \addtogroup FixedSizeVectorNormalise
 * \{
 */

/**
 * \fn Gan_Bool gan_vec3_normalize ( Gan_Vector3 *b, int n, double term_threshold, int max_iterations, Gan_SquMatrix33 *Lp )
 *\brief Normalize array of 3x1 vectors to identity inertia moment.
 * \param b array of vectors to normalise
 * \param n number of vectors in array
 * \param term_threshold termination threshold
 * \param max_iterations maximum number of iterations
 * \param Lp pointer to solution for triangular normalising matrix
 * \return #GAN_TRUE on success, or #GAN_FAILURE if algorithm didn't converge.
 *
 * This function applies projective normalization to the array of \a n 3x1
 * vectors \a b. After the normalization, the column vectors \a b are
 * transformed by a lower triangular matrix \f$ L \f$, and scaled to unit
 * length, so that the transformed vectors
 * \f[
 *     b'_i = \frac{1}{||L^{-1} b_i||}L^{-1} b_i
 * \f]
 * satisfy the equation
 * \f[
 *    \sum_{i=1\ldots n} (b'_i {b'_i}^{\top}) = \frac{n}{3} I_{3\times 3}
 * \f]
 * where \f$ I_{3\times 3} \f$ is the 3x3 identity matrix.
 *
 * \a term_threshold specifies a threshold on the smallness of an adjustment
 * to \f$ L \f$, below which the algorithm terminates successfully.
 *
 * \a max_iterations specifies the maximum number of iterations to perform.
 * If this is reached without the above threshold being reached, the algorithm
 * terminates with failure, returning #GAN_FALSE.
 *
 * \a Lp is a pointer set to the final solution for the triangular matrix
 * \f$ L \f$. If it is passed as \c NULL then \a Lp is ignored.
 *
 * Upon successful termination, the row vectors \a b are transformed to
 * \f$ b'_i \f$ as shown above, and #GAN_TRUE is returned.
 *
 * \sa gan_vec3_normalize(), gan_mat33_normalize().
 */

#define GAN_VEC_NORMALISE gan_vec3_normalize
#define GAN_VEC_NORM_FNAME  "gan_vec3_normalize"
#define GAN_VECTYPE       Gan_Vector3
#define GAN_SQUMATTYPE    Gan_SquMatrix33

#define GAN_VEC_SCALE_S  gan_vec3_scale_s
#define GAN_VEC_DIVIDE_S gan_vec3_divide_s
#define GAN_VEC_DOT_S    gan_vec3_dot_s
#define GAN_VEC_SQRLEN_S gan_vec3_sqrlen_s
#define GAN_VEC_OUTER_SYM_Q   gan_vec33_outer_sym_q
#define GAN_SYMMAT_IDENT_S    gan_symmat33_ident_s
#define GAN_SYMMAT_INVERT_Q   gan_symmat33_invert_q
#define GAN_SYMMAT_ZERO_Q     gan_symmat33_zero_q
#define GAN_SYMMAT_MULTV_Q    gan_symmat33_multv3_q
#define GAN_SYMMAT_DIVIDE_S   gan_symmat33_divide_s
#define GAN_SYMMAT_DIVIDE_I   gan_symmat33_divide_i
#define GAN_SYMMAT_INCREMENT  gan_symmat33_increment
#define GAN_SYMMAT_TRACE_S    gan_symmat33_trace_s
#define GAN_SYMMAT_SUB_Q      gan_symmat33_sub_q
#define GAN_SYMMAT_SUMSQR_Q   gan_symmat33_sumsqr_q
#define GAN_SYMMAT_CHOLESKY_Q gan_symmat33_cholesky_q
#define GAN_LTMATI_MULTV_S    gan_ltmat33I_multv3_s

#include <gandalf/linalg/vec_norm_noc.c>

#undef GAN_VEC_NORMALISE
#undef GAN_VEC_NORM_FNAME
#undef GAN_VECTYPE
#undef GAN_SQUMATTYPE
#undef GAN_VEC_SCALE_S
#undef GAN_VEC_DIVIDE_S
#undef GAN_VEC_DOT_S
#undef GAN_VEC_SQRLEN_S
#undef GAN_VEC_OUTER_SYM_Q
#undef GAN_SYMMAT_IDENT_S
#undef GAN_SYMMAT_INVERT_Q
#undef GAN_SYMMAT_ZERO_Q
#undef GAN_SYMMAT_MULTV_Q
#undef GAN_SYMMAT_DIVIDE_S
#undef GAN_SYMMAT_DIVIDE_I
#undef GAN_SYMMAT_INCREMENT
#undef GAN_SYMMAT_TRACE_S
#undef GAN_SYMMAT_SUB_Q
#undef GAN_SYMMAT_SUMSQR_Q
#undef GAN_SYMMAT_CHOLESKY_Q
#undef GAN_LTMATI_MULTV_S

/**
 * \}
 */

/**
 * \}
 */

/**
 * \}
 */

/**
 * \}
 */

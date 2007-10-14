/**
 *
 * File:          $RCSfile: squmatrixf_noc.c,v $
 * Module:        Fixed size square matrix functions
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

/* general fixed size square matrix function definitions. They assume that the
 * following symbols have been given values:
 *
 * GAN_MATTYPE: The matrix type for the functions being built, for instance
 *              Gan_Matrix22.
 * GAN_VECTYPE1: The type of vector corresponding to a column of the type
 *               represented by GAN_MATTYPE, e.g. Gan_Vector2 for 2x2 matrices.
 */

/**
 * \addtogroup FixedSizeMatrixFill
 * \{
 */

#ifdef GAN_SQUMAT_IDENT_S
/**
 * \brief Return fixed size square matrix filled with identity.
 *
 * Fill square matrix with identity and return as new matrix.
 */
GAN_MATTYPE GAN_SQUMAT_IDENT_S (void)
{
   GAN_MATTYPE A;

   (void)GAN_SQUMAT_IDENT_Q ( &A );
   return A;
}
#endif /* #ifdef GAN_SQUMAT_IDENT_S */

/**
 * \}
 */

/**
 * \addtogroup FixedSizeMatrixTranspose
 * \{
 */

#ifdef GAN_SQUMAT_TPOSE_S
/**
 * \brief Transpose fixed size square matrix.
 *
 * Return a transpose copy of fixed size square matrix \a A.
 */
GAN_MATTYPE GAN_SQUMAT_TPOSE_S ( const GAN_MATTYPE *A )
{
   GAN_MATTYPE B;

   (void)GAN_SQUMAT_TPOSE_Q ( (GAN_MATTYPE *)A, &B );
   return B;
}
#endif /* #ifdef GAN_SQUMAT_TPOSE_S */

/**
 * \}
 */

/**
 * \addtogroup FixedSizeMatrixAdd
 * \{
 */

#ifdef GAN_MAT_SADDT_S
/**
 * \brief Add matrix to its own transpose.
 *
 * Add fixed size square matrix \a A to its own transpose, returning the result
 * as a new symmetric fixed size matrix \f$ A + A^{\top} \f$.
 */
GAN_SQUMATTYPER GAN_MAT_SADDT_S ( const GAN_MATTYPE *A )
{
   GAN_SQUMATTYPER B;

   (void)GAN_MAT_SADDT_Q ( A, &B );
   return B;
}
#endif /* #ifdef GAN_MAT_SADDT_S */

/**
 * \}
 */

/**
 * \addtogroup FixedSizeMatrixVectorProduct
 * \{
 */

#ifdef GAN_SQUMATI_MULTV_S
/**
 * \brief Computes the product of the inverse of a matrix and a vector
 *
 * Returns the product \f$ A^{-1} p \f$ of the inverse of matrix \a A and
 * vector \a p as a new fixed size vector.
 */
GAN_VECTYPE1 GAN_SQUMATI_MULTV_S ( const GAN_MATTYPE *A, const GAN_VECTYPE1 *p )
{
   GAN_VECTYPE1 q;

   (void)GAN_SQUMATI_MULTV_Q ( A, p, &q );
   return q;
}
#endif /* #ifdef GAN_SQUMATI_MULTV_S */

#ifdef GAN_SQUMATIT_MULTV_S
/**
 * \brief Computes the product of the inverse transpose of a matrix and a vector
 *
 * Returns the product \f$ A^{-{\top}} p \f$ of the inverse transpose of
 * matrix \a A and vector \a p as a new fixed size vector.
 */
GAN_VECTYPE1 GAN_SQUMATIT_MULTV_S ( const GAN_MATTYPE *A, const GAN_VECTYPE1 *p )
{
   GAN_VECTYPE1 q;

   (void)GAN_SQUMATIT_MULTV_Q ( A, p, &q );
   return q;
}
#endif /* #ifdef GAN_SQUMATIT_MULTV_S */

/**
 * \}
 */

/**
 * \addtogroup FixedSizeMatrixInvert
 * \{
 */

#ifdef GAN_SQUMAT_INVERT_S
/**
 * \brief Invert fixed size square matrix.
 *
 * Compute inverse \f$ A^{-1} \f$ of fixed size square matrix \a A,
 * returning the result as a new fixed size square matrix.
 */
GAN_MATTYPE GAN_SQUMAT_INVERT_S ( const GAN_MATTYPE *A )
{
   GAN_MATTYPE B;

   if ( GAN_SQUMAT_INVERT_Q ( (GAN_MATTYPE *)A, &B ) == NULL )
      assert(0);
      
   return B;
}
#endif /* #ifdef GAN_SQUMAT_INVERT_S */

/**
 * \}
 */

/**
 * \addtogroup FixedSizeMatrixAdjoint
 * \{
 */

#ifdef GAN_SQUMAT_ADJOINT_S
/**
 * \brief Compute adjoint of fixed size square matrix.
 *
 * Compute adjoint \f$ A^{*} \f$ of fixed size square matrix \a A, so that
 * \f[
 *    A^{*} A^{\top} = \mbox{det}(A) I
 * \f]
 * returning the result as a new fixed size square matrix.
 */
GAN_MATTYPE GAN_SQUMAT_ADJOINT_S ( const GAN_MATTYPE *A )
{
   GAN_MATTYPE B;

   if ( GAN_SQUMAT_ADJOINT_Q ( A, &B ) == NULL )
      assert(0);

   return B;
}
#endif /* #ifdef GAN_SQUMAT_ADJOINT_S */

#ifdef GAN_SQUMAT_ADJOINTT_S
/**
 * \brief Compute adjoint transpose of fixed size square matrix.
 *
 * Compute adjoint transpose \f$ A^{*{\top}} \f$ of fixed size square matrix
 * \a A, so that
 * \f[
 *     A^{*{\top}} A = \mbox{det}(A) I
 * \f]
 * returning the result as a new fixed size square matrix.
 */
GAN_MATTYPE GAN_SQUMAT_ADJOINTT_S ( const GAN_MATTYPE *A )
{
   GAN_MATTYPE B;

   if ( GAN_SQUMAT_ADJOINTT_Q ( A, &B ) == NULL )
      assert(0);

   return B;
}
#endif /* #ifdef GAN_SQUMAT_ADJOINTT_S */

/**
 * \}
 */

/**
 * \addtogroup FixedSizeMatrixNorms
 * \{
 */

#ifdef GAN_SQUMAT_DET_S
/**
 * \brief Determinant of square matrix.
 *
 * Returns determinant of square matrix \a A.
 */
GAN_REALTYPE GAN_SQUMAT_DET_S ( const GAN_MATTYPE *A )
{
   return GAN_SQUMAT_DET_Q ( A );
}
#endif /* #ifdef GAN_SQUMAT_DET_S */

#ifdef GAN_SQUMAT_TRACE_S
/**
 * \brief Trace of square matrix.
 *
 * Returns trace of square matrix \a A.
 */
GAN_REALTYPE GAN_SQUMAT_TRACE_S ( const GAN_MATTYPE *A )
{
   return GAN_SQUMAT_TRACE_Q ( A );
}
#endif /* #ifdef GAN_SQUMAT_TRACE_S */

/**
 * \}
 */

#undef GAN_SQUMAT_IDENT_Q
#undef GAN_SQUMAT_IDENT_S
#undef GAN_SQUMAT_TPOSE_Q
#undef GAN_SQUMAT_TPOSE_S
#undef GAN_SQUMAT_TPOSE_I
#undef GAN_MAT_SADDT_Q
#undef GAN_MAT_SADDT_S
#undef GAN_SQUMATI_MULTV_Q
#undef GAN_SQUMATI_MULTV_S
#undef GAN_SQUMATI_MULTV_I
#undef GAN_SQUMATIT_MULTV_Q
#undef GAN_SQUMATIT_MULTV_S
#undef GAN_SQUMATIT_MULTV_I
#undef GAN_SQUMAT_INVERT
#undef GAN_SQUMAT_INVERT_Q
#undef GAN_SQUMAT_INVERT_S
#undef GAN_SQUMAT_INVERT_I
#undef GAN_SQUMAT_ADJOINT_Q
#undef GAN_SQUMAT_ADJOINT_S
#undef GAN_SQUMAT_ADJOINTT_Q
#undef GAN_SQUMAT_ADJOINTT_S
#undef GAN_SQUMAT_DET_Q
#undef GAN_SQUMAT_DET_S
#undef GAN_SQUMAT_TRACE_Q
#undef GAN_SQUMAT_TRACE_S

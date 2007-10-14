/**
 * File:          $RCSfile: matrixf_noc.c,v $
 * Module:        Fixed size matrix functions
 * Part of:       Gandalf Library
 *
 * Revision:      $Revision: 1.1.2.1 $
 * Last edited:   $Date: 2007/10/14 03:16:23 $
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

/* general small matrix function definitions. They assume that the following
 * symbols have been given values:
 *
 * GAN_MATTYPE: The matrix type for the functions being built, for instance
 *              Gan_Matrix22.
 */

#include <gandalf/common/endian_io.h>

/**
 * \addtogroup FixedSizeMatrixIO
 * \{
 */

#ifdef GAN_MAT_PRINT
/**
 * \brief Print fixed size matrix to standard output.
 *
 * Print fixed size matrix \a A to standard output, with prefix string
 * \a prefix, indentation \a indent and floating-point format \a fmt.
 *
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 */
Gan_Bool
 GAN_MAT_PRINT ( const GAN_MATTYPE *A,
                 const char *prefix, int indent, const char *fmt )
{
   return GAN_MAT_FPRINT ( stdout, A, prefix, indent, fmt );
}
#endif /* #ifdef GAN_MAT_PRINT */

#ifdef GAN_MAT_FWRITE
/**
 * \brief Write fixed size matrix to file stream
 *
 * Write fixed size matrix \a A to file stream \a fp, prefixed with
 * \a magic_number to identify the matrix for later reading.
 *
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 */
Gan_Bool
 GAN_MAT_FWRITE ( FILE *fp, const GAN_MATTYPE *A, gan_uint32 magic_number )
{
   if ( !gan_fwrite_lendian_ui32 ( fp, &magic_number, 1 ) )
   {
      gan_err_register ( "GAN_MAT_FWRITE", GAN_ERROR_CORRUPTED_FILE, "" );
      return GAN_FALSE;
   }

   if ( !GAN_FWRITE_LENDIAN ( fp, (GAN_REALTYPE *)A,
                              sizeof(GAN_MATTYPE)/sizeof(GAN_REALTYPE) ) )
   {
      gan_err_register ( "GAN_MAT_FWRITE", GAN_ERROR_CORRUPTED_FILE, "" );
      return GAN_FALSE;
   }

   return GAN_TRUE;
}
#endif /* #ifdef GAN_MAT_FWRITE */

#ifdef GAN_MAT_FREAD
/**
 * \brief Read fixed size matrix from file stream
 *
 * Read fixed size matrix \a A from file stream \a fp, prefixed by
 * a number written into the \a magic_number pointer.
 *
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 */
Gan_Bool
 GAN_MAT_FREAD ( FILE *fp, GAN_MATTYPE *A, gan_uint32 *magic_number )
{
   if ( !gan_fread_lendian_ui32 ( fp, magic_number, 1 ) )
   {
      gan_err_register ( "GAN_MAT_FREAD", GAN_ERROR_CORRUPTED_FILE, "" );
      return GAN_FALSE;
   }

   if ( !GAN_FREAD_LENDIAN ( fp, (GAN_REALTYPE *)A,
                             sizeof(GAN_MATTYPE)/sizeof(GAN_REALTYPE) ) )
   {
      gan_err_register ( "GAN_MAT_FREAD", GAN_ERROR_CORRUPTED_FILE, "" );
      return GAN_FALSE;
   }

   /* set matrix type if specified */
#ifdef GAN_FIXED_MATRIX_TYPE
   gan_eval ( A->type = GAN_FIXED_MATRIX_TYPE );
#endif
   return GAN_TRUE;
}
#endif /* #ifdef GAN_MAT_FREAD */

/**
 * \}
 */

/**
 * \addtogroup FixedSizeMatrixFill
 * \{
 */

#ifdef GAN_MAT_ZERO_S
/**
 * \brief Set fixed size matrix to zero.
 *
 * Return fixed size matrix filled with zero values.
 */
GAN_MATTYPE
 GAN_MAT_ZERO_S (void)
{
   GAN_MATTYPE A;

   (void)GAN_MAT_ZERO_Q ( &A );
   return A;
}
#endif /* #ifdef GAN_MAT_ZERO_S */

/**
 * \}
 */

/**
 * \addtogroup FixedSizeMatrixCopy
 * \{
 */

#ifdef GAN_MAT_COPY_S
/**
 * \brief Copy fixed size matrix.
 *
 * Copy fixed size matrix \a A, returning the result as a new fixed size
 * matrix.
 */
GAN_MATTYPE
 GAN_MAT_COPY_S ( const GAN_MATTYPE *A )
{
   GAN_MATTYPE B;

   (void)GAN_MAT_COPY_Q ( A, &B );
   return B;
}
#endif /* #ifdef GAN_MAT_COPY_S */

/**
 * \}
 */

/**
 * \addtogroup FixedSizeMatrixScale
 * \{
 */

#ifdef GAN_MAT_SCALE_S
/**
 * \brief Scale fixed size matrix.
 *
 * Return a copy of fixed size matrix \a A multiplied by scalar \a a.
 */
GAN_MATTYPE
 GAN_MAT_SCALE_S ( const GAN_MATTYPE *A, GAN_REALTYPE a )
{
   GAN_MATTYPE B;

   (void)GAN_MAT_SCALE_Q ( A, a, &B );
   return B;
}
#endif /* #ifdef GAN_MAT_SCALE_S */

#ifdef GAN_MAT_DIVIDE_S
/**
 * \brief Divide fixed size matrix by scalar.
 *
 * Return a copy of fixed size matrix \a A divided by scalar \a a.
 */
GAN_MATTYPE
 GAN_MAT_DIVIDE_S ( const GAN_MATTYPE *A, GAN_REALTYPE a )
{
   GAN_MATTYPE B;

   (void)GAN_MAT_DIVIDE_Q ( A, a, &B );
   return B;
}
#endif /* #ifdef GAN_MAT_DIVIDE_S */

#ifdef GAN_MAT_NEGATE_S
/**
 * \brief Negate fixed size matrix.
 *
 * Return a negated copy of fixed size matrix \a A.
 */
GAN_MATTYPE
 GAN_MAT_NEGATE_S ( const GAN_MATTYPE *A )
{
   GAN_MATTYPE B;

   (void)GAN_MAT_NEGATE_Q ( A, &B );
   return B;
}
#endif /* #ifdef GAN_MAT_NEGATE_S */

#ifdef GAN_MAT_UNIT_Q
/**
 * \brief Scale matrix to unit norm.
 *
 * Scale fixed size matrix \a A to unit Frobenius norm, filling \a B with the
 * result.
 *
 * \return Scaled matrix \a B.
 */
GAN_MATTYPE *
 GAN_MAT_UNIT_Q ( GAN_MATTYPE *A, GAN_MATTYPE *B )
{
   GAN_REALTYPE norm = (GAN_REALTYPE) GAN_MAT_FNORM_Q ( A );

   if ( norm == 0.0 )
   {
      gan_err_flush_trace();
      gan_err_register ( "GAN_MAT_UNIT_Q", GAN_ERROR_DIVISION_BY_ZERO, "" );
      return NULL;
   }

   return GAN_MAT_DIVIDE_Q ( A, norm, B );
}
#endif /* #ifdef GAN_MAT_UNIT_Q */

#ifdef GAN_MAT_UNIT_S
/**
 * \brief Scale matrix to unit norm.
 *
 * Scale fixed size matrix \a A to unit Frobenius norm.
 *
 * \return Scaled matrix \a A.
 */
GAN_MATTYPE
 GAN_MAT_UNIT_S ( const GAN_MATTYPE *A )
{
   GAN_MATTYPE B;

   if ( GAN_MAT_UNIT_Q ( (GAN_MATTYPE *)A, &B ) == NULL )
      assert(0);

   return B;
}
#endif /* #ifdef GAN_MAT_UNIT_S */

/**
 * \}
 */

/**
 * \addtogroup FixedSizeMatrixAdd
 * \{
 */

#ifdef GAN_MAT_ADD_S
/**
 * \brief Add two fixed size matrices.
 *
 * Add two fixed size matrices \a A, \a B and return the result as a new
 * fixed size matrix \f$ A + B \f$.
 */
GAN_MATTYPE
 GAN_MAT_ADD_S ( const GAN_MATTYPE *A, const GAN_MATTYPE *B )
{
   GAN_MATTYPE C;

   (void)GAN_MAT_ADD_Q ( A, B, &C );
   return C;
}
#endif /* #ifdef GAN_MAT_ADD_S */

/**
 * \}
 */

/**
 * \addtogroup FixedSizeMatrixSubtract
 * \{
 */

#ifdef GAN_MAT_SUB_S
/**
 * \brief Subtract two fixed size matrices.
 *
 * Subtract two fixed size matrices \a A, \a B and return the result as a new
 * fixed size matrix \f$ A - B \f$.
 */
GAN_MATTYPE
 GAN_MAT_SUB_S ( const GAN_MATTYPE *A, const GAN_MATTYPE *B )
{
   GAN_MATTYPE C;

   (void)GAN_MAT_SUB_Q ( A, B, &C );
   return C;
}
#endif /* #ifdef GAN_MAT_SUB_S */

/**
 * \}
 */

/**
 * \addtogroup FixedSizeMatrixVectorProduct
 * \{
 */

#ifdef GAN_VEC_OUTER_S
/**
 * \brief Compute the outer product of two fixed size vectors.
 *
 * Returns the outer product \f$ p q^{\top} \f$ of two vectors \a p and \a q.
 */
GAN_MATTYPE
 GAN_VEC_OUTER_S ( const GAN_VECTYPE1 *p, const GAN_VECTYPE2 *q )
{
   GAN_MATTYPE A;

   (void)GAN_VEC_OUTER_Q ( p, q, &A );
   return A;
}
#endif /* #ifdef GAN_VEC_OUTER_S */

#ifdef GAN_MAT_MULTV_S
/**
 * \brief Matrix/vector product.
 *
 * Returns the product \f$ A p \f$ of a matrix \a A and a vector \a p
 */
GAN_VECTYPE1
 GAN_MAT_MULTV_S ( const GAN_MATTYPE *A, const GAN_VECTYPE2 *p )
{
   GAN_VECTYPE1 q;

   (void)GAN_MAT_MULTV_Q ( A, p, &q );
   return q;
}
#endif /* #ifdef GAN_MAT_MULTV_S */

#ifdef GAN_MATT_MULTV_S
/**
 * \brief Matrix/vector product with the matrix transposed.
 *
 * Returns the product \f$ A^{\top} p \f$ of the transpose of matrix \a A and
 * vector \a p.
 */
GAN_VECTYPE2
 GAN_MATT_MULTV_S ( const GAN_MATTYPE *A, const GAN_VECTYPE1 *p )
{
   GAN_VECTYPE2 q;

   (void)GAN_MATT_MULTV_Q ( A, p, &q );
   return q;
}
#endif /* #ifdef GAN_MATT_MULTV_S */

/**
 * \}
 */

/**
 * \addtogroup FixedSizeMatrixMatrixProduct
 * \{
 */

#ifdef GAN_MAT_LMULTM_S
/**
 * \brief Left-multiply a matrix by another matrix.
 *
 * Returns the result \f$ B A \f$ of left-multiplying a matrix \a A with
 * another matrix \a B. \a B is square but of generic type #GAN_MATTYPEL.
 */
GAN_MATTYPE
 GAN_MAT_LMULTM_S ( const GAN_MATTYPE *A, const GAN_MATTYPEL *B )
{
   GAN_MATTYPE C;

   (void)GAN_MAT_LMULTM_Q ( A, B, &C );
   return C;
}
#endif /* #ifdef GAN_MAT_LMULTM_S */

#ifdef GAN_MAT_LMULTMT_S
/**
 * \brief Left-multiply a matrix by the transpose of another matrix.
 *
 * Returns the result \f$ B^{\top} A \f$ of left-multiplying a matrix \a A
 * with the transpose of a square matrix \a B of generic type #GAN_MATTYPEL.
 */
GAN_MATTYPE
 GAN_MAT_LMULTMT_S ( const GAN_MATTYPE *A, const GAN_MATTYPEL *B )
{
   GAN_MATTYPE C;

   (void)GAN_MAT_LMULTMT_Q ( A, B, &C );
   return C;
}
#endif /* #ifdef GAN_MAT_LMULTMT_S */

#ifdef GAN_MAT_RMULTM_S
/**
 * \brief Right-multiply a matrix by another matrix.
 *
 * Returns the result \f$ A B \f$ of right-multiplying a matrix \a A with a
 * square matrix \a B of generic type #GAN_MATTYPER.
 */
GAN_MATTYPE
 GAN_MAT_RMULTM_S ( const GAN_MATTYPE *A, const GAN_MATTYPER *B )
{
   GAN_MATTYPE C;

   (void)GAN_MAT_RMULTM_Q ( A, B, &C );
   return C;
}
#endif /* #ifdef GAN_MAT_RMULTM_S */

#ifdef GAN_MAT_RMULTMT_S
/**
 * \brief Right-multiply a matrix by the transpose of another matrix.
 *
 * Returns the result \f$ A B^{\top} \f$ of right-multiplying a matrix \a A
 * with the transpose of a square matrix \a B of generic type #GAN_MATTYPER.
 */
GAN_MATTYPE
 GAN_MAT_RMULTMT_S ( const GAN_MATTYPE *A, const GAN_MATTYPER *B )
{
   GAN_MATTYPE C;

   (void)GAN_MAT_RMULTMT_Q ( A, B, &C );
   return C;
}
#endif /* #ifdef GAN_MAT_RMULTMT_S */

#ifdef GAN_MAT_LMULTMT_SYM_S
/**
 * \brief Matrix product producing a symmetric matrix.
 *
 * Returns the result \f$ B^{\top} A \f$ of left-multiplying a matrix \a A
 * with the transpose of a matrix \a B of the same type type, producing a
 * symmetric matrix. Note that the result is \em assumed to be symmetric,
 * so this function should be used with care.
 */
GAN_SQUMATTYPER
 GAN_MAT_LMULTMT_SYM_S ( const GAN_MATTYPE *A, const GAN_MATTYPE *B )
{
   GAN_SQUMATTYPER C;

   (void)GAN_MAT_LMULTMT_SYM_Q ( A, B, &C );
   return C;
}
#endif /* #ifdef GAN_MAT_LMULTMT_SYM_S */

#ifdef GAN_MAT_RMULTMT_SYM_S
/**
 * \brief Matrix product producing a symmetric matrix.
 *
 * Returns the result \f$ A B^{\top} \f$ of right-multiplying a matrix \a A
 * with the transpose of a matrix \a B of the same type type, producing a
 * symmetric matrix. Note that the result is \em assumed to be symmetric,
 * so this function should be used with care.
 */
GAN_SQUMATTYPEL
 GAN_MAT_RMULTMT_SYM_S ( const GAN_MATTYPE *A, const GAN_MATTYPE *B )
{
   GAN_SQUMATTYPEL C;

   (void)GAN_MAT_RMULTMT_SYM_Q ( A, B, &C );
   return C;
}
#endif /* #ifdef GAN_MAT_RMULTMT_SYM_S */

#ifdef GAN_MAT_SLMULTT_S
/**
 * \brief Left-multiply a matrix by its own transpose, producing a symmetric matrix.
 *
 * Left multiply a fixed-size matrix \a A by its own transpose, producing
 * a symmetric fixed-size matrix \f$ A^{\top} A \f$.
 *
 * \return The result \f$ A^{\top} A \f$ as a new matrix.
 */
GAN_SQUMATTYPER GAN_MAT_SLMULTT_S ( const GAN_MATTYPE *A )
{
   GAN_SQUMATTYPER B;

   (void)GAN_MAT_SLMULTT_Q ( A, &B );
   return B;
}
#endif

#ifdef GAN_MAT_SRMULTT_S
/**
 * \brief Right-multiply a matrix by its own transpose, producing a symmetric matrix.
 *
 * Right multiply a fixed size matrix \a A by its own transpose, producing
 * a symmetric fixed-size matrix \f$ A A^{\top} \f$.
 *
 * \return The result \f$ A A^{\top} \f$ as a new matrix.
 */
GAN_SQUMATTYPEL GAN_MAT_SRMULTT_S ( const GAN_MATTYPE *A )
{
   GAN_SQUMATTYPEL B;

   (void)GAN_MAT_SRMULTT_Q ( A, &B );
   return B;
}
#endif

#ifdef GAN_MAT_LMULTS_S
/**
 * \brief Left-multiply a matrix by a symmetric matrix.
 *
 * Returns the result \f$ B A \f$ of left-multiplying a matrix \a A with a
 * symmetric matrix \a B.
 */
GAN_MATTYPE GAN_MAT_LMULTS_S ( const GAN_MATTYPE *A, const GAN_SQUMATTYPEL *B )
{
   GAN_MATTYPE C;

   (void)GAN_MAT_LMULTS_Q ( A, B, &C );
   return C;
}
#endif /* #ifdef GAN_MAT_LMULTS_S */

#ifdef GAN_MAT_RMULTS_S
/**
 * \brief Right-multiply a matrix by a symmetric matrix.
 *
 * Returns the result \f$ A B \f$ of right-multiplying a matrix \a A with a
 * symmetric matrix \a B.
 */
GAN_MATTYPE
 GAN_MAT_RMULTS_S ( const GAN_MATTYPE *A, const GAN_SQUMATTYPER *B )
{
   GAN_MATTYPE C;

   (void)GAN_MAT_RMULTS_Q ( A, B, &C );
   return C;
}
#endif /* #ifdef GAN_MAT_RMULTS_S */

/**
 * \}
 */

/**
 * \addtogroup FixedSizeTripleMatrixProduct
 * \{
 */

#ifdef GAN_SYMMATL_LRMULT_S
/**
 * \brief Matrix triple product involving symmetric matrices.
 *
 * Returns the triple product \f$ B^{\top} A B \f$ as a symmetric matrix,
 * for generic matrix \a B and symmetric matrix \a A. The intermediate result
 * \f$ A B \f$ is written into matrix \a C, which can be passed as \c NULL if
 * you don't need this result.
 */
GAN_SQUMATTYPER
 GAN_SYMMATL_LRMULT_S ( const GAN_SQUMATTYPEL *A, const GAN_MATTYPE *B, GAN_MATTYPE *C )
{
   GAN_SQUMATTYPER D;
   GAN_MATTYPE Cp;

   (void)GAN_SYMMATL_LRMULT_Q ( A, B, (C == NULL) ? &Cp : C, &D );
   return D;
}
#endif

#ifdef GAN_SYMMATR_LRMULT_S
/**
 * \brief Matrix triple product involving symmetric matrices.
 *
 * Returns the triple product \f$ B A B^{\top} \f$ as a symmetric matrix,
 * for generic matrix \a B and symmetric matrix \a A.
 * The intermediate result \f$ B A \f$ is written into matrix \a C, which can
 * be passed as \c NULL if you don't need this result.
 */
GAN_SQUMATTYPEL
 GAN_SYMMATR_LRMULT_S ( const GAN_SQUMATTYPER *A, const GAN_MATTYPE *B, GAN_MATTYPE *C )
{
   GAN_SQUMATTYPEL D;
   GAN_MATTYPE Cp;

   (void)GAN_SYMMATR_LRMULT_Q ( A, B, (C == NULL) ? &Cp : C, &D );
   return D;
}
#endif

/**
 * \}
 */

/**
 * \addtogroup FixedSizeMatrixMatrixProduct
 * \{
 */

#ifdef GAN_MAT_LMULTL_S
/**
 * \brief Left-multiply a matrix by a lower-triangular matrix.
 *
 * Returns the result \f$ B A \f$ of left-multiplying a matrix \a A with a
 * lower-triangular matrix \a B.
 */
GAN_MATTYPE
 GAN_MAT_LMULTL_S ( const GAN_MATTYPE *A, const GAN_SQUMATTYPEL *B )
{
   GAN_MATTYPE C;

   (void)GAN_MAT_LMULTL_Q ( A, B, &C );
   return C;
}
#endif /* #ifdef GAN_MAT_LMULTL_S */

#ifdef GAN_MAT_LMULTLI_S
/**
 * \brief Left-multiply a matrix by the inverse of a lower-triangular matrix.
 *
 * Returns the result \f$ B^{-1} A \f$ of left-multiplying a matrix \a A
 * with the inverse of a lower-triangular matrix \a B.
 */
GAN_MATTYPE
 GAN_MAT_LMULTLI_S ( const GAN_MATTYPE *A, const GAN_SQUMATTYPEL *B )
{
   GAN_MATTYPE C;

   (void)GAN_MAT_LMULTLI_Q ( A, B, &C );
   return C;
}
#endif /* #ifdef GAN_MAT_LMULTL_S */

#ifdef GAN_MAT_LMULTLT_S
/**
 * \brief Left-multiply a matrix by the transpose of a lower-triangular matrix.
 *
 * Returns the result \f$ B^{\top} A \f$ of left-multiplying a matrix \a A
 * with the transpose of a lower-triangular matrix \a B.
 */
GAN_MATTYPE
 GAN_MAT_LMULTLT_S ( const GAN_MATTYPE *A, const GAN_SQUMATTYPEL *B )
{
   GAN_MATTYPE C;

   (void)GAN_MAT_LMULTLT_Q ( A, B, &C );
   return C;
}
#endif /* #ifdef GAN_MAT_LMULTLT_S */

#ifdef GAN_MAT_LMULTLIT_S
/**
 * \brief Left-multiply a matrix by the inverse transpose of a lower-triangular matrix.
 *
 * Returns the result \f$ B^{-{\top}} A \f$ of left-multiplying a matrix \a A
 * with the inverse transpose of a lower-triangular matrix \a B.
 */
GAN_MATTYPE
 GAN_MAT_LMULTLIT_S ( const GAN_MATTYPE *A, const GAN_SQUMATTYPEL *B )
{
   GAN_MATTYPE C;

   (void)GAN_MAT_LMULTLIT_Q ( A, B, &C );
   return C;
}
#endif /* #ifdef GAN_MAT_LMULTLIT_S */

#ifdef GAN_MAT_RMULTL_S
/**
 * \brief Right-multiply a matrix by a lower-triangular matrix.
 *
 * Returns the result \f$ A B \f$ of right-multiplying a matrix \a A with a
 * lower-triangular matrix \a B.
 */
GAN_MATTYPE
 GAN_MAT_RMULTL_S ( const GAN_MATTYPE *A, const GAN_SQUMATTYPER *B )
{
   GAN_MATTYPE C;

   (void)GAN_MAT_RMULTL_Q ( A, B, &C );
   return C;
}
#endif /* #ifdef GAN_MAT_RMULTL_S */

#ifdef GAN_MAT_RMULTLI_S
/**
 * \brief Right-multiply a matrix by the inverse of a lower-triangular matrix.
 *
 * Returns the result \f$ A B^{-1} \f$ of right-multiplying a matrix \a A
 * with the inverse of a lower-triangular matrix \a B.
 */
GAN_MATTYPE
 GAN_MAT_RMULTLI_S ( const GAN_MATTYPE *A, const GAN_SQUMATTYPER *B )
{
   GAN_MATTYPE C;

   (void)GAN_MAT_RMULTLI_Q ( A, B, &C );
   return C;
}
#endif /* #ifdef GAN_MAT_RMULTL_S */

#ifdef GAN_MAT_RMULTLT_S
/**
 * \brief Right-multiply a matrix by the transpose of a lower-triangular matrix.
 *
 * Returns the result \f$ A B^{\top} \f$ of right-multiplying a matrix \a A
 * with the transpose of a lower-triangular matrix \a B.
 */
GAN_MATTYPE
 GAN_MAT_RMULTLT_S ( const GAN_MATTYPE *A, const GAN_SQUMATTYPER *B )
{
   GAN_MATTYPE C;

   (void)GAN_MAT_RMULTLT_Q ( A, B, &C );
   return C;
}
#endif /* #ifdef GAN_MAT_RMULTLT_S */

#ifdef GAN_MAT_RMULTLIT_S
/**
 * \brief Right-multiply a matrix by the inverse transpose of a lower-triangular matrix
 *
 * Returns the result \f$ A B^{-{\top}} \f$ of right-multiplying a matrix \a A
 * with the inverse transpose of a lower-triangular matrix \a B.
 */
GAN_MATTYPE
 GAN_MAT_RMULTLIT_S ( const GAN_MATTYPE *A, const GAN_SQUMATTYPER *B )
{
   GAN_MATTYPE C;

   (void)GAN_MAT_RMULTLIT_Q ( A, B, &C );
   return C;
}
#endif /* #ifdef GAN_MAT_RMULTLIT_S */

/**
 * \}
 */

/**
 * \addtogroup FixedSizeMatrixNorms
 * \{
 */

#ifdef GAN_MAT_SUMSQR_S
/**
 * \brief Returns sum of squares of matrix elements.
 *
 * Returns sum of squares of elements of matrix \a A. Implemented as a
 * function.
 */
GAN_REALTYPE
 GAN_MAT_SUMSQR_S ( const GAN_MATTYPE *A )
{
   return (GAN_REALTYPE) GAN_MAT_SUMSQR_Q ( A );
}
#endif /* #ifdef GAN_MAT_SUMSQR_S */

#ifdef GAN_MAT_FNORM_S
/**
 * \brief Frobenius norm of matrix.
 *
 * Returns Frobenius norm of matrix \a A, defined as the square-root of the
 * sum of squares of the elements of \a A. Implemented as a function.
 */
GAN_REALTYPE
 GAN_MAT_FNORM_S ( const GAN_MATTYPE *A )
{
   return (GAN_REALTYPE) GAN_MAT_FNORM_Q ( A );
}
#endif /* #ifdef GAN_MAT_FNORM_S */

/**
 * \}
 */

/**
 * \addtogroup FixedSizeMatrixConvert
 * \{
 */

#ifdef GAN_MAT_FROM_MAT_S
/**
 * \brief Convert generic rectangular matrix to fixed size matrix structure.
 *
 * Convert generic rectangular matrix \a A to fixed size matrix structure,
 * returning the result as a new matrix.
 */
GAN_MATTYPE
 GAN_MAT_FROM_MAT_S ( const GAN_MATRIX_TYPE *A )
{
   GAN_MATTYPE B;

   if ( GAN_MAT_FROM_MAT_Q ( A, &B ) == NULL )
      gan_assert ( 0, "conversion from matrix failed" );

   return B;
}
#endif /* #ifdef GAN_MAT_FROM_MAT_S */

#ifdef GAN_MAT_FROM_SQUMAT_S
/**
 * \brief Convert square matrix to fixed size matrix structure.
 *
 * Convert square matrix \a A to fixed size matrix structure. returning the
 * result as a new matrix.
 */
GAN_MATTYPE
 GAN_MAT_FROM_SQUMAT_S ( const GAN_SQUMATRIX_TYPE *A )
{
   GAN_MATTYPE B;

   if ( GAN_MAT_FROM_SQUMAT_Q ( A, &B ) == NULL )
      gan_assert ( 0, "conversion from square matrix failed" );

   return B;
}
#endif /* #ifdef GAN_MAT_FROM_SQUMAT_S */

#ifdef GAN_SQUMAT_FROM_SQUMAT_S
/**
 * \brief Convert a square matrix to a symmetric fixed size matrix.
 *
 * Return the result of converting a square matrix \a A to a symmetric
 * fixed size matrix.
 */
GAN_SQUMATTYPEL
 GAN_SQUMAT_FROM_SQUMAT_S ( const GAN_SQUMATRIX_TYPE *A )
{
   GAN_SQUMATTYPEL B;

   if ( GAN_SQUMAT_FROM_SQUMAT_Q ( A, &B ) == NULL )
      gan_assert ( 0, "conversion from square matrix failed" );

   return B;
}
#endif /* #ifdef GAN_SQUMAT_FROM_SQUMAT_S */

/**
 * \}
 */

#include <gandalf/linalg/matrixf_undef_noc.h>

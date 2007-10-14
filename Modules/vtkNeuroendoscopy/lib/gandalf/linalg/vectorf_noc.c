/**
 * File:          $RCSfile: vectorf_noc.c,v $
 * Module:        Fixed size vector functions
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

/* general small vector function definitions. They assume that the following
 * symbols have been given values:
 *
 * GAN_VECTYPE: The vector type for the functions being built, for instance
 *              Gan_Vector2.
 */

#include <gandalf/common/endian_io.h>


/**
 * \addtogroup FixedSizeVectorIO
 * \{
 */

/**
 * \brief Print fixed size vector to standard output.
 *
 * Print fixed size vector \a p to standard output, with prefix string
 * \a prefix, indentation \a indent and floating-point format \a fmt.
 *
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 */
Gan_Bool GAN_VEC_PRINT ( const GAN_VECTYPE *p,
                         const char *prefix, int indent, const char *fmt )
{
   return GAN_VEC_FPRINT ( stdout, p, prefix, indent, fmt );
}

/**
 * \brief Write fixed size vector to file stream.
 *
 * Write fixed size vector \a p to file stream \a fp, prefixed with
 * \a magic_number to identify the matrix for later reading.
 *
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 */
Gan_Bool
 GAN_VEC_FWRITE ( FILE *fp, const GAN_VECTYPE *p, gan_uint32 magic_number )
{
   if ( !gan_fwrite_lendian_ui32 ( fp, &magic_number, 1 ) )
   {
      gan_err_register ( "GAN_VEC_FWRITE", GAN_ERROR_CORRUPTED_FILE, "" );
      return GAN_FALSE;
   }

   if ( !GAN_FWRITE_LENDIAN (  fp, (GAN_REALTYPE *)p,
                               sizeof(GAN_VECTYPE)/sizeof(GAN_REALTYPE) ) )
   {
      gan_err_register ( "GAN_VEC_FWRITE", GAN_ERROR_CORRUPTED_FILE, "" );
      return GAN_FALSE;
   }

   return GAN_TRUE;
}

/**
 * \brief Read fixed size vector from file stream.
 *
 * Read fixed size vector \a p from file stream \a fp, prefixed by
 * a number written into the \a magic_number pointer.
 *
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 */
Gan_Bool
 GAN_VEC_FREAD ( FILE *fp, GAN_VECTYPE *p, gan_uint32 *magic_number )
{
   if ( !gan_fread_lendian_ui32 ( fp, magic_number, 1 ) )
   {
      gan_err_register ( "GAN_VEC_FREAD", GAN_ERROR_CORRUPTED_FILE, "" );
      return GAN_FALSE;
   }

   if ( !GAN_FREAD_LENDIAN ( fp, (GAN_REALTYPE *)p,
                             sizeof(GAN_VECTYPE)/sizeof(GAN_REALTYPE) ) )
   {
      gan_err_register ( "GAN_VEC_FREAD", GAN_ERROR_CORRUPTED_FILE, "" );
      return GAN_FALSE;
   }
      
   return GAN_TRUE;
}

/**
 * \}
 */

/**
 * \addtogroup FixedSizeVectorFill
 * \{
 */

/**
 * \brief Set fixed size vector to zero.
 *
 * Return fixed size vector filled with zero values.
 */
GAN_VECTYPE GAN_VEC_ZERO_S (void)
{
   GAN_VECTYPE A;

   (void)GAN_VEC_ZERO_Q ( &A );
   return A;
}

/**
 * \}
 */

/**
 * \addtogroup FixedSizeVectorCopy
 * \{
 */

/**
 * \brief Return a copy of fixed size vector.
 *
 * Copy fixed size vector \a p, returning the result as a new fixed size
 * matrix.
 */
GAN_VECTYPE GAN_VEC_COPY_S ( const GAN_VECTYPE *x )
{
   GAN_VECTYPE y;

   (void)GAN_VEC_COPY_Q ( x, &y );
   return y;
}

/**
 * \}
 */

/**
 * \addtogroup FixedSizeVectorScale
 * \{
 */

/**
 * \brief Scale fixed size vector.
 *
 * Return a copy of fixed size vector \a p multiplied by scalar \a a.
 */
GAN_VECTYPE GAN_VEC_SCALE_S ( const GAN_VECTYPE *x, const GAN_REALTYPE a )
{
   GAN_VECTYPE y;

   (void)GAN_VEC_SCALE_Q ( x, a, &y );
   return y;
}

/**
 * \brief Divide fixed size vector by scalar.
 *
 * Return a copy of fixed size vector \a p divided by scalar \a a.
 */
GAN_VECTYPE GAN_VEC_DIVIDE_S ( const GAN_VECTYPE *x, const GAN_REALTYPE a )
{
   GAN_VECTYPE y;

   if ( GAN_VEC_DIVIDE_Q ( x, a, &y ) == NULL )
      gan_assert ( 0, "division by scalar failed" );

   return y;
}

/**
 * \brief Negate fixed size vector.
 *
 * Return a negated copy of fixed size vector \a p.
 */
GAN_VECTYPE GAN_VEC_NEGATE_S ( const GAN_VECTYPE *x )
{
   GAN_VECTYPE y;

   (void)GAN_VEC_NEGATE_Q ( x, &y );
   return y;
}

/**
 * \brief Scale matrix to unit norm.
 *
 * Scale fixed size vector \a x to unit fixed size vector, writing result into
 * fixed size vector \a y.
 *
 * \return Pointer to the result vector \a y, or \c NULL on failure.
 */
GAN_VECTYPE *GAN_VEC_UNIT_Q ( GAN_VECTYPE *x, GAN_VECTYPE *y )
{
   GAN_REALTYPE norm = (GAN_REALTYPE)sqrt(GAN_VEC_SQRLEN_Q ( x ));

   if ( GAN_VEC_DIVIDE_Q ( x, norm, y ) == NULL )
   {
      gan_err_flush_trace();
      gan_err_register ( "GAN_VEC_UNIT_Q", GAN_ERROR_DIVISION_BY_ZERO, "" );
      return NULL;
   }

   return y;
}

/**
 * \brief Scale matrix to unit norm.
 *
 * Return a copy of fixed size vector \a x multiplied by scalar so that the
 * result is a unit fixed size vector.
 *
 * \result The unit vector \a x.
 */
GAN_VECTYPE GAN_VEC_UNIT_S ( const GAN_VECTYPE *x )
{
   GAN_VECTYPE y;

#ifdef NDEBUG
   GAN_VEC_UNIT_Q ( (GAN_VECTYPE *)x, &y );
#else
   gan_assert ( GAN_VEC_UNIT_Q ( (GAN_VECTYPE*)x, &y ) != NULL,
                "failed vector division by scalar" );
#endif
   return y;
}

/**
 * \}
 */

/**
 * \addtogroup FixedSizeVectorAdd
 * \{
 */

/**
 * \brief Add two fixed size vectors.
 *
 * Add two fixed size vectors \a p, \a q and return the result as a new
 * fixed size vector \f$p + q\f$.
 */
GAN_VECTYPE GAN_VEC_ADD_S ( const GAN_VECTYPE *x, const GAN_VECTYPE *y )
{
   GAN_VECTYPE z;

   (void)GAN_VEC_ADD_Q ( x, y, &z );
   return z;
}

/**
 * \}
 */

/**
 * \addtogroup FixedSizeVectorSubtract
 * \{
 */

/**
 * \brief Subtract two fixed size vectors.
 *
 * Subtract two fixed size vectors \a p, \a q and return the result as a new
 * fixed size vector \f$p - q\f$.
 */
GAN_VECTYPE GAN_VEC_SUB_S ( const GAN_VECTYPE *x, const GAN_VECTYPE *y )
{
   GAN_VECTYPE z;

   (void)GAN_VEC_SUB_Q ( x, y, &z );
   return z;
}

/**
 * \}
 */

/**
 * \addtogroup FixedSizeVectorScalarProduct
 * \{
 */

/**
 * \brief Dot product of fixed size vectors.
 *
 * Returns the scalar product of the two fixed size vectors \a p and \a q.
 * Implemented as a function.
 *
 * \sa GAN_VEC_DOT_Q().
 */
GAN_REALTYPE GAN_VEC_DOT_S ( const GAN_VECTYPE *p, const GAN_VECTYPE *q )
{
   return GAN_VEC_DOT_Q ( p, q );
}

/**
 * \}
 */

/**
 * \addtogroup FixedSizeVectorNorms
 * \{
 */

/**
 * \brief Returns sum of squares of vector elements.
 *
 * Returns the scalar product of fixed size vector \a p with itself, i.e. the
 * squared length of \a p. Implemented as a function.
 *
 * \sa GAN_VEC_SQRLEN_Q().
 */
GAN_REALTYPE GAN_VEC_SQRLEN_S ( const GAN_VECTYPE *p )
{
   return GAN_VEC_SQRLEN_Q ( p );
}

/**
 * \}
 */

/**
 * \addtogroup FixedSizeVectorConvert
 * \{
 */

/**
 * \brief Convert generic rectangular vector to fixed size vector structure.
 *
 * Converts general vector \a x to fixed-size vector, returning the result
 * as a new fixed size vector.
 */
GAN_VECTYPE
 GAN_VEC_FROM_VEC_S ( const GAN_VECTOR_TYPE *x )
{
   GAN_VECTYPE a;

   if ( !GAN_VEC_FROM_VEC_Q ( x, &a ) )
      gan_assert ( 0, "GAN_VEC_FROM_VEC_S" );

   return a;
}

/**
 * \}
 */

#include <gandalf/linalg/vectorf_undef_noc.h>


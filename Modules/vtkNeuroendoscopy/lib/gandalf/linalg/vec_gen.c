/**
 * File:          $RCSfile: vec_gen.c,v $
 * Module:        Generic column vectors (double precision)
 * Part of:       Gandalf Library
 *
 * Revision:      $Revision: 1.1.2.1 $
 * Last edited:   $Date: 2007/10/14 03:16:24 $
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

#include <stdarg.h>
#include <gandalf/linalg/vec_gen.h>
#include <gandalf/linalg/mat_gen.h>
#include <gandalf/linalg/mat_symmetric.h>
#include <gandalf/linalg/clapack.h>
#include <gandalf/linalg/cblas.h>
#include <gandalf/common/allocate.h>
#include <gandalf/common/misc_error.h>
#include <gandalf/common/endian_io.h>

/**
 * \addtogroup LinearAlgebra
 * \{
 */

/**
 * \addtogroup GeneralSizeMatVec
 * \{
 */

/**
 * \defgroup GeneralSizeVector General Size Vectors
 * \{
 */

/* start off some groups */

/**
 * \defgroup GeneralSizeVectorAccessElement Access an Element of a General Size Vector
 * \{
 */

/**
 * \}
 */

/**
 * \defgroup GeneralSizeVectorNorms General Size Vector Norms
 * \{
 */

/**
 * \}
 */

/**
 * \defgroup GeneralSizeVectorTest Test Attributes of a General Size Vector
 * \{
 */

/**
 * \}
 */

/**
 * \defgroup GeneralSizeVectorSize Return Size of General Size Vector
 * \{
 */

/**
 * \}
 */

/**
 * \defgroup GeneralSizeVectorAllocate Allocate/Free a General Size Vector
 * \{
 */

/* function to re-allocate vector data */
Gan_Bool
 gan_vec_realloc ( Gan_Vector *x, unsigned long rows )
{
   /* re-allocate vector data */
   gan_assert ( x->data_alloc,
                "cannot reallocate matrix in gan_vec_realloc()" );
   x->data_size = rows;
   x->data = gan_realloc_array ( double, x->data, x->data_size );

   /* set vector size */
   x->rows = rows;

   /* return with success */
   return GAN_TRUE;
}

/**
 * \brief Free a vector.
 *
 * Free a generic vector.
 */
void
 gan_vec_free ( Gan_Vector *x )
{
   if ( x->data_alloc && x->data != NULL ) free(x->data);
   if ( x->struct_alloc ) free(x);
}

/**
 * \}
 */

/**
 * \defgroup GeneralSizeVectorSet Set Attributes of a General Size Vector
 * \{
 */

/**
 * \brief Set size of vector.
 * \return Pointer to the vector \a x.
 *
 * Set size of vector \a x to \a rows.
 */
Gan_Vector *
 gan_vec_set_size ( Gan_Vector *x, unsigned long rows )
{
   /* check whether vector x is big enough; if it isn't, reallocate it */
   if ( x->data_size < rows )
      /* re-allocate vector data */
      gan_vec_realloc ( x, rows );
   else
      /* set vector size */
      x->rows = rows;

   /* return modified vector */
   return x;
}

/**
 * \}
 */

/**
 * \defgroup GeneralSizeVectorFill Fill a General Size Vector with Values
 * \{
 */

/**
 * \brief Fill vector from variable argument list.
 * \return Pointer to the vector \a x.
 *
 * Fill vector \a x with values from variable argument list, and set the
 * size of \a x to \a rows.
 */
Gan_Vector *
 gan_vec_fill_va ( Gan_Vector *x, unsigned long rows, ... )
{
   unsigned int i;
   va_list ap;

   /* allocate/set vector x as necessary */
   if ( x == NULL )
      x = gan_vec_alloc ( rows );
   else
      x = gan_vec_set_size ( x, rows );

   if ( x == NULL )
   {
      gan_err_register ( "gan_vec_fill_va", GAN_ERROR_FAILURE, "" );
      return NULL;
   }
   
   /* read variable argument list of vector elements */
   va_start ( ap, rows );
   for ( i = 0; i < rows; i++ )
      x->data[i] = va_arg ( ap, double );

   va_end ( ap );
   return x;
}

/**
 * \brief Fill vector from variable argument list.
 * \return Pointer to the vector \a x.
 *
 * Fill vector \a x with values from variable argument list \a aptr, and set
 * the size of \a x to \a rows.
 */
Gan_Vector *
 gan_vec_fill_vap ( Gan_Vector *x, unsigned long rows, va_list *aptr )
{
   unsigned int i;

   /* allocate/set vector x as necessary */
   if ( x == NULL )
      x = gan_vec_alloc ( rows );
   else
      x = gan_vec_set_size ( x, rows );

   if ( x == NULL )
   {
      gan_err_register ( "gan_vec_fill_vap", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* read variable argument list of vector elements */
   for ( i = 0; i < rows; i++ )
      x->data[i] = va_arg ( *aptr, double );

   return x;
}

/**
 * \brief Fill all elements of a vector with the same value.
 * \param x The vector to be filled
 * \param rows The new size of the vector
 * \param value The value to set each element of the vector to
 * \return Pointer to the result vector \a x, or \c NULL on failure..
 *
 * Set the size of vector \a x to \a rows and fill each element with the
 * provided value.
 *
 * \sa gan_vec_fill_const_s()
 */
Gan_Vector *
 gan_vec_fill_const_q ( Gan_Vector *x, unsigned long rows, double value )
{
   int i;

   /* allocate/set vector x as necessary */
   if ( x == NULL )
      x = gan_vec_alloc ( rows );
   else
      x = gan_vec_set_size ( x, rows );

   if ( x == NULL )
   {
      gan_err_register ( "gan_vec_fill_const_q", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* fill vector with constant */
   for ( i = rows-1; i >= 0; i-- )
      x->data[i] = value;

   return x;
}

/**
 * \}
 */

/**
 * \defgroup GeneralSizeVectorRead Read Elements of a General Size Vector
 * \{
 */

/**
 * \brief Read vector from variable argument list.
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Read elements of vector \a x from variable argument list.
 */
Gan_Bool
 gan_vec_read_va ( const Gan_Vector *x, unsigned long rows, ... )
{
   unsigned int i;
   va_list ap;

   /* check that dimensions match */
   gan_assert ( rows == x->rows,
                "dimensions don't match in gan_vec_read_va()" );

   /* fill variable argument list of vector element pointers */
   va_start ( ap, rows );
   for ( i = 0; i < rows; i++ )
      *(va_arg ( ap, double * )) = x->data[i];

   va_end ( ap );

   /* return with success */
   return GAN_TRUE;
}

/**
 * \}
 */

/**
 * \defgroup GeneralSizeVectorCopy Copy a General Size Vector
 * \{
 */

/**
 * \brief Copy vector.
 *
 * Copy vector \a x to vector \a y, and return \a y.
 */
Gan_Vector *
 gan_vec_copy_q ( const Gan_Vector *x, Gan_Vector *y )
{
#ifdef HAVE_LAPACK
   long onei = 1;
#endif
   
   /* allocate vector y if necessary */
   if ( y == NULL )
      y = gan_vec_alloc ( x->rows );
   else
   {
      /* check whether vector y is big enough; if it isn't, reallocate it */
      if ( y->data_size < x->rows )
         /* re-allocate vector data */
         gan_vec_realloc ( y, x->rows );
      else
         /* set vector size */
         y->rows = x->rows;
   }

   /* copy vector */
#ifdef HAVE_LAPACK
   dcopy_ ( (long *)&x->rows, x->data, &onei, y->data, &onei );
#else
   gan_dcopy ( x->rows, x->data, 1, y->data, 1 );
#endif /* #ifdef HAVE_LAPACK */

   return y;
}

/**
 * \}
 */

/**
 * \defgroup GeneralSizeVectorScale Multiply/Divide a General Size Vector by a Scalar
 * \{
 */

/**
 * \brief Multiply vector by scalar.
 * \return Result vector \a y.
 *
 * Multiply vector \a x by scalar \a a and write result \f$ a x \f$
 * into vector \a y.
 */
Gan_Vector *
 gan_vec_scale_q ( Gan_Vector *x, double a, Gan_Vector *y )
{
#ifdef HAVE_LAPACK
   long onei = 1;
#endif

   /* allocate vector y if necessary */
   if ( y == NULL )
      y = gan_vec_alloc ( x->rows );
   else
   {
      /* check whether vector y is big enough; if it isn't, reallocate it */
      if ( y->data_size < x->rows )
         /* re-allocate vector data */
         gan_vec_realloc ( y, x->rows );
      else
         /* set vector size */
         y->rows = x->rows;
   }

#ifdef HAVE_LAPACK
   /* copy and scale vector */
   dcopy_ ( (long *)&x->rows, x->data, &onei, y->data, &onei );
   dscal_ ( (long *)&y->rows, &a, y->data, &onei );
#else
   /* copy and scale vector */
   gan_dcopy ( x->rows, x->data, 1, y->data, 1 );
   gan_dscal ( y->rows, a, y->data, 1 );
#endif /* #ifdef HAVE_LAPACK */

   return y;
}

/**
 * \}
 */

/**
 * \defgroup GeneralSizeVectorAdd Add General Size Vectors
 * \{
 */

/**
 * \brief Add two vectors and write result into a third.
 * \return Result vector \a z.
 *
 * Add two vectors \a x, \a y and write result into \f$ z = x + y \f$.
 */
Gan_Vector *
 gan_vec_add_q ( Gan_Vector *x, Gan_Vector *y, Gan_Vector *z )
{
#ifdef HAVE_LAPACK
   long onei = 1;
   double onef = 1.0;
#endif

   /* check that the dimensions of input vectors x & y are compatible */
   gan_assert ( gan_vec_same_size(x,y),
                "incompatible vectors in vector_add()" );

   /* allocate result vector z if necessary */
   if ( z == NULL )
      z = gan_vec_alloc ( x->rows );
   else
   {
      /* check whether result vector z is big enough */
      if ( z->data_size < x->rows )
         /* re-allocate matrix data */
         gan_vec_realloc ( z, x->rows );
      else
         /* set result vector size */
         z->rows = x->rows;
   }

#ifdef HAVE_LAPACK
   /* add vector data */
   if ( z == x )
      /* in-place operation x += y */
      daxpy_ ( (long *)&x->rows, &onef, y->data, &onei, x->data, &onei );
   else if ( z == y )
      /* in-place operation y += x */
      daxpy_ ( (long *)&x->rows, &onef, x->data, &onei, y->data, &onei );
   else
   {
      /* z = x + y */
      dcopy_ ( (long *)&x->rows, x->data, &onei, z->data, &onei );
      daxpy_ ( (long *)&x->rows, &onef, y->data, &onei, z->data, &onei );
   }
#else
   /* add vector data */
   if ( z == x )
      /* in-place operation x += y */
      gan_daxpy ( x->rows, 1.0, y->data, 1, x->data, 1 );
   else if ( z == y )
      /* in-place operation y += x */
      gan_daxpy ( x->rows, 1.0, x->data, 1, y->data, 1 );
   else
   {
      /* z = x + y */
      gan_dcopy ( x->rows, x->data, 1, z->data, 1 );
      gan_daxpy ( x->rows, 1.0, y->data, 1, z->data, 1 );
   }
#endif /* #ifdef HAVE_LAPACK */

   /* return result vector */
   return z;
}

/**
 * \}
 */

/**
 * \defgroup GeneralSizeVectorSubtract Subtract General Size Vectors
 * \{
 */

/**
 * \brief Subtract two vectors and write result into a third.
 * \return Result vector \a z.
 *
 * Subtract two vectors \a x, \a y and write result into \f$ z = x - y \f$.
 */
Gan_Vector *
 gan_vec_sub_q ( Gan_Vector *x, Gan_Vector *y, Gan_Vector *z )
{
#ifdef HAVE_LAPACK
   long onei = 1;
   double minus_onef = -1.0;
#endif

   /* check that the dimensions of input vectors x & y are compatible */
   gan_assert ( gan_vec_same_size(x,y),
                "incompatible vectors in vector_sub()" );

   /* allocate result vector z if necessary */
   if ( z == NULL )
      z = gan_vec_alloc ( x->rows );
   else
   {
      /* check whether result vector z is big enough */
      if ( z->data_size < x->rows )
         /* re-allocate matrix data */
         gan_vec_realloc ( z, x->rows );
      else
         /* set result vector size */
         z->rows = x->rows;
   }

#ifdef HAVE_LAPACK
   /* subtract vector data */
   if ( z == x )
      /* in-place operation x -= y */
      daxpy_ ( (long *)&x->rows, &minus_onef, y->data, &onei, x->data, &onei );
   else if ( z == y )
   {
      /* in-place operation y = x - y */
      double onef = 1.0;

      dscal_ ( (long *)&x->rows, &minus_onef, y->data, &onei );
      daxpy_ ( (long *)&x->rows, &onef, x->data, &onei, y->data, &onei );
   }
   else
   {
      /* z = x - y */
      dcopy_ ( (long *)&x->rows, x->data, &onei, z->data, &onei );
      daxpy_ ( (long *)&x->rows, &minus_onef, y->data, &onei, z->data, &onei );
   }
#else
   /* subtract vector data */
   if ( z == x )
      /* in-place operation x -= y */
      gan_daxpy ( x->rows, -1.0, y->data, 1, x->data, 1 );
   else if ( z == y )
   {
      /* in-place operation y = x - y */
      gan_dscal ( x->rows, -1.0, y->data, 1 );
      gan_daxpy ( x->rows, 1.0, x->data, 1, y->data, 1 );
   }
   else
   {
      /* z = x - y */
      gan_dcopy ( x->rows, x->data, 1, z->data, 1 );
      gan_daxpy ( x->rows, -1.0, y->data, 1, z->data, 1 );
   }
#endif /* #ifdef HAVE_LAPACK */

   /* return result vector */
   return z;
}

/**
 * \}
 */

/**
 * \defgroup GeneralSizeVectorScalarProduct Scalar Product of General Size Vectors
 * \{
 */

/**
 * \brief Vector dot product.
 *
 * Returns the scalar product \f$ x.y \f$ of the two vectors \a x and \a y.
 */
double
 gan_vec_dot ( const Gan_Vector *x, const Gan_Vector *y )
{
#ifdef HAVE_LAPACK
   long onei = 1;
#endif

   /* make sure the vectors are the same size */
   gan_assert ( gan_vec_same_size(x,y),
                "incompatible vectors in vector_dot()" );

   /* compute dot-product */
#ifdef HAVE_LAPACK
   return ddot_ ( (long *)&x->rows, x->data, &onei, y->data, &onei );
#else
   return gan_ddot ( x->rows, x->data, 1, y->data, 1 );
#endif /* #ifdef HAVE_LAPACK */
}

/**
 * \}
 */

/**
 * \defgroup GeneralSizeVectorOuterProduct Outer Product of General Size Vectors
 * \{
 */

/**
 * \brief Vector outer product.
 * \return Matrix result \a A.
 *
 * Function to compute outer product matrix \f$ A = x y^{\top} \f$ of two
 * vectors \a x and \a y.
 */
Gan_Matrix *
 gan_vec_outer_q ( const Gan_Vector *x, const Gan_Vector *y, Gan_Matrix *A )
{
   int i, j;

   /* allocate result matrix A if necessary */
   if ( A == NULL )
      A = gan_mat_alloc ( x->rows, y->rows );
   else
      A = gan_mat_set_dims ( A, x->rows, y->rows );

   if ( A == NULL )
   {
      gan_err_register ( "gan_vec_outer_q", GAN_ERROR_FAILURE, "" );
      return NULL;
   }
   
   /* compute outer product */
   for ( i = x->rows-1; i >= 0; i-- )
      for ( j = y->rows-1; j >= 0; j-- )
         gan_mat_set_el ( A, i, j, x->data[i]*y->data[j] );

   return A;
}

/**
 * \brief Vector outer product with self.
 * \return Symmetric matrix result \a A.
 *
 * Function to compute outer product \f$ A = x x^{\top} \f$ between a vector
 * \a x and itself.
 */
Gan_SquMatrix *
 gan_vec_outer_sym_q ( const Gan_Vector *x, Gan_SquMatrix *A )
{
   int i, j;

   /* allocate result matrix A if necessary */
   if ( A == NULL )
      A = gan_symmat_alloc ( x->rows );
   else
      A = gan_symmat_set_size ( A, x->rows );

   if ( A == NULL )
   {
      gan_err_register ( "gan_vec_outer_sym_q", GAN_ERROR_FAILURE, "" );
      return NULL;
   }
   
   /* compute outer product */
   for ( i = x->rows-1; i >= 0; i-- )
      for ( j = i; j >= 0; j-- )
         gan_squmat_set_el ( A, i, j, x->data[i]*x->data[j] );

   return A;
}

/**
 * \}
 */

/**
 * \defgroup GeneralSizeVectorInsert Insert Values into a General Size Vector
 * \{
 */

/**
 * \brief Insert part of vector in another vector.
 * \return Result vector \a x.
 *
 * Fills the part of vector \a x starting at row position \a rx and of
 * size \a rows with the section of vector \a y starting at position \a ry.
 */
Gan_Vector *
 gan_vec_insert ( Gan_Vector *x, unsigned long rx,
                  const Gan_Vector *y, unsigned long ry, unsigned long rows )
{
   int i;

   /* make sure part of x/y vectors is valid */
   assert ( rx + rows <= x->rows && ry + rows <= y->rows );

   for ( i = rows-1; i >= 0; i-- )
      x->data[rx+i] = y->data[ry+i];

   return x;
}

/**
 * \brief Insert part of a column of a matrix in a vector.
 * \return Result vector \a x.
 *
 * Insert part of a column of matrix \a A in vector \a x. The column
 * starts at row/column position \a ra/\a ca in \a A, and extends down
 * for the given number of \a rows. It is inserted starting at position
 * \a rx in \a x.
 */
Gan_Vector *
 gan_vec_insert_mat ( Gan_Vector *x, unsigned long rx,
                      const Gan_Matrix *A, unsigned long ra,
                                           unsigned long ca,
                      unsigned long rows )
{
   int i;
   double *Aptr;

   /* make sure part of x and A is valid */
   assert ( rx + rows <= x->rows &&
            ra + rows <= A->rows && ca < A->cols );

   for ( i = rows-1, Aptr = &A->data[ca*A->rows+ra+rows-1]; i >= 0; i-- )
      x->data[rx+i] = *Aptr--;

   return x;
}

/**
 * \brief Insert part of a row of a matrix in a vector.
 * \return Result vector \a x.
 *
 * Insert part of a row of matrix \a A in vector \a x. The row
 * starts at row/column position \a ra/\a ca in \a A, and extends right
 * for the given number of \a rows. It is inserted starting at position
 * \a rx in \a x.
 */
Gan_Vector *
 gan_vec_insert_matT ( Gan_Vector *x, unsigned long rx,
                       const Gan_Matrix *A, unsigned long ra,
                                            unsigned long ca,
                       unsigned long rows )
{
   int i;

   /* make sure part of x and A is valid */
   assert ( rx + rows <= x->rows &&
            ra < A->rows && ca + rows <= A->cols );

   for ( i = rows-1; i >= 0; i-- )
      x->data[rx+i] = A->data[(ca+i)*A->rows+ra];

   return x;
}

/**
 * \}
 */

/**
 * \addtogroup GeneralSizeVectorAllocate
 * \{
 */

/**
 * \brief Free a \c NULL terminated variable argument list of vectors.
 * \return No value.
 *
 * Free a \c NULL terminated variable argument list of vectors, starting
 * with vector \a x.
 */
void
 gan_vec_free_va ( Gan_Vector *x, ... )
{
   va_list ap;

   va_start ( ap, x );
   while ( x != NULL )
   {
      /* free next vector */
      gan_vec_free ( x );

      /* get next vector in list */
      x = va_arg ( ap, Gan_Vector * );
   }

   va_end ( ap );
}


/* not a user function */
Gan_Vector *
 gan_vec_form_gen ( Gan_Vector *x, unsigned long rows,
                    double *data, size_t data_size )
{
   if ( x == NULL )
   {
      /* dyamically allocate vector */
      x = gan_malloc_object(Gan_Vector);

      /* set dynamic allocation flag */
      x->struct_alloc = GAN_TRUE;
   }
   else
      /* indicate vector was not dynamically allocated */
      x->struct_alloc = GAN_FALSE;

   if ( data == NULL )
   {
      if ( data_size == 0 )
         /* set size of vector data to be allocated */
         x->data_size = rows;
      else
      {
         /* a non-zero data size has been requested */
         assert ( data_size >= rows );
         x->data_size = data_size;
      }

      /* allocate vector data */
      if ( x->data_size == 0 ) x->data = NULL;
      else
      {
         x->data = gan_malloc_array ( double, x->data_size );
         if ( x->data == NULL )
         {
            gan_err_flush_trace();
            gan_err_register_with_number ( "gan_vec_form_gen", GAN_ERROR_MALLOC_FAILED, "", x->data_size*sizeof(double) );
            return NULL;
         }
      }

      x->data_alloc = GAN_TRUE;
   }
   else
   {
      /* data for this vector is provided */
      gan_assert ( data_size >= rows,
                   "data array not large enough in gan_vec_form_gen()" );

      /* set data pointer in vector */
      x->data = data;
      x->data_size = data_size;

      /* we shouldn't free the vector data */
      x->data_alloc = GAN_FALSE;
   }

   /* set vector size */
   x->rows = rows;

   return x;
}

/**
 * \}
 */

/**
 * \defgroup GeneralSizeVectorIO General Size Vector I/O
 * \{
 */

/**
 * \brief Print vector to file pointer.
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Print vector \a x to file pointer \a fp, with prefix string \a prefix,
 * indentation \a indent and floating-point format \a fmt.
 */
Gan_Bool
 gan_vec_fprint ( FILE *fp, const Gan_Vector *x, const char *prefix,
                  int indent, const char *fmt )
{
   unsigned long i;
   int p;

   /* print indentation */
   for ( p = indent-1; p >= 0; p-- ) fprintf ( fp, " " );
   fprintf ( fp, "%s (%0ld-vector):", prefix, x->rows );

   for ( i = 0; i < x->rows; i++ )
   {
      fprintf ( fp, " " );
      fprintf ( fp, fmt, x->data[i] );
   }

   fprintf ( fp, "\n" );

   /* return with success */
   return GAN_TRUE;
}

/**
 * \brief Read vector from file.
 *
 * Read vector \a x from file pointer \a fp. The prefix string for
 * the matrix is read from the file into the \a prefix string, up to the
 * maximum length \a prefix_len of the \a prefix string. Any remaining
 * characters after \a prefix has been filled are ignored. Pass \c NULL for
 * \a prefix and zero for \a prefix_len to ignore the prefix string.
 * \a x should either be a pre-allocated vector pointer or \c NULL.
 *
 * \return Pointer to the read vector on success, \c NULL on failure.
 * \sa gan_vec_fscanf_s().
 */
Gan_Vector *
 gan_vec_fscanf_q ( FILE *fp, Gan_Vector *x, char *prefix, int prefix_len )
{
   int ch;
   unsigned long rows, i;

   /* read indentation */
   for(;;)
      if ( (ch = getc(fp)) != ' ' || ch == EOF )
         break;

   /* end of file means corrupted file */
   if ( ch == EOF )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_vec_fscanf_q", GAN_ERROR_CORRUPTED_FILE, "" );
      return NULL;
   }

   /* rewind one character to reread the prefix string */
   fseek ( fp, -1, SEEK_CUR );

   /* read prefix string, terminated by end-of-line */
   for ( prefix_len--; prefix_len > 0; prefix_len-- )
   {
      ch = getc(fp);
      if ( ch == EOF || ch == '(' ) break;
      if ( prefix != NULL ) *prefix++ = (char)ch;
   }

   gan_err_test_ptr ( ch == '(', "gan_vec_fscanf_q",
                      GAN_ERROR_CORRUPTED_FILE, "" );

   /* terminate string */
   if ( prefix != NULL ) prefix[-1] = '\0';

   /* read rest of string if necessary */
   if ( prefix_len == 0 )
   {
      for(;;)
      {
         ch = getc(fp);
         if ( ch == EOF || ch == '(' ) break;
      }

      gan_err_test_ptr ( ch == '(', "gan_vec_fscanf_q",
                         GAN_ERROR_CORRUPTED_FILE, "" );
   }

   /* read vector size */
   if ( fscanf ( fp, "%ld-vector):", &rows ) != 1 )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_vec_fscanf_q", GAN_ERROR_CORRUPTED_FILE, "" );
      return NULL;
   }

   /* create/set vector */
   if ( x == NULL )
      x = gan_vec_alloc ( rows );
   else
      x = gan_vec_set_size ( x, rows );

   if ( x == NULL )
   {
      gan_err_register ( "gan_vec_fscanf_q", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* read vector data */
   for ( i = 0; i < x->rows; i++ )
      if ( fscanf ( fp, "%lf", &x->data[i] ) != 1 )
      {
         gan_err_flush_trace();
         gan_err_register ( "gan_vec_fscanf_q", GAN_ERROR_CORRUPTED_FILE, "" );
         return NULL;
      }

   /* read end-of-line character */
#ifdef NDEBUG
   getc(fp);
#else
   gan_err_test_ptr ( getc(fp) == '\n', "gan_vec_fscanf_q",
                      GAN_ERROR_CORRUPTED_FILE, "" );
#endif

   /* success */
   return x;
}

/**
 * \brief Print vector to binary file pointer.
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Print vector \a x to file pointer \a fp in binary format, with 32-bit
 * magic number printed first.
 */
Gan_Bool
 gan_vec_fwrite ( FILE *fp, const Gan_Vector *x, gan_uint32 magic_number )
{
   if ( !gan_fwrite_lendian_ui32 ( fp, &magic_number, 1 ) ||
        !gan_fwrite_lendian_ul ( fp, &x->rows, 1 ) ||
        !gan_fwrite_lendian_f64 ( fp, x->data, x->rows ) )
   {
      gan_err_register ( "gan_vec_fwrite", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   return GAN_TRUE;
}

/**
 * \brief Read vector from file.
 * \return Pointer to the read vector on success, \c NULL on failure.
 *
 * Read vector \a x from file pointer \a fp in binary format. The magic
 * number is read into the provided pointer.
 * \a x should either be a pre-allocated vector pointer or \c NULL.
 *
 * \sa gan_vecf_fread_s()
 */
Gan_Vector *
 gan_vec_fread_q ( FILE *fp, Gan_Vector *x, gan_uint32 *magic_number )
{
   unsigned long rows;

   /* read magic number */
   if ( !gan_fread_lendian_ui32 ( fp, magic_number, 1 ) )
   {
      gan_err_register ( "gan_vecf_fread_q", GAN_ERROR_CORRUPTED_FILE, "" );
      return NULL;
   }

   /* read size of vector */
   if ( !gan_fread_lendian_ul ( fp, &rows, 1 ) )
   {
      gan_err_register ( "gan_vec_fread_q", GAN_ERROR_CORRUPTED_FILE, "" );
      return NULL;
   }

   /* create/set vector */
   if ( x == NULL )
      x = gan_vec_alloc ( rows );
   else
      x = gan_vec_set_size ( x, rows );

   if ( x == NULL )
   {
      gan_err_register ( "gan_vec_fread_q", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* read vector data */
   if ( !gan_fread_lendian_f64 ( fp, x->data, x->rows ) )
   {
      gan_err_register ( "gan_vec_fread_q", GAN_ERROR_CORRUPTED_FILE, "" );
      return NULL;
   }

   /* success */
   return x;
}

/**
 * \}
 */

#ifndef NDEBUG
/* for use in a debugger: use gan_vec_print() or gan_vec_fprint in
 * your program!
 */
void gan_vec_db ( const Gan_Vector *x )
{
   gan_vec_print ( x, "", 0, "%+g" );
}
#endif

/**
 * \}
 */

/**
 * \}
 */

/**
 * \}
 */

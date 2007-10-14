/**
 * File:          $RCSfile: mat_square.c,v $
 * Module:        Square matrices (double precision)
 * Part of:       Gandalf Library
 *
 * Revision:      $Revision: 1.1.2.1 $
 * Last edited:   $Date: 2007/10/14 03:16:23 $
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
#include <string.h>
#include <gandalf/linalg/mat_square.h>
#include <gandalf/linalg/mat_symmetric.h>
#include <gandalf/linalg/mat_diagonal.h>
#include <gandalf/linalg/mat_scaledI.h>
#include <gandalf/linalg/mat_triangular.h>
#include <gandalf/linalg/mat_gen.h>
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
 * \addtogroup GeneralSizeMatrix
 * \{
 */

/**
 * \addtogroup GeneralSizeMatrixAllocate
 * \{
 */

/**
 * \brief Function to free square matrix.
 * \return No value.
 */
void
 gan_squmat_free ( Gan_SquMatrix *A )
{
   if ( A->data_alloc && A->data != NULL ) free(A->data);
   if ( A->struct_alloc ) free(A);
}

/**
 * \brief Free a NULL terminated variable argument list of square matrices.
 * \return No value.
 *
 * Free a \c NULL terminated variable argument list of square matrices,
 * starting with matrix \a A.
 */
void
 gan_squmat_free_va ( Gan_SquMatrix *A, ... )
{
   va_list ap;

   va_start ( ap, A );
   while ( A != NULL )
   {
      /* free next matrix */
      gan_squmat_free ( A );

      /* get next matrix in list */
      A = va_arg ( ap, Gan_SquMatrix * );
   }

   va_end ( ap );
}

/**
 * \}
 */

/**
 * \addtogroup GeneralSizeMatrixSet
 * \{
 */

/**
 * \brief Set type and size of square matrix.
 * \return Result matrix \a A.
 *
 * Set type and size of square matrix \a A to \a type and \a size.
 */
Gan_SquMatrix *
 gan_squmat_set_type_size ( Gan_SquMatrix *A,
                            Gan_SquMatrixType type, unsigned long size )
{
   Gan_SquMatrix *result;

   switch ( type )
   {
      case GAN_SYMMETRIC_MATRIX:
        result = gan_symmat_set_size ( A, size );
        break;

      case GAN_DIAGONAL_MATRIX:
        result = gan_diagmat_set_size ( A, size );
        break;

      case GAN_SCALED_IDENT_MATRIX:
        result = gan_scalImat_set_size ( A, size );
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_squmat_set_type_size", GAN_ERROR_ILLEGAL_TYPE,
                           "square matrix" );
        return NULL;
   }

   if ( result == NULL )
   {
      gan_err_register ( "gan_squmat_set_type_size", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   return A;
}

/**
 * \}
 */

/**
 * \addtogroup GeneralSizeMatrixFill
 * \{
 */

#if 0
/**
 * \brief Fill square matrix from variable argument list.
 *
 * Fill square matrix \a A with values from variable argument list
 * \a aptr, and set the type and size of \a A to \a type and \a size
 * respectively.
 */
Gan_SquMatrix *
 gan_squmat_fill_va ( Gan_SquMatrix *A,
                      Gan_SquMatrixType type, unsigned long size,
                      ... )
{
   va_list ap;

   va_start ( ap, size );

   /* fill A from variable argument list */
   switch ( type )
   {
      case GAN_SYMMETRIC_MATRIX:
        A = gan_symmat_fill_vap ( A, size, &ap );
        break;

      case GAN_DIAGONAL_MATRIX:
        A = gan_diagmat_fill_vap ( A, size, &ap );
        break;

      case GAN_SCALED_IDENT_MATRIX:
        A = gan_scalImat_fill_vap ( A, size, &ap );
        break;

      case GAN_LOWER_TRI_MATRIX:
        A = gan_ltmat_fill_vap ( A, size, &ap );
        break;

      case GAN_UPPER_TRI_MATRIX:
        A = gan_utmat_fill_vap ( A, size, &ap );
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_squmat_fill_va", GAN_ERROR_ILLEGAL_TYPE,
                           "square matrix" );
        return NULL;
        break;
   }

   va_end ( ap );

   if ( A == NULL )
   {
      gan_err_register ( "gan_squmat_fill_va", GAN_ERROR_FAILURE, "" );
      va_end ( ap );
      return NULL;
   }

   return A;
}
#endif

/**
 * \brief Fill square matrix from variable argument list.
 * \return Result matrix \a A.
 *
 * Fill square matrix \a A with values from variable argument list
 * \a aptr, and set the type and size of \a A to \a type and \a size
 * respectively.
 */
Gan_SquMatrix *
 gan_squmat_fill_vap ( Gan_SquMatrix *A,
                       Gan_SquMatrixType type, unsigned long size,
                       va_list *aptr )
{
   /* fill A from variable argument list */
   switch ( type )
   {
      case GAN_SYMMETRIC_MATRIX:
        A = gan_symmat_fill_vap ( A, size, aptr );
        break;

      case GAN_DIAGONAL_MATRIX:
        A = gan_diagmat_fill_vap ( A, size, aptr );
        break;

      case GAN_SCALED_IDENT_MATRIX:
        A = gan_scalImat_fill_vap ( A, size, aptr );
        break;

      case GAN_LOWER_TRI_MATRIX:
        A = gan_ltmat_fill_vap ( A, size, aptr );
        break;

      case GAN_UPPER_TRI_MATRIX:
        A = gan_utmat_fill_vap ( A, size, aptr );
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_squmat_fill_vap", GAN_ERROR_ILLEGAL_TYPE,
                           "square matrix" );
        return NULL;
   }

   if ( A == NULL )
   {
      gan_err_register ( "gan_squmat_fill_vap", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   return A;
}

/**
 * \brief Fill square matrix with constant value.
 * \return Result matrix \a A.
 *
 * Fill matrix \a A with constant \a value, and set the type and size
 * of \a A to \a type and \a size respectively.
 */
Gan_SquMatrix *
 gan_squmat_fill_const_q ( Gan_SquMatrix *A,
                           Gan_SquMatrixType type, unsigned long size,
                           double value )
{
   /* fill with constant value */
   switch ( type )
   {
      case GAN_SYMMETRIC_MATRIX:
        A = gan_symmat_fill_const_q ( A, size, value );
        break;

      case GAN_DIAGONAL_MATRIX:
        A = gan_diagmat_fill_const_q ( A, size, value );
        break;

      case GAN_SCALED_IDENT_MATRIX:
        A = gan_scalImat_fill_const_q ( A, size, value );
        break;

      case GAN_LOWER_TRI_MATRIX:
        A = gan_ltmat_fill_const_q ( A, size, value );
        break;

      case GAN_UPPER_TRI_MATRIX:
        A = gan_utmat_fill_const_q ( A, size, value );
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_squmat_fill_const", GAN_ERROR_ILLEGAL_TYPE,
                           "square matrix" );
        return NULL;
   }

   if ( A == NULL )
   {
      gan_err_register ( "gan_squmat_fill_const", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   return A;
}

/**
 * \}
 */

/**
 * \addtogroup GeneralSizeMatrixSize
 * \{
 */

/**
 * \brief Returns size of data array needed to create a matrix of given type & size.
 *
 * Returns size of data array needed to create a square matrix of given
 * \a type & \a size.
 */
size_t
 gan_squmat_data_size ( Gan_SquMatrixType type, unsigned long size )
{
   switch ( type )
   {
      case GAN_SYMMETRIC_MATRIX:
      case GAN_LOWER_TRI_MATRIX:
      case GAN_UPPER_TRI_MATRIX:
        return (size*(size+1)/2);

      case GAN_DIAGONAL_MATRIX:
        return size;

      case GAN_SCALED_IDENT_MATRIX:
        return 1;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_squmat_data_size", GAN_ERROR_ILLEGAL_TYPE,
                           "matrix" );
        return -1;
   }

   /* this doesn't happen */
   return 0;
}

/**
 * \}
 */

/**
 * \addtogroup GeneralSizeMatrixIO
 * \{
 */

#define TYPE_STRING_LENGTH 50

/**
 * \brief Read square matrix from ASCII file.
 * \return Result matrix \a A.
 * Read square matrix \a A from file pointer \a fp. The prefix string for
 * the matrix is read from the file into the \a prefix string, up to the
 * maximum length \a prefix_len of the \a prefix string. Any remaining
 * characters after \a prefix has been filled are ignored. Pass \c NULL for
 * \a prefix and zero for \a prefix_len to ignore the prefix string.
 * \a A should either be a pointer to a pre-allocated square matrix or \c NULL.
 *
 * \return Pointer to the read matrix on success, \c NULL on failure.
 *
 * \sa gan_squmatf_fscanf_s().
 */
Gan_SquMatrix *
 gan_squmat_fscanf_q ( FILE *fp, Gan_SquMatrix *A,
                       char *prefix, int prefix_len )
{
   int ch;
   unsigned long rows, cols, i, j;
   char type_string[TYPE_STRING_LENGTH];
   Gan_SquMatrixType type;

   /* read indentation */
   for(;;)
      if ( (ch = getc(fp)) != ' ' || ch == EOF )
         break;

   /* end of file means corrupted file */
   if ( ch == EOF )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_squmat_fscanf_q", GAN_ERROR_CORRUPTED_FILE, "" );
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

   gan_err_test_ptr ( ch == '(', "gan_squmat_fscanf_q",
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

      gan_err_test_ptr ( ch == '(', "gan_squmat_fscanf_q",
                         GAN_ERROR_CORRUPTED_FILE, "" );
   }

   /* read dimensions */
   if ( fscanf ( fp, "%ldx%ld ", &rows, &cols ) != 2 || rows != cols )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_squmat_fscanf_q", GAN_ERROR_CORRUPTED_FILE, "" );
      return NULL;
   }

   /* read matrix type string */
   for ( i = 0; i < TYPE_STRING_LENGTH; i++ )
   {
      type_string[i] = getc(fp);
      if ( type_string[i] == '\n' )
      {
         type_string[i] = '\0';
         break;
      }
   }

   gan_err_test_ptr ( i < TYPE_STRING_LENGTH, "gan_squmat_fscanf_q",
                      GAN_ERROR_CORRUPTED_FILE, "" );

   /* interpret type string */
   if ( strcmp ( type_string, "symmetric matrix)" ) == 0 )
      type = GAN_SYMMETRIC_MATRIX;
   else if ( strcmp ( type_string, "diagonal matrix)" ) == 0 )
      type = GAN_DIAGONAL_MATRIX;
   else if ( strcmp ( type_string, "scaled identity matrix)" ) == 0 )
      type = GAN_SCALED_IDENT_MATRIX;
   else if ( strcmp ( type_string, "lower triangular matrix)" ) == 0 )
      type = GAN_LOWER_TRI_MATRIX;
   else if ( strcmp ( type_string, "upper triangular matrix)" ) == 0 )
      type = GAN_UPPER_TRI_MATRIX;
   else
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_squmat_fscanf_q", GAN_ERROR_CORRUPTED_FILE, "" );
      return NULL;
   }

   /* create/set matrix */
   if ( A == NULL )
      A = gan_squmat_alloc ( type, rows );
   else
      A = gan_squmat_set_type_size ( A, type, rows );

   if ( A == NULL )
   {
      gan_err_register ( "gan_squmat_fscanf_q", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* read matrix data */
   switch ( type )
   {
      double tmp;

      case GAN_SYMMETRIC_MATRIX:
        for ( i = 0; i < A->size; i++ )
        {
           for ( j = 0; j < i; j++ )
              if ( fscanf ( fp, "%lf", &tmp ) != 1 ||
                   tmp != A->data[i*(i+1)/2+j] )
              {
                 gan_err_flush_trace();
                 gan_err_register ( "gan_squmat_fscanf_q",
                                    GAN_ERROR_CORRUPTED_FILE, "" );
                 return NULL;
              }

           for ( ; j < A->size; j++ )
              if ( fscanf ( fp, "%lf", &A->data[j*(j+1)/2+i] ) != 1 )
              {
                 gan_err_flush_trace();
                 gan_err_register ( "gan_squmat_fscanf_q",
                                    GAN_ERROR_CORRUPTED_FILE, "" );
                 return NULL;
              }

           /* read end-of-line character */
#ifdef NDEBUG
           getc(fp);
#else
           gan_err_test_ptr ( getc(fp) == '\n', "gan_squmat_fscanf_q",
                              GAN_ERROR_CORRUPTED_FILE, "" );
#endif
        }
        break;

      case GAN_DIAGONAL_MATRIX:
        for ( i = 0; i < A->size; i++ )
        {
           for ( j = 0; j < i; j++ )
              if ( fscanf ( fp, "%lf", &tmp ) != 1 || tmp != 0.0 )
              {
                 gan_err_flush_trace();
                 gan_err_register ( "gan_squmat_fscanf_q",
                                    GAN_ERROR_CORRUPTED_FILE, "" );
                 return NULL;
              }

           if ( fscanf ( fp, "%lf", &A->data[i] ) != 1 )
           {
              gan_err_flush_trace();
              gan_err_register ( "gan_squmat_fscanf_q",
                                 GAN_ERROR_CORRUPTED_FILE, "" );
              return NULL;
           }

           for ( j = i+1; j < A->size; j++ )
              if ( fscanf ( fp, "%lf", &tmp ) != 1 || tmp != 0.0 )
              {
                 gan_err_flush_trace();
                 gan_err_register ( "gan_squmat_fscanf_q",
                                    GAN_ERROR_CORRUPTED_FILE, "" );
                 return NULL;
              }

           /* read end-of-line character */
#ifdef NDEBUG
           getc(fp);
#else
           gan_err_test_ptr ( getc(fp) == '\n', "gan_squmat_fscanf_q",
                              GAN_ERROR_CORRUPTED_FILE, "" );
#endif
        }
        break;

      case GAN_SCALED_IDENT_MATRIX:
        for ( i = 0; i < A->size; i++ )
        {
           for ( j = 0; j < i; j++ )
              if ( fscanf ( fp, "%lf", &tmp ) != 1 || tmp != 0.0 )
              {
                 gan_err_flush_trace();
                 gan_err_register ( "gan_squmat_fscanf_q",
                                    GAN_ERROR_CORRUPTED_FILE, "" );
                 return NULL;
              }

           if ( (i == 0 && fscanf ( fp, "%lf", &A->data[0] ) != 1) ||
                (i > 0 && (fscanf ( fp, "%lf", &tmp ) != 1 ||
                           tmp != A->data[0])) )
           {
              gan_err_flush_trace();
              gan_err_register ( "gan_squmat_fscanf_q",
                                 GAN_ERROR_CORRUPTED_FILE, "" );
              return NULL;
           }

           for ( j = i+1; j < A->size; j++ )
              if ( fscanf ( fp, "%lf", &tmp ) != 1 || tmp != 0.0 )
              {
                 gan_err_flush_trace();
                 gan_err_register ( "gan_squmat_fscanf_q",
                                    GAN_ERROR_CORRUPTED_FILE, "" );
                 return NULL;
              }

           /* read end-of-line character */
#ifdef NDEBUG
           getc(fp);
#else
           gan_err_test_ptr ( getc(fp) == '\n', "gan_squmat_fscanf_q",
                              GAN_ERROR_CORRUPTED_FILE, "" );
#endif
        }
        break;

      case GAN_LOWER_TRI_MATRIX:
        for ( i = 0; i < A->size; i++ )
        {
           for ( j = 0; j <= i; j++ )
              if ( fscanf ( fp, "%lf", &A->data[i*(i+1)/2+j] ) != 1 )
              {
                 gan_err_flush_trace();
                 gan_err_register ( "gan_squmat_fscanf_q",
                                    GAN_ERROR_CORRUPTED_FILE, "" );
                 return NULL;
              }

           for ( ; j < A->size; j++ )
              if ( fscanf ( fp, "%lf", &tmp ) != 1 || tmp != 0.0 )
              {
                 gan_err_flush_trace();
                 gan_err_register ( "gan_squmat_fscanf_q",
                                    GAN_ERROR_CORRUPTED_FILE, "" );
                 return NULL;
              }

           /* read end-of-line character */
#ifdef NDEBUG
           getc(fp);
#else
           gan_err_test_ptr ( getc(fp) == '\n', "gan_squmat_fscanf_q",
                              GAN_ERROR_CORRUPTED_FILE, "" );
#endif
        }
        break;

      case GAN_UPPER_TRI_MATRIX:
        for ( i = 0; i < A->size; i++ )
        {
           for ( j = 0; j <= i; j++ )
              if ( fscanf ( fp, "%lf", &tmp ) != 1 || tmp != 0.0 )
              {
                 gan_err_flush_trace();
                 gan_err_register ( "gan_squmat_fscanf_q",
                                    GAN_ERROR_CORRUPTED_FILE, "" );
                 return NULL;
              }

           for ( ; j < A->size; j++ )
              if ( fscanf ( fp, "%lf", &A->data[j*(j+1)/2+i] ) != 1 )
              {
                 gan_err_flush_trace();
                 gan_err_register ( "gan_squmat_fscanf_q",
                                    GAN_ERROR_CORRUPTED_FILE, "" );
                 return NULL;
              }

           /* read end-of-line character */
#ifdef NDEBUG
           getc(fp);
#else
           gan_err_test_ptr ( getc(fp) == '\n', "gan_squmat_fscanf_q",
                              GAN_ERROR_CORRUPTED_FILE, "" );
#endif
        }
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_squmat_fscanf_q", GAN_ERROR_ILLEGAL_TYPE, "" );
        break;
   }

   /* success */
   return A;
}

/**
 * \brief Write square matrix to binary file.
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Write square matrix \a A in binary format to file pointer \a fp.
 * The provided magic number is written as a prefix.
 *
 * \sa gan_squmatf_fread_q().
 */
Gan_Bool
 gan_squmat_fwrite ( FILE *fp, const Gan_SquMatrix *A, gan_uint32 magic_number )
{
   char type_char;

   /* set type character */
   switch ( A->type )
   {
      case GAN_SYMMETRIC_MATRIX:    type_char = 'S'; break;
      case GAN_DIAGONAL_MATRIX:     type_char = 'D'; break;
      case GAN_SCALED_IDENT_MATRIX: type_char = 'I'; break;
      case GAN_LOWER_TRI_MATRIX:    type_char = 'L'; break;
      case GAN_UPPER_TRI_MATRIX:    type_char = 'U'; break;
      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_squmat_fwrite", GAN_ERROR_ILLEGAL_TYPE,
                           "" );
        return GAN_FALSE;
   }

   if ( !gan_fwrite_lendian_ui32 ( fp, &magic_number, 1 ) )
   {
      gan_err_register ( "gan_squmat_fwrite", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   if ( fwrite ( &type_char, sizeof(type_char), 1, fp ) != 1 )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_squmat_fwrite", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   if ( !gan_fwrite_lendian_ul ( fp, &A->size, 1 ) ||
        !gan_fwrite_lendian_f64 ( fp, A->data,
                                  gan_squmat_data_size ( A->type, A->size ) ) )
   {
      gan_err_register ( "gan_squmat_fwrite", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   return GAN_TRUE;
}

/**
 * \brief Read square matrix from binary file.
 * \return Read matrix on success, \c NULL on failure.
 *
 * Read square matrix from file pointer \a fp in binary format into provided
 * matrix \a A. The magic number for the matrix is read from the file into the
 * provided pointer.
 *
 * \sa gan_squmatf_fread_s().
 */
Gan_SquMatrix *
 gan_squmat_fread_q ( FILE *fp, Gan_SquMatrix *A, gan_uint32 *magic_number )
{
   unsigned long size;
   char type_char;
   Gan_SquMatrixType type;

   /* read magic number */
   if ( !gan_fread_lendian_ui32 ( fp, magic_number, 1 ) )
   {
      gan_err_register ( "gan_squmat_fread_q", GAN_ERROR_CORRUPTED_FILE, "" );
      return NULL;
   }

   /* read matrix type character */
   if ( fread ( &type_char, sizeof(type_char), 1, fp ) != 1 )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_squmat_fread_q", GAN_ERROR_CORRUPTED_FILE, "" );
      return NULL;
   }

   /* read dimensions */
   if ( !gan_fread_lendian_ul ( fp, &size, 1 ) )
   {
      gan_err_register ( "gan_squmat_fread_q", GAN_ERROR_CORRUPTED_FILE, "" );
      return NULL;
   }

   /* interpret type character */
   switch ( type_char )
   {
      case 'S': type = GAN_SYMMETRIC_MATRIX;    break;
      case 'D': type = GAN_DIAGONAL_MATRIX;     break;
      case 'I': type = GAN_SCALED_IDENT_MATRIX; break;
      case 'L': type = GAN_LOWER_TRI_MATRIX;    break;
      case 'U': type = GAN_UPPER_TRI_MATRIX;    break;
      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_squmat_fread_q", GAN_ERROR_CORRUPTED_FILE,
                           "" );
        return NULL;
   }

   /* create/set matrix */
   if ( A == NULL )
      A = gan_squmat_alloc ( type, size );
   else
      A = gan_squmat_set_type_size ( A, type, size );

   if ( A == NULL )
   {
      gan_err_register ( "gan_squmat_fread_q", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* read matrix data */
   if ( !gan_fread_lendian_f64 ( fp, A->data,
                                 gan_squmat_data_size ( type, size ) ) )
   {
      gan_err_register ( "gan_squmat_fread_q", GAN_ERROR_CORRUPTED_FILE, "" );
      return NULL;
   }

   /* success */
   return A;
}

/**
 * \}
 */

/**
 * \addtogroup GeneralSizeMatrixAllocate
 * \{
 */

/* not a user function */
Gan_SquMatrix *
 gan_squmat_form_gen ( Gan_SquMatrix *A,
                       Gan_SquMatrixType type, unsigned long size,
                       double *data, size_t data_size )
{
   switch ( type )
   {
      case GAN_SYMMETRIC_MATRIX:
      A = gan_symmat_form_data ( A, size, data, data_size );
      break;

      case GAN_DIAGONAL_MATRIX:
      A = gan_diagmat_form_data ( A, size, data, data_size );
      break;

      case GAN_SCALED_IDENT_MATRIX:
      A = gan_scalImat_form_data ( A, size, data, data_size );
      break;

      case GAN_LOWER_TRI_MATRIX:
      A = gan_ltmat_form_data ( A, size, data, data_size );
      break;

      case GAN_UPPER_TRI_MATRIX:
      A = gan_ltmat_form_data ( A, size, data, data_size );
      break;

      default:
      gan_err_flush_trace();
      gan_err_register ( "gan_squmat_form_gen", GAN_ERROR_ILLEGAL_TYPE,
                         "matrix" );
      break;
   }

   /* success */
   return A;
}

/**
 * \}
 */

#ifndef NDEBUG
/* for use in a debugger: use gan_squmat_print() or gan_squmat_fprint in
 * your program!
 */
void gan_squmat_db ( const Gan_SquMatrix *A )
{
   gan_squmat_print ( A, "", 0, "%+g" );
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

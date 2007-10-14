/**
 * File:          $RCSfile: 2x4matrix.c,v $
 * Module:        2x4 matrices (double precision)
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

#include <math.h>
#include <gandalf/linalg/2x4matrix.h>

/**
 * \addtogroup LinearAlgebra
 * \{
 */

/**
 * \addtogroup FixedSizeMatVec
 * \{
 */

/**
 * \addtogroup FixedSizeMatrix
 * \{
 */

/**
 * \addtogroup FixedSizeMatrixIO
 * \{
 */

/**
 * \brief Print 2x4 matrix to file
 *
 * Print 2x4 matrix \a A to file pointer \a fp, with prefix string \a prefix,
 * indentation \a indent and floating-point format \a fmt.
 *
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 */
Gan_Bool
 gan_mat24_fprint ( FILE *fp, const Gan_Matrix24 *A,
                    const char *prefix, int indent, const char *fmt )
{
   int i;

   /* print indentation on first line */
   for ( i = indent-1; i >= 0; i-- ) fprintf ( fp, " " );
   fprintf ( fp, "%s\n", prefix );
   for ( i = indent-1; i >= 0; i-- ) fprintf ( fp, " " );
   fprintf ( fp, " " ); fprintf ( fp, fmt, A->xx );
   fprintf ( fp, " " ); fprintf ( fp, fmt, A->xy );
   fprintf ( fp, " " ); fprintf ( fp, fmt, A->xz );
   fprintf ( fp, " " ); fprintf ( fp, fmt, A->xw );
   fprintf ( fp, "\n" );
   for ( i = indent-1; i >= 0; i-- ) fprintf ( fp, " " );
   fprintf ( fp, " " ); fprintf ( fp, fmt, A->yx );
   fprintf ( fp, " " ); fprintf ( fp, fmt, A->yy );
   fprintf ( fp, " " ); fprintf ( fp, fmt, A->yz );
   fprintf ( fp, " " ); fprintf ( fp, fmt, A->yw );
   fprintf ( fp, "\n" );

   /* success */
   return GAN_TRUE;
}

/**
 * \brief Read 2x4 matrix from file.
 *
 * Read 2x4 matrix \a A from file pointer \a fp. The prefix
 * string for the matrix is read from the file into the \a prefix string, up
 * to the maximum length \a prefix_len of the \a prefix string. Any remaining
 * characters after \a prefix has been filled are ignored. Pass \c NULL for
 * \a prefix and zero for \a prefix_len to ignore the prefix string.
 *
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 */
Gan_Bool
 gan_mat24_fscanf ( FILE *fp, Gan_Matrix24 *A, char *prefix, int prefix_len )
{
   int ch, result;

   /* read indentation */
   for(;;)
      if ( (ch = getc(fp)) != ' ' || ch == EOF )
         break;

   /* end of file means corrupted file */
   if ( ch == EOF )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_mat24_fscanf", GAN_ERROR_CORRUPTED_FILE, "" );
      return GAN_FALSE;
   }
   
   /* rewind one character to reread the prefix string */
   fseek ( fp, -1, SEEK_CUR );

   /* read prefix string, terminated by end-of-line */
   for ( prefix_len--; prefix_len > 0; prefix_len-- )
   {
      ch = getc(fp);
      if ( ch == EOF || ch == '\n' ) break;
      if ( prefix != NULL ) *prefix++ = (char)ch;
   }

   gan_err_test_bool ( ch == '\n', "gan_mat24_fscanf",
                       GAN_ERROR_CORRUPTED_FILE, "" );

   /* terminate string */
   if ( prefix != NULL ) *prefix = '\0';

   /* read rest of string if necessary */
   if ( prefix_len == 0 )
   {
      for(;;)
      {
         ch = getc(fp);
         if ( ch == EOF || ch == '\n' ) break;
      }

      gan_err_test_bool ( ch == '\n', "gan_mat24_fscanf",
                          GAN_ERROR_CORRUPTED_FILE, "" );
   }

   /* read rows of matrix */
   result = 0;
   result += fscanf ( fp, "\n%lf%lf%lf%lf\n", &A->xx, &A->xy, &A->xz, &A->xw );
   result += fscanf ( fp, "%lf%lf%lf%lf",     &A->yx, &A->yy, &A->yz, &A->yw );
   gan_err_test_bool ( result == 8, "gan_mat24_fscanf",
                       GAN_ERROR_CORRUPTED_FILE, "" );

   /* read end-of-line character */
   result = getc(fp);
   gan_err_test_bool ( result == '\n', "gan_mat24_fscanf",
                       GAN_ERROR_CORRUPTED_FILE, "" );

   /* success */
   return GAN_TRUE;
}

/**
 * \}
 */

/**
 * \addtogroup FixedSizeMatrixFill
 * \{
 */

/**
 * \brief Fill 2x4 matrix with values.
 *
 * Fill 2x4 matrix with values:
 * \f[
 *   \left(\begin{array}{cccc} XX & XY & XZ & XW \\ YX & YY & YZ & YW
 *     \end{array}\right)
 * \f]
 *
 * \return Filled matrix.
 */
Gan_Matrix24
 gan_mat24_fill_s ( double XX, double XY, double XZ, double XW,
                    double YX, double YY, double YZ, double YW )
{
   Gan_Matrix24 A;

   (void)gan_mat24_fill_q ( &A, XX, XY, XZ, XW, YX, YY, YZ, YW );
   return A;
}

/**
 * \}
 */

/**
 * \addtogroup FixedSizeMatrixVectorProduct
 * \{
 */

/**
 * \brief Multiply 2x4 matrix by homogeneous 3-vector.
 *
 * Multiply 2x4 matrix \a A by 3-vector \a p expanded to a homogeneous
 * 4-vector by adding fourth element \a h, in other words compute and
 * return
 * \f[
 *   A \left(\begin{array}{c} p \\ h \end{array}\right)
 * \f]
 */
Gan_Vector2
 gan_mat24_multv3h_s ( const Gan_Matrix24 *A, const Gan_Vector3 *p, double h )
{
   Gan_Vector2 q;

   (void)gan_mat24_multv3h_q ( A, p, h, &q );
   return q;
}

/**
 * \}
 */

/**
 * \addtogroup FixedSizeMatrixExtract
 * \{
 */

/**
 * \brief Extract rows of 2x4 matrix.
 *
 * Extract rows of 2x4 matrix \a A into 4-vectors \a p and \a q.
 * The arrangement can be written
 * \f[
 *     A = \left(\begin{array}{c} p^{\top} \\ q^{\top} \end{array}\right)
 * \f]
 *
 * \return No value.
 */
void gan_mat24_get_rows_s ( const Gan_Matrix24 *A, Gan_Vector4 *p, Gan_Vector4 *q )
{
   gan_mat24_get_rows_q ( A, p, q );
}

/**
 * \brief Extract columns of 2x4 matrix
 *
 * Extract columns of 2x4 matrix \a A into 2-vectors \a p, \a q, \a r and \a s.
 * The arrangement can be written
 * \f[
 *     A = \left(\begin{array}{cccc} p & q & r & s \end{array}\right)
 * \f]
 *
 * \return No value.
 */
void gan_mat24_get_cols_s ( const Gan_Matrix24 *A, Gan_Vector2 *p, Gan_Vector2 *q,
                                                   Gan_Vector2 *r, Gan_Vector2 *s )
{
   gan_mat24_get_cols_q ( A, p, q, r, s );
}

/**
 * \}
 */

#include <gandalf/common/misc_error.h>

/**
 * \addtogroup FixedSizeMatrixConvert
 * \{
 */

/**
 * \brief Convert generic rectangular matrix to 2x4 matrix structure.
 *
 * Convert generic rectangular matrix \a A to 2x4 matrix structure \a B.
 *
 * \return Pointer to result matrix \a B.
 */
Gan_Matrix24 *
 gan_mat24_from_mat_q ( const Gan_Matrix *A, Gan_Matrix24 *B )
{
   if ( !gan_mat_read_va ( A, 2, 4, &B->xx, &B->xy, &B->xz, &B->xw,
                                    &B->yx, &B->yy, &B->yz, &B->yw ) )
   {
      gan_err_register ( "gan_mat24_from_mat_q", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   return B;
}

/**
 * \}
 */

/* define all the standard small matrix functions for 2x4 matrices */
#include <gandalf/linalg/2x4mat_noc.h>
#include <gandalf/linalg/matrixf_noc.c>

/**
 * \}
 */

/**
 * \}
 */

/**
 * \}
 */

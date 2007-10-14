/**
 * File:          $RCSfile: 2x3matrix.c,v $
 * Module:        2x3 matrices (double precision)
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
#include <gandalf/linalg/2x3matrix.h>

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
 * \brief Print 2x3 matrix to file
 *
 * Print 2x3 matrix \a A to file pointer \a fp, with prefix string \a prefix,
 * indentation \a indent and floating-point format \a fmt.
 *
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 */
Gan_Bool
 gan_mat23_fprint ( FILE *fp, const Gan_Matrix23 *A,
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
   fprintf ( fp, "\n" );
   for ( i = indent-1; i >= 0; i-- ) fprintf ( fp, " " );
   fprintf ( fp, " " ); fprintf ( fp, fmt, A->yx );
   fprintf ( fp, " " ); fprintf ( fp, fmt, A->yy );
   fprintf ( fp, " " ); fprintf ( fp, fmt, A->yz );
   fprintf ( fp, "\n" );

   /* success */
   return GAN_TRUE;
}

/**
 * \brief Read 2x3 matrix from file.
 *
 * Read 2x3 matrix \a A from file pointer \a fp. The prefix
 * string for the matrix is read from the file into the \a prefix string, up
 * to the maximum length \a prefix_len of the \a prefix string. Any remaining
 * characters after \a prefix has been filled are ignored. Pass \c NULL for
 * \a prefix and zero for \a prefix_len to ignore the prefix string.
 *
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 */
Gan_Bool
 gan_mat23_fscanf ( FILE *fp, Gan_Matrix23 *A, char *prefix, int prefix_len )
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
      gan_err_register ( "gan_mat23_fscanf", GAN_ERROR_CORRUPTED_FILE, "" );
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

   gan_err_test_bool ( ch == '\n', "gan_mat23_fscanf",
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

      gan_err_test_bool ( ch == '\n', "gan_mat23_fscanf",
                          GAN_ERROR_CORRUPTED_FILE, "" );
   }

   /* read rows of matrix */
   result = 0;
   result += fscanf ( fp, "\n%lf%lf%lf\n", &A->xx, &A->xy, &A->xz );
   result += fscanf ( fp, "%lf%lf%lf",     &A->yx, &A->yy, &A->yz );
   gan_err_test_bool ( result == 6, "gan_mat23_fscanf",
                       GAN_ERROR_CORRUPTED_FILE, "" );

   /* read end-of-line character */
   result = getc(fp);
   gan_err_test_bool ( result == '\n', "gan_mat23_fscanf",
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
 * \brief Fill 2x3 matrix with values.
 *
 * Fill 2x3 matrix with values:
 * \f[
 *   \left(\begin{array}{ccc} XX & XY & XZ \\ YX & YY & YZ \end{array}\right)
 * \f]
 *
 * \return Filled matrix.
 */
Gan_Matrix23
 gan_mat23_fill_s ( double XX, double XY, double XZ,
                    double YX, double YY, double YZ )
{
   Gan_Matrix23 A;

   (void)gan_mat23_fill_q ( &A, XX, XY, XZ, YX, YY, YZ );
   return A;
}

/**
 * \}
 */

/**
 * \addtogroup FixedSizeMatrixMatrixProduct
 * \{
 */

/**
 * \brief Multiply 2x3 matrix by the transpose of 2x3 matrix.
 *
 * Multiply 2x3 matrix \a A by the transpose of 2x3 matrix \a B, producing
 * a 2x2 matrix as the result \f$ A B^{\top} \f$.
 *
 * \return The result \f$ A B^{\top} \f$ as a new matrix.
 */
Gan_Matrix22
 gan_mat23_rmultm23T_s ( const Gan_Matrix23 *A, const Gan_Matrix23 *B )
{
   Gan_Matrix22 C;

   (void)gan_mat23_rmultm23T_q ( A, B, &C );
   return C;
}

/**
 * \}
 */

/**
 * \addtogroup FixedSizeMatrixVectorProduct
 * \{
 */

/**
 * \brief Multiply 2x3 matrix by homogeneous 2-vector.
 *
 * Multiply 2x3 matrix \a A by 2-vector \a p expanded to a homogeneous
 * 3-vector by adding third element \a h, in other words compute and
 * return
 * \f[
 *   A \left(\begin{array}{c} p \\ h \end{array}\right)
 * \f]
 */
Gan_Vector2
 gan_mat23_multv2h_s ( const Gan_Matrix23 *A, const Gan_Vector2 *p, double h )
{
   Gan_Vector2 q;

   (void)gan_mat23_multv2h_q ( A, p, h, &q );
   return q;
}

/**
 * \}
 */

/**
 * \addtogroup FixedSizeMatrixInvert
 * \{
 */

/**
 * \brief Compute inverse of completed 3x3 matrix.
 *
 * Given 2x3 matrix \a A and scalar value \a a, compute 2x3 matrix B and
 * scalar \a b such that
 * \f[
 *   \left(\begin{array}{cc} \multicolumn{2}{c}{B} \\ 0^{\top} & b
 *   \end{array}\right)
 * = \left(\begin{array}{cc} \multicolumn{2}{c}{A} \\ 0^{\top} & a
 *   \end{array}\right)^{-1}
 * \f]
 *
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 */
Gan_Bool
 gan_mat23h_invert_q ( const Gan_Matrix23 *A, double a,
                       Gan_Matrix23       *B, double *b )
{
   Gan_Matrix22 H;
   Gan_Vector2 h;

   (void)gan_mat23_get_m22l_q ( A, &H );
   gan_assert ( a != 0.0 && gan_mat22_det_q(&H) != 0.0,
                "illegal arguments to gan_mat23h_invert_q" );
   (void)gan_mat23_get_v2r_q ( A, &h );
   H = gan_mat22_invert_s ( &H );
   h = gan_mat22_multv2_s ( &H, &h );
   (void)gan_vec2_divide_i ( &h, -a );
   (void)gan_mat23_set_parts_q ( B, &H, &h );
   if ( b != NULL ) *b = 1.0/a;
   return GAN_TRUE;
}

/**
 * \brief Compute inverse of completed 3x3 matrix.
 *
 * Given 2x3 matrix \a A and scalar value \a a, compute 2x3 matrix B and
 * scalar \a b such that
 * \f[
 *   \left(\begin{array}{cc} \multicolumn{2}{c}{B} \\ 0^{\top} & b
 *   \end{array}\right)
 * = \left(\begin{array}{cc} \multicolumn{2}{c}{A} \\ 0^{\top} & a
 *   \end{array}\right)^{-1}
 * \f]
 *
 * \return Result matrix \a B as a new 2x3 matrix.
 */
Gan_Matrix23
 gan_mat23h_invert_s ( const Gan_Matrix23 *A, double a, double *b )
{
   Gan_Matrix23 B;

   gan_mat23h_invert_q ( A, a, &B, b );
   return B;
}

/**
 * \}
 */

/**
 * \addtogroup FixedSizeMatrixExtract
 * \{
 */

/**
 * \brief Extract rows of 2x3 matrix.
 *
 * Extract rows of 2x3 matrix \a A into 3-vectors \a p and \a q.
 * The arrangement can be written
 * \f[
 *     A = \left(\begin{array}{c} p^{\top} \\ q^{\top} \end{array}\right)
 * \f]
 *
 * \return No value.
 */
void
 gan_mat23_get_rows_s ( const Gan_Matrix23 *A, Gan_Vector3 *p, Gan_Vector3 *q )
{
   gan_mat23_get_rows_q ( A, p, q );
}

/**
 * \brief Extract columns of 2x3 matrix
 *
 * Extract columns of 2x3 matrix \a A into 2-vectors \a p, \a q and \a r.
 * The arrangement can be written
 * \f[
 *     A = \left(\begin{array}{ccc} p & q & r \end{array}\right)
 * \f]
 *
 * \return No value.
 */
void
 gan_mat23_get_cols_s ( const Gan_Matrix23 *A,
                        Gan_Vector2 *p, Gan_Vector2 *q, Gan_Vector2 *r )
{
   gan_mat23_get_cols_q ( A, p, q, r );
}

/**
 * \brief Extract left-hand 2x2 part of 2x3 matrix.
 *
 * Extract left-hand 2x2 part of 2x3 matrix \a A into 2x2 matrix \a B.
 *
 * \return Result \a B as a new 2x2 matrix.
 */
Gan_Matrix22 gan_mat23_get_m22l_s ( const Gan_Matrix23 *A )
{
   Gan_Matrix22 B;

   (void)gan_mat23_get_m22l_q ( A, &B );
   return B;
}

/**
 * \brief Extract right-hand column of 2x3 matrix.
 *
 * Extract right-hand column of 2x3 matrix \a A into 2-vector \a p.
 *
 * \return Result \a p as a new 2-vector.
 */
Gan_Vector2 gan_mat23_get_v2r_s ( const Gan_Matrix23 *A )
{
   Gan_Vector2 p;

   (void)gan_mat23_get_v2r_q ( A, &p );
   return p;
}

/**
 * \}
 */

/**
 * \addtogroup FixedSizeMatrixBuild
 * \{
 */

/**
 * \brief Build 2x3 matrix from 2x2 matrix and column 2-vector.
 *
 * Build 2x3 matrix from 2x2 matrix \a B and column 2-vector \a p,
 * which are respectively inserted in the left-hand and right-hand parts
 * of the 2x3 matrix. The arrangement is thus
 * \f[
 *    \left(\begin{array}{cc} B & p \end{array}\right)
 * \f]
 *
 * \return Result as a new 2x3 matrix.
 */
Gan_Matrix23 gan_mat23_set_parts_s ( const Gan_Matrix22 *B, const Gan_Vector2 *p )
{
   Gan_Matrix23 A;

   (void)gan_mat23_set_parts_q ( &A, B, p );
   return A;
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
 * \brief Convert generic rectangular matrix to 2x3 matrix structure.
 *
 * Convert generic rectangular matrix \a A to 2x3 matrix structure \a B.
 *
 * \return Pointer to result matrix \a B.
 */
Gan_Matrix23 *
 gan_mat23_from_mat_q ( const Gan_Matrix *A, Gan_Matrix23 *B )
{
   if ( !gan_mat_read_va ( A, 2, 3, &B->xx, &B->xy, &B->xz,
                                    &B->yx, &B->yy, &B->yz ) )
   {
      gan_err_register ( "gan_mat23_from_mat_q", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   return B;
}

/**
 * \}
 */

/* define all the standard small matrix functions for 2x3 matrices */
#include <gandalf/linalg/2x3mat_noc.h>
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

/**
 * File:          $RCSfile: 3x3symmat_noc.c,v $
 * Module:        3x3 symmetric matrices (double precision)
 * Part of:       Gandalf Library
 *
 * Revision:      $Revision: 1.1.2.1 $
 * Last edited:   $Date: 2007/10/14 03:16:22 $
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

/**
 * \addtogroup FixedSizeMatrixIO
 * \{
 */

/**
 * \brief Print 3x3 symmetric matrix to file
 *
 * Print 3x3 symmetric matrix \a A to file pointer \a fp, with prefix
 * string \a prefix, indentation \a indent and floating-point format \a fmt.
 *
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 */
Gan_Bool
 gan_symmat33_fprint ( FILE *fp, const Gan_SquMatrix33 *A,
                       const char *prefix, int indent, const char *fmt )
{
   int i;

   assert ( A->type == GAN_SYMMETRIC_MATRIX33 );

   /* print indentation on first line */
   for ( i = indent-1; i >= 0; i-- ) fprintf ( fp, " " );
   fprintf ( fp, "%s\n", prefix );
   for ( i = indent-1; i >= 0; i-- ) fprintf ( fp, " " );
   fprintf ( fp, " " ); fprintf ( fp, fmt, A->xx );
   fprintf ( fp, " " ); fprintf ( fp, fmt, A->yx );
   fprintf ( fp, " " ); fprintf ( fp, fmt, A->zx );
   fprintf ( fp, "\n" );
   for ( i = indent-1; i >= 0; i-- ) fprintf ( fp, " " );
   fprintf ( fp, " " ); fprintf ( fp, fmt, A->yx );
   fprintf ( fp, " " ); fprintf ( fp, fmt, A->yy );
   fprintf ( fp, " " ); fprintf ( fp, fmt, A->zy );
   fprintf ( fp, "\n" );
   for ( i = indent-1; i >= 0; i-- ) fprintf ( fp, " " );
   fprintf ( fp, " " ); fprintf ( fp, fmt, A->zx );
   fprintf ( fp, " " ); fprintf ( fp, fmt, A->zy );
   fprintf ( fp, " " ); fprintf ( fp, fmt, A->zz );
   fprintf ( fp, "\n" );

   /* success */
   return GAN_TRUE;
}

/**
 * \brief Read 3x3 symmetric matrix from file.
 *
 * Read 3x3 symmetric matrix \a A from file pointer \a fp. The prefix
 * string for the matrix is read from the file into the \a prefix string, up
 * to the maximum length \a prefix_len of the \a prefix string. Any remaining
 * characters after \a prefix has been filled are ignored. Pass \c NULL for
 * \a prefix and zero for \a prefix_len to ignore the prefix string.
 *
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 */
Gan_Bool
 gan_symmat33_fscanf ( FILE *fp, Gan_SquMatrix33 *A,
                       char *prefix, int prefix_len )
{
   int ch, result;
   double dum1, dum2, dum3;

   /* read indentation */
   for(;;)
      if ( (ch = getc(fp)) != ' ' || ch == EOF )
         break;

   /* end of file means corrupted file */
   if ( ch == EOF )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_symmat33_fscanf", GAN_ERROR_CORRUPTED_FILE, "" );
      return GAN_FALSE;
   }

   /* rewind one character to reread the prefix string */
   fseek ( fp, -1, SEEK_CUR );

   /* read prefix string, terminated by end-of-line */
   for ( prefix_len--; prefix_len > 0; prefix_len-- )
   {
      ch = getc(fp);
      if ( ch == EOF || ch == '\n' ) break;
      *prefix++ = (char)ch;
   }

   gan_err_test_bool ( ch == '\n', "gan_symmat33_fscanf",
                       GAN_ERROR_CORRUPTED_FILE, "" );

   /* terminate string */
   *prefix = '\0';

   /* read rest of string if necessary */
   if ( prefix_len == 0 )
   {
      for(;;)
      {
         ch = getc(fp);
         if ( ch == EOF || ch == '\n' ) break;
      }

      gan_err_test_bool ( ch == '\n', "gan_symmat33_fscanf",
                          GAN_ERROR_CORRUPTED_FILE, "" );
   }

   /* read rows of matrix */
   result = 0;
   result += fscanf ( fp, "\n%lf%lf%lf\n", &A->xx, &dum1, &dum2 );
   result += fscanf ( fp, "%lf%lf%lf\n",   &A->yx, &A->yy, &dum3 );
   result += fscanf ( fp, "%lf%lf%lf",     &A->zx, &A->zy, &A->zz );
   gan_err_test_bool ( result == 9, "gan_symmat33_fscanf",
                       GAN_ERROR_CORRUPTED_FILE, "" );

   /* read end-of-line character */
   result = getc(fp);
   gan_err_test_bool ( result == '\n', "gan_symmat33_fscanf",
                       GAN_ERROR_CORRUPTED_FILE, "" );

   /* check that matrix is indeed symmetric */
   gan_err_test_bool ( dum1 == A->yx && dum2 == A->zx && dum3 == A->zy,
                       "gan_symmat33_fscanf", GAN_ERROR_CORRUPTED_FILE, "" );

   /* success */
   gan_eval ( A->type = GAN_SYMMETRIC_MATRIX33 );
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
 * \brief Return 3x3 symmetric matrix filled with values.
 *
 * Return 3x3 symmetric matrix filled with values:
 * \f[
 *  \left(\begin{array}{ccc} XX & YX & ZX \\ YX & YY & ZY \\
 *                           ZX & ZY & ZZ \end{array}\right)
 * \f]
 */
Gan_SquMatrix33
 gan_symmat33_fill_s ( double XX,
                       double YX, double YY,
                       double ZX, double ZY, double ZZ )
{
   Gan_SquMatrix33 A;

   (void)gan_symmat33_fill_q ( &A, XX, YX, YY, ZX, ZY, ZZ );
   return A;
}

/**
 * \}
 */

/**
 * \addtogroup FixedSizeMatrixCholesky
 * \{
 */

/**
 * \brief Compute Cholesky factorisation of symmetric 3x3 matrix.
 *
 * Compute Cholesky factorisation \f$ \mbox{chol}(A) \f$ of symmetric 3x3
 * matrix \a A, writing the result into lower-triangular matrix \a B,
 * so that
 * \f[
 *   B B^{\top} = A
 * \f]
 *
 * \a A must be positive definite.
 *
 * \return Pointer to result matrix \a B on success, \c NULL on failure.
 */
Gan_SquMatrix33 *
 gan_symmat33_cholesky ( Gan_SquMatrix33 *A, Gan_SquMatrix33 *B,
                         int *error_code )
{
   double tmp;

   gan_err_test_ptr ( A->type == GAN_SYMMETRIC_MATRIX33,
                      "gan_symmat33_cholesky", GAN_ERROR_ILLEGAL_ARGUMENT,
                      "" );

   if ( A->xx <= 0.0 )
   {
      if ( error_code == NULL )
      {
         gan_err_flush_trace();
         gan_err_register ( "gan_symmat33_cholesky",
                            GAN_ERROR_NOT_POSITIVE_DEFINITE, "" );
      }
      else
         *error_code = GAN_ERROR_NOT_POSITIVE_DEFINITE;

      return NULL;
   }
   
   B->xx = sqrt(A->xx);
   B->yx = A->yx/B->xx;
   tmp = A->yy - B->yx*B->yx;
   if ( tmp <= 0.0 )
   {
      if ( error_code == NULL )
      {
         gan_err_flush_trace();
         gan_err_register ( "gan_symmat33_cholesky",
                            GAN_ERROR_NOT_POSITIVE_DEFINITE, "" );
      }
      else
         *error_code = GAN_ERROR_NOT_POSITIVE_DEFINITE;

      return NULL;
   }
   
   B->yy = sqrt(tmp);
   B->zx = A->zx/B->xx;
   B->zy = (A->zy - B->yx*B->zx)/B->yy;
   tmp = A->zz - B->zx*B->zx - B->zy*B->zy;
   if ( tmp <= 0.0 )
   {
      if ( error_code == NULL )
      {
         gan_err_flush_trace();
         gan_err_register ( "gan_symmat33_cholesky",
                            GAN_ERROR_NOT_POSITIVE_DEFINITE, "" );
      }
      else
         *error_code = GAN_ERROR_NOT_POSITIVE_DEFINITE;

      return NULL;
   }

   B->zz = sqrt(tmp);
   gan_eval ( B->type = GAN_LOWER_TRI_MATRIX33 );
   return B;
}

/**
 * \}
 */

/**
 * \addtogroup FixedSizeMatrixInvert
 * \{
 */

/**
 * \brief Compute inverse of symmetric 3x3 matrix.
 *
 * Compute inverse \f$ A^{-1} \f$ of symmetric 3x3 matrix \a A, writing the
 * result into matrix \a B, so that
 * \f[
 *    A B = B A = I
 * \f]
 *
 * \a A must be non-singular.
 *
 * \return Pointer to result matrix \a B on success, \c NULL on failure.
 */
Gan_SquMatrix33 *
 gan_symmat33_invert ( Gan_SquMatrix33 *A, Gan_SquMatrix33 *B,
                       int *error_code )
{
   double d1 = A->yy*A->zz-A->zy*A->zy, d2 = A->zy*A->zx-A->yx*A->zz,
          d3 = A->yx*A->zy-A->yy*A->zx;
   double det = A->xx*d1 + A->yx*d2 + A->zx*d3;

   gan_err_test_ptr ( A->type == GAN_SYMMETRIC_MATRIX33,
                      "gan_symmat33_invert", GAN_ERROR_ILLEGAL_ARGUMENT,
                      "" );
   if ( det == 0.0 )
   {
      if ( error_code == NULL )
      {
         gan_err_flush_trace();
         gan_err_register ( "gan_symmat33_invert", GAN_ERROR_SINGULAR_MATRIX,
                            "" );
      }
      else
         *error_code = GAN_ERROR_SINGULAR_MATRIX;

      return NULL;
   }

   if ( A == B )
   {
      Gan_SquMatrix33 Ap = *A;
      
      B->xx = d1/det;
      B->yx = d2/det; B->yy = (Ap.zz*Ap.xx-Ap.zx*Ap.zx)/det;
      B->zx = d3/det; B->zy = (Ap.zx*Ap.yx-Ap.zy*Ap.xx)/det;
                      B->zz = (Ap.xx*Ap.yy-Ap.yx*Ap.yx)/det;
   }
   else
   {
      B->xx = d1/det;
      B->yx = d2/det; B->yy = (A->zz*A->xx-A->zx*A->zx)/det;
      B->zx = d3/det; B->zy = (A->zx*A->yx-A->zy*A->xx)/det;
                      B->zz = (A->xx*A->yy-A->yx*A->yx)/det;
      gan_eval ( B->type = GAN_SYMMETRIC_MATRIX33 );
   }

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
 * \brief Extract parts of 3x3 symmetric matrix.
 *
 * Extract top-left 2x2 part, bottom-left 1x2 part and bottom-right element
 * of 3x3 symmetric matrix \a A into 2x2 lower triangular matrix \a B,
 * row 2-vector \a p and scalar pointer \a s. The arrangement is
 * \f[
 *     A = \left(\begin{array}{cc} B & p \\ p^{\top} & s \end{array}\right)
 * \f]
 * If any of \a B, \a p or \a s are \c NULL, the corresponding data is
 * not referenced.
 *
 * \return No value.
 */
void
 gan_symmat33_get_parts_s ( const Gan_SquMatrix33 *A,
                            Gan_SquMatrix22 *B, Gan_Vector2 *p, double *s )
{
   Gan_SquMatrix22 Bp;
   Gan_Vector2     pp;
   double        sp;

   gan_symmat33_get_parts_q ( A, (B == NULL) ? &Bp : B, (p == NULL) ? &pp : p,
                              (s == NULL) ? &sp : s );
}

/**
 * \}
 */

/**
 * \addtogroup FixedSizeMatrixBuild
 * \{
 */

/**
 * \brief Build a 3x3 symmetric matrix from parts.
 *
 * Build 3x3 symmetric matrix from 2x2 symmetric matrix
 * \a B, 2-vector \a p, and scalar \a s, which are respectively inserted
 * in the top-left, bottom-left/top-right and bottom-right parts of the 3x3
 * matrix. The arrangement of the matrix is
 * \f[
 *    \left(\begin{array}{cc} B & p \\ p^{\top} & s \end{array}\right)
 * \f]
 * If \a p is passed as \c NULL, the corresponding part of the result is
 * filled with zeros.
 *
 * \return Filled matrix.
 */
Gan_SquMatrix33
 gan_symmat33_set_parts_s ( Gan_SquMatrix22 *B, const Gan_Vector2 *p, double s )
{
   Gan_SquMatrix33 A;
   Gan_Vector2 zerov = { 0.0, 0.0 };

   (void)gan_symmat33_set_parts_q ( &A, B, (p == NULL) ? &zerov : p, s );
   return A;
}

/**
 * \}
 */

/**
 * \addtogroup FixedSizeMatrixConvert
 * \{
 */

/**
 * \brief Convert symmetric 3x3 matrix to generic 3x3 matrix.
 *
 * Convert symmetric 3x3 matrix \a A to generic 3x3 matrix \a B.
 *
 * \return Pointer to result matrix \a B.
 */
Gan_Matrix33 *
 gan_symmat33_to_mat33_q ( const Gan_SquMatrix33 *A, Gan_Matrix33 *B )
{
   gan_err_test_ptr ( A->type == GAN_SYMMETRIC_MATRIX33,
                      "gan_symmat33_to_mat33_q", GAN_ERROR_ILLEGAL_TYPE, "" );
   B->xx = A->xx;
   B->xy = B->yx = A->yx; B->yy = A->yy;
   B->xz = B->zx = A->zx; B->yz = B->zy = A->zy; B->zz = A->zz;
   return B;
}

/**
 * \brief Convert symmetric 3x3 matrix to generic 3x3 matrix.
 *
 * Convert symmetric 3x3 matrix \a A to generic 3x3 matrix.
 *
 * \return Result matrix.
 */
Gan_Matrix33
 gan_symmat33_to_mat33_s ( const Gan_SquMatrix33 *A )
{
   Gan_Matrix33 B;

   gan_symmat33_to_mat33_q ( A, &B );
   return B;
}

/**
 * \}
 */

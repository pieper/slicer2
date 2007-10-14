/**
 * File:          $RCSfile: 2x2symmat_noc.c,v $
 * Module:        2x2 symmetric matrices (double precision)
 * Part of:       Gandalf Library
 *
 * Revision:      $Revision: 1.1.2.1 $
 * Last edited:   $Date: 2007/10/14 03:16:21 $
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
 * \brief Print 2x2 symmetric matrix to file
 *
 * Print 2x2 symmetric matrix \a A to file pointer \a fp, with prefix
 * string \a prefix, indentation \a indent and floating-point format \a fmt.
 *
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 */
Gan_Bool
 gan_symmat22_fprint ( FILE *fp, const Gan_SquMatrix22 *A,
                       const char *prefix, int indent, const char *fmt )
{
   int i;

   assert ( A->type == GAN_SYMMETRIC_MATRIX22 );

   /* print indentation on first line */
   for ( i = indent-1; i >= 0; i-- ) fprintf ( fp, " " );
   fprintf ( fp, "%s\n", prefix );
   for ( i = indent-1; i >= 0; i-- ) fprintf ( fp, " " );
   fprintf ( fp, " " ); fprintf ( fp, fmt, A->xx );
   fprintf ( fp, " " ); fprintf ( fp, fmt, A->yx );
   fprintf ( fp, "\n" );
   for ( i = indent-1; i >= 0; i-- ) fprintf ( fp, " " );
   fprintf ( fp, " " ); fprintf ( fp, fmt, A->yx );
   fprintf ( fp, " " ); fprintf ( fp, fmt, A->yy );
   fprintf ( fp, "\n" );

   /* success */
   return GAN_TRUE;
}

/**
 * \brief Read 2x2 symmetric matrix from file.
 *
 * Read 2x2 symmetric matrix \a A from file pointer \a fp. The prefix
 * string for the matrix is read from the file into the \a prefix string, up
 * to the maximum length \a prefix_len of the \a prefix string. Any remaining
 * characters after \a prefix has been filled are ignored. Pass \c NULL for
 * \a prefix and zero for \a prefix_len to ignore the prefix string.
 *
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 */
Gan_Bool
 gan_symmat22_fscanf ( FILE *fp, Gan_SquMatrix22 *A,
                       char *prefix, int prefix_len )
{
   int ch, result;
   double dum;

   /* read indentation */
   for(;;)
      if ( (ch = getc(fp)) != ' ' || ch == EOF )
         break;

   /* end of file means corrupted file */
   if ( ch == EOF )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_symmat22_fscanf", GAN_ERROR_CORRUPTED_FILE, "" );
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

   gan_err_test_bool ( ch == '\n', "gan_symmat22_fscanf",
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

      gan_err_test_bool ( ch == '\n', "gan_symmat22_fscanf",
                          GAN_ERROR_CORRUPTED_FILE, "" );
   }

   /* read rows of matrix */
   result = 0;
   result += fscanf ( fp, "\n%lf%lf\n", &A->xx, &dum );
   result += fscanf ( fp, "%lf%lf",     &A->yx, &A->yy );
   gan_err_test_bool ( result == 4, "gan_symmat22_fscanf",
                       GAN_ERROR_CORRUPTED_FILE, "" );

   /* read end-of-line character */
   result = getc(fp);
   gan_err_test_bool ( result == '\n', "gan_symmat22_fscanf",
                       GAN_ERROR_CORRUPTED_FILE, "" );

   /* check that matrix is indeed symmetric */
   gan_err_test_bool ( dum == A->yx, "gan_symmat22_fscanf",
                       GAN_ERROR_CORRUPTED_FILE, "" );

   /* success */
   gan_eval ( A->type = GAN_SYMMETRIC_MATRIX22 );
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
 * \brief Return 2x2 symmetric matrix filled with values.
 *
 * Return 2x2 symmetric matrix filled with values:
 * \f[
 *  \left(\begin{array}{cc} XX & YX \\ YX & YY \end{array}\right)
 * \f]
 */
Gan_SquMatrix22
 gan_symmat22_fill_s ( double XX, double YX, double YY )
{
   Gan_SquMatrix22 A;

   (void)gan_symmat22_fill_q ( &A, XX, YX, YY );
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
 * \brief Compute Cholesky factorisation of symmetric 2x2 matrix.
 *
 * Compute Cholesky factorisation \f$ \mbox{chol}(A) \f$ of symmetric 2x2
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
Gan_SquMatrix22 *
 gan_symmat22_cholesky ( Gan_SquMatrix22 *A, Gan_SquMatrix22 *B,
                         int *error_code )
{
   gan_err_test_ptr ( A->type == GAN_SYMMETRIC_MATRIX22,
                      "gan_symmat22_cholesky", GAN_ERROR_ILLEGAL_ARGUMENT,
                      "" );

   if ( A->xx <= 0.0 )
   {
      if ( error_code == NULL )
      {
         gan_err_flush_trace();
         gan_err_register ( "gan_symmat22_cholesky",
                            GAN_ERROR_NOT_POSITIVE_DEFINITE, "" );
      }
      else
         *error_code = GAN_ERROR_NOT_POSITIVE_DEFINITE;

      return NULL;
   }
         
   B->xx = sqrt(A->xx);
   B->yx = A->yx/B->xx;
   if ( A->yy <= B->yx*B->yx )
   {
      if ( error_code == NULL )
      {
         gan_err_flush_trace();
         gan_err_register ( "gan_symmat22_cholesky",
                            GAN_ERROR_NOT_POSITIVE_DEFINITE, "" );
      }
      else
         *error_code = GAN_ERROR_NOT_POSITIVE_DEFINITE;

      return NULL;
   }
   
   B->yy = sqrt(A->yy - B->yx*B->yx);
   gan_eval ( B->type = GAN_LOWER_TRI_MATRIX22 );
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
 * \brief Compute inverse of symmetric 2x2 matrix.
 *
 * Compute inverse \f$ A^{-1} \f$ of symmetric 2x2 matrix \a A, writing the
 * result into matrix \a B, so that
 * \f[
 *    A B = B A = I
 * \f]
 *
 * \a A must be non-singular.
 *
 * \return Pointer to result matrix \a B on success, \c NULL on failure.
 */
Gan_SquMatrix22 *
 gan_symmat22_invert ( Gan_SquMatrix22 *A, Gan_SquMatrix22 *B,
                       int *error_code )
{
   double det = A->xx*A->yy - A->yx*A->yx;

   if ( det == 0.0 )
   {
      if ( error_code == NULL )
      {
         gan_err_flush_trace();
         gan_err_register ( "gan_symmat22_invert_q", GAN_ERROR_SINGULAR_MATRIX,
                            "" );
      }
      else
         *error_code = GAN_ERROR_SINGULAR_MATRIX;

      return NULL;
   }

   if ( A == B )
   {
      Gan_SquMatrix22 Ap = *A;

      B->xx =  Ap.yy/det;
      B->yx = -Ap.yx/det; B->yy = Ap.xx/det;
   }
   else
   {
      B->xx =  A->yy/det;
      B->yx = -A->yx/det; B->yy = A->xx/det;
      gan_eval ( B->type = GAN_SYMMETRIC_MATRIX22 );
   }

   return B;
}

/**
 * \}
 */

/**
 * \addtogroup FixedSizeMatrixConvert
 * \{
 */

/**
 * \brief Convert symmetric 2x2 matrix to generic 2x2 matrix.
 *
 * Convert symmetric 2x2 matrix \a A to generic 2x2 matrix \a B.
 *
 * \return Pointer to result matrix \a B.
 */
Gan_Matrix22 *gan_symmat22_to_mat22_q ( const Gan_SquMatrix22 *A, Gan_Matrix22 *B )
{
   gan_err_test_ptr ( A->type == GAN_SYMMETRIC_MATRIX22,
                      "gan_symmat22_to_mat22_q", GAN_ERROR_ILLEGAL_TYPE, "" );
   B->xx = A->xx;
   B->xy = B->yx = A->yx;
   B->yy = A->yy;
   return B;
}

/**
 * \brief Convert symmetric 2x2 matrix to generic 2x2 matrix.
 *
 * Convert symmetric 2x2 matrix \a A to generic 2x2 matrix.
 *
 * \return Result matrix.
 */
Gan_Matrix22 gan_symmat22_to_mat22_s ( const Gan_SquMatrix22 *A )
{
   Gan_Matrix22 B;

   gan_symmat22_to_mat22_q ( A, &B );
   return B;
}

/**
 * \}
 */

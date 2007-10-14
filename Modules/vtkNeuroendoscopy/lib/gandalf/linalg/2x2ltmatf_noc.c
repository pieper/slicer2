/**
 * File:          $RCSfile: 2x2ltmatf_noc.c,v $
 * Module:        2x2 lower triangular matrices (single precision)
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

/**
 * \addtogroup FixedSizeMatrixIO
 * \{
 */

/**
 * \brief Print 2x2 lower triangular matrix to file
 *
 * Print 2x2 lower triangular matrix \a A to file pointer \a fp, with prefix
 * string \a prefix, indentation \a indent and floating-point format \a fmt.
 *
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 */
Gan_Bool
 gan_ltmat22f_fprint ( FILE *fp, const Gan_SquMatrix22_f *A,
                       const char *prefix, int indent, const char *fmt )
{
   int i;

   assert ( A->type == GAN_LOWER_TRI_MATRIX22_F );

   /* print indentation on first line */
   for ( i = indent-1; i >= 0; i-- ) fprintf ( fp, " " );
   fprintf ( fp, "%s\n", prefix );
   for ( i = indent-1; i >= 0; i-- ) fprintf ( fp, " " );
   fprintf ( fp, " " ); fprintf ( fp, fmt, A->xx );
   fprintf ( fp, " " ); fprintf ( fp, fmt, 0.0F );
   fprintf ( fp, "\n" );
   for ( i = indent-1; i >= 0; i-- ) fprintf ( fp, " " );
   fprintf ( fp, " " ); fprintf ( fp, fmt, A->yx );
   fprintf ( fp, " " ); fprintf ( fp, fmt, A->yy );
   fprintf ( fp, "\n" );

   /* success */
   return GAN_TRUE;
}

/**
 * \brief Read 2x2 lower triangular matrix from file.
 *
 * Read 2x2 lower triangular matrix \a A from file pointer \a fp. The prefix
 * string for the matrix is read from the file into the \a prefix string, up
 * to the maximum length \a prefix_len of the \a prefix string. Any remaining
 * characters after \a prefix has been filled are ignored. Pass \c NULL for
 * \a prefix and zero for \a prefix_len to ignore the prefix string.
 *
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 */
Gan_Bool
 gan_ltmat22f_fscanf ( FILE *fp, Gan_SquMatrix22_f *A,
                       char *prefix, int prefix_len )
{
   int ch, result;
   float dum;

   /* read indentation */
   for(;;)
      if ( (ch = getc(fp)) != ' ' || ch == EOF )
         break;

   /* end of file means corrupted file */
   if ( ch == EOF )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_ltmat22f_fscanf", GAN_ERROR_CORRUPTED_FILE, "" );
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

   gan_err_test_bool ( ch == '\n', "gan_ltmat22f_fscanf",
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

      gan_err_test_bool ( ch == '\n', "gan_ltmat22f_fscanf",
                          GAN_ERROR_CORRUPTED_FILE, "" );
   }

   /* read rows of matrix */
   result = 0;
   result += fscanf ( fp, "\n%f%f\n", &A->xx, &dum );
   result += fscanf ( fp, "%f%f",     &A->yx, &A->yy );
   gan_err_test_bool ( result == 4, "gan_ltmat22f_fscanf",
                       GAN_ERROR_CORRUPTED_FILE, "" );

   /* read end-of-line character */
   result = getc(fp);
   gan_err_test_bool ( result == '\n', "gan_ltmat22f_fscanf",
                       GAN_ERROR_CORRUPTED_FILE, "" );

   /* check that matrix is indeed upper triangular */
   gan_err_test_bool ( dum == 0.0F, "gan_ltmat22f_fscanf",
                       GAN_ERROR_CORRUPTED_FILE, "" );

   /* success */
   gan_eval ( A->type = GAN_LOWER_TRI_MATRIX22_F );
   return GAN_TRUE;
}

/**
 * \brief Print 2x2 upper triangular matrix to file.
 *
 * Print 2x2 upper triangular matrix \a A to file pointer \a fp, with prefix
 * string \a prefix, indentation \a indent and floating-point format \a fmt.
 *
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 */
Gan_Bool
 gan_ltmat22Tf_fprint ( FILE *fp, const Gan_SquMatrix22_f *A,
                        const char *prefix, int indent, const char *fmt )
{
   int i;

   assert ( A->type == GAN_LOWER_TRI_MATRIX22_F );

   /* print indentation on first line */
   for ( i = indent-1; i >= 0; i-- ) fprintf ( fp, " " );
   fprintf ( fp, "%s\n", prefix );
   for ( i = indent-1; i >= 0; i-- ) fprintf ( fp, " " );
   fprintf ( fp, " " ); fprintf ( fp, fmt, A->xx );
   fprintf ( fp, " " ); fprintf ( fp, fmt, A->yx );
   fprintf ( fp, "\n" );
   for ( i = indent-1; i >= 0; i-- ) fprintf ( fp, " " );
   fprintf ( fp, " " ); fprintf ( fp, fmt, 0.0F );
   fprintf ( fp, " " ); fprintf ( fp, fmt, A->yy );
   fprintf ( fp, "\n" );

   /* success */
   return GAN_TRUE;
}

/**
 * \brief Read 2x2 upper triangular matrix from file.
 *
 * Read 2x2 upper triangular matrix \a A from file pointer \a fp. The prefix
 * string for the matrix is read from the file into the \a prefix string, up
 * to the maximum length \a prefix_len of the \a prefix string. Any remaining
 * characters after \a prefix has been filled are ignored. Pass \c NULL for
 * \a prefix and zero for \a prefix_len to ignore the prefix string.
 *
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 */
Gan_Bool
 gan_ltmat22Tf_fscanf ( FILE *fp, Gan_SquMatrix22_f *A,
                        char *prefix, int prefix_len )
{
   int ch, result;
   float dum;

   /* read indentation */
   for(;;)
      if ( (ch = getc(fp)) != ' ' || ch == EOF )
         break;

   /* end of file means corrupted file */
   if ( ch == EOF )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_ltmat22Tf_fscanf", GAN_ERROR_CORRUPTED_FILE, "");
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

   gan_err_test_bool ( ch == '\n', "gan_ltmat22Tf_fscanf",
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

      gan_err_test_bool ( ch == '\n', "gan_ltmat22Tf_fscanf",
                          GAN_ERROR_CORRUPTED_FILE, "" );
   }

   /* read rows of matrix */
   result = 0;
   result += fscanf ( fp, "\n%f%f\n", &A->xx, &A->yx );
   result += fscanf ( fp, "%f%f",     &dum, &A->yy );
   gan_err_test_bool ( result == 4, "gan_ltmat22Tf_fscanf",
                       GAN_ERROR_CORRUPTED_FILE, "" );

   /* read end-of-line character */
   result = getc(fp);
   gan_err_test_bool ( result == '\n', "gan_ltmat22Tf_fscanf",
                       GAN_ERROR_CORRUPTED_FILE, "" );

   /* check that matrix is indeed upper triangular */
   gan_err_test_bool ( dum == 0.0F, "gan_ltmat22Tf_fscanf",
                       GAN_ERROR_CORRUPTED_FILE, "" );

   /* success */
   gan_eval ( A->type = GAN_LOWER_TRI_MATRIX22_F );
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
 * \brief Return 2x2 lower triangular matrix filled with values.
 *
 * Return 2x2 lower triangular matrix filled with values:
 * \f[
 *  \left(\begin{array}{cc} XX & 0 \\ YX & YY \end{array}\right)
 * \f]
 */
Gan_SquMatrix22_f
 gan_ltmat22f_fill_s ( float XX, float YX, float YY )
{
   Gan_SquMatrix22_f A;

   (void)gan_ltmat22f_fill_q ( &A, XX, YX, YY );
   return A;
}

/**
 * \}
 */

/**
 * \addtogroup FixedSizeMatrixInvert
 * \{
 */

/**
 * \brief Compute inverse of lower triangular 2x2 matrix.
 *
 * Compute inverse \f$ A^{-1} \f$ of lower triangular 2x2 matrix \a A,
 * writing the result into matrix \a B.
 *
 * \return Pointer to \a B on success, \c NULL on failure.
 */
Gan_SquMatrix22_f *
 gan_ltmat22f_invert ( Gan_SquMatrix22_f *A, Gan_SquMatrix22_f *B,
                       int *error_code )
{
   gan_err_test_ptr ( A->type == GAN_LOWER_TRI_MATRIX22_F,
                      "gan_ltmat22f_invert", GAN_ERROR_ILLEGAL_ARGUMENT,
                      "" );
   if ( A->xx == 0.0F || A->yy == 0.0F )
   {
      if ( error_code == NULL )
      {
         gan_err_flush_trace();
         gan_err_register ( "gan_ltmat22f_invert", GAN_ERROR_SINGULAR_MATRIX,
                            "" );
      }
      else
         *error_code = GAN_ERROR_SINGULAR_MATRIX;

      return NULL;
   }

   B->xx = 1.0F/A->xx;
   B->yx = -A->yx*B->xx/A->yy; B->yy = 1.0F/A->yy;
   gan_eval ( B->type = GAN_LOWER_TRI_MATRIX22_F );
   return B;
}

/**
 * \}
 */

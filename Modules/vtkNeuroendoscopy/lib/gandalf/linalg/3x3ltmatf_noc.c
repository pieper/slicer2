/**
 * File:          $RCSfile: 3x3ltmatf_noc.c,v $
 * Module:        3x3 lower triangular matrices (single precision)
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
 * \brief Print 3x3 lower triangular matrix to file
 *
 * Print 3x3 lower triangular matrix \a A to file pointer \a fp, with prefix
 * string \a prefix, indentation \a indent and floating-point format \a fmt.
 *
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 */
Gan_Bool
 gan_ltmat33f_fprint ( FILE *fp, const Gan_SquMatrix33_f *A,
                       const char *prefix, int indent, const char *fmt )
{
   int i;

   assert ( A->type == GAN_LOWER_TRI_MATRIX33_F );

   /* print indentation on first line */
   for ( i = indent-1; i >= 0; i-- ) fprintf ( fp, " " );
   fprintf ( fp, "%s\n", prefix );
   for ( i = indent-1; i >= 0; i-- ) fprintf ( fp, " " );
   fprintf ( fp, " " ); fprintf ( fp, fmt, A->xx );
   fprintf ( fp, " " ); fprintf ( fp, fmt, 0.0F );
   fprintf ( fp, " " ); fprintf ( fp, fmt, 0.0F );
   fprintf ( fp, "\n" );
   for ( i = indent-1; i >= 0; i-- ) fprintf ( fp, " " );
   fprintf ( fp, " " ); fprintf ( fp, fmt, A->yx );
   fprintf ( fp, " " ); fprintf ( fp, fmt, A->yy );
   fprintf ( fp, " " ); fprintf ( fp, fmt, 0.0F );
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
 * \brief Read 3x3 lower triangular matrix from file.
 *
 * Read 3x3 lower triangular matrix \a A from file pointer \a fp. The prefix
 * string for the matrix is read from the file into the \a prefix string, up
 * to the maximum length \a prefix_len of the \a prefix string. Any remaining
 * characters after \a prefix has been filled are ignored. Pass \c NULL for
 * \a prefix and zero for \a prefix_len to ignore the prefix string.
 *
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 */
Gan_Bool
 gan_ltmat33f_fscanf ( FILE *fp, Gan_SquMatrix33_f *A,
                       char *prefix, int prefix_len )
{
   int ch, result;
   float dum[3];

   /* read indentation */
   for(;;)
      if ( (ch = getc(fp)) != ' ' || ch == EOF )
         break;

   /* end of file means corrupted file */
   if ( ch == EOF )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_ltmat33f_fscanf", GAN_ERROR_CORRUPTED_FILE, "" );
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

   gan_err_test_bool ( ch == '\n', "gan_ltmat33f_fscanf",
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

      gan_err_test_bool ( ch == '\n', "gan_ltmat33f_fscanf",
                          GAN_ERROR_CORRUPTED_FILE, "" );
   }

   /* read rows of matrix */
   result = 0;
   result += fscanf ( fp, "\n%f%f%f\n", &A->xx, &dum[0], &dum[1] );
   result += fscanf ( fp, "%f%f%f\n",   &A->yx, &A->yy, &dum[2] );
   result += fscanf ( fp, "%f%f%f",     &A->zx, &A->zy, &A->zz );
   gan_err_test_bool ( result == 9, "gan_ltmat33f_fscanf",
                       GAN_ERROR_CORRUPTED_FILE, "" );

   /* read end-of-line character */
   result = getc(fp);
   gan_err_test_bool ( result == '\n', "gan_ltmat33f_fscanf",
                       GAN_ERROR_CORRUPTED_FILE, "" );

   /* check that matrix is indeed upper triangular */
   gan_err_test_bool ( dum[0] == 0.0F && dum[1] == 0.0F && dum[2] == 0.0F,
                       "gan_ltmat33f_fscanf", GAN_ERROR_CORRUPTED_FILE, "" );

   /* success */
   gan_eval ( A->type = GAN_LOWER_TRI_MATRIX33_F );
   return GAN_TRUE;
}

/**
 * \brief Print 3x3 upper triangular matrix to file.
 *
 * Print 3x3 upper triangular matrix \a A to file pointer \a fp, with prefix
 * string \a prefix, indentation \a indent and floating-point format \a fmt.
 *
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 */
Gan_Bool
 gan_ltmat33Tf_fprint ( FILE *fp, const Gan_SquMatrix33_f *A,
                        const char *prefix, int indent, const char *fmt )
{
   int i;

   assert ( A->type == GAN_LOWER_TRI_MATRIX33_F );

   /* print indentation on first line */
   for ( i = indent-1; i >= 0; i-- ) fprintf ( fp, " " );
   fprintf ( fp, "%s\n", prefix );
   for ( i = indent-1; i >= 0; i-- ) fprintf ( fp, " " );
   fprintf ( fp, " " ); fprintf ( fp, fmt, A->xx );
   fprintf ( fp, " " ); fprintf ( fp, fmt, A->yx );
   fprintf ( fp, " " ); fprintf ( fp, fmt, A->zx );
   fprintf ( fp, "\n" );
   for ( i = indent-1; i >= 0; i-- ) fprintf ( fp, " " );
   fprintf ( fp, " " ); fprintf ( fp, fmt, 0.0F );
   fprintf ( fp, " " ); fprintf ( fp, fmt, A->yy );
   fprintf ( fp, " " ); fprintf ( fp, fmt, A->zy );
   fprintf ( fp, "\n" );
   for ( i = indent-1; i >= 0; i-- ) fprintf ( fp, " " );
   fprintf ( fp, " " ); fprintf ( fp, fmt, 0.0F );
   fprintf ( fp, " " ); fprintf ( fp, fmt, 0.0F );
   fprintf ( fp, " " ); fprintf ( fp, fmt, A->zz );
   fprintf ( fp, "\n" );

   /* success */
   return GAN_TRUE;
}

/**
 * \brief Read 3x3 upper triangular matrix from file.
 *
 * Read 3x3 upper triangular matrix \a A from file pointer \a fp. The prefix
 * string for the matrix is read from the file into the \a prefix string, up
 * to the maximum length \a prefix_len of the \a prefix string. Any remaining
 * characters after \a prefix has been filled are ignored. Pass \c NULL for
 * \a prefix and zero for \a prefix_len to ignore the prefix string.
 *
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 */
Gan_Bool
 gan_ltmat33Tf_fscanf ( FILE *fp, Gan_SquMatrix33_f *A,
                        char *prefix, int prefix_len )
{
   int ch, result;
   float dum[3];

   /* read indentation */
   for(;;)
      if ( (ch = getc(fp)) != ' ' || ch == EOF )
         break;

   /* end of file means corrupted file */
   if ( ch == EOF )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_ltmat33Tf_fscanf", GAN_ERROR_CORRUPTED_FILE, "");
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

   gan_err_test_bool ( ch == '\n', "gan_ltmat33Tf_fscanf",
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

      gan_err_test_bool ( ch == '\n', "gan_ltmat33Tf_fscanf",
                          GAN_ERROR_CORRUPTED_FILE, "" );
   }

   /* read rows of matrix */
   result = 0;
   result += fscanf ( fp, "\n%f%f%f\n", &A->xx, &A->yx, &A->zx );
   result += fscanf ( fp, "%f%f%f\n",   &dum[0], &A->yy, &A->zy );
   result += fscanf ( fp, "%f%f%f",     &dum[1], &dum[2], &A->zz );
   gan_err_test_bool ( result == 9, "gan_ltmat33Tf_fscanf",
                       GAN_ERROR_CORRUPTED_FILE, "" );

   /* read end-of-line character */
   result = getc(fp);
   gan_err_test_bool ( result == '\n', "gan_ltmat33Tf_fscanf",
                       GAN_ERROR_CORRUPTED_FILE, "" );

   /* check that matrix is indeed upper triangular */
   gan_err_test_bool ( dum[0] == 0.0F && dum[1] == 0.0F && dum[2] == 0.0F,
                       "gan_ltmat33Tf_fscanf", GAN_ERROR_CORRUPTED_FILE, "" );

   /* success */
   gan_eval ( A->type = GAN_LOWER_TRI_MATRIX33_F );
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
 * \brief Return 3x3 lower triangular matrix filled with values.
 *
 * Return 3x3 lower triangular matrix filled with values
 * \f$ \left(\begin{array}{ccc} XX & 0 & 0 \\ YX & YY & 0 \\
 *                              ZX & ZY & ZZ \end{array}\right) \f$
 */
Gan_SquMatrix33_f
 gan_ltmat33f_fill_s ( float XX,
                       float YX, float YY,
                       float ZX, float ZY, float ZZ )
{
   Gan_SquMatrix33_f A;

   (void)gan_ltmat33f_fill_q ( &A, XX, YX, YY, ZX, ZY, ZZ );
   return A;
}

/**
 * \}
 */

/**
 * \addtogroup FixedSizeMatrixExtract
 * \{
 */

/**
 * \brief Extract parts of 3x3 lower triangular matrix.
 *
 * Extract top-left 2x2 part, bottom-left 1x2 part and bottom-right element
 * of 3x3 lower triangular matrix \a A into 2x2 lower triangular matrix \a B,
 * row 2-vector \a p and scalar pointer \a s. The arrangement is
 * \f[
 *     A = \left(\begin{array}{cc} B & 0 \\ p^{\top} & s \end{array}\right)
 * \f]
 * If any of \a B, \a p or \a s are \c NULL, the corresponding data is
 * not referenced.
 */
void
 gan_ltmat33f_get_parts_s ( const Gan_SquMatrix33_f *A,
                            Gan_SquMatrix22_f *B, Gan_Vector2_f *p, float *s )
{
   Gan_SquMatrix22_f Bp;
   Gan_Vector2_f     pp;
   float        sp;

   gan_ltmat33f_get_parts_q ( A, (B == NULL) ? &Bp : B, (p == NULL) ? &pp : p,
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
 * \brief Build a 3x3 lower triangular matrix from parts.
 *
 * Build 3x3 lower triangular matrix from 2x2 lower triangular matrix
 * \a B, row 2-vector \a p, and scalar \a s, which are respectively inserted
 * in the top-left, bottom-left and bottom-right parts of the 3x3 matrix.
 * The arrangement of the matrix is
 * \f[
 *    \left(\begin{array}{cc} B & 0 \\ p^{\top} & s \end{array}\right)
 * \f]
 * If \a p is passed as \c NULL, the corresponding part of the result is
 * filled with zeros.
 */
Gan_SquMatrix33_f
 gan_ltmat33f_set_parts_s ( const Gan_SquMatrix22_f *B, const Gan_Vector2_f *p, float s )
{
   Gan_SquMatrix33_f A;
   Gan_Vector2_f zerov = { 0.0F, 0.0F };

   (void)gan_ltmat33f_set_parts_q ( &A, B, (p == NULL) ? &zerov : p, s );
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
 * \brief Compute inverse of lower triangular 3x3 matrix.
 *
 * Compute inverse \f$ A^{-1} \f$ of lower triangular 3x3 matrix \a A,
 * writing the result into matrix \a B.
 *
 * \return Pointer to \a B on success, \c NULL on failure.
 */
Gan_SquMatrix33_f *
 gan_ltmat33f_invert ( Gan_SquMatrix33_f *A, Gan_SquMatrix33_f *B,
                       int *error_code )
{
   gan_err_test_ptr ( A->type == GAN_LOWER_TRI_MATRIX33_F,
                      "gan_ltmat33f_invert", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
   if ( A->xx == 0.0F || A->yy == 0.0F || A->zz == 0.0F )
   {
      if ( error_code == NULL )
      {
         gan_err_flush_trace();
         gan_err_register ( "gan_ltmat33f_invert", GAN_ERROR_SINGULAR_MATRIX,
                            "" );
      }
      else
         *error_code = GAN_ERROR_SINGULAR_MATRIX;

      return NULL;
   }

   B->xx = 1.0F/A->xx;
   B->yx = -A->yx*B->xx/A->yy; B->yy = 1.0F/A->yy;
   B->zx = -(A->zx*B->xx + A->zy*B->yx)/A->zz;
   B->zy = -A->zy*B->yy/A->zz;
   B->zz = 1.0F/A->zz;
   gan_eval ( B->type = GAN_LOWER_TRI_MATRIX33_F );
   return B;
}

/**
 * \}
 */

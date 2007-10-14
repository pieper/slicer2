/**
 * File:          $RCSfile: 4x4ltmat_noc.c,v $
 * Module:        4x4 lower triangular matrices (double precision)
 * Part of:       Gandalf Library
 *
 * Revision:      $Revision: 1.1.2.1 $
 * Last edited:   $Date: 2007/10/14 03:16:22 $
 * Author:        $Author: ruetz $
 * Copyright:     (c) 2000 Imagineer Software Limited
 *
 * Notes:         Not to be compiled separately
 * Private func:        
 * History:        
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
 * \brief Print 4x4 lower triangular matrix to file
 *
 * Print 4x4 lower triangular matrix \a A to file pointer \a fp, with prefix
 * string \a prefix, indentation \a indent and floating-point format \a fmt.
 *
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 */
Gan_Bool
 gan_ltmat44_fprint ( FILE *fp, const Gan_SquMatrix44 *A,
                      const char *prefix, int indent, const char *fmt )
{
   int i;

   assert ( A->type == GAN_LOWER_TRI_MATRIX44 );

   /* print indentation on first line */
   for ( i = indent-1; i >= 0; i-- ) fprintf ( fp, " " );
   fprintf ( fp, "%s\n", prefix );
   for ( i = indent-1; i >= 0; i-- ) fprintf ( fp, " " );
   fprintf ( fp, " " ); fprintf ( fp, fmt, A->xx );
   fprintf ( fp, " " ); fprintf ( fp, fmt, 0.0 );
   fprintf ( fp, " " ); fprintf ( fp, fmt, 0.0 );
   fprintf ( fp, " " ); fprintf ( fp, fmt, 0.0 );
   fprintf ( fp, "\n" );
   for ( i = indent-1; i >= 0; i-- ) fprintf ( fp, " " );
   fprintf ( fp, " " ); fprintf ( fp, fmt, A->yx );
   fprintf ( fp, " " ); fprintf ( fp, fmt, A->yy );
   fprintf ( fp, " " ); fprintf ( fp, fmt, 0.0 );
   fprintf ( fp, " " ); fprintf ( fp, fmt, 0.0 );
   fprintf ( fp, "\n" );
   for ( i = indent-1; i >= 0; i-- ) fprintf ( fp, " " );
   fprintf ( fp, " " ); fprintf ( fp, fmt, A->zx );
   fprintf ( fp, " " ); fprintf ( fp, fmt, A->zy );
   fprintf ( fp, " " ); fprintf ( fp, fmt, A->zz );
   fprintf ( fp, " " ); fprintf ( fp, fmt, 0.0 );
   fprintf ( fp, "\n" );
   for ( i = indent-1; i >= 0; i-- ) fprintf ( fp, " " );
   fprintf ( fp, " " ); fprintf ( fp, fmt, A->wx );
   fprintf ( fp, " " ); fprintf ( fp, fmt, A->wy );
   fprintf ( fp, " " ); fprintf ( fp, fmt, A->wz );
   fprintf ( fp, " " ); fprintf ( fp, fmt, A->ww );
   fprintf ( fp, "\n" );

   /* success */
   return GAN_TRUE;
}

/**
 * \brief Read 4x4 lower triangular matrix from file.
 *
 * Read 4x4 lower triangular matrix \a A from file pointer \a fp. The prefix
 * string for the matrix is read from the file into the \a prefix string, up
 * to the maximum length \a prefix_len of the \a prefix string. Any remaining
 * characters after \a prefix has been filled are ignored. Pass \c NULL for
 * \a prefix and zero for \a prefix_len to ignore the prefix string.
 *
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 */
Gan_Bool
 gan_ltmat44_fscanf ( FILE *fp, Gan_SquMatrix44 *A,
                      char *prefix, int prefix_len )
{
   int ch, result;
   double dum[6];

   /* read indentation */
   for(;;)
      if ( (ch = getc(fp)) != ' ' || ch == EOF )
         break;

   /* end of file means corrupted file */
   if ( ch == EOF )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_ltmat44_fscanf", GAN_ERROR_CORRUPTED_FILE, "" );
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

   gan_err_test_bool ( ch == '\n', "gan_ltmat44_fscanf",
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

      gan_err_test_bool ( ch == '\n', "gan_ltmat44_fscanf",
                          GAN_ERROR_CORRUPTED_FILE, "" );
   }

   /* read rows of matrix */
   result = 0;
   result += fscanf ( fp, "\n%lf%lf%lf%lf\n", &A->xx,&dum[0],&dum[1],&dum[2] );
   result += fscanf ( fp, "%lf%lf%lf%lf\n",   &A->yx, &A->yy,&dum[3],&dum[4] );
   result += fscanf ( fp, "%lf%lf%lf%lf\n",   &A->zx, &A->zy, &A->zz,&dum[5] );
   result += fscanf ( fp, "%lf%lf%lf%lf",     &A->wx, &A->wy, &A->wz, &A->ww );
   gan_err_test_bool ( result == 16, "gan_ltmat44_fscanf",
                       GAN_ERROR_CORRUPTED_FILE, "" );

   /* read end-of-line character */
   result = getc(fp);
   gan_err_test_bool ( result == '\n', "gan_ltmat44_fscanf",
                       GAN_ERROR_CORRUPTED_FILE, "" );

   /* check that matrix is indeed upper triangular */
   gan_err_test_bool ( dum[0] == 0.0 && dum[1] == 0.0 && dum[2] == 0.0 &&
                       dum[3] == 0.0 && dum[4] == 0.0 && dum[5] == 0.0,
                       "gan_ltmat44_fscanf", GAN_ERROR_CORRUPTED_FILE, "" );

   /* success */
   gan_eval ( A->type = GAN_LOWER_TRI_MATRIX44 );
   return GAN_TRUE;
}

/**
 * \brief Print 4x4 upper triangular matrix to file.
 *
 * Print 4x4 upper triangular matrix \a A to file pointer \a fp, with prefix
 * string \a prefix, indentation \a indent and floating-point format \a fmt.
 *
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 */
Gan_Bool
 gan_ltmat44T_fprint ( FILE *fp, const Gan_SquMatrix44 *A,
                       const char *prefix, int indent, const char *fmt )
{
   int i;

   assert ( A->type == GAN_LOWER_TRI_MATRIX44 );

   /* print indentation on first line */
   for ( i = indent-1; i >= 0; i-- ) fprintf ( fp, " " );
   fprintf ( fp, "%s\n", prefix );
   for ( i = indent-1; i >= 0; i-- ) fprintf ( fp, " " );
   fprintf ( fp, " " ); fprintf ( fp, fmt, A->xx );
   fprintf ( fp, " " ); fprintf ( fp, fmt, A->yx );
   fprintf ( fp, " " ); fprintf ( fp, fmt, A->zx );
   fprintf ( fp, " " ); fprintf ( fp, fmt, A->wx );
   fprintf ( fp, "\n" );
   for ( i = indent-1; i >= 0; i-- ) fprintf ( fp, " " );
   fprintf ( fp, " " ); fprintf ( fp, fmt, 0.0 );
   fprintf ( fp, " " ); fprintf ( fp, fmt, A->yy );
   fprintf ( fp, " " ); fprintf ( fp, fmt, A->zy );
   fprintf ( fp, " " ); fprintf ( fp, fmt, A->wy );
   fprintf ( fp, "\n" );
   for ( i = indent-1; i >= 0; i-- ) fprintf ( fp, " " );
   fprintf ( fp, " " ); fprintf ( fp, fmt, 0.0 );
   fprintf ( fp, " " ); fprintf ( fp, fmt, 0.0 );
   fprintf ( fp, " " ); fprintf ( fp, fmt, A->zz );
   fprintf ( fp, " " ); fprintf ( fp, fmt, A->wz );
   fprintf ( fp, "\n" );
   for ( i = indent-1; i >= 0; i-- ) fprintf ( fp, " " );
   fprintf ( fp, " " ); fprintf ( fp, fmt, 0.0 );
   fprintf ( fp, " " ); fprintf ( fp, fmt, 0.0 );
   fprintf ( fp, " " ); fprintf ( fp, fmt, 0.0 );
   fprintf ( fp, " " ); fprintf ( fp, fmt, A->ww );
   fprintf ( fp, "\n" );

   /* success */
   return GAN_TRUE;
}

/**
 * \brief Read 4x4 upper triangular matrix from file.
 *
 * Read 4x4 upper triangular matrix \a A from file pointer \a fp. The prefix
 * string for the matrix is read from the file into the \a prefix string, up
 * to the maximum length \a prefix_len of the \a prefix string. Any remaining
 * characters after \a prefix has been filled are ignored. Pass \c NULL for
 * \a prefix and zero for \a prefix_len to ignore the prefix string.
 *
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 */
Gan_Bool
 gan_ltmat44T_fscanf ( FILE *fp, Gan_SquMatrix44 *A,
                       char *prefix, int prefix_len )
{
   int ch, result;
   double dum[6];

   /* read indentation */
   for(;;)
      if ( (ch = getc(fp)) != ' ' || ch == EOF )
         break;

   /* end of file means corrupted file */
   if ( ch == EOF )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_ltmat44T_fscanf", GAN_ERROR_CORRUPTED_FILE, "" );
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

   gan_err_test_bool ( ch == '\n', "gan_ltmat44T_fscanf",
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

      gan_err_test_bool ( ch == '\n', "gan_ltmat44T_fscanf",
                          GAN_ERROR_CORRUPTED_FILE, "" );
   }

   /* read rows of matrix */
   result = 0;
   result += fscanf ( fp, "\n%lf%lf%lf%lf\n", &A->xx, &A->yx, &A->zx, &A->wx );
   result += fscanf ( fp, "%lf%lf%lf%lf\n",  &dum[0], &A->yy, &A->zy, &A->wy );
   result += fscanf ( fp, "%lf%lf%lf%lf\n", &dum[1], &dum[2], &A->zz, &A->wz );
   result += fscanf ( fp, "%lf%lf%lf%lf",  &dum[3], &dum[4], &dum[5], &A->ww );
   gan_err_test_bool ( result == 16, "gan_ltmat44T_fscanf",
                       GAN_ERROR_CORRUPTED_FILE, "" );

   /* read end-of-line character */
   result = getc(fp);
   gan_err_test_bool ( result == '\n', "gan_ltmat44T_fscanf",
                       GAN_ERROR_CORRUPTED_FILE, "" );

   /* check that matrix is indeed upper triangular */
   gan_err_test_bool ( dum[0] == 0.0 && dum[1] == 0.0 && dum[2] == 0.0 &&
                       dum[3] == 0.0 && dum[4] == 0.0 && dum[5] == 0.0,
                       "gan_ltmat44T_fscanf", GAN_ERROR_CORRUPTED_FILE, "" );

   /* success */
   gan_eval ( A->type = GAN_LOWER_TRI_MATRIX44 );
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
 * \brief Return 4x4 lower triangular matrix filled with values.
 *
 * Return 4x4 lower triangular matrix filled with values
 * \f[
 *   \left(\begin{array}{cccc} XX & 0 & 0 & 0 \\ YX & YY & 0 & 0 \\
 *                              ZX & ZY & ZZ & 0 \\ WX & WY & WZ & WW
 *     \end{array}\right)
 * \f]
 */
Gan_SquMatrix44
 gan_ltmat44_fill_s ( double XX,
                      double YX, double YY,
                      double ZX, double ZY, double ZZ,
                      double WX, double WY, double WZ, double WW )
{
   Gan_SquMatrix44 A;

   (void)gan_ltmat44_fill_q ( &A, XX, YX, YY, ZX, ZY, ZZ, WX, WY, WZ, WW );
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
 * \brief Extract parts of 4x4 lower triangular matrix.
 *
 * Extract top-left 3x3 part, bottom-left 1x3 part and bottom-right element
 * of 4x4 lower triangular matrix \a A into 3x3 lower triangular matrix \a B,
 * row 3-vector \a p and scalar pointer \a s. The arrangement is
 * \f[
 *     A = \left(\begin{array}{cc} B & 0 \\ p^{\top} & s \end{array}\right)
 * \f]
 * If any of \a B, \a p or \a s are \c NULL, the corresponding data is
 * not referenced.
 */
void
 gan_ltmat44_get_parts_s ( const Gan_SquMatrix44 *A,
                           Gan_SquMatrix33 *B, Gan_Vector3 *p, double *s )
{
   Gan_SquMatrix33 Bp;
   Gan_Vector3     pp;
   double        sp;

   gan_ltmat44_get_parts_q ( A, (B == NULL) ? &Bp : B, (p == NULL) ? &pp : p,
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
 * \brief Build a 4x4 lower triangular matrix from parts.
 *
 * Build 4x4 lower triangular matrix from 3x3 lower triangular matrix
 * \a B, row 3-vector \a p, and scalar \a s, which are respectively inserted
 * in the top-left, bottom-left and bottom-right parts of the 4x4 matrix.
 * The arrangement of the matrix is
 * \f[
 *    \left(\begin{array}{cc} B & 0 \\ p^{\top} & s \end{array}\right)
 * \f]
 * If \a p is passed as \c NULL, the corresponding part of the result is
 * filled with zeros.
 */
Gan_SquMatrix44
 gan_ltmat44_set_parts_s ( const Gan_SquMatrix33 *B, const Gan_Vector3 *p, double s )
{
   Gan_SquMatrix44 A;
   Gan_Vector3 zerov = { 0.0, 0.0, 0.0 };

   (void)gan_ltmat44_set_parts_q ( &A, B, (p == NULL) ? &zerov : p, s );
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
 * \brief Compute inverse of lower triangular 4x4 matrix.
 *
 * Compute inverse \f$ A^{-1} \f$ of lower triangular 4x4 matrix \a A,
 * writing the result into matrix \a B.
 *
 * \return Pointer to \a B on success, \c NULL on failure.
 */
Gan_SquMatrix44 *
 gan_ltmat44_invert ( Gan_SquMatrix44 *A, Gan_SquMatrix44 *B,
                      int *error_code )
{
   gan_err_test_ptr ( A->type == GAN_LOWER_TRI_MATRIX44,
                      "gan_ltmat44_invert", GAN_ERROR_ILLEGAL_ARGUMENT, "" );
   if ( A->xx == 0.0 || A->yy == 0.0 || A->zz == 0.0 || A->ww == 0.0 )
   {
      if ( error_code == NULL )
      {
         gan_err_flush_trace();
         gan_err_register ( "gan_ltmat44_invert", GAN_ERROR_SINGULAR_MATRIX,
                            "" );
      }
      else
         *error_code = GAN_ERROR_SINGULAR_MATRIX;

      return NULL;
   }

   B->xx = 1.0/A->xx;
   B->yx = -A->yx*B->xx/A->yy;
   B->yy = 1.0/A->yy;
   B->zx = -(A->zx*B->xx + A->zy*B->yx)/A->zz;
   B->zy = -A->zy*B->yy/A->zz;
   B->zz = 1.0/A->zz;
   B->wx = -(A->wx*B->xx + A->wy*B->yx + A->wz*B->zx)/A->ww;
   B->wy = -(A->wy*B->yy + A->wz*B->zy)/A->ww;
   B->wz = -A->wz*B->zz/A->ww;
   B->ww = 1.0/A->ww;
   gan_eval ( B->type = GAN_LOWER_TRI_MATRIX44 );
   return B;
}

/**
 * \}
 */

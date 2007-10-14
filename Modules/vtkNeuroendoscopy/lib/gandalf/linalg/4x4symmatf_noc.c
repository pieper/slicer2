/**
 * File:          $RCSfile: 4x4symmatf_noc.c,v $
 * Module:        4x4 symmetric matrices (single precision)
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
 * \brief Print 4x4 symmetric matrix to file
 *
 * Print 4x4 symmetric matrix \a A to file pointer \a fp, with prefix
 * string \a prefix, indentation \a indent and floating-point format \a fmt.
 *
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 */
Gan_Bool
 gan_symmat44f_fprint ( FILE *fp, const Gan_SquMatrix44_f *A,
                        const char *prefix, int indent, const char *fmt )
{
   int i;

   assert ( A->type == GAN_SYMMETRIC_MATRIX44_F );

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
   fprintf ( fp, " " ); fprintf ( fp, fmt, A->yx );
   fprintf ( fp, " " ); fprintf ( fp, fmt, A->yy );
   fprintf ( fp, " " ); fprintf ( fp, fmt, A->zy );
   fprintf ( fp, " " ); fprintf ( fp, fmt, A->wy );
   fprintf ( fp, "\n" );
   for ( i = indent-1; i >= 0; i-- ) fprintf ( fp, " " );
   fprintf ( fp, " " ); fprintf ( fp, fmt, A->zx );
   fprintf ( fp, " " ); fprintf ( fp, fmt, A->zy );
   fprintf ( fp, " " ); fprintf ( fp, fmt, A->zz );
   fprintf ( fp, " " ); fprintf ( fp, fmt, A->wz );
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
 * \brief Read 4x4 symmetric matrix from file.
 *
 * Read 4x4 symmetric matrix \a A from file pointer \a fp. The prefix
 * string for the matrix is read from the file into the \a prefix string, up
 * to the maximum length \a prefix_len of the \a prefix string. Any remaining
 * characters after \a prefix has been filled are ignored. Pass \c NULL for
 * \a prefix and zero for \a prefix_len to ignore the prefix string.
 *
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 */
Gan_Bool
 gan_symmat44f_fscanf ( FILE *fp, Gan_SquMatrix44_f *A,
                        char *prefix, int prefix_len )
{
   int ch, result;
   float dum1, dum2, dum3, dum4, dum5, dum6;

   /* read indentation */
   for(;;)
      if ( (ch = getc(fp)) != ' ' || ch == EOF )
         break;

   /* end of file means corrupted file */
   if ( ch == EOF )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_symmat44f_fscanf", GAN_ERROR_CORRUPTED_FILE, "");
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

   gan_err_test_bool ( ch == '\n', "gan_symmat44f_fscanf",
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

      gan_err_test_bool ( ch == '\n', "gan_symmat44f_fscanf",
                          GAN_ERROR_CORRUPTED_FILE, "" );
   }

   /* read rows of matrix */
   result = 0;
   result += fscanf ( fp, "\n%f%f%f%f\n", &A->xx, &dum1, &dum2, &dum3 );
   result += fscanf ( fp, "%f%f%f%f\n",   &A->yx, &A->yy, &dum4, &dum5 );
   result += fscanf ( fp, "%f%f%f%f\n",   &A->zx, &A->zy, &A->zz, &dum6 );
   result += fscanf ( fp, "%f%f%f%f",     &A->wx, &A->wy, &A->wz, &A->ww );
   gan_err_test_bool ( result == 16, "gan_symmat44f_fscanf",
                       GAN_ERROR_CORRUPTED_FILE, "" );

   /* read end-of-line character */
   result = getc(fp);
   gan_err_test_bool ( result == '\n', "gan_symmat44f_fscanf",
                       GAN_ERROR_CORRUPTED_FILE, "" );

   /* check that matrix is indeed symmetric */
   gan_err_test_bool ( dum1 == A->yx && dum2 == A->zx && dum3 == A->wx &&
                       dum4 == A->zy && dum5 == A->wy && dum6 == A->wz,
                       "gan_symmat44f_fscanf", GAN_ERROR_CORRUPTED_FILE, "" );

   /* success */
   gan_eval ( A->type = GAN_SYMMETRIC_MATRIX44_F );
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
 * \brief Return 4x4 symmetric matrix filled with values.
 *
 * Return 4x4 symmetric matrix filled with values:
 * \f[
 *  \left(\begin{array}{cccc} XX & YX & ZX & WX \\ YX & YY & ZY & WY \\
 *                            ZX & ZY & ZZ & WZ \\ WX & WY & WZ & WW
 *  \end{array}\right)
 * \f]
 */
Gan_SquMatrix44_f
 gan_symmat44f_fill_s ( float XX,
                        float YX, float YY,
                        float ZX, float ZY, float ZZ,
                        float WX, float WY, float WZ, float WW )
{
   Gan_SquMatrix44_f A;

   (void)gan_symmat44f_fill_q ( &A, XX, YX, YY, ZX, ZY, ZZ, WX, WY, WZ, WW );
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
 * \brief Compute Cholesky factorisation of symmetric 4x4 matrix.
 *
 * Compute Cholesky factorisation \f$ \mbox{chol}(A) \f$ of symmetric 4x4
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
Gan_SquMatrix44_f *
 gan_symmat44f_cholesky ( Gan_SquMatrix44_f *A, Gan_SquMatrix44_f *B,
                          int *error_code )
{
   float tmp;

   gan_err_test_ptr ( A->type == GAN_SYMMETRIC_MATRIX44_F,
                      "gan_symmat44f_cholesky_q", GAN_ERROR_ILLEGAL_ARGUMENT,
                      "" );
   if ( A->xx <= 0.0F )
   {
      if ( error_code == NULL )
      {
         gan_err_flush_trace();
         gan_err_register ( "gan_symmat44f_cholesky",
                            GAN_ERROR_NOT_POSITIVE_DEFINITE, "" );
      }
      else
         *error_code = GAN_ERROR_NOT_POSITIVE_DEFINITE;

      return NULL;
   }
   
   B->xx = (float)sqrt(A->xx);
   B->yx = A->yx/B->xx;
   tmp = A->yy - B->yx*B->yx;
   if ( tmp <= 0.0F )
   {
      if ( error_code == NULL )
      {
         gan_err_flush_trace();
         gan_err_register ( "gan_symmat44f_cholesky",
                            GAN_ERROR_NOT_POSITIVE_DEFINITE, "" );
      }
      else
         *error_code = GAN_ERROR_NOT_POSITIVE_DEFINITE;

      return NULL;
   }

   B->yy = (float)sqrt(tmp);
   B->zx = A->zx/B->xx;
   B->zy = (A->zy - B->yx*B->zx)/B->yy;
   tmp = A->zz - B->zx*B->zx - B->zy*B->zy;
   if ( tmp <= 0.0F )
   {
      if ( error_code == NULL )
      {
         gan_err_flush_trace();
         gan_err_register ( "gan_symmat44f_cholesky",
                            GAN_ERROR_NOT_POSITIVE_DEFINITE, "" );
      }
      else
         *error_code = GAN_ERROR_NOT_POSITIVE_DEFINITE;

      return NULL;
   }

   B->zz = (float)sqrt(tmp);
   B->wx = A->wx/B->xx;
   B->wy = (A->wy - B->yx*B->wx)/B->yy;
   B->wz = (A->wz - B->zx*B->wx - B->zy*B->wy)/B->zz;
   tmp = A->ww - B->wx*B->wx - B->wy*B->wy - B->wz*B->wz;
   if ( tmp <= 0.0F )
   {
      if ( error_code == NULL )
      {
         gan_err_flush_trace();
         gan_err_register ( "gan_symmat44f_cholesky",
                            GAN_ERROR_NOT_POSITIVE_DEFINITE, "" );
      }
      else
         *error_code = GAN_ERROR_NOT_POSITIVE_DEFINITE;

      return NULL;
   }

   B->ww = (float)sqrt(tmp);
   gan_eval ( B->type = GAN_LOWER_TRI_MATRIX44_F );
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
 * \brief Compute inverse of symmetric 4x4 matrix.
 *
 * Compute inverse \f$ A^{-1} \f$ of symmetric 4x4 matrix \a A, writing the
 * result into matrix \a B, so that
 * \f[
 *    A B = B A = I
 * \f]
 *
 * \a A must be non-singular.
 *
 * \return Pointer to result matrix \a B on success, \c NULL on failure.
 */
Gan_SquMatrix44_f *
 gan_symmat44f_invert ( Gan_SquMatrix44_f *A, Gan_SquMatrix44_f *B,
                        int *error_code )
{
   gan_err_test_ptr ( A->type == GAN_SYMMETRIC_MATRIX44_F,
                      "gan_symmat44f_invert", GAN_ERROR_ILLEGAL_ARGUMENT,
                      "" );

   /* perform Cholesky factorisation of A = B*B^T for lower triangular B */
   if ( gan_symmat44f_cholesky ( A, B, error_code ) == NULL )
   {
      if ( error_code == NULL )
         gan_err_register ( "gan_symmat44f_invert", GAN_ERROR_FAILURE, "" );

      return NULL;
   }

   /* invert triangular factor B --> B^-1 */
   if ( gan_ltmat44f_invert ( B, B, error_code ) == NULL )
   {
      if ( error_code == NULL )
         gan_err_register ( "gan_symmat44f_invert", GAN_ERROR_FAILURE, "" );

      return NULL;
   }

   /* multiply inverse by its own transpose to obtain A^-1 */
   gan_ltmat44f_slmultT_i ( B );
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
 * \brief Extract parts of 4x4 symmetric matrix.
 *
 * Extract top-left 3x3 part, bottom-left 1x3 part and bottom-right element
 * of 4x4 symmetric matrix \a A into 3x3 lower triangular matrix \a B,
 * row 3-vector \a p and scalar pointer \a s. The arrangement is
 * \f[
 *     A = \left(\begin{array}{cc} B & p \\ p^{\top} & s \end{array}\right)
 * \f]
 * If any of \a B, \a p or \a s are \c NULL, the corresponding data is
 * not referenced.
 *
 * \return No value.
 */
void
 gan_symmat44f_get_parts_s ( const Gan_SquMatrix44_f *A,
                             Gan_SquMatrix33_f *B, Gan_Vector3_f *p,
                             float *s )
{
   Gan_SquMatrix33_f Bp;
   Gan_Vector3_f pp;
   float sp;

   gan_symmat44f_get_parts_q ( A, (B == NULL) ? &Bp : B, (p == NULL) ? &pp : p,
                              (s == NULL) ? &sp : s );
}

/**
 * \brief Extract 2x2 block parts of 4x4 symmetric matrix.
 *
 * Extract top-left 2x2, top-right 2x2 (or transposed bottom-left) and
 * bottom-right 2x2 blocks of 4x4 matrix \a A into 2x2 symmetric
 * matrix \a B, 2x2 generic matrix \a C and 2x2 symmetric matrix \a D.
 * If any of \a B, \a C or \a D are \c NULL, the corresponding data is
 * not referenced.
 * \f[
 *   A = \left(\begin{array}{cc} B & C \\ C^{\top} & D \end{array}\right)
 * \f]
 *
 * \return No value.
 */
void
 gan_symmat44f_get_blocks_s ( const Gan_SquMatrix44_f *A,
                              Gan_SquMatrix22_f *B, Gan_Matrix22_f *C,
                              Gan_SquMatrix22_f *D )
{
   Gan_SquMatrix22_f S;
   Gan_Matrix22_f M;

   gan_symmat44f_get_blocks_q ( A, (B == NULL) ? &S : B, (C == NULL) ? &M : C,
                                   (D == NULL) ? &S : D );
}

/**
 * \}
 */

/**
 * \addtogroup FixedSizeMatrixBuild
 * \{
 */

/**
 * \brief Build a 4x4 symmetric matrix from parts.
 *
 * Build 4x4 symmetric matrix from 3x3 symmetric matrix
 * \a B, 3-vector \a p, and scalar \a s, which are respectively inserted
 * in the top-left, bottom-left/top-right and bottom-right parts of the 4x4
 * matrix. The arrangement of the matrix is
 * \f[
 *    \left(\begin{array}{cc} B & p \\ p^{\top} & s \end{array}\right)
 * \f]
 * If \a p is passed as \c NULL, the corresponding part of the result is
 * filled with zeros.
 *
 * \return Filled matrix.
 */
Gan_SquMatrix44_f
 gan_symmat44f_set_parts_s ( const Gan_SquMatrix33_f *B, const Gan_Vector3_f *p, float s )
{
   Gan_SquMatrix44_f A;
   Gan_Vector3_f zerov = { 0.0F, 0.0F, 0.0F };

   (void)gan_symmat44f_set_parts_q ( &A, B, (p == NULL) ? &zerov : p, s );
   return A;
}

/**
 * \brief Build a 4x4 symmetric matrix from 2x2 blocks.
 *
 * Build 4x4 symmetric matrix from 2x2 symmetric matrix \a B, generic
 * 2x2 matrix \a C and 2x2 symmetric matrix \a D, which are respectively
 * inserted in the top-left, top-right (or transposed bottom-left) and
 * bottom-right parts of the result 4x4 symmetric matrix. The arrangement is
 * \f[
 *   \left(\begin{array}{cc} B & C \\ C^{\top} & D \end{array}\right)
 * \f]
 *
 * If any of \a B, \a C or \a D are passed as \c NULL, the corresponding
 * parts of the result are filled with zeros.
 *
 * \return Filled 4x4 symmetric matrix.
 */
Gan_SquMatrix44_f
 gan_symmat44f_set_blocks_s ( const Gan_SquMatrix22_f *B, const Gan_Matrix22_f *C,
                              const Gan_SquMatrix22_f *D )
{
   Gan_SquMatrix44_f A;
   Gan_SquMatrix22_f sm22zero;
   Gan_Matrix22_f m22zero = {0.0F, 0.0F, 0.0F, 0.0F};

   (void)gan_symmat22f_zero_q(&sm22zero);
   (void)gan_symmat44f_set_blocks_q ( &A, (B == NULL) ? &sm22zero : B,
                                          (C == NULL) ? &m22zero : C,
                                          (D == NULL) ? &sm22zero : D );
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
 * \brief Convert symmetric 4x4 matrix to generic 4x4 matrix.
 *
 * Convert symmetric 4x4 matrix \a A to generic 4x4 matrix \a B.
 *
 * \return Pointer to result matrix \a B.
 */
Gan_Matrix44_f *
 gan_symmat44f_to_mat44f_q ( const Gan_SquMatrix44_f *A, Gan_Matrix44_f *B )
{
   gan_err_test_ptr ( A->type == GAN_SYMMETRIC_MATRIX44_F,
                      "gan_symmat44f_to_mat44f_q", GAN_ERROR_ILLEGAL_TYPE,
                      "" );
   B->xx = A->xx;
   B->xy = B->yx = A->yx; B->yy = A->yy;
   B->xz = B->zx = A->zx; B->yz = B->zy = A->zy; B->zz = A->zz;
   B->xw = B->wx = A->wx; B->yw = B->wy = A->wy; B->zw = B->wz = A->wz;
                                                 B->ww = A->ww;
   return B;
}

/**
 * \brief Convert symmetric 4x4 matrix to generic 4x4 matrix.
 *
 * Convert symmetric 4x4 matrix \a A to generic 4x4 matrix.
 *
 * \return Result matrix.
 */
Gan_Matrix44_f
 gan_symmat44f_to_mat44f_s ( const Gan_SquMatrix44_f *A )
{
   Gan_Matrix44_f B;

   gan_symmat44f_to_mat44f_q ( A, &B );
   return B;
}

/**
 * \}
 */

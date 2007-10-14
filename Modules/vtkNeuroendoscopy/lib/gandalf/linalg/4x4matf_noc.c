/**
 * File:          $RCSfile: 4x4matf_noc.c,v $
 * Module:        4x4 matrices (single precision)
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
 * \brief Print 4x4 matrix to file
 *
 * Print 4x4 matrix \a A to file pointer \a fp, with prefix string \a prefix,
 * indentation \a indent and floating-point format \a fmt.
 *
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 */
Gan_Bool
 gan_mat44f_fprint ( FILE *fp, const Gan_Matrix44_f *A,
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
   for ( i = indent-1; i >= 0; i-- ) fprintf ( fp, " " );
   fprintf ( fp, " " ); fprintf ( fp, fmt, A->zx );
   fprintf ( fp, " " ); fprintf ( fp, fmt, A->zy );
   fprintf ( fp, " " ); fprintf ( fp, fmt, A->zz );
   fprintf ( fp, " " ); fprintf ( fp, fmt, A->zw );
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
 * \brief Read 4x4 matrix from file.
 *
 * Read 4x4 matrix \a A from file pointer \a fp. The prefix
 * string for the matrix is read from the file into the \a prefix string, up
 * to the maximum length \a prefix_len of the \a prefix string. Any remaining
 * characters after \a prefix has been filled are ignored. Pass \c NULL for
 * \a prefix and zero for \a prefix_len to ignore the prefix string.
 *
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 */
Gan_Bool
 gan_mat44f_fscanf ( FILE *fp, Gan_Matrix44_f *A, char *prefix, int prefix_len)
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
      gan_err_register ( "gan_mat44f_fscanf", GAN_ERROR_CORRUPTED_FILE, "" );
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

   gan_err_test_bool ( ch == '\n', "gan_mat44f_fscanf",
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

      gan_err_test_bool ( ch == '\n', "gan_mat44f_fscanf",
                          GAN_ERROR_CORRUPTED_FILE, "" );
   }

   /* read rows of matrix */
   result = 0;
   result += fscanf ( fp, "\n%f%f%f%f\n", &A->xx, &A->xy, &A->xz, &A->xw );
   result += fscanf ( fp, "%f%f%f%f\n",   &A->yx, &A->yy, &A->yz, &A->yw );
   result += fscanf ( fp, "%f%f%f%f\n",   &A->zx, &A->zy, &A->zz, &A->zw );
   result += fscanf ( fp, "%f%f%f%f",     &A->wx, &A->wy, &A->wz, &A->ww );
   gan_err_test_bool ( result == 16, "gan_mat44f_fscanf",
                       GAN_ERROR_CORRUPTED_FILE, "" );

   /* read end-of-line character */
   result = getc(fp);
   gan_err_test_bool ( result == '\n', "gan_mat44f_fscanf",
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
 * \brief Fill 4x4 matrix with values.
 *
 * Fill 4x4 matrix with values:
 * \f[
 *     \left(\begin{array}{cccc} XX & XY & XZ & XW \\ YX & YY & YZ & YW \\
 *                               ZX & ZY & ZZ & ZW \\ WX & WY & WZ & WW
 *     \end{array}\right)
 * \f]
 *
 * \return Filled matrix.
 */
Gan_Matrix44_f
 gan_mat44f_fill_s ( float XX, float XY, float XZ, float XW,
                     float YX, float YY, float YZ, float YW,
                     float ZX, float ZY, float ZZ, float ZW,
                     float WX, float WY, float WZ, float WW )
{
   Gan_Matrix44_f A;

   (void)gan_mat44f_fill_q ( &A, XX, XY, XZ, XW, YX, YY, YZ, YW,
                                 ZX, ZY, ZZ, ZW, WX, WY, WZ, WW );
   return A;
}

/**
 * \}
 */

/**
 * \addtogroup FixedSizeMatrixTranspose
 * \{
 */

/**
 * \brief Transpose 4x4 matrix in-place.
 *
 * Transpose 4x4 matrix \a A in-place: \f$ A \leftarrow A^{\top} \f$.
 *
 * \return Pointer to transpose matrix \a A.
 */
Gan_Matrix44_f *
 gan_mat44f_tpose_i ( Gan_Matrix44_f *A )
{
   float tmp;

   tmp = A->xy; A->xy = A->yx; A->yx = tmp;
   tmp = A->xz; A->xz = A->zx; A->zx = tmp;
   tmp = A->yz; A->yz = A->zy; A->zy = tmp;
   tmp = A->wx; A->wx = A->xw; A->xw = tmp;
   tmp = A->wy; A->wy = A->yw; A->yw = tmp;
   tmp = A->wz; A->wz = A->zw; A->zw = tmp;
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
 * \brief Multiply 4x4 matrix by homogeneously constructed 4-vector.
 *
 * Multiply 4x4 matrix \a A by 4-vector constructed from 3-vector \a p by
 * adding \a h for the homogeneous 4th coordinate. The result is divided by
 * its 4th coordinate to yield a non-homogeneous 3-vector result \a q.
 * So in summary we have
 * \f[
 *    \left(\begin{array}{c} q \\ 1 \end{array}\right)
 *    = \lambda A \left(\begin{array}{c} p \\ h \end{array}\right)
 * \f]
 * from which \f$ \lambda \f$ is eliminated, yielding \a q.
 *
 * \return Pointer to result 3-vector \a q.
 */
Gan_Vector3_f *
 gan_mat44f_multv3h_q ( const Gan_Matrix44_f *A, const Gan_Vector3_f *p, float h,
                        Gan_Vector3_f *q )
{
   Gan_Vector4_f p4;

   p4 = gan_vec4f_set_parts_s ( p, h );
   p4 = gan_mat44f_multv4_s ( A, &p4 );
   gan_err_test_ptr ( p4.w != 0.0F, "gan_mat44f_multv3h_q",
                      GAN_ERROR_DIVISION_BY_ZERO, "" );
   gan_vec3f_fill_q ( q, h*p4.x/p4.w, h*p4.y/p4.w, h*p4.z/p4.w );
   return q;
}

/**
 * \brief Multiply 4x4 matrix by homogeneously constructed 4-vector.
 *
 * Multiply 4x4 matrix \a A by 4-vector constructed from 3-vector \a p by
 * adding \a h for the homogeneous 4th coordinate. The result is divided by
 * its 4th coordinate to yield a non-homogeneous 3-vector result.
 * So in summary we have
 * \f[
 *    \left(\begin{array}{c} q \\ 1 \end{array}\right)
 *    = \lambda A \left(\begin{array}{c} p \\ h \end{array}\right)
 * \f]
 * from which \f$ \lambda \f$ is eliminated, yielding \a q.
 *
 * \return Result 3-vector \a q.
 */
Gan_Vector3_f
 gan_mat44f_multv3h_s ( const Gan_Matrix44_f *A, const Gan_Vector3_f *p, float h )
{
   Gan_Vector3_f q;

   if ( gan_mat44f_multv3h_q ( A, p, h, &q ) == NULL )
      assert(0);

   return q;
}

/**
 * \}
 */

#if 0
/* LU decomposition of 4x4 general matrix */
static void
 gan_mat44f_LUdecomp_q ( const Gan_Matrix44_f *A,
                         Gan_SquMatrix44_f *L, Gan_SquMatrix44_f *U )
{
#ifndef NDEBUG
   L->type = GAN_LOWER_TRI_MATRIX44_F;
   U->type = GAN_LOWER_TRI_MATRIX44_F;
#endif /* #ifndef NDEBUG */

   /* U is a unit triangular matrix */
   U->xx = U->yy = U->zz = U->ww = 1.0;

   L->xx = A->xx;
   L->yx = A->yx;
   L->zx = A->zx;
   L->wx = A->wx;
   gan_assert ( L->xx != 0.0F, "bad matrix in gan_mat44f_LUdecomp_q()" );
   U->yx = A->xy/L->xx;
   U->zx = A->xz/L->xx;
   U->wx = A->xw/L->xx;
   L->yy = A->yy - L->yx*U->yx;
   gan_assert ( L->yy != 0.0F, "bad matrix in gan_mat44f_LUdecomp_q()" );
   U->zy = (A->yz - L->yx*U->zx)/L->yy;
   U->wy = (A->yw - L->yx*U->wx)/L->yy;
   L->zy = A->zy - L->zx*U->yx;
   L->zz = A->zz - L->zx*U->zx - L->zy*U->zy;
   gan_assert ( L->zz != 0.0F, "bad matrix in gan_mat44f_LUdecomp_q()" );
   U->wz = (A->zw - L->zx*U->wx - L->zy*U->wy)/L->zz;
   L->wy = A->wy - L->wx*U->yx;
   L->wz = A->wz - L->wx*U->zx - L->wy*U->zy;
   L->ww = A->ww - L->wx*U->wx - L->wy*U->wy - L->wz*U->wz;
}

/* right-multiply lower triangular matrix by upper triangular matrix */
static void
 gan_ltmat44f_rmultl44T_q ( const Gan_SquMatrix44_f *L, Gan_SquMatrix44_f *U,
                            Gan_Matrix44_f *A )
{
#ifndef NDEBUG
   assert ( L->type == GAN_LOWER_TRI_MATRIX44_F &&
            U->type == GAN_LOWER_TRI_MATRIX44_F );
#endif /* #ifndef NDEBUG */

   A->xx = L->xx*U->xx;
   A->yx = L->yx*U->xx;
   A->zx = L->zx*U->xx;
   A->wx = L->wx*U->xx;
   A->xy = L->xx*U->yx;
   A->xz = L->xx*U->zx;
   A->xw = L->xx*U->wx;
   A->yy = L->yx*U->yx + L->yy*U->yy;
   A->yz = L->yx*U->zx + L->yy*U->zy;
   A->yw = L->yx*U->wx + L->yy*U->wy;
   A->zy = L->zx*U->yx + L->zy*U->yy;
   A->wy = L->wx*U->yx + L->wy*U->yy;
   A->zz = L->zx*U->zx + L->zy*U->zy + L->zz*U->zz;
   A->zw = L->zx*U->wx + L->zy*U->wy + L->zz*U->wz;
   A->wz = L->wx*U->zx + L->wy*U->zy + L->wz*U->zz;
   A->ww = L->wx*U->wx + L->wy*U->wy + L->wz*U->wz + L->ww*U->ww;
}

/* left-multiply lower triangular matrix by upper triangular matrix */
static void
 gan_ltmat44f_lmultl44T_q ( const Gan_SquMatrix44_f *L, Gan_SquMatrix44_f *U,
                            Gan_Matrix44_f *A )
{
#ifndef NDEBUG
   assert ( L->type == GAN_LOWER_TRI_MATRIX44_F &&
            U->type == GAN_LOWER_TRI_MATRIX44_F );
#endif /* #ifndef NDEBUG */

   A->xx = U->xx*L->xx + U->yx*L->yx + U->zx*L->zx + U->wx*L->wx;
   A->yy = U->yy*L->yy + U->zy*L->zy + U->wy*L->wy;
   A->xy = U->yx*L->yy + U->zx*L->zy + U->wx*L->wy;
   A->yx = U->yy*L->yx + U->zy*L->zx + U->wy*L->wx;
   A->zz = U->zz*L->zz + U->wz*L->wz;
   A->yz = U->zy*L->zz + U->wy*L->wz;
   A->xz = U->zx*L->zz + U->wx*L->wz;
   A->zy = U->zz*L->zy + U->wz*L->wy;
   A->zx = U->zz*L->zx + U->wz*L->wx;
   A->ww = U->ww*L->ww;
   A->zw = U->wz*L->ww;
   A->yw = U->wy*L->ww;
   A->xw = U->wx*L->ww;
   A->wz = U->ww*L->wz;
   A->wy = U->ww*L->wy;
   A->wx = U->ww*L->wx;
}
#endif

/**
 * \addtogroup FixedSizeMatrixInvert
 * \{
 */

/**
 * \brief Invert 4x4 matrix.
 *
 * Compute inverse \f$ A^{-1} \f$ of 4x4 matrix \a A, writing the result into
 * matrix \a B, so that \f$ A B = I \f$.
 *
 * \return Result matrix \a B on success, \c NULL on error.
 */
Gan_Matrix44_f *
 gan_mat44f_invert ( Gan_Matrix44_f *A, Gan_Matrix44_f *B, int *error_code )
{
   float detA = gan_mat44f_det_q(A), detE, detF, detG, detH;
   Gan_Matrix22_f E, F, G, H, EaT, FaT, GaT, HaT, EaTF, GaTH, tmp;

   gan_err_test_ptr ( A != B,
                      "gan_mat44f_invert_q", GAN_ERROR_ILLEGAL_ARGUMENT,
                      "can't do inverse in place" );
   if ( detA == 0.0 )
   {
      if ( error_code == NULL )
      {
         gan_err_flush_trace();
         gan_err_register ( "gan_mat44f_invert_q", GAN_ERROR_SINGULAR_MATRIX,
                            "" );
      }
      else
         *error_code = GAN_ERROR_SINGULAR_MATRIX;

      return NULL;
   }
      
   /* get 2x2 blocks of A */
   gan_mat44f_get_blocks_q ( A, &E, &F, &G, &H );

   /* compute determinant and adjoint transpose of each 2x2 block */
   detE = gan_mat22f_det_q(&E); gan_mat22f_adjointT_q ( &E, &EaT );
   detF = gan_mat22f_det_q(&F); gan_mat22f_adjointT_q ( &F, &FaT );
   detG = gan_mat22f_det_q(&G); gan_mat22f_adjointT_q ( &G, &GaT );
   detH = gan_mat22f_det_q(&H); gan_mat22f_adjointT_q ( &H, &HaT );

   /* compute E^*^T * F and G^*^T * H */
   (void)gan_mat22f_rmultm22_q ( &EaT, &F, &EaTF );
   (void)gan_mat22f_rmultm22_q ( &GaT, &H, &GaTH );

   /* compute top-left inverse block multiplied by det(A) */
   (void)gan_mat22f_scale_q ( &EaT, detH, &E );
   (void)gan_mat22f_rmultm22_q ( &GaTH, &FaT, &tmp );
   (void)gan_mat22f_decrement ( &E, &tmp );

   /* compute top-right inverse block multiplied by det(A) */
   (void)gan_mat22f_scale_q ( &GaT, detF, &F );
   (void)gan_mat22f_rmultm22_q ( &EaTF, &HaT, &tmp );
   (void)gan_mat22f_decrement ( &F, &tmp );

   /* compute bottom-left inverse block multiplied by det(A) */
   GaTH = gan_mat22f_adjointT_s ( &GaTH );
   (void)gan_mat22f_scale_q ( &FaT, detG, &G );
   (void)gan_mat22f_rmultm22_q ( &GaTH, &EaT, &tmp );
   (void)gan_mat22f_decrement ( &G, &tmp );

   /* compute bottom-right inverse block multiplied by det(A) */
   EaTF = gan_mat22f_adjointT_s ( &EaTF );
   (void)gan_mat22f_scale_q ( &HaT, detE, &H );
   (void)gan_mat22f_rmultm22_q ( &EaTF, &GaT, &tmp );
   (void)gan_mat22f_decrement ( &H, &tmp );

   /* build matrix and divide by det(A) to compete calculation of inverse */
   (void)gan_mat44f_set_blocks_q ( B, &E, &F, &G, &H );
   (void)gan_mat44f_divide_i ( B, detA );
#if 0
   {
      Gan_SquMatrix44_f L, U;
      Gan_Matrix44_f I;

      gan_mat44f_rmultm44_q ( A, B, &I );
      gan_mat44f_LUdecomp_q ( A, &L, &U );
      gan_ltmat44f_invert_i ( &L );
      gan_ltmat44f_invert_i ( &U );
      gan_ltmat44f_lmultl44T_q ( &L, &U, B );
      gan_mat44f_rmultm44_q ( A, B, &I );
   }
#endif
   return B;
}

/**
 * \}
 */

/**
 * \addtogroup FixedSizeMatrixAdjoint
 * \{
 */

/**
 * \brief Compute adjoint of 4x4 matrix.
 *
 * Compute adjoint \f$ A^{*} \f$ of 4x4 matrix \a A, writing the result into
 * matrix \a B, so that
 * \f[
 *     B A^{\top} = \mbox{det}(A) I
 * \f]
 *
 * \return Result matrix \a B on success, \c NULL on error.
 */
Gan_Matrix44_f *
 gan_mat44f_adjoint_q ( const Gan_Matrix44_f *A, Gan_Matrix44_f *B )
{
   gan_err_test_ptr ( A != B, "gan_mat44f_adjoint_q",
                      GAN_ERROR_ILLEGAL_ARGUMENT, "can't do adjoint in place");
   gan_assert ( 0, "not implemented" );
   return B;
}

/**
 * \brief Compute adjoint transpose of 4x4 matrix.
 *
 * Compute adjoint transpose \f$ A^{*{\top}} \f$ of 4x4 matrix \a A, writing
 * the result into matrix \a B, so that
 * \f[
 *    B A = \mbox{det}(A) I
 * \f]
 *
 * \return Result matrix \a B on success, \c NULL on error.
 */
Gan_Matrix44_f *
 gan_mat44f_adjointT_q ( const Gan_Matrix44_f *A, Gan_Matrix44_f *B )
{
   gan_err_test_ptr ( A != B, "gan_mat44f_adjointT_q",
                      GAN_ERROR_ILLEGAL_ARGUMENT, "can't do adjoint in place");
   gan_assert ( 0, "not implemented" );
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
 * \brief Extract rows of 4x4 matrix.
 *
 * Extract rows of 4x4 matrix \a A into 4-vectors \a p, \a q, \a r and \a s.
 * The arrangement can be written
 * \f[
 *     A = \left(\begin{array}{c} p^{\top} \\ q^{\top} \\ r^{\top} \\ s^{\top}
 *         \end{array}\right)
 * \f]
 *
 * \return No value.
 */
void
 gan_mat44f_get_rows_s ( const Gan_Matrix44_f *A, Gan_Vector4_f *p, Gan_Vector4_f *q,
                                                  Gan_Vector4_f *r, Gan_Vector4_f *s)
{
   gan_mat44f_get_rows_q ( A, p, q, r, s );
}

/**
 * \brief Extract columns of 4x4 matrix
 *
 * Extract columns of 4x4 matrix \a A into 4-vectors \a p, \a q, \a r and \a s.
 * The arrangement can be written
 * \f[
 *     A = \left(\begin{array}{cccc} p & q & r & s \end{array}\right)
 * \f]
 *
 * \return No value.
 */
void
 gan_mat44f_get_cols_s ( const Gan_Matrix44_f *A, Gan_Vector4_f *p, Gan_Vector4_f *q,
                                                  Gan_Vector4_f *r, Gan_Vector4_f *s)
{
   gan_mat44f_get_cols_q ( A, p, q, r, s );
}

/**
 * \brief Extract top-left 3x3 part of 4x4 matrix.
 *
 * Extract top-left 3x3 part of 4x4 matrix \a A.
 *
 * \return Result 3x3 matrix part.
 */
Gan_Matrix33_f
 gan_mat44f_get_m33tl_s ( const Gan_Matrix44_f *A )
{
   Gan_Matrix33_f B;

   (void)gan_mat44f_get_m33tl_q ( A, &B );
   return B;
}

/**
 * \brief Extract top part of right hand column of 4x4 matrix.
 *
 * Extract top-right column of 4x4 matrix \a A.
 *
 * \return Result 3-vector part.
 */
Gan_Vector3_f
 gan_mat44f_get_v3tr_s ( const Gan_Matrix44_f *A )
{
   Gan_Vector3_f p;

   (void)gan_mat44f_get_v3tr_q ( A, &p );
   return p;
}

/**
 * \brief Extract parts of 4x4 matrix.
 *
 * Extract top-left 3x3 part, top-right 3x1 part, bottom-left 1x3 part and
 * bottom-right element of 4x4 matrix \a A into 3x3 matrix \a B,
 * column 3-vector \a p, row 3-vector \a q and scalar pointer \a s.
 * The arrangement is
 * \f[
 *   A = \left(\begin{array}{cc} B & p \\ q^{\top} & s \end{array}\right)
 * \f]
 *
 * \return No value.
 */
void
 gan_mat44f_get_parts_s ( const Gan_Matrix44_f *A, Gan_Matrix33_f *B,
                          Gan_Vector3_f *p, Gan_Vector3_f *q, float *s )
{
   gan_mat44f_get_parts_q ( A, B, p, q, s );
}

/**
 * \brief Extract 2x2 block parts of 4x4 matrix.
 *
 * Extract top-left, top-right, bottom-left bottom-right blocks of 4x4
 * matrix \a M into 2x2 matrices \a A, \a B, \a C and \a D.
 * The arrangement is
 * \f[
 *   M = \left(\begin{array}{cc} A & B \\ C & D \end{array}\right)
 * \f]
 *
 * \return No value.
 */
void gan_mat44f_get_blocks_s ( const Gan_Matrix44_f *M,
                               Gan_Matrix22_f *A, Gan_Matrix22_f *B,
                               Gan_Matrix22_f *C, Gan_Matrix22_f *D )
{
   gan_mat44f_get_blocks_q ( M, A, B, C, D );
}

/**
 * \}
 */

/**
 * \addtogroup FixedSizeMatrixBuild
 * \{
 */

/**
 * \brief Build a 4x4 matrix from parts.
 *
 * Build 4x4 matrix from 3x3 matrix \a B, column 3-vector \a p,
 * row 3-vector \a q and scalar \a s, which are respectively inserted
 * in the top-left, top-right, bottom-left and bottom-right parts of \a A.
 * The arrangement of the matrix is
 * \f[
 *    \left(\begin{array}{cc} B & p \\ q^{\top} & s \end{array}\right)
 * \f]
 * If \a p and/or \a q are passed as \c NULL, the corresponding
 * parts of the result are filled with zeros.
 *
 * \return Filled matrix.
 */
Gan_Matrix44_f
 gan_mat44f_set_parts_s ( Gan_Matrix33_f *B,
                          const Gan_Vector3_f *p, const Gan_Vector3_f *q, float s )
{
   Gan_Matrix44_f A;
   Gan_Vector3_f zerov = { 0.0F, 0.0F, 0.0F };

   (void)gan_mat44f_set_parts_q ( &A, B, (p == NULL) ? &zerov : p,
                                 (q == NULL) ? &zerov : q, s );
   return A;
}

/**
 * \brief Build a 4x4 matrix from 2x2 blocks.
 *
 * Build 4x4 matrix from 2x2 matrix blocks \a A, \a B, \a C and \a D,
 * which are respectively inserted in the top-left, top-right, bottom-left
 * and bottom-right parts of the 4x4 matrix.
 * The arrangement is
 * \f[
 *   \left(\begin{array}{cc} A & B \\ C & D \end{array}\right)
 * \f]
 * If any of \a A, \a B, \a C or \a D are passed as \c NULL, the corresponding
 * parts of the result are filled with zeros.
 *
 * \return Filled matrix.
 */
Gan_Matrix44_f
 gan_mat44f_set_blocks_s ( const Gan_Matrix22_f *A, const Gan_Matrix22_f *B,
                           const Gan_Matrix22_f *C, const Gan_Matrix22_f *D )
{
   Gan_Matrix44_f M;
   Gan_Matrix22_f zerob = { 0.0F, 0.0F,
                            0.0F, 0.0F };

   (void)gan_mat44f_set_blocks_q ( &M,
                              (A==NULL) ? &zerob : A, (B==NULL) ? &zerob : B,
                              (C==NULL) ? &zerob : C, (D==NULL) ? &zerob : D );
   return M;
}

/**
 * \}
 */

#include <gandalf/common/misc_error.h>
#include <gandalf/linalg/matf_symmetric.h>
#include <gandalf/linalg/matf_diagonal.h>
#include <gandalf/linalg/matf_scaledI.h>
#include <gandalf/linalg/matf_triangular.h>

/**
 * \addtogroup FixedSizeMatrixConvert
 * \{
 */

/**
 * \brief Convert generic rectangular matrix to 4x4 matrix structure.
 *
 * Convert generic rectangular matrix \a A to 4x4 matrix structure \a B.
 *
 * \return Pointer to result matrix \a B.
 */
Gan_Matrix44_f *
 gan_mat44f_from_matf_q ( const Gan_Matrix_f *A, Gan_Matrix44_f *B )
{
   if ( !gan_matf_read_va ( A, 4, 4, &B->xx, &B->xy, &B->xz, &B->xw,
                                     &B->yx, &B->yy, &B->yz, &B->yw,
                                     &B->zx, &B->zy, &B->zz, &B->zw,
                                     &B->wx, &B->wy, &B->wz, &B->ww ) )
   {
      gan_err_register ( "gan_mat44f_from_matf_q", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   return B;
}

/**
 * \brief Convert symmetric matrix to generic 4x4 matrix structure.
 *
 * Convert symmetric matrix \a A to generic 4x4 matrix structure \a B.
 *
 * \return Pointer to result matrix \a B.
 */
Gan_Matrix44_f *
 gan_mat44f_from_squmatf_q ( const Gan_SquMatrix_f *A, Gan_Matrix44_f *B )
{
   switch ( A->type )
   {
      case GAN_SCALED_IDENT_MATRIX:
        if ( !gan_scalImatf_read_va ( A, 4, &B->xx ) )
        {
           gan_err_register ( "gan_mat44f_from_squmatf_q", GAN_ERROR_FAILURE,
                              "" );
           return NULL;
        }

        B->xy = B->yx = B->xz = B->zx = B->yz = B->zy = B->xw = B->wx =
        B->yw = B->wy = B->zw = B->wz = 0.0F;
        B->ww = B->zz = B->yy = B->xx;
        break;

      case GAN_DIAGONAL_MATRIX:
        if ( !gan_diagmatf_read_va ( A, 4, &B->xx, &B->yy, &B->zz, &B->ww ) )
        {
           gan_err_register ( "gan_mat44f_from_squmatf_q", GAN_ERROR_FAILURE,
                              "" );
           return NULL;
        }

        B->xy = B->yx = B->xz = B->zx = B->yz = B->zy = B->xw = B->wx =
        B->yw = B->wy = B->zw = B->wz = 0.0F;
        break;

      case GAN_SYMMETRIC_MATRIX:
        if ( !gan_symmatf_read_va ( A, 4,
                                    &B->xx, &B->yx, &B->yy, &B->zx, &B->zy,
                                    &B->zz, &B->wx, &B->wy, &B->wz, &B->ww ) )
        {
           gan_err_register ( "gan_mat44f_from_squmatf_q", GAN_ERROR_FAILURE,
                              "" );
           return NULL;
        }

        B->xy = B->yx; B->xz = B->zx; B->yz = B->zy; B->xw = B->wx;
        B->yw = B->wy; B->zw = B->wz;
        break;

      case GAN_LOWER_TRI_MATRIX:
        if ( !gan_ltmatf_read_va ( A, 4,
                                   &B->xx, &B->yx, &B->yy, &B->zx, &B->zy,
                                   &B->zz, &B->wx, &B->wy, &B->wz, &B->ww ) )
        {
           gan_err_register ( "gan_mat44f_from_squmatf_q", GAN_ERROR_FAILURE,
                              "" );
           return NULL;
        }

        B->xy = B->xz =B->yz = B->xw = B->yw = B->zw = 0.0F;
        break;

      case GAN_UPPER_TRI_MATRIX:
        if ( !gan_utmatf_read_va ( A, 4,
                                   &B->xx, &B->xy, &B->yy, &B->xz, &B->yz,
                                   &B->zz, &B->xw, &B->yw, &B->zw, &B->ww ) )
        {
           gan_err_register ( "gan_mat44f_from_squmatf_q", GAN_ERROR_FAILURE,
                              "" );
           return NULL;
        }

        B->yx = B->zx =B->zy = B->wx = B->wy = B->wz = 0.0F;
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_mat44f_from_squmatf_q", GAN_ERROR_FAILURE, "");
        return NULL;
   }

   return B;
}

/**
 * \brief Convert symmetric matrix to specific 4x4 symmetric matrix structure.
 *
 * Convert symmetric matrix \a A to specific 4x4 symmetric matrix
 * structure \a B.
 *
 * \return Pointer to result matrix \a B.
 */
Gan_SquMatrix44_f *
 gan_squmat44f_from_squmatf_q ( const Gan_SquMatrix_f *A, Gan_SquMatrix44_f *B )
{
   switch ( A->type )
   {
      case GAN_SCALED_IDENT_MATRIX:
        gan_eval(B->type = GAN_SYMMETRIC_MATRIX44_F);
        if ( !gan_scalImatf_read_va ( A, 4, &B->xx ) )
        {
           gan_err_register ( "gan_squmat44f_from_squmatf_q",
                              GAN_ERROR_FAILURE, "" );
           return NULL;
        }

        B->yx = B->zx = B->zy = B->wx = B->wy = B->wz = 0.0F;
        B->ww = B->zz = B->yy = B->xx;
        break;

      case GAN_DIAGONAL_MATRIX:
        gan_eval(B->type = GAN_SYMMETRIC_MATRIX44_F);
        if ( !gan_diagmatf_read_va ( A, 4, &B->xx, &B->yy, &B->zz, &B->ww ) )
        {
           gan_err_register ( "gan_squmat44f_from_squmatf_q",
                              GAN_ERROR_FAILURE, "" );
           return NULL;
        }

        B->yx = B->zx = B->zy = B->wx = B->wy = B->wz = 0.0F;
        break;

      case GAN_SYMMETRIC_MATRIX:
        gan_eval(B->type = GAN_SYMMETRIC_MATRIX44_F);
        if ( !gan_symmatf_read_va ( A, 4,
                                    &B->xx, &B->yx, &B->yy, &B->zx, &B->zy,
                                    &B->zz, &B->wx, &B->wy, &B->wz, &B->ww ) )
        {
           gan_err_register ( "gan_squmat44f_from_squmatf_q",
                              GAN_ERROR_FAILURE, "" );
           return NULL;
        }

        break;

      case GAN_LOWER_TRI_MATRIX:
        gan_eval(B->type = GAN_LOWER_TRI_MATRIX44_F);
        if ( !gan_ltmatf_read_va ( A, 4,
                                   &B->xx, &B->yx, &B->yy, &B->zx, &B->zy,
                                   &B->zz, &B->wx, &B->wy, &B->wz, &B->ww ) )
        {
           gan_err_register ( "gan_squmat44f_from_squmatf_q",
                              GAN_ERROR_FAILURE, "" );
           return NULL;
        }

        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_squmat44f_from_squmatf_q", GAN_ERROR_FAILURE,
                           "" );
        return NULL;
   }

   return B;
}

/**
 * \}
 */

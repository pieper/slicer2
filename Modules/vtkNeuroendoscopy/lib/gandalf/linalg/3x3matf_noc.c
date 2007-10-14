/**
 * File:          $RCSfile: 3x3matf_noc.c,v $
 * Module:        3x3 matrices (single precision)
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
 * \brief Print 3x3 matrix to file
 *
 * Print 3x3 matrix \a A to file pointer \a fp, with prefix string \a prefix,
 * indentation \a indent and floating-point format \a fmt.
 *
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 */
Gan_Bool
 gan_mat33f_fprint ( FILE *fp, const Gan_Matrix33_f *A,
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
   for ( i = indent-1; i >= 0; i-- ) fprintf ( fp, " " );
   fprintf ( fp, " " ); fprintf ( fp, fmt, A->zx );
   fprintf ( fp, " " ); fprintf ( fp, fmt, A->zy );
   fprintf ( fp, " " ); fprintf ( fp, fmt, A->zz );
   fprintf ( fp, "\n" );

   /* success */
   return GAN_TRUE;
}

/**
 * \brief Read 3x3 matrix from file.
 *
 * Read 3x3 matrix \a A from file pointer \a fp. The prefix
 * string for the matrix is read from the file into the \a prefix string, up
 * to the maximum length \a prefix_len of the \a prefix string. Any remaining
 * characters after \a prefix has been filled are ignored. Pass \c NULL for
 * \a prefix and zero for \a prefix_len to ignore the prefix string.
 *
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 */
Gan_Bool
 gan_mat33f_fscanf ( FILE *fp, Gan_Matrix33_f *A, char *prefix, int prefix_len)
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
      gan_err_register ( "gan_mat33f_fscanf", GAN_ERROR_CORRUPTED_FILE, "" );
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

   gan_err_test_bool ( ch == '\n', "gan_mat33f_fscanf",
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

      gan_err_test_bool ( ch == '\n', "gan_mat33f_fscanf",
                          GAN_ERROR_CORRUPTED_FILE, "" );
   }

   /* read rows of matrix */
   result = 0;
   result += fscanf ( fp, "\n%f%f%f\n", &A->xx, &A->xy, &A->xz );
   result += fscanf ( fp, "%f%f%f\n",   &A->yx, &A->yy, &A->yz );
   result += fscanf ( fp, "%f%f%f",     &A->zx, &A->zy, &A->zz );
   gan_err_test_bool ( result == 9, "gan_mat33f_fscanf",
                       GAN_ERROR_CORRUPTED_FILE, "" );

   /* read end-of-line character */
   result = getc(fp);
   gan_err_test_bool ( result == '\n', "gan_mat33f_fscanf",
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
 * \brief Fill 3x3 matrix with values.
 *
 * Fill 3x3 matrix with values:
 * \f[
 *     \left(\begin{array}{ccc} XX & XY & XZ \\ YX & YY & YZ \\
                                ZX & ZY & ZZ \end{array}\right)
 * \f]
 *
 * \return Filled matrix.
 */
Gan_Matrix33_f
 gan_mat33f_fill_s ( float XX, float XY, float XZ,
                     float YX, float YY, float YZ,
                     float ZX, float ZY, float ZZ )
{
   Gan_Matrix33_f A;

   (void)gan_mat33f_fill_q ( &A, XX, XY, XZ, YX, YY, YZ, ZX, ZY, ZZ );
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
 * \brief Multiply 3x3 matrix by homogeneously constructed 3-vector.
 *
 * Multiply 3x3 matrix \a A by 3-vector constructed from 2-vector \a p by
 * adding \a h for the homogeneous 3th coordinate. The result is divided by
 * its 3th coordinate to yield a non-homogeneous 2-vector result \a q.
 * So in summary we have
 * \f[
 *    \left(\begin{array}{c} q \\ 1 \end{array}\right)
 *    = \lambda A \left(\begin{array}{c} p \\ h \end{array}\right)
 * \f]
 * from which \f$ \lambda \f$ is eliminated, yielding \a q.
 *
 * \return Pointer to result 2-vector \a q.
 */
Gan_Vector2_f *
 gan_mat33f_multv2h_q ( const Gan_Matrix33_f *A, const Gan_Vector2_f *p, float h,
                        Gan_Vector2_f *q )
{
   Gan_Vector3_f p3;

   p3 = gan_vec3f_set_parts_s ( p, h );
   p3 = gan_mat33f_multv3_s ( A, &p3 );
   gan_err_test_ptr ( p3.z != 0.0F, "gan_mat33f_multv2h_q",
                      GAN_ERROR_DIVISION_BY_ZERO, "" );

   gan_vec2f_fill_q ( q, h*p3.x/p3.z, h*p3.y/p3.z );
   return q;
}

/**
 * \brief Multiply 3x3 matrix by homogeneously constructed 3-vector.
 *
 * Multiply 3x3 matrix \a A by 3-vector constructed from 2-vector \a p by
 * adding \a h for the homogeneous 3th coordinate. The result is divided by
 * its 3th coordinate to yield a non-homogeneous 2-vector result.
 * So in summary we have
 * \f[
 *    \left(\begin{array}{c} q \\ 1 \end{array}\right)
 *    = \lambda A \left(\begin{array}{c} p \\ h \end{array}\right)
 * \f]
 * from which \f$ \lambda \f$ is eliminated, yielding \a q.
 *
 * \return Result 2-vector \a q.
 */
Gan_Vector2_f
 gan_mat33f_multv2h_s ( const Gan_Matrix33_f *A, const Gan_Vector2_f *p, float h )
{
   Gan_Vector2_f q;

   if ( gan_mat33f_multv2h_q ( A, p, h, &q ) == NULL )
      assert(0);

   return q;
}

/**
 * Compute vector product matrix.
 *
 * Write matrix form of vector product of 3-vector \a p into 3x3 matrix.
 * This is written as
 * \f[
 *   A = [p]_{\times}
 * \f]
 * such that for any vector \f$ x \f$,
 * \f[
 *   A x = p \times x
 * \f]
 *
 * \return Result matrix \f$ [p]_{\times} \f$.
 */
Gan_Matrix33_f
 gan_mat33f_cross_s ( const Gan_Vector3_f *p )
{
   Gan_Matrix33_f A;

   (void)gan_mat33f_cross_q ( p, &A );
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
 * \brief Transpose 3x3 matrix in-place.
 *
 * Transpose 3x3 matrix \a A in-place: \f$ A \leftarrow A^{\top} \f$.
 *
 * \return Pointer to transpose matrix \a A.
 */
Gan_Matrix33_f *
 gan_mat33f_tpose_i ( Gan_Matrix33_f *A )
{
   float tmp;

   tmp = A->xy; A->xy = A->yx; A->yx = tmp;
   tmp = A->xz; A->xz = A->zx; A->zx = tmp;
   tmp = A->yz; A->yz = A->zy; A->zy = tmp;
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
 * \brief Invert 3x3 matrix.
 *
 * Compute inverse \f$ A^{-1} \f$ of 3x3 matrix \a A, writing the result into
 * matrix \a B, so that \f$ A B = I \f$.
 *
 * \return Result matrix \a B on success, \c NULL on error.
 */
Gan_Matrix33_f *
 gan_mat33f_invert ( Gan_Matrix33_f *A, Gan_Matrix33_f *B, int *error_code )
{
   float d1 = A->yy*A->zz-A->yz*A->zy, d2 = A->yz*A->zx-A->yx*A->zz,
         d3 = A->yx*A->zy-A->yy*A->zx;
   float det = A->xx*d1 + A->xy*d2 + A->xz*d3;

   if ( det == 0.0 )
   {
      if ( error_code == NULL )
      {
         gan_err_flush_trace();
         gan_err_register ( "gan_mat33f_invert_q", GAN_ERROR_SINGULAR_MATRIX,
                            "" );
      }
      else
         *error_code = GAN_ERROR_SINGULAR_MATRIX;

      return NULL;
   }
      
   if ( A == B )
   {
      Gan_Matrix33_f Ap = *A;

      B->xx = d1/det; B->xy = (Ap.zy*Ap.xz-Ap.zz*Ap.xy)/det;
                      B->xz = (Ap.xy*Ap.yz-Ap.xz*Ap.yy)/det;
      B->yx = d2/det; B->yy = (Ap.zz*Ap.xx-Ap.zx*Ap.xz)/det;
                      B->yz = (Ap.xz*Ap.yx-Ap.xx*Ap.yz)/det;
      B->zx = d3/det; B->zy = (Ap.zx*Ap.xy-Ap.zy*Ap.xx)/det;
                      B->zz = (Ap.xx*Ap.yy-Ap.xy*Ap.yx)/det;
   }
   else
   {
      B->xx = d1/det; B->xy = (A->zy*A->xz-A->zz*A->xy)/det;
                      B->xz = (A->xy*A->yz-A->xz*A->yy)/det;
      B->yx = d2/det; B->yy = (A->zz*A->xx-A->zx*A->xz)/det;
                      B->yz = (A->xz*A->yx-A->xx*A->yz)/det;
      B->zx = d3/det; B->zy = (A->zx*A->xy-A->zy*A->xx)/det;
                      B->zz = (A->xx*A->yy-A->xy*A->yx)/det;
   }
   
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
 * \brief Compute adjoint of 3x3 matrix.
 *
 * Compute adjoint \f$ A^{*} \f$ of 3x3 matrix \a A, writing the result into
 * matrix \a B, so that
 * \f[
 *   B A^{\top} = \mbox{det}(A) I
 * \f]
 *
 * \return Result matrix \a B on success, \c NULL on error.
 */
Gan_Matrix33_f *
 gan_mat33f_adjoint_q ( const Gan_Matrix33_f *A, Gan_Matrix33_f *B )
{
   gan_err_test_ptr ( A != B, "gan_mat33f_adjoint_q",
                      GAN_ERROR_ILLEGAL_ARGUMENT, "can't do adjoint in place");
   B->xx = A->yy*A->zz-A->yz*A->zy; B->xy = A->yz*A->zx-A->yx*A->zz;
                                    B->xz = A->yx*A->zy-A->yy*A->zx;
   B->yx = A->zy*A->xz-A->zz*A->xy; B->yy = A->zz*A->xx-A->zx*A->xz;
                                    B->yz = A->zx*A->xy-A->zy*A->xx;
   B->zx = A->xy*A->yz-A->xz*A->yy; B->zy = A->xz*A->yx-A->xx*A->yz;
                                    B->zz = A->xx*A->yy-A->xy*A->yx;
   return B;
}

/**
 * \brief Compute adjoint transpose of 3x3 matrix.
 *
 * Compute adjoint transpose \f$ A^{*{\top}} \f$ of 3x3 matrix \a A, writing
 * the result into matrix \a B, so that
 * \f[
 *   B A = \mbox{det}(A) I
 * \f]
 *
 * \return Result matrix \a B on success, \c NULL on error.
 */
Gan_Matrix33_f *
 gan_mat33f_adjointT_q ( const Gan_Matrix33_f *A, Gan_Matrix33_f *B )
{
   gan_err_test_ptr ( A != B, "gan_mat33f_adjointT_q",
                      GAN_ERROR_ILLEGAL_ARGUMENT, "can't do adjoint in place");
   B->xx = A->yy*A->zz-A->yz*A->zy; B->xy = A->zy*A->xz-A->zz*A->xy;
                                    B->xz = A->xy*A->yz-A->xz*A->yy;
   B->yx = A->yz*A->zx-A->yx*A->zz; B->yy = A->zz*A->xx-A->zx*A->xz;
                                    B->yz = A->xz*A->yx-A->xx*A->yz;
   B->zx = A->yx*A->zy-A->yy*A->zx; B->zy = A->zx*A->xy-A->zy*A->xx;
                                    B->zz = A->xx*A->yy-A->xy*A->yx;
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
 * \brief Extract rows of 3x3 matrix.
 *
 * Extract rows of 3x3 matrix \a A into 3-vectors \a p, \a q and \a r.
 * The arrangement can be written
 * \f[
 *     A = \left(\begin{array}{c} p^{\top} \\ q^{\top} \\ r^{\top}
 *         \end{array}\right)
 * \f]
 *
 * \return No value.
 */
void
 gan_mat33f_get_rows_s ( const Gan_Matrix33_f *A,
                         Gan_Vector3_f *p, Gan_Vector3_f *q, Gan_Vector3_f *r )
{
   gan_mat33f_get_rows_q ( A, p, q, r );
}

/**
 * \brief Extract columns of 3x3 matrix
 *
 * Extract columns of 3x3 matrix \a A into 3-vectors \a p, \a q and \a r.
 * The arrangement can be written
 * \f[
 *     A = \left(\begin{array}{ccc} p & q & r \end{array}\right)
 * \f]
 *
 * \return No value.
 */
void
 gan_mat33f_get_cols_s ( const Gan_Matrix33_f *A,
                         Gan_Vector3_f *p, Gan_Vector3_f *q, Gan_Vector3_f *r )
{
   gan_mat33f_get_cols_q ( A, p, q, r );
}

/**
 * \brief Extract top-left 2x2 part of 3x3 matrix.
 *
 * Extract top-left 2x2 part of 3x3 matrix \a A.
 *
 * \return Result 2x2 matrix part.
 */
Gan_Matrix22_f
 gan_mat33f_get_m22tl_s ( const Gan_Matrix33_f *A )
{
   Gan_Matrix22_f B;

   (void)gan_mat33f_get_m22tl_q ( A, &B );
   return B;
}

/**
 * \brief Extract top part of right hand column of 3x3 matrix.
 *
 * Extract top-right column of 3x3 matrix \a A.
 *
 * \return Result 2-vector part.
 */
Gan_Vector2_f
 gan_mat33f_get_v2tr_s ( const Gan_Matrix33_f *A )
{
   Gan_Vector2_f p;

   (void)gan_mat33f_get_v2tr_q ( A, &p );
   return p;
}

/**
 * \brief Extract parts of 3x3 matrix.
 *
 * Extract top-left 2x2 part, top-right 2x1 part, bottom-left 1x2 part and
 * bottom-right element of 3x3 matrix \a A into 2x2 matrix \a B,
 * column 2-vector \a p, row 2-vector \a q and scalar pointer \a s.
 * The arrangement is
 * \f[
 *   A = \left(\begin{array}{cc} B & p \\ q^{\top} & s \end{array}\right)
 * \f]
 *
 * \return No value.
 */
void
 gan_mat33f_get_parts_s ( const Gan_Matrix33_f *A, Gan_Matrix22_f *B,
                          Gan_Vector2_f *p, Gan_Vector2_f *q, float *s )
{
   gan_mat33f_get_parts_q ( A, B, p, q, s );
}

/**
 * \}
 */

/**
 * \addtogroup FixedSizeMatrixBuild
 * \{
 */

/**
 * \brief Build a 3x3 matrix from parts.
 *
 * Build 3x3 matrix from 2x2 matrix \a B, column 2-vector \a p,
 * row 2-vector \a q and scalar \a s, which are respectively inserted
 * in the top-left, top-right, bottom-left and bottom-right parts of \a A.
 * The arrangement of the matrix is
 * \f[
 *    \left(\begin{array}{cc} B & p \\ q^{\top} & s \end{array}\right)
 * \f]
 * If \a p and/or \a q are passed as \c NULL, the corresponding
 * parts of the result are filled with zeros.
 *
 * \return Pointer to filled matrix \a A.
 */
Gan_Matrix33_f
 gan_mat33f_set_parts_s ( Gan_Matrix22_f *B,
                          const Gan_Vector2_f *p, const Gan_Vector2_f *q, float s )
{
   Gan_Matrix33_f A;
   Gan_Vector2_f zerov = { 0.0F, 0.0F };

   (void)gan_mat33f_set_parts_q ( &A, B, (p == NULL) ? &zerov : p,
                                 (q == NULL) ? &zerov : q, s );
   return A;
}

/**
 * \brief Build 3x3 matrix from 3-vector columns.
 *
 * Build 3x3 matrix from 3-vector columns \a p, \a q and \a r.
 * The arrangement can be written
 * \f[
 *     \left(\begin{array}{ccc} p & q & r \end{array}\right)
 * \f]
 *
 * \return Filled matrix.
 */
Gan_Matrix33_f
 gan_mat33f_set_cols_s ( const Gan_Vector3_f *p, const Gan_Vector3_f *q, const Gan_Vector3_f *r )
{
   Gan_Matrix33_f A;

   (void)gan_mat33f_set_cols_q ( &A, p, q, r );
   return A;
}

/**
 * \brief Build 3x3 matrix from 3-vector rows.
 *
 * Build 3x3 matrix from 3-vector rows \a p, \a q and \a r.
 * The arrangement can be written
 * \f[
 *     \left(\begin{array}{c} p^{\top} \\ q^{\top} \\ r^{\top}
 *     \end{array}\right)
 * \f]
 *
 * \return Filled matrix.
 */
Gan_Matrix33_f
 gan_mat33f_set_rows_s ( const Gan_Vector3_f *p, const Gan_Vector3_f *q, const Gan_Vector3_f *r )
{
   Gan_Matrix33_f A;

   (void)gan_mat33f_set_rows_q ( &A, p, q, r );
   return A;
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
 * \brief Convert generic rectangular matrix to 3x3 matrix structure.
 *
 * Convert generic rectangular matrix \a A to 3x3 matrix structure \a B.
 *
 * \return Pointer to result matrix \a B.
 */
Gan_Matrix33_f *
 gan_mat33f_from_matf_q ( const Gan_Matrix_f *A, Gan_Matrix33_f *B )
{
   if ( !gan_matf_read_va ( A, 3, 3, &B->xx, &B->xy, &B->xz,
                                     &B->yx, &B->yy, &B->yz,
                                     &B->zx, &B->zy, &B->zz ) )
   {
      gan_err_register ( "gan_mat33f_from_matf_q", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   return B;
}

/**
 * \brief Convert symmetric matrix to generic 3x3 matrix structure.
 *
 * Convert symmetric matrix \a A to generic 3x3 matrix structure \a B.
 *
 * \return Pointer to result matrix \a B.
 */
Gan_Matrix33_f *
 gan_mat33f_from_squmatf_q ( const Gan_SquMatrix_f *A, Gan_Matrix33_f *B )
{
   switch ( A->type )
   {
      case GAN_SCALED_IDENT_MATRIX:
        if ( !gan_scalImatf_read_va ( A, 3, &B->xx ) )
        {
           gan_err_register ( "gan_mat33f_from_squmatf_q", GAN_ERROR_FAILURE,
                              "" );
           return NULL;
        }

        B->xy = B->yx = B->xz = B->zx = B->yz = B->zy = 0.0F;
        B->zz = B->yy = B->xx;
        break;

      case GAN_DIAGONAL_MATRIX:
        if ( !gan_diagmatf_read_va ( A, 3, &B->xx, &B->yy, &B->zz ) )
        {
           gan_err_register ( "gan_mat33f_from_squmatf_q", GAN_ERROR_FAILURE,
                              "" );
           return NULL;
        }

        B->xy = B->yx = B->xz = B->zx = B->yz = B->zy = 0.0F;
        break;

      case GAN_SYMMETRIC_MATRIX:
        if ( !gan_symmatf_read_va ( A, 3, &B->xx, &B->yx, &B->yy,
                                          &B->zx, &B->zy, &B->zz ) )
        {
           gan_err_register ( "gan_mat33f_from_squmatf_q", GAN_ERROR_FAILURE,
                              "" );
           return NULL;
        }

        B->xy = B->yx; B->xz = B->zx; B->yz = B->zy;
        break;

      case GAN_LOWER_TRI_MATRIX:
        if ( !gan_ltmatf_read_va ( A, 3, &B->xx, &B->yx, &B->yy,
                                         &B->zx, &B->zy, &B->zz ) )
        {
           gan_err_register ( "gan_mat33f_from_squmatf_q", GAN_ERROR_FAILURE,
                              "" );
           return NULL;
        }

        B->xy = B->xz = B->yz = 0.0F;
        break;

      case GAN_UPPER_TRI_MATRIX:
        if ( !gan_utmatf_read_va ( A, 3, &B->xx, &B->xy, &B->yy,
                                         &B->xz, &B->yz, &B->zz ) )
        {
           gan_err_register ( "gan_mat33f_from_squmatf_q", GAN_ERROR_FAILURE,
                              "" );
           return NULL;
        }

        B->yx = B->zx = B->zy = 0.0F;
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_mat33f_from_squmatf_q", GAN_ERROR_FAILURE,
                           "" );
        return NULL;
   }

   return B;
}

/**
 * \brief Convert symmetric matrix to specific 3x3 symmetric matrix structure.
 *
 * Convert symmetric matrix \a A to specific 3x3 symmetric matrix
 * structure \a B.
 *
 * \return Pointer to result matrix \a B.
 */
Gan_SquMatrix33_f *
 gan_squmat33f_from_squmatf_q ( const Gan_SquMatrix_f *A, Gan_SquMatrix33_f *B )
{
   switch ( A->type )
   {
      case GAN_SCALED_IDENT_MATRIX:
        gan_eval(B->type = GAN_SYMMETRIC_MATRIX33_F);
        if ( !gan_scalImatf_read_va ( A, 3, &B->xx ) )
        {
           gan_err_register ( "gan_squmat33f_from_squmatf_q",
                              GAN_ERROR_FAILURE, "" );
           return NULL;
        }

        B->yx = B->zx = B->zy = 0.0F;
        B->zz = B->yy = B->xx;
        break;

      case GAN_DIAGONAL_MATRIX:
        gan_eval(B->type = GAN_SYMMETRIC_MATRIX33_F);
        if ( !gan_diagmatf_read_va ( A, 3, &B->xx, &B->yy, &B->zz ) )
        {
           gan_err_register ( "gan_squmat33f_from_squmatf_q",
                              GAN_ERROR_FAILURE, "" );
           return NULL;
        }

        B->yx = B->zx = B->zy = 0.0F;
        break;

      case GAN_SYMMETRIC_MATRIX:
        gan_eval(B->type = GAN_SYMMETRIC_MATRIX33_F);
        if ( !gan_symmatf_read_va ( A, 3, &B->xx, &B->yx, &B->yy,
                                          &B->zx, &B->zy, &B->zz ) )
        {
           gan_err_register ( "gan_squmat33f_from_squmatf_q",
                              GAN_ERROR_FAILURE, "" );
           return NULL;
        }

        break;

      case GAN_LOWER_TRI_MATRIX:
        gan_eval(B->type = GAN_LOWER_TRI_MATRIX33_F);
        if ( !gan_ltmatf_read_va ( A, 3, &B->xx, &B->yx, &B->yy,
                                         &B->zx, &B->zy, &B->zz ) )
        {
           gan_err_register ( "gan_squmat33f_from_squmatf_q",
                              GAN_ERROR_FAILURE, "" );
           return NULL;
        }

        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_squmat33f_from_squmatf_q", GAN_ERROR_FAILURE,
                           "" );
        return NULL;
   }

   return B;
}

/**
 * \}
 */

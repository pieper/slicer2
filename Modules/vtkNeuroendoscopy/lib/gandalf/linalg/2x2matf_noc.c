/**
 * File:          $RCSfile: 2x2matf_noc.c,v $
 * Module:        2x2 matrices (single precision)
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
 * \brief Print 2x2 matrix to file
 *
 * Print 2x2 matrix \a A to file pointer \a fp, with prefix string \a prefix,
 * indentation \a indent and floating-point format \a fmt.
 *
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 */
Gan_Bool
 gan_mat22f_fprint ( FILE *fp, const Gan_Matrix22_f *A,
                     const char *prefix, int indent, const char *fmt )
{
   int i;

   /* print indentation on first line */
   for ( i = indent-1; i >= 0; i-- ) fprintf ( fp, " " );
   fprintf ( fp, "%s\n", prefix );
   for ( i = indent-1; i >= 0; i-- ) fprintf ( fp, " " );
   fprintf ( fp, " " ); fprintf ( fp, fmt, A->xx );
   fprintf ( fp, " " ); fprintf ( fp, fmt, A->xy );
   fprintf ( fp, "\n" );
   for ( i = indent-1; i >= 0; i-- ) fprintf ( fp, " " );
   fprintf ( fp, " " ); fprintf ( fp, fmt, A->yx );
   fprintf ( fp, " " ); fprintf ( fp, fmt, A->yy );
   fprintf ( fp, "\n" );

   /* success */
   return GAN_TRUE;
}

/**
 * \brief Read 2x2 matrix from file.
 *
 * Read 2x2 matrix \a A from file pointer \a fp. The prefix
 * string for the matrix is read from the file into the \a prefix string, up
 * to the maximum length \a prefix_len of the \a prefix string. Any remaining
 * characters after \a prefix has been filled are ignored. Pass \c NULL for
 * \a prefix and zero for \a prefix_len to ignore the prefix string.
 *
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 */
Gan_Bool
 gan_mat22f_fscanf ( FILE *fp,
                     Gan_Matrix22_f *A, char *prefix, int prefix_len )
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
      gan_err_register ( "gan_mat22f_fscanf", GAN_ERROR_CORRUPTED_FILE, "" );
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

   gan_err_test_bool ( ch == '\n', "gan_mat22f_fscanf",
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

      gan_err_test_bool ( ch == '\n', "gan_mat22f_fscanf",
                          GAN_ERROR_CORRUPTED_FILE, "" );
   }

   /* read rows of matrix */
   result = 0;
   result += fscanf ( fp, "\n%f%f\n", &A->xx, &A->xy );
   result += fscanf ( fp, "%f%f",     &A->yx, &A->yy );
   gan_err_test_bool ( result == 4, "gan_mat22f_fscanf",
                       GAN_ERROR_CORRUPTED_FILE, "" );

   /* read end-of-line character */
   result = getc(fp);
   gan_err_test_bool ( result == '\n', "gan_mat22f_fscanf",
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
 * \brief Fill 2x2 matrix with values.
 *
 * Fill 2x2 matrix with values:
 * \f[
 *   \left(\begin{array}{cc} XX & XY \\ YX & YY  \end{array}\right)
 * \f]
 *
 * \return Filled matrix.
 */
Gan_Matrix22_f
 gan_mat22f_fill_s ( float XX, float XY, float YX, float YY )
{
   Gan_Matrix22_f A;

   (void)gan_mat22f_fill_q ( &A, XX, XY, YX, YY );
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
 * \brief Transpose 2x2 matrix in-place.
 *
 * Transpose 2x2 matrix \a A in-place: \f$ A \leftarrow A^{\top} \f$.
 *
 * \return Pointer to transpose matrix \a A.
 */
Gan_Matrix22_f *
 gan_mat22f_tpose_i ( Gan_Matrix22_f *A )
{
   float tmp;

   tmp = A->xy; A->xy = A->yx; A->yx = tmp;
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
 * \brief Invert 2x2 matrix.
 *
 * Compute inverse \f$ A^{-1} \f$ of 2x2 matrix \a A, writing the result into
 * matrix \a B, so that \f$ A B = I \f$.
 *
 * \return Result matrix \a B on success, \c NULL on error.
 */
Gan_Matrix22_f *
 gan_mat22f_invert ( Gan_Matrix22_f *A, Gan_Matrix22_f *B, int *error_code )
{
   float det = A->xx*A->yy - A->yx*A->xy;

   if ( det == 0.0 )
   {
      if ( error_code == NULL )
      {
         gan_err_flush_trace();
         gan_err_register ( "gan_mat22f_invert_q", GAN_ERROR_SINGULAR_MATRIX,
                            "" );
      }
      else
         *error_code = GAN_ERROR_SINGULAR_MATRIX;

      return NULL;
   }
      
   if ( A == B )
   {
      Gan_Matrix22_f Ap = *A;

      B->xx =  Ap.yy/det; B->xy = -Ap.xy/det;
      B->yx = -Ap.yx/det; B->yy =  Ap.xx/det;
   }
   else
   {
      B->xx =  A->yy/det; B->xy = -A->xy/det;
      B->yx = -A->yx/det; B->yy =  A->xx/det;
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
 * \brief Compute adjoint of 2x2 matrix.
 *
 * Compute adjoint \f$ A^{*} \f$ of 2x2 matrix \a A, writing the result into
 * matrix \a B, so that
 * \f[
 *   B A^{\top} = \mbox{det}(A) I
 * \f]
 *
 * \return Result matrix \a B on success, \c NULL on error.
 */
Gan_Matrix22_f *
 gan_mat22f_adjoint_q ( const Gan_Matrix22_f *A, Gan_Matrix22_f *B )
{
   gan_err_test_ptr ( A != B, "gan_mat22f_adjoint_q",
                      GAN_ERROR_ILLEGAL_ARGUMENT, "can't do adjoint in place");
   B->xx =  A->yy; B->xy = -A->yx;
   B->yx = -A->xy; B->yy =  A->xx;
   return B;
}

/**
 * \brief Compute adjoint transpose of 2x2 matrix.
 *
 * Compute adjoint transpose \f$ A^{*{\top}} \f$ of 2x2 matrix \a A, writing
 * the result into matrix \a B, so that
 * \f[
 *   B A = \mbox{det}(A) I
 * \f]
 *
 * \return Result matrix \a B on success, \c NULL on error.
 */
Gan_Matrix22_f *
 gan_mat22f_adjointT_q ( const Gan_Matrix22_f *A, Gan_Matrix22_f *B )
{
   gan_err_test_ptr ( A != B, "gan_mat22f_adjointT_q",
                      GAN_ERROR_ILLEGAL_ARGUMENT, "can't do adjoint in place");
   B->xx =  A->yy; B->xy = -A->xy;
   B->yx = -A->yx; B->yy =  A->xx;
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
 * \brief Extract rows of 2x2 matrix.
 *
 * Extract rows of 2x2 matrix \a A into 2-vectors \a p and \a q.
 * The arrangement can be written
 * \f[
 *     A = \left(\begin{array}{c} p^{\top} \\ q^{\top} \end{array}\right)
 * \f]
 *
 * \return No value.
 */
void gan_mat22f_get_rows_s ( const Gan_Matrix22_f *A,
                             Gan_Vector2_f *p, Gan_Vector2_f *q )
{
   gan_mat22f_get_rows_q ( A, p, q );
}

/**
 * \brief Extract columns of 2x2 matrix
 *
 * Extract columns of 2x2 matrix \a A into 2-vectors \a p and \a q.
 * The arrangement can be written
 * \f[
 *     A = \left(\begin{array}{cc} p & q \end{array}\right)
 * \f]
 *
 * \return No value.
 */
void gan_mat22f_get_cols_s ( const Gan_Matrix22_f *A,
                             Gan_Vector2_f *p, Gan_Vector2_f *q )
{
   gan_mat22f_get_cols_q ( A, p, q );
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
 * \brief Convert generic rectangular matrix to 2x2 matrix structure.
 *
 * Convert generic rectangular matrix \a A to 2x2 matrix structure \a B.
 *
 * \return Pointer to result matrix \a B.
 */
Gan_Matrix22_f *
 gan_mat22f_from_matf_q ( const Gan_Matrix_f *A, Gan_Matrix22_f *B )
{
   if ( !gan_matf_read_va ( A, 2, 2, &B->xx, &B->xy,
                                     &B->yx, &B->yy ) )
   {
      gan_err_register ( "gan_mat22f_from_matf_q", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   return B;
}

/**
 * \brief Convert symmetric matrix to generic 2x2 matrix structure.
 *
 * Convert symmetric matrix \a A to generic 2x2 matrix structure \a B.
 *
 * \return Pointer to result matrix \a B.
 */
Gan_Matrix22_f *
 gan_mat22f_from_squmatf_q ( const Gan_SquMatrix_f *A, Gan_Matrix22_f *B )
{
   switch ( A->type )
   {
      case GAN_SCALED_IDENT_MATRIX:
        if ( !gan_scalImatf_read_va ( A, 2, &B->xx ) )
        {
           gan_err_register ( "gan_mat22f_from_squmatf_q", GAN_ERROR_FAILURE,
                              "" );
           return NULL;
        }

        B->xy = B->yx = 0.0F;
        B->yy = B->xx;
        break;

      case GAN_DIAGONAL_MATRIX:
        if ( !gan_diagmatf_read_va ( A, 2, &B->xx, &B->yy ) )
        {
           gan_err_register ( "gan_mat22f_from_squmatf_q", GAN_ERROR_FAILURE,
                              "" );
           return NULL;
        }

        B->xy = B->yx = 0.0F;
        break;

      case GAN_SYMMETRIC_MATRIX:
        if ( !gan_symmatf_read_va ( A, 2, &B->xx, &B->yx, &B->yy ) )
        {
           gan_err_register ( "gan_mat22f_from_squmatf_q", GAN_ERROR_FAILURE,
                              "" );
           return NULL;
        }

        B->xy = B->yx;
        break;

      case GAN_LOWER_TRI_MATRIX:
        if ( !gan_ltmatf_read_va ( A, 2, &B->xx, &B->yx, &B->yy ) )
        {
           gan_err_register ( "gan_mat22f_from_squmatf_q", GAN_ERROR_FAILURE,
                              "" );
           return NULL;
        }

        B->xy = 0.0F;
        break;

      case GAN_UPPER_TRI_MATRIX:
        if ( !gan_utmatf_read_va ( A, 2, &B->xx, &B->xy, &B->yy ) )
        {
           gan_err_register ( "gan_mat22f_from_squmatf_q", GAN_ERROR_FAILURE,
                              "" );
           return NULL;
        }

        B->yx = 0.0F;
        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_mat22f_from_squmatf_q", GAN_ERROR_FAILURE,
                           "" );
        return NULL;
   }

   return B;
}

/**
 * \brief Convert symmetric matrix to specific 2x2 symmetric matrix structure.
 *
 * Convert symmetric matrix \a A to specific 2x2 symmetric matrix
 * structure \a B.
 *
 * \return Pointer to result matrix \a B.
 */
Gan_SquMatrix22_f *
 gan_squmat22f_from_squmatf_q ( const Gan_SquMatrix_f *A, Gan_SquMatrix22_f *B )
{
   switch ( A->type )
   {
      case GAN_SCALED_IDENT_MATRIX:
        gan_eval(B->type = GAN_SYMMETRIC_MATRIX22_F);
        if ( !gan_scalImatf_read_va ( A, 2, &B->xx ) )
        {
           gan_err_register ( "gan_squmat22f_from_squmatf_q",
                              GAN_ERROR_FAILURE, "" );
           return NULL;
        }
        
        B->yx = 0.0F;
        B->yy = B->xx;
        break;

      case GAN_DIAGONAL_MATRIX:
        gan_eval(B->type = GAN_SYMMETRIC_MATRIX22_F);
        if ( !gan_diagmatf_read_va ( A, 2, &B->xx, &B->yy ) )
        {
           gan_err_register ( "gan_squmat22f_from_squmatf_q",
                              GAN_ERROR_FAILURE, "" );
           return NULL;
        }

        B->yx = 0.0F;
        break;

      case GAN_SYMMETRIC_MATRIX:
        gan_eval(B->type = GAN_SYMMETRIC_MATRIX22_F);
        if ( !gan_symmatf_read_va ( A, 2, &B->xx, &B->yx, &B->yy ) )
        {
           gan_err_register ( "gan_squmat22f_from_squmatf_q",
                              GAN_ERROR_FAILURE, "" );
           return NULL;
        }

        break;

      case GAN_LOWER_TRI_MATRIX:
        gan_eval(B->type = GAN_LOWER_TRI_MATRIX22_F);
        if ( !gan_ltmatf_read_va ( A, 2, &B->xx, &B->yx, &B->yy ) )
        {
           gan_err_register ( "gan_squmat22f_from_squmatf_q",
                              GAN_ERROR_FAILURE, "" );
           return NULL;
        }

        break;

      default:
        gan_err_flush_trace();
        gan_err_register ( "gan_squmat22f_from_squmatf_q", GAN_ERROR_FAILURE,
                           "" );
        return NULL;
   }

   return B;
}

/**
 * \}
 */

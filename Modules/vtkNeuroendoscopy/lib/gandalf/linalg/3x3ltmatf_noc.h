/**
 * File:          $RCSfile: 3x3ltmatf_noc.h,v $
 * Module:        3x3 lower triangular matrices (single precision)
 * Part of:       Gandalf Library
 *
 * Revision:      $Revision: 1.1.2.1 $
 * Last edited:   $Date: 2007/10/14 03:16:25 $
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

#ifndef _GAN_3X3LTMATF_H

/**
 * \addtogroup FixedSizeMatrixFill
 * \{
 */

/**
 * \brief Macro: Fill 3x3 lower triangular matrix with values.
 *
 * Fill 3x3 lower triangular matrix \a A with values:
 * \f[ A = \left(\begin{array}{ccc} XX & 0 & 0 \\ YX & YY & 0 \\
 *                                  ZX & ZY & ZZ \end{array}\right)
 * \f]
 *
 * \return Pointer to filled matrix \a A.
 */
#ifdef GAN_GENERATE_DOCUMENTATION
GANDALF_API Gan_SquMatrix33_f *gan_ltmat33f_fill_q ( Gan_SquMatrix33_f *A,
                                         float XX,
                                         float YX, float YY,
                                         float ZX, float ZY, float ZZ );
#else
#define gan_ltmat33f_fill_q(A,XX,YX,YY,ZX,ZY,ZZ) (GAN_ST33F_FILL(A,GAN_LOWER_TRI_MATRIX33_F,XX,YX,YY,ZX,ZY,ZZ),A)
#endif

/**
 * \}
 */

/**
 * \addtogroup FixedSizeMatrixExtract
 * \{
 */

/**
 * \brief Macro: Extract parts of 3x3 lower triangular matrix.
 *
 * Extract top-left 2x2 part, bottom-left 1x2 part and bottom-right element
 * of 3x3 lower triangular matrix \a A into 2x2 lower triangular matrix \a B,
 * row 2-vector \a p and scalar pointer \a s. The arrangement is
 * \f[
 *     A = \left(\begin{array}{cc} B & 0 \\ p^{\top} & s \end{array}\right)
 * \f]
 */
#ifdef GAN_GENERATE_DOCUMENTATION
GANDALF_API void gan_ltmat33f_get_parts_q ( const Gan_SquMatrix33_f *A,
                                Gan_SquMatrix22_f *B, Gan_Vector2_f *p,
                                float *s );
#else
#define gan_ltmat33f_get_parts_q(A,B,p,s)\
           (GAN_TEST_OP3(A,B,p,\
                         Gan_SquMatrix33_f,Gan_SquMatrix22_f,Gan_Vector2_f),\
            assert((A)->type == GAN_LOWER_TRI_MATRIX33_F),\
            gan_eval((B)->type = GAN_LOWER_TRI_MATRIX22_F),\
            (B)->xx=(A)->xx,\
            (B)->yx=(A)->yx,(B)->yy=(A)->yy,\
            (p)->x=(A)->zx,(p)->y=(A)->zy,*(s)=(A)->zz)
#endif

/**
 * \}
 */

/**
 * \addtogroup FixedSizeMatrixBuild
 * \{
 */

/**
 * \brief Macro: Build a 3x3 lower triangular matrix from parts.
 *
 * Build 3x3 lower triangular matrix from 2x2 lower triangular matrix
 * \a B, row 2-vector \a p, and scalar \a s, which are respectively inserted
 * in the top-left, bottom-left and bottom-right parts of the 3x3 matrix.
 * The arrangement of the matrix is
 * \f[
 *    \left(\begin{array}{cc} B & 0 \\ p^{\top} & s \end{array}\right)
 * \f]
 *
 * \return Pointer to filled matrix \a A.
 */
#ifdef GAN_GENERATE_DOCUMENTATION
GANDALF_API Gan_SquMatrix33_f * gan_ltmat33f_set_parts_q ( Gan_SquMatrix33_f *A,
                                               const Gan_SquMatrix22_f *B,
                                               const Gan_Vector2_f *p, float s );
#else
#define gan_ltmat33f_set_parts_q(A,B,p,s)\
           (GAN_TEST_OP3(A,B,p,Gan_SquMatrix33_f,Gan_SquMatrix22_f,Gan_Vector2_f),\
            assert((B)->type == GAN_LOWER_TRI_MATRIX22_F),\
            gan_eval((A)->type = GAN_LOWER_TRI_MATRIX33_F),\
            (A)->xx=(B)->xx,\
            (A)->yx=(B)->yx,(A)->yy=(B)->yy,\
            (A)->zx=(p)->x,(A)->zy=(p)->y,(A)->zz=(s),A)
#endif

/**
 * \}
 */

#ifndef GAN_GENERATE_DOCUMENTATION

GANDALF_API Gan_SquMatrix33_f gan_ltmat33f_fill_s ( float XX,
                                        float YX, float YY,
                                        float ZX, float ZY, float ZZ );
GANDALF_API void gan_ltmat33f_get_parts_s ( const Gan_SquMatrix33_f *A,
                                Gan_SquMatrix22_f *B, Gan_Vector2_f *p,
                                float *s );
GANDALF_API Gan_SquMatrix33_f gan_ltmat33f_set_parts_s ( const Gan_SquMatrix22_f *B,
                                             const Gan_Vector2_f *p, float s );

#endif /* #ifndef GAN_GENERATE_DOCUMENTATION */

#endif /* #ifndef _GAN_3X3LTMATF_H */

#define GAN_MATTYPE Gan_SquMatrix33_f
#define GAN_MATRIX_TYPE Gan_SquMatrix_f
#define GAN_SQUMATRIX_TYPE Gan_SquMatrix_f
#define GAN_VECTOR_TYPE Gan_Vector_f
#define GAN_MAT_ELEMENT_TYPE GAN_FLOAT
#define GAN_REALTYPE float
#define GAN_FWRITE_LENDIAN gan_fwrite_lendian_f32
#define GAN_FREAD_LENDIAN  gan_fread_lendian_f32
#define GAN_VECTYPE1 Gan_Vector3_f
#define GAN_VECTYPE2 Gan_Vector3_f
#define GAN_SQUMATTYPEL Gan_SquMatrix33_f
#define GAN_SQUMATTYPER Gan_SquMatrix33_f
#define GAN_FIXED_MATRIX_TYPE GAN_LOWER_TRI_MATRIX33_F
#define GAN_MAT_FPRINT     gan_ltmat33f_fprint
#define GAN_MAT_PRINT      gan_ltmat33f_print
#define GAN_MAT_FSCANF     gan_ltmat33f_fscanf
#define GAN_MAT_FWRITE     gan_ltmat33f_fwrite
#define GAN_MAT_FREAD      gan_ltmat33f_fread
#define GAN_MAT_ZERO_Q     gan_ltmat33f_zero_q
#define GAN_MAT_ZERO_S     gan_ltmat33f_zero_s
#define GAN_MAT_COPY_Q     gan_ltmat33f_copy_q
#define GAN_MAT_COPY_S     gan_ltmat33f_copy_s
#define GAN_MAT_SCALE_Q    gan_ltmat33f_scale_q
#define GAN_MAT_SCALE_S    gan_ltmat33f_scale_s
#define GAN_MAT_SCALE_I    gan_ltmat33f_scale_i
#define GAN_MAT_DIVIDE_Q   gan_ltmat33f_divide_q
#define GAN_MAT_DIVIDE_S   gan_ltmat33f_divide_s
#define GAN_MAT_DIVIDE_I   gan_ltmat33f_divide_i
#define GAN_MAT_NEGATE_Q   gan_ltmat33f_negate_q
#define GAN_MAT_NEGATE_S   gan_ltmat33f_negate_s
#define GAN_MAT_NEGATE_I   gan_ltmat33f_negate_i
#define GAN_MAT_UNIT_Q     gan_ltmat33f_unit_q
#define GAN_MAT_UNIT_S     gan_ltmat33f_unit_s
#define GAN_MAT_UNIT_I     gan_ltmat33f_unit_i
#define GAN_MAT_ADD_Q      gan_ltmat33f_add_q
#define GAN_MAT_ADD_I1     gan_ltmat33f_add_i1
#define GAN_MAT_ADD_I2     gan_ltmat33f_add_i2
#define GAN_MAT_INCREMENT  gan_ltmat33f_increment
#define GAN_MAT_ADD_S      gan_ltmat33f_add_s
#define GAN_MAT_SUB_Q      gan_ltmat33f_sub_q
#define GAN_MAT_SUB_I1     gan_ltmat33f_sub_i1
#define GAN_MAT_SUB_I2     gan_ltmat33f_sub_i2
#define GAN_MAT_DECREMENT  gan_ltmat33f_decrement
#define GAN_MAT_SUB_S      gan_ltmat33f_sub_s
#define GAN_MAT_MULTV_Q    gan_ltmat33f_multv3_q
#define GAN_MAT_MULTV_S    gan_ltmat33f_multv3_s
#define GAN_MAT_MULTV_I    gan_ltmat33f_multv3_i
#define GAN_MATT_MULTV_Q   gan_ltmat33Tf_multv3_q
#define GAN_MATT_MULTV_S   gan_ltmat33Tf_multv3_s
#define GAN_MATT_MULTV_I   gan_ltmat33Tf_multv3_i
#define GAN_MAT_SRMULTT_Q  gan_ltmat33f_srmultT_q
#define GAN_MAT_SRMULTT_S  gan_ltmat33f_srmultT_s
#define GAN_MAT_SRMULTT_I  gan_ltmat33f_srmultT_i
#define GAN_MAT_SLMULTT_Q  gan_ltmat33f_slmultT_q
#define GAN_MAT_SLMULTT_S  gan_ltmat33f_slmultT_s
#define GAN_MAT_SLMULTT_I  gan_ltmat33f_slmultT_i
#define GAN_MAT_SUMSQR_Q   gan_ltmat33f_sumsqr_q
#define GAN_MAT_SUMSQR_S   gan_ltmat33f_sumsqr_s
#define GAN_MAT_FNORM_Q    gan_ltmat33f_Fnorm_q
#define GAN_MAT_FNORM_S    gan_ltmat33f_Fnorm_s

#define GAN_SQUMAT_IDENT_Q    gan_ltmat33f_ident_q
#define GAN_SQUMAT_IDENT_S    gan_ltmat33f_ident_s
#define GAN_SQUMATI_MULTV_Q   gan_ltmat33If_multv3_q
#define GAN_SQUMATI_MULTV_S   gan_ltmat33If_multv3_s
#define GAN_SQUMATI_MULTV_I   gan_ltmat33If_multv3_i
#define GAN_SQUMATIT_MULTV_Q  gan_ltmat33ITf_multv3_q
#define GAN_SQUMATIT_MULTV_S  gan_ltmat33ITf_multv3_s
#define GAN_SQUMATIT_MULTV_I  gan_ltmat33ITf_multv3_i
#define GAN_SQUMAT_INVERT     gan_ltmat33f_invert
#define GAN_SQUMAT_INVERT_Q   gan_ltmat33f_invert_q
#define GAN_SQUMAT_INVERT_S   gan_ltmat33f_invert_s
#define GAN_SQUMAT_INVERT_I   gan_ltmat33f_invert_i
#define GAN_SQUMAT_DET_Q      gan_ltmat33f_det_q
#define GAN_SQUMAT_DET_S      gan_ltmat33f_det_s
#define GAN_SQUMAT_TRACE_Q    gan_ltmat33f_trace_q
#define GAN_SQUMAT_TRACE_S    gan_ltmat33f_trace_s

#ifndef _GAN_3X3LTMATF_H
#define _GAN_3X3LTMATF_H

#ifndef GAN_GENERATE_DOCUMENTATION
#define gan_ltmat33f_zero_q(A) (GAN_ST33F_ZERO(A,GAN_LOWER_TRI_MATRIX33_F),A)
#define gan_ltmat33f_ident_q(A) (GAN_ST33F_IDENT(A,GAN_LOWER_TRI_MATRIX33_F),A)
#define gan_ltmat33f_copy_q(A,B) (GAN_ST33F_COPY(A,B,GAN_LOWER_TRI_MATRIX33_F),B)
#define gan_ltmat33f_scale_q(A,a,B) (GAN_ST33F_SCALE(A,a,B,GAN_LOWER_TRI_MATRIX33_F),B)
#define gan_ltmat33f_scale_i(A,a) gan_ltmat33f_scale_q(A,a,A)
#define gan_ltmat33f_divide_q(A,a,B) ((a)==0.0 ? NULL : (GAN_ST33F_DIVIDE(A,a,B,GAN_LOWER_TRI_MATRIX33_F),B))
#define gan_ltmat33f_divide_i(A,a) gan_ltmat33f_divide_q(A,a,A)
#define gan_ltmat33f_negate_q(A,B) (GAN_ST33F_NEGATE(A,B,GAN_LOWER_TRI_MATRIX33_F),B)
#define gan_ltmat33f_negate_i(A) gan_ltmat33f_negate_q(A,A)
#define gan_ltmat33f_unit_i(A) gan_ltmat33f_unit_q(A,A)
#define gan_ltmat33f_invert_q(A,B) gan_ltmat33f_invert(A,B,NULL)
#define gan_ltmat33f_invert_i(A)   gan_ltmat33f_invert(A,A,NULL)
#define gan_ltmat33f_add_q(A,B,C) (GAN_ST33F_ADD(A,B,C,GAN_LOWER_TRI_MATRIX33_F),C)
#define gan_ltmat33f_add_i1(A,B) gan_ltmat33f_add_q(A,B,A)
#define gan_ltmat33f_add_i2(A,B) gan_ltmat33f_add_q(A,B,B)
#define gan_ltmat33f_increment(A,B) gan_ltmat33f_add_i1(A,B)
#define gan_ltmat33f_sub_q(A,B,C) (GAN_ST33F_SUB(A,B,C,GAN_LOWER_TRI_MATRIX33_F),C)
#define gan_ltmat33f_sub_i1(A,B) gan_ltmat33f_sub_q(A,B,A)
#define gan_ltmat33f_sub_i2(A,B) gan_ltmat33f_sub_q(A,B,B)
#define gan_ltmat33f_decrement(A,B) gan_ltmat33f_sub_i1(A,B)
#define gan_ltmat33f_multv3_q(A,p,q)\
 (GAN_TEST_OP3(A,p,q,Gan_SquMatrix33_f,Gan_Vector3_f,Gan_Vector3_f),\
  assert((A)->type == GAN_LOWER_TRI_MATRIX33_F),\
  GAN_LOW33F_MULT1D((q)->,(A)->,(p)->,x,y,z,x,y,z),q)
#define gan_ltmat33f_multv3_i(A,p) gan_ltmat33f_multv3_q(A,p,p)
#define gan_ltmat33Tf_multv3_q(A,p,q)\
 (GAN_TEST_OP3(A,p,q,Gan_SquMatrix33_f,Gan_Vector3_f,Gan_Vector3_f),\
  assert((A)->type == GAN_LOWER_TRI_MATRIX33_F),\
  GAN_UPP33F_MULT1D((q)->,(A)->,(p)->,x,y,z,x,y,z),q)
#define gan_ltmat33Tf_multv3_i(A,p) gan_ltmat33Tf_multv3_q(A,p,p)
#define gan_ltmat33If_multv3_q(A,p,q)\
 (GAN_TEST_OP3(A,p,q,Gan_SquMatrix33_f,Gan_Vector3_f,Gan_Vector3_f),\
  assert((A)->type == GAN_LOWER_TRI_MATRIX33_F),\
  assert((A)->xx != 0.0 && (A)->yy != 0.0 && (A)->zz != 0.0),\
  GAN_LOW33IF_MULT1D((q)->,(A)->,(p)->,x,y,z,x,y,z),q)
#define gan_ltmat33If_multv3_i(A,p) gan_ltmat33If_multv3_q(A,p,p)
#define gan_ltmat33ITf_multv3_q(A,p,q)\
 (GAN_TEST_OP3(A,p,q,Gan_SquMatrix33_f,Gan_Vector3_f,Gan_Vector3_f),\
  assert((A)->type == GAN_LOWER_TRI_MATRIX33_F),\
  assert((A)->xx != 0.0 && (A)->yy != 0.0 && (A)->zz != 0.0),\
  GAN_UPP33IF_MULT1D((q)->,(A)->,(p)->,x,y,z,x,y,z),q)
#define gan_ltmat33ITf_multv3_i(A,p) gan_ltmat33ITf_multv3_q(A,p,p)
#define gan_ltmat33f_srmultT_q(A,B)\
 (GAN_TEST_OP2(A,B,Gan_SquMatrix33_f,Gan_SquMatrix33_f),\
  assert((A)->type == GAN_LOWER_TRI_MATRIX33_F),\
  (B)->zz = (A)->zx*(A)->zx + (A)->zy*(A)->zy + (A)->zz*(A)->zz,\
  (B)->zy = (A)->yx*(A)->zx + (A)->yy*(A)->zy,\
  (B)->zx = (A)->xx*(A)->zx,\
  (B)->yy = (A)->yx*(A)->yx + (A)->yy*(A)->yy,\
  (B)->yx = (A)->xx*(A)->yx,\
  (B)->xx = (A)->xx*(A)->xx,\
  gan_eval((B)->type = GAN_SYMMETRIC_MATRIX33_F),B)
#define gan_ltmat33f_srmultT_i(A) gan_ltmat33f_srmultT_q(A,A)
#define gan_ltmat33f_slmultT_q(A,B)\
 (GAN_TEST_OP2(A,B,Gan_SquMatrix33_f,Gan_SquMatrix33_f),\
  assert((A)->type == GAN_LOWER_TRI_MATRIX33_F),\
  (B)->xx = (A)->xx*(A)->xx + (A)->yx*(A)->yx + (A)->zx*(A)->zx,\
  (B)->yx = (A)->yx*(A)->yy + (A)->zx*(A)->zy,\
  (B)->yy = (A)->yy*(A)->yy + (A)->zy*(A)->zy,\
  (B)->zx = (A)->zx*(A)->zz,\
  (B)->zy = (A)->zy*(A)->zz,\
  (B)->zz = (A)->zz*(A)->zz,\
  gan_eval((B)->type = GAN_SYMMETRIC_MATRIX33_F),B)
#define gan_ltmat33f_slmultT_i(A) gan_ltmat33f_slmultT_q(A,A)
#define gan_ltmat33f_det_q(A) (GAN_TEST_OP1(A,Gan_SquMatrix33_f),\
                              assert((A)->type==GAN_LOWER_TRI_MATRIX33_F),\
                              ((A)->xx*(A)->yy*(A)->zz))
#define gan_ltmat33f_trace_q(A) (GAN_TEST_OP1(A,Gan_SquMatrix33_f),\
                                assert((A)->type==GAN_LOWER_TRI_MATRIX33_F),\
                                ((A)->xx + (A)->yy + (A)->zz))
#define gan_ltmat33f_sumsqr_q(A) (GAN_TEST_OP1(A,Gan_SquMatrix33_f),\
                                 assert((A)->type==GAN_LOWER_TRI_MATRIX33_F),\
                    ((A)->xx*(A)->xx + (A)->yx*(A)->yx + (A)->yy*(A)->yy +\
                     (A)->zx*(A)->zx + (A)->zy*(A)->zy + (A)->zz*(A)->zz))
#define gan_ltmat33f_Fnorm_q(A) (GAN_TEST_OP1(A,Gan_SquMatrix33_f),\
                                assert((A)->type==GAN_LOWER_TRI_MATRIX33_F),\
                 sqrt((A)->xx*(A)->xx + (A)->yx*(A)->yx + (A)->yy*(A)->yy +\
                      (A)->zx*(A)->zx + (A)->zy*(A)->zy + (A)->zz*(A)->zz))
#endif /* #ifndef GAN_GENERATE_DOCUMENTATION */

#endif /* #ifndef _GAN_3X3LTMATF_H */

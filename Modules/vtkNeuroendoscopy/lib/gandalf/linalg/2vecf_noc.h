/**
 * File:          $RCSfile: 2vecf_noc.h,v $
 * Module:        Size 2 vectors (single precision)
 * Part of:       Gandalf Library
 *
 * Revision:      $Revision: 1.1.2.1 $
 * Last edited:   $Date: 2007/10/14 03:16:24 $
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
 * \addtogroup FixedSizeVectorFill
 * \{
 */

/**
 * \brief Macro: Fill 2-vector with values
 *
 * Fill 2-vector \a p with values:
 * \f[ p = \left(\begin{array}{c} X \\ Y \end{array}\right)
 * \f]
 *
 * \return Pointer to filled 2-vector \a p.
 */
#ifdef GAN_GENERATE_DOCUMENTATION
GANDALF_API Gan_Vector2_f *gan_vec2f_fill_q ( Gan_Vector2_f *p, float X, float Y );
#else
#define gan_vec2f_fill_q(p,X,Y) (GAN_FREP2_A_C((p)->,=,(X),(Y)),p)
#endif

/**
 * \}
 */

#ifndef GAN_GENERATE_DOCUMENTATION

GANDALF_API Gan_Vector2_f gan_vec2f_fill_s ( float X, float Y );

#endif /* #ifndef GAN_GENERATE_DOCUMENTATION */

#define GAN_VECTYPE Gan_Vector2_f
#define GAN_VECTOR_TYPE Gan_Vector_f
#define GAN_VEC_ELEMENT_TYPE GAN_FLOAT
#define GAN_REALTYPE float
#define GAN_FWRITE_LENDIAN gan_fwrite_lendian_f32
#define GAN_FREAD_LENDIAN  gan_fread_lendian_f32
#define GAN_VEC_FPRINT     gan_vec2f_fprint
#define GAN_VEC_PRINT      gan_vec2f_print
#define GAN_VEC_FSCANF     gan_vec2f_fscanf
#define GAN_VEC_FWRITE     gan_vec2f_fwrite
#define GAN_VEC_FREAD      gan_vec2f_fread
#define GAN_VEC_ZERO_Q     gan_vec2f_zero_q
#define GAN_VEC_ZERO_S     gan_vec2f_zero_s
#define GAN_VEC_COPY_Q     gan_vec2f_copy_q
#define GAN_VEC_COPY_S     gan_vec2f_copy_s
#define GAN_VEC_SCALE_Q    gan_vec2f_scale_q
#define GAN_VEC_SCALE_S    gan_vec2f_scale_s
#define GAN_VEC_SCALE_I    gan_vec2f_scale_i
#define GAN_VEC_DIVIDE_Q   gan_vec2f_divide_q
#define GAN_VEC_DIVIDE_S   gan_vec2f_divide_s
#define GAN_VEC_DIVIDE_I   gan_vec2f_divide_i
#define GAN_VEC_NEGATE_Q   gan_vec2f_negate_q
#define GAN_VEC_NEGATE_S   gan_vec2f_negate_s
#define GAN_VEC_NEGATE_I   gan_vec2f_negate_i
#define GAN_VEC_UNIT_Q     gan_vec2f_unit_q
#define GAN_VEC_UNIT_S     gan_vec2f_unit_s
#define GAN_VEC_UNIT_I     gan_vec2f_unit_i
#define GAN_VEC_ADD_Q      gan_vec2f_add_q
#define GAN_VEC_ADD_I1     gan_vec2f_add_i1
#define GAN_VEC_ADD_I2     gan_vec2f_add_i2
#define GAN_VEC_INCREMENT  gan_vec2f_increment
#define GAN_VEC_ADD_S      gan_vec2f_add_s
#define GAN_VEC_SUB_Q      gan_vec2f_sub_q
#define GAN_VEC_SUB_I1     gan_vec2f_sub_i1
#define GAN_VEC_SUB_I2     gan_vec2f_sub_i2
#define GAN_VEC_DECREMENT  gan_vec2f_decrement
#define GAN_VEC_SUB_S      gan_vec2f_sub_s
#define GAN_VEC_DOT_Q      gan_vec2f_dot_q
#define GAN_VEC_DOT_S      gan_vec2f_dot_s
#define GAN_VEC_SQRLEN_Q   gan_vec2f_sqrlen_q
#define GAN_VEC_SQRLEN_S   gan_vec2f_sqrlen_s
#define GAN_VEC_FROM_VEC_Q gan_vec2f_from_vecf_q
#define GAN_VEC_FROM_VEC_S gan_vec2f_from_vecf_s

#ifndef GAN_GENERATE_DOCUMENTATION
/* macros: documentation in vectorf_noc.h */
#define gan_vec2f_zero_q(p) (GAN_TEST_OP1(p,Gan_Vector2_f),\
                             GAN_REP2_AS_C((p)->,=0.0),p)
#define gan_vec2f_copy_q(p,q) (GAN_TEST_OP2(p,q,Gan_Vector2_f,Gan_Vector2_f),\
                               GAN_REP2_AA_C((q)->,=(p)->),q)
#define gan_vec2f_scale_q(p,a,q) (GAN_TEST_OP2(p,q,Gan_Vector2_f,Gan_Vector2_f),\
                                  GAN_REP2_AAS_C((q)->,=(p)->,*(a)),q)
#define gan_vec2f_scale_i(p,a) gan_vec2f_scale_q(p,a,p)
#define gan_vec2f_divide_q(p,a,q) (GAN_TEST_OP2(p,q,Gan_Vector2_f,Gan_Vector2_f),\
                                  (a)==0.0 ? NULL :\
                                   (GAN_REP2_AAS_C((q)->,=(p)->,/(a)),q))
#define gan_vec2f_divide_i(p,a) gan_vec2f_divide_q(p,a,p)
#define gan_vec2f_unit_i(p) gan_vec2f_unit_q(p,p)
#define gan_vec2f_negate_q(p,q) (GAN_TEST_OP2(p,q,Gan_Vector2_f,Gan_Vector2_f),\
                                 GAN_REP2_AA_C((q)->,= -(p)->),q)
#define gan_vec2f_negate_i(p) gan_vec2f_negate_q(p,p)
#define gan_vec2f_add_q(p,q,r)\
           (GAN_TEST_OP3(p,q,r,Gan_Vector2_f,Gan_Vector2_f,Gan_Vector2_f),\
            GAN_REP2_AAA_C((r)->,=(p)->,+(q)->),r)
#define gan_vec2f_add_i1(p,q) gan_vec2f_add_q(p,q,p)
#define gan_vec2f_add_i2(p,q) gan_vec2f_add_q(p,q,q)
#define gan_vec2f_sub_q(p,q,r)\
           (GAN_TEST_OP3(p,q,r,Gan_Vector2_f,Gan_Vector2_f,Gan_Vector2_f),\
            GAN_REP2_AAA_C((r)->,=(p)->,-(q)->),r)
#define gan_vec2f_increment(p,q) gan_vec2f_add_i1(p,q)
#define gan_vec2f_sub_i1(p,q) gan_vec2f_sub_q(p,q,p)
#define gan_vec2f_sub_i2(p,q) gan_vec2f_sub_q(p,q,q)
#define gan_vec2f_decrement(p,q) gan_vec2f_sub_i1(p,q)
#define gan_vec2f_dot_q(p,q) (GAN_TEST_OP2(p,q,Gan_Vector2_f,Gan_Vector2_f),\
                             GAN_REP2_OP_AA_C((p)->,*(q)->,+))
#define gan_vec2f_sqrlen_q(p) gan_vec2f_dot_q(p,p)
#endif /* #ifndef GAN_GENERATE_DOCUMENTATION */

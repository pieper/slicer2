/**
 * File:          $RCSfile: 4x4matrixf.h,v $
 * Module:        4x4 matrices (single precision)
 * Part of:       Gandalf Library
 *
 * Revision:      $Revision: 1.1.2.1 $
 * Last edited:   $Date: 2007/10/14 03:16:26 $
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

#ifndef _GAN_4X4MATRIXF_H
#define _GAN_4X4MATRIXF_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <gandalf/common/misc_defs.h>
#include <gandalf/linalg/repeat44_noc.h>
#include <gandalf/linalg/4vectorf.h>
#include <gandalf/linalg/3x3matrixf.h>
#include <gandalf/linalg/matvecf_macros_noc.h>
#include <gandalf/linalg/matf_gen.h>
#include <gandalf/linalg/matf_square.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup LinearAlgebra
 * \{
 */

/**
 * \addtogroup FixedSizeMatVec
 * \{
 */

/**
 * \addtogroup FixedSizeMatrix
 * \{
 */

/// Structure definition for single precision 4x4 matrix
typedef struct Gan_Matrix44_f
{
   float xx, xy, xz, xw,
         yx, yy, yz, yw,
         zx, zy, zz, zw,
         wx, wy, wz, ww;
} Gan_Matrix44_f;

#ifndef NDEBUG
/// Square matrix type, for setting and checking in debug mode
typedef enum { GAN_SYMMETRIC_MATRIX44_F, GAN_LOWER_TRI_MATRIX44_F }
 Gan_SquMatrix44Type_f;
#endif /* #ifndef NDEBUG */

/// Structure definition for square single precision 4x4 matrix
typedef struct Gan_SquMatrix44_f
{
#ifndef NDEBUG
   /* square matrix type, for setting and checking in debug mode */
   Gan_SquMatrix44Type_f type;
#endif /* #ifndef NDEBUG */

   /* matrix data */
   float xx,
         yx, yy,
         zx, zy, zz,
         wx, wy, wz, ww;
} Gan_SquMatrix44_f;

/* matrix definitions specific to 4x4 matrices */
#define GAN_ST44F_FILL(A,t,XX,YX,YY,ZX,ZY,ZZ,WX,WY,WZ,WW)\
    (GAN_TEST_OP1(A,Gan_SquMatrix44_f),gan_eval((A)->type=t),\
    GAN_FREP44L_A_C((A)->,=,(XX),(YX),(YY),(ZX),(ZY),(ZZ),(WX),(WY),(WZ),(WW)))
#define GAN_ST44F_IDENT(A,t)\
           (GAN_TEST_OP1(A,Gan_SquMatrix44_f),gan_eval((A)->type=t),\
            GAN_FREP44L_A_C((A)->,=,1.0,0.0,1.0,0.0,0.0,1.0,0.0,0.0,0.0,1.0))

/* generic matrix multiply operations */
#define GAN_MAT44F_MULT1D(a,b,c,p1,p2,p3,p4,q1,q2,q3,q4)\
 (a p1 = GAN_REP4_OP_AB(b,* c,xx,xy,xz,xw,q1,q2,q3,q4,+),\
  a p2 = GAN_REP4_OP_AB(b,* c,yx,yy,yz,yw,q1,q2,q3,q4,+),\
  a p3 = GAN_REP4_OP_AB(b,* c,zx,zy,zz,zw,q1,q2,q3,q4,+),\
  a p4 = GAN_REP4_OP_AB(b,* c,wx,wy,wz,ww,q1,q2,q3,q4,+))
#define GAN_MAT44TF_MULT1D(a,b,c,p1,p2,p3,p4,q1,q2,q3,q4)\
 (a p1 = GAN_REP4_OP_AB(b,* c,xx,yx,zx,wx,q1,q2,q3,q4,+),\
  a p2 = GAN_REP4_OP_AB(b,* c,xy,yy,zy,wy,q1,q2,q3,q4,+),\
  a p3 = GAN_REP4_OP_AB(b,* c,xz,yz,zz,wz,q1,q2,q3,q4,+),\
  a p4 = GAN_REP4_OP_AB(b,* c,xw,yw,zw,ww,q1,q2,q3,q4,+))

/* symmetric matrix multiply operation */
#define GAN_SYM44F_MULT1D(a,b,c,p1,p2,p3,p4,q1,q2,q3,q4)\
 (a p1 = GAN_REP4_OP_AB(b,* c,xx,yx,zx,wx,q1,q2,q3,q4,+),\
  a p2 = GAN_REP4_OP_AB(b,* c,yx,yy,zy,wy,q1,q2,q3,q4,+),\
  a p3 = GAN_REP4_OP_AB(b,* c,zx,zy,zz,wz,q1,q2,q3,q4,+),\
  a p4 = GAN_REP4_OP_AB(b,* c,wx,wy,wz,ww,q1,q2,q3,q4,+))

/* lower triangular matrix multiply operations */
#define GAN_LOW44F_MULT1D(a,b,c,p1,p2,p3,p4,q1,q2,q3,q4)\
 (a p4 = GAN_REP4_OP_AB(b,* c,wx,wy,wz,ww,q1,q2,q3,q4,+),\
  a p3 = GAN_REP3_OP_AB(b,* c,zx,zy,zz,q1,q2,q3,+),\
  a p2 = GAN_REP2_OP_AB(b,* c,yx,yy,q1,q2,+),\
  a p1 = GAN_REP1_OP_AB(b,* c,xx,q1,+))
#define GAN_LOW44IF_MULT1D(a,b,c,p1,p2,p3,p4,q1,q2,q3,q4)\
 (a p1 =  c q1 / b xx,\
  a p2 = (c q2 - b yx * a p1) / b yy,\
  a p3 = (c q3 - b zx * a p1 - b zy * a p2) / b zz,\
  a p4 = (c q4 - b wx * a p1 - b wy * a p2 - b wz * a p3) / b ww)

/* upper triangular matrix multiply operations */
#define GAN_UPP44F_MULT1D(a,b,c,p1,p2,p3,p4,q1,q2,q3,q4)\
 (a p1 = GAN_REP4_OP_AB(b,* c,xx,yx,zx,wx,q1,q2,q3,q4,+),\
  a p2 = GAN_REP3_OP_AB(b,* c,yy,zy,wy,q2,q3,q4,+),\
  a p3 = GAN_REP2_OP_AB(b,* c,zz,wz,q3,q4,+),\
  a p4 = GAN_REP1_OP_AB(b,* c,ww,q4,+))
#define GAN_UPP44IF_MULT1D(a,b,c,p1,p2,p3,p4,q1,q2,q3,q4)\
 (a p4 =  c q4 / b ww,\
  a p3 = (c q3 - b wz * a p4) / b zz,\
  a p2 = (c q2 - b zy * a p3 - b wy * a p4) / b yy,\
  a p1 = (c q1 - b yx * a p2 - b zx * a p3 - b wx * a p4) / b xx)

/* symmetric/lower triangular square matrix definitions */
#define GAN_ST44F_ZERO(A,t)\
           (GAN_TEST_OP1(A,Gan_SquMatrix44_f), gan_eval((A)->type=t),\
            GAN_REP44L_AS_C((A)->,=0.0))
#define GAN_ST44F_COPY(A,B,t)\
           (GAN_TEST_OP2(A,B,Gan_SquMatrix44_f,Gan_SquMatrix44_f),\
            assert((A)->type==t), gan_eval((B)->type=(A)->type),\
            GAN_REP44L_AA_C((B)->,=(A)->))
#define GAN_ST44F_SCALE(A,a,B,t)\
           (GAN_TEST_OP2(A,B,Gan_SquMatrix44_f,Gan_SquMatrix44_f),\
            assert((A)->type==t), gan_eval((B)->type=(A)->type),\
            GAN_REP44L_AAS_C((B)->,=(A)->,*(a)))
#define GAN_ST44F_DIVIDE(A,a,B,t)\
           (GAN_TEST_OP2(A,B,Gan_SquMatrix44_f,Gan_SquMatrix44_f),\
            assert((A)->type==t), gan_eval((B)->type=(A)->type),\
            GAN_REP44L_AAS_C((B)->,=(A)->,/(a)))
#define GAN_ST44F_NEGATE(A,B,t)\
           (GAN_TEST_OP2(A,B,Gan_SquMatrix44_f,Gan_SquMatrix44_f),\
            assert((A)->type==t), gan_eval((B)->type=(A)->type),\
            GAN_REP44L_AA_C((B)->,=-(A)->))
#define GAN_ST44F_ADD(A,B,C,t)\
         (GAN_TEST_OP3(A,B,C,Gan_SquMatrix44_f,Gan_SquMatrix44_f,Gan_SquMatrix44_f),\
          assert((A)->type==t && (B)->type==t), gan_eval((C)->type=(A)->type),\
          GAN_REP44L_AAA_C((C)->,=(A)->,+(B)->))
#define GAN_ST44F_SUB(A,B,C,t)\
         (GAN_TEST_OP3(A,B,C,Gan_SquMatrix44_f,Gan_SquMatrix44_f,Gan_SquMatrix44_f),\
          assert((A)->type==t && (B)->type==t), gan_eval((C)->type=(A)->type),\
          GAN_REP44L_AAA_C((C)->,=(A)->,-(B)->))

/* declare functions specific to generic 4x4 matrices */
#include <gandalf/linalg/4x4matf_noc.h>
#include <gandalf/linalg/squmatrixf_noc.h>
#include <gandalf/linalg/matrixf_noc.h>

/* declare functions specific to symmetric 4x4 matrices */
#include <gandalf/linalg/4x4symmatf_noc.h>
#include <gandalf/linalg/symmatrixf_noc.h>
#include <gandalf/linalg/squmatrixf_noc.h>
#include <gandalf/linalg/matrixf_noc.h>

/* declare functions specific to lower triangular 4x4 matrices */
#include <gandalf/linalg/4x4ltmatf_noc.h>
#include <gandalf/linalg/squmatrixf_noc.h>
#include <gandalf/linalg/matrixf_noc.h>

/* declare functions specific to upper triangular 4x4 matrices */
#include <gandalf/linalg/4x4ltmatTf_noc.h>
#include <gandalf/linalg/squmatrixf_noc.h>
#include <gandalf/linalg/matrixf_noc.h>

/**
 * \}
 */

/**
 * \}
 */

/**
 * \}
 */

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _GAN_4X4MATRIXF_H */

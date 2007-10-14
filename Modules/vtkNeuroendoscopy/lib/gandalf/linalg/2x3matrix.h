/**
 * File:          $RCSfile: 2x3matrix.h,v $
 * Module:        2x3 matrices (double precision)
 * Part of:       Gandalf Library
 *
 * Revision:      $Revision: 1.1.2.1 $
 * Last edited:   $Date: 2007/10/14 03:16:24 $
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

#ifndef _GAN_2X3MATRIX_H
#define _GAN_2X3MATRIX_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <gandalf/common/misc_defs.h>
#include <gandalf/linalg/repeat23_noc.h>
#include <gandalf/linalg/2vector.h>
#include <gandalf/linalg/3vector.h>
#include <gandalf/linalg/2x2matrix.h>
#include <gandalf/linalg/3x3matrix.h>
#include <gandalf/linalg/matvecf_macros_noc.h>
#include <gandalf/linalg/mat_gen.h>

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

/// Structure definition for double precision 2x3 matrix
typedef struct Gan_Matrix23
{
   double xx, xy, xz,
          yx, yy, yz;
} Gan_Matrix23;

/* generic matrix multiply operations */
#define GAN_MAT23_MULT1D(a,b,c,p1,p2,q1,q2,q3)\
 (a p1 = GAN_REP3_OP_AB(b,* c,xx,xy,xz,q1,q2,q3,+),\
  a p2 = GAN_REP3_OP_AB(b,* c,yx,yy,yz,q1,q2,q3,+))
#define GAN_MAT23T_MULT1D(a,b,c,p1,p2,p3,q1,q2)\
 (a p1 = GAN_REP2_OP_AB(b,* c,xx,yx,q1,q2,+),\
  a p2 = GAN_REP2_OP_AB(b,* c,xy,yy,q1,q2,+),\
  a p3 = GAN_REP2_OP_AB(b,* c,xz,yz,q1,q2,+))

/* declare functions specific to 2x3 matrices */
#include <gandalf/linalg/2x3mat_noc.h>
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

#endif /* #ifndef _GAN_2X3MATRIX_H */

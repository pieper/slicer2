/**
 * File:          $RCSfile: 2x2matrix.c,v $
 * Module:        2x2 matrices (double precision)
 * Part of:       Gandalf Library
 *
 * Revision:      $Revision: 1.1.2.1 $
 * Last edited:   $Date: 2007/10/14 03:16:21 $
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

#include <math.h>
#include <string.h>
#include <gandalf/linalg/2x2matrix.h>
#include <gandalf/common/misc_error.h>

/**
 * \addtogroup LinearAlgebra
 * \{
 */

/**
 * \addtogroup FixedSizeMatVec
 * \{
 */

/**
 * \defgroup FixedSizeMatrix Fixed Size Matrices
 * \{
 */

/* start off other groups */

/**
 * \defgroup FixedSizeMatrixIO Fixed Size Matrix I/O
 * \{
 */

/**
 * \}
 */

/**
 * \defgroup FixedSizeMatrixFill Fill a Fixed Size Matrix
 * \{
 */

/**
 * \}
 */

/**
 * \defgroup FixedSizeMatrixScale Multiply/Divide a Fixed Size Matrix by a Scalar
 * \{
 */

/**
 * \}
 */

/**
 * \defgroup FixedSizeMatrixInvert Invert a Fixed Size Matrix
 * \{
 */

/**
 * \}
 */

/**
 * \defgroup FixedSizeMatrixAdjoint Compute Adjoint of a Fixed Size Matrix
 * \{
 */

/**
 * \}
 */

/**
 * \defgroup FixedSizeMatrixExtract Extract Parts of a Fixed Size Matrix
 * \{
 */

/**
 * \}
 */

/**
 * \defgroup FixedSizeMatrixConvert Convert a Fixed Size Matrix
 * \{
 */

/**
 * \}
 */

/**
 * \defgroup FixedSizeMatrixTranspose Transpose a Fixed Size Matrix
 * \{
 */

/**
 * \}
 */

/**
 * \defgroup FixedSizeMatrixCopy Copy a Fixed Size Matrix
 * \{
 */

/**
 * \}
 */

/**
 * \defgroup FixedSizeMatrixAdd Add Fixed Size Matrices
 * \{
 */

/**
 * \}
 */

/**
 * \defgroup FixedSizeMatrixSubtract Subtract Fixed Size Matrices
 * \{
 */

/**
 * \}
 */

/**
 * \defgroup FixedSizeMatrixVectorProduct Fixed Size Matrix/Vector Product
 * \{
 */

/**
 * \}
 */

/**
 * \defgroup FixedSizeMatrixMatrixProduct Fixed Size Matrix/Matrix Product
 * \{
 */

/**
 * \}
 */

/**
 * \defgroup FixedSizeTripleMatrixProduct Fixed Size Matrix Triple Product
 * \{
 */

/**
 * \}
 */

/**
 * \defgroup FixedSizeMatrixNorms Fixed Size Matrix Norms
 * \{
 */

/**
 * \}
 */

/**
 * \defgroup FixedSizeMatrixCholesky Cholesky Factorisation of Fixed Size Matrix
 * \{
 */

/**
 * \}
 */

/**
 * \defgroup FixedSizeMatrixBuild Build a Fixed Size Matrix
 * \{
 */

/**
 * \}
 */

/* include the functions specific to generic, symmetric and triangular
   2x2 matrices. */
#include <gandalf/linalg/2x2mat_noc.c>
#include <gandalf/linalg/2x2symmat_noc.c>
#include <gandalf/linalg/2x2ltmat_noc.c>

/* define all the standard small matrix functions for generic 2x2 matrices */
#include <gandalf/linalg/2x2mat_noc.h>
#include <gandalf/linalg/squmatrixf_noc.c>
#include <gandalf/linalg/matrixf_noc.c>

/* define all the standard small matrix functions for symmetric 2x2 matrices */
#include <gandalf/linalg/2x2symmat_noc.h>
#include <gandalf/linalg/symmatrixf_noc.c>
#include <gandalf/linalg/squmatrixf_noc.c>
#include <gandalf/linalg/matrixf_noc.c>

/* define all the standard small matrix functions for lower triangular
   matrices */
#include <gandalf/linalg/2x2ltmat_noc.h>
#include <gandalf/linalg/squmatrixf_noc.c>
#include <gandalf/linalg/matrixf_noc.c>

/* define all the standard small matrix functions for upper triangular
   matrices */
#include <gandalf/linalg/2x2ltmatT_noc.h>
#include <gandalf/linalg/squmatrixf_noc.c>
#include <gandalf/linalg/matrixf_noc.c>

/**
 * \}
 */

/**
 * \}
 */

/**
 * \}
 */

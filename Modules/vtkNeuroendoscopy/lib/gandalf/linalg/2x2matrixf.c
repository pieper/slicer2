/**
 * File:          $RCSfile: 2x2matrixf.c,v $
 * Module:        2x2 matrices (single precision)
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
#include <gandalf/linalg/2x2matrixf.h>
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
 * \addtogroup FixedSizeMatrix
 * \{
 */

/* include the functions specific to generic, symmetric and triangular
   2x2 matrices. */
#include <gandalf/linalg/2x2matf_noc.c>
#include <gandalf/linalg/2x2symmatf_noc.c>
#include <gandalf/linalg/2x2ltmatf_noc.c>

/* define all the standard small matrix functions for generic 2x2 matrices */
#include <gandalf/linalg/2x2matf_noc.h>
#include <gandalf/linalg/squmatrixf_noc.c>
#include <gandalf/linalg/matrixf_noc.c>

/* define all the standard small matrix functions for symmetric 2x2 matrices */
#include <gandalf/linalg/2x2symmatf_noc.h>
#include <gandalf/linalg/symmatrixf_noc.c>
#include <gandalf/linalg/squmatrixf_noc.c>
#include <gandalf/linalg/matrixf_noc.c>

/* define all the standard small matrix functions for lower triangular
   matrices */
#include <gandalf/linalg/2x2ltmatf_noc.h>
#include <gandalf/linalg/squmatrixf_noc.c>
#include <gandalf/linalg/matrixf_noc.c>

/* define all the standard small matrix functions for upper triangular
   matrices */
#include <gandalf/linalg/2x2ltmatTf_noc.h>
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

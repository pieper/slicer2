/**
 * File:          $RCSfile: array.c,v $
 * Module:        Array module
 * Part of:       Gandalf Library
 *
 * Revision:      $Revision: 1.1.2.1 $
 * Last edited:   $Date: 2007/10/14 02:11:44 $
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

#include <string.h>
#include <gandalf/common/array.h>

/**
 * \addtogroup Common
 * \{
 */

/**
 * \defgroup CommonArray 1D Arrays
 * \{
 */

#define GAN_ARRAY_TYPE unsigned char
#define GAN_ARRAY_FILLFUNC gan_fill_array_uc
#define GAN_ARRAY_COPYFUNC gan_copy_array_uc
#include <gandalf/common/array_noc.c>
#undef GAN_ARRAY_TYPE
#undef GAN_ARRAY_FILLFUNC
#undef GAN_ARRAY_COPYFUNC

#define GAN_ARRAY_TYPE short
#define GAN_ARRAY_FILLFUNC gan_fill_array_s
#define GAN_ARRAY_COPYFUNC gan_copy_array_s
#include <gandalf/common/array_noc.c>
#undef GAN_ARRAY_TYPE
#undef GAN_ARRAY_FILLFUNC
#undef GAN_ARRAY_COPYFUNC

#define GAN_ARRAY_TYPE unsigned short
#define GAN_ARRAY_FILLFUNC gan_fill_array_us
#define GAN_ARRAY_COPYFUNC gan_copy_array_us
#include <gandalf/common/array_noc.c>
#undef GAN_ARRAY_TYPE
#undef GAN_ARRAY_FILLFUNC
#undef GAN_ARRAY_COPYFUNC

#define GAN_ARRAY_TYPE int
#define GAN_ARRAY_FILLFUNC gan_fill_array_i
#define GAN_ARRAY_COPYFUNC gan_copy_array_i
#include <gandalf/common/array_noc.c>
#undef GAN_ARRAY_TYPE
#undef GAN_ARRAY_FILLFUNC
#undef GAN_ARRAY_COPYFUNC

#define GAN_ARRAY_TYPE unsigned int
#define GAN_ARRAY_FILLFUNC gan_fill_array_ui
#define GAN_ARRAY_COPYFUNC gan_copy_array_ui
#include <gandalf/common/array_noc.c>
#undef GAN_ARRAY_TYPE
#undef GAN_ARRAY_FILLFUNC
#undef GAN_ARRAY_COPYFUNC

#define GAN_ARRAY_TYPE long
#define GAN_ARRAY_FILLFUNC gan_fill_array_l
#define GAN_ARRAY_COPYFUNC gan_copy_array_l
#include <gandalf/common/array_noc.c>
#undef GAN_ARRAY_TYPE
#undef GAN_ARRAY_FILLFUNC
#undef GAN_ARRAY_COPYFUNC

#define GAN_ARRAY_TYPE unsigned long
#define GAN_ARRAY_FILLFUNC gan_fill_array_ul
#define GAN_ARRAY_COPYFUNC gan_copy_array_ul
#include <gandalf/common/array_noc.c>
#undef GAN_ARRAY_TYPE
#undef GAN_ARRAY_FILLFUNC
#undef GAN_ARRAY_COPYFUNC

#define GAN_ARRAY_TYPE float
#define GAN_ARRAY_FILLFUNC gan_fill_array_f
#define GAN_ARRAY_COPYFUNC gan_copy_array_f
#include <gandalf/common/array_noc.c>
#undef GAN_ARRAY_TYPE
#undef GAN_ARRAY_FILLFUNC
#undef GAN_ARRAY_COPYFUNC

#define GAN_ARRAY_TYPE double
#define GAN_ARRAY_FILLFUNC gan_fill_array_d
#define GAN_ARRAY_COPYFUNC gan_copy_array_d
#include <gandalf/common/array_noc.c>
#undef GAN_ARRAY_TYPE
#undef GAN_ARRAY_FILLFUNC
#undef GAN_ARRAY_COPYFUNC

#define GAN_ARRAY_TYPE void *
#define GAN_ARRAY_FILLFUNC gan_fill_array_p
#define GAN_ARRAY_COPYFUNC gan_copy_array_p
#include <gandalf/common/array_noc.c>
#undef GAN_ARRAY_TYPE
#undef GAN_ARRAY_FILLFUNC
#undef GAN_ARRAY_COPYFUNC

/**
 * \}
 */

/**
 * \}
 */

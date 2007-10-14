/**
 * File:          $RCSfile: allocate.c,v $
 * Module:        Memory allocation module
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

#include <gandalf/common/allocate.h>
#include <stdarg.h>

/**
 * \addtogroup Common
 * \{
 */

/**
 * \defgroup CommonAllocate Common Allocation Routines
 * \{
 */

/**
 * \brief Frees a \c NULL terminated variable argument list of memory blocks.
 * \param ptr The first memory block to free
 * \param ... List of other blocks to free, terminated by \c NULL
 * \return No value.
 *
 * Invokes \c free() to free each memory block in the list of pointers
 * starting with \a ptr and ending with \c NULL .
 *
 * \sa gan_malloc_object(), gan_malloc_array().
 */
void
 gan_free_va ( void *ptr, ... )
{
   va_list ap;

   va_start ( ap, ptr );
   while ( ptr != NULL )
   {
      /* free next matrix */
      free ( ptr );

      /* get next pointer in list */
      ptr = va_arg ( ap, void * );
   }

   va_end ( ap );
}

/**
 * \}
 */

/**
 * \}
 */

/**
 * File:          $RCSfile: file_io.c,v $
 * Module:        File_Io module
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

#include <gandalf/common/file_io.h>
#include <gandalf/common/misc_error.h>
#include <stdio.h>

/**
 * \addtogroup Common
 * \{
 */

/**
 * \defgroup CommonFileIO File I/O
 * \{
 */

#define GAN_FILEIO_BLOCK_SIZE 0x10000

/**
 * \brief Copy a file
 * \param source Source file name
 * \param dest Destination file name
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 */
Gan_Bool gan_file_copy ( const char *source, const char *dest )
{
   char array[GAN_FILEIO_BLOCK_SIZE];
   FILE *fpi, *fpo;

   fpi = fopen(source, "r+b");
   if(fpi == NULL)
   {
      gan_err_flush_trace();
      gan_err_register("gan_file_copy", GAN_ERROR_OPENING_FILE, source);
      return GAN_FALSE;
   }

   fpo = fopen(dest, "w+b");
   if(fpo == NULL)
   {
      gan_err_flush_trace();
      gan_err_register("gan_file_copy", GAN_ERROR_OPENING_FILE, dest);
      return GAN_FALSE;
   }

   for(;;)
   {
      size_t nbytesread, nbyteswritten;

      nbytesread = fread(array, 1, GAN_FILEIO_BLOCK_SIZE, fpi);
      nbyteswritten = fwrite(array, 1, nbytesread, fpo);
      if(nbytesread != GAN_FILEIO_BLOCK_SIZE)
         break;
   }

   fclose(fpi);
   fclose(fpo);
   return GAN_TRUE;
}

/**
 * \}
 */

/**
 * \}
 */

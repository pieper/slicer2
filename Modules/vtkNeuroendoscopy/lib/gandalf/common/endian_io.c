/**
 * File:          $RCSfile: endian_io.c,v $
 * Module:        Little and big-endian I/O for architecture independent I/O
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

#include <gandalf/common/endian_io.h>
#include <gandalf/common/misc_error.h>
#include <gandalf/config.h>

/**
 * \addtogroup Common
 * \{
 */

/**
 * \defgroup CommonEndianIO I/O Dependent on Endian-ness of Architecture
 * \{
 */

#ifdef GAN_INT16

/**
 * \brief Reads an array of signed 16-bit integers from a file.
 * \param fp Pointer to a binary file opened for reading
 * \param ai16 Array of integers
 * \param nitems The number of integers to be read into the array
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Reads an array of signed 16-bit integers from a file in little-endian byte
 * order.
 */
Gan_Bool
 gan_fread_lendian_i16 ( FILE *fp, gan_int16 *ai16, size_t nitems )
{
   /* reverse bytes if on big-endian architecture */
#ifdef WORDS_BIGENDIAN
   {
      char ca[2];
      int i;

      for ( i = (int)nitems-1; i >= 0; i--, ai16++ )
      {
         /* read bytes */
         if ( fread ( ca, 2, 1, fp ) != 1 )
         {
            gan_err_flush_trace();
            gan_err_register ( "gan_fread_lendian_i16",
                               GAN_ERROR_READING_FROM_FILE, "" );
            return GAN_FALSE;
         }

         /* reverse bytes */
         ((char *)ai16)[1] = ca[0];
         ((char *)ai16)[0] = ca[1];
      }
   }
#else
   /* little-endian architecture: read bytes directly */
   if ( fread ( ai16, 2, nitems, fp ) != nitems )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_fread_lendian_i16",
                         GAN_ERROR_READING_FROM_FILE, "" );
      return GAN_FALSE;
   }
#endif

   /* success */
   return GAN_TRUE;
}

/**
 * \brief Writes an array of signed 16-bit integers to a file.
 * \param fp Pointer to a binary file opened for reading
 * \param ai16 Array of integers
 * \param nitems The number of integers to be written from the array
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Writes an array of signed 16-bit integers to a file in little-endian byte
 * order.
 */
Gan_Bool
 gan_fwrite_lendian_i16 ( FILE *fp, gan_int16 *ai16, size_t nitems )
{
   /* reverse bytes if on big-endian architecture */
#ifdef WORDS_BIGENDIAN
   {
      char ca[2];
      int i;

      for ( i = (int)nitems-1; i >= 0; i--, ai16++ )
      {
         /* build reversed bytes */
         ca[0] = ((char *)ai16)[1];
         ca[1] = ((char *)ai16)[0];

         /* write reversed bytes */
         if ( fwrite ( ca, 2, 1, fp ) != 1 )
         {
            gan_err_flush_trace();
            gan_err_register ( "gan_fwrite_lendian_i16",
                               GAN_ERROR_WRITING_TO_FILE, "" );
            return GAN_FALSE;
         }
      }
   }
#else
   /* little-endian architecture: write bytes directly */
   if ( fwrite ( ai16, 2, nitems, fp ) != nitems )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_fwrite_lendian_i16",
                         GAN_ERROR_WRITING_TO_FILE, "" );
      return GAN_FALSE;
   }
#endif

   /* success */
   return GAN_TRUE;
}

/**
 * \brief Reads an array of unsigned 16-bit integers from a file.
 * \param fp Pointer to a binary file opened for reading
 * \param aui16 Array of integers
 * \param nitems The number of integers to be read into the array
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Reads an array of unsigned 16-bit integers from a file in little-endian byte
 * order.
 */
Gan_Bool
 gan_fread_lendian_ui16 ( FILE *fp, gan_uint16 *aui16, size_t nitems )
{
   /* reverse bytes if on big-endian architecture */
#ifdef WORDS_BIGENDIAN
   {
      char ca[2];
      int i;

      for ( i = (int)nitems-1; i >= 0; i--, aui16++ )
      {
         /* read bytes */
         if ( fread ( ca, 2, 1, fp ) != 1 )
         {
            gan_err_flush_trace();
            gan_err_register ( "gan_fread_lendian_ui16",
                               GAN_ERROR_READING_FROM_FILE, "" );
            return GAN_FALSE;
         }

         /* reverse bytes */
         ((char *)aui16)[1] = ca[0];
         ((char *)aui16)[0] = ca[1];
      }
   }
#else
   /* little-endian architecture: read bytes directly */
   if ( fread ( aui16, 2, nitems, fp ) != nitems )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_fread_lendian_ui16",
                         GAN_ERROR_READING_FROM_FILE, "" );
      return GAN_FALSE;
   }
#endif

   /* success */
   return GAN_TRUE;
}

/**
 * \brief Writes an array of unsigned 16-bit integers to a file
 * \param fp Pointer to a binary file opened for reading
 * \param aui16 Array of integers
 * \param nitems The number of integers to be written from the array
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Writes an array of unsigned 16-bit integers to a file in little-endian byte
 * order.
 */
Gan_Bool
 gan_fwrite_lendian_ui16 ( FILE *fp, gan_uint16 *aui16, size_t nitems )
{
   /* reverse bytes if on big-endian architecture */
#ifdef WORDS_BIGENDIAN
   {
      char ca[2];
      int i;

      for ( i = (int)nitems-1; i >= 0; i--, aui16++ )
      {
         /* build reversed bytes */
         ca[0] = ((char *)aui16)[1];
         ca[1] = ((char *)aui16)[0];

         /* write reversed bytes */
         if ( fwrite ( ca, 2, 1, fp ) != 1 )
         {
            gan_err_flush_trace();
            gan_err_register ( "gan_fwrite_lendian_ui16",
                               GAN_ERROR_WRITING_TO_FILE, "" );
            return GAN_FALSE;
         }
      }
   }
#else
   /* little-endian architecture: write bytes directly */
   if ( fwrite ( aui16, 2, nitems, fp ) != nitems )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_fwrite_lendian_ui16",
                         GAN_ERROR_WRITING_TO_FILE, "" );
      return GAN_FALSE;
   }
#endif

   /* success */
   return GAN_TRUE;
}

#endif /* #ifdef GAN_INT16 */

#ifdef GAN_INT32

/**
 * \brief Reads an array of signed 32-bit integers from a file
 * \param fp Pointer to a binary file opened for reading
 * \param ai32 Array of integers
 * \param nitems The number of integers to be read into the array
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Reads an array of signed 32-bit integers from a file in little-endian byte
 * order.
 */
Gan_Bool
 gan_fread_lendian_i32 ( FILE *fp, gan_int32 *ai32, size_t nitems )
{
   /* reverse bytes if on big-endian architecture */
#ifdef WORDS_BIGENDIAN
   {
      char ca[4];
      int i;

      for ( i = (int)nitems-1; i >= 0; i--, ai32++ )
      {
         /* read bytes */
         if ( fread ( ca, 4, 1, fp ) != 1 )
         {
            gan_err_flush_trace();
            gan_err_register ( "gan_fread_lendian_i32",
                               GAN_ERROR_READING_FROM_FILE, "" );
            return GAN_FALSE;
         }

         /* reverse bytes */
         ((char *)ai32)[3] = ca[0];
         ((char *)ai32)[2] = ca[1];
         ((char *)ai32)[1] = ca[2];
         ((char *)ai32)[0] = ca[3];
      }
   }
#else
   /* little-endian architecture: read bytes directly */
   if ( fread ( ai32, 4, nitems, fp ) != nitems )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_fread_lendian_i32",
                         GAN_ERROR_READING_FROM_FILE, "" );
      return GAN_FALSE;
   }
#endif

   /* success */
   return GAN_TRUE;
}

/**
 * \brief Writes an array of signed 32-bit integers to a file
 * \param fp Pointer to a binary file opened for reading
 * \param ai32 Array of integers
 * \param nitems The number of integers to be written from the array
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Writes an array of signed 32-bit integers to a file in little-endian byte
 * order.
 */
Gan_Bool
 gan_fwrite_lendian_i32 ( FILE *fp, gan_int32 *ai32, size_t nitems )
{
   /* reverse bytes if on big-endian architecture */
#ifdef WORDS_BIGENDIAN
   {
      char ca[4];
      int i;

      for ( i = (int)nitems-1; i >= 0; i--, ai32++ )
      {
         /* build reversed bytes */
         ca[0] = ((char *)ai32)[3];
         ca[1] = ((char *)ai32)[2];
         ca[2] = ((char *)ai32)[1];
         ca[3] = ((char *)ai32)[0];

         /* write reversed bytes */
         if ( fwrite ( ca, 4, 1, fp ) != 1 )
         {
            gan_err_flush_trace();
            gan_err_register ( "gan_fwrite_lendian_i32",
                               GAN_ERROR_WRITING_TO_FILE, "" );
            return GAN_FALSE;
         }
      }
   }
#else
   /* little-endian architecture: write bytes directly */
   if ( fwrite ( ai32, 4, nitems, fp ) != nitems )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_fwrite_lendian_i32",
                         GAN_ERROR_WRITING_TO_FILE, "" );
      return GAN_FALSE;
   }
#endif

   /* success */
   return GAN_TRUE;
}

/**
 * \brief Reads an array of unsigned 32-bit integers from a file
 * \param fp Pointer to a binary file opened for reading
 * \param aui32 Array of integers
 * \param nitems The number of integers to be read into the array
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Reads an array of unsigned 32-bit integers from a file in little-endian byte
 * order.
 */
Gan_Bool
 gan_fread_lendian_ui32 ( FILE *fp, gan_uint32 *aui32, size_t nitems )
{
   /* reverse bytes if on big-endian architecture */
#ifdef WORDS_BIGENDIAN
   {
      char ca[4];
      int i;

      for ( i = (int)nitems-1; i >= 0; i--, aui32++ )
      {
         /* read bytes */
         if ( fread ( ca, 4, 1, fp ) != 1 )
         {
            gan_err_flush_trace();
            gan_err_register ( "gan_fread_lendian_ui32",
                               GAN_ERROR_READING_FROM_FILE, "" );
            return GAN_FALSE;
         }

         /* reverse bytes */
         ((char *)aui32)[3] = ca[0];
         ((char *)aui32)[2] = ca[1];
         ((char *)aui32)[1] = ca[2];
         ((char *)aui32)[0] = ca[3];
      }
   }
#else
   /* little-endian architecture: read bytes directly */
   if ( fread ( aui32, 4, nitems, fp ) != nitems )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_fread_lendian_ui32",
                         GAN_ERROR_READING_FROM_FILE, "" );
      return GAN_FALSE;
   }
#endif

   /* success */
   return GAN_TRUE;
}

/**
 * \brief Writes an array of unsigned 32-bit integers to a file
 * \param fp Pointer to a binary file opened for reading
 * \param aui32 Array of integers
 * \param nitems The number of integers to be written from the array
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Writes an array of unsigned 32-bit integers to a file in little-endian byte
 * order.
 */
Gan_Bool
 gan_fwrite_lendian_ui32 ( FILE *fp, gan_uint32 *aui32, size_t nitems )
{
   /* reverse bytes if on big-endian architecture */
#ifdef WORDS_BIGENDIAN
   {
      char ca[4];
      int i;

      for ( i = (int)nitems-1; i >= 0; i--, aui32++ )
      {
         /* build reversed bytes */
         ca[0] = ((char *)aui32)[3];
         ca[1] = ((char *)aui32)[2];
         ca[2] = ((char *)aui32)[1];
         ca[3] = ((char *)aui32)[0];

         /* write reversed bytes */
         if ( fwrite ( ca, 4, 1, fp ) != 1 )
         {
            gan_err_flush_trace();
            gan_err_register ( "gan_fwrite_lendian_ui32",
                               GAN_ERROR_WRITING_TO_FILE, "" );
            return GAN_FALSE;
         }
      }
   }
#else
   /* little-endian architecture: write bytes directly */
   if ( fwrite ( aui32, 4, nitems, fp ) != nitems )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_fwrite_lendian_ui32",
                         GAN_ERROR_WRITING_TO_FILE, "" );
      return GAN_FALSE;
   }
#endif

   /* success */
   return GAN_TRUE;
}

#endif /* #ifdef GAN_INT32 */

#ifdef GAN_INT64

/**
 * \brief Reads an array of signed 64-bit integers from a file
 * \param fp Pointer to a binary file opened for reading
 * \param ai64 Array of integers
 * \param nitems The number of integers to be read into the array
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Reads an array of unsigned 64-bit integers from a file in little-endian byte
 * order.
 */
Gan_Bool
 gan_fread_lendian_i64 ( FILE *fp, gan_int64 *ai64, size_t nitems )
{
   /* reverse bytes if on big-endian architecture */
#ifdef WORDS_BIGENDIAN
   {
      char ca[8];
      int i;

      for ( i = (int)nitems-1; i >= 0; i--, ai64++ )
      {
         /* read bytes */
         if ( fread ( ca, 8, 1, fp ) != 1 )
         {
            gan_err_flush_trace();
            gan_err_register ( "gan_fread_lendian_i64",
                               GAN_ERROR_READING_FROM_FILE, "" );
            return GAN_FALSE;
         }

         /* reverse bytes */
         ((char *)ai64)[7] = ca[0];
         ((char *)ai64)[6] = ca[1];
         ((char *)ai64)[5] = ca[2];
         ((char *)ai64)[4] = ca[3];
         ((char *)ai64)[3] = ca[4];
         ((char *)ai64)[2] = ca[5];
         ((char *)ai64)[1] = ca[6];
         ((char *)ai64)[0] = ca[7];
      }
   }
#else
   /* little-endian architecture: read bytes directly */
   if ( fread ( ai64, 8, nitems, fp ) != nitems )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_fread_lendian_i64",
                         GAN_ERROR_READING_FROM_FILE, "" );
      return GAN_FALSE;
   }
#endif

   /* success */
   return GAN_TRUE;
}

/**
 * \brief Writes an array of signed 64-bit integers to a file
 * \param fp Pointer to a binary file opened for reading
 * \param ai64 Array of integers
 * \param nitems The number of integers to be written from the array
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Writes an array of signed 64-bit integers to a file in little-endian byte
 * order.
 */
Gan_Bool
 gan_fwrite_lendian_i64 ( FILE *fp, gan_int64 *ai64, size_t nitems )
{
   /* reverse bytes if on big-endian architecture */
#ifdef WORDS_BIGENDIAN
   {
      char ca[8];
      int i;

      for ( i = (int)nitems-1; i >= 0; i--, ai64++ )
      {
         /* build reversed bytes */
         ca[0] = ((char *)ai64)[7];
         ca[1] = ((char *)ai64)[6];
         ca[2] = ((char *)ai64)[5];
         ca[3] = ((char *)ai64)[4];
         ca[4] = ((char *)ai64)[3];
         ca[5] = ((char *)ai64)[2];
         ca[6] = ((char *)ai64)[1];
         ca[7] = ((char *)ai64)[0];

         /* write reversed bytes */
         if ( fwrite ( ca, 8, 1, fp ) != 1 )
         {
            gan_err_flush_trace();
            gan_err_register ( "gan_fwrite_lendian_i64",
                               GAN_ERROR_WRITING_TO_FILE, "" );
            return GAN_FALSE;
         }
      }
   }
#else
   /* little-endian architecture: write bytes directly */
   if ( fwrite ( ai64, 8, nitems, fp ) != nitems )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_fwrite_lendian_i64",
                         GAN_ERROR_WRITING_TO_FILE, "" );
      return GAN_FALSE;
   }
#endif

   /* success */
   return GAN_TRUE;
}

/**
 * \brief Reads an array of unsigned 64-bit integers from a file
 * \param fp Pointer to a binary file opened for reading
 * \param aui64 Array of integers
 * \param nitems The number of integers to be read into the array
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Reads an array of unsigned 64-bit integers from a file in little-endian byte
 * order.
 */
Gan_Bool
 gan_fread_lendian_ui64 ( FILE *fp, gan_uint64 *aui64, size_t nitems )
{
   /* reverse bytes if on big-endian architecture */
#ifdef WORDS_BIGENDIAN
   {
      char ca[8];
      int i;

      for ( i = (int)nitems-1; i >= 0; i--, aui64++ )
      {
         /* read bytes */
         if ( fread ( ca, 8, 1, fp ) != 1 )
         {
            gan_err_flush_trace();
            gan_err_register ( "gan_fread_lendian_ui64",
                               GAN_ERROR_READING_FROM_FILE, "" );
            return GAN_FALSE;
         }

         /* reverse bytes */
         ((char *)aui64)[7] = ca[0];
         ((char *)aui64)[6] = ca[1];
         ((char *)aui64)[5] = ca[2];
         ((char *)aui64)[4] = ca[3];
         ((char *)aui64)[3] = ca[4];
         ((char *)aui64)[2] = ca[5];
         ((char *)aui64)[1] = ca[6];
         ((char *)aui64)[0] = ca[7];
      }
   }
#else
   /* little-endian architecture: read bytes directly */
   if ( fread ( aui64, 8, nitems, fp ) != nitems )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_fread_lendian_ui64",
                         GAN_ERROR_READING_FROM_FILE, "" );
      return GAN_FALSE;
   }
#endif

   /* success */
   return GAN_TRUE;
}

/**
 * \brief Writes an array of unsigned 64-bit integers to a file
 * \param fp Pointer to a binary file opened for reading
 * \param aui64 Array of integers
 * \param nitems The number of integers to be written from the array
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Writes an array of unsigned 64-bit integers to a file in little-endian byte
 * order.
 */
Gan_Bool
 gan_fwrite_lendian_ui64 ( FILE *fp, gan_uint64 *aui64, size_t nitems )
{
   /* reverse bytes if on big-endian architecture */
#ifdef WORDS_BIGENDIAN
   {
      char ca[8];
      int i;

      for ( i = (int)nitems-1; i >= 0; i--, aui64++ )
      {
         /* build reversed bytes */
         ca[0] = ((char *)aui64)[7];
         ca[1] = ((char *)aui64)[6];
         ca[2] = ((char *)aui64)[5];
         ca[3] = ((char *)aui64)[4];
         ca[4] = ((char *)aui64)[3];
         ca[5] = ((char *)aui64)[2];
         ca[6] = ((char *)aui64)[1];
         ca[7] = ((char *)aui64)[0];

         /* write reversed bytes */
         if ( fwrite ( ca, 8, 1, fp ) != 1 )
         {
            gan_err_flush_trace();
            gan_err_register ( "gan_fwrite_lendian_ui64",
                               GAN_ERROR_WRITING_TO_FILE, "" );
            return GAN_FALSE;
         }
      }
   }
#else
   /* little-endian architecture: write bytes directly */
   if ( fwrite ( aui64, 8, nitems, fp ) != nitems )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_fwrite_lendian_ui64",
                         GAN_ERROR_WRITING_TO_FILE, "" );
      return GAN_FALSE;
   }
#endif

   /* success */
   return GAN_TRUE;
}

#endif /* #ifdef GAN_INT64 */

#ifdef GAN_FLOAT32

/**
 * \brief Reads an array of 32-bit floats from a file
 * \param fp Pointer to a binary file opened for reading
 * \param af32 Array of floats
 * \param nitems The number of values to be read into the array
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Reads an array of 32-bit floats from a file in little-endian byte order.
 */
Gan_Bool
 gan_fread_lendian_f32 ( FILE *fp, gan_float32 *af32, size_t nitems )
{
   
   /* reverse bytes if on big-endian architecture */
#ifdef WORDS_BIGENDIAN
   {
      char ca[4];
      int i;

      for ( i = (int)nitems-1; i >= 0; i--, af32++ )
      {
         /* read bytes */
         if ( fread ( ca, sizeof(float), 1, fp ) != 1 )
         {
            gan_err_flush_trace();
            gan_err_register ( "gan_fread_lendian_f32",
                               GAN_ERROR_READING_FROM_FILE, "" );
            return GAN_FALSE;
         }

         /* reverse bytes */
         ((char *)af32)[3] = ca[0];
         ((char *)af32)[2] = ca[1];
         ((char *)af32)[1] = ca[2];
         ((char *)af32)[0] = ca[3];
      }
   }
#else
   /* little-endian architecture: read bytes directly */
   if ( fread ( af32, sizeof(gan_float32), nitems, fp ) != nitems )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_fread_lendian_f32",
                         GAN_ERROR_READING_FROM_FILE, "" );
      return GAN_FALSE;
   }
#endif

   /* success */
   return GAN_TRUE;
}

/**
 * \brief Writes an array of 32-bit floats to a file
 * \param fp Pointer to a binary file opened for writing
 * \param af32 Array of floats
 * \param nitems The number of values to be written from the array
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Writes an array of 32-bit floats to a file in little-endian byte order.
 */
Gan_Bool
 gan_fwrite_lendian_f32 ( FILE *fp, gan_float32 *af32, size_t nitems )
{
   
   /* reverse bytes if on big-endian architecture */
#ifdef WORDS_BIGENDIAN
   {
      char ca[4];
      int i;

      for ( i = (int)nitems-1; i >= 0; i--, af32++ )
      {
         /* build reversed bytes */
         ca[0] = ((char *)af32)[3];
         ca[1] = ((char *)af32)[2];
         ca[2] = ((char *)af32)[1];
         ca[3] = ((char *)af32)[0];

         /* write reversed bytes */
         if ( fwrite ( ca, sizeof(float), 1, fp ) != 1 )
         {
            gan_err_flush_trace();
            gan_err_register ( "gan_fwrite_lendian_f32",
                               GAN_ERROR_WRITING_TO_FILE, "" );
            return GAN_FALSE;
         }
      }
   }
#else
   /* little-endian architecture: write bytes directly */
   if ( fwrite ( af32, sizeof(gan_float32), nitems, fp ) != nitems )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_fwrite_lendian_f32",
                         GAN_ERROR_WRITING_TO_FILE, "" );
      return GAN_FALSE;
   }
#endif

   /* success */
   return GAN_TRUE;
}

#endif /* #ifdef GAN_FLOAT32 */

#ifdef GAN_FLOAT64

/**
 * \brief Reads an array of 64-bit floats from a file
 * \param fp Pointer to a binary file opened for reading
 * \param af64 Array of 64-bit floats
 * \param nitems The number of values to be read into the array
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Reads an array of 64-bit floats from a file in little-endian byte order.
 */
Gan_Bool
 gan_fread_lendian_f64 ( FILE *fp, gan_float64 *af64, size_t nitems )
{
   /* reverse bytes if on big-endian architecture */
#ifdef WORDS_BIGENDIAN
   {
      char ca[8];
      int i;

      for ( i = (int)nitems-1; i >= 0; i--, af64++ )
      {
         /* read bytes */
         if ( fread ( ca, sizeof(gan_float64), 1, fp ) != 1 )
         {
            gan_err_flush_trace();
            gan_err_register ( "gan_fread_lendian_f64",
                               GAN_ERROR_READING_FROM_FILE, "" );
            return GAN_FALSE;
         }

         /* reverse bytes */
         ((char *)af64)[7] = ca[0];
         ((char *)af64)[6] = ca[1];
         ((char *)af64)[5] = ca[2];
         ((char *)af64)[4] = ca[3];
         ((char *)af64)[3] = ca[4];
         ((char *)af64)[2] = ca[5];
         ((char *)af64)[1] = ca[6];
         ((char *)af64)[0] = ca[7];
      }
   }
#else
   /* little-endian architecture: read bytes directly */
   if ( fread ( af64, sizeof(gan_float64), nitems, fp ) != nitems )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_fread_lendian_f64",
                         GAN_ERROR_READING_FROM_FILE, "" );
      return GAN_FALSE;
   }
#endif

   /* success */
   return GAN_TRUE;
}

/**
 * \brief Writes an array of 64-bit floats to a file.
 * \param fp Pointer to a binary file opened for writing
 * \param af64 Array of 64-bit floats
 * \param nitems The number of values to be written from the array
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Writes an array of 64-bit floats to a file in little-endian byte order.
 */
Gan_Bool
 gan_fwrite_lendian_f64 ( FILE *fp, gan_float64 *af64, size_t nitems )
{
   
   /* reverse bytes if on big-endian architecture */
#ifdef WORDS_BIGENDIAN
   {
      char ca[8];
      int i;

      for ( i = (int)nitems-1; i >= 0; i--, af64++ )
      {
         /* build reversed bytes */
         ca[0] = ((char *)af64)[7];
         ca[1] = ((char *)af64)[6];
         ca[2] = ((char *)af64)[5];
         ca[3] = ((char *)af64)[4];
         ca[4] = ((char *)af64)[3];
         ca[5] = ((char *)af64)[2];
         ca[6] = ((char *)af64)[1];
         ca[7] = ((char *)af64)[0];

         /* write reversed bytes */
         if ( fwrite ( ca, sizeof(gan_float64), 1, fp ) != 1 )
         {
            gan_err_flush_trace();
            gan_err_register ( "gan_fwrite_lendian_f64",
                               GAN_ERROR_WRITING_TO_FILE, "" );
            return GAN_FALSE;
         }
      }
   }
#else
   /* little-endian architecture: write bytes directly */
   if ( fwrite ( af64, sizeof(gan_float64), nitems, fp ) != nitems )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_fwrite_lendian_f64",
                         GAN_ERROR_WRITING_TO_FILE, "" );
      return GAN_FALSE;
   }
#endif

   /* success */
   return GAN_TRUE;
}

#endif /* #ifdef GAN_FLOAT64 */

/**
 * \}
 */

/**
 * \}
 */

/**
 * File:          $RCSfile: 4vector.c,v $
 * Module:        Size 4 vectors (double precision)
 * Part of:       Gandalf Library
 *
 * Revision:      $Revision: 1.1.2.1 $
 * Last edited:   $Date: 2007/10/14 03:16:22 $
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
#include <gandalf/linalg/3vector.h>
#define GAN_KEEP_VECDEFS
#include <gandalf/linalg/4vector.h>
#undef GAN_KEEP_VECDEFS
#include <gandalf/common/misc_defs.h>
#include <gandalf/common/misc_error.h>
#include <string.h>

/**
 * \addtogroup LinearAlgebra
 * \{
 */

/**
 * \addtogroup FixedSizeMatVec
 * \{
 */

/**
 * \addtogroup FixedSizeVector
 * \{
 */

/* define all the standard small vector functions */
#include <gandalf/linalg/vectorf_noc.c>

/**
 * \addtogroup FixedSizeVectorIO
 * \{
 */

/**
 * \brief Print 4-vector to file
 *
 * Print 4-vector \a p to file pointer \a fp, with prefix string \a prefix,
 * indentation \a indent and floating-point format \a fmt.
 *
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 */
Gan_Bool
 gan_vec4_fprint ( FILE *fp, const Gan_Vector4 *p,
                   const char *prefix, int indent,  const char *fmt )
{
   int i;

   /* print indentation */
   for ( i = indent-1; i >= 0; i-- ) fprintf ( fp, " " );
   fprintf ( fp, "%s: ", prefix ); fprintf ( fp, fmt, p->x );
   fprintf ( fp, " " );            fprintf ( fp, fmt, p->y );
   fprintf ( fp, " " );            fprintf ( fp, fmt, p->z );
   fprintf ( fp, " " );            fprintf ( fp, fmt, p->w );
   fprintf ( fp, "\n" );

   /* success */
   return GAN_TRUE;
}

/**
 * \brief Read 4-vector from file
 *
 * Read 4-vector \a p from file pointer \a fp. The prefix string for
 * the vector is read from the file into the \a prefix string, up to the
 * maximum length \a prefix_len of the \a prefix string. Any remaining
 * characters after \a prefix has been filled are ignored. Pass \c NULL for
 * \a prefix and zero for \a prefix_len to ignore the prefix string.
 *
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 */
Gan_Bool
 gan_vec4_fscanf ( FILE *fp, Gan_Vector4 *p, char *prefix, int prefix_len )
{
   int ch, result;

   /* read indentation */
   for(;;)
      if ( (ch = getc(fp)) != ' ' || ch == EOF )
         break;

   /* end of file means corrupted file */
   if ( ch == EOF )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_vec4_fscanf", GAN_ERROR_CORRUPTED_FILE, "" );
      return GAN_FALSE;
   }

   /* rewind one character to reread the prefix string */
   fseek ( fp, -1, SEEK_CUR );

   /* read prefix string, terminated by ":" */
   for ( prefix_len--; prefix_len > 0; prefix_len-- )
   {
      ch = getc(fp);
      if ( ch == EOF || ch == ':' ) break;
      if ( prefix != NULL ) *prefix++ = (char)ch;
   }

   if ( ch != ':' )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_vec4_fscanf", GAN_ERROR_CORRUPTED_FILE, "" );
      return GAN_FALSE;
   }

   /* terminate string */
   if ( prefix != NULL ) *prefix = '\0';

   /* read rest of string if necessary */
   if ( prefix_len == 0 )
   {
      for(;;)
      {
         ch = getc(fp);
         if ( ch == EOF || ch == ':' ) break;
      }

      gan_assert ( ch == ':', "corrupted file in gan_vec4_fscanf()" );
   }

   /* read vector data */
   result = fscanf ( fp, "%lf%lf%lf%lf\n", &p->x, &p->y, &p->z, &p->w );
   if ( result != 4 )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_vec4_fscanf", GAN_ERROR_CORRUPTED_FILE, "" );
      return GAN_FALSE;
   }

   /* read end-of-line character */
   result = getc(fp);
   assert ( result == '\n' );

   /* success */
   return GAN_TRUE;
}

/**
 * \}
 */

/**
 * \addtogroup FixedSizeVectorFill
 * \{
 */

/**
 * \brief Return 4-vector filled with values.
 *
 * Fill 4-vector with values:
 * \f[
 *   \left(\begin{array}{c} X \\ Y \\ Z \\ W \end{array}\right)
 * \f]
 *
 * \return filled 4-vector.
 */
Gan_Vector4
 gan_vec4_fill_s ( double X, double Y, double Z, double W )
{
   Gan_Vector4 p;

   (void)gan_vec4_fill_q ( &p, X, Y, Z, W );
   return p;
}

/**
 * \}
 */

/**
 * \addtogroup FixedSizeVectorExtract
 * \{
 */

/**
 * \brief Extract top 3-vector part of 4-vector.
 *
 * Extract and return top 3-vector part of 4-vector \a p.
 */
Gan_Vector3
 gan_vec4_get_v3t_s ( const Gan_Vector4 *p )
{
   Gan_Vector3 q;

   (void)gan_vec4_get_v3t_q ( p, &q );
   return q;
}

/**
 * \}
 */

/**
 * \addtogroup FixedSizeVectorBuild
 * \{
 */

/**
 * \brief Build 4-vector from 3-vector and scalar.
 *
 * Build 4-vector from 3-vector \a q and scalar \a s,
 * which are respectively inserted in the top and bottom parts of the 4-vector,
 * which is built as
 * \f[
 *    \left(\begin{array}{c} q \\ s \end{array}\right)
 * \f]
 *
 * \return Result 4-vector.
 */
Gan_Vector4
 gan_vec4_set_parts_s ( const Gan_Vector3 *q, double s )
{
   Gan_Vector4 p;

   (void)gan_vec4_set_parts_q ( &p, q, s );
   return p;
}

/**
 * \brief Macro: Build 4-vector from two 2-vectors.
 *
 * Build 4-vector from 2-vectors \a q and \a r, which are respectively
 * inserted in the top and bottom parts of the 4-vector, which is built as
 * \f[
 *   \left(\begin{array}{c} q \\ r \end{array}\right)
 * \f]
 *
 * \return Result 4-vector.
 */
Gan_Vector4
 gan_vec4_set_blocks_s ( const Gan_Vector2 *q, const Gan_Vector2 *r )
{
   Gan_Vector4 p;

   (void)gan_vec4_set_blocks_q ( &p, q, r );
   return p;
}

/**
 * \}
 */

/**
 * \addtogroup FixedSizeVectorConvert
 * \{
 */

/**
 * \brief Convert generic vector to 4-vector structure.
 *
 * Convert generic vector \a x to 4-vector structure \a a.
 *
 * \return Pointer to result 4-vector \a a.
 */
Gan_Vector4 *
 gan_vec4_from_vec_q ( const Gan_Vector *x, Gan_Vector4 *a )
{
   if ( !gan_vec_read_va ( x, 4, &a->x, &a->y, &a->z, &a->w ) )
   {
      gan_err_register ( "gan_vec4_from_vec_q", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   return a;
}

/**
 * \}
 */

/**
 * \}
 */

/**
 * \}
 */

/**
 * \}
 */

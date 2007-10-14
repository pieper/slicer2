/*
 * File:          $RCSfile: array_noc.c,v $
 * Module:        Array module
 * Part of:       Gandalf Library
 *
 * Revision:      $Revision: 1.1.2.1 $
 * Last edited:   $Date: 2007/10/14 02:11:44 $
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
 * \brief Fill array of objects.
 * \return No value.
 *
 * Fill array \a arr of objects of type #GAN_ARRAY_TYPE with a constant value
 * \a val.
 */
void
 GAN_ARRAY_FILLFUNC ( GAN_ARRAY_TYPE *arr, unsigned long size, long stride,
                      GAN_ARRAY_TYPE val )
{
   long i;

   if ( stride == 1 )
      for ( i = (long)size-1; i >= 0; i-- )
         *arr++ = val;
   else
      for ( i = (long)size-1; i >= 0; i--, arr += stride )
         *arr = val;
}

/**
 * \brief Copy array of objects.
 * \return No value.
 *
 * \brief Copy \a source array of objects of type #GAN_ARRAY_TYPE to \a dest
 * array. The stride arguments indicate the spacing between elements of the
 * arrays.
 */
void
 GAN_ARRAY_COPYFUNC( const GAN_ARRAY_TYPE *source, long s_stride, unsigned long size,
                           GAN_ARRAY_TYPE *dest,   long d_stride )
{
   const GAN_ARRAY_TYPE *send;

   assert ( source != (const GAN_ARRAY_TYPE *)dest || s_stride == d_stride );
   if ( s_stride == 1 && d_stride == 1 )
      memcpy ( (void *) dest, (const void *) source, size*sizeof(*source) );
   else
      for ( send = source + size; source != send; source += s_stride,
            dest += d_stride )
         *dest = *((GAN_ARRAY_TYPE*)source);
}

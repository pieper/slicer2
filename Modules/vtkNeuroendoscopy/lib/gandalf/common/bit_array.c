/**
 * File:          $RCSfile: bit_array.c,v $
 * Module:        Binary array module
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

#include <gandalf/common/bit_array.h>
#include <gandalf/common/allocate.h>
#include <gandalf/common/misc_error.h>
#include <stdlib.h>
#include <stdarg.h>
#include <limits.h>
#include <string.h>

/**
 * \addtogroup Common
 * \{
 */

/**
 * \addtogroup CommonArray
 * \{
 */

/**
 * \brief Form new bit array using pre-allocated data array.
 * \param ba Existing bit array or \c NULL
 * \param data Block of memory to use for bit array
 * \param data_words Number of words in \a data array
 * \param no_bits Number of bits in bit array
 * \return Formed bit array, or \c NULL on error.
 *
 * Form new bit array of size \a no_bits using pre-allocated data array
 * \a data. If \a data is not \c NULL, the number of \a data_words must be
 * enough to store \a no_bits bits.
 */
Gan_BitArray *
 gan_bit_array_form_data ( Gan_BitArray *ba,
                           Gan_BitWord *data, unsigned data_words,
                           unsigned int no_bits )
{
   if ( ba == NULL )
   {
      ba = gan_malloc_object ( struct Gan_BitArray );
      if ( ba == NULL )
      {
         gan_err_flush_trace();
         gan_err_register_with_number ( "gan_bit_array_form_data", GAN_ERROR_MALLOC_FAILED, "", sizeof(struct Gan_BitArray) );
         return NULL;
      }
      
      ba->alloc = GAN_TRUE;
   }
   else
      ba->alloc = GAN_FALSE;

   ba->no_bits = no_bits;
   ba->no_words = GAN_NO_BITWORDS(no_bits);
   if ( data == NULL )
   {
      if ( ba->no_words == 0 )
      {
         ba->data = NULL;
         ba->data_alloc = GAN_FALSE;
      }
      else
      {
         ba->data = gan_malloc_array ( Gan_BitWord, ba->no_words );
         if ( ba->data == NULL )
         {
            gan_err_flush_trace();
            gan_err_register_with_number ( "gan_bit_array_form_data", GAN_ERROR_MALLOC_FAILED, "", ba->no_words*sizeof(Gan_BitWord) );
            return NULL;
         }

         ba->data_alloc = GAN_TRUE;
      }

      ba->words_alloc = ba->no_words;
   }
   else
   {
      if ( data_words < ba->no_words )
      {
         gan_err_flush_trace();
         gan_err_register ( "gan_bit_array_form_data", GAN_ERROR_INCOMPATIBLE,
                            "" );
         return NULL;
      }

      ba->data = data;
      ba->words_alloc = data_words;
      ba->data_alloc = GAN_FALSE;
   }

   /* set last word to zero to fill in unused bits */
   if ( ba->no_words > 0 ) ba->data[ba->no_words-1] = 0;

   /* no memory stack for this array */
   ba->memory_stack = NULL;

   return ba;
}

/**
 * \brief Form new bit array using stack memory style memory allocation
 * \param ms Pointer to memory stack structure
 * \param ba Existing bit array or \c NULL
 * \param no_bits Number of bits in bit array
 * \return Formed bit array, or \c NULL on error.
 *
 * Form new bit array of size \a no_bits using stack-style memory allocation.
 */
Gan_BitArray *
 gan_bit_array_ms_form ( Gan_MemoryStack *ms, Gan_BitArray *ba,
                         unsigned int no_bits )
{
   if ( ba == NULL )
   {
      ba = gan_ms_malloc_object ( ms, struct Gan_BitArray );
      if ( ba == NULL )
      {
         gan_err_register ( "gan_bit_array_tform", GAN_ERROR_FAILURE, "" );
         return NULL;
      }
      
      ba->alloc = GAN_TRUE;
   }
   else
      ba->alloc = GAN_FALSE;

   ba->no_bits = no_bits;
   ba->no_words = GAN_NO_BITWORDS(no_bits);
   ba->data = gan_ms_malloc_array ( ms, Gan_BitWord, ba->no_words );
   if ( ba->data == NULL )
   {
      gan_err_register ( "gan_bit_array_tform", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   ba->words_alloc = ba->no_words;
   ba->data_alloc = GAN_TRUE;

   /* set last word to zero to fill in unused bits */
   if ( ba->no_words > 0 ) ba->data[ba->no_words-1] = 0;

   /* set memory stack pointer to freeing this structure */
   ba->memory_stack = ms;

   return ba;
}

/**
 * \brief Reset size of existing bit array.
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Reset size of existing bit array \a ba to \a no_bits.
 */
Gan_Bool
 gan_bit_array_set_size ( Gan_BitArray *ba, unsigned int no_bits )
{
   ba->no_words = GAN_NO_BITWORDS(no_bits);
   if ( ba->no_words > ba->words_alloc )
   {
      if ( !ba->data_alloc || ba->memory_stack != NULL )
      {
         gan_err_flush_trace();
         gan_err_register ( "gan_bit_array_set_size", GAN_ERROR_CANT_REALLOC, "" );
         return GAN_FALSE;
      }
         
      ba->data = gan_realloc_array ( Gan_BitWord, ba->data, ba->no_words );
      if ( ba->data == NULL )
      {
         gan_err_flush_trace();
         gan_err_register_with_number ( "gan_bit_array_set_size", GAN_ERROR_MALLOC_FAILED, "", ba->no_words*sizeof(Gan_BitWord) );
         return GAN_FALSE;
      }

      ba->words_alloc = ba->no_words;
   }

   ba->no_bits = no_bits;

   /* set last word to zero to fill in unused bits */
   if ( ba->no_words > 0 ) ba->data[ba->no_words-1] = 0;

   /* success */
   return GAN_TRUE;
}

/**
 * \brief Free bit array.
 */
void
 gan_bit_array_free ( Gan_BitArray *ba )
{
   if ( ba->memory_stack == NULL )
   {
      /* use normal free function */
      if ( ba->data_alloc ) free ( ba->data );
      if ( ba->alloc      ) free ( ba );
   }
   else
   {
      /* use memory stack free function */
      if ( ba->data_alloc ) gan_ms_free ( ba->memory_stack, ba->data );
      if ( ba->alloc      ) gan_ms_free ( ba->memory_stack, ba );
   }
}

/**
 * \brief Free \c NULL terminated list of bit arrays.
 * \return No value.
 *
 * Free list of bit arrays starting with \a ba and terminated with \c NULL.
 */
void
 gan_bit_array_free_va ( Gan_BitArray *ba, ... )
{
   va_list ap;

   va_start ( ap, ba );
   while ( ba != NULL )
   {
      /* free next matrix */
      gan_bit_array_free ( ba );

      /* get next matrix in list */
      ba = va_arg ( ap, Gan_BitArray * );
   }

   va_end ( ap );
}

/**
 * \brief Set all bits in a bit array.
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Set all bits in bit array \a ba to the given boolean value \a val.
 */
Gan_Bool
 gan_bit_array_fill ( Gan_BitArray *ba , Gan_Bool val )
{
   unsigned int ctr = ba->no_words;
   Gan_BitWord valword;

   valword = val ? GAN_BITWORD_FULL : 0;

   while(ctr-- != 0)
      ba->data[ctr] = valword;

   return GAN_TRUE;
}

/**
 * \brief Invert all the bits in a bit array.
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Invert all the bits in a bit array \a ba.
 */
Gan_Bool
 gan_bit_array_invert_i ( Gan_BitArray *ba )
{
   unsigned int ctr = ba->no_words;

   while(ctr-- != 0)
      ba->data[ctr] = ba->data[ctr] ^ GAN_BITWORD_FULL;

   /* success */
   return GAN_TRUE;
}

/**
 * \brief Invert all the bits in a bit array.
 * \return A new inverted bit array.
 *
 * Invert all the bits in a bit array \a ba, returning the result as anew bit
 * array.
 */
Gan_BitArray *
 gan_bit_array_invert_s ( const Gan_BitArray *ba )
{
   unsigned int ctr = ba->no_words;
   Gan_BitArray *ba_dst;

   /* allocate result bit array */
   ba_dst = gan_bit_array_alloc ( ba->no_bits );
   if ( ba_dst == NULL )
   {
      gan_err_register ( "gan_bit_array_invert_s", GAN_ERROR_FAILURE, "" );
      return NULL;
   }
   
   while(ctr-- != 0)
      ba_dst->data[ctr] = ba->data[ctr] ^ GAN_BITWORD_FULL;

   /* success */
   return ba_dst;
}

/**
 * \brief Perform binary AND operation between two bit arrays.
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Perform binary AND operation between bit arrays \a ba and \a ba_dst,
 * writing the result into \a ba_dst.
 */
Gan_Bool
 gan_bit_array_and_i ( Gan_BitArray *ba_dst, const Gan_BitArray *ba )
{
   unsigned int ctr = ba->no_words;

   /* check that the bit arrays are the same size */
   if ( ba_dst->no_bits != ba->no_bits )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_bit_array_and_i", GAN_ERROR_INCOMPATIBLE, "" );
      return GAN_FALSE;
   }

   while(ctr-- != 0)
      ba_dst->data[ctr] = ba->data[ctr] & ba_dst->data[ctr];

   /* success */
   return GAN_TRUE;
}

/**
 * \brief Perform binary NAND (not and) operation between bit arrays.
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Perform binary NAND (not and) operation between bit arrays \a ba and
 * \a ba_dst, writing the result into \a ba_dst.
 */
Gan_Bool
 gan_bit_array_nand_i ( Gan_BitArray *ba_dst, const Gan_BitArray *ba )
{
   unsigned int ctr = ba->no_words;

   /* check that the bit arrays are the same size */
   if ( ba_dst->no_bits != ba->no_bits )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_bit_array_nand_i", GAN_ERROR_INCOMPATIBLE, "" );
      return GAN_FALSE;
   }

   while(ctr-- != 0)
      ba_dst->data[ctr] = (ba->data[ctr] & ba_dst->data[ctr])
                          ^ GAN_BITWORD_FULL;

   /* success */
   return GAN_TRUE;
}

/**
 * Perform binary OR operation between bit arrays.
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Perform binary OR operation between bit arrays \a ba and \a ba_dst,
 * writing the result into \a ba_dst.
 */
Gan_Bool
 gan_bit_array_or_i ( Gan_BitArray *ba_dst, const Gan_BitArray *ba )
{
   unsigned int ctr = ba->no_words;

   /* check that the bit arrays are the same size */
   if ( ba_dst->no_bits != ba->no_bits )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_bit_array_or_i", GAN_ERROR_INCOMPATIBLE, "" );
      return GAN_FALSE;
   }

   while(ctr-- != 0)
      ba_dst->data[ctr] = ba->data[ctr] | ba_dst->data[ctr];

   /* success */
   return GAN_TRUE;
}

/**
 * Perform binary EOR (exclusive or) operation between bit arrays.
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Perform binary EOR (exclusive or) operation between bit arrays \a ba and
 * \a ba_dst, writing the result into \a ba_dst.
 */
Gan_Bool
 gan_bit_array_eor_i ( Gan_BitArray *ba_dst, const Gan_BitArray *ba )
{
   unsigned int ctr = ba->no_words;

   /* check that the bit arrays are the same size */
   if ( ba_dst->no_bits != ba->no_bits )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_bit_array_eor_i", GAN_ERROR_INCOMPATIBLE, "" );
      return GAN_FALSE;
   }

   while(ctr-- != 0)
      ba_dst->data[ctr] = ba->data[ctr] ^ ba_dst->data[ctr];

   /* success */
   return GAN_TRUE;
}

/**
 * \brief Perform binary AND-NOT operation between bit arrays.
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Perform binary AND-NOT operation between bit arrays \a ba and
 * \a ba_dst, writing the result into \a ba_dst.
 */
Gan_Bool
 gan_bit_array_andnot_i ( Gan_BitArray *ba_dst, const Gan_BitArray *ba )
{
   unsigned int ctr = ba->no_words;

   /* check that the bit arrays are the same size */
   if ( ba_dst->no_bits != ba->no_bits )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_bit_array_andnot_i", GAN_ERROR_INCOMPATIBLE, "" );
      return GAN_FALSE;
   }

   while(ctr-- != 0)
      ba_dst->data[ctr] = ba_dst->data[ctr] & (ba->data[ctr] ^ GAN_BITWORD_FULL);

   /* success */
   return GAN_TRUE;
}

/**
 * Perform binary AND operation between bit arrays.
 * \return The result as a new bit array.
 *
 * Perform binary AND operation between bit arrays \a ba1 and \a ba2,
 * returning the result as a new bit array.
 */
Gan_BitArray *
 gan_bit_array_and_s  ( const Gan_BitArray *ba1, const Gan_BitArray *ba2 )
{
   Gan_BitArray *ba_dst;
   unsigned int ctr = ba1->no_words;

   /* check that the bit arrays are the same size */
   if ( ba1->no_bits != ba2->no_bits )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_bit_array_and_s", GAN_ERROR_INCOMPATIBLE, "" );
      return NULL;
   }

   /* allocate result bit array */
   ba_dst = gan_bit_array_alloc ( ba1->no_bits );
   if ( ba_dst == NULL )
   {
      gan_err_register ( "gan_bit_array_and_s", GAN_ERROR_FAILURE, "" );
      return NULL;
   }
   
   while(ctr-- != 0)
      ba_dst->data[ctr] = ba1->data[ctr] & ba2->data[ctr];

   /* success */
   return ba_dst;
}

/**
 * Perform binary NAND (not and) operation between bit arrays.
 * \return The result as a new bit array.
 *
 * Perform binary NAND (not and) operation between bit arrays \a ba1 and
 * \a ba2, returning the result as a new bit array.
 */
Gan_BitArray *
 gan_bit_array_nand_s ( const Gan_BitArray *ba1, const Gan_BitArray *ba2 )
{
   Gan_BitArray *ba_dst;
   unsigned int ctr = ba1->no_words;

   /* check that the bit arrays are the same size */
   if ( ba1->no_bits != ba2->no_bits )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_bit_array_nand_s", GAN_ERROR_INCOMPATIBLE, "" );
      return NULL;
   }

   /* allocate result bit array */
   ba_dst = gan_bit_array_alloc ( ba1->no_bits );
   if ( ba_dst == NULL )
   {
      gan_err_register ( "gan_bit_array_nand_s", GAN_ERROR_FAILURE, "" );
      return NULL;
   }
   
   while(ctr-- != 0)
      ba_dst->data[ctr] = (ba1->data[ctr] & ba2->data[ctr])
                          ^ GAN_BITWORD_FULL;

   /* success */
   return ba_dst;
}

/**
 * Perform binary OR operation between bit arrays.
 * \return The result as a new bit array.
 *
 * Perform binary OR operation between bit arrays \a ba1 and \a ba2,
 * returning the result as a new bit array.
 */
Gan_BitArray *
 gan_bit_array_or_s ( const Gan_BitArray *ba1, const Gan_BitArray *ba2 )
{
   Gan_BitArray *ba_dst;
   unsigned int ctr = ba1->no_words;

   /* check that the bit arrays are the same size */
   if ( ba1->no_bits != ba2->no_bits )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_bit_array_or_s", GAN_ERROR_INCOMPATIBLE, "" );
      return NULL;
   }

   /* allocate result bit array */
   ba_dst = gan_bit_array_alloc ( ba1->no_bits );
   if ( ba_dst == NULL )
   {
      gan_err_register ( "gan_bit_array_or_s", GAN_ERROR_FAILURE, "" );
      return NULL;
   }
   
   while(ctr-- != 0)
      ba_dst->data[ctr] = ba1->data[ctr] | ba2->data[ctr];

   /* success */
   return ba_dst;
}

/**
 * Perform binary EOR (exclusive or) operation between bit arrays.
 * \return The result as a new bit array.
 *
 * Perform binary EOR (exclusive or) operation between bit arrays \a ba1 and
 * \a ba2, returning the result as a new bit array.
 */
Gan_BitArray *
 gan_bit_array_eor_s ( const Gan_BitArray *ba1, const Gan_BitArray *ba2 )
{
   Gan_BitArray *ba_dst;
   unsigned int ctr = ba1->no_words;

   /* check that the bit arrays are the same size */
   if ( ba1->no_bits != ba2->no_bits )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_bit_array_eor_s", GAN_ERROR_INCOMPATIBLE, "" );
      return NULL;
   }

   /* allocate result bit array */
   ba_dst = gan_bit_array_alloc ( ba1->no_bits );
   if ( ba_dst == NULL )
   {
      gan_err_register ( "gan_bit_array_eor_s", GAN_ERROR_FAILURE, "" );
      return NULL;
   }
   
   while(ctr-- != 0)
      ba_dst->data[ctr] = ba1->data[ctr] ^ ba2->data[ctr];

   /* success */
   return ba_dst;
}

/**
 * Perform binary AND-NOT operation between bit arrays.
 * \return The result as a new bit array.
 *
 * Perform binary AND-NOT operation between bit arrays \a ba1 and
 * \a ba2, returning the result as a new bit array.
 */
Gan_BitArray *
 gan_bit_array_andnot_s ( const Gan_BitArray *ba1, const Gan_BitArray *ba2 )
{
   Gan_BitArray *ba_dst;
   unsigned int ctr = ba1->no_words;

   /* check that the bit arrays are the same size */
   if ( ba1->no_bits != ba2->no_bits )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_bit_array_andnot_s", GAN_ERROR_INCOMPATIBLE, "" );
      return NULL;
   }

   /* allocate result bit array */
   ba_dst = gan_bit_array_alloc ( ba1->no_bits );
   if ( ba_dst == NULL )
   {
      gan_err_register ( "gan_bit_array_andnot_s", GAN_ERROR_FAILURE, "" );
      return NULL;
   }
   
   while(ctr-- != 0)
      ba_dst->data[ctr] = ba1->data[ctr] & (ba2->data[ctr] ^ GAN_BITWORD_FULL);

   /* success */
   return ba_dst;
}

/**
 * \brief Insert part of one bit array in another.
 * \param src Source bit array
 * \param offset_src Offset of start of insertion region in \a src
 * \param dst Destination bit array
 * \param offset_dst Offset of start of insertion region in \a dst
 * \param no_bits Number of bits to insert
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Insert part of bit array \a src in bit array \a dst
 */
Gan_Bool
 gan_bit_array_insert ( const Gan_BitArray *src, unsigned int offset_src,
                              Gan_BitArray *dst, unsigned int offset_dst, 
                        unsigned int no_bits )
{
   unsigned int lbits, ubits, lmask, umask,
                no_word_boundaries_in_src, no_word_boundaries_in_dst, ctr = 2;
   Gan_BitWord *ptr_faw_src, *ptr_faw_dst;
   int skew, tmp;
   div_t res_src_div, res_dst_div;

   gan_err_test_bool ( offset_src+no_bits <= src->no_bits &&
                       offset_dst+no_bits <= dst->no_bits,
                       "gan_bit_array_insert", GAN_ERROR_ILLEGAL_ARGUMENT,
                       "" );

#if 0

   for ( tmp = 0; tmp < no_bits; tmp++ )
      if ( gan_bit_array_get_bit ( src, offset_src+tmp ) )
      {
         gan_bit_array_set_bit ( dst, offset_dst+tmp ) ;
      }
      else
      {
         gan_bit_array_clear_bit ( dst, offset_dst+tmp ) ;
      }

      return GAN_TRUE;

#endif


   /* Evaluate the quotient and remainder of offset divided by word length */
   res_src_div = div (offset_src, GAN_BITWORD_SIZE); 
   res_dst_div = div (offset_dst, GAN_BITWORD_SIZE); 

   /* Determine the difference in offset within the first word of src and dst.
      If skew < 0 a left shift is required to align, otherwise a right shift */
   skew = res_dst_div.rem - res_src_div.rem;

   /* Number of word boundaries that are crossed in the src and dst array */
   no_word_boundaries_in_src = (offset_src + no_bits - 1) / GAN_BITWORD_SIZE - res_src_div.quot;
   no_word_boundaries_in_dst = (offset_dst + no_bits - 1) / GAN_BITWORD_SIZE - res_dst_div.quot;

   ptr_faw_src = &(src->data[res_src_div.quot]);
   ptr_faw_dst = &(dst->data[res_dst_div.quot]);

   switch(no_word_boundaries_in_src)
   {
      case 0:        /* Bits are contained within a single word in the src array */
                        
        switch(no_word_boundaries_in_dst)
        {
           case 0:        /* Bits are contained within a single word in the dst array */

#ifdef WORDS_BIGENDIAN
             /* Shift bits so they can be copied into dst words */
             if(skew < 0)
                lbits = (*ptr_faw_src) << abs(skew);        
             else
                lbits = (*ptr_faw_src) >> abs(skew);        

             /* Make mask */
             tmp = GAN_BITWORD_SIZE - no_bits;

             lmask = GAN_BITWORD_FULL << res_dst_div.rem;
             lmask = lmask >> tmp;
             lmask = lmask << (tmp - res_dst_div.rem);
#else
             /* Shift bits so they can be copied into dst words */
             if(skew < 0)
                lbits = (*ptr_faw_src) >> abs(skew);        
             else
                lbits = (*ptr_faw_src) << abs(skew);        

             /* Make mask */
             tmp = GAN_BITWORD_SIZE - no_bits;

             lmask = GAN_BITWORD_FULL >> res_dst_div.rem;
             lmask = lmask << tmp;
             lmask = lmask >> (tmp - res_dst_div.rem);
#endif /* #ifdef WORDS_BIGENDIAN */

             /* Apply mask and insert */
             *ptr_faw_dst = (*ptr_faw_dst & ~lmask) | (lbits & lmask);

             return GAN_TRUE;

           case 1: /* Bits are spread over two words in the dst array - skew must be > 0 */
                                        
#ifdef WORDS_BIGENDIAN
             /* Shift bits so they can be copied into dst words */
             ubits = (*ptr_faw_src) >> skew;
             lbits = (*ptr_faw_src) << (GAN_BITWORD_SIZE - skew);

             /* Make mask */
             umask = GAN_BITWORD_FULL >> res_dst_div.rem;
             lmask = GAN_BITWORD_FULL << ((GAN_BITWORD_SIZE << 1) - res_dst_div.rem - no_bits);
#else
             /* Shift bits so they can be copied into dst words */
             ubits = (*ptr_faw_src) << skew;
             lbits = (*ptr_faw_src) >> (GAN_BITWORD_SIZE - skew);

             /* Make mask */
             umask = GAN_BITWORD_FULL << res_dst_div.rem;
             lmask = GAN_BITWORD_FULL >> ((GAN_BITWORD_SIZE << 1) - res_dst_div.rem - no_bits);
#endif /* #ifdef WORDS_BIGENDIAN */

             /* Apply mask and insert */
             *ptr_faw_dst = (*ptr_faw_dst & ~umask) |  (ubits & umask);
             *(ptr_faw_dst + 1) = (*(ptr_faw_dst + 1) & ~lmask) |  (lbits & lmask);                                        

             return GAN_TRUE;
        }
        break;

      case 1:

        switch(no_word_boundaries_in_dst)
        {
           case 0:

             /* Shift bits so they can be copied into dst words (skew can only be negative) */
#ifdef WORDS_BIGENDIAN
             ubits = (*ptr_faw_src) << abs(skew);
             lbits = (*(ptr_faw_src+1)) >> (GAN_BITWORD_SIZE + skew);

             /* Make mask */
             umask = GAN_BITWORD_FULL >> res_src_div.rem;
             umask = umask << abs(skew);

             tmp = res_src_div.rem + no_bits;
             lmask = GAN_BITWORD_FULL >> ((GAN_BITWORD_SIZE << 1) - tmp );
             lmask = lmask << (abs(skew) + GAN_BITWORD_SIZE - tmp);
#else
             ubits = (*ptr_faw_src) >> abs(skew);
             lbits = (*(ptr_faw_src+1)) << (GAN_BITWORD_SIZE + skew);

             /* Make mask */
             umask = GAN_BITWORD_FULL << res_src_div.rem;
             umask = umask >> abs(skew);

             tmp = res_src_div.rem + no_bits;
             lmask = GAN_BITWORD_FULL << ((GAN_BITWORD_SIZE << 1) - tmp );
             lmask = lmask >> (abs(skew) + GAN_BITWORD_SIZE - tmp);
#endif /* #ifdef WORDS_BIGENDIAN */

             /* Apply mask and insert */
             *ptr_faw_dst = (*ptr_faw_dst & ~umask ) | (ubits & umask);
             *ptr_faw_dst = (*ptr_faw_dst & ~lmask ) | (lbits & lmask);

             return GAN_TRUE;

           case 1:


             if(skew < 0)
             {
                /* first word */

#ifdef WORDS_BIGENDIAN
                /* Shift bits so they can be copied into dst words */
                ubits = (*ptr_faw_src) << abs(skew);
                lbits = (*(ptr_faw_src+1)) >> (GAN_BITWORD_SIZE + skew);
                                                
                /* Make mask */
                umask = GAN_BITWORD_FULL >> res_dst_div.rem;
                lmask = ~(GAN_BITWORD_FULL << abs(skew));

                /* Apply mask and insert */
                *ptr_faw_dst = (*ptr_faw_dst & ~umask) | (ubits & (umask & ~lmask)) | (lbits & lmask);
                                                

                /* second word */
                ubits = (*(ptr_faw_src+1)) << abs(skew);

                /* Make mask */
                umask = GAN_BITWORD_FULL << ( (GAN_BITWORD_SIZE << 1) - no_bits - res_dst_div.rem);
#else
                /* Shift bits so they can be copied into dst words */
                ubits = (*ptr_faw_src) >> abs(skew);
                lbits = (*(ptr_faw_src+1)) << (GAN_BITWORD_SIZE + skew);
                                                
                /* Make mask */
                umask = GAN_BITWORD_FULL << res_dst_div.rem;
                lmask = ~(GAN_BITWORD_FULL >> abs(skew));

                /* Apply mask and insert */
                *ptr_faw_dst = (*ptr_faw_dst & ~umask) | (ubits & (umask & ~lmask)) | (lbits & lmask);
                                                

                /* second word */
                ubits = (*(ptr_faw_src+1)) >> abs(skew);

                /* Make mask */
                umask = GAN_BITWORD_FULL >> ( (GAN_BITWORD_SIZE << 1) - no_bits - res_dst_div.rem);
#endif /* #ifdef WORDS_BIGENDIAN */
                                                
                /* Apply mask and insert */
                *(ptr_faw_dst+1) = (*(ptr_faw_dst+1) & ~umask) | (ubits & umask);
             }
             else
             {
                /* first word */
                tmp = GAN_BITWORD_SIZE - skew;

#ifdef WORDS_BIGENDIAN
                /* Make Mask */
                umask = GAN_BITWORD_FULL >> res_dst_div.rem;

                /* Shift bits so they can be copied into dst words */
                ubits = (*ptr_faw_src) >> skew;
                                                
                /* Apply mask and insert */
                *ptr_faw_dst = (*ptr_faw_dst & ~umask) | (ubits & umask);
                                                
                /* second word */
                ubits = (*ptr_faw_src) << tmp;
                lbits = (*(ptr_faw_src+1)) >> skew;

                /* Make mask */
                umask = GAN_BITWORD_FULL << tmp;
                lmask = GAN_BITWORD_FULL << ((GAN_BITWORD_SIZE << 1) - no_bits - res_dst_div.rem);
#else
                /* Make Mask */
                umask = GAN_BITWORD_FULL << res_dst_div.rem;

                /* Shift bits so they can be copied into dst words */
                ubits = (*ptr_faw_src) << skew;
                                                
                /* Apply mask and insert */
                *ptr_faw_dst = (*ptr_faw_dst & ~umask) | (ubits & umask);
                                                
                /* second word */
                ubits = (*ptr_faw_src) >> tmp;
                lbits = (*(ptr_faw_src+1)) << skew;

                /* Make mask */
                umask = GAN_BITWORD_FULL >> tmp;
                lmask = GAN_BITWORD_FULL >> ((GAN_BITWORD_SIZE << 1) - no_bits - res_dst_div.rem);
#endif /* #ifdef WORDS_BIGENDIAN */

                /* Apply mask and insert */
                *(ptr_faw_dst+1) = ((*(ptr_faw_dst+1) & ~(umask | lmask)) ) | (ubits & umask) | (lbits & lmask);
             }
                                        
             return GAN_TRUE;
                                        
           case 2: /* skew can only be positive */

             /* first word */
             tmp = GAN_BITWORD_SIZE - skew;

#ifdef WORDS_BIGENDIAN
             /* Make Mask */        
             umask = GAN_BITWORD_FULL >> res_dst_div.rem;

             /* Shift bits so they can be copied into dst words */
             ubits = (*ptr_faw_src) >> skew;
                                        
             /* Apply mask and insert */
             *ptr_faw_dst = (*ptr_faw_dst & ~umask) | (ubits & umask);

             /* middle word */

             /* Shift bits so they can be copied into dst words */
             ubits = (*ptr_faw_src) << tmp;
             lbits = (*(ptr_faw_src+1)) >> skew;
                                        
             /* Make mask */
             umask = GAN_BITWORD_FULL << tmp;

             /* Apply mask and insert */
             *(ptr_faw_dst+1) = (ubits & umask) | (lbits & ~umask);

             /* last word */

             /* Shift bits so they can be copied into dst words */
             lbits = *(ptr_faw_src+1) << tmp;

             /* Make mask */
             lmask = GAN_BITWORD_FULL << ((GAN_BITWORD_SIZE + (GAN_BITWORD_SIZE<<1)) - no_bits - res_dst_div.rem);
#else
             /* Make Mask */        
             umask = GAN_BITWORD_FULL << res_dst_div.rem;

             /* Shift bits so they can be copied into dst words */
             ubits = (*ptr_faw_src) << skew;
                                        
             /* Apply mask and insert */
             *ptr_faw_dst = (*ptr_faw_dst & ~umask) | (ubits & umask);

             /* middle word */

             /* Shift bits so they can be copied into dst words */
             ubits = (*ptr_faw_src) >> tmp;
             lbits = (*(ptr_faw_src+1)) << skew;
                                        
             /* Make mask */
             umask = GAN_BITWORD_FULL >> tmp;

             /* Apply mask and insert */
             *(ptr_faw_dst+1) = (ubits & umask) | (lbits & ~umask);

             /* last word */

             /* Shift bits so they can be copied into dst words */
             lbits = *(ptr_faw_src+1) >> tmp;

             /* Make mask */
             lmask = GAN_BITWORD_FULL >> ((GAN_BITWORD_SIZE + (GAN_BITWORD_SIZE<<1)) - no_bits - res_dst_div.rem);
#endif /* #ifdef WORDS_BIGENDIAN */

             /* Apply mask and insert */
             *(ptr_faw_dst + 2) = (*(ptr_faw_dst + 2) & ~lmask) |  (lbits & lmask);                                        

             return GAN_TRUE;
        }

      default:

        switch(no_word_boundaries_in_dst - no_word_boundaries_in_src)
        {
           case -1: /* skew can only be negative */

             /* first word */
             tmp = GAN_BITWORD_SIZE + skew;

#ifdef WORDS_BIGENDIAN
             /* Shift bits so they can be copied into dst words */
             ubits = (*ptr_faw_src) << abs(skew);
             lbits = (*(ptr_faw_src+1)) >> tmp;
                                                
             /* Make mask */
             lmask = ~(GAN_BITWORD_FULL << abs(skew));
             umask = GAN_BITWORD_FULL >> res_dst_div.rem;
#else
             /* Shift bits so they can be copied into dst words */
             ubits = (*ptr_faw_src) >> abs(skew);
             lbits = (*(ptr_faw_src+1)) << tmp;
                                                
             /* Make mask */
             lmask = ~(GAN_BITWORD_FULL >> abs(skew));
             umask = GAN_BITWORD_FULL << res_dst_div.rem;
#endif /* #ifdef WORDS_BIGENDIAN */

             /* Apply mask and insert */
             *ptr_faw_dst = (*ptr_faw_dst & ~umask) | (ubits & (umask & ~lmask)) | (lbits & lmask);
                                                

             /* middle words */

#ifdef WORDS_BIGENDIAN
             /* Make mask */
             umask = GAN_BITWORD_FULL << abs(skew);

             for(;ctr < no_word_boundaries_in_src; ctr++)        
             {
                /* Shift bits so they can be copied into dst words */
                ubits = *(ptr_faw_src + ctr - 1) << abs(skew);
                lbits = (*(ptr_faw_src + ctr)) >> tmp;
                                        
                /* Apply mask and insert */
                *(ptr_faw_dst + ctr - 1) = (ubits & umask) | (lbits & ~umask);
             }

             /* last word */

             /* Shift bits so they can be copied into dst words */
             ubits = (*(ptr_faw_src + ctr - 1)) << abs(skew);
             lbits = (*(ptr_faw_src + ctr)) >> tmp;

             /* Make mask */
             umask = GAN_BITWORD_FULL << abs(skew);
             lmask = GAN_BITWORD_FULL << (-skew - (no_bits + offset_src) % GAN_BITWORD_SIZE );
#else
             /* Make mask */
             umask = GAN_BITWORD_FULL >> abs(skew);

             for(;ctr < no_word_boundaries_in_src; ctr++)        
             {
                /* Shift bits so they can be copied into dst words */
                ubits = *(ptr_faw_src + ctr - 1) >> abs(skew);
                lbits = (*(ptr_faw_src + ctr)) << tmp;
                                        
                /* Apply mask and insert */
                *(ptr_faw_dst + ctr - 1) = (ubits & umask) | (lbits & ~umask);
             }

             /* last word */

             /* Shift bits so they can be copied into dst words */
             ubits = (*(ptr_faw_src + ctr - 1)) >> abs(skew);
             lbits = (*(ptr_faw_src + ctr)) << tmp;

             /* Make mask */
             umask = GAN_BITWORD_FULL >> abs(skew);
             lmask = GAN_BITWORD_FULL >> (-skew - (no_bits + offset_src) % GAN_BITWORD_SIZE );
#endif /* #ifdef WORDS_BIGENDIAN */
                                                
             /* Apply mask and insert */
             *(ptr_faw_dst + ctr - 1) = (*(ptr_faw_dst + ctr - 1) & ~lmask) | (ubits & umask) | (lbits & (umask ^ lmask));

             return GAN_TRUE;

           case 0: 
             if(skew < 0)
             {
                /* first word */
                tmp = GAN_BITWORD_SIZE + skew;

#ifdef WORDS_BIGENDIAN
                /* Shift bits so they can be copied into dst words */
                ubits = (*ptr_faw_src) << abs(skew);
                lbits = (*(ptr_faw_src+1)) >> tmp;
                                                
                /* Make mask */
                lmask = ~(GAN_BITWORD_FULL << abs(skew));
                umask = GAN_BITWORD_FULL >> res_dst_div.rem;
#else
                /* Shift bits so they can be copied into dst words */
                ubits = (*ptr_faw_src) >> abs(skew);
                lbits = (*(ptr_faw_src+1)) << tmp;
                                                
                /* Make mask */
                lmask = ~(GAN_BITWORD_FULL >> abs(skew));
                umask = GAN_BITWORD_FULL << res_dst_div.rem;
#endif /* #ifdef WORDS_BIGENDIAN */

                /* Apply mask and insert */
                *ptr_faw_dst = (*ptr_faw_dst & ~umask) | (ubits & (umask & ~lmask)) | (lbits & lmask);
                                                
                /* middle words */

#ifdef WORDS_BIGENDIAN
                /* Make mask */
                umask = GAN_BITWORD_FULL << abs(skew);

                for(ctr = 1;ctr < no_word_boundaries_in_src; ctr++)        
                {
                   /* Shift bits so they can be copied into dst words */
                   ubits = *(ptr_faw_src + ctr) << abs(skew);
                   lbits = *(ptr_faw_src + ctr + 1) >> tmp;

                   /* Apply mask and insert */
                   *(ptr_faw_dst + ctr) = (ubits & umask) | (lbits & ~umask);
                }

                /* last word */

                /* Shift bits so they can be copied into dst words */
                ubits = *(ptr_faw_src + ctr) << abs(skew);

                /* Make mask */
                umask = ~(GAN_BITWORD_FULL >> ((res_dst_div.rem + no_bits) % GAN_BITWORD_SIZE)); 
#else
                /* Make mask */
                umask = GAN_BITWORD_FULL >> abs(skew);

                for(ctr = 1;ctr < no_word_boundaries_in_src; ctr++)        
                {
                   /* Shift bits so they can be copied into dst words */
                   ubits = *(ptr_faw_src + ctr) >> abs(skew);
                   lbits = *(ptr_faw_src + ctr + 1) << tmp;

                   /* Apply mask and insert */
                   *(ptr_faw_dst + ctr) = (ubits & umask) | (lbits & ~umask);
                }

                /* last word */

                /* Shift bits so they can be copied into dst words */
                ubits = *(ptr_faw_src + ctr) >> abs(skew);

                /* Make mask */
                umask = ~(GAN_BITWORD_FULL << ((res_dst_div.rem + no_bits) % GAN_BITWORD_SIZE)); 
#endif /* #ifdef WORDS_BIGENDIAN */

                /* Apply mask and insert */
                *(ptr_faw_dst + ctr) = (*(ptr_faw_dst + ctr) & ~umask) | (ubits & umask);

                return GAN_TRUE;
             }
             else /* skew > 0 */
             {
                /* first word */
                tmp = GAN_BITWORD_SIZE - skew;

#ifdef WORDS_BIGENDIAN
                /* Shift bits so they can be copied into dst words */
                ubits = *ptr_faw_src >> skew;
                                                
                /* Make mask */
                umask = GAN_BITWORD_FULL >> res_dst_div.rem;
#else
                /* Shift bits so they can be copied into dst words */
                ubits = *ptr_faw_src << skew;
                                                
                /* Make mask */
                umask = GAN_BITWORD_FULL << res_dst_div.rem;
#endif /* #ifdef WORDS_BIGENDIAN */

                /* Apply mask and insert */
                *ptr_faw_dst = (*ptr_faw_dst & ~umask) | (ubits & umask);
                                                
                /* middle words */

#ifdef WORDS_BIGENDIAN
                /* Make mask */
                umask = ~(GAN_BITWORD_FULL >> skew);

                for(ctr = 1;ctr < no_word_boundaries_in_src; ctr++)        
                {
                   /* Shift bits so they can be copied into dst words */
                   ubits = *(ptr_faw_src + ctr - 1) << (tmp);
                   lbits = *(ptr_faw_src + ctr) >> skew;
                                        
                   /* Apply mask and insert */
                   *(ptr_faw_dst + ctr) = (ubits & umask) | (lbits & ~umask);
                }

                /* last word */

                /* Shift bits so they can be copied into dst words */
                ubits = *(ptr_faw_src + ctr - 1) << tmp;
                lbits = *(ptr_faw_src + ctr) >> skew;

                /* Make mask */
                lmask = ~(GAN_BITWORD_FULL >> ((res_dst_div.rem + no_bits) % GAN_BITWORD_SIZE)); 
#else
                /* Make mask */
                umask = ~(GAN_BITWORD_FULL << skew);

                for(ctr = 1;ctr < no_word_boundaries_in_src; ctr++)        
                {
                   /* Shift bits so they can be copied into dst words */
                   ubits = *(ptr_faw_src + ctr - 1) >> (tmp);
                   lbits = *(ptr_faw_src + ctr) << skew;
                                        
                   /* Apply mask and insert */
                   *(ptr_faw_dst + ctr) = (ubits & umask) | (lbits & ~umask);
                }

                /* last word */

                /* Shift bits so they can be copied into dst words */
                ubits = *(ptr_faw_src + ctr - 1) >> tmp;
                lbits = *(ptr_faw_src + ctr) << skew;

                /* Make mask */
                lmask = ~(GAN_BITWORD_FULL << ((res_dst_div.rem + no_bits) % GAN_BITWORD_SIZE)); 
#endif /* #ifdef WORDS_BIGENDIAN */

                /* Apply mask and insert */
                *(ptr_faw_dst + ctr) = (*(ptr_faw_dst + ctr) & ~lmask) | (ubits & umask) | (lbits & (lmask ^umask));
                                                        
                return GAN_TRUE;
             }
                                
           case 1: /* skew can only be positive */

             /* first word */
             tmp = GAN_BITWORD_SIZE - skew;        

#ifdef WORDS_BIGENDIAN
             /* Shift bits so they can be copied into dst words */
             ubits = *ptr_faw_src >> skew;
                                                
             /* Make mask */
             umask = GAN_BITWORD_FULL >> res_dst_div.rem;
#else
             /* Shift bits so they can be copied into dst words */
             ubits = *ptr_faw_src << skew;
                                                
             /* Make mask */
             umask = GAN_BITWORD_FULL << res_dst_div.rem;
#endif /* #ifdef WORDS_BIGENDIAN */

             /* Apply mask and insert */
             *ptr_faw_dst = (*ptr_faw_dst & ~umask) | (ubits & umask);
                                                
             /* middle words */

#ifdef WORDS_BIGENDIAN
             /* Make mask */
             umask = ~(GAN_BITWORD_FULL >> skew);

             for(ctr = 1;ctr <= no_word_boundaries_in_src; ctr++)        
             {
                /* Shift bits so they can be copied into dst words */
                ubits = *(ptr_faw_src + ctr - 1) << tmp;
                lbits = *(ptr_faw_src + ctr) >> skew;
                                        
                /* Apply mask and insert */
                *(ptr_faw_dst + ctr) = (ubits & umask) | (lbits & ~umask);
             }

             /* last word */

             /* Shift bits so they can be copied into dst words */
             ubits = *(ptr_faw_src + ctr - 1) << tmp;
                                                
             /* Make mask */
             umask = GAN_BITWORD_FULL << ( GAN_BITWORD_SIZE - ( (no_bits + offset_src) % GAN_BITWORD_SIZE + skew) );
#else
             /* Make mask */
             umask = ~(GAN_BITWORD_FULL << skew);

             for(ctr = 1;ctr <= no_word_boundaries_in_src; ctr++)        
             {
                /* Shift bits so they can be copied into dst words */
                ubits = *(ptr_faw_src + ctr - 1) >> tmp;
                lbits = *(ptr_faw_src + ctr) << skew;
                                        
                /* Apply mask and insert */
                *(ptr_faw_dst + ctr) = (ubits & umask) | (lbits & ~umask);
             }

             /* last word */

             /* Shift bits so they can be copied into dst words */
             ubits = *(ptr_faw_src + ctr - 1) >> tmp;
                                                
             /* Make mask */
             umask = GAN_BITWORD_FULL >> ( GAN_BITWORD_SIZE - ( (no_bits + offset_src) % GAN_BITWORD_SIZE + skew) );
#endif /* #ifdef WORDS_BIGENDIAN */

             /* Apply mask and insert */
             *(ptr_faw_dst + ctr) = (*(ptr_faw_dst + ctr) & ~umask) | (ubits & umask);

             return GAN_TRUE;
        }
   }

   /* success */
   return GAN_TRUE;
}

/**
 * \brief Copy one bit array to another.
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Copy bit array \a ba_source to \a ba_dest.
 */
Gan_Bool
 gan_bit_array_copy_q ( const Gan_BitArray *ba_source, Gan_BitArray *ba_dest )
{
   /* make sure destination bit array can hold source bit array */
   if ( ba_dest->words_alloc < ba_source->no_words &&
        !gan_bit_array_set_size ( ba_dest, ba_source->no_bits ) )
   {
      gan_err_register ( "gan_bit_array_copy_q", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   /* copy bit array data */
   memcpy(ba_dest->data, ba_source->data, ba_source->no_words * sizeof(Gan_BitWord));

   /* success */
   return GAN_TRUE;
}

/**
 * \brief Return a copy of a bit array.
 * \return A new copy of the bit array \a ba_source.
 *
 * Return a copy of bit array \a ba_source.
 */
Gan_BitArray *
 gan_bit_array_copy_s ( const Gan_BitArray *ba_source )
{
   Gan_BitArray *ba_dest = gan_bit_array_alloc(ba_source->no_bits);

   if ( ba_dest == NULL ||
        !gan_bit_array_copy_q ( ba_source, ba_dest ) )
   {
      gan_err_register ( "gan_bit_array_copy_s", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* success */
   return ba_dest;
}

/**
 * \brief Expand bit array by padding it with zeros in a reference bit array.
 * \param ba Input bit array
 * \param ref_ba Reference bit array specifying padded bits
 * \param exp_ba Expanded bit array or \c NULL.
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Expand bit array \a ba by padding it with zeros in a reference bit array
 * \a ref_ba.
 */
Gan_BitArray *
 gan_bit_array_expand_q ( const Gan_BitArray *ba, const Gan_BitArray *ref_ba,
                          Gan_BitArray *exp_ba )
{
   unsigned int i, j;

   if ( exp_ba == NULL )
   {
      /* allocate new bit array */
      exp_ba = gan_bit_array_alloc ( ref_ba->no_bits );
      if ( exp_ba == NULL )
      {
         gan_err_register ( "gan_bit_array_expand_q", GAN_ERROR_FAILURE, "" );
         return NULL;
      }
   }
   else
      /* resize bit array if necessary */
      if ( exp_ba->no_bits != ref_ba->no_bits &&
           !gan_bit_array_set_size ( exp_ba, ref_ba->no_bits ) )
      {
         gan_err_register ( "gan_bit_array_expand_q", GAN_ERROR_FAILURE, "" );
         return NULL;
      }
   
   gan_bit_array_fill ( exp_ba, GAN_FALSE );
   for ( i = 0, j = 0; i < ref_ba->no_bits; i++ )
      if ( gan_bit_array_get_bit ( ref_ba, i ) )
      {
         if ( gan_bit_array_get_bit ( ba, j ) )
            gan_bit_array_set_bit ( exp_ba, i );

         j++;
      }

   /* make sure we've reached the end of the bit array */
   if ( j != ba->no_bits )
   {
      gan_err_flush_trace();
      gan_err_register ( "gan_bit_array_expand_q", GAN_ERROR_INCOMPATIBLE,
                         "" );
      return NULL;
   }

   /* success */
   return exp_ba;
}

/**
 * \brief Expand bit array by padding it with zeros in a reference bit array.
 * \param ba Input bit array
 * \param ref_ba Reference bit array specifying padded bits
 * \return Expanded bit array as a new bit array, or \c NULL on failure.
 *
 * Expand bit array \a ba by padding it with zeros in a reference bit array
 * \a ref_ba.
 */
Gan_BitArray *
 gan_bit_array_expand_s ( const Gan_BitArray *ba, const Gan_BitArray *ref_ba )
{
   Gan_BitArray *exp_ba = gan_bit_array_alloc(ref_ba->no_bits);

   if ( ref_ba == NULL || gan_bit_array_expand_q ( ba, ref_ba, exp_ba ) == NULL )
   {
      gan_err_register ( "gan_bit_array_expand_s", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* success */
   return exp_ba;
}

static Gan_Bool
 dilate_1bit ( Gan_BitArray *ba )
{
   unsigned bit;
   int ctr = (int)ba->no_words-1;
   Gan_BitWord word, end_bit=0, next_end_bit;

   if ( ctr < 0 ) return GAN_TRUE;
   
   bit = ba->no_bits % GAN_BITWORD_SIZE;
   if ( bit > 0 )
   {
      word = ba->data[ctr];

      /* clear last bit before combining */
#ifdef WORDS_BIGENDIAN
      next_end_bit = word & GAN_MSB_SET;
      end_bit = GAN_MSB_SET >> bit;
#else /* #ifndef WORDS_BIGENDIAN */
      next_end_bit = word & GAN_LSB_SET;
      end_bit = GAN_LSB_SET << bit;
#endif
      if ( word & end_bit ) word ^= end_bit;
      ba->data[ctr] |= (word << 1) | (word >> 1);
#ifdef WORDS_BIGENDIAN
      end_bit = ba->data[ctr] & GAN_MSB_SET;
      if ( ctr > 0 && (ba->data[ctr-1] & GAN_LSB_SET) )
         ba->data[ctr] |= GAN_MSB_SET;
#else /* #ifndef WORDS_BIGENDIAN */
      end_bit = ba->data[ctr] & GAN_LSB_SET;
      if ( ctr > 0 && (ba->data[ctr-1] & GAN_MSB_SET) )
         ba->data[ctr] |= GAN_LSB_SET;
#endif
      end_bit = next_end_bit;
      ctr--;
   }

   for ( ; ctr >= 0; ctr-- )
   {
      word = ba->data[ctr];
#ifdef WORDS_BIGENDIAN
      next_end_bit = word & GAN_MSB_SET;
      ba->data[ctr] = (word << 1) | word | (word >> 1);
      if ( end_bit ) ba->data[ctr] |= GAN_LSB_SET;
      if ( ctr > 0 && (ba->data[ctr-1] & GAN_LSB_SET) )
         ba->data[ctr] |= GAN_MSB_SET;
#else /* #ifndef WORDS_BIGENDIAN */
      next_end_bit = word & GAN_LSB_SET;
      ba->data[ctr] = (word << 1) | word | (word >> 1);
      if ( end_bit ) ba->data[ctr] |= GAN_MSB_SET;
      if ( ctr > 0 && (ba->data[ctr-1] & GAN_MSB_SET) )
         ba->data[ctr] |= GAN_LSB_SET;
#endif
      end_bit = next_end_bit;
   }

   /* success */
   return GAN_TRUE;
   
}

/**
 * \brief Dilate bit array and write result into another
 * \param ba_source Input bit array
 * \param no_bits Number of bits to dilate bit array
 * \param ba_dest Result bit array
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Dilate input bit array \a ba_source by \a no_bits, and write result into
 * another bit array \a ba_dest.
 */
Gan_Bool
 gan_bit_array_dilate_q ( Gan_BitArray *ba_source, unsigned int no_bits,
                          Gan_BitArray *ba_dest )
{
   int bit;

   if ( ba_source != ba_dest &&
        !gan_bit_array_copy_q ( ba_source, ba_dest ) )
   {
      gan_err_register ( "gan_bit_array_dilate_q", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   for ( bit = (int)no_bits-1; bit >= 0; bit-- )
      if ( !dilate_1bit ( ba_dest ) )
      {
         gan_err_register ( "gan_bit_array_dilate_q", GAN_ERROR_FAILURE, "" );
         return GAN_FALSE;
      }

   return GAN_TRUE;
}

/**
 * \brief Dilate bit array in-place
 * \param ba Input bit array
 * \param no_bits Number of bits to dilate bit array
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Dilate input bit array \a ba by \a no_bits in-place.
 */
Gan_BitArray *
 gan_bit_array_dilate_s ( Gan_BitArray *ba, unsigned int no_bits )
{
   Gan_BitArray *ba_dest = gan_bit_array_alloc(ba->no_bits);

   if ( ba_dest == NULL || !gan_bit_array_dilate_q ( ba, no_bits, ba_dest ) )
   {
      gan_err_register ( "gan_bit_array_dilate_s", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* success */
   return ba_dest;
}

static Gan_Bool
 shift_right ( Gan_BitArray *ba, unsigned int no_bits )
{
   unsigned int no_words, word;

   // shift whole words first
   no_words = no_bits / GAN_BITWORD_SIZE;
   if(no_words > 0)
   {
      for(word=ba->no_words-1; word>=no_words; word--)
         ba->data[word] = ba->data[word-no_words];

      // pad first words with zero
      for(word=0; word<no_words; word++)
         ba->data[word] = 0;
   }

   // now deal with remaining bits
   no_bits = no_bits % GAN_BITWORD_SIZE;
   if(no_bits != 0)
   {
      Gan_BitWord rollover=0;

      for(word=0; word<ba->no_words; word++)
      {
#ifdef WORDS_BIGENDIAN
         // remember the bits being shifted out
         Gan_BitWord new_rollover = ba->data[word] << (GAN_BITWORD_SIZE - no_bits);

         // apply shift and insert any rolled over bits
         ba->data[word] = (ba->data[word] >> no_bits) | rollover;
#else
         // remember the bits being shifted out
         Gan_BitWord new_rollover = ba->data[word] >> (GAN_BITWORD_SIZE - no_bits);

         // apply shift and insert any rolled over bits
         ba->data[word] = (ba->data[word] << no_bits) | rollover;
#endif         
         // remember discarded bits for next word
         rollover = new_rollover;
      }
   }
   
   return GAN_TRUE;
}

static Gan_Bool
 shift_left ( Gan_BitArray *ba, unsigned int no_bits )
{
   unsigned int no_words, word;

   // shift whole words first
   no_words = no_bits / GAN_BITWORD_SIZE;
   if(no_words > 0)
   {
      for(word=no_words; word<ba->no_words; word++)
         ba->data[word-no_words] = ba->data[word];

      // pad last words with zero
      for(word=ba->no_words-no_words; word<ba->no_words; word++)
         ba->data[word] = 0;
   }

   // now deal with remaining bits
   no_bits = no_bits % GAN_BITWORD_SIZE;
   if(no_bits != 0)
   {
      Gan_BitWord rollover=0;
      int iword;

      for(iword=(int)ba->no_words-1; iword >= 0; iword--)
      {
#ifdef WORDS_BIGENDIAN
         // remember the bits being shifted out
         Gan_BitWord new_rollover = ba->data[iword] >> (GAN_BITWORD_SIZE - no_bits);

         // apply shift and insert any rolled over bits
         ba->data[iword] = (ba->data[iword] << no_bits) | rollover;
#else
         // remember the bits being shifted out
         Gan_BitWord new_rollover = ba->data[iword] << (GAN_BITWORD_SIZE - no_bits);

         // apply shift and insert any rolled over bits
         ba->data[iword] = (ba->data[iword] >> no_bits) | rollover;
#endif         
         // remember discarded bits for next word
         rollover = new_rollover;
      }
   }
   
   return GAN_TRUE;
}

/**
 * \brief Shift bit array and write result into another
 * \param ba_source Input bit array
 * \param no_bits Number of bits to shift bit array
 * \param ba_dest Result bit array
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Shift input bit array \a ba_source by \a no_bits, and write result into
 * another bit array \a ba_dest. If \a no_bits is >= 0,
 * the shift is towards the end of the bit array, and vice veraa.
 */
Gan_Bool
 gan_bit_array_shift_q ( Gan_BitArray *ba_source, int no_bits,
                         Gan_BitArray *ba_dest )
{
   if ( ba_source != ba_dest &&
        !gan_bit_array_copy_q ( ba_source, ba_dest ) )
   {
      gan_err_register ( "gan_bit_array_shift_q", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   /* if we're shifting the whole bit array out of itself, fill with zeros */
   if(abs(no_bits) >= (int)ba_dest->no_bits)
   {
      if(!gan_bit_array_fill ( ba_dest, GAN_FALSE ))
      {
         gan_err_register ( "gan_bit_array_shift_q", GAN_ERROR_FAILURE, "" );
         return GAN_FALSE;
      }

      return GAN_TRUE;
   }

   if(no_bits >= 0)
   {
      if ( !shift_right ( ba_dest, (unsigned int)no_bits ) )
      {
         gan_err_register ( "gan_bit_array_shift_q", GAN_ERROR_FAILURE, "" );
         return GAN_FALSE;
      }
   }
   else
   {
      if ( !shift_left ( ba_dest, (unsigned int)(-no_bits) ) )
      {
         gan_err_register ( "gan_bit_array_shift_q", GAN_ERROR_FAILURE, "" );
         return GAN_FALSE;
      }
   }

   return GAN_TRUE;
}

/**
 * \brief Shift bit array in-place
 * \param ba Input bit array
 * \param no_bits Number of bits to shift bit array
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Shift input bit array \a ba by \a no_bits in-place. If \a no_bits is >= 0,
 * the shift is towards the end of the bit array, and vice veraa.
 */
Gan_BitArray *
 gan_bit_array_shift_s ( Gan_BitArray *ba, int no_bits )
{
   Gan_BitArray *ba_dest = gan_bit_array_alloc(ba->no_bits);

   if ( ba_dest == NULL || !gan_bit_array_shift_q ( ba, no_bits, ba_dest ) )
   {
      gan_err_register ( "gan_bit_array_shift_s", GAN_ERROR_FAILURE, "" );
      return NULL;
   }

   /* success */
   return ba_dest;
}

/**
 * \brief Get the first set bit in a bit array
 * \return The bit that is set or \c UINT_MAX if all are zero
 */
unsigned int
 gan_bit_array_get_first_set_bit(Gan_BitArray *ba)
{
   unsigned int word;
   int bit;

   for(word=0; word<ba->no_words; word++)
      if(ba->data[word] != 0)
      {
         /* find the bit */
#ifdef WORDS_BIGENDIAN
         for(bit = GAN_BITWORD_SIZE-1; bit >= 0; bit-- )
            if(ba->data[word] & ( GAN_MSB_SET >> bit ))
               return word*GAN_BITWORD_SIZE+bit;
#else
         for(bit = GAN_BITWORD_SIZE-1; bit >= 0; bit-- )
            if(ba->data[word] & ( GAN_LSB_SET << bit ))
               return word*GAN_BITWORD_SIZE+bit;
#endif
            
      }

   /* didn't find a bit */
   return UINT_MAX;
}

/**
 * \brief Fill part of a bit array.
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Fill part of a bit array, specified by the start point \a offset and the
 * given number of bits \a no_bits. The part is filled with the given boolean
 * value \a val.
 */
Gan_Bool
 gan_bit_array_fill_part ( Gan_BitArray *ba, unsigned int offset,
                           unsigned int no_bits, Gan_Bool val )
{
   div_t start_div, end_div;

   gan_err_test_bool ( offset+no_bits <= ba->no_bits,
                       "gan_bit_array_fill_part", GAN_ERROR_ILLEGAL_ARGUMENT,
                       "" );

   /* return immediately if no bits are to be filled */
   if ( no_bits == 0 ) return GAN_TRUE;

   /* compute word at start and end of filled part of row */
   start_div = div ( (int)offset, GAN_BITWORD_SIZE );
   end_div = div ( (int)(offset+no_bits-1), GAN_BITWORD_SIZE );

   if ( val )
   {
      /* fill with ones */
      if ( start_div.quot == end_div.quot )
         /* fill single word */
#ifdef WORDS_BIGENDIAN
         ba->data[start_div.quot] |=
               ((GAN_BITWORD_FULL << (GAN_BITWORD_SIZE
                                      - end_div.rem + start_div.rem - 1))
                >> start_div.rem);
#else      
         ba->data[start_div.quot] |=
               ((GAN_BITWORD_FULL << (GAN_BITWORD_SIZE
                                      - end_div.rem + start_div.rem - 1))
                >> (GAN_BITWORD_SIZE - end_div.rem - 1));
#endif
      else
      {
         int word;

         /* fill first word */
#ifdef WORDS_BIGENDIAN
         ba->data[start_div.quot] |= (GAN_BITWORD_FULL >> start_div.rem);
#else      
         ba->data[start_div.quot] |= (GAN_BITWORD_FULL << start_div.rem);
#endif
         
         /* fill middle words */
         for ( word = start_div.quot+1; word < end_div.quot; word++ )
            ba->data[word] = GAN_BITWORD_FULL;

         /* fill end word */
#ifdef WORDS_BIGENDIAN
         ba->data[end_div.quot] |= (GAN_BITWORD_FULL <<
                                    (GAN_BITWORD_SIZE - end_div.rem - 1));
#else      
         ba->data[end_div.quot] |= (GAN_BITWORD_FULL >>
                                    (GAN_BITWORD_SIZE - end_div.rem - 1));
#endif
      }
   }
   else
   {
      if ( start_div.quot == end_div.quot )
      {
         /* fill single word */
         if ( start_div.rem == 0 )
         {
            if ( end_div.rem == GAN_BITWORD_SIZE-1 )
               ba->data[start_div.quot] = 0;
            else
#ifdef WORDS_BIGENDIAN
               ba->data[start_div.quot] &= (GAN_BITWORD_FULL >>
                                            (end_div.rem+1));
            
#else      
               ba->data[start_div.quot] &= (GAN_BITWORD_FULL <<
                                            (end_div.rem+1));
#endif
         }
         else if ( end_div.rem == GAN_BITWORD_SIZE-1 )
         {
#ifdef WORDS_BIGENDIAN
            ba->data[start_div.quot] &=
                      (GAN_BITWORD_FULL << (GAN_BITWORD_SIZE - start_div.rem));
#else      
            ba->data[start_div.quot] &=
                      (GAN_BITWORD_FULL >> (GAN_BITWORD_SIZE - start_div.rem));
#endif
         }
         else
         {
#ifdef WORDS_BIGENDIAN
            ba->data[start_div.quot] &=
                ((GAN_BITWORD_FULL << (GAN_BITWORD_SIZE - start_div.rem))
                 | (GAN_BITWORD_FULL >> (end_div.rem + 1)));
#else      
            ba->data[start_div.quot] &=
                ((GAN_BITWORD_FULL >> (GAN_BITWORD_SIZE - start_div.rem))
                 | (GAN_BITWORD_FULL << (end_div.rem + 1)));
#endif
         }
      }
      else
      {
         int word;

         /* fill first word */
         if ( start_div.rem == 0 )
            ba->data[start_div.quot] = 0;
         else
         {
#ifdef WORDS_BIGENDIAN
            ba->data[start_div.quot] &=
                      (GAN_BITWORD_FULL << (GAN_BITWORD_SIZE - start_div.rem));
#else      
            ba->data[start_div.quot] &=
                      (GAN_BITWORD_FULL >> (GAN_BITWORD_SIZE - start_div.rem));
#endif
         }

         /* fill middle words */
         for ( word = start_div.quot+1; word < end_div.quot; word++ )
            ba->data[word] = 0;

         /* fill end word */
         if ( end_div.rem == GAN_BITWORD_SIZE-1 )
            ba->data[end_div.quot] = 0;
         else
         {
#ifdef WORDS_BIGENDIAN
            ba->data[end_div.quot] &= (GAN_BITWORD_FULL >> (end_div.rem + 1));
#else      
            ba->data[end_div.quot] &= (GAN_BITWORD_FULL << (end_div.rem + 1));
#endif
         }
      }
   }

   /* success */
   return GAN_TRUE;
}

/**
 * \brief Invert part of a bit array.
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Invert part of a bit array, specified by the start point \a offset and the
 * given number of bits \a no_bits.
 */
Gan_Bool
 gan_bit_array_invert_part ( Gan_BitArray *ba, unsigned int offset,
                             unsigned int no_bits )
{
   div_t start_div, end_div;

   gan_err_test_bool ( offset+no_bits <= ba->no_bits,
                       "gan_bit_array_invert_part", GAN_ERROR_ILLEGAL_ARGUMENT,
                       "" );

   /* return immediately if no bits are to be filled */
   if ( no_bits == 0 ) return GAN_TRUE;

   /* compute word at start and end of filled part of row */
   start_div = div ( (int)offset, GAN_BITWORD_SIZE );
   end_div = div ( (int)(offset+no_bits-1), GAN_BITWORD_SIZE );

   if ( start_div.quot == end_div.quot )
      /* fill single word */
#ifdef WORDS_BIGENDIAN
      ba->data[start_div.quot] ^=
               ((GAN_BITWORD_FULL << (GAN_BITWORD_SIZE
                                      - end_div.rem + start_div.rem - 1))
                >> start_div.rem);
#else      
      ba->data[start_div.quot] ^=
               ((GAN_BITWORD_FULL << (GAN_BITWORD_SIZE
                                      - end_div.rem + start_div.rem - 1))
                >> (GAN_BITWORD_SIZE - end_div.rem - 1));
#endif
   else
   {
      int word;

      /* fill first word */
#ifdef WORDS_BIGENDIAN
      ba->data[start_div.quot] ^= (GAN_BITWORD_FULL >> start_div.rem);
#else      
      ba->data[start_div.quot] ^= (GAN_BITWORD_FULL << start_div.rem);
#endif
      /* fill middle words */
      for ( word = start_div.quot+1; word < end_div.quot; word++ )
         ba->data[word] ^= GAN_BITWORD_FULL;

      /* fill end word */
#ifdef WORDS_BIGENDIAN
      ba->data[end_div.quot] ^= (GAN_BITWORD_FULL <<
                                 (GAN_BITWORD_SIZE - end_div.rem - 1));
#else      
      ba->data[end_div.quot] ^= (GAN_BITWORD_FULL >>
                                 (GAN_BITWORD_SIZE - end_div.rem - 1));
#endif
   }
   
   /* success */
   return GAN_TRUE;
}

/**
 * \brief Print bit array in ASCII to file.
 */
void
 gan_bit_array_fprint ( FILE *fp, const Gan_BitArray *ba, int indent )
{
   unsigned int i;

   /* print indentation */
   for ( indent--; indent >= 0; indent-- ) fprintf ( fp, " " );

   /* print bit array */
   for ( i = 0; i < ba->no_bits; i++ )
      fprintf ( fp, "%c", gan_bit_array_get_bit(ba,i) ? '1' : '0' );

   /* print end of line character */
   fprintf ( fp, "\n" );
}

/**
 * \}
 */

/**
 * \}
 */

/**
 * File:          $RCSfile: common_test.c,v $
 * Module:        Common package test program
 * Part of:       Gandalf Library
 *
 * Revision:      $Revision: 1.1.2.1 $
 * Last edited:   $Date: 2007/10/14 02:11:44 $
 * Author:        $Author: ruetz $
 * Copyright:     (c) 2000 Imagineer Software Limited
 *
 * Notes:         Tests parts of the common package.
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

#include <stdlib.h>
#include <stdio.h>

#include <gandalf/TestFramework/cUnit.h>
#include <gandalf/common/common_test.h>
#include <gandalf/common/numerics.h>
#include <gandalf/common/bit_array.h>
#include <gandalf/common/misc_error.h>
#include <gandalf/common/allocate.h>
#include <gandalf/common/numerics.h>

static Gan_Bool setup_test(void)
{
   printf("\nSetup for common_test completed!\n\n");
   return GAN_TRUE;
}

static Gan_Bool teardown_test(void)
{
   printf("\nTeardown for common_test completed!\n\n");
   return GAN_TRUE;
}

/* test quadratic equation solver */
static Gan_Bool test_quadratic(void)
{
   int i, n, j;
   double a, b, c;
   Gan_Complex x[2];

   for ( i = 0; i < 1000; i++ )
   {
      /* create coefficients */
      a = gan_random_m11();
      b = gan_random_m11();
      c = gan_random_m11();

      /* ignore trivial case that the quadratic term is zero */
      if ( fabs(a) < 1.0e-3 )
      {
         i--;
         continue;
      }

      /* solve quadratic equation */
      n = gan_solve_quadratic ( a, b, c, x );

      /* check that the number of solutions is correct */
      cu_assert ( n == 2 );

      /* test solution */
      for ( j = 0; j < n; j++ )
      {
         double tr=0.0, ti=0.0;

         /* quadratic term */
         tr += a*(x[j].r*x[j].r - x[j].i*x[j].i);
         ti += 2.0*a*x[j].r*x[j].i;
         
         /* linear term */
         tr += b*x[j].r;
         ti += b*x[j].i;

         /* constant term */
         tr += c;

         cu_assert ( fabs(tr) < 1.0e-12 && fabs(ti) < 1.0e-12 );
      }
   }
      
   return GAN_TRUE;
}

/* test cubic equation solver */
static Gan_Bool test_cubic(void)
{
   int i, n, j;
   double a, b, c, d;
   Gan_Complex x[3];

   for ( i = 0; i < 1000; i++ )
   {
      /* create coefficients */
      a = gan_random_m11();
      b = gan_random_m11();
      c = gan_random_m11();
      d = gan_random_m11();

      /* ignore trivial case that the cubic term is zero */
      if ( fabs(a) < 1.0e-3 )
      {
         i--;
         continue;
      }

      /* solve cubic equation */
      n = gan_solve_cubic ( a, b, c, d, x );

      /* check that the number of solutions is correct */
      cu_assert ( n == 3 );

      /* test solution */
      for ( j = 0; j < n; j++ )
      {
         double tr=0.0, ti=0.0;

         /* cubic term */
         tr += a*x[j].r*(x[j].r*x[j].r - 3.0*x[j].i*x[j].i);
         ti += a*x[j].i*(3.0*x[j].r*x[j].r - x[j].i*x[j].i);
         
         /* quadratic term */
         tr += b*(x[j].r*x[j].r - x[j].i*x[j].i);
         ti += 2.0*b*x[j].r*x[j].i;

         /* linear term */
         tr += c*x[j].r;
         ti += c*x[j].i;

         /* constant term */
         tr += d;

         cu_assert ( fabs(tr) < 1.0e-10 && fabs(ti) < 1.0e-10 );
      }
   }
      
   return GAN_TRUE;
}

/* returns GAN_TRUE if the two bit arrays are equal, GAN_FALSE otherwise */
static Gan_Bool bit_arrays_equal ( Gan_BitArray *ba1, Gan_BitArray *ba2 )
{
   int i;

   cu_assert ( ba1->no_words == ba2->no_words );
   cu_assert ( ba1->no_bits  == ba2->no_bits  );
   
   for ( i = (int)ba1->no_bits-1; i >= 0; i-- )
      cu_assert ( gan_bit_array_get_bit ( ba1, i ) ==
                  gan_bit_array_get_bit ( ba2, i ) );
   
   return GAN_TRUE;
}

#if 0
/* safe versions of bit array functions */
static Gan_Bool safe_bit_array_fill ( Gan_BitArray *ba, Gan_Bool val )
{
   int i;

   if ( val )
   {
      for ( i = (int)ba->no_bits-1; i >= 0; i-- )
         cu_assert ( gan_bit_array_set_bit ( ba, i ) );
   }
   else
   {
      for ( i = (int)ba->no_bits-1; i >= 0; i-- )
         cu_assert ( gan_bit_array_clear_bit ( ba, i ) );
   }

   return GAN_TRUE;
}
#endif

/* insert part of src bit array into dst bit array */
static Gan_Bool
 safe_bit_array_insert ( Gan_BitArray *source, unsigned int offset_s,
                         Gan_BitArray *dest,   unsigned int offset_d,
                         unsigned int no_bits )
{
   unsigned i;

   for ( i = 0; i < no_bits; i++ )
      if ( gan_bit_array_get_bit ( source, offset_s+i ) )
      {
         cu_assert ( gan_bit_array_set_bit ( dest, offset_d+i ) );
      }
      else
      {
         cu_assert ( gan_bit_array_clear_bit ( dest, offset_d+i ) );
      }

   return GAN_TRUE;
}

/* shift bit array */
static Gan_Bool
 safe_bit_array_shift ( Gan_BitArray *source, int no_bits,
                        Gan_BitArray *dest )
{
   int i;

   gan_bit_array_fill(dest, GAN_FALSE);
   for ( i = 0; i < (int)source->no_bits; i++ )
      if(i+no_bits >= 0 && i+no_bits<(int)source->no_bits &&
         gan_bit_array_get_bit ( source, i ))
         cu_assert ( gan_bit_array_set_bit ( dest, (unsigned)(i+no_bits) ) );

   return GAN_TRUE;
}

/* fill part of bit array */
static Gan_Bool
 safe_bit_array_fill_part ( Gan_BitArray *ba, unsigned int offset,
                            unsigned int no_bits, Gan_Bool val )
{
   unsigned i;

   if ( val )
   {
      for ( i = 0; i < no_bits; i++ )
         cu_assert ( gan_bit_array_set_bit ( ba, offset+i ) );
   }
   else
   {
      for ( i = 0; i < no_bits; i++ )
         cu_assert ( gan_bit_array_clear_bit ( ba, offset+i ) );
   }

   return GAN_TRUE;
}

static Gan_Bool
 random_bit_array ( Gan_BitArray *ba )
{
   unsigned i;

   for ( i = 0; i < ba->no_bits; i++ )
      if ( gan_random_01() > 0.5 )
      {
         cu_assert ( gan_bit_array_set_bit ( ba, i ) );
      }
      else
      {
         cu_assert ( gan_bit_array_clear_bit ( ba, i ) );
      }

   return GAN_TRUE;
}

static Gan_Bool test_bit_array(void)
{
   Gan_BitArray *b1, *b2; /* test bit arrays */
   Gan_BitArray *b1s, *b2s; /* safely constructed bit arrays */

#if GAN_BITWORD_SIZE == 64
   b1 = gan_bit_array_alloc(400);
   b2 = gan_bit_array_alloc(400);
   b1s = gan_bit_array_alloc(400);
   b2s = gan_bit_array_alloc(400);
#else /* #if GAN_BITWORD_SIZE == 32 */
   b1 = gan_bit_array_alloc(200);
   b2 = gan_bit_array_alloc(200);
   b1s = gan_bit_array_alloc(200);
   b2s = gan_bit_array_alloc(200);
#endif /* #if GAN_BITWORD_SIZE == 64 */
   cu_assert ( b1 != NULL && b2 != NULL && b1s != NULL && b2s != NULL );

   /* initialise bit arrays */
   cu_assert ( random_bit_array ( b1 ) );
   cu_assert ( random_bit_array ( b2 ) );
   cu_assert ( gan_bit_array_copy_q ( b1, b1s ) );
   cu_assert ( gan_bit_array_copy_q ( b2, b2s ) );
   cu_assert ( bit_arrays_equal ( b1, b1s ) );
   cu_assert ( bit_arrays_equal ( b2, b2s ) );

   /* Part of word inserted into part of other word */
#if GAN_BITWORD_SIZE == 64
   cu_assert ( gan_bit_array_insert  ( b1,  10, b2,  6, 20 ) );
   cu_assert ( safe_bit_array_insert ( b1s, 10, b2s, 6, 20 ) );
#else /* #if GAN_BITWORD_SIZE == 32 */
   cu_assert ( gan_bit_array_insert  ( b1,  10, b2,  3, 10 ) );
   cu_assert ( safe_bit_array_insert ( b1s, 10, b2s, 3, 10 ) );
#endif /* #if GAN_BITWORD_SIZE == 64 */
   cu_assert ( bit_arrays_equal ( b2, b2s ) );

   /* Part of word inserted into two other words */
#if (GAN_BITWORD_SIZE == 64)
   cu_assert ( gan_bit_array_insert  ( b1,  30, b2,  45, 30 ) );
   cu_assert ( safe_bit_array_insert ( b1s, 30, b2s, 45, 30 ) );
#else /* #if GAN_BITWORD_SIZE == 32 */
   cu_assert ( gan_bit_array_insert  ( b1,  10, b2,  25, 10 ) );
   cu_assert ( safe_bit_array_insert ( b1s, 10, b2s, 25, 10 ) );
#endif /* #if GAN_BITWORD_SIZE == 64 */
   cu_assert ( bit_arrays_equal ( b2, b2s ) );

   /* Part of two words inserted into single word */
#if (GAN_BITWORD_SIZE == 64)
   cu_assert ( gan_bit_array_insert  ( b1,  50, b2,  20, 20 ) );
   cu_assert ( safe_bit_array_insert ( b1s, 50, b2s, 20, 20 ) );
#else /* #if GAN_BITWORD_SIZE == 32 */
   cu_assert ( gan_bit_array_insert  ( b1,  25, b2,  10, 10 ) );
   cu_assert ( safe_bit_array_insert ( b1s, 25, b2s, 10, 10 ) );
#endif /* #if GAN_BITWORD_SIZE == 64 */
   cu_assert ( bit_arrays_equal ( b2, b2s ) );

   /* Part of two words inserted into two words - positive skew*/
#if (GAN_BITWORD_SIZE == 64)
   cu_assert ( gan_bit_array_insert  ( b1,  50, b2,  60, 20 ) );
   cu_assert ( safe_bit_array_insert ( b1s, 50, b2s, 60, 20 ) );
#else /* #if GAN_BITWORD_SIZE == 32 */
   cu_assert ( gan_bit_array_insert  ( b1,  25, b2,  30, 10 ) );
   cu_assert ( safe_bit_array_insert ( b1s, 25, b2s, 30, 10 ) );
#endif /* #if GAN_BITWORD_SIZE == 64 */
   cu_assert ( bit_arrays_equal ( b2, b2s ) );

   /* Part of two words inserted into two words - negative skew*/
#if (GAN_BITWORD_SIZE == 64)
   cu_assert ( gan_bit_array_insert  ( b1,  50, b2,  48, 20 ) );
   cu_assert ( safe_bit_array_insert ( b1s, 50, b2s, 48, 20 ) );
#else /* #if GAN_BITWORD_SIZE == 32 */
   cu_assert ( gan_bit_array_insert  ( b1,  25, b2,  24, 10 ) );
   cu_assert ( safe_bit_array_insert ( b1s, 25, b2s, 24, 10 ) );
#endif /* #if GAN_BITWORD_SIZE == 64 */
   cu_assert ( bit_arrays_equal ( b2, b2s ) );

   /* Part of two words inserted into three words */
#if (GAN_BITWORD_SIZE == 64)
   cu_assert ( gan_bit_array_insert  ( b1,  4, b2,  20, 120 ) );
   cu_assert ( safe_bit_array_insert ( b1s, 4, b2s, 20, 120 ) );
#else /* #if GAN_BITWORD_SIZE == 32 */
   cu_assert ( gan_bit_array_insert  ( b1,  2, b2,  10, 60 ) );
   cu_assert ( safe_bit_array_insert ( b1s, 2, b2s, 10, 60 ) );
#endif /* #if GAN_BITWORD_SIZE == 64 */
   cu_assert ( bit_arrays_equal ( b2, b2s ) );

   /* Part of x words inserted into x-1 words */
#if (GAN_BITWORD_SIZE == 64)
   cu_assert ( gan_bit_array_insert  ( b1,  60, b2,  20, 80 ) );
   cu_assert ( safe_bit_array_insert ( b1s, 60, b2s, 20, 80 ) );
#else /* #if GAN_BITWORD_SIZE == 32 */
   cu_assert ( gan_bit_array_insert  ( b1,  30, b2,  10, 40 ) );
   cu_assert ( safe_bit_array_insert ( b1s, 30, b2s, 10, 40 ) );
#endif /* #if GAN_BITWORD_SIZE == 64 */
   cu_assert ( bit_arrays_equal ( b2, b2s ) );

   /* Part of x words inserted into x words - negative skew*/
#if (GAN_BITWORD_SIZE == 64)
   cu_assert ( gan_bit_array_insert  ( b1,  10, b2,  16, 200 ) );
   cu_assert ( safe_bit_array_insert ( b1s, 20, b2s, 16, 200 ) );
#else /* #if GAN_BITWORD_SIZE == 32 */
   cu_assert ( gan_bit_array_insert  ( b1,  10, b2,  8, 100 ) );
   cu_assert ( safe_bit_array_insert ( b1s, 10, b2s, 8, 100 ) );
#endif /* #if GAN_BITWORD_SIZE == 64 */
   cu_assert ( bit_arrays_equal ( b2, b2s ) );

   /* Part of x words inserted into x words - positive skew */
#if (GAN_BITWORD_SIZE == 64)
   cu_assert ( gan_bit_array_insert  ( b1,  20, b2,  24, 200 ) );
   cu_assert ( safe_bit_array_insert ( b1s, 20, b2s, 24, 200 ) );
#else /* #if GAN_BITWORD_SIZE == 32 */
   cu_assert ( gan_bit_array_insert  ( b1,  10, b2,  12, 100 ) );
   cu_assert ( safe_bit_array_insert ( b1s, 10, b2s, 12, 100 ) );
#endif /* #if GAN_BITWORD_SIZE == 64 */
   cu_assert ( bit_arrays_equal ( b2, b2s ) );

   /* Part of x words inserted into x words */
#if (GAN_BITWORD_SIZE == 64)
   cu_assert ( gan_bit_array_insert  ( b1,  40, b2,  60, 142 ) );
   cu_assert ( safe_bit_array_insert ( b1s, 40, b2s, 60, 142 ) );
#else /* #if GAN_BITWORD_SIZE == 32 */
   cu_assert ( gan_bit_array_insert  ( b1,  20, b2,  30, 71 ) );
   cu_assert ( safe_bit_array_insert ( b1s, 20, b2s, 30, 71 ) );
#endif /* #if GAN_BITWORD_SIZE == 64 */
   cu_assert ( bit_arrays_equal ( b2, b2s ) );

   /* test part filling in a word with ones */
   cu_assert ( gan_bit_array_fill_part  ( b1,  0, GAN_BITWORD_SIZE,
                                          GAN_TRUE ) );
   cu_assert ( safe_bit_array_fill_part ( b1s, 0, GAN_BITWORD_SIZE,
                                          GAN_TRUE ) );
   cu_assert ( bit_arrays_equal ( b1, b1s ) );

   /* test part filling in a word with zeros */
   cu_assert ( gan_bit_array_fill_part  ( b1,  0, GAN_BITWORD_SIZE,
                                          GAN_FALSE ) );
   cu_assert ( safe_bit_array_fill_part ( b1s, 0, GAN_BITWORD_SIZE,
                                          GAN_FALSE ) );
   cu_assert ( bit_arrays_equal ( b1, b1s ) );

   /* test part filling in a word with ones */
#if (GAN_BITWORD_SIZE == 64)
   cu_assert ( gan_bit_array_fill_part  ( b1,  0, 54, GAN_TRUE ) );
   cu_assert ( safe_bit_array_fill_part ( b1s, 0, 54, GAN_TRUE ) );
#else /* #if GAN_BITWORD_SIZE == 32 */
   cu_assert ( gan_bit_array_fill_part  ( b1,  0, 27, GAN_TRUE ) );
   cu_assert ( safe_bit_array_fill_part ( b1s, 0, 27, GAN_TRUE ) );
#endif /* #if GAN_BITWORD_SIZE == 64 */
   cu_assert ( bit_arrays_equal ( b1, b1s ) );

   /* test part filling in a word with zeros */
#if (GAN_BITWORD_SIZE == 64)
   cu_assert ( gan_bit_array_fill_part  ( b1,  0, 54, GAN_FALSE ) );
   cu_assert ( safe_bit_array_fill_part ( b1s, 0, 54, GAN_FALSE ) );
#else /* #if GAN_BITWORD_SIZE == 32 */
   cu_assert ( gan_bit_array_fill_part  ( b1,  0, 27, GAN_FALSE ) );
   cu_assert ( safe_bit_array_fill_part ( b1s, 0, 27, GAN_FALSE ) );
#endif /* #if GAN_BITWORD_SIZE == 64 */
   cu_assert ( bit_arrays_equal ( b1, b1s ) );

   /* test part filling in a word with ones */
#if (GAN_BITWORD_SIZE == 64)
   cu_assert ( gan_bit_array_fill_part  ( b1,  16, 48, GAN_TRUE ) );
   cu_assert ( safe_bit_array_fill_part ( b1s, 16, 48, GAN_TRUE ) );
#else /* #if GAN_BITWORD_SIZE == 32 */
   cu_assert ( gan_bit_array_fill_part  ( b1,  8, 24, GAN_TRUE ) );
   cu_assert ( safe_bit_array_fill_part ( b1s, 8, 24, GAN_TRUE ) );
#endif /* #if GAN_BITWORD_SIZE == 64 */
   cu_assert ( bit_arrays_equal ( b1, b1s ) );

   /* test part filling in a word with zeros */
#if (GAN_BITWORD_SIZE == 64)
   cu_assert ( gan_bit_array_fill_part  ( b1,  16, 48, GAN_FALSE ) );
   cu_assert ( safe_bit_array_fill_part ( b1s, 16, 48, GAN_FALSE ) );
#else /* #if GAN_BITWORD_SIZE == 32 */
   cu_assert ( gan_bit_array_fill_part  ( b1,  8, 24, GAN_FALSE ) );
   cu_assert ( safe_bit_array_fill_part ( b1s, 8, 24, GAN_FALSE ) );
#endif /* #if GAN_BITWORD_SIZE == 64 */
   cu_assert ( bit_arrays_equal ( b1, b1s ) );

   /* test part filling in a word with ones */
#if (GAN_BITWORD_SIZE == 64)
   cu_assert ( gan_bit_array_fill_part  ( b1,  12, 48, GAN_TRUE ) );
   cu_assert ( safe_bit_array_fill_part ( b1s, 12, 48, GAN_TRUE ) );
#else /* #if GAN_BITWORD_SIZE == 32 */
   cu_assert ( gan_bit_array_fill_part  ( b1,  6, 24, GAN_TRUE ) );
   cu_assert ( safe_bit_array_fill_part ( b1s, 6, 24, GAN_TRUE ) );
#endif /* #if GAN_BITWORD_SIZE == 64 */
   cu_assert ( bit_arrays_equal ( b1, b1s ) );

   /* test part filling in a word with zeros */
#if (GAN_BITWORD_SIZE == 64)
   cu_assert ( gan_bit_array_fill_part  ( b1,  12, 48, GAN_FALSE ) );
   cu_assert ( safe_bit_array_fill_part ( b1s, 12, 48, GAN_FALSE ) );
#else /* #if GAN_BITWORD_SIZE == 32 */
   cu_assert ( gan_bit_array_fill_part  ( b1,  6, 24, GAN_FALSE ) );
   cu_assert ( safe_bit_array_fill_part ( b1s, 6, 24, GAN_FALSE ) );
#endif /* #if GAN_BITWORD_SIZE == 64 */
   cu_assert ( bit_arrays_equal ( b1, b1s ) );

   /* test part filling across words with ones */
#if (GAN_BITWORD_SIZE == 64)
   cu_assert ( gan_bit_array_fill_part  ( b1,  0, 138, GAN_TRUE ) );
   cu_assert ( safe_bit_array_fill_part ( b1s, 0, 138, GAN_TRUE ) );
#else /* #if GAN_BITWORD_SIZE == 32 */
   cu_assert ( gan_bit_array_fill_part  ( b1,  0, 69, GAN_TRUE ) );
   cu_assert ( safe_bit_array_fill_part ( b1s, 0, 69, GAN_TRUE ) );
#endif /* #if GAN_BITWORD_SIZE == 64 */
   cu_assert ( bit_arrays_equal ( b1, b1s ) );

   /* test part filling across words with zeros */
#if (GAN_BITWORD_SIZE == 64)
   cu_assert ( gan_bit_array_fill_part  ( b1,  0, 138, GAN_FALSE ) );
   cu_assert ( safe_bit_array_fill_part ( b1s, 0, 138, GAN_FALSE ) );
#else /* #if GAN_BITWORD_SIZE == 32 */
   cu_assert ( gan_bit_array_fill_part  ( b1,  0, 69, GAN_FALSE ) );
   cu_assert ( safe_bit_array_fill_part ( b1s, 0, 69, GAN_FALSE ) );
#endif /* #if GAN_BITWORD_SIZE == 64 */
   cu_assert ( bit_arrays_equal ( b1, b1s ) );

   /* test part filling across words with ones */
#if (GAN_BITWORD_SIZE == 64)
   cu_assert ( gan_bit_array_fill_part  ( b1,  22, 138, GAN_TRUE ) );
   cu_assert ( safe_bit_array_fill_part ( b1s, 22, 138, GAN_TRUE ) );
#else /* #if GAN_BITWORD_SIZE == 32 */
   cu_assert ( gan_bit_array_fill_part  ( b1,  11, 69, GAN_TRUE ) );
   cu_assert ( safe_bit_array_fill_part ( b1s, 11, 69, GAN_TRUE ) );
#endif /* #if GAN_BITWORD_SIZE == 64 */
   cu_assert ( bit_arrays_equal ( b1, b1s ) );

   /* test part filling across words with zeros */
#if (GAN_BITWORD_SIZE == 64)
   cu_assert ( gan_bit_array_fill_part  ( b1,  22, 138, GAN_FALSE ) );
   cu_assert ( safe_bit_array_fill_part ( b1s, 22, 138, GAN_FALSE ) );
#else /* #if GAN_BITWORD_SIZE == 32 */
   cu_assert ( gan_bit_array_fill_part  ( b1,  11, 69, GAN_FALSE ) );
   cu_assert ( safe_bit_array_fill_part ( b1s, 11, 69, GAN_FALSE ) );
#endif /* #if GAN_BITWORD_SIZE == 64 */
   cu_assert ( bit_arrays_equal ( b1, b1s ) );

   /* test bit shifting */
   {
      int it, ishift;
      for(it=0; it<100; it++)
      {
         cu_assert ( random_bit_array ( b1 ) );
         cu_assert ( gan_bit_array_copy_q ( b1, b1s ) );

         /* choose a random shift between -100 and 100 */
         ishift = (gan_random() % 200) - 100;
         cu_assert ( gan_bit_array_shift_q ( b1, ishift, b2 ) );
         cu_assert ( safe_bit_array_shift ( b1s, ishift, b2s ) );
         cu_assert ( bit_arrays_equal ( b2, b2s ) );
      }
   }
         
   /* success */
   gan_bit_array_free_va ( b2s, b1s, b2, b1, NULL );
   return GAN_TRUE;
}

static int
 compar ( const void *ptr1, const void *ptr2 )
{
   int i1 = *((const int *)ptr1), i2 = *((const int *)ptr2);
   if ( i1 < i2 ) return -1;
   else if ( i1 == i2 ) return 0;
   else return 1;
}

/* k'th element test function */
static Gan_Bool
 test_kth_element(void)
{
   int i;

   for(i=0; i < 100; i++)
   {
      /* choose size of array at random */
      size_t nmemb = 1 + (gan_random() % 100);
      int *array, *res;
      unsigned j;

      array = gan_malloc_array(int, nmemb);
      res = gan_malloc_array(int, nmemb);
      cu_assert ( array != NULL && res != NULL );

      for(j = 0; j < nmemb; j++)
         array[j] = gan_random() % 30;

      /* check each k'th element */
      for(j = 0; j < nmemb; j++)
         res[j] = *((int *)
                    gan_kth_highest(array, nmemb, sizeof(int), j, compar));

      /* sort array to make it easy */
      qsort ( array, nmemb, sizeof(int), compar );

      /* now check result */
      for(j = 0; j < nmemb; j++)
         cu_assert ( array[j] == res[j] );

      gan_free_va(res,array,NULL);
   }

   /* success */
   return GAN_TRUE;
}

/* Tests the common package */
static Gan_Bool run_test(void)
{
   /* test quadratic equation solver */
   cu_assert ( test_quadratic() );

   /* test cubic equation solver */
   cu_assert ( test_cubic() );

   /* test bit array functions */
   cu_assert ( test_bit_array() );

   /* test k'th element function */
   cu_assert ( test_kth_element() );

   return GAN_TRUE;
}

#ifdef COMMON_TEST_MAIN

int main ( int argc, char *argv[] )
{
   /* set default Gandalf error handler without trace handling */
   gan_err_set_reporter ( gan_err_default_reporter );
   gan_err_set_trace ( GAN_ERR_TRACE_OFF );

   setup_test();
   if ( run_test() )
      printf ( "Tests ran successfully!\n" );
   else
      printf ( "At least one test failed\n" );

   teardown_test();
   gan_heap_report(NULL);
   return 0;
}

#else

/* This function registers the unit tests to a cUnit_test_suite. */
cUnit_test_suite *common_test_build_suite(void)
{
   cUnit_test_suite *sp;

   /* Get a new test session */
   sp = cUnit_new_test_suite("common_test suite");

   cUnit_add_test(sp, "common_test", run_test);

   /* register a setup and teardown on the test suite 'common_test' */
   if (cUnit_register_setup(sp, setup_test) != GAN_TRUE)
      printf("Error while setting up test suite common_test");

   if (cUnit_register_teardown(sp, teardown_test) != GAN_TRUE)
      printf("Error while tearing down test suite common_test");

   return( sp );
}

#endif /* #ifdef COMMON_TEST_MAIN */

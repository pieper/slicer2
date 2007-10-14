/**
 * File:          $RCSfile: list_test.c,v $
 * Module:        Linked list test program
 * Part of:       Gandalf Library
 *
 * Revision:      $Revision: 1.1.2.1 $
 * Last edited:   $Date: 2007/10/14 02:11:45 $
 * Author:        $Author: ruetz $
 * Copyright:     (c) 2000 Imagineer Software Limited
 *
 * Notes:         Creates and tests the functions of doubly-linked lists
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
#include <gandalf/common/list_test.h>
#include <gandalf/common/linked_list.h>
#include <gandalf/common/misc_error.h>
#include <gandalf/common/allocate.h>

static Gan_List *dls1, *dls2, *dls3, *dls4;

/* Create and populate 2 lists */
static Gan_Bool setup_test(void)
{
   int i, *p;

   /***** Testing gan_list_insert_first *****/
   /* Create first list */
   dls1 = gan_list_new();

   for (i=0;i < 3;i++)
   {
      p = gan_malloc_object(int);
      cu_assert(p != NULL);
      *p = i;
      gan_list_insert_last(dls1,p);
   }

   /* Create second list */
   dls2 = gan_list_new();

   for (i=0;i < 3;i++)
   {
      p = gan_malloc_object(int);
      cu_assert(p != NULL);
      *p = i;
      gan_list_insert_last(dls2,p);
   }

   printf("\nSetup for list_test completed.\n\n");
   return GAN_TRUE;
}

/* Delete all nodes from the lists and free resources */
static Gan_Bool teardown_test(void)
{
   /***** Testing gan_list_free *****/
   gan_list_free(dls1, free);
   gan_list_free(dls2, free);
   /* Using NULL in the parameter list because the 3rd 
      and 4th lists are already empty so the delete_all will fail */
   gan_list_free(dls3, NULL);
   gan_list_free(dls4, NULL);

   printf("\nTeardown for list_test completed.\n\n");
   return GAN_TRUE;
}

/* Compare two values */
static int compare(const void *ppv1, const void *ppv2)
{
   int *pi1, *pi2;

   pi1 = *((int**) ppv1);
   pi2 = *((int**) ppv2);
   
   if (*pi1 > *pi2)
      return (1);
   
   if (*pi1 < *pi2)
      return (-1);

   return (0);
}

#if 0
/* Prints the values stored in each node */
static void print_list(int *item, char *msg)
{
   printf("%s %d\n",  msg, *item);
}
#endif

/* Tests all the doubly-linked list functions */
static Gan_Bool run_test(void)
{  
   int iCount, *p, **ppiArray;

   /***** Testing gan_list_insert_first *****/
   cu_assert(gan_list_get_size(dls1) == 3);
   cu_assert(gan_list_get_pos(dls1) == 2);

   for (iCount=3;iCount < 6;iCount++)
   {
      p = gan_malloc_object(int);
      cu_assert(p != NULL);
      *p = iCount;
      gan_list_insert_first(dls1,p);
   }

   cu_assert(gan_list_get_size(dls1) == 6);
   cu_assert(gan_list_get_pos(dls1) == 5);
   gan_list_goto_head(dls1);
   p = gan_list_get_next(dls1, int);
   cu_assert(*p == 5);
   p = gan_list_get_next(dls1, int);
   cu_assert(*p == 4);
   p = gan_list_get_next(dls1, int);
   cu_assert(*p == 3);

   /***** Testing gan_list_insert_next *****/
   cu_assert(gan_list_get_size(dls1) == 6);
   cu_assert(gan_list_get_pos(dls1) == 2);
   gan_list_goto_tail(dls1);

   for (iCount=6;iCount < 9;iCount++)
   {
      p = gan_malloc_object(int);
      cu_assert(p != NULL);
      *p = iCount;
      gan_list_insert_next(dls1,p);
   }

   cu_assert(gan_list_get_size(dls1) == 9);
   cu_assert(gan_list_get_pos(dls1) == 8);
   gan_list_goto_tail(dls1);
   p = gan_list_get_current(dls1, int);
   cu_assert(*p == 8);
   p = gan_list_get_prev(dls1, int);
   cu_assert(*p == 7);
   p = gan_list_get_prev(dls1, int);
   cu_assert(*p == 6);

   /***** Testing gan_list_insert_prev *****/
   cu_assert(gan_list_get_size(dls1) == 9);
   cu_assert(gan_list_get_pos(dls1) == 6);
   gan_list_goto_pos(dls1, 1);

   for (iCount=9;iCount < 12;iCount++)
   {
      p = gan_malloc_object(int);
      cu_assert(p != NULL);
      *p = iCount;
      gan_list_insert_prev(dls1,p);
   }

   cu_assert(gan_list_get_size(dls1) == 12);
   cu_assert(gan_list_get_pos(dls1) == 1);
   gan_list_goto_pos(dls1, 0);
   p = gan_list_get_next(dls1, int);
   cu_assert(*p == 11);
   p = gan_list_get_next(dls1, int);
   cu_assert(*p == 10);
   p = gan_list_get_next(dls1, int);
   cu_assert(*p == 9);

   /***** Testing gan_list_insert_last *****/
   cu_assert(gan_list_get_size(dls1) == 12);
   cu_assert(gan_list_get_pos(dls1) == 3);

   for (iCount=12;iCount < 15;iCount++)
   {
      p = gan_malloc_object(int);
      cu_assert(p != NULL);
      *p = iCount;
      gan_list_insert_last(dls1,p);
   }

   cu_assert(gan_list_get_size(dls1) == 15);
   cu_assert(gan_list_get_pos(dls1) == 14);
   gan_list_goto_tail(dls1);
   p = gan_list_get_current(dls1, int);
   cu_assert(*p == 14);
   p = gan_list_get_prev(dls1, int);
   cu_assert(*p == 13);
   p = gan_list_get_prev(dls1, int);
   cu_assert(*p == 12);

   /***** Testing insert_at *****/
   cu_assert(gan_list_get_size(dls1) == 15);
   cu_assert(gan_list_get_pos(dls1) == 12);

   for (iCount=15;iCount < 18;iCount++)
   {
      p = gan_malloc_object(int);
      cu_assert(p != NULL);
      *p = iCount;
      gan_list_insert_at(dls1,p, iCount);
   }

   cu_assert(gan_list_get_size(dls1) == 18);
   cu_assert(gan_list_get_pos(dls1) == 17);
   p = gan_list_get_current(dls1, int);
   cu_assert(*p == 17);
   p = gan_list_get_prev(dls1, int);
   cu_assert(*p == 16);
   p = gan_list_get_prev(dls1, int);
   cu_assert(*p == 15);

   /***** Testing gan_list_delete_first *****/
   cu_assert(gan_list_get_size(dls1) == 18);
   cu_assert(gan_list_get_pos(dls1) == 15);

   gan_list_delete_first(dls1, free);

   cu_assert(gan_list_get_size(dls1) == 17);
   cu_assert(gan_list_get_pos(dls1) == 14);
   gan_list_goto_head(dls1);
   p = gan_list_get_next(dls1, int);
   cu_assert(*p == 11);

   /***** Testing gan_list_delete_next *****/
   gan_list_goto_head(dls1);
   cu_assert(gan_list_get_size(dls1) == 17);
   cu_assert(gan_list_get_pos(dls1) == -1);

   gan_list_delete_next(dls1, free);

   cu_assert(gan_list_get_size(dls1) == 16);
   cu_assert(gan_list_get_pos(dls1) == -1);
   p = gan_list_get_next(dls1, int);
   cu_assert(*p == 10);

   /***** Testing gan_list_delete_current *****/
   gan_list_goto_pos(dls1, 15);
   cu_assert(gan_list_get_size(dls1) == 16);
   cu_assert(gan_list_get_pos(dls1) == 15);
        
   gan_list_delete_current(dls1, free);
        
   cu_assert(gan_list_get_size(dls1) == 15);
   cu_assert(gan_list_get_pos(dls1) == 14);
   p = gan_list_get_current(dls1, int);
   cu_assert(*p == 16);

   /***** Testing gan_list_delete_prev *****/
   gan_list_goto_pos(dls1, 14);
   cu_assert(gan_list_get_size(dls1) == 15);
   cu_assert(gan_list_get_pos(dls1) == 14);

   gan_list_delete_prev(dls1, free);

   cu_assert(gan_list_get_size(dls1) == 14);
   cu_assert(gan_list_get_pos(dls1) == 13);
   p = gan_list_get_current(dls1, int);
   cu_assert(*p == 16);

   /***** Testing gan_list_delete_last *****/
   cu_assert(gan_list_get_size(dls1) == 14);
   cu_assert(gan_list_get_pos(dls1) == 13);

   gan_list_delete_last(dls1, free);
        
   cu_assert(gan_list_get_size(dls1) == 13);
   cu_assert(gan_list_get_pos(dls1) == 12);
   p = gan_list_get_current(dls1, int);
   cu_assert(*p == 14);

   /***** Testing gan_list_delete_at *****/
   cu_assert(gan_list_get_size(dls1) == 13);
   cu_assert(gan_list_get_pos(dls1) == 12);
   
   gan_list_delete_at(dls1, free, 0);
        
   cu_assert(gan_list_get_size(dls1) == 12);
   cu_assert(gan_list_get_pos(dls1) == 11);
   gan_list_goto_head(dls1);
   p = gan_list_get_next(dls1, int);
   cu_assert(*p == 9);

   /***** Testing gan_list_set_prev *****/

   /* first free list element to be discarded */
   gan_list_goto_tail(dls1);
   free ( gan_list_get_prev(dls1, int) );
   
   gan_list_goto_tail(dls1);
   p = gan_malloc_object(int);
   cu_assert(p != NULL);
   *p = 9999;

   cu_assert(gan_list_get_size(dls1) == 12);
   cu_assert(gan_list_get_pos(dls1) == 11);

   gan_list_set_prev(dls1, p);

   cu_assert(gan_list_get_size(dls1) == 12);
   cu_assert(gan_list_get_pos(dls1) == 10);
   p = gan_list_get_current(dls1, int);
   cu_assert(*p == 9999);

   /***** Testing gan_list_set_current *****/
   gan_list_goto_tail(dls1);
   p = gan_malloc_object(int);
   cu_assert(p != NULL);
   *p = 8888;

   cu_assert(gan_list_get_size(dls1) == 12);
   cu_assert(gan_list_get_pos(dls1) == 11);

   /* first free list element to be discarded */
   free ( gan_list_get_current(dls1, int) );
   
   gan_list_set_current(dls1, p);

   cu_assert(gan_list_get_size(dls1) == 12);
   cu_assert(gan_list_get_pos(dls1) == 11);
   p = gan_list_get_current(dls1, int);
   cu_assert(*p == 8888);

   /***** Testing gan_list_set_next *****/

   /* first free list element to be discarded */
   gan_list_goto_head(dls1);
   free ( gan_list_get_next(dls1, int) );
   
   gan_list_goto_head(dls1);
   p = gan_malloc_object(int);
   cu_assert(p != NULL);
   *p = 1111;
        
   cu_assert(gan_list_get_size(dls1) == 12);
   cu_assert(gan_list_get_pos(dls1) == -1);

   gan_list_set_next(dls1, p);

   cu_assert(gan_list_get_size(dls1) == 12);
   cu_assert(gan_list_get_pos(dls1) == 0);
   p = gan_list_get_current(dls1, int);
   cu_assert(*p == 1111);

   /***** Testing gan_list_get_prev *****/
   gan_list_goto_tail(dls1);
   cu_assert(gan_list_get_size(dls1) == 12);
   cu_assert(gan_list_get_pos(dls1) == 11);

   p = gan_list_get_prev(dls1, int);
        
   cu_assert(gan_list_get_size(dls1) == 12);
   cu_assert(gan_list_get_pos(dls1) == 10);
   cu_assert(*p == 9999);

   /***** Testing gan_list_get_current *****/
   gan_list_goto_pos(dls1, 2);
   cu_assert(gan_list_get_size(dls1) == 12);
   cu_assert(gan_list_get_pos(dls1) == 2);

   p = gan_list_get_current(dls1, int);
        
   cu_assert(gan_list_get_size(dls1) == 12);
   cu_assert(gan_list_get_pos(dls1) == 2);
   cu_assert(*p == 3);

   /***** Testing gan_list_get_next *****/
   gan_list_goto_pos(dls1, 4);
   cu_assert(gan_list_get_size(dls1) == 12);
   cu_assert(gan_list_get_pos(dls1) == 4);

   p = gan_list_get_next(dls1, int);
        
   cu_assert(gan_list_get_size(dls1) == 12);
   cu_assert(gan_list_get_pos(dls1) == 5);
   cu_assert(*p == 2);

   /***** Testing gan_list_push_current *****/
   gan_list_goto_pos(dls1, 1);
   cu_assert(gan_list_get_size(dls1) == 12);
   cu_assert(gan_list_get_pos(dls1) == 1);

   gan_list_push_current (dls1);
        
   cu_assert(gan_list_get_size(dls1) == 12);
   cu_assert(gan_list_get_pos(dls1) == 1);
   gan_list_goto_tail(dls1);
   p = gan_list_get_current(dls1, int);
   cu_assert(*p == 8888);

   /***** Testing gan_list_pop_current *****/
   cu_assert(gan_list_get_size(dls1) == 12);
   cu_assert(gan_list_get_pos(dls1) == 11);

   gan_list_pop_current (dls1);
        
   cu_assert(gan_list_get_size(dls1) == 12);
   cu_assert(gan_list_get_pos(dls1) == 1);
   p = gan_list_get_current(dls1, int);
   cu_assert(*p == 4);

   /***** Testing gan_list_concat *****/
   gan_list_goto_tail(dls1);
   gan_list_goto_tail(dls2);
   cu_assert(gan_list_get_size(dls1) == 12);
   cu_assert(gan_list_get_pos(dls1) == 11);
   cu_assert(gan_list_get_size(dls2) == 3);
   cu_assert(gan_list_get_pos(dls2) == 2);

   dls3 = gan_list_concat (dls1, dls2);
        
   cu_assert(gan_list_get_size(dls3) == 15);
   cu_assert(gan_list_get_pos(dls3) == 14);
   gan_list_goto_head(dls3);
   p = gan_list_get_next(dls3, int);
   cu_assert(*p == 1111);
   gan_list_goto_pos(dls3, 11);
   p = gan_list_get_current(dls3, int);
   cu_assert(*p == 8888);
   p = gan_list_get_next(dls3, int);
   cu_assert(*p == 0);
   gan_list_goto_tail(dls3);
   p = gan_list_get_current(dls3, int);
   cu_assert(*p == 2);

   /***** Testing gan_list_reverse *****/
   gan_list_goto_head(dls1);
   cu_assert(gan_list_get_size(dls1) == 12);
   cu_assert(gan_list_get_pos(dls1) == -1);

   gan_list_reverse (dls1);
        
   cu_assert(gan_list_get_size(dls1) == 12);
   cu_assert(gan_list_get_pos(dls1) == -1);
   gan_list_goto_head(dls1);
   p = gan_list_get_next(dls1, int);
   cu_assert(*p == 8888);
   p = gan_list_get_next(dls1, int);
   cu_assert(*p == 9999);
   gan_list_goto_pos(dls1, 6);
   p = gan_list_get_current(dls1, int);
   cu_assert(*p == 2);
   gan_list_goto_tail(dls1);
   p = gan_list_get_current(dls1, int);
   cu_assert(*p == 1111);

   /***** Testing gan_list_copy *****/
   gan_list_goto_head(dls1);
   cu_assert(gan_list_get_size(dls1) == 12);
   cu_assert(gan_list_get_pos(dls1) == -1);

   dls4 = gan_list_copy (dls1);

   cu_assert(gan_list_get_size(dls4) == 12);
   cu_assert(gan_list_get_pos(dls4) == 11);
   gan_list_goto_head(dls4);
   p = gan_list_get_next(dls4, int);
   cu_assert(*p == 8888);
   p = gan_list_get_next(dls4, int);
   cu_assert(*p == 9999);
   gan_list_goto_pos(dls4, 6);
   p = gan_list_get_current(dls4, int);
   cu_assert(*p == 2);
   gan_list_goto_tail(dls4);
   p = gan_list_get_current(dls4, int);
   cu_assert(*p == 1111);

   /***** Testing gan_list_make_straight *****/
   gan_list_goto_tail(dls1);
   cu_assert(gan_list_get_size(dls1) == 12);
   cu_assert(gan_list_get_pos(dls1) == 11);

   gan_list_make_straight (dls1);
        
   cu_assert(gan_list_get_size(dls1) == 12);
   cu_assert(gan_list_get_pos(dls1) == 11);
   
   /***** Testing gan_list_is_circular *****/
   gan_list_goto_head(dls1);
   cu_assert(gan_list_get_size(dls1) == 12);
   cu_assert(gan_list_get_pos(dls1) == -1);

   cu_assert(gan_list_is_circular (dls1) == GAN_FALSE);

   cu_assert(gan_list_get_size(dls1) == 12);
   cu_assert(gan_list_get_pos(dls1) == -1);

   /***** Testing gan_list_make_circular *****/
   gan_list_goto_tail(dls1);
   cu_assert(gan_list_get_size(dls1) == 12);
   cu_assert(gan_list_get_pos(dls1) == 11);

   gan_list_make_circular (dls1);

   cu_assert(gan_list_get_size(dls1) == 12);
   cu_assert(gan_list_get_pos(dls1) == 11);
   p = gan_list_get_next(dls1, int);
   cu_assert(*p == 8888);
   p = gan_list_get_prev(dls1, int);
   cu_assert(*p == 1111);

   /***** Testing gan_list_is_circular *****/
   gan_list_goto_head(dls1);
   cu_assert(gan_list_get_size(dls1) == 12);
   cu_assert(gan_list_get_pos(dls1) == -1);

   cu_assert(gan_list_is_circular (dls1) == GAN_TRUE);

   cu_assert(gan_list_get_size(dls1) == 12);
   cu_assert(gan_list_get_pos(dls1) == -1);

   /***** Testing gan_list_to_array *****/
   gan_list_goto_head(dls4);
   cu_assert(gan_list_get_size(dls4) == 12);
   cu_assert(gan_list_get_pos(dls4) == -1);

   cu_assert((gan_list_to_array (dls4, (void ***)&ppiArray)) == 12);

   cu_assert(*(ppiArray[0]) == 8888);
   cu_assert(*(ppiArray[1]) == 9999);
   cu_assert(*(ppiArray[5]) == 6);
   cu_assert(*(ppiArray[11]) == 1111);
   cu_assert(gan_list_get_size(dls4) == 12);
   cu_assert(gan_list_get_pos(dls4) == 11);
   free(ppiArray);

   /***** Testing gan_list_sort_asc *****/
   gan_list_goto_head(dls1);
   cu_assert(gan_list_get_size(dls1) == 12);
   cu_assert(gan_list_get_pos(dls1) == -1);
   gan_list_sort_asc (dls1, compare);

   cu_assert(gan_list_get_size(dls1) == 12);
   cu_assert(gan_list_get_pos(dls1) == 11);
   gan_list_goto_head(dls1);
   p = gan_list_get_next(dls1, int);
   cu_assert(*p == 0);
   p = gan_list_get_next(dls1, int);
   cu_assert(*p == 1);
   gan_list_goto_pos(dls1, 6);
   p = gan_list_get_current(dls1, int);
   cu_assert(*p == 7);
   gan_list_goto_tail(dls1);
   p = gan_list_get_current(dls1, int);
   cu_assert(*p == 9999);

   return GAN_TRUE;
}

#ifdef LIST_TEST_MAIN

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
cUnit_test_suite *list_test_build_suite(void)
{
   cUnit_test_suite *sp;

   /* Get a new test session */
   sp = cUnit_new_test_suite("list_test suite");

   cUnit_add_test(sp, "list_test", run_test);

   /* register a setup and teardown on the test suite 'list_test' */
   if (cUnit_register_setup(sp, setup_test) != GAN_TRUE)
      printf("Error while setting up test suite list_test");

   if (cUnit_register_teardown(sp, teardown_test) != GAN_TRUE)
      printf("Error while tearing down test suite list_test");

   return( sp );
}

#endif /* #ifdef LIST_TEST_MAIN */

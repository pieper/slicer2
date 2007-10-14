/**
 * File:          $RCSfile: misc_defs.c,v $
 * Module:        Miscellaneous definitions
 * Part of:       Gandalf Library
 *
 * Revision:      $Revision: 1.1.2.1 $
 * Last edited:   $Date: 2007/10/14 02:11:45 $
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

#define GAN_DONT_DEFINE_MALLOC

#include <gandalf/common/misc_defs.h>
#include <gandalf/common/bit_array.h>
#include <gandalf/common/misc_error.h>
#include <string.h>

/**
 * \mainpage Gandalf: The Fast Computer Vision and Numerical Library
 */

/**
 * \addtogroup Common
 * \{
 */

/**
 * \defgroup CommonMisc Miscellaneous Definitions and Functions
 * \{
 */

#ifndef NDEBUG
/**
 *\brief debug flag; can be set interactively in debugger to switch on various messages
 */
GANDALF_API Gan_Bool gan_debug = GAN_FALSE;
#endif

/**
 * \brief Array of sizes of simple types used in Gandalf.
 */
const size_t gan_type_sizes[] =
{
   sizeof(char),           /**< corresponding to label #GAN_CHAR */
   sizeof(unsigned char),  /**< corresponding to label #GAN_UCHAR */
   sizeof(short),          /**< corresponding to label #GAN_SHORT */
   sizeof(unsigned short), /**< corresponding to label #GAN_USHORT */
   sizeof(int),            /**< corresponding to label #GAN_INT */
   sizeof(unsigned int),   /**< corresponding to label #GAN_UINT */
   sizeof(long),           /**< corresponding to label #GAN_LONG */
   sizeof(unsigned long),  /**< corresponding to label #GAN_ULONG */
#if (SIZEOF_LONG_LONG != 0)
   sizeof(long long),      /**< corresponding to label #GAN_LONGLONG */
#endif
   sizeof(float),          /**< corresponding to label #GAN_FLOAT */
   sizeof(double),         /**< corresponding to label #GAN_DOUBLE */
   sizeof(long double),    /**< corresponding to label #GAN_LONGDOUBLE */
   sizeof(char *),         /**< corresponding to label #GAN_STRING */
   sizeof(Gan_Bool),       /**< corresponding to label #GAN_BOOL */
   sizeof(void *)          /**< corresponding to label #GAN_POINTER */
};


/**
 * \brief Generates a string identifier for an internal type.
 * \param format Image format, e.g. #GAN_UINT8
 * \return String identifier, e.g. "8-bit", or "" for unknown format.
 */
const char *gan_type_string ( Gan_Type type )
{
   switch(type)
   {
      case GAN_INT8:   return "signed 8-bit";  break;
      case GAN_UINT8:  return "8-bit";         break;
      case GAN_INT16:  return "signed 16-bit"; break;
      case GAN_UINT16: return "16-bit";        break;
      case GAN_INT32:  return "signed 32-bit"; break;
      case GAN_UINT32: return "32-bit";        break;
#ifdef GAN_UINT64
      case GAN_INT64:  return "signed 64-bit"; break;
      case GAN_UINT64: return "64-bit";        break;
#endif
      case GAN_FLOAT32: return "32-bit float"; break;
      case GAN_FLOAT64: return "64-bit float"; break;
      case GAN_BOOL:    return "Boolean";      break;
      case GAN_POINTER: return "Pointer";      break;
      default:
        break;
   }

   /* unknown type */
   gan_err_flush_trace();
   gan_err_register ( "gan_type_string", GAN_ERROR_ILLEGAL_TYPE, "" );
   return "";
}

#ifdef GAN_MALLOC_DEBUG

#include <gandalf/common/linked_list.h>

typedef enum { GAN_MALLOC, GAN_CALLOC, GAN_DOUBLELOC } Gan_MallocType;
typedef struct Gan_MallocStackStruct
{
   int line;
   char *file;
   struct Gan_MallocStackStruct *next;
} Gan_MallocStackStruct;

typedef struct Gan_MallocStruct
{
   unsigned line;
   char *file;
   Gan_MallocType type;
   void *ptr;
   size_t size, bsize;
   Gan_MallocStackStruct *stack;

   struct Gan_MallocStruct *next;
} Gan_MallocStruct;

static Gan_MallocStackStruct *stack = NULL;
static Gan_MallocStruct *list = NULL;

static void
 stack_element_free ( Gan_MallocStackStruct *stack )
{
   free ( stack->file );
   free ( stack );
}

static void
 stack_free ( Gan_MallocStackStruct *stack )
{
   if ( stack == NULL ) return;
   stack_free ( stack->next );
   stack_element_free ( stack );
}

static Gan_MallocStackStruct *
 copy_reversed_stack ( Gan_MallocStackStruct *stack )
{
   Gan_MallocStackStruct *stack_copy = NULL;

   for ( ; stack != NULL; stack = stack->next )
   {
      Gan_MallocStackStruct *new = (Gan_MallocStackStruct *)
                                   malloc(sizeof(Gan_MallocStackStruct));

      assert ( new != NULL );
      new->line = stack->line;
      new->file = (char *) malloc((strlen(stack->file)+1)*sizeof(char));
      assert ( new->file != NULL );
      strcpy ( new->file, stack->file );
      new->next = stack_copy;
      stack_copy = new;
   }      
      
   return stack_copy;
}

static void
 add_to_malloc_list ( unsigned line, const char *file, Gan_MallocType type,
                      void *ptr, size_t size, size_t bsize )
{
   Gan_MallocStruct *new = (Gan_MallocStruct *)
                           malloc(sizeof(Gan_MallocStruct));

   assert ( new != NULL );

   /* fill fields of new structure */
   new->line = line;
   new->file = (char *) malloc((strlen(file)+1)*sizeof(char));
   assert ( new->file != NULL );
   strcpy ( new->file, file );
   new->type = type;
   new->ptr = ptr;
   new->size = size*bsize;
   new->bsize = bsize;
   new->stack = copy_reversed_stack(stack);

   /* insert new block in list */
   new->next = list;
   list = new;
}

static void
 remove_from_malloc_list ( void *ptr )
{
   Gan_MallocStruct *lptr, *prev;

   /* search through list for given pointer */
   for ( prev = NULL, lptr = list; lptr != NULL;
         prev = lptr, lptr = lptr->next )
      if ( lptr->ptr == ptr )
      {
         if ( prev == NULL ) list = lptr->next;
         else                prev->next = lptr->next;

         stack_free(lptr->stack);
         free(lptr->file);
         free(lptr);
         break;
      }

   assert ( lptr != NULL );
}

/**
 * \brief Gandalf debugging version of \c calloc().
 * \param file The file in which \c calloc() was called
 * \param line The line number that \c calloc() was called from
 * \param nmemb The number of blocks of data to allocate
 * \param size The size of each block
 * \return Pointer allocated from the heap, or \c NULL on failure.
 *
 * Puts details of the \c calloc() call into a linked list for subsequent
 * analysis of the heap.
 *
 * This function is for debug purposes only and is not thread-safe.
 */
void *
 gan_calloc  ( const char *file, unsigned line, size_t nmemb, size_t size )
{
   void *ptr = calloc(nmemb, size);

   add_to_malloc_list ( line, file, GAN_CALLOC, ptr, nmemb, size );
   return ptr;
}

/**
 * \brief Gandalf debugging version of \c malloc().
 * \param file The file in which \c malloc() was called
 * \param line The line number that \c malloc() was called from
 * \param size The size of the block of memory to be allocated
 * \return Pointer allocated from the heap, or \c NULL on failure.
 *
 * Puts details of the \c malloc() call into a linked list for subsequent
 * analysis of the heap.
 *
 * This function is for debug purposes only and is not thread-safe.
 */
void *
 gan_malloc  ( const char *file, unsigned line, size_t size )
{
   void *ptr = malloc(size);

   add_to_malloc_list ( line, file, GAN_MALLOC, ptr, size, 1 );
   return ptr;
}

/**
 * \brief Gandalf debugging version of \c realloc().
 * \param file The file in which realloc() was called
 * \param line The line number that realloc() was called from
 * \param ptr A previously allocated memory block or NULL
 * \param size The size of the block of memory to be allocated
 * \return Pointer allocated from the heap, or \c NULL on failure.
 *
 * Puts details of the \c malloc() call into a linked list for subsequent
 * analysis of the heap.
 *
 * This function is for debug purposes only and is not thread-safe.
 */
void *
 gan_realloc ( const char *file, unsigned line, void *ptr, size_t size )
{
   if ( ptr != NULL ) remove_from_malloc_list ( ptr );
   ptr = realloc(ptr,size);
   add_to_malloc_list ( line, file, GAN_DOUBLELOC, ptr, size, 1 );
   return ptr;
}

/**
 * \brief Gandalf debugging version of \c free().
 * \param ptr A previously allocated memory block
 * \return No value.
 *
 * Removes details of a previous \c malloc(), \c calloc() or \c realloc() call
 * from a linked list.
 *
 * This function is for debug purposes only and is not thread-safe.
 */
void gan_free ( void *ptr )
{
   remove_from_malloc_list ( ptr );
   free ( ptr );
}

/**
 * \brief Prints a report on the current heap status to \c stderr.
 * \param file A file to which to print the report or \c NULL
 * \return No value.
 *
 * Prints a report on the current heap status to the given \a file, or to
 * \c stderr if \a file is passed as \c NULL.
 */
Gan_Bool
 gan_heap_report(const char *file)
{
   Gan_MallocStruct *lptr;
   FILE *fp;

   if ( file == NULL )
      fp = stderr;
   else
   {
      fp = fopen ( file, "w" );
      if ( fp == NULL )
      {
         gan_err_flush_trace();
         gan_err_register ( "gan_heap_report", GAN_ERROR_OPENING_FILE, "" );
         return GAN_FALSE;
      }
   }
   
   for ( lptr = list; lptr != NULL; lptr = lptr->next )
   {
      Gan_MallocStackStruct *ptr;

      switch ( lptr->type )
      {
         case GAN_MALLOC:
           fprintf ( fp,
                     "malloc of %d bytes (%p) at line %d of file %s\n",
                     lptr->size, lptr->ptr, lptr->line, lptr->file );
           break;

         case GAN_CALLOC:
           fprintf ( fp, "calloc of %d bytes (%p) at line %d of file %s (block size %d)\n",
                     lptr->size, lptr->ptr, lptr->line, lptr->file, lptr->bsize );
           break;

         case GAN_DOUBLELOC:
           fprintf ( fp,
                     "realloc of %d bytes (%p) at line %d of file %s\n",
                     lptr->size, lptr->ptr, lptr->line, lptr->file );
           break;

         default:
           assert(0);
           break;
      }

      for ( ptr = lptr->stack; ptr != NULL; ptr = ptr->next )
         fprintf ( fp, "  called from line %d of file %s\n",
                   ptr->line, ptr->file );
   }

   /* success */
   if ( file != NULL ) fclose(fp);
   return GAN_TRUE;
}

void
 gan_heap_push_fileline ( const char *file, int line )
{
   Gan_MallocStackStruct *new = (Gan_MallocStackStruct *)
                                malloc(sizeof(Gan_MallocStackStruct));

   assert ( new != NULL );
   new->file = (char *) malloc(strlen(file)+1);
   new->line = line;
   assert ( new->file != NULL );
   strcpy ( new->file, file );
   new->next = stack;
   stack = new;
}

void
 gan_heap_pop(void)
{
   Gan_MallocStackStruct *next;

   assert ( stack != NULL );
   next = stack->next;
   stack_element_free ( stack );
   stack = next;
}

#endif /* #ifdef GAN_MALLOC_DEBUG */

/* define memcpy if it is not part of the C library */
#ifndef HAVE_MEMCPY
void *
 memcpy ( void *dest, const void *src, size_t n )
{
   char *chdest = (char *)dest, *chsrc = (char *)src;
   int intn;
   
   for ( intn = (int)n-1; intn >= 0; intn-- )
      *chdest++ = *chsrc++;

   return dest;
}
#endif /* #ifndef HAVE_MEMCPY */

/**
 * \}
 */

/**
 * \}
 */

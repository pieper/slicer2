/**
 * File:                $RCSfile: gan_err_trace.c,v $
 * Module:        Error trace
 * Part of:        Gandalf Exception Handling
 *
 * Revision:        $Revision: 1.1.2.1 $
 * Last edited:        $Date: 2007/10/14 02:11:45 $
 * Author:        $Author: ruetz $
 * Copyright:        (c) 2000 Industrial Research Limited
 *
 * Notes:
 * 
 * 
 *
 * Errors are stored for later retrieval by storing their details onto
 * a stack, implemented as a linked list. A record of the linked list
 * is the struct ET_TRACE defined in gan_exception_trace.h. Stack
 * contents are arranged in order of age, starting with the most recent
 * error at the top. The stack is referenced by the module level
 * variable et_trace.
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

#include <stdio.h>                        /* malloc(), free() */
#include <stdlib.h>                       /* malloc(), free() */
#include <string.h>                       /* strlen(), strcpy() */
#include <gandalf/common/allocate.h>
#include <gandalf/common/gan_err_trace.h>           /* Gan_ErrorTrace */

/**
 * \addtogroup Common
 * \{
 */

/**
 * \addtogroup CommonError
 * \{
 */

/*
 * \brief Register a deep error.
 * \param et_top       Pointer to top record of error trace
 * \param func_name    Name of function in which error occurs
 * \param file_name    Name of file in which error occurs
 * \param line_number  Line in file at which error occurs
 * \param number       Number attached to error
 * \return No value.
 *
 * Register a deep error. A deep error indicates that an error occurred within
 * this module. A deep error will be written into the record at the top of the
 * stack. The deep error message is static to this module.
 *
 * The error code registered will be #GAN_EC_DFT_DEEP_ERROR.
 *
 * \warning \c f_static_message will be set to #GAN_ET_YES.
 */
static void
 et_deep_error ( Gan_ErrorTrace *et_top,
                 const char *func_name,
                 const char *file_name,
                 int         line_number,
                 int         number )
{
   et_top->f_spare      = GAN_ET_NO;        /* This error record is now used */
   et_top->func_name    = func_name;
   et_top->err_code     = GAN_EC_DFT_DEEP_ERROR;
   et_top->file_name    = file_name;
   et_top->line_number  = line_number;
   et_top->number       = number;

   et_top->f_static_message = GAN_ET_YES;
   et_top->message      = "Deep Error: Memory allocation error occured in exception module.";
 
   return;
} /* et_deep_error() */

/*
 * \brief Allocate memory for a new error record for error trace.
 * \return Pointer to new record. \c NULL is error.
 *
 * Allocate memory for a new error record for error trace.
 * \warning Field f_static_message is specified at point message is provided.
 */
static Gan_ErrorTrace *et_new_record(void)
{
   Gan_ErrorTrace *new_record;

   if ( (new_record = gan_malloc_object(Gan_ErrorTrace)) != NULL )
   {
      new_record->f_static_record  = GAN_ET_NO;
      new_record->f_spare          = GAN_ET_YES;
      new_record->func_name        = NULL;
      new_record->err_code         = GAN_EC_DFT_SPARE;
      new_record->file_name        = NULL;
      new_record->line_number      = 0;
      new_record->f_static_message = GAN_ET_YES;
      new_record->message          = NULL;
      new_record->number           = 0;
   }

   return new_record;                        /* NULL if malloc fails */
} /* et_new_record() */

/*
 * \brief Write error information into second record of trace.
 * \param et_top       Pointer to top record of error trace
 * \param func_name    Name of function in which error occurs
 * \param err_code     Numeric code of error
 * \param file_name    Name of file in which error occurs
 * \param line_number  Line in file at which error occurs
 * \param message      Message string describing error
 * \param number       Number attached to error
 * \return No value.
 *
 * Write error information in the second to top error record of stack.
 * (Top of stack reserved for deep error).
 *
 * \warning If error occurs inside this function (only possibility is
 *          malloc() error) then do the best to write error information into
 *          second to top error record, and invoke deep error sequence.
 *          If error information is successfully written, malloc a new top of
 *          stack record.
 *
 * The second to top error record is assumed to exist. Check using
 * et_get_number_spare().
 * \sa et_get_number_spare()
 */
static Gan_ErrorTrace *
 et_register_error ( Gan_ErrorTrace *et_top, 
                     const char *func_name,
                     int         err_code,
                     const char *file_name,
                     int         line_number,
                     const char *message,
                     int         number )
{
   int n_chars;                        /* Size of string */

   et_top->prev->f_spare = GAN_ET_NO;        /* No longer spare */
   et_top->prev->func_name   = func_name;
   et_top->prev->file_name   = file_name;
   et_top->prev->line_number = line_number;
   et_top->prev->err_code    = err_code; 
   et_top->prev->number      = number;
    
   /*  Malloc space for message string, +1 for '\0' termination char */
   n_chars = strlen( message ) + 1;
   et_top->prev->message = gan_malloc_array ( char, n_chars );
    
   if ( et_top->prev->message == NULL )
   {                                        /* malloc error is a deep error */
      printf("FAILED\n");
      et_deep_error(et_top, "et_register_error", __FILE__, __LINE__, number);
   }
   else
   {
      /* Success, mark not spare, alloc new record */
      Gan_ErrorTrace * a_record = NULL;

      /*  Copy contents of the message string */
      strcpy(et_top->prev->message, message);
      et_top->prev->f_static_message = GAN_ET_NO; /* Not a static message */
        
      /**  Allocate new record and put on top of trace **/
      if ((a_record = et_new_record()) != NULL )
      {
         /* Successful */
         a_record->prev = et_top;  /* Add new record to top of stack */
         et_top = a_record;
      }
      else
      {
         /* Unable to allocate new record */
         et_deep_error(et_top, "et_register_error", __FILE__, __LINE__, number);
         return et_top;
      }
   }
    
   return et_top;
} /* et_register_error() */

/*
* Function:     Get the number of successive 'spare' records from and including
*               specified starting point.
* Arguments:    et_record  Pointer to a record of error trace
* Returns:      Count of spare records
* Description:  Get the number of successive 'spare' records from and including
*               specified starting point.
*               Typically et_record is set to top of stack.
* Warnings:     Non existent records are not counted as 'spare'.
*               If a record is observed to be 'not spare', then all later
*               records in the stack are assumed 'not spare'.
* See Also:        
*
*/
static int
 et_get_number_spare ( Gan_ErrorTrace *et_record )
{
   int n = 0;
    
   while ( et_record != NULL )
   {
      if ( et_record->f_spare == GAN_ET_YES )
         n++;                        /* "I see that spare" */
      else
         break;                        /* 'Not spare' found: stop counting */

      et_record = et_record->prev;
   }

   return n;
} /* et_get_number_spare() */

/**
 * \brief Pushes a new error recorded onto error trace.
 * \param et_top       Pointer to top record of error trace
 * \param func_name    Name of function in which error occurs
 * \param err_code     Numeric code of error
 * \param file_name    Name of file in which error occurs
 * \param line_number  Line in file at which error occurs
 * \param message      Message string describing error
 * \param number       Number attached to error
 * \return Pointer to new top of list.
 * \note
 *
 * A memory error may occur inside gan_et_push() because this function
 * calls malloc(): this is called a deep error. Two records are
 * reserved at the top of the trace to register the occurrence of a deep
 * error (top) and to register the error (second to top) that
 * identified by the error code GAN_EC_DFT_DEEP_ERROR.
 *
 * A deep error is treated no differently from any other error, except
 * that the reserved records are used. Therefore, even if the trace
 * registers a deep error, gan_et_push() will attempt to register
 * subsequent errors if memory allows. Each time a deep error occurs,
 * gan_et_push() will attempt to register a deep error. Deep error that
 * follow a normal error will always succeed in being registered. A
 * deep error that follows a deep error may not succeed in being
 * registered because of memory constraints -- however, the top of the
 * trace will nevertheless show that the most recent error was a deep
 * error. Errors requested to be registered while no system memory
 * exists will be lost permanently.
 *
 * The oldest two records of the LIFO stack are statically
 * allocated. This ensures that at any time a deep error can be
 * registered.
 *
 * gan_et_push() or any other function in the exception handling module
 * makes no attempt to free memory for the purpose of avoid a deep
 * error. Memory management firmly rest with an external component
 * (i.e. library or application). Such behaviour is consistent with the
 * policy that the application (or library) is the master and the
 * exception handling module is the slave. The role of the exception
 * module is to provide full error information to the application, so
 * that the application has the opportunity to handle the error in a
 * contextually meaningful way.
 *
 *
 * The psuedo code for handling deep errors is as follows:
 *
 * -----------------------------------------------------------------------
 *
 *     gan_et_push()
 *        Have 2 records spare?
 *            yes: register_error
 *             no: try to allocate records so that we have 2 spare
 *                     on error: deep_error
 *                 register_error
 *
 * -----------------------------------------------------------------------
 *
 *        register_error() (into second spare)
 *             do the best to furnish info into 2nd spare record
 *                 on error:
 *                     leave the error message part blank
 *                     deep_error()
 *            mark 2nd record as 'not spare'
 *            allocate a new record and put at top of stack
 *                 on error: deep_error()
 *
 * -----------------------------------------------------------------------
 *
 *         deep_error()
 *            Have 1 spare record (in which to write deep error)
 *                yes: write deep error into spare record
 *                no : do nothing, return
 *
 */
Gan_ErrorTrace *
 gan_et_push ( Gan_ErrorTrace *et_top,
               const char *func_name,
               int         err_code,
               const char *file_name,
               int         line_number,
               const char *message,
               int         number )
{
   Gan_ErrorTrace * a_record = NULL;
   int n_spare = 0;            /* Number of spare records at top of trace */
    
   /****
    * Need (at least) 2 records spare at top of stack to register an
    * error. If we can't get 2 spare records, register deep error.
    */
   for ( n_spare = et_get_number_spare(et_top); n_spare < 2; )
   {
      /**  Allocate new record **/
      if ( (a_record = et_new_record()) != NULL )
      {                                /*  Successful */
         a_record->prev = et_top;        /* Add to top of stack */
         et_top = a_record;
         n_spare++;
      }
      else
      {                                /* Unable to allocate new record */
         et_deep_error(et_top, func_name, file_name, line_number, number);
         return et_top;
      }
   } /* Creating 2 spare records */

   /*  yes: register_error. Must have at least 2 spare on top of trace
    *        before et_register_error() can be called. */
   a_record = et_register_error ( et_top, func_name, err_code, file_name, line_number, message, number );

   return a_record;
} /* gan_et_push() */

/**
 * \brief Counts number of records in trace from specified starting point.
 * \param et_record  Pointer to top record of error trace
 * \return The number of records in trace from and including \a et_record to
 *         bottom of trace.
 *
 * Counts number of \em non-spare records in trace from and including
 * \a et_record to the last (oldest) record in stack. Total number of
 * non-spare records in error trace is returned when the top of error trace
 * is specified by \a et_record.
 */
int
 gan_et_get_record_count ( Gan_ErrorTrace * et_record )
{
   int n = 0;

   et_record = gan_et_get_record_first( et_record );
    
   while( et_record != NULL )
   {
      n++;
      et_record = gan_et_get_record_next( et_record );
   }

   return n;
} /* gan_et_get_record_count() */

/**
 * \brief Returns pointer to next non-spare error record after et_record.
 * \param et_record  Pointer to the Error record preceding the one
 *                   that will be returned
 * \return Returns pointer to next non-spare error record after \a et_record.
 *
 * \warning If \a et_record is \c NULL, this function returns \c NULL.
 */
Gan_ErrorTrace *
 gan_et_get_record_next ( Gan_ErrorTrace *et_record )
{
   if ( et_record != NULL )
      return gan_et_get_record_first(et_record->prev);
   else
      return NULL;
} /* gan_et_get_record_next() */

/**
 * \brief Returns pointer to first non-spare error record from \a et_record.
 * \param et_record Pointer to error record in error trace for which error
 *        code is to be read.
 * \return Error code of specified error record, or \c NULL if \a et_record is
 *         \c NULL, or no non-spare records found.
 *
 * Returns pointer to first non-spare error record from and including
 * \a et_record.
 */
Gan_ErrorTrace *
 gan_et_get_record_first ( Gan_ErrorTrace *et_record )
{
   while ( et_record != NULL )
   {
      if ( et_record->f_spare == GAN_ET_YES )
         et_record = et_record->prev; /* Skip if spare */
      else
         break;
   }

   return et_record;
} /* gan_et_get_record_first() */

/**
 * \brief Flush all errors in error trace.
 * \param a_record The top record in error trace
 * \return Pointer to new top of error trace.
 *
 * Flush all errors in error trace.
 * \warning \a et_record must be top record in trace, or else residual
 *          trace will malformed (i.e. not \c NULL terminated).
 *
 * \note Traverse error trace from start to finish deleting all records,
 *       except leaving the final two which are reserved to store deep error
 *       information.
 */
Gan_ErrorTrace *
 gan_et_flush_trace ( Gan_ErrorTrace *a_record )
{
   Gan_ErrorTrace *free_me = NULL;
   Gan_ErrorTrace *et_top = NULL;                /* New top of trace */
    
   while ( a_record != NULL )
   {
      free_me = a_record;
      a_record = a_record->prev;

      /****
       * Free text of error message, so long it is not a deep error, which
       * uses a static error message.
       */
      if ( (free_me->f_static_message != GAN_ET_YES) &&
           (free_me->message != NULL) )
      {
         free( free_me->message );
         free_me->message = NULL;
      }

      /****
       * Free the record, so long it is not a statically allocated record.
       */
      if (free_me->f_static_record != GAN_ET_YES)
      {
         free(free_me);
         free_me = NULL;
      }
      else
      {
         /** Don't free free_me, but mark it spare, and continue */
         free_me->f_spare = GAN_ET_YES;

         if (et_top == NULL)             /* et_top is topmost static record */
            et_top = free_me;
      }
   }
    
   return et_top;
} /* gan_et_flush_trace() */

/**
 * \}
 */

/**
 * \}
 */

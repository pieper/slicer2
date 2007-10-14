/**
 *
 * File:                $RCSfile: gan_err.c,v $
 * Module:        Exception module
 * Part of:        Gandalf Library
 *
 * Revision:        $Revision: 1.1.2.1 $
 * Last edited:        $Date: 2007/10/14 02:11:45 $
 * Author:        $Author: ruetz $
 * Copyright:        (c) 2000 Industrial Research Limited
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

#include <stdio.h>
#include <gandalf/common/gan_err_trace.h>
#include <gandalf/common/gan_err.h>
#ifdef _MSC_VER
#include <Windows.h>
#else
#include <pthread.h>
#endif /* #ifdef _MSC_VER */

/**
 * \addtogroup Common
 * \{
 */

/**
 * \defgroup CommonError Error Handling
 * \{
 */

/*
 * \brief Default error reporter.
 * \return No value.
 *
 * Typically the application should provide its own error reporter.
 */
static void
 gan_err_default_reporter(void)
{
   int i, n;                                /* Loop counter */
    
   const char * func_name;
   int          err_code;
   const char * file_name;
   int          line_number;
   const char * message;
   int          number;

   n = gan_err_get_error_count();
   for ( i = 1; i<=n; i++ )
   {
      if ((gan_err_get_error(i, &func_name, &err_code, &file_name, &line_number, &message, &number ) != GAN_EC_OK))
         fprintf(stderr, "**** gan_err_get_error unsuccessful\n");
        
      fprintf(stderr, "\nError number %d\n", i );
      fprintf(stderr, "func_name   = %s\n", func_name );
      fprintf(stderr, "err_code    = %d\n", err_code );
      fprintf(stderr, "file_name   = %s\n", file_name );
      fprintf(stderr, "line_number = %d\n", line_number );
      fprintf(stderr, "message     = %s\n", message );
   }

   fprintf(stderr, "gan_err_default_reporter()\n");
}

/*  Pointer to current error reporting function. This is setup using
 * gan_err_set_reporter(), and typically invoked using gan_err_report().
 *
 * Default reporter is gan_err_default_reporter().
 */
static Gan_ErrorReporterFunc gan_err_current_reporter = gan_err_default_reporter;


/* Flag indicating Trace Mode - see GAN_ERR_TRACE_X in gan_exception.h for
 * details. Default action is to set trace off. Permitted values are:
 * GAN_ERR_TRACE_OFF or GAN_ERR_TRACE_ON.
 */
static Gan_TraceMode gan_err_trace_mode = GAN_ERR_TRACE_OFF; 


/* The error trace */
/*  Statically allocate last and 2nd to last records for error trace */
static Gan_ErrorTrace record_last = { NULL, GAN_ET_YES, GAN_ET_NO,
                                      GAN_ET_YES, NULL,
                                      GAN_EC_DFT_SPARE, NULL, 0, NULL };
static Gan_ErrorTrace record_2nd_last = { &record_last,  GAN_ET_YES, GAN_ET_NO,
                                          GAN_ET_YES, NULL,
                                          GAN_EC_DFT_SPARE, NULL, 0, NULL };

/* Address of error trace (i.e. top of LIFO stack) */
static Gan_ErrorTrace * gan_err_trace_top = &record_2nd_last;

/**
 * \brief Installs an error reporter
 * \param app_error_reporter Pointer to an application defined function
 * \return Pointer to previously installed error reporter, if successful.
 *          \c NULL otherwise.
 *
 * This exception module allows an application defined function to be called
 * when an error is reported using gan_err_report().
 * gan_err_set_reporter() installs this error reporter.
 * \a app_error_reporter should be a pointer to an application defined function
 * to access error details, or the macros:
 *    - GAN_ERR_DFL To set default error reporter
 *    - GAN_ERR_IGN To ignore error reporting
 *
 * If GAN_ERR_DFL is provided, then the default error reporter (see below) is
 * installed.
 * \warning The default error reporter simply writes a message to \c stderr.
 *          The exception module is initialised to use the default error
 *          reporter.
 * \sa gan_err_get_reporter().
 * \note Other functions that invoke the current error handler must check first
 *       if it is set to #GAN_ERR_IGN. If so, ignore the invocation.
 *       gan_err_current_reporter() is module scope variable.
 */
Gan_ErrorReporterFunc gan_err_set_reporter(
                                    Gan_ErrorReporterFunc app_error_reporter )
{
   /*  Buffer old handler, so that it can be returned */
   Gan_ErrorReporterFunc gan_err_temp_reporter = gan_err_current_reporter; 
    
   /* Set reporter, noting default (GAN_ERR_DFL) or ignore
      (GAN_ERR_IGN) modes */
   if ( app_error_reporter == GAN_ERR_DFL )
      gan_err_current_reporter = gan_err_default_reporter;
   else if ( app_error_reporter == GAN_ERR_IGN )
      gan_err_current_reporter = GAN_ERR_IGN;        
   else
      gan_err_current_reporter = app_error_reporter; /* Set reporter */

   return gan_err_temp_reporter;
}

/**
 * \brief Returns current error reporter.
 * \return Pointer to current error reporter, or #GAN_ERR_DFL or #GAN_ERR_IGN.
 *
 * Returns current error reporter.
 *
 * \sa gan_err_set_reporter().
 */
Gan_ErrorReporterFunc
 gan_err_get_reporter(void)
{
   return gan_err_current_reporter;
}


#ifdef _MSC_VER
static CRITICAL_SECTION lpCriticalSection;
#else
static pthread_mutex_t err_sem;
#endif /* #ifdef _MSC_VER */
static Gan_Bool bInitialisedMutex = GAN_FALSE;

static void gan_err_sem_wait(void)
{
#ifdef _MSC_VER
   if(!bInitialisedMutex)
   {
       InitializeCriticalSection(&lpCriticalSection);
       bInitialisedMutex = GAN_TRUE;
   }

   EnterCriticalSection(&lpCriticalSection);
#else
   if(!bInitialisedMutex)
   {
      if (-1 == pthread_mutex_init(&err_sem, NULL))
         perror("Error intialising semaphore");
       
       bInitialisedMutex = GAN_TRUE;
   }

   for(;;)
      if(pthread_mutex_lock(&err_sem) >= 0)
         break;

#endif /* #ifdef _MSC_VER */
}

static void gan_err_sem_post(void)
{
#ifdef _MSC_VER
   LeaveCriticalSection(&lpCriticalSection);
#else
   if (-1 == pthread_mutex_unlock(&err_sem))
        perror("Error unlocking semaphore");
#endif /* #ifdef _MSC_VER */
}

/**
 * \brief Registers occurence of an error.
 * \param func_name    Name of function in which error occurs
 * \param err_code     Numeric code of error
 * \param file_name    Name of file in which error occurs
 * \param line_number  Line in file at which error occurs
 * \param message      Message string describing error
 * \parem number       Number to attach to error, default 0
 *
 * \return The error number of error registered. #GAN_EC_DFT_DEEP_ERROR if a
 *         deep error occurs, \a err_code otherwise.
 *                
 * Registers occurence of an error. Intended to be called at the lowest
 * function level immediately after the occurence of an error, and called at
 * every level of the function call stack during unwinding, until the error is
 * handled, or it unwinds into a function level where a different error
 * handling mechanism is used.
 *
 * If the trace mode is #GAN_ERR_TRACE_OFF, this function causes the current
 * error reporter to called immediately. If the trace mode is
 * #GAN_ERR_TRACE_ON, this function causes the details of the error to be
 * placed onto an error trace. The error details are reported in "batch" at a
 * later time upon invokation of gan_err_report().
 *
 * \warning #GAN_EC_DFT_DEEP_ERROR is registered in the top record of the error
 *          trace if a deep error occurs. The error requested to be registered
 *          is placed in the second top record of the error trace. This error
 *          may be missing the message string, because it may have been the
 *          process of allocating memory for this string that caused the deep
 *          error to occur.
 *
 * This function is typically called using the macro gan_err_register().
 *
 * \sa gan_err_register() (macro), gan_err_set_trace().
 * \note If trace mode is off then call error reporter immediately, otherwise
 *       push error details onto error trace.
 */
int
 gan_err_register_fileline ( const char *func_name,
                             int         err_code,
                             const char *file_name,
                             int         line_number,
                             const char *message,
                             int         number )
{
   Gan_ErrorTrace *atrace = NULL;
   int             the_err_code = GAN_EC_FAIL; /* Registered error code */

   /* use thread-safe access to static global gan_err_trace_top */
   gan_err_sem_wait();

   /* When trace mode is off, still use the trace, but flush it before and
    * after use.
    */
   if ( gan_err_trace_mode == GAN_ERR_TRACE_OFF )
      gan_err_trace_top = gan_et_flush_trace(gan_err_trace_top);

   gan_err_trace_top = gan_et_push ( gan_err_trace_top, func_name, err_code, file_name, line_number, message, number );

   /*  Note the registered error code. Could be a deep error */
   if ( (atrace = gan_et_get_record_first(gan_err_trace_top)) != NULL )
      the_err_code = atrace->err_code;

   /*  Report immediately. Use trace as interim storage. Flush afterwards */
   if ( gan_err_trace_mode == GAN_ERR_TRACE_OFF )
   {
      gan_err_sem_post();
      gan_err_report();

      gan_err_sem_wait();
      gan_err_trace_top = gan_et_flush_trace(gan_err_trace_top);
      /*  This flush is belts and braces. The primary flush is at the head
       * of this function.
       */
   }
   
   gan_err_sem_post();

   return the_err_code;
}

/**
 * \brief Invokes current error reporter.
 * \return No value.
 *
 * Invokes current error reporter. That's all. It is the responsibility of the
 * error reporter to traverse the error trace, read the details of each error,
 * and to report those details in whatever application specific manner it
 * chooses.
 *
 * This function is automatically called from gan_err_register_fileline() when
 * trace mode is OFF.
 *               
 * \warning The error reporter will not be called if
 *          gan_err_set_reporter(#GAN_ERR_IGN) has been called.
 *          The default error reporter is called when
 *           gan_err_set_reporter(#GAN_ERR_DFL);
 * \sa gan_err_set_trace(), gan_err_set_reporter().
 */
void
 gan_err_report(void)
{
   if (gan_err_current_reporter == GAN_ERR_DFL )
      gan_err_default_reporter();
   else if ( gan_err_current_reporter != GAN_ERR_IGN )
      gan_err_current_reporter();
    
   return;
} /* gan_err_report() */

/**
 * \brief Enable or disable use of trace to store error details.
 * \param trace_mode Whether to switch trace mode on or off
 * \return No value.
 *
 * A trace is a data structure that stacks error details for subsequent
 * reporting (activated by gan_err_report()). If trace is disabled by
 * passing \a trace_mode as #GAN_ERR_TRACE_OFF, then errors are reported
 * immediately upon being registered i.e. when gan_err_register() is called.
 * Otherwise trace mode is switched on by passing #GAN_ERR_TRACE_ON.
 *
 * \warning Trace mode is initialised to #GAN_ERR_TRACE_OFF.
 *          Any non-zero trace_mode is assumed equivalent to
 *          #GAN_ERR_TRACE_OFF.
 *          When trace is turned off, the trace is flushed immediately.
 * \sa gan_err_report(), gan_err_register().
 */
void
 gan_err_set_trace( Gan_TraceMode trace_mode )
{
   gan_err_sem_wait();

   if ( trace_mode == GAN_ERR_TRACE_OFF )
      gan_err_trace_top = gan_et_flush_trace(gan_err_trace_top);
    
   gan_err_trace_mode = trace_mode;

   gan_err_sem_post();

   return;
}


/**
 * \brief Flush all errors in trace.
 * \return No value.
 *
 * Flush all errors in error trace
 * \note Traverse error trace from start to finish deleting all error records
 *       (except two reserved ones which are transparent to this module).
 */
void
 gan_err_flush_trace( void )
{
   gan_err_sem_wait();
   gan_err_trace_top = gan_et_flush_trace(gan_err_trace_top);

   gan_err_sem_post();
   return;
} /*  gan_err_flush_trace() */


/**
 * \brief Gets the number of errors in error trace.
 * \return Number of errors in error trace.
 *
 * Gets the number of errors in error trace.
 *
 * \sa gan_err_get_error().
 */
int
 gan_err_get_error_count( void )
{
   int iErrorCount;

   gan_err_sem_wait();

   iErrorCount = gan_et_get_record_count(gan_err_trace_top);

   gan_err_sem_post();

   return iErrorCount;
} /* gan_err_get_error_count() */

/**
 * \brief Gets details of n-th error stored in the error trace.
 * \param n            Index of requested error [1..N]
 * \param func_name    Name of function in which error occurs
 * \param err_code     Numeric code of error
 * \param file_name    Name of file in which error occurs
 * \param line_number  Line in file at which error occurs
 * \param message      Message string describing error
 * \param number       Number attached to error
 * \return Status of n-th error.
 *
 * If any of above pointers are \c NULL, then those details are not returned.
 *
 * Gets details of \a n-th error. \a n=1 refers to the most recent error
 * registered in error trace. Usually gan_err_get_error_count() is called to
 * obtain the number of error records in the trace. Return values:
 *     - #GAN_EC_DFT_BAD_N Index '\a n' out of bounds,
 *     - #GAN_EC_DFT_EMPTY Error trace is empty (regardless of requested n)
 *     - #GAN_EC_OK        Otherwise.
 *
 * \warning The returned strings (\a func_name, \a file_name, \a message) are
 *          not guaranteed to exist at a later time, nor should they be
 *          modified in place. Therefore the calling function must either
 *          use the returned strings immediately or make copies.
 *
 *          Because the stack is numbered from the top (1=most
 *          recent) an arbitrary index i may refer to different
 *          error records at different times. However, index 1
 *          always refers to the most recent error.
 *            
 * \sa gan_err_get_error_count().
 */
int
 gan_err_get_error ( int          n,
                     const char **func_name,
                     int         *err_code,
                     const char **file_name,
                     int         *line_number,
                     const char **message,
                     int         *number )
{
   int count;
   int i;                                /* Loop counter */
   Gan_ErrorTrace *a_record = NULL;

   gan_err_sem_wait();

   if ( (count = gan_et_get_record_count(gan_err_trace_top)) < 1 )
      return GAN_EC_DFT_EMPTY;                /* Error trace is empty */
    
   /*  Boundary check on n */
   if ( (n < 1) || (n > count) )
      return GAN_EC_DFT_BAD_N;                /* Out of bounds, bye */

   for ( a_record = gan_et_get_record_first(gan_err_trace_top), i = 1; i < n; i++ )
      a_record = gan_et_get_record_next(a_record); /* Step to N-th error */

   if ( func_name != NULL )
      *func_name = a_record->func_name;

   if ( err_code != NULL )
      *err_code = a_record->err_code;

   if ( file_name != NULL )
      *file_name = a_record->file_name;
    
   if ( line_number != NULL )
      *line_number = a_record->line_number;
    
   if ( message != NULL )
      *message = a_record->message;

   if ( number != NULL )
      *number = a_record->number;

   gan_err_sem_post();

   return GAN_EC_OK;
} /* gan_err_get_error() */

/**
 * \}
 */

/**
 * \}
 */

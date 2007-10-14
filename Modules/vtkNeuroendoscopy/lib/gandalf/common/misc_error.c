/**
 * File:          $RCSfile: misc_error.c,v $
 * Module:        Miscellaneous error declarations
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

#include <string.h>
#include <gandalf/common/misc_error.h>
#include <gandalf/common/allocate.h>

/**
 * \addtogroup Common
 * \{
 */

/**
 * \addtogroup CommonError
 * \{
 */

#if 0

typedef struct Gan_ErrorStruct
{
   char *funcname;
   int   code;
   char *message;
   char *filename;
   int   line;
   struct Gan_ErrorStruct *next;
} Gan_ErrorStruct;

static Gan_ErrorStruct *
 gan_error_stack_add ( Gan_ErrorStruct *error_stack,
                       const char *funcname, int code, char *message,
                       const char *filename, int line )
{
   Gan_ErrorStruct *new = gan_malloc_object(Gan_ErrorStruct);

   gan_assert ( new != NULL, "malloc() failed in gan_error_stack_add()" );
   new->funcname = gan_malloc_array ( char, strlen(funcname)+1 );
   strcpy ( new->funcname, funcname );
   new->code = code;
   new->message = gan_malloc_array ( char, strlen(message)+1 );
   strcpy ( new->message, message );
   new->filename = gan_malloc_array ( char, strlen(filename)+1 );
   strcpy ( new->filename, filename );
   new->line = line;
 
   new->next = error_stack;
   return new;
}

static void
 gan_error_stack_free ( Gan_ErrorStruct *error_stack )
{
   Gan_ErrorStruct *next;

   if ( error_stack == NULL ) return;
   next = error_stack->next;
   free ( error_stack );
   gan_error_stack_free ( next );
}

static Gan_ErrorStruct *error_stack = NULL;

void
 gan_err_flush_trace(void)
{
   gan_error_stack_free ( error_stack );
   error_stack = NULL;
}

/* not a user function */
void gan_err_register_expanded ( const char *funcname, int code, char *message,
                                 const char *filename, int line )
{
#if 1
   print_error ( funcname, code, message, filename, line );
   exit(-1);
#endif
   error_stack = gan_error_stack_add ( error_stack, funcname, code, message,
                                       filename, line );
}
#endif

static void print_error ( const char *funcname, int code, const char *message,
                          const char *filename, int line )
{
   fprintf ( stderr, "Gandalf error %d in function %s(): %s\n", code, funcname, gan_err_string(code) );
   fprintf ( stderr, "Line %d of file %s\n", line, filename );
}

/**
 * \brief Returns error message corresponding to given error code
 * \return Error string
 */
const char *gan_err_string(int error_code)
{
   switch ( error_code )
   {
      case GAN_ERROR_FAILURE:
        return "Failed";
        break;

      case GAN_ERROR_NOT_IMPLEMENTED:
        return "Not implemented";
        break;

      case GAN_ERROR_REF_OUTSIDE_MATRIX:
        return "Reference outside matrix";
        break;

      case GAN_ERROR_CANT_REALLOC:
        return "Can't reallocate";
        break;

      case GAN_ERROR_MALLOC_FAILED:
        return "Memory allocation failed";
        break;

      case GAN_ERROR_DIFFERENT_DIMS:
        return "Dimensions don't match";
        break;

      case GAN_ERROR_INCOMPATIBLE:
        return "Incompatible";
        break;

      case GAN_ERROR_MATRIX_NOT_SQUARE:
        return "Matrix not square";
        break;

     case GAN_ERROR_INPLACE_TRANSPOSE:
        return "Illegal in-place matrix transpose";
        break;

     case GAN_ERROR_INPLACE_MULTIPLY:
        return "Illegal in-place matrix multiply";
        break;

      case GAN_ERROR_ARRAY_TOO_SMALL:
        return "Array too small";
        break;

      case GAN_ERROR_ILLEGAL_TYPE:
        return "Illegal type";
        break;

      case GAN_ERROR_NO_IMPLICIT_INV:
        return "Can't do implicit inverse";
        break;

      case GAN_ERROR_CLAPACK_ILLEGAL_ARG:
        return "LAPACK function illegal argument";
        break;

      case GAN_ERROR_CBLAS_ILLEGAL_ARG:
        return "BLAS function illegal argument";
        break;

      case GAN_ERROR_CBLAS_FAILED:
        return "BLAS function failed";
        break;

      case GAN_ERROR_NOT_POSITIVE_DEFINITE:
        return "Matrix not positive definite";
        break;

      case GAN_ERROR_DIVISION_BY_ZERO:
        return "Attempted division by zero";
        break;

      case GAN_ERROR_SQRT_OF_NEG_NUMBER:
        return "Attempted square-root of negative number";
        break;

      case GAN_ERROR_NO_CONVERGENCE:
        return "No convergence";
        break;

      case GAN_ERROR_SINGULAR_MATRIX:
        return "Singular matrix";
        break;

      case GAN_ERROR_IMAGE_TOO_SMALL:
        return "Image too small";
        break;

      case GAN_ERROR_NO_DATA:
        return "No data";
        break;

      case GAN_ERROR_NOT_ENOUGH_DATA:
        return "Not enough data";
        break;

      case GAN_ERROR_OPENING_FILE:
        return "Couldn't open file";
        break;

      case GAN_ERROR_CORRUPTED_FILE:
        return "Corrupted file";
        break;

      case GAN_ERROR_OUTSIDE_RANGE:
        return "Outside range";
        break;

      case GAN_ERROR_TOO_LARGE:
        return "Too large";
        break;

      case GAN_ERROR_TOO_SMALL:
        return "Too small";
        break;

      case GAN_ERROR_TRUNCATED_FILE:
        return "Truncated file";
        break;

      case GAN_ERROR_NOT_INITIALISED:
        return "Not initialised";
        break;

      case GAN_ERROR_ILLEGAL_ARGUMENT:
        return "Illegal argument";
        break;

      case GAN_ERROR_CCMATH_FAILED:
        return "CCM library function failed";
        break;

      case GAN_ERROR_NO_SOLUTION:
        return "No solution found";
        break;

      case GAN_ERROR_READING_FROM_FILE:
        return "Reading from file";
        break;

      case GAN_ERROR_WRITING_TO_FILE:
        return "Writing to file";
        break;

      case GAN_ERROR_ILLEGAL_IMAGE_TYPE:
        return "Illegal image type";
        break;

      case GAN_ERROR_ILLEGAL_IMAGE_FORMAT:
        return "Illegal image format";
        break;

      case GAN_ERROR_ILLEGAL_IMAGE_FILE_FORMAT:
        return "Illegal image file format";
        break;

      case GAN_ERROR_ILLEGAL_IMAGE_CHANNEL:
        return "Illegal image channel";
        break;

      default:
        return "Unknown error";
        break;
   }

   /* shouldn't get here */
   return "";
}

const char *gan_err_string_at_lowest_level(void)
{
   int          iNoErrors;
   const char * pchFunctionName;
   int          iErrorCode;
   const char * pchFileName;
   int          iLineNumber;
   const char * pchMessage;
   int          iNumber;
   

   iNoErrors = gan_err_get_error_count();
   if(iNoErrors == 0)
      return "No error";

   if ((gan_err_get_error(iNoErrors, &pchFunctionName, &iErrorCode, &pchFileName, &iLineNumber, &pchMessage, &iNumber ) != GAN_EC_OK))
      return "Unknown error";
   
   return gan_err_string(iErrorCode);
}

/**
 * \brief Default Gandalf error handling function.
 * \return No value.
 *
 * Call gan_err_set_reporter(gan_err_default_reporter); in your Gandalf
 * program before calling any other Gandalf functions, if you want to use this
 * error handling routine, which just prints an error message and exits.
 */
void
 gan_err_default_reporter(void)
{
   int n, i;

   n = gan_err_get_error_count();
   for (i = 1; i<=n; i++)
   {
      const char *func_name, *message, *file_name;
      int code, line, number;

      if ((gan_err_get_error(i, &func_name, &code, &file_name, &line, &message, &number ) != GAN_EC_OK))
         fprintf(stderr, "**** gan_err_get_error unsuccessful\n");
        
      fprintf(stderr, "\nError number %d\n", i );
      print_error ( func_name, code, message, file_name, line );
   }

   abort();
}

/**
 * \}
 */

/**
 * \}
 */

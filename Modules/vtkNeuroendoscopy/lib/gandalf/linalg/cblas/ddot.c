/**************************************************************************
*
* File:          $RCSfile: ddot.c,v $
* Module:        BLAS function
* Part of:       Gandalf Library
*
* Revision:      $Revision: 1.1.2.1 $
* Last edited:   $Date: 2007/10/14 02:26:19 $
* Author:        $Author: ruetz $
* Copyright:     Modifications (c) 2000 Imagineer Software Limited
*
* Notes:         
* Private func:  
* History:       Modified from original CLAPACK source code 
*
**************************************************************************/

#include <math.h>
#include <gandalf/linalg/cblas/ddot.h>
#include <gandalf/common/misc_defs.h>
#include <gandalf/common/compare.h>
#include <gandalf/common/misc_error.h>

/* only define this function locally if there is no LAPACK installed */
#if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK)

double
 gan_ddot ( long n, double *dx, long incx, double *dy, long incy )
{
   double total = 0.0;
   long i;

   gan_err_test_bool ( n >= 0, "gan_ddot", GAN_ERROR_CBLAS_ILLEGAL_ARG, "" );

   if ( incx == 1 && incy == 1 )
      for ( i = n-1; i >= 0; i-- ) total += *dx++**dy++;
   else if ( incx == 1 )
      for ( i = n-1; i >= 0; dy += incy, i-- ) total += *dx++**dy;
   else if ( incy == 1 )
      for ( i = n-1; i >= 0; dx += incx, i-- ) total += *dx**dy++;
   else /* incx != 1 && incy != 1 */
      for ( i = n-1; i >= 0; dx += incx, dy += incy, i-- ) total += *dx**dy;

   /* success */
   return total;
}

#endif /* #if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK) */

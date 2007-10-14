/**************************************************************************
*
* File:          $RCSfile: snrm2.c,v $
* Module:        BLAS function
* Part of:       Gandalf Library
*
* Revision:      $Revision: 1.1.2.1 $
* Last edited:   $Date: 2007/10/14 02:26:22 $
* Author:        $Author: ruetz $
* Copyright:     Modifications (c) 2000 Imagineer Software Limited
*
* Notes:         
* Private func:  
* History:       Modified from original CLAPACK source code 
*
**************************************************************************/

#include <math.h>
#include <gandalf/linalg/cblas/snrm2.h>
#include <gandalf/common/misc_defs.h>
#include <gandalf/common/compare.h>
#include <gandalf/common/misc_error.h>

/* only define this function locally if there is no LAPACK installed */
#if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK)

float
 gan_snrm2 ( long n, float *x, long incx )
{
   long i;
   float total = 0.0;

   gan_assert ( n >= 0, "illegal argument in gan_snrm2()" );

   if ( n == 0 ) return 0.0;
   else if ( n == 1 ) return (float)fabs(*x);

   if ( incx == 1 ) for ( i = n-1; i >= 0; i--, x++ ) total += *x**x;
   else for ( i = n-1; i >= 0; i--, x += incx ) total += *x**x;

   /* success */
   return (float)sqrt(total);
}

#endif /* #if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK) */

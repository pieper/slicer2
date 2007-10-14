/**************************************************************************
*
* File:          $RCSfile: sswap.c,v $
* Module:        BLAS function
* Part of:       Gandalf Library
*
* Revision:      $Revision: 1.1.2.1 $
* Last edited:   $Date: 2007/10/14 02:26:23 $
* Author:        $Author: ruetz $
* Copyright:     Modifications (c) 2000 Imagineer Software Limited
*
* Notes:         
* Private func:  
* History:       Modified from original CLAPACK source code 
*
**************************************************************************/

#include <math.h>
#include <gandalf/linalg/cblas/sswap.h>
#include <gandalf/common/misc_defs.h>
#include <gandalf/common/compare.h>
#include <gandalf/common/misc_error.h>

/* only define this function locally if there is no LAPACK installed */
#if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK)

Gan_Bool
 gan_sswap ( long n, float *dx, long incx, float *dy, long incy )
{
   long i;
   float tmp;

   if ( incx == 1 && incy == 1 )
      for ( i = n-1; i >= 0; i-- )
      {
         tmp = *dx;
         *dx++ = *dy;
         *dy++ = tmp;
      }
   else if ( incx == 1 )
      for ( i = n-1; i >= 0; dy += incy, i-- )
      {
         tmp = *dx;
         *dx++ = *dy;
         *dy = tmp;
      }
   else if ( incy == 1 )
      {
         tmp = *dx;
         *dx = *dy;
         *dy++ = tmp;
      }
   else /* incx != 1 && incy != 1 */
      for ( i = n-1; i >= 0; dx += incx, dy += incy, i-- )
      {
         tmp = *dx;
         *dx = *dy;
         *dy = tmp;
      }

   /* success */
   return GAN_TRUE;
}

#endif /* #if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK) */

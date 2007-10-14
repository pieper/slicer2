/**************************************************************************
*
* File:          $RCSfile: stpmv.c,v $
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
#include <gandalf/linalg/cblas/stpmv.h>
#include <gandalf/common/misc_defs.h>
#include <gandalf/common/compare.h>
#include <gandalf/common/misc_error.h>

/* only define this function locally if there is no LAPACK installed */
#if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK)

Gan_Bool
 gan_stpmv ( Gan_LapackUpLoFlag upper, Gan_TposeFlag trans,
             Gan_LapackUnitFlag unit, long n, float *ap, float *x, long incx )
{
   float total;
   long i, j;

   gan_err_test_bool ( upper, "gan_stpmv", GAN_ERROR_NOT_IMPLEMENTED, "" );
   gan_err_test_bool ( !unit, "gan_stpmv", GAN_ERROR_NOT_IMPLEMENTED, "" );

   if ( n == 0 ) return GAN_FALSE;
   if ( trans )
      for ( i = n-1; i >= 0; i-- )
      {
         for ( j = i, total = 0.0; j >= 0; j-- )
            total += ap[i*(i+1)/2+j]*x[j*incx];

         x[i*incx] = total;
      }
   else /* !trans */
      for ( i = 0; i < n; i++ )
      {
         for ( j = n-1, total = 0.0; j >= i; j-- )
            total += ap[j*(j+1)/2+i]*x[j*incx];

         x[i*incx] = total;
      }

   /* success */
   return GAN_TRUE;
}

#endif /* #if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK) */

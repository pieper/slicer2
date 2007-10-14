/**************************************************************************
*
* File:          $RCSfile: dscal.c,v $
* Module:        BLAS function
* Part of:       Gandalf Library
*
* Revision:      $Revision: 1.1.2.1 $
* Last edited:   $Date: 2007/10/14 02:26:20 $
* Author:        $Author: ruetz $
* Copyright:     Modifications (c) 2000 Imagineer Software Limited
*
* Notes:         
* Private func:  
* History:       Modified from original CLAPACK source code 
*
**************************************************************************/

#include <math.h>
#include <gandalf/linalg/cblas/dscal.h>
#include <gandalf/common/misc_defs.h>
#include <gandalf/common/compare.h>
#include <gandalf/common/misc_error.h>

/* only define this function locally if there is no LAPACK installed */
#if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK)

Gan_Bool
 gan_dscal ( long n, double da, double *dx, long incx )
{
   long i;

   gan_err_test_bool ( n >= 0, "gan_dscal", GAN_ERROR_CBLAS_ILLEGAL_ARG, "" );

   if ( n == 0 ) return GAN_TRUE;
   if ( incx == 1 ) for ( i = n-1; i >= 0; i-- ) *dx++ *= da;
   else for ( i = n-1; i >= 0; i-- ) dx[i*incx] *= da;

   /* success */
   return GAN_TRUE;
}

#endif /* #if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK) */

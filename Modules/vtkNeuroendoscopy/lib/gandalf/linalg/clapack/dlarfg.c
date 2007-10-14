/**************************************************************************
*
* File:          $RCSfile: dlarfg.c,v $
* Module:        CLAPACK function
* Part of:       Gandalf Library
*
* Revision:      $Revision: 1.1.2.1 $
* Last edited:   $Date: 2007/10/14 02:26:45 $
* Author:        $Author: ruetz $
* Copyright:     Modifications (c) 2000 Imagineer Software Limited
*
* Notes:         
* Private func:  
* History:       Modified from original CLAPACK source code 
*
**************************************************************************/

#include <math.h>
#include <gandalf/linalg/clapack/dlarfg.h>
#include <gandalf/linalg/clapack/dlapy2.h>
#include <gandalf/linalg/cblas.h>
#include <gandalf/common/misc_defs.h>
#include <gandalf/common/array.h>
#include <gandalf/common/compare.h>

/* only define this function locally if there is no LAPACK installed */
#if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK)

static double d_sign ( double a, double b )
{
   double x;
   x = (a >= 0 ? a : -a);
   return( b >= 0 ? x : -x);
}

/* modified from CLAPACK source */
Gan_Bool
 gan_dlarfg ( long n, double *alpha, double *x, long incx, double *tau )
{
   double xnorm, beta;

   if ( n <= 1 )
   {
      *tau = 0.0;
      return GAN_TRUE;
   }

   xnorm = gan_dnrm2 ( n-1, x, incx );

   if ( xnorm == 0.0 )
   {
      /*        H  =  I */
      *tau = 0.0;
   }
   else
   {
      /*        general case */
      beta = -d_sign ( gan_dlapy2(*alpha, xnorm), *alpha );
      *tau = (beta - *alpha)/beta;
      gan_dscal ( n-1, 1.0/(*alpha-beta), x, incx);
      *alpha = beta;
   }

   /* success */
   return GAN_TRUE;
}

#endif /* #if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK) */

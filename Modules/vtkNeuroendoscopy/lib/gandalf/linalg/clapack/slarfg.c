/**************************************************************************
*
* File:          $RCSfile: slarfg.c,v $
* Module:        CLAPACK function
* Part of:       Gandalf Library
*
* Revision:      $Revision: 1.1.2.1 $
* Last edited:   $Date: 2007/10/14 02:26:49 $
* Author:        $Author: ruetz $
* Copyright:     Modifications (c) 2000 Imagineer Software Limited
*
* Notes:         
* Private func:  
* History:       Modified from original CLAPACK source code 
*
**************************************************************************/

#include <math.h>
#include <gandalf/linalg/clapack/slarfg.h>
#include <gandalf/linalg/clapack/slapy2.h>
#include <gandalf/linalg/cblas.h>
#include <gandalf/common/misc_defs.h>
#include <gandalf/common/array.h>
#include <gandalf/common/compare.h>

/* only define this function locally if there is no LAPACK installed */
#if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK)

static float f_sign ( float a, float b )
{
   float x;
   x = (a >= 0 ? a : -a);
   return( b >= 0 ? x : -x);
}

/* modified from CLAPACK source */
Gan_Bool
 gan_slarfg ( long n, float *alpha, float *x, long incx, float *tau )
{
   float xnorm, beta;

   if ( n <= 1 )
   {
      *tau = 0.0F;
      return GAN_TRUE;
   }

   xnorm = gan_snrm2 ( n-1, x, incx );

   if ( xnorm == 0.0F )
   {
      /*        H  =  I */
      *tau = 0.0F;
   }
   else
   {
      /*        general case */
      beta = -f_sign ( gan_slapy2(*alpha, xnorm), *alpha );
      *tau = (beta - *alpha)/beta;
      gan_sscal ( n-1, 1.0F/(*alpha-beta), x, incx);
      *alpha = beta;
   }

   /* success */
   return GAN_TRUE;
}

#endif /* #if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK) */

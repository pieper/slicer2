/**************************************************************************
*
* File:          $RCSfile: sgelq2.c,v $
* Module:        CLAPACK function
* Part of:       Gandalf Library
*
* Revision:      $Revision: 1.1.2.1 $
* Last edited:   $Date: 2007/10/14 02:26:48 $
* Author:        $Author: ruetz $
* Copyright:     Modifications (c) 2000 Imagineer Software Limited
*
* Notes:         
* Private func:  
* History:       Modified from original CLAPACK source code 
*
**************************************************************************/

#include <math.h>
#include <gandalf/linalg/clapack/sgelq2.h>
#include <gandalf/linalg/clapack/slarf.h>
#include <gandalf/linalg/clapack/slarfg.h>
#include <gandalf/linalg/cblas.h>
#include <gandalf/common/misc_defs.h>
#include <gandalf/common/array.h>
#include <gandalf/common/compare.h>

/* only define this function locally if there is no LAPACK installed */
#if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK)

/* modified from CLAPACK source */
Gan_Bool gan_sgelq2 ( long m, long n, float *a, long lda, float *tau,
                      float *work, long *info )
{
   long i, minmn = gan_min2(m,n);
   float aii;

   *info = 0;
   if ( m < 0 )
   {
      *info = -1;
      return GAN_FALSE;
   }

   if ( n < 0 )
   {
      *info = -2;
      return GAN_FALSE;
   }

   if ( lda < gan_max2(1,m) )
   {
      *info = -4;
      return GAN_FALSE;
   }

   for ( i = 0; i < minmn; i++ )
   {
      /*        Generate elementary reflector H(i) to annihilate A(i,i+1:n) 
       */

      /* Computing MIN */
      gan_slarfg ( n-i, &a[i*(lda+1)], &a[gan_min2(i+1,n)*lda+i], lda,
                   &tau[i] );
      if ( i < m-1 )
      {
         /*           Apply H(i) to A(i+1:m,i:n) from the right */
         aii = a[i*(lda+1)];
         a[i*(lda+1)] = 1.0;
         gan_slarf ( GAN_RIGHTMULT, m-i-1, n-i, &a[i*(lda+1)], lda, tau[i],
                     &a[i*(lda+1)+1], lda, work );
         a[i*(lda+1)] = aii;
      }
   }

   /* success */
   return GAN_TRUE;
}

#endif /* #if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK) */

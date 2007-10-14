/**************************************************************************
*
* File:          $RCSfile: sgeqr2.c,v $
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
#include <gandalf/linalg/clapack/sgeqr2.h>
#include <gandalf/linalg/clapack/slarf.h>
#include <gandalf/linalg/clapack/slarfg.h>
#include <gandalf/linalg/cblas.h>
#include <gandalf/common/misc_defs.h>
#include <gandalf/common/array.h>
#include <gandalf/common/compare.h>

/* only define this function locally if there is no LAPACK installed */
#if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK)

/* modified from CLAPACK source */
Gan_Bool
 gan_sgeqr2 ( long m, long n, float *a, long lda, float *tau,
              float *work, long *info )
{
   long i;
   long minmn = gan_min2(m,n);
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
      /*        Generate elementary reflector H(i) to annihilate A(i+1:m,i) 
       */

      /* Computing MIN */
      gan_slarfg ( m-i, &a[i*(lda+1)], &a[i*lda+gan_min2(i+1,m)], 1, &tau[i] );
      if ( i < n-1 )
      {
         /*           Apply H(i) to A(i:m,i+1:n) from the left */
         aii = a[i*(lda+1)];
         a[i*(lda+1)] = 1.0;
         gan_slarf ( GAN_LEFTMULT, m-i, n-i-1, &a[i*(lda+1)], 1, tau[i],
                     &a[(i+1)*lda+i], lda, work );
         a[i*(lda+1)] = aii;
      }
   }

   /* success */
   return GAN_TRUE;
}

#endif /* #if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK) */

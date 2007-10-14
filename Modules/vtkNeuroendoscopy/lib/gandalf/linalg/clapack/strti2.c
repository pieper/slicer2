/**************************************************************************
*
* File:          $RCSfile: strti2.c,v $
* Module:        CLAPACK function
* Part of:       Gandalf Library
*
* Revision:      $Revision: 1.1.2.1 $
* Last edited:   $Date: 2007/10/14 02:26:52 $
* Author:        $Author: ruetz $
* Copyright:     Modifications (c) 2000 Imagineer Software Limited
*
* Notes:         
* Private func:  
* History:       Modified from original CLAPACK source code 
*
**************************************************************************/

#include <math.h>
#include <gandalf/linalg/clapack/strti2.h>
#include <gandalf/linalg/cblas.h>
#include <gandalf/common/misc_defs.h>
#include <gandalf/common/array.h>
#include <gandalf/common/compare.h>

/* only define this function locally if there is no LAPACK installed */
#if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK)

/* modified from CLAPACK source */
Gan_Bool
 gan_strti2 ( Gan_LapackUpLoFlag upper, Gan_LapackUnitFlag unit,
              long n, float *a, long lda, long *info )
{
   long j;
   float ajj;

   /* lower triangular form not implemented */
   if ( !upper )
   {
      *info = -1;
      return GAN_FALSE;
   }

   /* size of matrix must be positive */
   if ( n < 0 )
   {
      *info = -3;
      return GAN_FALSE;
   }

   if ( lda < gan_max2_l(1,n) )
   {
      *info = -5;
      return GAN_FALSE;
   }

   for ( j = 0; j < n; j++ )
   {
      if ( !unit )
      {
         if ( a[j*(lda+1)] == 0.0F )
         {
            *info = j+1;
            return GAN_FALSE;
         }
         
         a[j*(lda+1)] = 1.0F/a[j*(lda+1)];
         ajj = -a[j*(lda+1)];
      }
      else ajj = -1.0F;

      gan_strmv ( GAN_MATRIXPART_UPPER, GAN_NOTRANSPOSE, unit, j, a, lda,
                  &a[j*lda], 1 );
      gan_sscal ( j, ajj, &a[j*lda], 1 );
   }

   /* success */
   return GAN_TRUE;
}

#endif /* #if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK) */

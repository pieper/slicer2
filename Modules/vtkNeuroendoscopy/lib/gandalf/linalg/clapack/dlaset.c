/**************************************************************************
*
* File:          $RCSfile: dlaset.c,v $
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
#include <gandalf/linalg/clapack/dlaset.h>
#include <gandalf/common/compare.h>

/* only define this function locally if there is no LAPACK installed */
#if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK)

/* modified from CLAPACK source */

Gan_Bool gan_dlaset(Gan_LapackUpLoFlag upper, long m, long n, double alpha, double beta, double *a, long lda)
{
   long i__1, i__2;
   long i__, j;
#define a_ref(a_1,a_2) a[(a_2)*lda + a_1]

   if (upper == GAN_MATRIXPART_LOWER)
   {
      /* Set the strictly upper triangular or trapezoidal part of the   
         array to ALPHA. */
      for (j = 2; j <= n; ++j)
      {
         /* Computing MIN */
         i__2 = gan_min2(j-1,m);
         for (i__ = 1; i__ <= i__2; ++i__)
            a_ref(i__, j) = alpha;
      }
   }
   else if (upper == GAN_MATRIXPART_UPPER)
   {
      /*        Set the strictly lower triangular or trapezoidal part of the   
                array to ALPHA. */
      i__1 = gan_min2(m,n);
      for (j = 1; j <= i__1; ++j)
      {
         for (i__ = j + 1; i__ <= m; ++i__)
            a_ref(i__, j) = alpha;
      }
   }
   else // upper == GAN_MATRIXPART_WHOLE
   {
      /*        Set the leading m-by-n submatrix to ALPHA. */
      for (j = 1; j <= n; ++j)
         for (i__ = 1; i__ <= m; ++i__)
            a_ref(i__, j) = alpha;
   }

   /*     Set the first min(M,N) diagonal elements to BETA. */
   i__1 = gan_min2(m,n);
   for (i__ = 1; i__ <= i__1; ++i__)
      a_ref(i__, i__) = beta;

   return GAN_TRUE;
}

#undef a_ref

#endif /* #if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK) */

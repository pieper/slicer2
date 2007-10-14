/**************************************************************************
*
* File:          $RCSfile: dlascl.c,v $
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
#include <gandalf/linalg/clapack/dlascl.h>
#include <gandalf/linalg/clapack/dlamch.h>
#include <gandalf/common/compare.h>

/* only define this function locally if there is no LAPACK installed */
#if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK)

/* modified from CLAPACK source */

Gan_Bool gan_dlascl(Gan_LapackMatStorage type, long kl, long ku, 
                    double *cfrom, double *cto, long m, long n, 
                    double *a, long lda, long *info)
{
   long i__2, i__3;
   Gan_Bool done;
   double ctoc;
   long i__, j;
   long k1, k3, k4;
   double cfrom1;
   double cfromc;
   double bignum, smlnum, mul, cto1;
#define a_ref(a_1,a_2) a[(a_2)*lda + a_1]

   *info = 0;
   if (*cfrom == 0.)
      *info = -4;
   else if (m < 0)
      *info = -6;
   else if (n < 0 || (type == GAN_MATSTORAGE_SYMMBANDLOWER && n != m) || (type == GAN_MATSTORAGE_SYMMBANDUPPER && n != m))
      *info = -7;
   else if (type <= GAN_MATSTORAGE_UPPERHESSENBERG && lda < gan_max2(1,m))
      *info = -9;
   else if (type >= GAN_MATSTORAGE_SYMMBANDLOWER)
   {
      /* Computing MAX */
      if (kl < 0 || kl > gan_max2_i(m-1,0))
         *info = -2;
      else /* if(complicated condition) */
      {
         /* Computing MAX */
         if (ku < 0 || ku > gan_max2(n-1,0) ||
             ((type == GAN_MATSTORAGE_SYMMBANDLOWER ||
               type == GAN_MATSTORAGE_SYMMBANDUPPER) && (kl != ku)))
            *info = -3;
         else if ((type == GAN_MATSTORAGE_SYMMBANDLOWER && lda < kl+1) ||
                  (type == GAN_MATSTORAGE_SYMMBANDUPPER && lda < ku+1) ||
                  (type == GAN_MATSTORAGE_BAND && lda < (kl<<1) + ku + 1))
            *info = -9;
      }
   }

   if (*info != 0)
      return GAN_FALSE;

   /*     Quick return if possible */
   if (n == 0 || m == 0)
      return 0;

   /*     Get machine parameters */
   smlnum = gan_dlamch(GAN_LAMCH_SFMIN);
   bignum = 1.0/smlnum;

   cfromc = *cfrom;
   ctoc = *cto;

  L10:
   cfrom1 = cfromc * smlnum;
   cto1 = ctoc / bignum;
   if (fabs(cfrom1) > fabs(ctoc) && ctoc != 0.)
   {
      mul = smlnum;
      done = GAN_FALSE;
      cfromc = cfrom1;
   }
   else if (fabs(cto1) > fabs(cfromc))
   {
      mul = bignum;
      done = GAN_FALSE;
      ctoc = cto1;
   }
   else
   {
      mul = ctoc / cfromc;
      done = GAN_TRUE;
   }

   switch(type)
   {
      case GAN_MATSTORAGE_FULL:
        /*        Full matrix */
        for (j = 0; j < n; ++j)
           for (i__ = 0; i__ < m; ++i__)
              a_ref(i__, j) = a_ref(i__, j) * mul;

        break;

      case GAN_MATSTORAGE_LOWERTRIANGULAR:
        /*        Lower triangular matrix */
        for (j = 0; j < n; ++j)
           for (i__ = j; i__ < m; ++i__)
              a_ref(i__, j) = a_ref(i__, j) * mul;

        break;

      case GAN_MATSTORAGE_UPPERTRIANGULAR:
        /*        Upper triangular matrix */
        for (j = 0; j < n; ++j)
        {
           i__2 = gan_min2_i(j,m-1);
           for (i__ = 0; i__ <= i__2; ++i__)
              a_ref(i__, j) = a_ref(i__, j) * mul;
        }

        break;

      case GAN_MATSTORAGE_UPPERHESSENBERG:
        /*        Upper Hessenberg matrix */
        for (j = 0; j < n; ++j)
        {
           /* Computing MIN */
           i__3 = j + 1;
           i__2 = gan_min2_i(i__3,m-1);
           for (i__ = 0; i__ <= i__2; ++i__)
              a_ref(i__, j) = a_ref(i__, j) * mul;
        }

        break;

      case GAN_MATSTORAGE_SYMMBANDLOWER:
        /*        Lower half of a symmetric band matrix */
        for (j = 0; j < n; ++j)
        {
           /* Computing MIN */
           i__2 = gan_min2_i(kl+1,n-j);
           for (i__ = 0; i__ < i__2; ++i__)
              a_ref(i__, j) = a_ref(i__, j) * mul;
        }

        break;

      case GAN_MATSTORAGE_SYMMBANDUPPER:
        /*        Upper half of a symmetric band matrix */
        for (j = 0; j < n; ++j)
        {
           /* Computing MAX */
           i__2 = ku+2-j-2;
           i__3 = ku;
           for (i__ = gan_max2_i(i__2,0); i__ <= i__3; ++i__)
              a_ref(i__,j) = a_ref(i__,j) * mul;
        }

        break;

      case GAN_MATSTORAGE_BAND:
        /*        Band matrix */
        k1 = kl + ku + 2;
        k3 = (kl << 1) + ku + 1;
        k4 = kl + ku + 1 + m;
        for (j = 0; j < n; ++j)
        {
           i__2 = gan_min2(k3,k4-j-1);
           for (i__ = gan_max2_i(kl-j-2, kl); i__ < i__2; ++i__)
              a_ref(i__, j) = a_ref(i__, j) * mul;
        }

        break;

      default:
        return GAN_FALSE;
   }

   if (! done)
      goto L10;

   return GAN_TRUE;
}

#endif /* #if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK) */

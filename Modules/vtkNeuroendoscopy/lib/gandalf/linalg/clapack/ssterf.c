/**************************************************************************
*
* File:          $RCSfile: ssterf.c,v $
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
#include <gandalf/linalg/clapack/ssterf.h>
#include <gandalf/linalg/clapack/slapy2.h>
#include <gandalf/linalg/clapack/slamch.h>
#include <gandalf/linalg/clapack/slanst.h>
#include <gandalf/linalg/clapack/slascl.h>
#include <gandalf/linalg/clapack/slasrt.h>
#include <gandalf/linalg/clapack/slae2.h>

/* only define this function locally if there is no LAPACK installed */
#if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK)

static float s_sign(float a, float b)
{
   float x;
   x = (a >= 0 ? a : - a);
   return( b >= 0 ? x : -x);
}

/* modified from CLAPACK source */
Gan_Bool gan_ssterf(long n, float *d, float *e, long *info)
{
   float d__1, d__2;
   float oldc;
   long lend, jtot;
   float c__;
   long i__, l, m;
   float p, gamma, r__, s, alpha, sigma, anorm;
   long l1;
   float bb;
   long iscale;
   float oldgam, safmin;
   float safmax;
   long lendsv;
   float ssfmin;
   long nmaxit;
   float ssfmax, rt1, rt2, eps, rte;
   long lsv;
   float eps2;

#if 0
   --e;
   --d__;
#endif

   /* Function Body */
   *info = 0;

   /*     Quick return if possible */
   if (n < 0)
   {
      *info = -1;
      return GAN_FALSE;
   }

   if (n <= 1)
      return GAN_TRUE;

   /*     Determine the unit roundoff for this environment. */
   eps = gan_slamch(GAN_LAMCH_EPS);

   /* Computing 2nd power */
   eps2 = eps*eps;
   safmin = gan_slamch(GAN_LAMCH_SFMIN);
   safmax = 1.0F/safmin;
   ssfmax = sqrt(safmax)/3.0F;
   ssfmin = sqrt(safmin)/eps2;

   /*     Compute the eigenvalues of the tridiagonal matrix. */
   nmaxit = n*30;
   sigma = 0.0F;
   jtot = 0;

   /*     Determine where the matrix splits and choose QL or QR iteration   
          for each block, according to whether top or bottom diagonal   
          element is smaller. */
   l1 = 0;

  L10:
   if (l1 >= n)
      goto L170;

   if (l1 > 0)
      e[l1-1] = 0.0F;

   for (m = l1; m < n-1; ++m)
   {
      if (fabs(e[m]) <= sqrt(fabs(d[m])) * sqrt(sqrt(d[m+1])) * eps)
      {
         e[m] = 0.0F;
         break;
      }
   }

   l = l1;
   lsv = l;
   lend = m;
   lendsv = lend;
   l1 = m + 1;
   if (lend == l)
      goto L10;

   /*     Scale submatrix in rows and columns L to LEND */
   anorm = gan_slanst(GAN_NORMTYPE_INFINITY, lend-l+1, &d[l], &e[l]);
   iscale = 0;
   if (anorm > ssfmax)
   {
      iscale = 1;
      gan_slascl(GAN_MATSTORAGE_FULL, 0, 0, &anorm, &ssfmax, lend-l+1, 1, &d[l], n, info);
      gan_slascl(GAN_MATSTORAGE_FULL, 0, 0, &anorm, &ssfmax, lend-l, 1, &e[l], n, info);
   }
   else if (anorm < ssfmin)
   {
      iscale = 2;
      gan_slascl(GAN_MATSTORAGE_FULL, 0, 0, &anorm, &ssfmin, lend-l+1, 1, &d[l], n, info);
      gan_slascl(GAN_MATSTORAGE_FULL, 0, 0, &anorm, &ssfmin, lend-l, 1, &e[l], n, info);
   }

   for (i__ = l; i__ <= lend-1; ++i__)
   {
      /* Computing 2nd power */
      d__1 = e[i__];
      e[i__] = d__1 * d__1;
   }

   /*     Choose between QL and QR iteration */
   if ((d__1 = d[lend], fabs(d__1)) < (d__2 = d[l], fabs(d__2)))
   {
      lend = lsv;
      l = lendsv;
   }

   if (lend >= l)
   {
      /*        QL Iteration   
      Look for small subdiagonal element. */
     L50:
      if (l != lend)
      {
         for (m = l; m <= lend-1; ++m)
         {
            if ((d__2 = e[m], fabs(d__2)) <= eps2 * (d__1 = d[m] * d[m+1], fabs(d__1)))
               goto L70;
         }
      }

      m = lend;

     L70:
      if (m < lend)
         e[m] = 0.0F;

      p = d[l];
      if (m == l)
         goto L90;

      /*        If remaining matrix is 2 by 2, use DLAE2 to compute its   
                eigenvalues. */
      if (m == l + 1)
      {
         rte = sqrt(e[l]);
         gan_slae2(d[l], rte, d[l+1], &rt1, &rt2);
         d[l] = rt1;
         d[l + 1] = rt2;
         e[l] = 0.;
         l += 2;
         if (l <= lend)
            goto L50;

         goto L150;
      }

      if (jtot == nmaxit)
         goto L150;

      ++jtot;

      /*        Form shift. */
      rte = sqrt(e[l]);
      sigma = (d[l + 1] - p) / (rte * 2.);
      r__ = gan_slapy2(sigma, 1.0F);
      sigma = p - rte / (sigma + s_sign(r__, sigma));

      c__ = 1.;
      s = 0.;
      gamma = d[m] - sigma;
      p = gamma * gamma;

      /*        Inner loop */
      for (i__ = m - 1; i__ >= l; --i__)
      {
         bb = e[i__];
         r__ = p + bb;
         if (i__ != m - 1)
            e[i__ + 1] = s * r__;

         oldc = c__;
         c__ = p / r__;
         s = bb / r__;
         oldgam = gamma;
         alpha = d[i__];
         gamma = c__ * (alpha - sigma) - s * oldgam;
         d[i__ + 1] = oldgam + (alpha - gamma);
         if (c__ != 0.)
            p = gamma * gamma / c__;
         else
            p = oldc * bb;
      }

      e[l] = s * p;
      d[l] = sigma + gamma;
      goto L50;

      /*        Eigenvalue found. */

     L90:
      d[l] = p;

      ++l;
      if (l <= lend) {
         goto L50;
      }
      goto L150;

   }
   else
   {
      /*        QR Iteration   

      Look for small superdiagonal element. */

     L100:
      for (m = l; m >= lend+1; --m) {
         if ((d__2 = e[m - 1], fabs(d__2)) <= eps2 * (d__1 = d[m] * d[m-1], fabs(d__1))) {
            goto L120;
         }
         /* L110: */
      }
      m = lend;

     L120:
      if (m > lend)
         e[m - 1] = 0.;

      p = d[l];
      if (m == l)
         goto L140;

      /*        If remaining matrix is 2 by 2, use DLAE2 to compute its   
                eigenvalues. */

      if (m == l - 1)
      {
         rte = sqrt(e[l - 1]);
         gan_slae2(d[l], rte, d[l-1], &rt1, &rt2);
         d[l] = rt1;
         d[l - 1] = rt2;
         e[l - 1] = 0.;
         l += -2;
         if (l >= lend)
            goto L100;

         goto L150;
      }

      if (jtot == nmaxit)
         goto L150;

      ++jtot;

      /*        Form shift. */
      rte = sqrt(e[l - 1]);
      sigma = (d[l - 1] - p) / (rte * 2.);
      r__ = gan_slapy2(sigma, 1.0F);
      sigma = p - rte / (sigma + s_sign(r__, sigma));

      c__ = 1.0F;
      s = 0.0F;
      gamma = d[m] - sigma;
      p = gamma * gamma;

      /*        Inner loop */
      for (i__ = m; i__ <= l-1; ++i__)
      {
         bb = e[i__];
         r__ = p + bb;
         if (i__ != m)
            e[i__ - 1] = s * r__;

         oldc = c__;
         c__ = p / r__;
         s = bb / r__;
         oldgam = gamma;
         alpha = d[i__ + 1];
         gamma = c__ * (alpha - sigma) - s * oldgam;
         d[i__] = oldgam + (alpha - gamma);
         if (c__ != 0.)
            p = gamma * gamma / c__;
         else
            p = oldc * bb;
      }

      e[l - 1] = s * p;
      d[l] = sigma + gamma;
      goto L100;

      /*        Eigenvalue found. */

     L140:
      d[l] = p;

      --l;
      if (l >= lend)
         goto L100;

      goto L150;
   }

   /*     Undo scaling if necessary */

  L150:
   if (iscale == 1)
      gan_slascl(GAN_MATSTORAGE_FULL, 0, 0, &ssfmax, &anorm, lendsv-lsv+1,
                 1, &d[lsv], n, info);

   if (iscale == 2)
      gan_slascl(GAN_MATSTORAGE_FULL, 0, 0, &ssfmin, &anorm, lendsv-lsv+1,
                 1, &d[lsv], n, info);

   /*     Check for no convergence to an eigenvalue after a total   
          of N*MAXIT iterations. */
   if (jtot < nmaxit)
      goto L10;

   for (i__ = 1; i__ <= n-1; ++i__)
   {
      if (e[i__] != 0.0F)
         ++(*info);
   }
   goto L180;

   /*     Sort eigenvalues in increasing order. */
  L170:
   gan_slasrt(GAN_SORTORDER_INCREASING, n, d, info);

  L180:
   return GAN_TRUE;
}

#endif /* #if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK) */

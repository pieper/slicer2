/**************************************************************************
*
* File:          $RCSfile: ssteqr.c,v $
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
#include <gandalf/linalg/clapack/ssteqr.h>
#include <gandalf/linalg/clapack/slae2.h>
#include <gandalf/linalg/clapack/slaev2.h>
#include <gandalf/linalg/clapack/slascl.h>
#include <gandalf/linalg/clapack/slamch.h>
#include <gandalf/linalg/clapack/slapy2.h>
#include <gandalf/linalg/clapack/slasr.h>
#include <gandalf/linalg/clapack/slasrt.h>
#include <gandalf/linalg/clapack/slartg.h>
#include <gandalf/linalg/clapack/slanst.h>
#include <gandalf/linalg/clapack/slaset.h>
#include <gandalf/linalg/cblas/sswap.h>
#include <gandalf/common/compare.h>

/* only define this function locally if there is no LAPACK installed */
#if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK)

/* modified from CLAPACK source */

static float s_sign(float a, float b)
{
   float x;
   x = (a >= 0 ? a : - a);
   return( b >= 0 ? x : -x);
}

Gan_Bool gan_ssteqr(Gan_LapackEvecType compz, long n, float *d,
                    float *e, float *z, long ldz, float *work, 
                    long *info)
{
   long i__1;
   float d__1, d__2;
   long lend, jtot;
   float b, c__, f, g;
   long i__, j, k, l, m;
   float p, r__, s;
   float anorm;
   long l1;
   long lendm1, lendp1;
   long ii;
   long iscale;
   float safmin;
   float safmax;
   long lendsv;
   float ssfmin;
   long nmaxit, icompz;
   float ssfmax;
   long lm1, mm1, nm1;
   float rt1, rt2, eps;
   long lsv;
   float tst, eps2;
#define z___ref(a_1,a_2) z[(a_2)*ldz + a_1]

   *info = 0;

   if (compz == GAN_EVECTYPE_EVALSONLY)
      icompz = 0;
   else if (compz == GAN_EVECTYPE_EVECSORIGINAL)
      icompz = 1;
   else if (compz == GAN_EVECTYPE_EVECSTRIDIAGONAL)
      icompz = 2;
   else
      icompz = -1;

   if (icompz < 0)
      *info = -1;
   else if (n < 0)
      *info = -2;
   else if (ldz < 1 || (icompz > 0 && ldz < gan_max2(1,n)))
      *info = -6;

   if (*info != 0)
      return GAN_FALSE;

   /* Quick return if possible */
   if (n == 0)
      return GAN_TRUE;

   if (n == 1)
   {
      if (icompz == 2)
         z___ref(0,0) = 1.0F;

      return GAN_TRUE;
   }

   /* Determine the unit roundoff and over/underflow thresholds. */
   eps = gan_slamch(GAN_LAMCH_EPS);

   /* Computing 2nd power */
   d__1 = eps;
   eps2 = d__1*d__1;
   safmin = gan_slamch(GAN_LAMCH_SFMIN);
   safmax = 1.0F/safmin;
   ssfmax = sqrt(safmax)/3.0F;
   ssfmin = sqrt(safmin)/eps2;

   /* Compute the eigenvalues and eigenvectors of the tridiagonal matrix. */
   if (icompz == 2)
      gan_slaset(GAN_MATSTORAGE_FULL, n, n, 0.0F, 1.0F, z, ldz);

   nmaxit = n*30;
   jtot = 0;

   /* Determine where the matrix splits and choose QL or QR iteration   
      for each block, according to whether top or bottom diagonal   
      element is smaller. */
   l1 = 1;
   nm1 = n-1;

L10:
   if (l1 > n)
      goto L160;

   if (l1 > 1)
      e[l1-2] = 0.0F;

   if (l1 <= nm1)
   {
      i__1 = nm1;
      for (m = l1; m <= i__1; ++m)
      {
         tst = (d__1 = e[m-1], fabs(d__1));
         if (tst == 0.0F)
            goto L30;

         if (tst <= sqrt(fabs(d[m-1])) * sqrt(fabs(d[m+1])) * eps)
         {
            e[m-1] = 0.0F;
            goto L30;
         }
      }
   }

   m = n;

  L30:
   l = l1;
   lsv = l;
   lend = m;
   lendsv = lend;
   l1 = m + 1;
   if (lend == l)
      goto L10;

   /*     Scale submatrix in rows and columns L to LEND */
   anorm = gan_slanst(GAN_NORMTYPE_INFINITY, lend-l+1, &d[l-1], &e[l-1]);
   iscale = 0;
   if (anorm == 0.0F)
      goto L10;

   if (anorm > ssfmax)
   {
      iscale = 1;
      gan_slascl(GAN_MATSTORAGE_FULL, 0, 0, &anorm, &ssfmax, lend-l+1, 1,
                 &d[l-1], n, info);
      gan_slascl(GAN_MATSTORAGE_FULL, 0, 0, &anorm, &ssfmax, lend-l, 1,
                 &e[l-1], n, info);
   }
   else if (anorm < ssfmin)
   {
      iscale = 2;
      gan_slascl(GAN_MATSTORAGE_FULL, 0, 0, &anorm, &ssfmin, lend-l+1, 1,
                 &d[l-1], n, info);
      gan_slascl(GAN_MATSTORAGE_FULL, 0, 0, &anorm, &ssfmin, lend-l, 1,
                 &e[l-1], n, info);
   }

   /*     Choose between QL and QR iteration */
   if ((d__1 = d[lend-1], fabs(d__1)) < (d__2 = d[l-1], fabs(d__2)))
   {
      lend = lsv;
      l = lendsv;
   }

   if (lend > l)
   {
      /*        QL Iteration   

      Look for small subdiagonal element. */
     L40:
      if (l != lend) {
         lendm1 = lend - 1;
         i__1 = lendm1;
         for (m = l; m <= i__1; ++m)
         {
            /* Computing 2nd power */
            d__2 = fabs(e[m-1]);
            tst = d__2*d__2;
            if (tst <= eps2 * fabs(d[m]) * fabs(d[m+1]) + safmin)
               goto L60;
         }
      }

      m = lend;

     L60:
      if (m < lend) {
         e[m-1] = 0.;
      }
      p = d[l-1];
      if (m == l)
         goto L80;

      /*        If remaining matrix is 2-by-2, use DLAE2 or SLAEV2   
                to compute its eigensystem. */

      if (m == l + 1)
      {
         if (icompz > 0)
         {
            gan_slaev2(d[l-1], e[l-1], d[l], &rt1, &rt2, &c__, &s);
            work[l-1] = c__;
            work[n-2+l] = s;
            gan_slasr(GAN_PLANEROTATION_RIGHT, GAN_PIVOTTYPE_VARIABLE,
                      GAN_PLANEROTATIONDIRECTION_BACKWARDS, n, 2,
                      &work[l-1], &work[n-2+l], &z___ref(0,l-1), ldz);
         }
         else
         {
            gan_slae2(d[l-1], e[l-1], d[l], &rt1, &rt2);
         }

         d[l-1] = rt1;
         d[l] = rt2;
         e[l-1] = 0.;
         l += 2;
         if (l <= lend) {
            goto L40;
         }
         goto L140;
      }

      if (jtot == nmaxit) {
         goto L140;
      }
      ++jtot;

      /*        Form shift. */

      g = (d[l] - p) / (e[l-1] * 2.0F);
      r__ = gan_slapy2(g, 1.0F);
      g = d[m-1] - p + e[l-1] / (g + s_sign(r__, g));

      s = 1.;
      c__ = 1.;
      p = 0.;

      /*        Inner loop */

      mm1 = m - 1;
      i__1 = l;
      for (i__ = mm1; i__ >= i__1; --i__)
      {
         f = s * e[i__-1];
         b = c__ * e[i__-1];
         gan_slartg(g, f, &c__, &s, &r__);
         if (i__ != m - 1)
            e[i__] = r__;

         g = d[i__] - p;
         r__ = (d[i__-1] - g) * s + c__ * 2. * b;
         p = s * r__;
         d[i__] = g + p;
         g = c__ * r__ - b;

         /*           If eigenvectors are desired, then save rotations. */
         if (icompz > 0)
         {
            work[i__-1] = c__;
            work[n-2+i__] = -s;
         }
      }

      /*        If eigenvectors are desired, then apply saved rotations. */

      if (icompz > 0)
         gan_slasr(GAN_PLANEROTATION_RIGHT, GAN_PIVOTTYPE_VARIABLE,
                   GAN_PLANEROTATIONDIRECTION_BACKWARDS, n, m-l+1,
                   &work[l-1], &work[n-2+l], &z___ref(0, l-1), ldz);

      d[l-1] -= p;
      e[l-1] = g;
      goto L40;

      /*        Eigenvalue found. */

     L80:
      d[l-1] = p;

      ++l;
      if (l <= lend) {
         goto L40;
      }
      goto L140;

   } else {

      /*        QR Iteration   

      Look for small superdiagonal element. */

     L90:
      if (l != lend) {
         lendp1 = lend + 1;
         i__1 = lendp1;
         for (m = l; m >= i__1; --m)
         {
            /* Computing 2nd power */
            d__2 = (d__1 = e[m-2], fabs(d__1));
            tst = d__2 * d__2;
            if (tst <= eps2 * (d__1 = d[m-1], fabs(d__1)) * (d__2 = d[m-2], fabs(d__2)) + safmin) {
               goto L110;
            }
            /* L100: */
         }
      }

      m = lend;

     L110:
      if (m > lend)
         e[m-2] = 0.0F;

      p = d[l-1];
      if (m == l)
         goto L130;

      /*        If remaining matrix is 2-by-2, use DLAE2 or SLAEV2   
                to compute its eigensystem. */
      if (m == l-1)
      {
         if (icompz > 0)
         {
            gan_slaev2(d[l-2], e[l-2], d[l-1], &rt1, &rt2, &c__, &s);
            work[m-1] = c__;
            work[n-2+m] = s;
            gan_slasr(GAN_PLANEROTATION_RIGHT, GAN_PIVOTTYPE_VARIABLE,
                      GAN_PLANEROTATIONDIRECTION_FORWARDS, n, 2,
                      &work[m-1], &work[n-2+m], &z___ref(0, l-2), ldz);
         }
         else
            gan_slae2(d[l-2], e[l-2], d[l-1], &rt1, &rt2);

         d[l-2] = rt1;
         d[l-1] = rt2;
         e[l-2] = 0.0F;
         l += -2;
         if (l >= lend)
            goto L90;

         goto L140;
      }

      if (jtot == nmaxit)
         goto L140;

      ++jtot;

      /*        Form shift. */
      g = (d[l-2] - p) / (e[l-2] * 2.0F);
      r__ = gan_slapy2(g, 1.0F);
      g = d[m-1] - p + e[l-2] / (g + s_sign(r__, g));

      s = 1.0F;
      c__ = 1.0F;
      p = 0.0F;

      /*        Inner loop */
      lm1 = l-1;
      for (i__ = m; i__ <= lm1; ++i__)
      {
         f = s * e[i__-1];
         b = c__ * e[i__-1];
         gan_slartg(g, f, &c__, &s, &r__);
         if (i__ != m)
            e[i__-2] = r__;

         g = d[i__-1] - p;
         r__ = (d[i__] - g) * s + c__ * 2. * b;
         p = s * r__;
         d[i__-1] = g + p;
         g = c__ * r__ - b;

         /*           If eigenvectors are desired, then save rotations. */
         if (icompz > 0)
         {
            work[i__-1] = c__;
            work[n-2+i__] = s;
         }
      }

      /*        If eigenvectors are desired, then apply saved rotations. */
      if (icompz > 0)
         gan_slasr(GAN_PLANEROTATION_RIGHT, GAN_PIVOTTYPE_VARIABLE,
                   GAN_PLANEROTATIONDIRECTION_FORWARDS, n, l-m+1,
                   &work[m-1], &work[n-2+m], &z___ref(0, m-1), ldz);

      d[l-1] -= p;
      e[lm1-1] = g;
      goto L90;

      /*        Eigenvalue found. */
L130:
      d[l-1] = p;

      --l;
      if (l >= lend)
         goto L90;

      goto L140;
   }

   /*     Undo scaling if necessary */
  L140:
   if (iscale == 1)
   {
      gan_slascl(GAN_MATSTORAGE_FULL, 0, 0, &ssfmax, &anorm, lendsv-lsv+1,
                 1, &d[lsv-1], n, info);
      gan_slascl(GAN_MATSTORAGE_FULL, 0, 0, &ssfmax, &anorm, lendsv-lsv,
                 1, &e[lsv-1], n, info);
   }
   else if (iscale == 2)
   {
      gan_slascl(GAN_MATSTORAGE_FULL, 0, 0, &ssfmin, &anorm, lendsv-lsv+1,
                 1, &d[lsv-1], n, info);
      gan_slascl(GAN_MATSTORAGE_FULL, 0, 0, &ssfmin, &anorm, lendsv-lsv,
                 1, &e[lsv-1], n, info);
   }

   /*     Check for no convergence to an eigenvalue after a total   
          of N*MAXIT iterations. */
   if (jtot < nmaxit)
      goto L10;

   i__1 = n - 1;
   for (i__ = 1; i__ <= i__1; ++i__)
      if (e[i__-1] != 0.0F)
         ++(*info);

   goto L190;

   /*     Order eigenvalues and eigenvectors. */

  L160:
   if (icompz == 0)
      /*        Use Quick Sort */
      gan_slasrt(GAN_SORTORDER_INCREASING, n, d, info);
   else
   {
      /*        Use Selection Sort to minimize swaps of eigenvectors */
      for (ii = 2; ii <= n; ++ii)
      {
         i__ = ii-1;
         k = i__;
         p = d[i__-1];
         for (j = ii; j <= n; ++j)
         {
            if (d[j-1] < p)
            {
               k = j;
               p = d[j-1];
            }
         }

         if (k != i__)
         {
            d[k-1] = d[i__-1];
            d[i__-1] = p;
            gan_sswap(n, &z___ref(0, i__-1), 1, &z___ref(0, k-1), 1);
         }
      }
   }

  L190:
   return GAN_TRUE;
}

#undef z___ref

#endif /* #if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK) */

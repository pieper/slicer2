/**************************************************************************
*
* File:          $RCSfile: dlasr.c,v $
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
#include <gandalf/linalg/clapack/dlasr.h>
#include <gandalf/common/compare.h>

/* only define this function locally if there is no LAPACK installed */
#if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK)

/* modified from CLAPACK source */

Gan_Bool gan_dlasr(Gan_LapackPlaneRotationType side,
                   Gan_LapackPivotType pivot,
                   Gan_LapackPlaneRotationDirection direct,
                   long m, long n, double *c, double *s, double *a, long lda)
{
   long info;
   double temp;
   long i__, j;
   double ctemp, stemp;
#define a_ref(a_1,a_2) a[(a_2)*lda + a_1]

   info = 0;
   if (m < 0)
      info = 4;
   else if (n < 0)
      info = 5;
   else if (lda < gan_max2(1,m))
      info = 9;

   if (info != 0)
      return GAN_FALSE;

   /*     Quick return if possible */
   if (m == 0 || n == 0)
      return GAN_TRUE;

   if (side == GAN_PLANEROTATION_LEFT)
   {
      /*        Form  P * A */
      if (pivot == GAN_PIVOTTYPE_VARIABLE)
      {
         if (direct == GAN_PLANEROTATIONDIRECTION_FORWARDS)
         {
            for (j = 0; j < m-1; ++j)
            {
               ctemp = c[j];
               stemp = s[j];
               if (ctemp != 1.0 || stemp != 0.0)
               {
                  for (i__ = 0; i__ < n; ++i__)
                  {
                     temp = a_ref(j+1, i__);
                     a_ref(j+1, i__) = ctemp * temp - stemp * a_ref(j,i__);
                     a_ref(j,i__) = stemp * temp + ctemp * a_ref(j,i__);
                  }
               }
            }
         }
         else if (direct == GAN_PLANEROTATIONDIRECTION_BACKWARDS)
         {
            for (j = m-2; j >= 0; --j)
            {
               ctemp = c[j];
               stemp = s[j];
               if (ctemp != 1.0 || stemp != 0.0)
               {
                  for (i__ = 0; i__ < n; ++i__)
                  {
                     temp = a_ref(j+1, i__);
                     a_ref(j+1, i__) = ctemp * temp - stemp * a_ref(j, i__);
                     a_ref(j, i__) = stemp * temp + ctemp * a_ref(j,i__);
                  }
               }
            }
         }
      }
      else if (pivot == GAN_PIVOTTYPE_TOP)
      {
         if (direct == GAN_PLANEROTATIONDIRECTION_FORWARDS)
         {
            for (j = 1; j < m; ++j)
            {
               ctemp = c[j-1];
               stemp = s[j-1];
               if (ctemp != 1.0 || stemp != 0.0)
               {
                  for (i__ = 1; i__ <= n; ++i__)
                  {
                     temp = a_ref(j, i__);
                     a_ref(j, i__) = ctemp * temp - stemp * a_ref(1,i__);
                     a_ref(1, i__) = stemp * temp + ctemp * a_ref(1,i__);
                  }
               }
            }
         }
         else if (direct == GAN_PLANEROTATIONDIRECTION_BACKWARDS)
         {
            for (j = m-1; j >= 1; --j)
            {
               ctemp = c[j-1];
               stemp = s[j-1];
               if (ctemp != 1.0 || stemp != 0.0)
               {
                  for (i__ = 0; i__ < n; ++i__)
                  {
                     temp = a_ref(j, i__);
                     a_ref(j, i__) = ctemp * temp - stemp * a_ref(1,i__);
                     a_ref(1, i__) = stemp * temp + ctemp * a_ref(1,i__);
                  }
               }
            }
         }
      }
      else if (pivot == GAN_PIVOTTYPE_TOP)
      {
         if (direct == GAN_PLANEROTATIONDIRECTION_FORWARDS)
         {
            for (j = 0; j < m-1; ++j)
            {
               ctemp = c[j];
               stemp = s[j];
               if (ctemp != 1.0 || stemp != 0.0)
               {
                  for (i__ = 0; i__ < n; ++i__)
                  {
                     temp = a_ref(j, i__);
                     a_ref(j, i__) = stemp * a_ref(m-1, i__) + ctemp * temp;
                     a_ref(m-1, i__) = ctemp * a_ref(m-1, i__) - stemp * temp;
                  }
               }
            }
         }
         else if (direct == GAN_PLANEROTATIONDIRECTION_BACKWARDS)
         {
            for (j = m-2; j >= 0; --j)
            {
               ctemp = c[j];
               stemp = s[j];
               if (ctemp != 1.0 || stemp != 0.0)
               {
                  for (i__ = 0; i__ < n; ++i__)
                  {
                     temp = a_ref(j, i__);
                     a_ref(j, i__) = stemp * a_ref(m-1, i__) + ctemp * temp;
                     a_ref(m-1, i__) = ctemp * a_ref(m-1, i__) - stemp * temp;
                  }
               }
            }
         }
      }
   }
   else if (side == GAN_PLANEROTATION_RIGHT)
   {
      /*        Form A * P' */
      if (pivot == GAN_PIVOTTYPE_VARIABLE)
      {
         if (direct == GAN_PLANEROTATIONDIRECTION_FORWARDS)
         {
            for (j = 0; j < n-1; ++j)
            {
               ctemp = c[j];
               stemp = s[j];
               if (ctemp != 1.0 || stemp != 0.0)
               {
                  for (i__ = 0; i__ < m; ++i__)
                  {
                     temp = a_ref(i__, j+1);
                     a_ref(i__, j+1) = ctemp * temp - stemp * a_ref(i__, j);
                     a_ref(i__, j) = stemp * temp + ctemp * a_ref(i__,j);
                  }
               }
            }
         }
         else if (direct == GAN_PLANEROTATIONDIRECTION_BACKWARDS)
         {
            for (j = n-2; j >= 0; --j)
            {
               ctemp = c[j];
               stemp = s[j];
               if (ctemp != 1.0 || stemp != 0.0)
               {
                  for (i__ = 0; i__ < m; ++i__)
                  {
                     temp = a_ref(i__, j+1);
                     a_ref(i__, j+1) = ctemp * temp - stemp * a_ref(i__, j);
                     a_ref(i__, j) = stemp * temp + ctemp * a_ref(i__,j);
                  }
               }
            }
         }
      }
      else if (pivot == GAN_PIVOTTYPE_TOP)
      {
         if (direct == GAN_PLANEROTATIONDIRECTION_FORWARDS)
         {
            for (j = 2; j <= n; ++j) {
               ctemp = c[j-1];
               stemp = s[j-1];
               if (ctemp != 1.0 || stemp != 0.0)
               {
                  for (i__ = 0; i__ < m; ++i__)
                  {
                     temp = a_ref(i__, j);
                     a_ref(i__, j) = ctemp * temp - stemp * a_ref(i__,1);
                     a_ref(i__, 1) = stemp * temp + ctemp * a_ref(i__,1);
                  }
               }
            }
         }
         else if (direct == GAN_PLANEROTATIONDIRECTION_BACKWARDS)
         {
            for (j = n-1; j >= 1; --j)
            {
               ctemp = c[j-1];
               stemp = s[j-1];
               if (ctemp != 1.0 || stemp != 0.0)
               {
                  for (i__ = 0; i__ < m; ++i__)
                  {
                     temp = a_ref(i__, j);
                     a_ref(i__, j) = ctemp * temp - stemp * a_ref(i__,1);
                     a_ref(i__, 1) = stemp * temp + ctemp * a_ref(i__,1);
                  }
               }
            }
         }
      }
      else if (pivot == GAN_PIVOTTYPE_BOTTOM)
      {
         if (direct == GAN_PLANEROTATIONDIRECTION_FORWARDS)
         {
            for (j = 0; j < n-1; ++j)
            {
               ctemp = c[j];
               stemp = s[j];
               if (ctemp != 1.0 || stemp != 0.0)
               {
                  for (i__ = 1; i__ <= m; ++i__)
                  {
                     temp = a_ref(i__, j);
                     a_ref(i__, j) = stemp * a_ref(i__, n-1) + ctemp * temp;
                     a_ref(i__, n-1) = ctemp * a_ref(i__, n-1) - stemp * temp;
                  }
               }
            }
         }
         else if (direct == GAN_PLANEROTATIONDIRECTION_BACKWARDS)
         {
            for (j = n-2; j >= 0; --j)
            {
               ctemp = c[j];
               stemp = s[j];
               if (ctemp != 1.0 || stemp != 0.0)
               {
                  for (i__ = 0; i__ < m; ++i__)
                  {
                     temp = a_ref(i__, j);
                     a_ref(i__, j) = stemp * a_ref(i__, n-1) + ctemp * temp;
                     a_ref(i__, n-1) = ctemp * a_ref(i__, n-1) - stemp * temp;
                  }
               }
            }
         }
      }
   }

   return GAN_TRUE;
}

#undef a_ref

#endif /* #if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK) */

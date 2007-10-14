/**************************************************************************
*
* File:          $RCSfile: slaev2.c,v $
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
#include <gandalf/linalg/clapack/slaev2.h>

/* only define this function locally if there is no LAPACK installed */
#if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK)

/* modified from CLAPACK source */

Gan_Bool gan_slaev2(float a, float b, float c, 
                    float *rt1, float *rt2, float *cs1, float *sn1)
{
   float d__1;
   float acmn, acmx, ab, df, cs, ct, tb, sm, tn, rt, adf, acs;
   long sgn1, sgn2;

   sm = a + c;
   df = a - c;
   adf = fabs(df);
   tb = b + b;
   ab = fabs(tb);
   if (fabs(a) > fabs(c))
   {
      acmx = a;
      acmn = c;
   }
   else
   {
      acmx = c;
      acmn = a;
   }

   if (adf > ab)
   {
      /* Computing 2nd power */
      d__1 = ab / adf;
      rt = adf * sqrt(d__1 * d__1 + 1.0F);
   }
   else if (adf < ab)
   {
      /* Computing 2nd power */
      d__1 = adf / ab;
      rt = ab * sqrt(d__1 * d__1 + 1.0F);
   }
   else
   {
      /*        Includes case AB=ADF=0 */
      rt = ab * sqrt(2.0F);
   }

   if (sm < 0.0F)
   {
      *rt1 = (sm - rt) * 0.5F;
      sgn1 = -1;

      /*        Order of execution important.   
                To get fully accurate smaller eigenvalue,   
                next line needs to be executed in higher precision. */
      *rt2 = acmx / *rt1 * acmn - b / *rt1 * b;
   }
   else if (sm > 0.)
   {
      *rt1 = (sm + rt) * 0.5F;
      sgn1 = 1;

      /*        Order of execution important.   
                To get fully accurate smaller eigenvalue,   
                next line needs to be executed in higher precision. */
      *rt2 = acmx / *rt1 * acmn - b / *rt1 * b;
   }
   else
   {
      /*        Includes case RT1 = RT2 = 0 */
      *rt1 = rt * 0.5F;
      *rt2 = rt * -0.5F;
      sgn1 = 1;
   }

   /*     Compute the eigenvector */
   if (df >= 0.0F)
   {
      cs = df + rt;
      sgn2 = 1;
   }
   else
   {
      cs = df - rt;
      sgn2 = -1;
   }

   acs = fabs(cs);
   if (acs > ab)
   {
      ct = -tb / cs;
      *sn1 = 1. / sqrt(ct * ct + 1.);
      *cs1 = ct * *sn1;
   }
   else
   {
      if (ab == 0.0F)
      {
         *cs1 = 1.0F;
         *sn1 = 0.0F;
      }
      else
      {
         tn = -cs / tb;
         *cs1 = 1.0F / sqrt(tn * tn + 1.0F);
         *sn1 = tn * *cs1;
      }
   }

   if (sgn1 == sgn2)
   {
      tn = *cs1;
      *cs1 = -(*sn1);
      *sn1 = tn;
   }

   return GAN_TRUE;
}

#endif /* #if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK) */


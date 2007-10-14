/**************************************************************************
*
* File:          $RCSfile: slasrt.c,v $
* Module:        CLAPACK function
* Part of:       Gandalf Library
*
* Revision:      $Revision: 1.1.2.1 $
* Last edited:   $Date: 2007/10/14 02:26:50 $
* Author:        $Author: ruetz $
* Copyright:     Modifications (c) 2000 Imagineer Software Limited
*
* Notes:         
* Private func:  
* History:       Modified from original CLAPACK source code 
*
**************************************************************************/

#include <math.h>
#include <gandalf/linalg/clapack/slasr.h>
#include <gandalf/common/compare.h>

/* only define this function locally if there is no LAPACK installed */
#if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK)

/* modified from CLAPACK source */

Gan_Bool gan_slasrt(Gan_LapackSortOrder id, long n, float *d, long *info)
{
   long i__1, i__2;
   long endd, i__, j;
   long stack[64]    /* was [2][32] */;
   float dmnmx, d1, d2, d3;
   long start;
   long stkpnt, dir;
   float tmp;
#define stack_ref(a_1,a_2) stack[(a_2)*2 + a_1]

   *info = 0;
   dir = -1;
   if (id == GAN_SORTORDER_DECREASING)
      dir = 0;
   else if (id == GAN_SORTORDER_INCREASING)
      dir = 1;

   if (dir == -1)
      *info = -1;
   else if (n < 0)
      *info = -2;

   if (*info != 0)
      return GAN_FALSE;

   /*     Quick return if possible */
   if (n <= 1)
      return GAN_TRUE;

   stkpnt = 0;
   stack_ref(0, 0) = 1;
   stack_ref(1, 0) = n;
L10:
   start = stack_ref(0, stkpnt);
   endd = stack_ref(1, stkpnt);
   --stkpnt;
   if (endd-start <= 20 && endd-start > 0)
   {
      /*        Do Insertion sort on D( START:ENDD ) */
      if (dir == 0)
      {
         /*           Sort into decreasing order */
         i__1 = endd;
         for (i__ = start + 1; i__ <= i__1; ++i__)
         {
            i__2 = start + 1;
            for (j = i__; j >= i__2; --j)
            {
               if (d[j] > d[j - 1])
               {
                  dmnmx = d[j];
                  d[j] = d[j-1];
                  d[j-1] = dmnmx;
               }
               else
                  goto L30;
            }
           L30:
            ;
         }
      }
      else
      {
         /*           Sort into increasing order */
         i__1 = endd;
         for (i__ = start + 1; i__ <= endd; ++i__)
         {
            i__2 = start + 1;
            for (j = i__; j >= i__2; --j)
            {
               if (d[j] < d[j - 1])
               {
                  dmnmx = d[j];
                  d[j] = d[j-1];
                  d[j-1] = dmnmx;
               }
               else
                  goto L50;
            }
           L50:
            ;
         }
      }
   }
   else if (endd - start > 20)
   {
      /* Partition D( START:ENDD ) and stack parts, largest one first   
         Choose partition entry as median of 3 */
      d1 = d[start];
      d2 = d[endd];
      i__ = (start + endd) / 2;
      d3 = d[i__];
      if (d1 < d2)
      {
         if (d3 < d1)
            dmnmx = d1;
         else if (d3 < d2)
            dmnmx = d3;
         else
            dmnmx = d2;
      }
      else
      {
         if (d3 < d2)
            dmnmx = d2;
         else if (d3 < d1)
            dmnmx = d3;
         else
            dmnmx = d1;
      }

      if (dir == 0)
      {
         /* Sort into decreasing order */
         i__ = start - 1;
         j = endd + 1;
        L60:
        L70:
         --j;
         if (d[j] < dmnmx)
            goto L70;

        L80:
         ++i__;
         if (d[i__] > dmnmx)
            goto L80;

         if (i__ < j)
         {
            tmp = d[i__];
            d[i__] = d[j];
            d[j] = tmp;
            goto L60;
         }

         if (j-start > endd-j-1)
         {
            ++stkpnt;
            stack_ref(0, stkpnt) = start;
            stack_ref(1, stkpnt) = j;
            ++stkpnt;
            stack_ref(0, stkpnt) = j + 1;
            stack_ref(1, stkpnt) = endd;
         }
         else
         {
            ++stkpnt;
            stack_ref(0, stkpnt) = j + 1;
            stack_ref(1, stkpnt) = endd;
            ++stkpnt;
            stack_ref(0, stkpnt) = start;
            stack_ref(1, stkpnt) = j;
         }
      }
      else
      {
         /*           Sort into increasing order */
         i__ = start - 1;
         j = endd + 1;
L90:
L100:
         --j;
         if (d[j] > dmnmx)
            goto L100;
L110:
         ++i__;
         if (d[i__] < dmnmx)
            goto L110;

         if (i__ < j)
         {
            tmp = d[i__];
            d[i__] = d[j];
            d[j] = tmp;
            goto L90;
         }

         if (j - start > endd - j - 1)
         {
            ++stkpnt;
            stack_ref(0, stkpnt) = start;
            stack_ref(1, stkpnt) = j;
            ++stkpnt;
            stack_ref(0, stkpnt) = j + 1;
            stack_ref(1, stkpnt) = endd;
         }
         else
         {
            ++stkpnt;
            stack_ref(0, stkpnt) = j + 1;
            stack_ref(1, stkpnt) = endd;
            ++stkpnt;
            stack_ref(0, stkpnt) = start;
            stack_ref(1, stkpnt) = j;
         }
      }
   }

   if (stkpnt >= 0)
      goto L10;

   return GAN_TRUE;
}

#undef stack_ref

#endif /* #if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK) */



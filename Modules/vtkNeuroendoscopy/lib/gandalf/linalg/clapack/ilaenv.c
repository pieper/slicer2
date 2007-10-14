/**************************************************************************
*
* File:          $RCSfile: ilaenv.c,v $
* Module:        CLAPACK function
* Part of:       Gandalf Library
*
* Revision:      $Revision: 1.1.2.1 $
* Last edited:   $Date: 2007/10/14 02:26:47 $
* Author:        $Author: ruetz $
* Copyright:     Modifications (c) 2000 Imagineer Software Limited
*
* Notes:         
* Private func:  
* History:       Modified from original CLAPACK source code 
*
**************************************************************************/

#include <math.h>
#include <gandalf/linalg/clapack/ilaenv.h>
#include <gandalf/linalg/cblas.h>
#include <gandalf/common/misc_defs.h>
#include <gandalf/common/array.h>
#include <gandalf/common/compare.h>

/* only define this function locally if there is no LAPACK installed */
#if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK)

/* assign strings:  a = b */
static void
 s_copy ( register char *a, register char *b, ftnlen la, ftnlen lb )
{
   register char *aend, *bend;

   aend = a + la;

   if(la <= lb)
      while(a < aend)
         *a++ = *b++;

   else
    {
      bend = b + lb;
      while(b < bend) *a++ = *b++;
      while(a < aend) *a++ = ' ';
    }
}

/* compare two strings */
static long
 s_cmp ( char *a0, char *b0, ftnlen la, ftnlen lb )
{
   register unsigned char *a, *aend, *b, *bend;
   a = (unsigned char *)a0;
   b = (unsigned char *)b0;
   aend = a + la;
   bend = b + lb;

   if(la <= lb)
    {
      while(a < aend)
         if(*a != *b)
            return( *a - *b );
         else
            { ++a; ++b; }

      while(b < bend)
         if(*b != ' ')
            return( ' ' - *b );
         else    ++b;
    }
   else
    {
      while(b < bend)
         if(*a == *b)
            { ++a; ++b; }
         else
            return( *a - *b );
      while(a < aend)
         if(*a != ' ')
            return(*a - ' ');
         else    ++a;
    }

   return 0;
}

long
 gan_ilaenv ( long ispec, char *name, char *opts,
              long n1, long n2, long n3, long n4,
              ftnlen name_len, ftnlen opts_len )
{
   /* Local variables */
   long i;
   Gan_Bool cname, sname;
   long nbmin;
   char c1[1], c2[2], c3[3], c4[2];
   long ic, nb, iz, nx;
   char subnam[6];

   switch (ispec) {
      case 1:  goto L100;
      case 2:  goto L100;
      case 3:  goto L100;
      case 4:  goto L400;
      case 5:  goto L500;
      case 6:  goto L600;
      case 7:  goto L700;
      case 8:  goto L800;
   }

   /* Invalid value for ISPEC */
   return -1;

  L100:

   /* Convert NAME to upper case if the first character is lower case. */
   s_copy(subnam, name, 6L, name_len);
   ic = *(unsigned char *)subnam;
   iz = 'Z';
   if (iz == 90 || iz == 122)
   {
      /* ASCII character set */
      if (ic >= 97 && ic <= 122)
      {
         *(unsigned char *)subnam = (char) (ic - 32);
         for (i = 2; i <= 6; ++i)
         {
            ic = *(unsigned char *)&subnam[i - 1];
            if (ic >= 97 && ic <= 122)
               *(unsigned char *)&subnam[i - 1] = (char) (ic - 32);
         }
      }

   }
   else if (iz == 233 || iz == 169)
   {
      /* EBCDIC character set */
      if ((ic >= 129 && ic <= 137) || (ic >= 145 && ic <= 153) ||
          (ic >= 162 && ic <= 169))
      {
         *(unsigned char *)subnam = (char) (ic + 64);
         for (i = 2; i <= 6; ++i)
         {
            ic = *(unsigned char *)&subnam[i - 1];
            if ((ic >= 129 && ic <= 137) || (ic >= 145 && ic <= 153) ||
                (ic >= 162 && ic <= 169)) {
               *(unsigned char *)&subnam[i - 1] = (char) (ic + 64);
            }
         }
      }
   }
   else if (iz == 218 || iz == 250)
   {
      /* Prime machines:  ASCII+128 */
      if (ic >= 225 && ic <= 250)
      {
         *(unsigned char *)subnam = (char) (ic - 32);
         for (i = 2; i <= 6; ++i)
         {
            ic = *(unsigned char *)&subnam[i - 1];
            if (ic >= 225 && ic <= 250)
               *(unsigned char *)&subnam[i - 1] = (char) (ic - 32);
         }
      }
   }

   *(unsigned char *)c1 = *(unsigned char *)subnam;
   sname = (Gan_Bool)
           (*(unsigned char *)c1 == 'S' || *(unsigned char *)c1 == 'D');
   cname = (Gan_Bool)
           (*(unsigned char *)c1 == 'C' || *(unsigned char *)c1 == 'Z');
   if ( !(cname || sname) ) return 1;

   s_copy(c2, subnam + 1, 2L, 2L);
   s_copy(c3, subnam + 3, 3L, 3L);
   s_copy(c4, c3 + 1, 2L, 2L);

   switch (ispec)
   {
      case 1:  goto L110;
      case 2:  goto L200;
      case 3:  goto L300;
   }

  L110:

   /* ISPEC = 1:  block size   

      In these examples, separate code is provided for setting NB for   
      real and complex.  We assume that NB will take the same value in   
      single or double precision. */
   nb = 1;

   if (s_cmp(c2, "GE", 2L, 2L) == 0)
   {
      if (s_cmp(c3, "TRF", 3L, 3L) == 0)
         nb = 64;
      else if (s_cmp(c3, "QRF", 3L, 3L) == 0 ||
               s_cmp(c3, "RQF", 3L, 3L) == 0 ||
               s_cmp(c3, "LQF", 3L, 3L) == 0 || s_cmp(c3, "QLF", 3L, 3L) == 0)
         nb = 32;
      else if (s_cmp(c3, "HRD", 3L, 3L) == 0)
         nb = 32;
    }
   else if (s_cmp(c3, "BRD", 3L, 3L) == 0)
        nb = 32;
   else if (s_cmp(c3, "TRI", 3L, 3L) == 0)
        nb = 64;
   else if (s_cmp(c2, "PO", 2L, 2L) == 0)
   {
      if (s_cmp(c3, "TRF", 3L, 3L) == 0)
         nb = 64;
    }
   else if (s_cmp(c2, "SY", 2L, 2L) == 0)
   {
      if (s_cmp(c3, "TRF", 3L, 3L) == 0)
         nb = 64;
    }
   else if (sname && s_cmp(c3, "TRD", 3L, 3L) == 0)
        nb = 1;
   else if (sname && s_cmp(c3, "GST", 3L, 3L) == 0)
        nb = 64;
   else if (cname && s_cmp(c2, "HE", 2L, 2L) == 0)
   {
      if (s_cmp(c3, "TRF", 3L, 3L) == 0)
         nb = 64;
      else if (s_cmp(c3, "TRD", 3L, 3L) == 0)
         nb = 1;
      else if (s_cmp(c3, "GST", 3L, 3L) == 0)
         nb = 64;
   }
   else if (sname && s_cmp(c2, "OR", 2L, 2L) == 0)
   {
      if (*(unsigned char *)c3 == 'G')
      {
         if (s_cmp(c4, "QR", 2L, 2L) == 0 || s_cmp(c4, "RQ", 2L, 2L) == 0 ||
             s_cmp(c4, "LQ", 2L, 2L) == 0 || s_cmp(c4, "QL", 2L, 2L) == 0 ||
             s_cmp(c4, "HR", 2L, 2L) == 0 || s_cmp(c4, "TR", 2L, 2L) == 0 ||
             s_cmp(c4, "BR", 2L, 2L) == 0)
            nb = 32;
      }
      else if (*(unsigned char *)c3 == 'M')
      {
         if (s_cmp(c4, "QR", 2L, 2L) == 0 || s_cmp(c4, "RQ", 2L, 2L) == 0 ||
             s_cmp(c4, "LQ", 2L, 2L) == 0 || s_cmp(c4, "QL", 2L, 2L) == 0 ||
             s_cmp(c4, "HR", 2L, 2L) == 0 || s_cmp(c4, "TR", 2L, 2L) == 0 ||
             s_cmp(c4, "BR", 2L, 2L) == 0)
            nb = 32;
      }
   }
   else if (cname && s_cmp(c2, "UN", 2L, 2L) == 0)
   {
      if (*(unsigned char *)c3 == 'G')
      {
         if (s_cmp(c4, "QR", 2L, 2L) == 0 || s_cmp(c4, "RQ", 2L, 2L) == 0 ||
             s_cmp(c4, "LQ", 2L, 2L) == 0 || s_cmp(c4, "QL", 2L, 2L) == 0 ||
             s_cmp(c4, "HR", 2L, 2L) == 0 || s_cmp(c4, "TR", 2L, 2L) == 0 ||
             s_cmp(c4, "BR", 2L, 2L) == 0)
            nb = 32;
      }
      else if (*(unsigned char *)c3 == 'M')
      {
         if (s_cmp(c4, "QR", 2L, 2L) == 0 || s_cmp(c4, "RQ", 2L, 2L) == 0 ||
             s_cmp(c4, "LQ", 2L, 2L) == 0 || s_cmp(c4, "QL", 2L, 2L) == 0 ||
             s_cmp(c4, "HR", 2L, 2L) == 0 || s_cmp(c4, "TR", 2L, 2L) == 0 ||
             s_cmp(c4, "BR", 2L, 2L) == 0)
            nb = 32;
      }
   }
   else if (s_cmp(c2, "GB", 2L, 2L) == 0)
   {
      if (s_cmp(c3, "TRF", 3L, 3L) == 0)
      {
         if (sname)
         {
            if (n4 <= 64) nb = 1;
            else nb = 32;
         }
         else
         {
            if (n4 <= 64) nb = 1;
            else nb = 32;
         }
      }
   }
   else if (s_cmp(c2, "PB", 2L, 2L) == 0)
   {
      if (s_cmp(c3, "TRF", 3L, 3L) == 0)
      {
         if (sname)
         {
            if (n2 <= 64) nb = 1;
            else nb = 32;
         }
         else
         {
            if (n2 <= 64) nb = 1;
            else nb = 32;
         }
      }
   }
   else if (s_cmp(c2, "TR", 2L, 2L) == 0)
   {
      if (s_cmp(c3, "TRI", 3L, 3L) == 0)
      {
            nb = 64;
      }
   }
   else if (s_cmp(c2, "LA", 2L, 2L) == 0)
   {
      if (s_cmp(c3, "UUM", 3L, 3L) == 0)
      {
         if (sname)
            nb = 64;
      }
    }
   else if (sname && s_cmp(c2, "ST", 2L, 2L) == 0)
   {
      if (s_cmp(c3, "EBZ", 3L, 3L) == 0)
         nb = 1;
   }

   return nb;

L200:

   /* ISPEC = 2:  minimum block size */
   nbmin = 2;
   if (s_cmp(c2, "GE", 2L, 2L) == 0)
   {
      if (s_cmp(c3, "QRF", 3L, 3L) == 0 || s_cmp(c3, "RQF", 3L, 3L) == 0 || 
          s_cmp(c3, "LQF", 3L, 3L) == 0 || s_cmp(c3, "QLF", 3L, 3L) == 0)
         nbmin = 2;
      else if (s_cmp(c3, "HRD", 3L, 3L) == 0)
         nbmin = 2;
      else if (s_cmp(c3, "BRD", 3L, 3L) == 0)
         nbmin = 2;
      else if (s_cmp(c3, "TRI", 3L, 3L) == 0)
      {
         nbmin = 2;
      }
   }
   else if (s_cmp(c2, "SY", 2L, 2L) == 0)
   {
      if (s_cmp(c3, "TRF", 3L, 3L) == 0)
         nbmin = 8;
      else if (sname && s_cmp(c3, "TRD", 3L, 3L) == 0)
         nbmin = 2;
   }
   else if (cname && s_cmp(c2, "HE", 2L, 2L) == 0)
   {
      if (s_cmp(c3, "TRD", 3L, 3L) == 0)
         nbmin = 2;
   }
   else if (sname && s_cmp(c2, "OR", 2L, 2L) == 0)
   {
      if (*(unsigned char *)c3 == 'G')
      {
         if (s_cmp(c4, "QR", 2L, 2L) == 0 || s_cmp(c4, "RQ", 2L, 2L) == 0 ||
             s_cmp(c4, "LQ", 2L, 2L) == 0 || s_cmp(c4, "QL", 2L, 2L) == 0 ||
             s_cmp(c4, "HR", 2L, 2L) == 0 || s_cmp(c4, "TR", 2L, 2L) == 0 ||
             s_cmp(c4, "BR", 2L, 2L) == 0)
            nbmin = 2;
      }
      else if (*(unsigned char *)c3 == 'M')
      {
         if (s_cmp(c4, "QR", 2L, 2L) == 0 || s_cmp(c4, "RQ", 2L, 2L) == 0 ||
             s_cmp(c4, "LQ", 2L, 2L) == 0 || s_cmp(c4, "QL", 2L, 2L) == 0 ||
             s_cmp(c4, "HR", 2L, 2L) == 0 || s_cmp(c4, "TR", 2L, 2L) == 0 ||
             s_cmp(c4, "BR", 2L, 2L) == 0)
            nbmin = 2;
      }
   }
   else if (cname && s_cmp(c2, "UN", 2L, 2L) == 0)
   {
      if (*(unsigned char *)c3 == 'G')
      {
         if (s_cmp(c4, "QR", 2L, 2L) == 0 || s_cmp(c4, "RQ", 2L, 2L) == 0 ||
             s_cmp(c4, "LQ", 2L, 2L) == 0 || s_cmp(c4, "QL", 2L, 2L) == 0 ||
             s_cmp(c4, "HR", 2L, 2L) == 0 || s_cmp(c4, "TR", 2L, 2L) == 0 ||
             s_cmp(c4, "BR", 2L, 2L) == 0)
            nbmin = 2;
      }
      else if (*(unsigned char *)c3 == 'M')
      {
         if (s_cmp(c4, "QR", 2L, 2L) == 0 || s_cmp(c4, "RQ", 2L, 2L) == 0 ||
             s_cmp(c4, "LQ", 2L, 2L) == 0 || s_cmp(c4, "QL", 2L, 2L) == 0 ||
             s_cmp(c4, "HR", 2L, 2L) == 0 || s_cmp(c4, "TR", 2L, 2L) == 0 ||
             s_cmp(c4, "BR", 2L, 2L) == 0)
            nbmin = 2;
      }
   }

   return nbmin;

L300:

   /* ISPEC = 3:  crossover point */
   nx = 0;
   if (s_cmp(c2, "GE", 2L, 2L) == 0)
   {
      if (s_cmp(c3, "QRF", 3L, 3L) == 0 || s_cmp(c3, "RQF", 3L, 3L) == 0 || 
          s_cmp(c3, "LQF", 3L, 3L) == 0 || s_cmp(c3, "QLF", 3L, 3L) == 0)
         nx = 128;
      else if (s_cmp(c3, "HRD", 3L, 3L) == 0)
         nx = 128;
      else if (s_cmp(c3, "BRD", 3L, 3L) == 0)
         nx = 128;
   }
   else if (s_cmp(c2, "SY", 2L, 2L) == 0)
   {
      if (sname && s_cmp(c3, "TRD", 3L, 3L) == 0)
         nx = 1;
   }
   else if (cname && s_cmp(c2, "HE", 2L, 2L) == 0)
   {
      if (s_cmp(c3, "TRD", 3L, 3L) == 0)
         nx = 1;
   }
   else if (sname && s_cmp(c2, "OR", 2L, 2L) == 0)
   {
      if (*(unsigned char *)c3 == 'G')
      {
         if (s_cmp(c4, "QR", 2L, 2L) == 0 || s_cmp(c4, "RQ", 2L, 2L) == 0 ||
             s_cmp(c4, "LQ", 2L, 2L) == 0 || s_cmp(c4, "QL", 2L, 2L) == 0 ||
             s_cmp(c4, "HR", 2L, 2L) == 0 || s_cmp(c4, "TR", 2L, 2L) == 0 ||
             s_cmp(c4, "BR", 2L, 2L) == 0)
            nx = 128;
      }
   }
   else if (cname && s_cmp(c2, "UN", 2L, 2L) == 0)
   {
      if (*(unsigned char *)c3 == 'G')
      {
         if (s_cmp(c4, "QR", 2L, 2L) == 0 || s_cmp(c4, "RQ", 2L, 2L) == 0 ||
             s_cmp(c4, "LQ", 2L, 2L) == 0 || s_cmp(c4, "QL", 2L, 2L) == 0 ||
             s_cmp(c4, "HR", 2L, 2L) == 0 || s_cmp(c4, "TR", 2L, 2L) == 0 ||
             s_cmp(c4, "BR", 2L, 2L) == 0)
            nx = 128;
      }
   }

   return nx;

  L400:

   /* ISPEC = 4:  number of shifts (used by xHSEQR) */
   return 6;

L500:

   /* ISPEC = 5:  minimum column dimension (not used) */
   return 2;

L600:

   /* ISPEC = 6:  crossover point for SVD (used by xGELSS and xGESVD) */
   return (long) ((float) gan_min2(n1,n2) * 1.6f);

L700:

   /* ISPEC = 7:  number of processors (not used) */
   return 1;

L800:

   /* ISPEC = 8:  crossover point for multishift (used by xHSEQR) */
   return 50;
}

#endif /* #if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK) */

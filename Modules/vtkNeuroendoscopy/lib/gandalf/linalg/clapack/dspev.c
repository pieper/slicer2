/**************************************************************************
*
* File:          $RCSfile: dspev.c,v $
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
#include <gandalf/linalg/clapack/dspev.h>
#include <gandalf/linalg/clapack/dsptrd.h>
#include <gandalf/linalg/clapack/dlamch.h>
#include <gandalf/linalg/clapack/dlansp.h>
#include <gandalf/linalg/clapack/dlascl.h>
#include <gandalf/linalg/clapack/dsterf.h>
#include <gandalf/linalg/clapack/dopgtr.h>
#include <gandalf/linalg/clapack/dsteqr.h>
#include <gandalf/linalg/cblas/dscal.h>
#include <gandalf/common/misc_defs.h>
#include <gandalf/common/array.h>
#include <gandalf/common/compare.h>

/* only define this function locally if there is no LAPACK installed */
#if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK)

/* modified from CLAPACK source */

Gan_Bool gan_dspev(Gan_LapackEvecType jobz, Gan_LapackUpLoFlag upper,
                   long n, double *ap, double *w, double *z, long ldz,
                   double *work, long *info)
{
   double anrm;
   double rmin, rmax;
   double sigma=0.0;
   long iinfo;
   long iscale;
   double safmin;
   double bignum;
   long indtau;
   long indwrk;
   double smlnum, eps;
   Gan_Bool wantz = (jobz == GAN_EVECTYPE_EVECSORIGINAL);

   /* Function Body */
   *info = 0;
   if (n < 0)
      *info = -3;
   else if (ldz < 1 || (wantz && ldz < n))
      *info = -7;

   if (*info != 0)
      return GAN_FALSE;

   /* Quick return if possible */
   if (n == 0)
      return GAN_TRUE;

   if (n == 1)
   {
      w[0] = ap[0];
      if (wantz)
         z[0] = 1.0;

      return GAN_TRUE;
   }

   /* Get machine constants. */
   safmin = gan_dlamch(GAN_LAMCH_SFMIN);
   eps = gan_dlamch(GAN_LAMCH_PREC);
   smlnum = safmin/eps;
   bignum = 1.0/smlnum;
   rmin = sqrt(smlnum);
   rmax = sqrt(bignum);

   /*     Scale matrix to allowable range, if necessary. */
   anrm = gan_dlansp(GAN_NORMTYPE_MAXABSVAL, upper, n, ap, work);
   iscale = 0;
   if (anrm > 0.0 && anrm < rmin)
   {
      iscale = 1;
      sigma = rmin / anrm;
   }
   else if (anrm > rmax)
   {
      iscale = 1;
      sigma = rmax / anrm;
   }

   if (iscale == 1)
      gan_dscal(n*(n+1)/2, sigma, ap, 1);

   /* Call DSPTRD to reduce symmetric packed matrix to tridiagonal form. */
   indtau = n;
   gan_dsptrd(upper, n, ap, w, work, &work[indtau], &iinfo);

   /* For eigenvalues only, call DSTERF.  For eigenvectors, first call   
      DOPGTR to generate the orthogonal matrix, then call DSTEQR. */
   if (!wantz)
      gan_dsterf(n, w, work, info);
   else
   {
      indwrk = indtau + n;
      gan_dopgtr(upper, n, ap, &work[indtau], z, ldz, &work[indwrk], &iinfo);
      gan_dsteqr(wantz, n, w, work, z, ldz, &work[indtau], info);
   }

   /* If matrix was scaled, then rescale eigenvalues appropriately. */
   if (iscale == 1)
   {
      long imax;

      if (*info == 0)
         imax = n;
      else
         imax = *info - 1;

      gan_dscal(imax, 1.0, w, 1);
   }

   return GAN_TRUE;
}

#endif /* #if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK) */

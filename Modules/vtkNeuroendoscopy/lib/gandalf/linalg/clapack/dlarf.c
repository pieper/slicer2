/**************************************************************************
*
* File:          $RCSfile: dlarf.c,v $
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
#include <gandalf/linalg/clapack/dlarf.h>
#include <gandalf/linalg/cblas.h>
#include <gandalf/common/misc_defs.h>
#include <gandalf/common/array.h>
#include <gandalf/common/compare.h>

/* only define this function locally if there is no LAPACK installed */
#if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK)

/* modified from CLAPACK source */
Gan_Bool
 gan_dlarf ( Gan_LapackSideFlag side, long m, long n, double *v, long incv,
             double tau, double *c, long ldc, double *work )
{
   if ( side == GAN_LEFTMULT )
   {
      /*        Form  H * C */
      if ( tau != 0.0 )
      {
         /*           w := C' * v */
         gan_fill_array_d ( work, n, 1, 0.0 );
         gan_dgemv ( GAN_TRANSPOSE, m, n, 1.0, c, ldc, v, incv, 0.0, work, 1);

         /*           C := C - v * w' */
         gan_dger ( m, n, -tau, v, incv, work, 1, c, ldc );
      }
   }
   else
   {
      /*        Form  C * H */
      if ( tau != 0.0)
      {
         /*           w := C * v */
         gan_dgemv ( GAN_NOTRANSPOSE, m, n, 1.0, c, ldc, v, incv, 0.0,
                     work, 1 );

         /*           C := C - w * v' */
         gan_dger ( m, n, -tau, work, 1, v, incv, c, ldc );
      }
   }

   /* success */
   return GAN_TRUE;
}

#endif /* #if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK) */

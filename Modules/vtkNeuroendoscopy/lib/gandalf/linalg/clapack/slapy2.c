/**************************************************************************
*
* File:          $RCSfile: slapy2.c,v $
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
#include <gandalf/linalg/clapack/slapy2.h>
#include <gandalf/linalg/cblas.h>
#include <gandalf/common/misc_defs.h>
#include <gandalf/common/array.h>
#include <gandalf/common/compare.h>

/* only define this function locally if there is no LAPACK installed */
#if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK)

/* modified from CLAPACK source */
float
 gan_slapy2 ( float x, float y )
{
   float ret_val, d, xabs, yabs, w, z;

   xabs = (float)fabs(x);
   yabs = (float)fabs(y);
   w = gan_max2(xabs,yabs);
   z = gan_min2(xabs,yabs);
   if ( z == 0.0F )
      ret_val = w;
   else
   {
      /* Computing 2nd power */
      d = z/w;
      ret_val = w * (float)sqrt(d*d + 1.0F);
   }

   return ret_val;
}

#endif /* #if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK) */

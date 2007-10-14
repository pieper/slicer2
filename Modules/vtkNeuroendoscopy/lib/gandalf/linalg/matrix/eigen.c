/*  eigen.c    CCMATH mathematics library source code.
 *
 *  Copyright (C)  2000   Daniel A. Atkinson    All rights reserved.
 *  This code may be redistributed under the terms of the GNU library
 *  public license (LGPL). ( See the lgpl.license file for details.)
 * ------------------------------------------------------------------------
 */

/* names of functions changed to avoid possible clashes:
 * Phil McLauchlan 14/3/2001
 */
   
#include <stdlib.h>
#include "matutl.h"

/* line added by PFM 12/3/01 */
void ccm_housev(double *a,double *d,double *dp,int n);

void ccm_eigen(double *a,double *ev,int n)
{ double *dp;
  dp=(double *)calloc(n,sizeof(double));
  ccm_housev(a,ev,dp,n);
  ccm_qrevec(ev,a,dp,n); ccm_trnm(a,n);
  free(dp);
}

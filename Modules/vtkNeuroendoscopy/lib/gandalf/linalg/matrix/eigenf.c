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
#include "matutlf.h"

/* line added by PFM 12/3/01 */
void ccmf_housev(float *a,float *d,float *dp,int n);

void ccmf_eigen(float *a,float *ev,int n)
{ float *dp;
  dp=(float *)calloc(n,sizeof(float));
  ccmf_housev(a,ev,dp,n);
  ccmf_qrevec(ev,a,dp,n); ccmf_trnm(a,n);
  free(dp);
}

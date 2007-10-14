/*  eigval.c    CCMATH mathematics library source code.
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
void ccm_eigval(double *a,double *ev,int n)
{ double *dp;
  dp=(double *)calloc(n,sizeof(double));
  ccm_house(a,ev,dp,n);
  ccm_qreval(ev,dp,n);
  free(dp);
}

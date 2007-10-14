/*  qrevec.c    CCMATH mathematics library source code.
 *
 *  Copyright (C)  2000   Daniel A. Atkinson    All rights reserved.
 *  This code may be redistributed under the terms of the GNU library
 *  public license (LGPL). ( See the lgpl.license file for details.)
 * ------------------------------------------------------------------------
 */

/* names of functions changed to avoid possible clashes:
 * Phil McLauchlan 14/3/2001
 */
   
#include <math.h>
int ccmf_qrevec(float *ev,float *evec,float *dp,int n)
{ float cc,sc,d,x,y,h,tzr=1.e-15F;
  int i,j,k,m,mqr=8*n;
  float *p;
  for(j=0,m=n-1;;++j){
    while(1){ if(m<1) return 0; k=m-1;
      if(fabs(dp[k])<=fabs(ev[m])*tzr) --m;
      else{ x=(ev[k]-ev[m])/2.0F; h=(float)sqrt(x*x+dp[k]*dp[k]);
        if(m>1 && fabs(dp[m-2])>fabs(ev[k])*tzr) break;
        if((cc=(float)sqrt((1.0F+x/h)/2.0F))!=0.0F) sc=dp[k]/(2.0F*cc*h); else sc=1.0F;
        x+=ev[m]; ev[m--]=x-h; ev[m--]=x+h;
        for(i=0,p=evec+n*(m+1); i<n ;++i,++p){
          h=p[0]; p[0]=cc*h+sc*p[n]; p[n]=cc*p[n]-sc*h;
         }
       }
     }
    if(j>mqr) return -1;
    if(x>0.0F) d=ev[m]+x-h; else d=ev[m]+x+h;
    cc=1.0F; y=0.0F; ev[0]-=d;
    for(k=0; k<m ;++k){
      x=ev[k]*cc-y; y=dp[k]*cc; h=(float)sqrt(x*x+dp[k]*dp[k]);
      if(k>0) dp[k-1]=sc*h;
      ev[k]=cc*h; cc=x/h; sc=dp[k]/h; ev[k+1]-=d; y*=sc;
      ev[k]=cc*(ev[k]+y)+ev[k+1]*sc*sc+d;
      for(i=0,p=evec+n*k; i<n ;++i,++p){
        h=p[0]; p[0]=cc*h+sc*p[n]; p[n]=cc*p[n]-sc*h;
       }
     }
    ev[k]=ev[k]*cc-y; dp[k-1]=ev[k]*sc; ev[k]=ev[k]*cc+d;
   }
  return 0;
}

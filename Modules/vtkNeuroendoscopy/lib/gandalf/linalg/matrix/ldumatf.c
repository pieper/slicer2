/*  ldumat.c    CCMATH mathematics library source code.
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
void ccmf_ldumat(float *a,float *u,int m,int n)
{ float *p0,*q0,*p,*q,*w;
  int i,j,k,mm;
  float s,h;
  w=(float *)calloc(m,sizeof(float));
  for(i=0,mm=m*m,q=u; i<mm ;++i) *q++ =0.0F;
  p0=a+n*n-1; q0=u+m*m-1; mm=m-n; i=n-1;
  for(j=0; j<mm ;++j,q0-=m+1) *q0=1.0F;
  if(mm==0){ p0-=n+1; *q0=1.0F; q0-=m+1; --i; ++mm;}
  for(; i>=0 ;--i,++mm,p0-=n+1,q0-=m+1){
    if(*p0!=0.0F){
      for(j=0,p=p0+n,h=1.0F; j<mm ;p+=n) w[j++]= *p;
      h= *p0; *q0=1.0F-h;
      for(j=0,q=q0+m; j<mm ;q+=m) *q= -h*w[j++];
      for(k=i+1,q=q0+1; k<m ;++k){
        for(j=0,p=q+m,s=0.0F; j<mm ;p+=m) s+=w[j++]* *p;
        s*=h;
        for(j=0,p=q+m; j<mm ;p+=m) *p-=s*w[j++];
        *q++ = -s;
       }
     }
    else{
      *q0=1.0F;
      for(j=0,p=q0+1,q=q0+m; j<mm ;++j,q+=m) *q= *p++ =0.0F;
     }
   }
  free(w);
}

/*  qrbdv.c    CCMATH mathematics library source code.
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
int ccmf_qrbdv(float *dm,float *em,float *um,int mm,float *vm,int m)
{ int i,j,k,n,jj,nm;
  float u,x,y,a,b,c,s,t,w,*p,*q;
  for(j=1,t=(float)fabs(dm[0]); j<m ;++j)
    if((s=(float)(fabs(dm[j])+fabs(em[j-1])))>t) t=s;
  t*=1.0e-15F; n=100*m; nm=m;
  for(j=0; m>1 && j<n ;++j){
    for(k=m-1; k>0 ;--k){
      if((float)fabs(em[k-1])<t) break;
      if((float)fabs(dm[k-1])<t){
        for(i=k,s=1.0F,c=0.0F; i<m ;++i){
          a=s*em[i-1]; b=dm[i]; em[i-1]*=c;
          dm[i]=u=(float)sqrt(a*a+b*b); s= -a/u; c=b/u;
          for(jj=0,p=um+k-1; jj<mm ;++jj,p+=mm){
            q=p+i-k+1;
            w=c* *p+s* *q; *q=c* *q-s* *p; *p=w;
           }
             }
        break;
       }
     }
    y=dm[k]; x=dm[m-1]; u=em[m-2];
    a=(y+x)*(y-x)-u*u; s=y*em[k]; b=s+s;
    u=(float)sqrt(a*a+b*b);
        if(u!=0.0F){
      c=(float)sqrt((u+a)/(u+u)); s/=(c*u);
          for(i=k; i<m-1 ;++i){
        b=em[i];
        if(i>k){
          a=s*em[i]; b*=c;
              em[i-1]=u=(float)sqrt(x*x+a*a);
              c=x/u; s=a/u;
         }
        a=c*y+s*b; b=c*b-s*y;
        for(jj=0,p=vm+i; jj<nm ;++jj,p+=nm){
          w=c* *p+s* *(p+1); *(p+1)=c* *(p+1)-s* *p; *p=w;
         }
        s*=dm[i+1]; dm[i]=u=(float)sqrt(a*a+s*s);
        y=c*dm[i+1]; c=a/u; s/=u;
        x=c*b+s*y; y=c*y-s*b;
        for(jj=0,p=um+i; jj<mm ;++jj,p+=mm){
          w=c* *p+s* *(p+1); *(p+1)=c* *(p+1)-s* *p; *p=w;
         }
           }
     }
    em[m-2]=x; dm[m-1]=y;
    if((float)fabs(x)<t) --m;
    if(m==k+1) --m; 
   }
  return j;
}

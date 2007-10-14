/*  housev.c    CCMATH mathematics library source code.
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
#include <math.h>
void ccmf_housev(float *a,float *d,float *dp,int n)
{ float sc,x,y,h;
  int i,j,k,m,e;
  float *qw,*qs,*pc,*p;
  qs=(float *)calloc(n,sizeof(float));
  for(j=0,pc=a; j<n-2 ;++j,pc+=n+1){
    m=n-j-1;
    for(i=1,sc=0.0F; i<=m ;++i) sc+=pc[i]*pc[i];
    if(sc>0.0F){ sc=(float)sqrt(sc);
      if((x= *(pc+1))<0.0F){ y=x-sc; h=1.0F/(float)sqrt(-2.0F*sc*y);}
      else{ y=x+sc; h=1.0F/(float)sqrt(2.0F*sc*y); sc= -sc;}
      for(i=0,qw=pc+1; i<m ;++i){
        qs[i]=0.0F; if(i) qw[i]*=h; else qw[i]=y*h;
       }
      for(i=0,e=j+2,p=pc+n+1,h=0.0F; i<m ;++i,p+=e++){
        qs[i]+=(y=qw[i])* *p++;
        for(k=i+1; k<m ;++k){
          qs[i]+=qw[k]* *p; qs[k]+=y* *p++;
         }
        h+=y*qs[i];
       }
      for(i=0; i<m ;++i){
        qs[i]-=h*qw[i]; qs[i]+=qs[i];
       }
      for(i=0,e=j+2,p=pc+n+1; i<m ;++i,p+=e++){
        for(k=i; k<m ;++k) *p++ -=qw[i]*qs[k]+qs[i]*qw[k];
       }
     }
    d[j]= *pc; dp[j]=sc;
   }
  d[j]= *pc; dp[j]= *(pc+1); d[j+1]= *(pc+=n+1);
  free(qs);
  for(i=0,m=n+n,p=pc; i<m ;++i) *p-- =0.0F;
  *pc=1.0F; *(pc-=n+1)=1.0F; qw=pc-n;
  for(m=2; m<n ;++m,qw-=n+1){
    for(j=0,p=pc,*pc=1.0F; j<m ;++j,p+=n){
      for(i=0,qs=p,h=0.0F; i<m ;) h+=qw[i++]* *qs++;
      for(i=0,qs=p,h+=h; i<m ;) *qs++ -=h*qw[i++];
     }
    for(i=0,p=qw+m; i<n ;++i) *(--p)=0.0F;
    *(pc-=n+1)=1.0F;
   }
}

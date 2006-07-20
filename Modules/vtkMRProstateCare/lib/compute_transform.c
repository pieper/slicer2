#include "bio_coords.h"

void crossvec(double *b, double *c, double *a) /* a=bxc */
{
    a[0] = b[1]*c[2] - c[1]*b[2];
    a[1] = c[0]*b[2] - b[0]*c[2];
    a[2] = b[0]*c[1] - c[0]*b[1];
}

void normalizevec(double *a) /* make a unit length */
{
    double d;
    d= sqrt(a[0]*a[0] + a[1]*a[1] + a[2]*a[2]);
    if (d == 0.0) return;
    a[0] = a[0] / d;
    a[1] = a[1] / d;
    a[2] = a[2] / d;
}


void meanvec(double *in, double * out, int M, int N)
{
    int i,j;
    for(i=0;i<M;i++)
    {
        out[i]=0.0;
        for(j=0;j<N;j++) out[i]+=in[j*M+i];
        out[i]/=(double)N;
    }
}

void compute_transform(bio_struct *bs)
{
    double T[16]={80,0,70,1,   80,0,0,1,   0,0,0,1,   0,0,70,1};
    double CRD[16]={0,14,0,1,  0,0,0,1,  16,0,0,1,  0,14,1,1};
    double meanT[4],centcrn[16],meanCrn[4],centT[16],CT[16];
    double *a[4],*b[4],*c[4],tmp,nvec[4],nve2[4],nve3[4];
    double RSA[16],tmp1[16];
    int i,j,k;
    HMatrix M,Q,S;

    HMatrixAlloc(&M);
    HMatrixAlloc(&Q);
    HMatrixAlloc(&S);

    for(i=0;i<4;i++)  
        for(j=0;j<4;j++)
            M[i][j]=Q[i][j]=S[i][j]=0.0;
    i=j=3;
    M[i][j]=Q[i][j]=S[i][j]=1.0;

    meanvec(T,meanT,4,4); meanT[3]=0;
    for (j=0;j<4;j++) for(i=0;i<4;i++) centT[i+j*4]=T[i+j*4]-meanT[i];

    meanvec(bs->corners,meanCrn,4,4); meanCrn[3]=0;
    for (j=0;j<4;j++) for(i=0;i<4;i++) 
        centcrn[i+j*4]=(bs->corners)[i+j*4]-meanCrn[i];

    for(i=0;i<16;i++) CT[i]=0; CT[15]=1;
    a[0]=centcrn; b[0]=centT; c[0]=CT;
    for(i=1;i<4;i++) {a[i]=a[i-1]+4; b[i]=b[i-1]+4; c[i]=c[i-1]+4;}
    for(i=0;i<3;i++)
        for(j=0;j<3;j++)
            for(k=0;k<4;k++)
                c[i][j]+= a[k][i]*b[k][j]; /* centcrn'*centT */
    for(i=0;i<4;i++) for(j=0;j<4;j++) M[i][j]=c[i][j];

    polar_decomp(M,Q,S);

    for(i=0;i<16;i++) (bs->cornersT)[i]=0;
    a[0]=centT; c[0]=bs->cornersT;
    for(i=1;i<4;i++) {a[i]=a[i-1]+4; c[i]=c[i-1]+4;}
    for(i=0;i<4;i++)
        for(j=0;j<3;j++)
            for(k=0;k<3;k++)
                c[i][j]+= a[i][k]*Q[j][k]; /* centT * Q' */
    for(i=0;i<4;i++) c[i][3]=1;

    a[0]=bs->corners; b[0]=centcrn;
    for(i=1;i<4;i++) {a[i]=a[i-1]+4; b[i]=b[i-1]+4;}
    for(i=0;i<4;i++) for(j=0;j<3;j++) c[i][j]+=a[i][j]-b[i][j];
    for(i=0;i<4;i++) c[i][3]=1;

    for(i=0;i<16;i++) (bs->corn_err)[i]=(bs->cornersT)[i]-(bs->corners)[i];
    c[0]=bs->corn_err;
    for(i=1;i<4;i++) c[i]=c[i-1]+4;
    for(i=0;i<4;i++)
    {
        tmp=0.0;    
        for(j=0;j<3;j++) tmp+=c[i][j]*c[i][j];
        c[i][3]=tmp;
    }

    for(i=0;i<16;i++) RSA[i]=(bs->cornersT)[i];
    for(i=0;i<3;i++) nvec[i]=(bs->cornersT)[i+4]-(bs->cornersT)[i];
    for(i=0;i<3;i++) nve2[i]=(bs->cornersT)[i+8]-(bs->cornersT)[i+4];
    crossvec(nve2,nvec,nve3); nve3[3]=0;
    normalizevec(nve3);
    if (nve3[1]<0) for(i=0;i<3;i++) nve3[i]=-nve3[i]; /* point is S dir */
    for(i=0;i<3;i++) nve3[i]*=10;  /* Centemeter units for depth */
    for(i=0;i<3;i++) RSA[i+12]=(bs->cornersT)[i]+nve3[i];
    RSA[3]=RSA[7]=RSA[11]=RSA[15]=1;

    MatInv(RSA,tmp1);
    MatMult(tmp1,CRD,bs->RSAtoCRD);

    MatInv(bs->RSAtoCRD,tmp1);
    (bs->RSANormalToTemplate)[0]=tmp1[8];
    (bs->RSANormalToTemplate)[1]=tmp1[9];
    (bs->RSANormalToTemplate)[2]=tmp1[10];
    normalizevec(bs->RSANormalToTemplate);

}

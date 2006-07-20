#include "Mat.h"

void MatScale(double *mat,  double c)
{
    int i;
    for(i=16;i--;) mat[i]*=c;
}


void MatCopy( double *in, double *out)
{
    int i;
    for(i=16;i--;) out[i]=in[i];
}

void MatMult( double *in1,  double * in2, double * out)
{
    double p[16],*In1[4],*In2[4];
    int i,j,k,w=0;
    for(i=0;i<4;i++)
    {
        In1[i]=in1+(i<<2);
        In2[i]=in2+(i<<2);
    }
    for(i=0;i<4;i++)
        for(j=0;j<4;j++)
        {
            p[w]=0.0;
            for(k=0;k<4;k++) p[w]+=In1[i][k]*In2[k][j];
            w++;
        }
    for(i=16;i--;) out[i]=p[i];
}

void MatEye(double * mat)
{
    int i;
    mat[0]=mat[5]=mat[10]=mat[15]=1.0;
    for(i=1;i<5;i++)
        mat[i]=mat[5+i]=mat[10+i]=0.0;
}

double MatNorm2( double * mat)
{
    int i;
    double r=0.0;
    for(i=16;i--;) r+=mat[i]*mat[i];
    return(r);
}

void MatDot( double *in1,  double * in2, double * out)
{
    int i;
    for(i=16;i--;) out[i]=in1[i]*in2[i];
}

void MatDiv( double *in1,  double * in2, double * out)
{
    int i;
    for(i=16;i--;) 
        out[i]=(in2[i]==0.0)?(0.0):(in1[i]/in2[i]);
}

void MatDiff( double *in1,  double * in2, double * out)
{
    int i;
    for(i=16;i--;) out[i]=in1[i]-in2[i];
}

void MatAdd( double *in1,  double * in2, double * out)
{
    int i;
    for(i=16;i--;) out[i]=in1[i]+in2[i];
}

void MatConst(double *mat,  double c)
{
    int i;
    for(i=16;i--;) mat[i]=c;
}

void MatTrans(double *in, double *out)
{
    double r;
    r=in[ 4]; out[ 4]=in[ 1]; out[ 1]=r;
    r=in[ 8]; out[ 8]=in[ 2]; out[ 2]=r;
    r=in[ 9]; out[ 9]=in[ 6]; out[ 6]=r;
    r=in[12]; out[12]=in[ 3]; out[ 3]=r;
    r=in[13]; out[13]=in[ 7]; out[ 7]=r;
    r=in[14]; out[14]=in[11]; out[11]=r;
}

double MatSubDet(double * mat, int * rows, int * cols)
{
    double * Mat[4];
    Mat[0]=mat; Mat[1]=mat+4;
    Mat[2]=mat+8; Mat[3]=mat+12;
    return(Mat[rows[0]][cols[0]]*(Mat[rows[1]][cols[1]]*Mat[rows[2]][cols[2]]
                -Mat[rows[2]][cols[1]]*Mat[rows[1]][cols[2]]) 
            +Mat[rows[0]][cols[1]]*(Mat[rows[1]][cols[2]]*Mat[rows[2]][cols[0]]
                -Mat[rows[2]][cols[2]]*Mat[rows[1]][cols[0]])
            +Mat[rows[0]][cols[2]]*(Mat[rows[1]][cols[0]]*Mat[rows[2]][cols[1]]
                -Mat[rows[2]][cols[0]]*Mat[rows[1]][cols[1]]));
}


double MatDet(double * mat)
{
    int cols[3]={0,1,2};
    int rows[3]={0,1,2};
    double d=0.0;
    if (mat[15]!=0.0) d= mat[15]*MatSubDet(mat,rows,cols);
    cols[2]=3;
    if (mat[14]!=0.0) d-=mat[14]*MatSubDet(mat,rows,cols);
    cols[1]=2;
    if (mat[13]!=0.0) d+=mat[13]*MatSubDet(mat,rows,cols);
    cols[0]=1;
    if (mat[12]!=0.0) d-=mat[12]*MatSubDet(mat,rows,cols);
    return(d);
}


void MatMinor(double * mat, double *min)
{
    int i,j,k=0;
    int rc[4][3]={{1,2,3},{0,2,3},{0,1,3},{0,1,2}};
    for(i=0;i<4;i++)
        for(j=0;j<4;j++)
            min[k++]=MatSubDet(mat,rc[i],rc[j]);
}


void MatCofact(double * mat, double *cof)
{
    MatMinor(mat,cof);
    cof[ 1]=-cof[ 1]; cof[ 3]=-cof[ 3];
    cof[ 4]=-cof[ 4]; cof[ 6]=-cof[ 6];
    cof[ 9]=-cof[ 9]; cof[11]=-cof[11];
    cof[12]=-cof[12]; cof[14]=-cof[14];
}

void MatAdjCof(double * mat, double *adj)
{
    MatCofact(mat,adj);
    MatTrans(adj,adj);
}


double MatInv(double * mat, double *inv)
{
    double det;
    int i;
    MatAdjCof(mat,inv);
    det=mat[3]*inv[12]+mat[7]*inv[13]+
        mat[11]*inv[14]+mat[15]*inv[15];
    if (det!=0.0) for(i=16;i--;) inv[i]/=det;
    else for(i=16;i--;) inv[i]=0.0;
    return(det);
}





/* (example)
   int main(void)
   {
   double mat[16]={9,9,4,9,1,4,8,0,1,2,2,6,3,2,0,7};
   double inv[16],det,out[16];
   int i;

   det=MatInv(mat,inv);
   printf("%f\n\n",det);

   for (i=0;i<16;i+=4)
   printf("%9f %9f %9f %9f\n",inv[0+i],inv[1+i],inv[2+i],inv[3+i]);

   MatMult(mat,inv,out);
   printf("\n");
   for (i=0;i<16;i+=4)
   printf("%9f %9f %9f %9f\n",out[0+i],out[1+i],out[2+i],out[3+i]);

   printf("\n");
   MatCofact(mat,inv);
   for (i=0;i<16;i+=4)
   printf("%9f %9f %9f %9f\n",inv[0+i],inv[1+i],inv[2+i],inv[3+i]);

   printf("\n");
   MatMinor(mat,inv);
   for (i=0;i<16;i+=4)
   printf("%9f %9f %9f %9f\n",inv[0+i],inv[1+i],inv[2+i],inv[3+i]);

   printf("\n");
   MatMult(mat,inv,out);
   for (i=0;i<16;i+=4)
   printf("%9f %9f %9f %9f\n",out[0+i],out[1+i],out[2+i],out[3+i]);


   return(0);
   }
 */

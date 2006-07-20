#include "bio_coords.h"

void compute_CRD(bio_struct *bs)
{
    double *RSA,*CRD,*M[4];
    int i,j,w;

    for(i=0;i<bs->target_count;i++) (bs->targetsRSA)[3+i*4]=1;
    M[0]=(bs->RSAtoCRD);
    for(i=1;i<4;i++) M[i]=M[i-1]+4;

    for(i=0;i<(bs->target_count)*4;i++) (bs->targetsCRD)[i]=0;
    for(w=0;w<bs->target_count;w++)
    {
        CRD=bs->targetsCRD+w*4;
        RSA=bs->targetsRSA+w*4;
        for(j=0;j<4;j++) for(i=0;i<4;i++) CRD[j]+=RSA[i]*M[i][j];
    }
    for(i=0;i<bs->target_count;i++) (bs->targetsCRD)[i*4]+=1; /* colums start at 1 */
}

#include "bio_coords.h"

void write_output(char *fileName, bio_struct *bs)
{
    int i;
    FILE *out;

    if ((out = fopen (fileName, "w" )) == NULL)
    {
        fprintf(stderr,"Can't open this file for writing: %s\n", fileName);
        exit(1);
    }

    fprintf(out,"\n\n\n\n\n");
    fprintf(out,"patient_name =%s\n",bs->patient_name);
    fprintf(out,"\n");
    fprintf(out,"patient_id =%s\n",bs->patient_id);
    fprintf(out,"\n");
    fprintf(out,"date =%s\n",bs->date);
    fprintf(out,"\n");
    fprintf(out,"\n");
    fprintf(out,"1) Anterior  Right (RSA) = %7.1f %7.1f %7.1f\n",
            (bs->AntRight)[0],(bs->AntRight)[1],(bs->AntRight)[2]);
    fprintf(out,"\n");
    fprintf(out,"2) Posterior Right (RSA) = %7.1f %7.1f %7.1f\n",
            (bs->PostRight)[0],(bs->PostRight)[1],(bs->PostRight)[2]);
    fprintf(out,"\n");
    fprintf(out,"3) Posterior Left  (RSA) = %7.1f %7.1f %7.1f\n",
            (bs->PostLeft)[0],(bs->PostLeft)[1],(bs->PostLeft)[2]);
    fprintf(out,"\n");
    fprintf(out,"4) Anterior  Left  (RSA) = %7.1f %7.1f %7.1f\n",
            (bs->AntLeft)[0],(bs->AntLeft)[1],(bs->AntLeft)[2]);
    fprintf(out,"\n");
    fprintf(out,"\n");
    fprintf(out,"Step = %7.1f\n",bs->grid_step);
    fprintf(out,"\n");
    fprintf(out,"\n");

    if (bs->target_count > 0)
    {
        fprintf(out,"Points           R       S       A    Col    Row    Depth(cm)\n");
        fprintf(out,"-------------------------------------------------------------\n");
        for(i=0;i<bs->target_count;i++)
            fprintf(out,"\n%s %7.1f %7.1f %7.1f  %5.1f  %5.1f  %7.1f\n",
                (bs->target_names)+i*256,
                (bs->targetsRSA)[0+4*i],(bs->targetsRSA)[1+4*i],(bs->targetsRSA)[2+4*i],
                (bs->targetsCRD)[0+4*i],(bs->targetsCRD)[1+4*i],(bs->targetsCRD)[2+4*i]);
    }

    for(i=0;i<4;i++) fprintf(out,"\n");
    fprintf(out,"Corrections to Make By Corner - SqError\n");
    fprintf(out,"---------------------------------------\n");  
    for(i=0;i<4;i++)
        fprintf(out,"%d) [%7.1f %7.1f %7.1f ] - %7.1f\n",i+1,
                (bs->corn_err)[0+4*i],(bs->corn_err)[1+4*i],
                (bs->corn_err)[2+4*i],(bs->corn_err)[3+4*i]);
    fprintf(out,"---------------------------------------\n");  
    fprintf(out,"Direction Perpendicular to Template \n");
    fprintf(out,"---------------------------------------\n");  
    fprintf(out,"N) [%7.1f %7.1f %7.1f ] - %7.1f (degrees from I/S)\n",
            (bs->RSANormalToTemplate)[0],  (bs->RSANormalToTemplate)[1],
            (bs->RSANormalToTemplate)[2],
            acos((bs->RSANormalToTemplate)[1])*180.0/3.1415926);

    fclose(out);
}

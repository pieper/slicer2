#include "bio_coords.h"

void do_sscanf(char * in_data, double * array)
{
    float v[3];
    int i;
    for(i=0;i<3;i++) v[i]=0.0;
    sscanf(in_data,"%f %f %f",v,v+1,v+2);
    for(i=0;i<3;i++) array[i]=v[i];
    array[3]=1.0; /* homogeneous coords */
}

void parse_input(char *fileName, bio_struct *bs)
{
    int i,got[20],point_mode=0;
    float tmp;
    FILE * in_file;
    char *ret,in_data[1000],*c;


    /* set defaults */
    (bs->patient_name)[0]=(bs->patient_id)[0]=(bs->date)[0]=0;
    bs->grid_step=5.0;
    bs->AntRight=bs->corners;
    bs->PostRight=bs->corners+4;
    bs->PostLeft=bs->corners+8;
    bs->AntLeft=bs->corners+12;
    for(i=0;i<16;i++) 
    {
        (bs->corners[i])=0.0;
        (bs->cornersT[i])=0.0;
        (bs->corn_err[i])=0.0;
        (bs->RSAtoCRD[i])=0.0;
    }
    bs->target_count=0;
    bs->targetsRSA=NULL;
    bs->targetsCRD=NULL;
    for(i=0;i<20;i++) got[i]=0;

    /* first pass through file */
    /* get patient info, number of targets */
    if ( (in_file = fopen (fileName, "r" )) == NULL)
    {
        fprintf(stderr,"Can't open this file for reading: %s\n", fileName);
        exit(1);
    }

    while(1)
    {
        if ((ret=fgets(in_data,256,in_file))==NULL) break;
        if ((strlen(in_data)<5)||(in_data[0]==' ')) continue;
        if (strncmp(in_data,"------",6)==0) continue;

        if (point_mode==0)
        {
            if (strncmp(in_data,"patient_name =",14)==0)
            {
                strncpy(bs->patient_name,in_data+14,255);
                got[0]=1;
                continue;
            }
            if (strncmp(in_data,"patient_id =",12)==0)
            {
                strncpy(bs->patient_id,in_data+12,255);
                got[1]=1;
                continue;      
            }
            if (strncmp(in_data,"date =",6)==0)
            {
                strncpy(bs->date,in_data+6,255);
                got[2]=1;
                continue;      
            }
            if (strncmp(in_data,"1) Anterior  Right (RSA) =",26)==0)
            {
                do_sscanf(in_data+26,bs->corners);
                got[3]=1;
                continue;
            }
            if (strncmp(in_data,"2) Posterior Right (RSA) =",26)==0)
            {
                do_sscanf(in_data+26,bs->corners+4);
                got[4]=1;
                continue;
            }
            if (strncmp(in_data,"3) Posterior Left  (RSA) =",26)==0)
            {
                do_sscanf(in_data+26,bs->corners+8);
                got[5]=1;
                continue;
            }
            if (strncmp(in_data,"4) Anterior  Left  (RSA) =",26)==0)
            {
                do_sscanf(in_data+26,bs->corners+12);
                got[6]=1;
                continue;
            }
            if (strncmp(in_data,"Step =",6)==0)
            {
                tmp=0.0;
                sscanf(in_data+6,"%f",&tmp);
                bs->grid_step=tmp;
                got[7]=1;
                continue;
            }
            if (strncmp(in_data,"Points",6)==0)
            {
                point_mode=1;
                continue;
            }
        }
        else /* point mode is 1 now, target coords area */
        {
            (bs->target_count)++;
        }
    }

    fclose(in_file);

    /* make sure we have gotten all the patient info */
    i=0;
    if (got[0]==0)
    {
        fprintf(stderr,"Missing patient name.\n"); i=1;
    }
    if (got[1]==0)
    {
        fprintf(stderr,"Missing patient id.\n"); i=1;
    }
    if (got[2]==0)
    {
        fprintf(stderr,"Missing date.\n"); i=1;
    }
    if (got[3]==0)
    {
        fprintf(stderr,"Missing 1) Anterior Right.\n"); i=1;
    }
    if (got[4]==0)
    {
        fprintf(stderr,"Missing 2) Posterior Right.\n"); i=1;
    }
    if (got[5]==0)
    {
        fprintf(stderr,"Missing 3) Posterior Left.\n"); i=1;
    }
    if (got[6]==0)
    {
        fprintf(stderr,"Missing 4) Anterior Left.\n"); i=1;
    }
    if (got[7]==0)
    {
        fprintf(stderr,"Missing grid step.\n"); i=1;
    }

    if ((bs->target_count)>0)
    {
        bs->target_names=(char*)calloc((bs->target_count)*256,1);
        bs->targetsRSA=(double*)calloc((bs->target_count)*4,sizeof(double));
        bs->targetsCRD=(double*)calloc((bs->target_count)*4,sizeof(double));
        if ((bs->target_names==NULL)||(bs->targetsRSA==NULL)||(bs->targetsCRD==NULL))
        {
            fprintf(stderr,"Can't Allocate Memory.\n"); i=1;
        }
    }

    if (i) exit(1);


    /* second pass, get target info */

    if ( (in_file = fopen (fileName, "r" )) == NULL)
    {
        fprintf(stderr,"Can't open this file for reading: %s\n", fileName);
        exit(1);
    }

    point_mode=0;
    bs->target_count=0;
    while(1)
    {
        if ((ret=fgets(in_data,256,in_file))==NULL) break;
        if ((strlen(in_data)<5)||(in_data[0]==' ')) continue;
        if (strncmp(in_data,"------",6)==0) continue;
        if (point_mode==0)
        {
            if (strncmp(in_data,"Points",6)==0)
            {
                point_mode=1;
                continue;
            }
        }
        else
        {
            i=0;
            while(in_data[i]!=' ') i++;
            in_data[i]=0;
            do_sscanf(in_data+i+1,bs->targetsRSA+4*(bs->target_count));
            c=bs->target_names+256*(bs->target_count);
            strncpy(c,in_data,255);
            c[10]=0; /* make all target names length 10 */
            if (strlen(c)<10) for(i=strlen(c);i<10;i++) c[i]=' ';
            (bs->target_count)++;
        }
    }

    fclose(in_file);

}

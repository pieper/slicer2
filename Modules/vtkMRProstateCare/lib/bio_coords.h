#ifndef _BIO_COORDS_H
#define _BIO_COORDS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <math.h>
#include "Decompose.h"
#include "Mat.h"

typedef struct _bio_struct
{
    char patient_name[256];
    char patient_id[256];
    char date[256];
    double grid_step;
    double corners[16];
    double *AntRight;
    double *PostRight;
    double *PostLeft;
    double *AntLeft;
    double cornersT[16];
    double corn_err[16];
    double RSAtoCRD[16]; 
    double RSANormalToTemplate[3];
    int target_count;
    char * target_names;
    double * targetsRSA;
    double * targetsCRD;
} bio_struct;

void free_bs(bio_struct *bs);
void write_output(char *fileName, bio_struct *bs);
void compute_CRD(bio_struct *bs);
void compute_transform(bio_struct *bs);
void parse_input(char *fileName, bio_struct *bs);

#ifdef __cplusplus
    }
#endif

#endif /* _BIO_COORDS_H */

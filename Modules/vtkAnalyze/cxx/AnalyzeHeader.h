/*=auto=========================================================================

(c) Copyright 2005 Massachusetts Institute of Technology (MIT) All Rights Reserved.

This software ("3D Slicer") is provided by The Brigham and Women's 
Hospital, Inc. on behalf of the copyright holders and contributors.
Permission is hereby granted, without payment, to copy, modify, display 
and distribute this software and its documentation, if any, for  
research purposes only, provided that (1) the above copyright notice and 
the following four paragraphs appear on all copies of this software, and 
(2) that source code to any modifications to this software be made 
publicly available under terms no more restrictive than those in this 
License Agreement. Use of this software constitutes acceptance of these 
terms and conditions.

3D Slicer Software has not been reviewed or approved by the Food and 
Drug Administration, and is for non-clinical, IRB-approved Research Use 
Only.  In no event shall data or images generated through the use of 3D 
Slicer Software be used in the provision of patient care.

IN NO EVENT SHALL THE COPYRIGHT HOLDERS AND CONTRIBUTORS BE LIABLE TO 
ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL 
DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, 
EVEN IF THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE BEEN ADVISED OF THE 
POSSIBILITY OF SUCH DAMAGE.

THE COPYRIGHT HOLDERS AND CONTRIBUTORS SPECIFICALLY DISCLAIM ANY EXPRESS 
OR IMPLIED WARRANTIES INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND 
NON-INFRINGEMENT.

THE SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS 
IS." THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE NO OBLIGATION TO 
PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.


=========================================================================auto=*/

/* ANALYZETM Header File Format 
 * 
 * (c) Copyright, 1986-1995 
 * Biomedical Imaging Resource 
 * Mayo Foundation 
 * 
 * Original file name: dbh.h 
 * 
 * databse sub-definitions 
 */ 
struct header_key                 /* header key */ 
{                                 /* off + size */ 
    int sizeof_hdr;               /* 0 + 4 */ 
    char data_type[10];           /* 4 + 10 */ 
    char db_name[18];             /* 14 + 18 */ 
    int extents;                  /* 32 + 4 */ 
    short int session_error;      /* 36 + 2 */ 
    char regular;                 /* 38 + 1 */ 
    char hkey_un0;                /* 39 + 1 */ 
};                                /* total=40 bytes */


struct image_dimension 
{                                 /* off + size */ 
    short int dim[8];             /* 0 + 16 */ 
    char vox_units[4];            /* 16 + 4 */
    char cal_units[8];            /* 20 + 8 */
    short int unused1;            /* 28 + 2 */
    short int datatype;           /* 30 + 2 */ 
    short int bitpix;             /* 32 + 2 */ 
    short int dim_un0;            /* 34 + 2 */ 
    float pixdim[8];              /* 36 + 32 */ 
        /* pixdim[] specifies the voxel dimensitons: 
           pixdim[1] - voxel width 
           pixdim[2] - voxel height 
           pixdim[3] - interslice distance ...etc 
         */ 
    float vox_offset;            /* 68 + 4 */ 
    float funused1;              /* 72 + 4 */ 
    float funused2;              /* 76 + 4 */ 
    float funused3;              /* 80 + 4 */ 
    float cal_max;               /* 84 + 4 */ 
    float cal_min;               /* 88 + 4 */ 
    float compressed;            /* 92 + 4 */ 
    float verified;              /* 96 + 4 */ 
    int glmax,glmin;             /* 100 + 8 */ 
};                               /* total=108 bytes */ 


struct data_history 
{                                /* off + size */ 
    char descrip[80];            /* 0 + 80 */ 
    char aux_file[24];           /* 80 + 24 */ 
    char orient;                 /* 104 + 1 */ 
    char originator[10];         /* 105 + 10 */ 
    char generated[10];          /* 115 + 10 */ 
    char scannum[10];            /* 125 + 10 */ 
    char patient_id[10];         /* 135 + 10 */ 
    char exp_date[10];           /* 145 + 10 */ 
    char exp_time[10];           /* 155 + 10 */ 
    char hist_un0[3];            /* 165 + 3 */ 
    int views;                   /* 168 + 4 */ 
    int vols_added;              /* 172 + 4 */ 
    int start_field;             /* 176 + 4 */ 
    int field_skip;              /* 180 + 4 */ 
    int omax, omin;              /* 184 + 8 */ 
    int smax, smin;              /* 192 + 8 */ 
};


struct dsr 
{ 
    header_key hk;               /* 0 + 40 */ 
    image_dimension dime;        /* 40 + 108 */ 
    data_history hist;           /* 148 + 200 */ 
};                               /* total= 348 bytes */ 


/* Acceptable values for datatype */ 
#define DT_NONE            0 
#define DT_UNKNOWN         0 
#define DT_BINARY          1 
#define DT_UNSIGNED_CHAR   2 
#define DT_SIGNED_SHORT    4 
#define DT_SIGNED_INT      8 
#define DT_FLOAT           16 
#define DT_COMPLEX         32 
#define DT_DOUBLE          64 
#define DT_RGB             128 
#define DT_ALL             255 


typedef struct 
{ 
    float real; 
    float imag; 
} COMPLEX;




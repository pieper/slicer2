/**
 * File:          $RCSfile: 3x3ltmatTf_noc.h,v $
 * Module:        3x3 lower triangular matrices (transposed, single precision)
 * Part of:       Gandalf Library
 *
 * Revision:      $Revision: 1.1.2.1 $
 * Last edited:   $Date: 2007/10/14 03:16:25 $
 * Author:        $Author: ruetz $
 * Copyright:     (c) 2000 Imagineer Software Limited
 *
 * Notes:         Not to be compiled separately
 */

#define GAN_MATTYPE Gan_SquMatrix33_f
#define GAN_MATRIX_TYPE Gan_SquMatrix_f
#define GAN_SQUMATRIX_TYPE Gan_SquMatrix_f
#define GAN_VECTOR_TYPE Gan_Vector_f
#define GAN_MAT_ELEMENT_TYPE GAN_FLOAT
#define GAN_REALTYPE float
#define GAN_MAT_FPRINT   gan_ltmat33Tf_fprint
#define GAN_MAT_PRINT    gan_ltmat33Tf_print
#define GAN_MAT_FSCANF   gan_ltmat33Tf_fscanf
#define GAN_FIXED_MATRIX_TYPE GAN_LOWER_TRI_MATRIX33_F

#define gan_ltmat33Tf_fwrite gan_ltmat33f_fwrite
#define gan_ltmat33Tf_fread  gan_ltmat33f_fread

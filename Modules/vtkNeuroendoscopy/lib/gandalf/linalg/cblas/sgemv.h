/**************************************************************************
*
* File:          $RCSfile: sgemv.h,v $
* Module:        BLAS function
* Part of:       Gandalf Library
*
* Revision:      $Revision: 1.1.2.1 $
* Last edited:   $Date: 2007/10/14 02:26:22 $
* Author:        $Author: ruetz $
* Copyright:     Modifications (c) 2000 Imagineer Software Limited
*
* Notes:         
* Private func:  
* History:       Modified from original CLAPACK source code 
*
**************************************************************************/

#ifndef _GAN_SGEMV_H
#define _GAN_SGEMV_H

#include <gandalf/common/misc_defs.h>
#include <gandalf/linalg/linalg_defs.h>

#ifdef __cplusplus
extern "C" {
#endif

/* only declare this function locally if there is no LAPACK installed */
#if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK)
Gan_Bool gan_sgemv ( Gan_TposeFlag trans, long m, long n, float alpha,
                     float *a, long lda, float *x, long incx, 
                     float beta, float *y, long incy );
#endif


#ifdef __cplusplus
}
#endif

#endif /* #ifndef _GAN_SGEMV_H */

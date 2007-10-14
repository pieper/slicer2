/**************************************************************************
*
* File:          $RCSfile: strsm.h,v $
* Module:        BLAS function
* Part of:       Gandalf Library
*
* Revision:      $Revision: 1.1.2.1 $
* Last edited:   $Date: 2007/10/14 02:26:23 $
* Author:        $Author: ruetz $
* Copyright:     Modifications (c) 2003 Imagineer Systems Limited
*
* Notes:         
* Private func:  
* History:       Modified from original CLAPACK source code 
*
**************************************************************************/

#ifndef _GAN_STRSM_H
#define _GAN_STRSM_H

#include <gandalf/common/misc_defs.h>
#include <gandalf/linalg/linalg_defs.h>

#ifdef __cplusplus
extern "C" {
#endif

/* only declare this function locally if there is no LAPACK installed */
#if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK)
Gan_Bool gan_strsm ( Gan_LapackSideFlag side, Gan_LapackUpLoFlag uplo,
                     Gan_TposeFlag transa, Gan_LapackUnitFlag diag,
                     long m, long n,
                     float alpha, float *a, long lda, float *b, long ldb );
#endif

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _GAN_STRSM_H */

/**************************************************************************
*
* File:          $RCSfile: dgetf2.h,v $
* Module:        CLAPACK function
* Part of:       Gandalf Library
*
* Revision:      $Revision: 1.1.2.1 $
* Last edited:   $Date: 2007/10/14 02:26:44 $
* Author:        $Author: ruetz $
* Copyright:     Modifications (c) 2000 Imagineer Software Limited
*
* Notes:         
* Private func:  
* History:       Modified from original CLAPACK source code 
*
**************************************************************************/

#ifndef _GAN_DGETF2_H
#define _GAN_DGETF2_H

#include <gandalf/common/misc_defs.h>

#ifdef __cplusplus
extern "C" {
#endif

/* only declare this function locally if there is no LAPACK installed */
#if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK)
Gan_Bool gan_dgetf2 ( long m, long n, double *a, long lda,
                      long *ipiv, long *info );
#endif /* #if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK) */

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _GAN_DGETF2_H */

/**************************************************************************
*
* File:          $RCSfile: dsptrd.h,v $
* Module:        CLAPACK function
* Part of:       Gandalf Library
*
* Revision:      $Revision: 1.1.2.1 $
* Last edited:   $Date: 2007/10/14 02:26:47 $
* Author:        $Author: ruetz $
* Copyright:     Modifications (c) 2000 Imagineer Software Limited
*
* Notes:         
* Private func:  
* History:       Modified from original CLAPACK source code 
*
**************************************************************************/

#ifndef _GAN_DSPTRD_H
#define _GAN_DSPTRD_H

#include <gandalf/common/misc_defs.h>
#include <gandalf/linalg/linalg_defs.h>

#ifdef __cplusplus
extern "C" {
#endif

/* only declare this function locally if there is no LAPACK installed */
#if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK)
Gan_Bool gan_dsptrd(Gan_LapackUpLoFlag upper, long n, double *ap, 
                    double *d, double *e, double *tau, long *info);
#endif

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _GAN_DSPTRD_H */


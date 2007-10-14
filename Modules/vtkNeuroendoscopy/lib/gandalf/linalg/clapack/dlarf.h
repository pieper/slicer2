/**************************************************************************
*
* File:          $RCSfile: dlarf.h,v $
* Module:        CLAPACK function
* Part of:       Gandalf Library
*
* Revision:      $Revision: 1.1.2.1 $
* Last edited:   $Date: 2007/10/14 02:26:45 $
* Author:        $Author: ruetz $
* Copyright:     Modifications (c) 2000 Imagineer Software Limited
*
* Notes:         
* Private func:  
* History:       Modified from original CLAPACK source code 
*
**************************************************************************/

#ifndef _GAN_DLARF_H
#define _GAN_DLARF_H

#include <gandalf/common/misc_defs.h>
#include <gandalf/linalg/linalg_defs.h>

#ifdef __cplusplus
extern "C" {
#endif

/* only declare this function locally if there is no LAPACK installed */
#if !defined(HAVE_LAPACK) || defined(FORCE_LOCAL_LAPACK)
Gan_Bool
 gan_dlarf ( Gan_LapackSideFlag side, long m, long n, double *v, long incv,
             double tau, double *c, long ldc, double *work );
#endif

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _GAN_DLARF_H */


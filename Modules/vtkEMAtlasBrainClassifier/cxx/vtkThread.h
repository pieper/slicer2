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
// In the Future use vtkMultiThreader instead and delete this file 
// An example how to use vtkMultiThreader is given in Module/vtkLevelSets/cxx/vtkLevelSets.cxx
#ifndef __vtkThread_h
#define __vtkThread_h

#include <vtkEMAtlasBrainClassifierConfigure.h>
#include "vtkObject.h"

#ifndef _WIN32
// ----------------------------------------------------------------------------------------------
// Definitions for parallising from thread_combat.h 
// ----------------------------------------------------------------------------------------------
#include <unistd.h>
#include <pthread.h>

#undef _REENTRANT
#define _REENTRANT

// ----------------------------------------------------------------------------------------------
// Definitions for paralising algorithm from
// workpile.h Simon Warfield simonw@bwh.harvard.edu */
// $Id: vtkThread.h,v 1.1 2005/09/02 15:26:02 pohl Exp $ */
// ----------------------------------------------------------------------------------------------

#include <stdlib.h>
#include "assert.h"
#include <sys/types.h>
//
//BTX - begin tcl exclude
//

/* Workpile controller */
typedef void (*work_proc_t)(void *);
typedef struct workpile_struct {
    pthread_mutex_t lock;        /* mutex for this structure */
    pthread_cond_t work_wait;    /* workers waiting for work */
    pthread_cond_t finish_wait;    /* to wait for workers to finish */
    int max_pile;    /* length of workpile array */
    int n_working;    /* number of workers working */
    int n_waiting;    /* number of workers waiting for work */
    int n_pile;    /* number of pointers in the workpile */
    int inp;    /* FIFO input pointer */
    int outp;    /* FIFO output pointer */
    int n_threads;    /* Number of threads created */
    void **pile;    /* array of pointers - the workpile */
    pthread_t *tid;    /* array keeping track of thread id's */
    work_proc_t worker_proc;    /* work procedure */
} *workpile_t;

#ifdef _SC_NPROCESSORS_ONLN
#define vtkThreadNumCpus(void) (int)(sysconf(_SC_NPROCESSORS_ONLN))
#else
#define vtkThreadNumCpus(void) 1
#endif

//ETX  
class VTK_EMATLASBRAINCLASSIFIER_EXPORT vtkThread { //; prevent man page generation
public:
  static vtkThread *New() {return (new vtkThread);}
//BTX
  int thread_concurrency(int concurrencyLevel);
  workpile_t work_init(int max_pile, work_proc_t worker_proc, int n_threads);
  void work_put(workpile_t wp, void *ptr) ;
  void work_wait(workpile_t wp) ;
  void work_finished_forever(workpile_t wp) ;

protected:
  static void worker(workpile_t wp);
  void work_change_worker_proc(work_proc_t worker_proc, workpile_t wp);
//ETX
};
#else 
// In the future use GetGlobalDefaultNumberOfThreads from vtkMultiThreade
// Cannpt use return bc we just subtitute the name  vtkThreadNumCpus with { ... }
#define vtkThreadNumCpus(x) {SYSTEM_INFO sysInfo; GetSystemInfo(&sysInfo); x = (int) sysInfo.dwNumberOfProcessors;}

#endif // _WIN32
#endif

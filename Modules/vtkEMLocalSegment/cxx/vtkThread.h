/*=auto=========================================================================

(c) Copyright 2001 Massachusetts Institute of Technology

Permission is hereby granted, without payment, to copy, modify, display 
and distribute this software and its documentation, if any, for any purpose, 
provided that the above copyright notice and the following three paragraphs 
appear on all copies of this software.  Use of this software constitutes 
acceptance of these terms and conditions.

IN NO EVENT SHALL MIT BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, 
INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE 
AND ITS DOCUMENTATION, EVEN IF MIT HAS BEEN ADVISED OF THE POSSIBILITY OF 
SUCH DAMAGE.

MIT SPECIFICALLY DISCLAIMS ANY EXPRESS OR IMPLIED WARRANTIES INCLUDING, 
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR 
A PARTICULAR PURPOSE, AND NON-INFRINGEMENT.

THE SOFTWARE IS PROVIDED "AS IS."  MIT HAS NO OBLIGATION TO PROVIDE 
MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

=========================================================================auto=*/
#ifndef __vtkThread_h
#define __vtkThread_h

#include <vtkEMLocalSegmentConfigure.h>
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
// $Id: vtkThread.h,v 1.3 2003/05/18 14:18:27 pieper Exp $ */
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
class VTK_EXPORT vtkThread { //; prevent man page generation
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

#endif // _WIN32
#endif

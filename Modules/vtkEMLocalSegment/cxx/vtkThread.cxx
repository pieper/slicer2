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
#undef _REENTRANT
#define _REENTRANT

#ifndef _WIN32

#include <unistd.h>
#include <pthread.h>
#include <vtkThread.h>
#ifdef __cplusplus
extern "C" {
#endif

#ifdef __SOLARIS__
/* Must be a Sun machine */
  include <thread.h>
#endif

#ifdef __cplusplus
}
#endif

extern "C" {
  extern int thr_setconcurrency(int);
}


//  int vtkThread::num_cpus(void)
//  {
//    return sysconf(_SC_NPROCESSORS_ONLN);
//  }

int vtkThread::thread_concurrency(int concurrencyLevel)
{
  int code = 0;
#ifndef linux
  code = thr_setconcurrency(concurrencyLevel);
#endif
  return code;
}

#define THREADSTUBONLY 1
#undef THREADSTUBONLY

#ifdef THREADSTUBONLY

int pthread_create __P ((pthread_t *__thread,
                                __const pthread_attr_t *__attr,
                                void *(*__start_routine) (void *),
                                void *__arg))
{
  return 1;
}

void pthread_exit(void *retval) { exit(0); }

int pthread_cond_broadcast(pthread_cond_t *cond)
{
  return 1;
}

int pthread_cond_init __P ((pthread_cond_t *__cond,
                                   __const pthread_condattr_t *__cond_attr))
{
  return 1;
}

pthread_t pthread_self __P ((void))
{
  return (pthread_t)0;
}

int pthread_mutex_destroy(pthread_mutex_t *mutex)
{
  return 1;
}

int pthread_cond_destroy(pthread_cond_t *cond)
{
  return 1;
}

int pthread_join(pthread_t th, void **thread_return)
{
  return 1;
}

int pthread_mutex_init __P ((pthread_mutex_t *__mutex,
                                    __const pthread_mutexattr_t *__mutex_attr))
{
  return 0;
}
#endif

// ---------------------------------------------------------
// Stuff To paralise algorithm from workpile.c
// ---------------------------------------------------------
/* Simon Warfield simonw@bwh.harvard.edu */
/* $Id: vtkThread.cxx,v 1.1 2003/05/12 17:38:58 pieper Exp $ */

/* Forward declaration for worker function */
// static void worker(workpile_t wp);

/*
 * Allocates and initializes a workpile that holds max_pile entries.
 * worker_proc is called to process each work item on the queue.
 */
workpile_t vtkThread::work_init(int max_pile, work_proc_t worker_proc, int n_threads)
{
  int err;
  workpile_t wp = NULL;
  pthread_attr_t attr;

  wp = (workpile_t)malloc(sizeof (struct workpile_struct));
  assert(wp != NULL);

  /* initialize attr with default attributes */
  assert(pthread_attr_init(&attr) == 0); 
  /* default */
  /* assert(pthread_attr_setscope(&attr, PTHREAD_SCOPE_PROCESS) == 0); */
  /* 'bound' to an LWP: */
  assert(pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM) == 0);

  /* 
   * Guarantee there will be at least enough room in the workpile
   * to allow work_finished_forever to kill all the threads.
   * Note that work_finished_forever waits for all the jobs to be done
   * before it requests the threads kill themselves.
   */
  if (max_pile < n_threads) {
    max_pile = n_threads;
  }

  assert(wp != NULL);
  wp->pile = (void **)malloc(max_pile * sizeof(void *));
  assert(wp->pile != NULL);
  wp->tid = (pthread_t *)malloc(n_threads * sizeof(pthread_t));
  assert(wp->tid != NULL);

  if (wp != NULL) {
    /* thread_concurrency(num_cpus()); */
    this->thread_concurrency(n_threads);
    pthread_mutex_init(&wp->lock, NULL);
    pthread_cond_init(&wp->work_wait, NULL);
    pthread_cond_init(&wp->finish_wait, NULL);
    wp->max_pile = max_pile;
    wp->worker_proc = worker_proc;
    wp->n_working = wp->n_waiting = wp->n_pile = 0;
    wp->inp = wp->outp = 0;
    wp->n_threads = n_threads;
    while (n_threads--) {
      err = pthread_create(&wp->tid[n_threads], &attr,
              (void *(*)(void *))worker, (void *)wp);
      if (err != 0) {
        perror("Thread creation returned error");
        assert(err == 0);
        exit(1);
      }
    }
  }
  /* Thread creation is over so lets destroy the pthread_attr */
  assert(pthread_attr_destroy(&attr) == 0); 
  return (wp);
}

/* Puts ptr in workpile. Called at the outset, or within a worker. */
void vtkThread::work_put(workpile_t wp, void *ptr)
{
    pthread_mutex_lock(&wp->lock);
    if (wp->n_waiting) {
        /* idle workers to be awakened */
        pthread_cond_signal(&wp->work_wait);
    }
    assert(wp->n_pile != wp->max_pile);                                    /* check for room */
    wp->n_pile++;
    wp->pile[wp->inp] = ptr;
    wp->inp = (wp->inp + 1) % wp->max_pile;
    pthread_mutex_unlock(&wp->lock);
}

/*
 * Worker thread routine. Continuously looks for work, calls the
 * worker_proc associated with the workpile to do work.
 */
inline void vtkThread::worker(workpile_t wp)
{
  void *ptr = NULL;

  pthread_mutex_lock(&wp->lock);
  wp->n_working++;
  for (;;) {
    while (wp->n_pile == 0) {/* wait for new work */
      if (--wp->n_working == 0) {
        pthread_cond_signal(&wp->finish_wait);
      }
      wp->n_waiting++;
      pthread_cond_wait(&wp->work_wait, &wp->lock);
      wp->n_waiting--;
      wp->n_working++;
    }
    wp->n_pile--;
    ptr = wp->pile[wp->outp];
    wp->outp = (wp->outp + 1) % wp->max_pile;
    if (ptr != NULL) {
      pthread_mutex_unlock(&wp->lock);
      /* Call application worker routine. */
      (*wp->worker_proc)(ptr);
      pthread_mutex_lock(&wp->lock);
    } else { 
      /* Special case - if the work to be done is NULL,
       * we are asking the worker thread to commit suicide */
      /* Lets return the workpile to a reasonable state */
      if (--wp->n_working == 0) {
      pthread_cond_signal(&wp->finish_wait);
      }
      /* printf("Thread %d electing to die\n",(int)pthread_self()); */
      pthread_mutex_unlock(&wp->lock);
      pthread_exit(NULL);
    }
  }
  /* NOTREACHED */
}

/* Wait until all work is done and workers quiesce. */
void vtkThread::work_wait(workpile_t wp)
{
  pthread_mutex_lock(&wp->lock);
  while(wp->n_pile !=0 || wp->n_working != 0) {
    pthread_cond_wait(&wp->finish_wait, &wp->lock);
  }
  pthread_mutex_unlock(&wp->lock);
}

/* Wait until all work is done and then ask the workers 
 * to kill themselves.
 *   Manage the number of LWP's appropriately.
 */
void  vtkThread::work_finished_forever(workpile_t wp)
{
  int i = 0;
  int n_threads = 0;

  pthread_mutex_lock(&wp->lock);
  while(wp->n_pile !=0 || wp->n_working != 0) {
    pthread_cond_wait(&wp->finish_wait, &wp->lock);
  }
  n_threads = wp->n_threads;

  /* Ask all the threads to kill themselves by the special mechanism
   * of giving them null work.
   *   This strategy requires that the work pile can have at least
   * as many jobs on it as there are threads.  
   * Hence, work_init guarantee's this.
   */
  for (i = 0; i < n_threads; i++) {
    assert(wp->n_pile != wp->max_pile);  /* check for room */
    wp->n_pile++;
    wp->pile[wp->inp] = NULL;
    wp->inp = (wp->inp + 1) % wp->max_pile;
  }
  if (wp->n_waiting) {
    /* idle workers to be awakened for the jobs I have created */
    pthread_cond_broadcast(&wp->work_wait);
  }
  pthread_mutex_unlock(&wp->lock);
/* During this period, the lock is released and someone could send more
 * work to the workpile.  However, it would never get done, since the
 * threads will kill themselves before they get to it.
 *   Consequently work_wait would never return for the processing adding
 * to the workpile.
 *   I could add a flag indicating impending death and prevent work_put
 * from happening, or assume the programmer won't use the same workpile
 * for new work and for killing threads at the same time.
 */
  /* In theory it should be safe to read wp->tid because they are
   * initialized once and never written again.
   */
  for (i = 0; i < n_threads; i++) {
    if (pthread_join(wp->tid[i], NULL) != 0) {
      fprintf(stderr,"Thread %d has nonzero exit status\n",(int)wp->tid[i]);
    }
  }
  /* Now all the threads are dead we can delete the resources */
  pthread_mutex_lock(&wp->lock);
  assert(wp->n_working == 0);  /* If not true, someone else has beaten
       us to the threads, and we are hosed */
  free(wp->pile);
  free(wp->tid);
  pthread_cond_destroy(&wp->work_wait);
  pthread_cond_destroy(&wp->finish_wait);
  pthread_mutex_unlock(&wp->lock);
  pthread_mutex_destroy(&wp->lock);
  /* We still need to free the wp itself */
  /* free(wp); wp = NULL; */
}

/* Update the worker proc for this workpile */
void  vtkThread::work_change_worker_proc(work_proc_t worker_proc, workpile_t wp)
{
  pthread_mutex_lock(&wp->lock);
  if (wp->n_pile !=0 || wp->n_working != 0) {
    fprintf(stderr,"It is probably a good idea to wait for all the worker functions to have finished working before changing the job, but you know best...\n");
  }
  wp->worker_proc = worker_proc;
  pthread_mutex_unlock(&wp->lock);
}

#endif  //_WIN32

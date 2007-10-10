/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkCapture.h,v $
  Date:      $Date: 2007/10/10 20:10:00 $
  Version:   $Revision: 1.1.2.1 $

=========================================================================auto=*/

#ifndef __vtkCapture_h
#define __vtkCapture_h
#include <pthread.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <getopt.h>             /* getopt_long() */

#include <fcntl.h>              /* low-level i/o */
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <asm/types.h>          /* for videodev2.h */

#include <linux/videodev2.h>

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>


#define CLEAR(x) memset (&(x), 0, sizeof (x))

class  vtkCapture  {
public:
  static void errno_exit(const char *s);
  static int xioctl (int fd, int request, void * arg);
  static char * process_image(void * p);
  static int read_frame (void);
  static void mainloop (void);
  static void stop_capturing (void);
  static void start_capturing (void);
  static void uninit_device (void);
  static void init_read (unsigned int buffer_size);
  static void init_mmap (void);
  static void init_userp (unsigned int buffer_size);
  static void init_device (void);
  static void close_device (void);
  static void open_device (void);
  static char * startCapture  (void);


};


#endif

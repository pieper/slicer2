/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: xc.h,v $
  Date:      $Date: 2007/08/10 00:36:53 $
  Version:   $Revision: 1.1.2.1 $

=========================================================================auto=*/

#ifndef __xc_h
#define __xc_h

#include <vtkNeuroendoscopyConfigure.h>

#include <pthread.h>

#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <errno.h>

#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/videodev.h>

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Porthole.h>
#include <X11/Xaw/MenuButton.h>
#include <X11/Xaw/SimpleMenu.h>
#include <X11/Xaw/SmeBSB.h>


/*  Set these according to your capture device set-up */
#define VID_DEVICE     "/dev/video0" /* Device Name */
#define VID_PIX_FLAGS     V4L2_FIELD_ANY /* Video Output Format */
#define VID_STD "NTSC-M" /* Default Standard */
#define VID_WIDTH      640 /* Default Width */
#define VID_HEIGHT     480 /* Default Height */
#define VID_STREAMING     0 /* Streaming mode boolean */
#define STREAMBUFS    20 /* Number of streaming buffer */




typedef struct tag_vimage
  {
    struct v4l2_buffer vidbuf;
    char *data;
    char *data2;
    int   length;
  }
VIMAGE;

typedef struct capture_device
{
  int fd_video;
  int width;
  int height;
  int depth;
  int pixelformat;
  VIMAGE vimage[STREAMBUFS];
  struct v4l2_format fmt;
  struct v4l2_requestbuffers req;
}capture_device;

/* Windows Display Objects */
typedef struct capture_window
{
  XtAppContext *app;
  Widget       w_toplevel;
  Window       w_window;
  Widget       w_topbox;
  Widget       w_menubox;
  Widget       w_quit;
  Display *    w_viewport;
  Widget       w_stdbutton;
  Widget       w_stdmenu;
  Widget       w_sizebutton;
  Widget       w_sizemenu;
  GC           imagegc;
  int          x_depth;
}capture_window;

/* Standard Menu Objects */
typedef struct StdMenuItem
{
  Widget w_item;
   //struct v4l2_input std;
  //old
  struct v4l2_standard std;
  capture_window *capt_w;
  capture_device *capt_d;
}
StdMenuItem;

/* Size Menu Objects */
//struct capturesize
//  {
//    int width, height;
//   };
// capturesize[] =
// {
//   { 160, 120 } ,
//   { 192, 144 } ,
//   { 320, 240 } ,
//   { 384, 288 } ,
//   { 640, 480 } ,
//   { 768, 576 } ,
// };

typedef struct
{
  int index;
  capture_window *capt_w;
  capture_device *capt_d;
}
SzMenuItem;




class  xc {


public:



   XImage *capt_ximg[STREAMBUFS];
  

  int xioctl (int fd, int request, void * arg);

  int capture_fmt (int x_depth);

  int capture_perf (capture_device * pcapt_d);

  void capture_end (capture_device * pcapt_d);

  void capture_std (capture_device * pcapt_d, struct v4l2_standard *std);

  void capture_size (capture_device * pcapt_d, int width, int height);

  void display_free (XImage *XImgTmp);

  XImage *display_alloc (capture_window * pcapt_w, char *data, int width, int height);

  void capture_stop (capture_device * pcapt_d, int streaming);

  int capture_start (capture_device * pcapt_d, int streaming);

  void open_device (capture_device * pcapt_d, char *dev_name);

  int capture_init(capture_device * pcapt_d, char *my_device, unsigned int format, int width, int height, int depth);

  int capture_read (capture_device * pcapt_d, int sz_img);

  int capture_dq (capture_device * pcapt_d, struct v4l2_buffer *pbuf, int streaming);

  void capture_q (capture_device * pcapt_d, struct v4l2_buffer *pbuf, int streaming);

  int display_get_depth (capture_window * pcapt_w);

  int display_start (capture_device * pcapt_d, capture_window * pcapt_w, int width, int height);

  void display_stop (capture_device *pcapt_d, capture_window * pcapt_w);

  void quit_callback (Widget w, XtPointer client, XtPointer call);

  void std_menu_callback (Widget w, XtPointer client, XtPointer call);

  void size_menu_callback (Widget w, XtPointer client, XtPointer call);

  int display_init (capture_window * pcapt_w, capture_device * pcapt_d, char *my_device, int argc, char *argv[]);

  int display_status (capture_window * pcapt_w);

  void display_refresh(capture_window *pcapt_w, XImage *XImgTmp, int width, int height);
  
  void display_wakeup (capture_window * pcapt_w);

  void grabSetFps(int fd, int fps);
  
  int startwindow(void * n);
  
  static void * startwindow2(void* n);
  
  XImage * getCurrentPic(void);
  
  void setCurrentPic (XImage *XImgTmp);

protected:

 XImage * currentPic;

};


#endif

/*      X Window Video Capture Testing Tool
 *
 *      For testing a Video for Linux Two video capture driver
 *
 *      This program was written by Olivier Carmona, based on Bill Dirks.
 *      This program is in the public domain.
 *
 */
 
#ifndef __xc_cxx
#define __xc_cxx

#include "xc.h"

#include <assert.h>   // I include this to test return values the lazy way
#include <unistd.h>   // So we got the profile for 10 seconds

#define NIL (0)       // A name for the void pointer


/***************************/
/* Device Catpure Routines */
/***************************/
int counter = 0;


int xc::capture_fmt (int x_depth)
{
  int fbf = V4L2_PIX_FMT_RGB565;

  if (x_depth == 15)
    fbf = V4L2_PIX_FMT_RGB555;
  else if (x_depth == 16)
    fbf = V4L2_PIX_FMT_RGB565;
  else if (x_depth == 24)
    fbf = V4L2_PIX_FMT_BGR24;
  else if (x_depth == 32)
    fbf = V4L2_PIX_FMT_BGR32;
  else
    {
      printf ("Unrecognized display depth. You may get an "
          "X Windows error.\n");
    }
    
  return fbf;
}


int xc::capture_perf (capture_device * pcapt_d)
{


return 30;
}

void xc::capture_end (capture_device * pcapt_d)
{
  close (pcapt_d->fd_video);
}

void xc::capture_std (capture_device * pcapt_d, struct v4l2_standard *std)
{
 int err;
v4l2_std_id std_id;
 std_id = V4L2_STD_NTSC_M;

//printf (V4L2_STD_NTSC_M);
// v4l2_std NTSC to Standard
 err = ioctl (pcapt_d->fd_video, VIDIOC_S_STD, &std->id);
 if (err == -1)
   {
     perror ("S_STD in capture_std");
   }




}

void xc::capture_size (capture_device * pcapt_d, int width, int height)
{
 int err;

 pcapt_d->fmt.fmt.pix.width = width;
 pcapt_d->fmt.fmt.pix.height = height;
 err = ioctl (pcapt_d->fd_video, VIDIOC_S_FMT, &pcapt_d->fmt);
 if (err == -1)
   {
     perror ("S_FMT in capture_size");
   }
}

void xc::display_free (XImage *XImgTmp)
{
  if (XImgTmp)
    XDestroyImage(XImgTmp);
   
  return;
}

XImage *xc::display_alloc (capture_window * pcapt_w, char *data, int width, int height)
{
  XImage *XImgTmp;


      
XImgTmp = XCreateImage (pcapt_w->w_viewport, DefaultVisual (pcapt_w->w_viewport, 0), (pcapt_w->x_depth == 32) ? 24 : pcapt_w->x_depth, ZPixmap, 0, data, width, height, 16, 0);
  
  
  if (XImgTmp == NULL)
    {
      printf ("No XImage\n");
    }
  
  return XImgTmp;
}

void xc::capture_stop (capture_device * pcapt_d, int streaming)
{
  int i, err;

  if (pcapt_d->fd_video >= 0)
    {
      if (streaming)
        {
          //old declaration
          //i = V4L2_BUF_TYPE_CAPTURE;
          i = V4L2_BUF_TYPE_VIDEO_CAPTURE;
          err = ioctl (pcapt_d->fd_video, VIDIOC_STREAMOFF, &i);
          for (i = 0; i < pcapt_d->req.count; ++i)
          {
          if (pcapt_d->vimage[i].data)
            munmap (pcapt_d->vimage[i].data, pcapt_d->vimage[i].length);
          pcapt_d->vimage[i].data = NULL;
          capt_ximg[i]->data = NULL;
          display_free (capt_ximg[i]);
        }
    }
      else
        {
          if (pcapt_d->vimage[0].data)
          free (pcapt_d->vimage[0].data);
          pcapt_d->vimage[0].data = NULL;
      capt_ximg[i]->data = NULL;
      display_free (capt_ximg[0]);
    }
    }
}

int xc::capture_start (capture_device * pcapt_d, int streaming)
{
  struct v4l2_standard std;
  int i, err;

  /* Get Video Standard */
  err = ioctl (pcapt_d->fd_video, VIDIOC_G_STD, &std);
  if (err == -1)
    perror ("G_STD in capture_start");

  /* Get Video Format */
  pcapt_d->fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  //old
  //pcapt_d->fmt.type = V4L2_BUF_TYPE_CAPTURE;
  err = ioctl (pcapt_d->fd_video, VIDIOC_G_FMT, &pcapt_d->fmt);
  if (err == -1)
    perror ("G_FMT in capture_start");

  pcapt_d->width = pcapt_d->fmt.fmt.pix.width;
  pcapt_d->height = pcapt_d->fmt.fmt.pix.height;
  pcapt_d->depth = 32;
  //old
  //pcapt_d->depth = pcapt_d->fmt.fmt.pix.depth;
  pcapt_d->pixelformat = pcapt_d->fmt.fmt.pix.pixelformat;
  
 if (streaming)
   {
     /* Ask Video Device for Buffers */
//new

     pcapt_d->req.count = STREAMBUFS;
     pcapt_d->req.type =  V4L2_BUF_TYPE_VIDEO_CAPTURE;
     pcapt_d->req.memory = V4L2_MEMORY_MMAP;
     //old
     //pcapt_d->req.type = V4L2_BUF_TYPE_CAPTURE;
    // err = ioctl (pcapt_d->fd_video, VIDIOC_REQBUFS, &pcapt_d->req);
     err = ioctl (pcapt_d->fd_video, VIDIOC_REQBUFS,  &pcapt_d->req);

    if (err == -1  || pcapt_d->req.count < 1) { 
      if (EINVAL == errno) { 
        fprintf (stderr, " does not support memory mapping\n");
        exit (EXIT_FAILURE);
      } else {
        perror ("VIDIOC_REQBUFS");
        return 0;
      }
    }





     
    
     /* Query each buffer and map it to the video device */
     for (i = 0; i < pcapt_d->req.count; ++i)
       {
         struct v4l2_buffer *vidbuf = &pcapt_d->vimage[i].vidbuf;
      
         vidbuf->index = i;
         vidbuf->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
         //old
         //vidbuf->type = V4L2_BUF_TYPE_CAPTURE;
         err = ioctl (pcapt_d->fd_video, VIDIOC_QUERYBUF, vidbuf);
         if (err == -1) 
          {
         perror ("QUERYBUF in capture_start");
         return 0;
       }
       
         pcapt_d->vimage[i].length = 0;
         pcapt_d->vimage[i].data = (char*) mmap (0, vidbuf->length, 
               PROT_READ|PROT_WRITE, MAP_SHARED,
               pcapt_d->fd_video, vidbuf->m.offset);
         //old
         //pcapt_d->vimage[i].data = mmap (0, vidbuf->length, PROT_READ|PROT_WRITE, MAP_SHARED, pcapt_d->fd_video, vidbuf->offset);
         if (*pcapt_d->vimage[i].data == -1)
       {
         perror ("mmap() in capture_start");
         return 0;
          }
         pcapt_d->vimage[i].length = vidbuf->length; 

     err = ioctl (pcapt_d->fd_video, VIDIOC_QBUF, vidbuf);
         if (err == -1)
           {
         perror ("QBUF in capture_start");
         return 0;
           }
       }
       
     /* Set video stream capture on */
     err = ioctl (pcapt_d->fd_video, VIDIOC_STREAMON, &pcapt_d->vimage[0].vidbuf.type);
     if (err == -1)
       perror ("STREAMON in capture_start");
   }
 else
    {
      /* Alloc one Buffer for one image */
     // int sizeimg = pcapt_d->width*pcapt_d->height*pcapt_d->depth/8;
      int sizeimg = pcapt_d->fmt.fmt.pix.sizeimage;
      
      pcapt_d->req.count = 1;
      pcapt_d->vimage[0].data = (char*) malloc (sizeimg);
      if (pcapt_d->vimage[0].data == NULL)
        {
          printf ("malloc(%d) failed in capture_start\n", sizeimg);
          return 0;
        }
    }

  return 1;
}

void xc::open_device (capture_device * pcapt_d, char *dev_name)
{
        struct stat st; 

        if (-1 == stat (dev_name, &st)) {
                fprintf (stderr, "Cannot identify '%s': %d, %s\n",
                         dev_name, errno, strerror (errno));
                exit (EXIT_FAILURE);
        }

        if (!S_ISCHR (st.st_mode)) {
                fprintf (stderr, "%s is no device\n", dev_name);
                exit (EXIT_FAILURE);
        }

        pcapt_d->fd_video = open (dev_name, O_RDWR /* required */ | O_NONBLOCK, 0);

        if (-1 == pcapt_d->fd_video) {
                fprintf (stderr, "Cannot open '%s': %d, %s\n",
                         dev_name, errno, strerror (errno));
                exit (EXIT_FAILURE);
        }
}


int xc::capture_init(capture_device * pcapt_d, char *my_device, unsigned int format, int width, int height, int depth)
{
  int err;
  
  struct v4l2_capability cap;
  struct v4l2_streamparm parm;
  struct v4l2_standard std;

//   /* Open Video Device */
//   pcapt_d->fd_video = open (my_device, O_RDWR /* required */ | O_NONBLOCK, 0);
//   //old
//   //pcapt_d->fd_video = open (my_device, O_RDWR);
// 
//   if (pcapt_d->fd_video < 0)
//     {
//       printf ("No video device \"%s\"\n", my_device);
//       return 0;
//     }

   open_device(pcapt_d,my_device);


  /* Querry Video Device Capabilities */
  err = ioctl (pcapt_d->fd_video, VIDIOC_QUERYCAP, &cap);
  if (err == -1)
    {
      perror ("QUERYCAP in capture_init");
      return 0;
    }
  if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE))
   //old
  //if (cap.type != V4L2_TYPE_CAPTURE)
    {
      printf ("Not a capture device.\n");
      return 0;
    }
   if (!(cap.capabilities & V4L2_CAP_READWRITE)) {    
   //old
  //if (!(cap.capabilities & V4L2_FLAG_READ))
    
      printf ("Device does not support the read() call.\n");
    }
   if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
   //old
  //if (!(cap.capabilities & V4L2_FLAG_STREAMING))
    
      printf ("Device does not support streaming capture.\n");
    }


  /* Set Video Parameters */
   parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
//old
//  parm.type = V4L2_BUF_TYPE_CAPTURE;

  err = ioctl (pcapt_d->fd_video, VIDIOC_G_PARM, &parm);
  if (err == -1)
    perror ("G_PARM in capture_init");
  
  err = ioctl (pcapt_d->fd_video, VIDIOC_S_PARM, &parm);
  if (err == -1)
    perror ("S_PARM in capture_init");
    
  /* Get Video Standard */
  err = ioctl (pcapt_d->fd_video, VIDIOC_G_STD, &std);
  if (err == -1)
    perror ("G_STD in capture_init");

  /* Set Video Format */
pcapt_d->fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  //old
  //  pcapt_d->fmt.type = V4L2_BUF_TYPE_CAPTURE;
  err = ioctl (pcapt_d->fd_video, VIDIOC_G_FMT, &pcapt_d->fmt);
  if (err == -1)
    perror ("G_FMT in capture_init");

  if (width != 0)
    pcapt_d->fmt.fmt.pix.width = width;
  else
    pcapt_d->fmt.fmt.pix.width = VID_WIDTH;
  if (height != 0) 
    pcapt_d->fmt.fmt.pix.height = height;
  else
    pcapt_d->fmt.fmt.pix.height = VID_HEIGHT;
  
   
   //old
   // pcapt_d->fmt.fmt.pix.depth = depth;
    
  //pcapt_d->fmt.fmt.pix.flags |= VID_PIX_FLAGS;
#ifdef VID_PIXELFORMAT
  pcapt_d->fmt.fmt.pix.pixelformat = VID_PIXELFORMAT;
#else
  pcapt_d->fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_BGR32;
#endif

  pcapt_d->width = pcapt_d->fmt.fmt.pix.width;
  pcapt_d->height = pcapt_d->fmt.fmt.pix.height;
  pcapt_d->depth = 32;
  //old
  //pcapt_d->depth = pcapt_d->fmt.fmt.pix.depth;
  pcapt_d->pixelformat = pcapt_d->fmt.fmt.pix.pixelformat;
  
  err = ioctl (pcapt_d->fd_video, VIDIOC_S_FMT, &pcapt_d->fmt);
  if (err == -1)
    perror ("S_FMT in capture_init");
  
  return (1);
}

int xc::capture_read (capture_device * pcapt_d, int sz_img)
{
  int err;
  
  err = read (pcapt_d->fd_video, pcapt_d->vimage[0].data, sz_img);

  
       
        
  if (err == -1)
    {
      perror ("READ in capture_read");
      return 0;
    } else {


  FILE * pFile;
  pFile = fopen ( "myfile.raw" , "w" );
  fwrite(pcapt_d->vimage[0].data, (size_t)err, 1, pFile);
  fclose (pFile);

    }
  pcapt_d->fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_BGR32;
  return 1;
}

int xc::capture_dq (capture_device * pcapt_d, struct v4l2_buffer *pbuf, int streaming)
{
  int err, n;
  fd_set rdset;
  struct timeval timeout;

  if (streaming)
    {
      FD_ZERO (&rdset);
      FD_SET (pcapt_d->fd_video, &rdset);

      timeout.tv_sec = 1;
      timeout.tv_usec = 0;

      n = select (pcapt_d->fd_video + 1, &rdset, NULL, NULL, &timeout);
      if (n == -1)
    fprintf (stderr, "select error.\n");
      else if (n == 0)
    fprintf (stderr, "select timeout\n");
      else if (FD_ISSET (pcapt_d->fd_video, &rdset))
    {
      pbuf->type = pcapt_d->vimage[0].vidbuf.type;
      err = ioctl (pcapt_d->fd_video, VIDIOC_DQBUF, pbuf);
      if (err == -1)
        {
          perror ("DQBUF in capture_dq");
          return 0;
        }
      return 1;
    }
    }
  else
    {
     
//      int sizepic = pcapt_d->width*pcapt_d->height*pcapt_d->depth/8;
pcapt_d->fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_BGR24;
      int sizepic = pcapt_d->fmt.fmt.pix.sizeimage;
      fprintf (stderr, "size of file %d\n",sizepic);
      capture_read(pcapt_d, sizepic);
      pbuf->index = 0;
      return 1;
    }
  return 0;
}

void xc::capture_q (capture_device * pcapt_d, struct v4l2_buffer *pbuf, int streaming)
{
  int err;

  if (streaming)
    {
      err = ioctl (pcapt_d->fd_video, VIDIOC_QBUF, pbuf);
      if (err == -1)
    perror ("QBUF in capture_q");
    }
} 
 

/****************************/
/* Windows Display Routines */
/****************************/

int xc::display_get_depth (capture_window * pcapt_w)
{
  if (pcapt_w->x_depth == 0)
    {
#ifdef X_DEPTH
      pcapt_w->x_depth = X_DEPTH;
#else
      Display *d = XtDisplay (pcapt_w->w_toplevel);
      Window w = DefaultRootWindow (XtDisplay (pcapt_w->w_toplevel));

      XWindowAttributes wa;
      XGetWindowAttributes (d, w, &wa);
      pcapt_w->x_depth = wa.depth;
#endif
    }
  return (pcapt_w->x_depth);
}

int xc::display_start (capture_device * pcapt_d, capture_window * pcapt_w, int width, int height)
{
  Arg arg[2];
  int i;

  for (i=0; i < pcapt_d->req.count; i++)
    {
      capt_ximg[i] = display_alloc (pcapt_w, pcapt_d->vimage[i].data, pcapt_d->width, pcapt_d->height);

    }
  
//   XtSetArg (arg[0], XtNwidth, width);
//   XtSetArg (arg[1], XtNheight, height);
//   XtSetValues (pcapt_w->w_viewport, arg, 2);
//   XtRealizeWidget (pcapt_w->w_toplevel);

  return 1;
}

void xc::display_stop (capture_device *pcapt_d, capture_window * pcapt_w)
{
  return;
}

void xc::quit_callback (Widget w, XtPointer client, XtPointer call)
{
  capture_device *pcapt_d = (capture_device *) client;

  if (pcapt_d->fd_video >= 0)
    {
      capture_perf(pcapt_d);
      capture_stop (pcapt_d, VID_STREAMING);
      capture_end (pcapt_d); 
    }
  exit (0);
}

void xc::std_menu_callback (Widget w, XtPointer client, XtPointer call)
{
  StdMenuItem *smi = (StdMenuItem *) client;
  capture_device *pcapt_d = smi->capt_d;
  capture_window *pcapt_w = smi->capt_w;

  capture_stop (pcapt_d, VID_STREAMING);
  display_stop (pcapt_d, pcapt_w);
  capture_std (pcapt_d, &smi->std);
  capture_start (pcapt_d, VID_STREAMING);
  display_start (pcapt_d, pcapt_w, pcapt_d->width, pcapt_d->height);
}

void xc::size_menu_callback (Widget w, XtPointer client, XtPointer call)
{
  SzMenuItem *szi = (SzMenuItem *) client;
  int i = szi->index;
  capture_window *pcapt_w = szi->capt_w;
  capture_device *pcapt_d = szi->capt_d;
  
  capture_stop (pcapt_d, VID_STREAMING);
  display_stop (pcapt_d, pcapt_w);
  //capture_size (pcapt_d, capturesize[i].width, capturesize[i].height);
  
  capture_size (pcapt_d, 640, 480);
  capture_start (pcapt_d, VID_STREAMING);
  display_start (pcapt_d, pcapt_w, pcapt_d->width, pcapt_d->height);
}

int xc::display_init (capture_window * pcapt_w, capture_device * pcapt_d, char *my_device, int argc, char *argv[])
{
  int err, i;
  int width, height, format;
  fprintf(stderr,"vor XtAppContext\n");
  pcapt_w->app = (XtAppContext *) malloc (sizeof (XtAppContext));
     
     
     
     
     
     
      Display *dpy = XOpenDisplay(NIL);
      assert(dpy);

      // Get some colors
      pcapt_w->w_viewport = dpy;

      int blackColor = BlackPixel(dpy, DefaultScreen(dpy));
      int whiteColor = WhitePixel(dpy, DefaultScreen(dpy));

      // Create the window

      Window w = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), 0, 0, 
                     640, 480, 0, blackColor, blackColor);

      pcapt_w->w_window = w;

      // We want to get MapNotify events

      XSelectInput(dpy, w, StructureNotifyMask);

      // "Map" the window (that is, make it appear on the screen)

      XMapWindow(dpy, w);

      // Create a "Graphics Context"

      GC gc = XCreateGC(dpy, w, 0, NIL);
      pcapt_w->imagegc = gc;
      // Tell the GC we draw using the white color

      XSetForeground(dpy, gc, whiteColor);

      // Wait for the MapNotify event

//       for(;;) {
//         XEvent e;
//         XNextEvent(dpy, &e);
//         if (e.type == MapNotify)
//           break;
//       }

      // Draw the line
      
      //XImgTmp = XCreateImage (dpy, DefaultVisual (XtDisplay (pcapt_w->w_viewport), 0), (pcapt_w->x_depth == 32) ? 24 : pcapt_w->x_depth, ZPixmap, 0, data, width, height, 16, 0);

      // Send the "DrawLine" request to the server

      XFlush(dpy);

      // Wait for 10 seconds

      //sleep(10);





  fprintf(stderr,"vor XtAppInitialize\n");
  //pcapt_w->w_toplevel = XtAppInitialize (pcapt_w->app, "DTSS", NULL, 0, &argc, argv, NULL, NULL, 0);

  fprintf(stderr,"vor Display depth\n");
  //display_get_depth (pcapt_w);
  fprintf(stderr,"vor capture_fmt\n");

  pcapt_w->x_depth = 32;

  //format = capture_fmt(pcapt_w->x_depth);
  fprintf(stderr,"vor capture init\n");
  capture_init(pcapt_d, my_device, format, 0, 0, 0);
  fprintf(stderr,"vor width\n");
  width = pcapt_d->fmt.fmt.pix.width;
fprintf(stderr,"vor height\n");
  height = pcapt_d->fmt.fmt.pix.height;

/*  
  pcapt_w->w_topbox = XtVaCreateManagedWidget ("topbox",
                           boxWidgetClass,
                           pcapt_w->w_toplevel,
                           XtNorientation,
                           XtorientVertical,
                           XtNborderWidth, 0, XtNhSpace,
                           0, XtNvSpace, 0, NULL);
  pcapt_w->w_menubox =
    XtVaCreateManagedWidget ("menubox", boxWidgetClass, pcapt_w->w_topbox,
                 XtNorientation, XtorientHorizontal,
                 XtNborderWidth, 0, XtNhSpace, 4, XtNvSpace, 1,
                 NULL);

  pcapt_w->w_stdbutton = XtVaCreateManagedWidget ("menubutton",
                          menuButtonWidgetClass,
                          pcapt_w->w_menubox,
                          XtNlabel, "Standard",
                          XtNmenuName, "standard",
                          XtNborderWidth, 0, NULL);
  pcapt_w->w_stdmenu = XtVaCreatePopupShell ("standard", simpleMenuWidgetClass, pcapt_w->w_stdbutton, NULL);
*/

//   for (i = 0;; ++i)
//     {
//       struct v4l2_standard estd;
//       //old
//       //struct v4l2_enumstd estd;
//       
//       StdMenuItem *smi;
// 
//       estd.index = i;
//       err = ioctl (pcapt_d->fd_video, VIDIOC_ENUMSTD, &estd);
//       if (err == -1)
//     break;
//       smi = (StdMenuItem *) malloc (sizeof (StdMenuItem));
//      // smi->std = VIDIOC_ENUMSTD;
//       //old
//       smi->std = estd;
//       smi->capt_w = pcapt_w;
//       smi->capt_d = pcapt_d;
//       smi->w_item = XtVaCreateManagedWidget ((char*) estd.name,
//                          smeBSBObjectClass,
//                          pcapt_w->w_stdmenu, XtNlabel,
//                          (char*) estd.name, 
//                                              NULL);
//       XtAddCallback (smi->w_item, XtNcallback, std_menu_callback, smi);
//     }
// 
//   pcapt_w->w_sizebutton = XtVaCreateManagedWidget ("menubutton",
//                            menuButtonWidgetClass,
//                            pcapt_w->w_menubox,
//                            XtNlabel, "Size",
//                            XtNmenuName, "size",
//                            XtNborderWidth, 0, NULL);
//   pcapt_w->w_sizemenu = XtVaCreatePopupShell ("size", simpleMenuWidgetClass,  pcapt_w->w_sizebutton, NULL);

//   for (i = 0; i < sizeof (capturesize) / sizeof (capturesize[0]); ++i)
//     {
//       char sizestr[32];
//       SzMenuItem *szi;
//       Widget w;
// 
//       szi = (SzMenuItem *) malloc (sizeof (SzMenuItem));
//       szi->index = i;
//       szi->capt_w = pcapt_w;
//       szi->capt_d = pcapt_d;
//       sprintf (sizestr, "%d x %d", capturesize[i].width, capturesize[i].height);
//       w = XtVaCreateManagedWidget (sizestr,
//                    smeBSBObjectClass, pcapt_w->w_sizemenu,
//                    XtNlabel, sizestr, NULL);
// 
//       XtAddCallback (w, XtNcallback, size_menu_callback, (char *) szi);
//     }

//   pcapt_w->w_quit = XtVaCreateManagedWidget ("Quit!",
//                          commandWidgetClass,
//                          pcapt_w->w_menubox,
//                          XtNborderWidth, 0, NULL);
//   XtAddCallback (pcapt_w->w_quit, XtNcallback, quit_callback, pcapt_d);

//   pcapt_w->w_viewport = XtVaCreateManagedWidget ("port",
//                          portholeWidgetClass,
//                          pcapt_w->w_topbox, XtNwidth,
//                          width, XtNheight,
//                          height, NULL);
//   XtRealizeWidget (pcapt_w->w_toplevel);
//   pcapt_w->imagegc = XCreateGC (XtDisplay(pcapt_w->w_viewport), XtWindow(pcapt_w->w_viewport), 0, NULL);
// 
// 
// 
//   if (pcapt_w->imagegc == NULL)
//     {
//       printf ("XCreateGC failed.\n");
//       return 0;
//     }

  

  return 1;
}

// int xc::display_status (capture_window * pcapt_w)
// {
//   XEvent event;
// 
//   if (XtIsRealized (pcapt_w->w_viewport))
//     {
//       return 1;
//     }
//   else
//     {
//       XtAppPeekEvent (*pcapt_w->app, &event);
//       return 0;
//     }
// 
// }

void xc::display_refresh(capture_window *pcapt_w, XImage *XImgTmp, int width, int height)
{



  XPutImage (pcapt_w->w_viewport, pcapt_w->w_window, pcapt_w->imagegc, XImgTmp, 0, 0, 0, 0, 640, 480);
  XFlush(pcapt_w->w_viewport);
  setCurrentPic(XImgTmp);
  //xc::currentPic = XImgTmp;

}

void xc::setCurrentPic (XImage * XImgTmp) {
  currentPic = XImgTmp;
}

XImage * xc::getCurrentPic(void) {

 XImage * XImgTmp = currentPic;
  return XImgTmp;
}

void xc::display_wakeup (capture_window * pcapt_w)
{
  XEvent event;

  while (XtAppPending (*pcapt_w->app))
    {
      XtAppNextEvent (*pcapt_w->app, &event);
      switch (event.type)
    {
      default:
        break;
    }
      XtDispatchEvent (&event);
    }
}
   
void xc::grabSetFps(int fd, int fps)
{
  struct v4l2_streamparm params;
  //new
  struct v4l2_fract framesact;



  printf("called v4l2_set_fps with fps=%d\n",fps);
  params.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  //old
  //params.type = V4L2_BUF_TYPE_CAPTURE;
  ioctl(fd, VIDIOC_G_PARM, &params);
  printf("time per frame is: %ld\n", params.parm.capture.timeperframe);
  //new
  params.parm.capture.capability = V4L2_CAP_TIMEPERFRAME;
  params.parm.capture.capturemode |= V4L2_MODE_HIGHQUALITY;
  //params.parm.capture.capturemode |= V4L2_CAP_TIMEPERFRAME;
  framesact.numerator = 10000000 ;
  framesact.denominator =  fps;
  params.parm.capture.timeperframe = framesact;
  if (fps == 30)
    framesact.numerator = 333667 ;
    framesact.denominator =  1;
    params.parm.capture.timeperframe = framesact;
  printf("time per frame is: %ld\n", params.parm.capture.timeperframe);
  ioctl(fd, VIDIOC_S_PARM, &params);
  
  framesact.numerator = 0 ;
  framesact.denominator =  1;
  params.parm.capture.timeperframe = framesact;
  ioctl(fd, VIDIOC_G_PARM, &params);
  
   printf("time per frame is: %ld\n", params.parm.capture.timeperframe);  
}
void * xc::startwindow2(void * n) {

xc * xcobj = new xc();
xcobj->startwindow(n);

}
//int main (int argc, char *argv[])
int xc::startwindow(void * n)
{
 

  capture_window capt_w;
  capture_device capt_d;
  int argi;
  int argc; 
  char **argv;
  /* Video Parameters */
  char my_device[64];

  /*    Put in the device node name */
  strcpy (my_device, VID_DEVICE);

  /* Read arguments */
  capt_w.x_depth = 0; /* detect */
//   for (argi = 1; argi < argc; ++argi)
//     {
//       if (strcmp (argv[argi], "-b") == 0)
//     {
//       capt_w.x_depth = 32;
//       continue;
//     }
//       strcpy (my_device, argv[argi]);
//     }
  fprintf(stderr,"vor displayinit\n");
  display_init (&capt_w, &capt_d, my_device, argc, argv);
  
  fprintf(stderr,"vor grabSetFPS\n");
  grabSetFps(capt_d.fd_video, 30);
  
  fprintf(stderr,"vor capture_start\n");
  
  if (!capture_start (&capt_d, VID_STREAMING))
    exit(1);
  fprintf(stderr,"vor display start\n");
  
  display_start (&capt_d, &capt_w, capt_d.width, capt_d.height);
    
  printf ("Capturing %dx%dx%d \"%4.4s\" images\n", capt_d.width, capt_d.height, capt_d.depth, (char *) &(capt_d.pixelformat));
  printf ("Images are %d bytes each\n", capt_d.width * capt_d.height * capt_d.depth / 8);
  

int count = 100;
  for (;;)
    //for (int i = 0; i < count; i++)
    {
      struct v4l2_buffer tempbuf;

      //if (display_status (&capt_w))
//    {

      if (capture_dq (&capt_d, &tempbuf, VID_STREAMING))
        {

          display_refresh (&capt_w, capt_ximg[tempbuf.index], capt_d.width, capt_d.height);
              
              
             // fprintf (stderr, capt_d.vimage[tempbuf.index].data); 
            capture_q (&capt_d, &tempbuf, VID_STREAMING);
              
              
              n = getCurrentPic();
        }
//    }
//
     // display_wakeup (&capt_w);
    }

  return 0;
}


#endif

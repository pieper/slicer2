/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkCaptureFrames.h,v $
  Date:      $Date: 2007/10/10 20:07:02 $
  Version:   $Revision: 1.1.2.2 $
  Programmed by: Christoph Ruetz (ruetz@bwh.harvard.edu
=========================================================================auto=*/
#ifndef __vtkCaptureFrames_h
#define __vtkCaptureFrames_h

//#include "vtkObjectFactory.h"
#include <string.h>
#include <vtkObject.h>
#include <vtkNeuroendoscopyConfigure.h>
#include <vtkImageImport.h>
#include <vtkImageSource.h>
#include <vtkImageData.h>
#include <pthread.h>


//  #include <stl>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <errno.h>

/* These are needed to use the Videum driver */
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/videodev.h>  /* Video for Linux Two */



typedef char * charpointer;
typedef void * voidpointer;

class VTK_NEUROENDOSCOPY_EXPORT vtkCaptureFrames : public vtkObject {
public:
    static vtkCaptureFrames *New();
    //void PrintSelf(ostream& os, vtkIndent indent);
    
   // char *GetEndoscopyNEWVersion();
    vtkTypeMacro(vtkCaptureFrames, vtkObject);

   int Initialize(int width, int height);
   
    vtkGetMacro(width,int);
   
    vtkGetMacro(height,int);
   //get the next frame of the Capture device
   vtkImageData * getNextFrame(void);



 
//   vtkImageData * startGrabbing();
  

private:

 
    charpointer device;
    int            vid;
    int            err;
    int            i;
    int            n;
//BTX
    struct v4l2_capability    cap;
        struct v4l2_format    fmt;
//ETX
    int            bgr;
    int            rgb;
    int            rgbswap    ;
    char * data;
    char            t;
    int offset;
    int width;
    int height;
//         static void * grabbingLoop(void* n);
        
//           vtkImageData * frameUpdated;
};

#endif

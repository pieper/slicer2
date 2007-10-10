/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkCaptureFrames.cxx,v $
  Date:      $Date: 2007/10/10 20:06:59 $
  Version:   $Revision: 1.1.2.2 $
  Programmed by: Christoph Ruetz (ruetz@bwh.harvard.edu
=========================================================================auto=*/
#ifndef __vtkCaptureFrames_cxx
#define __vtkCaptureFrames_cxx
#include "vtkObjectFactory.h"
#include "vtkCaptureFrames.h"
#include <vtkImageFlip.h>
#include <sstream>
#include <string>


vtkStandardNewMacro(vtkCaptureFrames);

int vtkCaptureFrames::Initialize(int width, int height)
{

 this->width = width;
 this->height = height;

    //if (device == NULL)
        device = "/dev/video0";
//opendevice
    vid = open(device, O_RDONLY);
    if (vid < 0)
    {
        fprintf(stderr, "Can't open %s\n", device);
        return 1;
    }

    err = ioctl(vid, VIDIOC_QUERYCAP, &cap);
    if (err)
    {
        fprintf(stderr, "QUERYCAP returned error %d\n", errno);
        return 1;
    }

        if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
                fprintf (stderr, "%s is no video capture device\n",
                         device);
                return 1; 
        }

       
    if (!(cap.capabilities & V4L2_CAP_READWRITE))    
        //old
        //if (!(cap.flags & V4L2_FLAG_READ))
    {
        fprintf(stderr, "Device %s doesn't support read().\n", device);
        return 1;
    }


    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

      //old
    //fmt.type = V4L2_BUF_TYPE_CAPTURE;
    err = ioctl(vid, VIDIOC_G_FMT, &fmt);
    if (err)
    {
        fprintf(stderr, "G_FMT returned error %d\n", errno);
        return 1;
    }

        fmt.fmt.pix.width = this->width;
        fmt.fmt.pix.height = this->height;
        fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_BGR24;
   
    err = ioctl(vid,VIDIOC_S_FMT,&fmt) < 0;
    if (err)
    {
        fprintf(stderr, "VIDIOC_S_FMT returned error %d\n", errno);
        return 1;
    }

    v4l2_std_id std_id;
    std_id = V4L2_STD_NTSC_M;
    err = ioctl(vid,VIDIOC_S_STD, &std_id);
    
    if (err) {
        fprintf(stderr, "VIDIOC_S_STD returned error %d\n", errno);
        return 1;
    }

     int index;

     index = 2;
     err = ioctl(vid, VIDIOC_S_INPUT, &index);
    
   
    if (err) {
        
        fprintf(stderr, "VIDIOC_S_INPUT returned error %d\n", errno);
        return 1;
    }
      

    if ((fmt.fmt.pix.pixelformat == V4L2_PIX_FMT_RGB24 && bgr) ||
        (fmt.fmt.pix.pixelformat == V4L2_PIX_FMT_BGR24 && rgb))
        rgbswap = 1;


    data = (char*) malloc(fmt.fmt.pix.sizeimage);
    if (data == NULL)
    {
        fprintf(stderr, "malloc(%d) failed\n", fmt.fmt.pix.sizeimage);
        return 1;
    }


        
}


// vtkImageData * vtkCaptureFrames::startGrabbing() {
// 
// 
// pthread_mutex_t lock;
// pthread_mutex_init(&lock,NULL);
// 
// pthread_attr_t attrCapture;
// pthread_t tidCapture;
// 
// pthread_attr_init(&attrCapture);
// 
// pthread_create(&tidCapture,&attrCapture,vtkCaptureFrames::grabbingLoop,frameUpdated);
// 
// return (vtkImageData *) frameUpdated;
//  
// 
// }
// 
//  void * vtkCaptureFrames::grabbingLoop(void* n) {
// vtkCaptureFrames * capFrame = new vtkCaptureFrames();
// 
// // capFrame->Initialize();
// 
// 
//   while (true) {
//     n = capFrame->getNextFrame();
// // fprintf(stderr, "in loop\n");
//   }
// 
// }


vtkImageData * vtkCaptureFrames::getNextFrame() 
{

   
        n = read(vid, data, fmt.fmt.pix.sizeimage);
    if (n < 0)
    {
        fprintf(stderr, "read() returned error %d\n", errno);
        
    }


// fprintf(stderr, "after read data \n");

    for (i = 0; i < n; i += 3)
    {
      t = data[i];
      data[i] = data[i + 2];
      data[i + 2] = t;
//fprintf(stderr, "%d\n",i);
    }

// fprintf(stderr, "after bgr changing \n");
std::stringstream ss;
std::string str;
ss << this->width << " " << this->height;
ss >> str;



std::string header = "P6\n" + str + "\n255\n ";

//char * header = "P6\n640 480\n255\n ";
offset = header.size();

        char ppmarray[n+offset];
        //char * ppmarray;



         for (i = 0; i < n+offset; i++)
     {
            
           if (i < offset) {
           ppmarray[i]=header[i];
           }
           else {
           //fprintf(stderr, "%d\n",i);
           
             t = data[i-offset];

             ppmarray[i+offset]=t;
           }
        
       
     }

// fprintf(stderr, "after array stuff\n");

//fprintf(stderr, "read() returned error %p\n", &data);

//concat(header,data,ppmarray);

//ppmarray = strcat (header,data);

//sprintf(ppmarray,"P6\n640 480\n255\n %p",&data);

//&ppmarray = &data-14;

//ppmarray = "P6\n640 480\n255\n";


//concat(header,data,ppmarray);




//fprintf(stderr, "test2\n");
//    FILE * pFile;
//    pFile = fopen ( "/tmp/myfile.ppm" , "w" );
//     fwrite(ppmarray, (size_t)n+15, 1, pFile);
//   // fwrite("P6\n640 480\n255\n",15,1,pFile);
//   // fwrite(data, (size_t)n, 1, pFile);
// 
//    fclose (pFile);
// //fprintf(stderr, "test\n"); 
// 
// 
// fprintf(stderr, "after file writing\n");
//   FILE * pFile;
//   long lSize;
//   char * buffer;
//   size_t result;
// 
//   pFile = fopen ( "base64.bin" , "rb" );
//   if (pFile==NULL) {fputs ("File error",stderr); exit (1);}
// 
//   // obtain file size:
//   fseek (pFile , 0 , SEEK_END);
//   lSize = ftell (pFile);
//   rewind (pFile);
// 
//   // allocate memory to contain the whole file:
//   buffer = (char*) malloc (sizeof(char)*lSize);
//   if (buffer == NULL) {fputs ("Memory error",stderr); exit (2);}
// 
//   // copy the file into the buffer:
//   result = fread (buffer,1,lSize,pFile);
//   if (result != lSize) {fputs ("Reading error",stderr); exit (3);}

  /* the whole file is now loaded in the memory buffer. */

  // terminate
//   fclose (pFile);
//   free (buffer);
//   return 0;

//   system("rawtoppm -bgr 640 480 myfile.raw > myfile.ppm");



  vtkImageImport *importer = vtkImageImport::New();
  importer->SetWholeExtent(0,639,0,479,0,0);
  importer->SetDataExtentToWholeExtent();
  importer->SetNumberOfScalarComponents(3);
  //importer->SetDataScalarTypeToShort();
  importer->SetDataScalarTypeToUnsignedChar();
  importer->SetImportVoidPointer(data);
//importer->CopyImportVoidPointer()

  vtkImageFlip *flip = vtkImageFlip::New();
  flip->SetInput(importer->GetOutput());
  flip->SetFilteredAxis(1);

//return ppmarray;
return flip->GetOutput();


}





#endif

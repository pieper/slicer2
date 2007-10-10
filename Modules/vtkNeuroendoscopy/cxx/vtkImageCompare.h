/* ====================================================================================================
 * This file compares the undistorted and distorted pictures. 
 * Provide a function to get Vectors for ITK to see the pic
 * 
 * File:          $RCSfile: vtkImageCompare.h,v $
 * Module:        SHORT DESCRIPTION OF YOUR FILE
 * Part of:       3D Slicer
 *
 * Revision:      $Revision: 1.1.2.1 $
 * Last edited:   $Date: 2007/10/10 20:08:01 $
 * Author:        $Author: ruetz $
 * Copyright:     Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.
 * 
 =====================================================================================================*/

#ifndef _VTKIMAGE_COMPARE_h
#define _VTKIMAGE_COMPARE_h
#include <vtkNeuroendoscopyConfigure.h>

#include <vtkObject.h>
#include <tsai/cal_main.h>
//#include <tsai/cal_util.c>

// #include "ImageCompare.h"
#include "vtkPoints.h"
#include <gandalf/vision.h>
#include <gandalf/vision/mask1D.h>
#include <gandalf/vision/harris_corner.h>
// #include <vtkNeuroendoscopyConfigure.h>
#include <gandalf/image.h>
#include <gandalf/image/image_defs.h>
#include <gandalf/image/io/image_io.h>
#include <gandalf/image/image_display.h>
#include <gandalf/image/pixel.h>
#include <stdio.h>


static Gan_RGBPixel_f blue = {0.0F, 0.0F, 1.0F}, green = {0.0F, 1.0F, 0.0F},
                      yellow = {1.0F, 1.0F, 0.0F}, red = {1.0F, 0.0F, 0.0F};

typedef struct tag_vector
{
  double x;
  double y;
  double z;
  
}
ImageVector;



typedef struct tag_image
{
  int width;
  int height;
  int depth;
  int dpi;
  int center;
  char *data;
  
}
Image;

class VTK_NEUROENDOSCOPY_EXPORT vtkImageCompare : public vtkObject {

public:

  static vtkImageCompare *New();
    //void PrintSelf(ostream& os, vtkIndent indent);
    
   // char *GetEndoscopyNEWVersion();
  vtkTypeMacro(vtkImageCompare, vtkObject);
  void SetOriginalPicture(char* path);
  void SetMovingPicture(char* path);
  
  void SetOriginalPictureFromArray(char * pic_arr);
  void SetMovingPictureFromArray(char * pic_arr);

//   int performUndistortion(void);
  int performUndistortion(char * outputImage);
//   void SetOutputPicture(char* path);
  vtkPoints *getFeatureCoordinates(void);
  int getNumberOfFeatures(void);
  void searchCorners(int cornersPerRow);
  void writeOutputPicture(char *path);
  int GetWidth(void);
  int GetHeight(void);


protected:

private:

  //types
  Gan_Image* org_pic;
  Gan_Image* mov_pic;
  Gan_Image* out_pic;
  Gan_Image* und_pic;
  Gan_ImageFileFormat inputFileFormat;
  Gan_ImageFileFormat outputFileFormat;
  
  char * outputPicPath;
    
  
  //functions
  double getBilinearFilteredPixel(Gan_Image *img, Gan_Image * undistImage, double u, double v, double distortion);
  void undistortImageNearest(Gan_Image *img, Gan_Image * undistImage, int width, int height, double distortion);
  void undistortImageBilinear1(Gan_Image *img, Gan_Image * undistImage, int width, int height, double distortion);
  void undistortImageBilinear1Reverse(Gan_Image *img, Gan_Image * undistImage, int width, int height, double distortion,double scale);
  Gan_ImageFileFormat fileFormatFromName(char *fileName);
  void checkImage(Gan_Image *img,char *inputImage);
  void undistortTsaiAlgo(Gan_Image *img, Gan_Image * undistImage, int width, int height, char *filename);
  Gan_CornerFeatureMap * cornerDetection(Gan_Image *img, Gan_CornerFeatureMap *cmap);
  double euclidDistance(double x, double y, double xu, double yu);
  double distanceStraightlinePoint(int startpointx, int startpointy, int endpointx, int endpointy, int checkpointx, int checkpointy);
  double distanceStraightlinePoint2(int startpointx, int startpointy, int endpointx, int endpointy, int checkpointx, int checkpointy);
  void distanceSort(Gan_CornerFeature *feature, int ncorners,int width,int height, int offset);
  void createCCFileforTSAI(Gan_CornerFeatureMap * cmap, int width, int height,char * filename, int offset);
  void createCalibPara(char * filename, char* filename2);
  Gan_Image* getUndistortedImage(void);
  ImageVector* calcDifference(ImageVector* v1, ImageVector* v2);
  double calcVecDistance(ImageVector* v1, ImageVector* v2);
  void sortDistance(Gan_CornerFeature *feature, int ncorners,int width,int height, int offset);
  void createNCCFileforTSAI(Gan_CornerFeatureMap * cmap, int width, int height,char * filename);
  Gan_CornerFeatureMap * cmap_new;
  void SetUndistortedPicture();


};

#endif

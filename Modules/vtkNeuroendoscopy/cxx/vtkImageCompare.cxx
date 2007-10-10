/* ====================================================================================================
 * This file compares the undistorted and distorted pictures. 
 * Provide a function to get Vectors for ITK to see the pic
 * 
 * File:          $RCSfile: vtkImageCompare.cxx,v $
 * Module:        SHORT DESCRIPTION OF YOUR FILE
 * Part of:       3D Slicer
 *
 * Revision:      $Revision: 1.1.2.1 $
 * Last edited:   $Date: 2007/10/10 20:07:44 $
 * Author:        $Author: ruetz $
 * Copyright:     Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.
 * 
 =====================================================================================================*/

#ifndef _VTKIMAGE_COMPARE_cxx
#define _VTKIMAGE_COMPARE_cxx

#include "vtkImageCompare.h"
#include "vtkObjectFactory.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>


vtkStandardNewMacro(vtkImageCompare);


void vtkImageCompare::undistortImageNearest(Gan_Image *img, Gan_Image * undistImage, int width, int height, double distortion)
{
  int i,j;
  int *uip,*ujp;
  unsigned char **row_ptr=(unsigned char**)undistImage->row_data_ptr;
  unsigned char **row_img_ptr=(unsigned char**)img->row_data_ptr;
  unsigned char *row;

  gan_image_get_pix_gl_uc(img, 4, 34);


   Gan_Camera CameraD;
   Gan_Vector3 v3x, v3xu;

      /* build camera with one parameter of radial distortion */
   gan_camera_build_radial_distortion_1 ( &CameraD,
                                     /*    ZH     FY     FX     Y0     X0 */
                                           100.0, 700.0, 500.0, 320.0, 240.0,
                                     /*    K1 */
                                           distortion );


   

//row by row (pixel by pixel)
  for(j=0;j<height;j++) {
//      uip = lutx[j];
//      ujp = luty[j];
    row=row_ptr[j];
    
    for(i=0;i<width;i++) {
      // The values are known to be in the image
      //if ((ui<width)&&(ui>=0)&&
      //    (uj<height)&&(uj>=0)) 
      //printf("%i,%i=%i,%i ", i,j,uip[i], ujp[i]);

      gan_vec3_fill_q ( &v3x, i, j, 100.0 );

      /* remove distortion from image point x --> xu */
      gan_camera_add_distortion_q ( &CameraD, &v3x, &v3xu );
      
      if (v3xu.x < 0 || v3xu.y < 0 || v3xu.x > 640 || v3xu.y > 480) {
        row[i]=gan_image_get_pix_gl_uc(img, 0, 0);
        
      } else {
        row[i]=gan_image_get_pix_gl_uc(img,(int) floor(v3xu.y),(int) floor(v3xu.x));
      } 
    }   
  }
}


//read from http://en.wikipedia.org/wiki/Bilinear_filtering
//
double vtkImageCompare::getBilinearFilteredPixel(Gan_Image *img, Gan_Image * undistImage, double u, double v, double distortion) {

  unsigned char value,val_ff,val_fc,val_cf,val_cc;
   u *= img->width;
   v *= img->height;
   
   Gan_Camera CameraD;
   Gan_Vector3 v3x, v3xu;

      /* build camera with one parameter of radial distortion */
   gan_camera_build_radial_distortion_1 ( &CameraD,
                                     /*    ZH     FY     FX     Y0     X0 */
                                           100.0, 700.0, 500.0, 240, 320,
                                     /*    K1 */
                                           2 );


   gan_vec3_fill_q ( &v3x, u, v, 100.0 );

   /* remove distortion from image point x --> xu */
   gan_camera_remove_distortion_q ( &CameraD, &v3x, &v3xu );
   //   printf("x coord: %f\n", v3xu.z);


   int x = (int) floor(v3xu.x);
   int y = (int) floor(v3xu.y);
   double u_ratio = u - x;
   double v_ratio = v - y;
   double u_opposite = 1 - u_ratio;
   double v_opposite = 1 - v_ratio;
   
   
   
   val_ff = gan_image_get_pix_gl_uc(img, x, y);
   val_fc = gan_image_get_pix_gl_uc(img, x, y+1);
   val_cf = gan_image_get_pix_gl_uc(img, x+1, y);
   val_cc = gan_image_get_pix_gl_uc(img, x+1, y+1);

   double result = (val_ff   * u_opposite  + val_cf   * u_ratio) * v_opposite + 
                   (val_fc * u_opposite  + val_cc * u_ratio) * v_ratio;
 
   return result;
}

void vtkImageCompare::undistortImageBilinear1(Gan_Image *img, Gan_Image * undistImage, int width, int height, double distortion) {

  int i,j;

  float *uip_f,*ujp_f;
  float *dip,*djp;
  unsigned char **row_ptr=(unsigned char**)undistImage->row_data_ptr;
  unsigned char **roworg_ptr=(unsigned char**)img->row_data_ptr;
  unsigned char *row;
  int ui_f,uj_f,ui_c,uj_c;
  float di,dj;
  unsigned char value,val_ff,val_fc,val_cf,val_cc;

  //define a camera with distortion
  Gan_Camera CameraD;
  Gan_Vector3 v3x, v3xu;

   /* build camera with one parameter of radial distortion */
  gan_camera_build_radial_distortion_1 ( &CameraD,
                                     /*    ZH     FY     FX     Y0     X0 */
                                           100.0, 700.0, 500.0, (double)(width/2),(double) (height/2),
                                     /*    K1 */
                                           distortion );


   

  
  for(j=0;j<height;j++) {
    
//     ujp_f = lutx[j];
    //ujp_f = roworg_ptr[j]
//     uip_f = luty[j];
    row=row_ptr[j];
    
    for(i=0;i<width;i++) {
      
      gan_vec3_fill_q ( &v3x, i, j, 100.0 );
      /* remove distortion from image point x --> xu */
      gan_camera_add_distortion_q ( &CameraD, &v3x, &v3xu );
      
      uj_f = (int)floor(v3xu.y);
      ui_f = (int)floor(v3xu.x);

      uj_c = uj_f+1;
      ui_c = ui_f+1;
      
      dj = v3xu.y-uj_f;
      di = v3xu.x-ui_f;

      /*if (dj!=0)
    printf("%1.30 f ",dj);
      */
      printf("calculated %f\n",v3xu.x);
      if ((ui_c<width)&&(ui_f>=0)&&
      (uj_c<height)&&(uj_f>=0)) {
    //get the value of the pixel at a certain point arount a square
    val_ff = gan_image_get_pix_gl_uc(img, uj_f, ui_f);
    val_fc = gan_image_get_pix_gl_uc(img, uj_f, ui_c);
    val_cf = gan_image_get_pix_gl_uc(img, uj_c, ui_f);
    val_cc = gan_image_get_pix_gl_uc(img, uj_c, ui_c);
    
    //calculate the 
    row[i]= val_ff+
      dj*(val_cf-val_ff)+
      di*(val_fc-val_ff)+
      dj*di*(val_ff+val_cc-val_cf-val_fc);
      }
    }
  }





}

void vtkImageCompare::undistortImageBilinear1Reverse(Gan_Image *img, Gan_Image * undistImage, int width, int height, double distortion, double scale) {

  int i,j;

  float *uip_f,*ujp_f;
  float *dip,*djp;
  unsigned char **row_ptr=(unsigned char**)undistImage->row_data_ptr;
  unsigned char **roworg_ptr=(unsigned char**)img->row_data_ptr;
  unsigned char *row;
  int ui_f,uj_f,ui_c,uj_c;
  float di,dj;
  unsigned char value,val_ff,val_fc,val_cf,val_cc;

  //define a camera with distortion
  Gan_Camera CameraD;
  Gan_Vector3 v3x, v3xu;

   /* build camera with one parameter of radial distortion */
  gan_camera_build_radial_distortion_1 ( &CameraD,
                                     /*    ZH     FY     FX     Y0     X0 */
                                           100.0, 700.0, 500.0, (double)(width/2),(double) (height/2),
                                     /*    K1 */
                                           distortion );


   

  
  for(j=0;j<height;j++) {
    
//     ujp_f = lutx[j];
    //ujp_f = roworg_ptr[j]
//     uip_f = luty[j];
    row=row_ptr[j];
    
    for(i=0;i<width;i++) {
      
      gan_vec3_fill_q ( &v3x, i, j, 100.0 );
      /* remove distortion from image point x --> xu */
      gan_camera_remove_distortion_q ( &CameraD, &v3x, &v3xu );
      
      uj_f = (int)floor(v3xu.y);
      ui_f = (int)floor(v3xu.x);

      uj_c = uj_f+1;
      ui_c = ui_f+1;
      
      dj = v3xu.y-uj_f;
      di = v3xu.x-ui_f;

      /*if (dj!=0)
    printf("%1.30 f ",dj);
      */
      //printf("calculated %f\n",v3xu.x);
      if ((ui_c<width)&&(ui_f>=0)&&
      (uj_c<height)&&(uj_f>=0)) {
    //get the value of the pixel at a certain point arount a square
    val_ff = gan_image_get_pix_gl_uc(img, uj_f, ui_f);
    val_fc = gan_image_get_pix_gl_uc(img, uj_f, ui_c);
    val_cf = gan_image_get_pix_gl_uc(img, uj_c, ui_f);
    val_cc = gan_image_get_pix_gl_uc(img, uj_c, ui_c);
    
    //calculate the 
    row[i]= val_ff+
      dj*(val_cf-val_ff)+
      di*(val_fc-val_ff)+
      dj*di*(val_ff+val_cc-val_cf-val_fc);
      }
    }
  }
//   Gan_Pixel pix;
//   pix.type = GAN_FLOAT;
//   pix.format = GAN_VECTOR_FIELD_2D;
//   pix.data.vfield2D.f.x = 1.1;
//   pix.data.vfield2D.f.y = 1.1;
//   undistImage = gan_image_scale_s(undistImage,NULL,&pix);



}



Gan_ImageFileFormat vtkImageCompare::fileFormatFromName(char *fileName) {
  
char * pointerP = rindex(fileName, 'p');

  if(pointerP!=NULL) {
    if( strlen(pointerP) <4) {
      if(strcmp(pointerP,"ppm")==0) {
    //printf("Found PPM file type.\n");
    return GAN_PPM_FORMAT;
      }
      if(strcmp(pointerP,"pgm")==0) {
    //printf("Found PGM file type.\n");
    return GAN_PGM_FORMAT;
      }
      if(strcmp(pointerP,"png")==0) {
    //printf("Found PNG file type.\n");
    return GAN_PNG_FORMAT;
      }
      if(strcmp(pointerP,"peg")==0) {
    //printf("Found JPEG file type.\n");
    return GAN_JPEG_FORMAT;
      }
      if(strcmp(pointerP,"pg")==0) {
    //printf("Found JPEG file type.\n");
    return GAN_JPEG_FORMAT;
      }
      printf("Unknown file type based on name, assuming PPM.\n");
      return GAN_PPM_FORMAT;
    }
  }

  pointerP = rindex(fileName, 't');

  if(pointerP!=NULL) {
    if(strlen(pointerP)<4) {
      if(strcmp(pointerP,"tif")==0) {
    //printf("Found TIFF file type.\n");
    return GAN_TIFF_FORMAT;
      }
    }
  }
  printf("Unknown file type based on name, assuming PPM.\n");
  return GAN_PPM_FORMAT;
}

void vtkImageCompare::checkImage(Gan_Image *img,char *inputImage) {
  if(img==NULL) {
    printf("Problem loading image '%s'.\n",inputImage);
    exit(-1);
  }
}



void vtkImageCompare::undistortTsaiAlgo(Gan_Image *img, Gan_Image * undistImage, int width, int height, char *filename) {
  int i,j;
  int *uip,*ujp;
  Gan_RGBPixel_uc  **row_ptr=(Gan_RGBPixel_uc **)undistImage->row_data_ptr;
  Gan_RGBPixel_uc  **row_img_ptr=(Gan_RGBPixel_uc **)img->row_data_ptr;
  Gan_RGBPixel_uc  *row;
  
  FILE     *data_fd;

    double    Xfd,
              Yfd,
              Xfu,
              Yfu,
              atof ();

    char      temp[256];

    if ((data_fd = fopen (filename, "r")) == NULL) {
     fprintf (stderr, "%s: unable to open file \"%s\"\n",filename,filename);
     exit (-1);
     }


    load_cp_cc_data (data_fd, &cp, &cc);
    fclose (data_fd);

    fprintf (stdout, "\n Input file: %s\n\n", filename);

    print_cp_cc_data (stdout, &cp, &cc);
    
    
//     printf("Xfu: %f\n",cp.sx);



   
//    Gan_Vector3 v3x, v3xu;

   
   

   

//row by row (pixel by pixel)
  for(j=0;j<height;j++) {
//      uip = lutx[j];
//      ujp = luty[j];
    
    row=row_ptr[j];
    
    for(i=0;i<width;i++) {
      // The values are known to be in the image
      //if ((ui<width)&&(ui>=0)&&
      //    (uj<height)&&(uj>=0)) 
      //printf("%i,%i=%i,%i ", i,j,uip[i], ujp[i]);
      

      //get the current point and mapping to undistorted coordinates
      undistorted_to_distorted_image_coord ((double) (i+1),(double) (j+1), &Xfu, &Yfu);
//         printf("Xfu: %f\n",Xfu);  
      if (Xfu < 0 || Yfu < 0 ||  Xfu >= width || Yfu >= height) {
         row[i]= gan_image_get_pix_rgb_uc(img, (unsigned int) 0, (unsigned int) 0);

        
        
      } else {
        row[i]= gan_image_get_pix_rgb_uc(img,(unsigned int) floor(Yfu),(unsigned int) floor(Xfu));
      } 
    }   
  }
    


}

Gan_CornerFeatureMap * vtkImageCompare::cornerDetection(Gan_Image *img, Gan_CornerFeatureMap *cmap) {
  
  printf("amount of corners: %x\n",&cmap);
//   Gan_CornerFeatureMap *cmap2 = cmap;
  Gan_Mask1D *mask;
  
  Gan_LocalFeatureMapParams lpms = { 20, 20, 3 };

 /* build convolution mask */
   mask = gan_gauss_mask_new ( GAN_FLOAT, 2.0, 9, 1.0, NULL );
//    cu_assert ( mask != NULL );

 /* compute corners of the image */
   cmap = gan_harris_corner_q ( img, NULL, NULL, NULL, mask, mask, 0.04F,
                                0.04F, NULL, 0, NULL, &lpms, NULL );
//    cu_assert ( cmap != NULL );
   

   return cmap;

} 


double vtkImageCompare::euclidDistance(double x, double y, double xu, double yu) {
  double result;
  //distance on the y axi
  result = sqrt((x-xu)*(x-xu) + ((y - yu)*(y - yu)));
  return result;

}


double vtkImageCompare::distanceStraightlinePoint(int startpointx, int startpointy, int endpointx, int endpointy, int checkpointx, int checkpointy) {
  double gx,gy,dx,dy,distance,cx,cy;
  double lambda; 
  
  gx = checkpointx -  startpointx; // p - ap x
  gy = checkpointy -  startpointy; // p - ap y
  dx = endpointx - startpointx; //rv x
  dy = endpointy - startpointy; //rv y
  
  
  // lambda = ((p - ap)* rv)/rv^2
  lambda = (gx*dx + gy*dy)/(dx*dx+dy*dy);
//   printf("lambda %f\n", lambda);
  cx = (checkpointx - (startpointx+lambda*dx));
  cy = (checkpointy - (startpointy+lambda*dy));
  distance = sqrt( cx*cx + cy*cy);
  return distance;
}

double vtkImageCompare::distanceStraightlinePoint2(int startpointx, int startpointy, int endpointx, int endpointy, int checkpointx, int checkpointy) {
  double gx,gy,dx,dy,distance,cx,cy, crossproduct;
  double lambda; 
  
  gx = checkpointx -  startpointx; // p - ap x
  gy = checkpointy -  startpointy; // p - ap y
  dx = endpointx - startpointx; //rv x
  dy = endpointy - startpointy; //rv y
  
  crossproduct = (gx*dy - gy*dx);
  //norm
  distance = crossproduct /sqrt(dx*dx + dy*dy);
  
  return distance;
}

//sort algorithm is TESTED only for 10 elements in a x row, but different values should work, too.
void vtkImageCompare::sortDistance(Gan_CornerFeature *feature, int ncorners,int width,int height, int offset) {

int i, j,k,end, leftBottomCornerX,leftBottomCornerY, cornerBorder;
int maxIdx,leftTopCornerIdx, leftBottomCornerIdx, rightTopCornerIdx, rightBottomCornerIdx; //indices
Gan_CornerFeature tmp; //temp feature
double oldDistance, leftTopCorner, leftBottomCorner, rightTopCorner, rightBottomCorner; //distances

end = 2*offset;
maxIdx = 0;
cornerBorder = height+width; //set the distance to the length that can be never reached
int newPoint = 0;

//before the follows check begins we have to be sure that the begining point is safe!
for (i = 0; i < ncorners;i++) {
  //find the corners for the line... after a step of ten collect new corners
  if (i % offset == 0 || i == 0) {
    
    //set the cornerborder to infinity
    //for recalculation of the distances 
    leftTopCorner = cornerBorder;
    leftBottomCorner = cornerBorder; 
    rightTopCorner= cornerBorder;
    rightBottomCorner= cornerBorder;
     
    for (j = i; j < ncorners; j++) {
      if (rightBottomCorner > euclidDistance(feature[j].c, feature[j].r,width,height)) {
        rightBottomCorner = euclidDistance(feature[j].c, feature[j].r,width,height);
        rightBottomCornerIdx =j;
        //assume that the right bottom corner is the element with the highest value
        newPoint = j;
      }
    }
    //move the right corner point to the top
    //IMPORTANT to be aware of consistency
    tmp = feature[newPoint];
    for (k =newPoint; k > i; k--) 
      feature[k] = feature[k-1];
    feature[i] = tmp;

    //if the point was moved set the pos to the moved position 
    newPoint = i;
    rightBottomCornerIdx = i;

    //set the left bottom corner position after moving the right corner to avoid inconsistency
    //needed for straight line calculation
    for (j = i; j < ncorners; j++) {
      if (leftBottomCorner  > euclidDistance(feature[j].c, feature[j].r,0,height)) {
        leftBottomCorner = euclidDistance(feature[j].c, feature[j].r,0,height);
        leftBottomCornerIdx = j;
        //set the coordinates of the leftBottomCorner to create a straight line after
        leftBottomCornerX = feature[j].c;
        leftBottomCornerY = feature[j].r;
      }
      //we don't need that just for completetion
      //as we are anyway in the loop we can collect this data as well
      //if (rightTopCorner  > euclidDistance(feature[j].c, feature[j].r,width,0)) {
      //  rightTopCorner = euclidDistance(feature[j].c, feature[j].r,width,0);
      //  rightTopCornerIdx = j;
      //  
      //}
      //if (leftTopCorner  > euclidDistance(feature[j].c, feature[j].r,0,0)) {
      //  leftTopCorner = euclidDistance(feature[j].c, feature[j].r,0,0);
      //  leftTopCornerIdx = j;
      //}
    }
    
    //if we are not at the beginning of the offset
  } else {
    //take care that the ncorners will not oversteped
    end = i+2*offset;
    if (i+(2*offset) > ncorners)
      end = ncorners;

    for (j = i; j < end;j++) {
   //create a straight line from the currentPoint safe point to the end of the field
   //assumed 10 as a good value to say that just points are they should be in a line ar in the line... 
   //experiences!!! not proofed
      if (distanceStraightlinePoint(feature[newPoint].c,feature[newPoint].r,leftBottomCornerX,
          leftBottomCornerY, feature[j].c, feature[j].r) < 10) { 
        if(euclidDistance(feature[newPoint].c, feature[newPoint].r,feature[j].c,feature[j].r) < oldDistance) {
          maxIdx = j;
          oldDistance = euclidDistance(feature[newPoint].c, feature[newPoint].r,feature[j].c,feature[j].r);
        }
      }
    }
    //the new point was found
    newPoint = maxIdx;
    //set distance again to the infinity
    oldDistance = cornerBorder;
    //lets sort the list (insertsort)
    //if we have to move a high feature in the front
    tmp = feature[maxIdx];
    for (k =maxIdx; k > i; k--) 
      feature[k] = feature[k-1];
    feature[i] = tmp;
    //set new point to the actual 
    newPoint = i;
  }
  //reset the corner distances after 10th
}

  
}
void vtkImageCompare::createNCCFileforTSAI(Gan_CornerFeatureMap * cmap, int width, int height,char * filename) {
  int i,j,k;
//   Gan_CornerFeature * corner;
//   corner = cmap->corner;
  Gan_CornerFeature * feature;
  feature = cmap->corner;
  FILE * data_fd;
  float real_coord[cmap->ncorners][2]; 
  

  
//   for (i = 0; i < cmap->ncorners; i++)
//     printf("X Coord: %d, Y Coord: %d, index: %d\n",feature[i].c,feature[i].r,i);
  j = -1;
  
  for (i = 0; i < cmap->ncorners; i++) {

    if (i % 10 == 0)
     j++;
    real_coord[i][0] = 29.9*j;
    real_coord[i][1] = 29.9*(i % 10);
    real_coord[i][2] = 0.0;

  }
  
  if ((data_fd = fopen (filename, "w")) == NULL) {
    fprintf (stderr, "%s: unable to open file \"%s\"\n",filename,filename);
    exit (-1);
  }
  
  for (i = 0; i < cmap->ncorners; i++) {
//     printf("X Coord: %d, Y Coord: %d, distance: %d\n",feature[i].c,feature[i].r,i);
    fprintf (data_fd,"%1.10e  %1.10e  %1.10e  %1.10e  %1.10e\n",  real_coord[i][0], real_coord[i][1], 0.0, (float) feature[i].c,(float) feature[i].r);
//     printf ("%1.10e  %1.10e  %1.10e  %1.10e  %1.10e\n",  real_coord[i][0], real_coord[i][1], 0.0, (float) feature[i].c,(float) feature[i].r);
   }

  fclose (data_fd);

}

void vtkImageCompare::createCalibPara(char * filename, char* filename2) {
FILE * data_fd;
int i;
    initialize_sony_xc75_matrox_parms ();

    printf ("\nCoplanar calibration (Tz, f, kappa1 optimization) \n");

    printf ("\ncamera type: %s\n", camera_type);

    /* run through all of the files on the command line */
    
    if ((data_fd = fopen (filename, "r")) == NULL) {
        printf ("%s: unable to open file \"%s\"\n", filename, filename);
        exit (-1);
    } 
    /* load up the calibration data (cd) from the given data file */
    load_cd_data (data_fd, &cd);
    fclose (data_fd);

        printf ("\ndata file: %s  (%d points)\n\n", filename, cd.point_count);

    
        /* determine the calibration constants from the 1st data file */
        coplanar_calibration_with_full_optimization();

    print_cp_cc_data (stderr, &cp, &cc);
    

    //print_error_stats (stderr);
         if ((data_fd = fopen (filename2, "w")) == NULL) {
         printf ("%s: unable to open file \"%s\"\n", filename2, filename2);
         exit (-1);
     }
     
     
         dump_cp_cc_data (data_fd, &cp, &cc);
         dump_cp_cc_data (stdout, &cp, &cc);
         fclose (data_fd);
    
}

vtkPoints *vtkImageCompare::getFeatureCoordinates(void) {
vtkPoints *point = vtkPoints::New();
int i;
point->SetDataTypeToUnsignedShort();
for (i = 0; i < this->cmap_new->ncorners;i++) {
 point->InsertNextPoint(this->cmap_new->corner[i].c,this->cmap_new->corner[i].r,0);
}
return point;

}

int vtkImageCompare::getNumberOfFeatures(void) {


return this->cmap_new->ncorners;
}

void vtkImageCompare::SetOriginalPicture(char* path) {

    this->inputFileFormat = fileFormatFromName(path);
    this->outputFileFormat = fileFormatFromName(path);  

  org_pic = gan_image_read ( path, this->inputFileFormat, NULL, NULL,NULL );
  checkImage(org_pic,path);

}

void vtkImageCompare::SetUndistortedPicture() {
   
 this->und_pic = gan_image_form_gen(NULL, org_pic->format, org_pic->type, org_pic->height,
                      org_pic->width, org_pic->stride, GAN_TRUE, NULL, 0,NULL,0);
   
 undistortTsaiAlgo(org_pic,und_pic,org_pic->width,org_pic->height,"calibdata.dat");
}
void vtkImageCompare::writeOutputPicture(char *path) {
    
    gan_image_write ( path, this->outputFileFormat, this->und_pic, NULL);
  
}
void vtkImageCompare::SetMovingPicture(char* path) {

      Gan_Camera CameraD;
      Gan_Vector3 v3x, v3xu;

      /* build camera with one parameter of radial distortion */
      gan_camera_build_radial_distortion_1 ( &CameraD,
                                       /*    ZH     FY     FX     Y0     X0 */
                                             100.0, 700.0, 500.0, 150.0, 100.0,
                                       /*    K1 */
                                             2 );

      /* build image point x assumed to have distortion */
      gan_vec3_fill_q ( &v3x, 50.0, -80.0, 100.0 );

      /* remove distortion from image point x --> xu */
      gan_camera_remove_distortion_q ( &CameraD, &v3x, &v3xu );
      printf("x coord: %f\n", v3xu.z);
}

int vtkImageCompare::GetWidth(void) {
return this->org_pic->width;

}
int vtkImageCompare::GetHeight(void) {

return this->org_pic->height;
}
void vtkImageCompare::SetOriginalPictureFromArray(char * pic_arr) {


}


void vtkImageCompare::SetMovingPictureFromArray(char * pic_arr) {


}

void vtkImageCompare::searchCorners(int cornersPerRow) {
    
    Gan_Image *img_grey;
    Gan_CornerFeatureMap * cmap;

    int width,height;
    
    width = org_pic->width;
    height = org_pic->height;
    //make a greylevel pic to find the corners
    img_grey = gan_image_convert_s ( org_pic, GAN_GREY_LEVEL_IMAGE, org_pic->type );
    //make a coordinate map to find corners
    this->cmap_new = cornerDetection(img_grey, cmap);
    //sort the Corners, need information about the picture how much are in one row
    //the algorithm need that
    sortDistance(cmap->corner, cmap->ncorners, width,height,cornersPerRow);

}




int vtkImageCompare::performUndistortion(char * outputImage) { 
   //Load image
  

    //corner extraction
   
    
   createNCCFileforTSAI(this->cmap_new,640,480,"notcoplanar.dat");

   SetUndistortedPicture();

   writeOutputPicture(outputImage);
   
}



#endif

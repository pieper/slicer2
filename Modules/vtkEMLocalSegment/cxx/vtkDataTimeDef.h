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
#ifndef __vtkDataTimeDef_h
#define __vtkDataTimeDef_h


#include <vtkEMLocalSegmentConfigure.h>
#include "vtkObject.h"

#ifndef _WIN32
#include <stdio.h>
#include <stdlib.h>
#include <iostream.h>
#include <errno.h>
#include <sys/time.h>

// ----------------------------------------------------------------------------------------------
// This file includes alll Data and time Structures  
// ----------------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------------
// Time Stop Measure up to msec  
// ----------------------------------------------------------------------------------------------
// Do not forget to define in cxx
// extern "C" {
//  struct timeval preciseTimeEnd;
//  struct timeval preciseTimeStart;
// }
extern time_t timeToComputeTime; /* In seconds */
extern time_t timeCount; /* In seconds */
extern time_t timer_Start;
extern time_t timer_Stop;

extern struct timeval preciseTimeStart;
extern struct timeval preciseTimeEnd;
extern long milliSecondDifferences;

#define START_PRECISE_TIMING \
do { \
  gettimeofday(&preciseTimeStart, NULL); \
} while (0)

#define END_PRECISE_TIMING \
do { \
  gettimeofday(&preciseTimeEnd, NULL); \
} while (0)

 
#define SHOW_ELAPSED_PRECISE_TIME \
do { \
  double uend = 1e-06*(double)preciseTimeEnd.tv_usec; \
  double ustart = 1e-06*(double)preciseTimeStart.tv_usec; \
  double end = preciseTimeEnd.tv_sec + uend;\
  double start = preciseTimeStart.tv_sec + ustart;\
  printf("Elapsed time: %g\n", (end-start) ); \
} while(0)

#define START_TIMING \
do { \
  findTimeToComputeTime(); \
  timer_Start = time((time_t *)NULL); \
} while (0)

#define END_TIMING \
do { \
  timer_Stop = time((time_t *)NULL); \
  timeCount = timer_Stop - timer_Start - timeToComputeTime; \
} while(0)


#define SHOW_ELAPSED_TIME(fp) \
do { \
  fprintf(fp,"Elapsed time: %d\n",(int)timeCount); \
} while(0)

//
//BTX - begin tcl exclude
//

#endif // _WIN32

// ----------------------------------------------------
// Stucture and function needed for convolution
// ----------------------------------------------------

// From Simon convolution.cxx
typedef struct {
  float *input;
  int nrow;
  int ncol;
  int nslice;
  float *filter;
  int *indexes;
  int numindexes;
  int M1;
  int M2;
  int N1;
  int N2;
  int O1;
  int O2;
  float *output;
  int startindex;
  int endindex;   /* Process voxels in the range [startindex, endindex) */
} convolution_filter_work;

// Function needed both in vtkDataTimeDef and vtkImageEMGeneral
// Convolution and polynomial multiplication . 
// This is assuming u and 'this' have the same dimensio
inline void convVector(float vec[], float u[], int uLen, float v[], int vLen){
  int stump = vLen /2;
  int k,j,jMin,jMax;
  int kMax = uLen + stump;
  float *uSta =u, *vSta=v;  

  for (k = stump; k <  kMax; k++) {
    *vec = 0;
    jMin = (0 > (k - vLen +1) ? 0 : (k - vLen+1) ); // max(0,k-vLen+1):
    jMax = ((k+1) < uLen ? (k+1) : uLen) ;               // min(k,uLen)+1 
    u = uSta + jMin; v = vSta + k-jMin;
    for (j=jMin; j < jMax; j++) 
      *vec += (*u++) * (*v--);
    vec ++;
  }  
}


// ----------------------------------------------------------------------------------------------
// Definitions for 3D float array EMVolume
// ----------------------------------------------------------------------------------------------/ 

// Kilian turn around dimension so it is y,x,z like in matlab ! 
class EMVolume {

protected :
  float *Data;
  int MaxX, MaxY, MaxZ, MaxXY;

  void allocate (int initZ,int initY, int initX) {
    this->MaxX  = initX;this->MaxY  = initY;this->MaxZ  = initZ;
    this->MaxXY = initX*initY;this->Data = new float[this->MaxXY*this->MaxZ];
  } 

  void deallocate () {
    if (this->Data) delete[] this->Data;
    this->Data  = NULL;this->MaxX  = 0;this->MaxY  = 0;this->MaxZ  = 0;this->MaxXY = 0;
  }

public:
  EMVolume(){this->Data  = NULL;this->MaxX  = 0;this->MaxY  = 0;this->MaxZ  = 0;this->MaxXY = 0;}
  EMVolume(int initZ,int initY, int initX) {this->allocate(initZ,initY, initX);}
  ~EMVolume() {this->deallocate();}

  void Resize(int DimZ,int DimY,int DimX) {
    if ((this->MaxX == DimX) && (this->MaxY == DimY) && (this->MaxZ == DimZ)) return;
    this->deallocate();this->allocate(DimZ,DimY,DimX);
  }

  float& operator () (int z,int y, int x) {return this->Data[x+this->MaxX*y + this->MaxXY*z];}
  const float& operator () (int z,int y, int x) const {return this->Data[x+this->MaxX*y + this->MaxXY*z];}

  EMVolume & operator = (const EMVolume &trg) {
    if ( this->Data == trg.Data ) return *this;
    if ((this->MaxX != trg.MaxX) || (this->MaxY != trg.MaxY) ||(this->MaxZ != trg.MaxZ)) {
      cerr << "Error:EMVolume & operator = : Dimesion of target was not equal to source" << endl;
    }
    int mul = this->MaxXY*this->MaxZ;
    for (int i = 0; i < mul; i++ ) this->Data[i] = trg.Data[i];
    return *this;
  }
  // Convolution in all three direction 
  // Can be made using less memory but then it will be probably be slower
  void Conv(float v[],int vLen);
  // Kilian : Just to be compatible with older version
  void Conv(double v[],int vLen);
  int ConvMultiThread(float* filter, int filterLen);
  // Start -  Multi Thread Function
  int ConvolutionFilter_workpile(float *input, float *filter, int M1, int M2, int N1, int N2, int O1, int O2);

  // End -  Multi Thread Function
  void ConvY(float v[], int vLen);
  void ConvX(float v[], int vLen);
  // Same as above only sorce and target Volume are different => Slower 
  void ConvX(EMVolume &src,float v[], int vLen);
  void ConvZ(float v[], int vLen);
  // Same as above only sorce and target Volume are different => Slower 
  void ConvZ(EMVolume &src,float v[], int vLen);
  void Print(char name[]);
  void Test(int Vdim);
  void SetValue(float val) {
    int Max = this->MaxXY*this->MaxZ;
    if (val) {for (int i = 0; i < Max; i++ ) this->Data[i] = val;}
    else { memset(this->Data, 0,sizeof(float)*Max);}
  }
}; 

// ----------------------------------------------------------------------------------------------
// Definitions for 5D float array EMTriVolume (lower triangle)
// ----------------------------------------------------------------------------------------------/ 
// It is a 5 dimensional Volume where m[y][x][][[][] y>= x is only defined 
// Lower Traingular matrix - or a symmetric matrix where you only save the lower triangle
class EMTriVolume {
protected :
  EMVolume **TriVolume;
  int Dim;

  void allocate (int initDim, int initZ,int initY, int initX) {
    this->Dim = initDim;
    int x,y;
    this->TriVolume = new EMVolume*[Dim];
    for (y=0; y < initDim; y++) {
      this->TriVolume[y] = new EMVolume[y+1];
      for (x = 0; x <= y ; x++) this->TriVolume[y][x].Resize(initZ,initY,initX);
    }
  }
  void deallocate () {
    if (this->TriVolume) {
      for (int y = 0; y < this->Dim; y++) delete[] this->TriVolume[y];
      delete[] this->TriVolume;
    }
    this->TriVolume  = NULL;this->Dim  = 0;
  }
public:
  EMTriVolume(){this->TriVolume  = NULL;this->Dim  = 0;}
  EMTriVolume(int initDim,int initZ,int initY, int initX) {this->allocate(initDim,initZ,initY, initX);}
  ~EMTriVolume() {this->deallocate();}

  void Resize(int initDim, int DimZ,int DimY,int DimX) {
    this->deallocate();this->allocate(initDim,DimZ,DimY,DimX);
  }

  float& operator () (int t1, int t2, int z,int y, int x) {return this->TriVolume[t1][t2](z,y,x);}
  const float& operator () (int t1, int t2, int z,int y, int x) const {return this->TriVolume[t1][t2](z,y,x);}

  // Kilian : Just to be complient with old version
  void Conv(double v[],int vLen) {
    float *v_f = new float[vLen];
    for (int i = 0; i < vLen; i++) v_f[i] = float(v[i]);
    this->Conv(v_f,vLen);  
    delete[] v_f;
  }

  void Conv(float v[],int vLen) {
    int x,y;
    for (y=0 ; y < this->Dim; y++) { 
      for (x = 0; x <= y; x++) this->TriVolume[y][x].Conv(v,vLen);
    }
  }
  void SetValue(float val) {
    int x,y;
    for (y=0 ; y < this->Dim; y++) { 
      for (x = 0; x <= y; x++) this->TriVolume[y][x].SetValue(val);
    }
  }
};



//
//ETX - end tcl exclude
//

// ----------------------------------------------------------------------------------------------
// Dummy class 
// ----------------------------------------------------------------------------------------------/ 
class VTK_EXPORT vtkDataTimeDef { 
public:
  static vtkDataTimeDef *New() {return (new vtkDataTimeDef);}
protected:

};

#endif

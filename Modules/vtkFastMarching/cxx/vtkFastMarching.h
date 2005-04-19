/*=auto=========================================================================

(c) Copyright 2005 Massachusetts Institute of Technology (MIT) All Rights Reserved.

This software ("3D Slicer") is provided by The Brigham and Women's 
Hospital, Inc. on behalf of the copyright holders and contributors.
Permission is hereby granted, without payment, to copy, modify, display 
and distribute this software and its documentation, if any, for  
research purposes only, provided that (1) the above copyright notice and 
the following four paragraphs appear on all copies of this software, and 
(2) that source code to any modifications to this software be made 
publicly available under terms no more restrictive than those in this 
License Agreement. Use of this software constitutes acceptance of these 
terms and conditions.

3D Slicer Software has not been reviewed or approved by the Food and 
Drug Administration, and is for non-clinical, IRB-approved Research Use 
Only.  In no event shall data or images generated through the use of 3D 
Slicer Software be used in the provision of patient care.

IN NO EVENT SHALL THE COPYRIGHT HOLDERS AND CONTRIBUTORS BE LIABLE TO 
ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL 
DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, 
EVEN IF THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE BEEN ADVISED OF THE 
POSSIBILITY OF SUCH DAMAGE.

THE COPYRIGHT HOLDERS AND CONTRIBUTORS SPECIFICALLY DISCLAIM ANY EXPRESS 
OR IMPLIED WARRANTIES INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND 
NON-INFRINGEMENT.

THE SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS 
IS." THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE NO OBLIGATION TO 
PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.


=========================================================================auto=*/
#ifndef __vtkFastMarching_h
#define __vtkFastMarching_h

#include "vtkFastMarchingConfigure.h"
#include "vtkImageData.h"
#include "vtkImageToImageFilter.h"

#ifdef _WIN32 // WINDOWS

//#include <assert.h> 
//don't use asserts to avoid program crash

#include <vector>
#include <algorithm>
#include <iostream>

#else // UNIX

#include <vector.h>
#include <algo.h>

#endif

#include "FMpdf.h"

#define MAJOR_VERSION 3
#define MINOR_VERSION 1
#define DATE_VERSION "2003-1-27/20:00EST"

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

// pretty big
#define INF 1e20 

// outside margin
#define BAND_OUT 3

#define GRANULARITY_PROGRESS 20

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

typedef enum fmstatus { fmsDONE, fmsKNOWN, fmsTRIAL, fmsFAR, fmsOUT } FMstatus;
#define MASK_BIT 256

struct FMnode {
  FMstatus status;
  float T;
  int leafIndex;
};

struct FMleaf {
  int nodeIndex;
};

// these typedef are for tclwrapper...
typedef std::vector<FMleaf> VecFMleaf;
typedef std::vector<int> VecInt;

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
class VTK_FASTMARCHING_EXPORT vtkFastMarching : public vtkImageToImageFilter
{
 private:
  bool somethingReallyWrong;

  double powerSpeed;

  int nNeighbors; // =6 pb wrap, cannot be defined as constant
  int arrayShiftNeighbor[27];
  double arrayDistanceNeighbor[27];
  int tmpNeighborhood[125]; // allocate it here so that we do not have to
  // allocate it over and over in getMedianInhomo

  float dx; 
  float dy;
  float dz;

  float invDx2;
  float invDy2;
  float invDz2;

  bool initialized;
  bool firstCall;

  FMnode *node;  // arrival time and status for all voxels
  int *inhomo; // inhomogeneity 
  int *median; // medican intensity

  short* outdata; // output
  short* indata;  // input

  // size of the indata (=size outdata, node, inhomo)
  int dimX;
  int dimY;
  int dimZ;
  int dimXY; // dimX*dimY
  int dimXYZ; // dimX*dimY*dimZ
  // coeficients of the RAS2IJK matrix
  float m11;
  float m12;
  float m13;
  float m14;

  float m21;
  float m22;
  float m23;
  float m24;

  float m31;
  float m32;
  float m33;
  float m34;

  float m41;
  float m42;
  float m43;
  float m44;

  int label;
  int depth;
  
  int nPointsEvolution;
  int nPointsBeforeLeakEvolution;
  int nEvolutions;

  VecInt knownPoints;
  // vector<int> knownPoints

  VecInt seedPoints;
  // vector<int> seedPoints

  // minheap used by the fast marching algorithm
  VecFMleaf tree;
  //  vector<FMleaf> tree;

  FMpdf *pdfIntensityIn;
  FMpdf *pdfInhomoIn;

  bool firstPassThroughShow;

  // minheap methods
  bool emptyTree(void);
  void insert(const FMleaf leaf);
  FMleaf removeSmallest( void );
  void downTree(int index);
  void upTree(int index);

  int indexFather(int index );

  void getMedianInhomo(int index, int &median, int &inhomo );

  int shiftNeighbor(int n);
  double distanceNeighbor(int n);
  float computeT(int index );
  
  void setSeed(int index );

  void collectInfoSeed(int index );
  void collectInfoAll( void );
  
  float speed(int index );

  bool minHeapIsSorted( void );

  /* perform one step of fast marching
     return the leaf which has just been added to fmsKNOWN */
  float step( void );

 public:

  void vtkErrorWrapper( char* s)
    {
      vtkErrorMacro( << s );
    };

  static vtkFastMarching *New();

  vtkTypeMacro(vtkFastMarching,vtkImageToImageFilter);

  vtkFastMarching();
  ~vtkFastMarching();
  void unInit( void );

  void PrintSelf(ostream& os, vtkIndent indent);

  //pb wrap  vtkFastMarching()(const vtkFastMarching&);
  //pb wrap  void operator=(const vtkFastMarching&);

  void init(int dimX, int dimY, int dimZ, int depth, double dx, double dy, double dz);

  void setActiveLabel(int label);

  void initNewExpansion( void );

  int nValidSeeds( void );

  void setNPointsEvolution( int n );

  void setInData(short* data);
  void setOutData(short* data);

  void setRAStoIJKmatrix(float m11, float m12, float m13, float m14,
             float m21, float m22, float m23, float m24,
             float m31, float m32, float m33, float m34,
             float m41, float m42, float m43, float m44);

  int addSeed( float r, float a, float s );

  void show(float r);
/*
  char * cxxVersionString(void)
    {
      char *text = new char[100];
      
      sprintf(text,"%d.%d \t(%s)",MAJOR_VERSION,MINOR_VERSION,DATE_VERSION);
      return text;
    }; 

   int cxxMajorVersion(void)
    {
      return MAJOR_VERSION;
    };
*/
    char * cxxVersionString(void);
    int cxxMajorVersion(void);
  void tweak(char *name, double value);

 protected:
  void ExecuteData(vtkDataObject *);

  friend void vtkFastMarchingExecute(vtkFastMarching *self,
                     vtkImageData *inData, short *inPtr,
                     vtkImageData *outData, short *outPtr, 
                     int outExt[6]);
};

#endif



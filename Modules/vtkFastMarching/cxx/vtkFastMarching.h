#ifndef __vtkFastMarching_h
#define __vtkFastMarching_h

#include "vtkFastMarchingConfigure.h"
#include "vtkImageToImageFilter.h"

#ifdef _WIN32
#include <assert.h>
#include <vector>
#include <iostream>
#else
#include <vector.h>
#include <algo.h>
#endif

#include "FMpdf.h"


///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

// pretty big
#define INF 1.0e+10 

// outside margin
#define BAND_OUT 1

#define GRANULARITY_PROGRESS 100

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
typedef std::vector<VecInt> VecVecInt;

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
class VTK_FASTMARCHING_EXPORT vtkFastMarching : public vtkImageToImageFilter
{
 private:
  unsigned int nNeighbors; // =6 pb wrap, cannot be defined as constant
  double dx; // =1

  bool initialized;
  bool firstCall;

  FMnode *node;  // arrival time and status for all voxels
  short *inhomo; // inhomogeneity 
  short *median; // medican intensity

  short* outdata; // output
  short* indata;  // input

  // size of the indata (=size outdata, node, inhomo)
  int dimX;
  int dimY;
  int dimZ;
  int dimXY; // dimX*dimY

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

  VecVecInt knownPoints;
  // vector<vector<int>> knownPoints

  VecInt seedPoints;
  // vector<int> seedPoints

  // minheap used by the fast marching algorithm
  VecFMleaf tree;
  //  vector<FMleaf> tree;

  FMpdf *pdfIntensityIn;
  FMpdf *pdfIntensityAll;
  FMpdf *pdfInhomoIn;
  FMpdf *pdfInhomoAll;

  // minheap methods
  bool emptyTree(void);
  void insert(const FMleaf leaf);
  FMleaf removeSmallest( void );
  void downTree(int index);
  void upTree(int index);

  int shiftNeighbor(int n);
  double computeT(int index );
  
  void setSeed( int index );

  void collectInfoSeed( int index );
  void collectInfoAll( void );
  
  double speed( int index );
 public:

  static vtkFastMarching *New();

  vtkTypeMacro(vtkFastMarching,vtkImageToImageFilter);

  vtkFastMarching();
  ~vtkFastMarching();
  void unInit( void );

  void PrintSelf(ostream& os, vtkIndent indent);

  //pb wrap  vtkFastMarching()(const vtkFastMarching&);
  //pb wrap  void operator=(const vtkFastMarching&);

  void init(int dimX, int dimY, int dimZ, int depth);

  void setActiveLabel(int label);

  void setNPointsEvolution( int n );

  void setInData(short* data);
  void setOutData(short* data);

  void setRAStoIJKmatrix(float m11, float m12, float m13, float m14,
             float m21, float m22, float m23, float m24,
             float m31, float m32, float m33, float m34,
             float m41, float m42, float m43, float m44);

  int addSeed( float r, float a, float s );


  /* perform one step of fast marching
     return the leaf which has just been added to fmsKNOWN */
  double step( void );

  void show(double r);

 protected:
  void ExecuteData(vtkDataObject *);

 friend void vtkFastMarchingExecute(vtkFastMarching *self,
                    vtkImageData *inData, short *inPtr,
                    vtkImageData *outData, short *outPtr, 
                    int outExt[6]);
};

#endif








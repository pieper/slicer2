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


//----------------------------------------------------------
// JIT: computes speed function from image as we need it
// ("just in time")

#define AVG 1
#define PHI 2

#define MASK_SIZE 3
// best results with 3
// mask is (2*MASK_SIZE+1) x (2*MASK_SIZE+1)

/*
  note: I really should have defined most of this class private
  but vtkWrapTcl won't take it !!
*/

struct JIT
{
  // private:

  short* data;

  unsigned char* status;
  float* _avg;
  float* _phi;

  float gaussian[2*MASK_SIZE+1][2*MASK_SIZE+1][2*MASK_SIZE+1];

  int dimX;
  int dimY;
  int dimZ;
  int dimXY;

  // max value of the volume
  int depth;

  // estimated stdev of the zone to segment
  int stdev;

  // stdev of the blurring mask (in pixels)
  float sigma;

  bool initialized;

  /*
    static int medianCompare(const void *i, const void *j)
    {
    if( (*(int*)i) > (*(int*)j) )
    return 1;

    return -1;
    }
  */

  /*  public: */
  JIT();
  JIT( int dimX, int dimY, int dimZ );

  void setDepth(int depth);
  void setStdev(int stdev);
  void setSigma(float sigma);

  void setInData( short* indata );
  void setOutData( short* indata );
 
  float value(int index); // fixme to inline !
  ~JIT();

  /*  private: */
  
  inline float avg(int index);
  inline float phi(int index);
  inline int index(int x, int y, int Z) const;


};



//----------------------------------------------------------
// FAST MARCHING CODE
//


// pretty big
#define INF 1.0e+10 

// outside margin
#define BAND_OUT 3 



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
  int nNeighbors; // =6 pb wrap, cannot be defined as constant
  double dx; // =1

  bool initialized;

  FMnode *node;

  // size of the input
  int dimX;
  int dimY;
  int dimZ;

  // dimX*dimY
  int dimXY;


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

  int xSeed;
  int ySeed;
  int zSeed;
  int label;
  

  JIT *jit;

  VecFMleaf tree;
  //  vector<FMleaf> tree;

  short* outdata;

  int nEvolutions;
  VecVecInt knownPoints;
  // vector<int> knownPoints;

  VecInt maskPoints;

  bool emptyTree(void);
  int shiftNeighbor(int n);
  double computeT(int index);
  
  double estN;
  double estM1;
  double estM2;
  bool updateEstim(int index);

  void insert(const FMleaf leaf);
  FMleaf removeSmallest( void );
  void downTree(int index);
  void upTree(int index);

 public:

  static vtkFastMarching *New();

  vtkTypeMacro(vtkFastMarching,vtkImageToImageFilter);

  vtkFastMarching();
  ~vtkFastMarching();
  void unInit( void );

  void PrintSelf(ostream& os, vtkIndent indent);

  //pb wrap  vtkFastMarching()(const vtkFastMarching&);
  //pb wrap  void operator=(const vtkFastMarching&);

  void init(int dimX, int dimY, int dimZ);

  void setSeed(void);
  void setDepth(int depth);
  void setStdev(int stdev);
  void setSigma(float sigma);

  void setInData(short* data);
  void setOutData(short* data);

  void addMask(float centerX, float centerY, float centerZ,
           float R, float theta, float phi);

  void setRAStoIJKmatrix(float m11, float m12, float m13, float m14,
             float m21, float m22, float m23, float m24,
             float m31, float m32, float m33, float m34,
             float m41, float m42, float m43, float m44);

  void show( void );

  void setSeedAndLabel(int x, int y, int z, int label);

  /*
    void showImg( short* outdata, int label );
    void showT( short* outdata, int label );
  */

  /* perform one step of fast marching
     return the leaf which has just been added to fmsKNOWN */
  double step( void );

  void back1Step( void );

 protected:
  void ExecuteData(vtkDataObject *);
};

#endif

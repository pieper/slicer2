#ifndef FMpdf_h
#define FMpdf_h

#ifdef _WIN32
#include <float.h>
#define isnan(x) _isnan(x)
#define min(a,b) __min((a),(b))
#endif

/*

  This class is used by vtkFastMarching to estimate the probability density function
  of Intensity and Inhomogeneity

 */

class FMpdf
{
  // the histogram
  int *bins;
  int realizationMax;

  // the number of realizations
  int N;

  // first 2 moments (not centered)  
  double m1;
  double m2;

  // first 2 moments (centered)
  double sigma2;
  double mean;

  bool needUpdateMoments;

  void updateMoments( void );

  double getMean( void );
  double getSigma2( void );

  double valueHisto( int k );
  double valueGauss( double k );

 public:

  FMpdf( int realizationMax );
  ~FMpdf();

  double value( double k );
  void addRealization( double k );
  void removeRealization( double k );
  bool isUnlikelyGauss( double k );
  bool isUnlikelyBigGauss( double k );

  void show( void );
};

#endif

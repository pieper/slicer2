#ifndef FMpdf_h
#define FMpdf_h

/*

  This class is used by vtkFastMarching to estimate the probability density function
  of Intensity and Inhomogeneity

 */

class FMpdf
{
  // the histogram
  unsigned int *bins;
  unsigned int realizationMax;

  // the number of realizations
  unsigned int N;

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

  double valueHisto( unsigned int k );
  double valueGauss( double k );

 public:

  FMpdf( unsigned int realizationMax );
  ~FMpdf();

  double value( double k );
  void addRealization( double k );
  void removeRealization( double k );
  bool isUnlikelyGauss( double k );
  bool isUnlikelyBigGauss( double k );

  void show( void );
};

#endif

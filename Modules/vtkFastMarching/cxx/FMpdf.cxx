#include "FMpdf.h"

FMpdf::FMpdf( unsigned int realizationMax )
{
  m1=m2=0.0;
  N=0;
  needUpdateMoments=true;

  this->realizationMax=realizationMax;

  bins = new (unsigned int) [realizationMax+1];

  assert( bins!=NULL );
  // or else there was a problem during allocation

  for(int k=0;k<=realizationMax;k++)
    bins[k]=0;
}

FMpdf::~FMpdf()
{
  delete [] bins;
}

double FMpdf::value( double k )
{
  assert( (k>=0) && (k<=realizationMax) );

  // if we have enough points then use the histogram
  // (i.e. N>50*sigma)
  if( N*N>2500*getSigma2() )
    return valueHisto( (unsigned int)k );

  // otherwise we make a gaussian assumption
  return valueGauss( k );
}

double FMpdf::valueGauss( double k )
{
  double s2=getSigma2();
  double m=getMean();

  return 1.0/sqrt(2*M_PI*s2)*exp( -0.5*(k-m)*(k-m)/s2 );
}

double FMpdf::valueHisto( unsigned int k )
{
  // note: assert( (k>=0) && (k<=realizationMax) )
  // already checked in FMpdf::value()
  if( (k>0) && (k<realizationMax) )
    return 0.3333/double(N)*( double(bins[k-1]) 
                  +double(bins[k]) 
                  +double(bins[k+1]) );

  return 1.0/double(N)*double(bins[k]);
}

void FMpdf::addRealization( double k )
{
  assert( !isnan(k) );

  m1+=k;
  m2+=(k*k);
  N++;

  if( (k>=0) && (k<=realizationMax) )
      bins[(unsigned int)k]++;

  needUpdateMoments=true;
}

void FMpdf::removeRealization( double k )
{
  assert( !isnan(k) );

  m1-=k;
  m2-=(k*k);
  N--;

  if( (k>=0) && (k<=realizationMax) )
    {
      assert( bins[(unsigned int)k]>0 );
      bins[(unsigned int)k]--;
    }

  needUpdateMoments=true;
}

bool FMpdf::isUnlikelyGauss( double k )
{
  return fabs( k-getMean() )>3.0*sqrt( getSigma2() );
}

bool FMpdf::isUnlikelyBigGauss( double k )
{
  return ( k-getMean() ) > ( 2.0*sqrt( getSigma2() ) );
}

double FMpdf::getSigma2( void )
{
  if(needUpdateMoments)
    updateMoments();

  return sigma2;
}

double FMpdf::getMean( void )
{
  if(needUpdateMoments)
    updateMoments();

  return mean;
}

void FMpdf::updateMoments( void )
{
  mean  = 1.0/double(N)*m1;
  sigma2 = (1.0/double(N)*m2)-mean*mean;
}

void FMpdf::show( void )
{
  cout << "realizationMax=" << realizationMax << endl;
  cout << "N=" << N << endl;
  cout << "mean=" << getMean() << endl;
  cout << "sqrt( sigma2 )=" << sqrt( getSigma2() ) << endl;

  for(int k=0;k<=realizationMax;k++)
    cout << bins[k] << endl;

  cout << "---" << endl;
}

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
#include "FMpdf.h"
#include <math.h>

FMpdf::FMpdf( int realizationMax )
{
  sigma2SmoothPDF=0.25;

  this->realizationMax=realizationMax;

  bins = new int [realizationMax+1];
  //  assert( bins!=NULL );
  if(!(bins!=NULL))
    {
      vtkErrorMacro("Error in vtkFastMarching, FMpdf::FMpdf(...), not enough memory for allocation of 'bins'");
      return;
    }

  smoothedBins = new double [realizationMax+1];
  //  assert( smoothedBins!=NULL );
  if(!(smoothedBins!=NULL))
    {
      vtkErrorMacro("Error in vtkFastMarching, FMpdf::FMpdf(...), not enough memory for allocation of 'smoothedBins'");
      return;
    }

  coefGauss = new double[realizationMax+1];
  //  assert( coefGauss!=NULL );
  if(!(bins!=NULL))
    {
      vtkErrorMacro("Error in vtkFastMarching, FMpdf::FMpdf(...), not enough memory for allocation of 'bins'");
      return;
    }
  
  reset();

  // default values
  memorySize=10000;
  updateRate=1000;
}

FMpdf::~FMpdf()
{
  reset(); // to empty the containers

  delete [] bins;
  delete [] smoothedBins;
  delete [] coefGauss;
}

void FMpdf::reset( void )
{
  counter=0;

  while( inBins.size()>0 )
    inBins.pop_back();

  while( toBeAdded.size()>0 )
    toBeAdded.pop_back();

  m1=m2=0.0;
  sigma2=mean=0.0;

  for(int k=0;k<=realizationMax;k++)
    bins[k]=0;  

  nRealInBins=0;
}

bool FMpdf::willUseGaussian( void )
{
  return nRealInBins<50*sqrt(sigma2);
}

double FMpdf::value( int k )
{
  if( !( (k>=0) && (k<=realizationMax) ) )
    {
      vtkErrorMacro( "Error in FMpdf::value(k)!" << endl
             << "k=" << k << " realizationMax=" 
             << realizationMax << endl );

      return valueGauss( k );
    }

  // if we have enough points then use the histogram
  if( !willUseGaussian() )
    return valueHisto( k );

  // otherwise we make a gaussian assumption
  return valueGauss( k );
}

double FMpdf::valueGauss( int k )
{
  return 1.0/sqrt(2*M_PI*sigma2)*exp( -0.5*(double(k)-mean)*(double(k)-mean)/sigma2 );
}

double FMpdf::valueHisto( int k )
{
  return smoothedBins[k];
}

void FMpdf::update( void )
{
  int r;

  // move all points from tobeadded to inbins
  while(toBeAdded.size()>0)
    {
      r=toBeAdded[toBeAdded.size()-1];
      toBeAdded.pop_back();

      inBins.push_front( r );
      bins[r]++;

      m1+=r;
      m2+=r*r;
    }

  if(memorySize!=-1)
    {
      // if inbins contains too many points, remove them
      while(inBins.size()>(unsigned)memorySize)
    {
      r=inBins[inBins.size()-1];
      inBins.pop_back();
      
      bins[r]--;
      m1-=r;
      m2-=r*r;
    }
    }

  nRealInBins=(signed)inBins.size();

  //assert( nRealInBins>0 );
  if(!( nRealInBins>0 ))
    {
      vtkErrorMacro("Error in vtkFastMarching, FMpdf::FMpdf(...), !nRealInBins>0");
      return;
    }


  // update moments
  mean=m1/double(nRealInBins);
  sigma2=m2/double(nRealInBins)-mean*mean;

  // create smoothed histogram
  double sigma2Smooth=sigma2SmoothPDF*sigma2;
  
  // create lookup table for smoothing
  for(int k=0;k<=realizationMax;k++)
    coefGauss[k]=exp(-0.5*double(k*k)/sigma2Smooth);

  {

    double val;
    double nval;
    double coef;

    for(int k=0;k<=realizationMax;k++)
      {
    val=0.0;
    nval=0.0;

    for(int j=0;j<=realizationMax;j++)
      {
        coef=coefGauss[abs(k-j)];

        val+=coef*double(bins[j]);
        nval+=coef;
      }

    smoothedBins[k]=val/nval/double(nRealInBins);
      }
  }
}

void FMpdf::addRealization( int k )
{
  //assert(finite(k)!=0);
  if(!(finite(k)!=0))
    {
      vtkErrorMacro("Error in vtkFastMarching, FMpdf::FMpdf(...), !(finite(k)!=0)");
      return;
    }

  toBeAdded.push_front(k);

  counter++;

  // update if (either or) :
  // - we have not been updated for updateRate rounds
  // - the number of points waiting to be taken into
  //   consideration  is more than half of our memory
  if( (updateRate!=-1) && 
      ( (counter%updateRate)==0 
    || (memorySize!=-1) && (toBeAdded.size()>((unsigned int)(memorySize/2))) ) )
    update();
}

/*
bool FMpdf::isUnlikelyGauss( double k )
{
  return fabs( k-getMean() )>3.0*sqrt( getSigma2() );
}

bool FMpdf::isUnlikelyBigGauss( double k )
{
  return ( k-getMean() ) > ( 2.0*sqrt( getSigma2() ) );
}
*/

void FMpdf::show( void )
{
  cout << "realizationMax=" << realizationMax << endl;
  cout << "nRealInBins=" <<  nRealInBins << endl;
  cout << "mean=" << mean << endl;
  cout << "sqrt( sigma2 )=" << sqrt( sigma2 ) << endl;

  for(int k=0;k<=realizationMax;k++)
    cout << value(k) << endl;

  cout << "---" << endl;
}

void FMpdf::setMemory( int mem )
{
  memorySize=mem;
}

void FMpdf::setUpdateRate( int rate )
{
  updateRate=rate;

  if( (updateRate!=-1) && (updateRate<10) )
    updateRate=10;
}


















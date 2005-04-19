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
#ifndef FMpdf_h
#define FMpdf_h

#ifdef _WIN32 // WINDOWS

#include <float.h>
#define isnan(x) _isnan(x)
#define finite(x) _finite(x)

#ifndef min
#define min(a,b) (((a)<(b))?(a):(b))
#endif

#ifndef M_PI
#define M_PI 3.1415926535898
#endif

#include <deque>

#else // UNIX

#ifdef _SOLARIS_
#include <math.h>
#include <ieeefp.h>
#endif
#include <deque.h>

#endif

#include <vtkObject.h>


/*

This class is used by vtkFastMarching to estimate the probability density function
of Intensity and Inhomogeneity

*/

class FMpdf : public vtkObject
{
  friend class vtkFastMarching;

  double sigma2SmoothPDF;

  int realizationMax;

  int counter;
  int memorySize; // -1=don't ever forget anything
  int updateRate;

  // the histogram
  int *bins;
  int nRealInBins;

  double *smoothedBins;

  double * coefGauss;  

  std::deque<int> inBins;
  std::deque<int> toBeAdded;

  // first 2 moments (not centered, not divided by number of samples)  
  double m1;
  double m2;

  // first 2 moments (centered)
  double sigma2;
  double mean;

  double valueHisto( int k );
  double valueGauss( int k );

 public:

  double getMean( void ) { return mean; };
  double getSigma2( void ) { return sigma2; };

  FMpdf( int realizationMax );
  ~FMpdf();

  void setMemory( int mem );
  void setUpdateRate( int rate );

  bool willUseGaussian( void );

  void reset( void );  
  void update( void );

  double value( int k );
  void addRealization( int k );

  /*
  bool isUnlikelyGauss( double k );
  bool isUnlikelyBigGauss( double k );
  */

  void show( void );

  const char* GetClassName(void)
    {return "FMpdf"; };

};

#endif


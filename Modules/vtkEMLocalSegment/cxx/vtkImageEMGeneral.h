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
// .NAME vtkImageEMGeneral
 
// The idea of vtkImageEMGeneral is to include all the tools we need for the 
// different EM Segmentation tools. The tool is put together in four parts
// - the special made header files
// - files needed so it is a vtk filter
// - Genral Math Functions 

// ------------------------------------
// Standard EM necessaties
// ------------------------------------
#ifndef __vtkImageEMGeneral_h
#define __vtkImageEMGeneral_h
#include <vtkEMLocalSegmentConfigure.h>
#include <math.h>
#include <cmath>
#include "vtkMath.h"
#include "vtkImageMultipleInputFilter.h"
#include "vtkImageThreshold.h"
#include "vtkImageMathematics.h"
#include "vtkImageAccumulate.h"
#include "vtkImageData.h"
// Just made for vtkImageEMGeneral and its kids
#include "vtkThread.h"
#include "vtkDataTimeDef.h"
#include "vtkFileOps.h" 

// ------------------------------------
// Definitions for gauss calculations
// ------------------------------------
// Abuse the type system.
#define COERCE(x, type) (*((type *)(&(x))))
// Some constants having to do with the way single
// floats are represented on alphas and sparcs
#define EMSEGMENT_MANTSIZE (23)
#define EMSEGMENT_SIGNBIT (1 << 31)
#define EMSEGMENT_EXPMASK (255 << EMSEGMENT_MANTSIZE)
#define EMSEGMENT_MENTMASK ((~EMSEGMENT_EXPMASK)&(~EMSEGMENT_SIGNBIT))
#define EMSEGMENT_PHANTOM_BIT (1 << EMSEGMENT_MANTSIZE)
#define EMSEGMENT_EXPBIAS 127
#define EMSEGMENT_SHIFTED_BIAS (EMSEGMENT_EXPBIAS << EMSEGMENT_MANTSIZE)
#define EMSEGMENT_SHIFTED_BIAS_COMP ((~ EMSEGMENT_SHIFTED_BIAS) + 1)
#define EMSEGMENT_ONE_OVER_2_PI 0.5/3.14159265358979
#define EMSEGMENT_ONE_OVER_ROOT_2_PI sqrt(EMSEGMENT_ONE_OVER_2_PI)
#define EMSEGMENT_MINUS_ONE_OVER_2_LOG_2 ((float) -.72134752)

// Definitions for Markof Field Approximation
#define EMSEGMENT_NORTH     1 
#define EMSEGMENT_SOUTH     2
#define EMSEGMENT_EAST      4 
#define EMSEGMENT_WEST      8 
#define EMSEGMENT_FIRST    16
#define EMSEGMENT_LAST     32 
#define EMSEGMENT_DEFINED  64
#define EMSEGMENT_NOTROI  128


//Definitions For Gauss Lookuptable
#define EMSEGMENT_TABLE_SIZE          64000   // For one image version
#define EMSEGMENT_TABLE_SIZE_MULTI    10000   // For multiple images
#define EMSEGMENT_TABLE_EPSILON       1e-4    // Cut of for smallest eps value: x < Eps  => GausTable(x) = 0
#define EMSEGMENT_TABLE_EPSILON_MULTI 1e-3    // Cut of for smallest eps value: x < Eps  => GausTable(x) = 0

class VTK_EXPORT vtkImageEMGeneral : public vtkImageMultipleInputFilter
{
  public:
  // -------------------------------
  // General vtk Functions
  // -------------------------------
  static vtkImageEMGeneral *New();
  vtkTypeMacro(vtkImageEMGeneral,vtkObject);
  void PrintSelf(ostream& os) { };
  void SetInputIndex(int index, vtkImageData *image) {this->SetInput(index,image);}

  void PrintMatrix(double **mat, int yMax,int xMax); 
  void PrintMatrix3D(double ***mat, int zMax,int yMax,int xMax); 

  // Description:
  // Calculated the determinant for a dim dimensional matrix -> the value is returned 
  static double determinant(double **mat,int dim); 

  // Description:
  // Inverts the matrix -> Retrns 0 if it could not do it 
  static int InvertMatrix(double **mat, double **inv_mat,int dim);

  // Description:
  // Just squares the matrix 
  static void SquareMatrix(double **Input,double **Output,int dim);

  // Description:
  // Multiplies the Matrix mat with the vector vec => res = mat * vec 
  static void MatrixVectorMulti(double **mat,double *vec,double *res,int dim);

  // Description :
  // Smoothes  3D-Matrix -> w(k) = sum(u(j)*v(k+1-j)) -> returns Matrix of size r_m
  // void smoothConv(double ***mat3D, int mat3DZlen, int mat3DYlen, int mat3DXlen, double v[],int vLen);

  // Description :
  // -------------------------------------------------------------------------------------------------------------------
  // CalculateLogMeanandLogCovariance - for multiple Image
  //
  // Input: Mu                  = Matrix with Mean values for every tissue class (row) and image (column) => [NumberOfClasses]x[NumberOfInputImages]
  //        CovMatrix           = Covariance matrix's diagonal for every tissue class (row) and image (column) => [NumberOfClasses]x[NumberOfInputImages]x[NumberOfInputImages]
  //        LogMu               = Log Mean Values calculated by the function
  //        LogCov              = Log Covariance Values calculated by the function
  //        NumberOfInputImages = Number of Input Images 
  //        NumberOfClasses     = Number of Classes 
  //        SequenceMax         = Maximum "grey value" for every image the mean and sigma value should be computed
  // 
  // Output: If return value is 0 => could not invert coveriance matrix  
  //     
  // Idea: Calculates the MuLog Vecotr and CovarianceLog Matrix given mu and CovDiag for every tissue class. To calculate MuLog 
  //       and LogCovariance we normally need samples from the image. In our case we do not have any samples, because we do not 
  //       know how the image is segmented to get around it we generate a Testsequence T where
  //       T[i] =  p(i)*log(i+1) with  
  //       p(i) = the probability that grey value i appears in the image given sigma and mu = Gauss(i, Sigma, Mu)
  //       and i ranges from [0,SequenceMax]   
  // -------------------------------------------------------------------------------------------------------------------
  static int CalculateLogMeanandLogCovariance(double **Mu, double ***CovMatrix, double **LogMu, double ***LogCov,int NumberOfInputImages, int  NumberOfClasses, int SequenceMax); 

  // Description:
  // -------------------------------------------------------------------------------------------------------------------
  // CalculateLogMeanandLogCovariance - for 1 Image
  //
  // Input: mu              = Vector with Mean values for every tissue class
  //        Sigma           = Vector with Sigma values for every tissue class
  //        logmu           = Log Mean Values calculated by the function
  //        logSigma        = Log Sigma Values calculated by the function - this case is standard deviation
  //                          or sqrt(variation)
  //        NumberOfClasses = Number of classes
  //        SequenceMax     = Maximum "grey value" the mean and sigma value should be computed
  //        
  // Idea: Calculates the MuLog and SigmaLog values given mu and sigma. To calculate MuLog and SigmaLog we 
  //       normally need samples from the image. In our case we do not have any samples, because we do not 
  //       know how the image is segmented to get around it we generate a Testsequence T where
  //       T[i] =  p(i)*log(i+1) with  
  //       p(i) = the probability that grey value i appears in the image given sigma and mu = Gauss(i, Sigma, Mu)
  //       and i ranges from [0,SequenceMax]   
  // -------------------------------------------------------------------------------------------------------------------
  static void CalculateLogMeanandLogCovariance(double *mu, double *Sigma, double *LogMu, double *LogSigma, int NumberOfClasses, int SequenceMax);

  // Description:
  // -------------------------------------------------------------------------------------------------------------------
  // CalculatingPJointDistribution
  //
  // Input: x                = set input values, x has to be of size numvar and the set variables have to be defined,
  //                           e.g. index n and m are set => y[n] and y[m] have to be defined
  //        Vleft            = all the vairables that are flexible => e.g Vleft = [1,...,n-1,n+1, .., m-1, m+1 ,..., numvar]
  //        mu               = Mean Values of the distribution of size numvar
  //        invcov           = The covariance Matrix's inverse of the distribution - has to be of size numvarxnumvar  
  //        inv_sqrt_det_cov = The covariance Matrix' determinant squared and inverted
  //        SequenceMax      = Test sequence for the different images - has to be of size numvar
  //        setvar           = How many veriable are allready fixed 
  //        numvar           = Size of Varaible Space V
  //        
  // Idea: Calculates the joint probability of the  P(i= V/Vleft: y[i] =x[i]) , e.g. P(y[n] = x[n], y[m] = x[m])  
  // Explanation: V = {y[1] ... y[numvar]}, V~ = Vleft = V / {y[n],y[m]}, and y = X <=> y element of X 
  //              =>  P(y[n] = x[n],y[m] = x[m]) = \sum{k = V~} (\sum{ l = [0..seq[y]]} P(y[n] = x[n],y[m] = x[m],y[k] = l,V~\{ y[k]}) 
  // -------------------------------------------------------------------------------------------------------------------
  static double CalculatingPJointDistribution(float* x,int *Vleft,double *mu, double **inv_cov, double inv_sqrt_det_cov,int SequenceMax, int setvar,int numvar);
  
  // -------------------------------
  // Gauss Functions
  // -------------------------------
  // Description :
  // Normal Gauss Function - pritty slow 
  static double GeneralGauss(double x,double m,double s);
  // Description :
  // Normal Gauss Function for 3D - pritty slow 
  // Input: 
  // x                = Value Vector of dimension n
  // mu               = Mean Value Vector of dimension n
  // inv_cov          = The inverse of the coveriance Matrix
  // inv_sqrt_det_cov = The covariance matrix's determinant sqrt and inverted = sqrt(inv_cov)
  static double GeneralGauss(float *x,double *mu,double **inv_cov, double inv_sqrt_det_cov,int n);

  // Description :
  // 3xfaster Gauss Function written by Sandy
  static double FastGauss(double inverse_sigma, double x);
  static double FastGaussTest(double inverse_sigma, double x);
  static float FastGauss2(double inverse_sqrt_det_covariance, float *x ,double *mu,  double **inv_cov);
  // Description :
  // Same as FastGauss - just for multi dimensional input ->  x = (vec - mu) * InvCov *(vec - mu)
  static float FastGaussMulti(double inverse_sqrt_det_covariance, float x,int dim);
  // Description :
  // Same as FastGauss - just for multi dimensional input 
  static float FastGaussMulti(double inverse_sqrt_det_covariance, float* x,double *mu, double **inv_cov, int dim);

  // Description :
  // Fastes Gauss Function (jep I wrote it) - just look in a predifend lookup table 
  static double LookupGauss(double* table, double lbound, double ubound, double resolution,double value);
  static double LookupGauss(double* table, double *lbound, double *ubound, double *resolution,double *value,int NumberOfInputImages);
  // Description:
  // Calculate the Gauss-Lookup-Table for one tissue class 
  static int CalculateGaussLookupTable(double *GaussLookupTable,double **ValueTable,double **InvCovMatrix, double InvSqrtDetCovMatrix, 
                                       double *ValueVec,int GaussTableIndex,int TableSize,int NumberOfInputImages, int index);
  // Description :
  // Calculates DICE and Jakobian Simularity Measure 
  // Value defines the vooxel with those label to be measured
  // Returns  Dice sim measure
  static float CalcSimularityMeasure (vtkImageData *Image1, vtkImageData *Image2,float val, int PrintRes);
protected:
  vtkImageEMGeneral() {};
  vtkImageEMGeneral(const vtkImageEMGeneral&) {};
  ~vtkImageEMGeneral() {};
  void DeleteVariables();
  void operator=(const vtkImageEMGeneral&) {};
  void ThreadedExecute(vtkImageData **inData, vtkImageData *outData,int outExt[6], int id){};

  // -------------------------------
  // Matrix Functions
  // -------------------------------
  // Description:
  // Writes Vector to file in Matlab format if name is specified otherwise just 
  // writes the values in the file
  // void WriteVectorToFile (FILE *f,char *name, double *vec, int xMax) const;

  // Description:
  // Writes Matrix to file in Matlab format if name is specified otherwise just 
  // writes the values in the file
  // void WriteMatrixToFile (FILE *f,char *name,double **mat, int imgY, int imgX) const;

  // Description:
  // Convolution and polynomial multiplication . 
  // This is assuming u and 'this' have the same dimension
  static void convMatrix3D(double*** mat3D, double*** U,int mat3DZlen, int mat3DYlen, int mat3DXlen, double v[],int vLen);

  // Description:
  // Convolution and polynomial multiplication . 
  // This is assuming u and 'this' have the same dimension
  // Convolution and polynomial multiplication . 
  // This is assuming u and 'this' have the same dimension
  // static void convMatrix(double** mat, double** U, int matYlen, int matXlen, double v[], int vLen);

  // Description:
  // Same just v is a row vector instead of column one
  // We use the following equation :
  // conv(U,v) = conv(U',v')' => conv(U,v') = conv(U',v)';
  // static void convMatrixT(double** mat, double** U, int matYlen, int matXlen, double v[], int vLen);

  // Description:
  // Convolution and polynomial multiplication . 
  // This is assuming u and 'this' have the same dimensio
  // static void convVector(double vec[], double u[], int uLen, double v[], int vLen);

  // Description:
  // Calculates Vector * Matrix * Vector
  static double CalculateVectorMatrixVectorOperation(double** mat, double *vec, int offY,int dimY, int offX, int dimX);

  // Description:                                                           
  // Calculates the inner product of <vec, mat[posY:dimY-1][posX]> = vec * mat[posY:dimY-1][posX]
  static double InnerproductWithMatrixY(double* vec, double **mat, int posY,int dimY, int posX);

  // Description:                                                             
  // Product of mat[posY][posX:dimY-1]*vec
  static double InnerproductWithMatrixX(double **mat, int posY,int posX, int dimX, double *vec);

  // Description:
  // -------------------------------------------------------------------------------------------------------------------
  // CalculateLogMeanandLogCovariance - for 1 Image
  //
  // Input: mu              = Vector with Mean values for every tissue class
  //        Sigma           = Vector with Sigma values for every tissue class
  //        LogMu           = Log Mean Values calculated by the function
  //        LogVariance     = Log Variance Values calculated by the function
  //        LogTestSequence = Sequence of log(i+1) with i ranges from [0,SequenceMax]
  //        SequenceMax     = Maximum "grey value" the mean and sigma value should be computed
  //        
  // Idea: Calculates the MuLog and SigmaLog values given mu and sigma. To calculate MuLog and SigmaLog we 
  //       normally need samples from the image. In our case we do not have any samples, because we do not 
  //       know how the image is segmented to get around it we generate a Testsequence T where
  //       T[i] =  p(i)*log(i+1) with  
  //       p(i) = the probability that grey value i appears in the image given sigma and mu = Gauss(i, Sigma, Mu)
  //       and i ranges from [0,SequenceMax]   
  // -------------------------------------------------------------------------------------------------------------------
  static void CalculateLogMeanandLogCovariance(double *mu, double *Sigma, double *LogMu, double *LogVariance,double *LogTestSequence, int NumberOfClasses, int SequenceMax); 
 
  void TestMatrixFunctions(int MatrixDim,int iter);
};

// ---------------------------------------------------------------------------------------------
//  -*- Mode: C++;  -*-
//  File: qgauss.hh
//  Author: Sandy Wells (sw@ai.mit.edu)
//  Copyright (C) MIT Artificial Intelligence Laboratory, 1995
// *------------------------------------------------------------------
// * FUNCTION:
// *
// * Implements an approximation to the Gaussian function.
// * It is based on a piecewise-linear approximation
// * to the 2**x function for negative arguments using integer arithmetic and
// * bit fiddling.  
// * origins: May 14, 1995, sw.  
// *
// * On an alpha qgauss is about 3 times faster than vanilla gaussian.
// * The error seems to be a six percent ripple.
// *
// * HISTORY:
// * Last edited: Nov  3 15:26 1995 (sw)
// * Created: Wed Jun  7 02:03:35 1995 (sw)
// *------------------------------------------------------------------

// A piecewise linear approximation to 2**x for negative arugments
// Provides exact results when the argument is a power of two,
// and some other times as well.
// The strategy is rougly as follows:
//    coerce the single float argument to unsigned int
//    extract the exponent as a signed integer
//    construct the mantissa, including the phantom high bit, and negate it
//    construct the result bit pattern by leftshifting the signed mantissa
//      this is done for both cases of the exponent sign
//      and check for potenital underflow

// Does no conditional branching on alpha or sparc :Jun  7, 1995

inline float qnexp2(float x)
{
    unsigned result_bits;
    unsigned bits = COERCE(x, unsigned int);
    int exponent = ((EMSEGMENT_EXPMASK & bits) >> EMSEGMENT_MANTSIZE) - (EMSEGMENT_EXPBIAS);
    int neg_mant =   - (int)((EMSEGMENT_MENTMASK & bits) | EMSEGMENT_PHANTOM_BIT);

    unsigned r1 = (neg_mant << exponent);
    unsigned r2 = (neg_mant >> (- exponent));

    result_bits = (exponent < 0) ? r2 : r1;
    result_bits = (exponent > 5) ? EMSEGMENT_SHIFTED_BIAS_COMP  : result_bits;
    
    result_bits += EMSEGMENT_SHIFTED_BIAS;

#ifdef DEBUG
    {
    float result;
    result = COERCE(result_bits, float);
    fprintf(stderr, "x %g, b %x, e %d, m %x, R %g =?",
           x,     bits, exponent,  neg_mant, pow(2.0, x));
    fflush(stderr);
    fprintf(stderr, " %g\n", result);
    }
#endif
    return(COERCE(result_bits, float));
}

// An approximation to the Gaussian function.
// The error seems to be a six percent ripple.
inline double vtkImageEMGeneral::FastGauss(double inverse_sigma, double x)
{
    float tmp = float(inverse_sigma * x);
    return (double) EMSEGMENT_ONE_OVER_ROOT_2_PI * inverse_sigma 
    * qnexp2(EMSEGMENT_MINUS_ONE_OVER_2_LOG_2 * tmp * tmp);
}

// An approximation to the Gaussian function.
// The error seems to be a six percent ripple.
inline double vtkImageEMGeneral::FastGaussTest(double inverse_sigma, double x)
{
    float tmp = float(inverse_sigma * x);
#ifndef _WIN32
    cerr << "Result " << tmp << " " << inverse_sigma * x << " " << qnexp2(EMSEGMENT_MINUS_ONE_OVER_2_LOG_2 * tmp * tmp) << endl ;
#endif
    return (double) EMSEGMENT_ONE_OVER_ROOT_2_PI * inverse_sigma 
    * qnexp2(EMSEGMENT_MINUS_ONE_OVER_2_LOG_2 * tmp * tmp);
}
// Same as FastGauss - just for 2 Dimensional multi dimensional input 
inline float vtkImageEMGeneral::FastGauss2(double inverse_sqrt_det_covariance, float *x ,double *mu,  double **inv_cov) {
  float term1 = x[0] - float(mu[0]),
         term2 = x[1] - float(mu[1]);
  // Kilian: can be done faster: term1*(inv_cov[0][0]*term1 + 2.0*inv_cov[0][1]*term2) + term2*term2*inv_cov[1][1];
  term2 = term1*(float(inv_cov[0][0])*term1 + float(inv_cov[0][1])*term2) + term2*(float(inv_cov[1][0])*term1 + float(inv_cov[1][1])*term2);
  return EMSEGMENT_ONE_OVER_2_PI * float(inverse_sqrt_det_covariance)  * qnexp2(EMSEGMENT_MINUS_ONE_OVER_2_LOG_2 * term2);
}

// Same as FastGauss - just for multi dimensional input ->  x = (vec - mu) * InvCov *(vec - mu) 
inline float vtkImageEMGeneral::FastGaussMulti(double inverse_sqrt_det_covariance, float x,int dim) {
  return pow(EMSEGMENT_ONE_OVER_ROOT_2_PI,dim) * inverse_sqrt_det_covariance * qnexp2(EMSEGMENT_MINUS_ONE_OVER_2_LOG_2 * x);
}

inline float vtkImageEMGeneral::FastGaussMulti(double inverse_sqrt_det_covariance, float* x,double *mu, double **inv_cov, int dim) {
  if (dim <2) return (float) vtkImageEMGeneral::FastGauss(inverse_sqrt_det_covariance,x[0]- float(mu[0]));
  if (dim <3) return vtkImageEMGeneral::FastGauss2(inverse_sqrt_det_covariance, x ,mu,inv_cov);
  float *x_m = new float[dim];
  float term = 0;
  int i,j; 
  for (i=0; i < dim; i++) x_m[i] = x[i] - float(mu[i]);
  for (i=0; i < dim; i++) {
    for (j=0; j < dim; j++) term += (float(inv_cov[i][j])*x_m[j]);
    term *= x_m[i];
  }
  delete []x_m;
  return vtkImageEMGeneral::FastGaussMulti(inverse_sqrt_det_covariance, term,dim);        
}

// Similar to above.  This one computes an
// approximation to the Gaussian of the square
// root of the argument, in other words, the
// argument should already be squared.

inline float qgauss_sqrt(float inverse_sigma, float x)
{
    return EMSEGMENT_ONE_OVER_ROOT_2_PI * inverse_sigma 
    * qnexp2(EMSEGMENT_MINUS_ONE_OVER_2_LOG_2 * inverse_sigma * inverse_sigma * x);
}

// ---------------------------------
// Lookup Table Gauss Function
// --------------------------------
inline double vtkImageEMGeneral::LookupGauss(double* table, double lbound, double ubound, double resolution,double value) {
  if (value < lbound) return 0.0;
  if (value >= ubound) return 0.0;
  // printf("%f %f %d \n",value,lbound,int((value-lbound)/resolution)); 
  return table[int((value-lbound)/resolution)];
}

// ---------------------------------
// Lookup Table Gauss Function for multiple Images
// --------------------------------
inline double vtkImageEMGeneral::LookupGauss(double* table, double *lbound, double *ubound, double *resolution,double *value,
                         int NumberOfInputImages ) {
  int index=0, offset = 1, i;
  for (i = 0; i< NumberOfInputImages; i++) {
    if (value[i] < lbound[i]) return 0.0;
    if (value[i] >= ubound[i]) return 0.0;
    index  += int((value[i]-lbound[i]) / resolution[i]) *offset;
    offset *= EMSEGMENT_TABLE_SIZE_MULTI;
  }
  return table[index];
}

// ---------------------------------
// Normal Gauss Function
// --------------------------------
inline double vtkImageEMGeneral::GeneralGauss(double x,double m,double s) {
  double term = (x-m)/s;
  if  (s > 0 ) return (EMSEGMENT_ONE_OVER_ROOT_2_PI/s * exp(-0.5 *term*term));
  return (m == x ? 1e20:0);
}

// ----------------------------------------
// Normal Gauss Function for Multiple Input 
// ----------------------------------------
inline double vtkImageEMGeneral::GeneralGauss(float *x,double *mu,double **inv_cov, double inv_sqrt_det_cov,int n) {
  double *x_m = new double[n];
  double term = 0;
  int i,j; 

  for (i=0; i < n; i++) x_m[i] = double(x[i]) - mu[i];
  for (i=0; i < n; i++) {
    for (j=0; j < n; j++) term += (inv_cov[i][j]*x_m[j]);
    term *= x_m[i];
  }
  delete []x_m;
  return (pow(EMSEGMENT_ONE_OVER_ROOT_2_PI,n)*inv_sqrt_det_cov * exp(-0.5 *term));
}
// -----------------------------------------
// Special Matrix function
// ----------------------------------------

// Convolution and polynomial multiplication . 
// This is assuming u and 'this' have the same dimension
inline void vtkImageEMGeneral::convMatrix3D(double*** mat3D, double*** U,int mat3DZlen, int mat3DYlen, int mat3DXlen, double v[],int vLen) {
  int stump = vLen /2;
  int k,j,jMin,jMax,x,y;
  int kMax = mat3DZlen + stump;
  double ***USta = U;
  double *vSta = v;

  for (k = stump; k <  kMax; k++) {
    for (y = 0; y < mat3DYlen; y++) {
    for (x = 0; x < mat3DXlen; x++) 
      (*mat3D)[y][x] = 0;
    }
    jMin = (0 > (k+1 - vLen) ? 0 : (k+1  - vLen));     //  max(0,k+1-vLen):
    jMax = ((k+1) < mat3DZlen ? (k+1) : mat3DZlen);     //  min(k+1,mat3DZlen) 
    // this->mat3D[k-stump] += U[j]*v[k-j];
    U = USta + jMin;  v = vSta + k-jMin; 
    for (j=jMin; j < jMax; j++) {
      for (y = 0; y < mat3DYlen; y++) {
    for (x = 0; x < mat3DXlen; x++)
      (*mat3D)[y][x] += (*U)[y][x] * (*v);
      }
      v--;
      U++;
    }
    mat3D++ ;
  }  
}

// Convolution and polynomial multiplication . 
// This is assuming u and 'this' have the same dimension
// Convolution and polynomial multiplication . 
// This is assuming u and 'this' have the same dimension
/*  inline void vtkImageEMGeneral::convMatrix(double** mat, double** U, int matYlen, int matXlen, double v[], int vLen) { */
/*    int i,j; */

/*    // => Utrans[i] represents the i column of U;     */
/*    double * Utrans  = new double[matYlen], *UtransPtr = Utrans;  */
/*    double * result  = new double[matYlen], *resultPtr = result; */

/*    for (i = 0; i < matXlen; i++) { */
/*       for (j = 0; j < matYlen; j++) *(Utrans++) = U[j][i];  */
/*       Utrans = UtransPtr;  */
/*       convVector(result,Utrans,matYlen,v,vLen); // Use the i-th Rows of Utrans;  */
/*       for (j=0; j < matYlen; j++) mat[j][i] = *(result ++); // Write result to this->mat as i-th column */
/*       result = resultPtr; */
/*    } */
/*    delete[] Utrans; */
/*    delete[] result; */
/*  }  */

// Same just v is a row vector instead of column one
// We use the following equation :
// conv(U,v) = conv(U',v')' => conv(U,v') = conv(U',v)';
/*  inline void vtkImageEMGeneral::convMatrixT(double** mat, double** U, int matYlen, int matXlen, double v[], int vLen) { */
/*    int i; */

/*    // Use the i-th Rows of U = ith Column of U'; */
/*    // write it to the i-th Row of 'this' => Transpose again */
/*    for (i = 0; i < matYlen; i++) { */
/*       convVector(mat[i],U[i],matXlen,v,vLen);   */
/*    } */
/*  }  */

// Calculated the determinant for a dim dimensional matrix -> the value is returned 
// Faster with LU decomposition - look in Numerical Recipecs
inline double vtkImageEMGeneral::determinant(double **mat,int dim) {
  if (dim < 2) return mat[0][0];
  if (dim < 3) return mat[0][0]*mat[1][1] - mat[0][1]*mat[1][0];
  if (dim < 4) return mat[0][0]*mat[1][1]*mat[2][2] + mat[1][0]*mat[2][1]*mat[0][2] + 
                      mat[2][0]*mat[0][1]*mat[1][2] - mat[0][0]*mat[2][1]*mat[1][2] - 
                      mat[1][0]*mat[0][1]*mat[2][2] - mat[2][0]*mat[1][1]*mat[0][2];
  int j,k,i;
  double result = 0;
  double **submat = new double*[dim-1];
  for (i=0; i< dim-1; i++) submat[i] = new double[dim-1];

  for (j = 0 ; j < dim ; j ++) {
    if (j < 1) {
      for (k=1 ; k < dim; k++) {
    for (i=1; i < dim; i++)
      submat[k-1][i-1] = mat[k][i];  
      }
    } else {
      for (i=1; i < dim; i++) submat[j-1][i-1] = mat[j-1][i];
    }

    result += (j%2 ? -1:1) * mat[0][j]*vtkImageEMGeneral::determinant(submat,dim-1);
  }

  for (i=0; i< dim-1; i++) delete[] submat[i];
  delete[] submat;

  return result;
}

inline int vtkImageEMGeneral::InvertMatrix(double **mat, double **inv_mat,int dim) {
  double det;
  if (dim < 2) {
    if (mat[0][0] == 0) return 0;
    inv_mat[0][0] = 1.0 / mat[0][0];
  } else {
    if (dim < 3) {
      det = vtkImageEMGeneral::determinant(mat,2);
      if (fabs(det) <  1e-15 ) return 0;
      det = 1.0 / det;
      inv_mat[0][0] = det * mat[1][1];
      inv_mat[1][1] = det * mat[0][0];
      inv_mat[0][1] = -det * mat[0][1];
      inv_mat[1][0] = -det * mat[1][0];
    } else {
      return vtkMath::InvertMatrix(mat,inv_mat,dim);
    }
  }
  return 1;
}

// Description:
// Multiplies the Matrix mat with the vector vec => res = mat * vec 
inline void vtkImageEMGeneral::MatrixVectorMulti(double **mat,double *vec,double *res,int dim) {
  int x,y;
  for (y= 0 ; y< dim; y++) {
    memset(res, 0, sizeof(double)*dim);
    for (x= 0; x<dim; x++) res[y] += mat[y][x]*vec[x];
  }
}

// -------------------------------------------------------------------------------------------------------------------
// CalculateLogMeanandLogCovariance - for 1 Image
//
// Input: mu              = Vector with Mean values for every tissue class
//        Sigma           = Vector with Sigma values for every tissue class
//        LogMu           = Log Mean Values calculated by the function
//        LogVariance     = Log Variance Values calculated by the function
//        LogTestSequence = Sequence of log(i+1) with i ranges from [0,SequenceMax]
//        SequenceMax     = Maximum "grey value" the mean and sigma value should be computed
//        
// Idea: Calculates the MuLog and SigmaLog values given mu and sigma. To calculate MuLog and SigmaLog we 
//       normally need samples from the image. In our case we do not have any samples, because we do not 
//       know how the image is segmented to get around it we generate a Testsequence T where
//       T[i] =  p(i)*log(i+1) with  
//       p(i) = the probability that grey value i appears in the image given sigma and mu = Gauss(i, Sigma, Mu)
//       and i ranges from [0,SequenceMax]   
// -------------------------------------------------------------------------------------------------------------------

inline void vtkImageEMGeneral::CalculateLogMeanandLogCovariance(double *mu, double *Sigma, double *LogMu, double *LogVariance,double *LogTestSequence, int NumberOfClasses, int SequenceMax) { 
  int i,j;
  double term;

  double *ProbSum = new double[NumberOfClasses];
  double *SigmaInverse = new double[NumberOfClasses];
  double  **ProbMatrix = new double*[NumberOfClasses];

  for (i=0; i< NumberOfClasses;i++) {
    SigmaInverse[i] = 1.0/ Sigma[i];
    ProbMatrix[i] = new double[SequenceMax];
  }

  memset(LogMu,       0, NumberOfClasses*sizeof(double));
  memset(LogVariance, 0, NumberOfClasses*sizeof(double));
  memset(ProbSum,     0, NumberOfClasses*sizeof(double));

  // The following is the same in Matlab as sum(p.*log(x+1)) with x =[0:iMax-1] and p =Gauss(x,mu,sigma)
  for (i = 0; i < SequenceMax; i++) {
    LogTestSequence[i] = log(i+1);
    for (j=0; j < NumberOfClasses; j++) {
      ProbMatrix[j][i] =  vtkImageEMGeneral::FastGauss(SigmaInverse[j],i - mu[j]);
      LogMu[j] +=  ProbMatrix[j][i]*LogTestSequence[i];
      ProbSum[j] += ProbMatrix[j][i]; 
    }
  } 
  // Normalize Mu over ProbSum
  for (i=0; i < NumberOfClasses; i++) {LogMu[i] /= ProbSum[i];}

  // The following is the same in Matlab as sqrt(sum(p.*(log(x+1)-mulog).*(log(x+1)-mulog))/psum)
  // with x =[0:iMax-1] and p =Gauss(x,mu,sigma) 
  for (i = 0; i < SequenceMax; i++) {
    for (j=0; j < NumberOfClasses; j++) {
      term = LogTestSequence[i] - LogMu[j];
      LogVariance[j] +=  ProbMatrix[j][i]*term*term;
    }
  } 
  // Take the sqrt Kilian Look it up again
  for (i=0; i < NumberOfClasses; i++) { LogVariance[i] =  LogVariance[i] / ProbSum[i]; }

  delete[] SigmaInverse;
  delete[] ProbSum;
  for (i=0; i< NumberOfClasses;i++) delete[] ProbMatrix[i] ;
  delete []ProbMatrix;
}

// -------------------------------------------------------------------------------------------------------------------
// CalculatingPJointDistribution
//
// Input: x                = set input values, x has to be of size numvar and the set variables have to be defined,
//                           e.g. index n and m are set => y[n] and y[m] have to be defined
//        Vleft            = all the vairables that are flexible => e.g Vleft = [1,...,n-1,n+1, .., m-1, m+1 ,..., numvar]
//        mu               = Mean Values of the distribution of size numvar
//        invcov           = The covariance Matrix's inverse of the distribution - has to be of size numvarxnumvar  
//        inv_sqrt_det_cov = The covariance Matrix' determinant squared and inverted
//        SequenceMax      = Test sequence for the different images - has to be of size numvar
//        setvar           = How many veriable are allready fixed 
//        numvar           = Size of Varaible Space V
//        
// Idea: Calculates the joint probability of the  P(i= V/Vleft: y[i] =x[i]) , e.g. P(y[n] = x[n], y[m] = x[m])  
// Explanation: V = {y[1] ... y[numvar]}, V~ = Vleft = V / {y[n],y[m]}, and y = X <=> y element of X 
//              =>  P(y[n] = x[n],y[m] = x[m]) = \sum{k = V~} (\sum{ l = [0..seq[y]]} P(y[n] = x[n],y[m] = x[m],y[k] = l,V~\{ y[k]}) 
// -------------------------------------------------------------------------------------------------------------------
inline double vtkImageEMGeneral::CalculatingPJointDistribution(float* x,int *Vleft,double *mu, double **inv_cov, double inv_sqrt_det_cov,int SequenceMax, int setvar,int numvar) {
  double JointProb = 0.0; 
  if (setvar == numvar) {
    if (numvar < 2) JointProb = FastGauss(inv_sqrt_det_cov, double(x[0]) - mu[0]);
      else {
    if (numvar < 3) JointProb = FastGauss2(inv_sqrt_det_cov,x, mu,inv_cov);
    else JointProb = vtkImageEMGeneral::GeneralGauss(x,mu,inv_cov,inv_sqrt_det_cov,numvar);
      }
    return JointProb;
  } 
  setvar ++;
  int index = Vleft[numvar - setvar];
  for (int i = 0 ; i < SequenceMax; i++) {
    x[index] = (float)i;
    JointProb += vtkImageEMGeneral::CalculatingPJointDistribution(x,Vleft,mu, inv_cov, inv_sqrt_det_cov,SequenceMax,setvar,numvar);
  }
  return JointProb;
}

// Description:
// Calculates : 
//     /offY->  / *\T /* * * \  /*\  <- offX \               
// dimY\        |V1|*| * M * | *|V2|         / dimX    where  vec = [* V1 *] = [* V2 *] 
//              \*/  \ * * */   \*/ 
 
inline double vtkImageEMGeneral::CalculateVectorMatrixVectorOperation(double** mat, double *vec, int offY,int dimY, int offX, int dimX) {
  double result = 0,ResMatVec;
  int y,x;
  for (y=offY; y < dimY;y++) {
    ResMatVec = 0;
    for (x=offX; x < dimX; x++) ResMatVec += mat[y][x]*vec[x];
    result += vec[y]*ResMatVec;
  }   
  return result; 
}
// Description:                                                            
// Calculates the inner product of <vec, mat[posY:dimY-1][posX]> = vec * mat[posY:dimY-1][posX]
inline double vtkImageEMGeneral::InnerproductWithMatrixY(double *vec, double **mat, int posY,int dimY, int posX) {
  double result = 0;
  for (int y = posY ; y < dimY ; y++) result += (*vec++) * mat[y][posX];
  return result;
}

// Description:                                         
// Product of mat[posY][posX:dimY-1]*vec
inline double vtkImageEMGeneral::InnerproductWithMatrixX(double **mat, int posY,int posX, int dimX,double* vec) {
  double result = 0;
  for (int x = posX ; x < dimX ; x++) result += mat[posY][x]*(*vec++);
  return result;
}

#endif








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
#include "vtkImageEMGeneral.h"
#include "vtkObjectFactory.h"
#include "vtkImageData.h"

#include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>

// ---------------------------------------------------------
// EMGeneral - Just for the Filter itself
// ---------------------------------------------------------

vtkImageEMGeneral* vtkImageEMGeneral::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkImageEMGeneral");
  if(ret)
  {
    return (vtkImageEMGeneral*)ret;
  }
  // If the factory was unable to create the object, then create it here.
  return new vtkImageEMGeneral;
}

// ---------------------------------------------------------
// Math Operation 
// ---------------------------------------------------------

void vtkImageEMGeneral::PrintMatrix(double **mat, int yMax,int xMax) {
  int i;
  for (int y = 0; y < yMax; y++) {
      for (i = 0; i < xMax; i++)
    cout << mat[y][i] << " ";
      cout << endl;
  }
  cout << endl;
}

void vtkImageEMGeneral::PrintMatrix3D(double ***mat, int zMax,int yMax,int xMax) {
  int y,x,z;
  for (z = 0; z < zMax; z++) {
    cout << "mat3D[" << z+1 << "] = ["; 
    for (y = 0; y < yMax; y++) {
      for (x = 0; x < xMax; x++)
    cout << mat[z][y][x] << " ";
      cout << ";" << endl;
    }
    cout << " ]; " << endl;
  }
}


void vtkImageEMGeneral::SquareMatrix(double **Input,double **Output,int dim) {
  int i,j,k;
  for (int i = 0 ; i < dim ; i++) {
    memset(Output[i],0,sizeof(double)*dim);
    for (int j = 0 ; j < dim ; j++) {
      for (int k = 0 ; k < dim ; k++)
    Output[i][j] += Input[k][j]*Input[i][k];
    }
  }
}

//  Smoothes  3D-Matrix
// w(k) = sum(u(j)*v(k+1-j))
// returns Matrix of size r_m
//  void vtkImageEMGeneral::smoothConv(double ***mat3D, int mat3DZlen, int mat3DYlen, int mat3DXlen, double v[],int vLen) {
//    int i,k;
  
//    double *** resultY  = new double**[mat3DZlen]; 
//    double *** resultX  = new double**[mat3DZlen];
//    for (k=0; k < mat3DZlen; k++) {
//        resultY[k]  = new double*[mat3DYlen];
//        resultX[k]  = new double*[mat3DYlen];
//        for (i=0;i < mat3DYlen; i ++) {
//      resultY[k][i]  = new double[mat3DXlen];
//      resultX[k][i]  = new double[mat3DXlen];
//        }
//    }  

//    // First: convolut in Y Direction 
//    for (i = 0; i < mat3DZlen; i++){
//      this->convMatrix(resultY[i],mat3D[i],mat3DYlen,mat3DXlen,v,vLen); 
//    }
//    // Second: convolut in X Direction 
//    for (i = 0; i < mat3DZlen; i++){
//      this->convMatrixT(resultX[i],resultY[i],mat3DYlen,mat3DXlen,v,vLen); 
//    }
//    // Third: in Z direction
//    this->convMatrix3D(mat3D,resultX,mat3DZlen,mat3DYlen,mat3DXlen,v,vLen);

//    for (k=0; k< mat3DZlen; k ++) { 
//      for (i=0; i < mat3DYlen; i++) {
//        delete[] resultY[k][i];
//        delete[] resultX[k][i];
//      }
//      delete[] resultY[k];
//      delete[] resultX[k];
//    }
//    delete[] resultY;
//    delete[] resultX;
//  }

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
int vtkImageEMGeneral::CalculateLogMeanandLogCovariance(double **Mu, double ***CovMatrix, double **LogMu, double ***LogCov,int NumberOfInputImages, int  NumberOfClasses, int SequenceMax) { 
  cout <<"vtkImageEMGeneral::CalculateLogMeanandLogCovariance start " << endl;
  int i,j,k,l,m;
  int flag = 1;
  int VleftDim = (NumberOfInputImages < 3 ? 1 : NumberOfInputImages -2);
  double inv_sqrt_det_cov;
  double JointProb, JointSum;
  double termJ; 

  int *Vleft                = new int[VleftDim];
  double *LogCovDiag        = new double[NumberOfInputImages];
  double *SqrtCovDiag       = new double[NumberOfInputImages];
  float *x                  = new float[NumberOfInputImages];
  double **inv_cov          = new double*[NumberOfInputImages];
  double *LogTestSequence  = new double[SequenceMax];
  for(i = 0; i < NumberOfInputImages; i++) { 
    inv_cov[i] =  new double[NumberOfInputImages];
  }
  // 1.) Calculate MuLog and the Diagonal Elements of the LogCoveriance Matrix
  for (i = 0; i < NumberOfClasses; i++) {
    for (j=0; j< NumberOfInputImages; j++) SqrtCovDiag[j] = sqrt(CovMatrix[i][j][j]);    
    vtkImageEMGeneral::CalculateLogMeanandLogCovariance(Mu[i], SqrtCovDiag, LogMu[i],LogCovDiag,LogTestSequence,NumberOfInputImages,SequenceMax);
    for (j = 0; j < NumberOfInputImages; j++)
      LogCov[i][j][j] = LogCovDiag[j];
  }
  // 2.) Now calculate the other values of the log covariance matrix
  // Remember: Cov[i][j]= (\sum_{c=[0..N]} \sum_{d=[0..M]} ((c - mu[i])(d- mu[j]) * P(x[i] = c ,x[j] = d))) / ( \sum_{c=[0..N]} \sum_{d=[0..M]} P(x[i] = c,x[j] = d))
  // more general :
  // Cov[i][j]= (\sum_{c=[0..N]} \sum_{d=[0..M]} ((f(c) - mu_f[i])(f(d)- mu_f[j]) * P(x[i] = c,x[j] = d))) / ( \sum_{c=[0..N]} \sum_{d=[0..M]} P(x[i]= c,x[j] = d))
  // where in our case f(x) := log(x+1) =>We (Sandy,Lilla, Dave and me) concluded this work of art after a very long and hard discussion overdays with including 
  // several offical revisions.

  for (i = 0; i < NumberOfClasses; i++) {
    // We know: size(mu) = [NumberOfClasses][NumberOfInputImages], size(cov) = [NumberOfClasses][NumberOfInputImages][NumberOfInputImages] 
    // => we just look at mu[i] (=>size(mu[i]) = [NumberOfInputImages]) and cov[i] (=>size(cov[i]) = [NumberOfInputImages][NumberOfInputImages])

    // 2.a) Calculate for the Gaussian Calculation the covariance Matrix's inverse and the inverse squared covariance matrix determinant
    if (vtkImageEMGeneral::InvertMatrix(CovMatrix[i],inv_cov, NumberOfInputImages) == 0) {
      flag = 0; // => could not invert matrix
      i =  NumberOfClasses;
      cerr << " Could not invert covariance matrix !" << endl;
    }  else { 
      inv_sqrt_det_cov = determinant(CovMatrix[i],NumberOfInputImages);
      if (inv_sqrt_det_cov <= 0.0) {
     flag = 0; // => could not invert matrix
     i =  NumberOfClasses;
     cerr << "Covariance Matrix is not positiv definit !" << endl;
      }
      inv_sqrt_det_cov = 1.0 / sqrt(inv_sqrt_det_cov);
      // 2.b ) Calculate the Joint Probabilites and Log Coveriance Matrix   
      // picking x[j]
      for (j = 0; j < NumberOfInputImages; j++) {
    // Initialise V~ for the first run the two variables are j and j+1
    for(k = 0; k < j; k++) Vleft[k] = k;
    for(k = j+2; k < NumberOfInputImages; k++) Vleft[k-2] = k;

    // picking x[k] 
    for (k = j+1; k < NumberOfInputImages; k++) {
      // Update V~ 
      if (k > j+1) Vleft[k-2] = k-1;
      // Remember covariance matrixes are symmetric => start at j+1    
      JointSum = 0;
      for (l = 0; l < SequenceMax;l++) {
        x[j] = (float)l;
        termJ = LogTestSequence[l] - LogMu[i][j];
        for (m = 0; m < SequenceMax;m++) {
          x[k] = (float)m;
          // Calculating P(x[j] = l,x[k] = m)
          JointProb = vtkImageEMGeneral::CalculatingPJointDistribution(x,Vleft,Mu[i],inv_cov,inv_sqrt_det_cov,SequenceMax,2,NumberOfInputImages);
          LogCov[i][j][k] +=  termJ * (LogTestSequence[m] - LogMu[i][k]) * JointProb;
          JointSum += JointProb;
        }
      } // End of n
      if (JointSum > 0) LogCov[i][j][k] /= JointSum;     
      LogCov[i][k][j] = LogCov[i][j][k];
    }     
      }
    }
  }
  // Delete All veriables
  delete[] Vleft;
  delete[] x;
  delete[] LogCovDiag;
  delete[] SqrtCovDiag;
  delete[] LogTestSequence;
  for(i = 0; i < NumberOfInputImages; i++) delete[] inv_cov[i];
  delete[] inv_cov;
  cout <<"vtkImageEMGeneral::CalculateLogMeanandLogCovariance end" << endl;
  return flag; // Everything went OK => flag == 1
}    

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
void vtkImageEMGeneral::CalculateLogMeanandLogCovariance(double *mu, double *Sigma, double *logmu, double *logSigma, int NumberOfClasses, int SequenceMax) { 
  double *LogTestSequence  = new double[SequenceMax];

  vtkImageEMGeneral::CalculateLogMeanandLogCovariance(mu,Sigma,logmu,logSigma,LogTestSequence,NumberOfClasses,SequenceMax);
  // -> I get the Variance - what I need for the one dimesional case is the standard deviation or sigma value for the gauss curve
  for (int k=0; k< NumberOfClasses;k++) logSigma[k] = sqrt(logSigma[k]); 

  delete[] LogTestSequence;
}

// -------------------------------------------------------------------------------------------------------------------
// CalculateGaussLookupTable
// Calculate the Gauss-Lookup-Table for one tissue class 
// -------------------------------------------------------------------------------------------------------------------
int vtkImageEMGeneral::CalculateGaussLookupTable(double *GaussLookupTable,double **ValueTable,double **InvCovMatrix, 
                            double InvSqrtDetCovMatrix, double *ValueVec,int GaussTableIndex,int TableSize,
                            int NumberOfInputImages, int index) {
  double F1,F2;
  int i;
  if (index > 0) {
    for (i=0; i < TableSize;i++) {
      ValueVec[index] = ValueTable[index][i];
      GaussTableIndex = vtkImageEMGeneral::CalculateGaussLookupTable(GaussLookupTable,ValueTable,InvCovMatrix,InvSqrtDetCovMatrix, ValueVec,
                                     GaussTableIndex,TableSize,NumberOfInputImages,index-1); 
    }
  } else {
    if (NumberOfInputImages > 1) {
      if (NumberOfInputImages > 2) {
    // for faster calculations:
    // already fixed values are Vector V, SubMatrixes S1 S2 S3, and Values a 
    // the flexible value is x
    //        T                   T T        T                                         T        T
    // => [x,V ] *|a  S1| * [ x, V ]  = [x,V] * [x* a  + S1*V ,  = X^2 *a + x*(S1*V + V *S2) + V *S3*V 
    //            |S2 S3|                        x* S2 + S3*V ] 
    //
    //                                = F1 + x*F2 + F3*x^2
    //             T                     T
    // with F1 =  V *S3*V1, F2 = S1*V + V *S2 and F3 = a
    F1 = vtkImageEMGeneral::CalculateVectorMatrixVectorOperation(InvCovMatrix,ValueVec, 1,NumberOfInputImages, 1, NumberOfInputImages);
        F2 =  vtkImageEMGeneral::InnerproductWithMatrixX(InvCovMatrix,0,1,NumberOfInputImages,ValueVec+1) 
        + vtkImageEMGeneral::InnerproductWithMatrixY(ValueVec+1,InvCovMatrix,1,NumberOfInputImages,0); 
      } else {
      F1 = ValueVec[1]*ValueVec[1]*InvCovMatrix[1][1];
      F2 = ValueVec[1]*(InvCovMatrix[0][1] + InvCovMatrix[1][0]);
      }
      for(int i=0 ; i< TableSize; i++) {
    GaussLookupTable[GaussTableIndex] = vtkImageEMGeneral::FastGaussMulti(InvSqrtDetCovMatrix, 
                                F1 + ValueTable[0][i]*(F2 + InvCovMatrix[0][0]*ValueTable[0][i]), NumberOfInputImages);
    GaussTableIndex ++;
      }

    } else {
      // Calculate Gauss Value for  the half open interval [TableLBound[i] +j/TableResolution[i],TableLBound[i] +(j+1) / TableResolution[i])
      // For mor exact measures
      // GausLookUpTable[i][j] = EMLocal3DSegmenterGauss(-HalfTableSpan + (j+0.5) * TableResolution[i],0,SigmaLog[i]);
      // => Does Not Cahnge a lot => Most influence over acccuracy EMSEGMENT_TABLE_EPSILON
      for (i= 0; i< TableSize; i++) GaussLookupTable[i] =  vtkImageEMGeneral::FastGauss(InvSqrtDetCovMatrix, ValueTable[0][i]);
      GaussTableIndex += TableSize;
    }
  }
  return GaussTableIndex;
}

void vtkImageEMGeneral::TestMatrixFunctions(int MatrixDim,int iter) {
  int i,j,k;
  double **mat = new double*[MatrixDim];
  double **out = new double*[MatrixDim];
  char name[100];
  int a;
  int NumberOfInputImages = 4, 
      NumberOfClasses = 2;
  int SequenceMax = 5000;
  double **Mu   = new double*[NumberOfClasses];
  double **LogMu = new double*[NumberOfClasses];
  double ***CovMatrix = new double**[NumberOfClasses];
  double ***LogCov    = new double**[NumberOfClasses];
  for (i=0;i < NumberOfClasses; i++) {
    Mu[i]        = new double[NumberOfInputImages];
    LogMu[i]     = new double[NumberOfInputImages];
    CovMatrix[i] = new double*[NumberOfInputImages];
    LogCov[i]    = new double*[NumberOfInputImages];
    for (j=0;j < NumberOfInputImages; j++) {
          CovMatrix[i][j] = new double[NumberOfInputImages];
      LogCov[i][j]    = new double[NumberOfInputImages];
      Mu[i][j] = (i+1)*100+j*20;
      for (k=0;k < NumberOfInputImages; k++)  CovMatrix[i][j][k] = ((j==k) ? ((i+1) + k):0.2);
    }
  }
  cout << "Calculate LogMean and Coveriance" << endl;
  cout << "Mu = [" ;
  this->PrintMatrix(Mu,NumberOfClasses,NumberOfInputImages);
  cout << "Covariance" ;
  this->PrintMatrix3D(CovMatrix,NumberOfClasses,NumberOfInputImages,NumberOfInputImages);
  this->CalculateLogMeanandLogCovariance(Mu,CovMatrix, LogMu,LogCov,NumberOfInputImages, NumberOfClasses, SequenceMax); 
  cout << "LogMu = [" ;
  this->PrintMatrix(LogMu,NumberOfClasses,NumberOfInputImages);
  cout << "LogCovariance" ;
  this->PrintMatrix3D(LogCov,NumberOfClasses,NumberOfInputImages,NumberOfInputImages);
  cout <<" Type in a number :";
  cin >> a;

  for (i=0;i < NumberOfClasses; i++) { 
    for (j=0;j < NumberOfInputImages; j++) {
      delete[] CovMatrix[i][j];
      delete[] LogCov[i][j];
    }
    delete[] Mu[i];
    delete[] LogMu[i];
    delete[] CovMatrix[i];
    delete[] LogCov[i];
  }
  delete[] Mu;
  delete[] LogMu;
  delete[] CovMatrix;
  delete[] LogCov;

  for (k=0; k<iter; k++) {
    for (i = 0; i < MatrixDim; i++) { 
      mat[i] = new double[MatrixDim];
      out[i] = new double[MatrixDim];
      for (j=1; j < MatrixDim; j++) mat[i][j] = double(int(vtkMath::Random(0,10)*100))/ 100.0; 
    }
    sprintf(name,"TestDet%d.m",k+1);
    vtkFileOps write;
    write.WriteMatrixMatlabFile(name,"mat",mat,MatrixDim,MatrixDim);
    cout << "Result of " << k << endl;
    cout <<" Determinant: " << vtkImageEMGeneral::determinant(mat,MatrixDim) << endl;
    cout <<" Square: " << endl;
    vtkImageEMGeneral::SquareMatrix(mat,out,MatrixDim);
    this->PrintMatrix(out,MatrixDim,MatrixDim);

  }

  for (i = 0; i < MatrixDim; i++) {
    delete[] mat[i];
    delete[] out[i];
  }
  delete[] mat;
  delete[] out;
}

float CountLabel(vtkImageThreshold* trash,vtkImageData * Input, float val) {
  float result;
  trash->SetInput(Input); 
  trash->ThresholdBetween(val,val);
  trash->SetInValue(1.0); 
  trash->SetOutValue(0.0);
  trash->SetOutputScalarType(Input->GetScalarType());
  trash->Update();
  vtkImageAccumulate *Accu = vtkImageAccumulate::New() ;
  Accu->SetInput(trash->GetOutput());
  Accu->SetComponentExtent(0,1,0,0,0,0);
  Accu->SetComponentOrigin(0.0,0.0,0.0); 
  Accu->SetComponentSpacing(1.0,1.0,1.0);
  Accu->Update();  
  result = Accu->GetOutput()->GetScalarComponentAsFloat(1,0,0,0);
  Accu->Delete();
  return result;
}
// Calculates DICE and Jakobian Simularity Measure 
// Value defines the vooxel with those label to be measured
// Returns  Dice sim measure
float vtkImageEMGeneral::CalcSimularityMeasure (vtkImageData *Image1, vtkImageData *Image2,float val, int PrintRes) {
  vtkImageThreshold *Trash1 =  vtkImageThreshold::New(), 
    *Trash2 =  vtkImageThreshold::New(),
    *Final =  vtkImageThreshold::New();

  vtkImageMathematics *MathImg = vtkImageMathematics::New();
  float result;
  float NumMeasure;
  float DivMeasure = CountLabel(Trash1,Image1, val); 
  DivMeasure += CountLabel(Trash2,Image2, val); 

  // Find out overlapping volume 
  MathImg->SetOperationToAdd();
  MathImg->SetInput(0,Trash1->GetOutput());
  MathImg->SetInput(1,Trash2->GetOutput());
  MathImg->Update();
  NumMeasure = CountLabel(Final,MathImg->GetOutput(),2);
  if (DivMeasure > 0) result = 2.0*NumMeasure / DivMeasure;
  else result = -1.0;
  if (PrintRes) {
    cout << "Label:                 " << val << endl; 
    cout << "Total Union Sum:       " << DivMeasure - NumMeasure << endl; 
    cout << "Total Interaction Sum: " << NumMeasure << endl;
    //  cout << "Jakobien sim measure:  " << ((DivMeasure - NumMeasure) > 0.0 ? NumMeasure / (DivMeasure - NumMeasure) : -1) << endl;
    cout << "Dice sim measure:      " << result << endl;
  }
  Trash1->Delete();
  Trash2->Delete();
  Final->Delete();
  MathImg->Delete();
  return result;
}  

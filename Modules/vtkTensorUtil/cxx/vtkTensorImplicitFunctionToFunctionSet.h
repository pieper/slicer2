/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkTensorImplicitFunctionToFunctionSet.h,v $
  Language:  C++
  Date:      $Date: 2004/08/05 00:16:56 $
  Version:   $Revision: 1.2 $
*/
// .NAME vtkTensorImplicitFunctionToFunctionSet - Wrapper class around itk::BSplineInterpolateImageFuntion
// .SECTION Description
// vtkTensorImplicitFunctionToFunctionSet


#ifndef __vtkTensorImplicitFunctionToFunctionSet_h
#define __vtkTensorImplicitFunctionToFunctionSet_h

#define VTK_INTEGRATE_MAJOR_EIGENVECTOR  0
#define VTK_INTEGRATE_MEDIUM_EIGENVECTOR 1
#define VTK_INTEGRATE_MINOR_EIGENVECTOR  2

#include "vtkTensorUtilConfigure.h"
#include "vtkImplicitFunction.h"
#include "vtkFunctionSet.h"
#include "vtkImageData.h"

class VTK_TENSORUTIL_EXPORT vtkTensorImplicitFunctionToFunctionSet : public vtkFunctionSet
{
 public:
  static vtkTensorImplicitFunctionToFunctionSet *New();
  vtkTypeRevisionMacro(vtkTensorImplicitFunctionToFunctionSet, vtkFunctionSet );
  virtual void PrintSelf(ostream& os, vtkIndent indent);
  virtual int FunctionValues(float* x, float* f);
  int GetTensor(float *x, float * f);
  virtual void AddDataSet(vtkDataSet* dataset) {}
  void AddImplicitFunction(vtkImplicitFunction * func, int numcomp ) {
    if ( numcomp < 0 || numcomp > 5 )
      return;
    else {
      TensorComponent[numcomp] = func;
    }
  }
  void SetDataBounds(vtkImageData* im) {
    im->GetExtent(DataExtent);
    im->GetOrigin(DataOrigin);
    im->GetSpacing(DataSpacing);
  }
  // Set the Integration Direction
  void SetIntegrationForward(void);
  void SetIntegrationBackward(void);
  void SetChangeIntegrationDirection(void);
  void SetIntegrationDirection(float dir[3]);

  // Description:
  // Set/get lower bound bias. If fractional anisotropy falls below this value, no regularization is done
  // ( should be set to the value of the terminal fractional anisotropy )
  vtkSetClampMacro(LowerBoundBias,float,0.0,UpperBoundBias);
  vtkGetMacro(LowerBoundBias,float);

  // Description:
  // Set/get upper bound bias. If fractional anisotropy falls below this value, regularization is done on the tensor
  vtkSetClampMacro(UpperBoundBias,float,LowerBoundBias,1.0);
  vtkGetMacro(UpperBoundBias,float);

  vtkGetMacro(LastFractionalAnisotropy,float);

  // Description:
  // Set/get the magnitude of the correction bias
  vtkSetClampMacro(CorrectionBias,float,0.0,UpperBoundBias);
  vtkGetMacro(CorrectionBias,float);
  int IsInsideImage(float x[3]);
  void GetLastEigenvalues(float v[3]) {
    memcpy(v,eigVal,3*sizeof(float));
  }
  void GetLastEigenvectors(float *v[3]) {
    for ( int i = 0 ; i < 3 ; i++ )
      memcpy(v[i],eigVec[i],3*sizeof(float));
  }
 protected:
  
  float Direction[3];
  int DirectionValid;
  int IntegrationDirection;
  float LastFractionalAnisotropy;
  float LowerBoundBias;
  float UpperBoundBias;
  float CorrectionBias;
  int DataExtent[6];
  float DataOrigin[3];
  float DataSpacing[3];

  float vec[9];
  float *eigVec[3];
  float eigVal[3];
  //BTX

  vtkImplicitFunction* TensorComponent[6];

  vtkTensorImplicitFunctionToFunctionSet()
    {
      for ( int i = 0 ; i < 6 ; i++ ) {
    TensorComponent[i]=0;
    DataExtent[i]=0;
      }
      for ( int i = 0 ; i < 3 ; i++ ) {
    DataOrigin[i]=0.0;
    DataSpacing[i]=1.0;
    eigVec[i] = &(vec[3*i]);
      }
      DirectionValid=0;
      NumFuncs=3;
      NumIndepVars=4;
      LowerBoundBias=0.0;
      UpperBoundBias=0.0;
      CorrectionBias=0.0;
      IntegrationDirection = VTK_INTEGRATE_MAJOR_EIGENVECTOR;
      SetIntegrationForward();
    }
  ~vtkTensorImplicitFunctionToFunctionSet() {}

  //ETX
  
private:
  vtkTensorImplicitFunctionToFunctionSet(const vtkTensorImplicitFunctionToFunctionSet&);  // Not implemented.
  void operator=(const vtkTensorImplicitFunctionToFunctionSet&);  // Not implemented.
};

#endif

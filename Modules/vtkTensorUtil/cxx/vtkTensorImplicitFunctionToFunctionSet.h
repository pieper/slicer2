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
/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkTensorImplicitFunctionToFunctionSet.h,v $
  Language:  C++
  Date:      $Date: 2005/04/19 22:07:04 $
  Version:   $Revision: 1.5 $
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
  virtual int FunctionValues(vtkFloatingPointType* x, vtkFloatingPointType* f);
  int GetTensor(vtkFloatingPointType *x, vtkFloatingPointType * f);
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
  void SetIntegrationDirection(vtkFloatingPointType dir[3]);

  // Description:
  // Set/get lower bound bias. If fractional anisotropy falls below this value, no regularization is done
  // ( should be set to the value of the terminal fractional anisotropy )
  vtkSetClampMacro(LowerBoundBias,vtkFloatingPointType,0.0,UpperBoundBias);
  vtkGetMacro(LowerBoundBias,vtkFloatingPointType);

  // Description:
  // Set/get upper bound bias. If fractional anisotropy falls below this value, regularization is done on the tensor
  vtkSetClampMacro(UpperBoundBias,vtkFloatingPointType,LowerBoundBias,1.0);
  vtkGetMacro(UpperBoundBias,vtkFloatingPointType);

  vtkGetMacro(LastFractionalAnisotropy,vtkFloatingPointType);

  // Description:
  // Set/get the magnitude of the correction bias
  vtkSetClampMacro(CorrectionBias,vtkFloatingPointType,0.0,UpperBoundBias);
  vtkGetMacro(CorrectionBias,vtkFloatingPointType);
  int IsInsideImage(vtkFloatingPointType x[3]);
  void GetLastEigenvalues(vtkFloatingPointType v[3]) {
    memcpy(v,eigVal,3*sizeof(vtkFloatingPointType));
  }
  void GetLastEigenvectors(vtkFloatingPointType *v[3]) {
    for ( int i = 0 ; i < 3 ; i++ )
      memcpy(v[i],eigVec[i],3*sizeof(vtkFloatingPointType));
  }
 protected:
  
  vtkFloatingPointType Direction[3];
  int DirectionValid;
  int IntegrationDirection;
  vtkFloatingPointType LastFractionalAnisotropy;
  vtkFloatingPointType LowerBoundBias;
  vtkFloatingPointType UpperBoundBias;
  vtkFloatingPointType CorrectionBias;
  int DataExtent[6];
  vtkFloatingPointType DataOrigin[3];
  vtkFloatingPointType DataSpacing[3];

  vtkFloatingPointType vec[9];
  vtkFloatingPointType *eigVec[3];
  vtkFloatingPointType eigVal[3];
  //BTX

  vtkImplicitFunction* TensorComponent[6];

  vtkTensorImplicitFunctionToFunctionSet()
    {
    int i;
    for (i = 0 ; i < 6 ; i++ ) {
    TensorComponent[i]=0;
    DataExtent[i]=0;
      }
    for (i = 0 ; i < 3 ; i++ ) {
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

/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkEstimateDiffusionTensor.h,v $
  Date:      $Date: 2007/10/29 15:42:58 $
  Version:   $Revision: 1.1.2.1.2.3 $

=========================================================================auto=*/
// .NAME vtkEstimateDiffusionTensor - 
// .SECTION Description

#ifndef __vtkEstimateDiffusionTensor_h
#define __vtkEstimateDiffusionTensor_h

#include "vtkDTMRIConfigure.h"
#include "vtkImageToImageFilter.h"
#include "vtkDoubleArray.h"
#include "vtkTransform.h"

class VTK_DTMRI_EXPORT vtkEstimateDiffusionTensor : public vtkImageToImageFilter
{
 public:
  static vtkEstimateDiffusionTensor *New();
  vtkTypeMacro(vtkEstimateDiffusionTensor,vtkImageToImageFilter);

  // Description:
  // The number of gradients is the same as the number of input
  // diffusion ImageDatas this filter will require.
  void SetNumberOfGradients(int num);
  vtkGetMacro(NumberOfGradients,int);

  // Description:
  // Set the 3-vectors describing the gradient directions
  void SetDiffusionGradient(int num, vtkFloatingPointType gradient[3])
    {
     this->DiffusionGradients->SetTuple(num,gradient);
     this->Modified();
    }
  void SetDiffusionGradient(int num, vtkFloatingPointType g0, vtkFloatingPointType g1, vtkFloatingPointType g2)
    {
     this->DiffusionGradients->SetComponent(num,0,g0);
     this->DiffusionGradients->SetComponent(num,1,g1);
     this->DiffusionGradients->SetComponent(num,2,g2);
     this->Modified();
     }
   void SetDiffusionGradients(vtkDoubleArray *grad);  
   vtkGetObjectMacro(DiffusionGradients,vtkDoubleArray);  

  // Description:
  // Get the 3-vectors describing the gradient directions
  void GetDiffusionGradient(int num,double grad[3])
  { 
    if (num<this->DiffusionGradients->GetNumberOfTuples()) {
       grad[0]=this->DiffusionGradients->GetComponent(num,0);
       grad[1]=this->DiffusionGradients->GetComponent(num,1);
       grad[2]=this->DiffusionGradients->GetComponent(num,2);
     } else {
       vtkErrorMacro("Gradient number is out of range");
     }
  }
  // the following look messy but are copied from vtkSetGet.h,
  // just adding the num parameter we need.


  void SetBValue(int num,double b)
   {
     this->BValues->SetValue(num,b);
     this->Modified();
    } 
  void SetBValues(vtkDoubleArray *bValues);  
  vtkGetObjectMacro(BValues,vtkDoubleArray);

  // Description:
  // Get Baseline Image
  vtkGetObjectMacro(Baseline,vtkImageData);

  // Description:
  // Get Average of all DWI images
  vtkGetObjectMacro(AverageDWI,vtkImageData);

   //Description
  vtkGetMacro(WeightedFitting,int);
  vtkSetMacro(WeightedFitting,int);
  vtkBooleanMacro(WeightedFitting,int);
  
  vtkGetMacro(ScaleFactor,double);
  
  // Description
  // Transformation of the tensors (for RAS coords, for example)
  // The gradient vectors are multiplied by this matrix
  vtkSetObjectMacro(Transform, vtkTransform);
  vtkGetObjectMacro(Transform, vtkTransform);

  // Description:
  // Internal class use only
  double** GetPinvA() {return this->PinvA;}
  double **GetA(){return this->A;}
  
  // Description:
  // Internal class use only
  //BTX
  void TransformDiffusionGradients();
  
  void EstimateLSTensorModel(double *dwi,double **PinvA, double D[3][3],
  double &B0);
  void EstimateWLSTensorModel(double *dwi,double **A,double **ATW2, double **invATW2A, double **ATW2A, double **localPinvA,  double D[3][3],
  double &B0);
  
  double **AllocateMatrix(int rows, int columns);
  void DeallocateMatrix(double **M,int rows, int columns);

  //ETX

 protected:
  vtkEstimateDiffusionTensor();
  ~vtkEstimateDiffusionTensor();
  vtkEstimateDiffusionTensor(const vtkEstimateDiffusionTensor&);
  void operator=(const vtkEstimateDiffusionTensor&);
  void PrintSelf(ostream& os, vtkIndent indent);

  int NumberOfGradients;

  vtkDoubleArray *BValues;
  vtkDoubleArray *DiffusionGradients;

  vtkImageData *Baseline;
  vtkImageData *AverageDWI;

  int WeightedFitting;

  // for transforming tensors
  vtkTransform *Transform;

  // Matrices for LS fitting
  double **A;
  double **PinvA;
  double ScaleFactor;
  
  void CalculatePseudoInverse();
  void CalculateA();

  void ExecuteInformation(vtkImageData *inData, vtkImageData *outData);
  void ExecuteInformation(){this->vtkImageToImageFilter::ExecuteInformation();};
  void ThreadedExecute(vtkImageData *inData, vtkImageData *outData,
        int extent[6], int id);

  // We override this in order to allocate output tensors
  // before threading happens.  This replaces the superclass 
  // vtkImageMultipleInputFilter's Execute function.
  void ExecuteData(vtkDataObject *out);



};

#endif





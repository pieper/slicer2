/*=auto=========================================================================
(c) Copyright 2003 Massachusetts Institute of Technology (MIT) All Rights Reserved.

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

#include "vtkITKRigidRegistrationTransformBase.h"

#include "vtkImageData.h"
#include "vtkObjectFactory.h"
#include "vtkImageExport.h"
#include "vtkTransform.h"
#include "vtkUnsignedIntArray.h"
#include "vtkDoubleArray.h"

#include "vtkMatrix4x4.h"
#include "vtkImageFlip.h"

#include "itkExceptionObject.h"

// itk classes
// All the MI Registration Stuff
#include "MIRegistration.h"
#include "vnl/vnl_math.h"

//----------------------------------------------------------------------------

vtkITKRigidRegistrationTransformBase::vtkITKRigidRegistrationTransformBase()
{
  // Default Images
  this->SourceImage=NULL;
  this->TargetImage=NULL;

  // The Default Parameters
  // Not all parameters are used for all Registration Algorithms
  this->SourceStandardDeviation = 0.4;
  this->TargetStandardDeviation = 0.4;
  this->TranslateScale = 320;
  this->NumberOfSamples = 50;
  this->MetricValue = 0;
  this->Matrix->Identity();

  // the last iteration finished with no error
  this->Error = 0;

  // LearningRate and NumberOfIterations at each level
  this->LearningRate          = vtkDoubleArray::New();
  this->MaxNumberOfIterations = vtkUnsignedIntArray::New();

  // Default Number of MultiResolutionLevels is 1
  this->SetNextLearningRate(0.0001);
  this->SetNextMaxNumberOfIterations(500);

  // Default Shrink Factors: No Shrink
  this->SetSourceShrinkFactors(1,1,1);
  this->SetTargetShrinkFactors(1,1,1);

  // Default: no flipping!
  this->FlipTargetZAxis = 0;
  this->ImageFlip = vtkImageFlip::New();
    this->ImageFlip->SetFilteredAxis(2);
    this->ImageFlip->FlipAboutOriginOn();
  this->ZFlipMat = vtkMatrix4x4::New();
    this->ZFlipMat->Identity();
    this->ZFlipMat->SetElement(2,2,-1);

    // The output matrix
  this->OutputMatrix = vtkMatrix4x4::New();
    this->OutputMatrix->Zero();
}

//----------------------------------------------------------------------------

vtkITKRigidRegistrationTransformBase::~vtkITKRigidRegistrationTransformBase()
{
  if(this->SourceImage)
    {
    this->SourceImage->Delete();
    }
  if(this->TargetImage)
    { 
    this->TargetImage->Delete();
    }
  this->LearningRate->Delete();
  this->MaxNumberOfIterations->Delete();
  this->ImageFlip->Delete();
  this->ZFlipMat->Delete();
}

//----------------------------------------------------------------------------

void vtkITKRigidRegistrationTransformBase::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

  os << "SourceStandardDeviation: " << this->SourceStandardDeviation  << endl;
  os << "TargetStandardDeviation: " << this->SourceStandardDeviation  << endl;
  os << "TranslateScale: " << this->TranslateScale  << endl;
  os << "NumberOfSamples: " << this->NumberOfSamples  << endl;
  os << "MetricValue: " << this->MetricValue  << endl;

  os << "Source Shrink: "       << SourceShrink[0] << ' '
     << SourceShrink[1] << ' '  << SourceShrink[2] << endl;
  os << "Target Shrink: "       << TargetShrink[0] << ' '
     << TargetShrink[1] << ' '  << TargetShrink[2] << endl;

  os << "NumberOfIterations: " << this->MaxNumberOfIterations  << endl;
    this->MaxNumberOfIterations->PrintSelf(os,indent.GetNextIndent());
  os << "LearningRate: "       << this->LearningRate  << endl;
    this->LearningRate->PrintSelf(os,indent.GetNextIndent());

   os << "Flip Target Z Axis? " << this->FlipTargetZAxis << endl;
   os << "Image Flipping:" << this->ImageFlip << endl;
   this->ImageFlip->PrintSelf(os,indent.GetNextIndent());
   os << "ZFlipMat" << this->ZFlipMat << endl;
   this->ZFlipMat->PrintSelf(os,indent.GetNextIndent());
   os << "OutputMatrix" << this->OutputMatrix << endl;
   this->OutputMatrix->PrintSelf(os,indent.GetNextIndent());

  os << "SourceImage: " << this->SourceImage << endl;
  if(this->SourceImage)
    {
    this->SourceImage->PrintSelf(os,indent.GetNextIndent());
    }
  os << "TargetImage: " << this->TargetImage << endl;
  if(this->TargetImage)
    {
    this->TargetImage->PrintSelf(os,indent.GetNextIndent());
    }
}

//----------------------------------------------------------------------------

vtkImageData *vtkITKRigidRegistrationTransformBase::GetPossiblyFlippedTargetImage() 
{
  if (this->FlipTargetZAxis)
    {
      std::cout << "Z-Flipping Target Input" << std::endl;
      this->ImageFlip->SetInput(TargetImage);
      this->ImageFlip->Update();
      return this->ImageFlip->GetOutput();
    }
  return this->TargetImage;
}

//----------------------------------------------------------------------------

//// This templated function executes the filter for any type of data.
//// But, actually we use only float...
//template <class T>
//static void vtkITKRigidRegistrationExecute(vtkITKRigidRegistrationTransformBase *self,
//                 vtkImageData *source,
//                 vtkImageData *target,
//                 vtkMatrix4x4 *matrix,
//                 T vtkNotUsed(dummy))
//{
//  // Declare the input and output types
//  typedef itk::Image<T,3>                       OutputImageType;
//
//  // ----------------------------------------
//  // Sources to ITK MIRegistration
//  // ----------------------------------------
//
//  // Create the Registrator
//  typedef itk::MIRegistration<OutputImageType,OutputImageType> RegistratorType;
//  typename RegistratorType::Pointer MIRegistrator = RegistratorType::New();
//
// MIRegistrator->Initialize(self);
//
//  MIRegistrator->SetMovingImageStandardDeviation(self->GetSourceStandardDeviation());
//  MIRegistrator->SetFixedImageStandardDeviation(self->GetTargetStandardDeviation());
//  MIRegistrator->SetNumberOfSpatialSamples(self->GetNumberOfSamples());
//
//  //
//  // Start registration
//  //
//  try { MIRegistrator->Execute(); }
//  catch( itk::ExceptionObject & err )
//    {
//    std::cout << "Caught an exception: " << std::endl;
//    std::cout << err << std::endl;
//    self->SetError(1);
//    return;
//    }
//
//  MIRegistrator->ResultsToMatrix(matrix);
//
//  // Get the Value of the agreement
//  self->SetMetricValue(MIRegistrator->GetMetricValue());
//  // self->SetMetricValue(optimizer->GetValue());
//
//  // the last iteration finished with no error
//  self->SetError(0);
//
//  self->Modified();
//}

//----------------------------------------------------------------------------
// Update the 4x4 matrix. Updates are only done as necessary.
 
//void vtkITKRigidRegistrationTransformBase::InternalUpdate()
//{
//
//  if (this->SourceImage == NULL || this->TargetImage == NULL)
//    {
//    this->Matrix->Identity();
//    return;
//    }
//
//  if (MaxNumberOfIterations->GetNumberOfTuples() != 
//         LearningRate->GetNumberOfTuples())
//    vtkErrorMacro (<< MaxNumberOfIterations->GetNumberOfTuples() 
//    << "is the number of levels of iterations"
//    << LearningRate->GetNumberOfTuples() 
//    << "is the number of levels of learning rates. "
//    << "the two numbers should be the same");
//
//  if (this->SourceImage->GetScalarType() != VTK_FLOAT)
//    {
//    vtkErrorMacro (<< "Source type " << this->SourceImage->GetScalarType()
//             << "must be float");
//    this->Matrix->Identity();
//    return;
//    }
//
//  if (this->TargetImage->GetScalarType() != VTK_FLOAT)
//    {
//    vtkErrorMacro (<< "Target type " << this->SourceImage->GetScalarType()
//             << "must be float");
//    this->Matrix->Identity();
//    return;
//    }
//
//  float dummy = 0.0;
//  vtkITKRigidRegistrationExecute(this,
//                   this->SourceImage,
//                   this->TargetImage,
//                   this->Matrix,
//                   dummy);
//}

//------------------------------------------------------------------------

void vtkITKRigidRegistrationTransformBase::Initialize(vtkMatrix4x4 *mat)
{
  this->Matrix->DeepCopy(mat);

  // Do we need the flip?
  if (mat->Determinant()<0)
    {
      std::cout << "Z-Flipping Input Matrix" << std::endl;
      this->FlipTargetZAxis = 1;
      vtkMatrix4x4::Multiply4x4(mat,this->ZFlipMat,this->Matrix);
    }
  else
    {
      this->FlipTargetZAxis = 0;
      this->Matrix->DeepCopy(mat);
    }
}

//------------------------------------------------------------------------

int vtkITKRigidRegistrationTransformBase::TestMatrixInitialize(vtkMatrix4x4 *aMat)
{
  // Initialize
  typedef itk::Image<float,3>                       OutputImageType;
  typedef itk::MIRegistration<OutputImageType,OutputImageType> RegistratorType;
  RegistratorType::Pointer MIRegistrator = RegistratorType::New();

  MIRegistrator->InitializeRegistration(aMat);

  // A TEST!!!
  {
    vtkMatrix4x4 *matt = vtkMatrix4x4::New();
    MIRegistrator->ParamToMatrix(MIRegistrator->GetInitialParameters(),matt);
    double diff = 0.0;
    for(int ii =0;ii<4;ii++)
      for(int jj=0;jj<4;jj++)
        diff += ((aMat->GetElement(ii,jj) - matt->GetElement(ii,jj))*
                 (aMat->GetElement(ii,jj) - matt->GetElement(ii,jj)));
    if (diff > 1e-6)
      {
        MIRegistrator->Print(std::cout);
        std::cout << "Was unable to set initial matricies accurately" << std::endl;
        std::cout << "Error was : " << diff << std::endl;
        std::cout << "Printing initially set matrix" << endl;
        aMat->Print(std::cout);
        std::cout << "Printing actually set matrix" << endl;
        matt->Print(std::cout);
        matt->Delete();
        return -1;
      }
  }
  return MIRegistrator->TestParamToMatrix();
}

//------------------------------------------------------------------------

vtkMatrix4x4 *vtkITKRigidRegistrationTransformBase::GetOutputMatrix()
{
  if(this->FlipTargetZAxis)
    {
      std::cout << "Z-Flipping Output Matrix" << std::endl;
      vtkMatrix4x4::Multiply4x4(this->Matrix,this->ZFlipMat,
                                this->OutputMatrix);
    }
  else
    {
      this->OutputMatrix->DeepCopy(this->Matrix);
    }
  return this->OutputMatrix;
}
//------------------------------------------------------------------------
unsigned long vtkITKRigidRegistrationTransformBase::GetMTime()
{
  unsigned long result = this->Superclass::GetMTime();
  unsigned long mtime;

  if (this->SourceImage)
    {
    mtime = this->SourceImage->GetMTime(); 
    if (mtime > result)
      {
      result = mtime;
      }
    }
  if (this->TargetImage)
    {
    mtime = this->TargetImage->GetMTime();
    if (mtime > result)
      {
      result = mtime;
      }
    }
  return result;
}

//----------------------------------------------------------------------------

void vtkITKRigidRegistrationTransformBase::InternalDeepCopy(vtkAbstractTransform *transform)
{
  vtkITKRigidRegistrationTransformBase *t = (vtkITKRigidRegistrationTransformBase *)transform;

  cerr << "Calling Internal Deep Copy" << endl;

  this->SetSourceStandardDeviation(t->GetSourceStandardDeviation());
  this->SetTargetStandardDeviation(t->GetTargetStandardDeviation());
  this->SetTranslateScale(t->GetTranslateScale());
  this->SetNumberOfSamples(t->GetNumberOfSamples());
  this->SetMetricValue(t->GetMetricValue());

  this->SetLearningRate(t->LearningRate);
  this->SetTranslateScale(t->TranslateScale);
  this->SetNumberOfSamples(t->NumberOfSamples);

  this->Modified();
}


//----------------------------------------------------------------------------

void vtkITKRigidRegistrationTransformBase::SetNextLearningRate(const double rate)
{ LearningRate->InsertNextValue(rate); }

//----------------------------------------------------------------------------

void vtkITKRigidRegistrationTransformBase::SetNextMaxNumberOfIterations(const int num) 
  { MaxNumberOfIterations->InsertNextValue(num); }

//----------------------------------------------------------------------------

void vtkITKRigidRegistrationTransformBase::SetSourceShrinkFactors(
    unsigned int i, unsigned int j, unsigned int k)
{
  SourceShrink[0] = i;
  SourceShrink[1] = j;
  SourceShrink[2] = k;
}

//----------------------------------------------------------------------------

void vtkITKRigidRegistrationTransformBase::SetTargetShrinkFactors(
    unsigned int i, unsigned int j, unsigned int k)
{
  TargetShrink[0] = i;
  TargetShrink[1] = j;
  TargetShrink[2] = k;
}

//----------------------------------------------------------------------------

void vtkITKRigidRegistrationTransformBase::Inverse()
{
  vtkImageData *tmp1 = this->SourceImage;
  vtkImageData *tmp2 = this->TargetImage;
  this->TargetImage = tmp1;
  this->SourceImage = tmp2;
  this->Modified();
}


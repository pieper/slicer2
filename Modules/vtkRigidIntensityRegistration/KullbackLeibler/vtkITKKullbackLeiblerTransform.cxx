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

#include "vtkITKKullbackLeiblerTransform.h"

#include "vtkImageData.h"
#include "vtkObjectFactory.h"
#include "vtkImageExport.h"
#include "vtkTransform.h"
#include "vtkUnsignedIntArray.h"
#include "vtkDoubleArray.h"

#include "vtkMatrix4x4.h"
#include "vtkImageFlip.h"

#include "itkVTKImageImport.h"
#include "vtkITKUtility.h"

// All the MI Registration Stuff
#include "itkExceptionObject.h"

// itk classes
// Some of this may be necessary, but I doubt it.
#include "itkImage.h"
#include "itkImageRegionIterator.h"
#include "itkQuaternionRigidTransform.h"
#include "itkQuaternionRigidTransformGradientDescentOptimizer.h"
#include "itkKLHistogramImageToImageMetric.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkImageRegistrationMethod.h"
#include "itkNumericTraits.h"
#include "vnl/vnl_math.h"

#include "itkObject.h"
#include "itkMultiResolutionImageRegistrationMethod.h"
#include "itkAffineTransform.h"
#include "itkRecursiveMultiResolutionPyramidImageFilter.h"

#include "itkArray.h"

#include "KLRegistration.h"

//------------------------------------------------------------------------------
vtkITKKullbackLeiblerTransform* vtkITKKullbackLeiblerTransform::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkITKKullbackLeiblerTransform");
  if(ret)
    {
    return (vtkITKKullbackLeiblerTransform*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkITKKullbackLeiblerTransform;
}

//----------------------------------------------------------------------------
vtkITKKullbackLeiblerTransform::vtkITKKullbackLeiblerTransform()
{
  // Default Parameters
  this->HistSizeSource = 32;
  this->HistSizeTarget = 32;
  this->HistEpsilon = 1e-12;

  this->TrainingHistogram = NULL; // No Histogram until calculated
  this->TrainingSourceImage = NULL;
  this->TrainingTargetImage = NULL;
  this->TrainingTransform = NULL;
}

//----------------------------------------------------------------------------

vtkITKKullbackLeiblerTransform::~vtkITKKullbackLeiblerTransform()
{
  if(this->TrainingSourceImage)
    {
    this->TrainingSourceImage->Delete();
    }
  if(this->TrainingTargetImage)
    {
    this->TrainingTargetImage->Delete();
    }
  if(this->TrainingTransform)
    {
    this->TrainingTransform->Delete();
    }
}

//----------------------------------------------------------------------------

void vtkITKKullbackLeiblerTransform::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

  os << "TrainingSourceImage: " << this->TrainingSourceImage << endl;
  if(this->TrainingSourceImage)
    {
    this->TrainingSourceImage->PrintSelf(os,indent.GetNextIndent());
    }
  os << "TrainingTargetImage: " << this->TrainingTargetImage << endl;
  if(this->TrainingTargetImage)
    {
    this->TrainingTargetImage->PrintSelf(os,indent.GetNextIndent());
    }

  // fill in stuff
}

//----------------------------------------------------------------------------

// This templated function executes the filter for any type of data.
// But, actually we use only float...
template <class T>
static void vtkITKKLExecute(vtkITKKullbackLeiblerTransform *self,
                               vtkImageData *source,
                               vtkImageData *target,
                               vtkMatrix4x4 *matrix,
                               T vtkNotUsed(dummy))
{

  // Declare the input and output types
  typedef itk::Image<T,3>                       OutputImageType;

  // Create the Registrator
  typedef itk::KLRegistration<OutputImageType,OutputImageType> RegistratorType;
  typename RegistratorType::Pointer KLRegistrator = RegistratorType::New();


  KlRegistrator->Initialize(self,matrix);

  self->Print(std::cout);

  // ---------------------------------------
  // Set Up the KL Metric
  // --------------------------------------

  typedef itk::AffineTransform< double, 3 > TrainingTransformType;
  TrainingTransformType::Pointer ITKTrainingTransform = TrainingTransformType::New();
  typedef TrainingTransformType::ParametersType ParametersType;

  ParametersType parameters( ITKTrainingTransform->GetNumberOfParameters() );
  
  int count = 0;
  for( unsigned int row = 0; row < 3; row++ )
    {
    for( unsigned int col = 0; col < 3; col++ )
      {
      parameters[count] = 0;
      if( row == col )
        {
        parameters[count] = self->GetTrainingTransform()->GetElement(row,col);
        }
      ++count;
      }
    }
  // initialize the offset/vector part
  for( unsigned int k = 0; k < 3; k++ )
    {
    parameters[count] =  self->GetTrainingTransform()->GetElement(3,k);
    ++count;
    }
  ITKTrainingTransform->SetParameters(parameters);

  KLRegistrator->SetTrainingTransform(ITKTrainingTransform);
  KLRegistrator->SetTrainingFixedImage(VTKtoITKImage(self->GetTrainingTargetImage(),(T*)(NULL)))->UnRegister();
  KLRegistrator->SetTrainingMovingImage(VTKtoITKImage(self->GetTrainingSourceImage(),(T*)(NULL)))->UnRegister();

  // take care of memory leak 
  //  KLRegistrator->GetTrainingFixedImage()
  //  KLRegistrator->GetTrainingMovingImage()->UnRegister();

  typedef RegistratorType::SizeType SizeType;

  SizeType histSize;
  histSize[0] = self->GetHistSizeSource();
  histSize[1] = self->GetHistSizeTarget();
  KLRegistrator->SetHistogramSize(histSize);

  KLRegistrator->SetHistogramEpsilon(self->GetHistEpsilon());

  // ----------------------------------------
  // Do the Registratioon Configuration
  // ----------------------------------------

  //
  // Start registration
  //
  try { KLRegistrator->Execute(); }
  catch( itk::ExceptionObject & err )
    {
      std::cout << "Caught an exception: " << std::endl;
      std::cout << err << std::endl;
      self->SetError(1);
      return;
    }

  KLRegistrator->ResultsToMatrix(matrix);

  // Get the Value of the agreement
  self->SetMetricValue(KLRegistrator->GetMetricValue());
  // self->SetMetricValue(optimizer->GetValue());

  // the last iteration finished with no error
  self->SetError(0);

  self->Modified();
}

//----------------------------------------------------------------------------
// Update the 4x4 matrix. Updates are only done as necessary.
 
void vtkITKKullbackLeiblerTransform::InternalUpdate()
{

  if (this->SourceImage == NULL || this->TargetImage == NULL)
    {
    this->Matrix->Identity();
    return;
    }

  if (MaxNumberOfIterations->GetNumberOfTuples() != 
               LearningRate->GetNumberOfTuples())
    vtkErrorMacro (<< MaxNumberOfIterations->GetNumberOfTuples() 
    << "is the number of levels of iterations"
    << LearningRate->GetNumberOfTuples() 
    << "is the number of levels of learning rates. "
    << "the two numbers should be the same");

  if (this->SourceImage->GetScalarType() != VTK_FLOAT)
    {
    vtkErrorMacro (<< "Source type " << this->SourceImage->GetScalarType()
                   << "must be float");
    this->Matrix->Identity();
    return;
    }

  if (this->TargetImage->GetScalarType() != VTK_FLOAT)
    {
    vtkErrorMacro (<< "Target type " << this->SourceImage->GetScalarType()
                   << "must be float");
    this->Matrix->Identity();
    return;
    }

  float dummy = 0.0;
  vtkITKKLExecute(this,
          this->SourceImage,
          this->TargetImage,
          this->Matrix,
          dummy);
}

//------------------------------------------------------------------------

unsigned long vtkITKKullbackLeiblerTransform::GetMTime()
{
  unsigned long result = this->Superclass::GetMTime();
  unsigned long mtime;

  if (this->TrainingSourceImage)
    {
    mtime = this->TrainingSourceImage->GetMTime(); 
    if (mtime > result)
      {
      result = mtime;
      }
    }
  if (this->TrainingTargetImage)
    {
    mtime = this->TrainingTargetImage->GetMTime();
    if (mtime > result)
      {
      result = mtime;
      }
    }
  return result;
}

//----------------------------------------------------------------------------
vtkAbstractTransform *vtkITKKullbackLeiblerTransform::MakeTransform()
{
  return vtkITKKullbackLeiblerTransform::New(); 
}

//----------------------------------------------------------------------------
void vtkITKKullbackLeiblerTransform::InternalDeepCopy(vtkAbstractTransform *transform)
{
  Superclass::InternalDeepCopy(transform);
  vtkITKKullbackLeiblerTransform *t = (vtkITKKullbackLeiblerTransform *)transform;

  // fill in stuff
}


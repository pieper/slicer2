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

#include "vtkITKMutualInformationTransform.h"

#include "vtkImageData.h"
#include "vtkObjectFactory.h"
#include "vtkImageExport.h"
#include "vtkTransform.h"

#include "itkVTKImageImport.h"
#include "vtkITKUtility.h"

// All the MI Registration Stuff
#include "MIRegistration.h"
#include "itkExceptionObject.h"

// Some of this may be necessary, but I doubt it.
#include "itkObject.h"
#include "itkMultiResolutionImageRegistrationMethod.h"
#include "itkAffineTransform.h"

#include "itkQuaternionRigidTransform.h"
#include "itkMutualInformationImageToImageMetric.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkQuaternionRigidTransformGradientDescentOptimizer.h"
#include "itkRecursiveMultiResolutionPyramidImageFilter.h"

#include "itkArray.h"

// turn itk exceptions into vtk errors
#undef itkExceptionMacro  
#define itkExceptionMacro(x) \
  { \
  ::itk::OStringStream message; \
  message << "itk::ERROR: " << this->GetNameOfClass() \
          << "(" << this << "): " x; \
  std::cerr << message.str().c_str() << std::endl; \
  }

#undef itkGenericExceptionMacro  
#define itkGenericExceptionMacro(x) \
  { \
  ::itk::OStringStream message; \
  message << "itk::ERROR: " x; \
  std::cerr << message.str().c_str() << std::endl; \
  }

// itk classes

#include "itkImage.h"
#include "itkImageRegionIterator.h"
#include "itkQuaternionRigidTransform.h"
#include "itkQuaternionRigidTransformGradientDescentOptimizer.h"
#include "itkMutualInformationImageToImageMetric.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkImageRegistrationMethod.h"
#include "itkNumericTraits.h"
#include "MIRegistration.txx"
#include "vnl/vnl_math.h"

vtkCxxRevisionMacro(vtkITKMutualInformationTransform, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkITKMutualInformationTransform);

//----------------------------------------------------------------------------
vtkITKMutualInformationTransform::vtkITKMutualInformationTransform()
{
  // Default Parameters
  this->SourceImage=NULL;
  this->TargetImage=NULL;
  this->SourceStandardDeviation = 2.0;
  this->TargetStandardDeviation = 2.0;
  this->LearningRate = .005;
  this->TranslateScale = 64;
  this->NumberOfIterations = 500;  
  this->NumberOfSamples = 50;
  this->MetricValue = 0;
  this->Matrix->Identity();
}

//----------------------------------------------------------------------------

vtkITKMutualInformationTransform::~vtkITKMutualInformationTransform()
{
  if(this->SourceImage)
    {
    this->SourceImage->Delete();
    }
  if(this->TargetImage)
    { 
    this->TargetImage->Delete();
    }
}

//----------------------------------------------------------------------------

void vtkITKMutualInformationTransform::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

  os << "SourceStandardDeviation: " << this->SourceStandardDeviation  << endl;
  os << "TargetStandardDeviation: " << this->SourceStandardDeviation  << endl;
  os << "LearningRate: " << this->LearningRate  << endl;
  os << "TranslateScale: " << this->TranslateScale  << endl;
  os << "NumberOfSamples: " << this->NumberOfSamples  << endl;
  os << "NumberOfIterations: " << this->NumberOfIterations  << endl;
  os << "MetricValue: " << this->MetricValue  << endl;

  os << "SourceImage: " << this->SourceImage << "\n";
  if(this->SourceImage)
    {
    this->SourceImage->PrintSelf(os,indent.GetNextIndent());
    }
  os << "TargetImage: " << this->TargetImage << "\n";
  if(this->TargetImage)
    {
    this->TargetImage->PrintSelf(os,indent.GetNextIndent());
    }
}

//----------------------------------------------------------------------------

// This templated function executes the filter for any type of data.
// But, actually we use only float...
template <class T>
static void vtkITKMutualInformationExecute(vtkITKMutualInformationTransform *self,
                               vtkImageData *source,
                               vtkImageData *target,
                               vtkMatrix4x4 *matrix,
                               T vtkNotUsed(dummy))
{
  self->Print(cout);

  // Declare the input and output types
  typedef itk::Image<T,3>                       OutputImageType;
  typedef itk::VTKImageImport<OutputImageType>  ImageImportType;

  // ----------------------------------------
  // Sources to ITK MIRegistration
  // ----------------------------------------

  // Source Image that is moving
  vtkImageExport *movingVtkExporter = vtkImageExport::New();
    movingVtkExporter->SetInput(source);

  // Source Image that is moving into ITK
  ImageImportType::Pointer movingItkImporter = ImageImportType::New();
  ConnectPipelines(movingVtkExporter, movingItkImporter);

  // Source Image that is not moving
  vtkImageExport *fixedVtkExporter = vtkImageExport::New();
    fixedVtkExporter->SetInput(target);

  // Source Image that is not moving into ITK
  ImageImportType::Pointer fixedItkImporter = ImageImportType::New();
  ConnectPipelines(fixedVtkExporter, fixedItkImporter);

  fixedItkImporter->Update();
  movingItkImporter->Update();

  // Create the Registrator
  typedef itk::MIRegistration<OutputImageType,OutputImageType> RegistratorType;
  typename RegistratorType::Pointer MIRegistrator = RegistratorType::New();

  MIRegistrator->SetFixedImage(fixedItkImporter->GetOutput());
  MIRegistrator->SetMovingImage(movingItkImporter->GetOutput());

  // ----------------------------------------
  // Do the Registratioon Configuration
  // ----------------------------------------

  // Initialize
  MIRegistrator->InitializeRegistration(matrix);

 // Setup the optimizer

//  // This is the scale on translation
//  for (int j=4; j<7; j++)
//    {
//    scales[j] = MIReg_TranslationScale;
//    // This was chosen by Steve. I'm not sure why.
//    // scales[j] = 1.0 / vnl_math_sqr(self->GetTranslateScale());
//    }

  // Set metric related parameters
  MIRegistrator->SetMovingImageStandardDeviation(self->GetSourceStandardDeviation());
  MIRegistrator->SetFixedImageStandardDeviation(self->GetTargetStandardDeviation());
  MIRegistrator->SetNumberOfSpatialSamples(self->GetNumberOfSamples());

  //
  // THIS NEEDS TO BE AN ARRAY
  // Temporary hack for single layer multi-res structure
  //
  typedef typename RegistratorType::UnsignedIntArray UnsignedIntArray;
  typedef typename RegistratorType::DoubleArray      DoubleArray;
  
  DoubleArray      LearnRates(1);
  LearnRates.Fill(self->GetLearningRate());
  MIRegistrator->SetLearningRates(LearnRates);
  
  UnsignedIntArray NumIterations(1);
  NumIterations.Fill(self->GetNumberOfIterations());
  MIRegistrator->SetNumberOfIterations(NumIterations);

  // Start registration
  MIRegistrator->Execute();

  MIRegistrator->ResultsToMatrix(matrix);

  // Get the Value of the agreement
  self->SetMetricValue(MIRegistrator->GetMetricValue());
  // self->SetMetricValue(optimizer->GetValue());

  self->Modified();
}

//----------------------------------------------------------------------------
// Update the 4x4 matrix. Updates are only done as necessary.
 
void vtkITKMutualInformationTransform::InternalUpdate()
{

  if (this->SourceImage == NULL || this->TargetImage == NULL)
    {
    this->Matrix->Identity();
    return;
    }

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
  vtkITKMutualInformationExecute(this,
                                 this->SourceImage,
                                 this->TargetImage,
                                 this->Matrix,
                                 dummy);
}

//------------------------------------------------------------------------
void vtkITKMutualInformationTransform::Identity()
{
    this->Matrix->Identity();
}

//------------------------------------------------------------------------
void vtkITKMutualInformationTransform::Initialize(vtkLinearTransform *initial)
{
  this->Matrix->DeepCopy(initial->GetMatrix());
}

//------------------------------------------------------------------------
unsigned long vtkITKMutualInformationTransform::GetMTime()
{
  unsigned long result = this->vtkLinearTransform::GetMTime();
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
//------------------------------------------------------------------------
void vtkITKMutualInformationTransform::SetSourceImage(vtkImageData *source)
{
  if (this->SourceImage == source)
    {
    return;
    }

  if (this->SourceImage)
    {
    this->SourceImage->Delete();
    }

  source->Register(this);
  this->SourceImage = source;

  this->Modified();
}

//------------------------------------------------------------------------
void vtkITKMutualInformationTransform::SetTargetImage(vtkImageData *target)
{
  if (this->TargetImage == target)
    {
    return;
    }

  if (this->TargetImage)
    {
    this->TargetImage->Delete();
    }

  target->Register(this);
  this->TargetImage = target;
  this->Modified();
}

//----------------------------------------------------------------------------
void vtkITKMutualInformationTransform::Inverse()
{
  vtkImageData *tmp1 = this->SourceImage;
  vtkImageData *tmp2 = this->TargetImage;
  this->TargetImage = tmp1;
  this->SourceImage = tmp2;
  this->Modified();
}

//----------------------------------------------------------------------------
vtkAbstractTransform *vtkITKMutualInformationTransform::MakeTransform()
{
  return vtkITKMutualInformationTransform::New(); 
}

//----------------------------------------------------------------------------
void vtkITKMutualInformationTransform::InternalDeepCopy(vtkAbstractTransform *transform)
{
  vtkITKMutualInformationTransform *t = (vtkITKMutualInformationTransform *)transform;

  this->SetSourceStandardDeviation(t->SourceStandardDeviation);
  this->SetTargetStandardDeviation(t->TargetStandardDeviation);

  //
  // ========= This Needs rewriting!!!
  //

  this->SetLearningRate(t->LearningRate);
  this->SetTranslateScale(t->TranslateScale);
  this->SetNumberOfSamples(t->NumberOfSamples);

  this->Modified();
}



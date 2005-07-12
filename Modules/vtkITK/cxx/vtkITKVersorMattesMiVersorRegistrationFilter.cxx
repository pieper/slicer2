#include "vtkITKVersorMattesMiVersorRegistrationFilter.h"
typedef itk::Array<unsigned int> UnsignedIntArray;
typedef itk::Array<double> DoubleArray;

vtkITKVersorMattesMiVersorRegistrationFilter::vtkITKVersorMattesMiVersorRegistrationFilter()
{
  m_ITKFilter = itk::itkVersorMattesMiVersorRegistrationFilter::New();
  LinkITKProgressToVTKProgress(m_ITKFilter);
  this->SetSourceShrinkFactors(1,1,1);
  this->SetTargetShrinkFactors(1,1,1);
  this->SetTranslateScale(0.001);

  this->MinimumStepLength = vtkDoubleArray::New();
  this->MaximumStepLength = vtkDoubleArray::New();
  this->MaxNumberOfIterations = vtkUnsignedIntArray::New();

  // Default Number of MultiResolutionLevels is 1
  this->SetNextMaxNumberOfIterations(100);
  this->SetNextMinimumStepLength(0.0001);
  this->SetNextMaximumStepLength(0.2);
  this->SetNumberOfSamples(100);
  this->SetNumberOfHistogramBins(256);
}

void vtkITKVersorMattesMiVersorRegistrationFilter::CreateRegistrationPipeline()
{
  //DemonsRegistrationFilterCommand::Pointer observer = DemonsRegistrationFilterCommand::New();
  //observer->SetDemonsRegistrationFilter(this);
  //m_ITKFilter->AddIterationObserver(observer );
  m_ITKFilter->SetInput(itkImporterFixed->GetOutput());
  m_ITKFilter->SetInput(1, itkImporterMoving->GetOutput());

  vtkITKVersorMattesMiVersorRegistrationCommand::Pointer observer = vtkITKVersorMattesMiVersorRegistrationCommand::New();
  observer->SetRegistrationFilter(this);
  m_ITKFilter->AddIterationObserver(observer );
}

void 
vtkITKVersorMattesMiVersorRegistrationFilter::UpdateRegistrationParameters()
{
  itk::itkVersorMattesMiVersorRegistrationFilter* filter = static_cast<itk::itkVersorMattesMiVersorRegistrationFilter *> (m_ITKFilter);

  filter->SetTranslationScale(this->GetTranslateScale());
  filter->SetMovingImageShrinkFactors(this->SourceShrink);
  filter->SetFixedImageShrinkFactors(this->TargetShrink);

  DoubleArray      MinimumStepLength(this->GetMinimumStepLength()->GetNumberOfTuples());
  DoubleArray      MaximumStepLength(this->GetMaximumStepLength()->GetNumberOfTuples());
  UnsignedIntArray NumIterations(this->GetMaxNumberOfIterations()->GetNumberOfTuples());


  for(int i=0; i< this->GetMaxNumberOfIterations()->GetNumberOfTuples();i++) {
    MinimumStepLength[i]    = this->GetMinimumStepLength()->GetValue(i);
    MaximumStepLength[i]    = this->GetMaximumStepLength()->GetValue(i);
    NumIterations[i] = this->GetMaxNumberOfIterations()->GetValue(i);
  }
  filter->SetNumberOfLevels(this->GetMaxNumberOfIterations()->GetNumberOfTuples());
  filter->SetMinimumStepLength(MinimumStepLength);
  filter->SetMaximumStepLength(MaximumStepLength);
  filter->SetNumberOfIterations(NumIterations);
  filter->SetNumberOfSpatialSamples(NumberOfSamples);
  filter->SetNumberOfHistogramBins(NumberOfHistogramBins);
}

vtkITKRegistrationFilter::OutputImageType::Pointer vtkITKVersorMattesMiVersorRegistrationFilter::GetTransformedOutput()
{
   return m_ITKFilter->GetOutput();
}

void
vtkITKVersorMattesMiVersorRegistrationFilter::GetTransformationMatrix(vtkMatrix4x4* matrix)
{
  itk::itkVersorMattesMiVersorRegistrationFilter::TransformType::Pointer transform 
    = itk::itkVersorMattesMiVersorRegistrationFilter::TransformType::New();

  m_ITKFilter->GetTransform(transform);

  transform->GetRotationMatrix();

  const itk::itkVersorMattesMiVersorRegistrationFilter::TransformType::MatrixType ResMat   =transform->GetRotationMatrix();
  const itk::itkVersorMattesMiVersorRegistrationFilter::TransformType::OffsetType ResOffset=transform->GetOffset();

  
  // Create Rotation Matrix
  matrix->Identity();

  for(int i=0;i<3;i++) {
    for(int j=0;j<3;j++) {
      matrix->Element[i][j] = ResMat[i][j];
    }
  }

  matrix->Invert();

  // Add translation with vtk to itk flip in Y and Z
  matrix->Element[0][3] = -ResOffset[0];
  matrix->Element[1][3] = ResOffset[1];
  matrix->Element[2][3] = ResOffset[2];
  
  matrix->Invert();
}
  
void
vtkITKVersorMattesMiVersorRegistrationFilter::GetCurrentTransformationMatrix(vtkMatrix4x4* matrix)
{
  itk::itkVersorMattesMiVersorRegistrationFilter::TransformType::Pointer transform 
    = itk::itkVersorMattesMiVersorRegistrationFilter::TransformType::New();
  
  m_ITKFilter->GetCurrentTransform(transform);
  
  transform->GetRotationMatrix();
  
  const itk::itkVersorMattesMiVersorRegistrationFilter::TransformType::MatrixType ResMat   =transform->GetRotationMatrix();
  const itk::itkVersorMattesMiVersorRegistrationFilter::TransformType::OffsetType ResOffset=transform->GetOffset();
  
  // Create Rotation Matrix
  matrix->Identity();

  for(int i=0;i<3;i++) {
    for(int j=0;j<3;j++) {
      matrix->Element[i][j] = ResMat[i][j];
    }
  }

  matrix->Invert();

  // Add translation with vtk to itk flip in Y and Z
  matrix->Element[0][3] = -ResOffset[0];
  matrix->Element[1][3] = ResOffset[1];
  matrix->Element[2][3] = ResOffset[2];
  
  matrix->Invert();
}
  
void 
vtkITKVersorMattesMiVersorRegistrationFilter::SetTransformationMatrix(vtkMatrix4x4 *matrix)
{
  itk::itkVersorMattesMiVersorRegistrationFilter::ParametersType  initialParameters = itk::itkVersorMattesMiVersorRegistrationFilter::ParametersType(7);

  // transform from vtk to itk space
  vtkMatrix4x4 *matrixITK =  vtkMatrix4x4::New();
  vtkITKTransformRegistrationFilter::vtkItkMatrixTransform(matrix, matrixITK);

  vnl_matrix<double> matrix3x4(3,4);

  for(int i=0;i<3;i++)
    for(int j=0;j<4;j++)
      matrix3x4[i][j] = matrixITK->Element[i][j];
  
  vnl_quaternion<double> matrixAsQuaternion(matrix3x4);

  // There is a transpose between the vnl quaternion and itk quaternion.
  vnl_quaternion<double> conjugated = matrixAsQuaternion.conjugate();

  // This command automatically does the conjugate.
  // But, it does not calculate the paramaters
  // m_Transform->SetRotation(matrixAsQuaternion);

  // Quaternions have 7 parameters. The first four represents the
  // quaternion and the last three represents the offset. 
  initialParameters[0] = conjugated.x();
  initialParameters[1] = conjugated.y();
  initialParameters[2] = conjugated.z();
  //  initialParameters[3] = conjugated.r();
  initialParameters[3] = matrix->Element[0][3];
  initialParameters[4] = matrix->Element[1][3];
  initialParameters[5] = matrix->Element[2][3];

  itk::itkVersorMattesMiVersorRegistrationFilter::TransformType::Pointer transform = itk::itkVersorMattesMiVersorRegistrationFilter::TransformType::New();
  transform->SetParameters(initialParameters);
  // The guess is: a quaternion followed by a translation
  m_ITKFilter->SetTransform(transform);
}


void 
vtkITKVersorMattesMiVersorRegistrationFilter::SetSourceShrinkFactors(unsigned int i,
                                                                     unsigned int j, 
                                                                     unsigned int k)
{
  SourceShrink[0] = i;
  SourceShrink[1] = j;
  SourceShrink[2] = k;
} //vtkITKVersorMattesMiVersorRegistrationFilter

void 
vtkITKVersorMattesMiVersorRegistrationFilter::SetTargetShrinkFactors(unsigned int i,
                                                                     unsigned int j, 
                                                                     unsigned int k)
{
  TargetShrink[0] = i;
  TargetShrink[1] = j;
  TargetShrink[2] = k;
} //vtkITKVersorMattesMiVersorRegistrationFilter

#include "vtkITKAffineMattesMIRegistrationFilter.h"
typedef itk::Array<unsigned int> UnsignedIntArray;
typedef itk::Array<double> DoubleArray;

vtkITKAffineMattesMIRegistrationFilter::vtkITKAffineMattesMIRegistrationFilter()
{
  m_ITKFilter = itk::itkAffineMattesMIRegistrationFilter::New();
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

void vtkITKAffineMattesMIRegistrationFilter::CreateRegistrationPipeline()
{
  //DemonsRegistrationFilterCommand::Pointer observer = DemonsRegistrationFilterCommand::New();
  //observer->SetDemonsRegistrationFilter(this);
  //m_ITKFilter->AddIterationObserver(observer );
  m_ITKFilter->SetInput(itkImporterFixed->GetOutput());
  m_ITKFilter->SetInput(1, itkImporterMoving->GetOutput());

  vtkITKAffineMattesMIRegistrationCommand::Pointer observer = vtkITKAffineMattesMIRegistrationCommand::New();
  observer->SetRegistrationFilter(this);
  m_ITKFilter->AddIterationObserver(observer );
}

void 
vtkITKAffineMattesMIRegistrationFilter::UpdateRegistrationParameters()
{
  itk::itkAffineMattesMIRegistrationFilter* filter = static_cast<itk::itkAffineMattesMIRegistrationFilter *> (m_ITKFilter);

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

vtkITKRegistrationFilter::OutputImageType::Pointer vtkITKAffineMattesMIRegistrationFilter::GetTransformedOutput()
{
   return m_ITKFilter->GetOutput();
}

void
vtkITKAffineMattesMIRegistrationFilter::GetTransformationMatrix(vtkMatrix4x4* matrix)
{
  itk::itkAffineMattesMIRegistrationFilter::TransformType::Pointer transform 
    = itk::itkAffineMattesMIRegistrationFilter::TransformType::New();

  m_ITKFilter->GetTransform(transform);

  itk::itkAffineMattesMIRegistrationFilter::TransformType::ParametersType params = transform->GetParameters();
  
  // itk matrix
  //vtkMatrix4x4 *matrixITK =  vtkMatrix4x4::New();
  //matrixITK->Identity();

  int count=0;
  for(int i=0;i<3;i++) {
    for(int j=0;j<3;j++) {
      matrix->Element[i][j] = params[count++];
    }
  }
  matrix->Invert();

  // Add translation with vtk to itk flip in Y and Z
  matrix->Element[0][3] = -params[9];
  matrix->Element[1][3] = params[10];
  matrix->Element[2][3] = params[11];

  matrix->Invert();

  // transform from itk to vtk space
  //vtkITKTransformRegistrationFilter::vtkItkMatrixTransform(matrixITK, matrix);
  //matrixITK->Delete();
}
  
void
vtkITKAffineMattesMIRegistrationFilter::GetCurrentTransformationMatrix(vtkMatrix4x4* matrix)
{
  itk::itkAffineMattesMIRegistrationFilter::TransformType::Pointer transform 
    = itk::itkAffineMattesMIRegistrationFilter::TransformType::New();
  
  m_ITKFilter->GetCurrentTransform(transform);
  
  itk::itkAffineMattesMIRegistrationFilter::TransformType::ParametersType params = transform->GetParameters();
  
 // itk matrix
  //vtkMatrix4x4 *matrixITK =  vtkMatrix4x4::New();
  //matrixITK->Identity();

  int count=0;
  for(int i=0;i<3;i++) {
    for(int j=0;j<3;j++) {
      matrix->Element[i][j] = params[count++];
    }
  }
  matrix->Invert();

  // Add translation with vtk to itk flip in Y and Z
  matrix->Element[0][3] = -params[9];
  matrix->Element[1][3] = params[10];
  matrix->Element[2][3] = params[11];

  matrix->Invert();

  // transform from itk to vtk space
  //vtkITKTransformRegistrationFilter::vtkItkMatrixTransform(matrixITK, matrix);
  //matrixITK->Delete();
}
  
void 
vtkITKAffineMattesMIRegistrationFilter::SetTransformationMatrix(vtkMatrix4x4 *matrix)
{
  itk::itkAffineMattesMIRegistrationFilter::ParametersType  initialParameters = itk::itkAffineMattesMIRegistrationFilter::ParametersType(12);

  /*****
  // transform from vtk to itk space
  vtkMatrix4x4 *matrixITK =  vtkMatrix4x4::New();
  vtkITKTransformRegistrationFilter::vtkItkMatrixTransform(matrix, matrixITK);

  int count=0;
  for(int i=0;i<3;i++) {
    for(int j=0;j<3;j++) {
      initialParameters[count++] = matrixITK->Element[i][j];
    }
  }
  
  initialParameters[9] = matrixITK->Element[0][3];
  initialParameters[10] = matrixITK->Element[1][3];
  initialParameters[11] = matrixITK->Element[2][3];

  matrixITK->Delete();
  ***/

  // transform from vtk to itk space
  vtkMatrix4x4 *matrixITK =  vtkMatrix4x4::New();
  matrixITK->DeepCopy(matrix);
  matrixITK->Invert();

  initialParameters[9] = -matrixITK->Element[0][3];
  initialParameters[10] = matrixITK->Element[1][3];
  initialParameters[11] = matrixITK->Element[2][3];

  matrixITK->Element[0][3] = 0;
  matrixITK->Element[1][3] = 0;
  matrixITK->Element[2][3] = 0;

  matrixITK->Invert();

  int count=0;
  for(int i=0;i<3;i++) {
    for(int j=0;j<3;j++) {
      initialParameters[count++] = matrixITK->Element[i][j];
    }
  }

  itk::itkAffineMattesMIRegistrationFilter::TransformType::Pointer transform = itk::itkAffineMattesMIRegistrationFilter::TransformType::New();
  transform->SetParameters(initialParameters);
  // The guess is: a quaternion followed by a translation
  m_ITKFilter->SetTransform(transform);
}


void 
vtkITKAffineMattesMIRegistrationFilter::SetSourceShrinkFactors(unsigned int i,
                                                                     unsigned int j, 
                                                                     unsigned int k)
{
  SourceShrink[0] = i;
  SourceShrink[1] = j;
  SourceShrink[2] = k;
} //vtkITKAffineMattesMIRegistrationFilter

void 
vtkITKAffineMattesMIRegistrationFilter::SetTargetShrinkFactors(unsigned int i,
                                                                     unsigned int j, 
                                                                     unsigned int k)
{
  TargetShrink[0] = i;
  TargetShrink[1] = j;
  TargetShrink[2] = k;
} //vtkITKAffineMattesMIRegistrationFilter

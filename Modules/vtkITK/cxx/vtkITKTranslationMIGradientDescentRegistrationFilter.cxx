#include "vtkITKTranslationMIGradientDescentRegistrationFilter.h"

typedef itk::Array<unsigned int> UnsignedIntArray;
typedef itk::Array<double> DoubleArray;

vtkITKTranslationMIGradientDescentRegistrationFilter::vtkITKTranslationMIGradientDescentRegistrationFilter()
{
  m_ITKFilter = itk::itkTranslationMIGradientDescentRegistrationFilter::New();
  LinkITKProgressToVTKProgress(m_ITKFilter);
  this->SetSourceShrinkFactors(1,1,1);
  this->SetTargetShrinkFactors(1,1,1);

  this->LearningRate = vtkDoubleArray::New();
  this->MaxNumberOfIterations = vtkUnsignedIntArray::New();

  // Default Number of MultiResolutionLevels is 1
  this->SetNextMaxNumberOfIterations(100);
  this->SetNextLearningRate(0.0001);
  this->SetNumberOfSamples(100);
  this->SetStandardDeviation(0.4);
}

void vtkITKTranslationMIGradientDescentRegistrationFilter::CreateRegistrationPipeline()
{
  m_ITKFilter->SetInput(itkImporterFixed->GetOutput());
  m_ITKFilter->SetInput(1, itkImporterMoving->GetOutput());

  vtkITKTranslationMIGradientDescentRegistrationCommand::Pointer observer = vtkITKTranslationMIGradientDescentRegistrationCommand::New();
  observer->SetRegistrationFilter(this);
  m_ITKFilter->AddIterationObserver(observer );
}

void 
vtkITKTranslationMIGradientDescentRegistrationFilter::UpdateRegistrationParameters()
{
  itk::itkTranslationMIGradientDescentRegistrationFilter* filter = static_cast<itk::itkTranslationMIGradientDescentRegistrationFilter *> (m_ITKFilter);

  filter->SetMovingImageShrinkFactors(this->SourceShrink);
  filter->SetFixedImageShrinkFactors(this->TargetShrink);

  DoubleArray      LearningRate(this->GetLearningRate()->GetNumberOfTuples());
  UnsignedIntArray NumIterations(this->GetMaxNumberOfIterations()->GetNumberOfTuples());


  for(int i=0; i< this->GetMaxNumberOfIterations()->GetNumberOfTuples();i++) {
    LearningRate[i]    = this->GetLearningRate()->GetValue(i);
    NumIterations[i] = this->GetMaxNumberOfIterations()->GetValue(i);
  }
  filter->SetNumberOfLevels(this->GetMaxNumberOfIterations()->GetNumberOfTuples());
  filter->SetLearningRate(LearningRate);
  filter->SetNumberOfIterations(NumIterations);
  filter->SetNumberOfSpatialSamples(NumberOfSamples);
  filter->SetStandardDeviation(StandardDeviation);
}

vtkITKRegistrationFilter::OutputImageType::Pointer vtkITKTranslationMIGradientDescentRegistrationFilter::GetTransformedOutput()
{
   return m_ITKFilter->GetOutput();
}

void
vtkITKTranslationMIGradientDescentRegistrationFilter::GetTransformationMatrix(vtkMatrix4x4* matrix)
{
  itk::itkTranslationMIGradientDescentRegistrationFilter::TransformType::Pointer transform 
    = itk::itkTranslationMIGradientDescentRegistrationFilter::TransformType::New();

  m_ITKFilter->GetTransform(transform);

  itk::itkTranslationMIGradientDescentRegistrationFilter::TransformType::ParametersType params = transform->GetParameters();
 
  // itk to vtk flip matrix
  vtkMatrix4x4 *rot = vtkMatrix4x4::New();
  rot->Identity();
  rot->Element[1][1] = -1;
  rot->Element[2][2] = -1;
  
  // itk matrix
  vtkMatrix4x4 *matrixITK =  vtkMatrix4x4::New();
  matrixITK->Identity();
  matrixITK->Element[0][3] = params[0];
  matrixITK->Element[1][3] = params[1];
  matrixITK->Element[2][3] = params[2];

  // vtk matrix
  vtkMatrix4x4::Multiply4x4(rot, matrixITK, matrix);
  vtkMatrix4x4::Multiply4x4(matrix, rot, matrixITK);
  matrix->DeepCopy(matrixITK);

}
  
void
vtkITKTranslationMIGradientDescentRegistrationFilter::GetCurrentTransformationMatrix(vtkMatrix4x4* matrix)
{
  itk::itkTranslationMIGradientDescentRegistrationFilter::TransformType::Pointer transform 
    = itk::itkTranslationMIGradientDescentRegistrationFilter::TransformType::New();
  
  m_ITKFilter->GetCurrentTransform(transform);
  
  itk::itkTranslationMIGradientDescentRegistrationFilter::TransformType::ParametersType params = transform->GetParameters();
 
  // itk to vtk flip matrix
  vtkMatrix4x4 *rot = vtkMatrix4x4::New();
  rot->Identity();
  rot->Element[1][1] = -1;
  rot->Element[2][2] = -1;
  
  // itk matrix
  vtkMatrix4x4 *matrixITK =  vtkMatrix4x4::New();
  matrixITK->Identity();
  matrixITK->Element[0][3] = params[0];
  matrixITK->Element[1][3] = params[1];
  matrixITK->Element[2][3] = params[2];

  // vtk matrix
  vtkMatrix4x4::Multiply4x4(rot, matrixITK, matrix);
  vtkMatrix4x4::Multiply4x4(matrix, rot, matrixITK);
  matrix->DeepCopy(matrixITK);

}
  
void 
vtkITKTranslationMIGradientDescentRegistrationFilter::SetTransformationMatrix(vtkMatrix4x4 *matrix)
{
  itk::itkTranslationMIGradientDescentRegistrationFilter::ParametersType  initialParameters = itk::itkTranslationMIGradientDescentRegistrationFilter::ParametersType(3);

 // itk to vtk flip matrix
  vtkMatrix4x4 *rot = vtkMatrix4x4::New();
  rot->Identity();
  rot->Element[1][1] = -1;
  rot->Element[2][2] = -1;

  // itk matrix
  vtkMatrix4x4 *matrixITK =  vtkMatrix4x4::New();
  vtkMatrix4x4 *matrixITK1 =  vtkMatrix4x4::New();
  vtkMatrix4x4::Multiply4x4(rot, matrix, matrixITK);
  vtkMatrix4x4::Multiply4x4(matrixITK, rot, matrixITK1);

  // rotate around X-axis to tranform from itk to vtk CS
  initialParameters[0] = matrixITK1->Element[0][3];
  initialParameters[1] = matrixITK1->Element[1][3];
  initialParameters[2] = matrixITK1->Element[2][3];

  itk::itkTranslationMIGradientDescentRegistrationFilter::TransformType::Pointer transform = itk::itkTranslationMIGradientDescentRegistrationFilter::TransformType::New();
  transform->SetParameters(initialParameters);

  m_ITKFilter->SetTransform(transform);
}


void 
vtkITKTranslationMIGradientDescentRegistrationFilter::SetSourceShrinkFactors(unsigned int i,
                                                                     unsigned int j, 
                                                                     unsigned int k)
{
  SourceShrink[0] = i;
  SourceShrink[1] = j;
  SourceShrink[2] = k;
} //vtkITKTranslationMIGradientDescentRegistrationFilter

void 
vtkITKTranslationMIGradientDescentRegistrationFilter::SetTargetShrinkFactors(unsigned int i,
                                                                     unsigned int j, 
                                                                     unsigned int k)
{
  TargetShrink[0] = i;
  TargetShrink[1] = j;
  TargetShrink[2] = k;
} //vtkITKTranslationMIGradientDescentRegistrationFilter

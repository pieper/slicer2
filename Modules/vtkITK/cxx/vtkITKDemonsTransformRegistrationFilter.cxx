#include "vtkITKDemonsTransformRegistrationFilter.h" // This class

vtkITKDemonsTransformRegistrationFilter::vtkITKDemonsTransformRegistrationFilter()
{
  NumIterations = 100;
  StandardDeviations = 1.0;
  CurrentIteration = 0;
  
  m_ITKFilter = itk::itkDemonsTransformRegistrationFilterF3::New();
  LinkITKProgressToVTKProgress(m_ITKFilter);

  // set identity transform by default
  m_Matrix = vtkMatrix4x4::New();
  m_Matrix->Identity();
  this->SetTransformationMatrix(m_Matrix);

}

vtkITKDemonsTransformRegistrationFilter::~vtkITKDemonsTransformRegistrationFilter()
{
  m_Matrix->Delete();
}

vtkITKRegistrationFilter::OutputImageType::Pointer vtkITKDemonsTransformRegistrationFilter::GetTransformedOutput()
{
  return m_ITKFilter->GetOutput();
}

vtkITKDeformableRegistrationFilter::DeformationFieldType::Pointer vtkITKDemonsTransformRegistrationFilter::GetDisplacementOutput()
{
  return m_ITKFilter->GetDeformationField();
}

void vtkITKDemonsTransformRegistrationFilter::CreateRegistrationPipeline()
{
  DemonsTransformRegistrationFilterCommand::Pointer observer = DemonsTransformRegistrationFilterCommand::New();
  observer->SetDemonsRegistrationFilter(this);
  m_ITKFilter->AddIterationObserver(observer );
  m_ITKFilter->SetInput(itkImporterFixed->GetOutput());
  m_ITKFilter->SetInput(1, itkImporterMoving->GetOutput());
}

void vtkITKDemonsTransformRegistrationFilter::UpdateRegistrationParameters()
{
  m_ITKFilter->SetNumIterations(NumIterations);
  m_ITKFilter->SetStandardDeviations(StandardDeviations);
  //m_ITKFilter->Update();
}

void
vtkITKDemonsTransformRegistrationFilter::GetTransformationMatrix(vtkMatrix4x4* matrix)
{
  TransformType::Pointer transform 
    = TransformType::New();

  m_ITKFilter->GetTransform(transform);

  TransformType::ParametersType params = transform->GetParameters();
  
  matrix->Identity();
  int count=0;
  for(int i=0;i<3;i++) {
    for(int j=0;j<3;j++) {
      matrix->Element[i][j] = params[count++];
    }
  }

  // Add translation
  matrix->Element[0][3] = params[9];
  matrix->Element[1][3] = params[10];
  matrix->Element[2][3] = params[11];
}
  
  
void 
vtkITKDemonsTransformRegistrationFilter::SetTransformationMatrix(vtkMatrix4x4 *matrix)
{
  TransformType::ParametersType  initialParameters = TransformType::ParametersType(12);

  int count=0;
  for(int i=0;i<3;i++) {
    for(int j=0;j<3;j++) {
      initialParameters[count++] = matrix->Element[i][j];
    }
  }
  
  initialParameters[9] = matrix->Element[0][3];
  initialParameters[10] = matrix->Element[1][3];
  initialParameters[11] = matrix->Element[2][3];

  TransformType::Pointer transform = TransformType::New();
  transform->SetParameters(initialParameters);
  m_ITKFilter->SetTransform(transform);
}


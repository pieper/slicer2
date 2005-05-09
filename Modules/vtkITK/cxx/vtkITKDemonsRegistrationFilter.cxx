#include "vtkITKDemonsRegistrationFilter.h" // This class

vtkITKDemonsRegistrationFilter::vtkITKDemonsRegistrationFilter()
{
  NumIterations = 100;
  StandardDeviations = 1.0;
  CurrentIteration = 0;
  
  m_ITKFilter = itk::itkDemonsRegistrationImageFilter::New();
  LinkITKProgressToVTKProgress(m_ITKFilter);
  
}

vtkITKRegistrationFilter::OutputImageType::Pointer vtkITKDemonsRegistrationFilter::GetTransformedOutput()
{
  return m_ITKFilter->GetOutput();
}

vtkITKDeformableRegistrationFilter::DeformationFieldType::Pointer vtkITKDemonsRegistrationFilter::GetDisplacementOutput()
{
  return m_ITKFilter->GetDeformationField();
}

void vtkITKDemonsRegistrationFilter::CreateRegistrationPipeline()
{
  DemonsRegistrationFilterCommand::Pointer observer = DemonsRegistrationFilterCommand::New();
  observer->SetDemonsRegistrationFilter(this);
  m_ITKFilter->AddIterationObserver(observer );
  m_ITKFilter->SetInput(itkImporterFixed->GetOutput());
  m_ITKFilter->SetInput(1, itkImporterMoving->GetOutput());
};

void vtkITKDemonsRegistrationFilter::UpdateRegistrationParameters()
{
  m_ITKFilter->SetNumIterations(NumIterations);
  m_ITKFilter->SetStandardDeviations(StandardDeviations);
  //m_ITKFilter->Update();
}


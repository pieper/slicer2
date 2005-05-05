#include "vtkITKDemonsRegistrationFilter.h" // This class

vtkITKDemonsRegistrationFilter::vtkITKDemonsRegistrationFilter()
{
  NumIterations = 100;
  StandardDeviations = 1.0;
  CurrentIteration = 0;
}

vtkITKRegistrationFilter::OutputImageType::Pointer vtkITKDemonsRegistrationFilter::GetTransformedOutput()
{
  return m_Warper->GetOutput();
}

vtkITKDeformableRegistrationFilter::DeformationFieldType::Pointer vtkITKDemonsRegistrationFilter::GetDisplacementOutput()
{
  return m_Filter->GetOutput();
}

void vtkITKDemonsRegistrationFilter::CreateRegistrationPipeline()
{
  // registration pipeline

  // first match the intensities of two images
  m_Matcher = MatchingFilterType::New();

  m_Matcher->SetInput( itkImporterMoving->GetOutput() );
  m_Matcher->SetReferenceImage( itkImporterFixed->GetOutput() );

  m_Matcher->SetNumberOfHistogramLevels( 1024 );
  m_Matcher->SetNumberOfMatchPoints( 7 );
  m_Matcher->ThresholdAtMeanIntensityOn();

  // create Demons m_Filter
  m_Filter = RegistrationFilterType::New();

  // Create the Command observer and register it with the registration m_Filter.
  DemonsRegistrationFilterCommand::Pointer observer = DemonsRegistrationFilterCommand::New();
  observer->SetDemonsRegistrationFilter(this);
  m_Filter->AddObserver( itk::IterationEvent(), observer );

  // set registration input
  m_Filter->SetFixedImage( itkImporterFixed->GetOutput() );
  m_Filter->SetMovingImage( m_Matcher->GetOutput() );

  // create wrapper
  m_Warper = WarperType::New();
  m_Interpolator = InterpolatorType::New();

  m_Warper->SetInput( itkImporterMoving->GetOutput() );
  m_Warper->SetInterpolator( m_Interpolator );
  m_Warper->SetDeformationField( m_Filter->GetOutput() );

  LinkITKProgressToVTKProgress(m_Filter);
};

void vtkITKDemonsRegistrationFilter::UpdateRegistrationParameters()
{
  // set registration parameters
  if (m_Filter->GetNumberOfIterations() != NumIterations ) {
    m_Filter->SetNumberOfIterations( NumIterations );
  }
  const double *stddev = m_Filter->GetStandardDeviations();
  if ( stddev[0] != StandardDeviations ) {
    m_Filter->SetStandardDeviations( StandardDeviations );
  }

  InputImageType::Pointer fixedImage = this->itkImporterFixed->GetOutput();
  m_Warper->SetOutputSpacing( fixedImage->GetSpacing() );
  m_Warper->SetOutputOrigin( fixedImage->GetOrigin() );
}



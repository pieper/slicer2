#include "vtkITKDemonsRegistrationFilter.h" // This class

vtkITKDemonsRegistrationFilter::vtkITKDemonsRegistrationFilter()
{
  NumIterations = 100;
  StandardDeviations = 1.0;
  CurrentIteration = 0;

  // set pipeline for fixed image
  this->vtkCast = vtkImageCast::New();
  this->vtkExporter = vtkImageExport::New();
  this->vtkCast->SetOutputScalarTypeToFloat();
  this->vtkExporter->SetInput ( this->vtkCast->GetOutput() );

  this->itkImporterFixed = ImageImportType::New();
  ConnectPipelines(this->vtkExporter, this->itkImporterFixed);

  // set pipeline for movin image
  this->vtkCastMoving = vtkImageCast::New();
  this->vtkExporterMoving = vtkImageExport::New();
  this->vtkCastMoving->SetOutputScalarTypeToFloat();
  this->vtkExporterMoving->SetInput ( this->vtkCastMoving->GetOutput() );

  this->itkImporterMoving = ImageImportType::New();
  ConnectPipelines(this->vtkExporterMoving, this->itkImporterMoving);

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

  // Set pipline for output transformed image
  this->itkExporterTransformed = ImageExportType::New();
  ConnectPipelines(this->itkExporterTransformed, this->vtkImporter);
  this->itkExporterTransformed->SetInput (m_Warper->GetOutput() );


  // Set pipline for output displacement image
  this->vtkImporterDisplacement = vtkImageImport::New();
  this->itkExporterDisplacement = DeformationExportType::New();
  ConnectPipelines(this->itkExporterDisplacement, this->vtkImporterDisplacement);
  this->itkExporterDisplacement->SetInput(m_Filter->GetOutput());

  //Update();

};

vtkITKDemonsRegistrationFilter::~vtkITKDemonsRegistrationFilter()
{
  this->vtkExporterMoving->Delete();
  this->vtkCastMoving->Delete();

  this->vtkImporterDisplacement->Delete();
};

void vtkITKDemonsRegistrationFilter::Update()
{
  // set registration parameters
  m_Filter->SetNumberOfIterations( NumIterations );
  m_Filter->SetStandardDeviations( StandardDeviations );


  InputImageType::Pointer fixedImage = this->itkImporterFixed->GetOutput();
  m_Warper->SetOutputSpacing( fixedImage->GetSpacing() );
  m_Warper->SetOutputOrigin( fixedImage->GetOrigin() );

  SetCurrentIteration(0);

  vtkITKImageToImageFilter::Update();
}


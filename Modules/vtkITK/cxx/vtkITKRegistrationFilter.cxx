#include "vtkITKRegistrationFilter.h" // This class
void vtkITKRegistrationFilter::InitializePipeline()
{
    CurrentIteration = 0;
    ConnectInputPipelines();
    CreateRegistrationPipeline();
    ConnectOutputPipelines();
}
void vtkITKRegistrationFilter::ConnectInputPipelines()
{
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
}

void vtkITKRegistrationFilter::ConnectOutputPipelines()
{
  // Set pipline for output transformed image
  this->itkExporterTransformed = ImageExportType::New();
  ConnectPipelines(this->itkExporterTransformed, this->vtkImporter);
  this->itkExporterTransformed->SetInput ( GetTransformedOutput() );

};

vtkITKRegistrationFilter::~vtkITKRegistrationFilter()
{
  this->vtkExporterMoving->Delete();
  this->vtkCastMoving->Delete();
};

void vtkITKRegistrationFilter::Update()
{
  this->itkImporterMoving->Update();
  this->itkImporterFixed->Update();

  this->UpdateRegistrationParameters();

  this->SetCurrentIteration(0);

  vtkITKImageToImageFilter::Update();

  vtkImporter->Update();
}


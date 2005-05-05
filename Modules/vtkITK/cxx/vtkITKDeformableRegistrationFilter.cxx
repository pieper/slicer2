#include "vtkITKDeformableRegistrationFilter.h" // This class

void vtkITKDeformableRegistrationFilter::ConnectOutputPipelines()
{

  vtkITKRegistrationFilter::ConnectOutputPipelines();

  // Set pipline for output displacement image
  this->vtkImporterDisplacement = vtkImageImport::New();
  this->itkExporterDisplacement = DeformationExportType::New();
  ConnectPipelines(this->itkExporterDisplacement, this->vtkImporterDisplacement);
  this->itkExporterDisplacement->SetInput(GetDisplacementOutput());
};

vtkITKDeformableRegistrationFilter::~vtkITKDeformableRegistrationFilter()
{
  this->vtkImporterDisplacement->Delete();
};

void vtkITKDeformableRegistrationFilter::Update()
{
  vtkITKRegistrationFilter::Update();

  vtkImporterDisplacement->Update();
}


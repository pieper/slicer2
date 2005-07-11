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

  this->itkFixedImageWriter         = FixedWriterType::New();
  this->itkMovingImageWriter        = MovingWriterType::New();

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

  //vtkImporter->Update();
}

void vtkITKRegistrationFilter::WriteFixedImage(char* filename)
{
  this->itkFixedImageWriter->SetInput(this->itkImporterFixed->GetOutput());
  this->itkFixedImageWriter->SetFileName( filename );
  this->itkFixedImageWriter->Update();
}

void vtkITKRegistrationFilter::WriteMovingImage(char* filename)
{
  this->itkMovingImageWriter->SetInput(this->itkImporterMoving->GetOutput());
  this->itkMovingImageWriter->SetFileName( filename );
  this->itkMovingImageWriter->Update();
}

void vtkITKRegistrationFilter::WriteFixedImageInfo(char* filename)
{
  std::ofstream ofs(filename);
  InputImageType::SpacingType spacing = (this->itkImporterFixed->GetOutput()->GetSpacing());
  InputImageType::PointType origin = (this->itkImporterFixed->GetOutput()->GetOrigin());
  ofs << "Origin = " << origin << std::endl;
  ofs << "Spacing = " << spacing << std::endl;
  ofs.close();
}

void vtkITKRegistrationFilter::WriteMovingImageInfo(char* filename)
{
  std::ofstream ofs(filename);
  InputImageType::SpacingType spacing = (this->itkImporterFixed->GetOutput()->GetSpacing());
  InputImageType::PointType origin = (this->itkImporterFixed->GetOutput()->GetOrigin());
  ofs << "Origin = " << origin << std::endl;
  ofs << "Spacing = " << spacing << std::endl;
  ofs.close();
}



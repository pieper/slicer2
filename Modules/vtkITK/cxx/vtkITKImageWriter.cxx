#include "vtkITKImageWriter.h"


#include "vtkImageExport.h"
#include "vtkITKUtility.h"
#include "vtkImageFlip.h"

#include "itkVTKImageImport.h"
#include "itkImageFileWriter.h"

//vtkStandardNewMacro(vtkITKImageWriter);

// helper function
template <class  TPixelType>
void ITKWriteVTKImage(vtkImageData *inputImage, char *fileName, TPixelType dummy) {

  typedef  itk::Image<TPixelType, 3> ImageType;

  // itk import for input itk images
  typedef itk::VTKImageImport<ImageType> ImageImportType;
  ImageImportType::Pointer itkImporter = ImageImportType::New();

  // vtk export for  vtk image
  vtkImageExport* vtkExporter = vtkImageExport::New();  
  vtkImageFlip* vtkFlip = vtkImageFlip::New();

  // writer 
  typedef itk::ImageFileWriter<ImageType> ImageWriterType;      
  ImageWriterType::Pointer   itkImageWriter =  ImageWriterType::New();

  // set pipeline for the image
  vtkFlip->SetInput( inputImage );
  vtkFlip->SetFilteredAxis(1);
  vtkFlip->FlipAboutOriginOn();

  vtkExporter->SetInput ( vtkFlip->GetOutput() );

  itkImporter = ImageImportType::New();
  ConnectPipelines(vtkExporter, itkImporter);

  // write image
  itkImageWriter->SetInput(itkImporter->GetOutput());
  itkImageWriter->SetFileName( fileName );
  itkImageWriter->Update();

  // clean up
  vtkExporter->Delete();
  vtkFlip->Delete();
}


//----------------------------------------------------------------------------
vtkITKImageWriter::vtkITKImageWriter()
{
  this->FileName = NULL;
}


//----------------------------------------------------------------------------
vtkITKImageWriter::~vtkITKImageWriter()
{
  // get rid of memory allocated for file names
  if (this->FileName) {
    delete [] this->FileName;
    this->FileName = NULL;
  }
}


//----------------------------------------------------------------------------
void vtkITKImageWriter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "FileName: " <<
    (this->FileName ? this->FileName : "(none)") << "\n";
}


//----------------------------------------------------------------------------
void vtkITKImageWriter::SetInput(vtkImageData *input)
{
  this->vtkProcessObject::SetNthInput(0, input);
}

//----------------------------------------------------------------------------
vtkImageData *vtkITKImageWriter::GetInput()
{
  if (this->NumberOfInputs < 1) {
    return NULL;
  }
  
  return (vtkImageData *)(this->Inputs[0]);
}



//----------------------------------------------------------------------------
// This function sets the name of the file. 
void vtkITKImageWriter::SetFileName(const char *name)
{
  if ( this->FileName && name && (!strcmp(this->FileName,name))) {
    return;
  }
  if (!name && !this->FileName) {
    return;
  }
  if (this->FileName) {
    delete [] this->FileName;
  }

  this->FileName = new char[strlen(name) + 1];
  strcpy(this->FileName, name);
  this->Modified();
}

//----------------------------------------------------------------------------
// Writes all the data from the input.
void vtkITKImageWriter::Write()
{
  if ( this->GetInput() == NULL ) {
    return;
  }
  if ( ! this->FileName ) {
    vtkErrorMacro(<<"vtkITKImageWriter: Please specify a FileName");
    return;
  }

  this->GetInput()->UpdateInformation();
  this->GetInput()->SetUpdateExtent(this->GetInput()->GetWholeExtent());

  // take into consideration the scalar type
  switch (this->GetInput()->GetScalarType())
    {
    case VTK_DOUBLE:
      {
        double pixelType=0;
        ITKWriteVTKImage(this->GetInput(), this->GetFileName(), pixelType);
      }
      break;
    case VTK_FLOAT:
      {
        float pixelType=0;
        ITKWriteVTKImage(this->GetInput(), this->GetFileName(), pixelType);
      }
      break;
    case VTK_LONG:
      {
        long pixelType=0;
        ITKWriteVTKImage(this->GetInput(), this->GetFileName(), pixelType);
      }
      break;
    case VTK_UNSIGNED_LONG:
      {
        unsigned long pixelType=0;
        ITKWriteVTKImage(this->GetInput(), this->GetFileName(), pixelType);
      }
      break;
    case VTK_INT:
      {
        int pixelType=0;
        ITKWriteVTKImage(this->GetInput(), this->GetFileName(), pixelType);
      }
      break;
    case VTK_UNSIGNED_INT:
      {
        unsigned int pixelType=0;
        ITKWriteVTKImage(this->GetInput(), this->GetFileName(), pixelType);
      }
      break;
    case VTK_SHORT:
      {
        short pixelType=0;
        ITKWriteVTKImage(this->GetInput(), this->GetFileName(), pixelType); 
      }
      break;
    case VTK_UNSIGNED_SHORT:
      {
        unsigned short pixelType=0;
        ITKWriteVTKImage(this->GetInput(), this->GetFileName(), pixelType);
      }
      break;
    case VTK_CHAR:
      {
        char pixelType=0;
        ITKWriteVTKImage(this->GetInput(), this->GetFileName(), pixelType);
      }
      break;
    case VTK_UNSIGNED_CHAR:
      {
        unsigned char pixelType=0;
        ITKWriteVTKImage(this->GetInput(), this->GetFileName(), pixelType);
      }
      break;
    default:
      vtkErrorMacro(<< "Execute: Unknown output ScalarType");
      return; 
    }
}

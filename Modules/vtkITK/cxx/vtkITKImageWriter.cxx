#include "vtkITKImageWriter.h"

#include <math.h>
#include "vtkImageExport.h"
#include "vtkITKUtility.h"
#include "vtkImageFlip.h"

#include "itkVTKImageImport.h"
#include "itkImageFileWriter.h"

//vtkStandardNewMacro(vtkITKImageWriter);

// helper function
template <class  TPixelType>
void ITKWriteVTKImage(vtkImageData *inputImage, char *fileName, 
                      vtkMatrix4x4* rasToIjkMatrix, TPixelType dummy) {

  typedef  itk::Image<TPixelType, 3> ImageType;

  vtkMatrix4x4 *ijkToRasMatrix = vtkMatrix4x4::New();

  vtkMatrix4x4::Invert(rasToIjkMatrix, ijkToRasMatrix);

  ijkToRasMatrix->Transpose();

  ImageType::DirectionType direction;
  direction.SetIdentity();

  double mag[3];
  for (int i=0; i<3; i++) {
    // normalize vectors
    mag[i] = 0;
    for (int j=0; j<3; j++) {
      mag[i] += ijkToRasMatrix->GetElement(i,j)* ijkToRasMatrix->GetElement(i,j);
    }
    if (mag[i] == 0.0) {
      mag[i] = 1;
    }
    mag[i] = sqrt(mag[i]);
    if (i == 1) { // Y flip
      mag[i] = -mag[i];
    }
  }
  for ( i=0; i<3; i++) {
    for (int j=0; j<3; j++) {
      direction[j][i] =  ijkToRasMatrix->GetElement(i,j)/mag[i];
    }
  }
  
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
  itkImporter->GetOutput()->SetDirection(direction);
  itkImageWriter->SetFileName( fileName );
  itkImageWriter->Update();

  // clean up
  vtkExporter->Delete();
  vtkFlip->Delete();
  ijkToRasMatrix->Delete();
}


//----------------------------------------------------------------------------
vtkITKImageWriter::vtkITKImageWriter()
{
  this->FileName = NULL;
  this->RasToIJKMatrix = NULL;
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
        ITKWriteVTKImage(this->GetInput(), this->GetFileName(), this->RasToIJKMatrix, pixelType);
      }
      break;
    case VTK_FLOAT:
      {
        float pixelType=0;
        ITKWriteVTKImage(this->GetInput(), this->GetFileName(), this->RasToIJKMatrix, pixelType);
      }
      break;
    case VTK_LONG:
      {
        long pixelType=0;
        ITKWriteVTKImage(this->GetInput(), this->GetFileName(), this->RasToIJKMatrix, pixelType);
      }
      break;
    case VTK_UNSIGNED_LONG:
      {
        unsigned long pixelType=0;
        ITKWriteVTKImage(this->GetInput(), this->GetFileName(), this->RasToIJKMatrix, pixelType);
      }
      break;
    case VTK_INT:
      {
        int pixelType=0;
        ITKWriteVTKImage(this->GetInput(), this->GetFileName(), this->RasToIJKMatrix, pixelType);
      }
      break;
    case VTK_UNSIGNED_INT:
      {
        unsigned int pixelType=0;
        ITKWriteVTKImage(this->GetInput(), this->GetFileName(), this->RasToIJKMatrix, pixelType);
      }
      break;
    case VTK_SHORT:
      {
        short pixelType=0;
        ITKWriteVTKImage(this->GetInput(), this->GetFileName(), this->RasToIJKMatrix, pixelType); 
      }
      break;
    case VTK_UNSIGNED_SHORT:
      {
        unsigned short pixelType=0;
        ITKWriteVTKImage(this->GetInput(), this->GetFileName(), this->RasToIJKMatrix, pixelType);
      }
      break;
    case VTK_CHAR:
      {
        char pixelType=0;
        ITKWriteVTKImage(this->GetInput(), this->GetFileName(), this->RasToIJKMatrix, pixelType);
      }
      break;
    case VTK_UNSIGNED_CHAR:
      {
        unsigned char pixelType=0;
        ITKWriteVTKImage(this->GetInput(), this->GetFileName(), this->RasToIJKMatrix, pixelType);
      }
      break;
    default:
      vtkErrorMacro(<< "Execute: Unknown output ScalarType");
      return; 
    }
}



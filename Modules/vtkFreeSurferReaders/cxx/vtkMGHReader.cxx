/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkMGHReader.cxx,v $
  Language:  C++
  Date:      $Date: 2003/02/19 23:55:50 $
  Version:   $Revision: 1.1 $

=========================================================================*/
#include "vtkMGHReader.h"
#include "vtkShortArray.h"
#include "vtkUnsignedCharArray.h"
#include "vtkFloatArray.h"
#include "vtkIntArray.h"
#include "vtkObjectFactory.h"
#include "vtkFSIO.h"

vtkMGHReader* vtkMGHReader::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMGHReader");
  if(ret)
    {
    return (vtkMGHReader*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkMGHReader;
}

vtkMGHReader::vtkMGHReader()
{
  this->DataDimensions[0] = 
    this->DataDimensions[1] = 
    this->DataDimensions[2] = 0;
  this->FileName = NULL;
  this->ScalarType = 0;
  this->NumFrames = 0;
  this->CurFrame = 0;
}

vtkMGHReader::~vtkMGHReader()
{
  if (this->FileName) {
    delete [] this->FileName;
  }
}


void vtkMGHReader::ExecuteInformation()
{
  vtkImageData *output = this->GetOutput();
  
  // Read the header.
  this->ReadVolumeHeader();


  // Set some data in the output.
  output->SetWholeExtent(0, this->DataDimensions[0]-1,
             0, this->DataDimensions[1]-1,
             0, this->DataDimensions[2]-1 );
  output->SetScalarType(this->ScalarType);
  output->SetNumberOfScalarComponents(1);
  output->SetSpacing(this->DataSpacing);
  output->SetOrigin(this->DataOrigin);
}
    
    
void vtkMGHReader::Execute()
{
  vtkImageData *output = this->GetOutput();

  // Read the header.
  this->ReadVolumeHeader();


  // Set some data in the output.
  output->SetWholeExtent(0, this->DataDimensions[0]-1,
             0, this->DataDimensions[1]-1,
             0, this->DataDimensions[2]-1 );
  output->SetScalarType(this->ScalarType);
  output->SetNumberOfScalarComponents(1);
  output->SetDimensions(this->DataDimensions);
  output->SetSpacing(this->DataSpacing);
  output->SetOrigin(this->DataOrigin);

  // Get the volume values from the MGH files. If we get them, copy
  // them to the output.
  vtkDataArray *newScalars = this->ReadVolumeData();
  if ( newScalars ) 
    {
      output->GetPointData()->SetScalars(newScalars);
      newScalars->Delete();
    }
}

vtkImageData *vtkMGHReader::GetImage(int ImageNumber)
{
  cerr << "vtkMGHReader::GetImage() called. uh oh." << endl;
  return NULL;
}

vtkDataArray *vtkMGHReader::ReadVolumeData()
{
  vtkDataArray          *scalars = NULL;
  vtkUnsignedCharArray  *ucharScalars = NULL;
  vtkShortArray         *shortScalars = NULL;
  vtkIntArray           *intScalars = NULL;
  vtkFloatArray         *floatScalars = NULL;
  void* destData;
  FILE *fp;
  int numRead;
  int numPts;
  int numPtsPerSlice;
  int numReadTotal;
  int slice;
  int elementSize;
  short s;
  int i;
  float f;

  // Check the file name.
  if( NULL == this->FileName || 
      (0 == strlen( this->FileName )) ) {
    vtkErrorMacro( << "No file name specified" );
    return NULL;
  }

  // Read header first.
  this->ReadVolumeHeader();

  // Calc the number of values.
  numPts = this->DataDimensions[0] * 
    this->DataDimensions[1] * 
    this->DataDimensions[2];

  // Create the scalar array for the volume. Set the element size for
  // the data we will read. Get a writable pointer to the scalar data
  // so we can read data into it.
  switch ( this->ScalarType ) {
  case VTK_UNSIGNED_CHAR:
    vtkDebugMacro (<< "Creating vtkUnsignedCharArray");
    ucharScalars = vtkUnsignedCharArray::New();
    ucharScalars->Allocate(numPts);
    destData = (void*) ucharScalars->WritePointer(0, numPts);
    scalars = (vtkDataArray*) ucharScalars;
    elementSize = sizeof( char );
    break;
  case VTK_SHORT:
    vtkDebugMacro (<< "Creating vtkShortArray");
    shortScalars = vtkShortArray::New();
    shortScalars->Allocate(numPts);
    destData = (void*) shortScalars->WritePointer(0, numPts);
    scalars = (vtkDataArray*) shortScalars;
    elementSize = sizeof( short );
    break;
  case VTK_INT:
    vtkDebugMacro (<< "Creating vtkIntArray");
    intScalars = vtkIntArray::New();
    intScalars->Allocate(numPts);
    destData = (void*) intScalars->WritePointer(0, numPts);
    scalars = (vtkDataArray*) intScalars;
    elementSize = sizeof( int );
    break;
  case VTK_FLOAT:
    vtkDebugMacro (<< "Creating vtkFloatArray");
    floatScalars = vtkFloatArray::New();
    floatScalars->Allocate(numPts);
    destData = (void*) floatScalars->WritePointer(0, numPts);
    scalars = (vtkDataArray*) floatScalars;
    elementSize = sizeof( float );
    break;
  default:
    vtkErrorMacro(<< "Volume type not supported.");
    return NULL;
  }
  if ( NULL == scalars ) {
    vtkErrorMacro(<< "Couldn't allocate scalars array.");
    return NULL;
  } 
  

  // Open the file.
  fp = fopen( this->FileName, "rb" );
  if( !fp ) {
    vtkErrorMacro(<< "Can't find/open file: " << this->FileName);
    return NULL;
  }

  // Skip all the header information and the frames we don't want.
  fseek( fp, FS_WHOLE_HEADER_SIZE + (this->CurFrame * numPts), SEEK_SET );

  // Read in a frame. We need to do this element by element so we can
  // do byte swapping, except for the uchars because they don't need
  // it.
  vtkDebugMacro(<< "Reading volume data" );
  if( VTK_UNSIGNED_CHAR == this->ScalarType ) {
    numRead = fread( destData, elementSize, numPts, fp );
    if ( numRead != numPts ) {
      vtkErrorMacro(<<"Trying to read " << numPts << " elements, "
            "but only got " << numRead << " of them.");
      scalars->Delete();
      return NULL;
    }
  } else {
    for( int nZ = 0; nZ < this->DataDimensions[2]; nZ++ ) {
      for( int nY = 0; nY < this->DataDimensions[1]; nY++ ) {
    for( int nX = 0; nX < this->DataDimensions[0]; nX++ ) {
      switch ( this->ScalarType ) {
      case VTK_SHORT:
        vtkFSIO::ReadShort( fp, s );
        *(short*)destData = s;
        (short*)destData += 1;
        break;
      case VTK_INT:
        vtkFSIO::ReadInt( fp, i );
        *(int*)destData = i;
        (int*)destData += 1;
        break;
      case VTK_FLOAT:
        vtkFSIO::ReadFloat( fp, f );
        *(float*)destData = f;
        (float*)destData += 1;
        break;
      default:
        vtkErrorMacro(<< "Volume type not supported.");
        return NULL;
      }
    }
      }
    }
  }

  // Close the file.
  fclose(fp);
  
  // return the scalars.
  return scalars;
}

void vtkMGHReader::ReadVolumeHeader()
{
  FILE *fp;
  int version;
  int type;
  int dof;
  short RASgood;

  // Check the file name.
  if( NULL == this->FileName || 
      (0 == strlen( this->FileName )) ) {
    vtkErrorMacro( << "No file name specified" );
    return;
  }

  // Open the file.
  fp = fopen( this->FileName, "rb" );
  if( !fp ) {
    vtkErrorMacro(<< "Can't find/open file: " << this->FileName);
    return;
  }

  // Read in dimension information.
  vtkFSIO::ReadInt( fp, version );
  vtkFSIO::ReadInt( fp, this->DataDimensions[0] );
  vtkFSIO::ReadInt( fp, this->DataDimensions[1] );
  vtkFSIO::ReadInt( fp, this->DataDimensions[2] );
  vtkFSIO::ReadInt( fp, this->NumFrames );
  vtkFSIO::ReadInt( fp, type );
  vtkFSIO::ReadInt( fp, dof );

  // Convert the type to a VTK scalar type.
  switch( type ) {
  case MRI_UCHAR: this->ScalarType = VTK_UNSIGNED_CHAR; break;
  case MRI_INT: this->ScalarType = VTK_INT; break;
  case MRI_FLOAT: this->ScalarType = VTK_FLOAT; break;
  case MRI_SHORT: this->ScalarType = VTK_SHORT; break;
  default:
    cerr << "Using float by default" << endl;
    this->ScalarType = VTK_FLOAT;
  }

  // The next short is says whether the RAS registration information
  // is good. If so, read in the voxel size and then the matrix.
  vtkFSIO::ReadShort( fp, RASgood );
  if( RASgood ) {

    for( int nSpacing = 0; nSpacing < 3; nSpacing++ ) {
      vtkFSIO::ReadFloat( fp, this->DataSpacing[nSpacing] );
    }

    // x_r x_a x_s
    // y_r y_a y_s
    // z_r z_a z_s
    // c_r c_a c_s
    for( int nMatrix = 0; nMatrix < 12; nMatrix++ ) {
      vtkFSIO::ReadFloat( fp, this->RASMatrix[nMatrix] );
    }
  }

  fclose(fp);
}

void vtkMGHReader::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkVolumeReader::PrintSelf( os, indent );

  os << indent << "Data Dimensions: (" << this->DataDimensions[0] << ", "
     << this->DataDimensions[1] << ", " << this->DataDimensions[2] << ")\n";
  os << indent << "Data Spacing: (" << this->DataSpacing[0] << ", "
     << this->DataSpacing[1] << ", " << this->DataSpacing[2] << ")\n";
  os << indent << "Scalar type: " << this->ScalarType << endl;
  os << indent << "Number of Frames: " << this->NumFrames << endl;
  os << indent << "Current Frame: " << this->CurFrame << endl;
}

/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkFSSurfaceScalarReader.cxx,v $
  Language:  C++
  Date:      $Date: 2002/10/07 20:39:51 $
  Version:   $Revision: 1.1 $

=========================================================================*/
#include "vtkFSSurfaceScalarReader.h"
#include "vtkObjectFactory.h"
#include "vtkFSIO.h"
#include "vtkByteSwap.h"
#include "vtkFloatArray.h"

//-------------------------------------------------------------------------
vtkFSSurfaceScalarReader* vtkFSSurfaceScalarReader::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkFSSurfaceScalarReader");
  if(ret)
    {
    return (vtkFSSurfaceScalarReader*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkFSSurfaceScalarReader;
}

vtkFSSurfaceScalarReader::vtkFSSurfaceScalarReader()
{
}

vtkFSSurfaceScalarReader::~vtkFSSurfaceScalarReader()
{
}


void vtkFSSurfaceScalarReader::Execute()
{
  FILE* scalarFile;
  int magicNumber;
  int numValues = 0;
  int numFaces = 0;
  int numValuesPerPoint = 0;
  int vIndex;
  int ivalue;
  float fvalue;
  float *scalars;
  vtkFloatArray *output = this->GetOutput();

  cerr << "vtkFSSurfaceScalarReader Execute() " << endl;

  if (!this->FileName) {
    vtkErrorMacro(<<"vtkFSSurfaceScalarReader Execute: FileName not specified.");
    return;
  }

  vtkDebugMacro(<<"Reading surface scalar data...");

  // Try to open the file.
  scalarFile = fopen(this->FileName, "rb") ;
  if (!scalarFile) {
    vtkErrorMacro (<< "Could not open file " << this->FileName);
    return;
  }  


  // In the old file type, there is no magin number; the first three
  // byte int is the number of values. In the (more common) new type,
  // the first three bytes is the magic number. So read three bytes
  // and assume it's a magic number, check and assign it to the number
  // of values if not. New style files also have a number of faces and
  // values per point, which aren't really used.
  vtkFSIO::ReadInt3 (scalarFile, magicNumber);
  if (FS_NEW_SCALAR_MAGIC_NUMBER == magicNumber) {
    fread (&numValues, sizeof(int), 1, scalarFile);
    fread (&numFaces, sizeof(int), 1, scalarFile);
    fread (&numValuesPerPoint, sizeof(int), 1, scalarFile);
    vtkByteSwap::Swap4BE (&numValues);
    vtkByteSwap::Swap4BE (&numFaces);
    vtkByteSwap::Swap4BE (&numValuesPerPoint);
    
    if (numValuesPerPoint != 1) {
      vtkErrorMacro (<< "vtkFSSurfaceScalarReader.cxx Execute: Number of values per point is not 1, can't process file.");
      return;
    }

  } else {
    numValues = magicNumber;
  }

  if (numValues < 0) {
    vtkErrorMacro (<< "vtkFSSurfaceScalarReader.cxx Execute: Number of vertices is 0 or negative, can't process file.");
      return;
  }
   
  // Make our float array.
  scalars = (float*) calloc (numValues, sizeof(float));

  // For each value, if it's a new style file read a float, otherwise
  // read a two byte int and divide it by 100. Add this value to the
  // array.
  for (vIndex = 0; vIndex < numValues; vIndex ++ ) {

    if (feof(scalarFile)) {
      vtkErrorMacro (<< "vtkFSSurfaceScalarReader.cxx Execute: Unexpected EOF after " << vIndex << " values read.");
      return;
    }

    if (FS_NEW_SCALAR_MAGIC_NUMBER == magicNumber) {
      fread (&fvalue, sizeof(float), 1, scalarFile);
      vtkByteSwap::Swap4BE (&fvalue);

    } else {
      vtkFSIO::ReadInt2 (scalarFile, ivalue);
      fvalue = ivalue / 100.0;
    }

    scalars[vIndex] = fvalue;
  }

  // Close the file.
  fclose (scalarFile);

  // Set the array in our output.
  output->SetArray (scalars, VTK_FLOAT, 1);
}

void vtkFSSurfaceScalarReader::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkDataReader::PrintSelf(os,indent);
}

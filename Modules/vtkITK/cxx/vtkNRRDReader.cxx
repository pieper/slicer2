/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkNRRDReader.cxx,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkNRRDReader.h"

#include <string>

#include "vtkByteSwap.h"
#include "vtkImageData.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"

vtkCxxRevisionMacro(vtkNRRDReader, "$Revision: 1.16 $");
vtkStandardNewMacro(vtkNRRDReader);

vtkNRRDReader::vtkNRRDReader() 
{
  RasToIjkMatrix = NULL;
  MeasurementFrameMatrix = NULL;
  HeaderKeys = NULL;
  CurrentFileName = NULL;
  nrrd = nrrdNew();
}

vtkNRRDReader::~vtkNRRDReader() 
{
  if (RasToIjkMatrix) {
    RasToIjkMatrix->Delete();
    RasToIjkMatrix = NULL;
  }
  
  if (MeasurementFrameMatrix) {
    MeasurementFrameMatrix->Delete();
    MeasurementFrameMatrix = NULL;
  }  
  
  if (HeaderKeys) {
    delete [] HeaderKeys;
    HeaderKeys = NULL;
  }
  if (CurrentFileName) {
    delete [] CurrentFileName;
    CurrentFileName = NULL;
  }
  nrrdNuke(nrrd);
}

vtkMatrix4x4* vtkNRRDReader::GetRasToIjkMatrix()
{
  this->ExecuteInformation();
  return RasToIjkMatrix;
}

vtkMatrix4x4* vtkNRRDReader::GetMeasurementFrameMatrix()
{
  this->ExecuteInformation();
  return MeasurementFrameMatrix;
}
  
char* vtkNRRDReader::GetHeaderKeys()
{
  std::string keys;
  for (std::map<std::string,std::string>::iterator i = HeaderKeyValue.begin();
       i != HeaderKeyValue.end(); i++) {
    std::string s = static_cast<std::string> (i->first);
    if (i != HeaderKeyValue.begin()){
      keys = keys + " ";
    }
    keys = keys + s;
  }
  if (HeaderKeys) {
    delete [] HeaderKeys;
  }
  HeaderKeys = NULL;

  if (keys.size() > 0) {
    HeaderKeys = new char[keys.size()+1];
    strcpy(HeaderKeys, keys.c_str());
  }
  return HeaderKeys;
}

char* vtkNRRDReader::GetHeaderValue(char *key)
{
  std::map<std::string,std::string>::iterator i = HeaderKeyValue.find(key);
  if (i != HeaderKeyValue.end()) {
    return (char *)(i->second.c_str());
  }
  else {
    return NULL;
  }
}

int vtkNRRDReader::CanReadFile(const char* filename)
{
#ifndef NRRD_SPACE_DIM_MAX 
    return false;
#else
  // Check the extension first to avoid opening files that do not
  // look like nrrds.  The file must have an appropriate extension to be
  // recognized.
  std::string fname = filename;
  if(  fname == "" )
    {
    vtkDebugMacro(<<"No filename specified.");
    return false;
    }

  // We'll assume we can read from stdin (don't try to read the header though)
  if ( fname == "-" )
    {
    return true;
    }

  bool extensionFound = false;
  std::string::size_type nrrdPos = fname.rfind(".nrrd");
  if ((nrrdPos != std::string::npos)
      && (nrrdPos == fname.length() - 5))
    {
    extensionFound = true;
    }

  std::string::size_type nhdrPos = fname.rfind(".nhdr");
  if ((nhdrPos != std::string::npos)
      && (nhdrPos == fname.length() - 5))
    {
    extensionFound = true;
    }

  if( !extensionFound )
    {
    vtkDebugMacro(<<"The filename extension is not recognized");
    return false;
    }

  // We have the correct extension, so now check for the Nrrd magic.
  std::ifstream inputStream;

  inputStream.open( filename, std::ios::in | std::ios::binary );

  if( inputStream.fail() )
    {
    return false;
    }

  char key[8000];

  inputStream >> key;

  if( inputStream.eof() )
    {
    inputStream.close();
    return false;
    }

  // Ignores the version number of the NRRD file.  Version is checked when the
  // file is read. For now we only want to report whether this is or is not a
  // NRRD file.
  if( strncmp(key,"NRRD", 4)==0 ) 
    {
    inputStream.close();
    return true;
    }

  inputStream.close();
  return false;
#endif
}


void vtkNRRDReader::ExecuteInformation()
{
#ifndef NRRD_SPACE_DIM_MAX 
    return;
#else
   // This method determines the following and sets the appropriate value in
   // the parent IO class:
   //
   // binary/ascii file type 
   // endianness
   // pixel type
   // pixel component type
   // number of pixel components
   // number of image dimensions
   // image spacing
   // image origin
   // meta data dictionary information

   int i;
   char *key = NULL;
   char *val = NULL;
   char *err;
   NrrdIoState *nio;

   // save the Nrrd struct for the current file and 
   // don't re-execute the read unless the filename changes
   if ( this->CurrentFileName != NULL &&
            !strcmp (this->CurrentFileName, this->GetFileName()) )
   {
       // filename hasn't changed, don't re-execute
       return;
   }

   if ( this->CurrentFileName != NULL )
   {
        delete [] this->CurrentFileName;
   }

   this->CurrentFileName = new char[1 + strlen(this->GetFileName())];
   strcpy (this->CurrentFileName, this->GetFileName());

   nrrdNuke(this->nrrd); // nuke and reallocate to reset the state
   this->nrrd = nrrdNew();

   HeaderKeyValue.clear();

   if (RasToIjkMatrix) {
     RasToIjkMatrix->Delete();
   }
   RasToIjkMatrix = vtkMatrix4x4::New();
   vtkMatrix4x4* IjkToRasMatrix = vtkMatrix4x4::New();

   RasToIjkMatrix->Identity();
   IjkToRasMatrix->Identity();

  if (MeasurementFrameMatrix) {
     MeasurementFrameMatrix->Delete();
  }
  MeasurementFrameMatrix = vtkMatrix4x4::New();
  MeasurementFrameMatrix->Identity();   

   nio = nrrdIoStateNew();

   if (nrrdLoad(this->nrrd, this->GetFileName(), nio) != 0) {
     err = biffGetDone("nrrd");
     vtkErrorMacro("Error reading " << this->GetFileName() << ": " << err);
     free(err); // err points to malloc'd data!!
     //     err = NULL;
     return;
   }

   if ( nio->endian == airEndianLittle ) {
     this->SetDataByteOrderToLittleEndian();
   }
   else {
     this->SetDataByteOrderToBigEndian();
   }

   // NrrdIO only supports 3 dimensional image with scalar data.  
   // Future implementations may support
   // read/write of vector data.

   if ( this->nrrd->dim == 3) {
     // Assume that all dimensions are spacial
     // and there is 1 data scalar
     this->SetNumberOfScalarComponents(1);
  }
   else if ( this->nrrd->dim == 4) {
     // Assume that 3 first dimensions are spacial
     // and last dimension is the number of data components
     this->SetNumberOfScalarComponents(1);
   }
   else {
      vtkErrorMacro("Error reading " << this->GetFileName() << ": " << "only 3D/4D volumes supported");
     return;
   }

   // Set type information
   this->SetDataScalarType( this->NrrdToVTKScalarType(this->nrrd->type) );
   
   // Set axis information
   int dataExtent[6];
   vtkFloatingPointType spacings[3];
   vtkFloatingPointType origins[3];
   double axis[NRRD_SPACE_DIM_MAX];

   for (i=0; i < this->nrrd->dim; i++) {
     int kind =  this->nrrd->axis[i].kind;
     if (i < 3) {
       if (kind != nrrdKindSpace) {
         vtkWarningMacro("Reading " << this->GetFileName() << ": " << "dimension # " << i << " not labeled spatial");
       }
       // spacial dimesion
       dataExtent[2*i] = 0;
       dataExtent[2*i+1] = this->nrrd->axis[i].size - 1;  
       spacings[i] = this->nrrd->axis[i].spacing;
#if defined(TEEM_VERSION) && TEEM_VERSION >= 10900
       nrrdSpacingCalculate(this->nrrd, i, &spacings[i], axis);
#else
       int sdim;
       nrrdSpacingCalculate(this->nrrd, i, &spacings[i], &sdim, axis);
#endif
       if ( !AIR_EXISTS(spacings[i]) ) { // is the spacing NaN?
         spacings[i] = 1.0;
       }
       if ( AIR_EXISTS(this->nrrd->axis[i].min) ) { // is the min NaN?
         origins[i] = this->nrrd->axis[i].min;
       }
       else { // If min has not been set, assume a default.
         // An ITK image _must_ have a valid origin.
         origins[i] = 0;
       }
       
       // get IJK to RAS direction vector
       for (int j=0; j<this->nrrd->spaceDim; j++) {
         IjkToRasMatrix->SetElement(j, i, this->nrrd->axis[i].spaceDirection[j]);
       }
     }
     else { 
       // assume this is data dimension
       // combine with the last spacial dimension
       if (kind == nrrdKindSpace) {
         vtkWarningMacro("Reading " << this->GetFileName() << ": " << "dimension # " << i << " should not be spatial");
         return;
       }
       dataExtent[2*(i-1)+1] =  this->nrrd->axis[i-1].size * this->nrrd->axis[i].size - 1;
     }
   }

   vtkMatrix4x4::Invert(IjkToRasMatrix, RasToIjkMatrix);
   for (i=0; i < this->nrrd->dim; i++) {
       RasToIjkMatrix->SetElement(i, 3, (dataExtent[2*i+1] - dataExtent[2*i])/2.0);
   }
   RasToIjkMatrix->SetElement(3,3,1.0);
   IjkToRasMatrix->Delete();

   this->SetDataSpacing(spacings);
   this->SetDataOrigin(origins);
   this->SetDataExtent(dataExtent);

   // Push extra key/value pair data into an itkDataDictionary
   for (i=0; i < nrrdKeyValueSize(this->nrrd); i++) {
     nrrdKeyValueIndex(this->nrrd, &key, &val, i);
     HeaderKeyValue[std::string(key)] = std::string(val);
     free(key);  // key and val point to malloc'd data!!
     free(val);
     key = val = NULL;
   }
   HeaderKeyValue[std::string("space")] = std::string( NrrdGetSpaceString(this->nrrd) );
   
#if defined(TEEM_VERSION) && TEEM_VERSION >= 10900
   for (int j=0;j<2;j++)
     for (int i=0;i<2;i++)
        MeasurementFrameMatrix->SetElement(i,j,this->nrrd->measurementFrame[i][j]);
#endif
   
   
   this->vtkImageReader2::ExecuteInformation();
   
   nrrdIoStateNix(nio);
#endif
}


//----------------------------------------------------------------------------
// This function reads a data from a file.  The datas extent/axes
// are assumed to be the same as the file extent/order.
void vtkNRRDReader::ExecuteData(vtkDataObject *output)
{
#ifndef NRRD_SPACE_DIM_MAX 
    return;
#else
  output->SetUpdateExtentToWholeExtent();
  vtkImageData *data = this->AllocateOutputData(output);

  if (this->GetFileName() == NULL)
    {
    vtkErrorMacro(<< "Either a FileName or FilePrefix must be specified.");
    return;
    }

  if (this->nrrd->data == NULL)
    {
    vtkErrorMacro(<< "data is null.");
    return;
    }

  data->GetPointData()->GetScalars()->SetName("NRRDImage");

  this->ComputeDataIncrements();

  int dims[3];
  data->GetDimensions(dims);
  size_t datasize = data->GetScalarSize() * 
                    data->GetNumberOfScalarComponents() *
                    dims[0] * dims[1] * dims[2];

  void *ptr = data->GetScalarPointer();

  // The data buffer has already been allocated and read in UpdateInformation
  memcpy (ptr, this->nrrd->data, datasize);
#endif
}

//----------------------------------------------------------------------------
void vtkNRRDReader::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

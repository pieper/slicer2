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

vtkCxxRevisionMacro(vtkNRRDReader, "$Revision: 1.7.2.1 $");
vtkStandardNewMacro(vtkNRRDReader);

vtkNRRDReader::vtkNRRDReader() 
{
  RasToIjkMatrix = NULL;
  HeaderKeys = NULL;
}

vtkNRRDReader::~vtkNRRDReader() 
{
  if (RasToIjkMatrix) {
    RasToIjkMatrix->Delete();
    RasToIjkMatrix = NULL;
  }
  if (HeaderKeys) {
    delete [] HeaderKeys;
    HeaderKeys = NULL;
  }
}

vtkMatrix4x4* vtkNRRDReader::GetRasToIjkMatrix()
{
  this->ExecuteInformation();
  return RasToIjkMatrix;
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
   Nrrd *nrrd;

   HeaderKeyValue.clear();

   if (RasToIjkMatrix) {
     RasToIjkMatrix->Delete();
   }
   RasToIjkMatrix = vtkMatrix4x4::New();
   vtkMatrix4x4* IjkToRasMatrix = vtkMatrix4x4::New();

   RasToIjkMatrix->Identity();
   IjkToRasMatrix->Identity();

   nio = nrrdIoStateNew();
   nrrdIoStateSet(nio, nrrdIoStateSkipData, 1);

   nrrd = nrrdNew();
   if (nrrdLoad(nrrd, this->GetFileName(), nio) != 0) {
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

// Probably not needed
#if 0
   if ( nio->encoding == nrrdEncodingAscii )
     {
     this->SetFileTypeToASCII();
     }
   else
     {
     this->SetFileTypeToBinary();
     }
#endif
   
   // NrrdIO only supports 3 dimensional image with scalar data.  
   // Future implementations may support
   // read/write of vector data.

   if ( nrrd->dim == 3) {
     // Assume that all dimensions are spacial
     // and there is 1 data scalar
     this->SetNumberOfScalarComponents(1);
  }
   else if ( nrrd->dim == 4) {
     // Assume that 3 first dimensions are spacial
     // and last dimension is the number of data components
     this->SetNumberOfScalarComponents(1);
   }
   else {
      vtkErrorMacro("Error reading " << this->GetFileName() << ": " << "only 3D/4D volumes supported");
     return;
   }

   // Set type information
   //   this->SetPixelType( this->NrrdToITKComponentType(nrrd->type) );
   // For now we only support scalar reads/writes
   //this->SetPixelType( SCALAR );
   this->SetDataScalarType( this->NrrdToVTKScalarType(nrrd->type) );
   
   // Set axis information
   int dataExtent[6];
   vtkFloatingPointType spacings[3];
   vtkFloatingPointType origins[3];
   int sdim;
   double axis[NRRD_SPACE_DIM_MAX];

   for (i=0; i < nrrd->dim; i++) {
     int kind =  nrrd->axis[i].kind;
     if (i < 3) {
       if (kind != nrrdKindSpace) {
         // assume this is really meant to be spatial -- even though it's not specified
         // vtkErrorMacro("Error reading " << this->GetFileName() << ": " << "dimension # " << i << " must be spatial");
       }
       // spacial dimesion
       dataExtent[2*i] = 0;
       dataExtent[2*i+1] = nrrd->axis[i].size - 1;  
       spacings[i] = nrrd->axis[i].spacing;
       nrrdSpacingCalculate(nrrd, i, &spacings[i], &sdim, axis);
       if ( !AIR_EXISTS(spacings[i]) ) { // is the spacing NaN?
         spacings[i] = 1.0;
       }
       if ( AIR_EXISTS(nrrd->axis[i].min) ) { // is the min NaN?
         origins[i] = nrrd->axis[i].min;
       }
       else { // If min has not been set, assume a default.
         // An ITK image _must_ have a valid origin.
         origins[i] = 0;
       }
       
       // get IJK to RAS direction vector
       for (int j=0; j<nrrd->spaceDim; j++) {
         IjkToRasMatrix->SetElement(j, i, nrrd->axis[i].spaceDirection[j]);
       }
     }
     else { 
       // assume this is data dimension
       // combine with the last spacial dimension
       if (kind == nrrdKindSpace) {
         // assume this is really meant to be spatial -- even though it's not specified
         //vtkErrorMacro("Error reading " << this->GetFileName() << ": " << "dimension # " << i << " must be not spatial");
       }
       dataExtent[2*(i-1)+1] =  nrrd->axis[i-1].size * nrrd->axis[i].size - 1;
     }
   }

   vtkMatrix4x4::Invert(IjkToRasMatrix, RasToIjkMatrix);
   for (i=0; i < nrrd->dim; i++) {
       RasToIjkMatrix->SetElement(i, 3, (dataExtent[2*i+1] - dataExtent[2*i])/2.0);
   }
   RasToIjkMatrix->SetElement(3,3,1.0);
   IjkToRasMatrix->Delete();

   this->SetDataSpacing(spacings);
   this->SetDataOrigin(origins);
   this->SetDataExtent(dataExtent);

   // Push extra key/value pair data into an itkDataDictionary
   for (i=0; i < nrrdKeyValueSize(nrrd); i++) {
     nrrdKeyValueIndex(nrrd, &key, &val, i);
     HeaderKeyValue[std::string(key)] = std::string(val);
     free(key);  // key and val point to malloc'd data!!
     free(val);
     key = val = NULL;
   }

    this->vtkImageReader2::ExecuteInformation();
   
   nrrdNix(nrrd);
   nrrdIoStateNix(nio);
#endif
}


//----------------------------------------------------------------------------
// This function reads in one data of data.
// templated to handle different data types.
template <class T> 
void vtkNRRDReaderUpdate(vtkNRRDReader *self, vtkImageData *data, 
                            T *outPtr)
{
  int outIncr[3];
  int outExtent[6];

  data->GetExtent(outExtent);
  data->GetIncrements(outIncr);


#if 0
  outPtr2 = outPtr;
  int idx2;
  for (idx2 = outExtent[4]; idx2 <= outExtent[5]; ++idx2)
    {
    vtkNRRDReaderUpdate2(self, outPtr2, outExtent, outIncr);
    self->UpdateProgress((idx2 - outExtent[4])/
                         (outExtent[5] - outExtent[4] + 1.0));
    outPtr2 += outIncr[2];
    }
#endif
  
  Nrrd *nrrd;

  // Allocate a nrrd and a nrrd io state.
  nrrd = nrrdNew();

  // The data buffer has already been allocated.  Hand this off to the nrrd.
  nrrd->data = outPtr;

  // Now to prevent the Nrrd IO from trying to delete the buffer (illegally
  // with free) when it reads the file, set the size, type, and dimensionality.
  //
  // fixed at 3 dimensions for vtk - TODO: handle multiple volumes
  
  nrrd->dim = 3;
  int dataExtent[6];
  self->GetDataExtent(dataExtent);
  for (unsigned int i = 0; i < 3; i++)
    {
    nrrd->axis[i].size = dataExtent[2*i+1] + 1;
    }

  // Set data type information
  nrrd->type = self->VTKToNrrdPixelType( self->GetDataScalarType() );
  
  // Load using the nrrdLoad call.
  if ( nrrdLoad(nrrd, self->GetFileName(), NULL) != 0 )
    {
    char *filename = self->GetFileName();
    char *err =  biffGetDone("nrrd");
    vtkErrorWithObjectMacro(self, "Could not read " << filename <<  "\nThe error returned was " << err << "\n");
    free(err); // err points to malloc'd data!
    }

  // Free the nrrd struct but do not delete nrrd.data
  nrrdNix(nrrd);
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

  data->GetPointData()->GetScalars()->SetName("NRRDImage");

  this->ComputeDataIncrements();
  
  // Call the correct templated function for the output
  void *outPtr;
  outPtr = data->GetScalarPointer();

  switch(this->GetDataScalarType())
    {
    // This is simple a #define for a big case list. It handles
    // all data types vtk can handle.
    vtkTemplateMacro3(vtkNRRDReaderUpdate, this, data,
                      (VTK_TT *)(outPtr));
    default:
      vtkGenericWarningMacro("ExecuteData: Unknown input ScalarType");
      return;
    }
#endif
}

//----------------------------------------------------------------------------
void vtkNRRDReader::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

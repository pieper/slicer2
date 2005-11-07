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
#include "vtkObjectFactory.h"
#include "vtkImageData.h"


#include "vtkDataArray.h"
#include "vtkBitArray.h"
#include "vtkUnsignedCharArray.h"
#include "vtkCharArray.h"
#include "vtkUnsignedShortArray.h"
#include "vtkShortArray.h"
#include "vtkUnsignedIntArray.h"
#include "vtkIntArray.h"
#include "vtkUnsignedLongArray.h"
#include "vtkLongArray.h"
#include "vtkDoubleArray.h"
#include "vtkFloatArray.h" 


vtkCxxRevisionMacro(vtkNRRDReader, "$Revision: 1.20 $");
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
  //if ( fname == "-" )
  //  {
  //  return true;
  //  }

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


  char magic[5] = {'\0','\0','\0','\0','\0'};
  inputStream.read(magic,4*sizeof(char));

  if( inputStream.eof() )
    {
    inputStream.close();
    return false;
    }

  if( strcmp(magic,"NRRD")==0 ) 
    {
    inputStream.close();
    return true;
    }

  inputStream.close();
  return false;
}




void vtkNRRDReader::ExecuteInformation()
{
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


   nio = nrrdIoStateNew();
   // this is the mechanism by which we tell nrrdLoad to read
   // just the header, and none of the data
   nrrdIoStateSet(nio, nrrdIoStateSkipData, 1);
   
   if (nrrdLoad(this->nrrd, this->GetFileName(), nio) != 0) {
     err = biffGetDone(NRRD);
     vtkErrorMacro("Error reading " << this->GetFileName() << ": " << err);
     free(err); // err points to malloc'd data!!
     //     err = NULL;
     return;
   }


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




  if (nrrdTypeBlock == nrrd->type)
    {
    vtkErrorMacro("ReadImageInformation: Cannot currently "
                      "handle nrrdTypeBlock");
    return;
    }


   if ( nio->endian == airEndianLittle ) {
     this->SetDataByteOrderToLittleEndian();
   }
   else {
     this->SetDataByteOrderToBigEndian();
   }

  //if ( nio->encoding == nrrdEncodingAscii )
  //  {
  //  this->SetFileTypeToASCII();
  //  }
  //else
  //  {
  //  this->SetFileTypeToBinary();
  //  }
    
    
   // Set the number of image dimensions and bail if needed
   unsigned int domainAxisNum, domainAxisIdx[NRRD_DIM_MAX],
    rangeAxisNum, rangeAxisIdx[NRRD_DIM_MAX];
   domainAxisNum = nrrdDomainAxesGet(nrrd, domainAxisIdx);
   rangeAxisNum = nrrdRangeAxesGet(nrrd, rangeAxisIdx);
   if (nrrd->spaceDim && nrrd->spaceDim != domainAxisNum)
    {
    vtkErrorMacro("ReadImageInformation: nrrd's # independent axes ("
                      << domainAxisNum << ") doesn't match dimension of space"
                      " in which orientation is defined ("
                      << nrrd->spaceDim << "); not currently handled");
    return;              
    }    
    

   // vtkNRRDReader only supports 3 or 4 dimensional image with scalar, vector,
   // normal or tensor data. Other dimensionality is considered a multicomponent scalar field. 

   if ( 3 == this->nrrd->dim && 0 == rangeAxisNum) {
     // Assume that all dimensions are spacial and the last
     //dimension is some vtk object
     
      // we don't have any non-scalar data
      //this->SetNumberOfDimensions(nrrd->dim);
      this->SetPointDataType(SCALARS);
      this->SetNumberOfComponents(1);
      
      
   } else if ( 4 == this->nrrd->dim && 1 == rangeAxisNum) {  
   
      //this->SetNumberOfDimensions(nrrd->dim - 1);
      unsigned int kind = nrrd->axis[rangeAxisIdx[0]].kind;
      unsigned int size = nrrd->axis[rangeAxisIdx[0]].size;
      // NOTE: it is the NRRD readers responsibility to make sure that
      // the size (# of components) associated with a specific kind is
      // matches the actual size of the axis.
      switch(kind) {
       case nrrdKindDomain:
       case nrrdKindSpace:
       case nrrdKindTime:
         vtkErrorMacro("ReadImageInformation: range axis kind ("
                        << airEnumStr(nrrdKind, kind) << ") seems more "
                        "like a domain axis than a range axis");
         break;
       case nrrdKindStub:
       case nrrdKindScalar:
         //this->SetPixelType( ImageIOBase::SCALAR );
         this->SetPointDataType(SCALARS);
         this->SetNumberOfComponents(size);
         break;
       case nrrdKind3Color:
       case nrrdKindRGBColor:
         //this->SetPixelType( ImageIOBase::RGB );
        this->SetPointDataType(SCALARS);
        this->SetNumberOfComponents(size);
        break;
       case nrrdKind4Color:
       case nrrdKindRGBAColor:
        //this->SetPixelType( ImageIOBase::RGBA );
        this->SetPointDataType(SCALARS);
        this->SetNumberOfComponents(size);
        break;
       case nrrdKind3Vector:
         //this->SetPixelType( ImageIOBase::VECTOR );
         this->SetPointDataType(VECTORS);
         this->SetNumberOfComponents(size);
         break;
       case nrrdKindVector:
       case nrrdKind2Vector: 
       case nrrdKind4Vector:
       case nrrdKindList:
         this->SetPointDataType(SCALARS);
         this->SetNumberOfComponents(size);
     break;
       case nrrdKindPoint:
         //this->SetPixelType( ImageIOBase::POINT );
         this->SetPointDataType(SCALARS);
         this->SetNumberOfComponents(size);
         break;
       case nrrdKindCovariantVector:
       case nrrdKind3Gradient:
         this->SetPointDataType(VECTORS);
         this->SetNumberOfComponents(size);
         break;
       case nrrdKindNormal:
       case nrrdKind3Normal:
         this->SetPointDataType(NORMALS);
         //this->SetPixelType( ImageIOBase::COVARIANTVECTOR );
         this->SetNumberOfComponents(size);
         break;
       case nrrdKind3DSymMatrix:
         // ImageIOBase::DIFFUSIONTENSOR3D is a subclass
         //this->SetPixelType( ImageIOBase::SYMMETRICSECONDRANKTENSOR );
         this->SetPointDataType(SCALARS);
     this->SetNumberOfComponents(size);
         break;
       case nrrdKind3DMaskedSymMatrix:
         //this->SetPixelType( ImageIOBase::SYMMETRICSECONDRANKTENSOR );
         // NOTE: we will crop out the mask in Read() below; this is the
         // one case where NumberOfComponents != size
         this->SetPointDataType(SCALARS);
         this->SetNumberOfComponents(size-1);
         break;
       case nrrdKindComplex:
         this->SetPointDataType(SCALARS);
         //this->SetPixelType( ImageIOBase::COMPLEX );
          this->SetNumberOfComponents(size);
     break;
       case nrrdKindHSVColor:
       case nrrdKindXYZColor:
       case nrrdKindQuaternion:
       case nrrdKind2DSymMatrix:
       case nrrdKind2DMaskedSymMatrix:
       case nrrdKind2DMatrix:
       case nrrdKind2DMaskedMatrix:

        // for all other Nrrd kinds, we punt and call it a vector
        //this->SetPixelType( ImageIOBase::VECTOR );
        this->SetPointDataType(SCALARS);
        this->SetNumberOfComponents(size);
        break;
      
       case nrrdKind3DMatrix:  
         this->SetPointDataType(TENSORS);
         this->SetNumberOfComponents(size); 
         break;
       default:
         vtkErrorMacro("ReadImageInformation: nrrdKind " << kind 
                        << " not known!");
         return;
        }
     }      
     else {
        vtkErrorMacro("Error reading " << this->GetFileName() << ": " << "only 3D/4D volumes supported");
        return;
     }

   // Set type information
   //this->SetDataScalarType( this->NrrdToVTKScalarType(this->nrrd->type) );
   this->SetDataType(this->NrrdToVTKScalarType(this->nrrd->type) );
   
   // Set axis information
   int dataExtent[6];
   vtkFloatingPointType spacings[3];
   double spacing;
   vtkFloatingPointType origins[3];
   double axis[NRRD_SPACE_DIM_MAX];

   double spaceDir[NRRD_SPACE_DIM_MAX];
   int spacingStatus;
   for (unsigned int axii=0; axii < domainAxisNum; axii++)
    {
    unsigned int naxi = domainAxisIdx[axii];
    dataExtent[2*axii] = 0;
    dataExtent[2*axii+1] = this->nrrd->axis[naxi].size - 1;  
    
    spacingStatus = nrrdSpacingCalculate(nrrd, naxi, &spacing, spaceDir);
    
    switch(spacingStatus) 
      {
      case nrrdSpacingStatusNone:
        // Let ITK's defaults stay
        // this->SetSpacing(axii, 1.0);
    spacings[axii]=1.0;
        break;
      case nrrdSpacingStatusScalarNoSpace:
        spacings[axii]=spacing;
    IjkToRasMatrix->SetElement(axii,axii,spacing);
        break;
      case nrrdSpacingStatusDirection:
        if (AIR_EXISTS(spacing))
          {
          // only set info if we have something to set
          //this->SetSpacing(axii, spacing);
          spacings[axii]=spacing;

        
      for (int j=0; j<this->nrrd->spaceDim; j++) 
        {
             IjkToRasMatrix->SetElement(j,axii , spaceDir[j]*spacing);
            }  
          
          }
        break;
      default:
      case nrrdSpacingStatusUnknown:
        vtkErrorMacro("ReadImageInformation: Error interpreting "
                          "nrrd spacing (nrrdSpacingStatusUnknown)");
        break;
      case nrrdSpacingStatusScalarWithSpace:
        vtkErrorMacro("ReadImageInformation: Error interpreting "
                          "nrrd spacing (nrrdSpacingStatusScalarWithSpace)");
        break;
      }
    }

  // Figure out origin
  if (3 == nrrd->spaceDim)
    {
    if (AIR_EXISTS(nrrd->spaceOrigin[0]))
      {
      // only set info if we have something to set
      for (unsigned int saxi=0; saxi < nrrd->spaceDim; saxi++)
        {
    origins[saxi] = nrrd->spaceOrigin[saxi];
        //this->SetOrigin(saxi, nrrd->spaceOrigin[saxi]);
        }
      }
    }
   else 
    {
    double spaceOrigin[NRRD_DIM_MAX];
    int originStatus = nrrdOriginCalculate(nrrd, domainAxisIdx, domainAxisNum,
                                           nrrdCenterCell, spaceOrigin);
    for (unsigned int saxi=0; saxi < domainAxisNum; saxi++) 
      {
      switch (originStatus)
        {
        case nrrdOriginStatusNoMin:
        case nrrdOriginStatusNoMaxOrSpacing:
          // only set info if we have something to set
          // this->SetOrigin(saxi, 0.0);
          origins[saxi] = 0.0;
      break;
        case nrrdOriginStatusOkay:
      origins[saxi] = spaceOrigin[saxi];
          break;
        default:
        case nrrdOriginStatusUnknown:
        case nrrdOriginStatusDirection:
          vtkErrorMacro("ReadImageInformation: Error interpreting "
                            "nrrd origin status");
          break;
        }
      }
    }
      

   vtkMatrix4x4::Invert(IjkToRasMatrix, RasToIjkMatrix);
   for (int i=0; i < this->nrrd->dim; i++) {
       RasToIjkMatrix->SetElement(i, 3, (dataExtent[2*i+1] - dataExtent[2*i])/2.0);
   }
  
   RasToIjkMatrix->SetElement(3,3,1.0);
   IjkToRasMatrix->Delete();

   this->SetDataSpacing(spacings);
   //this->SetDataOrigin(origins);
   this->SetDataExtent(dataExtent);

   // Push extra key/value pair data into an itkDataDictionary
   for (i=0; i < nrrdKeyValueSize(this->nrrd); i++) {
     nrrdKeyValueIndex(this->nrrd, &key, &val, i);
     HeaderKeyValue[std::string(key)] = std::string(val);
     free(key);  // key and val point to malloc'd data!!
     free(val);
     key = val = NULL;
   }
   
   if (nrrd->space) 
     {
      HeaderKeyValue[std::string("space")] = std::string( airEnumStr(nrrdSpace, nrrd->space) );
     }
     

   if (AIR_EXISTS(this->nrrd->measurementFrame[0][0])) 
   {
     for (int j=0;j<2;j++)
       for (int i=0;i<2;i++)
         MeasurementFrameMatrix->SetElement(i,j,this->nrrd->measurementFrame[i][j]);
   }
   
   this->vtkImageReader2::ExecuteInformation();
   
   //this->nrrd = nrrdNix(this->nrrd);
   nio = nrrdIoStateNix(nio);
}


vtkImageData *vtkNRRDReader::AllocateOutputData(vtkDataObject *out) {

 vtkImageData *res = vtkImageData::SafeDownCast(out);
  if (!res)
    {
    vtkWarningMacro("Call to AllocateOutputData with non vtkImageData output");
    return NULL;
    }

  // I would like to eliminate this method which requires extra "information"
  // That is not computed in the graphics pipeline.
  // Until I can eliminate the method, I will reexecute the ExecuteInformation
  // before the execute.
  this->ExecuteInformation();

  res->SetExtent(res->GetUpdateExtent());
  this->AllocatePointData(res);

  return res;

}

void vtkNRRDReader::AllocatePointData(vtkImageData *out) {

 vtkDataArray *pd = NULL;
 int Extent[6]; 
 out->GetExtent(Extent);

  // if the scalar type has not been set then we have a problem
  if (this->DataType == VTK_VOID)
    {
    vtkErrorMacro("Attempt to allocate scalars before scalar type was set!.");
    return;
    }

  // if we currently have scalars then just adjust the size
  switch (this->PointDataType) {
    case SCALARS:
       pd = out->GetPointData()->GetScalars();
       break;
    case VECTORS:
       pd = out->GetPointData()->GetVectors();
       break;
    case NORMALS:
       pd = out->GetPointData()->GetNormals();
       break;
    case TENSORS:
       pd = out->GetPointData()->GetTensors();
       break;
    default:
       vtkErrorMacro("Unknown PointData Type.");
       return;
   }             

  if (pd && pd->GetDataType() == this->DataType
      && pd->GetReferenceCount() == 1) 
    {
    pd->SetNumberOfComponents(this->GetNumberOfComponents());
    pd->SetNumberOfTuples((Extent[1] - Extent[0] + 1)*
                               (Extent[3] - Extent[2] + 1)*
                               (Extent[5] - Extent[4] + 1));
    // Since the execute method will be modifying the scalars
    // directly.
    pd->Modified();
    return;
    }
  
  // allocate the new scalars
  switch (this->DataType)
    {
    case VTK_BIT:
      pd = vtkBitArray::New();
      break;
    case VTK_UNSIGNED_CHAR:
      pd = vtkUnsignedCharArray::New();
      break;
    case VTK_CHAR:           
      pd = vtkCharArray::New();
      break;
    case VTK_UNSIGNED_SHORT: 
      pd = vtkUnsignedShortArray::New();
      break;
    case VTK_SHORT:          
      pd = vtkShortArray::New();
      break;
    case VTK_UNSIGNED_INT:   
      pd = vtkUnsignedIntArray::New();
      break;
    case VTK_INT:            
      pd = vtkIntArray::New();
      break;
    case VTK_UNSIGNED_LONG:  
      pd = vtkUnsignedLongArray::New();
      break;
    case VTK_LONG:           
      pd = vtkLongArray::New();
      break;
    case VTK_DOUBLE:          
      pd = vtkDoubleArray::New();
      break;
    case VTK_FLOAT:         
      pd = vtkFloatArray::New();
      break;
    default:
      vtkErrorMacro("Could not allocate data type.");
      return;
    }
  out->SetScalarType(this->DataType);
  pd->SetNumberOfComponents(this->GetNumberOfComponents());

  // allocate enough memory
  pd->SetNumberOfTuples((Extent[1] - Extent[0] + 1)*
                      (Extent[3] - Extent[2] + 1)*
                      (Extent[5] - Extent[4] + 1));

    switch (this->PointDataType) {
    case SCALARS:
       out->GetPointData()->SetScalars(pd);
       out->SetNumberOfScalarComponents(this->GetNumberOfComponents());
       break;
    case VECTORS:
       out->GetPointData()->SetVectors(pd);
       break;
    case NORMALS:
       out->GetPointData()->SetNormals(pd);
       break;
    case TENSORS:
       out->GetPointData()->SetTensors(pd);
       break;
    default:
       vtkErrorMacro("Unknown PointData Type.");
       return;
     }             
  pd->Delete();
}



//----------------------------------------------------------------------------
// This function reads a data from a file.  The datas extent/axes
// are assumed to be the same as the file extent/order.
void vtkNRRDReader::ExecuteData(vtkDataObject *output)
{

  output->SetUpdateExtentToWholeExtent();
  vtkImageData *data = this->AllocateOutputData(output);

  if (this->GetFileName() == NULL)
    {
    vtkErrorMacro(<< "Either a FileName or FilePrefix must be specified.");
    return;
    }


  // Read in the nrrd.  Yes, this means that the header is being read
  // twice: once by ExecuteInformation, and once here
  if ( nrrdLoad(this->nrrd, this->GetFileName(), NULL) != 0 )
    {
    char *err =  biffGetDone(NRRD); // would be nice to free(err)
    vtkErrorMacro("Read: Error reading " 
                      << this->GetFileName() << ":\n" << err);
    }


  if (this->nrrd->data == NULL)
    {
    vtkErrorMacro(<< "data is null.");
    return;
    }

  switch(PointDataType) {
    case SCALARS:
      data->GetPointData()->GetScalars()->SetName("NRRDImage");
      //get pointer
      break;
    case VECTORS:
      data->GetPointData()->GetVectors()->SetName("NRRDImage");
      //get pointer
      break;
    case NORMALS:
      data->GetPointData()->GetNormals()->SetName("NRRDImage");
      break;
    case TENSORS:
      data->GetPointData()->GetTensors()->SetName("NRRDImage");
      break;    
   }     
  this->ComputeDataIncrements();

  int dims[3];
  data->GetDimensions(dims);


  //Do the reading
   
  unsigned int rangeAxisNum, rangeAxisIdx[NRRD_DIM_MAX];
  rangeAxisNum = nrrdRangeAxesGet(nrrd, rangeAxisIdx);

  if ( rangeAxisNum > 1)
    {
    vtkErrorMacro("Read: handling more than one non-scalar axis "
                      "not currently handled");
    }
  if (1 == rangeAxisNum && 0 != rangeAxisIdx[0])
    {
    // the range (dependent variable) is not on the fastest axis,
    // so we have to permute axes to put it there, since that is
    // how we set things up in ReadImageInformation() above
    Nrrd *ntmp = nrrdNew();
    unsigned int axmap[NRRD_DIM_MAX];
    axmap[0] = rangeAxisIdx[0];
    for (unsigned int axi=1; axi<nrrd->dim; axi++)
      {
      axmap[axi] = axi - (axi <= rangeAxisIdx[0]);
      }
    // The memory size of the input and output of nrrdAxesPermute is
    // the same; the existing nrrd->data is re-used.
    
    
    if (nrrdCopy(ntmp, nrrd)
        || nrrdAxesPermute(nrrd, ntmp, axmap))
      {
      char *err =  biffGetDone(NRRD); // would be nice to free(err)
      vtkErrorMacro("Read: Error permuting independent axis in " 
                        << this->GetFileName() << ":\n" << err);
      }
      
    nrrdNuke(ntmp);
    }


     void *ptr = data->GetScalarPointer();
    // Now we have to get the data back into the given VTK data pointer
    // In any case, the logic here has the luxury of assuming that the
    // *single* non-scalar axis is the *first* (fastest) axis.
    if (nrrdKind3DMaskedSymMatrix == nrrd->axis[0].kind)
      {
      // we crop out the mask and put the output in ITK-allocated "buffer"
      size_t size[NRRD_DIM_MAX], minIdx[NRRD_DIM_MAX], maxIdx[NRRD_DIM_MAX];
      for (unsigned int axi=0; axi<nrrd->dim; axi++)
        {
        minIdx[axi] = (0 == axi) ? 1 : 0;
        maxIdx[axi] = nrrd->axis[axi].size-1;
        size[axi] = maxIdx[axi] - minIdx[axi] + 1;
        }
      Nrrd *ntmp = nrrdNew();
      if (nrrdCopy(ntmp, nrrd)
          || (nrrdEmpty(nrrd), 
              nrrdWrap_nva(nrrd, ptr, ntmp->type, ntmp->dim, size))
          || nrrdCrop(nrrd, ntmp, minIdx, maxIdx))
        {
        char *err =  biffGetDone(NRRD); // would be nice to free(err)
        vtkErrorMacro("Read: Error copying, crapping or cropping:\n"
                          << err);
        }
      nrrdNuke(ntmp);
      //nrrdNix(nrrd);
      }
   
   
     Nrrd *nflip = nrrdNew(); 
     nrrdCopy(nflip, this->nrrd);
     unsigned int domainAxisNum, domainAxisIdx[NRRD_DIM_MAX];
     domainAxisNum = nrrdDomainAxesGet(nrrd, domainAxisIdx);
     this->nrrdFlip(this->nrrd, nflip, domainAxisIdx[1]);
     nrrdNuke(nflip);
    
     
     //size_t datasize = data->GetScalarSize() * 
     //               data->GetNumberOfScalarComponents() *
     //               dims[0] * dims[1] * dims[2];
     cout<<"Element Size: "<<nrrdElementSize(nrrd)<<"Element number: "<<nrrdElementNumber(nrrd)<<endl;
     memcpy(ptr, this->nrrd->data,
             nrrdElementSize(nrrd)*nrrdElementNumber(nrrd));
      cout<<"Mem copy done"<<endl;
      
     //I don't know about this.
     //nrrdNuke(nrrd);
     
  
}

//----------------------------------------------------------------------------
void vtkNRRDReader::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}


/* NOTE: This a authorized copy of nrrdFlip from Teem.
/*Gordon Kindlmann holds the Copyright for this code.

/*
******** nrrdFlip()
**
** reverse the order of slices along the given axis.
** Actually, just a wrapper around nrrdShuffle() (with some
** extra setting of axis info)
*/
int
vtkNRRDReader::nrrdFlip(Nrrd *nout, const Nrrd *nin, unsigned int axis) {
  char me[]="nrrdFlip", func[]="flip", err[AIR_STRLEN_MED];
  size_t *perm, si;
  airArray *mop;
  unsigned int axisIdx;

  mop = airMopNew();
  if (!(nout && nin)) {
    sprintf(err, "%s: got NULL pointer", me);
    biffAdd(NRRD, err); airMopError(mop); return 1;
  }
  if (!(  axis < nin->dim )) {
    sprintf(err, "%s: given axis (%d) is outside valid range ([0,%d])", 
            me, axis, nin->dim-1);
    biffAdd(NRRD, err); airMopError(mop); return 1;
  }
  if (!(perm = (size_t*)calloc(nin->axis[axis].size, sizeof(size_t)))) {
    sprintf(err, "%s: couldn't alloc permutation array", me);
    biffAdd(NRRD, err); airMopError(mop); return 1;
  }
  airMopAdd(mop, perm, airFree, airMopAlways);
  for (si=0; si<nin->axis[axis].size; si++) {
    perm[si] = nin->axis[axis].size-1-si;
  }
  /* nrrdBasicInfoCopy called by nrrdShuffle() */
  if (nrrdShuffle(nout, nin, axis, perm)
      || nrrdContentSet(nout, func, nin, "%d", axis)) {
    sprintf(err, "%s:", me);
    biffAdd(NRRD, err); airMopError(mop); return 1;
  }
  this->_nrrdAxisInfoCopy(&(nout->axis[axis]), &(nin->axis[axis]),
                    NRRD_AXIS_INFO_SIZE_BIT
                    | NRRD_AXIS_INFO_KIND_BIT);
  /* HEY: (Tue Jan 18 00:28:26 EST 2005) there's a basic question to
     be answered here: do we want to keep the "location" of the
     samples fixed, while changing their ordering, or do want to flip
     the location of the samples?  In the former, the position
     information has to be flipped to cancel the flipping of the the
     sample order, so that samples maintain location.  In the latter,
     the position information is copied verbatim from the original.  */
  /* (Tue Sep 13 09:59:12 EDT 2005) answer: we keep the "location" of
     the samples fixed, while changing their ordering.  This is the 
     low-level thing to do, so for a nrrd function, its the right thing
     to do.  You don't need a nrrd function to simply manipulate 
     per-axis meta-information */
  nout->axis[axis].min = nin->axis[axis].max;
  nout->axis[axis].max = nin->axis[axis].min;
  /* HEY: Fri Jan 14 02:53:30 EST 2005: isn't spacing supposed to be
     the step from one sample to the next?  So its a signed quantity.
     If min and max can be flipped (so min > max), then spacing can
     be negative, right?  */
  nout->axis[axis].spacing = -nin->axis[axis].spacing;
  /* HEY: Fri Jan 14 02:53:30 EST 2005: but not thickness */
  nout->axis[axis].thickness = nin->axis[axis].thickness;
  /* need to set general orientation info too */
  for (axisIdx=0; axisIdx<NRRD_SPACE_DIM_MAX; axisIdx++) {
    nout->axis[axis].spaceDirection[axisIdx] = 
      -nin->axis[axis].spaceDirection[axisIdx];
  }
  /* modify origin only if we flipped a spatial axis */
  if (AIR_EXISTS(nin->axis[axis].spaceDirection[0])) {
    this->_nrrdSpaceVecScaleAdd2(nout->spaceOrigin,
                           1.0,
                           nin->spaceOrigin,
                           nin->axis[axis].size-1,
                           nin->axis[axis].spaceDirection);
  } else {
    this->_nrrdSpaceVecCopy(nout->spaceOrigin, nin->spaceOrigin);
  }
  airMopOkay(mop); 
  return 0;
}


/*
** _nrrdAxisInfoCopy
**
** HEY: we have a void return even though this function potentially
** involves calling airStrdup!!  
*/
void
vtkNRRDReader::_nrrdAxisInfoCopy(NrrdAxisInfo *dest, const NrrdAxisInfo *src, int bitflag) {
  int ii;

  if (!(NRRD_AXIS_INFO_SIZE_BIT & bitflag)) {
    dest->size = src->size;
  }
  if (!(NRRD_AXIS_INFO_SPACING_BIT & bitflag)) {
    dest->spacing = src->spacing;
  }
  if (!(NRRD_AXIS_INFO_THICKNESS_BIT & bitflag)) {
    dest->thickness = src->thickness;
  }
  if (!(NRRD_AXIS_INFO_MIN_BIT & bitflag)) {
    dest->min = src->min;
  }
  if (!(NRRD_AXIS_INFO_MAX_BIT & bitflag)) {
    dest->max = src->max;
  }
  if (!(NRRD_AXIS_INFO_SPACEDIRECTION_BIT & bitflag)) {
    for (ii=0; ii<NRRD_SPACE_DIM_MAX; ii++) {
      dest->spaceDirection[ii] = src->spaceDirection[ii];
    }
  }
  if (!(NRRD_AXIS_INFO_CENTER_BIT & bitflag)) {
    dest->center = src->center;
  }
  if (!(NRRD_AXIS_INFO_KIND_BIT & bitflag)) {
    dest->kind = src->kind;
  }
  if (!(NRRD_AXIS_INFO_LABEL_BIT & bitflag)) {
    if (dest->label != src->label) {
      dest->label = (char *)airFree(dest->label);
      dest->label = (char *)airStrdup(src->label);
    }
  }
  if (!(NRRD_AXIS_INFO_UNITS_BIT & bitflag)) {
    if (dest->units != src->units) {
      dest->units = (char *)airFree(dest->units);
      dest->units = (char *)airStrdup(src->units);
    }
  }

  return;
}

void
vtkNRRDReader::_nrrdSpaceVecScaleAdd2(double sum[NRRD_SPACE_DIM_MAX], 
                       double sclA, const double vecA[NRRD_SPACE_DIM_MAX],
                       double sclB, const double vecB[NRRD_SPACE_DIM_MAX]) {
  int ii;
  
  for (ii=0; ii<NRRD_SPACE_DIM_MAX; ii++) {
    sum[ii] = sclA*vecA[ii] + sclB*vecB[ii];
  }
}

void
vtkNRRDReader::_nrrdSpaceVecCopy(double dst[NRRD_SPACE_DIM_MAX], 
                  const double src[NRRD_SPACE_DIM_MAX]) {
  int ii;

  for (ii=0; ii<NRRD_SPACE_DIM_MAX; ii++) {
    dst[ii] = src[ii];
  }
}


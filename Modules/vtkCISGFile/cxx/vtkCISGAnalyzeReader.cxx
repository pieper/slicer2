/*=========================================================================
 
  Library   : vtk/objects/library/CISGImage
  Module    : $RCSfile: vtkCISGAnalyzeReader.cxx,v $
  Authors   : Thomas Hartkens <thomas@hartkens.de>
              Daniel Rueckert <d.rueckert@doc.ic.ac.uk>
          Julia Schnabel  <julia.schnabel@kcl.ac.uk>
          under the supervision of Dave Hawkes and Derek Hill
  Web       : http://www.image-registration.com
  Copyright : King's College London
              Div. of Radiological Sciences, 
              Computational Imaging Science Group, 1997 - 2000
          http://www-ipg.umds.ac.uk/cisg
  Date      : $Date: 2003/08/14 17:32:33 $
  Version   : $Revision: 1.1 $

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
=========================================================================*/
#include "vtkObjectFactory.h"
#include "vtkShortArray.h"
#include "vtkCharArray.h"
#include "vtkIntArray.h"
#include "vtkUnsignedShortArray.h"
#include "vtkUnsignedCharArray.h"
#include "vtkUnsignedIntArray.h"
#include "vtkFloatArray.h"
#include <string.h>
#include "vtkCISGAnalyzeReader.h"

#include "vtkCISGCifstream.h"
#include "vtkAnalyze.h"


#define MAXEXTENTION 10



vtkCISGAnalyzeReader* vtkCISGAnalyzeReader::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkCISGAnalyzeReader")
;
  if(ret)
    {
    return (vtkCISGAnalyzeReader*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkCISGAnalyzeReader;
}



// Construct object with NULL file prefix; file pattern "%s.%d"; image range 
// set to (1,1); data origin (0,0,0); data spacing (1,1,1); no data mask;
// header size 0; and byte swapping turned off.
vtkCISGAnalyzeReader::vtkCISGAnalyzeReader()
{
  this->FileName = NULL;
  this->FlippingSequence = NULL;
}

vtkCISGAnalyzeReader::~vtkCISGAnalyzeReader()
{
  if (this->FileName) delete [] this->FileName;
  if (this->FlippingSequence) delete [] this->FlippingSequence;
}


int vtkCISGAnalyzeReader::CheckHeader(char *filename)
{
  int i= strlen(filename)-1;
  int j=0;
  char extention[MAXEXTENTION];
  if ((filename[i-1]=='.')  && (filename[i]=='Z')) {
    i=i-2;
  }
  while ((i>=0) && (filename[i]!='.') 
     && (filename[i]!='/') && (filename[i]!='\\') 
     && (j<MAXEXTENTION)) {
    extention[j]=filename[i];
    i--; j++;
  }
  return !strncmp(extention,"RDH",3) || !strncmp(extention,"rdh",3) ;
}





void vtkCISGAnalyzeReader::Execute()
{
  int length;
  char imagename[255];
  this->from = new vtkCISGCifstream;

  vtkStructuredPoints *output=(vtkStructuredPoints *)this->GetOutput();

  // Validate instance variables
  if (this->FileName == NULL) {
    vtkErrorMacro(<< "Filename is NULL");
    return;
  }

  // Read header
  this->from->Open(FileName);
  this->ReadHeader();
  this->from->Close();

  // Parse directory name
  length = strlen(this->FileName)-1;
  sprintf(imagename, "%s", this->FileName);
  if ((imagename[length-1]=='.')  && (imagename[length]=='Z')) {
    length=length-2;
  }
  imagename[length] = 'g';
  imagename[length-1] = 'm';
  imagename[length-2] = 'i';


  // Read data
  this->from->Open(imagename);
  vtkDataArray *newScalars = this->ReadVolume();
  this->from->Close();
  
  // calculate dimensions of output from data dimensions
  output->SetDimensions(this->Dim);
  output->SetSpacing(this->Size);
  output->SetOrigin(this->Origin);
  output->SetWholeExtent(output->GetExtent());
  output->SetUpdateExtent(output->GetWholeExtent());

  if (newScalars){
    output->GetPointData()->SetScalars(newScalars);
  }

}




void vtkCISGAnalyzeReader::ReadHeader()
{
  int   sizeof_hdr;
  char  padding1[28];
  int   extents;
  short padding2;
  char  regular;   
  char  padding3;
  short dims[4];
  short padding4[11];
  short data_type;
  short bits;    
  short padding5;    
  float pixdims[4];
  float padding6[12];
  int   glmax;   
  int   glmin;   
  char  padding7[168];
  int   padding8[8];

  this->from->CReadAsShort(&data_type, 1, 70);
  if ( (data_type != ANALYZE_U_CHAR) && 
       (data_type != ANALYZE_SHORT) && 
       (data_type != ANALYZE_INT) && 
       (data_type != ANALYZE_FLOAT) ) {
    this->NeedSwapping=1;
    this->from->FlipSwapping();
  } else {
    this->NeedSwapping=0;
  }

  this->from->CReadAsInt(&sizeof_hdr, 1, 0);
  this->from->CReadAsChar(padding1, 28, 4);
  this->from->CReadAsInt(&extents, 1, 32);
  this->from->CReadAsShort(&padding2, 1, 36);
  this->from->CReadAsChar(&regular, 1, 38);
  this->from->CReadAsChar(&padding3, 1, 39);
  this->from->CReadAsShort(dims, 4, 40);
  this->from->CReadAsShort(padding4, 11, 48);
  this->from->CReadAsShort(&data_type, 1, 70);
  this->from->CReadAsShort(&bits, 1, 72);
  this->from->CReadAsShort(&padding5, 1, 74);
  this->from->CReadAsFloat(pixdims, 4, 76);
  this->from->CReadAsFloat(padding6, 12, 92);
  this->from->CReadAsInt(&glmax, 1, 140);
  this->from->CReadAsInt(&glmin, 1, 144);
  this->from->CReadAsChar(padding7, 168, 148);
  this->from->CReadAsInt(padding8, 8, 316);

  this->Dim[0] = dims[1];
  this->Dim[1]  = dims[2];
  this->Dim[2]  = dims[3];

  // Read voxel dimensions
  this->Size[0] = pixdims[1];
  this->Size[1] = pixdims[2];
  this->Size[2] = pixdims[3];

  Origin[0]    = (-Size[0]*(Dim[0]-1)/2.0);
  Origin[1]    = (-Size[1]*(Dim[1]-1)/2.0);
  Origin[2]    = (-Size[2]*(Dim[2]-1)/2.0);

  switch(data_type){
  case ANALYZE_U_CHAR:
    this->Type  = VTK_UNSIGNED_CHAR;
    this->Bytes = 1;
    break;
  case ANALYZE_SHORT:
    this->Type  = VTK_SHORT;
    this->Bytes = 2;
    break;
  case ANALYZE_INT:
    this->Type  = VTK_INT;
    this->Bytes = 4;
    break;
  case ANALYZE_FLOAT:
    this->Type  = VTK_FLOAT;
    this->Bytes = 4;
    break;
  default:
    vtkErrorMacro(<< "Read_Header(): Unknown voxel type: " << data_type );
    exit(1);
  }

}




// Read a volume of data.
vtkDataArray *vtkCISGAnalyzeReader::ReadVolume()
{
  // calculate the number of voxels
  int nvoxels = this->Dim[0] * this->Dim[1] * this->Dim[2];

  // create the short scalars for data
  vtkDataArray *Scalars = vtkDataArray::CreateDataArray(this->Type);
  Scalars->Allocate(1,nvoxels);

  if (this->NeedSwapping) {
    this->from->FlipSwapping();
  }


  vtkDataArray *DataArray=Scalars;
  char *p = this->FlippingSequence;
  switch(this->Type){
  case VTK_CHAR: 
    {
      char *data=
      ((vtkCharArray *)DataArray)->WritePointer(0,nvoxels);
      this->from->CReadAsChar(data, nvoxels, 0);
      while (p && *p) // apply sequence of flip operations specified by string
      {        InternalFlip( *p++ - '0', data, this->Dim );
      }
    }
  break;
  case VTK_UNSIGNED_CHAR:
    {
      unsigned char *data=
      ((vtkUnsignedCharArray *)DataArray)->WritePointer(0,nvoxels);
      this->from->CReadAsUChar(data, nvoxels, 0);
      while (p && *p) // apply sequence of flip operations specified by string
      {        InternalFlip( *p++ - '0', data, this->Dim );
      }
    }
  break;
  case VTK_SHORT:
    {
      short *data =
      ((vtkShortArray *) DataArray)->WritePointer(0,nvoxels);
      this->from->CReadAsShort(data, nvoxels, 0);
      while (p && *p) // apply sequence of flip operations specified by string
      {        InternalFlip( *p++ - '0', data, this->Dim );
      }
    }
  break;
  case VTK_UNSIGNED_SHORT:
    {
      unsigned short *data=
      ((vtkUnsignedShortArray *)DataArray)->WritePointer(0,nvoxels);
      this->from->CReadAsUShort(data, nvoxels, 0);
      while (p && *p) // apply sequence of flip operations specified by string
      {        InternalFlip( *p++ - '0', data, this->Dim );
      }
    }
  break;
  case VTK_INT:
    {
      int *data=
      ((vtkIntArray *)DataArray)->WritePointer(0,nvoxels);
      this->from->CReadAsInt(data, nvoxels, 0);
      while (p && *p) // apply sequence of flip operations specified by string
      {        InternalFlip( *p++ - '0', data, this->Dim );
      }
    }
  break;
  case VTK_UNSIGNED_INT:
    {
      unsigned int *data=
      ((vtkUnsignedIntArray *)DataArray)->WritePointer(0,nvoxels);
      this->from->CReadAsUInt(data, nvoxels, 0);
      while (p && *p) // apply sequence of flip operations specified by string
      {        InternalFlip( *p++ - '0', data, this->Dim );
      }
    }
    break;
  case VTK_FLOAT:
    {
      float *data=
      ((vtkFloatArray *)DataArray)->WritePointer(0,nvoxels);
      this->from->CReadAsFloat(data, nvoxels, 0);
      while (p && *p) // apply sequence of flip operations specified by string
      {        InternalFlip( *p++ - '0', data, this->Dim );
      }
    }
    break;
  default:
    vtkErrorMacro(<< "ReadVolume(): Unknown voxel type");
    exit(1);
  }

  return Scalars;
}




void vtkCISGAnalyzeReader::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkCISGMultiReader::PrintSelf(os,indent);
}



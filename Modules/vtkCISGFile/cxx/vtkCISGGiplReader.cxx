/*=========================================================================
 
  Library   : vtk/objects/library/CISGImage
  Module    : $RCSfile: vtkCISGGiplReader.cxx,v $
  Authors   : Thomas Hartkens <thomas@hartkens.de>
              Daniel Rueckert <d.rueckert@doc.ic.ac.uk>
              Julia Schnabel  <julia.schnabel@kcl.ac.uk>
              under the supervision of Dave Hawkes and Derek Hill
  Web       : http://www.image-registration.com
  Copyright : King's College London
              Div. of Radiological Sciences, 
              Computational Imaging Science Group, 1997 - 2000
              http://www-ipg.umds.ac.uk/cisg
  Date      : $Date: 2003/08/14 17:32:35 $
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
#include "vtkCISGGiplReader.h"

#include "vtkCISGCifstream.h"
#include "vtkGipl.h"



vtkCISGGiplReader* vtkCISGGiplReader::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkCISGGiplReader")
;
  if(ret)
    {
    return (vtkCISGGiplReader*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkCISGGiplReader;
}



// Construct object with NULL file prefix; file pattern "%s.%d"; image range 
// set to (1,1); data origin (0,0,0); data spacing (1,1,1); no data mask;
// header size 0; and byte swapping turned off.
vtkCISGGiplReader::vtkCISGGiplReader()
{
  this->FileName = NULL;
}

vtkCISGGiplReader::~vtkCISGGiplReader()
{
  if (this->FileName) delete [] this->FileName;
}


int vtkCISGGiplReader::CheckHeader(char *filename)
{
  unsigned int _magic_number;

  // Create file stream
  vtkCISGCifstream from;

  // Open new file for reading
  from.Open(filename);

  // Read header
  from.CReadAsUInt(&_magic_number, 1, 252);

  // Close file
  from.Close();

  // Check header
  if (_magic_number != GIPL_MAGIC){
    return False;
  } else {
    return True;
  }
}





void vtkCISGGiplReader::Execute()
{
  // Create file stream
  this->from = new vtkCISGCifstream;

  vtkStructuredPoints *output=(vtkStructuredPoints *)this->GetOutput();

  // Validate instance variables
  if (this->FileName == NULL) {
    vtkErrorMacro(<< "Filename is NULL");
    return;
  }

  this->from->Open(FileName);

  // Read header
  this->ReadHeader();

  // Read data
  vtkDataArray *newScalars = this->ReadVolume();

  this->from->Close();
  
  // calculate dimensions of output from data dimensions
  output->SetDimensions(this->Dim);
  output->SetSpacing(this->Size);
  output->SetOrigin(this->Origin);
  output->SetWholeExtent(output->GetExtent());
  
  // output->ModifyExtentForUpdateExtent();
  output->SetUpdateExtent(output->GetWholeExtent());
  output->SetNumberOfScalarComponents(this->NumberOfComponents);

  if (newScalars){
    output->GetPointData()->SetScalars(newScalars);
    //    newScalars->Delete();
  }

}




void vtkCISGGiplReader::ReadHeader()
{
  //  int i;
  float size;
  unsigned short dim, type;
  unsigned int _magic_number;

  // Read header
  this->from->CReadAsUInt(&_magic_number, 1, 252);

  // Check header
  if (_magic_number != GIPL_MAGIC){
    vtkErrorMacro(<< "Read_Header: Can't read magic number");
    exit(1);
  }

  // Read image dimensions
  this->from->CReadAsUShort(&dim, 1, 0);
  this->Dim[0] = dim;
  this->from->CReadAsUShort(&dim, 1, 2);
  this->Dim[1]  = dim;
  this->from->CReadAsUShort(&dim, 1, 4);
  this->Dim[2]  = dim;

  // Read voxel dimensions
  this->from->CReadAsFloat(&size, 1, 10);
  this->Size[0] = size;
  this->from->CReadAsFloat(&size, 1, 14);
  this->Size[1] = size;
  this->from->CReadAsFloat(&size, 1, 18);
  this->Size[2] = size;

  Origin[0]    = (-Size[0]*(Dim[0]-1)/2.0);
  Origin[1]    = (-Size[1]*(Dim[1]-1)/2.0);
  Origin[2]    = (-Size[2]*(Dim[2]-1)/2.0);

  // Read type of voxels
  this->from->CReadAsUShort(&type, 1, 8);

  switch(type){
  case GIPL_CHAR:
    this->Type  = VTK_CHAR;
    this->Bytes = 1;
    break;
  case GIPL_U_CHAR:
    this->Type  = VTK_UNSIGNED_CHAR;
    this->Bytes = 1;
    break;
  case GIPL_SHORT:
    this->Type  = VTK_SHORT;
    this->Bytes = 2;
    break;
  case GIPL_U_SHORT:
    this->Type  = VTK_UNSIGNED_SHORT;
    this->Bytes = 2;
    break;
  case GIPL_INT:
    this->Type  = VTK_INT;
    this->Bytes = 4;
    break;
  case GIPL_U_INT:
    this->Type  = VTK_UNSIGNED_INT;
    this->Bytes = 4;
    break;
  case GIPL_FLOAT:
    this->Type  = VTK_FLOAT;
    this->Bytes = 4;
    break;
  default:
    vtkErrorMacro(<< "Read_Header(): Unknown voxel type");
    exit(1);
  }

  // Write No of components
  short dummy;
  this->NumberOfComponents=1;
  this->from->CReadAsShort(&dummy,1, 248);
  if (dummy == 170) {
    this->from->CReadAsShort(&this->NumberOfComponents, 1, 250);
  }

}




// Read a volume of data.
vtkDataArray *vtkCISGGiplReader::ReadVolume()
{
  // calculate the number of voxels
  int nvoxels = this->Dim[0] * this->Dim[1] * this->Dim[2] *
    this->NumberOfComponents;
  
  // create the short scalars for data
  vtkDataArray *Scalars = vtkDataArray::CreateDataArray(this->Type);
  Scalars->Allocate(1,nvoxels);
  Scalars->SetNumberOfComponents(this->NumberOfComponents);

  vtkDataArray *DataArray=Scalars;
  switch(this->Type){
  case VTK_CHAR: 
    {
    char *data=
      ((vtkCharArray *)DataArray)->WritePointer(0,nvoxels);
    this->from->CReadAsChar(data, nvoxels, GIPL_HEADERSIZE);
    }
  break;
  case VTK_UNSIGNED_CHAR:
    {
      unsigned char *data=
        ((vtkUnsignedCharArray *)DataArray)->WritePointer(0,nvoxels);
      this->from->CReadAsUChar(data, nvoxels, GIPL_HEADERSIZE);
    }
  break;
  case VTK_SHORT:
    {
      short *data =
        ((vtkShortArray *) DataArray)->WritePointer(0,nvoxels);
      this->from->CReadAsShort(data, nvoxels, GIPL_HEADERSIZE);
    }
  break;
  case VTK_UNSIGNED_SHORT:
    {
      unsigned short *data=
        ((vtkUnsignedShortArray *)DataArray)->WritePointer(0,nvoxels);
      this->from->CReadAsUShort(data, nvoxels, GIPL_HEADERSIZE);
    }
  break;
  case VTK_INT:
    {
      int *data=
      ((vtkIntArray *)DataArray)->WritePointer(0,nvoxels);
      this->from->CReadAsInt(data, nvoxels, GIPL_HEADERSIZE);
    }
  break;
  case VTK_UNSIGNED_INT:
    {
      unsigned int *data=
        ((vtkUnsignedIntArray *)DataArray)->WritePointer(0,nvoxels);
      this->from->CReadAsUInt(data, nvoxels, GIPL_HEADERSIZE);
    }
    break;
  case VTK_FLOAT:
    {
      float *data=
        ((vtkFloatArray *)DataArray)->WritePointer(0,nvoxels);
      this->from->CReadAsFloat(data, nvoxels, GIPL_HEADERSIZE);
    }
    break;
  default:
    vtkErrorMacro(<< "ReadVolume(): Unknown voxel type");
    exit(1);
  }

  return Scalars;
}




void vtkCISGGiplReader::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkCISGMultiReader::PrintSelf(os,indent);
}



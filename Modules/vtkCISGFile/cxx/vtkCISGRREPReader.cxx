/*=auto=========================================================================

(c) Copyright 2005 Massachusetts Institute of Technology (MIT) All Rights Reserved.

This software ("3D Slicer") is provided by The Brigham and Women's 
Hospital, Inc. on behalf of the copyright holders and contributors.
Permission is hereby granted, without payment, to copy, modify, display 
and distribute this software and its documentation, if any, for  
research purposes only, provided that (1) the above copyright notice and 
the following four paragraphs appear on all copies of this software, and 
(2) that source code to any modifications to this software be made 
publicly available under terms no more restrictive than those in this 
License Agreement. Use of this software constitutes acceptance of these 
terms and conditions.

3D Slicer Software has not been reviewed or approved by the Food and 
Drug Administration, and is for non-clinical, IRB-approved Research Use 
Only.  In no event shall data or images generated through the use of 3D 
Slicer Software be used in the provision of patient care.

IN NO EVENT SHALL THE COPYRIGHT HOLDERS AND CONTRIBUTORS BE LIABLE TO 
ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL 
DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, 
EVEN IF THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE BEEN ADVISED OF THE 
POSSIBILITY OF SUCH DAMAGE.

THE COPYRIGHT HOLDERS AND CONTRIBUTORS SPECIFICALLY DISCLAIM ANY EXPRESS 
OR IMPLIED WARRANTIES INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND 
NON-INFRINGEMENT.

THE SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS 
IS." THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE NO OBLIGATION TO 
PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.


=========================================================================auto=*/
/*=========================================================================
 
  Library   : vtk/objects/library/CISGImage
  Module    : $RCSfile: vtkCISGRREPReader.cxx,v $
  Authors   : Thomas Hartkens <thomas@hartkens.de>
              Daniel Rueckert <d.rueckert@doc.ic.ac.uk>
              Julia Schnabel  <julia.schnabel@kcl.ac.uk>
              under the supervision of Dave Hawkes and Derek Hill
  Web       : http://www.image-registration.com
  Copyright : King's College London
              Div. of Radiological Sciences, 
              Computational Imaging Science Group, 1997 - 2000
              http://www-ipg.umds.ac.uk/cisg
  Date      : $Date: 2005/04/19 14:48:26 $
  Version   : $Revision: 1.2 $

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

#include <string.h>
//#include <libgen.h>
#include <sys/stat.h>

#include "vtkObjectFactory.h"
#include "vtkShortArray.h"
#include "vtkCharArray.h"
#include "vtkIntArray.h"
#include "vtkUnsignedShortArray.h"
#include "vtkUnsignedCharArray.h"
#include "vtkUnsignedIntArray.h"
#include "vtkFloatArray.h"

#include "vtkCISGCifstream.h"
#include "vtkCISGRREPReader.h"

vtkCISGRREPReader* vtkCISGRREPReader::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkCISGRREPReader");
  if(ret)
    {
    return (vtkCISGRREPReader*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkCISGRREPReader;
}


// Construct object with NULL file prefix; file pattern "%s.%d"; image range 
// set to (1,1); data origin (0,0,0); data spacing (1,1,1); no data mask;
// header size 0; and byte swapping turned off.
vtkCISGRREPReader::vtkCISGRREPReader()
{
  this->FileName = NULL;
}


vtkCISGRREPReader::~vtkCISGRREPReader()
{
  if (this->FileName) delete [] this->FileName;
}


// Check header 
int vtkCISGRREPReader::CheckHeader(char *filename)
{
  int i= strlen(filename)-1;
  int j=0;
  char extention[20];
  while ((i>=0)  
         && (filename[i]!='/') && (filename[i]!='\\') 
         && (j<20)) {
    extention[j]=filename[i];
    i--; j++;
  }
  return !strncmp(extention,"IICSA.REDAEH",12) ||
    !strncmp(extention,"iicsa.redaeh",12);
}


void vtkCISGRREPReader::Execute()
{
  char imagename[255];
  struct stat buffer;

  this->from = new vtkCISGCifstream;
  vtkStructuredPoints *output = (vtkStructuredPoints *)this->GetOutput();

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
  int i= strlen(FileName)-1;
  while ((i>=0)  
         && (FileName[i]!='/') && (FileName[i]!='\\')) {
    i--; 
  }
  FileName[i+1]=0;
  sprintf(imagename, "%simage.bin", FileName);

  // Test whether the file is compressed or not
  if (stat(imagename, &buffer) == -1){
    sprintf(imagename, "%s.Z", imagename);
  }

  // Read data
  this->from->Open(imagename);
  vtkDataArray *newScalars = this->ReadVolume();
  this->from->Close();
  
  // Calculate dimensions of output from data dimensions
  output->SetDimensions(this->Dim);
  output->SetSpacing(this->Size);
  output->SetOrigin(this->Origin);
  output->SetWholeExtent(output->GetExtent());
  output->SetUpdateExtent(output->GetWholeExtent());

  // Set data volume
  if (newScalars){
    output->GetPointData()->SetScalars(newScalars);
  }

}


// Read header of data
void vtkCISGRREPReader::ReadHeader()
{
  //  int i;
  char buffer[255], *buffer1, *buffer2;

  // Goto beginning of header
  this->from->seekg(0);

  // Read header
  do {
    from->get(buffer, 255);
    from->seekg(1, ifstream::cur);
    if ((buffer2 = strstr(buffer, ":=")) != NULL){
      buffer2++;
      buffer2++;
      if ((buffer1 = strstr(buffer, "Image Dimensions")) != NULL){
        if (atoi(buffer2) != 3){
          cerr << "vtkCISGRREPReader::ReadHeader: Unexpected ";
          cerr << "dimension: " << buffer << endl;
        }
      }
      if ((buffer1 = strstr(buffer, "Gantry Tilt")) != NULL){
        if (atof(buffer2) != 0){
          cerr << "vtkCISGRREPReader::ReadHeader: Unexpected gantry ";
          cerr << "tilt: " << buffer << endl;
        }
      }
      if ((buffer1 = strstr(buffer, "Bits allocated")) != NULL){
        if (atoi(buffer2) != 16){
          cerr << "vtkCISGRREPReader::ReadHeader: Unexpected no. of ";
          cerr << "bits allocated: " << buffer << endl;
        }
      }
      if ((buffer1 = strstr(buffer, "Bits stored")) != NULL){
        if (atoi(buffer2) != 16){
          cerr << "vtkCISGRREPReader::ReadHeader: Unexpected no. of ";
          cerr << "bits stored: " << buffer << endl;
        }
      }
      if ((buffer1 = strstr(buffer, "High bit")) != NULL){
        if (atoi(buffer2) != 15){
          cerr << "vtkCISGRREPReader::ReadHeader: Unexpected high ";
          cerr << "bit: " << buffer << endl;
        }
      }
      if ((buffer1 = strstr(buffer, "Rows")) != NULL){
        this->Dim[0] = atoi(buffer2);
      }
      if ((buffer1 = strstr(buffer, "Columns")) != NULL){
        this->Dim[1] = atoi(buffer2);
      }
      if ((buffer1 = strstr(buffer, "Slices")) != NULL){
        this->Dim[2] = atoi(buffer2);
      }
      if ((buffer1 = strstr(buffer, "Slice thickness")) != NULL){
        this->Size[2] = atof(buffer2);
      }
      if ((buffer1 = strstr(buffer, "Pixel size")) != NULL){
        if ((buffer1 = strstr(buffer2, ":")) != NULL){
          buffer1[0] = 0;
          buffer1++;
          this->Size[0] = atof(buffer2);
          this->Size[1] = atof(buffer1);
        } else {
          cerr << "vtkCISGRREPReader::ReadHeader: Can't read pixel size:";
          cerr << buffer << endl;
        }
      }  
    } else {
      if (strlen(buffer) > 0){
        cerr << "vtkCISGRREPReader::ReadHeader: Can't parse line ";
        cerr << buffer << endl;
      }
    }
  } while (!this->from->eof());

  // Set origin
  Origin[0]   = (-Size[0]*(Dim[0]-1)/2.0);
  Origin[1]   = (-Size[1]*(Dim[1]-1)/2.0);
  Origin[2]   = (-Size[2]*(Dim[2]-1)/2.0);

  // Default is unsigned short for this format
  this->Type  = VTK_SHORT;
  this->Bytes = 2;

}


// Read a volume of data.
vtkDataArray *vtkCISGRREPReader::ReadVolume()
{
  // calculate the number of voxels
  int nvoxels = this->Dim[0] * this->Dim[1] * this->Dim[2];
  vtkDataArray *Scalars = vtkDataArray::CreateDataArray(this->Type);
  Scalars->Allocate(1,nvoxels);

  vtkDataArray *DataArray=Scalars;
  short *data = ((vtkShortArray *) DataArray)->WritePointer(0,nvoxels);
  this->from->CReadAsShort(data, nvoxels, 0);
  InternalFlip(1, data, this->Dim);
  return Scalars;
}


void vtkCISGRREPReader::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkCISGMultiReader::PrintSelf(os,indent);
}

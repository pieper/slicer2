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

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkCORReader.cxx,v $
  Language:  C++
  Date:      $Date: 2005/04/04 15:35:45 $
  Version:   $Revision: 1.6 $

=========================================================================*/
#include "vtkCORReader.h"
#include "vtkUnsignedShortArray.h"
#include "vtkShortArray.h"
#include "vtkUnsignedLongArray.h"
#include "vtkLongArray.h"
#include "vtkUnsignedCharArray.h"
#include "vtkCharArray.h"
#include "vtkFloatArray.h"
#include "vtkDoubleArray.h"
#include "vtkObjectFactory.h"

vtkCORReader* vtkCORReader::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkCORReader");
  if(ret)
    {
    return (vtkCORReader*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkCORReader;
}

vtkCORReader::vtkCORReader()
{
  this->DataDimensions[0] = 
    this->DataDimensions[1] = 
    this->DataDimensions[2] = 0;

}

vtkCORReader::~vtkCORReader()
{
}


void vtkCORReader::ExecuteInformation()
{
  vtkImageData *output = this->GetOutput();
  
  // Read the header.
  this->ReadVolumeHeader();

  // Set some data in the output.
  output->SetWholeExtent(0, this->DataDimensions[0]-1,
             0, this->DataDimensions[1]-1,
             0, this->DataDimensions[2]-1 );
  output->SetScalarType(VTK_UNSIGNED_CHAR);
  output->SetNumberOfScalarComponents(1);
  output->SetSpacing(this->DataSpacing);
  output->SetOrigin(this->DataOrigin);
}
    
    
void vtkCORReader::Execute()
{
  vtkImageData *output = this->GetOutput();

  // Read the header.
  this->ReadVolumeHeader();

  // Set some data in the output.
  output->SetWholeExtent(0, this->DataDimensions[0]-1,
             0, this->DataDimensions[1]-1,
             0, this->DataDimensions[2]-1 );
  output->SetScalarType(VTK_UNSIGNED_CHAR);
  output->SetNumberOfScalarComponents(1);
  output->SetDimensions(this->DataDimensions);
  output->SetSpacing(this->DataSpacing);
  output->SetOrigin(this->DataOrigin);

  // Get the volume values from the COR files. If we get them, copy
  // them to the output.
  vtkDataArray *newScalars = this->ReadVolumeData();
  if ( newScalars ) 
    {
      output->GetPointData()->SetScalars(newScalars);
      newScalars->Delete();
    }
}
// Due to a change in vtk between versions 4.0 and 4.1, GetImage's declaration
// is not backwards compatible. Compiler preprocessor directives won't wrap
// properly in TCL so there's no automatic way to detect this and have it
// compile properly. Uncomment the line after the next code line if the vtk
// version is lower than 4.1.  
vtkImageData *vtkCORReader::GetImage(int ImageNumber)
// vtkStructuredPoints *vtkCORReader::GetImage(int ImageNumber)
{
    cerr << "vtkCORReader::GetImage() called. uh oh." << endl;
    return NULL;
}

vtkDataArray *vtkCORReader::ReadVolumeData()
{
  vtkUnsignedCharArray *scalars = NULL;
  void* pixels;
  char sliceFileName[1024];
  FILE *fp;
  int numRead;
  int numPts;
  int numPtsPerSlice;
  int numReadTotal;
  int slice;
  int elementSize;

  // Read header first.
  this->ReadVolumeHeader();

  // Calc the number of values.
  numPts = this->DataDimensions[0] * 
    this->DataDimensions[1] * 
    this->DataDimensions[2];

  // Create the scalars for all of the images. Set the element size
  // for the data we will read, always a uchar. 
  vtkDebugMacro (<< "Creating vtkUnsignedCharArray");

  scalars = vtkUnsignedCharArray::New();
  if ( NULL == scalars ) 
    {
      vtkErrorMacro(<< "Couldn't allocate scalars array.");
      return NULL;
    } 
  
  scalars->Allocate(numPts);
  elementSize = sizeof( unsigned char );

  // For each slice we need.... (note that we go in reverse order)
  numReadTotal = 0;
  numPtsPerSlice = this->DataDimensions[0] * this->DataDimensions[1];
  for(slice = 256; slice >= 1; slice--) {

    // Build the file name for this slice.
    sprintf (sliceFileName, "%s/COR-%.3d", this->FilePrefix, slice);
    fp = fopen(sliceFileName,"rb");
    if ( !fp )
      {
    vtkErrorMacro(<< "Can't find/open file: " << sliceFileName);
    return NULL;
      }

    // Get a writable pointer to the scalar data for this slice so we
    // can write data into it.
    pixels = (void*) scalars->WritePointer(numReadTotal, numPtsPerSlice);

    // Read 256*256 unsigned chars of data into the volume.
    vtkDebugMacro(<< "Reading volume data" );

    numRead = fread( pixels, elementSize, numPtsPerSlice, fp );
    if ( numRead != numPtsPerSlice )
      {
    vtkErrorMacro(<<"Trying to read " << numPtsPerSlice << " elements, "
              "but only got " << numRead << " of them.");
    scalars->Delete();
    return NULL;
      }
    
    // Close this slice.
    fclose(fp);

    // Inc the number of values we read.
    numReadTotal += numPtsPerSlice;
  }

  // return the scalars.
  return scalars;
}

void vtkCORReader::ReadVolumeHeader()
{
  char headerFile[1024];
  FILE *fp;

  // We don't really need any header information because all the
  // values we use are constant, so all we do is check if the file
  // exists here, otherwise it could be an invalid directory.

  // Make the header file name and open it.
  sprintf(headerFile, "%s/COR-.info", this->FilePrefix);

  fp = fopen(headerFile, "r");
  if ( !fp )
    {
      vtkErrorMacro(<< "Can't find/open file: " << headerFile);
      return;
    }
  fclose(fp);

  // Set our dimensions to the standard valus
  this->DataDimensions[0] = 256;
  this->DataDimensions[1] = 256;
  this->DataDimensions[2] = 256;

  // Our spacing is default 1
  this->DataSpacing[0] = 1;
  this->DataSpacing[1] = 1;
  this->DataSpacing[2] = 1;

  // Our origin is default 0,0,0
  this->DataOrigin[0] = 0;
  this->DataOrigin[1] = 0;
  this->DataOrigin[2] = 0;
}

void vtkCORReader::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkVolumeReader::PrintSelf( os, indent );

  os << indent << "Data Dimensions: (" << this->DataDimensions[0] << ", "
     << this->DataDimensions[1] << ", " << this->DataDimensions[2] << ")\n";
}

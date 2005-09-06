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
  Module:    $RCSfile: vtkMGHReader.h,v $
  Language:  C++
  Date:      $Date: 2005/09/06 21:22:56 $
  Version:   $Revision: 1.4.6.1 $

=========================================================================*/
// .NAME vtkMGHReader - read an MGH (.mgh) volume file from Freesurfer tools
// .SECTION Description
// .SECTION Caveats
// .SECTION See Also
// vtkPolyData vtkDataReader

#ifndef __vtkMGHReader_h
#define __vtkMGHReader_h

#include <vtkFreeSurferReadersConfigure.h>
#include <stdio.h>
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkVolumeReader.h"
#include "vtkTransform.h"
#include <zlib.h>

// Header sizes.
const int FS_DIMENSION_HEADER_SIZE = sizeof(int) * 7;
const int FS_RAS_HEADER_SIZE = (sizeof(float) * 15) + sizeof(short);
const int FS_UNUSED_HEADER_SIZE = 256 - FS_RAS_HEADER_SIZE;
const int FS_WHOLE_HEADER_SIZE =
    FS_RAS_HEADER_SIZE + FS_DIMENSION_HEADER_SIZE + FS_UNUSED_HEADER_SIZE;

// Type constants. We read in one of these and then convert it to a
// VTK scalar constant.
const int MRI_UCHAR = 0;
const int MRI_INT = 1;
const int MRI_FLOAT = 3;
const int MRI_SHORT = 4;

class VTK_FREESURFERREADERS_EXPORT vtkMGHReader : public vtkVolumeReader
{
public:
  vtkTypeMacro(vtkMGHReader,vtkVolumeReader);

  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  static vtkMGHReader *New();

  // Description:
  // Specify file name of vtk data file to read.
  vtkSetStringMacro(FileName);
  vtkGetStringMacro(FileName);

  // Description:
  vtkGetVectorMacro(DataDimensions,int,3);

    vtkGetVectorMacro(RASMatrix,float,12);
    
  // Description: 
  // Other objects make use of these methods but we don't. Left here
  // but not implemented.
  vtkImageData *GetImage(int ImageNumber);

  // Description:
  // Reads the MGH file and gets header information from it.
  void          ReadVolumeHeader();
  // Description:
  // Reads the MGH file and creates an array of values.
  vtkDataArray *ReadVolumeData();
 

    // expose the values that the read volume header function sets
    vtkGetMacro(ScalarType,int);
    vtkGetMacro(NumFrames,int);
    
protected:
  vtkMGHReader();
  ~vtkMGHReader();

    // for testing
    void SetOutput();
    
  void Execute();
  void ExecuteInformation();

  // File name.
  char *FileName;

  // Dimensions of the volume.
  int DataDimensions[3];

  // Scalar type of the data.
  int ScalarType;

  // Number of frames.
  int NumFrames;

  // Frame we want to look at.
  int CurFrame;

  // RAS registration matrix. (currently unused)
  float RASMatrix[12];


  // Description:
  // Sets the current frame for the volume.
  void          SetFrameIndex( int index );

private:
  vtkMGHReader(const vtkMGHReader&);  // Not implemented.
  void operator=(const vtkMGHReader&);  // Not implemented.

};

#endif


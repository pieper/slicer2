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
  Module:    $RCSfile: vtkBVolumeReader.h,v $
  Language:  C++
  Date:      $Date: 2005/04/04 15:35:08 $
  Version:   $Revision: 1.3 $

=========================================================================*/
// .NAME vtkBVolumeReader - read an MGH (.mgh) volume file from Freesurfer tools
// .SECTION Description
// .SECTION Caveats
// .SECTION See Also
// vtkPolyData vtkDataReader

#ifndef __vtkBVolumeReader_h
#define __vtkBVolumeReader_h

#include <vtkFreeSurferReadersConfigure.h>
#include <stdio.h>
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkVolumeReader.h"
#include "vtkTransform.h"

// Header sizes.
const int FS_DIMENSION_HEADER_SIZE = sizeof(int) * 7;
const int FS_RAS_HEADER_SIZE = (sizeof(float) * 15) + sizeof(short);
const int FS_UNUSED_HEADER_SIZE = 256 - FS_RAS_HEADER_SIZE;
const int FS_WHOLE_HEADER_SIZE =
    FS_RAS_HEADER_SIZE + FS_DIMENSION_HEADER_SIZE + FS_UNUSED_HEADER_SIZE;


class VTK_FREESURFERREADERS_EXPORT vtkBVolumeReader : public vtkVolumeReader
{
public:
  vtkTypeMacro(vtkBVolumeReader,vtkVolumeReader);

  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  static vtkBVolumeReader *New();

  // Description:
  vtkSetStringMacro(FileName);
  vtkGetStringMacro(FileName);

  // Description:
  vtkSetStringMacro(SliceFileNameExtension);
  vtkGetStringMacro(SliceFileNameExtension);

  // Description:
  vtkSetStringMacro(RegistrationFileName);
  vtkGetStringMacro(RegistrationFileName);

  // Description:
  vtkSetStringMacro(Stem);
  vtkGetStringMacro(Stem);

  // Description:
  vtkGetVectorMacro(DataDimensions,int,3);

  // Description: 
  // Other objects make use of these methods but we don't. Left here
  // but not implemented.
  vtkImageData *GetImage(int ImageNumber);
    
  vtkMatrix4x4* GetRegistrationMatrix ();

    vtkGetMacro(ScalarType,int);
//    vtkGetVectorMacro(DataSpacing,int,3);
    
protected:
  vtkBVolumeReader();
  ~vtkBVolumeReader();

  void Execute();
  void ExecuteInformation();

  // File name.
  char *FileName;

  // File name of the registration matrix.
  char *RegistrationFileName;

  // These are calculated and used internally.
  char *SliceFileNameExtension;
  char *Stem;
  
  // Dimensions of the volume.
  int DataDimensions[3];
    // int DataSpacing[3];

  // Scalar type of the data.
  int ScalarType;

  // Number of time points.
  int NumTimePoints;
  

  // Time point we want to look at.
  int CurTimePoint;

  // RAS registration matrix. (currently unused)
  // 0 x_r x_a x_s 4
  // 3 y_r y_a y_s 5
  // 6 z_r z_a z_s 8
  // 9 c_r c_a c_s 11
  float RASMatrix[12];
  
  // Functional -> Anatomical registration matrix.
  vtkMatrix4x4 *RegistrationMatrix;

  // Meta data.
  float TE;
  float TR;
  float TI;
  float FlipAngle;
  
  // Description:
  // Reads the MGH file and creates an array of values.
  vtkDataArray *ReadVolumeData();

  // Description:
  // Reads the MGH file and gets header information from it.
  int ReadVolumeHeader();

  void FindStemFromFilePrefixOrFileName();
  void GuessTypeFromStem();

  // Description:
  // Sets the current frame for the volume.
  void          SetTimePoint( int timePoint );

private:
  vtkBVolumeReader(const vtkBVolumeReader&);  // Not implemented.
  void operator=(const vtkBVolumeReader&);  // Not implemented.
};

#endif


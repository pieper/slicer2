/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkMGHReader.h,v $
  Language:  C++
  Date:      $Date: 2003/04/14 20:08:49 $
  Version:   $Revision: 1.2 $

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

  // Description: 
  // Other objects make use of these methods but we don't. Left here
  // but not implemented.
  vtkImageData *GetImage(int ImageNumber);
    
protected:
  vtkMGHReader();
  ~vtkMGHReader();

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
  // Reads the MGH file and creates an array of values.
  vtkDataArray *ReadVolumeData();

  // Description:
  // Reads the MGH file and gets header information from it.
  void          ReadVolumeHeader();

  // Description:
  // Sets the current frame for the volume.
  void          SetFrameIndex( int index );

private:
  vtkMGHReader(const vtkMGHReader&);  // Not implemented.
  void operator=(const vtkMGHReader&);  // Not implemented.
};

#endif


/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkFSSurfaceWFileReader.h,v $
  Date:      $Date: 2005/12/20 22:55:38 $
  Version:   $Revision: 1.3.2.1 $

=========================================================================auto=*/
/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkFSSurfaceWFileReader.h,v $
  Language:  C++
  Date:      $Date: 2005/12/20 22:55:38 $
  Version:   $Revision: 1.3.2.1 $

=========================================================================*/

// .NAME vtkFSSurfaceWFileReader - read a surface w file (*.w) file
// from Freesurfer tools
// .SECTION Description
// Reads a surface w file file from FreeSurfer and outputs a
// vtkFloatArray. Use the SetFileName function to specify the file
// name. The number of values in the array should be equal to the
// number of vertices/points in the surface.

#ifndef __vtkFSSurfaceWFileReader_h
#define __vtkFSSurfaceWFileReader_h

#include <vtkFreeSurferReadersConfigure.h>
#include "vtkDataReader.h"
#include "vtkPolyData.h"
#include "vtkFloatArray.h"

// file type magic numbers
const int FS_NEW_SCALAR_MAGIC_NUMBER = 16777215;

// error codes
const int FS_ERROR_W_NONE = 0;
const int FS_ERROR_W_OUTPUT_NULL = 1;
const int FS_ERROR_W_NO_FILENAME = 2;
const int FS_ERROR_W_OPEN = 3;
const int FS_ERROR_W_NUM_VALUES = 4;
const int FS_ERROR_W_ALLOC = 5;
const int FS_ERROR_W_EOF = 6;

class VTK_FREESURFERREADERS_EXPORT vtkFSSurfaceWFileReader : public vtkDataReader
{
public:
  static vtkFSSurfaceWFileReader *New();
  vtkTypeMacro(vtkFSSurfaceWFileReader,vtkDataReader);
  void PrintSelf(ostream& os, vtkIndent indent);

  vtkFloatArray *GetOutput()
    {return this->scalars; };
  void SetOutput(vtkFloatArray *output)
    {this->scalars = output; };

  int ReadWFile();

    vtkGetMacro(NumberOfVertices,int);
    vtkSetMacro(NumberOfVertices,int);

protected:
  vtkFSSurfaceWFileReader();
  ~vtkFSSurfaceWFileReader();

  vtkFloatArray *scalars;


    // Description:
    // this is the number of vertices in the associated model file,
    // there may not be as many value in this scalar file as there
    // are vertices
    int NumberOfVertices;
    
  int ReadInt3 (FILE* iFile, int& oInt);
  int ReadInt2 (FILE* iFile, int& oInt);
  int ReadFloat (FILE* iFile, float& oInt);

private:
  vtkFSSurfaceWFileReader(const vtkFSSurfaceWFileReader&);  // Not implemented.
  void operator=(const vtkFSSurfaceWFileReader&);  // Not implemented.
};


#endif



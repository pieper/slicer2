/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkFSSurfaceReader.h,v $
  Language:  C++
  Date:      $Date: 2002/08/16 22:12:27 $
  Version:   $Revision: 1.1 $

=========================================================================*/
// .NAME vtkFSSurfaceReader - read a surface file from Freesurfer tools
// .SECTION Description
// Reads a surface file from FreeSurfer and output PolyData. Use the
// SetFileName function to specify the file name.

#ifndef __vtkFSSurfaceReader_h
#define __vtkFSSurfaceReader_h

#include <vtkFreeSurferReadersConfigure.h>
#include "vtkDataReader.h"
#include "vtkPolyData.h"


// This code can calc normals but it doesn't seem to do so very well
// (bug?) and vtkPolyDataNormals does it anyway.
#define FS_CALC_NORMALS 0

// file type magic numbers
const int FS_QUAD_FILE_MAGIC_NUMBER = (-1 & 0x00ffffff);
const int FS_NEW_QUAD_FILE_MAGIC_NUMBER = (-3 & 0x00ffffff);

const int FS_NUM_SIDES_IN_FACE = 4; // dealing with quads
const int FS_MAX_NUM_FACES_PER_VERTEX = 10; // kinda arbitrary

class VTK_FREESURFERREADERS_EXPORT vtkFSSurfaceReader : public vtkDataReader
{
public:
  static vtkFSSurfaceReader *New();
  vtkTypeMacro(vtkFSSurfaceReader,vtkDataReader);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Get the output of this reader.
  vtkPolyData *GetOutput();
  vtkPolyData *GetOutput(int idx)
    {return (vtkPolyData *) this->vtkSource::GetOutput(idx); };
  void SetOutput(vtkPolyData *output);

protected:
  vtkFSSurfaceReader();
  ~vtkFSSurfaceReader();

  void Execute();

  // Update extent of PolyData is specified in pieces.  
  // Since all DataObjects should be able to set UpdateExent as pieces,
  // just copy output->UpdateExtent  all Inputs.
  void ComputeInputUpdateExtents(vtkDataObject *output);
  
  // Used by streaming: The extent of the output being processed by
  // the execute method. Set in the ComputeInputUpdateExtents method.
  int ExecutePiece;
  int ExecuteNumberOfPieces;
  int ExecuteGhostLevel;

  int ReadInt3 (FILE* iFile, int& oInt);
  int ReadInt2 (FILE* iFile, int& oInt);
  int ReadFloat (FILE* iFile, float& oInt);

private:
  vtkFSSurfaceReader(const vtkFSSurfaceReader&);  // Not implemented.
  void operator=(const vtkFSSurfaceReader&);  // Not implemented.
};


// If we're going to try to do normals, we need to keep some
// connectivity information for the verts and faces. Use these
// structures.
#if FS_CALC_NORMALS
typedef struct {
  int numFaces;
  int faces[FS_MAX_NUM_FACES_PER_VERTEX];
  int indicesInFace[FS_MAX_NUM_FACES_PER_VERTEX];
  float x, y, z;
  float nx, ny, nz;
} Vertex;

typedef struct {
  int vertices[FS_NUM_SIDES_IN_FACE];
} Face;
#endif

#endif



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
  Module:    $RCSfile: vtkFSSurfaceReader.h,v $
  Language:  C++
  Date:      $Date: 2005/04/04 15:35:08 $
  Version:   $Revision: 1.6 $

=========================================================================*/
// .NAME vtkFSSurfaceReader - read a surface file from Freesurfer tools
// .SECTION Description
// Reads a surface file from FreeSurfer and output PolyData. Use the
// SetFileName function to specify the file name.

#ifndef __vtkFSSurfaceReader_h
#define __vtkFSSurfaceReader_h

#include <vtkFreeSurferReadersConfigure.h>
#include "vtkDataReader.h"
#include "vtkCellArray.h"
#include "vtkPolyData.h"

// Prints debugging info.
#define FS_DEBUG 0

// This code can calc normals but it doesn't seem to do so very well
// (bug?) and vtkPolyDataNormals does it anyway.
#define FS_CALC_NORMALS 0

// file type magic numbers
const int FS_QUAD_FILE_MAGIC_NUMBER = (-1 & 0x00ffffff);
const int FS_NEW_QUAD_FILE_MAGIC_NUMBER = (-3 & 0x00ffffff);
const int FS_TRIANGLE_FILE_MAGIC_NUMBER = (-2 & 0x00ffffff);

const int FS_NUM_VERTS_IN_QUAD_FACE = 4; // dealing with quads
const int FS_NUM_VERTS_IN_TRI_FACE = 3; // dealing with tris
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



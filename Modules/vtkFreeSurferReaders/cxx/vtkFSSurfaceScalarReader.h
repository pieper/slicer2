/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkFSSurfaceScalarReader.h,v $
  Language:  C++
  Date:      $Date: 2002/10/07 22:07:01 $
  Version:   $Revision: 1.3 $

=========================================================================*/
// .NAME vtkFSSurfaceScalarReader - read a surface scalar file from
// Freesurfer tools
// .SECTION Description
// Reads a surface scalar file from FreeSurfer and outputs a
// vtkFloatArray. Use the SetFileName function to specify the file
// name. The number of values in the array should be equal to the
// number of vertices/points in the surface.

#ifndef __vtkFSSurfaceScalarReader_h
#define __vtkFSSurfaceScalarReader_h

#include <vtkFreeSurferReadersConfigure.h>
#include "vtkDataReader.h"
#include "vtkPolyData.h"

// file type magic numbers
const int FS_NEW_SCALAR_MAGIC_NUMBER = 16777215;

class VTK_FREESURFERREADERS_EXPORT vtkFSSurfaceScalarReader : public vtkDataReader
{
public:
  static vtkFSSurfaceScalarReader *New();
  vtkTypeMacro(vtkFSSurfaceScalarReader,vtkDataReader);
  void PrintSelf(ostream& os, vtkIndent indent);

  vtkFloatArray *GetOutput()
    {return (vtkFloatArray *) this->vtkSource::GetOutput(0); };
  vtkFloatArray *GetOutput(int idx)
    {return (vtkFloatArray *) this->vtkSource::GetOutput(idx); };
  void SetOutput(vtkFloatArray *output)
    {this->SetNthOutput(0, (vtkDataObject*)output);}

  void ReadFSScalars();

protected:
  vtkFSSurfaceScalarReader();
  ~vtkFSSurfaceScalarReader();


  int ReadInt3 (FILE* iFile, int& oInt);
  int ReadInt2 (FILE* iFile, int& oInt);
  int ReadFloat (FILE* iFile, float& oInt);

private:
  vtkFSSurfaceScalarReader(const vtkFSSurfaceScalarReader&);  // Not implemented.
  void operator=(const vtkFSSurfaceScalarReader&);  // Not implemented.
};


#endif



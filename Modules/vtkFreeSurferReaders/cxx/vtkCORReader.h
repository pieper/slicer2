/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkCORReader.h,v $
  Language:  C++
  Date:      $Date: 2003/02/19 23:54:40 $
  Version:   $Revision: 1.4 $

=========================================================================*/
// .NAME vtkFSCORReader - read COR file volume from Freesurfer tools
// .SECTION Description
// .SECTION Caveats
// .SECTION See Also
// vtkPolyData vtkDataReader

#ifndef __vtkCORReader_h
#define __vtkCORReader_h

#include <vtkFreeSurferReadersConfigure.h>
#include <stdio.h>
#include "vtkImageData.h"
#include "vtkVolumeReader.h"
#include "vtkTransform.h"

class VTK_FREESURFERREADERS_EXPORT vtkCORReader : public vtkVolumeReader
{
public:
  vtkTypeMacro(vtkCORReader,vtkVolumeReader);

  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  static vtkCORReader *New();

  // Description:
  vtkGetVectorMacro(DataDimensions,int,3);

  // Description: 
  // Other objects make use of these methods but we don't. Left here
  // but not implemented.
  vtkImageData *GetImage(int ImageNumber);
    
protected:
  vtkCORReader();
  ~vtkCORReader();

  void Execute();
  void ExecuteInformation();

  // COR volumes are always 256^3, so this is just for future changes.
  int DataDimensions[3];

  // Description:
  // Reads the actual COR files and creates an array of values.
  vtkDataArray *ReadVolumeData();

  // Description:
  // Read the COR-.info file and get header information from it.
  void          ReadVolumeHeader();

private:
  vtkCORReader(const vtkCORReader&);  // Not implemented.
  void operator=(const vtkCORReader&);  // Not implemented.
};

#endif


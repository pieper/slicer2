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
  Module:    $RCSfile: vtkFSSurfaceScalarReader.h,v $
  Language:  C++
  Date:      $Date: 2005/04/04 15:35:08 $
  Version:   $Revision: 1.7 $

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
#include "vtkFloatArray.h"

// file type magic numbers
const int FS_NEW_SCALAR_MAGIC_NUMBER = 16777215;

class VTK_FREESURFERREADERS_EXPORT vtkFSSurfaceScalarReader : public vtkDataReader
{
public:
  static vtkFSSurfaceScalarReader *New();
  vtkTypeMacro(vtkFSSurfaceScalarReader,vtkDataReader);
  void PrintSelf(ostream& os, vtkIndent indent);

  vtkFloatArray *GetOutput()
    {return this->scalars; };
  void SetOutput(vtkFloatArray *output)
    {this->scalars = output; };

  void ReadFSScalars();

protected:
  vtkFSSurfaceScalarReader();
  ~vtkFSSurfaceScalarReader();

  vtkFloatArray *scalars;

  int ReadInt3 (FILE* iFile, int& oInt);
  int ReadInt2 (FILE* iFile, int& oInt);
  int ReadFloat (FILE* iFile, float& oInt);

private:
  vtkFSSurfaceScalarReader(const vtkFSSurfaceScalarReader&);  // Not implemented.
  void operator=(const vtkFSSurfaceScalarReader&);  // Not implemented.
};


#endif



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
  Module:    $RCSfile: vtkCORReader.h,v $
  Language:  C++
  Date:      $Date: 2005/04/04 15:35:08 $
  Version:   $Revision: 1.7 $

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
#include "vtkPointData.h"
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
    // Due to a change in vtk between versions 4.0 and 4.1, GetImage's
    // declaration is not backwards compatible. Compiler preprocessor
    // directives won't wrap properly in TCL so there's no automatic
    // way to detect this and have it compile properly
    // Uncomment the line after the next code line if the vtk version
    // is lower than 4.1.
    vtkImageData *GetImage(int ImageNumber);  
//  vtkStructuredPoints *GetImage(int ImageNumber);


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


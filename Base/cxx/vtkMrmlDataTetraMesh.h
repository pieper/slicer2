/*=auto=========================================================================

(c) Copyright 2003 Massachusetts Institute of Technology (MIT) All Rights Reserved.

This software ("3D Slicer") is provided by The Brigham and Women's 
Hospital, Inc. on behalf of the copyright holders and contributors.
Permission is hereby granted, without payment, to copy, modify, display 
and distribute this software and its documentation, if any, for internal 
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
// .NAME vtkMrmlDataTetraMesh - Object used in the slicer to perform
// everything related to the access and display of image data (volumes).
// .SECTION Description
// Used in conjunction with a vtkMrmlDataTetraMeshNode (which neatly describes
// display settings, file locations, etc.).  Essentially, the MRML 
// node gives the high level description of what this class should 
// actually do with the ImageData.
// 

#ifndef __vtkMrmlDataTetraMesh_h
#define __vtkMrmlDataTetraMesh_h

//#include <fstream.h>
#include <stdlib.h>
//#include <iostream.h>

#include "vtkMrmlData.h"

#include "vtkMrmlTetraMeshNode.h"
#include "vtkUnstructuredGrid.h"
#include "vtkSlicer.h"

//----------------------------------------------------------------------------
class VTK_SLICER_BASE_EXPORT vtkMrmlDataTetraMesh : public vtkMrmlData
{
public:
  static vtkMrmlDataTetraMesh *New();
  vtkTypeMacro(vtkMrmlDataTetraMesh,vtkMrmlData);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  // Description:
  // Provides opportunity to insure internal consistency before access. 
  // Transfers all ivars from MrmlNode to internal VTK objects
  void Update();
  unsigned long int GetMTime();

  // Description:
  // Set the image data
  // Use GetOutput to get the image data.
  vtkSetObjectMacro(TheMesh, vtkUnstructuredGrid);
  vtkUnstructuredGrid* GetOutput();

  // Description:
  // Read/Write image 
  int Read();
  int Write();

protected:
  vtkMrmlDataTetraMesh();
  ~vtkMrmlDataTetraMesh();
  vtkMrmlDataTetraMesh(const vtkMrmlDataTetraMesh&) {};
  void operator=(const vtkMrmlDataTetraMesh&) {};

  // Description: 
  // If Data has not be created, create it.
  void CheckMrmlNode();

  vtkUnstructuredGrid *TheMesh;
};

#endif

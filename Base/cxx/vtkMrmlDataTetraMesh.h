/*=auto=========================================================================

(c) Copyright 2002 Massachusetts Institute of Technology

Permission is hereby granted, without payment, to copy, modify, display 
and distribute this software and its documentation, if any, for any purpose, 
provided that the above copyright notice and the following three paragraphs 
appear on all copies of this software.  Use of this software constitutes 
acceptance of these terms and conditions.

IN NO EVENT SHALL MIT BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, 
INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE 
AND ITS DOCUMENTATION, EVEN IF MIT HAS BEEN ADVISED OF THE POSSIBILITY OF 
SUCH DAMAGE.

MIT SPECIFICALLY DISCLAIMS ANY EXPRESS OR IMPLIED WARRANTIES INCLUDING, 
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR 
A PARTICULAR PURPOSE, AND NON-INFRINGEMENT.

THE SOFTWARE IS PROVIDED "AS IS."  MIT HAS NO OBLIGATION TO PROVIDE 
MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

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

//----------------------------------------------------------------------------
class VTK_EXPORT vtkMrmlDataTetraMesh : public vtkMrmlData
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

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
// .NAME vtkFastCellPicker - select a cell by shooting a ray into graphics window
// .SECTION Description
// vtkFastCellPicker is used to select a cell by shooting a ray into graphics
// window and intersecting with actor's defining geometry - specifically 
// its cells. Beside returning coordinates, actor and mapper, vtkCellPicker
// returns the id of the closest cell within the tolerance along the pick
// ray, and the dataset that was picked.
// .SECTION See Also
// vtkPicker vtkPointPicker

#ifndef __vtkFastCellPicker_h
#define __vtkFastCellPicker_h

#include "vtkVersion.h"
#include "vtkCollection.h"
#include "vtkAssemblyPath.h"
#include "vtkDataSet.h"
#include "vtkMapper.h"
#include "vtkActor.h"
#include "vtkPicker.h"
#include "vtkOBBTree.h"
#include "vtkSlicer.h"

class VTK_SLICER_BASE_EXPORT vtkFastCellPicker : public vtkPicker
{
public:
  vtkFastCellPicker();
  static vtkFastCellPicker *New() {return new vtkFastCellPicker;};
  const char *GetClassName() {return "vtkFastCellPicker";};
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Get the id of the picked cell. If CellId = -1, nothing was picked.
  vtkGetMacro(CellId,int);

  // Description:
  // Get the subId of the picked cell. If SubId = -1, nothing was picked.
  vtkGetMacro(SubId,int);

  // Description:
  // Get the parametric coordinates of the picked cell. Only valid if 
  // pick was made.
  vtkGetVectorMacro(PCoords,float,3);

protected:
  int CellId; // picked cell
  int SubId; // picked cell subId
  float PCoords[3]; // picked cell parametric coordinates

#if ( (VTK_MAJOR_VERSION == 3 && VTK_MINOR_VERSION == 2) || VTK_MAJOR_VERSION == 4 )
  virtual float IntersectWithLine(float p1[3], float p2[3], float tol, 
                  vtkAssemblyPath *assem, vtkActor *a, vtkMapper *m);
#else
  virtual float IntersectWithLine(float p1[3], float p2[3], float tol, 
                  vtkActor *assem, vtkActor *a, vtkMapper *m);
#endif
  void Initialize();
  vtkCollection *OBBTrees;
};

#endif



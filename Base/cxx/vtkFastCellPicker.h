/*=auto=========================================================================
Copyright (c) 2001 Surgical Planning Lab, Brigham and Women's Hospital
 
Direct all questions on this copyright to slicer@ai.mit.edu.
The following terms apply to all files associated with the software unless
explicitly disclaimed in individual files.   

The authors hereby grant permission to use, copy (but NOT distribute) this
software and its documentation for any NON-COMMERCIAL purpose, provided
that existing copyright notices are retained verbatim in all copies.
The authors grant permission to modify this software and its documentation 
for any NON-COMMERCIAL purpose, provided that such modifications are not 
distributed without the explicit consent of the authors and that existing
copyright notices are retained in all copies. Some of the algorithms
implemented by this software are patented, observe all applicable patent law.

IN NO EVENT SHALL THE AUTHORS OR DISTRIBUTORS BE LIABLE TO ANY PARTY FOR
DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
OF THE USE OF THIS SOFTWARE, ITS DOCUMENTATION, OR ANY DERIVATIVES THEREOF,
EVEN IF THE AUTHORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

THE AUTHORS AND DISTRIBUTORS SPECIFICALLY DISCLAIM ANY WARRANTIES, INCLUDING,
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
PARTICULAR PURPOSE, AND NON-INFRINGEMENT.  THIS SOFTWARE IS PROVIDED ON AN
'AS IS' BASIS, AND THE AUTHORS AND DISTRIBUTORS HAVE NO OBLIGATION TO PROVIDE
MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
=========================================================================auto=*/
/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkFastCellPicker.h,v $
  Language:  C++
  Date:      $Date: 2001/02/03 01:21:37 $
  Version:   $Revision: 1.4 $


Copyright (c) 1993-1998 Ken Martin, Will Schroeder, Bill Lorensen.

This software is copyrighted by Ken Martin, Will Schroeder and Bill Lorensen.
The following terms apply to all files associated with the software unless
explicitly disclaimed in individual files. This copyright specifically does
not apply to the related textbook "The Visualization Toolkit" ISBN
013199837-4 published by Prentice Hall which is covered by its own copyright.

The authors hereby grant permission to use, copy, and distribute this
software and its documentation for any purpose, provided that existing
copyright notices are retained in all copies and that this notice is included
verbatim in any distributions. Additionally, the authors grant permission to
modify this software and its documentation for any purpose, provided that
such modifications are not distributed without the explicit consent of the
authors and that existing copyright notices are retained in all copies. Some
of the algorithms implemented by this software are patented, observe all
applicable patent law.

IN NO EVENT SHALL THE AUTHORS OR DISTRIBUTORS BE LIABLE TO ANY PARTY FOR
DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
OF THE USE OF THIS SOFTWARE, ITS DOCUMENTATION, OR ANY DERIVATIVES THEREOF,
EVEN IF THE AUTHORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

THE AUTHORS AND DISTRIBUTORS SPECIFICALLY DISCLAIM ANY WARRANTIES, INCLUDING,
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
PARTICULAR PURPOSE, AND NON-INFRINGEMENT.  THIS SOFTWARE IS PROVIDED ON AN
"AS IS" BASIS, AND THE AUTHORS AND DISTRIBUTORS HAVE NO OBLIGATION TO PROVIDE
MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.


=========================================================================*/
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
#include "vtkPicker.h"
#include "vtkOBBTree.h"

class VTK_EXPORT vtkFastCellPicker : public vtkPicker
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

#if (VTK_MAJOR_VERSION == 3 && VTK_MINOR_VERSION == 2)
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



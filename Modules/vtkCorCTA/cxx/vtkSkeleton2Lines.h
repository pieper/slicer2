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

// .NAME vtkSkeleton2Lines - Creates lines from a skeleton
// .SECTION Description
// .SECTION see also

#ifndef __vtkSkeleton2Lines_h
#define __vtkSkeleton2Lines_h

// Authors: Karl Krissian, Arne Hans
// Brigham and Women's Hospital

#include <vtkCorCTAConfigure.h>
#include "vtkStructuredPointsToPolyDataFilter.h"
#include "vtkImageData.h"


//BTX
//----------------------------------------------------------------------
class extremity {
//    ---------
public:
  int x,y,z;

  extremity()
    {
      x=y=z= 0;
    }

  extremity(int x0, int y0, int z0)
    {
      x  = x0;
      y  = y0;
      z  = z0;
    }
};
//ETX


//----------------------------------------------------------------------
class VTK_CORCTA_EXPORT vtkSkeleton2Lines : public vtkStructuredPointsToPolyDataFilter
//               -----------------
{
public:
  vtkTypeMacro(vtkSkeleton2Lines,vtkStructuredPointsToPolyDataFilter);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Construct instance of vtkSkeleton2Lines 
  static vtkSkeleton2Lines *New();
  
  // Minimum number of points per line
  vtkSetMacro(MinPoints,int);
  vtkGetMacro(MinPoints,int);

protected:
  vtkSkeleton2Lines();
  ~vtkSkeleton2Lines();
  vtkSkeleton2Lines(const vtkSkeleton2Lines&) {};

  void Init();
  void Init_Pos();

  void ExecuteData(vtkDataObject* output);

//BTX
  int pos[3][3][3];
  int neighbors_pos  [27];
  int neighbors_place[27][3];

  int tx,ty,tz,txy;

  vtkImageData* InputImage; 
  vtkPolyData* OutputPoly;
  
  int MinPoints;
//ETX
};

#endif

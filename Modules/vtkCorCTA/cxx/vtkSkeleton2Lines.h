/*=auto==========================================================================
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
==========================================================================auto=*/

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

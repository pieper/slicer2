/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkSkeleton2Lines.h,v $
  Language:  C++
  Date:      $Date: 2003/12/18 19:53:23 $
  Version:   $Revision: 1.1 $


=========================================================================*/

// .NAME vtkSkeleton2Lines - Creates lines from a skeleton
// .SECTION Description
// .SECTION see also

#ifndef __vtkSkeleton2Lines_h
#define __vtkSkeleton2Lines_h

// Author Karl Krissian
// Brigham and Women's Hospital
// 05 06 2002

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

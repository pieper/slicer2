/*=auto=========================================================================

(c) Copyright 2001 Massachusetts Institute of Technology

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
// .NAME vtkPolyBoolean - perform boolean union, subtraction or intersection of volumes
// .SECTION Description
// .SECTION Caveats
// .SECTION See Also

#ifndef __vtkPolyBoolean_h
#define __vtkPolyBoolean_h

#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkPolyDataToPolyDataFilter.h"
#include "vtkOBBTree.h"
#include "vtkMatrix4x4.h"
#include "vtkPolygon.h"
#include "vtkBoolTess.h"
#include "vtkSlicer.h"

// Defines for Operation
#define BOOL_A_MINUS_B 0
#define BOOL_A_OR_B 1
#define BOOL_A_AND_B 2
#define BOOL_A_AND_B_1D 3
#define BOOL_A_TOUCHES_B 4
#define BOOL_DEBUG 5

// Defines for CellFlags
#define BOOL_CELL_UNKNOWN 0
#define BOOL_CELL_IN_RESULT 1
#define BOOL_CELL_CUT 2
#define BOOL_CELL_NOT_IN_RESULT 3

class vtkBoolTri;
class vtkPiercePoint;
//
// Special classes to support boolean intersections.
//
//BTX - begin tcl exclude
//
class vtkBoolTriEdge { //; prevent man page generation
public:
  vtkBoolTriEdge();
  ~vtkBoolTriEdge();

  void DeletePPs();

  int Flag;
  int Points[2];
  vtkBoolTri *Neighbors[2]; // the two neighbor triangles
  vtkPiercePoint *FirstPP;
  vtkBoolTriEdge *Next[2]; // next edge on Neighbors 0 & 1
  class vtkPiercePoint *PPoints[2];
};

class vtkPiercePoint { //; prevent man page generation
public:
  vtkPiercePoint();
  ~vtkPiercePoint();

  int NewId;
  void MergePP( vtkPiercePoint *otherPP );
  float Param; // parameter on edge
  float Xparam; // parameter on intersection
  int SnapIdx; // Index of equivalent input data point, or -1 if new.
  float Point[3]; // 3d position of PP
  vtkBoolTriEdge *Edge; // piercing edge;
  vtkBoolTri *Triangle;
  vtkPiercePoint *Next; // pointer to next PP on this edge
  vtkPiercePoint *Prev; // pointer to previous PP on this edge
  vtkPiercePoint *Merge; // next PP to merge with this one.
  vtkBoolTriEdge *NextEdge[2]; // pointer to next Edge along
                               // intersection in each direction
};

class vtkNewBoolEdges { //; prevent man page generation
public:
  vtkNewBoolEdges();
  ~vtkNewBoolEdges();

  void AddNewEdge( vtkBoolTriEdge *thisEdge );
  void Reset();
  int GetCount();
  vtkBoolTriEdge *Get( int idx ) { return this->Array[idx]; };

protected:
  vtkBoolTriEdge **Array;
  int ArraySize;
  int Count;
  int GrowSize;
};

class vtkBoolLoop { //;prevent man page generation
public:
  vtkBoolLoop();
  ~vtkBoolLoop();

  int sign;          // Positive for outer loops, negative for inner loops.
  vtkIdList *Points; // ids of loop vertices
  vtkBoolLoop *Next;     // next triangle in the triangulation of this cell
};

class vtkBoolTri { //;prevent man page generation
public:
  vtkBoolTri();
  ~vtkBoolTri();

  int AorB;             // 0=from PolyDataA, 1=from PolyDataB.
  float Normal[3];      // triangle Normal
  float Offset;         // plane offset of this triangle ( Normal*p0 )
  vtkBoolTriEdge *Edges[3]; // pointers to the three edges
  vtkBoolTri *Next;     // next triangle in the triangulation of this cell
  int CellId;           // id of the parent cell
  vtkBoolLoop *NewLoops;// linked list of resulting loops
};
//ETX - end tcl exclude
//

class VTK_SLICER_BASE_EXPORT vtkPolyBoolean : public vtkPolyDataToPolyDataFilter
{
public:
//  vtkPolyBoolean(vtkPolyData *pd=NULL);
  vtkPolyBoolean();
  ~vtkPolyBoolean();
  static vtkPolyBoolean *New() {return new vtkPolyBoolean;};
  const char *GetClassName() {return "vtkPolyBoolean";};
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Set the distance resolution. Default is 1.0E-6;
  vtkSetMacro(DistanceResolution,float);
  vtkGetMacro(DistanceResolution,float);

  // Description:
  // Set the angle resolution. Default is .003 degrees.
  vtkSetMacro(AngleResolution,float);
  vtkGetMacro(AngleResolution,float);

  void SPLTestIntersection();
  
  // Description:
  // Set the boolean operation. Default is A-B.
  void SetOperation( int operation );
  vtkGetMacro(Operation,int);
  
  // Description:
  // Override the SetInput functino from vtkPolyDataFilter
  //  void SetInput2( vtkPolyData *input );

  // Description:
  // Specify the second polydata source for the operation
  void SetPolyDataB( vtkPolyData *polyDataB );
  vtkPolyData *GetPolyDataB() {return (vtkPolyData *)this->PolyDataB;};

  // Description
  // Specify the two xform matrices;
  vtkSetObjectMacro(XformA,vtkMatrix4x4);
  vtkGetObjectMacro(XformA,vtkMatrix4x4);
  vtkSetObjectMacro(XformB,vtkMatrix4x4);
  vtkGetObjectMacro(XformB,vtkMatrix4x4);

  vtkGetMacro(IntersectionCount,int);
  vtkGetMacro(TriangleCount,int);
  vtkGetMacro(NodeCount,int);
  
  vtkGetMacro(TestCount,int);


  unsigned long int GetMTime();

  // Description:
  // Override the default Update method since this filter has multiple
  // inputs.
  void UpdateCutter();

protected:
  void Execute();

  static int ProcessTwoNodes(vtkOBBNode *nodeA, vtkOBBNode *nodeB,
                             vtkMatrix4x4 *XformBtoA,
                             void *bool_void);
  void BuildBPoints( vtkMatrix4x4 *XformBtoA );
  void AddCellTriangles( int cellId, int *ptIds, int type, int numPts,
                         int AorB );
  int IntersectCellPair( int cellIdA, int cellIdB );
  int IntersectBoolTriPair( vtkBoolTri *triA, vtkBoolTri *triB );
  void MakeNewPoint( vtkPiercePoint *inPP );
  vtkPolyData *PolyDataB;
  int Operation;
  int IntersectionCount; // Number of intersecting triangle pairs
  int TriangleCount; // Number of triangle pairs intersected
  int NodeCount; // Number of intersecting OBBTree node pairs
  int TestCount;
  vtkMatrix4x4 *XformA;
  vtkMatrix4x4 *XformB;
  float DistanceResolution;
  float AngleResolution;
  vtkNewBoolEdges NewEdges;
  vtkPoints *NewPoints;
  vtkCellArray *NewPolys;
  vtkCellArray *NewLines;
  vtkCellArray *NewVerts;
  int ExecutionCount;

  void DeleteTriDirectory( int AorB );
  void ResetBoolTris();
  void DeleteNewEdges();
  void ProcessNewEdges();
  void FormLoops();
  void ClassifyCells();
  void GatherMarkCellNeighbors( int AorB, vtkPolyData *dataset,
                                int cellId, int marker );
  void AddNewPolygons( vtkBoolTri *thisTri );
  void SortLoops( vtkBoolTri *thisTri );
  void DisplayIntersectionGeometry();

private:
  vtkOBBTree *OBBTreeA;
  vtkOBBTree *OBBTreeB;
  vtkPoints *BPoints;
  vtkBoolTri **TriDirectory[2];
  int *CellFlags[2];
  int *CellFlagsB;
  int *NewPointIds[2];
  int NewIdsUpdate;
  vtkIdList *ThisLoop;
  vtkIdList *TriPts;
  int IdOffsetB;
  int IdOffsetNew;
  vtkBoolTess *Tess;
  int TriDirectorySize[2];
  
  // time when Execute method was last called
  vtkTimeStamp ExecuteTime;
};

#endif

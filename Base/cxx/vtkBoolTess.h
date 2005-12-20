/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkBoolTess.h,v $
  Date:      $Date: 2005/12/20 22:44:08 $
  Version:   $Revision: 1.11.12.1 $

=========================================================================auto=*/
#ifndef __vtkBoolTess_h
#define __vtkBoolTess_h

#include "vtkObject.h"
#include "vtkSlicer.h"

#ifndef vtkFloatingPointType
#define vtkFloatingPointType float
#endif

//
//BTX - begin tcl exclude
//
#define VTK_BOOL_MAX_CONTOURS 100

typedef struct booltessvtx {
  int PntId;
  int Flag;
  struct booltessedge *NextEdge;
  struct booltessedge *PrevEdge;
  } vtkBoolTessVtx;

typedef struct booltessedge {
  vtkBoolTessVtx *Vertices[2];
  struct booltessedge *Next;
  struct booltessedge *Prev;
  } vtkBoolTessEdge;

// This structure represents an active pair of edges as the
// sweepline advances. Edge[0] points to the forward-pointing
// edge, while Edge[1] points to the rearward-point edge. Vtx
// points to the vertex at the beginning of the current
// trapezoid.

typedef struct booltesspair {
  vtkBoolTessEdge *Edges[2];
  vtkBoolTessEdge *PrevEdge;
  } vtkBoolTessPair;

//ETX - end tcl exclude
//

class VTK_SLICER_BASE_EXPORT vtkBoolTess { //; prevent man page generation
public:
  vtkBoolTess();
  ~vtkBoolTess();

  /*  inline */ static vtkBoolTess *New() {return (new vtkBoolTess);}
  /*  inline */ void Delete() { delete this; }
  void SetPoints( vtkFloatingPointType *points );
  int AddContour( vtkIdType nPts, vtkIdType *ptIds );
  void Reset();
  int Triangulate( vtkIdType **tris );

protected:
  int GenerateTriangles();
  static int SortCompare(const void *arg1, const void *arg2 );

  // this is the input data
  int NumContours;
  vtkIdType NLoopPts[VTK_BOOL_MAX_CONTOURS];
  vtkIdType *Contours[VTK_BOOL_MAX_CONTOURS];
  
  //BTX - begin tcl exclude
  vtkFloatingPointType (*Points)[3];
  //ETX - end tcl exclude

  // this data is used for the algorithm
  int ProjAxis;
  int SortAxis;
  int YAxis;
  int Orient;
  
  int NumInputEdges;
  int NumNewEdges;
  vtkBoolTessVtx *Vertices;
  vtkBoolTessEdge *InputEdges;
  vtkBoolTessEdge *NewEdges;
  vtkBoolTessVtx **SortArray;
  vtkBoolTessPair *ActivePairs;

  // this is the output data
  int NumTriangles;
  vtkIdType *Triangles;

  double ProjTriangleArea( int ptId0, int ptId1, int ptId2 );
  void AddNewEdges( vtkBoolTessEdge *prevEdge, vtkBoolTessEdge *nextEdge );
  void TriangulateMonotone( vtkBoolTessEdge *firstEdge );
  int PrevNumInputEdges;
  };
#endif

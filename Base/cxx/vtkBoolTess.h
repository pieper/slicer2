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
#ifndef __vtkBoolTess_h
#define __vtkBoolTess_h

#include "vtkObject.h"

#include "vtkSlicer.h"

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
  void SetPoints( float *points );
  int AddContour( int nPts, int *ptIds );
  void Reset();
  int Triangulate( int **tris );

protected:
  int GenerateTriangles();
  static int SortCompare(const void *arg1, const void *arg2 );

  // this is the input data
  int NumContours;
  int NLoopPts[VTK_BOOL_MAX_CONTOURS];
  int *Contours[VTK_BOOL_MAX_CONTOURS];
  
  //BTX - begin tcl exclude
  float (*Points)[3];
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
  int *Triangles;

  double ProjTriangleArea( int ptId0, int ptId1, int ptId2 );
  void AddNewEdges( vtkBoolTessEdge *prevEdge, vtkBoolTessEdge *nextEdge );
  void TriangulateMonotone( vtkBoolTessEdge *firstEdge );
  int PrevNumInputEdges;
  };
#endif

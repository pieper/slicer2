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
#ifndef __vtkBoolTess_h
#define __vtkBoolTess_h

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

class vtkBoolTess { //; prevent man page generation
public:
  vtkBoolTess();
  ~vtkBoolTess();

  inline static vtkBoolTess *New();
  inline void Delete();
  void SetPoints( float *points );
  int AddContour( int nPts, int *ptIds );
  void Reset();
  int Triangulate( int **tris );

protected:
  GenerateTriangles();
  static int SortCompare( void *arg1, void *arg2 );

  // this is the input data
  int NumContours;
  int NLoopPts[VTK_BOOL_MAX_CONTOURS];
  int *Contours[VTK_BOOL_MAX_CONTOURS];
  float (*Points)[3];

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

inline vtkBoolTess * vtkBoolTess::New()
  {
  return( new vtkBoolTess );
  }

inline void vtkBoolTess::Delete()
  {
  delete this;
  }
#endif

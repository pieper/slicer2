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

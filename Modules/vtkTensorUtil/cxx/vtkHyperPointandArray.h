#ifndef __vtkHyperPointandArray_h
#define __vtkHyperPointandArray_h

#include "vtkSystemIncludes.h"

// copied directly from vtkHyperStreamline.
// this class was defined in the vtkHyperStreamline.cxx file.

//
// Special classes for manipulating data
//BTX
class vtkHyperPoint { //;prevent man page generation
public:
    vtkHyperPoint(); // method sets up storage
    vtkHyperPoint &operator=(const vtkHyperPoint& hp); //for resizing
    
    float   X[3];    // position 
    vtkIdType     CellId;  // cell
    int     SubId; // cell sub id
    float   P[3];    // parametric coords in cell 
    float   W[3];    // eigenvalues (sorted in decreasing value)
    float   *V[3];   // pointers to eigenvectors (also sorted)
    float   V0[3];   // storage for eigenvectors
    float   V1[3];
    float   V2[3];
    float   S;       // scalar value 
    float   D;       // distance travelled so far 
};
//ETX

class vtkHyperArray { //;prevent man page generation
public:
  vtkHyperArray();
  ~vtkHyperArray()
    {
      if (this->Array)
        {
        delete [] this->Array;
        }
    };
  vtkIdType GetNumberOfPoints() {return this->MaxId + 1;};
  vtkHyperPoint *GetHyperPoint(vtkIdType i) {return this->Array + i;};
  vtkHyperPoint *InsertNextHyperPoint() 
    {
    if ( ++this->MaxId >= this->Size )
      {
      this->Resize(this->MaxId);
      }
    return this->Array + this->MaxId;
    }
  vtkHyperPoint *Resize(vtkIdType sz); //reallocates data
  void Reset() {this->MaxId = -1;};

  vtkHyperPoint *Array;  // pointer to data
  vtkIdType MaxId;             // maximum index inserted thus far
  vtkIdType Size;              // allocated size of data
  vtkIdType Extend;            // grow array by this amount
  float Direction;       // integration direction
};

#define VTK_START_FROM_POSITION 0
#define VTK_START_FROM_LOCATION 1

#endif

#ifndef __vtkPreciseHyperPoint_h
#define __vtkPreciseHyperPoint_h

//BTX

#include "vtkIdType.h"

class vtkPreciseHyperPoint { //;prevent man page generation
public:
    vtkPreciseHyperPoint(); // method sets up storage
    vtkPreciseHyperPoint &operator=(const vtkPreciseHyperPoint& hp); //for resizing
    
    float   X[3];    // position 
    vtkIdType CellId;  // cell
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

#endif

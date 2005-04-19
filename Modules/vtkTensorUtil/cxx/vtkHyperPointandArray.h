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
#ifndef __vtkHyperPointandArray_h
#define __vtkHyperPointandArray_h

#include "vtkSystemIncludes.h"
#include "vtkStreamer.h"
#include "vtkTensorUtilConfigure.h"

// copied directly from vtkHyperStreamline.
// this class was defined in the vtkHyperStreamline.cxx file.

//
// Special classes for manipulating data
//BTX
class VTK_TENSORUTIL_EXPORT vtkHyperPoint { //;prevent man page generation
public:
    vtkHyperPoint(); // method sets up storage
    vtkHyperPoint &operator=(const vtkHyperPoint& hp); //for resizing
    
    vtkFloatingPointType   X[3];    // position 
    vtkIdType     CellId;  // cell
    int     SubId; // cell sub id
    vtkFloatingPointType   P[3];    // parametric coords in cell 
    vtkFloatingPointType   W[3];    // eigenvalues (sorted in decreasing value)
    vtkFloatingPointType      *V[3];   // pointers to eigenvectors (also sorted)
    vtkFloatingPointType   V0[3];   // storage for eigenvectors
    vtkFloatingPointType   V1[3];
    vtkFloatingPointType   V2[3];
    vtkFloatingPointType   S;       // scalar value 
    vtkFloatingPointType   D;       // distance travelled so far 
};
//ETX

//BTX
class VTK_TENSORUTIL_EXPORT vtkHyperArray { //;prevent man page generation
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
  vtkFloatingPointType Direction;       // integration direction
};
//ETX

#define VTK_START_FROM_POSITION 0
#define VTK_START_FROM_LOCATION 1

#endif

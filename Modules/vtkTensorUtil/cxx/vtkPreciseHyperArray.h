#ifndef __vtkPreciseHyperArray_h
#define __vtkPreciseHyperArray_h 

//BTX
#include "vtkIdType.h"
#include "vtkPreciseHyperPoint.h"
class vtkPreciseHyperArray { //;prevent man page generation
public:
  vtkPreciseHyperArray();
  ~vtkPreciseHyperArray()
    {
      if (this->Array)
        {
        delete [] this->Array;
        }
    };
  vtkIdType GetNumberOfPoints() {return this->MaxId + 1;};
  vtkPreciseHyperPoint *GetPreciseHyperPoint(vtkIdType i) {return this->Array + i;};
  vtkPreciseHyperPoint *InsertNextPreciseHyperPoint() 
    {
    if ( ++this->MaxId >= this->Size )
      {
      this->Resize(this->MaxId);
      }
    return this->Array + this->MaxId;
    }
  vtkPreciseHyperPoint *Resize(vtkIdType sz); //reallocates data
  void Reset() {this->MaxId = -1;};

  vtkPreciseHyperPoint *Array;  // pointer to data
  vtkIdType MaxId;             // maximum index inserted thus far
  vtkIdType Size;              // allocated size of data
  vtkIdType Extend;            // grow array by this amount
  float Direction;       // integration direction
  vtkIdType MaxAngleLastId;
  void IncrementMaxAngleLastId(void) { MaxAngleLastId++; }
  float DistanceSoFarMaxAngle(void) { 
    if ( this->Array[MaxId].D == -1.0 && MaxId > 0 ) 
      return this->Array[MaxId-1].D - this->Array[MaxAngleLastId+1].D;
    else
      return this->Array[MaxId].D - this->Array[MaxAngleLastId+1].D;
  }
  float CosineOfAngle(void);
};
//ETX

#endif

#ifndef __vtkDataIntervalDrop_h
#define __vtkDataIntervalDrop_h

#include "vtkIntervalDrop.h"
#include "vtkDataObject.h"

class VTK_EXPORT vtkDataIntervalDrop : public vtkIntervalDrop {
 public:
    static vtkDataIntervalDrop *New ();
    vtkTypeRevisionMacro(vtkDataIntervalDrop, vtkIntervalDrop);
    void PrintSelf(ostream& os, vtkIndent indent);

    vtkDataObject *dropData;

 protected:
    vtkDataIntervalDrop ();
    vtkDataIntervalDrop ( vtkTransform& tr);
    ~vtkDataIntervalDrop ();

 private:
    vtkDataIntervalDrop(const vtkDataIntervalDrop&); //Not implemented
    void operator=(const vtkDataIntervalDrop&); //Not implemented
};
#endif

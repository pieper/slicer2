#ifndef __vtkImageIntervalDrop_h
#define __vtkImageIntervalDrop_h

#include "vtkIntervalDrop.h"
#include "vtkImageData.h"


class VTK_EXPORT vtkImageIntervalDrop : public vtkIntervalDrop {

 public:
    static vtkImageIntervalDrop *New();        
    void PrintSelf (ostream& os, vtkIndent indent );
    vtkTypeRevisionMacro (vtkImageIntervalDrop, vtkIntervalDrop );

    // image data.
    vtkImageData *dropData;
    
 protected:
    vtkImageIntervalDrop ( );
    vtkImageIntervalDrop ( vtkTransform& t);
    vtkImageIntervalDrop ( vtkImageInterval *ref );
    ~vtkImageIntervalDrop ( );
        
 private:
};

#endif


#ifndef __vtkITKVersion_h
#define __vtkITKVersion_h

#include "vtkObjectFactory.h"

#include <itkConfigure.h>
#include <itkVersion.h>

class VTK_EXPORT vtkITKVersion : public vtkObject
{
public:
    static vtkITKVersion *New();
    vtkTypeMacro(vtkITKVersion,vtkObject);
    void PrintSelf(ostream& os, vtkIndent indent);
    
    char *GetITKVersion();
};
#endif

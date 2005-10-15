// .NAME vtkTclHelper - a bridge between tcl and vtk

// .SECTION Description

// .SECTION See Also
// vtkTkRenderWidget 


#ifndef __vtkTclHelper_h
#define __vtkTclHelper_h

#include "vtkTcl.h"
#include "vtkObject.h"
#include "vtkImageData.h"

#include <vtkQueryAtlasConfigure.h>


class VTK_QUERYATLAS_EXPORT vtkTclHelper : public vtkObject
{
public:
    void PrintSelf(ostream& os, vtkIndent indent);
    vtkTypeRevisionMacro(vtkTclHelper, vtkObject);

    static vtkTclHelper *New();

    void SetInterpFromCommand(unsigned long tag);

    vtkSetObjectMacro(ImageData, vtkImageData);
    vtkGetObjectMacro(ImageData, vtkImageData);

    void SendImageDataScalars(char *sockname);
    void ReceiveImageDataScalars(char *sockname);
    const char *Execute (char *Command);


protected:
    vtkTclHelper();
    ~vtkTclHelper(); 

    vtkImageData *ImageData;           
    Tcl_Interp *Interp;           /* Tcl interpreter */
};


#endif

#include "vtkIntervalRenderController.h"
#include <vtkIbrowserConfigure.h>


vtkCxxRevisionMacro(vtkIntervalRenderController, "$Revision: 1.1 $");

//--------------------------------------------------------------------------------------
vtkIntervalRenderController *vtkIntervalRenderController::New ( )
{
    vtkObject* ret = vtkObjectFactory::CreateInstance ( "vtkIntervalRenderController" );
    if ( ret ) {
        return ( vtkIntervalRenderController* ) ret;
    }
    return new vtkIntervalRenderController;
}

//--------------------------------------------------------------------------------------
vtkIntervalRenderController::vtkIntervalRenderController ( ) {
    this->Opacity = 1.0;
}


//--------------------------------------------------------------------------------------
vtkIntervalRenderController::~vtkIntervalRenderController ( ) {

}


//--------------------------------------------------------------------------------------
void vtkIntervalRenderController::PrintSelf(ostream& os, vtkIndent indent)
{
    vtkObject::PrintSelf(os, indent);
    
}


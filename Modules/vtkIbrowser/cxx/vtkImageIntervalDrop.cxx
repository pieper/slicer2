#include "vtkImageIntervalDrop.h"

vtkCxxRevisionMacro(vtkImageIntervalDrop, "$Revision: 1.3 $");

//-------------------------------------------------------------------

vtkImageIntervalDrop *vtkImageIntervalDrop::New ( ) {

    vtkObject *ret = vtkObjectFactory::CreateInstance ( "vtkImageIntervalDrop" );
    if (ret) {
        return ( vtkImageIntervalDrop* ) ret;
    }
    return new vtkImageIntervalDrop;
}



//-------------------------------------------------------------------
vtkImageIntervalDrop::vtkImageIntervalDrop ( ) 
{
    // allocate image data.
    this->dropData = vtkImageData::New ( );
    this->myTransform = NULL;
    this->next = NULL;
    this->prev = NULL;
}



//-------------------------------------------------------------------
vtkImageIntervalDrop::vtkImageIntervalDrop ( vtkTransform& t )
{
    // allocate image data.
    this->dropData = vtkImageData::New ( );
    this->myTransform = &t;
    this->next = NULL;
    this->prev = NULL;

}


//-------------------------------------------------------------------
vtkImageIntervalDrop::vtkImageIntervalDrop ( vtkImageInterval *ref )
{
    // allocate image data.
    this->dropData = vtkImageData::New ( );
    this->myTransform = &t;
    this->next = NULL;
    this->prev = NULL;
    this->referenceDrop = ref;

}

//-------------------------------------------------------------------
vtkImageIntervalDrop::~vtkImageIntervalDrop ( )
{
    // delete image data.
    this->dropData->Delete ( );
    this->myTransform ->Delete ( );
}



//-------------------------------------------------------------------
void vtkImageIntervalDrop::PrintSelf(ostream& os, vtkIndent indent)
{
    vtkIntervalDrop::PrintSelf(os, indent);
}

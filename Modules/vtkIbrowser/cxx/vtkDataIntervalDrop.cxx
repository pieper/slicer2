#include "vtkDataIntervalDrop.h"

vtkCxxRevisionMacro(vtkDataIntervalDrop, "$Revision: 1.1 $");

//--------------------------------------------------------------------------------------
vtkDataIntervalDrop *vtkDataIntervalDrop::New( )
{
    vtkObject *ret = vtkObjectFactory::CreateInstance ("vtkDataIntervalDrop");
    if (ret) 
        {
        return ( vtkDataIntervalDrop* ) ret;
        }
    return new vtkDataIntervalDrop;
}



//--------------------------------------------------------------------------------------
vtkDataIntervalDrop::vtkDataIntervalDrop ( ) 
{
    // allocate data.
    this->drop = vtkDataObject::New ( );
    this->next = NULL;
    this->prev = NULL;
    this->myTransform = NULL;
}


//--------------------------------------------------------------------------------------
vtkDataIntervalDrop::vtkDataIntervalDrop ( vtkTransform& tr)
{
    // add t into the transform pipeline for this drop.
    this->drop = vtkDataObject::New ( );
    this->next = NULL;
    this->prev = NULL;
    this->myTransform = &tr;
}

//--------------------------------------------------------------------------------------
vtkDataIntervalDrop::~vtkDataIntervalDrop ( ) 
{
    // delete data.
    this->drop->Delete ( );
    this->myTransform->Delete ();
}


//--------------------------------------------------------------------------------------
void vtkDataIntervalDrop::PrintSelf(ostream& os, vtkIndent indent)
{
    vtkIntervalDrop::PrintSelf(os, indent);
    
}

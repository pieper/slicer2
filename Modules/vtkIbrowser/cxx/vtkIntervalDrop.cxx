#include "vtkIntervalDrop.h"

vtkCxxRevisionMacro(vtkIntervalDrop, "$Revision: 1.1 $");

//Description:
//--------------------------------------------------------------------------------------
vtkIntervalDrop *vtkIntervalDrop::New ( )
{
    vtkObject* ret = vtkObjectFactory::CreateInstance ( "vtkIntervalDrop" );
    if ( ret ) {
        return (vtkIntervalDrop* )ret;
    }
    return new vtkIntervalDrop;
}



//Description:
//--------------------------------------------------------------------------------------
vtkIntervalDrop::vtkIntervalDrop ( ) {

    this->dropPosition = 0.0;
    this->dropSustain = 1;
    this->dropDuration = 0.0;
    this->dropTimestep = 0.0;
    this->dropDimensions[0] = 0;
    this->dropDimensions[1] = 0;
    this->dropDimensions[2] = 0;
    this->referenceDrop = NULL;
    this->dropname = NULL;
    this->DropID = -1;
    this->RefID = -1;
    this->next = NULL;
    this->prev = NULL;
}



//--------------------------------------------------------------------------------------
vtkIntervalDrop::vtkIntervalDrop ( vtkTransform& t ) 
{
    this->dropPosition = 0.0;
    this->dropSustain = 1;
    this->dropDuration = 0.0;
    this->dropTimestep = 0.0;
    this->dropDimensions[0] = 0;
    this->dropDimensions[1] = 0;
    this->dropDimensions[2] = 0;
    this->referenceDrop = NULL;
    this->dropname = NULL;
    this->myTransform = &t;
    this->DropID = -1;
    this->RefID = -1;
    this->next = NULL;
    this->prev = NULL;
}



//Description:
//--------------------------------------------------------------------------------------
vtkIntervalDrop::vtkIntervalDrop ( char *name ) {

    this->dropPosition = 0.0;
    this->dropSustain = 1;
    this->dropDuration = 0.0;
    this->dropTimestep = 0.0;
    this->dropDimensions[0] = 0;
    this->dropDimensions[1] = 0;
    this->dropDimensions[2] = 0;
    this->referenceDrop = NULL;
    this->dropname = name;
    this->DropID = -1;
    this->RefID = -1;
    this->next = NULL;
    this->prev = NULL;
}


//description:
//--------------------------------------------------------------------------------------
vtkIntervalDrop::~vtkIntervalDrop ( ) {
    delete [] this->dropname;
}



//Description:
//--------------------------------------------------------------------------------------
void vtkIntervalDrop::PrintSelf(ostream &os, vtkIndent indent)
{
    vtkObject::PrintSelf(os, indent);
    os << indent << "dropPosition: " << this->dropPosition << "\n";
    os << indent << "dropSustain: " << this->dropSustain << "\n";
    os << indent << "dropDuration: " << this->dropDuration << "\n";    
    os << indent << "dropTimestep: " << this->dropTimestep << "\n";
    os << indent << "dropname: " << this->dropname << "\n";    
    os << indent << "dropIndex: " << this->dropIndex << "\n";
    os << indent << "DropID: " << this->DropID << "\n";
}


//--------------------------------------------------------------------------------------
vtkIntervalDrop *vtkIntervalDrop::getNext( ) {
    return this->next;
}


//--------------------------------------------------------------------------------------
vtkIntervalDrop *vtkIntervalDrop::getPrev( ){
    return this->prev;
}


//Description:
//--------------------------------------------------------------------------------------
void vtkIntervalDrop::setReferenceDrop ( vtkIntervalDrop *ref )
{
    this->referenceDrop = ref;
}



//Description:
//--------------------------------------------------------------------------------------
vtkIntervalDrop *vtkIntervalDrop::getReferenceDrop ( ) 
{
    return this->referenceDrop;
}



//Description:
//--------------------------------------------------------------------------------------
void vtkIntervalDrop::setDropTransform ( vtkTransform *mat )
{
    this->myTransform = mat;
}


//Description:
//--------------------------------------------------------------------------------------
vtkTransform *vtkIntervalDrop::getDropTransform ( )
{
    return this->myTransform;
}



//Description:
//--------------------------------------------------------------------------------------
void vtkIntervalDrop::shiftDrop ( float shiftAmount )
{
    this->dropPosition = this->dropPosition + shiftAmount;

}



//Description:
//--------------------------------------------------------------------------------------
int vtkIntervalDrop::getDropType ( ) {
    return this->dropType;
}




//Description:
//--------------------------------------------------------------------------------------
void vtkIntervalDrop::setDropType (int mytype) {
    this->dropType = mytype;
}

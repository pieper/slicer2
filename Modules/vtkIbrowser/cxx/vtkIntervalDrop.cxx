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
#include "vtkIntervalDrop.h"

vtkCxxRevisionMacro(vtkIntervalDrop, "$Revision: 1.2 $");

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

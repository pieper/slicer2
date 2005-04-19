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
#include "vtkInterval.h"


vtkCxxRevisionMacro(vtkInterval, "$Revision: 1.2 $");
vtkIntervalSpan *vtkInterval::globalSpan = vtkIntervalSpan::New();

//--------------------------------------------------------------------------------------
vtkInterval *vtkInterval::New ( )
{
    vtkObject* ret = vtkObjectFactory::CreateInstance ( "vtkInterval" );
    if ( ret ) {
        return ( vtkInterval* ) ret;
    }
    return new vtkInterval;
}





//--------------------------------------------------------------------------------------
vtkInterval::vtkInterval ( ) {

    this->name = "defaultNew";
    this->IntervalID = -1;
    this->RefID = -1;
    this->IntervalType = IMAGEINTERVAL;
    this->order = 0;
    this->isSelected = 0;
    this->visibility = 1;
    this->opacity = 1.0;
    this->referenceInterval = NULL;
    this->mySpan = vtkIntervalSpan::New();
    this->myTransform = vtkTransform::New();
    this->dropHash = vtkIntervalHash::New();
    this->dropCollection= vtkIntervalDropCollection::New();
    this->dropCollection->mySpan = this->mySpan;
    this->dropCollection->collectionType = IntervalType;
    this->next = NULL;
    this->prev = NULL;
}





//--------------------------------------------------------------------------------------
vtkInterval::vtkInterval ( char *myname, int type) {

    this->name = myname;
    this->IntervalID = -1;
    this->RefID = -1;
    this->IntervalType = type;
    this->order = 0;
    this->isSelected = 0;
    this->visibility = 1;
    this->opacity = 1.0;
    this->referenceInterval = NULL;
    this->mySpan = vtkIntervalSpan::New();
    this->myTransform = vtkTransform::New();
    this->dropHash = vtkIntervalHash::New();
    this->dropCollection= vtkIntervalDropCollection::New();
    this->dropCollection->mySpan = this->mySpan;
    this->dropCollection->collectionType = IntervalType;
    this->next = NULL;
    this->prev = NULL;
}





//--------------------------------------------------------------------------------------
vtkInterval::vtkInterval ( char *myname, int type, float max, float min ) {

    this->name = myname;
    this->IntervalID = -1;
    this->RefID = -1;
    this->IntervalType = type;
    this->order = 0;
    this->isSelected = 0;
    this->visibility = 1;
    this->opacity = 1.0;
    this->referenceInterval = NULL;
    this->mySpan = vtkIntervalSpan::New();
    this->mySpan->setSpan( min, max );
    this->myTransform = vtkTransform::New();
    this->dropHash = vtkIntervalHash::New();
    this->dropCollection= vtkIntervalDropCollection::New();
    this->dropCollection->mySpan = this->mySpan;
    this->dropCollection->collectionType = IntervalType;
    this->next = NULL;
    this->prev = NULL;
}



//--------------------------------------------------------------------------------------
vtkInterval::vtkInterval (char *myname, int type, float max, float min, vtkInterval *ref) {

    this->name = myname;
    this->IntervalID = -1;
    this->RefID = ref->IntervalID;
    this->IntervalType = type;
    this->order = 0;
    this->isSelected = 0;
    this->visibility = 1;
    this->opacity = 1.0;
    this->referenceInterval = ref;
    this->mySpan = vtkIntervalSpan::New();
    this->mySpan->setSpan( min, max );
    this->myTransform = vtkTransform::New();
    this->dropHash = vtkIntervalHash::New();
    this->dropCollection= vtkIntervalDropCollection::New();
    this->dropCollection->mySpan = this->mySpan;
    this->dropCollection->collectionType = IntervalType;
    this->next = NULL;
    this->prev = NULL;
}


//--------------------------------------------------------------------------------------
vtkInterval::~vtkInterval ( ) {

    this->mySpan->Delete ( );
    this->myTransform->Delete ( );
    this->dropHash->Delete ( );
    this->dropCollection->Delete ( );
}




// Description:
// editInterval allows an interval's name, visibility and opacity
// to be edited.
//--------------------------------------------------------------------------------------
void vtkInterval::editIntervalProperties ( char *newname ) {

    this->name = newname;
}




// Description:
// editInterval allows an interval's name, visibility and opacity
// to be edited.
//--------------------------------------------------------------------------------------
void vtkInterval::editIntervalProperties ( int newvis) {

    this->visibility = newvis;
}





// Description:
// editInterval allows an interval's name, visibility and opacity
// to be edited.
//--------------------------------------------------------------------------------------
void vtkInterval::editIntervalProperties ( float newopaq ) {

    this->opacity = newopaq;
}




// Description:
// shiftInterval translates the interval's span and
// shifts all drops within it. 
//--------------------------------------------------------------------------------------
void vtkInterval::shiftInterval ( float shiftAmount ) {

    // shift all drops.
    this->dropCollection->shiftDrops ( shiftAmount );

}





// Description:
// scaleIntervalAroundCenter scales an interval around its
// center point, changing its start and endpoints.
//--------------------------------------------------------------------------------------
void vtkInterval::scaleIntervalAroundCenter ( float scaleAmount ) {

    this->mySpan->scaleSpanAroundCenter ( scaleAmount);
}




// Description:
// scaleIntervalFromStart pins an interval at its start and
// scales it, changing its endpoint.
//--------------------------------------------------------------------------------------
void vtkInterval::scaleIntervalFromStart( float scaleAmount ) {

    this->mySpan->scaleSpanFromStart ( scaleAmount);
}




// Description:
// normalizeInterval normalizes an interval given a max and min
// which correspond to 0.0 and 1.0.
//--------------------------------------------------------------------------------------
void vtkInterval::normalizeIntervalSpan ( float max, float min ) {

    this->mySpan->normalizeSpan ( max, min );
}





// Description:
// selectInterval marks an interval as selected.
//--------------------------------------------------------------------------------------
void vtkInterval::selectInterval ( ) {

    this->isSelected = 1;
}





// Description:
// deselectInterval marks an interval as deselected.
//--------------------------------------------------------------------------------------
void vtkInterval::deselectInterval ( ) {

    this->isSelected = 0;
}




// Description:
// toggleVisibility changes an interval's visibility, from
// either visible or invisible.
//--------------------------------------------------------------------------------------
void vtkInterval::toggleVisibility ( ) {

    if ( this->visibility = 0 )
        this->visibility = 1;
    else this->visibility = 0;
}




// Description:
// setSampledNonlinearly marks an interval as having
// irregularly spaced drops.
//--------------------------------------------------------------------------------------
void vtkInterval::setSampledNonlinearly ( ) {
    this->dropCollection->regularDropSpacingFlag = 0;
}




// Description:
// setSampledLinearly marks an interval as having
// regularly spaced drops.
//--------------------------------------------------------------------------------------
void vtkInterval::setSampledLinearly ( ) {
    this->dropCollection->regularDropSpacingFlag = 1;
}








// Description:
// setReferenceInterval sets a pointer to
// the interval it references for transformations.
//--------------------------------------------------------------------------------------
void vtkInterval::setReferenceInterval ( vtkInterval *ref ) {

    this->referenceInterval = ref;
}



// Description:
//--------------------------------------------------------------------------------------
vtkInterval *vtkInterval::getReferenceInterval ( ){
    return this->referenceInterval;
}


// Description:
//--------------------------------------------------------------------------------------
void vtkInterval::setGlobalSpan ( vtkIntervalSpan *sp ) {
float start, stop;
    start = sp->GetunitStart ( );
    stop = sp->GetunitStop ( );
    this->globalSpan->updateSpan ( start, stop );
}



// Description:
//--------------------------------------------------------------------------------------
void vtkInterval::setGlobalSpan (float min, float max ) {

    this->globalSpan->updateSpan ( min, max );
}


// Description:
//--------------------------------------------------------------------------------------
vtkIntervalSpan *vtkInterval::getGlobalSpan () {

    return this->globalSpan;
}



// Description:
//--------------------------------------------------------------------------------------
void vtkInterval::setSpan (float min, float max){

    this->mySpan->updateSpan( min, max);

}

// Description:
//--------------------------------------------------------------------------------------
vtkIntervalSpan *vtkInterval::getSpan ( ){

    return this->mySpan;
}


// Description:
//--------------------------------------------------------------------------------------
void vtkInterval::setTransform ( vtkTransform *t )
{
 this->myTransform = t;
}




// Description:
//--------------------------------------------------------------------------------------
vtkTransform *vtkInterval::getTransform ( )
{
    return this->myTransform;
}




//Description:
//--------------------------------------------------------------------------------------
void vtkInterval::PrintSelf(ostream &os, vtkIndent indent)
{
    vtkObject::PrintSelf(os, indent);
}









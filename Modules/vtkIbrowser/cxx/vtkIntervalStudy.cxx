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
#include "vtkIntervalStudy.h"

//Description:
// A vtkIntervalStudy contains a doubly-linked list of
// related intervals. The collection's GUI representation
// mediates the exploration of data contained within each
// interval in the collection. The collection can be named,
// edited, saved and reloaded.
//--------------------------------------------------------------------------------------
vtkIntervalStudy *vtkIntervalStudy::New ( )
{
    vtkObject* ret = vtkObjectFactory::CreateInstance ( "vtkIntervalStudy" );
    if ( ret ) {
        return ( vtkIntervalStudy* ) ret;
    }
    return new vtkIntervalStudy;
}




//--------------------------------------------------------------------------------------
vtkIntervalStudy::vtkIntervalStudy ( ) {

    // initialize some parameters.
    this->Name = "defaultNew";
    this->zoomfactor = 1;
    this->intervalCollection = vtkIntervalCollection::New();
    this->player = NULL;
}



//--------------------------------------------------------------------------------------
vtkIntervalStudy::vtkIntervalStudy ( int ID ) {

    // initialize some parameters.
    this->Name = "defaultNew";
    this->zoomfactor = 1;
    this->intervalCollection = vtkIntervalCollection::New(ID);
    this->player = NULL;
}



//--------------------------------------------------------------------------------------
vtkIntervalStudy::vtkIntervalStudy ( float min, float max ) {

    this->Name = "defaultNew";
    this->zoomfactor = 1;
    this->intervalCollection = vtkIntervalCollection::New( min, max );
    this->player = NULL;
}


//--------------------------------------------------------------------------------------
vtkIntervalStudy::vtkIntervalStudy ( float min, float max, int ID ) {

    this->Name = "defaultNew";
    this->zoomfactor = 1;
    this->intervalCollection = vtkIntervalCollection::New( min, max, ID);
    this->player = NULL;
}




//--------------------------------------------------------------------------------------
vtkIntervalStudy::~vtkIntervalStudy ( ) {
}





//--------------------------------------------------------------------------------------
vtkCxxRevisionMacro ( vtkIntervalStudy, "$revision: 1.1 $" );






//--------------------------------------------------------------------------------------
void vtkIntervalStudy::PrintSelf ( ostream &os, vtkIndent indent ) {

    vtkObject::PrintSelf (os, indent );
}





//--------------------------------------------------------------------------------------
void vtkIntervalStudy::deleteIntervalStudy ( ){

    //delete intervals, but keep the empty collection.
    this->Name = "";
}


//--------------------------------------------------------------------------------------
void vtkIntervalStudy::zoomInIntervalStudy ( int zf ) {

    // zooms in by a factor of zoomfactor
    this->zoomfactor = zf;

}



//--------------------------------------------------------------------------------------
void vtkIntervalStudy::zoomOutIntervalStudy ( int zf ) {

    // zooms in by a factor of zoomfactor
    this->zoomfactor = zf;
}





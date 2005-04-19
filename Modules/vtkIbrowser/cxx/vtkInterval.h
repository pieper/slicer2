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
#ifndef __vtkInterval_h
#define __vtkInterval_h


#include "vtkObject.h"
#include "vtkObjectFactory.h"
#include "vtkIntervalConfig.h"
#include "vtkIntervalHash.h"
#include "vtkIntervalDropCollection.h"
#include "vtkIntervalSpan.h"
#include "vtkTransform.h"
#include "vtkImageToImageFilter.h"
#include <vtkIbrowserConfigure.h>

//-------------------------------------------------------------------
// Description:
// A vtkInterval is an abstract container for an ordered collection of
// vtkIntervalDrops  in a hash table (vtkIntervalHash), and keyed by
// a user-selected  index. There are different subclasses of vtkInterval.
// Related vtkIntervals are stored together in a vtkIntervalCollection.
//-------------------------------------------------------------------

class VTK_EXPORT vtkInterval : public vtkObject {

 public:

    static vtkInterval *New ();
    vtkTypeRevisionMacro(vtkInterval, vtkObject);
    void PrintSelf (ostream& os, vtkIndent indent);
    
    // vtkGet/Set interval parameters
    vtkSetStringMacro ( name );
    vtkGetStringMacro ( name );
    vtkSetMacro ( IntervalID, int );
    vtkGetMacro ( IntervalID, int );
    vtkSetMacro ( RefID, int );
    vtkGetMacro ( RefID, int );
    vtkSetMacro ( order, int );
    vtkGetMacro ( order, int );    
    vtkGetMacro ( numDrops, int );
    vtkSetMacro ( numDrops, int );
    vtkSetMacro ( isSelected, int );
    vtkGetMacro ( isSelected, int );
    vtkSetMacro ( visibility, int );
    vtkGetMacro ( visibility, int );
    vtkSetMacro ( opacity, float );
    vtkGetMacro ( opacity, float );
    vtkGetMacro ( MRMLid, int);
    vtkSetMacro ( MRMLid, int);
    void setReferenceInterval ( vtkInterval *ref );
    vtkInterval *getReferenceInterval ( );
    void setSampledLinearly ( );
    void setSampledNonlinearly ( );
    void setTransform ( vtkTransform *t );
    vtkTransform *getTransform ( );
    void setGlobalSpan ( vtkIntervalSpan *sp );
    void setGlobalSpan ( float min, float max );
    vtkIntervalSpan *getGlobalSpan ( );

    // edit interval properties
    void editIntervalProperties ( char *newname );
    void editIntervalProperties ( int newVis );
    void editIntervalProperties ( float newOpaq );

    // manipulations of vtkIntervals within the collection
    void normalizeIntervalSpan ( float max, float min );
    void selectInterval ( );
    void deselectInterval ( );
    void toggleVisibility ( );
    void scaleIntervalAroundCenter ( float scaleAmount );
    void scaleIntervalFromStart ( float scaleAmount );
    void shiftInterval ( float shiftAmount );

 protected:
    char *name;
    int MRMLid;
    int IntervalID;
    int RefID;
    int order;
    int isSpelected;
    int visibility;
    float opacity;
    intervalType myIntervalType;
    static vtkIntervalSpan *globalSpan;
    vtkInterval *referenceInterval;
    vtkIntervalDropCollection *dropCollection;
    vtkIntervalSpan *mySpan;
    vtkIntervalHash *dropHash;               
    vtkTransform *myTransform;

    vtkInterval *next;
    vtkInterval *prev;
    
    vtkInterval ( );
    vtkInterval ( char *myname, int type );
    vtkInterval ( char *myname, int type, float max, float min );
    vtkInterval (char *myname, int type, float max, float min, vtkInterval *ref);
    virtual ~vtkInterval ( );

};



#endif

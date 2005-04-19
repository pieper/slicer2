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
#ifndef __vtkIntervalDropCollection_h
#define __vtkIntervalDropCollection_h

#include "vtkIntervalSpan.h"
#include "vtkIntervalDrop.h"
#include "vtkIntervalConfig.h"


//-------------------------------------------------------------------
// Description:
// A vtkIntervalDropCollection is a linked list of
// vtkIntervalDrops.
//-------------------------------------------------------------------
class VTK_EXPORT vtkIntervalDropCollection : public vtkObject{

 public:
    static vtkIntervalDropCollection *New();        
    void PrintSelf (ostream& os, vtkIndent indent );
    vtkTypeRevisionMacro (vtkIntervalDropCollection, vtkObject );

    vtkSetMacro (numDrops, int);
    vtkGetMacro (numDrops, int);
    vtkSetMacro (collectionID, int);
    vtkGetMacro (collectionID, int);
    vtkSetMacro ( dropSpacing, float );
    vtkGetMacro ( dropSpacing, float );

    // access functions
    vtkIntervalDrop *getDropHead ();
    vtkIntervalDrop *getDropTail ();
    void setDropHead (vtkIntervalDrop *d);
    void setDropTail (vtkIntervalDrop *d);
    vtkIntervalDrop *getReferenceDrop ( );
    void setReferenceDrop ( vtkIntervalDrop *ref);
    void shiftDrop ( vtkIntervalDrop *d, float shiftAmount );
    void shiftDrops ( float shiftAmount );
    // compute things about vtkIntervalDrops
    void computeDropSpacing ( );
    void computeDropIndices ( );

    // add and delete and manipulate vtkIntervalDrops in list.
    void addDrop ( vtkIntervalDrop *drop);
    void deleteDrop ( vtkIntervalDrop *drop);
    void deleteDrop (  int id );
    void deleteAllDrops ( );
    void insertDropBeforeDrop (vtkIntervalDrop *putThis, vtkIntervalDrop *beforeThis );
    void insertDropAfterDrop (vtkIntervalDrop *putThis, vtkIntervalDrop *afterThis );
    void addDropBeforeDrop (vtkIntervalDrop *putThis, vtkIntervalDrop *beforeThis );
    void addDropAfterDrop (vtkIntervalDrop *putThis, vtkIntervalDrop *beforeThis );
    void moveDropBeforeDrop ( vtkIntervalDrop *moveThis, vtkIntervalDrop *beforeThis );
    void moveDropAfterDrop ( vtkIntervalDrop *moveThis, vtkIntervalDrop *afterThis );
    void moveDropToHead ( vtkIntervalDrop *moveThis );
    void moveDropToTail ( vtkIntervalDrop *moveThis );

    int numDrops;
    float dropSpacing;
    int regularDropSpacingFlag;
    int collectionID;
    
    vtkIntervalDrop *drop;
    vtkIntervalDrop *drophead;
    vtkIntervalDrop *droptail;
    vtkIntervalDrop *activeDrop;
    vtkIntervalDrop *referenceDrop;
    int collectionType;
    vtkIntervalSpan *mySpan;

 protected:
    vtkIntervalDropCollection ( );
    vtkIntervalDropCollection ( vtkTransform& t);
    vtkIntervalDropCollection ( vtkIntervalDrop *ref );
    ~vtkIntervalDropCollection ( );
        
 private:
};

#endif

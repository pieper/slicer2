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
#ifndef __vtkIntervalCollection_h
#define __vtkIntervalCollection_h

#include "vtkInterval.h"


//-------------------------------------------------------------------
// Description:
// A vtkIntervalCollection is a linked list of
// vtkIntervals.
//-------------------------------------------------------------------
class VTK_EXPORT vtkIntervalCollection : public vtkObject{

 public:
    static vtkIntervalCollection *New();        
    void PrintSelf (ostream& os, vtkIndent indent );
    vtkTypeRevisionMacro (vtkIntervalCollection, vtkObject );

    vtkGetMacro (CollectionID, int);
    vtkSetMacro (CollectionID, int);

    vtkGetMacro ( numIntervals, int );
    vtkSetMacro ( numIntervals, int  );

    //The interval variable that indexes all.
    vtkSetMacro ( index, float );
    vtkGetMacro ( index, float );    

    //Find an interval in the collection
    vtkInterval *getIntervalByName ( char *ivalname );
    vtkInterval *getIntervalByID ( int intervalID );     

    //Get/Set head, tail, reference intervals
    vtkInterval *getIntervalHead ( );
    void setIntervalHead ( vtkInterval *h );
    vtkInterval *getIntervalTail ( );
    void setIntervalTail ( vtkInterval *t );
    vtkInterval *getReferenceInterval ( );
    void setReferenceInterval ( vtkInterval *ref );

    //Get/Set global span
    vtkIntervalSpan *getGlobalSpan ( );
    void setGlobalSpan ( float min, float max );
    void setGlobalSpan ( vtkIntervalSpan *sp );
    void updateGlobalSpan ( );

    //Add, delete, select, manipulate intervals
    void addInterval ( vtkInterval *addme , int type );
    void addInterval (vtkInterval *addme, char *name, int type);
    void addInterval (vtkInterval *addme, char *name, int type, float min, float max);
    vtkInterval *addInterval ( char *myname, int type, float max, float min);
    void deleteInterval ( vtkInterval *killme );
    void deleteAllIntervals ( );
    void selectInterval ( char *myname );
    void selectInterval ( vtkInterval *i );
    vtkInterval *getSelectedInterval ( );
    void deselectInterval (char *myname );
    void deselectInterval ( vtkInterval *i );
    void addNewIntervalBeforeInterval ( vtkInterval *addThis, vtkInterval *beforeThis );
    void addNewIntervalAfterInterval ( vtkInterval *addThis, vtkInterval *afterThis );
    void moveIntervalBeforeInterval ( vtkInterval *moveThis, vtkInterval *beforeThis );
    void moveIntervalAfterInterval ( vtkInterval *moveThis, vtkInterval *afterThis );
    void moveIntervalToHead ( vtkInterval *moveThis );
    void moveIntervalToTail ( vtkInterval *moveThis );
    void insertIntervalBeforeInterval (vtkInterval *addThis, vtkInterval *beforeThis );
    void insertIntervalAfterInterval (vtkInterval *addThis, vtkInterval *afterThis );
    void reOrderIntervals ( );
    void orderSortIntervals ( );
    void shiftInterval ( vtkInterval *iptr, float shiftAmount  );
    void shiftIntervalDrops ( vtkInterval *iptr, float shiftAmount );
    void shiftIntervalDrop ( vtkIntervalDrop *dptr, float shiftAmount );

    vtkInterval *interval;
    
    vtkInterval *intervaltail;
    vtkInterval *intervalhead;
    vtkInterval *selectedInterval;

    //interval to which others are registered
    vtkInterval *referenceInterval; 
    vtkIntervalSpan *globalSpan;
    int CollectionID;
    int numIntervals;
    // index that probes the collection
    float index;
    
 protected:
    vtkIntervalCollection ( );
    vtkIntervalCollection (int ID);
    vtkIntervalCollection (float min, float max);
    vtkIntervalCollection (float min, float max, int ID);
    ~vtkIntervalCollection ( );
        
 private:
};

#endif

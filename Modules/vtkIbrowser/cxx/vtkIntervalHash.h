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
#ifndef __vtkIntervalHash_h
#define __vtkIntervalHash_h

#include "vtkObject.h"
#include "vtkObjectFactory.h"
#include "vtkIntervalSpan.h"
#include "vtkIntervalHashNode.h"
#include "vtkIbrowserConfigure.h"

//-------------------------------------------------------------------
// Description:
// A vtkInterval stores an ordered collection of vtkIntervalDrops 
// in a hash table (vtkIntervalHash), and keyed by the start/stop
// endpoints of their vtkSpan. There are different subclasses of
// vtkInterval; and related vtkIntervals are stored together in a
// vtkIntervalCollection.
//-------------------------------------------------------------------
class VTK_EXPORT vtkIntervalHash : public vtkObject
{ 
 public:          
    static vtkIntervalHash *New ( );
    vtkTypeRevisionMacro ( vtkIntervalHash, vtkObject );
    void PrintSelf ( ostream& os, vtkIndent indent );
    void setKeyStart ( float start, vtkIntervalHashNode *n );
    void setKeyStop ( float stop, vtkIntervalHashNode *n);

    void insertNode ( );
    void deleteNode ( );
    void hashTrack ( float start, float stop);
    
 protected:
    int gotInterval;
    vtkIntervalHashNode *tail;
    vtkIntervalHashNode *ref;
    vtkIntervalHashNode *head;
    vtkIntervalHashNode *hashIndex;
    vtkIntervalHashNode *tableSearcher;

    vtkIntervalHash(); 
    ~vtkIntervalHash();

 private:
    //These are declared but not implemented to prevent
    //automatic definition by the compiler.
    vtkIntervalHash(const vtkIntervalHash&);  //Not implemented.
    void operator=(const vtkIntervalHash&);   //Not implemented.
};
#endif

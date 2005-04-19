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
#ifndef __vtkIntervalDrop_h
#define __vtkIntervalDrop_h

#include "vtkObject.h"
#include "vtkObjectFactory.h"
#include "vtkIntervalConfig.h"
#include <vtkIbrowserConfigure.h>
#include "vtkTransform.h"


//-------------------------------------------------------------------
// Description:
// A vtkIntervalDrop is an abstract container for data which
// populates a vtkInterval. The Drop is  meant to be subclassed
// into containers for different kinds of data, and methods that
// process that data.
//-------------------------------------------------------------------
class VTK_EXPORT vtkIntervalDrop : public vtkObject {
 public:
    static vtkIntervalDrop *New ();
    vtkTypeRevisionMacro ( vtkIntervalDrop, vtkObject ); 
    void PrintSelf ( ostream& os, vtkIndent indent );

    // vtkGet/Set macros
    vtkSetMacro(dropDuration, float);
    vtkGetMacro(dropDuration, float);
    vtkSetMacro (dropSustain, int );
    vtkGetMacro (dropSustain, int );
    vtkSetMacro(dropTimestep, float); //for simple testing.
    vtkGetMacro(dropTimestep, float); //for simple testing.
    vtkGetVectorMacro(dropDimensions, int, 3);
    vtkSetVectorMacro(dropDimensions, int, 3);
    vtkGetStringMacro(dropname);
    vtkSetStringMacro(dropname);
    vtkGetMacro(DropID, int);
    vtkSetMacro (DropID, int);
    vtkGetMacro(RefID, int);
    vtkSetMacro (RefID, int);
    vtkGetMacro(dropIndex, int);
    vtkSetMacro(dropIndex, int);
    vtkGetMacro(dropPosition, float);
    vtkSetMacro(dropPosition, float);    

    // access functions 
    void setReferenceDrop ( vtkIntervalDrop *ref );
    vtkIntervalDrop *getReferenceDrop ( );
    void setDropTransform ( vtkTransform *t );
    vtkTransform *getDropTransform ( );
    int getDropType ( );
    void setDropType ( int);
    // manipulate vtkIntervalDrops
    void shiftDrop ( float shiftAmount );
    vtkIntervalDrop *getNext( );
    vtkIntervalDrop *getPrev( );

    int dropType;
    vtkIntervalDrop *referenceDrop;
    vtkTransform *myTransform;
    vtkIntervalDrop *next;
    vtkIntervalDrop *prev;

 protected:    
    float dropPosition;
    int dropSustain;
    float dropDuration;
    float dropTimestep;
    char *dropname;
    int dropIndex;
    int DropID;
    int RefID;
    int dropDimensions[3]; //wid, hit, depth

    
    vtkIntervalDrop () ;
    vtkIntervalDrop ( char * );
    vtkIntervalDrop ( vtkTransform& t);
    virtual ~vtkIntervalDrop ();
   
 private:
};

#endif

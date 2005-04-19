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
#ifndef __vtkIntervalStudy_h
#define __vtkIntervalStudy_h

#include "vtkObject.h"
#include "vtkObjectFactory.h"
#include "vtkInterval.h"
#include "vtkIntervalDrop.h"
#include "vtkIntervalAnimator.h"
#include "vtkIntervalConfig.h"
#include "vtkIntervalSpan.h"
#include "vtkIntervalCollection.h"
#include "vtkIbrowserConfigure.h"



//-------------------------------------------------------------------
// Description:
// A vtkIntervalStudy contains a doubly-linked list of related
// intervals. The collection's GUI representation mediates the
// exploration of data contained within the collection.
// The collection can be named, edited, saved and reloaded.
// Class methods add, delete and manipulate the intervals
// represented within the collection.
//-------------------------------------------------------------------
class VTK_EXPORT vtkIntervalStudy : public vtkObject {
 public:
    static vtkIntervalStudy *New ();
    vtkTypeRevisionMacro(vtkIntervalStudy, vtkObject);
    void PrintSelf (ostream& os, vtkIndent indent);

    //interval collection name and id
    vtkSetStringMacro ( Name );
    vtkGetStringMacro ( Name );
    //Sets the resolution of index
    vtkSetMacro ( zoomfactor, int );
    vtkGetMacro ( zoomfactor, int );

    //Description:
    // Top-level operations for an entire vtkIntervalStudy
    void deleteIntervalStudy ( );
    void zoomInIntervalStudy ( int );
    void zoomOutIntervalStudy ( int );

 protected:
    // A linked list of intervals, each containing data
    vtkIntervalCollection *intervalCollection;
    // the tool that plays the study thru slicer's viewer
    vtkIntervalAnimator *player;
    char *Name;
    int zoomfactor;

    ~vtkIntervalStudy ();
    vtkIntervalStudy ();
    vtkIntervalStudy (int ID);
    vtkIntervalStudy ( float min, float max);
    vtkIntervalStudy (float min, float max, int ID);

 private:
    vtkIntervalStudy(const vtkIntervalStudy&); //Not implemented
    void operator=(const vtkIntervalStudy&);        //Not implemented
};

#endif

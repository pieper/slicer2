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
#ifndef __vtkIntervalSpan_h
#define __vtkIntervalSpan_h

#include "vtkObject.h"
#include "vtkObjectFactory.h"
#include "vtkIntervalDrop.h"
#include <vtkIbrowserConfigure.h>

//--------------------------------------------------------------------------------------
// Description:
// In the IntervalBrowser, most objects have a span.
// vtkIntervalCollections have a vtkIntervalSpan: this describes
// the global span of the study, includes the global start,
// stop and duration.
//--------------------------------------------------------------------------------------
class VTK_EXPORT vtkIntervalSpan : public vtkObject {
 public:
    static vtkIntervalSpan *New ();
    vtkTypeRevisionMacro ( vtkIntervalSpan, vtkObject );
    void PrintSelf ( ostream& os, vtkIndent indent );

    vtkIntervalSpan *getReferenceSpan( );
    vtkIntervalDrop *getFirstDropInSpan ( );
    vtkIntervalDrop *getLastDropInSpan ( );
    void setReferenceSpan ( vtkIntervalSpan *ref );
    void setFirstDropInSpan ( vtkIntervalDrop  *first );
    void setLastDropInSpan ( vtkIntervalDrop *last );
    
    void updateSpan ( );
    void updateSpan ( float min, float max );
    void setSpan ( float min, float max );
    void checkUpdateSpan ( float locheck, float hicheck );
    void shiftSpan ( float shiftAmount );
    void scaleSpanAroundCenter ( float scaleAmount );
    void scaleSpanFromStart( float scaleAmount );
    void normalizeSpan ( float max, float min );
    float getDistanceFromStart ( float index );
    float getDistanceFromStop ( float index );
    float getDistanceFromReferenceStart ( float index );
    float getDistanceFromReferenceStop (float index);

    vtkSetMacro ( unitStart, float ) ;
    vtkGetMacro ( unitStart, float );
    vtkSetMacro ( unitStop, float );
    vtkGetMacro ( unitStop, float );
    vtkSetMacro ( unitSpan, float );
    vtkGetMacro ( unitSpan, float );
    vtkSetMacro ( isReference, int );
    vtkGetMacro ( isReference, int );
    
 protected:
    float unitStart;
    float unitStop;
    float unitSpan;
    vtkIntervalDrop *firstDrop;
    vtkIntervalDrop *lastDrop;
    vtkIntervalSpan *referenceSpan;
    int isReference;

    vtkIntervalSpan ( );
    vtkIntervalSpan ( float min, float max );
    ~vtkIntervalSpan ( );
    
 private:
    vtkIntervalSpan(const vtkIntervalSpan&);
    void operator=(const vtkIntervalSpan&);


};

#endif

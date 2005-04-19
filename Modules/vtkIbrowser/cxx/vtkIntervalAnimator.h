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
#ifndef __vtkIntervalAnimator_h
#define __vtkIntervalAnimator_h

#include "vtkObject.h"
#include "vtkObjectFactory.h"
#include "vtkIbrowserConfigure.h"

#define ONCE 0
#define LOOP 1
#define PINGPONG 2
#define STEP 3
#define RANDOMLY_INDEX 4
#define PAUSE 5
#define STOP 6

#define FORWARD 1
#define REVERSE 2

//Description:
//A vtkIntervalAnimator consolidates the animation functions
//for the IntervalBrowser. Plays, records and saves animations,
//and keeps track of the state of the controller.

class VTK_EXPORT vtkIntervalAnimator : public vtkObject {
 public:
    vtkTypeRevisionMacro(vtkIntervalAnimator, vtkObject)
    void PrintSelf (ostream &os, vtkIndent indent);

    static vtkIntervalAnimator *New ();

    //Description:
    //These specify controller state
    static int animationMode;
    static int animationDirection;
    static int animationRunning;
    static int animationPaused;

    //Description:
    //These do the animation work
    //void animateSingleFrame ( int index);
    //void animateSingleFrame ( float index);
    //void playAnimation ();
    //void pauseAnimation ();
    //void stopAnimation ();

    //Description:
    //These are effective get&set operations
    int getAnimationMode ( );
    void setAnimationMode ( int );
    int getAnimationDirection ( );
    void setAnimationDirection ( int );
    int getAnimationRunning ( );
    void setAnimationRunning( int );
    int getAnimationPaused ( );
    void setAnimationPaused ( int );
    
 protected:
    ~vtkIntervalAnimator ();
    vtkIntervalAnimator ();

 private:
    vtkIntervalAnimator(const vtkIntervalAnimator&);   //Not implemented.
    void operator=(const vtkIntervalAnimator&);          //Not implemented.
    
};

#endif

#include "vtkIntervalAnimator.h"
#include "vtkIbrowserConfigure.h"
#include <stdio.h>


vtkCxxRevisionMacro(vtkIntervalAnimator, "$Revision: 1.3 $");

int vtkIntervalAnimator::animationPaused = 0;
int vtkIntervalAnimator::animationRunning = 0;
int vtkIntervalAnimator::animationMode = ONCE;
int vtkIntervalAnimator::animationDirection = FORWARD;


//--------------------------------------------------------------------------------------
vtkIntervalAnimator *vtkIntervalAnimator::New ( )
{
    vtkObject* ret = vtkObjectFactory::CreateInstance ( "vtkIntervalAnimator" );
    if ( ret ) {
        return ( vtkIntervalAnimator* ) ret;
    }
    return new vtkIntervalAnimator;
}



//--------------------------------------------------------------------------------------
vtkIntervalAnimator::vtkIntervalAnimator ( ) {

}



//--------------------------------------------------------------------------------------
vtkIntervalAnimator::~vtkIntervalAnimator ( ) {
    
}

//--------------------------------------------------------------------------------------
int vtkIntervalAnimator::getAnimationMode ( ) {

    return vtkIntervalAnimator::animationMode;
}

//--------------------------------------------------------------------------------------
void vtkIntervalAnimator::setAnimationMode ( int setting) {

    vtkIntervalAnimator::animationMode = setting;
}


//--------------------------------------------------------------------------------------
int vtkIntervalAnimator::getAnimationDirection ( ) {

    return vtkIntervalAnimator::animationDirection;
}

//--------------------------------------------------------------------------------------
void vtkIntervalAnimator::setAnimationDirection ( int setting) {

    vtkIntervalAnimator::animationDirection = setting;
}


//--------------------------------------------------------------------------------------
int vtkIntervalAnimator::getAnimationPaused ( ) {

    return vtkIntervalAnimator::animationPaused;
}

//--------------------------------------------------------------------------------------
void vtkIntervalAnimator::setAnimationPaused ( int setting) {

    vtkIntervalAnimator::animationPaused = setting;
}

//--------------------------------------------------------------------------------------
int vtkIntervalAnimator::getAnimationRunning ( ) {

    return vtkIntervalAnimator::animationRunning;
}

//--------------------------------------------------------------------------------------
void vtkIntervalAnimator::setAnimationRunning ( int setting) {

    vtkIntervalAnimator::animationRunning = setting;
}


//--------------------------------------------------------------------------------------
void vtkIntervalAnimator::PrintSelf(ostream& os, vtkIndent indent)
{
    vtkObject::PrintSelf(os, indent);
}


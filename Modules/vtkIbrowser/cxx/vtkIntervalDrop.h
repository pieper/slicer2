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

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

#include "vtkObjectFactory.h"
#include "vtkObject.h"
#include "vtkIntervalHash.h"


//--------------------------------------------------------------------------------------
vtkCxxRevisionMacro(vtkIntervalHash, "$Revision: 1.3 $");




// Description:
// This is a basic container for a Hash for intervals.
//--------------------------------------------------------------------------------------
vtkIntervalHash *vtkIntervalHash::New()
{
    vtkObject* ret = vtkObjectFactory::CreateInstance ("vtkIntervalHash" );
    if (ret) {
        return ( vtkIntervalHash *)ret;
    }
    return new vtkIntervalHash;
}


//--------------------------------------------------------------------------------------
vtkIntervalHash::vtkIntervalHash ( )
{
    this->gotInterval = 0;
    this->head = NULL;
    this->tail = NULL;
    this->ref = NULL;
    this->tableSearcher = NULL;
    this->hashIndex = vtkIntervalHashNode::New ();
}

//--------------------------------------------------------------------------------------
vtkIntervalHash::~vtkIntervalHash ( )
{
    this->hashIndex->Delete();
}


//--------------------------------------------------------------------------------------
void vtkIntervalHash::insertNode ( ) {
}

//--------------------------------------------------------------------------------------
void vtkIntervalHash::deleteNode ( ) {
}

//--------------------------------------------------------------------------------------
void vtkIntervalHash::hashTrack ( float start, float stop ) {
}


//--------------------------------------------------------------------------------------
void vtkIntervalHash::setKeyStop ( float stop, vtkIntervalHashNode *n )
{
 n->setKeyStart(stop); 
}


//--------------------------------------------------------------------------------------
void vtkIntervalHash::setKeyStart ( float start, vtkIntervalHashNode *n )
{
    n->setKeyStop(start);
}



//--------------------------------------------------------------------------------------
void vtkIntervalHash::PrintSelf(ostream &os, vtkIndent indent)
{
    vtkObject::PrintSelf(os, indent);
}



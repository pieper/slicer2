#include "vtkObjectFactory.h"
#include "vtkObject.h"
#include "vtkIntervalHashNode.h"


//--------------------------------------------------------------------------------------
vtkCxxRevisionMacro(vtkIntervalHashNode, "$Revision: 1.3 $");



//--------------------------------------------------------------------------------------
vtkIntervalHashNode *vtkIntervalHashNode::New ( )
{
    vtkObject* ret = vtkObjectFactory::CreateInstance ("vtkIntervalHashNode" );
    if (ret) {
        return ( vtkIntervalHashNode *)ret;
    }
    return new vtkIntervalHashNode;
}



//--------------------------------------------------------------------------------------
vtkIntervalHashNode::vtkIntervalHashNode ( )
{
    this->keyStart = 0.0;
    this->keyStop = 0.0;
    this->drop = NULL;
    this->next = NULL;
    this->prev = NULL;
}


//--------------------------------------------------------------------------------------
vtkIntervalHashNode::~vtkIntervalHashNode ( )
{
}


//--------------------------------------------------------------------------------------
void vtkIntervalHashNode::initNode ( vtkIntervalHashNode *n) {

    n->keyStart = 0.0;
    n->keyStop = 0.0;
    n->drop = NULL;
    n->next = NULL;
    n->prev = NULL;
}


//--------------------------------------------------------------------------------------
void vtkIntervalHashNode::setKeyStart ( float start ) {
    this->keyStart = start;
}

//--------------------------------------------------------------------------------------
void vtkIntervalHashNode::setKeyStop ( float stop ) {
    this->keyStop = stop;
}



//--------------------------------------------------------------------------------------
void vtkIntervalHashNode::PrintSelf(ostream &os, vtkIndent indent)
{
    vtkObject::PrintSelf(os, indent);
}


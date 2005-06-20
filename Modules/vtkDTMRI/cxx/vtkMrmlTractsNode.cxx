#include "vtkMrmlTractsNode.h"
#include "vtkObjectFactory.h"

//------------------------------------------------------------------------------
vtkMrmlTractsNode* vtkMrmlTractsNode::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMrmlTractsNode");
  if(ret)
  {
    return (vtkMrmlTractsNode*)ret;
  }
  // If the factory was unable to create the object, then create it here.
  return new vtkMrmlTractsNode;
}

//----------------------------------------------------------------------------
vtkMrmlTractsNode::vtkMrmlTractsNode()
{
  this->FileName = NULL;

}

//----------------------------------------------------------------------------
vtkMrmlTractsNode::~vtkMrmlTractsNode()
{
  
  if (this->FileName)
    {
      delete [] this->FileName;
      this->FileName = NULL;
    }
}

//----------------------------------------------------------------------------
void vtkMrmlTractsNode::Write(ofstream& of, int nIndent)
{
  vtkIndent i1(nIndent);

  // Write all attributes
  
  of << i1 << "<Tracts";

  if (this->FileName) 
    {
      of << " fileName='" << this->FileName << "'";
    }
  else
    {
      of << " fileName=''";
    }

  of << "></Tracts>\n";
}

//----------------------------------------------------------------------------
void vtkMrmlTractsNode::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkMrmlNode::PrintSelf(os,indent);

}


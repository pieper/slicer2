#include "vtkMrmlTractGroupNode.h"
#include "vtkObjectFactory.h"

//------------------------------------------------------------------------------
vtkMrmlTractGroupNode* vtkMrmlTractGroupNode::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMrmlTractGroupNode");
  if(ret)
  {
    return (vtkMrmlTractGroupNode*)ret;
  }
  // If the factory was unable to create the object, then create it here.
  return new vtkMrmlTractGroupNode;
}

//----------------------------------------------------------------------------
vtkMrmlTractGroupNode::vtkMrmlTractGroupNode()
{
  this->TractGroupID = 0;
  this->TractIDs = new std::list< int >;

}

//----------------------------------------------------------------------------
vtkMrmlTractGroupNode::~vtkMrmlTractGroupNode()
{
}

//----------------------------------------------------------------------------
void vtkMrmlTractGroupNode::Write(ofstream& of, int nIndent)
{
  vtkIndent i1(nIndent);

  // Write all attributes
  
  of << i1 << "<TractGroup";
  
  of << " TractGroupID='" << this->TractGroupID << "'";

  of << " TractIDs='";

  // Iterate through list and output each element.
  std::list<int>::const_iterator iter;
  iter = this->TractIDs->begin();
  // Output first element before loop so commas can come before elements.
  of << (*iter);
  iter++;
  while (iter != this->TractIDs->end())
  {
    of << ", " << (*iter);
    iter++;
  }

  of  <<  "'";

  of << "></TractGroup>\n";
}

//----------------------------------------------------------------------------
void vtkMrmlTractGroupNode::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkMrmlNode::PrintSelf(os,indent);

  os << indent << "TractGroupID: " << this->TractGroupID << "\n";

  os << indent << "TractIDs: " << indent;

  // Iterate through list and output each element.
  std::list<int>::const_iterator iter;
  iter = this->TractIDs->begin();
  // Output first element before loop so commas can come before elements.
  os << (*iter);
  iter++;
  while (iter != this->TractIDs->end())
  {
    os << ", " << (*iter);
    iter++;
  }

}


//----------------------------------------------------------------------------
void vtkMrmlTractGroupNode::AddTractToGroup (int tractID)
{
  this->TractIDs->push_back(tractID);

}

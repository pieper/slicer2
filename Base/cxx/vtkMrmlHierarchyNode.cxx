/*=auto=========================================================================

(c) Copyright 2002 Massachusetts Institute of Technology

Permission is hereby granted, without payment, to copy, modify, display 
and distribute this software and its documentation, if any, for any purpose, 
provided that the above copyright notice and the following three paragraphs 
appear on all copies of this software.  Use of this software constitutes 
acceptance of these terms and conditions.

IN NO EVENT SHALL MIT BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, 
INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE 
AND ITS DOCUMENTATION, EVEN IF MIT HAS BEEN ADVISED OF THE POSSIBILITY OF 
SUCH DAMAGE.

MIT SPECIFICALLY DISCLAIMS ANY EXPRESS OR IMPLIED WARRANTIES INCLUDING, 
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR 
A PARTICULAR PURPOSE, AND NON-INFRINGEMENT.

THE SOFTWARE IS PROVIDED "AS IS."  MIT HAS NO OBLIGATION TO PROVIDE 
MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

=========================================================================auto=*/
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include "vtkMrmlHierarchyNode.h"
#include "vtkObjectFactory.h"

//------------------------------------------------------------------------------
vtkMrmlHierarchyNode* vtkMrmlHierarchyNode::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMrmlHierarchyNode");
  if(ret)
  {
    return (vtkMrmlHierarchyNode*)ret;
  }
  // If the factory was unable to create the object, then create it here.
  return new vtkMrmlHierarchyNode;
}

//----------------------------------------------------------------------------
vtkMrmlHierarchyNode::vtkMrmlHierarchyNode()
{
  this->Indent = 1;
  
  // Strings
  this->HierarchyID = NULL;
  this->Type = NULL;
}

//----------------------------------------------------------------------------
vtkMrmlHierarchyNode::~vtkMrmlHierarchyNode()
{
  if (this->HierarchyID)
  {
    delete [] this->HierarchyID;
    this->HierarchyID = NULL;
  }
  if (this->Type)
  {
    delete [] this->Type;
    this->Type = NULL;
  }
}

//----------------------------------------------------------------------------
void vtkMrmlHierarchyNode::Write(ofstream& of, int nIndent)
{
  // Write all attributes not equal to their defaults
  
  vtkIndent i1(nIndent);

  of << i1 << "<Hierarchy";

  // Strings
  if (this->HierarchyID && strcmp(this->HierarchyID, ""))
  {
    of << " id='" << this->HierarchyID << "'";
  }
  if (this->Type && strcmp(this->Type, "")) 
  {
    of << " type='" << this->Type << "'";
  }
  
  of << ">\n";;
}

//----------------------------------------------------------------------------
// Copy the node's attributes to this object.
// Does NOT copy: ID, FilePrefix, Name
void vtkMrmlHierarchyNode::Copy(vtkMrmlNode *anode)
{
  vtkMrmlNode::MrmlNodeCopy(anode);
  vtkMrmlHierarchyNode *node = (vtkMrmlHierarchyNode *) anode;

  // Strings
  this->SetHierarchyID(node->HierarchyID);
  this->SetType(node->Type);
}

//----------------------------------------------------------------------------
void vtkMrmlHierarchyNode::PrintSelf(ostream& os, vtkIndent indent)
{
  int idx;
  
  vtkMrmlNode::PrintSelf(os,indent);

  os << indent << "HierarchyID: " << 
    (this->HierarchyID ? this->HierarchyID : "(node)") << "\n";
  os << indent << "Type: " <<
    (this->Type ? this->Type : "(none)") << "\n";  
}

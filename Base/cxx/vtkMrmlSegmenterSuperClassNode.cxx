/*=auto=========================================================================

(c) Copyright 2001 Massachusetts Institute of Technology

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
#include "vtkMrmlSegmenterSuperClassNode.h"
#include "vtkObjectFactory.h"

//------------------------------------------------------------------------------
vtkMrmlSegmenterSuperClassNode* vtkMrmlSegmenterSuperClassNode::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMrmlSegmenterSuperClassNode");
  if(ret)
  {
    return (vtkMrmlSegmenterSuperClassNode*)ret;
  }
  // If the factory was unable to create the object, then create it here.
  return new vtkMrmlSegmenterSuperClassNode;
}

//----------------------------------------------------------------------------
vtkMrmlSegmenterSuperClassNode::vtkMrmlSegmenterSuperClassNode() { 
  // vtkMrmlNode's attributes => Tabs following sub classes  
  this->Indent     = 1;
  this->NumClasses = 0;
  this->Prob       = 0.0;
}

//----------------------------------------------------------------------------
vtkMrmlSegmenterSuperClassNode::~vtkMrmlSegmenterSuperClassNode() { }

//----------------------------------------------------------------------------
void vtkMrmlSegmenterSuperClassNode::Write(ofstream& of, int nIndent)
{
  // Write all attributes not equal to their defaults
  
  vtkIndent i1(nIndent);
  of << i1 << "<SegmenterSuperClass";
  if (this->Name && strcmp(this->Name, "")) of << " name ='" << this->Name << "'";
  of << " NumClasses ='" << this->NumClasses << "'";
  of << " Prob='" << this->Prob << "'";
  of << ">\n";
}

//----------------------------------------------------------------------------
// Copy the node's attributes to this object.
// Does NOT copy: ID, Name
void vtkMrmlSegmenterSuperClassNode::Copy(vtkMrmlNode *anode)
{
  vtkMrmlNode::MrmlNodeCopy(anode);
  vtkMrmlSegmenterSuperClassNode *node = (vtkMrmlSegmenterSuperClassNode *) anode;
  this->NumClasses = node->NumClasses;
  this->Prob = node->Prob;
}

//----------------------------------------------------------------------------
void vtkMrmlSegmenterSuperClassNode::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkMrmlNode::PrintSelf(os,indent);
  os << indent << "Name: " << (this->Name ? this->Name : "(none)") << "\n";
  os << indent << "NumClasses: "                << this->NumClasses      <<  "\n"; 
  os << indent << "Prob: " << this->Prob << "\n"; 
}



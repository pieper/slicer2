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
#include "vtkMrmlOptionsNode.h"
#include "vtkObjectFactory.h"

//------------------------------------------------------------------------------
vtkMrmlOptionsNode* vtkMrmlOptionsNode::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMrmlOptionsNode");
  if(ret)
  {
    return (vtkMrmlOptionsNode*)ret;
  }
  // If the factory was unable to create the object, then create it here.
  return new vtkMrmlOptionsNode;
}

//----------------------------------------------------------------------------
vtkMrmlOptionsNode::vtkMrmlOptionsNode()
{
  // Strings
  this->Program = NULL;
  this->Contents = NULL;
}

//----------------------------------------------------------------------------
vtkMrmlOptionsNode::~vtkMrmlOptionsNode()
{
  if (this->Program)
  {
    delete [] this->Program;
    this->Program = NULL;
  }
  if (this->Contents)
  {
    delete [] this->Contents;
    this->Contents = NULL;
  }
  if (this->Options)
  {
    delete [] this->Options;
    this->Options = NULL;
  }
}

//----------------------------------------------------------------------------
void vtkMrmlOptionsNode::Write(ofstream& of, int nIndent)
{
  vtkIndent i1(nIndent);

  // Write all attributes not equal to their defaults
  
  of << i1 << "<Options";
  
  // Strings
  if (this->Program && strcmp(this->Program, "")) 
  {
    of << " program='" << this->Program << "'";
  }
  if (this->Contents && strcmp(this->Contents, "")) 
  {
    of << " contents='" << this->Contents << "'";
  }

  of << ">\n";

  if (this->Options && strcmp(this->Options, "")) 
  {
    of << this->Options;
  }

  of << "</Options>\n";
}

//----------------------------------------------------------------------------
// Copy the node's attributes to this object.
// Does NOT copy: ID, Name
void vtkMrmlOptionsNode::Copy(vtkMrmlNode *anode)
{
  vtkMrmlNode::MrmlNodeCopy(anode);
  vtkMrmlOptionsNode *node = (vtkMrmlOptionsNode *) anode;

  // Strings
  this->SetOptions(node->Options);
}


//----------------------------------------------------------------------------
void vtkMrmlOptionsNode::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkObject::PrintSelf(os,indent);

  os << indent << "Name: " <<
    (this->Options ? this->Options : "(none)") << "\n";
}

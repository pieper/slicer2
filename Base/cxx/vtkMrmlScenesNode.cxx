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
#include "vtkMrmlScenesNode.h"
#include "vtkObjectFactory.h"

//------------------------------------------------------------------------------
vtkMrmlScenesNode* vtkMrmlScenesNode::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMrmlScenesNode");
  if(ret)
  {
    return (vtkMrmlScenesNode*)ret;
  }
  // If the factory was unable to create the object, then create it here.
  return new vtkMrmlScenesNode;
}

//----------------------------------------------------------------------------
vtkMrmlScenesNode::vtkMrmlScenesNode()
{
  this->Indent = 1;
  
  // Strings
  this->Lang = NULL;
}

//----------------------------------------------------------------------------
vtkMrmlScenesNode::~vtkMrmlScenesNode()
{
  if (this->Lang)
  {
    delete [] this->Lang;
    this->Lang = NULL;
  }
}

//----------------------------------------------------------------------------
void vtkMrmlScenesNode::Write(ofstream& of, int nIndent)
{
  // Write all attributes not equal to their defaults
  
  vtkIndent i1(nIndent);

  of << i1 << "<Scenes";

  // Strings
  if (this->Lang && strcmp(this->Lang, ""))
  {
    of << " lang='" << this->Lang << "'";
  }
  if (this->Name && strcmp(this->Name, ""))
  {
    of << " name='" << this->Name << "'";
  }
  if (this->Description && strcmp(this->Description, "")) 
  {
    of << " description='" << this->Description << "'";
  }
  
  of << ">\n";;
}

//----------------------------------------------------------------------------
// Copy the node's attributes to this object.
// Does NOT copy: ID, FilePrefix, Name
void vtkMrmlScenesNode::Copy(vtkMrmlNode *anode)
{
  vtkMrmlNode::MrmlNodeCopy(anode);
  vtkMrmlScenesNode *node = (vtkMrmlScenesNode *) anode;

  // Strings
  this->SetDescription(node->Description);
  this->SetLang(node->Lang);
}

//----------------------------------------------------------------------------
void vtkMrmlScenesNode::PrintSelf(ostream& os, vtkIndent indent)
{
  int idx;
  
  vtkMrmlNode::PrintSelf(os,indent);

  os << indent << "Name: " << 
    (this->Name ? this->Name : "(none)") << "\n";
  os << indent << "Description: " <<
    (this->Description ? this->Description : "(none)") << "\n";  
  os << indent << "Lang: " <<
    (this->Lang ? this->Lang : "(none)") << "\n";
}

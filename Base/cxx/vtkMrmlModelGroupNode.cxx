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
#include "vtkMrmlModelGroupNode.h"
#include "vtkObjectFactory.h"

//------------------------------------------------------------------------------
vtkMrmlModelGroupNode* vtkMrmlModelGroupNode::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMrmlModelGroupNode");
  if(ret)
  {
    return (vtkMrmlModelGroupNode*)ret;
  }
  // If the factory was unable to create the object, then create it here.
  return new vtkMrmlModelGroupNode;
}

//----------------------------------------------------------------------------
vtkMrmlModelGroupNode::vtkMrmlModelGroupNode()
{
  this->Indent = 1;
  
  // Strings
  this->ModelGroupID = NULL;
  this->Color = NULL;
  
  // Numbers
  this->Opacity = 1.0;
  this->Visibility = 1;
  this->Expansion = 1;
}

//----------------------------------------------------------------------------
vtkMrmlModelGroupNode::~vtkMrmlModelGroupNode()
{
  if (this->ModelGroupID)
  {
    delete [] this->ModelGroupID;
    this->ModelGroupID = NULL;
  }
  if (this->Color)
  {
    delete [] this->Color;
    this->Color = NULL;
  }
}

//----------------------------------------------------------------------------
void vtkMrmlModelGroupNode::Write(ofstream& of, int nIndent)
{
  // Write all attributes not equal to their defaults
  
  vtkIndent i1(nIndent);

  of << i1 << "<ModelGroup";

  // Strings
  if (this->ModelGroupID && strcmp(this->ModelGroupID,""))
  {
    of << " id='" << this->ModelGroupID << "'";
  }
  if (this->Name && strcmp(this->Name, "")) 
  {
    of << " name='" << this->Name << "'";
  }
  if (this->Color && strcmp(this->Color, ""))
  {
    of << " color='" << this->Color << "'";
  }
  
  //Numbers
  if (this->Opacity != 1.0)
  {
    of << " opacity='" << this->Opacity << "'";
  }
  if (this->Visibility != 1)
  {
    of << " visibility='" << (this->Visibility ? "true":"false") << "'";
  }
  
  of << ">\n";;
}

//----------------------------------------------------------------------------
// Copy the node's attributes to this object.
// Does NOT copy: ID, FilePrefix, Name
void vtkMrmlModelGroupNode::Copy(vtkMrmlNode *anode)
{
  vtkMrmlNode::MrmlNodeCopy(anode);
  vtkMrmlModelGroupNode *node = (vtkMrmlModelGroupNode *) anode;

  // Strings
  this->SetModelGroupID(node->ModelGroupID);
  this->SetColor(node->Color);
  
  // Numbers
  this->SetOpacity(node->Opacity);
  this->SetVisibility(node->Visibility);
  this->SetExpansion(node->Expansion);
  
}

//----------------------------------------------------------------------------
void vtkMrmlModelGroupNode::PrintSelf(ostream& os, vtkIndent indent)
{
  int idx;
  
  vtkMrmlNode::PrintSelf(os,indent);

  os << indent << "ModelGroupID: " <<
    (this->ModelGroupID ? this->ModelGroupID : "(none)") << "\n";
  os << indent << "Name: " <<
    (this->Name ? this->Name : "(none)") << "\n";  
  os << indent << "Color: " <<
    (this->Color ? this->Color : "(none)") << "\n";
    
  os << indent << "Opacity:    " << this->Opacity << "\n";
  os << indent << "Visibility: " << this->Visibility << "\n";
}

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
#include "vtkMrmlLocatorNode.h"
#include "vtkObjectFactory.h"

//------------------------------------------------------------------------------
vtkMrmlLocatorNode* vtkMrmlLocatorNode::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMrmlLocatorNode");
  if(ret)
  {
    return (vtkMrmlLocatorNode*)ret;
  }
  // If the factory was unable to create the object, then create it here.
  return new vtkMrmlLocatorNode;
}

//----------------------------------------------------------------------------
vtkMrmlLocatorNode::vtkMrmlLocatorNode()
{
  // Strings
  this->Driver = NULL;
  this->DiffuseColor = NULL;
  
  // Numbers
  this->Visibility = 0;
  this->TransverseVisibility = 1;
  this->NormalLen = 100;
  this->TransverseLen = 25;
  this->Radius = 3.0;
}

//----------------------------------------------------------------------------
vtkMrmlLocatorNode::~vtkMrmlLocatorNode()
{
  if (this->Driver)
  {
    delete [] this->Driver;
    this->Driver = NULL;
  }
  if (this->DiffuseColor)
  {
    delete [] this->DiffuseColor;
    this->DiffuseColor = NULL;
  }
}

//----------------------------------------------------------------------------
void vtkMrmlLocatorNode::Write(ofstream& of, int nIndent)
{
  // Write all attributes not equal to their defaults
  
  vtkIndent i1(nIndent);

  of << i1 << "<Locator";

  // Strings
  if (this->Driver && strcmp(this->Driver,""))
  {
    of << " driver='" << this->Driver << "'";
  }
  if (this->DiffuseColor && strcmp(this->DiffuseColor, "")) 
  {
    of << " diffuseColor='" << this->DiffuseColor << "'";
  }
  
  // Booleans and Numbers
  if (this->Visibility != 0)
  {
    of << " visibility='" << (this->Visibility ? "true":"false") << "'";
  }
  if (this->TransverseVisibility != 1)
  {
    of << " transverseVisibility='" << (this->TransverseVisibility ? "true":"false") << "'";
  }
  if (this->NormalLen != 100)
  {
    of << " normalLen='" << this->NormalLen << "'";
  }
  if (this->TransverseLen != 25)
  {
    of << " transverseLen='" << this->TransverseLen << "'";
  }
  if (this->Radius != 3.0)
  {
    of << " radius='" << this->Radius << "'";
  }
  
  of << "></Locator>\n";;
}

//----------------------------------------------------------------------------
// Copy the node's attributes to this object.
void vtkMrmlLocatorNode::Copy(vtkMrmlNode *anode)
{
  vtkMrmlNode::MrmlNodeCopy(anode);
  vtkMrmlLocatorNode *node = (vtkMrmlLocatorNode *) anode;

  // Strings
  this->SetDriver(node->Driver);
  this->SetDiffuseColor(node->DiffuseColor);
  
  //Numbers
  this->SetVisibility(node->Visibility);
  this->SetTransverseVisibility(node->TransverseVisibility);
  this->SetNormalLen(node->NormalLen);
  this->SetTransverseLen(node->TransverseLen);
  this->SetRadius(node->Radius);
}

//----------------------------------------------------------------------------
void vtkMrmlLocatorNode::PrintSelf(ostream& os, vtkIndent indent)
{
  
  vtkMrmlNode::PrintSelf(os,indent);

  os << indent << "Driver: " <<
    (this->Driver ? this->Driver : "(none)") << "\n";
  os << indent << "DiffuseColor: " <<
    (this->DiffuseColor ? this->DiffuseColor : "(none)") << "\n";
  os << indent << "Visibility: " << this->Visibility << "\n";
  os << indent << "TransverseVisibility: " << this->TransverseVisibility << "\n";
  os << indent << "NormalLen: " << this->NormalLen << "\n";
  os << indent << "TransverseLen: " << this->TransverseLen << "\n";
  os << indent << "Radius: " << this->Radius << "\n";
}

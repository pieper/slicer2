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
#include "vtkMrmlMatrixNode.h"
#include "vtkObjectFactory.h"

//------------------------------------------------------------------------------
vtkMrmlMatrixNode* vtkMrmlMatrixNode::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMrmlMatrixNode");
  if(ret)
  {
    return (vtkMrmlMatrixNode*)ret;
  }
  // If the factory was unable to create the object, then create it here.
  return new vtkMrmlMatrixNode;
}

//----------------------------------------------------------------------------
vtkMrmlMatrixNode::vtkMrmlMatrixNode()
{
  this->Transform = vtkTransform::New();
}

//----------------------------------------------------------------------------
vtkMrmlMatrixNode::~vtkMrmlMatrixNode()
{
  this->Transform->Delete();
}

//----------------------------------------------------------------------------
void vtkMrmlMatrixNode::Write(ofstream& of, int nIndent)
{
  // Write all attributes not equal to their defaults
  
  vtkIndent i1(nIndent);

  of << i1 << "<Matrix";

  // Strings
  if (this->Name && strcmp(this->Name, "")) 
  {
    of << " name='" << this->Name << "'";
  }
  if (this->Description && strcmp(this->Description, "")) 
  {
    of << " description='" << this->Description << "'";
  }

  // Matrix
  of << " matrix='" << 
    GetMatrixToString(this->Transform->GetMatrix()) << "'";

  of << "></Matrix>\n";;
}

//----------------------------------------------------------------------------
// Copy the node's attributes to this object.
// Does NOT copy: ID, FilePrefix, Name
void vtkMrmlMatrixNode::Copy(vtkMrmlNode *anode)
{
  vtkMrmlNode::MrmlNodeCopy(anode);
  vtkMrmlMatrixNode *node = (vtkMrmlMatrixNode *) anode;

  this->Transform->DeepCopy(node->Transform);
}

//----------------------------------------------------------------------------
void vtkMrmlMatrixNode::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkMrmlNode::PrintSelf(os,indent);

  os << indent << "Name: " <<
    (this->Name ? this->Name : "(none)") << "\n";

  // Transform
  os << indent << "Transform:\n";
    this->Transform->PrintSelf(os, indent.GetNextIndent());  
}



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
#include "vtkMrmlSegmenterCIMNode.h"
#include "vtkObjectFactory.h"

//------------------------------------------------------------------------------
vtkMrmlSegmenterCIMNode* vtkMrmlSegmenterCIMNode::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMrmlSegmenterCIMNode");
  if(ret)
  {
    return (vtkMrmlSegmenterCIMNode*)ret;
  }
  // If the factory was unable to create the object, then create it here.
  return new vtkMrmlSegmenterCIMNode;
}

//----------------------------------------------------------------------------
vtkMrmlSegmenterCIMNode::vtkMrmlSegmenterCIMNode()
{
  this->CIMMatrix = NULL; 
}

//----------------------------------------------------------------------------
vtkMrmlSegmenterCIMNode::~vtkMrmlSegmenterCIMNode()
{
  if (this->CIMMatrix)
  {
    delete [] this->CIMMatrix;
    this->CIMMatrix = NULL;
  }
}

//----------------------------------------------------------------------------
void vtkMrmlSegmenterCIMNode::Write(ofstream& of, int nIndent)
{
  // Write all attributes not equal to their defaults
  
  vtkIndent i1(nIndent);

  of << i1 << "<SegmenterCIM";
  if (this->Name && strcmp(this->Name, "")) 
  {
    of << " name ='" << this->Name << "'";
  }
  if (this->CIMMatrix && strcmp(this->CIMMatrix, "")) 
  {
    of << " CIMMatrix='" << this->CIMMatrix << "'";
  }
  of << "></SegmenterCIM>\n";;
}

//----------------------------------------------------------------------------
// Copy the node's attributes to this object.
// Does NOT copy: ID, Name
void vtkMrmlSegmenterCIMNode::Copy(vtkMrmlNode *anode)
{
  vtkMrmlNode::MrmlNodeCopy(anode);
  vtkMrmlSegmenterCIMNode *node = (vtkMrmlSegmenterCIMNode *) anode;

  this->SetCIMMatrix(node->CIMMatrix); 
}

//----------------------------------------------------------------------------
void vtkMrmlSegmenterCIMNode::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkMrmlNode::PrintSelf(os,indent);
   os << indent << "Name: " <<
    (this->Name ? this->Name : "(none)") << "\n";
   os << indent << "CIMMatrix: " <<
    (this->CIMMatrix ? this->CIMMatrix : "(none)") << "\n";
   os << ")\n";
}



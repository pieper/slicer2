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
#include "vtkMrmlWindowLevelNode.h"
#include "vtkObjectFactory.h"

//------------------------------------------------------------------------------
vtkMrmlWindowLevelNode* vtkMrmlWindowLevelNode::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMrmlWindowLevelNode");
  if(ret)
  {
    return (vtkMrmlWindowLevelNode*)ret;
  }
  // If the factory was unable to create the object, then create it here.
  return new vtkMrmlWindowLevelNode;
}

//----------------------------------------------------------------------------
vtkMrmlWindowLevelNode::vtkMrmlWindowLevelNode()
{
  // Numbers
  this->AutoWindowLevel = 1;
  this->Window = 256;
  this->Level = 128;
  this->ApplyThreshold = 0;
  this->AutoThreshold = 0;
  this->LowerThreshold = -32768;
  this->UpperThreshold = 32767;
}

//----------------------------------------------------------------------------
vtkMrmlWindowLevelNode::~vtkMrmlWindowLevelNode()
{
}

//----------------------------------------------------------------------------
void vtkMrmlWindowLevelNode::Write(ofstream& of, int nIndent)
{
  // Write all attributes not equal to their defaults
  
  vtkIndent i1(nIndent);

  of << i1 << "<WindowLevel";
  
  // Numbers
  if (this->AutoWindowLevel != 1)
  {
    of << " autoWindowLevel='" << (this->AutoWindowLevel ? "true":"false") << "'";
  }
  if (this->Window != 256)
  {
    of << " window='" << this->Window << "'";
  }
  if (this->Level != 128)
  {
    of << " level='" << this->Level << "'";
  }
  if (this->ApplyThreshold != 0)
  {
    of << " applyThreshold='" << (this->ApplyThreshold ? "true":"false") << "'";
  }
  if (this->AutoThreshold != 0)
  {
    of << " autoThreshold='" << (this->AutoThreshold ? "true":"false") << "'";
  }
  if (this->LowerThreshold != -32768)
  {
    of << " lowerThreshold='" << this->LowerThreshold << "'";
  }
  if (this->UpperThreshold != 32767)
  {
    of << " upperThreshold='" << this->UpperThreshold << "'";
  }
  
  of << "></WindowLevel>\n";
}

//----------------------------------------------------------------------------
// Copy the node's attributes to this object.
void vtkMrmlWindowLevelNode::Copy(vtkMrmlNode *anode)
{
  vtkMrmlNode::MrmlNodeCopy(anode);
  vtkMrmlWindowLevelNode *node = (vtkMrmlWindowLevelNode *) anode;

  // Numbers
  this->SetWindow(node->Window);
  this->SetLevel(node->Level);
  this->SetLowerThreshold(node->LowerThreshold);
  this->SetUpperThreshold(node->UpperThreshold);
  this->SetAutoWindowLevel(node->AutoWindowLevel);
  this->SetApplyThreshold(node->ApplyThreshold);
  this->SetAutoThreshold(node->AutoThreshold);
}

//----------------------------------------------------------------------------
void vtkMrmlWindowLevelNode::PrintSelf(ostream& os, vtkIndent indent)
{
  
  vtkMrmlNode::PrintSelf(os,indent);

  os << indent << "AutoWindowLevel: " << this->AutoWindowLevel << "\n";
  os << indent << "Window: " << this->Window << "\n";
  os << indent << "Level: " << this->Level << "\n";
  os << indent << "ApplyThreshold: " << this->ApplyThreshold << "\n";
  os << indent << "AutoThreshold: " << this->AutoThreshold << "\n";
  os << indent << "Lower threshold: " << this->LowerThreshold << "\n";
  os << indent << "Upper threshold: " << this->UpperThreshold << "\n";
}

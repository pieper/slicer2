/*=auto=========================================================================

(c) Copyright 2003 Massachusetts Institute of Technology (MIT) All Rights Reserved.

This software ("3D Slicer") is provided by The Brigham and Women's 
Hospital, Inc. on behalf of the copyright holders and contributors.
Permission is hereby granted, without payment, to copy, modify, display 
and distribute this software and its documentation, if any, for internal 
research purposes only, provided that (1) the above copyright notice and 
the following four paragraphs appear on all copies of this software, and 
(2) that source code to any modifications to this software be made 
publicly available under terms no more restrictive than those in this 
License Agreement. Use of this software constitutes acceptance of these 
terms and conditions.

3D Slicer Software has not been reviewed or approved by the Food and 
Drug Administration, and is for non-clinical, IRB-approved Research Use 
Only.  In no event shall data or images generated through the use of 3D 
Slicer Software be used in the provision of patient care.

IN NO EVENT SHALL THE COPYRIGHT HOLDERS AND CONTRIBUTORS BE LIABLE TO 
ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL 
DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, 
EVEN IF THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE BEEN ADVISED OF THE 
POSSIBILITY OF SUCH DAMAGE.

THE COPYRIGHT HOLDERS AND CONTRIBUTORS SPECIFICALLY DISCLAIM ANY EXPRESS 
OR IMPLIED WARRANTIES INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND 
NON-INFRINGEMENT.

THE SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS 
IS." THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE NO OBLIGATION TO 
PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.


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

/*=auto=========================================================================

(c) Copyright 2003 Massachusetts Institute of Technology (MIT) All Rights Reserved.

This software ("3D Slicer") is provided by The Brigham and Women's 
Hospital, Inc. on behalf of the copyright holders and contributors.
Permission is hereby granted, without payment, to copy, modify, display 
and distribute this software and its documentation, if any, for  
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
  this->LocalPriorWeight = 1.0;
  this->InputChannelWeights = NULL;

  this->PrintFrequency      = 0;
  this->PrintBias           = 0;
  this->PrintLabelMap       = 0;
  this->PrintWeights        = 0;
}

//----------------------------------------------------------------------------
vtkMrmlSegmenterSuperClassNode::~vtkMrmlSegmenterSuperClassNode() { 
  if (this->InputChannelWeights) {
    delete [] this->InputChannelWeights;
    this->InputChannelWeights = NULL;
  }
}

//----------------------------------------------------------------------------
void vtkMrmlSegmenterSuperClassNode::Write(ofstream& of, int nIndent)
{
  // Write all attributes not equal to their defaults
  
  vtkIndent i1(nIndent);
  of << i1 << "<SegmenterSuperClass";
  if (this->Name && strcmp(this->Name, "")) of << " name ='" << this->Name << "'";
  of << " NumClasses ='" << this->NumClasses << "'";
  of << " Prob='" << this->Prob << "'";

  if (this->InputChannelWeights && strcmp(this->InputChannelWeights, "")) 
  {
    of << " InputChannelWeights='" << this->InputChannelWeights << "'";
  }
  of << " LocalPriorWeight='" << this->LocalPriorWeight << "'";

  of << " PrintWeights='" << this->PrintWeights << "'";
  of << " PrintBias='" << this->PrintBias << "'";
  of << " PrintLabelMap='" << this->PrintLabelMap << "'";
  of << " PrintFrequency='" << this->PrintFrequency << "'";

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
  this->SetInputChannelWeights(node->InputChannelWeights);
  this->SetLocalPriorWeight(node->LocalPriorWeight);
  this->PrintWeights   = node->PrintWeights;
  this->PrintBias      = node->PrintBias;
  this->PrintLabelMap  = node->PrintLabelMap;
  this->PrintFrequency = node->PrintFrequency;

}

//----------------------------------------------------------------------------
void vtkMrmlSegmenterSuperClassNode::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkMrmlNode::PrintSelf(os,indent);
  os << indent << "Name: " << (this->Name ? this->Name : "(none)") << "\n";
  os << indent << "NumClasses: "                << this->NumClasses      <<  "\n"; 
  os << indent << "Prob: " << this->Prob << "\n"; 
  os << indent << "InputChannelWeights: " <<
    (this->InputChannelWeights ? this->InputChannelWeights : "(none)") << "\n";

  os << indent << "LocalPriorWeight: " << this->LocalPriorWeight << "\n";

  os << indent << " PrintWeights:   " << this->PrintWeights << "\n";
  os << indent << " PrintBias:      " << this->PrintBias << "\n";
  os << indent << " PrintLabelMap:  " << this->PrintLabelMap << "\n";
  os << indent << " PrintFrequency: " << this->PrintFrequency << "\n";
}



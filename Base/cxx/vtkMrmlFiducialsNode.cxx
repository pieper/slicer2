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
#include "vtkMrmlFiducialsNode.h"
#include "vtkObjectFactory.h"

//------------------------------------------------------------------------------
vtkMrmlFiducialsNode* vtkMrmlFiducialsNode::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMrmlFiducialsNode");
  if(ret)
  {
    return (vtkMrmlFiducialsNode*)ret;
  }
  // If the factory was unable to create the object, then create it here.
  return new vtkMrmlFiducialsNode;
}

//----------------------------------------------------------------------------
vtkMrmlFiducialsNode::vtkMrmlFiducialsNode()
{
  // vtkMrmlNode's attributes
  this->Indent = 1;
  this->SymbolSize = 6.0;
  this->TextSize = 4.5;
  this->Visibility = 1;
  this->Color[0]=0.4; this->Color[1]=1.0; this->Color[2]=1.0;
  this->Type = NULL;
  this->SetType("default");
}

//----------------------------------------------------------------------------
vtkMrmlFiducialsNode::~vtkMrmlFiducialsNode()
{
  if (this->Type) {
    delete [] this->Type;
    this->Type = NULL;
  }

}

//----------------------------------------------------------------------------
void vtkMrmlFiducialsNode::Write(ofstream& of, int nIndent)
{
  // Write all attributes not equal to their defaults
  
  vtkIndent i1(nIndent);

  of << i1 << "<Fiducials";
  // Strings
  if (this->Name && strcmp(this->Name, "")) 
    {
      of << " name='" << this->Name << "'";
    }
  if (this->Description && strcmp(this->Description, "")) 
    {
      of << " description='" << this->Description << "'";
    }
     of << " type='" << this->Type << "'";
     of << " symbolSize='" << this->SymbolSize << "'";
     of << " textSize='" << this->TextSize << "'";
     of << " visibility='" << this->Visibility << "'";
     of << " color='" << this->Color[0] << " " << this->Color[1] << " " <<
                    this->Color[2] << "'";
  
   of << ">\n";
}

//----------------------------------------------------------------------------
// Copy the node's attributes to this object.
// Does NOT copy: ID, FilePrefix, Name
void vtkMrmlFiducialsNode::Copy(vtkMrmlNode *anode)
{
  vtkMrmlNode::MrmlNodeCopy(anode);
  vtkMrmlFiducialsNode *node = (vtkMrmlFiducialsNode *) anode;

  this->SymbolSize = node->SymbolSize;
  this->TextSize = node->TextSize;
  this->Visibility = node->Visibility;
  this->Type = node->Type;
}

//----------------------------------------------------------------------------
void vtkMrmlFiducialsNode::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkMrmlNode::PrintSelf(os,indent);
    os << indent << "Name: " <<
    (this->Name ? this->Name : "(none)") << "\n";
  
  os << indent << "Type: ";
  os << indent << this->Type << " \n ";
  
  os << indent << "Symbol size: (";
  os << indent << this->SymbolSize << ") \n ";

  os << indent << "Text size: (";
  os << indent << this->TextSize << ") \n ";

}

void vtkMrmlFiducialsNode::SetTypeToEndoscopic() {

  this->SetType("endoscopic");

}

void vtkMrmlFiducialsNode::SetTypeToMeasurement(){

  this->SetType("measurement");

}

void vtkMrmlFiducialsNode::SetTypeToDefault(){

  this->SetType("default");

}



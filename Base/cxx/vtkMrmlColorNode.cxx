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
#include "vtkMrmlColorNode.h"
#include "vtkObjectFactory.h"

//------------------------------------------------------------------------------
vtkMrmlColorNode* vtkMrmlColorNode::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMrmlColorNode");
  if(ret)
  {
    return (vtkMrmlColorNode*)ret;
  }
  // If the factory was unable to create the object, then create it here.
  return new vtkMrmlColorNode;
}

//----------------------------------------------------------------------------
vtkMrmlColorNode::vtkMrmlColorNode()
{
  // Strings
  this->Labels = NULL;

  // Numbers
  this->Ambient = 0.0;
  this->Diffuse = 1.0;
  this->Specular = 0.0;
  this->Power = 1;
  
  // Arrays
  this->DiffuseColor[0] = 1.0;
  this->DiffuseColor[1] = 1.0;
  this->DiffuseColor[2] = 1.0;
}

//----------------------------------------------------------------------------
vtkMrmlColorNode::~vtkMrmlColorNode()
{
  if (this->Labels)
  {
    delete [] this->Labels;
    this->Labels = NULL;
  }
}

//----------------------------------------------------------------------------
void vtkMrmlColorNode::Write(ofstream& of, int nIndent)
{
  vtkIndent i1(nIndent);

  // Write all attributes not equal to their defaults
  
  of << i1 << "<Color";
  
  // Strings
  if (this->Name && strcmp(this->Name, "")) 
  {
    of << " name='" << this->Name << "'";
  }
  if (this->Labels && strcmp(this->Labels, "")) 
  {
    of << " labels='" << this->Labels << "'";
  }
  if (this->Description && strcmp(this->Description, "")) 
  {
    of << " description='" << this->Description << "'";
  }

  // Numbers
  if (this->Ambient != 0)
  {
    of << " ambient='" << this->Ambient << "'";
  }
  if (this->Diffuse != 1.0)
  {
    of << " diffuse='" << this->Diffuse << "'";
  }
  if (this->Specular != 0)
  {
    of << " specular='" << this->Specular << "'";
  }
  if (this->Power != 1)
  {
    of << " power='" << this->Power << "'";
  }

  // Arrays
  if (this->DiffuseColor[0] != 1.0 || this->DiffuseColor[1] != 1.0 ||
      this->DiffuseColor[2] != 1.0)
  {
    of << " diffuseColor='" << this->DiffuseColor[0] << " "
       << this->DiffuseColor[1] << " " << this->DiffuseColor[2] << "'";
  }

  of << "></Color>\n";
}

//----------------------------------------------------------------------------
// Copy the node's attributes to this object.
// Does NOT copy: ID, Name
void vtkMrmlColorNode::Copy(vtkMrmlNode *anode)
{
  vtkMrmlNode::MrmlNodeCopy(anode);
  vtkMrmlColorNode *node = (vtkMrmlColorNode *) anode;

  // Strings
  this->SetLabels(node->Labels);

  // Vectors
  this->SetDiffuseColor(node->DiffuseColor);
  
  // Numbers
  this->SetAmbient(node->Ambient);
  this->SetDiffuse(node->Diffuse);
  this->SetSpecular(node->Specular);
  this->SetPower(node->Power);
}


//----------------------------------------------------------------------------
void vtkMrmlColorNode::PrintSelf(ostream& os, vtkIndent indent)
{
  int idx;
  
  vtkMrmlNode::PrintSelf(os,indent);

  os << indent << "Name: " <<
    (this->Name ? this->Name : "(none)") << "\n";
  os << indent << "Labels: " <<
    (this->Labels ? this->Labels : "(none)") << "\n";

  os << indent << "Ambient:           " << this->Ambient << "\n";
  os << indent << "Diffuse:           " << this->Diffuse << "\n";
  os << indent << "Specular:          " << this->Specular << "\n";
  os << indent << "Power:             " << this->Power << "\n";

  os << "DiffuseColor:\n";
  for (idx = 0; idx < 3; ++idx)
  {
    os << indent << ", " << this->DiffuseColor[idx];
  }
  os << ")\n";
}

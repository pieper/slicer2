/*=auto=========================================================================
Copyright (c) 1999 Surgical Planning Lab, Brigham and Women's Hospital
 
Direct all questions on this copyright to slicer@ai.mit.edu.
The following terms apply to all files associated with the software unless
explicitly disclaimed in individual files.   

The authors hereby grant permission to use, copy, and distribute this
software and its documentation for any NON-COMMERCIAL purpose, provided
that existing copyright notices are retained verbatim in all copies.
The authors grant permission to modify this software and its documentation 
for any NON-COMMERCIAL purpose, provided that such modifications are not 
distributed without the explicit consent of the authors and that existing
copyright notices are retained in all copies. Some of the algorithms
implemented by this software are patented, observe all applicable patent law.

IN NO EVENT SHALL THE AUTHORS OR DISTRIBUTORS BE LIABLE TO ANY PARTY FOR
DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
OF THE USE OF THIS SOFTWARE, ITS DOCUMENTATION, OR ANY DERIVATIVES THEREOF,
EVEN IF THE AUTHORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

THE AUTHORS AND DISTRIBUTORS SPECIFICALLY DISCLAIM ANY WARRANTIES, INCLUDING,
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
PARTICULAR PURPOSE, AND NON-INFRINGEMENT.  THIS SOFTWARE IS PROVIDED ON AN
'AS IS' BASIS, AND THE AUTHORS AND DISTRIBUTORS HAVE NO OBLIGATION TO PROVIDE
MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
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
  // vtkMrmlNode's attributes
  this->ID = 0;
  this->Description = NULL;
  this->Options = NULL;
  this->Ignore = 0;

  this->Name = NULL;
  this->Labels = NULL;

  memset(this->DiffuseColor,0,3*sizeof(float));

  this->Ambient = 0.0;
  this->Diffuse = 1.0;
  this->Specular = 0.0;
  this->Power = 1;
}

//----------------------------------------------------------------------------
vtkMrmlColorNode::~vtkMrmlColorNode()
{
  if (this->Name)
  {
    delete [] this->Name;
    this->Name = NULL;
  }
  if (this->Labels)
  {
    delete [] this->Labels;
    this->Labels = NULL;
  }
}

//----------------------------------------------------------------------------
// Copy the node's attributes to this object.
// Does NOT copy: ID, Name
void vtkMrmlColorNode::Copy(vtkMrmlColorNode *node)
{
  vtkMrmlNode::Copy(node);

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

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
#include "vtkMrmlColorLutNode.h"
#include "vtkObjectFactory.h"

//------------------------------------------------------------------------------
vtkMrmlColorLutNode* vtkMrmlColorLutNode::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMrmlColorLutNode");
  if(ret)
  {
    return (vtkMrmlColorLutNode*)ret;
  }
  // If the factory was unable to create the object, then create it here.
  return new vtkMrmlColorLutNode;
}

//----------------------------------------------------------------------------
vtkMrmlColorLutNode::vtkMrmlColorLutNode()
{
  vtkMrmlNode::vtkMrmlNode();

  this->Name = NULL;
  this->FileName = NULL;
  this->NodeType = new char[9];
  strcpy(this->NodeType,"ColorLut");

  memset(this->HueRange,0,2*sizeof(float));
  memset(this->SaturationRange,0,2*sizeof(float));
  memset(this->ValueRange,0,2*sizeof(float));
  memset(this->AnnoColor,0,3*sizeof(float));

  this->NumberOfColors = 256;
}

//----------------------------------------------------------------------------
vtkMrmlColorLutNode::~vtkMrmlColorLutNode()
{
  if (this->Name)
  {
    delete [] this->Name;
    this->Name = NULL;
  }
  if (this->FileName)
  {
    delete [] this->FileName;
    this->FileName = NULL;
  }
}

//----------------------------------------------------------------------------
// Copy the node's attributes to this object.
// Does NOT copy: ID, Name
void vtkMrmlColorLutNode::Copy(vtkMrmlColorLutNode *node)
{
  vtkMrmlNode::Copy(node);

  // Strings
  this->SetFileName(node->FileName);

  // Vectors
  this->SetHueRange(node->HueRange);
  this->SetSaturationRange(node->SaturationRange);
  this->SetValueRange(node->ValueRange);
  this->SetAnnoColor(node->AnnoColor);
  
  // Numbers
  this->SetNumberOfColors(node->NumberOfColors);
}


//----------------------------------------------------------------------------
void vtkMrmlColorLutNode::PrintSelf(ostream& os, vtkIndent indent)
{
  int idx;
  
  vtkObject::PrintSelf(os,indent);

  os << indent << "Name: " <<
    (this->Name ? this->Name : "(none)") << "\n";
  os << indent << "FileName: " <<
    (this->FileName ? this->FileName : "(none)") << "\n";

  os << indent << "NumberOfColors:    " << this->NumberOfColors << "\n";

  os << "HueRange:\n";
  for (idx = 0; idx < 2; ++idx)
  {
    os << indent << ", " << this->HueRange[idx];
  }
  os << ")\n";

  os << "SaturationRange:\n";
  for (idx = 0; idx < 2; ++idx)
  {
    os << indent << ", " << this->SaturationRange[idx];
  }
  os << ")\n";

  os << "ValueRange:\n";
  for (idx = 0; idx < 2; ++idx)
  {
    os << indent << ", " << this->ValueRange[idx];
  }
  os << ")\n";

  os << "AnnoColor:\n";
  for (idx = 0; idx < 3; ++idx)
  {
    os << indent << ", " << this->AnnoColor[idx];
  }
  os << ")\n";

}

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
#include "vtkMrmlCrossSectionNode.h"
#include "vtkObjectFactory.h"

//------------------------------------------------------------------------------
vtkMrmlCrossSectionNode* vtkMrmlCrossSectionNode::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMrmlCrossSectionNode");
  if(ret)
  {
    return (vtkMrmlCrossSectionNode*)ret;
  }
  // If the factory was unable to create the object, then create it here.
  return new vtkMrmlCrossSectionNode;
}

//----------------------------------------------------------------------------
vtkMrmlCrossSectionNode::vtkMrmlCrossSectionNode()
{
  // Strings
  this->Direction = NULL;
  this->BackVolRefID = NULL;
  this->ForeVolRefID = NULL;
  this->LabelVolRefID = NULL;
  this->ClipType = NULL;
  
  // Numbers
  this->Position = 0;
  this->SliceSlider = 0;
  this->RotatorX = 0;
  this->RotatorY = 0;
  this->Zoom = 1.0;
  this->InModel = 0;
  this->ClipState = 0;
}

//----------------------------------------------------------------------------
vtkMrmlCrossSectionNode::~vtkMrmlCrossSectionNode()
{
  if (this->Direction)
  {
    delete [] this->Direction;
    this->Direction = NULL;
  }
  if (this->BackVolRefID)
  {
    delete [] this->BackVolRefID;
    this->BackVolRefID = NULL;
  }
  if (this->ForeVolRefID)
  {
    delete [] this->ForeVolRefID;
    this->ForeVolRefID = NULL;
  }
  if (this->LabelVolRefID)
  {
    delete [] this->LabelVolRefID;
    this->LabelVolRefID = NULL;
  }
  if (this->ClipType)
  {
    delete [] this->ClipType;
    this->ClipType = NULL;
  }
}

//----------------------------------------------------------------------------
void vtkMrmlCrossSectionNode::Write(ofstream& of, int nIndent)
{
  // Write all attributes not equal to their defaults
  
  vtkIndent i1(nIndent);

  of << i1 << "<CrossSection";

  of << " position='" << this->Position << "'";
  
  // Strings
  if (this->Direction && strcmp(this->Direction, "")) 
  {
    of << " direction='" << this->Direction << "'";
  }
  else {
    of << " direction='none'";
  }
  if (this->BackVolRefID && strcmp(this->BackVolRefID, ""))
  {
    of << " backVolRefID='" << this->BackVolRefID << "'";
  }
  if (this->ForeVolRefID && strcmp(this->ForeVolRefID, ""))
  {
    of << " foreVolRefID='" << this->ForeVolRefID << "'";
  }
  if (this->LabelVolRefID && strcmp(this->LabelVolRefID, ""))
  {
    of << " labelVolRefID='" << this->LabelVolRefID << "'";
  }
  if (this->ClipType && strcmp(this->ClipType, ""))
  {
    of << " clipType='" << this->ClipType << "'";
  }

  // Numbers
  if (this->InModel != 0)
  {
    of << " inmodel='" << (this->InModel ? "true":"false") << "'";
  }
  if (this->SliceSlider != 0)
  {
    of << " sliceslider='" << this->SliceSlider << "'";
  }
  if (this->RotatorX != 0)
  {
    of << " rotatorx='" << this->RotatorX << "'";
  }
  if (this->RotatorY != 0)
  {
    of << " rotatory='" << this->RotatorY << "'";
  }
  if (this->Zoom != 1.0)
  {
    of << " zoom='" << this->Zoom << "'";
  }
  if (this->ClipState != 0)
  {
    of << " clipState='" << this->ClipState << "'";
  }
  of << "></CrossSection>\n";
}

//----------------------------------------------------------------------------
// Copy the node's attributes to this object.
// Does NOT copy: Position
void vtkMrmlCrossSectionNode::Copy(vtkMrmlNode *anode)
{
  vtkMrmlNode::MrmlNodeCopy(anode);
  vtkMrmlCrossSectionNode *node = (vtkMrmlCrossSectionNode *) anode;

  // Strings
  this->SetDirection(node->Direction);
  this->SetBackVolRefID(node->BackVolRefID);
  this->SetForeVolRefID(node->ForeVolRefID);
  this->SetLabelVolRefID(node->LabelVolRefID);
  this->SetClipType(node->ClipType);
  // Numbers
  this->SetInModel(node->InModel);
  this->SetSliceSlider(node->SliceSlider);
  this->SetRotatorX(node->RotatorX);
  this->SetRotatorY(node->RotatorY);
  this->SetZoom(node->Zoom);
}

//----------------------------------------------------------------------------
void vtkMrmlCrossSectionNode::PrintSelf(ostream& os, vtkIndent indent)
{
  
  vtkMrmlNode::PrintSelf(os,indent);

  os << indent << "Position: " << this->Position << "\n";
  os << indent << "Description: " <<
    (this->Description ? this->Description : "(none)") << "\n";
  os << indent << "InModel: " << this->InModel << "\n";
  os << indent << "SliceSlider: " << this->SliceSlider << "\n";
  os << indent << "RotatorX: " << this->RotatorX << "\n";
  os << indent << "RotatorY: " << this->RotatorY << "\n";
  os << indent << "Zoom: " << this->Zoom << "\n";
  os << indent << "BackVolRefID: " << 
    (this->BackVolRefID ? this->BackVolRefID : "(none)") << "\n";
  os << indent << "ForeVolRefID: " <<
    (this->ForeVolRefID ? this->ForeVolRefID : "(none)") << "\n";
  os << indent << "LabelVolRefID: " <<
    (this->LabelVolRefID ? this->LabelVolRefID : "(none)") << "\n";
  os << indent << "ClipType: " <<
    (this->ClipType ? this->ClipType : "(none)") << "\n";
  os << indent << "ClipState: " << this->ClipState << "\n";
}

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
#include "vtkMrmlModelStateNode.h"
#include "vtkObjectFactory.h"

//------------------------------------------------------------------------------
vtkMrmlModelStateNode* vtkMrmlModelStateNode::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMrmlModelStateNode");
  if(ret)
  {
    return (vtkMrmlModelStateNode*)ret;
  }
  // If the factory was unable to create the object, then create it here.
  return new vtkMrmlModelStateNode;
}

//----------------------------------------------------------------------------
vtkMrmlModelStateNode::vtkMrmlModelStateNode()
{
  // Strings
  this->ModelRefID = NULL;
  
  // Numbers
  this->Visible = 1;
  this->Opacity = 1.0;
  this->SliderVisible = 1;
  this->SonsVisible = 1;
  this->Clipping = 0;
  this->BackfaceCulling = 1;
}

//----------------------------------------------------------------------------
vtkMrmlModelStateNode::~vtkMrmlModelStateNode()
{
  if (this->ModelRefID)
  {
    delete [] this->ModelRefID;
    this->ModelRefID = NULL;
  }
}

//----------------------------------------------------------------------------
void vtkMrmlModelStateNode::Write(ofstream& of, int nIndent)
{
  // Write all attributes not equal to their defaults
  
  vtkIndent i1(nIndent);

  of << i1 << "<ModelState";

  // Strings
  if (this->ModelRefID && strcmp(this->ModelRefID, "")) 
  {
    of << " modelRefID='" << this->ModelRefID << "'";
  }
  
  // Numbers
  if (this->Visible != 1)
  {
    of << " visible='" << (this->Visible ? "true":"false") << "'";
  }
  if (this->Opacity != 1.0)
  {
    of << " opacity='" << this->Opacity << "'";
  }
  if (this->SliderVisible != 1)
  {
    of << " slidervisible='" << (this->SliderVisible ? "true":"false") << "'";
  }
  if (this->SonsVisible != 1)
  {
    of << " sonsvisible='" << (this->SonsVisible ? "true":"false") << "'";
  }
  if (this->Clipping != 0)
  {
    of << " clipping='" << (this->Clipping ? "true":"false") << "'";
  }
  if (this->BackfaceCulling != 1)
  {
    of << " backfaceCulling='" << (this->BackfaceCulling ? "true":"false") << "'";
  }
  
  of << "></ModelState>\n";
}

//----------------------------------------------------------------------------
// Copy the node's attributes to this object.
// Does NOT copy: ID, FilePrefix, Name
void vtkMrmlModelStateNode::Copy(vtkMrmlNode *anode)
{
  vtkMrmlNode::MrmlNodeCopy(anode);
  vtkMrmlModelStateNode *node = (vtkMrmlModelStateNode *) anode;

  // Strings
  this->SetModelRefID(node->ModelRefID);
  // Numbers
  this->SetVisible(node->Visible);
  this->SetOpacity(node->Opacity);
  this->SetSliderVisible(node->SliderVisible);
  this->SetSonsVisible(node->SonsVisible);
  this->SetClipping(node->Clipping);
  this->SetBackfaceCulling(node->BackfaceCulling);
}

//----------------------------------------------------------------------------
void vtkMrmlModelStateNode::PrintSelf(ostream& os, vtkIndent indent)
{
  
  vtkMrmlNode::PrintSelf(os,indent);

  os << indent << "ModelRefID: " <<
    (this->ModelRefID ? this->ModelRefID : "(none)") << "\n";
  os << indent << "Visible: " << this->Visible << "\n";
  os << indent << "Opacity: " << this->Opacity << "\n";
  os << indent << "SliderVisible: " << this->SliderVisible << "\n";
  os << indent << "SonsVisible: " << this->SonsVisible << "\n";
  os << indent << "Clipping: " << this->Clipping << "\n";
  os << indent << "BackfaceCulling: " << this->BackfaceCulling << "\n";
}

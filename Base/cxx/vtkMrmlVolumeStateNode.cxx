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
#include "vtkMrmlVolumeStateNode.h"
#include "vtkObjectFactory.h"

//------------------------------------------------------------------------------
vtkMrmlVolumeStateNode* vtkMrmlVolumeStateNode::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMrmlVolumeStateNode");
  if(ret)
  {
    return (vtkMrmlVolumeStateNode*)ret;
  }
  // If the factory was unable to create the object, then create it here.
  return new vtkMrmlVolumeStateNode;
}

//----------------------------------------------------------------------------
vtkMrmlVolumeStateNode::vtkMrmlVolumeStateNode()
{
  this->Indent = 1;
  
  // Strings
  this->VolumeRefID = NULL;
  this->ColorLUT = NULL;

  // Numbers
  this->Foreground = 0;
  this->Background = 0;
  this->Fade = 0;
  this->Opacity = 1.0;
}

//----------------------------------------------------------------------------
vtkMrmlVolumeStateNode::~vtkMrmlVolumeStateNode()
{
  if (this->VolumeRefID)
  {
    delete [] this->VolumeRefID;
    this->VolumeRefID = NULL;
  }
  if (this->ColorLUT)
  {
    delete [] this->ColorLUT;
    this->ColorLUT = NULL;
  }
}

//----------------------------------------------------------------------------
void vtkMrmlVolumeStateNode::Write(ofstream& of, int nIndent)
{
  // Write all attributes not equal to their defaults
  
  vtkIndent i1(nIndent);

  of << i1 << "<VolumeState";

  // Strings
  if (this->VolumeRefID && strcmp(this->VolumeRefID, ""))
  {
    of << " volumeRefID='" << this->VolumeRefID << "'";
  }
  if (this->ColorLUT && strcmp(this->ColorLUT,""))
  {
    of << " colorLUT='" << this->ColorLUT << "'";
  }
 
  // Numbers 
  if (this->Foreground != 0) 
  {
    of << " foreground='" << (this->Foreground ? "true":"false") << "'";
  }
  if (this->Background != 0)
  {
    of << " background='" << (this->Background ? "true":"false") << "'";
  }
  if (this->Fade != 0)
  {
    of << " fade='" << (this->Fade ? "true":"false") << "'";
  }
  if (this->Opacity != 1.0)
  {
    of << " opacity='" << this->Opacity << "'";
  }
  
  of << ">\n";;
}

//----------------------------------------------------------------------------
// Copy the node's attributes to this object.
// Does NOT copy: ID, FilePrefix, Name
void vtkMrmlVolumeStateNode::Copy(vtkMrmlNode *anode)
{
  vtkMrmlNode::MrmlNodeCopy(anode);
  vtkMrmlVolumeStateNode *node = (vtkMrmlVolumeStateNode *) anode;

  // Strings
  this->SetVolumeRefID(node->VolumeRefID);
  this->SetColorLUT(node->ColorLUT);

  // Numbers
  this->SetForeground(node->Foreground);
  this->SetBackground(node->Background);
  this->SetFade(node->Fade);
  this->SetOpacity(node->Opacity);
}

//----------------------------------------------------------------------------
void vtkMrmlVolumeStateNode::PrintSelf(ostream& os, vtkIndent indent)
{
  
  vtkMrmlNode::PrintSelf(os,indent);

  os << indent << "VolumeRefID: " << 
    (this->VolumeRefID ? this->VolumeRefID : "(none)") << "\n";
  os << indent << "ColorLUT: " << 
    (this->ColorLUT ? this->ColorLUT : "(none)") << "\n";
  os << indent << "Foreground: " << this->Foreground << "\n";  
  os << indent << "Background: " << this->Background << "\n";
  os << indent << "Fade: " << this->Fade << "\n";
  os << indent << "Opacity: " << this->Opacity << "\n";
}

/*=auto=========================================================================

(c) Copyright 2005 Massachusetts Institute of Technology (MIT) All Rights Reserved.

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
//#include <stdio.h>
//#include <ctype.h>
//#include <string.h>
//#include <math.h>
#include "vtkMrmlSegmenterNode.h"
#include "vtkObjectFactory.h"

//------------------------------------------------------------------------------
vtkMrmlSegmenterNode* vtkMrmlSegmenterNode::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMrmlSegmenterNode");
  if(ret)
  {
    return (vtkMrmlSegmenterNode*)ret;
  }
  // If the factory was unable to create the object, then create it here.
  return new vtkMrmlSegmenterNode;
}

//----------------------------------------------------------------------------
vtkMrmlSegmenterNode::vtkMrmlSegmenterNode()
{
  this->Indent                        = 1;
  this->DisplayProb                   = 0;
  this->RegistrationInterpolationType = 0;
  this->DisableMultiThreading         = 0;
}

//----------------------------------------------------------------------------
void vtkMrmlSegmenterNode::Write(ofstream& of, int nIndent)
{
  // Write all attributes not equal to their defaults
  
  vtkIndent i1(nIndent);
  of << i1 << "<Segmenter";
  this->vtkMrmlSegmenterAtlasNode::Write(of);
  of << " DisplayProb  ='"               << this->DisplayProb  << "'";
  if (this->RegistrationInterpolationType) of << " RegistrationInterpolationType ='"<< this->RegistrationInterpolationType << "'";
  if (this->DisableMultiThreading)   of << " DisableMultiThreading ='" << this->DisableMultiThreading << "'";
  of << ">\n";
}

//----------------------------------------------------------------------------
// Copy the node's attributes to this object.
// Does NOT copy: ID, Name and PrintDir
void vtkMrmlSegmenterNode::Copy(vtkMrmlNode *anode)
{
  this->vtkMrmlSegmenterAtlasNode::Copy(anode);
  vtkMrmlSegmenterNode *node          = (vtkMrmlSegmenterNode *) anode;
  this->DisplayProb                   = node->DisplayProb;
  this->RegistrationInterpolationType = node->RegistrationInterpolationType;
  this->DisableMultiThreading         = node->DisableMultiThreading; 
}

//----------------------------------------------------------------------------
void vtkMrmlSegmenterNode::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkMrmlSegmenterNode::PrintSelf(os, indent);
  os << indent << "DisplayProb: "               << this->DisplayProb <<  "\n"; 
  os << indent << "RegistrationInterpolationType: " << this->RegistrationInterpolationType << "\n"; 
  os << indent << "DisableMultiThreading: "; 
  if (this->DisableMultiThreading) cout << "Yes\n"; 
  else cout << "No\n"; 
  os << "\n";
}

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
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include "vtkMrmlSegmenterAtlasInputNode.h"
#include "vtkObjectFactory.h"

//------------------------------------------------------------------------------
vtkMrmlSegmenterAtlasInputNode* vtkMrmlSegmenterAtlasInputNode::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMrmlSegmenterAtlasInputNode");
  if(ret)
  {
    return (vtkMrmlSegmenterAtlasInputNode*)ret;
  }
  // If the factory was unable to create the object, then create it here.
  return new vtkMrmlSegmenterAtlasInputNode;
}

//----------------------------------------------------------------------------
vtkMrmlSegmenterAtlasInputNode::vtkMrmlSegmenterAtlasInputNode()
{
  this->FileName = NULL;
}

//----------------------------------------------------------------------------
vtkMrmlSegmenterAtlasInputNode::~vtkMrmlSegmenterAtlasInputNode()
{
  if (this->FileName)
  {
    delete [] this->FileName;
    this->FileName = NULL;
  }
}

//----------------------------------------------------------------------------
void vtkMrmlSegmenterAtlasInputNode::Write(ofstream& of)
{
  // Write all attributes not equal to their defaults
  
  if (this->Name && strcmp(this->Name, "")) 
  {
    of << " name ='" << this->Name << "'";
  }
  if (this->FileName && strcmp(this->FileName, "")) 
  {
    of << " FileName='" << this->FileName << "'";
  }
}

//----------------------------------------------------------------------------
// Copy the node's attributes to this object.
// Does NOT copy: ID, Name
void vtkMrmlSegmenterAtlasInputNode::Copy(vtkMrmlNode *anode)
{
  vtkMrmlSegmenterAtlasInputNode *node = (vtkMrmlSegmenterAtlasInputNode *) anode;

  this->SetFileName(node->FileName); 
}

//----------------------------------------------------------------------------
void vtkMrmlSegmenterAtlasInputNode::PrintSelf(ostream& os, vtkIndent indent)
{
   os << indent << "Name: " <<
    (this->Name ? this->Name : "(none)") << "\n";
   os << indent << "FileName: " <<
    (this->FileName ? this->FileName : "(none)") << "\n";
}

/* 
  this->IntensityAvgValuePreDef = 0;

  vtkIndent i1(nIndent);
  of << i1 << "<SegmenterInput";
  of << " IntensityAvgValuePreDef ='"    << this->IntensityAvgValuePreDef << "'";
  of << "></SegmenterInput>\n";

  vtkMrmlNode::MrmlNodeCopy(anode);
  this->IntensityAvgValuePreDef = node->IntensityAvgValuePreDef;


  vtkMrmlNode::PrintSelf(os,indent);
  os << indent << "IntensityAvgValuePreDef:"          << this->IntensityAvgValuePreDef << "\n";
*/

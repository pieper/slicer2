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
#include "vtkMrmlPathNode.h"
#include "vtkObjectFactory.h"

//------------------------------------------------------------------------------
vtkMrmlPathNode* vtkMrmlPathNode::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMrmlPathNode");
  if(ret)
  {
    return (vtkMrmlPathNode*)ret;
  }
  // If the factory was unable to create the object, then create it here.
  return new vtkMrmlPathNode;
}

//----------------------------------------------------------------------------
vtkMrmlPathNode::vtkMrmlPathNode()
{

  // vtkMrmlNode's attributes
  this->Indent = 1;

//  this->cPathColor = NULL;
//  this->cLandColor = NULL;
//  this->fPathColor = NULL;
//  this->fLandColor = NULL;

}

//----------------------------------------------------------------------------
vtkMrmlPathNode::~vtkMrmlPathNode()
{

//  if (this->cLandColor)
//    {
//      delete [] this->cLandColor;
//      this->cLandColor = NULL;
//    }
//  if (this->cPathColor)
//    {
//      delete [] this->cPathColor;
//      this->cPathColor = NULL;
//    }
//  if (this->fLandColor)
//    {
//      delete [] this->fLandColor;
//      this->fLandColor = NULL;
//    }
//  if (this->fPathColor)
//    {
//      delete [] this->fPathColor;
//      this->fPathColor = NULL;
//    }
}

//----------------------------------------------------------------------------
void vtkMrmlPathNode::Write(ofstream& of, int nIndent)
{
  // Write all attributes not equal to their defaults
  
  vtkIndent i1(nIndent);

  of << i1 << "<Path";

  // Strings
//  if (this->cLandColor && strcmp(this->cLandColor, "")) 
//    {
//      of << " cLandColor='" << this->cLandColor << "'";
//    }
//  if (this->cPathColor && strcmp(this->cPathColor, "")) 
//    {
//      of << " cPathColor='" << this->cPathColor << "'";
//    }
//  if (this->fLandColor && strcmp(this->fLandColor, "")) 
//    {
//      of << " fLandColor='" << this->fLandColor << "'";
//    }
//  if (this->fPathColor && strcmp(this->fPathColor, "")) 
//    {
//      of << " fPathColor='" << this->fPathColor << "'";
//    }
  of << "></Path>\n";;
}

//----------------------------------------------------------------------------
// Copy the node's attributes to this object.
// Does NOT copy: ID, FilePrefix, Name
void vtkMrmlPathNode::Copy(vtkMrmlNode *node)
{
  vtkMrmlNode::MrmlNodeCopy(node);
  //Strings, how do we do with color?
}

//----------------------------------------------------------------------------
void vtkMrmlPathNode::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkMrmlNode::PrintSelf(os,indent);

//  os << indent << "cLandColor: " <<
//    (this->cLandColor ? this->cLandColor : "(none)") << "\n";
//  os << indent << "cPathColor: " <<
//    (this->cPathColor ? this->cPathColor : "(none)") << "\n";
//  os << indent << "fLandColor: " <<
//    (this->fLandColor ? this->fLandColor : "(none)") << "\n";
//  os << indent << "fPathColor: " <<
//    (this->fPathColor ? this->fPathColor : "(none)") << "\n";

}



/*=auto=========================================================================

(c) Copyright 2001 Massachusetts Institute of Technology

Permission is hereby granted, without payment, to copy, modify, display 
and distribute this software and its documentation, if any, for any purpose, 
provided that the above copyright notice and the following three paragraphs 
appear on all copies of this software.  Use of this software constitutes 
acceptance of these terms and conditions.

IN NO EVENT SHALL MIT BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, 
INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE 
AND ITS DOCUMENTATION, EVEN IF MIT HAS BEEN ADVISED OF THE POSSIBILITY OF 
SUCH DAMAGE.

MIT SPECIFICALLY DISCLAIMS ANY EXPRESS OR IMPLIED WARRANTIES INCLUDING, 
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR 
A PARTICULAR PURPOSE, AND NON-INFRINGEMENT.

THE SOFTWARE IS PROVIDED "AS IS."  MIT HAS NO OBLIGATION TO PROVIDE 
MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

=========================================================================auto=*/
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include "vtkMrmlLandmarkNode.h"
#include "vtkObjectFactory.h"

//------------------------------------------------------------------------------
vtkMrmlLandmarkNode* vtkMrmlLandmarkNode::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMrmlLandmarkNode");
  if(ret)
  {
    return (vtkMrmlLandmarkNode*)ret;
  }
  // If the factory was unable to create the object, then create it here.
  return new vtkMrmlLandmarkNode;
}

//----------------------------------------------------------------------------
vtkMrmlLandmarkNode::vtkMrmlLandmarkNode()
{
  this->XYZ[0] = this->XYZ[1] = this->XYZ[2] = 0.0;
  this->FXYZ[0] = this->FXYZ[1] = this->FXYZ[2] = 0.0;
  this->PathPosition = 0;
}

//----------------------------------------------------------------------------
vtkMrmlLandmarkNode::~vtkMrmlLandmarkNode()
{

}

//----------------------------------------------------------------------------
void vtkMrmlLandmarkNode::Write(ofstream& of, int nIndent)
{
  // Write all attributes not equal to their defaults
  
  vtkIndent i1(nIndent);

  of << i1 << "<Landmark";

  // Strings
  if (this->Name && strcmp(this->Name, "")) 
  {
    of << " name='" << this->Name << "'";
  }
  if (this->Description && strcmp(this->Description, "")) 
  {
    of << " description='" << this->Description << "'";
  }

  // Landmark
  of << " xyz='" << this->XYZ[0] << " " << this->XYZ[1] << " " <<
                    this->XYZ[2] << "'";

  // Landmark
  of << " focalxyz='" << this->FXYZ[0] << " " << this->FXYZ[1] << " " <<
                    this->FXYZ[2] << "'";

  of << " pathPosition='" << this->PathPosition << "'";

  of << "></Landmark>\n";;
}

//----------------------------------------------------------------------------
// Copy the node's attributes to this object.
// Does NOT copy: ID, FilePrefix, Name
void vtkMrmlLandmarkNode::Copy(vtkMrmlLandmarkNode *node)
{
  vtkMrmlNode::Copy(node);

  this->XYZ[0] = node->XYZ[0];
  this->XYZ[1] = node->XYZ[1];
  this->XYZ[2] = node->XYZ[2];
  this->PathPosition = node ->PathPosition;
}

//----------------------------------------------------------------------------
void vtkMrmlLandmarkNode::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkMrmlNode::PrintSelf(os,indent);

  os << indent << "Name: " <<
    (this->Name ? this->Name : "(none)") << "\n";

  // XYZ
  os << indent << "XYZ: (";
  os << indent << this->XYZ[0] << ", " << this->XYZ[1] << ", " << this->XYZ[2]
                  << ")" << "\n";
  // FXYZ
  os << indent << "FXYZ: (";
  os << indent << this->FXYZ[0] << ", " << this->FXYZ[1] << ", " << this->FXYZ[2]
                  << ")" << "\n";
  os << indent << "pathPosition: ";
  os << indent << this->PathPosition;
}



/*=auto=========================================================================

(c) Copyright 2002 Massachusetts Institute of Technology

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
#include "vtkMrmlPointNode.h"
#include "vtkObjectFactory.h"

//------------------------------------------------------------------------------
vtkMrmlPointNode* vtkMrmlPointNode::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMrmlPointNode");
  if(ret)
  {
    return (vtkMrmlPointNode*)ret;
  }
  // If the factory was unable to create the object, then create it here.
  return new vtkMrmlPointNode;
}

//----------------------------------------------------------------------------
vtkMrmlPointNode::vtkMrmlPointNode()
{
  this->XYZ[0] = this->XYZ[1] = this->XYZ[2] = 0.0;
  this->FXYZ[0] = 0.0;
  this->FXYZ[1] = 0.0; 
  this->FXYZ[2] = 0.0;
  this->Index = 0.0;
}

//----------------------------------------------------------------------------
vtkMrmlPointNode::~vtkMrmlPointNode()
{

}

//----------------------------------------------------------------------------
void vtkMrmlPointNode::Write(ofstream& of, int nIndent)
{
  // Write all attributes not equal to their defaults
  
  vtkIndent i1(nIndent);

  of << i1 << "<Point";

  // Strings
  if (this->Name && strcmp(this->Name, "")) 
  {
    of << " name='" << this->Name << "'";
  }
  if (this->Description && strcmp(this->Description, "")) 
  {
    of << " description='" << this->Description << "'";
  }

  // Point
  of << "index='" << this->Index << "'";
  of << " xyz='" << this->XYZ[0] << " " << this->XYZ[1] << " " <<
                    this->XYZ[2] << "'";
  of << " focalxyz='" << this->FXYZ[0] << " " << this->FXYZ[1] << " " <<
    this->FXYZ[2] << "'";
  of << "></Point>\n";;
}

//----------------------------------------------------------------------------
// Copy the node's attributes to this object.
// Does NOT copy: ID, FilePrefix, Name
void vtkMrmlPointNode::Copy(vtkMrmlNode *anode)
{
  vtkMrmlNode::MrmlNodeCopy(anode);
  vtkMrmlPointNode *node = (vtkMrmlPointNode *) anode;

  this->XYZ[0] = node->XYZ[0];
  this->XYZ[1] = node->XYZ[1];
  this->XYZ[2] = node->XYZ[2];
  this->FXYZ[0] = node->FXYZ[0];
  this->FXYZ[1] = node->FXYZ[1];
  this->FXYZ[2] = node->FXYZ[2];
  this->Index = node->Index;
}

//----------------------------------------------------------------------------
void vtkMrmlPointNode::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkMrmlNode::PrintSelf(os,indent);

  os << indent << "Name: " <<
    (this->Name ? this->Name : "(none)") << "\n";

  // INDEX
  os << indent << "Index: " << this->Index << "\n";

  // XYZ
  os << indent << "XYZ: (";
  os << indent << this->XYZ[0] << ", " << this->XYZ[1] << ", " << this->XYZ[2]
                  << ") \n" ;

// FXYZ
  os << indent << "FXYZ: (";
  os << indent << this->FXYZ[0] << ", " << this->FXYZ[1] << ", " << this->FXYZ[2] << ")" << "\n";

}



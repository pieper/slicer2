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
#include "vtkMath.h"
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
  this->OrientationWXYZ[0] = this->OrientationWXYZ[1] = this->OrientationWXYZ[2] = 0.0;
  this->OrientationWXYZ[3] = 1.0;
  this->Index = 0;
}

//----------------------------------------------------------------------------
vtkMrmlPointNode::~vtkMrmlPointNode()
{

}

//----------------------------------------------------------------------------
void vtkMrmlPointNode::SetOrientationWXYZFromMatrix4x4(vtkMatrix4x4 *mat)
{
    // copied from: vtkTransform::GetOrientationWXYZ 
    int i;


    // convenient access to matrix
    double (*matrix)[4] = mat->Element;
    double ortho[3][3];
    double wxyz[4];

    for (i = 0; i < 3; i++)
    {   ortho[0][i] = matrix[0][i];
        ortho[1][i] = matrix[1][i];
        ortho[2][i] = matrix[2][i];
    }
    if (vtkMath::Determinant3x3(ortho) < 0)
    {   ortho[0][i] = -ortho[0][i];
        ortho[1][i] = -ortho[1][i];
        ortho[2][i] = -ortho[2][i];
    }

    vtkMath::Matrix3x3ToQuaternion(ortho, wxyz);

    // calc the return value wxyz
    double mag = sqrt(wxyz[1]*wxyz[1] + wxyz[2]*wxyz[2] + wxyz[3]*wxyz[3]);

    if (mag)
    {   wxyz[0] = 2.0*acos(wxyz[0])/vtkMath::DoubleDegreesToRadians();
        wxyz[1] /= mag;
        wxyz[2] /= mag;
        wxyz[3] /= mag;
    }
    else
    {   wxyz[0] = 0.0;
        wxyz[1] = 0.0;
        wxyz[2] = 0.0;
        wxyz[3] = 1.0;
    } 
    this->OrientationWXYZ[0] = (float) wxyz[0];
    this->OrientationWXYZ[1] = (float) wxyz[1];
    this->OrientationWXYZ[2] = (float) wxyz[2];
    this->OrientationWXYZ[3] = (float) wxyz[3];
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
  of << " index='" << this->Index << "'";
  of << " xyz='" << this->XYZ[0] << " " << this->XYZ[1] << " " <<
                    this->XYZ[2] << "'";
  of << " focalxyz='" << this->FXYZ[0] << " " << this->FXYZ[1] << " " <<
    this->FXYZ[2] << "'";
  of << " orientationwxyz='" << this->OrientationWXYZ[0] << " " << this->OrientationWXYZ[1] << " " <<
    this->OrientationWXYZ[2] << " " << this->OrientationWXYZ[3] << "'";
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
  this->OrientationWXYZ[0] = node->OrientationWXYZ[0];
  this->OrientationWXYZ[1] = node->OrientationWXYZ[1];
  this->OrientationWXYZ[2] = node->OrientationWXYZ[2];
  this->OrientationWXYZ[3] = node->OrientationWXYZ[3];
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

// OrientationWXYZ
  os << indent << "OrientationWXYZ: (";
  os << indent ;
  os << this->OrientationWXYZ[0] << ", " ;
  os << this->OrientationWXYZ[1] << ", " ;
  os << this->OrientationWXYZ[2] << ", " ;
  os << this->OrientationWXYZ[3] << ")" << "\n";
}



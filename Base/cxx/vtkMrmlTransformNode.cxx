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
#include "vtkMrmlTransformNode.h"
#include "vtkObjectFactory.h"

//------------------------------------------------------------------------------
vtkMrmlTransformNode* vtkMrmlTransformNode::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMrmlTransformNode");
  if(ret)
  {
    return (vtkMrmlTransformNode*)ret;
  }
  // If the factory was unable to create the object, then create it here.
  return new vtkMrmlTransformNode;
}

//----------------------------------------------------------------------------
vtkMrmlTransformNode::vtkMrmlTransformNode()
{
  vtkMrmlNode::vtkMrmlNode();

  this->NodeType = new char[10];
  strcpy(this->NodeType,"Transform");

  this->Transform = vtkTransform::New();
}

//----------------------------------------------------------------------------
vtkMrmlTransformNode::~vtkMrmlTransformNode()
{
  this->Transform->Delete();

}

//----------------------------------------------------------------------------
// Copy the node's attributes to this object.
// Does NOT copy: ID
void vtkMrmlTransformNode::Copy(vtkMrmlTransformNode *node)
{
  vtkMrmlNode::Copy(node);

  this->Transform->DeepCopy(node->Transform);

}

//----------------------------------------------------------------------------
void vtkMrmlTransformNode::PrintSelf(ostream& os, vtkIndent indent)
{
  int idx;
  
  vtkObject::PrintSelf(os,indent);

  // Transform
  os << indent << "Transform:\n";
    this->Transform->PrintSelf(os, indent.GetNextIndent());  
}


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
#include "vtkMrmlScaleNode.h"
#include "vtkObjectFactory.h"

//------------------------------------------------------------------------------
vtkMrmlScaleNode* vtkMrmlScaleNode::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMrmlScaleNode");
  if(ret)
  {
    return (vtkMrmlScaleNode*)ret;
  }
  // If the factory was unable to create the object, then create it here.
  return new vtkMrmlScaleNode;
}

//----------------------------------------------------------------------------
vtkMrmlScaleNode::vtkMrmlScaleNode()
{
  vtkMrmlTransformNode::vtkMrmlTransformNode();
  memset(this->Scale,0,3*sizeof(float));
}

//----------------------------------------------------------------------------
vtkMrmlScaleNode::~vtkMrmlScaleNode()
{

}

//----------------------------------------------------------------------------
// Copy the node's attributes to this object.
// Does NOT copy: ID, FilePrefix, Name
void vtkMrmlScaleNode::Copy(vtkMrmlScaleNode *node)
{
  vtkMrmlTransformNode::Copy(node);

  // Numbers
  this->SetScale(node->Scale);
}

//----------------------------------------------------------------------------
void vtkMrmlScaleNode::PrintSelf(ostream& os, vtkIndent indent)
{
  int idx;
  
  vtkObject::PrintSelf(os,indent);

  os << "Scale:\n";
  for (idx = 0; idx < 3; ++idx)
  {
    os << indent << ", " << this->Scale[idx];
  }
  os << ")\n";
}

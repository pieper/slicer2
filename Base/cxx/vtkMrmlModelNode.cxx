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
#include "vtkMrmlModelNode.h"
#include "vtkObjectFactory.h"

//------------------------------------------------------------------------------
vtkMrmlModelNode* vtkMrmlModelNode::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMrmlModelNode");
  if(ret)
  {
    return (vtkMrmlModelNode*)ret;
  }
  // If the factory was unable to create the object, then create it here.
  return new vtkMrmlModelNode;
}

//----------------------------------------------------------------------------
vtkMrmlModelNode::vtkMrmlModelNode()
{
  // vtkMrmlNode's attributes
  this->ID = 0;
  this->Description = NULL;
  this->Options = NULL;
  this->Ignore = 0;

  this->Name = NULL;
  this->FileName = NULL;
  this->Color = NULL;
  this->RasToIjkMatrix = NULL;

  memset(this->ScalarRange, 0, 2*sizeof(int));

  this->RasToWld = vtkMatrix4x4::New();

  this->Opacity = 1.0;
  this->Visibility = 1;
  this->ScalarVisibility = 0;
  this->BackfaceCulling = 1;
  this->Clipping = 0;  

}

//----------------------------------------------------------------------------
vtkMrmlModelNode::~vtkMrmlModelNode()
{
  this->RasToWld->Delete();

  if (this->Name)
  {
    delete [] this->Name;
    this->Name = NULL;
  }
  if (this->FileName)
  {
    delete [] this->FileName;
    this->FileName = NULL;
  }
  if (this->Color)
  {
    delete [] this->Color;
    this->Color = NULL;
  }
  if (this->RasToIjkMatrix)
  {
    delete [] this->RasToIjkMatrix;
    this->RasToIjkMatrix = NULL;
  }
}

//----------------------------------------------------------------------------
// Copy the node's attributes to this object.
// Does NOT copy: ID, FilePrefix, Name
void vtkMrmlModelNode::Copy(vtkMrmlModelNode *node)
{
  vtkMrmlNode::Copy(node);

  // Strings
  this->SetFileName(node->FileName);
  this->SetColor(node->Color);
  this->SetRasToIjkMatrix(node->RasToIjkMatrix);

  // Vectors
  this->SetScalarRange(node->ScalarRange);
  
  // Numbers
  this->SetOpacity(node->Opacity);
  this->SetVisibility(node->Visibility);
  this->SetScalarVisibility(node->ScalarVisibility);
  this->SetBackfaceCulling(node->BackfaceCulling);
  this->SetClipping(node->Clipping);

  // Matrices
  this->SetRasToWld(node->RasToWld);

}

//----------------------------------------------------------------------------
void vtkMrmlModelNode::SetRasToWld(vtkMatrix4x4 *rasToWld)
{
  this->RasToWld->DeepCopy(rasToWld);
}

//----------------------------------------------------------------------------
void vtkMrmlModelNode::PrintSelf(ostream& os, vtkIndent indent)
{
  int idx;
  
  vtkMrmlNode::PrintSelf(os,indent);

  os << indent << "Name: " <<
    (this->Name ? this->Name : "(none)") << "\n";
  os << indent << "FilePattern: " <<
    (this->FileName ? this->FileName : "(none)") << "\n";
  os << indent << "Color: " <<
    (this->Color ? this->Color : "(none)") << "\n";
  os << indent << "RasToIjkMatrix: " <<
    (this->RasToIjkMatrix ? this->RasToIjkMatrix : "(none)") << "\n";

  os << indent << "Opacity:           " << this->Opacity << "\n";
  os << indent << "Visibility:        " << this->Visibility << "\n";
  os << indent << "ScalarVisibility:  " << this->ScalarVisibility << "\n";
  os << indent << "BackfaceCulling:   " << this->BackfaceCulling << "\n";
  os << indent << "Clipping:          " << this->Clipping << "\n";

  os << "ScalarRange:\n";
  for (idx = 0; idx < 2; ++idx)
  {
    os << indent << ", " << this->ScalarRange[idx];
  }
  os << ")\n";

  // Matrices
  os << indent << "RasToWld:\n";
    this->RasToWld->PrintSelf(os, indent.GetNextIndent());  
}

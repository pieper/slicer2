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
  // Strings
  this->FileName = NULL;
  this->Color = NULL;
  this->FullFileName = NULL;

  // Numbers
  this->Opacity = 1.0;
  this->Visibility = 1;
  this->Clipping = 0;
  this->BackfaceCulling = 1;
  this->ScalarVisibility = 0;
  
  // Arrays
  this->ScalarRange[0] = 0;
  this->ScalarRange[1] = 100;

  this->RasToWld = vtkMatrix4x4::New();
}

//----------------------------------------------------------------------------
vtkMrmlModelNode::~vtkMrmlModelNode()
{
  this->RasToWld->Delete();

  if (this->FileName)
  {
    delete [] this->FileName;
    this->FileName = NULL;
  }
  if (this->FullFileName)
  {
    delete [] this->FullFileName;
    this->FullFileName = NULL;
  }
  if (this->Color)
  {
    delete [] this->Color;
    this->Color = NULL;
  }
}

//----------------------------------------------------------------------------
void vtkMrmlModelNode::Write(ofstream& of, int nIndent)
{
  // Write all attributes not equal to their defaults
  
  vtkIndent i1(nIndent);

  of << i1 << "<Model";

  // Strings
  if (this->Name && strcmp(this->Name, "")) 
  {
    of << " name='" << this->Name << "'";
  }
  if (this->FileName && strcmp(this->FileName, "")) 
  {
    of << " fileName='" << this->FileName << "'";
  }
  if (this->Color && strcmp(this->Color, "")) 
  {
    of << " color='" << this->Color << "'";
  }
  if (this->Description && strcmp(this->Description, "")) 
  {
    of << " description='" << this->Description << "'";
  }

  // Numbers
  if (this->Opacity != 1.0)
  {
    of << " opacity='" << this->Opacity << "'";
  }
  if (this->Visibility != 1)
  {
    of << " visibility='" << (this->Visibility ? "yes" : "no") << "'";
  }
  if (this->Clipping != 0)
  {
    of << " clipping='" << (this->Clipping ? "yes" : "no") << "'";
  }
  if (this->BackfaceCulling != 1)
  {
    of << " backfaceCulling='" << (this->BackfaceCulling ? "yes" : "no") << "'";
  }
  if (this->ScalarVisibility != 0)
  {
    of << " scalarVisibility='" << (this->ScalarVisibility ? "yes" : "no") << "'";
  }

  // Arrays
  if (this->ScalarRange[0] != 0 || this->ScalarRange[1] != 100)
  {
    of << " scalarRange='" << this->ScalarRange[0] << " "
       << this->ScalarRange[1] << "'";
  }

  of << "></Model>\n";;
}

//----------------------------------------------------------------------------
// Copy the node's attributes to this object.
// Does NOT copy: ID, FilePrefix, Name
void vtkMrmlModelNode::Copy(vtkMrmlNode *anode)
{
  vtkMrmlNode::MrmlNodeCopy(anode);
  vtkMrmlModelNode *node = (vtkMrmlModelNode *) anode;

  // Strings
  this->SetFileName(node->FileName);
  this->SetFullFileName(node->FullFileName);
  this->SetColor(node->Color);

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
  os << indent << "FileName: " <<
    (this->FileName ? this->FileName : "(none)") << "\n";
  os << indent << "FullFileName: " <<
    (this->FullFileName ? this->FullFileName : "(none)") << "\n";
  os << indent << "Color: " <<
    (this->Color ? this->Color : "(none)") << "\n";

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

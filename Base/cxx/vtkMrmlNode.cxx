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
#include "vtkMrmlNode.h"
#include "vtkObjectFactory.h"

//------------------------------------------------------------------------------
vtkMrmlNode* vtkMrmlNode::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMrmlNode");
  if(ret)
  {
    return (vtkMrmlNode*)ret;
  }
  // If the factory was unable to create the object, then create it here.
  return new vtkMrmlNode;
}

//----------------------------------------------------------------------------
vtkMrmlNode::vtkMrmlNode()
{
  this->ID = 0;
  this->Indent = 0;
  this->Description = NULL;
  this->Options = NULL;
}

//----------------------------------------------------------------------------
vtkMrmlNode::~vtkMrmlNode()
{
  if (this->Description)
  {
    delete [] this->Description;
    this->Description = NULL;
  }

  if (this->Options)
  {
    delete [] this->Options;
    this->Options = NULL;
  }
}

//----------------------------------------------------------------------------
void vtkMrmlNode::Copy(vtkMrmlNode *node)
{
  // Copy everything except: ID
  this->SetDescription(node->Description);
  this->SetOptions(node->Options);
}

//----------------------------------------------------------------------------
void vtkMrmlNode::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkObject::PrintSelf(os,indent);

  os << indent << "ID:          " << this->ID << "\n";

  os << indent << "Indent:      " << this->Indent << "\n";

  os << indent << "Description: " <<
    (this->Description ? this->Description : "(none)") << "\n";

  os << indent << "Options:     " <<
    (this->Options ? this->Options : "(none)") << "\n";
}

//----------------------------------------------------------------------------
char* vtkMrmlNode::GetMatrixToString(vtkMatrix4x4 *mat)
{
  int i, j;
  float m[16];
  char *s = new char[200];
 
  for (i=0; i<4; i++)
  {
	  for (j=0; j<4; j++)
    {
      m[i*4+j] = mat->GetElement(i, j);
    }
  }
  sprintf(s, "%g %g %g %g %g %g %g %g %g %g %g %g %g %g %g %g",
    m[0], m[1], m[2], m[3], m[4], m[5], m[6], m[7], m[8],
    m[9], m[10], m[11], m[12], m[13], m[14], m[15]);

  return s;
}

//----------------------------------------------------------------------------
void vtkMrmlNode::SetMatrixToString(vtkMatrix4x4 *mat, char *s)
{
  int i, j;
  float m[16];
  
  sscanf(s, "%g %g %g %g %g %g %g %g %g %g %g %g %g %g %g %g",
    &m[0], &m[1], &m[2], &m[3], &m[4], &m[5], &m[6], &m[7], &m[8],
    &m[9], &m[10], &m[11], &m[12], &m[13], &m[14], &m[15]);

  for (i=0; i<4; i++)
  {
    for (j=0; j<4; j++)
    {
       mat->SetElement(i, j, m[i*4+j]);
    }
  }
}



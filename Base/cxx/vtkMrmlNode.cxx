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
  this->Description = NULL;
  this->Options = NULL;
  this->Ignore = 0;
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
  this->SetIgnore(node->Ignore);
}

//----------------------------------------------------------------------------
void vtkMrmlNode::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkObject::PrintSelf(os,indent);

  os << indent << "ID:          " << this->ID << "\n";

  os << indent << "Description: " <<
    (this->Description ? this->Description : "(none)") << "\n";

  os << indent << "Options:     " <<
    (this->Options ? this->Options : "(none)") << "\n";

  os << indent << "Ignore:      " << this->Ignore << "\n";
}



/*=auto=========================================================================
Copyright (c) 2000 Surgical Planning Lab, Brigham and Women's Hospital
 
Direct all questions on this copyright to slicer@ai.mit.edu.
The following terms apply to all files associated with the software unless
explicitly disclaimed in individual files.   

The authors hereby grant permission to use, copy (but NOT distribute) this
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
#include "vtkMrmlOptionsNode.h"
#include "vtkObjectFactory.h"

//------------------------------------------------------------------------------
vtkMrmlOptionsNode* vtkMrmlOptionsNode::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMrmlOptionsNode");
  if(ret)
  {
    return (vtkMrmlOptionsNode*)ret;
  }
  // If the factory was unable to create the object, then create it here.
  return new vtkMrmlOptionsNode;
}

//----------------------------------------------------------------------------
vtkMrmlOptionsNode::vtkMrmlOptionsNode()
{
  // Strings
  this->Program = NULL;
  this->Contents = NULL;
  this->Options = NULL;
}

//----------------------------------------------------------------------------
vtkMrmlOptionsNode::~vtkMrmlOptionsNode()
{
  if (this->Program)
  {
    delete [] this->Program;
    this->Program = NULL;
  }
  if (this->Contents)
  {
    delete [] this->Contents;
    this->Contents = NULL;
  }
  if (this->Options)
  {
    delete [] this->Options;
    this->Options = NULL;
  }
}

//----------------------------------------------------------------------------
void vtkMrmlOptionsNode::Write(ofstream& of, int nIndent)
{
  vtkIndent i1(nIndent);

  // Write all attributes not equal to their defaults
  
  of << i1 << "<Options";
  
  // Strings
  if (this->Program && strcmp(this->Program, "")) 
  {
    of << " program='" << this->Program << "'";
  }
  if (this->Contents && strcmp(this->Contents, "")) 
  {
    of << " contents='" << this->Contents << "'";
  }

  of << ">\n";

  if (this->Options && strcmp(this->Options, "")) 
  {
    of << this->Options;
  }

  of << "</Options>\n";
}

//----------------------------------------------------------------------------
// Copy the node's attributes to this object.
// Does NOT copy: ID, Name
void vtkMrmlOptionsNode::Copy(vtkMrmlOptionsNode *node)
{
  vtkMrmlNode::Copy(node);

  // Strings
  this->SetOptions(node->Options);
}


//----------------------------------------------------------------------------
void vtkMrmlOptionsNode::PrintSelf(ostream& os, vtkIndent indent)
{
  int idx;
  
  vtkObject::PrintSelf(os,indent);

  os << indent << "Name: " <<
    (this->Options ? this->Options : "(none)") << "\n";
}

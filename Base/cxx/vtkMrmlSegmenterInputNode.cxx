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
#include "vtkMrmlSegmenterInputNode.h"
#include "vtkObjectFactory.h"

//------------------------------------------------------------------------------
vtkMrmlSegmenterInputNode* vtkMrmlSegmenterInputNode::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMrmlSegmenterInputNode");
  if(ret)
  {
    return (vtkMrmlSegmenterInputNode*)ret;
  }
  // If the factory was unable to create the object, then create it here.
  return new vtkMrmlSegmenterInputNode;
}

//----------------------------------------------------------------------------
vtkMrmlSegmenterInputNode::vtkMrmlSegmenterInputNode()
{
  this->FilePrefix = NULL;
  this->FileName = NULL;
  memset(this->ImageRange,0,2*sizeof(int));
}

//----------------------------------------------------------------------------
vtkMrmlSegmenterInputNode::~vtkMrmlSegmenterInputNode()
{
  if (this->FilePrefix)
  {
    delete [] this->FilePrefix;
    this->FilePrefix = NULL;
  }
  if (this->FileName)
  {
    delete [] this->FileName;
    this->FileName = NULL;
  }
}

//----------------------------------------------------------------------------
void vtkMrmlSegmenterInputNode::Write(ofstream& of, int nIndent)
{
  // Write all attributes not equal to their defaults
  
  vtkIndent i1(nIndent);

  of << i1 << "<SegmenterInput";
  if (this->Name && strcmp(this->Name, "")) 
  {
    of << " name ='" << this->Name << "'";
  }
  if (this->FilePrefix && strcmp(this->FilePrefix, "")) 
  {
    of << " FilePrefix='" << this->FilePrefix << "'";
  }
  if (this->FileName && strcmp(this->FileName, "")) 
  {
    of << " FileName='" << this->FileName << "'";
  }
  if (this->ImageRange[0] != 1 || this->ImageRange[1] != 1)
  {
    of << " ImageRange='" << this->ImageRange[0] << " "
       << this->ImageRange[1] << "'";
  }
  of << "></SegmenterInput>\n";;
}

//----------------------------------------------------------------------------
// Copy the node's attributes to this object.
// Does NOT copy: ID, Name
void vtkMrmlSegmenterInputNode::Copy(vtkMrmlNode *anode)
{
  vtkMrmlNode::MrmlNodeCopy(anode);
  vtkMrmlSegmenterInputNode *node = (vtkMrmlSegmenterInputNode *) anode;

  this->SetFilePrefix(node->FilePrefix); 
  this->SetFileName(node->FileName); 
  this->SetImageRange(node->ImageRange); 
}

//----------------------------------------------------------------------------
void vtkMrmlSegmenterInputNode::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkMrmlNode::PrintSelf(os,indent);
   os << indent << "Name: " <<
    (this->Name ? this->Name : "(none)") << "\n";
   os << indent << "FilePrefix: " <<
    (this->FilePrefix ? this->FilePrefix : "(none)") << "\n";
   os << indent << "FileName: " <<
    (this->FileName ? this->FileName : "(none)") << "\n";
   os << "ImageRange:\n";
   for (int idx = 0; idx < 2; ++idx) {
     os << indent << ", " << this->ImageRange[idx];
   }
   os << ")\n";
}



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
#include "vtkMrmlVolumeReadWriteStructuredPointsNode.h"
#include "vtkObjectFactory.h"

  //------------------------------------------------------------------------------
  vtkMrmlVolumeReadWriteStructuredPointsNode* vtkMrmlVolumeReadWriteStructuredPointsNode::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMrmlVolumeReadWriteStructuredPointsNode");
  if(ret)
  {
    return (vtkMrmlVolumeReadWriteStructuredPointsNode*)ret;
  }
  // If the factory was unable to create the object, then create it here.
  return new vtkMrmlVolumeReadWriteStructuredPointsNode;
}

//----------------------------------------------------------------------------
vtkMrmlVolumeReadWriteStructuredPointsNode::vtkMrmlVolumeReadWriteStructuredPointsNode()
{
  // Strings
  this->ReaderType = NULL; // needed to use macro below
  this->SetReaderType("vtkStructuredPoints");
}

//----------------------------------------------------------------------------
vtkMrmlVolumeReadWriteStructuredPointsNode::~vtkMrmlVolumeReadWriteStructuredPointsNode()
{
  if (this->ReaderType)
    {
      delete [] this->ReaderType;
      this->ReaderType = NULL;
    }

}


//----------------------------------------------------------------------------
void vtkMrmlVolumeReadWriteStructuredPointsNode::Write(ofstream& of, int nIndent)
{
  
  vtkIndent i1(nIndent);

  of << i1 << "<VolumeReadWrite";

  // Write all attributes 
  // Strings
  if (this->ReaderType && strcmp(this->ReaderType,""))
    {
      of << " readerType='" << this->ReaderType << "'";
    }
  of << "></VolumeReadWrite>\n";;
}


//----------------------------------------------------------------------------
// Copy the node's attributes to this object.
// Does NOT copy: ID, FilePrefix, Name, VolumeID
void vtkMrmlVolumeReadWriteStructuredPointsNode::Copy(vtkMrmlNode *anode)
{
  vtkMrmlNode::MrmlNodeCopy(anode);
  vtkMrmlVolumeReadWriteStructuredPointsNode *node = (vtkMrmlVolumeReadWriteStructuredPointsNode *) anode;

  // Strings
  this->SetReaderType(node->ReaderType);
}


//----------------------------------------------------------------------------
void vtkMrmlVolumeReadWriteStructuredPointsNode::PrintSelf(ostream& os, vtkIndent indent)
{
  int idx;
  
  vtkMrmlNode::PrintSelf(os,indent);

  os << indent << "ReaderType: " <<
    (this->ReaderType ? this->ReaderType : "(none)") << "\n";

}


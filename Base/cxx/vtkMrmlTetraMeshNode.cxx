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
#include "vtkMrmlTetraMeshNode.h"
#include "vtkObjectFactory.h"

//------------------------------------------------------------------------------
vtkMrmlTetraMeshNode* vtkMrmlTetraMeshNode::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMrmlTetraMeshNode");
  if(ret)
  {
    return (vtkMrmlTetraMeshNode*)ret;
  }
  // If the factory was unable to create the object, then create it here.
  return new vtkMrmlTetraMeshNode;
}

//----------------------------------------------------------------------------
vtkMrmlTetraMeshNode::vtkMrmlTetraMeshNode()
{
  // Strings
  this->TetraMeshID = NULL;
  this->FileName = NULL;

  // DisplayTypes

  // Defaults are don't display anything
  this->DisplaySurfaces = 0;
  this->DisplayEdges    = 0;
  this->DisplayNodes    = 0;
    this->NodeScaling     = 9.5;
    this->NodeSkip        = 2;
  this->DisplayScalars  = 0;
    this->ScalarScaling   = 9.5;
    this->ScalarSkip      = 2;
  this->DisplayVectors  = 0;
    this->VectorScaling   = 9.5;
    this->VectorSkip      = 2;
    
}

//----------------------------------------------------------------------------
vtkMrmlTetraMeshNode::~vtkMrmlTetraMeshNode()
{
 if (this->TetraMeshID)
  {
    delete [] this->TetraMeshID;
    this->TetraMeshID = NULL;
  }
  if (this->FileName)
  {
    delete [] this->FileName;
    this->FileName = NULL;
  }
}

//----------------------------------------------------------------------------
void vtkMrmlTetraMeshNode::Write(ofstream& of, int nIndent)
{
  // Write all attributes not equal to their defaults

  vtkIndent i1(nIndent);

  of << i1 << "<TetraMesh";
  
  // Strings
  if (this->TetraMeshID && strcmp(this->TetraMeshID,""))
  {
    of << " id='" << this->TetraMeshID << "'";
  }
  if (this->Name && strcmp(this->Name, "")) 
  {
    of << " name='" << this->Name << "'";
  }
  if (this->FileName && strcmp(this->FileName, "")) 
  {
    of << " FileName='" << this->FileName << "'";
  }
  if (this->Description && strcmp(this->Description, "")) 
  {
    of << " description='" << this->Description << "'";
  }

  // Numbers

  // Defaults are don't display anything

    this->NodeScaling     = 9.5;
    this->NodeSkip        = 2;
    this->ScalarScaling   = 9.5;
    this->ScalarSkip      = 2;
    this->VectorScaling   = 9.5;
    this->VectorSkip      = 2;

  if (this->DisplaySurfaces != 0)
  {
    of << " DisplaySurfaces='" << this->DisplaySurfaces << "'";
  }
  if (this->DisplayEdges != 0)
  {
    of << " DisplayEdges='" << this->DisplayEdges << "'";
  }
  if (this->DisplayNodes != 0)
  {
    of << " DisplayNodes='" << this->DisplayNodes << "'";
  }
  if (this->NodeSkip != 2)
  {
    of << " NodeSkip='" << this->NodeSkip << "'";
  }
  if (this->NodeScaling != 9.5)
  {
    of << " NodeScaling='" << this->NodeScaling << "'";
  }
  if (this->DisplayScalars != 0)
  {
    of << " DisplayScalars='" << this->DisplayScalars << "'";
  }
  if (this->ScalarSkip != 2)
  {
    of << " ScalarSkip='" << this->ScalarSkip << "'";
  }
  if (this->ScalarScaling != 9.5)
  {
    of << " ScalarScaling='" << this->ScalarScaling << "'";
  }
  if (this->DisplayVectors != 0)
  {
    of << " DisplayVectors='" << this->DisplayVectors << "'";
  }
  if (this->VectorSkip != 2)
  {
    of << " VectorSkip='" << this->VectorSkip << "'";
  }
  if (this->VectorScaling != 9.5)
  {
    of << " VectorScaling='" << this->VectorScaling << "'";
  }

  //End
  of << "></TetraMesh>\n";;
}

//----------------------------------------------------------------------------
// Copy the node's attributes to this object.
// Does NOT copy: ID, FilePrefix, Name, TetraMeshID
void vtkMrmlTetraMeshNode::Copy(vtkMrmlNode *anode)
{
  vtkMrmlNode::MrmlNodeCopy(anode);
  vtkMrmlTetraMeshNode *node = (vtkMrmlTetraMeshNode *) anode;

  this->DisplaySurfaces = node->DisplaySurfaces;
  this->DisplayEdges    = node->DisplayEdges;
  this->DisplayNodes    = node->DisplayNodes;
    this->NodeScaling     = node->NodeScaling;
    this->NodeSkip        = node->NodeSkip;
  this->DisplayScalars  = node->DisplayScalars;
    this->ScalarScaling   = node->ScalarScaling;
    this->ScalarSkip      = node->ScalarSkip;
  this->DisplayVectors  = node->DisplayVectors;
    this->VectorScaling   = node->VectorScaling;
    this->VectorSkip      = node->VectorSkip;
}


//----------------------------------------------------------------------------
void vtkMrmlTetraMeshNode::PrintSelf(ostream& os, vtkIndent indent)
{
  int idx;
  
  vtkMrmlNode::PrintSelf(os,indent);

  os << indent << "TetraMeshID: " <<
    (this->TetraMeshID ? this->TetraMeshID : "(none)") << "\n";
  os << indent << "Name: " <<
    (this->Name ? this->Name : "(none)") << "\n";

  os << indent << "DisplaySurfaces: " << this->DisplaySurfaces << "\n";
  os << indent << "DisplayEdges: " << this->DisplayEdges << "\n";
  os << indent << "DisplayNodes: " << this->DisplayNodes << "\n";
  os << indent << "  NodeScaling: " << this->NodeScaling << "\n";
  os << indent << "  NodeSkip: " << this->NodeSkip << "\n";
  os << indent << "DisplayScalars: " << this->DisplayScalars << "\n";
  os << indent << "  ScalarScaling: " << this->ScalarScaling << "\n";
  os << indent << "  ScalarSkip: " << this->ScalarSkip << "\n";
  os << indent << "DisplayVectors: " << this->DisplayVectors << "\n";
  os << indent << "  VectorScaling: " << this->VectorScaling << "\n";
  os << indent << "  VectorSkip: " << this->VectorSkip << "\n";
}


/*=auto=========================================================================

(c) Copyright 2003 Massachusetts Institute of Technology (MIT) All Rights Reserved.

This software ("3D Slicer") is provided by The Brigham and Women's 
Hospital, Inc. on behalf of the copyright holders and contributors.
Permission is hereby granted, without payment, to copy, modify, display 
and distribute this software and its documentation, if any, for internal 
research purposes only, provided that (1) the above copyright notice and 
the following four paragraphs appear on all copies of this software, and 
(2) that source code to any modifications to this software be made 
publicly available under terms no more restrictive than those in this 
License Agreement. Use of this software constitutes acceptance of these 
terms and conditions.

3D Slicer Software has not been reviewed or approved by the Food and 
Drug Administration, and is for non-clinical, IRB-approved Research Use 
Only.  In no event shall data or images generated through the use of 3D 
Slicer Software be used in the provision of patient care.

IN NO EVENT SHALL THE COPYRIGHT HOLDERS AND CONTRIBUTORS BE LIABLE TO 
ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL 
DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, 
EVEN IF THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE BEEN ADVISED OF THE 
POSSIBILITY OF SUCH DAMAGE.

THE COPYRIGHT HOLDERS AND CONTRIBUTORS SPECIFICALLY DISCLAIM ANY EXPRESS 
OR IMPLIED WARRANTIES INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND 
NON-INFRINGEMENT.

THE SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS 
IS." THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE NO OBLIGATION TO 
PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.


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
  this->Clipping = 0;
  this->Opacity  = 1.0;

  // Defaults are don't display anything
  this->DisplaySurfaces = 0;
    this->SurfacesUseCellData = 1; // default, use cell data
    this->SurfacesSmoothNormals = 0; // default, don't smooth normals
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
  const char y[] = "yes";
  const char n[] = "no";

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

  if (this->Clipping != 0)
  {
    of << " Clipping='" << (this->Clipping ? y : n) << "'";
  }
  if (this->Opacity != 1.0)
  {
    of << " Opacity='" << this->Opacity << "'";
  }

  // Defaults are don't display anything


  if (this->DisplaySurfaces != 0)
  {
    of << " DisplaySurfaces='" << (this->DisplaySurfaces ? y : n) << "'";
  }
  if (this->SurfacesUseCellData != 1)
  {
    of << " SurfacesUseCellData'" << (this->SurfacesUseCellData ? y : n)<< "'";
  }
  if (this->SurfacesSmoothNormals != 0)
  {
   of << " SurfacesSmoothNormals'" << (this->SurfacesSmoothNormals ? y : n)<< "'";
  }
  if (this->DisplayEdges != 0)
  {
    of << " DisplayEdges='" << (this->DisplayEdges ? y : n) << "'";
  }
  if (this->DisplayNodes != 0)
  {
    of << " DisplayNodes='" << (this->DisplayNodes ? y : n) << "'";
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
    of << " DisplayScalars='" << (this->DisplayScalars ? y : n) << "'";
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
    of << " DisplayVectors='" << (this->DisplayVectors ? y : n) << "'";
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
  
  this->Clipping        = node->Clipping;
  this->Opacity         = node->Opacity;

  this->DisplaySurfaces = node->DisplaySurfaces;
    this->SurfacesUseCellData = node->SurfacesUseCellData;
    this->SurfacesSmoothNormals = node->SurfacesSmoothNormals;
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
  vtkMrmlNode::PrintSelf(os,indent);

  os << indent << "TetraMeshID: " <<
    (this->TetraMeshID ? this->TetraMeshID : "(none)") << "\n";
  os << indent << "Name: " <<
    (this->Name ? this->Name : "(none)") << "\n";

  os << indent << "Clipping: " << this->Clipping << "\n";
  os << indent << "Opacity: " << this->Opacity << "\n";
  os << indent << "DisplaySurfaces: " << this->DisplaySurfaces << "\n";
  os << indent << "  SurfacesUseCellData: "<< this->SurfacesUseCellData<<"\n";
  os << indent << "  SurfacesSmoothNormals: "<< this->SurfacesSmoothNormals<<"\n";
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


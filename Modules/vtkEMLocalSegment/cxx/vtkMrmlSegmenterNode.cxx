/*=auto=========================================================================

(c) Copyright 2003 Massachusetts Institute of Technology (MIT) All Rights Reserved.

This software ("3D Slicer") is provided by The Brigham and Women's 
Hospital, Inc. on behalf of the copyright holders and contributors.
Permission is hereby granted, without payment, to copy, modify, display 
and distribute this software and its documentation, if any, for  
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
#include "vtkMrmlSegmenterNode.h"
#include "vtkObjectFactory.h"

//------------------------------------------------------------------------------
vtkMrmlSegmenterNode* vtkMrmlSegmenterNode::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMrmlSegmenterNode");
  if(ret)
  {
    return (vtkMrmlSegmenterNode*)ret;
  }
  // If the factory was unable to create the object, then create it here.
  return new vtkMrmlSegmenterNode;
}

//----------------------------------------------------------------------------
vtkMrmlSegmenterNode::vtkMrmlSegmenterNode()
{
  this->Indent             = 1;
  // This is a flag so we can see if we already read the node 
  this->AlreadyRead        = 0;
  this->MaxInputChannelDef    = 0;
  this->NumClasses         = 1;
  this->EMShapeIter        = 1;
  this->EMiteration        = 1;
  this->MFAiteration       = 1;
  this->Alpha              = 0.0;   
  this->SmWidth     = 1;
  this->SmSigma     = 1;
  this->StartSlice         = 0;
  this->EndSlice           = 0;
  this->DisplayProb     = 0;
  this->NumberOfTrainingSamples = 0;
  this->IntensityAvgClass = -1;
  this->PrintDir = NULL;
  for (int i=0; i < 3; i++) {
    this->SegmentationBoundaryMin[i] = 0; // Lower bound of the boundary box where the image gets segments.
    this->SegmentationBoundaryMax[i] = 0;// Upper bound of the boundary box where the image gets segments.
  }
}

//----------------------------------------------------------------------------
vtkMrmlSegmenterNode::~vtkMrmlSegmenterNode()
{
  if (this->PrintDir) {
    delete [] this->PrintDir;
    this->PrintDir = NULL;
  }
}

//----------------------------------------------------------------------------
void vtkMrmlSegmenterNode::Write(ofstream& of, int nIndent)
{
  // Write all attributes not equal to their defaults
  
  vtkIndent i1(nIndent);

  of << i1 << "<Segmenter";

  // Strings
  //  if (this->Name && strcmp(this->Name, "")) 
  //    {
  //      of << " name='" << this->Name << "'";
  //    }
  //    if (this->Description && strcmp(this->Description, "")) 
  //    {
  //      of << " description='" << this->Description << "'";
  //    }

  of << " NumClasses ='"                 << this->NumClasses << "'";
  of << " MaxInputChannelDef ='"         << this->MaxInputChannelDef << "'";
  of << " EMShapeIter ='"                << this->EMShapeIter << "'";
  of << " EMiteration ='"                << this->EMiteration << "'";
  of << " MFAiteration ='"               << this->MFAiteration << "'";
  of << " Alpha ='"                      << this->Alpha << "'";
  of << " SmWidth ='"                    << this->SmWidth << "'";
  of << " SmSigma ='"                    << this->SmSigma << "'";
  of << " SegmentationBoundaryMin ='" ;
  for (int i=0; i < 3; i++) of << this->SegmentationBoundaryMin[i]<< " " ; // Upper bound of the boundary box where the image gets segments.
  of << "'";

  of << " SegmentationBoundaryMax ='" ;
  for (int i=0; i < 3; i++) of << this->SegmentationBoundaryMax[i]<< " " ; // Upper bound of the boundary box where the image gets segments.
  of << "'";

  of << " DisplayProb  ='"               << this->DisplayProb  << "'";
  of << " NumberOfTrainingSamples ='"    << this->NumberOfTrainingSamples << "'";
  of << " IntensityAvgClass ='"          << this->IntensityAvgClass << "'";

  if (this->PrintDir && strcmp(this->PrintDir, "")) 
  of << " PrintDir ='"                   << this->PrintDir << "'";
  of << ">\n";;
}

//----------------------------------------------------------------------------
// Copy the node's attributes to this object.
// Does NOT copy: ID, FilePrefix, Name and PrintDir
void vtkMrmlSegmenterNode::Copy(vtkMrmlNode *anode)
{
  vtkMrmlNode::MrmlNodeCopy(anode);
  vtkMrmlSegmenterNode *node = (vtkMrmlSegmenterNode *) anode;

  this->NumClasses                 = node->NumClasses;
  this->MaxInputChannelDef         = node->MaxInputChannelDef;
  this->EMiteration                = node->EMiteration;
  this->MFAiteration               = node->MFAiteration;
  this->Alpha                      = node->Alpha;   
  this->SmWidth                    = node->SmWidth;
  this->SmSigma                    = node->SmSigma;
  this->StartSlice                 = node->StartSlice;
  this->EndSlice                   = node->EndSlice;
  this->DisplayProb                = node->DisplayProb;
  this->NumberOfTrainingSamples    = node->NumberOfTrainingSamples;
  this->IntensityAvgClass          = node->IntensityAvgClass;

  memcpy(this->SegmentationBoundaryMin, node->SegmentationBoundaryMin, sizeof(int)*3);
  memcpy(this->SegmentationBoundaryMax, node->SegmentationBoundaryMax, sizeof(int)*3);
}

//----------------------------------------------------------------------------
void vtkMrmlSegmenterNode::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkMrmlNode::PrintSelf(os,indent);
  os << indent << "AlreadyRead: "               << this->AlreadyRead     <<  "\n"; 
  os << indent << "NumClasses: "                << this->NumClasses      <<  "\n"; 
  os << indent << "MaxInputChannelDef: "        << this->MaxInputChannelDef <<  "\n"; 
  os << indent << "EMShapeIter: "               << this->EMShapeIter     <<  "\n"; 
  os << indent << "EMiteration: "               << this->EMiteration     <<  "\n"; 
  os << indent << "MFAiteration: "              << this->MFAiteration <<  "\n"; 
  os << indent << "Alpha: "                     << this->Alpha <<  "\n"; 
  os << indent << "SmWidth: "                   << this->SmWidth <<  "\n"; 
  os << indent << "SmSigma: "                   << this->SmSigma <<  "\n"; 
  os << indent << "StartSlice: "                << this->StartSlice  <<  "\n"; 
  os << indent << "EndSlice: "                  << this->EndSlice <<  "\n"; 
  os << indent << "DisplayProb: "               << this->DisplayProb <<  "\n"; 
  os << indent << "NumberOfTrainingSamples: "   << this->NumberOfTrainingSamples <<  "\n"; 
  os << indent << "IntensityAvgClass:"          << this->IntensityAvgClass << "\n";
  os << indent << "PrintDir:"                   << this->PrintDir << "\n"; 
  os << indent << "SegmentationBoundaryMin:    " ;
  for (int i=0; i < 3; i++) os << this->SegmentationBoundaryMin[i]<< " " ; // Upper bound of the boundary box where the image gets segments.
  os << "\n";

  os << indent << "SegmentationBoundaryMax:    " ;
  for (int i=0; i < 3; i++) os << this->SegmentationBoundaryMax[i]<< " " ; // Upper bound of the boundary box where the image gets segments.
  os << "\n";
}

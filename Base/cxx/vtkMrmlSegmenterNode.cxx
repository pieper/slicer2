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
  this->EMiteration        = 1;
  this->MFAiteration       = 1;
  this->Alpha              = 0.0;   
  this->SmWidth     = 1;
  this->SmSigma     = 1;
  this->PrintIntermediateResults      = 0;
  this->PrintIntermediateSlice    = 0;
  this->PrintIntermediateFrequency = 0;
  this->StartSlice         = 0;
  this->EndSlice           = 0;
  this->DisplayProb     = 0;
  this->NumberOfTrainingSamples = 0;
  this->IntensityAvgClass = -1;
}

//----------------------------------------------------------------------------
vtkMrmlSegmenterNode::~vtkMrmlSegmenterNode()
{

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
  of << " MaxInputChannelDef ='"            << this->MaxInputChannelDef << "'";
  of << " EMiteration ='"                << this->EMiteration << "'";
  of << " MFAiteration ='"               << this->MFAiteration << "'";
  of << " Alpha ='"                      << this->Alpha << "'";
  of << " SmWidth ='"                    << this->SmWidth << "'";
  of << " SmSigma ='"                    << this->SmSigma << "'";
  of << " PrintIntermediateResults ='"   << this->PrintIntermediateResults  << "'";
  of << " PrintIntermediateSlice ='"     << this->PrintIntermediateSlice << "'";
  of << " PrintIntermediateFrequency ='" << this->PrintIntermediateFrequency << "'";
  of << " StartSlice ='"                 << this->StartSlice << "'";
  of << " EndSlice ='"                   << this->EndSlice << "'";
  of << " DisplayProb  ='"               << this->DisplayProb  << "'";
  of << " NumberOfTrainingSamples ='"    << this->NumberOfTrainingSamples << "'";
  of << " IntensityAvgClass ='"          << this->IntensityAvgClass << "'";
  of << ">\n";;
}

//----------------------------------------------------------------------------
// Copy the node's attributes to this object.
// Does NOT copy: ID, FilePrefix, Name
void vtkMrmlSegmenterNode::Copy(vtkMrmlNode *anode)
{
  vtkMrmlNode::MrmlNodeCopy(anode);
  vtkMrmlSegmenterNode *node = (vtkMrmlSegmenterNode *) anode;

  this->NumClasses                 = node->NumClasses;
  this->MaxInputChannelDef            = node->MaxInputChannelDef;
  this->EMiteration                = node->EMiteration;
  this->MFAiteration               = node->MFAiteration;
  this->Alpha                      = node->Alpha;   
  this->SmWidth                    = node->SmWidth;
  this->SmSigma                    = node->SmSigma;
  this->PrintIntermediateResults   = node->PrintIntermediateResults;
  this->PrintIntermediateSlice     = node->PrintIntermediateSlice;
  this->PrintIntermediateFrequency = node->PrintIntermediateFrequency;
  this->StartSlice                 = node->StartSlice;
  this->EndSlice                   = node->EndSlice;
  this->DisplayProb                = node->DisplayProb;
  this->NumberOfTrainingSamples    = node->NumberOfTrainingSamples;
  this->IntensityAvgClass          = node->IntensityAvgClass;
}

//----------------------------------------------------------------------------
void vtkMrmlSegmenterNode::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkMrmlNode::PrintSelf(os,indent);
  os << indent << "AlreadyRead: "               << this->AlreadyRead     <<  "\n"; 
  os << indent << "NumClasses: "                << this->NumClasses      <<  "\n"; 
  os << indent << "MaxInputChannelDef: "           << this->MaxInputChannelDef <<  "\n"; 
  os << indent << "EMiteration: "               << this->EMiteration     <<  "\n"; 
  os << indent << "MFAiteration: "              << this->MFAiteration <<  "\n"; 
  os << indent << "Alpha: "                     << this->Alpha <<  "\n"; 
  os << indent << "SmWidth: "                   << this->SmWidth <<  "\n"; 
  os << indent << "SmSigma: "                   << this->SmSigma <<  "\n"; 
  os << indent << "PrintIntermediateResults: "  << this->PrintIntermediateResults <<  "\n"; 
  os << indent << "PrintIntermediateSlice: "    << this->PrintIntermediateSlice <<  "\n"; 
  os << indent << "PrintIntermediateFrequency: "<< this->PrintIntermediateFrequency <<  "\n"; 
  os << indent << "StartSlice: "                << this->StartSlice  <<  "\n"; 
  os << indent << "EndSlice: "                  << this->EndSlice <<  "\n"; 
  os << indent << "DisplayProb: "               << this->DisplayProb <<  "\n"; 
  os << indent << "NumberOfTrainingSamples: "   << this->NumberOfTrainingSamples <<  "\n"; 
  os << indent << "IntensityAvgClass:"          << this->IntensityAvgClass << "\n";
}

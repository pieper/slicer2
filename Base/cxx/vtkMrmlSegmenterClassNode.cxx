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
#include "vtkMrmlSegmenterClassNode.h"
#include "vtkObjectFactory.h"

//------------------------------------------------------------------------------
vtkMrmlSegmenterClassNode* vtkMrmlSegmenterClassNode::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMrmlSegmenterClassNode");
  if(ret)
  {
    return (vtkMrmlSegmenterClassNode*)ret;
  }
  // If the factory was unable to create the object, then create it here.
  return new vtkMrmlSegmenterClassNode;
}

//----------------------------------------------------------------------------
vtkMrmlSegmenterClassNode::vtkMrmlSegmenterClassNode()
{
  this->LocalPriorPrefix = NULL; 
  this->LocalPriorName   = NULL; 
  memset(this->LocalPriorRange,0,2*sizeof(int));
  this->LogMean          = NULL;
  this->LogCovariance    = NULL;
  this->Label            = 0;
  this->Prob             = 0.0;
}

//----------------------------------------------------------------------------
vtkMrmlSegmenterClassNode::~vtkMrmlSegmenterClassNode()
{
  if (this->LocalPriorPrefix)
  {
    delete [] this->LocalPriorPrefix;
    this->LocalPriorPrefix = NULL;
  }
  if (this->LocalPriorName)
  {
    delete [] this->LocalPriorName;
    this->LocalPriorName = NULL;
  }
  if (this->LogMean)
  {
    delete [] this->LogMean;
    this->LogMean = NULL;
  }
    if (this->LogCovariance)
  {
    delete [] this->LogCovariance;
    this->LogCovariance = NULL;
  }
}

//----------------------------------------------------------------------------
void vtkMrmlSegmenterClassNode::Write(ofstream& of, int nIndent)
{
  // Write all attributes not equal to their defaults
  
  vtkIndent i1(nIndent);

  of << i1 << "<SegmenterClass";
  if (this->Name && strcmp(this->Name, "")) 
  {
    of << " name ='" << this->Name << "'";
  }
  of << " Label='" << this->Label << "'";
  of << " Prob='" << this->Prob << "'";
  if (this->LocalPriorPrefix && strcmp(this->LocalPriorPrefix, "")) 
  {
    of << " LocalPriorPrefix='" << this->LocalPriorPrefix << "'";
  }
  if (this->LocalPriorName && strcmp(this->LocalPriorName, "")) 
  {
    of << " LocalPriorName='" << this->LocalPriorName << "'";
  }
  if (this->LocalPriorRange[0] != 0 || this->LocalPriorRange[1] != 0)
  {
    of << " LocalPriorRange='" << this->LocalPriorRange[0] << " "
       << this->LocalPriorRange[1] << "'";
  }
  if (this->LogMean && strcmp(this->LogMean, "")) 
  {
    of << " LogMean='" << this->LogMean << "'";
  }
  if (this->LogCovariance && strcmp(this->LogCovariance, "")) 
  {
    of << " LogCovariance='" << this->LogCovariance << "'";
  }
  of << "></SegmenterClass>\n";;
}

//----------------------------------------------------------------------------
// Copy the node's attributes to this object.
// Does NOT copy: ID, Name
void vtkMrmlSegmenterClassNode::Copy(vtkMrmlNode *anode)
{
  vtkMrmlNode::MrmlNodeCopy(anode);
  vtkMrmlSegmenterClassNode *node = (vtkMrmlSegmenterClassNode *) anode;

  this->SetLabel(node->Label);
  this->SetProb(node->Prob);
  this->SetLocalPriorPrefix(node->LocalPriorPrefix); 
  this->SetLocalPriorName(node->LocalPriorName); 
  this->SetLocalPriorRange(node->LocalPriorRange); 
  this->SetLogMean(node->LogMean);
  this->SetLogCovariance(node->LogCovariance);

}

//----------------------------------------------------------------------------
void vtkMrmlSegmenterClassNode::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkMrmlNode::PrintSelf(os,indent);
   os << indent << "Name: " <<
    (this->Name ? this->Name : "(none)") << "\n";
   os << indent << "Label: " << this->Label << "\n";
   os << indent << "Prob: " << this->Prob << "\n"; 
   os << indent << "LocalPriorPrefix: " <<
    (this->LocalPriorPrefix ? this->LocalPriorPrefix : "(none)") << "\n";
   os << indent << "LocalPriorName: " <<
    (this->LocalPriorName ? this->LocalPriorName : "(none)") << "\n";
   os << "LocalPriorRange:\n";
   for (int idx = 0; idx < 2; ++idx) {
     os << indent << ", " << this->LocalPriorRange[idx];
   }
   os << indent << "LogMean: " <<
    (this->LogMean ? this->LogMean : "(none)") << "\n";
   os << indent << "LogCovariance: " <<
    (this->LogCovariance ? this->LogCovariance : "(none)") << "\n";
   os << ")\n";
}



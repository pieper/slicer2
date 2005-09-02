/*=auto=========================================================================

(c) Copyright 2005 Massachusetts Institute of Technology (MIT) All Rights Reserved.

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
#include "vtkMrmlSegmenterAtlasGenericClassNode.h"
#include "vtkObjectFactory.h"

//------------------------------------------------------------------------------
vtkMrmlSegmenterAtlasGenericClassNode* vtkMrmlSegmenterAtlasGenericClassNode::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMrmlSegmenterAtlasGenericClassNode");
  if(ret)
  {
    return (vtkMrmlSegmenterAtlasGenericClassNode*)ret;
  }
  // If the factory was unable to create the object, then create it here.
  return new vtkMrmlSegmenterAtlasGenericClassNode;
}

//----------------------------------------------------------------------------
vtkMrmlSegmenterAtlasGenericClassNode::vtkMrmlSegmenterAtlasGenericClassNode() { 
  // vtkMrmlNode's attributes => Tabs following sub classes  
  this->Prob       = 0.0;

  this->LocalPriorWeight = 1.0;
  this->LocalPriorName   = NULL; 

  this->InputChannelWeights = NULL;
  this->PrintWeights        = 0;
  this->PrintRegistrationParameters = 0;
  this->PrintRegistrationSimularityMeasure = 0;


  memset(this->RegistrationTranslation,0,3*sizeof(double));
  memset(this->RegistrationRotation,0,3*sizeof(double));
  int i;
  for (i = 0; i < 3; i++) RegistrationScale[i]= 1.0;
  for (i = 0; i < 6; i++) this->RegistrationCovariance[i] = 1.0;
  this->RegistrationCovariance[6] = this->RegistrationCovariance[7] = this->RegistrationCovariance[8] = 0.1;

  this->RegistrationClassSpecificRegistrationFlag = 0;
  this->ExcludeFromIncompleteEStepFlag = 0;
}

//----------------------------------------------------------------------------
vtkMrmlSegmenterAtlasGenericClassNode::~vtkMrmlSegmenterAtlasGenericClassNode() { 
  if (this->InputChannelWeights) {
    delete [] this->InputChannelWeights;
    this->InputChannelWeights = NULL;
  }

  if (this->LocalPriorName)
  {
    delete [] this->LocalPriorName;
    this->LocalPriorName = NULL;
  }

}

//----------------------------------------------------------------------------
void vtkMrmlSegmenterAtlasGenericClassNode::Write(ofstream& of, int nIndent)
{
  // Write all attributes not equal to their defaults
  of << " Prob='" << this->Prob << "'";
  if (this->InputChannelWeights && strcmp(this->InputChannelWeights, "")) 
  {
    of << " InputChannelWeights='" << this->InputChannelWeights << "'";
  }
  of << " LocalPriorWeight='" << this->LocalPriorWeight << "'";

  if (this->LocalPriorName && strcmp(this->LocalPriorName, "")) 
  {
    of << " LocalPriorName='" << this->LocalPriorName << "'";
  }

  if (this->PrintWeights) of << " PrintWeights='" << this->PrintWeights << "'";
  if (this->PrintRegistrationParameters) of << " PrintRegistrationParameters='" << this->PrintRegistrationParameters << "'";
  if (this->PrintRegistrationSimularityMeasure) of << " PrintRegistrationSimularityMeasure='" << this->PrintRegistrationSimularityMeasure << "'";

  if  (this->RegistrationTranslation[0] || this->RegistrationTranslation[1] || this->RegistrationTranslation[2]) 
    of << " RegistrationTranslation='" << this->RegistrationTranslation[0] << " " << this->RegistrationTranslation[1] << " " << this->RegistrationTranslation[2] << "'";
  if  (this->RegistrationRotation[0] || this->RegistrationRotation[1] || this->RegistrationRotation[2]) 
    of << " RegistrationRotation='" << this->RegistrationRotation[0] << " " << this->RegistrationRotation[1] << " " << this->RegistrationRotation[2] << "'";
  if  ((this->RegistrationScale[0] != 1) || (this->RegistrationScale[1] != 1) || (this->RegistrationScale[2] != 1)) 
    of << " RegistrationScale='" << this->RegistrationScale[0] << " " << this->RegistrationScale[1] << " " << this->RegistrationScale[2] << "'";

  of << " RegistrationCovariance='";
  for(int i=0; i < 9; i++)  of << this->RegistrationCovariance[i] << " ";
  of << "'"; 
  if (this->RegistrationClassSpecificRegistrationFlag) of << " RegistrationClassSpecificRegistrationFlag='" << this->RegistrationClassSpecificRegistrationFlag << "'";
  if (this->ExcludeFromIncompleteEStepFlag) of << " ExcludeFromIncompleteEStepFlag='" << this->ExcludeFromIncompleteEStepFlag << "'";
}

//----------------------------------------------------------------------------
// Copy the node's attributes to this object.
// Does NOT copy: ID, Name
void vtkMrmlSegmenterAtlasGenericClassNode::Copy(vtkMrmlNode *anode)
{
  vtkMrmlNode::MrmlNodeCopy(anode);
  vtkMrmlSegmenterAtlasGenericClassNode *node = (vtkMrmlSegmenterAtlasGenericClassNode *) anode;
  this->Prob = node->Prob;
  this->SetInputChannelWeights(node->InputChannelWeights);
  this->SetLocalPriorWeight(node->LocalPriorWeight);
  this->SetLocalPriorName(node->LocalPriorName); 

  this->PrintWeights                  = node->PrintWeights;
  this->PrintRegistrationParameters   = node->PrintRegistrationParameters;
  this->PrintRegistrationSimularityMeasure         = node->PrintRegistrationSimularityMeasure;
  this->RegistrationClassSpecificRegistrationFlag = node->RegistrationClassSpecificRegistrationFlag;
  this->ExcludeFromIncompleteEStepFlag = node->ExcludeFromIncompleteEStepFlag;
 
  memcpy(this->RegistrationTranslation,node->RegistrationTranslation,3*sizeof(double));
  memcpy(this->RegistrationRotation, node->RegistrationRotation,3*sizeof(double));
  memcpy(this->RegistrationScale,node->RegistrationScale,3*sizeof(double));
  memcpy(this->RegistrationCovariance,node->RegistrationCovariance,9*sizeof(double));
}

//----------------------------------------------------------------------------
void vtkMrmlSegmenterAtlasGenericClassNode::PrintSelf(ostream& os, vtkIndent indent)
{
  os << indent << "Prob:                               " << this->Prob << "\n"; 
  os << indent << "InputChannelWeights:                " <<
    (this->InputChannelWeights ? this->InputChannelWeights : "(none)") << "\n";

  os << indent << "LocalPriorWeight:                   " << this->LocalPriorWeight << "\n";
  os << indent << "LocalPriorName: " <<
    (this->LocalPriorName ? this->LocalPriorName : "(none)") << "\n";

  os << indent << "PrintWeights:                       " << this->PrintWeights << "\n";
  os << indent << "PrintRegistrationParameters:        " << this->PrintRegistrationParameters << "\n";
  os << indent << "PrintRegistrationSimularityMeasure: " << this->PrintRegistrationSimularityMeasure << "\n";
  os << indent << "RegistrationTranslation:            " << this->RegistrationTranslation[0] << ", " << this->RegistrationTranslation[1] << ", " << this->RegistrationTranslation[2] << "\n" ;
  os << indent << "RegistrationRotation:               " << this->RegistrationRotation[0] << ", " << this->RegistrationRotation[1] << ", " << this->RegistrationRotation[2] << "\n" ;
  os << indent << "RegistrationScale:                  " << this->RegistrationScale[0] << ", " << this->RegistrationScale[1] << ", " << this->RegistrationScale[2] << "\n" ;
  os << indent << "RegistrationCovariance:             " ;
  for (int i = 0 ; i < 9 ; i++)  os << RegistrationCovariance[i] << " "; 
  os << "\n" ;
  os << indent << "RegistrationClassSpecificRegistrationFlag: " << this->RegistrationClassSpecificRegistrationFlag << "\n" ;
  os << indent << "ExcludeFromIncompleteEStepFlag:     " << this->ExcludeFromIncompleteEStepFlag << "\n" ;
}



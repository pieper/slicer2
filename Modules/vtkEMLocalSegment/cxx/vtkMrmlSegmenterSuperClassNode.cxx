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
//#include <stdio.h>
//#include <ctype.h>
//#include <string.h>
//#include <math.h>
#include "vtkMrmlSegmenterSuperClassNode.h"
#include "vtkObjectFactory.h"

//------------------------------------------------------------------------------
vtkMrmlSegmenterSuperClassNode* vtkMrmlSegmenterSuperClassNode::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMrmlSegmenterSuperClassNode");
  if(ret)
  {
    return (vtkMrmlSegmenterSuperClassNode*)ret;
  }
  // If the factory was unable to create the object, then create it here.
  return new vtkMrmlSegmenterSuperClassNode;
}

//----------------------------------------------------------------------------
vtkMrmlSegmenterSuperClassNode::vtkMrmlSegmenterSuperClassNode() { 
  // vtkMrmlNode's attributes => Tabs following sub classes  
  this->Indent     = 1;

  this->PrintEMLabelMapConvergence  = 0;
  this->PrintEMWeightsConvergence = 0;
  this->PrintShapeSimularityMeasure = 0;
  this->PrintMFALabelMapConvergence  = 0;
  this->PrintMFAWeightsConvergence = 0;

  this->StopBiasCalculation = -1;
  this->RegistrationType    = 0;
  this->GenerateBackgroundProbability = 0;
  this->PCAShapeModelType = 0;
  this->RegistrationIndependentSubClassFlag = 0;
  this->AtlasNode = vtkMrmlSegmenterAtlasSuperClassNode::New();

  this->PredefinedLabelID = -1;
}

//----------------------------------------------------------------------------
void vtkMrmlSegmenterSuperClassNode::Write(ofstream& of, int nIndent)
{
  // Write all attributes not equal to their default
  vtkIndent i1(nIndent);
  of << i1 << "<SegmenterSuperClass";
  this->vtkMrmlSegmenterGenericClassNode::Write(of);
  this->AtlasNode->Write(of);

  of << " PrintEMLabelMapConvergence='" << this->PrintEMLabelMapConvergence <<  "'";
  of << " PrintEMWeightsConvergence='" << this->PrintEMWeightsConvergence  <<  "'";
  of << " PrintMFALabelMapConvergence='" << this->PrintMFALabelMapConvergence <<  "'";
  of << " PrintMFAWeightsConvergence='" << this->PrintMFAWeightsConvergence  <<  "'";

  of << " RegistrationType='" << this->RegistrationType << "' ";
  of << " StopStopBiasCalculation='" << this->StopBiasCalculation <<  "'";
  of << " GenerateBackgroundProbability='" << this->GenerateBackgroundProbability <<  "'";
  of << " PrintShapeSimularityMeasure='" << this->PrintShapeSimularityMeasure << "'";
  of << " PCAShapeModelType='" << this->PCAShapeModelType << "'";
  of << " RegistrationIndependentSubClassFlag='" << this->RegistrationIndependentSubClassFlag << "'";
  if (this->PredefinedLabelID > -1) of << " PredefinedLabelID ='" << this->PredefinedLabelID << "'";
  of << ">\n";

}

//----------------------------------------------------------------------------
// Copy the node's attributes to this object.
// Does NOT copy: ID, Name
void vtkMrmlSegmenterSuperClassNode::Copy(vtkMrmlNode *anode)
{
  vtkMrmlSegmenterGenericClassNode::Copy(anode);
  vtkMrmlSegmenterSuperClassNode *node = (vtkMrmlSegmenterSuperClassNode *) anode;
  this->AtlasNode->Copy(node);

  this->PrintEMLabelMapConvergence    = node->PrintEMLabelMapConvergence;
  this->PrintEMWeightsConvergence     = node->PrintEMWeightsConvergence;
  this->PrintMFALabelMapConvergence   = node->PrintMFALabelMapConvergence;
  this->PrintMFAWeightsConvergence    = node->PrintMFAWeightsConvergence;

  this->StopBiasCalculation   = node->StopBiasCalculation;
  this->RegistrationType              = node->RegistrationType;
  this->GenerateBackgroundProbability = node->GenerateBackgroundProbability;
  this->PrintShapeSimularityMeasure   = node->PrintShapeSimularityMeasure;
  this->PCAShapeModelType             = node->PCAShapeModelType;
  this->RegistrationIndependentSubClassFlag = node->RegistrationIndependentSubClassFlag;
  this->PredefinedLabelID        = node->PredefinedLabelID;

}

//----------------------------------------------------------------------------
void vtkMrmlSegmenterSuperClassNode::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkMrmlSegmenterGenericClassNode::PrintSelf(os, indent);
  this->AtlasNode->PrintSelf(os,indent);
  os << indent << "RegistrationType:              " << this->RegistrationType<< "\n" ;
  os << indent << "PrintEMLabelMapConvergence:    " << this->PrintEMLabelMapConvergence << "\n";
  os << indent << "PrintEMWeightsConvergence:     " << this->PrintEMWeightsConvergence << "\n";

  os << indent << "PrintMFALabelMapConvergence:   " << this->PrintMFALabelMapConvergence << "\n";
  os << indent << "PrintMFAWeightsConvergence:    " << this->PrintMFAWeightsConvergence << "\n";
  os << indent << "StopBiasCalculation:           " << this->StopBiasCalculation << "\n";

  os << indent << "GenerateBackgroundProbability: " << this->GenerateBackgroundProbability << "\n";
  os << indent << "PrintShapeSimularityMeasure:   " << this->PrintShapeSimularityMeasure << "\n";
  os << indent << "PCAShapeModelType:             " << this->PCAShapeModelType << "\n";
  os << indent << "RegistrationIndependentSubClassFlag: " << this->RegistrationIndependentSubClassFlag << "\n";
  os << indent << "PredefinedLabelID:             " << this->PredefinedLabelID << "\n";

}

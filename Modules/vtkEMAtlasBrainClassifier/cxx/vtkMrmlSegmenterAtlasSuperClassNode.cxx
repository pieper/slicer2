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
#include "vtkMrmlSegmenterAtlasSuperClassNode.h"
#include "vtkObjectFactory.h"

//------------------------------------------------------------------------------
vtkMrmlSegmenterAtlasSuperClassNode* vtkMrmlSegmenterAtlasSuperClassNode::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMrmlSegmenterAtlasSuperClassNode");
  if(ret)
  {
    return (vtkMrmlSegmenterAtlasSuperClassNode*)ret;
  }
  // If the factory was unable to create the object, then create it here.
  return new vtkMrmlSegmenterAtlasSuperClassNode;
}

//----------------------------------------------------------------------------
vtkMrmlSegmenterAtlasSuperClassNode::vtkMrmlSegmenterAtlasSuperClassNode() { 
  // vtkMrmlNode's attributes => Tabs following sub classes  
  this->NumClasses = 0;

  this->PrintFrequency      = 0;
  this->PrintBias           = 0;
  this->PrintLabelMap       = 0;

  this->StopEMType          = 0;
  this->StopEMValue         = 0.0; 
  this->StopEMMaxIter       = 0; 

  this->StopMFAType         = 0;
  this->StopMFAValue        = 0.0; 
  this->StopMFAMaxIter      = 0; 

  this->InitialBiasFilePrefix = NULL;  
  this->PredefinedLabelMapPrefix     = NULL; 
}

//----------------------------------------------------------------------------
vtkMrmlSegmenterAtlasSuperClassNode::~vtkMrmlSegmenterAtlasSuperClassNode() { 
}

//----------------------------------------------------------------------------
void vtkMrmlSegmenterAtlasSuperClassNode::Write(ofstream& of)
{
  // Write all attributes not equal to their defaults
  of << " NumClasses ='" << this->NumClasses << "'";
  of << " PrintBias='" << this->PrintBias << "'";
  of << " PrintLabelMap='" << this->PrintLabelMap << "'";
  of << " PrintFrequency='" << this->PrintFrequency << "'";

  of << " StopEMType='" << this->StopEMType  <<  "'";
  of << " StopEMValue='" << this->StopEMValue <<  "'";
  of << " StopEMMaxIter='" << this->StopEMMaxIter <<  "'";

  of << " StopMFAType='" << this->StopMFAType  <<  "'";
  of << " StopMFAValue='" << this->StopMFAValue <<  "'";
  of << " StopMFAMaxIter='" << this->StopMFAMaxIter <<  "'";

  if (this->InitialBiasFilePrefix && strcmp(this->InitialBiasFilePrefix , "")) of << " InitialBiasFilePrefix='" << this->InitialBiasFilePrefix <<  "'";
  if (this->PredefinedLabelMapPrefix && strcmp(PredefinedLabelMapPrefix, "")) of << " PredefinedLabelMapPrefix='" << this-> PredefinedLabelMapPrefix <<  "'" ;
}

//----------------------------------------------------------------------------
// Copy the node's attributes to this object.
// Does NOT copy: ID, Name
void vtkMrmlSegmenterAtlasSuperClassNode::Copy(vtkMrmlNode *anode)
{
  vtkMrmlSegmenterAtlasSuperClassNode *node = (vtkMrmlSegmenterAtlasSuperClassNode *) anode;
  this->NumClasses = node->NumClasses;

  this->PrintBias      = node->PrintBias;
  this->PrintLabelMap  = node->PrintLabelMap;
  this->PrintFrequency = node->PrintFrequency;

  this->StopEMType     = node->StopEMType;
  this->StopEMValue    = node->StopEMValue; 
  this->StopEMMaxIter  = node->StopEMMaxIter; 

  this->StopMFAType    = node->StopMFAType;
  this->StopMFAValue   = node->StopMFAValue; 
  this->StopMFAMaxIter = node->StopMFAMaxIter; 

  this->InitialBiasFilePrefix =  node->InitialBiasFilePrefix;
  this->PredefinedLabelMapPrefix     =  node->PredefinedLabelMapPrefix;

}

//----------------------------------------------------------------------------
void vtkMrmlSegmenterAtlasSuperClassNode::PrintSelf(ostream& os, vtkIndent indent)
{
  os << indent << "NumClasses:                    "  << this->NumClasses      <<  "\n"; 
  os << indent << "PrintBias:                     " << this->PrintBias << "\n";
  os << indent << "PrintLabelMap:                 " << this->PrintLabelMap << "\n";
  os << indent << "PrintFrequency:                " << this->PrintFrequency << "\n";

  os << indent << "StopEMType:                    " << this->StopEMType  << "\n";
  os << indent << "StopEMValue:                   " << this->StopEMValue << "\n";
  os << indent << "StopEMMaxIter:                 " << this->StopEMMaxIter << "\n";

  os << indent << "StopMFAType:                   " << this->StopMFAType  << "\n";
  os << indent << "StopMFAValue:                  " << this->StopMFAValue << "\n";
  os << indent << "StopMFAMaxIter:                " << this->StopMFAMaxIter << "\n";

  os << indent << "InitialBiasFilePrefix:        " << (this->InitialBiasFilePrefix ? this->InitialBiasFilePrefix : "(none)" ) << "\n";
  os << indent << "PredefinedLabelMapPrefix:            " << (this->PredefinedLabelMapPrefix ? this->PredefinedLabelMapPrefix : "(none)" ) << "\n";


}

/*
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

  vtkIndent i1(nIndent);
  of << i1 << "<SegmenterSuperClass";

  this->vtkMrmlSegmenterAtlasGenericClassNode::Write(of,nIndent);
  of << " RegistrationType='" << this->RegistrationType << "' ";

  of << " PrintEMLabelMapConvergence='" << this->PrintEMLabelMapConvergence <<  "'";
  of << " PrintEMWeightsConvergence='" << this->PrintEMWeightsConvergence  <<  "'";

  of << " PrintMFALabelMapConvergence='" << this->PrintMFALabelMapConvergence <<  "'";
  of << " PrintMFAWeightsConvergence='" << this->PrintMFAWeightsConvergence  <<  "'";

  of << " StopStopBiasCalculation='" << this->StopBiasCalculation <<  "'";
  of << " GenerateBackgroundProbability='" << this->GenerateBackgroundProbability <<  "'";
  of << " PrintShapeSimularityMeasure='" << this->PrintShapeSimularityMeasure << "'";
  of << " PCAShapeModelType='" << this->PCAShapeModelType << "'";
  of << " RegistrationIndependentSubClassFlag='" << this->RegistrationIndependentSubClassFlag << "'";
  of << ">\n";

  vtkMrmlNode::MrmlNodeCopy(anode);
  vtkMrmlSegmenterAtlasGenericClassNode::Copy(anode);

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

  vtkMrmlNode::PrintSelf(os,indent);
  os << indent << "Name:                          " << (this->Name ? this->Name : "(none)") << "\n";
  this->vtkMrmlSegmenterAtlasGenericClassNode::PrintSelf(os, indent);
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

 */

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
  // vtkMrmlNode's attributes => Tabs following sub classes  
  this->Indent     = 1;
  this->Label            = 0;

  this->ShapeParameter   = 0.0;

  this->LogMean          = NULL;
  this->LogCovariance    = NULL;

  this->PCAMeanName      = NULL; 
  this->ReferenceStandardFileName     = NULL; 
  
  this->PCALogisticSlope = 1.0;
  this->PCALogisticMin   = 0.0;
  this->PCALogisticMax   = 20.0;
  this->PCALogisticBoundary = 9.5;


  this->PrintQuality        = 0;
  this->PrintPCA            = 0;
}

//----------------------------------------------------------------------------
vtkMrmlSegmenterClassNode::~vtkMrmlSegmenterClassNode()
{
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
  
  if (this->PCAMeanName)
  {
    delete [] this->PCAMeanName;
    this->PCAMeanName = NULL;
  }

  if (this->ReferenceStandardFileName)
  {
    delete [] this->ReferenceStandardFileName;
    this->ReferenceStandardFileName = NULL;
  }


}

//----------------------------------------------------------------------------
void vtkMrmlSegmenterClassNode::Write(ofstream& of, int nIndent)
{
  // Write all attributes not equal to their defaults
  
  vtkIndent i1(nIndent);

  of << i1 << "<SegmenterClass";
  if (this->Name && strcmp(this->Name, ""))  {
    of << " name ='" << this->Name << "'";
  }
  of << " Label='" << this->Label << "'";

  this->vtkMrmlSegmenterGenericClassNode::Write(of,nIndent);

  of << " ShapeParameter='" << this->ShapeParameter << "'";
  
  if (this->LogMean && strcmp(this->LogMean, "")) 
  {
    of << " LogMean='" << this->LogMean << "'";
  }
  if (this->LogCovariance && strcmp(this->LogCovariance, "")) 
  {
    of << " LogCovariance='" << this->LogCovariance << "'";
  }

  if (this->PCAMeanName && strcmp( this->PCAMeanName, "")) 
  {
    of << " PCAMeanName='" << this->PCAMeanName << "'";
  }

  if (this->ReferenceStandardFileName && strcmp(this->ReferenceStandardFileName, "")) 
  {
    of << " ReferenceStandardFileName='" << this->ReferenceStandardFileName << "'";
  }

  of << " PCALogisticSlope ='" << this->PCALogisticSlope << "' "; 
  of << " PCALogisticMin ='" << this->PCALogisticMin << "' "; 
  of << " PCALogisticMax ='" << this->PCALogisticMax << "' "; 
  of << " PCALogisticBoundary ='" << this->PCALogisticBoundary << "' "; 

  of << " PrintQuality='" << this->PrintQuality << "' ";
  of << " PrintPCA='" << this->PrintPCA << "' ";

  of << ">\n";
}

//----------------------------------------------------------------------------
// Copy the node's attributes to this object.
// Does NOT copy: ID, Name
void vtkMrmlSegmenterClassNode::Copy(vtkMrmlNode *anode)
{
  vtkMrmlNode::MrmlNodeCopy(anode);
  vtkMrmlSegmenterGenericClassNode::Copy(anode);

  vtkMrmlSegmenterClassNode *node = (vtkMrmlSegmenterClassNode *) anode;

  this->SetLabel(node->Label);
  this->SetShapeParameter(node->ShapeParameter);
  this->SetLogMean(node->LogMean);
  this->SetLogCovariance(node->LogCovariance);
  this->SetPCAMeanName(node->PCAMeanName);
  this->SetReferenceStandardFileName(node->ReferenceStandardFileName);

  this->SetPCALogisticSlope(node->PCALogisticSlope);
  this->SetPCALogisticMin(node->PCALogisticMin);
  this->SetPCALogisticMax(node->PCALogisticMax);
  this->SetPCALogisticBoundary(node->PCALogisticBoundary);

  this->SetPrintQuality(node->PrintQuality);
  this->SetPrintPCA(node->PrintPCA);
}

//----------------------------------------------------------------------------
void vtkMrmlSegmenterClassNode::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkMrmlNode::PrintSelf(os,indent);
   os << indent << "Name: " <<
    (this->Name ? this->Name : "(none)") << "\n";
   os << indent << "Label: " << this->Label << "\n";
  this->vtkMrmlSegmenterGenericClassNode::PrintSelf(os, indent);

   os << indent << "ShapeParameter: " << this->ShapeParameter << "\n";

   os << indent << "LogMean: " <<
    (this->LogMean ? this->LogMean : "(none)") << "\n";
   os << indent << "LogCovariance: " <<
    (this->LogCovariance ? this->LogCovariance : "(none)") << "\n";

   os << indent << "ReferenceStandardFileName: " <<  (this->ReferenceStandardFileName ? this->ReferenceStandardFileName : "(none)") << "\n"; 
   os << indent << "PCAMeanName:               " <<  (this->PCAMeanName ? this->PCAMeanName : "(none)") << "\n"; 

   os << indent << "PrintQuality:              " << this->PrintQuality << "\n";
   os << indent << "PrintPCA:                  " << this->PrintPCA << "\n";
   os << indent << "PCALogisticSlope:          " << this->PCALogisticSlope << "\n"; 
   os << indent << "PCALogisticMin:            " << this->PCALogisticMin << "\n"; 
   os << indent << "PCALogisticMax:            " << this->PCALogisticMax << "\n"; 
   os << indent << "PCALogisticBoundary:       " << this->PCALogisticBoundary << "\n"; 

}

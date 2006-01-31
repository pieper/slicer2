/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMRMLVolumeNode.cxx,v $
  Date:      $Date: 2006/01/31 12:34:01 $
  Version:   $Revision: 1.2 $

=========================================================================auto=*/

#include <string>
#include <ostream>

#include "vtkObjectFactory.h"
#include "vtkMRMLVolumeNode.h"

// Initialize static member that controls resampling -- 
  // old comment: "This offset will be changed to 0.5 from 0.0 per 2/8/2002 Slicer 
  // development meeting, to move ijk coordinates to voxel centers."


//------------------------------------------------------------------------------
vtkMRMLVolumeNode* vtkMRMLVolumeNode::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMRMLVolumeNode");
  if(ret)
  {
    return (vtkMRMLVolumeNode*)ret;
  }
  // If the factory was unable to create the object, then create it here.
  return new vtkMRMLVolumeNode;
}

//----------------------------------------------------------------------------
vtkMRMLVolumeNode::vtkMRMLVolumeNode()
{
  // Strings
  this->FileArcheType = NULL;
  this->LUTName = NULL;
  this->ScanOrder = NULL;

  // Numbers
  this->FileScalarType = VTK_SHORT;
  this->FileNumberOfScalarComponents = 1;
  this->LabelMap = 0;
  this->Interpolate = 1;
  this->FileLittleEndian = 0;

  this->AutoWindowLevel = 1;
  this->Window = 256;
  this->Level = 128;
  this->AutoThreshold = 0;
  this->ApplyThreshold = 0;
  this->LowerThreshold = VTK_SHORT_MIN;
  this->UpperThreshold = VTK_SHORT_MAX;

  memset(this->FileDimensions,0,2*sizeof(int));
  memset(this->FileSpacing,0,3*sizeof(vtkFloatingPointType));

  // ScanOrder can never be NULL
  this->ScanOrder = new char[3];
  strcpy(this->ScanOrder, "LR");

  // Matrices
  this->IjkToRasMatrix = vtkMatrix4x4::New();

  // Initialize 
  this->SetFileDimensions(256, 256, 256);
  this->SetFileSpacing(0.9375, 0.9375, 1.5);

}

//----------------------------------------------------------------------------
vtkMRMLVolumeNode::~vtkMRMLVolumeNode()
{
  this->IjkToRasMatrix->Delete();

  if (this->FileArcheType)
  {
    delete [] this->FileArcheType;
    this->FileArcheType = NULL;
  }
  if (this->LUTName)
  {
    delete [] this->LUTName;
    this->LUTName = NULL;
  }
  if (this->ScanOrder)
  {
    delete [] this->ScanOrder;
    this->ScanOrder = NULL;
  }

}

//----------------------------------------------------------------------------
char* vtkMRMLVolumeNode::GetFileScalarTypeAsString()
{
  switch (this->FileScalarType)
  {
  case VTK_VOID:           return "Void"; break;
  case VTK_BIT:            return "Bit"; break;
  case VTK_CHAR:           return "Char"; break;
  case VTK_UNSIGNED_CHAR:  return "UnsignedChar"; break;
  case VTK_SHORT:          return "Short"; break;
  case VTK_UNSIGNED_SHORT: return "UnsignedShort"; break;
  case VTK_INT:            return "Int"; break;
  case VTK_UNSIGNED_INT:   return "UnsignedInt"; break;
  case VTK_LONG:           return "Long"; break;
  case VTK_UNSIGNED_LONG:  return "UnsignedLong"; break;
  case VTK_FLOAT:          return "Float"; break;
  case VTK_DOUBLE:         return "Double"; break;
  }
  return "Short";
}

//----------------------------------------------------------------------------
void vtkMRMLVolumeNode::WriteXML(ostream& of, int nIndent)
{
  vtkErrorMacro("NOT IMPLEMENTED YET");
}

//----------------------------------------------------------------------------
void vtkMRMLVolumeNode::ReadXMLAttributes(const char** atts)
{
  vtkErrorMacro("NOT IMPLEMENTED YET");
}

//----------------------------------------------------------------------------
// Copy the node's attributes to this object.
// Does NOT copy: ID, FilePrefix, Name, VolumeID
void vtkMRMLVolumeNode::Copy(vtkMRMLNode *anode)
{
  vtkMRMLNode::Copy(anode);
  vtkMRMLVolumeNode *node = (vtkMRMLVolumeNode *) anode;

  // Strings
  this->SetFileArcheType(node->FileArcheType);
  this->SetLUTName(node->LUTName);
  this->SetScanOrder(node->ScanOrder);

  // Vectors
  this->SetFileSpacing(node->FileSpacing);
  this->SetFileDimensions(node->FileDimensions);
  
  // Numbers
  this->SetLabelMap(node->LabelMap);
  this->SetFileLittleEndian(node->FileLittleEndian);
  this->SetFileScalarType(node->FileScalarType);
  this->SetFileNumberOfScalarComponents(node->FileNumberOfScalarComponents);
  this->SetAutoWindowLevel(node->AutoWindowLevel);
  this->SetWindow(node->Window);
  this->SetLevel(node->Level);
  this->SetAutoThreshold(node->AutoThreshold);
  this->SetApplyThreshold(node->ApplyThreshold);
  this->SetUpperThreshold(node->UpperThreshold);
  this->SetLowerThreshold(node->LowerThreshold);
  this->SetInterpolate(node->Interpolate);

  // Matrices
  this->IjkToRasMatrix->DeepCopy(node->IjkToRasMatrix);
}

//----------------------------------------------------------------------------
void vtkMRMLVolumeNode::PrintSelf(ostream& os, vtkIndent indent)
{
  int idx;
  
  vtkMRMLNode::PrintSelf(os,indent);

  os << indent << "FileArcheType: " <<
    (this->FileArcheType ? this->FileArcheType : "(none)") << "\n";
  os << indent << "ScanOrder: " <<
    (this->ScanOrder ? this->ScanOrder : "(none)") << "\n";
  os << indent << "LUTName: " <<
    (this->LUTName ? this->LUTName : "(none)") << "\n";

  os << indent << "LabelMap:          " << this->LabelMap << "\n";
  os << indent << "FileLittleEndian:  " << this->FileLittleEndian << "\n";
  os << indent << "FileScalarType:    " << this->FileScalarType << "\n";
  os << indent << "FileNumberOfScalarComponents:  " << this->FileNumberOfScalarComponents << "\n";
  os << indent << "AutoWindowLevel:   " << this->AutoWindowLevel << "\n";
  os << indent << "Window:            " << this->Window << "\n";
  os << indent << "Level:             " << this->Level << "\n";
  os << indent << "AutoThreshold:     " << this->AutoThreshold << "\n";
  os << indent << "ApplyThreshold:    " << this->ApplyThreshold << "\n";
  os << indent << "UpperThreshold:    " << this->UpperThreshold << "\n";
  os << indent << "LowerThreshold:    " << this->LowerThreshold << "\n";
  os << indent << "Interpolate:       " << this->Interpolate << "\n";

  os << "FileSpacing:\n";
  for (idx = 0; idx < 3; ++idx)
  {
    os << indent << ", " << this->FileSpacing[idx];
  }
  os << ")\n";

  os << "FileDimensions:\n";
  for (idx = 0; idx < 3; ++idx)
  {
    os << indent << ", " << this->FileDimensions[idx];
  }
  os << ")\n";

  // Matrices
  os << indent << "IjkToRasMatrix:\n";
    this->IjkToRasMatrix->PrintSelf(os, indent.GetNextIndent());  

}

//----------------------------------------------------------------------------
void vtkMRMLVolumeNode::ComputeIjkToRasFromScanOrder(char *order, vtkMatrix4x4 *IjkToRas)
{
  std::cerr << "NOT IMPLEMENTED YET" << std::endl;
}

char* vtkMRMLVolumeNode::ComputeScanOrderFromIjkToRas(vtkMatrix4x4 *ijkToRas)
{
  vtkFloatingPointType dir[4]={0,0,1,0};
  vtkFloatingPointType kvec[4];
 
  ijkToRas->MultiplyPoint(dir,kvec);
  int max_comp = 0;
  double max = fabs(kvec[0]);
  
  for (int i=1; i<3; i++) {
    if (fabs(kvec[i]) > max) {
        max = fabs(kvec[i]);
        max_comp=i;
     }   
  }
  
  switch(max_comp) {
     case 0:
        if (kvec[max_comp] > 0 ) {
           return "LR";
        } else {
           return "RL";
        }
           break;
      case 1:     
         if (kvec[max_comp] > 0 ) {
            return "PA";
         } else {
            return "AP";
         }
         break;
      case 2:
         if (kvec[max_comp] > 0 ) {
            return "IS";
         } else {
            return "SI";
         }
         break;
  default:
      cerr << "vtkMRMLVolumeNode::ComputeScanOrderFromRasToIjk:\n\tMax components "<< max_comp << " not in valid range 0,1,2\n";
      return "";
   }        
 
}

// End

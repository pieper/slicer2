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
#include "vtkMrmlVolumeNode.h"
#include "vtkObjectFactory.h"

//------------------------------------------------------------------------------
vtkMrmlVolumeNode* vtkMrmlVolumeNode::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMrmlVolumeNode");
  if(ret)
  {
    return (vtkMrmlVolumeNode*)ret;
  }
  // If the factory was unable to create the object, then create it here.
  return new vtkMrmlVolumeNode;
}

//----------------------------------------------------------------------------
vtkMrmlVolumeNode::vtkMrmlVolumeNode()
{
  // vtkMrmlVolumeNode's attributes

  // Strings
  this->FilePattern = NULL;
  this->FilePrefix = NULL;
  this->RasToIjkMatrix = NULL;
  this->RasToVtkMatrix = NULL;
  this->PositionMatrix = NULL;
  this->LUTName = NULL;
  this->FullPrefix = NULL;
  this->ScanOrder = NULL;

  // Numbers
  this->ScalarType = VTK_SHORT;
  this->NumScalars = 1;
  this->LabelMap = 0;
  this->Interpolate = 1;
  this->LittleEndian = 0;
  this->Tilt = 0.0;
  this->AutoWindowLevel = 1;
  this->Window = 256;
  this->Level = 128;
  this->AutoThreshold = 0;
  this->ApplyThreshold = 0;
  this->LowerThreshold = VTK_SHORT_MIN;
  this->UpperThreshold = VTK_SHORT_MAX;
  this->UseRasToVtkMatrix = 1;

  // Arrays
  memset(this->ImageRange,0,2*sizeof(int));
  memset(this->Dimensions,0,2*sizeof(int));
  memset(this->Spacing,0,3*sizeof(float));

  // ScanOrder can never be NULL
  this->ScanOrder = new char[3];
  strcpy(this->ScanOrder, "LR");

  // Matrices
  this->WldToIjk = vtkMatrix4x4::New();
  this->RasToWld = vtkMatrix4x4::New();
  this->RasToIjk = vtkMatrix4x4::New();
  this->Position = vtkMatrix4x4::New();

  // Initialize 
  this->SetImageRange(1, 1);
  this->SetDimensions(256, 256);
  this->SetSpacing(0.9375, 0.9375, 1.5);
  this->ComputeRasToIjkFromScanOrder("LR");

  // Added by Attila Tanacs 10/10/2000

  // DICOMFileNames
  this->DICOMFiles = 0;
  this->DICOMFileList = new char *[500];
  for(int i=0; i<500; i++)
    DICOMFileList[i] = NULL;

  //AddDICOMFileName("first.dcm");
  //AddDICOMFileName("second.dcm");
  
  // Ends
}

//----------------------------------------------------------------------------
vtkMrmlVolumeNode::~vtkMrmlVolumeNode()
{
  this->WldToIjk->Delete();
  this->RasToWld->Delete();
  this->RasToIjk->Delete();
  this->Position->Delete();

  if (this->FilePattern)
  {
    delete [] this->FilePattern;
    this->FilePattern = NULL;
  }
  if (this->FilePrefix)
  {
    delete [] this->FilePrefix;
    this->FilePrefix = NULL;
  }
  if (this->RasToVtkMatrix)
  {
    delete [] this->RasToVtkMatrix;
    this->RasToVtkMatrix = NULL;
  }
  if (this->RasToIjkMatrix)
  {
    delete [] this->RasToIjkMatrix;
    this->RasToIjkMatrix = NULL;
  }
  if (this->PositionMatrix)
  {
    delete [] this->PositionMatrix;
    this->PositionMatrix = NULL;
  }
  if (this->LUTName)
  {
    delete [] this->LUTName;
    this->LUTName = NULL;
  }
  if (this->FullPrefix)
  {
    delete [] this->FilePrefix;
    this->FilePrefix = NULL;
  }
  if (this->ScanOrder)
  {
    delete [] this->ScanOrder;
    this->ScanOrder = NULL;
  }

  // Added by Attila Tanacs 10/10/2000

  for(int i=0; i<500; i++)
    delete [] DICOMFileList[i];

  // End
}


//----------------------------------------------------------------------------
char* vtkMrmlVolumeNode::GetScalarTypeAsString()
{
  switch (this->ScalarType)
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
void vtkMrmlVolumeNode::Write(ofstream& of, int nIndent)
{
  // Write all attributes not equal to their defaults
  
  // If the description is blank, set it to the scan order
  if (this->Description && (strcmp(this->Description, "") == 0))
  {
    delete [] this->Description;
    this->Description = NULL;
  }
  if (this->Description == NULL)
  {
    this->Description = new char[3];
    strcpy(this->Description, this->ScanOrder);
  }

  vtkIndent i1(nIndent);

  of << i1 << "<Volume";
  
  // Strings
  if (this->Name && strcmp(this->Name, "")) 
  {
    of << " name='" << this->Name << "'";
  }
  if (this->FilePattern && strcmp(this->FilePattern, "")) 
  {
    of << " filePattern='" << this->FilePattern << "'";
  }
  if (this->FilePrefix && strcmp(this->FilePrefix, "")) 
  {
    of << " filePrefix='" << this->FilePrefix << "'";
  }

  // >> AT 4/2/01

  if(this->GetNumberOfDICOMFiles() > 0)
    {
      of << " dicomFileNameList='";
      int i;
      int num = GetNumberOfDICOMFiles();
      for(i = 0; i < num; i++)
	{
	  if(i > 0)
	    of << " ";
	  of << GetDICOMFileName(i);
	}
      of << "'";
    }

  // << AT 4/2/01

  if (this->RasToIjkMatrix && strcmp(this->RasToIjkMatrix, "")) 
  {
    of << " rasToIjkMatrix='" << this->RasToIjkMatrix << "'";
  }
  if (this->RasToVtkMatrix && strcmp(this->RasToVtkMatrix, "")) 
  {
    of << " rasToVtkMatrix='" << this->RasToVtkMatrix << "'";
  }
  if (this->PositionMatrix && strcmp(this->PositionMatrix, "")) 
  {
    of << " positionMatrix='" << this->PositionMatrix << "'";
  }
  if (this->ScanOrder && strcmp(this->ScanOrder, "LR")) 
  {
    of << " scanOrder='" << this->ScanOrder << "'";
  }
  if (this->Description && strcmp(this->Description, "")) 
  {
    of << " description='" << this->Description << "'";
  }
  if (this->LUTName && strcmp(this->LUTName, "")) 
  {
    of << " colorLUT='" << this->LUTName << "'";
  }

  // Numbers
  char *scalarType = this->GetScalarTypeAsString();
  if (strcmp(scalarType, "Short")) 
  {
    of << " scalarType='" << scalarType << "'";
  }
  if (this->NumScalars != 1)
  {
    of << " numScalars='" << this->NumScalars << "'";
  }
  if (this->LabelMap != 0)
  {
    of << " labelMap='" << (this->LabelMap ? "yes" : "no") << "'";
  }
  if (this->Interpolate != 1)
  {
    of << " interpolate='" << (this->Interpolate ? "yes" : "no") << "'";
  }
  if (this->LittleEndian != 0)
  {
    of << " littleEndian='" << (this->LittleEndian ? "yes" : "no") << "'";
  }
  if (this->Tilt != 0.0)
  {
    of << " tilt='" << this->Tilt << "'";
  }
  if (this->AutoWindowLevel != 1)
  {
    of << " autoWindowLevel='" << (this->AutoWindowLevel ? "yes" : "no") << "'";
  }
  if (this->Window != 256)
  {
    of << " window='" << this->Window << "'";
  }
  if (this->Level != 128)
  {
    of << " level='" << this->Level << "'";
  }
  if (this->AutoThreshold != 0)
  {
    of << " autoThreshold='" << (this->AutoThreshold ? "yes" : "no") << "'";
  }
  if (this->ApplyThreshold != 0)
  {
    of << " applyThreshold='" << (this->ApplyThreshold ? "yes" : "no") << "'";
  }
  if (this->LowerThreshold != VTK_SHORT_MIN)
  {
    of << " lowerThreshold='" << this->LowerThreshold << "'";
  }
  if (this->UpperThreshold != VTK_SHORT_MAX)
  {
    of << " upperThreshold='" << this->UpperThreshold << "'";
  }

  // Arrays
  if (this->ImageRange[0] != 1 || this->ImageRange[1] != 1)
  {
    of << " imageRange='" << this->ImageRange[0] << " "
       << this->ImageRange[1] << "'";
  }
  if (this->Dimensions[0] != 256 || this->Dimensions[1] != 256)
  {
    of << " dimensions='" << this->Dimensions[0] << " "
       << this->Dimensions[1] << "'";
  }
  if (this->Spacing[0] != 0.9375 || this->Spacing[1] != 0.9375 ||
      this->Spacing[2] != 1.5)
  {
    of << " spacing='" << this->Spacing[0] << " "
       << this->Spacing[1] << " " << this->Spacing[2] << "'";
  }

  //End
  of << "></Volume>\n";;
}

//----------------------------------------------------------------------------
// Copy the node's attributes to this object.
// Does NOT copy: ID, FilePrefix, Name
void vtkMrmlVolumeNode::Copy(vtkMrmlVolumeNode *node)
{
  vtkMrmlNode::Copy(node);

  // Strings
  this->SetFilePattern(node->FilePattern);
  this->SetRasToIjkMatrix(node->RasToIjkMatrix);
  this->SetRasToVtkMatrix(node->RasToVtkMatrix);
  this->SetPositionMatrix(node->PositionMatrix);
  this->SetLUTName(node->LUTName);
  this->SetScanOrder(node->ScanOrder);

  // Vectors
  this->SetSpacing(node->Spacing);
  this->SetImageRange(node->ImageRange);
  this->SetDimensions(node->Dimensions);
  
  // Numbers
  this->SetTilt(node->Tilt);
	this->SetLabelMap(node->LabelMap);
	this->SetLittleEndian(node->LittleEndian);
	this->SetScalarType(node->ScalarType);
	this->SetNumScalars(node->NumScalars);
	this->SetAutoWindowLevel(node->AutoWindowLevel);
	this->SetWindow(node->Window);
	this->SetLevel(node->Level);
	this->SetAutoThreshold(node->AutoThreshold);
	this->SetApplyThreshold(node->ApplyThreshold);
	this->SetUpperThreshold(node->UpperThreshold);
	this->SetLowerThreshold(node->LowerThreshold);
	this->SetInterpolate(node->Interpolate);

  // Matrices
  this->RasToIjk->DeepCopy(node->RasToIjk);
  this->RasToWld->DeepCopy(node->RasToWld);
  this->WldToIjk->DeepCopy(node->WldToIjk);
  this->Position->DeepCopy(node->WldToIjk);
}

//----------------------------------------------------------------------------
void vtkMrmlVolumeNode::SetScanOrder(char *s)
{
  if (s == NULL)
  {
    vtkErrorMacro(<< "SetScanOrder: order string cannot be NULL");
    return;
  }
  if (!strcmp(s,"SI") || !strcmp(s,"IS") || !strcmp(s,"LR") || 
      !strcmp(s,"RL") || !strcmp(s,"AP") || !strcmp(s,"PA")) 
  { 
    if (this->ScanOrder)
    {
      delete [] this->ScanOrder; 
    }
    this->ScanOrder = new char[strlen(s)+1];
    strcpy(this->ScanOrder, s);
    this->Modified();
  }
  else
  {
    vtkErrorMacro(<< "SetScanOrder: must be SI,IS,LR,RL,AP,or PA");
  }
}

//----------------------------------------------------------------------------
void vtkMrmlVolumeNode::SetRasToWld(vtkMatrix4x4 *rasToWld)
{
  // Store RasToWld for posterity and because modern computer systems
  // have lots of memory.
  this->RasToWld->DeepCopy(rasToWld);

  // Convert RasToIjk from string to matrix form
  if (this->UseRasToVtkMatrix)
  {
    SetMatrixToString(this->RasToIjk, this->RasToVtkMatrix);
  }
  else 
  {
    SetMatrixToString(this->RasToIjk, this->RasToIjkMatrix);
  }
  SetMatrixToString(this->Position, this->PositionMatrix);

  // Form WldToIjk matrix to pass to reformatter
  // --------------------------------------------

  // Form  wldToIjk = RasToIjk * WldToRas 
  //                = RasToIjk * Inv(RasToWld)

	// Set the vtkTransform to PostMultiply so a concatenated matrix, C,
	// is multiplied by the existing matrix, M, to form M`= C*M (not M*C)
  vtkTransform *transform = vtkTransform::New();
  transform->PostMultiply();

  // Invert RasToWld and concatenate it onto the WldToIjk.
  transform->SetMatrix(*rasToWld);
  transform->Inverse();
 
  transform->Concatenate(this->RasToIjk);
  
  transform->GetMatrix(this->WldToIjk);
  // This line is necessary after the transform->GetMatrix call
  // to force reformatters to update
  this->WldToIjk->Modified();
  
  transform->Delete();
}

//----------------------------------------------------------------------------
void vtkMrmlVolumeNode::PrintSelf(ostream& os, vtkIndent indent)
{
  int idx;
  
  vtkMrmlNode::PrintSelf(os,indent);

  os << indent << "Name: " <<
    (this->Name ? this->Name : "(none)") << "\n";
  os << indent << "FilePattern: " <<
    (this->FilePattern ? this->FilePattern : "(none)") << "\n";
  os << indent << "FilePrefix: " <<
    (this->FilePrefix ? this->FilePrefix : "(none)") << "\n";
  os << indent << "RasToIjkMatrix: " <<
    (this->RasToIjkMatrix ? this->RasToIjkMatrix : "(none)") << "\n";
  os << indent << "RasToVtkMatrix: " <<
    (this->RasToVtkMatrix ? this->RasToVtkMatrix : "(none)") << "\n";
  os << indent << "PositionMatrix: " <<
    (this->PositionMatrix ? this->PositionMatrix : "(none)") << "\n";
  os << indent << "ScanOrder: " <<
    (this->ScanOrder ? this->ScanOrder : "(none)") << "\n";
  os << indent << "LUTName: " <<
    (this->LUTName ? this->LUTName : "(none)") << "\n";

  os << indent << "LabelMap:          " << this->LabelMap << "\n";
  os << indent << "LittleEndian:      " << this->LittleEndian << "\n";
  os << indent << "ScalarType:        " << this->ScalarType << "\n";
  os << indent << "NumScalars:        " << this->NumScalars << "\n";
  os << indent << "Tilt:              " << this->Tilt << "\n";
  os << indent << "AutoWindowLevel:   " << this->AutoWindowLevel << "\n";
  os << indent << "Window:            " << this->Window << "\n";
  os << indent << "Level:             " << this->Level << "\n";
  os << indent << "AutoThreshold:     " << this->AutoThreshold << "\n";
  os << indent << "ApplyThreshold:    " << this->ApplyThreshold << "\n";
  os << indent << "UpperThreshold:    " << this->UpperThreshold << "\n";
  os << indent << "LowerThreshold:    " << this->LowerThreshold << "\n";
  os << indent << "Interpolate:       " << this->Interpolate << "\n";
  os << indent << "UseRasToVtkMatrix: " << this->UseRasToVtkMatrix << "\n";

  os << "Spacing:\n";
  for (idx = 0; idx < 3; ++idx)
  {
    os << indent << ", " << this->Spacing[idx];
  }
  os << ")\n";

  os << "ImageRange:\n";
  for (idx = 0; idx < 2; ++idx)
  {
    os << indent << ", " << this->ImageRange[idx];
  }
  os << ")\n";

  os << "Dimensions:\n";
  for (idx = 0; idx < 2; ++idx)
  {
    os << indent << ", " << this->Dimensions[idx];
  }
  os << ")\n";

  // Matrices
  os << indent << "RasToWld:\n";
    this->RasToWld->PrintSelf(os, indent.GetNextIndent());  
  os << indent << "RasToIjk:\n";
    this->RasToIjk->PrintSelf(os, indent.GetNextIndent());  
  os << indent << "WldToIjk:\n";
    this->WldToIjk->PrintSelf(os, indent.GetNextIndent());  
  os << indent << "Position:\n";
    this->Position->PrintSelf(os, indent.GetNextIndent());  

  // Added by Attila Tanacs 10/10/2000
    os << indent << "Number of DICOM Files: " << GetNumberOfDICOMFiles() << "\n";
    for(idx = 0; idx < DICOMFiles; idx++)
      os << indent << DICOMFileList[idx] << "\n";
  // End
}

//----------------------------------------------------------------------------
static void MakePermuteMatrix(vtkMatrix4x4 *mat, char *order)
{
	int orient = 0;

  // Orient:
  //  1 = Axial
  //  2 = Sagittal
  //  3 = Coronal
  //
  if (!strcmp(order,"SI") || !strcmp(order,"IS"))
  {
    orient = 1;
  }
  if (!strcmp(order,"LR") || !strcmp(order,"RL"))
  {
    orient = 2;
  }
  if (!strcmp(order,"PA") || !strcmp(order,"AP"))
  {
    orient = 3;
  }

  // IS = [-x  y  z]
  // LR = [-y  z  x]
  // PA = [-x  z  y]
  //
  // That is, the IStoXYZ matrix has column vectors [-x y z]
  //
  // IS = -1  0  0  0   LR =  0  0  1  0   PA = -1  0  0  0
  //       0  1  0  0        -1  0  0  0         0  0  1  0
  //       0  0  1  0         0  1  0  0         0  1  0  0
  //       0  0  0  1         0  0  0  0         0  0  0  0
  //
  switch (orient)
  {
    // Axial
    case 1:
      // Top Row
      mat->SetElement(0, 0, -1.0); mat->SetElement(0, 1,  0.0);
      mat->SetElement(0, 2,  0.0); mat->SetElement(0, 3,  0.0);
      // 2nd Row
      mat->SetElement(1, 0,  0.0); mat->SetElement(1, 1,  1.0);
      mat->SetElement(1, 2,  0.0); mat->SetElement(1, 3,  0.0);
      // 3rd Row
      mat->SetElement(2, 0,  0.0); mat->SetElement(2, 1,  0.0);
      mat->SetElement(2, 2,  1.0); mat->SetElement(2, 3,  0.0);
      break;

    // Sagittal
    case 2:
      mat->SetElement(0, 0,  0.0); mat->SetElement(0, 1,  0.0);
      mat->SetElement(0, 2,  1.0); mat->SetElement(0, 3,  0.0);

      mat->SetElement(1, 0, -1.0); mat->SetElement(1, 1,  0.0);
      mat->SetElement(1, 2,  0.0); mat->SetElement(1, 3,  0.0);

      mat->SetElement(2, 0,  0.0); mat->SetElement(2, 1,  1.0);
      mat->SetElement(2, 2,  0.0); mat->SetElement(2, 3,  0.0);
      break;

    // Coronal
    case 3:
      mat->SetElement(0, 0, -1.0); mat->SetElement(0, 1,  0.0);
      mat->SetElement(0, 2,  0.0); mat->SetElement(0, 3,  0.0);

      mat->SetElement(1, 0,  0.0); mat->SetElement(1, 1,  0.0);
      mat->SetElement(1, 2,  1.0); mat->SetElement(1, 3,  0.0);

      mat->SetElement(2, 0,  0.0); mat->SetElement(2, 1,  1.0);
      mat->SetElement(2, 2,  0.0); mat->SetElement(2, 3,  0.0);
    }

    // Bottom row of matrix
      mat->SetElement(3, 0,  0.0); mat->SetElement(3, 1,  0.0);
      mat->SetElement(3, 2,  0.0); mat->SetElement(3, 3,  1.0);

  // Direction of slice aquisition
  double zDir = 1.0;
  if (!strcmp(order,"SI") || !strcmp(order,"RL") || !strcmp(order,"AP"))
  {
    zDir = -1.0;
  }

  // Z-reflect
  vtkTransform *tran = vtkTransform::New();
	// Set the vtkTransform to PostMultiply so a concatenated matrix, C,
	// is multiplied by the existing matrix, M, to form M`= C*M (not M*C)
  tran->PostMultiply();
  tran->Identity();
  // Z:
  tran->Scale(1.0, 1.0, zDir);
  // R:
  tran->Concatenate(mat);
  tran->GetMatrix(mat);
}

//----------------------------------------------------------------------------
static void Normalize(float *a)
{
	float d = sqrt(a[0]*a[0] + a[1]*a[1] + a[2]*a[2]);

	if (d == 0) return;
	a[0] = a[0] / d;
	a[1] = a[1] / d;
	a[2] = a[2] / d;
}

// Cross a = b x c 
//----------------------------------------------------------------------------
static void Cross(float *a, float *b, float *c)
{
  a[0] = b[1]*c[2] - c[1]*b[2];
  a[1] = c[0]*b[2] - b[0]*c[2];
  a[2] = b[0]*c[1] - c[0]*b[1];
}

//----------------------------------------------------------------------------
static void MakeRotateMatrix(vtkMatrix4x4 *mat, float *ftl, float *ftr,
  float *fbr, float *ltl)
{
	float Ux[3], Uy[3], Uz[3];
  
  // Corner point notation:
  //   tl = top-left
  //   tr = top-right
  //   br = bottom-right
	// Convert this to a matrix that rotates the scanner's coordinate
	// frame to that of the volume. The volume frame has axis vectors Ux, Uy, Uz.
	// The final matrix looks like:
	//
	// Ux(r) Uy(r) Uz(r) 0
	// Ux(a) Uy(a) Uz(a) 0
	// Ux(s) Uy(s) Uz(s) 0
	//   0     0     0   1
	
	// Ux = ftr - ftl
	Ux[0] = ftr[0] - ftl[0];
	Ux[1] = ftr[1] - ftl[1];
	Ux[2] = ftr[2] - ftl[2];
	Normalize(Ux);

	// Note: this works for yDir == "1" (Y points up)
	// Uy = ftr - fbr
	Uy[0] = ftr[0] - fbr[0];
	Uy[1] = ftr[1] - fbr[1];
	Uy[2] = ftr[2] - fbr[2];
	Normalize(Uy);

	// Uz = ltl - ftl
	Uz[0] = ltl[0] - ftl[0];
	Uz[1] = ltl[1] - ftl[1];
	Uz[2] = ltl[2] - ftl[2];
	Normalize(Uz);

	mat->SetElement(0, 0, Ux[0]); mat->SetElement(0, 1, Uy[0]);
	mat->SetElement(0, 2, Uz[0]); mat->SetElement(0, 3, 0.0);

	mat->SetElement(1, 0, Ux[1]); mat->SetElement(1, 1, Uy[1]);
	mat->SetElement(1, 2, Uz[1]); mat->SetElement(1, 3, 0.0);

	mat->SetElement(2, 0, Ux[2]); mat->SetElement(2, 1, Uy[2]);
	mat->SetElement(2, 2, Uz[2]); mat->SetElement(2, 3, 0.0);

	mat->SetElement(3, 0, 0.0);   mat->SetElement(3, 1, 0.0);
	mat->SetElement(3, 2, 0.0);   mat->SetElement(3, 3, 1.0);
}

//----------------------------------------------------------------------------
static void MakeVolumeMatrix(vtkMatrix4x4 *mat, vtkMatrix4x4 *matRotate, 
                double yDir, int nx, int ny, int nz, 
                float vx, float vy, float vz, float ox, float oy, float oz, 
                float sh)
{
  vtkTransform *tran = vtkTransform::New();

	// Make RAS->IJK matrix.
	//---------------------------------------------------------------------
	// Center
	// 1 0 0 -nx/2
	// 0 1 0 -ny/2
	// 0 0 1 -nz/2
	// 0 0 0     4

	// Scale3
	// vx  0  0  0
	//  0 vy  0  0
	//  0  0 vz  0
	//  0  0  0  1

	// Shear (from tilt)
	//  1  0  0  0
	//  0  1  0  0
	//  0 sh  1  0
	//  0  0  0  1

	// Set the vtkTransform to PostMultiply so a concatenated matrix, C,
	// is multiplied by the existing matrix, M, to form M`= C*M (not M*C)
	tran->PostMultiply();

	// M = O*R*Y*Sh*S*C  
	// Order of operation: Center, Scale, Shear, Y-Reflect, Rotate, Offset
	
	tran->Identity();
	// C:
	tran->Translate(-nx/2.0, -ny/2.0, -nz/2.0);
	// S:
	tran->Scale(vx, vy, vz);
	// Sh:
	tran->GetMatrixPointer()->SetElement(2, 1, sh);
	// Y:
	tran->Scale(1.0, yDir, 1.0);
	// R:
	tran->Concatenate(matRotate);
	// O:
	tran->Translate(ox, oy, oz);

  tran->Inverse();

  tran->GetMatrix(mat);

  tran->Delete();
}

//----------------------------------------------------------------------------
static void MakePositionMatrix(vtkMatrix4x4 *mat, vtkMatrix4x4 *matRotate, 
                double yDir, int nx, int ny, int nz, 
                float vx, float vy, float vz, float ox, float oy, float oz, 
                float sh)
{
  vtkTransform *tran = vtkTransform::New();

  // The position matrix is a scaledIJK->RAS matrix.
  // The difference between this matrix and the RAS->IJK matrix is:
  //
  // No scale term
  // The matrix is not inverted at the end of this procedure.

	// Make ScaledIJK->RAS matrix.
	//---------------------------------------------------------------------
	// Scaled-Center
	// 1 0 0 -nx/2*vx
	// 0 1 0 -ny/2*vy
	// 0 0 1 -nz/2*vz
	// 0 0 0     4

	// Shear (from tilt)
	//  1  0  0  0
	//  0  1  0  0
	//  0 sh  1  0
	//  0  0  0  1

	// Set the vtkTransform to PostMultiply so a concatenated matrix, C,
	// is multiplied by the existing matrix, M, to form M`= C*M (not M*C)
	tran->PostMultiply();

	// M = O*R*Y*Sh*C  
	// Order of operation: Scaled-Center, Shear, Y-Reflect, Rotate, Offset
	
	tran->Identity();
	// C:
	tran->Translate(-nx/2.0*vx, -ny/2.0*vy, -nz/2.0*vz);
	// Sh:
	tran->GetMatrixPointer()->SetElement(2, 1, sh);
	// Y:
	tran->Scale(1.0, yDir, 1.0);
	// R:
	tran->Concatenate(matRotate);
	// O:
	tran->Translate(ox, oy, oz);

  tran->GetMatrix(mat);

  tran->Delete();
}

//----------------------------------------------------------------------------
void vtkMrmlVolumeNode::ComputeRasToIjk(vtkMatrix4x4 *matRotate, 
                                        float ox, float oy, float oz)
{
  int nx, ny, nz;
  float vx, vy, vz, sh;
  double yDir, zDir;
  vtkMatrix4x4 *mat = vtkMatrix4x4::New();

  char *order = this->GetScanOrder();

  nx = this->Dimensions[0];
  ny = this->Dimensions[1];
  nz = this->ImageRange[1] - this->ImageRange[0] + 1;
  
  vx = this->Spacing[0];
  vy = this->Spacing[1];
  vz = this->Spacing[2];

  if (vz == 0.0 || nz == 0)
  {
    vtkErrorMacro(<< "ComputeRasToIjkFromScanOrder: Slice spacing or number cannot be 0");
    return;
  }

  // Direction of slice aquisition
  zDir = 1.0;
  if (!strcmp(order,"SI") || !strcmp(order,"RL") || !strcmp(order,"AP"))
  {
    zDir = -1.0;
  }

  // Shear term for gantry tilt
  sh = zDir * vy * tan(this->Tilt * atan(1.0) / 45.0) / vz;

  // RAS -> VTK
  yDir = 1.0;

  MakeVolumeMatrix(mat, matRotate, yDir, nx, ny, nz, vx, vy, vz, 
    ox, oy, oz, sh);
  this->SetRasToVtkMatrix(GetMatrixToString(mat));

  // RAS -> IJK
  yDir = -1.0;
  MakeVolumeMatrix(mat, matRotate, yDir, nx, ny, nz, vx, vy, vz, 
    ox, oy, oz, sh);
  this->SetRasToIjkMatrix(GetMatrixToString(mat));

  // Position (rotate, offset)
  yDir = 1.0;
  MakePositionMatrix(mat, matRotate, yDir, nx, ny, nz, vx, vy, vz,
      ox, oy, oz, sh);
  this->SetPositionMatrix(GetMatrixToString(mat));

  mat->Delete();
}

//----------------------------------------------------------------------------
void vtkMrmlVolumeNode::ComputeRasToIjkFromScanOrder(char *order)
{
  float ox, oy, oz;
  vtkMatrix4x4 *matRotate = vtkMatrix4x4::New();

  // The offset from the origin of RAS space to the center of the volume
  ox = 0;
  oy = 0;
  oz = 0;

  this->SetScanOrder(order);

  MakePermuteMatrix(matRotate, this->GetScanOrder());
  this->ComputeRasToIjk(matRotate, ox, oy, oz);

  matRotate->Delete();
}

//----------------------------------------------------------------------------
int vtkMrmlVolumeNode::ComputeRasToIjkFromCorners(
  float *fc, float *ftl, float *ftr, float *fbr,
  float *lc, float *ltl)
{
  vtkMatrix4x4 *matRotate = vtkMatrix4x4::New();
  char *order;
  float ox, oy, oz;
  int orient=0; // axi=1, sag=2, cor=3

	// Determine orientation by looking at these vectors:
  //   top-left  -> top-right
	//   top-right -> bottom-right
	//---------------------------------------------------------------------
  // Check if no corner points in header
  if (ftl[0] == 0 && ftl[1] == 0 && ftl[2] == 1 &&
      ftr[0] == 0 && ftr[1] == 0 && ftr[2] == 0 && 
      fbr[0] == 1 && fbr[1] == 0 && fbr[2] == 0)
  {
    // (probably read a no-header image)
	  return(-1);
  }
  if (ftl[0] != ftr[0] && ftl[1] == ftr[1] && 
      ftl[2] == ftr[2] && ftr[0] == fbr[0] && 
      ftr[1] != fbr[1] && ftr[2] == fbr[2])
  {
    orient = 1;
  }
  else if (ftl[0] == ftr[0] && ftl[1] != ftr[1] && 
           ftl[2] == ftr[2] && ftr[0] == fbr[0] &&
           ftr[1] == fbr[1] && ftr[2] != fbr[2])
  {
    orient = 2;
  }
  else if (ftl[0] != ftr[0] && ftl[1] == ftr[1] && 
			     ftl[2] == ftr[2] && ftr[0] == fbr[0] && 
			     ftr[1] == fbr[1] && ftr[2] != fbr[2])
  {
    orient = 3;
  }
  if (!orient)
  {
    // oblique, so call it axial
	  orient = 1;
  }

	// Determine acquisition order 
	//---------------------------------------------------------------------
  switch (orient)
  {
  // Axial
  case 1:
    if (fc[2] < lc[2])
    {
      this->SetScanOrder("IS");
    }
    else
    {
      this->SetScanOrder("SI");
		}
	  break;

  // Axial
  case 2:
    if (fc[0] < lc[0])
    {
      this->SetScanOrder("LR");
    }
    else
    {
      this->SetScanOrder("RL");
		}
	  break;
  
  // Coronal
  case 3:
    if (fc[1] < lc[1])
    {
      this->SetScanOrder("PA");
    }
    else
    {
      this->SetScanOrder("AP");
		}
	  break;
  }
  order = this->GetScanOrder();

  // Find offset from the origin to the RAS center of the volume
  // by averaging the centers of the first and last slices.
  // Note: we could also average ftl and lbr if centers weren't available.
	//---------------------------------------------------------------------
  ox = (fc[0] + lc[0]) / 2.0;
  oy = (fc[1] + lc[1]) / 2.0;
  oz = (fc[2] + lc[2]) / 2.0;

  MakeRotateMatrix(matRotate, ftl, ftr, fbr, ltl);
  this->ComputeRasToIjk(matRotate, ox, oy, oz);

  matRotate->Delete();
  return(0);
}

// Added by Attila Tanacs 10/10/2000

// DICOMFileList
void vtkMrmlVolumeNode::AddDICOMFileName(char *str)
{
  DICOMFileList[DICOMFiles] = new char [strlen(str) + 1];
  strcpy(DICOMFileList[DICOMFiles], str);
  DICOMFiles++;
}

void vtkMrmlVolumeNode::SetDICOMFileName(int idx, char *str)
{
  delete [] DICOMFileList[idx];
  DICOMFileList[idx] = new char [strlen(str) + 1];
  strcpy(DICOMFileList[idx], str);
}

char *vtkMrmlVolumeNode::GetDICOMFileName(int idx)
{
  return DICOMFileList[idx];
}

void vtkMrmlVolumeNode::DeleteDICOMFileNames()
{
  int i;
  for(i=0; i<500; i++)
    if(DICOMFileList[i] != NULL)
      {
	delete [] DICOMFileList[i];
	DICOMFileList[i] = NULL;
      }

  DICOMFiles = 0;
}

// End

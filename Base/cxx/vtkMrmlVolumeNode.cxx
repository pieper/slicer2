/*=auto=========================================================================
Copyright (c) 1999 Surgical Planning Lab, Brigham and Women's Hospital
 
Direct all questions on this copyright to slicer@ai.mit.edu.
The following terms apply to all files associated with the software unless
explicitly disclaimed in individual files.   

The authors hereby grant permission to use, copy, and distribute this
software and its documentation for any NON-COMMERCIAL purpose, provided
that existing copyright notices are retained verbatim in all copies.
The authors grant permission to modify this software and its documentation 
for any NON-COMMERCIAL purpose, provided that such modifications are not 
distributed without the explicit consent of the authors and that existing
copyright notices are retained in all copies. Some of the algorithms
implemented by this software are patented, observe all applicable patent law.

IN NO EVENT SHALL THE AUTHORS OR DISTRIBUTORS BE LIABLE TO ANY PARTY FOR
DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
OF THE USE OF THIS SOFTWARE, ITS DOCUMENTATION, OR ANY DERIVATIVES THEREOF,
EVEN IF THE AUTHORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

THE AUTHORS AND DISTRIBUTORS SPECIFICALLY DISCLAIM ANY WARRANTIES, INCLUDING,
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
PARTICULAR PURPOSE, AND NON-INFRINGEMENT.  THIS SOFTWARE IS PROVIDED ON AN
'AS IS' BASIS, AND THE AUTHORS AND DISTRIBUTORS HAVE NO OBLIGATION TO PROVIDE
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
  // vtkMrmlNode's attributes
  this->ID = 0;
  this->Description = NULL;
  this->Options = NULL;
  this->Ignore = 0;

  // vtkMrmlVolumeNode's attributes
  this->Name = NULL;
  this->FilePattern = NULL;
  this->FilePrefix = NULL;
  this->FullPrefix = NULL;
  this->RasToIjkMatrix = NULL;
  this->RasToVtkMatrix = NULL;
  this->LUTName = NULL;

  // ScanOrder can never be NULL
  this->ScanOrder = new char[3];
  strcpy(this->ScanOrder, "SI");

  this->WldToIjk = vtkMatrix4x4::New();
  this->RasToWld = vtkMatrix4x4::New();
  this->RasToIjk = vtkMatrix4x4::New();

  memset(this->Spacing,0,3*sizeof(float));
  memset(this->ImageRange,0,2*sizeof(int));
  memset(this->Dimensions,0,2*sizeof(int));

  this->Tilt = 0.0;
  this->LabelMap = 0;
  this->LittleEndian = 0;
  this->ScalarType = VTK_SHORT;
  this->NumScalars = 1;
  this->AutoWindowLevel = 1;
  this->Window = 256;
  this->Level = 128;
  this->AutoThreshold = 0;
  this->UpperThreshold = VTK_SHORT_MAX;
  this->LowerThreshold = VTK_SHORT_MIN;
  this->Interpolate = 1;

  // Initialize to 64x64x1
  this->SetImageRange(1, 1);
  this->SetDimensions(64, 64);
  this->SetSpacing(1.0, 1.0, 1.0);
  this->ComputeRasToIjkFromScanOrder("SI");
}

//----------------------------------------------------------------------------
vtkMrmlVolumeNode::~vtkMrmlVolumeNode()
{
  this->WldToIjk->Delete();
  this->RasToWld->Delete();
  this->RasToIjk->Delete();

  if (this->Name)
  {
    delete [] this->Name;
    this->Name = NULL;
  }
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
  if (this->FullPrefix)
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
  if (this->ScanOrder)
  {
    delete [] this->ScanOrder;
    this->ScanOrder = NULL;
  }
  if (this->LUTName)
  {
    delete [] this->LUTName;
    this->LUTName = NULL;
  }
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
  this->SetScanOrder(node->ScanOrder);
  this->SetLUTName(node->LUTName);

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
	this->SetUpperThreshold(node->UpperThreshold);
	this->SetLowerThreshold(node->LowerThreshold);
	this->SetInterpolate(node->Interpolate);

  // Matrices
  this->RasToIjk->DeepCopy(node->RasToIjk);
  this->RasToWld->DeepCopy(node->RasToWld);
  this->WldToIjk->DeepCopy(node->WldToIjk);
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

  // Form WldToIjk matrix to pass to reformatter
  // --------------------------------------------

  // Form  wldToIjk = RasToIjk * WldToRas 
  //                = RasToIjk * Inv(RasToWld)

  // PostMultiply so concatenating C to M makes M=C*M
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

			mat->SetElement(2, 0,  0.0); mat->SetElement(2, 1, -1.0);
			mat->SetElement(2, 2,  0.0); mat->SetElement(2, 3,  0.0);
  }

		// Bottom row of matrix
		  mat->SetElement(3, 0,  0.0); mat->SetElement(3, 1,  0.0);
		  mat->SetElement(3, 2,  0.0); mat->SetElement(3, 3,  1.0);
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
                double yDir, double zDir, int nx, int ny, int nz, 
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
	// vx  0  0  0112
	//  0 vy  0  0
	//  0  0 vz  0
	//  0  0  0  1

	// Shear (from tilt)
	//  1  0  0  0
	//  0  1  0  0
	//  0 sh  1  0
	//  0  0  0  1

	// Set the vtkTransform to PostMultiply so a concatenated matVol, C,
	// is multiplied by the existing matVol, M: C*M (not M*C)
	tran->PostMultiply();

	// M = O*R*Z*Y*S*C  
	// Order of operation: Center, Scale, Shear, Y-Reflect, Z-Reflect, Rotate, Offset
	
	tran->Identity();
	// C:
	tran->Translate(-nx/2.0, -ny/2.0, -nz/2.0);
	// S:
	tran->Scale(vx, vy, vz);
	// Sh:
	tran->GetMatrixPointer()->SetElement(2, 1, zDir * sh);
	// Y:
	tran->Scale(1.0, yDir, 1.0);
	// Z:
  tran->Scale(1.0, 1.0, zDir);
	// R:
	tran->Concatenate(matRotate);
	// O:
	tran->Translate(ox, oy, oz);

	tran->Inverse();

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
	sh = vy * tan(this->Tilt * atan(1.0) / 45.0) / vz;

  // Direction of slice aquisition
  zDir = 1.0;
	if (!strcmp(order,"SI") || !strcmp(order,"RL") || !strcmp(order,"AP"))
  {
		zDir = -1.0;
	}

	// RAS -> VTK
	yDir = 1.0;
  MakeVolumeMatrix(mat, matRotate, yDir, zDir, nx, ny, nz, vx, vy, vz, 
    ox, oy, oz, sh);
  this->SetRasToVtkMatrix(GetMatrixToString(mat));

	// RAS -> IJK
	yDir = -1.0;
  MakeVolumeMatrix(mat, matRotate, yDir, zDir, nx, ny, nz, vx, vy, vz, 
    ox, oy, oz, sh);
  this->SetRasToIjkMatrix(GetMatrixToString(mat));

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
void vtkMrmlVolumeNode::ComputeRasToIjkFromCorners(
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
    vtkErrorMacro(<< "Compute: Invalid corner points");
    return;
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
}

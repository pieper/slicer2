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
#include "vtkImageReformatIJK.h"
#include "vtkObjectFactory.h"

#define ORDER_IS 0
#define ORDER_SI 1
#define ORDER_LR 2
#define ORDER_RL 3
#define ORDER_PA 4
#define ORDER_AP 5


//------------------------------------------------------------------------------
vtkImageReformatIJK* vtkImageReformatIJK::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkImageReformatIJK");
  if(ret)
    {
    return (vtkImageReformatIJK*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkImageReformatIJK;
}


//----------------------------------------------------------------------------
// Description:
// Constructor sets default values
vtkImageReformatIJK::vtkImageReformatIJK()
{
  this->NumSlices = 0;
  this->Slice = 0;
  memset(this->XStep,  0, 4*sizeof(float));
  memset(this->YStep,  0, 4*sizeof(float));
  memset(this->ZStep,  0, 4*sizeof(float));
  memset(this->Origin, 0, 4*sizeof(float));
  this->Indices = vtkIntArray::New();
  this->WldToIjkMatrix = NULL;

  this->InputOrder = ORDER_SI;
  this->OutputOrder = ORDER_SI;
  this->Modified();
}

vtkImageReformatIJK::~vtkImageReformatIJK()
{
	this->Indices->Delete();
  this->tran->Delete();

  // We must UnRegister any object that has a vtkSetObjectMacro
  if (this->WldToIjkMatrix != NULL) 
  {
    this->WldToIjkMatrix->UnRegister(this);
  }
}

void vtkImageReformatIJK::PrintSelf(ostream& os, vtkIndent indent)
{
	vtkImageToImageFilter::PrintSelf(os,indent);

	os << indent << "YStep[0]:    " << this->YStep[0] << "\n";
	os << indent << "YStep[1]:    " << this->YStep[1] << "\n";
	os << indent << "YStep[2]:    " << this->YStep[2] << "\n";
	os << indent << "XStep[0]:    " << this->XStep[0] << "\n";
	os << indent << "XStep[1]:    " << this->XStep[1] << "\n";
	os << indent << "XStep[2]:    " << this->XStep[2] << "\n";
	os << indent << "ZStep[0]:    " << this->ZStep[0] << "\n";
	os << indent << "ZStep[1]:    " << this->ZStep[1] << "\n";
	os << indent << "ZStep[2]:    " << this->ZStep[2] << "\n";
	os << indent << "Origin[0]:   " << this->Origin[0] << "\n";
	os << indent << "Origin[1]:   " << this->Origin[1] << "\n";
	os << indent << "Origin[2]:   " << this->Origin[2] << "\n";

  // vtkSetObjectMacro
  os << indent << "WldToIjkMatrix: " << this->WldToIjkMatrix << "\n";
  if (this->WldToIjkMatrix)
  {
    this->WldToIjkMatrix->PrintSelf(os,indent.GetNextIndent());
  }
}

void vtkImageReformatIJK::SetInputOrderString(char *str)
{
  if      (strcmp(str, "SI") == 0) this->SetInputOrder(ORDER_SI);
  else if (strcmp(str, "IS") == 0) this->SetInputOrder(ORDER_IS);
  else if (strcmp(str, "LR") == 0) this->SetInputOrder(ORDER_LR);
  else if (strcmp(str, "RL") == 0) this->SetInputOrder(ORDER_RL);
  else if (strcmp(str, "AP") == 0) this->SetInputOrder(ORDER_AP);
  else if (strcmp(str, "PA") == 0) this->SetInputOrder(ORDER_PA);
  else
  {
    vtkWarningMacro(<<"SetInputOrderString: invalid order:"<<str);
  }
}

void vtkImageReformatIJK::SetOutputOrderString(char *str)
{
  if      (strcmp(str, "SI") == 0) this->SetOutputOrder(ORDER_SI);
  else if (strcmp(str, "IS") == 0) this->SetOutputOrder(ORDER_IS);
  else if (strcmp(str, "LR") == 0) this->SetOutputOrder(ORDER_LR);
  else if (strcmp(str, "RL") == 0) this->SetOutputOrder(ORDER_RL);
  else if (strcmp(str, "AP") == 0) this->SetOutputOrder(ORDER_AP);
  else if (strcmp(str, "PA") == 0) this->SetOutputOrder(ORDER_PA);
  else
  {
    vtkWarningMacro(<<"SetOutputOrderString: invalid order:"<<str);
  }
}

void vtkImageReformatIJK::ComputeTransform()
{
  // IS = [-x  y  z]
  // SI = [-x  y -z]
  // LR = [-y  z  x]
  // RL = [-y  z -x]
  // PA = [-x  z  y]
  // AP = [-x  z -y]
  //
  // That is, the SItoXYZ matrix has column vectors [-x y -z]
  // str = col0, col1, ...
  int str[6][16] = {
    {-1, 0, 0, 0,  0, 1, 0, 0,  0, 0, 1, 0,  0, 0, 0, 1},
    {-1, 0, 0, 0,  0, 1, 0, 0,  0, 0,-1, 0,  0, 0, 0, 1},
    { 0,-1, 0, 0,  0, 0, 1, 0,  1, 0, 0, 0,  0, 0, 0, 1},
    { 0,-1, 0, 0,  0, 0, 1, 0, -1, 0, 0, 0,  0, 0, 0, 1},
    {-1, 0, 0, 0,  0, 0, 1, 0,  0, 1, 0, 0,  0, 0, 0, 1},
    {-1, 0, 0, 0,  0, 0, 1, 0,  0,-1, 0, 0,  0, 0, 0, 1},
    };
  int x, y;

  vtkMatrix4x4 *input = vtkMatrix4x4::New();
  for (y=0; y<4; y++)
  {
    for (x=0; x<4; x++)
    {
  		input->SetElement(x, y, (float)str[this->InputOrder][y*4+x]);
    }
  }

  vtkMatrix4x4 *output = vtkMatrix4x4::New();
  for (y=0; y<4; y++)
  {
    for (x=0; x<4; x++)
    {
  		output->SetElement(x, y, (float)str[this->OutputOrder][y*4+x]);
    }
  }

  // InputToOutput = Inv(Output) * Input
  this->tran = vtkTransform::New();
  this->tran->SetMatrix(*output);
  this->tran->Inverse();
  this->tran->PreMultiply();
  this->tran->Concatenate(input);
  // Now convert it to OutputToInput
  this->tran->Inverse();

  input->Delete();
  output->Delete();

  this->TransformTime.Modified();
}

//----------------------------------------------------------------------------
void vtkImageReformatIJK::ComputeOutputExtent()
{
  int i, inExt[6], ijk[3], dot;
  float x[4]={1,0,0,1}, y[4]={0,1,0,1}, z[4]={0,0,1,1};

  this->GetInput()->GetWholeExtent(inExt);

  // Output is XYZ, input is IJK
  // tran: XYZ->IJK
  
	// Convert origin from XYZ to IJK space
	this->tran->MultiplyPoint(x, this->XStep);
	this->tran->MultiplyPoint(y, this->YStep);
	this->tran->MultiplyPoint(z, this->ZStep);

  if (inExt[0] != 0 || inExt[2] != 0 || inExt[4] != 0) 
  {
    vtkErrorMacro(<<"The input extent needs to be 0-based.");
    return;
  }
	
  ijk[0] = inExt[1];
  ijk[1] = inExt[3];
  ijk[2] = inExt[5];

  memset(this->Origin, 0, 4*sizeof(float));

  dot = 0;
  for (i=0; i<3; i++)
  {
    dot += (int) (ijk[i] * this->XStep[i]);
  }
  this->OutputExtent[0] = 0;
  this->OutputExtent[1] = abs(dot);
  if (dot < 0) 
  {
    this->Origin[0] += this->XStep[0] * dot;
    this->Origin[1] += this->XStep[1] * dot;
    this->Origin[2] += this->XStep[2] * dot;
  }

  dot = 0;
  for (i=0; i<3; i++)
  {
    dot += (int) (ijk[i] * this->YStep[i]);
  }
  this->OutputExtent[2] = 0;
  this->OutputExtent[3] = abs(dot);
  if (dot < 0) 
  {
    this->Origin[0] += this->YStep[0] * dot;
    this->Origin[1] += this->YStep[1] * dot;
    this->Origin[2] += this->YStep[2] * dot;
  }

  dot = 0;
  for (i=0; i<3; i++)
  {
    dot += (int) (ijk[i] * this->ZStep[i]);
  }
  this->OutputExtent[4] = 0;
  this->NumSlices = abs(dot)+1;
  this->OutputExtent[5] = 0;
  if (dot < 0) 
  {
    this->Origin[0] += this->ZStep[0] * dot;
    this->Origin[1] += this->ZStep[1] * dot;
    this->Origin[2] += this->ZStep[2] * dot;
  }

  // advance by slice
  this->Origin[0] += this->ZStep[0] * this->Slice;
  this->Origin[1] += this->ZStep[1] * this->Slice;
  this->Origin[2] += this->ZStep[2] * this->Slice;

	// Create array of indices
  int size = (this->OutputExtent[3]-this->OutputExtent[2]+1)*
    (this->OutputExtent[1]-this->OutputExtent[0]+1);
	this->Indices->SetNumberOfComponents(1);
	this->Indices->SetNumberOfValues(size);

}

//----------------------------------------------------------------------------
void vtkImageReformatIJK::SetIJKPoint(int i, int j, int k)
{
  int v, dot, dijk[3], x, y;

  dijk[0] = i - (int) this->Origin[0];
  dijk[1] = j - (int) this->Origin[1];
  dijk[2] = k - (int) this->Origin[2];

  // x = abs(dijk . XStep) 
  dot = 0; 
  for (v=0; v<3; v++)
  {
    dot += (int) (dijk[v] * this->XStep[v]);
  }
  x = abs(dot);

  // y
  dot = 0; 
  for (v=0; v<3; v++)
  {
    dot += (int) (dijk[v] * this->YStep[v]);
  }
  y = abs(dot);

  this->XYPoint[0] = x;
  this->XYPoint[1] = y;
}

//----------------------------------------------------------------------------
void Normalize(float *a)
{
  float d;
  d = sqrt(a[0]*a[0] + a[1]*a[1] + a[2]*a[2]);

  if (d == 0.0) return;

  a[0] = a[0] / d;
  a[1] = a[1] / d;
  a[2] = a[2] / d;
}

//----------------------------------------------------------------------------
// a = b x c
void Cross(float *a, float *b, float *c)
{
  a[0] = b[1]*c[2] - c[1]*b[2];
  a[1] = c[0]*b[2] - b[0]*c[2];
  a[2] = b[0]*c[1] - c[0]*b[1];
}

//----------------------------------------------------------------------------
void vtkImageReformatIJK::ComputeReformatMatrix(vtkMatrix4x4 *ref)
{
  float O[4], X[4], Y[4], C[4];
  float Ux[3], Uy[3], Uz[3];
  int xExt, yExt, zExt, i;
  vtkMatrix4x4 *ijkToRas = vtkMatrix4x4::New();
  vtkTransform *trans = vtkTransform::New();

  if (!(this->WldToIjkMatrix)) 
  {
    vtkErrorMacro(<<"ComputeReformatMatrix: No WldToIjkMatrix");
    return;
  }
  trans->PostMultiply();

  // Find IJK extent
  zExt = this->OutputExtent[5] - this->OutputExtent[4] + 1;
  yExt = this->OutputExtent[3] - this->OutputExtent[2] + 1;
  xExt = this->OutputExtent[1] - this->OutputExtent[0] + 1;

  // Form IJK->RAS matrix
  trans->SetMatrix(*(this->WldToIjkMatrix));
  trans->Inverse();
  trans->GetMatrix(ijkToRas);

	// C (center of output image)
  // C = Origin + Step * Extent/2
  for (i=0; i<3; i++) 
  {
    C[i] = this->Origin[i] + this->XStep[i]*xExt/2 + this->YStep[i]*yExt/2;
  }
  C[3] = 1;
  trans->SetPoint(C);
  trans->GetPoint(C);

	// X (right)
  //
  for (i=0; i<3; i++) 
  {
    X[i] = this->Origin[i] + this->XStep[i];
  }
  X[3] = 1;
  trans->SetPoint(X);
  trans->GetPoint(X);

	// Y (lower)
  for (i=0; i<3; i++) 
  {
    Y[i] = this->Origin[i] + this->YStep[i];
  }
  Y[3] = 1;
  trans->SetPoint(Y);
  trans->GetPoint(Y);

  // O (origin)
  for (i=0; i<3; i++) 
  {
    O[i] = this->Origin[i];
  }
  O[3] = 1;
  trans->SetPoint(O);
  trans->GetPoint(O);

  // Ux (unit vector in x direction)
  // Uy (unit vector in y direction)
  for (i=0; i<3; i++) 
  {
    Ux[i] = X[i] - O[i];
    Uy[i] = Y[i] - O[i];
  }

  // Form Uz
  Cross(Uz, Ux, Uy);
  Normalize(Ux);
  Normalize(Uy);
  Normalize(Uz);

  //fprintf(stderr, "O=%.2f %.2f %.2f\n", O[0], O[1], O[2]);
  //fprintf(stderr, "Ux=%.2f %.2f %.2f\n", Ux[0], Ux[1], Ux[2]);
  //fprintf(stderr, "Uz=%.2f %.2f %.2f\n", Uz[0], Uz[1], Uz[2]);
  
  // Set ReformatMatrix
  for(i=0; i<3; i++) 
  {
    ref->SetElement(i, 0, Ux[i]);
    ref->SetElement(i, 1, Uy[i]);
    ref->SetElement(i, 2, Uz[i]);
    ref->SetElement(i, 3,  C[i]);
  }
  for(i=0; i<3; i++) {
    ref->SetElement(3, i, 0.0);
  }
  ref->SetElement(3, 3, 1.0);

  // cleanup
  ijkToRas->Delete();
  trans->Delete();
}

//----------------------------------------------------------------------------
void vtkImageReformatIJK::ExecuteInformation()
{    
  if (this->GetMTime() > this->TransformTime) 
  {
    this->ComputeTransform();
    this->ComputeOutputExtent();
  }
  this->GetOutput()->SetWholeExtent(this->OutputExtent);
}

//----------------------------------------------------------------------------
void vtkImageReformatIJK::ComputeRequiredInputUpdateExtent(int inExt[6], int outExt[6])
{
    // Use full input extent
    this->GetInput()->GetWholeExtent(inExt);
}

//----------------------------------------------------------------------------
// Description:
// This templated function executes the filter for any type of data.
template <class T>
static void vtkImageReformatIJKExecute(vtkImageReformatIJK *self,
				     vtkImageData *inData, T *inPtr,
				     vtkImageData *outData, T *outPtr, 
				     int outExt[6], int id)
{
	int inIncX, inIncY, inIncZ, outIncX, outIncY, outIncZ;
  int xStep[3], yStep[3], xRewind[3], origin[3];
	int i, x, y, z, nx, ny, nxy, maxX, maxY, idxX, idxY, idx;
	int inExt[6];

	// find the region to loop over
	maxX = outExt[1]-outExt[0]; 
	maxY = outExt[3]-outExt[2];
 
	// Find input dimensions
	inData->GetExtent(inExt);
  nx = inExt[1]-inExt[0]+1;
  ny = inExt[3]-inExt[2]+1;
  nxy=nx*ny;

	// Get increments to march through data 
	outData->GetContinuousIncrements(outExt, outIncX, outIncY, outIncZ);
	inData->GetContinuousIncrements(inExt, inIncX, inIncY, inIncZ);

  // Get pointer to indices
	int *indices = self->GetIndices()->GetPointer(0);

  for (i=0; i<3; i++)
  {
    xStep[i]  = (int)(self->XStep[i]);
    yStep[i]  = (int)(self->YStep[i]);
    origin[i] = (int)(self->Origin[i]);
	
    // rewind steps in x direction
	  xRewind[i] = xStep[i] * (maxX+1);
  }

  x = origin[0];
  y = origin[1];
  z = origin[2];

		// Loop through output pixels
		for (idxY = 0; idxY <= maxY; idxY++)
		{
			for (idxX = 0; idxX <= maxX; idxX++)
			{				
				// Test if coordinates are outside extent
				if ((x < inExt[0]) || (y < inExt[2]) || (z < inExt[4]) ||
					(x > inExt[1]) || (y > inExt[3]) || (z > inExt[5]))
				{
					*outPtr = 0;
					*indices = -1;
				}
				else {
         	idx = z*nxy+y*nx+x;
					*outPtr = inPtr[idx];
         	*indices = idx;
				}
				outPtr++;
        indices++;

				// Step volume coordinates in xs direction
				x += xStep[0];
				y += xStep[1];
				z += xStep[2];
			}
			outPtr  += outIncY;

			// Rewind volume coordinates back to first column
			x -= xRewind[0];
			y -= xRewind[1];
			z -= xRewind[2];

			// Step volume coordinates in ys direction
			x += yStep[0];
			y += yStep[1];
			z += yStep[2];
		}
}

//----------------------------------------------------------------------------
// Description:
// This method is passed a input and output data, and executes the filter
// algorithm to fill the output from the input.
// It just executes a switch statement to call the correct function for
// the datas data types.
void vtkImageReformatIJK::Execute(vtkImageData *inData, vtkImageData *outData)
{
	int outExt[6], id=0;
	this->GetOutput()->GetWholeExtent(outExt);
	void *inPtr = inData->GetScalarPointerForExtent(outExt);
	void *outPtr = outData->GetScalarPointerForExtent(outExt);
  
	switch (inData->GetScalarType())
	{
	case VTK_DOUBLE:
		vtkImageReformatIJKExecute(this, inData, (double *)(inPtr), 
			outData, (double *)(outPtr), outExt, id);
		break;
	case VTK_FLOAT:
		vtkImageReformatIJKExecute(this, inData, (float *)(inPtr), 
			outData, (float *)(outPtr), outExt, id);
		break;
	case VTK_LONG:
		vtkImageReformatIJKExecute(this, inData, (long *)(inPtr), 
			outData, (long *)(outPtr), outExt, id);
		break;
	case VTK_UNSIGNED_LONG:
		vtkImageReformatIJKExecute(this, inData, (unsigned long *)(inPtr), 
			outData, (unsigned long *)(outPtr), outExt, id);
		break;
	case VTK_INT:
		vtkImageReformatIJKExecute(this, inData, (int *)(inPtr), 
			outData, (int *)(outPtr), outExt, id);
		break;
	case VTK_UNSIGNED_INT:
		vtkImageReformatIJKExecute(this, inData, (unsigned int *)(inPtr), 
			outData, (unsigned int *)(outPtr), outExt, id);
		break;
	case VTK_SHORT:
		vtkImageReformatIJKExecute(this, inData, (short *)(inPtr), 
			outData, (short *)(outPtr), outExt, id);
		break;
	case VTK_UNSIGNED_SHORT:
		vtkImageReformatIJKExecute(this, inData, (unsigned short *)(inPtr), 
			outData, (unsigned short *)(outPtr), outExt, id);
		break;
	case VTK_CHAR:
		vtkImageReformatIJKExecute(this, inData, (char *)(inPtr), 
			outData, (char *)(outPtr), outExt, id);
		break;
	case VTK_UNSIGNED_CHAR:
		vtkImageReformatIJKExecute(this, inData, (unsigned char *)(inPtr), 
			outData, (unsigned char *)(outPtr), outExt, id);
		break;
	default:
		vtkErrorMacro(<< "Execute: Unknown input ScalarType");
		return;
	}
}





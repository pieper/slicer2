/*=auto=========================================================================
Copyright (c) 1999 Surgical Planning Lab, Brigham and Women's Hospital
 
Direct all questions on this copyright to slicer@ai.mit.edu.
The following terms apply to all files associated with the software unless
explicitly disclaimed in individual files.   

The authors hereby grant permission to use, copy (but NOT distribute) this
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
#include "vtkImageReformat.h"
#include "vtkTransform.h"
#include "vtkObjectFactory.h"
#include <time.h>
#include <math.h>


//------------------------------------------------------------------------------
vtkImageReformat* vtkImageReformat::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkImageReformat");
  if(ret)
    {
    return (vtkImageReformat*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkImageReformat;
}


//----------------------------------------------------------------------------
// Description:
// Constructor sets default values
vtkImageReformat::vtkImageReformat()
{
	this->InterpolateOn();
	this->ReformatMatrix = NULL;
	this->WldToIjkMatrix = NULL;
	this->Resolution = 256;
	this->FieldOfView = 240.0;
  this->RunTime = 0;
	
  for (int i=0; i<3; i++) 
  {
		this->XStep[i] = 0;
		this->YStep[i] = 0;
		this->Origin[i] = 0;
		this->WldPoint[i] = 0;
		this->IjkPoint[i] = 0;
	}
}

//----------------------------------------------------------------------------
vtkImageReformat::~vtkImageReformat()
{
  // Delete ImageData if self-created or if no one else is using it
  if (this->ReformatMatrix != NULL) 
  {
    // Signal that we're no longer using it
    this->ReformatMatrix->UnRegister(this);
  }
  if (this->WldToIjkMatrix != NULL) 
  {
    this->WldToIjkMatrix->UnRegister(this);
  }
}

void vtkImageReformat::PrintSelf(ostream& os, vtkIndent indent)
{
	vtkImageToImageFilter::PrintSelf(os,indent);
  
	os << indent << "YStep[0]:    " << this->YStep[0] << "\n";
	os << indent << "YStep[1]:    " << this->YStep[1] << "\n";
	os << indent << "YStep[2]:    " << this->YStep[2] << "\n";
	os << indent << "XStep[0]:    " << this->XStep[0] << "\n";
	os << indent << "XStep[1]:    " << this->XStep[1] << "\n";
	os << indent << "XStep[2]:    " << this->XStep[2] << "\n";
	os << indent << "Origin[0]:   " << this->Origin[0] << "\n";
	os << indent << "Origin[1]:   " << this->Origin[1] << "\n";
	os << indent << "Origin[2]:   " << this->Origin[2] << "\n";
	os << indent << "RunTime:     " << this->RunTime << "\n";

	os << indent << "IjkPoint[0]: " << this->IjkPoint[0] << "\n";
	os << indent << "IjkPoint[1]: " << this->IjkPoint[1] << "\n";
	os << indent << "IjkPoint[2]: " << this->IjkPoint[2] << "\n";
	os << indent << "WldPoint[0]: " << this->WldPoint[0] << "\n";
	os << indent << "WldPoint[1]: " << this->WldPoint[1] << "\n";
	os << indent << "WldPoint[2]: " << this->WldPoint[2] << "\n";

  os << indent << "Resolution:  " << this->Resolution << "\n";
  os << indent << "FieldOfView: " << this->FieldOfView << "\n";
  os << indent << "Interpolate: " << this->Interpolate << "\n";
	
  if (this->ReformatMatrix)
  {
    os << indent << "ReformatMatrix:\n";
    this->ReformatMatrix->PrintSelf (os, indent.GetNextIndent ());
  }
  else
  {
    os << indent << "ReformatMatrix: (none)\n";
  }

  if (this->WldToIjkMatrix)
  {
    os << indent << "WldToIjkMatrix:\n";
    this->WldToIjkMatrix->PrintSelf (os, indent.GetNextIndent ());
  }
  else
  {
    os << indent << "WldToIjkMatrix: (none)\n";
  }
}

//----------------------------------------------------------------------------
void vtkImageReformat::ExecuteInformation(vtkImageData *inData, vtkImageData *outData)
{
	int ext[6];
  float pix;

	// Set output to 2D, size specified by user
	ext[1] = this->Resolution - 1;
	ext[3] = this->Resolution - 1;
	ext[0] = ext[2] = ext[5] = ext[4] = 0;

  outData->SetWholeExtent(ext);

  // We don't use these anyway since we handle obliques with the
  // WldToIjkMatrix
  //
	pix = this->FieldOfView / this->Resolution;
  outData->SetSpacing(pix, pix, 1.0);
  outData->SetOrigin(0, 0, 0);
}

//----------------------------------------------------------------------------
void vtkImageReformat::ComputeInputUpdateExtent(int inExt[6], int outExt[6])
{
    // Use full input extent
    this->GetInput()->GetWholeExtent(inExt);
}

//----------------------------------------------------------------------------
void vtkImageReformat::SetPoint(int x, int y)
{
	float ras[4], ijk[4];
	int i;

	for (i=0; i<3; i++) 
  {
		this->WldPoint[i] = this->Origin[i] + this->XStep[i]*(float)x + 
		  this->YStep[i]*(float)y;
  }

	for (i=0; i<3; i++) 
  {
		ras[i] = this->WldPoint[i];
  }
	ras[3] = 1.0;

	this->WldToIjkMatrix->MultiplyPoint(ras, ijk);

  for (i=0; i<3; i++) 
  {
		this->IjkPoint[i] = ijk[i];
  }
}


// FAST1 (for indices) uses more bits of precision to the right
// of the decimal point than FAST2 (for data) because indices
// can only be as large as 512, but data can be 32767.
// We want more bits to reduce quantization errors.
//
#define NBITS1            16
#define MULTIPLIER1       65536.0f
#define FLOAT_TO_FAST1(x) (int)((x) * MULTIPLIER1)
#define FAST1_TO_FLOAT(x) ((x) / MULTIPLIER1)
#define FAST1_TO_INT(x)   ((x) >> NBITS1)
#define INT_TO_FAST1(x)   ((x) << NBITS1)
#define FAST1_MULT(x, y)  (((x) * (y)) >> NBITS1)

#define NBITS2            8
#define MULTIPLIER2       256.0f
#define FLOAT_TO_FAST2(x) (int)((x) * MULTIPLIER2)
#define FAST2_TO_FLOAT(x) ((x) / MULTIPLIER2)
#define FAST2_TO_INT(x)   ((x) >> NBITS2)
#define INT_TO_FAST2(x)   ((x) << NBITS2)
#define FAST2_MULT(x, y)  (((x) * (y)) >> NBITS2)

#define FAST1_TO_FAST2(x) ((x) >> (NBITS1-NBITS2))

//----------------------------------------------------------------------------
// Description:
// This templated function executes the filter for any type of data.
template <class T>
static void vtkImageReformatExecuteInt(vtkImageReformat *self,
				     vtkImageData *inData, int *inExt, T *inPtr,
				     vtkImageData *outData, 
				     int outExt[6], int wExt[6], int id)
{
	int res, i, idx, nxy, idxX, idxY, maxY, maxX;
	int inIncX, inIncY, inIncZ, outIncX, outIncY, outIncZ;
	float begin[4], origin[4], mx[4], my[4], mc[4], zero[4]={0.0,0.0,0.0,1.0};
	float originIJK[4], mxIJK[4], myIJK[4], zeroIJK[4];
	float xStep[3], yStep[3], xRewind[3];
	float x, y, z, scale;
	int nx, ny, nz, nx2, ny2, nz2, nz1, xi, yi, zi;
	T *ptr, *outPtr;
  vtkMatrix4x4 *mat = self->GetReformatMatrix();
  vtkMatrix4x4 *world = self->GetWldToIjkMatrix();
  //fast
  int fround, fx, fy, fz, fxStep[3], fyStep[3], fxRewind[3];
	int fone, fx0, fy0, fz0, fx1, fy1, fz1, fdx0, fdx1, fdxy0, fdxy1;
  // time
  clock_t tStart;
  if (id == 0) {tStart = clock();}

	// Find input dimensions
	nz = inExt[5] - inExt[4] + 1;
	ny = inExt[3] - inExt[2] + 1;
	nx = inExt[1] - inExt[0] + 1;
	nxy = nx * ny;
	nx2 = nx-2;
	ny2 = ny-2;
	nz2 = nz-2;
  nz1 = nz-1;

  // When the input extent is 0-based, then an index into it can be calculated as:
  // idx = zi*nxy + ni*nx + x
  // instead of the slower:
  // idx = (zi-inExt[4])*nxy + (yi-inExt[2])*nx + (xi-inExt[0])
  // so we'll only handle 0-based for now.
  //
  if (inExt[0] != 0 || inExt[2] != 0 || inExt[4] != 0)
  {
    fprintf(stderr, "Change vtkImageReformat to handle non-0-based extents.\n");
    return;
  }
	
  // find the region to loop over: this is the max pixel coordinate
	maxX = outExt[1]; 
	maxY = outExt[3];
  
	// Get pointer to output for this extent
	outPtr = (T*)outData->GetScalarPointerForExtent(outExt);

	// Get increments to march through data 
	outData->GetContinuousIncrements(outExt, outIncX, outIncY, outIncZ);
	inData->GetContinuousIncrements(inExt, inIncX, inIncY, inIncZ);

	// RAS-to-IJK Matrix looks like:
	//
	// mx0 my0 mz0 mc0
	// mx1 my1 mz1 mc1
	// mx2 my2 mz2 mc3
	//   0   0   0   1
	//
	// Where:
	// mx = normal vector along the x-direction of the output image
	// my = normal vector along the y-direction of the output image
	// mc = center of image
	//
	// Note:
	// The bottom row of the matrix needs to be "1"s
	// to treat each column as a homogeneous point for
	// matrix multiplication.
	//

	// Scale mx, my by FOV/RESOLUTION
  res = self->GetResolution();
	scale = self->GetFieldOfView() / res;

	mx[0] = mat->Element[0][0] * scale;
	mx[1] = mat->Element[1][0] * scale;
	mx[2] = mat->Element[2][0] * scale;
	mx[3] = 1.0;
	my[0] = mat->Element[0][1] * scale;
	my[1] = mat->Element[1][1] * scale;
	my[2] = mat->Element[2][1] * scale;
	my[3] = 1.0;
	mc[0] = mat->Element[0][3];
	mc[1] = mat->Element[1][3];
	mc[2] = mat->Element[2][3];
	mc[3] = 1.0;

	// Find the RAS origin (lower-left corner of reformated image).
	// The direction from the center to the origin is backwards from
	// the sum of the x-dir, y-dir vectors.
	// The length is half the OUTPUT image size.

	origin[0] = mc[0] - (mx[0] + my[0]) * res / 2.0;
	origin[1] = mc[1] - (mx[1] + my[1]) * res / 2.0;
	origin[2] = mc[2] - (mx[2] + my[2]) * res / 2.0;
	origin[3] = 1.0;

	// Advance to the origin of this output extent (used for threading)
	// x
 	scale = (float)(outExt[0]-wExt[0]);
	begin[0] = origin[0] + scale*mx[0];
	begin[1] = origin[1] + scale*mx[1];
	begin[2] = origin[2] + scale*mx[2];
	begin[3] = 1.0;
	// y
 	scale = (float)(outExt[2]-wExt[2]);	
	begin[0] = begin[0] + scale*my[0];
	begin[1] = begin[1] + scale*my[1];
	begin[2] = begin[2] + scale*my[2];
	begin[3] = 1.0;

	// Convert origin from RAS IJK space
	world->MultiplyPoint(begin, originIJK);
  world->MultiplyPoint(zero,  zeroIJK);
	world->MultiplyPoint(mx,    mxIJK);
	world->MultiplyPoint(my,    myIJK);
	
	// step vector in x direction
	xStep[0] = mxIJK[0] - zeroIJK[0];
	xStep[1] = mxIJK[1] - zeroIJK[1];
	xStep[2] = mxIJK[2] - zeroIJK[2];

	// step vector in y direction
	yStep[0] = myIJK[0] - zeroIJK[0];
	yStep[1] = myIJK[1] - zeroIJK[1];
	yStep[2] = myIJK[2] - zeroIJK[2];
	
	// Initialize volume coords x, y, z to origin
	x = originIJK[0];
	y = originIJK[1];
	z = originIJK[2];

	// rewind steps in x direction
	xRewind[0] = xStep[0] * (maxX+1);
	xRewind[1] = xStep[1] * (maxX+1);
	xRewind[2] = xStep[2] * (maxX+1);

	// Prepare to convert and return points to the user
	for (i=0; i<3; i++) 
  {
		self->Origin[i] = origin[i];
		self->XStep[i] = mx[i] - zero[i];
		self->YStep[i] = my[i] - zero[i];
	}

  // Convert float to fast
  fx = FLOAT_TO_FAST1(x);
  fy = FLOAT_TO_FAST1(y);
  fz = FLOAT_TO_FAST1(z);
  for (i=0; i<3; i++) 
  {
		fxStep[i] = FLOAT_TO_FAST1(xStep[i]);
  	fyStep[i] = FLOAT_TO_FAST1(yStep[i]);
		fxRewind[i] = FLOAT_TO_FAST1(xRewind[i]);
	}
  fround = FLOAT_TO_FAST1(0.49);
  fone = FLOAT_TO_FAST2(1.0);

  //
	// Interpolation
	//
	if (self->GetInterpolate())
	{
		// Loop through output pixels
		for (idxY = outExt[2]; idxY <= maxY; idxY++)
		{
      fxRewind[0] = fx;
      fxRewind[1] = fy;
      fxRewind[2] = fz;
        
			for (idxX = outExt[0]; idxX <= maxX; idxX++)
			{
				// Compute integer parts of volume coordinates
        xi = FAST1_TO_INT(fx);
				yi = FAST1_TO_INT(fy);
				zi = FAST1_TO_INT(fz);

				// Test if coordinates are outside volume
				if ((xi < 0) || (yi < 0) || (zi < 0) ||
			  	(xi > nx2) || (yi > ny2) || (zi > nz1))
				{
					// Indicate out of bounds with a -1
					*outPtr = 0;
				}
        // Handle the case of being on the last slice
        else if (zi == nz1)
        {
					fx1 = FAST1_TO_FAST2(fx) - INT_TO_FAST2(xi);
					fy1 = FAST1_TO_FAST2(fy) - INT_TO_FAST2(yi);
 
					fx0 = fone - fx1;
					fy0 = fone - fy1;

					// Interpolate in X and Y at Z0
					//
          idx = zi*nxy + yi*nx + xi;
					ptr = &inPtr[idx];

					fdx0 = FAST2_MULT(fx0, INT_TO_FAST2((int)ptr[0])) + 
						FAST2_MULT(fx1, INT_TO_FAST2((int)ptr[1]));
					ptr += nx;
					fdx1 = FAST2_MULT(fx0, INT_TO_FAST2((int)ptr[0])) + 
						FAST2_MULT(fx1, INT_TO_FAST2((int)ptr[1]));
        
					fdxy0 = FAST2_MULT(fy0, fdx0) + FAST2_MULT(fy1, fdx1);
        
					*outPtr = (T)FAST2_TO_FLOAT(fdxy0);
				}
				else 
        {
					fx1 = FAST1_TO_FAST2(fx) - INT_TO_FAST2(xi);
					fy1 = FAST1_TO_FAST2(fy) - INT_TO_FAST2(yi);
					fz1 = FAST1_TO_FAST2(fz) - INT_TO_FAST2(zi);
 
					fx0 = fone - fx1;
					fy0 = fone - fy1;
					fz0 = fone - fz1;

					// Interpolate in X and Y at Z0
					//
          idx = zi*nxy + yi*nx + xi;
					ptr = &inPtr[idx];

					fdx0 = FAST2_MULT(fx0, INT_TO_FAST2((int)ptr[0])) + 
						FAST2_MULT(fx1, INT_TO_FAST2((int)ptr[1]));
					ptr += nx;
					fdx1 = FAST2_MULT(fx0, INT_TO_FAST2((int)ptr[0])) + 
						FAST2_MULT(fx1, INT_TO_FAST2((int)ptr[1]));
        
					fdxy0 = FAST2_MULT(fy0, fdx0) + FAST2_MULT(fy1, fdx1);
        
					// Interpolate in X and Y at Z1
					//
					ptr = &inPtr[idx+nxy];

          fdx0 = FAST2_MULT(fx0, INT_TO_FAST2((int)ptr[0])) + 
						FAST2_MULT(fx1, INT_TO_FAST2((int)ptr[1]));
					ptr += nx;
					fdx1 = FAST2_MULT(fx0, INT_TO_FAST2((int)ptr[0])) + 
						FAST2_MULT(fx1, INT_TO_FAST2((int)ptr[1]));

					fdxy1 = FAST2_MULT(fy0, fdx0) + FAST2_MULT(fy1, fdx1);

					// Interpolate in Z
					//
					*outPtr = (T)(FAST2_TO_FLOAT(
            FAST2_MULT(fz0, fdxy0) + FAST2_MULT(fz1, fdxy1)));
        }
			  outPtr++;

				// Step volume coordinates in xs direction
				fx += fxStep[0];
				fy += fxStep[1];
				fz += fxStep[2];
			}
			outPtr  += outIncY;

			// Rewind volume coordinates back to first column and step in y
			fx = fxRewind[0] + fyStep[0];
			fy = fxRewind[1] + fyStep[1];
			fz = fxRewind[2] + fyStep[2];
		}
	}//interp

	//
	// Without interpolation 
	//
	else 
	{
	  nx2 = nx-1;
	  ny2 = ny-1;
	  nz2 = nz-1;

    // Loop through output pixels
		for (idxY = outExt[2]; idxY <= maxY; idxY++)
		{
      fxRewind[0] = fx;
      fxRewind[1] = fy;
      fxRewind[2] = fz;
        
			for (idxX = outExt[0]; idxX <= maxX; idxX++)
			{
				// Compute integer parts of volume coordinates
        xi = FAST1_TO_INT(fx + fround);
				yi = FAST1_TO_INT(fy + fround);
				zi = FAST1_TO_INT(fz + fround);
				
				// Test if coordinates are outside volume
				if ((xi < 0) || (yi < 0) || (zi < 0) ||
			  	(xi > nx2) || (yi > ny2) || (zi > nz2))
			  {
					*outPtr = 0; 
				}
				else {
					// Compute 'idx', the index into the input volume
					// where the output pixel value comes from.
					idx = zi*nxy + yi*nx + xi;
					*outPtr = inPtr[idx];
				}
				outPtr++;

				// Step volume coordinates in xs direction
				fx += fxStep[0];
				fy += fxStep[1];
				fz += fxStep[2];
			}
			outPtr  += outIncY;

			// Rewind volume coordinates back to first column and step in y
			fx = fxRewind[0] + fyStep[0];
			fy = fxRewind[1] + fyStep[1];
			fz = fxRewind[2] + fyStep[2];
		}
	}//withoutInterpolation

  if (id == 0) 
  {
    self->SetRunTime(clock() - tStart);
  }
}

//----------------------------------------------------------------------------
// Description:
// This templated function executes the filter for any type of data.
template <class T>
static void vtkImageReformatExecute(vtkImageReformat *self,
				     vtkImageData *inData, int *inExt, T *inPtr,
				     vtkImageData *outData, 
				     int outExt[6], int wExt[6], int id)
{
	int res, i, idx, nxy, idxX, idxY, maxY, maxX;
	int inIncX, inIncY, inIncZ, outIncX, outIncY, outIncZ;
	float begin[4], origin[4], mx[4], my[4], mc[4], zero[4]={0.0,0.0,0.0,1.0};
	float originIJK[4], mxIJK[4], myIJK[4], zeroIJK[4];
	float xStep[3], yStep[3], xRewind[3];
	float x, y, z, scale;
	float x0, y0, z0, x1, y1, z1, dx0, dx1, dxy0, dxy1;
	int nx, ny, nz, nx2, ny2, nz2, nz1, xi, yi, zi;
	T *ptr, *outPtr;
  vtkMatrix4x4 *mat = self->GetReformatMatrix();
  vtkMatrix4x4 *world = self->GetWldToIjkMatrix();
	// multiple components
  int nxc, idxC, numComps, scalarSize, inRowLength;
  // time
  clock_t tStart;
  if (id == 0)
    tStart = clock();

	// Find input dimensions
	numComps = inData->GetNumberOfScalarComponents();
	scalarSize = sizeof(T);
	inRowLength = (inExt[1] - inExt[0]+1);
	nz = inExt[5] - inExt[4] + 1;
	ny = inExt[3] - inExt[2] + 1;
	nx = inExt[1] - inExt[0] + 1;
	nxc = nx * numComps;
	nxy = nx * ny;
	nx2 = nx-2;
	ny2 = ny-2;
	nz2 = nz-2;
  nz1 = nx-1;

  // When the input extent is 0-based, then an index into it can be calculated as:
  // idx = zi*nxy + ni*nx + x
  // instead of the slower:
  // idx = (zi-inExt[4])*nxy + (yi-inExt[2])*nx + (xi-inExt[0])
  // so we'll only handle 0-based for now.
  //
  if (inExt[0] != 0 || inExt[2] != 0 || inExt[4] != 0)
  {
    fprintf(stderr, "Change vtkImageReformat to handle non-0-based extents.\n");
    return;
  }
	
  // find the region to loop over: this is the max pixel coordinate
	maxX = outExt[1]; 
	maxY = outExt[3];
  
	// Get pointer to output for this extent
	outPtr = (T*)outData->GetScalarPointerForExtent(outExt);

	// Get increments to march through data 
	outData->GetContinuousIncrements(outExt, outIncX, outIncY, outIncZ);
	inData->GetContinuousIncrements(inExt, inIncX, inIncY, inIncZ);

	// RAS-to-IJK Matrix looks like:
	//
	// mx0 my0 mz0 mc0
	// mx1 my1 mz1 mc1
	// mx2 my2 mz2 mc3
	//   0   0   0   1
	//
	// Where:
	// mx = normal vector along the x-direction of the output image
	// my = normal vector along the y-direction of the output image
	// mc = center of image
	//
	// Note:
	// The bottom row of the matrix needs to be "1"s
	// to treat each column as a homogeneous point for
	// matrix multiplication.
	//

	// Scale mx, my by FOV/RESOLUTION
  res = self->GetResolution();
	scale = self->GetFieldOfView() / res;

	mx[0] = mat->Element[0][0] * scale;
	mx[1] = mat->Element[1][0] * scale;
	mx[2] = mat->Element[2][0] * scale;
	mx[3] = 1.0;
	my[0] = mat->Element[0][1] * scale;
	my[1] = mat->Element[1][1] * scale;
	my[2] = mat->Element[2][1] * scale;
	my[3] = 1.0;
	mc[0] = mat->Element[0][3];
	mc[1] = mat->Element[1][3];
	mc[2] = mat->Element[2][3];
	mc[3] = 1.0;

	// Find the RAS origin (upper-left corner of reformated image).
	// The direction from the center to the origin is backwards from
	// the sum of the x-dir, y-dir vectors.
	// The length is half the OUTPUT image size.

	origin[0] = mc[0] - (mx[0] + my[0]) * res / 2.0;
	origin[1] = mc[1] - (mx[1] + my[1]) * res / 2.0;
	origin[2] = mc[2] - (mx[2] + my[2]) * res / 2.0;
	origin[3] = 1.0;

	// Advance to the origin of this output extent (used for threading)
	// x
 	scale = (float)(outExt[0]-wExt[0]);
	begin[0] = origin[0] + scale*mx[0];
	begin[1] = origin[1] + scale*mx[1];
	begin[2] = origin[2] + scale*mx[2];
	begin[3] = 1.0;
	// y
 	scale = (float)(outExt[2]-wExt[2]);	
	begin[0] = begin[0] + scale*my[0];
	begin[1] = begin[1] + scale*my[1];
	begin[2] = begin[2] + scale*my[2];
	begin[3] = 1.0;

	// Convert origin from RAS IJK space
	world->MultiplyPoint(begin, originIJK);
  world->MultiplyPoint(zero,  zeroIJK);
	world->MultiplyPoint(mx,    mxIJK);
	world->MultiplyPoint(my,    myIJK);
	
	// step vector in x direction
	xStep[0] = mxIJK[0] - zeroIJK[0];
	xStep[1] = mxIJK[1] - zeroIJK[1];
	xStep[2] = mxIJK[2] - zeroIJK[2];

	// step vector in y direction
	yStep[0] = myIJK[0] - zeroIJK[0];
	yStep[1] = myIJK[1] - zeroIJK[1];
	yStep[2] = myIJK[2] - zeroIJK[2];
	
	// Initialize volume coords x, y, z to origin
	x = originIJK[0];
	y = originIJK[1];
	z = originIJK[2];

	// rewind steps in x direction
	xRewind[0] = xStep[0] * (maxX+1);
	xRewind[1] = xStep[1] * (maxX+1);
	xRewind[2] = xStep[2] * (maxX+1);

	// Prepare to convert and return points to the user
	for (i=0; i<3; i++) 
  {
		self->Origin[i] = origin[i];
		self->XStep[i] = mx[i] - zero[i];
		self->YStep[i] = my[i] - zero[i];
	}

	//
	// Interpolation
	//
	if (self->GetInterpolate())
	{
		// Loop through output pixels
		for (idxY = outExt[2]; idxY <= maxY; idxY++)
		{
			for (idxX = outExt[0]; idxX <= maxX; idxX++)
			{
				// Compute integer parts of volume coordinates
				xi = (int)x;
				yi = (int)y;
				zi = (int)z;

				// Test if coordinates are outside volume
				if ((xi < 0) || (yi < 0) || (zi < 0) ||
			  	(xi > nx2) || (yi > ny2) || (zi > nz1))
				{
					// Indicate out of bounds with a -1
					memset(outPtr, 0, scalarSize);
	  			outPtr += numComps;
				}
        // Handle the case of being on the last slice
        else if (zi == nz1)
        {
					x1 = x - xi;
					y1 = y - yi;
 
					x0 = 1.0 - x1;
					y0 = 1.0 - y1;

					idx = zi*nxy + yi*nx + xi;
          idx *= numComps;

					for (idxC = 0; idxC < numComps; idxC++)
					{
				  	// Interpolate in X and Y at Z0
					  //
						ptr = &inPtr[idx+idxC];
						dx0 = x0*ptr[0] + x1*ptr[numComps]; ptr += nxc;
						dx1 = x0*ptr[0] + x1*ptr[numComps];
        
						dxy0 = y0*dx0 + y1*dx1;
        
						*outPtr = (T)dxy0;
            outPtr++;
					}//for c
        }
				else 
        {
					x1 = x - xi;
					y1 = y - yi;
					z1 = z - zi;
 
					x0 = 1.0 - x1;
					y0 = 1.0 - y1;
					z0 = 1.0 - z1;

					idx = zi*nxy + yi*nx + xi;
          idx *= numComps;

					for (idxC = 0; idxC < numComps; idxC++)
					{
				  	// Interpolate in X and Y at Z0
					  //
						ptr = &inPtr[idx+idxC];
						dx0 = x0*ptr[0] + x1*ptr[numComps]; ptr += nxc;
						dx1 = x0*ptr[0] + x1*ptr[numComps];
        
						dxy0 = y0*dx0 + y1*dx1;
        
						// Interpolate in X and Y at Z1
						//
						ptr = &inPtr[idx+idxC+nxy*numComps];
						dx0 = x0*ptr[0] + x1*ptr[numComps]; ptr += nxc;
						dx1 = x0*ptr[0] + x1*ptr[numComps];

						dxy1 = y0*dx0 + y1*dx1;

						// Interpolate in Z
						//
						*outPtr = (T)(z0*dxy0 + z1*dxy1);
            outPtr++;
					}//for c
				}// else

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

			// Step volume coordinates in y direction
			x += yStep[0];
			y += yStep[1];
			z += yStep[2];
		}
	}//interp

	//
	// Without interpolation 
	//
	else 
	{
	  nx2 = nx-1;
	  ny2 = ny-1;
	  nz2 = nz-1;

		// Loop through output pixels
		for (idxY = outExt[2]; idxY <= maxY; idxY++)
		{
			for (idxX = outExt[0]; idxX <= maxX; idxX++)
			{
				// Compute integer parts of volume coordinates
				xi = (int)(x + 0.5);
				yi = (int)(y + 0.5);
				zi = (int)(z + 0.5);
				
				// Test if coordinates are outside volume
				if ((xi < 0) || (yi < 0) || (zi < 0) ||
			  	(xi > nx2) || (yi > ny2) || (zi > nz2))
			  {
					memset(outPtr, 0, scalarSize);
				}
				else {
					// Compute 'idx', the index into the input volume
					// where the output pixel value comes from.
					idx = zi*nxy + yi*nx + xi;
					idx *= numComps;
					ptr = &inPtr[idx];
					memcpy(outPtr, ptr, scalarSize);
				}
				outPtr += numComps;

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
	}//withoutInterpolation

  if (id == 0) 
  {
    self->SetRunTime(clock() - tStart);
  }
}

//----------------------------------------------------------------------------
// Description:
// This method is passed a input and output data, and executes the filter
// algorithm to fill the output from the input.
// It just executes a switch statement to call the correct function for
// the datas data types.
void vtkImageReformat::ThreadedExecute(vtkImageData *inData, 
					vtkImageData *outData,
					int outExt[6], int id)
{
	int *inExt = inData->GetExtent();
	void *inPtr = inData->GetScalarPointerForExtent(inExt);
	int wExt[6], ext[6];
	this->GetOutput()->GetWholeExtent(wExt);
	int i, numComps = inData->GetNumberOfScalarComponents();

	// Example values for the extents (for a 4-processor machine on a 124 slice volume) are:
	// id: 0 outExt: 0 255 0 63 0 0,    inExt: 0 255 0 255 0 123  wExt: 0 255 0 255 0 0
	// id: 1 outExt: 0 255 64 127 0 0,  inExt: 0 255 0 255 0 123  wExt: 0 255 0 255 0 0
	// id: 2 outExt: 0 255 128 191 0 0, inExt: 0 255 0 255 0 123  wExt: 0 255 0 255 0 0
	// id: 3 outExt: 0 255 192 255 0 0, inExt: 0 255 0 255 0 123  wExt: 0 255 0 255 0 0

	// If no matrices provided, then create defaults
	if (!this->ReformatMatrix) 
  {
    // If the user has not set the ReformatMatrix, then create it.
    // The key is to perform: New(), Register(), Delete().
    // Then we can call UnRegister() in the destructor, and it will delete
    // the object if no one else is using it.  We don't have to distinguish
    // between whether we created the object, or someone else did!
		this->ReformatMatrix = vtkMatrix4x4::New();
    this->ReformatMatrix->Register(this);
    this->ReformatMatrix->Delete();
	}
	if (!this->WldToIjkMatrix) 
  {
		this->WldToIjkMatrix = vtkMatrix4x4::New();
    this->WldToIjkMatrix->Register(this);
    this->WldToIjkMatrix->Delete();

    this->GetInput()->GetWholeExtent(ext);
		for (i=0; i<3; i++)
    {
			this->WldToIjkMatrix->SetElement(i, 3, 
				(ext[i*2+1] - ext[i*2] + 1) / 2.0);
    }
	}

  // Use integer math for short and unsigned char data.
	
	switch (inData->GetScalarType())
	{
    case VTK_DOUBLE:
			vtkImageReformatExecute(this, inData, inExt, (double *)(inPtr), 
				outData, outExt, wExt, id);
			break;
		case VTK_FLOAT:
			vtkImageReformatExecute(this, inData, inExt, (float *)(inPtr), 
				outData, outExt, wExt, id);
			break;
    case VTK_LONG:
			vtkImageReformatExecute(this, inData, inExt, (long *)(inPtr), 
				outData, outExt, wExt, id);
			break;
    case VTK_UNSIGNED_LONG:
			vtkImageReformatExecute(this, inData, inExt, (unsigned long *)(inPtr), 
				outData, outExt, wExt, id);
			break;
		case VTK_INT:
			vtkImageReformatExecute(this, inData, inExt, (int *)(inPtr), 
				outData, outExt, wExt, id);
			break;
    case VTK_UNSIGNED_INT:
			vtkImageReformatExecute(this, inData, inExt, (unsigned int *)(inPtr), 
				outData, outExt, wExt, id);
			break;
    case VTK_SHORT:
      if (numComps == 1) {
			  vtkImageReformatExecuteInt(this, inData, inExt, (short *)(inPtr), outData, outExt, wExt, id);
			} else {
			  vtkImageReformatExecute(this, inData, inExt, (short *)(inPtr), outData, outExt, wExt, id);
			}
			break;
		case VTK_UNSIGNED_SHORT:
			vtkImageReformatExecute(this, inData, inExt, (unsigned short *)(inPtr), 
				outData, outExt, wExt, id);
			break;
		case VTK_CHAR:
      if (numComps == 1) {
			  vtkImageReformatExecuteInt(this, inData, inExt, (char *)(inPtr), outData, outExt, wExt, id);
			} else {
			  vtkImageReformatExecute(this, inData, inExt, (char *)(inPtr), outData, outExt, wExt, id);
			}
			break;
    case VTK_UNSIGNED_CHAR:
      if (numComps == 1) {
			 vtkImageReformatExecuteInt(this, inData, inExt, (unsigned char *)(inPtr), outData, outExt, wExt, id);
      } else {
			 vtkImageReformatExecute(this, inData, inExt, (unsigned char *)(inPtr), outData, outExt, wExt, id);
      }
			break;
		default:
			vtkErrorMacro(<< "Execute: Unknown input ScalarType");
			return;
	}
}



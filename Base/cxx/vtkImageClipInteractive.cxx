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
/*=============================================================================
Copyright (c) 1999 Surgical Planning Lab, Brigham and Women's Hospital
 
Direct all questions on this copyright to gering@ai.mit.edu.
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
=============================================================================*/
#include "vtkObjectFactory.h"
#include "vtkImageClipInteractive.h"
#include "vtkTransform.h"
#include <time.h>
#include <math.h>

//------------------------------------------------------------------------------
vtkImageClipInteractive* vtkImageClipInteractive::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkImageClipInteractive");
  if(ret)
    {
    return (vtkImageClipInteractive*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkImageClipInteractive;
}


//----------------------------------------------------------------------------
// Description:
// Constructor sets default values
vtkImageClipInteractive::vtkImageClipInteractive()
{
	this->ReformatMatrix = NULL;
	this->WldToIjkMatrix = NULL;
  this->FieldOfView = 240.0;
  memset(ClipExtent, 0, 6*sizeof(int));
}

vtkImageClipInteractive::~vtkImageClipInteractive()
{
  // Delete ReformatMatrix if self-created or if no one else is using it
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

void vtkImageClipInteractive::PrintSelf(ostream& os, vtkIndent indent)
{
	vtkImageToImageFilter::PrintSelf(os,indent);

  // vtkSetObjectMacro
  os << indent << "ReformatMatrix: " << this->ReformatMatrix << "\n";
  if (this->ReformatMatrix)
  {
    this->ReformatMatrix->PrintSelf(os,indent.GetNextIndent());
  }

  // vtkSetObjectMacro
  os << indent << "WldToIjkMatrix: " << this->WldToIjkMatrix << "\n";
  if (this->WldToIjkMatrix)
  {
    this->WldToIjkMatrix->PrintSelf(os,indent.GetNextIndent());
  }
        
  os << indent << "ClipExtent:\n";
  for(int i=0; i<6; i++)
  {
    os << indent << "ClipExtent[i] ";
  }
  os << indent << "\n";
}

//----------------------------------------------------------------------------
void vtkImageClipInteractive::ExecuteInformation(vtkImageData *inData, vtkImageData *outData)
{
	// Must have matrices provided
	if (!(this->ReformatMatrix || this->WldToIjkMatrix)) {
		vtkErrorMacro(<<"Must have matrices set.");
	}
}

void vtkImageClipInteractive::ComputeRequiredInputUpdateExtent(int inExt[6], int outExt[6])
{
  // Use full input extent
  this->GetInput()->GetWholeExtent(inExt);
}

//----------------------------------------------------------------------------
// Description:
// This templated function executes the filter for any type of data.
template <class T>
static void vtkImageClipInteractiveExecute(vtkImageClipInteractive *self,
				     vtkImageData *inData, T *inPtr,
				     vtkImageData *outData, 
				     int outExt[6], int wExt[6], int id)
{
	int i, iz, idx, nxy, idxX, idxY, maxY, maxX;
	int inIncX, inIncY, inIncZ, outIncX, outIncY, outIncZ;
	float begin[4], origin[4], mx[4], my[4], mc[4], zero[4]={0.0,0.0,0.0,1.0};
	float origin_ijk[4], mx_ijk[4], my_ijk[4], zero_ijk[4];
	float xStep[3], yStep[3], xRewind[3];
	float x, y, z, scale;
	float x0, y0, z0, x1, y1, z1, dx0, dx1, dxy0, dxy1;
	int nx, ny, nz, nx2, ny2, nz2, xi, yi, zi;
	T *ptr, *outPtr;
	int inExt[6], Resolution;
  int clipExt[6];
  vtkMatrix4x4 *WldToIjkMatrix = self->GetWldToIjkMatrix();
  vtkMatrix4x4 *ReformatMatrix = self->GetReformatMatrix();

  self->GetClipExtent(clipExt);

	// find the region to loop over
	maxX = outExt[1]; 
	maxY = outExt[3];
 
	// Find input dimensions
	inData->GetExtent(inExt);
	nz = inExt[5] - inExt[4] + 1;
	ny = inExt[3] - inExt[2] + 1;
	nx = inExt[1] - inExt[0] + 1;
  Resolution = nx;
	nxy = nx * ny;
	nx2 = nx-2;
	ny2 = ny-2;
	nz2 = nz-2;

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
	scale = self->FieldOfView / Resolution;

	mx[0] = ReformatMatrix->Element[0][0] * scale;
	mx[1] = ReformatMatrix->Element[1][0] * scale;
	mx[2] = ReformatMatrix->Element[2][0] * scale;
	mx[3] = 1.0;
	my[0] = ReformatMatrix->Element[0][1] * scale;
	my[1] = ReformatMatrix->Element[1][1] * scale;
	my[2] = ReformatMatrix->Element[2][1] * scale;
	my[3] = 1.0;
	mc[0] = ReformatMatrix->Element[0][3];
	mc[1] = ReformatMatrix->Element[1][3];
	mc[2] = ReformatMatrix->Element[2][3];
	mc[3] = 1.0;

	// Find the RAS origin (upper-left corner of reformated image).
	// The direction from the center to the origin is backwards from
	// the sum of the x-dir, y-dir vectors.
	// The length is half the OUTPUT image size.

	origin[0] = mc[0] - (mx[0] + my[0]) * Resolution / 2.0;
	origin[1] = mc[1] - (mx[1] + my[1]) * Resolution / 2.0;
	origin[2] = mc[2] - (mx[2] + my[2]) * Resolution / 2.0;
	origin[3] = 1.0;

	// Advance to the origin of this output extent (used for threading)
	// x
 	scale = (float)(outExt[0]-wExt[0])/(float)(Resolution);
	begin[0] = origin[0] + scale*mx[0] * Resolution;
	begin[1] = origin[1] + scale*mx[1] * Resolution;
	begin[2] = origin[2] + scale*mx[2] * Resolution;
	begin[3] = 1.0;
	// y
 	scale = (float)(outExt[2]-wExt[2])/(float)(Resolution);	
	begin[0] = begin[0] + scale*my[0] * Resolution;
	begin[1] = begin[1] + scale*my[1] * Resolution;
	begin[2] = begin[2] + scale*my[2] * Resolution;
	begin[3] = 1.0;

	// Convert origin from RAS IJK space
	WldToIjkMatrix->MultiplyPoint(begin, origin_ijk);
	WldToIjkMatrix->MultiplyPoint(zero,  zero_ijk);
	WldToIjkMatrix->MultiplyPoint(mx,    mx_ijk);
	WldToIjkMatrix->MultiplyPoint(my,    my_ijk);
	
	// step vector in x direction
	xStep[0] = mx_ijk[0] - zero_ijk[0];
	xStep[1] = mx_ijk[1] - zero_ijk[1];
	xStep[2] = mx_ijk[2] - zero_ijk[2];

	// step vector in y direction
	yStep[0] = my_ijk[0] - zero_ijk[0];
	yStep[1] = my_ijk[1] - zero_ijk[1];
	yStep[2] = my_ijk[2] - zero_ijk[2];
	
	// Initialize volume coords x, y, z to origin
	x = origin_ijk[0];
	y = origin_ijk[1];
	z = origin_ijk[2];

	// rewind steps in x direction
	xRewind[0] = xStep[0] * Resolution;
	xRewind[1] = xStep[1] * Resolution;
	xRewind[2] = xStep[2] * Resolution;

		// Loop through output pixels
		for (idxY = outExt[2]; idxY <= maxY; idxY++)
		{
			for (idxX = outExt[0]; idxX <= maxX; idxX++)
			{
				// Compute integer parts of volume coordinates
				xi = (int)(x + 0.5);
				yi = (int)(y + 0.5);
				zi = (int)(z + 0.5);
				
				// Test if coordinates are outside extent
				if ((xi < clipExt[0]) || (yi < clipExt[2]) || (zi < clipExt[4]) ||
					(xi > clipExt[1]) || (yi > clipExt[3]) || (zi > clipExt[5]))
				{
					*outPtr = 0; 
				}
				else {
					*outPtr = 1;
				}
				outPtr++;

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
void vtkImageClipInteractive::ThreadedExecute(vtkImageData *inData, 
					vtkImageData *outData,
					int outExt[6], int id)
//void vtkImageClipInteractive::Execute(vtkImageData *inData, vtkImageData *outData)
{
//	int outExt[6], id=0;
//	this->Output->GetWholeExtent(outExt);
	int *inExt = inData->GetExtent();
	void *inPtr = inData->GetScalarPointerForExtent(inExt);
	int wExt[6];
	outData->GetWholeExtent(wExt);

		switch (inData->GetScalarType())
		{
                case VTK_DOUBLE:
			vtkImageClipInteractiveExecute(this, inData, (double *)(inPtr), 
				outData, outExt, wExt, id);
			break;
		case VTK_FLOAT:
			vtkImageClipInteractiveExecute(this, inData, (float *)(inPtr), 
				outData, outExt, wExt, id);
			break;
	        case VTK_LONG:
			vtkImageClipInteractiveExecute(this, inData, (long *)(inPtr), 
				outData, outExt, wExt, id);
			break;
	        case VTK_UNSIGNED_LONG:
			vtkImageClipInteractiveExecute(this, inData, (unsigned long *)(inPtr), 
				outData, outExt, wExt, id);
			break;
		case VTK_INT:
			vtkImageClipInteractiveExecute(this, inData, (int *)(inPtr), 
				outData, outExt, wExt, id);
			break;
        	case VTK_UNSIGNED_INT:
			vtkImageClipInteractiveExecute(this, inData, (unsigned int *)(inPtr), 
				outData, outExt, wExt, id);
			break;
		case VTK_SHORT:
			vtkImageClipInteractiveExecute(this, inData, (short *)(inPtr), 
				outData, outExt, wExt, id);
			break;
		case VTK_UNSIGNED_SHORT:
			vtkImageClipInteractiveExecute(this, inData, (unsigned short *)(inPtr), 
				outData, outExt, wExt, id);
			break;
	        case VTK_CHAR:
			vtkImageClipInteractiveExecute(this, inData, (char *)(inPtr), 
				outData, outExt, wExt, id);
			break;
		case VTK_UNSIGNED_CHAR:
			vtkImageClipInteractiveExecute(this, inData, (unsigned char *)(inPtr), 
				outData, outExt, wExt, id);
			break;
		default:
			vtkErrorMacro(<< "Execute: Unknown input ScalarType");
			return;
		}
}



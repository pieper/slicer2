/*=auto=========================================================================

(c) Copyright 2003 Massachusetts Institute of Technology (MIT) All Rights Reserved.

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
#include "vtkImageResize.h"
#include "vtkObjectFactory.h"

//------------------------------------------------------------------------------
vtkImageResize* vtkImageResize::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkImageResize");
  if(ret)
  {
    return (vtkImageResize*)ret;
  }
  // If the factory was unable to create the object, then create it here.
  return new vtkImageResize;
}

//----------------------------------------------------------------------------
// Constructor: Sets default filter to be identity.
vtkImageResize::vtkImageResize()
{
  int idx;
  for (idx = 0; idx < 3; ++idx)
  {
    this->InputClipExtent[idx*2]  = -VTK_LARGE_INTEGER;
    this->InputClipExtent[idx*2+1] = VTK_LARGE_INTEGER;
    this->OutputWholeExtent[idx*2]  = -VTK_LARGE_INTEGER;
    this->OutputWholeExtent[idx*2+1] = VTK_LARGE_INTEGER;
  }
  this->Initialized = 0;
}

//----------------------------------------------------------------------------
void vtkImageResize::SetOutputWholeExtent(int extent[6])
{
  int idx;
  for (idx = 0; idx < 6; ++idx)
  {
    if (this->OutputWholeExtent[idx] != extent[idx])
    {
      this->OutputWholeExtent[idx] = extent[idx];
      this->Modified();
    }
  }
  this->Initialized = 1;
}

//----------------------------------------------------------------------------
void vtkImageResize::SetOutputWholeExtent(int minX, int maxX, 
                         int minY, int maxY,
                         int minZ, int maxZ)
{
  int extent[6];
  extent[0] = minX;  extent[1] = maxX;
  extent[2] = minY;  extent[3] = maxY;
  extent[4] = minZ;  extent[5] = maxZ;
  this->SetOutputWholeExtent(extent);
}

//----------------------------------------------------------------------------
void vtkImageResize::GetOutputWholeExtent(int extent[6])
{
  int idx;
  for (idx = 0; idx < 6; ++idx)
  {
    extent[idx] = this->OutputWholeExtent[idx];
  }
}

//----------------------------------------------------------------------------
void vtkImageResize::SetInputClipExtent(int extent[6])
{
  int idx;
  for (idx = 0; idx < 6; ++idx)
  {
    if (this->InputClipExtent[idx] != extent[idx])
    {
      this->InputClipExtent[idx] = extent[idx];
      this->Modified();
    }
  }
}

//----------------------------------------------------------------------------
void vtkImageResize::SetInputClipExtent(int minX, int maxX, 
                         int minY, int maxY,
                         int minZ, int maxZ)
{
  int extent[6];
  extent[0] = minX;  extent[1] = maxX;
  extent[2] = minY;  extent[3] = maxY;
  extent[4] = minZ;  extent[5] = maxZ;
  this->SetInputClipExtent(extent);
}

//----------------------------------------------------------------------------
void vtkImageResize::GetInputClipExtent(int extent[6])
{
  int idx;
  for (idx = 0; idx < 6; ++idx)
  {
    extent[idx] = this->InputClipExtent[idx];
  }
}


// Change the WholeExtent
//----------------------------------------------------------------------------
void vtkImageResize::ExecuteInformation(vtkImageData *inData, 
                      vtkImageData *outData)
{
  int idx, extent[6];
  float spacing[3], origin[3];
  
  inData->GetWholeExtent(extent);
  inData->GetSpacing(spacing);
  inData->GetOrigin(origin);

  if ( ! this->Initialized)
  {
    this->SetOutputWholeExtent(extent);
  }

  // Clip the OutputWholeExtent with the input WholeExtent
    for (idx = 0; idx < 3; ++idx)
    {
      if (this->OutputWholeExtent[idx*2] >= extent[idx*2] && 
        this->OutputWholeExtent[idx*2] <= extent[idx*2+1])
      {
        extent[idx*2] = this->OutputWholeExtent[idx*2];
      }
      if (this->OutputWholeExtent[idx*2+1] >= extent[idx*2] && 
          this->OutputWholeExtent[idx*2+1] <= extent[idx*2+1])
        {
          extent[idx*2+1] = this->OutputWholeExtent[idx*2+1];
        }
      // make sure the order is correct
      if (extent[idx*2] > extent[idx*2+1])
        {
          extent[idx*2] = extent[idx*2+1];
        }
    }

    // Scale spacing according to the input clip extent
    for (idx = 0; idx < 3; ++idx)
    {
      spacing[idx] *= (this->OutputWholeExtent[idx*2+1] -
        this->OutputWholeExtent[idx*2] + 1) /
        (this->InputClipExtent[idx*2+1] -
        this->InputClipExtent[idx*2] + 1);

      origin[idx] += this->InputClipExtent[idx*2];
    }
  outData->SetWholeExtent(extent);
  outData->SetSpacing(spacing);
  outData->SetOrigin(origin);
}

//----------------------------------------------------------------------------
// Get only the clipped input.
void vtkImageResize::ComputeInputUpdateExtent(int inExt[6], 
                                                        int outExt[6])
{
  memcpy(inExt, this->InputClipExtent, 6*sizeof(int));
}

//----------------------------------------------------------------------------
// The InputClipExtent is relative to the input's WholeExtent,
// so I have to have a ptr to the WholeExtent, or the ptr will be bad.
template <class T>
static void vtkImageResizeExecute1D(vtkImageResize *self,
  vtkImageData *inData, T *inPtr, int inExt[6],
  vtkImageData *outData, T *outPtr, int outExt[6], 
  int id)
{
  int idxX, maxX, inMaxX;
  int outIncX, outIncY, outIncZ;
  float scale, step[1], origin[1];
  float x;
  long idx, nx, nx2, xi;
  float magX;
  // interpolation
    float x0, x1;
    // multiple components
  int idxC, numComps, scalarSize, inRowLength;
  // whole input extent
  int wExt[6];
  inData->GetWholeExtent(wExt);
  T *ptr;

  // find the region to loop over
  numComps = inData->GetNumberOfScalarComponents();
  maxX = outExt[1] - outExt[0];
  inMaxX = inExt[1] - inExt[0]; 
  scalarSize = numComps*sizeof(T);

  // Integer Scale into tmp
  int iX, iMagX = (inMaxX+1) / (maxX+1);
  iMagX = (iMagX < 1) ? 1 : iMagX;
  int nTmp = (inMaxX+1) / iMagX;
  T *tmp = new T[nTmp];
  ptr = tmp;
  // Kilian : 
  // 1.) Interpolation: Here we interpolate by just adding up neighbors 
  // Example : inMaxX = 99 (Input Size) , maxX = 29 (Output size)
  //           => iMagX = 3 => nTemp = 33 
  //           We wantoutput size 29+1 and not 33
  //           => 2. Interpolation is following 
  for (idxX = 0; idxX < nTmp; idxX++)
  {
    *ptr = 0;
    for (iX = 0; iX < iMagX; iX++)
    {
      *ptr += *inPtr;
      inPtr++;
    }
    *ptr /= iMagX;
    ptr++;
  }

  // Reset the input to be tmp instead of inPtr
  wExt[0] = inExt[0] = 0;
  wExt[1] = inExt[1] = nTmp-1;
  inMaxX = inExt[1] - inExt[0]; 
  inRowLength = (inMaxX+1)*numComps;

  // find whole size of input for boundary checking
  nx = wExt[1] - wExt[0] + 1; 
  nx2 = nx - 2;

  // We will walk output (resized) space and calculate input space
  // coordinates to obtain source pixels.

  // mag = output/input
  magX = (float)(maxX+1) / (float)(inMaxX+1);

  // step vector is the inverse magnification
  magX = (magX == 0.0) ? 1.0 : magX;
  step[0] = 1.0 / magX;
    
  // Find origin (upper left) of output space in terms of input space coordinates
  // (NOTE: relative to the ScalarPointerForExtent(inExt))
  // Therefore: set origin[0] to inExt[0] if inExt=wExt, else 0
  origin[0] = 0;
    
  // Advance to the origin of this output extent (used for threading)
  // x
  // DAVE THREAD 
  // scale = (float)(outExt[0]-inExt[0])/(float)(inExt[1]-inExt[0]+1);
  scale = 0.0;
  origin[0] = origin[0] + scale*nx*step[0];

  // Initialize output coords x, y to origin
  x = origin[0];

  // Get increments to march through data 
  outData->GetContinuousIncrements(outExt, outIncX, outIncY, outIncZ);
  // Kilian
  // 2.) Interpolation: Here we interpolate between two neighbors x[i] and x[i+1]
  //     to shink it to final size  
  // Example : ptr is of size 33 and outPtr is of size 30 

      for (idxX = outExt[0]; idxX <= maxX; idxX++)
      {
        // Compute integer parts of volume coordinates
        xi = (int)x;

        // Test if coordinates are outside volume
        if ((xi < 0) || (xi > nx2))
        {
          memset(outPtr, 0, scalarSize);
          outPtr += numComps;
        }
        else 
        {
                    x1 = x - xi;
 
                    x0 = 1.0 - x1;

                    // Interpolate in X 
                    //
                    idx = xi;
          idx *= numComps;

                    for (idxC = 0; idxC < numComps; idxC++)
                    {
                        // Interpolate in X and Y
                        ptr = &tmp[idx+idxC];
                        *outPtr = (T)(x0*ptr[0] + x1*ptr[numComps]);
        
            outPtr++;
                    }//for c
        }
        x += step[0];
      }
  
      delete [] tmp;
}

//----------------------------------------------------------------------------
template <class T>
static void vtkImageResizeExecute2D(vtkImageResize *self,
  vtkImageData *inData, T *inPtr, int inExt[6],
  vtkImageData *outData, T *outPtr, int outExt[6], 
  int id)
{
  int idxX, idxY, maxX, maxY, inMaxX, inMaxY;
  int outIncX, outIncY, outIncZ;
  //float scale=0;
  float step[2], origin[2], xRewind;
  float x, y;
  long idx, nx, ny, nx2, ny2, xi, yi;
  float magX, magY;
  // interpolation
    float x0, y0, x1, y1, dx0, dx1;
    // multiple components
  int nxc, idxC, numComps, scalarSize, inRowLength;
  // whole input extent
  int wExt[6];
  inData->GetWholeExtent(wExt);
  T *ptr;

  // find whole size of input for boundary checking
  nx = wExt[1] - wExt[0] + 1; 
  ny = wExt[3] - wExt[2] + 1; 
  nx2 = nx - 2;
  ny2 = ny - 2;

  // find the region to loop over
  numComps = inData->GetNumberOfScalarComponents();
    nxc = nx * numComps;
  maxX = outExt[1];
  maxY = outExt[3];
  inMaxX = inExt[1] - inExt[0]; 
  inMaxY = inExt[3] - inExt[2]; 
  inRowLength = (inMaxX+1)*numComps;
  scalarSize = numComps*sizeof(T);

  // We will walk output (resized) space and calculate input space
  // coordinates to obtain source pixels.

  // mag = output/input
  magX = (float)(maxX+1) / (float)(inMaxX+1);
  magY = (float)(maxY+1) / (float)(inMaxY+1);

  // step vector is the inverse magnification
  magX = (magX == 0.0) ? 1.0 : magX;
  magY = (magY == 0.0) ? 1.0 : magY;
  step[0] = 1.0 / magX;
  step[1] = 1.0 / magY;
    
  // Find origin (upper left) of output space in terms of input space coordinates
  // (NOTE: relative to the ScalarPointerForExtent(inExt))
  // Therefore: set origin[0] to inExt[0] if inExt=wExt, else 0
  origin[0] = 0;
  origin[1] = 0;
    
  // Advance to the origin of this output extent (used for threading)
  // x
  //DAVE THREAD
//  scale = (float)(outExt[0]-wExt[0])/(float)(wExt[1]-wExt[0]+1);
//  origin[0] = origin[0] + scale*nx*step[0];
  // y
//  scale = (float)(outExt[2]-wExt[2])/(float)(wExt[3]-wExt[2]+1);    
//  origin[1] = origin[1] + scale*ny*step[1];

  // Initialize output coords x, y to origin
  x = origin[0];
  y = origin[1];

  // Get increments to march through data 
  outData->GetContinuousIncrements(outExt, outIncX, outIncY, outIncZ);

    // Loop through output pixels
    for (idxY = outExt[2]; idxY <= maxY; idxY++)
    {
      xRewind = x;
      for (idxX = outExt[0]; idxX <= maxX; idxX++)
      {
        // Compute integer parts of volume coordinates
        xi = (int)x;
        yi = (int)y;

        // Test if coordinates are outside volume
        if ((xi < 0) || (yi < 0) || (xi > nx2) || (yi > ny2))
        {
          memset(outPtr, 0, scalarSize);
          outPtr += numComps;
        }
        else 
        {
                    x1 = x - xi;
                    y1 = y - yi;
 
                    x0 = 1.0 - x1;
                    y0 = 1.0 - y1;

                    // Interpolate in X and Y at Z0
                    //
                    idx = yi*nx + xi;
          idx *= numComps;

                    for (idxC = 0; idxC < numComps; idxC++)
                    {
                        // Interpolate in X and Y
                        ptr = &inPtr[idx+idxC];
                        dx0 = x0*ptr[0] + x1*ptr[numComps]; ptr += nxc;
                        dx1 = x0*ptr[0] + x1*ptr[numComps];
        
                        *outPtr = (T)(y0*dx0 + y1*dx1);
            outPtr++;
                    }//for c
        }
        x += step[0];
      }
      outPtr += outIncY;
      x = xRewind;
      y += step[1];
    }
  
}

//----------------------------------------------------------------------------
template <class T>
static void vtkImageResizeExecute3D(vtkImageResize *self,
  vtkImageData *inData, T *inPtr, int inExt[6],
  vtkImageData *outData, T *outPtr, int outExt[6], 
  int id)
{
}

//----------------------------------------------------------------------------
template <class T>
static void vtkImageResizeExecute(vtkImageResize *self,
                  vtkImageData *inData, T *inPtr, int inExt[6],
                  vtkImageData *outData, T *outPtr,
                  int outExt[6], int id)
{
  if (outExt[5] == outExt[4])
  {
    if (outExt[3] == outExt[2])
    {
      vtkImageResizeExecute1D(self, inData, inPtr, inExt,
        outData, outPtr, outExt, id);
      return;
    }
    else
    {
      vtkImageResizeExecute2D(self, inData, inPtr, inExt,
        outData, outPtr, outExt, id);
      return;
    }
  }
  else
  {
    vtkImageResizeExecute3D(self, inData, inPtr, inExt,
      outData, outPtr, outExt, id);
  }
}

//----------------------------------------------------------------------------
void vtkImageResize::ThreadedExecute(vtkImageData *inData, 
                      vtkImageData *outData, int outExt[6], int id)
{
  int inExt[6];
  outData->GetExtent(outExt);
  this->ComputeInputUpdateExtent(inExt, outExt);
  void *inPtr = inData->GetScalarPointerForExtent(inExt);
  void *outPtr = outData->GetScalarPointerForExtent(outExt);

  // For now, need 1D
  if (outExt[5] != outExt[4] || outExt[3] != outExt[2])
  {
    vtkErrorMacro("This filter requires 1-D input, not: " <<
      outExt[0]<<"x"<<outExt[1]<<","<<outExt[2]<<"x"<<outExt[3]<<","<<
      outExt[4]<<"x"<<outExt[5]);
    return;
  }

  // this filter expects that input is the same type as output.
  if (inData->GetScalarType() != outData->GetScalarType())
  {
    vtkErrorMacro(<< "Execute: input ScalarType, " << inData->GetScalarType()
    << ", must match out ScalarType " << outData->GetScalarType());
    return;
  }
  
  switch (inData->GetScalarType())
    {
    case VTK_DOUBLE:
      vtkImageResizeExecute(this, 
                 inData, (double *)(inPtr), inExt,
                 outData, (double *)(outPtr), outExt, id);
      break;
    case VTK_FLOAT:
      vtkImageResizeExecute(this, 
                 inData, (float *)(inPtr), inExt,
                 outData, (float *)(outPtr), outExt, id);
      break;
    case VTK_LONG:
      vtkImageResizeExecute(this, 
                 inData, (long *)(inPtr), inExt,
                 outData, (long *)(outPtr), outExt, id);
      break;
    case VTK_UNSIGNED_LONG:
      vtkImageResizeExecute(this, 
                 inData, (unsigned long *)(inPtr), inExt,
                 outData, (unsigned long *)(outPtr), outExt, id);
      break;
    case VTK_INT:
      vtkImageResizeExecute(this, 
                 inData, (int *)(inPtr), inExt,
                 outData, (int *)(outPtr), outExt, id);
      break;
    case VTK_UNSIGNED_INT:
      vtkImageResizeExecute(this, 
                 inData, (unsigned int *)(inPtr), inExt,
                 outData, (unsigned int *)(outPtr), outExt, id);
      break;
    case VTK_SHORT:
      vtkImageResizeExecute(this, 
                 inData, (short *)(inPtr), inExt,
                 outData, (short *)(outPtr), outExt, id);
      break;
    case VTK_UNSIGNED_SHORT:
      vtkImageResizeExecute(this, 
                 inData, (unsigned short *)(inPtr), inExt,
                 outData, (unsigned short *)(outPtr), outExt, id);
      break;
    case VTK_CHAR:
      vtkImageResizeExecute(this, 
                 inData, (char *)(inPtr), inExt,
                 outData, (char *)(outPtr), outExt, id);
      break;
    case VTK_UNSIGNED_CHAR:
      vtkImageResizeExecute(this, 
                 inData, (unsigned char *)(inPtr), inExt,
                 outData, (unsigned char *)(outPtr), outExt, id);
      break;
    default:
      vtkErrorMacro(<< "Execute: Unknown ScalarType");
      return;
    }
}

//----------------------------------------------------------------------------
void vtkImageResize::PrintSelf(ostream& os, vtkIndent indent)
{
  int idx;
  vtkImageToImageFilter::PrintSelf(os,indent);

  os << "Output Whole Extent:\n";
  for (idx = 0; idx < 3; ++idx)
  {
    os << indent << ", " << this->OutputWholeExtent[idx*2]
       << "," << this->OutputWholeExtent[idx*2+1];
  }
  os << ")\n";


  os << "Input Clip Extent:\n";
  for (idx = 0; idx < 3; ++idx)
  {
    os << indent << ", " << this->InputClipExtent[idx*2]
       << "," << this->InputClipExtent[idx*2+1];
  }
  os << ")\n";

  os << indent << "Initialized: " << this->Initialized << "\n";
}



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
#include "vtkImageOverlay.h"
#include "stdlib.h"
#include "vtkObjectFactory.h"

//------------------------------------------------------------------------------
vtkImageOverlay* vtkImageOverlay::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkImageOverlay");
  if(ret)
  {
    return (vtkImageOverlay*)ret;
  }
  // If the factory was unable to create the object, then create it here.
  return new vtkImageOverlay;
}

// Description:
// Construct object to extract all of the input data.
//----------------------------------------------------------------------------
vtkImageOverlay::vtkImageOverlay()
{
  this->Opacity = NULL;
  this->nOpacity = 0;
}

//----------------------------------------------------------------------------
vtkImageOverlay::~vtkImageOverlay()
{
  if (this->Opacity != NULL) 
  {
    delete [] this->Opacity;
  }
}

//----------------------------------------------------------------------------
void vtkImageOverlay::UpdateForNumberOfInputs() 
{
  int i, nInputs = this->NumberOfInputs;

  if (this->nOpacity != this->NumberOfInputs) 
  {
    // Make new array to store the opacity of each input
    double *opacity = new double[nInputs];

    // Initialize 
    for (i=0; i < nInputs; i++)
    {
      opacity[i] = 1.0;
    }

    if (this->Opacity != NULL) 
    {
      // Copy old Opacities
      for (i=0; i < this->nOpacity && i < nInputs; i++)
      {
        opacity[i] = this->Opacity[i];
      }

      // Delete the previous array
      delete [] this->Opacity;
    }
    
    // Set the new array
    this->Opacity = opacity;
    this->nOpacity = nInputs;
  }
}

//----------------------------------------------------------------------------
double vtkImageOverlay::GetOpacity(int layer)
{
  if (layer >= this->nOpacity)
  {
    this->UpdateForNumberOfInputs();
  }
  if (layer < 0 || layer >= this->NumberOfInputs)
  {
    vtkErrorMacro(<< "Layer "<<layer<<" is not between 0 and "<<
      this->NumberOfInputs);
    return 0.0;
  }
  return this->Opacity[layer];
}

//----------------------------------------------------------------------------
void vtkImageOverlay::SetOpacity(int layer, double opacity) 
{
  if (layer >= this->nOpacity) 
  {
    this->UpdateForNumberOfInputs();
  }
  if (layer < 0 || layer >= this->NumberOfInputs)
  {
    vtkErrorMacro(<< "Layer "<<layer<<" is not between 0 and "<<
      this->NumberOfInputs);
    return;
  }
  this->Opacity[layer] = opacity;
  this->Modified();
}

//----------------------------------------------------------------------------
void vtkImageOverlay::SetInputOff(int layer) 
{
  if (layer < 0 || layer >= this->NumberOfInputs)
  {
    vtkErrorMacro(<< "Layer "<<layer<<" is not between 0 and "<<
      this->NumberOfInputs);
    return;
  }
  this->SetInput(layer, (vtkImageData *)NULL);
}

//----------------------------------------------------------------------------
// Description:
// This templated function executes the filter for any type of data
template <class T>
static void vtkImageOverlayExecute(vtkImageOverlay *self,
  vtkImageData *inData, T *inPtr, int inExt[6],
  vtkImageData *outData, T* outPtr,
  int outExt[6], int id, int layer, int firstLayer)
{
  int zero, c, idxX, idxY, idxZ, maxX, maxY, maxZ;
  int rowLength, size, rowSize, pixSize;
  int inIncX, inIncY, inIncZ, outIncX, outIncY, outIncZ, cpyIncY, cpyIncZ;
  double alpha, beta;
  int ncomp = inData->GetNumberOfScalarComponents();
  unsigned long count = 0;
  unsigned long target;

  // find the region to loop over
  maxX = outExt[1] - outExt[0];
  maxY = inExt[3] - inExt[2];
  maxZ = inExt[5] - inExt[4];
  rowLength = (maxX+1)*ncomp;
  size = inData->GetScalarSize();
  rowSize = rowLength * size;
  pixSize = ncomp * size;
  target = (unsigned long)((maxZ+1)*(maxY+1)/50.0);
  target++;

  // Get increments to march through data
  inData->GetContinuousIncrements(inExt, inIncX, inIncY, inIncZ);
  outData->GetContinuousIncrements(outExt, outIncX, outIncY, outIncZ);
  
  // adjust increments for this loop
  cpyIncY = outIncY + rowLength;
  cpyIncZ = outIncZ * size;

  inPtr  = (T*)inData->GetScalarPointerForExtent(outExt);
  outPtr = (T*)outData->GetScalarPointerForExtent(outExt);

  // The first layer just gets copied
  if (firstLayer)
  {
    for (idxZ = 0; idxZ <= maxZ; idxZ++) 
    {
      for (idxY = 0; idxY <= maxY; idxY++) 
      {
        memcpy(outPtr, inPtr, rowSize);
        inPtr += cpyIncY;
        outPtr += cpyIncY;
      }//for y
      outPtr += cpyIncZ;
      inPtr += cpyIncZ;
    }//for z
  }

  // Blend wherever alpha is not 0.
  // If there is no alpha (ie: not 4 components), then blend wherever
  // there is a non-zero component.
  // Speed up the special cases by doing nothing when opacity=0, and
  // overwriting when opacity=1.
  else
  {
    alpha = self->GetOpacity(layer);
    beta = 1.0 - alpha;
    
    // Each voxel has an Alpha value 
    if (ncomp == 4)
    {
      // Overwrite
      if (alpha == 1.0)
      {
        for (idxZ = 0; idxZ <= maxZ; idxZ++) {
        for (idxY = 0; idxY <= maxY; idxY++) {
        for (idxX = 0; idxX <= maxX; idxX++)
        {
          // Check alpha
          if (inPtr[3]) 
          {
            memcpy(outPtr, inPtr, pixSize);
          }
          inPtr  += ncomp;
          outPtr += ncomp;
        }//for x
        outPtr += outIncY;
        inPtr += outIncY;
        }//for y
        outPtr += outIncZ;
        inPtr += outIncZ;
        }//for z
      }
      // Blend 
      else if (alpha != 0)
      {
        for (idxZ = 0; idxZ <= maxZ; idxZ++) {
        for (idxY = 0; idxY <= maxY; idxY++) {
        for (idxX = 0; idxX <= maxX; idxX++)
        {
          if (inPtr[3]) 
          {
            for (c=0; c<ncomp; c++)
            {
              outPtr[c] = (T)(outPtr[c]*beta + inPtr[c]*alpha);
            }
          }
          inPtr  += ncomp;
          outPtr += ncomp;
        }//for x
        outPtr += outIncY;
        inPtr += outIncY;
        }//for y
        outPtr += outIncZ;
        inPtr += outIncZ;
        }//for z
      }//blend
    }//4-comp

    // Not 4-components
    else
    {
      // Overwrite
      if (alpha == 1.0)
      {
        for (idxZ = 0; idxZ <= maxZ; idxZ++) {
        for (idxY = 0; idxY <= maxY; idxY++) {
        for (idxX = 0; idxX <= maxX; idxX++)
        {
          // See if this pixel has any non-zero components
          zero = 1;
          for (c=0; c<ncomp; c++)
          {
            if (inPtr[c]) {
              zero = 0;
            }
          }
          // If input is non-zero, then copy to output
          if (!zero)
          {
            memcpy(outPtr, inPtr, pixSize);
          }
          inPtr  += ncomp;
          outPtr += ncomp;
        }//for x
        outPtr += outIncY;
        inPtr += outIncY;
        }//for y
        outPtr += outIncZ;
        inPtr += outIncZ;
        }//for z
      }
      // Blend 
      else if (alpha != 0)
      {
        for (idxZ = 0; idxZ <= maxZ; idxZ++) {
        for (idxY = 0; idxY <= maxY; idxY++) {
        for (idxX = 0; idxX <= maxX; idxX++)
        {
          // See if this pixel has any non-zero components
          zero = 1;
          for (c=0; c<ncomp; c++)
          {
            if (inPtr[c]) {
              zero = 0;
            }
          }
          // If input is non-zero, then blend it
          if (!zero)
          {
            for (c=0; c<ncomp; c++)
            {
              outPtr[c] = (T)(outPtr[c]*beta + inPtr[c]*alpha);
            }
          }
          inPtr  += ncomp;
          outPtr += ncomp;
        }//for x
        outPtr += outIncY;
        inPtr += outIncY;
        }//for y
        outPtr += outIncZ;
        inPtr += outIncZ;
        }//for z
      }//blend
    }//not 4-comp
  }//not first layer
}

//----------------------------------------------------------------------------
// Description:
// This method is passed a input and output regions, and executes the filter
// algorithm to fill the output from the inputs.
// It just executes a switch statement to call the correct function for
// the regions data types.
void vtkImageOverlay::ThreadedExecute(vtkImageData **inData, 
  vtkImageData *outData, int outExt[6], int id)
{
  int inExt[6];
  int firstFound, first;
  int layer, x1, y1, z1, x2, y2, z2, s1, s2, c1, c2;
  void *inPtr, *outPtr;

  // Ensure we have opacity info for each layer
  if (this->nOpacity < this->NumberOfInputs) 
  {
    this->UpdateForNumberOfInputs();
  }

  s1 = outData->GetScalarType();
  c1 = outData->GetNumberOfScalarComponents();
  x1 = outExt[1]-outExt[0]+1;
  y1 = outExt[3]-outExt[2]+1;
  z1 = outExt[5]-outExt[4]+1;
  
  // Loop thru each layer (input) 
  firstFound = first = 0;
  for (layer=0; layer < this->NumberOfInputs; layer++)
  {
    // If layer exists
    if (inData[layer] != NULL)
    {
      // See if this is the first non-NULL layer
      if (firstFound == 0)
      {
        firstFound = 1;
        first = 1;
      }
      else
      {
        first = 0;
      }

      // Check that the extent, scalar type, and number of components 
      // matches what the output has

      memcpy(inExt, outExt, 6*sizeof(int));
      this->ComputeRequiredInputUpdateExtent(inExt, outExt, layer);
      s2 = inData[layer]->GetScalarType();
      c2 = inData[layer]->GetNumberOfScalarComponents();
      x2 = inExt[1]-inExt[0]+1;
      y2 = inExt[3]-inExt[2]+1;
      z2 = inExt[5]-inExt[4]+1;
     
      // Extent 
      if (x1 != x2 || y1 != y2 || z1 != z2)
      {
        vtkErrorMacro(<< "Layer "<<layer<<" extent is "<<x2<<"x"<<y2<<"x"<<z2 
          << " instead of "<<x1<<"x"<<y1<<"x"<<z1);
        return;
      }

      // components
      if (c2 != c1) 
      {
        vtkErrorMacro(<<"Layer "<<layer<<" Input has "<<c2<<
          " instead of "<<c1<<" scalar components.");
         return;
      }

      // scalar type 
      if (s2 != s1) 
      {
        vtkErrorMacro(<<"Layer "<<layer<<" Input has "<<s2<<
          " instead of "<<s1<<" scalar type.");
         return;
      }

      inPtr  = inData[layer]->GetScalarPointerForExtent(inExt);
      outPtr = outData->GetScalarPointerForExtent(outExt);
  
      // Execute
      switch (inData[layer]->GetScalarType())
      {
        case VTK_FLOAT:
          vtkImageOverlayExecute(this, inData[layer], (float *)(inPtr),
            inExt, outData, (float *) outPtr, outExt, id, layer, first);
          break;
        case VTK_INT:
          vtkImageOverlayExecute(this, inData[layer], (int *)(inPtr), 
            inExt, outData, (int *) outPtr, outExt, id, layer, first);
          break;
        case VTK_SHORT:
          vtkImageOverlayExecute(this, inData[layer], (short *)(inPtr),
            inExt, outData, (short *) outPtr, outExt, id, layer, first);
          break;
        case VTK_UNSIGNED_SHORT:
          vtkImageOverlayExecute(this, inData[layer], (unsigned short *)(inPtr),
            inExt, outData, (unsigned short *) outPtr, outExt, id, layer, first);
          break;
        case VTK_UNSIGNED_CHAR:
          vtkImageOverlayExecute(this, inData[layer], (unsigned char *)(inPtr),
            inExt, outData, (unsigned char *) outPtr, outExt, id, layer, first);
          break;
        case VTK_CHAR:
          vtkImageOverlayExecute(this, inData[layer],  (char *)(inPtr),
            inExt, outData, (char *) outPtr, outExt, id, layer, first);
          break;
        case VTK_UNSIGNED_LONG:
          vtkImageOverlayExecute(this, inData[layer], (unsigned long *)(inPtr),
            inExt, outData, (unsigned long *) outPtr, outExt, id, layer, first);
          break;
        case VTK_LONG:
          vtkImageOverlayExecute(this, inData[layer], (long *)(inPtr),
            inExt, outData, (long *) outPtr, outExt, id, layer, first);
          break;
        case VTK_DOUBLE:
          vtkImageOverlayExecute(this, inData[layer], (double *)(inPtr),
            inExt, outData, (double *) outPtr, outExt, id, layer, first);
          break;
        case VTK_UNSIGNED_INT:
          vtkImageOverlayExecute(this, inData[layer], (unsigned int *)(inPtr),
            inExt, outData, (unsigned int *) outPtr, outExt, id, layer, first);
          break;
        default:
          vtkErrorMacro(<< "Execute: Unknown input ScalarType");
          return;
      }
    }
  }
}

void vtkImageOverlay::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkImageMultipleInputFilter::PrintSelf(os,indent);
  
  for (int layer=0; layer < this->GetNumberOfInputs(); layer++) 
  {
    os << indent << "Layer "<<layer<<", Opacity: " << 
      this->Opacity[layer] << "\n";
  }
}


#include <math.h>
#include "vtkImageFastGaussian.h"
#include "vtkObjectFactory.h"

//--------------------------------------------------------------------------
vtkImageFastGaussian* vtkImageFastGaussian::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkImageFastGaussian");
  if(ret)
    {
    return (vtkImageFastGaussian*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkImageFastGaussian;
}

//----------------------------------------------------------------------------
// This defines the default values for the parameters 
vtkImageFastGaussian::vtkImageFastGaussian()
{
  this->KernelSize = 5;
  this->Kernel[0] = 1;
  this->Kernel[1] = 4;
  this->Kernel[2] = 6;
  this->Kernel[3] = 4;
  this->Kernel[4] = 1;
}

//----------------------------------------------------------------------------
// This method tells the superclass that the whole input array is needed
// to compute any output region.
void vtkImageFastGaussian::ComputeInputUpdateExtent(int inExt[6], 
                                                         int outExt[6])
{
  memcpy(inExt, outExt, 6 * sizeof(int));

  // Assumes that the input update extent has been initialized to output ...
  if ( this->GetInput() != NULL ) 
    {
    this->GetInput()->GetWholeExtent(inExt);
    } 
  else 
    {
    vtkErrorMacro( "Input is NULL" );
    }
}

//----------------------------------------------------------------------------
void vtkImageFastGaussian::AllocateOutputScalars(vtkImageData *outData)
{
  outData->SetExtent(outData->GetWholeExtent());
  outData->AllocateScalars();
}

// This templated function executes the filter for any type of data.
//----------------------------------------------------------------------------
template <class T>
static void vtkImageFastGaussianExecute(vtkImageFastGaussian *self, 
                             vtkImageData *outData, int outExt[6], T *outPtr)
{
  int outMin0, outMax0, outMin1, outMax1, outMin2, outMax2;
  double accum, normalizeFactor, kernelSum;
  int nx, ny, nz, ks=self->GetKernelSize(), rx=ks/2;
  int y, z, k, offset, h, c, t, nk, outIncX, outIncY, outIncZ;
  double *cache, *kernel = self->GetKernel();
  
  // When the extent is 0-based, then an index into it can be calculated as:
  // idx = zi*nxy + ni*nx + x
  // instead of the slower:
  // idx = (zi-inExt[4])*nxy + (yi-inExt[2])*nx + (xi-inExt[0])
  // so we'll only handle 0-based for now.
  if (outExt[0] != 0 || outExt[2] != 0 || outExt[4] != 0)
  {
    fprintf(stderr, "Change vtkImageFastGaussianExecute to handle non-0-based extents.\n");
    return;
  }

  // Reorder axes (The outs here are just placeholdes
  self->PermuteExtent(outExt, outMin0,outMax0,outMin1,outMax1,outMin2,outMax2);
  self->PermuteIncrements(outData->GetIncrements(), outIncX, outIncY, outIncZ);

  nx = outMax0 - outMin0 + 1;
    ny = outMax1 - outMin1 + 1;
    nz = outMax2 - outMin2 + 1;
  
  // Radius need not be larger than image
  // (or I'd have to change the loop controls below)
  if (rx >= nx) rx = nx-1;

  // No radius, no filtering.
  if (rx == 0) return;

  // After each iteration:
  //  nk = kernel size
  //   c = kernel center (next output to write)
  //   h = kernel head   (next input to add to accumulator)
  //   t = kernel tail   (next input to remove from accumulator)

  // Caching the input provides a small improvement, but smoothing across Z
  // still takes 7 times longer than X due to cache misses! DAVE
  cache = new double[nx];

  for (z=0; z < nz; z++)
  {
    for (y=0; y < ny; y++)
    {
      offset = z*outIncZ + y*outIncY;

      // Pre-cache the input
      T *ptr = &outPtr[offset];
      for (c=0; c < nx; c++) 
      {
        cache[c] = (double)(*ptr);
        ptr += outIncX;
      }
      ptr = &outPtr[offset];

      // Init accumulator to contain righthand half of kernel
      kernelSum = 0;
      nk = 0;
      for (t=-1, h=0; h < rx; h++) 
      { 
        kernelSum += kernel[nk];
        nk++;
      }

      // Move kernel onto image from left end
      for (c=0; c <= rx; h++, c++)
      {
        kernelSum += kernel[nk];
        nk++;
        accum = 0;
        for (k=ks-nk; k<ks; k++)
          accum += cache[t+1+k] * kernel[k];

        *ptr = (T)(accum / (double)kernelSum);
        ptr += outIncX;
      }

      // Move kernel across middle of image
      normalizeFactor = 1.0 / (double)kernelSum;
      for (t=0; h < nx; h++, c++, t++)
      {
        accum = 0;
        for (k=0; k<nk; k++)
          accum += cache[t+1+k] * kernel[k];

        *ptr = (T)(accum * normalizeFactor);
        ptr += outIncX;
      }

      // Move kernel off right end of image
      for (h=nx-1; c < nx; c++, t++)
      {
        kernelSum -= kernel[ks-nk];
        nk--;
        accum = 0;
        for (k=0; k<nk; k++)
          accum += cache[t+1+k] * kernel[k];

        *ptr = (T)(accum / (double)kernelSum);
        ptr += outIncX;
      }
    }
  }

  delete [] cache;
}

//----------------------------------------------------------------------------
// This method is passed input and output Datas, and executes the EuclideanDistance
// algorithm to fill the output from the input.
void vtkImageFastGaussian::IterativeExecuteData(vtkImageData *inData,
                                                     vtkImageData *outData)
{
  void *inPtr;
  void *outPtr;
  int outExt[6];
  outData->GetWholeExtent( outExt );
  
//  inPtr = inData->GetScalarPointerForExtent(inData->GetUpdateExtent());
  inPtr  = inData->GetScalarPointer();
  outPtr = outData->GetScalarPointer();
    
  vtkDebugMacro(<<"Fast Gaussian Smoothing by Convolution");
  
  // this filter expects input to have 1 components
  if (outData->GetNumberOfScalarComponents() != 1 )
  {
    vtkErrorMacro(<< "Execute: Cannot handle more than 1 components");
    return;
  }

  // Copy input data to output so I can operate in place.
  // NOTE: The vtkImageIterativeFilter uses passes output data of each
  //   iteration as the input to the next iteration. So, the input
  //   data is at a different memory location at each iteration.
  if( inData != outData )
  {
     outData->DeepCopy(inData);
  }
  
  // Execute the algorithms 
  switch (outData->GetScalarType()) 
  {
    vtkTemplateMacro4(vtkImageFastGaussianExecute, 
      this, outData, outExt, (VTK_TT *)outPtr);
     default:
        vtkErrorMacro("Execute: Unknown input ScalarType");
        return;
    }
  
  this->UpdateProgress((this->GetIteration()+1.0)/3.0);
}

//----------------------------------------------------------------------------
void vtkImageFastGaussian::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkImageDecomposeFilter::PrintSelf(os,indent);

  os << indent << "Kernel Size: " << this->KernelSize << "\n";
}
  

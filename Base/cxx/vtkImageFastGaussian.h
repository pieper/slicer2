// .NAME vtkImageFastGaussian - computes 3D Gaussian smooth fast
// .SECTION Description
// Convolves image with a linear kernel along each axis, one at a time.
 

#ifndef __vtkImageFastGaussian_h
#define __vtkImageFastGaussian_h

#include "vtkImageData.h"
#include "vtkImageDecomposeFilter.h"

class VTK_EXPORT vtkImageFastGaussian : public vtkImageDecomposeFilter
{
public:
  static vtkImageFastGaussian *New();
  vtkTypeMacro(vtkImageFastGaussian,vtkImageDecomposeFilter);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Kernel size should be settable in the future, but it is currently fixed at 5.
  vtkGetMacro(KernelSize, int);

  // Description:
  // Templated functions need access to the kernel
  double *GetKernel() {return this->Kernel;};
  
  // Flags {1,0} indicating which IJK axes to filter.
  vtkSetVector3Macro(AxisFlags, int);
  vtkGetVector3Macro(AxisFlags, int);

  void IterativeExecuteData(vtkImageData *in, vtkImageData *out);
  
protected:
  vtkImageFastGaussian();
  ~vtkImageFastGaussian() {};

  double Kernel[5];
  int KernelSize;
  int AxisFlags[3];

  // Replaces "EnlargeOutputUpdateExtent"
  virtual void AllocateOutputScalars(vtkImageData *outData);
  
  void ComputeInputUpdateExtent(int inExt[6], int outExt[6]);
};

#endif











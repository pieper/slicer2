// .NAME vtkTensorRegularization - Filtering operations on tensors.
//
// .SECTION Description
// The input/output is tensors, should this really be 
// an imaging filter?  
//
#ifndef __vtkTensorRegularization_h
#define __vtkTensorRegularization_h

#include "vtkDTMRIConfigure.h"
#include "vtkImageToImageFilter.h"

class VTK_DTMRI_EXPORT vtkTensorRegularization : public vtkImageToImageFilter
{
 public:
  static vtkTensorRegularization *New();
  vtkTypeMacro(vtkTensorRegularization,vtkImageToImageFilter);
  void PrintSelf(ostream& os, vtkIndent indent);

  vtkGetMacro(ExtractEigenvalues,int);
  vtkSetMacro(ExtractEigenvalues,int);

protected:
  vtkTensorRegularization();
  ~vtkTensorRegularization();
  vtkTensorRegularization(const vtkTensorRegularization&) {};
  void operator=(const vtkTensorRegularization&) {};

  int ExtractEigenvalues;


  void ThreadedExecute(vtkImageData *inData, vtkImageData *outData,
		       int extent[6], int id);

  // We override this in order to allocate output tensors
  // before threading happens.  This replaces the superclass 
  // vtkImageToImageFilter's Execute function.
  void ExecuteData(vtkDataObject *out);
};

#endif




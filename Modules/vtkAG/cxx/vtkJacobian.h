// .NAME vtkJacobian - 
// 
// .SECTION Description
// 
// .SECTION See Also

#ifndef __vtkJacobian_h
#define __vtkJacobian_h

#include <vtkAGConfigure.h>

#include <stdio.h>
#include <vtkImageToImageFilter.h>

class VTK_AG_EXPORT vtkJacobian : public vtkImageToImageFilter {
public:
  static vtkJacobian* New();
  vtkTypeMacro(vtkJacobian,vtkImageToImageFilter);
  void PrintSelf(ostream& os, vtkIndent indent);

protected:
  vtkJacobian();
  ~vtkJacobian();
  vtkJacobian(const vtkJacobian&) {}
  void operator=(const vtkJacobian&) {}
  void ThreadedExecute(vtkImageData *inData, vtkImageData *outData,
		       int extent[6], int id);

  void ExecuteInformation(vtkImageData *inData, vtkImageData *outData);
};
#endif



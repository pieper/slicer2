// .NAME vtkSymMat3x3Determinant - 
// 
// .SECTION Description
// 
// .SECTION See Also

#ifndef __vtkSymMat3x3Determinant_h
#define __vtkSymMat3x3Determinant_h

#include <vtkAGConfigure.h>

#include <stdio.h>
#include <vtkImageToImageFilter.h>

class VTK_AG_EXPORT vtkSymMat3x3Determinant : public vtkImageToImageFilter {
public:
  static vtkSymMat3x3Determinant* New();
  vtkTypeMacro(vtkSymMat3x3Determinant,vtkImageToImageFilter);
  void PrintSelf(ostream& os, vtkIndent indent);
  
protected:
  vtkSymMat3x3Determinant();
  ~vtkSymMat3x3Determinant();
  vtkSymMat3x3Determinant(const vtkSymMat3x3Determinant&) {}
  void operator=(const vtkSymMat3x3Determinant&) {}
  void ExecuteInformation(vtkImageData *inData,vtkImageData *outData);
  void ThreadedExecute(vtkImageData *inDatas, vtkImageData *outData,
		       int extent[6], int id);
};
#endif



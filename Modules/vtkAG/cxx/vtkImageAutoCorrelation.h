// .NAME vtkImageAutoCorrelation - 
// 
// .SECTION Description
// 
// .SECTION See Also

#ifndef __vtkImageAutoCorrelation_h
#define __vtkImageAutoCorrelation_h

#include <vtkAGConfigure.h>

#include <stdio.h>
#include <vtkImageToImageFilter.h>

class VTK_AG_EXPORT vtkImageAutoCorrelation : public vtkImageToImageFilter {
public:
  static vtkImageAutoCorrelation* New();
  vtkTypeMacro(vtkImageAutoCorrelation,vtkImageToImageFilter);
  void PrintSelf(ostream& os, vtkIndent indent);
  
protected:
  vtkImageAutoCorrelation();
  ~vtkImageAutoCorrelation();
  vtkImageAutoCorrelation(const vtkImageAutoCorrelation&) {}
  void operator=(const vtkImageAutoCorrelation&) {}
  void ExecuteInformation(vtkImageData *inData,vtkImageData *outData);
  void ThreadedExecute(vtkImageData *inDatas, vtkImageData *outData,
		       int extent[6], int id);
};
#endif



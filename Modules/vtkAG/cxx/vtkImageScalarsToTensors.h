// .NAME vtkImageScalarsToTensors - 
// 
// .SECTION Description
// 
// .SECTION See Also

#ifndef __vtkImageScalarsToTensors_h
#define __vtkImageScalarsToTensors_h

#include <vtkAGConfigure.h>

#include <vtkImageToImageFilter.h>

class VTK_AG_EXPORT vtkImageScalarsToTensors : public vtkImageToImageFilter {
public:
  static vtkImageScalarsToTensors* New();
  vtkTypeMacro(vtkImageScalarsToTensors,vtkImageToImageFilter);
  void PrintSelf(ostream& os, vtkIndent indent);
  
protected:
  vtkImageScalarsToTensors() {}
  ~vtkImageScalarsToTensors() {}
  vtkImageScalarsToTensors(const vtkImageScalarsToTensors&) {}
  void operator=(const vtkImageScalarsToTensors&) {}
  void ExecuteData(vtkDataObject *out);
  void ThreadedExecute(vtkImageData *inData, vtkImageData *outData,
                int extent[6], int id);
};
#endif



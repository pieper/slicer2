// .NAME vtkImageTransformIntensity - 
// 
// .SECTION Description
// 
// .SECTION See Also

#ifndef __vtkImageTransformIntensity_h
#define __vtkImageTransformIntensity_h

#include <vtkAGConfigure.h>

#include <stdio.h>
#include <vtkImageToImageFilter.h>
#include <vtkIntensityTransform.h>

class VTK_AG_EXPORT vtkImageTransformIntensity : public vtkImageToImageFilter {
public:
  static vtkImageTransformIntensity* New();
  vtkTypeMacro(vtkImageTransformIntensity,vtkImageToImageFilter);
  void PrintSelf(ostream& os, vtkIndent indent);
      
  vtkSetObjectMacro(IntensityTransform,vtkIntensityTransform);
  vtkGetObjectMacro(IntensityTransform,vtkIntensityTransform);
      
protected:
  vtkImageTransformIntensity();
  ~vtkImageTransformIntensity();
  vtkImageTransformIntensity(const vtkImageTransformIntensity&) {}
  void operator=(const vtkImageTransformIntensity&) {}
  void ThreadedExecute(vtkImageData *inDatas, vtkImageData *outData,
               int extent[6], int id);
      
private:
  vtkIntensityTransform* IntensityTransform;
};
#endif



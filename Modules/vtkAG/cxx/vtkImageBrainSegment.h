// .NAME vtkImageBrainSegment - 
// 
// .SECTION Description
// 
// .SECTION See Also

#ifndef __vtkImageBrainSegment_h
#define __vtkImageBrainSegment_h

#include <vtkAGConfigure.h>

#include <stdio.h>
#include <vtkImageToImageFilter.h>

class VTK_AG_EXPORT vtkImageBrainSegment : public vtkImageToImageFilter {
public:
  static vtkImageBrainSegment* New();
  vtkTypeMacro(vtkImageBrainSegment,vtkImageToImageFilter);
  void PrintSelf(ostream& os, vtkIndent indent);

  vtkSetMacro(ErodeKernelSize,int);
  vtkGetMacro(ErodeKernelSize,int);
  
  vtkSetMacro(DilateKernelSize,int);
  vtkGetMacro(DilateKernelSize,int);
  
protected:
  vtkImageBrainSegment();
  ~vtkImageBrainSegment();
  vtkImageBrainSegment(const vtkImageBrainSegment&) {}
  void operator=(const vtkImageBrainSegment&) {}
  void ExecuteData(vtkDataObject *output);
  void ExecuteInformation(vtkImageData *inData, vtkImageData *outData);

private:
  int Average(vtkImageData* img,int thesh);
  int ErodeKernelSize;
  int DilateKernelSize;
};
#endif



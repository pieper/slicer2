// .NAME vtkImageWarpForce - 
// 
// .SECTION Description
// 
// .SECTION See Also

#ifndef __vtkImageWarpForce_h
#define __vtkImageWarpForce_h

#include <vtkAGConfigure.h>

#include <vtkImageMultipleInputFilter.h>

class VTK_AG_EXPORT vtkImageWarpForce : public vtkImageMultipleInputFilter
{
public:
  static vtkImageWarpForce* New();
  vtkTypeMacro(vtkImageWarpForce,vtkImageMultipleInputFilter);
  void PrintSelf(ostream& os, vtkIndent indent);

  virtual void SetTarget(vtkImageData *input)
  {
    this->SetInput(0, input);
  }
  vtkImageData* GetTarget();
  
  virtual void SetSource(vtkImageData *input)
  {
    this->SetInput(1, input);
  }
  vtkImageData* GetSource();
  
  virtual void SetDisplacement(vtkImageData *input)
  {
    this->SetInput(2, input);
  }
  vtkImageData* GetDisplacement();

  virtual void SetMask(vtkImageData *input)
  {
    this->SetInput(3, input);
  }
  vtkImageData* GetMask();

protected:
  vtkImageWarpForce();
  ~vtkImageWarpForce();
  vtkImageWarpForce(const vtkImageWarpForce&);
  void operator=(const vtkImageWarpForce&);
  void ExecuteInformation(vtkImageData **inDatas, vtkImageData *outData);
};
#endif

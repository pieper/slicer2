// .NAME vtkImageWarpDMForce - 
// 
// .SECTION Description
// 
// .SECTION See Also

#ifndef __vtkImageWarpDMForce_h
#define __vtkImageWarpDMForce_h

#include <vtkAGConfigure.h>

#include <vtkImageWarpForce.h>

class VTK_AG_EXPORT vtkImageWarpDMForce : public vtkImageWarpForce
{
public:
  static vtkImageWarpDMForce* New();
  vtkTypeMacro(vtkImageWarpDMForce,vtkImageWarpForce);
  void PrintSelf(ostream& os, vtkIndent indent);

protected:
  vtkImageWarpDMForce();
  ~vtkImageWarpDMForce();
  vtkImageWarpDMForce(const vtkImageWarpDMForce&);
  void operator=(const vtkImageWarpDMForce&);
  void ThreadedExecute(vtkImageData **inDatas, vtkImageData *outData,
		       int extent[6], int id);
};
#endif

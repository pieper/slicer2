// .NAME vtkImageWarpOFForce - 
// 
// .SECTION Description
// 
// .SECTION See Also

#ifndef __vtkImageWarpOFForce_h
#define __vtkImageWarpOFForce_h

#include <vtkAGConfigure.h>

#include <vtkImageWarpForce.h>

class VTK_AG_EXPORT vtkImageWarpOFForce : public vtkImageWarpForce
{
public:
  static vtkImageWarpOFForce* New();
  vtkTypeMacro(vtkImageWarpOFForce,vtkImageWarpForce);
  void PrintSelf(ostream& os, vtkIndent indent);

protected:
  vtkImageWarpOFForce();
  ~vtkImageWarpOFForce();
  vtkImageWarpOFForce(const vtkImageWarpOFForce&);
  void operator=(const vtkImageWarpOFForce&);
  void ThreadedExecute(vtkImageData **inDatas, vtkImageData *outData,
		       int extent[6], int id);
};
#endif

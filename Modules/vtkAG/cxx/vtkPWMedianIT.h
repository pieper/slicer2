#ifndef __vtkPWMedianIT_h
#define __vtkPWMedianIT_h

#include <vtkAGConfigure.h>

#include <vtkPWConstantIT.h>

class VTK_AG_EXPORT vtkPWMedianIT : public vtkPWConstantIT
{
public:
  static vtkPWMedianIT* New();
  vtkTypeMacro(vtkPWMedianIT,vtkPWConstantIT);
  void PrintSelf(ostream& os, vtkIndent indent);
      
protected:
  vtkPWMedianIT();
  ~vtkPWMedianIT();
  vtkPWMedianIT(const vtkPWMedianIT&);
  void operator=(const vtkPWMedianIT&);

  void InternalUpdate();
};

#endif



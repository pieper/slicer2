// .NAME vtkIntensityTransform - 
// 
// .SECTION Description
// 
// .SECTION See Also

#ifndef __vtkIntensityTransform_h
#define __vtkIntensityTransform_h

#include <vtkAGConfigure.h>

#include <vtkFunctionSet.h>
#include <vtkMutexLock.h>
#include <vtkImageData.h>

class VTK_AG_EXPORT vtkIntensityTransform : public vtkFunctionSet
{
public:
  vtkTypeMacro(vtkIntensityTransform,vtkFunctionSet);
  void PrintSelf(ostream& os, vtkIndent indent);
      
  virtual void Update();
      
  vtkSetObjectMacro(Target,vtkImageData);
  vtkGetObjectMacro(Target,vtkImageData);
  vtkSetObjectMacro(Source,vtkImageData);
  vtkGetObjectMacro(Source,vtkImageData);
  vtkSetObjectMacro(Mask,vtkImageData);
  vtkGetObjectMacro(Mask,vtkImageData);

protected:
  vtkIntensityTransform();
  ~vtkIntensityTransform();
  vtkIntensityTransform(const vtkIntensityTransform&);
  void operator=(const vtkIntensityTransform&);

  virtual void InternalUpdate() {};
  
  vtkImageData* Target;
  vtkImageData* Source;
  vtkImageData* Mask;

private:
  vtkTimeStamp UpdateTime;
  vtkSimpleMutexLock* UpdateMutex;
};

#endif



#ifndef __vtkPWConstantIT_h
#define __vtkPWConstantIT_h

#include <vtkAGConfigure.h>

#include <vtkIntensityTransform.h>

class VTK_AG_EXPORT vtkPWConstantIT : public vtkIntensityTransform
{
public:
  static vtkPWConstantIT* New();
  vtkTypeMacro(vtkPWConstantIT,vtkIntensityTransform);
  void PrintSelf(ostream& os, vtkIndent indent);
      
  void SetNumberOfFunctions(int n);
  int FunctionValues(vtkFloatingPointType* x, vtkFloatingPointType* f);

  void SetNumberOfPieces(int i, int p);
  int GetNumberOfPieces(int i);
  
  void SetBoundary(int i, int j, int p);
  int GetBoundary(int i,int j);
  
  void SetValue(int i,int j,int v);
  int GetValue(int i,int j);
  
protected:
  vtkPWConstantIT();
  ~vtkPWConstantIT();
  vtkPWConstantIT(const vtkPWConstantIT&);
  void operator=(const vtkPWConstantIT&);

  void DeleteFunction(int i);
  void DeleteFunctions();
  void BuildFunction(int i);
  void BuildFunctions();

  int* NumberOfPieces;
  int** Boundaries;
  int** Values;
};

#endif



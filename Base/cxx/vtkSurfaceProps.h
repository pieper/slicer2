#ifndef __vtkSurfaceProps_h
#define __vtkSurfaceProps_h

#include "vtkProcessObject.h"
#include "vtkPolyData.h"
#include "vtkCellTriMacro.h"

class VTK_EXPORT vtkSurfaceProps : public vtkProcessObject
{
public:
  vtkSurfaceProps();
  static vtkSurfaceProps *New() {return new vtkSurfaceProps;};
  const char *GetClassName() {return "vtkSurfaceProps";};

  void Update();
  void SetInput(vtkPolyData *input);
  vtkPolyData *GetInput();

  vtkGetMacro(SurfaceArea,float);
  vtkGetMacro(MinCellArea,float);
  vtkGetMacro(MaxCellArea,float);
  vtkGetMacro(Volume,float);
  vtkGetMacro(VolumeError,float);

protected:
  void Execute();
  float SurfaceArea;
  float MinCellArea;
  float MaxCellArea;
  float Volume;
  float VolumeError;
};

#endif

#ifndef __vtkPreciseHyperStreamlinePoints_h
#define __vtkPreciseHyperStreamlinePoints_h

#include "vtkTensorUtilConfigure.h"
#include "vtkPreciseHyperStreamline.h"
#include "vtkPoints.h"

class VTK_TENSORUTIL_EXPORT vtkPreciseHyperStreamlinePoints : public vtkPreciseHyperStreamline
{
public:
  static vtkPreciseHyperStreamlinePoints *New();
  vtkTypeMacro(vtkPreciseHyperStreamlinePoints,vtkDataSetToPolyDataFilter);
  void PrintSelf(ostream& os, vtkIndent indent);

  vtkGetObjectMacro(PreciseHyperStreamline0,vtkPoints);
  vtkGetObjectMacro(PreciseHyperStreamline1,vtkPoints);

protected:
  vtkPreciseHyperStreamlinePoints();
  ~vtkPreciseHyperStreamlinePoints();

  void Execute();
  
  // convenient pointers to PreciseHyperStreamline1 and PreciseHyperStreamline2
  vtkPoints *PreciseHyperStreamlines[2];
  
  // points calculated for first hyperstreamline
  vtkPoints *PreciseHyperStreamline0;
  // points calculated for optional second hyperstreamline
  vtkPoints *PreciseHyperStreamline1;

private:
  vtkPreciseHyperStreamlinePoints(const vtkPreciseHyperStreamlinePoints&);  // Not implemented.
  void operator=(const vtkPreciseHyperStreamlinePoints&);  // Not implemented.
};

#endif



#ifndef __vtkHyperStreamlinePoints_h
#define __vtkHyperStreamlinePoints_h

#include "vtkTensorUtilConfigure.h"
#include "vtkHyperStreamlineDTMRI.h"
#include "vtkPoints.h"

//class VTK_TENSORUTIL_EXPORT vtkHyperStreamlinePoints : public vtkHyperStreamline
class VTK_TENSORUTIL_EXPORT vtkHyperStreamlinePoints : public vtkHyperStreamlineDTMRI
{
public:
  static vtkHyperStreamlinePoints *New();
  vtkTypeMacro(vtkHyperStreamlinePoints,vtkDataSetToPolyDataFilter);
  void PrintSelf(ostream& os, vtkIndent indent);

  vtkGetObjectMacro(HyperStreamline0,vtkPoints);
  vtkGetObjectMacro(HyperStreamline1,vtkPoints);

protected:
  vtkHyperStreamlinePoints();
  ~vtkHyperStreamlinePoints();

  void Execute();
  
  // convenient pointers to HyperStreamline1 and HyperStreamline2
  vtkPoints *HyperStreamlines[2];
  
  // points calculated for first hyperstreamline
  vtkPoints *HyperStreamline0;
  // points calculated for optional second hyperstreamline
  vtkPoints *HyperStreamline1;

private:
  vtkHyperStreamlinePoints(const vtkHyperStreamlinePoints&);  // Not implemented.
  void operator=(const vtkHyperStreamlinePoints&);  // Not implemented.
};

#endif



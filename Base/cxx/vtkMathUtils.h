#ifndef __vtkMathUtils_h
#define __vtkMathUtils_h

#include "vtkObject.h"
#include "vtkPoints.h"
#include "vtkScalars.h"
#include "vtkVectors.h"
#include "vtkMatrix4x4.h"

class VTK_EXPORT vtkMathUtils : public vtkObject
{
public:
  static vtkMathUtils *New() {return new vtkMathUtils;};
  const char *GetClassName() {return "vtkMathUtils";};

  static int PrincipalMomentsAndAxes( vtkPoints *Points, vtkScalars *Weights,
                                      vtkScalars *Values, vtkVectors *Vectors );
  static int AlignPoints( vtkPoints *Data, vtkPoints *Ref,
                          vtkMatrix4x4 *Xform );
  static void SVD3x3( float A[][3], float U[][3], float W[], float V[][3] );
};

#endif

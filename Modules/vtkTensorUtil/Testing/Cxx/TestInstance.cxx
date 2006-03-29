#include "vtkBSplineInterpolateImageFunction.h"

int TestInstance(int argc, char *argv[])
{
  vtkBSplineInterpolateImageFunction *f = vtkBSplineInterpolateImageFunction::New();
  f->Delete();
  return 0;
}

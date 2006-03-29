#include "vtkBSplineInterpolateImageFunction.h"
#include "vtkHyperPointandArray.h"
#include "vtkHyperStreamlineDTMRI.h"
//#include "vtkHyperStreamlineTeem.h"
#include "vtkImageGetTensorComponents.h"
#include "vtkImageSetTensorComponents.h"
#include "vtkInteractiveTensorGlyph.h"
#include "vtkPreciseHyperArray.h"
#include "vtkPreciseHyperPoint.h"
#include "vtkPreciseHyperStreamline.h"
#include "vtkPreciseHyperStreamlinePoints.h"
#include "vtkSuperquadricSource2.h"
#include "vtkSuperquadricTensorGlyph.h"
#include "vtkTensorFlip.h"
#include "vtkTensorImplicitFunctionToFunctionSet.h"
#include "vtkTensorMask.h"
#include "vtkTensorMathematics.h"
#include "vtkVectorToOuterProductDualBasis.h"

int TestInstance(int argc, char *argv[])
{
  vtkBSplineInterpolateImageFunction::New()->Delete();
  //vtkHyperPointandArray::New()->Delete();
  vtkHyperStreamlineDTMRI::New()->Delete();
  //vtkHyperStreamlineTeem::New()->Delete();
  vtkImageGetTensorComponents::New()->Delete();
  vtkImageSetTensorComponents::New()->Delete();
  vtkInteractiveTensorGlyph::New()->Delete();
  //vtkPreciseHyperArray::New()->Delete();
  //vtkPreciseHyperPoint::New()->Delete();
  vtkPreciseHyperStreamline::New()->Delete();
  vtkPreciseHyperStreamlinePoints::New()->Delete();
  vtkSuperquadricSource2::New()->Delete();
  vtkSuperquadricTensorGlyph::New()->Delete();
  vtkTensorFlip::New()->Delete();
  vtkTensorImplicitFunctionToFunctionSet::New()->Delete();
  vtkTensorMask::New()->Delete();
  vtkTensorMathematics::New()->Delete();
  vtkVectorToOuterProductDualBasis::New()->Delete();

  // Test non-vtkObject
  vtkPreciseHyperPoint a;
  vtkPreciseHyperArray b;
  vtkHyperPoint c;
  vtkHyperArray d;

  return 0;
}


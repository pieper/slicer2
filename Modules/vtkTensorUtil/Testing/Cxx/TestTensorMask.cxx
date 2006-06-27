#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkDoubleArray.h"
#include "vtkImageToImageFilter.h"
#include "vtkTensorMask.h"
#include "vtkImageEllipsoidSource.h"

int TestTensorMask(int, char *[])
{
  vtkImageData *img = vtkImageData::New();
  img->SetDimensions(6,6,6);
  vtkIdType numPts = img->GetNumberOfPoints();

  vtkDoubleArray *tensors = vtkDoubleArray::New();
  tensors->SetNumberOfComponents(9);
  tensors->SetName("TestTensors");
  tensors->SetNumberOfTuples(numPts);
  img->GetPointData()->SetTensors( tensors );
  img->Update();
  img->Print ( cout );
  memset( tensors->GetPointer(0), 0, 9*numPts*sizeof(double));


  tensors->Delete();
  img->Delete();

  return 0;
}

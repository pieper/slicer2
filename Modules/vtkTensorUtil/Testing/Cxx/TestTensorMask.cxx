#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkDoubleArray.h"
#include "vtkImageToImageFilter.h"
#include "vtkTensorMask.h"
#include "vtkImageEllipsoidSource.h"
#include "vtkPointLoad.h"
#include "vtkImageViewer2.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkImageDataGeometryFilter.h"
#include "vtkStructuredPointsWriter.h"

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

#if 0
//  vtkPointLoad *ptLoad = vtkPointLoad::New();
//    ptLoad->SetLoadValue(100.0);
//    ptLoad->SetSampleDimensions(6,6,6);
//    ptLoad->ComputeEffectiveStressOn();
//    ptLoad->SetModelBounds(-10,10,-10,10,-10,10);
//  //pl->Update();
  vtkStructuredPointsWriter *writer = vtkStructuredPointsWriter::New();
  //writer->SetInput( ptLoad->GetOutput() );
  writer->SetInput( img );
  writer->SetFileName( "/tmp/ptload.vtk" );
  writer->Write();
  //ptLoad->GetOutput()->Print( cout );

 // extract plane of data
//vtkImageDataGeometryFilter *plane = vtkImageDataGeometryFilter::New();
//    plane->SetInput(ptLoad->GetOutput());
//    plane->SetExtent (2,2,0,99,0,99);

//  vtkImageEllipsoidSource *sphere = vtkImageEllipsoidSource::New();
//  sphere->SetWholeExtent(pl->GetOutput()->GetWholeExtent());
//  sphere->SetCenter(pl->GetOutput()->GetCenter());
//  sphere->SetRadius(25,25,25);
//  sphere->Update();
//
//  vtkTensorMask * t = vtkTensorMask::New();
//  t->SetImageInput (pl->GetOutput());
//  t->SetMaskInput(sphere->GetOutput());
//  t->Update();
//
//  t->GetOutput()->Print( cout );

//  vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::New();
//  
//  vtkImageViewer2 *viewer = vtkImageViewer2::New();
//  //viewer->SetInput(t->GetOutput());
//  viewer->SetInput(ptLoad->GetOutput());
//  viewer->SetupInteractor(iren);
//  //viewer->SetColorWindow (255);
//  //viewer->SetColorLevel (128);
//  iren->Start();

  //sphere->Delete();
  //pl->Delete();
  //t->Delete();
  writer->Delete();
#endif


  tensors->Delete();
  img->Delete();

  return 0;
}

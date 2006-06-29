#include "vtkTensorMathematics.h"
#include "vtkPointLoad.h"
#include "vtkAppendPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkImageData.h"
#include "vtkImageViewer.h"
#include "vtkImageViewer2.h"
#include "vtkImageShiftScale.h"
#include "vtkPNGReader.h"
#include "vtkPNGWriter.h"
#include "vtkStructuredPointsReader.h"
#include "vtkStructuredPoints.h"
#include "vtkActor2D.h"

#include "vtkHyperStreamlineDTMRI.h"
#include <sstream>

int TestTensorMathematics(int, char *[])
{
  vtkStructuredPointsReader *reader = vtkStructuredPointsReader::New();
  reader->SetFileName( "/home/mathieu/NAMIC/caseD20_DTI.vtk" );
  //reader->Update();
  //reader->GetOutput()->Print( cout );

  vtkPointLoad *ptLoad = vtkPointLoad::New();
  ptLoad->SetLoadValue (100.0);
  ptLoad->SetSampleDimensions (50, 50, 30);
  ptLoad->ComputeEffectiveStressOn();
  ptLoad->SetModelBounds (-20, 20, -20, 20, -10, 10);

  vtkPointLoad *ptLoad2 = vtkPointLoad::New();
  ptLoad2->SetLoadValue (1.0);
  ptLoad2->SetPoissonsRatio(0.7);
  ptLoad2->SetSampleDimensions (30, 30, 30);
  ptLoad2->ComputeEffectiveStressOn();
  ptLoad2->SetModelBounds (-10, 10, -10, 10, -10, 10);

  //vtkImageData *input = reader->GetOutput();
  vtkImageData *input = ptLoad->GetOutput();
  //vtkImageData *input = ptLoad2->GetOutput();
  int column = 1;
  int row = 1;

    //double *s = t->GetOutput()->GetScalarRange();
    //std::cout << s[0] << " " << s[1] << std::endl;
    double s[2] = {
      0.0187995,
      0.00939977
    };

  vtkRenderWindow *imgWin = vtkRenderWindow::New();
  for( int i = VTK_TENS_TRACE; i<= VTK_TENS_MAX_EIGENVALUE_PROJZ; i++)
    {
    vtkTensorMathematics *t = vtkTensorMathematics::New();
    t->SetOperation( i );
    t->SetInput1( input );
    t->SetInput2( input );
    t->Update();

    t->GetOutput()->Print( cout );

    vtkImageShiftScale *ss = vtkImageShiftScale::New();
    ss->SetInput( t->GetOutput() );
    ss->SetShift( - s[0] );
    ss->SetScale( 255. / (s[1] - s[0] ) );
    ss->SetOutputScalarTypeToUnsignedChar ();
    //s = ss->GetOutput()->GetScalarRange();
    //std::cout << s[0] << " " << s[1] << std::endl;

    std::ostringstream os;
    os << "test" << i << ".png";
    vtkPNGWriter *writer = vtkPNGWriter::New();
    writer->SetInput( ss->GetOutput() );
    writer->SetFileName( os.str().c_str() );
    writer->Write();
    writer->Delete();

    vtkPNGReader *reader = vtkPNGReader::New();
    reader->SetFileName( os.str().c_str() );

    vtkImageMapper *mapper = vtkImageMapper::New();
    mapper->SetInput( t->GetOutput() );
    //mapper->SetZSlice(15);
    mapper->SetColorWindow( s[0] );
    mapper->SetColorLevel ( (s[1]-s[0])/2 );
    vtkActor2D *actor2d = vtkActor2D::New();
    actor2d->SetMapper( mapper );
    vtkRenderer *imager = vtkRenderer::New();
    imager->AddActor2D( actor2d );
    imgWin->AddRenderer( imager );

    double deltaX = 1.0/6.0;
    double deltaY = 1.0/4.0;

    imager->SetViewport( (column - 1) * deltaX, (row - 1) * deltaY,
      column * deltaX, row * deltaY );
    imager->Print( cout );
    ++column;
    if ( column > 6 ) { column = 1; ++row; }

    // make the last operator finish the row
    //double *vp = imager->GetViewport();
    //imager->SetViewport( vp[0], vp[1], 1, 1);


    //t->Delete();
    //actor2d->Delete();
    //imager->Delete();

    }

  //imgWin->Render();
  //imgWin->Delete();

#if 0
  vtkImageShiftScale *ss = vtkImageShiftScale::New();
  ss->SetInput( t->GetOutput() );
  ss->SetShift( - s[0] );
  ss->SetScale( 255 / (s[1] - s[0] ) );
  ss->SetOutputScalarTypeToUnsignedChar ();
  //ss->Update();
  //s = ss->GetOutput()->GetScalarRange();
  //std::cout << s[0] << " " << s[1] << std::endl;

  //vtkPNGWriter *writer = vtkPNGWriter::New();
  //writer->SetInput( ss->GetOutput() );
  //writer->SetFileName( "test.png" );
  //writer->Write();
  //writer->Delete();
    
  vtkImageViewer2 *viewer = vtkImageViewer2::New();
  viewer->SetInput(t->GetOutput());
  viewer->SetZSlice (15);
  viewer->SetColorWindow ((s[1] - s[0]) / 2);
  viewer->SetColorLevel ( s[0] );

  //#make interface
  //viewer->Render();

  // Define graphics objects
  //vtkRenderer *ren = vtkRenderer::New();
  //vtkRenderWindow *renWin = vtkRenderWindow::New();
  //renWin->AddRenderer(ren);
#endif
  vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::New();
  iren->SetRenderWindow(imgWin);
  imgWin->SetSize (600,300);

  //ren->SetBackground(1, 1, 1);

  iren->Initialize();
  iren->Start();

  reader->Delete();
  ptLoad->Delete();
  ptLoad2->Delete();
  //ss->Delete();
  //mapper->Delete();
  //actor->Delete();
  //renWin->Delete();
  //ren->Delete();
  //viewer->Delete();
  iren->Delete();

  return 0;
}


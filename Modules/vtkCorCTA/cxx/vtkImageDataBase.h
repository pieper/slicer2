
#ifndef  _vtkImageDataBase_h_
#define  _vtkImageDataBase_h_

#include "vtkImageData.h"
#include "vtkStructuredPointsWriter.h"

#define FALSE 0
#define TRUE  1

class vtkImageDataBase
{

 protected:

  vtkImageData* _image;

  unsigned char _image_allocated;

 public:

  vtkImageDataBase( ) 
  {
    _image = NULL;
    _image_allocated = FALSE;
  }
  
  vtkImageDataBase( vtkImageData* image) 
    { 
      _image = image; 
      _image_allocated = TRUE;
    }
  

  ~vtkImageDataBase() 
    {
      if (_image_allocated) _image->Delete();
    }


  void Save(char* name)
    {

      vtkStructuredPointsWriter* vtk_iw;

      vtk_iw = vtkStructuredPointsWriter::New();

      vtk_iw->SetInput(_image);
      vtk_iw->SetFileName(name);
      vtk_iw->SetFileTypeToBinary();

      vtk_iw->Write();
      vtk_iw->Delete();
    }

  void* Buffer();


  int X() { return _image->GetDimensions()[0]; }
  int Y() { return _image->GetDimensions()[1]; }
  int Z() { return _image->GetDimensions()[2]; }


  long Size() 
    { 
      return _image->GetPointData()->GetScalars()->GetNumberOfTuples(); 
    }

  unsigned char CoordOK( int x, int y, int z) 
    {
      return (x>=0  && y>=0  && z>=0 &&
          x<X() && y<Y() && z<Z()
          );
    }

  virtual void InitBuffer( int pos = 0) = 0;
  //           ----------
  
  //  
  virtual void BufferPos( int x, int y, int z) = 0;
  //           ---------
  
  virtual void IncBuffer( int inc=1) = 0;
  //           ---------
  
  
  virtual double BufferValue( ) = 0;
  //             -----------
  
  //
  virtual double BufferValue( int inc) = 0;
  //             -----------
  
  //  virtual double VectValueBuffer( int i ) = 0;
  //             ---------------
  
  
  ///
  virtual void SetValue( double val) = 0;
  //           --------
  
  ///
  //  virtual void VectSetValues( double valx, double valy, double valz) = 0;
  //           -------------
  
  // 
  operator vtkImageData*() { return( _image); }
  //       ------------
  

  //  {\bf Operator(x,y,z) }
  virtual double operator()( int x, int y, int z) = 0;
  //             --------
  
  //  {\em Operator(x,y,z,coord) }
  virtual double operator()( int x, int y, int z, int coord) = 0;
  //             --------
                   

}; // vtkImageData


#endif // _vtkImageDataBase_h_

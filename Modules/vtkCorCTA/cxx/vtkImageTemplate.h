
#ifndef  _vtkImageTemplate_h_
#define  _vtkImageTemplate_h_

#include "vtkImageDataBase.h"


template <class T>
class vtkImageTemplate : public vtkImageDataBase
{

protected:

  T*         _buffer;

 public:

  vtkImageTemplate( ) 
  {
    _buffer = NULL;
  }
  
  vtkImageTemplate( vtkImageData* image): vtkImageDataBase(image)
  {
    _buffer = NULL;
  }
  
  vtkImageTemplate(int type, vtkImageData* im)
  {

    vtkImageData *Target;

    Target = vtkImageData::New();
    Target->SetDimensions(im->GetDimensions());
    Target->SetSpacing(   im->GetSpacing   ());
    Target->SetScalarType(type);
    Target->SetNumberOfScalarComponents(1);
    Target->AllocateScalars();

    _image = Target;
    _image_allocated = TRUE;
    _buffer = NULL;

  }

  ~vtkImageTemplate() {}

  void* Buffer()
    { return (void*) _buffer; }

  void InitBuffer( int pos = 0) 
  //   ----------
    {
      _buffer = (T*) _image->GetScalarPointer();
      _buffer += pos;
    }
  
  //  
  void BufferPos( int x, int y, int z) 
  //   ---------
    {
      _buffer = (T*) _image->GetScalarPointer(x,y,z);
    }
  
  void IncBuffer( int inc=1)
  //   ---------
    {
      _buffer += inc;
    }
  
  
  double BufferValue( ) 
  //     ------------
    {
      return *_buffer;
    }  

  //
  double BufferValue( int inc) 
  //     ------------
    {
      return *(_buffer+inc);
    }
  
    //  double VectBufferValue( int i ) 
  //     ---------------
  
  
  ///
  void SetValue( double val) 
  //   --------
    {
      *_buffer = (T) val;
    }
  
  ///
    //  void VectSetValues( double valx, double valy, double valz) 
  //           -------------
  

  //  {\bf Operator(x,y,z) }
  double operator()( int x, int y, int z) 
  //     --------
    {
      return *((T*) _image->GetScalarPointer(x,y,z));
    }
  
  //  {\em Operator(x,y,z,coord) }
  double operator()( int x, int y, int z, int coord) 
  //     --------
    {
      return _image->GetScalarComponentAsFloat(x,y,z,coord);
    }
                   

}; // vtkImageTemplate



//------------------------------------------------------------
inline vtkImageDataBase* new_image( vtkImageData* im)
{

  switch (im->GetScalarType()) {
  case VTK_DOUBLE:         return new vtkImageTemplate<double>         (im);
  case VTK_FLOAT:          return new vtkImageTemplate<float>          (im);
  case VTK_LONG:           return new vtkImageTemplate<long>           (im);
  case VTK_UNSIGNED_LONG:  return new vtkImageTemplate<unsigned long>  (im);
  case VTK_INT:            return new vtkImageTemplate<int>            (im);
  case VTK_UNSIGNED_INT:   return new vtkImageTemplate<unsigned int>   (im);
  case VTK_SHORT:          return new vtkImageTemplate<short>          (im);
  case VTK_UNSIGNED_SHORT: return new vtkImageTemplate<unsigned short> (im);
  case VTK_CHAR:           return new vtkImageTemplate<char>           (im);
  case VTK_UNSIGNED_CHAR:  return new vtkImageTemplate<unsigned char>  (im);
  }
}



//------------------------------------------------------------
inline void delete_image( vtkImageDataBase* image){
  delete image;
}


#endif // _vtkImageTemplate_h_

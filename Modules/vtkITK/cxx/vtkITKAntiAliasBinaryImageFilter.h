/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkITKAntiAliasBinaryImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2003/01/16 18:58:07 $
  Version:   $Revision: 1.1 $
*/
// .NAME vtkITKAntiAliasBinaryImageFilter - Wrapper class around itk::AntiAliasBinaryImageFilterImageFilter
// .SECTION Description
// vtkITKAntiAliasBinaryImageFilter


#ifndef __vtkITKAntiAliasBinaryImageFilter_h
#define __vtkITKAntiAliasBinaryImageFilter_h


#include "vtkITKImageToImageFilterFF.h"
#include "itkAntiAliasBinaryImageFilter.h"
#include "vtkObjectFactory.h"

class VTK_EXPORT vtkITKAntiAliasBinaryImageFilter : public vtkITKImageToImageFilterFF
{
 public:
  static vtkITKAntiAliasBinaryImageFilter *New();
  vtkTypeRevisionMacro(vtkITKAntiAliasBinaryImageFilter, vtkITKImageToImageFilterFF);

  float GetUpperBinaryValue ()
  {
    DelegateITKOutputMacro(GetUpperBinaryValue) ;
  };

  float GetLowerBinaryValue ()
  {
    DelegateITKOutputMacro(GetLowerBinaryValue) ;
  };

  float GetIsoSurfaceValue ()
  {
    DelegateITKOutputMacro(GetIsoSurfaceValue) ;
  };

  void SetMaximumIterations ( int value )
  {
    DelegateITKInputMacro ( SetMaximumIterations, value );
  };

  void SetMaximumRMSError ( float value )
  {
    DelegateITKInputMacro ( SetMaximumRMSError, value );
  };

protected:
  //BTX
  typedef itk::AntiAliasBinaryImageFilter<Superclass::InputImageType,Superclass::InputImageType> ImageFilterType;
  vtkITKAntiAliasBinaryImageFilter() : Superclass ( ImageFilterType::New() ){};
  ~vtkITKAntiAliasBinaryImageFilter() {};
  ImageFilterType* GetImageFilterPointer() { return dynamic_cast<ImageFilterType*> ( m_Filter.GetPointer() ); }

  //ETX
  
private:
  vtkITKAntiAliasBinaryImageFilter(const vtkITKAntiAliasBinaryImageFilter&);  // Not implemented.
  void operator=(const vtkITKAntiAliasBinaryImageFilter&);  // Not implemented.
};

vtkCxxRevisionMacro(vtkITKAntiAliasBinaryImageFilter, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkITKAntiAliasBinaryImageFilter);

#endif





/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkITKOtsuThresholdImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2005/04/26 00:43:32 $
  Version:   $Revision: 1.1 $
*/
// .NAME vtkITKOtsuThresholdImageFilter - Wrapper class around itk::OtsuThresholdImageFilter
// .SECTION Description
// vtkITKOtsuThresholdImageFilter


#ifndef __vtkITKOtsuThresholdImageFilter_h
#define __vtkITKOtsuThresholdImageFilter_h


#include "vtkITKImageToImageFilterUSUS.h"
#include "itkOtsuThresholdImageFilter.h"
#include "vtkObjectFactory.h"

class VTK_EXPORT vtkITKOtsuThresholdImageFilter : public vtkITKImageToImageFilterUSUS
{
 public:
  static vtkITKOtsuThresholdImageFilter *New();
  vtkTypeRevisionMacro(vtkITKOtsuThresholdImageFilter, vtkITKImageToImageFilterUSUS);
  
  void SetNumberOfHistogramBins( unsigned long value) 
  {
    DelegateITKInputMacro ( SetNumberOfHistogramBins, value );
  };
  unsigned long GetNumberOfHistogramBins ()
  { DelegateITKOutputMacro ( GetNumberOfHistogramBins ); };
  
  void SetInsideValue (unsigned short value)
  { 
    OutputImagePixelType d = static_cast<OutputImagePixelType> ( value );
    DelegateITKInputMacro (SetInsideValue,d);
  }
  
  void SetOutsideValue (unsigned short value)
  { 
    OutputImagePixelType d = static_cast<OutputImagePixelType> ( value );
    DelegateITKInputMacro (SetOutsideValue,d);
  }
  
  unsigned short GetInsideValue ()
  { DelegateITKOutputMacro ( GetInsideValue ); };
  
  unsigned short GetOutsideValue ()
  { DelegateITKOutputMacro ( GetOutsideValue ); };
 
  unsigned short GetThreshold()
  { DelegateITKOutputMacro ( GetThreshold ); };

protected:
  //BTX
  typedef itk::OtsuThresholdImageFilter<Superclass::InputImageType, Superclass::OutputImageType> ImageFilterType;
  vtkITKOtsuThresholdImageFilter() : Superclass ( ImageFilterType::New() ){};
  ~vtkITKOtsuThresholdImageFilter() {};
  ImageFilterType* GetImageFilterPointer() { return dynamic_cast<ImageFilterType*> ( m_Filter.GetPointer() ); }
  //ETX
  
private:
  vtkITKOtsuThresholdImageFilter(const vtkITKOtsuThresholdImageFilter&);  // Not implemented.
  void operator=(const vtkITKOtsuThresholdImageFilter&);  // Not implemented.
};

vtkCxxRevisionMacro(vtkITKOtsuThresholdImageFilter, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkITKOtsuThresholdImageFilter);

#endif





/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkITKOtsuThresholdImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2005/05/03 21:50:25 $
  Version:   $Revision: 1.2 $
*/
// .NAME vtkITKOtsuThresholdImageFilter - Wrapper class around itk::OtsuThresholdImageFilter
// .SECTION Description
// vtkITKOtsuThresholdImageFilter


#ifndef __vtkITKOtsuThresholdImageFilter_h
#define __vtkITKOtsuThresholdImageFilter_h


#include "vtkITKImageToImageFilterSS.h"
#include "itkOtsuThresholdImageFilter.h"
#include "vtkObjectFactory.h"

class VTK_EXPORT vtkITKOtsuThresholdImageFilter : public vtkITKImageToImageFilterSS
{
 public:
  static vtkITKOtsuThresholdImageFilter *New();
  vtkTypeRevisionMacro(vtkITKOtsuThresholdImageFilter, vtkITKImageToImageFilterSS);
  
  void SetNumberOfHistogramBins( unsigned long value) 
  {
    DelegateITKInputMacro ( SetNumberOfHistogramBins, value );
  };
  unsigned long GetNumberOfHistogramBins ()
  { DelegateITKOutputMacro ( GetNumberOfHistogramBins ); };
  
  void SetInsideValue (short value)
  { 
    OutputImagePixelType d = static_cast<OutputImagePixelType> ( value );
    DelegateITKInputMacro (SetInsideValue,d);
  }
  
  void SetOutsideValue (short value)
  { 
    OutputImagePixelType d = static_cast<OutputImagePixelType> ( value );
    DelegateITKInputMacro (SetOutsideValue,d);
  }
  
  short GetInsideValue ()
  { DelegateITKOutputMacro ( GetInsideValue ); };
  
  short GetOutsideValue ()
  { DelegateITKOutputMacro ( GetOutsideValue ); };
 
  short GetThreshold()
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

vtkCxxRevisionMacro(vtkITKOtsuThresholdImageFilter, "$Revision: 1.2 $");
vtkStandardNewMacro(vtkITKOtsuThresholdImageFilter);

#endif





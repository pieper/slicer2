/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkITKOtsuMultipleThresholdsImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2005/05/02 22:59:33 $
  Version:   $Revision: 1.1 $
*/
// .NAME vtkITKOtsuMultipleThresholdsImageFilter - Wrapper class around itk::OtsuMultipleThresholdsImageFilter
// .SECTION Description
// vtkITKOtsuMultipleThresholdsImageFilter


#ifndef __vtkITKOtsuMultipleThresholdsImageFilter_h
#define __vtkITKOtsuMultipleThresholdsImageFilter_h


#include "vtkITKImageToImageFilterUSUS.h"
#include "itkOtsuMultipleThresholdsImageFilter.h"
#include "stream.h"
#include "vtkUnsignedShortArray.h"
#include "vtkObjectFactory.h"

class VTK_EXPORT vtkITKOtsuMultipleThresholdsImageFilter : public vtkITKImageToImageFilterUSUS
{
 public:
  static vtkITKOtsuMultipleThresholdsImageFilter *New();
  vtkTypeRevisionMacro(vtkITKOtsuMultipleThresholdsImageFilter, vtkITKImageToImageFilterUSUS);
  
  void SetNumberOfHistogramBins( unsigned long value) 
  {
    DelegateITKInputMacro ( SetNumberOfHistogramBins, value );
  };
  unsigned long GetNumberOfHistogramBins ()
  { DelegateITKOutputMacro ( GetNumberOfHistogramBins ); };
  
  void SetNumberOfThresholds (unsigned long value)
  {
    DelegateITKInputMacro ( SetNumberOfThresholds, value );
  };
  unsigned long GetNumberOfThresholds ()
  { DelegateITKOutputMacro ( GetNumberOfThresholds ); };  
  
  void SetLabelOffset (unsigned short value)
  { 
    OutputImagePixelType d = static_cast<OutputImagePixelType> ( value );
    DelegateITKInputMacro (SetLabelOffset,d);
  }
  
  OutputImagePixelType GetLabelOffset ()
  { DelegateITKOutputMacro ( GetLabelOffset ); };
  
  vtkUnsignedShortArray *GetThresholds () {
    std::vector<unsigned short> th;
    th = this->GetImageFilterPointer()->GetThresholds();
    this->Thresholds->SetNumberOfComponents(1);
    this->Thresholds->SetNumberOfTuples(th.size());
    
    for(int i=0; i<th.size();i++)
      this->Thresholds->SetComponent(i,0,th[i]);  
   
    return this->Thresholds;
   }
   

protected:
  //BTX
  typedef itk::OtsuMultipleThresholdsImageFilter<Superclass::InputImageType, Superclass::OutputImageType> ImageFilterType;
  vtkITKOtsuMultipleThresholdsImageFilter() : Superclass (ImageFilterType::New())
  {
    this->Thresholds = vtkUnsignedShortArray::New();
  };
  ~vtkITKOtsuMultipleThresholdsImageFilter() {};
  ImageFilterType* GetImageFilterPointer() { return dynamic_cast<ImageFilterType*> ( m_Filter.GetPointer() ); }
  //ETX
  
private:
  vtkITKOtsuMultipleThresholdsImageFilter(const vtkITKOtsuMultipleThresholdsImageFilter&);  // Not implemented.
  void operator=(const vtkITKOtsuMultipleThresholdsImageFilter&);  // Not implemented.
  
  vtkUnsignedShortArray *Thresholds;
  
};

vtkCxxRevisionMacro(vtkITKOtsuMultipleThresholdsImageFilter, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkITKOtsuMultipleThresholdsImageFilter);

#endif





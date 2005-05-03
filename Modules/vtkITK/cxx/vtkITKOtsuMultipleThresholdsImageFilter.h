/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkITKOtsuMultipleThresholdsImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2005/05/03 21:50:25 $
  Version:   $Revision: 1.2 $
*/
// .NAME vtkITKOtsuMultipleThresholdsImageFilter - Wrapper class around itk::OtsuMultipleThresholdsImageFilter
// .SECTION Description
// vtkITKOtsuMultipleThresholdsImageFilter


#ifndef __vtkITKOtsuMultipleThresholdsImageFilter_h
#define __vtkITKOtsuMultipleThresholdsImageFilter_h


#include "vtkITKImageToImageFilterSS.h"
#include "itkOtsuMultipleThresholdsImageFilter.h"
#include "vtkShortArray.h"
#include "vtkObjectFactory.h"

class VTK_EXPORT vtkITKOtsuMultipleThresholdsImageFilter : public vtkITKImageToImageFilterSS
{
 public:
  static vtkITKOtsuMultipleThresholdsImageFilter *New();
  vtkTypeRevisionMacro(vtkITKOtsuMultipleThresholdsImageFilter, vtkITKImageToImageFilterSS);
  
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
  
  void SetLabelOffset (short value)
  { 
    OutputImagePixelType d = static_cast<OutputImagePixelType> ( value );
    DelegateITKInputMacro (SetLabelOffset,d);
  }
  
  OutputImagePixelType GetLabelOffset ()
  { DelegateITKOutputMacro ( GetLabelOffset ); };
  
  vtkShortArray *GetThresholds () {
    std::vector<InputImagePixelType> th;
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
    this->Thresholds = vtkShortArray::New();
  };
  ~vtkITKOtsuMultipleThresholdsImageFilter() {};
  ImageFilterType* GetImageFilterPointer() { return dynamic_cast<ImageFilterType*> ( m_Filter.GetPointer() ); }
  //ETX
  
private:
  vtkITKOtsuMultipleThresholdsImageFilter(const vtkITKOtsuMultipleThresholdsImageFilter&);  // Not implemented.
  void operator=(const vtkITKOtsuMultipleThresholdsImageFilter&);  // Not implemented.
  
  vtkShortArray *Thresholds;
  
};

vtkCxxRevisionMacro(vtkITKOtsuMultipleThresholdsImageFilter, "$Revision: 1.2 $");
vtkStandardNewMacro(vtkITKOtsuMultipleThresholdsImageFilter);

#endif





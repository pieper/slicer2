/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkITKGradientMagnitudeImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2003/01/16 19:01:29 $
  Version:   $Revision: 1.1 $
*/
// .NAME vtkITKGradientMagnitudeImageFilter - Wrapper class around itk::GradientMagnitudeImageFilterImageFilter
// .SECTION Description
// vtkITKGradientMagnitudeImageFilter


#ifndef __vtkITKGradientMagnitudeImageFilter_h
#define __vtkITKGradientMagnitudeImageFilter_h


#include "vtkITKImageToImageFilterFF.h"
#include "itkGradientMagnitudeImageFilter.h"
#include "vtkObjectFactory.h"

class VTK_EXPORT vtkITKGradientMagnitudeImageFilter : public vtkITKImageToImageFilterFF
{
 public:
  static vtkITKGradientMagnitudeImageFilter *New();
  vtkTypeRevisionMacro(vtkITKGradientMagnitudeImageFilter, vtkITKImageToImageFilterFF);

protected:
  //BTX
  typedef itk::GradientMagnitudeImageFilter<Superclass::InputImageType,Superclass::InputImageType> ImageFilterType;
  vtkITKGradientMagnitudeImageFilter() : Superclass ( ImageFilterType::New() ){};
  ~vtkITKGradientMagnitudeImageFilter() {};
  ImageFilterType* GetImageFilterPointer() { return dynamic_cast<ImageFilterType*> ( m_Filter.GetPointer() ); }

  //ETX
  
private:
  vtkITKGradientMagnitudeImageFilter(const vtkITKGradientMagnitudeImageFilter&);  // Not implemented.
  void operator=(const vtkITKGradientMagnitudeImageFilter&);  // Not implemented.
};

vtkCxxRevisionMacro(vtkITKGradientMagnitudeImageFilter, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkITKGradientMagnitudeImageFilter);

#endif





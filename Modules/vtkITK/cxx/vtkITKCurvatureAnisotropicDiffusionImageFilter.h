/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkITKCurvatureAnisotropicDiffusionImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2003/04/14 19:44:06 $
  Version:   $Revision: 1.2 $
*/
// .NAME vtkITKCurvatureAnisotropicDiffusionImageFilter - Wrapper class around itk::CurvatureAnisotropicDiffusionImageFilterImageFilter
// .SECTION Description
// vtkITKCurvatureAnisotropicDiffusionImageFilter


#ifndef __vtkITKCurvatureAnisotropicDiffusionImageFilter_h
#define __vtkITKCurvatureAnisotropicDiffusionImageFilter_h


#include "vtkITKImageToImageFilterFF.h"
#include "itkCurvatureAnisotropicDiffusionImageFilter.h"
#include "vtkObjectFactory.h"

class VTK_EXPORT vtkITKCurvatureAnisotropicDiffusionImageFilter : public vtkITKImageToImageFilterFF
{
 public:
  static vtkITKCurvatureAnisotropicDiffusionImageFilter *New();
  vtkTypeRevisionMacro(vtkITKCurvatureAnisotropicDiffusionImageFilter, vtkITKImageToImageFilterFF);

  double GetTimeStep ()
  {
    DelegateITKOutputMacro(GetTimeStep) ;
  };

  double GetConductanceParameter ()
  {
    DelegateITKOutputMacro(GetConductanceParameter) ;
  };

  unsigned int GetNumberOfIterations ()
  {
    DelegateITKOutputMacro(GetNumberOfIterations) ;
  };

  void SetNumberOfIterations( unsigned int value )
  {
    DelegateITKInputMacro ( SetNumberOfIterations, value );
  };

  void SetTimeStep ( double value )
  {
    DelegateITKInputMacro ( SetTimeStep, value );
  };

  void SetConductanceParameter ( double value )
  {
    DelegateITKInputMacro ( SetConductanceParameter, value );
  };

protected:
  //BTX
  typedef itk::CurvatureAnisotropicDiffusionImageFilter<Superclass::InputImageType,Superclass::InputImageType> ImageFilterType;
  vtkITKCurvatureAnisotropicDiffusionImageFilter() : Superclass ( ImageFilterType::New() ){};
  ~vtkITKCurvatureAnisotropicDiffusionImageFilter() {};
  ImageFilterType* GetImageFilterPointer() { return dynamic_cast<ImageFilterType*> ( m_Filter.GetPointer() ); }

  //ETX
  
private:
  vtkITKCurvatureAnisotropicDiffusionImageFilter(const vtkITKCurvatureAnisotropicDiffusionImageFilter&);  // Not implemented.
  void operator=(const vtkITKCurvatureAnisotropicDiffusionImageFilter&);  // Not implemented.
};

vtkCxxRevisionMacro(vtkITKCurvatureAnisotropicDiffusionImageFilter, "$Revision: 1.2 $");
vtkStandardNewMacro(vtkITKCurvatureAnisotropicDiffusionImageFilter);

#endif





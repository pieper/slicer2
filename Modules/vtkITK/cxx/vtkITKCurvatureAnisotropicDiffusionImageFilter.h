/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkITKCurvatureAnisotropicDiffusionImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2003/05/18 17:58:18 $
  Version:   $Revision: 1.3 $
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
#if (ITK_VERSION_MAJOR == 1 && ITK_VERSION_MINOR == 2 && ITK_VERSION_PATCH == 0)
    DelegateITKOutputMacro ( GetIterations );
#else
    DelegateITKOutputMacro ( GetNumberOfIterations );
#endif
  };

  void SetNumberOfIterations( unsigned int value )
  {
#if (ITK_VERSION_MAJOR == 1 && ITK_VERSION_MINOR == 2 && ITK_VERSION_PATCH == 0)
    DelegateITKInputMacro ( SetIterations, value );
#else
    DelegateITKInputMacro ( SetNumberOfIterations, value );
#endif
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

vtkCxxRevisionMacro(vtkITKCurvatureAnisotropicDiffusionImageFilter, "$Revision: 1.3 $");
vtkStandardNewMacro(vtkITKCurvatureAnisotropicDiffusionImageFilter);

#endif





/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkITKBSplineImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2004/07/27 16:39:07 $
  Version:   $Revision: 1.1 $
*/
// .NAME vtkITKBSplineImageFilter - Wrapper class around itk::BSplineImageFilterImageFilter
// .SECTION Description
// vtkITKBSplineImageFilter


#ifndef __vtkITKBSplineImageFilter_h
#define __vtkITKBSplineImageFilter_h


#include "vtkITKImageToImageFilterFF.h"
#include "itkBSplineDecompositionImageFilter.h"

class VTK_EXPORT vtkITKBSplineImageFilter : public vtkITKImageToImageFilterFF
{
 public:
  static vtkITKBSplineImageFilter *New();
  vtkTypeRevisionMacro(vtkITKBSplineImageFilter, vtkITKImageToImageFilterFF);
  void SetSplineOrder ( unsigned int order ) { DelegateSetMacro ( SplineOrder, order ); };
  int GetSplineOrder () { DelegateGetMacro ( SplineOrder ); };
  
protected:
  //BTX
  typedef Superclass::InputImageType InputImageType;
  typedef Superclass::OutputImageType OutputImageType;

  typedef itk::BSplineDecompositionImageFilter<InputImageType,OutputImageType> ImageFilterType;

  vtkITKBSplineImageFilter() : Superclass ( ImageFilterType::New() ){}
  ~vtkITKBSplineImageFilter() {};

  //ETX
  
private:
  vtkITKBSplineImageFilter(const vtkITKBSplineImageFilter&);  // Not implemented.
  void operator=(const vtkITKBSplineImageFilter&);  // Not implemented.
};

#endif





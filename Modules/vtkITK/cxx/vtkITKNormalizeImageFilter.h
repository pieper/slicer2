/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkITKNormalizeImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2003/04/14 19:44:08 $
  Version:   $Revision: 1.2 $
*/
// .NAME vtkITKNormalizeImageFilter - Wrapper class around itk::NormalizeImageFilterImageFilter
// .SECTION Description
// vtkITKNormalizeImageFilter


#ifndef __vtkITKNormalizeImageFilter_h
#define __vtkITKNormalizeImageFilter_h


#include "vtkITKImageToImageFilterFF.h"
#include "itkNormalizeImageFilter.h"
#include "vtkObjectFactory.h"

class VTK_EXPORT vtkITKNormalizeImageFilter : public vtkITKImageToImageFilterFF
{
 public:
  static vtkITKNormalizeImageFilter *New();
  vtkTypeRevisionMacro(vtkITKNormalizeImageFilter, vtkITKImageToImageFilterFF);

protected:
  //BTX
  typedef itk::NormalizeImageFilter<Superclass::InputImageType,Superclass::InputImageType> ImageFilterType;
  vtkITKNormalizeImageFilter() : Superclass ( ImageFilterType::New() ){};
  ~vtkITKNormalizeImageFilter() {};

  //ETX
  
private:
  vtkITKNormalizeImageFilter(const vtkITKNormalizeImageFilter&);  // Not implemented.
  void operator=(const vtkITKNormalizeImageFilter&);  // Not implemented.
};

vtkCxxRevisionMacro(vtkITKNormalizeImageFilter, "$Revision: 1.2 $");
vtkStandardNewMacro(vtkITKNormalizeImageFilter);

#endif





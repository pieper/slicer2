/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkITKTobogganImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2003/01/16 19:01:35 $
  Version:   $Revision: 1.1 $
*/
// .NAME vtkITKTobogganImageFilter - Wrapper class around itk::TobogganImageFilterImageFilter
// .SECTION Description
// vtkITKTobogganImageFilter


#ifndef __vtkITKTobogganImageFilter_h
#define __vtkITKTobogganImageFilter_h


#include "vtkITKImageToImageFilterFUL.h"
#include "itkTobogganImageFilter.h"
#include "vtkObjectFactory.h"

class VTK_EXPORT vtkITKTobogganImageFilter : public vtkITKImageToImageFilterFUL
{
 public:
  static vtkITKTobogganImageFilter *New();
  vtkTypeRevisionMacro(vtkITKTobogganImageFilter, vtkITKImageToImageFilterFUL);

protected:
  //BTX
  typedef itk::TobogganImageFilter<Superclass::InputImageType> ImageFilterType;
  vtkITKTobogganImageFilter() : Superclass ( ImageFilterType::New() ){};
  ~vtkITKTobogganImageFilter() {};

  //ETX
  
private:
  vtkITKTobogganImageFilter(const vtkITKTobogganImageFilter&);  // Not implemented.
  void operator=(const vtkITKTobogganImageFilter&);  // Not implemented.
};

vtkCxxRevisionMacro(vtkITKTobogganImageFilter, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkITKTobogganImageFilter);

#endif





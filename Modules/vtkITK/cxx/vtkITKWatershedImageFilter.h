/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkITKWatershedImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2003/01/16 19:01:35 $
  Version:   $Revision: 1.1 $
*/
// .NAME vtkITKWatershedImageFilter - Wrapper class around itk::WatershedImageFilterImageFilter
// .SECTION Description
// vtkITKWatershedImageFilter


#ifndef __vtkITKWatershedImageFilter_h
#define __vtkITKWatershedImageFilter_h


#include "vtkITKImageToImageFilterFUL.h"
#include "itkWatershedImageFilter.h"
#include "vtkObjectFactory.h"

class VTK_EXPORT vtkITKWatershedImageFilter : public vtkITKImageToImageFilterFUL
{
 public:
  static vtkITKWatershedImageFilter *New();
  vtkTypeRevisionMacro(vtkITKWatershedImageFilter, vtkITKImageToImageFilterFUL);

  void SetThreshold ( double d ) { DelegateSetMacro ( Threshold, d ); };
  double GetThreshold () { DelegateGetMacro ( Threshold ); };
  void SetLevel ( double d ) { DelegateSetMacro ( Level, d ); };
  double GetLevel () { DelegateGetMacro ( Level ); };

protected:
  //BTX
  typedef itk::WatershedImageFilter<Superclass::InputImageType> ImageFilterType;
  vtkITKWatershedImageFilter() : Superclass ( ImageFilterType::New() ){};
  ~vtkITKWatershedImageFilter() {};

  //ETX
  
private:
  vtkITKWatershedImageFilter(const vtkITKWatershedImageFilter&);  // Not implemented.
  void operator=(const vtkITKWatershedImageFilter&);  // Not implemented.
};

vtkCxxRevisionMacro(vtkITKWatershedImageFilter, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkITKWatershedImageFilter);

#endif





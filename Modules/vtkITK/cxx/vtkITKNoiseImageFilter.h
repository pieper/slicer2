/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkITKNoiseImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2005/04/26 00:43:01 $
  Version:   $Revision: 1.1 $
*/
// .NAME vtkITKNoiseImageFilter - Wrapper class around itk::NoiseImageFilterImageFilter
// .SECTION Description
// vtkITKNoiseImageFilter


#ifndef __vtkITKNoiseImageFilter_h
#define __vtkITKNoiseImageFilter_h


#include "vtkITKImageToImageFilterFF.h"
#include "itkNoiseImageFilter.h"
#include "vtkObjectFactory.h"



class VTK_EXPORT vtkITKNoiseImageFilter : public vtkITKImageToImageFilterFF
{
 public:
  static vtkITKNoiseImageFilter *New();
  vtkTypeRevisionMacro(vtkITKNoiseImageFilter, vtkITKImageToImageFilterFF);
  
  void SetRadius (int x, int y, int z)
  {
    ImageFilterType::InputSizeType radius;
    radius[0] = x;
    radius[1] = y;
    radius[2] = z;
    DelegateITKInputMacro(SetRadius, radius);
  };
  int * GetRadius ()
  {
    int val[3];
    ImageFilterType::InputSizeType radius;
    radius = this->GetImageFilterPointer()->GetRadius();
    val[0]=radius[0];
    val[1]=radius[1];
    val[2]=radius[2];
    return val;
  };
  
protected:
  //BTX
  typedef itk::NoiseImageFilter<Superclass::InputImageType,Superclass::InputImageType> ImageFilterType;
  vtkITKNoiseImageFilter() : Superclass ( ImageFilterType::New() ){};
  ~vtkITKNoiseImageFilter() {};
  ImageFilterType* GetImageFilterPointer() { return dynamic_cast<ImageFilterType*> ( m_Filter.GetPointer() ); }

  //ETX
  
private:
  vtkITKNoiseImageFilter(const vtkITKNoiseImageFilter&);  // Not implemented.
  void operator=(const vtkITKNoiseImageFilter&);  // Not implemented.
  
};

vtkCxxRevisionMacro(vtkITKNoiseImageFilter, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkITKNoiseImageFilter);

#endif





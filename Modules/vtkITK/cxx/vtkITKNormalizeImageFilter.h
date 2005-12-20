/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkITKNormalizeImageFilter.h,v $
  Date:      $Date: 2005/12/20 22:55:50 $
  Version:   $Revision: 1.3.8.1 $

=========================================================================auto=*/
/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkITKNormalizeImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2005/12/20 22:55:50 $
  Version:   $Revision: 1.3.8.1 $
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

vtkCxxRevisionMacro(vtkITKNormalizeImageFilter, "$Revision: 1.3.8.1 $");
vtkStandardNewMacro(vtkITKNormalizeImageFilter);

#endif





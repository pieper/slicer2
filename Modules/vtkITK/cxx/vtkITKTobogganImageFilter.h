/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkITKTobogganImageFilter.h,v $
  Date:      $Date: 2005/12/20 22:55:52 $
  Version:   $Revision: 1.3.8.1 $

=========================================================================auto=*/
/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkITKTobogganImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2005/12/20 22:55:52 $
  Version:   $Revision: 1.3.8.1 $
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

vtkCxxRevisionMacro(vtkITKTobogganImageFilter, "$Revision: 1.3.8.1 $");
vtkStandardNewMacro(vtkITKTobogganImageFilter);

#endif





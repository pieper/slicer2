/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkITKDanielssonDistanceMapImageFilter.h,v $
  Date:      $Date: 2006/01/06 17:57:45 $
  Version:   $Revision: 1.4 $

=========================================================================auto=*/
/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkITKDanielssonDistanceMapImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2006/01/06 17:57:45 $
  Version:   $Revision: 1.4 $
*/
// .NAME vtkITKDanielssonDistanceMapImageFilter - Wrapper class around itk::DanielssonDistanceMapImageFilter
// .SECTION Description
// vtkITKDanielssonDistanceMapImageFilter


#ifndef __vtkITKDanielssonDistanceMapImageFilter_h
#define __vtkITKDanielssonDistanceMapImageFilter_h


#include "vtkITKImageToImageFilterFF.h"
#include "itkDanielssonDistanceMapImageFilter.h"
#include "vtkObjectFactory.h"

class VTK_EXPORT vtkITKDanielssonDistanceMapImageFilter : public vtkITKImageToImageFilterFF
{
 public:
  static vtkITKDanielssonDistanceMapImageFilter *New();
  vtkTypeRevisionMacro(vtkITKDanielssonDistanceMapImageFilter, vtkITKImageToImageFilterFF);

  void SetSquaredDistance ( int value )
  {
    DelegateITKInputMacro ( SetSquaredDistance, (bool) value );
  }

  void SquaredDistanceOn()
  {
    this->SetSquaredDistance (true);
  }
  void SquaredDistanceOff()
  {
    this->SetSquaredDistance (false);
  }
  int GetSquaredDistance()
  { DelegateITKOutputMacro ( GetSquaredDistance ); }


  void SetInputIsBinary ( int value )
  {
    DelegateITKInputMacro ( SetInputIsBinary, (bool) value );
  }
  void InputIsBinaryOn()
  {
    this->SetInputIsBinary (true);
  }
  void InputIsBinaryOff()
  {
    this->SetInputIsBinary (false);
  }
  int GetInputIsBinary()
  { DelegateITKOutputMacro ( GetInputIsBinary ); };

protected:
  //BTX
  typedef itk::DanielssonDistanceMapImageFilter<Superclass::InputImageType, Superclass::OutputImageType> ImageFilterType;
  vtkITKDanielssonDistanceMapImageFilter() : Superclass ( ImageFilterType::New() ){};
  ~vtkITKDanielssonDistanceMapImageFilter() {};
  ImageFilterType* GetImageFilterPointer() { return dynamic_cast<ImageFilterType*> ( m_Filter.GetPointer() ); }
  //ETX
  
private:
  vtkITKDanielssonDistanceMapImageFilter(const vtkITKDanielssonDistanceMapImageFilter&);  // Not implemented.
  void operator=(const vtkITKDanielssonDistanceMapImageFilter&);  // Not implemented.
};

vtkCxxRevisionMacro(vtkITKDanielssonDistanceMapImageFilter, "$Revision: 1.4 $");
vtkStandardNewMacro(vtkITKDanielssonDistanceMapImageFilter);

#endif





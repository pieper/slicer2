/*=auto=========================================================================

(c) Copyright 2005 Massachusetts Institute of Technology (MIT) All Rights Reserved.

This software ("3D Slicer") is provided by The Brigham and Women's 
Hospital, Inc. on behalf of the copyright holders and contributors.
Permission is hereby granted, without payment, to copy, modify, display 
and distribute this software and its documentation, if any, for  
research purposes only, provided that (1) the above copyright notice and 
the following four paragraphs appear on all copies of this software, and 
(2) that source code to any modifications to this software be made 
publicly available under terms no more restrictive than those in this 
License Agreement. Use of this software constitutes acceptance of these 
terms and conditions.

3D Slicer Software has not been reviewed or approved by the Food and 
Drug Administration, and is for non-clinical, IRB-approved Research Use 
Only.  In no event shall data or images generated through the use of 3D 
Slicer Software be used in the provision of patient care.

IN NO EVENT SHALL THE COPYRIGHT HOLDERS AND CONTRIBUTORS BE LIABLE TO 
ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL 
DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, 
EVEN IF THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE BEEN ADVISED OF THE 
POSSIBILITY OF SUCH DAMAGE.

THE COPYRIGHT HOLDERS AND CONTRIBUTORS SPECIFICALLY DISCLAIM ANY EXPRESS 
OR IMPLIED WARRANTIES INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND 
NON-INFRINGEMENT.

THE SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS 
IS." THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE NO OBLIGATION TO 
PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.


=========================================================================auto=*/
/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkITKIsolateConnectedImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2005/04/19 18:47:07 $
  Version:   $Revision: 1.3 $
*/
// .NAME vtkITKIsolatedConnectedImageFilter - Wrapper class around itk::IsolatedConnectedImageFilter
// .SECTION Description
// vtkITKIsolatedConnectedImageFilter


#ifndef __vtkITKIsolatedConnectedImageFilter_h
#define __vtkITKIsolatedConnectedImageFilter_h


#include "vtkITKImageToImageFilterUSUS.h"
#include "itkIsolatedConnectedImageFilter.h"
#include "vtkObjectFactory.h"

class VTK_EXPORT vtkITKIsolatedConnectedImageFilter : public vtkITKImageToImageFilterUSUS
{
 public:
  static vtkITKIsolatedConnectedImageFilter *New();
  vtkTypeRevisionMacro(vtkITKIsolatedConnectedImageFilter, vtkITKImageToImageFilterUSUS);

  void SetReplaceValue ( double value )
  {
    InputImagePixelType d = static_cast<InputImagePixelType> ( value );
    DelegateITKInputMacro ( SetReplaceValue, d );
  };
  void SetLower ( double ind )
  {
    InputImagePixelType d = static_cast<InputImagePixelType> ( ind );
    DelegateITKInputMacro ( SetLower, d );
  };
  double GetLower()
  { DelegateITKOutputMacro ( GetLower ); };
  
  double GetIsolatedValue()
  { DelegateITKOutputMacro ( GetIsolatedValue ); };
  
  void SetSeed1 ( int x, int y, int z )
  {
    ImageFilterType::IndexType seed;
    seed[0] = x;
    seed[1] = y;
    seed[2] = z;
    this->GetImageFilterPointer()->SetSeed1 ( seed );
  }

  void SetSeed2 ( int x, int y, int z )
  {
    ImageFilterType::IndexType seed;
    seed[0] = x;
    seed[1] = y;
    seed[2] = z;
    this->GetImageFilterPointer()->SetSeed2 ( seed );
  }

protected:
  //BTX
  typedef itk::IsolatedConnectedImageFilter<Superclass::InputImageType, Superclass::OutputImageType> ImageFilterType;
  vtkITKIsolatedConnectedImageFilter() : Superclass ( ImageFilterType::New() ){};
  ~vtkITKIsolatedConnectedImageFilter() {};
  ImageFilterType* GetImageFilterPointer() { return dynamic_cast<ImageFilterType*> ( m_Filter.GetPointer() ); }
  //ETX
  
private:
  vtkITKIsolatedConnectedImageFilter(const vtkITKIsolatedConnectedImageFilter&);  // Not implemented.
  void operator=(const vtkITKIsolatedConnectedImageFilter&);  // Not implemented.
};

vtkCxxRevisionMacro(vtkITKIsolatedConnectedImageFilter, "$Revision: 1.3 $");
vtkStandardNewMacro(vtkITKIsolatedConnectedImageFilter);

#endif





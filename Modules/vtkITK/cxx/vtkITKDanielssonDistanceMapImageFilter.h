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
  Module:    $RCSfile: vtkITKDanielssonDistanceMapImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2006/03/03 21:50:45 $
  Version:   $Revision: 1.5 $
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

  void SetSquaredDistance ( int value ) {
    DelegateITKInputMacro ( SetSquaredDistance, (bool) value );
  }
  int GetSquaredDistance() { 
    DelegateITKOutputMacro ( GetSquaredDistance ); 
  }
  void SquaredDistanceOn() {
    this->SetSquaredDistance (true);
  }
  void SquaredDistanceOff() {
    this->SetSquaredDistance (false);
  }
  int GetInputIsBinary() { 
    DelegateITKOutputMacro ( GetInputIsBinary ); 
  }
  void SetInputIsBinary ( int value ) {
    DelegateITKInputMacro ( SetInputIsBinary, (bool) value );
  }
  void InputIsBinaryOn() {
    this->SetInputIsBinary (true);
  }
  void InputIsBinaryOff() {
    this->SetInputIsBinary (false);
  }
  void SetUseImageSpacing ( int value ) {
    DelegateITKInputMacro ( SetUseImageSpacing, (bool) value );
  }
  int GetUseImageSpacing () {
    DelegateITKOutputMacro ( GetUseImageSpacing );
  } 
  void UseImageSpacingOn () {
    this->SetUseImageSpacing (true);
  }
  void UseImageSpacingOff () {
    this->SetUseImageSpacing (false);
  }
  vtkImageData *GetVoronoiMap() {
    this->vtkVoronoiMapImporter->Update(); 
    return this->vtkVoronoiMapImporter->GetOutput();
  } 
  vtkImageData *GetDistanceMap() {
    this->vtkDistanceMapImporter->Update();
    return this->vtkDistanceMapImporter->GetOutput();
  } 
protected:
  //BTX
  typedef itk::DanielssonDistanceMapImageFilter<Superclass::InputImageType, Superclass::OutputImageType> ImageFilterType;
  typedef itk::VTKImageExport<OutputImageType> VoronoiMapExportType;
  typedef itk::VTKImageExport<OutputImageType> DistanceMapExportType;

  VoronoiMapExportType::Pointer itkVoronoiMapExporter;
  DistanceMapExportType::Pointer itkDistanceMapExporter;  
  vtkImageImport *vtkVoronoiMapImporter;  
  vtkImageImport *vtkDistanceMapImporter; 
 
  vtkITKDanielssonDistanceMapImageFilter() : Superclass ( ImageFilterType::New() ) {
     //VoronoiMap 
    this->itkVoronoiMapExporter = VoronoiMapExportType::New();
    this->vtkVoronoiMapImporter = vtkImageImport::New();
    ConnectPipelines(this->itkVoronoiMapExporter,this->vtkVoronoiMapImporter);
    this->itkVoronoiMapExporter->SetInput((dynamic_cast<ImageFilterType*>(m_Filter.GetPointer()))->GetVoronoiMap());
 
    //DistanceMap
    this->itkDistanceMapExporter = DistanceMapExportType::New();
    this->vtkDistanceMapImporter = vtkImageImport::New();
    ConnectPipelines(this->itkDistanceMapExporter,this->vtkDistanceMapImporter);
    this->itkDistanceMapExporter->SetInput((dynamic_cast<ImageFilterType*>(m_Filter.GetPointer()))->GetDistanceMap());
  }
  ~vtkITKDanielssonDistanceMapImageFilter() {};
  ImageFilterType* GetImageFilterPointer() { return dynamic_cast<ImageFilterType*> ( m_Filter.GetPointer() ); }

 //ETX

 
private:
  vtkITKDanielssonDistanceMapImageFilter(const vtkITKDanielssonDistanceMapImageFilter&);  // Not implemented.
  void operator=(const vtkITKDanielssonDistanceMapImageFilter&);  // Not implemented.
};

vtkCxxRevisionMacro(vtkITKDanielssonDistanceMapImageFilter, "$Revision: 1.5 $");
vtkStandardNewMacro(vtkITKDanielssonDistanceMapImageFilter);

#endif





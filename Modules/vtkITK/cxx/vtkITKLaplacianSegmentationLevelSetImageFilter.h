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
  Module:    $RCSfile: vtkITKLaplacianSegmentationLevelSetImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2005/04/19 18:47:07 $
  Version:   $Revision: 1.3 $
*/
// .NAME vtkITKLaplacianSegmentationLevelSetImageFilter - Wrapper class around itk::LaplacianSegmentationLevelSetImageFilterImageFilter
// .SECTION Description
// vtkITKLaplacianSegmentationLevelSetImageFilter


#ifndef __vtkITKLaplacianSegmentationLevelSetImageFilter_h
#define __vtkITKLaplacianSegmentationLevelSetImageFilter_h


#include "vtkITKImageToImageFilterFF.h"
#include "itkLaplacianSegmentationLevelSetImageFilter.h"
#include "vtkObjectFactory.h"

class VTK_EXPORT vtkITKLaplacianSegmentationLevelSetImageFilter : public vtkITKImageToImageFilterFF
{
 public:
  static vtkITKLaplacianSegmentationLevelSetImageFilter *New();
  vtkTypeRevisionMacro(vtkITKLaplacianSegmentationLevelSetImageFilter, vtkITKImageToImageFilterFF);

  float GetIsoSurfaceValue ()
  {
    DelegateITKOutputMacro(GetIsoSurfaceValue) ;
  };

  void SetIsoSurfaceValue ( float value )
  {
     DelegateITKInputMacro ( SetIsoSurfaceValue, value );
  };
  
  void SetMaximumIterations ( int value )
  {
    DelegateITKInputMacro ( SetMaximumIterations, value );
  };

  void SetMaximumRMSError ( float value )
  {
    DelegateITKInputMacro ( SetMaximumRMSError, value );
  };

  void SetUseNegativeFeatures (int value )
  {
    DelegateITKInputMacro( SetUseNegativeFeatures, value);
  };
  
  void SetFeatureImage ( vtkImageData *value)
  {
    this->vtkFeatureExporter->SetInput(value);
  };

  void SetFeatureScaling ( float value )
  {
    DelegateITKInputMacro ( SetFeatureScaling, value );
  };

  void SetCurvatureScaling ( float value )
  {
    DelegateITKInputMacro ( SetCurvatureScaling, value );
  };

  float GetCurvatureScaling ()
  {
    DelegateITKOutputMacro(GetCurvatureScaling) ;
  };
  
  vtkImageData *GetSpeedImage()
  {
    this->vtkSpeedImporter->Update();
    return this->vtkSpeedImporter->GetOutput();
  };

  float GetRMSChange ()
  {
    DelegateITKOutputMacro(GetRMSChange);
  };

  int GetElapsedIterations()
  {
    DelegateITKOutputMacro(GetElapsedIterations);
  };

  
  // Description: Override vtkSource's Update so that we can access this class's GetOutput(). vtkSource's GetOutput is not virtual.
  void Update()
  {
    if (this->vtkFeatureExporter->GetInput())
      {
        this->itkFeatureImporter->Update();
        
        if (this->GetOutput(0))
          {
            this->GetOutput(0)->Update();
            if ( this->GetOutput(0)->GetSource() )
              {
                //          this->SetErrorCode( this->GetOutput(0)->GetSource()->GetErrorCode() );
              }
          }
      }
  }
    
protected:
  //BTX
  typedef itk::LaplacianSegmentationLevelSetImageFilter<Superclass::InputImageType,Superclass::InputImageType> ImageFilterType;
  typedef itk::VTKImageImport<InputImageType> FeatureImageImportType;
  typedef itk::VTKImageExport<InputImageType> SpeedImageExportType;
  
  vtkITKLaplacianSegmentationLevelSetImageFilter() : Superclass ( ImageFilterType::New() )
  {
    this->vtkFeatureExporter = vtkImageExport::New();
    this->itkFeatureImporter = FeatureImageImportType::New();
    this->itkSpeedExporter = SpeedImageExportType::New();
    this->vtkSpeedImporter = vtkImageImport::New();
    ConnectPipelines(this->itkSpeedExporter, this->vtkSpeedImporter);
    ConnectPipelines(this->vtkFeatureExporter, this->itkFeatureImporter);
    (dynamic_cast<ImageFilterType*>(m_Filter.GetPointer()))->SetFeatureImage(this->itkFeatureImporter->GetOutput());
    this->itkSpeedExporter->SetInput((dynamic_cast<ImageFilterType*>(m_Filter.GetPointer()))->GetSpeedImage());
  };
  ~vtkITKLaplacianSegmentationLevelSetImageFilter() {};
  ImageFilterType* GetImageFilterPointer() { return dynamic_cast<ImageFilterType*> ( m_Filter.GetPointer() ); }
  
  FeatureImageImportType::Pointer itkFeatureImporter;
  SpeedImageExportType::Pointer itkSpeedExporter;
  //ETX

  vtkImageExport *vtkFeatureExporter;
  vtkImageImport *vtkSpeedImporter;
  
private:
  vtkITKLaplacianSegmentationLevelSetImageFilter(const vtkITKLaplacianSegmentationLevelSetImageFilter&);  // Not implemented.
  void operator=(const vtkITKLaplacianSegmentationLevelSetImageFilter&);  //
                                                                          // Not implemented
  
};

vtkCxxRevisionMacro(vtkITKLaplacianSegmentationLevelSetImageFilter, "$Revision: 1.3 $");
vtkStandardNewMacro(vtkITKLaplacianSegmentationLevelSetImageFilter);

#endif


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
  Module:    $RCSfile: vtkITKMRIBiasFieldCorrectionImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2005/10/22 16:48:24 $
  Version:   $Revision: 1.1 $
*/
// .NAME vtkITKMRIBiasFieldCorrectionImageFilter - Wrapper class around itk::MRIBiasFieldCorrectionImageFilterImageFilter
// .SECTION Description
// vtkITKMRIBiasFieldCorrectionImageFilter


#ifndef __vtkITKMRIBiasFieldCorrectionImageFilter_h
#define __vtkITKMRIBiasFieldCorrectionImageFilter_h


#include "vtkITKImageToImageFilterFF.h"

#include "itkMRIBiasFieldCorrectionFilter.h"
#include "itkArray.h"

#include "vtkObjectFactory.h"
#include "vtkDoubleArray.h"
#include "vtkUnsignedIntArray.h"

class VTK_EXPORT vtkITKMRIBiasFieldCorrectionImageFilter : public vtkITKImageToImageFilterFF
{
 public:
  static vtkITKMRIBiasFieldCorrectionImageFilter *New();
  vtkTypeRevisionMacro(vtkITKMRIBiasFieldCorrectionImageFilter, vtkITKImageToImageFilterFF);

  void SetTissueClassStatistics( vtkDoubleArray *classMeans, vtkDoubleArray *classSigmas )
  {
    itk::Array<double> itkClassMeans( classMeans->GetNumberOfTuples() ) ;
    itk::Array<double> itkClassSigmas( classSigmas->GetNumberOfTuples() ) ;
    
    for(int i=0; i< classMeans->GetNumberOfTuples();i++) {
      itkClassMeans[i] = classMeans->GetValue(i);
      itkClassSigmas[i] = classSigmas->GetValue(i);
    }
    this->GetImageFilterPointer()->SetTissueClassStatistics (itkClassMeans, itkClassSigmas);
  }

  bool GetUsingSlabIdentification ()
  {
    DelegateITKOutputMacro ( GetUsingSlabIdentification );
  };
  void SetUsingSlabIdentification( bool value )
  {
    DelegateITKInputMacro ( SetUsingSlabIdentification, value );
  };

  bool GetUsingInterSliceIntensityCorrection ()
  {
    DelegateITKOutputMacro ( GetUsingInterSliceIntensityCorrection );
  };
  void SetUsingInterSliceIntensityCorrection( bool value )
  {
    DelegateITKInputMacro ( SetUsingInterSliceIntensityCorrection, value );
  };


  int GetVolumeCorrectionMaximumIteration ()
  {
    DelegateITKOutputMacro ( GetVolumeCorrectionMaximumIteration );
  };
  void SetVolumeCorrectionMaximumIteration( int value )
  {
    DelegateITKInputMacro ( SetVolumeCorrectionMaximumIteration, value );
  };

  int GetInterSliceCorrectionMaximumIteration ()
  {
    DelegateITKOutputMacro ( GetInterSliceCorrectionMaximumIteration );
  };
  void SetInterSliceCorrectionMaximumIteration( int value )
  {
    DelegateITKInputMacro ( SetInterSliceCorrectionMaximumIteration, value );
  };

  int GetBiasFieldDegree ()
  {
    DelegateITKOutputMacro ( GetBiasFieldDegree );
  };
  void SetBiasFieldDegree( int value )
  {
    DelegateITKInputMacro ( SetBiasFieldDegree, value );
  };

  int GetSlabNumberOfSamples ()
  {
    DelegateITKOutputMacro ( GetSlabNumberOfSamples );
  };
  void SetSlabNumberOfSamples( int value )
  {
    DelegateITKInputMacro ( SetSlabNumberOfSamples, value );
  };

  void SetSlicingDirection( int value )
  {
    DelegateITKInputMacro ( SetSlicingDirection, value );
  };


  double GetSlabBackgroundMinimumThreshold ()
  {
    DelegateITKOutputMacro ( GetSlabBackgroundMinimumThreshold );
  };
  void SetSlabBackgroundMinimumThreshold( double value )
  {
    InputImagePixelType d = static_cast<InputImagePixelType> ( value );
    DelegateITKInputMacro ( SetSlabBackgroundMinimumThreshold, d );
  };

  void SetOptimizerGrowthFactor ( double value )
  {
    DelegateITKInputMacro ( SetOptimizerGrowthFactor, value );
  };

  double GetOptimizerInitialRadius ()
  {
    DelegateITKOutputMacro(GetOptimizerInitialRadius) ;
  };
  void SetOptimizerInitialRadius ( double value )
  {
    DelegateITKInputMacro ( SetOptimizerInitialRadius, value );
  };

  double GetSlabTolerance ()
  {
    DelegateITKOutputMacro(GetSlabTolerance) ;
  };
  void SetSlabTolerance ( double value )
  {
    DelegateITKInputMacro ( SetSlabTolerance, value );
  };


protected:
  //BTX
  typedef itk::MRIBiasFieldCorrectionFilter<Superclass::InputImageType,Superclass::InputImageType,Superclass::InputImageType> ImageFilterType;
  vtkITKMRIBiasFieldCorrectionImageFilter() : Superclass ( ImageFilterType::New() ) {};
  ~vtkITKMRIBiasFieldCorrectionImageFilter() {};
  ImageFilterType* GetImageFilterPointer() { return dynamic_cast<ImageFilterType*> ( m_Filter.GetPointer() ); }

  //ETX
  
private:
  vtkITKMRIBiasFieldCorrectionImageFilter(const vtkITKMRIBiasFieldCorrectionImageFilter&);  // Not implemented.
  void operator=(const vtkITKMRIBiasFieldCorrectionImageFilter&);  // Not implemented.
};

vtkCxxRevisionMacro(vtkITKMRIBiasFieldCorrectionImageFilter, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkITKMRIBiasFieldCorrectionImageFilter);

#endif





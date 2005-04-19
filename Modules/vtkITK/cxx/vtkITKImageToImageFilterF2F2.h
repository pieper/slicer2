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
// .NAME vtkITKImageToImageFilter - Abstract base class for connecting ITK and VTK
// .SECTION Description
// vtkITKImageToImageFilter provides a 

#ifndef __vtkITKImageToImageFilterF2F2_h
#define __vtkITKImageToImageFilterF2F2_h


#include "vtkITKImageToImageFilter.h"
#include "vtkImageToImageFilter.h"
#include "vtkImageAppendComponents.h"
#include "itkImageToImageFilter.h"
#include "itkSplitImageFilter.h"
#include "itkJoinImageFilter.h"
#include "itkVTKImageExport.h"
#include "itkVTKImageImport.h"
#include "vtkITKUtility.h"

class VTK_EXPORT vtkITKImageToImageFilterF2F2 : public vtkITKImageToImageFilter
{
public:
  vtkTypeMacro(vtkITKImageToImageFilterF2F2,vtkITKImageToImageFilter);
  static vtkITKImageToImageFilterF2F2* New() { return 0; };
  void PrintSelf(ostream& os, vtkIndent indent)
  {
    Superclass::PrintSelf ( os, indent );
    os << m_Filter;
  };

  // Description:
  // Set the Input of the filter.
  virtual void SetInput1(vtkImageData *Input)
  {
    this->SetInput ( Input );
  };
  virtual void SetInput2(vtkImageData *Input)
  {
    this->vtkProcessObject::SetNthInput(1, Input);
    this->vtkExporter1->SetInput(Input);
  };
  
  virtual vtkImageData *GetOutput() { return this->append->GetOutput(); };

protected:
  //BTX
  
  // To/from ITK
  typedef itk::Vector<float,2> InputImagePixelType;
  typedef itk::Vector<float,2> OutputImagePixelType;
  typedef itk::Image<InputImagePixelType, 3> InputImageType;
  typedef itk::Image<OutputImagePixelType, 3> OutputImageType;

  typedef itk::Image<float, 3> JoinImageType;
  typedef itk::VTKImageImport<JoinImageType> ImageImportType;
  typedef itk::VTKImageExport<JoinImageType> ImageExportType;
  ImageImportType::Pointer itkImporter;
  ImageExportType::Pointer itkExporter;
  ImageImportType::Pointer itkImporter1;
  ImageExportType::Pointer itkExporter1;

  typedef itk::JoinImageFilter<JoinImageType, JoinImageType> JoinFilterType;

  typedef itk::SplitImageFilter<OutputImageType, JoinImageType> SplitFilterType;

  typedef itk::ImageToImageFilter<InputImageType,OutputImageType> GenericFilterType;
  GenericFilterType::Pointer m_Filter;
  
  vtkImageImport* vtkImporter1;
  vtkImageExport* vtkExporter1;
  vtkImageAppendComponents* append;
  SplitFilterType::Pointer split1, split;
  JoinFilterType::Pointer join;
  
  vtkITKImageToImageFilterF2F2 ( GenericFilterType* filter ) : vtkITKImageToImageFilter ()
  {
    this->vtkImporter1 = vtkImageImport::New();
    this->vtkExporter1 = vtkImageExport::New();
    // Need an import, export, and a ITK pipeline
    m_Filter = filter;
    this->itkImporter = ImageImportType::New();
    this->itkExporter = ImageExportType::New();
    ConnectPipelines(this->vtkExporter, this->itkImporter);
    ConnectPipelines(this->itkExporter, this->vtkImporter);

    this->itkImporter1 = ImageImportType::New();
    this->itkExporter1 = ImageExportType::New();
    ConnectPipelines(this->vtkExporter1, this->itkImporter1);
    ConnectPipelines(this->itkExporter1, this->vtkImporter1);
    this->LinkITKProgressToVTKProgress ( m_Filter );

    // Set up the filter pipeline
    // Join before going in
    join = JoinFilterType::New();
    join->SetInput1 ( this->itkImporter->GetOutput() );
    join->SetInput2 ( this->itkImporter->GetOutput() );
    
    m_Filter->SetInput ( join->GetOutput() );
    // m_Filter->DebugOn();
    
    split = SplitFilterType::New();
    // split->SetInput ( join->GetOutput() );
    split->SetInput ( m_Filter->GetOutput() );
    split->SetIndex ( 0 );
    split1 = SplitFilterType::New();
    // split1->SetInput ( join->GetOutput() );
    split1->SetInput ( m_Filter->GetOutput() );
    split1->SetIndex ( 1 );

    this->itkExporter->SetInput ( split->GetOutput() );
    this->itkExporter1->SetInput ( split1->GetOutput() );

    this->append = vtkImageAppendComponents::New();
    this->append->SetInput ( 0, this->vtkImporter->GetOutput() );
    this->append->SetInput ( 1, this->vtkImporter1->GetOutput() );
    this->vtkCast->SetOutputScalarTypeToFloat();

//     cout << m_Filter;
//     vtkImporter1->DebugOn();
//     vtkImporter->DebugOn();
//     vtkExporter->DebugOn();
//     vtkExporter1->DebugOn();
//     itkImporter->DebugOn();
//     itkImporter1->DebugOn();
//     itkExporter->DebugOn();
//     itkExporter1->DebugOn();
//     split->DebugOn();
//     join->DebugOn();
//     append->DebugOn();
    // m_Filter->Update();
  };

  ~vtkITKImageToImageFilterF2F2()
  {
    this->vtkExporter1->Delete();
    this->vtkImporter1->Delete();
    this->append->Delete();
  };
  //ETX
  
private:
  vtkITKImageToImageFilterF2F2(const vtkITKImageToImageFilterF2F2&);  // Not implemented.
  void operator=(const vtkITKImageToImageFilterF2F2&);  // Not implemented.
};

#endif





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
#ifndef __vtkITKUtility_h
#define __vtkITKUtility_h


#include "vtkObjectFactory.h"
#include "vtkSetGet.h"

/**
 * This function will connect the given itk::VTKImageExport filter to
 * the given vtkImageImport filter.
 */
template <typename ITK_Exporter, typename VTK_Importer>
void ConnectPipelines(ITK_Exporter exporter, VTK_Importer* importer)
{
  importer->SetUpdateInformationCallback(exporter->GetUpdateInformationCallback());
  importer->SetPipelineModifiedCallback(exporter->GetPipelineModifiedCallback());
  importer->SetWholeExtentCallback(exporter->GetWholeExtentCallback());
  importer->SetSpacingCallback(exporter->GetSpacingCallback());
  importer->SetOriginCallback(exporter->GetOriginCallback());
  importer->SetScalarTypeCallback(exporter->GetScalarTypeCallback());
  importer->SetNumberOfComponentsCallback(exporter->GetNumberOfComponentsCallback());
  importer->SetPropagateUpdateExtentCallback(exporter->GetPropagateUpdateExtentCallback());
  importer->SetUpdateDataCallback(exporter->GetUpdateDataCallback());
  importer->SetDataExtentCallback(exporter->GetDataExtentCallback());
  importer->SetBufferPointerCallback(exporter->GetBufferPointerCallback());
  importer->SetCallbackUserData(exporter->GetCallbackUserData());
}

/**
 * This function will connect the given vtkImageExport filter to
 * the given itk::VTKImageImport filter.
 */
template <typename VTK_Exporter, typename ITK_Importer>
void ConnectPipelines(VTK_Exporter* exporter, ITK_Importer importer)
{
  importer->SetUpdateInformationCallback(exporter->GetUpdateInformationCallback());
  importer->SetPipelineModifiedCallback(exporter->GetPipelineModifiedCallback());
  importer->SetWholeExtentCallback(exporter->GetWholeExtentCallback());
  importer->SetSpacingCallback(exporter->GetSpacingCallback());
  importer->SetOriginCallback(exporter->GetOriginCallback());
  importer->SetScalarTypeCallback(exporter->GetScalarTypeCallback());
  importer->SetNumberOfComponentsCallback(exporter->GetNumberOfComponentsCallback());
  importer->SetPropagateUpdateExtentCallback(exporter->GetPropagateUpdateExtentCallback());
  importer->SetUpdateDataCallback(exporter->GetUpdateDataCallback());
  importer->SetDataExtentCallback(exporter->GetDataExtentCallback());
  importer->SetBufferPointerCallback(exporter->GetBufferPointerCallback());
  importer->SetCallbackUserData(exporter->GetCallbackUserData());
}


#define DelegateSetMacro(name,arg) DelegateITKInputMacro(Set##name,arg)
#define DelegateITKInputMacro(name,arg) \
if ( 1 ) { \
  vtkDebugMacro( << this->GetClassName() << " (" << this << "): setting " #name " to " << #arg ); \
  ImageFilterType* tempFilter = dynamic_cast<ImageFilterType*> ( this->m_Filter.GetPointer() ); \
  if ( tempFilter ) \
    { \
    tempFilter->name ( arg ); \
    this->Modified(); \
    } \
  }

#define DelegateGetMacro(name) DelegateITKOutputMacro (Get##name)
#define DelegateITKOutputMacro(name) \
if ( 1 ) { \
  vtkDebugMacro(<< this->GetClassName() << " (" << this << "): returning " #name ); \
  ImageFilterType* tempFilter = dynamic_cast<ImageFilterType*> ( this->m_Filter.GetPointer() ); \
  if ( tempFilter ) \
    { \
    return tempFilter->name (); \
    } \
    else \
    { \
    vtkErrorMacro ( << this->GetClassName() << " Error getting " #name " Dynamic cast returned 0" ); \
    return 0; \
    } \
  }


// struct vtkITKProgressDisplay
// {
//   ProgressDisplay(vtkObject* obj, itk::ProcessObject* process): m_Process(process), m_Object(obj) {}
  
//   void Display()
//   {
//     m_Object->SetProgress ( m_Process->GetProgress() );
//     }
  
//   itk::ProcessObject::Pointer m_Process;
//   vtkObject* m_Object();
// };

//   // Add a progress observer for the itk::CurvatureFlowImageFilter.
//   // This will make it clear when this part of the ITK pipeline
//   // executes.
//   ProgressDisplay progressDisplay(denoiser);
//   itk::SimpleMemberCommand<ProgressDisplay>::Pointer progressEvent =
//     itk::SimpleMemberCommand<ProgressDisplay>::New();
//   progressEvent->SetCallbackFunction(&progressDisplay,
//                                      &ProgressDisplay::Display);
//   denoiser->AddObserver(itk::ProgressEvent(), progressEvent);



#endif

/*=auto=========================================================================
(c) Copyright 2003 Massachusetts Institute of Technology (MIT) All Rights Reserved.

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
#include "vtkMatrix4x4.h"
#include "NewStoppingCondition.h"

namespace itk 
{

NewStoppingCondition::NewStoppingCondition() 
{
  last        = vtkMatrix4x4::New();
  current     = vtkMatrix4x4::New();
  change_mat  = vtkMatrix4x4::New();
  m_Transform = TransformType::New();
  last->Identity();
  current->Identity();
  abort           = 0;
  m_UpdateIter    = 100;
  CallbackData    = NULL;
  m_CurrentIter   = 0;
  m_CurrentLevel  = -1;
}

/* ====================================================================== */

void NewStoppingCondition::Reset()
{
  m_CurrentIter = 0;
  m_CurrentLevel++;
}

/* ====================================================================== */

NewStoppingCondition::~NewStoppingCondition() 
{
  last->Delete();
  current->Delete();
};

/* ====================================================================== */


void NewStoppingCondition::Execute(itk::Object * object, 
                   const itk::EventObject & event)
  {
    OptimizerPointer optimizer = 
                      dynamic_cast< OptimizerPointer >( object );

    if( typeid( event ) != typeid( itk::IterationEvent ) )
      {
      return;
      }
    if (optimizer->GetCurrentIteration() == 0)
      {
        this->Reset();
      }

    this->m_CurrentIter = optimizer->GetCurrentIteration();
    if(m_CurrentIter % m_UpdateIter == 0)
      {
      if (CallbackData != NULL)
       {
         this->abort = this->Callback(CallbackData,
                      this->m_CurrentLevel,
                      this->m_CurrentIter);
       }
      }
    if (this->abort)
      {
      optimizer->StopOptimization();
      return;
      }

    // The current matrix becomes the old one
    last->DeepCopy(current);

    // 
    m_Transform->SetParameters(optimizer->GetCurrentPosition());
    const TransformType::MatrixType ResMat   =m_Transform->GetRotationMatrix();
    const TransformType::OffsetType ResOffset=m_Transform->GetOffset();

    // Copy the Rotation Matrix
    for(int i=0;i<3;i++)
      for(int j=0;j<3;j++)
    current->Element[i][j] = ResMat[i][j];

    // Copy the Offset
    for(int s=0;s<3;s++)
      current->Element[s][3] = ResOffset[s];

    // Fill in the rest
    current->Element[3][0] = 0;
    current->Element[3][1] = 0;
    current->Element[3][2] = 0;
    current->Element[3][3] = 1;

    // Find the change between the last two steps
    change_mat->DeepCopy(last);
    change_mat->Invert();
    change_mat->Multiply4x4(change_mat,current,change_mat);

    // Find the metric
    double distancemetric = 0;
    for(int ii=0;ii<4;ii++)
      for(int jj=0;jj<4;jj++)
    {
      if (ii != jj)
        distancemetric += (change_mat->GetElement(ii,jj)*
                   change_mat->GetElement(ii,jj));
      else
        distancemetric += ((change_mat->GetElement(ii,jj)-1)*
                   (change_mat->GetElement(ii,jj)-1));

    }
    distancemetric = sqrt(distancemetric);

    itkDebugMacro( << optimizer->GetCurrentIteration() << " = ");
    itkDebugMacro( << optimizer->GetValue() << " : ");
    itkDebugMacro( << distancemetric << " : ");
    itkDebugMacro( << optimizer->GetCurrentPosition() << std::endl);
}


} /* end namespace itk */

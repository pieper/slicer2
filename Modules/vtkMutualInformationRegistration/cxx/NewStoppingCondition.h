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
// .NAME NewStoppingCondition - StoppingCondition for MI Registration
// .SECTION Description
// Stopping condition for MutualInformationRegistration algorithm

#ifndef __NewStoppingCondition_h
#define __NewStoppingCondition_h

#include "MIRegistration.h"
#include "vtkMutualInformationRegistrationConfigure.h"

namespace itk
{

class VTK_MUTUALINFORMATIONREGISTRATION_EXPORT NewStoppingCondition : public itk::Command 
{
public:
  /** Standard class typedefs. */
  typedef NewStoppingCondition      Self;
  typedef Command                   Superclass;
  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro( Self );

  /** Run-time type information (and related methods). */
  itkTypeMacro(NewStoppingCondition, Object);

protected:
  NewStoppingCondition();
  ~NewStoppingCondition();

public:
  //
  // The type definitions
  //

  //  typedef RegularStepGradientDescentOptimizer     OptimizerType;
  typedef MIRegistration< Image<float,3>,Image<float,3> >::OptimizerType  OptimizerType;
  typedef OptimizerType                          *OptimizerPointer;

  //  typedef QuaternionRigidTransform< double >       TransformType;
  typedef MIRegistration< Image<float,3>,Image<float,3> >::TransformType TransformType;

  typedef AffineTransform<double,3>   AffineTransformType;
  typedef AffineTransformType::Pointer AffineTransformPointer;

  /** A new iteration is starting, so reset all convergence settings */
  void Reset();

  /** The iteration event has occured */
  void Execute(itk::Object * object, 
               const itk::EventObject & event);

  /** a const execute command, un-necessary */
  void Execute(const itk::Object *caller, const itk::EventObject & event)
  { Execute( (const itk::Object *)caller, event);  }
protected:
  vtkMatrix4x4 *last,*current,*change_mat;
  TransformType::Pointer             m_Transform;
};

} /* end namespace itk */
#endif /* __NewStoppingCondition__h */

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

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkQuaternionRigidTransformGradientDescentOptimizerModified.h,v $
  Language:  C++
  Date:      $Date: 2005/04/19 21:45:32 $
  Version:   $Revision: 1.2 $

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkQuaternionRigidTransformGradientDescentOptimizerModified_h
#define __itkQuaternionRigidTransformGradientDescentOptimizerModified_h

// This class modified from QuaternionRigidTransformGradientDescentOptimizer.
// It (and the cxx file) have 5 changes in order to change inheritance and
// have it compile in this directory:
//
// 1) First, search and replace on 
//   QuaternionRigidTransformGradientDescentOptimizer ->
//     QuaternionRigidTransformGradientDescentOptimizerModified
// 2) Then search and replace on
//     itkGradientDescentOptimizer -> 
//     itkGradientDescentOptimizerWithStopCondition
// 3) Inherit from itkGradientDescentOptimizerWithStopCondition instead
//    of itkGradientDescentOptimizer.
// 4) ITK_EXPORT -> VTK_RIGIDINTENSITYREGISTRATION_EXPORT
// 5) addition of: #include "vtkRigidIntensityRegistrationConfigure.h"

#include "vtkRigidIntensityRegistrationConfigure.h"
#include "itkGradientDescentOptimizerWithStopCondition.h"

namespace itk
{
  
/** \class QuaternionRigidTransformGradientDescentOptimizerModified
 * \brief Implement a gradient descent optimizer
 *
 * QuaternionRigidTransformGradientDescentOptimizerModified is an extension to the
 * simple gradient descent optimizer implmented in GradientDescentOptimizer.
 * At each iteration the current position is updated according to
 *
 * p(n+1) = p(n) + learningRate * d f(p(n)) / d p(n)
 *
 * \f[ 
 *        p_{n+1} = p_n 
 *                + \mbox{learningRate} 
 *                \, \frac{\partial f(p_n) }{\partial p_n} 
 * \f]
 *
 * The learning rate is a fixed scalar defined via SetLearningRate().
 * The optimizer steps through a user defined number of iterations;
 * no convergence checking is done.
 * The first four components of p are assumed to be the four components
 * of the quaternion. After each update, the quaternion is normalized to 
 * have a magnitude of one. This ensures the the transform is purely rigid.
 * 
 * \sa GradientDescentOptimizer
 * \ingroup Numerics Optimizers
 */  

class VTK_RIGIDINTENSITYREGISTRATION_EXPORT QuaternionRigidTransformGradientDescentOptimizerModified : 
        public GradientDescentOptimizerWithStopCondition
{
public:
  /** Standard class typedefs. */
  typedef QuaternionRigidTransformGradientDescentOptimizerModified  Self;
  typedef GradientDescentOptimizerWithStopCondition         Superclass;
  typedef SmartPointer<Self>                                Pointer;
  typedef SmartPointer<const Self>                          ConstPointer;
  
  /** Method for creation through the object factory. */
  itkNewMacro(Self);

 /** Run-time type information (and related methods). */
  itkTypeMacro( QuaternionRigidTransformGradientDescentOptimizerModified, 
      GradientDescentOptimizer );

  /**  Parameters type.
   *  It defines a position in the optimization search space. */
  typedef Superclass::ParametersType ParametersType;

  /** Advance one step following the gradient direction. */
  virtual void AdvanceOneStep( void );

protected:
  QuaternionRigidTransformGradientDescentOptimizerModified();
  virtual ~QuaternionRigidTransformGradientDescentOptimizerModified() {};
  void PrintSelf(std::ostream& os, Indent indent) const;


private:
  QuaternionRigidTransformGradientDescentOptimizerModified(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
  
};

} // end namespace itk


#endif




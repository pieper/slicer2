/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkQuaternionRigidTransformGradientDescentOptimizerModified.h,v $
  Language:  C++
  Date:      $Date: 2003/09/17 16:42:18 $
  Version:   $Revision: 1.1 $

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
// 2) #include "itkGradientDescentOptimizerWithStopCondition.h"
//    instead of #include "itkGradientDescentOptimizer.h"
// 3) Inherit from itkGradientDescentOptimizerWithStopCondition instead
//    of itkGradientDescentOptimizer.
// 4) ITK_EXPORT -> VTK_MUTUALINFORMATIONREGISTRATION_EXPORT
// 5) addition of: #include "vtkMutualInformationRegistrationConfigure.h"

#include "vtkMutualInformationRegistrationConfigure.h"
#include "itkGradientDescentOptimizer.h"

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

class VTK_MUTUALINFORMATIONREGISTRATION_EXPORT QuaternionRigidTransformGradientDescentOptimizerModified : 
        public GradientDescentOptimizer
{
public:
  /** Standard class typedefs. */
  typedef QuaternionRigidTransformGradientDescentOptimizerModified  Self;
  typedef GradientDescentOptimizer                          Superclass;
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




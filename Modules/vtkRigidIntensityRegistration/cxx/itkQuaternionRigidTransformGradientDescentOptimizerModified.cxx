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
  Module:    $RCSfile: itkQuaternionRigidTransformGradientDescentOptimizerModified.cxx,v $
  Language:  C++
  Date:      $Date: 2005/04/19 21:45:32 $
  Version:   $Revision: 1.2 $

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _itkQuaternionRigidTransformGradientDescentOptimizerModified_txx
#define _itkQuaternionRigidTransformGradientDescentOptimizerModified_txx

#include "itkQuaternionRigidTransformGradientDescentOptimizerModified.h"
#include "vnl/vnl_quaternion.h"
#include "itkEventObject.h"

namespace itk
{

/**
 * Constructor
 */
QuaternionRigidTransformGradientDescentOptimizerModified
::QuaternionRigidTransformGradientDescentOptimizerModified()
{
}


void
QuaternionRigidTransformGradientDescentOptimizerModified
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
}


/**
 * Advance one Step following the gradient direction
 */
void
QuaternionRigidTransformGradientDescentOptimizerModified
::AdvanceOneStep( void )
{ 

  double direction;
  if( m_Maximize ) 
  {
    direction = 1.0;
  }
  else 
  {
    direction = -1.0;
  }


  ScalesType scales = this->GetScales();

  const unsigned int spaceDimension = 
                        m_CostFunction->GetNumberOfParameters();

  DerivativeType transformedGradient( spaceDimension);
  for ( unsigned int i=0; i< spaceDimension; i++)
    {
    transformedGradient[i] = m_Gradient[i] / scales[i];
    }

  ParametersType currentPosition = this->GetCurrentPosition();

  // compute new quaternion value
  vnl_quaternion<double> newQuaternion;
  for ( unsigned int j=0; j < 4; j++ )
    {
    newQuaternion[j] = currentPosition[j] + direction * m_LearningRate *
      transformedGradient[j];
    }

  newQuaternion.normalize();

  ParametersType newPosition( spaceDimension );
  // update quaternion component of currentPosition
  for ( unsigned int j=0; j < 4; j++ )
    {
    newPosition[j] = newQuaternion[j];
    }
  
  // update the translation component
  for (unsigned int j=4; j< spaceDimension; j++)
  {
    newPosition[j] = currentPosition[j] + 
      direction * m_LearningRate * transformedGradient[j];
  }

  this->SetCurrentPosition( newPosition );

  this->InvokeEvent( IterationEvent() );

}



} // end namespace itk

#endif

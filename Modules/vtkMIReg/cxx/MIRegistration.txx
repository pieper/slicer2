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
#ifndef __MIRegistration_txx
#define __MIRegistration_txx

#include "MIRegistration.h"

#include "itkCommand.h"

#include "vtkMatrix4x4.h"

namespace itk
{


template <typename TFixedImage, typename TMovingImage>
MIRegistration<TFixedImage,TMovingImage>::MIRegistration()
{
  // Images need to be set from the outside
  m_FixedImage  = NULL;
  m_MovingImage = NULL;

  // The itk components
  m_Transform          = TransformType::New();
  m_Optimizer          = OptimizerType::New();
  m_Metric             = MetricType::New();
  m_Interpolator       = InterpolatorType::New();
  m_FixedImagePyramid  = FixedImagePyramidType::New();
  m_MovingImagePyramid = MovingImagePyramidType::New();
  m_Registration       = RegistrationType::New();

  // Connect them all together
  m_Registration->SetTransform(m_Transform);
  m_Registration->SetOptimizer(m_Optimizer);
  m_Registration->SetMetric(m_Metric);
  m_Registration->SetInterpolator(m_Interpolator);
  m_Registration->SetFixedImagePyramid(m_FixedImagePyramid);
  m_Registration->SetMovingImagePyramid(m_MovingImagePyramid);

  // Default parameters
  m_NumberOfLevels = 1;
  m_TranslationScale = 1.0;
  m_MovingImageStandardDeviation = 0.4;
  m_FixedImageStandardDeviation = 0.4;
  m_NumberOfSpatialSamples = 50;

  m_FixedImageShrinkFactors.Fill(1);
  m_MovingImageShrinkFactors.Fill(1);

  m_NumberOfIterations = UnsignedIntArray(1);
  m_NumberOfIterations.Fill(10);

  m_LearningRates = DoubleArray(1);
  m_LearningRates.Fill(1e-4);

  m_InitialParameters = ParametersType(m_Transform->GetNumberOfParameters());
  m_InitialParameters.Fill(0.0);
  m_InitialParameters[3] = 1.0;

  // Not sure about this affine transform yet
  m_AffineTransform  = AffineTransformType::New();

  // Setup a registration observer
  // Not Sure what to do with observers
  typedef SimpleMemberCommand<Self> CommandType;
  typename CommandType::Pointer command = CommandType::New();
  command->SetCallbackFunction( this, &Self::StartNewLevel );
  m_Tag = m_Registration->AddObserver( IterationEvent(), command );
}

//----------------------------------------------------------------------

// Clean Up: Remove the observer
template <typename TFixedImage, typename TMovingImage>
MIRegistration<TFixedImage,TMovingImage>::~MIRegistration()
{
  m_Registration->RemoveObserver(m_Tag);
}


//----------------------------------------------------------------------------

// Go from an initial Matrix To Setting the initial Parameters of the Registration
template <typename TFixedImage, typename TMovingImage>
void MIRegistration<TFixedImage,TMovingImage>::InitializeRegistration(
                         vtkMatrix4x4 *matrix)
{
  vnl_matrix<double> matrix3x4(3,4);
  matrix3x4[0][0] = matrix->Element[0][0];
  matrix3x4[0][1] = matrix->Element[0][1];
  matrix3x4[0][2] = matrix->Element[0][2];
  matrix3x4[0][3] = matrix->Element[0][3];
  matrix3x4[1][0] = matrix->Element[1][0];
  matrix3x4[1][1] = matrix->Element[1][1];
  matrix3x4[1][2] = matrix->Element[1][2];
  matrix3x4[1][3] = matrix->Element[1][3];
  matrix3x4[2][0] = matrix->Element[2][0];
  matrix3x4[2][1] = matrix->Element[2][1];
  matrix3x4[2][2] = matrix->Element[2][2];
  matrix3x4[2][3] = matrix->Element[2][3];

  vnl_quaternion<double> matrixAsQuaternion(matrix3x4);

  //  typename RegistrationType::ParametersType guess(transform->GetNumberOfParameters() );
  // Quaternions have 7 parameters. The first four represents the
  // quaternion and the last three represents the offset. 

  m_InitialParameters[0] = matrixAsQuaternion.x();
  m_InitialParameters[1] = matrixAsQuaternion.y();
  m_InitialParameters[2] = matrixAsQuaternion.z();
  m_InitialParameters[3] = matrixAsQuaternion.r();
  m_InitialParameters[4] = matrix->Element[0][3];
  m_InitialParameters[5] = matrix->Element[1][3];
  m_InitialParameters[6] = matrix->Element[2][3];

  // The guess is: a quaternion followed by a translation
  m_Registration->SetInitialTransformParameters(m_InitialParameters);
}

//----------------------------------------------------------------------------

// Go from the Registration Results to a Matrix
template <typename TFixedImage, typename TMovingImage>
void MIRegistration<TFixedImage,TMovingImage>::ResultsToMatrix(
                         vtkMatrix4x4 *matrix)
{
  // Get the results
  ParametersType solution = this->GetTransformParameters();

  vnl_quaternion<double> quat(solution[0],solution[1],solution[2],solution[3]);
  vnl_matrix_fixed<double,3,3> mat = quat.rotation_matrix();
  
  // Convert the vnl matrix to a vtk mtrix
  matrix->Element[0][0] = mat(0,0);
  matrix->Element[0][1] = mat(0,1);
  matrix->Element[0][2] = mat(0,2);
  matrix->Element[0][3] = solution[4];
  matrix->Element[1][0] = mat(1,0);
  matrix->Element[1][1] = mat(1,1);
  matrix->Element[1][2] = mat(1,2);
  matrix->Element[1][3] = solution[5];
  matrix->Element[2][0] = mat(2,0);
  matrix->Element[2][1] = mat(2,1);
  matrix->Element[2][2] = mat(2,2);
  matrix->Element[2][3] = solution[6];
  matrix->Element[3][0] = 0;
  matrix->Element[3][1] = 0;
  matrix->Element[3][2] = 0;
  matrix->Element[3][3] = 1;
}

//----------------------------------------------------------------------

// Do the registration
template <typename TFixedImage, typename TMovingImage>
void MIRegistration<TFixedImage,TMovingImage>::Execute()
{
  //
  // Setup the optimizer
  //
  typename OptimizerType::ScalesType 
    scales(m_Transform->GetNumberOfParameters());
  scales.Fill(1.0);

  for ( int j = 4; j < 7; j++ )
    {
      scales[j] = m_TranslationScale;
    }

  m_Optimizer->SetScales( scales );
  m_Optimizer->MaximizeOn();

  //
  // Setup the metric
  //
  m_Metric->SetMovingImageStandardDeviation(m_MovingImageStandardDeviation);
  m_Metric->SetFixedImageStandardDeviation(m_FixedImageStandardDeviation);
  m_Metric->SetNumberOfSpatialSamples(m_NumberOfSpatialSamples);

  //
  // Setup the image pyramids
  //
  m_FixedImagePyramid->SetNumberOfLevels(m_NumberOfLevels);
  m_FixedImagePyramid->SetStartingShrinkFactors(
    m_FixedImageShrinkFactors.GetDataPointer());

  m_MovingImagePyramid->SetNumberOfLevels(m_NumberOfLevels);
  m_MovingImagePyramid->SetStartingShrinkFactors(
    m_MovingImageShrinkFactors.GetDataPointer());

  //
  // Setup the registrator
  //
  m_Registration->SetFixedImage(m_FixedImage);
  m_Registration->SetMovingImage(m_MovingImage);
  m_Registration->SetNumberOfLevels(m_NumberOfLevels);
 
  m_Registration->SetInitialTransformParameters(m_InitialParameters);
  m_Registration->SetFixedImageRegion(
                      m_FixedImage->GetBufferedRegion());

  //
  // Do the Registration
  //
  try { m_Registration->StartRegistration();  }
  catch( itk::ExceptionObject & err )
    {
      std::cout << "Caught an exception: " << std::endl;
      std::cout << err << std::endl;
      throw err;
    }
}

//----------------------------------------------------------------------

template <typename TFixedImage, typename TMovingImage>
typename MIRegistration<TFixedImage,TMovingImage>::AffineTransformPointer
MIRegistration<TFixedImage,TMovingImage>::GetAffineTransform()
{
  m_Transform->SetParameters(m_Registration->GetLastTransformParameters());
  m_AffineTransform->SetMatrix(m_Transform->GetRotationMatrix());
  m_AffineTransform->SetOffset(m_Transform->GetOffset());

  return m_AffineTransform;
}

//----------------------------------------------------------------------


template <typename TFixedImage, typename TMovingImage>
void MIRegistration<TFixedImage,TMovingImage>::StartNewLevel()
{
  std::cout << "--- Starting level " 
        << m_Registration->GetCurrentLevel()
            << std::endl;

  unsigned int level = m_Registration->GetCurrentLevel();
  if (m_NumberOfIterations.Size() >= level + 1)
    {
      m_Optimizer->SetNumberOfIterations(m_NumberOfIterations[level]);
    }

  if (m_LearningRates.Size() >= level + 1)
    {
      m_Optimizer->SetLearningRate( m_LearningRates[level] );
    }

  std::cout << " No. Iterations: " 
            << m_Optimizer->GetNumberOfIterations()
            << " Learning rate: "
            << m_Optimizer->GetLearningRate()
            << std::endl;
}

} // namespace itk


#endif /* __MIRegistration_txx */

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
#ifndef __KLRegistration_txx
#define __KLRegistration_txx

#include "KLRegistration.h"

#include "itkCommand.h"

#include "vtkMatrix4x4.h"
// #include "NewStoppingCondition.h"

namespace itk
{

template <typename TFixedImage, typename TMovingImage>
KLRegistration<TFixedImage,TMovingImage>::KLRegistration()
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

  // Default number of bins
  unsigned int nBins = 32;
  SizeType histSize;
  histSize[0] = nBins;
  histSize[1] = nBins;
  m_Metric->UsePaddingValue(false);
  m_Metric->SetHistogramSize(histSize);
  m_Metric->SetDerivativeStepLength(0.1); // 0.1 mm
  // possible memory leak
  InterpolatorType::Pointer  TrainingInterpolator = InterpolatorType::New();
  m_Metric->SetTrainingInterpolator(TrainingInterpolator);

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

  // Setup a registration observer
  // The observer watches when a new level is started
  typedef SimpleMemberCommand<Self> CommandType;
  typename CommandType::Pointer command = CommandType::New();
  command->SetCallbackFunction( this, &Self::StartNewLevel );
  m_ObserverTag = m_Registration->AddObserver( IterationEvent(), command );

  // Set up an observer that searcher for convergence
//  NewStoppingCondition::Pointer StoppingObserver = NewStoppingCondition::New();
//  m_OptimizeObserverTag = m_Optimizer->AddObserver( IterationEvent(), 
//                               StoppingObserver );

  // Ability to test the parameters to matrix and reverse
  //  this->Test();
}

//----------------------------------------------------------------------

// Clean Up: Remove the observer
template <typename TFixedImage, typename TMovingImage>
KLRegistration<TFixedImage,TMovingImage>::~KLRegistration()
{
  m_Registration->RemoveObserver(m_ObserverTag);
  //  m_Optimizer->RemoveObserver(m_OptimizeObserverTag);
}


//----------------------------------------------------------------------------

template < typename TFixedImage, typename TMovingImage  >
void KLRegistration<TFixedImage,TMovingImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "NumberOfLevels: "  << m_NumberOfLevels << endl;
  os << indent << "TranslationScale: " 
        <<   m_TranslationScale             << endl;
  os << indent << "MovingImageStandardDeviation: " 
        << m_MovingImageStandardDeviation   << endl;
  os << indent << "FixedImageStandardDeviation: " 
        <<   m_FixedImageStandardDeviation  << endl; 
  os << indent << "NumberOfSpatialSamples: " 
        <<   m_NumberOfSpatialSamples       << endl;
  os << indent << "NumberOfIterations: " 
        <<   m_NumberOfIterations           << endl;
  os << indent << "LearningRates: " 
        <<   m_LearningRates                << endl;
  os << indent << "MovingImageShrinkFactors: " 
        <<   m_MovingImageShrinkFactors     << endl;
  os << indent << "FixedImageShrinkFactors: " 
        <<   m_FixedImageShrinkFactors      << endl;
  os << indent << "InitialParameters: " 
        <<   m_InitialParameters            << endl;
  os << indent << "ObserveTag: "  <<   m_ObserverTag  << endl;
  os << indent << "OptimizeTag: " <<   m_OptimizeObserverTag  << endl;

  os << indent << "FixedImage: " <<  m_FixedImage          << endl;
  os << indent << "MovingImage: " <<  m_MovingImage         << endl;
  os << indent << "Transform: " <<  m_Transform           << endl;
  os << indent << "Optimizer: " <<  m_Optimizer           << endl;
  os << indent << "Metric: " <<  m_Metric              << endl;
  os << indent << "Interpolator: " <<  m_Interpolator        << endl;
  os << indent << "FixedImagePyramid: " <<  m_FixedImagePyramid   << endl;
  os << indent << "MovingImagePyramid: " <<  m_MovingImagePyramid  << endl;
  os << indent << "Registration: " <<  m_Registration        << endl;

}



//----------------------------------------------------------------------------

// Go from an initial Matrix To Setting the initial Parameters of the Registration
template <typename TFixedImage, typename TMovingImage>
void KLRegistration<TFixedImage,TMovingImage>::InitializeRegistration(
                         vtkMatrix4x4 *matrix)
{
  vnl_matrix<double> matrix3x4(3,4);

  for(int i=0;i<3;i++)
    for(int j=0;j<4;j++)
      matrix3x4[i][j] = matrix->Element[i][j];
  
  vnl_quaternion<double> matrixAsQuaternion(matrix3x4);

  // There is a transpose between the vnl quaternion and itk quaternion.
  vnl_quaternion<double> conjugated = matrixAsQuaternion.conjugate();

  // This command automatically does the conjugate.
  // But, it does not calculate the paramaters
  // m_Transform->SetRotation(matrixAsQuaternion);

  // Quaternions have 7 parameters. The first four represents the
  // quaternion and the last three represents the offset. 
  m_InitialParameters[0] = conjugated.x();
  m_InitialParameters[1] = conjugated.y();
  m_InitialParameters[2] = conjugated.z();
  m_InitialParameters[3] = conjugated.r();
  m_InitialParameters[4] = matrix->Element[0][3];
  m_InitialParameters[5] = matrix->Element[1][3];
  m_InitialParameters[6] = matrix->Element[2][3];

  // The guess is: a quaternion followed by a translation
  m_Registration->SetInitialTransformParameters(m_InitialParameters);
  m_Transform->SetParameters(m_InitialParameters);
}

//----------------------------------------------------------------------------

// Go from the Param to Matrix
template <typename TFixedImage, typename TMovingImage>
void KLRegistration<TFixedImage,TMovingImage>::ParamToMatrix(
                         const ParametersType &Param,
                         vtkMatrix4x4 *matrix)
{
  m_Transform->SetParameters(Param);
  m_Transform->GetRotationMatrix();

  const TransformType::MatrixType ResMat   =m_Transform->GetRotationMatrix();
  const TransformType::OffsetType ResOffset=m_Transform->GetOffset();

  // Copy the Rotation Matrix
  for(int i=0;i<3;i++)
    for(int j=0;j<3;j++)
      matrix->Element[i][j] = ResMat[i][j];

  // Copy the Offset
  for(int s=0;s<3;s++)
    matrix->Element[s][3] = ResOffset[s];

  // Fill in the rest
  matrix->Element[3][0] = 0;
  matrix->Element[3][1] = 0;
  matrix->Element[3][2] = 0;
  matrix->Element[3][3] = 1;
}

//----------------------------------------------------------------------------

template <typename TFixedImage, typename TMovingImage>
void KLRegistration<TFixedImage,TMovingImage>::Test()
{
  ParametersType test = ParametersType(m_Transform->GetNumberOfParameters());
  test[0] = 0.08428825861139;
  test[1] = 0.11238434481518;
  test[2] = 0.14048043101898;
  test[3] = 0.98006657784124;
  test[4] = 3.1;
  test[5] = 6.1;
  test[6] = 5.2;

  vtkMatrix4x4 *mat = vtkMatrix4x4::New();
  ParamToMatrix(test,mat);
  InitializeRegistration(mat);

  std::cout << "Testing for initial stuff " 
           << m_InitialParameters << endl;

  mat->Delete();
}

//----------------------------------------------------------------------

// Do the registration
template <typename TFixedImage, typename TMovingImage>
void KLRegistration<TFixedImage,TMovingImage>::Execute()
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
      // scales[j] = 1.0/vnl_math_sqr(m_TranslationScale);
    }

  m_Optimizer->SetScales( scales );
  m_Optimizer->MaximizeOn();

  //
  // Setup the metric
  //

  // DO NOT NEED THIS
  //m_Metric->SetMovingImageStandardDeviation(m_MovingImageStandardDeviation);
  //m_Metric->SetFixedImageStandardDeviation(m_FixedImageStandardDeviation);
  //m_Metric->SetNumberOfSpatialSamples(m_NumberOfSpatialSamples);

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

  std::cout << "Starting Iteration" << endl;
  this->Print(std::cout);
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

  std::cout << "Ending Iteration" << endl;
  this->Print(std::cout);
}

//----------------------------------------------------------------------

template <typename TFixedImage, typename TMovingImage>
void KLRegistration<TFixedImage,TMovingImage>::StartNewLevel()
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


#endif /* __KLRegistration_txx */

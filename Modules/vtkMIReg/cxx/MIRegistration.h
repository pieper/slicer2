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
#ifndef __MIRegistration_h
#define __MIRegistration_h
// .NAME MIRegistration - uses Mutual Information to Register 2 images 
// .SECTION Description
// MIRegistration computes a transformation that will align
// the source image with the target image.
//
// The algorithm is described in the paper: Viola, P. and Wells III,
// W. (1997).  "Alignment by Maximization of Mutual Information"
// International Journal of Computer Vision, 24(2):137-154
//
// This class was adopted by a class first written by
// Steve Pieper. It was also strongly derived from one of
// the ITK application Examples: the MultiResolutionMIRegistration.
//
//
// It uses the ITK registration framework with
// the following combination of components:
//   - MutualInformationImageToImageMetric
//   - QuaternionRigidTransform
//   - QuaternionRigidTransformGradientDescentOptimizer
//   - LinearInterpolateImageFunction
// 
// The registration is done using a multiresolution strategy.
// At each resolution level, the downsampled images are obtained
// using a RecursiveMultiResolutionPyramidImageFilter.
// 
// Note that this class requires both images to be 3D and with
// pixels of a real type.
// 
// The registration process is activated by method Execute().
// 
// Inputs:
//   - pointer to fixed image
//   - pointer to moving image
//   - number of resolution levels
//   - scaling applied to the translation parameters during optimization
//   - parzen window width for the fixed image
//   - parzen window width for the moving image
//   - number of optimization iterations at each level
//   - the optimization learning rate at each level
//   - the initial rigid (quaternion) transform parameters
//   - the coarest level shrink factors for the fixed image
//   - the coarest level shrink factors for the moving image
// 
// Outputs:
//   - rigid (quaternion) transform parameters to maps points from
//     the fixed image to the moving image.
//   - pointer to equivalent affine transform.
//
//
// .SECTION Thanks
// Thanks to Samson Timoner who created this class.

#include "itkObject.h"
#include "itkMultiResolutionImageRegistrationMethod.h"
#include "itkAffineTransform.h"

#include "itkQuaternionRigidTransform.h"
#include "itkMutualInformationImageToImageMetric.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkQuaternionRigidTransformGradientDescentOptimizer.h"
#include "itkRecursiveMultiResolutionPyramidImageFilter.h"

#include "itkArray.h"

class vtkMatrix4x4;

namespace itk
{

template <typename TFixedImage, typename TMovingImage>
class MIRegistration : public Object
{
public:

  /** Standard class typedefs. */
  typedef MIRegistration Self;
  typedef Object Superclass;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  /** Run-time type information (and related methods). */
  itkTypeMacro(MIRegistration, Object);

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  // ----------------------------------------------------------------------
  // All the Type Definitions
  // ----------------------------------------------------------------------

  /** Fixed Image Type. */
  typedef TFixedImage FixedImageType;

  /** Moving Image Type. */
  typedef TMovingImage MovingImageType;

  /** Image dimension enumeration. */
  itkStaticConstMacro (ImageDimension, unsigned int, TFixedImage::ImageDimension);

  /** Transform Type. */
  typedef QuaternionRigidTransform< double >       TransformType;

  /** Optimizer Type. */
  typedef QuaternionRigidTransformGradientDescentOptimizer 
                                                         OptimizerType;

  /** Metric Type. */
  typedef MutualInformationImageToImageMetric< 
                                    FixedImageType, 
                                    MovingImageType >    MetricType;

  /** Interpolation Type. */
  typedef LinearInterpolateImageFunction< 
                                    MovingImageType,
                                    double          >    InterpolatorType;

  /** Fixed Image Pyramid Type. */
  typedef RecursiveMultiResolutionPyramidImageFilter<
                                    FixedImageType,
                                    FixedImageType  >    FixedImagePyramidType;

  /** Moving Image Pyramid Type. */
  typedef RecursiveMultiResolutionPyramidImageFilter<
                                    MovingImageType,
                                    MovingImageType  >   MovingImagePyramidType;

  /** Registration Method. */
  typedef MultiResolutionImageRegistrationMethod< 
                                    FixedImageType, 
                                    MovingImageType >    RegistrationType;

  /** Transform parameters type. */
  typedef typename RegistrationType::ParametersType     ParametersType;

  /** DoubleArray type. */
  typedef Array<double>  DoubleArray;

  /** UnsignedIntArray type. */
  typedef Array<unsigned int> UnsignedIntArray;

  /** ShrinkFactorsArray type. */
  typedef FixedArray<unsigned int,itkGetStaticConstMacro(ImageDimension)> ShrinkFactorsArray;

  /** Affine transform type. */
  typedef AffineTransform<double,itkGetStaticConstMacro(ImageDimension)>   AffineTransformType;
  typedef typename AffineTransformType::Pointer AffineTransformPointer;

  // ----------------------------------------------------------------------
  // Set the Parameters for the Registration
  // ----------------------------------------------------------------------

  /** Set the fixed image. */
  itkSetObjectMacro( FixedImage, FixedImageType );

  /** Set the moving image. */
  itkSetObjectMacro( MovingImage, MovingImageType );

  /** Set the number of resolution levels. */
  itkSetClampMacro( NumberOfLevels, unsigned short, 1,
    NumericTraits<unsigned short>::max() );

  /** Set the translation parameter scales. */
  itkSetClampMacro( TranslationScale, double, 0.0,
    NumericTraits<double>::max() );

  /** Set the image parzen window widths. */
  itkSetClampMacro( MovingImageStandardDeviation, double, 0.0,
    NumericTraits<double>::max() );
  itkSetClampMacro( FixedImageStandardDeviation, double, 0.0,
    NumericTraits<double>::max() );

  /** Set the number of spatial samples. */
  itkSetClampMacro( NumberOfSpatialSamples, unsigned short, 1,
    NumericTraits<unsigned short>::max() );

  /** Set the number of iterations per level. */
  itkSetMacro( NumberOfIterations, UnsignedIntArray );

  /** Set the learning rate per level. */
  itkSetMacro( LearningRates, DoubleArray );

  /** Set the initial transform parameters. */
  itkSetMacro( InitialParameters, ParametersType );

  /** Set the fixed and moving image shrink factors. */
  itkSetMacro( FixedImageShrinkFactors, ShrinkFactorsArray );
  itkSetMacro( MovingImageShrinkFactors, ShrinkFactorsArray );

  // Description:
  // Initialize the Registration using a matrix
  void InitializeRegistration(vtkMatrix4x4 *matrix);

  // ----------------------------------------------------------------------
  // Run the Registration
  // ----------------------------------------------------------------------

  /** Method to execute the registration. */
  virtual void Execute();

  /** Initialize registration at the start of new level. */
  void StartNewLevel();

  // ----------------------------------------------------------------------
  // Get Parameters/Results
  // ----------------------------------------------------------------------

  /** Get number of parameters. */
  unsigned long GetNumberOfParameters()
    { return m_Transform->GetNumberOfParameters(); }

  /** Get computed transform parameters. */
  const ParametersType& GetTransformParameters()
    { return m_Registration->GetLastTransformParameters(); }

  /** Get computed affine transform. */
  AffineTransformPointer GetAffineTransform();

  // Description:
  // Set the Matrix using the current results of the registration
  void ResultsToMatrix(vtkMatrix4x4 *matrix);

  // Description:
  // How good was the alignment
  double GetMetricValue()
    {return m_Metric->GetValue(this->GetTransformParameters());}

protected:
  MIRegistration();
  ~MIRegistration();

private:
  MIRegistration( const Self& ); //purposely not implemented
  void operator=( const Self& ); //purposely not implemented

  typename FixedImageType::Pointer            m_FixedImage;
  typename MovingImageType::Pointer           m_MovingImage;
  typename TransformType::Pointer             m_Transform;
  typename OptimizerType::Pointer             m_Optimizer;
  typename MetricType::Pointer                m_Metric;
  typename InterpolatorType::Pointer          m_Interpolator;
  typename FixedImagePyramidType::Pointer     m_FixedImagePyramid;
  typename MovingImagePyramidType::Pointer    m_MovingImagePyramid;
  typename RegistrationType::Pointer          m_Registration;

  unsigned short                       m_NumberOfLevels;
  double                               m_TranslationScale;
  double                               m_MovingImageStandardDeviation;
  double                               m_FixedImageStandardDeviation;
  unsigned short                       m_NumberOfSpatialSamples;
                   
  UnsignedIntArray                     m_NumberOfIterations;
  DoubleArray                          m_LearningRates;
                   
  ShrinkFactorsArray                   m_MovingImageShrinkFactors;
  ShrinkFactorsArray                   m_FixedImageShrinkFactors;
                   
  ParametersType                       m_InitialParameters;
  AffineTransformPointer               m_AffineTransform;
                   
  unsigned long                        m_Tag;

};

} // namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "MIRegistration.txx"
#endif

#endif

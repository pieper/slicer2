/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkKLHistogramImageToImageMetric.h,v $
  Language:  C++
  Date:      $Date: 2003/12/12 22:37:17 $
  Version:   $Revision: 1.6 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkKLHistogramImageToImageMetric_h
#define __itkKLHistogramImageToImageMetric_h

#include "itkHistogramImageToImageMetric.h"

namespace itk
{
  /** \class KLHistogramImageToImageMetric
       \brief Computes the Kubler Lieblach metric between two images to
       be registered by comparing the histograms of the intensities in the 
       images to a Training histogram.

      This class is templated over the type of the fixed and moving
      images to be compared.

      This metric computes the similarity between the histogram produced
      by two images overlapping and a training histogram.

      Generally, the histogram from the pre-aligned data is to be
      computed in exactly the same way as the way the histogram from
      the images to be compared are computed. Thus, the user can set
      the interpolator, region, two training images and the transfrom
      and the histogram will be formed. OR, the user can simply set
      the histogram.

      \ingroup RegistrationMetrics */
template <class TFixedImage, class TMovingImage>
class ITK_EXPORT KLHistogramImageToImageMetric :
public HistogramImageToImageMetric<TFixedImage, TMovingImage>
{
 public:
  /** Standard class typedefs. */
  typedef KLHistogramImageToImageMetric Self;
  typedef HistogramImageToImageMetric<TFixedImage, TMovingImage> Superclass;
  typedef SmartPointer<Self>                                     Pointer;
  typedef SmartPointer<const Self>                               ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(KLHistogramImageToImageMetric,
    HistogramImageToImageMetric);

  /** Types transferred from the base class */
  typedef typename Superclass::RealType                 RealType;
  typedef typename Superclass::TransformType            TransformType;
  typedef typename Superclass::TransformPointer         TransformPointer;
  typedef typename Superclass::TransformParametersType
    TransformParametersType;
  typedef typename Superclass::TransformJacobianType    TransformJacobianType;
  typedef typename Superclass::GradientPixelType        GradientPixelType;

  typedef typename Superclass::MeasureType              MeasureType;
  typedef typename Superclass::DerivativeType           DerivativeType;
  typedef typename Superclass::FixedImageType           FixedImageType;
  typedef typename Superclass::MovingImageType          MovingImageType;
  typedef typename Superclass::FixedImageConstPointer   FixedImageConstPointer;
  typedef typename Superclass::MovingImageConstPointer
    MovingImageConstPointer;

  typedef typename Superclass::HistogramType            HistogramType;
  typedef typename Superclass::HistogramSizeType        HistogramSizeType;
  typedef typename HistogramType::FrequencyType         HistogramFrequencyType;
  typedef typename HistogramType::Iterator              HistogramIteratorType;
  typedef typename HistogramType::MeasurementVectorType
    HistogramMeasurementVectorType;

  typedef typename HistogramType::Pointer               HistogramPointerType;
  typedef typename TransformType::ConstPointer          TransformConstPointer;
  typedef typename Superclass::Superclass::InterpolatorType InterpolatorType;
  typedef typename Superclass::Superclass::InterpolatorPointer 
    InterpolatorPointer;

  /** Set the histogram to be used in the metric calculation */
  itkSetMacro( TrainingHistogram, HistogramPointerType );

  /** Get the histogram to be used in the metric calculation */
  itkGetConstMacro( TrainingHistogram, HistogramPointerType );

  /** Set the Training Fixed Image.  */
  itkSetConstObjectMacro( TrainingFixedImage, FixedImageType );

  /** Get the Training Fixed Image. */
  itkGetConstObjectMacro( TrainingFixedImage, FixedImageType );

  /** Set the Training Moving Image.  */
  itkSetConstObjectMacro( TrainingMovingImage, MovingImageType );

  /** Get the Training Moving Image. */
  itkGetConstObjectMacro( TrainingMovingImage, MovingImageType );

  /** Set the Training Transform. */
  itkSetObjectMacro( TrainingTransform, TransformType );

  /** Get a pointer to the Transform.  */
  itkGetObjectMacro( TrainingTransform, TransformType );

  /** Set the Interpolator. */
  itkSetObjectMacro( TrainingInterpolator, InterpolatorType );

  /** Get a pointer to the Interpolator.  */
  itkGetObjectMacro( TrainingInterpolator, InterpolatorType );

  /** Set the region over which the training histogram will be computed */
  itkSetMacro( TrainingFixedImageRegion, FixedImageRegionType );

  /** Get the region over which the training histogram will be computed */
  itkGetConstMacro( TrainingFixedImageRegion, FixedImageRegionType );

  /** Set epsilon, the histogram frequency to use if the frequency is 0 */
  itkSetMacro( Epsilon, double );

  /** Get epsilon, the histogram frequency to use if the frequency is 0 */
  itkGetConstMacro( Epsilon, double );

  /** Return the number of parameters required by the Transform */
  unsigned int GetNumberOfParameters(void) const 
  { return m_Transform->GetNumberOfParameters(); }
 
  /** Forms the histogram of the training images to prepare to evaluate the */
  /** metric. Must set all parameters first */
  void Initialize() throw (ExceptionObject);

protected:
  /** Constructor is protected to ensure that \c New() function is used to
      create instances. */
  KLHistogramImageToImageMetric();
  virtual ~KLHistogramImageToImageMetric(){}
  void PrintSelf(std::ostream& os, Indent indent) const;

  /** Form the Histogram for the Training data */
  void FormTrainingHistogram() throw (ExceptionObject);

  /** Evaluates the mutual information from the histogram. */
  virtual MeasureType EvaluateMeasure(HistogramType& histogram) const;

  FixedImageConstPointer  m_TrainingFixedImage;
  MovingImageConstPointer m_TrainingMovingImage;
  TransformPointer        m_TrainingTransform;
  InterpolatorPointer     m_TrainingInterpolator;
  FixedImageRegionType    m_TrainingFixedImageRegion;
  HistogramPointerType    m_TrainingHistogram;
  double                  m_Epsilon;

private:
  // Purposely not implemented.
  KLHistogramImageToImageMetric(Self const&);
  void operator=(Self const&); // Purposely not implemented.
};

} // End namespace itk.

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkKLHistogramImageToImageMetric.txx"
#endif

#endif // __itkKLHistogramImageToImageMetric_h

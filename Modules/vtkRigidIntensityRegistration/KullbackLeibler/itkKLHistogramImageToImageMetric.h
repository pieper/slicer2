/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkKLHistogramImageToImageMetric.h,v $
  Language:  C++
  Date:      $Date: 2003/12/09 16:51:36 $
  Version:   $Revision: 1.2 $

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
       images to a given histogram.

      This class is templated over the type of the fixed and moving
      images to be compared.

      This metric computes the similarity between the histogram produced
      by two images overlapping and a given histogram.

      Generally, the histogram from the pre-aligned data is to be
      computed in exactly the same way as the way the histogram from
      the images to be compared are computed. Thus, the user can 
      set the interpolator, region, two given images and the transfrom
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
  itkSetMacro( GivenHistogram, HistogramPointerType );

  /** Get the histogram to be used in the metric calculation */
  itkGetConstMacro( GivenHistogram, HistogramPointerType );

  /** Set the Given Fixed Image.  */
  itkSetConstObjectMacro( GivenFixedImage, FixedImageType );

  /** Get the Given Fixed Image. */
  itkGetConstObjectMacro( GivenFixedImage, FixedImageType );

  /** Set the Given Moving Image.  */
  itkSetConstObjectMacro( GivenMovingImage, MovingImageType );

  /** Get the Given Moving Image. */
  itkGetConstObjectMacro( GivenMovingImage, MovingImageType );

  /** Set the Given Transform. */
  itkSetObjectMacro( GivenTransform, TransformType );

  /** Get a pointer to the Transform.  */
  itkGetObjectMacro( GivenTransform, TransformType );

  /** Set the Interpolator. */
  itkSetObjectMacro( GivenInterpolator, InterpolatorType );

  /** Get a pointer to the Interpolator.  */
  itkGetObjectMacro( GivenInterpolator, InterpolatorType );

  /** Set the region over which the given histogram will be computed */
  itkSetMacro( GivenFixedImageRegion, FixedImageRegionType );

  /** Get the region over which the given histogram will be computed */
  itkGetConstMacro( GivenFixedImageRegion, FixedImageRegionType );

  /** Set epsilon, the histogram frequency to use if the frequency is 0 */
  itkSetMacro( Epsilon, double );

  /** Get epsilon, the histogram frequency to use if the frequency is 0 */
  itkGetConstMacro( Epsilon, double );

  /** Return the number of parameters required by the Transform */
  unsigned int GetNumberOfParameters(void) const 
  { return m_Transform->GetNumberOfParameters(); }
 
  /** Set the 2 images **/
  /** Set the affine transform in between */

  /** Forms the histogram of the given images to prepare to evaluate the */
  /** metric. Must set all parameters first */
  void Initialize() throw (ExceptionObject);

protected:
  /** Constructor is protected to ensure that \c New() function is used to
      create instances. */
  KLHistogramImageToImageMetric();
  virtual ~KLHistogramImageToImageMetric(){}
  void PrintSelf(std::ostream& os, Indent indent) const;

  /** Form the Histogram for the Given data */
  void FormGivenHistogram() throw (ExceptionObject);

  /** Evaluates the mutual information from the histogram. */
  virtual MeasureType EvaluateMeasure(HistogramType& histogram) const;

  FixedImageConstPointer  m_GivenFixedImage;
  MovingImageConstPointer m_GivenMovingImage;
  TransformPointer        m_GivenTransform;
  InterpolatorPointer     m_GivenInterpolator;
  FixedImageRegionType    m_GivenFixedImageRegion;
  HistogramPointerType    m_GivenHistogram;
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

/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkKLHistogramImageToImageMetric.h,v $
  Language:  C++
  Date:      $Date: 2003/12/12 22:21:29 $
  Version:   $Revision: 1.4 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkKLHistogramImageToImageMetric_h
#define __itkKLHistogramImageToImageMetric_h

#include "itkCompareHistogramImageToImageMetric.h"

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
public CompareHistogramImageToImageMetric<TFixedImage, TMovingImage>
{
 public:
  /** Standard class typedefs. */
  typedef KLHistogramImageToImageMetric Self;
  typedef CompareHistogramImageToImageMetric<TFixedImage, TMovingImage> 
    Superclass;
  typedef SmartPointer<Self>                                     Pointer;
  typedef SmartPointer<const Self>                               ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(KLHistogramImageToImageMetric,
    CompareHistogramImageToImageMetric);

  /** Types transferred from the base class */
  typedef typename Superclass::RealType                 RealType;
  typedef typename Superclass::TransformType            TransformType;
  typedef typename Superclass::TransformPointer         TransformPointer;
  typedef typename Superclass::TransformConstPointer    TransformConstPointer;

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
  typedef typename Superclass::MeasurementVectorType
    HistogramMeasurementVectorType;
  typedef typename Superclass::HistogramFrequencyType  HistogramFrequencyType;
  typedef typename Superclass::HistogramIteratorType   HistogramIteratorType;
  typedef typename Superclass::HistogramPointerType    HistogramPointerType;

  typedef typename Superclass::InterpolatorType        InterpolatorType;
  typedef typename Superclass::InterpolatorPointer     InterpolatorPointer;

protected:
  /** Constructor is protected to ensure that \c New() function is used to
      create instances. */
  KLHistogramImageToImageMetric(){};
  virtual ~KLHistogramImageToImageMetric(){};

  /** Evaluates the mutual information from the histogram. */
  MeasureType EvaluateMeasure(HistogramType& histogram) const;

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

/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkKLHistogramImageToImageMetric.txx,v $
  Language:  C++
  Date:      $Date: 2003/12/09 18:43:01 $
  Version:   $Revision: 1.2 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkKLHistogramImageToImageMetric_txx
#define __itkKLHistogramImageToImageMetric_txx

#include "itkKLHistogramImageToImageMetric.h"
#include "itkHistogram.h"

// Todo: need to access Use_Padding in parent. Make in protected
// need to figure out what to do when "stuff" is not in histogram
// kernel function?

namespace itk
{
  template <class TFixedImage, class TMovingImage>
  KLHistogramImageToImageMetric<TFixedImage, TMovingImage>::
  KLHistogramImageToImageMetric() 
  {
    m_TrainingFixedImage        = 0; // has to be provided by the user.
    m_TrainingMovingImage       = 0; // has to be provided by the user.
    m_TrainingTransform         = 0; // has to be provided by the user.
    m_TrainingInterpolator      = 0; // has to be provided by the user.
    m_TrainingHistogram         = 0; // either provided by the user or created
    m_Epsilon                = 1e-6; // should be smaller than 1/numBins^2
  }

  template <class TFixedImage, class TMovingImage>
  void 
  KLHistogramImageToImageMetric<TFixedImage, TMovingImage>
  ::Initialize()  throw (ExceptionObject)
  {
    Superclass::Initialize();

    if (!m_TrainingHistogram)
      {
    FormTrainingHistogram();
      }
  }

  template <class TFixedImage, class TMovingImage>
  void 
  KLHistogramImageToImageMetric<TFixedImage, TMovingImage>
  ::FormTrainingHistogram() throw (ExceptionObject)
  {
    // Check to make sure everything is set
  if( !m_TrainingTransform )
    {
    itkExceptionMacro(<<"Training Transform is not present");
    }

  if( !m_TrainingInterpolator )
    {
    itkExceptionMacro(<<"Training Interpolator is not present");
    }

  if( !m_TrainingMovingImage )
    {
    itkExceptionMacro(<<"Training MovingImage is not present");
    }

  // If the image is provided by a source, update the source.
  if( m_TrainingMovingImage->GetSource() )
    {
    m_TrainingMovingImage->GetSource()->Update();
    }


  if( !m_TrainingFixedImage )
    {
    itkExceptionMacro(<<"Training FixedImage is not present");
    }

  // If the image is provided by a source, update the source.
  if( m_TrainingFixedImage->GetSource() )
    {
    m_TrainingFixedImage->GetSource()->Update();
    }

  if( m_TrainingFixedImageRegion.GetNumberOfPixels() == 0 )
    {
    itkExceptionMacro(<<"TrainingFixedImageRegion is empty");
    }

  // Make sure the FixedImageRegion is within the FixedImage buffered region
  if ( !m_TrainingFixedImageRegion.Crop( m_TrainingFixedImage->GetBufferedRegion() ))
    {
    itkExceptionMacro(<<"TrainingFixedImageRegion does not overlap the training fixed image buffered region" );
    }

  this->m_TrainingInterpolator->SetInputImage(GetTrainingMovingImage());

  // Create the exact histogram structure as the one to be used
  // to evaluate the metric
  this->m_TrainingHistogram = HistogramType::New();
  this->m_TrainingHistogram->Initialize(this->Superclass::m_HistogramSize,
                     this->Superclass::m_LowerBound,
                     this->Superclass::m_UpperBound);

  typedef itk::ImageRegionConstIteratorWithIndex<FixedImageType>
    TrainingFixedIteratorType;
  typename FixedImageType::IndexType index;
  typename FixedImageType::RegionType fixedRegion;

  TrainingFixedIteratorType ti(this->m_TrainingFixedImage,
                this->m_TrainingFixedImageRegion);

  int NumberOfPixelsCounted = 0;

  ti.GoToBegin();
  while (!ti.IsAtEnd())
  {
    index = ti.GetIndex();

    if (this->m_TrainingFixedImageRegion.IsInside(index) &&
    (!this->Superclass::m_UsePaddingValue ||
     (this->Superclass::m_UsePaddingValue && 
      ti.Get() > this->Superclass::GetPaddingValue())))
    {
      typename Superclass::InputPointType inputPoint;
      this->m_TrainingFixedImage->
    TransformIndexToPhysicalPoint(index, inputPoint);

      typename Superclass::OutputPointType transformedPoint =
    this->m_TrainingTransform->TransformPoint(inputPoint);

      if (this->m_TrainingInterpolator->IsInsideBuffer(transformedPoint))
      {
    const RealType TrainingMovingValue =
      this->m_TrainingInterpolator->Evaluate(transformedPoint);
    const RealType TrainingFixedValue = ti.Get();
    NumberOfPixelsCounted++;

    typename HistogramType::MeasurementVectorType sample;
    sample[0] = TrainingFixedValue;
    sample[1] = TrainingMovingValue;
    this->m_TrainingHistogram->IncreaseFrequency(sample, 1);
      }
    }

    ++ti;
  }

    if (NumberOfPixelsCounted == 0)
      itkExceptionMacro(<< "All the points mapped to outside of the Training moving \
image");
  }

  template <class TFixedImage, class TMovingImage>
  typename KLHistogramImageToImageMetric<TFixedImage, \
  TMovingImage>::MeasureType
  KLHistogramImageToImageMetric<TFixedImage, \
  TMovingImage>
  ::EvaluateMeasure(HistogramType& histogram) const
  {
    // Two terms.
    // First the term that measures the entropy of the term
    // p(x,y) log p(x,y) - p(x,y) log q(x,y)

    MeasureType    KL = NumericTraits<MeasureType>::Zero;
    HistogramFrequencyType totalTrainingFreq = m_TrainingHistogram->GetTotalFrequency();
    HistogramFrequencyType totalMeasuredFreq = histogram.GetTotalFrequency();

    HistogramIteratorType measured_it   = histogram.Begin();
    HistogramIteratorType measured_end  = histogram.End();

    HistogramIteratorType training_it   = m_TrainingHistogram->Begin();
    HistogramIteratorType training_end  = m_TrainingHistogram->End();

    while (measured_it != measured_end)
    {
      HistogramFrequencyType TrainingFreq    = training_it.GetFrequency();
      HistogramFrequencyType MeasuredFreq = measured_it.GetFrequency();

      if (MeasuredFreq == 0) MeasuredFreq = m_Epsilon;
      if (TrainingFreq == 0)    TrainingFreq    = m_Epsilon;

      KL += TrainingFreq*log(TrainingFreq/MeasuredFreq);

      ++measured_it;
      ++training_it;
    }

    if (training_it != training_end)
      itkWarningMacro("The Measured and Training Histograms have different number of bins.");

    KL = -KL/static_cast<MeasureType>(totalTrainingFreq)
      - log(totalMeasuredFreq/totalTrainingFreq);

    return KL;
  }

  template <class TFixedImage, class TMovingImage>
  void KLHistogramImageToImageMetric<TFixedImage, TMovingImage>::
  PrintSelf(std::ostream& os, Indent indent) const
  {
    Superclass::PrintSelf(os, indent);
    os << indent << "Epsilon: ";
    os << m_Epsilon << std::endl;
    os << indent << "TrainingFixedImage: ";
    os << m_TrainingFixedImage << std::endl;
    os << indent << "TrainingMovingImage: ";
    os << m_TrainingMovingImage << std::endl;
    os << indent << "TrainingTransform: ";
    os << m_TrainingTransform << std::endl;
    os << indent << "TrainingInterpolator: ";
    os << m_TrainingInterpolator << std::endl;
    os << indent << "TrainingHistogram: ";
    os << m_TrainingHistogram << std::endl;
  }


} // End namespace itk

#endif // itkKLHistogramImageToImageMetric_txx

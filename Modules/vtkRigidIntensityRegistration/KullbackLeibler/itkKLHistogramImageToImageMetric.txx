/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkKLHistogramImageToImageMetric.txx,v $
  Language:  C++
  Date:      $Date: 2003/12/08 23:12:17 $
  Version:   $Revision: 1.1 $

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
    m_GivenFixedImage        = 0; // has to be provided by the user.
    m_GivenMovingImage       = 0; // has to be provided by the user.
    m_GivenTransform         = 0; // has to be provided by the user.
    m_GivenInterpolator      = 0; // has to be provided by the user.
    m_GivenHistogram         = 0; // either provided by the user or created
    m_Epsilon                = 1e-6;  // should be smaller than 1/numBins^2
  }

  template <class TFixedImage, class TMovingImage>
  void 
  KLHistogramImageToImageMetric<TFixedImage, TMovingImage>
  ::Initialize()  throw (ExceptionObject) 
  {
    Superclass::Initialize();

    if (!m_GivenHistogram)
      {
    FormGivenHistogram();
      }
  }

  template <class TFixedImage, class TMovingImage>
  void 
  KLHistogramImageToImageMetric<TFixedImage, TMovingImage>
  ::FormGivenHistogram() throw (ExceptionObject)
  {
    // Check to make sure everything is set
  if( !m_GivenTransform )
    {
    itkExceptionMacro(<<"Given Transform is not present");
    }

  if( !m_GivenInterpolator )
    {
    itkExceptionMacro(<<"Given Interpolator is not present");
    }

  if( !m_GivenMovingImage )
    {
    itkExceptionMacro(<<"Given MovingImage is not present");
    }

  // If the image is provided by a source, update the source.
  if( m_GivenMovingImage->GetSource() )
    {
    m_GivenMovingImage->GetSource()->Update();
    }


  if( !m_GivenFixedImage )
    {
    itkExceptionMacro(<<"Given FixedImage is not present");
    }

  // If the image is provided by a source, update the source.
  if( m_GivenFixedImage->GetSource() )
    {
    m_GivenFixedImage->GetSource()->Update();
    }

  if( m_GivenFixedImageRegion.GetNumberOfPixels() == 0 )
    {
    itkExceptionMacro(<<"GivenFixedImageRegion is empty");
    }

  // Make sure the FixedImageRegion is within the FixedImage buffered region
  if ( !m_GivenFixedImageRegion.Crop( m_GivenFixedImage->GetBufferedRegion() ))
    {
    itkExceptionMacro(<<"GivenFixedImageRegion does not overlap the given fixed image buffered region" );
    }


  this->m_GivenInterpolator->SetInputImage(GetGivenMovingImage());

  // Create the exact histogram structure as the one to be used
  // to evaluate the metric
  this->m_GivenHistogram = HistogramType::New();
  this->m_GivenHistogram->Initialize(this->Superclass::m_HistogramSize,
                     this->Superclass::m_LowerBound,
                     this->Superclass::m_UpperBound);

  typedef itk::ImageRegionConstIteratorWithIndex<FixedImageType>
    GivenFixedIteratorType;
  typename FixedImageType::IndexType index;
  typename FixedImageType::RegionType fixedRegion;

  GivenFixedIteratorType ti(this->m_GivenFixedImage,
                this->m_GivenFixedImageRegion);

  int NumberOfPixelsCounted = 0;

  ti.GoToBegin();
  while (!ti.IsAtEnd())
  {
    index = ti.GetIndex();

    if (this->m_GivenFixedImageRegion.IsInside(index) &&
    (!this->Superclass::m_UsePaddingValue ||
     (this->Superclass::m_UsePaddingValue && 
      ti.Get() > this->Superclass::GetPaddingValue())))
    {
      typename Superclass::InputPointType inputPoint;
      this->m_GivenFixedImage->
    TransformIndexToPhysicalPoint(index, inputPoint);

      typename Superclass::OutputPointType transformedPoint =
    this->m_GivenTransform->TransformPoint(inputPoint);

      if (this->m_GivenInterpolator->IsInsideBuffer(transformedPoint))
      {
    const RealType GivenMovingValue =
      this->m_GivenInterpolator->Evaluate(transformedPoint);
    const RealType GivenFixedValue = ti.Get();
    NumberOfPixelsCounted++;

    typename HistogramType::MeasurementVectorType sample;
    sample[0] = GivenFixedValue;
    sample[1] = GivenMovingValue;
    this->m_GivenHistogram->IncreaseFrequency(sample, 1);
      }
    }

    ++ti;
  }

    if (NumberOfPixelsCounted == 0)
      itkExceptionMacro(<< "All the points mapped to outside of the Given moving \
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
    HistogramFrequencyType totalGivenFreq = m_GivenHistogram->GetTotalFrequency();
    HistogramFrequencyType totalMeasuredFreq = histogram.GetTotalFrequency();

    HistogramIteratorType measured_it   = histogram.Begin();
    HistogramIteratorType measured_end  = histogram.End();

    HistogramIteratorType given_it   = m_GivenHistogram->Begin();
    HistogramIteratorType given_end  = m_GivenHistogram->End();

    while (measured_it != measured_end)
    {
      HistogramFrequencyType GivenFreq    = given_it.GetFrequency();
      HistogramFrequencyType MeasuredFreq = measured_it.GetFrequency();

      if (MeasuredFreq == 0) MeasuredFreq = m_Epsilon;
      if (GivenFreq == 0)    GivenFreq    = m_Epsilon;

      KL += GivenFreq*log(GivenFreq/MeasuredFreq);

      ++measured_it;
      ++given_it;
    }

    if (given_it != given_end)
      itkWarningMacro("The Measured and Given Histograms have different number of bins.");

    KL = -KL/static_cast<MeasureType>(totalGivenFreq)
      - log(totalMeasuredFreq/totalGivenFreq);

    return KL;
  }

  template <class TFixedImage, class TMovingImage>
  void KLHistogramImageToImageMetric<TFixedImage, TMovingImage>::
  PrintSelf(std::ostream& os, Indent indent) const
  {
    Superclass::PrintSelf(os, indent);
    os << indent << "Epsilon: ";
    os << m_Epsilon << std::endl;
    os << indent << "GivenFixedImage: ";
    os << m_GivenFixedImage << std::endl;
    os << indent << "GivenMovingImage: ";
    os << m_GivenMovingImage << std::endl;
    os << indent << "GivenTransform: ";
    os << m_GivenTransform << std::endl;
    os << indent << "GivenInterpolator: ";
    os << m_GivenInterpolator << std::endl;
    os << indent << "GivenHistogram: ";
    os << m_GivenHistogram << std::endl;
  }


} // End namespace itk

#endif // itkKLHistogramImageToImageMetric_txx

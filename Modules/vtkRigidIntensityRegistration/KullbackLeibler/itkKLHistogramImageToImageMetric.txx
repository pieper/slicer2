/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkKLHistogramImageToImageMetric.txx,v $
  Language:  C++
  Date:      $Date: 2003/12/12 22:21:30 $
  Version:   $Revision: 1.4 $

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
  typename CompareHistogramImageToImageMetric<TFixedImage, \
  TMovingImage>::MeasureType
  CompareHistogramImageToImageMetric<TFixedImage, \
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

} // End namespace itk

#endif // itkKLHistogramImageToImageMetric_txx

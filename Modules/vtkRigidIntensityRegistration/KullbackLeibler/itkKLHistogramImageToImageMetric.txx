/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkKLHistogramImageToImageMetric.txx,v $
  Language:  C++
  Date:      $Date: 2003/12/23 22:44:24 $
  Version:   $Revision: 1.7 $

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
    m_Epsilon                = 1e-12; // should be smaller than 1/numBins^2
  }

  template <class TFixedImage, class TMovingImage>
  void 
  KLHistogramImageToImageMetric<TFixedImage, TMovingImage>
  ::Initialize()  throw (ExceptionObject)
  {
    Superclass::Initialize();
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

    HistogramIteratorType measured_it   = histogram.Begin();
    HistogramIteratorType measured_end  = histogram.End();

    HistogramIteratorType training_it   = m_TrainingHistogram->Begin();
    HistogramIteratorType training_end  = m_TrainingHistogram->End();

    while (measured_it != measured_end)
    {
      // Every bin gets epsilon added to it
      double TrainingFreq = training_it.GetFrequency()+m_Epsilon;
      double MeasuredFreq = measured_it.GetFrequency()+m_Epsilon;

      KL += TrainingFreq*log(TrainingFreq/MeasuredFreq);

      ++measured_it;
      ++training_it;
    }

    if (training_it != training_end)
      itkWarningMacro("The Measured and Training Histograms have different number of bins.");

    // Get the total frequency for each histogram.
    HistogramFrequencyType totalTrainingFreq = m_TrainingHistogram->GetTotalFrequency();
    HistogramFrequencyType totalMeasuredFreq = histogram.GetTotalFrequency();

    // The actual number of total frequency is a bit larger
    // than the number of counst because we add m_Epsilon to every bin
    double AdjustedTotalTrainingFreq = totalTrainingFreq +
      m_HistogramSize[0]*m_HistogramSize[1]*m_Epsilon;
    double AdjustedTotalMeasuredFreq = totalTrainingFreq +
      m_HistogramSize[0]*m_HistogramSize[1]*m_Epsilon;

    KL = -KL/static_cast<MeasureType>(AdjustedTotalTrainingFreq)
      - log(AdjustedTotalMeasuredFreq/AdjustedTotalTrainingFreq);

    return KL;
  }

  template <class TFixedImage, class TMovingImage>
  void KLHistogramImageToImageMetric<TFixedImage, TMovingImage>::
  PrintSelf(std::ostream& os, Indent indent) const
  {
    Superclass::PrintSelf(os, indent);

    os << indent << "Epsilon: ";
    os << m_Epsilon << std::endl;
  }


} // End namespace itk

#endif // itkKLHistogramImageToImageMetric_txx

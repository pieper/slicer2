/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: itkVersorMattesMiVersorRegistrationFilter.cxx,v $
  Date:      $Date: 2005/12/20 22:55:43 $
  Version:   $Revision: 1.7.2.1 $

=========================================================================auto=*/
#include "itkVersorMattesMiVersorRegistrationFilter.h"

itk::itkVersorMattesMiVersorRegistrationFilter::itkVersorMattesMiVersorRegistrationFilter()
{
  m_MinimumStepLength = DoubleArray(1);
  m_MaximumStepLength = DoubleArray(1);
  m_MinimumStepLength.Fill( 0.0001);
  m_MaximumStepLength.Fill (0.2);
  m_NumberOfHistogramBins = 256;
  m_NumberOfSpatialSamples = 100000;
  m_ReinitializeSeed = 0;

  itkVersorMattesMiVersorRegistrationCommand::Pointer observer = itkVersorMattesMiVersorRegistrationCommand::New();
  observer->SetRegistrationFilter(this);
  m_Optimizer->AddObserver( itk::IterationEvent(), observer );
  m_Optimizer->AddObserver( itk::EndEvent(), observer );

  m_Optimizer->MinimizeOn();

  m_Optimizer->SetRelaxationFactor( 0.9 );
  m_Optimizer->SetGradientMagnitudeTolerance( 0.01);

}


void
itk::itkVersorMattesMiVersorRegistrationFilter::SetOptimizerParamters()
{

  typedef OptimizerType::ScalesType       OptimizerScalesType;
  OptimizerScalesType optimizerScales( m_Transform->GetNumberOfParameters() );
  optimizerScales.Fill(1.0);

  optimizerScales[3] = m_TranslationScale;
  optimizerScales[4] = m_TranslationScale;
  optimizerScales[5] = m_TranslationScale;

  m_Optimizer->SetScales( optimizerScales );

  m_Optimizer->SetMaximumStepLength( m_MaximumStepLength(0) ); 
  m_Optimizer->SetMinimumStepLength( m_MinimumStepLength(0) );
  m_Optimizer->SetNumberOfIterations( m_NumberOfIterations[0]);

  // TODO expose as parameters
  //m_Optimizer->SetRelaxationFactor( 0.9 );
  //m_Optimizer->SetGradientMagnitudeTolerance( 0.01);

}

void
itk::itkVersorMattesMiVersorRegistrationFilter::SetMetricParamters()
{
  m_Metric->SetNumberOfHistogramBins( m_NumberOfHistogramBins );
  m_Metric->SetNumberOfSpatialSamples( m_NumberOfSpatialSamples );
  if (m_ReinitializeSeed > 0) {
    m_Metric->ReinitializeSeed (m_ReinitializeSeed);
  }
}


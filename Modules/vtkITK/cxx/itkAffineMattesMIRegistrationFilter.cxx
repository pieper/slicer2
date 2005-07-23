#include "itkAffineMattesMIRegistrationFilter.h"

itk::itkAffineMattesMIRegistrationFilter::itkAffineMattesMIRegistrationFilter()
{
  m_MinimumStepLength = DoubleArray(1);
  m_MaximumStepLength = DoubleArray(1);
  m_MinimumStepLength.Fill( 0.0001);
  m_MaximumStepLength.Fill (0.2);
  m_NumberOfHistogramBins = 256;
  m_NumberOfSpatialSamples = 100000;
  m_ReinitializeSeed = 0;

  itkAffineMattesMIRegistrationCommand::Pointer observer = itkAffineMattesMIRegistrationCommand::New();
  observer->SetRegistrationFilter(this);
  m_Optimizer->AddObserver( itk::IterationEvent(), observer );
  m_Optimizer->AddObserver( itk::EndEvent(), observer );

  m_Optimizer->MinimizeOn();

  m_Optimizer->SetRelaxationFactor( 0.9 );
  m_Optimizer->SetGradientMagnitudeTolerance( 0.01);

}


void
itk::itkAffineMattesMIRegistrationFilter::SetOptimizerParamters()
{

  typedef OptimizerType::ScalesType       OptimizerScalesType;
  OptimizerScalesType optimizerScales( m_Transform->GetNumberOfParameters() );
  optimizerScales.Fill(1.0);
  
  // 12 parameters
  // 3x3 + 3 tarnslations paramters
  optimizerScales[9 ] = m_TranslationScale;
  optimizerScales[10] = m_TranslationScale;
  optimizerScales[11] = m_TranslationScale;

  m_Optimizer->SetScales( optimizerScales );

  m_Optimizer->SetMaximumStepLength( m_MaximumStepLength(0) ); 
  m_Optimizer->SetMinimumStepLength( m_MinimumStepLength(0) );
  m_Optimizer->SetNumberOfIterations( m_NumberOfIterations[0]);

  // TODO expose as parameters
  //m_Optimizer->SetRelaxationFactor( 0.9 );
  //m_Optimizer->SetGradientMagnitudeTolerance( 0.01);

}

void
itk::itkAffineMattesMIRegistrationFilter::SetMetricParamters()
{
  m_Metric->SetNumberOfHistogramBins( m_NumberOfHistogramBins );
  m_Metric->SetNumberOfSpatialSamples( m_NumberOfSpatialSamples );
  if (m_ReinitializeSeed > 0) {
    m_Metric->ReinitializeSeed (m_ReinitializeSeed);
  }
}


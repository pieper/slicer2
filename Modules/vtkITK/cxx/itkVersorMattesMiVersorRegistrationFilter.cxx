#include "itkVersorMattesMiVersorRegistrationFilter.h"

itk::itkVersorMattesMiVersorRegistrationFilter::itkVersorMattesMiVersorRegistrationFilter()
{
  m_MinimumStepLength = 0.0001;
  m_MaximumStepLength = 0.2;
  m_NumberOfHistogramBins = 256;
  m_NumberOfSpatialSamples = 100000;
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

  m_Optimizer->SetMaximumStepLength( m_MaximumStepLength  ); 
  m_Optimizer->SetMinimumStepLength( m_MinimumStepLength );

  // TODO make m_NumberOfIterations array
  m_Optimizer->SetNumberOfIterations( m_NumberOfIterations[0]);

}

void
itk::itkVersorMattesMiVersorRegistrationFilter::SetMetricParamters()
{
  m_Metric->SetNumberOfHistogramBins( m_NumberOfHistogramBins );
  m_Metric->SetNumberOfSpatialSamples( m_NumberOfSpatialSamples );
}


#ifndef __itkTransformRegistrationFilter_txx
#define __itkTransformRegistrationFilter_txx

#include "itkTransformRegistrationFilter.h" // This class
#include "itkProgressAccumulator.h"

namespace itk
{

template <class TOptimizerClass, class TTransformerClass, class TMetricClass >
itk::itkTransformRegistrationFilter<TOptimizerClass, TTransformerClass, TMetricClass >::itkTransformRegistrationFilter()
{
  this->SetNumberOfRequiredInputs( 2 );  

  // registration pipeline
  m_FixedImageCaster   = FixedImageCasterType::New();
  m_MovingImageCaster  = MovingImageCasterType::New();

  //m_FixedNormalizer     = FixedNormalizeFilterType::New();
  //m_MovingNormalizer    = MovingNormalizeFilterType::New();

  m_FixedImagePyramid  = FixedImagePyramidType::New();
  m_MovingImagePyramid = MovingImagePyramidType::New();

  m_Metric              = MetricType::New();
  m_Transform           = TransformType::New();
  m_Optimizer           = OptimizerType::New();
  m_LinearInterpolator  = LinearInterpolatorType::New();
  m_NearestInterpolator = NearestInterpolatorType::New();
  
  m_Registration  = RegistrationType::New();
  
  m_Registration->SetOptimizer(     m_Optimizer     );
  m_Registration->SetInterpolator(  m_LinearInterpolator  );
  m_Registration->SetMetric(        m_Metric        );
  m_Registration->SetTransform(     m_Transform     );

  m_Registration->SetFixedImagePyramid(m_FixedImagePyramid);
  m_Registration->SetMovingImagePyramid(m_MovingImagePyramid);

  m_BackgroundLevel = itk::NumericTraits< float >::Zero;

  m_Resampler = ResampleFilterType::New();

  // Default parameters
  m_NumberOfLevels = 1;
  m_TranslationScale = 0.001;

  m_ShrinkFactors.Fill(1);

  m_NumberOfIterations = UnsignedIntArray(1);
  m_NumberOfIterations.Fill(10);

  m_InitialParameters = ParametersType(m_Transform->GetNumberOfParameters());
  m_InitialParameters.Fill(0.0);

  // Specific parameters must be set in the subclass
  // for example:
  //m_Metric->SetNumberOfHistogramBins( 256 );
  //m_Metric->SetNumberOfSpatialSamples( 100000 );
  //m_LearningRates = DoubleArray(1);
  //m_LearningRates.Fill(1e-4);
  //m_InitialParameters[3] = 1.0;


  // set registration input
  m_Registration->SetFixedImage(  m_FixedImageCaster->GetOutput() );
  m_Registration->SetMovingImage( m_MovingImageCaster->GetOutput() );

  // create wrapper

} // itkTransformRegistrationFilter

template <class TOptimizerClass, class TTransformerClass, class TMetricClass >
void 
itk::itkTransformRegistrationFilter<TOptimizerClass, TTransformerClass, TMetricClass >::GenerateData()
{
  itk::ProgressAccumulator::Pointer progress = itk::ProgressAccumulator::New();
  progress->SetMiniPipelineFilter(this);

  progress->RegisterInternalFilter(m_Registration,1.f);
  
  m_FixedImageCaster->SetInput( this->GetInput() );
  m_MovingImageCaster->SetInput( this->GetInput(1) );

  m_FixedImageCaster->Update();
  m_MovingImageCaster->Update();

  m_Registration->SetFixedImageRegion( m_FixedImageCaster->GetOutput()->GetBufferedRegion() );

  m_Registration->SetNumberOfLevels( m_NumberOfLevels);
  
  // TODO: set number iteration per level
  m_Optimizer->SetNumberOfIterations( m_NumberOfIterations[0] );

  SetOptimizerParamters();
  SetMetricParamters();
  
  m_Registration->SetInitialTransformParameters( m_Transform->GetParameters() );
  
  try { 
    m_Registration->StartRegistration(); 
  } 
  catch( itk::ExceptionObject & err ) { 
    std::cout << "itkTransformRegistrationFilter:ExceptionObject caught !" << std::endl; 
    std::cout << err << std::endl; 
  } 

  m_FinalParameters = m_Registration->GetLastTransformParameters();

  typename TransformType::Pointer finalTransform = TransformType::New();
  finalTransform->SetParameters( m_FinalParameters );


  m_Resampler->SetTransform( finalTransform );
  m_Resampler->SetInput( m_MovingImageCaster->GetOutput() );

  FixedImageType::Pointer fixedImage = m_FixedImageCaster->GetOutput();
  m_Resampler->SetSize(    fixedImage->GetLargestPossibleRegion().GetSize() );
  m_Resampler->SetOutputOrigin(  fixedImage->GetOrigin() );
  m_Resampler->SetOutputSpacing( fixedImage->GetSpacing() );
  m_Resampler->SetDefaultPixelValue( 100 );

  m_Resampler->Update();
  this->GraftOutput(m_Resampler->GetOutput());

} // GenerateData


template <class TOptimizerClass, class TTransformerClass, class TMetricClass >
void
itkTransformRegistrationFilter< TOptimizerClass,  TTransformerClass, TMetricClass >
::SetTransform(const TransformType * transform)
{
  m_Transform->SetCenter( transform->GetCenter() );
  m_Transform->SetParameters( transform->GetParameters() );
}


template <class TOptimizerClass, class TTransformerClass, class TMetricClass >
void
itkTransformRegistrationFilter< TOptimizerClass,  TTransformerClass, TMetricClass >::GetTransform(typename itkTransformRegistrationFilter< TOptimizerClass,
TTransformerClass, TMetricClass >::TransformType * transform)
{
  transform->SetParameters( m_Registration->GetLastTransformParameters() );
  //transform->SetCenter( m_Transform->GetCenter() );
  //transform->SetParameters( m_Transform->GetParameters() );
  return;
}

template <class TOptimizerClass, class TTransformerClass, class TMetricClass >
void
itkTransformRegistrationFilter< TOptimizerClass,  TTransformerClass, TMetricClass >::GetCurrentTransform(typename itkTransformRegistrationFilter< TOptimizerClass,
TTransformerClass, TMetricClass >::TransformType * transform)
{
  transform->SetCenter( m_Transform->GetCenter() );
  transform->SetParameters( m_Transform->GetParameters() );
  return;
}


template <class TOptimizerClass, class TTransformerClass, class TMetricClass >
unsigned long 
itk::itkTransformRegistrationFilter<TOptimizerClass, TTransformerClass, TMetricClass >::AddIterationObserver (itk::Command *observer ) 
{
  return m_Optimizer->AddObserver( itk::IterationEvent(), observer );
  return m_Optimizer->AddObserver( itk::EndEvent(), observer );
}

template <class TOptimizerClass, class TTransformerClass, class TMetricClass >
double 
itk::itkTransformRegistrationFilter<TOptimizerClass, TTransformerClass, TMetricClass >::GetMetricValue()
{
  return m_Optimizer->GetValue();
}


} // namespace itk

#endif /* _itkTransformRegistrationFilter__txx */

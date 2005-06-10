
#ifndef __itkTransformRegistrationFilter_h
#define __itkTransformRegistrationFilter_h

#include "itkImageToImageFilter.h"


#include "itkEventObject.h"
#include "itkImage.h"
#include "itkImageRegistrationMethod.h"
#include "itkNormalizeImageFilter.h"

#include "itkCastImageFilter.h"
#include "itkImageRegistrationMethod.h"
#include "itkNormalizeImageFilter.h"
#include "itkResampleImageFilter.h"

#include "itkLinearInterpolateImageFunction.h"
#include "itkNearestNeighborInterpolateImageFunction.h"
#include "itkRecursiveMultiResolutionPyramidImageFilter.h"
#include "itkMultiResolutionImageRegistrationMethod.h"

#include "itkTimeProbesCollectorBase.h"


//BTX

namespace itk
{

typedef itk::Image<float, 3> itkRegistrationFilterImageType;

/** \class itkTransformRegistrationFilter
 * \brief Performs rigid registration at Multi-Resolution 
 *  Examples of template classes:
 *
 * TOptimizerClass = itk::VersorRigid3DTransformOptimizer 
 * TTransformerClass = itk::VersorRigid3DTransform< double >
 * TMetricClass=  typedef itk::MattesMutualInformationImageToImageMetric< 
 *                                InternalImageType, 
 *                                InternalImageType >    MutualInformationMetricType;
 *
 */
template <class TOptimizerClass, class TTransformerClass, class TMetricClass >
class ITK_EXPORT itkTransformRegistrationFilter : public itk::ImageToImageFilter<itk::itkRegistrationFilterImageType, itk::itkRegistrationFilterImageType>
{
public:
 
  /** Standard class typedefs */
  typedef itkTransformRegistrationFilter             Self;
  typedef itk::ImageToImageFilter<itkRegistrationFilterImageType,itkRegistrationFilterImageType>  Superclass;
  typedef ::itk::SmartPointer<Self>          Pointer;
  typedef ::itk::SmartPointer<const Self>    ConstPointer;

  typedef TTransformerClass   TransformType;
  typedef TOptimizerClass    OptimizerType;
  typedef TMetricClass       MetricType;

  typedef typename TransformType::ParametersType    ParametersType;

  itkStaticConstMacro( Dimension, unsigned int, 3 );

  /** DoubleArray type. */
  typedef Array<double>  DoubleArray;

  /** UnsignedIntArray type. */
  typedef Array<unsigned int> UnsignedIntArray;

  /** ShrinkFactorsArray type. */
  typedef FixedArray<unsigned int, itkGetStaticConstMacro(Dimension) > ShrinkFactorsArray;

  /** Method for creation through the object factory. */
  //itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(itkTransformRegistrationFilter, itk::ImageToImageFilter);

  void AbortIterations() {
    m_Optimizer->StopOptimization ();
  };
  
  unsigned long AddIterationObserver (itk::Command *observer );

  typedef  float         InputImagePixelType;
  typedef  float         OutputImagePixelType;

  typedef itk::Image< InputImagePixelType,    Dimension >  FixedImageType;
  typedef itk::Image< InputImagePixelType,   Dimension >  MovingImageType;

  /** Set init transfrom */
  void SetTransform( const TTransformerClass* transform );

  /** Get resulting transform */
  void GetTransform(TTransformerClass* transform);

  /** Get current transform */
  void GetCurrentTransform(TTransformerClass* transform);

  /** Get number of parameters. */
  unsigned long GetNumberOfParameters()
  { return m_Transform->GetNumberOfParameters(); }
  
  /** Get computed transform parameters. */
  //const ParametersType& GetTransformParameters()
  //{ return m_Registration->GetLastTransformParameters(); }

  /** Set the number of resolution levels. */
  itkSetClampMacro( NumberOfLevels, unsigned short, 1,
                    NumericTraits<unsigned short>::max() );
  
  /** Set the translation parameter scales. */
  itkSetClampMacro( TranslationScale, double, 0.0,
                    NumericTraits<double>::max() );
  
  /** Set the number of iterations per level. */
  itkSetMacro( NumberOfIterations, UnsignedIntArray );
  itkGetMacro( NumberOfIterations, UnsignedIntArray );

  /** Set the fixed and moving image shrink factors. */
  itkSetMacro( FixedImageShrinkFactors, ShrinkFactorsArray );
  itkSetMacro( MovingImageShrinkFactors, ShrinkFactorsArray );

  itkSetMacro(CurrentIteration, int);
  itkGetMacro(CurrentIteration, int);

  double GetMetricValue();


protected:  

  // Types
  

  typedef  float         InternalPixelType;
  typedef itk::Image< InternalPixelType, Dimension >  InternalImageType;

  /** Fixed Image Pyramid Type. */
  typedef RecursiveMultiResolutionPyramidImageFilter<
                                    FixedImageType,
                                    FixedImageType  >    FixedImagePyramidType;

  /** Moving Image Pyramid Type. */
  typedef RecursiveMultiResolutionPyramidImageFilter<
                                    MovingImageType,
                                    MovingImageType  >   MovingImagePyramidType;

  typedef itk::NormalizeImageFilter< 
                           FixedImageType,
                           InternalImageType >    FixedNormalizeFilterType;

  typedef itk::ResampleImageFilter< 
                           InternalImageType, 
                           InternalImageType >    ResampleFilterType;
  

  typedef itk::LinearInterpolateImageFunction< 
                                InternalImageType,
                                double             > LinearInterpolatorType;

  typedef itk::NearestNeighborInterpolateImageFunction< 
                                InternalImageType,
                                double             > NearestInterpolatorType;


  typedef typename OptimizerType::ScalesType       OptimizerScalesType;

  /** Registration Method. */
  typedef MultiResolutionImageRegistrationMethod< 
                                    FixedImageType, 
                                    MovingImageType >    RegistrationType;


  typedef itk::CastImageFilter< 
                        FixedImageType,
                        OutputImageType > FixedImageCasterType;

  typedef itk::CastImageFilter< 
                        MovingImageType,
                        OutputImageType > MovingImageCasterType;


  void  GenerateData ();


  // Default constructor
  itkTransformRegistrationFilter();

  virtual ~itkTransformRegistrationFilter() {};

  virtual void SetOptimizerParamters() = 0;

  virtual void SetMetricParamters() = 0;
  
  typename FixedImageCasterType::Pointer m_FixedImageCaster;
  typename MovingImageCasterType::Pointer m_MovingImageCaster;

  typename FixedImagePyramidType::Pointer     m_FixedImagePyramid;
  typename MovingImagePyramidType::Pointer    m_MovingImagePyramid;

  //typename FixedNormalizeFilterType::Pointer     m_FixedNormalizer;
  //typename MovingNormalizeFilterType::Pointer    m_MovingNormalizer;

  typename TransformType::Pointer                m_Transform;
  typename OptimizerType::Pointer                m_Optimizer;

  typename LinearInterpolatorType::Pointer       m_LinearInterpolator;
  typename NearestInterpolatorType::Pointer      m_NearestInterpolator;

  typename MetricType::Pointer                   m_Metric;

  typename RegistrationType::Pointer             m_Registration;

  typename ResampleFilterType::Pointer           m_Resampler;

  typename FixedImageType::Pointer               m_FixedImage;
  typename MovingImageType::Pointer              m_MovingImage;

  InputImagePixelType                            m_BackgroundLevel;

  //typename IterationObserverType::Pointer        m_IterationObserver;
  //typename StopObserverType::Pointer             m_StopObserver;

  //typename TimerType                             m_Timer;

  bool                                           m_ReportTimers;


  // Optimizer Stuff
  unsigned short                       m_NumberOfLevels;
  double                               m_TranslationScale;
                   
  UnsignedIntArray                     m_NumberOfIterations;
  //DoubleArray                          m_LearningRates;

  // Multi-res Stuff
  ShrinkFactorsArray                   m_MovingImageShrinkFactors;
  ShrinkFactorsArray                   m_FixedImageShrinkFactors;
                   
  // Transform-stuff
  ParametersType                       m_InitialParameters;
  ParametersType                       m_FinalParameters;

  int                                  m_CurrentIteration;

private:
  itkTransformRegistrationFilter(const itkTransformRegistrationFilter&);  // Not implemented.
  void operator=(const itkTransformRegistrationFilter&);  // Not implemented.
};

} // namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkTransformRegistrationFilter.txx"
#endif

 //ETX

#endif


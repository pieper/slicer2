
#ifndef __itkVersorMattesMiVersorRegistrationFilter_h
#define __itkVersorMattesMiVersorRegistrationFilter_h

//BTX


#include "itkTransformRegistrationFilter.h"

#include "itkMattesMutualInformationImageToImageMetric.h"
#include "itkVersorRigid3DTransform.h"
#include "itkVersorRigid3DTransformOptimizer.h"



namespace itk {

typedef  itk::VersorRigid3DTransformOptimizer OptimizerType;
typedef  itk::VersorRigid3DTransform< double > TransformType;
typedef  itk::MattesMutualInformationImageToImageMetric< 
                                 itk::itkRegistrationFilterImageType,
                                 itk::itkRegistrationFilterImageType >    MetricType;

class ITK_EXPORT itkVersorMattesMiVersorRegistrationFilter : public itk::itkTransformRegistrationFilter<OptimizerType, TransformType, MetricType>
{
public:
  typedef itkVersorMattesMiVersorRegistrationFilter             Self;
  typedef itk::itkTransformRegistrationFilter<OptimizerType, TransformType, MetricType>  Superclass;
  typedef ::itk::SmartPointer<Self>          Pointer;
  typedef ::itk::SmartPointer<const Self>    ConstPointer;


  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(itkVersorMattesMiVersorRegistrationFilter, itk::itkTransformRegistrationFilter);

  itkSetMacro(MinimumStepLength, double);
  itkGetMacro(MinimumStepLength, double);

  itkGetMacro(MaximumStepLength, double);
  itkSetMacro(MaximumStepLength, double);

  itkGetMacro(NumberOfHistogramBins, int);
  itkSetMacro(NumberOfHistogramBins, int);

  itkGetMacro(NumberOfSpatialSamples, int);
  itkSetMacro(NumberOfSpatialSamples, int);

  itkGetMacro(ReinitializeSeed, int);
  itkSetMacro(ReinitializeSeed, int);

protected:  
  virtual void SetOptimizerParamters();
  
  virtual void SetMetricParamters();

  double m_MinimumStepLength;
  double m_MaximumStepLength;

  int m_NumberOfHistogramBins;

  int m_NumberOfSpatialSamples;

  int m_ReinitializeSeed;

  // Default constructor
  itkVersorMattesMiVersorRegistrationFilter();

private:
  itkVersorMattesMiVersorRegistrationFilter(const itkVersorMattesMiVersorRegistrationFilter&);  // Not implemented.
  void operator=(const itkVersorMattesMiVersorRegistrationFilter&);  // Not implemented.
};

} // namespace itk

//ETX

#endif


#ifndef __itkVersorMattesMiVersorRegistrationFilter_h
#define __itkVersorMattesMiVersorRegistrationFilter_h

//BTX
#include <fstream>
#include <string>

#include "itkTransformRegistrationFilter.h"

#include "itkMattesMutualInformationImageToImageMetric.h"
#include "itkVersorRigid3DTransform.h"
#include "itkVersorRigid3DTransformOptimizer.h"

#include "itkArray.h"

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

  itkSetMacro(MinimumStepLength, DoubleArray);
  itkGetMacro(MinimumStepLength, DoubleArray);

  itkGetMacro(MaximumStepLength, DoubleArray);
  itkSetMacro(MaximumStepLength, DoubleArray);

  itkGetMacro(NumberOfHistogramBins, int);
  itkSetMacro(NumberOfHistogramBins, int);

  itkGetMacro(NumberOfSpatialSamples, int);
  itkSetMacro(NumberOfSpatialSamples, int);

  itkGetMacro(ReinitializeSeed, int);
  itkSetMacro(ReinitializeSeed, int);
  
  int GetCurrentLevel() { return m_Registration->GetCurrentLevel();};

protected:  
  virtual void SetOptimizerParamters();
  
  virtual void SetMetricParamters();

  DoubleArray m_MinimumStepLength;
  DoubleArray m_MaximumStepLength;

  int m_NumberOfHistogramBins;

  int m_NumberOfSpatialSamples;

  int m_ReinitializeSeed;

  // Default constructor
  itkVersorMattesMiVersorRegistrationFilter();

private:
  itkVersorMattesMiVersorRegistrationFilter(const itkVersorMattesMiVersorRegistrationFilter&);  // Not implemented.
  void operator=(const itkVersorMattesMiVersorRegistrationFilter&);  // Not implemented.
};

///////////////////////////////////////////////////////////////////
//
//  The following section of code implements a Command observer
//  that will monitor the evolution of the registration process.
//

//BTX
class itkVersorMattesMiVersorRegistrationCommand :  public itk::Command 
{
public:
  typedef  itkVersorMattesMiVersorRegistrationCommand   Self;
  typedef  itk::Command             Superclass;
  typedef  itk::SmartPointer<itkVersorMattesMiVersorRegistrationCommand>  Pointer;
  itkNewMacro( itkVersorMattesMiVersorRegistrationCommand );
  
  void SetRegistrationFilter (itkVersorMattesMiVersorRegistrationFilter *registration) {
    m_registration = registration;
  }
  void SetLogFileName(char *filename) {
    m_fo.open(filename);
  }
  
protected:
  itkVersorMattesMiVersorRegistrationCommand() : m_fo("C:\\Tmp\\regLevel.log"){};
  itkVersorMattesMiVersorRegistrationFilter  *m_registration;
  std::ofstream m_fo;
  
  typedef itk::VersorRigid3DTransformOptimizer     OptimizerType;
  typedef   OptimizerType   *    OptimizerPointer;
  
public:
  
  virtual void Execute(const itk::Object *caller, const itk::EventObject & event)
  {
    Execute( ( itk::Object *)caller, event);
  }
  
  virtual void Execute( itk::Object * object, const itk::EventObject & event)
  {
    
    OptimizerPointer optimizer = 
      dynamic_cast< OptimizerPointer >( object );
    
    if( ! itk::IterationEvent().CheckEvent( &event ) ) {
      return;
    }
    unsigned int level = m_registration->GetCurrentLevel();
    int numIter = m_registration->GetNumberOfIterations().GetElement(level);
    double maxStep  = m_registration->GetMaximumStepLength().GetElement(level); 
    double minStep =  m_registration->GetMinimumStepLength().GetElement(level);
    optimizer->SetNumberOfIterations( numIter );
    optimizer->SetMaximumStepLength( maxStep);
    optimizer->SetMinimumStepLength( minStep);
    if (m_fo.good()) {
      m_fo << "LEVEL = " << level << "  ITERATION =" << optimizer->GetCurrentIteration() << 
        " MaxStep=" << maxStep << " MinStep=" << minStep <<  "  Value=" << optimizer->GetValue() << std::endl;
      m_fo.flush();
    }
  }
};

} // namespace itk

//ETX

#endif

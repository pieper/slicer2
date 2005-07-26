
#ifndef __itkAffineMattesMIRegistrationFilter_h
#define __itkAffineMattesMIRegistrationFilter_h

//BTX
#include <fstream>
#include <string>

#include "itkTransformRegistrationFilter.h"

#include "itkMattesMutualInformationImageToImageMetric.h"
#include "itkAffineTransform.h"
#include "itkRegularStepGradientDescentOptimizer.h" 

#include "itkArray.h"


typedef  itk::Image<float, 3> ImageType;
typedef  itk::RegularStepGradientDescentOptimizer OptimizerType;
typedef  itk::AffineTransform< double > TransformType;
typedef  itk::MattesMutualInformationImageToImageMetric< 
                                 ImageType,
                                 ImageType> MetricType;
namespace itk {
class ITK_EXPORT itkAffineMattesMIRegistrationFilter : public itk::itkTransformRegistrationFilter<ImageType, OptimizerType, TransformType, MetricType>
{
public:
  typedef itkAffineMattesMIRegistrationFilter             Self;
  typedef itk::itkTransformRegistrationFilter<ImageType, OptimizerType, TransformType, MetricType>  Superclass;
  typedef ::itk::SmartPointer<Self>          Pointer;
  typedef ::itk::SmartPointer<const Self>    ConstPointer;


  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(itkAffineMattesMIRegistrationFilter, itk::itkTransformRegistrationFilter);

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
  itkAffineMattesMIRegistrationFilter();

private:
  itkAffineMattesMIRegistrationFilter(const itkAffineMattesMIRegistrationFilter&);  // Not implemented.
  void operator=(const itkAffineMattesMIRegistrationFilter&);  // Not implemented.
};

///////////////////////////////////////////////////////////////////
//
//  The following section of code implements a Command observer
//  that will monitor the evolution of the registration process.
//

//BTX
class itkAffineMattesMIRegistrationCommand :  public itk::Command 
{
public:
  typedef  itkAffineMattesMIRegistrationCommand   Self;
  typedef  itk::Command             Superclass;
  typedef  itk::SmartPointer<itkAffineMattesMIRegistrationCommand>  Pointer;
  itkNewMacro( itkAffineMattesMIRegistrationCommand );
  
  void SetRegistrationFilter (itkAffineMattesMIRegistrationFilter *registration) {
    m_registration = registration;
  }
  void SetLogFileName(char *filename) {
    m_fo.open(filename);
  }
  
protected:
  itkAffineMattesMIRegistrationCommand() : m_fo("regLevel.log"), m_level(0){};
  itkAffineMattesMIRegistrationFilter  *m_registration;
  std::ofstream m_fo;
  int m_level;
  
  typedef itk::RegularStepGradientDescentOptimizer     OptimizerType;
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

    if( typeid( event ) == typeid( itk::EndEvent ) ) {
      OptimizerType::StopConditionType stopCondition = optimizer->GetStopCondition();
      if (m_fo.good()) {
        m_fo << "Optimizer stopped : " << std::endl;
        m_fo << "Stop condition   =  " << stopCondition << std::endl;
        switch(stopCondition) {
        case OptimizerType::GradientMagnitudeTolerance:
          m_fo << "GradientMagnitudeTolerance" << std::endl; 
          break;
        case OptimizerType::StepTooSmall:
          m_fo << "StepTooSmall" << std::endl; 
          break;
        case OptimizerType::ImageNotAvailable:
          m_fo << "ImageNotAvailable" << std::endl; 
          break;
        case OptimizerType::SamplesNotAvailable:
          m_fo << "SamplesNotAvailable" << std::endl; 
          break;
        case OptimizerType::MaximumNumberOfIterations:
          m_fo << "MaximumNumberOfIterations" << std::endl; 
          break;
        }
        m_fo.flush();
      }
    }
    
    if( ! itk::IterationEvent().CheckEvent( &event ) ) {
      return;
    }
    unsigned int level = m_registration->GetCurrentLevel();
    if (level != m_level && m_registration->GetMaximumStepLength().GetNumberOfElements() > m_level + 1) {
      m_level++;
      optimizer->StopOptimization();
      double maxStep = m_registration->GetMaximumStepLength().GetElement(m_level); 
      optimizer->SetMaximumStepLength( maxStep );
      optimizer->StartOptimization(); 
      m_fo << "RESTART OPTIMIZATION FOR LEVEL= " <<  m_level <<
        " WITH maxStep = " << maxStep << std::endl; 
    }
    int numIter = m_registration->GetNumberOfIterations().GetElement(level);
    double maxStep  = m_registration->GetMaximumStepLength().GetElement(level); 
    double minStep =  m_registration->GetMinimumStepLength().GetElement(level);
    optimizer->SetNumberOfIterations( numIter );
    optimizer->SetMaximumStepLength( maxStep);
    optimizer->SetMinimumStepLength( minStep);
    if (m_fo.good()) {
      m_fo << "LEVEL = " << level << "  ITERATION =" << optimizer->GetCurrentIteration() << 
        " MaxStep=" << maxStep << " MinStep=" << minStep <<  
        " Step=" << optimizer->GetCurrentStepLength() <<
        "  Value=" << optimizer->GetValue() << std::endl;
      m_fo.flush();
    }
  }
};

} // namespace itk

//ETX

#endif

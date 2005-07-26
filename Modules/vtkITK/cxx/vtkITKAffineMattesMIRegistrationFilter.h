// .NAME vtkITKImageToImageFilter - Abstract base class for connecting ITK and VTK
// .SECTION Description
// vtkITKImageToImageFilter provides a 

#ifndef __vtkITKAffineMattesMIRegistrationFilter_h
#define __vtkITKAffineMattesMIRegistrationFilter_h

#include <fstream>
#include <string>

#include "vtkITKTransformRegistrationFilter.h"
#include "itkAffineMattesMIRegistrationFilter.h"


// vtkITKAffineMattesMIRegistrationFilter Class

class VTK_EXPORT vtkITKAffineMattesMIRegistrationFilter : public vtkITKTransformRegistrationFilter
{
public:
  vtkTypeMacro(vtkITKAffineMattesMIRegistrationFilter,vtkITKTransformRegistrationFilter);

  static vtkITKAffineMattesMIRegistrationFilter* New();

  void PrintSelf(ostream& os, vtkIndent indent)
  {
    Superclass::PrintSelf ( os, indent );
  };

  vtkSetMacro(TranslateScale, double);
  vtkGetMacro(TranslateScale, double);

  // Description
  // The Max Number of Iterations at each multi-resolution level.
  vtkSetObjectMacro(MaxNumberOfIterations,vtkUnsignedIntArray);
  vtkGetObjectMacro(MaxNumberOfIterations,vtkUnsignedIntArray);


  vtkSetObjectMacro(MinimumStepLength, vtkDoubleArray);
  vtkGetObjectMacro(MinimumStepLength, vtkDoubleArray);

  vtkSetObjectMacro(MaximumStepLength, vtkDoubleArray);
  vtkGetObjectMacro(MaximumStepLength, vtkDoubleArray);

  // Description:
  // Set the number of sample points for density estimation
  vtkSetMacro(NumberOfSamples, int);
  vtkGetMacro(NumberOfSamples, int);

  // Description:
  // Set the number of bins for density estimation
  vtkSetMacro(NumberOfHistogramBins, int);
  vtkGetMacro(NumberOfHistogramBins, int);


  // Description:
  // Reset the Multiresolution Settings
  // It blanks the Min/Max Step and NumberOfIterations
  void ResetMultiResolutionSettings()
  { MinimumStepLength->Reset(); 
    MaxNumberOfIterations->Reset();
    MaximumStepLength->Reset(); };
  
  // Description:
  // Set the min step for the algorithm.
  void SetNextMinimumStepLength(const double step)
  { MinimumStepLength->InsertNextValue(step); };

  // Description:
  // Set the max step for the algorithm.
  void SetNextMaximumStepLength(const double step)
  { MaximumStepLength->InsertNextValue(step); };

  // Description:
  // Set the max number of iterations at each level
  // Generally less than 5000, 2500 is OK.
  // Must set the same number of Learning Rates as Iterations
  void SetNextMaxNumberOfIterations(const int num)
  { MaxNumberOfIterations->InsertNextValue(num); };


  void SetSourceShrinkFactors(unsigned int i,
                              unsigned int j, 
                              unsigned int k);

  void SetTargetShrinkFactors(unsigned int i,
                              unsigned int j, 
                              unsigned int k);

  unsigned int GetSourceShrinkFactors(const int &dir)
  { return SourceShrink[dir]; }

  unsigned int GetTargetShrinkFactors(const int &dir)
  { return TargetShrink[dir]; }
  
  virtual void GetTransformationMatrix(vtkMatrix4x4* matrix);
  
  virtual void GetCurrentTransformationMatrix(vtkMatrix4x4* matrix);
  
  virtual void SetTransformationMatrix(vtkMatrix4x4 *matrix);

  virtual void AbortIterations() {
    m_ITKFilter->AbortIterations();
  };


  void SetAbort(int abort) {
    m_ITKFilter->AbortIterations();
  }

  virtual double GetMetricValue() {
    return  m_ITKFilter->GetMetricValue();
  }
  void ReSeedSamples() {
    m_ITKFilter->SetReinitializeSeed(8775070);
  }

  int GetCurrentLevel() { return m_ITKFilter->GetCurrentLevel();};

protected:

  double TranslateScale;

  vtkUnsignedIntArray  *MaxNumberOfIterations;

  vtkDoubleArray       *MinimumStepLength;

  vtkDoubleArray       *MaximumStepLength;

  int NumberOfHistogramBins;

  int NumberOfSamples;

  unsigned int SourceShrink[3];
  unsigned int TargetShrink[3];
  //BTX

  itk::itkAffineMattesMIRegistrationFilter::Pointer m_ITKFilter;

  virtual void UpdateRegistrationParameters();

  virtual void CreateRegistrationPipeline();

  virtual vtkITKRegistrationFilter::OutputImageType::Pointer GetTransformedOutput();

  // default constructor
  vtkITKAffineMattesMIRegistrationFilter (); // This is called from New() by vtkStandardNewMacro

  virtual ~vtkITKAffineMattesMIRegistrationFilter() {};
  //ETX
  
private:
  vtkITKAffineMattesMIRegistrationFilter(const vtkITKAffineMattesMIRegistrationFilter&);  // Not implemented.
  void operator=(const vtkITKAffineMattesMIRegistrationFilter&);  // Not implemented.
};

//vtkCxxRevisionMacro(vtkITKAffineMattesMIRegistrationFilter, "$Revision: 1.2 $");
//vtkStandardNewMacro(vtkITKAffineMattesMIRegistrationFilter);
vtkRegistrationNewMacro(vtkITKAffineMattesMIRegistrationFilter);





///////////////////////////////////////////////////////////////////
//
//  The following section of code implements a Command observer
//  that will monitor the evolution of the registration process.
//

//BTX
class vtkITKAffineMattesMIRegistrationCommand :  public itk::Command 
{
public:
  typedef  vtkITKAffineMattesMIRegistrationCommand   Self;
  typedef  itk::Command             Superclass;
  typedef  itk::SmartPointer<vtkITKAffineMattesMIRegistrationCommand>  Pointer;
  itkNewMacro( vtkITKAffineMattesMIRegistrationCommand );

  void SetRegistrationFilter (vtkITKAffineMattesMIRegistrationFilter *registration) {
    m_registration = registration;
  }
  void SetLogFileName(char *filename) {
    m_fo.open(filename);
  }

protected:
  vtkITKAffineMattesMIRegistrationCommand() : m_fo("reg.log"){};
  vtkITKAffineMattesMIRegistrationFilter  *m_registration;
  std::ofstream m_fo;

  typedef itk::RegularStepGradientDescentOptimizer     OptimizerType;
  typedef OptimizerType   *    OptimizerPointer;

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
    int iter = m_registration->GetCurrentIteration();
    unsigned int level = m_registration->GetCurrentLevel();

    /*
    int numIter = m_registration->GetMaxNumberOfIterations()->GetValue(level);
    double maxStep  = m_registration->GetMaximumStepLength()->GetValue(level); 
    double minStep =  m_registration->GetMinimumStepLength()->GetValue(level);
    optimizer->SetNumberOfIterations( numIter );
    optimizer->SetMaximumStepLength( maxStep);
    optimizer->SetMinimumStepLength( minStep);
    */
    
    vtkMatrix4x4 *mat = vtkMatrix4x4::New();
    m_registration->GetCurrentTransformationMatrix(mat);

    //if (m_fo.good()) {
      m_fo << "  ITERATION =" << optimizer->GetCurrentIteration() << "   " << std::endl;
      mat->Print(m_fo);
      m_fo << "Value=" << optimizer->GetValue() << "   ";
      m_fo << "Position=" << optimizer->GetCurrentPosition() << std::endl;
      m_fo.flush();
   // }
    m_registration->InvokeEvent(vtkCommand::ProgressEvent);
    
    m_registration->SetCurrentIteration(iter+1);

    m_registration->UpdateProgress((float)iter/m_registration->GetNumIterations() );

    if (m_registration->GetAbortExecute()) {
      m_registration->AbortIterations();
    }
  }
};
//ETX
#endif





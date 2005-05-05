// .NAME vtkITKImageToImageFilter - Abstract base class for connecting ITK and VTK
// .SECTION Description
// vtkITKImageToImageFilter provides a 

#ifndef __vtkITKDemonsRegistrationFilter_h
#define __vtkITKDemonsRegistrationFilter_h


#include "vtkITKDeformableRegistrationFilter.h"

#include "itkImageRegionIterator.h"
#include "itkDemonsRegistrationFilter.h"
#include "itkHistogramMatchingImageFilter.h"


// vtkITKDemonsRegistrationFilter Class

class VTK_EXPORT vtkITKDemonsRegistrationFilter : public vtkITKDeformableRegistrationFilter
{
public:
  vtkTypeMacro(vtkITKDemonsRegistrationFilter,vtkITKImageToImageFilter);

  static vtkITKDemonsRegistrationFilter* New();

  void PrintSelf(ostream& os, vtkIndent indent)
  {
    Superclass::PrintSelf ( os, indent );
  };

  vtkSetMacro(StandardDeviations, double);
  vtkGetMacro(StandardDeviations, double);

  virtual void AbortIterations() {
    m_Filter->SetAbortGenerateData(true);
  };

protected:
  double StandardDeviations;

  //BTX
  typedef OutputImageType::Pointer OutputImagePointerType;

  ////////////////////////////////
  // ITK Pipeline that does the job
  ////////////////////////////////

  // Matcher
  typedef itk::HistogramMatchingImageFilter<InputImageType, InputImageType> MatchingFilterType;
  MatchingFilterType::Pointer m_Matcher;

  // Registration filter
  typedef itk::DemonsRegistrationFilter<
                                InputImageType,
                                InputImageType,
                                DeformationFieldType>   RegistrationFilterType;
  RegistrationFilterType::Pointer m_Filter;

  // Warper and interpolator
  typedef itk::WarpImageFilter<
                          InputImageType, 
                          InputImageType,
                          DeformationFieldType  >     WarperType;
  typedef itk::LinearInterpolateImageFunction<
                                   InputImageType,
                                   double          >  InterpolatorType;

  WarperType::Pointer m_Warper;
  InterpolatorType::Pointer m_Interpolator;

  virtual vtkITKDeformableRegistrationFilter::DeformationFieldType::Pointer GetDisplacementOutput();

  virtual vtkITKRegistrationFilter::OutputImageType::Pointer GetTransformedOutput();

  virtual void UpdateRegistrationParameters();

  virtual void CreateRegistrationPipeline();

  // default constructor
  vtkITKDemonsRegistrationFilter (); // This is called from New() by vtkStandardNewMacro

  virtual ~vtkITKDemonsRegistrationFilter() {};
  //ETX
  
private:
  vtkITKDemonsRegistrationFilter(const vtkITKDemonsRegistrationFilter&);  // Not implemented.
  void operator=(const vtkITKDemonsRegistrationFilter&);  // Not implemented.
};

//vtkCxxRevisionMacro(vtkITKDemonsRegistrationFilter, "$Revision: 1.2 $");
//vtkStandardNewMacro(vtkITKDemonsRegistrationFilter);
vtkRegistrationNewMacro(vtkITKDemonsRegistrationFilter);





///////////////////////////////////////////////////////////////////
//
//  The following section of code implements a Command observer
//  that will monitor the evolution of the registration process.
//

//BTX
class DemonsRegistrationFilterCommand : public itk::Command 
{
public:
  typedef  DemonsRegistrationFilterCommand   Self;
  typedef  itk::Command             Superclass;
  typedef  itk::SmartPointer<DemonsRegistrationFilterCommand>  Pointer;
  itkNewMacro( DemonsRegistrationFilterCommand );

  void SetDemonsRegistrationFilter (vtkITKDemonsRegistrationFilter *registration) {
    m_registration = registration;
  }

protected:
  DemonsRegistrationFilterCommand() {};
  vtkITKDemonsRegistrationFilter *m_registration;

  typedef itk::Image< float, 3 > InternalImageType;
  typedef itk::Vector< float, 3 >    VectorPixelType;
  typedef itk::Image<  VectorPixelType, 3 > DeformationFieldType;
  
  typedef itk::DemonsRegistrationFilter<
    InternalImageType,
    InternalImageType,
    DeformationFieldType>   RegistrationFilterType;
  
public:
  
  void Execute(itk::Object *caller, const itk::EventObject & event)
  {
    Execute( (const itk::Object *)caller, event);
  }
  
  void Execute(const itk::Object * object, const itk::EventObject & event)
  {
    const RegistrationFilterType * filter = 
      dynamic_cast< const RegistrationFilterType * >( object );
    if( typeid( event ) != typeid( itk::IterationEvent ) ) {
      return;
    }
    if (filter) {
      int iter = m_registration->GetCurrentIteration();
      std::cout << filter->GetMetric() << std::endl;
      std::cout << "Iteration " << iter << std::endl;
      m_registration->SetCurrentIteration(iter+1);
      if (m_registration->GetAbortExecute()) {
        m_registration->AbortIterations();
      }
    }
    else {
      std::cout << "Error in DemonsRegistrationFilterCommand::Execute" << std::endl;
    }
  }
};
//ETX
#endif





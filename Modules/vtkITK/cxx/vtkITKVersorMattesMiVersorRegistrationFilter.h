// .NAME vtkITKImageToImageFilter - Abstract base class for connecting ITK and VTK
// .SECTION Description
// vtkITKImageToImageFilter provides a 

#ifndef __vtkITKVersorMattesMiVersorRegistrationFilter_h
#define __vtkITKVersorMattesMiVersorRegistrationFilter_h


#include "vtkITKTransformRegistrationFilter.h"
#include "itkVersorMattesMiVersorRegistrationFilter.h"


// vtkITKVersorMattesMiVersorRegistrationFilter Class

class VTK_EXPORT vtkITKVersorMattesMiVersorRegistrationFilter : public vtkITKTransformRegistrationFilter
{
public:
  vtkTypeMacro(vtkITKVersorMattesMiVersorRegistrationFilter,vtkITKTransformRegistrationFilter);

  static vtkITKVersorMattesMiVersorRegistrationFilter* New();

  void PrintSelf(ostream& os, vtkIndent indent)
  {
    Superclass::PrintSelf ( os, indent );
  };

  vtkSetMacro(NumberOfIterations, int);
  vtkGetMacro(NumberOfIterations, int);

  virtual void GetTransformationMatrix(vtkMatrix4x4* matrix);
  
  virtual void SetTransformationMatrix(vtkMatrix4x4 *matrix);

  virtual void AbortIterations() {
    m_ITKFilter->AbortIterations();
  };

protected:
  int NumberOfIterations;

  //BTX

  itk::itkVersorMattesMiVersorRegistrationFilter::Pointer m_ITKFilter;

  virtual void UpdateRegistrationParameters();

  virtual void CreateRegistrationPipeline();

  virtual vtkITKRegistrationFilter::OutputImageType::Pointer GetTransformedOutput();

  // default constructor
  vtkITKVersorMattesMiVersorRegistrationFilter (); // This is called from New() by vtkStandardNewMacro

  virtual ~vtkITKVersorMattesMiVersorRegistrationFilter() {};
  //ETX
  
private:
  vtkITKVersorMattesMiVersorRegistrationFilter(const vtkITKVersorMattesMiVersorRegistrationFilter&);  // Not implemented.
  void operator=(const vtkITKVersorMattesMiVersorRegistrationFilter&);  // Not implemented.
};

//vtkCxxRevisionMacro(vtkITKVersorMattesMiVersorRegistrationFilter, "$Revision: 1.1 $");
//vtkStandardNewMacro(vtkITKVersorMattesMiVersorRegistrationFilter);
vtkRegistrationNewMacro(vtkITKVersorMattesMiVersorRegistrationFilter);





///////////////////////////////////////////////////////////////////
//
//  The following section of code implements a Command observer
//  that will monitor the evolution of the registration process.
//

//BTX
class VersorMattesMiVersorRegistrationCommand : public itk::Command 
{
public:
  typedef  VersorMattesMiVersorRegistrationCommand   Self;
  typedef  itk::Command             Superclass;
  typedef  itk::SmartPointer<VersorMattesMiVersorRegistrationCommand>  Pointer;
  itkNewMacro( VersorMattesMiVersorRegistrationCommand );

  void SetRegistrationFilter (vtkITKVersorMattesMiVersorRegistrationFilter *registration) {
    m_registration = registration;
  }

protected:
  VersorMattesMiVersorRegistrationCommand() {};
  vtkITKVersorMattesMiVersorRegistrationFilter  *m_registration;

  typedef itk::VersorRigid3DTransformOptimizer     OptimizerType;
  typedef   const OptimizerType   *    OptimizerPointer;

public:
  
  void Execute(itk::Object *caller, const itk::EventObject & event)
  {
    Execute( (const itk::Object *)caller, event);
  }
  
  void Execute(const itk::Object * object, const itk::EventObject & event)
  {
    OptimizerPointer optimizer = 
      dynamic_cast< OptimizerPointer >( object );
    if( ! itk::IterationEvent().CheckEvent( &event ) ) {
      return;
    }
    std::cout << optimizer->GetCurrentIteration() << "   ";
    std::cout << optimizer->GetValue() << "   ";
    std::cout << optimizer->GetCurrentPosition() << std::endl;
    int iter = m_registration->GetCurrentIteration();
    m_registration->SetCurrentIteration(iter+1);
    if (m_registration->GetAbortExecute()) {
      m_registration->AbortIterations();
    }
    else {
      std::cout << "Error in VersorMattesMiVersorRegistrationCommand::Execute" << std::endl;
    }
  }
};
//ETX
#endif





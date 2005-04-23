// .NAME vtkITKImageToImageFilter - Abstract base class for connecting ITK and VTK
// .SECTION Description
// vtkITKImageToImageFilter provides a 

#ifndef __vtkITKDemonsRegistrationFilter_h
#define __vtkITKDemonsRegistrationFilter_h


#include "vtkITKImageToImageFilter.h"
#include "vtkImageToImageFilter.h"
#include "itkImageToImageFilter.h"
#include "itkVTKImageExport.h"
#include "itkVTKImageImport.h"
#include "vtkITKUtility.h"

#include "itkImageRegionIterator.h"
#include "itkDemonsRegistrationFilter.h"
#include "itkHistogramMatchingImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkWarpImageFilter.h"
#include "itkLinearInterpolateImageFunction.h"


// vtkITKDemonsRegistrationFilter Class

class VTK_EXPORT vtkITKDemonsRegistrationFilter : public vtkITKImageToImageFilter
{
public:
  vtkTypeMacro(vtkITKDemonsRegistrationFilter,vtkITKImageToImageFilter);
  static vtkITKDemonsRegistrationFilter* New();
  void PrintSelf(ostream& os, vtkIndent indent)
  {
    Superclass::PrintSelf ( os, indent );
  };

  vtkSetMacro(NumIterations, int);
  vtkGetMacro(NumIterations, int);

  vtkSetMacro(StandardDeviations, double);
  vtkGetMacro(StandardDeviations, double);

  vtkSetMacro(CurrentIteration, int);
  vtkGetMacro(CurrentIteration, int);

  void Update();

  // Description:
  // Set the Input, 0-fixed image, 1-moving image
  virtual void SetInput(vtkImageData *Input, int idx)
  {
    if (idx == 0) {
      this->vtkCast->SetInput(Input);
    }
    else if (idx == 1) {
      this->vtkCastMoving->SetInput(Input);
    }
  };

  // Description:
  // Set Moving Input
  void SetMovingInput(vtkImageData *Input)
  {
    this->vtkCastMoving->SetInput(Input);
  };

  // Description:
  // Get the Output, 0-transformed image, 1-dispacement image
  virtual vtkImageData *GetOutput(int idx)
  {
    if (idx == 0) {
      return (vtkImageData *) this->vtkImporter->GetOutput();
    }
    else if (idx == 1) {
      return (vtkImageData *) this->vtkImporterDisplacement->GetOutput();
    }
  };
  
protected:
  int    NumIterations;
  double StandardDeviations;
  int    CurrentIteration;
  //BTX
  
  // To/from ITK
  typedef float InputImagePixelType;
  typedef float OutputImagePixelType;
  typedef itk::Vector<float, 3>    VectorPixelType;

  typedef itk::Image<InputImagePixelType, 3> InputImageType;
  typedef itk::Image<OutputImagePixelType, 3> OutputImageType;
  typedef itk::Image<VectorPixelType, 3> DeformationFieldType;

  typedef itk::VTKImageImport<InputImageType> ImageImportType;
  typedef itk::VTKImageExport<OutputImageType> ImageExportType;
  typedef itk::VTKImageExport<DeformationFieldType> DeformationExportType;

  // itk import for input itk images
  ImageImportType::Pointer itkImporterFixed;
  ImageImportType::Pointer itkImporterMoving;

  // itk export for output itk images
  ImageExportType::Pointer itkExporterTransformed;
  DeformationExportType::Pointer itkExporterDisplacement;

  // use superclass vtkCast for fixed image

  // vtk export for moving vtk image
  vtkImageCast* vtkCastMoving;
  vtkImageExport* vtkExporterMoving;  

  // use supercalass vtkImporter for Transformed image 

  // vtk import for output vtk displacement image
  vtkImageImport *vtkImporterDisplacement;

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


  // default constructor
  vtkITKDemonsRegistrationFilter (); // This is called from New() by vtkStandardNewMacro

  virtual ~vtkITKDemonsRegistrationFilter();
  //ETX
  
private:
  vtkITKDemonsRegistrationFilter(const vtkITKDemonsRegistrationFilter&);  // Not implemented.
  void operator=(const vtkITKDemonsRegistrationFilter&);  // Not implemented.
};

//vtkCxxRevisionMacro(vtkITKDemonsRegistrationFilter, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkITKDemonsRegistrationFilter);





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
    }
    else {
      std::cout << "Error in DemonsRegistrationFilterCommand::Execute" << std::endl;
    }
  }
};
//ETX
#endif





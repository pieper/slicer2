// .NAME vtkITKImageToImageFilter - Abstract base class for connecting ITK and VTK
// .SECTION Description
// vtkITKImageToImageFilter provides a 

#ifndef __vtkITKRegistrationFilter_h
#define __vtkITKRegistrationFilter_h


#include "vtkITKImageToImageFilter.h"
#include "vtkImageToImageFilter.h"
#include "itkImageToImageFilter.h"
#include "itkVTKImageExport.h"
#include "itkVTKImageImport.h"
#include "vtkITKUtility.h"

#include "itkImageRegionIterator.h"
#include "itkCastImageFilter.h"


#define vtkRegistrationNewMacro(thisClass) \
  thisClass* thisClass::New() \
  { \
    vtkObject* ret = vtkObjectFactory::CreateInstance(#thisClass); \
    if(ret) \
      { \
          thisClass* c = static_cast<thisClass*>(ret); \
          c->InitializePipeline(); \
      return (c); \
      } \
    thisClass* c = new thisClass; \
    c->InitializePipeline(); \
    return c; \
  } \
  vtkInstantiatorNewMacro(thisClass)

// vtkITKRegistrationFilter Class

class VTK_EXPORT vtkITKRegistrationFilter : public vtkITKImageToImageFilter
{
public:
  vtkTypeMacro(vtkITKRegistrationFilter,vtkITKImageToImageFilter);

  static vtkITKRegistrationFilter* New(){return 0;};

  void PrintSelf(ostream& os, vtkIndent indent)
  {
    Superclass::PrintSelf ( os, indent );
  };

  vtkSetMacro(NumIterations, int);
  vtkGetMacro(NumIterations, int);

  void SetCurrentIteration (int iter) {
    CurrentIteration = iter;
  };
  int GetCurrentIteration() {
    return CurrentIteration;
  };

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
    else {
      // report error
    }
  };
  virtual vtkImageData* GetInput(int idx)
  {
    if (idx == 0) {
      return this->vtkCast->GetInput();
    }
    else if (idx == 1) {
      return this->vtkCastMoving->GetInput();
    }
    else {
      // report error
    }
    return NULL;
  };

  // Description:
  // Set Fixed Input
  void SetFixedInput(vtkImageData *Input)
  {
    this->SetInput(Input, 0);
  };

  virtual vtkImageData* GetFixedInput()
  {
    return this->GetInput(0);
  }

  // Description:
  // Set Moving Input
  void SetMovingInput(vtkImageData *Input)
  {
    this->SetInput(Input, 1);
  };

  virtual vtkImageData* GetMovingInput()
  {
    return this->GetInput(1);
  }


protected:
  int    NumIterations;
  int    CurrentIteration;
  //BTX
  
  // To/from ITK

  typedef float InputImagePixelType;
  typedef float OutputImagePixelType;
  typedef float InternalPixelType;

  typedef itk::Image<InputImagePixelType, 3> InputImageType;
  typedef itk::Image<OutputImagePixelType, 3> OutputImageType;
  typedef itk::Image<InternalPixelType, 3 >  InternalImageType;

  typedef itk::VTKImageImport<InputImageType> ImageImportType;
  typedef itk::VTKImageExport<OutputImageType> ImageExportType;

  // itk import for input itk images
  ImageImportType::Pointer itkImporterFixed;
  ImageImportType::Pointer itkImporterMoving;

  // itk export for output itk images
  ImageExportType::Pointer itkExporterTransformed;

  // vtk export for moving vtk image
  vtkImageCast* vtkCastMoving;
  vtkImageExport* vtkExporterMoving;  

  void InitializePipeline();

  virtual OutputImageType::Pointer GetTransformedOutput() = 0;

  virtual void UpdateRegistrationParameters() = 0;

  virtual void ConnectInputPipelines();

  virtual void CreateRegistrationPipeline(){};

  virtual void ConnectOutputPipelines();

  virtual void AbortIterations() = 0;

  // default constructor
  vtkITKRegistrationFilter (){}; 

  virtual ~vtkITKRegistrationFilter();
  //ETX
  
private:
  vtkITKRegistrationFilter(const vtkITKRegistrationFilter&);  // Not implemented.
  void operator=(const vtkITKRegistrationFilter&);  // Not implemented.
};

//vtkCxxRevisionMacro(vtkITKRegistrationFilter, "$Revision: 1.3 $");
//vtkStandardNewMacro(vtkITKRegistrationFilter);

#endif





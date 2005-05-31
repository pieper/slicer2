// .NAME vtkITKImageToImageFilter - Abstract base class for connecting ITK and VTK
// .SECTION Description
// vtkITKImageToImageFilter provides a 

#ifndef __vtkITKTransformRegistrationFilter_h
#define __vtkITKTransformRegistrationFilter_h


#include "vtkITKRegistrationFilter.h"
#include "itkTransformRegistrationFilter.h"
#include "vtkMatrix4x4.h"


///////////////////////////

// vtkITKTransformRegistrationFilter Class

class VTK_EXPORT vtkITKTransformRegistrationFilter : public vtkITKRegistrationFilter
{
public:
  vtkTypeMacro(vtkITKTransformRegistrationFilter,vtkITKImageToImageFilter);

  static vtkITKTransformRegistrationFilter* New(){return 0;};

  virtual void GetTransformationMatrix(vtkMatrix4x4* matrix) = 0;
  
  virtual void GetCurrentTransformationMatrix(vtkMatrix4x4* matrix) = 0;
  
  virtual void SetTransformationMatrix(vtkMatrix4x4 *matrix) = 0;

  virtual void AbortIterations() = 0;

  vtkProcessObject* GetProcessObject() {return this;};

  virtual void ResetMultiResolutionSettings() {};

  vtkSetMacro(Error, int);
  vtkGetMacro(Error, int);

  virtual double GetMetricValue() {return 0;};

protected:

  //BTX
  typedef itk::ImageToImageFilter<itk::itkRegistrationFilterImageType, itk::itkRegistrationFilterImageType> ITKRegistrationType;
  
  //ITKRegistrationType::Pointer m_ITKFilter;

  vtkMatrix4x4 *m_Matrix;

  int Error;

  // default constructor
  vtkITKTransformRegistrationFilter () {Error=0;}; // This is called from New() by vtkStandardNewMacro

  virtual void UpdateRegistrationParameters(){};

  virtual vtkITKRegistrationFilter::OutputImageType::Pointer GetTransformedOutput();

  virtual void CreateRegistrationPipeline();

  virtual ~vtkITKTransformRegistrationFilter(){};
  //ETX


private:
  vtkITKTransformRegistrationFilter(const vtkITKTransformRegistrationFilter&);  // Not implemented.
  void operator=(const vtkITKTransformRegistrationFilter&);  // Not implemented.
};


#endif





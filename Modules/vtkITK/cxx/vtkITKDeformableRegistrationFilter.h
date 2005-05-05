// .NAME vtkITKImageToImageFilter - Abstract base class for connecting ITK and VTK
// .SECTION Description
// vtkITKImageToImageFilter provides a 

#ifndef __vtkITKDeformableRegistrationFilter_h
#define __vtkITKDeformableRegistrationFilter_h


#include "vtkITKRegistrationFilter.h"

#include "itkHistogramMatchingImageFilter.h"
#include "itkWarpImageFilter.h"
#include "itkLinearInterpolateImageFunction.h"


// vtkITKDeformableRegistrationFilter Class

class VTK_EXPORT vtkITKDeformableRegistrationFilter : public vtkITKRegistrationFilter
{
public:
  vtkTypeMacro(vtkITKDeformableRegistrationFilter,vtkITKImageToImageFilter);

  static vtkITKDeformableRegistrationFilter* New(){return 0;};

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
    else {
      return NULL;
    }
  };
  
  // Description:
  // Get dispacement image
  virtual vtkImageData *GetOutputDisplacement()
  {
    return GetOutput(1);
  };

  void Update();

protected:

  //BTX

  typedef itk::Vector<float, 3>    VectorPixelType;
  typedef itk::Image<VectorPixelType, 3> DeformationFieldType;
  typedef itk::VTKImageExport<DeformationFieldType> DeformationExportType;

  DeformationExportType::Pointer itkExporterDisplacement;

  // vtk import for output vtk displacement image
  vtkImageImport *vtkImporterDisplacement;

  // default constructor
  vtkITKDeformableRegistrationFilter () {}; // This is called from New() by vtkStandardNewMacro

  virtual vtkITKDeformableRegistrationFilter::DeformationFieldType::Pointer GetDisplacementOutput() = 0;

  virtual vtkITKRegistrationFilter::OutputImageType::Pointer GetTransformedOutput() = 0;

  virtual void UpdateRegistrationParameters() = 0;

  virtual void CreateRegistrationPipeline(){};

  virtual void ConnectOutputPipelines();

  virtual void AbortIterations() = 0;

  virtual ~vtkITKDeformableRegistrationFilter();
  //ETX


private:
  vtkITKDeformableRegistrationFilter(const vtkITKDeformableRegistrationFilter&);  // Not implemented.
  void operator=(const vtkITKDeformableRegistrationFilter&);  // Not implemented.
};

//vtkCxxRevisionMacro(vtkITKDeformableRegistrationFilter, "$Revision: 1.1 $");
//vtkStandardNewMacro(vtkITKDeformableRegistrationFilter);

#endif





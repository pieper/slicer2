#include "vtkImageHistogramNormalization.h"
#include "vtkObjectFactory.h"
#include "vtkImageData.h"

//------------------------------------------------------------------------------
vtkImageHistogramNormalization* vtkImageHistogramNormalization::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkImageHistogramNormalization");
  if(ret)
    {
    return (vtkImageHistogramNormalization*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkImageHistogramNormalization;
}

vtkImageHistogramNormalization::vtkImageHistogramNormalization()
{
}

vtkImageHistogramNormalization::~vtkImageHistogramNormalization()
{
}

void vtkImageHistogramNormalization::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkImageShiftScale::PrintSelf(os,indent);
}

void vtkImageHistogramNormalization::ExecuteData(vtkDataObject* out)
{
  vtkDebugMacro("Execute to find shift/scale parameters");
  vtkImageData* inData = this->GetInput();
  vtkImageData* outData = this->AllocateOutputData(out);

   
  double typeMax = outData->GetScalarTypeMax();
  float minmax[2];

  inData->GetScalarRange(minmax);
  vtkDebugMacro("Min: " << minmax[0] << " Max: " << minmax[1]);
  
  this->SetShift(-minmax[0]);
  this->SetScale(typeMax/(minmax[1]-minmax[0]));
  this->vtkImageShiftScale::ExecuteData(out);
}

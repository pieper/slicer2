#include "vtkITKTransformRegistrationFilter.h" // This class

vtkITKTransformRegistrationFilter::vtkITKTransformRegistrationFilter()
{
  Error = 0;
  m_Matrix = vtkMatrix4x4::New();
}

vtkITKTransformRegistrationFilter::~vtkITKTransformRegistrationFilter()
{
  m_Matrix->Delete();
}

vtkITKRegistrationFilter::OutputImageType::Pointer vtkITKTransformRegistrationFilter::GetTransformedOutput()
{
  //return m_ITKFilter->GetOutput();
  return NULL;
}


void vtkITKTransformRegistrationFilter::CreateRegistrationPipeline()
{ 
}




#include "vtkITKTransformRegistrationFilter.h" // This class



vtkITKRegistrationFilter::OutputImageType::Pointer vtkITKTransformRegistrationFilter::GetTransformedOutput()
{
  //return m_ITKFilter->GetOutput();
  return NULL;
}


void vtkITKTransformRegistrationFilter::CreateRegistrationPipeline()
{ 
}




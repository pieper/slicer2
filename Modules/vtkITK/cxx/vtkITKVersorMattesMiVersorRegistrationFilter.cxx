#include "vtkITKVersorMattesMiVersorRegistrationFilter.h"
typedef itk::Array<unsigned int> UnsignedIntArray;

vtkITKVersorMattesMiVersorRegistrationFilter::vtkITKVersorMattesMiVersorRegistrationFilter()
{
  m_ITKFilter = itk::itkVersorMattesMiVersorRegistrationFilter::New();
  LinkITKProgressToVTKProgress(m_ITKFilter);
  this->SetNumberOfIterations (100);
  this->SetShrinkFactors(1,1,1);
  this->SetTranslateScale(320);
}

void vtkITKVersorMattesMiVersorRegistrationFilter::CreateRegistrationPipeline()
{
  //DemonsRegistrationFilterCommand::Pointer observer = DemonsRegistrationFilterCommand::New();
  //observer->SetDemonsRegistrationFilter(this);
  //m_ITKFilter->AddIterationObserver(observer );
  m_ITKFilter->SetInput(itkImporterFixed->GetOutput());
  m_ITKFilter->SetInput(1, itkImporterMoving->GetOutput());

  vtkITKTransformRegistrationCommand::Pointer observer = vtkITKTransformRegistrationCommand::New();
  observer->SetRegistrationFilter(this);
  m_ITKFilter->AddIterationObserver(observer );
}

void 
vtkITKVersorMattesMiVersorRegistrationFilter::UpdateRegistrationParameters()
{
  itk::itkVersorMattesMiVersorRegistrationFilter* filter = static_cast<itk::itkVersorMattesMiVersorRegistrationFilter *> (m_ITKFilter);
  UnsignedIntArray numberOfIterations = UnsignedIntArray(1);
  numberOfIterations.Fill(this->GetNumberOfIterations());
  filter->SetNumberOfIterations(numberOfIterations);
  filter->SetTranslationScale(this->GetTranslateScale());
  filter->SetShrinkFactors(this->ShrinkFactors);
  filter->SetMinimumStepLength(this->GetMinimumStepLength());
  filter->SetMaximumStepLength(this->GetMaximumStepLength());
}

vtkITKRegistrationFilter::OutputImageType::Pointer vtkITKVersorMattesMiVersorRegistrationFilter::GetTransformedOutput()
{
   return m_ITKFilter->GetOutput();
}

void
vtkITKVersorMattesMiVersorRegistrationFilter::GetTransformationMatrix(vtkMatrix4x4* matrix)
{
  itk::itkVersorMattesMiVersorRegistrationFilter::TransformType* transform = m_ITKFilter->GetTransform();

  transform->GetRotationMatrix();

  const itk::itkVersorMattesMiVersorRegistrationFilter::TransformType::MatrixType ResMat   =transform->GetRotationMatrix();
  const itk::itkVersorMattesMiVersorRegistrationFilter::TransformType::OffsetType ResOffset=transform->GetOffset();

  // Copy the Rotation Matrix
  for(int i=0;i<3;i++)
    for(int j=0;j<3;j++)
      matrix->Element[i][j] = ResMat[i][j];

  // Copy the Offset
  for(int s=0;s<3;s++)
    matrix->Element[s][3] = ResOffset[s];

  // Fill in the rest
  matrix->Element[3][0] = 0;
  matrix->Element[3][1] = 0;
  matrix->Element[3][2] = 0;
  matrix->Element[3][3] = 1;
}
  
void 
vtkITKVersorMattesMiVersorRegistrationFilter::SetTransformationMatrix(vtkMatrix4x4 *matrix)
{
  itk::itkVersorMattesMiVersorRegistrationFilter::ParametersType  initialParameters = itk::itkVersorMattesMiVersorRegistrationFilter::ParametersType(7);

  vnl_matrix<double> matrix3x4(3,4);

  for(int i=0;i<3;i++)
    for(int j=0;j<4;j++)
      matrix3x4[i][j] = matrix->Element[i][j];
  
  vnl_quaternion<double> matrixAsQuaternion(matrix3x4);

  // There is a transpose between the vnl quaternion and itk quaternion.
  vnl_quaternion<double> conjugated = matrixAsQuaternion.conjugate();

  // This command automatically does the conjugate.
  // But, it does not calculate the paramaters
  // m_Transform->SetRotation(matrixAsQuaternion);

  // Quaternions have 7 parameters. The first four represents the
  // quaternion and the last three represents the offset. 
  initialParameters[0] = conjugated.x();
  initialParameters[1] = conjugated.y();
  initialParameters[2] = conjugated.z();
  initialParameters[3] = conjugated.r();
  initialParameters[4] = matrix->Element[0][3];
  initialParameters[5] = matrix->Element[1][3];
  initialParameters[6] = matrix->Element[2][3];

  itk::itkVersorMattesMiVersorRegistrationFilter::TransformType::Pointer transform = itk::itkVersorMattesMiVersorRegistrationFilter::TransformType::New();
  transform->SetParameters(initialParameters);
  // The guess is: a quaternion followed by a translation
  m_ITKFilter->SetTransform(transform);
}


void 
vtkITKVersorMattesMiVersorRegistrationFilter::SetShrinkFactors(unsigned int i,
                                                               unsigned int j, 
                                                               unsigned int k)
{
  ShrinkFactors[0] = i;
  ShrinkFactors[1] = j;
  ShrinkFactors[2] = k;

} //vtkITKVersorMattesMiVersorRegistrationFilter

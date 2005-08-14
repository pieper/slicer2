// .NAME vtkITKImageToImageFilter - Abstract base class for connecting ITK and VTK
// .SECTION Description
// vtkITKImageToImageFilter provides a 

#ifndef __itkDemonsTransformRegistrationFilter_h
#define __itkDemonsTransformRegistrationFilter_h

#include "itkImageToImageFilter.h"
#include "itkEventObject.h"
#include "itkImage.h"

#include "itkDemonsRegistrationFilter.h"
#include "itkHistogramMatchingImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkWarpImageFilter.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkCastImageFilter.h"
#include "itkTransform.h"
#include "itkResampleImageFilter.h"
#include "itkImageFileWriter.h"

//BTX
namespace itk 
{

// itkDemonsTransformRegistrationFilter Class

template <class TImageType >

class ITK_EXPORT itkDemonsTransformRegistrationFilter : public itk::ImageToImageFilter< TImageType, TImageType>
{
public:

  typedef itkDemonsTransformRegistrationFilter             Self;
  typedef itk::ImageToImageFilter<TImageType,TImageType>  Superclass;
  typedef ::itk::SmartPointer<Self>          Pointer;
  typedef ::itk::SmartPointer<const Self>    ConstPointer;

  typedef itk::Vector<float, 3>    VectorPixelType;

  typedef TImageType FixedImageType;
  typedef TImageType MovingImageType;

  typedef itk::Image<VectorPixelType, 3> DeformationFieldType;

  typedef itk::Transform<double, 3, 3 > TransformType;
  
  /** Method for creation through the object factory. */
  itkNewMacro(Self);  

  /** Runtime information support. */
  itkTypeMacro(TImageType, ImageToImageFilter);
  
  /** Set init transfrom */
  void SetTransform( const TransformType* transform )
  {
    m_Transform->SetParameters( transform->GetParameters() );
  }

  /** Get resulting transform */
  void GetTransform(TransformType* transform)
  {
    transform->SetParameters( m_Transform->GetParameters() );
  }


  itkSetMacro(StandardDeviations, double);
  itkGetMacro(StandardDeviations, double);

  itkSetMacro(NumIterations, int);
  itkGetMacro(NumIterations, int);

  itkSetMacro(CurrentIteration, int);
  itkGetMacro(CurrentIteration, int);

  itkSetMacro(WriteInputs, bool);
  itkGetMacro(WriteInputs, bool);

  DeformationFieldType * GetDeformationField(void);

  virtual void AbortIterations() {
    m_Filter->SetAbortGenerateData(true);
  };

  unsigned long AddIterationObserver (itk::Command *observer );
protected:

  void  GenerateData ();

  double m_StandardDeviations;
  int    m_NumIterations;
  int    m_CurrentIteration;
  bool   m_WriteInputs;
  
  typedef float InternalPixelType;
  typedef itk::Image<InternalPixelType, 3 >  InternalImageType;

  ////////////////////////////////
  // ITK Pipeline that does the job
  ////////////////////////////////

  // Transformer for moving image
  typename TransformType::Pointer                m_Transform;
  typename InternalImageType::PixelType          m_BackgroundLevel;

  typedef itk::ResampleImageFilter<
                                    MovingImageType,
                                    MovingImageType  >    ResampleFilterType;
  typename ResampleFilterType::Pointer           m_Resampler;

  // Casters

  typedef itk::CastImageFilter< FixedImageType, 
                                InternalImageType > FixedImageCasterType;
  typedef itk::CastImageFilter< MovingImageType, 
                                InternalImageType > MovingImageCasterType;

  typename FixedImageCasterType::Pointer m_FixedImageCaster;

  typename MovingImageCasterType::Pointer m_MovingImageCaster;

  // Matcher
  typedef itk::HistogramMatchingImageFilter<InternalImageType,InternalImageType> MatchingFilterType;
  MatchingFilterType::Pointer m_Matcher;

  // Registration filter
  typedef itk::DemonsRegistrationFilter<
                                InternalImageType,
                                InternalImageType,
                                DeformationFieldType>   RegistrationFilterType;
  RegistrationFilterType::Pointer m_Filter;

  // Warper and interpolator
  typedef itk::WarpImageFilter<
                       MovingImageType,
                       MovingImageType,
                       DeformationFieldType>  WarperType;

  typedef itk::LinearInterpolateImageFunction<
                                   MovingImageType,
                                   double          >  InterpolatorType;

  typename WarperType::Pointer m_Warper;
  typename InterpolatorType::Pointer m_Interpolator;

  // Writer
  typedef itk::ImageFileWriter< InternalImageType  >    ImageWriterType;      
  typename ImageWriterType::Pointer m_Writer;

  // default constructor
  itkDemonsTransformRegistrationFilter(); 

  virtual ~itkDemonsTransformRegistrationFilter() {};
  
private:
  itkDemonsTransformRegistrationFilter(const itkDemonsTransformRegistrationFilter&);  // Not implemented.
  void operator=(const itkDemonsTransformRegistrationFilter&);  // Not implemented.
};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkDemonsTransformRegistrationFilter.txx"
#endif

 //ETX

#endif





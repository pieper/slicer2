
#ifndef __itkDemonsTransformRegistrationFilterF3_h
#define __itkDemonsTransformRegistrationFilterF3_h

//BTX
#include "itkDemonsTransformRegistrationFilter.h"

typedef  itk::Image<float, 3> ImageType;

namespace itk {
class ITK_EXPORT itkDemonsTransformRegistrationFilterF3 : public itk::itkDemonsTransformRegistrationFilter<ImageType>
{
public:
  typedef itkDemonsTransformRegistrationFilterF3             Self;
  typedef itk::itkDemonsTransformRegistrationFilter<ImageType>  Superclass;
  typedef ::itk::SmartPointer<Self>          Pointer;
  typedef ::itk::SmartPointer<const Self>    ConstPointer;


  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(itkDemonsTransformRegistrationFilterF3, itk::itkDemonsTransformRegistrationFilter);
  
protected:  
  // Default constructor
  itkDemonsTransformRegistrationFilterF3(){};

private:
  itkDemonsTransformRegistrationFilterF3(const itkDemonsTransformRegistrationFilterF3&);  // Not implemented.
  void operator=(const itkDemonsTransformRegistrationFilterF3&);  // Not implemented.
};

} // namespace itk

//ETX

#endif

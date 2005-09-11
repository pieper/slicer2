
#ifndef __itkBSplineMattesMIRegistrationFilterFF_h
#define __itkBSplineMattesMIRegistrationFilterFF_h

//BTX
#include "itkBSplineMattesMIRegistrationFilter.h"

typedef  itk::Image<float, 3> ImageType;

namespace itk {
class ITK_EXPORT itkBSplineMattesMIRegistrationFilterFF : public itk::itkBSplineMattesMIRegistrationFilter<ImageType>
{
public:
  typedef itkBSplineMattesMIRegistrationFilterFF             Self;
  typedef itk::itkBSplineMattesMIRegistrationFilter<ImageType>  Superclass;
  typedef ::itk::SmartPointer<Self>          Pointer;
  typedef ::itk::SmartPointer<const Self>    ConstPointer;


  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(itkBSplineMattesMIRegistrationFilterFF, itk::itkBSplineMattesMIRegistrationFilter);
  
protected:  
  // Default constructor
  itkBSplineMattesMIRegistrationFilterFF(){};

private:
  itkBSplineMattesMIRegistrationFilterFF(const itkBSplineMattesMIRegistrationFilterFF&);  // Not implemented.
  void operator=(const itkBSplineMattesMIRegistrationFilterFF&);  // Not implemented.
};

} // namespace itk

//ETX

#endif

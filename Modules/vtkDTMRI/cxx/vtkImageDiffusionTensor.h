// .NAME vtkImageDiffusionTensor - 
// .SECTION Description

#ifndef __vtkImageDiffusionTensor_h
#define __vtkImageDiffusionTensor_h

#include "vtkDTMRIConfigure.h"
#include "vtkImageMultipleInputFilter.h"
#include "vtkVectorToOuterProductDualBasis.h"
#include "vtkTransform.h"

class VTK_DTMRI_EXPORT vtkImageDiffusionTensor : public vtkImageMultipleInputFilter
{
 public:
  static vtkImageDiffusionTensor *New();
  vtkTypeMacro(vtkImageDiffusionTensor,vtkImageMultipleInputFilter);

  // Description:
  // Set the image created without diffusion gradients
  void SetNoDiffusionImage(vtkImageData *img) 
    {this->SetInput(0,img);}

  // Description:
  // Set the image corresponding to diffusion gradient number num
  void SetDiffusionImage(int num, vtkImageData *img) 
    {this->SetInput(num+1,img);}

  // Description:
  // The number of gradients is the same as the number of input
  // diffusion ImageDatas this filter will require.
  void SetNumberOfGradients(int num);
  vtkGetMacro(NumberOfGradients,int);

  // Description:
  // Set the 3-vectors describing the gradient directions
  void SetDiffusionGradient(int num, vtkFloatingPointType gradient[3])
    {this->DualBasis->SetInputVector(num,gradient);}
  void SetDiffusionGradient(int num, vtkFloatingPointType g0, vtkFloatingPointType g1, vtkFloatingPointType g2)
    {this->DualBasis->SetInputVector(num,g0,g1,g2);}

  // Description:
  // Get the 3-vectors describing the gradient directions
  //vtkFloatingPointType *GetDiffusionGradient(int num)
  //{return this->DualBasis->GetInputVector(num);}
  // the following look messy but are copied from vtkSetGet.h,
  // just adding the num parameter we need.

  virtual vtkFloatingPointType *GetDiffusionGradient(int num)
    { 
      vtkDebugMacro(<< this->GetClassName() << " (" << this << "): returning " << "DiffusionGradient pointer " << num << " " << this->DualBasis->GetInputVector(num)); 
      return this->DualBasis->GetInputVector(num);
    }

  // Description:
  // This is cheating since I don't know how to wrap this in tcl.
  //  First call SelectDiffusionGradient num
  //  Then GetSelectedDiffusionGradient to receive it as a string
  vtkGetVector3Macro(SelectedDiffusionGradient,vtkFloatingPointType);
  void SelectDiffusionGradient (int num)
    {
      this->SetSelectedDiffusionGradient(this->DualBasis->GetInputVector(num));
    }
  
  // Description:
  // For numerical stability: regularization parameter
  vtkGetMacro(Regularization,vtkFloatingPointType);
  vtkSetMacro(Regularization,vtkFloatingPointType);
  
  // Lauren make choices for type of regularization?

  // Description:
  // For numerical stability: regularization parameter
  // Controls the amount of regularization to do
  vtkGetMacro(Alpha,vtkFloatingPointType);
  vtkSetMacro(Alpha,vtkFloatingPointType);

  // Description:
  // For numerical stability: regularization parameter
  // Controls the drop-off of regularization where signal is strong
  vtkGetMacro(Beta,vtkFloatingPointType);
  vtkSetMacro(Beta,vtkFloatingPointType);

  // Description:
  // Scale factor for parameters of gradient fields
  // (LeBihan's b factor for physical MR gradient parameters)
  vtkGetMacro(B,vtkFloatingPointType);
  vtkSetMacro(B,vtkFloatingPointType);

  // Description:
  // Scale factor that exists in input image data
  vtkGetMacro(InputScaleFactor,vtkFloatingPointType);
  vtkSetMacro(InputScaleFactor,vtkFloatingPointType);

  // Description
  // Transformation of the tensors (for RAS coords, for example)
  // The gradient vectors are multiplied by this matrix
  vtkSetObjectMacro(Transform, vtkTransform);
  vtkGetObjectMacro(Transform, vtkTransform);

  // Description:
  // Internal class use only
  vtkFloatingPointType** GetG() {return this->G;}
  
  // Description:
  // Internal class use only
  vtkGetObjectMacro(DualBasis,vtkVectorToOuterProductDualBasis)

  // Description:
  // Internal class use only
  //BTX
  void TransformDiffusionGradients();
  //ETX

 protected:
  vtkImageDiffusionTensor();
  ~vtkImageDiffusionTensor();
  vtkImageDiffusionTensor(const vtkImageDiffusionTensor&) {};
  void operator=(const vtkImageDiffusionTensor&) {};
  void PrintSelf(ostream& os, vtkIndent indent);

  int NumberOfGradients;

  // Lauren remove this factor?
  vtkFloatingPointType Regularization;
  vtkFloatingPointType B;
  vtkFloatingPointType InputScaleFactor;
  vtkFloatingPointType Alpha;
  vtkFloatingPointType Beta;

  // for transforming tensors
  vtkTransform *Transform;

  vtkVectorToOuterProductDualBasis *DualBasis;
  // this is ~G 
  vtkFloatingPointType **G;

  void ExecuteInformation(vtkImageData **inDatas, vtkImageData *outData);
  void ExecuteInformation(){this->vtkImageMultipleInputFilter::ExecuteInformation();};
  void ThreadedExecute(vtkImageData **inDatas, vtkImageData *outData,
        int extent[6], int id);

  // We override this in order to allocate output tensors
  // before threading happens.  This replaces the superclass 
  // vtkImageMultipleInputFilter's Execute function.
  void ExecuteData(vtkDataObject *out);

  // just for tcl wrapping
  vtkFloatingPointType SelectedDiffusionGradient[3];
  vtkSetVector3Macro(SelectedDiffusionGradient,vtkFloatingPointType);

};

#endif





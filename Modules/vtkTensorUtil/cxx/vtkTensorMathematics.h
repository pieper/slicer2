// .NAME vtkTensorMathematics - Trace, determinant, anisotropy measures
// .SECTION Description
// Operates on input tensors and outputs image data scalars 
// that describe some feature of the input tensors.
//
// In future should optionally pass through input tensors,
// and also possibly output tensors with eigenvectors as columns.
// Currently a two-input filter like vtkImageMathematics, which
// may be useful someday.
//


#ifndef __vtkTensorMathematics_h
#define __vtkTensorMathematics_h


// Operation options.
#define VTK_TENS_TRACE                  0
#define VTK_TENS_DETERMINANT            1
#define VTK_TENS_RELATIVE_ANISOTROPY    2  
#define VTK_TENS_FRACTIONAL_ANISOTROPY  3
#define VTK_TENS_MAX_EIGENVALUE         4
#define VTK_TENS_MID_EIGENVALUE         5
#define VTK_TENS_MIN_EIGENVALUE         6
#define VTK_TENS_LINEAR_MEASURE         7
#define VTK_TENS_PLANAR_MEASURE         8
#define VTK_TENS_SPHERICAL_MEASURE      9
#define VTK_TENS_COLOR_ORIENTATION     10
#define VTK_TENS_D11                   11
#define VTK_TENS_D22                   12
#define VTK_TENS_D33                   13

#include "vtkTensorUtilConfigure.h"
#include "vtkImageTwoInputFilter.h"
#include "vtkMatrix4x4.h"

class VTK_TENSORUTIL_EXPORT vtkTensorMathematics : public vtkImageTwoInputFilter
{
public:
  static vtkTensorMathematics *New();
  vtkTypeMacro(vtkTensorMathematics,vtkImageTwoInputFilter);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Get the Operation to perform.
  vtkGetMacro(Operation,int);

  // Description:
  // Output the trace (sum of eigenvalues = sum along diagonal)
  void SetOperationToTrace() 
    {this->SetOperation(VTK_TENS_TRACE);};

  // Description:
  // Output the determinant
  void SetOperationToDeterminant() 
    {this->SetOperation(VTK_TENS_DETERMINANT);};

  // Description:
  // Output various anisotropy and shape measures
  void SetOperationToRelativeAnisotropy() 
    {this->SetOperation(VTK_TENS_RELATIVE_ANISOTROPY);};
  void SetOperationToFractionalAnisotropy() 
    {this->SetOperation(VTK_TENS_FRACTIONAL_ANISOTROPY);};
  void SetOperationToLinearMeasure() 
    {this->SetOperation(VTK_TENS_LINEAR_MEASURE);};
  void SetOperationToPlanarMeasure() 
    {this->SetOperation(VTK_TENS_PLANAR_MEASURE);};
  void SetOperationToSphericalMeasure() 
    {this->SetOperation(VTK_TENS_SPHERICAL_MEASURE);};

  // Description:
  // Output a selected eigenvalue
  void SetOperationToMaxEigenvalue() 
    {this->SetOperation(VTK_TENS_MAX_EIGENVALUE);};
  void SetOperationToMiddleEigenvalue() 
    {this->SetOperation(VTK_TENS_MID_EIGENVALUE);};
  void SetOperationToMinEigenvalue() 
    {this->SetOperation(VTK_TENS_MIN_EIGENVALUE);};


  // Description: 
  // Output a matrix (tensor) component
  void SetOperationToD11() 
    {this->SetOperation(VTK_TENS_D11);};
  void SetOperationToD22() 
    {this->SetOperation(VTK_TENS_D22);};
  void SetOperationToD33() 
    {this->SetOperation(VTK_TENS_D33);};
  
  // Description:
  // Output RGB color according to XYZ of eigenvectors.
  // Output A (alpha, or transparency) according to 
  // anisotropy (1-spherical measure).
  void SetOperationToColorByOrientation() 
    {this->SetOperation(VTK_TENS_COLOR_ORIENTATION);};

  // Description:
  // Specify scale factor to scale output (float) scalars by.
  // This is not used when the output is RGBA (char color data).
  vtkSetMacro(ScaleFactor,float);
  vtkGetMacro(ScaleFactor,float);

  // Description:
  // Turn on/off extraction of eigenvalues from tensor.
  vtkSetMacro(ExtractEigenvalues,int);
  vtkBooleanMacro(ExtractEigenvalues,int);
  vtkGetMacro(ExtractEigenvalues,int);

  // Description
  // This matrix is only used for ColorByOrientation.
  // We transform the tensor orientation by this matrix
  // before setting the output RGB values.
  //
  // This is useful to put the output colors into a standard
  // coordinate system (i.e. RAS) regardless of the data scan order.
  //
  // Example usage is as follows:
  // 1) If tensors are to be displayed in a coordinate system
  //    that is not IJK (array-based), and the whole volume is
  //    being rotated, each tensor needs also to be rotated.
  //    First find the matrix that positions your volume.
  //    (This is how the entire volume is positioned, not 
  //    the matrix that positions an arbitrary reformatted slice.)
  // 2) Remove scaling and translation from this matrix; we
  //    just need to rotate each tensor.
  // 3) Set TensorRotationMatrix to this rotation matrix.
  //
  vtkSetObjectMacro(TensorRotationMatrix, vtkMatrix4x4);
  vtkGetObjectMacro(TensorRotationMatrix, vtkMatrix4x4);

protected:
  vtkTensorMathematics();
  ~vtkTensorMathematics() {};
  vtkTensorMathematics(const vtkTensorMathematics&) {};
  void operator=(const vtkTensorMathematics&) {};

  int Operation; // math operation to perform
  vtkSetMacro(Operation,int);  
  float ScaleFactor; // Scale factor for output scalars
  int ExtractEigenvalues; // Boolean controls eigenfunction extraction

  vtkMatrix4x4 *TensorRotationMatrix;

  void ExecuteInformation(vtkImageData **inDatas, vtkImageData *outData);
  void ExecuteInformation(){this->vtkImageTwoInputFilter::ExecuteInformation();};
  void ThreadedExecute(vtkImageData **inDatas, vtkImageData *outData,
		       int extent[6], int id);
};

#endif














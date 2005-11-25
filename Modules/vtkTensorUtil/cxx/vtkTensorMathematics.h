/*=auto=========================================================================

(c) Copyright 2005 Massachusetts Institute of Technology (MIT) All Rights Reserved.

This software ("3D Slicer") is provided by The Brigham and Women's 
Hospital, Inc. on behalf of the copyright holders and contributors.
Permission is hereby granted, without payment, to copy, modify, display 
and distribute this software and its documentation, if any, for  
research purposes only, provided that (1) the above copyright notice and 
the following four paragraphs appear on all copies of this software, and 
(2) that source code to any modifications to this software be made 
publicly available under terms no more restrictive than those in this 
License Agreement. Use of this software constitutes acceptance of these 
terms and conditions.

3D Slicer Software has not been reviewed or approved by the Food and 
Drug Administration, and is for non-clinical, IRB-approved Research Use 
Only.  In no event shall data or images generated through the use of 3D 
Slicer Software be used in the provision of patient care.

IN NO EVENT SHALL THE COPYRIGHT HOLDERS AND CONTRIBUTORS BE LIABLE TO 
ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL 
DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, 
EVEN IF THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE BEEN ADVISED OF THE 
POSSIBILITY OF SUCH DAMAGE.

THE COPYRIGHT HOLDERS AND CONTRIBUTORS SPECIFICALLY DISCLAIM ANY EXPRESS 
OR IMPLIED WARRANTIES INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND 
NON-INFRINGEMENT.

THE SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS 
IS." THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE NO OBLIGATION TO 
PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.


=========================================================================auto=*/
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
#define VTK_TENS_MODE                  14
#define VTK_TENS_COLOR_MODE            15

#include "vtkTensorUtilConfigure.h"
#include "vtkImageTwoInputFilter.h"
#include "vtkMatrix4x4.h"
#include "vtkImageData.h"

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
  // This is the skewness of the eigenvalues 
  // (thanks to Gordon Lothar (of the Hill People) Kindlmann)
  void SetOperationToMode() 
    {this->SetOperation(VTK_TENS_MODE);};

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
  // Output RGB color according to colormapping of mode, with 
  // final RGB being a linear combination of gray and 
  // this color.  Amount of gray is determined by FA.
  // Thanks to Gordon Lothar Kindlmann for this method.
  void SetOperationToColorByMode() 
    {this->SetOperation(VTK_TENS_COLOR_MODE);};

  // Description:
  // Specify scale factor to scale output (float) scalars by.
  // This is not used when the output is RGBA (char color data).
  vtkSetMacro(ScaleFactor,vtkFloatingPointType);
  vtkGetMacro(ScaleFactor,vtkFloatingPointType);

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

  // Description
  // Input scalars are a binary mask: 0 prevents display
  // of tensor quantity at that point
  vtkBooleanMacro(MaskWithScalars, int);
  vtkSetMacro(MaskWithScalars, int);
  vtkGetMacro(MaskWithScalars, int);

  // Description:
  // Scalar mask
  vtkSetObjectMacro(ScalarMask, vtkImageData);
  vtkGetObjectMacro(ScalarMask, vtkImageData);
  

  // Public for access from threads
  static void ModeToRGB(double Mode, double FA,
                 double &R, double &G, double &B);


  // Description:
  // Helper functions to perform operations pixel-wise
  static vtkFloatingPointType Determinant(vtkFloatingPointType D[3][3]);
  static vtkFloatingPointType Trace(vtkFloatingPointType D[3][3]);
  static vtkFloatingPointType RelativeAnisotropy(vtkFloatingPointType w[3]);
  static vtkFloatingPointType FractionalAnisotropy(vtkFloatingPointType w[3]);
  static vtkFloatingPointType LinearMeasure(vtkFloatingPointType w[3]);
  static vtkFloatingPointType PlanarMeasure(vtkFloatingPointType w[3]);
  static vtkFloatingPointType SphericalMeasure(vtkFloatingPointType w[3]);
  static vtkFloatingPointType MaxEigenvalue(vtkFloatingPointType w[3]);
  static vtkFloatingPointType MiddleEigenvalue(vtkFloatingPointType w[3]);
  static vtkFloatingPointType MinEigenvalue(vtkFloatingPointType w[3]);
  static vtkFloatingPointType Mode(vtkFloatingPointType w[3]);
  static void ColorByMode(vtkFloatingPointType w[3], vtkFloatingPointType &R,vtkFloatingPointType &G, vtkFloatingPointType &B);

  //Description
  //Wrap function to teem eigen solver
  static int TeemEigenSolver(double **m, double *w, double **v);

protected:
  vtkTensorMathematics();
  ~vtkTensorMathematics() {};
  vtkTensorMathematics(const vtkTensorMathematics&) {};
  void operator=(const vtkTensorMathematics&) {};

  int Operation; // math operation to perform
  vtkSetMacro(Operation,int);  
  vtkFloatingPointType ScaleFactor; // Scale factor for output scalars
  int ExtractEigenvalues; // Boolean controls eigenfunction extraction

  int MaskWithScalars;
  vtkImageData *ScalarMask;
  
  vtkMatrix4x4 *TensorRotationMatrix;

  void ExecuteInformation(vtkImageData **inDatas, vtkImageData *outData);
  void ExecuteInformation(){this->vtkImageTwoInputFilter::ExecuteInformation();};
  void ThreadedExecute(vtkImageData **inDatas, vtkImageData *outData,
        int extent[6], int id);
};

#endif














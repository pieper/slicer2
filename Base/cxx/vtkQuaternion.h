/*=auto=========================================================================

(c) Copyright 2002 Massachusetts Institute of Technology

Permission is hereby granted, without payment, to copy, modify, display
and distribute this software and its documentation, if any, for any purpose,
provided that the above copyright notice and the following three paragraphs
appear on all copies of this software.  Use of this software constitutes
acceptance of these terms and conditions.

IN NO EVENT SHALL MIT BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL,
INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE
AND ITS DOCUMENTATION, EVEN IF MIT HAS BEEN ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.

MIT SPECIFICALLY DISCLAIMS ANY EXPRESS OR IMPLIED WARRANTIES INCLUDING,
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR
A PARTICULAR PURPOSE, AND NON-INFRINGEMENT.

THE SOFTWARE IS PROVIDED "AS IS."  MIT HAS NO OBLIGATION TO PROVIDE
MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

=========================================================================auto=*/
// .NAME vtkQuaternion - higher-dimension complex numbers
// .SECTION Description
// Quaternions are a higher-dimension complex number with 1 real part and
// 3 imaginary parts: q = s + ia + jb + kc
//
// There useful for representing rotations because they are more suitable
// to concatenation and interpolation than the alternatives of 3x3 orthogonal
// matrices, or 3 Euler angles.

#ifndef vtkQuaternion_H 
#define vtkQuaternion_H

#include "vtkObject.h"
#include "vtkVector3.h"

class VTK_SLICER_BASE_EXPORT vtkQuaternion : public vtkObject
{
public:
  // VTK requisites
  static vtkQuaternion *New();
  vtkTypeMacro(vtkQuaternion,vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Query routines
  double *GetElements();
  double GetElement(int y);
  double GetScalar();
  void GetVector(vtkVector3 *v);
  double GetRotationAngle();
  void GetRotationAxis(vtkVector3 *v);

  // Assignment routines
  void Zero();
  void Identity();
  void Copy(vtkQuaternion *q);
  void SetElement(int y, double v);
  void SetElements(double s, double x, double y, double z);
  void Set(double s, vtkVector3 *v);
  void SetRotation(vtkVector3 *axis, const double angle) ;

  // Operations
  double Length();
  void Normalize();
  void Invert(vtkQuaternion *q);
  void Invert();
  void Multiply(double s);
  void Multiply(vtkQuaternion *Q1, vtkQuaternion *Q2);
  void Multiply(vtkQuaternion *Q2);
  void Add(vtkQuaternion *q);
  void Rotate(vtkVector3 *u, vtkVector3 *v);

protected:
  // Constructor/Destructor
  vtkQuaternion();
  ~vtkQuaternion() {};

  double Element[4];
};

#endif

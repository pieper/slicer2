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
// .NAME vtkVector3 - 3-element column vectors
// .SECTION Description
#ifndef vtkVector3_H 
#define vtkVector3_H

#include "vtkObject.h"

#define VECTOR3_EPSILON 0.000001f

class VTK_EXPORT vtkVector3 : public vtkObject
{
public:
  // VTK requisites
  static vtkVector3 *New();
  vtkTypeMacro(vtkVector3,vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  // Query routines
  double *GetElements() {return this->Element;};
  double GetElement(int y);
//void PrintSelf();

  // Assignment routines
  void Copy(vtkVector3 *v);
  void SetElement(int y, double v);
  void SetElements(double x, double y, double z);
  void Zero();
  void XHat() {this->Zero(); this->Element[0] = 1.0f;};
  void YHat() {this->Zero(); this->Element[1] = 1.0f;};
  void ZHat() {this->Zero(); this->Element[2] = 1.0f;};

  // Operations
  double Length();
  void Normalize();
  void Invert();
  void Add(double s);
  void Add(vtkVector3 *v);
  void Add(vtkVector3 *A, vtkVector3* B);
  void Subtract(vtkVector3 *v);
  void Subtract(vtkVector3 *A, vtkVector3 *B);
  void Multiply(double s);
  double Dot(vtkVector3 *v);
  void Cross(vtkVector3 *A, vtkVector3 *B);

protected:
  // Constructor/Destructor
  vtkVector3();
  ~vtkVector3() {};

  double Element[3];
};

#endif

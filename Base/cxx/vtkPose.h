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
// .NAME vtkPose - Pose for registration
// .SECTION Description
// Pose consists of a vector of translation, and a quanterion of rotation.
#ifndef vtkPose_H 
#define vtkPose_H

#include "vtkObject.h"
#include "vtkQuaternion.h"
#include "vtkVector3.h"
#include "vtkMatrix4x4.h"

class VTK_EXPORT vtkPose : public vtkObject
{
public:
  // VTK requisites
  static vtkPose *New();
  vtkTypeMacro(vtkPose,vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Query routines
  vtkVector3 *GetTranslation() {return this->Translation;};
  vtkQuaternion *GetRotation() {return this->Rotation;};

  // Assignment routines
  void Identity();
  void Copy(vtkPose *p);
  void SetTranslation(vtkVector3 *t) {this->Translation->Copy(t); this->Modified();};
  void SetRotation(vtkQuaternion *r) {this->Rotation->Copy(r); this->Modified();};
  void Set(vtkQuaternion *r, vtkVector3 *t) {
    this->Translation->Copy(t); this->Rotation->Copy(r); this->Modified();};

  // Operations
  void Invert(vtkPose *p);
  void Invert();
  void Normalize();
  void Concat(vtkPose *p1, vtkPose *p2);
  void Concat(vtkPose *p2);
  void Transform(vtkVector3 *u, vtkVector3 *v);

  // Conversions
  void ConvertToMatrix4x4(vtkMatrix4x4 *m);
  void ConvertFromMatrix4x4(vtkMatrix4x4 *m);

protected:
  // Constructors/Destructor
  vtkPose();
  ~vtkPose();

  vtkVector3 *Translation;
  vtkQuaternion *Rotation;
};

#endif

/*=auto=========================================================================
                                                                                
(c) Copyright 2004 Massachusetts Institute of Technology (MIT) All Rights Reserved.
                                                                                
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
#ifndef __vtk_cone_predicate_h
#define __vtk_cone_predicate_h
#include <vtkMorphometricsConfigure.h>
#include "vtkPredicate.h"
#include "vtkAxisSource.h"
#include <vtkSetGet.h>

//---------------------------------------------------------
// Author: Axel Krauth
//
// this class represents an implicitly given cone. The cone is specified 
// by the axis of the cone and an angle. The center of the axis is
// the basis of the cone. A point p is inside the cone iff the vector
// from the basis of the cone to p has an angle to the axis smaller
// than the specified angle.
class VTK_MORPHOMETRICS_EXPORT vtkConePredicate : public vtkPredicate
{
  public:
  static vtkConePredicate* New();
  void Delete();
  vtkTypeMacro(vtkConePredicate,vtkPredicate);
  void PrintSelf();
  
  vtkSetObjectMacro(Axis,vtkAxisSource);

  vtkSetMacro(MaximalAngle,float);
  vtkGetMacro(MaximalAngle,float);

  // override in order to reflect changes in Axis
  unsigned long int GetMTime();


  virtual bool P(float* x);
  virtual void InitP();

 protected:
  vtkConePredicate();
  ~vtkConePredicate();

 private:
  vtkConePredicate(vtkConePredicate&);
  void operator=(const vtkConePredicate);

  vtkAxisSource* Axis;

  float MaximalAngle;

  // for deciding P(x) I currently need a vector pointer
  // this is the temp variable for this
  float* DiffVector;
};

#endif

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
#ifndef __vtk_predicate__h
#define __vtk_predicate__h
#include <vtkMorphometricsConfigure.h>
#include <vtkObject.h>
#include <vtkSetGet.h>
//---------------------------------------------------------
// Author: Axel Krauth
//
// An object of type vtkPredicate represents a predicate P 
// for points in R^3. Two function have to be implemented:
//  - InitP : Called to give the predicate the opportunity to
//            update itself prior to calling P a lot of times.
//  -     P : returns true for a Point x iff P holds for x.
//            This function gets called quite often.
class VTK_MORPHOMETRICS_EXPORT vtkPredicate : public vtkObject
{
 public:
  static vtkPredicate* New();
  void Delete();
  vtkTypeMacro(vtkPredicate,vtkObject);
  void PrintSelf();
  virtual bool P(vtkFloatingPointType* x);
  virtual void InitP();

 protected:
  vtkPredicate();
  ~vtkPredicate();

  void Execute();
 private:
  vtkPredicate(vtkPredicate&);
  void operator=(const vtkPredicate);
};

#endif

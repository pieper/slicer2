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
#ifndef __vtk_axis_source_h
#define __vtk_axis_source_h
#include <vtkMorphometricsConfigure.h>
#include <vtkObject.h>
#include <vtkPolyDataSource.h>
#include <vtkCylinderSource.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkTransform.h>
#include <vtkPolyData.h>
//---------------------------------------------------------
// Author: Axel Krauth
//
// Representation of an axis as a transformed cylinder.

class VTK_MORPHOMETRICS_EXPORT vtkAxisSource : public vtkPolyDataSource
{
 public:
  static vtkAxisSource* New();
  void Delete();
  vtkTypeMacro(vtkAxisSource,vtkPolyDataSource);
  void PrintSelf();

  vtkGetVector3Macro(Center,float);
  vtkGetVector3Macro(Direction,float);

  void SetDirection(float,float,float);
  void SetDirection(float*);

  void SetCenter(float,float,float);
  void SetCenter(float*);
 
 // compute the angle between the direction of the argument axis or vector
 // returns degrees
 float Angle(float*);
 float Angle(vtkAxisSource* right) {return Angle(right->Direction);};

 protected:
  vtkAxisSource();
  ~vtkAxisSource();

  void Execute();
 private:
  vtkAxisSource(vtkAxisSource&);
  void operator=(const vtkAxisSource);

 // visualization representation
  vtkCylinderSource* AxisSource;
  vtkTransformPolyDataFilter* AxisFilter;
  vtkTransform* AxisTransform;

 // internal representation
  float* Direction;
  float* Center;

 // two functions for making both representations consistent with each other.
 // the new values for the visualization variables or representation variables
 // are based on the not updated variables
 void UpdateVisualization();
 void UpdateRepresentation();
};

#endif

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
#ifndef __vtk_pelvis_metric_h
#define __vtk_pelvis_metric_h
#include "vtkMorphometricsConfigure.h"
#include <vtkObject.h>
#include <vtkPlaneSource.h>
#include <vtkCylinderSource.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkTransform.h>
#include <vtkPolyData.h>
//---------------------------------------------------------
// Author: Axel Krauth
//
// This class represents the basic and derived geometric
// properties of a hip. The basic properties are the acetabular
// plane as well as a coordinate system. The derived properties
// are the anteversion and inclination, which are the angles of the 
// acetabular plane in the coordinate system.
//
// The anteversion and inclination are dependend on a hip coordinate
// system since without they would be dependend how the image was acquired.
//
// The coordinate system has one degree of freedom, the frontal axis.
// The sagittal axis is constant (0,1,0) and the longitudinal axis
// is orthogonal to both axis. This approach is due to the way the inclination
// angle is computed by physicians. On a x-ray they connect the two caudal border
// points of the hip to a line and compute the angle with the acetabular plane (which
// is a line on a x-ray). The line connecting the two caudal border points is the 
// frontal axis in the coordinate system. Whereas that can be transferred to 3D I found
// no way to specify how much the hip is tilted, so I set it to (0,1,0).
//
// The visualization of the frontal axis is the transformation of a vtkCylinderSource.
// When you want to change the frontal axis, you have to set the transform
// LowerBoundaryFilter->GetTransform() so that multiplying it with (0,1,0,0), which is the
// initial direction of a vtkCylinderSource, yields the wanted direction.
//
class VTK_MORPHOMETRICS_EXPORT vtkPelvisMetric : public vtkObject
{
 public:
  static vtkPelvisMetric* New();
  void Delete();
  vtkTypeMacro(vtkPelvisMetric,vtkObject);
  void PrintSelf();

  vtkGetMacro(InclinationAngle,float);
  vtkGetMacro(AnteversionAngle,float);

  void SetPelvis(vtkPolyData*);
  vtkGetObjectMacro(Pelvis,vtkPolyData);

  vtkGetVector3Macro(FrontalAxis,float);
  vtkGetVector3Macro(SagittalAxis,float);
  vtkGetVector3Macro(LongitudinalAxis,float);

 // center of gravity of the member Pelvis
  vtkGetVector3Macro(Center,float);

  vtkGetObjectMacro(AcetabularPlane,vtkPlaneSource);

  vtkGetObjectMacro(LowerBoundaryFilter,vtkTransformPolyDataFilter);

  vtkGetObjectMacro(LowerBoundarySource,vtkCylinderSource);


 // converts the representation of the frontal axis into 
 // the representation via FrontalAxis, ensures that all axes
 // are orthogonal and computes the inclination and anteversion angles
 // for the current set of axes and acetabular plane
  void UpdateAndNormalize();

 // Update the members so that they fulfill some properties.
 // this faciliates the computation of the anteversion and inclination
  void Normalize();

 protected:
  vtkPelvisMetric();
  ~vtkPelvisMetric();

 private:
  vtkPelvisMetric(vtkPelvisMetric&);
  void operator=(const vtkPelvisMetric);

 // representation of the acetabular plane
  vtkPlaneSource* AcetabularPlane;

 // graphical representation of the frontal axis
  vtkCylinderSource* LowerBoundarySource;
  vtkTransformPolyDataFilter* LowerBoundaryFilter;
  vtkTransform* LowerBoundaryTransform;

 // model representing the pelvis
  vtkPolyData* Pelvis;

 // center of gravity of Pelvis
  float* Center;

 //the three axis vectors are orthogonal to each other
  float* FrontalAxis;
 // always set to (0,1,0)
  float* SagittalAxis;
// Crossproduct of FrontalAxis and SagittalAxis
  float* LongitudinalAxis;

 // convenience function for computing the angle between to vectors
  float Angle(float* a,float* b);
  float InclinationAngle;
  float AnteversionAngle;
 
 // convenience function so that the transform translates and rotates
 // the input vtkCylinderSource to the given location and orientation
  void SetAxis(vtkTransform*,float,float,float,float,float,float);
};

#endif

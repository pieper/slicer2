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
#include <vtkPolyData.h>
#include "vtkAxisSource.h"
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
// The user has three options for coordinate systems within anteversion and
// inclination are computed:
//  - the global coordinate system (== world coordinate system)
//  - the object coordinate system ( determined by the principal axes). This
//    is the only csys which yields the same results under translation and
//    rotation of the same hip.
//  - a symmetry adapted coordinate system. This is what an physician uses on an x-ray:
//    connect two symmetric points on the hip and compute the inclination
//    based on that line. the x-axis of this csys is the symmetry axis of
//    the hip. Basically this corrects rotation along the y-axis in the way
//    a physician corrects it. Use this one. 
//
//
// One thing is clearly missing: A way to compute the z-axis (== body axis)
// solely based on the hip. That would allow to define a csys which is on the
// one hand acceptable for a physician and on the other hand rotational and
// translational invariant.

class VTK_MORPHOMETRICS_EXPORT vtkPelvisMetric : public vtkObject
{
 public:
  static vtkPelvisMetric* New();
  void Delete();
  vtkTypeMacro(vtkPelvisMetric,vtkObject);
  void PrintSelf();

  vtkGetMacro(InclinationAngle,vtkFloatingPointType);
  vtkGetMacro(AnteversionAngle,vtkFloatingPointType);

  void SetPelvis(vtkPolyData*);
  vtkGetObjectMacro(Pelvis,vtkPolyData);
  vtkGetObjectMacro(WorldToObject,vtkTransform);

 // center of gravity of the member Pelvis
  vtkGetVector3Macro(Center,vtkFloatingPointType);

  vtkGetObjectMacro(AcetabularPlane,vtkPlaneSource);

  // Update the members so that they fulfill some properties as well as 
  // computes the derived values.
  void Normalize();

 // the following three functions set the coordinate system in which the inclination and the anteversion are
 // computed. The ObjectCsys is the only one which computes the same results if the pelvis is rotated. But 
 // the coordinate system is not necessarily the same a physician uses. WorldCsys does not adjustment, while
 // the SymmetryAdaptedWorldCsys adjusts is x-axis (in vtk this is from left to right from a patient POV) to
 // be the symmetry axis. This adjustment is done by a physician by connecting two symmetric points of the 
 // pelvis in a x-ray. 
  void WorldCsys(void);
  void ObjectCsys(void);
  void SymmetryAdaptedWorldCsys(void);

 protected:
  vtkPelvisMetric();
  ~vtkPelvisMetric();

 private:
  vtkPelvisMetric(vtkPelvisMetric&);
  void operator=(const vtkPelvisMetric);

 // representation of the acetabular plane
  vtkPlaneSource* AcetabularPlane;

 // model representing the pelvis
  vtkPolyData* Pelvis;

 // center of gravity of Pelvis
  vtkFloatingPointType* Center;

  vtkFloatingPointType InclinationAngle;
  vtkFloatingPointType AnteversionAngle;

  // recompute the angles
  void UpdateAngles();
  vtkFloatingPointType Angle(vtkFloatingPointType* Direction,vtkFloatingPointType* n);

  void NormalizeXAxis(vtkFloatingPointType* n);

  // transformation matrix from world coordinates to current object coordinate system
  vtkTransform* WorldToObject;

};

#endif

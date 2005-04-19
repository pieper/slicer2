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
#ifndef __vtk_femur_metric_h
#define __vtk_femur_metric_h
#include <vtkMorphometricsConfigure.h>
#include <vtkObject.h>
#include <vtkPlaneSource.h>
#include <vtkSphereSource.h>
#include <vtkPolyData.h>
#include <vtkImageEuclideanDistance.h>
#include "vtkAxisSource.h"
#include "vtkDataSetTriangleFilter.h"
#include "vtkDataSetToLabelMap.h"
#include "vtkImageDijkstra.h"

//---------------------------------------------------------
// Author: Axel Krauth
//
// This class represents the basic and derived geometric
// properties of a thigh bone. The basic properties are
// an approximating sphere for the head, an axis for the
// neck as well as for the shaft. The only derived property
// at the moment is the angle between the neck and the shaft axis
//
// To be able to compute the basic properties, a distance-to-surface
// map is computed.

class VTK_MORPHOMETRICS_EXPORT vtkFemurMetric : public vtkObject
{
 public:
  static vtkFemurMetric* New();
  void Delete();
  vtkTypeMacro(vtkFemurMetric,vtkObject);
  void PrintSelf();

  vtkGetObjectMacro(Femur,vtkPolyData);
  void SetFemur(vtkPolyData*);

  vtkGetMacro(NeckShaftAngle,vtkFloatingPointType);

 // representation of the approximation of the head sphere
  vtkGetObjectMacro(HeadSphere,vtkSphereSource);

  vtkGetObjectMacro(NeckAxis,vtkAxisSource);

  vtkGetObjectMacro(ShaftAxis,vtkAxisSource);

  vtkGetVector3Macro(HeadCenter,vtkFloatingPointType);
  vtkSetVector3Macro(HeadCenter,vtkFloatingPointType);

  vtkGetVector3Macro(NeckShaftCenter,vtkFloatingPointType);
  vtkSetVector3Macro(NeckShaftCenter,vtkFloatingPointType);

  vtkGetVector3Macro(DistalPoint,vtkFloatingPointType);
  vtkSetVector3Macro(DistalPoint,vtkFloatingPointType);
 
 // ensure that the geometry fulfills some properties, i.e. the head
 // of femur is in the halfspace specified by the NeckShaftPlane
  void Normalize();

  void ComputeNeckShaftAngle();

  void Precompute();

  void FittNeckAxis() {FittAxis(NeckAxis,HeadCenter,NeckShaftCenter);};

  void FittShaftAxis(){FittAxis(ShaftAxis,NeckShaftCenter,DistalPoint);};
 protected:
  vtkFemurMetric();
  ~vtkFemurMetric();

 private:
  vtkFemurMetric(vtkFemurMetric&);
  void operator=(const vtkFemurMetric);

  vtkSphereSource* HeadSphere;
  
  vtkAxisSource* NeckAxis;

  vtkAxisSource* ShaftAxis;

  vtkPolyData* Femur;

  vtkFloatingPointType NeckShaftAngle;

  vtkFloatingPointType* HeadCenter;
 
  vtkFloatingPointType* NeckShaftCenter;

  vtkFloatingPointType* DistalPoint;

  vtkDataSetTriangleFilter* TriangledFemur;

  vtkDataSetToLabelMap* Volume;

  vtkImageEuclideanDistance* DepthAnnotatedVolume;

  vtkImageDijkstra* Dijkstra;

  void FittAxis(vtkAxisSource*,vtkFloatingPointType* source,vtkFloatingPointType* sink);
  void FindPoints();
  void FindDeepestPoint(vtkFloatingPointType*);
  void FindNearestInside(int* p);

  bool IsInsideVolume(int* p){return IsInsideVolume(p[0],p[1],p[2]);};
#if (VTK_MAJOR_VERSION == 4 && VTK_MINOR_VERSION >= 3)
  bool IsInsideVolume(int x,int y, int z){return 2== ((int)(Volume->GetOutput()->GetScalarComponentAsDouble(x,y,z,0)));};
#else
  bool IsInsideVolume(int x,int y, int z){return 2== ((int)(Volume->GetOutput()->GetScalarComponentAsFloat(x,y,z,0)));};
#endif
};

#endif

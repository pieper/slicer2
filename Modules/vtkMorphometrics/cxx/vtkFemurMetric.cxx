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
#include "vtkFemurMetric.h"
#include <vtkObjectFactory.h>
#include <vtkMath.h>

vtkFemurMetric* vtkFemurMetric::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkFemurMetric")
;
  vtkFemurMetric* result;
  if(ret)
    {
      result =  (vtkFemurMetric*)ret;
    }
  else
    {
      result = new vtkFemurMetric();
    }
  return result;
}

void vtkFemurMetric::Delete()
{
  delete this;
}

void vtkFemurMetric::PrintSelf()
{

}

// defaults to the result values of a left femur for one data set I have
vtkFemurMetric::vtkFemurMetric()
{
  HeadSphere = vtkSphereSource::New();
  HeadNeckPlane = vtkPlaneSource::New();
  NeckShaftPlane = vtkPlaneSource::New();
  UpperShaftEndPlane = vtkPlaneSource::New();
  LowerShaftEndPlane = vtkPlaneSource::New();
  Femur = NULL;
  NeckAxis = vtkAxisSource::New();
  ShaftAxis = vtkAxisSource::New();

  // defaults:
  HeadNeckPlane->SetOrigin(0,0,0);
  HeadNeckPlane->SetPoint1(100,0,0);
  HeadNeckPlane->SetPoint2(0,100,0);
  HeadNeckPlane->SetCenter(-110.685,-10.4339,63.9478);
  HeadNeckPlane->SetNormal(0.542664,0.238796,0.805291);

  NeckShaftPlane->SetOrigin(0,0,0);
  NeckShaftPlane->SetPoint1(100,0,0);
  NeckShaftPlane->SetPoint2(0,100,0);
  NeckShaftPlane->SetCenter(-120.108,-21.5211,59.3707);
  NeckShaftPlane->SetNormal(0.606775,0.588549,0.534261);

  UpperShaftEndPlane->SetOrigin(0,0,0);
  UpperShaftEndPlane->SetPoint1(100,0,0);
  UpperShaftEndPlane->SetPoint2(0,100,0);
  UpperShaftEndPlane->SetCenter(-130,-20,-5);
  UpperShaftEndPlane->SetNormal(-1,0,5);

  LowerShaftEndPlane->SetOrigin(0,0,0);
  LowerShaftEndPlane->SetPoint1(100,0,0);
  LowerShaftEndPlane->SetPoint2(0,100,0);
  LowerShaftEndPlane->SetCenter(-105,-15,-155);
  LowerShaftEndPlane->SetNormal(-1,0,7);

  NeckAxis->SetCenter(-113.749,-5.89667,52.48);
  NeckAxis->SetDirection(0.643935,0.458941,0.612144);

  ShaftAxis->SetCenter(-116.32,-14.9477,-76.2995);
  ShaftAxis->SetDirection(0.107772,0.06512,-0.992041);

  HeadSphere->SetCenter(-105,-5,70);
  HeadSphere->SetRadius(24);
  HeadSphere->SetThetaResolution(30);
  HeadSphere->SetPhiResolution(30);

}

vtkFemurMetric::~vtkFemurMetric()
{
  HeadSphere->Delete();
  HeadNeckPlane->Delete();
  NeckShaftPlane->Delete();
  UpperShaftEndPlane->Delete();
  LowerShaftEndPlane->Delete();
  NeckAxis->Delete();
  ShaftAxis->Delete();
}

void vtkFemurMetric::Execute()
{
  //
}


// ensure that
// - the normal of HeadNeckPlane looks towards the head
// - the normal of NeckShaftPlane looks towards the head
// - the normal of UpperShaftEndPlane looks away from the tuberculum major
// - the normal of LowerShaftEndPlane looks towards the tuberculum major
// - the neck axis points from neck towards head
// - the shaft axis points from the upper end towards the lower end of the shaft
void vtkFemurMetric::Normalize()
{
  float* n;
  float* x0;
  float* x1;

  // Ensure Normal of HeadNeckPlane looks towards head
  // implemented via: Center of NeckShaftPlane not in halfspace
  n = HeadNeckPlane->GetNormal();
  x0= HeadNeckPlane->GetCenter();
  x1= NeckShaftPlane->GetCenter();
 
  if(!(vtkMath::Dot(n,x0) > vtkMath::Dot(n,x1))) 
    {
      HeadNeckPlane->SetNormal(-1*n[0],-1*n[1],-1*n[2]);
    }

  // Ensure Normal of NeckShaftPlane looks towards head
  // implemented via: Center of HeadNeckPlane in halfspace
  n = NeckShaftPlane->GetNormal();
  x0= NeckShaftPlane->GetCenter();
  x1= HeadNeckPlane->GetCenter();

  if(!(vtkMath::Dot(n,x0) < vtkMath::Dot(n,x1))) 
    {
      NeckShaftPlane->SetNormal(-1*n[0],-1*n[1],-1*n[2]);
    }


  // Ensure Normal of UpperShaftEndPlane looks away from tuberculum major
  // implemented via: Center of LowerShaftEndPlane in halfspace
  n = UpperShaftEndPlane->GetNormal();
  x0= UpperShaftEndPlane->GetCenter();
  x1= LowerShaftEndPlane->GetCenter();

  if(!(vtkMath::Dot(n,x0) < vtkMath::Dot(n,x1))) 
    {
      UpperShaftEndPlane->SetNormal(-n[0],-n[1],-n[2]);
    }
  
  // Ensure Normal of LowerShaftEndPlane looks towards tuberculum major
  // implemented via: Center of UpperShaftEndPlane in halfspace
  n = LowerShaftEndPlane->GetNormal();
  x0= LowerShaftEndPlane->GetCenter();
  x1= UpperShaftEndPlane->GetCenter();

  if(!(vtkMath::Dot(n,x0) < vtkMath::Dot(n,x1))) 
    {
      LowerShaftEndPlane->SetNormal(-n[0],-n[1],-n[2]);
    }

  // - the axis of the neck axis points from neck towards head
  // implemented: we simply ensure that the center of the 
  // HeadNeckPlane is inside the halfspace defined by the direction and the center of the axis

  n  = NeckAxis->GetDirection();
  x0 = NeckAxis->GetCenter();
  x1 = HeadNeckPlane->GetCenter();

  if(!(vtkMath::Dot(n,x0) < vtkMath::Dot(n,x1)))
    {
      NeckAxis->SetDirection(-n[0],-n[1],-n[2]);
    }

  // - the axis of the shaft axis points from the upper end towards the lower end of the shaft
  // implemented: we simply ensure that the center of the 
  // LowerShaftEndPlane is inside the halfspace defined by the direction and the center of the axis


  n  = ShaftAxis->GetDirection();
  x0 = ShaftAxis->GetCenter();
  x1 = LowerShaftEndPlane->GetCenter();

  if(!(vtkMath::Dot(n,x0) < vtkMath::Dot(n,x1)))
    {
      ShaftAxis->SetDirection(-n[0],-n[1],-n[2]);
    }

}

// NeckShaftAngle == angle between neck axis and shaft axis
void vtkFemurMetric::ComputeNeckShaftAngle()
{
  // ensure everthing is correct set up, especially that the axis are pointing in the correct direction
  Normalize();
  NeckShaftAngle = ShaftAxis->vtkAxisSource::Angle(NeckAxis);
  
}

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
#include <vtkCommand.h>
#include <vtkMath.h>

#include <iostream>

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
  NeckAxisSource = vtkCylinderSource::New();
  NeckAxisFilter = vtkTransformPolyDataFilter::New();
  NeckAxisTransform = vtkTransform::New();
  ShaftAxisSource = vtkCylinderSource::New();
  ShaftAxisFilter = vtkTransformPolyDataFilter::New();
  ShaftAxisTransform = vtkTransform::New();


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

  // make the axis visible
  NeckAxisSource->SetResolution(30);
  NeckAxisSource->SetRadius(3);
  NeckAxisSource->SetHeight(400);
  // put the pipeline together
  NeckAxisFilter->SetInput(NeckAxisSource->GetOutput());
  NeckAxisFilter->SetTransform(NeckAxisTransform);
  SetAxis(NeckAxisTransform,-113.749,-5.89667,52.48,0.643935,0.458941,0.612144);


  // make the axis visible
  ShaftAxisSource->SetResolution(30);
  ShaftAxisSource->SetRadius(3);
  ShaftAxisSource->SetHeight(400);
  // put the pipeline together
  ShaftAxisFilter->SetInput(ShaftAxisSource->GetOutput());
  ShaftAxisFilter->SetTransform(ShaftAxisTransform);
  SetAxis(ShaftAxisTransform,-116.32,-14.9477,-76.2995,0.107772,0.06512,-0.992041);

  HeadSphere->SetCenter(-105,-5,70);
  HeadSphere->SetRadius(24);
  HeadSphere->SetThetaResolution(30);
  HeadSphere->SetPhiResolution(30);

}

void vtkFemurMetric::SetAxis(vtkTransform* t,float x0_x,float x0_y,float x0_z,float dir_x,float dir_y,float dir_z)
{
  float norm =  sqrt(dir_x*dir_x + dir_y*dir_y + dir_z*dir_z);
  dir_x = dir_x / norm;
  dir_y = dir_y / norm;
  dir_z = dir_z / norm;

  dir_x = dir_x / 2;
  dir_y = (1+dir_y) / 2;
  dir_z = dir_z / 2;

  norm =  sqrt(dir_x*dir_x + dir_y*dir_y + dir_z*dir_z);
  dir_x = dir_x / norm;
  dir_y = dir_y / norm;
  dir_z = dir_z / norm;


  t->Identity();
  t->RotateWXYZ(180,dir_x,dir_y,dir_z);
  t->PostMultiply();
  t->Translate(x0_x,x0_y,x0_z);
}

vtkFemurMetric::~vtkFemurMetric()
{
  HeadSphere->Delete();
  HeadNeckPlane->Delete();
  NeckShaftPlane->Delete();
  UpperShaftEndPlane->Delete();
  LowerShaftEndPlane->Delete();
  NeckAxisSource->Delete();
  NeckAxisFilter->Delete();
  NeckAxisTransform->Delete();
  ShaftAxisSource->Delete();
  ShaftAxisFilter->Delete();
  ShaftAxisTransform->Delete();
}

vtkFemurMetric::vtkFemurMetric(vtkFemurMetric&)
{

}

void vtkFemurMetric::operator=(const vtkFemurMetric)
{

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
// - the axis of the neck axis points from neck towards head
// - the axis of the shaft axis points from the upper end towards the lower end of the shaft
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
      UpperShaftEndPlane->SetNormal(-1*n[0],-1*n[1],-1*n[2]);
    }
  
  // Ensure Normal of LowerShaftEndPlane looks towards tuberculum major
  // implemented via: Center of UpperShaftEndPlane in halfspace
  n = LowerShaftEndPlane->GetNormal();
  x0= LowerShaftEndPlane->GetCenter();
  x1= UpperShaftEndPlane->GetCenter();

  if(!(vtkMath::Dot(n,x0) < vtkMath::Dot(n,x1))) 
    {
      LowerShaftEndPlane->SetNormal(-1*n[0],-1*n[1],-1*n[2]);
    }

  // - the axis of the neck axis points from neck towards head
  // implemented: we simply ensure that the center of the 
  // HeadNeckPlane is inside the halfspace defined by the direction and the center of the axis

  n  = CylinderDirection(NeckAxisTransform);
  x0 = NeckAxisTransform->GetPosition();
  x1 = HeadNeckPlane->GetCenter();

  if(!(vtkMath::Dot(n,x0) < vtkMath::Dot(n,x1)))
    {
      SetAxis(NeckAxisTransform,x0[0],x0[1],x0[2],-n[0],-n[1],-n[2]);
    }

  free(n);

  // - the axis of the shaft axis points from the upper end towards the lower end of the shaft
  // implemented: we simply ensure that the center of the 
  // LowerShaftEndPlane is inside the halfspace defined by the direction and the center of the axis


  n  = CylinderDirection(ShaftAxisTransform);
  x0 = ShaftAxisTransform->GetPosition();
  x1 = LowerShaftEndPlane->GetCenter();

  if(!(vtkMath::Dot(n,x0) < vtkMath::Dot(n,x1)))
    {
      SetAxis(ShaftAxisTransform,x0[0],x0[1],x0[2],-n[0],-n[1],-n[2]);
    }

  free(n);
}

float* vtkFemurMetric::CylinderDirection(vtkTransform* t)
{
  float* dir_v   = (float*) malloc(4*sizeof(float));
  float* result  = (float*) malloc(4*sizeof(float));

  dir_v[0] = 0;
  dir_v[1] = 1;
  dir_v[2] = 0;
  dir_v[3] = 0;

  t->GetMatrix()->MultiplyPoint(dir_v, result);
  vtkMath::Normalize(result);

  free(dir_v);
  return result;
}

float vtkFemurMetric::Angle(float* a,float* b)
{
  float normA = vtkMath::Norm(a);
  float normB = vtkMath::Norm(b);
  float AdotB = vtkMath::Dot(a,b);
  
  float angle = acos(AdotB / (normA*normB));
  return angle*vtkMath::RadiansToDegrees();
}

void vtkFemurMetric::ComputeNeckShaftAngle()
{
  // NeckShaftAngle == angle between neck axis and shaft axis

  // ensure everthing is correct set up, especially that the axis are pointing in the correct direction
  Normalize();
  
  float* dir_shaft = CylinderDirection(ShaftAxisTransform);
  float* dir_neck  = CylinderDirection(NeckAxisTransform);

  NeckShaftAngle = Angle(dir_shaft,dir_neck);

  free(dir_neck);
  free(dir_shaft);
}

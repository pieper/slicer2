/*=auto=========================================================================
                     [[Pelvis GetWorldToObject] GetMatrix] Print
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
#include "vtkPelvisMetric.h"
#include <vtkObjectFactory.h>
#include <vtkMath.h>
#include "vtkPrincipalAxes.h"
#include <vtkTransform.h>
#include <vtkMatrix4x4.h>

vtkPelvisMetric* vtkPelvisMetric::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkPelvisMetric")
;
  vtkPelvisMetric* result;
  if(ret)
    {
      result =  (vtkPelvisMetric*)ret;
    }
  else
    {
      result = new vtkPelvisMetric();
    }
  return result;
}

void vtkPelvisMetric::Delete()
{
  delete this;
}

void vtkPelvisMetric::PrintSelf()
{

}

vtkPelvisMetric::vtkPelvisMetric()
{
  AcetabularPlane = vtkPlaneSource::New();
  Pelvis = NULL;

  // defaults:
  AcetabularPlane->SetOrigin(0,0,0);
  AcetabularPlane->SetPoint1(100,0,0);
  AcetabularPlane->SetPoint2(0,100,0);
  AcetabularPlane->SetCenter(-88.6134,-4.64934,87.0443);
  AcetabularPlane->SetNormal(-0.721505,0.320132,-0.613959);

  Center = (float*)malloc(3*sizeof(float)); 
  Center[0] = 0;
  Center[1] = 0;
  Center[2] = 0;

  //FrontalAxis  = vtkAxisSource::New();
  //FrontalAxis->SetDirection(1,0,0);

  //SagittalAxis  = vtkAxisSource::New();
  //SagittalAxis->SetDirection(0,1,0);

  //LongitudinalAxis  = vtkAxisSource::New();
  //LongitudinalAxis->SetDirection(0,0,1);

  InclinationAngle = 45;
  AnteversionAngle = 45;

  WorldToObject = vtkTransform::New();

  Normalize();

}

void vtkPelvisMetric::SetPelvis(vtkPolyData* newPelvis)
{
  if(newPelvis==NULL )
    return;

  if(newPelvis==Pelvis)
    return;

  Pelvis = newPelvis;

  // compute the center since this depends solely on the polyData
  Center[0] = 0;
  Center[1] = 0;
  Center[2] = 0;

  for(vtkIdType i=0;i<Pelvis->GetNumberOfPoints();i++)
    {
      Center[0] += Pelvis->GetPoint(i)[0];
      Center[1] += Pelvis->GetPoint(i)[1];
      Center[2] += Pelvis->GetPoint(i)[2];
    }

  Center[0] = Center[0] / Pelvis->GetNumberOfPoints();
  Center[1] = Center[1] / Pelvis->GetNumberOfPoints();
  Center[2] = Center[2] / Pelvis->GetNumberOfPoints();

  WorldCsys();

  Modified();
}

void vtkPelvisMetric::Normalize()
{
  // ensure that the different direction vector look in certain directions.
  // those are invariants needed for computing the angles.

  // acetabular plane normal not pointing towards center of gravity
  float p_acetabulum = vtkMath::Dot(AcetabularPlane->GetCenter(),AcetabularPlane->GetNormal());
  float p_center = vtkMath::Dot(Center,AcetabularPlane->GetNormal());
  if(p_center>p_acetabulum)
    {
      float* normal = AcetabularPlane->GetNormal();
      
      for(int i=0;i<3;i++)
    normal[i] = -normal[i];
      AcetabularPlane->SetNormal(normal);
    }
  
  // OBS! this invariant is also enforced in NormalizeXAxis
  // center of actabular plane in FrontalAxis halfspace
  float* frontalAxisDirection = WorldToObject->TransformFloatNormal(1,0,0);
  p_acetabulum = vtkMath::Dot(AcetabularPlane->GetCenter(),frontalAxisDirection);
  p_center = vtkMath::Dot(Center,frontalAxisDirection);
  if(p_acetabulum<p_center)
    {
      vtkMatrix4x4* m = WorldToObject->GetMatrix();
      for(int i=0;i<3;i++)
    m->SetElement(i,0,-1*m->GetElement(i,0));
    }


  UpdateAngles();
  Modified();
}

// also done for the WorldToObject transformation in Normalize();
void vtkPelvisMetric::NormalizeXAxis(float* n)
{
  float p_acetabulum = vtkMath::Dot(AcetabularPlane->GetCenter(),n);
  float p_center = vtkMath::Dot(Center,n);
  if(p_acetabulum<p_center)
    {
      for(int i=0;i<3;i++)
    n[i] *= -1;
    }
}

float vtkPelvisMetric::Angle(float* n,float* Direction)
{
  float angle = acos(vtkMath::Dot(Direction,n) / vtkMath::Norm(n));
  return angle*vtkMath::RadiansToDegrees();
}


void vtkPelvisMetric::UpdateAngles()
{
  float* normal_in_obj = WorldToObject->TransformFloatNormal(AcetabularPlane->GetNormal());
  float* reference_n = (float*) malloc(3*sizeof(float));

  for(int i = 0;i<3;i++)
    reference_n[i] = 0;
  reference_n[0] = 1;

  // Inclination : project normal_in_obj onto x-z-Plane
  normal_in_obj[1]= 0;

  vtkMath::Normalize(normal_in_obj);

  InclinationAngle = 90 - Angle(reference_n,normal_in_obj);

  // Clean up of inclination computation
  normal_in_obj = WorldToObject->TransformFloatNormal(AcetabularPlane->GetNormal());

  // Anteversion
  normal_in_obj[2]= 0;

  vtkMath::Normalize(normal_in_obj);

  AnteversionAngle = Angle(reference_n,normal_in_obj);
  
  free(reference_n);
}

vtkPelvisMetric::~vtkPelvisMetric()
{
  AcetabularPlane->Delete();

  free(Center);

  WorldToObject->Delete();
}


void vtkPelvisMetric::WorldCsys(void)
{
  WorldToObject->Identity();
  WorldToObject->Translate(-Center[0],-Center[1],-Center[2]);
  Normalize();
}

void vtkPelvisMetric::ObjectCsys(void)
{
  WorldToObject->Identity();

  vtkPrincipalAxes* vPA = vtkPrincipalAxes::New();
  vPA->SetInput(Pelvis);
  vPA->Update();

  vtkMatrix4x4* obj = WorldToObject->GetMatrix();

  for(int j=0;j<3;j++)
    {
      obj->SetElement(0,j,vPA->GetXAxis()[j]);
      obj->SetElement(1,j,vPA->GetYAxis()[j]);
      obj->SetElement(2,j,vPA->GetZAxis()[j]);
    }
  
  WorldToObject->PostMultiply();
  WorldToObject->Translate(-Center[0],-Center[1],-Center[2]);
  Normalize();
}

void vtkPelvisMetric::SymmetryAdaptedWorldCsys(void)
{
  WorldToObject->Identity();

  vtkPrincipalAxes* vPA = vtkPrincipalAxes::New();
  vPA->SetInput(Pelvis);
  vPA->Update();

  vtkMatrix4x4* obj = WorldToObject->GetMatrix();

  // write the symmetry axis - the one with the smallest angle to (1,0,0) - into the first column 
  float* axis = (float*)malloc(3*sizeof(float));

  axis[0] = 1;
  axis[1] = 0;
  axis[2] = 0;
  NormalizeXAxis(axis);

  float* candidate = vPA->GetXAxis();
  NormalizeXAxis(candidate);
  for(int i =0;i<3;i++)
    obj->SetElement(i,0,candidate[i]);

  float distance = vtkMath::Distance2BetweenPoints(candidate,axis);

  candidate = vPA->GetYAxis();
  NormalizeXAxis(candidate);
  if(vtkMath::Distance2BetweenPoints(candidate,axis) < distance)
    {
      distance = vtkMath::Distance2BetweenPoints(candidate,axis);
      for(int i =0;i<3;i++)
    obj->SetElement(i,0,candidate[i]);
    } 

  candidate = vPA->GetZAxis();
  NormalizeXAxis(candidate);
  if(vtkMath::Distance2BetweenPoints(candidate,axis) < distance)
    {
      distance = vtkMath::Distance2BetweenPoints(candidate,axis);
      for(int i =0;i<3;i++)
    obj->SetElement(i,0,candidate[i]);
    } 

  // projection of (0,1,0) onto the plane orthogonal to the symmetry axis
  for(int i =0;i<3;i++)
    candidate[i] = obj->GetElement(i,0);

  axis[0] = 0;
  axis[1] = 1;
  axis[2] = 0;

  float p = vtkMath::Dot(candidate,axis);
  for(int i = 0;i < 3;i++)
    axis[i] = axis[i] - p*candidate[i];
  vtkMath::Normalize(axis);

  for(int i = 0;i < 3;i++)
    obj->SetElement(i,1,axis[i]);

  // the last vector is automatically the crossproduct of the first two.
  float* third = (float*)malloc(3*sizeof(float));
  vtkMath::Cross(candidate,axis,third);

  for(int i = 0;i < 3;i++)
    obj->SetElement(i,2,third[i]);

  free(axis);
  free(third);

  WorldToObject->PostMultiply();
  WorldToObject->Translate(-Center[0],-Center[1],-Center[2]);
  Normalize();
              
}

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
#include "vtkPelvisMetric.h"
#include <vtkObjectFactory.h>
#include <vtkCommand.h>
#include <vtkMath.h>

#include <iostream>

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

  LowerBoundarySource = vtkCylinderSource::New();
  LowerBoundaryFilter = vtkTransformPolyDataFilter::New();
  LowerBoundaryTransform = vtkTransform::New();


  // make the axis visible
  LowerBoundarySource->SetResolution(30);
  LowerBoundarySource->SetRadius(3);
  LowerBoundarySource->SetHeight(400);
  // put the pipeline together
  LowerBoundaryFilter->SetInput(LowerBoundarySource->GetOutput());
  LowerBoundaryFilter->SetTransform(LowerBoundaryTransform);
  SetAxis(LowerBoundaryTransform,4.65723,-5.89667,9.52892,1,0,0);


  Center = (float*)malloc(3*sizeof(float)); 
  Center[0] = 0;
  Center[1] = 0;
  Center[2] = 0;

  FrontalAxis  = (float*)malloc(3*sizeof(float)); 
  FrontalAxis[0] = 1;
  FrontalAxis[1] = 0;
  FrontalAxis[2] = 0;

  SagittalAxis  = (float*)malloc(3*sizeof(float)); 
  SagittalAxis[0] = 0;
  SagittalAxis[1] = 1;
  SagittalAxis[2] = 0;

  LongitudinalAxis  = (float*)malloc(3*sizeof(float)); 
  LongitudinalAxis[0] = 0;
  LongitudinalAxis[1] = 0;
  LongitudinalAxis[2] = 1;

  InclinationAngle = 0;
  AnteversionAngle = 0;
}

void vtkPelvisMetric::SetPelvis(vtkPolyData* newPelvis)
{
  if(newPelvis==NULL)
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
  
  Modified();
}

void vtkPelvisMetric::UpdateAndNormalize()
{
  // update FrontalAxis and LongitudinalAxis
  // FrontalAxis = y-Axis of LowerBoundaryTransform
  FrontalAxis[0] = 0;
  FrontalAxis[1] = 1;
  FrontalAxis[2] = 0;

  // FrontalAxis is now the direction of LowerBoundaryTransform
  LowerBoundaryTransform->TransformNormal(FrontalAxis,FrontalAxis);

  // make FrontalAxis orthogonal to the Sagittal axis
  float frontalSagittalP = vtkMath::Dot(FrontalAxis,SagittalAxis);
  for(int i=0;i<3;i++)
    FrontalAxis[i] = FrontalAxis[i] - frontalSagittalP*SagittalAxis[i];

  SetAxis(LowerBoundaryTransform,LowerBoundaryTransform->GetPosition()[0],LowerBoundaryTransform->GetPosition()[1],LowerBoundaryTransform->GetPosition()[2],FrontalAxis[0],FrontalAxis[1],FrontalAxis[2]);

  // ensure the longitudinal is orthogonal to both axes
  vtkMath::Cross(FrontalAxis,SagittalAxis,LongitudinalAxis);

  // Normalization necessary for computation of angles
  Normalize();

  // recompute Angles

  float* normalProjected = (float*) malloc(3*sizeof(float));
  for(int i=0;i<3;i++)
    normalProjected[i] = AcetabularPlane->GetNormal()[i];

  float normalP = vtkMath::Dot(normalProjected,SagittalAxis);
  for(int i=0;i<3;i++)
    normalProjected[i] = normalProjected[i] - normalP*SagittalAxis[i];

  InclinationAngle = 90 - Angle(FrontalAxis,normalProjected);
  
  
  for(int i=0;i<3;i++)
    normalProjected[i] = AcetabularPlane->GetNormal()[i];

  normalP = vtkMath::Dot(normalProjected,LongitudinalAxis);
  for(int i=0;i<3;i++)
    normalProjected[i] = normalProjected[i] - normalP*LongitudinalAxis[i];

  AnteversionAngle = Angle(normalProjected,FrontalAxis);

  free(normalProjected);
  Modified();
}

vtkPelvisMetric::~vtkPelvisMetric()
{
  AcetabularPlane->Delete();
  free(Center);
  free(FrontalAxis);
  free(SagittalAxis);
  free(LongitudinalAxis);
}

float vtkPelvisMetric::Angle(float* a,float* b)
{
  float normA = vtkMath::Norm(a);
  float normB = vtkMath::Norm(b);
  float AdotB = vtkMath::Dot(a,b);
  
  float angle = acos(AdotB / (normA*normB));
  return angle*vtkMath::RadiansToDegrees();
}

vtkPelvisMetric::vtkPelvisMetric(vtkPelvisMetric&)
{

}

void vtkPelvisMetric::operator=(const vtkPelvisMetric)
{

}

void vtkPelvisMetric::Normalize()
{
  vtkMath::Normalize(SagittalAxis);
  vtkMath::Normalize(FrontalAxis);
  vtkMath::Normalize(LongitudinalAxis);

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
  

  // center of actabular plane in frontalAxis halfspace
  p_acetabulum = vtkMath::Dot(AcetabularPlane->GetCenter(),FrontalAxis);
  p_center = vtkMath::Dot(Center,FrontalAxis);
  if(p_acetabulum<p_center)
    {
      for(int i=0;i<3;i++)
    FrontalAxis[i] *= -1;
      SetAxis(LowerBoundaryTransform,LowerBoundaryTransform->GetPosition()[0],LowerBoundaryTransform->GetPosition()[1],LowerBoundaryTransform->GetPosition()[2],FrontalAxis[0],FrontalAxis[1],FrontalAxis[2]);
    }
  
}


void vtkPelvisMetric::SetAxis(vtkTransform* t,float x0_x,float x0_y,float x0_z,float dir_x,float dir_y,float dir_z)
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

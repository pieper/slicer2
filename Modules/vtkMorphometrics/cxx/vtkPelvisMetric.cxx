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
#include <vtkMath.h>

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

  FrontalAxis  = vtkAxisSource::New();
  FrontalAxis->SetDirection(1,0,0);

  SagittalAxis  = vtkAxisSource::New();
  SagittalAxis->SetDirection(0,1,0);

  LongitudinalAxis  = vtkAxisSource::New();
  LongitudinalAxis->SetDirection(0,0,1);

  InclinationAngle = 45;
  AnteversionAngle = 45;
  Normalize();
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

void vtkPelvisMetric::Normalize()
{
  OrthogonalizeAxes();

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
  

  // center of actabular plane in FrontalAxis halfspace
  p_acetabulum = vtkMath::Dot(AcetabularPlane->GetCenter(),FrontalAxis->GetDirection());
  p_center = vtkMath::Dot(Center,FrontalAxis->GetDirection());
  if(p_acetabulum<p_center)
    {
      float* dir= FrontalAxis->GetDirection();
      FrontalAxis->SetDirection(-dir[0],-dir[1],-dir[2]);
    }

  UpdateAngles();
  Modified();
}

void vtkPelvisMetric::OrthogonalizeAxes()
{
  float* FrontalAxisDirection = FrontalAxis->GetDirection();

  // make FrontalAxis orthogonal to the Sagittal axis
  float frontalSagittalP = vtkMath::Dot(FrontalAxis->GetDirection(),SagittalAxis->GetDirection());
  FrontalAxis->SetDirection(FrontalAxisDirection[0] - frontalSagittalP*SagittalAxis->GetDirection()[0],
                FrontalAxisDirection[1] - frontalSagittalP*SagittalAxis->GetDirection()[1],
                FrontalAxisDirection[2] - frontalSagittalP*SagittalAxis->GetDirection()[2]);

  // ensure the longitudinal axis is orthogonal to both axes
  float* longitudinalDirection = (float*)malloc(3*sizeof(float));
  vtkMath::Cross(FrontalAxis->GetDirection(),SagittalAxis->GetDirection(),longitudinalDirection);

  LongitudinalAxis->SetDirection(longitudinalDirection);

  free(longitudinalDirection);
}

void vtkPelvisMetric::UpdateAngles()
{
  float* normalProjected = (float*) malloc(3*sizeof(float));
  for(int i=0;i<3;i++)
    normalProjected[i] = AcetabularPlane->GetNormal()[i];
  
  float normalP = vtkMath::Dot(normalProjected,SagittalAxis->GetDirection());
  for(int i=0;i<3;i++)
    normalProjected[i] = normalProjected[i] - normalP*SagittalAxis->GetDirection()[i];
  
  InclinationAngle = 90 - FrontalAxis->Angle(normalProjected);
  
  
  for(int i=0;i<3;i++)
    normalProjected[i] = AcetabularPlane->GetNormal()[i];
  
  normalP = vtkMath::Dot(normalProjected,LongitudinalAxis->GetDirection());
  for(int i=0;i<3;i++)
    normalProjected[i] = normalProjected[i] - normalP*LongitudinalAxis->GetDirection()[i];
  
  AnteversionAngle = FrontalAxis->Angle(normalProjected);
  
  free(normalProjected);
}

vtkPelvisMetric::~vtkPelvisMetric()
{
  AcetabularPlane->Delete();

  free(Center);

  FrontalAxis->Delete();
  SagittalAxis->Delete();
  LongitudinalAxis->Delete();
}

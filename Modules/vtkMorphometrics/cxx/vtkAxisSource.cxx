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
#include "vtkAxisSource.h"
#include <vtkObjectFactory.h>
#include <vtkMath.h>

void vtkAxisSource::SetDirection(float x,float y,float z)
{
  Direction[0] = x;
  Direction[1] = y;
  Direction[2] = z;

  vtkMath::Normalize(Direction);

  UpdateVisualization();

  Modified();
}
 
void vtkAxisSource::SetDirection(float* d)
{
  SetDirection(d[0],d[1],d[2]);
}

void vtkAxisSource::SetCenter(float x,float y,float z)
{
  Center[0] = x;
  Center[1] = y;
  Center[2] = z;

  UpdateVisualization();

  Modified();
}

void vtkAxisSource::SetCenter(float* p)
{
  SetCenter(p[0],p[1],p[2]);
}

vtkAxisSource* vtkAxisSource::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkAxisSource")
;
  vtkAxisSource* result;
  if(ret)
    {
      result =  (vtkAxisSource*)ret;
    }
  else
    {
      result = new vtkAxisSource();
    }
  return result;
}

void vtkAxisSource::Delete()
{
  delete this;
}

void vtkAxisSource::PrintSelf()
{

}

vtkAxisSource::vtkAxisSource()
{
  Direction = (float*)malloc(3*sizeof(float));
  Center  = (float*)malloc(3*sizeof(float));;

  AxisSource = vtkCylinderSource::New();
  AxisFilter = vtkTransformPolyDataFilter::New();
  AxisTransform = vtkTransform::New();

  // make the axis visible
  AxisSource->SetResolution(30);
  AxisSource->SetRadius(3);
  AxisSource->SetHeight(400);
  // put the pipeline together
  AxisFilter->SetInput(AxisSource->GetOutput());
  AxisFilter->SetTransform(AxisTransform);

  Center[0] = 0;
  Center[1] = 0;
  Center[2] = 0;

  SetDirection(1,0,0);
}

vtkAxisSource::~vtkAxisSource()
{
  free(Direction);
  free(Center);
  AxisSource->Delete();
  AxisTransform->Delete();
  AxisFilter->Delete();
}

void vtkAxisSource::UpdateRepresentation()
{
  double* dir = AxisTransform->TransformNormal(0,1,0);
  float* pos = AxisTransform->GetPosition();
  
  for(int i = 0;i<3;i++)
    {
      Center[i] = pos[i];
      Direction[i] = dir[i];
    }

  vtkMath::Normalize(Direction);
}

// transforming a cylinder to the intended direction is done by rotating the cylinder (which has direction (0,1,0)) around
// the vector halfway between (0,1,0) and the intended direction around 180 degrees.
void vtkAxisSource::UpdateVisualization()
{
  float dir_x = Direction[0];
  float dir_y = Direction[1];
  float dir_z = Direction[2];

  dir_x = dir_x / 2;
  dir_y = (1+dir_y) / 2;
  dir_z = dir_z / 2;

  float norm =  sqrt(dir_x*dir_x + dir_y*dir_y + dir_z*dir_z);
  dir_x = dir_x / norm;
  dir_y = dir_y / norm;
  dir_z = dir_z / norm;

  AxisTransform->Identity();
  AxisTransform->RotateWXYZ(180,dir_x,dir_y,dir_z);
  AxisTransform->PostMultiply();
  AxisTransform->Translate(Center[0],Center[1],Center[2]);
}

void vtkAxisSource::Execute()
{
  vtkPolyData *output = this->GetOutput();

  AxisFilter->Update();
  
  vtkPolyData* input = AxisFilter->GetOutput();

  output->SetPoints(input->GetPoints());
  output->SetPolys(input->GetPolys());
  output->SetStrips(input->GetStrips());
  output->SetLines(input->GetLines());
}

float vtkAxisSource::Angle(float* n)
{
  float angle = acos(vtkMath::Dot(Direction,n) / vtkMath::Norm(n));
  return angle*vtkMath::RadiansToDegrees();
}

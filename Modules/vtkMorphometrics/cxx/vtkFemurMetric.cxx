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
#include <vtkPoints.h>
#include <vtkObjectFactory.h>
#include <vtkMath.h>
#include <vtkEuclideanLineFit.h>

#include <vector>
#include <algorithm>
#include <functional>
#include <numeric>

static float* headCenter = NULL;
static float* add = NULL;

struct less_mag : public std::binary_function<float*, float*, bool> {
  bool operator()(float* x,float* y) 
  {
    for(int i =0;i<3;i++)
      add[i] = x[i] + headCenter[i];
    float a = vtkMath::Dot(add,add);
    for(int i =0;i<3;i++)
      add[i] = y[i] + headCenter[i];
    float b = vtkMath::Dot(add,add);
    return a < b;
  }
};

void vtkFemurMetric::FindPoints()
{
  vtkPoints* p = Femur->GetPoints();
  if(add==NULL)
    add = (float*) malloc (3*sizeof(float));
  if(headCenter == NULL)
    headCenter = (float*) malloc (3*sizeof(float));
  
  for(int i =0;i<3;i++)
    headCenter[i] = - HeadSphere->GetCenter()[i];

  std::vector<float*> V;
  for(int i = 0;i<p->GetNumberOfPoints();i++)
    {
      V.push_back(p->GetPoint(i));
    }
  
  std::sort(V.begin(), V.end(), less_mag());
  float* last = V.back();
  for(int i =0;i<3;i++)
    DistalPoint[i] = V.back()[i];

  std::vector<float> diff;
  
  for(std::vector<float*>::iterator iter = V.begin(); iter != V.end();iter++)
    {
      float* b = *(iter++);
      if(iter == V.end())
    break;
      float* a = *(iter--);
      float a_minus_b = 0;
      for(int i =0;i<3;i++)
    a_minus_b = (a[i] - b[i]) * (a[i] - b[i]);
      diff.push_back(sqrt(a_minus_b));
    }
  std::vector<float>::iterator maxDiff= std::max_element(diff.begin(),diff.end());

  int index = std::distance(diff.begin(),maxDiff);

  for(int i =0;i<3;i++)
    DistalPoint[i] = V.back()[i];

  for(int i =0;i<3;i++)
    NeckShaftCenter[i] = V[index][i];

  for(int i =0;i<3;i++)
    HeadCenter[i] = HeadSphere->GetCenter()[i];
}


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

void vtkFemurMetric::SetFemur(vtkPolyData* newFemur)
{
  if(newFemur==NULL)
    return;

  if(newFemur==Femur)
    return;

  Femur = newFemur;

  if(TriangledFemur != NULL)
    {
      TriangledFemur->Delete();
      Volume->Delete();
      DepthAnnotatedVolume->Delete();
    }

  TriangledFemur = vtkDataSetTriangleFilter::New();
  TriangledFemur->SetInput(Femur);
  TriangledFemur->Update();


  Volume = vtkDataSetToLabelMap::New();
  Volume->SetUseBoundaryVoxels(true);
  Volume->SetInput((vtkDataSet*)TriangledFemur->GetOutput());
  Volume->Update();

  DepthAnnotatedVolume = vtkImageEuclideanDistance::New();
  DepthAnnotatedVolume->SetInput(Volume->GetOutput());
  DepthAnnotatedVolume->Update();


  Modified();
}

void vtkFemurMetric::FittAxis(vtkAxisSource* axis,float* source,float* sink)
{
  vtkPolyData* Path = vtkPolyData::New();
  vtkPoints* PathPoints = vtkPoints::New();
  vtkEuclideanLineFit* PathLine = vtkEuclideanLineFit::New();

  if(Dijkstra!=NULL)
    Dijkstra->Delete();
  Dijkstra = vtkImageDijkstra::New();



  Dijkstra->SetUseInverseDistance(true);
  Dijkstra->SetBoundaryScalars(Volume->GetBoundaryScalars());
  Dijkstra->SetInput(DepthAnnotatedVolume->GetOutput());

  Dijkstra->SetSourceID(DepthAnnotatedVolume->GetOutput()->FindPoint(source));
  Dijkstra->SetSinkID(DepthAnnotatedVolume->GetOutput()->FindPoint(sink));

  Dijkstra->Update();
  Dijkstra->InitTraversePath();
    
  for(int i = 0;i< Dijkstra->GetNumberOfPathNodes();i++)
    {
      PathPoints->InsertNextPoint(DepthAnnotatedVolume->GetOutput()->GetPoint(Dijkstra->GetNextPathNode()));
    }

  Path->SetPoints(PathPoints);

  PathLine->SetInput(Path);
  PathLine->Update();

  axis->SetCenter(PathLine->GetCenter());
  axis->SetDirection(PathLine->GetDirection());

  PathLine->Delete();
  Path->Delete();
  PathPoints->Delete();
}


// defaults to the result values of a left femur for one data set I have
vtkFemurMetric::vtkFemurMetric()
{
  HeadSphere = vtkSphereSource::New();
  Femur = NULL;
  NeckAxis = vtkAxisSource::New();
  ShaftAxis = vtkAxisSource::New();
  TriangledFemur = NULL;
  Volume = NULL;
  DepthAnnotatedVolume = NULL;
  Dijkstra = NULL;

  HeadCenter = (float*)malloc(3*sizeof(float));
 
  NeckShaftCenter  = (float*)malloc(3*sizeof(float));

  DistalPoint  = (float*)malloc(3*sizeof(float));

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
  NeckAxis->Delete();
  ShaftAxis->Delete();
  free(HeadCenter);
  free(NeckShaftCenter);
  free(DistalPoint);

  if(TriangledFemur != NULL)
    {
      TriangledFemur->Delete();
      Volume->Delete();
      DepthAnnotatedVolume->Delete();
      Dijkstra->Delete();
    }
}

// ensure that
// - the neck axis points from neck towards head
// - the shaft axis points from the upper end towards the lower end of the shaft
void vtkFemurMetric::Normalize()
{
  float* n;
  float* x0;
  float* x1;

  // - the axis of the neck axis points from neck towards head
  // implemented: we simply ensure that the center of the 
  // HeadSphere is inside the halfspace defined by the direction and the center of the axis

  n  = NeckAxis->GetDirection();
  x0 = NeckAxis->GetCenter();
  x1 = HeadSphere->GetCenter();

  if(!(vtkMath::Dot(n,x0) < vtkMath::Dot(n,x1)))
    {
      NeckAxis->SetDirection(-n[0],-n[1],-n[2]);
    }

  // - the axis of the shaft axis points from the upper end towards the lower end of the shaft
  // implemented: we simply ensure that the center of the 
  // HeadSphere is not inside the halfspace defined by the direction and the center of the axis


  n  = ShaftAxis->GetDirection();
  x0 = ShaftAxis->GetCenter();
  x1 = HeadSphere->GetCenter();

  if(!(vtkMath::Dot(n,x0) > vtkMath::Dot(n,x1)))
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

void vtkFemurMetric::FindNearestInside(int* p)
{
  int radius = 0;

  while(true)
    {
      for(int i= -radius + p[0];i<= radius + p[0];i++)
    for(int j= -radius + p[1];j<= radius + p[1];j++)
      for(int k= -radius + p[2];k<= radius + p[2];k++)
        if(IsInsideVolume(i,j,k))
          {
        p[0] = i;
        p[1] = j;
        p[2] = k;
        return;
          }
      radius++;
    }
}

void vtkFemurMetric::Precompute()
{
  FindPoints();
  FindDeepestPoint(HeadCenter);
  FindDeepestPoint(NeckShaftCenter);
  FindDeepestPoint(DistalPoint);
  FittShaftAxis();
  FittNeckAxis();
  Normalize();
  ComputeNeckShaftAngle();
}

void vtkFemurMetric::FindDeepestPoint(float* p) 
{
  int* coords = (int*)malloc(3*sizeof(int));
  for(int i=0;i<3;i++)
    coords[i] = (int)( - (Volume->GetOutput()->GetOrigin()[i] - p[i]));


  FindNearestInside(coords);
  
  float currentDepth = DepthAnnotatedVolume->GetOutput()->GetScalarComponentAsFloat(coords[0],coords[1],coords[2],0);
  bool isDeepestPoint = false;

  while(!isDeepestPoint)
    {
      isDeepestPoint = true;
      int currentX = coords[0];
      int currentY = coords[1];
      int currentZ = coords[2];

      for(int i = currentX -1; i <=currentX +1;i++)
    for(int j = currentY -1; j <=currentY +1;j++)
      for(int k = currentZ -1; k <=currentZ +1;k++)
        {
          if(!IsInsideVolume(i,j,k))
        continue;
          float depth = DepthAnnotatedVolume->GetOutput()->GetScalarComponentAsFloat(i,j,k,0);
          if(depth> currentDepth)
        {
          currentDepth = depth;
          isDeepestPoint = false;
          coords[0] = i;
          coords[1] = j;
          coords[2] = k;
        }
        }
    }

  for(int i=0;i<3;i++)
    p[i] = DepthAnnotatedVolume->GetOutput()->GetPoint(DepthAnnotatedVolume->GetOutput()->ComputePointId(coords))[i];
  free(coords);
}

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
// Internal Structure:
// The model of a convex hull used is viewing a convex hull as an intersection of
// halfspaces. For the exact convex hull all halfspaces have to be considered. In this
// algorithm we consider only a subset of all halfspaces. A halfspace can be represented
// by the hessian normal form of a plane: a point x0 on the limiting plane and a direction
// n, the normal of the limiting plane. Then the halfspace is defined by all points x with
//    vtkMath::Dot(n,x0)  <= vtkMath::Dot(n,x)
// So, given a set of normals, we have to search the point set for points minimalizing
// vtkMath::Dot(n,x) for every normal n if we want to find a limiting halfspace. 
// Searching additionaly for points maximalizing it, we find the minimum of vtkMath::Dot(-1*n,x).
//
// A proper set of normals are all vectors from (0,0,0) to one neighbour voxel:
// (0,0,-1),(0,0,1),(0,-1,-1),(0,-1,0),(0,-1,1),(0,1,-1), ... , (1,1,1)
// If we want a better approximation, we consider the set of all vectors from (0,0,0) to
// voxel at distance 2 from (0,0,0):
// (0,0,-2),(0,0,2),(0,-1,-2),(0,-1,2),(0,1,-2),(0,1,2), ... , (2,2,2)
// We can approximate even better by using vectors to voxels at distance 3 from (0,0,0).
// Which distance we take is defined by the object member Granularity.
//
// Seeing that for every normal n inside this set also -1*n is inside, we can 
// concentrate on computing the minimal and maximal point x for n and exclude -1*n from the set
// of normals. Using only normals which are positive regarding the lexicographic order is a
// proper characterization of that subset.
//
// Deciding whether a point is inside the convex hull boils down to the question whether
// it lies for every normal between Dot(normal,minimal_point) and Dot(normal,maximal_point)
// 
// Computing the distance for a point x inside the hull is searching for the minimal distance 
// to all limiting halfspaces. For a point x we have to search the projection point of it
// on the convex hull and compute the distance from it.
//
// Computing all extremal points is necessary since they form the corners of the polygons.
// It is done by intersecting all possible combinations of halfspaces and looking whether
// the intersection point is part of the convex hull. When such a point is found, we know that
// it is part of the polygons limiting each halfspace used for finding the point. If we
// bookkeep for each halfspace which extremal points are part of it, providing a polygonization
// of the surface of the convex hull is easy.
// 
// object members:
// - Granularity : explained above
// - NumberNormals : number of surface voxels of a cube with side length (2*Granularity +1)
// - ConvexHull : Array of normals and corresponding minimal a maximal points.
//                ConvexHull[i][0] : i-th normal
//                ConvexHull[i][1] : minimal point for i-th normal
//                ConvexHull[i][2] : maximal point for i-th normal
// - Extremals : Extremal points for ConvexHull
// - PolygonPoints : Bookkeeping array containing for each limiting halfspace a list of extremal points in it.
//                 PolygonPoints[i] : List of extremal points on limiting halfspace ConvexHull[i/2][0] and ConvexHull[i/2][1 + (i%2)]
// - PolygonPointCounter :  PolygonPointCounter[i] is the number of how many extremal points are found for
//                          the halfspace [i/2][0] and ConvexHull[i/2][1 + (i%2)]



#include "vtkConvexHullInexact.h"
#include <vtkObjectFactory.h>
#include <vtkCellArray.h>
#include <vtkPolyData.h>
#include <vtkMath.h>
#include <iostream>
#include <assert.h>
#include <float.h>

#define EPSILON 0.000001


bool vtkConvexHullInexact::Inside(float* x)
{
  for(int i =0;i<NumberNormals;i++)
    {
      float smallest_p = vtkMath::Dot(ConvexHull[i][0],ConvexHull[i][1]);
      float largest_p = vtkMath::Dot(ConvexHull[i][0],ConvexHull[i][2]);
      float p_x = vtkMath::Dot(ConvexHull[i][0],x);
      if((p_x < smallest_p - EPSILON) || (largest_p < p_x - EPSILON))
    return false;
    }
  return true;
}



float vtkConvexHullInexact::DistanceFromConvexHull(float x,float y,float z)
{
  float* t = (float*) malloc(sizeof(float)*3);
  t[0] = x;
  t[1] = y;
  t[2] = z;
  float result = DistanceFromConvexHull(t);
  free(t);
  return result;
}

float vtkConvexHullInexact::DistanceFromConvexHull(float* x)
{
  float result = FLT_MAX;
  if(Inside(x))
    {
      for(int i =0;i<NumberNormals;i++)
    {
      float distance1 = fabs(vtkMath::Dot(ConvexHull[i][0],x) - vtkMath::Dot(ConvexHull[i][0],ConvexHull[i][1]));
      float distance2 = fabs(vtkMath::Dot(ConvexHull[i][0],x) - vtkMath::Dot(ConvexHull[i][0],ConvexHull[i][2]));
      if(distance1 < result)
        result = distance1;
      if(distance2 < result)
        result = distance2;
    }
    }
  else
    {
      float* p = (float*)malloc(Dimension*sizeof(float));
      for(int i =0;i<NumberNormals;i++)
    for(int j =1;j<3;j++)
      {
        float* normal = ConvexHull[i][0];
        float distance = fabs(vtkMath::Dot(normal,x) - vtkMath::Dot(normal,ConvexHull[i][j]));
        // project on plane:
        // we normalize on the case that the point is not in the defined halfspace,
        // therefore we have to invert distance in the case j=2;
        if(j==2)
          distance = -distance;

        for(int k=0;k<3;k++)
          p[k] = x[k] + distance*normal[k];

      }
      if(result==FLT_MAX)
    {
      cout << "Error while computing distance from convex hull: couldn't compute projection point"<<endl;
    }
      free(p);
    }
  return result;
}

// computing the set of extremal points by intersecting all combinations of halfspaces
// since the halfspaces (ConvexHull[i][0],ConvexHull[i][1]) and (ConvexHull[i][0],ConvexHull[i][2])
// are parallel, we don't have to test them.
void vtkConvexHullInexact::UpdateExtremalPoints()
{
  // reset the index-counting array
  for(int i=0;i<2*NumberNormals;i++)
    PolygonPointCounter[i] = 0;

  vtkPoints* result = vtkPoints::New();
  double** IntersectionProblem = (double**)malloc(Dimension*sizeof(double*));
  for(int i = 0;i<Dimension;i++)
    IntersectionProblem[i] = (double*)malloc(Dimension*sizeof(double));

  double* LoadVector = (double*)malloc(Dimension*sizeof(double));
  float* Solution = (float*)malloc(Dimension*sizeof(float));
  for(int i=0;i<2*NumberNormals;i++)
    {
      float* normal_i = ConvexHull[i/2][0];
      float p_i = vtkMath::Dot(normal_i,ConvexHull[i/2][1 + (i%2)]);
      for(int j=i+1;j<2*NumberNormals;j++)
    {
      if(i/2 == j/2) continue;
      float* normal_j = ConvexHull[j/2][0];
      float p_j = vtkMath::Dot(normal_j,ConvexHull[j/2][1 + (j%2)]);
      for(int k=j+1;k<2*NumberNormals;k++)
        {
          if(j/2 == k/2) continue;
          float* normal_k = ConvexHull[k/2][0];
          float p_k = vtkMath::Dot(normal_k,ConvexHull[k/2][1 + (k%2)]);
          IntersectionProblem[0][0]=normal_i[0];
          IntersectionProblem[0][1]=normal_i[1];
          IntersectionProblem[0][2]=normal_i[2];

          IntersectionProblem[1][0]=normal_j[0];
          IntersectionProblem[1][1]=normal_j[1];
          IntersectionProblem[1][2]=normal_j[2];

          IntersectionProblem[2][0]=normal_k[0];
          IntersectionProblem[2][1]=normal_k[1];
          IntersectionProblem[2][2]=normal_k[2];
          
          LoadVector[0] = p_i;
          LoadVector[1] = p_j;
          LoadVector[2] = p_k;

          if(vtkMath::SolveLinearSystem(IntersectionProblem,LoadVector,3)!=0 )
        {
          Solution[0] = (float) LoadVector[0];
          Solution[1] = (float) LoadVector[1];
          Solution[2] = (float) LoadVector[2];
          if(Inside(Solution)) // == is part of the convex hull
            {
              // assert that it is not already inside
              bool AlreadyInserted = false;
              for(vtkIdType q= 0;q<result->GetNumberOfPoints();q++)
            {
              if(vtkMath::Distance2BetweenPoints(result->GetPoint(q),Solution)<1)
                {
                  AlreadyInserted= true;
                  InsertPolygonPoint(i,q);
                  InsertPolygonPoint(j,q);
                  InsertPolygonPoint(k,q);
                }
            }
              if(!AlreadyInserted)
            {
              vtkIdType id = result->InsertNextPoint(Solution[0],Solution[1],Solution[2]);
              InsertPolygonPoint(i,id);
              InsertPolygonPoint(j,id);
              InsertPolygonPoint(k,id);
            }
            }
        }
        }
    }
    }

  free(LoadVector);
  free(Solution);
  for(int i =0;i<Dimension;i++)
    free(IntersectionProblem[i]);
  free(IntersectionProblem);
  
  if(Extremals!=NULL)
    Extremals->Delete();
  Extremals = result;
}

// brute force approach: insert every possible triangle. A more sophisticated approach
// would be to use something like angle sorting used in Grahams Scan in order to come
// up with a clockwise sorting of the points. At the moment that approach looks like 
// an overkill.
void vtkConvexHullInexact::Polygonize(vtkPolyData* output)
{
  output->SetPoints(Extremals);
  vtkCellArray* cells = vtkCellArray::New();
  for(int i=0;i<2*NumberNormals;i++)
    {
      if(PolygonPointCounter[i]<3) 
    continue;
      for(int k=0;k<PolygonPointCounter[i];k++)
    for(int m=k+1;m<PolygonPointCounter[i];m++)
      for(int n=m+1;n<PolygonPointCounter[i];n++)
        {
          cells->InsertNextCell(3);
          cells->InsertCellPoint(PolygonPoints[i][k]);
          cells->InsertCellPoint(PolygonPoints[i][m]);
          cells->InsertCellPoint(PolygonPoints[i][n]);
        }
    }
  output->SetPolys(cells);
}


void vtkConvexHullInexact::UpdateConvexHull(vtkPoints* v)
{
  if(v->GetNumberOfPoints()==0) return;
  float* x0 = v->GetPoint(0);


  // Initialize to one point
  for(int i =0;i<NumberNormals;i++)
    {
      for(int j=1;j<3;j++)
    {
      for(int k = 0;k<Dimension;k++)
        ConvexHull[i][j][k] = x0[k];
    }
    }

  // iterative updating of the convex hull
  for (int i = 0; i< v->GetNumberOfPoints();i++)
    {
      float* p0 = v->GetPoint(i);
      for(int j= 0; j<NumberNormals;j++)
    {
      float* n = ConvexHull[j][0];

      bool smaller_p  =  vtkMath::Dot(n,p0) < vtkMath::Dot(n,ConvexHull[j][1]);
      bool larger_p   =  vtkMath::Dot(n,p0) > vtkMath::Dot(n,ConvexHull[j][2]);
      if(smaller_p)
        {
          ConvexHull[j][1][0] = p0[0];
          ConvexHull[j][1][1] = p0[1];
          ConvexHull[j][1][2] = p0[2];
        }
      if (larger_p)
        {
          ConvexHull[j][2][0] = p0[0];
          ConvexHull[j][2][1] = p0[1];
          ConvexHull[j][2][2] = p0[2];
        }
    }
    }
}


void vtkConvexHullInexact::Execute()
{
  vtkPolyData *input = (vtkPolyData *)this->Inputs[0];
  vtkPolyData *output = this->GetOutput();
  
  UpdateConvexHull(input->GetPoints());
  UpdateExtremalPoints();
  Polygonize(output);
}


vtkConvexHullInexact* vtkConvexHullInexact::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkConvexHullInexact")
;
  if(ret)
    {
    return (vtkConvexHullInexact*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkConvexHullInexact;
}

void vtkConvexHullInexact::Delete()
{
  delete this;

}
void vtkConvexHullInexact::PrintSelf()
{

}

vtkConvexHullInexact::vtkConvexHullInexact()
{
  ConvexHull = NULL;
  Extremals = NULL;
  PolygonPoints = NULL;
  PolygonPointCounter = NULL;
  Dimension = 3;
  NumberNormals=-1;
  Granularity= -1;
  SetGranularity(2);
}

vtkConvexHullInexact::~vtkConvexHullInexact()
{
  if(Extremals != NULL)
    Extremals->Delete();
  if(ConvexHull!=NULL)
    {
      for(int i = 0;i<NumberNormals;i++)
    {
      for(int j = 0;j<3;j++)
        free(ConvexHull[i][j]);
      free(ConvexHull[i]);
    }
      free(ConvexHull);
    }

  if(PolygonPointCounter!=NULL)
    free(PolygonPointCounter);

  if(PolygonPoints!=NULL)
    {
      for(int i=0;i< 2*NumberNormals;i++)
    free(PolygonPoints[i]);
      free(PolygonPoints);
    }
}

vtkConvexHullInexact::vtkConvexHullInexact(vtkConvexHullInexact&)
{

}

void vtkConvexHullInexact::operator=(const vtkConvexHullInexact)
{

}

// think of the normal n as a number to the base 2*(Granularity + 1)
// then the next normal is  n+1
void vtkConvexHullInexact::NextNormal(float* n)
{
  for(int i= Dimension-1;i>=0;i--)
    {
      if(n[i]!= Granularity)
    {
      n[i]++;
      for (int j= i+1;j<Dimension;j++)
        n[j] = -Granularity;
      break;
    }
    }
}

// returns true iff n strictly lexicographic positive
bool vtkConvexHullInexact::LexPositive(float* n)
{
  for(int i =0;i<Dimension;i++)
    {
      if(n[i]<0) return false;
      if(n[i]>0) return true;
    }
  return false;
}

bool vtkConvexHullInexact::AtLeastOneNeighbourDistEntry(float* n)
{
  for(int i = 0;i<Dimension;i++)
    {
      if(fabs(n[i])==Granularity) return true;
    }
  return false;
}

void vtkConvexHullInexact::SetGranularity(int newGranularity)
{
  if(newGranularity < 1 || newGranularity==Granularity) 
    return;

  Granularity = newGranularity;

  // free the current memory
  if(ConvexHull!=NULL)
    {
      for(int i = 0;i<NumberNormals;i++)
    {
      for(int j = 0;j<3;j++)
        free(ConvexHull[i][j]);
      free(ConvexHull[i]);
    }
      free(ConvexHull);
    }

  if(PolygonPointCounter!=NULL)
    free(PolygonPointCounter);

  if(PolygonPoints!=NULL)
    {
      for(int i=0;i< 2*NumberNormals;i++)
    free(PolygonPoints[i]);
      free(PolygonPoints);
    }

  // compute new count of normals
  NumberNormals = ((int)pow(2*Granularity +1,Dimension) - (int)pow(2*Granularity -1, Dimension)) / 2;

  // allocate new memory
  PolygonPointCounter = (int*)malloc(NumberNormals*2*sizeof(int));
  PolygonPoints = (vtkIdType**)malloc(NumberNormals*2*sizeof(vtkIdType*));
  for(int i =0;i< NumberNormals*2;i++)
    PolygonPoints[i] = (vtkIdType*)malloc(NumberNormals*sizeof(vtkIdType));


  ConvexHull = (float***) malloc(NumberNormals*sizeof(float**));
  for(int i =0;i<NumberNormals;i++)
    {
      ConvexHull[i] = (float**) malloc(3*sizeof(float*));
      for(int j=0;j<3;j++)
    {
      ConvexHull[i][j] = (float*) malloc(Dimension*sizeof(float));
    }
    }
  

  // insert new normals
  float* n = (float*) malloc(Dimension*sizeof(float));

  for(int i=0;i<Dimension;i++)
    n[i] = 0;

  int i = 0;

  while(i != NumberNormals)
    {
      NextNormal(n);
      if(LexPositive(n) && AtLeastOneNeighbourDistEntry(n))
    {
      
      for(int j=0;j< Dimension;j++)
        ConvexHull[i][0][j] = n[j];
      vtkMath::Normalize(ConvexHull[i][0]);
      i++;
    }
    }
  
  free(n);

  Modified();
}

void vtkConvexHullInexact::InsertPolygonPoint(int i,vtkIdType idPoint)
{
  // ensure that we don't insert a point twice
  for(int j=0;j<PolygonPointCounter[i];j++)
    if(PolygonPoints[i][j]==idPoint)
      return;
  PolygonPoints[i][PolygonPointCounter[i]++] = idPoint;
}

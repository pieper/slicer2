/* =auto=========================================================================
                                                                                
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
=========================================================================auto= */
// The basic idea of the Bookstein sphere fitting 
// is not to minimize the euclidean distance, which is 
//   \sum_{i=1}^n (distance(x_i,sphere_center) - radius)^2 ,
// but to minimize 
//   \sum_{i=1}^n distance(x_i,sphere_center)^2 - radius^2 
// instead. 

// From a computational point of view, the first sum
// mainly is solving equations of the order 4 whereas 
// the second boils down to the least squares problem
// (x_i,y_i,z_i,1) * (alpha,beta,gamma,delta) = x_i^2 + y_i^2 + z_i^2.
//
// The center of the fitting sphere found is 
// (-alpha/2,-beta/2,-gamma/2)
// and the radius is sqrt(vtkMath::Dot(center,center) - delta)
//
// The quality of the sphere found via the Bookstein algorithm
// can be improved using as radius 
// (\sum_{i=1}^n Norm(x_i - center)) / n
// which is the radius an euclidean best fit sphere would have.

#include "vtkBooksteinSphereFit.h"
#include <vtkObjectFactory.h>
#include <vtkPolyData.h>
#include <vtkMath.h>
#include <iostream>
#include <assert.h>

void vtkBooksteinSphereFit::Execute()
{
  vtkPolyData *input = (vtkPolyData *)this->Inputs[0];
  vtkPolyData *output = this->GetOutput();
  vtkIdType nr_points = input->GetNumberOfPoints();
  float* point;

  double* line = (double*)malloc(4*sizeof(double));
  line[3] = 1;

  Solver->Initialize(4);

  // add all points
  for(int i = 0;i<input->GetNumberOfPoints();i++)
    {
      point = input->GetPoint(i);
      line[0] = point[0];
      line[1] = point[1];
      line[2] = point[2];
      Solver->AddLine(line,vtkMath::Dot(point,point));
    }

  // solve the problem
  Solver->Solve(line);
  // compute the geometrical solution
  GeometricalSolution(line[0],line[1],line[2],line[3]);

  // recompute the radius
  BestEuclideanFitRadius(input->GetPoints());

  Base->SetRadius(Radius);
  Base->SetCenter(Center[0],Center[1],Center[2]);
  output->SetPoints(Base->GetOutput()->GetPoints());
  output->SetStrips(((vtkPolyData*)Base->GetOutput())->GetStrips());
  output->SetLines(((vtkPolyData*)Base->GetOutput())->GetLines());
  output->SetVerts(((vtkPolyData*)Base->GetOutput())->GetVerts());
  output->SetPolys(((vtkPolyData*)Base->GetOutput())->GetPolys());
}

void vtkBooksteinSphereFit::GeometricalSolution(float alpha,float beta,float gamma,float delta)
{
  Center[0] = - alpha/2;
  Center[1] = - beta/2;
  Center[2] = - gamma/2;
  
  Radius = sqrt (vtkMath::Dot(Center,Center)   - delta);
}

void vtkBooksteinSphereFit::BestEuclideanFitRadius(vtkPoints* points)
{
  float newRadius = 0;
  float norm;
  float* point_i;
  for(vtkIdType i=0;i<points->GetNumberOfPoints();i++)
    {
      point_i = points->GetPoint(i);
      norm = 0;
      for(int j=0;j<3;j++)
    norm += (Center[j]-point_i[j])*(Center[j]-point_i[j]);
      newRadius += sqrt(norm);
    }
  Radius = newRadius/points->GetNumberOfPoints();
}

vtkBooksteinSphereFit* vtkBooksteinSphereFit::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkBooksteinSphereFit");
  if(ret)
    {
    return (vtkBooksteinSphereFit*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkBooksteinSphereFit;
}

void vtkBooksteinSphereFit::Delete()
{
  delete this;

}
void vtkBooksteinSphereFit::PrintSelf()
{

}

vtkBooksteinSphereFit::vtkBooksteinSphereFit()
{
  Center = (float*) malloc(3*sizeof(float));
  Center[0] = 0;
  Center[1] = 0;
  Center[2] = 0;

  Radius = 3;

  Base = vtkSphereSource::New();

  Base->SetThetaResolution(30);
  Base->SetPhiResolution(30);
  Base->SetRadius(Radius);

  Solver = vtkLargeLeastSquaresProblem::New();
  Solver->SetNumberIncreasement(5);
}

vtkBooksteinSphereFit::~vtkBooksteinSphereFit()
{
  free(Center);
  Base->Delete();
  Solver->Delete();
}

vtkBooksteinSphereFit::vtkBooksteinSphereFit(vtkBooksteinSphereFit&)
{

}

void vtkBooksteinSphereFit::operator=(const vtkBooksteinSphereFit)
{

}

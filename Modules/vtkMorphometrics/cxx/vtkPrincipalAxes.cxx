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
#include "vtkPrincipalAxes.h"
#include <vtkObjectFactory.h>
#include <vtkPolyData.h>
#include <vtkMath.h>
#include <iostream>
#include <assert.h>
void vtkPrincipalAxes::Execute()
{
  vtkPolyData *input = (vtkPolyData *)this->Inputs[0];
  vtkPolyData *output = this->GetOutput();
  vtkIdType nr_points = input->GetNumberOfPoints();
  vtkFloatingPointType* x ;

  int i;
  // reset the intermediate arrays
  for(i =0;i<3;i++)
    Center[i] = 0;

  for(i =0;i<3;i++)
    for(int j = 0;j<3;j++)
      eigenvalueProblem[i][j] = 0;
  
  for(i =0;i<3;i++)
    for(int j = 0;j<3;j++)
      eigenvalueProblemDiag[i][j] = 0;


  // compute the center
  for(vtkIdType j=0;j<input->GetNumberOfPoints();j++)
    {
      x = input->GetPoint(j);
      Center[0] += x[0];
      Center[1] += x[1];
      Center[2] += x[2];
    }

  Center[0] =   Center[0] / nr_points;
  Center[1] =   Center[1] / nr_points;
  Center[2] =   Center[2] / nr_points;

  // create the eigenvalue-problem
  // using the symmetry of the result matrix

  for(i=0;i<3;i++)
    for(int j = i;j<3;j++)
      eigenvalueProblem[i][j] = - Center[i]*nr_points*Center[j];


  for(vtkIdType m=0;m< nr_points;m++)
    {
      x = input->GetPoint(m);
      for(int i=0;i<3;i++)
    for(int j = i;j<3;j++)
      eigenvalueProblemDiag[i][j] +=  x[i]*x[j];
    }
  for(i=0;i<3;i++)
    for(int j = i;j<3;j++)
      eigenvalueProblem[i][j] += eigenvalueProblemDiag[i][j];
  
  for(i=0;i<3;i++)
    for(int j= 0;j<i;j++)
      eigenvalueProblem[i][j] = eigenvalueProblem[j][i];

  vtkMath::Jacobi(eigenvalueProblem,eigenvalues,eigenvectors);

  // update Axes
  XAxis[0] = eigenvectors[0][0];
  XAxis[1] = eigenvectors[1][0];
  XAxis[2] = eigenvectors[2][0];

  YAxis[0] = eigenvectors[0][1];
  YAxis[1] = eigenvectors[1][1];
  YAxis[2] = eigenvectors[2][1];

  ZAxis[0] = eigenvectors[0][2];
  ZAxis[1] = eigenvectors[1][2];
  ZAxis[2] = eigenvectors[2][2];
}

vtkPrincipalAxes* vtkPrincipalAxes::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkPrincipalAxes");

  if(ret)
    {
    return (vtkPrincipalAxes*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkPrincipalAxes;
}

void vtkPrincipalAxes::Delete()
{
  delete this;

}
void vtkPrincipalAxes::PrintSelf()
{

}

vtkPrincipalAxes::vtkPrincipalAxes()
{
  Center = (vtkFloatingPointType*) malloc(3*sizeof(vtkFloatingPointType));
  Center[0] = 0;
  Center[1] = 0;
  Center[2] = 0;

  XAxis = (vtkFloatingPointType*) malloc(3*sizeof(vtkFloatingPointType));
  XAxis[0] = 1;
  XAxis[1] = 0;
  XAxis[2] = 0;

  YAxis = (vtkFloatingPointType*) malloc(3*sizeof(vtkFloatingPointType));
  YAxis[0] = 0;
  YAxis[1] = 1;
  YAxis[2] = 0;

  ZAxis = (vtkFloatingPointType*) malloc(3*sizeof(vtkFloatingPointType));
  ZAxis[0] = 0;
  ZAxis[1] = 0;
  ZAxis[2] = 1;

  eigenvalueProblem = (double**)malloc(sizeof(double*)*3);
  int i;
  for(i = 0;i<3;i++)
    eigenvalueProblem[i] = (double*) malloc(3*sizeof(double));

  eigenvalueProblemDiag = (double**)malloc(sizeof(double*)*3);
  for(i = 0;i<3;i++)
    eigenvalueProblemDiag[i] = (double*) malloc(3*sizeof(double));
  
  eigenvectors = (double**)malloc(sizeof(double*)*3);
  for(i = 0;i<3;i++)
    eigenvectors[i] = (double*) malloc(3*sizeof(double));

  eigenvalues = (double*)malloc(sizeof(double)*3);

}

vtkPrincipalAxes::~vtkPrincipalAxes()
{
  free(Center);
  free(XAxis);
  free(YAxis);
  free(ZAxis);
  for(int i = 0;i<3;i++)
    {
      free(eigenvalueProblem[i]);
      free(eigenvalueProblemDiag[i]);
      free(eigenvectors[i]);
    }
  free(eigenvalueProblem);
  free(eigenvalueProblemDiag);
  free(eigenvectors);
  free(eigenvalues);
}

vtkPrincipalAxes::vtkPrincipalAxes(vtkPrincipalAxes&)
{

}

void vtkPrincipalAxes::operator=(const vtkPrincipalAxes)
{

}

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
#include "vtkEuclideanPlaneFit.h"
#include <vtkObjectFactory.h>
#include <vtkPolyData.h>
#include <vtkMath.h>
#include <iostream>
#include <assert.h>

void vtkEuclideanPlaneFit::Execute()
{
  vtkPolyData *input = (vtkPolyData *)this->Inputs[0];
  vtkPolyData *output = this->GetOutput();

  CoordinateSystem->SetInput(input);
  CoordinateSystem->Update();
  
  Center[0] = CoordinateSystem->GetCenter()[0];
  Center[1] = CoordinateSystem->GetCenter()[1];
  Center[2] = CoordinateSystem->GetCenter()[2];
  
  Normal[0] = CoordinateSystem->GetZAxis()[0];
  Normal[1] = CoordinateSystem->GetZAxis()[1];
  Normal[2] = CoordinateSystem->GetZAxis()[2];

  FittingPlane->SetCenter(Center);
  FittingPlane->SetNormal(Normal);

  FittingPlane->Update();
  output->SetPoints(FittingPlane->GetOutput()->GetPoints());
  output->SetStrips(((vtkPolyData*)FittingPlane->GetOutput())->GetStrips());
  output->SetLines(((vtkPolyData*)FittingPlane->GetOutput())->GetLines());
  output->SetVerts(((vtkPolyData*)FittingPlane->GetOutput())->GetVerts());
  output->SetPolys(((vtkPolyData*)FittingPlane->GetOutput())->GetPolys());
}

vtkEuclideanPlaneFit* vtkEuclideanPlaneFit::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkEuclideanPlaneFit")
;
  if(ret)
    {
    return (vtkEuclideanPlaneFit*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkEuclideanPlaneFit;
}

void vtkEuclideanPlaneFit::Delete()
{
  delete this;
}
void vtkEuclideanPlaneFit::PrintSelf()
{

}

vtkEuclideanPlaneFit::vtkEuclideanPlaneFit()
{
  Center = (float*) malloc(3*sizeof(float));
  Center[0] = 0;
  Center[1] = 0;
  Center[2] = 0;

  Normal = (float*) malloc(3*sizeof(float));
  Normal[0] = 0;
  Normal[1] = 0;
  Normal[2] = 1;

  CoordinateSystem = vtkPrincipalAxes::New();
  FittingPlane = vtkPlaneSource::New();

  FittingPlane->SetOrigin(0,0,0);
  FittingPlane->SetPoint1(100,0,0);
  FittingPlane->SetPoint2(0,100,0);
}

vtkEuclideanPlaneFit::~vtkEuclideanPlaneFit()
{
  free(Center);
  free(Normal);
  CoordinateSystem->Delete();
  FittingPlane->Delete();
}

vtkEuclideanPlaneFit::vtkEuclideanPlaneFit(vtkEuclideanPlaneFit&)
{

}

void vtkEuclideanPlaneFit::operator=(const vtkEuclideanPlaneFit)
{

}

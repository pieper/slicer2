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
#include "vtkEuclideanLineFit.h"
#include <vtkObjectFactory.h>
#include <vtkPolyData.h>
#include <vtkMath.h>
#include <iostream>
#include <assert.h>

void vtkEuclideanLineFit::Execute()
{
  vtkPolyData *input = (vtkPolyData *)this->Inputs[0];
  vtkPolyData *output = this->GetOutput();

  CoordinateSystem->SetInput(input);
  CoordinateSystem->Update();

  Center[0] = CoordinateSystem->GetCenter()[0];
  Center[1] = CoordinateSystem->GetCenter()[1];
  Center[2] = CoordinateSystem->GetCenter()[2];

  Direction[0] = CoordinateSystem->GetXAxis()[0];
  Direction[1] = CoordinateSystem->GetXAxis()[1];
  Direction[2] = CoordinateSystem->GetXAxis()[2];

  // update Direction
  UpdateDirection();
  OrientationFilter->Update();
  
  output->SetPoints(OrientationFilter->GetOutput()->GetPoints());
  output->SetStrips(((vtkPolyData*)OrientationFilter->GetOutput())->GetStrips());
  output->SetLines(((vtkPolyData*)OrientationFilter->GetOutput())->GetLines());
  output->SetVerts(((vtkPolyData*)OrientationFilter->GetOutput())->GetVerts());
  output->SetPolys(((vtkPolyData*)OrientationFilter->GetOutput())->GetPolys());
}

void vtkEuclideanLineFit::UpdateDirection()
{
  vtkMath::Normalize(Direction);

  float dir_x = Direction[0] / 2;
  float dir_y = (1+Direction[1]) / 2;
  float dir_z = Direction[2] / 2;

  float norm =  sqrt(dir_x*dir_x + dir_y*dir_y + dir_z*dir_z);
  dir_x = dir_x / norm;
  dir_y = dir_y / norm;
  dir_z = dir_z / norm;


  Orientation->Identity();
  Orientation->RotateWXYZ(180,dir_x,dir_y,dir_z);
  Orientation->PostMultiply();
  Orientation->Translate(Center[0],Center[1],Center[2]);
}

vtkEuclideanLineFit* vtkEuclideanLineFit::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkEuclideanLineFit")
;
  if(ret)
    {
    return (vtkEuclideanLineFit*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkEuclideanLineFit;
}

void vtkEuclideanLineFit::Delete()
{
  delete this;

}
void vtkEuclideanLineFit::PrintSelf()
{

}

vtkEuclideanLineFit::vtkEuclideanLineFit()
{
  Base = vtkCylinderSource::New();
  OrientationFilter = vtkTransformFilter::New();
  Orientation = vtkTransform::New();
  
  Center = (float*) malloc(3*sizeof(float));
  Center[0] = 0;
  Center[1] = 0;
  Center[2] = 0;

  Direction = (float*) malloc(3*sizeof(float));
  Direction[0] = 0;
  Direction[1] = 1;
  Direction[2] = 0;

  CoordinateSystem = vtkPrincipalAxes::New();


  // setup the pipeline
  OrientationFilter->SetTransform(Orientation);
  OrientationFilter->SetInput(Base->GetOutput());
  
  Orientation->Identity();

  Base->SetResolution(30);
  Base->SetRadius(3);
  Base->SetHeight(400);

}

vtkEuclideanLineFit::~vtkEuclideanLineFit()
{
  free(Center);
  free(Direction);

  Orientation->Delete();
  OrientationFilter->Delete();
  Base->Delete();

  CoordinateSystem->Delete();
}

vtkEuclideanLineFit::vtkEuclideanLineFit(vtkEuclideanLineFit&)
{

}

void vtkEuclideanLineFit::operator=(const vtkEuclideanLineFit)
{

}

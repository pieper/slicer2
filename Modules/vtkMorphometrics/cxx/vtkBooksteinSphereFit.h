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
#ifndef __vtk_bookstein_sphere_fit_h
#define __vtk_bookstein_sphere_fit_h
#include <vtkMorphometricsConfigure.h>
#include <vtkPolyDataToPolyDataFilter.h>
#include <vtkSphereSource.h>
#include <vtkPoints.h>
#include <vtkSetGet.h>
#include "vtkLargeLeastSquaresProblem.h"
//---------------------------------------------------------
// Author: Axel Krauth
//
// This class implements sphere fitting with the Bookstein
// algorithm. 
//
// The result isn't the best fitting sphere for euclidean distance
// but a very good approximation of it. The main advantage of this
// algorithm lies in the fast computation of the sphere.
class VTK_MORPHOMETRICS_EXPORT vtkBooksteinSphereFit : public vtkPolyDataToPolyDataFilter
{
  public:
  static vtkBooksteinSphereFit* New();
  void Delete();
  vtkTypeMacro(vtkBooksteinSphereFit,vtkPolyDataToPolyDataFilter);
  
  vtkGetVector3Macro(Center,float);
  vtkGetMacro(Radius,float);
  
  void PrintSelf();
  protected:
  vtkBooksteinSphereFit();
  ~vtkBooksteinSphereFit();
  
  void Execute();
  private:
  vtkBooksteinSphereFit(vtkBooksteinSphereFit&);
  void operator=(const vtkBooksteinSphereFit);
  
  // used for visualizing the result of the sphere fit
  vtkSphereSource* Base;
  float* Center;
  float  Radius;
  
  // Sphere fitting with the Bookstein algorithm is
  // a least squares problem whose matrix has
  // input->GetNumberPoints() many rows.
  vtkLargeLeastSquaresProblem* Solver;

  // Derive from the solution of the least squares problem 
  // the geometrical solution.
  void GeometricalSolution(float alpha,float beta,float gamma,float delta);

  // the quality of the radius can be improved by using
  // the radius a euclidean best fitting sphere would have
  void BestEuclideanFitRadius(vtkPoints* points);
};

#endif

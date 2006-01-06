/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkPrincipalAxesAlign.h,v $
  Date:      $Date: 2006/01/06 17:58:02 $
  Version:   $Revision: 1.2 $

=========================================================================auto=*/
#ifndef __vtk_principal_axes_align_h
#define __vtk_principal_axes_align_h
#include <vtkRealignResampleConfigure.h>
#include <vtkPolyDataToPolyDataFilter.h>
#include <vtkSetGet.h>
// ---------------------------------------------------------
// Author: Axel Krauth
//
// This class computes the principal axes of the input.
// The direction of the eigenvector for the largest eigenvalue is the XAxis,
// the direction of the eigenvector for the smallest eigenvalue is the ZAxis,
// and the YAxis the the eigenvector for the remaining eigenvalue.

class VTK_REALIGNRESAMPLE_EXPORT vtkPrincipalAxesAlign : public vtkPolyDataToPolyDataFilter
{
 public:
  static vtkPrincipalAxesAlign* New();
  void Delete();
  vtkTypeMacro(vtkPrincipalAxesAlign,vtkPolyDataToPolyDataFilter);

  vtkGetVector3Macro(Center,vtkFloatingPointType);
  vtkGetVector3Macro(XAxis,vtkFloatingPointType);
  vtkGetVector3Macro(YAxis,vtkFloatingPointType);
  vtkGetVector3Macro(ZAxis,vtkFloatingPointType);
  void Execute();
  void PrintSelf();
 protected:
  vtkPrincipalAxesAlign();
  ~vtkPrincipalAxesAlign();

 private:
  vtkPrincipalAxesAlign(vtkPrincipalAxesAlign&);
  void operator=(const vtkPrincipalAxesAlign);

  vtkFloatingPointType* Center;
  vtkFloatingPointType* XAxis;
  vtkFloatingPointType* YAxis;
  vtkFloatingPointType* ZAxis;

  // a matrix of the eigenvalue problem
  double** eigenvalueProblem;
  // for efficiency reasons parts of the eigenvalue problem are computed separately
  double** eigenvalueProblemDiag;
  double** eigenvectors;
  double* eigenvalues;
};

#endif

/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkHyperStreamlinePoints.h,v $
  Date:      $Date: 2006/01/18 19:11:04 $
  Version:   $Revision: 1.6 $

=========================================================================auto=*/
#ifndef __vtkHyperStreamlinePoints_h
#define __vtkHyperStreamlinePoints_h

#include "vtkTensorUtilConfigure.h"
#include "vtkHyperStreamlineDTMRI.h"
#include "vtkPoints.h"
#include "vtkHyperPointandArray.h"

//class VTK_TENSORUTIL_EXPORT vtkHyperStreamlinePoints : public vtkHyperStreamline
class VTK_TENSORUTIL_EXPORT vtkHyperStreamlinePoints : public vtkHyperStreamlineDTMRI
{
public:
  static vtkHyperStreamlinePoints *New();
  vtkTypeMacro(vtkHyperStreamlinePoints,vtkHyperStreamlineDTMRI);
  void PrintSelf(ostream& os, vtkIndent indent);

  vtkGetObjectMacro(HyperStreamline0,vtkPoints);
  vtkGetObjectMacro(HyperStreamline1,vtkPoints);

protected:
  vtkHyperStreamlinePoints();
  ~vtkHyperStreamlinePoints();

  void Execute();
  
  // convenient pointers to HyperStreamline1 and HyperStreamline2
  vtkPoints *HyperStreamlines[2];
  
  // points calculated for first hyperstreamline
  vtkPoints *HyperStreamline0;
  // points calculated for optional second hyperstreamline
  vtkPoints *HyperStreamline1;

private:
  vtkHyperStreamlinePoints(const vtkHyperStreamlinePoints&);  // Not implemented.
  void operator=(const vtkHyperStreamlinePoints&);  // Not implemented.
};

#endif



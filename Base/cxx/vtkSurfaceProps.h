/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkSurfaceProps.h,v $
  Date:      $Date: 2006/02/14 20:40:16 $
  Version:   $Revision: 1.12 $

=========================================================================auto=*/
#ifndef __vtkSurfaceProps_h
#define __vtkSurfaceProps_h

#include "vtkProcessObject.h"
#include "vtkPolyData.h"
#include "vtkCellTriMacro.h"
#include "vtkSlicer.h"

#ifndef vtkFloatingPointType
#define vtkFloatingPointType float
#endif

class VTK_SLICER_BASE_EXPORT vtkSurfaceProps : public vtkProcessObject
{
  public:
  vtkSurfaceProps();
  static vtkSurfaceProps *New() {return new vtkSurfaceProps;};

  void Update();
  void SetInput(vtkPolyData *input);
  vtkPolyData *GetInput();
  
  vtkGetMacro(SurfaceArea,vtkFloatingPointType);
  vtkGetMacro(MinCellArea,vtkFloatingPointType);
  vtkGetMacro(MaxCellArea,vtkFloatingPointType);
  vtkGetMacro(Volume,vtkFloatingPointType);
  vtkGetMacro(VolumeError,vtkFloatingPointType);
  vtkGetMacro(Test,vtkFloatingPointType);

protected:
  void Execute();
  vtkFloatingPointType SurfaceArea;
  vtkFloatingPointType MinCellArea;
  vtkFloatingPointType MaxCellArea;
  vtkFloatingPointType Volume;
  vtkFloatingPointType VolumeError;
  vtkFloatingPointType Test;
};

#endif

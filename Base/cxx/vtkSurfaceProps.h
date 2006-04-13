/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkSurfaceProps.h,v $
  Date:      $Date: 2006/04/13 19:24:47 $
  Version:   $Revision: 1.14 $

=========================================================================auto=*/
#ifndef __vtkSurfaceProps_h
#define __vtkSurfaceProps_h

#include "vtkProcessObject.h"
#include "vtkSlicer.h"

class vtkPolyData;
class VTK_SLICER_BASE_EXPORT vtkSurfaceProps : public vtkProcessObject
{
public:
  static vtkSurfaceProps *New();
  vtkTypeMacro(vtkSurfaceProps,vtkProcessObject);
  void PrintSelf(ostream& os, vtkIndent indent);

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
  vtkSurfaceProps();
  ~vtkSurfaceProps();

  void Execute();
  vtkFloatingPointType SurfaceArea;
  vtkFloatingPointType MinCellArea;
  vtkFloatingPointType MaxCellArea;
  vtkFloatingPointType Volume;
  vtkFloatingPointType VolumeError;
  vtkFloatingPointType Test;

private:
  vtkSurfaceProps(const vtkSurfaceProps&);
  void operator=(const vtkSurfaceProps&);
};

#endif

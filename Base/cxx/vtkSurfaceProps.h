/*=auto=========================================================================

(c) Copyright 2001 Massachusetts Institute of Technology

Permission is hereby granted, without payment, to copy, modify, display 
and distribute this software and its documentation, if any, for any purpose, 
provided that the above copyright notice and the following three paragraphs 
appear on all copies of this software.  Use of this software constitutes 
acceptance of these terms and conditions.

IN NO EVENT SHALL MIT BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, 
INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE 
AND ITS DOCUMENTATION, EVEN IF MIT HAS BEEN ADVISED OF THE POSSIBILITY OF 
SUCH DAMAGE.

MIT SPECIFICALLY DISCLAIMS ANY EXPRESS OR IMPLIED WARRANTIES INCLUDING, 
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR 
A PARTICULAR PURPOSE, AND NON-INFRINGEMENT.

THE SOFTWARE IS PROVIDED "AS IS."  MIT HAS NO OBLIGATION TO PROVIDE 
MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

=========================================================================auto=*/
#ifndef __vtkSurfaceProps_h
#define __vtkSurfaceProps_h

#include "vtkProcessObject.h"
#include "vtkPolyData.h"
#include "vtkCellTriMacro.h"
#include "vtkSlicer.h"

class VTK_SLICER_BASE_EXPORT vtkSurfaceProps : public vtkProcessObject
{
  public:
  vtkSurfaceProps();
  static vtkSurfaceProps *New() {return new vtkSurfaceProps;};
  const char *GetClassName() {return "vtkSurfaceProps";};
  
  void Update();
  void SetInput(vtkPolyData *input);
  vtkPolyData *GetInput();
  
  vtkGetMacro(SurfaceArea,float);
  vtkGetMacro(MinCellArea,float);
  vtkGetMacro(MaxCellArea,float);
  vtkGetMacro(Volume,float);
  vtkGetMacro(VolumeError,float);
  vtkGetMacro(Test,float);

protected:
  void Execute();
  float SurfaceArea;
  float MinCellArea;
  float MaxCellArea;
  float Volume;
  float VolumeError;
  float Test;
};

#endif

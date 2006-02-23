/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkImageMeasureVoxels.h,v $
  Date:      $Date: 2006/02/23 01:43:33 $
  Version:   $Revision: 1.14 $

=========================================================================auto=*/
// .NAME vtkImageMeasureVoxels - This filter counts labeled voxels.
// .SECTION Description
// vtkImageMeasureVoxels - Counts voxels for each label, then converts
// to mL.  Output is written to a file.
//

#ifndef __vtkImageMeasureVoxels_h
#define __vtkImageMeasureVoxels_h

#include "vtkSlicer.h"

class vtkFloatArray;
class VTK_SLICER_BASE_EXPORT vtkImageMeasureVoxels : public vtkSlicerImageAlgorithm
{
public:
  static vtkImageMeasureVoxels *New();
  vtkTypeMacro(vtkImageMeasureVoxels,vtkSlicerImageAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description: 
  // Set/Get the filename where the measurements will be written
  vtkSetStringMacro(FileName);
  vtkGetStringMacro(FileName);
  
  vtkGetObjectMacro(Result,vtkFloatArray);

protected:
  vtkImageMeasureVoxels();
  ~vtkImageMeasureVoxels();

  char *FileName;
  vtkFloatArray *Result;
  void ExecuteData(vtkDataObject *);

private:
  vtkImageMeasureVoxels(const vtkImageMeasureVoxels&);
  void operator=(const vtkImageMeasureVoxels&);
};

#endif




/*=auto=========================================================================

  Portions (c) Copyright 2006 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkImageThermalMap.h,v $
  Date:      $Date: 2006/03/08 15:14:56 $
  Version:   $Revision: 1.1.2.1 $

=========================================================================auto=*/
// .NAME vtkImageThermalMap -  generates a thermal volume from the four
// input volumes.
//
// .SECTION Description
// Images must have the same extents. Output is always type float.
//  
//
#ifndef __vtkImageThermalMap_h
#define __vtkImageThermalMap_h

#include <vtkMRAblationConfigure.h>
#include "vtkImageData.h"
#include "vtkImageMultipleInputFilter.h"

class VTK_MRABLATION_EXPORT vtkImageThermalMap : public vtkImageMultipleInputFilter
{
  public:
  static vtkImageThermalMap *New();
  vtkTypeMacro(vtkImageThermalMap,vtkImageMultipleInputFilter);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  protected:

  vtkImageThermalMap();
  ~vtkImageThermalMap();
  
  void ExecuteInformation(vtkImageData **inputs, vtkImageData *output); 
  void ExecuteInformation(){this->vtkImageMultipleInputFilter::ExecuteInformation();};
  void ThreadedExecute(vtkImageData **inDatas, vtkImageData *outData, 
                       int extent[6], int id);
};

#endif


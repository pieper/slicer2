/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkImageOverlay.h,v $
  Date:      $Date: 2007/10/29 14:58:17 $
  Version:   $Revision: 1.14.2.1.2.1 $

=========================================================================auto=*/
// .NAME vtkImageOverlay - Overlay Images
// .SECTION Description
// vtkImageOverlay takes multiple inputs images and merges
// them into one output. All inputs must have the same extent,
// scalar type, and number of components.

#ifndef __vtkImageOverlay_h
#define __vtkImageOverlay_h

#include "vtkImageData.h"
#include "vtkImageMultipleInputFilter.h"
#include "vtkSlicer.h"

class VTK_SLICER_BASE_EXPORT vtkImageOverlay : public vtkImageMultipleInputFilter
{
public:
  static vtkImageOverlay *New();
  vtkTypeMacro(vtkImageOverlay,vtkImageMultipleInputFilter);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Sets the opacity used to overlay this layer on the others
  double GetOpacity(int layer);
  void SetOpacity(int layer, double opacity);

  // Sets whether to fade out the background even when the 
  // foreground is clear
  int GetFade(int layer);
  void SetFade(int layer, int fade);

protected:
  vtkImageOverlay();
  ~vtkImageOverlay();
  vtkImageOverlay(const vtkImageOverlay&);
  void operator=(const vtkImageOverlay&);
  double *Opacity;
  int nOpacity;
  int *Fade;
  int nFade;

  void UpdateForNumberOfInputs();
  //void ThreadedExecute(vtkImageData **inDatas, vtkImageData *outData,
    //int extent[6], int id);  
  void ExecuteData(vtkDataObject *data);
};

#endif

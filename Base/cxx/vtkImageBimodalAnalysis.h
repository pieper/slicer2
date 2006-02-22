/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkImageBimodalAnalysis.h,v $
  Date:      $Date: 2006/02/22 23:47:15 $
  Version:   $Revision: 1.18 $

=========================================================================auto=*/
// .NAME vtkImageBimodalAnalysis - Analysis bimodal histograms
// .SECTION Description
// vtkImageBimodalAnalysis - This filter assumes the input comes
// from vtkImageAccumulateDiscrete, so there.

#ifndef __vtkImageBimodalAnalysis_h
#define __vtkImageBimodalAnalysis_h

#define VTK_BIMODAL_MODALITY_CT 0
#define VTK_BIMODAL_MODALITY_MR 1

#include "vtkImageData.h"
#include "vtkImageToImageFilter.h"
#include "vtkSlicer.h"

class VTK_SLICER_BASE_EXPORT vtkImageBimodalAnalysis : public vtkImageToImageFilter
{
public:
  static vtkImageBimodalAnalysis *New();
  vtkTypeMacro(vtkImageBimodalAnalysis,vtkImageToImageFilter);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Set the type of data, if known
  vtkSetMacro(Modality, int);
  vtkGetMacro(Modality, int);
  void SetModalityToMR() {this->SetModality(VTK_BIMODAL_MODALITY_MR);};
  void SetModalityToCT() {this->SetModality(VTK_BIMODAL_MODALITY_CT);};
 
  // Description:
  // Get stats
  vtkSetMacro(Threshold, int);
  vtkGetMacro(Threshold, int);
  vtkSetMacro(Window, int);
  vtkGetMacro(Window, int);
  vtkSetMacro(Level, int);
  vtkGetMacro(Level, int);
  vtkSetMacro(Min, int);
  vtkGetMacro(Min, int);
  vtkSetMacro(Max, int);
  vtkGetMacro(Max, int);

  vtkGetVectorMacro(SignalRange, int, 2);
  vtkSetVector2Macro(SignalRange, int);

  vtkGetVectorMacro(ClipExtent, int, 6);
  vtkSetVectorMacro(ClipExtent, int, 6);

protected:
  vtkImageBimodalAnalysis();
  ~vtkImageBimodalAnalysis() {};
  vtkImageBimodalAnalysis(const vtkImageBimodalAnalysis&);
  void operator=(const vtkImageBimodalAnalysis&);

  int Modality;

  int Threshold;
  int Window;
  int Level;
  int Min;
  int Max;
  int ClipExtent[6];
  int SignalRange[2];

  void ExecuteInformation(vtkImageData *input, vtkImageData *output);
  void ExecuteInformation(){this->vtkImageToImageFilter::ExecuteInformation();};
  void ComputeInputUpdateExtent(int inExt[6], int outExt[6]);
  void ExecuteData(vtkDataObject *);

};

#endif




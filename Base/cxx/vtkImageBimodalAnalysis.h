/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkImageBimodalAnalysis.h,v $
  Date:      $Date: 2006/02/23 01:43:32 $
  Version:   $Revision: 1.19 $

=========================================================================auto=*/
// .NAME vtkImageBimodalAnalysis - Analysis bimodal histograms
// .SECTION Description
// vtkImageBimodalAnalysis - This filter assumes the input comes
// from vtkImageAccumulateDiscrete, so there.

#ifndef __vtkImageBimodalAnalysis_h
#define __vtkImageBimodalAnalysis_h

#include "vtkSlicer.h"

#define VTK_BIMODAL_MODALITY_CT 0
#define VTK_BIMODAL_MODALITY_MR 1

class VTK_SLICER_BASE_EXPORT vtkImageBimodalAnalysis : public vtkSlicerImageAlgorithm
{
public:
  static vtkImageBimodalAnalysis *New();
  vtkTypeMacro(vtkImageBimodalAnalysis,vtkSlicerImageAlgorithm);
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

  int Modality;

  int Threshold;
  int Window;
  int Level;
  int Min;
  int Max;
  int ClipExtent[6];
  int SignalRange[2];

  void ExecuteInformation(vtkImageData *input, vtkImageData *output);
#ifndef SLICER_VTK5
  void ExecuteInformation(){this->Superclass::ExecuteInformation();};
#endif
  void ComputeInputUpdateExtent(int inExt[6], int outExt[6]);
  void ExecuteData(vtkDataObject *);

private:
  vtkImageBimodalAnalysis(const vtkImageBimodalAnalysis&);
  void operator=(const vtkImageBimodalAnalysis&);
};

#endif




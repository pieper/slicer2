/*=auto=========================================================================

(c) Copyright 2002 Massachusetts Institute of Technology

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
// .NAME vtkImageBimodalAnalysis - Analysis bimodal histograms
// .SECTION Description
// vtkImageBimodalAnalysis - This filter assumes the input comes
// from vtkImageAccumulateDiscrete, so there.

#ifndef __vtkImageBimodalAnalysis_h
#define __vtkImageBimodalAnalysis_h

#define VTK_BIMODAL_MODALITY_CT 0
#define VTK_BIMODAL_MODALITY_MR 1

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
  vtkImageBimodalAnalysis(const vtkImageBimodalAnalysis&) {};
  void operator=(const vtkImageBimodalAnalysis&) {};

  int Modality;

  int Threshold;
  int Window;
  int Level;
  int Min;
  int Max;
  int ClipExtent[6];
  int SignalRange[2];

  void ExecuteInformation(vtkImageData *input, vtkImageData *output);
  void ComputeInputUpdateExtent(int inExt[6], int outExt[6]);
  void Execute(vtkImageData *inData, vtkImageData *outData);

  // Description:
  // Generate more than requested.  Called by the superclass before
  // an execute, and before output memory is allocated.
  void EnlargeOutputUpdateExtents( vtkDataObject *vtkNotUsed(data) );

};

#endif




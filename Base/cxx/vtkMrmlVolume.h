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
#ifndef __vtkMrmlVolume_h
#define __vtkMrmlVolume_h

#include <fstream.h>
#include <stdlib.h>
#include <iostream.h>

#include "vtkMrmlVolumeNode.h"
#include "vtkImageAccumulateDiscrete.h"
#include "vtkImageBimodalAnalysis.h"
#include "vtkImageResize.h"
#include "vtkIndirectLookupTable.h"
#include "vtkImagePlot.h"
#include "vtkLookupTable.h"
#include "vtkImageData.h"

//----------------------------------------------------------------------------
class VTK_EXPORT vtkMrmlVolume : public vtkProcessObject
{
public:
  static vtkMrmlVolume *New();
  vtkTypeMacro(vtkMrmlVolume,vtkProcessObject);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  // Description:
  // Set/Get the MRML info
  vtkSetObjectMacro(MrmlNode, vtkMrmlVolumeNode);
  vtkGetObjectMacro(MrmlNode, vtkMrmlVolumeNode);
  
  // Description:
  // Copy another MmrlVolume's MrmlNode to this one
  void CopyNode(vtkMrmlVolume *volume); 

  // Description:
  // Provides opportunity to insure internal consistency before access. 
  // Transfers all ivars from MrmlNode to internal VTK objects
  // Also causes ImageData's source (if any) to update itself.
  vtkImageData* GetOutput();
  void Update();
  unsigned long int GetMTime();

  // Description:
  // Description:
  // Set/Get the image data
  vtkSetObjectMacro(ImageData, vtkImageData);
  vtkGetObjectMacro(ImageData, vtkImageData);

  // Description:
  // Read/Write image 
  void Read();
  void Write();
  // Description:
  // For internal use during Read/Write
  vtkGetObjectMacro(ProcessObject, vtkProcessObject);

  // Get the indirect LUT (LookupTable).
  // If UseLabelLUT is on, then returns the LabelLUT, otherwise
  // the volume's own IndirectLookupTable.
  vtkIndirectLookupTable *GetIndirectLUT();

  // Description:
  // Set Label IndirectLookupTable
  vtkGetMacro(UseLabelIndirectLUT, int);
  vtkSetMacro(UseLabelIndirectLUT, int);
  vtkBooleanMacro(UseLabelIndirectLUT, int);
  vtkSetObjectMacro(LabelIndirectLUT, vtkIndirectLookupTable);
  vtkGetObjectMacro(LabelIndirectLUT, vtkIndirectLookupTable);

  // Description:
  // Set LookupTable
  void SetLookupTable(vtkLookupTable *lut) {
	  this->IndirectLUT->SetLookupTable(lut);};
  vtkLookupTable *GetLookupTable() {
    return this->IndirectLUT->GetLookupTable();};

  // Description:
  // Set Histogram attributes
  void SetHistogramHeight(int h) {this->HistPlot->SetHeight(h);};
  void SetHistogramWidth(int w);
  vtkSetVector3Macro(HistogramColor, float);

  // Description:
  // Get Histogram plot (an image)
  vtkImageData *GetHistogramPlot();

  int GetBimodalThreshold() {return this->Bimodal->GetThreshold();};

  // Description:
  // Assumes GetHistogramPlot has already been called
  int MapBinToScalar(int b) {return this->HistPlot->MapBinToScalar(b);};
  int MapScalarToBin(int s) {return this->HistPlot->MapScalarToBin(s);};
  
  // Description:
  // Set/Get window/level/threshold slider range
  vtkGetMacro(RangeLow,  int);
  vtkGetMacro(RangeHigh, int);
  vtkGetMacro(RangeAuto, int);
  vtkSetMacro(RangeLow,  int);
  vtkSetMacro(RangeHigh, int);
  vtkSetMacro(RangeAuto, int);
  vtkBooleanMacro(RangeAuto, int);

protected:
  vtkMrmlVolume();
  ~vtkMrmlVolume();
  vtkMrmlVolume(const vtkMrmlVolume&) {};
  void operator=(const vtkMrmlVolume&) {};

  void UpdateWindowLevelThreshold();
  void CheckImageData();
  void CheckMrmlNode();
  void CheckLabelIndirectLUT();

  vtkMrmlVolumeNode *MrmlNode;
  
  int RangeLow;
  int RangeHigh;
  int RangeAuto;
  int UseLabelIndirectLUT;
  float HistogramColor[3];

  vtkImageData *ImageData;
  vtkProcessObject *ProcessObject;
  vtkImageAccumulateDiscrete *Accumulate;
  vtkImageBimodalAnalysis *Bimodal;
  vtkImageResize *Resize;
  vtkImagePlot *HistPlot;
  vtkIndirectLookupTable *IndirectLUT;
  vtkIndirectLookupTable *LabelIndirectLUT;
};

#endif



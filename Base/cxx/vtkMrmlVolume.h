/*=auto=========================================================================
Copyright (c) 1999 Surgical Planning Lab, Brigham and Women's Hospital
 
Direct all questions on this copyright to slicer@ai.mit.edu.
The following terms apply to all files associated with the software unless
explicitly disclaimed in individual files.   

The authors hereby grant permission to use, copy, and distribute this
software and its documentation for any NON-COMMERCIAL purpose, provided
that existing copyright notices are retained verbatim in all copies.
The authors grant permission to modify this software and its documentation 
for any NON-COMMERCIAL purpose, provided that such modifications are not 
distributed without the explicit consent of the authors and that existing
copyright notices are retained in all copies. Some of the algorithms
implemented by this software are patented, observe all applicable patent law.

IN NO EVENT SHALL THE AUTHORS OR DISTRIBUTORS BE LIABLE TO ANY PARTY FOR
DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
OF THE USE OF THIS SOFTWARE, ITS DOCUMENTATION, OR ANY DERIVATIVES THEREOF,
EVEN IF THE AUTHORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

THE AUTHORS AND DISTRIBUTORS SPECIFICALLY DISCLAIM ANY WARRANTIES, INCLUDING,
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
PARTICULAR PURPOSE, AND NON-INFRINGEMENT.  THIS SOFTWARE IS PROVIDED ON AN
'AS IS' BASIS, AND THE AUTHORS AND DISTRIBUTORS HAVE NO OBLIGATION TO PROVIDE
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
  const char *GetClassName() {return "vtkMrmlVolume";};
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



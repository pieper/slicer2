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
// .NAME vtkMrmlDataVolume - Object used in the slicer to perform
// everything related to the access and display of image data (volumes).
// .SECTION Description
// Used in conjunction with a vtkMrmlDataVolumeNode (which neatly describes
// display settings, file locations, etc.).  Essentially, the MRML 
// node gives the high level description of what this class should 
// actually do with the ImageData.
// 

#ifndef __vtkMrmlDataVolume_h
#define __vtkMrmlDataVolume_h

//#include <fstream.h>
#include <stdlib.h>
//#include <iostream.h>

#include "vtkMrmlData.h"

#include "vtkMrmlVolumeNode.h"
#include "vtkImageAccumulateDiscrete.h"
#include "vtkImageBimodalAnalysis.h"
#include "vtkImageResize.h"
#include "vtkImagePlot.h"
#include "vtkImageData.h"

class vtkImageDICOMReader;
class vtkImageReader;

//----------------------------------------------------------------------------
class VTK_EXPORT vtkMrmlDataVolume : public vtkMrmlData
{
public:
  static vtkMrmlDataVolume *New();
  vtkTypeMacro(vtkMrmlDataVolume,vtkMrmlData);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  // Description:
  // Provides opportunity to insure internal consistency before access. 
  // Transfers all ivars from MrmlNode to internal VTK objects
  // Also causes ImageData's source (if any) to update itself.
  void Update();
  unsigned long int GetMTime();

  // Description:
  // Set the image data
  // Use GetOutput to get the image data.
  vtkSetObjectMacro(ImageData, vtkImageData);
  vtkImageData* GetOutput();

  // Description:
  // Read/Write image 
  int Read();
  int Write();

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
  vtkMrmlDataVolume();
  ~vtkMrmlDataVolume();
  vtkMrmlDataVolume(const vtkMrmlDataVolume&) {};
  void operator=(const vtkMrmlDataVolume&) {};

  // Description: 
  // If Data has not be created, create it.
  void CheckMrmlNode();
  void UpdateWindowLevelThreshold();
  void CheckImageData();

  int RangeLow;
  int RangeHigh;
  int RangeAuto;
  float HistogramColor[3];

  vtkImageData *ImageData;
  vtkImageAccumulateDiscrete *Accumulate;
  vtkImageBimodalAnalysis *Bimodal;
  vtkImageResize *Resize;
  vtkImagePlot *HistPlot;

// Description:
// Take information from the VtkMrmlNode and put it in the appropriate reader.
void DicomReaderSetup(vtkImageDICOMReader *dcmreader,
                      vtkMrmlVolumeNode *node,
                      int ext[6]);
void NonDicomReaderSetup(vtkImageReader *dcmreader,
                         vtkMrmlVolumeNode *node,
                         int ext[6]);

// Description:
// Do all the reading.
// Return the vtkImageSource already updated
// used by this->Read() and this->ReRead()
 vtkImageSource *ReaderHelper();


};

#endif

/*=auto=========================================================================

(c) Copyright 2003 Massachusetts Institute of Technology (MIT) All Rights Reserved.

This software ("3D Slicer") is provided by The Brigham and Women's 
Hospital, Inc. on behalf of the copyright holders and contributors.
Permission is hereby granted, without payment, to copy, modify, display 
and distribute this software and its documentation, if any, for  
research purposes only, provided that (1) the above copyright notice and 
the following four paragraphs appear on all copies of this software, and 
(2) that source code to any modifications to this software be made 
publicly available under terms no more restrictive than those in this 
License Agreement. Use of this software constitutes acceptance of these 
terms and conditions.

3D Slicer Software has not been reviewed or approved by the Food and 
Drug Administration, and is for non-clinical, IRB-approved Research Use 
Only.  In no event shall data or images generated through the use of 3D 
Slicer Software be used in the provision of patient care.

IN NO EVENT SHALL THE COPYRIGHT HOLDERS AND CONTRIBUTORS BE LIABLE TO 
ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL 
DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, 
EVEN IF THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE BEEN ADVISED OF THE 
POSSIBILITY OF SUCH DAMAGE.

THE COPYRIGHT HOLDERS AND CONTRIBUTORS SPECIFICALLY DISCLAIM ANY EXPRESS 
OR IMPLIED WARRANTIES INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND 
NON-INFRINGEMENT.

THE SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS 
IS." THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE NO OBLIGATION TO 
PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.


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
#include "vtkSlicer.h"
#include "vtkMrmlDataVolumeReadWrite.h"

class vtkImageDICOMReader;
class vtkImageReader;

//----------------------------------------------------------------------------
class VTK_SLICER_BASE_EXPORT vtkMrmlDataVolume : public vtkMrmlData
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

  //--------------------------------------------------------------------------
  // Read/Write volume data contained by this object.
  // This sub-object is specific to each
  // type of volume that needs to be read in.  This can be used
  // to clean up the special cases which handle
  // volumes of various types, such as dicom, header, etc.  In
  // future these things can be moved here.  Each read/write 
  // sub-object corresponds to a vtkMrmlVolumeReadWriteNode subclass.
  // These subclasses write any needed info in the MRML file.
  //--------------------------------------------------------------------------
  vtkSetObjectMacro(ReadWrite,vtkMrmlDataVolumeReadWrite);
  vtkGetObjectMacro(ReadWrite,vtkMrmlDataVolumeReadWrite);

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
  vtkGetMacro(RangeLow,  float);
  vtkGetMacro(RangeHigh, float);
  vtkSetMacro(RangeLow,  float);
  vtkSetMacro(RangeHigh, float);
  vtkGetMacro(RangeAuto, int);
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

  float RangeLow;
  float RangeHigh;
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

 // hook for developers to add any kind of volume reading
 vtkMrmlDataVolumeReadWrite *ReadWrite;


};

#endif

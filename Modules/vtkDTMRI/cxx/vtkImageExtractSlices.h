/*=auto=========================================================================

(c) Copyright 2005 Massachusetts Institute of Technology (MIT) All Rights Reserved.

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
// .NAME vtkImageDiffusionTensor - 
// .SECTION Description

#ifndef __vtkImageExtractSlices_h
#define __vtkImageExtractSlices_h

#include "vtkDTMRIConfigure.h"
#include "vtkImageToImageFilter.h"

#define MODESLICE 0
#define MODEVOLUME 1
#define MODEMOSAIC 2

class VTK_DTMRI_EXPORT vtkImageExtractSlices : public vtkImageToImageFilter
{
 public:
  static vtkImageExtractSlices *New();
  vtkTypeMacro(vtkImageExtractSlices,vtkImageToImageFilter);

  // Description:
  // Number of the slice to extract from the volume
  // Specified as an offset that repeats once each
  // SlicePeriod.  For example if the period is 5 slices
  // and the offset is 0, we will extract slices 0,
  // 5, 10, and so on.
  vtkSetMacro(SliceOffset, int);
  vtkGetMacro(SliceOffset, int);

  // Description:
  // Slices repeat in groups of the Period throughout the volume.
  // Use in conjunction with slice offset to extract every
  // Period-th slice throughout the volume starting
  // at slice number SliceOffset.
  vtkSetMacro(SlicePeriod, int);
  vtkGetMacro(SlicePeriod, int);
  // Lauren need to make sure the period is not larger than
  // the number of slices

  vtkSetMacro(Mode, int);
  vtkGetMacro(Mode, int);

  void SetModeToSLICE(void)
  {
   this->SetMode(0);
   this->Modified();
  };

  void SetModeToVOLUME(void)
  {
  this->SetMode(1);
  this->Modified(); 
  };

  void SetModeToMOSAIC(void)
  {
  this->SetMode(2);
  this->Modified();
  }

  // Description:
  // New variables to support MOSAIC, Siemens,data
  // Number of real slices per each mosaic slice
  vtkSetMacro(MosaicSlices,int);
  vtkGetMacro(MosaicSlices,int);

  // Numbers of tiles per mosaic slice row.
  vtkSetMacro(MosaicTiles,int);
  vtkGetMacro(MosaicTiles,int);

  vtkSetMacro(NumberOfRepetitions,int);
  vtkGetMacro(NumberOfRepetitions,int);
  
  vtkSetClampMacro(Repetition,int,1,NumberOfRepetitions);
  vtkGetMacro(Repetition,int);
  
  vtkBooleanMacro(AverageRepetitions,int);
  vtkSetMacro(AverageRepetitions,int);
  vtkGetMacro(AverageRepetitions,int);

 protected:
  vtkImageExtractSlices();
  ~vtkImageExtractSlices() {};
  vtkImageExtractSlices(const vtkImageExtractSlices&) {};
  void operator=(const vtkImageExtractSlices&) {};
  void PrintSelf(ostream& os, vtkIndent indent);

  int SliceOffset;
  int SlicePeriod;
  int Mode;
  int MosaicSlices;
  int MosaicTiles;
  int NumberOfRepetitions;
  int Repetition;
  int AverageRepetitions;

  void ExecuteInformation(vtkImageData *input, 
              vtkImageData *output);
  void ComputeInputUpdateExtent(int inExt[6], 
                int outExt[6]);
  void ThreadedExecute(vtkImageData *inData, vtkImageData *outData,
               int extent[6], int id);
};

#endif





// .NAME vtkImageDiffusionTensor - 
// .SECTION Description

#ifndef __vtkImageExtractSlices_h
#define __vtkImageExtractSlices_h

#include "vtkDTMRIConfigure.h"
#include "vtkImageToImageFilter.h"

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

 protected:
  vtkImageExtractSlices();
  ~vtkImageExtractSlices() {};
  vtkImageExtractSlices(const vtkImageExtractSlices&) {};
  void operator=(const vtkImageExtractSlices&) {};
  void PrintSelf(ostream& os, vtkIndent indent);

  int SliceOffset;
  int SlicePeriod;
  void ExecuteInformation(vtkImageData *input, 
			  vtkImageData *output);
  void ComputeInputUpdateExtent(int inExt[6], 
				int outExt[6]);
  void ThreadedExecute(vtkImageData *inData, vtkImageData *outData,
               int extent[6], int id);
};

#endif





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
// .NAME vtkImageMeasureVoxels - This filter counts labeled voxels.
// .SECTION Description
// vtkImageMeasureVoxels - Counts voxels for each label, then converts
// to mL.  Output is written to a file.
//


#ifndef __vtkImageMeasureVoxels_h
#define __vtkImageMeasureVoxels_h


#include "vtkImageToImageFilter.h"
#include "vtkSlicer.h"

class VTK_SLICER_BASE_EXPORT vtkImageMeasureVoxels : public vtkImageToImageFilter
{
public:
  static vtkImageMeasureVoxels *New();
  vtkTypeMacro(vtkImageMeasureVoxels,vtkImageToImageFilter);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description: 
  // Set/Get the filename where the measurements will be written
  vtkSetStringMacro(FileName);
  vtkGetStringMacro(FileName);


protected:
  vtkImageMeasureVoxels();
  ~vtkImageMeasureVoxels();
  vtkImageMeasureVoxels(const vtkImageMeasureVoxels&) {};
  void operator=(const vtkImageMeasureVoxels&) {};

  char *FileName;

  void Execute(vtkImageData *inData, vtkImageData *outData);

  // Description:
  // Generate more than requested.  Called by the superclass before
  // an execute, and before output memory is allocated.
  void EnlargeOutputUpdateExtents( vtkDataObject *data );

};

#endif




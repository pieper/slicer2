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
// .NAME vtkImageReplaceRegion - replaces a region in the input
// with this->Region.
// .SECTION Description
// vtkImageReplaceRegion - Used in vtkImageEditor.cxx to replace
// a 2D region with the output from an editor effect.  (For
// editing on a slice-by-slice basis.)

#ifndef __vtkImageReplaceRegion_h
#define __vtkImageReplaceRegion_h

#include "vtkImageInPlaceFilter.h"

#include "vtkIntArray.h"
#include "vtkImageData.h"

class VTK_EXPORT vtkImageReplaceRegion : public vtkImageInPlaceFilter
{
public:
  static vtkImageReplaceRegion *New();
  vtkTypeMacro(vtkImageReplaceRegion,vtkImageInPlaceFilter);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  //
  vtkGetObjectMacro(Region, vtkImageData);
  vtkSetObjectMacro(Region, vtkImageData);

  // Description:
  //
  vtkSetObjectMacro(Indices, vtkIntArray);
  vtkGetObjectMacro(Indices, vtkIntArray);

protected:
  vtkImageReplaceRegion();
  ~vtkImageReplaceRegion();
  vtkImageReplaceRegion(const vtkImageReplaceRegion&) {};
  void operator=(const vtkImageReplaceRegion&) {};

  vtkIntArray *Indices;
  vtkImageData *Region;

  void Execute(vtkImageData *inData, vtkImageData *outData);
};

#endif




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
// .NAME vtkMrmlDataVolumeReadWrite - 
// .SECTION Description
// This sub-object is specific to each
// type of volume that needs to be read in.  This can be used
// to clean up the special cases which handle
// volumes of various types, such as dicom, header, etc.  In
// future these things can be moved here.  Each read/write 
// sub-object corresponds to a vtkMrmlVolumeReadWriteNode subclass.
// These subclasses write any needed info in the MRML file.
//

#ifndef __vtkMrmlDataVolumeReadWrite_h
#define __vtkMrmlDataVolumeReadWrite_h

#include "vtkObject.h"
#include "vtkSlicer.h"
#include "vtkMrmlVolumeNode.h"
#include "vtkImageSource.h"

class VTK_SLICER_BASE_EXPORT vtkMrmlDataVolumeReadWrite : public vtkObject
{
  public:
  static vtkMrmlDataVolumeReadWrite *New();
  vtkTypeMacro(vtkMrmlDataVolumeReadWrite,vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  // Return code indicates success (1) or error (0)
  virtual int Read(vtkMrmlVolumeNode *node, vtkImageSource **output);
  virtual int Write(vtkMrmlVolumeNode *node, vtkImageData *input);

  //--------------------------------------------------------------------------
  // Specifics for reading/writing each type of volume data
  //--------------------------------------------------------------------------

  // Subclasses must fill these in.

protected:
  vtkMrmlDataVolumeReadWrite();
  ~vtkMrmlDataVolumeReadWrite();
  vtkMrmlDataVolumeReadWrite(const vtkMrmlDataVolumeReadWrite&) {};
  void operator=(const vtkMrmlDataVolumeReadWrite&) {};
};

#endif

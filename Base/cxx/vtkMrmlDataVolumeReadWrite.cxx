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
#include "vtkMrmlDataVolumeReadWrite.h"
#include "vtkObjectFactory.h"

  //------------------------------------------------------------------------------
  vtkMrmlDataVolumeReadWrite* vtkMrmlDataVolumeReadWrite::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMrmlDataVolumeReadWrite");
  if(ret)
  {
    return (vtkMrmlDataVolumeReadWrite*)ret;
  }
  // If the factory was unable to create the object, then create it here.
  return new vtkMrmlDataVolumeReadWrite;
}

//----------------------------------------------------------------------------
vtkMrmlDataVolumeReadWrite::vtkMrmlDataVolumeReadWrite()
{

}

//----------------------------------------------------------------------------
vtkMrmlDataVolumeReadWrite::~vtkMrmlDataVolumeReadWrite()
{

}

//----------------------------------------------------------------------------
void vtkMrmlDataVolumeReadWrite::PrintSelf(ostream& os, vtkIndent indent)
{

}

//----------------------------------------------------------------------------
//
// Do all the reading.
// Return the vtkImageSource already updated
// to be used by vtkMrmlDataVolume->Read
//
int vtkMrmlDataVolumeReadWrite::Read(vtkMrmlVolumeNode *node, 
                                     vtkImageSource **output)
{
  output = NULL;
  // return success
  return 1;
}


//----------------------------------------------------------------------------
int vtkMrmlDataVolumeReadWrite::Write(vtkMrmlVolumeNode *node,
                                      vtkImageData *input)
{
  // return success
  return 1;
}

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
#include "vtkMrmlDataVolumeReadWriteStructuredPoints.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkStructuredPointsReader.h"
#include "vtkStructuredPointsWriter.h"

  //------------------------------------------------------------------------------
  vtkMrmlDataVolumeReadWriteStructuredPoints* vtkMrmlDataVolumeReadWriteStructuredPoints::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMrmlDataVolumeReadWriteStructuredPoints");
  if(ret)
  {
    return (vtkMrmlDataVolumeReadWriteStructuredPoints*)ret;
  }
  // If the factory was unable to create the object, then create it here.
  return new vtkMrmlDataVolumeReadWriteStructuredPoints;
}

//----------------------------------------------------------------------------
vtkMrmlDataVolumeReadWriteStructuredPoints::vtkMrmlDataVolumeReadWriteStructuredPoints()
{
  this->FileName = NULL;
}

//----------------------------------------------------------------------------
vtkMrmlDataVolumeReadWriteStructuredPoints::~vtkMrmlDataVolumeReadWriteStructuredPoints()
{

}

//----------------------------------------------------------------------------
void vtkMrmlDataVolumeReadWriteStructuredPoints::PrintSelf(ostream& os, vtkIndent indent)
{

}

//----------------------------------------------------------------------------
//
// Do all the reading.
// Return the vtkImageSource already updated
// to be used by vtkMrmlDataVolume->Read
//
int vtkMrmlDataVolumeReadWriteStructuredPoints::Read(vtkMrmlVolumeNode *node, 
                                                     vtkImageSource **output)
{
  vtkStructuredPointsReader* reader = vtkStructuredPointsReader::New();
  reader->DebugOn();
  reader->SetFileName(this->FileName);
  reader->Update();

  // perhaps we should tell the reader this info,
  // but the default may be worse than the reader's default.
  //node->GetLittleEndian();  

  // return pointer to the reader object
  *output = (vtkImageSource *) reader;

  // set up things in the node that are specified by the file
  int ext[6];
  vtkStructuredPoints *sp =   reader->GetOutput();
  sp->GetExtent(ext);
  node->SetImageRange(ext[4],ext[5]);
  node->SetDimensions(ext[1]-ext[0]+1,ext[3]-ext[2]+1);
  node->SetSpacing(sp->GetSpacing());
  node->SetScalarType(sp->GetScalarType());
  node->SetNumScalars(sp->GetPointData()->GetScalars()->GetNumberOfComponents());
  // Set up things in the node that may have required user input.
  // These things should be set in the node from the GUI 
  // before reading in the volume, since this info is not in a 
  // structured points file
  //node->SetLittleEndian();  // was set from GUI already
  //node->SetTilt();   // was set from GUI
  // this should be set in the node from GUI input before reading file
  node->ComputeRasToIjkFromScanOrder(node->GetScanOrder());

  // return success
  return 1;
}


//----------------------------------------------------------------------------
int vtkMrmlDataVolumeReadWriteStructuredPoints::Write(vtkMrmlVolumeNode *node,
                                                      vtkImageData *input)
{
  vtkStructuredPointsWriter* writer = vtkStructuredPointsWriter::New();
  writer->DebugOn();
  writer->SetFileName(this->FileName);
  writer->SetInput(input);
  writer->Update();

  // return success
  return 1;
}

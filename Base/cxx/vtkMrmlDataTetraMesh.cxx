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
#include <stdio.h>
#include "vtkCommand.h"
#include "vtkMrmlDataTetraMesh.h"
#include "vtkMrmlTetraMeshNode.h"
#include "vtkObjectFactory.h"
#include "vtkUnstructuredGridReader.h"
#include "vtkUnstructuredGridWriter.h"
#include <time.h>

//------------------------------------------------------------------------------
vtkMrmlDataTetraMesh* vtkMrmlDataTetraMesh::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMrmlDataTetraMesh");
  if(ret)
  {
    return (vtkMrmlDataTetraMesh*)ret;
  }
  // If the factory was unable to create the object, then create it here.
  return new vtkMrmlDataTetraMesh;
}

//----------------------------------------------------------------------------
vtkMrmlDataTetraMesh::vtkMrmlDataTetraMesh()
{
  // Allocate VTK objects
  this->TheMesh = NULL;
}

//----------------------------------------------------------------------------
vtkMrmlDataTetraMesh::~vtkMrmlDataTetraMesh()
{
  // Delete if self-created or if no one else is using it
  if (this->TheMesh != NULL) 
  {
    this->TheMesh->UnRegister(this);
  }

}

//----------------------------------------------------------------------------
void vtkMrmlDataTetraMesh::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkMrmlData::PrintSelf(os, indent);

  os << indent << "Volume Mesh: " << this->TheMesh << "\n";
  if (this->TheMesh)
  {
    this->TheMesh->PrintSelf(os,indent.GetNextIndent());
  }
}

//----------------------------------------------------------------------------
// Determine the modified time of this object
unsigned long int vtkMrmlDataTetraMesh::GetMTime()
{
  unsigned long result, t;

  result = vtkMrmlData::GetMTime();
 
  // The Mesh
  if (this->TheMesh)
  {
    t = this->TheMesh->GetMTime(); 
    result = (t > result) ? t : result;
  }

  return result;
}

//----------------------------------------------------------------------------
void vtkMrmlDataTetraMesh::CheckMrmlNode()
{
  // If the user has not set the ImageData, then create it.
  // The key is to perform: New(), Register(), Delete().
  // Then we can call UnRegister() in the destructor, and it will delete
  // the object if no one else is using it.  We don't have to distinguish
  // between whether we created the object, or someone else did!

  if (this->MrmlNode == NULL)
  {
    this->MrmlNode = (vtkMrmlNode*) vtkMrmlTetraMeshNode::New();
    this->MrmlNode->Register(this);
    this->MrmlNode->Delete();
  }
}

//----------------------------------------------------------------------------
void vtkMrmlDataTetraMesh::Update()
{
  int ext[6];

  this->vtkMrmlData::Update();
  
  // We really should have an Update time that we compare with the
  // MTime, but since the other objects inside this class do this, 
  // its alright.


}

//----------------------------------------------------------------------------
vtkUnstructuredGrid* vtkMrmlDataTetraMesh::GetOutput()
{
  this->Update();
  return this->TheMesh;
}

//----------------------------------------------------------------------------
int vtkMrmlDataTetraMesh::Read()
{
  vtkMrmlTetraMeshNode *node = (vtkMrmlTetraMeshNode*) this->MrmlNode;

  vtkUnstructuredGridReader *reader = vtkUnstructuredGridReader::New();
  reader->SetFileName(node->GetFileName());
  reader->Update();

  // Detach image data from reader
  this->SetTheMesh(reader->GetOutput());
  reader->SetOutput(NULL);
  this->ProcessObject = NULL;
  reader->Delete();

  // End progress reporting
  this->InvokeEvent(vtkCommand::EndEvent,NULL);

  // Update W/L
  this->Update();

  // Right now how no way to deal with failure
  return 1;
}


//----------------------------------------------------------------------------
int vtkMrmlDataTetraMesh::Write()
{

  this->CheckMrmlNode();
  vtkMrmlTetraMeshNode *node = (vtkMrmlTetraMeshNode*) this->MrmlNode;
  
  // Start progress reporting
  this->InvokeEvent(vtkCommand::StartEvent,NULL);

  // Set up the image writer
  vtkUnstructuredGridWriter *writer = vtkUnstructuredGridWriter::New();
  writer->SetFileName(node->GetFileName());
  writer->SetInput(this->TheMesh);
  
  // Progress callback
  writer->SetProgressMethod(vtkMrmlData::vtkMrmlDataProgress, (void *)this);
  // The progress callback function needs a handle to the writer 
  this->ProcessObject = writer;
 
  // Write it
  writer->Write();

  writer->SetInput(NULL);
  writer->Delete();

  // End progress reporting
  this->InvokeEvent(vtkCommand::EndEvent,NULL);

  // Right now how no way to deal with failure
  return 1;
}

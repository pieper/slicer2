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
#include <stdio.h>
#include "vtkCommand.h"
#include "vtkCallbackCommand.h"
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

  // Setup a callback for the internal writer to report progress.
  this->ProgressObserver = vtkCallbackCommand::New();
  this->ProgressObserver->SetCallback(&vtkMrmlData::ProgressCallbackFunction);
  this->ProgressObserver->SetClientData(this);
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
  
#ifndef SLICER_VTK5
    // TODO -- need fix for vtk 5
  // Progress callback
  writer->AddObserver (vtkCommand::ProgressEvent,
                       this->ProgressObserver);
  // The progress callback function needs a handle to the writer 
  this->ProcessObject = writer;
#endif
 
  // Write it
  writer->Write();

  writer->SetInput(NULL);
  writer->Delete();

  // End progress reporting
  this->InvokeEvent(vtkCommand::EndEvent,NULL);

  // Right now how no way to deal with failure
  return 1;
}

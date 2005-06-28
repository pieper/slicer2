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
#include "vtkSaveTracts.h"

#include "vtkAppendPolyData.h"
#include "vtkPolyDataWriter.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkMrmlModelNode.h"

#include "vtkPolyDataSource.h"
#include "vtkPolyData.h"

#include <sstream>

#include "vtkImageWriter.h"

//------------------------------------------------------------------------------
vtkSaveTracts* vtkSaveTracts::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkSaveTracts");
  if(ret)
    {
      return (vtkSaveTracts*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkSaveTracts;
}

//----------------------------------------------------------------------------
vtkSaveTracts::vtkSaveTracts()
{
  // Initialize these to identity, so if the user doesn't set them it's okay.
  this->WorldToTensorScaledIJK = vtkTransform::New();

  // collections
  this->Streamlines = NULL;
  this->TubeFilters = NULL;

}

//----------------------------------------------------------------------------
vtkSaveTracts::~vtkSaveTracts()
{
  this->WorldToTensorScaledIJK->Delete();
  if (this->Streamlines) this->Streamlines->Delete();
  if (this->TubeFilters) this->TubeFilters->Delete();
}


void vtkSaveTracts::SaveStreamlinesAsPolyData(char *filename,
                                                                char *name)
{
  this->SaveStreamlinesAsPolyData(filename, name, NULL);
}

// NOTE: Limit currently is 1000 models (1000 different input colors is max).
//----------------------------------------------------------------------------
void vtkSaveTracts::SaveStreamlinesAsPolyData(char *filename,
                                              char *name,
                                              vtkMrmlTree *colorTree)
{
  vtkPolyData *currStreamline;
  vtkCollection *collectionOfModels;
  vtkAppendPolyData *currAppender;
  vtkPolyDataWriter *writer;
  vtkTransformPolyDataFilter *currTransformer;
  vtkTransform *currTransform;
  std::stringstream fileNameStr;
  vtkMrmlTree *tree;
  vtkMrmlModelNode *currNode;
  vtkMrmlColorNode *currColorNode;
  std::stringstream colorNameStr;
  vtkMrmlTree *colorTreeTwo;
  vtkPolyDataSource *source;

  // traverse streamline collection
  this->TubeFilters->InitTraversal();
  source = dynamic_cast <vtkPolyDataSource *> 
    (this->TubeFilters->GetNextItemAsObject());

  // test we have actors and streamlines
  if (currStreamline == NULL)
    {
      vtkErrorMacro("No streamlines have been created yet.");
      return;      
    }

  // init things with the first streamline.
  currAppender = vtkAppendPolyData::New();
  collectionOfModels = vtkCollection::New();
  collectionOfModels->AddItem((vtkObject *)currAppender);

  vtkDebugMacro("Traverse STREAMLINES");
  while(source)
    {
      
      currStreamline = source->GetOutput();
      vtkDebugMacro("stream " << currStreamline);

      // transform models so that they are written in the coordinate
      // system in which they are displayed.
      // Currently this class displays them with 
      // currActor->SetUserMatrix(currTransform->GetMatrix());
      currTransformer = vtkTransformPolyDataFilter::New();
      currTransform = vtkTransform::New();
      
      // CONVERT this to get the right matrix
      //currTransform->SetMatrix(currActor->GetUserMatrix());

      currTransformer->SetTransform(currTransform);
      currTransformer->SetInput(currStreamline);

      // Append this streamline to the chosen model using the appender
      currAppender->AddInput(currTransformer->GetOutput());

      // call Delete on both to decrement the reference count
      // so that when we delete the appender they delete too.
      currTransformer->Delete();
      currTransform->Delete();

      // get next objects in collections
      source = dynamic_cast <vtkPolyDataSource *> 
        (this->TubeFilters->GetNextItemAsObject());
    }


  // traverse appender collection (collectionOfModels) and write each to disk
  vtkDebugMacro("Traverse APPENDERS");

  writer = vtkPolyDataWriter::New();
  tree = vtkMrmlTree::New();
  // object to hold any new colors we encounter (not on input color tree)
  colorTreeTwo = vtkMrmlTree::New();

  collectionOfModels->InitTraversal();
  currAppender = (vtkAppendPolyData *) 
    collectionOfModels->GetNextItemAsObject();
  int idx=0;
  while(currAppender)
    {
      vtkDebugMacro("appender index: " << idx);

      writer->SetInput(currAppender->GetOutput());
      writer->SetFileType(2);
      // clear the buffer (set to empty string)
      fileNameStr.str("");
      fileNameStr << name << '_' << idx << ".vtk";
      writer->SetFileName(fileNameStr.str().c_str());
      writer->Write();
      
      // Delete it (but it survives until the collection it's on is deleted).
      currAppender->Delete();

      // Also write a MRML file: add to MRML tree
      currNode=vtkMrmlModelNode::New();
      currNode->SetFullFileName(fileNameStr.str().c_str());
      currNode->SetFileName(fileNameStr.str().c_str());
      currNode->BackfaceCullingOff();
      // use the name argument to name the model (name label in slicer GUI)
      fileNameStr.str("");
      fileNameStr << name << '_' << idx;
      currNode->SetName(fileNameStr.str().c_str());
      currNode->SetDescription("Model of a DTMRI tract");

      // CONVERT this to use some info in this class
      //if (this->ScalarVisibility) currNode->ScalarVisibilityOn();

      currNode->ClippingOn();

      // CONVERT this to use some info in this class
      //currNode->SetScalarRange(this->StreamlineLookupTable->GetTableRange());

      // Find the name of the color if it's on the input color tree.
      // CONVERT this to use the new LUT and tract group IDs
      // tie the node to its color

      // add it to the MRML file
      tree->AddItem(currNode);

      currAppender = (vtkAppendPolyData *) 
        collectionOfModels->GetNextItemAsObject();
      idx++;
    } 

  // If we had color inputs put them at the end of the MRML file
  if (colorTree)
    {
      colorTree->InitTraversal();
      currColorNode = (vtkMrmlColorNode *) colorTree->GetNextItemAsObject();      
      while (currColorNode)
        {
          tree->AddItem(currColorNode);
          currColorNode = (vtkMrmlColorNode *) 
            colorTree->GetNextItemAsObject();
        }
    }
  
  // CONVERT this to use the new LUT and tract group labels
  // Also add any new colors we found
  colorTreeTwo->InitTraversal();
  currColorNode = (vtkMrmlColorNode *) colorTreeTwo->GetNextItemAsObject();      
  while (currColorNode)
    {
      tree->AddItem(currColorNode);
      currColorNode = (vtkMrmlColorNode *) 
        colorTreeTwo->GetNextItemAsObject();
    }
  
  // Write the MRML file
  fileNameStr.str("");
  fileNameStr << filename << ".xml";
  tree->Write((char *)fileNameStr.str().c_str());

  cout << "DELETING" << endl;

  // Delete all objects we created
  collectionOfModels->Delete();
  writer->Delete();
  tree->Delete();
  colorTreeTwo->Delete();

}




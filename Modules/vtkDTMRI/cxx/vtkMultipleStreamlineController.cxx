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
#include "vtkMultipleStreamlineController.h"
#include "vtkLookupTable.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkRenderer.h"
#include "vtkFloatArray.h"
#include "vtkTubeFilter.h"
#include "vtkPointData.h"

#include "vtkHyperStreamline.h"
#include "vtkHyperStreamlinePoints.h"
#include "vtkPreciseHyperStreamlinePoints.h"

#include <sstream>

#include "vtkImageWriter.h"

//------------------------------------------------------------------------------
vtkMultipleStreamlineController* vtkMultipleStreamlineController::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMultipleStreamlineController");
  if(ret)
    {
      return (vtkMultipleStreamlineController*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkMultipleStreamlineController;
}

//----------------------------------------------------------------------------
vtkMultipleStreamlineController::vtkMultipleStreamlineController()
{
  // Initialize these to identity, so if the user doesn't set them it's okay.
  this->WorldToTensorScaledIJK = vtkTransform::New();

  // The user must set these for the class to function.
  this->InputTensorField = NULL;
  this->InputRenderers = vtkCollection::New();
  
  // collections
  this->Streamlines = vtkCollection::New();
  this->Mappers = vtkCollection::New();
  this->TubeFilters = vtkCollection::New();
  this->Actors = vtkCollection::New();

  // Streamline parameters for all streamlines
  this->ScalarVisibility=0;

  // user-accessible property and lookup table for all streamlines
  this->StreamlineProperty = vtkProperty::New();
  this->StreamlineLookupTable = vtkLookupTable::New();
  // default: make 0 dark blue, not red
  this->StreamlineLookupTable->SetHueRange(.6667, 0.0);

  // the number of actors displayed in the scene
  this->NumberOfVisibleActors=0;

  // radius of the tube which is displayed
  this->TubeRadius = 0.5;
 
  // Helper classes
  // ---------------

  // for tract clustering
  this->TractClusterer = vtkClusterTracts::New();

  // for tract saving
  this->SaveTracts = vtkSaveTracts::New();

  // for creating tracts
  this->SeedTracts = vtkSeedTracts::New();

  // for coloring an ROI based on tract color
  this->ColorROIFromTracts = vtkColorROIFromTracts::New();

  // Helper class pipelines
  // ----------------------
  this->SaveTracts->SetStreamlines(this->Streamlines);
  this->SaveTracts->SetTubeFilters(this->TubeFilters);
  this->SaveTracts->SetActors(this->Actors);

  this->SeedTracts->SetStreamlines(this->Streamlines);

  this->ColorROIFromTracts->SetStreamlines(this->Streamlines);
  this->ColorROIFromTracts->SetActors(this->Actors);
}

//----------------------------------------------------------------------------
vtkMultipleStreamlineController::~vtkMultipleStreamlineController()
{
  this->DeleteAllStreamlines();

  this->WorldToTensorScaledIJK->Delete();
  if (this->InputTensorField) this->InputTensorField->Delete();

  this->InputRenderers->Delete();
  this->Streamlines->Delete();
  this->Mappers->Delete();
  this->Actors->Delete();
  this->TubeFilters->Delete();

  this->StreamlineLookupTable->Delete();

  // delete helper classes
  this->TractClusterer->Delete();
  this->SaveTracts->Delete();  
  this->SeedTracts->Delete();  
  this->ColorROIFromTracts->Delete(); 
}


//----------------------------------------------------------------------------
void vtkMultipleStreamlineController::SetInputTensorField(vtkImageData *tensorField)
{
  
  vtkDebugMacro("Setting input tensor field.");

  // Decrease reference count of old object
  if (this->InputTensorField != 0)
    this->InputTensorField->UnRegister(this);

  // Set new value in this class
  this->InputTensorField = tensorField;

  // Increase reference count of new object
  if (this->InputTensorField != 0)
    this->InputTensorField->Register(this);

  // This class has changed
  this->Modified();

  // helper class pipelines
  // ----------------------
  this->SaveTracts->SetInputTensorField(this->InputTensorField);
  this->SeedTracts->SetInputTensorField(this->InputTensorField);

}

//----------------------------------------------------------------------------
void vtkMultipleStreamlineController::SetWorldToTensorScaledIJK(vtkTransform *trans)
{
  
  vtkDebugMacro("Setting WorldToTensorScaledIJK.");

  // Decrease reference count of old object
  if (this->WorldToTensorScaledIJK != 0)
    this->WorldToTensorScaledIJK->UnRegister(this);

  // Set new value in this class
  this->WorldToTensorScaledIJK = trans;

  // Increase reference count of new object
  if (this->WorldToTensorScaledIJK != 0)
    this->WorldToTensorScaledIJK->Register(this);

  // This class has changed
  this->Modified();

  // helper class pipelines
  // ----------------------
  this->SaveTracts->SetWorldToTensorScaledIJK(this->WorldToTensorScaledIJK);
  this->SeedTracts->SetWorldToTensorScaledIJK(this->WorldToTensorScaledIJK);
  this->ColorROIFromTracts->SetWorldToTensorScaledIJK(this->WorldToTensorScaledIJK);
}

//----------------------------------------------------------------------------
void vtkMultipleStreamlineController::SetTensorRotationMatrix(vtkMatrix4x4 *trans)
{

  // helper class pipelines
  // ----------------------
  this->SaveTracts->SetTensorRotationMatrix(trans);
}


//----------------------------------------------------------------------------
void vtkMultipleStreamlineController::SetScalarVisibility(int value)
{
  vtkPolyDataMapper *currMapper;

  // test if we are changing the value before looping through all streamlines
  if (this->ScalarVisibility != value)
    {
      this->ScalarVisibility = value;
      // apply this to ALL streamlines
      // traverse actor collection and make all visible
      this->Mappers->InitTraversal();
      currMapper= (vtkPolyDataMapper *)this->Mappers->GetNextItemAsObject();
      while(currMapper)
        {
          currMapper->SetScalarVisibility(this->ScalarVisibility);
          currMapper= (vtkPolyDataMapper *)this->Mappers->GetNextItemAsObject();      
        }
    }
}

// Set the properties of one streamline's graphics objects as requested
// by the user
//----------------------------------------------------------------------------
void vtkMultipleStreamlineController::ApplyUserSettingsToGraphicsObject(int index)
{
  vtkPolyDataMapper *currMapper;
  vtkActor *currActor;
  vtkTubeFilter *currTubeFilter;

  currActor = (vtkActor *) this->Actors->GetItemAsObject(index);
  currMapper = (vtkPolyDataMapper *) this->Mappers->GetItemAsObject(index);
  currTubeFilter = (vtkTubeFilter *) this->TubeFilters->GetItemAsObject(index);

  // set the Actor's properties according to the sample 
  // object that the user can access.
  currActor->GetProperty()->SetAmbient(this->StreamlineProperty->GetAmbient());
  currActor->GetProperty()->SetDiffuse(this->StreamlineProperty->GetDiffuse());
  currActor->GetProperty()->SetSpecular(this->StreamlineProperty->GetSpecular());
  currActor->GetProperty()->
    SetSpecularPower(this->StreamlineProperty->GetSpecularPower());
  currActor->GetProperty()->SetColor(this->StreamlineProperty->GetColor());    
  // Set the scalar visibility as desired by the user
  currMapper->SetScalarVisibility(this->ScalarVisibility);
  // Set the tube width as desired by the user
  currTubeFilter->SetRadius(this->TubeRadius);

}

// Make actors, mappers, and lookup tables as needed for streamlines
// in the collection.
//----------------------------------------------------------------------------
void vtkMultipleStreamlineController::CreateGraphicsObjects()
{
  int numStreamlines, numActorsCreated;
  vtkHyperStreamline *currStreamline;
  vtkPolyDataMapper *currMapper;
  vtkActor *currActor;
  vtkTransform *currTransform;
  vtkRenderer *currRenderer;
  vtkTubeFilter *currTubeFilter;

  // Find out how many streamlines we have, and if they all have actors
  numStreamlines = this->Streamlines->GetNumberOfItems();
  numActorsCreated = this->Actors->GetNumberOfItems();

  vtkDebugMacro(<< "in CreateGraphicsObjects " << numActorsCreated << "  " << numStreamlines);

  // If we have already made all of the object needed, stop here.
  if (numActorsCreated == numStreamlines) 
    return;

  // Create transformation matrix to place actors in scene
  currTransform=vtkTransform::New();
  currTransform->SetMatrix(this->WorldToTensorScaledIJK->GetMatrix());
  currTransform->Inverse();

  // Make actors and etc. for all streamlines that need them
  while (numActorsCreated < numStreamlines) 
    {
      // Get the streamline
      currStreamline = (vtkHyperStreamline *)
        this->Streamlines->GetItemAsObject(numActorsCreated);

      // Now create the objects needed
      currActor = vtkActor::New();
      this->Actors->AddItem((vtkObject *)currActor);
      currMapper = vtkPolyDataMapper::New();
      this->Mappers->AddItem((vtkObject *)currMapper);
      currTubeFilter = vtkTubeFilter::New();
      this->TubeFilters->AddItem((vtkObject *)currTubeFilter);

      // Apply user's visualization settings to these objects
      this->ApplyUserSettingsToGraphicsObject(numActorsCreated);

      // Hook up the pipeline
      currTubeFilter->SetInput(currStreamline->GetOutput());
      currMapper->SetInput(currTubeFilter->GetOutput());
      currMapper->SetLookupTable(this->StreamlineLookupTable);
      currMapper->UseLookupTableScalarRangeOn();
      currActor->SetMapper(currMapper);
      
      // Place the actor correctly in the scene
      currActor->SetUserMatrix(currTransform->GetMatrix());

      // add to the scene (to each renderer)
      // This is the same as MainAddActor in Main.tcl.
      this->InputRenderers->InitTraversal();
      currRenderer= (vtkRenderer *)this->InputRenderers->GetNextItemAsObject();
      while(currRenderer)
        {
          currRenderer->AddActor(currActor);
          currRenderer= (vtkRenderer *)this->InputRenderers->GetNextItemAsObject();
        }
      
      // Increment the count of actors we have created
      numActorsCreated++;
    }


  // For debugging print this info again
  // Find out how many streamlines we have, and if they all have actors
  numStreamlines = this->Streamlines->GetNumberOfItems();
  numActorsCreated = this->Actors->GetNumberOfItems();

  vtkDebugMacro(<< "in CreateGraphicsObjects " << numActorsCreated << "  " << numStreamlines);

}

//----------------------------------------------------------------------------
void vtkMultipleStreamlineController::AddStreamlinesToScene()
{
  vtkActor *currActor;
  int index;

  // make objects if needed
  this->CreateGraphicsObjects();

  // traverse actor collection and make all visible
  // only do the ones that are not visible now
  // this code assumes any invisible ones are at the end of the
  // list since they were just created.
  index = this->NumberOfVisibleActors;
  while (index < this->Actors->GetNumberOfItems())
    {
      currActor = (vtkActor *) this->Actors->GetItemAsObject(index);
      currActor->VisibilityOn();
      index++;
    }

  // the number of actors displayed in the scene
  this->NumberOfVisibleActors=this->Actors->GetNumberOfItems();
}



//----------------------------------------------------------------------------
void vtkMultipleStreamlineController::RemoveStreamlinesFromScene()
{
  vtkActor *currActor;

  // traverse actor collection and make all invisible
  this->Actors->InitTraversal();
  currActor= (vtkActor *)this->Actors->GetNextItemAsObject();
  while(currActor)
    {
      currActor->VisibilityOff();
      currActor= (vtkActor *)this->Actors->GetNextItemAsObject();      
    }

  // the number of actors displayed in the scene
  this->NumberOfVisibleActors=0;
}

//----------------------------------------------------------------------------
void vtkMultipleStreamlineController::DeleteAllStreamlines()
{
  int numStreamlines, i;

  i=0;
  numStreamlines = this->Streamlines->GetNumberOfItems();
  while (i < numStreamlines)
    {
      vtkDebugMacro( << "Deleting streamline " << i);
      // always delete the first streamline from the collections
      // (they change size as we do this, shrinking away)
      this->DeleteStreamline(0);
      i++;
    }
  
}

// Delete one streamline and all of its associated objects.
//----------------------------------------------------------------------------
void vtkMultipleStreamlineController::DeleteStreamline(int index)
{
  vtkRenderer *currRenderer;
  //vtkLookupTable *currLookupTable;
  vtkPolyDataMapper *currMapper;
  vtkTubeFilter *currTubeFilter;
  vtkHyperStreamline *currStreamline;
  vtkActor *currActor;

  vtkDebugMacro( << "Deleting actor " << index);
  currActor = (vtkActor *) this->Actors->GetItemAsObject(index);
  if (currActor != NULL)
    {
      currActor->VisibilityOff();
      this->NumberOfVisibleActors--;
      // Remove from the scene (from each renderer)
      // Just like MainRemoveActor in Main.tcl.
      this->InputRenderers->InitTraversal();
      currRenderer= (vtkRenderer *)this->InputRenderers->GetNextItemAsObject();
      while(currRenderer)
        {
          vtkDebugMacro( << "rm actor from renderer " << currRenderer);
          currRenderer->RemoveActor(currActor);
          currRenderer= (vtkRenderer *)this->InputRenderers->GetNextItemAsObject();
        }
      this->Actors->RemoveItem(index);
      currActor->Delete();
    }
  vtkDebugMacro( << "Delete stream" );
  currStreamline = (vtkHyperStreamline *)
    this->Streamlines->GetItemAsObject(index);
  if (currStreamline != NULL)
    {
      this->Streamlines->RemoveItem(index);
      currStreamline->Delete();
    }

  vtkDebugMacro( << "Delete mapper" );
  currMapper = (vtkPolyDataMapper *) this->Mappers->GetItemAsObject(index);
  if (currMapper != NULL)
    {
      this->Mappers->RemoveItem(index);
      currMapper->Delete();
    }

  vtkDebugMacro( << "Delete tubeFilter" );
  currTubeFilter = (vtkTubeFilter *) this->TubeFilters->GetItemAsObject(index);
  if (currTubeFilter != NULL)
    {
      this->TubeFilters->RemoveItem(index);
      currTubeFilter->Delete();
    }

  vtkDebugMacro( << "Done deleting streamline");

}

// This is the delete called from the user interface where the
// actor has been picked with the mouse.
//----------------------------------------------------------------------------
void vtkMultipleStreamlineController::DeleteStreamline(vtkActor *pickedActor)
{
  int index;

  index = this->GetStreamlineIndexFromActor(pickedActor);

  if (index >=0)
    {
      this->DeleteStreamline(index);
    }
}

// Get the index into all of the collections corresponding to the picked
// actor.
//----------------------------------------------------------------------------
int vtkMultipleStreamlineController::GetStreamlineIndexFromActor(vtkActor *pickedActor)
{
  int index;

  vtkDebugMacro( << "Picked actor (present?): " << pickedActor);
  // find the actor on the collection.
  // nonzero index means item was found.
  index = this->Actors->IsItemPresent(pickedActor);

  // the index returned was 1-based but actually to get items
  // from the list we must use 0-based indices.  Very
  // strange but this is necessary.
  index--;

  // so now "not found" is -1, and >=0 are valid indices
  return(index);
}



// Call the tract clustering object, and then color our hyperstreamlines
// according to their cluster numbers.
//----------------------------------------------------------------------------
void vtkMultipleStreamlineController::ClusterTracts(int tmp)
{
  if (this->Streamlines == 0)
    {
      vtkErrorMacro("Streamlines are NULL.");
      return;      
    }

  if (this->Streamlines->GetNumberOfItems() < 1)
    {
      vtkErrorMacro("No streamlines exist.");
      return;      
    }

  // First make sure none of the streamlines have 0 length
  this->CleanStreamlines();

  int numberOfClusters= this->TractClusterer->GetNumberOfClusters();

  this->TractClusterer->SetInputStreamlines(this->Streamlines);
  this->TractClusterer->ComputeClusters();

  vtkClusterTracts::OutputType *clusters =  this->TractClusterer->GetOutput();

  if (clusters == 0)
    {
      vtkErrorMacro("Error: clusters have not been computed.");
      return;      
    }

  // Color tracts based on class membership...
  vtkLookupTable *lut = vtkLookupTable::New();
  lut->SetTableRange (0, numberOfClusters-1);
  lut->SetNumberOfTableValues (numberOfClusters);
  lut->Build();

  double rgba[4];
  vtkActor *currActor;
  vtkPolyDataMapper *currMapper;
  for (int idx = 0; idx < clusters->GetNumberOfTuples(); idx++)
    {
      vtkDebugMacro("index = " << idx << "class label = " << clusters->GetValue(idx));
      
      currActor = (vtkActor *) this->Actors->GetItemAsObject(idx);
      currMapper = (vtkPolyDataMapper *) this->Mappers->GetItemAsObject(idx);

      if (currActor && currMapper) 
        {
          lut->GetColor(clusters->GetValue(idx),rgba);
          vtkDebugMacro("rgb " << rgba[0] << " " << rgba[1] << " " << rgba[2]); 
          currActor->GetProperty()->SetColor(rgba[0],rgba[1],rgba[2]); 
          currMapper->SetScalarVisibility(0);
        }
      else
        {
          vtkErrorMacro("Classified actor " << idx << " not found.");
        }
    }

}

// Remove any streamlines with 0 length
//----------------------------------------------------------------------------
void vtkMultipleStreamlineController::CleanStreamlines()
{
  int numStreamlines, index;
  vtkHyperStreamlinePoints *currStreamline;


  numStreamlines = this->Streamlines->GetNumberOfItems();
  index = 0;
  for (int i = 0; i < numStreamlines; i++)
    {
      vtkDebugMacro( << "Cleaning streamline " << i << " : " << index);

      // Get the streamline
      currStreamline = (vtkHyperStreamlinePoints *) 
        this->Streamlines->GetItemAsObject(index);

      if (currStreamline == NULL)
        {
          vtkErrorMacro( "No streamline " << index);
          return;
        }

      vtkDebugMacro( "streamline " << i << "length " << 
                     currStreamline->GetHyperStreamline0()->GetNumberOfPoints() +
                     currStreamline->GetHyperStreamline1()->GetNumberOfPoints());

      if (currStreamline->GetHyperStreamline0()->GetNumberOfPoints() +
          currStreamline->GetHyperStreamline1()->GetNumberOfPoints() < 5)
        {
          vtkErrorMacro( "Remove short streamline " << i << "length " << 
                         currStreamline->GetHyperStreamline0()->GetNumberOfPoints() +
                         currStreamline->GetHyperStreamline1()->GetNumberOfPoints());
          // Delete the streamline from the collections
          this->DeleteStreamline(index);
        }
      else
        {
          // Only increment if we haven't deleted one (and shortened the list)
          index++;
        }

    }

}



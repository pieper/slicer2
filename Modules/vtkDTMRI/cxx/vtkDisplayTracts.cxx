/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkDisplayTracts.cxx,v $
  Date:      $Date: 2006/02/09 00:38:34 $
  Version:   $Revision: 1.12 $

=========================================================================auto=*/
#include "vtkDisplayTracts.h"
#include "vtkPolyDataMapper.h"
#include "vtkRenderer.h"
#include "vtkAppendPolyData.h"
#include "vtkTubeFilter.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkClipPolyData.h"


//------------------------------------------------------------------------------
vtkDisplayTracts* vtkDisplayTracts::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkDisplayTracts");
  if(ret)
    {
      return (vtkDisplayTracts*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkDisplayTracts;
}

//----------------------------------------------------------------------------
vtkDisplayTracts::vtkDisplayTracts()
{
  // The user must set these for the class to function.
  this->Renderers = vtkCollection::New();
  

  // Initialize transforms to identity, 
  // so if the user doesn't set them it's okay.
  this->WorldToTensorScaledIJK = vtkTransform::New();

  // input collections
  this->Streamlines = NULL;

  // internal/output collections
  this->ClippedStreamlines = vtkCollection::New();
  this->Mappers = vtkCollection::New();
  this->TransformFilters = vtkCollection::New();
  this->TubeFilters = vtkCollection::New();
  this->Actors = vtkCollection::New();

  // Streamline parameters for all streamlines
  this->ScalarVisibility=0;
  this->Clipping=0;
  this->ClipFunction = NULL;

  // for tube filter
  this->TubeRadius = 0.5;
  this->TubeNumberOfSides = 4;
 
  // user-accessible property and lookup table for all streamlines
  this->StreamlineProperty = vtkProperty::New();
  this->StreamlineLookupTable = vtkLookupTable::New();
  // default: make 0 dark blue, not red
  this->StreamlineLookupTable->SetHueRange(.6667, 0.0);

  // the number of actors displayed in the scene
  this->NumberOfVisibleActors=0;

}

//----------------------------------------------------------------------------
vtkDisplayTracts::~vtkDisplayTracts()
{
  this->DeleteAllStreamlines();

  this->Renderers->Delete();
  if (this->Streamlines != NULL)
    this->Streamlines->Delete();
  this->ClippedStreamlines->Delete();
  this->Mappers->Delete();
  this->Actors->Delete();
  this->TubeFilters->Delete();
  this->TransformFilters->Delete();

  this->StreamlineLookupTable->Delete();
  this->StreamlineProperty->Delete();
}

//----------------------------------------------------------------------------
void vtkDisplayTracts::SetScalarVisibility(int value)
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


//----------------------------------------------------------------------------
void vtkDisplayTracts::SetClipping(int value)
{
  vtkHyperStreamline *currStreamline;
  vtkTubeFilter *currTubeFilter;
#if (VTK_MAJOR_VERSION >= 5)
   vtkPolyDataAlgorithm *clippedStreamline;
#else
  vtkPolyDataSource *clippedStreamline;
#endif
  
  // test if we are changing the value before looping through all streamlines
  if (this->Clipping != value)
    {

      if (value)
        {
          if (!this->ClipFunction)
            {
              vtkErrorMacro("Set the ClipFunction before turning clipping on");
              return;
            }
        }

      this->Clipping = value;


      // clear storage for these streamlines
      this->ClippedStreamlines->RemoveAllItems();      
      
          
      // apply this to ALL streamlines
      // traverse streamline collection and clip each one
      this->Streamlines->InitTraversal();
      this->TubeFilters->InitTraversal();
      
      currStreamline = (vtkHyperStreamline *)
        this->Streamlines->GetNextItemAsObject();
      currTubeFilter = (vtkTubeFilter *)
        this->TubeFilters->GetNextItemAsObject();
      
      while (currStreamline && currTubeFilter)
        {
          // clip or not, depending on this->Clipping
          clippedStreamline = this->ClipStreamline(currStreamline);

          // Make sure we are displaying clipped streamline
          // this corresponds to contents of ClippedStreamlines collection
          currTubeFilter->SetInput(clippedStreamline->GetOutput());

          currTubeFilter = (vtkTubeFilter *)
            this->TubeFilters->GetNextItemAsObject();
          currStreamline = (vtkHyperStreamline *)
            this->Streamlines->GetNextItemAsObject();
        }
    }
}


// Handle clipping/not clipping a single streamline
//----------------------------------------------------------------------------
//BTX
#if (VTK_MAJOR_VERSION >= 5)
vtkPolyDataAlgorithm *
#else
vtkPolyDataSource *
#endif
vtkDisplayTracts::ClipStreamline(vtkHyperStreamline *currStreamline)
//ETX
{

  if (this->Clipping) 
    {
      
      // Turn clipping on
      // Put a clipped streamline onto the collection
      vtkClipPolyData *currClipper = vtkClipPolyData::New();
      currClipper->SetInput(currStreamline->GetOutput());
      currClipper->SetClipFunction(this->ClipFunction);
      //currClipper->SetValue(0.0);
      currClipper->Update();
      
      this->ClippedStreamlines->AddItem((vtkObject *) currClipper);
      
      // The object survives as long as it is on the
      // collection. (until clipping is turned off)
      currClipper->Delete();

      return currClipper;
    }
  else
    {
      // Turn clipping off
      // Put the original streamline onto the collection
      this->ClippedStreamlines->AddItem((vtkObject *) currStreamline);
      return currStreamline;
    }

}


// Set the properties of one streamline's graphics objects as requested
// by the user
//----------------------------------------------------------------------------
void vtkDisplayTracts::ApplyUserSettingsToGraphicsObject(int index)
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

  // Set the tube width and number of sides as desired by the user
  currTubeFilter->SetRadius(this->TubeRadius);

  currTubeFilter->SetNumberOfSides(this->TubeNumberOfSides);

}

void vtkDisplayTracts::UpdateAllTubeFiltersWithCurrentSettings()
{
  vtkTubeFilter *currTubeFilter;

  this->TubeFilters->InitTraversal();
  currTubeFilter= (vtkTubeFilter *)this->TubeFilters->GetNextItemAsObject();
  while(currTubeFilter)
    {
      vtkDebugMacro( << "Updating tube filter " << currTubeFilter);

      // Set the tube width and number of sides as desired by the user
      currTubeFilter->SetRadius(this->TubeRadius);     
      currTubeFilter->SetNumberOfSides(this->TubeNumberOfSides);
     
      currTubeFilter= (vtkTubeFilter *)this->TubeFilters->GetNextItemAsObject();
    }


}

// Make actors, mappers, and lookup tables as needed for streamlines
// in the collection.
//----------------------------------------------------------------------------
void vtkDisplayTracts::CreateGraphicsObjects()
{
  int numStreamlines, numActorsCreated;
#if (VTK_MAJOR_VERSION >= 5)
  vtkPolyDataAlgorithm *currStreamline;
#else
  vtkPolyDataSource *currStreamline;
#endif
  vtkPolyDataMapper *currMapper;
  vtkActor *currActor;
  vtkTransform *currTransform;
  vtkRenderer *currRenderer;
  vtkTransformPolyDataFilter *currTransFilter;
  vtkTubeFilter *currTubeFilter;

  // Find out how many streamlines we have, and if they all have actors
  numStreamlines = this->Streamlines->GetNumberOfItems();
  numActorsCreated = this->Actors->GetNumberOfItems();

  vtkDebugMacro(<< "in CreateGraphicsObjects " << numActorsCreated << "  " << numStreamlines);

  // If we have already made all of the objects needed, stop here.
  if (numActorsCreated == numStreamlines) 
    return;

  // Create transformation matrix to place actors in scene
  currTransform=vtkTransform::New();
  currTransform->SetMatrix(this->WorldToTensorScaledIJK->GetMatrix());
  currTransform->Inverse();

  // Make actors and etc. for all streamlines that need them
  while (numActorsCreated < numStreamlines) 
    {
      // Handle clipping/not clipping
      // Now use the clipped/not clipped streamline for the rest
      currStreamline = 
        this->ClipStreamline((vtkHyperStreamline *)
                             this->Streamlines->GetItemAsObject(numActorsCreated));

      // Now create the objects needed
      currActor = vtkActor::New();
      this->Actors->AddItem((vtkObject *)currActor);
      currMapper = vtkPolyDataMapper::New();
      this->Mappers->AddItem((vtkObject *)currMapper);
          currTransFilter = vtkTransformPolyDataFilter::New();
          this->TransformFilters->AddItem((vtkObject *) currTransFilter);
      currTubeFilter = vtkTubeFilter::New();
      this->TubeFilters->AddItem((vtkObject *)currTubeFilter);

      // Apply user's visualization settings to these objects
      this->ApplyUserSettingsToGraphicsObject(numActorsCreated);

      // Hook up the pipeline
          currTransFilter->SetInput(currStreamline->GetOutput());
      currTubeFilter->SetInput(currTransFilter->GetOutput());
      currMapper->SetInput(currTubeFilter->GetOutput());
      currMapper->SetLookupTable(this->StreamlineLookupTable);
      currMapper->UseLookupTableScalarRangeOn();
      currActor->SetMapper(currMapper);
      
      // Place the actor correctly in the scene
      //currActor->SetUserMatrix(currTransform->GetMatrix());
      currTransFilter->SetTransform(currTransform);
          
      // add to the scene (to each renderer)
      // This is the same as MainAddActor in Main.tcl.
      this->Renderers->InitTraversal();
      currRenderer= (vtkRenderer *)this->Renderers->GetNextItemAsObject();
      while(currRenderer)
        {
          currRenderer->AddActor(currActor);
          currRenderer= (vtkRenderer *)this->Renderers->GetNextItemAsObject();
        }
      
      // Increment the count of actors we have created
      numActorsCreated++;
    }


  currTransform->Delete();

  // For debugging print this info again
  // Find out how many streamlines we have, and if they all have actors
  numStreamlines = this->Streamlines->GetNumberOfItems();
  numActorsCreated = this->Actors->GetNumberOfItems();

  vtkDebugMacro(<< "in CreateGraphicsObjects " << numActorsCreated << "  " << numStreamlines);

}

//----------------------------------------------------------------------------
void vtkDisplayTracts::AddStreamlinesToScene()
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
void vtkDisplayTracts::RemoveStreamlinesFromScene()
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
void vtkDisplayTracts::DeleteAllStreamlines()
{
  int numStreamlines, i;

  i=0;
  numStreamlines = this->ClippedStreamlines->GetNumberOfItems();
  while (i < numStreamlines)
    {
      vtkDebugMacro( << "Deleting streamline " << i);
      // always delete the first streamline from the collections
      // (they change size as we do this, shrinking away)
      this->DeleteStreamline(0);
      i++;
    }
  
}

// Delete all of the DISPLAY objects created for one streamline 
//----------------------------------------------------------------------------
void vtkDisplayTracts::DeleteStreamline(int index)
{
  vtkRenderer *currRenderer;
  //vtkLookupTable *currLookupTable;
  vtkPolyDataMapper *currMapper;
  vtkTransformPolyDataFilter *currTransFilter;
  vtkTubeFilter *currTubeFilter;
  vtkActor *currActor;

  vtkDebugMacro( << "Deleting actor " << index);
  currActor = (vtkActor *) this->Actors->GetItemAsObject(index);
  if (currActor != NULL)
    {
      if (currActor->GetVisibility()) {
          currActor->VisibilityOff();
          this->NumberOfVisibleActors--;
      }
      // Remove from the scene (from each renderer)
      // Just like MainRemoveActor in Main.tcl.
      // Don't delete the renderers since they are input.
      this->Renderers->InitTraversal();
      currRenderer= (vtkRenderer *)this->Renderers->GetNextItemAsObject();
      while(currRenderer)
        {
          vtkDebugMacro( << "rm actor from renderer " << currRenderer);
          currRenderer->RemoveActor(currActor);
          currRenderer= (vtkRenderer *)this->Renderers->GetNextItemAsObject();
        }

      // Delete the actors, this class created them
      this->Actors->RemoveItem(index);
      currActor->Delete();
    }

  vtkDebugMacro( << "Delete stream" );
  // Remove from collection.
  // If we are clipping this should delete the clipper object.
  // Otherwise it removes a reference to the streamline object still 
  // on the Streamlines collection.
  this->ClippedStreamlines->RemoveItem(index);

  vtkDebugMacro( << "Delete mapper" );
  currMapper = (vtkPolyDataMapper *) this->Mappers->GetItemAsObject(index);
  if (currMapper != NULL)
    {
      this->Mappers->RemoveItem(index);
      currMapper->Delete();
    }

  vtkDebugMacro( << "Delete transformFilter" );
  currTransFilter = (vtkTransformPolyDataFilter *) this->TransformFilters->GetItemAsObject(index);
  if (currTransFilter != NULL)
    {
      this->TransformFilters->RemoveItem(index);
      currTransFilter->Delete();
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
void vtkDisplayTracts::DeleteStreamline(vtkActor *pickedActor)
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
int vtkDisplayTracts::GetStreamlineIndexFromActor(vtkActor *pickedActor)
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





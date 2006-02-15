/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMultipleStreamlineController.cxx,v $
  Date:      $Date: 2006/02/15 19:47:39 $
  Version:   $Revision: 1.65.2.2 $

=========================================================================auto=*/
#include "vtkMultipleStreamlineController.h"
#include "vtkLookupTable.h"
#include "vtkRenderer.h"
#include "vtkFloatArray.h"
#include "vtkPointData.h"

#include "vtkHyperStreamline.h"

#include <sstream>

#include "vtkPolyDataMapper.h"

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

  // Helper classes
  // ---------------

  // for tract clustering
  this->TractClusterer = vtkClusterTracts::New();

  // for tract saving
  this->SaveTracts = vtkSaveTracts::New();

  // for tract display
  this->DisplayTracts = vtkDisplayTracts::New();

  // for creating tracts
  this->SeedTracts = vtkSeedTracts::New();

  // for coloring an ROI based on tract color
  this->ColorROIFromTracts = vtkColorROIFromTracts::New();

  // Helper class pipelines
  // ----------------------

  this->SeedTracts->SetStreamlines(this->Streamlines);

  this->DisplayTracts->SetStreamlines(this->Streamlines);

  this->SaveTracts->SetStreamlines(this->DisplayTracts->GetClippedStreamlines());
  this->SaveTracts->SetTubeFilters(this->DisplayTracts->GetTubeFilters());
  this->SaveTracts->SetActors(this->DisplayTracts->GetActors());

  this->ColorROIFromTracts->SetStreamlines(this->DisplayTracts->GetClippedStreamlines());
  this->ColorROIFromTracts->SetActors(this->DisplayTracts->GetActors());
}

//----------------------------------------------------------------------------
vtkMultipleStreamlineController::~vtkMultipleStreamlineController()
{
  this->DeleteAllStreamlines();

  this->Streamlines->Delete();

  this->WorldToTensorScaledIJK->Delete();
  if (this->InputTensorField) this->InputTensorField->Delete();

  this->InputRenderers->Delete();

  // delete helper classes
  this->TractClusterer->Delete();
  this->SaveTracts->Delete();  
  this->SeedTracts->Delete();  
  this->DisplayTracts->Delete();  
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
void vtkMultipleStreamlineController::SetInputRenderers(vtkCollection *renderers)
{
  
  vtkDebugMacro("Setting input renderers.");

  // Decrease reference count of old object
  if (this->InputRenderers != 0)
    this->InputRenderers->UnRegister(this);

  // Set new value in this class
  this->InputRenderers = renderers;

  // Increase reference count of new object
  if (this->InputRenderers != 0)
    this->InputRenderers->Register(this);

  // This class has changed
  this->Modified();

  // helper class pipelines
  // ----------------------
  this->DisplayTracts->SetRenderers(this->InputRenderers);

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
  this->DisplayTracts->SetWorldToTensorScaledIJK(this->WorldToTensorScaledIJK);
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
// Here we delete the actual vtkHyperStreamline subclass object.
// We call helper class DeleteStreamline functions in order
// to get rid of (for example) graphics display objects.
//----------------------------------------------------------------------------
void vtkMultipleStreamlineController::DeleteStreamline(int index)
{
  vtkHyperStreamline *currStreamline;

  // Helper class
  // Delete display (actor, mapper)
  vtkDebugMacro( << "Calling DisplayTracts DeleteStreamline" );
  this->DisplayTracts->DeleteStreamline(index);


  // Delete actual streamline
  vtkDebugMacro( << "Delete stream" );
  currStreamline = (vtkHyperStreamline *)
    this->Streamlines->GetItemAsObject(index);
  if (currStreamline != NULL)
    {
      this->Streamlines->RemoveItem(index);
      currStreamline->Delete();
    }

  vtkDebugMacro( << "Done deleting streamline");

}

// This is the delete called from the user interface where the
// actor has been picked with the mouse.
//----------------------------------------------------------------------------
void vtkMultipleStreamlineController::DeleteStreamline(vtkActor *pickedActor)
{
  int index;

  index = this->DisplayTracts->GetStreamlineIndexFromActor(pickedActor);

  if (index >=0)
    {
      this->DeleteStreamline(index);
    }
}

// Call the tract clustering object, and then color our hyperstreamlines
// according to their cluster numbers.
//----------------------------------------------------------------------------
void vtkMultipleStreamlineController::ClusterTracts(int tmp)
{
  //vtkCollection *streamlines = this->Streamlines;
  vtkCollection *streamlines = this->DisplayTracts->GetClippedStreamlines();

  if (streamlines == 0)
    {
      vtkErrorMacro("Streamlines are NULL.");
      return;      
    }

  if (streamlines->GetNumberOfItems() < 1)
    {
      vtkErrorMacro("No streamlines exist.");
      return;      
    }

  // First make sure none of the streamlines have 0 length
  this->CleanStreamlines(streamlines);

  int numberOfClusters= this->TractClusterer->GetNumberOfClusters();

  this->TractClusterer->SetInputStreamlines(streamlines);
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
      
      currActor = (vtkActor *) this->DisplayTracts->GetActors()->GetItemAsObject(idx);
      currMapper = (vtkPolyDataMapper *) this->DisplayTracts->GetMappers()->GetItemAsObject(idx);

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
void vtkMultipleStreamlineController::CleanStreamlines(vtkCollection *streamlines)
{
  int numStreamlines, index;
  vtkPolyDataSource *currStreamline;



  numStreamlines = streamlines->GetNumberOfItems();
  index = 0;
  for (int i = 0; i < numStreamlines; i++)
    {
      vtkDebugMacro( << "Cleaning streamline " << i << " : " << index);

      // Get the streamline
      currStreamline = (vtkPolyDataSource *) 
        streamlines->GetItemAsObject(index);

      if (currStreamline == NULL)
        {
          vtkErrorMacro( "No streamline " << index);
          return;
        }

      vtkDebugMacro( "streamline " << i << "length " << 
                     currStreamline->GetOutput()->GetNumberOfPoints());

      if (currStreamline->GetOutput()->GetNumberOfPoints() < 5)
        {
          vtkErrorMacro( "Remove short streamline " << i << "length " << 
                         currStreamline->GetOutput()->GetNumberOfPoints());
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



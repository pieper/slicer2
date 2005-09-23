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
#include "vtkCellArray.h"
#include "vtkStreamlineConvolve.h"
#include "vtkPruneStreamline.h"
#include "vtkTubeFilter.h"
#include "vtkPointData.h"

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
  this->ROIToWorld = vtkTransform::New();
  this->ROI2ToWorld = vtkTransform::New();
  this->WorldToTensorScaledIJK = vtkTransform::New();

  // The user must set these for the class to function.
  this->InputTensorField = NULL;
  this->InputRenderers = vtkCollection::New();
  
  // The user may need to set these, depending on class usage
  this->InputROI = NULL;
  this->InputROIValue = -1;
  this->InputMultipleROIValues = NULL;
  this->InputROI2 = NULL;
  this->InputROIForColoring = NULL;
  this->OutputROIForColoring = NULL;

  // collections
  this->Streamlines = vtkCollection::New();
  this->Mappers = vtkCollection::New();
  this->TubeFilters = vtkCollection::New();
  this->Actors = vtkCollection::New();

  // Streamline parameters for all streamlines
  this->IntegrationDirection = VTK_INTEGRATE_BOTH_DIRECTIONS;
  this->ScalarVisibility=0;

  // user-accessible property and lookup table for all streamlines
  this->StreamlineProperty = vtkProperty::New();
  this->StreamlineLookupTable = vtkLookupTable::New();
  // default: make 0 dark blue, not red
  this->StreamlineLookupTable->SetHueRange(.6667, 0.0);

  // if the user doesn't set these they will be ignored
  this->VtkHyperStreamlineSettings=NULL;
  this->VtkHyperStreamlinePointsSettings=NULL;
  this->VtkPreciseHyperStreamlinePointsSettings=NULL;

  // default to vtkHyperStreamline class creation
  this->UseVtkHyperStreamline();

  // the number of actors displayed in the scene
  this->NumberOfVisibleActors=0;

  // for fibers selecting fibers that pass through a ROI
  this->StreamlinesAsPolyLines = vtkPolyData::New();
  this->StreamlineIdPassTest = vtkIntArray::New();

 
  // Helper classes
  // ---------------

  // for tract clustering
  this->TractClusterer = vtkClusterTracts::New();

  // for tract saving
  this->SaveTracts = vtkSaveTracts::New();

  // Helper class pipelines
  // ----------------------
  this->SaveTracts->SetStreamlines(this->Streamlines);
  this->SaveTracts->SetTubeFilters(this->TubeFilters);
  this->SaveTracts->SetActors(this->Actors);

}

//----------------------------------------------------------------------------
vtkMultipleStreamlineController::~vtkMultipleStreamlineController()
{
  this->DeleteAllStreamlines();

  this->ROIToWorld->Delete();
  this->WorldToTensorScaledIJK->Delete();
  if (this->InputTensorField) this->InputTensorField->Delete();
  if (this ->InputROI) this->InputROI->Delete();
  this->InputRenderers->Delete();
  this->Streamlines->Delete();
  this->Mappers->Delete();
  this->Actors->Delete();
  this->TubeFilters->Delete();
  this->StreamlinesAsPolyLines->Delete();
  this->StreamlineLookupTable->Delete();
  this->StreamlineIdPassTest->Delete();

  // delete helper classes
  this->TractClusterer->Delete();
  this->SaveTracts->Delete();  
  
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
  // TEST this should be specified separately from hyperstreamline settings
  currTubeFilter->SetRadius(this->VtkHyperStreamlinePointsSettings->GetRadius());

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


// Test whether the given point is in bounds (inside the input data)
//----------------------------------------------------------------------------
int vtkMultipleStreamlineController::PointWithinTensorData(double *point, double *pointw)
{
  vtkFloatingPointType *bounds;
  int inbounds;

  bounds=this->InputTensorField->GetBounds();
  vtkDebugMacro("Bounds " << bounds[0] << " " << bounds[1] << " " << bounds[2] << " " << bounds[3] << " " << bounds[4] << " " << bounds[5]);
  
  inbounds=1;
  if (point[0] < bounds[0]) inbounds = 0;
  if (point[0] > bounds[1]) inbounds = 0;
  if (point[1] < bounds[2]) inbounds = 0;
  if (point[1] > bounds[3]) inbounds = 0;
  if (point[2] < bounds[4]) inbounds = 0;
  if (point[2] > bounds[5]) inbounds = 0;

  if (inbounds ==0)
    {
      cout << "point " << pointw[0] << " " << pointw[1] << " " << pointw[2] << " outside of tensor dataset" << endl;
    }

  return(inbounds);
}

// Here we create the type of streamline class requested by the user.
// Elsewhere in this class, all are treated as vtkHyperStreamline *.
// We copy settings from the example object that the user has access
// to.
// (It would be nicer if we required the hyperstreamline classes to 
// implement a copy function.)
//----------------------------------------------------------------------------
vtkHyperStreamline * vtkMultipleStreamlineController::CreateHyperStreamline()
{
  vtkHyperStreamline *currHS;
  vtkHyperStreamlinePoints *currHSP;
  vtkPreciseHyperStreamlinePoints *currPHSP;

  vtkDebugMacro(<< "in create HyperStreamline, type " << this->TypeOfHyperStreamline);

  switch (this->TypeOfHyperStreamline)
    {
    case USE_VTK_HYPERSTREAMLINE:
      if (this->VtkHyperStreamlineSettings) 
        {
          currHS=vtkHyperStreamline::New();
          return(currHS);
        }
      else
        {
          return(vtkHyperStreamline::New());
        }
      break;
    case USE_VTK_HYPERSTREAMLINE_POINTS:
      if (this->VtkHyperStreamlinePointsSettings) 
        {
          // create object
          currHSP=vtkHyperStreamlinePoints::New();

          // Now copy user's settings into this object:
          // MaximumPropagationDistance 
          currHSP->SetMaximumPropagationDistance(this->VtkHyperStreamlinePointsSettings->GetMaximumPropagationDistance());
          // IntegrationStepLength
          currHSP->SetIntegrationStepLength(this->VtkHyperStreamlinePointsSettings->GetIntegrationStepLength());
          // StepLength
          currHSP->SetStepLength(this->VtkHyperStreamlinePointsSettings->GetStepLength());
          // Radius
          currHSP->SetRadius(this->VtkHyperStreamlinePointsSettings->GetRadius());
          // NumberOfSides
          currHSP->SetNumberOfSides(this->VtkHyperStreamlinePointsSettings->GetNumberOfSides());
          // MaxCurvature
          currHSP->SetMaxCurvature(this->VtkHyperStreamlinePointsSettings->GetMaxCurvature());

          // MinFractionalAnisotropy
          currHSP->SetMinFractionalAnisotropy(this->VtkHyperStreamlinePointsSettings->GetMinFractionalAnisotropy());

          // Eigenvector to integrate
          currHSP->SetIntegrationEigenvector(this->VtkHyperStreamlinePointsSettings->GetIntegrationEigenvector());

          // IntegrationDirection (set in this class, default both ways)
          currHSP->SetIntegrationDirection(this->IntegrationDirection);

          return((vtkHyperStreamline *)currHSP);
        }
      else
        {
          return((vtkHyperStreamline *) vtkHyperStreamlinePoints::New());

        }


      break;
    case USE_VTK_PRECISE_HYPERSTREAMLINE_POINTS:
      if (this->VtkPreciseHyperStreamlinePointsSettings) 
        {

          // create object
          currPHSP=vtkPreciseHyperStreamlinePoints::New();

          // Now copy user's settings into this object:
          // Method
          currPHSP->
            SetMethod(this->VtkPreciseHyperStreamlinePointsSettings->
                      GetMethod());
          // Terminal FA
          currPHSP->
            SetTerminalFractionalAnisotropy(this->VtkPreciseHyperStreamlinePointsSettings->GetTerminalFractionalAnisotropy());
          // MaximumPropagationDistance 
          currPHSP->
            SetMaximumPropagationDistance(this->VtkPreciseHyperStreamlinePointsSettings->GetMaximumPropagationDistance());
          // MinimumPropagationDistance 
          currPHSP->
            SetMinimumPropagationDistance(this->VtkPreciseHyperStreamlinePointsSettings->GetMinimumPropagationDistance());
          // TerminalEigenvalue
          currPHSP->
            SetTerminalEigenvalue(this->VtkPreciseHyperStreamlinePointsSettings->GetTerminalEigenvalue());
          // IntegrationStepLength
          currPHSP->
            SetIntegrationStepLength(this->VtkPreciseHyperStreamlinePointsSettings->GetIntegrationStepLength());
          // StepLength 
          currPHSP->
            SetStepLength(this->VtkPreciseHyperStreamlinePointsSettings->GetStepLength());
          // Radius  
          currPHSP->
            SetRadius(this->VtkPreciseHyperStreamlinePointsSettings->GetRadius());
          // NumberOfSides
          currPHSP->
            SetNumberOfSides(this->VtkPreciseHyperStreamlinePointsSettings->GetNumberOfSides());

          // Eigenvector to integrate
          currPHSP->SetIntegrationEigenvector(this->VtkPreciseHyperStreamlinePointsSettings->GetIntegrationEigenvector());

          // IntegrationDirection (set in this class, default both ways)
          currPHSP->SetIntegrationDirection(this->IntegrationDirection);

          // MaxStep
          currPHSP->
            SetMaxStep(this->VtkPreciseHyperStreamlinePointsSettings->GetMaxStep());
          // MinStep
          currPHSP->
            SetMinStep(this->VtkPreciseHyperStreamlinePointsSettings->GetMinStep());
          // MaxError
          currPHSP->
            SetMaxError(this->VtkPreciseHyperStreamlinePointsSettings->GetMaxError());
          // MaxAngle
          currPHSP->
            SetMaxAngle(this->VtkPreciseHyperStreamlinePointsSettings->GetMaxAngle());
          // LengthOfMaxAngle
          currPHSP->
            SetLengthOfMaxAngle(this->VtkPreciseHyperStreamlinePointsSettings->GetLengthOfMaxAngle());

          return((vtkHyperStreamline *) currPHSP);
          // 
        }
      else
        {
          return((vtkHyperStreamline *) vtkPreciseHyperStreamlinePoints::New());
        }
      break;
    }
  return (NULL);
}

//----------------------------------------------------------------------------
void vtkMultipleStreamlineController::SeedStreamlineFromPoint(double x, 
                                                              double y, 
                                                              double z)

{
  double pointw[3], point[3];
  vtkHyperStreamline *newStreamline;

  // test we have input
  if (this->InputTensorField == NULL)
    {
      vtkErrorMacro("No tensor data input.");
      return;      
    }

  pointw[0]=x;
  pointw[1]=y;
  pointw[2]=z;

  vtkDebugMacro("Starting streamline from point " << pointw[0] << " " << pointw[1] << " " << pointw[2]);

  // Transform from world coords to scaled ijk of the input tensors
  this->WorldToTensorScaledIJK->TransformPoint(pointw,point);

  vtkDebugMacro("Starting streamline from point " << point[0] << " " << point[1] << " " << point[2]);

  // make sure it is within the bounds of the tensor dataset
  if (!this->PointWithinTensorData(point,pointw))
    {
      vtkErrorMacro("Point " << x << ", " << y << ", " << z << " outside of tensor dataset.");
      return;
    }

  // Now create a streamline and put it on the collection.
  newStreamline=this->CreateHyperStreamline();
  this->Streamlines->AddItem((vtkObject *)newStreamline);
  
  // Set its input information.
  newStreamline->SetInput(this->InputTensorField);
  newStreamline->SetStartPosition(point[0],point[1],point[2]);
  
}

void vtkMultipleStreamlineController::SeedStreamlinesFromROIWithMultipleValues()
{

  int numROIs;
  int initialROIValue = this->InputROIValue;
  
  if (this->InputMultipleROIValues == NULL)
    {
      vtkErrorMacro(<<"No values to seed from. SetInputMultipleROIValues before trying.");
      return;
    }  
  
  numROIs=this->InputMultipleROIValues->GetNumberOfTuples();
  
  // test we have input
  if (this->InputROI == NULL)
    {
      vtkErrorMacro("No ROI input.");
      return;      
    }
  if (this->InputTensorField == NULL)
    {
      vtkErrorMacro("No tensor data input.");
      return;      
    }
    
  for (int i=0 ; i<numROIs ; i++)
    {
      this->InputROIValue = this->InputMultipleROIValues->GetValue(i);
      // check ROI's value of interest
      if (this->InputROIValue <= 0)
        {
          vtkErrorMacro("Input ROI value has not been set or is 0. (value is "  << this->InputROIValue << ". Trying next value");
          break;      
        }
      this->SeedStreamlinesFromROI();
    }
    
  //Restore InputROIValue variable
  this->InputROIValue = initialROIValue;   
}       
      

//----------------------------------------------------------------------------
void vtkMultipleStreamlineController::SeedStreamlinesFromROI()
{
  int idxX, idxY, idxZ;
  int maxX, maxY, maxZ;
  int inIncX, inIncY, inIncZ;
  int inExt[6];
  double point[3], point2[3];
  unsigned long count = 0;
  //unsigned long target;
  short *inPtr;
  vtkHyperStreamline *newStreamline;

  // test we have input
  if (this->InputROI == NULL)
    {
      vtkErrorMacro("No ROI input.");
      return;      
    }
  if (this->InputTensorField == NULL)
    {
      vtkErrorMacro("No tensor data input.");
      return;      
    }
  // check ROI's value of interest
  if (this->InputROIValue <= 0)
    {
      vtkErrorMacro("Input ROI value has not been set or is 0. (value is "  << this->InputROIValue << ".");
      return;      
    }
  // make sure it is short type
  if (this->InputROI->GetScalarType() != VTK_SHORT)
    {
      vtkErrorMacro("Input ROI is not of type VTK_SHORT");
      return;      
    }

  // currently this filter is not multithreaded, though in the future 
  // it could be (especially if it inherits from an image filter class)
  this->InputROI->GetWholeExtent(inExt);
  this->InputROI->GetContinuousIncrements(inExt, inIncX, inIncY, inIncZ);

  // find the region to loop over
  maxX = inExt[1] - inExt[0];
  maxY = inExt[3] - inExt[2]; 
  maxZ = inExt[5] - inExt[4];

  //cout << "Dims: " << maxX << " " << maxY << " " << maxZ << endl;
  //cout << "Incr: " << inIncX << " " << inIncY << " " << inIncZ << endl;

  // for progress notification
  //target = (unsigned long)((maxZ+1)*(maxY+1)/50.0);
  //target++;

  // start point in input integer field
  inPtr = (short *) this->InputROI->GetScalarPointerForExtent(inExt);

  for (idxZ = 0; idxZ <= maxZ; idxZ++)
    {
      //for (idxY = 0; !this->AbortExecute && idxY <= maxY; idxY++)
      for (idxY = 0; idxY <= maxY; idxY++)
        {
          //if (!(count%target)) 
          //{
          //this->UpdateProgress(count/(50.0*target) + (maxZ+1)*(maxY+1));
          //cout << (count/(50.0*target) + (maxZ+1)*(maxY+1)) << endl;
          //cout << "progress: " << count << endl;
          //}
          //count++;
          
          for (idxX = 0; idxX <= maxX; idxX++)
            {
              // If the point is equal to the ROI value then seed here.
              if (*inPtr == this->InputROIValue)
                {
                  vtkDebugMacro( << "start streamline at: " << idxX << " " <<
                                 idxY << " " << idxZ);

                  // First transform to world space.
                  point[0]=idxX;
                  point[1]=idxY;
                  point[2]=idxZ;
                  this->ROIToWorld->TransformPoint(point,point2);
                  // Now transform to scaled ijk of the input tensors
                  this->WorldToTensorScaledIJK->TransformPoint(point2,point);

                  // make sure it is within the bounds of the tensor dataset
                  if (this->PointWithinTensorData(point,point2))
                    {
                      // Now create a streamline and put it on the collection.
                      newStreamline=this->CreateHyperStreamline();
                      this->Streamlines->AddItem((vtkObject *)newStreamline);
                      
                      // Set its input information.
                      newStreamline->SetInput(this->InputTensorField);
                      newStreamline->SetStartPosition(point[0],point[1],point[2]);
                    }
                }
              inPtr++;
              inPtr += inIncX;
            }
          inPtr += inIncY;
        }
      inPtr += inIncZ;
    }
}

// seed in each voxel in the ROI unless already colored - skip unneeded ones
//----------------------------------------------------------------------------
void vtkMultipleStreamlineController::SeedStreamlinesEvenlyInROI()
{

  int idxX, idxY, idxZ;
  int maxX, maxY, maxZ;
  int inIncX, inIncY, inIncZ;
  int inExt[6];
  double point[3], point2[3];
  unsigned long count = 0;
  //unsigned long target;
  short *inPtr;
  vtkHyperStreamlinePoints *newStreamline;

  // test we have input
  if (this->InputROI == NULL)
    {
      vtkErrorMacro("No ROI input.");
      return;      
    }
  if (this->InputTensorField == NULL)
    {
      vtkErrorMacro("No tensor data input.");
      return;      
    }
  // check ROI's value of interest
  if (this->InputROIValue <= 0)
    {
      vtkErrorMacro("Input ROI value has not been set or is 0. (value is "  << this->InputROIValue << ".");
      return;      
    }
  // make sure it is short type
  if (this->InputROI->GetScalarType() != VTK_SHORT)
    {
      vtkErrorMacro("Input ROI is not of type VTK_SHORT");
      return;      
    }

  // Create a volume to "color in" and keep track of the passage
  // of streamlines.
  int dims[3];
  this->InputROI->GetDimensions(dims);
  vtkImageData *streamlineCount = vtkImageData::New();
  streamlineCount->SetDimensions(dims);
  streamlineCount->SetScalarTypeToFloat();
  streamlineCount->AllocateScalars();
  float *streamlineCountImage = (float *) streamlineCount->GetScalarPointer();

  for (int idx0 = 0; idx0 < dims[0]; idx0++)
    {
      for (int idx1 = 0; idx1 < dims[1]; idx1++)
        {
          for (int idx2 = 0; idx2 < dims[2]; idx2++)
            {
              
              // init the count to 0
              *streamlineCountImage = 0;
              streamlineCountImage++;
            }
        }
    }
  // reset pointer to start of image
  streamlineCountImage = (float *) streamlineCount->GetScalarPointer();

  // Create transformation matrices to go backwards from streamline points to ROI space
  vtkTransform *WorldToROI = vtkTransform::New();
  WorldToROI->SetMatrix(this->ROIToWorld->GetMatrix());
  WorldToROI->Inverse();
  vtkTransform *TensorScaledIJKToWorld = vtkTransform::New();
  TensorScaledIJKToWorld->SetMatrix(this->WorldToTensorScaledIJK->GetMatrix());
  TensorScaledIJKToWorld->Inverse();

  // currently this filter is not multithreaded, though in the future 
  // it could be (especially if it inherits from an image filter class)
  this->InputROI->GetWholeExtent(inExt);
  this->InputROI->GetContinuousIncrements(inExt, inIncX, inIncY, inIncZ);

  // find the region to loop over
  maxX = inExt[1] - inExt[0];
  maxY = inExt[3] - inExt[2]; 
  maxZ = inExt[5] - inExt[4];

  //cout << "Dims: " << maxX << " " << maxY << " " << maxZ << endl;
  //cout << "Incr: " << inIncX << " " << inIncY << " " << inIncZ << endl;

  // for progress notification
  //target = (unsigned long)((maxZ+1)*(maxY+1)/50.0);
  //target++;

  // start point in input integer field
  inPtr = (short *) this->InputROI->GetScalarPointerForExtent(inExt);
  int tmpCount=0;

  for (idxZ = 0; idxZ <= maxZ; idxZ++)
    {
      //for (idxY = 0; !this->AbortExecute && idxY <= maxY; idxY++)
      for (idxY = 0; idxY <= maxY; idxY++)
        {
          //if (!(count%target)) 
          //{
          //this->UpdateProgress(count/(50.0*target) + (maxZ+1)*(maxY+1));
          //cout << (count/(50.0*target) + (maxZ+1)*(maxY+1)) << endl;
          //cout << "progress: " << count << endl;
          //}
          //count++;
          
          for (idxX = 0; idxX <= maxX; idxX++)
            {
              // if it is in the ROI/mask
              if (*inPtr == this->InputROIValue)
                {

                  // if it is not colored in already
                  if (*streamlineCountImage < 1)
                    {
                  
                      // seed there and update
                      vtkDebugMacro( << "start streamline at: " << idxX << " " <<
                                     idxY << " " << idxZ);
                      
                      // First transform to world space.
                      point[0]=idxX;
                      point[1]=idxY;
                      point[2]=idxZ;
                      this->ROIToWorld->TransformPoint(point,point2);
                      // Now transform to scaled ijk of the input tensors
                      this->WorldToTensorScaledIJK->TransformPoint(point2,point);

                      // make sure it is within the bounds of the tensor dataset
                      if (this->PointWithinTensorData(point,point2))
                        {
                          // Now create a streamline and put it on the collection.
                          newStreamline=(vtkHyperStreamlinePoints *) this->CreateHyperStreamline();
                          this->Streamlines->AddItem((vtkObject *)newStreamline);
                          
                          // Set its input information.
                          newStreamline->SetInput(this->InputTensorField);
                          newStreamline->SetStartPosition(point[0],point[1],point[2]);

                          // Force it to update to access the path points
                          newStreamline->Update();

                          // for each point on the path, 
                          // color in the nearest voxel
                          vtkPoints *hs0=newStreamline->GetHyperStreamline0();
                          vtkPoints *hs1=newStreamline->GetHyperStreamline1();
                          int numPts=hs0->GetNumberOfPoints();
                          int ptidx=0;
                          int pt[3];
                          while (ptidx < numPts)
                            {
                              hs0->GetPoint(ptidx,point);
                              // First transform to world space.
                              TensorScaledIJKToWorld->TransformPoint(point,point2);
                              // Now transform to ROI IJK space
                              WorldToROI->TransformPoint(point2,point);
                              // Color in that voxel in the streamlineCountImage
                              pt[0]= (int) floor(point[0] + 0.5);
                              pt[1]= (int) floor(point[1] + 0.5);
                              pt[2]= (int) floor(point[2] + 0.5);
                              float *tmp = (float *)streamlineCount->GetScalarPointer(pt);
                              if (tmp != NULL)
                                {
                                  *(tmp) += 1;
                                  tmpCount++;
                                }
                              ptidx++;
                            }
                          numPts=hs1->GetNumberOfPoints();
                          // Skip the first point in the second line since it
                          // is a duplicate of the initial point.
                          ptidx=1;
                          while (ptidx < numPts)
                            {
                              hs1->GetPoint(ptidx,point);
                              // First transform to world space.
                              TensorScaledIJKToWorld->TransformPoint(point,point2);
                              // Now transform to ROI IJK space
                              WorldToROI->TransformPoint(point2,point);
                              // Color in that voxel in the streamlineCountImage
                              pt[0]= (int) floor(point[0] + 0.5);
                              pt[1]= (int) floor(point[1] + 0.5);
                              pt[2]= (int) floor(point[2] + 0.5);
                              float *tmp = (float *)streamlineCount->GetScalarPointer(pt);
                              if (tmp != NULL)
                                {
                                  *(tmp) += 1;
                                  tmpCount++;
                                }
                              ptidx++;
                            }                          

                        } // end if inside tensor field
                    } // end if count < max
                } // end if in ROI

              inPtr++;
              streamlineCountImage++;

              inPtr += inIncX;
              streamlineCountImage += inIncX;
            }
          inPtr += inIncY;
          streamlineCountImage += inIncY;
        }
      inPtr += inIncZ;
      streamlineCountImage += inIncZ;
    }

  vtkImageWriter *tmp = vtkImageWriter::New();
  tmp->SetInput(streamlineCount);
  tmp->SetFilePrefix("tractCountImage");
  tmp->SetFilePattern("%s.%03d");
  tmp->SetFileDimensionality(2);
  //tmp->Write();

  vtkErrorMacro("number ofnon-null pointers: " << tmpCount);

  // other ideas:
  // pick a random voxel rather than traverse ROI
  // repeat until all voxels in the ROI are colored in
  // in future, want subvoxel resolution for arbitrary density

}



// seed in each voxel in the ROI, only keep paths that intersect the
// second ROI
//----------------------------------------------------------------------------
void vtkMultipleStreamlineController::SeedStreamlinesFromROIIntersectWithROI2()
{

  int idxX, idxY, idxZ;
  int maxX, maxY, maxZ;
  int inIncX, inIncY, inIncZ;
  int inExt[6];
  double point[3], point2[3];
  unsigned long count = 0;
  //unsigned long target;
  short *inPtr;
  vtkHyperStreamlinePoints *newStreamline;

  // test we have input
  if (this->InputROI == NULL)
    {
      vtkErrorMacro("No ROI input.");
      return;      
    }
  if (this->InputTensorField == NULL)
    {
      vtkErrorMacro("No tensor data input.");
      return;      
    }
  if (this->InputROI2 == NULL)
    {
      vtkErrorMacro("No ROI input.");
      return;      
    }

  // make sure it is short type
  if (this->InputROI->GetScalarType() != VTK_SHORT)
    {
      vtkErrorMacro("Input ROI is not of type VTK_SHORT");
      return;      
    }
  // make sure it is short type
  if (this->InputROI2->GetScalarType() != VTK_SHORT)
    {
      vtkErrorMacro("Input ROI is not of type VTK_SHORT");
      return;      
    }

  // Create transformation matrices to go backwards from streamline points to ROI space
  // This is used to access ROI2.
  vtkTransform *WorldToROI2 = vtkTransform::New();
  WorldToROI2->SetMatrix(this->ROI2ToWorld->GetMatrix());
  WorldToROI2->Inverse();
  vtkTransform *TensorScaledIJKToWorld = vtkTransform::New();
  TensorScaledIJKToWorld->SetMatrix(this->WorldToTensorScaledIJK->GetMatrix());
  TensorScaledIJKToWorld->Inverse();

  // currently this filter is not multithreaded, though in the future 
  // it could be (especially if it inherits from an image filter class)
  this->InputROI->GetWholeExtent(inExt);
  this->InputROI->GetContinuousIncrements(inExt, inIncX, inIncY, inIncZ);

  // find the region to loop over
  maxX = inExt[1] - inExt[0];
  maxY = inExt[3] - inExt[2]; 
  maxZ = inExt[5] - inExt[4];

  //cout << "Dims: " << maxX << " " << maxY << " " << maxZ << endl;
  //cout << "Incr: " << inIncX << " " << inIncY << " " << inIncZ << endl;

  // for progress notification
  //target = (unsigned long)((maxZ+1)*(maxY+1)/50.0);
  //target++;

  // start point in input integer field
  inPtr = (short *) this->InputROI->GetScalarPointerForExtent(inExt);

  // testing for seeding at a certain resolution.
  int increment = 1;

  for (idxZ = 0; idxZ <= maxZ; idxZ++)
    {
      //for (idxY = 0; !this->AbortExecute && idxY <= maxY; idxY++)
      //for (idxY = 0; idxY <= maxY; idxY++)
      for (idxY = 0; idxY <= maxY; idxY += increment)
        {
          //if (!(count%target)) 
          //{
          //this->UpdateProgress(count/(50.0*target) + (maxZ+1)*(maxY+1));
          //cout << (count/(50.0*target) + (maxZ+1)*(maxY+1)) << endl;
          //cout << "progress: " << count << endl;
          //}
          //count++;
          
          //for (idxX = 0; idxX <= maxX; idxX++)
          for (idxX = 0; idxX <= maxX; idxX += increment)
            {
              // if it is in the ROI/mask
              if (*inPtr == this->InputROIValue)
                {

                  // seed there and update
                  vtkDebugMacro( << "start streamline at: " << idxX << " " <<
                                 idxY << " " << idxZ);
                      
                  // First transform to world space.
                  point[0]=idxX;
                  point[1]=idxY;
                  point[2]=idxZ;
                  this->ROIToWorld->TransformPoint(point,point2);
                  // Now transform to scaled ijk of the input tensors
                  this->WorldToTensorScaledIJK->TransformPoint(point2,point);

                  // make sure it is within the bounds of the tensor dataset
                  if (this->PointWithinTensorData(point,point2))
                    {
                      // Now create a streamline.
                      newStreamline=(vtkHyperStreamlinePoints *) this->CreateHyperStreamline();

                      // Set its input information.
                      newStreamline->SetInput(this->InputTensorField);
                      newStreamline->SetStartPosition(point[0],point[1],point[2]);
                      
                      // Force it to update to access the path points
                      newStreamline->Update();
                      
                      // for each point on the path, test
                      // the nearest voxel for path/ROI intersection.
                      vtkPoints *hs0=newStreamline->GetHyperStreamline0();
                      vtkPoints *hs1=newStreamline->GetHyperStreamline1();
                      int numPts=hs0->GetNumberOfPoints();
                      int ptidx=0;
                      int pt[3];
                      int intersects = 0;
                      while (ptidx < numPts)
                        {
                          hs0->GetPoint(ptidx,point);
                          // First transform to world space.
                          TensorScaledIJKToWorld->TransformPoint(point,point2);
                          // Now transform to ROI2 IJK space
                          WorldToROI2->TransformPoint(point2,point);
                          // Find that voxel number
                          pt[0]= (int) floor(point[0]+0.5);
                          pt[1]= (int) floor(point[1]+0.5);
                          pt[2]= (int) floor(point[2]+0.5);
                          short *tmp = (short *) this->InputROI2->GetScalarPointer(pt);
                          if (tmp != NULL)
                            {
                              if (*tmp == this->InputROI2Value) {
                                intersects = 1;
                              }
                            }
                          ptidx++;
                        }
                      numPts=hs1->GetNumberOfPoints();
                      // Skip the first point in the second line since it
                      // is a duplicate of the initial point.
                      ptidx=1;
                      while (ptidx < numPts)
                        {
                          hs1->GetPoint(ptidx,point);
                          // First transform to world space.
                          TensorScaledIJKToWorld->TransformPoint(point,point2);
                          // Now transform to ROI IJK space
                          WorldToROI2->TransformPoint(point2,point);
                          // Find that voxel number
                          pt[0]= (int) floor(point[0]+0.5);
                          pt[1]= (int) floor(point[1]+0.5);
                          pt[2]= (int) floor(point[2]+0.5);
                          short *tmp = (short *) this->InputROI2->GetScalarPointer(pt);
                          if (tmp != NULL)
                            {
                              if (*tmp > 0) {
                                intersects = 1;
                              }
                            }
                          ptidx++;
                        }                          

                      // if it intersects with some ROI, then 
                      // display it, otherwise delete it.
                      if (intersects) 
                        {
                          this->Streamlines->AddItem
                            ((vtkObject *)newStreamline);
                        }
                      else 
                        {
                          newStreamline->Delete();
                        }

                    } // end if inside tensor field

                } // end if in ROI

              //inPtr++;
              inPtr += increment;

              inPtr += inIncX;
            }
          //inPtr += inIncY;
          inPtr += inIncY*increment;
        }
      inPtr += inIncZ;
    }

}


void vtkMultipleStreamlineController::ConvertStreamlinesToPolyLines()
{

  int numStreamlines;
  vtkPoints *newPoints = vtkPoints::New();
  vtkCellArray *newLines = vtkCellArray::New(); 
  vtkPoints *strPoints;
  vtkHyperStreamlinePoints *currStreamline = NULL;
  int npts = 0;
  numStreamlines = this->GetNumberOfStreamlines();
  
  this->Streamlines->InitTraversal();
  for(int i=0 ; i<numStreamlines; i++)
    {
      currStreamline= dynamic_cast<vtkHyperStreamlinePoints *> (this->Streamlines->GetNextItemAsObject());
    
      strPoints = currStreamline->GetHyperStreamline0();
      npts += strPoints->GetNumberOfPoints();
      strPoints = currStreamline->GetHyperStreamline1();
      npts += strPoints->GetNumberOfPoints();
    }
  
  cout<<"Number of points of the streamlines: "<<npts<<endl;
  
  newPoints->SetNumberOfPoints(npts);
  
  int strIdx=0;
  this->Streamlines->InitTraversal();
  for(int i=0 ; i<numStreamlines; i++)
    {
      cout<<"Processing streamline: "<<i<<endl;
      currStreamline= dynamic_cast<vtkHyperStreamlinePoints *> (this->Streamlines->GetNextItemAsObject());
    
      strPoints = currStreamline->GetHyperStreamline0();
      newLines->InsertNextCell(strPoints->GetNumberOfPoints());
      for(int j=0; j<strPoints->GetNumberOfPoints();j++)
        {
          newPoints->SetPoint(strIdx,strPoints->GetPoint(j));
          newLines->InsertCellPoint(strIdx);
          strIdx++;
        }
      
      strPoints = currStreamline->GetHyperStreamline1();
      newLines->InsertNextCell(strPoints->GetNumberOfPoints());
      for(int j=0; j<strPoints->GetNumberOfPoints();j++)
        {
          newPoints->SetPoint(strIdx,strPoints->GetPoint(j));
          newLines->InsertCellPoint(strIdx);
          strIdx++;
        }
    }
  cout<<"Saving streamlines as poly lines"<<endl;
  this->StreamlinesAsPolyLines->SetPoints(newPoints);
  this->StreamlinesAsPolyLines->SetLines(newLines);
  newPoints->Delete();
  newLines->Delete();
}
    
void vtkMultipleStreamlineController::FindStreamlinesThatPassThroughROI()
{

  cout<<"Converting Streamlines to PolyLines"<<endl;
  this->ConvertStreamlinesToPolyLines();
 
  vtkStreamlineConvolve *conv = vtkStreamlineConvolve::New();
  vtkPruneStreamline *finder = vtkPruneStreamline::New();
 
  //Create minipipeline
  conv->SetStreamlines(this->StreamlinesAsPolyLines);
  conv->SetInput(this->InputROI);
  conv->SetKernel7x7x7(this->ConvolutionKernel);
  cout<<"Updtating vtkStreamlineConvolve"<<endl;
  conv->Update();
 
  finder->SetInput(conv->GetOutput());
  finder->SetROIValues(this->InputMultipleROIValues);
  finder->SetThreshold(1);
 
  //Update minipipeline
  cout<<"Updating vtkPruneStreamline"<<endl;
  finder->Update();
 
  //Get streamline info and set visibility off.
  this->StreamlineIdPassTest->DeepCopy(finder->GetStreamlineIdPassTest());
 
 
  //Delete minipipeline
  conv->Delete();
  finder->Delete();
 
}


void vtkMultipleStreamlineController::HighlightStreamlinesPassTest()
{
 
  vtkIdType strId;
  int numStr = this->StreamlineIdPassTest->GetNumberOfTuples();
  cout<<"Number of Streamlines that pass test: "<<numStr<<endl;
  vtkActor *currActor;
  int idx=0;
  for (int i=0;i<this->GetNumberOfStreamlines();i++) {
    strId = this->StreamlineIdPassTest->GetValue(idx);
    if(strId!=i) {
      //this->DeleteStreamline(i);
      //Changes Opacity
      currActor = (vtkActor *) this->Actors->GetItemAsObject(i);
      currActor->GetProperty()->SetOpacity(0.1);
    }
    else {
      currActor = (vtkActor *) this->Actors->GetItemAsObject(i);
      currActor->GetProperty()->SetColor(1,0,0);
      cout<<"Streamline Id: "<<strId<<endl;
      idx++;
    }  
     
  }
}

void vtkMultipleStreamlineController::DeleteStreamlinesNotPassTest()
{
 
  vtkIdType strId;
  int numStr = this->StreamlineIdPassTest->GetNumberOfTuples();
  cout<<"Number of Streamlines that pass test: "<<numStr<<endl;

  int idx=0;
  for (int i=0;i<this->GetNumberOfStreamlines();i++) {
    strId = this->StreamlineIdPassTest->GetValue(idx);
    if(strId!=i) {
      this->DeleteStreamline(i);
    }
    else {
      cout<<"Streamline Id: "<<strId<<endl;
      idx++;
    }  
     
  }
}
 

// Call the tract clustering object, and then color our hyperstreamlines
// according to their cluster numbers.
//----------------------------------------------------------------------------
void vtkMultipleStreamlineController::ClusterTracts(int tmp)
{
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



// Color in volume with color ID of streamline passing through it.
// Note: currently does not handle multiple streamlines per voxel
// (chooses last to pass through).
// Note: currently IDs are assigned in order of colors on streamline list.
// This should be changed to use internal color IDs when we have those.
//----------------------------------------------------------------------------
void vtkMultipleStreamlineController::ColorROIFromStreamlines()
{
  if (this->InputROIForColoring == NULL)
    {
      vtkErrorMacro("No ROI input.");
      return;      
    }
  
  // make sure it is short type
  if (this->InputROIForColoring->GetScalarType() != VTK_SHORT)
    {
      vtkErrorMacro("Input ROI is not of type VTK_SHORT");
      return;      
    }
  
  // prepare to traverse streamline collection
  this->Streamlines->InitTraversal();
  vtkHyperStreamlinePoints *currStreamline = 
    dynamic_cast<vtkHyperStreamlinePoints *> (this->Streamlines->GetNextItemAsObject());
  
  // test we have streamlines
  if (currStreamline == NULL)
    {
      vtkErrorMacro("No streamlines have been created yet.");
      return;      
    }
  
  this->Actors->InitTraversal();
  vtkActor *currActor= (vtkActor *)this->Actors->GetNextItemAsObject();
  
  // test we have actors and streamlines
  if (currActor == NULL)
    {
      vtkErrorMacro("No streamlines have been created yet.");
      return;      
    }
  
  // Create output
  if (this->OutputROIForColoring != NULL)
    this->OutputROIForColoring->Delete();
  this->OutputROIForColoring = vtkImageData::New();
  // Start with some defaults.
  this->OutputROIForColoring->CopyTypeSpecificInformation( this->InputROIForColoring );
  this->OutputROIForColoring->SetExtent(this->InputROIForColoring->GetWholeExtent());
  this->OutputROIForColoring->AllocateScalars();
  
  // initialize to all 0's
  int dims[3];
  this->OutputROIForColoring->GetDimensions(dims);
  int size = dims[0]*dims[1]*dims[2];
  short *outPtr = (short *) this->OutputROIForColoring->GetScalarPointer();
  for(int i=0; i<size; i++)
    {
      *outPtr = (short) 0;
      outPtr++;
    }


  // Create transformation matrices to go backwards from streamline points to ROI space
  // This is used to access ROIForColoring, it has to have same 
  // dimensions and location as seeding ROI for now.
  vtkTransform *WorldToROI = vtkTransform::New();
  WorldToROI->SetMatrix(this->ROIToWorld->GetMatrix());
  WorldToROI->Inverse();
  vtkTransform *TensorScaledIJKToWorld = vtkTransform::New();
  TensorScaledIJKToWorld->SetMatrix(this->WorldToTensorScaledIJK->GetMatrix());
  TensorScaledIJKToWorld->Inverse();
  
  // init color IDs with the first streamline.
  double rgb[3];
  currActor->GetProperty()->GetColor(rgb);
  double R[1000], G[1000], B[1000];
  int arraySize=1000;
  int lastColor = 0;
  int currColor, newColor;
  R[0]=rgb[0];
  G[0]=rgb[1];
  B[0]=rgb[2];
  
  // testing
  double spacing[3];
  this->OutputROIForColoring->GetSpacing(spacing);

  
  while(currStreamline && currActor)
    {
      
      currColor=0;
      newColor=1;
      // If we have this color already, store its index in currColor
      while (currColor<=lastColor && currColor<arraySize)
        {
          currActor->GetProperty()->GetColor(rgb);
          if (rgb[0]==R[currColor] &&
              rgb[1]==G[currColor] &&
              rgb[2]==B[currColor])
            {
              newColor=0;
              break;
            }
          currColor++;
        }
      
      if (newColor)
        {
          // increment count of colors
          lastColor=currColor;
          // save this color's info in the array
          R[currColor]=rgb[0];
          G[currColor]=rgb[1];
          B[currColor]=rgb[2];
        }
      // now currColor is set to this color's index, which we will
      // use to label voxels
      
      // for each point on the path, test
      // the nearest voxel for path/ROI intersection.
      vtkPoints *hs0=currStreamline->GetHyperStreamline0();
      vtkPoints *hs1=currStreamline->GetHyperStreamline1();
      int ptidx=0;
      int pt[3];
      double point[3], point2[3];
      for (ptidx = 0; ptidx < hs0->GetNumberOfPoints(); ptidx++)
        {
          hs0->GetPoint(ptidx,point);
          // First transform to world space.
          TensorScaledIJKToWorld->TransformPoint(point,point2);
          // Now transform to ROI IJK space
          WorldToROI->TransformPoint(point2,point);
          // Find that voxel number
          pt[0]= (int) floor(point[0]+0.5);
          pt[1]= (int) floor(point[1]+0.5);
          pt[2]= (int) floor(point[2]+0.5);
      
          //pt[0]= (int) floor(point[0]/spacing[0]+0.5);
          //pt[1]= (int) floor(point[1]/spacing[1]+0.5);
          //pt[2]= (int) floor(point[2]/spacing[2]+0.5);

          short *tmp = (short *) this->InputROIForColoring->GetScalarPointer(pt);
          if (tmp != NULL)
            {
              // if we are in the ROI to be colored 
              if (*tmp > 0) {
                
                tmp = (short *) this->OutputROIForColoring->GetScalarPointer(pt);
                *tmp = (short) (currColor + 1);
                
          
              }
            }
        }
      // Skip the first point in the second line since it
      // is a duplicate of the initial point.
      for (ptidx = 0; ptidx < hs1->GetNumberOfPoints(); ptidx++)
        {
          hs1->GetPoint(ptidx,point);
          // First transform to world space.
          TensorScaledIJKToWorld->TransformPoint(point,point2);
          // Now transform to ROI IJK space
          WorldToROI->TransformPoint(point2,point);
          // Find that voxel number
          pt[0]= (int) floor(point[0]+0.5);
          pt[1]= (int) floor(point[1]+0.5);
          pt[2]= (int) floor(point[2]+0.5);

          //pt[0]= (int) floor(point[0]/spacing[0]+0.5);
          //pt[1]= (int) floor(point[1]/spacing[1]+0.5);
          //pt[2]= (int) floor(point[2]/spacing[2]+0.5);

          short *tmp = (short *) this->InputROIForColoring->GetScalarPointer(pt);
          if (tmp != NULL)
            {
              // if we are in the ROI to be colored 
              if (*tmp > 0) {
                
                tmp = (short *) this->OutputROIForColoring->GetScalarPointer(pt);
                *tmp = (short) (currColor + 1);
                
              }
            }

        }                          
      
      // get next objects in collections
      currStreamline = dynamic_cast<vtkHyperStreamlinePoints *> 
        (this->Streamlines->GetNextItemAsObject());
      currActor = (vtkActor *) this->Actors->GetNextItemAsObject();
    }
}


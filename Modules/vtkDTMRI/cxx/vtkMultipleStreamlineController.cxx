#include "vtkMultipleStreamlineController.h"
#include "vtkLookupTable.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkRenderer.h"

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
  this->ROIToWorld = vtkTransform::New();
  this->WorldToTensorScaledIJK = vtkTransform::New();

  this->InputTensorField = NULL;
  this->InputROI = NULL;
  this->InputRenderers = vtkCollection::New();


  this->Streamlines = vtkCollection::New();
  //this->LookupTables = vtkCollection::New();
  this->Mappers = vtkCollection::New();
  this->Actors = vtkCollection::New();

  // Streamline parameters
  this->IntegrationDirection = VTK_INTEGRATE_BOTH_DIRECTIONS;
  this->StreamlineProperty = vtkProperty::New();
  this->ScalarVisibility=0;
  this->StreamlineLookupTable = vtkLookupTable::New();
  // make 0 dark blue, not red
  this->StreamlineLookupTable->SetHueRange(.6667, 0.0);

}

//----------------------------------------------------------------------------
vtkMultipleStreamlineController::~vtkMultipleStreamlineController()
{
  this->DeleteAllStreamlines();

  this->ROIToWorld->Delete();
  this->WorldToTensorScaledIJK->Delete();
  this->InputTensorField->Delete();
  this->InputROI->Delete();
  this->InputRenderers->Delete();
  this->Streamlines->Delete();
  //this->LookupTables->Delete();
  this->Mappers->Delete();
  this->Actors->Delete();
  this->StreamlineLookupTable->Delete();
}

// Set the properties of one streamline's graphics objects as requested
// by the user
//----------------------------------------------------------------------------
void vtkMultipleStreamlineController::ApplyUserSettingsToGraphicsObject(int index)
{
  vtkPolyDataMapper *currMapper;
  vtkActor *currActor;

  currActor = (vtkActor *) this->Actors->GetItemAsObject(index);
  currMapper = (vtkPolyDataMapper *) this->Mappers->GetItemAsObject(index);

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


}

// Make actors, mappers, and lookup tables as needed for streamlines
// in the collection.
// Corresponds to DTMRIAddStreamline in DTMRI.tcl.
//----------------------------------------------------------------------------
void vtkMultipleStreamlineController::CreateGraphicsObjects()
{
  // TO DO: see if all can use same LUT
  int numStreamlines, numActorsCreated;
  vtkHyperStreamline *currStreamline;
  //vtkLookupTable *currLookupTable;
  vtkPolyDataMapper *currMapper;
  vtkActor *currActor;
  vtkTransform *currTransform;
  vtkRenderer *currRenderer;

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
      //currLookupTable = vtkLookupTable::New();
      //this->LookupTables->AddItem((vtkObject *)currLookupTable);
      currActor = vtkActor::New();
      this->Actors->AddItem((vtkObject *)currActor);
      currMapper = vtkPolyDataMapper::New();
      this->Mappers->AddItem((vtkObject *)currMapper);

      // Apply user's visualization settings to these objects
      this->ApplyUserSettingsToGraphicsObject(numActorsCreated);

      // Hook up the pipeline
      currMapper->SetInput(currStreamline->GetOutput());
      //currMapper->SetLookupTable(currLookupTable);
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

  // make objects if needed
  this->CreateGraphicsObjects();

  // traverse actor collection and make all visible
  this->Actors->InitTraversal();
  currActor= (vtkActor *)this->Actors->GetNextItemAsObject();
  while(currActor)
    {
      currActor->VisibilityOn();
      currActor= (vtkActor *)this->Actors->GetNextItemAsObject();      

    }

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
      // (they change size as we do this)
      this->DeleteStreamline(0);
      i++;
    }
  
}

//----------------------------------------------------------------------------
void vtkMultipleStreamlineController::DeleteStreamline(int index)
{
  vtkRenderer *currRenderer;
  //vtkLookupTable *currLookupTable;
  vtkPolyDataMapper *currMapper;
  vtkHyperStreamline *currStreamline;
  vtkActor *currActor;

  cout << "actor " << index << endl;
  currActor = (vtkActor *) this->Actors->GetItemAsObject(index);
  currActor->VisibilityOff();
  // Remove from the scene (from each renderer)
  // Just like MainRemoveActor in Main.tcl.
  this->InputRenderers->InitTraversal();
  currRenderer= (vtkRenderer *)this->InputRenderers->GetNextItemAsObject();
  while(currRenderer)
    {
      cout << "rm actor from renderer " << currRenderer << endl;
      currRenderer->RemoveActor(currActor);
      currRenderer= (vtkRenderer *)this->InputRenderers->GetNextItemAsObject();
    }
  this->Actors->RemoveItem(index);
  currActor->Delete();
  
  cout << "stream" << endl;
  currStreamline = (vtkHyperStreamline *)
    this->Streamlines->GetItemAsObject(index);
  this->Streamlines->RemoveItem(index);
  currStreamline->Delete();
  
  cout << "mapper" << endl;
  currMapper = (vtkPolyDataMapper *) this->Mappers->GetItemAsObject(index);
  this->Mappers->RemoveItem(index);
  //currMapper->SetInput(NULL);
  //currMapper->SetLookupTable(NULL);
  currMapper->Delete();
  
  //cout << "lut" << endl;
  //currLookupTable = (vtkLookupTable *) this->LookupTables->GetItemAsObject(index);
  //this->LookupTables->RemoveItem(index);
  //currLookupTable->Delete();
  
  cout << "Done" << endl;

}

//----------------------------------------------------------------------------
void vtkMultipleStreamlineController::DeleteStreamline(vtkActor *pickedActor)
{
  int index;

  cout << "Picked actor (present?): " << pickedActor << endl;
  // find the actor on the collection and remove and delete all
  // corresponding objects. nonzero index means item was found.
  index = this->Actors->IsItemPresent(pickedActor);
  if (index)
    {
      // the index returned was 1-based but actually to get items
      // from the list we must use 0-based indices.  Very
      // strange but this is necessary.
      index--;

      this->DeleteStreamline(index);
    }
}

//----------------------------------------------------------------------------
int vtkMultipleStreamlineController::PointWithinTensorData(double *point, double *pointw)
{
  int *dims;
  float *spacing;
  int i, inbounds;

  dims=this->InputTensorField->GetDimensions();
  spacing=this->InputTensorField->GetSpacing();
  i=0;
  inbounds=1;
  while (i<3)
    {
      if (point[i] <0)
        inbounds=0;
      if (point[i] > dims[i]*spacing[i])
        inbounds=0;
      i++;
    }

  if (inbounds ==0)
    {
      cout << "point " << pointw[0] << " " << pointw[1] << " " << pointw[2] << " outside of tensor dataset" << endl;
    }

  return(inbounds);
}


//----------------------------------------------------------------------------
void vtkMultipleStreamlineController::SeedStreamlineFromPoint(double x, 
                                                              double y, 
                                                              double z)

{
  double pointw[3], point[3];
  vtkHyperStreamline *newStreamline;

  pointw[0]=x;
  pointw[1]=y;
  pointw[2]=z;
  
  // Transform from world coords to scaled ijk of the input tensors
  this->WorldToTensorScaledIJK->TransformPoint(pointw,point);

  // make sure it is within the bounds of the tensor dataset
  if (!this->PointWithinTensorData(point,pointw))
    return;

  // Now create a streamline and put it on the collection.
  //newStreamline=vtkHyperStreamline::New();
  newStreamline=vtkHyperStreamlinePoints::New();
  //newStreamline->DebugOn();
  this->Streamlines->AddItem((vtkObject *)newStreamline);
  
  // Set its input information.
  newStreamline->SetInput(this->InputTensorField);
  newStreamline->SetStartPosition(point[0],point[1],point[2]);
  
  // Set its parameters
  newStreamline->
    SetIntegrationDirection(this->IntegrationDirection);

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
  unsigned long target;
  short *inPtr;
  vtkHyperStreamline *newStreamline;

  // currently this filter is not multithreaded, though in the future 
  // it could be (if it inherits from an image filter class)
  this->InputROI->GetWholeExtent(inExt);
  this->InputROI->GetContinuousIncrements(inExt, inIncX, inIncY, inIncZ);

  // find the region to loop over
  maxX = inExt[1] - inExt[0];
  maxY = inExt[3] - inExt[2]; 
  maxZ = inExt[5] - inExt[4];

  cout << "Dims: " << maxX << " " << maxY << " " << maxZ << endl;
  cout << "Incr: " << inIncX << " " << inIncY << " " << inIncZ << endl;

  // for progress notification
  target = (unsigned long)((maxZ+1)*(maxY+1)/50.0);
  target++;

  // start point in input integer field
  inPtr = (short *) this->InputROI->GetScalarPointerForExtent(inExt);

  for (idxZ = 0; idxZ <= maxZ; idxZ++)
    {
      //for (idxY = 0; !this->AbortExecute && idxY <= maxY; idxY++)
      for (idxY = 0; idxY <= maxY; idxY++)
        {
          if (!(count%target)) 
            {
              //this->UpdateProgress(count/(50.0*target) + (maxZ+1)*(maxY+1));
              //cout << (count/(50.0*target) + (maxZ+1)*(maxY+1)) << endl;
              cout << "progress: " << count << endl;
            }
          count++;
          
          for (idxX = 0; idxX <= maxX; idxX++)
            {
              // If the point is equal to the ROI value then seed here.
              if (*inPtr == this->InputROIValue)
                {
                  cout << "***** multiple streamline " << idxX << " " <<
                    idxY << " " << idxZ << " *****" << endl;

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
                      //newStreamline=vtkHyperStreamlineDTMRI::New();
                      newStreamline=vtkHyperStreamlinePoints::New();
                      //newStreamline=vtkHyperStreamline::New();
                      //newStreamline->DebugOn();
                      this->Streamlines->AddItem((vtkObject *)newStreamline);
                      
                      // Set its input information.
                      newStreamline->SetInput(this->InputTensorField);
                      newStreamline->SetStartPosition(point[0],point[1],point[2]);
                      
                      // Set its parameters
                      newStreamline->
                        SetIntegrationDirection(this->IntegrationDirection);
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


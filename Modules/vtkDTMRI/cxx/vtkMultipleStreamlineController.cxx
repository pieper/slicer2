#include "vtkMultipleStreamlineController.h"
#include "vtkLookupTable.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkRenderer.h"
#include "vtkAppendPolyData.h"
#include "vtkPolyDataWriter.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkMrmlModelNode.h"
#include "vtkErrorCode.h"

#include <string>


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
  this->WorldToTensorScaledIJK = vtkTransform::New();

  // The user must set these for the class to function.
  this->InputTensorField = NULL;
  this->InputROI = NULL;
  this->InputRenderers = vtkCollection::New();
  this->InputROIValue = -1;

  // collections
  this->Streamlines = vtkCollection::New();
  this->Mappers = vtkCollection::New();
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
  this->StreamlineLookupTable->Delete();
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
//----------------------------------------------------------------------------
void vtkMultipleStreamlineController::CreateGraphicsObjects()
{
  int numStreamlines, numActorsCreated;
  vtkHyperStreamline *currStreamline;
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
      currActor = vtkActor::New();
      this->Actors->AddItem((vtkObject *)currActor);
      currMapper = vtkPolyDataMapper::New();
      this->Mappers->AddItem((vtkObject *)currMapper);

      // Apply user's visualization settings to these objects
      this->ApplyUserSettingsToGraphicsObject(numActorsCreated);

      // Hook up the pipeline
      currMapper->SetInput(currStreamline->GetOutput());
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
  vtkHyperStreamline *currStreamline;
  vtkActor *currActor;

  vtkDebugMacro( << "Deleting actor " << index);
  currActor = (vtkActor *) this->Actors->GetItemAsObject(index);
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
  
  vtkDebugMacro( << "Delete stream" );
  currStreamline = (vtkHyperStreamline *)
    this->Streamlines->GetItemAsObject(index);
  this->Streamlines->RemoveItem(index);
  currStreamline->Delete();
  
  vtkDebugMacro( << "Delete mapper" );
  currMapper = (vtkPolyDataMapper *) this->Mappers->GetItemAsObject(index);
  this->Mappers->RemoveItem(index);
  currMapper->Delete();
  
  vtkDebugMacro( << "Done deleting streamline");

}

// This is the delete called from the user interface where the
// actor has been picked with the mouse.
//----------------------------------------------------------------------------
void vtkMultipleStreamlineController::DeleteStreamline(vtkActor *pickedActor)
{
  int index;

  vtkDebugMacro( << "Picked actor (present?): " << pickedActor);
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

// Save only one streamline. Called from within functions that save 
// many streamlines in a loop.
// Current format is x1,y1,z1 x2,y2,z2 x3,y3,z3 \n
void vtkMultipleStreamlineController::SaveStreamlineAsTextFile(ofstream &file,
                                                               vtkHyperStreamlinePoints *currStreamline)
{
  vtkPoints *hs0, *hs1;
  int ptidx, numPts;
  double point[3];

  
  //GetHyperStreamline0/1 and write their points.
  hs0=currStreamline->GetHyperStreamline0();
  hs1=currStreamline->GetHyperStreamline1();
  
  // Write the first one in reverse order since both lines
  // travel outward from the initial point.
  // Also, skip the first point in the second line since it
  // is a duplicate of the initial point.
  numPts=hs0->GetNumberOfPoints();
  ptidx=numPts-1;
  while (ptidx >= 0)
    {
      hs0->GetPoint(ptidx,point);
      //file << point[0] << " " << point[1] << " " << point[2] << endl;
      file << point[0] << "," << point[1] << "," << point[2] << " ";
      ptidx--;
    }
  numPts=hs1->GetNumberOfPoints();
  ptidx=1;
  while (ptidx < numPts)
    {
      hs1->GetPoint(ptidx,point);
      //file << point[0] << " " << point[1] << " " << point[2] << endl;
      file << point[0] << "," << point[1] << "," << point[2] << " ";
      ptidx++;
    }
  file << endl;
}


void vtkMultipleStreamlineController::SaveStreamlinesAsTextFiles(char *filename)
{ 
  std::string fileNameStr;
  std::string idxStr;
  vtkHyperStreamlinePoints *currStreamline;
  ofstream file;
  int idx;

  // traverse streamline collection
  this->Streamlines->InitTraversal();
  // TO DO: make sure this is a vtkHyperStreamlinePoints object
  currStreamline= (vtkHyperStreamlinePoints *)this->Streamlines->GetNextItemAsObject();
  
  // test we have streamlines
  if (currStreamline == NULL)
    {
      vtkErrorMacro("No streamlines have been created yet.");
      return;      
    }


  cout << "Traverse STREAMLINES" << endl;

  idx=0;
  while(currStreamline)
    {
      cout << "stream " << currStreamline << endl;
      
      idxStr = idx + ".txt";
      fileNameStr = filename + '_' + idxStr;
      file.open(fileNameStr.c_str());
      if (file.fail())
        {
          vtkErrorMacro("Write: Could not open file " << fileNameStr.c_str());
          cerr << "Write: Could not open file " << fileNameStr.c_str();
#if (VTK_MAJOR_VERSION <= 5)      
          this->SetErrorCode(2);
#else
          this->SetErrorCode(vtkErrorCode::GetErrorCodeFromString("CannotOpenFileError"));
#endif
          return;
        }

      this->SaveStreamlineAsTextFile(file, currStreamline);
      
      // Close file
      file.close();
      
      // get next object in collection
      currStreamline= (vtkHyperStreamlinePoints *)
        this->Streamlines->GetNextItemAsObject();

      idx++;
    }

#if (VTK_MAJOR_VERSION <= 5)      
  this->SetErrorCode(0);
#else
  this->SetErrorCode(vtkErrorCode::GetErrorCodeFromString("NoError"));
#endif
  
}


void vtkMultipleStreamlineController::SaveStreamlinesAsPolyData(char *filename,
                                                                char *name)
{
  this->SaveStreamlinesAsPolyData(filename, name, NULL);
}

// NOTE: Limit currently is 1000 models (1000 different input colors is max).
//----------------------------------------------------------------------------
void vtkMultipleStreamlineController::SaveStreamlinesAsPolyData(char *filename,
                                                                char *name,
                                                                vtkMrmlTree *colorTree)
{
  vtkHyperStreamline *currStreamline;
  vtkActor *currActor;
  vtkCollection *collectionOfModels;
  vtkAppendPolyData *currAppender;
  vtkPolyDataWriter *writer;
  vtkTransformPolyDataFilter *currTransformer;
  vtkTransform *currTransform;
  double R[1000], G[1000], B[1000];
  int arraySize=1000;
  int lastColor;
  int currColor, newColor, idx;
  vtkFloatingPointType rgb_vtk_float[3];
  double rgb[3];
  std::string fileNameStr;
  std::string idxStr;
  vtkMrmlTree *tree;
  vtkMrmlModelNode *currNode;
  vtkMrmlColorNode *currColorNode;

  // traverse streamline collection, grouping streamlines into models by color
  this->Streamlines->InitTraversal();
  this->Actors->InitTraversal();
  currStreamline= (vtkHyperStreamline *)this->Streamlines->GetNextItemAsObject();
  currActor= (vtkActor *)this->Actors->GetNextItemAsObject();

  // test we have actors and streamlines
  if (currActor == NULL || currStreamline == NULL)
    {
      vtkErrorMacro("No streamlines have been created yet.");
      return;      
    }

  // init things with the first streamline.
  currActor->GetProperty()->GetColor(rgb);
  currAppender = vtkAppendPolyData::New();
  collectionOfModels = vtkCollection::New();
  collectionOfModels->AddItem((vtkObject *)currAppender);
  lastColor=0;
  R[0]=rgb[0];
  G[0]=rgb[1];
  B[0]=rgb[2];

  cout << "Traverse STREAMLINES" << endl;
  while(currStreamline && currActor)
    {
      cout << "stream " << currStreamline << endl;
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

      // if this is a new color, we must create a new model to save.
      if (newColor)
        {
          // add an appender to the collection of models.
          currAppender = vtkAppendPolyData::New();
          collectionOfModels->AddItem((vtkObject *)currAppender);
          // increment count of colors
          lastColor=currColor;
          // save this color's info in the array
          R[currColor]=rgb[0];
          G[currColor]=rgb[1];
          B[currColor]=rgb[2];
        }
      else
        {
          // use the appender number currColor that we found in the while loop
          currAppender = (vtkAppendPolyData *) 
            collectionOfModels->GetItemAsObject(currColor);
        }

      // transform models so that they are written in the coordinate
      // system in which they are displayed.
      // Currently this class displays them with 
      // currActor->SetUserMatrix(currTransform->GetMatrix());
      currTransformer = vtkTransformPolyDataFilter::New();
      currTransform = vtkTransform::New();
      currTransform->SetMatrix(currActor->GetUserMatrix());
      currTransformer->SetTransform(currTransform);
      currTransformer->SetInput(currStreamline->GetOutput());

      // Append this streamline to the chosen model using the appender
      currAppender->AddInput(currTransformer->GetOutput());

      // call Delete on both to decrement the reference count
      // so that when we delete the appender they delete too.
      currTransformer->Delete();
      currTransform->Delete();

      // get next objects in collections
      currStreamline= (vtkHyperStreamline *)
        this->Streamlines->GetNextItemAsObject();
      currActor = (vtkActor *) this->Actors->GetNextItemAsObject();
    }


  // traverse appender collection (collectionOfModels) and write each to disk
  cout << "Traverse APPENDERS" << endl;
  writer = vtkPolyDataWriter::New();
  tree = vtkMrmlTree::New();
  collectionOfModels->InitTraversal();
  currAppender = (vtkAppendPolyData *) 
    collectionOfModels->GetNextItemAsObject();
  idx=0;
  while(currAppender)
    {
      cout << idx << endl;
      writer->SetInput(currAppender->GetOutput());
      writer->SetFileType(2);
      idxStr = idx + ".vtk";
      fileNameStr = filename + '_' + idxStr;
      writer->SetFileName(fileNameStr.c_str());
      writer->Write();
      
      // Delete it (but it survives until the collection it's on is deleted).
      currAppender->Delete();

      // Also write a MRML file: add to MRML tree
      currNode=vtkMrmlModelNode::New();
      currNode->SetFullFileName(fileNameStr.c_str());
      currNode->SetFileName(fileNameStr.c_str());
      // use the name argument to name the model
      fileNameStr = name + '_' + idxStr;
      currNode->SetName(fileNameStr.c_str());
      currNode->SetDescription("Model of a DTMRI tract");
      if (this->ScalarVisibility) currNode->ScalarVisibilityOn();
      currNode->ClippingOn();
      currNode->SetScalarRange(this->StreamlineLookupTable->GetTableRange());
      // If we have a color tree as input find the name of the color
      if (colorTree)
        {
          colorTree->InitTraversal();
          currColorNode = (vtkMrmlColorNode *) colorTree->GetNextItemAsObject();
          while (currColorNode)
            {
              currColorNode->GetDiffuseColor(rgb_vtk_float);
              if (rgb_vtk_float[0]==R[idx] &&
                  rgb_vtk_float[1]==G[idx] &&
                  rgb_vtk_float[2]==B[idx])              
                {
                  currNode->SetColor(currColorNode->GetName());
                  break;
                }
              currColorNode = (vtkMrmlColorNode *) 
                colorTree->GetNextItemAsObject();
            }
        }

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
  
  // Write the MRML file
  fileNameStr = filename;
  fileNameStr += ".xml";
  tree->Write((char *)fileNameStr.c_str());

  cout << "DELETING" << endl;

  // Delete all objects we created
  collectionOfModels->Delete();
  writer->Delete();
  tree->Delete();

}





// Test whether the given point is in bounds (inside the input data)
//----------------------------------------------------------------------------
int vtkMultipleStreamlineController::PointWithinTensorData(double *point, double *pointw)
{
  int *dims;
  vtkFloatingPointType *spacing;
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
  
  // Transform from world coords to scaled ijk of the input tensors
  this->WorldToTensorScaledIJK->TransformPoint(pointw,point);

  // make sure it is within the bounds of the tensor dataset
  if (!this->PointWithinTensorData(point,pointw))
    return;

  // Now create a streamline and put it on the collection.
  newStreamline=this->CreateHyperStreamline();
  this->Streamlines->AddItem((vtkObject *)newStreamline);
  
  // Set its input information.
  newStreamline->SetInput(this->InputTensorField);
  newStreamline->SetStartPosition(point[0],point[1],point[2]);
  
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



// Seed each streamline, cause it to Update, save its info to disk
// and then Delete it.  This is a way to seed in the whole brain
// without running out of memory. Nothing is displayed in the renderers.
//----------------------------------------------------------------------------
void vtkMultipleStreamlineController::SeedAndSaveStreamlinesFromROI(char *filename)
{
  int idxX, idxY, idxZ;
  int maxX, maxY, maxZ;
  int inIncX, inIncY, inIncZ;
  int inExt[6];
  double point[3], point2[3];
  unsigned long count = 0;
  unsigned long target;
  int count2 = 0;
  short *inPtr;
  vtkHyperStreamline *newStreamline;
  vtkTransform *transform;
  vtkTransformPolyDataFilter *transformer;
  vtkPolyDataWriter *writer;
  std::string fileNameStr;
  std::string idxStr;
  int idx;
  ofstream file;

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


  // Create transformation matrix to place actors in scene
  // This is used to transform the models before writing them to disk
  transform=vtkTransform::New();
  transform->SetMatrix(this->WorldToTensorScaledIJK->GetMatrix());
  transform->Inverse();
  transformer=vtkTransformPolyDataFilter::New();
  transformer->SetTransform(transform);

  writer = vtkPolyDataWriter::New();

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


  // Save all points to the same text file.
  //snprintf(fileName,100,"%s.3dpts",filename);
  fileNameStr = filename;
  fileNameStr += ".3dpts";
  // Open file
  file.open(fileNameStr.c_str());
  if (file.fail())
    {
      vtkErrorMacro("Write: Could not open file " 
                    << fileNameStr.c_str());
      cerr << "Write: Could not open file " << fileNameStr.c_str();
#if (VTK_MAJOR_VERSION <= 5)      
      this->SetErrorCode(2);
#else
      this->SetErrorCode(vtkErrorCode::GetErrorCodeFromString("CannotOpenFileError"));
#endif
      return;
    }                   


  // for progress notification
  target = (unsigned long)((maxZ+1)*(maxY+1)/50.0);
  target++;

  // start point in input integer field
  inPtr = (short *) this->InputROI->GetScalarPointerForExtent(inExt);

  // filename index
  idx=0;

  for (idxZ = 0; idxZ <= maxZ; idxZ++)
    {
      //for (idxY = 0; !this->AbortExecute && idxY <= maxY; idxY++)
      for (idxY = 0; idxY <= maxY; idxY++)
        {
          if (!(count%target)) 
          {
            //this->UpdateProgress(count/(50.0*target) + (maxZ+1)*(maxY+1));
            //cout << (count/(50.0*target) + (maxZ+1)*(maxY+1)) << endl;
            //cout << "progress: " << count << endl;
            // just output numbers from 1 to 50.
            cout << count2 << endl;
            count2++;
          }
          count++;
          
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
                      // Now create a streamline 
                      newStreamline=this->CreateHyperStreamline();
                      
                      // Set its input information.
                      newStreamline->SetInput(this->InputTensorField);
                      newStreamline->SetStartPosition(point[0],point[1],point[2]);
                      //newStreamline->DebugOn();
                      
                      // Force it to execute
                      newStreamline->Update();

                      // transform model
                      transformer->SetInput(newStreamline->GetOutput());

                      // Save the model to disk
                      writer->SetInput(transformer->GetOutput());
                      writer->SetFileType(2);
                      //snprintf(fileName,100,"%s_%d.vtk",filename,idx);
                      idxStr = idx + ".vtk";
                      fileNameStr = filename + '_' + idxStr;
                      writer->SetFileName(fileNameStr.c_str());
                      writer->Write();

                      // Save the center points to disk
                      if (newStreamline->IsA("vtkHyperStreamlinePoints"))
                        {
                          this->SaveStreamlineAsTextFile(file,(vtkHyperStreamlinePoints *) newStreamline);
                        }

                      // Delete objects
                      newStreamline->Delete();

                      idx++;
                    }
                }
              inPtr++;
              inPtr += inIncX;
            }
          inPtr += inIncY;
        }
      inPtr += inIncZ;
    }

  transform->Delete();
  transformer->Delete();
  writer->Delete();

  // Close text file
  file.close();

}


/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkSeedTracts.cxx,v $
  Date:      $Date: 2006/02/26 02:39:36 $
  Version:   $Revision: 1.19 $

=========================================================================auto=*/

#include "vtkSeedTracts.h"
#include "vtkFloatArray.h"
#include "vtkCellArray.h"
#include "vtkStreamlineConvolve.h"
#include "vtkPruneStreamline.h"

#include "vtkTransformPolyDataFilter.h"
#include "vtkPolyDataWriter.h"
#include "vtkTimerLog.h"

#include <sstream>
#include <string>

//------------------------------------------------------------------------------
vtkSeedTracts* vtkSeedTracts::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkSeedTracts");
  if(ret)
    {
      return (vtkSeedTracts*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkSeedTracts;
}

//----------------------------------------------------------------------------
vtkSeedTracts::vtkSeedTracts()
{
  // matrices
  // Initialize these to identity, so if the user doesn't set them it's okay.
  this->ROIToWorld = vtkTransform::New();
  this->ROI2ToWorld = vtkTransform::New();
  this->WorldToTensorScaledIJK = vtkTransform::New();

  // The user must set these for the class to function.
  this->InputTensorField = NULL;
  
  // The user may need to set these, depending on class usage
  this->InputROI = NULL;
  this->InputROIValue = -1;
  this->InputMultipleROIValues = NULL;
  this->InputROI2 = NULL;

  // if the user doesn't set these they will be ignored
  this->VtkHyperStreamlineSettings=NULL;
  this->VtkHyperStreamlinePointsSettings=NULL;
  this->VtkPreciseHyperStreamlinePointsSettings=NULL;
  this->VtkHyperStreamlineTeemSettings=NULL;

  // default to vtkHyperStreamline class creation
  this->UseVtkHyperStreamline();

  // collections
  this->Streamlines = vtkCollection::New();


  // Streamline parameters for all streamlines
  this->IntegrationDirection = VTK_INTEGRATE_BOTH_DIRECTIONS;

  this->MinimumPathLength = 15;
}

//----------------------------------------------------------------------------
vtkSeedTracts::~vtkSeedTracts()
{
  // matrices
  this->ROIToWorld->Delete();
  this->ROI2ToWorld->Delete();
  this->WorldToTensorScaledIJK->Delete();
  
  // volumes
  if (this->InputTensorField) this->InputTensorField->Delete();
  if (this->InputROI) this->InputROI->Delete();
  if (this->InputROI2) this->InputROI->Delete();

  // settings
  if (this->VtkHyperStreamlineSettings) 
    this->VtkHyperStreamlineSettings->Delete();
  if (this->VtkHyperStreamlinePointsSettings) 
    this->VtkHyperStreamlinePointsSettings->Delete();
  if (this->VtkPreciseHyperStreamlinePointsSettings) 
    this->VtkPreciseHyperStreamlinePointsSettings->Delete();
  if (this->VtkHyperStreamlineTeemSettings) 
    this->VtkHyperStreamlineTeemSettings->Delete();

  // collection
  if (this->Streamlines) this->Streamlines->Delete();
}


// Here we create the type of streamline class requested by the user.
// Elsewhere in this class, all are treated as vtkHyperStreamline *.
// We copy settings from the example object that the user has access
// to.
// (It would be nicer if we required the hyperstreamline classes to 
// implement a copy function.)
//----------------------------------------------------------------------------
vtkHyperStreamline * vtkSeedTracts::CreateHyperStreamline()
{
  vtkHyperStreamline *currHS;
  vtkHyperStreamlineDTMRI *currHSP;
  vtkPreciseHyperStreamlinePoints *currPHSP;
  vtkHyperStreamlineTeem *currHST;

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
    case USE_VTK_HYPERSTREAMLINE_TEEM:
      if (this->VtkHyperStreamlineTeemSettings) 
        {
          // create object
          std::cout << "Creatng HST" << endl;
          currHST=vtkHyperStreamlineTeem::New();

          std::cout << "settings for HST" << endl;

          this->UpdateHyperStreamlineTeemSettings(currHST);


          std::cout << "returning HST" << endl;

          return((vtkHyperStreamline *)currHST);
        }
      else
        {
          return((vtkHyperStreamline *) vtkHyperStreamlineTeem::New());
        }
      break;
    case USE_VTK_HYPERSTREAMLINE_POINTS:
      if (this->VtkHyperStreamlinePointsSettings) 
        {
          // create object
          currHSP=vtkHyperStreamlineDTMRI::New();

          this->UpdateHyperStreamlinePointsSettings(currHSP);

          return((vtkHyperStreamline *)currHSP);
        }
      else
        {
          return((vtkHyperStreamline *) vtkHyperStreamlineDTMRI::New());

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

// Loop through all of the hyperstreamline objects and set their
// parameters according to the current vtkHyperStreamline*Settings object
// which the user can modify. 
//----------------------------------------------------------------------------
void vtkSeedTracts::UpdateAllHyperStreamlineSettings()
{
  vtkObject *currStreamline;
  vtkHyperStreamlineDTMRI *currHSP;
  vtkHyperStreamlineTeem *currHST;

  // traverse streamline collection
  this->Streamlines->InitTraversal();

  currStreamline= (vtkObject *)this->Streamlines->GetNextItemAsObject();

  while(currStreamline)
    {
      vtkDebugMacro( << currStreamline->GetClassName() );
      if (strcmp(currStreamline->GetClassName(),"vtkHyperStreamlineDTMRI") == 0)
        {
          vtkDebugMacro( << " match" );
          currHSP = (vtkHyperStreamlineDTMRI *) currStreamline;
          this->UpdateHyperStreamlinePointsSettings(currHSP);
          currHSP->Update();
        }
      if (strcmp(currStreamline->GetClassName(),"vtkHyperStreamlineTeem") == 0)
        {
          vtkDebugMacro( << " match" );
          currHST = (vtkHyperStreamlineTeem *) currStreamline;
          this->UpdateHyperStreamlineTeemSettings(currHST);
          currHST->Update();
        }

      currStreamline= (vtkObject *)this->Streamlines->GetNextItemAsObject();
    }
}

// Update settings of one hyper streamline
//----------------------------------------------------------------------------
void vtkSeedTracts::UpdateHyperStreamlinePointsSettings( vtkHyperStreamlineDTMRI *currHSP)
{

  // Copy user's settings into this object:
  
  // MaximumPropagationDistance 
  currHSP->SetMaximumPropagationDistance(this->VtkHyperStreamlinePointsSettings->GetMaximumPropagationDistance());
  // IntegrationStepLength
  currHSP->SetIntegrationStepLength(this->VtkHyperStreamlinePointsSettings->GetIntegrationStepLength());
  // RadiusOfCurvature
  currHSP->SetRadiusOfCurvature(this->VtkHyperStreamlinePointsSettings->GetRadiusOfCurvature());
  
  // Stopping threshold
  currHSP->SetStoppingThreshold(this->VtkHyperStreamlinePointsSettings->GetStoppingThreshold());
  
  // Stopping Mode
  currHSP->SetStoppingMode(this->VtkHyperStreamlinePointsSettings->GetStoppingMode());
  
  
  // Eigenvector to integrate
  currHSP->SetIntegrationEigenvector(this->VtkHyperStreamlinePointsSettings->GetIntegrationEigenvector());
  
  // IntegrationDirection (set in this class, default both ways)
  currHSP->SetIntegrationDirection(this->IntegrationDirection);

}

// Update settings of one hyper streamline:
// This is where teem hyperstreamlines have their settings updated
// from the user interface.
//----------------------------------------------------------------------------
void vtkSeedTracts::UpdateHyperStreamlineTeemSettings( vtkHyperStreamlineTeem *currHST)
{

  std::cout << "in settings  function HST" << endl;

  // Potentially this should update the tendFiberContext class for the given volume,
  // instead of updating all streamlines.
  
  // Copy user's settings into this object:
  
  // MaximumPropagationDistance 
  currHST->SetMaximumPropagationDistance(this->VtkHyperStreamlineTeemSettings->GetMaximumPropagationDistance());
  // IntegrationStepLength
  currHST->SetIntegrationStepLength(this->VtkHyperStreamlineTeemSettings->GetIntegrationStepLength());
  // RadiusOfCurvature
  currHST->SetRadiusOfCurvature(this->VtkHyperStreamlineTeemSettings->GetRadiusOfCurvature());
  
  // Stopping threshold
  currHST->SetStoppingThreshold(this->VtkHyperStreamlineTeemSettings->GetStoppingThreshold());
  
  // Stopping Mode
  currHST->SetStoppingMode(this->VtkHyperStreamlineTeemSettings->GetStoppingMode());
  
  
  // Eigenvector to integrate
  currHST->SetIntegrationEigenvector(this->VtkHyperStreamlineTeemSettings->GetIntegrationEigenvector());
  
  // IntegrationDirection (set in this class, default both ways)
  currHST->SetIntegrationDirection(this->IntegrationDirection);

  std::cout << "DONE in settings  function HST" << endl;
}


// Test whether the given point is in bounds (inside the input data)
//----------------------------------------------------------------------------
int vtkSeedTracts::PointWithinTensorData(double *point, double *pointw)
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
      std::cout << "point " << pointw[0] << " " << pointw[1] << " " << pointw[2] << " outside of tensor dataset" << endl;
    }

  return(inbounds);
}

//----------------------------------------------------------------------------
void vtkSeedTracts::SeedStreamlineFromPoint(double x, 
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

void vtkSeedTracts::SeedStreamlinesInROIWithMultipleValues()
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
      this->SeedStreamlinesInROI();
    }
    
  //Restore InputROIValue variable
  this->InputROIValue = initialROIValue;   
}       
      

//----------------------------------------------------------------------------
void vtkSeedTracts::SeedStreamlinesInROI()
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

  // time
  vtkTimerLog *timer = vtkTimerLog::New();
  timer->StartTimer();

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

  vtkDebugMacro( << "Seed streamlines in ROI");

  // currently this filter is not multithreaded, though in the future 
  // it could be (especially if it inherits from an image filter class)
  this->InputROI->GetWholeExtent(inExt);
  this->InputROI->GetContinuousIncrements(inExt, inIncX, inIncY, inIncZ);

  // find the region to loop over
  maxX = inExt[1] - inExt[0];
  maxY = inExt[3] - inExt[2]; 
  maxZ = inExt[5] - inExt[4];

  vtkDebugMacro( << "Extent: " << inExt[0] << " " << inExt[1] << " " << inExt[2] << " " << inExt[3] << " " << inExt[4] << " " << inExt[5]);
  vtkDebugMacro( << "Dims: " << maxX << " " << maxY << " " << maxZ);
  vtkDebugMacro( << "Incr: " << inIncX << " " << inIncY << " " << inIncZ);

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

  timer->StopTimer();
  std::cout << "Tractography in ROI time: " << timer->GetElapsedTime() << endl;

}

// seed in each voxel in the ROI, only keep paths that intersect the
// second ROI
//----------------------------------------------------------------------------
void vtkSeedTracts::SeedStreamlinesFromROIIntersectWithROI2()
{

  int idxX, idxY, idxZ;
  int maxX, maxY, maxZ;
  int inIncX, inIncY, inIncZ;
  int inExt[6];
  double point[3], point2[3];
  unsigned long count = 0;
  //unsigned long target;
  short *inPtr;
  vtkHyperStreamlineDTMRI *newStreamline;

  // time
  vtkTimerLog *timer = vtkTimerLog::New();
  timer->StartTimer();

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
                      newStreamline=(vtkHyperStreamlineDTMRI *) this->CreateHyperStreamline();

                      // Set its input information.
                      newStreamline->SetInput(this->InputTensorField);
                      newStreamline->SetStartPosition(point[0],point[1],point[2]);
                      
                      // Force it to update to access the path points
                      newStreamline->Update();
                      
                      // for each point on the path, test
                      // the nearest voxel for path/ROI intersection.
                      vtkPoints * hs0
                        = newStreamline->GetOutput()->GetCell(0)->GetPoints();

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

                      vtkPoints * hs1
                        = newStreamline->GetOutput()->GetCell(1)->GetPoints();
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

  timer->StopTimer();
  std::cout << "Tractography in ROI time: " << timer->GetElapsedTime() << endl;
}


// Seed each streamline, cause it to Update, save its info to disk
// and then Delete it.  This is a way to seed in the whole brain
// without running out of memory. Nothing is displayed in the renderers.
//----------------------------------------------------------------------------
void vtkSeedTracts::SeedAndSaveStreamlinesInROI(char *pointsFilename, char *modelFilename)
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
  vtkHyperStreamlineDTMRI *newStreamline;
  vtkTransform *transform;
  vtkTransformPolyDataFilter *transformer;
  vtkPolyDataWriter *writer;
  std::stringstream fileNameStr;
  int idx;
  ofstream filePoints, fileCoordinateSystemInfo;

  // time
  vtkTimerLog *timer = vtkTimerLog::New();
  timer->StartTimer();

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


  // make sure we are creating objects with points
  this->UseVtkHyperStreamlinePoints();

  // Create transformation matrix to place actors in scene
  // This is used to transform the models before writing them to disk
  transform=vtkTransform::New();
  transform->SetMatrix(this->WorldToTensorScaledIJK->GetMatrix());
  transform->Inverse();
  transformer=vtkTransformPolyDataFilter::New();
  transformer->SetTransform(transform);


  // Store information to put points into 
  // centered scaled IJK space, instead of just
  // leaving them in scaled IJK. This way the output
  // can be transformed into Lilla Zollei's coordinate
  // system so we can use her registration.
  // Also we save the world to scaled IJK transform,
  // now that paths are stored in world coords.

  // Open file
  fileNameStr << pointsFilename << ".coords";
  fileCoordinateSystemInfo.open(fileNameStr.str().c_str());
  if (fileCoordinateSystemInfo.fail())
    {
      vtkErrorMacro("Write: Could not open file " 
                    << fileNameStr.str().c_str());
      cerr << "Write: Could not open file " << fileNameStr.str().c_str();
      return;
    }        
  int extent[6];
  double spacing[3];
  fileCoordinateSystemInfo << "extent of tensor volume" << endl; 
  this->InputTensorField->GetWholeExtent(extent);
  fileCoordinateSystemInfo << extent[0] << " " << extent[1] << " " << extent[2] << 
    " " << extent[3] << " " << extent[4] << " " << extent[5] << endl;
  fileCoordinateSystemInfo << "voxel dimensions of tensor volume" << endl; 
  this->InputTensorField->GetSpacing(spacing);
  fileCoordinateSystemInfo << spacing[0] << " " << spacing[1] << " " << spacing[2] << endl;
  fileCoordinateSystemInfo << "world to scaled IJK transform" << endl; 
  for (int idxI = 0; idxI < 4; idxI++)
    {
      for (int idxJ = 0; idxJ < 4; idxJ++)
        {
          fileCoordinateSystemInfo << this->WorldToTensorScaledIJK->GetMatrix()->GetElement(idxI,idxJ) << " ";
        }
    }
  fileCoordinateSystemInfo << endl;

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
  fileNameStr.str("");
  fileNameStr << pointsFilename << ".3dpts";

  // Open file
  filePoints.open(fileNameStr.str().c_str());
  if (filePoints.fail())
    {
      vtkErrorMacro("Write: Could not open file " 
                    << fileNameStr.str().c_str());
      cerr << "Write: Could not open file " << fileNameStr.str().c_str();
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
              std::cout << count2 << endl;
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
                      newStreamline=(vtkHyperStreamlineDTMRI *) 
                        this->CreateHyperStreamline();
                      
                      // Set its input information.
                      newStreamline->SetInput(this->InputTensorField);
                      newStreamline->SetStartPosition(point[0],point[1],point[2]);
                      //newStreamline->DebugOn();
                      
                      // Force it to execute
                      newStreamline->Update();

                      // See if we like it enough to write
                      // This relies on the fact that the step length is in units of
                      // length (unlike fractions of a cell in vtkHyperStreamline).
                      double length = 
                        (newStreamline->GetOutput()->GetNumberOfPoints() - 1) * 
                        newStreamline->GetIntegrationStepLength();
 
                      if (length > this->MinimumPathLength)
                        {
                          
                          // transform model
                          transformer->SetInput(newStreamline->GetOutput());
                          
                          // Save the model to disk
                          writer->SetInput(transformer->GetOutput());
                          writer->SetFileType(2);
                          
                          // clear the buffer (set to empty string)
                          fileNameStr.str("");
                          fileNameStr << modelFilename << '_' << idx << ".vtk";
                          writer->SetFileName(fileNameStr.str().c_str());
                          writer->Write();
                          
                          // Save the center points to disk
                          this->SaveStreamlineAsTextFile(filePoints,transformer->GetOutput());

                          idx++;
                        }

                      // Delete objects
                      newStreamline->Delete();

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
  filePoints.close();
  
  // Tell user how many we wrote
  std::cout << "Wrote " << idx << "model files." << endl;

  fileCoordinateSystemInfo << "Model files written:" << endl;
  fileCoordinateSystemInfo << idx << endl;
  fileCoordinateSystemInfo.close();

  timer->StopTimer();
  std::cout << "Tractography in ROI time: " << timer->GetElapsedTime() << endl;
}

// Save only one streamline. Called from within functions that save 
// many streamlines in a loop.
// Current format is x1,y1,z1 x2,y2,z2 x3,y3,z3 \n
//----------------------------------------------------------------------------
void vtkSeedTracts::SaveStreamlineAsTextFile(ofstream &filePoints,
                                             vtkPolyData *currStreamline)
{
  vtkPoints *hs0, *hs1;
  int ptidx, numPts;
  double point[3];

  
  //GetHyperStreamline0/1 and write their points.
  hs0=currStreamline->GetCell(0)->GetPoints();

  // Write the first one in reverse order since both lines
  // travel outward from the initial point.
  // Also, skip the first point in the second line since it
  // is a duplicate of the initial point.
  numPts=hs0->GetNumberOfPoints();
  ptidx=numPts-1;
  while (ptidx >= 0)
    {
      hs0->GetPoint(ptidx,point);
      filePoints << point[0] << "," << point[1] << "," << point[2] << " ";
      ptidx--;
    }

  hs1=currStreamline->GetCell(1)->GetPoints();
  numPts=hs1->GetNumberOfPoints();
  ptidx=1;
  while (ptidx < numPts)
    {
      hs1->GetPoint(ptidx,point);
      filePoints << point[0] << "," << point[1] << "," << point[2] << " ";
      ptidx++;
    }
  filePoints << endl;

}



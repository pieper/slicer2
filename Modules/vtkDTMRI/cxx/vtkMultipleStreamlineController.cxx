#include "vtkMultipleStreamlineController.h"

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

  this->Streamlines = vtkCollection::New();

  // Streamline parameters
  this->IntegrationDirection = VTK_INTEGRATE_BOTH_DIRECTIONS;
}

//----------------------------------------------------------------------------
vtkMultipleStreamlineController::~vtkMultipleStreamlineController()
{
  // Delete everything

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

                  // Now create a streamline and put it on the collection.
                  newStreamline=vtkHyperStreamlineDTMRI::New();
                  this->Streamlines->AddItem((vtkObject *)newStreamline);

                  // Set its input information.
                  newStreamline->SetInput(this->InputTensorField);
                  newStreamline->SetStartPosition(point[0],point[1],point[2]);

                  // Set its parameters
                  newStreamline->
                    SetIntegrationDirection(this->IntegrationDirection);
                
                  // also create an actor, transform it, 
                  // add to another collection
                  // then in DTMRI.tcl just do MainAddActor to all of them
                }
              inPtr++;
              inPtr += inIncX;
            }
          inPtr += inIncY;
        }
      inPtr += inIncZ;
    }
}


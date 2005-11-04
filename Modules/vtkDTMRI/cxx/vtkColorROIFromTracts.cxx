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

#include "vtkColorROIFromTracts.h"
#include "vtkActor.h"
#include "vtkProperty.h"
#include "vtkPolyData.h"
#include "vtkPolyDataSource.h"
#include "vtkCell.h"

//------------------------------------------------------------------------------
vtkColorROIFromTracts* vtkColorROIFromTracts::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkColorROIFromTracts");
  if(ret)
    {
      return (vtkColorROIFromTracts*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkColorROIFromTracts;
}

//----------------------------------------------------------------------------
vtkColorROIFromTracts::vtkColorROIFromTracts()
{

  this->InputROIForColoring = NULL;
  this->OutputROIForColoring = NULL;
  this->Streamlines = NULL;
  this->Actors = NULL;
  this->WorldToTensorScaledIJK = NULL;
  this->ROIToWorld = NULL;
}

//----------------------------------------------------------------------------
vtkColorROIFromTracts::~vtkColorROIFromTracts()
{


}




// Color in volume with color ID of streamline passing through it.
// Note: currently does not handle multiple streamlines per voxel
// (chooses last to pass through).
// Note: currently IDs are assigned in order of colors on streamline list.
// This should be changed to use internal color IDs when we have those.
//----------------------------------------------------------------------------
void vtkColorROIFromTracts::ColorROIFromStreamlines()
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
  vtkPolyDataSource *currStreamline = 
    dynamic_cast<vtkPolyDataSource *> (this->Streamlines->GetNextItemAsObject());
  
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
      vtkPoints * hs0=currStreamline->GetOutput()->GetCell(0)->GetPoints();
      vtkPoints * hs1=currStreamline->GetOutput()->GetCell(1)->GetPoints();

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
      currStreamline = dynamic_cast<vtkPolyDataSource *> 
        (this->Streamlines->GetNextItemAsObject());
      currActor = (vtkActor *) this->Actors->GetNextItemAsObject();
    }
}




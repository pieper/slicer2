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

#include "vtkROISelectTracts.h"
#include "vtkStreamlineConvolve.h"
#include "vtkPruneStreamline.h"
#include "vtkCellArray.h"
#include "vtkActor.h"
#include "vtkProperty.h"

#include "vtkHyperStreamlinePoints.h"


//------------------------------------------------------------------------------
vtkROISelectTracts* vtkROISelectTracts::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkROISelectTracts");
  if(ret)
    {
      return (vtkROISelectTracts*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkROISelectTracts;
}

//----------------------------------------------------------------------------
vtkROISelectTracts::vtkROISelectTracts()
{
  // matrices
  // Initialize these to identity, so if the user doesn't set them it's okay.
  this->ROIToWorld = vtkTransform::New();
  this->ROI2ToWorld = vtkTransform::New();
  this->WorldToTensorScaledIJK = vtkTransform::New();
  
  // The user may need to set these, depending on class usage
  this->InputROI = NULL;
  this->InputROIValue = -1;
  this->InputMultipleROIValues = NULL;
  this->InputROI2 = NULL;

  // collections
  this->Streamlines = NULL;
  this->Actors = NULL;

  // for fibers selecting fibers that pass through a ROI
  this->StreamlinesAsPolyLines = vtkPolyData::New();
  this->StreamlineIdPassTest = vtkIntArray::New();

}

//----------------------------------------------------------------------------
vtkROISelectTracts::~vtkROISelectTracts()
{
  // matrices
  this->ROIToWorld->Delete();
  this->ROI2ToWorld->Delete();
  this->WorldToTensorScaledIJK->Delete();
  
  // volumes
  if (this->InputROI) this->InputROI->Delete();
  if (this->InputROI2) this->InputROI->Delete();

}

void vtkROISelectTracts::ConvertStreamlinesToPolyLines()
{

  int numStreamlines;
  vtkPoints *newPoints = vtkPoints::New();
  vtkCellArray *newLines = vtkCellArray::New(); 
  vtkPoints *strPoints;
  vtkHyperStreamlinePoints *currStreamline = NULL;
  int npts = 0;
  
  if (this->Streamlines == 0) 
    {
      vtkErrorMacro("You must set the Streamlines before using this class.");
      return;
    }

  numStreamlines = this->Streamlines->GetNumberOfItems();
  
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
    
void vtkROISelectTracts::FindStreamlinesThatPassThroughROI()
{

  if (this->Streamlines == 0) 
    {
      vtkErrorMacro("You must set the Streamlines before using this class.");
      return;
    }

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


void vtkROISelectTracts::HighlightStreamlinesPassTest()
{
 
  vtkIdType strId;

  if (this->Streamlines == 0) 
    {
      vtkErrorMacro("You must set the Streamlines before using this class.");
      return;
    }
  if (this->Actors == 0) 
    {
      vtkErrorMacro("You must set the Actors before using this class.");
      return;
    }

  int numStr = this->StreamlineIdPassTest->GetNumberOfTuples();
  cout<<"Number of Streamlines that pass test: "<<numStr<<endl;
  vtkActor *currActor;
  int idx=0;
  for (int i=0;i<this->Streamlines->GetNumberOfItems();i++) {
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

void vtkROISelectTracts::DeleteStreamlinesNotPassTest()
{
 
  vtkIdType strId;

  if (this->Streamlines == 0) 
    {
      vtkErrorMacro("You must set the Streamlines before using this class.");
      return;
    }

  int numStr = this->StreamlineIdPassTest->GetNumberOfTuples();
  cout<<"Number of Streamlines that pass test: "<<numStr<<endl;

  int idx=0;
  for (int i=0;i<this->Streamlines->GetNumberOfItems();i++) {
    strId = this->StreamlineIdPassTest->GetValue(idx);
    cout<<"Streamline Id: "<<strId<<endl;
    if(strId!=i) {
      //this->DeleteStreamline(i);
      vtkErrorMacro("Can't delete streamline yet in vtkROISelectTracts");
    }
    else {
      idx++;
    }  
     
  }
}
 

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
/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkPruneStreamline.cxx,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkPruneStreamline.h"

#include "vtkAbstractTransform.h"
#include "vtkCellData.h"
#include "vtkFloatArray.h"
#include "vtkLinearTransform.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"

#include "vtkDoubleArray.h"
#include "vtkCellArray.h"
#include "vtkDataArray.h"
#include "vtkMath.h"

#define VTK_MARGIN 0.3

vtkCxxRevisionMacro(vtkPruneStreamline, "$Revision: 1.2 $");
vtkStandardNewMacro(vtkPruneStreamline);

vtkPruneStreamline::vtkPruneStreamline()
{
  this->ROIValues = NULL;
  this->Threshold = 1;
  this->StreamlineIdPassTest = vtkIntArray::New();
}

vtkPruneStreamline::~vtkPruneStreamline()
{
  this->SetROIValues(NULL);
  this->StreamlineIdPassTest->Delete();
}

void vtkPruneStreamline::Execute()
{
  vtkPoints *inPts;
  vtkPoints *newPts;
  vtkIdType numPts, numCells, numStreamlines, numROIs;
  vtkPolyData *input = this->GetInput();
  vtkPolyData *output = this->GetOutput();
  vtkPointData *pd=input->GetPointData(), *outPD=output->GetPointData();
  vtkCellData *cd=input->GetCellData(), *outCD=output->GetCellData();
  vtkCellArray *inLines;
  vtkDataArray *inScalars;

  vtkDebugMacro(<<"Executing pruning of streamlines");

  // Check input
  //
  if ( this->ROIValues == NULL )
    {
    vtkErrorMacro(<<"No ROIs defined!");
    return;
    }
    

  inPts = input->GetPoints();
  inLines = input->GetLines();

  if ( !inPts )
    {
    vtkErrorMacro(<<"No input data");
    return;
    }
  if ( !inLines )
    {
    vtkErrorMacro(<<"No Streamline data");
    return;
    }
  
  numCells = inLines->GetNumberOfCells();
  numPts = inPts->GetNumberOfPoints();
  inScalars = input->GetPointData()->GetScalars();
  
  numROIs = this->ROIValues->GetNumberOfTuples();
  
  numStreamlines = numCells/2;

  cout<<"Number of streamlines to attemp pruning: "<<numStreamlines<<endl;
  cout<<"Number of Cells: "<<numCells<<endl;
    
  vtkIdType npts;
  vtkIdType *ptId;
  
  
  vtkDoubleArray *DinScalars = vtkDoubleArray::New();
  DinScalars->SetNumberOfValues(numPts);
  double pp1[3];
  double pm1[3];
  double dt;
  double vp1;
  double vm1;
  
  this->UpdateProgress (.2);
  //For each streamline, compute Gradient
  inLines->InitTraversal();
  for(int i=0; i<numStreamlines; i++) {
  
    for(int cellId=0; cellId<2; cellId++) {
     //inLines->GetCell(i*2+cellId,npts,ptId);
     inLines->GetNextCell(npts,ptId);
     //cout<<"Num points in cell: "<<npts<<endl;
     
     //Compute Central Finite Difference
     for(int j=1;j<npts-1;j++) {
       vp1=inScalars->GetComponent(ptId[j+1],0);
       vm1=inScalars->GetComponent(ptId[j-1],0);

       inPts->GetPoint(ptId[j+1],pp1);
       inPts->GetPoint(ptId[j-1],pm1);
  
       dt =vtkMath::Distance2BetweenPoints(pp1,pm1);
       
       DinScalars->SetValue(ptId[j],(vp1-vm1)/dt);
       //cout<<"Ds: "<<(vp1-vm1)/dt<<endl;
     }
     // Last point equals to last value
     DinScalars->SetValue(ptId[npts-1],(vp1-vm1)/dt);
     // First point equals to first value
     DinScalars->SetValue(ptId[0],DinScalars->GetValue(ptId[1]));
    
    }
  }
 
 this->UpdateProgress (.4);  

  StreamlineIdPassTest->Initialize();
  StreamlineIdPassTest->Allocate(numStreamlines);
  int *streamlineTest = new int[numROIs]; 
  
  //Data to store result  
  newPts = vtkPoints::New();
  newPts->Allocate(numPts);
  vtkCellArray *newLines = vtkCellArray::New();
  vtkIdType newptId = 0;
  short rval0;
  double val;
  int test;
    
   //For each streamline see the responses that pass the test.
  inLines->InitTraversal();
  for(int sId=0; sId<numStreamlines; sId++) {
  
    //Set array to zero
    for (int i=0; i<numROIs;i++)
      streamlineTest[i] = 0;
 
    
    for(int cellId=0; cellId<2; cellId++) {
     cout<<"Cell ID: "<<cellId<<endl;
     inLines->GetNextCell(npts,ptId);
     cout<<"Num points in cell: "<<npts<<endl;
  
     for(int j=0;j<npts;j++) {
         
    val=inScalars->GetComponent(ptId[j],0);
    //cout<<"Value: "<<val<<endl;
    for(int rId=0;rId<numROIs;rId++) {
      
      rval0 = ROIValues->GetValue(rId);
      if(val>(rval0-VTK_MARGIN) && val<(rval0+VTK_MARGIN)) {
        //We got response for this ROI
        streamlineTest[rId] += 1; 
        break;
      }  
 
        }
    
     } //end j loop throught cell points
   } //end cellId  
 
   test=this->TestForStreamline(streamlineTest, numROIs);
      
   //Copy cell info to output
   if(test ==1) {
     StreamlineIdPassTest->InsertNextValue(sId);
     for(int cellId=0; cellId<2; cellId++) {
       inLines->GetCell(sId*2+cellId,npts,ptId);
       newLines->InsertNextCell(npts);
       for(int j=0;j<npts;j++) {
         newptId=newPts->InsertNextPoint(inPts->GetPoint(ptId[j]));
     newLines->InsertCellPoint(newptId);
       }    
     }
   }  
 
 }// end loop thourgh streamlines.
 
 this->UpdateProgress (.8);
 
 delete streamlineTest; 
 StreamlineIdPassTest->Squeeze();
     
  // Define output    
  output->SetPoints(newPts);
  newPts->Delete();
  output->SetLines(newLines);
  newLines->Delete();
  output->Squeeze();        

  DinScalars->Delete();

}       
       
  
int vtkPruneStreamline::TestForStreamline(int* streamlineTest,int npts)
{

  int i;
  int test;
  test =0;
  
  //cout<<"StreamlineTest: "<<endl;
  for(i=0;i<npts;i++) {
    cout<<streamlineTest[i]<<endl;
  }  
  
  for(i=0;i<npts;i++) {    
    if(streamlineTest[i]>this->Threshold)
      test +=1; 
  }
  cout<<"Test result: "<<test<<endl;
         
  if(test==npts)
    return 1;
  else
    return 0;  
    
}  
  

unsigned long vtkPruneStreamline::GetMTime()
{
  unsigned long mTime=this->MTime.GetMTime();
  unsigned long transMTime;

  if ( this->ROIValues )
    {
    transMTime = this->ROIValues->GetMTime();
    mTime = ( transMTime > mTime ? transMTime : mTime );
    }

  return mTime;
}

void vtkPruneStreamline::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "Array of ROI Values: " << this->ROIValues << "\n";
}

/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkHyperStreamlinePoints.cxx,v $
  Date:      $Date: 2005/12/20 22:56:24 $
  Version:   $Revision: 1.5.8.1 $

=========================================================================auto=*/
#include "vtkHyperStreamlinePoints.h"
#include "vtkObjectFactory.h"
// is there any problem with including a cxx file?
// this is done for class vtkHyperPoint which is defined here
// otherwise we cannot access the points calculated by superclass
//#include "vtkHyperStreamline.cxx"

vtkStandardNewMacro(vtkHyperStreamlinePoints);

//------------------------------------------------------------------------------
vtkHyperStreamlinePoints::vtkHyperStreamlinePoints()
{
  this->HyperStreamline0 = vtkPoints::New();
  this->HyperStreamline1 = vtkPoints::New();
  this->HyperStreamlines[0] = this->HyperStreamline0;
  this->HyperStreamlines[1] = this->HyperStreamline1;
}

//------------------------------------------------------------------------------
vtkHyperStreamlinePoints::~vtkHyperStreamlinePoints()
{

}

//------------------------------------------------------------------------------
void vtkHyperStreamlinePoints::Execute()
{
  vtkHyperPoint *sPtr;
  vtkIdType i, npts;
  int ptId;
  vtkIdType numIntPts;

  vtkDebugMacro(<<"Calling superclass execute");
  
  // default superclass behavior
  //vtkHyperStreamline::Execute();
  vtkHyperStreamlineDTMRI::Execute();

  vtkDebugMacro(<<"Grabbing superclass output points.");

  // just grab points of output to make them available to user
  // more info in BuildTube code in superclass
  //
  // Loop over all hyperstreamlines generating points
  //
  for (ptId=0; ptId < this->NumberOfStreamers; ptId++)
    {
      // if no points give up
      if ( (numIntPts=this->Streamers[ptId].GetNumberOfPoints()) < 1 )
        {
          continue;
        }

      // get first point
      i = 0;
      sPtr=this->Streamers[ptId].GetHyperPoint(i);

      // loop through all points
      for ( npts=0; i < numIntPts && sPtr->CellId >= 0;
            i++, sPtr=this->Streamers[ptId].GetHyperPoint(i) )
        {
          //for (j=0; j<3; j++) // grab point's coordinates
          //{
          //cout << sPtr->X[j] << " ";
          //}
          //cout << endl;

          this->HyperStreamlines[ptId]->InsertPoint(i,sPtr->X);
          npts++;
        } //while
    } //for this hyperstreamline

  vtkDebugMacro(<<"Done Grabbing superclass output points.");
}

//------------------------------------------------------------------------------
void vtkHyperStreamlinePoints::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkHyperStreamline::PrintSelf(os,indent);
}

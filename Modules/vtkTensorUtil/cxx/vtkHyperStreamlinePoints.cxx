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

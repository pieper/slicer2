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
#include "vtkPreciseHyperStreamlinePoints.h"
#include "vtkObjectFactory.h"
// is there any problem with including a cxx file?
// this is done for class vtkPreciseHyperPoint which is defined here
// otherwise we cannot access the points calculated by superclass
//#include "vtkPreciseHyperStreamline.cxx"

//------------------------------------------------------------------------------

#include "vtkPreciseHyperArray.h"
#include "vtkPreciseHyperPoint.h"

/*
class vtkPreciseHyperPoint { //;prevent man page generation
public:
    vtkPreciseHyperPoint(); // method sets up storage
    vtkPreciseHyperPoint &operator=(const vtkPreciseHyperPoint& hp); //for resizing
    
    float   X[3];    // position 
    vtkIdType     CellId;  // cell
    int     SubId; // cell sub id
    float   P[3];    // parametric coords in cell 
    float   W[3];    // eigenvalues (sorted in decreasing value)
    float   *V[3];   // pointers to eigenvectors (also sorted)
    float   V0[3];   // storage for eigenvectors
    float   V1[3];
    float   V2[3];
    float   S;       // scalar value 
    float   D;       // distance travelled so far 
};
//ETX

class vtkPreciseHyperArray { //;prevent man page generation
public:
  vtkPreciseHyperArray();
  ~vtkPreciseHyperArray()
    {
      if (this->Array)
        {
        delete [] this->Array;
        }
    };
  vtkIdType GetNumberOfPoints() {return this->MaxId + 1;};
  vtkPreciseHyperPoint *GetPreciseHyperPoint(vtkIdType i) {return this->Array + i;};
  vtkPreciseHyperPoint *InsertNextPreciseHyperPoint() 
    {
    if ( ++this->MaxId >= this->Size )
      {
      this->Resize(this->MaxId);
      }
    return this->Array + this->MaxId;
    }
  vtkPreciseHyperPoint *Resize(vtkIdType sz); //reallocates data
  void Reset() {this->MaxId = -1;};

  vtkPreciseHyperPoint *Array;  // pointer to data
  vtkIdType MaxId;             // maximum index inserted thus far
  vtkIdType Size;              // allocated size of data
  vtkIdType Extend;            // grow array by this amount
  float Direction;       // integration direction
};
*/

vtkStandardNewMacro(vtkPreciseHyperStreamlinePoints);

//------------------------------------------------------------------------------
vtkPreciseHyperStreamlinePoints::vtkPreciseHyperStreamlinePoints()
{
  this->PreciseHyperStreamline0 = vtkPoints::New();
  this->PreciseHyperStreamline1 = vtkPoints::New();
  this->PreciseHyperStreamlines[0] = this->PreciseHyperStreamline0;
  this->PreciseHyperStreamlines[1] = this->PreciseHyperStreamline1;
}

//------------------------------------------------------------------------------
vtkPreciseHyperStreamlinePoints::~vtkPreciseHyperStreamlinePoints()
{

}

//------------------------------------------------------------------------------
void vtkPreciseHyperStreamlinePoints::Execute()
{
  vtkPreciseHyperPoint *sPtr;
  vtkIdType i, npts;
  int ptId;
  vtkIdType numIntPts;

  vtkDebugMacro(<<"Calling superclass execute");
  
  // default superclass behavior
  vtkPreciseHyperStreamline::Execute();

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
      sPtr=this->Streamers[ptId].GetPreciseHyperPoint(i);

      // loop through all points
      for ( npts=0; i < numIntPts && sPtr->CellId >= 0;
            i++, sPtr=this->Streamers[ptId].GetPreciseHyperPoint(i) )
        {
          //for (j=0; j<3; j++) // grab point's coordinates
          //{
          //cout << sPtr->X[j] << " ";
          //}
          //cout << endl;

          this->PreciseHyperStreamlines[ptId]->InsertPoint(i,sPtr->X);
          npts++;
        } //while
    } //for this hyperstreamline

  vtkDebugMacro(<<"Done Grabbing superclass output points.");
}

//------------------------------------------------------------------------------
void vtkPreciseHyperStreamlinePoints::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkPreciseHyperStreamline::PrintSelf(os,indent);
}

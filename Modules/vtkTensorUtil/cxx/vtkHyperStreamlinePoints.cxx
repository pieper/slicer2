#include "vtkHyperStreamlinePoints.h"
#include "vtkObjectFactory.h"
// is there any problem with including a cxx file?
// this is done for class vtkHyperPoint which is defined here
// otherwise we cannot access the points calculated by superclass
//#include "vtkHyperStreamline.cxx"

//------------------------------------------------------------------------------
vtkHyperStreamlinePoints* vtkHyperStreamlinePoints::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkHyperStreamlinePoints");
  if(ret)
    {
    return (vtkHyperStreamlinePoints*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkHyperStreamlinePoints;
}

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
  vtkHyperPoint *sPrev, *sPtr;
  vtkIdType i, npts;
  int ptId, j, id;
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

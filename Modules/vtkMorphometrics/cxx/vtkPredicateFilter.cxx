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
#include "vtkPredicateFilter.h"
#include <vtkObjectFactory.h>
#include <vtkCellArray.h>
#include <vtkPolyData.h>
#include <iostream>
#include <assert.h>


/*
  Puts every point x of the input which fulfills Predicate->P(x) into the set of
  output points. Calls functions to do the same for the input strips and polys.
 */
void vtkPredicateFilter::Execute()
{
  vtkPolyData *input = (vtkPolyData *)this->Inputs[0];
  vtkPolyData *output = this->GetOutput();
  vtkPoints *outputPoints = vtkPoints::New();

  vtkIdType ccn = 0;
  vtkIdType nn = 0;

  vtkFloatingPointType* x;

  if(input == NULL ) return;


  Predicate->InitP();


  // first insert every point fulfilling P

  outputPoints->SetNumberOfPoints(input->GetPoints()->GetNumberOfPoints());
  
  for(nn = input->GetPoints()->GetNumberOfPoints() -1;nn >= 0;nn--)
    {
      x = input->GetPoint(nn);
      if(Predicate->P(x))
    outputPoints->SetPoint(ccn++,x);
    }

  outputPoints->SetNumberOfPoints(ccn);

  output->SetPoints(outputPoints);

  // then copy those structures still relevant, i.e. only those polygons where every point fulfills P()
  if(input->GetStrips()->GetNumberOfCells()!=0)
    {
      ExecuteUpdateStrips(input,output);
    }

  if(input->GetPolys()->GetNumberOfCells()!=0)
    {
      ExecuteUpdatePolys(input,output);
    }
}

/*
  Since a polygon is the clockwise enumeration of involved points, we simply have to 
  drop those points not fulfilling Predicate->P().
*/
void vtkPredicateFilter:: ExecuteUpdatePolys(vtkPolyData* input,vtkPolyData* output)
{
  vtkCellArray *outputPolys = vtkCellArray::New();
  vtkCellArray *inputPolys = input->GetPolys();
  
  inputPolys->InitTraversal();
  vtkIdType *pts = 0;
  vtkIdType npts = 0;
  vtkIdType insertedPts =0;

  while(inputPolys->GetNextCell(npts,pts))
    {
      outputPolys->InsertNextCell(npts);
      for (int i = 0;  i < npts; i++)
    {
      if(Predicate->P(input->GetPoint(pts[i])))
        {
          inputPolys->InsertCellPoint(pts[i]);
          insertedPts++;
        }
    }
      outputPolys->UpdateCellCount(insertedPts);
    }
  
  output->SetPolys(outputPolys);
}

/*
  Finding the strips of the output is achieved by:
  - first splitting each strip of the input at each occurrence of one or more points 
    not fulfilling Predicate P
  and then
  - second dropping all strips of size 2 or smaller, since they don't represent polygons
    any more.
*/
void vtkPredicateFilter:: ExecuteUpdateStrips(vtkPolyData* input,vtkPolyData* output)
{
  vtkCellArray *outputStrips = vtkCellArray::New();
  vtkCellArray *outputStrips2 = vtkCellArray::New();
  vtkCellArray *inputStrips = input->GetStrips();
  vtkFloatingPointType *x;
  vtkIdType *pts = 0;
  vtkIdType npts = 0;
  int nr_inserted=0;
  int i;

  inputStrips->InitTraversal();
  while(inputStrips->GetNextCell(npts,pts))
    {
      outputStrips2->InsertNextCell(npts);
      nr_inserted=0;
      for (i = 0;  i < npts; i++)
    {
      x = input->GetPoint(pts[i]);
      if(Predicate->P(x))
        {
          int nrnr = output->FindPoint(x[0],x[1],x[2]);
          
          outputStrips2->InsertCellPoint(nrnr);
          nr_inserted++;
        }
      else 
        {
          if(nr_inserted != 0)
        {
          outputStrips2->UpdateCellCount(nr_inserted);
          outputStrips2->InsertNextCell(npts);

          nr_inserted = 0;
        }
        }
    }
      outputStrips2->UpdateCellCount(nr_inserted);
    }
  
  // in a second pass we have to eliminate all cells of size <=2;
  outputStrips2->InitTraversal();
  while(outputStrips2->GetNextCell(npts,pts))
    {
      if(npts>=3)
    {
      outputStrips->InsertNextCell(npts,pts);
    }
    }

  outputStrips2->Delete();
  output->SetStrips(outputStrips);
}

vtkPredicateFilter* vtkPredicateFilter::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkPredicateFilter")
;
  if(ret)
    {
    return (vtkPredicateFilter*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkPredicateFilter;
}

void vtkPredicateFilter::Delete()
{
  delete this;

}
void vtkPredicateFilter::PrintSelf()
{

}

vtkPredicateFilter::vtkPredicateFilter()
{
  Predicate = NULL;
}

vtkPredicateFilter::~vtkPredicateFilter()
{
  if(Predicate!=NULL)
    Predicate->Delete();
}

vtkPredicateFilter::vtkPredicateFilter(vtkPredicateFilter&)
{

}

void vtkPredicateFilter::operator=(const vtkPredicateFilter)
{

}

unsigned long vtkPredicateFilter::GetMTime()
{
  unsigned long mTime=this->vtkPolyDataToPolyDataFilter::GetMTime();
  unsigned long OtherMTime;

  if ( this->Predicate != NULL )
    {
    OtherMTime = this->Predicate->GetMTime();
    mTime = ( OtherMTime > mTime ? OtherMTime : mTime );
    }
  return mTime;
}

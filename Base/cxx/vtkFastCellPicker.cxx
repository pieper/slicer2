/*=auto=========================================================================

(c) Copyright 2001 Massachusetts Institute of Technology

Permission is hereby granted, without payment, to copy, modify, display 
and distribute this software and its documentation, if any, for any purpose, 
provided that the above copyright notice and the following three paragraphs 
appear on all copies of this software.  Use of this software constitutes 
acceptance of these terms and conditions.

IN NO EVENT SHALL MIT BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, 
INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE 
AND ITS DOCUMENTATION, EVEN IF MIT HAS BEEN ADVISED OF THE POSSIBILITY OF 
SUCH DAMAGE.

MIT SPECIFICALLY DISCLAIMS ANY EXPRESS OR IMPLIED WARRANTIES INCLUDING, 
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR 
A PARTICULAR PURPOSE, AND NON-INFRINGEMENT.

THE SOFTWARE IS PROVIDED "AS IS."  MIT HAS NO OBLIGATION TO PROVIDE 
MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

=========================================================================auto=*/
/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkFastCellPicker.cxx,v $
  Language:  C++
  Date:      $Date: 2001/02/19 17:53:39 $
  Version:   $Revision: 1.5 $


Copyright (c) 1993-1998 Ken Martin, Will Schroeder, Bill Lorensen.

This software is copyrighted by Ken Martin, Will Schroeder and Bill Lorensen.
The following terms apply to all files associated with the software unless
explicitly disclaimed in individual files. This copyright specifically does
not apply to the related textbook "The Visualization Toolkit" ISBN
013199837-4 published by Prentice Hall which is covered by its own copyright.

The authors hereby grant permission to use, copy, and distribute this
software and its documentation for any purpose, provided that existing
copyright notices are retained in all copies and that this notice is included
verbatim in any distributions. Additionally, the authors grant permission to
modify this software and its documentation for any purpose, provided that
such modifications are not distributed without the explicit consent of the
authors and that existing copyright notices are retained in all copies. Some
of the algorithms implemented by this software are patented, observe all
applicable patent law.

IN NO EVENT SHALL THE AUTHORS OR DISTRIBUTORS BE LIABLE TO ANY PARTY FOR
DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
OF THE USE OF THIS SOFTWARE, ITS DOCUMENTATION, OR ANY DERIVATIVES THEREOF,
EVEN IF THE AUTHORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

THE AUTHORS AND DISTRIBUTORS SPECIFICALLY DISCLAIM ANY WARRANTIES, INCLUDING,
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
PARTICULAR PURPOSE, AND NON-INFRINGEMENT.  THIS SOFTWARE IS PROVIDED ON AN
"AS IS" BASIS, AND THE AUTHORS AND DISTRIBUTORS HAVE NO OBLIGATION TO PROVIDE
MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.


=========================================================================*/
#include "vtkFastCellPicker.h"

vtkFastCellPicker::vtkFastCellPicker()
{
  this->CellId = -1;
  this->SubId = -1;
  for (int i=0; i<3; i++)
    {
    this->PCoords[i] = 0.0;
    }
  this->OBBTrees = vtkCollection::New();
}

#if (VTK_MAJOR_VERSION == 3 && VTK_MINOR_VERSION == 2)
float vtkFastCellPicker::IntersectWithLine(float p1[3], float p2[3], float tol, 
				      vtkAssemblyPath *assem, vtkActor *a, 
				      vtkMapper *m)
#else
float vtkFastCellPicker::IntersectWithLine(float p1[3], float p2[3], float tol, 
				      vtkActor *assem, vtkActor *a, 
				      vtkMapper *m)
#endif
{
  int numCells, ii, nOBBTrees;
  int i, minCellId, minSubId;
  float x[3], t, pcoords[3];
  vtkCell *cell;
  vtkDataSet *input=m->GetInput();
  vtkOBBTree *pOBBTree, *thisOBBTree;

  if ( (numCells = input->GetNumberOfCells()) < 1 )
    {
    return 2.0;
    }

  nOBBTrees = this->OBBTrees->GetNumberOfItems();
  pOBBTree = NULL;
  for ( ii=0; ii<nOBBTrees && pOBBTree == NULL; ii++ )
    {
    thisOBBTree = (vtkOBBTree *)this->OBBTrees->GetItemAsObject( ii );
    if ( thisOBBTree->GetDataSet() == input )
      {
      pOBBTree = thisOBBTree;
      vtkDebugMacro("Using OBBTree:" << pOBBTree);
      }
    }
  if ( pOBBTree == NULL )
    { // Initialize an OBBTree for this dataset
    pOBBTree = vtkOBBTree::New();
    pOBBTree->SetDataSet( input );
    pOBBTree->SetDebug( 0 );
    this->OBBTrees->AddItem( (vtkObject *)pOBBTree );
    vtkDebugMacro("Making OBBTree:" << pOBBTree);
    vtkDebugMacro("Number of OBBTrees now is:" <<
                  this->OBBTrees->GetNumberOfItems());
    }
  pOBBTree->Update();
  //
  //  Intersect each cell with ray.  Keep track of one closest to 
  //  the eye (and within the clipping range).
  //
  minCellId = -1;
  minSubId = -1;
  if ( pOBBTree->IntersectWithLine( &p1[0], &p2[0], tol, t, &x[0], &pcoords[0],
                                    minSubId, minCellId ) )
    {
    //
    //  Now compare this against other actors.
    //
    if ( minCellId>(-1) && t < this->GlobalTMin ) 
      {
      this->MarkPicked(assem, a, m, t, x);
      this->CellId = minCellId;
      this->SubId = minSubId;
      for (i=0; i<3; i++)
        {
        this->PCoords[i] = pcoords[i];
        }
      vtkDebugMacro("Picked cell id= " << minCellId);
      }
    }
  else
    {
    t = 2.0;
    }
  return t;
}

void vtkFastCellPicker::Initialize()
{
  this->CellId = (-1);
  this->SubId = (-1);
  for (int i=0; i<3; i++)
    {
    this->PCoords[i] = 0.0;
    }
  this->vtkPicker::Initialize();
}

void vtkFastCellPicker::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkPicker::PrintSelf(os,indent);

  os << indent << "Cell Id: " << this->CellId << "\n";
  os << indent << "SubId: " << this->SubId << "\n";
  os << indent << "PCoords: (" << this->PCoords[0] << ", " 
     << this->PCoords[1] << ", " << this->PCoords[2] << ")\n";
}

/*=auto=========================================================================
Copyright (c) 2000 Surgical Planning Lab, Brigham and Women's Hospital
 
Direct all questions on this copyright to slicer@ai.mit.edu.
The following terms apply to all files associated with the software unless
explicitly disclaimed in individual files.   

The authors hereby grant permission to use, copy (but NOT distribute) this
software and its documentation for any NON-COMMERCIAL purpose, provided
that existing copyright notices are retained verbatim in all copies.
The authors grant permission to modify this software and its documentation 
for any NON-COMMERCIAL purpose, provided that such modifications are not 
distributed without the explicit consent of the authors and that existing
copyright notices are retained in all copies. Some of the algorithms
implemented by this software are patented, observe all applicable patent law.

IN NO EVENT SHALL THE AUTHORS OR DISTRIBUTORS BE LIABLE TO ANY PARTY FOR
DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
OF THE USE OF THIS SOFTWARE, ITS DOCUMENTATION, OR ANY DERIVATIVES THEREOF,
EVEN IF THE AUTHORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

THE AUTHORS AND DISTRIBUTORS SPECIFICALLY DISCLAIM ANY WARRANTIES, INCLUDING,
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
PARTICULAR PURPOSE, AND NON-INFRINGEMENT.  THIS SOFTWARE IS PROVIDED ON AN
'AS IS' BASIS, AND THE AUTHORS AND DISTRIBUTORS HAVE NO OBLIGATION TO PROVIDE
MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
=========================================================================auto=*/
#include <math.h>
#include "vtkMath.h"
#include "vtkSurfaceProps.h"

// Description:
vtkSurfaceProps::vtkSurfaceProps()
  {
  }

// Description:
// Specifies the input data...
void vtkSurfaceProps::SetInput(vtkPolyData *input)
{
  this->vtkProcessObject::SetNthInput(0, input);
}

//----------------------------------------------------------------------------
vtkPolyData *vtkSurfaceProps::GetInput()
{
  if (this->NumberOfInputs < 1)
    {
    return NULL;
    }
  
  return (vtkPolyData *)(this->Inputs[0]);
}

//
// Compute Surface Properties for Input vtkPolyData
//
void vtkSurfaceProps::Execute()
  {
  vtkPolyData *input = (vtkPolyData *)this->Inputs[0];
  int *ptIds;
  int numPts, numCells, jj, kk, cellId, type, pId, qId, rId;
  float *p, *q, *r, xp[3], edge0[3], edge1[3], edge2[3], *center;
  double tri_area, cell_area, tot_area, tri_vol, tot_vol, tot_vol2;

  vtkDebugMacro(<< "Calculating Surface Properties." );

  this->SurfaceArea = 0.0;
  this->MinCellArea = VTK_LARGE_FLOAT;
  this->MaxCellArea = 0.0;
  this->Volume = 0.0;
  this->VolumeError = 0.0;

  tot_area = tot_vol = tot_vol2 = 0.0;

  numPts = input->GetNumberOfPoints();
  numCells = input->GetNumberOfCells();
  if ( this->Inputs[0] == NULL || numPts < 1 || numCells < 1 )
    {
    vtkErrorMacro(<<"Can't compute surface properties - no data available!");
    return;
    }

  center = input->GetCenter();
  //
  // Loop on all polygonal cells
  //
  for ( cellId=0; cellId < numCells; cellId++ )
    {
    type = input->GetCellType( cellId );
    input->GetCellPoints( cellId, numPts, ptIds );
    cell_area = 0.0;
    for ( jj=0; jj<numPts-2; jj++ )
      {
      CELLTRIANGLES( ptIds, type, jj, pId, qId, rId );
      if ( pId < 0 )
        continue;
      p = input->GetPoint( pId );
      q = input->GetPoint( qId );
      r = input->GetPoint( rId );
      // p, q, and r are the oriented triangle points.
      for ( kk=0; kk<3; kk++ )
        {
        // two triangle edge vectors
        edge0[kk] = q[kk] - p[kk];
        edge1[kk] = r[kk] - p[kk];
        }
      vtkMath::Cross( edge0, edge1, xp );
      tri_area = 0.5*vtkMath::Norm( xp );
      cell_area += tri_area;

      tri_vol = vtkMath::Dot( p, xp )/6.0;
      tot_vol2 += tri_vol;
      for ( kk=0; kk<3; kk++ )
        {
        // three tetrahedron edge vectors
        edge0[kk] = p[kk] - center[kk];
        edge1[kk] = q[kk] - center[kk];
        edge2[kk] = r[kk] - center[kk];
        }
      vtkMath::Cross( edge0, edge1, xp );
      tri_vol = vtkMath::Dot( xp, edge2 )/6.0;
      tot_vol += tri_vol;
      } // end foreach triangle

    tot_area += cell_area;
    if ( cell_area > this->MaxCellArea )
      this->MaxCellArea = cell_area;
    if ( cell_area < this->MinCellArea )
      this->MinCellArea = cell_area;
    } // end foreach cell
  this->SurfaceArea = tot_area;
  this->Volume = tot_vol;
  if ( tot_vol > tot_vol2 )
    this->VolumeError = tot_vol - tot_vol2;
  else
    this->VolumeError = tot_vol2 - tot_vol;
  }

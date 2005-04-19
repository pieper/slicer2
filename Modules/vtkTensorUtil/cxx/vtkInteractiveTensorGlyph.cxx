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
#include "vtkInteractiveTensorGlyph.h"
#include "vtkTransform.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkTensor.h"
#include "vtkFloatArray.h"
#include "vtkPolyData.h"
#include "vtkPointData.h"
#include "vtkCellArray.h"
#include <time.h>

//------------------------------------------------------------------------------
vtkInteractiveTensorGlyph* vtkInteractiveTensorGlyph::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkInteractiveTensorGlyph");
  if(ret)
    {
    return (vtkInteractiveTensorGlyph*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkInteractiveTensorGlyph;
}




// Construct object with defaults from superclass: these are
// scaling on and scale factor 1.0. Eigenvalues are 
// extracted, glyphs are colored with input scalar data, and logarithmic
// scaling is turned off.
vtkInteractiveTensorGlyph::vtkInteractiveTensorGlyph()
{
  // Instead of coloring glyphs by passing through input
  // scalars, color according to features we are computing.
  this->ColorGlyphsWithLinearMeasure();

  this->VolumePositionMatrix = NULL;
  this->TensorRotationMatrix = NULL;

  this->MaskGlyphsWithScalars = 0;
  this->ScalarMask = NULL;
  this->Resolution = 1;
}

vtkInteractiveTensorGlyph::~vtkInteractiveTensorGlyph()
{

}

void vtkInteractiveTensorGlyph::ColorGlyphsWithLinearMeasure() {
  this->ColorGlyphsWith(VTK_LINEAR_MEASURE);
}
void vtkInteractiveTensorGlyph::ColorGlyphsWithSphericalMeasure() {
  this->ColorGlyphsWith(VTK_SPHERICAL_MEASURE);
}
void vtkInteractiveTensorGlyph::ColorGlyphsWithPlanarMeasure() {
  this->ColorGlyphsWith(VTK_PLANAR_MEASURE);
}
void vtkInteractiveTensorGlyph::ColorGlyphsWithMaxEigenvalue() {
  this->ColorGlyphsWith(VTK_MAX_EIGENVAL_MEASURE);
}
void vtkInteractiveTensorGlyph::ColorGlyphsWithMiddleEigenvalue() {
  this->ColorGlyphsWith(VTK_MIDDLE_EIGENVAL_MEASURE);
}
void vtkInteractiveTensorGlyph::ColorGlyphsWithMinEigenvalue() {
  this->ColorGlyphsWith(VTK_MIN_EIGENVAL_MEASURE);
}
void vtkInteractiveTensorGlyph::ColorGlyphsWithMaxMinusMidEigenvalue() {
  this->ColorGlyphsWith(VTK_EIGENVAL_DIFFERENCE_MAX_MID_MEASURE);
}

void vtkInteractiveTensorGlyph::ColorGlyphsWithDirection() {
  this->ColorGlyphsWith(VTK_DIRECTION_MEASURE);
}
void vtkInteractiveTensorGlyph::ColorGlyphsWithRelativeAnisotropy() {
  this->ColorGlyphsWith(VTK_RELATIVE_ANISOTROPY_MEASURE);
}
void vtkInteractiveTensorGlyph::ColorGlyphsWithFractionalAnisotropy() {
  this->ColorGlyphsWith(VTK_FRACTIONAL_ANISOTROPY_MEASURE);
}

void vtkInteractiveTensorGlyph::ColorGlyphsWith(int measure) {
  if (this->ScalarMeasure != measure) 
    {
      this->ColorGlyphs = 0;
      this->ColorGlyphsWithAnisotropy = 1;
      this->ScalarMeasure = measure;
      this->Modified();
    }
}

// Lauren we need to check that the scalarmask has the same
// extent!

void vtkInteractiveTensorGlyph::Execute()
{
  vtkDataArray *inTensors;
  vtkFloatingPointType tensor[3][3];
  vtkDataArray *inScalars;
  int numPts, numSourcePts, numSourceCells;
  int inPtId, i, j;
  vtkPoints *sourcePts;
  vtkDataArray *sourceNormals;
  vtkCellArray *sourceCells, *cells;  
  vtkPoints *newPts;
  vtkFloatArray *newScalars=NULL;
  vtkFloatArray *newNormals=NULL;
  vtkFloatingPointType *x, s;
  vtkTransform *trans = vtkTransform::New();
  vtkCell *cell;
  vtkIdList *cellPts;
  int npts;
  vtkIdType *pts;
  int cellId;
  int ptOffset=0;
  vtkMatrix4x4 *matrix = vtkMatrix4x4::New();
  vtkFloatingPointType *m[3], w[3], *v[3];
  vtkFloatingPointType m0[3], m1[3], m2[3];
  vtkFloatingPointType v0[3], v1[3], v2[3];
  vtkFloatingPointType xv[3], yv[3], zv[3];
  vtkFloatingPointType maxScale;
  vtkPointData *pd, *outPD;
  vtkDataSet *input = this->GetInput();
  vtkPolyData *output = this->GetOutput();

  if (this->GetSource() == NULL)
    {
    vtkDebugMacro("No source.");
    return;
    }

  pts = new vtkIdType[this->GetSource()->GetMaxCellSize()];

  vtkDataArray *inMask;
  int doMasking;
  // time
  clock_t tStart=0;
  tStart = clock();


  // set up working matrices
  m[0] = m0; m[1] = m1; m[2] = m2; 
  v[0] = v0; v[1] = v1; v[2] = v2; 

  vtkDebugMacro(<<"Generating tensor glyphs");

  pd = input->GetPointData();
  outPD = output->GetPointData();
  inTensors = pd->GetTensors();
  inScalars = pd->GetScalars();
  numPts = input->GetNumberOfPoints();
  inMask = NULL;
  if (this->ScalarMask)
    {
      inMask = this->ScalarMask->GetPointData()->GetScalars();
    }

  if ( !inTensors || numPts < 1 )
    {
      vtkErrorMacro(<<"No data to glyph!");
      return;
    }
  //
  // Allocate storage for output PolyData
  //
  sourcePts = this->GetSource()->GetPoints();
  numSourcePts = sourcePts->GetNumberOfPoints();
  numSourceCells = this->GetSource()->GetNumberOfCells();

  newPts = vtkPoints::New();
  newPts->Allocate(numPts*numSourcePts);

  // Setting up for calls to PolyData::InsertNextCell()
  if ( (sourceCells=this->GetSource()->GetVerts())->GetNumberOfCells() > 0 )
    {
      cells = vtkCellArray::New();
      cells->Allocate(numPts*sourceCells->GetSize());
      output->SetVerts(cells);
      cells->Delete();
    }
  if ( (sourceCells=this->GetSource()->GetLines())->GetNumberOfCells() > 0 )
    {
      cells = vtkCellArray::New();
      cells->Allocate(numPts*sourceCells->GetSize());
      output->SetLines(cells);
      cells->Delete();
    }
  if ( (sourceCells=this->GetSource()->GetPolys())->GetNumberOfCells() > 0 )
    {
      cells = vtkCellArray::New();
      cells->Allocate(numPts*sourceCells->GetSize());
      output->SetPolys(cells);
      cells->Delete();
    }
  if ( (sourceCells=this->GetSource()->GetStrips())->GetNumberOfCells() > 0 )
    {
      cells = vtkCellArray::New();
      cells->Allocate(numPts*sourceCells->GetSize());
      output->SetStrips(cells);
      cells->Delete();
    }

  // copy point data through or create it here
  pd = this->GetSource()->GetPointData();

  // always output scalars
  newScalars = vtkFloatArray::New();
  newScalars->Allocate(numPts*numSourcePts);
  
  if ( (sourceNormals = pd->GetNormals()) )
    {
      newNormals = vtkFloatArray::New();
      // vtk4.0, copied from tensor glyph filter
      newNormals->SetNumberOfComponents(3);
      newNormals->Allocate(3*numPts*numSourcePts);
      //newNormals->Allocate(numPts*numSourcePts);
    }

  // Figure out whether we are using a mask (if the user has
  // asked us to mask and also has set the mask input).
  doMasking = 0;
  //if (inMask && this->MaskGlyphsWithScalars)
  //doMasking = 1;
  if (this->MaskGlyphsWithScalars)
    {
      if (inMask)
    {
      doMasking = 1;
    }
      else 
    {
      vtkErrorMacro("User has not set input mask, but has requested MaskGlyphsWithScalars");
    }
    }

  // figure out if we are transforming output point locations
  vtkTransform *userVolumeTransform = vtkTransform::New();
  if (this->VolumePositionMatrix)
    {
      userVolumeTransform->SetMatrix(this->VolumePositionMatrix);
      userVolumeTransform->PreMultiply();
    }

  //
  // Traverse all Input points, transforming glyph at Source points
  //
  trans->PreMultiply();

  //cout << "glyph time before pt traversal: " << clock() - tStart << endl;

  for (inPtId=0; inPtId < numPts; inPtId=inPtId+this->Resolution)
    {
      
      if ( ! (inPtId % 10000) ) 
    {
      this->UpdateProgress ((vtkFloatingPointType)inPtId/numPts);
      if (this->GetAbortExecute())
        {
          break;
        }
    }

      //ptIncr = inPtId * numSourcePts;

      //tensor = inTensors->GetTuple(inPtId);
      inTensors->GetTuple(inPtId,(vtkFloatingPointType *)tensor);

      trans->Identity();

      // threshold: if trace is <= 0, don't do expensive computations
      // This used to be: tensor ->GetComponent(0,0) + 
      // tensor->GetComponent(1,1) + tensor->GetComponent(2,2);
      vtkFloatingPointType trace = tensor[0][0] + tensor[1][1] + tensor[2][2];

      
      // only display this glyph if either:
      // a) we are masking and the mask is 1 at this location.
      // b) the trace is positive and we are not masking (default).
      // (If the trace is 0 we don't need to go through the code just to
      // display nothing at the end, since we expect that our data has
      // non-negative eigenvalues.)
      if ((doMasking && inMask->GetTuple1(inPtId)) || (!this->MaskGlyphsWithScalars && trace > 0)) 
    {
      // copy topology
      for (cellId=0; cellId < numSourceCells; cellId++)
        {
          cell = this->GetSource()->GetCell(cellId);
          cellPts = cell->GetPointIds();
          npts = cellPts->GetNumberOfIds();
          for (i=0; i < npts; i++)
        {
          //pts[i] = cellPts->GetId(i) + ptIncr;
          pts[i] = cellPts->GetId(i) + ptOffset;
        }
          output->InsertNextCell(cell->GetCellType(),npts,pts);
        }

      // translate Source to Input point
      x = input->GetPoint(inPtId);
      // If we have a user-specified matrix determining the points
      vtkFloatingPointType x2[3];
      if (this->VolumePositionMatrix)
        {
          userVolumeTransform->TransformPoint(x,x2);
          // point x to x2 now
          x = x2;
        }  
      trans->Translate(x[0], x[1], x[2]);

      // compute orientation vectors and scale factors from tensor
      if ( this->ExtractEigenvalues ) // extract appropriate eigenfunctions
        {
          for (j=0; j<3; j++)
        {
          for (i=0; i<3; i++)
            {
              // transpose
              //m[i][j] = tensor[i+3*j];
              m[i][j] = tensor[j][i];
            }
        }
          vtkMath::Jacobi(m, w, v);

          //copy eigenvectors
          xv[0] = v[0][0]; xv[1] = v[1][0]; xv[2] = v[2][0];
          yv[0] = v[0][1]; yv[1] = v[1][1]; yv[2] = v[2][1];
          zv[0] = v[0][2]; zv[1] = v[1][2]; zv[2] = v[2][2];
        }
      else //use tensor columns as eigenvectors
        {
          for (i=0; i<3; i++)
        {
          //xv[i] = tensor[i];
          //yv[i] = tensor[i+3];
          //zv[i] = tensor[i+6];
          xv[i] = tensor[0][i];
          yv[i] = tensor[1][i];
          zv[i] = tensor[2][i];
        }
          w[0] = vtkMath::Normalize(xv);
          w[1] = vtkMath::Normalize(yv);
          w[2] = vtkMath::Normalize(zv);
        }

      // output scalars before modifying the value of 
      // the eigenvalues (scaling, etc)
      if ( inScalars && this->ColorGlyphs ) 
        {
          // Copy point data from source
          s = inScalars->GetTuple1(inPtId);
        }
      else 
        {
          // create scalar data from computed features
          vtkFloatingPointType trace = w[0]+w[1]+w[2];

          // avoid division by 0
          vtkFloatingPointType eps = 1;
          if (trace == 0) 
        trace = eps;
          vtkFloatingPointType norm;

          // regularization to compensate for small eigenvalues
          vtkFloatingPointType r = 0.001;
          trace += r;

          switch (this->ScalarMeasure) 
        {
        case VTK_LINEAR_MEASURE:
          s = (w[0] - w[1])/trace;
          break;
        case VTK_PLANAR_MEASURE:
          s = 2*(w[1] - w[2])/trace;
          break;
        case VTK_SPHERICAL_MEASURE:
          s = 3*w[2]/trace;
          break;
        case VTK_MAX_EIGENVAL_MEASURE:
          s = w[0];
          break;
        case VTK_MIDDLE_EIGENVAL_MEASURE:
          s = w[1];
          break;
        case VTK_MIN_EIGENVAL_MEASURE:
          s = w[2]; 
          break;
        case VTK_EIGENVAL_DIFFERENCE_MAX_MID_MEASURE:
          s = w[0] - w[2]; 
          break;
        case VTK_DIRECTION_MEASURE:
          // vary color only with x and y, since unit vector
          // these two determine z component.
          // use max evector for direction
          s = fabs(xv[0])/(fabs(yv[0]) + eps);
          break;
        case VTK_RELATIVE_ANISOTROPY_MEASURE:
          // 1/sqrt(2) is the constant used here
          s = (0.70710678)*(sqrt((w[0]-w[1])*(w[0]-w[1]) + 
                     (w[2]-w[1])*(w[2]-w[1]) +
                     (w[2]-w[0])*(w[2]-w[0])))/trace;
          break;
        case VTK_FRACTIONAL_ANISOTROPY_MEASURE:
          norm = sqrt(w[0]*w[0]+ w[1]*w[1] +  w[2]*w[2]);
          if (norm == 0) 
            norm = eps;
          s = (0.70710678)*(sqrt((w[0]-w[1])*(w[0]-w[1]) + 
                     (w[2]-w[1])*(w[2]-w[1]) +
                     (w[2]-w[0])*(w[2]-w[0])))/norm;

          break;
        default:
          s = 0;
          break;
        }
        }          

      for (i=0; i < numSourcePts; i++) 
        {
          //newScalars->InsertScalar(ptIncr+i, s);
          newScalars->InsertNextTuple1(s);
        }        

      // compute scale factors
      w[0] *= this->ScaleFactor;
      w[1] *= this->ScaleFactor;
      w[2] *= this->ScaleFactor;
    
      if ( this->ClampScaling )
        {
          for (maxScale=0.0, i=0; i<3; i++)
        {
          if ( maxScale < fabs(w[i]) )
            {
              maxScale = fabs(w[i]);
            }
        }
          if ( maxScale > this->MaxScaleFactor )
        {
          maxScale = this->MaxScaleFactor / maxScale;
          for (i=0; i<3; i++)
            {
              w[i] *= maxScale; //preserve overall shape of glyph
            }
        }
        }

      // If we have a user-specified matrix rotating the tensor
       if (this->TensorRotationMatrix)
         {
           trans->Concatenate(this->TensorRotationMatrix);
         }


      // normalized eigenvectors rotate object
      // odonnell: test -y for display 
      int yFlipFlag = 1;
      matrix->Element[0][0] = xv[0];
      matrix->Element[0][1] = yFlipFlag*yv[0];
      matrix->Element[0][2] = zv[0];
      matrix->Element[1][0] = xv[1];
      matrix->Element[1][1] = yFlipFlag*yv[1];
      matrix->Element[1][2] = zv[1];
      matrix->Element[2][0] = xv[2];
      matrix->Element[2][1] = yFlipFlag*yv[2];
      matrix->Element[2][2] = zv[2];
      trans->Concatenate(matrix);

      // make sure scale is okay (non-zero) and scale data
      for (maxScale=0.0, i=0; i<3; i++)
        {
          if ( w[i] > maxScale )
        {
          maxScale = w[i];
        }
        }
      if ( maxScale == 0.0 )
        {
          maxScale = 1.0;
        }
      for (i=0; i<3; i++)
        {
          if ( w[i] == 0.0 )
        {
          w[i] = maxScale * 1.0e-06;
        }
        }
      trans->Scale(w[0], w[1], w[2]);

      // multiply points (and normals if available) by resulting matrix
      // this also appends them to the output "new" data
      trans->TransformPoints(sourcePts,newPts);
      if ( newNormals )
        {
          trans->TransformNormals(sourceNormals,newNormals);
        }

      ptOffset += numSourcePts;

    }  // end if mask is 1 OR trace is ok
    }

  vtkDebugMacro(<<"Generated " << numPts <<" tensor glyphs");
  //
  // Update output and release memory
  //
  delete [] pts;

  output->SetPoints(newPts);
  newPts->Delete();

  if ( newScalars )
    {
      outPD->SetScalars(newScalars);
      newScalars->Delete();
    }

  if ( newNormals )
    {
      outPD->SetNormals(newNormals);
      newNormals->Delete();
    }

  // reclaim extra memory we allocated
  output->Squeeze();

  trans->Delete();
  matrix->Delete();

  cout << "glyph time: " << clock() - tStart << endl;
}

void vtkInteractiveTensorGlyph::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkTensorGlyph::PrintSelf(os,indent);

  //  os << indent << "ColorGlyphsWithAnisotropy: " << this->ColorGlyphsWithAnisotropy << "\n";
}

//----------------------------------------------------------------------------
// Account for the MTime of objects we use
//
unsigned long int vtkInteractiveTensorGlyph::GetMTime()
{
  unsigned long mTime=this->vtkObject::GetMTime();
  unsigned long time;

  if ( this->ScalarMask != NULL )
    {
      time = this->ScalarMask->GetMTime();
      mTime = ( time > mTime ? time : mTime );
    }

  if ( this->VolumePositionMatrix != NULL )
    {
      time = this->VolumePositionMatrix->GetMTime();
      mTime = ( time > mTime ? time : mTime );
    }

  if ( this->TensorRotationMatrix != NULL )
    {
      time = this->TensorRotationMatrix->GetMTime();
      mTime = ( time > mTime ? time : mTime );
    }

  return mTime;
}

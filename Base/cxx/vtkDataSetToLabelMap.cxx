/*=auto=========================================================================

(c) Copyright 2003 Massachusetts Institute of Technology (MIT) All Rights Reserved.

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
//#include <math.h>
//#include <stdio.h>
#include "vtkPointData.h"
#include "vtkPoints.h"
#include "vtkCell.h"
#include "vtkDataSetToLabelMap.h"
#include "vtkIntArray.h"
#include "vtkObjectFactory.h"
#include "vtkCell.h"
#include "vtkPointData.h"
#include "vtkPoints.h"



//------------------------------------------------------------------------------
vtkDataSetToLabelMap* vtkDataSetToLabelMap::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkDataSetToLabelMap");
  if(ret)
    {
      return (vtkDataSetToLabelMap*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkDataSetToLabelMap;
}




// Construct an instance of vtkDataSetToLabelMap 

// The models bounds are the min/max coordinates of the bounding box
// of the input. They are set to 0 so that ComputeOutputParameters finds the 
// bounds of the input automatically
// The sample dimensions are the number of voxels of the output
 

vtkDataSetToLabelMap::vtkDataSetToLabelMap()
{

  this->OutputDimensions[0] = 1;
  this->OutputDimensions[1] = 1;
  this->OutputDimensions[2] = 1;

  this->OutputSpacing[0] = 1;
  this->OutputSpacing[1] = 1;
  this->OutputSpacing[2] = 1;

  //this->BoundaryBoundaryBoolean = 0;
  
  this->BoundaryScalars = NULL;
  this->UseBoundaryVoxels = 1;
}


//----------------------------------------------------------------------------
vtkDataSetToLabelMap::~vtkDataSetToLabelMap()
{      

  if(this->BoundaryScalars)
    this->BoundaryScalars->Delete();
  this->UseBoundaryVoxels = 1;
}

/*******************************************************************************************
 *
 ****************************** EXECUTION PROC OF THIS FILTER  *****************************
 *
 *******************************************************************************************/


// execution of this filter
// SetOutputSpacing should be called before executing this filter if voxel
// size is different than the default (1,1,1)


void vtkDataSetToLabelMap::Execute()
{
  vtkPoints *points;
  vtkCell *cell;
  //vtkNormals *normals; 
  vtkDataSet *input=this->GetInput();
  vtkStructuredPoints *output=this->GetOutput();
  int cellNum;
  int numPts, numCells;
  float insidePoint[3];
  float *v0,*v1,*v2;
  float stepT,stepS;
  int i,j,k;
  int jkFactor;
  int idx;
  
  vtkDebugMacro(<< "Executing Voxel model");


  // compute model bounds: number of voxels on each edge of the box that holds
  // the model. Figure out the dimensions and origin from ComputeOutputParameters
  
  this->ComputeOutputParameters();
  
  numPts = (this->OutputDimensions[0]) * (this->OutputDimensions[1]) * (this->OutputDimensions[2]);
  // set things in output (vtkStructuredPoints)  
  output->SetDimensions(this->GetOutputDimensions());
  output->SetSpacing(this->GetOutputSpacing());
  output->SetOrigin(this->GetOutputOrigin());
  // create the scalars that will store the 0/1 bits


  // the inside voxels are 2
  if(this->BoundaryScalars)
    this->BoundaryScalars->Delete();
  this->BoundaryScalars = vtkShortArray::New();
  this->BoundaryScalars->SetNumberOfTuples(numPts);
  for (i=0; i<numPts; i++)
    {
      this->BoundaryScalars->SetTuple1(i,2);
    }
  

  
  // start the computation
  jkFactor = this->OutputDimensions[0]*this->OutputDimensions[1];

  // get the normals of the input
  numCells = input->GetNumberOfCells();

  
  // scan each cell in the list
  this->UpdateProgress (0.0);
  for (cellNum=0; cellNum < numCells; cellNum++)
    {
      //this->UpdateProgress ((float) cellNum / ((float) numCells));
      cell = input->GetCell(cellNum);      
      
      if (cell->GetCellType() != VTK_TRIANGLE) {
        //tell the user, then exit
        printf (" *********************************** \n one of the cells is not a triangle!! \n");
        return;
      }
      
      points = cell->GetPoints();
      
      //get the coordinates of each corner      
      v0 = points->GetPoint(0);
      v1 = points->GetPoint(1);
      v2 = points->GetPoint(2);
      
      // step T is spacing/(v2-v0)
      stepT = this->ComputeStep(this->GetOutputSpacing(),v2,v0);
      
      // step S is spacing/(v1-v0)
      stepS = this->ComputeStep(this->GetOutputSpacing(),v1,v0);
      //printf("stepT is %f, stepS is %f",stepT,stepS);
      
      for(float t = 0; t <= 1; t = t+stepT)
      {
        for(float s = 0; s <= 1; s = s+stepS)
        {
          
          if(this->IsPointInside(s,t))
          {
            
            // we get the coordinate of a point that is on the triangle plane (in world coordinates) stored in insidePoint
            this->EvaluatePoint(v0,v1,v2,s,t,insidePoint);
            //calculate idx in the vtkStructuredPoints coordinates
            // use floor because all calculations start with voxel 0
            // if the inside point is on an edge, do nothing
            
            
            i = floor((insidePoint[0] - this->OutputOrigin[0])/ this->OutputSpacing[0]);
            j = floor((insidePoint[1] - this->OutputOrigin[1])/ this->OutputSpacing[1]);
            k = floor((insidePoint[2] - this->OutputOrigin[2])/ this->OutputSpacing[2]);
            
            idx = jkFactor*k + this->OutputDimensions[0]*j + i;
            //printf(" scalar id %i is 1", idx);
            // check that idx is not too big
            if (idx > numPts) 
            {
              printf("ERROR scalar too big");
              //exit; removed by A.T. 7/30/02
              return;
            } 
            // the boundary voxels are 1
            if(this->UseBoundaryVoxels)
              this->BoundaryScalars->SetTuple1(idx,1);
            else 
              this->BoundaryScalars->SetTuple1(idx,0);
          }
        }
      }
    }
  
  
  //printf("************ END OF CELL TRAVERSAL ******************* \n");
     this->UpdateProgress (0.5);
  // now figure out inside voxels (0 is outside, 1 surface, 2 inside)
  this->BoundaryFill(0,0,0,this->BoundaryScalars);
  // add the boundary and inside scalars
  //printf("************ END OF BOUNDARY FILL ******************* \n");
  output->GetPointData()->SetScalars(this->BoundaryScalars);
  this->UpdateProgress (1.0);
}


/****************************************************************************
 *                                                                          *
 *                        HELPER FUNCTIONS FOR Execute()                    *
 *                                                                          *  
 ****************************************************************************/

void vtkDataSetToLabelMap::BoundaryFill(int i, int j, int k, vtkShortArray *scalars) {

  int idx = k *(this->OutputDimensions[0]*this->OutputDimensions[1]) + j*(this->OutputDimensions[0]) + i;
  int kFactor = this->OutputDimensions[0] * this->OutputDimensions[1];
  int jFactor = this->OutputDimensions[0];
  

  int numPoints = kFactor * this->OutputDimensions[2];
  int length = numPoints * 4;
  vtkIntArray* Q = vtkIntArray::New();
  Q->SetNumberOfValues(length);
  Q->SetValue(0,0);
  
  int pointer = 1;
  int current;
  int n;

  while(pointer != 0) {
    
    current = Q->GetValue(pointer-1);
    //printf("we are looking at id %i %i \n",current,Q->GetValue(pointer-1));
    
    pointer = pointer - 1;
    
    // 1 means we are outside
    if(scalars->GetTuple1(current) == 2) {
      //printf("pointer is %i, scalar value is 2",pointer);
      scalars->SetTuple1(current,0);
      
      //printf("it is 1, expand");
      
      // top
      n = current + kFactor;
      if (n <numPoints){
    if(scalars->GetTuple1(n) == 2) {
      Q->SetValue(pointer,n);
      pointer = pointer + 1;
    }
      }
      n = current - kFactor;
      // bottom
      if (n >= 0){
    if(scalars->GetTuple1(n) == 2) {
      Q->SetValue(pointer,n);
      pointer = pointer + 1;
    }
      }
      // front
      n = current + jFactor;
      if (n <numPoints){
    if(scalars->GetTuple1(n) == 2) {
      Q->SetValue(pointer,n);
      pointer = pointer + 1;
    }
      }
      // back
      n = current - jFactor;
      if (n >= 0){
    if(scalars->GetTuple1(n) == 2) {
      Q->SetValue(pointer,n);
      pointer = pointer + 1;
    }
      }
      // left
      n = current+1;
      if (n <numPoints){
    if(scalars->GetTuple1(n) == 2) {
      Q->SetValue(pointer,n);
      pointer = pointer + 1;
    }
      }
      n = current - 1;
      // right
      if (n >= 0){
    if(scalars->GetTuple1(n) == 2) {
      Q->SetValue(pointer,n);
      pointer = pointer + 1;
    }
      }   
    }
  }
  Q->Delete();
}


// Compute the parametric step to scan the triangles
// the step defines the max number of voxels that are traversed by
// this edge from vertex0 -> vertex1
float vtkDataSetToLabelMap::ComputeStep(float spacing[3],float vertex0[3],float vertex1[3])
{
  float tmpx = vertex1[0] - vertex0[0];
  float tmpy = vertex1[1] - vertex0[1];
  float tmpz = vertex1[2] - vertex0[2];
    
  float distance = sqrt(tmpx*tmpx+tmpy*tmpy+tmpz*tmpz);

  // how many voxels (based in the edge distance, the smallest distance to 
  // traverse a voxel) could fit in that edge?
  // take half that to be sure that we don't fall onto only edges of voxels
  // (since we discard points that fall on edges)
  // => (voxel edge * 5) / distance

  // The step should be 1/n where n is an integer, so that n*step = 1
  // [since we vary the step from 0->1]
  // so if (step > 1) <=> (n < 1) just cast it down to 1
  // if the step is 1/x where x is not an integer, take the ceiling of x
  
  float n = ceil(distance / (spacing[0] * .5));
  if (n < 1)
    n = 1;
  
  return (1/n);
}

//EvaluatePoint

void vtkDataSetToLabelMap::EvaluatePoint(float v0[3], float v1[3], float v2[3], float s, float t, float result[3])
{
  
  result[0] = v0[0]+ s*(v1[0]-v0[0]) + t*(v2[0]-v0[0]);
  result[1] = v0[1]+ s*(v1[1]-v0[1]) + t*(v2[1]-v0[1]);
  result[2] = v0[2]+ s*(v1[2]-v0[2]) + t*(v2[2]-v0[2]);
  
}

//IsPointInside
int vtkDataSetToLabelMap::IsPointInside(float s, float t) {
  if (s >= 0 && t >=0 && (s+t)<= 1) return 1;
  else return 0;
}


/*****************************************************************************************
 *
 ********************************** SETTER/GETTER METHODS ********************************
 *
 *****************************************************************************************/

// Compute the dimensions based on the input geometry.
// set the origin coordinate (bottom left corner) of the output


void vtkDataSetToLabelMap::ComputeOutputParameters()
{
  float *bounds;
  
  // compute model bounds 
  // get the bounding box of the input
  bounds = (this->GetInput())->GetBounds();
  
  // set the sample dimensions to be the edges of the bounding box + 2 voxels
  for (int i=0; i<3; i++)
    {
      this->OutputDimensions[i] = ((ceil(bounds[2*i+1]) - floor(bounds[2*i])) / this->OutputSpacing[i]) + 2;
    }
  
  
  // Set volume origin to be shifted by a voxel
  for (int j=0; j<3; j++)
    {
      this->OutputOrigin[j] = bounds[2*j] - this->OutputSpacing[j];
    }
}

// Set the size of the voxels
void vtkDataSetToLabelMap::SetOutputSpacing(float i, float j, float k)
{
  float dim[3];

  dim[0] = i;
  dim[1] = j;
  dim[2] = k;

  this->SetOutputSpacing(dim);
}

void vtkDataSetToLabelMap::SetOutputSpacing(float dim[3])
{
  int dataDim, i;

  vtkDebugMacro(<< " setting OutputSpacing to (" << dim[0] << "," << dim[1] << "," << dim[2] << ")");

  if ( dim[0] != this->OutputSpacing[0] || dim[1] != this->OutputSpacing[1] ||
  dim[2] != this->OutputSpacing[2] )
    {
      if ( dim[0]<=0 || dim[1]<=0 || dim[2]<=0 )
    {
      vtkErrorMacro (<< "Bad Output Dimensions, retaining previous values");
      return;
    }
      for (dataDim=0, i=0; i<3 ; i++)
    {
      if (dim[i] >= 0)
        {
          dataDim++;
        }
    }
      if ( dataDim  < 3 )
    {
      vtkErrorMacro(<<"Output dimensions must define a volume!");
      return;
    }
      
      for ( i=0; i<3; i++)
    {
      this->OutputSpacing[i] = dim[i];
    }
      this->Modified();
    }
}


// write the output of this filter (vtkStructuredPoints)

void vtkDataSetToLabelMap::Write(char *fname)
{
  FILE *fp;
  int i, j, k;
  float* spacing;


  int idx;
  int bitcount;
  unsigned char uc;
  vtkStructuredPoints *output=this->GetOutput();

  vtkDebugMacro(<< "Writing Voxel model");

  // update the data
  this->Update();
  
  //this->BoundaryScalars = output->GetPointData()->GetScalars();
  this->BoundaryScalars->DeepCopy(output->GetPointData()->GetScalars());
  this->ComputeOutputParameters();
  spacing = this->GetOutputSpacing();
  output->SetDimensions(this->GetOutputDimensions());
  

  fp = fopen(fname,"w");
  if (!fp) 
    {
    vtkErrorMacro(<< "Couldn't open file: " << fname << endl);
    return;
    }

  fprintf(fp,"Voxel Data File\n");
  fprintf(fp,"Origin: %f %f %f\n",this->OutputOrigin[0],this->OutputOrigin[1],this->OutputOrigin[2]);
  fprintf(fp,"Aspect: %f %f %f\n",this->OutputSpacing[0],this->OutputSpacing[1],this->OutputSpacing[2]);
  fprintf(fp,"Dimensions: %i %i %i\n",this->OutputDimensions[0],
      this->OutputDimensions[1],this->OutputDimensions[2]);

  // write out the data
  bitcount = 0;
  idx = 0;
  uc = 0x00;

  for (k = 0; k < this->OutputDimensions[2]; k++)
    {
    for (j = 0; j < this->OutputDimensions[1]; j++)
      {
      for (i = 0; i < this->OutputDimensions[0]; i++)
    {
    if (this->BoundaryScalars->GetTuple1(idx))
      {
      uc |= (0x80 >> bitcount);
      }
    bitcount++;
    if (bitcount == 8)
      {
      fputc(uc,fp);
      uc = 0x00;
      bitcount = 0;
      }
    idx++;
    }
      }
    }
  if (bitcount)
    {
    fputc(uc,fp);
    }

  fclose(fp);
}

void vtkDataSetToLabelMap::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkDataSetToStructuredPointsFilter::PrintSelf(os,indent);

  os << indent << "Output Dimensions: (" << this->OutputDimensions[0] << ", "
               << this->OutputDimensions[1] << ", "
               << this->OutputDimensions[2] << ")\n";
 os << indent << "Output Origin: (" << this->OutputOrigin[0] << ", "
               << this->OutputOrigin[1] << ", "
               << this->OutputOrigin[2] << ")\n";

 os << indent << "Output Spacing: (" << this->OutputSpacing[0] << ", "
               << this->OutputSpacing[1] << ", "
               << this->OutputSpacing[2] << ")\n";

}






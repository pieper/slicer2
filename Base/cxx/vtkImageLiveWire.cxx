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

#include "vtkImageLiveWire.h"
#include "stdlib.h"
#include "vtkObjectFactory.h"
#include <math.h>
#include <time.h>

//------------------------------------------------------------------------------
vtkImageLiveWire* vtkImageLiveWire::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkImageLiveWire");
  if(ret)
    {
    return (vtkImageLiveWire*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkImageLiveWire;
}

//----------------------------------------------------------------------------
// Constructor sets defaults
vtkImageLiveWire::vtkImageLiveWire()
{
  // settings to be changed by user
  for (int i=0; i<2; i++) 
    {
      // not initialized yet
      this->StartPoint[i] = -1;
      this->EndPoint[i] = -1;
    }
  memset(this->PrevEndPoint, 0, 2*sizeof(int));
  this->MaxEdgeCost = 255;
  this->Verbose = 0;
  this->Label = 2;

  // output
  this->ContourEdges = vtkPoints::New();
  this->ContourPixels = vtkPoints::New();
  this->NewEdges = vtkPoints::New();
  this->NewPixels = vtkPoints::New();

  // all inputs 
  this->NumberOfRequiredInputs = 5;
  this->NumberOfInputs = 0;

  // path computation data structures
  this->Q = NULL;
  this->CC = NULL;
  this->Dir = NULL;
  this->L = NULL;
  this->B = NULL;

  // what connectedness the path uses
  this->NumberOfNeighbors = 4;
  //this->NumberOfNeighbors = 8;

  // for display only
  this->InvisibleLastSegment = 0;
}

//----------------------------------------------------------------------------
vtkImageLiveWire::~vtkImageLiveWire()
{      
  this->DeallocatePathInformation();

  if (this->ContourEdges)
    {
      this->ContourEdges->Delete();
    }
  if (this->ContourPixels)
    {
      this->ContourPixels->Delete();
    }
  if (this->NewPixels)
    {
      this->NewPixels->Delete();
    }
  if (this->NewEdges)
    {
      this->NewEdges->Delete();
    }
}

//----------------------------------------------------------------------------
// Clear the last part (revert to the previous clicked-on point)
void vtkImageLiveWire::ClearLastContourSegment() 
{
  float *point;
  int done = 0;
  int i;
  vtkPoints *tempPixels = vtkPoints::New();

  // remove the latest section of points
  int numPoints = this->ContourPixels->GetNumberOfPoints();
  int index;
  // find the previous endpoint, at location number index
  for (index=numPoints-2; (index>=0 && !done); index--)
    {
      point = this->ContourPixels->GetPoint(index);
      if ((int)point[2] == 1)
	{
	  // we have hit an endpoint at index
	  done = 1;
	  // we would like to start here from now on
	  this->StartPoint[0] = (int)point[0];
	  this->StartPoint[1] = (int)point[1];
	  this->EndPoint[0] = (int)point[0];
	  this->EndPoint[1] = (int)point[1];
	  this->DeallocatePathInformation();
	}
    }  
  // remove all things after the index by resetting, copying, etc.
  for (i = 0; i <= index; i++)
    {
      point = this->ContourPixels->GetPoint(i);
      tempPixels->InsertPoint(i,point);
    }
  this->ContourPixels->Reset();
  for (i = 0; i <= index; i++)
    {
      point = tempPixels->GetPoint(i);
      this->ContourPixels->InsertPoint(i,point);
    }  


//    // do all the above for this->ContourEdges too
//    tempPixels->Reset();
//    done = 0;
//    numPoints = this->ContourEdges->GetNumberOfPoints();
//    // find the previous endpoint, at location number index
//    for (index=numPoints-2; (index>=0 && !done); index--)
//      {
//        point = this->ContourEdges->GetPoint(index);
//        if ((int)point[2] == 1)
//  	{
//  	  // we have hit an endpoint at index
//  	  done = 1;
//  	}
//      }  
//    // remove all things after the index by resetting, copying, etc.
//    for (i = 0; i <= index; i++)
//      {
//        point = this->ContourEdges->GetPoint(i);
//        tempPixels->InsertPoint(i,point);
//      }
//    this->ContourEdges->Reset();
//    for (i = 0; i <= index; i++)
//      {
//        point = tempPixels->GetPoint(i);
//        this->ContourEdges->InsertPoint(i,point);
//      }  


  // reset the moving "tail" of the wire
  //  this->NewEdges->Reset();
  this->NewPixels->Reset();

  this->Modified();
}


//----------------------------------------------------------------------------
// This method deletes stored shortest path information
// This must be called when input or start point change.
void vtkImageLiveWire::DeallocatePathInformation() 
{
  // delete all stored data
  if (this->Q) 
    {
      delete(this->Q);
      this->Q = NULL;
    }
  if (this->CC)
    {
      delete(this->CC);
      this->CC = NULL;
    }
  if (this->Dir)
    {
      delete(this->Dir);
      this->Dir = NULL;
    }
  if (this->L)
    {
      delete(this->L);
      this->L = NULL;
    }
  if (this->B)
    {
      delete(this->B);
      this->B = NULL;
    }
}

//----------------------------------------------------------------------------
// Set up data structures if they don't currently exist.
void vtkImageLiveWire::AllocatePathInformation(int numRows, int numCols) 
{
  // data structures (see IEEE Trans Med Img Jan 2000, Live Wire on the Fly)
  // These can only be allocated when the size of the input is known.

  if (!this->Q)
    {
      this->Q = new circularQueue(numRows, numCols, this->GetMaxEdgeCost());
      // debug
      if (this->Verbose > 1) 
	{
	  this->Q->VerboseOn();
	}
    }
  if (!this->CC)
    {
      this->CC = new array2D<int>(numRows, numCols, 65536);

      // current cumulative cost at StartPoint must be 0
      this->CurrentCC = 0;
      // set start point's cost in CC array
      (*this->CC)(this->StartPoint[0],this->StartPoint[1]) = this->CurrentCC;
      // Insert StartPoint into Q (into zero-length path bucket)
      Q->Insert(this->StartPoint[0], this->StartPoint[1], this->CurrentCC);      
    }
  if (!this->Dir)
    {
      this->Dir = new array2D<int>(numRows, numCols, this->NONE);
    }
  if (!this->L)
    {
      this->L = new array2D<int>(numRows, numCols, 0);
    }
  if (!this->B)
    {
      this->B = new array2D<int>(numRows, numCols, 0);
    }
}

//----------------------------------------------------------------------------
// This method computes the input extent necessary to generate the output.
void vtkImageLiveWire::ComputeInputUpdateExtent(int inExt[6],
							  int outExt[6],
							  int whichInput)
{
  int *wholeExtent;
  int idx;

  wholeExtent = this->GetInput(whichInput)->GetWholeExtent();
  
  memcpy(inExt,outExt,6*sizeof(int));
  
  // grow input whole extent.
  for (idx = 0; idx < 2; ++idx)
    {
    inExt[idx*2] -= 2;
    inExt[idx*2+1] += 2;

    // we must clip extent with whole extent is we hanlde boundaries.
    if (inExt[idx*2] < wholeExtent[idx*2])
      {
	inExt[idx*2] = wholeExtent[idx*2];
      }
    if (inExt[idx*2 + 1] > wholeExtent[idx*2 + 1])
      {
	inExt[idx*2 + 1] = wholeExtent[idx*2 + 1];
      }
    }
}

//----------------------------------------------------------------------------
// This method sets the StartPoint and resets stored shortest path information
void vtkImageLiveWire::SetStartPoint(int x, int y)
{
  int modified = 0;
  int extent[6];
  int i;

  if (this->NumberOfInputs < this->NumberOfRequiredInputs)
    {
      // the pipeline isn't all set up yet
      vtkErrorMacro(<< "SetStartPoint: Expected " << this->NumberOfRequiredInputs << " inputs, got only " << this->NumberOfInputs);
      return;      
    }

  // if we have a previous short path, add it to contour 
  // and start next short path from contour's end
  if (this->NewPixels->GetNumberOfPoints())
    {

      // if end point of current contour doesn't match where the user clicked.
      if (x != this->EndPoint[0] || y != this->EndPoint[1])
	{
	  cout << "click: ("<<x<<","<<y<<") end: ("<<this->EndPoint[0]<<","<<this->EndPoint[1]<<")"<<endl;
	}

      // we have a contour already.  Start adding to its end:
      x  = this->EndPoint[0];
      y  = this->EndPoint[1];     

      // append new points to the saved contour
     //   int numPoints = this->NewEdges->GetNumberOfPoints();
//        for (i = 0; i < numPoints; i++)
//  	{
//  	  this->ContourEdges->InsertNextPoint(this->NewEdges->GetPoint(i));
//  	}

      // append new points to the saved contour
      int numPoints = this->NewPixels->GetNumberOfPoints();
      for (i = 0; i < numPoints; i++)
	{
	  this->ContourPixels->InsertNextPoint(this->NewPixels->GetPoint(i));
	}
    }
  else
    {
      // start a new contour.
      // crop point with image extent:
      if (this->GetInput(1)) 
	{
	  // all inputs should have the same extent
	  this->GetInput(1)->GetExtent(extent);
	}
      else
	{
	  // the pipeline isn't all set up yet
	  cout << "LiveWire SetStartPoint: No input 1 yet!" << endl;
	  memset(extent, 0, 6*sizeof(int));
	}
      
      if (x < extent[0]) 
	x = extent[0];
      else
	if (x > extent[1]) 
	  x = extent[1];
      
      if (y < extent[2]) 
	y = extent[2];
      else
	if (y > extent[3])
	  y = extent[3];

      // also put the end point here since starting a new contour
      this->EndPoint[0] = x;
      this->EndPoint[1] = y;
    }

  //cout << "Coords of start point: (" << x << "," << y << ")" << endl;      

  if (this->StartPoint[0] != x)
    {
      modified = 1;
      this->StartPoint[0] = x;
    }
  if (this->StartPoint[1] != y)
    {
      modified = 1;
      this->StartPoint[1] = y;
    }

  if (modified)
    {
      // delete everything
      this->DeallocatePathInformation();
      // Don't set this->Modified until EndPoint is also set 
      // (this is when we are ready to execute).

    }
}

//----------------------------------------------------------------------------
// This method sets the EndPoint 
void vtkImageLiveWire::SetEndPoint(int x, int y)
{
  int modified = 0;
  int extent[6];

  if (this->NumberOfInputs < this->NumberOfRequiredInputs)
    {
      // the pipeline isn't all set up yet
      vtkErrorMacro(<< "SetEndPoint: Expected " << this->NumberOfRequiredInputs << " inputs, got only " << this->NumberOfInputs);
      return;      
    }

  // if there is no start point yet, don't set the end point
  if (this->StartPoint[0] == -1 || this->StartPoint[1] == -1)
    {
      return;
    }

  // just check against the first edge input
  if (this->GetInput(1)) 
    {
      this->GetInput(1)->GetExtent(extent);
    }
  else
    {
      cout << "LiveWire SetEndPoint: No input 1 yet!" << endl;
      memset(extent, 0, 6*sizeof(int));
    }
     
  // crop point with image coordinates
  if (x < extent[0] || x > extent[1] ||
      y < extent[2] || y > extent[3]) 
    {
      cout << "Coords (" << x << "," << y << ") are outside of image!" << endl;      
      if (x < extent[0]) x = extent[0];
      else
	if (x > extent[1]) x = extent[1];
      
      if (y < extent[2]) y = extent[2];
      else
	if (y > extent[3]) y = extent[3];
    }

  //cout << "Coords of end point: (" << x << "," << y << ")" << endl;      
  
  if (this->EndPoint[0] != x)
    {
      modified = 1;
      this->EndPoint[0] = x;
    }
  if (this->EndPoint[1] != y)
    {
      modified = 1;
      this->EndPoint[1] = y;
    }
  
  if (modified)
    {
      this->Modified();
    }
}

//----------------------------------------------------------------------------
// This method clears old saved paths 
// (use it to start over from a new start point)
void vtkImageLiveWire::ClearContour()
{
//    if (this->Verbose > 0)
//      {
//        cout << "Clear Contour" << endl;
//      }

  this->ContourPixels->Reset();
  //  this->ContourEdges->Reset();
  //this->NewEdges->Reset();
  this->NewPixels->Reset();

  // unset start and end points
  for (int i=0; i<2; i++) 
    {
      // not initialized yet
      this->StartPoint[i] = -1;
      this->EndPoint[i] = -1;
    }

  // Next Execute will output a clear image.
  this->Modified();

//    if (this->Verbose > 0)
//      {
//        cout << "Clear Contour Done" << endl;
//      }
}

//----------------------------------------------------------------------------
// Description:
// This templated function executes the filter for any type of data.
template <class T>
static void vtkImageLiveWireExecute(vtkImageLiveWire *self,
				     vtkImageData **inDatas, T **inPtrs,
				     vtkImageData *outData, T* outPtr)
{
  int *extent = inDatas[0]->GetWholeExtent();
  int numrows = extent[1] - extent[0] + 1;
  int numcols = extent[3] - extent[2] + 1;
  int i;

  clock_t tStart, tEnd, tDiff;
  tStart = clock();

  int sizeX, sizeY, sizeZ, outExt[6];
  outData->GetExtent(outExt);
  sizeX = outExt[1] - outExt[0] + 1; 
  sizeY = outExt[3] - outExt[2] + 1; 
  sizeZ = outExt[5] - outExt[4] + 1;
  // clear the output (will draw a contour over it later).
  memset(outPtr, 0, sizeX*sizeY*sizeZ*sizeof(T));   

  int *start = self->GetStartPoint();
  int *end = self->GetEndPoint();

  // if the start or end points are not set, just output the clear image.
  if (start[0] == -1 || start[1] == -1 || end[0] == -1 || end[1] == -1)
    {
      //cout << "clear image output since point(s) -1" << endl;
      return;
    }  

  // ----------------  Data structures  ------------------ //

  // The reason for all the tests for the number of neighbors 
  // below is that the phase-based live wire uses 8-connected neighbors
  // and finds paths along pixels, while the regular live wire 
  // uses 4-connected paths *along the cracks between pixels*.
  // The input images are not perfectly aligned in the 
  // 4-neighbor case, and the path found is the outline of the pixels
  // to draw on the slice.

  // allocate if don't exist
  self->AllocatePathInformation(numrows, numcols);
  // for nice access to arrays
  circularQueue *Q = self->Q;
  array2D<int> &CC = (*self->CC);
  array2D<int> &Dir = (*self->Dir);
  array2D<int> &L = (*self->L);

  const int NONE = self->NONE;
  const int UP = self->UP;
  const int DOWN = self->DOWN;
  const int LEFT = self->LEFT;
  const int RIGHT = self->RIGHT;
  // used if running 8-connected
  const int UP_LEFT = self->UP_LEFT;
  const int UP_RIGHT = self->UP_RIGHT;
  const int DOWN_LEFT = self->DOWN_LEFT;
  const int DOWN_RIGHT = self->DOWN_RIGHT;

  // order of arrows, neighbors, offsets, and colors matches with edges below.
  
  // directions the path takes to the neighbors
  int arrows[8] = {UP, DOWN, LEFT, RIGHT, UP_LEFT, UP_RIGHT, 
		   DOWN_LEFT, DOWN_RIGHT};

  // to look for path to 4 or 8 neighbors of current pixel corner
  int neighbors[8][2] = {{0,1},{0,-1},{-1,0},{1,0},{-1,1},
			 {1,1},{-1,-1},{1,-1}};

  // sqrt two factor: diagonal edges cost more.
#define SQRT_TWO 1.4142
  float factor[8] = {1,1,1,1,SQRT_TWO,SQRT_TWO,SQRT_TWO,SQRT_TWO};

  // offset to access edge images 
  int offset[8][2] = {{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}};
  if (self->GetNumberOfNeighbors() == 4)
    {
      // handle misalignment of input images
      //int offset[4][2] = {{-1,-1},{0,0},{-1,-1},{0,0}};
      offset[0][0] = -1;
      offset[0][1] = -1;
      offset[2][0] = -1;
      offset[2][1] = -1;
    }


  // to color in the correct pixel relative to the edge
  int color[8][2] = {{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}};
  if (self->GetNumberOfNeighbors() == 4)
    {
      // handle the fact that we color pixels 
      // but path is along pixel cracks
      //
      // the edge is stored at the lower left hand corner of each pixel
      // (when looking at the image).  so using this and the bel type:
      //int color[4][2] = {{0,0},{-1,-1},{-1,0},{0,-1}};
      color[1][0] = -1;
      color[1][1] = -1;
      color[2][0] = -1;
      color[3][1] = -1;
    }

  // edge information to use when checking out a pixel's neighbors:
  T* edges[8] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
  for (i = 1; i < self->GetNumberOfInputs(); i++) 
    {
      // i-1 since 1st input to this filter is original image, not edge image
      edges[i - 1] = inPtrs[i];
    }


  // ----------------  Dijkstra's algorithm starts here ------------------ //

  // cumulative cost of "longest shortest" path found so far
  int currentCC = self->GetCurrentCC();
  
  int currentX = -1;
  int currentY = -1;
  // while end point not in L keep checking out neighbors of current point
  while ( L(end[0],end[1]) == 0) 
    {

      // get min vertex from Q 
      ListElement *min = Q->GetListElement(currentCC);

      // debug: test if same as last point.
      if (min->Coord[0] == currentX && min->Coord[1] == currentY)
	{
	  cout << "ERROR in vtkImageLiveWireExecute: same point as last time!" << endl;
	  cout << "-- CC: " << CC(min->Coord[0],min->Coord[1]) << " --  (" << min->Coord[0]
	       << "," << min->Coord[1]<< ") -- L: " << L(end[0],end[1]) 
	       <<" --" << endl;
	  return;
	}

      // update the current location
      currentX = min->Coord[0];
      currentY = min->Coord[1];

      // update the current path cost
      currentCC = CC(currentX,currentY);

      // put vertex into L, the already looked at list
      L(currentX,currentY) = 1;

      // remove it from Q
      Q->Remove(min);
      
//        if (self->GetVerbose() > 1) 
//  	{
//  	  cout << "-- CC: " << currentCC << " --  (" << currentX 
//  	       << "," << currentY << ") -- L: " << L(end[0],end[1]) 
//  	       <<" --" << endl;
//  	}
    
//        if (self->GetVerbose() > 0) 
//  	{
//  	  if (currentX == end[0] && currentY == end[1])
//  	    {
//  	      cout << "final point: (" << currentX << "," 
//  		   << currentY << ")  CC: " << currentCC << endl;
//  	    }
//  	}


      // check out its 4 or 8 neighbors
      int oldCC, tempCC;
      int x,y;
      T* edge;
      int totalNeighbors = self->GetNumberOfNeighbors();

      for (int n=0; n < totalNeighbors; n++) 
	{
	  x = currentX + neighbors[n][0];
	  y = currentY + neighbors[n][1];

	  // if neighbor in image
	  if (y < numrows && x < numcols && x >= 0 && y >= 0) 
	    {
	      // save previous cost to reach this point
	      oldCC = CC(x,y);
	      edge = edges[n];

	      // find new path's cost.
	      // (first handle shift in UP and LEFT edge images)
	      int ex = x + offset[n][0];
	      int ey = y + offset[n][1];
	      // if edge cost in (shifted) edge image
	      if (ey < numrows && ex < numcols && ex >= 0 && ey >= 0) 
		{
		  // if we are running w/ regular 4-connected path
		  if (totalNeighbors == 4) 
		    {
		      tempCC = currentCC + (int)edge[ex + ey*numcols];
		    }
		  else
		    {
		      // error checking
		      //  if ((int)edge[ex + ey*numcols] > self->GetMaxEdgeCost())
//  			{
//  			  cout << "lw: " << (int)edge[ex + ey*numcols];
//  			}
		      
		      // extra cost for 8-connected corner path
		      int edgeCost = (int)(edge[ex + ey*numcols]*factor[n]);
		      if (edgeCost > self->GetMaxEdgeCost()) 
			edgeCost = self->GetMaxEdgeCost();
		      tempCC = currentCC + edgeCost;
		    }

		}
	      else
		{
		  // handle boundary
		  tempCC = currentCC + self->GetMaxEdgeCost();
		}

//  	      if (self->GetVerbose() > 2) 
//  		{
//  		  cout << "NEIGHBOR (" << x << "," << y << ") CC: " 
//  		       << tempCC << "," << oldCC << endl;
//  		}
	      
	      // if path from current point shorter than old path
	      if (tempCC < oldCC) 
		{
		  // lower the cumulative cost to reach this neighbor
		  CC(x,y) = tempCC;

		  // store new short path direction
		  Dir(x,y) = arrows[n];

		  // remove this neighbor from Q if in it
		  Q->Remove(x,y);

		  // then put it in the proper place in Q for its new cost
		  Q->Insert(x,y,CC(x,y));
		}

	    } // end if neighbor in image
	} // end loop over neighbors
    } // end while

  // save cost for next time (essentially pointer to current location in Q)
  self->SetCurrentCC(currentCC);
  
  //cout << "current cc: " << currentCC << endl;


  // ------- Trace the shortest path using the Dir array. -----------//

  // clear previous shortest path points
  //vtkPoints *newEdges = self->GetNewEdges();
  vtkPoints *newPixels = self->GetNewPixels();
  //newEdges->Reset();
  newPixels->Reset();
  vtkPoints *tempPixels = vtkPoints::New();

  // current spot on path of arrows
  int traceX = end[0];
  int traceY = end[1];

  // current pixel to color in output image
  // (may not be same as trace since trace is along pixel edges
  // for 4-connected and it outlines the area to color)
  // in this case coloring assumes clockwise segmentation.
  int colorX, colorY;

  // Insert first points into lists
  //newEdges->InsertNextPoint(traceX,traceY,0);
  //newEdges->InsertNextPoint(traceX,traceY,Dir(traceX,traceY));
  colorX = traceX + color[Dir(traceX,traceY)][0];
  colorY = traceY + color[Dir(traceX,traceY)][1];
  // the 1 at the end means this was an endpoint
  tempPixels->InsertNextPoint(colorX,colorY,1);

  // follow "arrows" backwards from end point to the start point
  while (traceX!=start[0] || traceY!=start[1])
    {
//        if (self->GetVerbose() > 2) 
//  	{
//  	  cout <<"("<<traceX<<","<<traceY<<")"<<"  ("<<colorX<<","<<colorY<<")"<<endl;
//  	}

      // arrow is NONE, UP, DOWN, LEFT, or RIGHT
      int arrow = Dir(traceX,traceY);
      if (arrow == NONE)
	{
	  cout << "ERROR in vtkImageLiveWire: broken path" << endl;
	  return;      
	}

      traceX -= neighbors[arrow][0];
      traceY -= neighbors[arrow][1];
      // select the pixel to color in
      colorX = traceX + color[arrow][0];
      colorY = traceY + color[arrow][1];

      if (self->GetVerbose() > 2) 
	{
	  cout << arrow;
	}

      // add to path lists
      tempPixels->InsertNextPoint(colorX,colorY,0);
      //newEdges->InsertNextPoint(traceX,traceY,Dir(traceX,traceY));

    } // end while

//    if (self->GetVerbose() > 0) 
//      {
//        cout << "(" << traceX << "," << traceY << ")" << endl;
//      }  

  // fix (reverse) the order of the pixels list.
  int numPoints = tempPixels->GetNumberOfPoints();
  newPixels->SetNumberOfPoints(numPoints);
  float *point;
  int count = 0;

  // the 1 at the end when setting the point means this was an endpoint
  point = tempPixels->GetPoint(numPoints-1);
  newPixels->SetPoint(count,point[0],point[1],1);
  count++;

  for (i=numPoints-2; i>=0; i--)
    {
      point = tempPixels->GetPoint(i);
      newPixels->SetPoint(count,point);
      count++;
    }  


  // ----------------  Output Image  ------------------ //
  // draw points over image
  T outLabel = (T)self->GetLabel();
  numPoints = newPixels->GetNumberOfPoints();

  // don't draw last section if it should be invisible
  if (self->GetInvisibleLastSegment() == 0) 
    {
      // draw last segment
      for (i=0; i<numPoints; i++)
	{
      
	  for (i=0; i<numPoints; i++)
	    {
	      //cout << ".";
	      point = newPixels->GetPoint(i);
	      //cout << (int)point[0] + ((int)point[1])*sizeX << endl;
	      if ((int)point[2] == 1)
		{
		  // color endpoints differently
		  outPtr[(int)point[0] + ((int)point[1])*sizeX] = 10;	  
		}
	      else
		{
		  outPtr[(int)point[0] + ((int)point[1])*sizeX] = outLabel;
		}
	    }
	}

    }
  // draw previously chosen contour over image
  vtkPoints *contour = self->GetContourPixels();
  numPoints = contour->GetNumberOfPoints();
  for (i=0; i<numPoints; i++)
    {
      //cout << ".";
      point = contour->GetPoint(i);


      if ((int)point[2] == 1)
	{
	  // color endpoints differently
	  outPtr[(int)point[0] + ((int)point[1])*sizeX] = 10;	  
	}
      else
	{
	//cout << (int)point[0] + ((int)point[1])*sizeX << endl;
	  outPtr[(int)point[0] + ((int)point[1])*sizeX] = outLabel;
	}
    }  


//    // ------------- test --------------
//    numPoints = newEdges->GetNumberOfPoints();
//    for (i=0; i<numPoints; i++)
//      {
//        //cout << ".";
//        point = newEdges->GetPoint(i);
//        //cout << (int)point[0] + ((int)point[1])*sizeX << endl;
//        //outPtr[(int)point[0] + ((int)point[1])*sizeX] += 10;
//        // color depending on direction followed...
//        //outPtr[(int)point[0] + ((int)point[1])*sizeX] = (T)((point[2]+1)*2);
//      }
//    // ------------- end test --------------

  // test points
  //cout << "num points C++ " << newEdges->GetNumberOfPoints() << endl;

  tEnd = clock();
  tDiff = tEnd - tStart;
  cout << "LW time: " << tDiff << endl;

  return;
}


//----------------------------------------------------------------------------
// Description:
// This method is passed a input and output data, and executes the filter
// algorithm to fill the output from the input.
// It just executes a switch statement to call the correct function for
// the datas data types.
void vtkImageLiveWire::Execute(vtkImageData **inDatas, 
			       vtkImageData *outData)
{
  void **inPtrs = new void*[this->NumberOfInputs];
  void *outPtr;
  int i;

  // if EndPoint is the same, do nothing.
  if (this->EndPoint[0] == this->PrevEndPoint[0] &&
      this->EndPoint[1] == this->PrevEndPoint[1]) 
    {
      //cout << "End point the SAME, not executing" << endl;
      return;
    }
  this->PrevEndPoint[0] = this->EndPoint[0];
  this->PrevEndPoint[1] = this->EndPoint[1];

  int type = this->GetInput(0)->GetScalarType();

  for (i = 0; i < this->NumberOfInputs; i++) 
    {
      // Scalar type of all inputs should be the same
      if (this->GetInput(i)->GetScalarType() != type) {
	vtkErrorMacro(<<"Inputs are not same scalar type");
	return;
      }

      // Single component input is required
      int c = this->GetInput(i)->GetNumberOfScalarComponents();
      if (c != 1) {
	vtkErrorMacro(<<"Input " << i << "  has "<< c <<" instead of 1 scalar component.");
	return;
      }

      // 2D input is required
      int ext[6];
      inDatas[i]->GetExtent(ext);
      if (ext[4] != ext[5]) {
	vtkErrorMacro(<<"Input is not 2D");
	return;
      }

      // input pointers        
      inPtrs[i] = inDatas[i]->GetScalarPointerForExtent(ext);
    }

  outPtr = outData->GetScalarPointerForExtent(outData->GetExtent());
  
  switch (inDatas[0]->GetScalarType())
    {
    case VTK_DOUBLE:
      vtkImageLiveWireExecute(this, inDatas, (double **)(inPtrs), outData, (double *)(outPtr));
      break;
    case VTK_FLOAT:
      vtkImageLiveWireExecute(this, inDatas, (float **)(inPtrs), outData, (float *)(outPtr));
      break;
    case VTK_LONG:
      vtkImageLiveWireExecute(this, inDatas, (long **)(inPtrs), outData, (long *)(outPtr));
      break;
    case VTK_UNSIGNED_LONG:
      vtkImageLiveWireExecute(this, inDatas, (unsigned long **)(inPtrs), outData, (unsigned long *)(outPtr));
      break;
    case VTK_INT:
      vtkImageLiveWireExecute(this, inDatas, (int **)(inPtrs), outData, (int *)(outPtr));
      break;
    case VTK_UNSIGNED_INT:
      vtkImageLiveWireExecute(this, inDatas, (unsigned int **)(inPtrs), outData, (unsigned int *)(outPtr));
      break;
    case VTK_SHORT:
      vtkImageLiveWireExecute(this, inDatas, (short **)(inPtrs), outData, (short *)(outPtr));
      break;
    case VTK_UNSIGNED_SHORT:
      vtkImageLiveWireExecute(this, inDatas, (unsigned short **)(inPtrs), outData, (unsigned short *)(outPtr));
      break;
    case VTK_CHAR:
      vtkImageLiveWireExecute(this, inDatas, (char **)(inPtrs), outData, (char *)(outPtr));
      break;
    case VTK_UNSIGNED_CHAR:
      vtkImageLiveWireExecute(this, inDatas, (unsigned char **)(inPtrs), outData, (unsigned char *)(outPtr));
      break;
    default:
      vtkErrorMacro(<< "Execute: Unknown input ScalarType");
      return;
    }
}

// Lauren check inputs are same type and size
//----------------------------------------------------------------------------
// Make sure both the inputs are the same size. Doesn't really change 
// the output. Just performs a sanity check
void vtkImageLiveWire::ExecuteInformation(vtkImageData **inputs,
					    vtkImageData *vtkNotUsed(output))
{
  int *in1Ext, *in2Ext;
  
  // we require that all inputs have been set.
  if (this->NumberOfInputs < this->NumberOfRequiredInputs)
    {
      vtkErrorMacro(<< "ExecuteInformation: Expected " << this->NumberOfRequiredInputs << " inputs, got only " << this->NumberOfInputs);
      return;      
    }

  // Lauren fix this to check all extents are the same size.

  in1Ext = inputs[0]->GetWholeExtent();
  in2Ext = inputs[1]->GetWholeExtent();


  if (in1Ext[0] != in2Ext[0] || in1Ext[1] != in2Ext[1] || 
      in1Ext[2] != in2Ext[2] || in1Ext[3] != in2Ext[3] || 
      in1Ext[4] != in2Ext[4] || in1Ext[5] != in2Ext[5])
    {
      vtkErrorMacro("ExecuteInformation: Inputs are not the same size. " <<
		    in1Ext[0] << " " << in1Ext[1] << " " << in1Ext[2] << in1Ext[3] <<
		    in2Ext[0] << " " << in2Ext[1] << " " << in2Ext[2] << in2Ext[3] );
      return;
    }
}

void vtkImageLiveWire::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkImageMultipleInputFilter::PrintSelf(os,indent);
}



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

// Lauren make sure this does what we want...
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
  int numEdgePoints, numPixPoints;

  // if we have a previous short path, add it to contour 
  // and start next short path from contour's end
  // (even if end point of contour doesn't match where the user clicked.)
  if (this->NewEdges->GetNumberOfPoints())
    {
      // Lauren need to check if clicked twice on same start point???

      if (x != this->EndPoint[0] || y != this->EndPoint[1])
	{
	  cout << "click: ("<<x<<","<<y<<") end: ("<<this->EndPoint[0]<<","<<this->EndPoint[1]<<")"<<endl;
	}
      // we have a contour already.  Start adding to its end:
      x  = this->EndPoint[0];
      y  = this->EndPoint[1];     

      // Lauren make sure curve goes through this point??

      // append new points to the saved contour
      int numPoints = this->NewEdges->GetNumberOfPoints();
      for (int i = 0; i < numPoints; i++)
	{
	  this->ContourEdges->InsertNextPoint(this->NewEdges->GetPoint(i));
	}

      numPoints = this->NewPixels->GetNumberOfPoints();
      // confuses compilers
      //for (int i = 0; i < numPoints; i++)
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

  // Lauren check this first???
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

  //cout << "deallocating..." << endl;

  if (modified)
    {
      // delete everything
      this->DeallocatePathInformation();
      // Lauren don't set Modified until EndPoint is also set?

    }
  //cout << "deallocated" << endl;
}

//----------------------------------------------------------------------------
// This method sets the EndPoint 
void vtkImageLiveWire::SetEndPoint(int x, int y)
{
  int modified = 0;
  int extent[6];

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
  // Lauren remove the outer "if"
  if (x < extent[0] || x > extent[1] ||
      y < extent[2] || y > extent[3]) 
    {
      cout << "Coords (" << x << "," << y << ") are outside of image!" << endl;      
      // Lauren test!
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
  if (this->Verbose > 0)
    {
      cout << "Clear Contour" << endl;
    }

  this->ContourPixels->Reset();
  this->ContourEdges->Reset();
  this->NewEdges->Reset();
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

  if (this->Verbose > 0)
    {
      cout << "Clear Contour Done" << endl;
    }
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
  int numpix = numrows*numcols;

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

  // Lauren test if input image is different, must deallocate all stuff.
  // Need to override SetInput! (also check for 2D input, etc.)
  // Also, is update information for this kind of thing?????


  // ----------------  Data structures  ------------------ //

  // allocate if don't exist
  self->AllocatePathInformation(numrows, numcols);
  // for nice access to arrays
  circularQueue *Q = self->Q;
  array2D<int> &CC = (*self->CC);
  array2D<int> &Dir = (*self->Dir);
  array2D<int> &L = (*self->L);
  array2D<int> &B = (*self->B);

  const int NONE = self->NONE;
  const int UP = self->UP;
  const int DOWN = self->DOWN;
  const int LEFT = self->LEFT;
  const int RIGHT = self->RIGHT;
  
  // arrows, neighbors, offsets, and colors match with edges below.
  int arrows[4] = {UP, DOWN, LEFT, RIGHT};
  // to test path to 4 neighbors of current pixel corner
  int neighbors[4][2] = {{0,1},{0,-1},{-1,0},{1,0}};
  // to access edge images (not perfectly aligned)
  int offset[4][2] = {{-1,-1},{0,0},{-1,-1},{0,0}};

  // to color in the correct pixel relative to the edge
  //int color[4][2] = {{0,1},{-1,0},{-1,1},{0,0}};
  // the edge is stored at the lower left hand corner of each pixel
  // (when looking at the image).  so using this and the bel type:
  int color[4][2] = {{0,0},{-1,-1},{-1,0},{0,-1}};
  
  vtkImageData *upEdge, *downEdge, *leftEdge, *rightEdge;
  upEdge = self->GetUpEdges();
  downEdge = self->GetDownEdges();
  leftEdge = self->GetLeftEdges();
  rightEdge = self->GetRightEdges();

  T *upEdgeVal = (T*)upEdge->GetScalarPointerForExtent(upEdge->GetExtent());
  T *downEdgeVal = (T*)downEdge->GetScalarPointerForExtent(downEdge->GetExtent());
  T *leftEdgeVal = (T*)leftEdge->GetScalarPointerForExtent(leftEdge->GetExtent());
  T *rightEdgeVal = (T*)rightEdge->GetScalarPointerForExtent(rightEdge->GetExtent());

  T* edges[4] = {upEdgeVal, downEdgeVal, leftEdgeVal, rightEdgeVal};

  // ----------------  Dijkstra ------------------ //

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
      
      if (self->GetVerbose() > 1) 
	{
	  cout << "-- CC: " << currentCC << " --  (" << currentX 
	       << "," << currentY << ") -- L: " << L(end[0],end[1]) 
	       <<" --" << endl;
	}
    
      if (self->GetVerbose() > 0) 
	{
	  if (currentX == end[0] && currentY == end[1])
	    {
	      cout << "final point: (" << currentX << "," 
		   << currentY << ")  CC: " << currentCC << endl;
	    }
	}


      // check out its 4 neighbors
      int oldCC, tempCC;
      int x,y;
      T* edge;

      for (int n=0; n<4; n++) 
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
		  tempCC = currentCC + (int)edge[ex + ey*numcols];
		}
	      else
		{
		  // handle boundary
		  tempCC = currentCC + self->GetMaxEdgeCost();
		}

	      if (self->GetVerbose() > 2) 
		{
		  cout << "NEIGHBOR (" << x << "," << y << ") CC: " 
		       << tempCC << "," << oldCC << endl;
		}
	      
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

  // save cost for next time
  self->SetCurrentCC(currentCC);
  
  //cout << "current cc: " << currentCC << endl;

  // ------- Trace the shortest path using the Dir array. -----------//

  // Lauren row column, x and y confusing.  test/fix it all.

  // clear previous shortest path points
  vtkPoints *newEdges = self->GetNewEdges();
  vtkPoints *newPixels = self->GetNewPixels();
  newEdges->Reset();
  newPixels->Reset();
  vtkPoints *tempPixels = vtkPoints::New();

  // current spot on path of arrows
  int traceX = end[0];
  int traceY = end[1];

  // current pixel to color in output image
  // (not same as trace since trace is along pixel edges
  // and outlines the area to color)
  // coloring assumes clockwise segmentation.
  int colorX, colorY;

  // Insert first points into lists
  //newEdges->InsertNextPoint(traceX,traceY,0);
  newEdges->InsertNextPoint(traceX,traceY,Dir(traceX,traceY));
  colorX = traceX + color[Dir(traceX,traceY)][0];
  colorY = traceY + color[Dir(traceX,traceY)][1];
  tempPixels->InsertNextPoint(colorX,colorY,0);

  // follow "arrows" backwards to the start point
  while (traceX!=start[0] || traceY!=start[1])
    {
      if (self->GetVerbose() > 2) 
	{
	  cout <<"("<<traceX<<","<<traceY<<")"<<"  ("<<colorX<<","<<colorY<<")"<<endl;
	}

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
      //newEdges->InsertNextPoint(traceX,traceY,0);
      newEdges->InsertNextPoint(traceX,traceY,Dir(traceX,traceY));

    } // end while

  if (self->GetVerbose() > 0) 
    {
      cout << "(" << traceX << "," << traceY << ")" << endl;
    }  

  // fix (reverse) the order of the pixels list.
  int numPoints = tempPixels->GetNumberOfPoints();
  newPixels->SetNumberOfPoints(numPoints);
  float *point;
  int count = 0;
  for (int i=numPoints-1; i>=0; i--)
    {
      point = tempPixels->GetPoint(i);
      newPixels->SetPoint(count,point);
      count++;
    }  

  // ----------------  Output Image  ------------------ //
  // draw points over image
  T outLabel = (T)self->GetLabel();
  numPoints = newPixels->GetNumberOfPoints();
  // confuses compilers
  //for (int i=0; i<numPoints; i++)
  for (i=0; i<numPoints; i++)
    {
      //cout << ".";
      point = newPixels->GetPoint(i);
      //cout << (int)point[0] + ((int)point[1])*sizeX << endl;
      outPtr[(int)point[0] + ((int)point[1])*sizeX] = outLabel;
    }

  // draw previously chosen contour over image
  vtkPoints *contour = self->GetContourPixels();
  numPoints = contour->GetNumberOfPoints();
  // confuses compilers
  //for (int i=0; i<numPoints; i++)
  for (i=0; i<numPoints; i++)
    {
      //cout << ".";
      point = contour->GetPoint(i);
      //cout << (int)point[0] + ((int)point[1])*sizeX << endl;
      outPtr[(int)point[0] + ((int)point[1])*sizeX] = outLabel;
    }  


  // ------------- test --------------
  numPoints = newEdges->GetNumberOfPoints();
  // confuses compilers
  //for (int i=0; i<numPoints; i++)
  for (i=0; i<numPoints; i++)
    {
      //cout << ".";
      point = newEdges->GetPoint(i);
      //cout << (int)point[0] + ((int)point[1])*sizeX << endl;
      //outPtr[(int)point[0] + ((int)point[1])*sizeX] += 10;
      // color depending on direction followed...
      //outPtr[(int)point[0] + ((int)point[1])*sizeX] = (T)((point[2]+1)*2);
    }
  // ------------- end test --------------

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
void vtkImageLiveWire::Execute(vtkImageData **inData, 
			       vtkImageData *outData)
{
  // if EndPoint is the same, do nothing.
  // Lauren what does this do to the output?
  if (this->EndPoint[0] == this->PrevEndPoint[0] &&
      this->EndPoint[1] == this->PrevEndPoint[1]) 
    {
      cout << "Lauren end point the SAME" << endl;
      return;
    }
  this->PrevEndPoint[0] = this->EndPoint[0];
  this->PrevEndPoint[1] = this->EndPoint[1];

  int x1;

  // Single component input is required
  for (int i = 0; i < this->NumberOfInputs; i++)
    {
      x1 = this->GetInput(i)->GetNumberOfScalarComponents();
      if (x1 != 1) {
	vtkErrorMacro(<<"Input " << i << "  has "<<x1<<" instead of 1 scalar component.");
	return;
      }
    }

  // Lauren check this
  // 2D input is required

  void *inPtr[5], *outPtr;

  inPtr[0] = inData[0]->GetScalarPointerForExtent(inData[0]->GetExtent());
  inPtr[1] = inData[1]->GetScalarPointerForExtent(inData[1]->GetExtent());
  inPtr[2] = inData[2]->GetScalarPointerForExtent(inData[2]->GetExtent());
  inPtr[3] = inData[3]->GetScalarPointerForExtent(inData[3]->GetExtent());
  inPtr[4] = inData[4]->GetScalarPointerForExtent(inData[4]->GetExtent());

  outPtr = outData->GetScalarPointerForExtent(outData->GetExtent());
  
  switch (inData[0]->GetScalarType())
    {
    case VTK_DOUBLE:
      vtkImageLiveWireExecute(this, inData, (double **)(inPtr), outData, (double *)(outPtr));
      break;
    case VTK_FLOAT:
      vtkImageLiveWireExecute(this, inData, (float **)(inPtr), outData, (float *)(outPtr));
      break;
    case VTK_LONG:
      vtkImageLiveWireExecute(this, inData, (long **)(inPtr), outData, (long *)(outPtr));
      break;
    case VTK_UNSIGNED_LONG:
      vtkImageLiveWireExecute(this, inData, (unsigned long **)(inPtr), outData, (unsigned long *)(outPtr));
      break;
    case VTK_INT:
      vtkImageLiveWireExecute(this, inData, (int **)(inPtr), outData, (int *)(outPtr));
      break;
    case VTK_UNSIGNED_INT:
      vtkImageLiveWireExecute(this, inData, (unsigned int **)(inPtr), outData, (unsigned int *)(outPtr));
      break;
    case VTK_SHORT:
      vtkImageLiveWireExecute(this, inData, (short **)(inPtr), outData, (short *)(outPtr));
      break;
    case VTK_UNSIGNED_SHORT:
      vtkImageLiveWireExecute(this, inData, (unsigned short **)(inPtr), outData, (unsigned short *)(outPtr));
      break;
    case VTK_CHAR:
      vtkImageLiveWireExecute(this, inData, (char **)(inPtr), outData, (char *)(outPtr));
      break;
    case VTK_UNSIGNED_CHAR:
      vtkImageLiveWireExecute(this, inData, (unsigned char **)(inPtr), outData, (unsigned char *)(outPtr));
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
      vtkErrorMacro("ExecuteInformation: Inputs are not the same size.");
      return;
    }
}

void vtkImageLiveWire::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkImageMultipleInputFilter::PrintSelf(os,indent);
}



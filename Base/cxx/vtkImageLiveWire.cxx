
#include "vtkImageLiveWire.h"
#include "stdlib.h"
#include "vtkObjectFactory.h"
#include <math.h>


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
  memset(this->StartPoint, 0, 2*sizeof(int));
  memset(this->EndPoint, 0, 2*sizeof(int));

  this->MaxEdgeCost = 255;

  this->ContourPoints = vtkImageDrawROI::New();

  this->Q = NULL;
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
// Description:
// This templated function executes the filter for any type of data.
template <class T>
static void vtkImageLiveWireExecute(vtkImageLiveWire *self,
				     vtkImageData **inDatas, T **inPtrs,
				     vtkImageData *outData)
{
  // Lauren output something
  outData->CopyAndCastFrom(inDatas[0], inDatas[0]->GetExtent());

  int *extent = inDatas[0]->GetWholeExtent();
  int numrows = extent[1] - extent[0] + 1;
  int numcols = extent[3] - extent[2] + 1;
  int numpix = numrows*numcols;

  // data structures (see IEEE Trans Med Img Jan 2000, Live Wire on the Fly)
  if (!self->Q)
    {
      self->Q = new CircularQueue(self->GetMaxEdgeCost(), numrows, numcols);
    }
  CircularQueue *Q = self->Q;

  // -------------- INIT should not happen in here ------------- //
  // cc is the cumulative cost from StartPoint to each pixel
  float cc[numrows][numcols];
  for (int i = 0; i < numrows; i++) {
    for (int j = 0; j < numcols; j++) {
      cc[i][j] = FLT_MAX;
    }
  }

  // dir is the direction the optimal path takes through each pixel.
  int *dir = new int[numpix];  
  enum {NONE, UP, DOWN, RIGHT, LEFT};
  memset(dir, NONE, numpix*sizeof(int));

  // Lauren this should be a member of the class
  // L is the list of edges ("bels") which have already been processed
  int *L = new int[numpix];
  memset(L, 0, numpix*sizeof(int));

  // Lauren this should be a member of the class
  // set for the entire contour search, then clear btwn slices...)
  // B is list of edges ("bels") on the contour already
  int *B = new int[numpix];
  memset(B, 0, numpix*sizeof(int));

  // Lauren boolean arrays should not be ints.
  
  // Dijkstra
  int *start = self->GetStartPoint();
  // cumulative cost at StartPoint = 0;
  cc[start[0]][start[1]] = 0;

  // Insert StartPoint into zero-length path bucket of Q
  int bucket = 0;
  Q->Insert(bucket, self->GetStartPoint());

  // current search bucket in Q
  float currentCC = 0; // or init to cc[start point]

  // -------------- end INIT should not happen in here ------------- //


  int *end = self->GetEndPoint();
  // while end point not in L do
  while (L[end[0]+end[1]*numcols] == 0) 
    {

      // get min vertex from Q
      bucket = Q->FindMinBucket(currentCC);
      ListElement *min = Q->GetListElement(bucket);

      //printf("chosen bucket %d, cc %f\n", bucket, currentCC);
    
      // put it into L, the already looked at list
      int coordX = min->Coord[0];
      int coordY = min->Coord[1];
      L[coordX+coordY*numcols] = 1;
      printf("add to L: %d %d\n", coordX, coordY);
      
      // remove it from Q (and list, A, in Q's bucket)
      Q->Remove(min);

      // update the current path cost
      currentCC = cc[coordX][coordY];

      // check out its 4 neighbors
      float oldCC, tempCC;
      vtkImageData *topEdge, *rightEdge;
      topEdge = self->GetTopEdges();
      rightEdge = self->GetRightEdges();
      T *topEdgeVal = (T*)topEdge->GetScalarPointerForExtent(topEdge->GetExtent());
      T *rightEdgeVal = (T*)rightEdge->GetScalarPointerForExtent(rightEdge->GetExtent());
      int x,y;

      int neighbors[4][2] = {{0,1},{1,0},{-1,0},{0,-1}};
      T* edges[4] = {rightEdgeVal, topEdgeVal, rightEdgeVal, topEdgeVal};
      T* edge;
  
      for (int n=0; n<4; n++) 
	{
	  x = coordX + neighbors[n][0];
	  y = coordY + neighbors[n][1];

	  if (y < numrows && x < numcols && x > 0 && y > 0) 
	    {
	      oldCC = cc[x][y];
	      // Lauren use the proper edge map
	      edge = edges[n];
	      tempCC = currentCC + edge[(x) + y*numcols];
	      if (tempCC < oldCC) 
		{
		  cc[x][y] = tempCC;
		  dir[x + y*numcols] = UP;
		  //printf("updated CC value from %f to %f\n", oldCC, tempCC);
		  // remove neighbor from Q if in it
		  Q->Remove(x,y);
		  // then put it in the proper place
		  // Lauren make this part of the Q class
		  bucket = (int)fmodf(cc[x][y], self->GetMaxEdgeCost()+1);
		  //printf("bucket %d\n",bucket);
		  Q->Insert(bucket, x,y);

		  printf("  put %d %d in Q[%d]: %f \n", x, y,bucket,tempCC);
		}
	      //	    printf("CC values from %f to %f\n", oldCC, tempCC);
      
	    }
	}
    }
  printf ("L at end point %d\n", L[end[0]+end[1]*numcols]); 
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
  int x1;

  // Single component input is required
  x1 = this->GetInput1()->GetNumberOfScalarComponents();
  if (x1 != 1) {
    vtkErrorMacro(<<"Input has "<<x1<<" instead of 1 scalar component.");
    return;
  }
  x1 = this->GetInput2()->GetNumberOfScalarComponents();
  if (x1 != 1) {
    vtkErrorMacro(<<"Input has "<<x1<<" instead of 1 scalar component.");
    return;
  }

  void *inPtr[2];

  // Lauren fix:
  // ERROR: In vtkImageData.cxx, line 1226
  // vtkImageData (0xaa470): GetScalarPointer: Pixel (0, 0, 0) not in memory.
  //  Current extent= (0, 255, 0, 255, 15, 15)

  inPtr[0] = inData[0]->GetScalarPointerForExtent(inData[0]->GetExtent());
  inPtr[1] = inData[1]->GetScalarPointerForExtent(inData[1]->GetExtent());
  
  switch (inData[0]->GetScalarType())
    {
    case VTK_DOUBLE:
      vtkImageLiveWireExecute(this, inData, (double **)(inPtr), outData);
      break;
    case VTK_FLOAT:
      vtkImageLiveWireExecute(this, inData, (float **)(inPtr), outData);
      break;
    case VTK_LONG:
      vtkImageLiveWireExecute(this, inData, (long **)(inPtr), outData);
      break;
    case VTK_UNSIGNED_LONG:
      vtkImageLiveWireExecute(this, inData, (unsigned long **)(inPtr), outData);
      break;
    case VTK_INT:
      vtkImageLiveWireExecute(this, inData, (int **)(inPtr), outData);
      break;
    case VTK_UNSIGNED_INT:
      vtkImageLiveWireExecute(this, inData, (unsigned int **)(inPtr), outData);
      break;
    case VTK_SHORT:
      vtkImageLiveWireExecute(this, inData, (short **)(inPtr), outData);
      break;
    case VTK_UNSIGNED_SHORT:
      vtkImageLiveWireExecute(this, inData, (unsigned short **)(inPtr), outData);
      break;
    case VTK_CHAR:
      vtkImageLiveWireExecute(this, inData, (char **)(inPtr), outData);
      break;
    case VTK_UNSIGNED_CHAR:
      vtkImageLiveWireExecute(this, inData, (unsigned char **)(inPtr), outData);
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
  
  // Make sure the Input has been set.
  // we require that input 1 be set.
  if ( ! inputs[0] || ! inputs[1])
    {
    vtkErrorMacro(<< "ExecuteInformation: Input is not set.");
    return;
    }
  
  in1Ext = inputs[0]->GetWholeExtent();
  in2Ext = inputs[1]->GetWholeExtent();

  if (in1Ext[0] != in2Ext[0] || in1Ext[1] != in2Ext[1] || 
      in1Ext[2] != in2Ext[2] || in1Ext[3] != in2Ext[3] || 
      in1Ext[4] != in2Ext[4] || in1Ext[5] != in2Ext[5])
    {
    vtkErrorMacro("ExecuteInformation: Input are not the same size.");
    return;
    }
}

void vtkImageLiveWire::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkImageTwoInputFilter::PrintSelf(os,indent);
}



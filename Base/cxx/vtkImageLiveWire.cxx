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
  this->CC = NULL;
  this->Dir = NULL;
  this->L = NULL;

  this->Verbose = 1;
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

  // ----------------  Data structures  ------------------ //

  // data structures (see IEEE Trans Med Img Jan 2000, Live Wire on the Fly)
  // Allocate structures.
  // Lauren each time a path is chosen by the user these should go away
  // and be created again for the next path. (when StartPoint changes)
  // So there should be a allocate/delete data structures function that is called then somehow.

  if (!self->Q)
    {
      self->Q = new circularQueue(numrows, numcols, self->GetMaxEdgeCost());
    }
  circularQueue *Q = self->Q;

  if (!self->CC)
    {
      self->CC = new array2D<int>(numrows, numcols, 65536);
    }
  array2D<int> &CC = (*self->CC);

  if (!self->Dir)
    {
      self->Dir = new array2D<int>(numrows, numcols, self->NONE);
    }
  array2D<int> &Dir = (*self->Dir);

  if (!self->L)
    {
      self->L = new array2D<bool>(numrows, numcols, false);
    }
  array2D<bool> &L = (*self->L);

  if (!self->B)
    {
      self->B = new array2D<bool>(numrows, numcols, false);
    }
  array2D<bool> &B = (*self->B);


  
  // ----------------  Dijkstra ------------------ //

  int *start = self->GetStartPoint();
  int *end = self->GetEndPoint();

  // current cumulative cost at StartPoint must be 0
  int currentCC = 0;
  CC(start[0],start[1]) = currentCC;

  // Insert StartPoint into (zero-length path bucket of) Q
  Q->Insert(start[0], start[1], currentCC);

  // while end point not in L keep checking out neighbors of current point
  while ( L(end[0],end[1]) == false) 
    {
      // get min vertex from Q 
      ListElement *min = Q->GetListElement(currentCC);

      // update the current location
      int currentX = min->Coord[0];
      int currentY = min->Coord[1];

      // update the current path cost
      currentCC = CC(currentX,currentY);

      // put vertex into L, the already looked at list
      L(currentX,currentY) = 1;

      // remove it from Q (and list, A, in Q's bucket)
      Q->Remove(min);

     
      if (self->GetVerbose() > 1) 
	{
	  cout << "----- CC: " << currentCC << " -----  (" << currentX 
	       << "," << currentY << ") ----- L: " << L(end[0],end[1]) 
	       <<" -----" << endl;
	}
    
      if (self->GetVerbose() == 1) 
	{
	  if (currentX == end[0] && currentY == end[1])
	    {
	      cout << "final point: (" << currentX << "," 
		   << currentY << ")  CC: " << currentCC << endl;
	    }
	}

      // check out its 4 neighbors
      int oldCC, tempCC;
      vtkImageData *topEdge, *rightEdge;
      topEdge = self->GetTopEdges();
      rightEdge = self->GetRightEdges();
      T *topEdgeVal = (T*)topEdge->GetScalarPointerForExtent(topEdge->GetExtent());
      T *rightEdgeVal = (T*)rightEdge->GetScalarPointerForExtent(rightEdge->GetExtent());
      int x,y;

      int neighbors[4][2] = {{0,1},{1,0},{-1,0},{0,-1}};
      // Lauren use the proper edge map
      T* edges[4] = {rightEdgeVal, topEdgeVal, rightEdgeVal, topEdgeVal};
      T* edge;
  
      for (int n=0; n<4; n++) 
	{
	  x = currentX + neighbors[n][0];
	  y = currentY + neighbors[n][1];

	  if (y < numrows && x < numcols && x > 0 && y > 0) 
	    {
	      oldCC = CC(x,y);
	      edge = edges[n];
	      // Lauren is edge image being accessed right?
	      // Lauren get int input!
	      tempCC = currentCC + (int)edge[(x) + y*numcols];

	      if (self->GetVerbose() > 2) 
		{
		  cout << "NEIGHBOR (" << x << "," << y << ") CC: " 
		       << tempCC << "," << oldCC << endl;
		}

	      if (tempCC < oldCC) 
		{
		  // lower the cumulative cost to reach this neighbor
		  CC(x,y) = tempCC;
		  // store new short path direction
		  Dir(x,y) = n+1;
		  // remove this neighbor from Q if in it
		  Q->Remove(x,y);
		  // then put it in the proper place in Q for its new cost
		  Q->Insert(x,y,CC(x,y));

		  if (self->GetVerbose() > 2)
		    {
		    cout << oldCC << " to  " << tempCC << endl;
		    }
		}
	    }
	}
    }
  return;
}


// Lauren just accept int as input!
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



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
#include "vtkImageLiveWireTester.h"
#include "vtkObjectFactory.h"
#include "vtkImageLiveWireEdgeWeights.h"

//------------------------------------------------------------------------------
vtkImageLiveWireTester* vtkImageLiveWireTester::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkImageLiveWireTester");
  if(ret)
    {
    return (vtkImageLiveWireTester*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkImageLiveWireTester;
}

//----------------------------------------------------------------------------
// Description:
// Constructor sets default values.
vtkImageLiveWireTester::vtkImageLiveWireTester()
{
  // must be set by user
  this->LiveWire = NULL;
  
  this->NumberOfEdgeFilters = 4;
  
  this->EdgeFilters = new vtkImageLiveWireEdgeWeights*[this->NumberOfEdgeFilters];

  for (int i = 0; i < this->NumberOfEdgeFilters; i++)
    {
      this->EdgeFilters[i] = vtkImageLiveWireEdgeWeights::New();
    }

}

vtkImageLiveWireTester::~vtkImageLiveWireTester()
{
  if (this->LiveWire)
    {
      this->LiveWire->Delete();
    }
  
  if (this->EdgeFilters)
    {
      for (int i = 0; i < this->NumberOfEdgeFilters; i++)
	{
	  if (this->EdgeFilters[i])
	    {
	      this->EdgeFilters[i]->Delete();
	    }
	}
    }
  
  delete [] this->EdgeFilters;
}

//----------------------------------------------------------------------------
// Description:
// Pseudo-multiple-output filter.  Feeds 4 edge images to LiveWire, which
// should be next in pipeline and get this filter's output image too.
// So LiveWire will get: 
// input[0] : output of this filter (just matches filter's input)
// input[1] - input[4]: edges
//
static void vtkImageLiveWireTesterExecute(vtkImageLiveWireTester *self,
				     vtkImageData *inData, short *inPtr,
				     vtkImageData *outData, short *outPtr, 
				     int outExt[6])
{
  // feed the edge inputs into vtkImageLiveWire.
  // output of this filter is same as its input, and will be
  // fed to vtkImageLiveWire as output[0]

  if (!self->GetLiveWire())
    {
      cout << "ERROR in vtkImageLiveWire??: vtkImageLiveWire member not set."<< endl;
      return;
    }

  vtkImageLiveWire *liveWire = self->GetLiveWire();

  // ----------------  Make edge images  ------------------ //

  int numEdges = self->GetNumberOfEdgeFilters();
  vtkImageLiveWireEdgeWeights ** edgeFilters = self->GetEdgeFilters();

  // Lauren max edge cost of edge filters and live wire need to match
  for (int i = 0; i < numEdges; i++)
    {
      edgeFilters[i]->SetInput(inData);
      // Lauren fix!!!!!!!!1
      edgeFilters[i]->SetNeighborhoodToLine(2);
      edgeFilters[i]->Update();
    }

  liveWire->SetTopEdges(edgeFilters[0]->GetOutput());
  liveWire->SetBottomEdges(edgeFilters[1]->GetOutput());
  liveWire->SetRightEdges(edgeFilters[2]->GetOutput());
  liveWire->SetLeftEdges(edgeFilters[3]->GetOutput());

//    topEdge->SetInput(inData);
//    bottomEdge->SetInput(inData);
//    rightEdge->SetInput(inData);
//    leftEdge->SetInput(inData);

//    topEdge->SetNeighborhoodToLine(2);
//    bottomEdge->SetNeighborhoodToLine(2);
//    rightEdge->SetNeighborhoodToLine(2);
//    leftEdge->SetNeighborhoodToLine(2);

//    topEdge->Update();
//    bottomEdge->Update();
//    rightEdge->Update();
//    leftEdge->Update();

//    liveWire->SetTopEdges(topEdge->GetOutput());
//    liveWire->SetBottomEdges(bottomEdge->GetOutput());
//    liveWire->SetRightEdges(rightEdge->GetOutput());
//    liveWire->SetLeftEdges(leftEdge->GetOutput());

  // Lauren do this directly??
  liveWire->SetStartPoint(self->GetStartPoint());
  liveWire->SetEndPoint(self->GetEndPoint());

  // Output is the same as input
  outData->CopyAndCastFrom(inData, inData->GetExtent());  

  return;
}


//----------------------------------------------------------------------------
// Description:
// This method is passed a input and output data, and executes the filter
// algorithm to fill the output from the input.
// It just executes a switch statement to call the correct function for
// the datas data types.
void vtkImageLiveWireTester::Execute(vtkImageData *inData, vtkImageData *outData)
{
  int outExt[6], id=0, s;
  outData->GetWholeExtent(outExt);
  void *inPtr = inData->GetScalarPointerForExtent(outExt);
  void *outPtr = outData->GetScalarPointerForExtent(outExt);

  int x1;

  x1 = GetInput()->GetNumberOfScalarComponents();
  if (x1 != 1) 
  {
    vtkErrorMacro(<<"Input has "<<x1<<" instead of 1 scalar component.");
    return;
  }

  /* Need short data */
  s = inData->GetScalarType();
  if (s != VTK_SHORT) 
  {
    vtkErrorMacro("Input scalars are type "<<s 
      << " instead of "<<VTK_SHORT);
    return;
  }

  vtkImageLiveWireTesterExecute(this, inData, (short *)inPtr, 
          outData, (short *)(outPtr), outExt);
}

void vtkImageLiveWireTester::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkImageToImageFilter::PrintSelf(os,indent);
  

}

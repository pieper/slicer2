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
#include "vtkImageDrawROI.h"
#include "vtkImageSimpleEdge.h"
#include "vtkImageCast.h"
#include "vtkImageGradientMagnitude.h"

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
  this->LiveWire = NULL;
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

  vtkImageLiveWire *LiveWire = self->GetLiveWire();

  // ----------------  Make edge images  ------------------ //

  // Lauren this needs to use another filter that can do any 
  // kind of cool variation on edge weighting...
  // Lauren Filters should be set by user, and their parameters too!

//    vtkImageSimpleEdge *topEdge, *bottomEdge, *rightEdge, *leftEdge;
//    topEdge = vtkImageSimpleEdge::New();
//    bottomEdge = vtkImageSimpleEdge::New();
//    rightEdge = vtkImageSimpleEdge::New();
//    leftEdge = vtkImageSimpleEdge::New();

//    topEdge->SetKernelToVertical();
//    rightEdge->SetKernelToHorizontal();

  vtkImageGradientMagnitude *topEdge, *bottomEdge, *rightEdge, *leftEdge;
  topEdge = vtkImageGradientMagnitude::New();
  bottomEdge = vtkImageGradientMagnitude::New();
  rightEdge = vtkImageGradientMagnitude::New();
  leftEdge = vtkImageGradientMagnitude::New();

  topEdge->SetInput(inData);
  bottomEdge->SetInput(inData);
  rightEdge->SetInput(inData);
  leftEdge->SetInput(inData);

  topEdge->Update();
  bottomEdge->Update();
  rightEdge->Update();
  leftEdge->Update();

  vtkImageCast * topCast = vtkImageCast::New();
  vtkImageCast * bottomCast = vtkImageCast::New();
  vtkImageCast * rightCast = vtkImageCast::New();
  vtkImageCast * leftCast = vtkImageCast::New();

  topCast->SetInput(topEdge->GetOutput());
  bottomCast->SetInput(bottomEdge->GetOutput());
  rightCast->SetInput(rightEdge->GetOutput());
  leftCast->SetInput(leftEdge->GetOutput());

  topCast->SetOutputScalarTypeToInt();
  bottomCast->SetOutputScalarTypeToInt();
  rightCast->SetOutputScalarTypeToInt();
  leftCast->SetOutputScalarTypeToInt();

//    LiveWire->SetTopEdges(topEdge->GetOutput());
//    LiveWire->SetBottomEdges(bottomEdge->GetOutput());
//    LiveWire->SetRightEdges(rightEdge->GetOutput());
//    LiveWire->SetLeftEdges(leftEdge->GetOutput());

  topCast->Update();
  bottomCast->Update();
  rightCast->Update();
  leftCast->Update();

  LiveWire->SetTopEdges(topCast->GetOutput());
  LiveWire->SetBottomEdges(bottomCast->GetOutput());
  LiveWire->SetRightEdges(rightCast->GetOutput());
  LiveWire->SetLeftEdges(leftCast->GetOutput());

  LiveWire->SetStartPoint(self->GetStartPoint());
  LiveWire->SetEndPoint(self->GetEndPoint());

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

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
#include <stdlib.h>
#include "vtkImageFrameSource.h"
#include "vtkObjectFactory.h"

//----------------------------------------------------------------------------
vtkImageFrameSource* vtkImageFrameSource::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkImageFrameSource");
  if(ret)
    {
    return (vtkImageFrameSource*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkImageFrameSource;
}

//----------------------------------------------------------------------------
vtkImageFrameSource::vtkImageFrameSource()
{
    this->WholeExtent[0] = this->WholeExtent[1] = this->WholeExtent[2] = 0;
    this->WholeExtent[3] = this->WholeExtent[4] = this->WholeExtent[5] = 0;
    this->SetExtent(0, 255, 0, 255);
    this->RenderWindow = NULL;
}

//----------------------------------------------------------------------------
vtkImageFrameSource::~vtkImageFrameSource()
{
    vtkErrorMacro(<<"this->RenderWindow = " << this->RenderWindow << ", if != NULL, will UnRegister it\n");
  // We must UnRegister any object that has a vtkSetObjectMacro
  if (this->RenderWindow != NULL) 
  {
    this->RenderWindow->UnRegister(this);
  }
}

void vtkImageFrameSource::PrintSelf(ostream& os, vtkIndent indent)
{
    vtkImageSource::PrintSelf(os,indent);
  
    os << indent << "WholeExtent: "
        << this->WholeExtent[1]-this->WholeExtent[0]+1 << "x"
        << this->WholeExtent[3]-this->WholeExtent[2]+1 << "x"
        << this->WholeExtent[5]-this->WholeExtent[4]+1 << "\n";

  // vtkSetObjectMacro
  os << indent << "RenderWindow: " << this->RenderWindow << "\n";
  if (this->RenderWindow)
  {
    this->RenderWindow->PrintSelf(os,indent.GetNextIndent());
  }
}

//----------------------------------------------------------------------------
void vtkImageFrameSource::SetExtent(int xMin, int xMax, int yMin, int yMax)
{
    if (this->WholeExtent[0] != xMin || this->WholeExtent[1] != xMax ||
        this->WholeExtent[2] != yMin || this->WholeExtent[2] != yMax)
    {
        this->WholeExtent[0] = xMin;
        this->WholeExtent[1] = xMax;
        this->WholeExtent[2] = yMin;
        this->WholeExtent[3] = yMax;
        this->WholeExtent[4] = 0;
        this->WholeExtent[5] = 0;
        this->Modified();
    }
}

//----------------------------------------------------------------------------
void vtkImageFrameSource::ExecuteInformation()
{
  vtkImageData *output = this->GetOutput();
 
  output->SetWholeExtent(this->WholeExtent);
  output->SetScalarType(VTK_UNSIGNED_CHAR);
  output->SetNumberOfScalarComponents(3);
}

//----------------------------------------------------------------------------
void vtkImageFrameSource::Execute(vtkImageData *data)
{
    int x1, y1, x2, y2, idxX, idxY, w, h;
    int wOut, hOut, wIn, hIn, nc, nb;
    int outIncX, outIncY, outIncZ;
    int *size;
    int *outExt;
    unsigned char *frame, *inPtr, *outPtr;
  
    if (this->RenderWindow == NULL) {
        vtkErrorMacro("Execute: This source requires a vtkRenderWindow.");
        return;
    }
    if (data->GetScalarType() != VTK_UNSIGNED_CHAR) {
        vtkErrorMacro("Execute: This source outputs unsigned char data not #"
            << data->GetScalarType());
        return;
    }
  
    // find the region to loop over
    outExt = data->GetExtent();
    hOut = outExt[3] - outExt[2] + 1;
    wOut = outExt[1] - outExt[0] + 1;
    nc = data->GetNumberOfScalarComponents();
  
    // Get increments to march through data 
    data->GetContinuousIncrements(outExt, outIncX, outIncY, outIncZ);
    outPtr = (unsigned char*) data->GetScalarPointer();

    // Get Frame buffer from center of render window
    size = this->RenderWindow->GetSize();
    wIn = size[0];
    hIn = size[1];

    // If input is smaller than output, then the output will need to
    // be zero padded.
    // If input is larger than output, then take the output from the
    // center of the input.

    // Input smaller
    if (wIn < wOut) {
        x1 = 0;
        x2 = wIn-1;
    } 
    // Input larger
    else {
        x1 = wIn/2 - wOut/2;
        x2 = x1 + wOut - 1; 
    }

    // Input smaller
    if (hIn < hOut) {
        y1 = 0;
        y2 = hIn-1;
    }
    // Input larger
    else {
        y1 = hIn/2 - hOut/2;
        y2 = y1 + hOut - 1; 
    }

    w = x2 - x1 + 1;
    h = y2 - y1 + 1;

    // Allocate and return frame data
    frame = this->RenderWindow->GetPixelData(x1, y1, x2, y2, 1);
    inPtr = frame;
    nb = sizeof(unsigned char)*nc;

    // March thru output pixels.
    // Wherever the output is outside the input, place 0's.
    // Else, place input.
    for (idxY = 0; idxY < hOut; idxY++)
    {
        for (idxX = 0; idxX < wOut; idxX++)
        {
            if (idxX < w && idxY < h)
            {
                memcpy(outPtr, inPtr, nb);
                outPtr += nc;
                inPtr += nc;
            } 
            else {
                memset(outPtr, 0, nb);
                outPtr += nc;
            }
        }
    }

    // I am responsible for deleting the buffer created by the RenderWindow
    delete [] frame;
    frame = NULL;
    inPtr = NULL;
}


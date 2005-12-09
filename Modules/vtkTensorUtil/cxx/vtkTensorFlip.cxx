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


#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkFloatArray.h"
#include "vtkTensorFlip.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"


//----------------------------------------------------------------------------
vtkTensorFlip* vtkTensorFlip::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkTensorFlip");
  if(ret)
    {
    return (vtkTensorFlip*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkTensorFlip;
}


//----------------------------------------------------------------------------
vtkTensorFlip::vtkTensorFlip()
{
}



//----------------------------------------------------------------------------
// 
void vtkTensorFlip::ExecuteInformation(vtkImageData *inData, 
                         vtkImageData *outData)
{
  int ext[6];

  inData->GetWholeExtent(ext);
  outData->SetWholeExtent(ext);
}

//----------------------------------------------------------------------------
// This templated function executes the filter for any type of data.
// this is for when only tensors are present
template <class D>
static void vtkTensorFlipExecute(vtkTensorFlip *self, int ext[6],
                       vtkImageData *inData, 
                       vtkImageData *outData, D *outPtr, int id)
{
  int num0, num1, num2;
  int idx0, idx1, idx2;
  int outInc0, outInc1, outInc2;
  unsigned long count = 0;
  unsigned long target;

  vtkDataArray *inTensors;
  vtkDataArray *outTensors;
  vtkFloatingPointType inT[3][3];
  vtkFloatingPointType outT[3][3];

  int ptId;

  // input tensors come from the inData
  inTensors = inData->GetPointData()->GetTensors();
  // output tensors are an ivar that gets put on the output later (avoids default overwrite by pipeline)
  outTensors = (vtkDataArray *) self->GetOutTensors();


  //Raul: Bad ptId inizialization
  // "GetTensorPointerForExtent" (get start spot in point data)
  // This is the id in the input and output datasets.
  //ptId = ext[0] + ext[2]*(ext[1]-ext[0]) + ext[4]*(ext[3]-ext[2]);

  int outInc[3],outFullUpdateExt[6];
  self->GetOutput()->GetIncrements(outInc);
  self->GetOutput()->GetUpdateExtent(outFullUpdateExt); //We are only working over the update extent
  ptId = ((ext[0] - outFullUpdateExt[0]) * outInc[0]
         + (ext[2] - outFullUpdateExt[2]) * outInc[1]
         + (ext[4] - outFullUpdateExt[4]) * outInc[2]);

  // Get the full size of the output so we can mirror the Y axis
  int *outFullDims = outData->GetDimensions();
  int ptIdOut;

  // Get information to march through data 
  outData->GetContinuousIncrements(ext, outInc0, outInc1, outInc2);
  num0 = ext[1] - ext[0] + 1;
  num1 = ext[3] - ext[2] + 1;
  num2 = ext[5] - ext[4] + 1;
  
  target = (unsigned long)(num2*num1/50.0);
  target++;

  // Loop through ouput pixels
  for (idx2 = 0; idx2 < num2; ++idx2)
    {
    for (idx1 = 0; !self->AbortExecute && idx1 < num1; ++idx1)
      {
      if (!id) 
        {
        if (!(count%target))
          {
          self->UpdateProgress(count/(50.0*target));
          }
        count++;
        }

      //
      // cacluate the corresponding point index for the mirror relative to the full
      // input image (in case the output extent is just a small window)
      //
      int sliceSize = outFullDims[0] * outFullDims[1];
      int ptIdSlice = ptId / sliceSize;
      int sliceStartId = ptIdSlice * sliceSize;
      int ptIdInSlice = (ptId - sliceStartId);
      int ptIdRow = ptIdInSlice / outFullDims[0];
      int rowStartId = sliceStartId + ptIdRow * outFullDims[0];

      int outRow = outFullDims[1] - ptIdRow -1;
      int outRowId = sliceStartId + (outRow * outFullDims[0]);
      ptIdOut = outRowId + ext[0];

      for (idx0 = 0; idx0 < num0; ++idx0)
        {
        inTensors->GetTuple(ptId,(vtkFloatingPointType *)inT);
        for (int j=0; j<3; j++)
          {
          for (int i=0; i<3; i++)
            {
            outT[i][j] = inT[i][j];
            }
          }
        outT[0][1] = -inT[0][1];
        outT[0][2] = -inT[0][2];
        outT[1][0] = -inT[1][0];
        outT[2][0] = -inT[2][0];

          // set the output tensor to the calculated one
        outTensors->SetTuple(ptIdOut,(vtkFloatingPointType *)outT);
        
        ptId += 1;
        ptIdOut += 1;
        }
    ptId += outInc1;
    }
  ptId += outInc2;
  }
}

//----------------------------------------------------------------------------
// Replace superclass Execute with a function that allocates tensors
// as well as scalars.  This gets called before multithreader starts
// (after which we can't allocate, for example in ThreadedExecute).
// Note we return to the regular pipeline at the end of this function.
void vtkTensorFlip::ExecuteData(vtkDataObject *out)
{
  vtkImageData *output = vtkImageData::SafeDownCast(out);

  // set extent so we know how many tensors to allocate
  output->SetExtent(output->GetUpdateExtent());

  // allocate output tensors -- save them in the 
  // instance variable for calculation by the threads
  // otherwise the superclass will overwrite them
  vtkFloatArray* data = vtkFloatArray::New();
  int* dims = output->GetDimensions();
  data->SetNumberOfComponents(9);
  data->SetNumberOfTuples(dims[0]*dims[1]*dims[2]);
  this->OutTensors = data;

  // jump back into normal pipeline: call standard superclass method here
  this->vtkImageToImageFilter::ExecuteData(out);

  output->GetPointData()->SetTensors(data);
  data->Delete();
}

//----------------------------------------------------------------------------
// This method is passed a input and output datas, and executes the filter
// algorithm to fill the output from the inputs.
// It just executes a switch statement to call the correct function for
// the datas data types.
void vtkTensorFlip::ThreadedExecute(vtkImageData *inData, 
                      vtkImageData *outData,
                      int outExt[6], int id)
{
  void *outPtr;
  
  vtkDebugMacro(<< "Execute: inData = " << inData 
        << ", outData = " << outData);

  if (inData == NULL)
    {
      vtkErrorMacro(<< "Input must be specified.");
      return;
    }


  outPtr = outData->GetScalarPointerForExtent(outExt);
  
  switch (outData->GetScalarType())
    {
      // we set the output data scalar type depending on the op
      // already.  And we only access the input tensors
      // which are float.  So this switch statement on output
      // scalar type is sufficient.
      vtkTemplateMacro6(vtkTensorFlipExecute,
                this, outExt, inData, outData, 
                (VTK_TT *)(outPtr), id);
      default:
        vtkErrorMacro(<< "Execute: Unknown ScalarType");
        break;
    }
    return;
}

void vtkTensorFlip::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkImageToImageFilter::PrintSelf(os,indent);
}

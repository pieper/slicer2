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
#include "vtkTensorMask.h"
#include "vtkObjectFactory.h"
#include "vtkFloatArray.h"
#include "vtkImageData.h"
#include "vtkPointData.h"

//------------------------------------------------------------------------------
vtkTensorMask* vtkTensorMask::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkTensorMask");
  if(ret)
    {
      return (vtkTensorMask*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkTensorMask;
}

//----------------------------------------------------------------------------
vtkTensorMask::vtkTensorMask()
{

}

vtkTensorMask::~vtkTensorMask()
{

}



//----------------------------------------------------------------------------
// Replace superclass Execute with a function that allocates tensors
// as well as scalars.  This gets called before multithreader starts
// (after which we can't allocate, for example in ThreadedExecute).
// Note we return to the regular pipeline at the end of this function.
void vtkTensorMask::ExecuteData(vtkDataObject *out)
{
  vtkImageData *output = vtkImageData::SafeDownCast(out);

  // set extent so we know how many tensors to allocate
  output->SetExtent(output->GetUpdateExtent());

  // allocate output tensors
  vtkFloatArray* data = vtkFloatArray::New();
  int* dims = output->GetDimensions();
  data->SetNumberOfComponents(9);
  data->SetNumberOfTuples(dims[0]*dims[1]*dims[2]);
  output->GetPointData()->SetTensors(data);
  data->Delete();

  // jump back into normal pipeline: call standard superclass method here
  this->vtkImageMultipleInputFilter::ExecuteData(out);
}


//----------------------------------------------------------------------------
// This templated function executes the filter for any type of data.
// This is exactly the same as the superclass: default behavior
// when only scalars are present
template <class T, class D>
static void vtkTensorMaskExecute(vtkTensorMask *self, int ext[6],
                vtkImageData *in1Data, T *in1Ptr,
                vtkImageData *in2Data, D *in2Ptr,
                vtkImageData *outData, T *outPtr, int id)
{
  int num0, num1, num2, numC, pixSize;
  int idx0, idx1, idx2;
  int in1Inc0, in1Inc1, in1Inc2;
  int in2Inc0, in2Inc1, in2Inc2;
  int outInc0, outInc1, outInc2;
  T *maskedValue;
  vtkFloatingPointType *v;
  int nv;
  int maskState;
  unsigned long count = 0;
  unsigned long target;
  
  // create a masked output value with the correct length by cycling
  numC = outData->GetNumberOfScalarComponents();
  maskedValue = new T[numC];
  v = self->GetMaskedOutputValue();
  nv = self->GetMaskedOutputValueLength();
  for (idx0 = 0, idx1 = 0; idx0 < numC; ++idx0, ++idx1)
    {
      if (idx1 >= nv)
    {
      idx1 = 0;
    }
      maskedValue[idx0] = (T)(v[idx1]);
    }
  pixSize = numC * sizeof(T);
  maskState = self->GetNotMask();
  
  // Get information to march through data 
  in1Data->GetContinuousIncrements(ext, in1Inc0, in1Inc1, in1Inc2);
  in2Data->GetContinuousIncrements(ext, in2Inc0, in2Inc1, in2Inc2);
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
      for (idx0 = 0; idx0 < num0; ++idx0)
        {
          // Pixel operation
          if (*in2Ptr && maskState == 1)
        {
          memcpy(outPtr, maskedValue, pixSize);
        }
          else if ( ! *in2Ptr && maskState == 0)
        {
          memcpy(outPtr, maskedValue, pixSize);
        }
          else
        {
          memcpy(outPtr, in1Ptr, pixSize);
        }
    
          in1Ptr += numC;
          outPtr += numC;
          in2Ptr += 1;
        }
      in1Ptr += in1Inc1;
      in2Ptr += in2Inc1;
      outPtr += outInc1;
    }
      in1Ptr += in1Inc2;
      in2Ptr += in2Inc2;
      outPtr += outInc2;
    }
  
  delete [] maskedValue;
}


//----------------------------------------------------------------------------
// This templated function executes the filter for any type of data.
// this is for when only tensors are present
template <class D>
static void vtkTensorMaskExecuteTensor(vtkTensorMask *self, int ext[6],
                       vtkImageData *in1Data,
                       vtkImageData *in2Data, D *in2Ptr,
                       vtkImageData *outData, int id)
{
  int num0, num1, num2;
  int idx0, idx1, idx2;
  int in1Inc0, in1Inc1, in1Inc2;
  int in2Inc0, in2Inc1, in2Inc2;
  int outInc0, outInc1, outInc2;
  int maskState;
  unsigned long count = 0;
  unsigned long target;

  vtkDataArray *inTensors;
  vtkDataArray *outTensors;
  vtkFloatingPointType inT[3][3];
  vtkFloatingPointType outT[3][3];

  int ptId;

  // do we NOT the mask?
  maskState = self->GetNotMask();

  // input tensors
  inTensors = self->GetInput()->GetPointData()->GetTensors();
  // output tensors
  outTensors = self->GetOutput()->GetPointData()->GetTensors();


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
  
  // Get information to march through data 
  in1Data->GetContinuousIncrements(ext, in1Inc0, in1Inc1, in1Inc2);
  in2Data->GetContinuousIncrements(ext, in2Inc0, in2Inc1, in2Inc2);
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
      for (idx0 = 0; idx0 < num0; ++idx0)
        {
          inTensors->GetTuple(ptId,(vtkFloatingPointType *)inT);
          //outTensors->GetTuple(ptId,outT);

          // Pixel operation: clear or copy
          if (*in2Ptr && maskState == 1)
        {
          //outT->Initialize();
          for (int j=0; j<3; j++)
            {
              for (int i=0; i<3; i++)
            {
                   outT[i][j] = 0;
            }
            }
        }
          else if ( ! *in2Ptr && maskState == 0)
        {
          //outT->Initialize();
          for (int j=0; j<3; j++)
            {
              for (int i=0; i<3; i++)
            {
                   outT[i][j] = 0;
            }
            }
        }
          else
        {
          //outT->DeepCopy(inT);
          for (int j=0; j<3; j++)
            {
              for (int i=0; i<3; i++)
            {
                   outT[i][j] = inT[i][j];
            }
            }
        }

          // set the output tensor to the calculated one
          outTensors->SetTuple(ptId,(vtkFloatingPointType *)outT);
          
          ptId += 1;
          in2Ptr += 1;
        }
      ptId += outInc1;
    }
      ptId += outInc2;
    }
}



//----------------------------------------------------------------------------
// This method is passed a input and output Datas, and executes the filter
// algorithm to fill the output from the inputs.
// It just executes a switch statement to call the correct function for
// the Datas data types.
void vtkTensorMask::ThreadedExecute(vtkImageData **inData, 
                   vtkImageData *outData,
                   int outExt[6], int id)
{
  void *inPtr1;
  void *inPtr2;
  void *outPtr;
  int *tExt;
  vtkDataArray *inTensors;

  vtkDebugMacro(<< "Execute: inData = " << inData 
  << ", outData = " << outData);
  

  if (inData[0] == NULL)
    {
      vtkErrorMacro(<< "Input " << 0 << " must be specified.");
      return;
    }
  if (inData[1] == NULL)
    {
      vtkErrorMacro(<< "Input " << 1 << " must be specified.");
      return;
    }
  
  // input image
  inPtr1 = inData[0]->GetScalarPointerForExtent(outExt);
  // mask
  inPtr2 = inData[1]->GetScalarPointerForExtent(outExt);
  // output
  outPtr = outData->GetScalarPointerForExtent(outExt);
  // input tensors
  inTensors = this->GetInput()->GetPointData()->GetTensors();


  tExt = inData[1]->GetExtent();
  if (tExt[0] > outExt[0] || tExt[1] < outExt[1] || 
      tExt[2] > outExt[2] || tExt[3] < outExt[3] ||
      tExt[4] > outExt[4] || tExt[5] < outExt[5])
    {
      vtkErrorMacro("Mask extent not large enough");
      return;
    }
  
  if (inData[1]->GetNumberOfScalarComponents() != 1)
    {
      vtkErrorMacro("Masks can have one component");
    }
    
  if (inData[0]->GetScalarType() != outData->GetScalarType() ||
      (inData[1]->GetScalarType() != VTK_UNSIGNED_CHAR &&
       inData[1]->GetScalarType() != VTK_SHORT))
    {
      vtkErrorMacro(<< "Execute: image ScalarType (" 
      << inData[0]->GetScalarType() << ") must match out ScalarType (" 
      << outData->GetScalarType() << "), and mask scalar type (" 
      << inData[1]->GetScalarType() << ") must be unsigned char or short.");
      return;
    }

  // for now process scalars and tensors separately, 
  // since scalars are an afterthought, though this is slower.

  // do we have input tensors?
  if (inTensors) 
    {
      // call the execute code for tensors
      switch (inData[1]->GetScalarType())
    {  
    case VTK_UNSIGNED_CHAR:
      vtkTensorMaskExecuteTensor(this, outExt, inData[0], 
                     inData[1], 
                     (unsigned char *)(inPtr2),
                     outData, id);
      break;
    case VTK_SHORT:
      vtkTensorMaskExecuteTensor(this, outExt, inData[0], 
                     inData[1], 
                     (short *)(inPtr2),
                     outData, id);
      break;
    default:
      vtkErrorMacro(<< "Execute: Unknown ScalarType for mask input");
      return;
    }

    }

  // do we have input scalars?
  if (inPtr1) 
    {
      switch (inData[1]->GetScalarType())
    {  
    case VTK_UNSIGNED_CHAR:
      switch (inData[0]->GetScalarType())
        {
          vtkTemplateMacro9(vtkTensorMaskExecute, this, outExt, 
                inData[0], (VTK_TT *)(inPtr1), 
                inData[1], (unsigned char *)(inPtr2),
                outData, (VTK_TT *)(outPtr),id);
        default:
          vtkErrorMacro(<< "Execute: Unknown ScalarType");
          return;
        }
      break;
    case VTK_SHORT:
      switch (inData[0]->GetScalarType())
        {
          vtkTemplateMacro9(vtkTensorMaskExecute, this, outExt, 
                inData[0], (VTK_TT *)(inPtr1), 
                inData[1], (short *)(inPtr2),
                outData, (VTK_TT *)(outPtr),id);
        default:
          vtkErrorMacro(<< "Execute: Unknown ScalarType");
          return;
        }
      break;
    default:
      vtkErrorMacro(<< "Execute: Unknown ScalarType for mask input");
      return;
    }
    }
}

void vtkTensorMask::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkImageMask::PrintSelf(os,indent);
}


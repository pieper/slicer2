#include "vtkImageTensorDiffusion.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkObjectFactory.h"



//----------------------------------------------------------------------------
vtkImageTensorDiffusion* vtkImageTensorDiffusion::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkImageTensorDiffusion");
  if(ret)
    {
    return (vtkImageTensorDiffusion*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkImageTensorDiffusion;
}

//----------------------------------------------------------------------------
// Construct an instance of vtkImageTensorDiffusion fitler.
vtkImageTensorDiffusion::vtkImageTensorDiffusion()
{
  this->InputTensor = NULL;
  this->InputSource = NULL;

  this->S = 1;
  this->K = 0.0125;
  this->Da = 0.125;
  this->Db = 0.03125;
  this->InitialConcentration = 4;
  this->Test = 0;
}

//----------------------------------------------------------------------------
void vtkImageTensorDiffusion::SetNumberOfIterations(int num)
{
  this->vtkImageIterateFilter::SetNumberOfIterations(num);
}


// Lauren is this what's wanted?
//----------------------------------------------------------------------------
// This method computes the extent of the input region necessary to generate
// an output region.  Before this method is called "region" should have the 
// extent of the output region.  After this method finishes, "region" should 
// have the extent of the required input region.
void vtkImageTensorDiffusion::ComputeInputUpdateExtent(int inExt[6], 
                          int outExt[6])
{
  int idx;
  int *wholeExtent;
  
  wholeExtent = this->GetInput()->GetWholeExtent();

  inExt[4] = outExt[4];
  inExt[5] = outExt[5];
  
  for (idx = 0; idx < 2; ++idx)
    {
    inExt[idx*2] = outExt[idx*2] - 1;
    inExt[idx*2+1] = outExt[idx*2+1] + 1;
    
    // If the expanded region is out of the IMAGE Extent (grow min)
    if (inExt[idx*2] < wholeExtent[idx*2])
      {
      inExt[idx*2] = wholeExtent[idx*2];
      }
    // If the expanded region is out of the IMAGE Extent (shrink max)      
    if (inExt[idx*2+1] > wholeExtent[idx*2+1])
      {
      // shrink the required region extent
      inExt[idx*2+1] = wholeExtent[idx*2+1];
      }
    }
}




//----------------------------------------------------------------------------
// 
template <class T>
static void vtkImageTensorDiffusionExecute(vtkImageTensorDiffusion *self,
                      vtkImageData *inData, T *inPtr, 
                      vtkImageData *outData, int *outExt, 
                      T *outPtr, int id)
{
  // For looping though output (and input) pixels.
  int outMin0, outMax0, outMin1, outMax1, outMin2, outMax2;
  int idx0, idx1, idx2;
  int inInc0, inInc1, inInc2;
  int srcInc0, srcInc1, srcInc2;
  int outInc0, outInc1, outInc2;
  T *inPtr0, *inPtr1, *inPtr2;
  T *srcPtr, *srcPtr0, *srcPtr1, *srcPtr2;
  T *outPtr0, *outPtr1, *outPtr2;
  int wholeMin0, wholeMax0, wholeMin1, wholeMax1, wholeMin2, wholeMax2;
  //float n[9], M[9];
  float n[27], M[27];
  float tensor[3][3];
  int *tensorInc;
  int *tensorExt;
  int tenIncX, tenIncY, tenIncZ;
  vtkDataArray *tensors;
  int index;
  int useSource =0;
  srcInc0 = srcInc1 = srcInc2 =0;
  srcPtr = NULL;

  if (self->GetInputSource())
    useSource == 1;

  // Get information to march through data
  inData->GetIncrements(inInc0, inInc1, inInc2); 
  if (useSource)
    self->GetInputSource()->GetIncrements(srcInc0, srcInc1, srcInc2); 
  outData->GetIncrements(outInc0, outInc1, outInc2); 

  outMin0 = outExt[0];  outMax0 = outExt[1];
  outMin1 = outExt[2];  outMax1 = outExt[3];
  outMin2 = outExt[4];  outMax2 = outExt[5];
  self->GetInput()->GetWholeExtent(wholeMin0, wholeMax0, wholeMin1, wholeMax1,
                   wholeMin2, wholeMax2);
  
  //Lauren make sure extents are okay and also num components and types
  tensors = self->GetInputTensor()->GetPointData()->GetTensors();
 
 //Get extent from tensor data to initialize an index that runs over the extent
 //this thread wants.
 tensorInc = self->GetInputTensor()->GetIncrements();
 tensorExt = self->GetInputTensor()->GetExtent();
 self->GetInputTensor()->GetContinuousIncrements(outExt, tenIncX, tenIncY, tenIncZ);
 index = ((outExt[0] - tensorExt[0]) * tensorInc[0]
     + (outExt[2] - tensorExt[2]) * tensorInc[1]
     + (outExt[4] - tensorExt[4]) * tensorInc[2]);

  if (useSource)
    srcPtr =  (T*) self->GetInputSource()->GetScalarPointerForExtent(outExt);

  if (!id) 
    {
      if (!(self->GetIteration() %10)) 
    {
      cout <<  self->GetIteration() << endl;
    }
      self->UpdateProgress(self->GetIteration()/self->GetNumberOfIterations());
    }

  // simulation parameters
  float s = self->GetS();
  float k = self->GetK();
  float da = self->GetDa();
  float db = self->GetDb();
  float init = self->GetInitialConcentration();

  // for accessing neighbors
  int slice = (wholeMax0 - wholeMin0 + 1)*(wholeMax1 - wholeMin1 + 1);
  cout << "slice: " << slice << endl;

  inPtr2 = inPtr;
  srcPtr2 = srcPtr;
  outPtr2 = outPtr;
  for (idx2 = outMin2; idx2 <= outMax2; ++idx2)
    {

      inPtr1 = inPtr2;
      srcPtr1 = srcPtr2;
      outPtr1 = outPtr2;

      for (idx1 = outMin1; !self->AbortExecute && idx1 <= outMax1; ++idx1)
    {

      inPtr0 = inPtr1;
      srcPtr0 = srcPtr1;
      outPtr0 = outPtr1;

      for (idx0 = outMin0; idx0 <= outMax0; ++idx0)
        {
          float c;

          // use result of previous iteration (or input values)
          c = *inPtr0;
          
          // if off the edge pretend concentration is same there
          // neighbors independent of boundaries

          // previous slice
          // -----------------------------
          if ((idx2-1) > wholeMin2) {
        // if the previous slice is in the dataset
        // prev row
        n[0] = (idx0>wholeMin0)&&(idx1>wholeMin1) 
          ? (float)*(inPtr0-inInc0-inInc1-slice) : c;
        n[1] = (idx1>wholeMin1) ? (float)*(inPtr0-inInc1-slice) : c;
        n[2] = (idx1>wholeMin1)&&(idx0<wholeMax0) 
          ? (float)*(inPtr0-inInc1+inInc0-slice) : c;
        // this row
        n[3] = (idx0>wholeMin0) ? (float)*(inPtr0-inInc0-slice) : c;
        n[4] = (float)*(inPtr0-slice);
        n[5] = (idx0<wholeMax0) ? (float)*(inPtr0+inInc0-slice) : c;
        // next row (+inInc1)
        n[6] = (idx1<wholeMax1)&&(idx0>wholeMin0) 
          ? (float)*(inPtr0+inInc1-inInc0-slice) : c;
        n[7] = (idx1<wholeMax1) ? (float)*(inPtr0+inInc1-slice) : c;
        n[8] = (idx0<wholeMax0)&&(idx1<wholeMax1) 
          ? (float)*(inPtr0+inInc0+inInc1-slice) : c;
          } else {
        // just assign all to constant
        for (int i = 0; i<9; i++) {
          n[i] = c;
        }
          }

          // this slice
          // -----------------------------
          // prev row (-inInc1)
          n[9] = (idx0>wholeMin0)&&(idx1>wholeMin1) 
        ? (float)*(inPtr0-inInc0-inInc1) : c;
          n[10] = (idx1>wholeMin1) ? (float)*(inPtr0-inInc1) : c;
          n[11] = (idx1>wholeMin1)&&(idx0<wholeMax0) 
        ? (float)*(inPtr0-inInc1+inInc0) : c;
          // this row
          n[12] = (idx0>wholeMin0) ? (float)*(inPtr0-inInc0) : c;
          n[13] = c;
          n[14] = (idx0<wholeMax0) ? (float)*(inPtr0+inInc0) : c;
          // next row (+inInc1)
          n[15] = (idx1<wholeMax1)&&(idx0>wholeMin0) 
        ? (float)*(inPtr0+inInc1-inInc0) : c;
          n[16] = (idx1<wholeMax1) ? (float)*(inPtr0+inInc1) : c;
          n[17] = (idx0<wholeMax0)&&(idx1<wholeMax1) 
        ? (float)*(inPtr0+inInc0+inInc1) : c;
          
          // next slice
          // -----------------------------
          if ((idx2+1) < wholeMax2) {
        // if the next slice is in the dataset
        // prev row
        n[18] = (idx0>wholeMin0)&&(idx1>wholeMin1) 
          ? (float)*(inPtr0-inInc0-inInc1+slice) : c;
        n[19] = (idx1>wholeMin1) ? (float)*(inPtr0-inInc1+slice) : c;
        n[20] = (idx1>wholeMin1)&&(idx0<wholeMax0) 
          ? (float)*(inPtr0-inInc1+inInc0+slice) : c;
        // this row
        n[21] = (idx0>wholeMin0) ? (float)*(inPtr0-inInc0+slice) : c;
        n[22] = (float)*(inPtr0+slice);
        n[23] = (idx0<wholeMax0) ? (float)*(inPtr0+inInc0+slice) : c;
        // next row (+inInc1)
        n[24] = (idx1<wholeMax1)&&(idx0>wholeMin0) 
          ? (float)*(inPtr0+inInc1-inInc0+slice) : c;
        n[25] = (idx1<wholeMax1) ? (float)*(inPtr0+inInc1+slice) : c;
        n[26] = (idx0<wholeMax0)&&(idx1<wholeMax1) 
          ? (float)*(inPtr0+inInc0+inInc1+slice) : c;
          } else {
        // just assign all to constant
        for (int i = 18; i<27; i++) {
          n[i] = c;
        }
          }

          // Diffuse neighbors using tensors
          // -----------------------------
          tensors->GetTuple(index,(vtkFloatingPointType *)tensor);

          // construct mask M from tensor data
          float Dxx, Dyy, Dzz, Dxy, Dxz, Dyz, trace;
          
          // Grab components of the (symmetric) tensor
          Dxx = tensor[0][0];
          Dyy = tensor[1][1];
          Dzz = tensor[2][2];

          Dxy = tensor[1][0];
          Dxz = tensor[2][0];
          Dyz = tensor[2][1];

          trace = Dxx + Dyy + Dzz;

          // previous slice
          M[0] =  0;
          M[1] =  -Dyz/2;
          M[2] =  0;
          M[3] =  Dxz/2;
          M[4] =  Dzz;
          M[5] =  -Dxz/2;
          M[6] =  0;
          M[7] =  Dyz/2;
          M[8] =  0;

          // this slice
          M[9] = -Dxy/2;
          M[10] =  Dyy;
          M[11] =  Dxy/2;
          M[12] =  Dxx;
          M[13] = -2*trace;
          M[14] =  Dxx;
          M[15] =  Dxy/2;
          M[16] =  Dyy;
          M[17] = -Dxy/2;

          // next slice
          M[18] =  0;
          M[19] =  Dyz/2;
          M[20] =  0;
          M[21] =  -Dxz/2;
          M[22] =  Dzz;
          M[23] =  Dxz/2;
          M[24] =  0;
          M[25] =  -Dyz/2;
          M[26] =  0;

          // test isotropic diffusion: laplacian
          //              M[0] = M[2] = M[4] = M[6] = M[8] = 0;
          //              M[1] = M[3] = M[5] = M[7] = 1;
          //              M[4] = -4;

          float laplacian = 0;

          // Lauren do we need to scale the input tensors?
          for (int i = 0; i < 27; i++) 
          {
            laplacian += M[i]*n[i];
        }
          
          // if we are using uniform diffusion include diffusivity
          //laplacian *= da;

          // update rule 
          float delta;
          delta = s*(laplacian);
          *outPtr0 = (T) (c + delta);

          // if we have a source here let it flow
          if (useSource)
        *outPtr0 += (T) *srcPtr0;
          // count number of tensors
          index++;

          inPtr0 += inInc0;
          srcPtr0 += srcInc0;
          outPtr0 += outInc0;
        }
      inPtr1 += inInc1;
      srcPtr1 += srcInc1;
      outPtr1 += outInc1;
      index += tenIncY;
          //cout << index << endl;
    }
      // inInc uses components
      inPtr2 += inInc2;
      srcPtr2 += srcInc2;
      outPtr2 += outInc2;
      index += tenIncZ;
      //cout << index << endl;
    }

}





//----------------------------------------------------------------------------
// This method contains the first switch statement that calls the correct
// templated function for the input and output region types.
void vtkImageTensorDiffusion::ThreadedExecute(vtkImageData *inData, 
                        vtkImageData *outData, 
                        int outExt[6], int id)
{
  void *inPtr;
  void *outPtr = outData->GetScalarPointerForExtent(outExt);
  //vtkImageData *tempData;
  int inExt[6];
  
  // this filter expects that input is the same type as output.
  if (inData->GetScalarType() != outData->GetScalarType())
    {
    vtkErrorMacro(<< "Execute: input ScalarType, " << inData->GetScalarType()
      << ", must match out ScalarType " << outData->GetScalarType());
    return;
    }
  
  this->ComputeInputUpdateExtent(inExt, outExt); 

  inPtr = inData->GetScalarPointerForExtent(outExt);

  switch (inData->GetScalarType())
    {
    vtkTemplateMacro7(vtkImageTensorDiffusionExecute, this, inData,
                      (VTK_TT *)(inPtr), outData, outExt, 
                      (VTK_TT *)(outPtr), id);
    default:
      vtkErrorMacro(<< "Execute: Unknown ScalarType");
      return;
    }
}

void vtkImageTensorDiffusion::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkImageIterateFilter::PrintSelf(os,indent);

  os << indent << "S: " << this->S << "\n";
  os << indent << "K: " << this->K << "\n";
  os << indent << "Da: " << this->Da << "\n";
  os << indent << "Db: " << this->Db << "\n";
  os << indent << "InitialConcentration: " 
     << this->InitialConcentration << "\n";

}

